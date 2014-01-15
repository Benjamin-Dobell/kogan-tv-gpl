/*
 * Copyright (c) 2010-2011 MStar Semiconductor, Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MStar Semiconductor Inc. and be kept in strict confidence
 * ("MStar Confidential Information") by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MStar Confidential
 * Information is unlawful and strictly prohibited. MStar hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 * MStar's Audio Driver for ALSA.
 * Author: Darren Chen <darren.chen@mstarsemi.com>
 *
 */


/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/soundcard.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>

#include <sound/core.h>
#include <sound/asound.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/version.h>
#include <sound/control.h>
#include <sound/initval.h>

#include "mdrv_audio.h"


/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
#define _MAD_SND_PLATFORM    "Mstar-alsa"
#define _MAD_SND_DRIVER    "MStar ALSA Driver"
#define _MAD_SND_SHORTNAME    "MStar MAD"
#define _MAD_SND_LONGNAME    "MStar MPEG Audio Decoder"

#define _MAD_PCM_INSTANCE    "mad_pcm"
#define _MAD_PCM_NAME    "MStar MAD PCM"
#define _MAD_CONTROL_NAME    "MStar MAD Control"

#define _MAD_DEVICE    2
#define _MAD_MAX_PCM_BUFFER_SIZE    64*1024


/*
 * ============================================================================
 * Forward Declarations
 * ============================================================================
 */
#if 0
static void _mdrv_alsa_dump_runtime_structure(char *caller_function, struct snd_pcm_runtime *runtime);
#endif

static void _mdrv_alsa_timer_open(struct timer_list *timer, unsigned long data, void *func);
static void _mdrv_alsa_timer_close(struct timer_list *timer);
static int _mdrv_alsa_timer_reset(struct timer_list *timer, unsigned long data);
static int _mdrv_alsa_timer_update(struct timer_list *timer, unsigned long data, unsigned long time_interval);

static int _mdrv_alsa_set_default_hw(struct snd_pcm_hardware *target_hw, int direction);
static int _mdrv_alsa_set_default_constraint_list(struct snd_pcm_hw_constraint_list *target_constraint_list, int direction);
static int _mdrv_alsa_set_hw_by_constrain_list(struct snd_pcm_hardware *target_hw, struct snd_pcm_hw_constraint_list *target_constraint_list);

static int _mdrv_alsa_playback_open(struct snd_pcm_substream *substream);
static int _mdrv_alsa_playback_close(struct snd_pcm_substream *substream);
static int _mdrv_alsa_playback_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params);
static int _mdrv_alsa_playback_hw_free(struct snd_pcm_substream *substream);
static int _mdrv_alsa_playback_prepare(struct snd_pcm_substream *substream);
static int _mdrv_alsa_playback_trigger(struct snd_pcm_substream *substream, int cmd);
static snd_pcm_uframes_t _mdrv_alsa_playback_pointer(struct snd_pcm_substream *substream);
static int _mdrv_alsa_playback_copy(struct snd_pcm_substream *substream, int channel, snd_pcm_uframes_t pos, void __user *buf, snd_pcm_uframes_t count);
static void _mdrv_alsa_playback_monitor(unsigned long device_no);

static int _mdrv_alsa_capture_open(struct snd_pcm_substream *substream);
static int _mdrv_alsa_capture_close(struct snd_pcm_substream *substream);
static int _mdrv_alsa_capture_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params);
static int _mdrv_alsa_capture_hw_free(struct snd_pcm_substream *substream);
static int _mdrv_alsa_capture_prepare(struct snd_pcm_substream *substream);
static int _mdrv_alsa_capture_trigger(struct snd_pcm_substream *substream, int cmd);
static snd_pcm_uframes_t _mdrv_alsa_capture_pointer(struct snd_pcm_substream *substream);
static int _mdrv_alsa_capture_copy(struct snd_pcm_substream *substream, int channel, snd_pcm_uframes_t pos, void __user *buf, snd_pcm_uframes_t count);

#if 0 /* It's not in used for the moment, might be TODO */
static int _mdrv_alsa_dev_free(struct snd_device *dev);
static void _mdrv_alsa_runtime_free(struct snd_pcm_runtime *runtime);
#endif

#if 0 /* Not support this for the moment, might be TODO */
static int _mdrv_alsa_control_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo);
static int _mdrv_alsa_control_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);
static int _mdrv_alsa_control_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);
#endif

static int _mdrv_alsa_system_probe(struct platform_device *dev);
static int _mdrv_alsa_system_remove(struct platform_device *dev);
static int _mdrv_alsa_system_suspend(struct platform_device *dev, pm_message_t state);
static int _mdrv_alsa_system_resume(struct platform_device *dev);

static int _mdrv_alsa_pcm_new(struct MStar_Device_Struct *madchip, int device, int playback_substreams, int capture_substreams);
static int _mdrv_alsa_probe(int device_id);
static int _mdrv_alsa_remove(void);


/*
 * ============================================================================
 * Local Variables
 * ============================================================================
 */
/* MStar Audio DSP */
static bool driver_has_installed = MAD_FALSE;
static struct MStar_Device_Struct *MAD_Chip = NULL;
static struct snd_card *MAD_SND_Card = NULL;
static unsigned char MAD_Capability[MAD_MAX_DEVICES][2] = {
	/* Playback's Substream & Captures's Substreams */
	/* Note! Currently we only support one substream in each device!
	 * if we can support more substreams in one device,
	 * it's required to implement the function pointers of struct MStar_MAD_Ops in HAL layers as well.
	 */
	{1, 1},
	{1, 1},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
};

/* Platform device definition */
static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;

/* Default sample rate of ALSA PCM playback */
static unsigned int MAD_Playback_default_rates[] = {
	8000,
	11025,
	12000,
	16000,
	22050,
	24000,
	32000,
	44100,
	48000,
};

/* Default sample rate of ALSA PCM capture */
static unsigned int MAD_Capture_default_rates[] = {
	48000,
};

/* Default hardware settings of ALSA PCM playback */
static struct snd_pcm_hardware MAD_Playback_default_HW = {
	.info = (SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER | SNDRV_PCM_INFO_MMAP_VALID),
	.formats = SNDRV_PCM_FMTBIT_S16_LE,
	.rates = SNDRV_PCM_RATE_CONTINUOUS | SNDRV_PCM_RATE_8000_48000,
	.rate_min = 8000,
	.rate_max = 48000,
	.channels_min = 1,
	.channels_max = 2,
	.buffer_bytes_max = _MAD_MAX_PCM_BUFFER_SIZE,
	.period_bytes_min = 64,
	.period_bytes_max = (_MAD_MAX_PCM_BUFFER_SIZE >> 2),
	.periods_min = 1,
	.periods_max = 1024,
	.fifo_size = 0,
};

/* Default hardware settings of ALSA PCM capture */
static struct snd_pcm_hardware MAD_Capture_default_HW = {
	.info = (SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER | SNDRV_PCM_INFO_MMAP_VALID),
	.formats = SNDRV_PCM_FMTBIT_S16_LE,
	.rates = SNDRV_PCM_RATE_48000,
	.rate_min = 48000,
	.rate_max = 48000,
	.channels_min = 1,
	.channels_max = 2,
	.buffer_bytes_max = _MAD_MAX_PCM_BUFFER_SIZE,
	.period_bytes_min = 64,
	.period_bytes_max =  (_MAD_MAX_PCM_BUFFER_SIZE >> 2),
	.periods_min = 1,
	.periods_max = 1024,
	.fifo_size = 0,
};

#if 0 /* It's not in used for the moment, might be TODO */
/* Operators for ALSA device */
static struct snd_device_ops MAD_Device_Ops = {
	.dev_free = _mdrv_alsa_dev_free,
};
#endif

/* Operators for ALSA PCM playback */
static struct snd_pcm_ops MAD_Playback_Ops = {
	.open = _mdrv_alsa_playback_open,
	.close = _mdrv_alsa_playback_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = _mdrv_alsa_playback_hw_params,
	.hw_free = _mdrv_alsa_playback_hw_free,
	.prepare = _mdrv_alsa_playback_prepare,
	.trigger = _mdrv_alsa_playback_trigger,    /* Atomic */
	.pointer = _mdrv_alsa_playback_pointer,    /* Atomic */
  	.copy = _mdrv_alsa_playback_copy,
};

/* Operators for ALSA PCM capture */
static struct snd_pcm_ops MAD_Capture_Ops = {
	.open = _mdrv_alsa_capture_open,
	.close = _mdrv_alsa_capture_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = _mdrv_alsa_capture_hw_params,
	.hw_free = _mdrv_alsa_capture_hw_free,
	.prepare = _mdrv_alsa_capture_prepare,
	.trigger = _mdrv_alsa_capture_trigger,    /* Atomic */
	.pointer = _mdrv_alsa_capture_pointer,    /* Atomic */
  	.copy = _mdrv_alsa_capture_copy,
};

#if 0 /* Not support this for the moment, might be TODO */
/* Operators for ALSA control */
static struct snd_kcontrol_new MAD_Control_Ops = {
	.iface = SNDRV_CTL_ELEM_IFACE_CARD,
	.name = _MAD_CONTROL_NAME,
	.info = _mdrv_alsa_control_info,
	.get = _mdrv_alsa_control_get,
	.put = _mdrv_alsa_control_put,
};
#endif

/* Platform driver definition */
static struct platform_driver mstar_mad_system_driver = {
	.probe = _mdrv_alsa_system_probe,
	.remove = _mdrv_alsa_system_remove,
	.suspend = _mdrv_alsa_system_suspend,
	.resume = _mdrv_alsa_system_resume,
	.driver = {
		.name = _MAD_SND_PLATFORM,
		.owner  = THIS_MODULE,
	}
};


/*
 * ============================================================================
 * Function Implementations
 * ============================================================================
 */
int _mdrv_alsa_hook_device(struct MStar_MAD_Info *mad_info)
{
	struct MStar_Device_Struct *madchip = MAD_Chip;
	int device_no = 0;

	if ((mad_info == NULL) || (madchip == NULL))
		return -EINVAL;

	for (device_no = 0; device_no < MAD_MAX_DEVICES; device_no++) {
		if ((madchip->pcm_playback[device_no].active_substreams > 0) || (madchip->pcm_capture[device_no].active_substreams > 0))
			return -EBUSY;
	}

	MAD_PRINT(KERN_INFO "Hal Driver '%s' is hooked.\n", mad_info->name);
	MAD_PRINT(KERN_INFO "MStar Core Version : %s\n", mad_info->version);

	for (device_no = 0; device_no < MAD_MAX_DEVICES; device_no++) {
		if (mad_info->playback_pcm_ops[device_no] != NULL)
			memcpy(&madchip->pcm_playback[device_no].ops, mad_info->playback_pcm_ops[device_no], sizeof(struct MStar_MAD_Ops));
		else
			memset(&madchip->pcm_playback[device_no].ops, 0x00, sizeof(struct MStar_MAD_Ops));

		if (mad_info->capture_pcm_ops[device_no] != NULL)
			memcpy(&madchip->pcm_capture[device_no].ops, mad_info->capture_pcm_ops[device_no], sizeof(struct MStar_MAD_Ops));
		else
			memset(&madchip->pcm_capture[device_no].ops, 0x00, sizeof(struct MStar_MAD_Ops));
	}

	return 0;
}

int _mdrv_alsa_unhook_device(void)
{
	struct MStar_Device_Struct *madchip = MAD_Chip;
	int device_no = 0;

	if (madchip == NULL)
		return -EINVAL;

	for (device_no = 0; device_no < MAD_MAX_DEVICES; device_no++) {
		if ((madchip->pcm_playback[device_no].active_substreams > 0) || (madchip->pcm_capture[device_no].active_substreams > 0))
			return -EBUSY;
	}

	MAD_PRINT(KERN_INFO "Unhook Hal Driver.\n");

	for (device_no = 0; device_no < MAD_MAX_DEVICES; device_no++) {
		memset(&madchip->pcm_playback[device_no].ops, 0x00, sizeof(struct MStar_MAD_Ops));
		memset(&madchip->pcm_capture[device_no].ops, 0x00, sizeof(struct MStar_MAD_Ops));
	}

	return 0;
}

#if 0
static void _mdrv_alsa_dump_runtime_structure(char *caller_function, struct snd_pcm_runtime *runtime)
{
	MAD_PRINT(KERN_INFO "%s() is called by %s()\n", __FUNCTION__, caller_function);
	MAD_PRINT(KERN_INFO "================\n");
	MAD_PRINT(KERN_INFO "runtime->trigger_tstamp.tv_sec = %ld\n", runtime->trigger_tstamp.tv_sec);
	MAD_PRINT(KERN_INFO "runtime->trigger_tstamp.tv_nsec = %ld\n", runtime->trigger_tstamp.tv_nsec);
	MAD_PRINT(KERN_INFO "runtime->avail_max = %lu\n", runtime->avail_max);
	MAD_PRINT(KERN_INFO "runtime->hw_ptr_base = %lu\n", runtime->hw_ptr_base);
	MAD_PRINT(KERN_INFO "runtime->hw_ptr_interrupt = %lu\n", runtime->hw_ptr_interrupt);
	MAD_PRINT(KERN_INFO "runtime->hw_ptr_jiffies = %lu\n", runtime->hw_ptr_jiffies);
	MAD_PRINT(KERN_INFO "runtime->delay = %ld\n", runtime->delay);
	MAD_PRINT(KERN_INFO "--------------------\n");
	MAD_PRINT(KERN_INFO "runtime->access = 0x%08X\n", runtime->access);
	MAD_PRINT(KERN_INFO "runtime->format = 0x%08X\n", runtime->format);
	MAD_PRINT(KERN_INFO "runtime->subformat = 0x%08X\n", runtime->subformat);
	MAD_PRINT(KERN_INFO "runtime->rate = %u\n", runtime->rate);
	MAD_PRINT(KERN_INFO "runtime->channels = %u\n", runtime->channels);
	MAD_PRINT(KERN_INFO "runtime->period_size = %lu\n", runtime->period_size);
	MAD_PRINT(KERN_INFO "runtime->periods = %u\n", runtime->periods);
	MAD_PRINT(KERN_INFO "runtime->buffer_size = %lu\n", runtime->buffer_size);
	MAD_PRINT(KERN_INFO "runtime->min_align = %lu\n", runtime->min_align);
	MAD_PRINT(KERN_INFO "runtime->byte_align = %u\n", runtime->byte_align);
	MAD_PRINT(KERN_INFO "runtime->frame_bits = %u\n", runtime->frame_bits);
	MAD_PRINT(KERN_INFO "runtime->sample_bits = %u\n", runtime->sample_bits);
	MAD_PRINT(KERN_INFO "runtime->info = %u\n", runtime->info);
	MAD_PRINT(KERN_INFO "runtime->rate_num = %u\n", runtime->rate_num);
	MAD_PRINT(KERN_INFO "runtime->rate_den = %u\n", runtime->rate_den);
	MAD_PRINT(KERN_INFO "--------------------\n");
	MAD_PRINT(KERN_INFO "runtime->tstamp_mode = %d\n", runtime->tstamp_mode);
	MAD_PRINT(KERN_INFO "runtime->period_step = %u\n", runtime->period_step);
	MAD_PRINT(KERN_INFO "runtime->start_threshold = %lu\n", runtime->start_threshold);
	MAD_PRINT(KERN_INFO "runtime->stop_threshold = %lu\n", runtime->stop_threshold);
	MAD_PRINT(KERN_INFO "runtime->silence_threshold = %lu\n", runtime->silence_threshold);
	MAD_PRINT(KERN_INFO "runtime->silence_size = %lu\n", runtime->silence_size);
	MAD_PRINT(KERN_INFO "runtime->boundary = %lu\n", runtime->boundary);
	MAD_PRINT(KERN_INFO "runtime->silence_start = %lu\n", runtime->silence_start);
	MAD_PRINT(KERN_INFO "runtime->silence_filled = %lu\n", runtime->silence_filled);
	MAD_PRINT(KERN_INFO "--------------------\n");
	MAD_PRINT(KERN_INFO "runtime->status->state = 0x%08X\n", runtime->status->state);
	MAD_PRINT(KERN_INFO "runtime->status->pad1 = %d\n", runtime->status->pad1);
	MAD_PRINT(KERN_INFO "runtime->status->hw_ptr = %lu\n", runtime->status->hw_ptr);
	MAD_PRINT(KERN_INFO "runtime->status->tstamp.tv_sec = %ld\n", runtime->status->tstamp.tv_sec);
	MAD_PRINT(KERN_INFO "runtime->status->tstamp.tv_nsec = %ld\n", runtime->status->tstamp.tv_nsec);
	MAD_PRINT(KERN_INFO "runtime->status->suspended_state = 0x%08X\n", runtime->status->suspended_state);
	MAD_PRINT(KERN_INFO "--------------------\n");
	MAD_PRINT(KERN_INFO "runtime->control->appl_ptr = %lu\n", runtime->control->appl_ptr);
	MAD_PRINT(KERN_INFO "runtime->control->avail_min = %lu\n", runtime->control->avail_min);
	MAD_PRINT(KERN_INFO "--------------------\n");
	MAD_PRINT(KERN_INFO "runtime->hw.info = %u\n", runtime->hw.info);
	MAD_PRINT(KERN_INFO "runtime->hw.formats = %llu\n", runtime->hw.formats);
	MAD_PRINT(KERN_INFO "runtime->hw.rates = %u\n", runtime->hw.rates);
	MAD_PRINT(KERN_INFO "runtime->hw.rate_min = %u\n", runtime->hw.rate_min);
	MAD_PRINT(KERN_INFO "runtime->hw.rate_max = %u\n", runtime->hw.rate_max);
	MAD_PRINT(KERN_INFO "runtime->hw.channels_min = %u\n", runtime->hw.channels_min);
	MAD_PRINT(KERN_INFO "runtime->hw.channels_max = %u\n", runtime->hw.channels_max);
	MAD_PRINT(KERN_INFO "runtime->hw.buffer_bytes_max = %u\n", runtime->hw.buffer_bytes_max);
	MAD_PRINT(KERN_INFO "runtime->hw.period_bytes_min = %u\n", runtime->hw.period_bytes_min);
	MAD_PRINT(KERN_INFO "runtime->hw.period_bytes_max = %u\n", runtime->hw.period_bytes_max);
	MAD_PRINT(KERN_INFO "runtime->hw.periods_min = %u\n", runtime->hw.periods_min);
	MAD_PRINT(KERN_INFO "runtime->hw.periods_max = %u\n", runtime->hw.periods_max);
	MAD_PRINT(KERN_INFO "runtime->hw.fifo_size = %u\n", runtime->hw.fifo_size);
	MAD_PRINT(KERN_INFO "--------------------\n");
	MAD_PRINT(KERN_INFO "runtime->timer_resolution = %u\n", runtime->timer_resolution);
	MAD_PRINT(KERN_INFO "runtime->tstamp_type = %d\n", runtime->tstamp_type);
	MAD_PRINT(KERN_INFO "--------------------\n");
	MAD_PRINT(KERN_INFO "runtime->dma_area = 0x%08X\n", (unsigned int)runtime->dma_area);
	MAD_PRINT(KERN_INFO "runtime->dma_addr = 0x%08X\n", (unsigned int)runtime->dma_addr);
	MAD_PRINT(KERN_INFO "runtime->dma_bytes = %u\n", runtime->dma_bytes);
	MAD_PRINT(KERN_INFO "--------------------\n");
	MAD_PRINT(KERN_INFO "runtime->dma_buffer_p = 0x%08X\n", (unsigned int)runtime->dma_buffer_p);
	MAD_PRINT(KERN_INFO "runtime->dma_buffer_p->area = 0x%08X\n", (unsigned int)runtime->dma_buffer_p->area);
	MAD_PRINT(KERN_INFO "runtime->dma_buffer_p->addr = 0x%08X\n", (unsigned int)runtime->dma_buffer_p->addr);
	MAD_PRINT(KERN_INFO "runtime->dma_buffer_p->bytes = %u\n", runtime->dma_buffer_p->bytes);
	MAD_PRINT(KERN_INFO "================\n");

	return;
}
#endif

static void _mdrv_alsa_timer_open(struct timer_list *timer, unsigned long data, void *func)
{
	MAD_PRINT(KERN_INFO "Open software timer.\n");

	if (timer == NULL) {
		MAD_PRINT(KERN_ERR "Error! timer pointer is invalid.\n");
		return;
	}
	else if (func == NULL) {
		MAD_PRINT(KERN_ERR "Error! timer callback function is invalid.\n");
		return;
	}

	init_timer(timer);
	timer->data = data;
	timer->function = func;
	timer->expires = jiffies + 20;
	add_timer(timer);

	return;
}

static void _mdrv_alsa_timer_close(struct timer_list *timer)
{
	MAD_PRINT(KERN_INFO "Close software timer.\n");

	if (timer == NULL) {
		MAD_PRINT(KERN_ERR "Error! timer pointer is invalid.\n");
		return;
	}

	del_timer(timer);
	memset(timer, 0x00, sizeof(struct timer_list));

	return;
}

static int _mdrv_alsa_timer_reset(struct timer_list *timer, unsigned long data)
{
	//MAD_PRINT(KERN_INFO "Reset software timer.\n");

	if (timer == NULL) {
		MAD_PRINT(KERN_ERR "Error! timer pointer is invalid.\n");
		return -EINVAL;
	}
	else if (timer->function == NULL) {
		MAD_PRINT(KERN_ERR "Error! timer callback function is invalid.\n");
		return -EINVAL;
	}

	timer->data = data;
	timer->expires = jiffies + 20;
	add_timer(timer);

	return 0;
}

static int _mdrv_alsa_timer_update(struct timer_list *timer, unsigned long data, unsigned long time_interval)
{
	//MAD_PRINT(KERN_INFO "Update software timer(%lu).\n", time_interval);

	if (timer == NULL) {
		MAD_PRINT(KERN_ERR "Error! timer pointer is invalid.\n");
		return -EINVAL;
	}
	else if (timer->function == NULL) {
		MAD_PRINT(KERN_ERR "Error! timer callback function is invalid.\n");
		return -EINVAL;
	}
	else if (timer->expires > jiffies) {
		MAD_PRINT(KERN_ERR "Error! expires = %lu, jiffies = %lu, time_interval = %lu.\n", timer->expires, jiffies, time_interval);
		return -EINVAL;
	}

	timer->data = data;
	timer->expires = jiffies + time_interval;
	add_timer(timer);

	return 0;
}

static int _mdrv_alsa_set_default_hw(struct snd_pcm_hardware *target_hw, int direction)
{
	if (target_hw == NULL)
		return -EINVAL;

	memset(target_hw, 0x00, sizeof(struct snd_pcm_hardware));
	if (direction == SNDRV_PCM_STREAM_PLAYBACK) {
		memcpy(target_hw, &MAD_Playback_default_HW, sizeof(struct snd_pcm_hardware));
	}
	else {
		memcpy(target_hw, &MAD_Capture_default_HW, sizeof(struct snd_pcm_hardware));
	}

	return 0;
}

static int _mdrv_alsa_set_default_constraint_list(struct snd_pcm_hw_constraint_list *target_constraint_list, int direction)
{
	if (target_constraint_list == NULL)
		return -EINVAL;

	memset(target_constraint_list, 0x00, sizeof(struct snd_pcm_hw_constraint_list));

	if (direction == SNDRV_PCM_STREAM_PLAYBACK) {
		target_constraint_list->count = ARRAY_SIZE(MAD_Playback_default_rates);
		target_constraint_list->list = MAD_Playback_default_rates;
	}
	else {
		target_constraint_list->count = ARRAY_SIZE(MAD_Capture_default_rates);
		target_constraint_list->list = MAD_Capture_default_rates;
	}

	target_constraint_list->mask = 0;

	return 0;
}

static int _mdrv_alsa_set_hw_by_constrain_list(struct snd_pcm_hardware *target_hw, struct snd_pcm_hw_constraint_list *target_constraint_list)
{
	int idx = 0;

	target_hw->rates = 0;
	for (idx = 0; idx < target_constraint_list->count; idx++) {
		if (target_constraint_list->list[idx] == 5512)	target_hw->rates |= SNDRV_PCM_RATE_5512;
		else if (target_constraint_list->list[idx] == 8000)	target_hw->rates |= SNDRV_PCM_RATE_8000;
		else if (target_constraint_list->list[idx] == 11025)	target_hw->rates |= SNDRV_PCM_RATE_11025;
		else if (target_constraint_list->list[idx] == 16000)	target_hw->rates |= SNDRV_PCM_RATE_16000;
		else if (target_constraint_list->list[idx] == 22050)	target_hw->rates |= SNDRV_PCM_RATE_22050;
		else if (target_constraint_list->list[idx] == 32000)	target_hw->rates |= SNDRV_PCM_RATE_32000;
		else if (target_constraint_list->list[idx] == 44100)	target_hw->rates |= SNDRV_PCM_RATE_44100;
		else if (target_constraint_list->list[idx] == 48000)	target_hw->rates |= SNDRV_PCM_RATE_48000;
		else if (target_constraint_list->list[idx] == 64000)	target_hw->rates |= SNDRV_PCM_RATE_64000;
		else if (target_constraint_list->list[idx] == 88200)	target_hw->rates |= SNDRV_PCM_RATE_88200;
		else if (target_constraint_list->list[idx] == 96000)	target_hw->rates |= SNDRV_PCM_RATE_96000;
		else if (target_constraint_list->list[idx] == 176400)	target_hw->rates |= SNDRV_PCM_RATE_176400;
		else if (target_constraint_list->list[idx] == 192000)	target_hw->rates |= SNDRV_PCM_RATE_192000;
	}
	target_hw->rate_min = target_constraint_list->list[0];
	target_hw->rate_max = target_constraint_list->list[target_constraint_list->count - 1];

	return 0;
}

static int _mdrv_alsa_playback_open(struct snd_pcm_substream *substream)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_playback[substream->pcm->device];
	struct snd_pcm_hw_constraint_list *madchip_constraints_list = &madchip_runtime->constraints_rates;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_pcm_hardware madchip_hardware;
	int substream_no = substream->number;
	int err = 0;
	MAD_PRINT(KERN_INFO "Open '%s'(%d) of MStar ALSA playback device\n", substream->name, substream->number);

	mutex_lock(&madchip_runtime->mutex_lock);

	if (madchip->active_playback_devices == MAD_MAX_DEVICES) {
		MAD_PRINT(KERN_ERR "Error! no more availabe device, try again later\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -EBUSY;
	}
	else if (madchip_runtime->active_substreams == madchip_runtime->max_substreams) {
		MAD_PRINT(KERN_ERR "Error! no more availabe substream, try again later\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -EBUSY;
	}

	/* Set specified information */
	madchip_runtime->substreams[substream_no].substream= substream;
	runtime->private_data = madchip_runtime;
	runtime->private_free = NULL;

	/* Set default configurations */
	_mdrv_alsa_set_default_hw(&madchip_hardware, SNDRV_PCM_STREAM_PLAYBACK);
	_mdrv_alsa_set_default_constraint_list(madchip_constraints_list, SNDRV_PCM_STREAM_PLAYBACK);
	substream->dma_buffer.dev.type = SNDRV_DMA_TYPE_CONTINUOUS;

	/* Get configuration from MStar Audio DSP  */
	if (madchip_runtime->ops.get) {
		madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_MAX_CHANNEL, &madchip_hardware.channels_max);
		//madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_BUFFER_SIZE, &madchip_hardware.buffer_bytes_max);
		//madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_PERIOD_SIZE, &madchip_hardware.period_bytes_max);
		madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_CONSTRAINTS_COUNT, &madchip_constraints_list->count);
		madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_CONSTRAINTS_LIST, (unsigned int *)&madchip_constraints_list->list);
		madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_CONSTRAINTS_MASK, &madchip_constraints_list->mask);
		_mdrv_alsa_set_hw_by_constrain_list(&madchip_hardware, madchip_constraints_list);
	}

	/* Allocate system memory for Specific Copy */
	madchip_runtime->buffer[substream_no].addr = (unsigned char *)vmalloc(_MAD_MAX_PCM_BUFFER_SIZE);
	if (madchip_runtime->buffer[substream_no].addr == NULL) {
		MAD_PRINT(KERN_ERR "Error! fail to allocate %u bytes memory for PCM buffer!\n", _MAD_MAX_PCM_BUFFER_SIZE);
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -ENOMEM;
	}
	memset((void *)madchip_runtime->buffer[substream_no].addr, 0x00, _MAD_MAX_PCM_BUFFER_SIZE);
	madchip_runtime->buffer[substream_no].size = _MAD_MAX_PCM_BUFFER_SIZE;

#if 0
	if ((err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS)) < 0) {
		MAD_PRINT(KERN_ERR "Error! fail to set hw constraint integer!\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return err;
	}
#endif

	/* Fill in some settings */
	memcpy(&runtime->hw, &madchip_hardware, sizeof(struct snd_pcm_hardware));
	if ((err = snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE, madchip_constraints_list)) < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to set hw constraint list!\n", err);
		mutex_unlock(&madchip_runtime->mutex_lock);
		return err;
	}

	if (madchip_runtime->active_substreams == 0) {
		madchip->active_playback_devices++;

		spin_lock_init(&madchip_runtime->spin_lock);
		_mdrv_alsa_timer_open(&madchip_runtime->timer, (unsigned long)substream->pcm->device, (void *)_mdrv_alsa_playback_monitor);
	}
	madchip_runtime->active_substreams++;

	mutex_unlock(&madchip_runtime->mutex_lock);

	return 0;
}

static int _mdrv_alsa_playback_close(struct snd_pcm_substream *substream)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_playback[substream->pcm->device];
	unsigned int warning_counter = 0;
	int substream_no = substream->number;
	unsigned long flags;
	MAD_PRINT(KERN_INFO "Close '%s'(%d) of MStar ALSA playback device\n", substream->name, substream->number);

	mutex_lock(&madchip_runtime->mutex_lock);

	if (madchip->active_playback_devices == 0) {
		MAD_PRINT(KERN_ERR "Error! no device is in active\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -ENODEV;
	}
	else if (madchip_runtime->active_substreams == 0) {
		MAD_PRINT(KERN_ERR "Error! no substream is in active\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -ENODEV;
	}

	madchip_runtime->active_substreams--;

	/* Reset some variables */
	madchip_runtime->substreams[substream_no].substream_status = E_STOP;
	madchip_runtime->substreams[substream_no].substream = NULL;

	/* Free allocated memory */
	if (madchip_runtime->buffer[substream_no].addr != NULL) {
		vfree(madchip_runtime->buffer[substream_no].addr);
		madchip_runtime->buffer[substream_no].addr = NULL;
	}

	if (madchip_runtime->active_substreams == 0) {
		madchip->active_playback_devices--;

		while (madchip_runtime->monitor.monitor_status != E_STOP) {
			if ((++warning_counter) > 100) {
				MAD_PRINT(KERN_WARNING "Warning! The status of Playback Monitor %d is in unexpected status (%u)!\n", substream->pcm->device, madchip_runtime->monitor.monitor_status);
				break;
			}

			mdelay(2);
		}

		_mdrv_alsa_timer_close(&madchip_runtime->timer);

		spin_lock_irqsave(&madchip_runtime->spin_lock, flags);
		memset(&madchip_runtime->monitor, 0x00, sizeof(struct MStar_Monitor_Struct));
		spin_unlock_irqrestore(&madchip_runtime->spin_lock, flags);

		/* Reset PCM configurations */
		madchip_runtime->sample_rate = 0;
		madchip_runtime->channel_mode= 0;
		madchip_runtime->runtime_status = E_STOP;
		madchip_runtime->device_status = MAD_FALSE;

		/* Stop MStar Audio DSP */
		if (madchip_runtime->ops.stop) {
			madchip_runtime->ops.stop();
			mdelay(2);
		}

		/* Close MStar Audio DSP */
		if (madchip_runtime->ops.close) {
			madchip_runtime->ops.close();
			mdelay(2);
		}
	}

	mutex_unlock(&madchip_runtime->mutex_lock);

	return 0;
}

static int _mdrv_alsa_playback_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	size_t size = _MAD_MAX_PCM_BUFFER_SIZE;
	int err = 0;
	MAD_PRINT(KERN_INFO "Configure PCM HW resource\n");

	err = snd_pcm_lib_malloc_pages(substream, size);
	if (err < 0)
		MAD_PRINT(KERN_ERR "Error(%d)! fail to allocate %u bytes for PCM HW resource !\n", err, size);

	return err;
}

static int _mdrv_alsa_playback_hw_free(struct snd_pcm_substream *substream)
{
	MAD_PRINT(KERN_INFO "Free PCM HW resource\n");
	return snd_pcm_lib_free_pages(substream);
}

static int _mdrv_alsa_playback_prepare(struct snd_pcm_substream *substream)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_playback[substream->pcm->device];
	struct snd_pcm_runtime *runtime = substream->runtime;
	static unsigned int warning_counter = 0;
	//MAD_PRINT(KERN_INFO "Reset and prepare PCM resource\n");

	mutex_lock(&madchip_runtime->mutex_lock);

	if (madchip_runtime->ops.get) {
		madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_DEVICE_STATUS, &madchip_runtime->device_status);
		if (madchip_runtime->device_status != MAD_TRUE) {
			if (((++warning_counter) % 10) == 1)
				MAD_PRINT(KERN_WARNING "Warning! Playback device is not ready yet, try again later!\n");

			mutex_unlock(&madchip_runtime->mutex_lock);

			return 0;
		}
		warning_counter = 0;
	}
	else {
		madchip_runtime->device_status = MAD_TRUE;
	}

	if (madchip_runtime->runtime_status != E_START) {
		/* Configure MStar Audio DSP sample rate */
		madchip_runtime->sample_rate = runtime->rate;
		if (madchip_runtime->ops.set) {
			madchip_runtime->ops.set(E_PCM_PLAYBACK_SET_SAMPLE_RATE, &runtime->rate);
		}

		/* Configure MStar Audio DSP channel mode */
		madchip_runtime->channel_mode = runtime->channels;
		if (madchip_runtime->ops.set) {
			madchip_runtime->ops.set(E_PCM_PLAYBACK_SET_CHANNEL_MODE, &runtime->channels);
		}

		/* Open MStar Audio DSP  */
		if (madchip_runtime->ops.open) {
			madchip_runtime->ops.open();
		}

		/* Start MStar Audio DSP  */
		if (madchip_runtime->ops.start) {
			madchip_runtime->ops.start();
		}

		/* Configure MStar Audio DSP device status */
		madchip_runtime->runtime_status = E_START;
	}
	else {
		/* Stop MStar Audio DSP  */
		if (madchip_runtime->ops.stop) {
			madchip_runtime->ops.stop();
			mdelay(2);
		}

		/* Start MStar Audio DSP  */
		if (madchip_runtime->ops.start) {
			madchip_runtime->ops.start();
		}
	}

	mutex_unlock(&madchip_runtime->mutex_lock);

	return 0;
}

static int _mdrv_alsa_playback_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_playback[substream->pcm->device];
	int substream_no = substream->number;
	int err = 0;

	switch(cmd) {
		case SNDRV_PCM_TRIGGER_STOP:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_STOP command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_STOP;
			break;
		}

		case SNDRV_PCM_TRIGGER_START:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_START command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_START;
			break;
		}

		case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_PAUSE_PUSH command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_PAUSE;
			break;
		}

		case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_PAUSE_RELEASE command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_PAUSE_RELEASE;
			break;
		}

		case SNDRV_PCM_TRIGGER_SUSPEND:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_SUSPEND command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_SUSPEND;
			break;
		}

		case SNDRV_PCM_TRIGGER_RESUME:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_RESUME command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_RESUME;
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid playback's trigger command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

static snd_pcm_uframes_t _mdrv_alsa_playback_pointer(struct snd_pcm_substream *substream)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_playback[substream->pcm->device];
	struct snd_pcm_runtime *runtime = substream->runtime;
	int substream_no = substream->number;
	snd_pcm_uframes_t pcm_buffer_level = 0;
	snd_pcm_uframes_t new_pointer = 0;
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);

	if (madchip_runtime->runtime_status == E_START) {
		if (madchip_runtime->ops.get) {
			madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_BUFFER_INUSED_BYTES, &madchip_runtime->buffer[substream_no].inused_size);
			pcm_buffer_level = bytes_to_frames(runtime, madchip_runtime->buffer[substream_no].inused_size);
		}
	}

	new_pointer = runtime->control->appl_ptr - pcm_buffer_level;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31)) /* Since ALSA Kernel Module v1.0.20 */
	new_pointer = new_pointer % runtime->buffer_size;
#endif

	return new_pointer;
}

static int _mdrv_alsa_playback_copy(struct snd_pcm_substream *substream, int channel, snd_pcm_uframes_t pos, void __user *buf, snd_pcm_uframes_t count)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_playback[substream->pcm->device];
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned char *buffer_tmp = NULL;
	unsigned int period_size = (_MAD_MAX_PCM_BUFFER_SIZE >> 2);
	unsigned int request_size = 0;
	unsigned int copied_size = 0;
	unsigned int size_to_copy = 0;
	unsigned int  size = 0;
	int substream_no = substream->number;
	int retry_counter = 0;
	unsigned long flags;
	static unsigned int warning_counter = 0;
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);

	if (buf == NULL) {
		MAD_PRINT(KERN_ERR "Error! invalid memory address!\n");
		return -EINVAL;
	}

	if (count == 0) {
		MAD_PRINT(KERN_ERR "Error! request bytes is zero!\n");
		return -EINVAL;
	}

	if ((madchip_runtime->device_status != MAD_TRUE) || (madchip_runtime->runtime_status != E_START)) {
		if (((++warning_counter) % 10) == 1)
			MAD_PRINT(KERN_ERR "Error! Playback device is not ready yet, try again later!\n");
		return -EAGAIN;
	}
	warning_counter = 0;

	buffer_tmp = madchip_runtime->buffer[substream_no].addr;
	if (buffer_tmp == NULL) {
		MAD_PRINT(KERN_ERR "Error! need to allocate system memory for PCM buffer!\n");
		return -ENXIO;
	}

	/* Wake up Monitor if necessary */
	if (madchip_runtime->monitor.monitor_status == E_STOP) {
		MAD_PRINT(KERN_INFO "Wake up Playback Monitor %d\n", substream->pcm->device);

		while(1) {
			if (_mdrv_alsa_timer_reset(&madchip_runtime->timer, (unsigned long)substream->pcm->device) == 0) {
				spin_lock_irqsave(&madchip_runtime->spin_lock, flags);
				madchip_runtime->monitor.monitor_status = E_START;
				spin_unlock_irqrestore(&madchip_runtime->spin_lock, flags);
				break;
			}

			if ((++retry_counter) > 50) {
				MAD_PRINT(KERN_ERR "Error! fail to wake up Playback Monitor %d\n", substream->pcm->device);
				break;
			}

			msleep(2);
		}
	}

	/* A patch here for Supernova + Android platform !!!
	 * To ensure the audio path of DMA Reader is correct and DMA Reader is turned on !!!
	 */
	if (madchip_runtime->ops.start) {
		madchip_runtime->ops.start();
	}

	if (madchip_runtime->ops.get) {
		madchip_runtime->ops.get(E_PCM_PLAYBACK_GET_PERIOD_SIZE, &period_size);
	}

	request_size = frames_to_bytes(runtime, count);

	do {
		if (madchip_runtime->ops.write) {
			unsigned long receive_size = 0;

			size_to_copy = request_size - copied_size;
			if (size_to_copy > period_size) {
				size_to_copy = period_size;
			}

			/* Deliver PCM data */
			receive_size = copy_from_user(buffer_tmp, (buf + copied_size), size_to_copy);
			while(1) {
				size = madchip_runtime->ops.write((void *)buffer_tmp, size_to_copy);
				if (size == 0) {
					if ((++retry_counter) > 500) {
						MAD_PRINT(KERN_DEBUG "Retry to write PCM\n");
						retry_counter = 0;
					}

					msleep(2);
				}
				else {
					break;
				}
			}

			copied_size += size;
		}
		else {
			copied_size = request_size;
		}
	} while (copied_size < request_size);

	return 0;
}

static void _mdrv_alsa_playback_monitor(unsigned long device_no)
{
	struct MStar_Device_Struct *madchip = NULL;
	struct MStar_Runtime_Struct *madchip_runtime = NULL;
	struct MStar_Monitor_Struct *monitor = NULL;
	struct snd_pcm_substream *substream = NULL;
	struct snd_pcm_runtime *runtime = NULL;
	char time_interval = 20;
	int substream_no = 0;
	unsigned int expiration_counter = 500 / time_interval; /* Set expiration as 500ms */
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);

	if (MAD_Chip == NULL) {
		MAD_PRINT(KERN_ERR "Error! invalid parameters!\n");
		return;
	}

	madchip = MAD_Chip;
	madchip_runtime = &madchip->pcm_playback[device_no];
	monitor = &madchip_runtime->monitor;

	if (madchip_runtime->active_substreams == 0) {
		//MAD_PRINT(KERN_INFO "No any active substreams, just exit Playback Monitor %lu\n", device_no);
		spin_lock(&madchip_runtime->spin_lock);
		memset(monitor, 0x00, sizeof(struct MStar_Monitor_Struct));
		spin_unlock(&madchip_runtime->spin_lock);
		return;
	}

	if (monitor->monitor_status == E_STOP) {
		//MAD_PRINT(KERN_INFO "No action is required, just exit Playback Monitor %lu\n", device_no);
		spin_lock(&madchip_runtime->spin_lock);
		memset(monitor, 0x00, sizeof(struct MStar_Monitor_Struct));
		spin_unlock(&madchip_runtime->spin_lock);
		return;
	}

	for (substream_no = 0; substream_no < madchip_runtime->max_substreams; substream_no++) {
		if ((substream = madchip_runtime->substreams[substream_no].substream) == NULL) {
			continue;
		}

		runtime = substream->runtime;
		if ((runtime->buffer_size - runtime->control->avail_min) <= (runtime->control->appl_ptr - runtime->status->hw_ptr)) {
			//MAD_PRINT(KERN_INFO "Force to update status in Playback Monitor %lu\n", device_no);
			snd_pcm_period_elapsed(substream);
		}
	}

	/* If there is nothing to do, increase monitor's "expiration_counter" */
	if (monitor->last_appl_ptr == runtime->control->appl_ptr) {
		monitor->expiration_counter++;

		if (monitor->expiration_counter >= expiration_counter) {
			//MAD_PRINT(KERN_INFO "Exit Playback Monitor %lu\n", device_no);
			spin_lock(&madchip_runtime->spin_lock);
			memset(monitor, 0x00, sizeof(struct MStar_Monitor_Struct));
			spin_unlock(&madchip_runtime->spin_lock);
			return;
		}
	}
	else {
		monitor->last_appl_ptr = runtime->control->appl_ptr;
		monitor->expiration_counter = 0;
	}

	if (_mdrv_alsa_timer_update(&madchip_runtime->timer, device_no, time_interval) != 0) {
		//MAD_PRINT(KERN_ERR "Error! fail to update timer for Playback Monitor %lu!\n", device_no);
		spin_lock(&madchip_runtime->spin_lock);
		memset(monitor, 0x00, sizeof(struct MStar_Monitor_Struct));
		spin_unlock(&madchip_runtime->spin_lock);
	}

	return;
}

static int _mdrv_alsa_capture_open(struct snd_pcm_substream *substream)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_capture[substream->pcm->device];
	struct snd_pcm_hw_constraint_list *madchip_constraints_list = &madchip_runtime->constraints_rates;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_pcm_hardware madchip_hardware;
	int substream_no = substream->number;
	int err = 0;
	MAD_PRINT(KERN_INFO "Open '%s'(%d) of MStar ALSA capture device\n", substream->name, substream->number);

	mutex_lock(&madchip_runtime->mutex_lock);

	if (madchip->active_capture_devices == MAD_MAX_DEVICES) {
		MAD_PRINT(KERN_ERR "Error! no more availabe device, try again later\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -EBUSY;
	}
	else if (madchip_runtime->active_substreams == madchip_runtime->max_substreams) {
		MAD_PRINT(KERN_ERR "Error! no more availabe substream, try again later\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -EBUSY;
	}

	/* Set specified information */
	madchip_runtime->substreams[substream_no].substream = substream;
	runtime->private_data = madchip_runtime;
	runtime->private_free = NULL;

	/* Set default configurations */
	_mdrv_alsa_set_default_hw(&madchip_hardware, SNDRV_PCM_STREAM_CAPTURE);
	_mdrv_alsa_set_default_constraint_list(madchip_constraints_list, SNDRV_PCM_STREAM_CAPTURE);
	substream->dma_buffer.dev.type = SNDRV_DMA_TYPE_CONTINUOUS;

	/* Allocate system memory for Specific Copy */
	madchip_runtime->buffer[substream_no].addr = (unsigned char *)vmalloc(_MAD_MAX_PCM_BUFFER_SIZE);
	if (madchip_runtime->buffer[substream_no].addr == NULL) {
		MAD_PRINT(KERN_ERR "Error! fail to allocate %u bytes memory for PCM buffer!\n", _MAD_MAX_PCM_BUFFER_SIZE);
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -ENOMEM;
	}
	memset((void *)madchip_runtime->buffer[substream_no].addr, 0x00, _MAD_MAX_PCM_BUFFER_SIZE);
	madchip_runtime->buffer[substream_no].size = _MAD_MAX_PCM_BUFFER_SIZE;

#if 0
	if ((err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS)) < 0) {
		MAD_PRINT(KERN_ERR "Error! fail to set hw constraint integer!\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return err;
	}
#endif

	/* Fill in some settings */
	memcpy(&runtime->hw, &madchip_hardware, sizeof(struct snd_pcm_hardware));
	if ((err = snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE, madchip_constraints_list)) < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to set hw constraint list!\n", err);
		mutex_unlock(&madchip_runtime->mutex_lock);
		return err;
	}

	if (madchip_runtime->active_substreams == 0) {
		madchip->active_capture_devices++;
	}
	madchip_runtime->active_substreams++;

	mutex_unlock(&madchip_runtime->mutex_lock);

	return 0;
}

static int _mdrv_alsa_capture_close(struct snd_pcm_substream *substream)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_capture[substream->pcm->device];
	int substream_no = substream->number;
	MAD_PRINT(KERN_INFO "Close '%s'(%d) of MStar ALSA capture device\n", substream->name, substream->number);

	mutex_lock(&madchip_runtime->mutex_lock);

	if (madchip->active_capture_devices == 0) {
		MAD_PRINT(KERN_ERR "Error! no device is in active\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -ENODEV;
	}
	else if (madchip_runtime->active_substreams == 0) {
		MAD_PRINT(KERN_ERR "Error! no substream is in active\n");
		mutex_unlock(&madchip_runtime->mutex_lock);
		return -ENODEV;
	}

	madchip_runtime->active_substreams--;

	/* Reset some variables */
	madchip_runtime->substreams[substream_no].substream_status = E_STOP;
	madchip_runtime->substreams[substream_no].substream = NULL;

	/* Free allocated memory */
	if (madchip_runtime->buffer[substream_no].addr != NULL) {
		vfree(madchip_runtime->buffer[substream_no].addr);
		madchip_runtime->buffer[substream_no].addr = NULL;
	}

	if (madchip_runtime->active_substreams == 0) {
		madchip->active_capture_devices--;

		memset(&madchip_runtime->monitor, 0x00, sizeof(struct MStar_Monitor_Struct));

		/* Reset PCM configurations */
		madchip_runtime->sample_rate = 0;
		madchip_runtime->channel_mode= 0;
		madchip_runtime->runtime_status = E_STOP;
		madchip_runtime->device_status = MAD_FALSE;

		/* Stop MStar Audio DSP */
		if (madchip_runtime->ops.stop) {
			madchip_runtime->ops.stop();
			mdelay(2);
		}

		/* Close MStar Audio DSP */
		if (madchip_runtime->ops.close) {
			madchip_runtime->ops.close();
			mdelay(2);
		}
	}

	mutex_unlock(&madchip_runtime->mutex_lock);

	return 0;
}

static int _mdrv_alsa_capture_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	size_t size = _MAD_MAX_PCM_BUFFER_SIZE;
	int err = 0;
	MAD_PRINT(KERN_INFO "Configure PCM HW resource\n");

	err = snd_pcm_lib_malloc_pages(substream, size);
	if (err < 0)
		MAD_PRINT(KERN_ERR "Error(%d)! fail to allocate %u bytes for PCM HW resource !\n", err, size);

	return err;
}

static int _mdrv_alsa_capture_hw_free(struct snd_pcm_substream *substream)
{
	MAD_PRINT(KERN_INFO "Free PCM HW resource\n");
	return snd_pcm_lib_free_pages(substream);
}

static int _mdrv_alsa_capture_prepare(struct snd_pcm_substream *substream)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_capture[substream->pcm->device];
	static unsigned int warning_counter = 0;
	//MAD_PRINT(KERN_INFO "Reset and prepare PCM resource\n");

	mutex_lock(&madchip_runtime->mutex_lock);

	if (madchip_runtime->ops.get) {
		madchip_runtime->ops.get(E_PCM_CAPTURE_GET_DEVICE_STATUS, &madchip_runtime->device_status);
		if (madchip_runtime->device_status != MAD_TRUE) {
			if (((++warning_counter) % 10) == 1)
				MAD_PRINT(KERN_WARNING "Warning! Capture device is not ready yet, try again later!\n");

			mutex_unlock(&madchip_runtime->mutex_lock);

			return 0;
		}
		warning_counter = 0;
	}
	else {
		madchip_runtime->device_status = MAD_TRUE;
	}

	if (madchip_runtime->runtime_status != E_START) {
		/* Open MStar Audio DSP */
		if (madchip_runtime->ops.open) {
			madchip_runtime->ops.open();
		}

		/* Start MStar Audio DSP  */
		if (madchip_runtime->ops.start) {
			madchip_runtime->ops.start();
		}

		/* Configure MStar Audio DSP device status */
		madchip_runtime->runtime_status = E_START;
	}
	else {
		/* Start MStar Audio DSP  */
		if (madchip_runtime->ops.start) {
			madchip_runtime->ops.start();
		}
	}

	mutex_unlock(&madchip_runtime->mutex_lock);

	return 0;
}

static int _mdrv_alsa_capture_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_capture[substream->pcm->device];
	int substream_no = substream->number;
	int err = 0;

	switch(cmd) {
		case SNDRV_PCM_TRIGGER_STOP:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_STOP command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_STOP;
			break;
		}

		case SNDRV_PCM_TRIGGER_START:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_START command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_START;
			break;
		}

		case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_PAUSE_PUSH command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_PAUSE;
			break;
		}

		case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_PAUSE_RELEASE command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_PAUSE_RELEASE;
			break;
		}

		case SNDRV_PCM_TRIGGER_SUSPEND:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_SUSPEND command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_SUSPEND;
			break;
		}

		case SNDRV_PCM_TRIGGER_RESUME:
		{
			//MAD_PRINT(KERN_INFO "SNDRV_PCM_TRIGGER_RESUME command is received\n");
			madchip_runtime->substreams[substream_no].substream_status = E_RESUME;
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid capture's trigger command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

static snd_pcm_uframes_t _mdrv_alsa_capture_pointer(struct snd_pcm_substream *substream)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_capture[substream->pcm->device];
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned int new_avail_bytes = 0;
	snd_pcm_uframes_t pcm_buffer_level = 0;
	snd_pcm_uframes_t new_pointer = 0;
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);

	if (madchip_runtime->ops.get) {
		madchip_runtime->ops.get(E_PCM_CAPTURE_GET_NEW_PCM_AVAIL_BYTES, &new_avail_bytes);
	}
	else {
		new_avail_bytes += frames_to_bytes(runtime, runtime->periods);
	}

	pcm_buffer_level = bytes_to_frames(runtime, new_avail_bytes);
	new_pointer = runtime->status->hw_ptr + pcm_buffer_level;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31)) /* Since ALSA Kernel Module v1.0.20 */
	new_pointer = new_pointer % runtime->buffer_size;
#endif

	return new_pointer;
}

static int _mdrv_alsa_capture_copy(struct snd_pcm_substream *substream, int channel, snd_pcm_uframes_t pos, void __user *buf, snd_pcm_uframes_t count)
{
	struct MStar_Device_Struct *madchip = snd_pcm_substream_chip(substream);
	struct MStar_Runtime_Struct *madchip_runtime = &madchip->pcm_capture[substream->pcm->device];
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned char *buffer_tmp = NULL;
	unsigned int avail_size = 0;
	unsigned int request_size = 0;
	unsigned int copied_size = 0;
	unsigned int  size = 0;
	int substream_no = substream->number;
	unsigned long ret = 0;
	static unsigned int warning_counter = 0;
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);

	if (buf == NULL) {
		MAD_PRINT(KERN_ERR "Error! invalid memory address!\n");
		return -EINVAL;
	}

	if (count == 0) {
		MAD_PRINT(KERN_ERR "Error! request bytes is zero!\n");
		return -EINVAL;
	}

	if ((madchip_runtime->device_status != MAD_TRUE) || (madchip_runtime->runtime_status != E_START)) {
		if (((++warning_counter) % 10) == 1)
			MAD_PRINT(KERN_ERR "Error! Capture device is not ready yet, try again later!\n");
		return -EAGAIN;
	}
	warning_counter = 0;

	buffer_tmp = madchip_runtime->buffer[substream_no].addr;
	if (buffer_tmp == NULL) {
		MAD_PRINT(KERN_ERR "Error! need to allocate system memory for PCM buffer!\n");
		return -ENXIO;
	}

	if (madchip_runtime->ops.get) {
		madchip_runtime->ops.get(E_PCM_CAPTURE_GET_TOTAL_PCM_AVAIL_BYTES, &madchip_runtime->buffer[substream_no].avail_size);
	}
	else {
		madchip_runtime->buffer[substream_no].avail_size += frames_to_bytes(runtime, runtime->periods);
	}

	request_size = frames_to_bytes(runtime, count);
	avail_size = madchip_runtime->buffer[substream_no].avail_size;

	if ((avail_size == 0) || (avail_size < request_size)) {
		//MAD_PRINT(KERN_ERR "Error! available PCM is %u bytes, but request %u bytes\n", avail_size, request_size);
		return -EAGAIN;
	}

	do {
		if (madchip_runtime->ops.read) {
			/* Receive PCM data */
			size = madchip_runtime->ops.read(buffer_tmp, (request_size - copied_size));
			ret = copy_to_user((buf + copied_size), buffer_tmp, size);
			copied_size += size;
		}
		else {
			copied_size = request_size;
		}
	} while (copied_size < request_size);

	madchip_runtime->buffer[substream_no].avail_size -= request_size;

	return 0;
}

#if 0 /* Not support this for the moment, might be TODO */
static int _mdrv_alsa_control_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);
	return 0;
}

static int _mdrv_alsa_control_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);
	return 0;
}

static int _mdrv_alsa_control_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);
	return 0;
}
#endif

#if 0 /* It's not in used for the moment, might be TODO */
static int _mdrv_alsa_dev_free(struct snd_device *dev)
{
	struct MStar_Device_Struct *madchip = dev->device_data;
	MAD_PRINT(KERN_INFO "Free MStar ALSA driver Device resource\n");

	/* TODO: if there is any other resource in MStar_Device_Struct has to be released, ie. irq, ... an so on */

	return 0;
}

static void _mdrv_alsa_runtime_free(struct snd_pcm_runtime *runtime)
{
	struct MStar_Runtime_Struct *madchip_runtime = runtime->private_data;
	MAD_PRINT(KERN_INFO "Free MStar ALSA driver PCM resource\n");

	return;
}
#endif

static int _mdrv_alsa_pcm_new(struct MStar_Device_Struct *madchip, int device_no, int playback_substreams, int capture_substreams)
{
	struct snd_card *card = madchip->card;
	struct snd_pcm *pcm = NULL;
	int err = 0;

	/* Create a PCM instance */
	if ((err = snd_pcm_new(card, _MAD_PCM_INSTANCE, device_no, playback_substreams, capture_substreams, &pcm)) < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to create a PCM instance!\n", err);
		return err;
	}

	/* Set playback operations to the PCM instance */
	if (playback_substreams > 0)
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &MAD_Playback_Ops);
	if (capture_substreams > 0)
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &MAD_Capture_Ops);

	/* Fill in MStar device data structure */
	pcm->info_flags = 0;
	pcm->private_data = madchip;
	madchip->pcm[device_no] = pcm;

	/* Fill in MStar ALSA information (PCM) */
	sprintf(pcm->name, "%s No.%02d", _MAD_PCM_NAME, device_no); /* 80 bytes */

	/* Pre-allocate DMA memory to all substreams of the given pcm */
	snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_CONTINUOUS,
		snd_dma_continuous_data(GFP_KERNEL), _MAD_MAX_PCM_BUFFER_SIZE, _MAD_MAX_PCM_BUFFER_SIZE);

#if 0 /* Not support this for the moment, might be TODO */
	/* Create a control element */
	err = snd_ctl_add(card, (struct snd_kcontrol *)&MAD_Control_Ops);
	if (err < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to create a control element!\n", err);
		return err;
	}
#endif

	return 0;
}

static int _mdrv_alsa_system_probe(struct platform_device *dev)
{
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);
	return 0;
}

static int _mdrv_alsa_system_remove(struct platform_device *dev)
{
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);
	return 0;
}

static int _mdrv_alsa_system_suspend(struct platform_device *dev, pm_message_t state)
{
	struct MStar_Device_Struct *madchip = MAD_Chip;
	int device_no = 0;
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);

	if (madchip == NULL)
		return -EINVAL;

	for (device_no = 0; device_no < _MAD_DEVICE; device_no++) {
		mutex_lock(&madchip->pcm_playback[device_no].mutex_lock);
		madchip->pcm_playback[device_no].runtime_status = E_SUSPEND;
		madchip->pcm_playback[device_no].device_status = MAD_FALSE;
		if (madchip->pcm_playback[device_no].ops.suspend) {
			madchip->pcm_playback[device_no].ops.suspend();
		}
		mutex_unlock(&madchip->pcm_playback[device_no].mutex_lock);

		mutex_lock(&madchip->pcm_capture[device_no].mutex_lock);
		madchip->pcm_capture[device_no].runtime_status = E_SUSPEND;
		madchip->pcm_capture[device_no].device_status = MAD_FALSE;
		if (madchip->pcm_capture[device_no].ops.suspend) {
			madchip->pcm_capture[device_no].ops.suspend();
		}
		mutex_unlock(&madchip->pcm_capture[device_no].mutex_lock);
	}

	return 0;
}

static int _mdrv_alsa_system_resume(struct platform_device *dev)
{
	struct MStar_Device_Struct *madchip = MAD_Chip;
	int device_no = 0;
	//MAD_PRINT(KERN_INFO "%s() is invoked\n", __FUNCTION__);

	if (madchip == NULL)
		return -EINVAL;

	for (device_no = 0; device_no < _MAD_DEVICE; device_no++) {
		mutex_lock(&madchip->pcm_playback[device_no].mutex_lock);
		madchip->pcm_playback[device_no].runtime_status = E_RESUME;
		if (madchip->pcm_playback[device_no].ops.resume) {
			madchip->pcm_playback[device_no].ops.resume();
		}
		mutex_unlock(&madchip->pcm_playback[device_no].mutex_lock);

		mutex_lock(&madchip->pcm_capture[device_no].mutex_lock);
		madchip->pcm_capture[device_no].runtime_status = E_RESUME;
		if (madchip->pcm_capture[device_no].ops.resume) {
			madchip->pcm_capture[device_no].ops.resume();
		}
		mutex_unlock(&madchip->pcm_capture[device_no].mutex_lock);
	}

	return 0;
}

static int _mdrv_alsa_probe(int card_no)
{
	struct snd_card *card = NULL;
	struct MStar_Device_Struct *madchip = NULL;
	int device_no = 0;
	int err = 0;
	MAD_PRINT(KERN_INFO "Go to register MStar ALSA driver\n");

	/* Create a sound card */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)) /* Since ALSA Kernel Module v1.0.19 */
	if ((err = snd_card_create(index[card_no], id[card_no], THIS_MODULE, sizeof(struct MStar_Device_Struct), &card)) != 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to create a sound card!\n", err);
		return err;
	}
#else
	card = snd_card_new(index[card_no], id[card_no], THIS_MODULE, sizeof(struct MStar_Device_Struct));
	if (card == NULL) {
		MAD_PRINT(KERN_ERR "Error! fail to create a sound card!\n");
		return -EPERM;
	}
#endif

	strcpy(card->driver, _MAD_SND_DRIVER); /* 16 bytes */
	sprintf(card->shortname, "%s No.%02d", _MAD_SND_SHORTNAME, card_no); /* 32 bytes */
	sprintf(card->longname, "%s No.%02d", _MAD_SND_LONGNAME, card_no); /* 80 bytes */
#if 0 /* It's not in used for the moment, might be TODO */
	sprintf(card->mixername, "%s No.%02d", ???, card_no); /* 80 bytes */
	sprintf(card->components, "%s No.%02d", ???, card_no); /* 80 bytes */
#endif

	madchip = card->private_data;
	memset(madchip, 0x00, sizeof(struct MStar_Device_Struct));

	madchip->card = card;
	MAD_SND_Card = card;
	MAD_Chip = madchip;

	/* Create a MStar PCM instance */
	for (device_no = 0; device_no < _MAD_DEVICE; device_no++) {
		/* Initiate Mutex */
		mutex_init(&madchip->pcm_playback[device_no].mutex_lock);
		mutex_init(&madchip->pcm_capture[device_no].mutex_lock);

		err = _mdrv_alsa_pcm_new(madchip, device_no, MAD_Capability[device_no][0], MAD_Capability[device_no][1]);
		if (err < 0) {
			MAD_PRINT(KERN_ERR "Error(%d)! fail to create PCM instance No.%02d!\n", err, device_no);
			_mdrv_alsa_remove();
			return err;
		}
		madchip->pcm_playback[device_no].max_substreams = MAD_Capability[device_no][0];
		madchip->pcm_capture[device_no].max_substreams = MAD_Capability[device_no][1];
	}

#if 0 /* It's not in used for the moment, might be TODO */
	/* Create a sound card device */
	if ((err = snd_device_new(card, SNDRV_DEV_PCM, madchip, &MAD_Device_Ops)) < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to create a sound card device!\n", err);
		_mdrv_alsa_remove();
		return err;
	}
	snd_card_set_dev(card, &dev->dev);
#endif

	/* Register sound card to OS */
	if ((err = snd_card_register(card)) < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to register sound card to OS!\n", err);
		_mdrv_alsa_remove();
		return err;
	}

	MAD_PRINT(KERN_INFO "MStar ALSA driver is registered to slot %d\n", card->number);

	return 0;
}

static int  _mdrv_alsa_remove(void)
{
	struct snd_card *card = MAD_SND_Card;
	struct MStar_Device_Struct *madchip = MAD_Chip;
	int device_no = 0;
	int substream_no = 0;
	int err = 0;

	if (card == NULL)
		return -EINVAL;

	MAD_PRINT(KERN_INFO "Remove MStar ALSA driver from slot %d\n", card->number);

	/* Unhook device operatos */
	_mdrv_alsa_unhook_device();

	/* Free a created MStar device and allocated memory */
	if (madchip != NULL) {
		for (device_no = 0; device_no < _MAD_DEVICE; device_no++) {
			mutex_destroy(&madchip->pcm_playback[device_no].mutex_lock);
			mutex_destroy(&madchip->pcm_capture[device_no].mutex_lock);

			for (substream_no = 0; substream_no < MAD_MAX_SUBSTREAMS; substream_no++) {
				if (madchip->pcm_playback[device_no].buffer[substream_no].addr != NULL) {
					vfree(madchip->pcm_playback[device_no].buffer[substream_no].addr);
					madchip->pcm_playback[device_no].buffer[substream_no].addr = NULL;
				}
				if (madchip->pcm_capture[device_no].buffer[substream_no].addr != NULL) {
					vfree(madchip->pcm_capture[device_no].buffer[substream_no].addr);
					madchip->pcm_capture[device_no].buffer[substream_no].addr = NULL;
				}
			}
		}

		madchip->card = NULL;
		memset(madchip->pcm, 0x00, sizeof(madchip->pcm));
		MAD_Chip = NULL;
		MAD_SND_Card = NULL;

		kfree(madchip);
		card->private_data = NULL;
	}

	/* Free a created sound card */
	if ((err = snd_card_free(card)) < 0)
		MAD_PRINT(KERN_ERR "Error(%d)! fail to free allocated sound card!\n", err);

	return err;
}

static int __init _mdrv_alsa_init(void)
{
	int card_no = 0;
	int err = 0;

	if (driver_has_installed == MAD_TRUE) {
		MAD_PRINT(KERN_ERR "MStar ALSA driver is already installed!\n");
	}
	else {
		MAD_PRINT(KERN_INFO "Linux Kernel Version : %d.%d.%d\n", ((LINUX_VERSION_CODE >> 16) & 0xFF), ((LINUX_VERSION_CODE >> 8) & 0xFF), (LINUX_VERSION_CODE & 0xFF));
		MAD_PRINT(KERN_INFO "ALSA Kernel Version : %s\n", CONFIG_SND_VERSION);
		MAD_PRINT(KERN_INFO "MStar ALSA Driver Version : %d.%d.%d\n", _MAD_ALSA_DRIVER_VERSION_MAJOR, _MAD_ALSA_DRIVER_VERSION_MINOR, _MAD_ALSA_DRIVER_VERSION_REVISION);
		MAD_PRINT(KERN_INFO "Initiate MStar ALSA driver engine\n");

		/* Register the device */
		for (card_no = 0; card_no < SNDRV_CARDS; card_no++) {
			err = _mdrv_alsa_probe(card_no);
			if (err == 0) {
				/* Only need to register one device to ALSA Kernel Moduel */
				driver_has_installed = MAD_TRUE;
				break;
			}
		}

		err = platform_driver_register(&mstar_mad_system_driver);
		if (err < 0)
			MAD_PRINT(KERN_ERR "Error(%d)! fail to register to platform driver system!\n", err);
	}

	return err;
}

static void __exit _mdrv_alsa_exit(void)
{
	int err = 0;

	if (driver_has_installed == MAD_TRUE) {
		MAD_PRINT(KERN_INFO "Exit MStar ALSA driver engine\n");

		err = _mdrv_alsa_remove();
		driver_has_installed = MAD_FALSE;

		platform_driver_unregister(&mstar_mad_system_driver);
	}

	return;
}


/*
 * ============================================================================
 * Module Information
 * ============================================================================
 */
module_init(_mdrv_alsa_init);
module_exit(_mdrv_alsa_exit);

MODULE_AUTHOR("MStar Semiconductor, Inc.");
MODULE_DESCRIPTION("MStar ALSA Driver - DRV Layer");
MODULE_SUPPORTED_DEVICE("MAD DEVICE");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL(_mdrv_alsa_hook_device);
EXPORT_SYMBOL(_mdrv_alsa_unhook_device);


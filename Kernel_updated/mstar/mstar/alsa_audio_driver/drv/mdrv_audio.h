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

#ifndef _MDRV_ALSA_DRIVER_HEADER
#define _MDRV_ALSA_DRIVER_HEADER

/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */
#include "mdrv_public.h"
#include "mdrv_version.h"


/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
/* Define a Ring Buffer data structure for MStar Audio DSP */
struct MStar_Device_Buffer_Struct {
	unsigned char *addr;
	unsigned int size;
	unsigned int avail_size;
	unsigned int inused_size;
};

/* Define a Substream data structure for MStar Audio DSP */
struct MStar_Substream_Struct {
	struct snd_pcm_substream *substream;
	unsigned int substream_status;
};

/* Define a Monitor data structure for MStar Audio DSP */
struct MStar_Monitor_Struct {
	unsigned int monitor_status;
	unsigned int expiration_counter;
	snd_pcm_uframes_t last_appl_ptr;
};

/* Define a Runtime data structure for MStar Audio DSP */
struct MStar_Runtime_Struct {
	struct snd_pcm_hw_constraint_list constraints_rates;
	struct MStar_MAD_Ops ops;
	struct MStar_Substream_Struct substreams[MAD_MAX_SUBSTREAMS];
	struct MStar_Device_Buffer_Struct buffer[MAD_MAX_SUBSTREAMS];
	struct MStar_Monitor_Struct monitor;
	struct timer_list timer;
	struct mutex mutex_lock;
	spinlock_t spin_lock;
	unsigned char active_substreams;
	unsigned char max_substreams;
	unsigned char channel_mode;
	unsigned int sample_rate;
	unsigned int runtime_status;
	unsigned int device_status;
};

/* Define a Device data structure for MStar Audio DSP */
struct MStar_Device_Struct {
	struct snd_card *card;
	struct snd_pcm *pcm[MAD_MAX_DEVICES];
	struct MStar_Runtime_Struct pcm_playback[MAD_MAX_DEVICES];
	struct MStar_Runtime_Struct pcm_capture[MAD_MAX_DEVICES];
	unsigned char active_playback_devices;
	unsigned char active_capture_devices;
};

#endif /* _MDRV_ALSA_DRIVER_HEADER */


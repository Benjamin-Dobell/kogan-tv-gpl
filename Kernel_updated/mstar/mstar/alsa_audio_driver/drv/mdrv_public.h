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

#ifndef _MDRV_ALSA_PUBLIC_HEADER
#define _MDRV_ALSA_PUBLIC_HEADER

/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
#define MAD_TRUE    1
#define MAD_FALSE    0
#define MAD_MAX_DEVICES    8
#define MAD_MAX_SUBSTREAMS    8

#define MAD_PRINT(fmt, args...)    printk("[%010lu][MAD_ALSA][%06d]     " fmt, jiffies, __LINE__, ## args)

enum MStar_GET_CMD {
	/* Playback */
	E_PCM_PLAYBACK_GET_BUFFER_SIZE = 0,
	E_PCM_PLAYBACK_GET_PERIOD_SIZE,
	E_PCM_PLAYBACK_GET_SAMPLE_RATE,
	E_PCM_PLAYBACK_GET_CHANNEL_MODE,
	E_PCM_PLAYBACK_GET_MAX_CHANNEL,
	E_PCM_PLAYBACK_GET_CONSTRAINTS_COUNT,
	E_PCM_PLAYBACK_GET_CONSTRAINTS_LIST,
	E_PCM_PLAYBACK_GET_CONSTRAINTS_MASK,
	E_PCM_PLAYBACK_GET_BUFFER_INUSED_BYTES,
	E_PCM_PLAYBACK_GET_BUFFER_AVAIL_BYTES,
	E_PCM_PLAYBACK_GET_DEVICE_STATUS,
	E_PCM_PLAYBACK_GET_STR_STATUS,

	/* Capture */
	E_PCM_CAPTURE_GET_NEW_PCM_AVAIL_BYTES,
	E_PCM_CAPTURE_GET_TOTAL_PCM_AVAIL_BYTES,
	E_PCM_CAPTURE_GET_DEVICE_STATUS,
};

enum MStar_SET_CMD {
	/* Playback */
	E_PCM_PLAYBACK_SET_SAMPLE_RATE = 0,
	E_PCM_PLAYBACK_SET_CHANNEL_MODE,

	/* Capture */
	E_PCM_CAPTURE_SET_BUFFER_SIZE,
};

enum {
	E_MONO = 1,
	E_STEREO = 2,
};

enum {
	E_STOP = 0,
	E_START,
	E_PAUSE,
	E_PAUSE_RELEASE,
	E_PREPARE,
	E_SUSPEND,
	E_RESUME,
};

struct MStar_MAD_Ops {
	int (*open)(void);
	int (*close)(void);
	int (*start)(void);
	int (*stop)(void);
	int (*resume)(void);
	int (*suspend)(void);
	unsigned int (*read)(void *buffer, unsigned int bytes);
	unsigned int (*write)(void *buffer, unsigned int bytes);
	int (*get)(int cmd, unsigned int *param);
	int (*set)(int cmd, unsigned int *param);
};

struct MStar_MAD_Info {
	char name[32];
	char version[32];
	struct MStar_MAD_Ops *playback_pcm_ops[MAD_MAX_DEVICES];
	struct MStar_MAD_Ops *capture_pcm_ops[MAD_MAX_DEVICES];
};


/*
 * ============================================================================
 * Forward Declarations
 * ============================================================================
 */
extern int _mdrv_alsa_hook_device(struct MStar_MAD_Info *mad_info);
extern int _mdrv_alsa_unhook_device(void);

#endif /* _MDRV_ALSA_PUBLIC_HEADER */


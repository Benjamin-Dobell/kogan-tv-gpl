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

#ifndef _MHAL_ALSA_DRIVER_HEADER
#define _MHAL_ALSA_DRIVER_HEADER

/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */
#include "mdrv_public.h"
#include "mhal_version.h"


/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
/* Define a Ring Buffer data structure for MStar Audio DSP */
struct MStar_Ring_Buffer_Struct {
	unsigned char *addr;
	unsigned int size;
	unsigned char *w_ptr;
	unsigned char *r_ptr;
};

/* Define a STR (Suspend To Ram) data structure for MStar Audio DSP */
 struct MStar_STR_MODE_Struct {
	unsigned int status;
	unsigned int physical_addr;
	unsigned int bus_addr;
	unsigned int virtual_addr;
};

/* Define a DMA Reader data structure for MStar Audio DSP */
struct MStar_DMA_Reader_Struct {
	struct MStar_Ring_Buffer_Struct buffer;
	struct MStar_STR_MODE_Struct str_mode_info;
	unsigned int initialized_status;
	unsigned int channel_mode;
	unsigned int sample_rate;
	unsigned int period_size;
	unsigned int high_threshold;
};

/* Define a DMA Reader data structure for MStar Audio DSP */
struct MStar_PCM_Capture_Struct {
	struct MStar_Ring_Buffer_Struct buffer;
	struct MStar_STR_MODE_Struct str_mode_info;
	unsigned int initialized_status;
	unsigned int channel_mode;
	unsigned int sample_rate;
};


/*
 * ============================================================================
 * Forward Declarations
 * ============================================================================
 */
extern void Chip_Flush_Memory(void);

#endif /* _MHAL_ALSA_DRIVER_HEADER */


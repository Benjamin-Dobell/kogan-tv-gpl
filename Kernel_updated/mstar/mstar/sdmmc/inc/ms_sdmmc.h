////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2012 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MS_SDMMC_H
#define MS_SDMMC_H

#include <linux/cdev.h>

//***********************************************************************************************************
// Config Setting (Externel)
//***********************************************************************************************************

#if defined(CONFIG_MSTAR_SDMMC_TWOCARDS)
#define EN_SDMMC_TWO_CARDS     (TRUE)
#else
#define EN_SDMMC_TWO_CARDS     (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC_REVCDZ)
#define EN_SDMMC_CDZREV        (TRUE)
#else
#define EN_SDMMC_CDZREV        (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC_REVWP)
#define EN_SDMMC_WPREV         (TRUE)
#else
#define EN_SDMMC_WPREV         (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC1_HOTP)
#define EN_SDMMC1_HOTP         (TRUE)
#else
#define EN_SDMMC1_HOTP         (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC2_HOTP)
#define EN_SDMMC2_HOTP         (TRUE)
#else
#define EN_SDMMC2_HOTP         (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC1_FAKECDZ)
#define EN_SDMMC1_FAKECDZ      (TRUE)
#else
#define EN_SDMMC1_FAKECDZ      (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC2_FAKECDZ)
#define EN_SDMMC2_FAKECDZ      (TRUE)
#else
#define EN_SDMMC2_FAKECDZ      (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC_HIGHCLK)
#define MAX_CLK_SPEED1         24600000
#define MAX_CLK_SPEED2         24600000
#else
#define MAX_CLK_SPEED1         19200000
#define MAX_CLK_SPEED2         19200000
#endif


//***********************************************************************************************************
typedef enum
{
	EV_SDMMC1 = 0,
	EV_SDMMC2 = 1,

} SlotEmType;

struct ms_sdmmc_host
{
	struct platform_device	*pdev;
	struct ms_sdmmc_slot *sdmmc_slot[2];
};

struct ms_sdmmc_slot
{
	struct mmc_host		*mmc;

	unsigned int	slotNo;   	//Slot No.
	unsigned int	mieIRQNo;	//MIE IRQ No.
	unsigned int	cdzIRQNo;	//CDZ IRQ No.
	unsigned int	irqIP;		//IP for INT
	unsigned int	currClk;	//Current Clock

	int ro;		  //WP
	int card_det;		//Card Detect

	/****** DMA buffer used for transmitting *******/
	u32 *dma_buffer;
	dma_addr_t dma_phy_addr;

	/***** Tasklet for hotplug ******/
	struct tasklet_struct   hotplug_tasklet;

};  /* struct ms_sdmmc_hot*/


#endif


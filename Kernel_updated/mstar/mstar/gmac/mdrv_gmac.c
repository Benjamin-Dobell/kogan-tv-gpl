////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (��MStar Confidential Information��) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   devGMAC.c
/// @brief  GMAC Driver
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/init.h>
#include <linux/autoconf.h>
#include <linux/mii.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/pci.h>
#include <linux/crc32.h>
#include <linux/ethtool.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#if defined(CONFIG_MIPS)
#include <asm/mips-boards/prom.h>
#include "mhal_chiptop_reg.h"
#elif defined(CONFIG_ARM)
#include <asm/arm-boards/prom.h>
#include <asm/mach/map.h>
#endif
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
#include "chip_int.h"
#include "mhal_gmac.h"
#include "mdrv_gmac.h"
#include "chip_setup.h"

#ifdef CONFIG_GMAC_SUPPLY_RNG
#include <linux/input.h>
#include <random.h>
#include "mhal_rng_reg.h"
#endif
//--------------------------------------------------------------------------------------------------
//  Forward declaration
//--------------------------------------------------------------------------------------------------
#define GMAC_RX_TMR         (0)
#define GMAC_LINK_TMR       (1)

#define GMAC_CHECK_LINK_TIME    	(HZ)
#define GMAC_IER_FOR_INT_JULIAN_D   		(0x0000E4B5)
#define GMAC_CHECK_CNT              (500000)

#define GMAC_TX_PTK_BASE            (GMAC_TX_SKB_BASE + GMAC_RAM_VA_PA_OFFSET)

#define GMAC_ALBANY_OUI_MSB              (0)
#define GMAC_RTL_8210                    (0x1C)

#define GMAC_RX_THROUGHPUT_TEST 0
#define GMAC_TX_THROUGHPUT_TEST 0


//--------------------------------------------------------------------------------------------------
//  Local variable
//--------------------------------------------------------------------------------------------------
unsigned char gmac_phyaddr = 0;
u32 gmac_contiROVR = 0;
u32 gmac_initstate= 0;
u8 gmac_txidx =0;
spinlock_t gmac_lock;

// 0x78c9: link is down. 
static u32 gmac_phy_status_register = 0x78c9;

struct sk_buff *gmac_pseudo_packet;

#if GMAC_TX_THROUGHPUT_TEST
unsigned char gmac_packet_content[] = {
0xa4, 0xba, 0xdb, 0x95, 0x25, 0x29, 0x00, 0x30, 0x1b, 0xba, 0x02, 0xdb, 0x08, 0x00, 0x45, 0x00,
0x05, 0xda, 0x69, 0x0a, 0x40, 0x00, 0x40, 0x11, 0xbe, 0x94, 0xac, 0x10, 0x5a, 0xe3, 0xac, 0x10,
0x5a, 0x70, 0x92, 0x7f, 0x13, 0x89, 0x05, 0xc6, 0x0c, 0x5b, 0x00, 0x00, 0x03, 0x73, 0x00, 0x00,
0x00, 0x65, 0x00, 0x06, 0xe1, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
0x13, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x18, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39
};
#endif

//-------------------------------------------------------------------------------------------------
//  Data structure
//-------------------------------------------------------------------------------------------------
static struct timer_list GMAC_timer, GMAC_Link_timer;
#if GMAC_RX_THROUGHPUT_TEST
static struct timer_list GMAC_RX_timer;
#endif
static struct net_device *gmac_dev;
//-------------------------------------------------------------------------------------------------
//  GMAC Function
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_tx (struct sk_buff *skb, struct net_device *dev);
static void MDev_GMAC_timer_callback( unsigned long value );
static int MDev_GMAC_SwReset(struct net_device *dev);
static void MDev_GMAC_Send_PausePkt(struct net_device* dev);
unsigned long gmac_oldTime;
static unsigned long getCurMs(void)
{
	struct timeval tv;
	unsigned long curMs;

	do_gettimeofday(&tv);
    curMs = tv.tv_usec/1000;
	curMs += tv.tv_sec * 1000;
    return curMs;
}

#if GMAC_RX_THROUGHPUT_TEST
int gmac_receive_bytes = 0;
static void RX_timer_callback( unsigned long value){
    int get_bytes = receive_bytes;
    int cur_speed;
    receive_bytes = 0;

    cur_speed = get_bytes*8/20/1024;
    printk(" %dkbps",cur_speed);
    RX_timer.expires = jiffies + 20*GMAC_CHECK_LINK_TIME;
    add_timer(&RX_timer);
}
#endif

//-------------------------------------------------------------------------------------------------
// PHY MANAGEMENT
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Access the PHY to determine the current Link speed and Mode, and update the
// MAC accordingly.
// If no link or auto-negotiation is busy, then no changes are made.
// Returns:  0 : OK
//              -1 : No link
//              -2 : AutoNegotiation still in progress
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_update_linkspeed (struct net_device *dev)
{
    u32 bmsr, bmcr, LocPtrA;
    u32 speed, duplex;

    // Link status is latched, so read twice to get current value //
    MHal_GMAC_read_phy (gmac_phyaddr, MII_BMSR, &bmsr);
    MHal_GMAC_read_phy (gmac_phyaddr, MII_BMSR, &bmsr);
    if (!(bmsr & BMSR_LSTATUS)){
        return -1;          //no link //
    }
    MHal_GMAC_read_phy (gmac_phyaddr, MII_BMCR, &bmcr);

    if (bmcr & BMCR_ANENABLE)
    {               //AutoNegotiation is enabled //
        if (!(bmsr & BMSR_ANEGCOMPLETE))
        {
            GMAC_DBG("==> AutoNegotiation still in progress\n");
            return -2;
        }

        MHal_GMAC_read_phy (gmac_phyaddr, MII_LPA, &LocPtrA);
        if ((LocPtrA & LPA_100FULL) || (LocPtrA & LPA_100HALF))
        {
            speed = SPEED_100;
        }
        else
        {
            speed = SPEED_10;
        }
        if ((LocPtrA & LPA_100FULL) || (LocPtrA & LPA_10FULL))
            duplex = DUPLEX_FULL;
        else
            duplex = DUPLEX_HALF;
    }
    else
    {
        speed = (bmcr & BMCR_SPEED100) ? SPEED_100 : SPEED_10;
        duplex = (bmcr & BMCR_FULLDPLX) ? DUPLEX_FULL : DUPLEX_HALF;
    }

    // Update the MAC //
    MHal_GMAC_update_speed_duplex(speed,duplex);
    return 0;
}

static int MDev_GMAC_get_info(struct net_device *dev)
{
    u32 bmsr, bmcr, LocPtrA;
    u32 uRegStatus =0;

    // Link status is latched, so read twice to get current value //
    MHal_GMAC_read_phy (gmac_phyaddr, MII_BMSR, &bmsr);
    MHal_GMAC_read_phy (gmac_phyaddr, MII_BMSR, &bmsr);
    if (!(bmsr & BMSR_LSTATUS)){
        uRegStatus &= ~GMAC_ETHERNET_TEST_RESET_STATE;
        uRegStatus |= GMAC_ETHERNET_TEST_NO_LINK; //no link //
    }
    MHal_GMAC_read_phy (gmac_phyaddr, MII_BMCR, &bmcr);

    if (bmcr & BMCR_ANENABLE)
    {
        //AutoNegotiation is enabled //
        if (!(bmsr & BMSR_ANEGCOMPLETE))
        {
            uRegStatus &= ~GMAC_ETHERNET_TEST_RESET_STATE;
            uRegStatus |= GMAC_ETHERNET_TEST_AUTO_NEGOTIATION; //AutoNegotiation //
        }
        else
        {
            uRegStatus &= ~GMAC_ETHERNET_TEST_RESET_STATE;
            uRegStatus |= GMAC_ETHERNET_TEST_LINK_SUCCESS; //link success //
        }

        MHal_GMAC_read_phy (gmac_phyaddr, MII_LPA, &LocPtrA);
        if ((LocPtrA & LPA_100FULL) || (LocPtrA & LPA_100HALF))
        {
            uRegStatus |= GMAC_ETHERNET_TEST_SPEED_100M; //SPEED_100//
        }
        else
        {
            uRegStatus &= ~GMAC_ETHERNET_TEST_SPEED_100M; //SPEED_10//
        }

        if ((LocPtrA & LPA_100FULL) || (LocPtrA & LPA_10FULL))
        {
            uRegStatus |= GMAC_ETHERNET_TEST_DUPLEX_FULL; //DUPLEX_FULL//
        }
        else
        {
            uRegStatus &= ~GMAC_ETHERNET_TEST_DUPLEX_FULL; //DUPLEX_HALF//
        }
    }
    else
    {
        if(bmcr & BMCR_SPEED100)
        {
            uRegStatus |= GMAC_ETHERNET_TEST_SPEED_100M; //SPEED_100//
        }
        else
        {
            uRegStatus &= ~GMAC_ETHERNET_TEST_SPEED_100M; //SPEED_10//
        }

        if(bmcr & BMCR_FULLDPLX)
        {
            uRegStatus |= GMAC_ETHERNET_TEST_DUPLEX_FULL; //DUPLEX_FULL//
        }
        else
        {
            uRegStatus &= ~GMAC_ETHERNET_TEST_DUPLEX_FULL; //DUPLEX_HALF//
        }
    }

    return uRegStatus;
}

//-------------------------------------------------------------------------------------------------
//Program the hardware MAC address from dev->dev_addr.
//-------------------------------------------------------------------------------------------------
void MDev_GMAC_update_mac_address (struct net_device *dev)
{
    u32 value;
    value = (dev->dev_addr[3] << 24) | (dev->dev_addr[2] << 16) | (dev->dev_addr[1] << 8) |(dev->dev_addr[0]);
    MHal_GMAC_Write_SA1L(value);
    value = (dev->dev_addr[5] << 8) | (dev->dev_addr[4]);
    MHal_GMAC_Write_SA1H(value);
}

//-------------------------------------------------------------------------------------------------
// ADDRESS MANAGEMENT
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Set the ethernet MAC address in dev->dev_addr
//-------------------------------------------------------------------------------------------------
static void MDev_GMAC_get_mac_address (struct net_device *dev)
{
    char addr[6];
    u32 HiAddr, LoAddr;

    // Check if bootloader set address in Specific-Address 1 //
    HiAddr = MHal_GMAC_get_SA1H_addr();
    LoAddr = MHal_GMAC_get_SA1L_addr();

    addr[0] = (LoAddr & 0xff);
    addr[1] = (LoAddr & 0xff00) >> 8;
    addr[2] = (LoAddr & 0xff0000) >> 16;
    addr[3] = (LoAddr & 0xff000000) >> 24;
    addr[4] = (HiAddr & 0xff);
    addr[5] = (HiAddr & 0xff00) >> 8;

    if (is_valid_ether_addr (addr))
    {
        memcpy (dev->dev_addr, &addr, 6);
        return;
    }
    // Check if bootloader set address in Specific-Address 2 //
    HiAddr = MHal_GMAC_get_SA2H_addr();
    LoAddr = MHal_GMAC_get_SA2L_addr();
    addr[0] = (LoAddr & 0xff);
    addr[1] = (LoAddr & 0xff00) >> 8;
    addr[2] = (LoAddr & 0xff0000) >> 16;
    addr[3] = (LoAddr & 0xff000000) >> 24;
    addr[4] = (HiAddr & 0xff);
    addr[5] = (HiAddr & 0xff00) >> 8;

    if (is_valid_ether_addr (addr))
    {
        memcpy (dev->dev_addr, &addr, 6);
        return;
    }
}

#ifdef GMAC_URANUS_ETHER_ADDR_CONFIGURABLE
//-------------------------------------------------------------------------------------------------
// Store the new hardware address in dev->dev_addr, and update the MAC.
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_set_mac_address (struct net_device *dev, void *addr)
{
    struct sockaddr *address = addr;
    if (!is_valid_ether_addr (address->sa_data))
        return -EADDRNOTAVAIL;

    memcpy (dev->dev_addr, address->sa_data, dev->addr_len);
    MDev_GMAC_update_mac_address (dev);
    return 0;
}
#endif

#if 0
//-------------------------------------------------------------------------------------------------
// Add multicast addresses to the internal multicast-hash table.
//-------------------------------------------------------------------------------------------------
static void MDev_GMAC_sethashtable (struct net_device *dev)
{
   struct dev_mc_list *curr;
   u32 mc_filter[2], i, bitnr;

   mc_filter[0] = mc_filter[1] = 0;

   curr = dev->mc_list;
   for (i = 0; i < dev->mc_count; i++, curr = curr->next)
   {
       if (!curr)
           break;          // unexpected end of list //

       bitnr = ether_crc (ETH_ALEN, curr->dmi_addr) >> 26;
       mc_filter[bitnr >> 5] |= 1 << (bitnr & 31);
   }

   MHal_GMAC_update_HSH(mc_filter[0],mc_filter[1]);
}
#endif

//-------------------------------------------------------------------------------------------------
//Enable/Disable promiscuous and multicast modes.
//-------------------------------------------------------------------------------------------------
static void MDev_GMAC_set_rx_mode (struct net_device *dev)
{
    u32 uRegVal;
    uRegVal  = MHal_GMAC_Read_CFG();
	
    if (dev->flags & IFF_PROMISC)
    {   // Enable promiscuous mode //
        uRegVal |= GMAC_CAF;
    }
    else if (dev->flags & (~IFF_PROMISC))
    {   // Disable promiscuous mode //
        uRegVal &= ~GMAC_CAF;
    }
    MHal_GMAC_Write_CFG(uRegVal);

    if (dev->flags & IFF_ALLMULTI)
    {   // Enable all multicast mode //
        MHal_GMAC_update_HSH(-1,-1);
        uRegVal |= GMAC_MTI;
    }
    else if (dev->flags & IFF_MULTICAST)
    {   // Enable specific multicasts//
        //MDev_GMAC_sethashtable (dev);
        MHal_GMAC_update_HSH(-1,-1);
        uRegVal |= GMAC_MTI;
    }
    else if (dev->flags & ~(IFF_ALLMULTI | IFF_MULTICAST))
    {   // Disable all multicast mode//
        MHal_GMAC_update_HSH(0,0);
        uRegVal &= ~GMAC_MTI;
    }
	
    MHal_GMAC_Write_CFG(uRegVal);
}
//-------------------------------------------------------------------------------------------------
// IOCTL
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Enable/Disable MDIO
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_mdio_read (struct net_device *dev, int phy_id, int location)
{
    u32 value;
    MHal_GMAC_read_phy (phy_id, location, &value);
    return value;
}

static void MDev_GMAC_mdio_write (struct net_device *dev, int phy_id, int location, int value)
{
    MHal_GMAC_write_phy (phy_id, location, value);
}

//-------------------------------------------------------------------------------------------------
//ethtool support.
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_ethtool_ioctl (struct net_device *dev, void *useraddr)
{
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    u32 ethcmd;
    int res = 0;

    if (copy_from_user (&ethcmd, useraddr, sizeof (ethcmd)))
        return -EFAULT;

    spin_lock_irq (LocPtr->lock);

    switch (ethcmd)
    {
        case ETHTOOL_GSET:
        {
            struct ethtool_cmd ecmd = { ETHTOOL_GSET };
            res = mii_ethtool_gset (&LocPtr->mii, &ecmd);
            if (LocPtr->phy_media == PORT_FIBRE)
            {   //override media type since mii.c doesn't know //
                ecmd.supported = SUPPORTED_FIBRE;
                ecmd.port = PORT_FIBRE;
            }
            if (copy_to_user (useraddr, &ecmd, sizeof (ecmd)))
                res = -EFAULT;
            break;
        }
        case ETHTOOL_SSET:
        {
            struct ethtool_cmd ecmd;
            if (copy_from_user (&ecmd, useraddr, sizeof (ecmd)))
                res = -EFAULT;
            else
                res = mii_ethtool_sset (&LocPtr->mii, &ecmd);
            break;
        }
        case ETHTOOL_NWAY_RST:
        {
            res = mii_nway_restart (&LocPtr->mii);
            break;
        }
        case ETHTOOL_GLINK:
        {
            struct ethtool_value edata = { ETHTOOL_GLINK };
            edata.data = mii_link_ok (&LocPtr->mii);
            if (copy_to_user (useraddr, &edata, sizeof (edata)))
                res = -EFAULT;
            break;
        }
        default:
            res = -EOPNOTSUPP;
    }
    spin_unlock_irq (LocPtr->lock);
    return res;
}

//-------------------------------------------------------------------------------------------------
// User-space ioctl interface.
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_ioctl (struct net_device *dev, struct ifreq *rq, int cmd)
{
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    struct mii_ioctl_data *data = if_mii(rq);

    if (!netif_running(dev))
    {
        rq->ifr_metric = GMAC_ETHERNET_TEST_INIT_FAIL;
    }

    switch (cmd)
    {
        case SIOCGMIIPHY:
            data->phy_id = (gmac_phyaddr & 0x1F);
            return 0;

        case SIOCDEVPRIVATE:
            rq->ifr_metric = (MDev_GMAC_get_info(gmac_dev)|gmac_initstate);
            return 0;

        case SIOCDEVON:
            MHal_GMAC_Power_On_Clk();
            return 0;

        case SIOCDEVOFF:
            MHal_GMAC_Power_Off_Clk();
            return 0;

        case SIOCGMIIREG:
            // check PHY's register 1.
            if((data->reg_num & 0x1f) == 0x1) 
            {
                // PHY's register 1 value is set by timer callback function.
                spin_lock_irq(LocPtr->lock);
                data->val_out = gmac_phy_status_register;
                spin_unlock_irq(LocPtr->lock);
            }
            else
            {
                MHal_GMAC_read_phy((gmac_phyaddr & 0x1F), (data->reg_num & 0x1f), (u32 *)&(data->val_out));
            }
            return 0;

        case SIOCSMIIREG:
            if (!capable(CAP_NET_ADMIN))
                return -EPERM;
            MHal_GMAC_write_phy((gmac_phyaddr & 0x1F), (data->reg_num & 0x1f), data->val_in);
            return 0;

        case SIOCETHTOOL:
            return MDev_GMAC_ethtool_ioctl (dev, (void *) rq->ifr_data);

        default:
            return -EOPNOTSUPP;
    }
}
//-------------------------------------------------------------------------------------------------
// MAC
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//Initialize and start the Receiver and Transmit subsystems
//-------------------------------------------------------------------------------------------------
static void MDev_GMAC_start (struct net_device *dev)
{
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    struct gmac_recv_desc_bufs *dlist, *dlist_phys;
#ifndef GMAC_SOFTWARE_DESCRIPTOR
    int i;
#endif
    u32 uRegVal;

    dlist = LocPtr->dlist;
    dlist_phys = LocPtr->dlist_phys;
#ifdef GMAC_SOFTWARE_DESCRIPTOR
    dlist->descriptors[GMAC_MAX_RX_DESCR - 1].addr |= GMAC_DESC_WRAP;
#else
    for(i = 0; i < GMAC_MAX_RX_DESCR; i++)
    {
        dlist->descriptors[i].addr = 0;
        dlist->descriptors[i].size = 0;
    }
    // Set the Wrap bit on the last descriptor //
    dlist->descriptors[GMAC_MAX_RX_DESCR - 1].addr = GMAC_DESC_WRAP;
#endif //#ifndef SOFTWARE_DESCRIPTOR
    // set offset of read and write pointers in the receive circular buffer //
    uRegVal = MHal_GMAC_Read_BUFF();
    uRegVal = (GMAC_RX_BUFFER_BASE|GMAC_RX_BUFFER_SEL) - GMAC_MIU0_BUS_BASE;
    MHal_GMAC_Write_BUFF(uRegVal);
    MHal_GMAC_Write_RDPTR(0);
    MHal_GMAC_Write_WRPTR(0);

    // Program address of descriptor list in Rx Buffer Queue register //
    uRegVal = ((GMAC_REG) & dlist_phys->descriptors)- GMAC_RAM_VA_PA_OFFSET - GMAC_MIU0_BUS_BASE;
    MHal_GMAC_Write_RBQP(uRegVal);

    //Reset buffer index//
    LocPtr->rxBuffIndex = 0;

    // Enable Receive and Transmit //
    uRegVal = MHal_GMAC_Read_CTL();
    uRegVal |= (GMAC_RE | GMAC_TE);
    MHal_GMAC_Write_CTL(uRegVal);
}

//-------------------------------------------------------------------------------------------------
// Open the ethernet interface
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_open (struct net_device *dev)
{
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    u32 uRegVal;
    int ret;

    spin_lock_irq (LocPtr->lock);
    ret = MDev_GMAC_update_linkspeed(dev);
    spin_unlock_irq (LocPtr->lock);
	 
    if (!is_valid_ether_addr (dev->dev_addr))
       return -EADDRNOTAVAIL;

    //ato  GMAC_SYS->PMC_PCER = 1 << GMAC_ID_GMAC;   //Re-enable Peripheral clock //
    MHal_GMAC_Power_On_Clk();
    uRegVal = MHal_GMAC_Read_CTL();
    uRegVal |= GMAC_CSR;
    MHal_GMAC_Write_CTL(uRegVal);
    // Enable PHY interrupt //
    MHal_GMAC_enable_phyirq ();

    // Enable MAC interrupts //
#ifndef GMAC_INT_JULIAN_D
    uRegVal = GMAC_INT_RCOM | GMAC_IER_FOR_INT_JULIAN_D;
    MHal_GMAC_Write_IER(uRegVal);
#else
    MHal_GMAC_Write_IER(GMAC_IER_FOR_INT_JULIAN_D);
#endif

    LocPtr->ep_flag |= GMAC_EP_FLAG_OPEND;

    MDev_GMAC_start (dev);
    netif_start_queue (dev);

    init_timer( &GMAC_Link_timer );
    GMAC_Link_timer.data = GMAC_LINK_TMR;
    GMAC_Link_timer.function = MDev_GMAC_timer_callback;
    GMAC_Link_timer.expires = jiffies + GMAC_CHECK_LINK_TIME;
    add_timer(&GMAC_Link_timer);

    /* check if network linked */
    if (-1 == ret)
        netif_carrier_off(dev);
    else if(0 == ret)
        netif_carrier_on(dev);

    return 0;
}

//-------------------------------------------------------------------------------------------------
// Close the interface
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_close (struct net_device *dev)
{
    u32 uRegVal;
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    //Disable Receiver and Transmitter //
    uRegVal = MHal_GMAC_Read_CTL();
    uRegVal &= ~(GMAC_TE | GMAC_RE);
    MHal_GMAC_Write_CTL(uRegVal);
    // Disable PHY interrupt //
    MHal_GMAC_disable_phyirq ();
#ifndef GMAC_INT_JULIAN_D
    //Disable MAC interrupts //
    uRegVal = GMAC_INT_RCOM | GMAC_IER_FOR_INT_JULIAN_D;
    MHal_GMAC_Write_IDR(uRegVal);
#else
    MHal_GMAC_Write_IDR(GMAC_IER_FOR_INT_JULIAN_D);
#endif
    netif_stop_queue (dev);
    netif_carrier_off(dev);
    del_timer(&GMAC_Link_timer);
    //MHal_GMAC_Power_Off_Clk();
    GmacThisBCE.connected = 0;
    LocPtr->ep_flag &= (~GMAC_EP_FLAG_OPEND);

    return 0;
}

//-------------------------------------------------------------------------------------------------
// Update the current statistics from the internal statistics registers.
//-------------------------------------------------------------------------------------------------
static struct net_device_stats * MDev_GMAC_stats (struct net_device *dev)
{
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    int ale, lenerr, seqe, lcol, ecol;
    if (netif_running (dev))
    {
        LocPtr->stats.rx_packets += MHal_GMAC_Read_OK();            /* Good frames received */
        ale = MHal_GMAC_Read_ALE();
        LocPtr->stats.rx_frame_errors += ale;                       /* Alignment errors */
        lenerr = MHal_GMAC_Read_ELR();
        LocPtr->stats.rx_length_errors += lenerr;                   /* Excessive Length or Undersize Frame error */
        seqe = MHal_GMAC_Read_SEQE();
        LocPtr->stats.rx_crc_errors += seqe;                        /* CRC error */
        LocPtr->stats.rx_fifo_errors += MHal_GMAC_Read_ROVR();
        LocPtr->stats.rx_errors += ale + lenerr + seqe + MHal_GMAC_Read_SE() + MHal_GMAC_Read_RJB();
        LocPtr->stats.tx_packets += MHal_GMAC_Read_FRA();           /* Frames successfully transmitted */
        LocPtr->stats.tx_fifo_errors += MHal_GMAC_Read_TUE();       /* Transmit FIFO underruns */
        LocPtr->stats.tx_carrier_errors += MHal_GMAC_Read_CSE();    /* Carrier Sense errors */
        LocPtr->stats.tx_heartbeat_errors += MHal_GMAC_Read_SQEE(); /* Heartbeat error */
        lcol = MHal_GMAC_Read_LCOL();
        ecol = MHal_GMAC_Read_ECOL();
        LocPtr->stats.tx_window_errors += lcol;                     /* Late collisions */
        LocPtr->stats.tx_aborted_errors += ecol;                    /* 16 collisions */
        LocPtr->stats.collisions += MHal_GMAC_Read_SCOL() + MHal_GMAC_Read_MCOL() + lcol + ecol;
    }
    return &LocPtr->stats;
}

static int MDev_GMAC_TxReset(void)
{
    u32 val = MHal_GMAC_Read_CTL() & 0x000001FF;

    MHal_GMAC_Write_CTL((val & ~GMAC_TE));
    MHal_GMAC_Write_TCR(0);
    MHal_GMAC_Write_CTL((MHal_GMAC_Read_CTL() | GMAC_TE));
    return 0;
}

static int MDev_GMAC_CheckTSR(void)
{
    u32 check;
    u32 tsrval = 0;

    #ifdef GMAC_TX_QUEUE_4
    u8  avlfifo[8] = {0};
    u8  avlfifoidx;
    u8  avlfifoval = 0;

    for (check = 0; check < GMAC_CHECK_CNT; check++)
    {
        tsrval = MHal_GMAC_Read_TSR();
		
        avlfifo[0] = ((tsrval & GMAC_IDLETSR) != 0)? 1 : 0;
        avlfifo[1] = ((tsrval & GMAC_BNQ)!= 0)? 1 : 0;
        avlfifo[2] = ((tsrval & GMAC_TBNQ) != 0)? 1 : 0;
        avlfifo[3] = ((tsrval & GMAC_FBNQ) != 0)? 1 : 0;
        avlfifo[4] = ((tsrval & GMAC_FIFO1IDLE) !=0)? 1 : 0;
        avlfifo[5] = ((tsrval & GMAC_FIFO2IDLE) != 0)? 1 : 0;
        avlfifo[6] = ((tsrval & GMAC_FIFO3IDLE) != 0)? 1 : 0;
        avlfifo[7] = ((tsrval & GMAC_FIFO4IDLE) != 0)? 1 : 0;

        avlfifoval = 0;

        for(avlfifoidx = 0; avlfifoidx < 8; avlfifoidx++)
        {
            avlfifoval += avlfifo[avlfifoidx];
        }
   
        if (avlfifoval > 4)
            return NETDEV_TX_OK;
    }
    #else
    for (check = 0; check < GMAC_CHECK_CNT; check++)
    {
        tsrval = MHal_GMAC_Read_TSR();
        if ((tsrval & GMAC_IDLETSR) || (tsrval & GMAC_BNQ))
            return NETDEV_TX_OK;
    }
    #endif

    GMAC_DBG("Err CheckTSR:0x%x\n", tsrval);
    MDev_GMAC_TxReset();

    return NETDEV_TX_BUSY;
}

static u8 pause_pkt[] =
{
    //DA - multicast
    0x01, 0x80, 0xC2, 0x00, 0x00, 0x01,
    //SA
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //Len-Type
    0x88, 0x08,
    //Ctrl code
    0x00, 0x01,
    //Ctrl para 8192
    0x20, 0x00
};

static dma_addr_t get_tx_addr(void)
{
    dma_addr_t addr;

    addr = GMAC_TX_PTK_BASE + 16384*gmac_txidx;
    gmac_txidx ++;
    gmac_txidx = gmac_txidx % GMAC_TX_RING_SIZE;
    return addr;
}

void MDrv_GMAC_DumpMem(u32 addr, u32 len)
{
    u8 *ptr = (u8 *)addr;
    u32 i;

    printk("\n ===== Dump %lx =====\n", (long unsigned int)ptr);
    for (i=0; i<len; i++)
    {
        if ((u32)i%0x10 ==0)
            printk("%lx: ", (long unsigned int)ptr);
        if (*ptr < 0x10)
            printk("0%x ", *ptr);
        else
            printk("%x ", *ptr);
        if ((u32)i%0x10 == 0x0f)
            printk("\n");
	ptr++;
    }
    printk("\n");
}

//Background send
static int MDev_GMAC_BGsend(struct net_device* dev, u32 addr, int len )
{
	dma_addr_t skb_addr;
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);

    if (NETDEV_TX_OK != MDev_GMAC_CheckTSR())
        return NETDEV_TX_BUSY;

    skb_addr = get_tx_addr();
    memcpy((void*)skb_addr,(void *)addr, len);

    LocPtr->stats.tx_bytes += len;

#ifdef CHIP_FLUSH_READ
    #if defined(CONFIG_MIPS)
    if((unsigned int)skb_addr < 0xC0000000)
    {
        Chip_Flush_Memory_Range((unsigned int)skb_addr&0x0FFFFFFF, len);
    }
    else
    {
        Chip_Flush_Memory_Range(0, 0xFFFFFFFF);
    }
    #elif defined(CONFIG_ARM)
        Chip_Flush_Memory_Range(0, 0xFFFFFFFF);
    #else
	    #ERROR
    #endif
#endif

    //Set address of the data in the Transmit Address register //
    MHal_GMAC_Write_TAR(skb_addr - GMAC_RAM_VA_PA_OFFSET - GMAC_MIU0_BUS_BASE);

    // Set length of the packet in the Transmit Control register //
    MHal_GMAC_Write_TCR(len);

    return NETDEV_TX_OK;
}

static void MDev_GMAC_Send_PausePkt(struct net_device* dev)
{
    u32 val = MHal_GMAC_Read_CTL() & 0x000001FF;

    //Disable Rx
    MHal_GMAC_Write_CTL((val & ~GMAC_RE));
    memcpy(&pause_pkt[6], dev->dev_addr, 6);
    MDev_GMAC_BGsend(dev, (u32)pause_pkt, sizeof(pause_pkt));
    //Enable Rx
    MHal_GMAC_Write_CTL((MHal_GMAC_Read_CTL() | GMAC_RE));
}

//-------------------------------------------------------------------------------------------------
//Transmit packet.
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_tx (struct sk_buff *skb, struct net_device *dev)
{
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    unsigned long flags;
    dma_addr_t skb_addr;

    spin_lock_irqsave(LocPtr->lock, flags);
    if (skb->len > GMAC_MTU)
    {
        GMAC_DBG("Wrong Tx len:%u\n", skb->len);
        spin_unlock_irqrestore(LocPtr->lock, flags);
        return NETDEV_TX_BUSY;
    }

    if (NETDEV_TX_OK != MDev_GMAC_CheckTSR())
    {
	    spin_unlock_irqrestore(LocPtr->lock, flags);
	    return NETDEV_TX_BUSY; //check
    }

#ifndef GMAC_TX_SKB_PTR
    #ifndef GMAC_TX_QUEUE_4
        skb_addr = get_tx_addr();
        memcpy((void*)skb_addr, skb->data, skb->len);
    #else
        //tx_fifo->skb_physaddr = dma_map_single(NULL, skb->data, skb->len,DMA_TO_DEVICE);
        skb_addr = get_tx_addr();
        memcpy((void*)skb_addr, skb->data, skb->len);
	#endif
#else
    LocPtr->txpkt = dma_map_single(NULL, skb->data, skb->len,DMA_TO_DEVICE);
#endif

    if (!skb_addr)
    {
        dev_err(NULL,
                "dma map 2 failed (%p, %i). Dropping packet\n",
                skb->data, skb->len);
		spin_unlock_irqrestore(LocPtr->lock, flags);
        return -ENOMEM;
    }

    // Store packet information (to free when Tx completed) //
    //LocPtr->skb = skb;
    //LocPtr->skb_length =(int) skb->len;
    LocPtr->stats.tx_bytes += skb->len;

#ifdef GMAC_CHIP_FLUSH_READ
    #if defined(CONFIG_MIPS)
    if((unsigned int)skb_addr < 0xC0000000)
    {
        Chip_Flush_Memory_Range((unsigned int)skb_addr&0x0FFFFFFF, skb->len);
    }
    else
    {
        Chip_Flush_Memory_Range(0, 0xFFFFFFFF);
    }
    #elif defined(CONFIG_ARM)
        Chip_Flush_Memory_Range(0, 0xFFFFFFFF);
    #else
	    #ERROR
    #endif
#endif

    //Set address of the data in the Transmit Address register //
    MHal_GMAC_Write_TAR(skb_addr - GMAC_RAM_VA_PA_OFFSET - GMAC_MIU0_BUS_BASE);

    // Set length of the packet in the Transmit Control register //
    MHal_GMAC_Write_TCR(skb->len);

    //netif_stop_queue (dev);
    dev->trans_start = jiffies;
    dev_kfree_skb_irq(skb);
    spin_unlock_irqrestore(LocPtr->lock, flags);
    return NETDEV_TX_OK;
}

#ifdef GMAC_RX_SOFTWARE_DESCRIPTOR

//-------------------------------------------------------------------------------------------------
// Extract received frame from buffer descriptors and sent to upper layers.
// (Called from interrupt context)
// (Enable RX software discriptor)
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_rx (struct net_device *dev)
{
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    struct recv_desc_bufs *dlist;
    unsigned char *p_recv;
    u32 pktlen = 0;
    u32 retval = 0;
#ifdef GMAC_RX_SOFTWARE_DESCRIPTOR
    u32 uRegVal = 0;
    u32 RBQP_offset;
#else
    //u32 wrap_bit;
    struct sk_buff *skb;
#endif

#ifndef GMAC_INT_JULIAN_D
    u32 uRegVal = 0;
    int count = 0;
#endif

    dlist = LocPtr->dlist ;
    // If any Ownership bit is 1, frame received.
    //while ( (dlist->descriptors[LocPtr->rxBuffIndex].addr )& GMAC_DESC_DONE)
    do
    {
#ifdef GMAC_CHIP_FLUSH_READ
    #if defined(CONFIG_MIPS)
        Chip_Read_Memory_Range((unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex].addr)) & 0x0FFFFFFF, sizeof(dlist->descriptors[LocPtr->rxBuffIndex].addr));
    #elif defined(CONFIG_ARM)
        Chip_Inv_Cache_Range_VA_PA((unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex])),(unsigned int)__pa((&(dlist->descriptors[LocPtr->rxBuffIndex].addr))), sizeof(dlist->descriptors[LocPtr->rxBuffIndex]));
	#else
        #ERROR
    #endif
#endif
        if(!((dlist->descriptors[LocPtr->rxBuffIndex].addr) & GMAC_DESC_DONE))
        {
            break;
        }
        
        p_recv = (char *) ((dlist->descriptors[LocPtr->rxBuffIndex].addr) & ~(GMAC_DESC_DONE | GMAC_DESC_WRAP));
        pktlen = ((dlist->descriptors[LocPtr->rxBuffIndex].high_tag & 0x7) << 11) | (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & 0x7ff);    /* Length of frame including FCS */

    #if GMAC_RX_THROUGHPUT_TEST
    	receive_bytes += pktlen;
    #endif
#ifdef GMAC_CHIP_FLUSH_READ
    #if defined(CONFIG_MIPS)
        if((unsigned int)p_recv < 0xC0000000)
        {
        	Chip_Read_Memory_Range((unsigned int)(p_recv) & 0x0FFFFFFF, pktlen);
        }
        else
        {
       		Chip_Read_Memory_Range(0, 0xFFFFFFFF);
        }
    #elif defined(CONFIG_ARM)
        Chip_Inv_Cache_Range((unsigned int)(p_recv), pktlen);
    #else
	#ERROR
    #endif
#endif
        // the frame is not splitted in two parts //
        if(rx_skb[LocPtr->rxBuffIndex] == rx_skb_dummy)
        {
	        rx_skb[LocPtr->rxBuffIndex] = alloc_skb(SOFTWARE_DESC_LEN, GFP_ATOMIC);
            if(NULL == rx_skb[LocPtr->rxBuffIndex])
		    {
                rx_skb[LocPtr->rxBuffIndex] = rx_skb_dummy;
                LocPtr->stats.rx_dropped += 1;
                GMAC_DBG("Dummy, skb no enough memory!\n");
                goto NOBUF;
            }

            rx_abso_addr[LocPtr->rxBuffIndex] = (u32)rx_skb[LocPtr->rxBuffIndex]->data;
            //copy content of dummy to new skb
            *rx_skb[LocPtr->rxBuffIndex] = *rx_skb_dummy;
            rx_skb[LocPtr->rxBuffIndex]->data = (unsigned char *)rx_abso_addr[LocPtr->rxBuffIndex];
            memcpy(rx_skb[LocPtr->rxBuffIndex]->data, (void *)rx_abso_addr_dummy, pktlen);
	    }
    #ifdef GMAC_RX_BYTE_ALIGN_OFFSET
        else
        {
            skb_reserve(rx_skb[LocPtr->rxBuffIndex], 2);
        }
    #endif
        skb_put(rx_skb[LocPtr->rxBuffIndex], pktlen);
        // update consumer pointer//
        rx_skb[LocPtr->rxBuffIndex]->dev = dev;
        rx_skb[LocPtr->rxBuffIndex]->protocol = eth_type_trans (rx_skb[LocPtr->rxBuffIndex], dev);
        rx_skb[LocPtr->rxBuffIndex]->len = pktlen;
        dev->last_rx = jiffies;
        LocPtr->stats.rx_bytes += pktlen;

    #ifdef GMAC_RX_CHECKSUM
		if(((dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_DESC_TCP ) || (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_DESC_UDP )) \
			&& (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_DESC_IP_CSUM) \
			&& (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_DESC_TCP_UDP_CSUM) )
        {
            rx_skb[LocPtr->rxBuffIndex]->ip_summed = CHECKSUM_UNNECESSARY;
        }
        else
        {
            rx_skb[LocPtr->rxBuffIndex]->ip_summed = CHECKSUM_NONE;
        }
    #endif


    #if GMAC_RX_THROUGHPUT_TEST
        kfree_skb(rx_skb[LocPtr->rxBuffIndex]);
    #else
        retval = netif_rx (rx_skb[LocPtr->rxBuffIndex]);
    #endif
        rx_skb[LocPtr->rxBuffIndex] = alloc_skb(SOFTWARE_DESC_LEN, GFP_ATOMIC);
        if (NULL == rx_skb[LocPtr->rxBuffIndex])
        {
            rx_skb[LocPtr->rxBuffIndex] = rx_skb_dummy;
            GMAC_DBG("Skb no enough memory!\n");
        }

NOBUF:
        rx_abso_addr[LocPtr->rxBuffIndex] = (u32)rx_skb[LocPtr->rxBuffIndex]->data;

        RBQP_offset = LocPtr->rxBuffIndex * 8;
        if(LocPtr->rxBuffIndex<(MAX_RX_DESCR-1))
        {
            MHal_GMAC_WritRam32(RAM_VA_PA_OFFSET,RBQP_BASE + RBQP_offset, rx_abso_addr[LocPtr->rxBuffIndex]);
        }
        else
        {
            MHal_GMAC_WritRam32(RAM_VA_PA_OFFSET,RBQP_BASE + RBQP_offset, (rx_abso_addr[LocPtr->rxBuffIndex]+GMAC_DESC_WRAP));
        }

        if (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_MULTICAST)
        {
            LocPtr->stats.multicast++;
        }
        dlist->descriptors[LocPtr->rxBuffIndex].addr  &= ~GMAC_DESC_DONE;  /* reset ownership bit */

#ifdef GMAC_CHIP_FLUSH_READ
    #if defined(CONFIG_MIPS)
        Chip_Flush_Memory_Range((unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex].addr)) & 0x0FFFFFFF, sizeof(dlist->descriptors[LocPtr->rxBuffIndex].addr));
    #elif defined(CONFIG_ARM)
        Chip_Inv_Cache_Range_VA_PA((unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex])),(unsigned int)__pa((&(dlist->descriptors[LocPtr->rxBuffIndex]))), sizeof(dlist->descriptors[LocPtr->rxBuffIndex]));
#else
        #ERROR
    #endif
#endif

        //wrap after last buffer //
        LocPtr->rxBuffIndex++;
        if (LocPtr->rxBuffIndex == MAX_RX_DESCR)
        {
            LocPtr->rxBuffIndex = 0;
        }

        uRegVal = (u32)rx_skb[LocPtr->rxBuffIndex];
        MHal_GMAC_Write_RDPTR(uRegVal);
    #ifdef CONFIG_GMAC_SUPPLY_RNG
        {
            static unsigned long u32LastInputRNGJiff=0;
            unsigned long u32Jiff=jiffies;

            if ( time_after(u32Jiff, u32LastInputRNGJiff+InputRNGJiffThreshold) )
            {
                unsigned int u32Temp;
                unsigned short u16Temp;

                u32LastInputRNGJiff = u32Jiff;
                u16Temp = MIPS_REG(REG_RNG_OUT);
                memcpy((unsigned char *)&u32Temp+0, &u16Temp, 2);
                u16Temp = MIPS_REG(REG_RNG_OUT);
                memcpy((unsigned char *)&u32Temp+2, &u16Temp, 2);
                add_input_randomness(EV_MSC, MSC_SCAN, u32Temp);
            }
        }
    #endif
    #ifdef GMAC_INT_JULIAN_D
        if(ThisUVE.flagRBNA == 0)
        {
            GmacxReceiveNum--;
            if(GmacxReceiveNum==0)
                  return 0;
        }
    #else
        if( retval != 0)
        {
            uRegVal  = MHal_GMAC_Read_IDR();
            uRegVal |= (GMAC_INT_RCOM |GMAC_INT_RBNA);
            MHal_GMAC_Write_IDR(uRegVal);
            GMAC_timer.expires = jiffies+10;
            add_timer(&GMAC_timer);
            return 1;
        }

        if( ++count > 5 ) return 0;
    #endif//#ifdef GMAC_INT_JULIAN_D
    }while(1);
#ifdef GMAC_INT_JULIAN_D
    GmacxReceiveNum=0;
    GmacThisUVE.flagRBNA=0;
#endif

    return 0;
}

#else //#ifdef GMAC_RX_SOFTWARE_DESCRIPTOR

//-------------------------------------------------------------------------------------------------
// Extract received frame from buffer descriptors and sent to upper layers.
// (Called from interrupt context)
// (Disable RX software discriptor)
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_rx (struct net_device *dev)
{
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    struct gmac_recv_desc_bufs *dlist;
    unsigned char *p_recv;
    u32 pktlen;
    u32 retval=0;
    struct sk_buff *skb;


#ifndef GMAC_INT_JULIAN_D
    u32 uRegVal=0;
    int count = 0;
#endif

    dlist = LocPtr->dlist ;
    // If any Ownership bit is 1, frame received.
    //while ( (dlist->descriptors[LocPtr->rxBuffIndex].addr )& GMAC_DESC_DONE)
    do
    {
#ifdef GMAC_CHIP_FLUSH_READ
    #if defined(CONFIG_MIPS)
        Chip_Read_Memory_Range((unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex].addr)) & 0x0FFFFFFF, sizeof(dlist->descriptors[LocPtr->rxBuffIndex].addr));
    #elif defined(CONFIG_ARM)
        Chip_Inv_Cache_Range_VA_PA((unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex])),(unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex])) - GMAC_RAM_VA_PA_OFFSET ,sizeof(dlist->descriptors[LocPtr->rxBuffIndex]));
	#else
        #ERROR
    #endif
#endif
        if(!((dlist->descriptors[LocPtr->rxBuffIndex].addr) & GMAC_DESC_DONE))
        {
            break;
        }

        p_recv = (char *) ((((dlist->descriptors[LocPtr->rxBuffIndex].addr) & 0xFFFFFFFF) + GMAC_RAM_VA_PA_OFFSET + GMAC_MIU0_BUS_BASE) & ~(GMAC_DESC_DONE | GMAC_DESC_WRAP));
        pktlen = ((dlist->descriptors[LocPtr->rxBuffIndex].high_tag & 0x7) << 11) | (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & 0x7ff);    /* Length of frame including FCS */
		
    #if GMAC_RX_THROUGHPUT_TEST
    	receive_bytes += pktlen;
    #endif

        skb = alloc_skb (pktlen + 6, GFP_ATOMIC);

        if (skb != NULL)
        {
            skb_reserve (skb, 2);
    #ifdef GMAC_CHIP_FLUSH_READ
        #if defined(CONFIG_MIPS)
            if((unsigned int)p_recv < 0xC0000000)
            {
                Chip_Read_Memory_Range((unsigned int)(p_recv) & 0x0FFFFFFF, pktlen);
            }
            else
            {
                Chip_Read_Memory_Range(0, 0xFFFFFFFF);
            }
        #elif defined(CONFIG_ARM)
            Chip_Inv_Cache_Range_VA_PA((unsigned int)p_recv,((unsigned int)(p_recv) - GMAC_RAM_VA_PA_OFFSET) ,pktlen);
        #else
            #ERROR
        #endif
    #endif
            memcpy(skb_put(skb, pktlen), p_recv, pktlen);
            skb->dev = dev;
            skb->protocol = eth_type_trans (skb, dev);
            skb->len = pktlen;
            dev->last_rx = jiffies;
            LocPtr->stats.rx_bytes += pktlen;
        #if GMAC_RX_THROUGHPUT_TEST
            kfree_skb(skb);
        #else

        #ifdef GMAC_RX_CHECKSUM
            if(((dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_DESC_TCP ) || (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_DESC_UDP )) && \
               (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_DESC_IP_CSUM) && \
               (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_DESC_TCP_UDP_CSUM) )
            {
                skb->ip_summed = CHECKSUM_UNNECESSARY;
            }
            else
            {
                skb->ip_summed = CHECKSUM_NONE;
            }
        #endif
            retval = netif_rx (skb);
        #endif
        }
        else
        {
            LocPtr->stats.rx_dropped += 1;
        }

        if (dlist->descriptors[LocPtr->rxBuffIndex].low_tag & GMAC_MULTICAST)
        {
            LocPtr->stats.multicast++;
        }
        dlist->descriptors[LocPtr->rxBuffIndex].addr  &= ~GMAC_DESC_DONE;  /* reset ownership bit */
#ifdef GMAC_CHIP_FLUSH_READ
    #if defined(CONFIG_MIPS)
        Chip_Flush_Memory_Range((unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex].addr)) & 0x0FFFFFFF, sizeof(dlist->descriptors[LocPtr->rxBuffIndex].addr));
    #elif defined(CONFIG_ARM)
        Chip_Flush_Cache_Range_VA_PA((unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex])),(unsigned int)(&(dlist->descriptors[LocPtr->rxBuffIndex])) - GMAC_RAM_VA_PA_OFFSET ,sizeof(dlist->descriptors[LocPtr->rxBuffIndex]));
    #else
        #ERROR
    #endif
#endif
        //wrap after last buffer //
        LocPtr->rxBuffIndex++;
        if (LocPtr->rxBuffIndex == GMAC_MAX_RX_DESCR)
        {
            LocPtr->rxBuffIndex = 0;
        }

    #ifdef CONFIG_GMAC_SUPPLY_RNG
        {
            static unsigned long u32LastInputRNGJiff=0;
            unsigned long u32Jiff=jiffies;

            if ( time_after(u32Jiff, u32LastInputRNGJiff+InputRNGJiffThreshold) )
            {
                unsigned int u32Temp;
                unsigned short u16Temp;

                u32LastInputRNGJiff = u32Jiff;
                u16Temp = MIPS_REG(REG_RNG_OUT);
                memcpy((unsigned char *)&u32Temp+0, &u16Temp, 2);
                u16Temp = MIPS_REG(REG_RNG_OUT);
                memcpy((unsigned char *)&u32Temp+2, &u16Temp, 2);
                add_input_randomness(EV_MSC, MSC_SCAN, u32Temp);
            }
        }
    #endif
    #ifdef GMAC_INT_JULIAN_D
        if(GmacThisUVE.flagRBNA == 0)
        {
            GmacxReceiveNum--;
            if(GmacxReceiveNum==0)
                  return 0;
        }
    #else
        if( retval != 0)
        {
            uRegVal  = MHal_GMAC_Read_IDR();
            uRegVal |= (GMAC_INT_RCOM |GMAC_INT_RBNA);
            MHal_GMAC_Write_IDR(uRegVal);
            GMAC_timer.expires = jiffies+10;
            add_timer(&GMAC_timer);
            return 1;
        }

        if( ++count > 5 ) return 0;
    #endif//#ifdef GMAC_INT_JULIAN_D
    }while(1);
#ifdef GMAC_INT_JULIAN_D
    GmacxReceiveNum=0;
    GmacThisUVE.flagRBNA=0;
#endif

    return 0;
}

#endif //#ifdef GMAC_RX_SOFTWARE_DESCRIPTOR

#ifdef GMAC_INT_JULIAN_D
//-------------------------------------------------------------------------------------------------
//MAC interrupt handler
//(Interrupt delay enable)
//-------------------------------------------------------------------------------------------------

void MDev_GMAC_bottom_task(struct work_struct *work)
{
    struct GMAC_private *LocPtr = container_of(work, struct GMAC_private, task);
    struct net_device *dev = LocPtr->dev;

    MDev_GMAC_rx(dev);
}

irqreturn_t MDev_GMAC_interrupt(int irq,void *dev_id)
{
    struct net_device *dev = (struct net_device *) dev_id;
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    u32 intstatus=0;
    u32 xReceiveFlag=0;
    unsigned long flags;
#ifndef GMAC_RX_SOFTWARE_DESCRIPTOR
    u32 wp = 0;
#endif

    spin_lock_irqsave(LocPtr->lock, flags);
    //MAC Interrupt Status register indicates what interrupts are pending.
    //It is automatically cleared once read.
    GmacxoffsetValue = MHal_GMAC_Read_JULIAN_0108() & 0x0000FFFF;
    GmacxReceiveNum += GmacxoffsetValue&0xFF;

#ifndef GMAC_RX_SOFTWARE_DESCRIPTOR
    wp = MHal_GMAC_Read_JULIAN_0100() & 0x00100000;
    if(wp)
    {
        GMAC_DBG("GMAC HW write invalid address");
    }
#endif

    if(GmacxoffsetValue&0x8000)
    {
        xReceiveFlag = 1;
    }
    GmacThisUVE.flagRBNA = 0;

    gmac_oldTime = getCurMs();
    while((xReceiveFlag == 1) || (GmacxReceiveNum != 0) ||  (intstatus = (MHal_GMAC_Read_ISR() & ~MHal_GMAC_Read_IMR() & GMAC_INT_MASK )) )
    {
        if (intstatus & GMAC_INT_RBNA)
        {
            LocPtr->stats.rx_dropped ++;
            GmacThisUVE.flagRBNA = 1;
            xReceiveFlag = 1;
		    //write 1 clear
            MHal_GMAC_Write_RSR(GMAC_BNA);
        }

        // Transmit complete //
        if (intstatus & GMAC_INT_TCOM)
        {
            // The TCOM bit is set even if the transmission failed. //
            if (intstatus & (GMAC_INT_TUND | GMAC_INT_RTRY))
            {
                LocPtr->stats.tx_errors += 1;

                if(intstatus & GMAC_INT_TUND)
                {
				    //write 1 clear
                    MHal_GMAC_Write_TSR(GMAC_UND);
                    //GMAC_DBG ("==> %s: Transmit TUND error\n", dev->name);
                }

            }
            else
            {
                LocPtr->retx_count = 0;
            }

        #if GMAC_TX_THROUGHPUT_TEST
            MDev_GMAC_tx(pseudo_packet, gmac_dev);
        #endif
            if (((LocPtr->ep_flag&GMAC_EP_FLAG_SUSPENDING)==0) && netif_queue_stopped (dev))
                netif_wake_queue(dev);
        }

        if(intstatus&GMAC_INT_DONE)
        {
            GmacThisUVE.flagISR_INT_DONE = 0x01;
        }

        //Overrun //
        if(intstatus & GMAC_INT_ROVR)
        {
            LocPtr->stats.rx_dropped++;
            gmac_contiROVR++;
            //write 1 clear
            MHal_GMAC_Write_RSR(GMAC_RSROVR);
            //GMAC_DBG ("==> %s: ROVR error %u times!\n", dev->name, gmac_contiROVR);
            if (gmac_contiROVR < 3)
            {
                MDev_GMAC_Send_PausePkt(dev);
            }
            else
            {
                MDev_GMAC_SwReset(dev);
                GmacxReceiveNum = 0;
            }
        }
        else
        {
            gmac_contiROVR = 0;
        }

        if(GmacxReceiveNum != 0)
        {
            xReceiveFlag = 1;
        }
        // Receive complete //
        if(xReceiveFlag == 1)
        {
            xReceiveFlag = 0;
        #ifdef ISR_BOTTOM_HALF
            schedule_work(&LocPtr->task);
        #else
            MDev_GMAC_rx(dev);
        #endif
        }
    }
    spin_unlock_irqrestore(LocPtr->lock, flags);
    return IRQ_HANDLED;
}

#else //#ifdef GMAC_INT_JULIAN_D

//-------------------------------------------------------------------------------------------------
//MAC interrupt handler with interrupt delay disable
//(Interrupt delay Disable)
//-------------------------------------------------------------------------------------------------
irqreturn_t MDev_GMAC_interrupt(int irq,void *dev_id)
{
    struct net_device *dev = (struct net_device *) dev_id;
    struct GMAC_private *LocPtr = (struct GMAC_private*) netdev_priv(dev);
    unsigned long flags;
    unsigned long intstatus;
    spin_lock_irqsave(LocPtr->lock, flags);

    while(intstatus = (MHal_GMAC_Read_ISR() & ~MHal_GMAC_Read_IMR() & MASK))
    {
        //RX buffer not available//
        if (intstatus & GMAC_INT_RBNA)
        {
            LocPtr->stats.rx_dropped ++;
        }
        // Receive complete //
        if (intstatus & GMAC_INT_RCOM)
        {
            if(MDev_GMAC_rx (dev)) goto quit_int;
        }
        // Transmit complete //
        if (intstatus & GMAC_INT_TCOM)
        {
            struct gmac_tx_ring* tx_fifo_data = NULL;
            u32 remove_cnt = 1;
            // The TCOM bit is set even if the transmission failed. //
            if (intstatus & (GMAC_INT_TUND | GMAC_INT_RTRY))
            {
                LocPtr->stats.tx_errors += 1;
                if(intstatus &GMAC_INT_TUND)
                {
                    GMAC_DBG ("%s: Transmit TUND error\n", dev->name);
                }
                if(intstatus &GMAC_INT_RTRY)
                {
                    GMAC_DBG ("%s: Transmit RTRY error\n", dev->name);
                }
            }

            if (intstatus & GMAC_INT_TBRE)
                 remove_cnt = 2;
        #if GMAC_TX_THROUGHPUT_TEST
            MDev_GMAC_tx(pseudo_packet, gmac_dev);
        #else
        #ifdef GMAC_TX_QUEUE_4
           remove_cnt = (intstatus >> GMAC_TX_COUNT);
        #endif
           while (remove_cnt > 0)
           {
                tx_fifo_data = gmac_tx_ring_get(LocPtr, 1);
	            if (tx_fifo_data == NULL)
                {
                    break;
                }
                else
                {
                 #ifdef GMAC_TX_QUEUE_4
	                 dma_unmap_single(NULL, tx_fifo_data->skb_physaddr, tx_fifo_data->skb->len, DMA_FROM_DEVICE);
                 #endif
		             if (tx_fifo_data->skb)
	                 {
                         dev_kfree_skb_irq(tx_fifo_data->skb);
      	                 tx_fifo_data->skb = NULL;
		             }
		             else
		             {
                         GMAC_DBG ("skb is null!\n");
		             }
	            }
	            remove_cnt--;
            }
        #endif
            if (netif_queue_stopped(dev))
                netif_wake_queue(dev);
        }

        if(intstatus&GMAC_INT_DONE)
        {
            ThisUVE.flagISR_INT_DONE = 0x01;
        }
        //Overrun //
        if(intstatus & GMAC_INT_ROVR)
        {
            MDev_GMAC_rx(dev);
            LocPtr->stats.rx_dropped++;
        }
    }
quit_int:
    spin_unlock_irqrestore(LocPtr->lock, flags);
    return IRQ_HANDLED;
}
#endif //#ifdef GMAC_INT_JULIAN_D

//-------------------------------------------------------------------------------------------------
// GMAC Hardware register set
//-------------------------------------------------------------------------------------------------
void MDev_GMAC_HW_init(void)
{
    u32 word_ETH_CTL = 0x00000000;
    u32 word_ETH_CFG = 0x00000800;
    u32 uJulian104Value = 0;
    u32 uNegPhyVal = 0;
#ifdef GMAC_SOFTWARE_DESCRIPTOR
    u32 idxRBQP = 0;
    u32 RBQP_offset = 0;
#endif
    // (20071026_CHARLES) Disable TX, RX and MDIO:   (If RX still enabled, the RX buffer will be overwrited)
    MHal_GMAC_Write_CTL(word_ETH_CTL);
    // Init RX --------------------------------------------------------------
    memset((u8*)GMAC_RAM_VA_PA_OFFSET + GMAC_RX_BUFFER_BASE, 0x00, GMAC_RX_BUFFER_SIZE);

    MHal_GMAC_Write_BUFF((GMAC_RX_BUFFER_BASE | GMAC_RX_BUFFER_SEL) - GMAC_MIU0_BUS_BASE);
    MHal_GMAC_Write_RDPTR(0x00000000);
    MHal_GMAC_Write_WRPTR(0x00000000);

    // Initialize "Receive Buffer Queue Pointer"
    MHal_GMAC_Write_RBQP(GMAC_RBQP_BASE -GMAC_MIU0_BUS_BASE);

    // Initialize Receive Buffer Descriptors
    memset((u8*)GMAC_RAM_VA_PA_OFFSET + GMAC_RBQP_BASE, 0x00, GMAC_RBQP_SIZE);        // Clear for max(8*1024)bytes (max:1024 descriptors)
    MHal_GMAC_WritRam32(GMAC_RAM_VA_PA_OFFSET, (GMAC_RBQP_BASE + GMAC_RBQP_SIZE - 0x10), 0x00000002);             // (n-1) : Wrap = 1
#ifdef GMAC_INT_JULIAN_D
    //Reg_rx_frame_cyc[15:8] -0xFF range 1~255
    //Reg_rx_frame_num[7:0]  -0x05 receive frames per INT.
    //0x80 Enable interrupt delay mode.
    //register 0x104 receive counter need to modify smaller for ping
    //Modify bigger(need small than 8) for throughput
    uJulian104Value = GMAC_JULIAN_104_VAL;//0xFF050080;
    MHal_GMAC_Write_JULIAN_0104(uJulian104Value);
#else
    // Enable Interrupts ----------------------------------------------------
    uJulian104Value = 0x00000000;
    MHal_GMAC_Write_JULIAN_0104(uJulian104Value);
#endif
    // Set MAC address ------------------------------------------------------
    MHal_GMAC_Write_SA1_MAC_Address(GmacThisBCE.sa1[0], GmacThisBCE.sa1[1], GmacThisBCE.sa1[2], GmacThisBCE.sa1[3], GmacThisBCE.sa1[4], GmacThisBCE.sa1[5]);
    MHal_GMAC_Write_SA2_MAC_Address(GmacThisBCE.sa2[0], GmacThisBCE.sa2[1], GmacThisBCE.sa2[2], GmacThisBCE.sa2[3], GmacThisBCE.sa2[4], GmacThisBCE.sa2[5]);
    MHal_GMAC_Write_SA3_MAC_Address(GmacThisBCE.sa3[0], GmacThisBCE.sa3[1], GmacThisBCE.sa3[2], GmacThisBCE.sa3[3], GmacThisBCE.sa3[4], GmacThisBCE.sa3[5]);
    MHal_GMAC_Write_SA4_MAC_Address(GmacThisBCE.sa4[0], GmacThisBCE.sa4[1], GmacThisBCE.sa4[2], GmacThisBCE.sa4[3], GmacThisBCE.sa4[4], GmacThisBCE.sa4[5]);

#ifdef GMAC_SOFTWARE_DESCRIPTOR
    #ifdef GMAC_RX_CHECKSUM
    uJulian104Value=uJulian104Value | (GMAC_RX_CHECKSUM_ENABLE | GMAC_SOFTWARE_DESCRIPTOR_ENABLE);
    #else
    uJulian104Value=uJulian104Value | GMAC_SOFTWARE_DESCRIPTOR_ENABLE;
    #endif

    MHal_GMAC_Write_JULIAN_0104(uJulian104Value);

    for(idxRBQP = 0; idxRBQP < GMAC_RBQP_LENG; idxRBQP++)
    {
    #ifdef GMAC_RX_SOFTWARE_DESCRIPTOR
        rx_skb[idxRBQP] = alloc_skb(GMAC_SOFTWARE_DESC_LEN, GFP_ATOMIC);

        rx_abso_addr[idxRBQP] = (u32)rx_skb[idxRBQP]->data;
        RBQP_offset = idxRBQP * 16;
        if(idxRBQP < (RBQP_LENG - 1))
        {
            MHal_GMAC_WritRam32(GMAC_RAM_VA_PA_OFFSET, GMAC_RBQP_BASE + RBQP_offset, rx_abso_addr[idxRBQP]);
        }
        else
        {
            MHal_GMAC_WritRam32(GMAC_RAM_VA_PA_OFFSET, GMAC_RBQP_BASE + RBQP_offset, (rx_abso_addr[idxRBQP] + GMAC_DESC_WRAP));
        }
    #else
        RBQP_offset = idxRBQP * 16;

        if(idxRBQP < (GMAC_RBQP_LENG - 1))
        {
            MHal_GMAC_WritRam32(GMAC_RAM_VA_PA_OFFSET, GMAC_RBQP_BASE + RBQP_offset, (GMAC_RX_BUFFER_BASE - GMAC_MIU0_BUS_BASE + GMAC_SOFTWARE_DESC_LEN * idxRBQP));
        }
        else
        {
            MHal_GMAC_WritRam32(GMAC_RAM_VA_PA_OFFSET, GMAC_RBQP_BASE + RBQP_offset, (GMAC_RX_BUFFER_BASE - GMAC_MIU0_BUS_BASE + GMAC_SOFTWARE_DESC_LEN * idxRBQP + GMAC_DESC_WRAP));
        }
    #endif
    }
#ifdef GMAC_RX_SOFTWARE_DESCRIPTOR
	rx_skb_dummy = 	alloc_skb(GMAC_SOFTWARE_DESC_LEN, GFP_ATOMIC);
	if(rx_skb_dummy == NULL)
    {
        GMAC_DBG(KERN_INFO "allocate skb dummy failed\n");
    }
	else
    {
	    rx_abso_addr_dummy = (u32)(rx_skb_dummy->data);
    }
#endif

#endif //#ifdef GMAC_SOFTWARE_DESCRIPTOR

    if (!GmacThisUVE.initedGMAC)
    {
        MHal_GMAC_write_phy(gmac_phyaddr, MII_BMCR, 0x9000);
        MHal_GMAC_write_phy(gmac_phyaddr, MII_BMCR, 0x1000);
        // IMPORTANT: Run NegotiationPHY() before writing REG_ETH_CFG.
        uNegPhyVal = MHal_GMAC_NegotiationPHY();
        if(uNegPhyVal == 0x01)
        {
            GmacThisUVE.flagMacTxPermit = 0x01;
            GmacThisBCE.duplex = 1;

        }
        else if(uNegPhyVal == 0x02)
        {
            GmacThisUVE.flagMacTxPermit = 0x01;
            GmacThisBCE.duplex = 2;
        }

        // ETH_CFG Register -----------------------------------------------------
        word_ETH_CFG = 0x00000800;        // Init: CLK = 0x2
        // (20070808) IMPORTANT: REG_ETH_CFG:bit1(FD), 1:TX will halt running RX frame, 0:TX will not halt running RX frame.
        // If always set FD=0, no CRC error will occur. But throughput maybe need re-evaluate.
        // IMPORTANT: (20070809) NO_MANUAL_SET_DUPLEX : The real duplex is returned by "negotiation"
        if(GmacThisBCE.speed     == GMAC_SPEED_100) word_ETH_CFG |= 0x00000001;
        if(GmacThisBCE.duplex    == 2)              word_ETH_CFG |= 0x00000002;
        if(GmacThisBCE.cam       == 1)              word_ETH_CFG |= 0x00000200;
        if(GmacThisBCE.rcv_bcast == 0)              word_ETH_CFG |= 0x00000020;
        if(GmacThisBCE.rlf       == 1)              word_ETH_CFG |= 0x00000100;

        MHal_GMAC_Write_CFG(word_ETH_CFG);
        // ETH_CTL Register -----------------------------------------------------
        word_ETH_CTL = 0x0000000C;                          // Enable transmit and receive : TE + RE = 0x0C (Disable MDIO)
        if(GmacThisBCE.wes == 1) word_ETH_CTL |= 0x00000080;
        MHal_GMAC_Write_CTL(word_ETH_CTL);
#if 0 //FIXME latter
        MHal_GMAC_Write_JULIAN_0100(GMAC_JULIAN_100_VAL);
#else
        MHal_GMAC_Write_JULIAN_0100(0x0000F001);
#endif

    #ifdef CONFIG_GMAC_ETHERNET_ALBANY
        MHal_GMAC_Write_JULIAN_0100(0x0000F001);
    #endif

        GmacThisUVE.flagPowerOn = 1;
        GmacThisUVE.initedGMAC  = 1;
    }

    MHal_GMAC_HW_init();

}


//-------------------------------------------------------------------------------------------------
// GMAC init Variable
//-------------------------------------------------------------------------------------------------
static u32 MDev_GMAC_VarInit(void)
{
    u32 alloRAM_PA_BASE;
    u32 alloRAM_SIZE;
    char addr[6];
    u32 HiAddr, LoAddr;
    u32 *alloRAM_VA_BASE;

    get_boot_mem_info(EMAC_MEM, &alloRAM_PA_BASE, &alloRAM_SIZE);
    alloRAM_VA_BASE = (u32 *)ioremap(alloRAM_PA_BASE, alloRAM_SIZE); //map buncing buffer from PA to VA

    GMAC_DBG("alloRAM_VA_BASE = 0x%X alloRAM_PA_BASE= 0x%X  alloRAM_SIZE= 0x%X\n", (u32)alloRAM_VA_BASE, alloRAM_PA_BASE, alloRAM_SIZE);
#ifndef GMAC_RX_SOFTWARE_DESCRIPTOR
    //Add Write Protect
    MHal_GMAC_Write_Protect(alloRAM_PA_BASE & 0x0fffffff, alloRAM_SIZE);
#endif
    memset((u32 *)alloRAM_VA_BASE,0x00,alloRAM_SIZE);
    
    GMAC_RAM_VA_BASE       = ((u32)alloRAM_VA_BASE + sizeof(struct GMAC_private) + 0x3FFF) & ~0x3FFF;   // IMPORTANT: Let lowest 14 bits as zero.
    GMAC_RAM_PA_BASE       = ((u32)alloRAM_PA_BASE + sizeof(struct GMAC_private) + 0x3FFF) & ~0x3FFF;   // IMPORTANT: Let lowest 14 bits as zero.
    GMAC_RX_BUFFER_BASE    = GMAC_RAM_PA_BASE + GMAC_RBQP_SIZE;
    GMAC_RBQP_BASE         = GMAC_RAM_PA_BASE;
    GMAC_TX_BUFFER_BASE    = GMAC_RAM_PA_BASE + (GMAC_RX_BUFFER_SIZE + GMAC_RBQP_SIZE);
    GMAC_RAM_VA_PA_OFFSET  = GMAC_RAM_VA_BASE - GMAC_RAM_PA_BASE;  // IMPORTANT_TRICK_20070512
    GMAC_TX_SKB_BASE       = GMAC_TX_BUFFER_BASE + GMAC_MAX_RX_DESCR * sizeof(struct gmac_rbf_t);
    
    memset(&GmacThisBCE,0x00,sizeof(BasicConfigGMAC));
    memset(&GmacThisUVE,0x00,sizeof(UtilityVarsGMAC));

    GmacThisBCE.wes       	= 0;             		// 0:Disable, 1:Enable (WR_ENABLE_STATISTICS_REGS)
    GmacThisBCE.duplex 		= 2;                    // 1:Half-duplex, 2:Full-duplex
    GmacThisBCE.cam			= 0;                 	// 0:No CAM, 1:Yes
    GmacThisBCE.rcv_bcast	= 0;                  	// 0:No, 1:Yes
    GmacThisBCE.rlf  		= 0;                 	// 0:No, 1:Yes receive long frame(1522)
    GmacThisBCE.rcv_bcast   = 1;
    GmacThisBCE.speed       = GMAC_SPEED_100;

    // Check if bootloader set address in Specific-Address 1 //
    HiAddr = MHal_GMAC_get_SA1H_addr();
    LoAddr = MHal_GMAC_get_SA1L_addr();

    addr[0] = (LoAddr & 0xff);
    addr[1] = (LoAddr & 0xff00) >> 8;
    addr[2] = (LoAddr & 0xff0000) >> 16;
    addr[3] = (LoAddr & 0xff000000) >> 24;
    addr[4] = (HiAddr & 0xff);
    addr[5] = (HiAddr & 0xff00) >> 8;

    if (is_valid_ether_addr (addr))
    {
        memcpy (GmacThisBCE.sa1, &addr, 6);
    }
    else
    {
        // Check if bootloader set address in Specific-Address 2 //
        HiAddr = MHal_GMAC_get_SA2H_addr();
        LoAddr = MHal_GMAC_get_SA2L_addr();
        addr[0] = (LoAddr & 0xff);
        addr[1] = (LoAddr & 0xff00) >> 8;
        addr[2] = (LoAddr & 0xff0000) >> 16;
        addr[3] = (LoAddr & 0xff000000) >> 24;
        addr[4] = (HiAddr & 0xff);
        addr[5] = (HiAddr & 0xff00) >> 8;

        if (is_valid_ether_addr (addr))
        {
            memcpy (GmacThisBCE.sa1, &addr, 6);
        }
        else
        {
            GmacThisBCE.sa1[0]      = GMAC_MY_MAC[0];
            GmacThisBCE.sa1[1]      = GMAC_MY_MAC[1];
            GmacThisBCE.sa1[2]      = GMAC_MY_MAC[2];
            GmacThisBCE.sa1[3]      = GMAC_MY_MAC[3];
            GmacThisBCE.sa1[4]      = GMAC_MY_MAC[4];
            GmacThisBCE.sa1[5]      = GMAC_MY_MAC[5];
        }
    }
    GmacThisBCE.connected = 0;
    return (u32)alloRAM_VA_BASE;
}

//-------------------------------------------------------------------------------------------------
// Initialize the ethernet interface
// @return TRUE : Yes
// @return FALSE : FALSE
//-------------------------------------------------------------------------------------------------

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
static const struct net_device_ops mstar_lan_netdev_ops = {
        .ndo_open               = MDev_GMAC_open,
        .ndo_stop               = MDev_GMAC_close,
        .ndo_start_xmit =  MDev_GMAC_tx,
        .ndo_set_mac_address    = MDev_GMAC_set_mac_address,
        .ndo_set_multicast_list = MDev_GMAC_set_rx_mode,
        .ndo_do_ioctl           = MDev_GMAC_ioctl,
        .ndo_get_stats          = MDev_GMAC_stats,
};
#endif
static int MDev_GMAC_setup (struct net_device *dev, unsigned long phy_type)
{
    struct GMAC_private *LocPtr;

    static int already_initialized = 0;
    dma_addr_t dmaaddr;
    u32 val;
    u32 RetAddr;
#ifdef CONFIG_MSTAR_GMAC_HW_TX_CHECKSUM
    u32 retval;
#endif
    if (already_initialized)
        return FALSE;

    LocPtr = (struct GMAC_private *) netdev_priv(dev);

    /*Init the bottom half ISR task*/
    INIT_WORK(&LocPtr->task, MDev_GMAC_bottom_task);

    LocPtr->dev = dev;
    RetAddr = MDev_GMAC_VarInit();

    if(!RetAddr)
    {
        GMAC_DBG("Var init fail!!\n");
        return FALSE;
    }

    if (LocPtr == NULL)
    {
        free_irq (dev->irq, dev);
        GMAC_DBG("LocPtr fail\n");
        return -ENOMEM;
    }

    dev->base_addr = (long) GMAC_REG_ADDR_BASE;
    MDev_GMAC_HW_init();
    dev->irq =  E_IRQEXPL_GMAC;


    // Allocate memory for DMA Receive descriptors //
    LocPtr->dlist_phys = LocPtr->dlist = (struct gmac_recv_desc_bufs *) (GMAC_RBQP_BASE + GMAC_RAM_VA_PA_OFFSET);

    if (LocPtr->dlist == NULL)
    {
        dma_free_noncoherent((void *)LocPtr, GMAC_ABSO_MEM_SIZE,&dmaaddr,0);//kfree (dev->priv);
        free_irq (dev->irq, dev);
        return -ENOMEM;
    }

    LocPtr->lock = &gmac_lock;
    spin_lock_init (LocPtr->lock);
    ether_setup (dev);
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,28)
    dev->open = MDev_GMAC_open;
    dev->stop = MDev_GMAC_close;
    dev->hard_start_xmit = MDev_GMAC_tx;
    dev->get_stats = MDev_GMAC_stats;
    dev->set_multicast_list = MDev_GMAC_set_rx_mode;
    dev->do_ioctl = MDev_GMAC_ioctl;
    dev->set_mac_address = MDev_GMAC_set_mac_address;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    dev->netdev_ops = &mstar_lan_netdev_ops;
#endif
    dev->tx_queue_len = GMAC_MAX_TX_QUEUE;

    MDev_GMAC_get_mac_address (dev);    // Get ethernet address and store it in dev->dev_addr //
    MDev_GMAC_update_mac_address (dev); // Program ethernet address into MAC //
    spin_lock_irq (LocPtr->lock);
    MHal_GMAC_enable_mdi ();
    MHal_GMAC_read_phy (gmac_phyaddr, GMAC_MII_USCR_REG, &val);
    if ((val & (1 << 10)) == 0)   // DSCR bit 10 is 0 -- fiber mode //
        LocPtr->phy_media = PORT_FIBRE;

    spin_unlock_irq (LocPtr->lock);

    //Support for ethtool //
    LocPtr->mii.dev = dev;
    LocPtr->mii.mdio_read = MDev_GMAC_mdio_read;
    LocPtr->mii.mdio_write = MDev_GMAC_mdio_write;
    already_initialized = 1;
#ifdef CONFIG_MSTAR_GMAC_HW_TX_CHECKSUM
    retval = MHal_GMAC_Read_JULIAN_0104() | GMAC_TX_CHECKSUM_ENABLE;
    MHal_GMAC_Write_JULIAN_0104(retval);
    dev->features |= NETIF_F_IP_CSUM;
#endif

    //Install the interrupt handler //
    //Notes: Modify linux/kernel/irq/manage.c  /* interrupt.h */
    if (request_irq(dev->irq, MDev_GMAC_interrupt, SA_INTERRUPT, dev->name, dev))
        return -EBUSY;

    //Determine current link speed //
    spin_lock_irq (LocPtr->lock);
    (void) MDev_GMAC_update_linkspeed (dev);
    spin_unlock_irq (LocPtr->lock);

    return 0;
}

//-------------------------------------------------------------------------------------------------
// Restar the ethernet interface
// @return TRUE : Yes
// @return FALSE : FALSE
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_SwReset(struct net_device *dev)
{
    struct GMAC_private *LocPtr = (struct GMAC_private *) netdev_priv(dev);
    u32 oldCFG, oldCTL;
    u32 retval;

    MDev_GMAC_get_mac_address (dev);
    oldCFG = MHal_GMAC_Read_CFG();
    oldCTL = MHal_GMAC_Read_CTL() & ~(GMAC_TE | GMAC_RE);

    //free tx skb
    if (LocPtr->retx_count)
    {
        if (LocPtr->skb)
        {
            dev_kfree_skb_irq(LocPtr->skb );
            LocPtr->skb = NULL;
        }
        if (netif_queue_stopped (dev))
            netif_wake_queue (dev);
    }

    netif_stop_queue (dev);

    retval = MHal_GMAC_Read_JULIAN_0100();
    MHal_GMAC_Write_JULIAN_0100(retval & 0x00000FFF);
    MHal_GMAC_Write_JULIAN_0100(retval);

    MDev_GMAC_HW_init();
    MHal_GMAC_Write_CFG(oldCFG);
    MHal_GMAC_Write_CTL(oldCTL);
    MHal_GMAC_enable_mdi ();
    MDev_GMAC_update_mac_address (dev); // Program ethernet address into MAC //
    MDev_GMAC_update_linkspeed (dev);
    MHal_GMAC_Write_IER(GMAC_IER_FOR_INT_JULIAN_D);
    MDev_GMAC_start (dev);
    MDev_GMAC_set_rx_mode(dev);
    netif_start_queue (dev);
    gmac_contiROVR = 0;
    LocPtr->retx_count = 0;
#ifdef CONFIG_MSTAR_GMAC_HW_TX_CHECKSUM
    retval = MHal_GMAC_Read_JULIAN_0104() | GMAC_TX_CHECKSUM_ENABLE;
    MHal_GMAC_Write_JULIAN_0104(retval);
    dev->features |= NETIF_F_IP_CSUM;
#endif
    GMAC_DBG("=> Take %lu ms to reset GMAC!\n", (getCurMs() - gmac_oldTime));
    return 0;
}

//-------------------------------------------------------------------------------------------------
// Detect MAC and PHY and perform initialization
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_probe (struct net_device *dev)
{
    int detected = -1;

    /* Read the PHY ID registers - try all addresses */
    detected = MDev_GMAC_setup(dev, GMAC_MII_URANUS_ID);
    return detected;
}

//-------------------------------------------------------------------------------------------------
// GMAC Timer set for Receive function
//-------------------------------------------------------------------------------------------------
static void MDev_GMAC_timer_callback(unsigned long value)
{
    int ret = 0;
    struct GMAC_private *LocPtr = (struct GMAC_private *) netdev_priv(gmac_dev);
    static u32 bmsr, time_count = 0;
#ifndef GMAC_INT_JULIAN_D
    if (GMAC_RX_TMR == value)
    {
        MHal_GMAC_timer_callback(value);
        return;
    }
#endif

    spin_lock_irq (LocPtr->lock);
    ret = MDev_GMAC_update_linkspeed(gmac_dev);
    spin_unlock_irq (LocPtr->lock);
    if (0 == ret)
    {
        if (!GmacThisBCE.connected)
        {
            GmacThisBCE.connected = 1;
            netif_carrier_on(gmac_dev);
        }

        // Link status is latched, so read twice to get current value //
        MHal_GMAC_read_phy (gmac_phyaddr, MII_BMSR, &bmsr);
        MHal_GMAC_read_phy (gmac_phyaddr, MII_BMSR, &bmsr);
        time_count = 0;
        spin_lock_irq (LocPtr->lock);
        gmac_phy_status_register = bmsr;
        spin_unlock_irq (LocPtr->lock);
        // Normally, time out sets 1 Sec.
        GMAC_Link_timer.expires = jiffies + GMAC_CHECK_LINK_TIME;
    }
    else if (-1 == ret)    //no link
    {
        if(GmacThisBCE.connected) {
            GmacThisBCE.connected = 0;
        }
        // If disconnected is over 3 Sec, the real value of PHY's status register will report to application.
        if(time_count > 30) {
            // Link status is latched, so read twice to get current value //
            MHal_GMAC_read_phy (gmac_phyaddr, MII_BMSR, &bmsr);
            MHal_GMAC_read_phy (gmac_phyaddr, MII_BMSR, &bmsr);
            spin_lock_irq (LocPtr->lock);
            gmac_phy_status_register = bmsr;
            spin_unlock_irq (LocPtr->lock);
            // Report to kernel.
            netif_carrier_off(gmac_dev);
            // Normally, time out is set 1 Sec.
            GMAC_Link_timer.expires = jiffies + GMAC_CHECK_LINK_TIME;
        }
        else if(time_count <= 30){
            time_count++;
            // Time out is set 100ms. Quickly checks next phy status.
            GMAC_Link_timer.expires = jiffies + (GMAC_CHECK_LINK_TIME / 10);
        }
    }

    add_timer(&GMAC_Link_timer);
}

//-------------------------------------------------------------------------------------------------
// GMAC MACADDR Setup
//-------------------------------------------------------------------------------------------------

#define MACADDR_FORMAT "XX:XX:XX:XX:XX:XX"

static int __init macaddr_auto_config_setup(char *addrs)
{
    if (strlen(addrs) == strlen(MACADDR_FORMAT)
        && ':' == addrs[2]
        && ':' == addrs[5]
        && ':' == addrs[8]
        && ':' == addrs[11]
        && ':' == addrs[14]
       )
    {
        addrs[2]  = '\0';
        addrs[5]  = '\0';
        addrs[8]  = '\0';
        addrs[11] = '\0';
        addrs[14] = '\0';

        GMAC_MY_MAC[0] = (u8)simple_strtoul(&(addrs[0]),  NULL, 16);
        GMAC_MY_MAC[1] = (u8)simple_strtoul(&(addrs[3]),  NULL, 16);
        GMAC_MY_MAC[2] = (u8)simple_strtoul(&(addrs[6]),  NULL, 16);
        GMAC_MY_MAC[3] = (u8)simple_strtoul(&(addrs[9]),  NULL, 16);
        GMAC_MY_MAC[4] = (u8)simple_strtoul(&(addrs[12]), NULL, 16);
        GMAC_MY_MAC[5] = (u8)simple_strtoul(&(addrs[15]), NULL, 16);

        /* set back to ':' or the environment variable would be destoried */ // REVIEW: this coding style is dangerous
        addrs[2]  = ':';
        addrs[5]  = ':';
        addrs[8]  = ':';
        addrs[11] = ':';
        addrs[14] = ':';
    }

    return 1;
}

__setup("macaddr=", macaddr_auto_config_setup);

//-------------------------------------------------------------------------------------------------
// GMAC init module
//-------------------------------------------------------------------------------------------------
static int MDev_GMAC_ScanPhyAddr(void)
{
    unsigned char addr = 0;
    u32 value = 0;

#ifdef CONFIG_GMAC_ETHERNET_ALBANY
    MHal_GMAC_Write_JULIAN_0100(0x0000F001);
#else
    MHal_GMAC_Write_JULIAN_0100(0x0000F007);
#endif

    MHal_GMAC_enable_mdi();
    do
    {
        MHal_GMAC_read_phy(addr, MII_BMSR, &value);
        if (0 != value && 0x0000FFFF != value)
        {
            GMAC_DBG("[ PHY Addr ] ==> :%u\n", addr);
            break;
        }
    }while(++addr && addr < 32);
    MHal_GMAC_disable_mdi();
    gmac_phyaddr = addr;

	if (gmac_phyaddr >= 32)
	{
		GMAC_DBG("Wrong PHY Addr and reset to 0\n");
		gmac_phyaddr = 0;
		return -1;
	}
	return 0;
}

static void Rtl_Patch(void)
{
    u32 val;

    MHal_GMAC_read_phy(gmac_phyaddr, 25, &val);
    MHal_GMAC_write_phy(gmac_phyaddr, 25, 0x400);
    MHal_GMAC_read_phy(gmac_phyaddr, 25, &val);
}

static void MDev_GMAC_Patch_PHY(void)
{
    u32 val;

    MHal_GMAC_read_phy(gmac_phyaddr, 2, &val);
    if (GMAC_RTL_8210 == val)
        Rtl_Patch();
}

static int MDev_GMAC_init(void)
{
    struct GMAC_private *LocPtr;

    if(gmac_dev)
        return -1;

    gmac_dev = alloc_etherdev(sizeof(*LocPtr));
    LocPtr = netdev_priv(gmac_dev);
    if (!gmac_dev)
    {
        GMAC_DBG( KERN_ERR "No GMAC dev mem!\n" );
        return -ENOMEM;
    }

#if GMAC_TX_THROUGHPUT_TEST
    printk("==========TX_THROUGHPUT_TEST===============");
    pseudo_packet = alloc_skb(SOFTWARE_DESC_LEN, GFP_ATOMIC);
    memcpy(pseudo_packet->data, (void *)packet_content, sizeof(packet_content));
    pseudo_packet->len = sizeof(packet_content);
#endif

#if GMAC_RX_THROUGHPUT_TEST
    printk("==========RX_THROUGHPUT_TEST===============");
    init_timer(&RX_timer);

    RX_timer.data = GMAC_RX_TMR;
    RX_timer.function = RX_timer_callback;
    RX_timer.expires = jiffies + 20*GMAC_CHECK_LINK_TIME;
    add_timer(&RX_timer);
#endif

    MHal_GMAC_Power_On_Clk();

    init_timer(&GMAC_timer);
    init_timer(&GMAC_Link_timer);

    GMAC_timer.data = GMAC_RX_TMR;
    GMAC_timer.function = MDev_GMAC_timer_callback;
    GMAC_timer.expires = jiffies;


    MHal_GMAC_Write_JULIAN_0100(GMAC_JULIAN_100_VAL);

    if (0 > MDev_GMAC_ScanPhyAddr())
        //goto end;

    MDev_GMAC_Patch_PHY();
    if (!MDev_GMAC_probe (gmac_dev))
        return register_netdev (gmac_dev);

end:
    free_netdev(gmac_dev);
    gmac_dev = 0;
    gmac_initstate = GMAC_ETHERNET_TEST_INIT_FAIL;
    GMAC_DBG( KERN_ERR "Init GMAC error!\n" );
    return -1;
}
//-------------------------------------------------------------------------------------------------
// GMAC exit module
//-------------------------------------------------------------------------------------------------
static void MDev_GMAC_exit(void)
{
    if (gmac_dev)
    {
    #ifndef GMAC_INT_JULIAN_D
        del_timer(&GMAC_timer);
    #endif
        unregister_netdev(gmac_dev);
        free_netdev(gmac_dev);
    }
}

static int mstar_gmac_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    struct net_device *netdev=(struct net_device*)dev->dev.platform_data;
    struct GMAC_private *LocPtr;
    u32 uRegVal;
    printk(KERN_INFO "mstar_gmac_drv_suspend\n");
    if(!netdev)
    {
        return -1;
    }

    LocPtr = (struct GMAC_private*) netdev_priv(netdev);
    LocPtr->ep_flag |= GMAC_EP_FLAG_SUSPENDING;
    netif_stop_queue (netdev);

    disable_irq(netdev->irq);
    del_timer(&GMAC_Link_timer);

    MHal_GMAC_Power_On_Clk();

    //Disable Receiver and Transmitter //
    uRegVal = MHal_GMAC_Read_CTL();
    uRegVal &= ~(GMAC_TE | GMAC_RE);
    MHal_GMAC_Write_CTL(uRegVal);

    // Disable PHY interrupt //
    MHal_GMAC_disable_phyirq ();
#ifndef GMAC_INT_JULIAN_D
    //Disable MAC interrupts //
    uRegVal = GMAC_INT_RCOM | GMAC_IER_FOR_INT_JULIAN_D;
    MHal_GMAC_Write_IDR(uRegVal);
#else
    MHal_GMAC_Write_IDR(GMAC_IER_FOR_INT_JULIAN_D);
#endif
    MHal_GMAC_Power_Off_Clk();

    return 0;
}
static int mstar_gmac_drv_resume(struct platform_device *dev)
{
    struct net_device *netdev=(struct net_device*)dev->dev.platform_data;
    struct GMAC_private *LocPtr;
    u32 alloRAM_PA_BASE;
    u32 alloRAM_SIZE;
    u32 retval;
    printk(KERN_INFO "mstar_gmac_drv_resume\n");
    if(!netdev)
    {
        return -1;
    }
    LocPtr = (struct GMAC_private*) netdev_priv(netdev);;
    LocPtr->ep_flag &= ~GMAC_EP_FLAG_SUSPENDING;

    MHal_GMAC_Power_On_Clk();

    MHal_GMAC_Write_JULIAN_0100(GMAC_JULIAN_100_VAL);

    if (0 > MDev_GMAC_ScanPhyAddr())
        return -1;

    MDev_GMAC_Patch_PHY();

    get_boot_mem_info(EMAC_MEM, &alloRAM_PA_BASE, &alloRAM_SIZE);
#ifndef GMAC_RX_SOFTWARE_DESCRIPTOR
    //Add Write Protect
    //MHal_GMAC_Write_Protect(alloRAM_PA_BASE, alloRAM_SIZE);
#endif

    GmacThisUVE.initedGMAC = 0;
    MDev_GMAC_HW_init();

    MDev_GMAC_update_mac_address (netdev); // Program ethernet address into MAC //
    spin_lock_irq (LocPtr->lock);
    MHal_GMAC_enable_mdi ();
    MHal_GMAC_read_phy (gmac_phyaddr, GMAC_MII_USCR_REG, &retval);
    if ((retval & (1 << 10)) == 0)   // DSCR bit 10 is 0 -- fiber mode //
        LocPtr->phy_media = PORT_FIBRE;

    spin_unlock_irq (LocPtr->lock);

#ifdef CONFIG_MSTAR_GMAC_HW_TX_CHECKSUM
    retval = MHal_GMAC_Read_JULIAN_0104() | GMAC_TX_CHECKSUM_ENABLE;
    MHal_GMAC_Write_JULIAN_0104(retval);
#endif

    enable_irq(netdev->irq);
    if(LocPtr->ep_flag & GMAC_EP_FLAG_OPEND)
    {
        if(0>MDev_GMAC_open(netdev))
        {
            printk(KERN_WARNING "Driver GMAC: open failed after resume\n");
        }
    }
    return 0;
}

static int mstar_gmac_drv_probe(struct platform_device *pdev)
{
    int retval=0;

    if( !(pdev->name) || strcmp(pdev->name,"Mstar-gmac")
        || pdev->id!=0)
    {
        retval = -ENXIO;
    }
    retval = MDev_GMAC_init();
    if(!retval)
    {
        pdev->dev.platform_data=gmac_dev;
    }
	return retval;
}

static int mstar_gmac_drv_remove(struct platform_device *pdev)
{
    if( !(pdev->name) || strcmp(pdev->name,"Mstar-gmac")
        || pdev->id!=0)
    {
        return -1;
    }
    MDev_GMAC_exit();
    pdev->dev.platform_data=NULL;
    return 0;
}



static struct platform_driver Mstar_gmac_driver = {
	.probe 		= mstar_gmac_drv_probe,
	.remove 	= mstar_gmac_drv_remove,
    .suspend    = mstar_gmac_drv_suspend,
    .resume     = mstar_gmac_drv_resume,

	.driver = {
		.name	= "Mstar-gmac",
        .owner  = THIS_MODULE,
	}
};

static int __init mstar_gmac_drv_init_module(void)
{
    int retval=0;

    gmac_dev=NULL;
    retval = platform_driver_register(&Mstar_gmac_driver);
    return retval;
}

static void __exit mstar_gmac_drv_exit_module(void)
{
    platform_driver_unregister(&Mstar_gmac_driver);
    gmac_dev=NULL;
}



module_init(mstar_gmac_drv_init_module);
module_exit(mstar_gmac_drv_exit_module);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("GMAC Ethernet driver");
MODULE_LICENSE("GPL");

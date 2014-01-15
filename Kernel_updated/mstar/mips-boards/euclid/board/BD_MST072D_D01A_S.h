////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BD_MST072D_D01A_S_
#define _BD_MST072D_D01A_S_

#define BOARD_NAME                  "BD_MST072D_D01A_S"

//------Peripheral Device Setting----------------------------
#ifndef IR_TYPE_SEL
#define IR_TYPE_SEL                 IR_TYPE_MSTAR_DTV
#endif

//------MCU use Scaler internal MPLL clock-------------------
#define XTAL_CLOCK_FREQ             12000000    //12 MHz
#define MCU_XTAL_CLK_HZ             (XTAL_CLOCK_FREQ*MCU_PLL_MUL) // Hz
#define MCU_XTAL_CLK_KHZ            (MCU_XTAL_CLK_HZ / 1000)
#define MCU_XTAL_CLK_MHZ            (MCU_XTAL_CLK_KHZ / 1000)

//------MIU setting ----------------------------------------------------------------
#define MIU_0_02                        0x0C45  // 128MB
#define MIU_1_02                        0x0C45
#define MIU_0_36                        0x0244
#define MIU_0_38                        0x0070
#define ENABLE_MIU_1                    1       // disable MIU1 for bring up
#define DDRII_ODT

#define MIU_SSC

#define MIU_0_SPEED                     400
#define MIU_0_DD                        0x0000
#define MIU_0_1A                        0x5151

#define MIU_1_SPEED                     400
#define MIU_1_DD                        0x0000
#define MIU_1_1A                        0x5151

#if defined(CONFIG_MSTAR_EUCLID_MMAP_128MB_128MB)
#include "mmap/mmap_128mb_128mb.h"
#elif defined(CONFIG_MSTAR_MMAP_128MB_128MB_DEFAULT)
#include "mmap/mmap_128mb_128mb_default.h"
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (??MStar Confidential Information??) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BD_FPGA_
#define _BD_FPGA_

#define BOARD_NAME                  "BD_FPGA"

//------Peripheral Device Setting----------------------------
#ifndef IR_TYPE_SEL
#define IR_TYPE_SEL                 IR_TYPE_MSTAR_DTV
#endif

//------MCU use Scaler internal MPLL clock-------------------
#define XTAL_CLOCK_FREQ             12000000    //12 MHz
#define MCU_XTAL_CLK_HZ             (XTAL_CLOCK_FREQ*MCU_PLL_MUL) // Hz
#define MCU_XTAL_CLK_KHZ            (MCU_XTAL_CLK_HZ / 1000)
#define MCU_XTAL_CLK_MHZ            (MCU_XTAL_CLK_KHZ / 1000)

#if defined(CONFIG_MSTAR_ARM_MMAP_128MB_128MB)
#include "mmap/mmap_128mb_128mb.h"
#elif defined(CONFIG_MSTAR_ARM_MMAP_128MB)
#include "mmap/mmap_128mb.h"
#elif defined(CONFIG_MSTAR_MMAP_128MB_128MB_DEFAULT)
#include "mmap/mmap_128mb_128mb_default.h"
#endif

#endif

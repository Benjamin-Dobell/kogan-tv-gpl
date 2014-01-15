////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2010 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// file    mhal_tsp.c
// @brief  Transport Stream Processer (TSP) Driver
// @author MStar Semiconductor,Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>
#include "mdrv_types.h"

#include "mhal_chiptop_reg.h"
#include "mhal_tsp.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
// External functions
/////////////////////////////////////////////////////////////////////////////////////////////////////

U32 REG32_IndR(REG32 *reg)
{

    // set address
    REG16_T(ADDR_INDR_ADDR0)=   ((((U32)reg)>> 1) & 0xFFFF);
    REG16_T(ADDR_INDR_ADDR1)=   ((((U32)reg)>> 17) & 0xFFFF);

    mb();

    // set command
    REG16_T(ADDR_INDR_CTRL)=    (TSP_IDR_MCUWAIT | TSP_IDR_READ | TSP_IDR_START);

    mb();

    // get read value
    return ((U32)(REG16_T(ADDR_INDR_READ0))| ((U32)(REG16_T(ADDR_INDR_READ1)<< 16)));
}

void REG32_IndW(REG32 *reg, U32 value)
{
    // set address
    REG16_T(ADDR_INDR_ADDR0)=   ((((U32)reg)>> 1) & 0xFFFF);
    REG16_T(ADDR_INDR_ADDR1)=   ((((U32)reg)>> 17) & 0xFFFF);

    // set write value
    REG16_T(ADDR_INDR_WRITE0)=  (value & 0xFFFF);
    REG16_T(ADDR_INDR_WRITE1)=  ((value >> 16) & 0xFFFF);

    mb();

    // set command
    REG16_T(ADDR_INDR_CTRL)=    (TSP_IDR_MCUWAIT | TSP_IDR_WRITE | TSP_IDR_START);

    mb();
}

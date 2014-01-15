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
#ifndef __DRV_MOD_ST_H__
#define __DRV_MOD_ST_H__

//------------------------------------------------------------------------------
// Structure
//------------------------------------------------------------------------------
typedef enum
{
    MOD_CTRL0_LVDS_TI       = BIT2,
    MOD_CTRL0_PDP_10BIT     = BIT3,
    MOD_CTRL0_LVDS_PLASMA   = BIT4,
    MOD_CTRL0_CH_POLARITY   = BIT5,
    MOD_CTRL0_CH_SWAP       = BIT6,
} MOD_CTRL0_e;

typedef enum
{
    MOD_CTRLA_ABS_SWITCH    = BIT0,
    MOD_CTRLA_DUAL_MODE     = BIT1,
    MOD_CTRLA_DE_INVERT     = BIT2,
    MOD_CTRLA_VS_INVERT     = BIT3,
    MOD_CTRLA_CLK_INVERT    = BIT4,
} MOD_CTRLA_e;


#endif // __DRV_MOD_ST_H__ 


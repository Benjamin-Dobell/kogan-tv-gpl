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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   mhal_mbx_interrupt_reg.h
/// @brief  MStar Mailbox Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em>legacy interface is only used by MStar proprietary Mail Message communication\n
/// It's API level for backward compatible and will be remove in the next version.\n
/// Please refer @ref drvGE.h for future compatibility.</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_MBX_INTERRUPT_REG_H
#define _MHAL_MBX_INTERRUPT_REG_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_types.h"

//=============================================================================
// Defines & Macros
//=============================================================================
#define REG_INT_BASE                     (0xBF805600)
#define INT_REG(address)                 (*((volatile MS_U16 *)(REG_INT_BASE + ((address)<<2) )))

#define REG_INT_FIQMASK_L                   0x0000
    #define INT_FIQMASK_AEON2PM             BIT3
    #define INT_FIQMASK_PM2AEON             BIT4
    #define INT_FIQMASK_PM2MIPS              BIT5
    #define INT_FIQMASK_MIPS2PM              BIT6
    #define INT_FIQMASK_MIPS2AEON          BIT7
    #define INT_FIQMASK_AEON2MIPS          BIT8
#define REG_INT_FIQMASK_H                   0x0001
//#define REG_INT_FIQFORCE_L                  0x0002
//#define REG_INT_FIQFORCE_H                  0x0003
#define REG_INT_FIQCLEAR_L                   0x0004
    #define INT_FIQCLEAR_AEON2PM             BIT3
    #define INT_FIQCLEAR_PM2AEON             BIT4
    #define INT_FIQCLEAR_PM2MIPS              BIT5
    #define INT_FIQCLEAR_MIPS2PM              BIT6
    #define INT_FIQCLEAR_MIPS2AEON          BIT7
    #define INT_FIQCLEAR_AEON2MIPS          BIT8
#define REG_INT_FIQCLEAR_H                   0x0005
//#define REG_INT_FIQRAWSTATUS_L          0x0006
//#define REG_INT_FIQRAWSTATUS_H          0x0007
#define REG_INT_FIQFINALSTATUS_L        0x0008
    #define INT_FIQFINALSTATUS_AEON2PM  BIT3
    #define INT_FIQFINALSTATUS_PM2AEON  BIT4
    #define INT_FIQFINALSTATUS_PM2MIPS  BIT5
    #define INT_FIQFINALSTATUS_MIPS2PM  BIT6
    #define INT_FIQFINALSTATUS_MIPS2AEON    BIT7
    #define INT_FIQFINALSTATUS_AEON2MIPS    BIT8
#define REG_INT_FIQFINALSTATUS_H        0x0009

#define REG_INT_PMFIRE          0x003E
    #define INT_PM2AEON               BIT0
    #define INT_PM2MIPS                BIT1
#define REG_INT_AEONFIRE      0x003F
    #define INT_AEON2PM               BIT0
    #define INT_AEON2MIPS            BIT1


#define REG_AEON1_INT_BASE         (0xBF800A80)
#define AEON1_INT_REG(address)                 (*((volatile MS_U16 *)(REG_AEON1_INT_BASE + ((address)<<2) )))
#define REG_INT_MIPSFIRE                   0x001F
    #define INT_MIPS2PM                           BIT0
    #define INT_MIPS2AEON                       BIT1


#endif //_MHAL_MBX_INTERRUPT_REG_H


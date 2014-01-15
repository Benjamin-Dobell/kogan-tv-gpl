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
//#define REG_INT_BASE                     (0xBF203200) //0x101900
#define INT_REG(address)                    (*((volatile MS_U16 *)(REG_INT_BASE + ((address)<<2) )))

#define INTNO_BASE_16_31                    16 //16~31
#define INTNO_FRCR2_MIPS                    30
#define INTNO_BASE_32_47                    32 //32~47
#define INTNO_PM_MIPS                       38
#define INTNO_MIPS_PM                       42


// interrupt index address
// For PM
#define REG_INT_FIQMASK_PM                  0x06
#define REG_INT_FIQCLEAR_PM                 0x0E
#define REG_INT_FIQFINALSTATUS_PM           0x0E
// For MIPS
#define REG_INT_FIQMASK_MIPS                0x26
#define REG_INT_FIQCLEAR_MIPS               0x2E
#define REG_INT_FIQFINALSTATUS_MIPS         0x2E
    #define _BIT_PM_MIPS                    (1<<(INTNO_PM_MIPS-INTNO_BASE_32_47))
    #define _BIT_MIPS_PM                    (1<<(INTNO_MIPS_PM-INTNO_BASE_32_47))
#define REG_INT_FIQLMASK_MIPS               0x25
#define REG_INT_FIQLCLEAR_MIPS              0x2D
#define REG_INT_FIQLFINALSTATUS_MIPS        0x2D
    #define _BIT_FRCR2_MIPS                 (1<<(INTNO_FRCR2_MIPS-INTNO_BASE_16_31))

// Non-PM CPU Interrupt
#define REG_CPU_INT_BASE                    (0xBF200A80) //0x100540
#define CPU_INT_REG(address)                (*((volatile MS_U16 *)(REG_CPU_INT_BASE + ((address)<<2) )))
#define REG_INT_PMFIRE                      0x0000
    #define INT_PM_MIPS                     BIT0

#define REG_INT_MIPSFIRE                    0x0002
    #define INT_MIPS_PM                     BIT0


//################
//
// FRC - FIQ
//
#define REG_FRC_INT_BASE                    (0xBF602200) //0x301100
#define INT_FRC_REG(address)                (*((volatile MS_U16 *)(REG_FRC_INT_BASE + ((address)<<2) )))

// interrupt number
#define INTFRCNO_BASE_0_15                  0 //0~15
#define INTFRCNO_MIPS_FRCR2                 0
#define INTFRCNO_FRCR2_MIPS                 4
// interrupt index address
#define REG_FRC_INT_FIQMASK_MIPS            0x04
#define REG_FRC_INT_FIQCLEAR_MIPS           0x0C
#define REG_FRC_INT_FIQFINALSTATUS_MIPS     0x0C
#define REG_FRC_INT_FIQMASK_FRCR2           0x24
#define REG_FRC_INT_FIQCLEAR_FRCR2          0x2C
#define REG_FRC_INT_FIQFINALSTATUS_FRCR2    0x2C
    #define _BITFRC_MIPS_FRCR2              (1<<(INTFRCNO_MIPS_FRCR2-INTFRCNO_BASE_0_15))
    #define _BITFRC_FRCR2_MIPS              (1<<(INTFRCNO_FRCR2_MIPS-INTFRCNO_BASE_0_15))

// FRC CPU Interrupt
#define REG_FRC_CPU_INT_BASE                (0xBF600B00) //0x300580
#define FRC_CPU_INT_REG(address)            (*((volatile MS_U16 *)(REG_FRC_CPU_INT_BASE + ((address)<<2) )))
#define REG_FRC_INT_MIPSFIRE                0x0000
    #define INTFRC_MIPS_FRCR2               BIT0
    
#define REG_FRC_INT_FRCR2FIRE               0x0002
    #define INTFRC_FRCR2_MIPS               BIT0

#endif //_MHAL_MBX_INTERRUPT_REG_H


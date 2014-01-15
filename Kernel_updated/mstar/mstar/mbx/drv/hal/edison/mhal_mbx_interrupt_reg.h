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
//#define REG_INT_BASE                     (0xBF203200)
#define INT_REG(address)                 (*((volatile MS_U16 *)(REG_INT_BASE + ((address)<<2) )))

#define INTNO_BASE_32                       32
#define INTNO_BASE_47                       47
#define INTNO_PM_ARM                        38
#define INTNO_ARM_PM                        42


// interrupt index address
#define REG_INT_FIQMASK_PM                  0x06
#define REG_INT_FIQMASK_ARM                 0x26

#define REG_INT_FIQCLEAR_PM                 0x0E
#define REG_INT_FIQCLEAR_ARM                0x2E

#define REG_INT_FIQFINALSTATUS_PM           0x0E
#define REG_INT_FIQFINALSTATUS_ARM          0x2E

    #define _BIT_PM_ARM              (1<<(INTNO_PM_ARM-INTNO_BASE_32))
    #define _BIT_ARM_PM              (1<<(INTNO_ARM_PM-INTNO_BASE_32))


#define REG_CPU_INT_BASE                 (0xFD200A80)
#define CPU_INT_REG(address)             (*((volatile MS_U16 *)(REG_CPU_INT_BASE + ((address)<<2) )))
#define REG_INT_PMFIRE                   0x0000
    #define INT_PM_ARM                   BIT0

#define REG_INT_ARMFIRE                  0x0002
    #define INT_ARM_PM                   BIT0


#endif //_MHAL_MBX_INTERRUPT_REG_H


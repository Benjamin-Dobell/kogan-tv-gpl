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

#define INTNO_PM_H3                         36
#define INTNO_PM_H2                         37
#define INTNO_PM_AEON                       38

#define INTNO_AEON_H3                       40
#define INTNO_AEON_H2                       41
#define INTNO_AEON_PM                       42

#define INTNO_H2_H3                         44
#define INTNO_H2_AEON                       45
#define INTNO_H2_PM                         46

#define INTNO_H3_H2                         48
#define INTNO_H3_AEON                       49
#define INTNO_H3_PM                         50

// interrupt index address
#define REG_INT_FIQMASK_PM                  0x06
#define REG_INT_FIQMASK_AEON                0x26
#define REG_INT_FIQMASK_H2                  0x46
#define REG_INT_FIQMASK_H3                  0x66

#define REG_INT_FIQCLEAR_PM                 0x0E
#define REG_INT_FIQCLEAR_AEON               0x2E
#define REG_INT_FIQCLEAR_H2                 0x4E
#define REG_INT_FIQCLEAR_H3                 0x6E

#define REG_INT_FIQFINALSTATUS_PM           0x0E
#define REG_INT_FIQFINALSTATUS_AEON         0x2E
#define REG_INT_FIQFINALSTATUS_H2           0x4E
#define REG_INT_FIQFINALSTATUS_H3           0x6E

    #define _BIT_PM_H3               (1<<(INTNO_PM_H3-INTNO_BASE_32))
    #define _BIT_PM_H2               (1<<(INTNO_PM_H2-INTNO_BASE_32))
    #define _BIT_PM_AEON             (1<<(INTNO_PM_AEON-INTNO_BASE_32))

    #define _BIT_AEON_H3             (1<<(INTNO_AEON_H3-INTNO_BASE_32))
    #define _BIT_AEON_H2             (1<<(INTNO_AEON_H2-INTNO_BASE_32))
    #define _BIT_AEON_PM             (1<<(INTNO_AEON_PM-INTNO_BASE_32))

    #define _BIT_H3_H2               (1<<(INTNO_H3_H2-INTNO_BASE_47))
    #define _BIT_H3_AEON             (1<<(INTNO_H3_AEON-INTNO_BASE_47))
    #define _BIT_H3_PM               (1<<(INTNO_H3_PM-INTNO_BASE_47))

#define REG_CPU_INT_BASE                 (0xBF200A80)
#define CPU_INT_REG(address)             (*((volatile MS_U16 *)(REG_CPU_INT_BASE + ((address)<<2) )))
#define REG_INT_PMFIRE                   0x0000
    #define INT_PM_AEON                  BIT0
    #define INT_PM_H2                    BIT1
    #define INT_PM_H3                    BIT2

#define REG_INT_AEONFIRE                 0x0002
    #define INT_AEON_PM                  BIT0
    #define INT_AEON_H2                  BIT1
    #define INT_AEON_H3                  BIT2

#define REG_INT_H2FIRE                   0x0004
    #define INT_H2_PM                    BIT0
    #define INT_H2_AEON                  BIT1
    #define INT_H2_H3                    BIT2

#define REG_INT_H3FIRE                   0x0004
    #define INT_H3_PM                    BIT0
    #define INT_H3_AEON                  BIT1
    #define INT_H3_H2                    BIT2


#endif //_MHAL_MBX_INTERRUPT_REG_H


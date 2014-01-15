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
/// @file   mhal_mbx_interrupt.h
/// @brief  MStar Mailbox Interrupt HAL DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em>legacy interface is only used by MStar proprietary Mail Message communication\n
/// It's API level for backward compatible and will be remove in the next version.\n
/// Please refer @ref drvGE.h for future compatibility.</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_MBX_INTERRUPT_H
#define _MHAL_MBX_INTERRUPT_H

#ifdef _MHAL_MBX_INTERRUPT_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//=============================================================================
// Includs
//=============================================================================
#include "mhal_mbx_interrupt_reg.h"
#include "chip_int.h"

//=============================================================================
// Defines & Macros
//=============================================================================
#define E_FIQ_INT_AEON_TO_8051    E_FIQVIRL_AEON_TO_8051 //virtual
#define E_FIQ_INT_MIPS_TO_8051    E_FIQEXPL_ARM_TO_8051
#define E_FIQ_INT_8051_TO_AEON    E_FIQVIRL_8051_TO_AEON //virtual
#define E_FIQ_INT_MIPS_TO_AEON    E_FIQVIRL_ARM_TO_FRCR2 //virtual
#define E_FIQ_INT_8051_TO_MIPS    E_FIQEXPL_8051_TO_ARM
#define E_FIQ_INT_AEON_TO_MIPS    E_FIQ_FRCR2ARM

#define MBX_INT_AEON2PM     (E_FIQ_INT_AEON_TO_8051-E_FIQEXPL_START)
#define MBX_INT_PM2AEON     (E_FIQ_INT_8051_TO_AEON-E_FIQEXPL_START)
#define MBX_INT_PM2MIPS     (E_FIQ_INT_8051_TO_MIPS-E_FIQEXPL_START)
#define MBX_INT_MIPS2PM     (E_FIQ_INT_MIPS_TO_8051-E_FIQEXPL_START)
#define MBX_INT_MIPS2AEON   (E_FIQ_INT_MIPS_TO_AEON-E_FIQEXPL_START)
#define MBX_INT_AEON2MIPS   (E_FIQ_INT_AEON_TO_MIPS-E_FIQEXPL_START)

//=============================================================================
// Type and Structure Declaration
//=============================================================================
typedef void (*MBX_MSGRECV_CB_FUNC)(MS_S32 s32Irq);

//=============================================================================
// Function
//=============================================================================
INTERFACE MBX_Result MHAL_MBXINT_Init (MBX_CPU_ID eHostCPU, MBX_MSGRECV_CB_FUNC pMBXRecvMsgCBFunc);
INTERFACE void MHAL_MBXINT_DeInit (MBX_CPU_ID eHostCPU);
INTERFACE MBX_Result MHAL_MBXINT_ResetHostCPU (MBX_CPU_ID ePrevCPU, MBX_CPU_ID eConfigCpu);
INTERFACE MBX_Result MHAL_MBXINT_Enable (MS_S32 s32Fiq, MS_BOOL bEnable);
INTERFACE MBX_Result MHAL_MBXINT_Fire (MBX_CPU_ID eDstCPUID, MBX_CPU_ID eSrcCPUID);
INTERFACE MBX_Result MHAL_MBXINT_Clear (MS_S32 s32Fiq);

#undef INTERFACE

#endif //_MHAL_MBX_INTERRUPT_H


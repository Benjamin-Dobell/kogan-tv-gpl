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
///
/// file    regMailBox.h
/// @brief  MailBox Registers Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_MAILBOX_REG_H_
#define _MHAL_MAILBOX_REG_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
//Total Mailbox buffer 96 Bytes

//TBD: Refine it
#if 1
#define MB_SIZE_8051_TO_AEON    16
#define MB_SIZE_AEON_TO_8051    16
#define MB_SIZE_AEON_TO_MIPS    16
#define MB_SIZE_MIPS_TO_AEON    16
#define MB_SIZE_MIPS_TO_8051    16
#define MB_SIZE_8051_TO_MIPS    16
#endif

typedef enum
{
    //IRQ
    E_MB_START                  = 0,
    E_MB_8051_TO_AEON           = E_MB_START+0,
    E_MB_AEON_TO_8051           = E_MB_8051_TO_AEON + MB_SIZE_8051_TO_AEON,
    E_MB_AEON_TO_MIPS           = E_MB_AEON_TO_8051 + MB_SIZE_AEON_TO_8051,
    E_MB_MIPS_TO_AEON           = E_MB_AEON_TO_MIPS + MB_SIZE_AEON_TO_MIPS,
    E_MB_MIPS_TO_8051           = E_MB_MIPS_TO_AEON + MB_SIZE_MIPS_TO_AEON,
    E_MB_8051_TO_MIPS           = E_MB_MIPS_TO_8051 + MB_SIZE_MIPS_TO_8051,
    E_MB_END                    = E_MB_8051_TO_MIPS + MB_SIZE_8051_TO_MIPS
} MailBoxOffset;

#define MAILBOX_REG_BASE                0xBF806700  /*RT:0x19c0*/

#define REG_MB_8051_TO_AEON     (MAILBOX_REG_BASE + (E_MB_8051_TO_AEON*2))
#define REG_MB_AEON_TO_8051     (MAILBOX_REG_BASE + (E_MB_AEON_TO_8051*2))
#define REG_MB_AEON_TO_MIPS     (MAILBOX_REG_BASE + (E_MB_AEON_TO_MIPS*2))
#define REG_MB_MIPS_TO_AEON     (MAILBOX_REG_BASE + (E_MB_MIPS_TO_AEON*2))
#define REG_MB_MIPS_TO_8051     (MAILBOX_REG_BASE + (E_MB_MIPS_TO_8051*2))
#define REG_MB_8051_TO_MIPS     (MAILBOX_REG_BASE + (E_MB_8051_TO_MIPS*2))


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _MHAL_MAILBOX_REG_H_


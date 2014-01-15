//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all 
// or part of MStar Software is expressly prohibited, unless prior written 
// permission has been granted by MStar. 
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.  
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software. 
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s 
//    confidential information in strictest confidence and not disclose to any
//    third party.  
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.  
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or 
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.  
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#include "linux/kernel.h"
#include "halGOP.h"
#include "regGOP.h"

//------------------------------------------------------------------------------
//  Driver Compiler Options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Local Defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Local Var
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Global Functions
//------------------------------------------------------------------------------
U8 _HAL_GOP_GetBnkOfstByGop(U8 gop, U32 *pBnkOfst)
{
    if (gop==0)
        *pBnkOfst = GOP_4G_OFST<<8;
    else if (gop==1)
        *pBnkOfst = GOP_2G_OFST<<8;
    else if (gop==2)
        *pBnkOfst = GOP_1G_OFST<<8;
    else if (gop==3)
        *pBnkOfst = GOP_1GX_OFST<<8;
    else if (gop==4)
        *pBnkOfst = GOP_DW_OFST<<8;
    else
        return FALSE;
    return TRUE;
}
void _HAL_GOP_Read16Reg(U32 u32addr, U16* pu16ret)
{
    *pu16ret = GOP_READ2BYTE(u32addr);
}

void _HAL_GOP_Write16Reg(U32 u32addr, U16 u16val, U16 mask)
{
	U16 u16tmp = 0;

	u16tmp = GOP_READ2BYTE(u32addr);
    u16tmp &= ~mask;
    u16val &=  mask;
    u16val |=  u16tmp;
	GOP_WRITE2BYTE(u32addr,u16val);
}
void _HAL_GOP_Write32Reg(U32 u32addr, U32 u32val)
{
	GOP_WRITE2BYTE(u32addr, (u32val&0xFFFF));
	GOP_WRITE2BYTE(u32addr+2, (u32val&0xFFFF0000)>>16);
}
void _HAL_GOP_Init(U8 u8GOP)
{
    U32 u32bankoff = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32bankoff);
    _HAL_GOP_Write16Reg(u32bankoff+GOP_4G_CTRL0, 0x000, 0x200);     // Genshot fast=0 for t3, for T4 and after no need to set this bit.
}
void _HAL_GOP_SetBlending(U8 u8GOP, U8 u8win, U8 bEnable, U8 u8coef)
{
    U32 u32bankoff = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32bankoff);

	_HAL_GOP_Write16Reg(u32bankoff+GOP_4G_GWIN0_CTRL(u8win), bEnable<<14, 0x4000);
	_HAL_GOP_Write16Reg(u32bankoff+GOP_4G_GWIN_ALPHA01(u8win), u8coef, 0xFF);
}
void _HAL_GOP_SetGOPEnable2SC(U8 u8GOP, U8 bEnable)
{
    /* GOP OP Path enable to SC Setting
        A5: GOP OP Path blending with SC sequence
        mux1-->mux0-->mux2-->mux3
    */
    U32 u32bankoff = 0;   
    U16 muxValue=0, regval=0;

	_HAL_GOP_GetBnkOfstByGop(u8GOP, &u32bankoff);
	
    _HAL_GOP_Read16Reg(u32bankoff+GOP_MUX, &muxValue);
    _HAL_GOP_Read16Reg(GOP_SC_GOPEN, &regval);

    if (u8GOP== (muxValue & GOP_REGMUX_MASK))    //enable mux0 to SC
    {
        _HAL_GOP_Write16Reg(GOP_SC_GOPEN, bEnable?(regval |0x8000):(regval & ~0x8000), 0x8000);
    }
    else if (u8GOP== ((muxValue & (GOP_BIT5|GOP_BIT4|GOP_BIT3))>>(GOP_MUX_SHIFT*1))) //enable mux1
    {
        _HAL_GOP_Write16Reg(GOP_SC_GOPEN, bEnable?(regval |0x1000):(regval & ~0x1000), 0x1000);
    }
    else if (u8GOP== ((muxValue & (GOP_BIT8|GOP_BIT7|GOP_BIT6))>>(GOP_MUX_SHIFT*2))) //enable mux2
    {
        _HAL_GOP_Write16Reg(GOP_SC_GOPEN, bEnable?(regval |0x4000):(regval & ~0x4000), 0x4000);
    }
    else if (u8GOP== ((muxValue & (GOP_BIT11|GOP_BIT10|GOP_BIT9))>>(GOP_MUX_SHIFT*3))) //enable mux3
    {
        _HAL_GOP_Write16Reg(GOP_SC_GOPEN, bEnable?(regval |0x2000):(regval & ~0x2000), 0x2000);
    }
    return;
}


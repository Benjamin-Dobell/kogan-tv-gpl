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
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   drvGOP.h
/// @brief  Graphics Output Path Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_GOP_H_
#define _DRV_GOP_H_

#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#include "mdrv_types.h"

#define GOP_STRETCH_WIDTH_UNIT          2    // 2-pixel
#define SCALING_MULITPLIER              0x1000
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
/// Define GOP driver/hal layer return code, this enum should sync with E_GOP_API_Result
typedef enum
{
    /// Color format RGB555 and Blink.
    E_DRV_GOP_COLOR_RGB555_BLINK    =0,
    /// Color format RGB565.
    E_DRV_GOP_COLOR_RGB565          =1,
    /// Color format ARGB4444.
    E_DRV_GOP_COLOR_ARGB4444        =2,
    /// Color format alpha blink.
    E_DRV_GOP_COLOR_2266      =3,
    /// Color format I8 (256-entry palette).
    E_DRV_GOP_COLOR_I8              =4,
    /// Color format ARGB8888.
    E_DRV_GOP_COLOR_ARGB8888        =5,
    /// Color format ARGB1555.
    E_DRV_GOP_COLOR_ARGB1555        =6,
    /// Color format ARGB8888.  - Andriod format
    E_DRV_GOP_COLOR_ABGR8888        =7,
    /// Color format RGB555/YUV422.
    E_DRV_GOP_COLOR_RGB555YUV422    =8,
    /// Color format YUV422.
    E_DRV_GOP_COLOR_YUV422          =9,
    /// Color format ARGB8888.  - Andriod format
    E_DRV_GOP_COLOR_RGBA5551        =10,
    /// Color format ARGB8888.  - Andriod format
    E_DRV_GOP_COLOR_RGBA4444        =11,

    /// Invalid color format.
    E_DRV_GOP_COLOR_INVALID
} DRV_GOPColorType;

typedef enum
{
    E_DRV_GOP_DST_OP0       =   2,
} DRV_GOPDstType;

typedef enum
{
    ///Select gop output to mux0
    E_GOP_MUX0 = 0,
    /// Select gop output to mux1
    E_GOP_MUX1 = 1,
    /// Select gop output to mux2
    E_GOP_MUX2 = 2,
    /// Select gop output to mux3
    E_GOP_MUX3 = 3,
        ///Select gop output to IP0
    MAX_GOP_MUX_SUPPORT,
} Gop_MuxSel;

typedef enum
{
    /// E_DRV_GOP_SEL_MIU0. gop access miu 0
    E_DRV_GOP_SEL_MIU0    = 0,
    /// E_GOP_SEL_MIU1. gop access miu1
    E_DRV_GOP_SEL_MIU1    = 1,
} E_DRV_GOP_SEL_TYPE;

typedef struct
{
    U16 u16HStart;		//!< unit: pix
    U16 u16HEnd;		//!< unit: pix
    U16 u16VStart;		//!< unit: pix
    U16 u16VEnd;		//!< unit: pix
    U32 u16Pitch;		//!< unit: Byte
    U32 u32Addr;		//!< unit: pix
    DRV_GOPColorType clrType;       //!< color format of the buffer
} DRV_GWIN_INFO;

///the GOP and mux setting info
typedef struct
{
  //GopMux arrays record the u8GopIndex and the corresponding MuxIndex
  //u8MuxCounts: how many mux need to modify the gop settings

  struct
  {
      U8 u8GopIndex;
      U8 u8MuxIndex;
  }GopMux[6];
  U8 u8MuxCounts;
}GOP_MuxConfig;

//----------------------------------------------------------------------------
// GOP Reg Page Form
//----------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
void _MDrv_GOP_UpdateReg(U8 u8Gop);
void _MDrv_GOP_SetForceWrite(U8 bEnable);
void _MDrv_GOP_Init(U8 u8GOP);
void _MDrv_GOP_SetStretchWin(U8 u8GOP,U16 x, U16 y, U16 width, U16 height);
void _MDrv_GOP_SetHScale(U8 u8GOP,U8 bEnable, U16 src, U16 dst );
void _MDrv_GOP_SetVScale(U8 u8GOP,U8 bEnable, U16 src, U16 dst );
void _MDrv_GOP_SetGwinInfo(U8 u8GOP,U8 u8win,DRV_GWIN_INFO WinInfo);
void _MDrv_GOP_SetBlending(U8 u8GOP, U8 u8win, U8 bEnable, U8 u8coef);
void _MDrv_GOP_SetDstPlane(U8 u8GOP,DRV_GOPDstType eDstType);
void _MDrv_GOP_GWIN_Enable(U8 u8GOP,U8 u8win,u8 bEnable);
void _MDrv_GOP_EnableHMirror(U8 u8GOP,u8 bEnable);
void _MDrv_GOP_EnableVMirror(U8 u8GOP,u8 bEnable);
void _MDrv_GOP_SetMux(GOP_MuxConfig GopMuxConfig);
U8 _MDrv_GOP_GetMux(Gop_MuxSel eGopMux);
void _MDrv_GOP_SetPipeDelay(U8 u8GOP,U16 u16PnlHstart);
void _MDrv_GOP_MIU_Sel(U8 u8GOP,E_DRV_GOP_SEL_TYPE miusel);

#ifdef __cplusplus
}
#endif


#endif // _DRV_TEMP_H_

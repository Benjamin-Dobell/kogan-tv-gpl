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
/// @file   mhal_gflip.h
/// @brief  MStar GFLIP Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_GFLIP_H
#define _HAL_GFLIP_H

#ifdef _HAL_GFLIP_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines & Macros
//=============================================================================
#define MAX_GOP_SUPPORT         3
#define MAX_GOP0_GWIN            4
#define MAX_GOP1_GWIN            2
#define MAX_GOP2_GWIN            1
#define MAX_GOP_GWIN              MAX_GOP0_GWIN

#define GFLIP_GOP_BANKOFFSET    (0x3)
#define GFLIP_ADDR_ALIGN_RSHIFT (0x3)

#define GFLIP_GOP_IDX_4G             (0x0)
#define GFLIP_GOP_IDX_2G             (0x1)
#define GFLIP_GOP_IDX_1G             (0x2)
#define GFLIP_GOP_IDX_1GX            (0x3)
#define GFLIP_GOP_IDX_DWIN        (0x4)
#define GFLIP_GOP_IDX_INVALID       (0xFFFFFFFF)

#define GFLIP_GOP_BANK_IDX_0     (0x0)
#define GFLIP_GOP_BANK_IDX_1     (0x1)

#define GFLIP_GOP_DWIN_BANKID     0x6

#define GFLIP_GOP_DST_IPMAIN   0x0
#define GFLIP_GOP_DST_MIXER2VE 0x1
#define GFLIP_GOP_DST_OP0      0x2
#define GFLIP_GOP_DST_IPSUB    0x3
#define GFLIP_GOP_DST_VOP      0x4

//=============================================================================
// HAL Driver Function
//=============================================================================

INTERFACE MS_BOOL MHal_GFLIP_IntEnable(MS_U32 u32GopIdx, MS_BOOL bEnable);
INTERFACE MS_BOOL MHal_GFLIP_IsVSyncInt(MS_U32 u32GopIdx);
INTERFACE MS_U32 MHal_GFLIP_GetIntGopIdx(void);
INTERFACE MS_BOOL MHal_GFLIP_SetFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32MainAddr, MS_U32 u32SubAddr);

INTERFACE MS_BOOL MHal_GFLIP_ClearDWINIRQ(GFLIP_DWININT_INFO *pGFlipDWinIntInfo);

INTERFACE MS_BOOL MHal_GFLIP_HandleVsyncLimitation(MS_U32 u32GopIdx);
INTERFACE MS_BOOL MHal_GFLIP_RestoreFromVsyncLimitation(MS_U32 u32GopIdx);

INTERFACE MS_BOOL MHal_GFLIP_IsTagIDBack(MS_U16 u16TagId);
INTERFACE MS_U16 MHal_GFLIP_GetGopDst(MS_U32 u32GopIdx);
#undef INTERFACE
#endif //_HAL_GFLIP_H


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

#include <common.h>
#include <config.h>
#include <MsOS.h>
#include <apiGOP.h>
#include <apiPNL.h>
#include <gop/MsDrvGop.h>
#include <uboot_mmap.h>
#include <ShareType.h>

#include <bootlogo/MsPoolDB.h>

#ifndef UNUSED
#define UNUSED( var )                   ((void)(var))
#endif
extern U8 g_logoGopIdx;
U8 u8OsdLayerFbId=0xFF, u8OsdLayerGwinId=0xFF;
U16 u16OsdW = 0,u16OsdH = 0;
U8 g_logoGopIdx = 0;
U8 gop_hwcursor = 3;


MS_BOOL _sc_is_interlace(void)
{
    return 0;
}

MS_U16 _sc_get_h_cap_start(void)
{
    return 0x60;
}
void _Sys_PQ_ReduceBW_ForOSD(MS_U8 PqWin, MS_BOOL bOSD_On)
{
    PqWin=bOSD_On=0;
    UNUSED(PqWin);
    UNUSED(bOSD_On);
}

void MsDrvGOP_SetStretch(GFX_BufferInfo *dst_info, MS_BOOL bHorStretch, MS_BOOL bVerStretch,MS_U16 u16DispX,MS_U16 u16DispY)
{
    U32 u32GWinDispX_Max;
    U32 u32GWinDispY_Max;

    u32GWinDispX_Max = g_IPanel.Width();
    u32GWinDispY_Max = g_IPanel.Height();

    MApi_GOP_GWIN_Set_HSCALE(bHorStretch, dst_info->u32Width, u32GWinDispX_Max);

    MApi_GOP_GWIN_Set_VSCALE(bVerStretch, dst_info->u32Height, u32GWinDispY_Max);

    MApi_GOP_GWIN_Set_STRETCHWIN(g_logoGopIdx, E_GOP_DST_OP0,u16DispX,u16DispY, dst_info->u32Pitch, dst_info->u32Height);
}


void gop_stretch(U16 graph_pitch, U16 graph_width, U16 graph_height, MS_BOOL bHorStretch, MS_BOOL bVerStretch)
{
    U32 u32GWinDispX_Max;
    U32 u32GWinDispY_Max;


    u32GWinDispX_Max = g_IPanel.Width();
    u32GWinDispY_Max = g_IPanel.Height();

    if (bHorStretch)
        MApi_GOP_GWIN_Set_HSCALE(TRUE, graph_width, u32GWinDispX_Max);
    if (bVerStretch)
        MApi_GOP_GWIN_Set_VSCALE(TRUE, graph_height, u32GWinDispY_Max);

    MApi_GOP_GWIN_Set_STRETCHWIN(g_logoGopIdx, E_GOP_DST_OP0, 0, 0, graph_pitch, graph_height);
}

void MsDrvGFX_Init(void)
{
    GFX_Config tGFXcfg;

    tGFXcfg.bIsCompt = TRUE;
    tGFXcfg.bIsHK = TRUE;
    MApi_GFX_Init(&tGFXcfg);

}
void MsDrvGFX_Blt(GFX_DrawRect *rect,GFX_BufferInfo *src_info, GFX_BufferInfo *dst_info )
{
    GFX_Point gfxPt0 = { rect->dstblk.x, rect->dstblk.y };
    GFX_Point gfxPt1 = { rect->dstblk.x+rect->dstblk.width,rect->dstblk.y+rect->dstblk.height };

    MApi_GFX_SetSrcBufferInfo(src_info, 0);

    MApi_GFX_SetDstBufferInfo(dst_info, 0);

    MApi_GFX_SetClip(&gfxPt0, &gfxPt1);

    MApi_GFX_SetDC_CSC_FMT(GFX_YUV_RGB2YUV_PC, GFX_YUV_OUT_PC, GFX_YUV_IN_255, GFX_YUV_YVYU,  GFX_YUV_YUYV);

    if( (rect->srcblk.width!= rect->dstblk.width)||(rect->srcblk.height!= rect->dstblk.height) )
    {
        MApi_GFX_BitBlt(rect, GFXDRAW_FLAG_SCALE);
    }
    else
    {
        MApi_GFX_BitBlt(rect, 0);
    }
    MApi_GFX_FlushQueue();

}
void MsDrvGOP_Init(void)
{
    MS_U8 u8Idx;
    GOP_InitInfo pGopInit;
    //MApi_GOP_RegisterFBFmtCB(( U32(*)(MS_U16 pitch,U32 addr , MS_U16 fmt ))OSD_RESOURCE_SetFBFmt);
    MApi_GOP_RegisterXCIsInterlaceCB(_sc_is_interlace);
    MApi_GOP_RegisterXCGetCapHStartCB(_sc_get_h_cap_start);
    MApi_GOP_RegisterXCReduceBWForOSDCB(_Sys_PQ_ReduceBW_ForOSD);
    pGopInit.u16PanelWidth = g_IPanel.Width();
    pGopInit.u16PanelHeight = g_IPanel.Height();
    pGopInit.u16PanelHStr = g_IPanel.HStart();
    pGopInit.u32GOPRBAdr = 0x0;//((GOP_GWIN_RB_MEMORY_TYPE & MIU1) ? (GOP_GWIN_RB_ADR | MIU_INTERVAL) : (GOP_GWIN_RB_ADR));
    pGopInit.u32GOPRBLen = 0x0;

    //there is a GOP_REGDMABASE_MIU1_ADR for MIU1
    pGopInit.u32GOPRegdmaAdr = 0;//((GOP_REGDMABASE_MEMORY_TYPE & MIU1) ? (GOP_REGDMABASE_ADR | MIU_INTERVAL) : (GOP_REGDMABASE_ADR));
    pGopInit.u32GOPRegdmaLen = 0;
    pGopInit.bEnableVsyncIntFlip = FALSE;
    MApi_GOP_InitByGOP(&pGopInit, g_logoGopIdx);

    for(u8Idx=0;u8Idx< MApi_GOP_GWIN_GetGwinNum(g_logoGopIdx);u8Idx++)
    {
        MApi_GOP_GWIN_DeleteWin(u8Idx);
    }

}

void MsDrvGOP_Show(GFX_BufferInfo *dst_info,MS_BOOL bHorStretch, MS_BOOL bVerStretch,MS_U16 u16DispX,MS_U16 u16DispY)
{
    MApi_GOP_GWIN_SwitchGOP(g_logoGopIdx);
    MsDrvGOP_SetStretch(dst_info, bHorStretch, bVerStretch,u16DispX,u16DispY);

    MApi_GOP_GWIN_SetGOPDst(g_logoGopIdx, E_GOP_DST_OP0);
    
    u8OsdLayerFbId = MApi_GOP_GWIN_GetFreeFBID();
    u8OsdLayerGwinId = MApi_GOP_GWIN_GetFreeWinID();
    
    MApi_GOP_GWIN_CreateFBbyStaticAddr(u8OsdLayerFbId, 0, 0, dst_info->u32Pitch, dst_info->u32Height, E_MS_FMT_YUV422, dst_info->u32Addr);
    
    MApi_GOP_GWIN_MapFB2Win(u8OsdLayerFbId, u8OsdLayerGwinId);
    
    MApi_GOP_GWIN_SetBlending(u8OsdLayerGwinId, FALSE, 0x3F);
    
    MApi_GOP_GWIN_Enable(u8OsdLayerGwinId, TRUE);
    
}

#if (ENABLE_DISPLAY_LOGO) // GEGOP
int do_destroy_logo (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    if (u8OsdLayerGwinId != 0xFF)
    {
        MApi_GOP_GWIN_Set_HSCALE(FALSE, 0, 0);
        MApi_GOP_GWIN_Set_VSCALE(FALSE, 0, 0);
        MApi_GOP_GWIN_Enable(u8OsdLayerGwinId, FALSE);
        MApi_GOP_GWIN_DestroyWin(u8OsdLayerGwinId);
        u8OsdLayerGwinId = 0xFF;
    }

    if (u8OsdLayerFbId != 0xFF)
    {
        MApi_GOP_GWIN_DestroyFB(u8OsdLayerFbId);
        u8OsdLayerFbId = 0xFF;
    }
    return 0;
}

U_BOOT_CMD(
destroy_logo,    1,   1,  do_destroy_logo,
"destroy_logo - Stop logo display \n",
"Stop logo display\n"
);


#endif

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
//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------

#include <MsTypes.h>
#include <apiPNL.h>
#include <common.h>
#include <command.h>
#include <bootlogo/MsPoolDB.h>
#include <panel/MsDrvPanel.h>
#if(ENABLE_URSA_6M30 == 1)
#include <ursa/ursa_6m30.h>
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MS_BOOL bPanleReady=FALSE;
//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------
MS_BOOL MsAPI_PNL_Init(PanelType*  panel_data);


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int IsPanelReady(void)
{
    if(bPanleReady==TRUE)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
MS_BOOL MsAPI_PNL_Init(PanelType*  panel_data)
{
    return MsDrv_PNL_Init(panel_data);
}

int do_panel_sinit( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    PanelType* panel_data = NULL;
    //load panel para
    PANEL_RESOLUTION_TYPE enPanelType;
    enPanelType = MApi_PNL_GetPnlTypeSetting();
    if(MApi_PNL_PreInit(E_PNL_NO_OUTPUT)!=TRUE)
    {
        bPanleReady=FALSE;
        return -1;
    }

    panel_data=MApi_XC_GetPanelSpec(enPanelType);
    if(panel_data==NULL)
    {
        bPanleReady=FALSE;
        return -1;
    }
    //pane init
    #ifdef BD_LVDS_CONNECT_TYPE
	MApi_BD_LVDS_Output_Type(BD_LVDS_CONNECT_TYPE);
    #endif
    if(MsAPI_PNL_Init(panel_data)==FALSE)
    {
        bPanleReady=FALSE;
        return -1;
    }
    bPanleReady=TRUE;
    return 0;
}
#define PANEL_DEBUG 0
void cmp(PanelType *p1, PanelType *p2);

int do_panel_init( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char *pSwingLevel;
    MS_U16 u16Panel_SwingLevel;
    PanelType panelpara;
    st_board_para boardpara;
    memset(&panelpara, 0, sizeof(panelpara));
    memset(&boardpara, 0, sizeof(boardpara));
    //load panel para
    if(Read_PanelParaFromflash(&panelpara)!=0)
    {
        bPanleReady=FALSE;
	    printf("%s: Read_PanelParaFromflash() failed, at %d\n", __func__, __LINE__);
        return -1;
    }

    //load board para
    if(Read_BoardParaFromflash(&boardpara)!=0)
    {
        bPanleReady=FALSE;
	    printf("%s: Read_BoardParaFromflash() failed, at %d\n", __func__, __LINE__);
        return -1;
    }

    //panel setting by each board
    panelpara.m_bPanelPDP10BIT = boardpara.m_bPANEL_PDP_10BIT;
    panelpara.m_bPanelSwapLVDS_POL = boardpara.m_bPANEL_SWAP_LVDS_POL;
    panelpara.m_bPanelSwapLVDS_CH = boardpara.m_bPANEL_SWAP_LVDS_CH;
    panelpara.m_bPanelSwapPort ^= boardpara.m_bPANEL_CONNECTOR_SWAP_PORT;
    panelpara.m_u16LVDSTxSwapValue = (boardpara.m_u16LVDS_PN_SWAP_H << 8) + boardpara.m_u16LVDS_PN_SWAP_L;

#if (ENABLE_URSA_6M30 == 1)
    MDrv_Ursa_6M30_Initialize();
    if(bMst6m30Installed)
    {
        ursa_6m30_cmd_table cmd_table={0};
        if(Read_Ursa_6m30_Para(&cmd_table)==0)
        {
            Ursa_6M30_Setting(&cmd_table);
        }
        else
        {
           printf("read ursa_6m30 data fail ...>>>\n");
        }
		//MDrv_Ursa_6M30_VideoOutput_Mode(VIDEO_OUTPUT_MUTE);

		//MDrv_Ursa_6M30_LVDS_Enalbe(FALSE);
	    if(strcmp(getenv("game_mode"), "enable") == 0)
		{
			printf("Enable Game mode \n");
			MDrv_Ursa_6M30_3D_Cmd(UC_SET_RGBBYPASS);
		}
    }
    else
    {
           printf("ursa_6m30 installed fail ...>>>\n");
    }
#endif

#if PANEL_DEBUG
    PanelType* panel_data = NULL;
    PANEL_RESOLUTION_TYPE enPanelType;
	enPanelType = MApi_PNL_GetPnlTypeSetting();
	MApi_PNL_PreInit(E_PNL_NO_OUTPUT);
	panel_data=MApi_XC_GetPanelSpec(enPanelType);
    cmp(panel_data,&panelpara);
#endif
    //pane init
    MApi_BD_LVDS_Output_Type(boardpara.m_u16BOARD_LVDS_CONNECT_TYPE);
    if(MsAPI_PNL_Init(&panelpara)==FALSE)
    {
        bPanleReady=FALSE;
        return -1;
    }

    //set swing level
    pSwingLevel = getenv("swing_level");
    if(pSwingLevel==NULL)
    {
        bPanleReady=FALSE;
        return -1;
    }
    u16Panel_SwingLevel = simple_strtoul(pSwingLevel, NULL, 10);
    if(MApi_PNL_Control_Out_Swing(u16Panel_SwingLevel)!=TRUE)
    {
        bPanleReady=FALSE;
        return -1;
    }
    bPanleReady=TRUE;
    return 0;
}


#if PANEL_DEBUG
void cmp(PanelType *p1, PanelType *p2)
{
    if((p1 ==NULL) || (p2==NULL))
    {
        printf("null return\n");
        return;
    }
//    printf("compare: '%s', '%s'\n", p1->m_pPanelName, p2->m_pPanelName);
    if(p1->m_bPanelDither != p2->m_bPanelDither)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelDither, p2->m_bPanelDither, __LINE__);
    }
    if(p1->m_ePanelLinkType != p2->m_ePanelLinkType)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ePanelLinkType, p2->m_ePanelLinkType, __LINE__);
    }
    if(p1->m_bPanelDualPort != p2->m_bPanelDualPort)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelDualPort, p2->m_bPanelDualPort, __LINE__);
    }
    if(p1->m_bPanelSwapPort != p2->m_bPanelSwapPort)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapPort, p2->m_bPanelSwapPort, __LINE__);
    }
    if(p1->m_bPanelSwapOdd_ML != p2->m_bPanelSwapOdd_ML)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapOdd_ML, p2->m_bPanelSwapOdd_ML, __LINE__);
    }
    if(p1->m_bPanelSwapEven_ML != p2->m_bPanelSwapEven_ML)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapEven_ML, p2->m_bPanelSwapEven_ML, __LINE__);
    }
    if(p1->m_bPanelSwapOdd_RB != p2->m_bPanelSwapOdd_RB)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapOdd_RB, p2->m_bPanelSwapOdd_RB, __LINE__);
    }
    if(p1->m_bPanelSwapEven_RB != p2->m_bPanelSwapEven_RB)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapEven_RB, p2->m_bPanelSwapEven_RB, __LINE__);
    }
    if(p1->m_bPanelSwapLVDS_POL != p2->m_bPanelSwapLVDS_POL)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapLVDS_POL, p2->m_bPanelSwapLVDS_POL, __LINE__);
    }
    if(p1->m_bPanelSwapLVDS_CH != p2->m_bPanelSwapLVDS_CH)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapLVDS_CH, p2->m_bPanelSwapLVDS_CH, __LINE__);
    }
    if(p1->m_bPanelPDP10BIT != p2->m_bPanelPDP10BIT)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelPDP10BIT, p2->m_bPanelPDP10BIT, __LINE__);
    }
    if(p1->m_bPanelLVDS_TI_MODE != p2->m_bPanelLVDS_TI_MODE)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelLVDS_TI_MODE, p2->m_bPanelLVDS_TI_MODE, __LINE__);
    }
    if(p1->m_ucPanelDCLKDelay != p2->m_ucPanelDCLKDelay)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelDCLKDelay, p2->m_ucPanelDCLKDelay, __LINE__);
    }
    if(p1->m_bPanelInvDCLK != p2->m_bPanelInvDCLK)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelInvDCLK, p2->m_bPanelInvDCLK, __LINE__);
    }
    if(p1->m_bPanelInvDE != p2->m_bPanelInvDE)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelInvDE, p2->m_bPanelInvDE, __LINE__);
    }
    if(p1->m_bPanelInvHSync != p2->m_bPanelInvHSync)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelInvHSync, p2->m_bPanelInvHSync, __LINE__);
    }
    if(p1->m_bPanelInvVSync != p2->m_bPanelInvVSync)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelInvVSync, p2->m_bPanelInvVSync, __LINE__);
    }
    if(p1->m_ucPanelDCKLCurrent != p2->m_ucPanelDCKLCurrent)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelDCKLCurrent, p2->m_ucPanelDCKLCurrent, __LINE__);
    }
    if(p1->m_ucPanelDECurrent != p2->m_ucPanelDECurrent)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelDECurrent, p2->m_ucPanelDECurrent, __LINE__);
    }
    if(p1->m_ucPanelODDDataCurrent != p2->m_ucPanelODDDataCurrent)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelODDDataCurrent, p2->m_ucPanelODDDataCurrent, __LINE__);
    }
    if(p1->m_ucPanelEvenDataCurrent != p2->m_ucPanelEvenDataCurrent)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelEvenDataCurrent, p2->m_ucPanelEvenDataCurrent, __LINE__);
    }
    if(p1->m_wPanelOnTiming1 != p2->m_wPanelOnTiming1)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelOnTiming1, p2->m_wPanelOnTiming1, __LINE__);
    }
    if(p1->m_wPanelOnTiming2 != p2->m_wPanelOnTiming2)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelOnTiming2, p2->m_wPanelOnTiming2, __LINE__);
    }
    if(p1->m_wPanelOffTiming1 != p2->m_wPanelOffTiming1)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelOffTiming1, p2->m_wPanelOffTiming1, __LINE__);
    }
    if(p1->m_wPanelOffTiming2 != p2->m_wPanelOffTiming2)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelOffTiming2, p2->m_wPanelOffTiming2, __LINE__);
    }
    if(p1->m_ucPanelHSyncWidth != p2->m_ucPanelHSyncWidth)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelHSyncWidth, p2->m_ucPanelHSyncWidth, __LINE__);
    }
    if(p1->m_ucPanelHSyncBackPorch != p2->m_ucPanelHSyncBackPorch)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelHSyncBackPorch, p2->m_ucPanelHSyncBackPorch, __LINE__);
    }
    if(p1->m_ucPanelVSyncWidth != p2->m_ucPanelVSyncWidth)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelVSyncWidth, p2->m_ucPanelVSyncWidth, __LINE__);
    }
    if(p1->m_ucPanelVBackPorch != p2->m_ucPanelVBackPorch)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelVBackPorch, p2->m_ucPanelVBackPorch, __LINE__);
    }
    if(p1->m_wPanelHStart != p2->m_wPanelHStart)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelHStart, p2->m_wPanelHStart, __LINE__);
    }
    if(p1->m_wPanelVStart != p2->m_wPanelVStart)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelVStart, p2->m_wPanelVStart, __LINE__);
    }
    if(p1->m_wPanelWidth != p2->m_wPanelWidth)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelWidth, p2->m_wPanelWidth, __LINE__);
    }
    if(p1->m_wPanelHeight != p2->m_wPanelHeight)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelHeight, p2->m_wPanelHeight, __LINE__);
    }
    if(p1->m_wPanelMaxHTotal != p2->m_wPanelMaxHTotal)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelMaxHTotal, p2->m_wPanelMaxHTotal, __LINE__);
    }
    if(p1->m_wPanelHTotal != p2->m_wPanelHTotal)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelHTotal, p2->m_wPanelHTotal, __LINE__);
    }
    if(p1->m_wPanelMinHTotal != p2->m_wPanelMinHTotal)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelMinHTotal, p2->m_wPanelMinHTotal, __LINE__);
    }
    if(p1->m_wPanelMaxVTotal != p2->m_wPanelMaxVTotal)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelMaxVTotal, p2->m_wPanelMaxVTotal, __LINE__);
    }
    if(p1->m_wPanelVTotal != p2->m_wPanelVTotal)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelVTotal, p2->m_wPanelVTotal, __LINE__);
    }
    if(p1->m_wPanelMinVTotal != p2->m_wPanelMinVTotal)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wPanelMinVTotal, p2->m_wPanelMinVTotal, __LINE__);
    }
    if(p1->m_dwPanelMaxDCLK != p2->m_dwPanelMaxDCLK)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_dwPanelMaxDCLK, p2->m_dwPanelMaxDCLK, __LINE__);
    }
    if(p1->m_dwPanelDCLK != p2->m_dwPanelDCLK)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_dwPanelDCLK, p2->m_dwPanelDCLK, __LINE__);
    }
    if(p1->m_dwPanelMinDCLK != p2->m_dwPanelMinDCLK)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_dwPanelMinDCLK, p2->m_dwPanelMinDCLK, __LINE__);
    }
    if(p1->m_wSpreadSpectrumStep != p2->m_wSpreadSpectrumStep)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wSpreadSpectrumStep, p2->m_wSpreadSpectrumStep, __LINE__);
    }
    if(p1->m_wSpreadSpectrumSpan != p2->m_wSpreadSpectrumSpan)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_wSpreadSpectrumSpan, p2->m_wSpreadSpectrumSpan, __LINE__);
    }
    if(p1->m_ucDimmingCtl != p2->m_ucDimmingCtl)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucDimmingCtl, p2->m_ucDimmingCtl, __LINE__);
    }
    if(p1->m_ucMaxPWMVal != p2->m_ucMaxPWMVal)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucMaxPWMVal, p2->m_ucMaxPWMVal, __LINE__);
    }
    if(p1->m_ucMinPWMVal != p2->m_ucMinPWMVal)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucMinPWMVal, p2->m_ucMinPWMVal, __LINE__);
    }
    if(p1->m_bPanelDeinterMode != p2->m_bPanelDeinterMode)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelDeinterMode, p2->m_bPanelDeinterMode, __LINE__);
    }
    if(p1->m_ucPanelAspectRatio != p2->m_ucPanelAspectRatio)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucPanelAspectRatio, p2->m_ucPanelAspectRatio, __LINE__);
    }
    if(p1->m_u16LVDSTxSwapValue != p2->m_u16LVDSTxSwapValue)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_u16LVDSTxSwapValue, p2->m_u16LVDSTxSwapValue, __LINE__);
    }
    if(p1->m_ucTiBitMode != p2->m_ucTiBitMode)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucTiBitMode, p2->m_ucTiBitMode, __LINE__);
    }
    if(p1->m_ucOutputFormatBitMode != p2->m_ucOutputFormatBitMode)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucOutputFormatBitMode, p2->m_ucOutputFormatBitMode, __LINE__);
    }
    if(p1->m_bPanelSwapOdd_RG != p2->m_bPanelSwapOdd_RG)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapOdd_RG, p2->m_bPanelSwapOdd_RG, __LINE__);
    }
    if(p1->m_bPanelSwapEven_RG != p2->m_bPanelSwapEven_RG)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapEven_RG, p2->m_bPanelSwapEven_RG, __LINE__);
    }
    if(p1->m_bPanelSwapOdd_GB != p2->m_bPanelSwapOdd_GB)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapOdd_GB, p2->m_bPanelSwapOdd_GB, __LINE__);
    }
    if(p1->m_bPanelSwapEven_GB != p2->m_bPanelSwapEven_GB)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelSwapEven_GB, p2->m_bPanelSwapEven_GB, __LINE__);
    }
    if(p1->m_bPanelDoubleClk != p2->m_bPanelDoubleClk)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelDoubleClk, p2->m_bPanelDoubleClk, __LINE__);
    }
    if(p1->m_dwPanelMaxSET != p2->m_dwPanelMaxSET)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_dwPanelMaxSET, p2->m_dwPanelMaxSET, __LINE__);
    }
    if(p1->m_dwPanelMinSET != p2->m_dwPanelMinSET)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_dwPanelMinSET, p2->m_dwPanelMinSET, __LINE__);
    }
    if(p1->m_ucOutTimingMode != p2->m_ucOutTimingMode)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_ucOutTimingMode, p2->m_ucOutTimingMode, __LINE__);
    }
    if(p1->m_bPanelNoiseDith != p2->m_bPanelNoiseDith)
    {
        printf("diff: '%u', '%u', at %u\n", p1->m_bPanelNoiseDith, p2->m_bPanelNoiseDith, __LINE__);
    }
}
#endif //PANEL_DEBUG

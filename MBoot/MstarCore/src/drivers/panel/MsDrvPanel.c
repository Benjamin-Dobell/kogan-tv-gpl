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
#include <command.h>
#include <config.h>
#include <configs/uboot_board_config.h>
#include <MsTypes.h>

#include <MsCommon.h>
#include <MsVersion.h>

#include <apiPNL.h>
#include <drvPWM.h>
#include <drvPM.h>

#include <panel/MsDrvPanel.h>
#include <panel/Gamma12BIT_256.inc>
#include <MsApiPM.h>

#if(ENABLE_URSA_6M30 == 1)
#include <ursa/ursa_6m30.h>
#endif

#if(ENABLE_URSA_6M40 == 1)
#include <ursa/ursa_6m40.h>
#endif

#define PWM_DUTY   "u32PWMDuty"
#define PWM_PERIOD "u32PWMPeriod"
#define PWM_DIVPWM "u16DivPWM"
#define MIRROR_ON "MIRROR_ON"
#define PWM_POLPWM "bPolPWM"
#define PWM_MAXPWM "u16MaxPWMvalue"
#define PWM_MINPWM "u16MinPWMvalue"

#define LOCALDIMMING "bLocalDIMMINGEnable"

#include <bootlogo/MsPoolDB.h>
#include <MsDebug.h>

#if defined(CONFIG_A3_STB)
#include <drvTVEncoder.h>
#include <drvXC_IOPort.h>
#include <apiXC.h>
#include <apiXC_Adc.h>
#include <drvGPIO.h>
#include <apiGOP.h>


MS_BOOL mvideo_sc_is_interlace(void)
{
    return 0;
}

void verDispCvbs_Main(void)
{
    MS_Switch_VE_Dest_Info SwitchOutputDest;
    MS_VE_Output_Ctrl OutputCtrl;
    MS_VE_Set_Mode_Type SetModeType;
    //XC_INITDATA XC_InitData;
	MS_U16 u16OutputVfreq;
	XC_INITDATA XC_InitData;
	GOP_InitInfo pGopInit;
	//xc init
	memset((void *)&XC_InitData, 0, sizeof(XC_InitData));
    MApi_XC_Init(&XC_InitData, 0);
	MApi_XC_SetDispWindowColor(0x82,MAIN_WINDOW);
	memset((void *)&pGopInit, 0, sizeof(pGopInit));
	MApi_GOP_RegisterXCIsInterlaceCB(mvideo_sc_is_interlace);
	MApi_GOP_Init(&pGopInit);
	MApi_GOP_GWIN_OutputColor(GOPOUT_YUV);

    MApi_GOP_SetGOPHStart(0, g_IPanel.HStart());
    MApi_GOP_SetGOPHStart(1, g_IPanel.HStart());

	MDrv_VE_Init(VE_BUFFER_ADDR);
	MApi_XC_SetOutputCapture(ENABLE, E_XC_OP2);     // Enable op2 to ve path

	u16OutputVfreq = MApi_XC_GetOutputVFreqX100()/10;
    if(u16OutputVfreq > 550)
    {
        //60 hz for NTSC
        MDrv_VE_SetOutputVideoStd(MS_VE_NTSC);
		SetModeType.u16InputVFreq = (3000 * 2) / 10;
    }
    else
    {
        //50 hz for PAL
        MDrv_VE_SetOutputVideoStd(MS_VE_PAL);
		SetModeType.u16InputVFreq = (2500 * 2) / 10;
    }


    MS_Switch_VE_Src_Info SwitchInputSrc;
    SwitchInputSrc.InputSrcType = MS_VE_SRC_SCALER;
    MDrv_VE_SwitchInputSource(&SwitchInputSrc);
	MDrv_VE_SetRGBIn(false);

    SwitchOutputDest.OutputDstType = MS_VE_DEST_CVBS;
    MDrv_VE_SwitchOuputDest(&SwitchOutputDest);

    SetModeType.u16H_CapSize     = g_IPanel.Width();
    SetModeType.u16V_CapSize     = g_IPanel.Height();
    SetModeType.u16H_CapStart    = g_IPanel.HStart();
    SetModeType.u16V_CapStart    = g_IPanel.VStart();
    SetModeType.u16H_SC_CapSize  = g_IPanel.Width();
    SetModeType.u16V_SC_CapSize  = g_IPanel.Height();
    SetModeType.u16H_SC_CapStart = g_IPanel.HStart();
    SetModeType.u16V_SC_CapStart = g_IPanel.VStart();

	SetModeType.bHDuplicate = FALSE;
	SetModeType.bSrcInterlace = FALSE;

    MDrv_VE_SetMode(&SetModeType);

    OutputCtrl.bEnable = TRUE;
    OutputCtrl.OutputType = MS_VE_OUT_TVENCODER;

    MDrv_VE_SetOutputCtrl(&OutputCtrl);
	MDrv_VE_SetBlackScreen(FALSE);
	MApi_XC_ADC_SetCVBSOut(ENABLE, OUTPUT_CVBS1, INPUT_SOURCE_DTV, TRUE);


}
#endif

#define LVDS_OUTPUT_USER          4
#if(ENABLE_URSA_6M40 == 1)
#define PANEL_4K2K_ENABLE         0
#endif
#if (CONFIG_PANEL_INIT)

void SetPWM(U16 Period,U16 Duty,U16 PwmDiv)
{
    PWM_ChNum PWMPort = (PWM_ChNum)E_PWM_CH2;
    PWM_Result result = E_PWM_FAIL;

    U16 bPolPWM = 0;
    char *p_str=NULL;


    result = MDrv_PWM_Init(E_PWM_DBGLV_NONE);
    if(result == E_PWM_FAIL)
    {
        UBOOT_DEBUG("Sorry , init failed!~\n");
        return ;
    }
	p_str = getenv(PWM_POLPWM);
		if(p_str == NULL)
		{
			UBOOT_DEBUG("PWM period in env is NULL,so use default value 0xC350\n");
			setenv(PWM_POLPWM,"0x1");
		}
		else
		{
			bPolPWM = simple_strtol(p_str, NULL, 16);
		}
		UBOOT_DEBUG("\nbPolPWM_T = %d\n",bPolPWM);

    MDrv_PWM_UnitDiv(0);
    MDrv_PWM_Oen(PWMPort, 0);    /* Set 0 for output enable */
    MDrv_PWM_Period(PWMPort, Period);
    MDrv_PWM_DutyCycle(PWMPort, Duty);
    MDrv_PWM_Div(PWMPort, PwmDiv);
	if(bPolPWM==1)
    MDrv_PWM_Polarity(PWMPort, TRUE);
	else
    MDrv_PWM_Polarity(PWMPort, FALSE);
    MDrv_PWM_Vdben(PWMPort, FALSE);
    MDrv_PWM_Dben(PWMPort,FALSE);
}

int PWM_init(void)
{
    U16 u32PWMDuty = 0;
    U16 u32PWMPeriod = 0;
    U16 u32PWMDIV = 0;
	U16 u16maxPWM =0;
	U16 u16minPWM =0;
    char *p_str=NULL;
    // printf("getenv duty[%s]\n",getenv(PWM_DUTY));
    // printf("getenv period[%s]\n",getenv(PWM_PERIOD));
    p_str = getenv(PWM_DIVPWM);
    if(p_str == NULL)
    {
        UBOOT_DEBUG("PWM period in env is NULL,so use default value 0xC350\n");
        setenv(PWM_DIVPWM,"0x1FF");
    }
    else
    {
        u32PWMDIV = simple_strtol(p_str, NULL, 16);
    }

    p_str = getenv(PWM_PERIOD);
    if(p_str == NULL)
    {
        UBOOT_DEBUG("PWM period in env is NULL,so use default value 0xC350\n");
        setenv(PWM_PERIOD,"0xC350");
    }
    else
    {
        u32PWMPeriod = simple_strtol(p_str, NULL, 16);
    }

	p_str = getenv(PWM_MAXPWM);
	if(p_str == NULL)
	{
		UBOOT_DEBUG("PWM PWM_MAXPWM in env is NULL,so use default value 0xC350\n");
		setenv(PWM_PERIOD,"0xC350");
	}
	else
	{
		u16maxPWM = simple_strtol(p_str, NULL, 16);
	}
	p_str = getenv(PWM_MINPWM);
	if(p_str == NULL)
	{
		UBOOT_DEBUG("PWM PWM_MAXPWM in env is NULL,so use default value 0xC350\n");
		setenv(PWM_PERIOD,"0xC350");
	}
	else
	{
		u16minPWM = simple_strtol(p_str, NULL, 16);
	}
	//  For STR Backlight Mutation
	p_str = getenv(PWM_DUTY);
	if(p_str == NULL)
	{
		u32PWMDuty = 1008;
	}
	else
	{
		u32PWMDuty = simple_strtol(p_str, NULL, 10);
	}


    //aaron test u32PWMDuty = (u16maxPWM+u16minPWM)/2;
    //u32PWMDuty = u16maxPWM;

    UBOOT_DEBUG("u32PWMPeriod_T = %d\n",u32PWMPeriod);
    UBOOT_DEBUG("u32PWMDuty_T = %d\n",u32PWMDuty);
    UBOOT_DEBUG("u32PWMDIV_T = %d\n",u32PWMDIV);
    UBOOT_DEBUG("u16maxPWM_T = %d\n",u16maxPWM);
    UBOOT_DEBUG("u16minPWM_T = %d\n",u16minPWM);
    printf("u32PWMPeriod_T = %d,u32PWMDuty_T = %d,u32PWMDIV_T = %d\n",u32PWMPeriod,u32PWMDuty,u32PWMDIV);

    SetPWM(u32PWMPeriod,u32PWMDuty, u32PWMDIV);
    return 0;
}


#if (ENABLE_TCON_PANEL == 1)
#include <MsOS.h>
#include <drvXC_IOPort.h>
#include <apiXC.h>
#include <panel/pnl_tcon_tbl.h>
#include <panel/pnl_tcon_tbl.c>


E_TCON_TAB_TYPE eTCONPNL_TypeSel = TCON_TABTYPE_GENERAL;
static void _MApi_TCon_Tab_DumpPSRegTab(MS_U8* pu8TconTab)
{
    #define TCON_DUMP_TIMEOUT_TIME  1000
    MS_U32 u32tabIdx = 0;
    MS_U32 u32Addr;
    MS_U16 u16Mask;
    MS_U16 u16Value;
    MS_U8 u8NeedDelay;
    MS_U8 u8DelayTime;
    MS_U32 u32StartTime = MsOS_GetSystemTime();
    UBOOT_TRACE("IN\n");


    if (pu8TconTab == NULL)
    {
        UBOOT_ERROR("pu8TconTab is NULL! at %s %u \n", __FUNCTION__, __LINE__);
        return;
    }

    while(1)
    {
        if( (MsOS_GetSystemTime() - u32StartTime) > TCON_DUMP_TIMEOUT_TIME )
        {
            UBOOT_ERROR("Dump tcon tab timeout! at %s %u \n", __FUNCTION__, __LINE__);
            return;
        }

        u32Addr     = ((pu8TconTab[u32tabIdx]<<8) + pu8TconTab[(u32tabIdx +1)]) & 0xFFFF;
        u16Mask     = pu8TconTab[(u32tabIdx +2)] & 0xFF;
        u16Value    = pu8TconTab[(u32tabIdx +3)] & 0xFF;
        u8NeedDelay = pu8TconTab[(u32tabIdx +4)] & 0xFF;
        u8DelayTime = pu8TconTab[(u32tabIdx +5)] & 0xFF;

        if (u32Addr == REG_TABLE_END) // check end of table
            break;

        u32Addr = (u32Addr | 0x100000);

        if( u32Addr == REG_TCON_BASE )
        {
            UBOOT_DEBUG("Wait V sync \n");
            MApi_XC_WaitOutputVSync(1, 50, MAIN_WINDOW);
        }

        if (u32Addr & 0x1)
        {
            u32Addr --;
            //temp_mask, need check!! MApi_PNL_Write2ByteMask(u32Addr, (u16Value << 8), (u16Mask << 8));//MApi_XC_Write2ByteMask
            MApi_XC_Write2ByteMask(u32Addr, (u16Value << 8), (u16Mask << 8));
            UBOOT_DEBUG("[Odd .addr=%04lx, msk=%02x, val=%02x] \n", u32Addr, (u16Mask << 8), (u16Value << 8));
        }
        else
        {
            MApi_XC_Write2ByteMask(u32Addr, u16Value, u16Mask);
            //temp_mask, need check!! MApi_PNL_Write2ByteMask(u32Addr, u16Value, u16Mask);//MApi_XC_Write2ByteMask
            UBOOT_DEBUG("[Even.addr=%04lx, msk=%02x, val=%02x] \n", u32Addr, u16Mask, u16Value);
        }
        // Check need delay?
        if( u8NeedDelay && u8DelayTime )
        {
            MsOS_DelayTask(u8DelayTime);
        }
        u32tabIdx = u32tabIdx + 7;
    }
    UBOOT_TRACE("OK\n");
}


static void _MApi_XC_Sys_Init_TCON_Panel(TCON_TAB_INFO* pTcontab)
{
    // TCON init
    MApi_PNL_TCON_Init();

    // dump TCON general register tab
    MApi_PNL_TCONMAP_DumpTable(pTcontab->pTConInitTab, E_APIPNL_TCON_TAB_TYPE_GENERAL);

    // dump TCON mod register tab
    MApi_PNL_TCONMAP_DumpTable(pTcontab->pTConInit_MODTab, E_APIPNL_TCON_TAB_TYPE_MOD);

    // dump TCON GPIO register tab
    MApi_PNL_TCONMAP_DumpTable(pTcontab->pTConInit_GPIOTab, E_APIPNL_TCON_TAB_TYPE_GPIO);

    // dump TCON gamma register tab
    #if 0
    {
        _MApi_XC_Sys_Init_TCON_GAMMA(pTcontab->pTConGammaTab);
    }
    #endif

    _MApi_TCon_Tab_DumpPSRegTab(pTcontab->pTConPower_Sequence_OnTab);

}

static void Init_TCON_Panel(void)
{
    UBOOT_TRACE("IN\n");
    TCON_TAB_INFO* pTcontab = &(TConMAP_Main[eTCONPNL_TypeSel]);
    _MApi_XC_Sys_Init_TCON_Panel(pTcontab);
    UBOOT_TRACE("OK\n");
}


static void PNL_SetPanleExtType(APIPNL_LINK_TYPE PanelLinkType)
{

    APIPNL_LINK_EXT_TYPE ePanelExtLinkType = LINK_MINILVDS_2CH_6P_8BIT;
    UBOOT_TRACE("IN\n");

    if(LINK_EXT == PanelLinkType)
    {
        ePanelExtLinkType = LINK_MINILVDS_2CH_6P_8BIT;//config it in mode.ini
    }

    UBOOT_DEBUG("%s, %d, u16PanelLinkType =%u\n", __FUNCTION__, __LINE__, ePanelExtLinkType);
    MApi_PNL_SetLPLLTypeExt(ePanelExtLinkType);
    UBOOT_TRACE("OK\n");
}
#endif

#if (CONFIG_3D_HWLVDSLRFLAG)
static void PNL_Set3D_HWLVDSLRFlag(void)
{
    MS_PNL_HW_LVDSResInfo lvdsresinfo;
    lvdsresinfo.bEnable = 1;
    lvdsresinfo.u16channel = 0x03; // channel A: BIT0, channel B: BIT1,
    lvdsresinfo.u32pair = 0x18; // pair 0: BIT0, pair 1: BIT1, pair 2: BIT2, pair 3: BIT3, pair 4: BIT4, etc ...

    MApi_PNL_HWLVDSReservedtoLRFlag(lvdsresinfo);
}
#endif

MS_BOOL MsDrv_PNL_Init(PanelType*  panel_data)
{
	MS_BOOL ret=TRUE;

	MS_U8 u8IsIwwat = 0;
    char *p_str=NULL;
    // printf("getenv duty[%s]\n",getenv(PWM_DUTY));
    // printf("getenv period[%s]\n",getenv(PWM_PERIOD));
    p_str = getenv(LOCALDIMMING);
    if(p_str == NULL)
    {
        UBOOT_DEBUG("bLocalDIMMINGEnable in env is NULL,so use default value 0\n");
        setenv(LOCALDIMMING,"0");
    }
    else
    {
        u8IsIwwat = simple_strtol(p_str, NULL, 10);
    }
	
    UBOOT_TRACE("IN\n");

#if(ENABLE_URSA_6M40 == 1)
	MDrv_Ursa_6M40_Set_Osd_Off();
#endif
	MApi_PNL_PreInit(E_PNL_NO_OUTPUT);

	MApi_PNL_SkipTimingChange(FALSE);

#if (ENABLE_TCON_PANEL == 1)
        if(LINK_LVDS != panel_data->m_ePanelLinkType)
        {
            PNL_SetPanleExtType(panel_data->m_ePanelLinkType);
        }
#else
    if (panel_data->m_ePanelLinkType == LINK_TTL)
    {
        MApi_BD_LVDS_Output_Type(LVDS_OUTPUT_USER);
        MApi_PNL_MOD_OutputConfig_User(0, 0, 0);
    }
    else if(panel_data->m_ePanelLinkType == LINK_EXT)
    {
        MApi_PNL_SetLPLLTypeExt(LINK_HS_LVDS);
        MApi_PNL_PreInit(E_PNL_CLK_DATA);
        MApi_BD_LVDS_Output_Type(3); // LVDS_SINGLE_OUTPUT_B
    }

#endif
	MApi_PNL_Init(panel_data);
#if defined(CONFIG_SET_4K2K_MODE)
    MApi_PNL_EnableInternalTermination(TRUE);
#else
    MApi_PNL_EnableInternalTermination(FALSE);
#endif


        g_IPanel.Dump();
        g_IPanel.SetGammaTbl(E_APIPNL_GAMMA_12BIT, tAllGammaTab, GAMMA_MAPPING_MODE);
#if(ENABLE_URSA_6M30 == 1)
		#if(ENABLE_URSA_6M40 == 1)
        if(PANEL_4K2K_ENABLE == 1)
            MDrv_WriteByte(0x1032B5, 0xF0);  //LR Flag Send From LVDS Dummy Pixel
		#endif

        if(bMst6m30Installed)
        {
            PWM_init();
            ret=MApi_PNL_PreInit(E_PNL_CLK_DATA);
            MApi_PNL_SetOutput(E_APIPNL_OUTPUT_CLK_DATA);
			printf("\n delay [%d]ms \n",panel_data->m_wPanelOnTiming2);
            MsOS_DelayTask(panel_data->m_wPanelOnTiming2);
            g_IPanel.Enable(TRUE);
			MApi_PNL_On(panel_data->m_wPanelOnTiming2);//Delay Timming2
            MDrv_Ursa_6M30_LVDS_Enalbe(TRUE);//run_command("ursa_lvds_on", 0);
            
            #if (CONFIG_ACTIVE_STANDBY == 1)
                if(FALSE == pm_check_back_ground_active())
                {
                    MApi_PNL_SetBackLight(BACKLITE_INIT_SETTING);//Power on backlight
                }
                else
                {
                    MApi_PNL_SetBackLight(DISABLE);
                }
            #else
            unsigned int  u8IsSkipInitPanel = 0;
    		if (MDrv_PM_GetWakeupSource() == E_PM_WAKEUPSRC_RTC2)
   			{
        		u8IsSkipInitPanel = 1;
			}
			if(u8IsSkipInitPanel == 0)
			{
               MApi_PNL_SetBackLight(BACKLITE_INIT_SETTING);//Power on backlight
			}
			else
			{
				printf("jump  set backlight\n");
			}

            #endif
        }
        else
#endif
        {
			if(MDrv_PM_GetWakeupSource() != E_PM_WAKEUPSRC_RTC2)
			{
            MApi_PNL_On(0);
			}
            //ret=MApi_PNL_PreInit(E_PNL_CLK_DATA);
            //MApi_PNL_SetOutput(E_APIPNL_OUTPUT_CLK_DATA);
            PWM_init();
			//printf("\r\n XXXXX=========>>> fun:[%s],line:[%d],u8IsIwwat[%d] \r\n",__FUNCTION__,__LINE__,u8IsIwwat);
            unsigned int  u8IsSkipInitPanel = 0;
    		if (MDrv_PM_GetWakeupSource() == E_PM_WAKEUPSRC_RTC2)
   			{
        		u8IsSkipInitPanel = 1;
			}

			if(u8IsIwwat)
			{
	            MsOS_DelayTask(20);//Delay for PWM
	            
				#if (CONFIG_ACTIVE_STANDBY == 1)
	            if(FALSE == pm_check_back_ground_active())
	            {
	                MApi_PNL_SetBackLight(BACKLITE_INIT_SETTING);//Power on backlight
	            }
	            else
	            {
	                MApi_PNL_SetBackLight(DISABLE);
	            }
	            #else
				if(u8IsSkipInitPanel == 0)
				{
					//printf("\r\n XXXXX=========>>> fun:[%s],line:[%d],u8IsIwwat[%d] \r\n",__FUNCTION__,__LINE__,u8IsIwwat);
	                MApi_PNL_SetBackLight(BACKLITE_INIT_SETTING);//Power on backlight
	            	MsOS_DelayTask(2000);//Delay 2s for iWatt
	            	MApi_PNL_En(TRUE, panel_data->m_wPanelOnTiming1);//Power on TCON and Delay Timming1
				}
				else
				{
	            	MApi_PNL_En(FALSE, panel_data->m_wPanelOnTiming1);
				}
	            #endif
				
	            //MsOS_DelayTask(2000);//Delay 2s for iWatt
				//printf("\r\n XXXXX=========>>> fun:[%s],line:[%d],u8IsIwwat[%d] \r\n",__FUNCTION__,__LINE__,u8IsIwwat);
	            
	            //MApi_PNL_En(TRUE, panel_data->m_wPanelOnTiming1);//Power on TCON and Delay Timming1
	            MsOS_DelayTask(panel_data->m_wPanelOnTiming2);//Delay Timming2
			}
			else
			{
	            MsOS_DelayTask(20);//Delay for PWM
	            MApi_PNL_En(TRUE, panel_data->m_wPanelOnTiming1);//Power on TCON and Delay Timming1
	            MsOS_DelayTask(panel_data->m_wPanelOnTiming2);//Delay Timming2
				#if (CONFIG_ACTIVE_STANDBY == 1)
	            if(FALSE == pm_check_back_ground_active())
	            {
	                MApi_PNL_SetBackLight(BACKLITE_INIT_SETTING);//Power on backlight
	            }
	            else
	            {
	                MApi_PNL_SetBackLight(DISABLE);
	            }
	            #else			
				if(u8IsSkipInitPanel == 0)
				{
	                MApi_PNL_SetBackLight(BACKLITE_INIT_SETTING);//Power on backlight
				}
	            #endif
			}

        #if (ENABLE_TCON_PANEL == 1)
            if(MApi_XC_Init(NULL, 0) == TRUE)
            {
                //Fix back porch for TCON
                XC_PANEL_INFO_EX stPanelInfoEx;
                memset(&stPanelInfoEx, 0, sizeof(XC_PANEL_INFO_EX));
                stPanelInfoEx.u32PanelInfoEx_Version = PANEL_INFO_EX_VERSION;
                stPanelInfoEx.u16PanelInfoEX_Length = sizeof(XC_PANEL_INFO_EX);
                stPanelInfoEx.bVSyncBackPorchValid = TRUE;
                stPanelInfoEx.u16VSyncBackPorch = panel_data->m_ucPanelVBackPorch; //set back porch value
                stPanelInfoEx.u16VFreq = 500; //this step setting info is only for 50hz
                if(MApi_XC_SetExPanelInfo(TRUE, &stPanelInfoEx))//Check the set is accepted or not
                    UBOOT_DEBUG("---%s:%d Set ExPanel Info OK\n", __FUNCTION__, __LINE__);
                else
                    UBOOT_ERROR("---%s:%d Set ExPanel Info Fail\n", __FUNCTION__, __LINE__);

                stPanelInfoEx.u16VFreq = 600; //set same setting for 60hz
                if(MApi_XC_SetExPanelInfo(TRUE, &stPanelInfoEx))//Check the set is accepted or not
                    UBOOT_DEBUG("---%s:%d Set ExPanel Info OK\n", __FUNCTION__, __LINE__);
                else
                    UBOOT_ERROR("---%s:%d Set ExPanel Info Fail\n", __FUNCTION__, __LINE__);

                Init_TCON_Panel();
            }
            else
            {
                UBOOT_DEBUG("xc Init failed....\n");
            }
    #endif



        }

#if defined(CONFIG_A3_STB)
    verDispCvbs_Main();
#endif

    #if (CONFIG_3D_HWLVDSLRFLAG)
    PNL_Set3D_HWLVDSLRFlag();
    #endif
    UBOOT_TRACE("OK\n");

	return ret;

}


#endif


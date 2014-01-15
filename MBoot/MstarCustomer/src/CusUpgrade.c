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
#include <common.h>
#include <MsTypes.h>
#include <CusUpgrade.h>
#include <CusUpgradeUtility.h>
#include <MsUpgrade.h>
#include <MsUpgradeUtility.h>
#include <MsSystem.h>
#include <MsUtility.h>
#include <MsVfs.h>
#include <MsDebug.h>
#if (ENABLE_MODULE_USB == 1)
#include <usb.h>
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define GWIN_WIDTH              720
#define GWIN_HEIGHT             576
#define GRAPHIC_WIDTH           600
#define GRAPHIC_HEIGHT          400
#define GRAPHIC_X               60
#define GRAPHIC_Y               88
#define LINE_HEIGHT             50
#define RECT_LEFT_INTERVAL      50

#define OAD_BIN_PATH            "/MstarUpgrade.bin"
#define NET_BIN_PATH            "/MstarUpgrade.bin"
#define AP_BIN_PATH             "/MstarUpgrade.bin"
#define MBOOT_BIN_PATH          "/MbootUpgrade.bin"
#define SBOOT_BIN_PATH          "/SbootUpgrade.bin"
#define URSA_BIN_PATH           "/UrsaUpgrade.bin"



#define UPGRADE_STATE_URSA_BIN            (BIT0)
#define UPGRADE_STATE_SBOOT_BIN           (BIT1)
#define UPGRADE_STATE_MBOOT_BIN           (BIT2)
#define UPGRADE_STATE_AP_BIN              (BIT3)


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#if (CONFIG_DISPLAY_OSD)
static const U32 progress_max_value = 1000;
static U32 progress_cnt = 0;
static U32 progress_start_load_value = 10;
static U32 progress_finish_load_value = 200;
#endif

//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------
extern int snprintf(char *str, size_t size, const char *fmt, ...);

//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------
#if (ENABLE_MODULE_USB == 1)
static int check_bin_flag(void);
#endif

#if (((ENABLE_MODULE_USB == 1)||(ENABLE_MODULE_NETUPDATE == 1)||(ENABLE_MODULE_OAD == 1))&&(ENABLE_MODULE_DISPLAY_OSD==1))
static int show_LoadData(int var);
static int show_StartUpgrading(int var);
static int show_Upgrading(int current_cnt, int total_cnt);
static int show_Error(int var);
static int show_Finish(int var);
#endif

//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
#if (((ENABLE_MODULE_USB == 1)||(ENABLE_MODULE_NETUPDATE == 1)||(ENABLE_MODULE_OAD == 1))&&(ENABLE_MODULE_DISPLAY_OSD==1))
static int show_LoadData(int var)
{

    char buffer[CMD_BUF]="\0";
    UBOOT_TRACE("IN\n");        
    progress_cnt = 0;
    progress_start_load_value = 10;
    progress_finish_load_value = 200;    

    snprintf(buffer, CMD_BUF, "osd_create %d %d", GWIN_WIDTH, GWIN_HEIGHT);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_rect %d %d %d %d 0x800000ff", GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_string %d %d 0x3fffffff 1 LOADING DATA...", GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 2);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_progress %d %d 0x3fffffff %d", GRAPHIC_X + RECT_LEFT_INTERVAL, GRAPHIC_Y + LINE_HEIGHT * 5, progress_start_load_value/10);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "osd_flush");
    run_command(buffer, 0);
    
    UBOOT_TRACE("OK\n");        
    return 0;     
}

static int show_StartUpgrading(int var)
{
    char buffer[CMD_BUF]="\0";
    UBOOT_TRACE("IN\n");    
    snprintf(buffer, CMD_BUF, "draw_rect %d %d %d %d 0x800000ff", GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_string %d %d 0x3fffffff 1 UPGRADING SOFTWARE", GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 2);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_string %d %d 0x3fffffff 1 PLEASE DO NOT TURN OFF", GRAPHIC_X,  GRAPHIC_Y + LINE_HEIGHT * 3);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_progress %d %d 0x3fffffff %d", GRAPHIC_X + RECT_LEFT_INTERVAL, GRAPHIC_Y + LINE_HEIGHT * 5, progress_finish_load_value/10);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "osd_flush");
    run_command(buffer, 0);
    UBOOT_TRACE("OK\n");
    return 0;     
}

static int show_Upgrading(int current_cnt, int total_cnt)
{
    char buffer[CMD_BUF]="\0";
    UBOOT_TRACE("IN\n");
    progress_cnt = (((progress_max_value - progress_finish_load_value)*1000)/total_cnt); //(1000-200)/cmd_cnt
    progress_cnt = (progress_cnt *current_cnt+progress_finish_load_value*1000)/(10*1000);
    printf("--%d %%\n", progress_cnt);

    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_rect %d %d %d %d 0x800000ff", GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 5, GRAPHIC_WIDTH, LINE_HEIGHT);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_progress %d %d 0x3fffffff %d", GRAPHIC_X + RECT_LEFT_INTERVAL,GRAPHIC_Y + LINE_HEIGHT * 5, progress_cnt);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "osd_flush");
    run_command(buffer, 0);
    

       
   
    UBOOT_TRACE("OK\n");

    return 0;
}

static int show_Error(int var)
{
    char buffer[CMD_BUF]="\0";
    UBOOT_TRACE("IN\n");
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_rect %d %d %d %d 0x800000ff", GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_string %d %d 0x3fffffff 1 SOFTWARE UPGRADE ERROR", GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "osd_flush");
    run_command(buffer, 0);
    UBOOT_TRACE("OK\n");
    return 0;    
}

static int show_Finish(int var)
{
    char buffer[CMD_BUF]="\0";
    UBOOT_TRACE("IN\n");
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_rect %d %d %d %d 0x800000ff", GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_string %d %d 0x3fffffff 1 SOFTWARE UPGRADE COMPLETE", GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 2);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "draw_progress %d %d 0x3fffffff %d", GRAPHIC_X + RECT_LEFT_INTERVAL, GRAPHIC_Y + LINE_HEIGHT * 5, 100);
    run_command(buffer, 0);
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "osd_flush");
    run_command(buffer, 0);
    udelay(2000000);
    UBOOT_TRACE("OK\n");
    return 0;    
}
#endif


#if (ENABLE_MODULE_USB == 1)
static int check_bin_flag(void)
{
    int stage = 0;
    UBOOT_TRACE("IN\n");

    if(check_file_partition(AP_BIN_PATH,NULL,NULL)==0)//check Supernova file
    {
      stage |= UPGRADE_STATE_AP_BIN;
      UBOOT_DEBUG("has app upgrade bin\n");
    }

    if(check_file_partition(SBOOT_BIN_PATH,NULL,NULL)==0)//check sboot file
    {
      stage |= UPGRADE_STATE_SBOOT_BIN;
      UBOOT_DEBUG("has sboot upgrade bin\n");      
    }

    if(check_file_partition(MBOOT_BIN_PATH,NULL,NULL)==0)//check mboot file
    {
      stage |= UPGRADE_STATE_MBOOT_BIN;
      UBOOT_DEBUG("has mboot upgrade bin\n");            
    }

    if(check_file_partition(URSA_BIN_PATH,NULL,NULL)==0)//check ursa file
    {
      stage |= UPGRADE_STATE_URSA_BIN;
      UBOOT_DEBUG("has ursa upgrade bin\n");                  
    }
    
    UBOOT_TRACE("OK\n");

	return stage;
}

int do_custar(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

    char cmd[128];
    char upgrade_state_flag[] = "0x00";
    char* env_state = NULL;
    int state=0;
    UBOOT_TRACE("IN\n");
    //usb init
    if (-1 == appInitUsbDisk())
    {
       printf("FAIL : can not init usb!! \n");
       return -1;
    }
    #if (ENABLE_MODULE_DISPLAY_OSD==1)   
    UnRegisterShowCb();
    RegisterCBShowError((show_Error_cb)show_Error);
    RegisterCBShowFinish((show_Finish_cb)show_Finish);
    RegisterCBShowLoadData((show_LoadData_cb)show_LoadData);
    RegisterCBShowStartUpgrading((show_StartUpgrading_cb)show_StartUpgrading);
    RegisterCBShowUpgrading((show_Upgrading_cb)show_Upgrading);
    #endif
    //check upgrade file content
    env_state = getenv(ENV_FORCEUPGRADE);
    if(env_state != NULL )
    {
        printf("mingwu_mboot---------do_forceupgrade-->env_state != NULL\n");
         state = (int)simple_strtoul(env_state, NULL, 16);
    }
    else
    {
        printf("mingwu_mboot---------do_forceupgrade-->check_bin_flag\n");
        state=check_bin_flag();
        if(state == 0 )
        {
          UBOOT_INFO("warn!! no upgrade file in usb. \n");
          UBOOT_TRACE("OK\n");
          return 0;
        }
        snprintf(upgrade_state_flag, sizeof(upgrade_state_flag)-1, "0x%02x",state);
        setenv(ENV_FORCEUPGRADE,upgrade_state_flag);
        saveenv();
    }

    UBOOT_INFO("Current state ->[0x%x]\n",state);

    if(state&UPGRADE_STATE_URSA_BIN) // upgrade Ursa
    {
        printf("mingwu_mboot---------do_forceupgrade-->UPGRADE_STATE_URSA_BIN\n");
        memset(cmd, 0, 128);
        snprintf(cmd, sizeof(cmd)-1, "ustar %s",URSA_BIN_PATH);
        UBOOT_DEBUG("cmd=%s\n",cmd);
        if(-1 == run_command(cmd, 0))
        {
            UBOOT_ERROR("Upgrade Ursa error!\n");
            setenv(ENV_FORCEUPGRADE,NULL);
            return -1;
        }
        state = state&(~UPGRADE_STATE_URSA_BIN);
        UBOOT_INFO("Ursa upgrade OK!~\n");
    }

    else if(state&UPGRADE_STATE_SBOOT_BIN)//upgrade Sboot
    {
        printf("mingwu_mboot---------do_forceupgrade-->UPGRADE_STATE_SBOOT_BIN\n");
        memset(cmd, 0, 128);
        snprintf(cmd, sizeof(cmd)-1, "ustar %s",SBOOT_BIN_PATH);
        UBOOT_DEBUG("cmd=%s\n",cmd);        
        if(-1 == run_command(cmd, 0))
        {
            UBOOT_ERROR("Upgrade Sboot error!\n");
            setenv(ENV_FORCEUPGRADE,NULL);
            return -1;
        }
        state = state&(~UPGRADE_STATE_SBOOT_BIN);
        UBOOT_INFO("Sboot upgrade OK!~\n");
    }

    else if(state&UPGRADE_STATE_MBOOT_BIN)//upgrade Mboot
    {
        printf("mingwu_mboot---------do_forceupgrade-->UPGRADE_STATE_MBOOT_BIN\n");
        memset(cmd, 0, 128);
        snprintf(cmd, sizeof(cmd)-1, "ustar %s",MBOOT_BIN_PATH);
        UBOOT_DEBUG("cmd=%s\n",cmd);
        if(-1 == run_command(cmd, 0))
        {
            UBOOT_ERROR("Upgrade Mboot error!\n");
            setenv(ENV_FORCEUPGRADE,NULL);
            return -1;
        }
        state = state&(~UPGRADE_STATE_MBOOT_BIN);
        UBOOT_INFO("Mboot upgrade OK!~\n");
    }
    else if(state&UPGRADE_STATE_AP_BIN)//upgrade Supernova
    {
        printf("mingwu_mboot---------do_forceupgrade-->UPGRADE_STATE_AP_BIN\n");
        memset(cmd, 0, 128);
        snprintf(cmd, sizeof(cmd)-1, "ustar %s",AP_BIN_PATH);
        UBOOT_DEBUG("cmd=%s\n",cmd);
        if(-1 == run_command(cmd, 0))
        {
            UBOOT_ERROR("Upgrade Application error!\n");
            setenv(ENV_FORCEUPGRADE,NULL);
            return -1;
        }
        state = state&(~UPGRADE_STATE_AP_BIN);
        UBOOT_INFO("AP upgrade OK!~\n");
    }

    if(state == 0)
    {
        UBOOT_INFO("All Upgrade File Had Finished !!\n");
        setenv(ENV_FORCEUPGRADE,NULL);
    }
    else
    {
        snprintf(upgrade_state_flag, sizeof(upgrade_state_flag)-1, "0x%02x",state);
        UBOOT_DEBUG("upgrade_state_flag=%s\n",upgrade_state_flag);
        setenv(ENV_FORCEUPGRADE,upgrade_state_flag);
    }

    saveenv();
    UBOOT_TRACE("OK\n");
    run_command("reset",0);
    return 0;
}
#endif

#if (ENABLE_MODULE_OAD == 1)
int do_costar(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char buffer[CMD_BUF] = "\0"; 
    int ret=0;
    
    UBOOT_TRACE("IN\n");
    #if (ENABLE_MODULE_DISPLAY_OSD==1)   
    UnRegisterShowCb();
    RegisterCBShowError((show_Error_cb)show_Error);
    RegisterCBShowFinish((show_Finish_cb)show_Finish);
    RegisterCBShowLoadData((show_LoadData_cb)show_LoadData);
    RegisterCBShowStartUpgrading((show_StartUpgrading_cb)show_StartUpgrading);
    RegisterCBShowUpgrading((show_Upgrading_cb)show_Upgrading);
    #endif
    snprintf(buffer,CMD_BUF,"ostar %s",OAD_BIN_PATH);
    UBOOT_DEBUG("cmd=%s\n",buffer);
    ret=run_command(buffer,0);
    if(ret==-1)
    {
        UBOOT_ERROR("do ostar fail\n");
        return -1;
    }
    
    UBOOT_TRACE("OK\n");
    return 0;
}
#endif

#if (ENABLE_MODULE_NETUPDATE == 1)
int do_cnstar(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char buffer[CMD_BUF] = "\0"; 
    int ret=0;
    
    UBOOT_TRACE("IN\n");

    #if (ENABLE_MODULE_DISPLAY_OSD==1)
    UnRegisterShowCb();
    RegisterCBShowError((show_Error_cb)show_Error);
    RegisterCBShowFinish((show_Finish_cb)show_Finish);
    RegisterCBShowLoadData((show_LoadData_cb)show_LoadData);
    RegisterCBShowStartUpgrading((show_StartUpgrading_cb)show_StartUpgrading);
    RegisterCBShowUpgrading((show_Upgrading_cb)show_Upgrading);
    #endif
    snprintf(buffer,CMD_BUF,"nstar %s",NET_BIN_PATH);
    UBOOT_DEBUG("cmd=%s\n",buffer);
    ret=run_command(buffer,0);
    if(ret==-1)
    {
        UBOOT_ERROR("do nstar fail\n");
        return -1;
    }
    
    UBOOT_TRACE("OK\n");
    return 0;
}


#endif


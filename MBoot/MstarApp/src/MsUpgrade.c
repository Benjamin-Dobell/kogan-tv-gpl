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
#include <MsUpgrade.h>
#include <MsUpgradeUtility.h>
#include <MsUtility.h>
#include <config.h>
#include <MsTypes.h>
#include <ShareType.h>
#include <MsVfs.h>
#include <CusOsd.h>
#include <MsString.h>
#include <MsDebug.h>
#include <MsSystem.h>
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define SCRIPT_BUF_FOR_DO_MSTAR 4096
#define DEFAULT_BIN_PATH             "/MstarUpgrade.bin"
#define DEFAULT_SCRIPT_FILE_NAME    "auto_update.txt"   // script file name
#define DEFAULT_DEBUG_SCRIPT_PATH             "/MstarDebug.script"

#if (ENABLE_MODULE_OAD == 1) || (ENABLE_MODULE_NETUPDATE == 1)
#define MOUNT_LOCATION            "oad"
#endif

#define ARG_NUM_SCRIPT_FILE         1
#define IS_ARG_AVAILABLE_SCRIPT_FILE(x) ((x) > ARG_NUM_SCRIPT_FILE)
#define ARG_SCRIPT_FILE(x)              (x)[ARG_NUM_SCRIPT_FILE]


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static show_Error_cb fpShow_Error=NULL;
static show_Finish_cb fpShow_Finish=NULL;
static show_LoadData_cb fpShow_LoadData=NULL;
static show_StartUpgrading_cb fpShow_StartUpgrading=NULL;
static show_Upgrading_cb fpShow_upgrading=NULL;


//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------
extern int snprintf(char *str, size_t size, const char *fmt, ...);

//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------
#if (ENABLE_MODULE_USB == 1)
static int LoadScript2DramByUsb(char *upgradeFile, char *downloadBuf);
#endif
#if (ENABLE_MODULE_OAD == 1) || (ENABLE_MODULE_NETUPDATE == 1)||(ENABLE_MODULE_USB == 1)
static int _show_Error(unsigned int var);
static int _show_Finish(unsigned int var);
static int _show_LoadData(unsigned int var);
static int _show_StartUpgrading(unsigned int var);
static int _show_Upgrading(unsigned int var1, unsigned int var2);
static unsigned int _check_upgrade_script_cmd_count(char *buffer);
#endif
//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
#if (ENABLE_MODULE_NET == 1)
int do_mstar (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char* buffer = NULL;
    char *script_buf = NULL;
    char *next_line = NULL;
    char *ScriptFile = NULL;
    unsigned long iFilesize = 0;
    char *tmp = NULL;
    int BeAbort = 1;
    UBOOT_TRACE("IN\n");
    setUpgradeMode(EN_UPDATE_MODE_TFTP);

    ScriptFile= IS_ARG_AVAILABLE_SCRIPT_FILE(argc) ? ARG_SCRIPT_FILE(argv) : DEFAULT_SCRIPT_FILE_NAME;
    UBOOT_DEBUG("ScriptFile=%s\n",ScriptFile);

    buffer=(char *)malloc(SCRIPT_BUF_FOR_DO_MSTAR);
    if(buffer==NULL)
    {
        UBOOT_ERROR("no memory for command string!!\n");
        return -1;
    }

    // Set mstar commmand abort or not when cmd error.
    tmp = getenv ("CmdAbort");
    if (tmp != NULL)
    {
        BeAbort = (int)simple_strtol(tmp, NULL, 10);
        UBOOT_DEBUG("BeAbort=%d\n",BeAbort);
    }

    //Enable emac
    if(-1 == run_command("estart", 0))
    {
        UBOOT_ERROR("estart Fail \n");
        free(buffer);
        return -1;
    }

    // Set DHCP
    if(getenv("ipaddr") == NULL)
    {
        if(-1 == run_command("dhcp",0))
        {
            UBOOT_ERROR("please use the legal mac address.\n");
            free(buffer);// Fix CID:147674
            return -1;
        }
    }
    else
    {
        UBOOT_DEBUG("ipaddr=%s\n",getenv("ipaddr") );
    }
    do_Timer_ISR_Register();
    // Load script
    memset(buffer, 0 , CMD_BUF);
    snprintf(buffer, CMD_BUF, "tftp %X %s", buffer,ScriptFile);
    UBOOT_DEBUG("cmd=%s\n",buffer);
    if(-1 == run_command(buffer, 0))
    {
        UBOOT_ERROR("cmd: %s\n",buffer);
        free(buffer);
	SetTimerIsrDone();
        return -1;
    }

    // Run script
    iFilesize = (unsigned long)simple_strtol(getenv("filesize"), NULL, 16);
    UBOOT_DEBUG("iFilesize=0x%x\n",(unsigned int)iFilesize);
    script_buf = buffer;
    script_buf[iFilesize]='\n'; script_buf[iFilesize+1]='%';
    while ((next_line = get_script_next_line(&script_buf)) != NULL)
    {
        next_line=ParaReplace(next_line,argc-2,&argv[1]);
        UBOOT_INFO ("\n>> %s \n", next_line);
        if(-1 == run_command(next_line, 0))
        {
            UBOOT_ERROR("command error!\n");
            if(BeAbort == 1)
            {
                free(buffer);
                SetTimerIsrDone();
                return -1;
            }
        }
    }
    free(buffer);
    SetTimerIsrDone();
    UBOOT_TRACE("OK\n");
    return 0;
}
#endif

#if (ENABLE_MODULE_USB == 1)
static int LoadScript2DramByUsb(char *upgradeFile, char *downloadBuf)
{
    char *buffer=NULL;
    int partition = 0;
    int device = 0;
    UBOOT_TRACE("IN\n");
    if(check_file_partition(upgradeFile,&device,&partition)!=0)
    {
        UBOOT_ERROR("fail : no file in usb\n");
        return -1;
    }
    buffer=malloc(CMD_BUF);
    if(buffer==NULL)
    {
        UBOOT_ERROR("malloc fail\n");
        return -1;
    }
    memset(buffer,0,CMD_BUF);

    //Load Script
    snprintf(buffer, CMD_BUF, "fatload usb %d:%d  %X %s %X",device, partition, downloadBuf, upgradeFile, getScritSizeOfUpgradeImage());
    UBOOT_DEBUG("cmd=%s\n",buffer);
    if(-1 == run_command(buffer, 0))
    {

        UBOOT_ERROR("Error: cmd: %s\n",buffer);
        free(buffer);//Coverity CID:149588
        return -1;
    }
    /*Check script file is ciphertext or not(start)*/
#if (CONFIG_SECURITY_BOOT==1)
     if(check_script_encrypt(downloadBuf) == TRUE)
     {
        setUpgradeMode(EN_UPDATE_MODE_USB_WITH_SEG_DECRYPTED);
        memset(buffer, 0 , CMD_BUF);
        snprintf(buffer, CMD_BUF, "fileSegRSA usb %d:%d %s",device, partition, upgradeFile);
        UBOOT_DEBUG("cmd=%s\n",buffer);
        if(-1 == run_command(buffer, 0))
        {
            free(buffer);
            UBOOT_ERROR("Error: cmd: %s\n",buffer);
            return -1;
        }

        memset(buffer, 0 , CMD_BUF);
        snprintf(buffer, CMD_BUF, "filepartload %x %s %x %x",downloadBuf, upgradeFile, 0, getScritSizeOfUpgradeImage());
        UBOOT_DEBUG("cmd=%s\n",buffer);
        if(-1 == run_command(buffer, 0))
        {
            free(buffer);
            UBOOT_ERROR("Error: cmd: %s\n",buffer);
            return -1;
        }
     }
     else
#endif
     {
/*
        //check script crc
        if(check_script_crc(upgradeFile,downloadBuf)==-1)
        {
            free(buffer);
            UBOOT_ERROR("Error: check_script_crc Fail !!\n");
            return -1;
        }
*/
     }
     free(buffer);
     UBOOT_TRACE("OK\n");
     return 0;
}

int do_ustar(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char *script_buf = NULL;
    char *_script_buf = NULL;    
    char *next_line = NULL;
    char *upgradeFile = NULL;
    unsigned int scriptFileSize=0;
    unsigned int progress_total_cnt = 0;
    unsigned int progress_cnt = 0;

    UBOOT_TRACE("IN\n");
    setUpgradeMode(EN_UPDATE_MODE_USB); // set the Upgrade mode
    /* Load Script */
    _show_LoadData(0);

    upgradeFile = IS_ARG_AVAILABLE_SCRIPT_FILE(argc) ? ARG_SCRIPT_FILE(argv) : DEFAULT_BIN_PATH;

    scriptFileSize=getScritSizeOfUpgradeImage();
    if(scriptFileSize==0)
    {
        UBOOT_ERROR("The size of script file is zero \n");
        return -1;
    }

    _script_buf=malloc(scriptFileSize);
    if(_script_buf==NULL)
    {
        UBOOT_ERROR("malloc fail \n");
        return -1;
    }
    script_buf=_script_buf;
    memset(script_buf,0,scriptFileSize);


    if(setUpgradeFileName(upgradeFile)==-1)
    {
        setUpgradeMode(EN_UPDATE_MODE_NONE);
        free(_script_buf);
        UBOOT_ERROR("Error> set upgrade fail fail\n");
        _show_Error(0);
        return -1;
    }
    do_Timer_ISR_Register();
    if(LoadScript2DramByUsb(upgradeFile,script_buf) == -1)
    {
        deleteUpgradeFileName();
        free(_script_buf);
        setUpgradeMode(EN_UPDATE_MODE_NONE);
        _show_Error(0);
	SetTimerIsrDone();
        return -1;
    }
    #if(ENABLE_MODULE_DONT_OVERWRITE==1)
    set_dont_overwrite_byname(upgradeFile);
    #endif
    //Run Script

    //script_buf = (char *)UPGRADE_BUFFER_ADDR;

    progress_total_cnt=_check_upgrade_script_cmd_count(script_buf);
    if(progress_total_cnt==0)
    {
        deleteUpgradeFileName();
        setUpgradeMode(EN_UPDATE_MODE_NONE);
        free(_script_buf);
        UBOOT_ERROR("There are no any commands in the upgrade script file\n");
       _show_Error(0);
	    SetTimerIsrDone();
       return -1;
    }
    UBOOT_DEBUG("progress_total_cnt=%d\n",progress_total_cnt);

    _show_StartUpgrading(0);

    while ((next_line = get_script_next_line(&script_buf)) != NULL)
    {
        UBOOT_INFO("\n>> %s \n", next_line);

        progress_cnt++;
        UBOOT_DEBUG("progress_cnt=%d,progress_total_cnt=%d\n",progress_cnt,progress_total_cnt);
        _show_Upgrading(progress_cnt,progress_total_cnt);
        #if(ENABLE_MODULE_DONT_OVERWRITE==1)
        if(check_skip_cmd(next_line)== FALSE)//filterCmd
        {
        	printf("\n xxxxxx>> %s \n", next_line);
            if(run_command(next_line, 0) < 0)
            {
                deleteUpgradeFileName();
                setUpgradeMode(EN_UPDATE_MODE_NONE);
                free(_script_buf);
                UBOOT_ERROR("Error> cmd: %s Fail !!\n", next_line);
                _show_Error(0);
		        SetTimerIsrDone();
                return -1;

            }
        }
        #endif
        
    }

    if(deleteUpgradeFileName()==-1)
    {
        _show_Error(0);
        setUpgradeMode(EN_UPDATE_MODE_NONE);
        free(_script_buf);
	    SetTimerIsrDone();
        UBOOT_ERROR("Error> delete upgrade fail fail\n");
        return -1;
    }

    free(_script_buf);
    _show_Finish(0);
    SetTimerIsrDone();
    UBOOT_TRACE("OK\n");
    return 0;


}

int do_udstar (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char *cmdBuf=NULL;
    char *script_buf=NULL;
    char *pBuf=NULL;
    char *next_line=NULL;
    char *upgradeFile=NULL;
    char *env=NULL;
    int ret=0;
    unsigned int u32FileSize=0;
    int partition = 0;
    int device = 0;
    UBOOT_TRACE("IN\n");

    upgradeFile = IS_ARG_AVAILABLE_SCRIPT_FILE(argc) ? ARG_SCRIPT_FILE(argv) : DEFAULT_DEBUG_SCRIPT_PATH;
    
    if(check_file_partition(upgradeFile,&device,&partition)!=0)
    {
        UBOOT_ERROR("no file in usb\n");
        return -1;
    }
    
    UBOOT_DEBUG("upgradeFile=%s\n",upgradeFile);

    cmdBuf=malloc(CMD_BUF);
    if(cmdBuf==NULL)
    {
        UBOOT_ERROR("malloc fail\n");
        return -1;
    }
    memset(cmdBuf, 0 , CMD_BUF);
    snprintf(cmdBuf, CMD_BUF, "fatload usb %d:%d %X %s 1", device,partition,cmdBuf, upgradeFile);
    UBOOT_DEBUG("cmd=%s\n",cmdBuf);
    ret=run_command(cmdBuf, 0); 
    if(ret!=0)
    {
        free(cmdBuf);
        UBOOT_ERROR("No script file exist\n");
        return -1;
    }

    memset(cmdBuf, 0, CMD_BUF);
    snprintf(cmdBuf, CMD_BUF, "fatfilesize usb %d:%d %s", device,partition, upgradeFile);
    UBOOT_DEBUG("cmd=%s\n",cmdBuf);
    ret = run_command(cmdBuf, 0);
    if(ret!=0)
    {
        free(cmdBuf); 
        UBOOT_ERROR("get script file's size fail\n");
        return -1;
    }
    // Get file size. If larger then DRAM, don't load the whole. Just load its script.
    env=getenv("filesize");
    if(env==NULL)
    {
       free(cmdBuf);    
       UBOOT_ERROR("get env 'filesize' fail\n");
       return -1;
    }
    u32FileSize = simple_strtol(env, NULL, 16);
    pBuf=malloc(u32FileSize);
    if(pBuf==NULL)
    {
        free(cmdBuf); 
        UBOOT_ERROR("malloc for script file fail\n");
        return -1;
    }
    script_buf=pBuf;
    memset(script_buf,0,u32FileSize);

    memset(cmdBuf, 0, CMD_BUF);
    snprintf(cmdBuf, CMD_BUF, "fatload usb %d:%d %X %s %X",device,partition,script_buf,upgradeFile,u32FileSize);
    UBOOT_DEBUG("cmd=%s\n",cmdBuf);    
    ret = run_command(cmdBuf, 0);
    if(ret!=0)
    {
        free(pBuf);
        free(cmdBuf);        
        return -1;
    }

    while ((next_line = get_script_next_line(&script_buf)) != NULL)
    {
        UBOOT_INFO ("\n>>1 %s \n", next_line);
        
        if(strstr((char *)next_line,(char *)"go")!=NULL)
        {
            UBOOT_INFO ("skip cmd 'go' \n");
            continue;
        }

        if(strstr((char *)next_line,(char *)"bootm")!=NULL)
        {
            UBOOT_INFO ("skip cmd 'bootm' \n");
            continue;
        }
        
        ret=run_command(next_line, 0);
        if(ret!=0)
        {
            free(pBuf);
            free(cmdBuf);    
            UBOOT_ERROR("run cmd '%s' fail\n",next_line);
            return -1;
        }
    }

    free(cmdBuf);
    free(pBuf);
    UBOOT_TRACE("OK\n");
    return 0;
}

#endif

#if (ENABLE_MODULE_OAD == 1) || (ENABLE_MODULE_NETUPDATE == 1)

int CheckOadCRC(char *upgradeFile)
{
     U32 FileSize = 0;
     U32 CRC_Value = 0;
     U32 CRC_Result = 0;
     char const *HeadLine = NULL;
     char const *TailLine = NULL;
     UBOOT_TRACE("IN\n");

     FileSize = vfs_getsize(upgradeFile);
     HeadLine=(char const *)UPGRADE_BUFFER_ADDR;
     TailLine=(char const *)(UPGRADE_BUFFER_ADDR + (FileSize-16));

     if(strncmp(HeadLine,TailLine,16)==0)//check encrypt
     {
         CRC_Value=*((U32*)(UPGRADE_BUFFER_ADDR+(FileSize-24)));
     }
     else
     {
         CRC_Value=*((U32*)(UPGRADE_BUFFER_ADDR+(FileSize-8)));
     }

     CRC_Result=crc32(0x00, (U8*)UPGRADE_BUFFER_ADDR, getScritSizeOfUpgradeImage());
     if(CRC_Result!= CRC_Value)
     {
         UBOOT_ERROR("CRC_Result = 0x%X , CRC_Value = 0x%X, size=0x%X\n",CRC_Result,CRC_Value,getScritSizeOfUpgradeImage());
         return -1;
     }
     UBOOT_TRACE("OK\n");

     return 0;
}

int do_ostar(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char *script_buf=NULL;
    char *_script_buf = NULL; 
    char *next_line=NULL;
    char *upgradeFile = NULL;
    unsigned int scriptFileSize=0;
    unsigned int progress_total_cnt = 0;
    unsigned int progress_cnt = 0;

    UBOOT_TRACE("IN\n");
    setUpgradeMode(EN_UPDATE_MODE_OAD); // set the Upgrade mode

    upgradeFile = IS_ARG_AVAILABLE_SCRIPT_FILE(argc) ? ARG_SCRIPT_FILE(argv) : DEFAULT_BIN_PATH;
    UBOOT_DEBUG("upgradeFile=%s\n",upgradeFile);
    if(setUpgradeFileName(upgradeFile)==-1)
    {
        UBOOT_ERROR("Error> set upgrade fail fail\n");
        _show_Error(0);
        setUpgradeMode(EN_UPDATE_MODE_NONE);
        return -1;
    }

     scriptFileSize=getScritSizeOfUpgradeImage();
     if(scriptFileSize==0)
     {
         UBOOT_ERROR("The size of script file is zero \n");
         return -1;
     }
    
     _script_buf=malloc(scriptFileSize+512);
     if(_script_buf==NULL)
     {
         UBOOT_ERROR("malloc fail \n");
         return -1;
     }
     script_buf=_script_buf;
     memset(script_buf,0,scriptFileSize);

    
    _show_LoadData(0);
    
    // load script
    if(vfs_mount(MOUNT_LOCATION) == -1)
    {
        _show_Error(0);
        deleteUpgradeFileName();
        setUpgradeMode(EN_UPDATE_MODE_NONE);
        free(_script_buf);        
        UBOOT_ERROR("Error: vfs_mount Fail !!\n");        
        return -1;
    }

    upgradeFile=getenv(ENV_UPGRADE_NAME);
    if(upgradeFile==NULL)
    {
        _show_Error(0);
        deleteUpgradeFileName();
        setUpgradeMode(EN_UPDATE_MODE_NONE);
        free(_script_buf);        
        UBOOT_ERROR("get upgrade file name fail\n");        
        return -1;
    }
    
    if(vfs_read((void *)script_buf,upgradeFile,0,getScritSizeOfUpgradeImage()) == -1)
    {
        
        free(_script_buf);
        deleteUpgradeFileName();
        setUpgradeMode(EN_UPDATE_MODE_NONE);
        UBOOT_ERROR("Error: vfs_read Fail !!\n");
        _show_Error(0);
        return -1;
    }
    do_Timer_ISR_Register();

    //set dont overwrite
    #if(ENABLE_MODULE_DONT_OVERWRITE==1)
    dont_overwrite_enable();
    #endif
    _show_StartUpgrading(0);
    
    //run script
    progress_total_cnt=_check_upgrade_script_cmd_count(script_buf);
    if(progress_total_cnt==0)
    {
       _show_Error(0);
       deleteUpgradeFileName();
       setUpgradeMode(EN_UPDATE_MODE_NONE);
	   SetTimerIsrDone();
       free(_script_buf);     
       UBOOT_ERROR("There are no any commands in the upgrade script file\n");
       return -1;
    }
    UBOOT_DEBUG("progress_total_cnt=%d\n",progress_total_cnt);

    while ((next_line = get_script_next_line(&script_buf)) != NULL)
    {
        UBOOT_INFO ("\n>> %s \n", next_line);

        progress_cnt++;
        UBOOT_DEBUG("progress_cnt=%d,progress_total_cnt=%d\n",progress_cnt,progress_total_cnt);
        _show_Upgrading(progress_cnt,progress_total_cnt);
        #if(ENABLE_MODULE_DONT_OVERWRITE==1)
        if(check_skip_cmd(next_line)== FALSE)
        {
            if(run_command(next_line, 0) < 0)
            {
                
                _show_Error(0);
                deleteUpgradeFileName();
                setUpgradeMode(EN_UPDATE_MODE_NONE);
		        SetTimerIsrDone();
                free(_script_buf);     
                UBOOT_ERROR("Error> cmd: %s Fail !!\n", next_line);
                return -1;
            }
        }
        #endif
        
    }

    if(deleteUpgradeFileName()==-1)
    {
       
        _show_Error(0);
        setUpgradeMode(EN_UPDATE_MODE_NONE);
	    SetTimerIsrDone();
        free(_script_buf);
        UBOOT_ERROR("Error> delete upgrade fail fail\n");
        return -1;
    }

    
    free(_script_buf);
    SetTimerIsrDone();
    _show_Finish(0);
    UBOOT_TRACE("OK\n");
    return 0;
}

#endif

#if ((ENABLE_MODULE_OAD == 1) || (ENABLE_MODULE_NET == 1)||(ENABLE_MODULE_USB == 1))

void RegisterCBShowError(show_Error_cb cb)
{
    UBOOT_TRACE("IN\n");
    fpShow_Error=cb;
    UBOOT_TRACE("OK\n");
}

void RegisterCBShowFinish(show_Finish_cb cb)
{
    UBOOT_TRACE("IN\n");
    fpShow_Finish=cb;
    UBOOT_TRACE("OK\n");
}

void RegisterCBShowLoadData(show_LoadData_cb cb)
{
    UBOOT_TRACE("IN\n");
    fpShow_LoadData=cb;
    UBOOT_TRACE("OK\n");
}

void RegisterCBShowStartUpgrading(show_StartUpgrading_cb cb)
{
    UBOOT_TRACE("IN\n");
    fpShow_StartUpgrading=cb;
    UBOOT_TRACE("OK\n");
}
void RegisterCBShowUpgrading(show_Upgrading_cb cb)
{
    UBOOT_TRACE("IN\n");
    fpShow_upgrading=cb;
    UBOOT_TRACE("OK\n");
}

void UnRegisterShowCb(void)
{
    UBOOT_TRACE("IN\n");
    fpShow_Error=NULL;
    fpShow_Finish=NULL;
    fpShow_LoadData=NULL;
    fpShow_StartUpgrading=NULL;
    fpShow_upgrading=NULL;
    UBOOT_TRACE("OK\n");
}


static int _show_Error(unsigned int var)
{
    UBOOT_TRACE("IN\n");
    if(fpShow_Error!=NULL)
    {
        UBOOT_TRACE("OK\n");
        return fpShow_Error(var);
    }
    else
    {
        UBOOT_DEBUG("No call back function for 'error' phase\n");
        UBOOT_TRACE("OK\n");
        return 0;
    }
}

static int _show_Finish(unsigned int var)
{
    UBOOT_TRACE("IN\n");
    if(fpShow_Finish!=NULL)
    {
        UBOOT_TRACE("OK\n");
        return fpShow_Finish(var);
    }
    else
    {
        UBOOT_DEBUG("No call back function for 'finish' phase\n");
        UBOOT_TRACE("OK\n");
        return 0;
    }
}

static int _show_LoadData(unsigned int var)
{
    UBOOT_TRACE("IN\n");
    if(fpShow_LoadData!=NULL)
    {
        UBOOT_TRACE("OK\n");
        return fpShow_LoadData(var);
    }
    else
    {
        UBOOT_DEBUG("No call back function for 'LoadData' phase\n");
        UBOOT_TRACE("OK\n");
        return 0;
    }
}
static int _show_StartUpgrading(unsigned int var)
{
    UBOOT_TRACE("IN\n");
    if(fpShow_StartUpgrading!=NULL)
    {
        UBOOT_TRACE("OK\n");
        return fpShow_StartUpgrading(var);
    }
    else
    {
        UBOOT_DEBUG("No call back function for 'StartUpgrading' phase\n");
        UBOOT_TRACE("OK\n");
        return 0;
    }
}

static int _show_Upgrading(unsigned int var1, unsigned int var2)
{
    UBOOT_TRACE("IN\n");
    if(fpShow_upgrading!=NULL)
    {
        UBOOT_TRACE("OK\n");
        return fpShow_upgrading(var1, var2);
    }
    else
    {
        UBOOT_DEBUG("No call back function for 'Upgrading' phase\n");
        UBOOT_TRACE("OK\n");
        return 0;
    }
}
static unsigned int _check_upgrade_script_cmd_count(char *buffer)
{
    unsigned int scriptFileSize=0;
    unsigned int amount=0;
    char *tempBuf=NULL;

    UBOOT_TRACE("IN\n");
    scriptFileSize=getScritSizeOfUpgradeImage();
    if(scriptFileSize==0)
    {
        UBOOT_ERROR("The size of script file is zero \n");
        return 0;
    }

    tempBuf=malloc(scriptFileSize);
    if(tempBuf==NULL)
    {
        UBOOT_ERROR("malloc fail \n");
        return 0;
    }
    memcpy(tempBuf,buffer,scriptFileSize);
    amount=check_script_cmd_count(tempBuf);
    free(tempBuf);

    UBOOT_TRACE("OK\n");
    return amount;
}

#endif



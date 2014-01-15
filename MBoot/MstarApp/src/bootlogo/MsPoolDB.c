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

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <drvBDMA.h>
#include <bootlogo/MsPoolDB.h>
#include <MsVfs.h>
#include <MsUtility.h>
#include <MsDebug.h>
#include <MsSystem.h>
#include <MsRawIO.h>
#include <drvSERFLASH.h>
#include <CusConfig.h>

#include <uboot_mmap.h>
#include <bootlogo/iniparser.h>
#include <bootlogo/MsBootLogo.h>



#if (CONFIG_URSA_6M30)
#include <../drivers/mstar/ursa/ursa_6m30.h>
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
db_struct db_table;
static char gPnlPath[BUFFER_SIZE];
static char gBoardPath[BUFFER_SIZE];
static char gLogoPath[BUFFER_SIZE];
static char gMusicPath[BUFFER_SIZE];
static char gKeypadPath[BUFFER_SIZE];
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------
extern char * strncpy(char * dest,const char *src,size_t count);
extern int snprintf(char *str, size_t size, const char *fmt, ...);

//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------
int Load_LogoToFlash(void);
int Load_PanelSetting_ToFlash(void);
int Load_BoardSetting_ToFlash(void);
int Load_KeypadSetting_ToFlash(void);
int Load_MusicToFlash(void);
int iniparser_6m30_cmd(ursa_6m30_cmd_table * cmd_table);
int Load_Ursa6m30Para_ToFlash(void);
int Ursa_6M30_Setting(ursa_6m30_cmd_table * cmd_table);

void count_data_crc32_setenv(U32 addr,int size,char *name);
int count_data_crc32_getenv(U32 addr,int size,char *name);

#if (CONFIG_PANEL_INIT)
int parse_board_ini(char *path, st_board_para *p_data);
#endif
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

int parse_sys_ini(char* path,char* ModelPath)
{
    int ret = -1;
    char *script = NULL;
    char str[BUFFER_SIZE];
    U32 filesize = 0;

    script = loadscript(path,&filesize);
    if(script == NULL)
    {
        goto exit;
    }
    UBOOT_INFO("sizeof( sys ini script ) --------> %u bytes\n",filesize);
    Profile_Init((char *)script,filesize);

    // select mode via project id
#if defined (CONFIG_LOGO_STORE_IN_NAND_FLASH)
    if(Profile_GetBoolean("select_model_via_project_id", "bEnabled",0) == TRUE)
    {

        U16 u16MaxProjectID  = 0xffff;
        char cTempBuffer[BUFFER_SIZE]= {0};
        U32 u32ProjectIdAddr = 0;
        U32 u32ProjectIdBackupAddr = 0;
        U32 u32ProjectId;

#if (ENABLE_MSTAR_BD_MST036B_EDISON_CUS19 == 1)
  	    U16 u16projectId=0xffff;
  	    U16 u16temp=0xffff;
#endif

        u16MaxProjectID = Profile_GetInteger("select_model_via_project_id", "MAX_MODEL_INDEX", -1);
        u32ProjectIdAddr = Profile_GetInteger("select_model_via_project_id", "project_id_spi_addr", 0);
        flush_cache((MS_PHYADDR)&u32ProjectId, sizeof(u32ProjectId));
        if(E_BDMA_OK != MDrv_BDMA_FlashCopy2Dram(u32ProjectIdAddr, (MS_PHYADDR)TO_PHY_ADDR(&u32ProjectId), sizeof(u32ProjectId)))
        {
            UBOOT_ERROR("ERROR: read project id fail...>>>\n");
            goto exit;
        }
        else
        {
            UBOOT_DEBUG("read project id success...\n");
            UBOOT_DEBUG("project id is [0x%08x]\n", u32ProjectId);

#if (ENABLE_MSTAR_BD_MST036B_EDISON_CUS19 == 1)
		    u16projectId = (U16)(u32ProjectId & 0xffff);
		    u16temp = u16projectId >>8;
		    u16projectId = u16projectId<<8;
		    u32ProjectId = u16projectId |u16temp;
		    u16projectId =0;
#endif
            // Read Data from backup
            if((u32ProjectId > u16MaxProjectID) || (u32ProjectId == 0x0000))
            {
                u32ProjectIdBackupAddr = Profile_GetInteger("select_model_via_project_id", "project_id_backup_spi_addr", 0);
                if(E_BDMA_OK != MDrv_BDMA_FlashCopy2Dram(u32ProjectIdBackupAddr, (MS_PHYADDR)TO_PHY_ADDR(&u32ProjectId), sizeof(u32ProjectId)))
                {
                    UBOOT_ERROR("ERROR: read backup project id fail...>>>\n");
                    goto exit;
                }
                else
                {
#if (ENABLE_MSTAR_BD_MST036B_EDISON_CUS19 == 1)
   		           u16projectId = (U16)(u32ProjectId & 0xffff);
   		           u16temp = u16projectId >>8;
   		           u16projectId = u16projectId<<8;
   		           u32ProjectId = u16projectId |u16temp;
   		           u16projectId =0;
#endif
                    if(u32ProjectId > u16MaxProjectID) //set to default project ID
                    {
                        u32ProjectId = 1;
                    }
                }
            }
        }
        flush_cache((U32)&u32ProjectId, sizeof(u32ProjectId));
        Chip_Flush_Memory();

        snprintf(cTempBuffer, BUFFER_SIZE, "Model_%d", u32ProjectId);

        if(Profile_GetString("select_model_via_project_id", cTempBuffer, "", str, sizearray(str)))
        {
            UBOOT_INFO("Half model.ini path --> %s\n",str);
            snprintf(ModelPath, BUFFER_SIZE, "%s",str);
            UBOOT_INFO("Full model.ini  path --> %s\n",ModelPath);
            ret = 0;
        }
    }
    else
    #endif
    {
        if(Profile_GetString("model", "gModelName", "", str, sizearray(str)))
        {
            UBOOT_INFO("Half model.ini path --> %s\n",str);
            snprintf(ModelPath, BUFFER_SIZE, "%s",str);
            UBOOT_INFO("Full model.ini  path --> %s\n",ModelPath);
            ret = 0;
        }
    }

exit:
    if(script)
        free(script);
    return ret;
}

int parse_model_ini(char* path, char* PnlPath, char* BoardPath)
{
    int ret = -1;//
    char *script = NULL;
    char str[BUFFER_SIZE];
    U32 filesize = 0;
    script = loadscript(path,&filesize);
    if(script == NULL)
    {
        while (*path== '/') path++;
        path=strstr(path,"/");
        UBOOT_INFO("filter /config, and find file again!!\n");
        script = loadscript(path,&filesize);
        if(script == NULL)
        {
            goto exit;
        }
    }
    UBOOT_INFO("sizeof( model ini script ) --------> %u bytes\n",filesize);

    Profile_Init((char *)script,filesize);
    if(Profile_GetString("panel", "m_pPanelName","", str, sizearray(str)))
    {
        UBOOT_INFO("Half panel.ini path --> %s\n",str);
        snprintf(PnlPath,BUFFER_SIZE,"%s",str);
        UBOOT_INFO("Full panel.ini  path --> %s\n",PnlPath);
        ret = 0;
    }
    else
    {
        goto exit;
    }

    if(Profile_GetString("board", "m_pBoardName","", str, sizearray(str)))
    {
        UBOOT_INFO("Half board.ini path --> %s\n",str);
        snprintf(BoardPath,BUFFER_SIZE,"%s",str);
        UBOOT_INFO("Full board.ini  path --> %s\n",BoardPath);
        ret = 0;
    }
    else
    {
        goto exit;
    }

#if (CONFIG_PANEL_INIT == 1)
    int n = 0;
    char *tmp = NULL;
    memset(str,0,sizeof(str));
    tmp = getenv("swing_level");
    if (tmp != NULL)
    {
        n = (int)simple_strtol(tmp, NULL, 10);
    }
    else
    {
        n = Profile_GetInteger("PANEL_SWING_LEVEL", "SWING_LEVEL", -1);
    }

    if (n >= 0)
    {
        snprintf(str,sizeof(str),"%u",n);
        UBOOT_INFO("swing_level -> [%s]\n",str);
        setenv("swing_level",str);
    }
    else
    {
        setenv("swing_level","250");
    }
#endif

exit:
    if(script)
        free(script);
    return ret;
}

int parse_boot_ini(char* path)
{
    int ret = 0;//
    int n = 0;
    char *script = NULL;
    char str[BUFFER_SIZE];
    U32 filesize = 0;
    char *tmp;
    script = loadscript(path,&filesize);
    if(script == NULL)
    {
        ret = -1;
        goto exit;
    }
    UBOOT_INFO("sizeof( boot ini script ) --------> %u bytes\n",filesize);
    Profile_Init((char *)script,filesize);

    //Music
    tmp = getenv ("music");
    if (tmp != NULL)
    {
        n = (int)simple_strtol(tmp, NULL, 10);
    }
    else
    {
        n = Profile_GetInteger("MUSIC_CFG", "MUSIC_NUM", -1);
        memset(str,0 ,sizeof(str));
        snprintf(str,sizeof(str),"%u",n);
        setenv("music",str);
        UBOOT_INFO("music -> [%s]\n",str);
    }

    if (n > 0 && n < 3)
    {
#ifdef KONKA_BOOT_CFG
        snprintf(gMusicPath,sizeof(gMusicPath),"%s/boot%u.mp3",CUSTOMER_PATH_KONKA,n - 1);
#else
        snprintf(gMusicPath,sizeof(gMusicPath),"%s/boot%u.mp3",CUSTOMER_PATH,n - 1);
#endif
        UBOOT_INFO("gMusicPath --> %s\n",gMusicPath);
    }
    else if (n == 0)
    {
        setenv("music","0");
#ifdef KONKA_BOOT_CFG
        snprintf(gMusicPath,sizeof(gMusicPath),"%s/boot0.mp3",CUSTOMER_PATH_KONKA);
#else
        snprintf(gMusicPath,sizeof(gMusicPath),"%s/boot0.mp3",CUSTOMER_PATH);
#endif
        UBOOT_INFO("gMusicPath --> %s\n",gMusicPath);
        UBOOT_INFO("user set music off \n");
    }
    else
    {
        setenv("music","1");
        UBOOT_INFO("Get exception music number!reset default music!\n");
#ifdef KONKA_BOOT_CFG
        snprintf(gMusicPath,sizeof(gMusicPath),"%s/boot0.mp3",CUSTOMER_PATH_KONKA);
#else
        snprintf(gMusicPath,sizeof(gMusicPath),"%s/boot0.mp3",CUSTOMER_PATH);
#endif
        UBOOT_INFO("gMusicPath --> %s\n",gMusicPath);
    }


    //Logo
    tmp = getenv ("logo");
    if (tmp != NULL)
    {
        n = (int)simple_strtol(tmp, NULL, 10);
    }
    else
    {
        n = Profile_GetInteger("LOGO_CFG", "LOGO_NUM", -1);
        memset(str,0 ,sizeof(str));
        snprintf(str,sizeof(str),"%u",n);
        setenv("logo",str);
        UBOOT_INFO("logo -> [%s]\n",str);
    }
    if (n > 0 && n < 4)
    {
#ifdef KONKA_BOOT_CFG
        snprintf(gLogoPath,sizeof(gLogoPath),"%s/boot%u.jpg",CUSTOMER_PATH_KONKA,n - 1);
#else
        snprintf(gLogoPath,sizeof(gLogoPath),"%s/boot%u.jpg",CUSTOMER_PATH,n - 1);
#endif
        UBOOT_INFO("gLogoPath --> %s\n",gLogoPath);
    }
    else if (n == 0)
    {
        setenv("logo","0");
#ifdef KONKA_BOOT_CFG
        snprintf(gLogoPath,sizeof(gLogoPath),"%s/boot0.jpg",CUSTOMER_PATH_KONKA);
#else
        snprintf(gLogoPath,sizeof(gLogoPath),"%s/boot0.jpg",CUSTOMER_PATH);
#endif
        UBOOT_INFO("user set logo off \n");
    }
    else
    {
        setenv("logo","1");
        UBOOT_INFO("Get exception logo number!reset default logo!\n");
#ifdef KONKA_BOOT_CFG
        snprintf(gLogoPath,sizeof(gLogoPath),"%s/boot0.jpg",CUSTOMER_PATH_KONKA);
#else
        snprintf(gLogoPath,sizeof(gLogoPath),"%s/boot0.jpg",CUSTOMER_PATH);
#endif
        UBOOT_INFO("gLogoPath --> %s\n",gLogoPath);
    }


    //Music volume
    memset(str,0 ,sizeof(str));
    tmp = getenv("music_vol");
    if (0)//(tmp != NULL)	// need to read from ini file again
    {
        n = (int)simple_strtol(tmp, NULL, 10);
    }
    else
    {
        n = Profile_GetInteger("MUSIC_VOL_CFG", "MUSIC_VOL", -1);
    }

    if((n >= 0)&&(n <= 0x7F))
    {
        snprintf(str,sizeof(str),"0x%x",n);
        UBOOT_INFO("music_vol -> [%s]\n",str);
        setenv("music_vol",str);
    }
    else
    {
        setenv("music_vol","30");
    }

exit:
    if(script)
        free(script);
    return ret;
}


int parse_ini(void)
{
    int ret = -1;//
    char InPath[BUFFER_SIZE];
    char OutPath[BUFFER_SIZE];
    memset(gPnlPath,0,sizeof(gPnlPath));
    memset(gBoardPath,0,sizeof(gBoardPath));
    memset(gMusicPath,0,sizeof(gMusicPath));
    memset(gLogoPath,0,sizeof(gLogoPath));
    memset(gKeypadPath,0,sizeof(gKeypadPath));
    vfs_mount(CONFIG);
    memset(InPath,0,sizeof(InPath));
    snprintf(InPath,sizeof(InPath),"%s/sys.ini",CONFIG_PATH);
    ret = parse_sys_ini(InPath,OutPath);
    if(ret)
    {
        goto exit;
    }
    memset(InPath,0,sizeof(InPath));
    memcpy(InPath,OutPath,sizeof(OutPath));
    //set keypad init setting
    memcpy(gKeypadPath,OutPath,sizeof(OutPath));
    UBOOT_INFO("Full Customer.ini for keypad path --> %s\n",gKeypadPath);

#ifdef KONKA_BOOT_CFG
    vfs_mount(CUSTOMER_KONKA);
#endif
    ret = parse_model_ini(InPath, gPnlPath, gBoardPath);
    if(ret)
    {
        goto exit;
    }
#if (ENABLE_MSTAR_BD_MST036B_EDISON_CUS19 == 1)//copy from MS801:swing level by panel
#if (CONFIG_PANEL_INIT == 1)
    vfs_mount(CONFIG);
    st_board_para stBoard;
    memset(&stBoard,0,sizeof(stBoard));
    memset(InPath,0,sizeof(InPath));
    snprintf(InPath,sizeof(InPath),"%s/board.ini",CONFIG_PATH);
    //st_board_para stBoard;
    memset(&stBoard,0,sizeof(stBoard));
    ret = parse_board_ini(InPath, &stBoard);	//huaxing must refine
    if(ret == -1)
    {
        setenv("swing_level","250");
    }
#endif
#endif
#ifdef KONKA_BOOT_CFG
    vfs_mount(CUSTOMER_KONKA);
    memset(InPath,0,sizeof(InPath));
    snprintf(InPath,sizeof(InPath),"%s/boot.ini",CUSTOMER_PATH_KONKA);
#else
    vfs_mount(CUSTOMER);
    memset(InPath,0,sizeof(InPath));
    snprintf(InPath,sizeof(InPath),"%s/boot.ini",CUSTOMER_PATH);
#endif
    ret = parse_boot_ini(InPath);
    if(ret)
    {
        goto exit;
    }
exit:
    return ret;
}

int prepare_nand_dbtable(U32 u32LogoOffset)
{
    int ret = -1;
    UBOOT_TRACE("IN\n");
   // U32 u32FlashSize;
    memset((void*)&db_table,0,sizeof(db_table));
#ifdef KONKA_BOOT_CFG
    vfs_mount(CUSTOMER_KONKA);
#else
    vfs_mount(CUSTOMER);
#endif
#if(ENABLE_DISPLAY_LOGO)
    db_table.logo_size = vfs_getsize(gLogoPath);
    UBOOT_DEBUG("gLogoPath=%s\n",gLogoPath);
    UBOOT_DEBUG("db_table.logo_size=0x%x\n",db_table.logo_size);

    if(db_table.logo_size > 0)
    {
        UBOOT_DEBUG("%s size =:0x%08x \n",gLogoPath,db_table.logo_size);
    }
    else
    {
        UBOOT_ERROR(" get logo_size fail...>>>:%d\n",db_table.logo_size);
        return ret;
    }
#endif

#if ENABLE_POWER_MUSIC
    //get music_size
    db_table.music_size = vfs_getsize(gMusicPath);
    UBOOT_DEBUG("gMusicPath=%s\n",gMusicPath);
    UBOOT_DEBUG("db_table.music_size=0x%x\n",db_table.music_size);
    if( db_table.music_size > 0)
    {
        UBOOT_DEBUG("%s size =:0x%08x \n",gMusicPath,db_table.music_size);
    }
    else
    {
        UBOOT_ERROR(" get music_size fail...>>>:%d\n",db_table.music_size);
        return ret; //althought music is set off, logo can be showed if it is set on
    }
#endif


#if (CONFIG_PANEL_INIT)
    db_table.panelpara_size = sizeof(PanelType);
    db_table.boardpara_size = sizeof(st_board_para);
#endif

#if(ENABLE_URSA_6M30 == 1)
    db_table.ursa_6m30_size = sizeof(ursa_6m30_cmd_table);
#endif

#if(ENABLE_KEYPAD_CUST == 1)
    db_table.keypad_size= sizeof(st_keypad_para);
#endif

    db_table.logo_offset  = u32LogoOffset+sizeof(db_table);
    db_table.music_offset = db_table.logo_offset + db_table.logo_size;
    db_table.panelpara_offset = db_table.music_offset + db_table.music_size;
    db_table.boardpara_offset = db_table.panelpara_offset + db_table.panelpara_size;
    db_table.ursa_6m30_offset = db_table.boardpara_offset + db_table.boardpara_size;
    db_table.keypad_offset = db_table.ursa_6m30_offset + db_table.ursa_6m30_size;

    UBOOT_DEBUG("logo  offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.logo_offset,db_table.logo_size,(db_table.logo_offset+db_table.logo_size));
    UBOOT_DEBUG("music offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.music_offset,db_table.music_size,(db_table.music_offset+db_table.music_size));
    UBOOT_DEBUG("panel offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.panelpara_offset,db_table.panelpara_size,(db_table.panelpara_offset+db_table.panelpara_size));
    UBOOT_DEBUG("board offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.boardpara_offset,db_table.boardpara_size,(db_table.boardpara_offset+db_table.boardpara_size));
    UBOOT_DEBUG("ursa  offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.ursa_6m30_offset,db_table.ursa_6m30_size,(db_table.ursa_6m30_offset+db_table.ursa_6m30_size));
    UBOOT_DEBUG("keypad  offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.keypad_offset,db_table.keypad_size,(db_table.keypad_offset+db_table.keypad_size));

    {
        U32 db_size = 0;
        db_size = (sizeof(db_table)+db_table.logo_size+db_table.music_size+db_table.panelpara_size+db_table.ursa_6m30_size+db_table.keypad_size+env_sector_size+u32LogoOffset);
        UBOOT_DEBUG("u32LogoOffset  =: 0x%x \n",u32LogoOffset);
        UBOOT_DEBUG("db size        =: 0x%x \n",db_size);
    }
    count_data_crc32_setenv((U32)&db_table,sizeof(db_struct),"db_crc");
    ret = raw_write((U32)&db_table,u32LogoOffset,sizeof(db_table));
    UBOOT_TRACE("OK\n");
    return ret;
}

int write_dbtable(U32 u32LogoOffset)
{
    int ret = -1;
    UBOOT_TRACE("IN\n");
    parse_ini();
    if(prepare_nand_dbtable(u32LogoOffset)==-1)
    {
        UBOOT_ERROR("flash database not ready...>>>\n");
        return ret;
    }
    else
    {

#if ENABLE_DISPLAY_LOGO
        Load_LogoToFlash();
#endif
#if ENABLE_POWER_MUSIC
        Load_MusicToFlash();
#endif
#if (CONFIG_PANEL_INIT)
        Load_PanelSetting_ToFlash();
        Load_BoardSetting_ToFlash();
#endif
#if ENABLE_URSA_6M30
        Load_Ursa6m30Para_ToFlash();
#endif

#if ENABLE_KEYPAD_CUST
        //Load_KeypadSetting_ToFlash();
#endif

        setenv("db_table","1");
        saveenv();
        ret = 0;
    }
    UBOOT_TRACE("OK\n");
    return ret;
}

int read_dbtable(U32 u32LogoOffset)
{
    if(-1 == raw_read((U32)&db_table,u32LogoOffset,sizeof(db_table)))
    {
        UBOOT_ERROR("ERROR: read_mmc_dbtable fail...>>>\n");
        return -1;
    }
    else
    {
        UBOOT_DEBUG("read_dbtable success...>>>\n");
        UBOOT_DEBUG("read table >>> \n");
        UBOOT_DEBUG("logo  offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.logo_offset,db_table.logo_size,(db_table.logo_offset+db_table.logo_size));
        UBOOT_DEBUG("music offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.music_offset,db_table.music_size,(db_table.music_offset+db_table.music_size));
        UBOOT_DEBUG("panel offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.panelpara_offset,db_table.panelpara_size,(db_table.panelpara_offset+db_table.panelpara_size));
        UBOOT_DEBUG("ursa  offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.ursa_6m30_offset,db_table.ursa_6m30_size,(db_table.ursa_6m30_offset+db_table.ursa_6m30_size));
        UBOOT_DEBUG("keypad  offset[0x%08x] size[0x%08x] end[0x%08x]\n",db_table.keypad_offset,db_table.keypad_size,(db_table.keypad_offset+db_table.keypad_size));
    }
    ulong checksum = 0,savesum = 0;
    char *p_str = NULL;
    checksum = crc32(0,(unsigned char *)&db_table,sizeof(db_struct));
    p_str = getenv("db_crc");
    if(p_str == NULL)
    {
        savesum = 0;
    }
    else
    {
        savesum = simple_strtoul(p_str, NULL, 16);
    }
    if(checksum != savesum)
    {
        UBOOT_ERROR("Error : savesum =%ld , checksum= %ld \n",savesum,checksum);
        UBOOT_ERROR("Reload the dbtable from AP !!!\n");
        return write_dbtable(u32LogoOffset);
    }
    return 0;
}
void count_data_crc32_setenv(U32 addr,int size,char *name)
{
    char buf[32] = "\0";
    ulong checksum = 0;
    checksum = crc32(0,(unsigned char *)addr,size);
    snprintf(buf,sizeof(buf),"%lx",checksum);
    setenv(name,buf);
    UBOOT_DEBUG("%s checksum =: 0x%lx ... %s \n",name,checksum,buf);
}
int count_data_crc32_getenv(U32 addr,int size,char *name)
{
    int ret = -1;
    U32 checksum,savesum;
    char * p_str = NULL;
    checksum = crc32(0,(unsigned char *)addr,size);
    p_str = getenv(name);
    if(p_str == NULL)
    {
        savesum = 0;
    }
    else
    {
        savesum = simple_strtoul(p_str,NULL,16);
    }
    if(checksum == savesum)
    {
        UBOOT_DEBUG("## CRC check done, %s\n", name);
        ret = 0;
    }
    else
    {
        UBOOT_ERROR("checksum :0X%x ; savesum :0X%x\n",checksum,savesum);
        UBOOT_ERROR("## CRC check Error, %s\n", name);
        setenv("db_table","0");
        saveenv();
    }
    return ret;
}
#if ENABLE_DISPLAY_LOGO
int Load_LogoToFlash(void)
{
    int ret = -1;
    U32 u32BinAddr = (U32)(PA2NVA(LOGO_JPG_DRAM_ADDR));
    U32 u32fileSize=0;
    UBOOT_TRACE("IN\n");
#ifdef KONKA_BOOT_CFG
    vfs_mount(CUSTOMER_KONKA);
#else
    vfs_mount(CUSTOMER);
#endif
    u32fileSize=vfs_getsize(gLogoPath);
    UBOOT_DEBUG("u32fileSize=0x%x\n",u32fileSize);
    UBOOT_DEBUG("db_table.logo_size=0x%x\n",db_table.music_size);
    
    if (vfs_read((void *)u32BinAddr,gLogoPath,0,u32fileSize) == 0)
    {
        count_data_crc32_setenv(u32BinAddr,db_table.logo_size,"logo_crc");
        ret = raw_write(u32BinAddr,db_table.logo_offset,db_table.logo_size);
        if(ret == -1)
        {
            UBOOT_ERROR("write data to flash fail...>>>\n");
        }
        else
        {
            UBOOT_TRACE("OK\n");
        }
    }
    else
    {
        UBOOT_ERROR("ubifsload data fail ...>>>\n");
    }
    return ret;
}
int Read_LogoToDisplayAddr(U32 u32LogoAddr)
{
    int ret = -1;
    UBOOT_DEBUG("db_table.logo_offset =:0x%x ... db_table.logo_size=: 0x%x\n",db_table.logo_offset,db_table.logo_size);
    if(-1 == raw_read(u32LogoAddr,db_table.logo_offset,db_table.logo_size))
    {
        UBOOT_ERROR("ERROR: Read_LogoToDisplayAddr fail...>>>\n");
    }
    else
    {
        UBOOT_DEBUG("Read_LogoToDisplayAddr success...>>>\n");
        //crc check
        if(count_data_crc32_getenv(u32LogoAddr,db_table.logo_size,"logo_crc")==-1)
        {
            UBOOT_ERROR("check LogoToDisplayAddr fail...>>>\n");
            return ret;
        }
        else
        {
            ret = 0;
        }
    }
    return ret;
}
#endif

#if ENABLE_POWER_MUSIC
int Load_MusicToFlash(void)
{
    int ret = -1;
    U32 u32MusicAddr = (U32)(PA2NVA(AUDIO_ES1_ADR));
    U32 u32fileSize=0;
    UBOOT_TRACE("IN\n");
#ifdef KONKA_BOOT_CFG
    vfs_mount(CUSTOMER_KONKA);
#else
    vfs_mount(CUSTOMER);
#endif
    u32fileSize=vfs_getsize(gMusicPath);
    UBOOT_DEBUG("u32fileSize=0x%x\n",u32fileSize);
    UBOOT_DEBUG("db_table.music_size=0x%x\n",db_table.music_size);
    if (vfs_read((void *)u32MusicAddr,gMusicPath,0,u32fileSize) == 0)
    {
        count_data_crc32_setenv(u32MusicAddr,db_table.music_size,"music_crc");
        UBOOT_INFO("music_offset = 0x%x music_size = 0x%x\n",db_table.music_offset,db_table.music_size);
        ret = raw_write(u32MusicAddr,db_table.music_offset,db_table.music_size);
        if(ret == -1)
        {
            UBOOT_ERROR("write data to flash fail...>>>\n");
        }
        else
        {
            UBOOT_TRACE("OK\n");
        }
    }
    else
    {
        UBOOT_ERROR("load mp3 file faile>>>>>>>>>>\n");
    }
    return ret;
}

int Read_MusicToPlayAddr(MS_U32 u32MusicDramAddr)
{
    int ret = -1;
    U32 u32MusicAddr = (U32)(PA2NVA(u32MusicDramAddr));
    // Clear ES1 buffer = 64K
    memset((void*)(u32MusicAddr), 0, 65536);
    UBOOT_DEBUG("Read_>>>\n");
    if(-1 == raw_read(u32MusicAddr,db_table.music_offset,db_table.music_size))
    {
        UBOOT_ERROR("ERROR: Read_MusicToPlayAddr fail...>>>\n");
    }
    else
    {
        UBOOT_DEBUG("Read_MusicToPlayAddr success...>>>\n");
        UBOOT_DEBUG("Read_MusicToPlayAddr success...>>> 2\n");
        //crc check
        if(count_data_crc32_getenv(u32MusicAddr,db_table.music_size,"music_crc")==-1)
        {
            return ret;
        }
        else
        {
            ret = 0;
        }
    }
    return ret;
}

#endif

#if (CONFIG_PANEL_INIT)
int parse_pnl_ini(char *path,PanelType *p_data)
{
    long n;
    char *script = NULL;
    char str[BUFFER_SIZE];
    U32 filesize = 0;
    script = loadscript(path,&filesize);
    if(script == NULL)
    {
        while (*path== '/') path++;
        path=strstr(path,"/");
        UBOOT_INFO("filter /config, and find file again!!\n");
        script = loadscript(path,&filesize);
        if(script == NULL)
        {
            return -1;
        }
    }
    UBOOT_INFO("sizeof( panel ini script ) --------> %u bytes\n",filesize);
    Profile_Init((char *)script,filesize);
    n = Profile_GetInteger("panel", "m_bPanelDither", -1);
    UBOOT_DEBUG("m_bPanelDither = %ld \n",n);
    p_data->m_bPanelDither = n;

    n = Profile_GetInteger("panel", "m_ePanelLinkType", -1);
    UBOOT_DEBUG("m_ePanelLinkType = %ld \n",n);
    p_data->m_ePanelLinkType = (APIPNL_LINK_TYPE)n;
    n = Profile_GetInteger("panel", "m_bPanelDualPort", -1);
    UBOOT_DEBUG("m_bPanelDualPort = %ld \n",n);
    p_data->m_bPanelDualPort = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapPort", -1);
    UBOOT_DEBUG("m_bPanelSwapPort  = %ld \n",n);
    p_data->m_bPanelSwapPort = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapOdd_ML", -1);
    UBOOT_DEBUG("m_bPanelSwapOdd_ML = %ld \n",n);
    p_data->m_bPanelSwapOdd_ML  = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapEven_ML", -1);
    UBOOT_DEBUG("m_bPanelSwapEven_ML = %ld \n",n);
    p_data->m_bPanelSwapEven_ML = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapOdd_RB", -1);
    UBOOT_DEBUG("m_bPanelSwapOdd_RB = %ld \n",n);
    p_data->m_bPanelSwapOdd_RB = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapEven_RB", -1);
    UBOOT_DEBUG("m_bPanelSwapEven_RB = %ld \n",n);
    p_data->m_bPanelSwapEven_RB = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapLVDS_POL", -1);
    UBOOT_DEBUG("m_bPanelSwapLVDS_POL = %ld \n",n);
    p_data->m_bPanelSwapLVDS_POL = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapLVDS_CH", -1);
    UBOOT_DEBUG("m_bPanelSwapLVDS_CH = %ld \n",n);
    p_data->m_bPanelSwapLVDS_CH = n;
    n = Profile_GetInteger("panel", "m_bPanelPDP10BIT", -1);
    UBOOT_DEBUG("m_bPanelPDP10BIT = %ld \n",n);
    p_data->m_bPanelPDP10BIT = n;
    n = Profile_GetInteger("panel", "m_bPanelLVDS_TI_MODE", -1);
    UBOOT_DEBUG("m_bPanelLVDS_TI_MODE = %ld \n",n);
    p_data->m_bPanelLVDS_TI_MODE = n;
    n = Profile_GetInteger("panel", "m_ucPanelDCLKDelay", -1);
    UBOOT_DEBUG("m_ucPanelDCLKDelay = %ld \n",n);
    p_data->m_ucPanelDCLKDelay = n;
    n = Profile_GetInteger("panel", "m_bPanelInvDCLK", -1);
    UBOOT_DEBUG("m_bPanelInvDCLK = %ld \n",n);
    p_data->m_bPanelInvDCLK = n;
    n = Profile_GetInteger("panel", "m_bPanelInvDE", -1);
    UBOOT_DEBUG("m_bPanelInvDE = %ld \n",n);
    p_data->m_bPanelInvDE = n;
    n = Profile_GetInteger("panel", "m_bPanelInvHSync", -1);
    UBOOT_DEBUG("m_bPanelInvHSync = %ld \n",n);
    p_data->m_bPanelInvHSync = n;
    n = Profile_GetInteger("panel", "m_bPanelInvVSync", -1);
    UBOOT_DEBUG("m_bPanelInvVSync = %ld \n",n);
    p_data->m_bPanelInvVSync = n;
    n = Profile_GetInteger("panel", "m_ucPanelDCKLCurrent", -1);
    UBOOT_DEBUG("m_ucPanelDCKLCurrent = %ld \n",n);
    p_data->m_ucPanelDCKLCurrent = n;
    n = Profile_GetInteger("panel", "m_ucPanelDECurrent", -1);
    UBOOT_DEBUG("m_ucPanelDECurrent = %ld \n",n);
    p_data->m_ucPanelDECurrent = n;
    n = Profile_GetInteger("panel", "m_ucPanelODDDataCurrent", -1);
    UBOOT_DEBUG("m_ucPanelODDDataCurrent = %ld \n",n);
    p_data->m_ucPanelODDDataCurrent = n;
    n = Profile_GetInteger("panel", "m_ucPanelEvenDataCurrent", -1);
    UBOOT_DEBUG("m_ucPanelEvenDataCurrent = %ld \n",n);
    p_data->m_ucPanelEvenDataCurrent = n;
    n = Profile_GetInteger("panel", "m_wPanelOnTiming1", -1);
    UBOOT_DEBUG("m_wPanelOnTiming1 = %ld \n",n);
    p_data->m_wPanelOnTiming1 = n;
    n = Profile_GetInteger("panel", "m_wPanelOnTiming2", -1);
    UBOOT_DEBUG("m_wPanelOnTiming2 = %ld \n",n);
    p_data->m_wPanelOnTiming2 = n;
    n = Profile_GetInteger("panel", "m_wPanelOffTiming1", -1);
    UBOOT_DEBUG("m_wPanelOffTiming1 = %ld \n",n);
    p_data->m_wPanelOffTiming1 = n;
    n = Profile_GetInteger("panel", "m_wPanelOffTiming2", -1);
    UBOOT_DEBUG("m_wPanelOffTiming2 = %ld \n",n);
    p_data->m_wPanelOffTiming2 = n;
    n = Profile_GetInteger("panel", "m_ucPanelHSyncWidth", -1);
    UBOOT_DEBUG("m_ucPanelHSyncWidth  = %ld \n",n);
    p_data->m_ucPanelHSyncWidth = n;
    n = Profile_GetInteger("panel", "m_ucPanelHSyncBackPorch", -1);
    UBOOT_DEBUG("m_ucPanelHSyncBackPorch  = %ld \n",n);
    p_data->m_ucPanelHSyncBackPorch = n;

    n = Profile_GetInteger("panel", "m_ucPanelVSyncWidth", -1);
    UBOOT_DEBUG("m_ucPanelVSyncWidth = %ld \n",n);
    p_data->m_ucPanelVSyncWidth = n;
    n = Profile_GetInteger("panel", "m_ucPanelVBackPorch", -1);
    UBOOT_DEBUG("m_ucPanelVBackPorch = %ld \n",n);
    p_data->m_ucPanelVBackPorch = n;

    n = Profile_GetInteger("panel", "m_wPanelHStart", -1);
    UBOOT_DEBUG("m_wPanelHStart = %ld \n",n);
    p_data->m_wPanelHStart = n;
    n = Profile_GetInteger("panel", "m_wPanelVStart", -1);
    UBOOT_DEBUG("m_wPanelVStart = %ld \n",n);
    p_data->m_wPanelVStart = n;
    n = Profile_GetInteger("panel", "m_wPanelWidth", -1);
    UBOOT_DEBUG("m_wPanelWidth = %ld \n",n);
    p_data->m_wPanelWidth = n;
    n = Profile_GetInteger("panel", "m_wPanelHeight", -1);
    UBOOT_DEBUG("m_wPanelHeight = %ld \n",n);
    p_data->m_wPanelHeight = n;

    n = Profile_GetInteger("panel", "m_wPanelMaxHTotal", -1);
    UBOOT_DEBUG("m_wPanelMaxHTotal = %ld \n",n);
    p_data->m_wPanelMaxHTotal = n;
    n = Profile_GetInteger("panel", "m_wPanelHTotal", -1);
    UBOOT_DEBUG("m_wPanelHTotal = %ld \n",n);
    p_data->m_wPanelHTotal = n;
    n = Profile_GetInteger("panel", "m_wPanelMinHTotal", -1);
    UBOOT_DEBUG("m_wPanelMinHTotal = %ld \n",n);
    p_data->m_wPanelMinHTotal = n;
    n = Profile_GetInteger("panel", "m_wPanelMaxVTotal", -1);
    UBOOT_DEBUG("m_wPanelMaxVTotal = %ld \n",n);
    p_data->m_wPanelMaxVTotal = n;
    n = Profile_GetInteger("panel", "m_wPanelVTotal", -1);
    UBOOT_DEBUG("m_wPanelVTotal = %ld \n",n);
    p_data->m_wPanelVTotal = n;

    n = Profile_GetInteger("panel", "m_wPanelMinVTotal", -1);
    UBOOT_DEBUG("m_wPanelMinVTotal = %ld \n",n);
    p_data->m_wPanelMinVTotal = n;
    n = Profile_GetInteger("panel", "m_dwPanelMaxDCLK", -1);
    UBOOT_DEBUG("m_dwPanelMaxDCLK = %ld \n",n);
    p_data->m_dwPanelMaxDCLK = n;
    n = Profile_GetInteger("panel", "m_dwPanelDCLK", -1);
    UBOOT_DEBUG("m_dwPanelDCLK = %ld \n",n);
    p_data->m_dwPanelDCLK = n;
    n = Profile_GetInteger("panel", "m_dwPanelMinDCLK", -1);
    UBOOT_DEBUG("m_dwPanelMinDCLK = %ld \n",n);
    p_data->m_dwPanelMinDCLK = n;
    n = Profile_GetInteger("panel", "m_wSpreadSpectrumStep", -1);
    UBOOT_DEBUG("m_wSpreadSpectrumStep = %ld \n",n);
    p_data->m_wSpreadSpectrumStep = n;
    n = Profile_GetInteger("panel", "m_wSpreadSpectrumSpan", -1);
    UBOOT_DEBUG("m_wSpreadSpectrumSpan = %ld \n",n);
    p_data->m_wSpreadSpectrumSpan = n;
    n = Profile_GetInteger("panel", "m_ucDimmingCtl", -1);
    UBOOT_DEBUG("m_ucDimmingCtl = %ld \n",n);
    p_data->m_ucDimmingCtl = n;
    n = Profile_GetInteger("panel", "m_ucMaxPWMVal", -1);
    UBOOT_DEBUG("m_ucMaxPWMVal = %ld \n",n);
    p_data->m_ucMaxPWMVal = n;
    n = Profile_GetInteger("panel", "m_ucMinPWMVal", -1);
    UBOOT_DEBUG("m_ucMinPWMVal = %ld \n",n);
    p_data->m_ucMinPWMVal = n;
    n = Profile_GetInteger("panel", "m_bPanelDeinterMode", -1);
    UBOOT_DEBUG("m_bPanelDeinterMode = %ld \n",n);
    p_data->m_bPanelDeinterMode = n;
    n = Profile_GetInteger("panel", "m_ucPanelAspectRatio", -1);
    UBOOT_DEBUG("m_ucPanelAspectRatio = %ld \n",n);
    p_data->m_ucPanelAspectRatio = (E_PNL_ASPECT_RATIO)n;

    n = Profile_GetInteger("panel", "m_u16LVDSTxSwapValue", -1);
    UBOOT_DEBUG("m_u16LVDSTxSwapValue = %ld \n",n);
    p_data->m_u16LVDSTxSwapValue = n;
    n = Profile_GetInteger("panel", "m_ucTiBitMode", -1);
    UBOOT_DEBUG("m_ucTiBitMode = %ld \n",n);
    p_data->m_ucTiBitMode = (APIPNL_TIBITMODE)n;

    n = Profile_GetInteger("panel", "m_ucOutputFormatBitMode", -1);
    UBOOT_DEBUG("m_ucOutputFormatBitMode = %ld \n",n);
    p_data->m_ucOutputFormatBitMode = (APIPNL_OUTPUTFORMAT_BITMODE)n;

    n = Profile_GetInteger("panel", "m_bPanelSwapOdd_RG", -1);
    UBOOT_DEBUG("m_bPanelSwapOdd_RG = %ld \n",n);
    p_data->m_bPanelSwapOdd_RG = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapEven_RG", -1);
    UBOOT_DEBUG("m_bPanelSwapEven_RG = %ld \n",n);
    p_data->m_bPanelSwapEven_RG = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapOdd_GB", -1);
    UBOOT_DEBUG("m_bPanelSwapOdd_GB = %ld \n",n);
    p_data->m_bPanelSwapOdd_GB = n;
    n = Profile_GetInteger("panel", "m_bPanelSwapEven_GB", -1);
    UBOOT_DEBUG("m_bPanelSwapEven_GB = %ld \n",n);
    p_data->m_bPanelSwapEven_GB = n;
    n = Profile_GetInteger("panel", "m_bPanelDoubleClk", -1);
    UBOOT_DEBUG("m_bPanelDoubleClk = %ld \n",n);
    p_data->m_bPanelDoubleClk = n;
    n = Profile_GetInteger("panel", "m_dwPanelMaxSET", -1);
    UBOOT_DEBUG("m_dwPanelMaxSET = %ld \n",n);
    p_data->m_dwPanelMaxSET = n;
    n = Profile_GetInteger("panel", "m_dwPanelMinSET", -1);
    UBOOT_DEBUG("m_dwPanelMinSET = %ld \n",n);
    p_data->m_dwPanelMinSET = n;
    n = Profile_GetInteger("panel", "m_ucOutTimingMode", -1);
    UBOOT_DEBUG("m_ucOutTimingMode = %ld \n",n);
    p_data->m_ucOutTimingMode = (APIPNL_OUT_TIMING_MODE)n;
#if 1//use pwm setting in panel.ini and save it to env
    memset(str,0,sizeof(str));
    n = Profile_GetInteger("panel", "u32PeriodPWM", -1);
    snprintf(str,sizeof(str),"0x%x",n);
    UBOOT_INFO("u32PeriodPWM = %ld \n",n);
    setenv(PWM_PERIOD,str);

    memset(str,0,sizeof(str));
    n = Profile_GetInteger("panel", "u16DivPWM", -1);
    snprintf(str,sizeof(str),"0x%x",n);
    UBOOT_DEBUG("u16DivPWM = %ld \n",n);
    setenv(PWM_DIVPWM,str);

    memset(str,0,sizeof(str));
    n = Profile_GetInteger("panel", "u32DutyPWM", -1);// Duty use Div value,fix it later
    snprintf(str,sizeof(str),"0x%x",n);
    UBOOT_INFO("u32DutyPWM = %s\n",str);
    setenv(PWM_DUTY,str);

    memset(str,0,sizeof(str));
    n = Profile_GetInteger("panel", "bPolPWM", -1);// Duty use polpwm value,fix it later
    snprintf(str,sizeof(str),"0x%x",n);
    UBOOT_DEBUG("bPolPWM = %s\n",str);
    setenv(PWM_POLPWM,str);

    memset(str,0,sizeof(str));
    n = Profile_GetInteger("panel", "u16MaxPWMvalue", -1);// Duty use u16MaxPWMvalue value,fix it later
    snprintf(str,sizeof(str),"0x%x",n);
    UBOOT_DEBUG("u16MaxPWMvalue = %s\n",str);
    setenv(PWM_MAXPWM,str);

    memset(str,0,sizeof(str));
    n = Profile_GetInteger("panel", "u16MinPWMvalue", -1);// Duty use u16MinPWMvalue value,fix it later
    snprintf(str,sizeof(str),"0x%x",n);
    UBOOT_DEBUG("u16MinPWMvalue = %s\n",str);
    setenv(PWM_MINPWM,str);
    memset(str,0,sizeof(str));

    n = Profile_GetInteger("panel", "mirrormode", -1);// Duty use Div value,fix it later
    snprintf(str,sizeof(str),"%u",n);
    UBOOT_INFO("mirrormode = %s\n",str);
    setenv(MIRROR_ON,str);

#if (ENABLE_MSTAR_BD_MST036B_EDISON_CUS19 == 1)
	memset(str,0,sizeof(str));
    n = Profile_GetInteger("panel", "u16PanelType", -1);
    snprintf(str,sizeof(str),"0x%x",n);
    printf("u16PanelType = %s\n",str);
    setenv(PANEL_TYPE,str);
#endif
#endif

#if (ENABLE_MSTAR_BD_MST036B_EDISON_CUS19 == 1)//copy from MS801:swing level by panel
	   n = Profile_GetInteger("PANEL_SWING_LEVEL", "SWING_LEVEL", -1);

	   if ((n>=40) && (n<=600))
		{
			snprintf(str,sizeof(str),"%u",n);
			UBOOT_INFO("swing_level -> [%s]\n",str);
			setenv("swing_level",str);
		}
		else
		{
			setenv("swing_level","220");
		}
#endif

    memset(str,0,sizeof(str));
    n = Profile_GetInteger("LocalDIMMING", "bLocalDIMMINGEnable", 0);
	snprintf(str,sizeof(str),"%u",n);
    UBOOT_INFO("bLocalDIMMINGEnable = %s\n",str);
    setenv(LOCALDIMMING,str);

    if(script)
        free(script);
    return 0;
}

int Load_PanelSetting_ToFlash(void)
{
    int ret = -1;
    PanelType pnl;
    memset(&pnl,0,sizeof(pnl));
#ifdef KONKA_BOOT_CFG
    vfs_mount(CUSTOMER_KONKA);
#else
    vfs_mount(CONFIG);
#endif
    if(parse_pnl_ini(gPnlPath,&pnl)!=0)
    {
        UBOOT_ERROR("parse_pnl_ini fail !!\n");
        return ret;
    }

    UBOOT_DEBUG("panel para size =: 0x%08x \n",sizeof(pnl));
    count_data_crc32_setenv((U32)&pnl,sizeof(pnl),"panel_crc");

    ret = raw_write((U32)&pnl,db_table.panelpara_offset,sizeof(pnl));
    if(ret == -1)
    {
        UBOOT_ERROR("write data to spiflash fail...>>>\n");
    }
    return ret;
}

int Read_PanelParaFromflash(PanelType * panel_data)
{
    int ret = -1;
    UBOOT_DEBUG("db_table.panelpara_offset =:0x%x...db_table.panelpara_size=:0x%x\n",db_table.panelpara_offset,db_table.panelpara_size);
    if(-1 == raw_read((U32)panel_data,db_table.panelpara_offset,db_table.panelpara_size))
    {
        UBOOT_ERROR("ERROR: Read_PanelParaFromflash fail...>>>\n");
    }
    else
    {
        UBOOT_DEBUG("Read_PanelParaFromflash success...>>>\n");
        if(count_data_crc32_getenv((U32)panel_data,db_table.panelpara_size,"panel_crc")==-1)
        {
            return ret;
        }
        else
        {
            ret = 0;
        }
    }
    UBOOT_DEBUG("panel_data.m_wPanelVStart=: 0x%x\n",panel_data->m_wPanelVStart);
    UBOOT_DEBUG("panel_data.m_wPanelWidth=: 0x%x\n",panel_data->m_wPanelWidth);
    UBOOT_DEBUG("panel_data.m_wPanelHeight=: 0x%x\n",panel_data->m_wPanelHeight);
    return ret;
}

int parse_board_ini(char *path, st_board_para *p_data)
{
    long n;
    char *script = NULL;
    U32 filesize = 0;
    script = loadscript(path,&filesize);
    if(script == NULL)
    {
        while (*path== '/') path++;
        path=strstr(path,"/");
        UBOOT_INFO("filter /config, and find file again!!\n");
        script = loadscript(path,&filesize);
        if(script == NULL)
        {
            return -1;
        }
    }

    UBOOT_INFO("sizeof( board ini script ) --------> %u bytes\n",filesize);
    Profile_Init((char *)script,filesize);

    n = Profile_GetInteger("PanelRelativeSetting", "m_u8BOARD_PWM_PORT", -1);
    UBOOT_DEBUG("m_u8BOARD_PWM_PORT = %ld \n",n);
    p_data->m_u8BOARD_PWM_PORT = n;

    n = Profile_GetInteger("PanelRelativeSetting", "m_u16BOARD_LVDS_CONNECT_TYPE", -1);
    UBOOT_DEBUG("m_u16BOARD_LVDS_CONNECT_TYPE = %ld \n",n);
    p_data->m_u16BOARD_LVDS_CONNECT_TYPE = n;

    n = Profile_GetInteger("PanelRelativeSetting", "m_bPANEL_PDP_10BIT", -1);
    UBOOT_DEBUG("m_bPANEL_PDP_10BIT = %ld \n",n);
    p_data->m_bPANEL_PDP_10BIT = n;

    n = Profile_GetInteger("PanelRelativeSetting", "m_bPANEL_SWAP_LVDS_POL", -1);
    UBOOT_DEBUG("m_bPANEL_SWAP_LVDS_POL = %ld \n",n);
    p_data->m_bPANEL_SWAP_LVDS_POL = n;

    n = Profile_GetInteger("PanelRelativeSetting", "m_bPANEL_SWAP_LVDS_CH", -1);
    UBOOT_DEBUG("m_bPANEL_SWAP_LVDS_CH = %ld \n",n);
    p_data->m_bPANEL_SWAP_LVDS_CH = n;

    n = Profile_GetInteger("PanelRelativeSetting", "m_bPANEL_CONNECTOR_SWAP_PORT", -1);
    UBOOT_DEBUG("m_bPANEL_CONNECTOR_SWAP_PORT = %ld \n",n);
    p_data->m_bPANEL_CONNECTOR_SWAP_PORT = n;

    n = Profile_GetInteger("PanelRelativeSetting", "m_u16LVDS_PN_SWAP_L", -1);
    UBOOT_DEBUG("m_u16LVDS_PN_SWAP_L = %ld \n",n);
    p_data->m_u16LVDS_PN_SWAP_L = n;

    n = Profile_GetInteger("PanelRelativeSetting", "m_u16LVDS_PN_SWAP_H", -1);
    UBOOT_DEBUG("m_u16LVDS_PN_SWAP_H = %ld \n",n);
    p_data->m_u16LVDS_PN_SWAP_H = n;

    if(script)
        free(script);
    return 0;
}

int Load_BoardSetting_ToFlash(void)
{
    int ret = -1;
    st_board_para stBoard;
    memset(&stBoard,0,sizeof(stBoard));
    vfs_mount(CONFIG);
    if(parse_board_ini(gBoardPath, &stBoard)!=0)
    {
        UBOOT_ERROR("parse_board_ini fail !!\n");
        return ret;
    }

    UBOOT_DEBUG("board para size =: 0x%08x \n",sizeof(stBoard));
    count_data_crc32_setenv((U32)&stBoard, sizeof(stBoard), "board_crc");

    ret = raw_write((U32)&stBoard, db_table.boardpara_offset, sizeof(stBoard));
    if(ret == -1)
    {
        UBOOT_ERROR("write data to spiflash fail...>>>\n");
    }
    return ret;
}

int Read_BoardParaFromflash(st_board_para * board_data)
{
    int ret = -1;
    UBOOT_DEBUG("db_table.boardpara_offset =:0x%x...db_table.boardpara_size=:0x%x\n",db_table.boardpara_offset,db_table.boardpara_size);
    if(-1 == raw_read((U32)board_data,db_table.boardpara_offset,db_table.boardpara_size))
    {
        UBOOT_ERROR("ERROR: Read_BoardParaFromflash fail...>>>\n");
    }
    else
    {
        UBOOT_DEBUG("Read_BoardParaFromflash success...>>>\n");
        if(count_data_crc32_getenv((U32)board_data,db_table.boardpara_size,"board_crc")==-1)
        {
            return ret;
        }
        else
        {
            ret = 0;
        }
    }
    return ret;
}

#endif

#if ENABLE_KEYPAD_CUST
static U8 parse_strhex_2_int(char *strhex)
{
    char *hexptr;
    U8 u8Result = 0;
    U8 u8Offset = 0;

    if (strlen(strhex) < 2)
        return 0;

    hexptr = strhex + 2; //ignore  "0x"

    u8Offset = 0;
    while (*hexptr != '\0')
    {
        u8Result = u8Result << u8Offset;
        if (*hexptr >= '0' && *hexptr <= '9')
            u8Result = u8Result + (*hexptr - '0');
        else if (*hexptr >= 'A' && *hexptr <= 'F')
            u8Result = u8Result + (*hexptr - 'A') + 10;
        else if (*hexptr >= 'a' && *hexptr <= 'f')
            u8Result = u8Result + (*hexptr - 'a') + 10;

        hexptr++;
        u8Offset += 4;
    }

    return u8Result;
}

static void parse_keypad_threshold(char *str, SAR_RegCfg *pdata)
{
    U8 u8Index = 0;
    char *sptr;
    char *pch;

    if (strlen(str) < 3)
        return ;

    sptr = str + 1; //ignore "{"
    sptr[strlen(sptr) - 2] = 0;
    pch = strtok(sptr, ", ");
    u8Index = 0;
    while (pch != NULL)
    {
        pdata->u8KeyThreshold[u8Index] = parse_strhex_2_int(pch);
        UBOOT_DEBUG("[parse_keypad_threshold] %s to 0x%x\n", pch, pdata->u8KeyThreshold[u8Index]);
        pch = strtok(NULL, ", ");
        u8Index++;
    }
}

static void parse_keypad_keycode(char *str, SAR_RegCfg *pdata)
{
    U8 u8Index;
    char *sptr;
    char *pch;

    if (strlen(str) < 3)
        return ;

    sptr = str + 1; //ignore "{"
    sptr[strlen(sptr) - 2] = 0;
    pch = strtok(sptr, ", ");
    u8Index = 0;
    while (pch != NULL)
    {
        pdata->u8KeyCode[u8Index] = parse_strhex_2_int(pch);
        UBOOT_DEBUG("[parse_keypad_keycode] %s to 0x%x\n", pch, pdata->u8KeyCode[u8Index]);
        pch = strtok(NULL, ", ");
        u8Index++;
    }
}


int parse_keypad_ini(char *path, st_keypad_para *kpdpdata, int *pcount)
{
#define SAR_CH_NUM 4

#define PARSE_TITLE "Keypad_Ext"

#define PARSE_ENABLE "bEnable"
#define PARSE_CHID "u8SARChID"
#define PARSE_UPBND "u8SARChUpBnd"
#define PARSE_LOWBND "u8SARChLoBnd"
#define PARSE_LEVELNUM "u8KeyLevelNum"
#define PARSE_THRESHOLD "u8KeyThreshold"
#define PARSE_KEYCODE "u8KeyCode"


    long n;
    char str[BUFFER_SIZE];
    char *script = NULL;
    U32 filesize = 0;
    SAR_RegCfg *pdata = kpdpdata->kpdCfg;

    script = loadscript(path,&filesize);
    if(script == NULL)
    {
        while (*path== '/') path++;
        path=strstr(path,"/");
        UBOOT_INFO("filter /config, and find file again!!\n");
        script = loadscript(path,&filesize);
        if(script == NULL)
        {
            return -1;
        }
    }
    UBOOT_INFO("sizeof( keypad ini script ) --------> %u bytes path=%s\n",filesize, path);
    Profile_Init((char *)script,filesize);

    //set keypad select
    U8 u8KeypadSelect = 1;
    U8 u8Ch = 0;
    char title[BUFFER_SIZE];
    char parse[BUFFER_SIZE];
    u8KeypadSelect = Profile_GetInteger("KEYPAD", "keypadSelect", 0);

    if (u8KeypadSelect > 0)
    {
        title[0] = 0;
        UBOOT_INFO(title, "%s_%d", PARSE_TITLE, u8KeypadSelect);

        for (u8Ch = 1; u8Ch <= SAR_CH_NUM; u8Ch++)
        {
            U8 u8ChID_Offset = 0;

            parse[0] = 0;
            UBOOT_INFO(parse, "%s_%d", PARSE_ENABLE, u8Ch);
            n=Profile_GetBoolean(title, parse,0);
            UBOOT_DEBUG("[KEYPAD PARSE] %s => %ld\n", parse, n));
            if (n == 0)
                u8ChID_Offset = Kpd_ChID_Offset;

            parse[0] = 0;
            UBOOT_INFO(parse, "%s_%d", PARSE_CHID, u8Ch);
            n=Profile_GetInteger(title, parse,0) + u8ChID_Offset;
            UBOOT_DEBUG("[KEYPAD PARSE] %s => 0x%lx\n", parse, n));
            pdata[*pcount].u8SARChID = (U8) n;

            parse[0] = 0;
            UBOOT_INFO(parse, "%s_%d", PARSE_UPBND, u8Ch);
            n=Profile_GetInteger(title, parse,0);
            UBOOT_DEBUG("[KEYPAD PARSE] %s => 0x%lx\n", parse, n));
            pdata[*pcount].u8UpBnd = (U8) n;

            parse[0] = 0;
            UBOOT_INFO(parse, "%s_%d", PARSE_LOWBND, u8Ch);
            n=Profile_GetInteger(title, parse,0);
            UBOOT_DEBUG("[KEYPAD PARSE] %s => 0x%lx\n", parse, n));
            pdata[*pcount].u8LoBnd = (U8) n;

            parse[0] = 0;
            UBOOT_INFO(parse, "%s_%d", PARSE_LEVELNUM, u8Ch);
            n=Profile_GetInteger(title, parse,0);
            UBOOT_DEBUG("[KEYPAD PARSE] %s => 0x%lx\n", parse, n));
            pdata[*pcount].u8KeyLevelNum = (U8) n;

            parse[0] = 0;
            UBOOT_INFO(parse, "%s_%d", PARSE_THRESHOLD, u8Ch);
            Profile_GetString(title, parse,"", str, sizearray(str));
            UBOOT_DEBUG("[KEYPAD PARSE] %s => %s\n", parse, str));
            parse_keypad_threshold(str, &pdata[*pcount]);

            parse[0] = 0;
            UBOOT_INFO(parse, "%s_%d", PARSE_KEYCODE, u8Ch);
            Profile_GetString(title, parse,"", str, sizearray(str));
            UBOOT_DEBUG("[KEYPAD PARSE] %s => %s\n", parse, str));
            parse_keypad_keycode(str, &pdata[*pcount]);

            *pcount = *pcount+1;
        }
    }

    //saveenv keypad select for 0: default setting, 1 ~ :setting for customer
    UBOOT_INFO("[KEYPAD Init] u8KeypadSelect=%d\n", u8KeypadSelect);

    kpdpdata->u8KeypadSelect = u8KeypadSelect;

    if(script)
        free(script);

    return 0;
}

int Load_KeypadSetting_ToFlash(void)
{
    int ret = -1;
    st_keypad_para kpadPara;
    int kpdCount = 0;
    memset(&kpadPara,0,sizeof(kpadPara));
#ifdef KONKA_BOOT_CFG
    vfs_mount(CUSTOMER_KONKA);
#else
    vfs_mount(CONFIG);
#endif
    if(parse_keypad_ini(gKeypadPath, &kpadPara, &kpdCount)!=0)
    {
        UBOOT_ERROR("parse_pnl_ini fail !!\n");
        return ret;
    }

    UBOOT_DEBUG("keypad para size =: 0x%08x \n",sizeof(kpadPara));

    ret = raw_write((U32)&kpadPara, db_table.keypad_offset, sizeof(kpadPara));
    if(ret == -1)
    {
        UBOOT_ERROR("write data to spiflash fail...>>>\n");
    }
    return ret;
}

int Read_KeypadParaFromflash(st_keypad_para *keypad_data)
{
    int ret = -1;
    UBOOT_DEBUG("db_table.keypad_offset =:0x%x...db_table.keypad_size=:0x%x\n",db_table.keypad_offset,db_table.keypad_size));
    if(-1 == raw_read((U32))keypad_data,db_table.keypad_offset,db_table.keypad_size))
    {
        UBOOT_ERROR("ERROR: Read_KeypadParaFromflash fail...>>>\n");
    }
    else
    {
        UBOOT_DEBUG("Read_PanelParaFromflash success...>>>\n");
    }

    return ret;
}
#endif

#if (ENABLE_URSA_6M30 == 1)
int iniparser_6m30_cmd(ursa_6m30_cmd_table * cmd_table)
{
    int ret = -1;
    char * script = NULL;
    char InPath[BUFFER_SIZE]={0};
    U32 filesize = 0;
    vfs_mount(CONFIG);
    snprintf(InPath,sizeof(InPath),"%s/panel/6m30_mode.ini",CUSTOMER_PATH_KONKA);
    script = loadscript(InPath,&filesize);
    if(script != NULL)
    {
        Profile_Init(script,filesize);
        cmd_table->URSA_6M30_LVDS_CHANNEL_SWAP_MODE = Profile_GetInteger("MISC_6M30_CFG", "6M30_LVDS_CHANNEL_SWAP_MODE", -1);
        cmd_table->URSA_6M30_IN_LVDS_TIMODE = Profile_GetInteger("MISC_6M30_CFG", "6M30_IN_LVDS_TIMODE", -1);
        cmd_table->URSA_6M30_IN_LVDS_TI_BITNUM = Profile_GetInteger("MISC_6M30_CFG", "6M30_IN_LVDS_TI_BITNUM", -1);
        cmd_table->URSA_6M30_OUT_LVDS_TIMODE = Profile_GetInteger("MISC_6M30_CFG", "6M30_OUT_LVDS_TIMODE", -1);
        cmd_table->URSA_6M30_OUT_LVDS_TI_BITNUM = Profile_GetInteger("MISC_6M30_CFG", "6M30_OUT_LVDS_TI_BITNUM", -1);
        cmd_table->URSA_6M30_OUT_LVDS_SWAP_POLARITY = Profile_GetInteger("MISC_6M30_CFG", "6M30_OUT_LVDS_SWAP_POLARITY", -1);

        cmd_table->URSA_6M30_SSC_SEL = Profile_GetInteger("MISC_6M30_CFG", "6M30_SSC_SEL", -1);
        cmd_table->URSA_6M30_SSC_ENABLE = Profile_GetInteger("MISC_6M30_CFG", "6M30_SSC_ENABLE", -1);
        cmd_table->URSA_6M30_SSC_FREQ = Profile_GetInteger("MISC_6M30_CFG", "6M30_SSC_FREQ", -1);
        cmd_table->URSA_6M30_SSC_PERCENT = Profile_GetInteger("MISC_6M30_CFG", "6M30_SSC_PERCENT", -1);

        UBOOT_DEBUG(" --->>> 6M30_LVDS_CHANNEL_SWAP_MODE is %x \n",cmd_table->URSA_6M30_LVDS_CHANNEL_SWAP_MODE);
        UBOOT_DEBUG(" --->>> 6M30_IN_LVDS_TIMODE is %x \n",cmd_table->URSA_6M30_IN_LVDS_TIMODE);
        UBOOT_DEBUG(" --->>> 6M30_IN_LVDS_TI_BITNUM is %x \n",cmd_table->URSA_6M30_IN_LVDS_TI_BITNUM);
        UBOOT_DEBUG(" --->>> 6M30_OUT_LVDS_TIMODE is %x \n",cmd_table->URSA_6M30_OUT_LVDS_TIMODE);
        UBOOT_DEBUG(" --->>> 6M30_OUT_LVDS_TI_BITNUM is %x \n",cmd_table->URSA_6M30_OUT_LVDS_TI_BITNUM);
        UBOOT_DEBUG(" --->>> 6M30_OUT_LVDS_SWAP_POLARITY is %x \n",cmd_table->URSA_6M30_OUT_LVDS_SWAP_POLARITY);

        UBOOT_DEBUG(" --->>> 6M30_SSC_SEL is %x \n",cmd_table->URSA_6M30_SSC_SEL);
        UBOOT_DEBUG(" --->>> 6M30_SSC_ENABLE is %x \n",cmd_table->URSA_6M30_SSC_ENABLE);
        UBOOT_DEBUG(" --->>> 6M30_SSC_FREQ is %x \n",cmd_table->URSA_6M30_SSC_FREQ);
        UBOOT_DEBUG(" --->>> 6M30_SSC_PERCENT is %x \n",cmd_table->URSA_6M30_SSC_PERCENT);
        ret = 0;
    }
    else
    {
        UBOOT_ERROR("ubifsload 6m30.ini fail...>>>\n");
    }
    free(script);
    return ret;
}
int Read_Ursa_6m30_Para(ursa_6m30_cmd_table * cmd_table)
{
    int ret = -1;
    UBOOT_INFO("db_table.ursa_6m30_offset =:0x%x...db_table.ursa_6m30_size=:0x%x\n",db_table.ursa_6m30_offset,db_table.ursa_6m30_size);

	if(-1 == raw_read((U32)cmd_table,db_table.ursa_6m30_offset,db_table.ursa_6m30_size))
    {
        UBOOT_ERROR("ERROR: Read_UrsaParaFromflash fail...>>>\n");
    }
    else
    {
        if(count_data_crc32_getenv((U32)cmd_table,db_table.ursa_6m30_size,"ursa_crc")==-1)
        {
            return ret;
        }
        else
        {
            ret = 0;
        }
    }

    UBOOT_DEBUG(" cmd_table.6M30_LVDS_CHANNEL_SWAP_MODE is %x \n",cmd_table->URSA_6M30_LVDS_CHANNEL_SWAP_MODE);
    UBOOT_DEBUG(" cmd_table.6M30_IN_LVDS_TIMODE is %x \n",cmd_table->URSA_6M30_IN_LVDS_TIMODE);
    UBOOT_DEBUG(" cmd_table.6M30_IN_LVDS_TI_BITNUM is %x \n",cmd_table->URSA_6M30_IN_LVDS_TI_BITNUM);
    UBOOT_DEBUG(" cmd_table.6M30_OUT_LVDS_TIMODE is %x \n",cmd_table->URSA_6M30_OUT_LVDS_TIMODE);
    UBOOT_DEBUG(" cmd_table.6M30_OUT_LVDS_TI_BITNUM is %x \n",cmd_table->URSA_6M30_OUT_LVDS_TI_BITNUM);
    UBOOT_DEBUG(" cmd_table.6M30_OUT_LVDS_SWAP_POLITY is %x \n",cmd_table->URSA_6M30_OUT_LVDS_SWAP_POLARITY);

    UBOOT_DEBUG(" cmd_table.6M30_SSC_SEL is %x \n",cmd_table->URSA_6M30_SSC_SEL);
    UBOOT_DEBUG(" cmd_table.6M30_SSC_ENABLE is %x \n",cmd_table->URSA_6M30_SSC_ENABLE);
    UBOOT_DEBUG(" cmd_table.6M30_SSC_FREQ is %x \n",cmd_table->URSA_6M30_SSC_FREQ);
    UBOOT_DEBUG(" cmd_table.6M30_SSC_PERCENT is %x \n",cmd_table->URSA_6M30_SSC_PERCENT);
    return ret;
}
int Load_Ursa6m30Para_ToFlash(void)
{
    int ret = -1;
    ursa_6m30_cmd_table cmd_table={0};
    if(iniparser_6m30_cmd(&cmd_table)==0)
    {
        UBOOT_DEBUG("6m30 para size =: 0x%08x \n",sizeof(cmd_table));
        UBOOT_DEBUG("db_table.ursa_6m30_offset =: 0x%x\n",db_table.ursa_6m30_offset);
        count_data_crc32_setenv((U32)&cmd_table,sizeof(cmd_table),"ursa_crc");

        ret = raw_write((U32)&cmd_table,db_table.ursa_6m30_offset,sizeof(cmd_table));
        if(ret == -1)
        {
            UBOOT_ERROR("write ursa_6m30 data to spiflash fail...>>>\n");
        }
    }
    else
    {
        UBOOT_ERROR("parser_6m30_cmd fail ...>>>\n");
    }

    return ret;
}

int Ursa_6M30_Setting(ursa_6m30_cmd_table * cmd_table)
{
    int ret = -1;
    MDrv_Ursa_6M30_ChanelSwap_Mode(cmd_table->URSA_6M30_LVDS_CHANNEL_SWAP_MODE);
    g_UrsaCMDGenSetting.g_InitInLvdsAll.bTIMode = cmd_table->URSA_6M30_IN_LVDS_TIMODE;
    g_UrsaCMDGenSetting.g_InitInLvdsAll.BitNums = cmd_table->URSA_6M30_IN_LVDS_TI_BITNUM;
    g_UrsaCMDGenSetting.g_OutLvdsMode.bTIMode = cmd_table->URSA_6M30_OUT_LVDS_TIMODE;
    g_UrsaCMDGenSetting.g_OutLvdsMode.PanelBitNums = cmd_table->URSA_6M30_OUT_LVDS_TI_BITNUM;
	g_UrsaCMDGenSetting.g_OutLvdsMode.bLvdsSwapPol = cmd_table->URSA_6M30_OUT_LVDS_SWAP_POLARITY;

    MDrv_Ursa_6M30_Data_Init();

    g_UrsaCMDGenSetting.g_SscFreqPercent.SscSel = cmd_table->URSA_6M30_SSC_SEL;
    g_UrsaCMDGenSetting.g_SscFreqPercent.SscEN = cmd_table->URSA_6M30_SSC_ENABLE;
    g_UrsaCMDGenSetting.g_SscFreqPercent.SscFreq = cmd_table->URSA_6M30_SSC_FREQ;
    g_UrsaCMDGenSetting.g_SscFreqPercent.SscPercent = cmd_table->URSA_6M30_SSC_PERCENT;

    if(MDrv_Ursa_6M30_3D_Cmd(UC_SET_SSC_FREQ_PERCENT)>0)
    {
        ret = 0;
    }

    return ret;
}
#endif

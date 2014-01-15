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
#include <malloc.h>
#include <ShareType.h>
#include <bootlogo/MsBootLogo.h>
#include <bootlogo/MsPoolDB.h>
#include <MsApiJpd.h>
#include <MsApiGEGOP.h>
#include <MsVfs.h>
#include <MsSystem.h>
#include <MsApiPanel.h>
#include <MsUboot.h>
#include <MsDebug.h>
#include <MsRawIO.h>
#include <CusConfig.h>

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#if defined (CONFIG_LOGO_STORE_IN_SPI_FLASH)
#define FLASH_DEFAULT_TARGET  E_RAW_DATA_IN_SPI
#define FLASH_DEFAULT_PARTITION SPI_DEFAULT_PARTITION
#define FLASH_DEFAULT_VOLUME SPI_DEFAULT_VOLUME
#elif defined (CONFIG_LOGO_STORE_IN_NAND_FLASH)
#define FLASH_DEFAULT_TARGET  E_RAW_DATA_IN_NAND
#define FLASH_DEFAULT_PARTITION NAND_DEFAULT_PARTITION
#define FLASH_DEFAULT_VOLUME NAND_DEFAULT_VOLUME
#elif defined  (CONFIG_LOGO_STORE_IN_MMC_FLASH)
#define FLASH_DEFAULT_TARGET  E_RAW_DATA_IN_MMC
#define FLASH_DEFAULT_PARTITION MMC_DEFAULT_PARTITION
#define FLASH_DEFAULT_VOLUME MMC_DEFAULT_VOLUME
#else
#error "please set the correct security storage!!\n"
#endif

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
extern db_struct db_table;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static BOOLEAN bdbtableReady=FALSE;
//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------
extern int snprintf(char *str, size_t size, const char *fmt, ...);

//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// @brief \b Function Name \b dbtable_init
// @brief \b Description   \b prepare the logo/music/panel/ursa information & data
// @return                 \b dbtable_init_Result
////////////////////////////////////////////////////////////////////////////////


int do_dbtable_init(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret =-1;
    int dbtable = 0;
    U32 u32LogoOffset = 0;
    char * p_str = NULL;
    UBOOT_TRACE("IN\n");

    ret = raw_io_config(FLASH_DEFAULT_TARGET,FLASH_DEFAULT_PARTITION,FLASH_DEFAULT_VOLUME);

    if(ret != 0)
    {       
        bdbtableReady=FALSE;
 		UBOOT_ERROR("raw_io_config setting fail!\n");
        return -1;
    }

    ret=MsApiChunkHeader_GetValue(CH_UBOOT_POOL_ROM_OFFSET,&u32LogoOffset);
    if(ret != 0)
    {
        bdbtableReady=FALSE;
        UBOOT_ERROR("get logo offset fail!\n");
        return -1;
    }

    UBOOT_DEBUG("u32LogoOffset=0x%x\n",(unsigned int)u32LogoOffset);
    if( (argc > 1) && (strcmp(argv[1],"1")==0))
    {
        UBOOT_DEBUG("sikp db_table\n");

        ret = write_dbtable(u32LogoOffset);
    }
    else
    {
        UBOOT_DEBUG("get db_table\n");
        p_str = getenv ("db_table");
        if(p_str != NULL)
        {

            dbtable = (int)simple_strtol(p_str, NULL, 10);
            UBOOT_DEBUG("db_table=%d\n",dbtable);
            if(dbtable == 0)
            {
        		ret =  write_dbtable(u32LogoOffset);

            }
            else
            {
                ret =  read_dbtable(u32LogoOffset);
            }
        }
        else
        {
            UBOOT_DEBUG("No db_table\n");
            ret =  write_dbtable(u32LogoOffset);
        }
    }
    if(ret!=0)
    {
        bdbtableReady=FALSE;
        UBOOT_ERROR("dbtable init fail\n");        
    }
    else
    {
        bdbtableReady=TRUE;
        UBOOT_TRACE("OK\n");        
    }
    return ret;
}

#if (CONFIG_LOGO_STORE_IN_MBOOT)
static int LogoInMboot2Dram(U32 Dram_Addr)
{
    int ret = -1;
    U32 u32LogoSize = 0;
    U32 u32LogoOffset = 0;
    UBOOT_TRACE("IN\n");
	Dram_Addr=PA2NVA(Dram_Addr);
	
    ret = mboot_raw_io_Config();

    if(ret != 0)
    {
        UBOOT_ERROR("raw_io_config setting fail!\n");
        return -1;
    }

    ret=MsApiChunkHeader_GetValue(CH_LOGO_ROM_OFFSET,&u32LogoOffset);
    if(ret!=0)
    {
        UBOOT_ERROR("get u32LogoOffset fail\n");
        return -1;
    }
    ret=MsApiChunkHeader_GetValue(CH_LOGO_ROM_SIZE,&u32LogoSize);
    if(ret!=0)
    {
        UBOOT_ERROR("get u32LogoSize fail\n");
        return -1;
    }

    if(raw_read(Dram_Addr,u32LogoOffset,u32LogoSize)!=0)
    {
       UBOOT_ERROR("LogoInMboot2Dram Fail!!\n");
       return -1;
    }

    UBOOT_TRACE("OK\n");
    return u32LogoSize;
}
#endif

static unsigned int Load_Photo2Dram(char *SrcPath,U32 Dram_Addr)
{
    U32 u32LogoSize=0;
    UBOOT_TRACE("IN\n");
    Dram_Addr=PA2NVA(Dram_Addr);
    UBOOT_DEBUG("PA2NVA(Dram_Addr)=0x%x\n",(unsigned int)Dram_Addr);

#if ENABLE_DISPLAY_LOGO
    if(SrcPath==NULL)
    {
        UBOOT_DEBUG("No SrcPath\n");
        if(-1 == Read_LogoToDisplayAddr(Dram_Addr)) //read logo from spi/emmc to dram
        {
            UBOOT_ERROR("Error: Read_LogoToDisplayAddr Fail\n");
            return 0;
        }
        u32LogoSize = db_table.logo_size;
        UBOOT_DEBUG("u32LogoSize=0x%x\n",(unsigned int)u32LogoSize);

    }
    else // from nand/emmc directly
#endif    
    {
        char PathBuf[128] = "\0";
        vfs_mount(CUSTOMER);
        snprintf(PathBuf, sizeof(PathBuf), "%s/%s",CUSTOMER_PATH,SrcPath);
        UBOOT_DEBUG("PathBuf=%s\n",PathBuf);
        u32LogoSize = vfs_getsize(PathBuf);
        UBOOT_DEBUG("u32LogoSize=0x%x\n",(unsigned int)u32LogoSize);
        vfs_read((void *)Dram_Addr,PathBuf,0,u32LogoSize);

    }
    UBOOT_TRACE("OK\n");
    return u32LogoSize;
}

void showLogo(LogoDispConfigParams stConfig,U32 JPD_OUTADDR)
{

	GFX_BufferInfo src_info, dst_info;
	GFX_DrawRect rect;
	MS_BOOL bHorStretch=stConfig.bHorStretch;
	MS_BOOL bVerStretch=stConfig.bVerStretch;
    UBOOT_TRACE("IN\n");

	src_info.u32Addr = JPD_OUTADDR;
    UBOOT_DEBUG("src_info.u32Addr=0x%x\n",(unsigned int)src_info.u32Addr);

    src_info.u32ColorFmt = GFX_FMT_YUV422;
    UBOOT_DEBUG("src_info.u32ColorFmt=0x%x\n",(unsigned int)src_info.u32ColorFmt);

    src_info.u32Width = (U32)JPG_GetAlignmentWidth();
    UBOOT_DEBUG("src_info.u32Width=0x%x\n",(unsigned int)src_info.u32Width);

    src_info.u32Height = (U32)JPG_GetAlignmentHeight();
    UBOOT_DEBUG("src_info.u32Height=0x%x\n",(unsigned int)src_info.u32Height);

    src_info.u32Pitch = (U32)JPG_GetAlignmentPitch()<<1;
    UBOOT_DEBUG("src_info.u32Pitch=0x%x\n",(unsigned int)src_info.u32Pitch);

	dst_info.u32Addr = GOP_BUFFER_ADDR;

    UBOOT_DEBUG("dst_info.u32Addr=0x%x\n",(unsigned int)dst_info.u32Addr);

    dst_info.u32ColorFmt = GFX_FMT_YUV422;
    UBOOT_DEBUG("dst_info.u32ColorFmt=0x%x\n",(unsigned int)dst_info.u32ColorFmt);

    dst_info.u32Width = (U32)((JPG_GetAlignmentWidth()+15)&0xFFFFF0);
    UBOOT_DEBUG("dst_info.u32Width=0x%x\n",(unsigned int)dst_info.u32Width);

    dst_info.u32Height = (U32)JPG_GetAlignmentHeight();
    UBOOT_DEBUG("dst_info.u32Height=0x%x\n",(unsigned int)dst_info.u32Height);

    dst_info.u32Pitch =  (U32)(((JPG_GetAlignmentPitch()+15)&0xFFFFF0)<<1);
    UBOOT_DEBUG("dst_info.u32Pitch=0x%x\n",(unsigned int)dst_info.u32Pitch);

	rect.srcblk.x = 0;
    UBOOT_DEBUG("rect.srcblk.x=0x%x\n",(unsigned int)rect.srcblk.x);

    rect.srcblk.y = 0;
    UBOOT_DEBUG("rect.srcblk.y=0x%x\n",(unsigned int)rect.srcblk.y);

    rect.srcblk.width = JPG_GetAlignmentWidth()/*u16Pitch*/;
    UBOOT_DEBUG("rect.srcblk.width=0x%x\n",(unsigned int)rect.srcblk.width);


    rect.srcblk.height = JPG_GetAlignmentHeight();
    UBOOT_DEBUG("rect.srcblk.height=0x%x\n",(unsigned int)rect.srcblk.height);

    rect.dstblk.x = 0;
    UBOOT_DEBUG("rect.dstblk.x=0x%x\n",(unsigned int)rect.dstblk.x);

    rect.dstblk.y = 0;
    UBOOT_DEBUG("rect.dstblk.y=0x%x\n",(unsigned int)rect.dstblk.y);

    rect.dstblk.width = ((JPG_GetAlignmentWidth()+15)&0xFFFFF0)/*u16Pitch*/;
    UBOOT_DEBUG("rect.dstblk.width=0x%x\n",(unsigned int)rect.dstblk.width);

    rect.dstblk.height = JPG_GetAlignmentHeight();
    UBOOT_DEBUG("rect.dstblk.height=0x%x\n",(unsigned int)rect.dstblk.height);

	MsApiDisplay_Logo(&rect,&src_info,&dst_info,bHorStretch,bVerStretch,stConfig.u16DispX,stConfig.u16DispY);
    UBOOT_TRACE("OK\n");
    //while(1);//jh bebug
}

int do_bootlogo (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    U32 u32LogoSize=0;
    U32 JPD_OUTADDR=0;
    char *PhotoPath=NULL;
    LogoDispConfigParams stConfig;
	UBOOT_TRACE("IN\n");
    if(argc > 4)
    {
        stConfig.u16DispX = simple_strtoul(argv[1], NULL, 10);
        stConfig.u16DispY = simple_strtoul(argv[2], NULL, 10);
        stConfig.bHorStretch = simple_strtoul(argv[3], NULL, 10);
        stConfig.bVerStretch = simple_strtoul(argv[4], NULL, 10);
    }
    else
    {
        stConfig.u16DispX = 0;
        stConfig.u16DispY = 0;
        stConfig.bHorStretch = 1;
        stConfig.bVerStretch = 1;
    }

    if(bdbtableReady!=TRUE)
    {
        UBOOT_ERROR("dbtable is not ready\n");
        return -1;
    }

    if(IsPanelReady()!=0)
    {
        UBOOT_ERROR("Panle is not ready\n");
        return -1;
    }
    UBOOT_DEBUG("u16DispX=0x%x,u16DispY=0x%x,bHorStretch=0x%x,bVerStretch=0x%x\n",stConfig.u16DispX,stConfig.u16DispY,stConfig.bHorStretch,stConfig.bVerStretch);
    if(argc > 5)
    {
       PhotoPath = argv[5];
       UBOOT_DEBUG("PhotoPath=%s\n",PhotoPath);
    }
#if (CONFIG_LOGO_STORE_IN_MBOOT)
    u32LogoSize = LogoInMboot2Dram(LOGO_JPG_DRAM_ADDR);
#else	
    u32LogoSize = Load_Photo2Dram(PhotoPath,LOGO_JPG_DRAM_ADDR);
#endif	

    UBOOT_DEBUG("u32LogoSize : 0x%x \n",u32LogoSize);
    if(u32LogoSize == 0)
    {
        UBOOT_ERROR("Fail: Load_Photo2Dram return size is 0 !!\n");
        return -1;
    }
    JPD_OUTADDR = MsApi_JPD_Decode(LOGO_JPG_DRAM_ADDR, u32LogoSize);
	showLogo(stConfig,JPD_OUTADDR);
	UBOOT_TRACE("OK\n");
    return 0;
}
void draw_jpg(BltConfigParams stConfig,U32 JPD_OUTADDR)
{

	GFX_BufferInfo src_info;
	GFX_Block Blk;
	
    UBOOT_TRACE("IN\n");

	src_info.u32Addr = JPD_OUTADDR;
    UBOOT_DEBUG("src_info.u32Addr=0x%x\n",(unsigned int)src_info.u32Addr);

    src_info.u32ColorFmt = GFX_FMT_YUV422;
    UBOOT_DEBUG("src_info.u32ColorFmt=0x%x\n",(unsigned int)src_info.u32ColorFmt);

    src_info.u32Width = (U32)JPG_GetAlignmentWidth();
    UBOOT_DEBUG("src_info.u32Width=0x%x\n",(unsigned int)src_info.u32Width);

    src_info.u32Height = (U32)JPG_GetAlignmentHeight();
    UBOOT_DEBUG("src_info.u32Height=0x%x\n",(unsigned int)src_info.u32Height);

    src_info.u32Pitch = (U32)JPG_GetAlignmentPitch()<<1;
    UBOOT_DEBUG("src_info.u32Pitch=0x%x\n",(unsigned int)src_info.u32Pitch);

	Blk.x = (U16)((stConfig.u16DispX+15)&0xFFFFF0);
	Blk.y = stConfig.u16DispY;
	Blk.width = (U16)((stConfig.u16DispW+15)&0xFFFFF0);;
	Blk.height = stConfig.u16DispH;
	MsApiDrawJPG(src_info,Blk);
    UBOOT_TRACE("OK\n");
    //while(1);//jh bebug
}

int do_draw_jpg (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	U32 u32LogoSize=0;
	U32 JPD_OUTADDR=0;
	char *PhotoPath=NULL;
	BltConfigParams stConfig;
	UBOOT_TRACE("IN\n");

	if(argc > 4)
	{
		stConfig.u16DispX = simple_strtoul(argv[1], NULL, 10);
		stConfig.u16DispY = simple_strtoul(argv[2], NULL, 10);
		stConfig.u16DispW = simple_strtoul(argv[3], NULL, 10);
		stConfig.u16DispH = simple_strtoul(argv[4], NULL, 10);
	}
	else
	{
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if(bdbtableReady!=TRUE)
	{
		UBOOT_ERROR("dbtable is not ready\n");
		return -1;
	}

	if(IsPanelReady()!=0)
	{
		UBOOT_ERROR("Panle is not ready\n");
		return -1;
	}
	UBOOT_DEBUG("u16DispX=0x%x,u16DispY=0x%x,u16DispW=0x%x,u16DispH=0x%x\n",stConfig.u16DispX,stConfig.u16DispY,stConfig.u16DispW,stConfig.u16DispH);
	if(argc > 5)
	{
	   PhotoPath = argv[5];
	   UBOOT_DEBUG("PhotoPath=%s\n",PhotoPath);
	}
	u32LogoSize = Load_Photo2Dram(PhotoPath,LOGO_JPG_DRAM_ADDR);
	UBOOT_DEBUG("u32LogoSize : 0x%x \n",u32LogoSize);
	if(u32LogoSize == 0)
	{
		UBOOT_ERROR("Fail: Load_Photo2Dram return size is 0 !!\n");
		return -1;
	}
	JPD_OUTADDR = MsApi_JPD_Decode(LOGO_JPG_DRAM_ADDR, u32LogoSize);
	draw_jpg(stConfig,JPD_OUTADDR);
	UBOOT_TRACE("OK\n");
	return 0;
}

#if (ENABLE_MODULE_POWER_MUSIC == 1)
#include <MsApiAudio.h>

int Load_Music2Dram(char *MusicPath,U32 Dram_Addr)
{
    U32 musicSize=0;
	UBOOT_TRACE("IN\n");
    if(MusicPath == NULL)
    {
        UBOOT_DEBUG("MusicPath is a null pointer\n");
        UBOOT_DEBUG("Dram_Addr=0x%x\n",Dram_Addr);
        //read music from spi/emmc to dram defalut logo
        if(Read_MusicToPlayAddr(Dram_Addr)!=0)
        {
            UBOOT_ERROR("Read_MusicToPlayAddr() failed\n");
            return -1;
        }
    }
    else // from nand/emmc directly
    {
        char PathBuf[128] = "\0";
        vfs_mount(CUSTOMER);
        snprintf(PathBuf, sizeof(PathBuf), "%s/%s",CUSTOMER_PATH,MusicPath);
        UBOOT_DEBUG("PathBuf=%s\n",PathBuf);
        musicSize = vfs_getsize(PathBuf);
        UBOOT_DEBUG("musicSize=0x%x,Dram_Addr=0x%x\n",(unsigned int)musicSize,(unsigned int)Dram_Addr);
        if(vfs_read((void *)(PA2NVA(Dram_Addr)),PathBuf,0,musicSize)==-1)
        {
            UBOOT_ERROR("vfs_read fail\n");
            return -1;
        }
    }
	UBOOT_TRACE("OK\n");
    return 0;
}


int do_bootmusic (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret = 0;
    char *MusicPath = NULL;
	UBOOT_TRACE("IN\n");
    if(argc > 1)
    {
       MusicPath = argv[1];
    }
    
    if(bdbtableReady!=TRUE)
    {
        UBOOT_ERROR("dbtable is not ready\n");
        return -1;
    }
    
    ret = Load_Music2Dram(MusicPath,AUDIO_ES1_ADR);
    if(ret==-1)
    {
        UBOOT_ERROR("load music to dram fail\n");
    }
    else
    {
        play_powermusic_main();
	    UBOOT_TRACE("OK\n");
    }
    return ret;
}
#endif



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
#include <exports.h>
#include <MsTypes.h>
#include <ShareType.h>
#include <MsVfs.h>
#include <MsString.h>
#include <MsDebug.h>
#include <MsRawIO.h>
#include <drvSERFLASH.h>
#include <MsApiSpi.h>
#include <CusRawIO.h>
#include <linux/list.h>

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MMC_SECTOR_ALIGNMENT             9//align 512 byte
#define MMC_SECTOR_SIZE (1<<MMC_SECTOR_ALIGNMENT)

#define PARTITION_NAME_LEN 32
#define VOLUME_NAME_LEN 32

typedef struct{
    EN_RAW_DATA_TARGET target;
    char partition[PARTITION_NAME_LEN];
    char volume[VOLUME_NAME_LEN];
    struct list_head list;
}ST_RAW_IO_CONFIG;


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static EN_RAW_DATA_TARGET enTarget=E_RAW_DATA_INVALED;
static SERFLASH_Info stSpiInfo;
static BOOLEAN spi_ready = FALSE;
/// partName is only used on NAND
static char partName[PARTITION_NAME_LEN];

/// volName is only used on NAND amd MMC
static char volName[VOLUME_NAME_LEN];
static struct list_head config_list;
static unsigned int count=0;
//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------
static int raw_read_spi(unsigned int addr, unsigned int offset, unsigned int len);
static int raw_write_spi(unsigned int addr, unsigned int offset, unsigned int len);
static int raw_read_nand(unsigned int addr, unsigned int offset, unsigned int len);
static int raw_write_nand(unsigned int addr, unsigned int offset, unsigned int len);
static int raw_read_mmc(unsigned int addr, unsigned int offset, unsigned int len);
static int raw_write_mmc(unsigned int addr, unsigned int offset, unsigned int len);

//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int do_raw_io_init(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    UBOOT_TRACE("IN\n");
    INIT_LIST_HEAD(&config_list);
    
#if (CONFIG_SPI)    
    if(getSpiInfo(&stSpiInfo)==0)
    {
       spi_ready=TRUE;
    }
#endif

    UBOOT_TRACE("OK\n");
    return 0;
}

int do_raw_io_status(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  ST_RAW_IO_CONFIG *stStackID=NULL;
  unsigned int m=0;
  UBOOT_TRACE("IN\n");
  UBOOT_INFO("===current config===\n");
  if(enTarget ==E_RAW_DATA_IN_SPI)
  {
        UBOOT_INFO("Target : SPI\n"); 
  }
  else if(enTarget ==E_RAW_DATA_IN_NAND)
  {        
      UBOOT_INFO("Target : NAND\n");       
  }
  else if(enTarget ==E_RAW_DATA_IN_MMC)
  {        
      UBOOT_INFO("Target : EMMC\n");       
  }
  UBOOT_INFO("Partition : %s\n",partName);
  UBOOT_INFO("Volume : %s\n",volName);
  
  UBOOT_INFO("===stack===\n"); 
  m=count;
  list_for_each_entry(stStackID,&config_list,list)
  {
    if(stStackID->target ==E_RAW_DATA_IN_SPI)
    {
        UBOOT_INFO("Target : SPI\n"); 
    }
    else if(stStackID->target ==E_RAW_DATA_IN_NAND)
    {        
        UBOOT_INFO("Target : NAND\n");       
    }
    else if(stStackID->target ==E_RAW_DATA_IN_MMC)
    {        
        UBOOT_INFO("Target : EMMC\n");       
    }
    
    UBOOT_INFO("Partition : %s\n",stStackID->partition);
    UBOOT_INFO("Volume : %s\n",stStackID->volume);
    UBOOT_INFO("count=0x%x\n",m);
    UBOOT_INFO("---\n");      
    m--;
  }
  UBOOT_TRACE("OK\n");
  return 0;
}

int do_raw_io_config_push(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  UBOOT_TRACE("IN\n");
  raw_io_config_push();
  UBOOT_TRACE("OK\n");
  return 0;
}

int do_raw_io_config_pop(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  UBOOT_TRACE("IN\n");
  raw_io_config_pop();
  UBOOT_TRACE("OK\n");
  return 0;
}


int do_raw_io_config(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  int ret=-1;
  UBOOT_TRACE("IN\n");
  UBOOT_DEBUG("argc=%d\n",argc);
  if(argc==1)
  {
      #if defined (CONFIG_ENV_IN_NAND)
      ret=raw_io_config(E_RAW_DATA_IN_NAND,NAND_DEFAULT_PARTITION,NAND_DEFAULT_VOLUME);
      #endif
      #if defined (CONFIG_ENV_IN_MMC)
      ret=raw_io_config(E_RAW_DATA_IN_MMC,MMC_DEFAULT_PARTITION,MMC_DEFAULT_VOLUME);
      #endif
      #if defined (CONFIG_ENV_IN_SERIAL)  
      ret=raw_io_config(E_RAW_DATA_IN_SPI,SPI_DEFAULT_PARTITION,SPI_DEFAULT_VOLUME);
      #endif
  }
  else 
  {
       if(strcmp(argv[1],"NAND")==0)
       {
         if(argc!=4)
         {
            cmd_usage(cmdtp);
            return -1;
         }
         ret=raw_io_config(E_RAW_DATA_IN_NAND,argv[2],argv[3]);
       }
       else if(strcmp(argv[1],"SPI")==0)
       {
         if(argc!=2)
         {
            cmd_usage(cmdtp);
            return -1;
         }
         ret=raw_io_config(E_RAW_DATA_IN_SPI,SPI_DEFAULT_PARTITION,SPI_DEFAULT_VOLUME);
       }
       else if (strcmp(argv[1],"MMC")==0)
       {
         if(argc!=3)
         {
            cmd_usage(cmdtp);
            return -1;
         }
         ret=raw_io_config(E_RAW_DATA_IN_MMC,MMC_DEFAULT_PARTITION,argv[2]);
       }
       else
       {
            cmd_usage(cmdtp);
            return -1;
       }
  }
  if(ret==0)
  {
     UBOOT_TRACE("OK\n");    
  }
  else
  {
     UBOOT_ERROR("raw io config fail\n");
  }
  return ret;
}

int do_raw_read(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret=0;
    unsigned int addr;
    unsigned int offset;
    unsigned int len;
    UBOOT_TRACE("IN\n");
    if(argc!=4)
    {
        cmd_usage(cmdtp);
        return -1;
    }
    addr=simple_strtoul(argv[1], 0, 16);
    offset=simple_strtoul(argv[2], 0, 16);    
    len=simple_strtoul(argv[3], 0, 16);    
    ret=raw_read(addr,offset,len);
    if(ret==-1)
    {
        UBOOT_ERROR("do raw_read fail\n");
    }
    else
    {
        UBOOT_TRACE("OK\n");
    }
    return ret;    
}

int do_raw_write(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret=0;
    unsigned int addr;
    unsigned int offset;
    unsigned int len;
    UBOOT_TRACE("IN\n");
    if(argc!=4)
    {
        cmd_usage(cmdtp);
        return -1;
    }
    addr=simple_strtoul(argv[1], 0, 16);
    offset=simple_strtoul(argv[2], 0, 16);    
    len=simple_strtoul(argv[3], 0, 16);    
    ret=raw_write(addr,offset,len);
    if(ret==-1)
    {
        UBOOT_ERROR("do raw_write fail\n");
    }
    else
    {
        UBOOT_TRACE("OK\n");
    }
    return ret;    
}

void raw_io_config_push()
{
    ST_RAW_IO_CONFIG *stStackID=NULL;
    UBOOT_TRACE("IN\n");
    stStackID=(ST_RAW_IO_CONFIG *)malloc(sizeof(ST_RAW_IO_CONFIG));
    if(stStackID == NULL)
    {
        UBOOT_ERROR("malloc ST_RAW_IO_CONFIG fail!!\n");
    }
    else
    {
        memset(stStackID,0,sizeof(ST_RAW_IO_CONFIG));
        INIT_LIST_HEAD(&stStackID->list);
        stStackID->target = enTarget;
        memcpy(stStackID->partition,partName,strlen(partName)+1);
        memcpy(stStackID->volume,volName,strlen(volName)+1); 
        list_add(&stStackID->list,&config_list);
        count++;
        UBOOT_TRACE("OK\n");
    }
}

void raw_io_config_pop(void)
{
    ST_RAW_IO_CONFIG *stStackID=NULL;
    UBOOT_TRACE("IN\n"); 

    if(list_empty(&config_list)==1)
    {
        UBOOT_ERROR("stack is empty\n");
        return;
    }
    stStackID=list_entry((config_list.next),ST_RAW_IO_CONFIG,list);
    enTarget = stStackID->target;
    memcpy(partName,stStackID->partition,strlen(stStackID->partition)+1);
    memcpy(volName,stStackID->volume,strlen(stStackID->volume)+1); 
    list_del(&stStackID->list);
    free(stStackID);
    count--;
    UBOOT_TRACE("OK\n");    
}

EN_RAW_DATA_TARGET get_raw_status(void)
{
    UBOOT_TRACE("IN\n");
    if(enTarget ==E_RAW_DATA_IN_SPI)
    {
        UBOOT_DEBUG("Target : SPI\n"); 
    }
    else if(enTarget ==E_RAW_DATA_IN_NAND)
    {        
        UBOOT_DEBUG("Target : NAND\n");       
    }
    else if(enTarget ==E_RAW_DATA_IN_MMC)
    {        
        UBOOT_DEBUG("Target : EMMC\n");       
    }
    UBOOT_DEBUG("Partition : %s\n",partName);
    UBOOT_DEBUG("Volume : %s\n",volName);
    UBOOT_TRACE("OK\n");
    return enTarget;
}

int mboot_raw_io_Config(void)
{

#if defined (CONFIG_MBOOT_IN_SPI_FLASH)
#define FLASH_DEFAULT_TARGET  E_RAW_DATA_IN_SPI
#define FLASH_DEFAULT_PARTITION SPI_DEFAULT_PARTITION
#define FLASH_DEFAULT_VOLUME SPI_DEFAULT_VOLUME
#elif defined (CONFIG_MBOOT_IN_NAND_FLASH)
#define FLASH_DEFAULT_TARGET  E_RAW_DATA_IN_NAND
#define FLASH_DEFAULT_PARTITION "MBOOT"
#define FLASH_DEFAULT_VOLUME UNNECESSARY_PARTITION
#elif defined  (CONFIG_MBOOT_IN_MMC_FLASH)  
#define FLASH_DEFAULT_TARGET  E_RAW_DATA_IN_MMC
#define FLASH_DEFAULT_PARTITION MMC_DEFAULT_PARTITION
#define FLASH_DEFAULT_VOLUME "MBOOT"
#else
#error "please set the correct Mboot storage!!\n"
#endif  

    return raw_io_config(FLASH_DEFAULT_TARGET,FLASH_DEFAULT_PARTITION,FLASH_DEFAULT_VOLUME);  
}

int raw_io_config(EN_RAW_DATA_TARGET target, char *partition, char *volume)
{
    int ret=0;
    UBOOT_TRACE("IN\n");

    if(partition==NULL)
    {
        UBOOT_ERROR("The input parameter 'partition' is a null pointer\n");
        return -1;
    }

    if(volume==NULL)
    {
        UBOOT_ERROR("The input parameter 'volume' is a null pointer\n");
        return -1;
    }

    if(strlen(partition)>=PARTITION_NAME_LEN)
    {
        UBOOT_ERROR("The partition len is over %x\n",PARTITION_NAME_LEN);
        return -1;
    }

    if(strlen(volume)>=VOLUME_NAME_LEN)
    {
        UBOOT_ERROR("The volume len is over %x\n",VOLUME_NAME_LEN);
        return -1;
    }
 
    if(target>=E_RAW_DATA_INVALED)
    {
        UBOOT_ERROR("wrong target\n");
        return -1;
    }
    else
    {
        enTarget=target;
        memset(partName,0,sizeof(partName));
        memset(volName,0,sizeof(volName));      
        switch(enTarget)
        {
            case E_RAW_DATA_IN_SPI:
                 UBOOT_DEBUG("raw data in spi\n");
                 if(spi_ready!=TRUE)
                 {
                    enTarget = E_RAW_DATA_INVALED;
                    ret = -1;
                 }
                 break;
             case E_RAW_DATA_IN_NAND:
                 UBOOT_DEBUG("raw data in nand\n"); 
                 memset(partName,0,sizeof(partName));
                 strcpy(partName,partition);
                 memset(volName,0,sizeof(volName));
                 strcpy(volName,volume);
                 break;
             case E_RAW_DATA_IN_MMC:
                 UBOOT_DEBUG("raw data in mmc\n");     
                 memset(volName,0,sizeof(volName));
                 strcpy(volName,volume);
                 break;
             default:
                 enTarget = E_RAW_DATA_INVALED;
                 UBOOT_ERROR("No availabe target\n");
                 ret = -1;
                 break;
        }
        
    }

    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("raw_io_config fail\n");
    }
    return ret;
}

int raw_read(unsigned int addr, unsigned int offset, unsigned int len)
{
    int ret = -1;
    UBOOT_TRACE("IN\n");
    UBOOT_DEBUG("addr : %x\n",(U32)addr);
    UBOOT_DEBUG("offset : %x\n",(U32)addr);
    UBOOT_DEBUG("len : %x\n",(U32)len);  
    if(len==0)
    {
        UBOOT_ERROR("len is zero\n");
        return -1;
    } 
    switch(enTarget)
    {
        case E_RAW_DATA_IN_SPI:
            UBOOT_DEBUG("raw data in spi\n");
            ret=raw_read_spi(addr,offset,len);
            break;
        case E_RAW_DATA_IN_NAND:
            UBOOT_DEBUG("raw data in nand\n");            
            ret=raw_read_nand(addr,offset,len);            
            break;
        case E_RAW_DATA_IN_MMC:
            UBOOT_DEBUG("raw data in mmc\n");                        
            ret=raw_read_mmc(addr,offset,len);            
            break;
        default:
            UBOOT_ERROR("No availabe target\n");
            break;
    }
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("read raw data fail\n");    
    }
    return ret;
}

int raw_write(unsigned int addr, unsigned int offset, unsigned int len)
{
    int ret = -1;
    UBOOT_TRACE("IN\n");
    if(len==0)
    {
        UBOOT_ERROR("len is zero\n");
        return -1;
    } 
    switch(enTarget)
    {
        case E_RAW_DATA_IN_SPI:
            UBOOT_DEBUG("raw data in spi\n");
            ret=raw_write_spi(addr,offset,len);
            break;
        case E_RAW_DATA_IN_NAND:
            UBOOT_DEBUG("raw data in nand\n");            
            ret=raw_write_nand(addr,offset,len);            
            break;
        case E_RAW_DATA_IN_MMC:
            UBOOT_DEBUG("raw data in mmc\n");                        
            ret=raw_write_mmc(addr,offset,len);            
            break;
        default:
            UBOOT_ERROR("No availabe target\n");
            break;
    }
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("write raw data fail\n");    
    }
    return ret;
}

static int raw_read_spi(unsigned int addr, unsigned int offset, unsigned int len)
{
    char *buffer=NULL;
    int ret=-1;
    UBOOT_TRACE("IN\n");
    
    buffer=malloc(CMD_BUF);
    if(buffer==NULL)
    {
        UBOOT_ERROR("malloc fail\n");
        return -1;
    }
    memset(buffer,0,CMD_BUF);

    flush_cache(addr, len);
    snprintf(buffer,CMD_BUF,"spi rdc %x %x %x",(unsigned int)addr,(unsigned int)offset, (unsigned int)len);
    UBOOT_DEBUG("cmd=%s\n",buffer);
    ret=run_command(buffer,0);
    if(ret==-1)
    {
        UBOOT_ERROR("read raw data from spi fail\n");
    }
    else
    {
        UBOOT_TRACE("OK\n");    
    }
    free(buffer);
    return ret;
}

static int raw_write_spi(unsigned int addr, unsigned int offset, unsigned int len)
{
    unsigned int  sector_index = 0;    
    unsigned int  sector_offset = 0;
    unsigned int  sector_left_len = 0;
    unsigned int  spi_offset=0;
    int ret=0;
    char *tempBuf=NULL;
    char *cmdBuf=NULL;

    UBOOT_TRACE("IN\n");

    cmdBuf=malloc(CMD_BUF);
    if(cmdBuf==NULL)
    {
        UBOOT_ERROR("malloc fail\n");
        return -1;
    }
    memset(cmdBuf,0,CMD_BUF);



    tempBuf=malloc(stSpiInfo.u32SecSize);
    if(tempBuf==NULL)
    {
        free(cmdBuf);
        UBOOT_ERROR("malloc fail\n");
        return -1;
    }
    memset(tempBuf,0,stSpiInfo.u32SecSize);
    
    sector_index = (offset/stSpiInfo.u32SecSize);
    sector_offset = (offset%stSpiInfo.u32SecSize);
    
    if((len+sector_offset)>stSpiInfo.u32SecSize)
    {
        UBOOT_DEBUG("The replace area across a sector\n");
        sector_left_len=stSpiInfo.u32SecSize-sector_offset;
    }
    else
    {
        UBOOT_DEBUG("The replace area doesn't across a sector\n");
        sector_left_len=len;
    }
    
    while(len>0)
    {
        UBOOT_DEBUG("sector_index=0x%x\n",sector_index);
        UBOOT_DEBUG("len=0x%x\n",len);        
        UBOOT_DEBUG("sector_left_len=0x%x\n",sector_left_len);                
        
        spi_offset=sector_index*stSpiInfo.u32SecSize;
        ret=raw_read_spi((unsigned int)tempBuf,spi_offset,stSpiInfo.u32SecSize);
        if(ret==-1)
        {
            UBOOT_ERROR("read data from spi fail\n");
            free(tempBuf);
            free(cmdBuf);
            return -1;
        }
         
        memcpy((void *)((unsigned int)tempBuf+sector_offset), (const void *)addr,sector_left_len);

        memset(cmdBuf,0,CMD_BUF);
        snprintf(cmdBuf,CMD_BUF,"spi wrc %x %x %x",tempBuf,spi_offset, stSpiInfo.u32SecSize);
        UBOOT_DEBUG("cmd=%s\n",cmdBuf);
        ret=run_command(cmdBuf,0);
        if(ret==-1)
        {
            UBOOT_ERROR("write raw data to spi fail\n");
            free(tempBuf);
            free(cmdBuf);
            return -1;
        }
        sector_index++;
        sector_offset=0;
        addr+=sector_left_len;
        len-=sector_left_len;
        sector_left_len=(len>stSpiInfo.u32SecSize)?stSpiInfo.u32SecSize:len;
    }
   
    free(tempBuf);
    free(cmdBuf);    
    UBOOT_TRACE("OK\n");
    return 0;
}

static int raw_read_nand(unsigned int addr, unsigned int offset, unsigned int len)
{
    char *buffer=NULL;
    int ret=0;
    UBOOT_TRACE("IN\n");
  
    buffer=malloc(CMD_BUF);
    if(buffer==NULL)
    {
        UBOOT_ERROR("malloc fail\n");
        return -1;
    }

    if(strcmp(volName,UNNECESSARY_PARTITION)!=0)
    { 
        memset(buffer,0,CMD_BUF);
        snprintf(buffer,CMD_BUF,"ubi part %s",partName);
        UBOOT_DEBUG("cmd=%s\n",buffer);
        ret=run_command(buffer,0);
        if(ret==-1)
        {
            UBOOT_ERROR("ubi part %s fail\n",partName);
            return -1;
        }

        memset(buffer,0,CMD_BUF);
        snprintf(buffer,CMD_BUF,"ubi partial_read %x %s %x %x",addr ,volName ,len, offset);
        UBOOT_DEBUG("cmd=%s\n",buffer);
        ret=run_command(buffer,0);
    }
    else
    {
        memset(buffer,0,CMD_BUF);
        snprintf(buffer,CMD_BUF,"nand read.e %x %s 0x%x",addr ,partName, len);
        UBOOT_DEBUG("cmd=%s\n",buffer);

        ret=run_command(buffer,0);        
    }
    if(ret==-1)
    {
        UBOOT_ERROR("ubi/nand read %s fail\n",volName);
    }
    else
    {
        UBOOT_TRACE("OK\n");
    }
    return ret;
}

int raw_write_nand(unsigned int addr, unsigned int offset, unsigned int len)
{
    char *buffer=NULL;
    int ret=0;

    UBOOT_TRACE("IN\n"); 

    buffer=malloc(CMD_BUF);
    if(buffer==NULL)
    {
        UBOOT_ERROR("malloc fail\n");
        return -1;
    }
    memset(buffer,0,CMD_BUF);
    snprintf(buffer,CMD_BUF,"ubi part %s",partName);
    UBOOT_DEBUG("cmd=%s\n",buffer);
    ret=run_command(buffer,0);
    if(ret==-1)
    {
        UBOOT_ERROR("ubi part %s fail\n",partName);
        return -1;
    }

    memset(buffer,0,CMD_BUF);
    snprintf(buffer,CMD_BUF,"ubi partial_write %x %s %x %x",addr ,volName ,len, offset);
    UBOOT_DEBUG("cmd=%s\n",buffer);
    ret=run_command(buffer,0);
    if(ret==-1)
    {
        UBOOT_ERROR("ubi write %s fail\n",volName);
        return -1;
    }

    UBOOT_TRACE("OK\n");
    return 0;

}

static int raw_read_mmc(unsigned int addr, unsigned int offset, unsigned int len)
{
    int ret=0;
    char *tempBuf=NULL;
    char *cmdBuf=NULL;
    unsigned int  sector_index = 0;    
    unsigned int  sector_offset = 0;
    unsigned int  sector_left_len = 0;
    unsigned int  mmc_offset=0;
    UBOOT_TRACE("IN\n");
 
    cmdBuf=malloc(CMD_BUF);
    if(cmdBuf==NULL)
    {
        UBOOT_ERROR("malloc for cmd buffer fail\n");
        return -1;
    }
    memset(cmdBuf,0,CMD_BUF);

    tempBuf=malloc(MMC_SECTOR_SIZE);
    if(tempBuf==NULL)
    {
        free(cmdBuf);
        UBOOT_ERROR("malloc for temp buffer fail\n");
        return -1;
    }
    memset(tempBuf,0,MMC_SECTOR_SIZE);
    sector_offset=offset%MMC_SECTOR_SIZE;
    sector_index=offset/MMC_SECTOR_SIZE;

    if((len+sector_offset)>MMC_SECTOR_SIZE)
    {
        UBOOT_DEBUG("The replace area across a sector\n");
        sector_left_len=MMC_SECTOR_SIZE-sector_offset;
    }
    else
    {
        UBOOT_DEBUG("The replace area doesn't across a sector\n");
        sector_left_len=len;
    }

    while(len>0)
    {
        UBOOT_DEBUG("sector_index=0x%x\n",sector_index);
        UBOOT_DEBUG("len=0x%x\n",len);        
        UBOOT_DEBUG("sector_left_len=0x%x\n",sector_left_len);                
        
        mmc_offset=sector_index*MMC_SECTOR_SIZE;
        snprintf(cmdBuf, CMD_BUF,"mmc read.p.continue 0x%X %s 0x%X 0x%X",(unsigned int)tempBuf,volName,mmc_offset,MMC_SECTOR_SIZE);
        UBOOT_DEBUG("cmd=%s\n",cmdBuf);
        ret=run_command(cmdBuf,0);
        if(ret==-1)
        {
            UBOOT_ERROR("read raw data from mmc fail\n");
            free(tempBuf);
            free(cmdBuf);
            return -1;
        }
        memcpy((void *)addr,tempBuf,sector_left_len);
        sector_index++;
        sector_offset=0;
        addr+=sector_left_len;
        len-=sector_left_len;
        sector_left_len=(len>MMC_SECTOR_SIZE)?MMC_SECTOR_SIZE:len;

    }

    free(tempBuf);
    free(cmdBuf);
    UBOOT_TRACE("OK\n");
    return 0;
}

static int raw_write_mmc(unsigned int addr, unsigned int offset, unsigned int len)
{
    int ret=0;
    char *tempBuf=NULL;
    char *cmdBuf=NULL;
    unsigned int  sector_index = 0;    
    unsigned int  sector_offset = 0;
    unsigned int  sector_left_len = 0;
    unsigned int  mmc_offset=0;
    UBOOT_TRACE("IN\n");
 
    cmdBuf=malloc(CMD_BUF);
    if(cmdBuf==NULL)
    {
        UBOOT_ERROR("malloc for cmd buffer fail\n");
        return -1;
    }
    memset(cmdBuf,0,CMD_BUF);

    tempBuf=malloc(MMC_SECTOR_SIZE);
    if(tempBuf==NULL)
    {
        free(cmdBuf);
        UBOOT_ERROR("malloc for temp buffer fail\n");
        return -1;
    }
    memset(tempBuf,0,MMC_SECTOR_SIZE);
    sector_offset=offset%MMC_SECTOR_SIZE;
    sector_index=offset/MMC_SECTOR_SIZE;

    if((len+sector_offset)>MMC_SECTOR_SIZE)
    {
        UBOOT_DEBUG("The replace area across a sector\n");
        sector_left_len=MMC_SECTOR_SIZE-sector_offset;
    }
    else
    {
        UBOOT_DEBUG("The replace area doesn't across a sector\n");
        sector_left_len=len;
    }

    while(len>0)
    {
        UBOOT_DEBUG("sector_index=0x%x\n",sector_index);
        UBOOT_DEBUG("len=0x%x\n",len);        
        UBOOT_DEBUG("sector_left_len=0x%x\n",sector_left_len);                
        
        mmc_offset=sector_index*MMC_SECTOR_SIZE;
        memset(cmdBuf,0,CMD_BUF);
        snprintf(cmdBuf, CMD_BUF,"mmc read.p.continue 0x%X %s 0x%X 0x%X",(unsigned int)tempBuf,volName,mmc_offset,MMC_SECTOR_SIZE);
        UBOOT_DEBUG("cmd=%s\n",cmdBuf);
        ret=run_command(cmdBuf,0);
        if(ret==-1)
        {
            UBOOT_ERROR("fread raw data from mmc fail\n");
            free(tempBuf);
            free(cmdBuf);
            return -1;
        }
        memcpy((void *)((unsigned int)tempBuf+sector_offset),(void *)addr,sector_left_len);
        memset(cmdBuf,0,CMD_BUF);
        snprintf(cmdBuf, CMD_BUF,"mmc write.p.continue 0x%X %s 0x%X 0x%X",(unsigned int)tempBuf,volName,mmc_offset,MMC_SECTOR_SIZE);
        UBOOT_DEBUG("cmd=%s\n",cmdBuf);
        ret=run_command(cmdBuf,0);
        if(ret==-1)
        {
            UBOOT_ERROR("write raw data to mmc fail\n");
            free(tempBuf);
            free(cmdBuf);
            return -1;
        }
        
        sector_index++;
        sector_offset=0;
        addr+=sector_left_len;
        len-=sector_left_len;
        sector_left_len=(len>MMC_SECTOR_SIZE)?MMC_SECTOR_SIZE:len;

    }

    free(tempBuf);
    free(cmdBuf);
    UBOOT_TRACE("OK\n");;
    return 0;
}


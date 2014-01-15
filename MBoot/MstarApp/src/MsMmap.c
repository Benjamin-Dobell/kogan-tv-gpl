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

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <common.h>
#include <command.h>
#include <config.h>
#include <ShareType.h>
#include <MsTypes.h>
#include <MsDebug.h>
#include <exports.h>
#include <MsVfs.h>
#include <MsMmap.h>
#include <CusConfig.h>
#include <MsUtility.h>
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MMAP_FILE_NAME "mmap.ini"
//12 ??? For a example
//#define E_MMAP_ID_PM51_USAGE_MEM_LEN                           0x0000200000
//'0x0000200000' has 12 characters.
#define NUM_LEN 12

typedef struct 
{
    U32    u32gID;
    U32    u32Addr;
    U32    u32Size;
    U8     u8Layer;
    U32    u32Align;
    U32    b_is_miu0;
}MMapInfo_s;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static U8 *mmap_buffer=NULL;
static U32 mmap_buffer_size=0;

static BOOLEAN is_mmap_ready=FALSE;
static U32 miu_interval=0;
static U32 dram_length=0;

//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------
extern int snprintf(char *str, size_t size, const char *fmt, ...);

//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------
int init_mmap(void);
int LoadMMAP(void);
int _get_mmap(U8 *buffer, char *id, MMapInfo_s *mmapInfo);
int get_mmap(char *id, MMapInfo_s *mmapInfo);
int list_mmap_id(void);


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


int do_get_mmap(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    U32 addr=0;
    U32 len=0;
    int ret=0;
    UBOOT_TRACE("IN\n");

    if(argc!=2)
    {
        cmd_usage(cmdtp);
        return -1;  
    }
    if(strncmp(argv[1], "-l", 2) == 0)
    {
        
        ret=list_mmap_id();
    }
    else
    {
        ret=get_addr_from_mmap(argv[1],&addr);
        if(ret!=0)
        {
            UBOOT_ERROR("get addresss fail\n");
            return -1;
        }
        ret=get_length_from_mmap(argv[1],&len);    
        if(ret!=0)
        {
            UBOOT_ERROR("get addresss fail\n");
            return -1;
        }    
        UBOOT_INFO("addr=0x%x\n",(unsigned int)addr);
        UBOOT_INFO("len=0x%x\n",(unsigned int)len);    
    }


    if(ret!=0)
    {
        UBOOT_ERROR("get_mmap fail\n");
    }
    else
    {
        UBOOT_TRACE("OK\n");
    }
    
    return 0;
}

int get_dram_length(U32 *length)
{
    UBOOT_TRACE("IN\n");
    if(length==NULL)
    {
        UBOOT_ERROR("The 'length' is a null pointer\n");
        return -1;
    }

    if(init_mmap()!=0)
    {
        UBOOT_ERROR("init mmap fail\n");
        return -1;
    }
    UBOOT_DEBUG("dram_length=0x%x\n",dram_length);
    *length=dram_length;
    UBOOT_TRACE("OK\n");    
    return 0;
    
      
}

int get_miu_interval(U32 *interval)
{
    UBOOT_TRACE("IN\n");
    if(interval==NULL)
    {
        UBOOT_ERROR("The 'interval' is a null pointer\n");
        return -1;
    }

    if(init_mmap()!=0)
    {
        UBOOT_ERROR("init mmap fail\n");
        return -1;
    }
    UBOOT_DEBUG("min_interval=0x%x\n",miu_interval);
    *interval=miu_interval;
    UBOOT_TRACE("OK\n");    
    return 0;
    
      
}
int get_addr_from_mmap(char *id, U32 *addr)
{
    MMapInfo_s mmapInfo;
    UBOOT_TRACE("IN\n");
    if((id==NULL)||(addr==NULL))
    {
        UBOOT_ERROR("One of the parameters is a null pointer\n");
        return -1;
    }
    UBOOT_DEBUG("id=%s\n",id);
    if(get_mmap(id,&mmapInfo)!=0)
    {
        UBOOT_ERROR("get mmap fail\n");
        return -1;
    }
    if(mmapInfo.b_is_miu0==0)
    {
        UBOOT_DEBUG("In MIU0\n");
        *addr=mmapInfo.u32Addr+miu_interval;
    }
    else
    {
        UBOOT_DEBUG("In MIU1\n");
        *addr=mmapInfo.u32Addr;
    }
    UBOOT_DEBUG("addr=0x%x\n",*addr);     
    UBOOT_TRACE("OK\n");
    return 0;
}

int get_length_from_mmap(char *id, U32 *len)
{
    MMapInfo_s mmapInfo;
    UBOOT_TRACE("IN\n");
    if((id==NULL)||(len==NULL))
    {
        UBOOT_ERROR("One of the parameters is a null pointer\n");
        return -1;
    }
    UBOOT_DEBUG("id=%s\n",id);    
    if(get_mmap(id,&mmapInfo)!=0)
    {
        UBOOT_ERROR("get mmap fail\n");
        return -1;
    }
    *len=mmapInfo.u32Size;
    UBOOT_DEBUG("len=0x%x\n",*len);    
    UBOOT_TRACE("OK\n");
    return 0;
}



int get_mmap(char *id, MMapInfo_s *mmapInfo)
{
    UBOOT_TRACE("IN\n");

    if((id==NULL)||(mmapInfo==NULL))
    {
        UBOOT_ERROR("One of the parameters is a null pointer\n");
        return -1;
    }
    
    if(init_mmap()!=0)
    {
        UBOOT_ERROR("init mmap fail\n");
        return -1;
    }

    _get_mmap(mmap_buffer,id,mmapInfo);
    
    UBOOT_TRACE("OK\n");
    return 0;
}

int init_mmap(void)
{
    char *str=NULL;
    char *ptr=NULL;
    UBOOT_TRACE("IN\n");
    if(is_mmap_ready==TRUE)
    {
        UBOOT_TRACE("OK\n");
        return 0;
    }
    
    if(LoadMMAP()!=0)
    {
        UBOOT_ERROR("load mmap fail\n");
        return -1;
    }

    str=malloc(CMD_BUF);
    if(str==NULL)
    {
        UBOOT_ERROR("malloc for str fail\n");
        return -1;
    }
    memset(str,0,CMD_BUF);

    //#define MIU_INTERVAL                0x0020000000
    ptr=strstr((char *)mmap_buffer,"MIU_INTERVAL");
    ptr=strstr(ptr,"0x");
    snprintf(str,NUM_LEN,"%s\n",ptr);
    UBOOT_DEBUG("str=%s\n",str);
    miu_interval=simple_strtoul(str, NULL, 16);
    UBOOT_DEBUG("miu_interval=0x%x\n",miu_interval);

    //#define MIU_DRAM_LEN                0x0020000000
    ptr=strstr((char *)mmap_buffer,"MIU_DRAM_LEN");
    ptr=strstr(ptr,"0x");
    snprintf(str,NUM_LEN,"%s\n",ptr);
    UBOOT_DEBUG("str=%s\n",str);
    dram_length=simple_strtoul(str, NULL, 16);
    UBOOT_DEBUG("dram_length=0x%x\n",dram_length);
    is_mmap_ready=TRUE;
    free(str);
    
    UBOOT_TRACE("OK\n");
    return 0;
}

int LoadMMAP(void)
{

    UBOOT_TRACE("IN\n");
  

    if(vfs_mount(CONFIG)!=0)
    {
        UBOOT_ERROR("mount %s fail\n",CONFIG);
        return -1;    
    }
    
    mmap_buffer_size=vfs_getsize(MMAP_FILE_NAME);
    if(mmap_buffer_size==0)
    {       
        UBOOT_ERROR("get the file size of %s fail\n",MMAP_FILE_NAME);
        return -1;
    }

    mmap_buffer=malloc(mmap_buffer_size);
    UBOOT_DEBUG("mmmap_buffer at 0x%x\n",(unsigned int)mmap_buffer);
    if(mmap_buffer==NULL)
    {
        UBOOT_ERROR("malloc for mmap_buffer fail\n");
        return NULL;
    }

    if(vfs_read((char *)mmap_buffer,MMAP_FILE_NAME,0,mmap_buffer_size)!=0)
    {
        free(mmap_buffer);
        mmap_buffer=NULL;
        UBOOT_ERROR("read %s fail\n",MMAP_FILE_NAME);
        return -1;
    }

    vfs_umount();
    

    UBOOT_TRACE("OK\n");    
    return 0;
}

int _get_mmap(U8 *buffer, char *id, MMapInfo_s *mmapInfo)
{
    char *str=NULL;
    char *ptr=NULL;
    UBOOT_TRACE("IN\n");
    if((buffer==NULL)||(id==NULL)||(mmapInfo==NULL))
    {
        UBOOT_ERROR("One of the input parameters is null pointer\n");
        return -1;
    }
    UBOOT_DEBUG("@@id=%s\n",id);
    str=malloc(CMD_BUF);
    if(str==NULL)
    {
        UBOOT_ERROR("malloc for str fail\n");
        return -1;
    }

    memset(str,0,CMD_BUF);
    snprintf(str,CMD_BUF,"%s_ADR",id);
    UBOOT_DEBUG("str=%s\n",str);
    ptr=pattern_search((char *)mmap_buffer,mmap_buffer_size,str);
    if(ptr!=NULL)
    {
        ptr=pattern_search(ptr,mmap_buffer_size-((unsigned int)ptr-(unsigned int)mmap_buffer+1),"0x");
    }
    if(ptr==NULL)
    {
        UBOOT_ERROR("get addr from mmap fail\n");
        return -1;
    }
    memset(str,0,CMD_BUF);
    snprintf(str,NUM_LEN,"%s\n",ptr);
    UBOOT_DEBUG("str=%s\n",str);
    mmapInfo->u32Addr=(U32)simple_strtol(str,NULL,16);

    memset(str,0,CMD_BUF);
    snprintf(str,CMD_BUF,"%s_LEN",id);
    UBOOT_DEBUG("str=%s\n",str);
    ptr=pattern_search((char *)mmap_buffer,mmap_buffer_size,str);
    if(ptr!=NULL)
    {
        ptr=pattern_search(ptr,mmap_buffer_size-((unsigned int)ptr-(unsigned int)mmap_buffer+1),"0x");
    }
    if(ptr==NULL)
    {
        UBOOT_ERROR("get length from mmap fail\n");
        return -1;
    }
    memset(str,0,CMD_BUF);
    snprintf(str,NUM_LEN,"%s\n",ptr);
    UBOOT_DEBUG("str=%s\n",str);
    mmapInfo->u32Size=(U32)simple_strtol(str,NULL,16);
    
    //For an example
    //#define E_MMAP_ID_PM51_USAGE_MEM_MEMORY_TYPE                   (MIU0 | TYPE_NONE | UNCACHE)
    memset(str,0,CMD_BUF);
    snprintf(str,CMD_BUF,"%s_MEMORY_TYPE",id);
    UBOOT_DEBUG("str=%s",str);
    ptr=pattern_search((char *)mmap_buffer,mmap_buffer_size,str);
    if(ptr!=NULL)
    {
        ptr=pattern_search(ptr,mmap_buffer_size-((unsigned int)ptr-(unsigned int)mmap_buffer+1),"(");
    }
    if(ptr==NULL)
    {
        UBOOT_ERROR("get memory type from mmap fail\n");
        return -1;
    }
    ptr++;
    if(memcmp(ptr, "MIU0", strlen("MIU0"))==0)
    {
        UBOOT_DEBUG("In MIU0\n");
        mmapInfo->b_is_miu0=1;
    }
    else
    {
        UBOOT_DEBUG("In MIU1\n");
        mmapInfo->b_is_miu0=0;    
    }
    
    mmapInfo->u32Align=0;
    mmapInfo->u32gID=0;    
    mmapInfo->u8Layer=0;      

    free(str);
    UBOOT_TRACE("OK\n");
    return 0;
}

int list_mmap_id(void)
{
    #define PATTERN_HEAD "/* E_MMAP_ID"
    #define PATTERN_TAIL "*/"
    char *pS=NULL,*pE=NULL;
    char *pBuf=NULL;
    char *strBuf=NULL;
    unsigned int buf_size=mmap_buffer_size;
    UBOOT_TRACE("IN\n");
    if(init_mmap()!=0)
    {
        UBOOT_ERROR("init mmap fail\n");
        return -1;
    }
    strBuf=malloc(CMD_BUF);
    if(strBuf==NULL)
    {
        UBOOT_ERROR("malloc for string buf fail\n");
        return -1;
    }
    memset(strBuf,0,CMD_BUF);
    
    pS=(char *)mmap_buffer;
    pBuf=pS;
    while(buf_size)
    {
        pS=pattern_search((char *)pBuf,buf_size,PATTERN_HEAD);
        if(pS!=NULL)
        {
            buf_size=buf_size-((unsigned int)pS-(unsigned int)pBuf+1);
            pE=pattern_search((char *)pS,buf_size,PATTERN_TAIL);    
            buf_size=buf_size-((unsigned int)pE-(unsigned int)pS+1);
        }
        if((pS==NULL)||(pE==NULL))
        {
            break;
        }
        pS+=3;//+3? skip  "/* "
        pBuf=pE+strlen(PATTERN_TAIL);        
        snprintf(strBuf,((unsigned int)pE-(unsigned int)pS),"%s",pS);
        UBOOT_INFO("%s\n",strBuf);
    }
    
    UBOOT_TRACE("OK\n");
    return 0;

}

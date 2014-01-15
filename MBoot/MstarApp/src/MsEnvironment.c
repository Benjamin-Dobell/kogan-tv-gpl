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
#include <exports.h>
#include <MsTypes.h>
#include <MsDebug.h>
#include <MsEnvironment.h>
#include <MsMmap.h>
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MMAP_ID_LXMEM "E_LX_MEM"
#define MMAP_ID_EMAC "E_EMAC_MEM"
#define MMAP_ID_LXMEM2 "E_LX_MEM2"
#define MMAP_ID_GE "E_MST_GEVQ"
#define MMAP_ID_GOP "E_MST_GOP_REGDMA"
#define MMAP_ID_PM51_USAGE "E_MMAP_ID_PM51_USAGE_MEM"
#define MMAP_ID_FLUSH_BUFFER "E_MMAP_ID_FLUSH_BUFFER"

#define ENV_DRAM_LEN "DRAM_LEN"
#define ENV_LX_MEM_LEN "LX_MEM"
#define ENV_EMAC_MEM_LEN "EMAC_MEM"
#define ENV_LX_MEM2 "LX_MEM2"
#define ENV_FLUSH_BUF_ADDR "BB_ADDR"
#define ENV_PM51_ADDR "PM51_ADDR"
#define ENV_PM51_LEN "PM51_LEN"



#define ENV_GE_MEM "GE_ADDR"
#define ENV_GE_LEN "GE_LEN"
#define ENV_GOP_MEM "GOP_ADDR"
#define ENV_GOP_LEN "GOP_LEN"

#define ENV_SYNC_MMAP "sync_mmap"

#if defined(__ARM__)
#define MIU1_LOGIC_ADR_OFFSET 0xA0000000
#else
#define MIU1_LOGIC_ADR_OFFSET 0x60000000
#endif

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
extern unsigned int cfg_env_offset;
//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------
extern int snprintf(char *str, size_t size, const char *fmt, ...);
//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------
static int del_cfg(char *source,char *delCfg);
static int set_info_exchange_cfg(void);
static int set_env_cfg(void);
static int set_panelinit_cfg(void);
static int syncMmap2bootargs(void);
static int _syncLen2bootargs(char *id, char *envName);
static int _syncAddr2bootargs(char *id, char *envName);
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

static int del_cfg(char *source,char *delCfg)
{
    char *substr = NULL;
    char *pPreEnvEnd = NULL;
   
    UBOOT_TRACE("IN\n");
    if(source==NULL)
    {
        UBOOT_ERROR("The input parameter 'source' is a null pointer\n");
        return -1;
    }

    if(delCfg==NULL)
    {
        UBOOT_ERROR("The input parameter 'delCfg' is a null pointer\n");
        return -1;
    }

    if(strlen(source)==0)
    {
        UBOOT_ERROR("The length of source is zero\n");
        return -1;
    }

       
    substr=strstr(source,delCfg);
    if(substr != NULL)
    {

        pPreEnvEnd = strchr(substr,' ');
        if(((unsigned int)pPreEnvEnd-(unsigned int)source+1)<strlen(source))
        {
            strcpy(substr,pPreEnvEnd+1); //the +1 is for skip space
        }
        else
        {
            UBOOT_DEBUG("This member is the last one in the bootargs\n");
            //clear the rst of size. 
            *(substr-1)='\0';
            memset(substr,0,strlen(substr));

        }
    }
    UBOOT_TRACE("OK\n");
    return 0;
}

int do_del_boogargs_cfg (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
   int ret;
   UBOOT_TRACE("IN\n");
   if(argc!=2)
   {
        return cmd_usage(cmdtp);
   }

   ret = del_bootargs_cfg(argv[1]);
   if(ret==0)
   {
        UBOOT_TRACE("OK\n");
   }
   else
   {
        UBOOT_ERROR("delete %s fail\n",argv[1]);
   }
   return ret;
}

int del_bootargs_cfg(char *delCfg)
{
    int ret=0;
    char *bootarg=NULL;
    char *OriArg = NULL;
    UBOOT_TRACE("IN\n");
    if(delCfg==NULL)
    {
        UBOOT_ERROR("The input parameter 'delCfg' is a null pointer\n");
        return -1;
    }

    bootarg = getenv("bootargs");
    if(bootarg==NULL)
    {
        UBOOT_ERROR("No env 'bootargs'\n");
        return -1;
    }

    OriArg=malloc(strlen(bootarg)+1);
    if(OriArg==NULL)
    {
        UBOOT_ERROR("malloc for tempBuf fail\n");
        return -1;
    }
    memset(OriArg,0,strlen(bootarg)+1);
    strcpy(OriArg,bootarg);
        
    ret=del_cfg(OriArg,delCfg);
    if(ret==0)
    {
        UBOOT_DEBUG("OriArg=%s\n",OriArg);
        setenv("bootargs", OriArg);
        saveenv();
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("delete %s fail\n",delCfg);
    }
    free(OriArg);    
    return 0;
}

int do_set_bootargs_cfg (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
   int ret;
   unsigned int prefixLen=0;
   unsigned int contentLen=0;
   char *buffer=NULL;
   UBOOT_TRACE("IN\n");
   if(argc!=3)
   {
        return cmd_usage(cmdtp);
   }
   prefixLen=strlen(argv[1]);
   contentLen=strlen(argv[2]);
   //2? for '=' and the end symbol
   #define BUFFER_SIZE (prefixLen+contentLen+2)
   buffer=malloc(BUFFER_SIZE);
   if(buffer==NULL)
   {
       UBOOT_ERROR("malloc fail\n");
       return -1;
   }
   snprintf(buffer,BUFFER_SIZE,"%s=%s",argv[1],argv[2]);
   UBOOT_DEBUG("buffer: %s\n",buffer);
   ret = set_bootargs_cfg(argv[1],buffer);
   if(ret==0)
   {
        UBOOT_TRACE("OK\n");
   }
   else
   {
        UBOOT_ERROR("delete %s fail\n",argv[1]);
   }
   free(buffer);
   #undef BUFFER_SIZE
   return ret;
}


int set_bootargs_cfg(char * prefix_cfg,char *setCfg)
{
    char *preCheck=NULL;
    char *bootargs = NULL;
    char *OriArg = NULL;
    char *NewArg = NULL;
    int NewArgLen = 0;
    UBOOT_TRACE("IN\n");

    if(prefix_cfg==NULL)
    {
        UBOOT_ERROR("The input parameter 'prefix_cfg' is a null pointer\n");
        return -1;
    }

    if(setCfg==NULL)
    {
        UBOOT_ERROR("The input parameter 'setCfg' is a null pointer\n");
        return -1;
    }

    //get origin bootargs
     bootargs = getenv("bootargs");
     if (bootargs == NULL)
     {
         UBOOT_ERROR("bootargs doesn't exist\n");
         return -1;
     }
     preCheck=strstr(bootargs,setCfg);
     if(preCheck!=0)
     {
        UBOOT_DEBUG("%s has already existed\n",setCfg);
        UBOOT_TRACE("OK\n");
        return 0;
     }

     OriArg = (char*)malloc(strlen(bootargs) + 1);
     if(OriArg == NULL)
     {
          UBOOT_ERROR("malloc for OriArg fail\n");
          return -1;
     }
     strncpy(OriArg, bootargs,strlen(bootargs)+1);
     UBOOT_DEBUG("OriArg=%s\n",OriArg);
     //if setCfg exist, delete it.
     del_cfg(OriArg,prefix_cfg);
     UBOOT_DEBUG("OriArg=%s\n",OriArg);
     //add the NewCfg to Arg's tail.
     NewArgLen = strlen(OriArg) + strlen(setCfg) + 2 ;
     NewArg = malloc(NewArgLen);

     if(NewArg==NULL)
     {
        free(OriArg);
        UBOOT_ERROR("malloc for NewArg fail\n");
        return -1;
     }
     UBOOT_DEBUG("OriArg=%s\n",OriArg);

     snprintf(NewArg, NewArgLen, "%s %s", OriArg, setCfg);
     setenv("bootargs", NewArg);
     saveenv();
     free(OriArg);
     free(NewArg);

     UBOOT_TRACE("IN\n");
     return 0;

}


int do_add_autotest (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){
    int ret = 0;
    UBOOT_TRACE("IN\n");
    ret=set_bootargs_cfg("autotest","autotest=true");
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("set autotest=ture to bootargs fail\n");
    }
    return ret;
}


int do_add_hsl (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){
    int ret = 0;
    UBOOT_TRACE("IN\n");
    ret=set_bootargs_cfg("hsl","hsl=true");
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("set hsl=true to bootargs fail\n");
    }
    return ret;
}




static int set_info_exchange_cfg(void)
{
    char* pInfo = getenv(INFO_EXCHANGE_CFG);
    UBOOT_TRACE("IN\n");
    if (!pInfo || strcmp(pInfo, INFO_EXCHANGE_STORAGE) != 0)
    {
        UBOOT_DEBUG("write %s=%s\n",INFO_EXCHANGE_CFG,INFO_EXCHANGE_STORAGE);
        setenv(INFO_EXCHANGE_CFG, INFO_EXCHANGE_STORAGE);
        saveenv();
    }
    UBOOT_TRACE("OK\n");
    return 0;
}

static int set_env_cfg(void)
{
   int ret=0;
   char *buf=NULL;
   UBOOT_TRACE("IN\n");
   
   buf=malloc(CMD_BUF);
   if(buf==NULL)
   {
       UBOOT_ERROR("malloc fail\n");
       return -1;
   }
   memset(buf,0,CMD_BUF);
   snprintf(buf,CMD_BUF,"ENV_VAR_OFFSET=0x%x", cfg_env_offset);
   UBOOT_DEBUG("cmd=%s\n",buf);
   ret=set_bootargs_cfg("ENV_VAR_OFFSET",buf);
   if(ret==-1)
   {
        free(buf);
        UBOOT_ERROR("set %s to bootargs fail\n",buf);
        return -1;
   }
   memset(buf,0,CMD_BUF);
   snprintf(buf,CMD_BUF,"ENV_VAR_SIZE=0x%x", CONFIG_ENV_VAR_SIZE);
   UBOOT_DEBUG("cmd=%s\n",buf);        
   ret=set_bootargs_cfg("ENV_VAR_SIZE",buf);    
   if(ret==-1)
   {
        free(buf);
        UBOOT_ERROR("set %s to bootargs fail\n",buf);
        return -1;
   } 
   ret=set_bootargs_cfg(ENV_CFG_PREFIX,ENV_CFG);
   if(ret==-1)
   {
        free(buf);
        UBOOT_ERROR("set %s to bootargs fail.\n",ENV_CFG);
        return -1;
   }
   free(buf);
   UBOOT_TRACE("OK\n");
   return ret;
}

#define LOGO_IN_MBOOT "BOOTLOGO_IN_MBOOT"
static int set_panelinit_cfg(void)
{
    int ret =0;
    UBOOT_TRACE("IN\n");

#ifdef CONFIG_PANEL_INIT
    UBOOT_DEBUG("do config panel init\n");
    ret = set_bootargs_cfg(LOGO_IN_MBOOT,LOGO_IN_MBOOT);
#else
    UBOOT_DEBUG("no config panel init\n");
    ret = del_bootargs_cfg(LOGO_IN_MBOOT);
#endif
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("set %s to bootargs fail\n",LOGO_IN_MBOOT);
    }
    return ret;
}

int do_set_bootargs( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    UBOOT_TRACE("IN\n");
    UBOOT_INFO("============= set bootargs ===============\n");
    set_info_exchange_cfg();
    set_panelinit_cfg();
    set_env_cfg();
    UBOOT_TRACE("OK\n");
    return 0;
}

static int syncMmap2bootargs(void)
{
    int ret=0;
    unsigned int addr=0;
    unsigned int len=0;
    char buf[CMD_BUF];
    unsigned int min_interval=0;
    
    UBOOT_TRACE("IN\n");
    
    //Sync LX_MEM
    ret=_syncLen2bootargs(MMAP_ID_LXMEM,ENV_LX_MEM_LEN);
    if(ret!=0)
    {
        UBOOT_ERROR("sync LX_MEM fail\n");
        return -1;
    }

    //Sync EMAC_MEM
    ret=_syncLen2bootargs(MMAP_ID_EMAC,ENV_EMAC_MEM_LEN);
    if(ret!=0)
    {
        UBOOT_ERROR("sync EMAC_MEM fail\n");
        return -1;
    }

    //Sync BB_ADDR
    ret=_syncAddr2bootargs(MMAP_ID_FLUSH_BUFFER,ENV_FLUSH_BUF_ADDR);
    if(ret!=0)
    {
        UBOOT_ERROR("sync BB_ADDR fail\n");
        return -1;
    }

    //Sync PM51_ADDR
    ret=_syncAddr2bootargs(MMAP_ID_PM51_USAGE,ENV_PM51_ADDR);
    if(ret!=0)
    {
        UBOOT_ERROR("sync PM51_ADDR fail\n");
        return -1;
    }

    //Sync PM51_LEN
    ret=_syncLen2bootargs(MMAP_ID_PM51_USAGE,ENV_PM51_LEN);
    if(ret!=0)
    {
        UBOOT_ERROR("sync PM51_LEN fail\n");
        return -1;
    }    

    //Sync LX_MEM2
    ret=get_miu_interval(&min_interval);
    if(ret!=0)
    {
        UBOOT_ERROR("get min interval fail\n");
        return -1;
    }
    
    ret=get_addr_from_mmap(MMAP_ID_LXMEM2, &addr); 
    if(ret!=0)
    {
        UBOOT_ERROR("get %s fail\n",MMAP_ID_LXMEM2);
        return -1;
    }
    
    ret=get_length_from_mmap(MMAP_ID_LXMEM2, &len); 
    if(ret!=0)
    {
        UBOOT_ERROR("get %s fail\n",MMAP_ID_LXMEM2);
        return -1;
    }
    memset(buf,0,sizeof(buf));
    snprintf(buf,CMD_BUF,"%s=0x%x,0x%x",ENV_LX_MEM2,(addr&(~min_interval))+MIU1_LOGIC_ADR_OFFSET,len);
    UBOOT_DEBUG("buf=%s\n",buf);
    ret=set_bootargs_cfg(ENV_LX_MEM2,buf);
    if(ret!=0)
    {
        UBOOT_ERROR("sync LX_MEM2 fail\n");
        return -1;
    }

    //Sync DRAM Length
    ret=get_dram_length(&len);
    if(ret!=0)
    {
        UBOOT_ERROR("get dram length fail\n");
        return -1;
    }

    memset(buf,0,sizeof(buf));
    snprintf(buf,CMD_BUF,"%s=0x%x",ENV_DRAM_LEN,len);
    UBOOT_DEBUG("buf=%s\n",buf);
    ret=set_bootargs_cfg(ENV_DRAM_LEN,buf);
    if(ret!=0)
    {
        UBOOT_ERROR("sync dram length fail\n");
        return -1;
    }
  
    UBOOT_TRACE("OK\n");    
    return 0;
}

static int _syncLen2bootargs(char *id, char *envName)
{
    int ret=0;
    unsigned int len=0;
    char buf[CMD_BUF];
    UBOOT_TRACE("IN\n");
    if(id==NULL)
    {
        UBOOT_ERROR("id is a null pointer\n");
        return -1;   
    }
    if(envName==NULL)
    {
        UBOOT_ERROR("envName is a null pointer\n");
        return -1;   
    }    
    
    ret=get_length_from_mmap(id, &len); 
    if(ret!=0)
    {
        UBOOT_ERROR("get %s fail\n",envName);
        return -1;
    }
    memset(buf,0,sizeof(buf));
    snprintf(buf,CMD_BUF,"%s=0x%x",envName,len);
    UBOOT_DEBUG("buf=%s\n",buf);
    ret=set_bootargs_cfg(envName,buf);
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("sync %s to bootargs fail\n",id);
    }
    return 0;
}

static int _syncAddr2bootargs(char *id, char *envName)
{
    int ret=0;
    unsigned int addr=0;
    char buf[CMD_BUF];
    unsigned int min_interval=0;
    UBOOT_TRACE("IN\n");
    if(id==NULL)
    {
        UBOOT_ERROR("id is a null pointer\n");
        return -1;   
    }
    if(envName==NULL)
    {
        UBOOT_ERROR("envName is a null pointer\n");
        return -1;   
    }    
    
    ret=get_addr_from_mmap(id, &addr); 
    if(ret!=0)
    {
        UBOOT_ERROR("get %s fail\n",envName);
        return -1;
    }
    ret=get_miu_interval(&min_interval);
    if(ret!=0)
    {
        UBOOT_ERROR("get min interval fail\n");
        return -1;
    }
    memset(buf,0,sizeof(buf));
    snprintf(buf,CMD_BUF,"%s=0x%x",envName,addr&(~min_interval));
    UBOOT_DEBUG("buf=%s\n",buf);
    ret=set_bootargs_cfg(envName,buf);
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("sync %s to bootargs fail\n",id);
    }
    return 0;
}


int setMmapInfo2Env(char *id_mmap, char *env_addr, char *env_len)
{
    char *buf=NULL;
    char *pEnv_addr=NULL;
    char *pEnv_len=NULL;    
    int ret=0;
    unsigned int addr=0;
    unsigned int len=0;
    UBOOT_TRACE("IN\n"); 

    if((id_mmap==NULL)||(env_addr==NULL)||(env_len==NULL))
    {
        UBOOT_ERROR("One of the parameters is null pointer\n");
        return -1;
    }
    UBOOT_DEBUG("id_mmap=%s\n",id_mmap);
    UBOOT_DEBUG("env_addr=%s\n",env_addr);    
    UBOOT_DEBUG("env_len=%s\n",env_len);        
    
    pEnv_addr=getenv(env_addr);
    pEnv_len=getenv(env_len);    
    if((pEnv_addr==NULL)||(pEnv_len==NULL))
    {
        buf=malloc(CMD_BUF);
        if(buf==NULL)
        {
            UBOOT_ERROR("malloc for buf fail\n");
            return -1;
        }
        memset(buf,0,CMD_BUF);
        
        ret=get_addr_from_mmap(id_mmap, &addr);
        if(ret!=0)
        {
            free(buf);
            UBOOT_ERROR("get %s addr fail\n",id_mmap);
            return -1;
        }

        ret=get_length_from_mmap(id_mmap, &len);
        if(ret!=0)
        {
            free(buf);
            UBOOT_ERROR("get %s len fail\n",id_mmap);
            return -1;
        }
        snprintf(buf,CMD_BUF,"setenv %s 0x%x;setenv %s 0x%x;saveenv",env_addr,addr,env_len,len);
        UBOOT_DEBUG("cmd=%s\n",buf);
        ret=run_command(buf,0);
        if(ret!=0)
        {
            free(buf);
            UBOOT_ERROR("set %s=%x to env fail\n",env_addr,addr);
            return -1;
        }
        free(buf);
    }
    else
    {
        UBOOT_DEBUG("%s has already existed\n",id_mmap);
    }

    UBOOT_TRACE("OK\n");        
    return 0;
}



int do_mmap_to_env( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret=0;
    UBOOT_TRACE("IN\n");
    if(argc!=4)
    {
        cmd_usage(cmdtp);
        return -1;
    }
    if((argv[1]==NULL)||(argv[2]==NULL)||(argv[3]==NULL))
    {
        UBOOT_ERROR("One of the parameters is null pointer\n");
        return -1;
    }
    ret=setMmapInfo2Env(argv[1],argv[2],argv[3]);
    if(ret!=0)
    {
        UBOOT_ERROR("handle %s fail\n",argv[1]);
        return -1;
    }
    
    UBOOT_TRACE("OK\n");        
    return 0; 
}


int do_sync_mmap_to_env( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret=0;
    char *buf=NULL;
    char *pEnv=NULL;
    unsigned char sync_mmap=0;
    UBOOT_TRACE("IN\n");
    if(argc!=1)
    {
        cmd_usage(cmdtp);
        return -1;
    }
    pEnv=getenv(ENV_SYNC_MMAP);
    if(pEnv!=NULL)
    {
        sync_mmap=simple_strtol(pEnv, NULL, 16);
    }
    else
    {
        sync_mmap=1;
    }

   if(sync_mmap==1)
   {
        //Sync bootargs
        ret=syncMmap2bootargs();
        if(ret==-1)
        {
            UBOOT_ERROR("sync the part of mmap info to bootargs fail\n");
            return -1;
        }
        
        buf=malloc(CMD_BUF);
        if(buf==NULL)
        {
            UBOOT_ERROR("malloc for buf fail\n");
            return -1;
        }

        //Sync GE address and len
        memset(buf,0,CMD_BUF);
        snprintf(buf,CMD_BUF,"m2e %s %s %s",MMAP_ID_GE,ENV_GE_MEM,ENV_GE_LEN);
        UBOOT_DEBUG("cmd=%s\n",buf);
        ret=run_command(buf,0);
        if(ret!=0)
        {
            free(buf);
            UBOOT_ERROR("handle %s fail\n",MMAP_ID_GE);
            return -1;
        }

        //Sync GOP address and len
        memset(buf,0,CMD_BUF);
        snprintf(buf,CMD_BUF,"m2e %s %s %s",MMAP_ID_GOP,ENV_GOP_MEM,ENV_GOP_LEN);
        UBOOT_DEBUG("cmd=%s\n",buf);
        ret=run_command(buf,0);
        if(ret!=0)
        {
            free(buf);
            UBOOT_ERROR("handle %s fail\n",MMAP_ID_GOP);
            return -1;
        }
        setenv(ENV_SYNC_MMAP,0);
        saveenv();
    }

    free(buf);
    UBOOT_TRACE("OK\n");        
    return 0; 
}



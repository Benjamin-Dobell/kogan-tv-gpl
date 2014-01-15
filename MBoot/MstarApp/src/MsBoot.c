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
#include <ShareType.h>
#include <MsBoot.h>
#include <MsUpgrade.h>
#include <uboot_mmap.h>
#include <MsDebug.h>
#include <apiGOP.h>
#include <gop/MsDrvGop.h>

#if defined (CONFIG_URSA_6M40) && defined(CONFIG_SET_4K2K_MODE)
#include <CusSystem.h>
#endif
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define ENV_UPGRADE_COMPLETE        "MstarUpgrade_complete"
#define ENV_UPGRADE_MODE            "upgrade_mode"

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------

int boot_mode_recovery(void)
{
    char *s = NULL;
    char cmd[128];
	char * p_str = NULL;
    memset(cmd, 0, 128);
	run_command("panel_init", 0);
	//add mirror set by liying 2013-8-13
	MsDrvGFX_Init();
    MsDrvGOP_Init();
	p_str = getenv ("MIRROR_ON");
    if(p_str != NULL)
    {
        if((int)simple_strtol(p_str, NULL, 10))
        {
            printf("<<set_mirror ON!!>>");
            MApi_GOP_GWIN_SetVMirror(TRUE);
           	MApi_GOP_GWIN_SetHMirror(TRUE);
        }
    }
	else
	{
		printf("MIRROR_ON is NULL\n");
		setenv("MIRROR_ON",0);
	}
	//end add


    s = getenv("recoverycmd");
    if ((s == NULL) || (strncmp(s, BOOT_COMMAND, 4) != 0))
    {
        snprintf(cmd, sizeof(cmd)-1, BOOT_COMMAND);
    }
    else
    {
        snprintf(cmd, sizeof(cmd)-1, s);
    }
    printf("%s: at %d\n", __func__, __LINE__);
#if defined (CONFIG_URSA_6M40) && defined(CONFIG_SET_4K2K_MODE)
    Set_4K2K_OP0();
#endif
    return run_command(cmd, 0);
}


#define ENABLE_MODULE_ENV_BOOT 1
#if (ENABLE_MODULE_ENV_BOOT == 1)
EN_BOOT_MODE get_boot_mode_from_env(void)
{
    char *buffer = NULL;
    buffer = (char*) malloc(CMD_BUF);
    char *cUpgradeComplete = getenv(ENV_UPGRADE_COMPLETE);
    char *cUpgradeMode = getenv(ENV_UPGRADE_MODE);
    ulong ulComplete = 0;

    if(cUpgradeComplete == NULL)
    {
        printf("mingwu_mboot------------>get_boot_mode_from_env-->cUpgradeComplete=NULL\n");
        memset(buffer, 0 , CMD_BUF);
        snprintf(buffer, CMD_BUF, "setenv %s 0", ENV_UPGRADE_COMPLETE);
        run_command(buffer, 0);  // run_command("setenv MstarUpgrade_complete 0");
        run_command("saveenv", 0);

        //Get environment variable "ENV_UPGRADE_COMPLETE" again if it was NULL originally.
        cUpgradeComplete = getenv(ENV_UPGRADE_COMPLETE);
        if(cUpgradeComplete == NULL)
        {
            //printf("Reset environment '%s' failed\n", ENV_UPGRADE_COMPLETE);
            free(buffer);
            return EN_BOOT_MODE_NORMAL;
        }
    }
    ulComplete = simple_strtoul (cUpgradeComplete, NULL, 10);
    if(ulComplete == 0 )
    {
        printf("mingwu_mboot------------>get_boot_mode_from_env-->ulComplete == 0\n");
        if(strncmp(cUpgradeMode, "usb",3) == 0)
        {
            printf("mingwu_mboot------------>get_boot_mode_from_env==->EN_BOOT_MODE_USB_UPGRADE\n");
            free(buffer);
            return EN_BOOT_MODE_USB_UPGRADE;
        }
        else if(strncmp(cUpgradeMode, "oad",3) == 0)
        {
            free(buffer);
            return EN_BOOT_MODE_OAD_UPGRADE;
        }
        else if(strncmp(cUpgradeMode, "net",3) == 0)
        {
            free(buffer);
            return EN_BOOT_MODE_NET_UPGRADE;
        }
        else if(strncmp(cUpgradeMode, "mboot",5) == 0)
        {
            free(buffer);
            return EN_BOOT_MODE_FORCEUGRADE;
        }
    }
    else
    {
        printf("mingwu_mboot------------>get_boot_mode_from_env-->ulComplete !!!!= 0\n");
        memset(buffer, 0 , CMD_BUF);
        snprintf(buffer, CMD_BUF, "setenv %s 0", ENV_UPGRADE_COMPLETE);
        run_command(buffer, 0);  // run_command("setenv MstarUpgrade_complete 0");
        run_command("saveenv", 0);
#if (!CONFIG_MSTAR_SILENT)
        printf("last upgrade complete\n");
#endif
       
    }
    free(buffer);
    return EN_BOOT_MODE_NORMAL;
}
#endif
    
#if (ENABLE_MODULE_BOOT_IR == 1)
EN_BOOT_MODE get_boot_mode_from_ir(void)
{
     U8 IRKey=0xFF;
     U8 IRFlag;
     EN_BOOT_MODE mode = EN_BOOT_MODE_UNKNOWN;
     //IRKey = MDrv_ReadByte(0x3DA9); // get IR code
     extern BOOLEAN msIR_GetIRKeyCode(U8 *pkey, U8 *pu8flag);
     msIR_GetIRKeyCode(&IRKey,&IRFlag);
     printf("fore uup IRKey [0x%x]\n",IRKey);
     switch(IRKey) // NOTE: read IR Key to decide what boot mode to go
    {
        case IR_RECOVERY_KEY:
            mode =EN_BOOT_MODE_RECOVERY;
            break;
        case IR_UPGRADEBOOTLOADER_KEY:
            mode =EN_BOOT_MODE_FORCEUGRADE;
            break;
        case IR_FORCEUGRADE_KEY:
            mode =EN_BOOT_MODE_USB_UPGRADE;
            break;
        case IR_UART_ON_KEY:
            mode =EN_BOOT_MODE_UARTON;
            break;
        default:
            return EN_BOOT_MODE_UNKNOWN;
            break;
    }
     return mode;
}
#endif

#if (ENABLE_MODULE_ANDROID_BOOT == 1 )

EN_BOOT_MODE get_boot_mode_from_mtd0(void)
{
    EN_BOOT_MODE mode = EN_BOOT_MODE_UNKNOWN;
    struct bootloader_message *p_msg = NULL;
    p_msg = (struct bootloader_message*)BOOT_MODE_TMP_BUF_ADDR;
    char cmd[128];
    memset(cmd, 0, sizeof(cmd));
#if (ENABLE_MODULE_MMC == 1)
    snprintf(cmd, sizeof(cmd)-1, "mmc read.p 0x%08lX misc 32", (unsigned long)BOOT_MODE_TMP_BUF_ADDR);
#else
    snprintf(cmd, sizeof(cmd)-1, "nand read.e 0x%08lX misc 32", (unsigned long)BOOT_MODE_TMP_BUF_ADDR);
#endif
    if(-1 != run_command(cmd, 0))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd)-1, "md.b 0x%08lX 32", (unsigned long)BOOT_MODE_TMP_BUF_ADDR);
        run_command(cmd, 0);
        if((strlen(p_msg->command) == strlen(BOOT_MODE_RECOVERY_STR)) && (0==strncmp(p_msg->command, BOOT_MODE_RECOVERY_STR, strlen(BOOT_MODE_RECOVERY_STR))))
        {
            mode = EN_BOOT_MODE_RECOVERY;
        }
        /*else {do nothing;}*/
    }
    else
    {
        printf("%s: '%s' fails, at %d\n", __func__, cmd, __LINE__);
    }
    return mode;
}

#endif


#if (ENABLE_MODULE_BOOT_KEYPAD == 1)

extern BOOLEAN msKeypad_GetKey(U8 *pkey, U8 *pflag);

EN_BOOT_MODE get_boot_mode_from_keypad(void)
{
     U8 u8KeyPad_KeyVal=0xFF;
	 U8 u8KeyPad_RepFlag = 0;
     EN_BOOT_MODE mode = EN_BOOT_MODE_UNKNOWN;

	 msKeypad_GetKey(&u8KeyPad_KeyVal,&u8KeyPad_RepFlag);

     printf("fore uup u8KeyPad_KeyVal [0x%x]\n",u8KeyPad_KeyVal);
     switch(u8KeyPad_KeyVal) // NOTE: read IR Key to decide what boot mode to go
    {
        case KEYPAD_RECOVERY_KEY:
            mode =EN_BOOT_MODE_RECOVERY;
            break;
        case KEYPAD_UPGRADEBOOTLOADER_KEY:
            mode =EN_BOOT_MODE_FORCEUGRADE;
            break;
        case KEYPAD_FORCEUGRADE_KEY:
            mode =EN_BOOT_MODE_USB_UPGRADE;
            break;
        case KEYPAD_UART_ON_KEY:
            mode =EN_BOOT_MODE_UARTON;
            break;
        default:
            return EN_BOOT_MODE_UNKNOWN;
            break;
    }
     return mode;
}
#endif

int do_bootcheck (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
     int ret=0;
     EN_BOOT_MODE BootMode;
     BOOLEAN BootFlag;
     BootMode = EN_BOOT_MODE_UNKNOWN;
     BootFlag=0; // decide the boot mode
		char upgrade_state_flag[] = "0x00";


    UBOOT_TRACE("IN\n");

#if (ENABLE_MODULE_BOOT_IR == 1)
     if(BootFlag == 0)
     {
        BootMode =get_boot_mode_from_ir();
        if(BootMode!=EN_BOOT_MODE_UNKNOWN)
        {
            //bootmode via IR
            BootFlag=1;        
        }
     }
#endif

#if (ENABLE_MODULE_BOOT_KEYPAD == 1)
     if(BootFlag == 0)
     {
        BootMode =get_boot_mode_from_keypad();
        if(BootMode!=EN_BOOT_MODE_UNKNOWN)
        {
            //BootMode via KEYPAD
            BootFlag=1;
        }
     }
#endif

#if (ENABLE_MODULE_ANDROID_BOOT == 1)
    // NOTE: read BCB in mtd0 (misc) to decide what boot mode to go
    if(BootFlag == 0)
    {
        BootMode=get_boot_mode_from_mtd0();
        if(BootMode!=EN_BOOT_MODE_UNKNOWN)
        {
            //BootMode via MTD
            BootFlag=1;
        }
    }
#endif

#if (ENABLE_MODULE_ENV_BOOT == 1)
    if(BootFlag == 0)
    {
        BootMode=get_boot_mode_from_env();
    }
/*
    char* force_upgrade = getenv("force_upgrade");
    if(force_upgrade)
    {
        printf("mingwu_mboot----------------->force_upgrade=1\n");
        int force_flag = simple_strtoul(force_upgrade,NULL,16);
        if((force_flag <= 0x0F)&&(force_flag > 0x00))
        {
            printf("mingwu_mboot----------------->force_upgrade=1----EN_BOOT_MODE_USB_UPGRADE\n");
           //last time force upgrade not finish,so continue upgrading
            BootMode = EN_BOOT_MODE_USB_UPGRADE;
        }
    }
*/
#endif

    switch(BootMode)
    {
        case EN_BOOT_MODE_RECOVERY:
            ret = boot_mode_recovery();
            break;
        case EN_BOOT_MODE_FORCEUGRADE:
            {
                printf("mingwu_mboot-----EN_BOOT_MODE_UPGRADEBOOTLOADER\n");
                snprintf(upgrade_state_flag, sizeof(upgrade_state_flag)-1, "0x%02x",0x4);
                setenv("force_upgrade",upgrade_state_flag);
                saveenv();
		  printf("upgrade_state_flag ====== %s\n", upgrade_state_flag);		
                char *uartstr = getenv ("UARTOnOff");
                if (uartstr && (strncmp(uartstr, "off", 3) == 0))
                {
                    setenv("UARTOnOff","on");
                }
                //ret = run_command("custar",0);//boot_mode_forceupgrade();
                ret = run_command("usb_bootloader_upgrade",0);
                
            }
            break;
        #if (ENABLE_MODULE_USB == 1)
        case EN_BOOT_MODE_USB_UPGRADE:
            printf("mingwu_mboot-----EN_BOOT_MODE_USB_UPGRADE\n");
            snprintf(upgrade_state_flag, sizeof(upgrade_state_flag)-1, "0x%02x",0x8);
	     printf("upgrade_state_flag ====== %s\n", upgrade_state_flag);			
            setenv("force_upgrade",upgrade_state_flag);
            saveenv();
             //ret = run_command("custar",0);//usb upgrade
             ret = run_command("usb_upgrade",0);
            break;
        #endif            
        #if (ENABLE_MODULE_OAD == 1)
        case EN_BOOT_MODE_OAD_UPGRADE:
            ret = run_command("costar",0);//oad upgrade
            break;
        #endif    
        #if (ENABLE_MODULE_NETUPDATE == 1)
        case EN_BOOT_MODE_NET_UPGRADE:
            ret = run_command("cnstar",0);//net upgrade
            break;
        #endif    
        case EN_BOOT_MODE_UARTON:
            setenv("UARTOnOff","on");
            saveenv();
            MsOS_DelayTask(1000);
            ret = run_command("reset",0);
            break;
        case EN_BOOT_MODE_NORMAL:
            break;
        default:
            //normal booting according bootcmd in main.c
            UBOOT_DEBUG("non available case\n");
            break;
    }
    UBOOT_TRACE("OK\n");
    return ret;
}


#if TEMP_SECURE_BOOT

#if (CONFIG_R2_BRINGUP)
#include "drvSERFLASH.h" //for flash copy fw from flash to dram
#include "drvBDMA.h" //for bdma copy fw from flash to dram

static MS_U32 _gMIO_MapBase = 0x1F000000;

#define UART_DBG_FRCR2  0x00 //A5
#define UART_DBG_UART0  0x04
#define UART_DBG_SECR2  0x0D
#define UART_DBG_OTHER  UART_DBG_UART0 

#define UART_DBG_SRC    UART_DBG_SECR2
#define REG_R2_BASE			0x122A00

#define BIT11			(1 << 11)
#define BIT12			(1 << 12)
#define BIT13			(1 << 13)

MS_U8 _ReadByte(MS_U32 u32RegAddr)
{
    return ((volatile MS_U8*)(_gMIO_MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)];
}

MS_U16 _Read2Byte(MS_U32 u32RegAddr)
{
    return ((volatile MS_U16*)(_gMIO_MapBase))[u32RegAddr];
}

MS_U32 _Read4Byte(MS_U32 u32RegAddr)
{
    return (_Read2Byte(u32RegAddr) | _Read2Byte(u32RegAddr+2) << 16);
}

MS_BOOL _WriteByte(MS_U32 u32RegAddr, MS_U8 u8Val)
{
    if (!u32RegAddr)
    {
        return FALSE;
    }
    ((volatile MS_U8*)(_gMIO_MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)] = u8Val;
    return TRUE;
}

MS_BOOL _Write2Byte(MS_U32 u32RegAddr, MS_U16 u16Val)
{
    if (!u32RegAddr)
    {
        return FALSE;
    }

    ((volatile MS_U16*)(_gMIO_MapBase))[u32RegAddr] = u16Val;
    return TRUE;
}

MS_BOOL _Write4Byte(MS_U32 u32RegAddr, MS_U32 u32Val)
{
    if (!u32RegAddr)
    {
        return FALSE;
    }

    _Write2Byte(u32RegAddr, u32Val & 0x0000FFFF);
    _Write2Byte(u32RegAddr+2, u32Val >> 16);
    return TRUE;
}

MS_BOOL _WriteRegBit(MS_U32 u32RegAddr, MS_U8 u8Mask, MS_BOOL bEnable)
{
    MS_U8 u8Val = _ReadByte(u32RegAddr);
    if (!u32RegAddr)
    {
        return FALSE;
    }

    u8Val = _ReadByte(u32RegAddr);
    u8Val = (bEnable) ? (u8Val | u8Mask) : (u8Val & ~u8Mask);
    _WriteByte(u32RegAddr, u8Val);
    return TRUE;
}


void MApi_R2_FwCopy(MS_U32 u32FlashAddr, MS_U32 u32DramAddr, MS_U32 u32Len)
{
	printf("Copy R2 Fw from Flash 0x%08x to Ram 0x%08x, len = 0x%08x ... ",
		(unsigned int)u32FlashAddr, (unsigned int)u32DramAddr, (unsigned int)u32Len);

	MDrv_BDMA_SetDbgLevel(E_BDMA_DBGLV_ALL);

	MDrv_BDMA_Init(0x20000000);

	MDrv_BDMA_FlashCopy2Dram((MS_PHYADDR)u32FlashAddr, (MS_PHYADDR)u32DramAddr, u32Len);

	printf("Done!\n");
}

void MApi_R2_StartFrDRAM(MS_U32 addr)
{
	MS_U32 AeonBufAdr=addr;
	MS_U16 value;

	printf("R2 Boot from 0x%08x\n", (unsigned int)AeonBufAdr);

	// select UART source
	//_WriteByte(0x101EA6, UART_DBG_SRC);
	_Write2Byte(0x101E04, 0x0400);
	_Write2Byte(0x101EA6, 0x0D04);

	//switch UART to dig_mux (instead of MCU51)
	value = _Read2Byte(0x0E12);
#if 1 //HK message
	value &= (~BIT12); //0: dig_mux (reg_uart_sel0)
	value |= BIT11; //1: enable uart rx via  PAD_DDCA for dig_mux (reg_uart_sel0)
#else //PM message
	value |= (BIT12); //1: HK MCU51 uart0
	value |= BIT11; //1: enable uart rx via  PAD_DDCA for dig_mux (reg_uart_sel0)
#endif
	_Write2Byte(0x0E12,value);


	//(1) Disable R2 -> Stop
	_WriteByte(REG_R2_BASE+0x40*2, 0x00); //disable R2

	//(2) Set R2 clock
	// [1:0] = 2'b00
	// [3:2] = 2'b00: 240MHz,  2'b01: 192HMz, 2'b10: 172MHz, 2'b11: 144MHz
	// Set to 240 MHz
	_WriteByte(0x10337A, 0x00);

	// check MIU memory mapping
#if 0
	//(3) Set CPU SDR base
	_Write2Byte(REG_R2_BASE+0x41*2, (AeonBufAdr&0x0000FFFF)); //I Fetch Offset - Low
	_Write2Byte(REG_R2_BASE+0x42*2, (AeonBufAdr>>16)); //I Fetch Offset - High
	_Write2Byte(REG_R2_BASE+0x43*2, (AeonBufAdr&0x0000FFFF)); //D Fetch Offset - Low
	_Write2Byte(REG_R2_BASE+0x44*2, (AeonBufAdr>>16)); //D Fetch Offset - High
#else
	//(3) Set CPU reset base(vector)
	_Write2Byte(REG_R2_BASE+0x5A*2, (AeonBufAdr>>16)); //CPU reset base (vector)  // reset vector address 0x0(64K alignment)
#endif

	//(4) Set UART base
	_Write2Byte(REG_R2_BASE+0x55*2, 0x9000); // UART base address

	//(5) Set IO space enable (UART, RIU) with QMEM space disabled
	_Write2Byte(REG_R2_BASE+0x58*2, 0x0003); // io_space_en[3:0]: UART,RIU, qmem_space_en[4] : disable

	//(6) CPU SW reset R2
	_Write2Byte(0x2E52, _Read2Byte(0x2E52) &(~BIT13));
	_Write2Byte(0x2E52, _Read2Byte(0x2E52) |(BIT13));

	//(7) Without QMEM 
#if 0
	_Write2Byte(REG_R2_BASE+0x4E*2, 0xB000); // QMEM base address = 0x80000000
	_Write2Byte(REG_R2_BASE+0x50*2, 0xF000); // QMEM less, set DCU Decoding..
#endif

	//(8) Enable R2 -> Start
	_WriteByte(REG_R2_BASE+0x40*2, 0x24); // miu_sw_rst and sdram_boot = 1
	_WriteByte(REG_R2_BASE+0x40*2, 0x27); // sdram_boot and (miu/r2/r2_rst) =1
}

#endif	// CONFIG_R2_BRINGUP


int do_bootNuttx (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret = 0;
 #if (CONFIG_R2_BRINGUP)
 
  ret = run_command ("nand read.e 0x58000000 NUTTX 0x200000", 0 );
	MS_U32 u32R2FWAddr = 0x100000;
	MS_U32 u32R2BaseAddr = 0x18000000;
	MS_U32 u32R2FWLen = 0x20000;

	//MApi_R2_FwCopy(u32R2FWAddr, u32R2BaseAddr, u32R2FWLen);

	MApi_R2_StartFrDRAM(u32R2BaseAddr);
 #endif	// CONFIG_R2_BRINGUP
	

    return ret;
}
#endif


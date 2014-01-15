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

#include <command.h>
#include <common.h>
#include <drvWDT.h>
#include <MsMath.h>
#include <MsSystem.h>
#include <MsUtility.h>
#include <MsDebug.h>
#include <exports.h>
#include <drvGPIO.h>
#include <MsOS.h>
#include <MsRawIO.h>
#include <ShareType.h>
#include <drvUART.h>
#include <MsUboot.h>
#include <drvMBX.h>
#include <msAPI_Power.h>

extern int snprintf(char *str, size_t size, const char *fmt, ...);

void do_Led_Flicker_Control(void)
{
    static U8 u8LedStatus = 0;
    if(u8LedStatus != 0)
    {
      LED_RED_ON();
    }
    else
    {
      LED_RED_OFF();
    }
    u8LedStatus = ~u8LedStatus;
  }

int do_wdt_enable (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int u32Timer = 0;

    if (argc < 2) {
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    u32Timer = simple_strtoul(argv[1], NULL, 10);

    MDrv_WDT_Init(E_WDT_DBGLV_ALL);
    MDrv_WDT_SetTimer(E_WDT_DBGLV_ALL, u32Timer);

    return 0;
}

extern U8 MDrv_MIU_SelectMIU_UBoot(U8 bMIU1, U16 *au16SelMiu, U8 u8GpNum);
extern U8 MDrv_MIU_SetGroupPriority_UBoot(U8 bMIU1, U8 *au8GpPriority, U8 u8GpNum);

static void set_miu_group_priority(U8 u8MiuIdx, const char* cStr)
{
    if (cStr)
    {
        int i = 0;
        int j = 0;
        int length = 0;
        U8 au8GpPriority[4] = {0};

        for (;;)
        {
            if (cStr[i] != ':')
            {
                au8GpPriority[j++] = cStr[i] - 0x30;
            }
            else
            {
                ++length; // length is ':' number and priority number need add one more.
            }

            if ('\0' == cStr[i])
            {
                ++length;
                break;
            }

            ++i;
        }

        MDrv_MIU_SetGroupPriority_UBoot(u8MiuIdx, au8GpPriority, length);
    }
}

static void select_miu(U8 u8MiuIdx, const char* cStr)
{
    if (cStr)
    {
        int i = 0;
        int j = 0;
        int length = 0;

        for (;;)
        {
            if (':' == cStr[i])
            {
                ++length;
            }

            if ('\0' == cStr[i])
            {
                ++length; // length is ':' number and priority number need add one more.
                break;
            }

            ++i;
        }

        i = 0;
        j = 0;
        U16 au16SelMiu[4] = {0};

        for (;;)
        {
            au16SelMiu[j++] = ascii_to_hex(&cStr[i], 4);
            i += 5;

            if (length == j)
            {
                break;
            }
        }

        MDrv_MIU_SelectMIU_UBoot (u8MiuIdx, au16SelMiu, length);
    }
}

static void set_miu(void)
{
    set_miu_group_priority(0, getenv("MIU0_GROUP_PRIORITY"));
    set_miu_group_priority(1, getenv("MIU1_GROUP_PRIORITY"));
    select_miu(0, getenv("MIU0_GROUP_SELMIU"));
    select_miu(1, getenv("MIU1_GROUP_SELMIU"));
}

int do_set_miu(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    set_miu();
    return 0;
}

int do_disable_uart ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    volatile u32 *ptr;

    printf("Disable UART\n");

    ptr = (u32 *)(MS_RIU_MAP + (0x0E12<<1));
    *ptr &= ~0x1800;
    //mdrv_uart_connect(E_UART_PORT0,E_UART_OFF);
    return 0;
}

static U32 gu32ChunkHeader[CH_ITEM_LAST+1];
static BOOLEAN ChunkHeaderReady = FALSE;
int MsApiChunkHeader_Init(void)
{
    U32 u32ChunkHeaderOffset = 0;
    int ret = -1;

    UBOOT_TRACE("IN\n");
    raw_io_config_push();
    ret = mboot_raw_io_Config();
    if(ret != 0)
    {
        UBOOT_ERROR("raw_io_config setting fail!\n");
        raw_io_config_pop();
        return ret;
    }

    if(get_raw_status()==E_RAW_DATA_IN_SPI)
    {
        if(IsHouseKeepingBootingMode()==FALSE){
             u32ChunkHeaderOffset = 0x30000;
        }
        else
        {
            #if(ENABLE_MSTAR_STR_ENABLE==1 ||ENABLE_MSTAR_PM_SWIR==1 )
                u32ChunkHeaderOffset = 0x30000;
            #else
                u32ChunkHeaderOffset = 0x20000;
            #endif
        }
    }
    else
    {
         u32ChunkHeaderOffset = 0;
    }
    ret = raw_read((U32)gu32ChunkHeader,u32ChunkHeaderOffset,(CH_ITEM_LAST+1)*4);
    
    raw_io_config_pop();
 
    if(ret != 0)
    {
         UBOOT_ERROR("raw_read gu8ChunkHeader fail\n");
         return -1;
    }
    
    ChunkHeaderReady = TRUE; 
    UBOOT_TRACE("OK\n");
    return 0;    
}

int MsApiChunkHeader_GetValue(EN_CHUNK_HEADER_ITEM enItem,unsigned int *puValue)
{
    UBOOT_TRACE("IN\n");
    
    //UBOOT_DUMP(gu8ChunkHeader,CH_ITEM_LAST);
    if(puValue==NULL)
    {
        UBOOT_ERROR("puValue is a null pointer\n");
        return -1;
    }
    
    if(ChunkHeaderReady != TRUE)
    {
        if(MsApiChunkHeader_Init() !=0)
        {
            UBOOT_ERROR("MsApiChunkHeader_Init fail\n");
            return -1;
        }
    }


    if((enItem>=CH_ITEM_FIRST) && (enItem<=CH_ITEM_LAST))
    {
        *puValue = gu32ChunkHeader[enItem];
        UBOOT_DEBUG("ChunkHeaderOffset(0x%x): 0x%x\n",enItem,*puValue);
    }
    else
    {
        UBOOT_ERROR("ERROR: Out of chunk header bound : 0x%x \n",enItem);
        return -1;
    }

    UBOOT_TRACE("OK\n");
    return 0;    
}

MS_S32 g_s32TimerID = -1;
void SetTimerIsrDone(void)
{
    if(g_s32TimerID >= 0)
    {
        MsOS_StopTimer(g_s32TimerID);
        MsOS_DeleteTimer(g_s32TimerID);
        g_s32TimerID = -1;
        printf("\nTimer ISR finished\n");
    }
}

void TimerISR(MS_U32 u32Tick,MS_U32 u32TimerID)
{
    u32Tick    = u32Tick;
    u32TimerID = u32TimerID;
    #if (ENABLE_LED_FLICKER_WHEN_SW_UPDATE == 1)
    do_Led_Flicker_Control();
    #endif
}

void do_Timer_ISR_Register(void)
{
    #if (ENABLE_LED_FLICKER_WHEN_SW_UPDATE == 1)
    if(g_s32TimerID == -1)
    {
        g_s32TimerID = MsOS_CreateTimer( (TimerCb)TimerISR,
                                                      0,
                                                      1000,
                                                      TRUE,
                                                      "Check timer");
        if(g_s32TimerID >= 0)
            printf("\t Creat Timer Success,TimerId = %ld\n",g_s32TimerID);
        else
            printf("\t Creat Timer Failed...\n");
    }
    #endif
}


extern U32 __heap;
extern U32 __heap_end;
void console_init(void)
{
#if 0//(CONFIG_R2_BRINGUP)
    //don't reinit uart
    return;
#endif	// CONFIG_R2_BRINGUP

    //bit 12 UART Enable, bit 11 UART RX Enable
    *(volatile U32*)(MS_RIU_MAP+(0x0E12<<1)) &= ~0x1800;    //don't enable uart (decided by mboot env)

    // Switch UART0 to PIU UART 0
    // Clear UART_SEL0
    *(volatile U32*)(MS_RIU_MAP+(0x101EA6<<1)) &= ~0x000F;
#if (EARLY_DISABLE_UART)
#else
    // bit 11 UART RX Enable
    *(volatile U32*)(MS_RIU_MAP+(0x0E12<<1)) |= 0x0800;

    // UART_SEL0 --> PIU UART0
#if (ENABLE_MSTAR_BD_MST147B_D01A_S_AGATE)
    *(volatile U32*)(MS_RIU_MAP+(0x101EA6<<1)) = 0x5004;
#else
    *(volatile U32*)(MS_RIU_MAP+(0x101EA6<<1)) = 0x0054;
#endif
    *(volatile U32*)(MS_RIU_MAP+(0x101EA8<<1)) = 0x0000;
   // *(volatile U32*)(0x1f200000+(0x0F53*4)) |= 0x0045;
   // *(volatile U32*)(0x1f200000+(0x0F02*4)) |= 0x0300;
#endif
    // <-------

    // UART mode
    //*(volatile U32*)(0x1f200000+(0x0F6E*4)) &= ~0x0F00;

    //enable reg_uart_rx_enable;
    *(volatile U32*)(MS_RIU_MAP+(0x101EA2<<1)) |= (1<<10);

//    MDrv_UART_Init(E_UART_PIU_UART0, 115200);

	//Terry, It is not necessary to set baudrate here, because it already be initialized in sboot
//    MDrv_UART_Init(E_UART_PIU_UART0, 38400);
}

void console_disable(void)
{
    //*(volatile U32*)(0x1f000000+(0x0709*4)) &= ~0x1800;    //don't enable uart (decided by mboot env)
    //*(volatile U32*)(MS_RIU_MAP+(0x0E12<<1)) |= 0x1800;    //don't enable uart (decided by mboot env)
    // Switch UART0 to PIU UART 0
//mdrv_uart_connect(E_UART_PORT0,E_UART_OFF);
   *(volatile U32*)(0x1f000000+(0x0709*4)) &= ~0x1800;    //don't enable uart (decided by mboot env)
    *(volatile U32*)(0x1f200000+(0x0F53*4)) &= ~0x000F;
}

MS_BOOL IsHouseKeepingBootingMode(void)
{
    UBOOT_TRACE("IN\n");
    #if defined (CONFIG_BOOTING_FROM_OTP_WITH_PM51)|| defined (CONFIG_BOOTING_FROM_EXT_SPI_WITH_PM51)
    UBOOT_DEBUG("Host keeping is not the booting CPU\n");
    UBOOT_TRACE("OK\n");
    return FALSE;
    #else
    UBOOT_DEBUG("Host keeping is the booting CPU\n");
    UBOOT_TRACE("OK\n");
    return TRUE;
    #endif
}

MS_BOOL IsBootingFromMaskRom(void)
{
    UBOOT_TRACE("IN\n");
    #if defined (CONFIG_MSTAR_ROM_BOOT_WITH_NAND_FLASH)|| defined (CONFIG_MSTAR_ROM_BOOT_WITH_EMMC_FLASH)
    UBOOT_DEBUG("Boot from mask rom\n");
    UBOOT_TRACE("OK\n");
    return TRUE;
    #else
    UBOOT_DEBUG("Doesn't boot from mask rom\n");
    UBOOT_TRACE("OK\n");
    return FALSE;
    #endif
}

MS_BOOL MApi_MBX_Init(void)
{
    MBX_CPU_ID eHKCPU;
    MS_U32 u32TimeoutMillSecs = 1500;

    eHKCPU = E_MBX_CPU_MIPS;
    if( E_MBX_SUCCESS != MDrv_MBX_Init(eHKCPU, E_MBX_ROLE_HK, u32TimeoutMillSecs))
    {
        printf("Error> MBX init failed !!\n");
        while(1);
    }
    else
    {
        MDrv_MBX_Enable(TRUE);
        return TRUE;
    }
}

void MsSystemReset(void)
{
   if((IsHouseKeepingBootingMode()==FALSE)&&(ENABLE_MODULE_SECURITY_BOOT==1))
   {
    msAPI_PM_MBX_Init();
    msAPI_PM_Reset();
   }
   else
   {
    MDrv_Sys_WholeChipReset();
   }
   hang();
}


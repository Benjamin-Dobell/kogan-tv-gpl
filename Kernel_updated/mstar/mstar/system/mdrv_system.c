////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drv_system_io.c
/// @brief  System Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <asm/io.h>     //robert.yang
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/version.h>

#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
#include "mdrv_system_io.h"
#include "board/Board.h"
#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
#include "chip_setup.h"
#endif

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
/*
#define DEF_STC_SYNTH               0x14000000                          // 216/2.5  = 86.4, 2.5(dec) = 00010.1(bin) = 0x14000000(u5.27)
#define MAX_STC_SYNTH               0x15000000                          // 216/2.75 = 78.55
#define MIN_STC_SYNTH               0x13000000                          // 216/2.25 = 96
*/

#define SYS_DEBUG                   1

#if SYS_DEBUG
#define SYS_PRINT(fmt, args...)     printk("[SYS][%06d]     " fmt, __LINE__, ## args)
#else
#define SYS_PRINT(fmt, args...)
#endif

#define ALL_INT_DISABLE()       local_irq_disable()
#define ALL_INT_ENABLE()        local_irq_enable()
#define printf                  printk

#define PCMCIA_NAND_SEM(x)          (x)

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
#define PCMCIA_LOOP_DELAY_WAIT_EMMC_DONE	(2000000)
#endif

#define KERNEL_PCMCIA_PERF 0

#if defined(CONFIG_MSTAR_AGATE) || defined(CONFIG_MSTAR_EAGLE)
#define PAD_PCM_ENABLE() \
    REG_ADDR(REG_CHIP_PCM_PE)   |= REG_CHIP_PCM_PE_MASK; \
    REG_ADDR(REG_CHIP_PCM_PE1)  |= REG_CHIP_PCM_PE1_MASK; \
    REG_ADDR(REG_CHIP_PCM_D_PE) |= REG_CHIP_PCM_D_PE_MASK; \

#define PAD_PCM_DISABLE() \
    REG_ADDR(REG_CHIP_PCM_PE)   &= ~REG_CHIP_PCM_PE_MASK; \
    REG_ADDR(REG_CHIP_PCM_PE1)  &= ~REG_CHIP_PCM_PE1_MASK; \
    REG_ADDR(REG_CHIP_PCM_D_PE) &= ~REG_CHIP_PCM_D_PE_MASK; \

#endif

unsigned int MDrv_SYS_GetMIU0Length(void)
{
#if  (defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
       {
           if(REG_ADDR(REG_MAU0_MIU0_SIZE)&0x0100)
           {
               return MS_MIU_INTERVAL;//256M
           }
           return 0x08000000;//128
       }
#else
     return MS_MIU_INTERVAL;
#endif
}

void MDrv_SYS_GetMIU1Base(U32 argv)
{
    unsigned int u32Data;
    u32Data=MDrv_SYS_GetMIU0Length();
    if(copy_to_user((void*)argv, (void*)&u32Data, sizeof(unsigned int)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}

void MDrv_SYS_GetMIU1BusBase(U32 argv)
{
    unsigned int u32Data=0;
#if defined(CONFIG_MSTAR_TITANIA2) || \
    defined(CONFIG_MSTAR_EUCLID) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_AGATE)    || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u32Data=0x08000000;
#elif (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10) )
    u32Data=MIPS_MIU1_BUS_BASE;
#else
     #error unknown chip!!
#endif
    if(copy_to_user((void*)argv, (void*)&u32Data, sizeof(unsigned int)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}

//-------------------------------------------------------------------------------------------------
//  Export
//-------------------------------------------------------------------------------------------------
EXPORT_SYMBOL(MDrv_SYS_GetPanelInfo);
EXPORT_SYMBOL(MDrv_SYS_GetMMAP);
EXPORT_SYMBOL(MDrv_SYS_GetDRAMLength);
EXPORT_SYMBOL(MDrv_SYS_GetMIU0Length);

//-------------------------------------------------------------------------------------------------
//  Import
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
extern bool mstar_mci_exit_checkdone_ForCI(void);
#endif
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
MST_PLATFORM_INFO_t gPlatformInfo;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20)
DEFINE_SEMAPHORE(PfModeSem);
#else
DECLARE_MUTEX(PfModeSem);
#endif


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

static unsigned char* aeon_msg = (unsigned char*)(0xA017E000) ;
static unsigned int aeon_idx = 0 ;
#define AEON_MSG_LEN_MASK   0x7FFFF

#if 0
static SYS_Info                  sysInfo =
                                    {
                                        .SWLib.Board = BOARD_NAME,
                                        .SWLib.Platform = CHIP_NAME,
                                    };

#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

/*
inline void _System_SetStcSynth(MS_U8 u8StcId, MS_U32 u32StcCw)
{
    REG(REG_STC0_SYNC_CW_LO + (8*u8StcId)) = (MS_U16)(u32StcCw & 0xFFFF);
    REG(REG_STC0_SYNC_CW_HI + (8*u8StcId)) = (MS_U16)(u32StcCw >> 16);
    REG(REG_DCSYNC_MISC) = (REG(REG_DCSYNC_MISC) & ~(STC0_CW_SEL_TSP<<(1*u8StcId))) | (UPDATE_STC0_CW << (1*u8StcId));
}
*/

static int gfx_gop_index=0; //default is GOP 0
static int display_controller_seperated=0;
static int using_nexus=0;
static int nexus_pid=-1;

static U32 u32MsBinAddr = 0x00;
static U32 u32MsBinSzie = 0x00;
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
U32 MDrv_SYS_SetPanelInfo(U32 arg)
{
    IO_SYS_PANEL_INFO_t info;

    SYS_PRINT("%s\n", __FUNCTION__);

    if (copy_from_user(&info, (IO_SYS_PANEL_INFO_t __user*)arg, sizeof(IO_SYS_PANEL_INFO_t)))
    {
        return -EFAULT;
    }

    if (info.u16Len != sizeof(MST_PANEL_INFO_t))
    {
        return  -EFAULT;
    }

    if (copy_from_user((void*)&gPlatformInfo.panel, (void __user*)info.pPanelInfo, sizeof(MST_PANEL_INFO_t)))
    {
        return -EFAULT;
    }

    return 0;
}

void MDrv_SYS_GetPanelRes(U32 arg)
{
    IO_SYS_PANEL_GET_RES_t info;

    info.u16Width  = gPlatformInfo.panel.u16Width;
    info.u16Height  = gPlatformInfo.panel.u16Height;


    if(copy_to_user((void*)arg, (void*)&info, sizeof(IO_SYS_PANEL_GET_RES_t)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}


void MDrv_SYS_ReadGeneralRegister(U32 arg)
{
    U16 u16ReadReg;
    //BOOL bHiByte;

    if (copy_from_user(&u16ReadReg, (void __user *)arg, sizeof(U16)))
    {
        return;
    }
    if (u16ReadReg&1)
    {
        u16ReadReg= u16ReadReg - 1;
        u16ReadReg = ((REG_ADDR(u16ReadReg) & 0xFF00) >> 8);
    }
    else
    {
        u16ReadReg = (REG_ADDR(u16ReadReg) & 0x00FF);
    }
    SYS_PRINT ("u16ReadReg= %x ", u16ReadReg);

    if (copy_to_user((void *)arg, &u16ReadReg, sizeof(U16)))
    {
        return;
    }

}

void MDrv_SYS_WriteGeneralRegister(U32 arg)
{
    IO_SYS_GENERAL_REG_t param;
    U16 u16RegValue;
    BOOL bHiByte;

    if (copy_from_user(&param, (void __user *)arg, sizeof(IO_SYS_GENERAL_REG_t)))
    {
        return;
    }
    bHiByte = ((param.u16Reg & 0x00FF)%2) ? TRUE : FALSE;
    if (param.u16Reg&1)
    {
        U16 reg  = param.u16Reg - 1;
        u16RegValue = (REG_ADDR(reg));
        u16RegValue = ((u16RegValue & 0x00FF) | ((param.u8Value) << 8));
        REG_ADDR(reg) = u16RegValue;
    }
    else
    {
        u16RegValue = (REG_ADDR(param.u16Reg));
        u16RegValue = ((u16RegValue & 0xFF00) | (param.u8Value));
        REG_ADDR(param.u16Reg) = u16RegValue;
    }
}

void MDrv_SYS_LoadAeon(U32 arg)
{
//    U32 u32AeonMemAddr = 0;
//    U32 u32AeonMemLen = 0x300000 - 0x1000;

    IO_SYS_AEONBIN_INFO_t AeonInfo;

    U8* pu8Aeon = NULL;
    BOOL bRet = TRUE;
    SYS_PRINT("Load Aeon Code.\n");

    if (copy_from_user(&AeonInfo, (void __user *)arg, sizeof(IO_SYS_AEONBIN_INFO_t)))
    {
        return;
    }

    MDrv_SYS_DisableAeon();

    pu8Aeon = (U8*)0xA0000000 ; // ioremap(u32AeonMemAddr, u32AeonMemLen);
    SYS_PRINT("Aeon star addr: 0x%x.\n", (U32)pu8Aeon);

     //Un-protect 0x00000000 ~ 0x00100000
    REG_ADDR(MIU_PROTECT_EN) &= (~MIU_PROTECT_4);
    REG_ADDR(MBX_AEON_JUDGEMENT) = 0x01;

    //printk( "sizeof AeonBin=%d\n", sizeof(AeonBin) ) ;
    //memcpy((void*)pu8Aeon, (void*)AeonBin, sizeof(AeonBin) ) ;
    if (copy_from_user(pu8Aeon, (void __user *)AeonInfo.pu8AeonStart, AeonInfo.u32Len))
    {
        return;
    }

    //Protect 0x00000000 ~ 0x0010000
    REG_ADDR(MIU_PROTECT3_ID0) = MIU_CLI_AEON_RW;
    REG_ADDR(MIU_PROTECT3_START_ADDR_H) = 0x0;
    REG_ADDR(MIU_PROTECT3_END_ADDR_H) = 0x400;  //0x100000
    REG_ADDR(MIU_PROTECT_EN) |= MIU_PROTECT_4;

#if defined(CONFIG_Triton)
    REG_ADDR(0x1E00) |= 0x02;
#endif

    MDrv_SYS_EnableAeon();
    msleep(200);
    memcpy((void*)&AeonInfo.bRet, &bRet, sizeof(BOOL));

}

void MDrv_SYS_ResetAeon(U32 arg)
{
    SYS_PRINT("Reset Aeon. \n");

    REG_ADDR(MHEG5_CPU_STOP) = 0x1;
    msleep(1);
    REG_ADDR(REG_SW_RESET_CPU_AEON) &= ~AEON_SW_RESET;
    REG_ADDR(REG_SW_RESET_CPU_AEON) = AEON_SW_RESET;

}

//static unsigned int vd_clock ;

void MDrv_SYS_EnableAeon(void)
{
    //unsigned int tmp ;
    MDrv_SYS_DisableAeon();
    SYS_PRINT("Enable Aeon. \n");

    aeon_idx = 0 ;

    // disable VD
    //vd_clock = *(volatile unsigned int*)(0xBF800000+(0x1E2C<<1)) ;
    //*(volatile unsigned int*)(0xBF800000+(0x1E2C<<1)) |= (1<<12) ;
    //vd_clock=tmp=REG_ADDR(0x3610);
    //tmp &= 0xFF00 ;
    //tmp |= 0x0012 ;
    //REG_ADDR(0x3610)=tmp;


    REG_ADDR(MHEG5_REG_IOWINORDER) = 0x2; // Let aeon command ¡§in order¡¨
    REG_ADDR(REG_CKG_AEONTS0) &= ~AEON_CLK_DISABLE;

    //REG_ADDR(AEON_SPI_ADDR0) = 0xA100;

    //Mask Aeon FIQ/IRQ
    REG_ADDR(REG_AEON_C_FIQ_MASK_L) = 0xFFFF;
    REG_ADDR(REG_AEON_C_FIQ_MASK_H) = 0xFFFF;
    REG_ADDR(REG_AEON_C_IRQ_MASK_L) = 0xFFFF;
    REG_ADDR(REG_AEON_C_IRQ_MASK_H) = 0xFFFF;


/*
    REG_ADDR(0x3380) = 0;
    REG_ADDR(0x3382) = 0;
    REG_ADDR(0x3384) = 0;
    REG_ADDR(0x3386) = 0;
    REG_ADDR(0x3388) = 0;
    REG_ADDR(0x338A) = 0;
    REG_ADDR(0x338C) = 0;
    REG_ADDR(0x338E) = 0;
    REG_ADDR(0x3390) = 0;
    REG_ADDR(0x3392) = 0;
    REG_ADDR(0x3394) = 0;
    REG_ADDR(0x3396) = 0;
    REG_ADDR(0x3398) = 0;
    REG_ADDR(0x339A) = 0;
    REG_ADDR(0x339C) = 0;
    REG_ADDR(0x339E) = 0;
*/

    REG_ADDR(AEON_REG_CTRL) |= (AEON_CTRL_EN + AEON_CTRL_RST);
    REG_ADDR(REG_SW_RESET_CPU_AEON) |= AEON_SW_RESET;

    //==============================================
    // bit0 = 1/0 : Aeon stop/start
    // bit1 = 1 for patch1
    // bit2 = 1 for patch2
    REG_ADDR(MHEG5_CPU_STOP) = 0x02;

    SYS_PRINT("Enable Aeon Done. \n");

    //XBYTE[0x1246] &= (~0x10);
    //REG_ADDR(0x1246) &= (~0x10);
}

void MDrv_SYS_DumpAeonMessage( void ){
    aeon_msg[AEON_MSG_LEN_MASK+1] = 0 ;
    rmb() ;
    while( 0!=aeon_msg[aeon_idx] ){
        printk( aeon_msg+aeon_idx ) ;
        aeon_idx += strlen(aeon_msg+aeon_idx)+1 ;
        aeon_idx &= AEON_MSG_LEN_MASK ;
    }
}

void MDrv_SYS_DisableAeon(void)
{
    U16 u16Reg = 0;
    SYS_PRINT("Disable Aeon. \n");

    u16Reg = REG_ADDR(REG_SW_RESET_CPU_AEON);
#if defined(CONFIG_MSTAR_TITANIA2)
    if (u16Reg & AEON_SW_RESET)
    {
#endif
        REG_ADDR(MHEG5_CPU_STOP) = STOP_AEON; //Stop Aeon
        //msleep(1);
        REG_ADDR(REG_SW_RESET_CPU_AEON) = u16Reg & (~AEON_SW_RESET);
        REG_ADDR(AEON_REG_CTRL) &= ~(AEON_CTRL_EN + AEON_CTRL_RST);
#if defined(CONFIG_MSTAR_TITANIA2)
    }
#endif

    SYS_PRINT("Disable Aeon Done. \n");

    // restore VD
    //if( 0==(vd_clock&(1<<12)) )
    //    *(volatile unsigned int*)(0xBF800000+(0x1E2C<<1)) &= ~(1<<12) ;

    //vd_clock &= 0xFF00 ;
    //vd_clock |= 0x0017 ;
    //REG_ADDR(0x3610)=vd_clock;

    // Clear MBX, move from Chakra to here
    REG_ADDR(0x3384) = 0;

}

void MDrv_SYS_SwitchUart(U32 arg)
{
    if (*((U8*)arg) == TRUE)
    {
        //SYS_PRINT("UART Switch to Aeon\n");
        REG_ADDR(0x1EAA) &= ~0x1C;
        REG_ADDR(0x1EAA) |= 0x0C;
    }
    else
    {
        REG_ADDR(0x1EAA) &= ~0x1C;
        REG_ADDR(0x1EAA) |= 0x05<<2;
    }
}

U32 MDrv_SYS_IsAeonEnable(U32 arg)
{
    U32 u32Temp = 0;
    if (( REG_ADDR(0x3384) & (~BIT2)) == 0x1818)
    {
        u32Temp = 1;
    }

    if (copy_to_user((void*)arg, &u32Temp, sizeof(U32)))
    {
        return -EFAULT;
    }
    return 0;
}

PMST_PANEL_INFO_t MDrv_SYS_GetPanelInfo(void)
{
    return &gPlatformInfo.panel;
}


//U32 MDrv_SYS_SetBoardInfo(U32 arg)
//{
//
//
//}

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
#define SYS_REGOP(addr) *((volatile unsigned int*)(0xFD800000 + (addr)))
#else
#define SYS_REGOP(addr) *((volatile unsigned int*)(0xBF800000 + (addr)))
#endif

#ifdef IO_SYS_REG_OP

typedef struct REG_OPERATION{
    // 0=hw address mode(refer to excel files)
    // 1=8051 address mode(using 8051 address space)
    // 2=MIPS address mode(using MIPS address space)
    unsigned int mode ;
    unsigned int op ; // 0=read, 1=write
    unsigned int addr ; // address
    unsigned int value ; // set or get value
} REG_OPERATION_T ;

U32 MDrv_SYS_RegOP(U32 arg){
    REG_OPERATION_T op ;

    if(copy_from_user( &op, (REG_OPERATION_T*)arg, sizeof(op) ))
        {
            printk( "copy_from_user error\n" ) ;
        }

    if( 0==op.op ){ // read
        op.value = SYS_REGOP(op.addr) ;
        if(copy_to_user( (void *)arg, &op, sizeof(op) ))
            {
                 printk( "copy_to_user error\n" ) ;
            }
    }else{          // write
        SYS_REGOP(op.addr) = op.value ;
    }

    return 0 ;
}
#endif

#ifdef IO_SYS_GET_RAW_UART
#define RAWUART_MAX_SIZE    512

unsigned char g_get_raw_uart[RAWUART_MAX_SIZE] ;
#if defined(CONFIG_MSTAR_OBERON)
#define uartreg(addr) *((volatile unsigned int*)(0xbf808c80 + ((addr))))
#define UART_REG_ONE_BYTE(addr) *((volatile unsigned char*)(0xbf808c80 + ((addr))))
#elif defined(CONFIG_MSTAR_EUCLID)
#define uartreg(addr) *((volatile unsigned int*)(0xbf201300 + ((addr))))
#define UART_REG_ONE_BYTE(addr) *((volatile unsigned char*)(0xbf201300 + ((addr))))
#elif defined(CONFIG_MSTAR_TITANIA3) || \
      defined(CONFIG_MSTAR_TITANIA10) || \
      defined(CONFIG_MSTAR_URANUS4) || \
      defined(CONFIG_MSTAR_TITANIA4) || \
      defined(CONFIG_MSTAR_TITANIA8) || \
      defined(CONFIG_MSTAR_TITANIA9) || \
      defined(CONFIG_MSTAR_TITANIA11) || \
      defined(CONFIG_MSTAR_JANUS2) || \
      defined(CONFIG_MSTAR_TITANIA12) || \
      defined(CONFIG_MSTAR_TITANIA13) || \
      defined(CONFIG_MSTAR_AMBER1) || \
      defined(CONFIG_MSTAR_AMBER2) || \
      defined(CONFIG_MSTAR_AMBER5) || \
      defined(CONFIG_MSTAR_KRONUS) || \
      defined(CONFIG_MSTAR_AMBER6) || \
      defined(CONFIG_MSTAR_AMBER7) || \
      defined(CONFIG_MSTAR_KAISERIN) || \
      defined(CONFIG_MSTAR_AMETHYST) || \
      defined(CONFIG_MSTAR_EMERALD)
#define uartreg(addr) *((volatile unsigned int*)(0xbf201300 + ((addr))))
#define UART_REG_ONE_BYTE(addr) *((volatile unsigned char*)(0xbf201300 + ((addr))))
#elif defined(CONFIG_MSTAR_AGATE)    || \
      defined(CONFIG_MSTAR_AMBER3) || \
      defined(CONFIG_MSTAR_EAGLE) || \
      defined(CONFIG_MSTAR_EDISON) || \
      defined(CONFIG_MSTAR_EINSTEIN) || \
      defined(CONFIG_MSTAR_EIFFEL)
#define uartreg(addr) *((volatile unsigned int*)(0xFD201300 + ((addr))))
#define UART_REG_ONE_BYTE(addr) *((volatile unsigned char*)(0xFD201300 + ((addr))))
#else
#define uartreg(addr) *((volatile unsigned int*)(0xBF801300 + ((addr))))
#define UART_REG_ONE_BYTE(addr) *((volatile unsigned char*)(0xBF801300 + ((addr))))
#endif

extern int g_in_raw_get_mode;

#if defined(CONFIG_MSTAR_TITANIA2) || \
    defined(CONFIG_MSTAR_OBERON) || \
    defined(CONFIG_MSTAR_EUCLID) || \
    defined(CONFIG_MSTAR_TITANIA3) || \
    defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_EMERALD)

#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE)    || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)

#define MSTAR_UART_RX          (0x00)
#define MSTAR_UART_TX          (0x00)
#define MSTAR_UART_DLL         (0x00)
#define MSTAR_UART_DLM         (0x02)
#define MSTAR_UART_IER         (0x02)
#define MSTAR_UART_IIR         (0x04)
#define MSTAR_UART_FCR         (0x04)
#define MSTAR_UART_LSR         (0x0A)
#else
#define MSTAR_UART_RX          (0)
#define MSTAR_UART_TX          (0)
#define MSTAR_UART_DLL         (0)
#define MSTAR_UART_DLM         (1)
#define MSTAR_UART_IER         (1)
#define MSTAR_UART_IIR         (2)
#define MSTAR_UART_FCR         (2)
#define MSTAR_UART_LSR         (5)
#endif

#if defined(CONFIG_MSTAR_URANUS4_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA4_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA8_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA9_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA11_BD_FPGA) || \
    defined(CONFIG_MSTAR_JANUS2_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA12_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA13_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER1_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER2_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER5_BD_FPGA) || \
    defined(CONFIG_MSTAR_KRONUS_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER6_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER7_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMETHYST_BD_FPGA) || \
    defined(CONFIG_MSTAR_KAISERIN_BD_FPGA) || \
    defined(CONFIG_MSTAR_AGATE_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER3_BD_FPGA) || \
    defined(CONFIG_MSTAR_EAGLE_BD_FPGA) || \
    defined(CONFIG_MSTAR_EMERALD_BD_FPGA) || \
    defined(CONFIG_MSTAR_EDISON_BD_FPGA) || \
    defined(CONFIG_MSTAR_EINSTEIN_BD_FPGA) || \
    defined(CONFIG_MSTAR_EIFFEL_BD_FPGA)

//because 12M is too slow, reduce timeout period.
#define GETRAWUART_TIMEOUT 0xFFFF
#else
#define GETRAWUART_TIMEOUT 0xFFFF
#endif

U32 MDrv_SYS_GetRawUART(U32 arg)
{
    //unsigned int flags ;

    unsigned int i, c, ch, offset, size, force_e ;
    unsigned char uart_int, uart_int_old ;
    //unsigned int dummy ;

    // disable Rx/Tx interrupts.
    uart_int_old = uart_int = uartreg(MSTAR_UART_IER<<2) ;
    uart_int &= (~0x03) ;
    uartreg(MSTAR_UART_IER<<2) = uart_int ;

    g_in_raw_get_mode = 1 ;

    // to make sure Rx FIFO is empty
    while( uartreg(MSTAR_UART_LSR<<2)&1 ){
        uart_int = uartreg(MSTAR_UART_RX<<2) ;
    }

    //printk( "start get\n" ) ;

    offset = 0 ;
    force_e = 0 ;

start_raw_get:
    i = 0 ;
    while( i<RAWUART_MAX_SIZE )
	{
        c = 0 ;
        while( 0==(uartreg(MSTAR_UART_LSR<<2)&1) )
		{
            c++ ;
            if((i>0&&c>=GETRAWUART_TIMEOUT )||(c>=0xFFFFF))
			{
                //printk( "no more data!\n" ) ;
                force_e = 1 ;
                goto force_end ;
            }
        }
        ch = uartreg(MSTAR_UART_RX<<2) ;
        //dummy = uartreg(3<<2) ;
        g_get_raw_uart[i++] = (ch&0xFF) ;
    }


force_end:
	if(i>0)
	{
        size = i+offset ;
        copy_to_user( (void *)arg, &size, sizeof(size) ) ;
        copy_to_user( (void *)arg+sizeof(size)+offset, g_get_raw_uart, i ) ;
        offset += i ;
	}
    if( !force_e && i>=RAWUART_MAX_SIZE )
        goto start_raw_get ;

    uartreg(MSTAR_UART_IER<<2) = uart_int_old ;

    g_in_raw_get_mode = 0 ;

    //printk( "done!\n" ) ;

    return 0 ;
}

#else

U32 MDrv_SYS_GetRawUART(U32 arg)
{
    //unsigned int flags ;

    unsigned int i, c, ch, offset, size, force_e ;
    unsigned char uart_int ;
    unsigned int s ;
    unsigned int aeon_image ;
    unsigned char* aeon_ptr = (unsigned char*)0xA0000000 ;

    aeon_image = 0 ;

//    local_irq_save( flags ) ;
//    local_irq_disable() ;
    uart_int = UART_REG_ONE_BYTE(5) ;
    UART_REG_ONE_BYTE(5) = 0 ;

    for( i=0; i<8; i++ )
        uartreg(0x0c) |= 0x8000 ;  // pop Rx FIFO

    while( 1==(uartreg(0x0c)&0x00000001) ) ;

    printk( "start get\n" ) ;

    offset = 0 ;
    force_e = 0 ;

start_raw_get:
    i = 0 ;
    while( i<RAWUART_MAX_SIZE ){
        c = 0 ;
        while( 0==(uartreg(0x0c)&0x00000001) ){
            c++ ;
            if( i>0 && c>=0xCFFFF ){
                printk( "no more data!\n" ) ;
                force_e = 1 ;
                goto force_end ;
            }
        }
        ch = uartreg(0x04) ;
        g_get_raw_uart[i++] = (ch&0xFF) ;
        uartreg(0x0c) |= 0x8000 ;  // pop Rx FIFO
    }


force_end:
    size = i+offset ;
    if(copy_to_user( (void *)arg, &size, sizeof(size) ) )
        {
            printk( "copy_to_user error\n" ) ;
        }
    if(copy_to_user( (void *)arg+sizeof(size)+offset, g_get_raw_uart, i ))
        {
            printk( "copy_to_user error\n" ) ;
        }

    // detect AEON image
    if( 0==offset ){
        for( s=0; s<32; s++ ){
            if( g_get_raw_uart[0]!=0 )
                break ;
        }
        if( s>=32 ){
            MDrv_SYS_DisableAeon() ;
            REG_ADDR(0x33DE) = 0x01;
            aeon_image = 1 ;
            aeon_msg[0] = 0 ;
            aeon_idx = 0 ;
        }
    }

    if( aeon_image )
        memcpy( aeon_ptr+offset, g_get_raw_uart, i ) ;


    offset += i ;
    if( !force_e && i>=RAWUART_MAX_SIZE )
        goto start_raw_get ;

    UART_REG_ONE_BYTE(5) = uart_int ;
//    local_irq_restore( flags ) ;
//    local_irq_enable() ;

    if( aeon_image ){
        REG_ADDR(0x1E00) |= 0x02;
        MDrv_SYS_EnableAeon();
        msleep(200);
        printk( "AEON reloaded\n" ) ;
    }

    printk( "done!\n" ) ;
    return 0 ;
}

#endif


#endif

void MDrv_SYS_ChangeUart( U32 arg )
{
    int uart_idx ;
    if(copy_from_user( &uart_idx, (int*)arg, sizeof(uart_idx) ) )
        {
            printk( "copy_from_user error\n" ) ;
        }
    if( 0==uart_idx ){ // 0==isp_port
        SYS_REGOP(0x0F55<<2) = 0x0314;
        SYS_REGOP(0x0F02<<2) &= ~0x0800;
        SYS_REGOP(0x0F02<<2) &= ~0x0400;
    }else if( 1==uart_idx ){ // 1==rs232_port
        SYS_REGOP(0x0F55<<2) = 0x0520;
        SYS_REGOP(0x0F02<<2) &= ~0x0800;
        SYS_REGOP(0x0F02<<2) |= 0x0400;
    }else{
        printk( "error: N/A Uart index\n" ) ;
    }
}

void MDrv_SYS_ReloadAeon( U32 arg ){
    U32 i ;
    if(copy_from_user( &i, (U32*)arg, sizeof(i) ))
        {
            printk( "copy_from_user error\n" ) ;
        }
    if( RELOAD_AEON_STOP==i ){
        MDrv_SYS_DisableAeon() ;
        REG_ADDR(0x33DE) = 0x01;
        aeon_msg[0] = 0 ;
        aeon_idx = 0 ;
    }else if( RELOAD_AEON_RESTART==i ){
        REG_ADDR(0x1E00) |= 0x02;
        MDrv_SYS_EnableAeon();
        msleep(200);
        printk( "AEON reloaded\n" ) ;
    }
}


U32 MDrv_SYS_Timer(U32 arg){
    U32 ret_timer ;

    //while(1){
    // stop and capture timer value
    SYS_REGOP( (0x3C88<<1) ) = 0x0301 ;
    // set max timer0 value
    SYS_REGOP( (0x3C80<<1) ) = 0xFFFF ;
    SYS_REGOP( (0x3C82<<1) ) = 0xFFFF ;
    SYS_REGOP( (0x3C82<<1) ) = 0xFFFF ; // one more right for delay
    ret_timer = SYS_REGOP( (0x3C86<<1) ) ;
    ret_timer <<= 16 ;
    ret_timer += SYS_REGOP( (0x3C84<<1) ) ;

    if(copy_to_user( (void *)arg, &ret_timer, sizeof(ret_timer) ))
        {
            printk( "copy_to_user error\n" ) ;
        }
    //printk( "\n\n%d\n\n", ret_timer ) ;

    // re-start timer
    SYS_REGOP( (0x3C88<<1) ) = 0x0003 ;
    SYS_REGOP( (0x3C88<<1) ) = 0x0300 ;

    //msleep(1000) ;
    //}

    return 0 ;
}



int MDrv_SYS_GetMMAP(int type, unsigned int *addr, unsigned int *len)
{
    switch(type)
    {
        case E_SYS_MMAP_LINUX_BASE:
            #ifdef LINUX_MEM_BASE_ADR
                *addr   = LINUX_MEM_BASE_ADR;
                *len    = LINUX_MEM_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_VD_3DCOMB:
            #ifdef VD_3DCOMB_BASE_ADR
                *addr   = VD_3DCOMB_BASE_ADR;
                *len    = VD_3DCOMB_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_MAD_BASE:
            #ifdef MAD_BASE_BUFFER_ADR
                *addr   = MAD_BASE_BUFFER_ADR;
                *len    = MAD_BASE_BUFFER_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_MVD_FB:
            #ifdef MVD_FRAMEBUFFER_ADR
                *addr   = MVD_FRAMEBUFFER_ADR;
                *len    = MVD_FRAMEBUFFER_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_MVD_BS:
            #ifdef MVD_BITSTREAM_ADR
                *addr   = MVD_BITSTREAM_ADR;
                *len    = MVD_BITSTREAM_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_LINUX_MEM:

            #if defined(CONFIG_MIPS)
            *addr   = LINUX_MEM_BASE_ADR;
            *len    = LINUX_MEM_LEN;
            #elif defined(CONFIG_ARM)
            *addr   = LINUX_MEM_BASE_ADR;
            *len    = LINUX_MEM_LEN;
            #endif
            break;
        case E_SYS_MMAP_VE:
            #ifdef VE_FRAMEBUFFER_ADR
                *addr   = VE_FRAMEBUFFER_ADR;
                *len    = VE_FRAMEBUFFER_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_SCALER_DNR_BUF:
            #ifdef SCALER_DNR_BUF_ADR
                *addr   = SCALER_DNR_BUF_ADR;
                *len    = SCALER_DNR_BUF_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_RLD_BUF:
            #ifdef RLD_BUF_ADR
                *addr   = RLD_BUF_ADR;
                *len    = RLD_BUF_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_TTX_BUF:
            #ifdef TTX_BUF_ADR
                *addr   = TTX_BUF_ADR;
                *len    = TTX_BUF_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_MPOOL:
            #ifdef MPOOL_ADR
                *addr   = MPOOL_ADR;
                *len    = MPOOL_LEN;
            #else
                return false;
            #endif
            break;

        case E_SYS_MMAP_SVD:
            #ifdef SVD_CPU_AVAILABLE
                *addr   = SVD_CPU_ADR;
                *len    = SVD_CPU_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_MVD_SW:
            #ifdef MVD_SW_AVAILABLE
                *addr   = MVD_SW_ADR;
                *len    = MVD_SW_LEN;
            #else
                return false;
            #endif
            break;
        case E_SYS_MMAP_SVD_ALL:
            #ifdef SVD_CPU_AVAILABLE
                *addr   = SVD_CPU_ADR;
                *len    = SVD_CPU_LEN + MVD_FRAMEBUFFER_LEN + MVD_BITSTREAM_LEN + JPD_OUTPUT_LEN;
            #else
                return false;
            #endif
            break;

        case E_SYS_MMAP_POSD0_MEM:
            #ifdef POSD0_AVAILABLE
                *addr   = POSD0_ADR;
                *len    = POSD0_LEN;
            #else
                return false;
            #endif
            break;

        case E_SYS_MMAP_POSD1_MEM:
            #ifdef POSD1_AVAILABLE
                *addr   = POSD1_ADR;
                *len    = POSD1_LEN;
            #else
                return false;
            #endif
            break;

        case E_SYS_MMAP_BIN_MEM:
            #ifdef BIN_MEM_AVAILABLE
                *addr   = BIN_MEM_ADR;
                *len    = BIN_MEM_LEN;
            #else
                return false;
            #endif
            break;

        case E_SYS_MMAP_EMAC_MEM:
            #if defined(CONFIG_MIPS)
                *addr   = EMAC_MEM_ADR;//0x200000;//
                *len    = EMAC_MEM_LEN;//0x100000;//
            #elif defined(CONFIG_ARM)
                *addr   = EMAC_MEM_ADR;//0x200000;//
                *len    = EMAC_MEM_LEN;//0x100000;//
            #endif
            break;

        case E_SYS_MMAP_MBOX_SHM:
          #ifdef  MBOX_SHM_ADR
                *addr   = MBOX_SHM_ADR;
                *len    = MBOX_SHM_LEN;
          #else
                return false;
          #endif
            break;

        case E_SYS_MMAP_CHAKRA_FW:
          #ifdef  CHAKRA_FW_ADR
                *addr   = CHAKRA_FW_ADR;
                *len    = CHAKRA_FW_LEN;
          #else
                return false;
          #endif
            break;

#ifdef CONFIG_MSTAR_KIP
        case E_SYS_MMAP_AEON_SHM:
            *addr   = AEON_SHM_ADDR;
            *len    = AEON_SHM_LEN;
              break;
#endif

        case E_SYS_MMAP_CHAKRA_SUBSYSTEM:
          #ifdef  CHAKRA_SUBSYSTEM_ADR
                *addr   = CHAKRA_SUBSYSTEM_ADR;
                *len    = CHAKRA_SUBSYSTEM_LEN;
          #else
                return false;
          #endif
            break;
        default:
            return false;
    }
    return true;
}

void MDrv_SYS_MMAP_Dump( void ){
#if 1
    return;
#else
    printk( "\n[System Memory Map]------------------\n" ) ;
    printk( "MIU_DRAM_LEN=0x%08X, %d, %dKB\n\n", MIU_DRAM_LEN, MIU_DRAM_LEN, MIU_DRAM_LEN/1024 ) ;

    printk( "LINUX_MEM_BASE_ADR=0x%08X, %d, %dKB\n", (int) LINUX_MEM_BASE_ADR,(int)LINUX_MEM_BASE_ADR, (int)LINUX_MEM_BASE_ADR/1024 ) ;
    printk( "LINUX_MEM_GAP_CHK=0x%08X, %d, %dKB\n", (int)LINUX_MEM_GAP_CHK, (int)LINUX_MEM_GAP_CHK, (int)LINUX_MEM_GAP_CHK/1024 ) ;
    printk( "LINUX_MEM_LEN=0x%08X, %d, %dKB\n", LINUX_MEM_LEN, LINUX_MEM_LEN, LINUX_MEM_LEN/1024 ) ;
    printk( "LINUX_MEM_END=0x%08X\n\n", (int)(LINUX_MEM_BASE_ADR+LINUX_MEM_LEN) );

    printk( "BIN_MEM_ADR=0x%08X, %d, %dKB\n", (int)BIN_MEM_ADR, (int)BIN_MEM_ADR,(int) BIN_MEM_ADR/1024 ) ;
    printk( "BIN_MEM_GAP_CHK=0x%08X, %d, %dKB\n", (int)BIN_MEM_GAP_CHK, (int)BIN_MEM_GAP_CHK, (int)BIN_MEM_GAP_CHK/1024 ) ;
    printk( "BIN_MEM_LEN=0x%08X, %d, %dKB\n", BIN_MEM_LEN, BIN_MEM_LEN, BIN_MEM_LEN/1024 ) ;
    printk( "BIN_MEM_END=0x%08X\n\n", (int)(BIN_MEM_ADR+BIN_MEM_LEN) );

    printk( "VE_FRAMEBUFFER_ADR=0x%08X, %d, %dKB\n", (int)VE_FRAMEBUFFER_ADR, (int)VE_FRAMEBUFFER_ADR, (int)VE_FRAMEBUFFER_ADR/1024 ) ;
    printk( "VE_FRAMEBUFFER_ADR_GAP_CHK=0x%08X, %d, %dKB\n", (int)VE_FRAMEBUFFER_ADR_GAP_CHK, (int)VE_FRAMEBUFFER_ADR_GAP_CHK, (int)VE_FRAMEBUFFER_ADR_GAP_CHK/1024 ) ;
    printk( "VE_FRAMEBUFFER_LEN=0x%08X, %d, %dKB\n", (int)VE_FRAMEBUFFER_LEN, (int)VE_FRAMEBUFFER_LEN, (int)VE_FRAMEBUFFER_LEN/1024 ) ;
    printk( "VE_FRAMEBUFFER_END=0x%08X\n\n", (int)(VE_FRAMEBUFFER_ADR+VE_FRAMEBUFFER_LEN) );
#if 0
    printk( "SCALER_DNR_BUF_ADR=0x%08X, %d, %dKB\n", (int)SCALER_DNR_BUF_ADR,(int) SCALER_DNR_BUF_ADR, (int)SCALER_DNR_BUF_ADR/1024 ) ;
    printk( "SCALER_DNR_GAP_CHK=0x%08X, %d, %dKB\n", (int)SCALER_DNR_GAP_CHK, (int)SCALER_DNR_GAP_CHK,(int) SCALER_DNR_GAP_CHK/1024 ) ;
    printk( "SCALER_DNR_BUF_LEN_EXT=0x%08X, %d, %dKB\n",(int) SCALER_DNR_BUF_LEN_EXT, (int)SCALER_DNR_BUF_LEN_EXT, (int)SCALER_DNR_BUF_LEN_EXT/1024 ) ;
    printk( "SCALER_DNR_BUF_LEN=0x%08X, %d, %dKB\n", SCALER_DNR_BUF_LEN, SCALER_DNR_BUF_LEN, SCALER_DNR_BUF_LEN/1024 ) ;
    printk( "SCALER_DNR_BUF_END=0x%08X\n\n", (int)(SCALER_DNR_BUF_ADR+SCALER_DNR_BUF_LEN));

    printk( "SCALER_DNR_W_BARRIER_ADR=0x%08X, %d, %dKB\n", (int)SCALER_DNR_W_BARRIER_ADR, (int)SCALER_DNR_W_BARRIER_ADR, (int)SCALER_DNR_W_BARRIER_ADR/1024 ) ;
    printk( "SCALER_DNR_W_GAP_CHK=0x%08X, %d, %dKB\n", (int)SCALER_DNR_W_GAP_CHK, (int)SCALER_DNR_W_GAP_CHK, (int)SCALER_DNR_W_GAP_CHK/1024 ) ;
    printk( "SCALER_DNR_W_BARRIER_LEN=0x%08X, %d, %dKB\n", SCALER_DNR_W_BARRIER_LEN, SCALER_DNR_W_BARRIER_LEN, SCALER_DNR_W_BARRIER_LEN/1024 ) ;
    printk( "SCALER_DNR_W_BARRIER_END=0x%08X\n\n", (int)(SCALER_DNR_W_BARRIER_ADR+SCALER_DNR_W_BARRIER_LEN) );
#endif
    printk( "MAD_BASE_BUFFER_ADR=0x%08X, %d, %dKB\n",(int) MAD_BASE_BUFFER_ADR,(int) MAD_BASE_BUFFER_ADR, (int)MAD_BASE_BUFFER_ADR/1024 ) ;
    printk( "MAD_BASE_BUFFER_ADR_GAP_CHK=0x%08X, %d, %dKB\n", (int)MAD_BASE_BUFFER_ADR_GAP_CHK, (int)MAD_BASE_BUFFER_ADR_GAP_CHK, (int)MAD_BASE_BUFFER_ADR_GAP_CHK/1024 ) ;
    printk( "MAD_BASE_BUFFER_LEN=0x%08X, %d, %dKB\n", MAD_BASE_BUFFER_LEN, MAD_BASE_BUFFER_LEN, MAD_BASE_BUFFER_LEN/1024 ) ;
    printk( "MAD_BASE_BUFFER_END=0x%08X\n\n", (int)(MAD_BASE_BUFFER_ADR+MAD_BASE_BUFFER_LEN) );

    printk( "VD_3DCOMB_BASE_ADR=0x%08X, %d, %dKB\n", (int)VD_3DCOMB_BASE_ADR, (int)VD_3DCOMB_BASE_ADR, (int)VD_3DCOMB_BASE_ADR/1024 ) ;
    printk( "VD_3DCOMB_GAP_CHK=0x%08X, %d, %dKB\n", (int)VD_3DCOMB_GAP_CHK, (int)VD_3DCOMB_GAP_CHK, (int)VD_3DCOMB_GAP_CHK/1024 ) ;
    printk( "VD_3DCOMB_LEN=0x%08X, %d, %dKB\n", VD_3DCOMB_LEN, VD_3DCOMB_LEN, VD_3DCOMB_LEN/1024 ) ;
    printk( "VD_3DCOMB_END=0x%08X\n\n", (int)(VD_3DCOMB_BASE_ADR+VD_3DCOMB_LEN) );

    printk( "RLD_BUF_ADR=0x%08X, %d, %dKB\n", (int)RLD_BUF_ADR, (int)RLD_BUF_ADR, (int)RLD_BUF_ADR/1024 ) ;
    printk( "RLD_BUF_GAP_CHK=0x%08X, %d, %dKB\n", (int)RLD_BUF_GAP_CHK, (int)RLD_BUF_GAP_CHK, (int)RLD_BUF_GAP_CHK/1024 ) ;
    printk( "RLD_BUF_LEN=0x%08X, %d, %dKB\n", RLD_BUF_LEN, RLD_BUF_LEN, RLD_BUF_LEN/1024 ) ;
    printk( "RLD_BUF_END=0x%08X\n\n", (int)(RLD_BUF_ADR+RLD_BUF_LEN) );

    printk( "TTX_BUF_ADR=0x%08X, %d, %dKB\n", (int)TTX_BUF_ADR, (int)TTX_BUF_ADR, (int)TTX_BUF_ADR/1024 ) ;
    printk( "TTX_BUF_GAP_CHK=0x%08X, %d, %dKB\n", (int)TTX_BUF_GAP_CHK,(int) TTX_BUF_GAP_CHK, (int)TTX_BUF_GAP_CHK/1024 ) ;
    printk( "TTX_BUF_LEN=0x%08X, %d, %dKB\n", TTX_BUF_LEN, TTX_BUF_LEN, TTX_BUF_LEN/1024 ) ;
    printk( "TTX_BUF_END=0x%08X\n\n", (int)(TTX_BUF_ADR+TTX_BUF_LEN) );

#if 0
    printk( "MVD_SW_ADR=0x%08X, %d, %dKB\n", (int)MVD_SW_ADR, (int)MVD_SW_ADR,(int) MVD_SW_ADR/1024 ) ;
    printk( "MVD_SW_ADR_GAP_CHK=0x%08X, %d, %dKB\n", (int)MVD_SW_ADR_GAP_CHK, (int)MVD_SW_ADR_GAP_CHK, (int)MVD_SW_ADR_GAP_CHK/1024 ) ;
    printk( "MVD_SW_LEN=0x%08X, %d, %dKB\n", (int)MVD_SW_LEN, (int)MVD_SW_LEN, (int)MVD_SW_LEN/1024 ) ;
    printk( "MPOOL_END=0x%08X\n\n", (int)(MVD_SW_ADR+MVD_SW_LEN) );
#endif

    printk( "MPOOL_ADR=0x%08X, %d, %dKB\n", (int)MPOOL_ADR, (int)MPOOL_ADR,(int) MPOOL_ADR/1024 ) ;
    printk( "MPOOL_GAP_CHK=0x%08X, %d, %dKB\n", (int)MPOOL_GAP_CHK, (int)MPOOL_GAP_CHK, (int)MPOOL_GAP_CHK/1024 ) ;
    printk( "MPOOL_LEN=0x%08X, %d, %dKB\n", (int)MPOOL_LEN, (int)MPOOL_LEN, (int)MPOOL_LEN/1024 ) ;
    printk( "MPOOL_END=0x%08X\n\n", (int)(MPOOL_ADR+MPOOL_LEN) );

    printk( "LINUX_MEM2_BASE_ADR=0x%08X, %d, %dKB\n", (int)LINUX_MEM2_BASE_ADR, (int)LINUX_MEM2_BASE_ADR,(int) LINUX_MEM2_BASE_ADR/1024 ) ;
    printk( "LINUX_MEM2_GAP_CHK=0x%08X, %d, %dKB\n", (int)LINUX_MEM2_GAP_CHK, (int)LINUX_MEM2_GAP_CHK, (int)LINUX_MEM2_GAP_CHK/1024 ) ;
    printk( "LINUX_MEM2_LEN=0x%08X, %d, %dKB\n", (int)LINUX_MEM2_LEN, (int)LINUX_MEM2_LEN, (int)LINUX_MEM2_LEN/1024 ) ;
    printk( "LINUX_MEM2_END=0x%08X\n\n", (int)(LINUX_MEM2_BASE_ADR+LINUX_MEM2_LEN) );
#ifdef MVD_FRAMEBUFFER_ADR
    printk( "MVD_FRAMEBUFFER_ADR=0x%08X, %d, %dKB\n", (int)MVD_FRAMEBUFFER_ADR,(int) MVD_FRAMEBUFFER_ADR, (int)MVD_FRAMEBUFFER_ADR/1024 ) ;
    printk( "MVD_FRAMEBUFFER_ADR_GAP_CHK=0x%08X, %d, %dKB\n", (int)MVD_FRAMEBUFFER_ADR_GAP_CHK, (int)MVD_FRAMEBUFFER_ADR_GAP_CHK,(int) MVD_FRAMEBUFFER_ADR_GAP_CHK/1024 ) ;
    printk( "MVD_FRAMEBUFFER_LEN=0x%08X, %d, %dKB\n", MVD_FRAMEBUFFER_LEN, MVD_FRAMEBUFFER_LEN, MVD_FRAMEBUFFER_LEN/1024 ) ;
    printk( "MVD_FRAMEBUFFER_END=0x%08X\n\n", (int)(MVD_FRAMEBUFFER_ADR+MVD_FRAMEBUFFER_LEN) );
#endif
#ifdef MVD_BITSTREAM_ADR
    printk( "MVD_BITSTREAM_ADR=0x%08X, %d, %dKB\n", (int)MVD_BITSTREAM_ADR, (int)MVD_BITSTREAM_ADR, (int)MVD_BITSTREAM_ADR/1024 ) ;
    printk( "MVD_BITSTREAM_ADR_GAP_CHK=0x%08X, %d, %dKB\n", (int)MVD_BITSTREAM_ADR_GAP_CHK, (int)MVD_BITSTREAM_ADR_GAP_CHK, (int)MVD_BITSTREAM_ADR_GAP_CHK/1024 ) ;
    printk( "MVD_BITSTREAM_LEN=0x%08X, %d, %dKB\n", MVD_BITSTREAM_LEN, MVD_BITSTREAM_LEN, MVD_BITSTREAM_LEN/1024 ) ;
    printk( "MVD_BITSTREAM_END=0x%08X\n\n", (int)(MVD_BITSTREAM_ADR+MVD_BITSTREAM_LEN) );
#endif
#endif
}

unsigned int MDrv_SYS_GetKernelMemLen( void ){
    unsigned int a, b ;
    if( MDrv_SYS_GetMMAP(E_SYS_MMAP_LINUX_BASE, &a, &b) )
        return( b ) ;
    return( 0 ) ;
}

extern int g_in_hotel_mode;

U32 MDrv_SYS_HotelMode(U32 arg){

    if (copy_from_user(&g_in_hotel_mode, (int __user*)arg, sizeof(int)))
    {
        return -EFAULT;
    }

    return( 0 ) ;
}

#define HOTEL_MODE_PRINTF_BUF   16
#define HOTEL_MODE_PRINTF_MAX   128

U32 MDrv_SYS_HotelModePrintf(U32 arg){
    int c, i ;
    unsigned char tmp[HOTEL_MODE_PRINTF_BUF] ;

    c = 0 ;
    while( 1 ){

        if (copy_from_user( tmp, (unsigned char*)(arg+(c*HOTEL_MODE_PRINTF_BUF)), HOTEL_MODE_PRINTF_BUF))
        {
            return -EFAULT;
        }

        for( i=0; i<HOTEL_MODE_PRINTF_BUF; i++ ){
            if( 0==tmp[i] ) break ;
            // char output
            *((volatile unsigned int*)(0xBF801300)) = tmp[i] ;
            //printk( "%c",tmp[i] );
        }
        if( i<HOTEL_MODE_PRINTF_BUF ) // encounter 0
            break ;

        c++ ;
        if( c>=HOTEL_MODE_PRINTF_MAX )
            break ;
        msleep(1) ;
    }

    return( 0 ) ;
}



void MDrv_SYS_PD_ADC_R(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2508) &= ~BIT5; //ADC_R
    }
    else
    {
        REG_ADDR(0x2508) |= BIT5; //ADC_R
    }
}

void MDrv_SYS_PD_ADC_G(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2508) &= ~BIT4; //ADC_G
    }
    else
    {
        REG_ADDR(0x2508) |= BIT4; //ADC_G
    }
}

void MDrv_SYS_PD_ADC_B(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2508) &= ~BIT3; //ADC_B
    }
    else
    {
        REG_ADDR(0x2508) |= BIT3; //ADC_B
    }
}

void MDrv_SYS_PD_ADC_Y(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2508) &= ~BIT12; //ADC_Y
    }
    else
    {
        REG_ADDR(0x2508) |= BIT12; //ADC_Y
    }
}

void MDrv_SYS_PD_GMC_P(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x250a) &= ~(BIT4 | BIT3 | BIT2); //GMC parameter
    }
    else
    {
        REG_ADDR(0x250a) |= (BIT4 | BIT3 | BIT2); //GMC parameter
    }
}

void MDrv_SYS_PD_GMC_Y(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x250a) &= ~BIT1; //GMC_Y
    }
    else
    {
        REG_ADDR(0x250a) |= BIT1; //GMC_Y
    }
}

void MDrv_SYS_PD_GMC_C(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x250a) &= ~BIT0; //GMC_C
    }
    else
    {
        REG_ADDR(0x250a) |= BIT0; //GMC_C
    }
}

void MDrv_SYS_PD_CVBS_Buffer(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2570) &= ~BIT0; //CVBS output buffer
    }
    else
    {
        REG_ADDR(0x2570) |= BIT0; //CVBS output buffer
    }
}

void MDrv_SYS_PD_DAC_CVBS(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x258c) |= BIT7; //DAC_CVBS
    }
    else
    {
        REG_ADDR(0x258c) &= ~BIT7; //DAC_CVBS
    }
}

void MDrv_SYS_PD_DAC(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2534) = 0x001d; //adc_ibias_rgb
        REG_ADDR(0x253a) = 0x001d; //adc_ibias_ycu
        REG_ADDR(0x25ba) |= BIT0;
        REG_ADDR(0x25b8) |= (BIT15 | BIT14);
        REG_ADDR(0x2606) &= 0xffc0; //phase_delta
        REG_ADDR(0x2606) = 0x0500; //phase_delta
    }
    else
    {
        REG_ADDR(0x2534) = 0x001d; //adc_ibias_rgb
        REG_ADDR(0x253a) = 0x001d; //adc_ibias_ycu
        REG_ADDR(0x25ba) |= BIT0;
        REG_ADDR(0x25b8) |= (BIT15 | BIT14);
        REG_ADDR(0x2606) &= 0xffc0; //phase_delta
        REG_ADDR(0x2606) = 0x0500; //phase_delta
    }
}

void MDrv_SYS_PD_FB_DAC(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2580) &= ~BIT6; //fast blanking ADC
    }
    else
    {
        REG_ADDR(0x2580) |= BIT6; //fast blanking ADC
    }
}

void MDrv_SYS_PD_DAC_RGB(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2536) = 0x3fff; //ADC RGB bias current control
        REG_ADDR(0x253c) = 0x3fff; //ADC RGB bias current control
    }
    else
    {
        REG_ADDR(0x2536) = 0x0555; //ADC RGB bias current control
        REG_ADDR(0x253c) = 0x0555; //ADC RGB bias current control
    }
}

void MDrv_SYS_PD_Audio(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2ce0) = 0x0000; //Audio low power mode
    }
    else
    {
        REG_ADDR(0x2ce0) = 0x5550; //Audio low power mode
    }
}

void MDrv_SYS_PD_LVDS(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x32f0) &= 0xff00; //LVDS
        REG_ADDR(0x32f0) |= (BIT6 | BIT3 | BIT2); //LVDS
        REG_ADDR(0x32da) = 0x5550; //LVDS
        REG_ADDR(0x32dc) = 0x0555; //LVDS
        REG_ADDR(0x32de) = 0x0000; //LVDS
    }
    else
    {
        REG_ADDR(0x32f0) &= 0xff00; //LVDS
        REG_ADDR(0x32f0) |= (BIT6 | BIT2 | BIT1); //LVDS
        REG_ADDR(0x32da) = 0x5550; //LVDS
        REG_ADDR(0x32dc) = 0x0555; //LVDS
        REG_ADDR(0x32de) = 0x0000; //LVDS
    }
}

void MDrv_SYS_PD_VD(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e2e) &= ~BIT0; //VD
        REG_ADDR(0x1e2e) &= ~BIT4; //VD
        REG_ADDR(0x1e2c) &= ~BIT12; //VD
        REG_ADDR(0x3612) &= ~BIT14; //VD
        REG_ADDR(0x3800) &= ~BIT15; //VD
        REG_ADDR(0x3770) &= ~BIT3; //VD
        REG_ADDR(0x3514) &= ~BIT7; //VD
        REG_ADDR(0x130c) &= ~BIT0; //VD
        REG_ADDR(0x1246) &= ~BIT11; //VD
        REG_ADDR(0x1266) &= ~BIT8; //VD
        REG_ADDR(0x1266) &= ~BIT9; //VD
        REG_ADDR(0x1286) &= ~BIT8; //VD
        REG_ADDR(0x0666) &= ~BIT8; //VD
        REG_ADDR(0x0666) &= ~BIT9; //VD
    }
    else
    {
        REG_ADDR(0x3612) |= BIT14; //VD
        REG_ADDR(0x3800) |= BIT15; //VD
        REG_ADDR(0x3770) |= BIT3; //VD
        REG_ADDR(0x3514) |= BIT7; //VD
        REG_ADDR(0x130c) |= BIT0; //VD
        REG_ADDR(0x1246) |= BIT11; //VD
        REG_ADDR(0x1266) |= BIT8; //VD
        REG_ADDR(0x1266) |= BIT9; //VD
        REG_ADDR(0x1286) |= BIT8; //VD
        REG_ADDR(0x0666) |= BIT8; //VD
        REG_ADDR(0x0666) |= BIT9; //VD
        REG_ADDR(0x1e2e) |= BIT0; //VD
        REG_ADDR(0x1e2e) |= BIT4; //VD
        REG_ADDR(0x1e2c) |= BIT12; //VD
    }
}

void MDrv_SYS_PD_SVD(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e58) &= ~BIT0; //SVD
        REG_ADDR(0x1b14) &= ~BIT0; //SVD
        REG_ADDR(0x1b14) &= ~BIT1; //SVD
        REG_ADDR(0x1246) &= ~BIT12; //SVD
        REG_ADDR(0x1246) &= ~BIT13; //SVD
        REG_ADDR(0x1286) &= ~BIT0; //SVD
        REG_ADDR(0x1286) &= ~BIT6; //SVD
        REG_ADDR(0x0646) &= ~BIT7; //SVD
        REG_ADDR(0x0646) &= ~BIT8; //SVD
        REG_ADDR(0x0646) &= ~BIT9; //SVD
        REG_ADDR(0x0646) &= ~BIT10; //SVD
    }
    else
    {
        REG_ADDR(0x1b14) |= BIT0; //SVD
        REG_ADDR(0x1b14) |= BIT1; //SVD
        REG_ADDR(0x1246) |= BIT12; //SVD
        REG_ADDR(0x1246) |= BIT13; //SVD
        REG_ADDR(0x1286) |= BIT0; //SVD
        REG_ADDR(0x1286) |= BIT6; //SVD
        REG_ADDR(0x0646) |= BIT7; //SVD
        REG_ADDR(0x0646) |= BIT8; //SVD
        REG_ADDR(0x0646) |= BIT9; //SVD
        REG_ADDR(0x0646) |= BIT10; //SVD
        REG_ADDR(0x1e58) |= BIT0; //SVD
    }
}

void MDrv_SYS_PD_MVD_M4V(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e28) &= ~BIT12; //MVD & M4V
        REG_ADDR(0x1e28) &= ~BIT8; //MVD & M4V
        REG_ADDR(0x1e2a) &= ~BIT0; //MVD & M4V
        REG_ADDR(0x1e28) |= BIT9; //MVD & M4V
        REG_ADDR(0x1100) &= ~BIT0; //MVD & M4V
        REG_ADDR(0x1286) &= ~BIT6; //MVD & M4V
        REG_ADDR(0x1286) &= ~BIT11; //MVD & M4V
        REG_ADDR(0x0646) &= ~BIT9; //MVD & M4V
        REG_ADDR(0x0686) &= ~BIT11; //MVD & M4V
    }
    else
    {
        REG_ADDR(0x1100) |= BIT0; //MVD & M4V
        REG_ADDR(0x1286) |= BIT6; //MVD & M4V
        REG_ADDR(0x1286) |= BIT11; //MVD & M4V
        REG_ADDR(0x0646) |= BIT9; //MVD & M4V
        REG_ADDR(0x0686) |= BIT11; //MVD & M4V
        REG_ADDR(0x1e28) |= BIT12; //MVD & M4V
        REG_ADDR(0x1e28) |= BIT8; //MVD & M4V
        REG_ADDR(0x1e2a) |= BIT0; //MVD & M4V
    }
}

void MDrv_SYS_PD_TSP(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e26) &= ~BIT0; //TSP
        REG_ADDR(0x1e26) &= ~BIT8; //TSP
        REG_ADDR(0x1246) &= ~BIT10; //TSP
        REG_ADDR(0x1286) &= ~BIT1; //TSP
    }
    else
    {
        REG_ADDR(0x1e26) |= BIT0; //TSP
        REG_ADDR(0x1e26) |= BIT8; //TSP
        REG_ADDR(0x1246) |= BIT10; //TSP
        REG_ADDR(0x1286) |= BIT1; //TSP
    }
}

void MDrv_SYS_PD_VE(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e32) &= ~BIT8; //VE
        REG_ADDR(0x1e32) &= ~BIT12; //VE
        REG_ADDR(0x1e38) &= ~BIT0; //VE
        REG_ADDR(0x3b00) &= ~BIT4; //VE
        REG_ADDR(0x1246) &= ~BIT8; //VE
        REG_ADDR(0x1266) &= ~BIT10; //VE
        REG_ADDR(0x1266) &= ~BIT11; //VE
        REG_ADDR(0x0666) &= ~BIT5; //VE
        REG_ADDR(0x0666) &= ~BIT10; //VE
        REG_ADDR(0x0666) &= ~BIT11; //VE
    }
    else
    {
        REG_ADDR(0x3b00) &= 0xff00; //VE
        REG_ADDR(0x3b00) |= BIT4; //VE
        REG_ADDR(0x1246) |= BIT8; //VE
        REG_ADDR(0x1266) |= BIT10; //VE
        REG_ADDR(0x1266) |= BIT11; //VE
        REG_ADDR(0x0666) |= BIT5; //VE
        REG_ADDR(0x0666) |= BIT10; //VE
        REG_ADDR(0x0666) |= BIT11; //VE
        REG_ADDR(0x1e32) |= BIT8; //VE
        REG_ADDR(0x1e32) |= BIT12; //VE
        REG_ADDR(0x1e38) |= BIT0; //VE
    }
}

void MDrv_SYS_PD_RVD(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1286) |= BIT6; //RVD
        REG_ADDR(0x0646) |= BIT9; //RVD
        REG_ADDR(0x1e2a) |= BIT8; //RVD
    }
    else
    {
        REG_ADDR(0x1286) |= BIT6; //RVD
        REG_ADDR(0x0646) |= BIT9; //RVD
        REG_ADDR(0x1e2a) |= BIT8; //RVD
    }
}

//20100120 Terry, load code
U32 MDrv_SYS_SPI_LOAD(U32 arg)
{
	U8 *pf;
	IO_SYS_SPI_t spi;
	if(copy_from_user( &spi,(IO_SYS_SPI_t __user *)arg, sizeof(IO_SYS_SPI_t)))
       {
            printk("copy_from_user error\n");
       }

	pf=(unsigned char *)spi.u32Start;
        memcpy(spi.u8data,pf,spi.u32Len);

//----test------
//	printk("\nstart: 0x%08x   , size: %d bytes \n",spi.u32Start,spi.u32Len);
//	unsigned int i;
//	printk("\nMDrv_SYS_SPI_LOAD\n");
//	for (i=0;i<0x10;i++)  printk("0x%02x ",*(spi.u8data+i));

	return copy_to_user((void *)arg,(void *)&spi,sizeof(IO_SYS_SPI_t));

}


void MDrv_SYS_PD_STRLD(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e44) &= ~BIT0; //STRLD
        REG_ADDR(0x1246) &= ~BIT3; //STRLD
        REG_ADDR(0x1246) &= ~BIT4; //STRLD
    }
    else
    {
        REG_ADDR(0x1e44) |= BIT0; //STRLD
        REG_ADDR(0x1246) |= BIT3; //STRLD
        REG_ADDR(0x1246) |= BIT4; //STRLD
    }
}

void MDrv_SYS_PD_AEON(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e26) &= ~BIT6; //AEON
    }
    else
    {
        REG_ADDR(0x1e26) |= BIT6; //AEON
    }
}

void MDrv_SYS_PD_GOPG2(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e30) &= ~BIT8; //GOPG2
    }
    else
    {
        REG_ADDR(0x1e30) |= BIT8; //GOPG2
    }
}

void MDrv_SYS_SetGFXGOPIndex(U32 argv)
{
    gfx_gop_index=(int)argv;
}

void MDrv_SYS_GetGFXGOPIndex(U32 argv)
{

   if(copy_to_user((void*)argv, (void*)&gfx_gop_index, sizeof(gfx_gop_index)))
    {
         printk( "copy_to_user error\n" ) ;
    }

}

void MDrv_SYS_SetDisplayControllerSeparated(U32 argv)
{
    display_controller_seperated=(int)argv;
}

U32 MDrv_SYS_GetDisplayControllerSeparated(void)
{
    return display_controller_seperated;
}

void MDrv_SYS_IsDisplayControllerSeparated(U32 argv)
{

    if(copy_to_user((void*)argv, (void*)&display_controller_seperated, sizeof(display_controller_seperated)))
        {
             printk( "copy_to_user error\n" ) ;
        }

}

void MDrv_SYS_SetNexus(U32 argv)
{
    using_nexus=(int)argv;
}

void MDrv_SYS_HasNexus(U32 argv)
{

    if(copy_to_user((void*)argv, (void*)&using_nexus, sizeof(using_nexus)))
        {
             printk( "copy_to_user error\n" ) ;
        }

}

void MDrv_SYS_SetNexusPID(U32 argv)
{
    nexus_pid=(int)argv;
}

void MDrv_SYS_GetNexusPID(U32 argv)
{

    if(copy_to_user((void*)argv, (void*)&nexus_pid, sizeof(nexus_pid)))
        {
             printk( "copy_to_user error\n" ) ;
        }
}

static inline BOOL _MDrv_PCMCIA_ReadReg( U8 u8Addr, U8 *pu8Value )
{
    U16 u16Addr;

    if ( NULL == pu8Value )
    {
        return FALSE;
    }

    //*pu8Value = HAL_PCMCIA_Read_Byte( u8Addr );
    if(u8Addr&0x01)
    {
        u16Addr = REG_PCMCIA_BASE + (u8Addr - 1);
        *pu8Value = (REG_ADDR(u16Addr) >> 8);
    }
    else
    {
        u16Addr = REG_PCMCIA_BASE + (u8Addr);
        *pu8Value = (REG_ADDR(u16Addr) & 0xFF);
    }

    return TRUE;
}

static inline BOOL _MDrv_PCMCIA_WriteReg( U8 u8Addr, U8 u8Value )
{
    U16 u16Addr;

    //HiByte
    if(u8Addr&0x01)
    {
        u16Addr = REG_PCMCIA_BASE + (u8Addr - 1);
        REG_ADDR(u16Addr) &=0x00FF;
        REG_ADDR(u16Addr) |= (u8Value << 8);
    }
    else  // LoByte
    {
        u16Addr = REG_PCMCIA_BASE + (u8Addr);
        REG_ADDR(u16Addr) &=0xFF00;
        REG_ADDR(u16Addr) |= u8Value;
    }

    return TRUE;
}

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
static BOOL _MDrv_PCMCIA_WaitEMMCDone(U32 u32loopCnt)
{
    return mstar_mci_exit_checkdone_ForCI();
}
#endif

U32 MDrv_SYS_PCMCIA_WRITE(U32 arg)
{
    PCMCIA_Map_Info_t op ;
    U16 i, index;
    U8 u8Reg;
    U16 u16RegVal;
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    U16 u16RegVal2;
#endif

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16Bypass;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16RegVal4;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16RegVal1, u16RegVal3;
#endif
    if(copy_from_user( &op, (PCMCIA_Map_Info_t*)arg, sizeof(op) ))
    {
        printk( "copy_from_user error\n" ) ;
        return -EFAULT;        
    }
    
    if(op.u8Type == 3) // write a block of data
    {
        if((!access_ok(VERIFY_WRITE, (void *)op.u8pWriteBuffer, op.u16DataLen)) || 
           (!access_ok(VERIFY_READ, (void *)op.u8pWriteBuffer, op.u16DataLen)))
        {
            return -EFAULT;
        }
        
        // touch memory to prevent page fault inside the mutex lock
        if(copy_from_user((void *)op.u8pWriteBuffer, (void *)op.u8pWriteBuffer, op.u16DataLen))
        {
            return -EFAULT;
        }
    }
    PCMCIA_NAND_SEM(down(&PfModeSem));

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
    _MDrv_PCMCIA_WaitEMMCDone(PCMCIA_LOOP_DELAY_WAIT_EMMC_DONE);
#endif

    // Switch to PCMCIA
    u16RegVal = REG_ADDR( REG_CHIP_NAND_MODE ) & REG_CHIP_NAND_MODE_MASK;
    REG_ADDR( REG_CHIP_NAND_MODE ) &= ~REG_CHIP_NAND_MODE_MASK;

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16Bypass = REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) & REG_CHIP_PCM_NAND_BYPASS_MASK;
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= REG_CHIP_PCM_NAND_BYPASS_ENABLE;
#endif

#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    u16RegVal2 = REG_ADDR(REG_CHIP_PF_MODE) & REG_CHIP_PF_MODE_MASK;
    REG_ADDR(REG_CHIP_PF_MODE) &= ~REG_CHIP_PF_MODE_MASK;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16RegVal3 = REG_ADDR(REG_EMMC_CONFIG) & REG_EMMC_CONFIG_MASK;
    REG_ADDR(REG_EMMC_CONFIG) &= ~REG_EMMC_CONFIG_MASK;
    u16RegVal1 = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
    REG_ADDR(REG_CHIP_PCMCFG) |= REG_CHIP_PCMCFG_CTRL_EN;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16RegVal4 = REG_ADDR(REG_SD_CONFIG2) & REG_SD_CONFIG2_MASK;
    REG_ADDR(REG_SD_CONFIG2) &= ~REG_SD_CONFIG2_MASK;
#endif

    // AttribMem
    if(1 == op.u8Type)
    {
        // select attribute memory write, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_ATTRIBMEMORY_WRITE );

        // write address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( op.u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, op.u16Addr );

        // write data
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_WRITE_DATA, op.u8Value );

        // fire command
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );


        //polling if fire is done
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

            if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
            {
                break;
            }
        }

        // polling if bus is idle
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );
            if ( u8Reg & PCMCIA_STATE_BUS_IDLE )
            {
                break;
            }
        }
    }
    else if( 2 == op.u8Type )  // IOMem
    {
        // select attribute memory write, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_IO_WRITE );

        // write address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( op.u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, op.u16Addr );
        // write data
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_WRITE_DATA, op.u8Value );

        // fire command
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );


        //polling if fire is done
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

            if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
            {
                break;
            }
        }

        // polling if bus is idle
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( u8Reg & PCMCIA_STATE_BUS_IDLE )
            {
                break;
            }
        }
    }
    else
    {

#if KERNEL_PCMCIA_PERF
        struct timeval tv1, tv2;
        do_gettimeofday(&tv1);
#endif
                
        // select attribute memory write, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_IO_WRITE );

        // write address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( op.u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, op.u16Addr );
        for(index=0;index < op.u16DataLen; ++index )
        {
            // write data
            _MDrv_PCMCIA_WriteReg( REG_PCMCIA_WRITE_DATA, op.u8pWriteBuffer[index] );

            // fire command
            _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );

            //polling if fire is done
            for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

                if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
                {
                    break;
                }
            }

            // polling if bus is idle
            for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

                if ( u8Reg & PCMCIA_STATE_BUS_IDLE )
                {
                    break;
                }
            }
        }
#if KERNEL_PCMCIA_PERF
        do_gettimeofday(&tv2);
        if(op.u16DataLen > 10)
        {
            printf("\n\n[MDrv_SYS_PCMCIA_WRITE_DATA]: Len(%d), Time(%d.%d - %d.%d), Diff(0.%06d)\n\n",
            op.u16DataLen, 
            (int)tv1.tv_sec, 
            (int)tv1.tv_usec, 
            (int)tv2.tv_sec, 
            (int)tv2.tv_usec, 
            (int)((tv2.tv_sec*1000000+tv2.tv_usec) - (tv1.tv_sec*1000000+tv1.tv_usec)))
        } 
#endif
    }


#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_EMMC_CONFIG) = (REG_ADDR(REG_EMMC_CONFIG) & ~REG_EMMC_CONFIG_MASK) | u16RegVal3;
    REG_ADDR(REG_CHIP_PCMCFG) = (REG_ADDR(REG_CHIP_PCMCFG) & ~REG_CHIP_PCMCFG_MASK) | u16RegVal1;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_SD_CONFIG2) = (REG_ADDR(REG_SD_CONFIG2) & ~REG_SD_CONFIG2_MASK) | u16RegVal4;
#endif

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) &= ~REG_CHIP_PCM_NAND_BYPASS_MASK;
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= u16Bypass;
#endif

    // Switch to Nand
    REG_ADDR( REG_CHIP_NAND_MODE ) |= u16RegVal; //(BIT0|BIT2);
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    REG_ADDR( REG_CHIP_PF_MODE ) |= u16RegVal2;
#endif

    PCMCIA_NAND_SEM(up(&PfModeSem));
    return 0;
}

U32 MDrv_SYS_PCMCIA_READ(U32 arg)
{
    PCMCIA_Map_Info_t op ;
    U16 u16Addr, i;
    U8 u8Value=0, u8Type, u8Reg;
    U8 u8TryLoop = 0;
    U16 u16RegVal;
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    U16 u16RegVal2;
#endif

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16Bypass;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16RegVal4;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16RegVal1, u16RegVal3;
#endif

    if(copy_from_user( &op, (PCMCIA_Map_Info_t*)arg, sizeof(op) ))
    {
        printk( "copy_from_user error\n" ) ;
        return -EFAULT;        
    }
    u16Addr = op.u16Addr;
    u8Type = op.u8Type;

    PCMCIA_NAND_SEM(down(&PfModeSem));

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
    _MDrv_PCMCIA_WaitEMMCDone(PCMCIA_LOOP_DELAY_WAIT_EMMC_DONE);
#endif

    // Switch to PCMCIA
    u16RegVal = REG_ADDR( REG_CHIP_NAND_MODE ) & REG_CHIP_NAND_MODE_MASK;
    REG_ADDR( REG_CHIP_NAND_MODE ) &= ~REG_CHIP_NAND_MODE_MASK;

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16Bypass = REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) & REG_CHIP_PCM_NAND_BYPASS_MASK;
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= REG_CHIP_PCM_NAND_BYPASS_ENABLE;
#endif

#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    u16RegVal2 = REG_ADDR(REG_CHIP_PF_MODE) & REG_CHIP_PF_MODE_MASK;
    REG_ADDR(REG_CHIP_PF_MODE) &= ~REG_CHIP_PF_MODE_MASK;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16RegVal3 = REG_ADDR(REG_EMMC_CONFIG) & REG_EMMC_CONFIG_MASK;
    REG_ADDR(REG_EMMC_CONFIG) &= ~REG_EMMC_CONFIG_MASK;
    u16RegVal1 = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
    REG_ADDR(REG_CHIP_PCMCFG) |= REG_CHIP_PCMCFG_CTRL_EN;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16RegVal4 = REG_ADDR(REG_SD_CONFIG2) & REG_SD_CONFIG2_MASK;
    REG_ADDR(REG_SD_CONFIG2) &= ~REG_SD_CONFIG2_MASK;
#endif
    // AttribMem
    if(1 == u8Type)
    {
        // select attribute memory read, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_ATTRIBMEMORY_READ );

        // read address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, (U8)( ( u16Addr * 2 ) >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, (U8)( u16Addr * 2 ) );

        // fire command
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );

        //polling if fire is done
        while (1)
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

            if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
            {
                break;
            }
            else
            {
                u8TryLoop++;
                if ( u8TryLoop > PCMCIA_HW_MAX_RETRY_COUNT )
                {
                    u8TryLoop = 0;
                    printf( "[%s:%d][Warning!][PCMCIA] Timeout!\n", __FILE__, __LINE__ );
                    goto EXIT_FUNCTION;
                }
            }
        }

        //polling if data ready
        while (1)
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( u8Reg & PCMCIA_STATE_RD_DONE )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA, &u8Value );
                break;
            }
            else
            {
                u8TryLoop++;
                if ( u8TryLoop > PCMCIA_HW_MAX_RETRY_COUNT )
                {
                    u8TryLoop = 0;
                    printf( "[%s:%d][Warning!][PCMCIA] Timeout!\n", __FILE__, __LINE__ );
                    goto EXIT_FUNCTION;
                }
            }
        }

        // clean stat_rd done
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_CLEAN_STATE_RD_DONE );

        // polling if bus is idle
        while (1)
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( PCMCIA_STATE_BUS_IDLE == ( u8Reg & ( PCMCIA_STATE_BUS_IDLE | PCMCIA_STATE_RD_DONE ) ) )
            {
                break;
            }
            else
            {
                u8TryLoop++;
                if ( u8TryLoop > PCMCIA_HW_MAX_RETRY_COUNT )
                {
                    u8TryLoop = 0;
                    printf( "[%s:%d][Warning!][PCMCIA] Timeout!\n", __FILE__, __LINE__ );
                    goto EXIT_FUNCTION;
                }
            }
        }
    }
    else  // IOMem
    {
        // select attribute memory read, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_IO_READ );

        // read address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, u16Addr);

        // fire command
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );

        //polling if fire is done
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

            if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
            {
                break;
            }
        }

        //polling if data ready
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( u8Reg & PCMCIA_STATE_RD_DONE )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA, &u8Value );
                break;
            }
        }

        // clean stat_rd done
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_CLEAN_STATE_RD_DONE );

        // polling if bus is idle
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( PCMCIA_STATE_BUS_IDLE == ( u8Reg & ( PCMCIA_STATE_BUS_IDLE | PCMCIA_STATE_RD_DONE ) ) )
            {
                break;
            }
        }
    }

    op.u8Value = u8Value;

EXIT_FUNCTION:

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_EMMC_CONFIG) = (REG_ADDR(REG_EMMC_CONFIG) & ~REG_EMMC_CONFIG_MASK) | u16RegVal3;
    REG_ADDR(REG_CHIP_PCMCFG) = (REG_ADDR(REG_CHIP_PCMCFG) & ~REG_CHIP_PCMCFG_MASK) | u16RegVal1;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_SD_CONFIG2) = (REG_ADDR(REG_SD_CONFIG2) & ~REG_SD_CONFIG2_MASK) | u16RegVal4;
#endif

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) &= ~REG_CHIP_PCM_NAND_BYPASS_MASK;
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= u16Bypass;
#endif

    // Switch to Nand
    REG_ADDR( REG_CHIP_NAND_MODE ) |= u16RegVal; //(BIT0|BIT2);
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    REG_ADDR( REG_CHIP_PF_MODE ) |= u16RegVal2;
#endif

    PCMCIA_NAND_SEM(up(&PfModeSem));
    if(copy_to_user( (void *)arg, &op, sizeof(op) ))
    {
        printk( "copy_to_user error\n" ) ;
        return -EFAULT;
    }
    return 0;
}

void MDrv_SYS_PCMCIA_WRITE2(U16 u16Addr, U8 u8Value, U8 type, U16 u16DataLen, U8* u8pWriteBuffer)
{
    U16 i, index;
    U8 u8Reg;

    // AttribMem
    if(1 == type)
    {
        // select attribute memory write, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_ATTRIBMEMORY_WRITE );

        // write address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, u16Addr );

        // write data
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_WRITE_DATA, u8Value );

        // fire command
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );


        //polling if fire is done
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

            if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
            {
                break;
            }
        }

        // polling if bus is idle
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );
            if ( u8Reg & PCMCIA_STATE_BUS_IDLE )
            {
                break;
            }
        }
    }
    else if( 2 == type )  // IOMem
    {
        // select attribute memory write, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_IO_WRITE );

        // write address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, u16Addr );
        // write data
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_WRITE_DATA, u8Value );

        // fire command
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );


        //polling if fire is done
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

            if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
            {
                break;
            }
        }

        // polling if bus is idle
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( u8Reg & PCMCIA_STATE_BUS_IDLE )
            {
                break;
            }
        }
    }
    else
    {
        // select attribute memory write, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_IO_WRITE );

        // write address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, u16Addr );
        for(index=0;index < u16DataLen; ++index )
        {
            // write data
            _MDrv_PCMCIA_WriteReg( REG_PCMCIA_WRITE_DATA, u8pWriteBuffer[index] );

            // fire command
            _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );

            //polling if fire is done
            for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

                if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
                {
                    break;
                }
            }

            // polling if bus is idle
            for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

                if ( u8Reg & PCMCIA_STATE_BUS_IDLE )
                {
                    break;
                }
            }
        }
    }
}

U8 MDrv_SYS_PCMCIA_READ2(U16 u16Addr, U8 u8Type, U16 u16DataLen, U8 * u8pReadBuffer)
{
    U16 i, index;
    U8 u8Reg;
    U8 u8TryLoop = 0;
    U8 u8Value = 0;

    // AttribMem
    if(1 == u8Type)
    {
        // select attribute memory read, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_ATTRIBMEMORY_READ );

        // read address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, (U8)( ( u16Addr * 2 ) >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, (U8)( u16Addr * 2 ) );

        // fire command
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );

        //polling if fire is done
        while (1)
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

            if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
            {
                break;
            }
            else
            {
                u8TryLoop++;
                if ( u8TryLoop > PCMCIA_HW_MAX_RETRY_COUNT )
                {
                    u8TryLoop = 0;
                    printf( "[%s:%d][Warning!][PCMCIA] Timeout!\n", __FILE__, __LINE__ );

                    return 0;
                }
            }
        }

        //polling if data ready
        while (1)
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( u8Reg & PCMCIA_STATE_RD_DONE )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA, &u8Value );
                break;
            }
            else
            {
                u8TryLoop++;
                if ( u8TryLoop > PCMCIA_HW_MAX_RETRY_COUNT )
                {
                    u8TryLoop = 0;
                    printf( "[%s:%d][Warning!][PCMCIA] Timeout!\n", __FILE__, __LINE__ );

                    return 0;
                }
            }
        }

        // clean stat_rd done
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_CLEAN_STATE_RD_DONE );

        // polling if bus is idle
        while (1)
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( PCMCIA_STATE_BUS_IDLE == ( u8Reg & ( PCMCIA_STATE_BUS_IDLE | PCMCIA_STATE_RD_DONE ) ) )
            {
                break;
            }
            else
            {
                u8TryLoop++;
                if ( u8TryLoop > PCMCIA_HW_MAX_RETRY_COUNT )
                {
                    u8TryLoop = 0;
                    printf( "[%s:%d][Warning!][PCMCIA] Timeout!\n", __FILE__, __LINE__ );

                    return 0;
                }
            }
        }
    }
    else if(2 == u8Type) // IOMem
    {
        // select attribute memory read, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_IO_READ );

        // read address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, u16Addr);

        // fire command
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );

        //polling if fire is done
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

            if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
            {
                break;
            }
        }

        //polling if data ready
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( u8Reg & PCMCIA_STATE_RD_DONE )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA, &u8Value );
                break;
            }
        }

        // clean stat_rd done
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_CLEAN_STATE_RD_DONE );

        // polling if bus is idle
        for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
        {
            _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

            if ( PCMCIA_STATE_BUS_IDLE == ( u8Reg & ( PCMCIA_STATE_BUS_IDLE | PCMCIA_STATE_RD_DONE ) ) )
            {
                break;
            }
        }
    }
    else
    {
#if KERNEL_PCMCIA_PERF        
        struct timeval tv1, tv2;
        do_gettimeofday(&tv1);
#endif
                
        // select attribute memory read, low byte
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_PCM_MEM_IO_CMD, PCMCIA_IO_READ );

        // read address
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR1, ( u16Addr >> 8 ) );
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_ADDR0, u16Addr);

        for(index = 0; index < u16DataLen; ++index)
        {
            // fire command
            _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_FIRE_COMMAND );

            //polling if fire is done
            for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, (U8 *)&u8Reg );

                if ( !( u8Reg & PCMCIA_FIRE_COMMAND ) )
                {
                    break;
                }
            }

            //polling if data ready
            for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

                if ( u8Reg & PCMCIA_STATE_RD_DONE )
                {
                    _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA, &u8pReadBuffer[index] );
                    break;
                }
            }

            // clean stat_rd done
            _MDrv_PCMCIA_WriteReg( REG_PCMCIA_FIRE_READ_DATA_CLEAR, PCMCIA_CLEAN_STATE_RD_DONE );

            // polling if bus is idle
            for ( i = 0; i < PCMCIA_MAX_POLLING_COUNT; i++ )
            {
                _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, (U8 *)&u8Reg );

                if ( PCMCIA_STATE_BUS_IDLE == ( u8Reg & ( PCMCIA_STATE_BUS_IDLE | PCMCIA_STATE_RD_DONE ) ) )
                {
                    break;
                }
            }
        }
#if KERNEL_PCMCIA_PERF
        do_gettimeofday(&tv2);

        if(u16DataLen > 100)
        {
            printf("\n\n[MDrv_SYS_PCMCIA_READ_DATA]: Len(%d), Time(%d.%d - %d.%d), Diff(0.%06d)\n\n",
            u16DataLen, 
            (int)tv1.tv_sec, 
            (int)tv1.tv_usec, 
            (int)tv2.tv_sec, 
            (int)tv2.tv_usec, 
            (int)((tv2.tv_sec*1000000+tv2.tv_usec) - (tv1.tv_sec*1000000+tv1.tv_usec)));
        }         
#endif        
    }

    return u8Value;
}

#define PCMCIA_PHYS_REG_DATA            (0)
#define PCMCIA_PHYS_REG_COMMANDSTATUS   (1)
#define PCMCIA_PHYS_REG_SIZELOW         (2)
#define PCMCIA_PHYS_REG_SIZEHIGH        (3)
#define PCMCIA_STATUS_FREE              (0x40)  //!< PCMCIA Status Register Bit - The module can accept data
#define PCMCIA_COMMAND_HOSTCONTROL      (0x01)  //!< PCMCIA Command register bit - Host control
U32 MDrv_SYS_PCMCIA_WRITE_DATA(U32 arg)
{
    PCMCIA_Map_Info_t op ;
    U16 u16RegVal;
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    U16 u16RegVal2;
#endif

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16Bypass;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16RegVal4;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16RegVal1, u16RegVal3;
#endif
    U8 u8Value;
    U8  u8TryLoop = 0;

    if(copy_from_user( &op, (PCMCIA_Map_Info_t*)arg, sizeof(op) ))
    {
        printk( "copy_from_user error\n" ) ;
        return -EFAULT;
    }
        
    if((!access_ok(VERIFY_WRITE, (void *)op.u8pWriteBuffer, op.u16DataLen)) || 
       (!access_ok(VERIFY_READ, (void *)op.u8pWriteBuffer, op.u16DataLen)))
    {
        return -EFAULT;
    }
    
    // touch memory to prevent page fault inside the mutex lock
    if(copy_from_user((void *)op.u8pWriteBuffer, (void *)op.u8pWriteBuffer, op.u16DataLen))
    {
        return -EFAULT;
    }

    PCMCIA_NAND_SEM(down(&PfModeSem));

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
    _MDrv_PCMCIA_WaitEMMCDone(PCMCIA_LOOP_DELAY_WAIT_EMMC_DONE);
#endif

    // Switch to PCMCIA
    u16RegVal = REG_ADDR( REG_CHIP_NAND_MODE ) & REG_CHIP_NAND_MODE_MASK;
    REG_ADDR( REG_CHIP_NAND_MODE ) &= ~REG_CHIP_NAND_MODE_MASK;

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16Bypass = REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) & REG_CHIP_PCM_NAND_BYPASS_MASK;
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= REG_CHIP_PCM_NAND_BYPASS_ENABLE;
#endif

#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    u16RegVal2 = REG_ADDR(REG_CHIP_PF_MODE) & REG_CHIP_PF_MODE_MASK;
    REG_ADDR(REG_CHIP_PF_MODE) &= ~REG_CHIP_PF_MODE_MASK;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16RegVal3 = REG_ADDR(REG_EMMC_CONFIG) & REG_EMMC_CONFIG_MASK;
    REG_ADDR(REG_EMMC_CONFIG) &= ~REG_EMMC_CONFIG_MASK;
    u16RegVal1 = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
    REG_ADDR(REG_CHIP_PCMCFG) |= REG_CHIP_PCMCFG_CTRL_EN;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16RegVal4 = REG_ADDR(REG_SD_CONFIG2) & REG_SD_CONFIG2_MASK;
    REG_ADDR(REG_SD_CONFIG2) &= ~REG_SD_CONFIG2_MASK;
#endif

    // Up HC
    MDrv_SYS_PCMCIA_WRITE2( PCMCIA_PHYS_REG_COMMANDSTATUS, PCMCIA_COMMAND_HOSTCONTROL, 2, 0, NULL );

    u8Value = MDrv_SYS_PCMCIA_READ2( PCMCIA_PHYS_REG_COMMANDSTATUS, 2, 0, NULL);

    while ( ( !( u8Value & PCMCIA_STATUS_FREE ) ) &&
            ( u8TryLoop < PCMCIA_HW_MAX_RETRY_COUNT ) )
    {
        // Down HC
        MDrv_SYS_PCMCIA_WRITE2( PCMCIA_PHYS_REG_COMMANDSTATUS, 0x00, 2, 0, NULL );
    	udelay(1000) ;
        // Up HC
        MDrv_SYS_PCMCIA_WRITE2( PCMCIA_PHYS_REG_COMMANDSTATUS, PCMCIA_COMMAND_HOSTCONTROL, 2, 0, NULL );
        u8TryLoop++;
    }

    if ( PCMCIA_HW_MAX_RETRY_COUNT == u8TryLoop )
    {
        // Switch to Nand
        REG_ADDR( REG_CHIP_NAND_MODE ) |= u16RegVal; //(BIT0|BIT2);
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
        REG_ADDR( REG_CHIP_PF_MODE ) |= u16RegVal2;
#endif
        PCMCIA_NAND_SEM(up(&PfModeSem));

        printf( "MDrv_PCMCIA_WriteData: not \"free\"\r\n" );

        return -EAGAIN;
    }

    MDrv_SYS_PCMCIA_WRITE2( PCMCIA_PHYS_REG_SIZELOW, (U8)( op.u16DataLen & 0xFF ), 2, 0, NULL );
    MDrv_SYS_PCMCIA_WRITE2( PCMCIA_PHYS_REG_SIZEHIGH, (U8)( op.u16DataLen >> 8 ), 2, 0, NULL );
    MDrv_SYS_PCMCIA_WRITE2( PCMCIA_PHYS_REG_DATA, op.u8pWriteBuffer[0], 3, op.u16DataLen, op.u8pWriteBuffer );

    // Down HC
    MDrv_SYS_PCMCIA_WRITE2( PCMCIA_PHYS_REG_COMMANDSTATUS, 0x00, 2, 0, NULL );

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_EMMC_CONFIG) = (REG_ADDR(REG_EMMC_CONFIG) & ~REG_EMMC_CONFIG_MASK) | u16RegVal3;
    REG_ADDR(REG_CHIP_PCMCFG) = (REG_ADDR(REG_CHIP_PCMCFG) & ~REG_CHIP_PCMCFG_MASK) | u16RegVal1;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_SD_CONFIG2) = (REG_ADDR(REG_SD_CONFIG2) & ~REG_SD_CONFIG2_MASK) | u16RegVal4;
#endif

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) &= ~REG_CHIP_PCM_NAND_BYPASS_MASK;
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= u16Bypass;
#endif

    // Switch to Nand
    REG_ADDR( REG_CHIP_NAND_MODE ) |= u16RegVal; //(BIT0|BIT2);
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    REG_ADDR( REG_CHIP_PF_MODE ) |= u16RegVal2;
#endif

    PCMCIA_NAND_SEM(up(&PfModeSem));
    return 0;
}

U32 MDrv_SYS_PCMCIA_READ_DATA(U32 arg)
{
    PCMCIA_Map_Info_t op ;
    U16 u16RegVal, u16ReadBufferSize;
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    U16 u16RegVal2;
#endif

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16Bypass;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16RegVal4;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    U16 u16RegVal1, u16RegVal3;
#endif

    if(copy_from_user( &op, (PCMCIA_Map_Info_t*)arg, sizeof(op) ))
    {
        printk( "copy_from_user error\n" ) ;
        return -EFAULT;
    }
        
    if((!access_ok(VERIFY_WRITE, (void *)op.u8pReadBuffer, op.u16DataLen)) || 
       (!access_ok(VERIFY_READ, (void *)op.u8pReadBuffer, op.u16DataLen)))
    {
        return -EFAULT;
    }
    
    // touch memory to prevent page fault inside the mutex lock
    if(copy_from_user((void *)op.u8pReadBuffer, (void *)op.u8pReadBuffer, op.u16DataLen))
    {
        return -EFAULT;
    }
    u16ReadBufferSize = op.u16DataLen;

    PCMCIA_NAND_SEM(down(&PfModeSem));

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
    _MDrv_PCMCIA_WaitEMMCDone(PCMCIA_LOOP_DELAY_WAIT_EMMC_DONE);
#endif

    // Switch to PCMCIA
    u16RegVal = REG_ADDR( REG_CHIP_NAND_MODE ) & REG_CHIP_NAND_MODE_MASK;
    REG_ADDR( REG_CHIP_NAND_MODE ) &= ~REG_CHIP_NAND_MODE_MASK;

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16Bypass = REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) & REG_CHIP_PCM_NAND_BYPASS_MASK;
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= REG_CHIP_PCM_NAND_BYPASS_ENABLE;
#endif

#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    u16RegVal2 = REG_ADDR(REG_CHIP_PF_MODE) & REG_CHIP_PF_MODE_MASK;
    REG_ADDR(REG_CHIP_PF_MODE) &= ~REG_CHIP_PF_MODE_MASK;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16RegVal3 = REG_ADDR(REG_EMMC_CONFIG) & REG_EMMC_CONFIG_MASK;
    REG_ADDR(REG_EMMC_CONFIG) &= ~REG_EMMC_CONFIG_MASK;
    u16RegVal1 = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
    REG_ADDR(REG_CHIP_PCMCFG) |= REG_CHIP_PCMCFG_CTRL_EN;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    u16RegVal4 = REG_ADDR(REG_SD_CONFIG2) & REG_SD_CONFIG2_MASK;
    REG_ADDR(REG_SD_CONFIG2) &= ~REG_SD_CONFIG2_MASK;
#endif

    op.u16DataLen = (U16)MDrv_SYS_PCMCIA_READ2( PCMCIA_PHYS_REG_SIZEHIGH, 2, 0, NULL) << 8 |
                 (U16)MDrv_SYS_PCMCIA_READ2( PCMCIA_PHYS_REG_SIZELOW, 2, 0, NULL);

    //if ( ( 0 != u16ReadBufferSize ) & ( op.u16DataLen > u16ReadBufferSize ) )
    if (  op.u16DataLen > u16ReadBufferSize )
        op.u16DataLen = u16ReadBufferSize;

    MDrv_SYS_PCMCIA_READ2( PCMCIA_PHYS_REG_DATA, 3, op.u16DataLen, op.u8pReadBuffer );

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_EMMC_CONFIG) = (REG_ADDR(REG_EMMC_CONFIG) & ~REG_EMMC_CONFIG_MASK) | u16RegVal3;
    REG_ADDR(REG_CHIP_PCMCFG) = (REG_ADDR(REG_CHIP_PCMCFG) & ~REG_CHIP_PCMCFG_MASK) | u16RegVal1;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_SD_CONFIG2) = (REG_ADDR(REG_SD_CONFIG2) & ~REG_SD_CONFIG2_MASK) | u16RegVal4;
#endif

#if defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) &= ~REG_CHIP_PCM_NAND_BYPASS_MASK;
    REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= u16Bypass;
#endif

    // Switch to Nand
    REG_ADDR( REG_CHIP_NAND_MODE ) |= u16RegVal; //(BIT0|BIT2);
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST)
    REG_ADDR( REG_CHIP_PF_MODE ) |= u16RegVal;
#endif

    PCMCIA_NAND_SEM(up(&PfModeSem));
    if(copy_to_user( (void *)arg, &op, sizeof(op) ))
    {
        printk( "copy_to_user error\n" ) ;
        return -EFAULT;        
    }

    return 0;    
}

void MDrv_SYS_GetGFXGOPPipelineDelay(U32 argv)
{
    int gop_index;
    int value=0;

    if(copy_from_user((void*)&gop_index,(void*)argv,sizeof(gop_index)))
        {
            printk( "copy_from_user error\n" ) ;
        }

    //should be chip and GOP dependent
    if(gop_index==0)
    {
        value=0x60;

    }
    else if(gop_index==1)
    {
        value=0x60;
    }


    if(copy_to_user((void*)argv, (void*)&value, sizeof(value)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}

void MDrv_SYS_GetPanelHStart(U32 argv)
{

    int value=gPlatformInfo.panel.u16HStart;
    if(copy_to_user((void*)argv, (void*)&value, sizeof(value)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}


void MDrv_SYS_GetMBoxShareMemory(U32 argv)
{
    U32 au32Data[2];

    MDrv_SYS_GetMMAP(E_SYS_MMAP_MBOX_SHM, &au32Data[0], &au32Data[1]);
    if(copy_to_user((void*)argv, (void*)au32Data, sizeof(U32)*2))
        {
            printk( "copy_to_user error\n" ) ;
        }
}

void MDrv_SYS_PowerDown(int src){
#if 0
    switch(src)
    {
        case INPUT_SRC_DTV:
            MDrv_SYS_PD_ADC_R(FALSE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(FALSE);
            MDrv_SYS_PD_ADC_Y(FALSE);
            MDrv_SYS_PD_GMC_P(FALSE);
            MDrv_SYS_PD_GMC_Y(FALSE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(TRUE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(TRUE);
            MDrv_SYS_PD_MVD_M4V(TRUE);
            MDrv_SYS_PD_TSP(TRUE);
            MDrv_SYS_PD_VE(TRUE);
            MDrv_SYS_PD_RVD(TRUE);
            MDrv_SYS_PD_STRLD(TRUE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_DTV_MLINK:
            break;
        case INPUT_SRC_ATV:
            MDrv_SYS_PD_ADC_R(FALSE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(FALSE);
            MDrv_SYS_PD_ADC_Y(TRUE);
            MDrv_SYS_PD_GMC_P(TRUE);
            MDrv_SYS_PD_GMC_Y(TRUE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(TRUE);
            MDrv_SYS_PD_DAC_CVBS(TRUE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(TRUE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_CVBS_1:
            MDrv_SYS_PD_ADC_R(FALSE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(FALSE);
            MDrv_SYS_PD_ADC_Y(TRUE);
            MDrv_SYS_PD_GMC_P(TRUE);
            MDrv_SYS_PD_GMC_Y(TRUE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(TRUE);
            MDrv_SYS_PD_DAC_CVBS(TRUE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_CVBS_2:
            MDrv_SYS_PD_ADC_R(FALSE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(FALSE);
            MDrv_SYS_PD_ADC_Y(TRUE);
            MDrv_SYS_PD_GMC_P(TRUE);
            MDrv_SYS_PD_GMC_Y(TRUE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(TRUE);
            MDrv_SYS_PD_DAC_CVBS(TRUE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_CVBS_3:
            break;
        case INPUT_SRC_VGA:
            MDrv_SYS_PD_ADC_R(TRUE);
            MDrv_SYS_PD_ADC_G(TRUE);
            MDrv_SYS_PD_ADC_B(TRUE);
            MDrv_SYS_PD_ADC_Y(FALSE);
            MDrv_SYS_PD_GMC_P(FALSE);
            MDrv_SYS_PD_GMC_Y(FALSE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(FALSE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(FALSE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(FALSE);
            MDrv_SYS_PD_GOPG2(FALSE);
            break;
        case INPUT_SRC_SCART_1:
            MDrv_SYS_PD_ADC_R(TRUE);
            MDrv_SYS_PD_ADC_G(TRUE);
            MDrv_SYS_PD_ADC_B(TRUE);
            MDrv_SYS_PD_ADC_Y(TRUE);
            MDrv_SYS_PD_GMC_P(TRUE);
            MDrv_SYS_PD_GMC_Y(TRUE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(TRUE);
            MDrv_SYS_PD_DAC_RGB(TRUE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_SCART_2:
            MDrv_SYS_PD_ADC_R(TRUE);
            MDrv_SYS_PD_ADC_G(TRUE);
            MDrv_SYS_PD_ADC_B(TRUE);
            MDrv_SYS_PD_ADC_Y(TRUE);
            MDrv_SYS_PD_GMC_P(TRUE);
            MDrv_SYS_PD_GMC_Y(TRUE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(TRUE);
            MDrv_SYS_PD_DAC_RGB(TRUE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_YPBPR_1:
            MDrv_SYS_PD_ADC_R(TRUE);
            MDrv_SYS_PD_ADC_G(TRUE);
            MDrv_SYS_PD_ADC_B(TRUE);
            MDrv_SYS_PD_ADC_Y(FALSE);
            MDrv_SYS_PD_GMC_P(FALSE);
            MDrv_SYS_PD_GMC_Y(FALSE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(FALSE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_YPBPR_2:
            MDrv_SYS_PD_ADC_R(TRUE);
            MDrv_SYS_PD_ADC_G(TRUE);
            MDrv_SYS_PD_ADC_B(TRUE);
            MDrv_SYS_PD_ADC_Y(FALSE);
            MDrv_SYS_PD_GMC_P(FALSE);
            MDrv_SYS_PD_GMC_Y(FALSE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(FALSE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_HDMI_A:
            MDrv_SYS_PD_ADC_R(FALSE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(FALSE);
            MDrv_SYS_PD_ADC_Y(FALSE);
            MDrv_SYS_PD_GMC_P(FALSE);
            MDrv_SYS_PD_GMC_Y(FALSE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(FALSE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_HDMI_B:
            MDrv_SYS_PD_ADC_R(FALSE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(FALSE);
            MDrv_SYS_PD_ADC_Y(FALSE);
            MDrv_SYS_PD_GMC_P(FALSE);
            MDrv_SYS_PD_GMC_Y(FALSE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(FALSE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_HDMI_C:
            MDrv_SYS_PD_ADC_R(FALSE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(FALSE);
            MDrv_SYS_PD_ADC_Y(FALSE);
            MDrv_SYS_PD_GMC_P(FALSE);
            MDrv_SYS_PD_GMC_Y(FALSE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(FALSE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_SVIDEO_1:
            MDrv_SYS_PD_ADC_R(TRUE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(TRUE);
            MDrv_SYS_PD_ADC_Y(TRUE);
            MDrv_SYS_PD_GMC_P(TRUE);
            MDrv_SYS_PD_GMC_Y(TRUE);
            MDrv_SYS_PD_GMC_C(TRUE);
            MDrv_SYS_PD_CVBS_Buffer(TRUE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_SVIDEO_2:
            MDrv_SYS_PD_ADC_R(TRUE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(TRUE);
            MDrv_SYS_PD_ADC_Y(TRUE);
            MDrv_SYS_PD_GMC_P(TRUE);
            MDrv_SYS_PD_GMC_Y(TRUE);
            MDrv_SYS_PD_GMC_C(TRUE);
            MDrv_SYS_PD_CVBS_Buffer(TRUE);
            MDrv_SYS_PD_DAC_CVBS(FALSE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(FALSE);
            MDrv_SYS_PD_MVD_M4V(FALSE);
            MDrv_SYS_PD_TSP(FALSE);
            MDrv_SYS_PD_VE(FALSE);
            MDrv_SYS_PD_RVD(FALSE);
            MDrv_SYS_PD_STRLD(FALSE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_STORAGE:
            MDrv_SYS_PD_ADC_R(FALSE);
            MDrv_SYS_PD_ADC_G(FALSE);
            MDrv_SYS_PD_ADC_B(FALSE);
            MDrv_SYS_PD_ADC_Y(FALSE);
            MDrv_SYS_PD_GMC_P(FALSE);
            MDrv_SYS_PD_GMC_Y(FALSE);
            MDrv_SYS_PD_GMC_C(FALSE);
            MDrv_SYS_PD_CVBS_Buffer(FALSE);
            MDrv_SYS_PD_DAC_CVBS(TRUE);
            MDrv_SYS_PD_DAC(FALSE); //for T2 U03
            MDrv_SYS_PD_FB_DAC(FALSE);
            MDrv_SYS_PD_DAC_RGB(FALSE);
            MDrv_SYS_PD_Audio(TRUE);
            MDrv_SYS_PD_LVDS(FALSE);
            MDrv_SYS_PD_VD(TRUE);
            MDrv_SYS_PD_SVD(TRUE);
            MDrv_SYS_PD_MVD_M4V(TRUE);
            MDrv_SYS_PD_TSP(TRUE);
            MDrv_SYS_PD_VE(TRUE);
            MDrv_SYS_PD_RVD(TRUE);
            MDrv_SYS_PD_STRLD(TRUE);
            MDrv_SYS_PD_AEON(TRUE);
            MDrv_SYS_PD_GOPG2(TRUE);
            break;
        case INPUT_SRC_NUM:
            break;
    }
#endif
}

void MDrv_SYS_PrintMsg(U32 arg)
{
    char buffer[256];
    if (copy_from_user(buffer, (char*)arg, 256))
    {
        printk("MDrv_SYS_PrintMsg() - Warning!! There are some bytes that could not be copied!\n");
        return;
    }

    printk("%s", buffer);
}

unsigned int MDrv_SYS_GetDRAMLength(void)
{
    // is should be implemented in another way that should not be decided at copmiling time
    #if defined(CONFIG_MIPS)
       return MIU_DRAM_LEN;
    #elif defined(CONFIG_ARM)
       return MIU_DRAM_LEN;
    #endif
}

void MDrv_SYS_GetMsBinInfo(U32 argv)
{
    unsigned int au32Data[2];

    au32Data[0] = u32MsBinAddr;
    au32Data[1] = u32MsBinSzie;
    if(copy_to_user((void*)argv, (void*)au32Data, sizeof(unsigned int)*2))
        {
            printk( "copy_to_user error\n" ) ;
        }
}


#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)

void Chip_Flush_Memory(void);
void Chip_Read_Memory(void);
#endif
void MDrv_SYS_FlushMemory(void)
{
#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    #if defined(CONFIG_MIPS)
    Chip_Flush_Memory();
    #elif defined(CONFIG_ARM)
    Chip_Flush_Memory();
    #endif

#endif
}

void MDrv_SYS_ReadMemory(void)
{
#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_EIFFEL)
    #if defined(CONFIG_MIPS)
    Chip_Read_Memory();
    #elif defined(CONFIG_ARM)
    Chip_Read_Memory();
    #endif
#endif
}

#if 0
static __init int MDrv_SYS_SetMsBinAddr(char *msBinAddr)
{
//    printk("msBinAddr:%s\n", msBinAddr);
    u32MsBinAddr = simple_strtol(msBinAddr, NULL, 16);
    printk("u32MsBinAddr:%X\n", u32MsBinAddr);

    return 0;
}

static __init int MDrv_SYS_SetMsBinSize(char *msBinSize)
{
//    printk("msBinSize:%s\n", msBinSize);
    u32MsBinSzie = simple_strtol(msBinSize, NULL, 16);
    printk("u32MsBinSzie:%X\n", u32MsBinSzie);

    return 0;
}
#endif

void MDrv_SYS_StopUARTClock(void)
{
#if defined(CONFIG_MSTAR_EUCLID) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_TITANIA3) || \
    defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_EMERALD)
       *((volatile unsigned int*)(0xBF200000+(0xF1F<<2))) |= 0x100 ;
       *((volatile unsigned int*)(0xBF200000+(0xF1B<<2))) &= 0x100 ;
#elif defined(CONFIG_MSTAR_TITANIA2)
       *((volatile unsigned int*)(0xBF800000+(0xF1E<<2))) |= 0x10 ;
#elif defined(CONFIG_MSTAR_AGATE) || \
      defined(CONFIG_MSTAR_AMBER3) || \
      defined(CONFIG_MSTAR_EAGLE) || \
      defined(CONFIG_MSTAR_EDISON) || \
      defined(CONFIG_MSTAR_EINSTEIN) || \
      defined(CONFIG_MSTAR_EIFFEL)
       *((volatile unsigned int*)(0xFD200000+(0xF1F<<2))) |= 0x100 ;
       *((volatile unsigned int*)(0xFD200000+(0xF1B<<2))) &= 0x100 ;
#else
       printk("MDrv_SYS_StopUARTClock not implemented...\n");
#endif
}

void MDrv_SYS_ResumeUARTClock(void)
{
#if defined(CONFIG_MSTAR_EUCLID) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_TITANIA3) || \
    defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_EMERALD)
        *((volatile unsigned int*)(0xBF200000+(0xF1F<<2))) &= ~0x100 ;
#elif defined(CONFIG_MSTAR_TITANIA2)
        *((volatile unsigned int*)(0xBF800000+(0xF1E<<2))) &= ~0x10 ;
#elif defined(CONFIG_MSTAR_AGATE) || \
      defined(CONFIG_MSTAR_AMBER3) || \
      defined(CONFIG_MSTAR_EAGLE) || \
      defined(CONFIG_MSTAR_EDISON) || \
      defined(CONFIG_MSTAR_EINSTEIN) || \
      defined(CONFIG_MSTAR_EIFFEL)
        *((volatile unsigned int*)(0xFD200000+(0xF1F<<2))) &= ~0x100 ;
#else
        printk("MDrv_SYS_ResumeUARTClock not implemented...\n");
#endif
}

void MDrv_SYS_HoldKernel(void)
{

#if defined(CONFIG_MSTAR_EUCLID) || defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10)

	unsigned long flags ;

	local_irq_save(flags);
	local_irq_disable() ;

	(*(volatile unsigned int*)(0xBF200A1E)) &= 0xFF00 ;
	(*(volatile unsigned int*)(0xBF200A1E)) |= 0x005A ;
	while( 1 )
	{

		udelay(1000000) ;

		if( 0xA5==(0xFF&(*(volatile unsigned int*)(0xBF200A1E))) )
		{
			   (*(volatile unsigned int*)(0xBF200A1E)) &= 0xFF00 ;
			   break ;
		}
	}
	local_irq_restore(flags);
	printk("Kernel Resumed...\n");
#else
        printk("MDrv_SYS_HoldKernel not implemented...\n");
#endif

}

//early_param("MS_BIN_START", MDrv_SYS_SetMsBinAddr);
//early_param("MS_BIN_SIZE", MDrv_SYS_SetMsBinSize);

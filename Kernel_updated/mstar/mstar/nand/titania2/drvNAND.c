//*********************************************************************
//
//   MODULE NAME:
//       drvNAND.c
//
//   DESCRIPTION:
//      give a short description about this file
//
//  PUBLIC PROCEDURES:
//       Name                    Title
//       ----------------------- --------------------------------------
//       int xxx_proc           declare in its corresponding header file
//
//   LOCAL PROCEDURES:
//       Name                    Title
//       ----------------------- --------------------------------------
//       get_prnt_cnvs           the local procedure in the file
//
//---------------------------------------------------------------------
//
//********************************************************************
//--------------------------------------------------------------------
//                             GENERAL INCLUDE
//--------------------------------------------------------------------

#include <linux/delay.h>
#include <asm/io.h>

#include "drvNAND.h"
//#include "drvNAND_SLC.h"

#if (NAND_PLATFORM==MCU_32BIT)
#ifndef NON_OS
#include <pkgconf/memalloc.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>             // delays
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/disk.h>
#include <cyg/kernel/kapi.h>
#include <pkgconf/platform_intr.h>
#include "globalvar.h"
#endif
#else
#include "mcu.h"
#include <intrins.h>
#include "drvmiu.h"
#include "drvSys.h"
static code const char drvNAND_dummy_constant = 0;
#endif

#if ENABLE_NAND // make off whole file

//--------------------------------------------------------------------
//  LITERAL CONSTANTS
//--------------------------------------------------------------------
//#define DEBUG 1
#define NAND_DMA_RACING_PATCH 1

#ifdef NAND_DMA_RACING_PATCH
#define NAND_READ_MAGIC 0x74240319
#endif

//--------------------------------------------------------------------
//  TYPES
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  MACROS
//--------------------------------------------------------------------
#define memset      UTL_memset
#define strncmp     UTL_strncmp
//--------------------------------------------------------------------
//  VARIABLES
//--------------------------------------------------------------------

                             //  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
const U8 u8MSTARFSTYPE[256]=  { 0,19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
	                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
	                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
                            	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
	                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
                            	 0, 0, 0, 0, 0, 8, 0, 5, 7, 0, 0, 0, 0, 0, 0, 0, // 5
	                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, // 6
	                             0,18, 0, 6, 0, 8,10, 0, 0,12, 0, 0, 0, 0, 0, 0, // 7
	                             0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, // 8
	                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	                             0,13, 0, 0, 0, 0, 0, 0, 0, 0,15, 0,14, 0, 0, 0, // A
	                             0,13, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0,12, 0, 0, // B
	                             0,13, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, // C
	                             0, 0, 0,16, 0,17, 3, 0, 0, 9,15, 0,14, 0, 0, 0, // D
	                             0, 0, 0, 2, 0, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
	                             0,13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F
                                };

NAND_FLASH_INFO _fsinfo;
//volatile U32 *_pu32CIFIFO = (volatile U32 *)CIFIFO_DATA_ADDR;
//U16 _pu16LUT[LUT_SIZE] __attribute__((aligned(16)));;
//NAND_LUT _pu16LUT;// __attribute__((aligned(32)));

#if (NAND_PLATFORM==MCU_32BIT)
//cyg_interrupt g_NC_intr_data;
//cyg_handle_t  g_NC_intr_handle;
#ifndef NON_OS
cyg_flag_t NCflag;
#else
//U8 Nand_CopyBuffer[4096] __attribute__((aligned (64)));
#endif
volatile U32 *_pu32CIF_C = (volatile U32 *)CIFIFO_CMD_ADDR;
#else
volatile U8 xdata *_pu8CIF_C = (volatile U8 xdata *)CIFIFO_CMD_ADDR;
//volatile U8 xdata Nand_CopyBuffer[512ul*8];// _at_ 0xB000;
#endif
U8 u8RDDBuf[64];
#if (defined(ENABLE_LINUX_MTD) && (ENABLE_LINUX_MTD==1))
U8 u8CIF_C_Index=0;
U16 u16ByteIdxofPage=0;
U8 _stPageBuffer[2048+64] __attribute((aligned(64)));
int gint_IS_ECC_Fail=0;
#endif
//extern struct nand_operations_struct mlc2plane_op;
//extern struct nand_operations_struct slc_op;
//extern U8 gu8Boardtype;
//--------------------------------------------------------------------
//  Local Function
//--------------------------------------------------------------------

static void drvNAND_CHECK_FLASH_TYPE(void);

void drvNAND_READ_ID(void);
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
static U8 drvNAND_GetSectorNumofBlk(void);
#endif
void drvNAND_RW_TEST(void);
// TODO: remove debug code
void drvNAND_DEBUG(void);
void drvNAND_MLCDEBUG(void);
void drvNAND_CIFD_DEBUG(void);
void drvNAND_2KSLCDEBUG(void);

#if (NAND_PLATFORM==MCU_8BIT)
#if 0
void HAL_WRITE_UINT16(U16 u16REG,U16 u16dat)
{
    REG(u16REG) = (U8)u16dat;
    REG(u16REG+1) = (U8)(u16dat>>8);
}
#endif

extern void MDrv_Timer_Delayms(U32 u32DelayTime);
extern void MDrv_MapXdataWindow1(U8 addr_start, U8 addr_end, U32 addr_physical);
#endif

#ifdef ECOS_DISK_DRIVER
static cyg_bool nand_disk_init(struct cyg_devtab_entry *tab);
static DISK_FUNS ( nand_disk_funs,
                        nand_disk_read,
                        nand_disk_write,
                        nand_disk_get_config,
                        nand_disk_set_config);


//NAND_DISK_INSTANCE(0, 0, 0, true, "/dev/csdb/");
static ide_disk_info_t nand_disk_info0 = {
    port: (U8) 0,
    chan: (U8) 0
};

//static DISK_CONTROLLER(cyg_sd_bus_disk_controller_0,NULL);
disk_controller cyg_nand_bus_disk_controller_0 = {
    priv:       NULL,
    init:       false,
    busy:       false
};

DISK_CHANNEL(nand_disk_channel0,
             nand_disk_funs,
             nand_disk_info0,
             cyg_nand_bus_disk_controller_0,
             true
);

BLOCK_DEVTAB_ENTRY(nand_disk_io0,
                   "/dev/csdb/",
                   0,
                   &cyg_io_disk_devio,
                   &nand_disk_init,
                   &nand_disk_lookup,
                   &nand_disk_channel0
);
#endif

//-------------------------------------------------------------------------------------------------
/// Fill memory area with a constant byte
/// @param  pDst    \b IN: destination address ptr
/// @param  u8Ch    \b IN: constant byte value
/// @param  u32Cnt  \b IN: # of bytes
/// @return	destination address ptr
//-------------------------------------------------------------------------------------------------
void *UTL_memset(void *pDst, signed int u8Ch, unsigned int u32Cnt)
{
    unsigned int u32Rem, u32WriteBytes;
    register unsigned int u32Val, u32I;
    register unsigned int u32Dst = (unsigned int)pDst;

    u32Rem = u32Dst % 4;
    if(u32Rem)
    {
        u32WriteBytes = 4 - u32Rem;
        if(u32Cnt < u32WriteBytes)
        {
            u32WriteBytes = u32Cnt;
        }
        for( u32I=0; u32I<u32WriteBytes; u32I++)
        {
            ((volatile unsigned char *)u32Dst)[u32I] = (unsigned char)u8Ch;
        }
        u32Dst += u32WriteBytes;
        u32Cnt -= u32WriteBytes;
    }

    u32Val = ((unsigned int)u8Ch << 24) | ((unsigned int)u8Ch << 16) | ((unsigned int)u8Ch << 8) | (unsigned int)u8Ch;
    for( u32I=0; u32I<u32Cnt/4; u32I++)
    {
        ((volatile unsigned int *)u32Dst)[u32I] = u32Val;
    }

    u32Dst += u32I*4;
    for( u32I=0; u32I<u32Cnt%4; u32I++)
    {
        ((volatile unsigned char *)u32Dst)[u32I] = (unsigned char)u8Ch;
    }

    return pDst;
}

#if 0
int UTL_strncmp( const char *string1, const char *string2, int count )
{
    int iRet, i;

    for(i=0;i<count;i++)
    {
        iRet = string1[i] - string2[i];
        if(iRet)
        {
            return iRet;
        }
    }
    return 0;
}
#endif

void HAL_WRITE_UINT16(U32 u32REG,U16 u16dat)
{
    REG(u32REG) = u16dat;
}

U16 HAL_READ_UINT16_IMPL(U32 addr, U16* regval)
{
	*regval = REG(addr);
	return *regval;
}

#if 1
#define HAL_DCACHE_FLUSH(x,y) dma_cache_inv(x,y)
#else
void HAL_DCACHE_FLUSH( U32 base , U16 asize )
{
#if 1
    register U32 addr  = HAL_DCACHE_START_ADDRESS(base);
    register U32 eaddr = HAL_DCACHE_END_ADDRESS(base, asize);
    //register U16 state;

    _HAL_ASM_SET_MIPS_ISA(3);
    for( ; addr < eaddr; addr += HAL_DCACHE_LINE_SIZE )
    {
        _HAL_ASM_DCACHE_ALL_WAYS(HAL_MIPS_CACHE_HIT_WRITEBACK_INV_D, addr);
    }
    _HAL_ASM_SET_MIPS_ISA(0);
#endif
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_SWITCH_PAD Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
void drvNAND_SWITCH_PAD(U8 u8Type)
{
    U16 u16RegValue = 0;

    u8Type = u8Type;    // fixed compile warning

    HAL_READ_UINT16(CHIPTOP_BASE+(0x6F<<2), u16RegValue);
    u16RegValue |= 0x10;

    HAL_WRITE_UINT16(CHIPTOP_BASE+(0x6F<<2), u16RegValue);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_Release_PAD Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
void drvNAND_Release_PAD(void)
{
#if 0
    REG(reg_chiptop_A) &= ~reg_alt_nd;
    REG(reg_chiptop_8) &= ~reg_nfie_en;
    if(gu8Boardtype==MSPD1111B)
    {
        REG(reg_chiptop_8+1) |= reg_xd_en;
    }
#endif
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_ClearNFIE_EVENT Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
void drvNAND_ClearNFIE_EVENT(void)
{
    U16  u16Cnt=0xFFFF;
    U16 u16Tmp = (R_NC_JOB_END+R_NC_MMA_DATA_END);

    while( u16Tmp & (R_NC_JOB_END+R_NC_MMA_DATA_END))
    {
        HAL_WRITE_UINT16(NC_MIE_EVENT, u16Tmp);
        HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
        if((--u16Cnt)==0)
            break;
    }
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_ISR Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#ifdef ECOS_DISK_DRIVER
U32 drvNAND_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    if (vector != NC_IRQ_NUM)
    {
        return 0;   // This is not our interrupt
    }

    cyg_drv_interrupt_mask(vector);


    cyg_drv_interrupt_acknowledge(vector);

    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);

}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_DSR Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#ifdef ECOS_DISK_DRIVER
void drvNAND_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    U16 temp;

    HAL_READ_UINT16((NC_MIE_EVENT), temp);
    if(temp)
    {
        cyg_flag_setbits(&NCflag,temp);
        HAL_WRITE_UINT16(NC_MIE_EVENT,temp);

        HAL_READ_UINT16(NC_MIE_EVENT, temp);
        while( temp & (R_NC_JOB_END+R_NC_MMA_DATA_END))
        {
            HAL_WRITE_UINT16(NC_MIE_EVENT, temp);
            HAL_READ_UINT16(NC_MIE_EVENT, temp);
        }
    }
    cyg_drv_interrupt_unmask(vector);

}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_InitISR Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#ifdef ECOS_DISK_DRIVER
void drvNAND_InitISR(void)
{
    cyg_flag_init(&NCflag);

    cyg_drv_interrupt_create(NC_IRQ_NUM,
                            99,        // priority
                            0,         // data
                            &drvNAND_ISR,
                            &drvNAND_DSR,
                            &g_NC_intr_handle,
                            &g_NC_intr_data);

    cyg_drv_interrupt_attach(g_NC_intr_handle);

    cyg_drv_interrupt_unmask(NC_IRQ_NUM);
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_SetNC_CLOCK Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
void drvNAND_SetNC_CLOCK(U16 u16Param)
{
    U16 u16Tmp;

    DBG_NAND(printk("drvNAND_SetNC_CLOCK\r\n"));
    DBG_NAND(printk("set u16Param=0x%X\r\n",u16Param));
    HAL_READ_UINT16(reg_clk_nfie, u16Tmp);
    u16Tmp &= ~(NC_CLK_MASK+R_INVERT_CLOCK+R_DISABLE_CLOCK);
    u16Param &= NC_CLK_MASK;
    u16Tmp |= u16Param;
    HAL_WRITE_UINT16(reg_clk_nfie, u16Param);
    DBG_NAND(printk("set reg_clk_nfie=0x%X\r\n",u16Tmp));
}

#define NC_CFG0_RESET_VALUE    0x3
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_SoftReset Procedure
*
*   DESCRIPTION:
*       wait NC_JOB_END event
*   NOTE:
*
*********************************************************************/
void drvNAND_SoftReset(void)
{
    U16 u16Tmp=0;
    U8 u8Tmp;
    //#if (NAND_PLATFORM==MCU_32BIT)
    DBG_NAND(printk("drvNAND_SoftReset\r\n"));
    DBG_NAND(printk("_fsinfo.u16NC_CFG0=0x%X\r\n",_fsinfo.u16NC_CFG0));
    //HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    u8Tmp = 0x00;
    do /*  While Looping_Free: Exit counter added*/
    {
        HAL_WRITE_UINT16(NC_CFG0,0x00);
        HAL_READ_UINT16(NC_CFG0, u16Tmp);
    }while( (u16Tmp != 0x00) && (u8Tmp++<0x80));
    DBG_NAND(printk("1 "));
    HAL_WRITE_UINT16(NC_CFG1,0x00);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,0);
    HAL_WRITE_UINT16(NC_CTRL,0);
    HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END+R_NC_MMA_DATA_END);
    #if defined(CONFIG_TITANIA3_FPGA) || defined(CONFIG_GP2_DEMO1)
    HAL_READ_UINT16(NC_TEST_MODE, u16Tmp);
    HAL_WRITE_UINT16(NC_TEST_MODE,u16Tmp & (~R_NFIE_SOFT_RESETZ));
    #else
    HAL_WRITE_UINT16(NC_TEST_MODE,R_NFIE_SOFT_RESETZ);
    #endif
    #if (NAND_PLATFORM==MCU_32BIT)
    #ifndef NON_OS
    HAL_DELAY_US(1);
    #else
    //hal_delay_us(1);
    udelay(1);
    #endif
    #else
    MDrv_Timer_Delayms(1);
    #endif
    #if defined(CONFIG_TITANIA3_FPGA) || defined(CONFIG_GP2_DEMO1)
    HAL_WRITE_UINT16(NC_TEST_MODE,(u16Tmp | R_NFIE_SOFT_RESETZ));
    #else
    HAL_WRITE_UINT16(NC_TEST_MODE,0x00);
    #endif
    #if (NAND_PLATFORM==MCU_32BIT)
    #ifndef NON_OS
    HAL_DELAY_US(1);
    #else
    //hal_delay_us(1);
    udelay(200);
    #endif
    #else
    MDrv_Timer_Delayms(1);
    #endif
    u8Tmp = 0x00;
    /*  While Looping_Free: Exit counter added*/
    while((u16Tmp != NC_CFG0_RESET_VALUE) && (u8Tmp++<0x80))
    {
       HAL_READ_UINT16(NC_CFG0, u16Tmp);
       DBG_NAND(printk("NC_CFG0=0x%X\r\n",u16Tmp));
    }
    DBG_NAND(printk("2\r\n"));
    u8Tmp = 0x00;
    do /*  While Looping_Free: Exit counter added*/
    {
        HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
        HAL_READ_UINT16(NC_CFG0, u16Tmp);
        DBG_NAND(printk("NC_CFG0=0x%X\r\n",u16Tmp));
    }while((u16Tmp != _fsinfo.u16NC_CFG0)&& (u8Tmp++<0xF0));
    DBG_NAND(printk("NC_CFG0=0x%X\r\n",u16Tmp));
    HAL_WRITE_UINT16(NC_CFG1,0x00);
    HAL_WRITE_UINT16(NC_CTRL,0);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_NC_EN);
#if 0
    if(CHIP_VERSION>=3)
    {
        // TODO: fine tune SSO
        //REG(NC_MISC) |= R_NC_DOL4_DLY;
        //REG(NC_MISC) |= R_NC_DOH4_DLY;
    }
#endif
    DBG_NAND(printk("drvNAND_SoftReset end\r\n"));
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_wNCMMAEND Procedure
*
*   DESCRIPTION:
*       wait NC_MMA_END event
*   NOTE:
*
*********************************************************************/
NCRW_STATUS drvNAND_wNCMMAEND(void)
{
    U32 u32Count=0;
    U16 u16Tmp=0;

    while( !(u16Tmp & R_NC_MMA_DATA_END))
    {
        HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
        if(++u32Count==0x003FFFF)
        {
            DBG_NAND(printk("MMAEND time out\r\n"));
            break;
        }
    }
    HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_MMA_DATA_END); // clear R_NC_JOB_END
    u32Count = 0x00;
    do /*  While Looping_Free: Exit counter added*/
    {   // RIU sync issue, wait R_NC_JOB_END really clear
        HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
    }while((u16Tmp & R_NC_MMA_DATA_END) && (u32Count++<0x80));

    return(NCRW_STS_OK);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_wNCJOBEND Procedure
*
*   DESCRIPTION:
*       wait NC_JOB_END event
*   NOTE:
*
*********************************************************************/
#if (NAND_PLATFORM==MCU_32BIT)
NCRW_STATUS drvNAND_wNCJOBEND(bool wMMA_END)
#else
NCRW_STATUS drvNAND_wNCJOBEND(bool wMMA_END)
#endif
{
    U16 u16Tmp=0;
    U32 u32Cnt=0;

    #if (NAND_PLATFORM==MCU_32BIT)
    #ifndef NON_OS
    if(HAL_READ_UINT16(NC_MIE_INT_EN,u16Tmp)==(R_NC_JOB_ENDE+R_NC_MMA_DATA_ENDE))
    {
        if(wMMA_END)
        {
            if(cyg_flag_timed_wait( &NCflag, R_NC_MMA_DATA_END+R_NC_JOB_END, CYG_FLAG_WAITMODE_AND,cyg_current_time()+4000)==0)
            {
                DBG_NAND(printk("JobEND+MMAEND Timeout\n"));
                drvNAND_SoftReset();
                return(NCRW_STS_TIMEOUT);
            }
            //cyg_flag_wait( &NCflag, R_NC_JOB_END, CYG_FLAG_WAITMODE_AND);
            cyg_flag_maskbits(&NCflag, ~(R_NC_MMA_DATA_END+R_NC_JOB_END));
            }
            else
            {
            if(cyg_flag_timed_wait( &NCflag, R_NC_JOB_END, CYG_FLAG_WAITMODE_AND,cyg_current_time()+4000)==0)
            {
                DBG_NAND(printk("JobEND Timeout\n"));
                drvNAND_SoftReset();
                return(NCRW_STS_TIMEOUT);
            }
            //cyg_flag_wait( &NCflag, R_NC_JOB_END, CYG_FLAG_WAITMODE_AND);
            cyg_flag_maskbits(&NCflag, ~R_NC_JOB_END);
        }
        return(NCRW_STS_OK);
    }
    else
    #endif
    #endif
    {
        while( !(u16Tmp & R_NC_JOB_END))
        {
            HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
            if(++u32Cnt == 0x3FFFFFF)
            {

                DBG_NAND(printk("**********Job End Time out**********\r\n"));
                HAL_READ_UINT16(NC_CFG0, u16Tmp);
                DBG_NAND(printk("NC_CFG0=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_CFG1, u16Tmp);
                DBG_NAND(printk("NC_CFG1=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_STAT, u16Tmp);
                DBG_NAND(printk("NC_STAT=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_CTRL, u16Tmp);
                DBG_NAND(printk("NC_CTRL=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_ST_READ, u16Tmp);
                DBG_NAND(printk("NC_ST_READ=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
                DBG_NAND(printk("NC_MIE_EVENT=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_JOB_BL_CNT, u16Tmp);
                DBG_NAND(printk("NC_JOB_BL_CNT=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_TR_BK_CNT, u16Tmp);
                DBG_NAND(printk("NC_TR_BK_CNT=0x%X\r\n",u16Tmp));

                drvNAND_SoftReset();
                return(NCRW_STS_TIMEOUT);
        }
        }
        //printk("Rcv JobEND\n");
        HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END); // clear R_NC_JOB_END
        u32Cnt = 0;
        HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
        while(u16Tmp & R_NC_JOB_END)
        {   // RIU sync issue, wait R_NC_JOB_END really clear
            HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
            if(++u32Cnt == 0x3FFFFF)
            {
                (printk("Clear EVENT fail\r\n"));
                //while(1);
            }
            HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END);
        }
        if(wMMA_END)
        {
            HAL_READ_UINT16(NC_STAT, u16Tmp);
            if( (u16Tmp & R_NC_ECC_FLAG1) && (!(u16Tmp & R_NC_ECC_FLAG0)) )
            {
                DBG_NAND(printk("NC_STAT=0x%X, SoftReset\r\n",u16Tmp));
                // TODO: remove debug code
                drvNAND_SoftReset();
                return(NCRW_STS_ECC_ERROR);
            }
            else if ((!(u16Tmp & R_NC_ECC_FLAG1)) && (u16Tmp & R_NC_ECC_FLAG0))
            {
                drvNAND_wNCMMAEND();
                return(NCRW_STS_BIT_FLIP);
            }
            else if(u16Tmp & R_NC_ECC_FLAG1)
            {
                drvNAND_wNCMMAEND();
                return(NCRW_STS_OK);
            }
            else
            {
                drvNAND_wNCMMAEND();
            }
        }
        return(NCRW_STS_OK);
    }

}

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_wNCMMA_JOBEND Procedure
*
*   DESCRIPTION:
*       wait NC_MMA_END & NC_JOB_END event(NC_MMA_END comes first)
*   NOTE:
*
*********************************************************************/
NCRW_STATUS drvNAND_wNCMMA_JOBEND(void)
{
	U16 u16Tmp=0;
	U32 u32Cnt=0;

    #if (NAND_PLATFORM==MCU_32BIT)
    #ifndef NON_OS
    if(HAL_READ_UINT16(NC_MIE_INT_EN,u16Tmp)==(R_NC_JOB_ENDE+R_NC_MMA_DATA_ENDE))
    {
        //printk("INT mode\r\n");
        if(cyg_flag_timed_wait( &NCflag, R_NC_JOB_END, CYG_FLAG_WAITMODE_AND,cyg_current_time()+4000)==0)
        {
            DBG_NAND(printk("JobEND Timeout\n"));
            drvNAND_SoftReset();
            return(NCRW_STS_TIMEOUT);
        }
        cyg_flag_maskbits(&NCflag, ~R_NC_JOB_END);
        HAL_READ_UINT16(NC_STAT, u16Tmp);
        //printk("Rcv JobEND\n");
        if(cyg_flag_peek(&NCflag) & R_NC_MMA_DATA_END)
        {
            //printk("Peek MMAEND\n");
            cyg_flag_maskbits(&NCflag, ~R_NC_MMA_DATA_END);
            HAL_READ_UINT16(NC_ST_READ, u16Tmp);
            if((u16Tmp & 0xC1) == 0xC1)
            {
                return(NCRW_STS_PROGRAM_FAIL);
            }
            HAL_READ_UINT16(NC_STAT, u16Tmp);
            if( (u16Tmp & R_NC_ECC_FLAG1) && (!(u16Tmp & R_NC_ECC_FLAG0)) )
            {
                drvNAND_SoftReset();
                return(NCRW_STS_ECC_ERROR);
            }
            return(NCRW_STS_OK);
        }
        else    // program fail
        {
            //printk("Lost MMAEND\n");
            drvNAND_SoftReset();
            return(NCRW_STS_PROGRAM_FAIL);
        }
    }
    else
    #endif
    #endif
    {
        //printk("Polling mode\r\n");
        while( true)
        {
            HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
            if(u16Tmp & R_NC_JOB_END)
            {
                if(u16Tmp & R_NC_MMA_DATA_END)
                {
                    HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END+R_NC_MMA_DATA_END);
                    while( u16Tmp != 0x00)
                    {
                        HAL_WRITE_UINT16(NC_MIE_EVENT, R_NC_JOB_END+R_NC_MMA_DATA_END);
                        HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
                    }
                    HAL_READ_UINT16(NC_ST_READ, u16Tmp);
                    if((u16Tmp & 0xC1) == 0xC1)
                    {
                        //printk("NCRW_STS_PROGRAM STS FAIL:0x%X\r\n",u16Tmp);
                        return(NCRW_STS_PROGRAM_FAIL);
                    }
                    HAL_READ_UINT16(NC_STAT, u16Tmp);
                    if( (u16Tmp & R_NC_ECC_FLAG1) && (!(u16Tmp & R_NC_ECC_FLAG0)) )
                    {
                        //printk("NCRW_STS_PROGRAM ECC fail\r\n");
                        drvNAND_SoftReset();
                        return(NCRW_STS_ECC_ERROR);
                    }
                    //printk("RX MMAEND\r\n");
                    return(NCRW_STS_OK);
                }
                else    // program fail
                {
                    //printk("NCRW_STS_PROGRAM_FAIL\r\n");
                    HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END);
                    HAL_READ_UINT16(NC_ST_READ, u16Tmp);
                    //if((u16Tmp & 0xC1) == 0xC1)
                    //{
                    //    DEBUG_EXCEP();
                    //}
                    drvNAND_SoftReset();
                    return(NCRW_STS_PROGRAM_FAIL);
                }
            }
            if(++u32Cnt == 0x3FFFF)
                {
                /*
                DBG_NAND(printk("Job End Time out\r\n"));
                HAL_READ_UINT16(NC_CFG0, u16Tmp);
                DBG_NAND(printk("NC_CFG0=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_CFG1, u16Tmp);
                DBG_NAND(printk("NC_CFG1=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_STAT, u16Tmp);
                DBG_NAND(printk("NC_STAT=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_CTRL, u16Tmp);
                DBG_NAND(printk("NC_CTRL=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_ST_READ, u16Tmp);
                DBG_NAND(printk("NC_ST_READ=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
                DBG_NAND(printk("NC_MIE_EVENT=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_JOB_BL_CNT, u16Tmp);
                DBG_NAND(printk("NC_JOB_BL_CNT=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_TR_BK_CNT, u16Tmp);
                DBG_NAND(printk("NC_TR_BK_CNT=0x%X\r\n",u16Tmp));
                HAL_READ_UINT16(NC_MIE_PATH_CTL, u16Tmp);
                DBG_NAND(printk("NC_MIE_PATH_CTL=0x%X\r\n",u16Tmp));
                */
                    drvNAND_SoftReset();
                return(NCRW_STS_TIMEOUT);
            }
        }
        //printk("Polling mode END\r\n");
    }

}
#endif
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_ReadFreeQueue Procedure
*
*   DESCRIPTION:
*       input: free block queue index
*       return: free block address
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
U16 drvNAND_GetFreeBlock(void)
{
    U16 u16PBA;

    if(_fsinfo.FreeQueue.Count <= 0 )
    {
        DBG_NAND(printk("drvNAND_GetFreeBlock No Free block\r\n"));
    }
    //u16PBA = _pu16LUT[MAX_LOGI_BLK+_fsinfo.FreeQueue.Head++];
    u16PBA = _pu16LUT.pu16LUT[MAX_LOGI_BLK+_fsinfo.FreeQueue.Head++];
    if( _fsinfo.FreeQueue.Head == MAX_FREE_BLK )
    {
        _fsinfo.FreeQueue.Head = 0;
    }
    _fsinfo.FreeQueue.Count--;

    return(u16PBA);
}
#endif
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_WriteFreeQueue Procedure
*
*   DESCRIPTION:
*       input
*           u16Idx: free block queue index
*           u16PBA: free block address
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
void drvNAND_ReleaseFreeBlock(U16 u16PBA)
{
    if(_fsinfo.FreeQueue.Count < MAX_FREE_BLK)
    {
        //_pu16LUT[MAX_LOGI_BLK+_fsinfo.FreeQueue.Tail++] = u16PBA;
        _pu16LUT.pu16LUT[MAX_LOGI_BLK+_fsinfo.FreeQueue.Tail++] = u16PBA;
        if(_fsinfo.FreeQueue.Tail == MAX_FREE_BLK)
        {
            _fsinfo.FreeQueue.Tail = 0;
        }
        _fsinfo.FreeQueue.Count++;
    }
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_FLASHRESET Procedure
*
*   DESCRIPTION:
*       reset NAND flash
*   NOTE:
*
*********************************************************************/
void drvNAND_FLASHRESET(void)
{
    //DBG_NAND(printk("drvNAND_FLASHRESET\r\n"));
    #if (NAND_PLATFORM==MCU_8BIT)
    //DBG_NAND(printk("REG(NC_CFG0)=0x%bX\r\n",REG(NC_CFG0)));
    //DBG_NAND(printk("REG(NC_CFG01)=0x%bX\r\n",REG(NC_CFG0_1)));
    #else
    DBG_NAND(printk("REG(NC_CFG0)=0x%X\r\n",(U16)REG(NC_CFG0)));
    #endif
    HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END); // clear event first

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_WAIT_READY_BUSY<<8)+OP_CMDFF_RESET);

    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_BREAK);

    HAL_WRITE_UINT16(NC_CTRL,R_NC_JOB_START);

    //DBG_NAND(printk("wait Flash RESET\r\n"));
    if(drvNAND_wNCJOBEND(false)!=NCRW_STS_OK)
        DBG_NAND(printk("Flash RESET FAIL\r\n"));
    //DBG_NAND(printk("Flash RESET end\r\n"));
}


/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_GetCMD_RSP_BUF Procedure
*
*   DESCRIPTION:
*       Get CIF_C byte value
*   NOTE:
*
*********************************************************************/
U8 drvNAND_GetCMD_RSP_BUF(U8 u8addr)
{
    #if (NAND_PLATFORM==MCU_32BIT)
    U16 u16Tmp;

    u16Tmp = _pu32CIF_C[u8addr>>1];
    if(u8addr&0x1)
    {
        return(u16Tmp>>8);
    }
    else
    {
        return(u16Tmp&0xFF);
    }
    #else

    U8 u8Tmp;
    HAL_WRITE_UINT16(NC_CIF_FIFO_CTL, R_CIFC_RD_REQ);
    // write dummy data to expect address, let HW latch the address
    if(u8addr & 1)
    {
        _pu8CIF_C[u8addr-1] = 0;
        _pu8CIF_C[u8addr] = 0;
    }
    else
    {
        _pu8CIF_C[u8addr] = 0;
        _pu8CIF_C[u8addr+1] = 0;
    }

    u8Tmp = _pu8CIF_C[u8addr];
    // clear read RIURAM bit
    HAL_WRITE_UINT16(NC_CIF_FIFO_CTL, 0);
    return(u8Tmp);
    #endif
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_SetCMD_RSP_BUF Procedure
*
*   DESCRIPTION:
*       Set CIF_C byte value
*   NOTE:
*
*********************************************************************/
void drvNAND_SetCMD_RSP_BUF(U8 u8addr, U8 u8value)
{
    #if (NAND_PLATFORM==MCU_32BIT)
    U16 u16Tmp;

    u16Tmp = _pu32CIF_C[u8addr>>1] & 0xFFFF;

    if(u8addr & 1)
    {
        u16Tmp = ((U16)u8value<<8) + (u16Tmp&0xFF);
    }
    else
    {
        u16Tmp = (u16Tmp&0xFF00)+u8value;
    }
    _pu32CIF_C[u8addr>>1] = u16Tmp;
    #else
    U8 u8Tmp;

    //REG(NC_CIF_FIFO_CTL) = (R_CIFC_RD_REQ);
    //REG(NC_CIF_FIFO_CTL_1) = (0x00);
    HAL_WRITE_UINT16(NC_CIF_FIFO_CTL, R_CIFC_RD_REQ);
    if(u8addr & 1)
    {
        _pu8CIF_C[u8addr-1] = 0;
        _pu8CIF_C[u8addr] = 0;

        u8Tmp = _pu8CIF_C[u8addr-1];
        //REG(NC_CIF_FIFO_CTL) = (0x00);
        //REG(NC_CIF_FIFO_CTL_1) = (0x00);
        HAL_WRITE_UINT16(NC_CIF_FIFO_CTL, 0);
        _pu8CIF_C[u8addr-1] = u8Tmp;
        _pu8CIF_C[u8addr] = u8value;
    }
    else
    {
        _pu8CIF_C[u8addr] = 0;
        _pu8CIF_C[u8addr+1] = 0;

        u8Tmp = _pu8CIF_C[u8addr+1];
        //REG(NC_CIF_FIFO_CTL) = (0x00);
        //REG(NC_CIF_FIFO_CTL_1) = (0x00);
        HAL_WRITE_UINT16(NC_CIF_FIFO_CTL, 0);
        _pu8CIF_C[u8addr]= u8value;
        _pu8CIF_C[u8addr+1] = u8Tmp;
    }
    #endif
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_GetSectorNumofBlk Procedure
*
*   DESCRIPTION:
*       Set CIF_D byte value
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
static U8 drvNAND_GetSectorNumofBlk(void)
{
    U8 SectorNum;

    SectorNum = _fsinfo.u8Pageshift;
    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        SectorNum += 2;
    }

    #ifdef MLC_2PLAN
    if ((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE))
    {
        SectorNum++;
    }
    #endif

    return(SectorNum);
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_GetLBAPage Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
U16 drvNAND_GetLBAPage(U32 u32LBA)
{
    switch(_fsinfo.u8SecNumofBlk)
    {
        case 4: return (u32LBA & 0x0F);

        case 5: return (u32LBA & 0x1F);

        case 6: return (u32LBA & 0x3F);

        case 7: return (u32LBA & 0x7F);

        case 8: return (u32LBA & 0xFF);

        case 9: return (u32LBA & 0x1FF);

        case 10:return (u32LBA & 0x3FF);

        case 11:return (u32LBA & 0x7FF);

        default:return(0);
    }
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_GetLBAPage Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
U16 drvNAND_STG3PAGE(U32 u32ROW)
{
    U32 u32Tmp;

    u32Tmp = u32ROW & (_fsinfo.u8PagesPerBlock-1);
    if(u32Tmp)
    {
        u32Tmp = _fsinfo.u8PagesPerBlock - u32Tmp;
    }
    return((U16)u32Tmp);
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_GetZoneBlock Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
U16 drvNAND_GetZoneBlock(U8 *bTmp, U32 u32LBA)
{
    *bTmp = 0;
    /*
    if( _fsinfo.eflashplane == TWO_PLANE )
    {
        u32LBA >>= _fsinfo.u8SecNumofBlk;
    }
    else
    */
    if( _fsinfo.eFlashConfig & FLASH_2KPAGE )
    {
        u32LBA >>=  (_fsinfo.u8Pageshift+ 2);
    }
    else
    {
        u32LBA >>= _fsinfo.u8Pageshift;
    }
    /*
    if( (_fsinfo.eFlashConfig & FLASH_2KPAGE)&&(_fsinfo.eflashtype==TYPE_MLC) )
    {
        u32LBA<<=1;
    }
    */
    while(u32LBA > (MAX_LOGI_BLK - 1))
    {
        u32LBA -= MAX_LOGI_BLK;
        *bTmp += 1;
    }
    /*
    if( (_fsinfo.eFlashConfig & FLASH_2KPAGE)&&(_fsinfo.eflashtype==TYPE_MLC) )
    {
        u32LBA>>=1;
    }
    */
    return( (U16)u32LBA);
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_CHECK_FLASH_TYPE Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
static void drvNAND_CHECK_FLASH_TYPE(void)
{
    if( (_fsinfo.eMaker != SAMSUMG) && (_fsinfo.eMaker != TOSHIBA) &&
        (_fsinfo.eMaker != RENESAS) && (_fsinfo.eMaker != HYNIX)  &&
        (_fsinfo.eMaker != ST) )
    {
        _fsinfo.u8ID = 0;
        _fsinfo.u16BlocksPerCS = 0;
        _fsinfo.u8PagesPerBlock = 0;
        _fsinfo.u8Pageshift = 0;
        _fsinfo.u8AddrWidth = 0;
        _fsinfo.eFlashConfig = FLASH_UNCONFIG;
        return; // unsupported flash maker
    }

    switch(u8MSTARFSTYPE[_fsinfo.u8ID])
    {
    case 0:
        _fsinfo.u8ID = 0;
        _fsinfo.u16BlocksPerCS = 0;
        _fsinfo.u8PagesPerBlock = 0;
        _fsinfo.u8Pageshift = 0;
        _fsinfo.u8AddrWidth = 0;
        _fsinfo.eFlashConfig = 0;
        return;
    case 1:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 2:
        _fsinfo.u16BlocksPerCS = 512;
        _fsinfo.u8PagesPerBlock = 16;
        _fsinfo.u8Pageshift = 4;
        _fsinfo.u32Capacity = NAND_DISK_4M;
        return;
    case 3:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 4:
        _fsinfo.u16BlocksPerCS = 1024;
        _fsinfo.u8PagesPerBlock = 16;
        _fsinfo.u8Pageshift = 4;
        _fsinfo.u32Capacity = NAND_DISK_8M;
        _fsinfo.u8AddrWidth = 3;
        return;
    case 5:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 6:
        _fsinfo.u16BlocksPerCS = 1024;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u32Capacity = NAND_DISK_16M;
        _fsinfo.u8AddrWidth = 3;
        return;
    case 7:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 8:
        _fsinfo.u16BlocksPerCS = 2048;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u32Capacity = NAND_DISK_32M;
        _fsinfo.u8AddrWidth = 3;
        return;
    case 9:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 10:
        _fsinfo.u16BlocksPerCS = 4096;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u32Capacity = NAND_DISK_64M;
        _fsinfo.u8AddrWidth = 4;
        return;
    case 11:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 12:
        _fsinfo.u16BlocksPerCS = 8192;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u32Capacity = NAND_DISK_128M;
        _fsinfo.u8AddrWidth = 4;
        return;
    case 13:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        _fsinfo.u16BlocksPerCS = 1024;
        _fsinfo.u8PagesPerBlock = 64;
        _fsinfo.u8Pageshift = 6;
        _fsinfo.u8AddrWidth = 4;
        _fsinfo.u32Capacity = NAND_DISK_128M;
        return;
    case 14:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        if((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE))
        {
            _fsinfo.u16BlocksPerCS = 2048;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
            _fsinfo.u32Capacity = NAND_DISK_512M;
        }
        else if(_fsinfo.eMaker==HYNIX)
        {
            _fsinfo.u16BlocksPerCS = 4096;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
            _fsinfo.u32Capacity = NAND_DISK_1G;
        }
        else if(_fsinfo.eMaker == SAMSUMG)
        {
        	_fsinfo.u16BlocksPerCS = 8192;
        	_fsinfo.u8PagesPerBlock = 64;
        	_fsinfo.u8Pageshift = 6;
        	_fsinfo.u32Capacity = NAND_DISK_1G;
        }
        else
        {
            _fsinfo.u16BlocksPerCS = 4096;
            _fsinfo.u8PagesPerBlock = 64;
            _fsinfo.u8Pageshift = 6;
            _fsinfo.u32Capacity = NAND_DISK_512M;
        }
        _fsinfo.u8AddrWidth = 5;

        return;
    case 15:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        _fsinfo.u16BlocksPerCS = 2048;
        _fsinfo.u8PagesPerBlock = 64;
        _fsinfo.u8Pageshift = 6;
        _fsinfo.u8AddrWidth = 5;
        _fsinfo.u32Capacity = NAND_DISK_256M;
        return;
    case 16:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        //if((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE))
        if((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eMaker==SAMSUMG))
        {
            _fsinfo.u16BlocksPerCS = 4096;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
        }
        else if((_fsinfo.eMaker==HYNIX) || (_fsinfo.eMaker==ST) )
        {
            _fsinfo.u16BlocksPerCS = 4096;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
        }
        else
        {
            _fsinfo.u16BlocksPerCS = 8192;
            _fsinfo.u8PagesPerBlock = 64;
            _fsinfo.u8Pageshift = 6;
        }
        _fsinfo.u8AddrWidth = 5;
        _fsinfo.u32Capacity = NAND_DISK_1G;
        return;
    case 17:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        if(_fsinfo.eMaker==SAMSUMG)
        {
            _fsinfo.u16BlocksPerCS = 8192;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
        }
        else
        {
            _fsinfo.u16BlocksPerCS = 16384;
            _fsinfo.u8PagesPerBlock = 64;
            _fsinfo.u8Pageshift = 6;
        }
        _fsinfo.u8AddrWidth = 5;
        _fsinfo.u32Capacity = NAND_DISK_2G;
        return;
    case 18:
        _fsinfo.u16BlocksPerCS = 16384;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u8AddrWidth = 4;
        _fsinfo.u32Capacity = NAND_DISK_256M;
        return;
    case 19:
        _fsinfo.eflashtype = TYPE_AGAND;
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        _fsinfo.u16BlocksPerCS = 32768;
        _fsinfo.u8PagesPerBlock = 2;
        _fsinfo.u8Pageshift = 1;
        _fsinfo.u8AddrWidth = 4;
        _fsinfo.u32Capacity = NAND_DISK_128M;
        return;
    default:
        _fsinfo.u16BlocksPerCS = 0;
        _fsinfo.u8PagesPerBlock = 0;
        _fsinfo.u8Pageshift = 0;
        _fsinfo.u8AddrWidth = 0;
        _fsinfo.u32Capacity = 0;
        return;
    }
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_READ_ID Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
void drvNAND_READ_ID(void)
{
    U8   u8Tmp;
    #if (defined(ENABLE_LINUX_MTD) &&(ENABLE_LINUX_MTD==1))
    u8CIF_C_Index = 0;
    #endif
    //U16  u16Tmp;
    //DBG_NAND(printk("Flash Reset\r\n"));
    drvNAND_FLASHRESET();
    //DBG_NAND(printk("Flash Reset End\r\n"));

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00); // Address for Read ID

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(ADR_C2TIS0<<8)+OP_CMD90_READID);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_RANDOM_DATA_IN);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_RAN_CNT);

    HAL_WRITE_UINT16(NC_AUXR_DAT,0x6);  // read 5 bytes
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x0);

    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);

    drvNAND_wNCJOBEND(false);
    // TODO: remove debug code
#if 0
    {
        U8 i;
        DBG_NAND(printk("ID:"));
        for(i=0;i<6;i++)
        {
            DBG_NAND(printk("%X ",drvNAND_GetCMD_RSP_BUF(i)));
        }
        DBG_NAND(printk("\r\n"));
    }
#endif

    //HAL_WRITE_UINT16(NC_CIF_FIFO_CTL,R_CIFC_RD_REQ);

    _fsinfo.eMaker = drvNAND_GetCMD_RSP_BUF(0);
    _fsinfo.u8ID = drvNAND_GetCMD_RSP_BUF(1);
#if 0
    #if (NAND_PLATFORM==MCU_32BIT)
    DBG_NAND(printk("\n Flash Make code = 0x%x, ID = 0x%x\n",_fsinfo.eMaker,_fsinfo.u8ID));
    #else
    DBG_NAND(printk("\n Flash Make code = 0x%bx, ID = 0x%bx\n",_fsinfo.eMaker,_fsinfo.u8ID));
    #endif
#endif

    u8Tmp = drvNAND_GetCMD_RSP_BUF(2);
    if(_fsinfo.eMaker == SAMSUMG)
    {
        switch(u8Tmp & 0x0C)
        {
            case 0x00:  _fsinfo.eflashtype = TYPE_SLC;  break;
            case 0x04:
            case 0x08:
            case 0x0C:  _fsinfo.eflashtype = TYPE_MLC;  break;
        }
        switch(u8Tmp & 0x30)
        {
            case 0x00:  _fsinfo.eflashplane = ONE_PLANE;    break;
            #ifdef MLC_2PLAN
            case 0x10:  _fsinfo.eflashplane = TWO_PLANE;    break;
            case 0x20:  _fsinfo.eflashplane = FOUR_PLANE;   break;
            case 0x30:  _fsinfo.eflashplane = EIGHT_PLANE;  break;
            #endif
        }
    }
    else if((_fsinfo.eMaker == HYNIX) || (_fsinfo.eMaker == ST))
    {
        switch(u8Tmp & 0x0C)
        {
            case 0x00:  _fsinfo.eflashtype = TYPE_SLC;  break;
            case 0x04:  _fsinfo.eflashtype = TYPE_MLC;  break;
        }
        if(u8Tmp & 0x80)
        {
            DBG_NAND(printk("Support Cache program\r\n"));
            _fsinfo.eFlashConfig |= FLASH_CACHEPROGRAM;
        }
        u8Tmp = drvNAND_GetCMD_RSP_BUF(3);
        if(u8Tmp & 0x40)
        {
            _fsinfo.eFlashConfig |= FLASH_16BIT;
        }
    }
    //#if ( !defined(ENABLE_LINUX_MTD) && (ENABLE_LINUX_MTD==0))
    drvNAND_CHECK_FLASH_TYPE();
    //_fsinfo.u8SecNumofBlk = drvNAND_GetSectorNumofBlk();
    //#endif
    #if ENABLE_CERAMAL_NAND_IMAGE
    _fsinfo.u16ReservedBlocks = ((CERAMAL_NAND_IMAGE_SECTOR_SIZE*CERAMAL_NAND_IMAGE_NUMBER)>>_fsinfo.u8SecNumofBlk);
    DBG_NAND(printk("u16ReservedBlocks = 0x%X\r\n",_fsinfo.u16ReservedBlocks));
    _fsinfo.u32Capacity -= (CERAMAL_NAND_IMAGE_SECTOR_SIZE*CERAMAL_NAND_IMAGE_NUMBER);
    #else
    _fsinfo.u16ReservedBlocks = 0;
    #endif

    _fsinfo.u32Capacity -= (JPG_NAND_BUF_SECTOR_SIZE+WALLPAPER_SECTOR_SIZE);

    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
       _fsinfo.u16NC_CFG0 |= R_NC_PSIZE_SEL;
    }
    else
    {
        DBG_NAND(printk("ONE COLUMN mode\r\n"));
       _fsinfo.u16NC_CFG0 |= R_NC_ONECOLADR;
    }

    HAL_WRITE_UINT16(NC_CFG0, _fsinfo.u16NC_CFG0);


    DBG_NAND(printk("_fsinfo.u8SecNumofBlk=0x%X\r\n",_fsinfo.u8SecNumofBlk));
    HAL_WRITE_UINT16(NC_CIF_FIFO_CTL,0);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_BuildRdd Procedure
*
*   DESCRIPTION:
*       build 6 bytes RDD data to CIF_C
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
void drvNAND_BuildRdd(U8 u8BlkStatus,U16 u16LBA,U8 u8Sector)
{
    #if (NAND_PLATFORM==MCU_32BIT)
    _pu32CIF_C[(u8Sector<<3)+0] = (u8BlkStatus<<8)+u8BlkStatus;
    _pu32CIF_C[(u8Sector<<3)+1] = ((u16LBA&0xFF)<<8) + ((u16LBA>>8)&0xFF);
    _pu32CIF_C[(u8Sector<<3)+2] = 0;
    #else
    _pu8CIF_C[(u8Sector*16)+0] = u8BlkStatus;
    _pu8CIF_C[(u8Sector*16)+1] = u8BlkStatus;
    _pu8CIF_C[(u8Sector*16)+2] = (U8)(u16LBA>>8);
    _pu8CIF_C[(u8Sector*16)+3] = (U8)u16LBA;
    _pu8CIF_C[(u8Sector*16)+4] = 0x00;
    _pu8CIF_C[(u8Sector*16)+5] = 0x00;
    #endif
}
#endif

#if ENABLE_CERAMAL_NAND_IMAGE
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_BuildRdd Procedure
*
*   DESCRIPTION:
*       build 6 bytes RDD data to CIF_C
*   NOTE:
*
*********************************************************************/
void drvNAND_BuildImageRdd(U8 u8BlkStatus,U16 u16LBA,U8 u8Sector)
{
    #if (NAND_PLATFORM==MCU_32BIT)
    _pu32CIF_C[(u8Sector<<3)+0] = (u8BlkStatus<<8)+u8BlkStatus;
    _pu32CIF_C[(u8Sector<<3)+1] = ((u16LBA&0xFF)<<8) + ((u16LBA>>8)&0xFF);
    _pu32CIF_C[(u8Sector<<3)+2] = 0;
    #else
    _pu8CIF_C[(u8Sector*16)+0] = u8BlkStatus;
    _pu8CIF_C[(u8Sector*16)+1] = u8BlkStatus;
    _pu8CIF_C[(u8Sector*16)+2] = (U8)(NAND_IMAGE_LBA>>8);
    _pu8CIF_C[(u8Sector*16)+3] = (U8)(NAND_IMAGE_LBA);
    _pu8CIF_C[(u8Sector*16)+4] = (U8)(u16LBA>>8);
    _pu8CIF_C[(u8Sector*16)+5] = (U8)u16LBA;
    #endif
}
#endif



/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_ReadRdd Procedure
*
*   DESCRIPTION:
*       Read the first 6 bytes data from spare area
*   NOTE:
*
*********************************************************************/
void drvNAND_ReadRdd(U32 u32Row, U16 u16Column)
{
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE )
    {
        if(_fsinfo.eFlashConfig & FLASH_16BIT)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, u16Column>>1);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, u16Column);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C5TFS0<<8) + OP_CMD00_READ);
        }
        else if( _fsinfo.u8AddrWidth == 4)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS0<<8) + OP_CMD00_READ);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS0<<8) + OP_CMD00_READ);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_WAIT_READY_BUSY<<8) + OP_CMD30_READ);
        //HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_BREAK<<8) + OP_A_READ_RDD);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_BREAK<<8) + OP_A_RANDOM_DATA_IN);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
        if( _fsinfo.u8AddrWidth == 4)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS0<<8)+OP_CMD50_READ_RDD);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS0<<8)+OP_CMD50_READ_RDD);
        }
        //HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_READ_RDD<<8)+OP_A_WAIT_READY_BUSY);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_RANDOM_DATA_IN<<8)+OP_A_WAIT_READY_BUSY);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_BREAK<<8) + OP_A_BREAK);
    }

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_RAN_CNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x10);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);

    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(false);

    // TODO: remove debug code
    #if 0
    {
        U8 i;

        DBG_NAND(printk("RDD_B(%lX):",u32Row));
        for(i=0;i<16;i++)
        {
            DBG_NAND(printk("%2bX ",drvNAND_GetCMD_RSP_BUF(i)));
        }
        DBG_NAND(printk("\r\n"));
    }
    #endif
}

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==1))
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_ReadPage Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
void drvNAND_ReadPage(U32 u32Row,U16 u16Col)
{
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u16Col);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
    HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

    // if small block set one column mode first
    if( _fsinfo.u8AddrWidth == 5)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C5TFS0<<8) + OP_CMD00_READ);
    }
    else if( _fsinfo.u8AddrWidth == 4)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C4TFS0<<8) + OP_CMD00_READ);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C3TFS0<<8) + OP_CMD00_READ);
    }

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_WAIT_READY_BUSY<<8) + OP_CMD30_READ);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)(OP_A_REPEAT+0)<<8)+OP_A_SERIAL_DATA_IN); // repeat from instruct queue 0
        HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_BREAK);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_SERIAL_DATA_IN<<8) + OP_A_WAIT_READY_BUSY);
        // Derek 2008/01/08 debug
        //HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)(OP_A_BREAK+0)<<8)+OP_A_REPEAT); // repeat from instruct queue 0
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)(OP_A_REPEAT)<<8)+OP_A_WAIT_READY_BUSY); // repeat from instruct queue 0
        HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_BREAK);
    }

}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_WritePage Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
void drvNAND_WritePage(U32 u32Row,U16 u16Col)
{
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u16Col);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
    HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    // if small block set one column mode first
    if( _fsinfo.u8AddrWidth == 5)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C5TFS0<<8) + OP_CMD80_SERIAL_DATA_INPUT);
    }
    else if( _fsinfo.u8AddrWidth == 4)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C4TFS0<<8) + OP_CMD80_SERIAL_DATA_INPUT);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C3TFS0<<8) + OP_CMD80_SERIAL_DATA_INPUT);
    }

}

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_EraseBlock Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
U8 drvNAND_EraseBlock(U32 u32Row)
{
    #if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
    U16 u16LBA;
    drvNAND_ReadRdd(u32Row,0x200);
    u16LBA = ((U16)drvNAND_GetCMD_RSP_BUF(2)<<8) + drvNAND_GetCMD_RSP_BUF(3);
    if( !(_fsinfo.eFlashConfig & FLASH_2KPAGE))
        drvNAND_FLASHRESET();
    if((drvNAND_GetCMD_RSP_BUF(0)==0xFF) && (drvNAND_GetCMD_RSP_BUF(1)==0xFF))
    {
        if(_fsinfo.u8BOOT_MODE==false)
        {
            if((u16LBA==CISBLOCK_LBA)||(u16LBA==NAND_IMAGE_LBA))
            {
               (printk("NAND Driver critical error,Erase CIS or Code area\r\nRDD: "));
               (printk("%2bX ",drvNAND_GetCMD_RSP_BUF(0)));
               (printk("%2bX ",drvNAND_GetCMD_RSP_BUF(1)));
               (printk("%2bX ",drvNAND_GetCMD_RSP_BUF(2)));
               (printk("%2bX ",drvNAND_GetCMD_RSP_BUF(3)));
               (printk("%2bX ",drvNAND_GetCMD_RSP_BUF(4)));
               (printk("%2bX ",drvNAND_GetCMD_RSP_BUF(5)));
               (printk("\r\nROW=0x%lX",u32Row));
               return(NCRW_STS_WRITE_PROTECT);
            }
        }
    }
    else
    {
        printk("Erase BAD Block\r\n");
        return(NCRW_STS_WRITE_PROTECT);
    }
    #endif
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
    HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C3TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C2TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
    }
    else
    {
        if( _fsinfo.u8AddrWidth == 3)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C2TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C3TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_WAIT_READY_BUSY<<8) + OP_CMDD0_BLOCK_ERASE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_CHECK_STATUS<<8) + OP_CMD70_READ_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_BREAK);

    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    return(NCRW_STS_OK);
}
#endif

#if 0
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_DumpLUT Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
void drvNAND_DumpLUT(void)
{
    U8 i,j;

    (printk("\r\nLUT===============================================================================\r\n"));
    for(i=0;i<(1024/16);i++)
    {
        for(j=0;j<16;j++)
        {
            (printk("%4X ",_pu16LUT.pu16LUT[(U16)i*16+j]));
        }
        (printk("\r\n"));
    }
    (printk("===============================================================================\r\n"));
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_BUILDLUT Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
#ifdef NAND_1KLUT
void drvNAND_BUILDLUT(U8 bZone,bool bField)
#else
void drvNAND_BUILDLUT(U8 bZone)
#endif
{
    const U8 CIS_TAG[]= {"MSTARSEMI"};
    bool CIS_FOUND = false;
    U32 u32PBA,u32DMAAddr;
    U16 u16PhyBlkNo, u16LBA;
    U16 u16MAX_LBA=MAX_LOGI_BLK;
    #if (NAND_PLATFORM==MCU_32BIT)
    U8 pu8Buf[512] __attribute__((aligned (64)));
    #else
    U8 xdata *pu8Buf = (0xF000 + (BASEADR_SDRAM_NAND_COPY_BUF&0xFFF));
    U8 u8Win1StarAddr,u8Win1EndAddr;
    U32 u32Win1PhyAdr;

    MDrv_GetXdataWindow1Map(&u8Win1StarAddr, &u8Win1EndAddr, &u32Win1PhyAdr);

    #endif

    DBG_NAND(printk("\n drvNAND_BUILDLUT(%X)\n",bZone));
#ifdef NAND_1KLUT
    if (bZone || bField)
#else
    if (bZone != 0)
#endif
    {
        CIS_FOUND = true;
    }
    else
    {
        u16MAX_LBA -= _fsinfo.u16ReservedBlocks;
        DBG_NAND(printk("MAX_LBA=0x%X\r\n",u16MAX_LBA));
    }

    _fsinfo.FreeQueue.Head = 0x00;
    _fsinfo.FreeQueue.Tail = 0x00;
    _fsinfo.FreeQueue.Count = 0x00;

    memset(_pu16LUT.pu16LUT,0xFF,MAX_PHYS_BLK*2);

    u16PhyBlkNo = _fsinfo.u16BlocksPerCS;
    #ifdef NAND_1KLUT
    if(bField)
    {
        u32PBA = MAX_PHYS_BLK;
        if(u16PhyBlkNo>(MAX_PHYS_BLK*2))
        {
            u16PhyBlkNo = (MAX_PHYS_BLK*2);
        }
    }
    else
    #endif
    {
        u32PBA = 0;
        if(u16PhyBlkNo > MAX_PHYS_BLK)
        {
            u16PhyBlkNo = MAX_PHYS_BLK;
        }
    }
    #if (NAND_PLATFORM==MCU_32BIT)
    u32DMAAddr = ((U32)VA2PA(pu8Buf))/MIUWIDTH;
    HAL_DCACHE_FLUSH((U32)pu8Buf,512);
    #else
    u32DMAAddr = ((U32)BASEADR_SDRAM_NAND_COPY_BUF/MIUWIDTH);
    #endif
    DBG_NAND(printk("\r\n u32DMAAddr = 0x%X\r\n",u32DMAAddr));
    // TODO: remove testing code

    drvNAND_ClearNFIE_EVENT();
    while( u32PBA < u16PhyBlkNo ) /*  While Looping_Free: Impossible to infinite looping*/
    {
        //if ((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE))
            drvNAND_ReadRdd( (((U32)bZone<<10)+u32PBA) << _fsinfo.u8Pageshift,0x200 );

        // Data Status & Block Status
        if(drvNAND_GetCMD_RSP_BUF(0) == 0xFF)
        {   //get logical block address
            u16LBA = ((U16)drvNAND_GetCMD_RSP_BUF(2)<<8) + drvNAND_GetCMD_RSP_BUF(3);
            // Check if this block is CIS/IDI
            if ( CIS_FOUND==false )
            {
                if (u16LBA == CISBLOCK_LBA) // CIS block
                {
                    //HAL_DCACHE_FLUSH(pu8Buf, 512);
                    HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
                    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAAddr>>16);
                    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
                    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0);
                    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
                    // Read a Full page or Redundant area only

                    drvNAND_ReadPage(u32PBA<<_fsinfo.u8Pageshift, 0);

                    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
                    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
                    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // random data count ignored
                    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // radom addres offset must be 0

                    //MDrv_MapXdataWindow1( 60, 64, (U32)(2*1024*1024ul));

                    HAL_WRITE_UINT16(NC_CTRL,R_NC_SER_PART_MODE+R_NC_CIFC_ACCESS+R_NC_JOB_START);
                    drvNAND_wNCJOBEND(true);

                    #if (NAND_PLATFORM==MCU_8BIT)
                    MDrv_MapXdataWindow1( 60, 64, BASEADR_SDRAM_NAND_COPY_BUF);
                    #endif
                    if(strncmp(pu8Buf,CIS_TAG, 9)==0)
                    {
                        _fsinfo.u8CIS_BLOCK = u32PBA;
                        CIS_FOUND = true;
                        printk("find CIS Block\r\n");
                    }
                    else
                    {
                        HAL_READ_UINT16(NC_MIU_DMA1,u16LBA);
                        DBG_NAND(printk("\r\nNC_MIU_DMA1 = 0x%X\r\n",u16LBA));
                        HAL_READ_UINT16(NC_MIU_DMA0,u16LBA);
                        DBG_NAND(printk("NC_MIU_DMA0 = 0x%X\r\n",u16LBA));
                        //MDrv_MapXdataWindow1( 60, 64, (U32)(2*1024*1024ul));
                        for(u16LBA=0;u16LBA<512;u16LBA++)
                        {
                            DBG_NAND(printk("%2X ",pu8Buf[u16LBA]));
                            if((u16LBA&0xF)==0xF)
                                DBG_NAND(printk("\r\n"));
                        }
                    }

                }
                else if(u32PBA > 0x0F) // page address
                {
                    CIS_FOUND = true;// Give up searching, mark as CIS found
                    u32PBA = 0;
                    _fsinfo.u8CIS_BLOCK = 0xFF;
                    continue;
                }
            }    //end if ( !(bFlag & _CIS_FOUND) )
            else
            {
#ifdef NAND_1KLUT
                if (bField)
                    u16LBA -= MAX_LOGI_BLK;
#endif
                #ifdef MLC_2PLAN
                if ((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE))
                {
                    if ( (u16LBA < (MAX_LOGI_BLK/2) ) && (_pu16LUT.pu16LUT[u16LBA]==NO_MAPPING) )
                    {
                        _pu16LUT.pu16LUT[u16LBA] = u32PBA;
                    }
                    else
                    {
                        if( _fsinfo.FreeQueue.Count < MAX_FREE_BLK )
                        {
                            drvNAND_ReleaseFreeBlock(u32PBA);

                            if(_fsinfo.FreeQueue.Tail >= MAX_FREE_BLK)
                            {
                                _fsinfo.FreeQueue.Tail = 0;
                            }
                        }
                    }
                }
                else
                #endif
                {
                    //if ( (u16LBA < MAX_LOGI_BLK ) && (_pu16LUT.pu16LUT[u16LBA]==NO_MAPPING) )
                    if ( (u16LBA < u16MAX_LBA ) && (_pu16LUT.pu16LUT[u16LBA]==NO_MAPPING) )
                    {
                        _pu16LUT.pu16LUT[u16LBA] = u32PBA;
                    }
                    #if ENABLE_CERAMAL_NAND_IMAGE
                    else if( (bZone==0) && (u16LBA == NAND_IMAGE_LBA) )
                    {
                        u16LBA = ((U16)drvNAND_GetCMD_RSP_BUF(4)<<8) + drvNAND_GetCMD_RSP_BUF(5);
                        if(u16LBA<_fsinfo.u16ReservedBlocks)
                            _pu16LUT.pu16LUT[u16LBA+u16MAX_LBA] = u32PBA;
                    }
                    #endif
                    #if ENABLE_USER_SETTING_BLOCK
                    else if( (bZone==0) && (u16LBA == USER_SETTING_LBA) )
                    {
                        _fsinfo.u16UserBlock = (U16)u32PBA;
                    }
                    #endif
                    else if((bZone==0) && (u16LBA == CISBLOCK_LBA))
                    {
                        (printk("CIS2 found\r\n"));
                    }
                    else
                    {
                        if( _fsinfo.FreeQueue.Count < MAX_FREE_BLK )
                        {
                            drvNAND_ReleaseFreeBlock(u32PBA);

                            if(_fsinfo.FreeQueue.Tail >= MAX_FREE_BLK)
                            {
                                _fsinfo.FreeQueue.Tail = 0;
                            }
                        }
                    }
                }
            }
        }// Data corrupt!
        else
        {
            DBG_NAND(printk("\n BAD_BLOCK=0x%X \r\n",u32PBA));
        }
        #ifdef MLC_2PLAN
        if ((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE))
        {
            u32PBA+=2;
        }
        else
        #endif
        {
            u32PBA++;
        }
    } // End of while

    drvNAND_FLASHRESET();// Reset Device
    drvNAND_ClearNFIE_EVENT();
    // TODO: remove debug code
    DBG_NAND(printk("_fsinfo.FreeQueue.Count=%X",_fsinfo.FreeQueue.Count));
    drvNAND_DumpLUT();
    #if (NAND_PLATFORM==MCU_8BIT)
    MDrv_MapXdataWindow1(u8Win1StarAddr, u8Win1EndAddr, u32Win1PhyAdr);
    #endif
}
#endif

//#define CIS_BLOCK_DEBUG
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
#if defined(BOOT_LOADER) || (defined(ENABLE_NAND_APP_CIS_CHECK) && ENABLE_NAND_APP_CIS_CHECK)
void drvNAND_BUILDCIS(BOOL bScanFlg,BOOL bEraseFlg )
{
    const U8 CIS_TAG[]= {"MSTARSEMI"};
    const U8 u8Str[] = {"FACTORY DEFECT!!"};
    const U8 u8DEFECTSIG[] = {"BADBLOCK"};
    bool CIS_FOUND = FALSE;
    U32 u32PBA,u32DMAAddr;
    U16 u16LBA;
    U16	m_nBadBlockCnt=0, m_nBadBlockList[512];
    U8  BadUnit = FALSE;
    U32 u32Row,u32Row2;
    U16 u16Tmp;
    U16 bZone, page_size, page_size_extra, check_pos;
    U32 old_w1_phys;
    U32 u32Idx;
    #if (NAND_PLATFORM==MCU_8BIT)
    U8 xdata *pu8Buf = (0xF000 + (BASEADR_SDRAM_NAND_COPY_BUF&0xFFF));
    U8 old_w1_start, old_w1_end;
    #endif
    U8 u8CIS_BLOCK2=0xFF;
    printk("[drvNAND_BUILDCIS] new flash cis !\n");

    MDrv_GetXdataWindow1Map( &old_w1_start, &old_w1_end, &old_w1_phys );

    if( _fsinfo.u8CIS_BLOCK != 0xFF )
    {
        CIS_FOUND = TRUE;
    }

    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        page_size = 0x800;
        page_size_extra = 0x840;
        check_pos = 2000;
        printk("big block\n");
    }
	else
    {
        page_size = 0x200;
        page_size_extra = 0x210;
        check_pos = 0x205;
        printk("small block\n");
    }

    u32DMAAddr = BASEADR_SDRAM_NAND_COPY_BUF;

    MDrv_MapXdataWindow1( 60, 64, u32DMAAddr);
    memset(m_nBadBlockList,0x00,1024);

    for(bZone=0; bZone<(_fsinfo.u16BlocksPerCS>>10); bZone++)
    {
        u16LBA = 0;
        for(u32PBA=0; u32PBA<MAX_PHYS_BLK; )
        {

    		if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
            {
                if (_fsinfo.eflashtype == TYPE_SLC)
					MDrv_NAND_ReadPHYPage((((U32)bZone<<10)+u32PBA) << _fsinfo.u8Pageshift, u32DMAAddr, page_size_extra);
				else
					MDrv_NAND_ReadPHYPage((((U32)bZone<<10)+u32PBA) << _fsinfo.u8Pageshift +  _fsinfo.u8PagesPerBlock -1, u32DMAAddr, page_size_extra);
    		}
		    else
            {
                MDrv_NAND_ReadPHYPage((((U32)bZone<<10)+u32PBA) << _fsinfo.u8Pageshift, u32DMAAddr, page_size_extra);
		    }

            if (CIS_FOUND ==TRUE)
            {
                if(strncmp((char *)(pu8Buf[page_size] ),(char *)u8Str, sizeof(u8Str)-1)==0)
                {   // bad block
                    m_nBadBlockList[m_nBadBlockCnt] = ((U16)bZone*1024 + u32PBA);
                    m_nBadBlockCnt++;
                    BadUnit = TRUE;
                }
            }
            else
            {
                if( (pu8Buf[0]==0xFF) && (pu8Buf[1]==0xFF) && (pu8Buf[2]==0xFF) &&
                    (pu8Buf[3]==0xFF) && (pu8Buf[4]==0xFF) && (pu8Buf[5]==0xFF) &&
                    (pu8Buf[6]==0xFF) && (pu8Buf[7]==0xFF) && (pu8Buf[8]==0xFF) )
                {   // unused block
                    if( pu8Buf[check_pos] != 0xFF)
                    {   // bad block
                        m_nBadBlockList[m_nBadBlockCnt] = ((U16)bZone*1024 + u32PBA);
                        m_nBadBlockCnt++;
                        BadUnit = TRUE;
                    }
                }
                else
                {
                    // used block
                    if( (pu8Buf[page_size]!=0xFF) || (pu8Buf[page_size+1]!=0xFF))
                    {
                        m_nBadBlockList[m_nBadBlockCnt] = ((U16)bZone*1024 + u32PBA);
                        m_nBadBlockCnt++;
                        BadUnit = TRUE;
                    }
                }
            }
			if (bScanFlg == FALSE)
            {
                if(BadUnit)
                {
                    BadUnit = FALSE;
                    u32Row= ((U32)bZone<<10)+u32PBA;
                    printk("BAD_BLOCK=0x%lx \r\n",u32Row);
                    MDrv_NAND_EraseBlock(u32Row<< _fsinfo.u8Pageshift);
                    memset(pu8Buf,0x00,2048);
                    memset(u8RDDBuf,0x00,64);
                    u32Idx = 0;
                    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
                    {
                        while(u8Str[u32Idx] != '\0') /*  While Looping_Free:Impossible to infinite looping*/
                        {
                            u8RDDBuf[u32Idx] = u8Str[u32Idx];
                            u8RDDBuf[u32Idx+0x10] = u8Str[u32Idx];
                            u8RDDBuf[u32Idx+0x20] = u8Str[u32Idx];
                            u8RDDBuf[u32Idx+0x30] = u8Str[u32Idx];
                            u32Idx++;
                        }
                    }
                    else
                    {
                        memcpy(u8RDDBuf,u8Str,sizeof(u8Str));
                    }
                    for(u16Tmp=0;u16Tmp<_fsinfo.u8PagesPerBlock;u16Tmp++)
                        MDrv_NAND_WritePHYPage((u32Row<< _fsinfo.u8Pageshift)+u16Tmp,u32DMAAddr, page_size_extra, 1);
                }
                else if( _fsinfo.u8CIS_BLOCK == 0xFF)
                {
                    _fsinfo.u8CIS_BLOCK = u32PBA;
                }
                else if(u8CIS_BLOCK2==0xFF)
                {
                    u8CIS_BLOCK2 = u32PBA;
                    printk("CIS2=0x%bX\r\n",u8CIS_BLOCK2);
                }
            }
			if ((bEraseFlg == TRUE)&&(bScanFlg == TRUE))
			{
				if(BadUnit)
				{
					BadUnit = FALSE;
				}
				else
				{
					MDrv_NAND_EraseBlock((((U32)bZone<<10)+u32PBA)<< _fsinfo.u8Pageshift);
				}
			}

        	#ifdef MLC_2PLAN
        	if ((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE))
        	{
            		u32PBA+=2;
        	}
            else
        	#endif
        	{
            		u32PBA++;
        	}
        //printk(".");
        }
    }

    //printk("\n");
    u32Row= (U32)_fsinfo.u8CIS_BLOCK << _fsinfo.u8Pageshift ;
    u32Row2 = (U32)u8CIS_BLOCK2 << _fsinfo.u8Pageshift ;
	if (bScanFlg == FALSE)
	{
        // Write CIS Area
        MDrv_NAND_EraseBlock(u32Row);
        MDrv_NAND_EraseBlock(u32Row2);

        memset(pu8Buf,0x00,2048);
        memset(u8RDDBuf,0x00,64);
        memcpy(pu8Buf,CIS_TAG,sizeof(CIS_TAG));
        u32Idx = 0;
        while(u32Idx <64 )
        {
            u8RDDBuf[ 0+ u32Idx] =0xff;
            u8RDDBuf[ 1+ u32Idx] =0xff;
            u8RDDBuf[ 2+ u32Idx] =0xcc;
            u8RDDBuf[ 3+ u32Idx] =0xcc;
            u8RDDBuf[ 4+ u32Idx] =0x0;
            u8RDDBuf[ 5+ u32Idx] =0x0;
            u32Idx = u32Idx + 0x10;
        }
        MDrv_NAND_WritePHYPage(u32Row,u32DMAAddr, page_size_extra, 1);
        MDrv_NAND_WritePHYPage(u32Row2,u32DMAAddr, page_size_extra, 1);
#ifdef CIS_BLOCK_DEBUG
        MDrv_NAND_ReadPHYPage(u32Row, u32DMAAddr, page_size_extra);
        printk("CIS block: 0x%bx, page size:0x%x\n",_fsinfo.u8CIS_BLOCK, page_size_extra);
        for(i=0;i<0x210;i++)
        {
            if (!(i%16) && i) printk("\n");
                printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
#endif
        // Write Defect Block information
        memset(pu8Buf,0x00,2048);
        memset(u8RDDBuf,0x00,64);
        memcpy(pu8Buf,u8DEFECTSIG,sizeof(u8DEFECTSIG));
        u32Idx =4;
        ((U16*)pu8Buf)[u32Idx] = m_nBadBlockCnt;
        u32Idx++;
        u16Tmp = 0;
        while(m_nBadBlockCnt)
        {
            ((U16*)pu8Buf)[u32Idx] = m_nBadBlockList[u16Tmp];
            u32Idx++;
            m_nBadBlockCnt--;
            u16Tmp++;
        }
        u32Idx = 0;
        while(u32Idx <64 )
        {
            u8RDDBuf[ 0+ u32Idx] =0xff;
            u8RDDBuf[ 1+ u32Idx] =0xff;
            u8RDDBuf[ 2+ u32Idx] =0xcc;
            u8RDDBuf[ 3+ u32Idx] =0xcc;
            u8RDDBuf[ 4+ u32Idx] =0x0;
            u8RDDBuf[ 5+ u32Idx] =0x0;
            u32Idx = u32Idx + 0x10;
        }
        MDrv_NAND_WritePHYPage(u32Row+1,u32DMAAddr, page_size_extra, 1);
        MDrv_NAND_WritePHYPage(u32Row2+1,u32DMAAddr, page_size_extra, 1);
#ifdef CIS_BLOCK_DEBUG
        printk("statistic block:\n");
        MDrv_NAND_ReadPHYPage(u32Row+1, u32DMAAddr, page_size_extra);
        for(i=0;i<page_size_extra;i++)
        {
            if (!(i%16) && i) printk("\n");
            printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
#endif
        //other blocks
        u16Tmp = 2;
        memset(pu8Buf,0x00,2048);
        memset(u8RDDBuf,0x00,64);
        while(u16Tmp < _fsinfo.u8PagesPerBlock)
        {
            u32Idx = 0;
            while(u32Idx <64 )
            {
                u8RDDBuf[ 0+ u32Idx] =0xff;
                u8RDDBuf[ 1+ u32Idx] =0xff;
                u8RDDBuf[ 2+ u32Idx] =0xcc;
                u8RDDBuf[ 3+ u32Idx] =0xcc;
                u8RDDBuf[ 4+ u32Idx] =0x0;
                u8RDDBuf[ 5+ u32Idx] =0x0;
                u32Idx = u32Idx + 0x10;
            }
            memset(pu8Buf,(BYTE)u16Tmp,2048);
            MDrv_NAND_WritePHYPage(u32Row+u16Tmp,u32DMAAddr, page_size_extra, 1);
            MDrv_NAND_WritePHYPage(u32Row2+u16Tmp,u32DMAAddr, page_size_extra, 1);
            u16Tmp++;
        }
#ifdef CIS_BLOCK_DEBUG
        printk("following block:\n");
        MDrv_NAND_ReadPHYPage(u32Row+2, u32DMAAddr, page_size_extra);
        for(i=0;i<page_size_extra;i++)
        {
            if (!(i%16) && i) printk("\n");
            printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
#endif
        if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
        {
            if (_fsinfo.eflashtype == TYPE_SLC)
            {
                MDrv_NAND_ReadPHYPage(u32Row, u32DMAAddr, page_size_extra);
                MDrv_NAND_ReadPHYPage(u32Row2, u32DMAAddr, page_size_extra);
            }
            else
            {
                MDrv_NAND_ReadPHYPage(u32Row +  _fsinfo.u8PagesPerBlock -1, u32DMAAddr, page_size_extra);
                MDrv_NAND_ReadPHYPage(u32Row2 +  _fsinfo.u8PagesPerBlock -1, u32DMAAddr, page_size_extra);
            }
        }
        else
        {
            MDrv_NAND_ReadPHYPage(u32Row, u32DMAAddr, page_size_extra);
            MDrv_NAND_ReadPHYPage(u32Row2, u32DMAAddr, page_size_extra);
        }

        if( (pu8Buf[page_size]!=0xFF) || (pu8Buf[page_size+2]!=0xCC) || (pu8Buf[page_size+3]!=0xCC))
        {
            printk("Format CIS Failed!!\n");
        }
	}
    drvNAND_FLASHRESET();// Reset Device
    //TODO: remove debug code
    DBG_NAND(printk("_fsinfo.FreeQueue.Count=%2bX",_fsinfo.FreeQueue.Count));
    //drvNAND_DumpLUT();

    MDrv_MapXdataWindow1(old_w1_start, old_w1_end, old_w1_phys);

}
#endif  // ifdef  BOOT_LOADER || (defined(ENABLE_NAND_APP_CIS_CHECK) && ENABLE_NAND_APP_CIS_CHECK)
#endif
#ifndef BOOT_LOADER

/*
 * ucHeadNo - heads per cylinder
 * ucSectorNo - sectors per track
 */
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
void drvNAND_lba_to_chs(U32 uLba, U32 nHeadNo, U32 nSectorNo,
						U16 *pusCylinder, U8 *pucHead, U8 *pucSector)
{
	int		nTemp;

	*pusCylinder = (U16)(uLba / (nHeadNo * nSectorNo));
	nTemp = uLba % (nHeadNo * nSectorNo);
	*pucHead = nTemp / nSectorNo;
	*pucSector = (nTemp % nSectorNo) + 1;
}
#endif

///------------------------------------------------------------------------
///Format to FAT16 and FAT32
///@param FATType:  0=> FAT16 , 1=>FAT32
///@return None
///------------------------------------------------------------------------
//#if ENABLE_NAND_FAT_CHECK
#if 0
BOOL drvNAND_FS_CreateMBR(U8 FATType,U32 TotalSecs)
{
    U32	usBpbSecNo=63;
    U32	ucHeadNo=255;
    U32	ucSectorNo=63;
    U32	uPartTotalSecNo=0;
    U32	uLastSecNo=0;
    U16	usCylinder;
    U8	ucHead;
    U8	ucSector;
    BOOL	status = 0;
    U8 xdata *pu8Buf = (0xF000 + (BASEADR_SDRAM_FS_DATA&0xFFF));

    U8 old_w1_start, old_w1_end;
    U32 old_w1_phys,u32DMAAddr;

    u32DMAAddr = BASEADR_SDRAM_FS_DATA;

    MDrv_GetXdataWindow1Map( &old_w1_start, &old_w1_end, &old_w1_phys );
    MDrv_MapXdataWindow1( 60, 64, u32DMAAddr);

	uLastSecNo = TotalSecs - 1;
	uPartTotalSecNo = (uLastSecNo - usBpbSecNo + 1);
	memset(pu8Buf, 0, 512);
	drvNAND_lba_to_chs(usBpbSecNo, ucHeadNo, ucSectorNo, &usCylinder, &ucHead, &ucSector);
	pu8Buf[0x1BE] = 0x80;				/* BootID */
	pu8Buf[0x1BF] = ucHead;
	pu8Buf[0x1C0] = (ucSector & 0x3f) | ((usCylinder >> 2) & 0xC0);
	pu8Buf[0x1C1] = usCylinder & 0xFF;
	drvNAND_lba_to_chs(uPartTotalSecNo-1, ucHeadNo, ucSectorNo, &usCylinder, &ucHead, &ucSector);
	pu8Buf[0x1C3] = ucHead;
	pu8Buf[0x1C4] = (ucSector & 0x3f) | ((usCylinder >> 2) & 0xC0);
	pu8Buf[0x1C5] = usCylinder & 0xFF;
	pu8Buf[0x1C6] = (BYTE)usBpbSecNo;
	pu8Buf[0x1C7] = (BYTE)(usBpbSecNo >> 8);
	pu8Buf[0x1C8] = (BYTE)(usBpbSecNo >> 16);
	pu8Buf[0x1C9] = (BYTE)(usBpbSecNo >> 24);

	pu8Buf[0x1CA] = (BYTE)(uPartTotalSecNo);
	pu8Buf[0x1CB] = (BYTE)(uPartTotalSecNo >> 8);
	pu8Buf[0x1CC] = (BYTE)(uPartTotalSecNo >> 16);
	pu8Buf[0x1CD] = (BYTE)(uPartTotalSecNo >> 24);
	if (FATType==0)
	{
		pu8Buf[0x1c2]=0x06;  //FAT16
	}
	else if (FATType==1)
	{
		pu8Buf[0x1C2] = 0x0B;  //FAT32
	}
	pu8Buf[0x1FE]=0x55;
	pu8Buf[0x1FF]=0xaa;
	status = nand_disk_write(NULL, u32DMAAddr, 1, 0);
	memset(pu8Buf,0,512);
	status = nand_disk_write(NULL, u32DMAAddr, 1, usBpbSecNo);

    MDrv_MapXdataWindow1(old_w1_start, old_w1_end, old_w1_phys);

	return status;
}

///------------------------------------------------------------------------
///Format to FAT16 and FAT32
///@param FATType:  0=> FAT16 , 1=>FAT32
///@return None
///------------------------------------------------------------------------
//#define FAT_BLOCK_DEBUG
BOOL drvNAND_FS_FmtCardToFAT(U8 FATType)
{
    U16	i,j,SecPerFAT=0xf1;
    U32	LBABegin=0, tmp=0;
    U32	totalSec=0, totalClsr=0;
    U8	totalSec4Byte[4], RsvSecN=1, MediaDcp=0xf8;
    BOOL	IsThereMBR=1;
    U16	rt=0;
	BOOL	status = 0;
	U16	pos = 0;
	U32 TotalSecs;
    U8 xdata *pu8Buf = (0xF000 + (BASEADR_SDRAM_FS_DATA&0xFFF));
    U8 old_w1_start, old_w1_end;
    U32 old_w1_phys,u32DMAAddr;

    printk("[NAND_FS_FmtCardToFAT] flash formating ... \n");

    u32DMAAddr = BASEADR_SDRAM_FS_DATA;

    MDrv_GetXdataWindow1Map( &old_w1_start, &old_w1_end, &old_w1_phys );
    MDrv_MapXdataWindow1( 60, 64, u32DMAAddr);

	TotalSecs = _fsinfo.u32Capacity;
	status = drvNAND_FS_CreateMBR(FATType,TotalSecs);
	if ( status != NCRW_STS_OK){
		printk("Create MBR Fail !!\n");
		goto next;
	}

	//status = m_NANDAPI.SendRead10(0,buf,SECTOR_SIZE,Lun);
	status = nand_disk_read(NULL, u32DMAAddr, 1, 0);
	if ( status != NCRW_STS_OK){
		printk("NAND_FS_FmtCardToFAT error 1\n");
		goto next;
	}

    //check if this sector is ok
    if ((pu8Buf[510]==0x55) && (pu8Buf[511]==0xAA))
    {
        //check is there MBR
        if( ((pu8Buf[36]==0x46) && (pu8Buf[37]==0x41) && (pu8Buf[38]==0x54)) || ((pu8Buf[52]==0x46)&&(pu8Buf[53]==0x41)&&(pu8Buf[54]==0x54)) )
        {//'F' , 'A' , 'T'
            IsThereMBR=0;
        }
        else
        {
            IsThereMBR=1;
        }


        if(IsThereMBR)
        {//handle MBR

            //get LBA Begin
            if((pu8Buf[0x1c7]!=0)||(pu8Buf[0x1c8]!=0)||(pu8Buf[0x1c9]!=0))
            {
                LBABegin=pu8Buf[0x1c6] | (pu8Buf[0x1c7]<<8) | (pu8Buf[0x1c8]<<16) | (pu8Buf[0x1c9]<<24);
            }
            else
            {
                LBABegin=pu8Buf[0x1c6];
            }

            //get Total Sector
            totalSec=pu8Buf[0x1ca] | ((U32)pu8Buf[0x1cb]<<8) | ((U32)pu8Buf[0x1cc]<<16) | ((U32)pu8Buf[0x1cd]<<24);
            for(i=0;i<4;i++)
            {
                totalSec4Byte[i]=pu8Buf[458+i];
            }

            if(totalSec<=0x20000)
            {
                //when card capacity<=64M,  force to format FAT16
                FATType=0;
            }
            else if(totalSec>0x400000) //FAT16 limition is 4G
            {
                //when card capacity>2G, force to format FAT32
                FATType=1;
            }

            if(FATType==0)
            {
                pu8Buf[0x1c2] = 0x6;
            }
            else if(FATType==1)
            {
                pu8Buf[0x1c2] = 0xB;
            }

			//status = m_NANDAPI.SendWrite10(0,buf,SECTOR_SIZE,Lun);
			status = nand_disk_write(NULL, u32DMAAddr, 1, 0);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 2\n");
				goto next;
			}
#ifdef FAT_BLOCK_DEBUG
            printk("MBR block (0):\n");
            status = nand_disk_read(NULL, u32DMAAddr, 1, 0);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 2-1\n");
				goto next;
				}
      			for(i=0;i<512;i++){
	  			if (!(i%16) && i) printk("\n");
     				printk("%02bx ",pu8Buf[i] );
 			}
  			printk("\n");
#endif
            //**********read 1st sector (Boot Sector) of partition******************
                     //status = m_NANDAPI.SendRead10(LBABegin,buf,SECTOR_SIZE,Lun);
			status = nand_disk_read(NULL, u32DMAAddr, 1, LBABegin);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 3\n");
				goto next;
			}

        }
        else if(IsThereMBR==0)
        {
            //total sector number
            totalSec = TotalSecs;
            totalSec4Byte[0]=(BYTE)(totalSec&0x000000ff);
            totalSec4Byte[1]=(BYTE)((totalSec&0x0000ff00)>>8);
            totalSec4Byte[2]=(BYTE)((totalSec&0x00ff0000)>>16);
            totalSec4Byte[3]=(BYTE)((totalSec&0xff000000)>>24);
        }

		printk("Create MBR OK !! \n");

        if(totalSec<65536)
        {
            pu8Buf[0x13]=totalSec4Byte[0];
            pu8Buf[0x14]=totalSec4Byte[1];

            for(i=0;i<4;i++)
            {
                pu8Buf[0x20+i]=0;
            }
        }
        else
        {
            pu8Buf[0x13]=0;
            pu8Buf[0x14]=0;

            for(i=0;i<4;i++)
            {
                pu8Buf[0x20+i]=totalSec4Byte[i];
            }
        }

	    pu8Buf[0x00]=0xeb;//*

        if(FATType==0)
        {
            pu8Buf[0x01]=0x3c;//*
        }
        else if(FATType==1)
        {
            pu8Buf[0x01]=0x58;//*
        }

        pu8Buf[0x02]=0x90;//*
        pu8Buf[0x03]=0x4d;//"M"
        pu8Buf[0x04]=0x53;//"S"
        pu8Buf[0x05]=0x44;//"D"
        pu8Buf[0x06]=0x4f;//"O"
        pu8Buf[0x07]=0x53;//"S"
        pu8Buf[0x08]=0x35;//"5"
        pu8Buf[0x09]=0x2E;//"."
        pu8Buf[0x0a]=0x30;//"0"
        pu8Buf[0x0b]=0;   //bytes per sector, sectors per cluster ( 512=0x200)
        pu8Buf[0x0c]=0x2;

        if(FATType==0)
        {
            //FAT16
            tmp=totalSec/8;
            pu8Buf[0x0d]=8;

            if(tmp>524200)
            {
                //in FAT16,total Clusters can't exceed 65525
                pu8Buf[0x0d]=128;
            }
            else if((tmp>262100)&&(tmp<524200))
            {
                pu8Buf[0x0d]=64;
            }
            else if((tmp>131050)&&(tmp<262100))
            {
                pu8Buf[0x0d]=32;
            }

            else if((tmp>65525)&&(tmp<131050))
            {
                pu8Buf[0x0d]=16;
            }

            pu8Buf[0x0e] =1; // reserved sector count
            //SecPerFAT in FAT16 is fixed to 0xff=255; 65525*2byte/512byte~=255 sectors
            SecPerFAT=0xff;
        }
        else if(FATType==1)
        {
            //FAT32
            pu8Buf[0x0d]=8;
            totalClsr=(totalSec/pu8Buf[0x0d]);

            if((totalSec%pu8Buf[0x0d])!=0)
            {
                totalClsr++;
            }

            if(totalClsr<64000)
            {
            // FAT32 can't accept total cluster num < 65527, so I set a criteria.
                if(totalClsr<16500)
                {
                    return FALSE;
                }
                else if((totalClsr<33000) && (totalClsr>=16500))
                {
                    pu8Buf[0x0d]=2;
                }
                else if((totalClsr<64000) && (totalClsr>=33000))
                {
                    pu8Buf[0x0d]=4;
                }

                 totalClsr=(totalSec/pu8Buf[0x0d]);
                if((totalSec%pu8Buf[0x0d])!=0)
                {
                    totalClsr++;
                }
            }
            SecPerFAT=(U16)(totalClsr/128); //SecPerFAT=totalClsr*4/512
            if( (totalClsr%128)!=0)
            {
                SecPerFAT++;
            }
            pu8Buf[0x0e] =0x22; // reserved sector count
        }

        RsvSecN=pu8Buf[0x0e];

        pu8Buf[0x10]=2;
        //Maximum number of root dir entries.
        if(FATType==0)
        {
            pu8Buf[0x11]=0;
            pu8Buf[0x12]=2;
        }
        else if(FATType==1)
        {
            pu8Buf[0x11]=0;
            pu8Buf[0x12]=0;
        }

        pu8Buf[0x15]=0xf8;
        MediaDcp=pu8Buf[0x15];

        pu8Buf[0x18]=0x3f;
        pu8Buf[0x19]=0;
        pu8Buf[0x1a]=0xff;
        pu8Buf[0x1b]=0;
        pu8Buf[0x1c]=0xfd;
        pu8Buf[0x1d]=0;
        pu8Buf[0x1e]=0;
        pu8Buf[0x1f]=0;

        if(FATType==0)
        {
            pu8Buf[0x16]=(BYTE)SecPerFAT;//fix to give 255 sectors per FAT
            pu8Buf[0x17]=0x0;
            pu8Buf[0x24]=0x80; //physical drive number
            pu8Buf[0x25]=0;    //reserved("current head")
            pu8Buf[0x26]=0x29; //signature
            pu8Buf[0x27]=0x53; //'M'
            pu8Buf[0x28]=0x54; //'S'
            pu8Buf[0x29]=0x41; //'T'
            pu8Buf[0x2a]=0x52; //'A'
            pu8Buf[0x2b]=0x53; //'R'
            pu8Buf[0x2c]=0x44; //' '
            pu8Buf[0x2d]=0x20; //' '
            pu8Buf[0x2e]=0x43; //'C'
            pu8Buf[0x2f]=0x41; //'A'
            pu8Buf[0x30]=0x52; //'R'
            pu8Buf[0x31]=0x44; //'D'
            pu8Buf[0x32]=0x20; //' '
            pu8Buf[0x33]=0x20; //' '
            pu8Buf[0x34]=0x20; //' '
            pu8Buf[0x35]=0x20; //' '
            pu8Buf[0x36]=0x46; //'F'
            pu8Buf[0x37]=0x41; //'A'
            pu8Buf[0x38]=0x54; //'T'
            pu8Buf[0x39]=0x31; //'1'
            pu8Buf[0x3a]=0x36; //'6'
            pu8Buf[0x3b]=0x20;
            pu8Buf[0x3c]=0x20;
            pu8Buf[0x3d]=0x20;

            for(i=0x3e;i<510;i++)
                pu8Buf[i]=0;

            pu8Buf[0x1fe]=0x55;
            pu8Buf[0x1ff]=0xAA;
        }
        else if(FATType==1)
        {
            pu8Buf[0x16]=0x0;
            pu8Buf[0x17]=0x0;
            pu8Buf[0x24]=(SecPerFAT&0x000000ff);
            pu8Buf[0x25]=((SecPerFAT&0x0000ff00)>>8);
            pu8Buf[0x26]=((SecPerFAT&0x00ff0000)>>16);
            pu8Buf[0x27]=((SecPerFAT&0xff000000)>>24);
            pu8Buf[0x28]=0;
            pu8Buf[0x29]=0;
            pu8Buf[0x2a]=0;
            pu8Buf[0x2b]=0;
            pu8Buf[0x2c]=2;
            pu8Buf[0x2d]=0;
            pu8Buf[0x2e]=0;
            pu8Buf[0x2f]=0;
            pu8Buf[0x30]=0x1;//sec num of FS Info Sec
            pu8Buf[0x31]=0;
            pu8Buf[0x32]=0x6;//sec num of a copy of this boot sec
            pu8Buf[0x33]=0;

            for(i=0x34;i<0x40;i++)
                pu8Buf[i]=0;

            pu8Buf[0x40]=0;//removable disk
            pu8Buf[0x41]=0;
            pu8Buf[0x42]=0x29;
            pu8Buf[0x43]=0x53; //'M'
            pu8Buf[0x44]=0x54; //'S'
            pu8Buf[0x45]=0x41; //'T'
            pu8Buf[0x46]=0x52; //'A'
            pu8Buf[0x47]=0x53; //'R'
            pu8Buf[0x48]=0x44; //' '
            pu8Buf[0x49]=0x20; //' '
            pu8Buf[0x4a]=0x43; //'C'
            pu8Buf[0x4b]=0x41; //'A'
            pu8Buf[0x4c]=0x52; //'R'
            pu8Buf[0x4d]=0x44; //'D'
            pu8Buf[0x4e]=0x20; //' '
            pu8Buf[0x4f]=0x20; //' '
            pu8Buf[0x50]=0x20; //' '
            pu8Buf[0x51]=0x20; //' '
            pu8Buf[0x52]=0x46; //'F'
            pu8Buf[0x53]=0x41; //'A'
            pu8Buf[0x54]=0x54; //'T'
            pu8Buf[0x55]=0x33; //'3'
            pu8Buf[0x56]=0x32; //'2'
            pu8Buf[0x57]=0x20;
            pu8Buf[0x58]=0x20;
            pu8Buf[0x59]=0x20;

            for(i=0x5a;i<510;i++)
                pu8Buf[i]=0;

            pu8Buf[0x1fe]=0x55;
            pu8Buf[0x1ff]=0xAA;
			//status = m_NANDAPI.SendWrite10(LBABegin+6,pu8Buf,SECTOR_SIZE,Lun);
			status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+6);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 4\n");
				goto next;
				}
        }

		//status = m_NANDAPI.SendWrite10(LBABegin,pu8Buf,SECTOR_SIZE,Lun);
		status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin);
		if ( status != NCRW_STS_OK){
			printk("Create PBR Fail !! \n");
			goto next;
		}
#ifdef FAT_BLOCK_DEBUG
        printk("LBABegin block (0x%lx):\n",LBABegin);
        status = nand_disk_read(NULL, u32DMAAddr, 1, LBABegin);
        for(i=0;i<512;i++){
            if (!(i%16) && i) printk("\n");
            printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
#endif

		printk("Create PBR OK !! \n");
        //================= boot sector and copy of boot sectorfinished=============

        //===============Start to handle FS Information Sector=========
        if(FATType==1)
        {
            pu8Buf[0]=0x52;
            pu8Buf[1]=0x52;
            pu8Buf[2]=0x61;
            pu8Buf[3]=0x41;

            for(i=4;i<0x1E4;i++)
                pu8Buf[i]=0;

            pu8Buf[0x1e4]=0x72;
            pu8Buf[0x1e5]=0x72;
            pu8Buf[0x1e6]=0x41;
            pu8Buf[0x1e7]=0x61;
            pu8Buf[0x1e8]=(BYTE)(totalClsr&0x000000ff);//0xff;//-1 is unknown
            pu8Buf[0x1e9]=(BYTE)((totalClsr&0x0000ff00)>>8);//0xff;
            pu8Buf[0x1ea]=(BYTE)((totalClsr&0x00ff0000)>>16);//0xff;
            pu8Buf[0x1eb]=(BYTE)((totalClsr&0xff000000)>>24);//0xff;
            pu8Buf[0x1ec]=0x3;
            pu8Buf[0x1ed]=0;
            pu8Buf[0x1ee]=0;
            pu8Buf[0x1ef]=0;

            for(i=0x1f0;i<0x1fe;i++)
            pu8Buf[i]=0;

            pu8Buf[0x1fe]=0x55;
            pu8Buf[0x1ff]=0xAA;

			//status = m_NANDAPI.SendWrite10(LBABegin+1,pu8Buf,SECTOR_SIZE,Lun);
			status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+1);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 5\n");
				goto next;
			}

            pu8Buf[0x1e8]=0xff;//-1 is unknown
            pu8Buf[0x1e9]=0xff;
            pu8Buf[0x1ea]=0xff;
            pu8Buf[0x1eb]=0xff;
            pu8Buf[0x1ec]=0x2;

			//status = m_NANDAPI.SendWrite10(LBABegin+7,pu8Buf,SECTOR_SIZE,Lun);
			status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+7);
			if (status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 6\n");
				return status;
				}

			for(i=0;i<0x1fe;i++)
                pu8Buf[i]=0;

            pu8Buf[0x1fe]=0x55;
            pu8Buf[0x1ff]=0xAA;

			//status = m_NANDAPI.SendWrite10(LBABegin+2,pu8Buf,SECTOR_SIZE,Lun);
			status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+2);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 7\n");
				goto next;
			}
			//status = m_NANDAPI.SendWrite10(LBABegin+8,pu8Buf,SECTOR_SIZE,Lun);
			status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+8);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 8\n");
				goto next;
			}

        }

		printk("Create File Infomation OK !! \n");
        //=============FS Information Sector finished===============

        //=============start to handle FAT1 &FAT2=================
        //read FAT 1 and write data back to FAT1 & FAT2
		//status = m_NANDAPI.SendRead10(LBABegin+RsvSecN,pu8Buf,SECTOR_SIZE,Lun);
		status = nand_disk_read(NULL, u32DMAAddr, 1, LBABegin+RsvSecN);
		if ( status != NCRW_STS_OK){
			printk("NAND_FS_FmtCardToFAT error 9\n");
			goto next;
		}

        if(FATType==0)
        {
            pu8Buf[0]=MediaDcp;
            pu8Buf[1]=0xff;
            pu8Buf[2]=0xff;
            pu8Buf[3]=0xff;

            for(i=4;i<512;i++)
            {
                if((pu8Buf[i]==0xf7)&&(pu8Buf[i+1]==0xff))
                {//bad sector
                    i=i+1;  //pu8Buf[i]==0xf7 ; pu8Buf[i+1]==0xff
                }
                else
                {
                    pu8Buf[i]=0;
                }
            }
        }
        else if(FATType==1)
        {
            pu8Buf[0]=MediaDcp;
            pu8Buf[1]=0xff;
            pu8Buf[2]=0xff;
            pu8Buf[3]=0x0f;
            pu8Buf[4]=0xff;//end-of-file marker
            pu8Buf[5]=0xff;//end-of-file marker
            pu8Buf[6]=0xff;//end-of-file marker
            pu8Buf[7]=0xff;//end-of-file marker
            pu8Buf[8]=0xff;
            pu8Buf[9]=0xff;
            pu8Buf[0xa]=0xff;
            pu8Buf[0xb]=0x0f;

            for(i=0xc;i<512;i++)
            {
                if((pu8Buf[i]==0xf7)&&(pu8Buf[i+1]==0xff)&&(pu8Buf[i+2]==0xff)&&(pu8Buf[i+3]==0x0f))
                {//bad sector
                    i=i+3;  //pu8Buf[i]==0xf7 ; pu8Buf[i+1]==0xff, pu8Buf[i+2]==0xff, pu8Buf[i+3]==0x0f
                }
                else
                {
                    pu8Buf[i]=0;
                }
            }
        }

 		//status = m_NANDAPI.SendWrite10(LBABegin+RsvSecN,pu8Buf,SECTOR_SIZE,Lun);
 		status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+RsvSecN);
		if ( status != NCRW_STS_OK){
			printk("NAND_FS_FmtCardToFAT error 10\n");
			goto next;
		}

 		//status = m_NANDAPI.SendWrite10(LBABegin+RsvSecN+SecPerFAT,pu8Buf,SECTOR_SIZE,Lun);
 		status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+SecPerFAT);
		if ( status != NCRW_STS_OK){
			printk("NAND_FS_FmtCardToFAT error 11\n");
			goto next;
		}
#ifdef FAT_BLOCK_DEBUG
        printk("LBABegin+RsvSecN block (0x%lx):\n",LBABegin+RsvSecN);
        status = nand_disk_read(NULL, u32DMAAddr, 1, LBABegin+RsvSecN);
        for(i=0;i<512;i++){
            if (!(i%16) && i) printk("\n");
                printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
        printk(" LBABegin(0x%lx) +RsvSecN(0x%bx)+SecPerFAT(0x%x) block (0x%lx):\n", LBABegin,RsvSecN,SecPerFAT, LBABegin+RsvSecN+SecPerFAT);
        status = nand_disk_read(NULL, u32DMAAddr, 1,  LBABegin+RsvSecN+SecPerFAT);
        for(i=0;i<512;i++){
            if (!(i%16) && i) printk("\n");
                printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
#endif
        //Clear  2nd ~end sectors of FAT
        for(i=0;i<0xc;i++)
            pu8Buf[i]=0;

		printk("Create FAT Now !! \n");
		j = 0;
        for(i=1;i<SecPerFAT;i++)
        {

 			//status = m_NANDAPI.SendWrite10(LBABegin+RsvSecN+i,pu8Buf,SECTOR_SIZE,Lun);
 		       status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+i);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 12\n");
				goto next;
			}

 			//status = m_NANDAPI.SendWrite10(LBABegin+RsvSecN+SecPerFAT+i,pu8Buf,SECTOR_SIZE,Lun);
 		       status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+SecPerFAT+i);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 13\n");
				goto next;
			}

        }
#ifdef FAT_BLOCK_DEBUG
        printk("LBABegin+RsvSecN+1 block (0x%lx):\n",LBABegin+RsvSecN+1);
        status = nand_disk_read(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+1);
        for(i=0;i<512;i++){
            if (!(i%16) && i) printk("\n");
                printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
        printk(" LBABegin+RsvSecN+SecPerFAT+1 block (0x%lx):\n", LBABegin+RsvSecN+SecPerFAT+1);
        status = nand_disk_read(NULL, u32DMAAddr, 1,  LBABegin+RsvSecN+SecPerFAT+1);
        for(i=0;i<512;i++){
            if (!(i%16) && i) printk("\n");
                printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
#endif
        //============= FAT1 &FAT2 finished=================
        printk("Create FAT OK !! \n");
        //===========start to handle Root Directory=============
        printk("Create Root Directory Area Now!!\n");
        //status = m_NANDAPI.SendRead10(LBABegin+RsvSecN+SecPerFAT*2,pu8Buf,SECTOR_SIZE,Lun);
        status = nand_disk_read(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+SecPerFAT*2);
        if ( status != NCRW_STS_OK){
            printk("NAND_FS_FmtCardToFAT error 14\n");
            goto next;
        }
        for(i=0x0;i<0x200;i++)
            pu8Buf[i]=0;

        //status = m_NANDAPI.SendWrite10(LBABegin+RsvSecN+SecPerFAT*2,pu8Buf,SECTOR_SIZE,Lun);
        status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+SecPerFAT*2);
        if ( status != NCRW_STS_OK){
            printk("NAND_FS_FmtCardToFAT error 15\n");
            goto next;
        }

        for(i=0;i<0x0c;i++)
            pu8Buf[i]=0;

        for(i=1;i<=32;i++)
        {
			//status = m_NANDAPI.SendWrite10(LBABegin+RsvSecN+SecPerFAT*2+i,pu8Buf,SECTOR_SIZE,Lun);
			status = nand_disk_write(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+SecPerFAT*2+i);
			if ( status != NCRW_STS_OK){
				printk("NAND_FS_FmtCardToFAT error 16\n");
				goto next;
			}
        }
#ifdef FAT_BLOCK_DEBUG
        printk("LBABegin+RsvSecN+SecPerFAT*2 block (0x%lx):\n",LBABegin+RsvSecN+SecPerFAT*2);
        status = nand_disk_read(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+SecPerFAT*2);
        for(i=0;i<512;i++){
            if (!(i%16) && i) printk("\n");
                printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");
        printk("LBABegin+RsvSecN+1 block (0x%lx):\n",LBABegin+RsvSecN+SecPerFAT*2+1);
        status = nand_disk_read(NULL, u32DMAAddr, 1, LBABegin+RsvSecN+SecPerFAT*2+1);
        for(i=0;i<512;i++){
            if (!(i%16) && i) printk("\n");
                printk("%02bx ",pu8Buf[i] );
        }
        printk("\n");

#endif
		printk("Create Root Directory Area OK !!\n");
    }
    else
    {
        printk("NAND_FS_FmtCardToFAT error 18\n");
        goto next;
    }

    #if (NAND_PLATFORM==MCU_8BIT)
    MDrv_MapXdataWindow1(old_w1_start, old_w1_end, old_w1_phys);
    #endif

next:

	return status;
}
#endif
#endif// ifdef  BOOT_LOADER

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_2KPAGESCOPY Procedure
*
*   DESCRIPTION:
*       big block page copy function
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
NCRW_STATUS drvNAND_2KPAGESCOPY(U32 u32Source,U32 u32Dest,U16 u16Block,U16 u16PageNum,U8 *pBuf)
{
    NCRW_STATUS eStatus = NCRW_STS_OK;
    U16 u16PageCnt,u16Tmp=0;
    #if (NAND_PLATFORM==MCU_32BIT)
    U32 u32DMAADDR = ((U32)VA2PA(pBuf))/MIUWIDTH;
    #else
    U32 u32DMAADDR = ((U32)BASEADR_SDRAM_NAND_COPY_BUF/MIUWIDTH);
    pBuf = pBuf;
    #endif

    DBG_NAND(printk("drvNAND_2KPAGESCOPY\r\n"));
    DBG_NAND(printk("Src:0x%lX,Fre:0x%lX,LBA:0x%X,Len:0x%X\r\n",u32Source,u32Dest,u16Block,u16PageNum));
    //HAL_READ_UINT16(NC_CFG0,u16Tmp);
    //HAL_WRITE_UINT16(NC_CFG0,u16Tmp | R_NC_REDU_BYPASS);
    _fsinfo.u16NC_CFG0 |= R_NC_REDU_BYPASS;
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0 );
    drvNAND_BuildRdd(0xFF, u16Block,0);
    drvNAND_BuildRdd(0xFF, u16Block,1);
    drvNAND_BuildRdd(0xFF, u16Block,2);
    drvNAND_BuildRdd(0xFF, u16Block,3);

    //drvNAND_BuildRdd(0xFF, u16Block,4);
    while(u16PageNum)
    {

        u16PageCnt = u16PageNum;    // physical 2k Page number
        // TODO: DMA debug

        if(u16PageCnt > 0x200)
        {
            u16PageCnt = 0x200;
        }

        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x04);
        HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAADDR>>16);
        HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAADDR&0xFFFF);
        HAL_WRITE_UINT16(NC_MMA_PRI_REG,0); //read
        HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
        HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT, u32Source);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Source>>16)&0xFF);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest<<8)&0xFF00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest>>8)&0xFFFF);

        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_PAGECOPY_US<<8)+OP_A_WAIT_IDLE);
        if(_fsinfo.u8AddrWidth==5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C5TFS0<<8)+OP_CMD00_READ);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C4TFS0<<8)+OP_CMD00_READ);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_READY_BUSY<<8)+OP_CMD30_READ);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_IDLE<<8)+OP_A_SERIAL_DATA_IN);

        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_IDLE<<8)+OP_A_WAIT_MMA);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_CMD80_SERIAL_DATA_INPUT<<8)+OP_A_PAGECOPY_DS);
        if(_fsinfo.u8AddrWidth==5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_SERIAL_DATA_OUT<<8)+OP_ADR_C5TFS1);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_SERIAL_DATA_OUT<<8)+OP_ADR_C4TFS1);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_READY_BUSY<<8)+OP_CMD10_TRUE_PROGRAM);

        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_CHECK_STATUS<<8)+OP_CMD70_READ_STATUS);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_MMA<<8)+OP_A_WAIT_IDLE);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8)+OP_A_REPEAT);
        HAL_WRITE_UINT16(NC_ADDEND,0x00);   // Set Address increment
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,u16PageCnt-1);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_WAIT_IDLE_CNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x03);
        HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
        u16PageNum -= u16PageCnt;
        u32Source+=u16PageCnt;
        u32Dest+=u16PageCnt;
        // debug for program fail
        eStatus = drvNAND_wNCJOBEND(true);
        if(eStatus == NCRW_STS_ECC_ERROR)
        {
            DBG_NAND(printk("ECC FAIL, Physical Block=0x%lX\r\n",(u32Source-u16PageCnt)>>_fsinfo.u8Pageshift));
        }
        //eStatus = drvNAND_wNCMMA_JOBEND();
        u16PageCnt = 0;
        do /*  While Looping_Free: Exit counter added */
        {
            HAL_WRITE_UINT16(NC_MIE_EVENT, R_NC_JOB_END+R_NC_MMA_DATA_END);
            HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
            if(++u16PageCnt>0x10)
                break;
        }while( u16Tmp &(R_NC_JOB_END+R_NC_MMA_DATA_END) );
        // TODO: remove debug code
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_READ_UINT16(NC_AUXR_DAT,u16PageCnt);
        if(u16PageCnt != 0x00)
            DBG_NAND(printk("u16PageCnt = 0x%X\r\n",u16PageCnt));

        /*
        if(eStatus != NCRW_STS_OK)
        {
            HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
            drvNAND_EraseBlock(u32Dest);
            drvNAND_wNCJOBEND(false);
            //drvNAND_ReleaseFreeBlock(u32Dest>>_fsinfo.u8Pageshift);
            drvNAND_SLC_MARKBADBLOCK(u32Dest>>_fsinfo.u8Pageshift);
            return(eStatus);
        }
        */
    }
    //DBG_NAND(printk("end of drvNAND_2KPAGESCOPY\r\n"));
    _fsinfo.u16NC_CFG0 &= ~R_NC_REDU_BYPASS;
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    return(eStatus);

}
#endif


//#ifndef HW_COPY
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
#if 1
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_2KPAGECOPY Procedure
*
*   DESCRIPTION:
*       big block page copy function
*   NOTE:
*
*********************************************************************/
NCRW_STATUS drvNAND_2KPAGECOPY(U32 u32Source,U32 u32Dest,U16 u16Block,U8 *pBuf)
{
    #if (NAND_PLATFORM==MCU_32BIT)
    U32 u32DMAADDR = ((U32)VA2PA(pBuf))/MIUWIDTH;
    #else
    U32 u32DMAADDR = ((U32)BASEADR_SDRAM_NAND_COPY_BUF/MIUWIDTH);
    UNUSED(pBuf);
    #endif
    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAADDR>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAADDR&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0); //read
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,u32Source);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Source>>16)&0xFF);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest<<8)&0xFF00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest>>8)&0xFFFF);

    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_INSTQUE);
    if(_fsinfo.u8AddrWidth==5)
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C5TFS0<<8)+OP_CMD00_READ);
    else
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C4TFS0<<8)+OP_CMD00_READ);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_READY_BUSY<<8)+OP_CMD30_READ);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8)+OP_A_SERIAL_DATA_IN);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(true);
    drvNAND_ClearNFIE_EVENT();

    drvNAND_BuildRdd(0xFF, u16Block,0);
    drvNAND_BuildRdd(0xFF, u16Block,1);
    drvNAND_BuildRdd(0xFF, u16Block,2);
    drvNAND_BuildRdd(0xFF, u16Block,3);

    HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAADDR>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAADDR&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR); // write
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_INSTQUE);
    if(_fsinfo.u8AddrWidth==5)
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C5TFS1<<8)+OP_CMD80_SERIAL_DATA_INPUT);
    else
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C4TFS1<<8)+OP_CMD80_SERIAL_DATA_INPUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);

    return(drvNAND_wNCJOBEND(true));

}
#endif
#endif
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
#ifndef HW_COPY_SLC_SMALL
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_512PAGECOPY Procedure
*
*   DESCRIPTION:
*       small page copy function
*   NOTE:
*
*********************************************************************/
NCRW_STATUS drvNAND_512PAGECOPY(U32 u32Source,U32 u32Dest,U16 u16Block,U8 *pBuf)
{
    U32 u32DMAADDR = ((U32)VA2PA(pBuf))/MIUWIDTH;

    HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAADDR>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAADDR&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0); //read
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,u32Source);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Source>>16)&0xFF);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest<<8)&0xFF00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest>>8)&0xFFFF);

    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_INSTQUE);
    if( _fsinfo.u8AddrWidth == 5)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C5TFS0<<8)+OP_CMD00_READ);
    }
    else if( _fsinfo.u8AddrWidth == 4)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS0<<8)+OP_CMD00_READ);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS0<<8)+OP_CMD00_READ);
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_WAIT_READY_BUSY<<8)+OP_CMD30_READ);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_SERIAL_DATA_IN);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(true);

    drvNAND_BuildRdd(0xFF, u16Block,0);

    HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAADDR>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAADDR&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR); // write
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_INSTQUE);

    if( _fsinfo.u8AddrWidth == 5)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C5TFS1<<8)+OP_CMD80_SERIAL_DATA_INPUT);
    }
    else if( _fsinfo.u8AddrWidth == 4)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS1<<8)+OP_CMD80_SERIAL_DATA_INPUT);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS1<<8)+OP_CMD80_SERIAL_DATA_INPUT);
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    return(drvNAND_wNCJOBEND(true));

}
#endif
#endif

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
// TODO: DMA debug remove later
#if 1
NCRW_STATUS drvNAND_512PAGESCOPY(U32 u32Source,U32 u32Dest,U16 u16Block,U8 *pBuf,U16 u16PageCnt)
{
    U16 u16ProcessPage;
    NCRW_STATUS eStatus = NCRW_STS_OK;
    #if (NAND_PLATFORM==MCU_32BIT)
    U32 u32DMAADDR = ((U32)VA2PA(pBuf))/MIUWIDTH;
    #else
    U32 u32DMAADDR = ((U32)BASEADR_SDRAM_NAND_COPY_BUF/MIUWIDTH);
    pBuf = pBuf;
    #endif


    //DBG_NAND(printk("drvNAND_512PAGESCOPY\r\n"));

    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0 | R_NC_REDU_BYPASS);

    drvNAND_BuildRdd(0xFF, u16Block,0);
    drvNAND_BuildRdd(0xFF, u16Block,1);
    drvNAND_BuildRdd(0xFF, u16Block,2);
    drvNAND_BuildRdd(0xFF, u16Block,3);

    while(u16PageCnt)
    {
        if(u16PageCnt > 0x200)
        {
            u16ProcessPage = 0x200;
        }
        else
        {
            u16ProcessPage = u16PageCnt;
        }
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
        HAL_WRITE_UINT16(NC_MIU_DMA1,(u32DMAADDR>>16));
        HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAADDR&0xFFFF);
        HAL_WRITE_UINT16(NC_MMA_PRI_REG,0); //read
        HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,u32Source);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Source>>16)&0xFF);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest<<8)&0xFF00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest>>8)&0xFFFF);

        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_INSTQUE);

        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_PAGECOPY_US<<8)+OP_A_WAIT_IDLE);

        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C5TFS0<<8)+OP_CMD00_READ);
        }
        else if( _fsinfo.u8AddrWidth == 4)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS0<<8)+OP_CMD00_READ);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS0<<8)+OP_CMD00_READ);
        }

        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_SERIAL_DATA_IN<<8)+OP_A_WAIT_READY_BUSY);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_WAIT_MMA<<8)+OP_A_WAIT_IDLE);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_PAGECOPY_DS<<8)+OP_A_WAIT_IDLE);
        // Derek 2008/01/08 debug
        //HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD80_SERIAL_DATA_INPUT<<8)+OP_A_WAIT_IDLE);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD80_SERIAL_DATA_INPUT<<8)+OP_A_WAIT_READY_BUSY);

        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_SERIAL_DATA_OUT<<8)+OP_ADR_C5TFS1);
        }
        else if( _fsinfo.u8AddrWidth == 4)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_SERIAL_DATA_OUT<<8)+OP_ADR_C4TFS1);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_SERIAL_DATA_OUT<<8)+OP_ADR_C3TFS1);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_WAIT_READY_BUSY<<8)+OP_CMD10_TRUE_PROGRAM);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_CHECK_STATUS<<8)+OP_CMD70_READ_STATUS);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((OP_A_REPEAT+1)<<8)+OP_A_WAIT_MMA);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_BREAK);

        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,u16ProcessPage-1);
        HAL_WRITE_UINT16(NC_ADDEND,0x00);   // Set Address increment
        // TODO: fine tune idle cnt later
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_WAIT_IDLE_CNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x5);
        //==========================================================
        /*
        {
            U8 i;
            U16 u16Tmp;
            HAL_READ_UINT16(NC_CFG0,u16Tmp);
            DBG_NAND(printk("NC_CFG0=0x%X\r\n",u16Tmp));
            HAL_READ_UINT16(NC_CFG1,u16Tmp);
            DBG_NAND(printk("NC_CFG1=0x%X\r\n",u16Tmp));
            for(i=0;i<0x40;i++)
            {
                HAL_WRITE_UINT16(NC_AUXR_ADR,i);
                HAL_READ_UINT16(NC_AUXR_DAT,u16Tmp);
                DBG_NAND(printk("AUXR[0x%X]=0x%X\r\n",i,u16Tmp));
            }

        }
        */
        //==========================================================

        HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);

        eStatus |= drvNAND_wNCJOBEND(true);
        u32Source += u16ProcessPage;
        u32Dest += u16ProcessPage;
        u16PageCnt -= u16ProcessPage;
    }

    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    return(eStatus);

}
#else
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_512PAGECOPY Procedure
*
*   DESCRIPTION:
*       small page copy function
*   NOTE:
*
*********************************************************************/
NCRW_STATUS drvNAND_512PAGESCOPY(U32 u32Source,U32 u32Dest,U16 u16Block,U8 *pBuf,U16 u16PageCnt)
{
    U32 u32DMAADDR = ((U32)VA2PA(pBuf))/MIUWIDTH;
    U16 u16ProcessPage;
    NCRW_STATUS eStatus = NCRW_STS_OK;

    DBG_NAND(printk("drvNAND_512PAGESCOPY\r\n"));

    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0 | R_NC_REDU_BYPASS);

    drvNAND_BuildRdd(0xFF, u16Block,0);
    drvNAND_BuildRdd(0xFF, u16Block,1);
    drvNAND_BuildRdd(0xFF, u16Block,2);
    drvNAND_BuildRdd(0xFF, u16Block,3);

    while(u16PageCnt)
    {
        if(u16PageCnt > 0x200)
        {
            u16ProcessPage = 0x200;
        }
        else
        {
            u16ProcessPage = u16PageCnt;
        }
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
        HAL_WRITE_UINT16(NC_MIU_DMA1,(u32DMAADDR>>16));
        HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAADDR&0xFFFF);
        HAL_WRITE_UINT16(NC_MMA_PRI_REG,0); //read
        HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,u32Source);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Source>>16)&0xFF);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest<<8)&0xFF00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(u32Dest>>8)&0xFFFF);

        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_INSTQUE);
        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C5TFS0<<8)+OP_CMD00_READ);
        }
        else if( _fsinfo.u8AddrWidth == 4)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C4TFS0<<8)+OP_CMD00_READ);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C3TFS0<<8)+OP_CMD00_READ);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_SERIAL_DATA_IN<<8)+OP_A_WAIT_READY_BUSY);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_IDLE<<8)+OP_A_WAIT_MMA);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_IDLE<<8)+OP_A_PAGECOPY_DS);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_CMD80_SERIAL_DATA_INPUT<<8)+OP_A_WAIT_IDLE);
        /*
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_READY_BUSY<<8)+OP_CMD30_READ);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_MMA<<8)+OP_A_SERIAL_DATA_IN);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_PAGECOPY_DS<<8)+OP_A_WAIT_IDLE);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_CMD80_SERIAL_DATA_INPUT<<8)+OP_A_WAIT_IDLE);
        */
        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_SERIAL_DATA_OUT<<8)+OP_ADR_C5TFS1);
        }
        else if( _fsinfo.u8AddrWidth == 4)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_SERIAL_DATA_OUT<<8)+OP_ADR_C4TFS1);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_SERIAL_DATA_OUT<<8)+OP_ADR_C3TFS1);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_READY_BUSY<<8)+OP_CMD10_TRUE_PROGRAM);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_CHECK_STATUS<<8)+OP_CMD70_READ_STATUS);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_WAIT_MMA<<8)+OP_A_REPEAT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8)+OP_A_BREAK);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,u16ProcessPage-1);
        HAL_WRITE_UINT16(NC_ADDEND,0x00);   // Set Address increment
        // TODO: fine tune idle cnt later
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_WAIT_IDLE_CNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x5);
        //==========================================================
/*
        {
            U8 i;
            U16 u16Tmp;
            HAL_READ_UINT16(NC_CFG0,u16Tmp);
            DBG_NAND(printk("NC_CFG0=0x%X\r\n",u16Tmp));
            HAL_READ_UINT16(NC_CFG1,u16Tmp);
            DBG_NAND(printk("NC_CFG1=0x%X\r\n",u16Tmp));
            for(i=0;i<0x40;i++)
        {
                HAL_WRITE_UINT16(NC_AUXR_ADR,i);
                HAL_READ_UINT16(NC_AUXR_DAT,u16Tmp);
                DBG_NAND(printk("AUXR[0x%X]=0x%X\r\n",i,u16Tmp));
        }

        }
*/
        //==========================================================

        HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);

        eStatus |= drvNAND_wNCJOBEND(true);
        u32Source += u16ProcessPage;
        u32Dest += u16ProcessPage;
        u16PageCnt -= u16ProcessPage;
    }

    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    return(eStatus);

}
#endif
#endif
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_FLASH_INIT Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
int drvNAND_FLASH_INIT(void)
{
    #if (NAND_PLATFORM==MCU_32BIT)
    #ifdef ECOS_DISK_DRIVER
    cyg_disk_identify_t ident;
    cyg_io_handle_t     io_h;
    cyg_devtab_entry_t *dev_h;
	int	err;
    disk_channel        *chan;
    cyg_devtab_entry_t *t;
    #endif
    //int err;
    #endif
    DBG_NAND(printk("drvNAND_FLASH_INIT\r\n"));
	#if defined(CONFIG_TITANIA3_FPGA) || defined(CONFIG_GP2_DEMO1)
    DBG_NAND(printk("FCIE2 Ver:%X\r\n",REG(NC_FCIE2_VERSION)));
	#endif
    // open NAND flash power???
    #if (NAND_PLATFORM==MCU_8BIT)
    if(CHIP_VERSION>=CHIP_VER_U04)
        REG(NC_MISC) = (R_NC_DBFWEN_SYNC+R_NC_RW8B_EN);
    else
        REG(NC_MISC) = R_NC_RW8B_EN; // enable REG High 8 ,low 8 bit function
    #else
	#if defined(CONFIG_TITANIA3_FPGA) || defined(CONFIG_GP2_DEMO1)

	#else
    REG(NC_MISC) = 0;
    #endif
    #endif
    drvNAND_SWITCH_PAD(0);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    memset(&_fsinfo, 0, sizeof(_fsinfo));
    _fsinfo.u8CIS_BLOCK = 0xFF;
    #if ENABLE_USER_SETTING_BLOCK
    _fsinfo.u16UserBlock = NO_MAPPING;
    #endif
    // Derek debug 0816
    _fsinfo.u16NC_CFG0 = R_NC_DYNGATED_ON+R_NC_ECCERR_NSTOP+R_NC_CHK_RB_HIGH+R_NC_DSIZE_SEL+R_NC_CE_EN+R_NC_WP_EN;//+R_NC_CE_AUTO; // FPGA version

	//drvNAND_SetNC_CLOCK(NC_CLK_27M);
    drvNAND_SetNC_CLOCK(NC_CLK_43M);

    drvNAND_SoftReset();

    //drvNAND_InitISR();
    // TODO: remove mark later
    //HAL_WRITE_UINT16(NC_MIE_INT_EN,(R_NC_JOB_ENDE+R_NC_MMA_DATA_ENDE));
    HAL_WRITE_UINT16(NC_MIE_INT_EN,0x00);
    HAL_WRITE_UINT16(NC_CARD_INT_EN,0x00);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_NC_EN);
    drvNAND_READ_ID();
    if(_fsinfo.u8ID == 0)
    {
        #if (NAND_PLATFORM==MCU_32BIT)
        //DBG_MSG("\n unknown FLASH\n");
        printk("\n unknown FLASH\n");
        #else
        DBG_NAND(printk("\n unknown FLASH\n"));
        #endif
        drvNAND_Release_PAD();
        return false;
    }

    #ifdef MLC_2PLAN
    if( (_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE) && (_fsinfo.eFlashConfig & FLASH_2KPAGE))
    {
        #if (NAND_PLATFORM==MCU_32BIT)
        printk("\n mlc2plane_op\n");
        #endif
        _fsinfo.nand_op = &mlc2plane_op;
        #if (NAND_PLATFORM==MCU_8BIT)
        //_fsinfo.nand_op->AUTOSCAN = drvNAND_MLC2PLANE_FLASHFORMAT;
        _fsinfo.nand_op->READ_CIS = NULL;
        _fsinfo.nand_op->BUILD_LUT = drvNAND_BUILDLUT;
        _fsinfo.nand_op->NAND_RW = drvNAND_MLC2PLANE_READWRITE;
        #endif
    }
    else
    #endif
    {
        #if (NAND_PLATFORM==MCU_32BIT)
        //printk("\n slc_op\n");
        #endif
#if 0
        _fsinfo.nand_op = &slc_op;
        #if (NAND_PLATFORM==MCU_8BIT)
        _fsinfo.nand_op->AUTOSCAN = NULL;//drvNAND_SLC_FLASHFORMAT;
        _fsinfo.nand_op->READ_CIS = NULL;
        _fsinfo.nand_op->BUILD_LUT = drvNAND_BUILDLUT;
        _fsinfo.nand_op->NAND_RW = drvNAND_SLC_READWRITE;
        #endif
#endif
    }
    _fsinfo.u8CurZone = 0;
    _fsinfo.u8BOOT_MODE = false;
    // TODO: remove debug code
    //drvNAND_DEBUG();
    //drvNAND_MLCDEBUG();
    //drvNAND_CIFD_DEBUG();
    //drvNAND_2KSLCDEBUG();
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
    DBG_NAND(printk("\n Build LUT\n"));
#ifdef NAND_1KLUT
    drvNAND_BUILDLUT(0,0);
#else
    drvNAND_BUILDLUT(0);
#endif

    if( _fsinfo.u8CIS_BLOCK != 0xFF )
    {
        _fsinfo.eFlashConfig |= FLASH_READY;
    }
    else
    {
//#if defined(BOOT_LOADER) || (defined(ENABLE_NAND_APP_CIS_CHECK) && ENABLE_NAND_APP_CIS_CHECK)
#if 0
        //create cis here
        _fsinfo.u8BOOT_MODE = true;
        drvNAND_BUILDCIS(FALSE, FALSE);
        MDrv_NAND_Eject_Insert(0);
        _fsinfo.u8BOOT_MODE = false;
	#ifdef NAND_1KLUT
    	//_fsinfo.nand_op->BUILD_LUT(0x00,0);
    	drvNAND_BUILDLUT(0,0);
	#else
   	 //_fsinfo.nand_op->BUILD_LUT(0x00);
    	drvNAND_BUILDLUT(0);
	#endif

    	if( _fsinfo.u8CIS_BLOCK != 0xFF )
        {
            _fsinfo.eFlashConfig |= FLASH_READY;
        }
    	else
#endif  //#ifdef  BOOT_LOADER
    	{
            DBG_NAND(printk("\r\n!!!!!!!No CIS block!!!!!!!\r\n"));
            DBG_NAND(printk("\r\nPlease use NANDAPP.EXE to initial NAND flash\r\n"));
#if 0
        _fsinfo.nand_op->AUTOSCAN();
#ifdef NAND_1KLUT
        drvNAND_BUILDLUT(0x00,0);
#else
        drvNAND_BUILDLUT(0x00);
#endif
            if( _fsinfo.u8CIS_BLOCK != 0xFF )
            {
                _fsinfo.eFlashConfig |= FLASH_READY;
            }
            else
#endif
            {
                drvNAND_Release_PAD();
                return false;
            }
        }
    }
    #if (NAND_PLATFORM==MCU_32BIT)
    DBG_NAND(printk("\n NAND Disk init\n"));
    #else
    DBG_NAND(printk("NAND_INIT_END\r\n"));
    #endif

    //drvNAND_RW_TEST();
    //while(_fsinfo.u16NC_CFG0)
    //    ;

    #ifdef ECOS_DISK_DRIVER
    for (t = &__DEVTAB__[0]; t != &__DEVTAB_END__; t++)
    {
        if (strncmp("/dev/csdb/", t->name,10)==0)
        {
            chan = (disk_channel *) t->priv;
            chan->info->connected = true;
            break;
        }
    }

    err = cyg_io_lookup("/dev/csdb/0", &io_h);
    if (err != ENOERR)
    {
        DBG_MSG("\n /dev/csdb not found\n");

        return false;
    }

    dev_h = (cyg_devtab_entry_t *) io_h;
    chan = (disk_channel*) dev_h->priv;

    ident.serial[0] = '\0';
    ident.firmware_rev[0] = '\0';
    ident.model_num[0] = '\0';
    ident.cylinders_num = 0;
    ident.heads_num = 0;
    ident.sectors_num = 0;
    ident.lba_sectors_num =  _fsinfo.u32Capacity-1;
    ident.phys_block_size=1;
    ident.max_transfer = 512;

    if (!(chan->callbacks->disk_init)(dev_h))
        return false;

    if (ENOERR != (chan->callbacks->disk_connected)(dev_h, &ident))
        return false;
#endif
#endif

    drvNAND_Release_PAD();
    return true;
//================================
}

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_ReadNANDInfo Procedure
*
*   DESCRIPTION:
*       Output: pu8Buf[6]
*   NOTE:
*
*********************************************************************/
void MDrv_NAND_ReadNANDInfo(U8 *pu8Buf)
{
    pu8Buf[0] = _fsinfo.u8ID;
    pu8Buf[1] = _fsinfo.eMaker;
    pu8Buf[2] = _fsinfo.eflashtype;
    pu8Buf[3] = _fsinfo.eflashplane;
    if(_fsinfo.u16BlocksPerCS==512)
        pu8Buf[4] = 9;
    else if(_fsinfo.u16BlocksPerCS==1024)
        pu8Buf[4] = 10;
    else if(_fsinfo.u16BlocksPerCS==2048)
        pu8Buf[4] = 11;
    else if(_fsinfo.u16BlocksPerCS==4096)
        pu8Buf[4] = 12;
    else if(_fsinfo.u16BlocksPerCS==8192)
        pu8Buf[4] = 13;
    else if(_fsinfo.u16BlocksPerCS==16384)
        pu8Buf[4] = 14;
    else if(_fsinfo.u16BlocksPerCS==32768)
        pu8Buf[4] = 15;
    pu8Buf[5] = _fsinfo.u8Pageshift;
    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
        pu8Buf[6] =  0x1;
    else
        pu8Buf[6] =  0x0;
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_EraseBlock Procedure
*
*   DESCRIPTION:
*       input: u32Row: Flash block address
*
*       return:
*           true: success
*           fasle: fail
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
bool MDrv_NAND_EraseBlock(U32 u32Row)
{
    NCRW_STATUS eStatus;
    U8 u8Mode=_fsinfo.u8BOOT_MODE;

    DBG_NAND(printk("MDrv_NAND_EraseBlock\r\n"));
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    _fsinfo.u8BOOT_MODE = true;
    if(_fsinfo.u8ID != 0x00)
    {
        drvNAND_SWITCH_PAD(0);
        drvNAND_EraseBlock(u32Row);
        eStatus = drvNAND_wNCJOBEND(false);
        _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
        HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
        drvNAND_Release_PAD();
        _fsinfo.u8BOOT_MODE = u8Mode;
        if(eStatus!= NCRW_STS_OK)
        {
            DBG_NAND(printk("Erase fail\r\n"));
            return(false);
        }
        else
        {
            DBG_NAND(printk("Erase PASS\r\n"));
            return(true);
        }
    }
    else
    {
        _fsinfo.u8BOOT_MODE = u8Mode;
        return(false);
    }

}
#else
void MDrv_NAND_Erase1(U32 u32Row)
{
    NCRW_STATUS eStatus;

    DBG_NAND(printk("MDrv_NAND_EraseBlock\r\n"));
    drvNAND_SWITCH_PAD(0);
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
    HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C3TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C2TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
    }
    else
    {
        if( _fsinfo.u8AddrWidth == 3)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C2TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C3TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_BREAK);
    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(false);

}

bool MDrv_NAND_Erase2(void)
{
    NCRW_STATUS eStatus;

    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_WAIT_READY_BUSY<<8) + OP_CMDD0_BLOCK_ERASE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_CHECK_STATUS<<8) + OP_CMD70_READ_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(false);

    if(eStatus!= NCRW_STS_OK)
    {
        (printk("Erase fail\r\n"));
        return(false);
    }
    else
    {
        DBG_NAND(printk("Erase PASS\r\n"));
        return(true);
    }


}
#endif

#if 0
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_ReadPHYPage Procedure
*
*   DESCRIPTION:
*       input:
*           u32Row: Flash block address
*       output:
*           pBuf: Physical page data and 16 bytes RDD data
*
*   NOTE:
*
*********************************************************************/
#if (NAND_PLATFORM==MCU_32BIT)
void MDrv_NAND_ReadPHYPage(U32 u32Row,U8 *pBuf,U16 u16Len)
#else
void MDrv_NAND_ReadPHYPage(U32 u32Row,U32 u32DRMAPHYADDR,U16 u16Len)
#endif
{
    #if (NAND_PLATFORM==MCU_32BIT)
    U32 u32DMAAddr = (U32)VA2PA(pBuf)/MIUWIDTH;
    //U16 i;
    #else
    U32 u32DMAAddr = (u32DRMAPHYADDR/MIUWIDTH);
    U8 u8Win1StarAddr,u8Win1EndAddr;
    U32 u32Win1PhyAdr;
    U8 xdata *pBuf;// = 0xF000;

    MDrv_GetXdataWindow1Map(&u8Win1StarAddr, &u8Win1EndAddr, &u32Win1PhyAdr);
    if(u16Len>0x800)
    {
        MDrv_MapXdataWindow1(60,64,u32DRMAPHYADDR+2048);
        pBuf = (U8*)(0xF000 + ((u32DRMAPHYADDR+2048) & 0xFFF));
    }
    else
    {
        MDrv_MapXdataWindow1(60,64,u32DRMAPHYADDR+512);
        pBuf = (U8*)(0xF000 + ((u32DRMAPHYADDR+512) & 0xFFF));
    }
    #endif

    drvNAND_SWITCH_PAD(0);
    //DBG_NAND(printk("MDrv_NAND_ReadPHYPage,u32DRMAPHYADDR=0x%lX\r\n",u32DRMAPHYADDR));
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_MMA_DATA_END+R_NC_JOB_END);

    if(u16Len==0x810)
    {
        //HAL_DCACHE_FLUSH((U32)pu8Buf, 2048+16);
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);
    }
    else if(u16Len==0x840)
    {
        //HAL_DCACHE_FLUSH((U32)pu8Buf, 2048+64);
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);
    }
    else
    {
        //HAL_DCACHE_FLUSH((U32)pu8Buf, 512+16);
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
    }

    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    drvNAND_ReadPage(u32Row,0x00);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    #if (defined(ENABLE_LINUX_MTD)  && (ENABLE_LINUX_MTD==1) )
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    #else
    HAL_WRITE_UINT16(NC_CTRL,R_NC_ECC_BYPASS+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    #endif
    drvNAND_wNCJOBEND(true);
    for(u32DMAAddr=0;u32DMAAddr<16;u32DMAAddr++)
    {
        if(u16Len==0x810)
        {
            pBuf[0x800+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
        }
        else if(u16Len==0x840)
        {
            pBuf[0x800+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
            pBuf[0x800+u32DMAAddr+0x10] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x10);
            pBuf[0x800+u32DMAAddr+0x20] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x20);
            pBuf[0x800+u32DMAAddr+0x30] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x30);
        }
        else
        {
            pBuf[0x200+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
        }
    }
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    drvNAND_Release_PAD();

    //DBG_NAND(printk("end of MDrv_NAND_ReadPHYPage\r\n"));
    #if (NAND_PLATFORM==MCU_8BIT)
    MDrv_MapXdataWindow1(u8Win1StarAddr, u8Win1EndAddr, u32Win1PhyAdr);
    #endif
}
#endif

#if 0
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_WritePHYPage Procedure
*
*   DESCRIPTION:
*       input:
*           u32Row: Flash block address
*       return:
*           true: success
*           fasle: fail
*
*   NOTE:
*
*********************************************************************/
#if (NAND_PLATFORM==MCU_32BIT)
bool MDrv_NAND_WritePHYPage(U32 u32Row,U8 *pBuf,U16 u16Len,U8 u8Ecc)
#else
bool MDrv_NAND_WritePHYPage(U32 u32Row,U32 u32DRAMPHYADDR,U16 u16Len,U8 u8Ecc)
#endif
{
    #if (defined(ENABLE_LINUX_MTD)  && (ENABLE_LINUX_MTD==1) )
    U8 u8Idx;
    U16 u16PageSize;
    #endif
    U8 u8Tmp;
    U32 u32DMAAddr;
    NCRW_STATUS eStatus;

    DBG_NAND(printk("MDrv_NAND_WritePHYPage\r\n"));
    DBG_NAND(printk("Row=0x%X,Len:0x%X\r\n",u32Row,u16Len));
    DBG_NAND(printk("u32DRAMPHYADDR=0x%lX\r\n",(U32)pBuf));
    drvNAND_SWITCH_PAD(0);
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

    HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_MMA_DATA_END+R_NC_JOB_END);

    #if (NAND_PLATFORM==MCU_32BIT)
    u32DMAAddr = (U32)VA2PA(pBuf)/MIUWIDTH;
    #else
    u32DMAAddr = (u32DRAMPHYADDR/MIUWIDTH);
    #endif

    #if (defined(ENABLE_LINUX_MTD)  && (ENABLE_LINUX_MTD==1) )
    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
        {
        u8Tmp = 64;
        u16PageSize = 0x800;
        }
        else
        {
        u8Tmp = 16;
        u16PageSize = 0x200;
    }
    u8Idx = 0;
    for(u8Idx=0;u8Idx<u8Tmp;u8Idx++)
    {
        drvNAND_SetCMD_RSP_BUF(u8Idx,pBuf[u16PageSize+u8Idx]);
    }
    #else
    for(u8Tmp=0;u8Tmp<16;u8Tmp++)
    {
        drvNAND_SetCMD_RSP_BUF(u8Tmp, u8RDDBuf[u8Tmp]);
        drvNAND_SetCMD_RSP_BUF(u8Tmp+16, u8RDDBuf[u8Tmp+16]);
        drvNAND_SetCMD_RSP_BUF(u8Tmp+32, u8RDDBuf[u8Tmp+32]);
        drvNAND_SetCMD_RSP_BUF(u8Tmp+48, u8RDDBuf[u8Tmp+48]);
    }
    #endif
    if((u16Len==0x800) || (u16Len==0x840))
    {
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);
        //HAL_DCACHE_FLUSH(pBuf, 2048+64);
    }
    else
    {
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
        //HAL_DCACHE_FLUSH(pBuf, 512+16);
    }
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR); //write
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage(u32Row, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    if(u8Ecc)
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    else
        HAL_WRITE_UINT16(NC_CTRL,R_NC_ECC_BYPASS+R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);


    eStatus = drvNAND_wNCJOBEND(true);
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    drvNAND_Release_PAD();
    if(eStatus==NCRW_STS_OK)
    {
        DBG_NAND(printk("MDrv_NAND_WritePHYPage PASS\r\n"));
        return(true);
    }
    else
    {
        DBG_NAND(printk("MDrv_NAND_WritePHYPage false\r\n"));
        return(false);
    }

}
#endif

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_WriteRDD Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#if (NAND_PLATFORM==MCU_32BIT)
void MDrv_NAND_WriteRDD(U8 *pBuf,U8 u8Len)
#else
void MDrv_NAND_WriteRDD(U8 xdata *pBuf,U8 u8Len)
#endif
{
    U8 u8Tmp;

    for(u8Tmp=0;u8Tmp<u8Len;u8Tmp++)
    {
        u8RDDBuf[u8Tmp] = pBuf[u8Tmp];
    }
}
#endif
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_Eject_Insert Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#if ( !defined(ENABLE_LINUX_MTD) && (ENABLE_LINUX_MTD==0))
void MDrv_NAND_Eject_Insert(U8 u8CMD)
{
    drvNAND_SWITCH_PAD(0);
    if(u8CMD==0)
    {
        _fsinfo.u8CIS_BLOCK = 0xFF;
        _fsinfo.eFlashConfig &= ~FLASH_READY;
        _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
        _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN);
        HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
        drvNAND_BUILDLUT(0x00);
        _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
        HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
        _fsinfo.u8CurZone = 0x00;
        if(_fsinfo.u8CIS_BLOCK != 0xFF)
        {
            _fsinfo.eFlashConfig |= (FLASH_MEDIACHANGE+FLASH_READY);
        }
    }
    drvNAND_Release_PAD();
}
#endif

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_GetCapacity Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
U32 MDrv_NAND_GetCapacity(void)
{
    DBG_NAND(printk("MDrv_NAND_GetCapacity=%lX\r\n",_fsinfo.u32Capacity));
    return(_fsinfo.u32Capacity);
}
#endif

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_CheckReady Procedure
*
*   DESCRIPTION:
*       For
*   NOTE:
*
*********************************************************************/
NC_NAND_STATUS MDrv_NAND_CheckReady(void)
{
    static U8 u8Cnt=3;
    if(_fsinfo.eFlashConfig & FLASH_READY)
    {
        if(_fsinfo.eFlashConfig & FLASH_MEDIACHANGE)
        {
            if(--u8Cnt==0)
            {
                _fsinfo.eFlashConfig &= ~FLASH_MEDIACHANGE;
                u8Cnt = 3;
                return(NC_STS_READY);
            }
            return(NC_STS_MEDIA_CHANGE);
        }
        //DBG_NAND(printk("MDrv_NAND_CheckReady: Raedy\r\n"));
        return(NC_STS_READY);
    }
    else
    {
        //DBG_NAND(printk("MDrv_NAND_CheckReady: NOT Raedy!!!\r\n"));
        return(NC_STS_NOT_READY);
    }

}
#endif
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_ForceWriteBack Procedure
*
*   DESCRIPTION:
*       For
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
void MDrv_NAND_ForceWriteBack(void)
{
    DBG_NAND(printk("MDrv_NAND_ForceWriteBack\r\n"));
    if(_fsinfo.eFlashConfig & FLASH_READY)
    {
        if(_fsinfo.WriteBack)
        {
            drvNAND_SWITCH_PAD(0);
            DBG_NAND(printk("Clr STG3\r\n"));
            _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
            _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
            HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
            HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_MMA_DATA_END+R_NC_JOB_END);
            drvNAND_SLC_WriteBack();
            _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
            HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
            drvNAND_Release_PAD();
        }
    }
}
#endif

#ifdef ECOS_DISK_DRIVER
// ----------------------------------------------------------------------------
bool
nand_disk_init(struct cyg_devtab_entry *tab)
{
    disk_channel *chan = (disk_channel *) tab->priv;
    cyg_disk_info_t *info = chan->info;

    info->connected = false;
    chan->valid = false;

    if (chan->init)
        return true;

    //drvNAND_FLASH_INIT();

    return true;
}
// ----------------------------------------------------------------------------

Cyg_ErrNo
nand_disk_lookup(struct cyg_devtab_entry **tab,
                        struct cyg_devtab_entry  *sub_tab,
                        const char *name)
{
    disk_channel *chan = (disk_channel *) (*tab)->priv;
    return (chan->callbacks->disk_lookup)(tab, sub_tab, name);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       nand_disk_read Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/

Cyg_ErrNo
nand_disk_read(disk_channel *chan,
                    void         *buf,
                    U32    len,
                    U32    block_num)
{
    NCRW_STATUS eStatus;
    //void *pTmpBuf;
    printk("\nnand_disk_read LEN=0x%lX, LBA=0x%lX\r\n",len,block_num);
    //drvNAND_SoftReset();
    if ( (U32)buf & 0x7 ) //must be 4-B aligned
    {
        printk("NAND Driver critical error,buffer not 4bytes alignment \n");
        return(EIO);
    }
    else //if ( ((U32)buf&(U32)KSEG1_BASE) == (U32)KSEG0_BASE ) //must be non-cahceable
    if ((cyg_uint32)buf!=CYGARC_UNCACHED_ADDRESS(buf))
    {
        //DBG_NAND(printk("Dcache flush \r\n"));
        //HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
        //pTmpBuf = KSEG02KSEG1(buf);
    }
    //else
    //{
        //pTmpBuf = (void *)buf;
    //}
    //eStatus = _fsinfo.nand_op->NAND_RW(RW_READ,len,block_num,(U32)VA2PA(pTmpBuf)/MIUWIDTH);
    eStatus = _fsinfo.nand_op->NAND_RW(RW_READ,len,block_num+JPG_NAND_BUF_SECTOR_SIZE,(U32)VA2PA(buf)/MIUWIDTH);

    // TODO: remove debug message
    #if 0
    if(block_num>0x200)
    {
        U32 u32Cnt;
        for(u32Cnt=0;u32Cnt<(len*0x200);u32Cnt++)
        {
            DBG_NAND(printk("%2X ",((U8*)buf)[u32Cnt]));
            if((u32Cnt & 0xF)==0xF)
                DBG_NAND(printk("\n"));
        }
        DBG_NAND(printk("\n\n"));
    }
    #endif
    if(eStatus != NCRW_STS_OK)
    {
        DBG_NAND(printk("NAND_R_FAIL\n"));
        return(EIO);
    }
    else
    {
        DBG_NAND(printk("NAND_R_OK\n"));
        return(ENOERR);
    }
}

Cyg_ErrNo
nand_disk_direct_read(disk_channel *chan,
                    void         *buf,
                    U32    len,
                    U32    block_num)
{
    NCRW_STATUS eStatus;
    //void *pTmpBuf;
    //DBG_NAND(printk("\nnand_disk_read LEN=0x%lX, LBA=0x%lX, buf=0x%lX\r\n",len,block_num,(U32)buf));
    //DBG_NAND(printk("nand_disk_r LEN=0x%lX,LBA=0x%lX,buf=0x%lX\n",len,block_num,(U32)buf));
    //drvNAND_SoftReset();
    if ( (U32)buf & 0x3 ) //must be 4-B aligned
    {
        //DBG_NAND(printk("NAND buffer not 4bytes alignment \n"));
        return(EIO);
    }
    else //if ( ((U32)buf&(U32)KSEG1_BASE) == (U32)KSEG0_BASE ) //must be non-cahceable
    if ((cyg_uint32)buf!=CYGARC_UNCACHED_ADDRESS(buf))
    {
        //DBG_NAND(printk("Dcache flush \r\n"));
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
        //pTmpBuf = KSEG02KSEG1(buf);
    }
    //else
    //{
        //pTmpBuf = (void *)buf;
    //}
    //eStatus = _fsinfo.nand_op->NAND_RW(RW_READ,len,block_num,(U32)VA2PA(pTmpBuf)/MIUWIDTH);
    eStatus = _fsinfo.nand_op->NAND_RW(RW_READ,len,block_num,(U32)VA2PA(buf)/MIUWIDTH);

    // TODO: remove debug message
    #if 0
    if(block_num>0x200)
    {
        U32 u32Cnt;
        for(u32Cnt=0;u32Cnt<(len*0x200);u32Cnt++)
        {
            DBG_NAND(printk("%2X ",((U8*)buf)[u32Cnt]));
            if((u32Cnt & 0xF)==0xF)
                DBG_NAND(printk("\n"));
        }
        DBG_NAND(printk("\n\n"));
    }
    #endif
    if(eStatus != NCRW_STS_OK)
    {
        //DBG_NAND(printk("NAND_R_FAIL\n"));
        return(EIO);
    }
    else
    {
        //DBG_NAND(printk("NAND_R_OK\n"));
        return(ENOERR);
    }
}

// ----------------------------------------------------------------------------

Cyg_ErrNo
nand_disk_write(disk_channel *chan,
                    const void   *buf,
                    U32    len,
                    U32    block_num)
{
    NCRW_STATUS eStatus;

    //DBG_NAND(printk("nand_disk_write LEN=0x%lX, LBA=0x%lX, buf=0x%lX\r\n",len,block_num,(U32)buf));
    DBG_NAND(printk("nand_disk_w LEN=0x%lX,LBA=0x%lX\n",len,block_num));
    // TODO: remove debug message
    #if 0
    if(block_num>0x200)
    {
        U32 u32Cnt;
        for(u32Cnt=0;u32Cnt<(len*0x200);u32Cnt++)
        {
            DBG_NAND(printk("%2X ",((U8*)buf)[u32Cnt]));
            if((u32Cnt & 0xF)==0xF)
                DBG_NAND(printk("\n"));
        }
        DBG_NAND(printk("\n\n"));
    }
    #endif

    if ((cyg_uint32)buf!=CYGARC_UNCACHED_ADDRESS(buf))
    {
        // TODO: remove mark later
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
    }

    eStatus = _fsinfo.nand_op->NAND_RW(RW_WRITE,len,block_num+JPG_NAND_BUF_SECTOR_SIZE,(U32)VA2PA(buf)/MIUWIDTH);

    if(eStatus != NCRW_STS_OK)
    {
        DBG_NAND(printk("NAND_W_FAIL\n"));
        return(EIO);
    }
    else
    {
        DBG_NAND(printk("NAND_W_OK\n"));
        return(ENOERR);
    }
}

Cyg_ErrNo __attribute__((nomips16))
nand_disk_direct_write(disk_channel *chan,
                    const void   *buf,
                    U32    len,
                    U32    block_num)
{
    NCRW_STATUS eStatus;

    DBG_NAND(printk("nand_disk_write LEN=0x%lX, LBA=0x%lX\r\n",len,block_num));
    //DBG_NAND(printk("nand_disk_direct_w\r\n")); //LEN=0x%lX,LBA=0x%lX,buf=0x%lX\n",len,block_num,(U32)buf));
    // TODO: remove debug message
    #if 0
    if(block_num>0x200)
    {
        U32 u32Cnt;
        for(u32Cnt=0;u32Cnt<(len*0x200);u32Cnt++)
        {
            DBG_NAND(printk("%2X ",((U8*)buf)[u32Cnt]));
            if((u32Cnt & 0xF)==0xF)
                DBG_NAND(printk("\n"));
        }
        DBG_NAND(printk("\n\n"));
    }
    #endif

    if ((cyg_uint32)buf!=CYGARC_UNCACHED_ADDRESS(buf))
    {
        // TODO: remove mark later
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
    }

    //drvNAND_SoftReset();
    eStatus = _fsinfo.nand_op->NAND_RW(RW_WRITE,len,block_num,(U32)VA2PA(buf)/MIUWIDTH);

    if(eStatus != NCRW_STS_OK)
    {
        DBG_NAND(printk("NAND_W_FAIL\n"));
        return(EIO);
    }
    else
    {
        DBG_NAND(printk("NAND_W_OK\n"));
        return(ENOERR);
    }
}

// ----------------------------------------------------------------------------

Cyg_ErrNo
nand_disk_get_config(disk_channel *chan,
                            U32    key,
                            const void   *xbuf,
                            U32   *len)
{
    return -EINVAL;
}

// ----------------------------------------------------------------------------

Cyg_ErrNo
nand_disk_set_config(disk_channel *chan,
                            U32    key,
                            const void   *xbuf,
                            U32   *len)
{
    Cyg_ErrNo                   result  = ENOERR;
    // cyg_mmc_bus_device*    disk    = (cyg_mmc_bus_device*) chan->dev_priv;
    //cyg_disk_info_t *info = chan->info;

    switch(key)
    {
    case CYG_IO_SET_CONFIG_DISK_MOUNT:
        // There will have been a successful lookup(), so there's
        // little point in checking the disk again.
        // if ((sd_chk_ready()!=_DEVICE_READY)&&
        //     result=-EINVAL;
        //else device is OK

        break;

    case CYG_IO_SET_CONFIG_DISK_UMOUNT:
        if (0 == chan->info->mounts)
        {
            //cyg_mmc_bus* bus = disk->mmc_bus_dev;

            // If this is the last unmount of the card, mark it as
            // disconnected. If the user then removes the card and
            // plugs in a new one everything works cleanly.
            // disk->mmc_connected = false;
            // info->connected = false;
            //result = (chan->callbacks->disk_disconnected)(chan);

            /* deselect it too if it had been selected */
            /* We don't care about any error really. This device is going away. */
            //mmc_bus_select_card( bus, NULL );
        }
        break;
    }

    return result;
    // return -EINVAL;
}
#else

#if ENABLE_CERAMAL_NAND_IMAGE
/*********************************************************************
*
*   PROCEDURE NAME:
*       nand_image_read Procedure
*
*   DESCRIPTION:
*       u8ImageNo: 0-> image 0, 1-> Image 1
*       block_num: 0~CERAMAL_NAND_IMAGE_SECTOR_SIZE
*   NOTE:
*
*********************************************************************/
NCRW_STATUS
nand_image_read(U8 u8ImageNo,
                    U32 buf,
                    U32 len,
                    U32 block_num)
{
    NCRW_STATUS eStatus;
    //void *pTmpBuf;
    DBG_NAND(printk("\nnand_image_readd LEN=0x%lX, LBA=0x%lX\r\n",len,block_num));
    //drvNAND_SoftReset();

    if ( (U32)buf & 0x7 ) //must be 4-B aligned
    {
        (printk("NAND Driver critical error,buffer not 8bytes alignment \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    if( (block_num+len-1) > CERAMAL_NAND_IMAGE_SECTOR_SIZE)
    {
        (printk("NAND Driver critical error,address exceed Image size \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    #if (NAND_PLATFORM==MCU_32BIT)
    else //if ( ((U32)buf&(U32)KSEG1_BASE) == (U32)KSEG0_BASE ) //must be non-cahceable
    if ((U32)buf!=(U32)GetUnCachedAddr(buf))
    {
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
        //pTmpBuf = KSEG02KSEG1(buf);
    }
    else
    {
        //pTmpBuf = (void *)buf;
    }
    #endif
    if(u8ImageNo)
    {
        block_num+=CERAMAL_NAND_IMAGE_SECTOR_SIZE;
    }
    drvNAND_SWITCH_PAD(0);
    eStatus = drvNAND_IMAGE_READWRITE(RW_READ,(U16)len,(U16)block_num,(U32)(buf)/MIUWIDTH);
    drvNAND_Release_PAD();
    if(eStatus != NCRW_STS_OK)
    {
        //printk("r fail\r\n");
        return(eStatus);
    }
    else
    {
        //printk("r ok\r\n");
        return(NCRW_STS_OK);
    }
}
#endif

/*********************************************************************
*
*   PROCEDURE NAME:
*       nand_disk_read Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
NCRW_STATUS
nand_disk_read(void *chan,
                    U32 buf,
                    U32 len,
                    U32 block_num)
{
    NCRW_STATUS eStatus=NCRW_STS_OK;
    //void *pTmpBuf;
    #if(ENABLE_SHARE_PIN_PATCH)
    U8  u8CardSts,u8CardDet;
    #endif
    (printk("\nnand_disk_read LEN=0x%lX, LBA=0x%lX\r\n",len,block_num));
    //drvNAND_SoftReset();

    if ( (U32)buf & 0x7 ) //must be 4-B aligned
    {
        (printk("NAND Driver critical error,buffer not 8bytes alignment \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    if( (block_num+len-1) > _fsinfo.u32Capacity)
    {
        (printk("NAND Driver critical error,address exceed Capacity \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    #if (NAND_PLATFORM==MCU_32BIT)
    else //if ( ((U32)buf&(U32)KSEG1_BASE) == (U32)KSEG0_BASE ) //must be non-cahceable
    if ((U32)buf!=(U32)GetUnCachedAddr(buf))
    {
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
        //pTmpBuf = KSEG02KSEG1(buf);
    }
    else
    {
        //pTmpBuf = (void *)buf;
    }
    #endif
    drvNAND_SWITCH_PAD(0);
    #if(0)
    if(gu8Boardtype == MSPD1111B)
    {
    }
    #endif
    //eStatus = _fsinfo.nand_op->NAND_RW(RW_READ,len,block_num,(U32)VA2PA(pTmpBuf)/MIUWIDTH);
    //eStatus = _fsinfo.nand_op->NAND_RW(RW_READ,len,block_num,(U32)VA2PA(buf)/MIUWIDTH);
    eStatus = drvNAND_SLC_READWRITE(RW_READ,len,block_num+JPG_NAND_BUF_SECTOR_SIZE+WALLPAPER_SECTOR_SIZE,(U32)VA2PA(buf)/MIUWIDTH);
    drvNAND_Release_PAD();
    if(eStatus != NCRW_STS_OK)
    {
        printk("r fail\r\n");
        return(eStatus);
    }
    else
    {
        printk("r ok\r\n");
        return(NCRW_STS_OK);
    }

    chan = chan;
}
#endif

// ----------------------------------------------------------------------------
#if ENABLE_CERAMAL_NAND_IMAGE
/*********************************************************************
*
*   PROCEDURE NAME:
*       nand_image_write Procedure
*
*   DESCRIPTION:
*       u8ImageNo: 0-> image 0, 1-> Image 1
*       block_num: 0~CERAMAL_NAND_IMAGE_SECTOR_SIZE
*   NOTE:
*
*********************************************************************/
NCRW_STATUS
nand_image_write(U8 u8ImageNo,
                    U32 buf,
                    U32 len,
                    U32 block_num)
{
    NCRW_STATUS eStatus;

    //DBG_NAND(printk("nand_image_write LEN=0x%lX, LBA=0x%lX\r\n",len,block_num));
    //(printk(",n_w, "));//
    if ( (U32)buf & 0x7 ) //must be 8-B aligned
    {
        (printk("NAND Driver critical error,buffer not 8bytes alignment \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    if( (block_num+len-1) > CERAMAL_NAND_IMAGE_SECTOR_SIZE)
    {
        (printk("NAND Driver critical error,address exceed Capacity \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    #if (NAND_PLATFORM==MCU_32BIT)
    if ((U32)buf!=(U32)GetUnCachedAddr(buf))
    {
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
    }
    #endif
    if(u8ImageNo)
    {
        block_num+=CERAMAL_NAND_IMAGE_SECTOR_SIZE;
    }
    drvNAND_SWITCH_PAD(0);
    eStatus = drvNAND_IMAGE_READWRITE(RW_WRITE,(U16)len,(U16)block_num,(U32)(buf)/MIUWIDTH);
    drvNAND_Release_PAD();
    if(eStatus != NCRW_STS_OK)
    {
        //printk("w fail\r\n");
        return(eStatus);
    }
    else
    {
        //printk("w ok\r\n");
        //printk("P\r\n");
        return(NCRW_STS_OK);
    }

}
#endif

#if ENABLE_WALLPAPER_IMAGE
/*********************************************************************
*
*   PROCEDURE NAME:
*       nand_wallpaper_read Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
NCRW_STATUS
nand_wallpaper_read(
                    U32 buf,
                    U32 len,
                    U32 block_num)
{
    NCRW_STATUS eStatus;
    //void *pTmpBuf;
    DBG_NAND(printk("\nnand_wallpaper_read LEN=0x%lX, LBA=0x%lX,DRAM=0x%lX\r\n",len,block_num,buf));
    //drvNAND_SoftReset();

    if ( (U32)buf & 0x7 ) //must be 4-B aligned
    {
        (printk("NAND Driver critical error,buffer not 8bytes alignment \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    if( (block_num+len-1) > WALLPAPER_SECTOR_SIZE)
    {
        (printk("NAND Driver critical error,address exceed Image size \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    #if (NAND_PLATFORM==MCU_32BIT)
    else //if ( ((U32)buf&(U32)KSEG1_BASE) == (U32)KSEG0_BASE ) //must be non-cahceable
    if ((U32)buf!=(U32)GetUnCachedAddr(buf))
    {
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
        //pTmpBuf = KSEG02KSEG1(buf);
    }
    else
    {
        //pTmpBuf = (void *)buf;
    }
    #endif
    drvNAND_SWITCH_PAD(0);
    //eStatus = _fsinfo.nand_op->NAND_RW(RW_READ,len,block_num,(U32)VA2PA(pTmpBuf)/MIUWIDTH);
    //eStatus = _fsinfo.nand_op->NAND_RW(RW_READ,len,block_num,(U32)VA2PA(buf)/MIUWIDTH);
    eStatus = drvNAND_SLC_READWRITE(RW_READ,len,block_num,(U32)(buf)/MIUWIDTH);
    drvNAND_Release_PAD();
    if(eStatus != NCRW_STS_OK)
    {
        //printk("r fail\r\n");
        return(eStatus);
    }
    else
    {
        //printk("r ok\r\n");
        return(NCRW_STS_OK);
    }

}

// ----------------------------------------------------------------------------
NCRW_STATUS
nand_wallpaper_write(
                    U32 buf,
                    U32 len,
                    U32 block_num)
{
    NCRW_STATUS eStatus;

    DBG_NAND(printk("nand_wallpaper_write LEN=0x%lX, LBA=0x%lXDRAM=0x%lX\r\n",len,block_num,buf));
    //(printk(",n_w, "));//
    if ( (U32)buf & 0x7 ) //must be 8-B aligned
    {
        (printk("NAND Driver critical error,buffer not 8bytes alignment \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    if( (block_num+len-1) > WALLPAPER_SECTOR_SIZE)
    {
        (printk("NAND Driver critical error,address exceed Capacity \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    #if (NAND_PLATFORM==MCU_32BIT)
    if ((U32)buf!=(U32)GetUnCachedAddr(buf))
    {
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
    }
    #endif
    drvNAND_SWITCH_PAD(0);
    //drvNAND_SoftReset();
    //eStatus = _fsinfo.nand_op->NAND_RW(RW_WRITE,len,block_num,(U32)VA2PA(buf)/MIUWIDTH);
    eStatus = drvNAND_SLC_READWRITE(RW_WRITE,len,block_num,(U32)(buf)/MIUWIDTH);
    drvNAND_Release_PAD();
    if(eStatus != NCRW_STS_OK)
    {
        //printk("w fail\r\n");
        return(eStatus);
    }
    else
    {
        //printk("w ok\r\n");
        //printk("P\r\n");
        return(NCRW_STS_OK);
    }
}
#endif

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
NCRW_STATUS
nand_disk_write(void *chan,
                    U32 buf,
                    U32 len,
                    U32 block_num)
{
    NCRW_STATUS eStatus=NCRW_STS_OK;

    DBG_NAND(printk("nand_disk_write LEN=0x%lX, LBA=0x%lX\r\n",len,block_num));
    if ( (U32)buf & 0x7 ) //must be 8-B aligned
    {
        (printk("NAND Driver critical error,buffer not 8bytes alignment \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    if( (block_num+len-1) > _fsinfo.u32Capacity)
    {
        (printk("NAND Driver critical error,address exceed Capacity \n"));
        return(NCRW_STS_ADDRESS_ERROR);
    }
    #if (NAND_PLATFORM==MCU_32BIT)
    if ((U32)buf!=(U32)GetUnCachedAddr(buf))
    {
        HAL_DCACHE_FLUSH(buf,len*NC_SECTOR_SIZE);
    }
    #endif
    drvNAND_SWITCH_PAD(0);
    //drvNAND_SoftReset();
    //eStatus = _fsinfo.nand_op->NAND_RW(RW_WRITE,len,block_num,(U32)VA2PA(buf)/MIUWIDTH);
    eStatus = drvNAND_SLC_READWRITE(RW_WRITE,len,block_num+JPG_NAND_BUF_SECTOR_SIZE+WALLPAPER_SECTOR_SIZE,(U32)VA2PA(buf)/MIUWIDTH);
    drvNAND_Release_PAD();
    if(eStatus != NCRW_STS_OK)
    {
        //printk("w fail\r\n");
        return(eStatus);
    }
    else
    {
        //printk("w ok\r\n");
        //printk("P\r\n");
        return(NCRW_STS_OK);
    }

    chan = chan;
}
#endif //#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
#endif

#ifndef BOOT_LOADER
#if ENABLE_USER_SETTING_BLOCK
/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_ReadCFGfromNANDFlash Procedure
*
*   DESCRIPTION:
*
*
*   NOTE:
*
*********************************************************************/
void MDrv_GE_DMA(U32 u32srcaddr, U32 u32dstaddr, U32 u32len);
NCRW_STATUS MDrv_NAND_ReadUserSetting_dma(U32 u32dstaddr,U32 u32len)
{
    U32 u32TmpDMAAddr = ((U32)BASEADR_SDRAM_NAND_COPY_BUF/MIUWIDTH);
    NCRW_STATUS eStatus=NCRW_STS_OK;
    U8 IsNoMapping=false;

    drvNAND_SWITCH_PAD(0);
    DBG_NAND(printk("MDrv_NAND_ReadUserSetting\r\n"));
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    if( (_fsinfo.u8CurZone!=0) || (_fsinfo.FreeQueue.Count==0))
    {
        if(_fsinfo.WriteBack)
        {
            drvNAND_SLC_WriteBack();
        }
        drvNAND_BUILDLUT(0);
        _fsinfo.u8CurZone = 0x00;
    }
    _fsinfo.u16NC_CFG0 &= ~(R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    if(_fsinfo.u16UserBlock == NO_MAPPING)
    {   // No User information Block
        _fsinfo.u16UserBlock = _pu16LUT.pu16LUT[_fsinfo.FreeQueue.Head+MAX_LOGI_BLK];
        IsNoMapping = true;
    }
    DBG_NAND(printk("UserBlock=0x%X\r\n",_fsinfo.u16UserBlock));
    HAL_WRITE_UINT16(NC_JOB_BL_CNT,4);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32TmpDMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32TmpDMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
    drvNAND_ReadPage((_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift),0x00);//read block 0 page u16Tmp

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    }
    else
    {
        HAL_WRITE_UINT16(NC_ADDEND,0x00);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,3); // repeat count
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // random data count ignored
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // radom addres offset must be 0
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(true);

    //MDrv_MIU_SDRAM2VARCopy(BASEADR_SDRAM_NAND_COPY_BUF,dstaddr, u16Len);
    MDrv_GE_DMA(BASEADR_SDRAM_NAND_COPY_BUF, u32dstaddr, u32len );
    if(IsNoMapping)
        _fsinfo.u16UserBlock = NO_MAPPING;
    drvNAND_Release_PAD();
    return(eStatus);
}

NCRW_STATUS MDrv_NAND_ReadUserSetting(U8 *dstaddr,U16 u16Len)
{
    U32 u32TmpDMAAddr = ((U32)BASEADR_SDRAM_NAND_COPY_BUF/MIUWIDTH);
    NCRW_STATUS eStatus=NCRW_STS_OK;
    U8 IsNoMapping=false;
    drvNAND_SWITCH_PAD(0);
    DBG_NAND(printk("MDrv_NAND_ReadUserSetting\r\n"));
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    if( (_fsinfo.u8CurZone!=0) || (_fsinfo.FreeQueue.Count==0))
    {
        if(_fsinfo.WriteBack)
        {
            drvNAND_SLC_WriteBack();
        }
        drvNAND_BUILDLUT(0);
        _fsinfo.u8CurZone = 0x00;
    }
    _fsinfo.u16NC_CFG0 &= ~(R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    if(_fsinfo.u16UserBlock == NO_MAPPING)
    {   // No User information Block
        _fsinfo.u16UserBlock = _pu16LUT.pu16LUT[_fsinfo.FreeQueue.Head+MAX_LOGI_BLK];
        IsNoMapping = true;
    }
    DBG_NAND(printk("UserBlock=0x%X\r\n",_fsinfo.u16UserBlock));
    HAL_WRITE_UINT16(NC_JOB_BL_CNT,4);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32TmpDMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32TmpDMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
    drvNAND_ReadPage((_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift),0x00);//read block 0 page u16Tmp

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    }
    else
    {
        HAL_WRITE_UINT16(NC_ADDEND,0x00);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,3); // repeat count
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // random data count ignored
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // radom addres offset must be 0
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(true);

    MDrv_MIU_SDRAM2VARCopy(BASEADR_SDRAM_NAND_COPY_BUF,dstaddr, u16Len);
    drvNAND_Release_PAD();
    if(IsNoMapping)
        _fsinfo.u16UserBlock = NO_MAPPING;
    return(eStatus);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       MDrv_NAND_WriteCFGfromNANDFlash Procedure
*
*   DESCRIPTION:
*
*
*   NOTE:
*
*********************************************************************/
NCRW_STATUS MDrv_NAND_WriteUserSetting_dma(U32 u32srcaddr,U32 u32len)
{
    U32 u32TmpDMAAddr = ((U32)BASEADR_SDRAM_NAND_COPY_BUF/MIUWIDTH);
    NCRW_STATUS eStatus=NCRW_STS_OK;

    drvNAND_SWITCH_PAD(0);
    DBG_NAND(printk("MDrv_NAND_WriteUserSetting\r\n"));
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

    if(_fsinfo.u16UserBlock == NO_MAPPING)
    {   // No User information Block
        if(_fsinfo.WriteBack)
        {
            drvNAND_SLC_WriteBack();
        }
        if( (_fsinfo.u8CurZone!=0) || (_fsinfo.FreeQueue.Count==0))
        {
            drvNAND_BUILDLUT(0);
            _fsinfo.u8CurZone = 0x00;
        }
        _fsinfo.u16UserBlock = drvNAND_GetFreeBlock();
        if(!(_fsinfo.u16UserBlock & ERASED_BLOCK))
        {
            if(drvNAND_EraseBlock(_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift)==NCRW_STS_OK)
            {
                drvNAND_wNCJOBEND(false);
            }
            else
            {
                _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
                HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
                drvNAND_Release_PAD();
                return(NCRW_STS_WRITE_PROTECT);
            }
        }
        else
        {
            _fsinfo.u16UserBlock &= 0x3FF;
        }
    }
    DBG_NAND(printk("UserBlock=0x%X\r\n",_fsinfo.u16UserBlock));
    HAL_WRITE_UINT16(NC_JOB_BL_CNT,4);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32TmpDMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32TmpDMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
    drvNAND_ReadPage((_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift),0x00);//read block 0 page u16Tmp

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    }
    else
    {
        HAL_WRITE_UINT16(NC_ADDEND,0x00);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,3); // repeat count
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // random data count ignored
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // radom addres offset must be 0
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(true);

    if(drvNAND_EraseBlock(_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift)==NCRW_STS_OK)
    {
        drvNAND_wNCJOBEND(false);
    }
    else
    {
        _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
        HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
        drvNAND_Release_PAD();
        return(NCRW_STS_WRITE_PROTECT);
    }


    //MDrv_MIU_VAR2SDRAMCopy(srcaddr, BASEADR_SDRAM_NAND_COPY_BUF, u16Len);
    MDrv_GE_DMA(u32srcaddr, BASEADR_SDRAM_NAND_COPY_BUF, u32len );

    drvNAND_BuildRdd(0xFF, USER_SETTING_LBA, 0);
    drvNAND_BuildRdd(0xFF, USER_SETTING_LBA, 1);
    drvNAND_BuildRdd(0xFF, USER_SETTING_LBA, 2);
    drvNAND_BuildRdd(0xFF, USER_SETTING_LBA, 3);

    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 0x4);
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32TmpDMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32TmpDMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage((_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift), 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_REPEAT<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_BREAK);
    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    }
    else
    {
        HAL_WRITE_UINT16(NC_ADDEND,0x00);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,3); // repeat count
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(true);

    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    drvNAND_Release_PAD();
    return(eStatus);
}

NCRW_STATUS MDrv_NAND_WriteUserSetting(U8 *srcaddr,U16 u16Len)
{
    U32 u32TmpDMAAddr = ((U32)BASEADR_SDRAM_NAND_COPY_BUF/MIUWIDTH);
    NCRW_STATUS eStatus=NCRW_STS_OK;

    drvNAND_SWITCH_PAD(0);
    DBG_NAND(printk("MDrv_NAND_WriteUserSetting\r\n"));
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

    if(_fsinfo.u16UserBlock == NO_MAPPING)
    {   // No User information Block
        if(_fsinfo.WriteBack)
        {
            drvNAND_SLC_WriteBack();
        }
        if( (_fsinfo.u8CurZone!=0) || (_fsinfo.FreeQueue.Count==0))
        {
            drvNAND_BUILDLUT(0);
            _fsinfo.u8CurZone = 0x00;
        }
        _fsinfo.u16UserBlock = drvNAND_GetFreeBlock();
        if(!(_fsinfo.u16UserBlock & ERASED_BLOCK))
        {
            if(drvNAND_EraseBlock(_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift)==NCRW_STS_OK)
            {
                drvNAND_wNCJOBEND(false);
            }
            else
            {
                return(NCRW_STS_WRITE_PROTECT);
            }
        }
        else
        {
            _fsinfo.u16UserBlock &= 0x3FF;
        }
    }
    DBG_NAND(printk("UserBlock=0x%X\r\n",_fsinfo.u16UserBlock));
    HAL_WRITE_UINT16(NC_JOB_BL_CNT,4);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32TmpDMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32TmpDMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
    drvNAND_ReadPage((_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift),0x00);//read block 0 page u16Tmp

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    }
    else
    {
        HAL_WRITE_UINT16(NC_ADDEND,0x00);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,3); // repeat count
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // random data count ignored
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // radom addres offset must be 0
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(true);

    if(drvNAND_EraseBlock(_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift)==NCRW_STS_OK)
    {
        drvNAND_wNCJOBEND(false);
    }
    else
    {
        _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
        HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
        drvNAND_Release_PAD();
        return(NCRW_STS_WRITE_PROTECT);
    }

    MDrv_MIU_VAR2SDRAMCopy(srcaddr, BASEADR_SDRAM_NAND_COPY_BUF, u16Len);

    drvNAND_BuildRdd(0xFF, USER_SETTING_LBA, 0);
    drvNAND_BuildRdd(0xFF, USER_SETTING_LBA, 1);
    drvNAND_BuildRdd(0xFF, USER_SETTING_LBA, 2);
    drvNAND_BuildRdd(0xFF, USER_SETTING_LBA, 3);

    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 0x4);
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32TmpDMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32TmpDMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage((_fsinfo.u16UserBlock<<_fsinfo.u8Pageshift), 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_REPEAT<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_BREAK);
    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    }
    else
    {
        HAL_WRITE_UINT16(NC_ADDEND,0x00);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,3); // repeat count
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(true);

    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    drvNAND_Release_PAD();
    return(eStatus);
}
#endif
#endif //#ifndef BOOT_LOADER

#if (!defined(ENABLE_LINUX_MTD) || (ENABLE_LINUX_MTD==0))
NCRW_STATUS MDrv_NAND_Load_IMAGE(U32 u32DRAMADDR)
{
    U32 u32CodeLength=0,u32Tmp;
    U16 u16Chksum=0,u16Sftsum=0,u16NewChksum=0,u16NewSfsum=0;
    U8 *pBuf = (U8 *)VA2PA(u32DRAMADDR);

    printk("MDrv_NAND_Load_IMAGE\r\n");
    drvNAND_FLASH_INIT();
    printk("Read Header\r\n");
    if(nand_disk_read(NULL,u32DRAMADDR,8,0)==NCRW_STS_OK)
    {
        u16Chksum = ((U16)pBuf[0] << 8) + (U16)pBuf[1];
        u16Sftsum = ((U16)pBuf[2] << 8) + (U16)pBuf[3];
        u32CodeLength = ((U32)pBuf[4] << 24) + ((U32)pBuf[5] << 16) + ((U32)pBuf[6] << 8) + (U32)pBuf[7];
        printk("Chksum=0x%X\r\n",u16Chksum);
        printk("Sftsum=0x%X\r\n",u16Sftsum);
        printk("CodeLength=0x%X\r\n",u32CodeLength);
        if(u32CodeLength==0)
        {
            printk("Read Header fail\r\n");
            return(false);
        }
    }
    else
    {
        printk("Read Header fail\r\n");
        return(false);
    }
    if(nand_disk_read(NULL,u32DRAMADDR,(u32CodeLength + NC_SECTOR_SIZE-1)/NC_SECTOR_SIZE,8)==NCRW_STS_OK)
    {
        printk("Read IMAGE PASS\r\n");
        u16NewChksum = 0;
        u16NewSfsum = 0;
        u32Tmp = 0;
        while(u32Tmp<u32CodeLength)
        {
            u16NewChksum += pBuf[u32Tmp];
            u16NewSfsum <<= 1;
            u16NewSfsum += pBuf[u32Tmp];
            u32Tmp++;
        }
        printk("New Chksum=0x%X\r\n",u16NewChksum);
        printk("New Sftsum=0x%X\r\n",u16NewSfsum);
        if(u16NewChksum != u16Chksum)
            printk("Check sum error\r\n");
        if(u16NewSfsum != u16Sftsum)
            printk("Shift sum error\r\n");
    }
    else
    {
        printk("Read IMAGE fail\r\n");
        return(false);
    }
    return(true);
}
#endif

#if 0
void drvNAND_DumpDRAM(U8 *buf,U32 u32Len)
{
    U32 k;

    for(k=0;k<u32Len;k++)
    {
        (printk("%2X ",buf[k]));
        if((k&0x1FF)==0x1FF)
    	{
            (printk("Sec:%X=================\r\n",k/0x200));
        }
        if((k&0xF) == 0xF)
            (printk("\r\n"));
    }
}


#define TEST_DATA_LEN   0x1000  // 64k Bytes
#define XWINDOWS_SIZE   0x1000  // 4k Bytes
#define NAND_TEST_BUF  (2*1024*1024ul)
//#define MAD_JPEG_INBUFFER_LEN       (0x4000)
//#define TEST_LBA        (_fsinfo.u32Capacity-(TEST_DATA_LEN/512))
#define TEST_LBA        (WALLPAPER_SECTOR_SIZE - (TEST_DATA_LEN/512))
void drvNAND_RW_TEST(void)
{
    data U32 u32SectorNo;//,u32SrcAddr;
    U32 u16ByteNo,i;
    U8  u8windowIdx,u8Retry;

    DBG_NAND(printk("NAND Test Read/Write\n"));
    //if( _fsinfo.eFlashConfig & FLASH_READY)
    {
        //DBG_NAND(printk("Ready For Test......\n"));
        for(u32SectorNo=0;u32SectorNo < TEST_LBA;u32SectorNo++)
        {
            //DBG_NAND(printk("Testing LBA:%ld......\n", u32SectorNo));
            //DBG_NAND(printk("Initial Test Pattern....\n"));
            //DBG_NAND(printk("DRAM Starting ADDR=0x%lX....\n",(U32)MAD_JPEG_INBUFFER_ADR));
            for(u8windowIdx=0;u8windowIdx<((U32)TEST_DATA_LEN/(U32)XWINDOWS_SIZE);u8windowIdx++)
            {
                //DBG_NAND(printk("windowIdx:%bx\n", u8windowIdx));
                //MDrv_Sys_SetXdataWindow1Base(((U32)MAD_JPEG_INBUFFER_ADR+(U32)u8windowIdx*XWINDOWS_SIZE)>>12);
                MDrv_MapXdataWindow1( 60, 64, ((U32)NAND_TEST_BUF+(U32)u8windowIdx*XWINDOWS_SIZE));
                for(i=0;i<(XWINDOWS_SIZE/0x200);i++)
                {
                    for(u16ByteNo=0;u16ByteNo<0x200;u16ByteNo++)
                    {
                        XBYTE[0xF000+i*0x200+u16ByteNo] = (U8)(u32SectorNo+i+u16ByteNo);
                    }
                }
            }
            //while(1);
            //if(nand_disk_write(NULL, (U32)NAND_TEST_BUF, ((U32)TEST_DATA_LEN/0x200), u32SectorNo)!= NCRW_STS_OK)
            if(nand_wallpaper_write((U32)NAND_TEST_BUF, ((U32)TEST_DATA_LEN/0x200), u32SectorNo)!= NCRW_STS_OK)
            {
                DBG_NAND(printk("Write fail\r\n"));
            }
            else
                DBG_NAND(printk("Write OK\n"));
            //DBG_NAND(printk("Reset Test Pattern to AA ....\n"));
            for(u8windowIdx=0;u8windowIdx<((U32)TEST_DATA_LEN/(U32)XWINDOWS_SIZE);u8windowIdx++)
            {
                //DBG_NAND(printk("windowIdx:%bx\n", u8windowIdx));
                //MDrv_Sys_SetXdataWindow1Base(((U32)MAD_JPEG_INBUFFER_ADR+(U32)u8windowIdx*XWINDOWS_SIZE)>>12);
                MDrv_MapXdataWindow1( 60, 64, ((U32)NAND_TEST_BUF+(U32)u8windowIdx*XWINDOWS_SIZE));
                for(i=0;i<(XWINDOWS_SIZE/0x200);i++)
                {
                    for(u16ByteNo=0;u16ByteNo<0x200;u16ByteNo++)
                    {
                        XBYTE[0xF000+i*0x200+u16ByteNo] = 0xAA;
                    }
                }
            }
            u8Retry = 3;
RETRY_READ:
            //if(nand_disk_read(NULL, (U32)NAND_TEST_BUF, ((U32)TEST_DATA_LEN/0x200), u32SectorNo)!= NCRW_STS_OK)
            if(nand_wallpaper_read( (U32)NAND_TEST_BUF, ((U32)TEST_DATA_LEN/0x200), u32SectorNo)!= NCRW_STS_OK)
            {
                DBG_NAND(printk("Read fail\r\n"));
            }
            else
                DBG_NAND(printk("Read OK\n"));
            //DBG_NAND(printk("Compare Test Pattern From Read Data .....\n"));
            for(u8windowIdx=0;u8windowIdx<((U32)TEST_DATA_LEN/(U32)XWINDOWS_SIZE);u8windowIdx++)
            {
                //DBG_NAND(printk("windowIdx:%bx\n", u8windowIdx));
                //MDrv_Sys_SetXdataWindow1Base(((U32)MAD_JPEG_INBUFFER_ADR+(U32)u8windowIdx*XWINDOWS_SIZE)>>12);
                MDrv_MapXdataWindow1( 60, 64, ((U32)NAND_TEST_BUF+(U32)u8windowIdx*XWINDOWS_SIZE));

                for(i=0;i<(XWINDOWS_SIZE/0x200);i++)
                {
                    for(u16ByteNo=0;u16ByteNo<0x200;u16ByteNo++)
                    {
                        if(XBYTE[0xF000+i*0x200+u16ByteNo] != (U8)(u32SectorNo+i+u16ByteNo))
                        {
                            /*
                            if(u8Retry>0)
                            {
                                u8Retry--;
                                goto RETRY_READ;
                            }
                            */
                            DBG_NAND(printk("Compare fail\r\n"));
                            DBG_NAND(printk("0x%bX != 0x%bX\r\n",XBYTE[0xF000+i*0x200+u16ByteNo],(U8)(u32SectorNo+i+u16ByteNo)));
                            DBG_NAND(printk("i=0x%lX,u16ByteNo=0x%lX\r\n",i,u16ByteNo));
                            goto DUMPDRAM;
                        }
                    }
                }
            }
            DBG_NAND(printk("Compare Pass\n"));
            //if(kbhit())
            //    break;
        }
        DBG_NAND(printk("testing 2\r\n"));
        for(u32SectorNo=0;u32SectorNo<TEST_LBA ;u32SectorNo++)
        {
            MDrv_MapXdataWindow1( 60, 64, ((U32)NAND_TEST_BUF));
            for(i=0;i<(XWINDOWS_SIZE/0x200);i++)
            {
                for(u16ByteNo=0;u16ByteNo<0x200;u16ByteNo++)
                {
                    XBYTE[0xF000+i*0x200+u16ByteNo] = 0xAA;
                }
            }
            if(nand_disk_read(NULL, (U32)NAND_TEST_BUF, 0x1, u32SectorNo)!= NCRW_STS_OK)
            {
                DBG_NAND(printk("Read fail\r\n"));
            }
            else
            {
                DBG_NAND(printk("Read OK\n"));
            }
            for(u16ByteNo=0;u16ByteNo<0x200;u16ByteNo++)
            {
                if(XBYTE[0xF000+u16ByteNo] != (U8)(u32SectorNo+u16ByteNo))
                {
                    DBG_NAND(printk("Compare fail\r\n"));
                    DBG_NAND(printk("u32SectorNo=0x%lX,u16ByteNo=0x%X\r\n",u32SectorNo,u16ByteNo));
                    for(u16ByteNo=0;u16ByteNo<0x200;u16ByteNo++)
                    {
                        DBG_NAND(printk("%2bX ",XBYTE[0xF000+u16ByteNo]));
                        if((u16ByteNo & 0xF) == 0xF)
                            DBG_NAND(printk("\r\n"));
                    }
                    return;
                }
            }

        }
        DBG_NAND(printk("test ok\n"));
    }
        return;
DUMPDRAM:
    for(u8windowIdx=0;u8windowIdx<((U32)TEST_DATA_LEN/(U32)XWINDOWS_SIZE);u8windowIdx++)
    {
        DBG_NAND(printk("windowIdx:%bx\r\n", u8windowIdx));
        MDrv_MapXdataWindow1( 60, 64, ((U32)NAND_TEST_BUF+(U32)u8windowIdx*XWINDOWS_SIZE));
        for(i=0;i<(XWINDOWS_SIZE/0x200);i++)
        {
            DBG_NAND(printk("Offset 0x%lX=================================\r\n",i));
            for(u16ByteNo=0;u16ByteNo<0x200;u16ByteNo++)
            {
                DBG_NAND(printk("%2bX ",XBYTE[0xF000+i*0x200+u16ByteNo]));
                if((u16ByteNo & 0xF) == 0xF)
                    DBG_NAND(printk("\r\n"));
            }
            DBG_NAND(printk("\r\n"));
        }
    }
    MDrv_Timer_Delayms(5000);
    DBG_NAND(printk("\r\nRe-Read\r\n"));
    //nand_disk_read(NULL, (U32)NAND_TEST_BUF, ((U32)TEST_DATA_LEN/0x200), u32SectorNo);
    nand_wallpaper_read( (U32)NAND_TEST_BUF, ((U32)TEST_DATA_LEN/0x200), u32SectorNo);
    for(u8windowIdx=0;u8windowIdx<((U32)TEST_DATA_LEN/(U32)XWINDOWS_SIZE);u8windowIdx++)
    {
        DBG_NAND(printk("windowIdx:%bx\r\n", u8windowIdx));
        MDrv_MapXdataWindow1( 60, 64, ((U32)NAND_TEST_BUF+(U32)u8windowIdx*XWINDOWS_SIZE));
        for(i=0;i<(XWINDOWS_SIZE/0x200);i++)
        {
            DBG_NAND(printk("Offset 0x%lX=================================\r\n",i));
            for(u16ByteNo=0;u16ByteNo<0x200;u16ByteNo++)
            {
                DBG_NAND(printk("%2bX ",XBYTE[0xF000+i*0x200+u16ByteNo]));
                if((u16ByteNo & 0xF) == 0xF)
                    DBG_NAND(printk("\r\n"));
            }
            DBG_NAND(printk("\r\n"));
        }
    }

}
#endif

#if 0
void drvNAND_DEBUG(void)
{
    //U8  u8SrcBuf[NFIE_TEST_LEN] __attribute__((aligned(128)));
    //U8  u8DstBuf[NFIE_TEST_LEN] __attribute__((aligned(128)));
    U8  i=0,*pu8TmpBuf;
    U16 j=0;
    U32 u32DMAAddr;
    NCRW_STATUS eStatus;

    DBG_NAND(printk("drvNAND_DEBUG\r\n"));
    pu8TmpBuf = Nand_CopyBuffer;
    //pu8TmpBuf = (U8*)0xA0100000;
    u32DMAAddr = ((U32)VA2PA(pu8TmpBuf))/MIUWIDTH;
    drvNAND_EraseBlock(0x00);
    if( drvNAND_wNCJOBEND(false) != NCRW_STS_OK)
    {
        DBG_NAND(printk("Erase 0 Fail\r\n"));
    }
    drvNAND_EraseBlock(0x01<<_fsinfo.u8Pageshift);
    if( drvNAND_wNCJOBEND(false) != NCRW_STS_OK)
    {
        DBG_NAND(printk("Erase 1 Fail\r\n"));
    }
    drvNAND_BuildRdd(0xFF, 0xAAAA,0);
    drvNAND_BuildRdd(0xFF, 0xAAAA,1);
    drvNAND_BuildRdd(0xFF, 0xAAAA,2);
    drvNAND_BuildRdd(0xFF, 0xAAAA,3);

    while(i<0x20)
    {
        for(j=0;j<0x200;j++)
        {
            pu8TmpBuf[j] = (i+1);
        }
        HAL_WRITE_UINT16(NC_JOB_BL_CNT, 1);
        HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAAddr>>16);
        HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
        HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR);
        HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
        drvNAND_WritePage(i, 0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_REPEAT<<8)+OP_A_CHECK_STATUS);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_BREAK);
        HAL_WRITE_UINT16(NC_ADDEND,0x00);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
        HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
        eStatus = drvNAND_wNCMMA_JOBEND();
        i++;
    }
    for(j=0;j<(0x200*6);j++)
    {
        pu8TmpBuf[j] = 0xAA;
    }
    eStatus =  drvNAND_512PAGESCOPY(0x00,(0x1<<_fsinfo.u8Pageshift), 0xBBBB,pu8TmpBuf,0x2);

    for(i=0;i<6;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            DBG_NAND(printk("%2X ",pu8TmpBuf[(U16)i*0x200+j]));
            if( (j&0xF)==0xF )
                DBG_NAND(printk("\r\n"));
        }
        DBG_NAND(printk("*****************************************************\r\n"));
    }

    while(_fsinfo.u8Pageshift)
        ;

}
#endif

#if 0
void drvNAND_MLCDEBUG(void)
{
    //U8  u8SrcBuf[NFIE_TEST_LEN] __attribute__((aligned(128)));
    //U8  u8DstBuf[NFIE_TEST_LEN] __attribute__((aligned(128)));
    U8  i=0,*pu8TmpBuf;
    U16 j=0;
    U32 u32DMAAddr,u32Free=4,u32Free1,u32Source=10,u32Source1=11;
    NCRW_STATUS eStatus;

    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

    DBG_NAND(printk("drvNAND_MLCDEBUG\r\n"));
    pu8TmpBuf = Nand_CopyBuffer;
    u32DMAAddr = ((U32)VA2PA(pu8TmpBuf))/MIUWIDTH;

    #if 0
    //1 Read Test
    //======================================================================
    while(_fsinfo.u8Pageshift)
        {
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, 0x00);    // read from card
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_ReadPage(u32Source, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0); // repeat count
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START+R_NC_ECC_BYPASS);
    drvNAND_wNCJOBEND(true);
    //for(i=0;i<6;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            printk("%2X ",pu8TmpBuf[(U16)i*0x200+j]);
            if( (j&0xF)==0xF )
                printk("\r\n");
        }
        printk("*****************************************************\r\n");
    }
        }

    //======================================================================
    #endif
    DBG_NAND(printk("u32Free=0x%X\r\n",u32Free));
    u32Source <<= _fsinfo.u8Pageshift;
    u32Source1 <<= _fsinfo.u8Pageshift;
    u32Free += (((U32)_fsinfo.u8CurZone)<<10);
    u32Free1 = u32Free + 1;
    u32Free <<= _fsinfo.u8Pageshift;
    u32Free1 <<= _fsinfo.u8Pageshift;
    drvNAND_MLC2PLANE_Erase2Block(u32Source1);
    if(drvNAND_wNCJOBEND(false)!=NCRW_STS_OK)
    {
        DBG_NAND(printk("SRC Erase Fail\r\n"));
    }
    else
    {
        DBG_NAND(printk("SRC Erase OK\r\n"));
    }
    drvNAND_MLC2PLANE_Erase2Block(u32Free1);
    if(drvNAND_wNCJOBEND(false)!=NCRW_STS_OK)
    {
        DBG_NAND(printk("Erase Fail\r\n"));
    }
    else
    {
        DBG_NAND(printk("Erase OK\r\n"));
    }
    drvNAND_BuildRdd(0xFF, 0xAAAA,0);
    drvNAND_BuildRdd(0xFF, 0xAAAA,1);
    drvNAND_BuildRdd(0xFF, 0xAAAA,2);
    drvNAND_BuildRdd(0xFF, 0xAAAA,3);

    #if 1
    //1 Write Test
    //======================================================================
    for(i=0;i<4;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            pu8TmpBuf[(U16)i*0x200+j] = (i+0x10);
        }
    }
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, R_NC_JOB_RW_DIR);    // read from card
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage(u32Source, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0); // repeat count
    HAL_WRITE_UINT16(NC_AUXR_DAT,0);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(true);
    #endif
    #if 0
    for(i=0;i<4;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            pu8TmpBuf[(U16)i*0x200+j] = (i+0x14);
        }
    }
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, R_NC_JOB_RW_DIR);    // read from card
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage(u32Source+1, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0); // repeat count
    HAL_WRITE_UINT16(NC_AUXR_DAT,0);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(true);
    for(i=0;i<4;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            pu8TmpBuf[(U16)i*0x200+j] = (i+0xC0);
        }
    }
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, R_NC_JOB_RW_DIR);    // read from card
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage(u32Source1, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0); // repeat count
    HAL_WRITE_UINT16(NC_AUXR_DAT,0);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(true);
    for(i=0;i<4;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            pu8TmpBuf[(U16)i*0x200+j] = (i+0xC4);
        }
    }
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, R_NC_JOB_RW_DIR);    // read from card
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage(u32Source1+1, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0); // repeat count
    HAL_WRITE_UINT16(NC_AUXR_DAT,0);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(true);
    //======================================================================
    #endif

    //1 Partial write
    // Partial Write
    // testing 1
    //u32DMAAddr = ((U32)VA2PA(pu8CIS))>>2;
    for(i=0;i<4;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            pu8TmpBuf[(U16)i*0x200+j] = (i+0xC0);
        }
    }
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 1);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, R_NC_JOB_RW_DIR);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage(u32Free, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count
    //HAL_WRITE_UINT16(NC_CFG0,R_NC_WP_EN+R_NC_CHK_RB_HIGH+R_NC_PSIZE_SEL+R_NC_DSIZE_SEL+R_NC_CE_EN);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_SER_PART_MODE+R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(true);
    //u32DMAAddr += (NC_SECTOR_SIZE/MIUWIDTH);
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 3);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, 0x00);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_ReadPage(u32Source, 0x210);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // random data count ignored
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // radom addres offset must be 0
    HAL_WRITE_UINT16(NC_CTRL,((3-1)<<8)+R_NC_SER_PART_MODE+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus |= drvNAND_wNCJOBEND(true);

    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 3);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, R_NC_JOB_RW_DIR);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_WritePage(u32Free, 0x210);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count
    HAL_WRITE_UINT16(NC_CTRL,((U16)(3-1)<<8)+R_NC_SER_PART_MODE+R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(true);
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, 0x00);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_ReadPage(u32Free, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // random data count ignored
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // radom addres offset must be 0
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus |= drvNAND_wNCJOBEND(true);
    for(i=0;i<4;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            DBG_NAND(printk("%2X ",pu8TmpBuf[(U16)i*0x200+j]));
            if( (j&0xF)==0xF )
                DBG_NAND(printk("\r\n"));
        }
        DBG_NAND(printk("*****************************************************\r\n"));
    }

    //===============================================================
    //printk("u32Free=0x%X\r\n",u32Free);

    //eStatus |= drvNAND_MLC2PLANE_UNITSCOPY(u32Source, u32Source1, u32Free1, 0xBBBB, pu8TmpBuf,0x40>>3);
    //eStatus |= drvNAND_MLC2PLANE_UNITSCOPY(u32Source, u32Source1, u32Free1, 0xBBBB, pu8TmpBuf,3);


    while(_fsinfo.u8Pageshift)
        ;

}
#endif

#if 0
void drvNAND_CIFD_DEBUG(void)
{
    U8  i=0,*pu8TmpBuf;
    U16 j=0;
    U32 u32DMAAddr;
    NCRW_STATUS eStatus;

    DBG_NAND(printk("drvNAND_CIFD_DEBUG\r\n"));
    pu8TmpBuf = Nand_CopyBuffer;
    u32DMAAddr = ((U32)VA2PA(pu8TmpBuf))/MIUWIDTH;
    for(j=0;j<0x200;j++)
    {
        ((U16*)pu8TmpBuf)[j] = j;
    }
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 2);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_ADDEND,0x00);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCMMAEND();
    drvNAND_SoftReset();
    for(j=0;j<0x200;j++)
    {
        ((U16*)pu8TmpBuf)[j] = 0xAA;
    }
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 2);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,0x00);
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_SERIAL_DATA_IN);
    HAL_WRITE_UINT16(NC_ADDEND,0x00);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCMMAEND();
    for(j=0;j<0x400;j++)
    {
        DBG_NAND(printk("%2X ",pu8TmpBuf[j]));
        if((j&0x1FF)==0x1FF)
            DBG_NAND(printk("\r\n"));
        if((j&0xF)==0xF)
            DBG_NAND(printk("\r\n"));
    }
}
#endif

#if 0
void drvNAND_2KSLCDEBUG(void)
{
    //U8  u8SrcBuf[NFIE_TEST_LEN] __attribute__((aligned(128)));
    //U8  u8DstBuf[NFIE_TEST_LEN] __attribute__((aligned(128)));
    U8  i=0,*pu8TmpBuf;
    U16 j=0;
    U16 u16PPA;
    U32 u32DMAAddr,u32Free=3,u32Source=2;
    NCRW_STATUS eStatus;

    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

    DBG_NAND(printk("drvNAND_2kSLCDEBUG\r\n"));
    pu8TmpBuf = Nand_CopyBuffer;
    u32DMAAddr = ((U32)VA2PA(pu8TmpBuf))/MIUWIDTH;

    //1 Erase Free Block
    u32Source <<= _fsinfo.u8Pageshift;
    u32Free <<= _fsinfo.u8Pageshift;
    drvNAND_EraseBlock(u32Source);
    eStatus = drvNAND_wNCJOBEND(false);
    if(eStatus != NCRW_STS_OK)
        DBG_NAND(printk("Erase 1 fail"));
    drvNAND_EraseBlock(u32Free);
    eStatus = drvNAND_wNCJOBEND(false);
    if(eStatus != NCRW_STS_OK)
        DBG_NAND(printk("Erase 2 fail"));
    #if 1
    //1 Write Test
    //======================================================================

    //for(u16PPA=0;u16PPA<_fsinfo.u8PagesPerBlock;u16PPA++)
    //for(u16PPA=0;u16PPA<3;u16PPA++)
    u16PPA = 0x00;
    {
        drvNAND_BuildRdd(0xFF, 0xABCD,0);
        drvNAND_BuildRdd(0xFF, 0xABCD,1);
        drvNAND_BuildRdd(0xFF, 0xABCD,2);
        drvNAND_BuildRdd(0xFF, 0xABCD,3);
        for(i=0;i<4;i++)
        {
            for(j=0;j<(0x200);j++)
            {
                pu8TmpBuf[(U16)i*0x200+j] = (U8)(u16PPA+i);
            }
        }
        HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
        HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
        HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
        HAL_WRITE_UINT16(NC_MMA_PRI_REG, R_NC_JOB_RW_DIR);    // read from card
        HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
        drvNAND_WritePage(u32Source+u16PPA, 0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD10_TRUE_PROGRAM<<8)+OP_A_SERIAL_DATA_OUT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_CMD70_READ_STATUS<<8)+OP_A_WAIT_READY_BUSY);
        HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_CHECK_STATUS);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0); // repeat count
        HAL_WRITE_UINT16(NC_AUXR_DAT,0);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0);
        HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
        drvNAND_wNCJOBEND(true);

        HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
        HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
        HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
        HAL_WRITE_UINT16(NC_MMA_PRI_REG, 0x00);    // read from card
        HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
        drvNAND_ReadPage(u32Source+u16PPA, 0x00);
        HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        HAL_WRITE_UINT16(NC_AUXR_DAT,0); // repeat count
        HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START+R_NC_ECC_BYPASS);
        drvNAND_wNCJOBEND(true);
    }
    #endif
    #if 1
    //1 Read Test
    //======================================================================
    HAL_WRITE_UINT16(NC_JOB_BL_CNT, 4);
    HAL_WRITE_UINT16(NC_MIU_DMA0, u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MIU_DMA1, u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG, 0x00);    // read from card
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL, R_MMA_ENABLE+R_NC_EN);
    drvNAND_ReadPage(u32Source, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0); // repeat count
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START+R_NC_ECC_BYPASS);
    drvNAND_wNCJOBEND(true);
    for(i=0;i<4;i++)
    {
        for(j=0;j<(0x200);j++)
        {
            printk("%2bX ",pu8TmpBuf[(U16)i*0x200+j]);
            if( (j&0xF)==0xF )
                printk("\r\n");
        }
        printk("*****************************************************\r\n");
    }
    #endif
    #if (NAND_PLATFORM==MCU_32BIT)
    HAL_DELAY_US(2000000);
    #else
    MDrv_Timer_Delayms(2000);
    #endif
    //drvNAND_2KPAGESCOPY(u32Source, u32Free, 0xABCD, _fsinfo.u8PagesPerBlock, pu8TmpBuf);
    drvNAND_2KPAGESCOPY(u32Source, u32Free, 0xABCD, 3, pu8TmpBuf);
    while(_fsinfo.u8Pageshift)
        ;

}
#endif

#if (defined(ENABLE_LINUX_MTD) && (ENABLE_LINUX_MTD==1))

U8 MDrv_NAND_Set_Chip_Select(U8 u8Level)
{
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    if(u8Level)
    {
        _fsinfo.u16NC_CFG0 &= ~R_NC_CE_EN;
    }
    else
    {
        _fsinfo.u16NC_CFG0 |= R_NC_CE_EN;
    }

    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    return(1);
}

U8 MDrv_NAND_Set_Command_Latch(U8 u8Level)
{
    if(!u8Level)
        _fsinfo.ehw_ctl = HW_CLE;
    else
        _fsinfo.ehw_ctl= 0;
    return(1);
}

U8 MDrv_NAND_Set_Address_Latch(U8 u8Level)
{
    if(!u8Level)
    {
        _fsinfo.ehw_ctl = HW_ALE;
        _fsinfo.u8AddressCnt = 0;   // reset address bytes count
    }
    else
    {
        _fsinfo.ehw_ctl= 0;
    }
    return(1);
}

U8 MDrv_NAND_Set_Write_Protect(U8 u8Level)
{
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    if(u8Level)
        _fsinfo.u16NC_CFG0 &= ~(R_NC_WP_EN);
    else
        _fsinfo.u16NC_CFG0 |= (R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    return(1);
}

U8 MDrv_NAND_WaitReady(void)
{
    drvNAND_SWITCH_PAD(0);
    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8) + OP_A_WAIT_READY_BUSY);

    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    drvNAND_wNCJOBEND(false);
    drvNAND_Release_PAD();
    return(1);
}

U8 MDrv_NAND_Send_Read_Status_CMD(U8 u8Cmd)
{
    u8CIF_C_Index = 0;
    DBG_NAND(printk("MDrv_NAND_Send_Read_Status_CMD\r\n"));
    drvNAND_ClearNFIE_EVENT();
    drvNAND_SWITCH_PAD(0);
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_USERCMD);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)u8Cmd<<8) + u8Cmd);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_CHECK_STATUS<<8) + OP_CMD_REG);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_BREAK);

    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    drvNAND_wNCJOBEND(false);
    drvNAND_Release_PAD();
    DBG_NAND(printk("status=0x%X\r\n",(U16)REG(NC_ST_READ)));
    drvNAND_SetCMD_RSP_BUF(0, (U8)REG(NC_ST_READ));
    return(1);
}

U8 MDrv_NAND_WriteByte(U8 u8Byte)
{
    if(_fsinfo.ehw_ctl == HW_CLE)
    {

    }
    return(true);
}

U8 MDrv_NAND_ReadByte(void)
{
    //DBG_NAND(printk("Read Byte=0x%X\r\n",drvNAND_GetCMD_RSP_BUF(u8CIF_C_Index)));
    return( drvNAND_GetCMD_RSP_BUF(u8CIF_C_Index++));
}

#if 0
NCRW_STATUS MDrv_NAND_ReadPage(U32 u32Row,U16 u16Column)
{
    u16ByteIdxofPage = 0;
    HAL_DCACHE_FLUSH((U32)_stPageBuffer, (U32)sizeof(_stPageBuffer));
    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
        MDrv_NAND_ReadPHYPage( u32Row, _stPageBuffer,0x840);
    else
        MDrv_NAND_ReadPHYPage( u32Row, _stPageBuffer,0x210);

    return(NCRW_STS_OK);
}
#endif

#define DISABLE_ALIGNMENT_CHECK		0 // for MSTAR only

void MDrv_NAND_ReadBuf(U8 * const u8Buf,U16 u16Len)
{
    #if (defined(DISABLE_ALIGNMENT_CHECK) && (DISABLE_ALIGNMENT_CHECK==1))
    U32 u32DMAAddr;// = (U32)VA2PA(u8Buf)/MIUWIDTH;
    #else
    U32 u32DMAAddr;// = (U32)VA2PA(u8Buf)/MIUWIDTH;
    U8 *pu8Buf;
    #endif
    //NCRW_STATUS eStatus;
    U16 u16Tmp;
    
#ifdef NAND_DMA_RACING_PATCH
    U32 *u32pBuf = 0;
#endif

    DBG_NAND(printk("MDrv_NAND_ReadBuf 0x%X, buf=0x%lX\r\n",u16Len,(U32)u8Buf));
    drvNAND_ClearNFIE_EVENT();
    if((u16Len==0x200)||(u16Len==0x210)||(u16Len==0x800)||(u16Len==0x840))
    {
        u32DMAAddr = (U32)VA2PA(u8Buf)/MIUWIDTH;
        u16ByteIdxofPage = u16Len;
        // DMA must be WIUWIDTH aligned, For titania, MIUWIDTH is 8 bytes.
        #if (defined(DISABLE_ALIGNMENT_CHECK) && (DISABLE_ALIGNMENT_CHECK==1))
        //HAL_DCACHE_FLUSH((U32)u8Buf, u16Len);
        _dma_cache_inv((U32)u8Buf, u16Len);
        __asm(
                "sync;"
                "nop;"
        );
        #else
        if(((U32)u8Buf & (MIUWIDTH-1))==0)
        {
            if( ((unsigned int)u8Buf) >= 0xC0000000 ) // To avoid the DMA using the non-continous memory
                 pu8Buf = _stPageBuffer;
             else
                 pu8Buf = u8Buf;
        }
        else
        {
            pu8Buf = _stPageBuffer;
        }
        
#ifdef NAND_DMA_RACING_PATCH
    	if( u16Len == 0x200 || u16Len == 0x210 )
    	{
    		u32pBuf = (U32*)(pu8Buf+508);
    		*u32pBuf = NAND_READ_MAGIC;
    	}
    	else if( u16Len == 0x800 || u16Len == 0x840 )
    	{
    		u32pBuf = (U32*)(pu8Buf+2044);
    		*u32pBuf = NAND_READ_MAGIC;
    	}
#endif

        _dma_cache_wback_inv((U32)pu8Buf, u16Len);
        __asm(
                "sync;"
                "nop;"
        );
        u32DMAAddr = (U32)VA2PA(pu8Buf)/MIUWIDTH;
        #endif

        if(u16Len<=0x210)
            HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
        else
            HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);

        HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAAddr>>16);
        HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
        HAL_WRITE_UINT16(NC_MMA_PRI_REG,0);
        HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

        if(u16Len<=0x210)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_SERIAL_DATA_IN<<8) + OP_A_WAIT_READY_BUSY);
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)(OP_A_BREAK)<<8)+OP_A_BREAK);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_SERIAL_DATA_IN<<8) + OP_A_WAIT_READY_BUSY);
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8) + OP_A_BREAK);
        }
        //HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
        //HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
        HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);

        u16Tmp = drvNAND_wNCJOBEND(true);

#ifdef NAND_DMA_RACING_PATCH
		if( u16Len >= 0x200 )
		{
			int tmcnt = 0;
			//DBG_NAND(printf("*u32pBuf=%X\n", *u32pBuf));
			while( *u32pBuf == NAND_READ_MAGIC )
			{
				if((tmcnt++)>0x3fffff)
				{
					printk(KERN_WARNING "Timeout!!!!\n");
					break;
				}
			}
		}
#endif
            if(u16Tmp==NCRW_STS_ECC_ERROR)
            {
			// Check ECC code by SW, our HW generate the ECC code by (512+6) bytes
			if((drvNAND_GetCMD_RSP_BUF(6)==0xFF) && (drvNAND_GetCMD_RSP_BUF(7)==0xFF) &&
               (drvNAND_GetCMD_RSP_BUF(8)==0xFF) && (drvNAND_GetCMD_RSP_BUF(9)==0xFF) &&
               (drvNAND_GetCMD_RSP_BUF(10)==0xFF) && (drvNAND_GetCMD_RSP_BUF(11)==0xFF) &&
               (drvNAND_GetCMD_RSP_BUF(12)==0xFF) && (drvNAND_GetCMD_RSP_BUF(13)==0xFF) &&
               (drvNAND_GetCMD_RSP_BUF(14)==0xFF) && (drvNAND_GetCMD_RSP_BUF(15)==0xFF) )
		    {
		        gint_IS_ECC_Fail = 0;
    	    }
            else
    		{
                printk("\r\n\r\n\r\n\r\nECC Error\r\n");
                {
                        U16 i;
                            printk("============================================\r\n");
                            for(i=0;i<0x200;i++)
                            {
                                printk("%2X ",pu8Buf[i]);
                                if((i&0xF)==0xF)
                                    printk("\r\n");
                            }
                            printk("============================================\r\n");
                            for(i=0;i<0x10;i++)
                                printk("%2X ",drvNAND_GetCMD_RSP_BUF(i));
                            printk("\r\n============================================\r\n");
                        }

                    gint_IS_ECC_Fail = -1;
                }
		}
        else if (u16Tmp == NCRW_STS_BIT_FLIP)
        {
            gint_IS_ECC_Fail = 1;
        }
        else
        {
        #if 0
            U16 u16PageSize;
            int i, j;
        
            if ((u16Len = 0x200) || (u16Len = 0x210))
            {
                u16PageSize = 0x200;
                i = 0;
            }
            if ((u16Len = 0x800) || (u16Len = 0x840))
            {
                u16PageSize = 0x800;
                i = 3;
            }
            for (; i >= 0; i--)
            {
                for (j = 15; j >= 6; j--)
                {
                    if (drvNAND_GetCMD_RSP_BUF(i * 16 + j) != 0xFF)
                    {
                        break;
                    }
                }
                if (j >= 6)
                {
                    break;
                }
            }
            if ((i < 0) && (j < 6))  //Maybe an empty page
            {
	         gint_IS_ECC_Fail = 0;

                for (i = u16PageSize - 1; i >= 0; i--)
                {
                    if (pu8Buf[i] != 0xff)
                    {
                        // the page is not really empty
                        printk("0xff error detect, i=%d,data=0x%x\r\n", i, pu8Buf[i]);
                        pu8Buf[i] = 0xff;
                        gint_IS_ECC_Fail = 1;
                    }
                }
	     }
            else
        #endif
            {
                gint_IS_ECC_Fail = 0;
            }
        }
        #if (defined(DISABLE_ALIGNMENT_CHECK) && (DISABLE_ALIGNMENT_CHECK==1))
        if ( u16Len == 0x210)
        {
            for(u32DMAAddr=0;u32DMAAddr<8;u32DMAAddr++)
            {
                u16Tmp = (U16)_pu32CIF_C[u32DMAAddr];
                u8Buf[0x200+(u32DMAAddr<<1)] = (U8)u16Tmp;
                u8Buf[0x201+(u32DMAAddr<<1)] = (U8)(u16Tmp>>8);
                //u8Buf[0x200+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
            }
        }
        else if(( u16Len == 0x840))
		{
			for(u32DMAAddr=0;u32DMAAddr<16;u32DMAAddr++)
                {
                    u8Buf[0x800+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
                    u8Buf[0x800+u32DMAAddr+0x10] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x10);
                    u8Buf[0x800+u32DMAAddr+0x20] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x20);
                    u8Buf[0x800+u32DMAAddr+0x30] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x30);
                }
        }
        #else
        if( u16Len == 0x840 || u16Len == 0x800 )
        {
            for(u32DMAAddr=0;u32DMAAddr<16;u32DMAAddr++)
            {
                _stPageBuffer[0x800+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
                _stPageBuffer[0x800+u32DMAAddr+0x10] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x10);
                _stPageBuffer[0x800+u32DMAAddr+0x20] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x20);
                _stPageBuffer[0x800+u32DMAAddr+0x30] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr+0x30);

            }
        }
        else //0x210 or 0x200 case
        {
            for(u32DMAAddr=0;u32DMAAddr<16;u32DMAAddr++)
            {
                _stPageBuffer[0x200+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
            }
        }

        if((U32)u8Buf != (U32)pu8Buf)
        {
            memcpy(u8Buf,_stPageBuffer,u16Len);
        }
        else
        {
            if(u16Len==0x840)
                memcpy(&u8Buf[0x800],&(_stPageBuffer[0x800]),0x40);
            else if(u16Len==0x210)
                memcpy(&u8Buf[0x200],&(_stPageBuffer[0x200]),0x10);
        }
        #endif
    }
    else
    {
        #if (defined(DISABLE_ALIGNMENT_CHECK) && (DISABLE_ALIGNMENT_CHECK==1))
        if(u16Len==0x40)
        {
            for(u32DMAAddr=0;u32DMAAddr<(u16Len/2);u32DMAAddr++)
            {
                u8Buf[u32DMAAddr] = _pu32CIF_C[u32DMAAddr]&0xFF;
                u8Buf[u32DMAAddr+1] = (_pu32CIF_C[u32DMAAddr]>>8);
                u8Buf[u32DMAAddr+0x10] = _pu32CIF_C[u32DMAAddr+0x10]&0xFF;
                u8Buf[u32DMAAddr+0x10+1] = _pu32CIF_C[u32DMAAddr+0x10]>>8;
                u8Buf[u32DMAAddr+0x20] = _pu32CIF_C[u32DMAAddr+0x20]&0xFF;
                u8Buf[u32DMAAddr+0x20+1] = _pu32CIF_C[u32DMAAddr+0x20]>>8;
                u8Buf[u32DMAAddr+0x30] = _pu32CIF_C[u32DMAAddr+0x30]&0xFF;
                u8Buf[u32DMAAddr+0x30+1] = _pu32CIF_C[u32DMAAddr+0x30]>>8;
			}
            }
            else
            {
            for(u32DMAAddr=0;u32DMAAddr<u16Len;u32DMAAddr+=2)
            {
                u16Tmp = (U16)_pu32CIF_C[u32DMAAddr>>1];
                u8Buf[(u32DMAAddr)] = (U8)u16Tmp;
                u8Buf[(u32DMAAddr)+1] = (U8)(u16Tmp>>8);
            }
        }
        #else
        memcpy(u8Buf,&_stPageBuffer[u16ByteIdxofPage],u16Len);
        #endif
    }

    //printf("end\r\n");
}

void MDrv_NAND_ReadOOB(U32 u32Row,U16 u16Column)
{
    U8 u8Tmp;

    u16ByteIdxofPage = 0;   // for readbuf
    u8CIF_C_Index = u16Column;      // for readbyte

    drvNAND_SWITCH_PAD(0);
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    if( _fsinfo.eFlashConfig & FLASH_2KPAGE )
    {
        drvNAND_ReadRdd(u32Row,0x200);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
        drvNAND_ReadRdd(u32Row,0x410);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[0x10+u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[0x10+u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
        drvNAND_ReadRdd(u32Row,0x620);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[0x20+u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[0x20+u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
        drvNAND_ReadRdd(u32Row,0x830);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[0x30+u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[0x30+u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
    }
    else
    {
        drvNAND_ReadRdd(u32Row,0x200);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
    }
/*
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    if( _fsinfo.eFlashConfig & FLASH_2KPAGE )
    {
        if(_fsinfo.eFlashConfig & FLASH_16BIT)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, u16Column>>1);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, u16Column);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C5TFS0<<8) + OP_CMD00_READ);
        }
        else if( _fsinfo.u8AddrWidth == 4)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS0<<8) + OP_CMD00_READ);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS0<<8) + OP_CMD00_READ);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_WAIT_READY_BUSY<<8) + OP_CMD30_READ);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_BREAK<<8) + OP_A_RANDOM_DATA_IN);
        u8RDDLen = 0x40;
        for(u8Tmp=0;u8Tmp<u8RDDLen;u8Tmp++)
        {
            _stPageBuffer[u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
        HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
        if( _fsinfo.u8AddrWidth == 4)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS0<<8)+OP_CMD50_READ_RDD);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS0<<8)+OP_CMD50_READ_RDD);
        }
        HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_RANDOM_DATA_IN<<8)+OP_A_WAIT_READY_BUSY);
        HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_BREAK<<8) + OP_A_BREAK);
        u8RDDLen = 0x10;
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_RAN_CNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u8RDDLen);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);

    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(false);
    for(u8Tmp=0;u8Tmp<u8RDDLen;u8Tmp++)
    {
        _stPageBuffer[u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
    }


    }

*/
#if 0
	_fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
	HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
	drvNAND_Release_PAD();
#endif
}

static U32 _tWriteLen;

NCRW_STATUS MDrv_NAND_WritePage(U32 u32Row,U16 u16Column)
{
    u16ByteIdxofPage = u16Column;
    _tWriteLen = 0;
#if 0
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

    drvNAND_WritePage(u32Row,u16Column);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_BREAK);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    drvNAND_wNCJOBEND(false);
#endif

    _fsinfo.u32PreSource[0] = u32Row;
    _fsinfo.u32PreSource[1] = u16Column;

    return(NCRW_STS_OK);
}

extern void * memcpy(void * dest,const void *src,size_t count);

void MDrv_NAND_WriteBuf(U8 * const u8Buf,U16 u16Len)
{
    DBG_NAND(printk("MDrv_NAND_WriteBuf 0x%X,u16ByteIdxofPage=0x%X\r\n",u16Len,u16ByteIdxofPage));
    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        if((_tWriteLen==0) && (u16ByteIdxofPage>=0x800))
        {
            memset(_stPageBuffer,0xFF,0x840);
        }
    }
    else
    {
    if((_tWriteLen==0) && (u16ByteIdxofPage>=0x200))
    {
        memset(_stPageBuffer,0xFF,0x210);
    }
    }
    memcpy(&(_stPageBuffer[u16ByteIdxofPage]),u8Buf,u16Len);
    u16ByteIdxofPage += u16Len;
    _tWriteLen  += u16Len;
}

void MDrv_NAND_Cmd_PageProgram(U8 u8Cmd)
{
    NCRW_STATUS eStatus;
    U32 u32DMAAddr;
    U16 u16PageSize,u16Cnt;
    U8 u8Tmp,u8Idx;
    bool ECC_ENABLE = true;

	drvNAND_SWITCH_PAD(0);
    drvNAND_ClearNFIE_EVENT();
    DBG_NAND(printk("MDrv_NAND_Cmd_PageProgram,Len:0x%lX\r\n",_tWriteLen));

    u32DMAAddr = ((U32)VA2PA(_stPageBuffer))/MIUWIDTH;
    DBG_NAND(printk("_stPageBuffer=0x%lX\r\n",(U32)_stPageBuffer));
    DBG_NAND(printk("u32DMAAddr=0x%lX\r\n",u32DMAAddr));

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE )
    {
        u8Tmp = 6;
        u16PageSize = 0x800;
        if( (_stPageBuffer[u16PageSize+1] == 0xFF) &&
            (_stPageBuffer[u16PageSize+2] == 0xFF) &&
            (_stPageBuffer[u16PageSize+3] == 0xFF) &&
            (_stPageBuffer[u16PageSize+4] == 0xFF) &&
            (_stPageBuffer[u16PageSize+5] == 0xFF) && (_tWriteLen==0x840))
        {
            drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x200);
            for(u8Idx=0;u8Idx<5;u8Idx++)
	{
                _stPageBuffer[u16PageSize+u8Idx] = drvNAND_GetCMD_RSP_BUF(u8Idx);
                //printk("_stPageBuffer[%4X]=%2X\r\n",(0x200+u8Idx),_stPageBuffer[0x200+u8Idx]);
	}
            drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x410);
            for(u8Idx=0;u8Idx<5;u8Idx++)
	{
                _stPageBuffer[u16PageSize+u8Idx+0x10] = drvNAND_GetCMD_RSP_BUF(u8Idx);
                //printk("_stPageBuffer[%4X]=%2X\r\n",(0x200+u8Idx),_stPageBuffer[0x200+u8Idx]);
	}
            drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x620);
            for(u8Idx=0;u8Idx<5;u8Idx++)
    {
                _stPageBuffer[u16PageSize+u8Idx+0x20] = drvNAND_GetCMD_RSP_BUF(u8Idx);
                //printk("_stPageBuffer[%4X]=%2X\r\n",(0x200+u8Idx),_stPageBuffer[0x200+u8Idx]);
            }
            drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x830);
            for(u8Idx=0;u8Idx<5;u8Idx++)
        {
                _stPageBuffer[u16PageSize+u8Idx+0x30] = drvNAND_GetCMD_RSP_BUF(u8Idx);
                //printk("_stPageBuffer[%4X]=%2X\r\n",(0x200+u8Idx),_stPageBuffer[0x200+u8Idx]);
            }
            #if 0
            for(u16Cnt=0;u16Cnt<0x840;u16Cnt++)
            {
                printk("%2X ",_stPageBuffer[u16Cnt]);
                if( (u16Cnt&0xF)==0xF)
                    printk("\r\n");
            }
            #endif
        }
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);
    }
    else
    {
        u8Tmp = 6;
        u16PageSize = 0x200;
        if( (_stPageBuffer[u16PageSize+0] == 0xFF) &&
            (_stPageBuffer[u16PageSize+1] == 0xFF) &&
            (_stPageBuffer[u16PageSize+2] == 0xFF) &&
            (_stPageBuffer[u16PageSize+3] == 0xFF) &&
            (_stPageBuffer[u16PageSize+4] == 0xFF) && (_tWriteLen==0x210))
        {
            drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x200);
            drvNAND_FLASHRESET();
            for(u8Idx=0;u8Idx<5;u8Idx++)
            {
                _stPageBuffer[u16PageSize+u8Idx] = drvNAND_GetCMD_RSP_BUF(u8Idx);
                //printk("_stPageBuffer[%4X]=%2X\r\n",(0x200+u8Idx),_stPageBuffer[0x200+u8Idx]);
            }
        }
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
    }

    // Patch Code for write JFFS2 image
    //==================================================================
    {
        u16Cnt = 0;

        while(_stPageBuffer[u16Cnt++]==0xFF)
        {
        	#if 0
            //for Clean Marker case
            if((u16Cnt==u16PageSize) &&
                  (
                    _stPageBuffer[u16Cnt+0]==0x85 &&
                    _stPageBuffer[u16Cnt+1]==0x19 &&
                    _stPageBuffer[u16Cnt+2]==0x03 &&
                    _stPageBuffer[u16Cnt+3]==0x20 &&
                    _stPageBuffer[u16Cnt+4]==0x08
                  )
             )
            {
                ECC_ENABLE = false;
                break;
            }
            //for all 0xFF case
            else if(u16Cnt==(u8Tmp+u16PageSize))
            {
                ECC_ENABLE = false;
                break;
            }
            #else
            if(u16Cnt==(u8Tmp+u16PageSize))
                return;
            #endif
        }
    }


    u8Idx = 0;
    for(u8Idx=0;u8Idx<u8Tmp;u8Idx++)
    {
        drvNAND_SetCMD_RSP_BUF(u8Idx,_stPageBuffer[u16PageSize+u8Idx]);
    }
    //printk("\r\n");
    _dma_cache_wback_inv((U32)_stPageBuffer, (U32)sizeof(_stPageBuffer));
    __asm(
                "sync;"
                "nop;"
        );
    HAL_WRITE_UINT16(NC_MIU_DMA1,u32DMAAddr>>16);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR); //write
    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_USERCMD);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)u8Cmd<<8) + u8Cmd);

    drvNAND_WritePage(_fsinfo.u32PreSource[0],0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(((U16)OP_CMD_REG)<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(((U16)OP_A_WAIT_READY_BUSY)<<8)+OP_A_WAIT_IDLE);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(((U16)OP_A_CHECK_STATUS)<<8)+OP_CMD70_READ_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(((U16)OP_A_BREAK)<<8)+OP_A_BREAK);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);

    if(((_tWriteLen != 0x210) && (_tWriteLen != 0x840)) || (ECC_ENABLE==false))
        HAL_WRITE_UINT16(NC_CTRL,R_NC_ECC_BYPASS+R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    else
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(true);

    if(eStatus==NCRW_STS_OK)
    {
        DBG_NAND(printk("MDrv_NAND_Cmd_PageProgram PASS\r\n"));
    }
    else
    {
        DBG_NAND(printk("MDrv_NAND_Cmd_PageProgram false\r\n"));
    }
    drvNAND_Release_PAD();
    //============================================================================

}

NCRW_STATUS MDrv_NAND_SendCmd(U8 u8Cmd)
{
    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_USERCMD);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)u8Cmd<<8) + u8Cmd);
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_CMD_REG<<8) + OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8) + OP_A_BREAK);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    return(drvNAND_wNCJOBEND(false));
}

NCRW_STATUS MDrv_NAND_SendAdr(U32 u32Row,U16 u16Column)
{
    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u16Column);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
    HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

    if( _fsinfo.u8AddrWidth == 5)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8) + OP_ADR_C5TFS0);
    }
    else if( _fsinfo.u8AddrWidth == 4)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8) + OP_ADR_C4TFS0);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8) + OP_ADR_C3TFS0);
    }
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    return(drvNAND_wNCJOBEND(false));
}

int MDrv_NAND_CheckECC(void)
{
    return(gint_IS_ECC_Fail);
}

#if 0
void MDrv_NAND_SendCmdAdr(U8 u8Cmd,U32 u32Row,U16 u16Column)
{

    if(MDrv_NAND_SendCmd(u8Cmd)!=NCRW_STS_OK)
        DBG_NAND(printk("Send CMD fail\r\n"));
    if(MDrv_NAND_SendAdr(u32Row,u16Column)!=NCRW_STS_OK)
        DBG_NAND(printk("Send ADR fail\r\n"));

}
#endif

#endif
#endif // #if ENABLE_NAND

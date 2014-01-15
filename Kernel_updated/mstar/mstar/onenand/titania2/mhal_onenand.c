////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/kernel.h>

#include "mdrv_types.h"
#include "mhal_onenand.h"
#include "mhal_onenand_reg.h"

#ifdef __cplusplus
extern "C"
{
#endif


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
typedef enum
{
    PARFLASH_DBGLV_NONE,    ///< disable all the debug message
    PARFLASH_DBGLV_INFO,    ///< information
    PARFLASH_DBGLV_NOTICE,  ///< normal but significant condition
    PARFLASH_DBGLV_WARNING, ///< warning conditions
    PARFLASH_DBGLV_ERR,     ///< error conditions
    PARFLASH_DBGLV_CRIT,    ///< critical conditions
    PARFLASH_DBGLV_ALERT,   ///< action must be taken immediately
    PARFLASH_DBGLV_EMERG,   ///< system is unusable
    PARFLASH_DBGLV_DEBUG,   ///< debug-level messages
} ParFlash_DbgLv;


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static U8 _u8ParFlashDbgLv;


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static void _HAL_ParFlash_Delay(void);
static BOOL _HAL_ParFlash_WaitReady(void);


static void _HAL_ParFlash_Delay(void)
{
    int i;

    for (i = 0; i < 10; i ++)
    {
        asm volatile
        (
            "nop;"
        );
    }
}


static BOOL _HAL_ParFlash_WaitReady(void)
{
    U16 u16data;
    U32 u32timeout;

    u16data = 0;
    u32timeout = 0;

    while (u16data == 0)
    {
        _HAL_ParFlash_Delay();

        u16data = (PFSH_READ(REG_NORPF_XIU_STATUS) & NORPF_XIU_STATUS_MASK);
        u32timeout++ ;

        if (u32timeout > 0x10000)
        {
            DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("_HAL_ParFlash_WaitReady(): TIMEOUT\n"));
            return FALSE ; // timeout
        }
    }

    return TRUE ; // Success
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

void HAL_ParFlash_Read(U32 u32Addr, U16 *u16Data)
{
    u32Addr >>= 1;

    PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode

    PFSH_WRITE(REG_NORPF_XIU_ADDR_L, u32Addr & 0xFFFF);
    PFSH_WRITE(REG_NORPF_XIU_ADDR_H, u32Addr >> 16);

    PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x0C);
    PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x0D);

    _HAL_ParFlash_WaitReady();

    *u16Data = (U16)(PFSH_READ(REG_NORPF_XIU_RDATA_L) & 0xFFFF);

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Read(0x%08X) = 0x%04X\n", (int)u32Addr, *u16Data));
}


void HAL_ParFlash_Reads(U32 u32Addr, U8 *pu8Data, U32 u32Size)
{
    U32 u32Loop ;
    U16 *pu16data ;

    u32Addr >>= 1;

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Reads(0x%08X, %p, %d)\n", (int)u32Addr, pu8Data, (int)u32Size));

    pu16data = (U16 *)(void *)pu8Data ;  // TODO: review, pu8Data is 2-B aligned

    PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode

    for (u32Loop = 0; u32Loop < (u32Size + 1) / 2; u32Loop++)
    {
        PFSH_WRITE(REG_NORPF_XIU_ADDR_L, (u32Addr+u32Loop) & 0xFFFF);
        PFSH_WRITE(REG_NORPF_XIU_ADDR_H, (u32Addr+u32Loop) >> 16);

        PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x0C);
        PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x0D);

        _HAL_ParFlash_WaitReady();

        *(pu16data+u32Loop) = (U16)(PFSH_READ(REG_NORPF_XIU_RDATA_L) & 0xFFFF) ;
    }
}


void HAL_ParFlash_Write(U32 u32Addr, U16 u16Data)
{
    u32Addr >>= 1;

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Write(0x%08X, 0x%04X)\n", (int)u32Addr, u16Data));

    PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode

    PFSH_WRITE(REG_NORPF_XIU_ADDR_L, u32Addr & 0xFFFF);
    PFSH_WRITE(REG_NORPF_XIU_ADDR_H, u32Addr >> 16);
    PFSH_WRITE(REG_NORPF_XIU_WDATA, u16Data);

    PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x0A);
    PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x0B);

    _HAL_ParFlash_WaitReady();
}


void HAL_ParFlash_Writes(U32 u32Addr, const U8 *pu8Data, U32 u32Size)
{
    U32 u32Loop ;
    U16 *pu16data ;

    u32Addr >>= 1;

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Writes(0x%08X, %p, %d)\n", (int)u32Addr, pu8Data, (int)u32Size));

    pu16data = (U16 *)(void *)pu8Data ;  // TODO: review, pu8Data is 2-B aligned

    PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode

    for (u32Loop = 0; u32Loop < (u32Size + 1) / 2; u32Loop++)
    {
        PFSH_WRITE(REG_NORPF_XIU_ADDR_L, (u32Addr+u32Loop) & 0xFFFF);
        PFSH_WRITE(REG_NORPF_XIU_ADDR_H, (u32Addr+u32Loop) >> 16);
        PFSH_WRITE(REG_NORPF_XIU_WDATA, *(pu16data+u32Loop));

        PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x0A);
        PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x0B);

        _HAL_ParFlash_WaitReady();
    }
}


void HAL_ParFlash_Init(void)
{
    _u8ParFlashDbgLv = PARFLASH_DBGLV_INFO;

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Init()\n"));

    WRITE_WORD_MASK(0xBF800000 + (0x1EDE << 1), 0, BIT4); // TODO: review, check chip setup in Linux kernel

    WRITE_WORD_MASK(0xBF800000 + (0x1EDC << 1), 0, BIT1 | BIT0);                    // 0x1EDC

    PIU_WRITE_MASK(REG_PIU_SPI_CLK_SRC, PSCS_CLK_SRC_SEL_XTAL,  PSCS_CLK_SRC_SEL_MASK);
    PIU_WRITE_MASK(REG_PIU_SPI_CLK_SRC, PSCS_CLK_SEL_108MHZ,    PSCS_CLK_SEL_MASK);
    PIU_WRITE_MASK(REG_PIU_SPI_CLK_SRC, PSCS_CLK_SRC_SEL_CLK,   PSCS_CLK_SRC_SEL_MASK);

    PFSH_WRITE_MASK(REG_NORPF_CLOCK_LENGTH, 0xA, BMASK(7:0));
    PFSH_WRITE(REG_NORPF_CEB_START, 0x3040);
    PFSH_WRITE(REG_NORPF_WEB_START, 0x4043);
    PFSH_WRITE_MASK(REG_NORPF_DATA_LATCH_CNT, 0x04, BMASK(7:0));
    PFSH_WRITE_MASK(REG_NORPF_BRIDGE_CTRL, 0x01, BMASK(7:0)); // 0x0A
}


#ifdef __cplusplus
}
#endif

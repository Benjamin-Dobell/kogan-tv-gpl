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
#include <linux/module.h>

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
#define ONENAND_BOUNCE_BUF 0
#define USELOCK    1

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

static BOOL _HAL_ParFlash_WaitReady(void);

extern void Chip_Flush_Memory(void);

static void Chip_Read_Memory(void)
{
	volatile unsigned int *pu8;
	volatile unsigned int t;

	pu8 = ((volatile unsigned int *)0xA0380000);
	t = pu8[0];
	t = pu8[64];
}

#if 0
static void _HAL_ParFlash_Delay(void)
{
    int i;

    for (i = 0; i < 10; i ++) // TODO: review, fine tune
    {
        asm volatile
        (
            "nop;"
        );
    }
}
#endif

static BOOL _HAL_ParFlash_WaitReady(void)
{
    U16 u16data;
    U32 u32timeout;

    u16data = 0;
    u32timeout = 0;

    while (u16data == 0)
    {
        //_HAL_ParFlash_Delay();

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
#if  USELOCK
    unsigned long flags ;
    local_irq_save(flags);
    local_irq_disable() ;
#endif
    u32Addr >>= 1;

    // PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode // <-@@@, enhanced access mode

    PFSH_WRITE(REG_NORPF_XIU_ADDR_L, u32Addr & 0xFFFF);
    PFSH_WRITE(REG_NORPF_XIU_ADDR_H, u32Addr >> 16);

    PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x04); // 0x0C // <-@@@, enhanced access mode
    //_HAL_ParFlash_Delay(); // TODO: review, fine tune
    PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x05); // 0x0D // <-@@@, enhanced access mode
    //_HAL_ParFlash_Delay(); // TODO: review, fine tune

    _HAL_ParFlash_WaitReady();

    *u16Data = (U16)(PFSH_READ(REG_NORPF_XIU_RDATA_L) & 0xFFFF);

    // PFSH_WRITE_MASK(REG_NORPF_XIU_CTRL, 0x00, 0x08);         // <-@@@, enhanced access mode
    // PFSH_WRITE_MASK(REG_NORPF_WRITE_RUN, 0x00, BMASK(5:4));  // <-@@@, enhanced access mode

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Read(0x%08X) = 0x%04X\n", (int)u32Addr, *u16Data));
#if  USELOCK
    local_irq_restore(flags);
#endif
}

typedef enum _BDMA_Result
{
    E_BDMA_NOT_SUPPORT = -1
    ,E_BDMA_FAIL = 0
    ,E_BDMA_OK = 1
    ,E_BDMA_TIMEOUT
    ,E_BDMA_QUEUE_FULL
    ,E_BDMA_BUSY
}BDMA_Result;

extern BDMA_Result _BDMA_START(unsigned int src, unsigned int dst, unsigned int len);

#if defined(ONENAND_BOUNCE_BUF) && ONENAND_BOUNCE_BUF
static unsigned char u8BounceBuffer[4096];
#endif

void HAL_ParFlash_Reads(U32 u32Addr, U8 *pu8Data, U32 u32Size)
{
    U32 u32Loop ;
    U16 *pu16data ;
	U32 addr = REG_NORPF_BASE;
	U32 addr1 = addr + ((REG_NORPF_XIU_ADDR_L)<<2);
	U32 addr2 = addr + ((REG_NORPF_XIU_ADDR_H)<<2);
	U32 addr3 = addr + ((REG_NORPF_XIU_CTRL)<<2);
	U32 addr4 = addr + ((REG_NORPF_XIU_RDATA_L)<<2);

#if  USELOCK
    unsigned long flags ;
    local_irq_save(flags);
    local_irq_disable() ;

#endif
    u32Addr >>= 1;

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Reads(0x%08X, %p, %d)\n", (int)u32Addr, pu8Data, (int)u32Size));

    pu16data = (U16 *)(void *)pu8Data ;  // TODO: review, pu8Data is 2-B aligned

    // PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode // <-@@@, enhanced access mode

    if( u32Size >= 16 )
	{
		*(volatile unsigned short *)(0xBF200B00+(0x08<<2)) &= (~0x08);
		*(volatile unsigned short *)(0xBF200B00+(0x01<<2)) &= (~0x30);

		#if defined(ONENAND_BOUNCE_BUF) && ONENAND_BOUNCE_BUF
		_dma_cache_wback_inv((U32)u8BounceBuffer, u32Size);
		Chip_Flush_Memory();

		_BDMA_START(u32Addr<<1, (unsigned int)u8BounceBuffer, u32Size);

		Chip_Read_Memory();

		memcpy(pu8Data, u8BounceBuffer, u32Size);
		#else
		_dma_cache_wback_inv((U32)pu8Data, u32Size);
		Chip_Flush_Memory();

		_BDMA_START(u32Addr<<1, (unsigned int)pu8Data, u32Size);

		Chip_Read_Memory();
		#endif
    }
    else
    {
        int c ;
        c = 0 ;
    	for (u32Loop = 0; u32Loop < (u32Size + 1) / 2; u32Loop++)
    	{
            unsigned short tt ;
    		#if 1
			WRITE_WORD(addr1, (u32Addr+u32Loop) & 0xFFFF);
			WRITE_WORD(addr2, (u32Addr+u32Loop) >> 16);

			WRITE_WORD(addr3, 0x04); // 0x0C // <-@@@, enhanced access mode
			WRITE_WORD(addr3, 0x05); // 0x0D // <-@@@, enhanced access mode

			_HAL_ParFlash_WaitReady();

            tt = (READ_WORD(addr4) & 0xFFFF) ;
			//*(pu16data+u32Loop) = tt ;
            *(pu8Data+c) = (tt&0xFF) ;
            c++ ;
            if( c<u32Size ){
                *(pu8Data+c) = (tt>>8) ;
                c++ ;
            }
			#else
			PFSH_WRITE(REG_NORPF_XIU_ADDR_L, (u32Addr+u32Loop) & 0xFFFF);
			PFSH_WRITE(REG_NORPF_XIU_ADDR_H, (u32Addr+u32Loop) >> 16);

			// PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x04); // 0x0C // <-@@@, enhanced access mode
			//_HAL_ParFlash_Delay(); // TODO: review, fine tune
			// PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x05); // 0x0D // <-@@@, enhanced access mode
			//_HAL_ParFlash_Delay(); // TODO: review, fine tune

			_HAL_ParFlash_WaitReady();

			*(pu16data+u32Loop) = (U16)(PFSH_READ(REG_NORPF_XIU_RDATA_L) & 0xFFFF) ;
			#endif
		}
#if 1
    }
#endif

    // PFSH_WRITE_MASK(REG_NORPF_XIU_CTRL, 0x00, 0x08);         // <-@@@, enhanced access mode
    // PFSH_WRITE_MASK(REG_NORPF_WRITE_RUN, 0x00, BMASK(5:4));  // <-@@@, enhanced access mode
#if  USELOCK
                local_irq_restore(flags);

#endif
}

EXPORT_SYMBOL(HAL_ParFlash_Reads);

void HAL_ParFlash_Write(U32 u32Addr, U16 u16Data)
{
#if USELOCK
    unsigned long flags ;
    local_irq_save(flags);
    local_irq_disable() ;
#endif
    u32Addr >>= 1;

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Write(0x%08X, 0x%04X)\n", (int)u32Addr, u16Data));

    // PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode // <-@@@, enhanced access mode

    PFSH_WRITE(REG_NORPF_XIU_ADDR_L, u32Addr & 0xFFFF);
    PFSH_WRITE(REG_NORPF_XIU_ADDR_H, u32Addr >> 16);
    PFSH_WRITE(REG_NORPF_XIU_WDATA, u16Data);

    PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x02); // 0x0A // <-@@@, enhanced access mode
    //_HAL_ParFlash_Delay(); // TODO: review, fine tune
    PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x03); // 0x0B // <-@@@, enhanced access mode
    //_HAL_ParFlash_Delay(); // TODO: review, fine tune

    //_HAL_ParFlash_WaitReady();

    // PFSH_WRITE_MASK(REG_NORPF_XIU_CTRL, 0x00, 0x08);        // <-@@@, enhanced access mode
    // PFSH_WRITE_MASK(REG_NORPF_WRITE_RUN, 0x00, BMASK(5:4)); // <-@@@, enhanced access mode
#if USELOCK
    local_irq_restore(flags);
#endif
}

void HAL_ParFlash_Writes(U32 u32Addr, const U8 *pu8Data, U32 u32Size)
{
    U32 u32Loop ;
    U16 *pu16data ;
	U32 stop_num;
	const U32 addr1 = RIUBASE_NONPM + BK_PFSH + ((REG_NORPF_XIU_ADDR_L)<<2);
	const U32 addr2 = RIUBASE_NONPM + BK_PFSH + ((REG_NORPF_XIU_ADDR_H)<<2);
	const U32 addr3 = RIUBASE_NONPM + BK_PFSH + ((REG_NORPF_XIU_WDATA)<<2);
	const U32 addr4 = RIUBASE_NONPM + BK_PFSH + ((REG_NORPF_XIU_CTRL)<<2);
	//const U32 addr5 = RIUBASE_NONPM + BK_PFSH + ((REG_NORPF_XIU_STATUS)<<2);

#if USELOCK
    unsigned long flags ;
    local_irq_save(flags);
    local_irq_disable() ;
#endif

    u32Addr >>= 1;

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Writes(0x%08X, %p, %d)\n", (int)u32Addr, pu8Data, (int)u32Size));

    pu16data = (U16 *)(void *)pu8Data ;  // TODO: review, pu8Data is 2-B aligned

    // PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode // <-@@@, enhanced access mode

	stop_num = (u32Size + 1) / 2;

//    for (u32Loop = 0; u32Loop < (u32Size + 1) / 2; u32Loop++)
	for (u32Loop = 0; u32Loop < stop_num; u32Loop++)
    {
    	#if 1
		WRITE_WORD(addr1, (u32Addr+u32Loop) & 0xFFFF);
		WRITE_WORD(addr2, (u32Addr+u32Loop) >> 16);
		WRITE_WORD(addr3, *(pu16data+u32Loop));
		WRITE_WORD(addr4, 0x02); // 0x0A // <-@@@, enhanced access mode
		//_HAL_ParFlash_Delay();
		WRITE_WORD(addr4, 0x03); // 0x0B // <-@@@, enhanced access mode
		//_HAL_ParFlash_Delay();
		//_HAL_ParFlash_WaitReady();
		#else
        PFSH_WRITE(REG_NORPF_XIU_ADDR_L, (u32Addr+u32Loop) & 0xFFFF);
        PFSH_WRITE(REG_NORPF_XIU_ADDR_H, (u32Addr+u32Loop) >> 16);
        PFSH_WRITE(REG_NORPF_XIU_WDATA, *(pu16data+u32Loop));

        // PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x02); // 0x0A // <-@@@, enhanced access mode
        //_HAL_ParFlash_Delay(); // TODO: review, fine tune
        // PFSH_WRITE(REG_NORPF_XIU_CTRL, 0x03); // 0x0B // <-@@@, enhanced access mode
        //_HAL_ParFlash_Delay(); // TODO: review, fine tune

        //_HAL_ParFlash_WaitReady();
		#endif
    }

    // PFSH_WRITE_MASK(REG_NORPF_XIU_CTRL, 0x00, 0x08);        // <-@@@, enhanced access mode
    // PFSH_WRITE_MASK(REG_NORPF_WRITE_RUN, 0x00, BMASK(5:4)); // <-@@@, enhanced access mode
#if USELOCK
    local_irq_restore(flags);
#endif
}

EXPORT_SYMBOL(HAL_ParFlash_Writes);

static int ParFlash_inited = 0 ;
void HAL_ParFlash_Init(void)
{
#if USELOCK
    unsigned long flags ;
#endif
    if( ParFlash_inited )
        return ;
    ParFlash_inited = 1 ;
#if USELOCK
    local_irq_save(flags);
    local_irq_disable() ;
#endif
    _u8ParFlashDbgLv = PARFLASH_DBGLV_INFO;

    DEBUG_PAR_FLASH(PARFLASH_DBGLV_DEBUG, printk("HAL_ParFlash_Init()\n"));

    WRITE_WORD_MASK(0xBF200000 + (0x1EDE << 1), 0, BIT4);           // TODO: review, check chip setup in Linux kernel, reg_pf_mode

    WRITE_WORD_MASK(0xBF200000 + (0x1EDC << 1), 0, BIT1 | BIT0);    // TODO: review, reg_pcmconfig (all set to zero?)

    WRITE_WORD_MASK(0xBF200000 + (0x1EE0 << 1), 0, BMASK(5:0));     // TODO: review, reg_pcmisgpio (all set to zero?)

    WRITE_WORD(0xBF200000 + (0x1EC6 << 1), 0xB72D);                         // <-@@@
    WRITE_WORD_MASK(0xBF200000 + (0x1EBC << 1), BIT13, BIT13);              // <-@@@
    WRITE_WORD_MASK(0xBF200000 + (0x1EBE << 1), 0, BIT13);                  // <-@@@
    WRITE_WORD_MASK(0xBF000000 + (0x0E3E << 1), BIT9 | BIT8, BIT9 | BIT8);  // <-@@@

#if 0 // Review again
	PIU_WRITE_MASK(REG_PIU_SPI_CLK_SRC, PSCS_CLK_SRC_SEL_XTAL,	PSCS_CLK_SRC_SEL_MASK);
	PIU_WRITE_MASK(REG_PIU_SPI_CLK_SRC, PSCS_CLK_SEL_108MHZ,	PSCS_CLK_SEL_MASK);
	PIU_WRITE_MASK(REG_PIU_SPI_CLK_SRC, PSCS_CLK_SRC_SEL_CLK,	PSCS_CLK_SRC_SEL_MASK);
#else
	SLP_WRITE_MASK(REG_PM_CKG_SPI,PM_SPI_CLK_SWITCH_OFF,PM_SPI_CLK_SWITCH_MASK); // run @ 12M
	SLP_WRITE_MASK(REG_PM_CKG_SPI,PM_SPI_CLK_108MHZ,PM_SPI_CLK_SEL_MASK);		 // set ckg_spi
	SLP_WRITE_MASK(REG_PM_CKG_SPI,PM_SPI_CLK_SWITCH_ON,PM_SPI_CLK_SWITCH_MASK);  // run @ ckg_spi
#endif

    PFSH_WRITE_MASK(REG_NORPF_CLOCK_LENGTH, 0xA, BMASK(7:0));
    PFSH_WRITE(REG_NORPF_CEB_START, 0x3040);
    PFSH_WRITE(REG_NORPF_WEB_START, 0x4043);
    PFSH_WRITE_MASK(REG_NORPF_DATA_LATCH_CNT, 0x04, BMASK(7:0));
    //PFSH_WRITE_MASK(REG_NORPF_BRIDGE_CTRL, 0x01, BMASK(7:0)); // 0x0A
    PFSH_WRITE_MASK(REG_NORPF_BRIDGE_CTRL, 0x02, BMASK(7:0)); // 0x01 // 0x0A

	// config overwrite
	WRITE_WORD_MASK(REG_PMSLEEP_BASE+(0x1F<<2), BIT6 | BIT5 | BIT3 | BIT2 | BIT1 | BIT0, BMASK(7:0));

    *((volatile unsigned short *)(0xBF200BE8)) = 0x0010; // <-@@@, enhanced access mode
    PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x10); // 1 run, direct mode // <-@@@, enhanced access mode

#if USELOCK
    local_irq_restore(flags);
#endif

}

EXPORT_SYMBOL(HAL_ParFlash_Init);

#ifdef __cplusplus
}
#endif

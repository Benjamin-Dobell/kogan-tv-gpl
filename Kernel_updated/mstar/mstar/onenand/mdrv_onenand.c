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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_onenand.c
/// @brief  OneNAND Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/onenand.h>
#include <linux/mtd/partitions.h>

#include "mdrv_types.h"
#include "mhal_onenand.h"

#ifdef __cplusplus
extern "C"
{
#endif

unsigned char bouncing[4096+128] __attribute((aligned(128)));

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#define DRIVER_NAME "[ONENAND] "


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
struct onenand_info
{
    struct mtd_info         mtd;
    struct mtd_partition   *parts;
    struct onenand_chip     onenand;
};


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Extern function
//-------------------------------------------------------------------------------------------------

#ifdef CONFIG_MTD_CMDLINE_PARTS
extern int parse_cmdline_partitions(struct mtd_info *master, struct mtd_partition **pparts, unsigned long);
#endif

static const struct mtd_partition partition_info[] =
{
    {
    	.name   = "onenandtest0",
    	.offset = 0,
        .size   = 32 * 1024 * 1024
    },
    {
        .name   = "onenandtest1",
        .offset = 32 * 1024 * 1024,
        .size   = 32 * 1024 * 1024
    },
};

#define NUM_PARTITIONS ARRAY_SIZE(partition_info)

#if (defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_TITANIA11) || defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_TITANIA13)) && defined(CONFIG_MSTAR_NAND)
extern void nand_lock_fcie(void);
extern void nand_unlock_fcie(void);
#define NC_LOCK_FCIE()		nand_lock_fcie()
#define NC_UNLOCK_FCIE()	nand_unlock_fcie()
#else
#define NC_LOCK_FCIE()
#define NC_UNLOCK_FCIE()
#endif


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static inline int onenand_bufferram_offset(struct mtd_info *mtd, int area);

unsigned short MDrv_ONENAND_read_word(void __iomem *addr);
void MDrv_ONENAND_write_word(unsigned short value, void __iomem *addr);
static int MDrv_ONENAND_read_bufferram(struct mtd_info *mtd, int area, unsigned char *buffer, int offset, size_t count);
static int MDrv_ONENAND_write_bufferram(struct mtd_info *mtd, int area, const unsigned char *buffer, int offset, size_t count);


//
// Please sync to onenand_bufferram_offset() @ onenand_base.c
//
static inline int onenand_bufferram_offset(struct mtd_info *mtd, int area)
{
	struct onenand_chip *this = mtd->priv;

	if (ONENAND_CURRENT_BUFFERRAM(this)) {
		/* Note: the 'this->writesize' is a real page size */
		if (area == ONENAND_DATARAM)
			return this->writesize;
		if (area == ONENAND_SPARERAM)
			return mtd->oobsize;
	}

	return 0;
}


unsigned short MDrv_ONENAND_read_word(void __iomem *addr)
{
    U16 u16Data;

#if (defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
    HAL_ParFlash_Read((U32)addr, &u16Data);
#elif defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_TITANIA11) || defined(CONFIG_MSTAR_JANUS2) || defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_TITANIA13)
	NC_LOCK_FCIE();
	HAL_ONIF_Async_Read2Reg((U16*)(&u16Data), ((U32)addr)/2, 1);
	NC_UNLOCK_FCIE();
#endif

    return u16Data;
}

EXPORT_SYMBOL(MDrv_ONENAND_read_word);

void MDrv_ONENAND_write_word(unsigned short value, void __iomem *addr)
{
#if (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10) )
    HAL_ParFlash_Write((U32)addr, value);
#elif defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_TITANIA11) || defined(CONFIG_MSTAR_JANUS2) || defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_TITANIA13)
	NC_LOCK_FCIE();
	HAL_ONIF_Async_Write((U16*)(&value), ((U32)addr)/2, 1);
	NC_UNLOCK_FCIE();
#endif
}

EXPORT_SYMBOL(MDrv_ONENAND_write_word);

static int MDrv_ONENAND_read_bufferram(struct mtd_info *mtd, int area, unsigned char *buffer, int offset, size_t count)
{
	struct onenand_chip *this = mtd->priv;
	void __iomem *bufferram;

	bufferram = this->base + area;

	bufferram += onenand_bufferram_offset(mtd, area);

	if (ONENAND_CHECK_BYTE_ACCESS(count)) {
		unsigned short word;

		/* Align with word(16-bit) size */
		count--;

		/* Read word and save byte */
		word = this->read_word(bufferram + offset + count);
		buffer[count] = (word & 0xff);
	}

#if (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10))
    HAL_ParFlash_Reads((U32)(bufferram + offset), buffer, count);
#elif defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_TITANIA11) || defined(CONFIG_MSTAR_JANUS2 || defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_TITANIA13)
	NC_LOCK_FCIE();
	//HAL_ONIF_Async_Read2Reg((U16*)buffer, ((U32)(bufferram + offset))/2, ((U32)(count+1))/2);
	HAL_ONIF_Async_Read2Reg((U16*)bouncing, ((U32)(bufferram + offset))/2, ((U32)(count+1))/2);
	memcpy(buffer, bouncing, count);
	NC_UNLOCK_FCIE();
#endif

	return 0;
}


static int MDrv_ONENAND_write_bufferram(struct mtd_info *mtd, int area, const unsigned char *buffer, int offset, size_t count)
{
	struct onenand_chip *this = mtd->priv;
	void __iomem *bufferram;

	bufferram = this->base + area;

	bufferram += onenand_bufferram_offset(mtd, area);

	if (ONENAND_CHECK_BYTE_ACCESS(count)) {
		unsigned short word;
		int byte_offset;

		/* Align with word(16-bit) size */
		count--;

		/* Calculate byte access offset */
		byte_offset = offset + count;

		/* Read word and save byte */
		word = this->read_word(bufferram + byte_offset);
		word = (word & ~0xff) | buffer[count];
		this->write_word(word, bufferram + byte_offset);
	}

#if (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10))
    HAL_ParFlash_Writes((U32)(bufferram + offset), buffer, count);
#elif defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_TITANIA11) || defined(CONFIG_MSTAR_JANUS2) || defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_TITANIA13)
	NC_LOCK_FCIE();
	memcpy(bouncing, buffer, count);
	HAL_ONIF_Async_Write((U16*)bouncing, ((U32)(bufferram + offset))/2, ((U32)(count+1))/2);
	NC_UNLOCK_FCIE();
#endif

	return 0;
}


//-------------------------------------------------------------------------------------------------
//  Test Code
//-------------------------------------------------------------------------------------------------
#define ONENAND_DMA_TEST_CODE 0 // NOTE: just for T3

#if ONENAND_DMA_TEST_CODE

#include "mhal_onenand_reg.h"

#define ONENAND_INT_READY 0x8000

#define BDMA_SET_CH0_REG(x)         (0xBF200000 + ((0x900 + (x)) << 1))

#define BDMA_REG_CH0_CTRL           BDMA_SET_CH0_REG(0)
#define BDMA_REG_CH0_STATUS         BDMA_SET_CH0_REG(0x02)
#define BDMA_REG_CH0_SRC_SEL        BDMA_SET_CH0_REG(0x04)
#define BDMA_REG_CH0_DST_SEL        BDMA_SET_CH0_REG(0x05)
#define BDMA_REG_CH0_MISC           BDMA_SET_CH0_REG(0x06)
#define BDMA_REG_CH0_DWUM_CNT       BDMA_SET_CH0_REG(0x07)
#define BDMA_REG_CH0_SRC_ADDR_L     BDMA_SET_CH0_REG(0x08)
#define BDMA_REG_CH0_SRC_ADDR_H     BDMA_SET_CH0_REG(0x0A)
#define BDMA_REG_CH0_DST_ADDR_L     BDMA_SET_CH0_REG(0x0C)
#define BDMA_REG_CH0_DST_ADDR_H     BDMA_SET_CH0_REG(0x0E)
#define BDMA_REG_CH0_SIZE_L         BDMA_SET_CH0_REG(0x10)
#define BDMA_REG_CH0_SIZE_H         BDMA_SET_CH0_REG(0x12)

#define BUF_SIZE 0x800 // 2KB

static U8 u8Buf[BUF_SIZE] __attribute__((aligned(16)));

static void MDrv_ONENAND_blockerase(U16 u16BlockAddr)
{
    U16 u16Ret;

    HAL_ParFlash_Write(0xF100 << 1, ((u16BlockAddr&0x400)<<5)|(u16BlockAddr&0x3FF));    // set DFS[15] & Block Address[9:0]
    HAL_ParFlash_Write(0xF101 << 1, ((u16BlockAddr<1024) ? 0x0000 : 0x8000));           // set DBS to select bufferram for DDP
    HAL_ParFlash_Write(0xF241 << 1, 0x0000);                                            // clear interrupt register
    HAL_ParFlash_Write(0xF220 << 1, 0x0094);                                            // issue block erase command

    // wait until onenand is ready
    do
    {
        HAL_ParFlash_Read(0xF241 << 1, &u16Ret);
    } while (!(u16Ret & ONENAND_INT_READY)); // <-@@@

    // get operation result
    HAL_ParFlash_Read(0xF240 << 1, &u16Ret);
    if (u16Ret & 0x400)
    {
        printk(KERN_INFO DRIVER_NAME ">> block erase Fail\n");
    }
    else
    {
        printk(KERN_INFO DRIVER_NAME ">> block erase OK\n");
    }
}

static void MDrv_ONENAND_program(U16 u16BlockAddr, U16 u16PageSectorAddr)
{
    U16 u16Ret;

    HAL_ParFlash_Write(0xF100 << 1, ((u16BlockAddr&0x400)<<5)|(u16BlockAddr&0x3FF));    // set DFS[15] & Block Address[9:0]
    HAL_ParFlash_Write(0xF101 << 1, ((u16BlockAddr<1024) ? 0x0000 : 0x8000));           // set DBS to select bufferram for DDP
    HAL_ParFlash_Write(0xF107 << 1, u16PageSectorAddr);                                 // set page & sector address
    HAL_ParFlash_Write(0xF200 << 1, 0x0800);                                            // set BSA & BSC
    HAL_ParFlash_Write(0xF241 << 1, 0x0000);                                            // clear interrupt register
    HAL_ParFlash_Write(0xF220 << 1, 0x0080);                                            // issue load command

    // wait until onenand is ready
    do
    {
        HAL_ParFlash_Read(0xF241 << 1, &u16Ret);
    } while (!(u16Ret & ONENAND_INT_READY)); // <-@@@

    // get operation result
    HAL_ParFlash_Read(0xF240 << 1, &u16Ret);
    if (u16Ret & 0x400)
    {
        printk(KERN_INFO DRIVER_NAME ">> load bufferram Fail\n");
    }
    else
    {
        printk(KERN_INFO DRIVER_NAME ">> load bufferram OK\n");
    }

}

static void MDrv_ONENAND_load(U16 u16BlockAddr, U16 u16PageSectorAddr)
{
    U16 u16Ret, i;
    U8 *u8BufNonCache;

    HAL_ParFlash_Write(0xF100 << 1, ((u16BlockAddr&0x400)<<5)|(u16BlockAddr&0x3FF));    // set DFS[15] & Block Address[9:0]
    HAL_ParFlash_Write(0xF101 << 1, ((u16BlockAddr<1024) ? 0x0000 : 0x8000));           // set DBS to select bufferram for DDP
    HAL_ParFlash_Write(0xF107 << 1, u16PageSectorAddr);                                 // set page & sector address
    HAL_ParFlash_Write(0xF200 << 1, 0x0800);                                            // set BSA & BSC
    HAL_ParFlash_Write(0xF241 << 1, 0x0000);                                            // clear interrupt register
    HAL_ParFlash_Write(0xF220 << 1, 0x0000);                                            // issue load command

    // wait until onenand is ready
    do
    {
        HAL_ParFlash_Read(0xF241 << 1, &u16Ret);
    } while (!(u16Ret & ONENAND_INT_READY)); // <-@@@

    // get operation result
    HAL_ParFlash_Read(0xF240 << 1, &u16Ret);
    if (u16Ret & 0x400)
    {
        printk(KERN_INFO DRIVER_NAME ">> load bufferram Fail\n");
    }
    else
    {
        printk(KERN_INFO DRIVER_NAME ">> load bufferram OK\n");
    }

    // reset for read
    PFSH_WRITE_MASK(REG_NORPF_XIU_CTRL, 0, BIT3);
    PFSH_WRITE(REG_NORPF_WRITE_RUN, 0x05);

    printk(">> u8Buf = %p <<\n", u8Buf); // <-@@@

    // SPI (i.e. onenand) copy
    #if 1
    *((volatile U16 *)BDMA_REG_CH0_SRC_ADDR_L) = (U16)(0x0400);                             // set src
    *((volatile U16 *)BDMA_REG_CH0_SRC_ADDR_H) = (U16)(0x0000);
    #else
    *((volatile U16 *)BDMA_REG_CH0_SRC_ADDR_L) = (U16)(0xE000);                             // set src (i.e. 0xF000)
    *((volatile U16 *)BDMA_REG_CH0_SRC_ADDR_H) = (U16)(0x0001);
    #endif
    *((volatile U16 *)BDMA_REG_CH0_DST_ADDR_L) = (U16)(((U32)u8Buf) & 0xFFFF);              // set dst
    *((volatile U16 *)BDMA_REG_CH0_DST_ADDR_H) = (U16)((((U32)u8Buf) & 0x7FFFFFFF) >> 16);
    *((volatile U16 *)BDMA_REG_CH0_SIZE_L) = (U16)(BUF_SIZE & 0xFFFF);                      // set len
    *((volatile U16 *)BDMA_REG_CH0_SIZE_H) = (U16)(BUF_SIZE >> 16);
    *((volatile U16 *)BDMA_REG_CH0_SRC_SEL) = (U16)(0x4045);                                // spi -> MIU0 (16 bytes)
    *((volatile U16 *)BDMA_REG_CH0_STATUS) = BIT2 | BIT3 | BIT4;                            // clear status
    // *((volatile U16 *)BDMA_REG_CH0_MISC) |= 0x0400;
    *((volatile U16 *)BDMA_REG_CH0_CTRL) = BIT0;                                            // trigger
    do
    {
        u16Ret = *((volatile U16 *)BDMA_REG_CH0_STATUS);                                    // wait done
    } while (!(u16Ret & BIT3)); // <-@@@

    u8BufNonCache = (U8 *)((U32)u8Buf | 0x20000000);

    for (i = 0; i < BUF_SIZE; i++)                                                          // display the result
    {
        if (i % 32 == 0) printk("\n");
        printk("%02X ", u8BufNonCache[i]);
    }

    printk(">>>\n");

    for (i = 0; i < BUF_SIZE / 2; i++)
    {
        HAL_ParFlash_Read((0x200 + i )<< 1, &u16Ret);
        if (i % 16 == 0) printk("\n");
        printk("%04X ", u16Ret);
    }
}
#endif // ONENAND_DMA_TEST_CODE


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

static int __init MDrv_ONENAND_ModuleInit(void)
{
    struct onenand_info *info;
    int err = 0;

    printk(KERN_INFO DRIVER_NAME "initializing\n");

    info = kzalloc(sizeof(struct onenand_info), GFP_KERNEL);

    if (!info)
    {
        return -ENOMEM;
    }

    info->onenand.base = 0; // TODO: review, check HAL_ParFlash_Read(), HAL_ParFlash_Reads(), HAL_ParFlash_Write(), HAL_ParFlash_Writes()
    // info->mtd.name =     // TODO: review
    info->mtd.priv = &info->onenand;
    info->mtd.owner = THIS_MODULE;

    info->onenand.read_word         = MDrv_ONENAND_read_word;
    info->onenand.write_word        = MDrv_ONENAND_write_word;
    info->onenand.read_bufferram    = MDrv_ONENAND_read_bufferram;
    info->onenand.write_bufferram   = MDrv_ONENAND_write_bufferram;

#if (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10))
    HAL_ParFlash_Init();
#elif defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_TITANIA11) || defined(CONFIG_MSTAR_JANUS2) || defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_TITANIA13)
	NC_LOCK_FCIE();
	HAL_ONIF_INIT();
	NC_UNLOCK_FCIE();
#endif

#if 1 // <----------------------------------------------------------

#ifdef CONFIG_MTD_ONENAND
    if (onenand_scan(&info->mtd, 1))
    {
        printk(KERN_ERR DRIVER_NAME "onenand_scan() fail\n");
        err = -ENXIO;
        goto MDrv_ONENAND_ModuleInit_free_info;
    }
#endif

	{
#if 0//def CONFIG_MTD_CMDLINE_PARTS
        int mtd_parts_nb = 0;
        struct mtd_partition *mtd_parts = NULL;

        mtd_parts_nb = parse_cmdline_partitions(&(info->mtd), &mtd_parts, 0); // parse the partition information from command line
        if (mtd_parts_nb > 0)
        {
            add_mtd_partitions(&(info->mtd), mtd_parts, mtd_parts_nb);
        }
        else
#endif
			add_mtd_partitions(&(info->mtd), partition_info, NUM_PARTITIONS); // TODO: review, add default partition here
	}

#if ONENAND_DMA_TEST_CODE
    {
        #define BLOCK_NUM   1 // 4 // 1024
        #define PAGE_NUM    1 // 64

        U16 u16BlockNum;
        U16 u16PageNum;
        U16 i;

        for (u16BlockNum = 0; u16BlockNum < BLOCK_NUM; u16BlockNum++)
        {
            MDrv_ONENAND_blockerase(u16BlockNum);

            for (u16PageNum = 0; u16PageNum < PAGE_NUM; u16PageNum++)
            {
                printk("\n\n== u16BlockNum = %X, u16PageNum = %X ==\n\n", u16BlockNum, u16PageNum);

                for (i = 0; i < BUF_SIZE / 2; i++)
                {
                    HAL_ParFlash_Write((0x200 + i )<< 1, i);
                }

                MDrv_ONENAND_program(u16BlockNum, u16PageNum << 2);
                MDrv_ONENAND_load(u16BlockNum, u16PageNum << 2); // <-@@@
            }
        }
    }
#endif // ONENAND_DMA_TEST_CODE

#endif // <-------------------------------------------------------

    return err;

#ifdef CONFIG_MTD_ONENAND
MDrv_ONENAND_ModuleInit_free_info:

    kfree(info);
    return err;
#endif

}


static void __exit MDrv_ONENAND_ModuleExit(void)
{
    printk(KERN_INFO "MStar ONENAND driver exit\n");
}

module_init(MDrv_ONENAND_ModuleInit);
module_exit(MDrv_ONENAND_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("ONENAND driver");
MODULE_LICENSE("MSTAR");

#ifdef __cplusplus
}
#endif

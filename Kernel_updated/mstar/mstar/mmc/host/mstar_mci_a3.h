////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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

#ifndef MSTAR_MCI_H
#define MSTAR_MCI_H

#ifdef CONFIG_ARM
#include <mach/io.h>
#endif

#ifndef BIT0
#define BIT0	(0x0001)
#endif
#ifndef BIT1
#define BIT1	(0x0002)
#endif
#ifndef BIT2
#define BIT2	(0x0004)
#endif
#ifndef BIT3
#define BIT3	(0x0008)
#endif
#ifndef BIT4
#define BIT4	(0x0010)
#endif
#ifndef BIT5
#define BIT5	(0x0020)
#endif
#ifndef BIT6
#define BIT6	(0x0040)
#endif
#ifndef BIT7
#define BIT7	(0x0080)
#endif
#ifndef BIT8
#define BIT8	(0x0100)
#endif
#ifndef BIT9
#define BIT9	(0x0200)
#endif
#ifndef BIT10
#define BIT10	(0x0400)
#endif
#ifndef BIT11
#define BIT11	(0x0800)
#endif
#ifndef BIT12
#define BIT12	(0x1000)
#endif
#ifndef BIT13
#define BIT13	(0x2000)
#endif
#ifndef BIT14
#define BIT14	(0x4000)
#endif
#ifndef BIT15
#define BIT15	(0x8000)
#endif
#ifndef BIT16
#define BIT16   (0x00010000)
#endif
#ifndef BIT17
#define BIT17   (0x00020000)
#endif
#ifndef BIT18
#define BIT18   (0x00040000)
#endif
#ifndef BIT19
#define BIT19   (0x00080000)
#endif
#ifndef BIT20
#define BIT20   (0x00100000)
#endif
#ifndef BIT21
#define BIT21   (0x00200000)
#endif
#ifndef BIT22
#define BIT22   (0x00400000)
#endif
#ifndef BIT23
#define BIT23   (0x00800000)
#endif
#ifndef BIT24
#define BIT24   (0x01000000)
#endif
#ifndef BIT25
#define BIT25   (0x02000000)
#endif
#ifndef BIT26
#define BIT26   (0x04000000)
#endif
#ifndef BIT27
#define BIT27   (0x08000000)
#endif
#ifndef BIT28
#define BIT28   (0x10000000)
#endif
#ifndef BIT29
#define BIT29   (0x20000000)
#endif
#ifndef BIT30
#define BIT30   (0x40000000)
#endif
#ifndef BIT31
#define BIT31   (0x80000000)
#endif
//===========================================================
// device status (R1, R1b)
//===========================================================
#define eMMC_R1_ADDRESS_OUT_OF_RANGE     BIT31
#define eMMC_R1_ADDRESS_MISALIGN         BIT30
#define eMMC_R1_BLOCK_LEN_ERROR          BIT29
#define eMMC_R1_ERASE_SEQ_ERROR          BIT28
#define eMMC_R1_ERASE_PARAM              BIT27
#define eMMC_R1_WP_VIOLATION             BIT26
#define eMMC_R1_DEVICE_IS_LOCKED         BIT25
#define eMMC_R1_LOCK_UNLOCK_FAILED       BIT24
#define eMMC_R1_COM_CRC_ERROR            BIT23
#define eMMC_R1_ILLEGAL_COMMAND          BIT22
#define eMMC_R1_DEVICE_ECC_FAILED        BIT21
#define eMMC_R1_CC_ERROR                 BIT20
#define eMMC_R1_ERROR                    BIT19
#define eMMC_R1_CID_CSD_OVERWRITE        BIT16
#define eMMC_R1_WP_ERASE_SKIP            BIT15
#define eMMC_R1_ERASE_RESET              BIT13
#define eMMC_R1_CURRENT_STATE            (BIT12|BIT11|BIT10|BIT9)
#define eMMC_R1_READY_FOR_DATA           BIT8
#define eMMC_R1_SWITCH_ERROR             BIT7
#define eMMC_R1_EXCEPTION_EVENT          BIT6
#define eMMC_R1_APP_CMD                  BIT5

#define eMMC_ERR_R1_31_24       (eMMC_R1_ADDRESS_OUT_OF_RANGE| \
	                             eMMC_R1_ADDRESS_MISALIGN| \
	                             eMMC_R1_BLOCK_LEN_ERROR| \
	                             eMMC_R1_ERASE_SEQ_ERROR| \
	                             eMMC_R1_ERASE_PARAM| \
	                             eMMC_R1_WP_VIOLATION| \
	                             eMMC_R1_LOCK_UNLOCK_FAILED)
#define eMMC_ERR_R1_23_16       (eMMC_R1_COM_CRC_ERROR| \
								 eMMC_R1_ILLEGAL_COMMAND| \
								 eMMC_R1_DEVICE_ECC_FAILED| \
								 eMMC_R1_CC_ERROR| \
								 eMMC_R1_ERROR| \
								 eMMC_R1_CID_CSD_OVERWRITE)
#define eMMC_ERR_R1_15_8        (eMMC_R1_WP_ERASE_SKIP| \
								 eMMC_R1_ERASE_RESET)
#define eMMC_ERR_R1_7_0         (eMMC_R1_SWITCH_ERROR)
#define eMMC_ERR_R1_31_0        (eMMC_ERR_R1_31_24|eMMC_ERR_R1_23_16|eMMC_ERR_R1_15_8|eMMC_ERR_R1_7_0)

/* Get Register Address Macro Functions */
#define REG_OFFSET_SHIFT_BITS           2
#define GET_REG_ADDR(x, y)              (x+((y)<<REG_OFFSET_SHIFT_BITS))

#ifdef CONFIG_ARM
#define fcie_readw(addr)                reg_readl(addr)
#define fcie_writew(val, addr)          reg_writel(val, addr)

#define fcie_reg_read(host, reg)        reg_readl(((host)->baseaddr)+(reg))
#define fcie_reg_write(host, reg, val)  reg_writel(val, ((host)->baseaddr)+(reg))

#define fcie_setbits(host, reg, val)    fcie_reg_write(host, reg, (fcie_reg_read(host, reg)|val))
#define fcie_clrbits(host, reg, val)    fcie_reg_write(host, reg, (fcie_reg_read(host, reg)&~val))
#endif

#ifdef CONFIG_MIPS
#define readl(addr)						(*(volatile unsigned int *) (addr))
#define writel(b,addr)					((*(volatile unsigned int *) (addr)) = (b))

#define fcie_readw(addr)				(*(volatile unsigned short *) (addr))
#define fcie_writew(val, addr)			((*(volatile unsigned short *) (addr)) = (u16)(val))

#define fcie_reg_read(host, reg)		__raw_readl((host)->baseaddr + (reg))
#define fcie_reg_write(host, reg, val)	__raw_writel((val), (host)->baseaddr + (reg))

#define fcie_setbits(host, reg, val)	fcie_reg_write(host, reg, (fcie_reg_read(host, reg)|val))
#define fcie_clrbits(host, reg, val)	fcie_reg_write(host, reg, (fcie_reg_read(host, reg)&~val))
#endif

/******************************************************************************
* Function define for this driver
******************************************************************************/

/******************************************************************************
* Register Address Base
******************************************************************************/
#define REG_BANK_CLKGEN                 (0x580)
#define REG_BANK_CHIPTOP                (0xF00)
#define REG_BANK_FCIE0                  (0x8980)
#define REG_BANK_FCIE1                  (0x89E0)
#define REG_BANK_FCIE2                  (0x8A00)
#define REG_BANK_FCIE3                  (0x8A80)

#define REG_BANK_TIMER1					(0x1800)

#ifdef CONFIG_MIPS
#define RIU_PM_BASE						0xBF000000
#define RIU_BASE						0xBF200000
#endif

#ifdef CONFIG_ARM
#define RIU_PM_BASE						0x1F000000
#define RIU_BASE                        0x1F200000
#endif

#define CLKGEN_BASE                     GET_REG_ADDR(RIU_BASE, REG_BANK_CLKGEN)
#define CHIPTOP_BASE                    GET_REG_ADDR(RIU_BASE, REG_BANK_CHIPTOP)
#define FCIE0_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE0)
#define FCIE1_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE1)
#define FCIE2_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE2)
#define FCIE3_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE3)

#define TIMER1_BASE                     GET_REG_ADDR(RIU_PM_BASE, REG_BANK_TIMER1)

#define CLK_48MHz						(48000000)
#define CLK_43MHz						(43000000)
#define CLK_40MHz						(40000000)
#define CLK_36MHz						(36000000)
#define CLK_32MHz						(32000000)
#define CLK_27MHz						(27000000)
#define CLK_24MHz						(24000000)
#define CLK_20MHz						(20000000)
#define CLK_300KHz						(300000)

#define REG_CLK_MCU                     GET_REG_ADDR(CLKGEN_BASE, 0x10)
#define REG_CLK_MIU                     GET_REG_ADDR(CLKGEN_BASE, 0x1D)
#define REG_CLK_NFIE                    GET_REG_ADDR(CLKGEN_BASE, 0x64)

#define BIT_FCIE_CLK_XTAL				(0)
#define BIT_FCIE_CLK_20M				(BIT6|BIT2)
#define BIT_FCIE_CLK_27M				(BIT6|BIT3)
#define BIT_FCIE_CLK_32M				(BIT6|BIT3|BIT2)
#define BIT_FCIE_CLK_36M				(BIT6|BIT4)
#define BIT_FCIE_CLK_40M				(BIT6|BIT4|BIT2)
#define BIT_FCIE_CLK_43M				(BIT6|BIT4|BIT3)
#define BIT_FCIE_CLK_54M				(BIT6|BIT4|BIT3|BIT2)
#define BIT_FCIE_CLK_62M				(BIT6|BIT5)
#define BIT_FCIE_CLK_72M				(BIT6|BIT5|BIT2)
#define BIT_FCIE_CLK_80M				(BIT6|BIT5|BIT3)
#define BIT_FCIE_CLK_SSC				(BIT6|BIT5|BIT4)
#define BIT_FCIE_CLK_300K				(BIT6|BIT5|BIT4|BIT2)
#define BIT_FCIE_CLK_24M				(BIT6|BIT5|BIT4|BIT3)
#define BIT_FCIE_CLK_48M				(BIT6|BIT5|BIT4|BIT3|BIT2)

#if (defined(CONFIG_MSTAR_AMBER3) || \
     defined(CONFIG_MSTAR_AGATE) || \
     defined(CONFIG_MSTAR_EAGLE))
/* Be careful!! This clock should be 4X of REG_CLK_NFIE */
/* For example, if EMMC clock = 80MHz, the FCIE clock "MUST BE" 20MHz.  */
#define BIT_EMMC_CLK_80M                (0)
#define BIT_EMMC_CLK_108M               (BIT10)
#define BIT_EMMC_CLK_144M               (BIT11)
#define BIT_EMMC_CLK_160M               (BIT11|BIT10)
#define BIT_EMMC_CLK_192M               (BIT12)
#endif

#define REG_USB_DRV_BUS_CONFIG          GET_REG_ADDR(CHIPTOP_BASE, 0x02)
#define REG_EXTINT                      GET_REG_ADDR(CHIPTOP_BASE, 0x42)
#define REG_ALLAD_IN                    GET_REG_ADDR(CHIPTOP_BASE, 0x50)
#define REG_EMMC_BOOT                   GET_REG_ADDR(CHIPTOP_BASE, 0x10)
#define REG_SD_CONFIG                   GET_REG_ADDR(CHIPTOP_BASE, 0x5A)
#define REG_SD2_CONFIG                  GET_REG_ADDR(CHIPTOP_BASE, 0x1C)
#define REG_PCM_CONFIG                  GET_REG_ADDR(CHIPTOP_BASE, 0x64)
#define REG_EMMC_CONFIG                 GET_REG_ADDR(CHIPTOP_BASE, 0x6E)
#define REG_NAND_CONFIG                 GET_REG_ADDR(CHIPTOP_BASE, 0x6F)

#define TIMER1_ENABLE					GET_REG_ADDR(TIMER1_BASE, 0x20)
#define TIMER1_HIT						GET_REG_ADDR(TIMER1_BASE, 0x21)
#define TIMER1_MAX_LOW					GET_REG_ADDR(TIMER1_BASE, 0x22)
#define TIMER1_MAX_HIGH					GET_REG_ADDR(TIMER1_BASE, 0x23)
#define TIMER1_CAP_LOW					GET_REG_ADDR(TIMER1_BASE, 0x24)
#define TIMER1_CAP_HIGH					GET_REG_ADDR(TIMER1_BASE, 0x25)

/******************************************************************************
* MStar FCIE Register Address
******************************************************************************/
#define MIE_EVENT                       GET_REG_ADDR(FCIE0_BASE, 0x00)
#define FCIE_MIE_EVENT					0x00    //00h
#define MMA_DATA_END					BIT0    /* MMA data transcation complete event */
#define SD_CMD_END						BIT1    /* SD/MMC card command and response transcation complete event */
#define SD_DATA_END						BIT2    /* SD/MMC data transcation complete event */
#define CARD_DMA_END					BIT11
#define MMA_LAST_DONE					BIT14
#define SD_ALL_INT						(MMA_DATA_END|SD_CMD_END|SD_DATA_END|CARD_DMA_END|MMA_LAST_DONE)
/*---------------------------------------------------------------------------*/
#define MIE_INT_EN                      GET_REG_ADDR(FCIE0_BASE, 0x01)
#define FCIE_MIE_INT_EN					0x04    //01h
#define MMA_DATA_ENDE					BIT0    /* MMA_DATA_END interrupt enable*/
#define SD_CMD_ENDE						BIT1    /* SD_CMD_END interrupt enable */
#define SD_DATA_ENDE					BIT2    /* SD_DATA_END interrupt enable */
#define CARD_DMA_END_EN					BIT11
#define MMA_LAST_DONE_INT               BIT14
/*---------------------------------------------------------------------------*/
#define MMA_PRI_REG                     GET_REG_ADDR(FCIE0_BASE, 0x02)
#define FCIE_MMA_PRI_REG				0x08    //02h
#define MMA_R_PRIORITY					BIT0    /* MIU read request priority. 0:low priority, 1:high priroity*/
#define MMA_W_PRIORITY					BIT1    /* MIU write request priority. 0:low priority, 1:high priroity*/
#define JOB_RW_DIR						BIT2    /* Specify DMA cycle is Read or Write.*/
#define MIU_REQ_RST                     BIT4
#define FIFO_CLKRDY						BIT5    /* 0:Read from card(Data write to DRAM) 1:Write to card(Data read from DRAM)*/
#define BURST_LEN_CTRL                  BIT10
/*---------------------------------------------------------------------------*/
#define MIU_DMA_SEL                     GET_REG_ADDR(FCIE0_BASE, 0x03)
#define FCIE_MIU_DMA1					0x0C    //03h   /* DMA address: addr16~addr23*/
#define MIU1_SELECT						BIT15
/*---------------------------------------------------------------------------*/
#define FCIE_MIU_DMA0                   0x10    //04h   /* DMA address: addr00~addr15*/
/*---------------------------------------------------------------------------*/
#define CARD_EVENT                      GET_REG_ADDR(FCIE0_BASE, 0x05)
#define FCIE_CARD_EVENT					0x14    //05h
#define SD_STS_CHG						BIT0    /* SD/MMC Card plug-in or remove status */
#define SD_CARD_EVENTS					(SD_STS_CHG)
/*---------------------------------------------------------------------------*/
#define CARD_INT_EN                     GET_REG_ADDR(FCIE0_BASE, 0x06)
#define FCIE_CARD_INT_EN				0x18    //06h
#define SD_STS_EN						BIT0    /* SD card status change interrupt enable */
#define SD_CARD_DET_SRC					BIT7    /* SD Card detect select.0:SD_CDZ, 1:SD_DAT3*/
/*---------------------------------------------------------------------------*/
#define CARD_DET                        GET_REG_ADDR(FCIE0_BASE, 0x07)
#define FCIE_CARD_DET					0x1C    //07h
#define SD_DET_N						BIT0    /* SD/MMC card Detection*/
/*---------------------------------------------------------------------------*/
#define MIE_PATH_CTL                    GET_REG_ADDR(FCIE0_BASE, 0x08)
#define FCIE_MIE_PATH_CTL				0x28    //0Ah
#define MMA_ENABLE						BIT0    /* MIU DMA enable*/
#define SD_EN							BIT1    /* SD / MMC card interface enable*/
#define NC_EN							BIT5
/*---------------------------------------------------------------------------*/
#define JOB_BL_CNT                      GET_REG_ADDR(FCIE0_BASE, 0x0B)
#define FCIE_JOB_BL_CNT					0x2C    //0Bh   /* Total block count for this job*/
/*---------------------------------------------------------------------------*/
#define TR_BK_CNT                       GET_REG_ADDR(FCIE0_BASE, 0x0C)
#define FCIE_TR_BL_CNT					0x30    //0Ch   /* Real time number of sectors remained  to be transferred*/
/*---------------------------------------------------------------------------*/
#define RSP_SIZE                        GET_REG_ADDR(FCIE0_BASE, 0x0D)
#define FCIE_RSP_SIZE					0x34    //0Dh   /* The expected respone size(byte count). h01=1 byte, h40=64 bytes*/
/*---------------------------------------------------------------------------*/
#define CMD_SIZE                        GET_REG_ADDR(FCIE0_BASE, 0x0E)
#define FCIE_CMD_SIZE					0x38    //0Eh   /* The command transfer size(byte count). h01=1 byte, h40=64 bytes*/
/*---------------------------------------------------------------------------*/
#define CARD_WD_CNT                     GET_REG_ADDR(FCIE0_BASE, 0x0F)
#define FCIE_CARD_WD_CNT				0x3C    //0Fh   /* Expect data word count that transferred through CIF FIFO. 0x00 represents 256 words.*/
/*---------------------------------------------------------------------------*/
#define SD_MODE                         GET_REG_ADDR(FCIE0_BASE, 0x10)
#define FCIE_SD_MODE					0x40    //10h
#define SD_CLK_EN						BIT0    /* SD MIF output clock enable*/
#define SD_DAT_LINE						(BIT2|BIT1)
#define SD_DAT_LINE0					BIT1    /* Use DAT3-0 line*/
#define SD_DAT_LINE1					BIT2    /* Use DAT7-0 line*/
#define SDDRL							BIT3    /* Firmware write 1 to SD interface drive low*/
#define SD_CS_EN						BIT4    /* Set to enable clock auto-stop featuer, which will stop CLK between read blocks when Data FIFO is FULL. 1:enable*/
#define SD_DEST							BIT5    /* SD/MMC data transfer destination. 0:Data FIFO. 1:CIF FIFO.*/
#define SD_DATSYNC						BIT6    /* Synchornize data bus, for SD1.1 specification*/
#define MMC_BUS_TEST					BIT7    /* Test MMC bus type through CIF Data FIFO*/
#define SDIO_RDWAIT						BIT8    /* When read block,and Data FIFO busy. Hardware will drive SD_DAT1 to low to inform card controller that host is busy. High active*/
#define SDIO_SD_BUS_SW					BIT9    /* SDIO interface and SD/MMC card interface select. Set"1" to select SDIO. Default SD/MMC*/
#define SD_DMA_RD_CLK_STOP              BIT11
#define SDIO_PORT_SEL					BIT12
/*---------------------------------------------------------------------------*/
#define SD_CTL                          GET_REG_ADDR(FCIE0_BASE, 0x11)
#define FCIE_SD_CTL						0x44    //11h
#define SD_RSPR2_EN						BIT0    /* SD/MMC receive command response for R2 type*/
#define SD_RSP_EN						BIT1    /* SD/MMC receive command response enable*/
#define SD_CMD_EN						BIT2    /* SD/MMC transmit command enable*/
#define SD_DTRX_EN						BIT3    /* SD/MMC data transmit/receive enable*/
#define SD_DTRX_DIR						BIT4    /* SD/MMC data transmit direction. 0:Read from card. 1:Write to card*/
/*---------------------------------------------------------------------------*/
#define SD_STS                          GET_REG_ADDR(FCIE0_BASE, 0x12)
#define FCIE_SD_STS						0x48    //12h
#define SD_DAT_CERR						BIT0    /* Received data phase CRC error event*/
#define SD_DAT_STSERR					BIT1    /* Transmitted data phase:Flash program error*/
#define SD_DAT_STSNEG					BIT2    /* Transmitted data phase:Transmission error*/
#define SD_DAT_NORSP					BIT3    /* Transmitted CMD phase: Time out=64 clocks, there is no response on CMD line.*/
#define SD_CMDRSP_CERR					BIT4    /* Response CRC error*/
#define SD_WR_PRO_N						BIT5    /* Used for SD card Write protected*/
#define SD_CARD_BUSY                    BIT6
#define SD_STS_ERRORS					(SD_DAT_CERR|SD_DAT_STSERR|SD_DAT_STSNEG|SD_DAT_NORSP|SD_CMDRSP_CERR)
/*---------------------------------------------------------------------------*/
#define SDIO_CTL                        GET_REG_ADDR(FCIE0_BASE, 0x1B)
#define SDIO_BLK_SIZE					0x6C    //1B
#define SDIO_BLK_512					(0x200)
#define SDIO_BLK_MOD_EN					BIT15   /* SDIO block mode enable */
/*---------------------------------------------------------------------------*/
#define SDIO_ADDR0                      GET_REG_ADDR(FCIE0_BASE, 0x1C)
#define SDIO_MIU_DMA0					0x70    //1C
/*---------------------------------------------------------------------------*/
#define SDIO_ADDR1                      GET_REG_ADDR(FCIE0_BASE, 0x1D)
#define SDIO_MIU_DMA1					0x74    //1D
/*---------------------------------------------------------------------------*/
#define MIU_OFFSET                      GET_REG_ADDR(FCIE0_BASE, 0x2E)
#define FCIE_DMA_OFFSET					0xB8    //2Eh
/*---------------------------------------------------------------------------*/
#define EMMC_BOOT_CONF                  GET_REG_ADDR(FCIE0_BASE, 0x2F)
#define EMMC_BOOT_CONFIG				0xBC    //2Fh
#define BOOT_STG2_EN					BIT0
#define BOOT_END_EN						BIT1
#if (defined(CONFIG_MSTAR_AMBER3) || \
     defined(CONFIG_MSTAR_AGATE) || \
     defined(CONFIG_MSTAR_EAGLE))
#define SD_DDR_MODE                     BIT8
#define SD_BYPASS_MODE_DIS              BIT9
#define SD_SDRIN_BYPASS_MODE            BIT10
#define DQS_DELAY_SEL_MASK              (BIT15|BIT14|BIT13|BIT12)
#endif
/*---------------------------------------------------------------------------*/
#define TEST_MODE                       GET_REG_ADDR(FCIE0_BASE, 0x30)
#define FCIE_TEST_MODE					0xC0    //30h
#define REG_DBG_MOD_CLR                 (BIT8|BIT9|BIT10)
#define REG_DBG_MOD_MMA                 (BIT8|BIT10)
#define REG_DBG_MOD_DFF                 (BIT9|BIT10)
#define FCIE_SOFT_RST					BIT12   /* FCIE module soft reset, low active*/
#define ENDIAN_SEL						BIT13   /* Low:little endian. High:big endian*/
/*---------------------------------------------------------------------------*/
#define DEBUG_BUS0                      GET_REG_ADDR(FCIE0_BASE, 0x31)
#define FCIE_DEBUG_BUS_LOW				0xC4    //31h
#if (defined(CONFIG_MSTAR_AMBER3) || \
     defined(CONFIG_MSTAR_AGATE) || \
     defined(CONFIG_MSTAR_EAGLE))
#define DDR_DISABLE_CLOCK_STOP          BIT6
#define TOGGLE_COUNT_RESET              BIT7
#endif
/*---------------------------------------------------------------------------*/
#define DEBUG_BUS1                      GET_REG_ADDR(FCIE0_BASE, 0x32)
#define FCIE_DEBUG_BUS_HIGH				0xC8    //32h
/*---------------------------------------------------------------------------*/
#if (defined(CONFIG_MSTAR_AMBER3) || \
     defined(CONFIG_MSTAR_AGATE) || \
     defined(CONFIG_MSTAR_EAGLE))
#define FCIE_SD_TOGGLE_CNT              0xCC    //33h
#endif
/*---------------------------------------------------------------------------*/
#define MISC                            GET_REG_ADDR(FCIE0_BASE, 0x36)
#define FCIE_MISC                       0xD8    // 36h
#if (defined(CONFIG_MSTAR_AMBER3) || \
     defined(CONFIG_MSTAR_AGATE) || \
     defined(CONFIG_MSTAR_EAGLE))
#define MISC_TEST_MIU                   BIT0
#define MISC_TEST_MIU_STS               BIT2
#define DQS_MODE_DELAY_MASK             (3<<10)
#define DQS_MODE_DELAY_2T               (0<<10)
#define DQS_MODE_DELAY_1_5T             (1<<10)
#define DQS_MODE_DELAY_2_5T             (2<<10)
#define DQS_MODE_DELAY_1T               (3<<10)
#endif

/******************************************************************************
* Low level type for this driver
******************************************************************************/
struct mstar_mci_host
{
    struct mmc_host			*mmc;
    struct mmc_command		*cmd;
    struct mmc_request		*request;

    void __iomem			*baseaddr;
    s32						irq;

    u16						sd_clk;
    u16						sd_mod;

    /* Flag indicating when the command has been sent.          */
    /* This is used to work out whether or not to send the stop */
    u32						flags;
    u32                     present;
	u32                     u02_flag; // indicate u02 chip, u02 needs special patch
	u32                     u02_set;

    #ifdef CONFIG_DEBUG_FS
    struct dentry           *debug_root;
    struct dentry           *debug_regs;
    struct dentry           *debug_perf;
    struct dentry           *debug_reql;
    #endif
};  /* struct mstar_mci_host*/

#endif

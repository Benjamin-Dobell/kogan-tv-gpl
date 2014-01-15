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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/dma-mapping.h>
#include <linux/mmc/host.h>
#include <linux/scatterlist.h>

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#endif

#if (defined(CONFIG_MSTAR_AMBER3) || \
     defined(CONFIG_MSTAR_AGATE) || \
     defined(CONFIG_MSTAR_EAGLE))
#include <mstar/mstar_chip.h>
#endif
#include "chip_int.h"
#include "mstar_mci_a3.h"

/******************************************************************************
 * Defines
 ******************************************************************************/
#define DRIVER_NAME					"mstar_mci"
#define FL_SENT_COMMAND				(1 << 0)
#define FL_SENT_STOP				(1 << 1)
#define LOOP_DELAY_SAVE_CPU         (500)
#define LOOP_DELAY_WAIT_COMPLETE	(3200000)

#define MSTAR_MCI_IRQ               1

/* For debugging purpose */
#define MSTAR_MCI_DEBUG				0

#define CMD_ARG_DEBUG               0
#define CMD_RSP_DEBUG               0

#if (defined(CONFIG_MSTAR_AMBER3) || \
     defined(CONFIG_MSTAR_AGATE) || \
     defined(CONFIG_MSTAR_EAGLE))

/* For clock setting */
#define CLK_48M                     0
#if !(defined(CLK_48M) && CLK_48M)
#define CLK_43M                     0
#if !(defined(CLK_43M) && CLK_43M)
#define CLK_40M                     0
#if !(defined(CLK_40M) && CLK_40M)
#define CLK_36M                     0
#if !(defined(CLK_36M) && CLK_36M)
#define CLK_32M                     1
#if !(defined(CLK_32M) && CLK_32M)
#define CLK_27M                     0
#if !(defined(CLK_27M) && CLK_27M)
#define CLK_24M                     0
#if !(defined(CLK_24M) && CLK_24M)
#define CLK_20M                     0
#endif  // CLK_24M
#endif  // CLK_27M
#endif  // CLK_32M
#endif  // CLK_36M
#endif  // CLK_40M
#endif  // CLK_43M
#endif  // CLK_48M

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
#define BYPASS_MODE_ENABLE          1
#define SDR_MODE_ENABLE             0
#define DDR_MODE_ENABLE             0
#else
#define BYPASS_MODE_ENABLE          1
#define SDR_MODE_ENABLE             0
#define DDR_MODE_ENABLE             0
#endif
#endif

#define MMC_SPEED_TEST              0

#if defined(MMC_SPEED_TEST) && MMC_SPEED_TEST

#define DMA_TIME_TEST               0
#if DMA_TIME_TEST
#define DMA_TIME_TEST_READ          0
#endif

#define REQ_TIME_TEST               0
#if REQ_TIME_TEST
#define REQ_TIME_TEST_READ          0
#endif

#define BUSY_TIME_TEST              0

#endif


#define MMC_REQ_LENGTH_TEST         0

#define MMC_MIU_DEBUG               1

#if (MSTAR_MCI_DEBUG)
#undef pr_debug
#define pr_debug(fmt, arg...)		printk(KERN_INFO fmt, ##arg)
#else
#undef pr_debug
#define pr_debug(fmt, arg...)
#endif
#define simple_debug(fmt, arg...)	//printk(KERN_INFO fmt, ##arg)

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
static void mstar_mci_process_next(struct mstar_mci_host *pHost_st);
static void mstar_mci_completed_command(struct mstar_mci_host *pHost_st);

static void mstar_mci_request(struct mmc_host *pMMC_st, struct mmc_request *pMRQ_st);
static void mstar_mci_set_ios(struct mmc_host *pMMC_st, struct mmc_ios *pIOS_st);
static s32 mstar_mci_get_ro(struct mmc_host *pMMC_st);

static void fcie_reg_dump(struct mstar_mci_host *pHost_st);
static u8 clear_fcie_status(struct mstar_mci_host *pHost_st, u16 reg, u16 value);


/*****************************************************************************
 * Define external Global Variables
 ******************************************************************************/
static atomic_t emmc_in_use= ATOMIC_INIT(0);
extern struct semaphore	PfModeSem;
/*****************************************************************************
 * Define Static Global Variables
 ******************************************************************************/
/* MSTAR Multimedia Card Interface Operations */
static const struct mmc_host_ops sg_mstar_mci_ops =
{
    .request =	mstar_mci_request,
    .set_ios =	mstar_mci_set_ios,
    .get_ro =	mstar_mci_get_ro,
};

static struct resource sg_mmc_resources_st[] =
{
    [0] =
    {
        .start =	FCIE0_BASE,
        .end =		FCIE0_BASE + (0x00000400*2) - 1,
        .flags =	IORESOURCE_MEM,
    },
    [1] =
    {
        .start =    E_IRQ_NFIE,
        .end =      E_IRQ_NFIE,
        .flags =    IORESOURCE_IRQ,
    },
};

static struct platform_device sg_mstar_mmc_device_st =
{
    .name =	"mstar_mci",
    .id = 0,
    .resource =	sg_mmc_resources_st,
    .num_resources = ARRAY_SIZE(sg_mmc_resources_st),
};

/******************************************************************************
 * Functions
 ******************************************************************************/
#if defined(DMA_TIME_TEST) && DMA_TIME_TEST
static u32 totaldmalen = 0;
static u32 totaldmatime = 0;
#endif

#if defined(REQ_TIME_TEST) && REQ_TIME_TEST
static u32 totalreqlen = 0;
static u32 totalreqtime = 0;
#endif

#if defined(BUSY_TIME_TEST) && BUSY_TIME_TEST
static u32 total_small_D0_count = 0;
static u32 total_10us_D0_count = 0;
static u32 total_100us_D0_count = 0;
static u32 total_1ms_D0_count = 0;
static u32 total_10ms_D0_count = 0;
static u32 total_large_D0_count = 0;

static u32 min_D0_time = 0xFFFFFFFF;
static u32 max_D0_time = 0;
static u32 total_D0_time = 0;
static u32 total_D0_count = 0;
#endif

#if defined(MMC_REQ_LENGTH_TEST) && MMC_REQ_LENGTH_TEST
static u32 total_512B_read_count = 0;
static u32 total_1KB_read_count = 0;
static u32 total_2KB_read_count = 0;
static u32 total_4KB_read_count = 0;
static u32 total_8KB_read_count = 0;
static u32 total_16KB_read_count = 0;
static u32 total_32KB_read_count = 0;
static u32 total_64KB_read_count = 0;
static u32 total_128KB_read_count = 0;
static u32 total_256KB_read_count = 0;
static u32 total_512KB_read_count = 0;
static u32 total_1MB_read_count = 0;
static u32 total_2MB_read_count = 0;
static u32 total_read_count = 0;

static u32 total_512B_write_count = 0;
static u32 total_1KB_write_count = 0;
static u32 total_2KB_write_count = 0;
static u32 total_4KB_write_count = 0;
static u32 total_8KB_write_count = 0;
static u32 total_16KB_write_count = 0;
static u32 total_32KB_write_count = 0;
static u32 total_64KB_write_count = 0;
static u32 total_128KB_write_count = 0;
static u32 total_256KB_write_count = 0;
static u32 total_512KB_write_count = 0;
static u32 total_1MB_write_count = 0;
static u32 total_2MB_write_count = 0;
static u32 total_write_count = 0;
#endif

#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE)

#define FCIE_INT      GET_REG_ADDR(FCIE0_BASE, 0x01)
#define FCIE_MIU      GET_REG_ADDR(FCIE0_BASE, 0x03)
#if defined(CONFIG_MSTAR_SDMMC)
extern void enable_top_sdpad(u8 enable);
#endif
void enable_top_emmcpad(u8 enable)
{
    u16 emmc_reg;

    emmc_reg = fcie_readw(REG_EMMC_CONFIG);
    emmc_reg &= ~(BIT6|BIT7);

    if (enable)
    {
        fcie_writew((fcie_readw(REG_SD2_CONFIG)&~BIT13), REG_SD2_CONFIG);
        #if defined(BYPASS_MODE_ENABLE) && BYPASS_MODE_ENABLE
        emmc_reg |= BIT6;
        #else
        emmc_reg |= BIT7;
        #endif
        fcie_writew(emmc_reg, REG_EMMC_CONFIG);
    }
    else
    {
        fcie_writew(emmc_reg, REG_EMMC_CONFIG);
        fcie_writew((fcie_readw(REG_SD2_CONFIG)|BIT13), REG_SD2_CONFIG);
    }
}

void emmc_bus_obtain(u8 select)
{
    if (select)   // select emmc
    {
        fcie_writew(0, FCIE_INT);
        #if defined(CONFIG_MSTAR_SDMMC)
        enable_top_sdpad(0);
        #endif
        enable_top_emmcpad(1);
        atomic_set(&emmc_in_use, 1);
    }
    else // release
    {
        enable_top_emmcpad(0);
        #if defined(CONFIG_MSTAR_SDMMC)
        enable_top_sdpad(1);
        #endif
        fcie_writew(0, FCIE_MIU);
        atomic_set(&emmc_in_use, 0);
    }
}
#endif

#if defined(MMC_SPEED_TEST) && MMC_SPEED_TEST
void mstar_mci_PIU_Timer1_Start(void)
{
    // Reset PIU Timer1
    reg_writel(0xFFFF, TIMER1_MAX_LOW);
    reg_writel(0xFFFF, TIMER1_MAX_HIGH);
    reg_writel(0, TIMER1_ENABLE);

    // Start PIU Timer1
    reg_writel(0x1, TIMER1_ENABLE);
}

u32 mstar_mci_PIU_Timer1_End(void)
{
    u32 u32HWTimer = 0;
    u32 u32TimerLow = 0;
    u32 u32TimerHigh = 0;

    // Get timer value
    u32TimerLow = reg_readl(TIMER1_CAP_LOW);
    u32TimerHigh = reg_readl(TIMER1_CAP_HIGH);

    u32HWTimer = (u32TimerHigh<<16) | u32TimerLow;

    return u32HWTimer;
}
#endif


#if defined(MMC_MIU_DEBUG) && MMC_MIU_DEBUG
void mstar_mci_check_miu_sts(struct mstar_mci_host *pHost_st)
{
     u8 u8Retry=3;
     u16 u16Reg;

     fcie_setbits(pHost_st, FCIE_MISC, MISC_TEST_MIU);
     while(u8Retry--)
     {
        udelay(10);
        u16Reg = fcie_reg_read(pHost_st, FCIE_MISC);
        printk(KERN_ERR "FCIE_REG[0x36]:[%x], MIU clock is", u16Reg);
        if (u16Reg & MISC_TEST_MIU_STS)
            printk(KERN_ERR "running!!\n");
        else
            printk(KERN_ERR "stop!!\n");
     }
     fcie_clrbits(pHost_st, FCIE_MISC, MISC_TEST_MIU);
}

void mstar_mci_check_fcie_dbgbus(struct mstar_mci_host *pHost_st)
{
     u8 u8Retry=100;
     u16 u16Reg_mma, u16Reg_dff;

     while(u8Retry--)
     {
        fcie_clrbits(pHost_st, FCIE_TEST_MODE, REG_DBG_MOD_CLR);
        fcie_setbits(pHost_st, FCIE_TEST_MODE, REG_DBG_MOD_MMA);
        u16Reg_mma = fcie_reg_read(pHost_st, FCIE_DEBUG_BUS_LOW)&0xFF;

        fcie_clrbits(pHost_st, FCIE_TEST_MODE, REG_DBG_MOD_CLR);
        fcie_setbits(pHost_st, FCIE_TEST_MODE, REG_DBG_MOD_DFF);
        u16Reg_dff = fcie_reg_read(pHost_st, FCIE_DEBUG_BUS_LOW)&0xFF;

        if ((u16Reg_mma == 0)&&(u16Reg_dff == 0))
            return;
     }

     printk(KERN_ERR "%s: error, dump fcie registers\n", __FUNCTION__);
     fcie_reg_dump(pHost_st);
}
#endif

static void mstar_mci_reset_FCIE(struct mstar_mci_host *pHost_st)
{
    u16 u16_TimeoutCount = 0;

    fcie_setbits(pHost_st, FCIE_MMA_PRI_REG, MIU_REQ_RST);
	fcie_clrbits(pHost_st, FCIE_MIE_PATH_CTL, MMA_ENABLE);
    fcie_clrbits(pHost_st, FCIE_TEST_MODE, FCIE_SOFT_RST);

    udelay(10);

    while( (fcie_reg_read(pHost_st, FCIE_TEST_MODE) & FCIE_SOFT_RST) == 0 )
    {
        fcie_setbits(pHost_st, FCIE_TEST_MODE, FCIE_SOFT_RST);

        if( ++u16_TimeoutCount < 1000000 )
        {
            udelay(1);
        }
        else
        {
            panic("%s: Wait reset FCIE timeout\n", __FUNCTION__);
        }
    }
    fcie_clrbits(pHost_st, FCIE_MMA_PRI_REG, MIU_REQ_RST);
    udelay(100);
	clear_fcie_status(pHost_st, FCIE_MIE_EVENT, SD_ALL_INT);
	clear_fcie_status(pHost_st, FCIE_SD_STS, SD_STS_ERRORS);
}

static u8 waitD0High(struct mstar_mci_host *pHost_st, u32 u32LoopCnt)
{
    u32 u32Cnt=0;

    #if defined(BUSY_TIME_TEST) && BUSY_TIME_TEST
    u32 u32Ticks = 0;

    mstar_mci_PIU_Timer1_Start();
    #endif

    while ((fcie_reg_read(pHost_st, FCIE_SD_STS) & BIT8) == 0) /* card busy */
    {
        if( (u32Cnt++) == LOOP_DELAY_SAVE_CPU )
            break;
        udelay(1);
    }

    while ((fcie_reg_read(pHost_st, FCIE_SD_STS) & BIT8) == 0) /* card busy */
    {
        //schedule_timeout(1);    //Use schedule_timeout for improving the CPU usage
        msleep(1);
        if ((u32Cnt++) == u32LoopCnt)
        {
            printk("\033[31mmstar_mci: card busy\033[m\n");
            return 1;
        }

    }

    #if defined(BUSY_TIME_TEST) && BUSY_TIME_TEST
    u32Ticks = mstar_mci_PIU_Timer1_End();

    if( min_D0_time > u32Ticks )
        min_D0_time = u32Ticks;

    if( max_D0_time < u32Ticks )
        max_D0_time = u32Ticks;

    total_D0_time += u32Ticks;

    if( u32Ticks <= 12 )
        total_small_D0_count++;
    else if( u32Ticks <= 120 )
        total_10us_D0_count++;
    else if( u32Ticks <= 1200 )
        total_100us_D0_count++;
    else if( u32Ticks <= 12000 )
        total_1ms_D0_count++;
    else if( u32Ticks <= 120000 )
        total_10ms_D0_count++;
    else
        total_large_D0_count++;

    total_D0_count++;
    #endif

    return 0;
}

int mstar_mci_check_D0_status(void)
{
    u16 u16Reg = 0;

    if (fcie_readw(REG_EMMC_BOOT) & BIT1)
        return 1;

    #if defined(CONFIG_MSTAR_SDMMC)
    enable_top_sdpad(0);
    #endif
    enable_top_emmcpad(1);

    u16Reg = fcie_readw(SD_STS);

    #if defined(CONFIG_MSTAR_SDMMC)
    enable_top_sdpad(1);
    #endif
    enable_top_emmcpad(0);

    if( u16Reg & BIT8 )
        return 1;
    else
        return 0;

    return -1;
}
EXPORT_SYMBOL(mstar_mci_check_D0_status);


bool mstar_mci_exit_checkdone_ForCI(void)
{
  
  u32 u32_Cnt=0;
  
  while (mstar_mci_check_D0_status() == 0) /* card busy */
  {
	  if ((u32_Cnt++) == 3000) // 3 sec
	  {
		  printk("eMMC Info: PCM check -> mstar_mci: card busy\n");
		  return false;
	  }
	  schedule_timeout(1);	  //Use schedule_timeout for improving the CPU usage
  }
  return true;  
}
EXPORT_SYMBOL(mstar_mci_exit_checkdone_ForCI);


static u8 clear_fcie_status(struct mstar_mci_host *pHost_st, u16 reg, u16 value)
{
   u8 i=0;

   do
   {
       fcie_reg_write(pHost_st, reg, value);
       if (++i == 0)
       {
           printk("mstar_mci: Error, clear FCIE status in RIU time out!\n");
           return 1;
       }
       udelay(1);
   } while(fcie_reg_read(pHost_st, reg)&value);

   return 0;
}

static u8 wait_fifoclk_ready(struct mstar_mci_host *pHost_st)
{
   u32 wait_cnt=0;

   do
   {
       if (++wait_cnt >= 1000000)
       {
           fcie_reg_dump(pHost_st);
           #if defined(MMC_MIU_DEBUG) && MMC_MIU_DEBUG
           mstar_mci_check_miu_sts(pHost_st);
           #endif
           panic("%s: Error, wait FIFO clock ready time out!\n", __FUNCTION__);
           return 1;
       }
       udelay(1);
   } while((fcie_reg_read(pHost_st, FCIE_MMA_PRI_REG)&FIFO_CLKRDY)==0);

   return 0;
}

#if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
//static atomic_t fcie_int = ATOMIC_INIT(0);
volatile u32 fcie_int = 0;
#endif

static u32 mstar_mci_wait_event(struct mstar_mci_host *pHost_st, u32 u32Event, u32 u32Timeout)
{
    u32 i = 0, u32_read_events;

    #if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ

    if( wait_event_timeout(fcie_wait, (fcie_int == 1), msecs_to_jiffies(3000)) == 0 )
    //if( wait_event_timeout(fcie_wait, (atomic_read(&fcie_int) == 1), u32Timeout*HZ/1000000) == 0 )
    {
        printk("\033[31m Warning!!! There is an interrupt lost!!!\033[m\n");

        #if defined(MMC_MIU_DEBUG) && MMC_MIU_DEBUG
        mstar_mci_check_miu_sts(pHost_st);
        #endif
        if( u32Event & SD_CMD_END )
        {
            fcie_clrbits(pHost_st, FCIE_MIE_INT_EN, SD_CMD_END);
        }

        if( u32Event & MMA_LAST_DONE )
        {
            fcie_clrbits(pHost_st, FCIE_MIE_INT_EN, MMA_LAST_DONE);
        }

        if( u32Event & CARD_DMA_END )
        {
            fcie_clrbits(pHost_st, FCIE_MIE_INT_EN, CARD_DMA_END);
        }

        // Error handling of interrupt lose
        for(i=0; i<u32Timeout; i++)
        {
            if( (fcie_reg_read(pHost_st, FCIE_MIE_EVENT) & u32Event) == u32Event )
                break;

            //udelay(1);
            schedule_timeout(1);    //Use schedule_timeout for improving the CPU usage
        }

    }

    fcie_int = 0;

    #else

    for(i=0; i<u32Timeout; i++)
    {
        if( (fcie_reg_read(pHost_st, FCIE_MIE_EVENT) & u32Event) == u32Event )
            break;

        //udelay(1);
        schedule_timeout(1);    //Use schedule_timeout for improving the CPU usage
    }

    #endif

    if( i == u32Timeout )
    {
        fcie_reg_dump(pHost_st);
        //panic("%s: Wait event timeout!\n", __FUNCTION__);
        printk("%s: Wait event timeout!\n", __FUNCTION__);
    }

    #if 0//defined(MMC_MIU_DEBUG) && MMC_MIU_DEBUG
    if ((u32Event & SD_CMD_END)==0)
        mstar_mci_check_fcie_dbgbus(pHost_st);
    #endif

	for(i=0; i<1000000; i++)
	{
		u32_read_events= fcie_reg_read(pHost_st, FCIE_MIE_EVENT);
		
		if((u32_read_events & u32Event) == u32Event)
			break;
		udelay(1);
	}
	for(i=0; i<1000000; i++)
	{
		fcie_reg_write(pHost_st, FCIE_MIE_EVENT, u32Event);
		u32_read_events = fcie_reg_read(pHost_st, FCIE_MIE_EVENT);
		
		if((u32_read_events & u32Event) == 0)
			break;
	}

    return i;

}

static void fcie_reg_dump(struct mstar_mci_host *pHost_st)
{
	u16 i;
	u16 regval;

	printk("\nCHIPTOP:\n");
	printk("allpadin:\t\t%04X\n", fcie_readw(REG_ALLAD_IN));
    printk("USB DRV BUS config: \t%04X\n", fcie_readw(REG_USB_DRV_BUS_CONFIG));
    printk("EXT INT: \t\t%04X\n", fcie_readw(REG_EXTINT));
    printk("SD pad: \t\t\t%04X\n", fcie_readw(REG_SD_CONFIG));
	printk("SD2 pad: \t\t\t%04X\n", fcie_readw(REG_SD2_CONFIG));
	printk("PCM pad:\t\t\t%04X\n", fcie_readw(REG_PCM_CONFIG));
	printk("eMMC pad:\t\t%04X\n", fcie_readw(REG_EMMC_CONFIG));
	printk("nand pad:\t\t%04X\n", fcie_readw(REG_NAND_CONFIG));

	printk("\nCLKGEN0:\n");
    printk("mcu clk:\t\t%04X\n", fcie_readw(REG_CLK_MCU));
    printk("miu clk:\t\t%04X\n", fcie_readw(REG_CLK_MIU));
	printk("fcie clk:\t\t%04X\n", fcie_readw(REG_CLK_NFIE));

	printk("\nDump FCIE3 bank:\n");

	regval = fcie_reg_read(pHost_st, FCIE_MIE_EVENT);
	printk("MIE_EVENT(0x00)=\t\t%04X ", regval);
	if( regval & MMA_DATA_END )
		printk("\033[31mMMA_DATA_END\033[m ");
	if( regval & SD_CMD_END )
		printk("\033[31mSD_CMD_END\033[m ");
	if( regval & SD_DATA_END )
		printk("\033[31mSD_DATA_END\033[m ");
	if( regval & CARD_DMA_END )
		printk("\033[31mCARD_DMA_END\033[m ");
	if( regval & MMA_LAST_DONE)
		printk("\033[31mMMA_LAST_DONE\033[m ");
	printk("\n");

	regval = fcie_reg_read(pHost_st, FCIE_MIE_INT_EN);
	printk("MIE_INT_EN(0x01)=\t%04X ", regval);
	if( regval & MMA_DATA_END )
		printk("\033[31mMMA_DATA_END\033[m ");
	if( regval & SD_CMD_END )
		printk("\033[31mSD_CMD_END\033[m ");
	if( regval & SD_DATA_END )
		printk("\033[31mSD_DATA_END\033[m ");
	if( regval & CARD_DMA_END )
		printk("\033[31mCARD_DMA_END\033[m ");
	if( regval & MMA_LAST_DONE)
		printk("\033[31mMMA_LAST_DONE\033[m ");
	printk("\n");

	regval = fcie_reg_read(pHost_st, FCIE_MMA_PRI_REG);
	printk("MMA_PRI_REG(0x02)=\t%04X ", regval);
	if( regval & JOB_RW_DIR)
		printk("\033[31mWrite\033[m ");
	else
		printk("\033[31mRead\033[m ");
	if( regval & FIFO_CLKRDY )
		printk("\033[31mFIFO_CLKRDY\033[m ");
	printk("\n");

	regval = fcie_reg_read(pHost_st, FCIE_MIU_DMA1);
	printk("MIU_DMA1(0x03)=\t\t%04X ", regval);
	if( regval & MIU1_SELECT )
		printk("\033[31mMIU1\033[m\n");
	else
		printk("\033[31mMIU0\033[m\n");

	regval = fcie_reg_read(pHost_st, FCIE_CARD_EVENT);
	printk("CARD_EVENT(0x05)=\t%04X ", regval);
	if( regval & SD_STS_CHG )
		printk("\033[31mSD_STS_CHG\033m");
	printk("\n");

	regval = fcie_reg_read(pHost_st, FCIE_CARD_INT_EN);
	printk("CARD_INT_EN(0x06)=\t%04X ", regval);
	if( regval & SD_STS_EN )
		printk("\033[31mSD_STS_EN\033[m");
	printk("\n");

	regval = fcie_reg_read(pHost_st, FCIE_CARD_DET);
	printk("CARD_DET(0x07)=\t\t%04X ", regval);
	if( regval & SD_DET_N )
		printk("\033[31mSD_DET_N\033[m ");
	printk("\n");

	regval = fcie_reg_read(pHost_st, FCIE_MIE_PATH_CTL);
	printk("MIE_PATH_CTL(0x0A)=\t%04X ", regval);
	if( regval & MMA_ENABLE )
		printk("\033[31mMMA_ENABLE\033[m ");
	if( regval & SD_EN )
		printk("\033[31mSD_EN\033[m ");
	if( regval & NC_EN )
		printk("\033[31mNC_EN\033[m ");
	printk("\n");

	printk("JOB_BL_CNT(0x0B)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_JOB_BL_CNT));
	printk("TR_BL_CNT(0x0C)=\t\t%04X\n", fcie_reg_read(pHost_st, FCIE_TR_BL_CNT));

    fcie_setbits(pHost_st, FCIE_JOB_BL_CNT, BIT15);
    fcie_setbits(pHost_st, FCIE_JOB_BL_CNT, BIT14);
    printk("\tMIU TR_BL_CNT(0x0C)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_TR_BL_CNT));

    fcie_clrbits(pHost_st, FCIE_JOB_BL_CNT, BIT14);
    printk("\tCARD TR_BL_CNT(0x0C)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_TR_BL_CNT));

    fcie_clrbits(pHost_st, FCIE_JOB_BL_CNT, BIT15);

	printk("RSP_SIZE(0x0D)=\t\t%04X\n", fcie_reg_read(pHost_st, FCIE_RSP_SIZE));
	printk("CMD_SIZE(0x0E)=\t\t%04X\n", fcie_reg_read(pHost_st, FCIE_CMD_SIZE));
	printk("CARD_WD_CNT(0x0F)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_CARD_WD_CNT));

	regval = fcie_reg_read(pHost_st, FCIE_SD_MODE);
	printk("SD_MODE(0x10)=\t\t%04X ", regval);
	if( regval & SD_CLK_EN )
		printk("\033[31mSD_CLK_EN\033[m ");
	if( regval & SD_DAT_LINE0 )
		printk("\033[31mSD_DAT_LINE0\033[m ");
	if( regval & SD_DAT_LINE1 )
		printk("\033[31mSD_DAT_LINE1\033[m ");
	if( regval & SD_CS_EN )
		printk("\033[31mSD_CS_EN\033[m ");
	if( regval & SD_DEST )
		printk("\033[31mCIFC\033[m ");
	else
		printk("\033[31mCIFD\033[m ");
	if( regval & SD_DATSYNC )
		printk("\033[31mSD_DATSYNC\033[m ");
	if( regval & SDIO_SD_BUS_SW )
		printk("\033[31mSDIO_SD_BUS_SW!!!\033[m ");
    if( regval & SD_DMA_RD_CLK_STOP )
		printk("\033[31mSD_DMA_RD_CLK_STOP!!!\033[m ");
	printk("\n");

	regval = fcie_reg_read(pHost_st, FCIE_SD_CTL);
	printk("SD_CTL(0x11)=\t\t%04X ", regval);
	if( regval & SD_RSPR2_EN )
		printk("\033[31mSD_RSPR2_EN\033[m ");
	if( regval & SD_RSP_EN )
		printk("\033[31mSD_RSP_EN\033[m ");
	if( regval & SD_CMD_EN )
		printk("\033[31mSD_CMD_EN\033[m ");
	if( regval & SD_DTRX_EN )
		printk("\033[31mSD_DTRX_EN\033[m ");
	if( regval & SD_DTRX_DIR )
		printk("\033[31mSD_DTRX_DIR\033[m ");
	printk("\n");

	regval = fcie_reg_read(pHost_st, FCIE_SD_STS);
	printk("SD_STS(0x12)=\t\t%04X ", regval);
	if( regval & SD_DAT_CERR )
		printk("\033[31mSD_DAT_CERR\033[m ");
	if( regval & SD_DAT_STSERR )
		printk("\033[31mSD_DAT_STSERR\033[m ");
	if( regval & SD_DAT_STSNEG )
		printk("\033[31mSD_DAT_STSNEG\033[m ");
	if( regval & SD_DAT_NORSP )
		printk("\033[31mSD_DAT_NORSP\033[m ");
	if( regval & SD_CMDRSP_CERR )
		printk("\033[31mSD_CMDRSP_CERR\033[m ");
    if( regval & SD_WR_PRO_N )
		printk("\033[31mSD_WR_PRO_N\033[m ");
    if( regval & SD_CARD_BUSY )
		printk("\033[31mSD_CARD_BUSY\033[m ");
	printk("\n");

	regval = fcie_reg_read(pHost_st, SDIO_BLK_SIZE);
	printk("SDIO_CTL(0x1B)=\t\t%04X ", regval);
	if( regval & SDIO_BLK_MOD_EN )
		printk("\033[31mSDIO_BLK_MOD_EN\033[m ");
	printk("\n");

	printk("SDIO_MIU_DMA0(0x1C)=\t%04X\n", fcie_reg_read(pHost_st, SDIO_MIU_DMA0));
	printk("SDIO_MIU_DMA1(0x1D)=\t%04X\n", fcie_reg_read(pHost_st, SDIO_MIU_DMA1));
	printk("MIU_OFFSET(0x2E)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_DMA_OFFSET));

	regval = fcie_reg_read(pHost_st, EMMC_BOOT_CONFIG);
	printk("EMMC_BOOT_CONFIG(0x2F)=\t%04X ", regval);
	if( regval & BOOT_STG2_EN )
		printk("\033[31mBOOT_STG2_EN\033[m ");
	if( regval & BOOT_END_EN )
		printk("\033[31mBOOT_END_EN\033[m ");
	printk("\n");

	printk("TEST_MODE(0x30)=\t\t%04X\n", fcie_reg_read(pHost_st, FCIE_TEST_MODE));

    #if (defined(CONFIG_MSTAR_AMBER3) || \
         defined(CONFIG_MSTAR_AGATE) || \
         defined(CONFIG_MSTAR_EAGLE))
    printk("SD_DDR_CNT(0x33)=\t\t%04X\n", fcie_reg_read(pHost_st, FCIE_SD_TOGGLE_CNT));

    regval = fcie_reg_read(pHost_st, FCIE_MISC);
    printk("MISC(0x36)=\t\t\t%04X ", regval);
    if( regval & DQS_MODE_DELAY_2T )
        printk("\033[31mDQS_MODE_DELEY_2T\033[m ");
    if( regval & DQS_MODE_DELAY_1_5T )
        printk("\033[DQS_MODE_DELAY_1_5T\033[m ");
    if( regval & DQS_MODE_DELAY_2_5T )
        printk("\033[DQS_MODE_DELAY_2_5T\033[m ");
    if( regval & DQS_MODE_DELAY_1T )
        printk("\033[DQS_MODE_DELAY_NO_DELAY\033[m ");
    #endif

    printk("\nCHIPTOP:\n");
    for(i=0x0; i<0x80; i++)
    {
        if(i%8==0) printk("%02X: ", (i/8)*8);
        printk("0x%04X ", fcie_readw(GET_REG_ADDR(CHIPTOP_BASE, i)));
        if(i%8==7) printk(":%02X\r\n", ((i/8)*8)+7);
    }

    printk("\nFCIE:\n");
    for(i=0x0; i<0x60; i++)
    {
        if(i%8==0) printk("%02X: ", (i/8)*8);
        printk("0x%04X ", fcie_reg_read(pHost_st, i*4) );
        if(i%8==7) printk(":%02X\r\n", ((i/8)*8)+7);
    }

    printk("\nCIFC:\n");
    for(i=0x60; i<0x80; i++)
    {
        if(i%8==0) printk("%02X: ", (i/8)*8);
        printk("0x%04X ", fcie_reg_read(pHost_st, i*4) );
        if(i%8==7) printk(":%02X\r\n", ((i/8)*8)+7);
    }

	printk("\nDump FCIE3 debug ports:\n");
	for(i=0; i<=7; i++)
	{
		// Enable debug ports
		fcie_clrbits(pHost_st, FCIE_TEST_MODE, (BIT10|BIT9|BIT8));
		fcie_setbits(pHost_st, FCIE_TEST_MODE, i<<8);

		// Dump debug ports
		printk("\nDebug Mode \033[31m%d:\033[m\n", i);
		printk("DBus[15:0]=\033[34m%04X\033[m\n", fcie_reg_read(pHost_st, FCIE_DEBUG_BUS_LOW));
		printk("DBus[23:16]=\033[34m%04X\033[m\n", fcie_reg_read(pHost_st, FCIE_DEBUG_BUS_HIGH));
	}

}

#if (defined(CONFIG_MSTAR_AMBER3) || \
     defined(CONFIG_MSTAR_AGATE) || \
     defined(CONFIG_MSTAR_EAGLE))
static void mstar_mci_disable_stop_clock(struct mstar_mci_host *pHost_st)
{
    fcie_reg_write(pHost_st, FCIE_DEBUG_BUS_LOW, 0x4000);
}

void mstar_mci_reset_toggle_count(struct mstar_mci_host *pHost_st)
{
    fcie_reg_write(pHost_st, FCIE_DEBUG_BUS_LOW, 0x8000);
    udelay(1);
    fcie_reg_write(pHost_st, FCIE_DEBUG_BUS_LOW, 0x0000);
}

static void mstar_mci_set_toggle_count(struct mstar_mci_host *pHost_st)
{
    if( pHost_st->cmd->data->flags == MMC_DATA_READ )
        fcie_reg_write(pHost_st, FCIE_SD_TOGGLE_CNT, 0x111);
    else
        fcie_reg_write(pHost_st, FCIE_SD_TOGGLE_CNT, 0x11A);
}
#if defined(DDR_MODE_ENABLE) && DDR_MODE_ENABLE
static void mstar_mci_config_ddr_mode(struct mstar_mci_host *pHost_st)
{
    fcie_writew(0x0080, REG_EMMC_CONFIG); // Switch pad for eMMC DDR
    fcie_reg_write(pHost_st, EMMC_BOOT_CONFIG, 0x0300); // DDR
    fcie_reg_write(pHost_st, FCIE_MISC, DQS_MODE_DELAY_2_5T);
}
#endif
#endif

static void mstar_mci_pre_dma_read(struct mstar_mci_host *pHost_st)
{
    /* Define Local Variables */
    struct scatterlist *pSG_st = 0;
    struct mmc_command *pCmd_st = 0;
    struct mmc_data *pData_st = 0;

    int sglen = 0;
    u32 dmalen = 0;

    dma_addr_t dmaaddr = 0;

    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "mstar_mci_pre_dma_read parameter pHost_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    pr_debug("pre dma read\n");

    pCmd_st = pHost_st->cmd;
    if (!pCmd_st)
    {
        pr_debug("no command\n");
        return;
    }

    pData_st = pCmd_st->data;
    if (!pData_st)
    {
        pr_debug("no data\n");
        return;
    }

    sglen = dma_map_sg(mmc_dev(pHost_st->mmc), pData_st->sg, pData_st->sg_len, DMA_FROM_DEVICE);

    pSG_st = &pData_st->sg[0];

    dmaaddr = (u32)sg_dma_address(pSG_st);
    dmalen = sg_dma_len(pSG_st);

    #if (defined(CONFIG_MSTAR_AMBER3) || \
         defined(CONFIG_MSTAR_AGATE) || \
         defined(CONFIG_MSTAR_EAGLE))
    if( dmaaddr >= MSTAR_MIU1_BUS_BASE)
    {
        dmaaddr -= MSTAR_MIU1_BUS_BASE;
        fcie_setbits(pHost_st, FCIE_MIU_DMA1, MIU1_SELECT);
    }
    else
    {
        dmaaddr -= MSTAR_MIU0_BUS_BASE;
        fcie_clrbits(pHost_st, FCIE_MIU_DMA1, MIU1_SELECT);
    }
    #endif

    dmalen /= 512;
    fcie_reg_write(pHost_st,FCIE_JOB_BL_CNT, (sg_dma_len(pSG_st)%512) ? (dmalen+1) : (dmalen));

    fcie_reg_write(pHost_st,SDIO_MIU_DMA0,(((u32)dmaaddr) & 0xFFFF));
    fcie_reg_write(pHost_st,SDIO_MIU_DMA1,(((u32)dmaaddr) >> 16));

    pr_debug("pre dma read done\n");

    #if defined(MMC_REQ_LENGTH_TEST) && MMC_REQ_LENGTH_TEST
    if( sglen == 1 )
    {
        if( pData_st->bytes_xfered <= 512 )
            total_512B_read_count++;
        else if( pData_st->bytes_xfered <= 1024 )
            total_1KB_read_count++;
        else if( pData_st->bytes_xfered <= 2048 )
            total_2KB_read_count++;
        else if( pData_st->bytes_xfered <= 4096 )
            total_4KB_read_count++;
        else if( pData_st->bytes_xfered <= 8192 )
            total_8KB_read_count++;
        else if( pData_st->bytes_xfered <= 16384 )
            total_16KB_read_count++;
        else if( pData_st->bytes_xfered <= 32768 )
            total_32KB_read_count++;
        else if( pData_st->bytes_xfered <= 65536 )
            total_64KB_read_count++;
        else if( pData_st->bytes_xfered <= (128 * 1024) )
            total_128KB_read_count++;
        else if( pData_st->bytes_xfered <= (256 * 1024) )
            total_256KB_read_count++;
        else if( pData_st->bytes_xfered <= (512 * 1024) )
            total_512KB_read_count++;
        else if( pData_st->bytes_xfered <= (1024 * 1024) )
            total_1MB_read_count++;
        else if( pData_st->bytes_xfered <= (2048 * 1024) )
            total_2MB_read_count++;

        total_read_count++;
    }
    #endif
}

static void mstar_mci_post_dma_read(struct mstar_mci_host *pHost_st)
{
    /* Define Local Variables */
    struct mmc_command	*pCmd_st = 0;
    struct mmc_data		*pData_st = 0;
    struct scatterlist	*pSG_st = 0;

    int i;
    u32 dmalen = 0;

    dma_addr_t dmaaddr = 0;

    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "mstar_mci_post_dma_read parameter pHost_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    pr_debug("post dma read\n");

    pCmd_st = pHost_st->cmd;

    if (!pCmd_st)
    {
        pr_debug("no command\n");
        return;
    }

    pData_st = pCmd_st->data;
    if (!pData_st)
    {
        pr_debug("no data\n");
        return;
    }

    pSG_st = &(pData_st->sg[0]);
    pData_st->bytes_xfered += pSG_st->length;

    for(i=1; i<pData_st->sg_len; i++)
    {
        pSG_st = &(pData_st->sg[i]);

        dmaaddr = sg_dma_address(pSG_st);
        dmalen = sg_dma_len(pSG_st);

        #if (defined(CONFIG_MSTAR_AMBER3) || \
             defined(CONFIG_MSTAR_AGATE) || \
             defined(CONFIG_MSTAR_EAGLE))
        if( dmaaddr >= MSTAR_MIU1_BUS_BASE)
        {
            dmaaddr -= MSTAR_MIU1_BUS_BASE;
            fcie_setbits(pHost_st, FCIE_MIU_DMA1, MIU1_SELECT);
        }
        else
        {
            dmaaddr -= MSTAR_MIU0_BUS_BASE;
            fcie_clrbits(pHost_st, FCIE_MIU_DMA1, MIU1_SELECT);
        }
        #endif

        fcie_reg_write(pHost_st,FCIE_JOB_BL_CNT,(dmalen/512));

        fcie_reg_write(pHost_st,SDIO_MIU_DMA0,(((u32)dmaaddr) & 0xFFFF));
        fcie_reg_write(pHost_st,SDIO_MIU_DMA1,(((u32)dmaaddr) >> 16));

        //wait clock sync
        udelay(1);

        wait_fifoclk_ready(pHost_st);

        #if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
        fcie_setbits(pHost_st, FCIE_MIE_INT_EN, MMA_LAST_DONE);
        #endif

        fcie_setbits(pHost_st, FCIE_MIE_PATH_CTL, MMA_ENABLE);

        fcie_reg_write(pHost_st, FCIE_SD_CTL, SD_DTRX_EN);

        if( mstar_mci_wait_event(pHost_st, MMA_LAST_DONE, LOOP_DELAY_WAIT_COMPLETE) == LOOP_DELAY_WAIT_COMPLETE )
        {
            printk("%s: Wait MMA last done timeout!\n", __FUNCTION__);
        }

        pData_st->bytes_xfered += pSG_st->length;
    }

    dma_unmap_sg(mmc_dev(pHost_st->mmc), pData_st->sg, pData_st->sg_len, DMA_FROM_DEVICE);

}

static s32 mstar_mci_handle_cmdrdy(struct mstar_mci_host *pHost_st)
{
    struct mmc_command	*pCmd_st = 0;
    struct mmc_data		*pData_st = 0;
    struct scatterlist	*pSG_st = 0;

    int i;
    int sglen;
    u32 dmalen = 0;

    dma_addr_t dmaaddr = 0;

    #if defined(DMA_TIME_TEST) && DMA_TIME_TEST
    #if !(defined(DMA_TIME_TEST_READ) && DMA_TIME_TEST_READ)
    u32 u32Ticks = 0;
    #endif
    #endif

    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "mstar_mci_handle_cmdrdy parameter pHost_st is NULL\n");
        return -EINVAL;
    }
    /* End Parameter out-of-bound check */

    if (!pHost_st->cmd)
    {
        return 1;
    }
    pCmd_st = pHost_st->cmd;

    if (!pCmd_st->data)
    {
        return 1;
    }
    pData_st = pCmd_st->data;

    #if defined(DMA_TIME_TEST) && DMA_TIME_TEST
    #if !(defined(DMA_TIME_TEST_READ) && DMA_TIME_TEST_READ)
    mstar_mci_PIU_Timer1_Start();
    #endif
    #endif

    if (pData_st->flags & MMC_DATA_WRITE)
    {
        sglen = dma_map_sg(mmc_dev(pHost_st->mmc), pData_st->sg, pData_st->sg_len, DMA_TO_DEVICE);

        for(i=0; i<sglen; i++)
        {
            pSG_st = &(pData_st->sg[i]);

            dmaaddr = sg_dma_address(pSG_st);
            dmalen = sg_dma_len(pSG_st);

            #if (defined(CONFIG_MSTAR_AMBER3) || \
                 defined(CONFIG_MSTAR_AGATE) || \
                 defined(CONFIG_MSTAR_EAGLE))
            if( dmaaddr >= MSTAR_MIU1_BUS_BASE)
            {
                dmaaddr -= MSTAR_MIU1_BUS_BASE;
                fcie_setbits(pHost_st, FCIE_MIU_DMA1, MIU1_SELECT);
            }
            else
            {
                dmaaddr -= MSTAR_MIU0_BUS_BASE;
                fcie_clrbits(pHost_st, FCIE_MIU_DMA1, MIU1_SELECT);
            }
            #endif

            fcie_reg_write(pHost_st, FCIE_JOB_BL_CNT, (dmalen / 512));

            fcie_reg_write(pHost_st, SDIO_MIU_DMA0, (dmaaddr & 0xFFFF));
            fcie_reg_write(pHost_st, SDIO_MIU_DMA1, (dmaaddr >> 16));

            fcie_reg_write(pHost_st, FCIE_MMA_PRI_REG, (fcie_reg_read(pHost_st, FCIE_MMA_PRI_REG) | JOB_RW_DIR));

            //wait clock sync
            udelay(1);

            wait_fifoclk_ready(pHost_st);

            #if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
            fcie_setbits(pHost_st, FCIE_MIE_INT_EN, CARD_DMA_END);
            #endif

            fcie_reg_write(pHost_st, FCIE_MIE_PATH_CTL, (fcie_reg_read(pHost_st, FCIE_MIE_PATH_CTL) | MMA_ENABLE));
            fcie_reg_write(pHost_st, FCIE_SD_CTL, (fcie_reg_read(pHost_st, FCIE_SD_CTL) | (SD_DTRX_DIR | SD_DTRX_EN)));

            if( mstar_mci_wait_event(pHost_st, CARD_DMA_END, LOOP_DELAY_WAIT_COMPLETE) == LOOP_DELAY_WAIT_COMPLETE )
            {
                printk("%s: Wait MMA last done timeout!\n", __FUNCTION__);
            }

            pData_st->bytes_xfered += pSG_st->length;
        }

        dma_unmap_sg(mmc_dev(pHost_st->mmc), pData_st->sg, pData_st->sg_len, DMA_TO_DEVICE);

        #if defined(DMA_TIME_TEST) && DMA_TIME_TEST
        #if !(defined(DMA_TIME_TEST_READ) && DMA_TIME_TEST_READ)
        u32Ticks = mstar_mci_PIU_Timer1_End();

        totaldmalen += pData_st->bytes_xfered;
        totaldmatime += u32Ticks;
        #endif
        #endif

        #if defined(MMC_REQ_LENGTH_TEST) && MMC_REQ_LENGTH_TEST
        if( pData_st->bytes_xfered <= 512 )
            total_512B_write_count++;
        else if( pData_st->bytes_xfered <= 1024 )
            total_1KB_write_count++;
        else if( pData_st->bytes_xfered <= 2048 )
            total_2KB_write_count++;
        else if( pData_st->bytes_xfered <= 4096 )
            total_4KB_write_count++;
        else if( pData_st->bytes_xfered <= 8192 )
            total_8KB_write_count++;
        else if( pData_st->bytes_xfered <= 16384 )
            total_16KB_write_count++;
        else if( pData_st->bytes_xfered <= 32768 )
            total_32KB_write_count++;
        else if( pData_st->bytes_xfered <= 65536 )
            total_64KB_write_count++;
        else if( pData_st->bytes_xfered <= (128 * 1024) )
            total_128KB_write_count++;
        else if( pData_st->bytes_xfered <= (256 * 1024) )
            total_256KB_write_count++;
        else if( pData_st->bytes_xfered <= (512 * 1024) )
            total_512KB_write_count++;
        else if( pData_st->bytes_xfered <= (1024 * 1024) )
            total_1MB_write_count++;
        else if( pData_st->bytes_xfered <= (2048 * 1024) )
            total_2MB_write_count++;

        total_write_count++;
        #endif

        return 1;

    }

    return 0;
}

static void mstar_mci_switch_sd(struct mstar_mci_host *pHost_st)
{
    fcie_writew(fcie_readw(REG_NAND_CONFIG)&0xFF3F, REG_NAND_CONFIG);

    #if defined(CONFIG_MMC_MSTAR_MMC_SD) && CONFIG_MMC_MSTAR_MMC_SD
    /* 1. Switch interface to SD */
    fcie_writew(fcie_readw(REG_PCM_CONFIG)&0xFFE7, REG_PCM_CONFIG);
    fcie_reg_write(pHost_st, FCIE_MIE_PATH_CTL, SD_EN);
    fcie_writew(fcie_readw(REG_SD_CONFIG)|BIT5, REG_SD_CONFIG);
    #endif

    /* 2. Set SD clock */
	fcie_clrbits(pHost_st, FCIE_SD_MODE, SD_CLK_EN);
    fcie_writew(pHost_st->sd_clk, REG_CLK_NFIE);

    /* 3. Set SD mode */
    fcie_reg_write(pHost_st, FCIE_SD_MODE, pHost_st->sd_mod);
    /* 4. Disable SDIO register */
    fcie_reg_write(pHost_st, SDIO_MIU_DMA0, 0);
    fcie_reg_write(pHost_st, SDIO_MIU_DMA1, 0);

    // Clear MIE event (write 1 clear)
    fcie_reg_write(pHost_st, FCIE_MIE_EVENT, SD_ALL_INT);

    clear_fcie_status(pHost_st, FCIE_SD_STS, SD_STS_ERRORS);

}

static void mstar_mci_enable(struct mstar_mci_host *pHost_st)
{
    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "mstar_mci_enable parameter pHost_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    /* init registers */
    fcie_reg_write(pHost_st, FCIE_MIE_INT_EN, 0);
    fcie_reg_write(pHost_st, FCIE_SD_CTL, 0);
    fcie_reg_write(pHost_st, FCIE_SD_MODE, 0);

    /* clear events */
    fcie_reg_write(pHost_st, FCIE_SD_STS, SD_STS_ERRORS);

    #if (defined(CONFIG_MSTAR_AMBER3) || \
         defined(CONFIG_MSTAR_AGATE) || \
         defined(CONFIG_MSTAR_EAGLE))

    #if defined(BYPASS_MODE_ENABLE) && BYPASS_MODE_ENABLE
    fcie_writew((fcie_readw(REG_EMMC_CONFIG)&~(BIT6|BIT7))|BIT6, REG_EMMC_CONFIG);
    fcie_reg_write(pHost_st, EMMC_BOOT_CONFIG, 0x0400);
    #else
    fcie_writew((fcie_readw(REG_EMMC_CONFIG)&~(BIT6|BIT7))|BIT7, REG_EMMC_CONFIG);
    fcie_reg_write(pHost_st, EMMC_BOOT_CONFIG, 0x0200);
    #endif

	fcie_writew(0xA0, REG_PCM_CONFIG);
    // Set MIU select determined by FCIE, not MIU itself
    fcie_writew(fcie_readw(0x1f200de8)|BIT9, 0x1f200de8);

    #endif

    #if defined(CONFIG_MMC_MSTAR_MMC_SD) && CONFIG_MMC_MSTAR_MMC_SD
    fcie_reg_write(pHost_st, FCIE_CARD_INT_EN, SD_STS_EN);
    fcie_writew(fcie_readw(REG_SD_CONFIG)|BIT5, REG_SD_CONFIG);
    if (!(fcie_reg_read(pHost_st, FCIE_CARD_DET) & SD_DET_N))
    {
        pHost_st->present = 1;
        pr_debug("SD card present!\n");
    }
    else
    {
        pHost_st->present = 0;
        pr_debug("SD card not present!\n");
    }
    #endif
}

static void mstar_mci_disable(struct mstar_mci_host *pHost_st)
{
    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "mstar_mci_disable parameter pHost_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    fcie_reg_write(pHost_st, FCIE_MIE_PATH_CTL, ~(MMA_ENABLE | SD_EN));
}

static void mstar_mci_send_command(struct mstar_mci_host *pHost_st,
                                      struct mmc_command *pCmd_st)
{
    /* Define Local Variables */
    #if defined(CMD_ARG_DEBUG) && CMD_ARG_DEBUG
    int i;
    u8 pTemp[6];
    #endif
    struct mmc_data *pData_st;
    u32 sd_ctl = 0;
	u32 status;

    #if defined(DMA_TIME_TEST) && DMA_TIME_TEST
    #if (defined(DMA_TIME_TEST_READ) && DMA_TIME_TEST_READ)
    u32 u32Ticks = 0;
    #endif
    #endif

    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "mstar_mci_send_command parameter pHost_st is NULL\n");
        return;
    }

    if (!pCmd_st)
    {
        printk(KERN_WARNING "mstar_mci_send_command parameter pCmd_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    pr_debug("mstar_mci_send_command:(%d)\n", pCmd_st->opcode);

    //fcie_reg_write(pHost_st, FCIE_SD_CTL, sd_ctl);

    #if defined(CONFIG_MMC_MSTAR_MMC_SD) && CONFIG_MMC_MSTAR_MMC_SD
    if (!pHost_st->present)
    {
        pCmd_st->error = -ENODEV;
        mmc_request_done(pHost_st->mmc, pHost_st->request);
        return;
    }
    #endif

    mstar_mci_switch_sd(pHost_st);

    pData_st = pCmd_st->data;

    pHost_st->cmd = pCmd_st;
    //fcie_reg_write(pHost_st, FCIE_SD_CTL, sd_ctl);

    if (pData_st)
    {
        pr_debug("pData_st!=0\n");

        if (pData_st->blksz & 0x3)
        {
            printk("\033[31m Unsupported block size \033[m\n");
            pCmd_st->error = -EINVAL;
            mmc_request_done(pHost_st->mmc, pHost_st->request);
            return;
        }

        if (pData_st->flags & MMC_DATA_READ)
        {
            sd_ctl |= SD_DTRX_EN;

            // Enable stoping read clock when using scatter list DMA
            if( (pData_st->sg_len > 1) && (pCmd_st->opcode == 18) )
                fcie_setbits(pHost_st, FCIE_SD_MODE, SD_DMA_RD_CLK_STOP);
            else
                fcie_clrbits(pHost_st, FCIE_SD_MODE, SD_DMA_RD_CLK_STOP);
        }
    }

    sd_ctl |= SD_CMD_EN;
    
    if (pData_st)
    {
        //wait_fifoclk_ready(pHost_st);

        fcie_reg_write(pHost_st, FCIE_SD_MODE, fcie_reg_read(pHost_st, FCIE_SD_MODE) & ~SD_DEST);

        pData_st->bytes_xfered = 0;

        fcie_reg_write(pHost_st, SDIO_BLK_SIZE, (SDIO_BLK_MOD_EN|(u16)pData_st->blksz));
        pr_debug("[SDIO_BLK_SIZE]:%X\n", fcie_reg_read(pHost_st, SDIO_BLK_SIZE));

        if (pData_st->flags & MMC_DATA_READ)
        {
            /* Handle a read */
            #if 0//defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
            // We should wait for the MMA_LAST_DONE for read to make sure all data has been in DRAM
            if( (pCmd_st->opcode != 17) && (pCmd_st->opcode != 18) )
                mie_int |= MMA_LAST_DONE;
            #endif

            mstar_mci_pre_dma_read(pHost_st);

            fcie_reg_write(pHost_st, FCIE_MMA_PRI_REG, (fcie_reg_read(pHost_st, FCIE_MMA_PRI_REG) & (~JOB_RW_DIR)));

            //wait clock sync
            udelay(1);

            wait_fifoclk_ready(pHost_st);

            fcie_reg_write(pHost_st, FCIE_MIE_PATH_CTL, (fcie_reg_read(pHost_st, FCIE_MIE_PATH_CTL) | MMA_ENABLE));
        }
    }

    pr_debug("FCIE_SD_STS = %08X\n",fcie_reg_read(pHost_st, FCIE_SD_STS));

    #if 1
    if (pCmd_st->opcode!=12 && waitD0High(pHost_st, LOOP_DELAY_WAIT_COMPLETE))
    {
        printk(KERN_WARNING "Card busy!!\n");
        pCmd_st->error = -ETIMEDOUT;
        return;
    }
    #endif

    /* Send the command and then enable the PDC - not the other way round as */
    /* the data sheet says */
    fcie_writew((((pCmd_st->arg >> 24) << 8) | (0x40 | pCmd_st->opcode)), FCIE1_BASE);
    fcie_writew(((pCmd_st->arg & 0xFF00) | ((pCmd_st->arg >> 16) & 0xFF)), FCIE1_BASE + 4);
    fcie_writew(((fcie_readw(FCIE1_BASE + 8) & 0xFF00) + (pCmd_st->arg & 0xFF)), FCIE1_BASE + 8);

    #if defined(CMD_ARG_DEBUG) && CMD_ARG_DEBUG
    printk("CMD: ");
    for (i=0; i < 6; i++)
    {
        pTemp[(3 - (i % 4)) + (4 * (i / 4))] =
            (u8)(fcie_readw(FCIE1_BASE + (((i + 1) / 2) * 4)) >> (8 * ((i + 1) % 2)));
        printk("%02X ", pTemp[i]);
    }
    printk("\n");
    #endif

    fcie_reg_write(pHost_st, FCIE_CMD_SIZE, 5);

    if (mmc_resp_type(pCmd_st) == MMC_RSP_NONE)
    {
        sd_ctl &= ~SD_RSP_EN;
        fcie_reg_write(pHost_st, FCIE_RSP_SIZE, 0);
    }
    else
    {
        if (mmc_resp_type(pCmd_st) == MMC_RSP_R2)
        {
            sd_ctl |= SD_RSPR2_EN;
            fcie_reg_write(pHost_st, FCIE_RSP_SIZE, 16); /* (136-8)/8 */
        }
        else
        {
            fcie_reg_write(pHost_st, FCIE_RSP_SIZE, 5); /*(48-8)/8 */
        }

        sd_ctl |= SD_RSP_EN;
    }

    #if (defined(CONFIG_MSTAR_AMBER3) || \
         defined(CONFIG_MSTAR_AGATE) || \
         defined(CONFIG_MSTAR_EAGLE))
    if( pHost_st->mmc->caps & MMC_CAP_1_8V_DDR )
    {
        if( (pCmd_st->opcode == 17) || (pCmd_st->opcode == 18) )
        {
            mstar_mci_set_toggle_count(pHost_st);
            mstar_mci_reset_toggle_count(pHost_st);
        }
        else if( (pCmd_st->opcode == 24) || (pCmd_st->opcode == 25) )
        {
            mstar_mci_set_toggle_count(pHost_st);
            mstar_mci_reset_toggle_count(pHost_st);
            mstar_mci_disable_stop_clock(pHost_st);
        }
        else if( pCmd_st->opcode == 12 )
            mstar_mci_disable_stop_clock(pHost_st);
    }
    #endif

    simple_debug("Sending command %d as %08X, mod %08X, arg = %08X\n",
             pCmd_st->opcode, sd_ctl, fcie_reg_read(pHost_st, FCIE_SD_MODE), pCmd_st->arg);
    simple_debug("Clock:0x%X\n", fcie_readw(REG_CLK_NFIE));

    #if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
	fcie_reg_write(pHost_st, FCIE_MIE_INT_EN, SD_CMD_ENDE);
	pr_debug("[FCIE_MIE_INT_EN]:%X\n", fcie_reg_read(pHost_st, FCIE_MIE_INT_EN));
	pr_debug("[FCIE_MIE_EVENT]:%X\n", fcie_reg_read(pHost_st, FCIE_MIE_EVENT));
    #endif

    #if defined(DMA_TIME_TEST) && DMA_TIME_TEST
    #if defined(DMA_TIME_TEST_READ) && DMA_TIME_TEST_READ
    if( pCmd_st->opcode== 18 )
        mstar_mci_PIU_Timer1_Start();
    #endif
    #endif

	fcie_reg_write(pHost_st, FCIE_SD_CTL, sd_ctl);

    if( mstar_mci_wait_event(pHost_st, SD_CMD_END, LOOP_DELAY_WAIT_COMPLETE) == LOOP_DELAY_WAIT_COMPLETE )
    {
        printk("%s:[emmc_error], Wait CMD END timeout!\n", __FUNCTION__);
    }
    else
    {
        status = fcie_reg_read(pHost_st, FCIE_SD_STS);

        if (status & SD_DAT_NORSP)
        {
            if( (pCmd_st->opcode == 24) || (pCmd_st->opcode == 25) )
            {
                 printk("%s:[emmc_error], cmd %d no rsp\n", __FUNCTION__, pCmd_st->opcode);
                 fcie_reg_dump(pHost_st);
			     mstar_mci_completed_command(pHost_st);
			     return;
            }
            else if( (pCmd_st->opcode == 17) || (pCmd_st->opcode == 18) )
            {
                 printk("%s:[emmc_error], cmd %d no rsp\n", __FUNCTION__, pCmd_st->opcode);
                 fcie_reg_dump(pHost_st);
                 pCmd_st->error = -EIO;
            }
        }

		if( mstar_mci_handle_cmdrdy(pHost_st) )
        {
            pr_debug("Completed command\n");

            mstar_mci_completed_command(pHost_st);

            return;
        }
        else
        {
            pr_debug("\033[34mIrq no complete!\033[0m\n");

            if( pData_st->flags & MMC_DATA_READ )
            {
                #if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
                // We should wait for the MMA_LAST_DONE for read to make sure all data has been in DRAM
                //if( (pCmd_st->opcode == 17) || (pCmd_st->opcode == 18) )
                    fcie_setbits(pHost_st, FCIE_MIE_INT_EN, MMA_LAST_DONE);
                #endif

                if( mstar_mci_wait_event(pHost_st, MMA_LAST_DONE, LOOP_DELAY_WAIT_COMPLETE) == LOOP_DELAY_WAIT_COMPLETE )
                {
                    printk("%s:[emmc_error], Wait MMA last done timeout!\n", __FUNCTION__);
                    fcie_reg_write(pHost_st, FCIE_SD_CTL, 0);
                    mstar_mci_reset_FCIE(pHost_st);
                    pData_st->error = -ETIMEDOUT;
                    mstar_mci_completed_command(pHost_st);
                }
                else
                {
                    mstar_mci_post_dma_read(pHost_st);

                    mstar_mci_completed_command(pHost_st);

                    #if defined(DMA_TIME_TEST) && DMA_TIME_TEST
                    #if defined(DMA_TIME_TEST_READ) && DMA_TIME_TEST_READ
                    u32Ticks = mstar_mci_PIU_Timer1_End();

                    totaldmalen += pData_st->bytes_xfered;
                    totaldmatime += u32Ticks;
                    #endif
                    #endif

                    #if defined(MMC_REQ_LENGTH_TEST) && MMC_REQ_LENGTH_TEST
                    if( pData_st->bytes_xfered <= 512 )
                        total_512B_read_count++;
                    else if( pData_st->bytes_xfered <= 1024 )
                        total_1KB_read_count++;
                    else if( pData_st->bytes_xfered <= 2048 )
                        total_2KB_read_count++;
                    else if( pData_st->bytes_xfered <= 4096 )
                        total_4KB_read_count++;
                    else if( pData_st->bytes_xfered <= 8192 )
                        total_8KB_read_count++;
                    else if( pData_st->bytes_xfered <= 16384 )
                        total_16KB_read_count++;
                    else if( pData_st->bytes_xfered <= 32768 )
                        total_32KB_read_count++;
                    else if( pData_st->bytes_xfered <= 65536 )
                        total_64KB_read_count++;
                    else if( pData_st->bytes_xfered <= (128 * 1024) )
                        total_128KB_read_count++;
                    else if( pData_st->bytes_xfered <= (256 * 1024) )
                        total_256KB_read_count++;
                    else if( pData_st->bytes_xfered <= (512 * 1024) )
                        total_512KB_read_count++;
                    else if( pData_st->bytes_xfered <= (1024 * 1024) )
                        total_1MB_read_count++;
                    else if( pData_st->bytes_xfered <= (2048 * 1024) )
                        total_2MB_read_count++;

                    total_read_count++;
                    #endif
                }
            }
        }
    }

}

static void mstar_mci_process_next(struct mstar_mci_host *pHost_st)
{
    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "mstar_mci_process_next parameter pHost_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    if (!(pHost_st->flags & FL_SENT_COMMAND))
    {
        pr_debug("CMD%d\n", pHost_st->request->cmd->opcode);
        pHost_st->flags |= FL_SENT_COMMAND;

        mstar_mci_send_command(pHost_st, pHost_st->request->cmd);
    }
    else if ((!(pHost_st->flags & FL_SENT_STOP)) && pHost_st->request->stop)
    {
        pr_debug("CMD%d\n", pHost_st->request->stop->opcode);
        pHost_st->flags |= FL_SENT_STOP;
        mstar_mci_send_command(pHost_st, pHost_st->request->stop);
    }
    else
    {
        pr_debug("mmc_request_done\n");
        mmc_request_done(pHost_st->mmc, pHost_st->request);
    }

	//waitD0High(pHost_st, LOOP_DELAY_WAIT_COMPLETE);
}

static void mstar_mci_completed_command(struct mstar_mci_host *pHost_st)
{
    /* Define Local Variables */
    u8 *pTemp = 0;
    u32 status = 0, i = 0;
    struct mmc_command *pCmd_st = pHost_st->cmd;
    struct mmc_data *pData_st = pCmd_st->data;

    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "mstar_mci_completed_command parameter pHost_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    if( (pCmd_st->flags == MMC_RSP_R1B) || (pData_st && (pData_st->flags & MMC_DATA_WRITE)) ) // Save CPU time for waitD0
    {
        waitD0High(pHost_st, LOOP_DELAY_WAIT_COMPLETE);
    }

    pTemp = (u8*)&(pCmd_st->resp[0]);

    #if defined(CMD_RSP_DEBUG) && CMD_RSP_DEBUG
    printk("RSP: ");
    #endif
    for (i=0; i < 15; i++)
    {
        pTemp[(3 - (i % 4)) + (4 * (i / 4))] =
            (u8)(fcie_readw(FCIE1_BASE + (((i + 1) / 2) * 4)) >> (8 * ((i + 1) % 2)));
        #if defined(CMD_RSP_DEBUG) && CMD_RSP_DEBUG
        printk("%02X ", pTemp[i]);
        #endif
    }
    #if defined(CMD_RSP_DEBUG) && CMD_RSP_DEBUG
    printk("\n");
    #endif

    status = fcie_reg_read(pHost_st, FCIE_SD_STS);

    simple_debug( "Status = %08X [%08X %08X %08X %08X]\n",
              status, pCmd_st->resp[0], pCmd_st->resp[1], pCmd_st->resp[2], pCmd_st->resp[3]);

    if (status & SD_STS_ERRORS)
    {
        if ((status & SD_CMDRSP_CERR) && !(mmc_resp_type(pCmd_st) & MMC_RSP_CRC))
        {
            pCmd_st->error = 0;
        }
        else
        {
            if( (pCmd_st->opcode == 55) || ((pCmd_st->opcode == 8) && (pCmd_st->arg != 0)) )
            {
                pCmd_st->error = 0;
            }
            else
            {
                //printk(KERN_WARNING "r = %x\n", status);
                if (status & (SD_DAT_NORSP | SD_DAT_STSERR))
                {
                    printk("SD_STS=%04X\n", status);
                    pCmd_st->error = -ETIMEDOUT;
                }
                else if (status & (SD_CMDRSP_CERR | SD_DAT_CERR | SD_DAT_STSNEG))
                {
                    printk("SD_STS=%04X\n", status);
                    pCmd_st->error = -EILSEQ;
                }
                else
                {
                    printk("SD_STS=%04X\n", status);
                    pCmd_st->error = -EIO;
                }
                printk(KERN_ERR "Error detected and set to %d (cmd = %d, retries = %d)\n",
                     pCmd_st->error, pCmd_st->opcode, pCmd_st->retries);
            }

        }
    }
    else
    {
        pCmd_st->error = 0;
    }

    if((pCmd_st->opcode == 17) || (pCmd_st->opcode == 18) //read
        ||(pCmd_st->opcode == 24) || (pCmd_st->opcode == 25) //write
        ||(pCmd_st->opcode == 12))  //stop transmission
    {
       if(pCmd_st->resp[0] & eMMC_ERR_R1_31_0)
       {
         pCmd_st->error |= -EIO;
         printk(KERN_ERR "R1 response error %08x\n", pCmd_st->resp[0]);
         if(pCmd_st->opcode == 12)
            printk(KERN_ERR "opcode %08x\n", pCmd_st->opcode);
         else
            printk(KERN_ERR "opcode %08x, arg %08x, blocks %08x\n", pCmd_st->opcode, pCmd_st->arg, pCmd_st->data->blocks);
       }
    }    

    clear_fcie_status(pHost_st, FCIE_SD_STS, SD_STS_ERRORS);

    fcie_clrbits(pHost_st, FCIE_SD_MODE, SD_CLK_EN);

    mstar_mci_process_next(pHost_st);
}

static void mstar_mci_request(struct mmc_host *pMMC_st, struct mmc_request *pMRQ_st)
{
    /* Define Local Variables */
    struct mstar_mci_host *pHost_st = mmc_priv(pMMC_st);

    #if defined(REQ_TIME_TEST) && REQ_TIME_TEST
    u32 u32Ticks = 0;
    #endif

    /* Parameter out-of-bound check */
    if (!pMMC_st)
    {
        printk(KERN_WARNING "mstar_mci_request parameter pMMC_st is NULL\n");
        return;
    }

    if (!pMRQ_st)
    {
        printk(KERN_WARNING "mstar_mci_request parameter pMRQ_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    pHost_st->request = pMRQ_st;
    pHost_st->flags = 0;

    #if defined(REQ_TIME_TEST) && REQ_TIME_TEST
    #if defined(REQ_TIME_TEST_READ) && REQ_TIME_TEST_READ
    if( (pMRQ_st->cmd->data) && (pMRQ_st->cmd->data->flags & MMC_DATA_READ) )
    {
        mstar_mci_PIU_Timer1_Start();
    }
    #else
    if( (pMRQ_st->cmd->data) && (pMRQ_st->cmd->data->flags & MMC_DATA_WRITE) )
    {
        mstar_mci_PIU_Timer1_Start();
    }
    #endif
    #endif

    mstar_mci_process_next(pHost_st);

    #if defined(REQ_TIME_TEST) && REQ_TIME_TEST
    #if defined(REQ_TIME_TEST_READ) && REQ_TIME_TEST_READ
    if( (pMRQ_st->cmd->data) && (pMRQ_st->cmd->data->flags & MMC_DATA_READ) )
    {
        u32Ticks = mstar_mci_PIU_Timer1_End();

        totalreqlen += pMRQ_st->cmd->data->bytes_xfered;
        totalreqtime += u32Ticks;
    }
    #else
    if( (pMRQ_st->cmd->data) && (pMRQ_st->cmd->data->flags & MMC_DATA_WRITE) )
    {
        u32Ticks = mstar_mci_PIU_Timer1_End();

        totalreqlen += pMRQ_st->cmd->data->bytes_xfered;
        totalreqtime += u32Ticks;
    }
    #endif
    #endif

}

static void mstar_mci_set_ios(struct mmc_host *pMMC_st, struct mmc_ios *pIOS_st)
{
    /* Define Local Variables */
    s32 clkdiv = 0;
    u32 sd_mod = 0;
    u16 reg_mask = ~((u16)0x3F);

    struct mstar_mci_host *pHost_st = mmc_priv(pMMC_st);

    /* Parameter out-of-bound check */
    if (!pMMC_st)
    {
        printk(KERN_WARNING "mstar_mci_set_ios parameter pMMC_st is NULL\n");
        return;
    }

    if (!pIOS_st)
    {
        printk(KERN_WARNING "mstar_mci_set_ios parameter pIOS_st is NULL\n");
        return;
    }
    /* End Parameter out-of-bound check */

    pr_debug("MMC: mstar_mci_set_ios\n");

    down(&PfModeSem);

    sd_mod = 0;
    clkdiv =fcie_readw(REG_CLK_NFIE); /* High triggle level */

    pr_debug("bus_width:%d\n", pIOS_st->bus_width);
    pr_debug("clkdiv:%d\n", clkdiv);
    pr_debug("clock:%d\n", pIOS_st->clock);
    if (pIOS_st->clock == 0)
    {
        /* Disable the MCI controller */
        sd_mod &= ~(SD_CLK_EN | SD_CS_EN);
        clkdiv |= 0x01; /* disable clock */
    }
    else
    {
        /* Enable the MCI controller */
        #if (defined(SDR_MODE_ENABLE) && SDR_MODE_ENABLE) || (defined(DDR_MODE_ENABLE) && DDR_MODE_ENABLE)
        sd_mod |= (SD_CLK_EN | SD_CS_EN);
        #else
        sd_mod |= (SD_CLK_EN | SD_CS_EN | SD_DATSYNC);
        #endif

        clkdiv &= reg_mask;

        #if defined(CLK_48M) && CLK_48M
        if( pIOS_st->clock >= CLK_48MHz )
		{
            printk("FCIE3 set 48MHz...\n");
            clkdiv |= (BIT_FCIE_CLK_48M);
            #if (defined(CONFIG_MSTAR_AMBER3) || \
                 defined(CONFIG_MSTAR_AGATE) || \
                 defined(CONFIG_MSTAR_EAGLE))
            if( pHost_st->mmc->caps & MMC_CAP_1_8V_DDR )
                clkdiv |= (BIT_EMMC_CLK_192M);
            #endif
        }
		else
        #endif // CLK_48M

        #if defined(CLK_43M) && CLK_43M
        if( pIOS_st->clock >= CLK_43MHz )
        {
            printk("FCIE3 set 43MHz...\n");
            clkdiv |= (BIT_FCIE_CLK_43M);
        }
        else
        #endif // CLK_43M

        #if defined(CLK_40M) && CLK_40M
        if( pIOS_st->clock >= CLK_40MHz )
        {
			printk("FCIE3 set 40MHz...\n");
			clkdiv |= (BIT_FCIE_CLK_40M);
            #if (defined(CONFIG_MSTAR_AMBER3) || \
                 defined(CONFIG_MSTAR_AGATE) || \
                 defined(CONFIG_MSTAR_EAGLE))
            if( pHost_st->mmc->caps & MMC_CAP_1_8V_DDR )
                clkdiv |= (BIT_EMMC_CLK_160M);
            #endif
		}
		else
        #endif

        #if defined(CLK_36M) && CLK_36M
        if( pIOS_st->clock >= CLK_36MHz )
		{
			printk("FCIE3 set 36MHz...\n");
			clkdiv |= (BIT_FCIE_CLK_36M);
            #if (defined(CONFIG_MSTAR_AMBER3) || \
                 defined(CONFIG_MSTAR_AGATE) || \
                 defined(CONFIG_MSTAR_EAGLE))
            if( pHost_st->mmc->caps & MMC_CAP_1_8V_DDR )
                clkdiv |= (BIT_EMMC_CLK_144M);
            #endif
        }
        else
        #endif

        #if defined(CLK_32M) && CLK_32M
        if( pIOS_st->clock >= CLK_32MHz )
		{
            printk("FCIE3 set 32MHz...\n");
            clkdiv |= (BIT_FCIE_CLK_32M);
        }
        else
        #endif // CLK_32M

        #if defined(CLK_27M) && CLK_27M
        if( pIOS_st->clock >= CLK_27MHz )
        {
			printk("FCIE3 set 27MHz...\n");
			clkdiv |= (BIT_FCIE_CLK_27M);
            #if (defined(CONFIG_MSTAR_AMBER3) || \
                 defined(CONFIG_MSTAR_AGATE) || \
                 defined(CONFIG_MSTAR_EAGLE))
            if( pHost_st->mmc->caps & MMC_CAP_1_8V_DDR )
                clkdiv |= (BIT_EMMC_CLK_108M);
            #endif
        }
        else
        #endif

        #if defined(CLK_24M) && CLK_24M
        if( pIOS_st->clock >= CLK_24MHz )
        {
			printk("FCIE3 set 24MHz...\n");
			clkdiv |= (BIT_FCIE_CLK_24M);
        }
        else
        #endif

        #if defined(CLK_20M) && CLK_20M
        if( pIOS_st->clock >= CLK_20MHz )
		{
			printk("FCIE3 set 20MHz...\n");
			clkdiv |= (BIT_FCIE_CLK_20M);
            #if (defined(CONFIG_MSTAR_AMBER3) || \
                 defined(CONFIG_MSTAR_AGATE) || \
                 defined(CONFIG_MSTAR_EAGLE))
            if( pHost_st->mmc->caps & MMC_CAP_1_8V_DDR )
                clkdiv |= (BIT_EMMC_CLK_80M);
            #endif
        }
        else
        #endif
		{
			printk("FCIE3 set 300KHz...\n");
            clkdiv |= (BIT_FCIE_CLK_300K);
        }

		fcie_clrbits(pHost_st, FCIE_SD_MODE, SD_CLK_EN);
        fcie_writew(clkdiv, REG_CLK_NFIE);

        pHost_st->sd_clk = (u16)clkdiv;
        pr_debug("[clkdiv]=%d\n", clkdiv);
        pr_debug("[pHost_st->sd_clk]=%d\n", pHost_st->sd_clk);

    }

	if (pIOS_st->bus_width == MMC_BUS_WIDTH_8)
    {
        pr_debug("\033[31mMMC: Setting controller bus width to 8\033[m\n");
        sd_mod = (sd_mod & ~SD_DAT_LINE) | SD_DAT_LINE1;
    }
    else if (pIOS_st->bus_width == MMC_BUS_WIDTH_4)
    {
        pr_debug("\033[31mMMC: Setting controller bus width to 4\033[m\n");
        sd_mod = (sd_mod & ~SD_DAT_LINE) | SD_DAT_LINE0;
    }
    else
    {
        pr_debug("MMC: Setting controller bus width to 1\n");
        sd_mod = (sd_mod & ~SD_DAT_LINE);
    }

    #if defined(DDR_MODE_ENABLE) && DDR_MODE_ENABLE
    if( pMMC_st->ios.ddr != 0 )
        mstar_mci_config_ddr_mode(pHost_st);
    #endif

    pr_debug("[FCIE_SD_MODE]=%x\n", sd_mod);
    pHost_st->sd_mod = sd_mod;

    up(&PfModeSem);
}

#if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
static irqreturn_t mstar_mci_irq(s32 irq, void *pDevID)
{
    struct mstar_mci_host *pHost_st = pDevID;
    volatile u32 mie_evt = fcie_reg_read(pHost_st, FCIE_MIE_EVENT);
	volatile u32 mie_en = fcie_reg_read(pHost_st, FCIE_MIE_INT_EN);
	volatile u32 u32_wake=0;

    #if defined(CONFIG_MMC_MSTAR_MMC_SD) && CONFIG_MMC_MSTAR_MMC_SD
    u32 card_event, card_int_en, present=0;

    /* Parameter out-of-bound check */
    if (!pDevID)
    {
        printk(KERN_WARNING "mstar_mci_irq parameter pDevID is NULL\n");
        return -EINVAL;
    }
    /* End Parameter out-of-bound check */

    card_event = fcie_reg_read(pHost_st, FCIE_CARD_EVENT);
    card_int_en = fcie_reg_read(pHost_st, FCIE_CARD_INT_EN);
    if ((card_event & SD_STS_CHG)&&
        (card_int_en & SD_STS_EN))
    {
        mdelay(10);
        present = !(fcie_reg_read(pHost_st, FCIE_CARD_DET) & SD_DET_N);
        printk("%s: card %s\n", mmc_hostname(pHost_st->mmc),
               present ? "insert" : "remove");

        pHost_st->present = present;
        mmc_detect_change(pHost_st->mmc, msecs_to_jiffies(1));
        fcie_reg_write(pHost_st, FCIE_CARD_EVENT, card_event);
        fcie_reg_write(pHost_st, FCIE_SD_STS, SD_STS_ERRORS);
    }

    if (fcie_reg_read(pHost_st, FCIE_MIE_PATH_CTL) & NC_EN)
    {
        pr_debug("irq, nc_en!\n");
        return IRQ_HANDLED;
    }
    #endif

    if (atomic_read(&emmc_in_use) == 0)
        return IRQ_NONE;
	
    mie_evt = mie_evt & mie_en;
	
    if( mie_evt & SD_CMD_END )
    {
        fcie_clrbits(pHost_st, FCIE_MIE_INT_EN, SD_CMD_END);

        fcie_int = 1;
        wake_up(&fcie_wait);
		u32_wake = 1;
    }

    if( mie_evt & MMA_LAST_DONE )
    {
        fcie_clrbits(pHost_st, FCIE_MIE_INT_EN, MMA_LAST_DONE);

        if(0 == u32_wake)
		{
			fcie_int = 1;
			wake_up(&fcie_wait);
			u32_wake = 1;
		}
    }

    if( mie_evt & CARD_DMA_END )
    {
        fcie_clrbits(pHost_st, FCIE_MIE_INT_EN, CARD_DMA_END);

        if(0 == u32_wake)
		{
			fcie_int = 1;
			wake_up(&fcie_wait);
			u32_wake = 1;
		}
    }

    return IRQ_HANDLED;
}
#endif

static s32 mstar_mci_get_ro(struct mmc_host *pMMC_st)
{
    /* Define Local Variables */
    s32 read_only = 0;
    #if 0
    struct mstar_mci_host *pHost_st = mmc_priv(pMMC_st);
    #endif

    /* Parameter out-of-bound check */
    if (!pMMC_st)
    {
        printk(KERN_WARNING "mstar_mci_get_ro parameter pMMC_st is NULL\n");
        return -EINVAL;
    }
    /* End Parameter out-of-bound check */

    return read_only;
}

#ifdef CONFIG_DEBUG_FS

static int mstar_mci_regs_show(struct seq_file *seq, void *v)
{
    struct mstar_mci_host *pHost_st = seq->private;
    u16 i;
	u16 regval;

	seq_printf(seq, "\nCHIPTOP:\n");
	seq_printf(seq, "allpadin:\t\t%04X\n", fcie_readw(REG_ALLAD_IN));
    seq_printf(seq, "USB DRV BUS config: \t%04X\n", fcie_readw(REG_USB_DRV_BUS_CONFIG));
    seq_printf(seq, "EXT INT: \t\t%04X\n", fcie_readw(REG_EXTINT));
    seq_printf(seq, "SD pad: \t\t%04X\n", fcie_readw(REG_SD_CONFIG));
	seq_printf(seq, "PCM pad:\t\t%04X\n", fcie_readw(REG_PCM_CONFIG));
	seq_printf(seq, "eMMC pad:\t\t%04X\n", fcie_readw(REG_EMMC_CONFIG));
	seq_printf(seq, "nand pad:\t\t%04X\n", fcie_readw(REG_NAND_CONFIG));

	seq_printf(seq, "\nCLKGEN0:\n");
    seq_printf(seq, "mcu clk:\t\t%04X\n", fcie_readw(REG_CLK_MCU));
    seq_printf(seq, "miu clk:\t\t%04X\n", fcie_readw(REG_CLK_MIU));
	seq_printf(seq, "fcie clk:\t\t%04X\n", fcie_readw(REG_CLK_NFIE));

	seq_printf(seq, "\nFCIE3:\n");

	regval = fcie_reg_read(pHost_st, FCIE_MIE_EVENT);
	seq_printf(seq, "MIE_EVENT(0x00)=\t%04X ", regval);
	if( regval & MMA_DATA_END )
		seq_printf(seq, "MMA_DATA_END ");
	if( regval & SD_CMD_END )
		seq_printf(seq, "SD_CMD_END ");
	if( regval & SD_DATA_END )
		seq_printf(seq, "SD_DATA_END ");
	if( regval & CARD_DMA_END )
		seq_printf(seq, "CARD_DMA_END ");
	if( regval & MMA_LAST_DONE)
		seq_printf(seq, "MMA_LAST_DONE ");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, FCIE_MIE_INT_EN);
	seq_printf(seq, "MIE_INT_EN(0x01)=\t%04X ", regval);
	if( regval & MMA_DATA_END )
		seq_printf(seq, "MMA_DATA_END ");
	if( regval & SD_CMD_END )
		seq_printf(seq, "SD_CMD_END ");
	if( regval & SD_DATA_END )
		seq_printf(seq, "SD_DATA_END ");
	if( regval & CARD_DMA_END )
		seq_printf(seq, "CARD_DMA_END ");
	if( regval & MMA_LAST_DONE)
		seq_printf(seq, "MMA_LAST_DONE ");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, FCIE_MMA_PRI_REG);
	seq_printf(seq, "MMA_PRI_REG(0x02)=\t%04X ", regval);
	if( regval & JOB_RW_DIR)
		seq_printf(seq, "Write ");
	else
		seq_printf(seq, "Read ");
	if( regval & FIFO_CLKRDY )
		seq_printf(seq, "FIFO_CLKRDY ");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, FCIE_MIU_DMA1);
	seq_printf(seq, "MIU_DMA1(0x03)=\t\t%04X ", regval);
	if( regval & MIU1_SELECT )
		seq_printf(seq, "MIU1\n");
	else
		seq_printf(seq, "MIU0\n");

	regval = fcie_reg_read(pHost_st, FCIE_CARD_EVENT);
	seq_printf(seq, "CARD_EVENT(0x05)=\t%04X ", regval);
	if( regval & SD_STS_CHG )
		seq_printf(seq, "SD_STS_CHG\033m");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, FCIE_CARD_INT_EN);
	seq_printf(seq, "CARD_INT_EN(0x06)=\t%04X ", regval);
	if( regval & SD_STS_EN )
		seq_printf(seq, "SD_STS_EN");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, FCIE_CARD_DET);
	seq_printf(seq, "CARD_DET(0x07)=\t\t%04X ", regval);
	if( regval & SD_DET_N )
		seq_printf(seq, "SD_DET_N ");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, FCIE_MIE_PATH_CTL);
	seq_printf(seq, "MIE_PATH_CTL(0x0A)=\t%04X ", regval);
	if( regval & MMA_ENABLE )
		seq_printf(seq, "MMA_ENABLE ");
	if( regval & SD_EN )
		seq_printf(seq, "SD_EN ");
	if( regval & NC_EN )
		seq_printf(seq, "NC_EN ");
	seq_printf(seq, "\n");

	seq_printf(seq, "JOB_BL_CNT(0x0B)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_JOB_BL_CNT));
	seq_printf(seq, "TR_BL_CNT(0x0C)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_TR_BL_CNT));

    fcie_setbits(pHost_st, FCIE_JOB_BL_CNT, BIT15);
    fcie_setbits(pHost_st, FCIE_JOB_BL_CNT, BIT14);

    seq_printf(seq, "\tMIU TR_BL_CNT(0x0C)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_TR_BL_CNT));

    fcie_clrbits(pHost_st, FCIE_JOB_BL_CNT, BIT14);
    seq_printf(seq, "\tCARD TR_BL_CNT(0x0C)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_TR_BL_CNT));

    fcie_clrbits(pHost_st, FCIE_JOB_BL_CNT, BIT15);

	seq_printf(seq, "RSP_SIZE(0x0D)=\t\t%04X\n", fcie_reg_read(pHost_st, FCIE_RSP_SIZE));
	seq_printf(seq, "CMD_SIZE(0x0E)=\t\t%04X\n", fcie_reg_read(pHost_st, FCIE_CMD_SIZE));
	seq_printf(seq, "CARD_WD_CNT(0x0F)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_CARD_WD_CNT));

	regval = fcie_reg_read(pHost_st, FCIE_SD_MODE);
	seq_printf(seq, "SD_MODE(0x10)=\t\t%04X ", regval);
	if( regval & SD_CLK_EN )
		seq_printf(seq, "SD_CLK_EN ");
	if( regval & SD_DAT_LINE0 )
		seq_printf(seq, "SD_DAT_LINE0 ");
	if( regval & SD_DAT_LINE1 )
		seq_printf(seq, "SD_DAT_LINE1 ");
	if( regval & SD_CS_EN )
		seq_printf(seq, "SD_CS_EN ");
	if( regval & SD_DEST )
		seq_printf(seq, "CIFC ");
	else
		seq_printf(seq, "CIFD ");
	if( regval & SD_DATSYNC )
		seq_printf(seq, "SD_DATSYNC ");
	if( regval & SDIO_SD_BUS_SW )
		seq_printf(seq, "SDIO_SD_BUS_SW!!! ");
    if( regval & SD_DMA_RD_CLK_STOP )
		seq_printf(seq, "SD_DMA_RD_CLK_STOP!!! ");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, FCIE_SD_CTL);
	seq_printf(seq, "SD_CTL(0x11)=\t\t%04X ", regval);
	if( regval & SD_RSPR2_EN )
		seq_printf(seq, "SD_RSPR2_EN ");
	if( regval & SD_RSP_EN )
		seq_printf(seq, "SD_RSP_EN ");
	if( regval & SD_CMD_EN )
		seq_printf(seq, "SD_CMD_EN ");
	if( regval & SD_DTRX_EN )
		seq_printf(seq, "SD_DTRX_EN ");
	if( regval & SD_DTRX_DIR )
		seq_printf(seq, "SD_DTRX_DIR ");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, FCIE_SD_STS);
	seq_printf(seq, "SD_STS(0x12)=\t\t%04X ", regval);
	if( regval & SD_DAT_CERR )
		seq_printf(seq, "SD_DAT_CERR ");
	if( regval & SD_DAT_STSERR )
		seq_printf(seq, "SD_DAT_STSERR ");
	if( regval & SD_DAT_STSNEG )
		seq_printf(seq, "SD_DAT_STSNEG ");
	if( regval & SD_DAT_NORSP )
		seq_printf(seq, "SD_DAT_NORSP ");
	if( regval & SD_CMDRSP_CERR )
		seq_printf(seq, "SD_CMDRSP_CERR ");
    if( regval & SD_WR_PRO_N )
		seq_printf(seq, "SD_WR_PRO_N ");
    if( regval & SD_CARD_BUSY )
		seq_printf(seq, "SD_CARD_BUSY ");
	seq_printf(seq, "\n");

	regval = fcie_reg_read(pHost_st, SDIO_BLK_SIZE);
	seq_printf(seq, "SDIO_CTL(0x1B)=\t\t%04X ", regval);
	if( regval & SDIO_BLK_MOD_EN )
		seq_printf(seq, "SDIO_BLK_MOD_EN ");
	seq_printf(seq, "\n");

	seq_printf(seq, "SDIO_MIU_DMA0(0x1C)=\t%04X\n", fcie_reg_read(pHost_st, SDIO_MIU_DMA0));
	seq_printf(seq, "SDIO_MIU_DMA1(0x1D)=\t%04X\n", fcie_reg_read(pHost_st, SDIO_MIU_DMA1));
	seq_printf(seq, "MIU_OFFSET(0x2E)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_DMA_OFFSET));

	regval = fcie_reg_read(pHost_st, EMMC_BOOT_CONFIG);
	seq_printf(seq, "EMMC_BOOT_CONFIG(0x2F)=\t%04X ", regval);
	if( regval & BOOT_STG2_EN )
		seq_printf(seq, "BOOT_STG2_EN ");
	if( regval & BOOT_END_EN )
		seq_printf(seq, "BOOT_END_EN ");
	seq_printf(seq, "\n");

	seq_printf(seq, "TEST_MODE(0x30)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_TEST_MODE));

    #ifdef CONFIG_MSTAR_AMBER3
    seq_printf(seq, "SD_DDR_CNT(0x33)=\t%04X\n", fcie_reg_read(pHost_st, FCIE_SD_TOGGLE_CNT));

    regval = fcie_reg_read(pHost_st, FCIE_MISC);
    seq_printf(seq, "MISC(0x36)=\t\t%04X ", regval);
    if( (regval & DQS_MODE_DELAY_MASK) == DQS_MODE_DELAY_2T )
        seq_printf(seq, "DQS_MODE_DELAY_2T ");
    if( (regval & DQS_MODE_DELAY_MASK) == DQS_MODE_DELAY_1_5T )
        seq_printf(seq, "DQS_MODE_DELAY_1_5T ");
    if( (regval & DQS_MODE_DELAY_MASK) == DQS_MODE_DELAY_2_5T )
        seq_printf(seq, "DQS_MODE_DELAY_2_5T ");
    if( (regval & DQS_MODE_DELAY_MASK) == DQS_MODE_DELAY_1T )
        seq_printf(seq, "DQS_MODE_DELAY_NO_DELAY ");
    seq_printf(seq, "\n");
    #endif

	seq_printf(seq, "\nCIFC:\n");
	for(i=0x60; i<0x80; i++)
    {
        if(i%8==0) seq_printf(seq, "%02X: ", (i/8)*8);
        seq_printf(seq, "0x%04X ", fcie_reg_read(pHost_st, i*4) );
        if(i%8==7) seq_printf(seq, ":%02X\r\n", ((i/8)*8)+7);
    }

	seq_printf(seq, "\nDebug ports:\n");
	for(i=0; i<=7; i++)
	{
		// Enable debug ports
		fcie_clrbits(pHost_st, FCIE_TEST_MODE, (BIT10|BIT9|BIT8));
		fcie_setbits(pHost_st, FCIE_TEST_MODE, i<<8);

		// Dump debug ports
		seq_printf(seq, "\nDebug Mode %d:\n", i);
		seq_printf(seq, "DBus[15:0]=%04X\n", fcie_reg_read(pHost_st, FCIE_DEBUG_BUS_LOW));
		seq_printf(seq, "DBus[23:16]=%04X\n", fcie_reg_read(pHost_st, FCIE_DEBUG_BUS_HIGH));
	}

	return 0;
}

static int mstar_mci_regs_open(struct inode *inode, struct file *file)
{
	return single_open(file, mstar_mci_regs_show, inode->i_private);
}

static const struct file_operations mstar_mci_fops_regs = {
	.owner		= THIS_MODULE,
	.open		= mstar_mci_regs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

#if defined(MMC_SPEED_TEST) && MMC_SPEED_TEST
static int mstar_mci_perf_show(struct seq_file *seq, void *v)
{
    #if defined(DMA_TIME_TEST) && DMA_TIME_TEST

    #if DMA_TIME_TEST_READ

    seq_printf(seq, "Total Read Length: 0x%08X\n", totaldmalen);
    seq_printf(seq, "Total Ticks of Read: 0x%08X\n", totaldmatime);

    #else

    seq_printf(seq, "Total Write Length: 0x%08X\n", totaldmalen);
    seq_printf(seq, "Total Tick of Write: 0x%08X\n", totaldmatime);

    #endif

    #endif

    #if defined(REQ_TIME_TEST) && REQ_TIME_TEST

    seq_printf(seq, "Total Request Length: 0x%08X\n", totalreqlen);
    seq_printf(seq, "Total Ticks of Request: 0x%08X\n", totalreqtime);

    #endif

    #if defined(BUSY_TIME_TEST) && BUSY_TIME_TEST
    seq_printf(seq, "min D0 time = %X\n", min_D0_time);
    seq_printf(seq, "max D0 time = %X\n", max_D0_time);
    seq_printf(seq, "total D0 time = %X\n", total_D0_time);

    seq_printf(seq, "D0 <1us = %d\n", total_small_D0_count);
    seq_printf(seq, "D0 1us ~ 10us = %d\n", total_10us_D0_count);
    seq_printf(seq, "D0 10us ~ 100us = %d\n", total_100us_D0_count);
    seq_printf(seq, "D0 100us ~ 1ms = %d\n", total_1ms_D0_count);
    seq_printf(seq, "D0 1ms ~ 10ms = %d\n", total_10ms_D0_count);
    seq_printf(seq, "D0 >10ms = %d\n", total_large_D0_count);

    seq_printf(seq, "total D0 count = %X\n", total_D0_count);
    #endif

    return 0;
}

static int mstar_mci_perf_open(struct inode *inode, struct file *file)
{
	return single_open(file, mstar_mci_perf_show, inode->i_private);
}

static const struct file_operations mstar_mci_fops_perf = {
	.owner		= THIS_MODULE,
	.open		= mstar_mci_perf_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

#if defined(MMC_REQ_LENGTH_TEST) && MMC_REQ_LENGTH_TEST
static int mstar_mci_reql_show(struct seq_file *seq, void *v)
{
    seq_printf(seq, "Read request size:\n");
    seq_printf(seq, "<512B: %X\n", total_512B_read_count);
    seq_printf(seq, "512B ~ 1KB: %X\n", total_1KB_read_count);
    seq_printf(seq, "1KB ~ 2KB: %X\n", total_2KB_read_count);
    seq_printf(seq, "2KB ~ 4KB: %X\n", total_4KB_read_count);
    seq_printf(seq, "4KB ~ 8KB: %X\n", total_8KB_read_count);
    seq_printf(seq, "8KB ~ 16KB: %X\n", total_16KB_read_count);
    seq_printf(seq, "16KB ~ 32KB: %X\n", total_32KB_read_count);
    seq_printf(seq, "32KB ~ 64KB: %X\n", total_64KB_read_count);
    seq_printf(seq, "64KB ~ 128KB: %X\n", total_128KB_read_count);
    seq_printf(seq, "128KB ~ 256KB: %X\n", total_256KB_read_count);
    seq_printf(seq, "256KB ~ 512KB: %X\n", total_512KB_read_count);
    seq_printf(seq, "512KB ~ 1MB: %X\n", total_1MB_read_count);
    seq_printf(seq, "1MB ~ 2MB: %X\n", total_2MB_read_count);

    seq_printf(seq, "total read count: %X\n", total_read_count);

    seq_printf(seq, "\nWrite request size:\n");
    seq_printf(seq, "<512B: %X\n", total_512B_write_count);
    seq_printf(seq, "512B ~ 1KB: %X\n", total_1KB_write_count);
    seq_printf(seq, "1KB ~ 2KB: %X\n", total_2KB_write_count);
    seq_printf(seq, "2KB ~ 4KB: %X\n", total_4KB_write_count);
    seq_printf(seq, "4KB ~ 8KB: %X\n", total_8KB_write_count);
    seq_printf(seq, "8KB ~ 16KB: %X\n", total_16KB_write_count);
    seq_printf(seq, "16KB ~ 32KB: %X\n", total_32KB_write_count);
    seq_printf(seq, "32KB ~ 64KB: %X\n", total_64KB_write_count);
    seq_printf(seq, "64KB ~ 128KB: %X\n", total_128KB_write_count);
    seq_printf(seq, "128KB ~ 256KB: %X\n", total_256KB_write_count);
    seq_printf(seq, "256KB ~ 512KB: %X\n", total_512KB_write_count);
    seq_printf(seq, "512KB ~ 1MB: %X\n", total_1MB_write_count);
    seq_printf(seq, "1MB ~ 2MB: %X\n", total_2MB_write_count);

    seq_printf(seq, "total write count: %X\n", total_write_count);

    return 0;
}

static int mstar_mci_reql_open(struct inode *inode, struct file *file)
{
	return single_open(file, mstar_mci_reql_show, inode->i_private);
}

static const struct file_operations mstar_mci_fops_reql = {
	.owner		= THIS_MODULE,
	.open		= mstar_mci_reql_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

static void mstar_mci_debugfs_attach(struct mstar_mci_host *pHost_st)
{
    struct device *dev = mmc_dev(pHost_st->mmc);

    pHost_st->debug_root = debugfs_create_dir(dev_name(dev), NULL);

	if (IS_ERR(pHost_st->debug_root)) {
		dev_err(dev, "failed to create debugfs root\n");
		return;
	}

    pHost_st->debug_regs = debugfs_create_file("FCIE_REGS", 0444,
					       pHost_st->debug_root, pHost_st,
					       &mstar_mci_fops_regs);

	if (IS_ERR(pHost_st->debug_regs))
		dev_err(dev, "failed to create debug regs file\n");

    #if defined(MMC_SPEED_TEST) && MMC_SPEED_TEST
    pHost_st->debug_perf = debugfs_create_file("FCIE_PERF", 0444,
					       pHost_st->debug_root, pHost_st,
					       &mstar_mci_fops_perf);

	if (IS_ERR(pHost_st->debug_perf))
		dev_err(dev, "failed to create debug regs file\n");
    #endif

    #if defined(MMC_REQ_LENGTH_TEST) && MMC_REQ_LENGTH_TEST
    pHost_st->debug_reql = debugfs_create_file("FCIE_REQL", 0444,
					       pHost_st->debug_root, pHost_st,
					       &mstar_mci_fops_reql);

	if (IS_ERR(pHost_st->debug_reql))
		dev_err(dev, "failed to create debug regs file\n");
    #endif
}

static void mstar_mci_debugfs_remove(struct mstar_mci_host *pHost_st)
{
    #if defined(MMC_REQ_LENGTH_TEST) && MMC_REQ_LENGTH_TEST
    debugfs_remove(pHost_st->debug_reql);
    #endif
    #if defined(MMC_SPEED_TEST) && MMC_SPEED_TEST
    debugfs_remove(pHost_st->debug_perf);
    #endif
    debugfs_remove(pHost_st->debug_regs);
	debugfs_remove(pHost_st->debug_root);
}

#else

static inline void mstar_mci_debugfs_attach(struct mstar_mci_host *pHost_st) { }
static inline void mstar_mci_debugfs_remove(struct mstar_mci_host *pHost_st) { }

#endif

/* Special code for 029B, U02 chip */
static void _halFCIE_Platform_DisableBootMode(struct mstar_mci_host *pHost_st)
{
    unsigned short regval;
    u16 u16_TimeoutCount = 0;

    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "_halFCIE_Platform_DisableBootMode parameter pHost_st is NULL\n");
        return;
    }

    fcie_writew((fcie_readw(0x1f203d40) & (~0x8000)), 0x1f203d40);
    fcie_writew((fcie_readw(0x1f203dbc) & (~0xC0)), 0x1f203dbc);
    fcie_writew((fcie_readw(0x1f203d90) & (~0x10)), 0x1f203d90);

#ifdef CONFIG_MSTAR_AMBER3_BD_GENERIC
    fcie_writew((fcie_readw(REG_EMMC_CONFIG) & (~0xC0)), REG_EMMC_CONFIG);
    fcie_writew((fcie_readw(REG_EMMC_CONFIG) | 0x40), REG_EMMC_CONFIG);

    fcie_writew((fcie_readw(0x1f203c40) | 0x2), 0x1f203c40);

    fcie_reg_write(pHost_st, FCIE_TEST_MODE, (fcie_reg_read(pHost_st, FCIE_TEST_MODE) | 0x100));
    msleep(100);

    regval=fcie_reg_read(pHost_st, FCIE_DEBUG_BUS_LOW);
    if ((regval != 0x1F) && (regval != 0x0F))
    {
        if (regval == 0x0E)
        {
            fcie_reg_write(pHost_st, EMMC_BOOT_CONFIG, (fcie_reg_read(pHost_st, EMMC_BOOT_CONFIG) | 0x2));

            while( (fcie_reg_read(pHost_st, FCIE_MIE_EVENT)&0x8000) == 0 )
            {
                if( ++u16_TimeoutCount < 1000000 )
                {
                    udelay(1);
                }
                else
                {
                    panic("%s: Wait Clear MIE event timeout case 1\n", __FUNCTION__);
                }
            }

            //fcie_reg_write(pHost_st, FCIE_MIE_PATH_CTL, 0);
            fcie_reg_write(pHost_st, FCIE_JOB_BL_CNT, 0);
            regval=fcie_reg_read(pHost_st, FCIE_SD_STS);
            fcie_reg_write(pHost_st, FCIE_SD_STS, regval);
            regval=fcie_reg_read(pHost_st, FCIE_MIE_EVENT);
            fcie_reg_write(pHost_st, FCIE_MIE_EVENT, regval);
        }
        fcie_reg_write(pHost_st, FCIE_CARD_INT_EN, regval);
        return;
    }

    fcie_reg_write(pHost_st, FCIE_JOB_BL_CNT, 1);
    fcie_reg_write(pHost_st, FCIE_SD_MODE, (fcie_reg_read(pHost_st, FCIE_SD_MODE) | 0x200));
    udelay(100);
    fcie_reg_write(pHost_st, FCIE_SD_CTL, 0x8);

    u16_TimeoutCount = 0;
    while( (fcie_reg_read(pHost_st, FCIE_MIE_EVENT)&0x4) == 0 )
    {
        if( ++u16_TimeoutCount < 100000 )
        {
            udelay(10);
        }
        else
        {
            panic("%s: Wait Clear MIE event timeout case 2\n", __FUNCTION__);
        }
    }

    udelay(1000);
    fcie_reg_write(pHost_st, EMMC_BOOT_CONFIG, (fcie_reg_read(pHost_st, EMMC_BOOT_CONFIG) | 0x2));

    u16_TimeoutCount = 0;
    while( (fcie_reg_read(pHost_st, FCIE_MIE_EVENT)&0x8000) == 0 )
    {
        if( ++u16_TimeoutCount < 100000 )
        {
            udelay(10);
        }
        else
        {
            panic("%s: Wait Clear MIE event timeout case 3\n", __FUNCTION__);
        }
    }

    //fcie_reg_write(pHost_st, FCIE_MIE_PATH_CTL, 0);
    fcie_reg_write(pHost_st, FCIE_JOB_BL_CNT, 0);
    regval=fcie_reg_read(pHost_st, FCIE_SD_STS);
    fcie_reg_write(pHost_st, FCIE_SD_STS, regval);
    regval=fcie_reg_read(pHost_st, FCIE_MIE_EVENT);
    fcie_reg_write(pHost_st, FCIE_MIE_EVENT, regval);

    fcie_reg_write(pHost_st, FCIE_SD_MODE, (fcie_reg_read(pHost_st, FCIE_SD_MODE) & (~0x200)));
    fcie_reg_write(pHost_st, FCIE_SD_CTL, 0);
#endif
}

void halFCIE_Init(struct mstar_mci_host *pHost_st)
{
    u16 u16_TimeoutCount = 0;

    /* Parameter out-of-bound check */
    if (!pHost_st)
    {
        printk(KERN_WARNING "halFCIE_Init parameter pHost_st is NULL\n");
        return;
    }

	// reenable clk after resume
	fcie_setbits(pHost_st, FCIE_SD_MODE, SD_CLK_EN);
	atomic_set(&emmc_in_use, 1);

    if (!(pHost_st->u02_flag))
    {
        return;
    }

    _halFCIE_Platform_DisableBootMode(pHost_st);

    //fcie_reg_write(pHost_st, FCIE_MIE_PATH_CTL, 0x0);
    //fcie_reg_write(pHost_st, FCIE_MIE_EVENT, 0xFFFF);

    fcie_reg_write(pHost_st, FCIE_TEST_MODE, (fcie_reg_read(pHost_st, FCIE_TEST_MODE) & (~0x1000)));
    udelay(10); // 10us
    //=================================================
    // PATCH write CIFC fail after FCIE reset
    //=================================================
    while( (fcie_reg_read(pHost_st, FCIE_TEST_MODE) & 0x1000) == 0 )
    {
        fcie_reg_write(pHost_st, FCIE_TEST_MODE, (fcie_reg_read(pHost_st, FCIE_TEST_MODE) | 0x1000));

        if( ++u16_TimeoutCount < 1000000 )
        {
            udelay(1);
        }
        else
        {
            panic("%s: Wait reset FCIE timeout\n", __FUNCTION__);
        }
    }
    udelay(100); // 100us

    fcie_reg_write(pHost_st, EMMC_BOOT_CONFIG, 0x200);
}

static s32 mstar_mci_probe(struct platform_device *pDev_st)
{
    /* Define Local Variables */
    struct mmc_host *pMMC_st        = 0;
    struct mstar_mci_host *pHost_st = 0;
    struct resource *pRes_st        = 0;

    #if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
    s32 ret = 0;
    #endif

    /* Parameter out-of-bound check */
    if (!pDev_st)
    {
        printk(KERN_WARNING "mstar_mci_probe parameter pDev_st is NULL\n");
        return -EINVAL;
    }
    /* End Parameter out-of-bound check */

    pRes_st = platform_get_resource(pDev_st, IORESOURCE_MEM, 0);
    if (!pRes_st)
    {
        return -ENXIO;
    }

    if (!request_mem_region(pRes_st->start, pRes_st->end - pRes_st->start + 1, DRIVER_NAME))
    {
        return -EBUSY;
    }

    pMMC_st = mmc_alloc_host(sizeof(struct mstar_mci_host), &pDev_st->dev);
    if (!pMMC_st)
    {
        pr_debug("Failed to allocate mmc host\n");
        release_mem_region(pRes_st->start, pRes_st->end - pRes_st->start + 1);
        return -ENOMEM;
    }

    pMMC_st->ops = &sg_mstar_mci_ops;
    pMMC_st->f_min = CLK_300KHz;
	pMMC_st->f_max = CLK_48MHz;

    pMMC_st->ocr_avail = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 | \
						 MMC_VDD_31_32 | MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;

    pMMC_st->max_blk_count  = 4095;
    pMMC_st->max_blk_size   = 512; /* sector */
    pMMC_st->max_req_size   = pMMC_st->max_blk_count  * pMMC_st->max_blk_size;
    pMMC_st->max_seg_size   = pMMC_st->max_req_size;

    pMMC_st->max_phys_segs  = 128;
    pMMC_st->max_hw_segs    = 128;

    pHost_st           = mmc_priv(pMMC_st);
    pHost_st->mmc      = pMMC_st;

    #if defined(CONFIG_MMC_MSTAR_MMC_SD) && CONFIG_MMC_MSTAR_MMC_SD
    pMMC_st->caps = MMC_CAP_4_BIT_DATA | MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED;
    #else
    pMMC_st->caps = MMC_CAP_8_BIT_DATA | MMC_CAP_MMC_HIGHSPEED | MMC_CAP_NONREMOVABLE;
    #endif

    #if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
    #if (defined(CONFIG_MSTAR_AMBER3) || \
         defined(CONFIG_MSTAR_AGATE) || \
         defined(CONFIG_MSTAR_EAGLE))
    #if defined(DDR_MODE_ENABLE) && DDR_MODE_ENABLE
    pMMC_st->caps |= MMC_CAP_1_8V_DDR;
    #endif
    #endif
    #endif

    pHost_st->baseaddr = (void __iomem *)FCIE0_BASE;

    mmc_add_host(pMMC_st);

    #ifdef CONFIG_DEBUG_FS
    mstar_mci_debugfs_attach(pHost_st);
    #endif

    platform_set_drvdata(pDev_st, pMMC_st);

    mstar_mci_enable(pHost_st);

    #if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
    /***** Allocate the MCI interrupt *****/
    pHost_st->irq = platform_get_irq(pDev_st, 0);
    ret = request_irq(pHost_st->irq, mstar_mci_irq, IRQF_SHARED, DRIVER_NAME, pHost_st);
    if (ret)
    {
        printk(KERN_ERR "Mstar MMC: Failed to request MCI interrupt\n");
        mmc_free_host(pMMC_st);
        release_mem_region(pRes_st->start, pRes_st->end - pRes_st->start + 1);
        return ret;
    }

    pr_debug("Mstar MMC: request MCI interrupt = %d\n", pHost_st->irq);

    #endif

    return 0;
}

static s32 __exit mstar_mci_remove(struct platform_device *pDev_st)
{
    /* Define Local Variables */
    struct mmc_host *pMMC_st        = platform_get_drvdata(pDev_st);
    struct mstar_mci_host *pHost_st = mmc_priv(pMMC_st);
    struct resource *pRes_st        = 0;

    /* Parameter out-of-bound check */
    if (!pDev_st)
    {
        printk(KERN_WARNING "mstar_mci_remove parameter pDev_st is NULL\n");
        return -EINVAL;
    }
    /* End Parameter out-of-bound check */

    if (!pMMC_st)
    {
        return -1;
    }

    #ifdef CONFIG_DEBUG_FS
    mstar_mci_debugfs_remove(pHost_st);
    #endif

    mstar_mci_disable(pHost_st);
    mmc_remove_host(pMMC_st);

    #if defined(MSTAR_MCI_IRQ) && MSTAR_MCI_IRQ
    free_irq(pHost_st->irq, pHost_st);
    #endif

    pRes_st = platform_get_resource(pDev_st, IORESOURCE_MEM, 0);
    release_mem_region(pRes_st->start, pRes_st->end - pRes_st->start + 1);

    mmc_free_host(pMMC_st);
    platform_set_drvdata(pDev_st, NULL);
    pr_debug("MCI Removed\n");

    return 0;
}


#ifdef CONFIG_PM
static s32 mstar_mci_suspend(struct platform_device *pDev_st, pm_message_t state)
{
    /* Define Local Variables */
    struct mmc_host *pMMC_st = platform_get_drvdata(pDev_st);
    s32 ret = 0;
    struct mstar_mci_host *pHost_st = NULL;

    if (pMMC_st)
    {
		    /* identify u02 chip, it needs a special patch */
				pHost_st = mmc_priv(pMMC_st);
				if (pHost_st->u02_set != 0x7e)
				{
					if (fcie_readw(0x1f203c40) & 0x2)
					{
					    pHost_st->u02_flag = 1;
					}
					else
					{
					    pHost_st->u02_flag = 0;
					}

					pHost_st->u02_set = 0x7e;
				}
			  //pHost_st->sd_clk |= BIT_FCIE_CLK_300K;

        ret = mmc_suspend_host(pMMC_st);
    }

    pr_debug("MCI suspend ret=%d\n", ret);
    return ret;
}

static s32 mstar_mci_resume(struct platform_device *pDev_st)
{
    /* Define Local Variables */
    struct mmc_host *pMMC_st = platform_get_drvdata(pDev_st);
    struct mstar_mci_host *pHost_st = NULL;
    s32 ret = 0;

    if (pMMC_st)
    {
        pHost_st = mmc_priv(pMMC_st);
        halFCIE_Init(pHost_st);
        mstar_mci_enable(pHost_st);
        ret = mmc_resume_host(pMMC_st);
        mstar_mci_enable(pHost_st);
    }

    pr_debug("MCI resume ret=%d\n", ret);
    return ret;
}
#endif  /* End ifdef CONFIG_PM */

/******************************************************************************
 * Define Static Global Variables
 ******************************************************************************/

/* MSTAR MCI Driver (Include Remove, Suspend, and Resume) */
static struct platform_driver sg_mstar_mci_driver =
{
	.probe = mstar_mci_probe,
    .remove = __exit_p(mstar_mci_remove),

    #ifdef CONFIG_PM
    .suspend = mstar_mci_suspend,
    .resume = mstar_mci_resume,
    #endif

    .driver  =
    {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    },
};

/******************************************************************************
 * Init & Exit Modules
 ******************************************************************************/
static s32 __init mstar_mci_init(void)
{
	int err = 0;

    printk(KERN_INFO "EMMC Init: %s.\n",__FUNCTION__);

    if( (err = platform_device_register(&sg_mstar_mmc_device_st)) < 0 )
		printk("\033[31mFail to register eMMC platform device\033[m\n");
    if( (err = platform_driver_register(&sg_mstar_mci_driver)) < 0 )
		printk("\033[31mFail to register eMMC platform driver\033[m\n");

	return err;
}

static void __exit mstar_mci_exit(void)
{
    platform_driver_unregister(&sg_mstar_mci_driver);
}

module_init(mstar_mci_init);
module_exit(mstar_mci_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mstar Multimedia Card Interface driver");
MODULE_AUTHOR("Benson.Hsiao");

/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    PROJECT: amber3

	FILE NAME: arch/arm/mach-amber3/amber3_arch.c

    DESCRIPTION:
          Power Management Driver

    HISTORY:
         <Date>     <Author>    <Modification Description>
        2008/07/18  Fred Cheng  Add IO tables for ITCM and DTCM
        2008/07/22  Evan Chang  Add SD card init

------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/memory.h>
#include <mach/io.h>
#include <asm/mach/map.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <asm/irq.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/hardware/gic.h>
#include <asm/smp_twd.h>
#include <asm/cacheflush.h>
#include <asm/pmu.h>

#include "chip_int.h"

#if CONFIG_WRITE_TO_ANDROID_MISC
#include <linux/reboot.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#endif

#ifdef CONFIG_WRITE_TO_ANDROID_MISC_NAND
#include <linux/err.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stringify.h>
#include <linux/namei.h>
#include <linux/stat.h>
#include <linux/miscdevice.h>
#include <linux/log2.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#endif




/*------------------------------------------------------------------------------
    External Function Prototypes
-------------------------------------------------------------------------------*/
/* initialize amber3's IRQ */
extern void amber3_init_irq(void);
/* Add SD Card driver */
extern void __init amber3_add_device_mmc(short mmc_id);
extern void __init amber3_add_device_memstick(s16 memstick_id);
extern int Mstar_ehc_platform_init(void);
#ifdef CONFIG_WRITE_TO_ANDROID_MISC_NAND
int Write_Mtd0_init(void);
int Mtd_erase_ctrl_blocks(void);
int Write_Mtd0_recovery(__u8* DataBuf,unsigned long u32Size);
#endif

/*------------------------------------------------------------------------------
    External Global Variable
-------------------------------------------------------------------------------*/
/* amber3's system timer */
extern struct sys_timer amber3_timer;
extern struct sys_timer amber3_ptimer;
/*------------------------------------------------------------------------------
    Global Variable
-------------------------------------------------------------------------------*/
/* platform device for amber3_board_init() */
struct platform_device amber3_device_usbgadget =
{
	.name		  = "amber3-usbgadget",
	.id		  = -1,
};

static struct platform_device Mstar_emac_device = {
	.name		= "Mstar-emac",
	.id		= 0,
};

static struct platform_device Mstar_ir_device = {
	.name		= "Mstar-ir",
	.id		= 0,
};

static struct platform_device Mstar_nand_device = {
	.name		= "Mstar-nand",
	.id		= 0,
};

static struct platform_device Mstar_mbx_device = {
	.name		= "Mstar-mbx",
	.id		= 0,
};

static struct platform_device Mstar_gflip_device = {
	.name		= "Mstar-gflip",
	.id		= 0,
};

static struct platform_device Mstar_alsa_device = {
	.name		= "Mstar-alsa",
	.id		= 0,
};

/* resource for pmu irq */
static struct resource pmu_resources[] =
{
	[0] = {
		.start	= AMBER3_IRQ_PMU0,
		.end	= AMBER3_IRQ_PMU0,
		.flags	= IORESOURCE_IRQ,
	},
	[1] = {
		.start	= AMBER3_IRQ_PMU1,
		.end	= AMBER3_IRQ_PMU1,
		.flags	= IORESOURCE_IRQ,
	},
};

/* platform device for amber3_board_init() */
static struct platform_device amber3_device_pmu =
{
	.name		= "arm-pmu",
	.id		= ARM_PMU_DEVICE_CPU,
	.num_resources	= ARRAY_SIZE(pmu_resources),
	.resource	= pmu_resources,
};

/* platform device array for amber3_board_init() */
static struct platform_device *amber3_devices[] __initdata =
{
    &amber3_device_usbgadget,
    &amber3_device_pmu,
    &Mstar_emac_device,
    &Mstar_ir_device,
    &Mstar_nand_device,
    &Mstar_mbx_device,
    &Mstar_gflip_device,
    &Mstar_alsa_device,
};

/* IO tables for Registers, ITCM and DTCM */
static struct map_desc amber3_io_desc[] __initdata =
{
    /* Define Registers' physcial and virtual addresses */
    {
	    .virtual        = IO_VIRT,
	    .pfn            = __phys_to_pfn(IO_PHYS),
	    .length         = IO_SIZE,
	    .type           = MT_DEVICE
    },

    /* Define periphral physcial and virtual addresses */
    {
	    .virtual        = PERI_VIRT,
	    .pfn            = __phys_to_pfn(PERI_PHYS),
	    .length         = PERI_SIZE,
	    .type           = MT_DEVICE
    },

    {
	    .virtual	    = L2_CACHE_VIRT,
	    .pfn	    = __phys_to_pfn(L2_CACHE_PHYS),
	    .length	    = L2_CACHE_SIZE,
	    .type	    = MT_DEVICE,
    },



    #if 0
    /* Define ITCM' physcial and virtual addresses */
    {
		.virtual	= amber3_ITCM_VIRT,
		.pfn		= __phys_to_pfn(amber3_ITCM_PHYS),
		.length		= amber3_ITCM_SIZE,
		.type		= MT_DEVICE
    },
	/* Define DTCM' physcial and virtual addresses */
	{
		.virtual	= amber3_DTCM_VIRT,
		.pfn		= __phys_to_pfn(amber3_DTCM_PHYS),
		.length		= amber3_DTCM_SIZE,
		.type		= MT_DEVICE
	},
    /* Define MEM51' physcial and virtual addresses */
	{
		.virtual	= amber3_MEM51_VIRT,
		.pfn		= __phys_to_pfn(amber3_MEM51_PHY),
		.length		= amber3_MEM51_SIZE,
		.type		= MT_MEMORY_NONCACHED
	}
	#endif
};

/*------------------------------------------------------------------------------
    Local Function
-------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Function: amber3_map_io

    Description:
        init amber3 IO tables
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void __init amber3_map_io(void)
{
#ifdef CONFIG_HAVE_ARM_TWD
    twd_base=((void __iomem *)PERI_ADDRESS(0x16000600)); //private timer& watchdog base
#endif
    iotable_init(amber3_io_desc, ARRAY_SIZE(amber3_io_desc) );
}

#ifdef CONFIG_MSTAR_AMBER3_BD_FPGA
void UartInit(u32 baud, u8 data, u8 parity, u8 stop)
{
#define UART_REG8(addr)             *((volatile unsigned int*)(0xFD201300 + ((addr)<< 2)))
#define UART_BAUD_RATE	            38400 //9600 //19200 //38400 //57600 //115200 =>too frequent Rx INT
#define UART_CLK_FREQ               12000000 //172800000 //160000000 //144000000 //12000000 //123000000    //14318180    //43200000 //14318180
#define UART_DIVISOR                (UART_CLK_FREQ/(16 * UART_BAUD_RATE))
#define UART_IER	                2	/* Out: Interrupt Enable Register */
#define UART_SCCR                   8                                   // Smartcard Control Register
#define UART_SCCR_RST               0x10 
#define UART_FCR	                4	/* Out: FIFO Control Register */
#define UART_FCR_ENABLE_FIFO	    0x00 /*NA*/ /* Enable the FIFO */
#define UART_FCR_CLEAR_RCVR	        0x01 /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT	        0x02 /* Clear the XMIT FIFO */
#define UART_FCR_DMA_SELECT	        0x00 /*NA*/ /* For DMA applications */
#define UART_FCR_TRIGGER_MASK       0x00 /*NA*/ /* Mask for the FIFO trigger range */
#define UART_FCR_TRIGGER_1          0x00        /* Mask for trigger set at 1 */
#define UART_LCR	6	/* Out: Line Control Register */
#define UART_LCR_WLEN8		        0x03	                            // Wordlength: 8 bits
#define UART_LCR_STOP1		        0x00	                            //
#define UART_LCR_PARITY		        0x08	                            // Parity Enable
#define UART_DLL	                0	                                // Out: Divisor Latch Low (DLAB=1)
#define UART_DLM	                1                                // Out: Divisor Latch High (DLAB=1)
#define UART_LCR_DLAB		        0x80	                            // Divisor latch access bit
#define UART_IER_RDI		        0x01	                            // Enable receiver data available interrupt
#define BIT(bits)                   (1<<bits)
#define TOP_UART_RX_EN              BIT(10)
#define REG_TOP_UTMI_UART_SETHL     0x0001
#define REG_TOP_BASE                0xFD203C00
#define TOP_REG(addr)               (*((volatile unsigned int*)(REG_TOP_BASE + ((addr)<<2))))

    *(volatile u32*)(0xFD000000+(0x0709*4)) |= 0x0800;//bit 12 UART Enable, bit 11 UART RX Enable
    *(volatile u32*)(0xFD200000+(0x0F53*4)) &= ~0xFFFF;          // Clear UART_SEL0
    *(volatile u32*)(0xFD200000+(0x0F53*4)) |= 0x0004;           // T2 UART_SEL0 --> PIU UART0
    
    //enable reg_uart_rx_enable;
    *(volatile u32*)(0xFD200000+(0x0F01*4)) |= (1<<10);	//BIT10;

    //Disable all interrupts 							        //Interrupt Enable Register
    UART_REG8(UART_IER) = 0x00;
    UART_REG8(UART_SCCR) = UART_SCCR_RST;                       // Receive enable & reset

    //Reset receiver and transmiter 							//FIFO Control Register
    UART_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_1 | UART_FCR_TRIGGER_MASK;

    //Set 8 bit char, 1 stop bit, no parity 					//Line Control Register
    UART_REG8(UART_LCR) = (UART_LCR_WLEN8 | UART_LCR_STOP1) & (~UART_LCR_PARITY);

    //Set baud rate
    UART_REG8(UART_LCR) |= UART_LCR_DLAB;						//Line Control Register
    UART_REG8(UART_DLM) = (UART_DIVISOR >> 8) & 0x00ff;	            //Divisor Latch High
    UART_REG8(UART_DLL) = UART_DIVISOR & 0x00ff;			            //Divisor Latch Low
    UART_REG8(UART_LCR) &= ~(UART_LCR_DLAB);					//Line Control Register
    
    //Enable receiver data available interrupt
    UART_REG8(UART_IER) = UART_IER_RDI;
	
    //enable UART Rx
    TOP_REG(REG_TOP_UTMI_UART_SETHL) |= TOP_UART_RX_EN;

}
#endif

void __init serial_init(void)
{

    //UART1
    struct uart_port u1;

#ifdef CONFIG_SERIAL_8250
    struct uart_port s;
    memset(&s, 0, sizeof(s));
    s.type = PORT_16550;
    s.iobase = 0xFD201300; //virtual RIU_BASE = 0xFD000000
    s.membase = (unsigned char __iomem *)0xFD201300;
    s.mapbase = 0xFD201300;
    s.irq =E_IRQ_UART0;

#ifdef CONFIG_MSTAR_AMBER3_BD_FPGA
    s.uartclk = 12000000; //FPGA
#elif  CONFIG_MSTAR_AMBER3_BD_GENERIC
    s.uartclk = 123000000; //real chip
#endif
    s.iotype = 6;
    s.regshift = 0;
    s.fifosize = 16 ; // use the 8 byte depth FIFO well
    s.line = 0;

#ifdef CONFIG_MSTAR_AMBER3_BD_FPGA
    UartInit(1,2,3,4);
#endif

    if (early_serial_setup(&s) != 0) {
        printk(KERN_ERR "Serial(0) setup failed!\n");
    }

    memset(&u1, 0, sizeof(u1));
    u1.type = PORT_16550;
    u1.iobase = 0xFD220c00; //virtual RIU_BASE = 0xFD000000
    u1.membase = (unsigned char __iomem *)0xFD220c00;
    u1.mapbase = 0xFD220c00;
    u1.irq =E_IRQEXPL_UART1;

    u1.uartclk = 123000000; //real chip

    u1.iotype = 6;
    u1.regshift = 0;
    u1.fifosize = 16 ; // use the 8 byte depth FIFO well
    u1.line = 1;

    if (early_serial_setup(&u1) != 0) {
        printk(KERN_ERR "Serial(0) uart1 setup failed!\n");
    }
#endif

}
extern void back_trace(void);

//i,j is for fix compile warning
irqreturn_t amber3_xiu_timeout_interrupt(int i,void * j)
{
 
    printk("XIU Time Out Occurred!\n");
    printk("Address is 0x%x\n",*(volatile  short*)(0xfd200224));  
   
    #ifdef CONFIG_HANG_ISSUE_DEBUG
    back_trace(); 
    #endif
     
    #ifdef CONFIG_HANG_ISSUE_DEBUG
    *ptr = 1; 
    #endif
 
    return IRQ_HANDLED;
} 

static struct irqaction amber3_xiu_timeout_irq = {
    .name = "Amber3 Timeout IRQ",
    .flags = IRQF_TIMER | IRQF_IRQPOLL | IRQF_DISABLED,
    .handler = amber3_xiu_timeout_interrupt,

};

/*------------------------------------------------------------------------------
    Function: amber3_board_init

    Description:
        amber3 board init function
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
#if CONFIG_WRITE_TO_ANDROID_MISC

#define REBOOT_RECOVERY_STR  "boot-recovery"
#define REBOOT_UPGRADEBOOTLOADER_STR  "update-radio/hboot"

static int recovery_sys_notify_reboot(struct notifier_block *this,
            unsigned long code, void *x)
{
#ifdef CONFIG_WRITE_TO_ANDROID_MISC_EMMC
	struct file *fd;
	size_t t;
#endif
    if(x && strlen((char*)x))
    {
        mm_segment_t old_fs;
        loff_t off;

        old_fs = get_fs();
        set_fs(KERNEL_DS);
        //printk(KERN_ERR "reboot cmd %s\n", (char*)x);
#if CONFIG_WRITE_TO_ANDROID_MISC_EMMC //EMMC
        fd = filp_open("/dev/block/mmcblk1", O_RDWR, 0);
        if(fd)
        {
            off = 0;
            t = vfs_write(fd, REBOOT_RECOVERY_STR, strlen(REBOOT_RECOVERY_STR)+1, &off);
            printk(KERN_ERR "write to/dev/block/mmcblk1 %d bytes", t);
            vfs_fsync(fd, 0);
            filp_close(fd, NULL);
        }
        else
            printk(KERN_ERR "open /dev/block/mmcblk1 failed\n");
#elif defined(CONFIG_WRITE_TO_ANDROID_MISC_NAND)//NAND
    #if 1
        if(!Write_Mtd0_init())
        {
            Mtd_erase_ctrl_blocks();
            Write_Mtd0_recovery(REBOOT_RECOVERY_STR,2048);
            //Read_Mtd0_recovery();
        }
        else
        {
            printk(  "Write_Mtd0_init failed\n");    
        }
    #else
        struct file *fd = filp_open("/dev/mtd/mtd0", O_RDWR, 0);
        char *buff = kzalloc(2048, GFP_USER);
        strncpy(buff, REBOOT_RECOVERY_STR, strlen(REBOOT_RECOVERY_STR)+1);
        if(fd)
        {
            off = 0;
            size_t t = vfs_write(fd, buff, 2048, &off);
            printk(KERN_ERR "write to /dev/mtd/mtd0 %ld bytes\n", t);
            vfs_fsync(fd, 0);
            filp_close(fd, NULL);
        }
        else
            printk(KERN_ERR "open /dev/mtd/mtd0 failed\n");
        kfree(buff);
    #endif
#endif 
        set_fs(old_fs);
    }
    return NOTIFY_DONE;
}

static struct notifier_block recovery_sys_notifier = {
        .notifier_call    = recovery_sys_notify_reboot,
        .next   = NULL,
        .priority = 0,
};
#endif

static void __init amber3_board_init(void)
{
    #ifdef CONFIG_CACHE_L2X0
    unsigned int val = 0;
    void __iomem * l2x0_base;
    #endif

    platform_add_devices(amber3_devices, ARRAY_SIZE(amber3_devices));

    #if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
        Mstar_ehc_platform_init();
    #endif

    #if defined(CONFIG_MEMSTICK_AMBER3_MS)
        amber3_add_device_memstick(0);
    #endif

    #if 1
    #ifdef CONFIG_CACHE_L2X0
        l2x0_base = (void __iomem *)(L2_CACHE_ADDRESS(L2_CACHE_PHYS));

        #if L2_LINEFILL
            val = L2_CACHE_read( L2_CACHE_PHYS + PREFETCH_CTL_REG );
            L2_CACHE_write(( val | DOUBLE_LINEFILL_ENABLE | LINEFILL_WRAP_DISABLE ), L2_CACHE_PHYS + PREFETCH_CTL_REG);
        #endif

        #if L2_PREFETCH
            val = L2_CACHE_read( L2_CACHE_PHYS + PREFETCH_CTL_REG );
            L2_CACHE_write(( val | I_PREFETCH_ENABLE | D_PREFETCH_ENABLE | PREFETCH_OFFSET ), L2_CACHE_PHYS + PREFETCH_CTL_REG );
        #endif

    	/* set RAM latencies to 2 cycle for this core tile. */

		if( (*(volatile unsigned short *)(0xfd003C04))&0xFF00 >= 0x0300 )	//read chip version ; equal or later than u04 set 0x111
    writel(0x111, l2x0_base + L2X0_TAG_LATENCY_CTRL);
    else  
		writel(0x113, l2x0_base + L2X0_TAG_LATENCY_CTRL);

		
    	writel(0x111, l2x0_base + L2X0_DATA_LATENCY_CTRL);
		
      	l2x0_init(l2x0_base, 0x00400000, 0xfe0fffff);
    #endif
	
	*(volatile unsigned short *)(0xfd200200) = 0x1 ;
	*(volatile unsigned short *)(0xfd200250) = 0xFFFF ;
	*(volatile unsigned short *)(0xfd200254) = 0xFFFF ;

        setup_irq(E_FIQ_XIU_TIMEOUT , &amber3_xiu_timeout_irq);


    #endif

    #if 0  //switch UART RX pad
    reg_writew(0x0800, 0x1F001C24);
    #endif

    //serial_init();
    /* Turn on Poseidon2 power as default */
    //val = amber3_readw(0xA0003D60);
    //val |= 0x2;
    //a_writew(val, 0xA0003D60);
    #if CONFIG_WRITE_TO_ANDROID_MISC
    register_reboot_notifier(&recovery_sys_notifier);
    #endif
}


/*************************************
*  Mstar chip flush function
*************************************/
#define _BIT(x)                      (1<<(x))
spinlock_t l2prefetch_lock=SPIN_LOCK_UNLOCKED;

inline void _chip_flush_miu_pipe(void)
{
        unsigned long   dwLockFlag = 0;
        unsigned short dwReadData = 0;

    spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);

     //toggle the flush miu pipe fire bit
    *(volatile unsigned short *)(0xfd203114) = 0x0;
    *(volatile unsigned short *)(0xfd203114) = 0x1;

    do
    {
        dwReadData = *(volatile unsigned short *)(0xfd203140);
        dwReadData &= _BIT(12);  //Check Status of Flush Pipe Finish

    } while(dwReadData == 0);

    spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);

}

#ifndef CONFIG_OUTER_CACHE
struct outer_cache_fns outer_cache;
#endif

void Chip_Flush_Cache_Range(unsigned long u32Addr, unsigned long u32Size)
{
	if(  u32Addr == (unsigned long) NULL )
        {
                printk("u32Addr is invalid\n");
                return;
        }        	
	//Clean L1 & Inv L1   
	dmac_flush_range((const void *)u32Addr,(const void *)(u32Addr + u32Size));

    #ifdef CONFIG_OUTER_CACHE
    if (outer_cache.is_enable()) //check if L2 is enabled
    {
        if(!virt_addr_valid(u32Addr) || !virt_addr_valid(u32Addr+ u32Size - 1 ))
            //Clean&Inv L2 by Way
            outer_cache.flush_all();
        else
            //Clean&Inv L2 by Range
            outer_cache.flush_range(__pa(u32Addr) , __pa(u32Addr) + u32Size);

    }
    #endif

    #ifndef CONFIG_OUTER_CACHE
    _chip_flush_miu_pipe();
    #endif
}

void Chip_Clean_Cache_Range(unsigned long u32Addr, unsigned long u32Size)
{
        if(  u32Addr ==(unsigned long) NULL )
        {
                printk("u32Addr is invalid\n");
                return;
        }
    //Clean L1
    dmac_map_area((const void *)u32Addr,(size_t)u32Size,1);
    #ifdef CONFIG_OUTER_CACHE
    if (outer_cache.is_enable()) //check if L2 is enabled
    {
        if(!virt_addr_valid(u32Addr) || !virt_addr_valid(u32Addr+ u32Size - 1))
            //Clean L2 by range
            outer_cache.clean_all();
        else
            //Clean&Inv L2 by Range
            outer_cache.clean_range(__pa(u32Addr) , __pa(u32Addr) + u32Size);
    }
    #endif

    #ifndef CONFIG_OUTER_CACHE
    _chip_flush_miu_pipe();
    #endif
}


void Chip_Inv_Cache_Range(unsigned long u32Addr, unsigned long u32Size)
{
        if(  u32Addr == (unsigned long) NULL )
        {
                printk("u32Addr is invalid\n");
                return;
        }
    #ifdef CONFIG_OUTER_CACHE
    if (outer_cache.is_enable()) //check if L2 is enabled
    {
        if(!virt_addr_valid(u32Addr) || !virt_addr_valid(u32Addr+ u32Size - 1))
        {
           printk("%s:Input VA can't be converted to PA\n",__func__);
        }
        else
        {
            //Inv L2 by range
            outer_cache.inv_range(__pa(u32Addr) , __pa(u32Addr)+ u32Size);
        }
    }
    #endif
    //Inv L1
    dmac_map_area((const void *)u32Addr,(size_t)u32Size,2);

}

void Chip_Flush_Cache_Range_VA_PA(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size)
{
        if(  u32VAddr ==(unsigned long) NULL )
        {
                printk("u32VAddr is invalid\n");
                return;
        }
    //Clean & Invalid L1
    dmac_flush_range((const void *)u32VAddr,(const void *)(u32VAddr + u32Size));

    #ifdef CONFIG_OUTER_CACHE
    if (outer_cache.is_enable()) //check if L2 is enabled
    {
        //Clean&Inv L2 by range
        outer_cache.flush_range(u32PAddr,u32PAddr + u32Size);
    }
    #endif

    #ifndef CONFIG_OUTER_CACHE
    _chip_flush_miu_pipe();
    #endif
}


void Chip_Clean_Cache_Range_VA_PA(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size)
{
        if(  u32VAddr == (unsigned long) NULL )
        {
                printk("u32VAddr is invalid\n");
                return;
        }
    //Clean L1
    dmac_map_area((const void *)u32VAddr,(size_t)u32Size,1);

    #ifdef CONFIG_OUTER_CACHE
    if (outer_cache.is_enable()) //check if L2 is enabled
    {
        //Clean L2 by Way
        outer_cache.clean_range(u32PAddr,u32PAddr + u32Size);
    }
    #endif

    #ifndef CONFIG_OUTER_CACHE
    _chip_flush_miu_pipe();
    #endif
}


void Chip_Inv_Cache_Range_VA_PA(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size)
{
        if(  u32VAddr == (unsigned long)NULL )
        {
                printk("u32VAddr is invalid\n");
                return;
        }

    #ifdef CONFIG_OUTER_CACHE
    if (outer_cache.is_enable()) //check if L2 is enabled
    {
           //Inv L2 by range
           outer_cache.inv_range( u32PAddr ,u32PAddr + u32Size );
    }
    #endif
    //Inv L1
    dmac_map_area((const void *)u32VAddr,(size_t)u32Size,2);

}

//add for fixing compile warning
void L1_Cache_Flush_All(void * i)
{
	__cpuc_flush_kern_all();
}

void Chip_Flush_Cache_All(void)
{
        //Clean & Inv All L1
        unsigned long flags;

        local_irq_save(flags);

        __cpuc_flush_kern_all();

        local_irq_restore(flags);

        smp_call_function(L1_Cache_Flush_All,(void *) NULL, 1);
       //on_each_cpu(__cpuc_flush_kern_all,(void *) NULL, 1);

       local_irq_save(flags);
#ifdef CONFIG_OUTER_CACHE
        if (outer_cache.is_enable()) //check if L2 is enabled
        {
                //Clean&Inv L2 by Way
                outer_cache.flush_all();
        }
#endif
        //Clean L1  & Inv L1
        //dmac_flush_range(u32Addr,u32Addr + u32Size );

#ifndef CONFIG_OUTER_CACHE
        _chip_flush_miu_pipe();
#endif
       local_irq_restore(flags);
}

//need to be modified
void Chip_L2_cache_wback_inv( unsigned long addr, unsigned long size)
{
    outer_cache.flush_all();
}

//need to be modified
void Chip_L2_cache_wback( unsigned long addr, unsigned long size)
{
    //Flush L2 by Way, change to by Addr later
    outer_cache.clean_all();
}

//need to be modified
void Chip_L2_cache_inv( unsigned long addr, unsigned long size)
{
    //Inv L2 by Way, change to by Addr later
    outer_cache.inv_all();
}

void Chip_Flush_Memory_Range(unsigned long pAddress , unsigned long  size)
{
    #ifdef CONFIG_OUTER_CACHE
	if(outer_cache.sync)
        outer_cache.sync();
    else
    #endif
        _chip_flush_miu_pipe();
}

void Chip_Flush_Memory(void)
{
    #ifdef CONFIG_OUTER_CACHE
    if(outer_cache.sync)
        outer_cache.sync();
    else
	#endif
        _chip_flush_miu_pipe();

}

void Chip_Read_Memory_Range(unsigned long pAddress , unsigned long  size)
{
    #ifdef CONFIG_OUTER_CACHE
	if(outer_cache.sync)
        outer_cache.sync();
    else
	#endif
        _chip_flush_miu_pipe();

}

void Chip_Read_Memory(void)
{
    #ifdef CONFIG_OUTER_CACHE 
    if(outer_cache.sync)
        outer_cache.sync();
    else
	#endif
        _chip_flush_miu_pipe();

}

void Chip_Query_L2_Config(void)
{
}

unsigned int Chip_Query_Rev(void)
{
   unsigned int    dwRevisionId = 0;
   dwRevisionId = *((volatile unsigned int*)(0xFD003D9C));
   dwRevisionId >>= 8;

   return dwRevisionId;

}

#ifdef CONFIG_WRITE_TO_ANDROID_MISC_NAND
static struct mtd_info * ctrl_mtd;
static __u8 u8_Partfound;

//extern struct mtd_info * __init open_mtd_device(const char *mtd_dev);

extern struct mtd_info *get_mtd_device_nm(const char *name);
extern struct mtd_info *get_mtd_device(struct mtd_info *mtd, int num);
/**
 * open_mtd_by_chdev - open an MTD device by its character device node path.
 * @mtd_dev: MTD character device node path
 *
 * This helper function opens an MTD device by its character node device path.
 * Returns MTD device description object in case of success and a negative
 * error code in case of failure.
 */
static struct mtd_info * __init open_mtd_by_chdev(const char *mtd_dev)
{
	int err, major, minor, mode;
	struct path path;

	/* Probably this is an MTD character device node path */
	err = kern_path(mtd_dev, LOOKUP_FOLLOW, &path);
	if (err)
		return ERR_PTR(err);

	/* MTD device number is defined by the major / minor numbers */
	major = imajor(path.dentry->d_inode);
	minor = iminor(path.dentry->d_inode);
	mode = path.dentry->d_inode->i_mode;
	path_put(&path);
	if (major != MTD_CHAR_MAJOR || !S_ISCHR(mode))
		return ERR_PTR(-EINVAL);

	if (minor & 1)
		/*
		 * Just do not think the "/dev/mtdrX" devices support is need,
		 * so do not support them to avoid doing extra work.
		 */
		return ERR_PTR(-EINVAL);

	return get_mtd_device(NULL, minor / 2);
}



static struct mtd_info * __init open_mtd_device(const char *mtd_dev)
{
	struct mtd_info *mtd;
	int mtd_num;
	char *endp;

	mtd_num = simple_strtoul(mtd_dev, &endp, 0);
	if (*endp != '\0' || mtd_dev == endp) {
		/*
		 * This does not look like an ASCII integer, probably this is
		 * MTD device name.
		 */
		mtd = get_mtd_device_nm(mtd_dev);
		if (IS_ERR(mtd) && PTR_ERR(mtd) == -ENODEV)
			/* Probably this is an MTD character device node path */
			mtd = open_mtd_by_chdev(mtd_dev);
	} else
		mtd = get_mtd_device(NULL, mtd_num);

	return mtd;
}


int Write_Mtd0_init(void)
{
	int err = 0;
	u8_Partfound = 0;
	ctrl_mtd = open_mtd_device("misc");
	if(IS_ERR(ctrl_mtd))
	{
		printk("Can't find \"misc\" partition\n");
		err = PTR_ERR(ctrl_mtd);
		return err;
	}
	u8_Partfound = 1;

	return err;
}



#define ENINIT_MAX_SQNUM			0x7FFFFFFF
//control error code
#define ERR_CTRL_RECORD_FOUND		(0x600|1)
#define ERR_CTRL_RECORD_INVALID		(0x600|2)
#define ERR_CTRL_RECORD_FAIL		(0x600|3)
#define ERR_CTRL_RECORD_NOFOUND		(0x600|4)
#define ERR_CTRL_RECORD_NOAVLBLK 	(0x600|5)
#define ERR_CTRL_RECORD_WRITEFAIL 	(0x600|6)
#define ERR_TBL_RECORD_POWERCUT		(0x600|7)
#define ERR_TBL_RECORD_NOAVLBLK 	(0x600|8)


int Mtd_erase_ctrl_blocks(void)
{
	int blk_page_count, err;
	__u32 u32_i, cb_count;
	struct erase_info ei;
	blk_page_count = (ctrl_mtd->erasesize/ctrl_mtd->writesize);
	cb_count = ctrl_mtd->size >> ctrl_mtd->writesize_shift;

	for(u32_i = 0; u32_i < cb_count; u32_i += blk_page_count)
	{
		err = ctrl_mtd->block_isbad(ctrl_mtd, u32_i << ctrl_mtd->writesize_shift);
		if(err == 1)
		{
			printk("Skip bad block %X", u32_i);
			continue;
		}
		else if (err < 0)
		{
			return ERR_CTRL_RECORD_FAIL;
		}

		memset(&ei, 0, sizeof(struct erase_info));
		ei.mtd = ctrl_mtd;
		ei.addr = (u32_i / blk_page_count) << ctrl_mtd->erasesize_shift;
		ei.len = ctrl_mtd->erasesize;
		ei.callback = NULL;

		err = ctrl_mtd->erase(ctrl_mtd, &ei);
		if(err == -EIO)
		{
			err = ctrl_mtd->block_markbad(ctrl_mtd,
					(u32_i / blk_page_count)<< ctrl_mtd->erasesize_shift);
			if(err < 0)
				return err;
		}
		else if(err < 0 && err != -EIO)
		{
			return err;
		}

	}
	return 0;
}


int Write_Mtd0_recovery(__u8* DataBuf,unsigned long u32Size)
{
    __u8* pu8_DataBuf;
    __u32 err = 0;
	size_t written;
    pu8_DataBuf = kzalloc(ctrl_mtd->writesize, GFP_KERNEL);
	memcpy(pu8_DataBuf,DataBuf,u32Size);
	if(!pu8_DataBuf)
		return err;

    err = ctrl_mtd->write(ctrl_mtd, 0, ctrl_mtd->writesize, &written, pu8_DataBuf);

    kfree(pu8_DataBuf);
	return err;
}

int Read_Mtd0_recovery(void)
{
    __u8* pu8_DataBuf;
    __u32 err = 0;
    size_t read;
	pu8_DataBuf = kzalloc(ctrl_mtd->writesize, GFP_KERNEL);
	memset(pu8_DataBuf, 0xFF, ctrl_mtd->writesize);
    err = ctrl_mtd->read(ctrl_mtd,0, 10, &read, pu8_DataBuf);

	printk("%d %d %d \n",pu8_DataBuf[0],pu8_DataBuf[1],pu8_DataBuf[2]);
	return err;
}

#endif

#if 0 //modify later
unsigned int Chip_Query_CLK(void)
{
    return (12*reg_readw(0x1F22184c)); // 1 = 12(Hz)

}
#endif


EXPORT_SYMBOL(Chip_Flush_Cache_Range);
EXPORT_SYMBOL(Chip_Flush_Cache_All);
EXPORT_SYMBOL(Chip_Clean_Cache_Range);
EXPORT_SYMBOL(Chip_Inv_Cache_Range);
EXPORT_SYMBOL(Chip_L2_cache_wback_inv);
EXPORT_SYMBOL(Chip_L2_cache_wback);
EXPORT_SYMBOL(Chip_L2_cache_inv);
EXPORT_SYMBOL(Chip_Flush_Memory);
EXPORT_SYMBOL(Chip_Flush_Memory_Range);
EXPORT_SYMBOL(Chip_Read_Memory);
EXPORT_SYMBOL(Chip_Read_Memory_Range);
EXPORT_SYMBOL(Chip_Flush_Cache_Range_VA_PA);
EXPORT_SYMBOL(Chip_Clean_Cache_Range_VA_PA);
EXPORT_SYMBOL(Chip_Inv_Cache_Range_VA_PA);


/**
 ** please refer to include/asm-arm/mach/arch.h for further details
 **
 ** #define MACHINE_START(_type,_name)				\
 ** static const struct machine_desc __mach_desc_##_type	\
 **  __used							\
 **   __attribute__((__section__(".arch.info.init"))) = {	\
 **   	.nr		= MACH_TYPE_##_type,			\
 **   	.name		= _name,
 **
 **
 ** #define MACHINE_END			\
 ** };
 **
 **
 **/
//#ifdef CONFIG_MSTAR_amber3
MACHINE_START(AMBER3, "amber3")
//#endif
    .map_io = amber3_map_io,
    .init_irq = amber3_init_irq,
    .init_machine = amber3_board_init,
#ifdef CONFIG_GENERIC_CLOCKEVENTS
    .timer = &amber3_timer,
#else
    .timer = &amber3_ptimer,
#endif
MACHINE_END

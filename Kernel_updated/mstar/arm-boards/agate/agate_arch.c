/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    PROJECT: agate

	FILE NAME: arch/arm/mach-agate/agate_arch.c

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

#include "chip_int.h"


/*------------------------------------------------------------------------------
    External Function Prototypes
-------------------------------------------------------------------------------*/
/* initialize agate's IRQ */
extern void agate_init_irq(void);
/* Add SD Card driver */
extern void __init agate_add_device_mmc(short mmc_id);
extern void __init agate_add_device_memstick(s16 memstick_id);
extern int Mstar_ehc_platform_init(void);


/*------------------------------------------------------------------------------
    External Global Variable
-------------------------------------------------------------------------------*/
/* agate's system timer */
extern struct sys_timer agate_timer;
extern struct sys_timer agate_ptimer;
/*------------------------------------------------------------------------------
    Global Variable
-------------------------------------------------------------------------------*/
/* platform device for agate_board_init() */
struct platform_device agate_device_usbgadget =
{
	.name		  = "agate-usbgadget",
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

/* platform device array for agate_board_init() */
static struct platform_device *agate_devices[] __initdata =
{
    &agate_device_usbgadget,
    &Mstar_emac_device,
    &Mstar_ir_device,
    &Mstar_nand_device,
    &Mstar_mbx_device,
    &Mstar_gflip_device,
    &Mstar_alsa_device,
};

/* IO tables for Registers, ITCM and DTCM */
static struct map_desc agate_io_desc[] __initdata =
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
		.virtual	= agate_ITCM_VIRT,
		.pfn		= __phys_to_pfn(agate_ITCM_PHYS),
		.length		= agate_ITCM_SIZE,
		.type		= MT_DEVICE
    },
	/* Define DTCM' physcial and virtual addresses */
	{
		.virtual	= agate_DTCM_VIRT,
		.pfn		= __phys_to_pfn(agate_DTCM_PHYS),
		.length		= agate_DTCM_SIZE,
		.type		= MT_DEVICE
	},
    /* Define MEM51' physcial and virtual addresses */
	{
		.virtual	= agate_MEM51_VIRT,
		.pfn		= __phys_to_pfn(agate_MEM51_PHY),
		.length		= agate_MEM51_SIZE,
		.type		= MT_MEMORY_NONCACHED
	}
	#endif
};

/*------------------------------------------------------------------------------
    Local Function
-------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Function: agate_map_io

    Description:
        init agate IO tables
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void __init agate_map_io(void)
{
#ifdef CONFIG_HAVE_ARM_TWD
    twd_base=((void __iomem *)PERI_ADDRESS(0x16000600)); //private timer& watchdog base
#endif    
    iotable_init(agate_io_desc, ARRAY_SIZE(agate_io_desc) );
}


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

#ifdef CONFIG_MSTAR_AGATE_BD_FPGA
    s.uartclk = 12000000; //FPGA
#elif  CONFIG_MSTAR_AGATE_BD_GENERIC  
    s.uartclk = 123000000; //real chip
#endif
    s.iotype = 0;
    s.regshift = 0;
    s.fifosize = 16 ; // use the 8 byte depth FIFO well
    s.line = 0;

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

    u1.iotype = 0;
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
irqreturn_t agate_xiu_timeout_interrupt(int i,void * j)
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

static struct irqaction agate_xiu_timeout_irq = {
    .name = "Agate Timeout IRQ",
    .flags = IRQF_TIMER | IRQF_IRQPOLL | IRQF_DISABLED,
    .handler = agate_xiu_timeout_interrupt,

};

/*------------------------------------------------------------------------------
    Function: agate_board_init

    Description:
        agate board init function
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
static void __init agate_board_init(void)
{
    #ifdef CONFIG_CACHE_L2X0
    unsigned int val = 0;
    void __iomem * l2x0_base;
    #endif 

    platform_add_devices(agate_devices, ARRAY_SIZE(agate_devices));

    //#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE) 
        Mstar_ehc_platform_init();
    //#endif

    #if defined(CONFIG_MEMSTICK_agate_MS)
        agate_add_device_memstick(0);
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
		#if defined(CONFIG_MSTAR_Master0_Only) ||  defined(CONFIG_MSTAR_Master1_Only)
    	writel(0x111, l2x0_base + L2X0_TAG_LATENCY_CTRL);
        #else
		writel(0x113, l2x0_base + L2X0_TAG_LATENCY_CTRL);
		#endif
		
    	writel(0x111, l2x0_base + L2X0_DATA_LATENCY_CTRL);
      	l2x0_init(l2x0_base, 0x00400000, 0xfe0fffff);
    #endif

	*(volatile unsigned short *)(0xfd200200) = 0x1 ;
	*(volatile unsigned short *)(0xfd200250) = 0xFFFF ;
	*(volatile unsigned short *)(0xfd200254) = 0xFFFF ;

        setup_irq(E_FIQ_XIU_TIMEOUT , &agate_xiu_timeout_irq);


    #endif
  
    #if 0  //switch UART RX pad
    reg_writew(0x0800, 0x1F001C24);
    #endif
     
    //serial_init();	
    /* Turn on Poseidon2 power as default */
    //val = reg_readw(0xA0003D60);
    //val |= 0x2;
    //reg_writew(val, 0xA0003D60);
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
//#ifdef CONFIG_MSTAR_AGATE
MACHINE_START(AGATE, "MStar MSB2510")
//#endif
    .map_io = agate_map_io,
    .init_irq = agate_init_irq,
    .init_machine = agate_board_init,
#ifdef CONFIG_GENERIC_CLOCKEVENTS
    .timer = &agate_timer,
#else
    .timer = &agate_ptimer,
#endif    
MACHINE_END

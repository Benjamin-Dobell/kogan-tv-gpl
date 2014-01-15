/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */
// #include <linux/config.h>
#include <linux/autoconf.h>
#include <linux/undefconf.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/tty.h>

#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>


#ifdef CONFIG_MTD
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#endif

#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>
#include <asm/dma.h>
#include <asm/time.h>
#include <asm/traps.h>

#include "chip_int.h"
#include "hwreg_S5.h"
#ifdef CONFIG_VT
#include <linux/console.h>
#endif

#include "board/Board.h"


const char display_string[] = "        LINUX ON Titania3     ";

extern void mips_reboot_setup(void);
extern void mips_time_init(void);
extern void mips_timer_setup(struct irqaction *irq);
extern unsigned long mips_rtc_get_time(void);

static void __init serial_init(void);

const char *get_system_type(void)
{
	return "MStar Titania3";
}

static int __init Titania3_setup(void)
{
       // int i;

	printk("Begin Titania3_setup\n");

    (*(volatile unsigned char *)(0xBF2025C0))=(unsigned char)(0x0);//for SWUpdateDelayCode for T3 only.

	serial_init();

	mips_reboot_setup();

	return 0;
}

void __init plat_mem_setup(void)
{
    Titania3_setup();
}

extern void Uart16550IntrruptEnable(void);
extern void Uart16550Put(unsigned char);
static void __init serial_init(void)
{
#ifdef CONFIG_SERIAL_8250
	struct uart_port s,u,f;

	memset(&s, 0, sizeof(s));

    Uart16550IntrruptEnable();
	//while(1) Uart16550Put('*');
	s.type = PORT_16550;
	s.iobase = 0xbf201300;
	// s.irq = E_IRQ_UART;
	s.irq = E_IRQ_UART0;

#ifdef CONFIG_MSTAR_TITANIA3

	// for FPGA
	// s.uartclk   = 12000000;
	//s.uartclk = 14318180;
	s.uartclk = 123000000;
	//s.uartclk = 144000000;
	//s.uartclk = 160000000;
	//s.uartclk = 172800000;
	//s.uartclk = 108000000;

#else

#ifdef CONFIG_MSTAR_TITANIA3_BD_FPGA
	//s.uartclk = FPGA_XTAL_CLOCK;    //12000000;///14318180;   //43200000;
	s.uartclk = 12000000 ;
#else
	s.uartclk = 123000000;
#endif

#endif
	s.iotype = 0;
	s.regshift = 0;
	s.fifosize = 16 ; // use the 8 byte depth FIFO well

	if (early_serial_setup(&s) != 0) {
		printk(KERN_ERR "Serial setup failed!\n");
	}

	// ------->hank.lai 2010.2.3
	memset(&u, 0, sizeof(u));

	u.type = PORT_16550;
	u.iobase = 0xBF220C00;
	u.irq = E_IRQEXPL_UART1;
	u.uartclk = 123000000;
	u.iotype = 0;
	u.regshift = 0;
	u.fifosize = 16 ; // use the 8 byte depth FIFO well

	u.line = 1;

	if (early_serial_setup(&u) != 0) {
		printk(KERN_ERR "Serial piu_uart1 setup failed!\n");
	}
	// <-------

	// ------->hank.lai 2010.2.11
	memset(&f, 0, sizeof(f));

	f.type = PORT_16550;
	f.iobase = 0xBF220D00;
	f.irq = E_IRQEXPH_UART2MCU;//E_IRQEXPL_UART2;
	f.uartclk = 123000000;
	f.iotype = 0;
	f.regshift = 0;
	f.fifosize = 16 ; // use the 8 byte depth FIFO well

	f.line = 2;

	if (early_serial_setup(&f) != 0) {
		printk(KERN_ERR "Serial piu_fuart setup failed!\n");
	}
	// <-------
#endif
}

extern unsigned long get_BBAddr(void);
void Chip_Flush_Memory(void)
{
    static unsigned int u32_4Lines[64];
    volatile unsigned int *pu8;
    volatile unsigned int tmp ;  // Fixme: temporary mark this code.
    unsigned int LX_MEM2_addr, LX_MEM2_size = 0;

    // Transfer the memory to noncache memory
    pu8 = ((volatile unsigned int *)(((unsigned int)u32_4Lines) | 0xa0000000));

    // Flush the data from pipe and buffer in MIU0
    pu8[0] = pu8[16] = pu8[32] = pu8[48] = 1;

    // Flush the data in the EC bridge buffer
    mb();

    // final read
    tmp = pu8[0] ;
    tmp = pu8[16] ;
    tmp = pu8[32] ;
    tmp = pu8[48] ;

    get_boot_mem_info(LINUX_MEM2, &LX_MEM2_addr, &LX_MEM2_size);
    // Transfer the memory to noncache memory
    if(get_BBAddr()!=0) //with MIU1, remember to set BBAddr
    {
        pu8 =(volatile unsigned int*)(((unsigned int)get_BBAddr()) | 0xD0000000);
        // Flush the data from pipe and buffer in MIU1
        pu8[0] = pu8[16] = pu8[32] = pu8[48] = 1;

        // Flush the data in the EC bridge buffer
        mb();

        // final read
        tmp = pu8[0] ;
        tmp = pu8[16] ;
        tmp = pu8[32] ;
        tmp = pu8[48] ;

    }
}

void Chip_Read_Memory(void)
{
    volatile unsigned int *pu8;
    volatile unsigned int t ;
    unsigned int LX_MEM2_addr, LX_MEM2_size = 0;

    // Transfer the memory to noncache memory
    pu8 = ((volatile unsigned int *)0xA0380000);
    t = pu8[0] ;
    t = pu8[64] ;

    // Fixme: temporary mark this code.
    get_boot_mem_info(LINUX_MEM2, &LX_MEM2_addr, &LX_MEM2_size);
    if(LX_MEM2_size!=0)
    {
        pu8 = ((volatile unsigned int *)0xD0380000);
        t = pu8[0] ;
        t = pu8[64] ;
    }
}


unsigned int Chip_Query_MIPS_CLK(void)
{
    unsigned int u32Count = 0;
    unsigned int uSpeed_L = 0, uSpeed_H = 0;

    uSpeed_L = *(volatile u32*)(0xBF221864);
    uSpeed_H = *(volatile u32*)(0xBF221868);

    if(uSpeed_H==0x1c)
    {
        //printk("(CPU Speed is 12 MHz)\n");
        u32Count = 12000000;
    }
    else if(uSpeed_L==0x14)
    {
        //printk("(CPU Speed %d MHz)\n", (int)(uSpeed_H*24));
        u32Count = uSpeed_H*24*1000000;
    }
    else if(uSpeed_L==0x18)
    {
        //printk("(CPU Speed %d MHz setting)\n", (int)(uSpeed_H*48));
        u32Count = uSpeed_H*48*1000000;
    }

    return u32Count;
}

EXPORT_SYMBOL(Chip_Flush_Memory);
EXPORT_SYMBOL(Chip_Read_Memory);


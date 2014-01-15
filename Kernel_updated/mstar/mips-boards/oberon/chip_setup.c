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
#ifdef CONFIG_VT
#include <linux/console.h>
#endif

#include "board/Board.h"

const char display_string[] = "        LINUX ON Oberon       ";
/******************************************************************************/
/* MEMORY SETTING                                                             */
/******************************************************************************/
const unsigned long LINUX_MEMORY_ADDRESS=LINUX_MEM2_BASE_ADR;
const unsigned long LINUX_MEMORY_LENGTH=LINUX_MEM2_LEN;

/******************************************************************************/
/* Macro for bitwise                                                          */
/******************************************************************************/
#define _BITMASK(loc_msb, loc_lsb) ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))
#define BITMASK(x) _BITMASK(1?x, 0?x)

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

//------------------------------------------------------------------------------

extern void mips_reboot_setup(void);
extern void mips_time_init(void);
extern void mips_timer_setup(struct irqaction *irq);
extern unsigned long mips_rtc_get_time(void);

static void __init serial_init(void);
static void __init chiptop_init(void);
static void __init pad_config(void);
static void __init miu_priority_config(void);
static void __init miu_assignment(void);



const char *get_system_type(void)
{
	return "MStar Oberon";
}

extern int MDrv_SYS_GetMMAP(int type, unsigned int *addr, unsigned int *len) ;

static int __init Oberon_setup(void)
{

	printk("Begin Oberon_setup\n");
	ioport_resource.end = 0x7fffffff;
	serial_init();
    pad_config();
    mips_reboot_setup();
	return 0;
}

void __init plat_mem_setup(void)
{
    Oberon_setup();
}
extern void Uart16550IntrruptEnable(void);
extern void Uart16550Put(unsigned char);

#define UART_REG(addr)          (*(volatile unsigned int*)((addr)))
#define OBERON_UART_BASE        0xbf808c80
#define OBERON_UART_RX          (0*4 + OBERON_UART_BASE)
#define OBERON_UART_TX          (0*4 + OBERON_UART_BASE)
#define OBERON_UART_DLL         (0*4 + OBERON_UART_BASE)
#define OBERON_UART_DLM         (1*4 + OBERON_UART_BASE)
#define OBERON_UART_IER         (1*4 + OBERON_UART_BASE)
#define OBERON_UART_IIR         (2*4 + OBERON_UART_BASE)
#define OBERON_UART_FCR         (2*4 + OBERON_UART_BASE)
#define OBERON_UART_EFR         (2*4 + OBERON_UART_BASE)
#define OBERON_UART_LCR         (3*4 + OBERON_UART_BASE)
#define OBERON_UART_MCR         (4*4 + OBERON_UART_BASE)
#define OBERON_UART_LSR         (5*4 + OBERON_UART_BASE)
#define OBERON_UART_MSR         (6*4 + OBERON_UART_BASE)
#define OBERON_UART_SCR         (7*4 + OBERON_UART_BASE)
#define OBERON_UART_SCCR_RST    (8*4 + OBERON_UART_BASE)

//Baud rate
#define OBERON_UART_BAUD_RATE  115200

//CPU clock
#define OBERON_CPU_CLOCK_FREQ  216000000

//Clock source
#define OBERON_UART_CLK_FREQ   43200000

//Divisor
#define OBERON_UART_DIVISOR    (OBERON_UART_CLK_FREQ/(16 * OBERON_UART_BAUD_RATE))
void init_chip_uart(void)
{
    //UART1 INIT:

    //Disable all UART interrupts
    UART_REG(OBERON_UART_IER) = 0x00;

    //Reset receiver and transmitr
    UART_REG(OBERON_UART_FCR) = 0x07;

    //Set 8 bit char, 1 stop bit, no parity
    UART_REG(OBERON_UART_LCR) = 0x03;

    //Set baud rate
    UART_REG(OBERON_UART_LCR) = 0x83;
    UART_REG(OBERON_UART_DLM) = (OBERON_UART_DIVISOR >> 8) & 0x00ff;
    UART_REG(OBERON_UART_DLL) = OBERON_UART_DIVISOR & 0x00ff;
    UART_REG(OBERON_UART_LCR) = 0x03;

    //enable reg_uart_rx_enable;
    *(volatile unsigned int*)(0xbf800000+(0x0F01*4)) |= (1<<10);//BIT10;
    UART_REG(OBERON_UART_SCCR_RST) = 0x10;// Receive enable & reset
}
static void __init serial_init(void)
{

#ifdef CONFIG_SERIAL_8250
	struct uart_port s;

	memset(&s, 0, sizeof(s));

    Uart16550IntrruptEnable();
	s.type = PORT_16550;
	s.iobase = OBERON_UART_BASE;
	s.irq = E_IRQ_UART1;
    UART_REG(OBERON_UART_IER) = 0x01;

	//s.uartclk = 14318180;
	//s.uartclk = 123000000;
	//s.uartclk = 144000000;
	//s.uartclk = 160000000;
	//s.uartclk = 172800000;
	//s.uartclk = 12000000;
	//s.uartclk = 108000000;
	  s.uartclk = 43200000;

	s.iotype = 0;
	s.regshift = 0;
	s.fifosize = 16 ; // use the 8 byte depth FIFO well

	if (early_serial_setup(&s) != 0) {
		printk(KERN_ERR "Serial setup failed!\n");
	}
#endif
}

static void __init chiptop_init(void)
{

}

static void __init pad_config(void)
{
    TOP_REG(REG_TOP_PCI_PD_TEST) = (TOP_REG(REG_TOP_PCI_PD_TEST) & ~(TOP_TURN_OFF_PAD));
}

static void __init miu_priority_config(void)
{
}

static void __init miu_assignment(void)
{
}

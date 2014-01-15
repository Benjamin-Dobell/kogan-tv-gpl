/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000, 2001, 2004 MIPS Technologies, Inc.
 * Copyright (C) 2001 Ralf Baechle
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
 *
 * Routines for generic manipulation of the interrupts found on the MIPS
 * Malta board.
 * The interrupt controller is located in the South Bridge a PIIX4 device
 * with two internal 82C95 interrupt controllers.
 */
//#include <linux/config.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/random.h>

//#include <asm/i8259.h>
#include <asm/io.h>

#include <asm/mips-boards/malta.h>
#include <asm/mips-boards/maltaint.h>
#include <asm/mips-boards/piix4.h>
#include <asm/gt64120.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/msc01_pci.h>
#include "chip_int.h"

//#define REG(addr) (*(volatile unsigned int *)(addr))
static int Oberon_EnableInterrupt (InterruptNum eIntNum)
{
    int bRet = true;

    if (eIntNum == E_IRQ_FIQ_ALL)
    {
        IRQ_REG(REG_IRQ_MASK_L) &= ~IRQL_MASK;
        IRQ_REG(REG_IRQ_MASK_H) &= ~IRQH_MASK;
        IRQ_REG(REG_FIQ_MASK_L) &= ~FIQL_MASK;
        IRQ_REG(REG_FIQ_MASK_H) &= ~FIQH_MASK;
    }
    else if ( (eIntNum >= E_IRQL_START) && (eIntNum <= E_IRQL_END) )
    {
        IRQ_REG(REG_IRQ_MASK_L) &= ~(0x1 << (eIntNum-E_IRQL_START) );
    }
    else if ( (eIntNum >= E_IRQH_START) && (eIntNum <= E_IRQH_END) )
    {
        IRQ_REG(REG_IRQ_MASK_H) &= ~(0x1 << (eIntNum-E_IRQH_START) );
    }
    else if ( (eIntNum >= E_FIQL_START) && (eIntNum <= E_FIQL_END) )
    {
        IRQ_REG(REG_FIQ_MASK_L) &= ~(0x1 << (eIntNum-E_FIQL_START) );
    }
    else if ( (eIntNum >= E_FIQH_START) && (eIntNum <= E_FIQH_END) )
    {
        IRQ_REG(REG_FIQ_MASK_H) &= ~(0x1 << (eIntNum-E_FIQH_START) );
    }

    return bRet;
}


static int Oberon_DisableInterrupt (InterruptNum eIntNum)
{
    if (eIntNum == E_IRQ_FIQ_ALL)
    {
        IRQ_REG(REG_IRQ_MASK_L) |= IRQL_MASK;
        IRQ_REG(REG_IRQ_MASK_H) |= IRQH_MASK;
        IRQ_REG(REG_FIQ_MASK_L) |= FIQL_MASK;
        IRQ_REG(REG_FIQ_MASK_H) |= FIQH_MASK;
    }
    else if ( (eIntNum >= E_IRQL_START) && (eIntNum <= E_IRQL_END) )
    {
        IRQ_REG(REG_IRQ_MASK_L) |= (0x1 << (eIntNum-E_IRQL_START) );
    }
    else if ( (eIntNum >= E_IRQH_START) && (eIntNum <= E_IRQH_END) )
    {
        IRQ_REG(REG_IRQ_MASK_H) |= (0x1 << (eIntNum-E_IRQH_START) );
    }
    else if ( (eIntNum >= E_FIQL_START) && (eIntNum <= E_FIQL_END) )
    {
        IRQ_REG(REG_FIQ_MASK_L) |= (0x1 << (eIntNum-E_FIQL_START) );
        IRQ_REG(REG_FIQ_CLEAR_L) = (0x1 << (eIntNum-E_FIQL_START) );
        IRQ_REG(REG_FIQ_CLEAR_L) = 0;
    }
    else if ( (eIntNum >= E_FIQH_START) && (eIntNum <= E_FIQH_END) )
    {
        IRQ_REG(REG_FIQ_MASK_H) |= (0x1 << (eIntNum-E_FIQH_START) );
        IRQ_REG(REG_FIQ_CLEAR_H) = (0x1 << (eIntNum-E_FIQH_START) );
        IRQ_REG(REG_FIQ_CLEAR_H) = 0;
    }

    return true;
}



static unsigned int startup_oberon_irq(unsigned int irq)
{

	//enable_lasat_irq(irq);
    Oberon_EnableInterrupt((InterruptNum)irq);
	return 0; /* never anything pending */
}

static void shutdown_oberon_irq(unsigned int irq)
{

	//enable_lasat_irq(irq);
    Oberon_DisableInterrupt((InterruptNum)irq);
	//return 0; /* never anything pending */
}

#define enable_oberon_irq startup_oberon_irq
#define disable_oberon_irq shutdown_oberon_irq
#define mask_and_ack_oberon_irq shutdown_oberon_irq
#define end_oberon_irq startup_oberon_irq

static unsigned int startup_oberon_fiq(unsigned int irq)
{

	//enable_lasat_irq(irq);
    Oberon_EnableInterrupt((InterruptNum)irq);
	return 0; /* never anything pending */
}

static void shutdown_oberon_fiq(unsigned int irq)
{

	//enable_lasat_irq(irq);
    Oberon_DisableInterrupt((InterruptNum)irq);
	//return 0; /* never anything pending */
}

#define enable_oberon_fiq startup_oberon_fiq
#define disable_oberon_fiq shutdown_oberon_fiq
#define mask_and_ack_oberon_fiq shutdown_oberon_fiq
#define end_oberon_fiq startup_oberon_fiq




// Dean modify to match kernel 2.6.26
static struct hw_interrupt_type oberon_irq_type = {
	.name = "Oberon IRQ",
	.startup = startup_oberon_irq,
	.shutdown = shutdown_oberon_irq,
	.enable = (void (*)(unsigned int irq))enable_oberon_irq,
	.disable = disable_oberon_irq,
	.ack = mask_and_ack_oberon_irq,
	.mask = disable_oberon_irq,
	.mask_ack = disable_oberon_irq,
	.unmask = (void (*)(unsigned int irq))enable_oberon_irq,
	.eoi = (void (*)(unsigned int irq))enable_oberon_irq,
	.end = (void (*)(unsigned int irq))end_oberon_irq
};
static struct hw_interrupt_type oberon_fiq_type = {
	.name = "Oberon IRQ",
	.startup = startup_oberon_fiq,
	.shutdown = shutdown_oberon_fiq,
	.enable = (void (*)(unsigned int irq))enable_oberon_fiq,
	.disable = disable_oberon_fiq,
	.ack = mask_and_ack_oberon_fiq,
	.mask = disable_oberon_fiq,
	.mask_ack = disable_oberon_fiq,
	.unmask = (void (*)(unsigned int irq))enable_oberon_fiq,
	.eoi = (void (*)(unsigned int irq))enable_oberon_fiq,
	.end = (void (*)(unsigned int irq))end_oberon_fiq
};


// void Oberon_hw0_irqdispatch(struct pt_regs *regs)
void Oberon_hw0_irqdispatch(void)
{
	unsigned short u16Reglow,u16Reghigh;

	u16Reglow = (unsigned short)IRQ_REG(REG_IRQ_PENDING_L);
	u16Reghigh = (unsigned short)IRQ_REG(REG_IRQ_PENDING_H);

    //u16Reglow += MSTAR_INT_BASE;
    if ( u16Reglow & IRQL_UHC )
    {
    	    do_IRQ((unsigned int)E_IRQ_UHC);
    }
    if ( u16Reglow & IRQL_DEB )
    {
    	do_IRQ((unsigned int)E_IRQ_DEB);
    }
    if ( u16Reglow & IRQL_UART0 )
    {
		do_IRQ((unsigned int)E_IRQ_UART0);
    }
    if ( u16Reglow & IRQL_UART1 )
    {
		do_IRQ((unsigned int)E_IRQ_UART1);
    }
    if ( u16Reglow & IRQL_UART2 )
    {
 		do_IRQ((unsigned int)E_IRQ_UART2);
    }
    if(u16Reglow & IRQL_EMAC)
	{
		do_IRQ((unsigned int)E_IRQ_EMAC);
    } // if

    if ( u16Reghigh & IRQH_TSP )
    {
		do_IRQ((unsigned int)E_IRQ_TSP);

    }

    if ( u16Reghigh & IRQH_HDMITX )
    {
		do_IRQ((unsigned int)E_IRQ_HDMITX);
    }
    if ( u16Reghigh & IRQH_GOP )
    {
		do_IRQ((unsigned int)E_IRQ_GOP);
    }
    if ( u16Reghigh & IRQH_PCM2MCU )
    {
		do_IRQ((unsigned int)E_IRQ_PCM2MCU);
    }
    if ( u16Reghigh & IRQH_RTC )
    {
		do_IRQ((unsigned int)E_IRQ_RTC);
    }
}


// void Oberon_hw0_fiqdispatch(struct pt_regs *regs)
void Oberon_hw0_fiqdispatch(void)
{
    unsigned short u16Reg;

    u16Reg = IRQ_REG(REG_FIQ_PENDING_H);

    //u16Reg += MSTAR_INT_BASE;

    if ( u16Reg & FIQH_DSP2UP )
    {
        do_IRQ((unsigned int)E_FIQ_DSP2UP);
    }
    if ( u16Reg & FIQH_IR )
    {
        do_IRQ((unsigned int)E_FIQ_IR);
    }

    u16Reg = IRQ_REG(REG_FIQ_PENDING_L);

    //u16Reg += MSTAR_INT_BASE;


}

static inline int clz(unsigned long x)
{
	__asm__(
	"	.set	push					\n"
	"	.set	mips32					\n"
	"	clz	%0, %1					\n"
	"	.set	pop					\n"
	: "=r" (x)
	: "r" (x));

	return x;
}

/*
 * Version of ffs that only looks at bits 12..15.
 */
static inline unsigned int irq_ffs(unsigned int pending)
{
#if defined(CONFIG_CPU_MIPS32) || defined(CONFIG_CPU_MIPS64)
	return -clz(pending) + 31 - CAUSEB_IP;
#else
	unsigned int a0 = 7;
	unsigned int t0;

	t0 = s0 & 0xf000;
	t0 = t0 < 1;
	t0 = t0 << 2;
	a0 = a0 - t0;
	s0 = s0 << t0;

	t0 = s0 & 0xc000;
	t0 = t0 < 1;
	t0 = t0 << 1;
	a0 = a0 - t0;
	s0 = s0 << t0;

	t0 = s0 & 0x8000;
	t0 = t0 < 1;
	//t0 = t0 << 2;
	a0 = a0 - t0;
	//s0 = s0 << t0;

	return a0;
#endif
}

/*
 * IRQs on the SEAD board look basically are combined together on hardware
 * interrupt 0 (MIPS IRQ 2)) like:
 *
 *	MIPS IRQ	Source
 *      --------        ------
 *             0	Software (ignored)
 *             1        Software (ignored)
 *             2        UART0 (hw0)
 *             3        UART1 (hw1)
 *             4        Hardware (ignored)
 *             5        Hardware (ignored)
 *             6        Hardware (ignored)
 *             7        R4k timer (what we use)
 *
 * We handle the IRQ according to _our_ priority which is:
 *
 * Highest ----     R4k Timer
 * Lowest  ----     Combined hardware interrupt
 *
 * then we just return, if multiple IRQs are pending then we will just take
 * another exception, big deal.
 */
asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pending = read_c0_cause() & read_c0_status() & ST0_IM;
	int irq;

	irq = irq_ffs(pending);
       //printk("\nDean --> [%s] %d irq = %d\n", __FILE__, __LINE__, irq);
       //if(irq != 7)
            //printk("\nirq = %d\n", irq);
       if (irq == 2)
              Oberon_hw0_irqdispatch();
       else if (irq == 3)
              Oberon_hw0_fiqdispatch();
	else if (irq >= 0)
		do_IRQ(MIPS_CPU_IRQ_BASE + irq);
	else
		spurious_interrupt();
       //printk("\nDean --> [%s] %d irq = %d\n", __FILE__, __LINE__, irq);
}

extern void __init mips_cpu_irq_init(void);
void __init arch_init_irq(void)
{
    int i;

       for (i = MSTAR_INT_BASE; i <= (OBERONINT_END+MSTAR_INT_BASE); i++) {
            if ( i <64+MSTAR_INT_BASE)
                set_irq_chip_and_handler(i, &oberon_irq_type, handle_level_irq);
            else
                set_irq_chip_and_handler(i, &oberon_irq_type, handle_level_irq);
       }
       set_irq_chip_and_handler(0, &oberon_fiq_type, handle_level_irq);
       set_irq_chip_and_handler(1, &oberon_fiq_type, handle_level_irq);
	//disable all
	Oberon_DisableInterrupt(E_IRQ_FIQ_ALL);

       mips_cpu_irq_init();
}

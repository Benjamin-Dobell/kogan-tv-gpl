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
#include <linux/config.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/random.h>

#include <asm/i8259.h>
#include <asm/io.h>

#include <asm/mips-boards/malta.h>
#include <asm/mips-boards/maltaint.h>
#include <asm/mips-boards/piix4.h>
#include <asm/gt64120.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/msc01_pci.h>
#include "chip_int.h"
extern asmlinkage void mipsIRQ(void);

#define REG(addr) (*(volatile unsigned int *)(addr))

//static raw_spinlock_t mips_irq_lock = RAW_SPIN_LOCK_UNLOCKED;


static int Titania_EnableInterrupt (InterruptNum eIntNum)
{
    int bRet = 0;

    if (eIntNum == E_IRQ_FIQ_ALL)
    {
            REG(REG_IRQ_MASK_L) &= ~IRQL_ALL;
            REG(REG_IRQ_MASK_H) &= ~IRQH_ALL;
            REG(REG_FIQ_MASK_L) &= ~FIQL_ALL;
            REG(REG_FIQ_MASK_H) &= ~FIQH_ALL;
    }
    else if ( (eIntNum >= E_IRQL_START) && (eIntNum <= E_IRQL_END) )
    {
            REG(REG_IRQ_MASK_L) &= ~(0x1 << (eIntNum-E_IRQL_START) );
    }
    else if ( (eIntNum >= E_IRQH_START) && (eIntNum <= E_IRQH_END) )
    {
            REG(REG_IRQ_MASK_H) &= ~(0x1 << (eIntNum-E_IRQH_START) );
    }
    else if ( (eIntNum >= E_FIQL_START) && (eIntNum <= E_FIQL_END) )
    {
            REG(REG_FIQ_MASK_L) &= ~(0x1 << (eIntNum-E_FIQL_START) );
    }
    else if ( (eIntNum >= E_FIQH_START) && (eIntNum <= E_FIQH_END) )
    {
            REG(REG_FIQ_MASK_H) &= ~(0x1 << (eIntNum-E_FIQH_START) );
    }

    return bRet;
}


static int Titania_DisableInterrupt (InterruptNum eIntNum)
{

    if (eIntNum == E_IRQ_FIQ_ALL)
    {
        REG(REG_IRQ_MASK_L) |= IRQL_ALL;
        REG(REG_IRQ_MASK_H) |= IRQH_ALL;
        REG(REG_FIQ_MASK_L) |= FIQL_ALL;
        REG(REG_FIQ_MASK_H) |= FIQH_ALL;
    }
    else if ( (eIntNum >= E_IRQL_START) && (eIntNum <= E_IRQL_END) )
    {
        REG(REG_IRQ_MASK_L) |= (0x1 << (eIntNum-E_IRQL_START) );
    }
    else if ( (eIntNum >= E_IRQH_START) && (eIntNum <= E_IRQH_END) )
    {
        REG(REG_IRQ_MASK_H) |= (0x1 << (eIntNum-E_IRQH_START) );
    }
    else if ( (eIntNum >= E_FIQL_START) && (eIntNum <= E_FIQL_END) )
    {
        REG(REG_FIQ_MASK_L) |= (0x1 << (eIntNum-E_FIQL_START) );
        REG(REG_FIQ_CLEAR_L) |= (0x1 << (eIntNum-E_FIQL_START) );
        REG(REG_FIQ_CLEAR_L) &= ~(0x1 << (eIntNum-E_FIQL_START) );
    }
    else if ( (eIntNum >= E_FIQH_START) && (eIntNum <= E_FIQH_END) )
    {
        REG(REG_FIQ_MASK_H) |= (0x1 << (eIntNum-E_FIQH_START) );
        REG(REG_FIQ_CLEAR_H) |= (0x1 << (eIntNum-E_FIQH_START) );
        REG(REG_FIQ_CLEAR_H) &= ~(0x1 << (eIntNum-E_FIQH_START) );
    }

    return 0;
}



static unsigned int startup_titania_irq(unsigned int irq)
{
	//enable_lasat_irq(irq);
    Titania_EnableInterrupt((InterruptNum)irq);
	return 0; /* never anything pending */
}

static void shutdown_titania_irq(unsigned int irq)
{
	//enable_lasat_irq(irq);
    Titania_DisableInterrupt((InterruptNum)irq);
	//return 0; /* never anything pending */
}

#define enable_titania_irq startup_titania_irq
#define disable_titania_irq shutdown_titania_irq
#define mask_and_ack_titania_irq shutdown_titania_irq
#define end_titania_irq startup_titania_irq

static unsigned int startup_titania_fiq(unsigned int irq)
{
	//enable_lasat_irq(irq);
    Titania_EnableInterrupt((InterruptNum)irq);
	return 0; /* never anything pending */
}

static void shutdown_titania_fiq(unsigned int irq)
{
	//enable_lasat_irq(irq);
    Titania_DisableInterrupt((InterruptNum)irq);
	//return 0; /* never anything pending */
}

#define enable_titania_fiq startup_titania_fiq
#define disable_titania_fiq shutdown_titania_fiq
#define mask_and_ack_titania_fiq shutdown_titania_fiq
#define end_titania_fiq startup_titania_fiq





static struct hw_interrupt_type titania_irq_type = {
	"Titania IRQ",
	startup_titania_irq,
	shutdown_titania_irq,
	(void (*)(unsigned int irq))enable_titania_irq,
	disable_titania_irq,
	mask_and_ack_titania_irq,
	(void (*)(unsigned int irq))end_titania_irq,
	NULL
};
static struct hw_interrupt_type titania_fiq_type = {
	"Titania FIQ",
	startup_titania_fiq,
	shutdown_titania_fiq,
	(void (*)(unsigned int irq))enable_titania_fiq,
	disable_titania_fiq,
	mask_and_ack_titania_fiq,
	(void (*)(unsigned int irq))end_titania_fiq,
	NULL
};





void Titania_hw0_irqdispatch(struct pt_regs *regs)
{
	unsigned short u16Reglow,u16Reghigh;

	u16Reglow = (unsigned short)REG(REG_IRQ_PENDING_L);
	u16Reghigh = (unsigned short)REG(REG_IRQ_PENDING_H);

    if ( u16Reglow & IRQL_UART )
    {
		do_IRQ((unsigned int)E_IRQ_UART, regs);
    }

   if ( u16Reghigh & IRQH_MVD )
    {
		do_IRQ((unsigned int)E_IRQ_MVD, regs);
    }

    if ( u16Reglow & IRQL_UHC )
    {
    	do_IRQ((unsigned int)E_IRQ_UHC , regs);
    }
    if ( u16Reglow & IRQL_DEB )
    {
    	do_IRQ((unsigned int)E_IRQ_DEB , regs);
    }
    if ( u16Reglow & IRQL_UART0 )
    {
		do_IRQ((unsigned int)E_IRQ_UART0, regs);
    }
    if ( u16Reglow & IRQL_UART1 )
    {
		do_IRQ((unsigned int)E_IRQ_UART1 , regs);
    }
    if ( u16Reglow & IRQL_UART2 )
    {
 		do_IRQ((unsigned int)E_IRQ_UART2 , regs);
    }
    if(u16Reglow & IRQL_EMAC)
	{
		do_IRQ((unsigned int)E_IRQ_EMAC, regs);
    } // if

    if(u16Reglow & IRQL_8051_LITE)
	{
		do_IRQ((unsigned int)E_IRQ_8051_LITE, regs);
    }
    if ( u16Reglow & IRQL_SVD )
    {
    	do_IRQ((unsigned int)E_IRQ_SVD , regs);
    }

    if( u16Reglow & IRQL_VBI )
    {
    	do_IRQ((unsigned int)E_IRQ_VBI , regs);
    }

    if ( u16Reghigh & IRQH_TSP )
    {
		do_IRQ((unsigned int)E_IRQ_TSP , regs);

    }

    if ( u16Reghigh & IRQH_MLINK )
    {
        do_IRQ((unsigned int)E_IRQ_MLINK , regs);
    }

    if ( u16Reghigh & IRQH_HDMITX )
    {
		do_IRQ((unsigned int)E_IRQ_HDMITX, regs);
    }
    if ( u16Reghigh & IRQH_GOP )
    {
		do_IRQ((unsigned int)E_IRQ_GOP , regs);
    }
    if ( u16Reghigh & IRQH_PCM2MCU )
    {
		do_IRQ((unsigned int)E_IRQ_PCM2MCU, regs);
    }
    if ( u16Reghigh & IRQH_RTC )
    {
		do_IRQ((unsigned int)E_IRQ_RTC , regs);
    }


	//do_IRQ(irq, regs);
}


void Titania_hw0_fiqdispatch(struct pt_regs *regs)
{
    unsigned short u16Reg;

    u16Reg = REG(REG_FIQ_PENDING_H);

    if ( u16Reg & FIQH_VSYN_GOP1 )
    {
        do_IRQ((unsigned int)E_FIQ_VSYN_GOP1 , regs);
    }
    if ( u16Reg & FIQH_VSYN_GOP0 )
    {
        do_IRQ((unsigned int)E_FIQ_VSYN_GOP0 , regs);
    }
    if ( u16Reg & FIQH_DSP2UP )
    {
        do_IRQ((unsigned int)E_FIQ_DSP2UP, regs);
    }
    if ( u16Reg & FIQH_IR )
    {
        do_IRQ((unsigned int)E_FIQ_IR, regs);
    }
    if ( u16Reg & FIQH_DSP2MIPS )
    {
        do_IRQ((unsigned int)E_FIQ_DSP2MIPS, regs);
    }
    if ( u16Reg & FIQH_VSYNC_VE4VBI )
    {
        do_IRQ((unsigned int)E_FIQ_VSYNC_VE4VBI, regs);
    }
    if ( u16Reg & FIQH_FIELD_VE4VBI )
    {
        do_IRQ((unsigned int)E_FIQ_FIELD_VE4VBI, regs);
    }

    u16Reg = REG(REG_FIQ_PENDING_L);

    if ( u16Reg & FIQL_AEON_TO_8051 )
    {
        do_IRQ((unsigned int)E_FIQ_INT_AEON_TO_8051, regs);
    }
    if ( u16Reg & FIQL_8051_TO_AEON )
    {
        do_IRQ((unsigned int)E_FIQ_INT_8051_TO_AEON, regs);
    }
    if ( u16Reg & FIQL_8051_TO_MIPS )
    {
        do_IRQ((unsigned int)E_FIQ_INT_8051_TO_MIPS, regs);
    }
    if ( u16Reg & FIQL_MIPS_TO_8051 )
    {
        do_IRQ((unsigned int)E_FIQ_INT_MIPS_TO_8051, regs);
    }
    if ( u16Reg & FIQL_MIPS_TO_AEON )
    {
        do_IRQ((unsigned int)E_FIQ_INT_MIPS_TO_AEON, regs);
    }
    if ( u16Reg & FIQL_AEON_TO_MIPS )
    {
        do_IRQ((unsigned int)E_FIQ_INT_AEON_TO_MIPS, regs);
    }
}


void __init arch_init_irq(void)
{
    int i;
	set_except_vector(0, mipsIRQ);
	//mips_cpu_irq_init(0);
	for (i = 0; i <= TITANIAINT_END; i++) {
		irq_desc[i].status	= IRQ_DISABLED;
		irq_desc[i].action	= 0;
		irq_desc[i].depth	= 1;
		if ( i <32 )
		   irq_desc[i].handler	= &titania_irq_type;
		else
		   irq_desc[i].handler  = &titania_fiq_type;
	}
	//disable all
	Titania_DisableInterrupt(E_IRQ_FIQ_ALL);
}

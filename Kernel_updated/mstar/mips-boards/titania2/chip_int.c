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

#define REG(addr) (*(volatile unsigned int *)(addr))

#define DO_IRQ_FUNC(reg,num,base) if((reg) & (0x1 << ((num) - (base))))do_IRQ((unsigned int)(num));
#define DO_IRQL(reg,num)    DO_IRQ_FUNC((reg),(num),E_IRQL_START)
#define DO_IRQH(reg,num)    DO_IRQ_FUNC((reg),(num),E_IRQH_START)
#define DO_FIQL(reg,num)    DO_IRQ_FUNC((reg),(num),E_FIQL_START)
#define DO_FIQH(reg,num)    DO_IRQ_FUNC((reg),(num),E_FIQH_START)
#define DO_IRQLEXP(reg,num) DO_IRQ_FUNC((reg),(num),E_IRQL_EXP_START)

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

            REG(REG_IRQ_EXP_MASK_L) &= ~IRQL_EXP_ALL;
            REG(REG_IRQ_EXP_MASK_H) &= ~IRQH_EXP_ALL;
            REG(REG_FIQ_EXP_MASK_L) &= ~FIQL_EXP_ALL;
            REG(REG_FIQ_EXP_MASK_H) &= ~FIQH_EXP_ALL;
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
    else if ( (eIntNum >= E_IRQL_EXP_START) && (eIntNum <= E_IRQL_EXP_END) )
    {
            REG(REG_IRQ_EXP_MASK_L) &= ~(0x1 << (eIntNum-E_IRQL_EXP_START) );
    }
    else if ( (eIntNum >= E_IRQH_EXP_START) && (eIntNum <= E_IRQH_EXP_END) )
    {
            REG(REG_IRQ_EXP_MASK_H) &= ~(0x1 << (eIntNum-E_IRQH_EXP_START) );
    }
    else if ( (eIntNum >= E_FIQL_EXP_START) && (eIntNum <= E_FIQL_EXP_END) )
    {
            REG(REG_FIQ_EXP_MASK_L) &= ~(0x1 << (eIntNum-E_FIQL_EXP_START) );
    }
    else if ( (eIntNum >= E_FIQH_EXP_START) && (eIntNum <= E_FIQH_EXP_END) )
    {
            REG(REG_FIQ_EXP_MASK_H) &= ~(0x1 << (eIntNum-E_FIQH_EXP_START) );
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

        REG(REG_IRQ_EXP_MASK_L) |= IRQL_EXP_ALL;
        REG(REG_IRQ_EXP_MASK_H) |= IRQH_EXP_ALL;
        REG(REG_FIQ_EXP_MASK_L) |= FIQL_EXP_ALL;
        REG(REG_FIQ_EXP_MASK_H) |= FIQH_EXP_ALL;
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
        REG(REG_FIQ_CLEAR_L) = (0x1 << (eIntNum-E_FIQL_START) );
        REG(REG_FIQ_CLEAR_L) = 0;
    }
    else if ( (eIntNum >= E_FIQH_START) && (eIntNum <= E_FIQH_END) )
    {
        REG(REG_FIQ_MASK_H) |= (0x1 << (eIntNum-E_FIQH_START) );
        REG(REG_FIQ_CLEAR_H) = (0x1 << (eIntNum-E_FIQH_START) );
        REG(REG_FIQ_CLEAR_H) = 0;
    }
    else if ( (eIntNum >= E_IRQL_EXP_START) && (eIntNum <= E_IRQL_EXP_END) )
    {
        REG(REG_IRQ_EXP_MASK_L) |= (0x1 << (eIntNum-E_IRQL_EXP_START) );
    }
    else if ( (eIntNum >= E_IRQH_EXP_START) && (eIntNum <= E_IRQH_EXP_END) )
    {
        REG(REG_IRQ_EXP_MASK_H) |= (0x1 << (eIntNum-E_IRQH_EXP_START) );
    }
    else if ( (eIntNum >= E_FIQL_EXP_START) && (eIntNum <= E_FIQL_EXP_END) )
    {
        REG(REG_FIQ_EXP_MASK_L) |= (0x1 << (eIntNum-E_FIQL_EXP_START) );
        REG(REG_FIQ_EXP_CLEAR_L) = (0x1 << (eIntNum-E_FIQL_EXP_START) );
        REG(REG_FIQ_EXP_CLEAR_L) = 0;
    }
    else if ( (eIntNum >= E_FIQH_EXP_START) && (eIntNum <= E_FIQH_EXP_END) )
    {
        REG(REG_FIQ_EXP_MASK_H) |= (0x1 << (eIntNum-E_FIQH_EXP_START) );
        REG(REG_FIQ_EXP_CLEAR_H) = (0x1 << (eIntNum-E_FIQH_EXP_START) );
        REG(REG_FIQ_EXP_CLEAR_H) = 0;
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




// Dean modify to match kernel 2.6.26
static struct hw_interrupt_type titania_irq_type = {
	.name = "Titania IRQ",
	.startup = startup_titania_irq,
	.shutdown = shutdown_titania_irq,
	.enable = (void (*)(unsigned int irq))enable_titania_irq,
	.disable = disable_titania_irq,
	.ack = mask_and_ack_titania_irq,
	.mask = disable_titania_irq,
	.mask_ack = disable_titania_irq,
	.unmask = (void (*)(unsigned int irq))enable_titania_irq,
	.eoi = (void (*)(unsigned int irq))enable_titania_irq,
	.end = (void (*)(unsigned int irq))end_titania_irq
};
static struct hw_interrupt_type titania_fiq_type = {
	.name = "Titania IRQ",
	.startup = startup_titania_fiq,
	.shutdown = shutdown_titania_fiq,
	.enable = (void (*)(unsigned int irq))enable_titania_fiq,
	.disable = disable_titania_fiq,
	.ack = mask_and_ack_titania_fiq,
	.mask = disable_titania_fiq,
	.mask_ack = disable_titania_fiq,
	.unmask = (void (*)(unsigned int irq))enable_titania_fiq,
	.eoi = (void (*)(unsigned int irq))enable_titania_fiq,
	.end = (void (*)(unsigned int irq))end_titania_fiq
};


// void Titania_hw0_irqdispatch(struct pt_regs *regs)
void Titania_hw0_irqdispatch(void)
{
	unsigned short u16Reglow,u16Reghigh;

	u16Reglow = (unsigned short)REG(REG_IRQ_PENDING_L);
    //u16Reglow += MSTAR_INT_BASE;
    DO_IRQL(u16Reglow,     E_IRQ_UART            );
    DO_IRQL(u16Reglow,     E_IRQ_MVD             );
    DO_IRQL(u16Reglow,     E_IRQ_EMAC            );
    DO_IRQL(u16Reglow,     E_IRQ_UART0           );
    DO_IRQL(u16Reglow,     E_IRQ_UART1           );
    DO_IRQL(u16Reglow,     E_IRQ_IIC1            );
    DO_IRQL(u16Reglow,     E_IRQ_UHC             );
    //DO_IRQL(u16Reglow,     E_IRQ_SECOND_UHC      );???????
    DO_IRQL(u16Reglow,     E_IRQ_SPI_ARB_DUP_REG );
    DO_IRQL(u16Reglow,     E_IRQ_SPI_ARB_CHG_ADDR);
    DO_IRQL(u16Reglow,     E_IRQ_DEB             );
    DO_IRQL(u16Reglow,     E_IRQ_UART2           );
    DO_IRQL(u16Reglow,     E_IRQ_MICOM           );
    DO_IRQL(u16Reglow,     E_IRQ_SBM             );
    DO_IRQL(u16Reglow,     E_IRQ_COMB            );
    DO_IRQL(u16Reglow,     E_IRQ_ECC_DERR        );

	u16Reghigh = (unsigned short)REG(REG_IRQ_PENDING_H);
    DO_IRQH(u16Reghigh,    E_IRQH_START          );
    DO_IRQH(u16Reghigh,    E_IRQ_TSP             );
    DO_IRQH(u16Reghigh,    E_IRQ_VE              );
    DO_IRQH(u16Reghigh,    E_IRQ_CIMAX2MCU       );
    DO_IRQH(u16Reghigh,    E_IRQ_DC              );
    DO_IRQH(u16Reghigh,    E_IRQ_GOP             );
    DO_IRQH(u16Reghigh,    E_IRQ_PCM2MCU         );
    DO_IRQH(u16Reghigh,    E_IRQ_IIC0            );
    DO_IRQH(u16Reghigh,    E_IRQ_RTC             );
    DO_IRQH(u16Reghigh,    E_IRQ_KEYPAD          );
    DO_IRQH(u16Reghigh,    E_IRQ_PM              );
    DO_IRQH(u16Reghigh,    E_IRQ_D2B             );
    DO_IRQH(u16Reghigh,    E_IRQ_VBI             );
    DO_IRQH(u16Reghigh,    E_IRQ_M4V             );
    DO_IRQH(u16Reghigh,    E_IRQ_MLINK           );
    DO_IRQH(u16Reghigh,    E_IRQ_HDMITX          );

    u16Reglow = (unsigned short)REG(REG_IRQ_EXP_PENDING_L);
    DO_IRQLEXP(u16Reglow,  E_IRQEXP_SVD          );
    DO_IRQLEXP(u16Reglow,  E_IRQEXP_USB2         );
    DO_IRQLEXP(u16Reglow,  E_IRQEXP_UHC2         );
    DO_IRQLEXP(u16Reglow,  E_IRQEXP_MIU          );

    u16Reghigh = (unsigned short)REG(REG_IRQ_EXP_PENDING_H);
}


// void Titania_hw0_fiqdispatch(struct pt_regs *regs)
void Titania_hw0_fiqdispatch(void)
{
    unsigned short u16Reg;

    u16Reg = REG(REG_FIQ_PENDING_L);
    //u16Reg += MSTAR_INT_BASE;
    DO_FIQL(u16Reg,        E_FIQ_EXTIMER0         );
    DO_FIQL(u16Reg,        E_FIQ_EXTIMER1         );
    DO_FIQL(u16Reg,        E_FIQ_WDT              );
    DO_FIQL(u16Reg,        E_FIQ_INT_AEON_TO_8051 );
    DO_FIQL(u16Reg,        E_FIQ_INT_8051_TO_AEON );
    DO_FIQL(u16Reg,        E_FIQ_INT_8051_TO_MIPS );
    DO_FIQL(u16Reg,        E_FIQ_INT_MIPS_TO_8051 );
    DO_FIQL(u16Reg,        E_FIQ_INT_MIPS_TO_AEON );
    DO_FIQL(u16Reg,        E_FIQ_INT_AEON_TO_MIPS );
    DO_FIQL(u16Reg,        E_FIQ_JPD              );
    DO_FIQL(u16Reg,        E_FIQ_MENULOAD         );
    DO_FIQL(u16Reg,        E_FIQ_HDMI_NON_PCM     );
    DO_FIQL(u16Reg,        E_FIQ_SPDIF_IN_NON_PCM );
    DO_FIQL(u16Reg,        E_FIQ_EMAC             );
    DO_FIQL(u16Reg,        E_FIQ_SE_DSP2UP        );
    DO_FIQL(u16Reg,        E_FIQ_TSP2AEON         );

    u16Reg = REG(REG_FIQ_PENDING_H);
    DO_FIQH(u16Reg,        E_FIQ_VIVALDI_STR      );
    DO_FIQH(u16Reg,        E_FIQ_VIVALDI_PTS      );
    DO_FIQH(u16Reg,        E_FIQ_DSP_MIU_PROT     );
    DO_FIQH(u16Reg,        E_FIQ_XIU_TIMEOUT      );
    DO_FIQH(u16Reg,        E_FIQ_DMA_DONE         );
    DO_FIQH(u16Reg,        E_FIQ_VSYNC_VE4VBI     );
    DO_FIQH(u16Reg,        E_FIQ_FIELD_VE4VBI     );
    DO_FIQH(u16Reg,        E_FIQ_VDMCU2HK         );
    DO_FIQH(u16Reg,        E_FIQ_VE_DONE_TT       );
    DO_FIQH(u16Reg,        E_FIQ_INT_CCFL         );
    DO_FIQH(u16Reg,        E_FIQ_INT              );
    DO_FIQH(u16Reg,        E_FIQ_IR               );
    DO_FIQH(u16Reg,        E_FIQ_AFEC_VSYNC       );
    DO_FIQH(u16Reg,        E_FIQ_DEC_DSP2UP       );
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
              Titania_hw0_irqdispatch();
       else if (irq == 3)
              Titania_hw0_fiqdispatch();
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
	//set_except_vector(0, mipsIRQ);
	//mips_cpu_irq_init(0);
#if 0
	for (i = 0; i <= TITANIAINT_END; i++) {
		irq_desc[i].status	= IRQ_DISABLED;
		irq_desc[i].action	= 0;
		irq_desc[i].depth	= 1;
		if ( i <64)
                irq_desc[i].chip    = &titania_irq_type;
		   //irq_desc[i].handler	= &titania_irq_type;
		else
                irq_desc[i].chip    = &titania_fiq_type;
		   //irq_desc[i].handler  = &titania_fiq_type;
	}

#endif
       for (i = MSTAR_INT_BASE; i <= (TITANIAINT_END+MSTAR_INT_BASE); i++) {
            if ( i <64+MSTAR_INT_BASE)
                set_irq_chip_and_handler(i, &titania_irq_type, handle_level_irq);
            else
                set_irq_chip_and_handler(i, &titania_fiq_type, handle_level_irq);
       }
       set_irq_chip_and_handler(0, &titania_fiq_type, handle_level_irq);
       set_irq_chip_and_handler(1, &titania_fiq_type, handle_level_irq);
	//disable all
	Titania_DisableInterrupt(E_IRQ_FIQ_ALL);

       mips_cpu_irq_init();
}

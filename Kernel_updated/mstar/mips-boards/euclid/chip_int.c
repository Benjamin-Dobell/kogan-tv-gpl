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

//static raw_spinlock_t mips_irq_lock = RAW_SPIN_LOCK_UNLOCKED;


static int Euclid_EnableInterrupt (InterruptNum eIntNum)
{
    int bRet = 0;

    if (eIntNum == E_IRQ_FIQ_ALL)
    {
            REG(REG_IRQ_MASK_L) &= ~IRQL_MASK;
            REG(REG_IRQ_MASK_H) &= ~IRQH_MASK;
            REG(REG_FIQ_MASK_L) &= ~FIQL_MASK;
            REG(REG_FIQ_MASK_H) &= ~FIQH_MASK;

            REG(REG_IRQ_EXP_MASK_L) &= ~IRQEXPL_MASK;
            REG(REG_IRQ_EXP_MASK_H) &= ~IRQEXPH_MASK;
            REG(REG_FIQ_EXP_MASK_L) &= ~FIQEXPL_MASK;
            REG(REG_FIQ_EXP_MASK_H) &= ~FIQEXPH_MASK;
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
    else if ( (eIntNum >= E_IRQEXPL_START) && (eIntNum <= E_IRQEXPL_END) )
    {
            REG(REG_IRQ_EXP_MASK_L) &= ~(0x1 << (eIntNum-E_IRQEXPL_START) );
    }
    else if ( (eIntNum >= E_IRQEXPH_START) && (eIntNum <= E_IRQEXPH_END) )
    {
            REG(REG_IRQ_EXP_MASK_H) &= ~(0x1 << (eIntNum-E_IRQEXPH_START) );
    }
    else if ( (eIntNum >= E_FIQEXPL_START) && (eIntNum <= E_FIQEXPL_END) )
    {
            REG(REG_FIQ_EXP_MASK_L) &= ~(0x1 << (eIntNum-E_FIQEXPL_START) );
    }
    else if ( (eIntNum >= E_FIQEXPH_START) && (eIntNum <= E_FIQEXPH_END) )
    {
            REG(REG_FIQ_EXP_MASK_H) &= ~(0x1 << (eIntNum-E_FIQEXPH_START) );
    }
    return bRet;
}


static int Euclid_DisableInterrupt (InterruptNum eIntNum)
{

    if (eIntNum == E_IRQ_FIQ_ALL)
    {
        REG(REG_IRQ_MASK_L) |= IRQL_MASK;
        REG(REG_IRQ_MASK_H) |= IRQH_MASK;
        REG(REG_FIQ_MASK_L) |= FIQL_MASK;
        REG(REG_FIQ_MASK_H) |= FIQH_MASK;

        REG(REG_IRQ_EXP_MASK_L) |= IRQEXPL_MASK;
        REG(REG_IRQ_EXP_MASK_H) |= IRQEXPH_MASK;
        REG(REG_FIQ_EXP_MASK_L) |= FIQEXPL_MASK;
        REG(REG_FIQ_EXP_MASK_H) |= FIQEXPH_MASK;
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
    else if ( (eIntNum >= E_IRQEXPL_START) && (eIntNum <= E_IRQEXPL_END) )
    {
        REG(REG_IRQ_EXP_MASK_L) |= (0x1 << (eIntNum-E_IRQEXPL_START) );
    }
    else if ( (eIntNum >= E_IRQEXPH_START) && (eIntNum <= E_IRQEXPH_END) )
    {
        REG(REG_IRQ_EXP_MASK_H) |= (0x1 << (eIntNum-E_IRQEXPH_START) );
    }
    else if ( (eIntNum >= E_FIQEXPL_START) && (eIntNum <= E_FIQEXPL_END) )
    {
        REG(REG_FIQ_EXP_MASK_L) |= (0x1 << (eIntNum-E_FIQEXPL_START) );
        REG(REG_FIQ_EXP_CLEAR_L) = (0x1 << (eIntNum-E_FIQEXPL_START) );
        REG(REG_FIQ_EXP_CLEAR_L) = 0;
    }
    else if ( (eIntNum >= E_FIQEXPH_START) && (eIntNum <= E_FIQEXPH_END) )
    {
        REG(REG_FIQ_EXP_MASK_H) |= (0x1 << (eIntNum-E_FIQEXPH_START) );
        REG(REG_FIQ_EXP_CLEAR_H) = (0x1 << (eIntNum-E_FIQEXPH_START) );
        REG(REG_FIQ_EXP_CLEAR_H) = 0;
    }
    return 0;
}



static unsigned int startup_euclid_irq(unsigned int irq)
{
	//enable_lasat_irq(irq);
    Euclid_EnableInterrupt((InterruptNum)irq);
	return 0; /* never anything pending */
}

static void shutdown_euclid_irq(unsigned int irq)
{
	//enable_lasat_irq(irq);
    Euclid_DisableInterrupt((InterruptNum)irq);
	//return 0; /* never anything pending */
}

#define enable_euclid_irq startup_euclid_irq
#define disable_euclid_irq shutdown_euclid_irq
#define mask_and_ack_euclid_irq shutdown_euclid_irq
#define end_euclid_irq startup_euclid_irq

static unsigned int startup_euclid_fiq(unsigned int irq)
{
	//enable_lasat_irq(irq);
    Euclid_EnableInterrupt((InterruptNum)irq);
	return 0; /* never anything pending */
}

static void shutdown_euclid_fiq(unsigned int irq)
{
	//enable_lasat_irq(irq);
    Euclid_DisableInterrupt((InterruptNum)irq);
	//return 0; /* never anything pending */
}

#define enable_euclid_fiq startup_euclid_fiq
#define disable_euclid_fiq shutdown_euclid_fiq
#define mask_and_ack_euclid_fiq shutdown_euclid_fiq
#define end_euclid_fiq startup_euclid_fiq




// Dean modify to match kernel 2.6.26
static struct hw_interrupt_type euclid_irq_type = {
	.name = "Euclid IRQ",
	.startup = startup_euclid_irq,
	.shutdown = shutdown_euclid_irq,
	.enable = (void (*)(unsigned int irq))enable_euclid_irq,
	.disable = disable_euclid_irq,
	.ack = mask_and_ack_euclid_irq,
	.mask = disable_euclid_irq,
	.mask_ack = disable_euclid_irq,
	.unmask = (void (*)(unsigned int irq))enable_euclid_irq,
	.eoi = (void (*)(unsigned int irq))enable_euclid_irq,
	.end = (void (*)(unsigned int irq))end_euclid_irq
};
static struct hw_interrupt_type euclid_fiq_type = {
	.name = "Euclid IRQ",
	.startup = startup_euclid_fiq,
	.shutdown = shutdown_euclid_fiq,
	.enable = (void (*)(unsigned int irq))enable_euclid_fiq,
	.disable = disable_euclid_fiq,
	.ack = mask_and_ack_euclid_fiq,
	.mask = disable_euclid_fiq,
	.mask_ack = disable_euclid_fiq,
	.unmask = (void (*)(unsigned int irq))enable_euclid_fiq,
	.eoi = (void (*)(unsigned int irq))enable_euclid_fiq,
	.end = (void (*)(unsigned int irq))end_euclid_fiq
};


// void Euclid_hw0_irqdispatch(struct pt_regs *regs)
void Euclid_hw0_irqdispatch(void)
{
    unsigned short u16Reg = 0;

    //in interrupt context
    //IRQ Low 16 bits
    u16Reg = REG(REG_IRQ_PENDING_L);

    if (u16Reg & IRQL_UART0)
    {
		do_IRQ((unsigned int)E_IRQ_UART0);
    }
    if (u16Reg & IRQL_BDMA_CH0)
    {
		do_IRQ((unsigned int)E_IRQ_BDMA_CH0);
    }
    if (u16Reg & IRQL_BDMA_CH1)
    {
		do_IRQ((unsigned int)E_IRQ_BDMA_CH1);
    }
    if (u16Reg & IRQL_MVD)
    {
		do_IRQ((unsigned int)E_IRQ_MVD);
    }
    if (u16Reg & IRQL_PS)
    {
		do_IRQ((unsigned int)E_IRQ_PS);
    }
    if (u16Reg & IRQL_NFIE)
    {
		do_IRQ((unsigned int)E_IRQ_NFIE);
    }
    if (u16Reg & IRQL_USB)
    {
		do_IRQ((unsigned int)E_IRQ_USB);
    }
    if (u16Reg & IRQL_UHC)
    {
		do_IRQ((unsigned int)E_IRQ_UHC);
    }
    if (u16Reg & IRQL_EC_BRIDGE)
    {
		do_IRQ((unsigned int)E_IRQ_EC_BRIDGE);
    }
    if (u16Reg & IRQL_EMAC)
    {
		do_IRQ((unsigned int)E_IRQ_EMAC);
    }
    if (u16Reg & IRQL_DISP)
    {
		do_IRQ((unsigned int)E_IRQ_DISP);
    }
    if (u16Reg & IRQL_DHC)
    {
		do_IRQ((unsigned int)E_IRQ_DHC);
    }
/*
    if (u16Reg & IRQL_PMSLEEP)
    {
		do_IRQ((unsigned int)E_IRQ_PMSLEEP);
    }
*/
    if (u16Reg & IRQL_SBM)
    {
		do_IRQ((unsigned int)E_IRQ_SBM);
    }
    if (u16Reg & IRQL_COMB)
    {
		do_IRQ((unsigned int)E_IRQ_COMB);
    }
    if (u16Reg & IRQL_ECC_DERR)
    {
		do_IRQ((unsigned int)E_IRQ_ECC_DERR);
    }

    u16Reg = REG(REG_IRQ_PENDING_H);
    if (u16Reg & IRQH_TSP2HK)
    {
		do_IRQ((unsigned int)E_IRQ_TSP2HK);
    }
    if (u16Reg & IRQH_VE)
    {
		do_IRQ((unsigned int)E_IRQ_VE);
    }
    if (u16Reg & IRQH_CIMAX2MCU)
    {
		do_IRQ((unsigned int)E_IRQ_CIMAX2MCU);
    }
    if (u16Reg & IRQH_DC)
    {
		do_IRQ((unsigned int)E_IRQ_DC);
    }
    if (u16Reg & IRQH_GOP)
    {
		do_IRQ((unsigned int)E_IRQ_GOP);
    }
    if (u16Reg & IRQH_PCM)
    {
		do_IRQ((unsigned int)E_IRQ_PCM);
    }
    if (u16Reg & IRQH_IIC0)
    {
		do_IRQ((unsigned int)E_IRQ_IIC0);
    }
    if (u16Reg & IRQH_RTC)
    {
		do_IRQ((unsigned int)E_IRQ_RTC);
    }
/*
    if (u16Reg & IRQH_KEYPAD)
    {
		do_IRQ((unsigned int)E_IRQ_KEYPAD);
    }
*/
    if (u16Reg & IRQH_PM)
    {
		do_IRQ((unsigned int)E_IRQ_PM);
    }
    if (u16Reg & IRQH_D2B)
    {
		do_IRQ((unsigned int)E_IRQ_DDC2BI);
    }
    if (u16Reg & IRQH_SCM)
    {
		do_IRQ((unsigned int)E_IRQ_SCM);
    }
    if (u16Reg & IRQH_VBI)
    {
		do_IRQ((unsigned int)E_IRQ_VBI);
    }
/*
    if (u16Reg & IRQH_M4VD)
    {
		do_IRQ((unsigned int)E_IRQ_M4VD);
    }
*/
/*
    if (u16Reg & IRQH_FCIE2RIU)
    {
		do_IRQ((unsigned int)E_IRQ_FCIE2RIU);
    }
*/
    if (u16Reg & IRQH_ADCVI2RIU)
    {
		do_IRQ((unsigned int)E_IRQ_ADCDVI2RIU);
    }

    u16Reg = REG(REG_IRQ_EXP_PENDING_L);
    if (u16Reg & IRQEXPL_HVD)
    {
		do_IRQ((unsigned int)E_IRQEXPL_HVD);
    }
    if (u16Reg & IRQEXPL_USB2)
    {
		do_IRQ((unsigned int)E_IRQEXPL_USB2);
    }
    if (u16Reg & IRQEXPL_UHC2)
    {
		do_IRQ((unsigned int)E_IRQEXPL_UHC2);
    }
    if (u16Reg & IRQEXPL_MIU)
    {
		do_IRQ((unsigned int)E_IRQEXPL_MIU);
    }
    if (u16Reg & IRQEXPL_GDMA)
    {
		do_IRQ((unsigned int)E_IRQEXPL_GDMA);
    }
    if (u16Reg & IRQEXPL_UART2)
    {
		do_IRQ((unsigned int)E_IRQEXPL_UART2);
    }
    if (u16Reg & IRQEXPL_UART1)
    {
		do_IRQ((unsigned int)E_IRQEXPL_UART1);
    }
    if (u16Reg & IRQEXPL_DEMOD)
    {
		do_IRQ((unsigned int)E_IRQEXPL_DEMOD);
    }
    if (u16Reg & IRQEXPL_MPIF)
    {
		do_IRQ((unsigned int)E_IRQEXPL_MPIF);
    }
    if (u16Reg & IRQEXPL_JPD)
    {
		do_IRQ((unsigned int)E_IRQEXPL_JPD);
    }
    if (u16Reg & IRQEXPL_AEON2HI)
    {
		do_IRQ((unsigned int)E_IRQEXPL_AEON2HI);
    }
    if (u16Reg & IRQEXPL_BDMA0)
    {
		do_IRQ((unsigned int)E_IRQEXPL_BDMA0);
    }
    if (u16Reg & IRQEXPL_BDMA1)
    {
		do_IRQ((unsigned int)E_IRQEXPL_BDMA1);
    }
    if (u16Reg & IRQEXPL_OTG)
    {
		do_IRQ((unsigned int)E_IRQEXPL_OTG);
    }
    if (u16Reg & IRQEXPL_MVD_CHECKSUM_FAIL)
    {
		do_IRQ((unsigned int)E_IRQEXPL_MVD_CHECKSUM_FAIL);
    }
    if (u16Reg & IRQEXPL_TSP_CHECKSUM_FAIL)
    {
		do_IRQ((unsigned int)E_IRQEXPL_TSP_CHECKSUM_FAIL);
    }

    u16Reg = REG(REG_IRQ_EXP_PENDING_H);
    if (u16Reg & IRQEXPH_CA_I3)
    {
		do_IRQ((unsigned int)E_IRQEXPH_CA_I3);
    }
    if (u16Reg & IRQEXPH_HDMI_LEVEL)
    {
		do_IRQ((unsigned int)E_IRQEXPH_HDMI_LEVEL);
    }
    if (u16Reg & IRQEXPH_MIPS_WADR_ERR)
    {
		do_IRQ((unsigned int)E_IRQEXPH_MIPS_WADR_ERR);
    }
    if (u16Reg & IRQEXPH_RASP)
    {
		do_IRQ((unsigned int)E_IRQEXPH_RASP);
    }
    if (u16Reg & IRQEXPH_CA_SVP)
    {
		do_IRQ((unsigned int)E_IRQEXPH_CA_SVP);
    }
    if (u16Reg & IRQEXPH_UART2MCU)
    {
		do_IRQ((unsigned int)E_IRQEXPH_UART2MCU);
    }
    if (u16Reg & IRQEXPH_URDMA2MCU)
    {
		do_IRQ((unsigned int)E_IRQEXPH_URDMA2MCU);
    }
    if (u16Reg & IRQEXPH_IIC1)
    {
		do_IRQ((unsigned int)E_IRQEXPH_IIC1);
    }
    if (u16Reg & IRQEXPH_HDCP)
    {
		do_IRQ((unsigned int)E_IRQEXPH_HDCP);
    }
    if (u16Reg & IRQEXPH_DMA_WADR_ERR)
    {
		do_IRQ((unsigned int)E_IRQEXPH_DMA_WADR_ERR);
    }
    if (u16Reg & IRQEXPH_UP_IRQ_UART_CA)
    {
		do_IRQ((unsigned int)E_IRQEXPH_UP_IRQ_UART_CA);
    }
    if (u16Reg & IRQEXPH_UP_IRQ_EMM_ECM)
    {
		do_IRQ((unsigned int)E_IRQEXPH_UP_IRQ_EMM_ECM);
    }
}


// void Euclid_hw0_fiqdispatch(struct pt_regs *regs)
void Euclid_hw0_fiqdispatch(void)
{
    unsigned short u16Reg;

    u16Reg = REG(REG_FIQ_PENDING_L);

    if(u16Reg & FIQL_AEON_TO_8051)
    {
		do_IRQ((unsigned int)E_FIQ_AEON_TO_8051);
    }
    if(u16Reg & FIQL_EXTIMER0)
    {
		do_IRQ((unsigned int)E_FIQ_EXTIMER0);
    }
    if(u16Reg & FIQL_EXTIMER1)
    {
		do_IRQ((unsigned int)E_FIQ_EXTIMER1);
    }
    if(u16Reg & FIQL_WDT)
    {
		do_IRQ((unsigned int)E_FIQ_WDT);
    }
    if(u16Reg & FIQL_AEON_TO_8051)
    {
		do_IRQ((unsigned int)E_FIQ_AEON_TO_8051);
    }
    if(u16Reg & FIQL_8051_TO_AEON)
    {
		do_IRQ((unsigned int)E_FIQ_8051_TO_AEON);
    }
    if(u16Reg & FIQL_8051_TO_BEON)
    {
		do_IRQ((unsigned int)E_FIQ_8051_TO_BEON);
    }
    if(u16Reg & FIQL_BEON_TO_8051)
    {
		do_IRQ((unsigned int)E_FIQ_BEON_TO_8051);
    }
    if(u16Reg & FIQL_BEON_TO_AEON)
    {
		do_IRQ((unsigned int)E_FIQ_BEON_TO_AEON);
    }
    if(u16Reg & FIQL_AEON_TO_BEON)
    {
		do_IRQ((unsigned int)E_FIQ_AEON_TO_BEON);
    }
/*
    if(u16Reg & FIQL_JPD)
    {
		do_IRQ((unsigned int)E_FIQ_JPD);
    }
*/
    if(u16Reg & FIQL_FIQ_MENULOAD)
    {
		do_IRQ((unsigned int)E_FIQ_MENULOAD);
    }
    if(u16Reg & FIQL_HDMI_NON_PCM)
    {
		do_IRQ((unsigned int)E_FIQ_HDMI_NON_PCM);
    }
    if(u16Reg & FIQL_SPDIF_IN_NON_PCM)
    {
		do_IRQ((unsigned int)E_FIQ_SPDIF_IN_NON_PCM);
    }
    if(u16Reg & FIQL_EMAC)
    {
		do_IRQ((unsigned int)E_FIQ_EMAC);
    }
    if(u16Reg & FIQL_SE_DSP2UP)
    {
		do_IRQ((unsigned int)E_FIQ_SE_DSP2UP);
    }
    if(u16Reg & FIQL_TSP2AEON)
    {
		do_IRQ((unsigned int)E_FIQ_TSP2AEON);
    }

    // REG_FIQ_MASK_H
    //FIQ High 16 bits
    u16Reg = REG(REG_FIQ_PENDING_H);
    if(u16Reg & FIQH_VIVALDI_STR)
    {
		do_IRQ((unsigned int)E_FIQ_VIVALDI_STR);
    }
    if(u16Reg & FIQH_DSP_MIU_PROT)
    {
		do_IRQ((unsigned int)E_FIQ_DSP_MIU_PROT);
    }
    if(u16Reg & FIQH_XIU_TIMEOUT)
    {
		do_IRQ((unsigned int)E_FIQ_XIU_TIMEOUT);
    }
    if(u16Reg & FIQH_DMA_DONE)
    {
		do_IRQ((unsigned int)E_FIQ_DMA_DONE);
    }
    if(u16Reg & FIQH_VSYNC_VE4VBI)
    {
		do_IRQ((unsigned int)E_FIQ_VSYNC_VE4VBI);
    }
    if(u16Reg & FIQH_FIELD_VE4VBI)
    {
		do_IRQ((unsigned int)E_FIQ_FIELD_VE4VBI);
    }
    if(u16Reg & FIQH_VDMCU2HK)
    {
		do_IRQ((unsigned int)E_FIQ_VDMCU2HK);
    }
    if(u16Reg & FIQH_VE_DONE_TT)
    {
		do_IRQ((unsigned int)E_FIQ_VE_DONE_TT);
    }
    if(u16Reg & FIQH_INT_CCFL)
    {
		do_IRQ((unsigned int)E_FIQ_INT_CCFL);
    }
    if(u16Reg & FIQH_INT)
    {
		do_IRQ((unsigned int)E_FIQ_INT);
    }
    if(u16Reg & FIQH_IR)
    {
		do_IRQ((unsigned int)E_FIQ_IR);
    }
    if(u16Reg & FIQH_AFEC_VSYNC)
    {
		do_IRQ((unsigned int)E_FIQ_AFEC_VSYNC);
    }
    if(u16Reg & FIQH_DSP2UP)
    {
		do_IRQ((unsigned int)E_FIQ_DEC_DSP2UP);
    }

    u16Reg = REG(REG_FIQ_EXP_PENDING_L);
    if (u16Reg & FIQEXPL_IR_INT_RC)
    {
		do_IRQ((unsigned int)E_FIQEXPL_IR_INT_RC);
    }
    if (u16Reg & FIQEXPL_HDMITX_IRQ_EDGE)
    {
		do_IRQ((unsigned int)E_FIQEXPL_HDMITX_IRQ_EDGE);
    }
    if (u16Reg & FIQEXPL_UP_IRQ_UART_CA)
    {
		do_IRQ((unsigned int)E_FIQEXPL_UP_IRQ_UART_CA);
    }
    if (u16Reg & FIQEXPL_UP_IRQ_EMM_ECM)
    {
		do_IRQ((unsigned int)E_FIQEXPL_UP_IRQ_EMM_ECM);
    }
    if (u16Reg & FIQEXPL_PVR2MI_INT0)
    {
		do_IRQ((unsigned int)E_FIQEXPL_PVR2MI_INT0);
    }
    if (u16Reg & FIQEXPL_PVR2MI_INT1)
    {
		do_IRQ((unsigned int)E_FIQEXPL_PVR2MI_INT1);
    }
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
              Euclid_hw0_irqdispatch();
       else if (irq == 3)
              Euclid_hw0_fiqdispatch();
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
	for (i = 0; i <= EUCLIDINT_END; i++) {
		irq_desc[i].status	= IRQ_DISABLED;
		irq_desc[i].action	= 0;
		irq_desc[i].depth	= 1;
		if ( i <64)
                irq_desc[i].chip    = &euclid_irq_type;
		   //irq_desc[i].handler	= &euclid_irq_type;
		else
                irq_desc[i].chip    = &euclid_fiq_type;
		   //irq_desc[i].handler  = &euclid_fiq_type;
	}

#endif
       for (i = MSTAR_INT_BASE; i <= (EUCLIDINT_END+MSTAR_INT_BASE); i++) {
            if ( i <64+MSTAR_INT_BASE)
                set_irq_chip_and_handler(i, &euclid_irq_type, handle_level_irq);
            else
                set_irq_chip_and_handler(i, &euclid_fiq_type, handle_level_irq);
       }
       set_irq_chip_and_handler(0, &euclid_fiq_type, handle_level_irq);
       set_irq_chip_and_handler(1, &euclid_fiq_type, handle_level_irq);
	//disable all
	Euclid_DisableInterrupt(E_IRQ_FIQ_ALL);

       mips_cpu_irq_init();
}

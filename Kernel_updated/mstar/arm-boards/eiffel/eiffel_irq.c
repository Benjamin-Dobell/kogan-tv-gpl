#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/types.h>

#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <mach/io.h>
#include <asm/mach-types.h>
#include <asm/setup.h>

#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <asm/hardware/gic.h>

#include "chip_int.h"

#define __MMIO_P2V(x)	            (((x) & 0xfffff) | (((x) & 0x0f000000) >> 4) | 0xf8000000)
#define MMIO_P2V(x)	                ((void __iomem *)__MMIO_P2V(x))
#define CT_CA9X4_MPIC		        (0x1e000000)

/* Clear FIQ (Clear is not supported for IRQ) */
void eiffel_irq_ack(unsigned int irq)
{
    unsigned short  tmp;
    
    if(irq < 16)
	{
        tmp = (0x01 << irq);
        reg_writew(tmp, REG_INT_BASE_PA + (0x2c << 2));
	}
	else if((irq >= 16) && (irq < 32))
	{
        tmp = (0x01 << (irq - 16));
        reg_writew(tmp, REG_INT_BASE_PA + (0x2d << 2));
	}
	else if( (irq >= 32) && (irq < 48))
	{
        tmp = (0x01) << (irq - 32);
        reg_writew(tmp, REG_INT_BASE_PA + (0x2e << 2));
	}
	else if( (irq >= 48) && (irq < 64))
	{
        tmp = (0x01) << (irq - 48);
        reg_writew(tmp, REG_INT_BASE_PA + (0x2f << 2));
    }
#if 0
	else if( (irq >= 64) && (irq < 80))
	{
        tmp = (0x01 << (irq - 64));
        reg_writew(tmp, REG_INT_BASE_PA + (0x3c << 2));
        }
	else if((irq >= 80) && (irq < 96))
	{
        tmp = (0x01 << (irq - 81));
        reg_writew(tmp, REG_INT_BASE_PA + (0x3d << 2));
	}
	else if( (irq >= 96) && (irq < 112))
	{
        tmp = (0x01 << (irq - 97));
        reg_writew(tmp, REG_INT_BASE_PA + (0x3e << 2));
	}
	else if( (irq >= 112) && (irq < 128))
	{
        tmp = (0x01 << (irq - 112));
        reg_writew(tmp, REG_INT_BASE_PA + (0x3f << 2));
	}
#endif
}

/* Mask IRQ/FIQ */
void eiffel_irq_mask(unsigned int irq)
{
    unsigned short  tmp;
    int             cpu = smp_processor_id();

    /* temp solution to prevent core 1 masked irq but not unmask*/
    if (cpu !=0)
    {
        if(irq == E_IRQ_USB ||      \
           irq == E_IRQ_UHC ||      \
           irq == E_IRQEXPL_USB1 || \
           irq == E_IRQEXPL_UHC1 || \
           irq == E_IRQEXPL_USB2 || \
           irq == E_IRQEXPL_UHC2    \
           )
        return;
    }

	if(irq < 16)
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x24 << 2));
        tmp |= (0x01) << irq;
        reg_writew(tmp, REG_INT_BASE_PA + (0x24 << 2));
	}
	else if((irq >= 16) && (irq < 32))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x25 << 2));
		tmp |= (0x01) << (irq - 16);
        reg_writew(tmp, REG_INT_BASE_PA + (0x25 << 2));
	}
	else if( (irq >= 32) && (irq < 48))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x26 << 2));
		tmp |= (0x01) << (irq - 32);
        reg_writew(tmp, REG_INT_BASE_PA + (0x26 << 2));
	}
	else if( (irq >= 48) && (irq < 64))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x27 << 2));
    	tmp |= (0x01) << (irq - 48);
        reg_writew(tmp, REG_INT_BASE_PA + (0x27 << 2));
	}
	else if((irq >= 64) && (irq < 80))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x34 << 2));
		tmp |= (0x01) << (irq - 64);
        reg_writew(tmp, REG_INT_BASE_PA + (0x34 << 2));
	}
	else if((irq >= 80) && (irq < 96))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x35 << 2));
		tmp |= (0x01) << (irq - 80);
        reg_writew(tmp, REG_INT_BASE_PA + (0x35 << 2));
	}
	else if( (irq >= 96) && (irq < 112))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x36 << 2));
		tmp |= (0x01) << (irq - 96);
        reg_writew(tmp, REG_INT_BASE_PA + (0x36 << 2));
	}
	else if( (irq >= 112) && (irq < 128))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x37 << 2));
    	tmp |= (0x01) << (irq - 112);
        reg_writew(tmp, REG_INT_BASE_PA + (0x37 << 2));
	}

}

/* Un-Mask IRQ/FIQ */
void eiffel_irq_unmask(unsigned int irq)
{
    unsigned short tmp;

    if(irq < 16)
    {
        tmp = reg_readw(REG_INT_BASE_PA + (0x24 << 2));
        tmp &= ~((0x01) << irq);
        reg_writew(tmp, REG_INT_BASE_PA + (0x24 << 2));
    }

    else if((irq >= 16) && (irq < 32))
    {
        tmp = reg_readw(REG_INT_BASE_PA + (0x25 << 2));
        tmp &= ~((0x01) << (irq - 16));
        reg_writew(tmp, REG_INT_BASE_PA + (0x25 << 2));
    }
    else if((irq >= 32) && (irq < 48))
    {
        tmp = reg_readw(REG_INT_BASE_PA + (0x26 << 2));
        tmp &= ~((0x01) << (irq - 32));
        reg_writew(tmp, REG_INT_BASE_PA + (0x26 << 2));
    }
    else if((irq >= 48) && (irq < 64))
    {
        tmp = reg_readw(REG_INT_BASE_PA + (0x27 << 2));
        tmp &= ~((0x01) << (irq - 48));
        reg_writew(tmp, REG_INT_BASE_PA + (0x27 << 2));
    }
    	else if((irq >= 64) && (irq < 80))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x34 << 2));
        tmp &= ~((0x01) << (irq - 64));
        reg_writew(tmp, REG_INT_BASE_PA + (0x34 << 2));
	}
	else if((irq >= 80) && (irq < 96))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x35 << 2));
        tmp &= ~((0x01) << (irq - 80));
        reg_writew(tmp, REG_INT_BASE_PA + (0x35 << 2));
	}
	else if( (irq >= 96) && (irq < 112))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x36 << 2));
        tmp &= ~((0x01) << (irq - 96));
        reg_writew(tmp, REG_INT_BASE_PA + (0x36 << 2));
	}
	else if( (irq >= 112) && (irq < 128))
	{
        tmp = reg_readw(REG_INT_BASE_PA + (0x37 << 2));
        tmp &= ~((0x01) << (irq - 112));
        reg_writew(tmp, REG_INT_BASE_PA + (0x37 << 2));
    }
}

void __iomem *gic_cpu_base_addr = (void __iomem *) (0xFC000000 + 0x0100);
void __iomem *gic_dist_base_addr = (void __iomem *) (0xFC000000 + 0x1000);

extern void gic_dist_init(unsigned int gic_nr, void __iomem *base, unsigned int irq_start);
extern void gic_cpu_init(unsigned int gic_nr, void __iomem *base);

void __init eiffel_init_irq(void)
{
    unsigned long temp;

    //GIC Interrupt Set Enable Register for MSTAR controller
    temp=PERI_R(GIC_DIST_SET_EANBLE);
    temp= temp | (0x1 << INT_PPI_IRQ );
    PERI_W(GIC_DIST_SET_EANBLE,temp);

    gic_dist_init(0, gic_dist_base_addr , 29);
    gic_cpu_init(0, gic_cpu_base_addr);
}

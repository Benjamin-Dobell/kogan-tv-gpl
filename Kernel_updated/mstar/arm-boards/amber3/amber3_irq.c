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
//eddy
#define __MMIO_P2V(x)	(((x) & 0xfffff) | (((x) & 0x0f000000) >> 4) | 0xf8000000)
#define MMIO_P2V(x)	((void __iomem *)__MMIO_P2V(x))
#define CT_CA9X4_MPIC		(0x1e000000)
//#define A9_MPCORE_GIC_CPU	(CT_CA9X4_MPIC + 0x0100)
//#define A9_MPCORE_GIC_CPU	(0xFC000000 + 0x0100)


//#if defined(CONFIG_ARCH_PIONEER)
//#define REG_INT_BASE_PA		0x1F242200
//#elif defined(CONFIG_ARCH_amber3)
//#define REG_INT_BASE_PA		amber3_BASE_REG_INTR_PA
//#else
//#error "Unknown Machine Type "
//#endif

/* Clear FIQ (Clear is not supported for IRQ) */
void amber3_irq_ack(unsigned int irq) {
    unsigned short tmp;
//printk(KERN_WARNING "amber3_irq_ack(irq=%d)\n",irq);
        if(irq <16)
	{
	  tmp = (0x01 << irq);
          reg_writew(tmp, REG_INT_BASE_PA + 0x2c*4);
	}
	else if((irq >= 16) && (irq < 32))
	{
	  tmp = (0x01 << (irq-16));
          reg_writew(tmp, REG_INT_BASE_PA + 0x2d*4);
	}
	else if( (irq >= 32) && (irq < 48))
	{
	  tmp = (0x01) << (irq - 32);
          reg_writew(tmp, REG_INT_BASE_PA + 0x2e*4);
	}
	else if( (irq >= 48) && (irq < 64))
	{
          tmp = (0x01) << (irq - 48);
          reg_writew(tmp, REG_INT_BASE_PA + 0x2f*4);
        }
	/*
	else if( (irq >= 64) && (irq < 80))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x3c*4);
    	tmp |= (0x01) << (irq - 64);
        reg_writew(tmp, REG_INT_BASE_PA + 0x3c*4);
        }
	else if((irq >= 80) && (irq < 96))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x3d*4);
	    tmp |= 0x01 << (irq - 81);
        reg_writew(tmp, REG_INT_BASE_PA + 0x3d*4);
	}
	else if( (irq >= 96) && (irq < 112))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x3e*4);
		tmp |= (0x01) << (irq - 97);
        reg_writew(tmp, REG_INT_BASE_PA + 0x3e*4);
	}
	else if( (irq >= 112) && (irq < 128))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x3f*4);
    	tmp |= (0x01) << (irq - 112);
        reg_writew(tmp, REG_INT_BASE_PA + 0x3f*4);
	}
	*/
}

/* Mask IRQ/FIQ */
void amber3_irq_mask(unsigned int irq) {
    
    unsigned short tmp;
    int cpu = smp_processor_id();

    /* temp solution to prevent core 1 masked irq but not unmask*/
    if (cpu !=0)
    {
	if( irq == E_IRQ_USB || irq == E_IRQ_UHC || irq == E_IRQEXPL_USB1 || irq == E_IRQEXPL_UHC1 || irq == E_IRQEXPL_USB2 || irq == E_IRQEXPL_UHC2 )
		    return;
    }
	
//printk(KERN_WARNING "amber3_irq_mask(irq=%d)\n",irq);
	if(irq <16)
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x24*4);
        tmp |= (0x01)<<irq;
        reg_writew(tmp, REG_INT_BASE_PA + 0x24*4);
	}
	else if((irq >= 16) && (irq < 32))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x25*4);
		tmp |= (0x01) << (irq - 16);
        reg_writew(tmp, REG_INT_BASE_PA + 0x25*4);
	}
	else if( (irq >= 32) && (irq < 48))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x26*4);
		tmp |= (0x01) << (irq - 32);
        reg_writew(tmp, REG_INT_BASE_PA + 0x26*4);
	}
	else if( (irq >= 48) && (irq < 64))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x27*4);
		tmp |= (0x01) << (irq - 48);
        reg_writew(tmp, REG_INT_BASE_PA + 0x27*4);
	}
	else if((irq >= 64) && (irq < 80))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x34*4);
		tmp |= (0x01) << (irq - 64);
        reg_writew(tmp, REG_INT_BASE_PA + 0x34*4);
	}
	else if((irq >= 80) && (irq < 96))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x35*4);
		tmp |= (0x01) << (irq - 80);
        reg_writew(tmp, REG_INT_BASE_PA + 0x35*4);
	}
	else if( (irq >= 96) && (irq < 112))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x36*4);
		tmp |= (0x01) << (irq - 96);
        reg_writew(tmp, REG_INT_BASE_PA + 0x36*4);
	}
	else if( (irq >= 112) && (irq < 128))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x37*4);
    	tmp |= (0x01) << (irq - 112);
        reg_writew(tmp, REG_INT_BASE_PA + 0x37*4);
	}

}

/* Un-Mask IRQ/FIQ */
void amber3_irq_unmask(unsigned int irq) {
   unsigned short tmp;

    //printk(KERN_WARNING "amber3_irq_unmask(irq=%d)\n",irq);

    if(irq < 16)
    {
        tmp = reg_readw(REG_INT_BASE_PA + 0x24*4);
        tmp &= ~((0x01) << irq);
        reg_writew(tmp, REG_INT_BASE_PA + 0x24*4);
    }

    else if((irq >= 16) && (irq < 32))
    {
        tmp = reg_readw(REG_INT_BASE_PA + 0x25*4);
        tmp &= ~((0x01) << (irq -16));
        reg_writew(tmp, REG_INT_BASE_PA + 0x25*4);
    }
    else if((irq >= 32) && (irq < 48))
    {
        tmp = reg_readw(REG_INT_BASE_PA + 0x26*4);
        tmp &= ~((0x01) << (irq-32));
        reg_writew(tmp, REG_INT_BASE_PA + 0x26*4);
    }
    else if((irq >= 48) && (irq < 64))
    {
        tmp = reg_readw(REG_INT_BASE_PA + 0x27*4);
        tmp &= ~((0x01) << (irq -48));
        reg_writew(tmp, REG_INT_BASE_PA + 0x27*4);
    }
    	else if((irq >= 64) && (irq < 80))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x34*4);
        tmp &= ~((0x01) << (irq - 64));
        reg_writew(tmp, REG_INT_BASE_PA + 0x34*4);
	}
	else if((irq >= 80) && (irq < 96))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x35*4);
        tmp &= ~((0x01) << (irq - 80));
        reg_writew(tmp, REG_INT_BASE_PA + 0x35*4);
	}
	else if( (irq >= 96) && (irq < 112))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x36*4);
        tmp &= ~((0x01) << (irq - 96));
        reg_writew(tmp, REG_INT_BASE_PA + 0x36*4);
	}
	else if( (irq >= 112) && (irq < 128))
	{
        tmp = reg_readw(REG_INT_BASE_PA + 0x37*4);
        tmp &= ~((0x01) << (irq - 112));
        reg_writew(tmp, REG_INT_BASE_PA + 0x37*4);
    }
}

#if 0
static struct irq_chip amber3_irq_chip = {
	.name = "MSB25xx IRQ chip",
	.ack = amber3_irq_ack,
	.mask = amber3_irq_mask,
	.unmask = amber3_irq_unmask,
};
#endif

void __iomem *gic_cpu_base_addr=(void __iomem *)(0xFC000000 + 0x0100);
void __iomem *gic_dist_base_addr=(void __iomem *)(0xFC000000 + 0x1000);

extern void gic_dist_init(unsigned int gic_nr, void __iomem *base, unsigned int irq_start);
extern void gic_cpu_init(unsigned int gic_nr, void __iomem *base);

void __init amber3_init_irq(void)
{

    unsigned long temp;
    //eddy for fixing compile errors
    //gic_cpu_base_addr = MMIO_P2V(A9_MPCORE_GIC_CPU);

#if 1
    //GIC Interrupt Set Enable Register for MSTAR controller
    #ifdef CONFIG_SPI_IRQ
    temp=PERI_R(GIC_DIST_SET_EANBLE+(4*(INT_SPI_IRQ/32)));
    temp= temp | (0x1 << (INT_SPI_IRQ%32) );
    PERI_W(GIC_DIST_SET_EANBLE+(4*(INT_SPI_IRQ/32)),temp);
    #else
    temp=PERI_R(GIC_DIST_SET_EANBLE);
    temp= temp | (0x1 << INT_PPI_IRQ );
    PERI_W(GIC_DIST_SET_EANBLE,temp);
#endif

#endif

    gic_dist_init(0, gic_dist_base_addr , 29);
    gic_cpu_init(0, gic_cpu_base_addr);

        #if 0
	for(irq=0;irq<NR_IRQS;irq++){
		set_irq_chip(irq, &amber3_irq_chip);
		//set_irq_chip(31, &amber3_irq_chip);

	//	if(irq<32)
	        //set_irq_handler(irq, handle_edge_irq);

		//if(irq>=32)
			set_irq_handler(irq, handle_level_irq);

        //set_irq_flags(irq, IRQF_VALID | IRQF_PROBE);
        set_irq_flags(irq, IRQF_VALID | IRQF_PROBE);
	}
        #endif

}

#ifdef CONFIG_SPI_IRQ
void chip_gic_set_target(void)
{
    int i;
    for(i=SPI_MIN_NUM;i<=SPI_MAX_NUM;i+=4)
        PERI_W(peri_v2p(gic_dist_base_addr + GIC_DIST_TARGET + i), 0x03030303);

}
#endif

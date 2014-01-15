#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/smp.h>
#include <linux/cpumask.h>
#include <linux/io.h>

#include <asm/cpu_pm.h>
#include <asm/mach/irq.h>
#include <asm/hardware/gic.h>

#include <asm/irq.h>
#include <linux/irq.h>
#include <mach/hardware.h>
#include "chip_int.h"
static DEFINE_SPINLOCK(irq_controller_lock);

#define REG(addr) (*(volatile unsigned int *)(addr))
#define VIRTUAL_FIQ_START 128
#define VIRTUAL_IRQ_START (VIRTUAL_FIQ_START + FIQ_NUMBER)
#define IRQ_NUMBER 64
#define FIQ_NUMBER 64


extern void chip_irq_ack(unsigned int irq);
extern void chip_irq_mask(unsigned int irq);
extern void chip_irq_unmask(unsigned int irq);

static unsigned int MAsm_CPU_GetTrailOne(unsigned int u32Flags)
{
    u32Flags = (~u32Flags) & (u32Flags-1);
    asm volatile
    (
        "clz %0, %0\n"
        : "=r" (u32Flags)
        : "0" (u32Flags)
    );
    return 32 - u32Flags;
}

static void arm_ack_irq(struct irq_data *d)
{

 chip_irq_ack( d->irq - VIRTUAL_FIQ_START);
 
}

static void arm_mask_irq(struct irq_data *d)
{
	//handle MSTAR IRQ controler 
	spin_lock(&irq_controller_lock);
	chip_irq_mask( d->irq - VIRTUAL_FIQ_START);
	spin_unlock(&irq_controller_lock);
}

static void arm_unmask_irq(struct irq_data *d)
{
	//handle MSTAR IRQ controler 
	spin_lock(&irq_controller_lock);
	chip_irq_unmask( d->irq - VIRTUAL_FIQ_START);
	spin_unlock(&irq_controller_lock);

}

static int arm_irq_type(struct irq_data *d, unsigned int type)
{

}

static int arm_set_affinity(struct irq_data *d, const struct cpumask *mask_val, bool force)
{

}

static struct irq_chip arm_irq_chip = {
        .name           = "MSTAR",
        .irq_ack        = arm_ack_irq,
        .irq_mask       = arm_mask_irq,
        .irq_unmask     = arm_unmask_irq,
        .irq_set_type   = arm_irq_type,
		.irq_set_affinity = arm_set_affinity,
};

void Mstar_Chip_hw0_irqdispatch(void)
{
    unsigned int        u16Reg;
    unsigned int        u32Bit;
    InterruptNum        eIntNum;

    u16Reg = REG(REG_IRQ_PENDING_L);
 
    while (32> (u32Bit = MAsm_CPU_GetTrailOne(u16Reg)))
    {
	
        eIntNum = (InterruptNum)(u32Bit+ E_IRQL_START);
        generic_handle_irq((unsigned int)eIntNum);
        u16Reg &= ~(0x1<< u32Bit);
    }

    u16Reg = REG(REG_IRQ_PENDING_H);
    while (32> (u32Bit = MAsm_CPU_GetTrailOne(u16Reg)))
    {
        eIntNum = (InterruptNum)(u32Bit+ E_IRQH_START);
        generic_handle_irq((unsigned int)eIntNum);
        u16Reg &= ~(0x1<< u32Bit);
    }
    u16Reg = REG(REG_IRQ_EXP_PENDING_L);
    while (32> (u32Bit = MAsm_CPU_GetTrailOne(u16Reg)))
    {
        eIntNum = (InterruptNum)(u32Bit+ E_IRQEXPL_START);
        generic_handle_irq((unsigned int)eIntNum);
        u16Reg &= ~(0x1<< u32Bit);
    }
    u16Reg = REG(REG_IRQ_EXP_PENDING_H);
    while (32> (u32Bit = MAsm_CPU_GetTrailOne(u16Reg)))
    {
        eIntNum = (InterruptNum)(u32Bit+ E_IRQEXPH_START);
        generic_handle_irq((unsigned int)eIntNum);
        u16Reg &= ~(0x1<< u32Bit);
    }

}

void Mstar_Chip_hw0_fiqdispatch(void)
{
    unsigned int        u16Reg;
    unsigned int        u32Bit;
    InterruptNum        eIntNum;

    u16Reg = REG(REG_FIQ_PENDING_L);
    while (32> (u32Bit = MAsm_CPU_GetTrailOne(u16Reg)))
    {
        eIntNum = (InterruptNum)(u32Bit+ E_FIQL_START);
        generic_handle_irq((unsigned int)eIntNum);
        u16Reg &= ~(0x1<< u32Bit);
    }

    u16Reg = REG(REG_FIQ_PENDING_H);
    while (32> (u32Bit = MAsm_CPU_GetTrailOne(u16Reg)))
    {
        eIntNum = (InterruptNum)(u32Bit+ E_FIQH_START);
        generic_handle_irq((unsigned int)eIntNum);
        u16Reg &= ~(0x1<< u32Bit);
    }
    u16Reg = REG(REG_FIQ_EXP_PENDING_L);

    while (32> (u32Bit = MAsm_CPU_GetTrailOne(u16Reg)))
    {
        eIntNum = (InterruptNum)(u32Bit+ E_FIQEXPL_START);
        generic_handle_irq((unsigned int)eIntNum);
        u16Reg &= ~(0x1<< u32Bit);
    }
    u16Reg = REG(REG_FIQ_EXP_PENDING_H);
    while (32> (u32Bit = MAsm_CPU_GetTrailOne(u16Reg)))
    {
        eIntNum = (InterruptNum)(u32Bit+ E_FIQEXPH_START);
        generic_handle_irq((unsigned int)eIntNum);
        u16Reg &= ~(0x1<< u32Bit);
    }

}

static void arm_irq_handler(unsigned int irq, struct irq_desc *desc)
{
 struct irq_chip *chip = irq_desc_get_chip(desc);

chained_irq_enter(chip, desc);
Mstar_Chip_hw0_fiqdispatch();
Mstar_Chip_hw0_irqdispatch();
chained_irq_exit(chip, desc);
}

void arm_interrupt_chain_setup(int chain_num)
{ 
    int i=0,j=0;

	for (i = VIRTUAL_IRQ_START;i < VIRTUAL_IRQ_START + IRQ_NUMBER; i++) 
	{
		irq_set_chip(i, &arm_irq_chip);
		irq_set_handler(i, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);
	}
	
	for (j = VIRTUAL_FIQ_START;j < VIRTUAL_FIQ_START + FIQ_NUMBER; j++) 
	{
		irq_set_chip(j, &arm_irq_chip);
		irq_set_handler(j, handle_level_irq);
		set_irq_flags(j, IRQF_VALID);
	}
	 irq_set_chained_handler(chain_num, arm_irq_handler);
}

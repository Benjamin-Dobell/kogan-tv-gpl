/*
 *  linux/arch/arm/kernel/smp_scu.c
 *
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/io.h>

#include <asm/smp_scu.h>
#include <asm/cacheflush.h>
#include <asm/cputype.h>

#define SCU_CTRL		0x00
#define SCU_CONFIG		0x04
#define SCU_CPU_STATUS		0x08
#define SCU_INVALIDATE		0x0c
#define SCU_FPGA_REVISION	0x10

#ifdef CONFIG_SMP
/*
 * Get the number of CPU cores from the SCU configuration
 */
unsigned int __init scu_get_core_count(void __iomem *scu_base)
{
	unsigned int ncores = __raw_readl(scu_base + SCU_CONFIG);
	return (ncores & 0x03) + 1;
}

/*
 * Enable the SCU
 */
void __init scu_enable(void __iomem *scu_base)
{
	u32 scu_ctrl;

#ifdef CONFIG_ARM_ERRATA_764369
	/* Cortex-A9 only */
	if ((read_cpuid(CPUID_ID) & 0xff0ffff0) == 0x410fc090) {
		scu_ctrl = __raw_readl(scu_base + 0x30);
		if (!(scu_ctrl & 1))
			__raw_writel(scu_ctrl | 0x1, scu_base + 0x30);
	}
#endif

#if defined(CONFIG_MP_PLATFORM_SW_PATCH_L2_SRAM_RMA)
#if defined(CONFIG_MSTAR_Master0_Only)
        __raw_writel(0xe0000000, scu_base + 0x40);
        __raw_writel(0xe0100000, scu_base + 0x44);
#endif

#if defined(CONFIG_MSTAR_Master1_Only)
        __raw_writel(0x00000000, scu_base + 0x40);
        __raw_writel(0xe0000000, scu_base + 0x44);
#endif

#if defined(CONFIG_MSTAR_Master0_Only) ||  defined(CONFIG_MSTAR_Master1_Only)
        scu_ctrl = __raw_readl(scu_base + SCU_CTRL);
        scu_ctrl |= 0x02;
        __raw_writel(scu_ctrl, scu_base + SCU_CTRL);

#endif
#endif /* CONFIG_MP_PLATFORM_SW_PATCH_L2_SRAM_RMA */

	scu_ctrl = __raw_readl(scu_base + SCU_CTRL);
	/* already enabled? */
	if (scu_ctrl & 1)
		return;

	scu_ctrl |= 1;
	__raw_writel(scu_ctrl, scu_base + SCU_CTRL);

	/*
	 * Ensure that the data accessed by CPU0 before the SCU was
	 * initialised is visible to the other CPUs.
	 */
	flush_cache_all();
}
#endif

/*
 * Set the executing CPUs power mode as defined.  This will be in
 * preparation for it executing a WFI instruction.
 *
 * This function must be called with preemption disabled, and as it
 * has the side effect of disabling coherency, caches must have been
 * flushed.  Interrupts must also have been disabled.
 */
int scu_power_mode(void __iomem *scu_base, unsigned int mode)
{
	unsigned int val;
	int cpu = smp_processor_id();

	if (mode > 3 || mode == 1 || cpu > 3)
		return -EINVAL;

	val = __raw_readb(scu_base + SCU_CPU_STATUS + cpu) & ~0x03;
	val |= mode;
	__raw_writeb(val, scu_base + SCU_CPU_STATUS + cpu);

	return 0;
}

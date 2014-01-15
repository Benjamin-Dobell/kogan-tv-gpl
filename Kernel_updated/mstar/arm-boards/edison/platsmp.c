/*
 *  linux/arch/arm/mach-vexpress/platsmp.c
 *
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/smp.h>
#include <linux/io.h>

#include <asm/smp_scu.h>
#include <asm/hardware/gic.h>
#include <asm/cacheflush.h>
#include <asm/unified.h>

//#define SECOND_START_ADDR_HI	0xFD206740
//#define SECOND_START_ADDR_LO	0xFD206744
#define SECOND_MAGIC_NUMBER_ADRESS	0xC0008000
#define SECOND_START_ADDR	0xC0008004

extern void vexpress_secondary_startup(void);

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
void __init smp_init_cpus(void)
{
	unsigned int i, ncores;
	ncores = scu_get_core_count((void __iomem *)(PERI_ADDRESS(0x16000000)));

	/* sanity check */
	if (ncores == 0) {
		printk(KERN_ERR
		       "vexpress: strange CM count of 0? Default to 1\n");

		ncores = 1;
	}

	if (ncores > NR_CPUS) {
		printk(KERN_WARNING
		       "vexpress: no. of cores (%d) greater than configured "
		       "maximum of %d - clipping\n",
		       ncores, NR_CPUS);
		ncores = NR_CPUS;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);

	set_smp_cross_call(gic_raise_softirq);
}

void __init platform_smp_prepare_cpus(unsigned int max_cpus)
{
	int i;

	/*
	 * Initialise the present map, which describes the set of CPUs
	 * actually populated at the present time.
	 */
	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);

	scu_enable((void __iomem *)(PERI_ADDRESS(0x16000000))); // SCU PA = 0x16000000

	/*
	 * Write the address of secondary startup into the
	 * system-wide flags register. The boot monitor waits
	 * until it receives a soft interrupt, and then the
	 * secondary CPU branches to this address.
	 */

        //printk("_Secondary_startup physical address = 0x%08x\n",BSYM(virt_to_phys(vexpress_secondary_startup)));
	//writew((BSYM(virt_to_phys(vexpress_secondary_startup))>>16), SECOND_START_ADDR_HI);
	//writew(BSYM(virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR_LO);
    writel(0xbabe, SECOND_MAGIC_NUMBER_ADRESS);
	writel(BSYM(virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR);
	__cpuc_flush_kern_all();
}
#if defined(CONFIG_MP_MSTAR_STR_BASE)
int __init platform_smp_boot_secondary_init(unsigned int cpu)
{
    scu_enable((void __iomem *)(PERI_ADDRESS(0x16000000))); // SCU PA = 0x16000000
    writel(0xbabe, SECOND_MAGIC_NUMBER_ADRESS);
	writel(BSYM(virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR);
	__cpuc_flush_kern_all();
    return 0;
}
#endif

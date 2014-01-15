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
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <asm/localtimer.h>
#include <asm/unified.h>
#include <asm/smp_scu.h>


//#define SECOND_START_ADDR_HI	0xFD206740
//#define SECOND_START_ADDR_LO	0xFD206744

#define SECOND_MAGIC_NUMBER_ADRESS	0xC0008000
#define SECOND_START_ADDR	0xC0008004

extern void vexpress_secondary_startup(void);

/*
 * control for which core is the next to come out of the secondary
 * boot "holding pen"
 */
volatile int __cpuinitdata pen_release = -1;
extern void __iomem *gic_cpu_base_addr;
static DEFINE_SPINLOCK(boot_lock);

void __cpuinit platform_secondary_init(unsigned int cpu)
{
	trace_hardirqs_off();

	/*
	 * if any interrupts are already enabled for the primary
	 * core (e.g. timer irq), then they will not have been enabled
	 * for us: do so
	 */
	gic_cpu_init(0, gic_cpu_base_addr);
	
	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	pen_release = -1;
	smp_wmb();

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

int __cpuinit boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
	 * Set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	/*
	 * This is really belt and braces; we hold unintended secondary
	 * CPUs in the holding pen until we're ready for them.  However,
	 * since we haven't sent them a soft interrupt, they shouldn't
	 * be there.
	 */
	//writew((BSYM(virt_to_phys(vexpress_secondary_startup))>>16), SECOND_START_ADDR_HI);
	//writew(BSYM(virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR_LO);
	writel(0xbabe, SECOND_MAGIC_NUMBER_ADRESS);
	writel(BSYM(virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR);
	__cpuc_flush_kern_all();
	pen_release = cpu;

	#if 0  //debug
	unsigned int *ptr=&pen_release;
	printk("pen_release = 0x%08x, addr= 0x%08x, pen_release ptr = 0x%08x\n ",pen_release,&pen_release,*ptr);
	#endif
	
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));

	/*
	 * Send the secondary CPU a soft interrupt, thereby causing
	 * the boot monitor to read the system wide flags register,
	 * and branch to the address found there.
	 */
	smp_cross_call(cpumask_of(cpu));

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1)
			break;

		udelay(10);
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);
	return pen_release != -1 ? -ENOSYS : 0;
}

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
}

void __init smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int ncores = num_possible_cpus();
	unsigned int cpu = smp_processor_id();
	int i;

	smp_store_cpu_info(cpu);

	/*
	 * are we trying to boot more cores than exist?
	 */
	if (max_cpus > ncores)
		max_cpus = ncores;

	/*
	 * Initialise the present map, which describes the set of CPUs
	 * actually populated at the present time.
	 */
	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);

	/*
	 * Initialise the SCU if there are more than one CPU and let
	 * them know where to start.
	 */
	if (max_cpus > 1) {
		/*
		 * Enable the local timer or broadcast device for the
		 * boot CPU, but only if we have more than one CPU.
		 */
		percpu_timer_setup();
	
		scu_enable((void __iomem *)(PERI_ADDRESS(0x16000000))); // SCU PA = 0x16000000 

		/*
		 * Write the address of secondary startup into the
		 * system-wide flags register. The boot monitor waits
		 * until it receives a soft interrupt, and then the
		 * secondary CPU branches to this address.
		 */
		
		//printk("_Secondary_startup physical address = 0x%08x\n",BSYM(virt_to_phys(vexpress_secondary_startup)));
		//writew((BSYM(virt_to_phys(vexpress_secondary_startup))>>16), SECOND_START_ADDR_HI);
		writel(0xbabe, SECOND_MAGIC_NUMBER_ADRESS);
		writel(BSYM(virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR);
		__cpuc_flush_kern_all();
		
	}
}

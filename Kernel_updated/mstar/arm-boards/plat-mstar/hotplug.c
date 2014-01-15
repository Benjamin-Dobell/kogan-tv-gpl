/*
 *  linux/arch/arm/plat-versatile/hotplug.c
 *
 *  Copyright (C) 2010 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/smp.h>
#include <linux/completion.h>
#include <asm/cacheflush.h>

#include <plat/hotplug.h>
#include <asm/cputype.h>


static DECLARE_COMPLETION(cpu_killed);


static unsigned int get_smp_ctrl_mask(void)
{
    switch (read_cpuid_id() & 0xfff0)
    {
        case 0xb020: /* 11mpcore */
            return 0x20;
        case 0xc090: /* Cortex-A9 */
            return 0x40;
        default:
            return 0;
    }
}
extern void Chip_Flush_Cache_All(void);

static inline void cpu_enter_lowpower(void)
{
    unsigned int v, smp_ctrl = get_smp_ctrl_mask();

    flush_cache_all();
    //Chip_Flush_Cache_All();
    dsb();
    asm volatile(
    /*
     * Turn off coherency
     */
    "   mrc p15, 0, %0, c1, c0, 1\n"
    "   bic %0, %0, %1\n"
    "   mcr p15, 0, %0, c1, c0, 1\n"
    /* DSB */
    "       mcr     p15, 0, %2, c7, c10, 4\n"
    /* Disable D-cache */
    "   mrc p15, 0, %0, c1, c0, 0\n"
    "   bic %0, %0, #0x04\n"
    "   mcr p15, 0, %0, c1, c0, 0\n"
      : "=&r" (v)
      : "r" (smp_ctrl), "r" (0)
      : "memory");
    isb();
}

static inline void cpu_leave_lowpower(void)
{
    unsigned int v, smp_ctrl = get_smp_ctrl_mask();

    flush_cache_all();
    //Chip_Flush_Cache_All();
    dsb();
    asm volatile(   "mrc    p15, 0, %0, c1, c0, 0\n"
    "   orr %0, %0, #0x04\n"
    "   mcr p15, 0, %0, c1, c0, 0\n"
    "   mrc p15, 0, %0, c1, c0, 1\n"
    "   orr %0, %0, %1\n"
    "   mcr p15, 0, %0, c1, c0, 1\n"
      : "=&r" (v)
      : "r" (smp_ctrl)
      : "memory");
    isb();
}


int platform_cpu_kill(unsigned int cpu)
{
    return wait_for_completion_timeout(&cpu_killed, 5000);
}

/*
 * platform-specific code to shutdown a CPU
 *
 * Called with IRQs disabled
 */
void platform_cpu_die(unsigned int cpu)
{
#ifdef DEBUG
    unsigned int this_cpu = hard_smp_processor_id();

    if (cpu != this_cpu) {
        printk(KERN_CRIT "Eek! platform_cpu_die running on %u, should be %u\n",
               this_cpu, cpu);
        BUG();
    }
#endif

	printk(KERN_NOTICE "CPU%u: shutdown\n", cpu);
	complete(&cpu_killed);
	
	cpu_enter_lowpower();

	   /*
     * there is no power-control hardware on this platform, so all
     * we can do is put the core into WFI; this is safe as the calling
     * code will have already disabled interrupts
     */
    for (;;) {
        /*
         * here's the WFI
         */
        extern volatile int pen_release ;
        asm volatile("wfi" : : : "memory");

		//extern volatile int pen_release ;
        if (pen_release == cpu) {
            /*
             * OK, proper wakeup, we're done
             */
            break;
        }

        /*
         * getting here, means that we have come out of WFI without
         * having been woken up - this shouldn't happen
         *
         * The trouble is, letting people know about this is not really
         * possible, since we are currently running incoherently, and
         * therefore cannot safely call printk() or anything else
         */
#ifdef DEBUG
        printk("CPU%u: spurious wakeup call\n", cpu);
#endif
    }
    cpu_leave_lowpower();
}

int platform_cpu_disable(unsigned int cpu)
{
    /*
     * we don't allow CPU 0 to be shutdown (it is still too special
     * e.g. clock tick interrupts)
     */
    return cpu == 0 ? -EPERM : 0;
}

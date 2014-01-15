/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
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
 * Setting up the clock on the MIPS boards.
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/mc146818rtc.h>

#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/hardirq.h>
#include <asm/i8253.h>
#include <asm/irq.h>
#include <asm/div64.h>
#include <asm/cpu.h>
#include <asm/time.h>
#include <asm/mc146818-time.h>
#include <asm/msc01_ic.h>

#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>

#ifdef CONFIG_MIPS_ATLAS
#include <asm/mips-boards/atlasint.h>
#endif
#ifdef CONFIG_MIPS_MALTA
#include <asm/mips-boards/maltaint.h>
#endif
#ifdef CONFIG_MIPS_SEAD
#include <asm/mips-boards/seadint.h>
#endif
#if 0
#ifdef CONFIG_MSTAR_TITANIA2
#define	MSC01E_INT_PERFCTR	10
#define	MSC01E_INT_CPUCTR	       11
#define MSC01E_INT_BASE		64
#endif
#endif

unsigned long cpu_khz;

static int mips_cpu_timer_irq;
static int mips_cpu_perf_irq;
extern int cp0_perfcount_irq;

static void mips_timer_dispatch(void)
{
	do_IRQ(mips_cpu_timer_irq);
}

static void mips_perf_dispatch(void)
{
	do_IRQ(mips_cpu_perf_irq);
}

extern unsigned int Chip_Query_MIPS_CLK(void);

/*
 * Estimate CPU frequency.  Sets mips_hpt_frequency as a side-effect
 */
static unsigned int __init estimate_cpu_frequency(void)
{
	unsigned int prid = read_c0_prid() & 0xffff00;
	unsigned int count=0;

#if defined(CONFIG_MIPS_SEAD) || defined(CONFIG_MIPS_SIM)
	/*
	 * The SEAD board doesn't have a real time clock, so we can't
	 * really calculate the timer frequency
	 * For now we hardwire the SEAD board frequency to 12MHz.
	 */

	if ((prid == (PRID_COMP_MIPS | PRID_IMP_20KC)) ||
	    (prid == (PRID_COMP_MIPS | PRID_IMP_25KF)))
		count = 12000000;
	else
		count = 6000000;
#endif
#if defined(CONFIG_MIPS_ATLAS) || defined(CONFIG_MIPS_MALTA)
	unsigned long flags;
	unsigned int start;

	local_irq_save(flags);

	/* Start counter exactly on falling edge of update flag */
	while (CMOS_READ(RTC_REG_A) & RTC_UIP);
	while (!(CMOS_READ(RTC_REG_A) & RTC_UIP));

	/* Start r4k counter. */
	start = read_c0_count();

	/* Read counter exactly on falling edge of update flag */
	while (CMOS_READ(RTC_REG_A) & RTC_UIP);
	while (!(CMOS_READ(RTC_REG_A) & RTC_UIP));

	count = read_c0_count() - start;

	/* restore interrupts */
	local_irq_restore(flags);
#endif
#ifdef CONFIG_MSTAR_TITANIA2
    //count = 14318180  >> 1;
    //printk( "CPU freq count = %d\n", count ) ;
    if( *((unsigned int*)0xbf803c6c) == 0x2020)
        count = 216000000 >> 1;
    else if( *((unsigned int*)0xbf803c6c) == 0x203C)
        count = 200000000 >> 1;
    else if( *((unsigned int*)0xbf803c6c) == 0x2028)
        count = 170000000 >> 1;
    else if( *((unsigned int*)0xbf803c6c) == 0x202C)
        count = 160000000 >> 1;
    else if( *((unsigned int*)0xbf803c6c) == 0x2030)
        count = 144000000 >> 1;
    else if( *((unsigned int*)0xbf803c6c) == 0x2034)
        count = 123000000 >> 1;
    else if( *((unsigned int*)0xbf803c6c) == 0x2038)
        count = 108000000 >> 1;
    else
    {
        count = 123000000 >> 1;
        printk("\nMCU clock read error !!\n");
    }
    printk( "CPU freq count = %d\n", count ) ;
#endif
#ifdef CONFIG_MSTAR_EUCLID
    count = (*((volatile unsigned int*)0xbf203c48) ) & 0xFF;
    switch (count)
    {
    case 0x34:
        count = 123000000 >> 1;
        break;
    case 0x30:
        count = 144000000 >> 1;
        break;
    case 0x2c:
        count = 160000000 >> 1;
        break;
    case 0x28:
        count = 170000000 >> 1;
        break;
    case 0x20:
        count = 216000000 >> 1;
        break;
    case 0x38:
        count = 240000000 >> 1;
        break;
    default:
        count = 123000000 >> 1;
        printk("\nMCU clock read error !!\n");
        break;
    }
    printk( "CPU freq count = %d\n", count ) ;
#endif

#if (defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10))
    #if 1
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    #else
    count = 192000000>>1 ; //count = 216000000 >> 1;
    #endif
    printk( "CPU freq count = %d\n", count ) ;
#endif


#ifdef CONFIG_MSTAR_TITANIA4
    #if 1
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    #else
    count = 192000000>>1 ; //count = 216000000 >> 1;
    #endif
    printk( "CPU freq count = %d\n", count ) ;
    if(0==count)
    {//jc_db_u4
        printk("!!!!!!!Can't read CPU frequence!!!!!!!!!!\n");
        count = 12000000;
    }

#endif
#ifdef CONFIG_MSTAR_TITANIA8
    #if 1
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    #else
    count = 192000000>>1 ; //count = 216000000 >> 1;
    #endif
    printk( "CPU freq count = %d\n", count ) ;
    if(0==count)
    {//jc_db_u4
        printk("!!!!!!!Can't read CPU frequence!!!!!!!!!!\n");
        count = 360000000;
    }

#endif

#ifdef CONFIG_MSTAR_TITANIA9

    count = Chip_Query_MIPS_CLK();
    count >>= 1;

    printk( "CPU Freq Count = %d\n", count);

    if(0==count)
    {
        printk("!!!!!!!Can't Read CPU Frequence!!!!!!!!!!\n");
        count = (552000000 >> 1);    //Default Clock is 552MHz
    }

#endif
#ifdef CONFIG_MSTAR_TITANIA11
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    printk( "CPU freq count = %d\n", count ) ;
    if(0 == count)
    {
        printk("!!!!!!!Can't read CPU frequence!!!!!!!!!!\n");
        count = 12000000;   //Default 12MHz
    }
#endif
#ifdef CONFIG_MSTAR_TITANIA12
    #if 1
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    #else
    count = 192000000>>1 ; //count = 216000000 >> 1;
    #endif
    printk( "CPU freq count = %d\n", count ) ;
    if(0==count)
    {//jc_db_u4
        printk("!!!!!!!Can't read CPU frequence!!!!!!!!!!\n");
        count = 360000000;
    }
#endif
#ifdef CONFIG_MSTAR_TITANIA13

    count = Chip_Query_MIPS_CLK();
    count >>= 1;

    printk( "CPU Freq Count = %d\n", count);

    if(0==count)
    {
        printk("!!!!!!!Can't Read CPU Frequence!!!!!!!!!!\n");
        count = (552000000 >> 1);    //Default Clock is 552MHz
    }

#endif
#if defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EMERALD)

    count = Chip_Query_MIPS_CLK();
    count >>= 1;

    printk( "CPU Freq Count = %d\n", count);

    if(0==count)
    {
        printk("!!!!!!!Can't Read CPU Frequence!!!!!!!!!!\n");
        count = (552000000 >> 1);    //Default Clock is 552MHz
    }

#endif
#ifdef CONFIG_MSTAR_URANUS4
    #if 1
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    #else
    count = 192000000>>1 ; //count = 216000000 >> 1;
    #endif
    printk( "CPU freq count = %d\n", count ) ;
    if(0==count)
    {//jc_db_u4
        printk("!!!!!!!Can't read CPU frequence!!!!!!!!!!\n");
        count = 252000000;
    }

#endif
#ifdef CONFIG_MSTAR_OBERON
    //count = 14318180  >> 1;
    //printk( "CPU freq count = %d\n", count ) ;
    switch(((*((unsigned int*)0xbf803C88))>>10)&0x7)
    {
        case 0: count = 216000000 >> 1; break;
        case 2: count = 192000000 >> 1; break;
        case 3: count = 173000000 >> 1; break;
        case 4: count = 160000000 >> 1; break;
        case 5: count = 123000000 >> 1; break;
        case 6: count = 108000000 >> 1; break;
        default:
        count = 123000000 >> 1;
        printk("\nMCU clock read error !!\n");
            break;
    }
    printk( "CPU freq count = %d\n", count ) ;
#endif

#ifdef CONFIG_MSTAR_JANUS2
    #if 1
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    #else
    count = 192000000>>1 ; //count = 216000000 >> 1;
    #endif
    printk( "CPU freq count = %d\n", count ) ;
    if(0==count)
    {//jc_db_u4
        printk("!!!!!!!Can't read CPU frequence!!!!!!!!!!\n");
        count = 360000000;
    }

#endif

#ifdef CONFIG_MSTAR_KRONUS
    #if 1
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    #else
    count = 192000000 >> 1; //count = 216000000 >> 1;
    #endif
    printk( "CPU freq count = %d\n", count ) ;
    if (0 == count)
    {
        printk("!!!!!!!Can't read CPU frequence!!!!!!!!!!\n");
        count = (552000000 >> 1);    //Default Clock is 552MHz
    }
#endif

#ifdef CONFIG_MSTAR_KAISERIN
#if defined(CONFIG_MSTAR_KAISERIN_BD_FPGA)
    count = (12000000 >> 1);        //FPGA Clock is 12MHz
#else
    #if 1
    count = Chip_Query_MIPS_CLK();
    count = count >> 1;
    #else
    count = 192000000 >> 1; //count = 216000000 >> 1;
    #endif
    printk( "CPU freq count = %d\n", count);
    if (0 == count)
    {
        printk("!!!!!!!Can't read CPU frequence!!!!!!!!!!\n");
        count = (504000000 >> 1);    //Default Clock is 504MHz
    }
#endif
#endif
	mips_hpt_frequency = count;
	if ((prid != (PRID_COMP_MIPS | PRID_IMP_20KC)) &&
	    (prid != (PRID_COMP_MIPS | PRID_IMP_25KF)))
		count *= 2;

	count += 5000;    /* round */
	count -= count%10000;

	return count;
}

unsigned long read_persistent_clock(void)
{
	//return mc146818_get_cmos_time(); // Dean
	return 0; // actually, porting this function to read RTC value
}

static void __init plat_perf_setup(void)
{
#ifdef MSC01E_INT_BASE
	if (cpu_has_veic) {
		set_vi_handler(MSC01E_INT_PERFCTR, mips_perf_dispatch);
		mips_cpu_perf_irq = MSC01E_INT_BASE + MSC01E_INT_PERFCTR;
	} else
#endif
	if (cp0_perfcount_irq >= 0) {
		if (cpu_has_vint)
			set_vi_handler(cp0_perfcount_irq, mips_perf_dispatch);
		mips_cpu_perf_irq = MIPS_CPU_IRQ_BASE + cp0_perfcount_irq;
#ifdef CONFIG_SMP
		set_irq_handler(mips_cpu_perf_irq, handle_percpu_irq);
#endif
	}
}

unsigned int __cpuinit get_c0_compare_int(void)
{
#ifdef MSC01E_INT_BASE
	if (cpu_has_veic) {
		set_vi_handler(MSC01E_INT_CPUCTR, mips_timer_dispatch);
		mips_cpu_timer_irq = MSC01E_INT_BASE + MSC01E_INT_CPUCTR;
	} else
#endif
	{
		if (cpu_has_vint)
			set_vi_handler(cp0_compare_irq, mips_timer_dispatch);
		mips_cpu_timer_irq = MIPS_CPU_IRQ_BASE + cp0_compare_irq;
	}

	return mips_cpu_timer_irq;
}

void __init plat_time_init(void)
{
	unsigned int est_freq;

        /* Set Data mode - binary. */
        //CMOS_WRITE(CMOS_READ(RTC_CONTROL) | RTC_DM_BINARY, RTC_CONTROL);

	est_freq = estimate_cpu_frequency();

	printk("CPU frequency %d.%02d MHz\n", est_freq/1000000,
	       (est_freq%1000000)*100/1000000);

        cpu_khz = est_freq / 1000;

	mips_scroll_message();
#ifdef CONFIG_I8253		/* Only Malta has a PIT */
	setup_pit_timer();
#endif

	plat_perf_setup();

       set_c0_status(IE_IRQ0 | IE_IRQ1 | IE_IRQ5);
}

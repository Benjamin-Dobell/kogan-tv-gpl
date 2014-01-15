#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/mach/time.h>
#include <mach/hardware.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>

#include <mach/io.h>
#include <mach/timex.h>
#include <asm/irq.h>
#include <mach/pm.h>
#include <linux/timer.h>

#include "chip_int.h"


//ARM Global Timer
static int GLB_TIMER_FREQ_KHZ;	// PERICLK = CPUCLK/2

int query_frequency(void)
{
    return (12 * (reg_readw(0x1F000000 + (0x110C26 << 1)) & 0x00FF)); // 1 = 1(Mhz)
}
EXPORT_SYMBOL(query_frequency);

#if defined(CONFIG_MSTAR_EAGLE_FPGA)
#define CLOCK_TICK_RATE         (12*1000*1000)
#endif


#ifdef CONFIG_GENERIC_CLOCKEVENTS

//static unsigned int clksrc_base;
//static unsigned int clkevt_base;

static cycle_t timer_read(struct clocksource *cs)
{
	cycle_t src_timer_cnt;

    src_timer_cnt = PERI_R(GT_LOADER_UP);
    src_timer_cnt = (src_timer_cnt << 32) + PERI_R(GT_LOADER_LOW);
//	printk("%s(): src_timer_cnt = 0x%llx\n",__FUNCTION__, src_timer_cnt);

    return src_timer_cnt;
}

static struct clocksource clocksource_timer =
{
    .name		= "timer1",
    .rating		= 200,
    .read		= timer_read,
    .mask		= CLOCKSOURCE_MASK(32),
    .shift		= 20,
    .flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

//void __init eagle_clocksource_init(unsigned int base)
void __init eagle_clocksource_init(void)
{
	struct clocksource *cs = &clocksource_timer;

//	clksrc_base = base;

	PERI_W(GT_CONTROL, 0x1); //Enable

	//calculate the value of mult    //cycle= ( time(ns) *mult ) >> shift
	cs->mult = clocksource_khz2mult(GLB_TIMER_FREQ_KHZ, cs->shift);

	clocksource_register(cs);
}

unsigned long long notrace sched_clock(void)
{
    // use 96bit data to prevent overflow
    cycle_t cycles = clocksource_timer.read(&clocksource_timer);
    u32 mult = clocksource_timer.mult;
    u32 shift = clocksource_timer.shift;

    u64 cyc_up=(cycles>>32);
    u64 cyc_low=(cycles&0xFFFFFFFF);
    u64 cyc_tmp;
    cyc_up  *= mult;
    cyc_low *= mult;
    cyc_tmp = cyc_low;
    cyc_low = (cyc_low + (cyc_up <<32));
    cyc_up  = ((cyc_up + (cyc_tmp>>32))>>32);
	return (shift>=64)?0:((cyc_low>>shift)|(cyc_up<<(64-shift)));
}

/*
 * IRQ handler for the timer
 */
static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
    struct clock_event_device *evt = dev_id;
#if defined(CONFIG_MSTAR_EAGLE)
    int cpu = smp_processor_id();
    /* local_timer call pmu_handle_irq instead of PMU interrupt can't raise by itself. */
        extern u32 armpmu_enable_flag[];
    if (armpmu_enable_flag[cpu]) {
        extern irqreturn_t armpmu_handle_irq(int irq_num, void *dev);
        armpmu_handle_irq(cpu, NULL);
    }
#endif
	//clear timer event flag
	PERI_W(PT_STATUS, FLAG_EVENT);

    evt->event_handler(evt);

    return IRQ_HANDLED;
}

static void timer_set_mode(enum clock_event_mode mode, struct clock_event_device *evt)
{
	unsigned long interval;
	unsigned long ctl = FLAG_TIMER_PRESCALAR;

//	printk("\n\n\n====> %s(): mode = %d\n\n\n", __FUNCTION__, mode);

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		interval = (GLB_TIMER_FREQ_KHZ * 1000 / HZ);
		PERI_W(PT_LOADER, interval);
		ctl |= FLAG_IT_ENABLE | FLAG_AUTO_RELOAD | FLAG_TIMER_ENABLE;
		break;

	case CLOCK_EVT_MODE_ONESHOT:
		PERI_W(PT_COUNTER, 0);
		ctl |= FLAG_IT_ENABLE | FLAG_TIMER_ENABLE;
		break;

    case CLOCK_EVT_MODE_UNUSED:
    case CLOCK_EVT_MODE_SHUTDOWN:
    default:
    	break;
    }

	PERI_W(PT_CONTROL, ctl);
//	printk("%s(): ctl = 0x%lx\n", __FUNCTION__, ctl);
}

static int timer_set_next_event(unsigned long next, struct clock_event_device *evt)
{
//	printk("%s(): next = 0x%lx\n", __FUNCTION__, next);

    PERI_W(PT_COUNTER, next);

    return 0;
}

static struct clock_event_device clockevent_timer =
{
	.name		= "timer0",
	.shift		= 32,
	.features	= CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.set_mode	= timer_set_mode,
	.set_next_event	= timer_set_next_event,
	.rating		= 300,
	.cpumask	= cpu_all_mask,
};

static struct irqaction timer_irq =
{
	.name		= "timer",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= timer_interrupt,
	.dev_id		= &clockevent_timer,
};

//void __init eagle_clockevents_init(unsigned int base,unsigned int irq)
void __init eagle_clockevents_init(unsigned int irq)
{
	struct clock_event_device *evt = &clockevent_timer;
	unsigned long temp;

//	clkevt_base = base;

	evt->irq = irq;
	evt->mult = div_sc(GLB_TIMER_FREQ_KHZ, NSEC_PER_MSEC, evt->shift);
	evt->max_delta_ns = clockevent_delta2ns(0xffffffff, evt);
	evt->min_delta_ns = clockevent_delta2ns(0xf, evt);

	//clear timer event flag
	PERI_W(PT_STATUS, FLAG_EVENT);

	//Interrupt Set Enable Register
	temp = PERI_R(GIC_DIST_SET_EANBLE);
	temp = temp | (0x1 << irq);
	PERI_W(GIC_DIST_SET_EANBLE, temp);

    setup_irq(irq, &timer_irq);

    clockevents_register_device(evt);
}

#else	// !CONFIG_GENERIC_CLOCKEVENTS

static irqreturn_t eagle_timer_interrupt(int irq, void *dev_id)
{
	timer_tick();

	//clear timer event flag
	PERI_W(PT_STATUS, FLAG_EVENT);

	return IRQ_HANDLED;
}

static struct irqaction eagle_timer_irq =
{
	.name	= "Timer Tick",
	.flags	= IRQF_TIMER | IRQF_IRQPOLL | IRQF_DISABLED,
	.handler	= eagle_timer_interrupt,
};

#endif	// CONFIG_GENERIC_CLOCKEVENTS


void __init eagle_init_timer(void)
{
#ifdef CONFIG_MSTAR_EAGLE_BD_FPGA
	GLB_TIMER_FREQ_KHZ = 24 * 1000 ;	// PERIPHCLK = CPU Clock / 2,
										// div 2 later,when CONFIG_GENERIC_CLOCKEVENTS
										// clock event will handle this value
#elif CONFIG_MSTAR_EAGLE_BD_GENERIC
	GLB_TIMER_FREQ_KHZ = (query_frequency() * 1000 / 2);	// PERIPHCLK = CPU Clock / 2
															// div 2 later,when CONFIG_GENERIC_CLOCKEVENTS
															// clock event will handle this value
#endif

	printk("Global Timer Frequency = %d MHz\n", GLB_TIMER_FREQ_KHZ / 1000);
	printk("CPU Clock Frequency = %d MHz\n", query_frequency());


#ifdef CONFIG_GENERIC_CLOCKEVENTS

	eagle_clocksource_init();
	eagle_clockevents_init(INT_ID_PTIMER);

#else
{
	unsigned long interval, temp;

    //clear timer event flag
    PERI_W(PT_STATUS,FLAG_EVENT);

    //set timer interrupt interval
    interval = (GLB_TIMER_FREQ_KHZ * 1000 / HZ);
    PERI_W(PT_LOADER,interval);

    //Interrupt Set Enable Register
    temp = PERI_R(GIC_DIST_SET_EANBLE);
    temp = temp | (0x1 << INT_ID_PTIMER);
    PERI_W(GIC_DIST_SET_EANBLE, temp);

    //setup timer IRQ
    setup_irq(INT_ID_PTIMER, &eagle_timer_irq);

    //start timer
     PERI_W(PT_CONTROL, FLAG_TIMER_PRESCALAR | FLAG_IT_ENABLE | FLAG_AUTO_RELOAD | FLAG_TIMER_ENABLE);
}
#endif
}

struct sys_timer eagle_timer =
{
	.init	= eagle_init_timer,
};


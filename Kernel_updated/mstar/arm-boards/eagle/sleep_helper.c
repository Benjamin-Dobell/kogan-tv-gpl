#include <asm/cacheflush.h>
#include "sleep_helper.h"

appf_u32 performance_monitor_save[PM_SLEEP_SAVE_LEN];
a9_timer_context a9_timer_save;
appf_u32 gic_interface_save[GIC_INTERFACE_SAVE_LEN];
appf_u32 gic_distributor_private_save[GIC_DISTRIBUTOR_PRIVATE_SAVE_LEN];
appf_u32 cp15_save[CP15_SAVE_LEN];
appf_u32 a9_other_save[A9_OTHER_LEN];
appf_u32 a9_global_timer_save[A9_GLOBAL_TIMER_LEN];
appf_u32 gic_distributor_shared_save[GIC_DISTRIBUTOR_SHARED_SAVE_LEN];
appf_u32 a9_scu_save[A9_SCU_SAVE_LEN];
pl310_context pl310_context_save;
debug_context_t a9_dbg_data_save;
appf_u32 control_data[CONTROL_DATA_SAVE_LEN];
appf_u32 mmu_data[MMU_DATA_SAVE_LEN];

#define A9_TIMERS_OFFSET 0x600

void save_a9_timers(appf_u32 *pointer, unsigned scu_address)
{
    a9_timer_context *context = (a9_timer_context *)pointer;
    a9_timer_registers *timers = (a9_timer_registers *)(scu_address + A9_TIMERS_OFFSET);

    /*
     * First, stop the timers
     */
    context->timer_control    = timers->timer_control;
    timers->timer_control     = 0;
    context->watchdog_control = timers->watchdog_control;
    timers->watchdog_control  = 0;

    context->timer_load                = timers->timer_load;
    context->timer_counter             = timers->timer_counter;
    context->timer_interrupt_status    = timers->timer_interrupt_status;
    context->watchdog_load             = timers->watchdog_load;
    context->watchdog_counter          = timers->watchdog_counter;
    context->watchdog_interrupt_status = timers->watchdog_interrupt_status;
    /*
     * We ignore watchdog_reset_status, since it can only clear the status bit.
     * If the watchdog has reset the system, the OS will want to know about it.
     * Similarly, we have no use for watchdog_disable - this is only used for
     * returning to timer mode, which is the default mode after reset.
     */
}

void restore_a9_timers(appf_u32 *pointer, unsigned scu_address)
{
    a9_timer_context *context = (a9_timer_context *)pointer;
    a9_timer_registers *timers = (a9_timer_registers *)(scu_address + A9_TIMERS_OFFSET);

    timers->timer_control = 0;
    timers->watchdog_control = 0;

    /*
     * We restore the load register first, because it also sets the counter register.
     */
    timers->timer_load    = context->timer_load;
    timers->watchdog_load = context->watchdog_load;

    /*
     * If a timer has reached zero (presumably during the context save) and triggered
     * an interrupt, then we set it to the shortest possible expiry time, to make it
     * trigger again real soon.
     * We could fake this up properly, but we would have to wait around until the timer
     * ticked, which could be some time if PERIPHCLK is slow. This approach should be
     * good enough in most cases.
     */
    if (context->timer_interrupt_status)
    {
        timers->timer_counter = 1;
    }
    else
    {
        timers->timer_counter = context->timer_counter;
    }

    if (context->watchdog_interrupt_status)
    {
        timers->watchdog_counter = 1;
    }
    else
    {
        timers->watchdog_counter = context->watchdog_counter;
    }

    timers->timer_control = context->timer_control;
    timers->watchdog_control = context->watchdog_control;
}


void save_a9_global_timer(appf_u32 *pointer, unsigned scu_address)
{
    a9_global_timer_registers *timer = (void*)(scu_address + A9_GLOBAL_TIMER_OFFSET);
    a9_global_timer_context *context = (void*)pointer;

    unsigned tmp_lo, tmp_hi, tmp2_hi;

    do
    {
        tmp_hi  = timer->counter_hi;
        tmp_lo  = timer->counter_lo;
        tmp2_hi = timer->counter_hi;
    } while (tmp_hi != tmp2_hi);

    context->counter_lo     = tmp_lo;
    context->counter_hi     = tmp_hi;
    context->control        = timer->control;
    context->status         = timer->status;
    context->comparator_lo  = timer->comparator_lo;
    context->comparator_hi  = timer->comparator_hi;
    context->auto_increment = timer->auto_increment;
}

void restore_a9_global_timer(appf_u32 *pointer, unsigned scu_address)
{
    a9_global_timer_registers *timer = (void*)(scu_address + A9_GLOBAL_TIMER_OFFSET);
    a9_global_timer_context *context = (void*)pointer;

    unsigned long long comparator_ull, current_ull;
    unsigned current_hi, current_lo;

    /* Is the timer currently enabled? */
    if (timer->control & A9_GT_TIMER_ENABLE)
    {
        /* Temporarily stop the timer so we can mess with it */
        timer->control &= ~A9_GT_TIMER_ENABLE;
    }
    else /* We must be the first CPU back up, or the timer is not in use */
    {
        /*
         * Restore the counter value - possibly we should update it to
         * reflect the length of time the power has been off
         */
        timer->counter_lo    = context->counter_lo;
        timer->counter_hi    = context->counter_hi;
    }

    /*
     * Timer is now stopped - do we need to set its event flag?
     * We set it if the event flag was set when the context was saved,
     * or the timer has expired and the Compare control bit was set.
     */
    current_hi     = timer->counter_hi;
    current_lo     = timer->counter_lo;
    current_ull    = ((unsigned long long)current_hi << 32) + current_lo;
    comparator_ull = ((unsigned long long)context->comparator_hi << 32) + context->comparator_lo;

    if ((context->status & A9_GT_EVENT_FLAG) ||
        ((current_ull > comparator_ull) &&
        ((context->control & (A9_GT_COMPARE_ENABLE | A9_GT_TIMER_ENABLE))
                 == ((A9_GT_COMPARE_ENABLE | A9_GT_TIMER_ENABLE)))))
    {
        /* Set the comparator to the current counter value */
        timer->comparator_hi = current_hi;
        timer->comparator_lo = current_lo;

        /* Start the timer */
        timer->control = context->control;

        /* Wait for the timer event */
        while (timer->status == 0)
        {
                /* Do nothing */
        }

        /* Set the comparator to the original value */
        timer->comparator_lo = context->comparator_lo;
        timer->comparator_hi = context->comparator_hi;
    }
    else /* Event flag does not need to be set */
    {
        timer->comparator_lo = context->comparator_lo;
        timer->comparator_hi = context->comparator_hi;
        /* Start the timer */
        timer->control = context->control;
    }

    timer->auto_increment = context->auto_increment;
}

/*
 * Saves the GIC CPU interface context
 * Requires 3 or 4 words of memory
 */
void save_gic_interface(appf_u32 *pointer, unsigned gic_interface_address, int is_secure)
{
    cpu_interface *ci = (cpu_interface *)gic_interface_address;

    pointer[0] = ci->control;
    pointer[1] = ci->priority_mask;
    pointer[2] = ci->binary_point;

    if (is_secure)
    {
        pointer[3] = ci->aliased_binary_point;
    }
}
void restore_gic_interface(appf_u32 *pointer, unsigned gic_interface_address, int is_secure)
{
    cpu_interface *ci = (cpu_interface *)gic_interface_address;

    ci->priority_mask = pointer[1];
    ci->binary_point = pointer[2];

    if (is_secure)
    {
        ci->aliased_binary_point = pointer[3];
    }

    /* Restore control register last */
    ci->control = pointer[0];
}

/*
 * Saves this CPU's banked parts of the distributor
 * Returns non-zero if an SGI/PPI interrupt is pending (after saving all required context)
 * Requires 19 words of memory
 */
int save_gic_distributor_private(appf_u32 *pointer, unsigned gic_distributor_address, int is_secure)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;

    *pointer = id->enable.set[0];
    ++pointer;
    pointer = copy_words(pointer, id->priority, 8);
    pointer = copy_words(pointer, id->target, 8);
    if (is_secure)
    {
        *pointer = id->security[0];
        ++pointer;
    }
    /* Save just the PPI configurations (SGIs are not configurable) */
    *pointer = id->configuration[1];
    ++pointer;
    *pointer = id->pending.set[0];
    if (*pointer)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
void restore_gic_distributor_private(appf_u32 *pointer, unsigned gic_distributor_address, int is_secure)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;

    /* We assume the distributor is disabled so we can write to its config registers */

    id->enable.set[0] = *pointer;
    ++pointer;
    copy_words(id->priority, pointer, 8);
    pointer += 8;
    copy_words(id->target, pointer, 8);
    pointer += 8;
    if (is_secure)
    {
        id->security[0] = *pointer;
        ++pointer;
    }
    /* Restore just the PPI configurations (SGIs are not configurable) */
    id->configuration[1] = *pointer;
    ++pointer;
    id->pending.set[0] = *pointer;
}

debug_registers_t *read_debug_address(void)
{
    unsigned drar, dsar;

    drar = read_drar();
    dsar = read_dsar();

    if (!(drar & DRAR_VALID_MASK)
     || !(dsar & DSAR_VALID_MASK))
    {
        return 0;  /* No memory-mapped debug on this processor */
    }

    return (debug_registers_t *)((drar & DRAR_ADDRESS_MASK)
                               + (dsar & DSAR_ADDRESS_MASK));
}

/*
 * We assume that before save (and after restore):
 *   - OSLAR is NOT locked, or the debugger would not work properly
 *   - LAR is locked, because the ARM ARM says it must be
 *   - OSDLR is NOT locked, or the debugger would not work properly
 */

void save_v7_debug(appf_u32 *context)
{
    debug_registers_t *dbg = (void*)read_debug_address();
    debug_context_t *ctx = (void*)context;
    unsigned v71, num_bps, num_wps, i;
    appf_u32 didr;

    if (!dbg)
    {
        return;
    }

    didr = dbg->didr;
    /*
     * Work out what version of debug we have
     */
    v71 = (((didr >> DIDR_VERSION_SHIFT) & DIDR_VERSION_MASK) == DIDR_VERSION_7_1);

    /*
     * Save all context to memory
     */
    ctx->vcr    = dbg->vcr;
    ctx->dscr_e = dbg->dscr_e;
    ctx->claim  = dbg->claimclr;

    if (v71)
    {
        ctx->eacr = dbg->eacr;
    }

    num_bps = 1 + ((didr >> DIDR_BP_SHIFT) & DIDR_BP_MASK);
    for (i=0; i<num_bps; ++i)
    {
        ctx->bvr[i]  = dbg->bvr[i];
        ctx->bcr[i]  = dbg->bcr[i];
#ifdef VIRTUALIZATION
        ctx->bxvr[i] = dbg->bxvr[i]; /* TODO: don't save the ones that don't exist */
#endif
    }

    num_wps = 1 + ((didr >> DIDR_WP_SHIFT) & DIDR_WP_MASK);
    for (i=0; i<num_wps; ++i)
    {
        ctx->wvr[i] = dbg->wvr[i];
        ctx->wcr[i] = dbg->wcr[i];
    }

    /*
     * If Debug V7.1, we must set osdlr (by cp14 interface) before power down.
     * Once we have done this, debug becomes inaccessible.
     */
    if (v71)
    {
        write_osdlr(OSDLR_LOCKED);
    }
}

void restore_v7_debug(appf_u32 *context)
{
    debug_registers_t *dbg = (void*)read_debug_address();
    debug_context_t *ctx = (void*)context;
    unsigned v71, num_bps, num_wps, i;
    appf_u32 didr;

    if (!dbg)
    {
        return;
    }

    didr = dbg->didr;
    /*
     * Work out what version of debug we have
     */
    v71 = (((didr >> DIDR_VERSION_SHIFT) & DIDR_VERSION_MASK) == DIDR_VERSION_7_1);

    /* Enable write access to registers */
    dbg->lar = LAR_UNLOCKED;
    /*
     * If Debug V7.1, we must unset osdlr (by cp14 interface) before restoring.
     * (If the CPU has not actually power-cycled, osdlr may not be reset).
     */
    if (v71)
    {
        write_osdlr(OSDLR_UNLOCKED);
    }

    /*
     * Restore all context from memory
     */
    dbg->vcr      = ctx->vcr;
    dbg->claimclr = CLAIMCLR_CLEAR_ALL;
    dbg->claimset = ctx->claim;

    if (v71)
    {
        dbg->eacr = ctx->eacr;
    }

    num_bps = 1 + ((didr >> DIDR_BP_SHIFT) & DIDR_BP_MASK);
    for (i=0; i<num_bps; ++i)
    {
        dbg->bvr[i]  = ctx->bvr[i];
        dbg->bcr[i]  = ctx->bcr[i];
#ifdef VIRTUALIZATION
        dbg->bxvr[i] = ctx->bxvr[i];  /* TODO: don't restore the ones that don't exist */
#endif
    }

    num_wps = 1 + ((didr >> DIDR_WP_SHIFT) & DIDR_WP_MASK);
    for (i=0; i<num_wps; ++i)
    {
        dbg->wvr[i] = ctx->wvr[i];
        dbg->wcr[i] = ctx->wcr[i];
    }

    /* Clear PRSR.SPD by reading PRSR */
    if (!v71)
    {
        (dbg->prsr);
    }

    /* Re-enable debug */
    dbg->dscr_e   = ctx->dscr_e;

    /* Disable write access to registers */
    dbg->lar = LAR_LOCKED;
}
/*
 * Enables or disables the GIC distributor (for the current security state)
 * Parameter 'enabled' is boolean.
 * Return value is boolean, and reports whether GIC was previously enabled.
 */
int gic_distributor_set_enabled(int enabled, unsigned gic_distributor_address)
{
    unsigned tmp;
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;

    tmp = id->control;
    if (enabled)
    {
        id->control = tmp | GIC_DIST_ENABLE;
    }
    else
    {
        id->control = tmp & ~GIC_DIST_ENABLE;
    }
    return (tmp & GIC_DIST_ENABLE) != 0;
}
/*
 * Saves the shared parts of the distributor.
 * Requires 1 word of memory, plus 20 words for each block of 32 SPIs (max 641 words)
 * Returns non-zero if an SPI interrupt is pending (after saving all required context)
 */
int save_gic_distributor_shared(appf_u32 *pointer, unsigned gic_distributor_address, int is_secure)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned num_spis, *saved_pending;
    int i, retval = 0;


    /* Calculate how many SPIs the GIC supports */
    num_spis = 32 * (id->controller_type & 0x1f);

    /* TODO: add nonsecure stuff */

    /* Save rest of GIC configuration */
    if (num_spis)
    {
        pointer = copy_words(pointer, id->enable.set + 1,    num_spis / 32);
        pointer = copy_words(pointer, id->priority + 8,      num_spis / 4);
        pointer = copy_words(pointer, id->target + 8,        num_spis / 4);
        pointer = copy_words(pointer, id->configuration + 2, num_spis / 16);
        if (is_secure)
        {
            pointer = copy_words(pointer, id->security + 1,  num_spis / 32);
        }
        saved_pending = pointer;
        pointer = copy_words(pointer, id->pending.set + 1,   num_spis / 32);

        /* Check interrupt pending bits */
        for (i=0; i<num_spis/32; ++i)
        {
            if (saved_pending[i])
            {
                retval = -1;
                break;
            }
        }
    }

    /* Save control register */
    *pointer = id->control;

    return retval;
}
void restore_gic_distributor_shared(appf_u32 *pointer, unsigned gic_distributor_address, int is_secure)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned num_spis;

    /* Make sure the distributor is disabled */
    gic_distributor_set_enabled(FALSE, gic_distributor_address);

    /* Calculate how many SPIs the GIC supports */
    num_spis = 32 * ((id->controller_type) & 0x1f);

    /* TODO: add nonsecure stuff */

    /* Restore rest of GIC configuration */
    if (num_spis)
    {
        copy_words(id->enable.set + 1, pointer, num_spis / 32);
        pointer += num_spis / 32;
        copy_words(id->priority + 8, pointer, num_spis / 4);
        pointer += num_spis / 4;
        copy_words(id->target + 8, pointer, num_spis / 4);
        pointer += num_spis / 4;
        copy_words(id->configuration + 2, pointer, num_spis / 16);
        pointer += num_spis / 16;
        if (is_secure)
        {
            copy_words(id->security + 1, pointer, num_spis / 32);
            pointer += num_spis / 32;
        }
        copy_words(id->pending.set + 1, pointer, num_spis / 32);
        pointer += num_spis / 32;
    }

    /* Restore control register - if the GIC was disabled during save, it will be restored as disabled. */
    id->control = *pointer;

    return;
}

/*
 * TODO: we need to use the power status register, not save it!
 */

void save_a9_scu(appf_u32 *pointer, unsigned scu_address)
{
    a9_scu_registers *scu = (a9_scu_registers *)scu_address;

    pointer[0] = scu->control;
    pointer[1] = scu->power_status.w;
    pointer[2] = scu->filtering_start;
    pointer[3] = scu->filtering_end;
    pointer[4] = scu->access_control;
    pointer[5] = scu->ns_access_control;
}

void restore_a9_scu(appf_u32 *pointer, unsigned scu_address)
{
    a9_scu_registers *scu = (a9_scu_registers *)scu_address;

    scu->invalidate_all = 0xffff;
    scu->filtering_start = pointer[2];
    scu->filtering_end = pointer[3];
    scu->access_control = pointer[4];
    scu->ns_access_control = pointer[5];
    scu->power_status.w = pointer[1];
    scu->control = pointer[0];
    //flush_cache_all();
}
static void inv_pl310(unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    int i;

    pl310->inv_way = 0xffff;
    while (pl310->inv_way)
    {
        /* Spin */
        for (i=10; i>0; --i)
        {
            __nop();
        }
    }
}
void save_pl310(appf_u32 *pointer, unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    pl310_context *context = (pl310_context *)pointer;
    int i;

    /* TODO: are all these registers are present in earlier PL310 versions? */
    context->aux_control = pl310->aux_control;
    context->tag_ram_control = pl310->tag_ram_control;
    context->data_ram_control = pl310->data_ram_control;
    context->ev_counter_ctrl = pl310->ev_counter_ctrl;
    context->ev_counter1_cfg = pl310->ev_counter1_cfg;
    context->ev_counter0_cfg = pl310->ev_counter0_cfg;
    context->ev_counter1 = pl310->ev_counter1;
    context->ev_counter0 = pl310->ev_counter0;
    context->int_mask = pl310->int_mask;
    context->lock_line_en = pl310->lock_line_en;
    for (i=0; i<8; ++i)
    {
        context->lockdown[i].d = pl310->lockdown[i].d;
        context->lockdown[i].i = pl310->lockdown[i].i;
    }
    context->addr_filtering_start = pl310->addr_filtering_start;
    context->addr_filtering_end = pl310->addr_filtering_end;
    context->debug_ctrl = pl310->debug_ctrl;
    context->prefetch_ctrl = pl310->prefetch_ctrl;
    context->power_ctrl = pl310->power_ctrl;
}

void restore_pl310(appf_u32 *pointer, unsigned pl310_address, int dormant)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    pl310_context *context = (pl310_context *)pointer;
    int i;

    /* We may need to disable the PL310 if the boot code has turned it on */
    if (pl310->control)
    {
        /* Wait for the cache to be idle, then disable */
        pl310->cache_sync = 0;
        dsb();
        pl310->control = 0;
    }

    /* TODO: are all these registers present in earlier PL310 versions? */
    pl310->aux_control = context->aux_control;
    pl310->tag_ram_control = context->tag_ram_control;
    pl310->data_ram_control = context->data_ram_control;
    pl310->ev_counter_ctrl = context->ev_counter_ctrl;
    pl310->ev_counter1_cfg = context->ev_counter1_cfg;
    pl310->ev_counter0_cfg = context->ev_counter0_cfg;
    pl310->ev_counter1 = context->ev_counter1;
    pl310->ev_counter0 = context->ev_counter0;
    pl310->int_mask = context->int_mask;
    pl310->lock_line_en = context->lock_line_en;
    for (i=0; i<8; ++i)
    {
        pl310->lockdown[i].d = context->lockdown[i].d;
        pl310->lockdown[i].i = context->lockdown[i].i;
    }
    pl310->addr_filtering_start = context->addr_filtering_start;
    pl310->addr_filtering_end = context->addr_filtering_end;
    pl310->debug_ctrl = context->debug_ctrl;
    pl310->prefetch_ctrl = context->prefetch_ctrl;
    pl310->power_ctrl = context->power_ctrl;
    dsb();

    /*
     * If the RAMs were powered off, we need to invalidate the cache
     */
    if (!dormant)
    {
        inv_pl310(pl310_address);
    }

    pl310->control = 1;
    dsb();
}

int is_enabled_pl310(unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;

    return (pl310->control & 1);
}

void clean_pl310(unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    int i;

    pl310->clean_way = 0xffff;
    while (pl310->clean_way)
    {
        /* Spin */
        for (i=10; i>0; --i)
        {
            __nop();
        }
    }
}



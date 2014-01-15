
#include <asm/cacheflush.h>


#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )

#include "chip_setup.h"

void Chip_Flush_Cache_All(void)
{
    unsigned long   flags = 0;

    local_irq_save(flags);

    // write back and invalid L1 cache
    __flush_cache_all();

    // write back and invalid L2 cache
    Chip_L2_cache_wback_inv(0,0);

    local_irq_restore(flags);
}
#endif


EXPORT_SYMBOL(Chip_Flush_Cache_All);


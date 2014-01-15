#ifndef __MHAL_SWREG_H__
#define __MHAL_SWREG_H__

#define SWREG_N_BIT_WIDTH_MASK(n)   ((1<<(n))-1)
#define SWREG_MASK_RANGE(high,low)  (SWREG_N_BIT_WIDTH_MASK(high+1)-SWREG_N_BIT_WIDTH_MASK(low))
#define SWREG_BIT_RANGE(high,low)   low, SWREG_MASK_RANGE(high,low)

#define SWREG_BASE              0x280
                                //reg , SWREG_BIT_RANGE(high,low)
#define SWREG_REG_SYS_HOLD      0x00,   SWREG_BIT_RANGE(0,0)

#endif//__MHAL_SWREG_H__

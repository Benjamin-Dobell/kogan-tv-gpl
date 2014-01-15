#ifndef __MDRV_SWREG_H__
#define __MDRV_SWREG_H__

struct SWREG_Param
{
    U16 u16RegValue;
} __attribute__ ((packed));

typedef struct SWREG_Param SWREG_Param_t;

#define MDrv_SWREG_REG(addr) (*((volatile U16*)(0xbf200000+(SWREG_BASE+addr)*4)))
void MDrv_SWREG_WRITE_BIT(U8 offset, U8 low_bit, U16 mask, U16 value);
U16 MDrv_SWREG_READ_BIT(U8 offset, U8 low_bit, U16 mask);

void SWREG_SYS_HOLD(void);

#endif//__MDRV_SWREG_H__

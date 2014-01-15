/*******************************************************************************
*
* g3dkmd_defs.h
*
*   -G3Dk
*   -Command queue management
*
* Copyright (c) 2009-2010 MStar Semiconductor, Inc.
*
*******************************************************************************/
#if 0
#ifndef _G3DKMD_DEFS_H_
#define _G3DKMD_DEFS_H_


#include "mhal_g3dkmd.h"


/*------------------------------------------------------------------------------
 *
 * Global variables
 *
 *----------------------------------------------------------------------------*/
extern unsigned int g_g3dRegisterBase;


/*------------------------------------------------------------------------------
 *
 * Macros
 *
 *----------------------------------------------------------------------------*/
#ifdef LG_LINUX_DRIVER
//#define REG_LOW(x)      ((x - 0x8000) * 2)
//#define REG_HIGH(x)     ((x - 0x8000) * 2 + 4)
#define REG_LOW(x)      ((x & 0xFF) * 2)
#define REG_HIGH(x)     ((x & 0xFF) * 2 + 4)
#else
#define REG_LOW(x)      (x*2)
#define REG_HIGH(x)     (x*2 + 4)
#endif

/*
#ifdef HW_CMODEL

#define REG_WRITE       (reg, value)
#define REG_MASK_WRITE  (reg, value, mask)
#define REG_READ        (reg)                   0
#define REG_LATCH_READ  (reg, value)            ((value) = 0)

#else  // !HW_CMODEL
*/

#define REG_WRITE(reg, value)                                                           \
    *(volatile U16*)(g_g3dRegisterBase + REG_LOW(reg))   = (U16)(value & 0x0000ffff);   \
    *(volatile U16*)(g_g3dRegisterBase + REG_HIGH(reg))  = (U16)(value >> 16);

#define REG_MASK_WRITE(reg, value, mask)                                    \
{                                                                           \
    U32 data;                                                               \
    data = *(volatile U16*)(g_g3dRegisterBase + REG_LOW(reg));              \
    data |= (*(volatile U16*)(g_g3dRegisterBase + REG_HIGH(reg))) << 16;    \
    data = (data & ~mask) | (value & mask);                                 \
    REG_WRITE(reg, data);                                                   \
}

#define REG_READ(reg)                                                       \
    (((*(volatile U32*)(g_g3dRegisterBase + REG_LOW(reg))) & 0x0000ffff) |  \
     ((*(volatile U32*)(g_g3dRegisterBase + REG_HIGH(reg))) << 16))

#define REG_LATCH_READ(reg, value)                                                              \
{                                                                                               \
    *(volatile U16*)(g_g3dRegisterBase + REG_LOW(REG_MULTI_BUF)) |= (U16)MSK_LATCH_VALUE_EN;    \
    value = (((*(volatile U32*)(g_g3dRegisterBase + REG_LOW(reg))) & 0x0000ffff) |              \
             ((*(volatile U32*)(g_g3dRegisterBase + REG_HIGH(reg))) << 16));                    \
    *(volatile U16*)(g_g3dRegisterBase + REG_LOW(REG_MULTI_BUF)) &= (U16)(~MSK_LATCH_VALUE_EN); \
}

//#endif  // !HW_CMODEL

#endif
#endif  /* _G3DKMD_DEFS_H_ */

////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>

#include "mdrv_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* hal level register access maacro functions */
#define _BITMASK(loc_msb, loc_lsb)      ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))
#define BITMASK(x)                      _BITMASK(1?x, 0?x)
#define BITFLAG(loc)                    (1U << (loc))
#define IS_BITS_SET(val, bits)          (((val)&(bits)) == (bits))
#define MAKEWORD(value1, value2)        (((U16)(value1)) * 0x100) + (value2)


#define X2BYTE(addr)                    *(volatile U16*)(0xBF200000 + (U32)(addr))
#define X1BYTEH(addr)                   *(volatile U8*)(0xBF200000 + (U32)(addr) + 1)
#define X1BYTEL(addr)                   *(volatile U8*)(0xBF200000 + (U32)(addr))
#define WORD2REAL(W)                    ((W)<<2)
#define _MHal_R1BH( u32Reg )            X1BYTEH(u32Reg)
#define _MHal_R1BL( u32Reg )            X1BYTEL(u32Reg)
#define _MHal_W1BH( u32Reg, u08Val )    (X1BYTEH(u32Reg) = u08Val)
#define _MHal_W1BL( u32Reg, u08Val )    (X1BYTEL(u32Reg) = u08Val)
#define _MHal_R2B( u32Reg )             X2BYTE(u32Reg)
#define _MHal_W2B( u32Reg, u16Val )     (X2BYTE(u32Reg) = (u16Val))

#define _MHal_W1BHM( u32Reg, u08Val, u08Mask )    \
    (X1BYTEH(u32Reg) = (X1BYTEH(u32Reg) & ~(u08Mask)) | ((u08Val) & (u08Mask)))

#define _MHal_W1BLM( u32Reg, u08Val, u08Mask )    \
    (X1BYTEL(u32Reg) = (X1BYTEL(u32Reg) & ~(u08Mask)) | ((u08Val) & (u08Mask)))

#define _MHal_W1BHb( u32Reg, bBit, u08BitPos ) \
    (X1BYTEH(u32Reg) = (bBit) ? (X1BYTEH(u32Reg) | (u08BitPos)) : (X1BYTEH(u32Reg) & ~(u08BitPos)))

#define _MHal_W1BLb( u32Reg, bBit, u08BitPos ) \
    (X1BYTEL(u32Reg) = (bBit) ? (X1BYTEL(u32Reg) | (u08BitPos)) : (X1BYTEL(u32Reg) & ~(u08BitPos)))

#define _MHal_R1BHb( u32Reg, u08BitPos ) \
    (X1BYTEH(u32Reg) & (u08BitPos))

#define _MHal_R1BLb( u32Reg, u08BitPos ) \
    (X1BYTEL(u32Reg) & (u08BitPos))

#define _MHal_W2BM( u32Reg, u08Val, u08Mask )    \
    (X2BYTE(u32Reg) = (X2BYTE(u32Reg) & ~(u08Mask)) | ((u08Val) & (u08Mask)))

#define _MHal_W2Bb( u32Reg, bBit, u08BitPos ) \
    (X2BYTE(u32Reg) = (bBit) ? (X2BYTE(u32Reg) | (u08BitPos)) : (X2BYTE(u32Reg) & ~(u08BitPos)))

#define _MHal_R2Bb( u32Reg, u08BitPos ) \
    (X2BYTE(u32Reg) & (u08BitPos))

#define BDMA_BASE_REG_CH0       0x0480
#define BDMA_BASE_REG_CH1       0x0490
#define BDMA_CH0(x)                              WORD2REAL(BDMA_BASE_REG_CH0+(x))
#define BDMA_CH1(x)                              WORD2REAL(BDMA_BASE_REG_CH1+(x))


typedef enum _BDMA_Result
{
    E_BDMA_NOT_SUPPORT = -1
    ,E_BDMA_FAIL = 0
    ,E_BDMA_OK = 1
    ,E_BDMA_TIMEOUT
    ,E_BDMA_QUEUE_FULL
    ,E_BDMA_BUSY
}BDMA_Result;

// ONLY FLASH TO MIU0
BDMA_Result _BDMA_START(unsigned int src, unsigned int dst, unsigned int len)
{
    unsigned int count;


    if((_MHal_R2B(BDMA_CH0(0x01)) & (BIT0|BIT1)) != 0)
    {
    	printk("BDMA BUSY\n");
        return E_BDMA_BUSY;
    }

    // Stop DMA
    _MHal_W2BM(BDMA_CH0(0x00), BIT4, BIT4);
    _MHal_W2BM(BDMA_CH0(0x00), 0, BIT4);

    // Clear Status
    _MHal_W2BM(BDMA_CH0(0x01), (BIT2|BIT3|BIT4), (BIT2|BIT3|BIT4));

    // Set start address
    _MHal_W2B(BDMA_CH0(0x04), src&0xFFFF);
    _MHal_W2B(BDMA_CH0(0x05), (src>>16));

    _MHal_W2B(BDMA_CH0(0x06), dst&0xFFFF);
    _MHal_W2B(BDMA_CH0(0x07), (dst>>16));

    // Set length
    _MHal_W2B(BDMA_CH0(0x08), len&0x0000FFFF);
    _MHal_W2B(BDMA_CH0(0x09), (len>>16)&0x0FFF);

    // Set source dev to SPI, data width is 16 bytes
    _MHal_W2BM(BDMA_CH0(0x02), (0x05|BIT6), 0x00FF);

    // Set destination dev to MIU0, data width is 16 bytes
    _MHal_W2BM(BDMA_CH0(0x02), ((0x0|BIT6)<<8), 0xFF00);

    // set command
//    _MHal_W2B(BDMA_CH0(0x0A), dst&0x00FF);
//    _MHal_W2B(BDMA_CH0(0x0B), (dst>>16));

    _MHal_W2B(BDMA_CH0(0x0A), 0);
    _MHal_W2B(BDMA_CH0(0x0B), 0);

    _MHal_W2B(BDMA_CH0(0x0C), 0);
    _MHal_W2B(BDMA_CH0(0x0D), 0);

    // Disable interrupt
    _MHal_W2BM(BDMA_CH0(0x03), 0, BIT1);
    // Set address direction
    _MHal_W2BM(BDMA_CH0(0x03), 0, BIT0);
    // Set crc reflection
    _MHal_W2BM(BDMA_CH0(0x03), 0, 0xF0);
    // Set DmyWrCnt
    _MHal_W2BM(BDMA_CH0(0x03), 0, 0xF00);

    if((_MHal_R2B(BDMA_CH0(0x01)) & (BIT0|BIT1)) != 0)
        return E_BDMA_BUSY;

    // Trigger DMA
    _MHal_W2B(BDMA_CH0(0x00), 0x01);

    count = jiffies;
    while(1)
    {
        if((_MHal_R2B(BDMA_CH0(0x01)) & (BIT3)) != 0)
            break;
        if(jiffies-count>1000)
        {
			printk("TIME OUT\n");
            return E_BDMA_TIMEOUT;
        }
    }

    return E_BDMA_OK;
}

EXPORT_SYMBOL(_BDMA_START);


#ifdef __cplusplus
}
#endif


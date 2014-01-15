////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_gpio_io.h
/// @brief  GPIO Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <asm/types.h>
#include "mdrv_types.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct GPIO_Reg
{
    U32 u32Reg;
    U8 u8Enable;
    U8 u8BitMsk;
} __attribute__ ((packed));

typedef struct GPIO_Reg GPIO_Reg_t;

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define GPIO_IOC_MAGIC               'g'

#define MDRV_GPIO_INIT               _IO(GPIO_IOC_MAGIC, 0)
#define MDRV_GPIO_SET                _IOW(GPIO_IOC_MAGIC, 1, U8)
#define MDRV_GPIO_OEN                _IOW(GPIO_IOC_MAGIC, 2, U8)
#define MDRV_GPIO_ODN                _IOW(GPIO_IOC_MAGIC, 3, U8)
#define MDRV_GPIO_READ               _IOWR(GPIO_IOC_MAGIC, 4, U8)
#define MDRV_GPIO_PULL_HIGH          _IOW(GPIO_IOC_MAGIC, 5, U8)
#define MDRV_GPIO_PULL_LOW           _IOW(GPIO_IOC_MAGIC, 6, U8)
#define MDRV_GPIO_INOUT              _IOWR(GPIO_IOC_MAGIC, 7, U8)
#define MDRV_GPIO_WREGB              _IOW(GPIO_IOC_MAGIC, 8, GPIO_Reg_t)

#define GPIO_IOC_MAXNR               9

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

void __mod_gpio_init(void);


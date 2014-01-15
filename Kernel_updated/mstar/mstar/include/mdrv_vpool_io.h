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
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_mpool_io.h
/// @brief  Memory Pool  Driver IO Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_VPOOL_IO_H_
#define _MDRV_VPOOL_IO_H_

/* Use 'M' as magic number */
#define VPOOL_IOC_MAGIC                'V'

#define VPOOL_IOC_FLUSH_INV_DCACHE    _IOR(VPOOL_IOC_MAGIC,0x01,DrvVPool_Info_t)

typedef struct
{
    void*                         pAddr;
    unsigned int                u32Size;
} DrvVPool_Info_t;


#endif


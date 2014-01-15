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

#ifndef _MDRV_MPOOL_IO_H_
#define _MDRV_MPOOL_IO_H_

/* Use 'M' as magic number */
#define MPOOL_IOC_MAGIC                'M'

#define MPOOL_IOC_INFO           _IOWR(MPOOL_IOC_MAGIC, 0x00, DrvMPool_Info_t)
#define MPOOL_IOC_FLUSHDCACHE    _IOR(MPOOL_IOC_MAGIC,0x01,DrvMPool_Info_t)
#define MPOOL_IOC_GET_BLOCK_OFFSET    _IOR(MPOOL_IOC_MAGIC,0x02,unsigned int)
#define MPOOL_IOC_SET_MAP_CACHE _IOR(MPOOL_IOC_MAGIC,0x03,unsigned int)
#define MPOOL_IOC_TEST           _IOWR(MPOOL_IOC_MAGIC, 0x06, De_Sys_Info_t)
#define MPOOL_IOC_SET _IOR(MPOOL_IOC_MAGIC,0x04,DrvMPool_Info_t)
#define MPOOL_IOC_KERNEL_DETECT _IOWR(MPOOL_IOC_MAGIC,0x05,DrvMPool_Info_t)
#define MPOOL_IOC_FLUSHDCACHE_PAVA    _IOR(MPOOL_IOC_MAGIC,0x07,DrvMPool_Flush_Info_t)
#define MPOOL_IOC_VERSION _IOWR(MPOOL_IOC_MAGIC,0x08,unsigned int)
#define MPOOL_IOC_FLUSHDCACHE_ALL  _IOR(MPOOL_IOC_MAGIC,0x09,DrvMPool_Flush_Info_t)

#endif


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
/// @file   mdrv_miomap_io.h
/// @brief  Memory IO Map Driver IO Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MIOMAP_IO_H_
#define _MDRV_MIOMAP_IO_H_

/* Use 'M' as magic number */
#define MIOMAP_IOC_MAGIC                'I'

#define MIOMAP_IOC_CHIPINFO         _IOWR(MIOMAP_IOC_MAGIC, 0x00, DrvMIOMap_ChipInfo_t)
#define MIOMAP_IOC_SET_MAP          _IOW (MIOMAP_IOC_MAGIC, 0x01, DrvMIOMap_Info_t)
#define MIOMAP_IOC_FLUSHDCACHE      _IOR (MIOMAP_IOC_MAGIC, 0x02, DrvMIOMap_Info_t)
#define MIOMAP_IOC_GET_BLOCK_OFFSET _IOR (MIOMAP_IOC_MAGIC, 0x03, unsigned int)

#endif


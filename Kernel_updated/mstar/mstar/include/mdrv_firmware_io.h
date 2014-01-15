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
/// @file   mdrv_firmware_io.h
/// @brief  Firmware Driver Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_FW_IO_H_
#define _MDRV_FW_IO_H_

/* Use 'M' as magic number */
#define FW_IOC_MAGIC                    'M'

#define FW_IOC_TSP_ADDR                 _IOR (FW_IOC_MAGIC, 0x00, unsigned int)
#define FW_IOC_TSP_SIZE                 _IOR (FW_IOC_MAGIC, 0x01, unsigned int)

#endif


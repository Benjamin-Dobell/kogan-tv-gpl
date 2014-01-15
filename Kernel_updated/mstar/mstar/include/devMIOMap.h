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
/// @file   drvMIOMap.h
/// @brief  MIOMAP Driver Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_MIOMAP_H_
#define _DRV_MIOMAP_H_


//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
/* Use 'I' as magic number */
#define MIOMAP_IOC_MAGIC                'I'

#define MIOMAP_IOC_INFO             _IOWR(MIOMAP_IOC_MAGIC, 0x00, DrvMIOMap_Info_t)
#define MIOMAP_IOC_SET_MAP          _IOW (MIOMAP_IOC_MAGIC, 0x01, DrvMIOMap_Info_t)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// MIOMAP_IOC_INFO 
typedef struct
{
    unsigned int                u32Addr;
    unsigned int                u32Size;
} DrvMIOMap_Info_t;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------


#endif // _DRV_MIOMAP_H_

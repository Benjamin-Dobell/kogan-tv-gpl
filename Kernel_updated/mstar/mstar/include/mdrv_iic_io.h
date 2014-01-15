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
/// @file   mdrv_iic.h
/// @brief  IIC Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <asm/types.h>//<asm-mips/types.h>
#include "mdrv_types.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

struct IIC_Param
{
    U8 u8IdIIC;      	/// IIC ID: Channel 1~7
    U8 u8ClockIIC;   	/// IIC clock speed
    U8 u8SlaveIdIIC;    /// Device slave ID
    U8 u8AddrSizeIIC;	/// Address length in bytes
    U8 u8AddrIIC[4];	/// Starting address inside the device
    U8 *u8pbufIIC;     	/// buffer
    U32 u32DataSizeIIC;	/// size of buffer

} __attribute__ ((packed));

typedef struct IIC_Param IIC_Param_t;

struct IIC_BusCfg
{
    U8 u8ChIdx;         ///Channel index
    U16 u16PadSCL;      ///Pad(Gpio) number for SCL
    U16 u16PadSDA;      ///Pad(Gpio) number for SDA
    U16 u16SpeedKHz;    ///Speed in KHz
    U8 u8Enable;        ///Enable
} __attribute__ ((packed));

typedef struct IIC_BusCfg IIC_BusCfg_t;

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define IIC_IOC_MAGIC               'u'

#define MDRV_IIC_INIT               _IO(IIC_IOC_MAGIC, 0)
#define MDRV_IIC_SET_PARAM          _IOW(IIC_IOC_MAGIC, 1, IIC_Param_t)
#define MDRV_IIC_CLOCK              _IOW(IIC_IOC_MAGIC, 2, IIC_Param_t)
#define MDRV_IIC_ENABLE             _IOW(IIC_IOC_MAGIC, 3, IIC_Param_t) 
#define MDRV_IIC_BUSCFG             _IOW(IIC_IOC_MAGIC, 4, IIC_BusCfg_t)
#define IIC_IOC_MAXNR               5

#define IIC_RW_BUF_SIZE             1024
#define IIC_WR_BUF_SIZE             128		// added for RGB EDID
#define IIC_RD_BUF_SIZE             256		// added for RGB EDID

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------


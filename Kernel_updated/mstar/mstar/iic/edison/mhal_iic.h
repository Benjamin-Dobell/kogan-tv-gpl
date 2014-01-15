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

#ifndef _HAL_IIC_H_
#define _HAL_IIC_H_

#include <asm/types.h>
#include "mdrv_types.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
extern void MHal_IIC_Init(void);
extern void MHal_IIC_Clock_Select(U8 u8ClockIIC);
extern void MHal_IIC_Start(void);
extern void MHal_IIC_Stop(void);
extern void MHal_IIC_NoAck(void);
extern B16 MHal_IIC_SendData(U8 u8DataIIC);
extern B16 MHal_IIC_SendByte(U8 u8DataIIC);
extern B16 MHal_IIC_GetByte(U8* pu8DataIIC);
extern U32 MHal_Query_MIPS_CLK(void);

#endif // _HAL_IIC_H_


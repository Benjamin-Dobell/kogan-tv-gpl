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

#ifndef __MHAL_ONENAND_H__
#define __MHAL_ONENAND_H__

#include "mdrv_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DEBUG_PAR_FLASH(_dbg_lv, x)     do { if (_u8ParFlashDbgLv >= (_dbg_lv)) (x); } while(0)


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
extern void HAL_ParFlash_Read(U32 u32addr, U16 *u16Data);
extern void HAL_ParFlash_Reads(U32 u32Addr, U8 *pu8Data, U32 u32Size);
extern void HAL_ParFlash_Write(U32 u32Addr, U16 u16Data);
extern void HAL_ParFlash_Writes(U32 u32Addr, const U8 *pu8Data, U32 u32Size);
extern void HAL_ParFlash_Init(void);


#ifdef __cplusplus
}
#endif

#endif // __MHAL_ONENAND_H__

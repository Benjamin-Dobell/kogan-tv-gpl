///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
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
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_gflip_ve_st.h
// @brief  GFlip KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MDRV_GFLIP_VE_ST_H
#define _MDRV_GFLIP_VE_ST_H

//=============================================================================
// Macros
//=============================================================================
#define VE_VEC_CONFIG_LENGTH_1STVERSION     16 //DO NOT CHANGE THIS, it is the structure length of the first version
#define VE_VEC_CONFIG_VERSION               0 //Version number for compatibility with kernel and VE driver

//=============================================================================
// Type and Structure Declaration
//=============================================================================
typedef enum
{
    MS_VEC_ISR_GOP_OP,
    MS_VEC_ISR_VE,
    MS_VEC_ISR_MAXNUM,
} MS_VEC_ISR_TYPE;

typedef enum
{
    MS_VEC_CONFIG_INIT,
    MS_VEC_CONFIG_ENABLE,
} MS_VEC_CONFIG_TYPE;

//IO Ctrl struct defines:
typedef struct
{
    MS_BOOL bEnable;      //< InOut, VE capture enable state
    MS_U8   u8FrameCount; //< Out, Current captured frame number,value range: 0~3
    MS_U8   u8Result;     //< Out, Function return status
}MS_GFLIP_VEC_STATE, *PMS_GFLIP_VEC_STATE;

typedef struct
{
    MS_U16  u16Version;   //< Version number for this structure
    MS_U16  u16Length;    //< Length of this structure, unit: byte
    MS_U8   u8Result;     //< Out, Function return status
    MS_BOOL bInterlace;   //< If the VEC capture source is interlace
    MS_U8   u8MaxFrameNumber_P;     //< The max frame number for progressive capture. This setting should obey with VE driver
    MS_U8   u8MaxFrameNumber_I;     //< The max frame number for Interlace capture. This setting should obey with VE driver
    MS_VEC_ISR_TYPE     eIsrType;   //< The ISR type of VEC capture
    MS_VEC_CONFIG_TYPE eConfigType; //< The ISR type of VEC capture
}MS_GFLIP_VEC_CONFIG, *PMS_GFLIP_VEC_CONFIG;

#endif //_MDRV_GFLIP_VE_ST_H

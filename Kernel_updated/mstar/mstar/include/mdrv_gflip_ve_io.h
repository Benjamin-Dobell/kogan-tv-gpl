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
// @file   mdrv_gflip_ve_io.h
// @brief  GFlip KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_GFLIP_VE_IO_H
#define _MDRV_GFLIP_VE_IO_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_gflip_ve_st.h"

//=============================================================================
// Defines
//=============================================================================
//IO Ctrl defines:
#define MDRV_GFLIP_VEC_IOC_CONFIG_NR        0
#define MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE_NR         MDRV_GFLIP_VEC_IOC_CONFIG_NR+1
#define MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE_NR       MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE_NR+1
#define MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME_NR    MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE_NR+1
#define MDRV_GFLIP_VEC_IOC_MAX_NR                     MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME_NR+1

// use 'v' as magic number for ve driver
#define MDRV_GFLIP_VEC_IOC_MAGIC         'v'
#define MDRV_GFLIP_VEC_IOC_CONFIG                     _IOR (MDRV_GFLIP_VEC_IOC_MAGIC,  MDRV_GFLIP_VEC_IOC_CONFIG_NR, MS_GFLIP_VEC_CONFIG)
#define MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE            _IOWR(MDRV_GFLIP_VEC_IOC_MAGIC,  MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE_NR, MS_GFLIP_VEC_STATE)
#define MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE          _IOWR(MDRV_GFLIP_VEC_IOC_MAGIC,  MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE_NR, MS_GFLIP_VEC_STATE)
#define MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME       _IOWR(MDRV_GFLIP_VEC_IOC_MAGIC,  MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME_NR, MS_GFLIP_VEC_STATE)
#endif //_MDRV_GFLIP_VE_IO_H

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
// @file   mdrv_gflip_io.h
// @brief  GFlip KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_GFLIP_IO_H
#define _MDRV_GFLIP_IO_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_gflip_st.h"

//=============================================================================
// Defines
//=============================================================================
//IO Ctrl defines:
#define MDRV_GFLIP_IOC_INIT_NR        (0)
#define MDRV_GFLIP_IOC_DEINIT_NR      (MDRV_GFLIP_IOC_INIT_NR+1)
#define MDRV_GFLIP_IOC_SETFLIPINFO_NR   (MDRV_GFLIP_IOC_DEINIT_NR+1)
#define MDRV_GFLIP_IOC_SETINPUTSIGSTATUS_NR    (MDRV_GFLIP_IOC_SETFLIPINFO_NR+1)
#define MDRV_GFLIP_IOC_GETDWININTINFO_NR    (MDRV_GFLIP_IOC_SETINPUTSIGSTATUS_NR+1)
#define MDRV_GFLIP_IOC_GETDWININTINFO2_NR   (MDRV_GFLIP_IOC_GETDWININTINFO_NR + 1)
#define MDRV_GFLIP_IOC_SETPIXELIDADDR_NR    (MDRV_GFLIP_IOC_GETDWININTINFO2_NR+1)
#define MDRV_GFLIP_IOC_SETGPIO3DPIN_NR  (MDRV_GFLIP_IOC_SETPIXELIDADDR_NR+1)
#define MDRV_GFLIP_IOC_ENABLEVECAPTURE_NR      (MDRV_GFLIP_IOC_SETGPIO3DPIN_NR+1)
#define MDRV_GFLIP_IOC_GETVECAPTURESTATE_NR    (MDRV_GFLIP_IOC_ENABLEVECAPTURE_NR+1)
#define MDRV_GFLIP_IOC_VECAPTUREWAITONFRAME_NR    (MDRV_GFLIP_IOC_GETVECAPTURESTATE_NR+1)
#define MDRV_GFLIP_IOC_CLEARFLIPQUEUE_NR     (MDRV_GFLIP_IOC_VECAPTUREWAITONFRAME_NR+1)
#define MDRV_GFLIP_IOC_SETGWININFO_NR       (MDRV_GFLIP_IOC_CLEARFLIPQUEUE_NR+1)
#define MDRV_GFLIP_IOC_DLCCHANGECURVE_NR     (MDRV_GFLIP_IOC_SETGWININFO_NR+1)
#define MDRV_GFLIP_IOC_DLCONOFFINFO_NR       (MDRV_GFLIP_IOC_DLCCHANGECURVE_NR+1)       //14
#define MDRV_GFLIP_IOC_SET3DFLIPINFO_NR       (MDRV_GFLIP_IOC_DLCONOFFINFO_NR+1)       //15
#define MDRV_GFLIP_IOC_BLECHANGEPOINT_NR      (MDRV_GFLIP_IOC_SET3DFLIPINFO_NR+1)       //16
#define MDRV_GFLIP_IOC_BLEONOFFINFO_NR        (MDRV_GFLIP_IOC_BLECHANGEPOINT_NR+1)       //17
#define MDRV_GFLIP_IOC_DLCGETHISTOGRAMINFO_NR  (MDRV_GFLIP_IOC_BLEONOFFINFO_NR+1)       //18

#define MDRV_GFLIP_IOC_MAX_NR              (MDRV_GFLIP_IOC_DLCGETHISTOGRAMINFO_NR+1)

// use 'm' as magic number
#define MDRV_GFLIP_IOC_MAGIC      ('2')
#define MDRV_GFLIP_IOC_INIT                     _IOR(MDRV_GFLIP_IOC_MAGIC,   MDRV_GFLIP_IOC_INIT_NR, MS_U32)
//#define MDRV_GFLIP_IOC_DEINIT     _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_DEINIT_NR, MS_U32)
#define MDRV_GFLIP_IOC_SETFLIPINFO       _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_SETFLIPINFO_NR, MS_GFLIP_INFO)
#define MDRV_GFLIP_IOC_SETINPUTSIGSTATUS    _IOR(MDRV_GFLIP_IOC_MAGIC,   MDRV_GFLIP_IOC_SETINPUTSIGSTATUS_NR, MS_BOOL)
#define MDRV_GFLIP_IOC_GETDWININTINFO       _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_GETDWININTINFO_NR, MS_GFLIP_DWININT_INFO)
#define MDRV_GFLIP_IOC_GETDWININTINFO2      _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_GETDWININTINFO2_NR, MS_GFLIP_DWININT_INFO2)
#define MDRV_GFLIP_IOC_SETPIXELIDADDR       _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_SETPIXELIDADDR_NR, MS_GFLIP_INFO)
#define MDRV_GFLIP_IOC_SETGPIO3DPIN         _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_SETGPIO3DPIN_NR, MS_GFLIP_INFO)
#define MDRV_GFLIP_IOC_ENABLEVECAPTURE      _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_ENABLEVECAPTURE_NR, MS_GFLIP_VECAPTURESTATE)
#define MDRV_GFLIP_IOC_GETVECAPTURESTATE    _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_GETVECAPTURESTATE_NR, MS_GFLIP_VECAPTURESTATE)
#define MDRV_GFLIP_IOC_VECAPTUREWAITONFRAME     _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_VECAPTUREWAITONFRAME_NR, MS_GFLIP_VECAPTURESTATE)
#define MDRV_GFLIP_IOC_CLEARFLIPQUEUE       _IOWR(MDRV_GFLIP_IOC_MAGIC,MDRV_GFLIP_IOC_CLEARFLIPQUEUE_NR,MS_GFLIP_GOPGWINIDX)
#define MDRV_GFLIP_IOC_SETGWININFO          _IOWR(MDRV_GFLIP_IOC_MAGIC,MDRV_GFLIP_IOC_SETGWININFO_NR,MS_GWIN_INFO)
#define MDRV_GFLIP_IOC_DLCCHANGECURVE       _IOWR(MDRV_GFLIP_IOC_MAGIC,MDRV_GFLIP_IOC_DLCCHANGECURVE_NR,MS_DLC_INFO)
#define MDRV_GFLIP_IOC_DLCONOFFINFO         _IOWR(MDRV_GFLIP_IOC_MAGIC,MDRV_GFLIP_IOC_DLCONOFFINFO_NR,MS_BOOL)
#define MDRV_GFLIP_IOC_SET3DFLIPINFO        _IOWR(MDRV_GFLIP_IOC_MAGIC, MDRV_GFLIP_IOC_SET3DFLIPINFO_NR, MS_GFLIP_3DINFO)
#define MDRV_GFLIP_IOC_BLECHANGECURVE       _IOWR(MDRV_GFLIP_IOC_MAGIC,MDRV_GFLIP_IOC_BLECHANGEPOINT_NR,MS_BLE_INFO)
#define MDRV_GFLIP_IOC_BLEONOFFINFO          _IOWR(MDRV_GFLIP_IOC_MAGIC,MDRV_GFLIP_IOC_BLEONOFFINFO_NR,MS_BOOL)
#define MDRV_GFLIP_IOC_DLCGETHISTOGRAMINFO     _IOWR(MDRV_GFLIP_IOC_MAGIC,MDRV_GFLIP_IOC_DLCGETHISTOGRAMINFO_NR,MS_U16)

#endif //_MDRV_GFLIP_IO_H

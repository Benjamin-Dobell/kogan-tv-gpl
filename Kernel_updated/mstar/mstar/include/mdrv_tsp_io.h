///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2010 MStar Semiconductor, Inc.
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
// @file   mdrv_tsp_io.h
// @brief  GFlip KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_TSP_IO_H
#define _MDRV_TSP_IO_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_tsp_st.h"

//=============================================================================
// Defines
//=============================================================================
//IO Ctrl defines:
#define MDRV_TSP_IOC_INIT_NR            0
#define MDRV_TSP_IOC_GETEVENTINFO_NR    MDRV_TSP_IOC_INIT_NR+1
#define MDRV_TSP_IOC_CLEAREVENTINFO_NR  MDRV_TSP_IOC_GETEVENTINFO_NR+1

#define MDRV_TSP_IOC_MAX_NR             MDRV_TSP_IOC_CLEAREVENTINFO_NR+1

// use 'T' as magic number
#define MDRV_TSP_IOC_MAGIC              'T'
#define MDRV_TSP_IOC_INIT               _IOR(MDRV_TSP_IOC_MAGIC,  MDRV_TSP_IOC_INIT_NR, MS_U32)
#define MDRV_TSP_IOC_GETEVENTINFO       _IOWR(MDRV_TSP_IOC_MAGIC, MDRV_TSP_IOC_GETEVENTINFO_NR, MS_TSP_EVENT_INFO)
#define MDRV_TSP_IOC_CLEAREVENTINFO     _IOWR(MDRV_TSP_IOC_MAGIC, MDRV_TSP_IOC_CLEAREVENTINFO_NR, MS_U32)

#endif //_MDRV_TSP_IO_H

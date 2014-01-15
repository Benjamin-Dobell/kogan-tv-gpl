////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
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

// PUT THIS FILE under INLCUDE path

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_temp_io.h
/// @brief  TEMP Driver Interface.
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_SEMUTEX_IO_H_
#define _MDRV_SEMUTEX_IO_H_

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define SEMUTEX_IOC_MAGIC                'S'
#define MUTEX_INDEX_BEGIN                 0xFF

/* IOCTL functions. */
/* int ioctl(int fd,
             int request == MDRV_SEMUTEX_LOCK);
*/
#define MDRV_SEMUTEX_LOCK                (int)_IO(SEMUTEX_IOC_MAGIC, 0U)

/* int ioctl(int fd,
             int request == MDRV_SC_DETACH_INTERRUPT);
*/
#define MDRV_SEMUTEX_UNLOCK                (int)_IO(SEMUTEX_IOC_MAGIC, 1U)

/* int ioctl(int fd,
             int request == MDRV_SC_RESET_FIFO);
*/
#define MDRV_SEMUTEX_CREATE_SHAREMEMORY                      (int)_IO(SEMUTEX_IOC_MAGIC, 2U)

/* int ioctl(int fd,
             int request == MDRV_SC_GET_EVENTS,
             int *events);
*/
#define MDRV_SEMUTEX_MAP_SHAREMEMORY                      (int)_IOR(SEMUTEX_IOC_MAGIC, 3U, int)

#define MDRV_SEMUTEX_UNMAP_SHAREMEMORY                      (int)_IOR(SEMUTEX_IOC_MAGIC, 4U, int)

#define MDRV_SEMUTEX_CREATE_MUTEX                      _IOR(SEMUTEX_IOC_MAGIC, 5U, int)

#define MDRV_SEMUTEX_DEL_MUTEX                      _IOR(SEMUTEX_IOC_MAGIC, 6U, int)

#define MDRV_SEMUTEX_TRY_LOCK                     _IOR(SEMUTEX_IOC_MAGIC, 7U, int)

#define MDRV_SEMUTEX_QUERY_ISFIRST_SHAREMEMORY                      (int)_IO(SEMUTEX_IOC_MAGIC, 8U)

#define MDRV_SEMUTEX_LOCK_WITHTIMEOUT                      _IOR(SEMUTEX_IOC_MAGIC, 9U, int)

#define MDRV_SEMUTEX_EXPAND_SHAREMEMORY                      _IOR(SEMUTEX_IOC_MAGIC, 10U, int)


typedef struct{
int index;
int time;
}LOCKARG;

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#endif // SEMUTEX


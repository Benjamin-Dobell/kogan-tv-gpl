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

#ifndef _MDRV_SC_IO_H_
#define _MDRV_SC_IO_H_

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define SC_IOC_MAGIC                's'

/* IOCTL functions. */
/* int ioctl(int fd,
             int request == MDRV_SC_ATTACH_INTERRUPT);
*/
#define MDRV_SC_ATTACH_INTERRUPT                (int)_IO(SC_IOC_MAGIC, 0U)

/* int ioctl(int fd,
             int request == MDRV_SC_DETACH_INTERRUPT);
*/
#define MDRV_SC_DETACH_INTERRUPT                (int)_IO(SC_IOC_MAGIC, 1U)

/* int ioctl(int fd,
             int request == MDRV_SC_RESET_FIFO);
*/
#define MDRV_SC_RESET_FIFO                      (int)_IO(SC_IOC_MAGIC, 2U)

/* int ioctl(int fd,
             int request == MDRV_SC_GET_EVENTS,
             int *events);
*/
#define MDRV_SC_GET_EVENTS                      (int)_IOR(SC_IOC_MAGIC, 3U, int)


#define SC_IOC_MAXNR                            3

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#endif // _MDRV_TEMP_IO_H_


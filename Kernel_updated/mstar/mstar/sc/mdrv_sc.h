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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_temp.h
/// @brief  TEMP Driver Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_SC_H_
#define _MDRV_SC_H_

#include <linux/fs.h>
#include <linux/cdev.h>
#include "mdrv_types.h"
#include <linux/version.h>

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define SC_FIFO_SIZE                512                                 // Rx fifo size

// #define SC_DEBUG
#ifdef SC_DEBUG
#define SC_PRINT(_fmt, _args...)    printk(KERN_WARNING "[%s][%d] " _fmt, __FUNCTION__, __LINE__, ## _args)
#define SC_ASSERT(_con) \
    do { \
        if (!(_con)) { \
            printk(KERN_CRIT "BUG at %s:%d assert(%s)\n", \
                    __FILE__, __LINE__, #_con); \
            BUG(); \
        } \
    } while (0)
#else
#define SC_PRINT(fmt, args...)
#define SC_ASSERT(arg)
#endif


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    int                         s32Major;
    int                         s32Minor;
    struct cdev                 stCDev;
    struct file_operations      fops;
    struct fasync_struct        *async_queue; /* asynchronous readers */
} SC_DEV;

/// SmartCard Info
typedef struct
{
    BOOL                        bCardIn;                            ///Status care in
    BOOL                        bLastCardIn;
    U8                          u8CardStatus;
    wait_queue_head_t           stWaitQue;

    U8                          u8FifoRx[SC_FIFO_SIZE];
    U16                         u16FifoRxRead;
    U16                         u16FifoRxWrite;

    U8                          u8FifoTx[SC_FIFO_SIZE];
    U16                         u16FifoTxRead;
    U16                         u16FifoTxWrite;
} SC_Info;


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int MDrv_SC_Open(struct inode *inode, struct file *filp);
ssize_t MDrv_SC_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t MDrv_SC_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
unsigned int MDrv_SC_Poll(struct file *filp, poll_table *wait);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_AttachInterrupt(struct file *filp, unsigned long arg);
int MDrv_SC_DetachInterrupt(struct file *filp, unsigned long arg);
int MDrv_SC_ResetFIFO(struct file *filp, unsigned long arg);
int MDrv_SC_GetEvent(struct file *filp, unsigned long arg);
#else
int MDrv_SC_AttachInterrupt(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_DetachInterrupt(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_ResetFIFO(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_GetEvent(struct inode *inode, struct file *filp, unsigned long arg);
#endif


#endif // _MDRV_TEMP_H_


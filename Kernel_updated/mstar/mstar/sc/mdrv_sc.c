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
/// file    mdrv_temp.c
/// @brief  TEMP Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/version.h>

//drver header files
#include "chip_int.h"
#include "mdrv_mstypes.h"
#include "reg_sc.h"
#include "mhal_sc.h"
#include "mdrv_sc.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static SC_Info  _scInfo[SC_DEV_NUM] = {
    {
        .bCardIn        = FALSE,
        .bLastCardIn    = FALSE,
        .u8CardStatus   = 0,
        .u16FifoRxRead  = 0,
        .u16FifoRxWrite = 0,
        .u16FifoTxRead  = 0,
        .u16FifoTxWrite = 0,
    },
#if (SC_DEV_NUM > 1) // no more than 2
    {
        .bCardIn        = FALSE,
        .bLastCardIn    = FALSE,
        .u8CardStatus   = 0,
        .u16FifoRxRead  = 0,
        .u16FifoRxWrite = 0,
        .u16FifoTxRead  = 0,
        .u16FifoTxWrite = 0,
    }
#endif
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

BOOL MDrv_SC_ISR_Proc(U8 u8SCID)
{
    U8  u8Reg;
    U32 cnt;
    U32 idx;
    BOOL bWakeUp = FALSE;

    u8Reg = SC_READ(u8SCID, UART_IIR);
    if (!(u8Reg & UART_IIR_NO_INT))
    {
        u8Reg = SC_READ(u8SCID, UART_LSR);
        while (u8Reg & (UART_LSR_DR | UART_LSR_BI))
        {
            bWakeUp = TRUE;
            _scInfo[u8SCID].u8FifoRx[_scInfo[u8SCID].u16FifoRxWrite] = SC_READ(u8SCID, UART_RX);

            idx = _scInfo[u8SCID].u16FifoRxWrite + 1;
            if ((idx == SC_FIFO_SIZE) && (_scInfo[u8SCID].u16FifoRxRead != 0))
            {
                // Not overflow but wrap
                _scInfo[u8SCID].u16FifoRxWrite = 0;
            }
            else if (idx != _scInfo[u8SCID].u16FifoRxRead)
            {
                // Not overflow
                _scInfo[u8SCID].u16FifoRxWrite = idx;
            }
            else
            {
                // overflow
                printk("[%s][%d] RX buffer Overflow\n", __FUNCTION__, __LINE__);
                break;
            }

            u8Reg = SC_READ(u8SCID, UART_LSR);
        }

        if (u8Reg & UART_LSR_THRE)
        {
            cnt = 16;
            do
            {
                if (_scInfo[u8SCID].u16FifoTxRead == _scInfo[u8SCID].u16FifoTxWrite)
                    break;

                bWakeUp = TRUE;
                SC_WRITE(u8SCID, UART_TX, _scInfo[u8SCID].u8FifoTx[_scInfo[u8SCID].u16FifoTxRead++]);
                if (_scInfo[u8SCID].u16FifoTxRead == SC_FIFO_SIZE)
                {
                    _scInfo[u8SCID].u16FifoTxRead = 0;
                }

            } while (--cnt > 0);
        }
    }

    // Check special event from SMART
    u8Reg = SC_READ(u8SCID, UART_SCSR);
    if (u8Reg & (UART_SCSR_INT_CARDIN | UART_SCSR_INT_CARDOUT))
    {
        SC_WRITE(u8SCID, UART_SCSR, u8Reg); // clear interrupt
        _scInfo[u8SCID].u8CardStatus |= u8Reg & (UART_SCSR_INT_CARDIN | UART_SCSR_INT_CARDOUT);
        bWakeUp = TRUE;
    }

    if (bWakeUp)
    {
        wake_up_interruptible(&_scInfo[u8SCID].stWaitQue);
    }

    return TRUE; // handled
}

//-------------------------------------------------------------------------------------------------
/// Handle smart card Interrupt notification handler
/// @param  irq             \b IN: interrupt number
/// @param  devid           \b IN: device id
/// @return IRQ_HANDLED
/// @attention
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_SC_ISR1(int irq, void *devid)
{
    if (!MDrv_SC_ISR_Proc(0))
    {
        SC_PRINT("ISR proc is failed\n");
    }

    return IRQ_HANDLED;
}

//-------------------------------------------------------------------------------------------------
/// Handle smart card Interrupt notification handler
/// @param  irq             \b IN: interrupt number
/// @param  devid           \b IN: device id
/// @return IRQ_HANDLED
/// @attention
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_SC_ISR2(int irq, void *devid)
{
    if (!MDrv_SC_ISR_Proc(1))
    {
        SC_PRINT("ISR proc is failed\n");
    }

    return IRQ_HANDLED;
}

int MDrv_SC_Open(struct inode *inode, struct file *filp)
{
    U8 u8SCID = (U8)(int)filp->private_data;

    SC_PRINT("%s is invoked\n", __FUNCTION__);
    init_waitqueue_head(&_scInfo[u8SCID].stWaitQue);

    return 0;
}

ssize_t MDrv_SC_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    U8 u8SCID = (U8)(int)filp->private_data;
    ssize_t idx = 0;

    for (idx = 0; idx < count; idx++)
    {
        if (_scInfo[u8SCID].u16FifoRxWrite == _scInfo[u8SCID].u16FifoRxRead)
            break;

        buf[idx] = _scInfo[u8SCID].u8FifoRx[_scInfo[u8SCID].u16FifoRxRead++];
        if (_scInfo[u8SCID].u16FifoRxRead == SC_FIFO_SIZE)
        {
            _scInfo[u8SCID].u16FifoRxRead = 0;
        }
    }

    return idx;
}

ssize_t MDrv_SC_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    U8 u8SCID = (U8)(int)filp->private_data;
    ssize_t idx = 0;
    U32 tmp;

    for (idx = 0; idx < count; idx++)
    {
        _scInfo[u8SCID].u8FifoTx[_scInfo[u8SCID].u16FifoTxWrite] = buf[idx];

        tmp = _scInfo[u8SCID].u16FifoTxWrite + 1;
        if ((tmp == SC_FIFO_SIZE) && (_scInfo[u8SCID].u16FifoTxRead != 0))
        {
            // Not overflow but wrap
            _scInfo[u8SCID].u16FifoTxWrite = 0;
        }
        else if (tmp != _scInfo[u8SCID].u16FifoTxRead)
        {
            // Not overflow
            _scInfo[u8SCID].u16FifoTxWrite = tmp;
        }
        else
        {
            printk("[%s][%d] TX buffer Overflow\n", __FUNCTION__, __LINE__);
            break;
        }
    }

    if ((SC_READ(u8SCID, UART_LSR) & UART_LSR_THRE) &&
            (_scInfo[u8SCID].u16FifoTxRead != _scInfo[u8SCID].u16FifoTxWrite))
    {
        SC_WRITE(u8SCID, UART_TX, _scInfo[u8SCID].u8FifoTx[_scInfo[u8SCID].u16FifoTxRead++]);
        if (_scInfo[u8SCID].u16FifoTxRead == SC_FIFO_SIZE)
        {
            _scInfo[u8SCID].u16FifoTxRead = 0;
        }

    }

    return idx;
}

unsigned int MDrv_SC_Poll(struct file *filp, poll_table *wait)
{
    U8 u8SCID = (U8)(int)filp->private_data;
    unsigned int mask = 0;

    poll_wait(filp, &_scInfo[u8SCID].stWaitQue, wait);
    if (_scInfo[u8SCID].u16FifoRxRead != _scInfo[u8SCID].u16FifoRxWrite)
    {
        mask |= POLLIN;
    }
    if (_scInfo[u8SCID].u8CardStatus)
    {
        mask |= POLLPRI;
    }

    return mask;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_AttachInterrupt(struct file *filp, unsigned long arg)
#else
int MDrv_SC_AttachInterrupt(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    SC_PRINT("%s is invoked\n", __FUNCTION__);

    if (u8SCID == 0)
    {
        if (request_irq(SC_IRQ, MDrv_SC_ISR1, SA_INTERRUPT, "SC", NULL))
        {
            SC_PRINT("SC IRQ1 registartion ERROR\n");
        }
        else
        {
            SC_PRINT("SC IRQ1 registartion OK\n");
        }
    }
#if (SC_DEV_NUM > 1) // no more than 2
    else if (u8SCID == 1)
    {
        if (request_irq(SC_IRQ2, MDrv_SC_ISR2, SA_INTERRUPT, "SC", NULL))
        {
            SC_PRINT("SC IRQ2 registartion ERROR\n");
        }
        else
        {
            SC_PRINT("SC IRQ2 registartion OK\n");
        }
    }
#endif
    return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_DetachInterrupt(struct file *filp, unsigned long arg)
#else
int MDrv_SC_DetachInterrupt(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    SC_PRINT("%s is invoked\n", __FUNCTION__);
    if (u8SCID == 0)
    {
        free_irq(SC_IRQ, NULL);
    }
#if (SC_DEV_NUM > 1) // no more than 2
    else if (u8SCID == 1)
    {
        free_irq(SC_IRQ2, NULL);
    }
#endif

    return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_ResetFIFO(struct file *filp, unsigned long arg)
#else
int MDrv_SC_ResetFIFO(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    SC_PRINT("%s is invoked\n", __FUNCTION__);
    _scInfo[u8SCID].u16FifoRxRead   = 0;
    _scInfo[u8SCID].u16FifoRxWrite  = 0;
    _scInfo[u8SCID].u16FifoTxRead   = 0;
    _scInfo[u8SCID].u16FifoTxWrite  = 0;

    return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_GetEvent(struct file *filp, unsigned long arg)
#else
int MDrv_SC_GetEvent(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    put_user(_scInfo[u8SCID].u8CardStatus, (int __user *)arg);
    _scInfo[u8SCID].u8CardStatus = 0;

    return 0;
}

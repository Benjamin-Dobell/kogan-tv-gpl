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
/// file    mdrv_temp_io.c
/// @brief  TEMP Driver Interface for Export
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>

//drver header files
#include "mst_devid.h"
#include "mdrv_mstypes.h"
#include "mdrv_sc_io.h"
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

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                  _mod_sc_open (struct inode *inode, struct file *filp);
static int                  _mod_sc_release(struct inode *inode, struct file *filp);
static ssize_t              _mod_sc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t              _mod_sc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static unsigned int         _mod_sc_poll(struct file *filp, poll_table *wait);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long                 _mod_sc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int                  _mod_sc_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
//static int                  _mod_sc_fasync(int fd, struct file *filp, int mode);

SC_DEV devSC =
{
    .s32Major   =           MDRV_MAJOR_SMART,
    .s32Minor   =           MDRV_MINOR_SMART,
    .stCDev =
    {
        .kobj   =           {.name = MDRV_NAME_SMART, },
        .owner  =           THIS_MODULE,
    },
    .fops =
    {
        .open   =           _mod_sc_open,
        .release=           _mod_sc_release,
        .read   =           _mod_sc_read,
        .write  =           _mod_sc_write,
        .poll   =           _mod_sc_poll,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20)
        .unlocked_ioctl  =  _mod_sc_ioctl,
        #else
        .ioctl  =           _mod_sc_ioctl,
        #endif
        //.fasync =	        _mod_sc_fasync,
    },
};


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static int _mod_sc_open(struct inode *inode, struct file *filp)
{
    SC_PRINT("%s is invoked\n", __FUNCTION__);
    filp->private_data = (void*)(iminor(inode) - devSC.s32Minor);

    return MDrv_SC_Open(inode, filp);
}

static int _mod_sc_release(struct inode *inode, struct file *filp)
{
    SC_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static ssize_t _mod_sc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

    SC_PRINT("%s is invoked count=%d\n", __FUNCTION__, count);
    return MDrv_SC_Read(filp, buf, count, f_pos);
}

static ssize_t _mod_sc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    SC_PRINT("%s is invoked count=%d\n", __FUNCTION__, count);
    return MDrv_SC_Write(filp, buf, count, f_pos);
}

static unsigned int _mod_sc_poll(struct file *filp, poll_table *wait)
{
    // SC_PRINT("%s is invoked\n", __FUNCTION__);
    return MDrv_SC_Poll(filp, wait);
}

#if 0
static int _mod_sc_fasync(int fd, struct file *filp, int mode)
{
    SC_PRINT("%s is invoked\n", __FUNCTION__);
	return fasync_helper(fd, filp, mode, &devSC.async_queue);
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _mod_sc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _mod_sc_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int err= 0;

    SC_PRINT("%s is invoked\n", __FUNCTION__);
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if ((SC_IOC_MAGIC != _IOC_TYPE(cmd)) || (_IOC_NR(cmd) > SC_IOC_MAXNR))
    {
        return -ENOTTY;
    }

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (err)
    {
        return -EFAULT;
    }

    #if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
    switch(cmd)
    {
        case MDRV_SC_ATTACH_INTERRUPT:
            MDrv_SC_AttachInterrupt(filp, arg);
            break;
        case MDRV_SC_DETACH_INTERRUPT:
            MDrv_SC_DetachInterrupt(filp, arg);
            break;
        case MDRV_SC_RESET_FIFO:
            MDrv_SC_ResetFIFO(filp, arg);
            break;
        case MDRV_SC_GET_EVENTS:
            MDrv_SC_GetEvent(filp, arg);
            break;
        default:
            SC_PRINT("ioctl: unknown command\n");
            return -ENOTTY;
    }
    #else
    switch(cmd)
    {
        case MDRV_SC_ATTACH_INTERRUPT:
            MDrv_SC_AttachInterrupt(inode, filp, arg);
            break;
        case MDRV_SC_DETACH_INTERRUPT:
            MDrv_SC_DetachInterrupt(inode, filp, arg);
            break;
        case MDRV_SC_RESET_FIFO:
            MDrv_SC_ResetFIFO(inode, filp, arg);
            break;
        case MDRV_SC_GET_EVENTS:
            MDrv_SC_GetEvent(inode, filp, arg);
            break;
        default:
            SC_PRINT("ioctl: unknown command\n");
            return -ENOTTY;
    }
    #endif

    return 0;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
MSYSTEM_STATIC int __init mod_sc_init(void)
{
    int         s32Ret;
    dev_t       dev;

    SC_PRINT("%s is invoked\n", __FUNCTION__);

    if (devSC.s32Major)
    {
        dev = MKDEV(devSC.s32Major, devSC.s32Minor);
        s32Ret = register_chrdev_region(dev, SC_DEV_NUM, MDRV_NAME_SMART);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, devSC.s32Minor, SC_DEV_NUM, MDRV_NAME_SMART);
        devSC.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        SC_PRINT("Unable to get major %d\n", devSC.s32Major);
        return s32Ret;
    }

    cdev_init(&devSC.stCDev, &devSC.fops);
    if (0 != (s32Ret = cdev_add(&devSC.stCDev, dev, SC_DEV_NUM)))
    {
        SC_PRINT("Unable add a character device\n");
        unregister_chrdev_region(dev, SC_DEV_NUM);
        return s32Ret;
    }

    return 0;
}

MSYSTEM_STATIC void __exit mod_sc_exit(void)
{
    SC_PRINT("%s is invoked\n", __FUNCTION__);

    cdev_del(&devSC.stCDev);
    unregister_chrdev_region(MKDEV(devSC.s32Major, devSC.s32Minor), SC_DEV_NUM);
}

module_init(mod_sc_init);
module_exit(mod_sc_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("Smart card driver");
MODULE_LICENSE("GPL");

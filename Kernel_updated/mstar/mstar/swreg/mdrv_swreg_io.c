////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
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
/// file    mdrv_swreg_io.c
/// @brief  SWREG Driver Interface
/// @author MStar Semiconductor Inc.
/// please reference //THEALE/utopia/sw_register_table/sw_special_register.xls
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#include "mst_devid.h"
#include "mdrv_types.h"
#include "mdrv_swreg.h"
#include "mhal_swreg.h"
#include "mdrv_swreg_io.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
typedef struct
{
    S32                         s32MajorSWREG;
    S32                         s32MinorSWREG;
    struct cdev                 cDevice;
    struct file_operations      SWREGFop;
    struct fasync_struct        *async_queue; /* asynchronous readers */
} SWREG_ModHandle_t;


#define MOD_SWREG_DEVICE_COUNT         1
#define MOD_SWREG_NAME                 "ModSWREG"


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_SWREG_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_SWREG_Release(struct inode *inode, struct file *filp);
static ssize_t                  _MDrv_SWREG_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t                  _MDrv_SWREG_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static unsigned int             _MDrv_SWREG_Poll(struct file *filp, poll_table *wait);
static int                      _MDrv_SWREG_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
static int                      _MDrv_SWREG_Fasync(int fd, struct file *filp, int mode);

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#define SWREG_MUTEX_WAIT_TIME         3000

#define SWREG_DBG_ENABLE              1

#if SWREG_DBG_ENABLE
#define SWREG_DBG(_f)                 (_f)
#else
#define SWREG_DBG(_f)
#endif

#if 1
#define LINE_DBG()                  printf("SWREG %d\n", __LINE__)
#else
#define LINE_DBG()
#endif

#define SWREG_PRINT(fmt, args...)		printk("[SWREG][%05d] " fmt, __LINE__, ## args)

static SWREG_ModHandle_t SWREGDev=
{
    .s32MajorSWREG = MDRV_MAJOR_SWREG,
    .s32MinorSWREG = MDRV_MINOR_SWREG,
    .cDevice =
    {
        .kobj = {.name= MOD_SWREG_NAME, },
        .owner = THIS_MODULE,
    },
    .SWREGFop =
    {
        .open =     _MDrv_SWREG_Open,
        .release =  _MDrv_SWREG_Release,
        .read =     _MDrv_SWREG_Read,
        .write =    _MDrv_SWREG_Write,
        .poll =     _MDrv_SWREG_Poll,
        .ioctl =    _MDrv_SWREG_Ioctl,
        .fasync =   _MDrv_SWREG_Fasync,
    },
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

static int _MDrv_SWREG_Open (struct inode *inode, struct file *filp)
{
	SWREG_ModHandle_t *dev;

    SWREG_PRINT("%s is invoked\n", __FUNCTION__);

    dev = container_of(inode->i_cdev, SWREG_ModHandle_t, cDevice);
	filp->private_data = dev;

    return 0;
}

static int _MDrv_SWREG_Release(struct inode *inode, struct file *filp)
{
    SWREG_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static ssize_t _MDrv_SWREG_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    SWREG_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static ssize_t _MDrv_SWREG_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    SWREG_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static unsigned int _MDrv_SWREG_Poll(struct file *filp, poll_table *wait)
{
    SWREG_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}


static int _MDrv_SWREG_Fasync(int fd, struct file *filp, int mode)
{
    SWREG_PRINT("%s is invoked\n", __FUNCTION__);
	return 0;//fasync_helper(fd, filp, mode, &SWREGDev.async_queue);
}

static int _MDrv_SWREG_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    S32 s32Err= 0;

    SWREG_PRINT("%s is invoked\n", __FUNCTION__);

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if ((SWREG_IOC_MAGIC!= _IOC_TYPE(cmd)) || (_IOC_NR(cmd)> SWREG_IOC_MAXNR))
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
        s32Err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        s32Err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (s32Err)
    {
        return -EFAULT;
    }

    switch(cmd)
    {
        case MDRV_SWREG_SYS_HOLD:
            SWREG_PRINT("ioctl: MDRV_SWREG_SYS_HOLD\n");
            SWREG_SYS_HOLD();
            break;
        default:
            SWREG_PRINT("ioctl: unknown command\n");
            return -ENOTTY;
    }
    return 0;
}

static int __init mod_swreg_init(void)
{
    S32         s32Ret;
    dev_t       dev;

    if (SWREGDev.s32MajorSWREG)
    {
        dev = MKDEV(SWREGDev.s32MajorSWREG, SWREGDev.s32MinorSWREG);
        s32Ret = register_chrdev_region(dev, MOD_SWREG_DEVICE_COUNT, MOD_SWREG_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, SWREGDev.s32MinorSWREG, MOD_SWREG_DEVICE_COUNT, MOD_SWREG_NAME);
        SWREGDev.s32MajorSWREG = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        SWREG_PRINT("Unable to get major %d\n", SWREGDev.s32MajorSWREG);
        return s32Ret;
    }

    cdev_init(&SWREGDev.cDevice, &SWREGDev.SWREGFop);
    if (0!= (s32Ret= cdev_add(&SWREGDev.cDevice, dev, MOD_SWREG_DEVICE_COUNT)))
    {
        SWREG_PRINT("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_SWREG_DEVICE_COUNT);
        return s32Ret;
    }

    return 0;
}

static void __exit mod_swreg_exit(void)
{
    SWREG_PRINT("%s is invoked\n", __FUNCTION__);

    cdev_del(&SWREGDev.cDevice);
    unregister_chrdev_region(MKDEV(SWREGDev.s32MajorSWREG, SWREGDev.s32MinorSWREG), MOD_SWREG_DEVICE_COUNT);
}

module_init(mod_swreg_init);
module_exit(mod_swreg_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("SWREG driver");
MODULE_LICENSE("MSTAR");

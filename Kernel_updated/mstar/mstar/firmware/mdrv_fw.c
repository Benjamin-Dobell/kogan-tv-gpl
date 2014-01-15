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
/// file    mdrv_iomap.c
/// @brief  Memory IO remap Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/autoconf.h>
#include <linux/undefconf.h>
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
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <asm/io.h>
#include <asm/types.h>

#include "mst_devid.h"
#include "mdrv_firmware_io.h"
#include "mdrv_fw.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
//#define FW_DPRINTK(fmt, args...) printk(KERN_WARNING"%s:%d " fmt,__FUNCTION__,__LINE__,## args)
#define FW_DPRINTK(fmt, args...)

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MOD_FW_DEVICE_COUNT         1
#define MOD_FW_NAME                 "FwMAP"

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------;

typedef struct
{
    int                         s32FwMajor;
    int                         s32FwMinor;
    void*                       dmaBuf;
    struct cdev                 cDevice;
    struct file_operations      FirmwareFop;
} FwModHandle;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_FW_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_FW_Release(struct inode *inode, struct file *filp);
static int                      _MDrv_FW_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------
static FwModHandle FwDev=
{
    .s32FwMajor=          MDRV_MAJOR_FIRMWARE,
    .s32FwMinor=          MDRV_MINOR_FIRMWARE,
    .cDevice=
    {
        .kobj=                  {.name= MOD_FW_NAME, },
        .owner  =               THIS_MODULE,
    },
    .FirmwareFop=
    {
        .open=                  _MDrv_FW_Open,
        .release=               _MDrv_FW_Release,
        .ioctl=                 _MDrv_FW_Ioctl,
    },
};


static unsigned char _fw_tsp_dat[] __attribute__((section(".firmware_drv"))) =
{
    #include "fwTSP.dat"
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static int _MDrv_FW_Open (struct inode *inode, struct file *filp)
{
    FwModHandle *dev;

    // FW_DPRINTK("LennyD\n");
    dev = container_of(inode->i_cdev, FwModHandle, cDevice);
    filp->private_data = dev;
    return 0;
}

static int _MDrv_FW_Release(struct inode *inode, struct file *filp)
{
    // FW_DPRINTK("LennyD\n");
    return 0;
}

int MDrv_FW_TSP_Addr(void)
{
    return virt_to_phys(_fw_tsp_dat);
}
EXPORT_SYMBOL(MDrv_FW_TSP_Addr);

int MDrv_FW_TSP_Size(void)
{
    return sizeof(_fw_tsp_dat);
}
EXPORT_SYMBOL(MDrv_FW_TSP_Size);

static int _MDrv_FW_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int         err= 0;
    unsigned int u32Ret;

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (FW_IOC_MAGIC!= _IOC_TYPE(cmd))
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
        err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (err)
    {
        return -EFAULT;
    }

    // @FIXME: Use a array of function pointer for program readable and code size later
    switch(cmd)
    {
    //------------------------------------------------------------------------------
    // Signal
    //------------------------------------------------------------------------------
    case FW_IOC_TSP_ADDR:
        u32Ret = virt_to_phys(_fw_tsp_dat);
        copy_to_user((void*)arg, &u32Ret, sizeof(u32Ret));
        break;
    case FW_IOC_TSP_SIZE:
        u32Ret = sizeof(_fw_tsp_dat);
        copy_to_user((void*)arg, &u32Ret, sizeof(u32Ret));
        break;
    default:
        FW_DPRINTK("Unknown ioctl command %d\n", cmd);
        return -ENOTTY;
    }
    return 0;
}

static int __init mod_FW_init(void)
{
    int s32Ret;
    dev_t dev;

    if (FwDev.s32FwMajor)
    {
        dev = MKDEV(FwDev.s32FwMajor, FwDev.s32FwMinor);
        s32Ret = register_chrdev_region(dev, MOD_FW_DEVICE_COUNT, MOD_FW_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, FwDev.s32FwMinor, MOD_FW_DEVICE_COUNT, MOD_FW_NAME);
        FwDev.s32FwMajor = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        FW_DPRINTK("Unable to get major %d\n", FwDev.s32FwMajor);
        return s32Ret;
    }

    cdev_init(&FwDev.cDevice, &FwDev.FirmwareFop);
    if (0!= (s32Ret= cdev_add(&FwDev.cDevice, dev, MOD_FW_DEVICE_COUNT)))
    {
        FW_DPRINTK("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_FW_DEVICE_COUNT);
        return s32Ret;
    }

    return 0;
}

static void __exit mod_FW_exit(void)
{
    cdev_del(&FwDev.cDevice);
    unregister_chrdev_region(MKDEV(FwDev.s32FwMajor, FwDev.s32FwMinor), MOD_FW_DEVICE_COUNT);
}

module_init(mod_FW_init);
module_exit(mod_FW_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("Firmware driver");
MODULE_LICENSE("MSTAR");

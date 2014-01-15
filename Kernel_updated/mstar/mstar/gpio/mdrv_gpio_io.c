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
/// file    mdrv_gpio_io.c
/// @brief  GPIO Driver Interface
/// @author MStar Semiconductor Inc.
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
#include <linux/semaphore.h>
#include <linux/platform_device.h>

#include "mst_devid.h"

#include "mdrv_gpio_io.h"
#include "mhal_gpio_reg.h"
#include "mhal_gpio.h"
#include "mdrv_gpio.h"
//#include "mdrv_probe.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define GPIO_DBG_ENABLE              0

#if GPIO_DBG_ENABLE
#define GPIO_DBG(_f)                 (_f)
#else
#define GPIO_DBG(_f)
#endif

#if 0
#define LINE_DBG()                  printf("GPIO %d\n", __LINE__)
#else
#define LINE_DBG()
#endif

#define GPIO_PRINT(fmt, args...)        //printk("[GPIO][%05d] " fmt, __LINE__, ## args)

typedef struct
{
    S32                          s32MajorGPIO;
    S32                          s32MinorGPIO;
    struct cdev                 cDevice;
    struct file_operations      GPIOFop;
    struct fasync_struct        *async_queue; /* asynchronous readers */
} GPIO_ModHandle_t;


#define MOD_GPIO_DEVICE_COUNT         1
#define MOD_GPIO_NAME                 "ModGPIO"


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
//DECLARE_MUTEX(PfModeSem);


//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_GPIO_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_GPIO_Release(struct inode *inode, struct file *filp);
static ssize_t                  _MDrv_GPIO_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t                  _MDrv_GPIO_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static unsigned int             _MDrv_GPIO_Poll(struct file *filp, poll_table *wait);
#ifdef HAVE_UNLOCKED_IOCTL
static long _MDrv_GPIO_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int _MDrv_GPIO_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
static int                      _MDrv_GPIO_Fasync(int fd, struct file *filp, int mode);

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

static GPIO_ModHandle_t GPIODev=
{
    .s32MajorGPIO = MDRV_MAJOR_GPIO,
    .s32MinorGPIO = MDRV_MINOR_GPIO,
    .cDevice =
    {
        .kobj = {.name= MOD_GPIO_NAME, },
        .owner = THIS_MODULE,
    },
    .GPIOFop =
    {
        .open =     _MDrv_GPIO_Open,
        .release =  _MDrv_GPIO_Release,
        .read =     _MDrv_GPIO_Read,
        .write =    _MDrv_GPIO_Write,
        .poll =     _MDrv_GPIO_Poll,
        #ifdef HAVE_UNLOCKED_IOCTL
        .unlocked_ioctl = _MDrv_GPIO_Ioctl,
        #else        
        .ioctl = _MDrv_GPIO_Ioctl,
        #endif 
        .fasync =   _MDrv_GPIO_Fasync,
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

static int _MDrv_GPIO_Open (struct inode *inode, struct file *filp)
{
	GPIO_ModHandle_t *dev;

    GPIO_PRINT("%s is invoked\n", __FUNCTION__);

    dev = container_of(inode->i_cdev, GPIO_ModHandle_t, cDevice);
	filp->private_data = dev;

    return 0;
}

static int _MDrv_GPIO_Release(struct inode *inode, struct file *filp)
{
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static ssize_t _MDrv_GPIO_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static ssize_t _MDrv_GPIO_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static unsigned int _MDrv_GPIO_Poll(struct file *filp, poll_table *wait)
{
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}


static int _MDrv_GPIO_Fasync(int fd, struct file *filp, int mode)
{
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);

	return 0;//fasync_helper(fd, filp, mode, &IICDev.async_queue);
}

#ifdef HAVE_UNLOCKED_IOCTL
static long _MDrv_GPIO_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _MDrv_GPIO_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    S32 s32Err= 0;
  	U8 u8Temp;
	GPIO_Reg_t GPIO_reg;

    GPIO_PRINT("%s is invoked\n", __FUNCTION__);

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if ((GPIO_IOC_MAGIC!= _IOC_TYPE(cmd)) || (_IOC_NR(cmd)> GPIO_IOC_MAXNR))
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


    //PROBE_IO_ENTRY(MDRV_MAJOR_GPIO, _IOC_NR(cmd));

    switch(cmd)
    {
        //the cases of this section set to initialize
        case MDRV_GPIO_INIT:
            GPIO_PRINT("ioctl: MDRV_GPIO_INIT\n");
            MDrv_GPIO_Init();
            break;
        case MDRV_GPIO_SET:
            GPIO_PRINT("ioctl: MDRV_GPIO_SET\n");
            if (copy_from_user(&u8Temp, (U8 *) arg, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            MDrv_GPIO_Pad_Set(u8Temp);
            break;
        case MDRV_GPIO_OEN:
            GPIO_PRINT("ioctl: MDRV_GPIO_OEN\n");
            if (copy_from_user(&u8Temp, (U8 *) arg, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            MDrv_GPIO_Pad_Oen(u8Temp);
            break;
        case MDRV_GPIO_ODN:
            GPIO_PRINT("ioctl: MDRV_GPIO_ODN\n");
            if (copy_from_user(&u8Temp, (U8 *) arg, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            MDrv_GPIO_Pad_Odn(u8Temp);
            break;
        case MDRV_GPIO_READ:
            GPIO_PRINT("ioctl: MDRV_GPIO_READ\n");
            if (copy_from_user(&u8Temp, (U8 *) arg, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            u8Temp=MDrv_GPIO_Pad_Read(u8Temp);
            if (copy_to_user((U8 *) arg, &u8Temp, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
                return -EFAULT;
            }
            break;
        case MDRV_GPIO_PULL_HIGH:
            GPIO_PRINT("ioctl: MDRV_GPIO_PULL_HIGH\n");
            if (copy_from_user(&u8Temp, (U8 *) arg, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            MDrv_GPIO_Pull_High(u8Temp);
            break;
        case MDRV_GPIO_PULL_LOW:
            GPIO_PRINT("ioctl: MDRV_GPIO_PULL_LOW\n");
            if (copy_from_user(&u8Temp, (U8 *) arg, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            MDrv_GPIO_Pull_Low(u8Temp);
            break;
        case MDRV_GPIO_INOUT:
            GPIO_PRINT("ioctl: MDRV_GPIO_INOUT\n");
            if (copy_from_user(&u8Temp, (U8 *) arg, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            u8Temp=MDrv_GPIO_Pad_InOut(u8Temp);
            if (copy_to_user((U8 *) arg, &u8Temp, sizeof(U8)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
                return -EFAULT;
            }
            break;
        case MDRV_GPIO_WREGB:
            GPIO_PRINT("ioctl: MDRV_GPIO_WREGB\n");
            if (copy_from_user(&GPIO_reg, (GPIO_Reg_t __user *) arg, sizeof(GPIO_Reg_t)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            MDrv_GPIO_WriteRegBit(GPIO_reg.u32Reg, GPIO_reg.u8Enable, GPIO_reg.u8BitMsk);
            break;
        default:
            GPIO_PRINT("ioctl: unknown command\n");
            //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
            return -ENOTTY;
    }

    //PROBE_IO_EXIT(MDRV_MAJOR_GPIO, _IOC_NR(cmd));
    return 0;
}

void __mod_gpio_init(void)
{
    //GPIO chiptop initialization
	MDrv_GPIO_Init();
}

static int mod_gpio_init(void)
{
    S32         s32Ret;
    dev_t       dev;

    GPIO_PRINT("%s is invoked\n", __FUNCTION__);

    if (GPIODev.s32MajorGPIO)
    {
        dev = MKDEV(GPIODev.s32MajorGPIO, GPIODev.s32MinorGPIO);
        s32Ret = register_chrdev_region(dev, MOD_GPIO_DEVICE_COUNT, MOD_GPIO_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, GPIODev.s32MinorGPIO, MOD_GPIO_DEVICE_COUNT, MOD_GPIO_NAME);
        GPIODev.s32MajorGPIO = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        GPIO_PRINT("Unable to get major %d\n", GPIODev.s32MajorGPIO);
        return s32Ret;
    }

    cdev_init(&GPIODev.cDevice, &GPIODev.GPIOFop);
    if (0!= (s32Ret= cdev_add(&GPIODev.cDevice, dev, MOD_GPIO_DEVICE_COUNT)))
    {
        GPIO_PRINT("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_GPIO_DEVICE_COUNT);
        return s32Ret;
    }

	__mod_gpio_init();

	return 0;


}

static void mod_gpio_exit(void)
{
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);

    cdev_del(&GPIODev.cDevice);
    unregister_chrdev_region(MKDEV(GPIODev.s32MajorGPIO, GPIODev.s32MinorGPIO), MOD_GPIO_DEVICE_COUNT);
}

static int mstar_gpio_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}
static int mstar_gpio_drv_resume(struct platform_device *dev)
{
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);
    __mod_gpio_init();
    return 0;
}

static int mstar_gpio_drv_probe(struct platform_device *pdev)
{
    int retval=0;
    if( !(pdev->name) || strcmp(pdev->name,"Mstar-gpio")
        || pdev->id!=0)
    {
        retval = -ENXIO;
    }

    retval = mod_gpio_init();
    if(!retval)
    {
        pdev->dev.platform_data=&GPIODev;
    }
	return retval;
}

static int mstar_gpio_drv_remove(struct platform_device *pdev)
{
    if( !(pdev->name) || strcmp(pdev->name,"Mstar-gpio")
        || pdev->id!=0)
    {
        return -1;
    }

    mod_gpio_exit();
    pdev->dev.platform_data=NULL;
    return 0;
}


static struct platform_driver Mstar_gpio_driver = {
    .probe      = mstar_gpio_drv_probe,
    .remove 	= mstar_gpio_drv_remove,
    .suspend    = mstar_gpio_drv_suspend,
    .resume     = mstar_gpio_drv_resume,

	.driver = {
		.name	= "Mstar-gpio",
        .owner  = THIS_MODULE,
	}
};

static int __init mstar_gpio_drv_init_module(void)
{
    int retval=0;
    GPIO_PRINT("%s is invoked\n", __FUNCTION__);
    retval = platform_driver_register(&Mstar_gpio_driver);
    return retval;
}

static void __exit mstar_gpio_drv_exit_module(void)
{
    platform_driver_unregister(&Mstar_gpio_driver);
}

module_init(mstar_gpio_drv_init_module);
module_exit(mstar_gpio_drv_exit_module);

//EXPORT_SYMBOL(PfModeSem);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("GPIO driver");
MODULE_LICENSE("GPL");


////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
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
/// file    mdrv_mpool.c
/// @brief  Memory Pool Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
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
#include <linux/device.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/string.h>

#include <asm/cache.h>
#include <asm/io.h>

#include <dma-coherence.h>


#include "mst_devid.h"
#include "mdrv_vpool.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
//#define MPOOL_DPRINTK(fmt, args...) printk(KERN_WARNING"%s:%d " fmt,__FUNCTION__,__LINE__,## args)
#define VPOOL_DPRINTK(fmt, args...)

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MOD_VPOOL_DEVICE_COUNT     1
#define MOD_VPOOL_NAME             "vpool"


#if 0
#if defined(CONFIG_MSTAR_TITANIA3) || \
    defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_AMBER2  || \
    defined(CONFIG_MSTAR_AMBER5)
    #define HAL_MIU0_LEN 0x10000000
#else
    #define HAL_MIU0_LEN 0x08000000
#endif
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------;

typedef struct
{
    int                         s32VPoolMajor;
    int                         s32VPoolMinor;
    void*                       dmaBuf;
    struct cdev                 cDevice;
    struct file_operations      VPoolFop;
} VPoolModHandle;


//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_VPool_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_VPool_Release(struct inode *inode, struct file *filp);
static int                      _MDrv_VPool_MMap(struct file *filp, struct vm_area_struct *vma);
static int                      __MDrv_VPool_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------

static struct class *vpool_class;

static VPoolModHandle VPool_Dev=
{
    .s32VPoolMajor=               MDRV_MAJOR_VPOOL,
    .s32VPoolMinor=               MDRV_MINOR_VPOOL,
    .cDevice=
    {
        .kobj=                  {.name= MOD_VPOOL_NAME, },
        .owner  =               THIS_MODULE,
    },
    .VPoolFop=
    {
        .open=                  _MDrv_VPool_Open,
        .release=               _MDrv_VPool_Release,
        .mmap=                  _MDrv_VPool_MMap,
        .ioctl=                 __MDrv_VPool_Ioctl,
    },
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static int _MDrv_VPool_Open (struct inode *inode, struct file *filp)
{
    VPoolModHandle *dev;

    dev = container_of(inode->i_cdev, VPoolModHandle, cDevice);
    // dev->dmaBuf=ioremap_nocache(MPOOL_BASE, MPOOL_SIZE);
    filp->private_data = dev;

    return 0;
}

static int _MDrv_VPool_Release(struct inode *inode, struct file *filp)
{
    // MPoolModHandle *dev = filp->private_data ;;

    // iounmap(dev->dmaBuf) ;
    return 0;
}

static inline int valid_video_phys_addr_range(unsigned long pfn, size_t size)
{
       //need to add verify here
	return 1;
}
static int _MDrv_VPool_MMap(struct file *filp, struct vm_area_struct *vma)
{
     unsigned long prot;
    //printk("mpool_base=%x\n",mpool_base);
    //printk("linux_base=%x\n",linux_base);

       size_t size = vma->vm_end - vma->vm_start;

       VPOOL_DPRINTK(printk("vpool map range 0x%08x~0x%08x\n", vma->vm_pgoff<<PAGE_SHIFT, size));

	if (!valid_video_phys_addr_range(vma->vm_pgoff, size))
		return -EINVAL;

	prot = pgprot_val(vma->vm_page_prot);
	prot = (prot & ~_CACHE_MASK) | _CACHE_CACHABLE_NONCOHERENT;
	vma->vm_page_prot = __pgprot(prot);



	/* Remap-pfn-range will mark the range VM_IO and VM_RESERVED */
	if (remap_pfn_range(vma,
			    vma->vm_start,
			    vma->vm_pgoff,
			    size,
			    vma->vm_page_prot)) {
		return -EAGAIN;
	}

	return 0;

}
#if defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) || defined(CONFIG_MSTAR_TITANIA4) || defined(CONFIG_MSTAR_URANUS4)
extern void Chip_Flush_Memory(void);
IMPORT_SYMBOL(Chip_Flush_Memory);
#endif
static int __MDrv_VPool_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int         err= 0;

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (VPOOL_IOC_MAGIC!= _IOC_TYPE(cmd))
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

    switch(cmd)
    {
    case VPOOL_IOC_FLUSH_INV_DCACHE:
       {
           DrvVPool_Info_t region ;
            copy_from_user(&region, (void __user *)arg, sizeof(region));
           dma_cache_wback_inv((unsigned long)region.pAddr, region.u32Size);
       }
#if defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) || defined(CONFIG_MSTAR_TITANIA4) || defined(CONFIG_MSTAR_URANUS4)
	Chip_Flush_Memory();
#endif

       break;
    default:
        printk("Unknown ioctl command %d\n", cmd);
        return -ENOTTY;
    }
    return 0;
}


static int __init mod_vpool_init(void)
{
    int s32Ret;
    dev_t dev;

    //printk( "\nMpool size=0x%08X\n", mpool_size );

    vpool_class = class_create(THIS_MODULE, "vpool");
    if (IS_ERR(vpool_class))
    {
        return PTR_ERR(vpool_class);
    }

    if (VPool_Dev.s32VPoolMajor)
    {
        dev = MKDEV(VPool_Dev.s32VPoolMajor, VPool_Dev.s32VPoolMinor);
        s32Ret = register_chrdev_region(dev, MOD_VPOOL_DEVICE_COUNT, MOD_VPOOL_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, 0, MOD_VPOOL_DEVICE_COUNT, MOD_VPOOL_NAME);
        VPool_Dev.s32VPoolMajor = MAJOR(dev);
        VPool_Dev.s32VPoolMinor = MINOR(dev);
    }

    if ( 0 > s32Ret)
    {
        VPOOL_DPRINTK("Unable to get major %d\n");
        class_destroy(vpool_class);
        return s32Ret;
    }

    cdev_init(&VPool_Dev.cDevice, &VPool_Dev.VPoolFop);
    if (0!= (s32Ret= cdev_add(&VPool_Dev.cDevice, dev, MOD_VPOOL_DEVICE_COUNT)))
    {
        VPOOL_DPRINTK("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_VPOOL_DEVICE_COUNT);
        class_destroy(vpool_class);
        return s32Ret;
    }

    device_create(vpool_class, NULL, dev, NULL, MOD_VPOOL_NAME);

    return 0;
}


static void __exit mod_vpool_exit(void)
{
    cdev_del(&VPool_Dev.cDevice);
    unregister_chrdev_region(MKDEV(VPool_Dev.s32VPoolMajor, 0), MOD_VPOOL_DEVICE_COUNT);
    class_destroy(vpool_class);
}

module_init(mod_vpool_init);
module_exit(mod_vpool_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("VPOOL driver");
MODULE_LICENSE("GPL");

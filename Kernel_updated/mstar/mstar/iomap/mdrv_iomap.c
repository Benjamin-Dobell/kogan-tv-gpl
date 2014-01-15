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
//#include "MsCommon.h"
#include <linux/autoconf.h>
//#include <linux/undefconf.h>
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
#include <linux/version.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/cacheflush.h>

#include "mst_devid.h"
#include "mdrv_iomap.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
//#define MIOMAP_DPRINTK(fmt, args...) printk(KERN_WARNING"%s:%d " fmt,__FUNCTION__,__LINE__,## args)
#define MIOMAP_DPRINTK(fmt, args...)

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define MOD_MIOMAP_DEVICE_COUNT     1
#define MOD_MIOMAP_NAME             "miomap"

// Define MIOMAP Device

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------;

typedef struct
{
    int                         s32MIOMapMajor;
    int                         s32MIOMapMinor;
    void*                       dmaBuf;
    struct cdev                 cDevice;
    struct file_operations      MIOMapFop;
} MIOMapModHandle;

typedef  struct
{
   U32 miomap_base;
   U32 miomap_size;
}MMIO_FileData;
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_MIOMAP_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_MIOMAP_Release(struct inode *inode, struct file *filp);
static int                      _MDrv_MIOMAP_MMap(struct file *filp, struct vm_area_struct *vma);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long                     _MDrv_MIOMAP_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int                      _MDrv_MIOMAP_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif 



//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------
static struct class *miomap_class;

static MIOMapModHandle MIOMapDev=
{
    .s32MIOMapMajor=               MDRV_MAJOR_MIOMAP,
    .s32MIOMapMinor=               MDRV_MINOR_MIOMAP,
    .cDevice=
    {
        .kobj=                  {.name= MOD_MIOMAP_NAME, },
        .owner  =               THIS_MODULE,
    },
    .MIOMapFop=
    {
        .open=                  _MDrv_MIOMAP_Open,
        .release=               _MDrv_MIOMAP_Release,
        .mmap=                  _MDrv_MIOMAP_MMap,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
        .unlocked_ioctl=        _MDrv_MIOMAP_Ioctl,
	#else
        .ioctl =                _MDrv_MIOMAP_Ioctl,
	#endif 
    },
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static int _MDrv_MIOMAP_Open (struct inode *inode, struct file *filp)
{

    MMIO_FileData *mmioData;

    MIOMAP_DPRINTK("LennyD\n");


    mmioData = kzalloc(sizeof(*mmioData), GFP_KERNEL);
    if (mmioData == NULL)
         return -ENOMEM;

    filp->private_data = mmioData;

    return 0;
}

static int _MDrv_MIOMAP_Release(struct inode *inode, struct file *filp)
{
     MMIO_FileData *mmioData = filp->private_data ;
     kfree(mmioData);

    MIOMAP_DPRINTK("LennyD\n");
    // iounmap(dev->dmaBuf) ;
    return 0;
}

static int _MDrv_MIOMAP_MMap(struct file *filp, struct vm_area_struct *vma)
{
    MMIO_FileData *mmioData = filp->private_data ;

    vma->vm_pgoff = mmioData->miomap_base >> PAGE_SHIFT;

    if(vma->vm_end-vma->vm_start != PAGE_ALIGN(mmioData->miomap_size))
    {
          printk(KERN_ERR "_MDrv_MIOMAP_MMap invalid argument, request io length is 0x%08lx, current io length is 0x%08x\n      --pgoff 0x%08lx, tid%d, pid%d\n", 
                     vma->vm_end-vma->vm_start , mmioData->miomap_size, vma->vm_pgoff, current->tgid, current->pid);
          panic("It's not really a kernel bug. I panic here for purpose to notify you that user mode app has bug!");
         return -EINVAL;
    }
#if 0
    vma->vm_flags = VM_WRITE|VM_MAYWRITE|VM_READ|VM_MAYREAD|VM_RESERVED|VM_DONTCOPY;
    vma->vm_flags |= VM_LOCKED;
#endif
    /* set page to no cache */
    #if defined(CONFIG_MIPS)
    {
       pgprot_val(vma->vm_page_prot) &= ~_CACHE_MASK;
       pgprot_val(vma->vm_page_prot) |= _CACHE_UNCACHED;
    }

    #elif defined(CONFIG_ARM)
    {

       pgprot_val(vma->vm_page_prot) = pgprot_noncached(vma->vm_page_prot);
    }
    #endif

    if (io_remap_pfn_range(vma, vma->vm_start,
                            mmioData->miomap_base >> PAGE_SHIFT, mmioData->miomap_size,
                            vma->vm_page_prot))
    {
        return -EAGAIN;
    }
    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _MDrv_MIOMAP_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _MDrv_MIOMAP_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int         err= 0;
	int         ret= 0;
   MMIO_FileData *mmioData = filp->private_data ;
   
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (MIOMAP_IOC_MAGIC!= _IOC_TYPE(cmd))
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
    case MIOMAP_IOC_CHIPINFO:
        {
            DrvMIOMap_ChipInfo_t i ;
            // Chip Type
            // Uranus 0x0101
            // Oberon 0x0102
            // Euclid 0x0103
            // T1     0x0201
            // T2     0x0202
            // T3     0x0203
            // Chip version
            // FPGA   0x0000
            // U01    0x0001
            // U02    0x0002
            // U03    0x0003

            #if defined(CONFIG_MSTAR_OBERON)
                i.u16chiptype    = 0x0102 ;
                i.u16chipversion = 0x0001 ;
                //printk("It's oberon now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_TITANIA2)
                i.u16chiptype    = 0x0202 ;
                i.u16chipversion = 0x0001 ;
                //printk("It's oberon now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_EUCLID)
                i.u16chiptype    = 0x0103 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's oberon now\n"); // test string
            #endif

            #if (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10))
                i.u16chiptype    = 0x0203 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's oberon now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_TITANIA4)
                i.u16chiptype    = 0x0204 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's titania4 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA12)
                i.u16chiptype    = 0x0208 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's titania8 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_TITANIA9)
                i.u16chiptype    = 0x0209 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's titania9 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_TITANIA11)
                i.u16chiptype    = 0x020B ;
                i.u16chipversion = 0x0000 ;
                //printk("It's titania11 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_TITANIA13)
                i.u16chiptype    = 0x020D ;
                i.u16chipversion = 0x0000 ;
                //printk("It's titania13 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_URANUS4)
                i.u16chiptype    = 0x0104 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's uranus4 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_JANUS2)
                i.u16chiptype    = 0x0302 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's janus2 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_KRONUS)
                i.u16chiptype    = 0x0104 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's kronus now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_KAISERIN)
                i.u16chiptype    = 0x0104 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's kaiserin now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_AMBER1)
                i.u16chiptype    = 0x0301 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's amber1 now\n"); // test string
            #endif

	    #if defined(CONFIG_MSTAR_AMBER2)
                i.u16chiptype    = 0x0303 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's amber2 now\n"); // test string
            #endif

	        #if defined(CONFIG_MSTAR_AMBER5)
                i.u16chiptype    = 0x0303 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's amber5 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_AMBER6)
                i.u16chiptype    = 0x0400 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's amber6 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_AMBER7)
                i.u16chiptype    = 0x0402 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's amber7 now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_AMETHYST)
                i.u16chiptype    = 0x040D ;
                i.u16chipversion = 0x0000 ;
                //printk("It's amethyst now\n"); // test string
            #endif

            #if defined(CONFIG_MSTAR_EAGLE)
                i.u16chiptype    = 0x0501 ;
                i.u16chipversion = 0x0000 ;
                //printk("It's eagle now\n"); // test string
            #endif
            
            ret= copy_to_user( (void *)arg, &i, sizeof(i) ) ;
        }
        break;

    case MIOMAP_IOC_SET_MAP:
        {
            DrvMIOMap_Info_t i ;

            ret= copy_from_user( &i, (DrvMIOMap_Info_t*)arg, sizeof(i) ) ;
            mmioData->miomap_base = i.u32Addr ;
            mmioData->miomap_size = i.u32Size ;
            //printk("MIOMAP_BASE=%x MIOMAP_SIZE=%x\n", MIOMAP_BASE, MIOMAP_SIZE);
        }
        break;

    case MIOMAP_IOC_FLUSHDCACHE:
        {
            DrvMIOMap_Info_t i ;

            ret= copy_from_user( &i, (DrvMIOMap_Info_t*)arg, sizeof(i) ) ;

            #if defined(CONFIG_MIPS)
            _dma_cache_wback_inv((unsigned long)__va(i.u32Addr), i.u32Size) ;
            #elif defined(CONFIG_ARM)
            __cpuc_flush_dcache_area(__va(i.u32Addr),i.u32Size);

            #endif
        }
        break;

    case MIOMAP_IOC_GET_BLOCK_OFFSET:
        {
            DrvMIOMap_Info_t i ;
            ret= copy_from_user( &i, (DrvMIOMap_Info_t*)arg, sizeof(i) ) ;
            // mark temporarily, restore later
            //MDrv_SYS_GetMMAP(i.u32Addr, &(i.u32Addr), &(i.u32Size)) ;
            ret= copy_to_user( (void *)arg, &i, sizeof(i) ) ;
        }
        break ;
    default:
        printk("Unknown ioctl command %d\n", cmd);
        return -ENOTTY;
    }
    return 0;
}

MSYSTEM_STATIC int __init mod_miomap_init(void)
{
    int s32Ret;
    dev_t dev;

    miomap_class = class_create(THIS_MODULE, "miomap");
    if (IS_ERR(miomap_class))
    {
        return PTR_ERR(miomap_class);
    }

    if (MIOMapDev.s32MIOMapMajor)
    {
        dev = MKDEV(MIOMapDev.s32MIOMapMajor, MIOMapDev.s32MIOMapMinor);
        s32Ret = register_chrdev_region(dev, MOD_MIOMAP_DEVICE_COUNT, MOD_MIOMAP_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, MIOMapDev.s32MIOMapMinor, MOD_MIOMAP_DEVICE_COUNT, MOD_MIOMAP_NAME);
        MIOMapDev.s32MIOMapMajor = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        MIOMAP_DPRINTK("Unable to get major %d\n", MIOMapDev.s32MIOMapMajor);
        class_destroy(miomap_class);
        return s32Ret;
    }

    cdev_init(&MIOMapDev.cDevice, &MIOMapDev.MIOMapFop);
    if (0!= (s32Ret= cdev_add(&MIOMapDev.cDevice, dev, MOD_MIOMAP_DEVICE_COUNT)))
    {
        MIOMAP_DPRINTK("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_MIOMAP_DEVICE_COUNT);
        class_destroy(miomap_class);
        return s32Ret;
    }

    device_create(miomap_class, NULL, dev, NULL, MOD_MIOMAP_NAME);

    return 0;
}

MSYSTEM_STATIC void __exit mod_miomap_exit(void)
{
    cdev_del(&MIOMapDev.cDevice);
    unregister_chrdev_region(MKDEV(MIOMapDev.s32MIOMapMajor, MIOMapDev.s32MIOMapMinor), MOD_MIOMAP_DEVICE_COUNT);
    device_destroy(miomap_class, MKDEV(MIOMapDev.s32MIOMapMajor, MIOMapDev.s32MIOMapMinor));
    class_destroy(miomap_class);
}

#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
module_init(mod_miomap_init);
module_exit(mod_miomap_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MIOMAP driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)

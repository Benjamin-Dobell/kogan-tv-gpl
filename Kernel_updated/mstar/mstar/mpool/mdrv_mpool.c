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
//#include <linux/undefconf.h> //unused header file now
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

#if defined(CONFIG_MIPS)
#include <asm/mips-boards/prom.h>
#include "mdrv_cache.h"
#elif defined(CONFIG_ARM)
#endif

#include "mst_devid.h"
#include "mdrv_mpool.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"

#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
#include "chip_setup.h"
#endif

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
//#define MPOOL_DPRINTK(fmt, args...) printk(KERN_WARNING"%s:%d " fmt,__FUNCTION__,__LINE__,## args)
#define MPOOL_DPRINTK(fmt, args...)

//-------------------------------------------------------------------------------------------------
//  Global variables
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_MIPS)
unsigned int bMPoolInit = 0;
#endif
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MPOOL_VERSION 1
#define MOD_MPOOL_DEVICE_COUNT     1
#define MOD_MPOOL_NAME             "malloc"
#define KER_CACHEMODE_CACHE   1
#define KER_CACHEMODE_UNCACHE_NONBUFFERABLE 0
#define KER_CACHEMODE_UNCACHE_BUFFERABLE 2



// Define MPOOL Device
U32 linux_base;
U32 linux_size;
U32 linux2_base;
U32 linux2_size;
U32 emac_base;
U32 emac_size;


typedef  struct
{
   U32 mpool_base;
   U32 mpool_size;
U32 mmap_offset;
U32 mmap_size;
U32 mmap_interval;
U8  mmap_miusel;
   unsigned int u8MapCached;
}MMAP_FileData;

bool setflag;


unsigned int mpool_version = MPOOL_VERSION;
#if defined(CONFIG_MSTAR_TITANIA3) || \
    defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE)    || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL) || \
    defined(CONFIG_MSTAR_EINSTEIN)
    #define HAL_MIU0_LEN 0x10000000
#else
    #define HAL_MIU0_LEN 0x08000000
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------;

typedef struct
{
    int                         s32MPoolMajor;
    int                         s32MPoolMinor;
    void*                       dmaBuf;
    struct cdev                 cDevice;
    struct file_operations      MPoolFop;
} MPoolModHandle;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
extern void (*flush_cache_all)(void);

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_MPOOL_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_MPOOL_Release(struct inode *inode, struct file *filp);
static int                      _MDrv_MPOOL_MMap(struct file *filp, struct vm_area_struct *vma);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long                     _MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int                      _MDrv_MPOOL_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------

static struct class *mpool_class;

static MPoolModHandle MPoolDev=
{
    .s32MPoolMajor=               MDRV_MAJOR_MPOOL,
    .s32MPoolMinor=               MDRV_MINOR_MPOOL,
    .cDevice=
    {
        .kobj=                  {.name= MOD_MPOOL_NAME, },
        .owner  =               THIS_MODULE,
    },
    .MPoolFop=
    {
        .open=                  _MDrv_MPOOL_Open,
        .release=               _MDrv_MPOOL_Release,
        .mmap=                  _MDrv_MPOOL_MMap,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
        .unlocked_ioctl=        _MDrv_MPOOL_Ioctl,
	#else
        .ioctl =  		_MDrv_MPOOL_Ioctl,
        #endif
    },
};



//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static int _MDrv_MPOOL_Open (struct inode *inode, struct file *filp)
{

    MMAP_FileData *mmapData;

    mmapData = kzalloc(sizeof(*mmapData), GFP_KERNEL);
    if (mmapData == NULL)
          return -ENOMEM;

    filp->private_data = mmapData;
    mmapData->u8MapCached = 1;


    return 0;
}

static int _MDrv_MPOOL_Release(struct inode *inode, struct file *filp)
{
    MMAP_FileData *mmapData = filp->private_data ;
    kfree(mmapData);

    // iounmap(dev->dmaBuf) ;
    return 0;
}


static int _MDrv_MPOOL_MMap(struct file *filp, struct vm_area_struct *vma)
{
    MMAP_FileData *mmapData = filp->private_data;

    u32 miu0_len=0x10000000;
    if(!setflag)
    	     vma->vm_pgoff = mmapData->mpool_base >> PAGE_SHIFT;
    else
    {
        if(mmapData->mmap_miusel == 0)
            vma->vm_pgoff = mmapData->mmap_offset >> PAGE_SHIFT;
        else
            vma->vm_pgoff = (mmapData->mmap_offset+mmapData->mmap_interval) >> PAGE_SHIFT;
    }

    /* set page to no cache */

    if (mmapData->u8MapCached == KER_CACHEMODE_CACHE)
    {
        #if defined(CONFIG_MIPS)
        pgprot_val(vma->vm_page_prot) &= ~_CACHE_MASK;
        pgprot_val(vma->vm_page_prot) |= _page_cachable_default;
        #elif defined(CONFIG_ARM)
        //vma->vm_page_prot=__pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK,L_PTE_MT_WRITEBACK);
        vma->vm_page_prot=__pgprot_modify(vma->vm_page_prot,L_PTE_MT_MASK,L_PTE_MT_DEV_CACHED);
        #endif
    }
    else
    {
        #if defined(CONFIG_MIPS)
        pgprot_val(vma->vm_page_prot) &= ~_CACHE_MASK;
        pgprot_val(vma->vm_page_prot) |= _CACHE_UNCACHED;
        #elif defined(CONFIG_ARM)
        if(mmapData->u8MapCached == KER_CACHEMODE_UNCACHE_BUFFERABLE)
        {
            pgprot_val(vma->vm_page_prot) = pgprot_dmacoherent(vma->vm_page_prot);
        }
        else
        {
            pgprot_val(vma->vm_page_prot) = pgprot_noncached(vma->vm_page_prot);
        }
        #endif
    }

    if(setflag)
    {
            if(mmapData->mmap_miusel == 0)
            {
                #if defined(CONFIG_MIPS)
                if(io_remap_pfn_range(vma, vma->vm_start,
                                (MIPS_MIU0_BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
                                         vma->vm_page_prot))
                #elif defined(CONFIG_ARM)
                if(io_remap_pfn_range(vma, vma->vm_start,
                                (ARM_MIU0_BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
                                         vma->vm_page_prot))

                #endif
                    return -EAGAIN;
             }

            else
            {
                #if defined(CONFIG_MIPS)
                if(io_remap_pfn_range(vma, vma->vm_start,
                                (MIPS_MIU1_BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
                                         vma->vm_page_prot))
                #elif defined(CONFIG_ARM)
                if(io_remap_pfn_range(vma, vma->vm_start,
                                (ARM_MIU1_BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
                                         vma->vm_page_prot))

                #endif
                    return -EAGAIN;

            }
    }
else
{

#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA7) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2) || \
	defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL)
    {

#if  (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10))
   //check MIU0 size from MAU
   u16 miu0_size_reg_val=0;
#endif

	unsigned long u32MIU0_MapStart=0;
        unsigned long u32MIU0_MapSize=0;
        unsigned long u32MIU1_MapStart=0;
        unsigned long u32MIU1_MapSize=0;

#if  (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10))
   miu0_size_reg_val=REG_ADDR(REG_MAU0_MIU0_SIZE);
   if(miu0_size_reg_val&0x100)
   {
       miu0_len=0x10000000;
   }
#endif

        //calculate map size & start
        if(mmapData->mpool_base<miu0_len)
        {
            u32MIU0_MapStart=mmapData->mpool_base;

            if((mmapData->mpool_base+mmapData->mpool_size)>miu0_len)
            {
                u32MIU0_MapSize=(miu0_len-mmapData->mpool_base);
                u32MIU1_MapSize=mmapData->mpool_size-u32MIU0_MapSize;
                #if defined(CONFIG_MIPS)
                u32MIU1_MapStart=MIPS_MIU1_BUS_BASE;
                #elif defined(CONFIG_ARM)
                u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
                #endif
            }
	    else
            {
                u32MIU0_MapSize=mmapData->mpool_size;
                u32MIU1_MapSize=0;
                #if defined(CONFIG_MIPS)
                u32MIU1_MapStart=MIPS_MIU1_BUS_BASE;
                #elif defined(CONFIG_ARM)
                u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
                #endif

            }
        }
        else
        {
            u32MIU0_MapStart=0;
            u32MIU0_MapSize=0;
            #if defined(CONFIG_MIPS)
            u32MIU1_MapStart=(mmapData->mpool_base-miu0_len)+MIPS_MIU1_BUS_BASE;
            #elif defined(CONFIG_ARM)
            u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
            #endif
            u32MIU1_MapSize=mmapData->mpool_size;

        }
        //printk("MPOOL MAP INFORMATION:\n");
        //printk("    MIU0 Length=0x%08X\n",miu0_len);
        //printk("    MIU0 MAP:0x%08lX,0x%08lX\n",u32MIU0_MapStart,u32MIU0_MapSize);
        //printk("    MIU1 MAP:0x%08lX,0x%08lX\n",u32MIU1_MapStart,u32MIU1_MapSize);


        if(u32MIU0_MapSize)
        {
            if (io_remap_pfn_range(vma, vma->vm_start+ mmapData->mpool_base,
                                        u32MIU0_MapStart >> PAGE_SHIFT, u32MIU0_MapSize,
                                        vma->vm_page_prot))
            {
              return -EAGAIN;
            }

        }

        if(u32MIU1_MapSize)
        {
            #if defined(CONFIG_MIPS)
           if (io_remap_pfn_range(vma, vma->vm_start+u32MIU0_MapSize,
                                       MIPS_MIU1_BUS_BASE >> PAGE_SHIFT, u32MIU1_MapSize,
                                       vma->vm_page_prot))
            #elif defined(CONFIG_ARM)
           if (io_remap_pfn_range(vma, vma->vm_start+u32MIU0_MapSize,
                                       ARM_MIU1_BUS_BASE >> PAGE_SHIFT, u32MIU1_MapSize,
                                       vma->vm_page_prot))
            #endif
           {
             return -EAGAIN;
           }

        }


    }

#else
    if (io_remap_pfn_range(vma, vma->vm_start+mmapData->mpool_base,
                           mmapData->mpool_base >> PAGE_SHIFT, mmapData->mpool_size,
                           vma->vm_page_prot))
    {
        return -EAGAIN;
    }
#endif
}
    return 0;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _MDrv_MPOOL_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int         err= 0;
    int         ret= 0;

    MMAP_FileData *mmapData = filp->private_data ;

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (MPOOL_IOC_MAGIC!= _IOC_TYPE(cmd))
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
    case MPOOL_IOC_INFO:
        {
            DrvMPool_Info_t i ;

            setflag = false;
            i.u32Addr = mmapData->mpool_base;
            i.u32Size = mmapData->mpool_size;
            MPOOL_DPRINTK("MPOOL_IOC_INFO i.u32Addr = %d\n", i.u32Addr);
            MPOOL_DPRINTK("MPOOL_IOC_INFO i.u32Size = %d\n", i.u32Size);

            ret= copy_to_user( (void *)arg, &i, sizeof(i) );
        }
        break;
    case MPOOL_IOC_FLUSHDCACHE:
        {
#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA7) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2)|| \
	defined(CONFIG_MSTAR_AMBER5)|| \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL)
            DrvMPool_Info_t i ;
            unsigned long miu0_len;
            ret= copy_from_user(&i, (void __user *)arg, sizeof(i));
            miu0_len=HAL_MIU0_LEN;

#if (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10))
            //check MIU0 size from MAU
            unsigned short miu0_size_reg_val;
            miu0_size_reg_val=REG_ADDR(REG_MAU0_MIU0_SIZE);
            if(miu0_size_reg_val&0x100)
            {
                miu0_len=0x10000000;
            }
            else
            {
                miu0_len=0x08000000;
            }
#endif//defined(CONFIG_MSTAR_TITANIA3)
            {
                #if defined(CONFIG_MIPS)
                _dma_cache_wback_inv((unsigned long)(i.u32Addr), i.u32Size);
                #elif defined(CONFIG_ARM)
                //printk("i.u32Addr=0x%x,i.u32Size=0x%x,__va(i.u32Addr)=0x%x\n",i.u32Addr,i.u32Size,__va(i.u32Addr));
                Chip_Flush_Cache_Range((i.u32Addr), i.u32Size);
                #endif
            }
#else
            DrvMPool_Info_t i ;
            copy_from_user(&i, (void __user *)arg, sizeof(i));
            #if defined(CONFIG_MIPS)
            _dma_cache_wback_inv((unsigned long)__va(i.u32Addr), i.u32Size);
            #elif defined(CONFIG_ARM)
            //printk("i.u32Addr=0x%x,i.u32Size=0x%x,__va(i.u32Addr)=0x%x\n",i.u32Addr,i.u32Size,__va(i.u32Addr));
            Chip_Flush_Cache_Range((i.u32Addr), i.u32Size);
            #if 0
            __cpuc_flush_dcache_area((i.u32Addr),i.u32Size);
            outer_cache.flush_range(0x40400000,0xAA000000);
            outer_cache.inv_range(0x40400000,0xAA000000);
            dmac_map_area((i.u32Addr),i.u32Size,2);
            _chip_flush_miu_pipe();
            #endif

            #endif
#endif
        }
        break;
    case MPOOL_IOC_FLUSHDCACHE_PAVA:
        {
#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL)

            DrvMPool_Flush_Info_t i ;
            ret= copy_from_user(&i, (void __user *)arg, sizeof(i));
           /*Compare "u32AddrPhys" with "mmap_interval" to decide if located in MIU1 or not*/
           if(i.u32AddrPhys >= mmapData->mmap_interval)
               Chip_Flush_Cache_Range_VA_PA(i.u32AddrVirt, (i.u32AddrPhys - mmapData->mmap_interval) + ARM_MIU1_BUS_BASE , i.u32Size);
           else
               Chip_Flush_Cache_Range_VA_PA(i.u32AddrVirt, i.u32AddrPhys + ARM_MIU0_BUS_BASE , i.u32Size);
#endif
    	}
        break ;
    case MPOOL_IOC_GET_BLOCK_OFFSET:
        {
            DrvMPool_Info_t i ;
            ret= copy_from_user( &i, (void __user *)arg, sizeof(i) );
            MDrv_SYS_GetMMAP(i.u32Addr, &(i.u32Addr), &(i.u32Size)) ;
            ret= copy_to_user( (void __user *)arg, &i, sizeof(i) );
        }
        break ;
    case MPOOL_IOC_SET_MAP_CACHE:
        {
            ret= copy_from_user(&mmapData->u8MapCached, (void __user *)arg, sizeof(mmapData->u8MapCached));
        }
        break;
    case MPOOL_IOC_SET:
        {
            DrvMPool_Info_t i;
            ret= copy_from_user(&i, (void __user *)arg, sizeof(i));
            setflag = true;
            mmapData->mmap_offset = i.u32Addr;
            mmapData->mmap_size = i.u32Size;
            mmapData->mmap_interval = i.u32Interval;
            mmapData->mmap_miusel = i.u8MiuSel;
        }
        break;

	case MPOOL_IOC_KERNEL_DETECT:
			{
				DrvMPool_Kernel_Info_t i;
                		i.u32lxAddr = linux_base;
				i.u32lxSize = linux_size;
				i.u32lx2Addr = linux2_base;
				i.u32lx2Size = linux2_size;

                printk("lxaddr = %08x, lxsize = %08x\n", i.u32lxAddr, i.u32lxSize);
                printk("lx2addr = %08x, lx2size = %08x\n", i.u32lx2Addr, i.u32lx2Size);
				ret= copy_to_user( (void *)arg, &i, sizeof(i) );
			}
			break;
    case MPOOL_IOC_VERSION:
        {
            ret= copy_to_user( (void *)arg, &mpool_version, sizeof(mpool_version) );
        }
	    break;

    case MPOOL_IOC_FLUSHDCACHE_ALL:
    {
#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )    	
         Chip_Flush_Cache_All();
#endif
    }
    break ;

    default:
        printk("Unknown ioctl command %d\n", cmd);
        return -ENOTTY;
    }
    return 0;
}

#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA7) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2) || \
	defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_EMERALD)
static void _mod_mpool_map_miu1(void* p)
{
    unsigned long flags;

#if defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2) || \
	defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_EMERALD)
    unsigned long entrylo0 = ((0x60000000>> 12) << 6) | ((_page_cachable_default >> _CACHE_SHIFT) << 3) | (1<< 2) | (1<< 1) | (1<< 0); // cacheable
    unsigned long entrylo1 = ((0x60000000>> 12) << 6) | (2<<3) | (1<< 2) | (1<< 1) | (1<< 0); // un-cacheable or 0x7
#else
    unsigned long entrylo0 = ((0x40000000>> 12) << 6) | ((_page_cachable_default >> _CACHE_SHIFT) << 3) | (1<< 2) | (1<< 1) | (1<< 0); // cacheable
    unsigned long entrylo1 = ((0x40000000>> 12) << 6) | (2<<3) | (1<< 2) | (1<< 1) | (1<< 0); // un-cacheable or 0x7
#endif
    printk ("CPU%d: _mod_mpool_map_miu1: entrylo0=%08lx entrylo1=%08lx\n", smp_processor_id(), entrylo0, entrylo1);

    local_irq_save(flags);
    add_wired_entry(entrylo0, entrylo1, 0xC0000000, PM_256M);
    local_irq_restore(flags);
}
#endif // #if defined(CONFIG_MSTAR_XXXX)

//extern unsigned int MDrv_SYS_GetDRAMLength(void);

MSYSTEM_STATIC int __init mod_mpool_init(void)
{
    int s32Ret;
    dev_t dev;

    //MDrv_SYS_GetMMAP(E_SYS_MMAP_LINUX_BASE, &mpool_size, &mpool_base);
    //mpool_size = MDrv_SYS_GetDRAMLength()-mpool_base ;

    #if defined(CONFIG_MIPS)
    //get_boot_mem_info(LINUX_MEM, &linux_base, &linux_size);
    //get_boot_mem_info(LINUX_MEM2, &linux2_base, &linux2_size);
    //get_boot_mem_info(MPOOL_MEM, &mpool_base, &mpool_size);
    //get_boot_mem_info(EMAC_MEM, &emac_base, &emac_size);
    #elif defined(CONFIG_ARM)
    //add here later

    #endif

    //printk( "\nMpool base=0x%08X\n", mpool_base );
    //printk( "\nMpool size=0x%08X\n", mpool_size );

    mpool_class = class_create(THIS_MODULE, "mpool");
    if (IS_ERR(mpool_class))
    {
        return PTR_ERR(mpool_class);
    }

    if (MPoolDev.s32MPoolMajor)
    {
        dev = MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor);
        s32Ret = register_chrdev_region(dev, MOD_MPOOL_DEVICE_COUNT, MOD_MPOOL_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, MPoolDev.s32MPoolMinor, MOD_MPOOL_DEVICE_COUNT, MOD_MPOOL_NAME);
        MPoolDev.s32MPoolMajor = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        MPOOL_DPRINTK("Unable to get major %d\n", MPoolDev.s32MPoolMajor);
        class_destroy(mpool_class);
        return s32Ret;
    }

    cdev_init(&MPoolDev.cDevice, &MPoolDev.MPoolFop);
    if (0!= (s32Ret= cdev_add(&MPoolDev.cDevice, dev, MOD_MPOOL_DEVICE_COUNT)))
    {
        MPOOL_DPRINTK("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_MPOOL_DEVICE_COUNT);
        class_destroy(mpool_class);
        return s32Ret;
    }

#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA7) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2) || \
	defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_EMERALD)

//    _mod_mpool_map_miu1(NULL);
    #if defined(CONFIG_MIPS)
    on_each_cpu(_mod_mpool_map_miu1, NULL, 1);
    #elif defined(CONFIG_ARM)
    //add here later
    #endif
    // only for testing
    // *((unsigned int*) 0xC0000000) = 0x12345678;
    // printk("[%s][%d] 0x%08x\n", __FILE__, __LINE__, *((unsigned int*) 0xC0000000));
    // printk("[%s][%d] 0x%08x\n", __FILE__, __LINE__, *((unsigned int*) 0x80000000));
    // printk("[%s][%d] 0x%08x\n", __FILE__, __LINE__, *((unsigned int*) 0xA0000000));

#endif // #ifdef CONFIG_MSTAR_XXXX

    device_create(mpool_class, NULL, dev, NULL, MOD_MPOOL_NAME);
    #if defined(CONFIG_MIPS)
    bMPoolInit = 1;
    #endif
    return 0;
}


MSYSTEM_STATIC void __exit mod_mpool_exit(void)
{
    cdev_del(&MPoolDev.cDevice);
    unregister_chrdev_region(MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor), MOD_MPOOL_DEVICE_COUNT);

    device_destroy(mpool_class, MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor));
    class_destroy(mpool_class);
}
#if defined(CONFIG_MSTAR_MPOOL) || defined(CONFIG_MSTAR_MPOOL_MODULE)
module_init(mod_mpool_init);
module_exit(mod_mpool_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MPOOL driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MPOOL) || defined(CONFIG_MSTAR_MPOOL_MODULE)

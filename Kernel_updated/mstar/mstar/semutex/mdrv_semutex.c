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
/// file    mdrv_semutex.c
/// @brief  Memory Pool Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
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
#include <linux/time.h>  //added
#include <linux/device.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/cacheflush.h>
#include <linux/vmalloc.h>
#include <linux/version.h>



#if defined(CONFIG_MIPS)
#include <asm/mips-boards/prom.h>
#elif defined(CONFIG_ARM)
#endif
#include "mdrv_semutex_io.h"
#include "mst_devid.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
#include "chip_setup.h"
#endif

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#ifdef SEMUTEX_DEBUG
#define SEMUTEX_DPRINTK(fmt, args...)      printk("[Semutex (Driver)][%05d] " fmt, __LINE__, ## args)
#else
#define SEMUTEX_DPRINTK(fmt, args...)
#endif


//-------------------------------------------------------------------------------------------------
//  Global variables
//-------------------------------------------------------------------------------------------------
static DEFINE_MUTEX(semutex_sem);
static DEFINE_MUTEX(semutex_sharememory);
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MOD_SEMUTEX_DEVICE_COUNT     1
#define MOD_SEMUTEX_NAME             "semutex"

//for sharememory
#define SHAREDMEMORY_LEN       1024*1024

//for mutex
#define MUTEX_NAME_LEN  64
#define MUTEX_SUPPORTED 320
//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------;

typedef struct
{
    char name[MUTEX_NAME_LEN];
    bool     locked_status;                   //mutex reference count
    struct semaphore *sem;
    pid_t           locked_pid;                 //the pid this mutex locked by
    pid_t           locked_tgid;                 //the tgid this mutex locked by
    struct file *locked_file;
}mutex_info;

typedef struct
{
    struct page *               pg;
    struct list_head            list;
} share_page_node;

typedef struct
{
    int                         s32SemutexMajor;
    int                         s32SemutexMinor;

    int                         refcnt_sharemem;            //sharedmemory reference count
    //struct page **              pages;
    struct list_head            share_page_list;
    int                         length;
    int                         pageCount;
    bool                        isfirst_sharememory;

    mutex_info                  mutex_info[MUTEX_SUPPORTED];

    struct cdev                 cDevice;
    struct file_operations      SemutexFileop;
} SemutexModHandle;


typedef struct
{
    //for sharememory
    struct vm_area_struct   *vma;
    struct task_struct *tsk;
    struct file *createsharememory_filp;

    //for mutex
    //
    SemutexModHandle        *semutex;
} procinfo;

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int  _MDrv_SEMUTEX_Open (struct inode *inode, struct file *filp);
static int  _MDrv_SEMUTEX_Release(struct inode *inode, struct file *filp);
static int  _MDrv_SEMUTEX_MMap(struct file *filp, struct vm_area_struct *vma);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _MDrv_SEMUTEX_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int  _MDrv_SEMUTEX_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
static int  _MDrv_SEMUTEX_Flush(struct file *filp, fl_owner_t id);

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------

static struct class *semutex_class;

static SemutexModHandle SemutexDev=
{
    .s32SemutexMajor=               MDRV_MAJOR_SEMUTEX,
    .s32SemutexMinor=               MDRV_MINOR_SEMUTEX,
    .refcnt_sharemem               =                 0,
    //.pages              =                   NULL,
    .cDevice=
    {
        .kobj=                  {.name= MOD_SEMUTEX_NAME, },
        .owner  =               THIS_MODULE,
    },
    .SemutexFileop=
    {
        .open =                   _MDrv_SEMUTEX_Open,
        .release =                _MDrv_SEMUTEX_Release,
        .mmap =                   _MDrv_SEMUTEX_MMap,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
        .unlocked_ioctl=          _MDrv_SEMUTEX_Ioctl,
        #else
        .ioctl =                  _MDrv_SEMUTEX_Ioctl,
        #endif
        .flush =                  _MDrv_SEMUTEX_Flush,
    },
};



//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static int alloc_sharepage(SemutexModHandle *dev,int page_num)
{
    share_page_node* page_node = NULL;
    struct page * pg = NULL;
    struct list_head *p, *n;
    int i;
    SEMUTEX_DPRINTK("share page:allocated page \n");
    for(i = 0; i < page_num; i++)
    {
        page_node = kzalloc(sizeof(page_node), GFP_KERNEL);
        pg = alloc_page(GFP_HIGHUSER | __GFP_ZERO);
        if(pg == NULL || page_node == NULL)
        {
            printk(KERN_ERR "share page:allocated page fail!!! \n");
            goto allocated_fail;
        }
        page_node->pg = pg;
        SEMUTEX_DPRINTK("share page:allocated page pg=%X\n",(unsigned int)pg);
        SEMUTEX_DPRINTK("share page:add node to list i=%d\n",i);

        list_add_tail(&(page_node->list), &(dev->share_page_list));
    }
    
    return 0;
    
allocated_fail:
    
    list_for_each_safe(p, n,&(dev->share_page_list)) {
        page_node = list_entry(p, share_page_node, list);
        pg = page_node->pg;
        if(pg != NULL)
            __free_page(pg);
        list_del(p);
        kfree(page_node);
    }
    return -ENOMEM;
}

static int free_sharemem(SemutexModHandle *dev)
{
    share_page_node* page_node = NULL;
    struct page * pg = NULL;
    struct list_head *p, *n;
    list_for_each_safe(p, n,&(dev->share_page_list)) {
        page_node = list_entry(p, share_page_node, list);
        pg = page_node->pg;
        if(pg != NULL)
            __free_page(pg);
        list_del(p);
        kfree(page_node);
    }
    return 0;
}


struct page * append_onePage(SemutexModHandle *dev)
{
    share_page_node* page_node = NULL;
    struct page * pg = NULL;
    page_node = kzalloc(sizeof(page_node), GFP_KERNEL);
    pg = alloc_page(GFP_HIGHUSER | __GFP_ZERO);
    if(pg == NULL || page_node == NULL)
    {
        printk("share page:allocated page fail!!! \n");
        return NULL;
    }
    page_node->pg = pg;
    list_add_tail(&(page_node->list), &(dev->share_page_list));
    return pg;
}

static int expend_sharemem(procinfo * pprocinfo, SemutexModHandle *dev,int page_num)
{
    int i;
    int ret;
    unsigned long user_page_addr,start,end;
    struct page * pg = NULL;
    struct vm_area_struct *vma = NULL;
    struct mm_struct *mm = NULL;
    
    SEMUTEX_DPRINTK("share memory expend_sharemem!!! \n");
    if( pprocinfo->vma == NULL)
    {
        printk("share memory No MMAPPING!!! \n");
        return -1;
    }
    else
    {
        mm = get_task_mm(pprocinfo->tsk);
        if (mm) {
            down_write(&mm->mmap_sem);
        }
        vma = pprocinfo->vma;
    }
    
    start = vma->vm_start;
    end   = vma->vm_end;
    if(end < start + ((dev->pageCount + page_num) << PAGE_SHIFT))
    {
        printk("not enough virtual address space!!! \n");
        return -1;
    }
    
    user_page_addr = start + (dev->pageCount << PAGE_SHIFT);
    for(i = 0; i < page_num; i++)
    {
        pg = append_onePage(dev);
        if(pg == NULL)
            return -ENOMEM;
        SEMUTEX_DPRINTK( "share page:expend_sharemem page=%X, i=%d, addr = %X \n",(unsigned int)pg, i, (unsigned int)user_page_addr);
        
    	ret = vm_insert_page(vma, user_page_addr, pg);
    	if (ret) {
    		printk(KERN_ERR "vm_insert_page fail. ret1 = %d \n",ret);
    		goto err_no_vma;
    	}
        else
        {
            user_page_addr+= PAGE_SIZE;
        }
        
    }
    
    if (mm) {
    	up_write(&mm->mmap_sem);
    	mmput(mm);
    }
    return 0;

err_no_vma:
    if (mm) {
    	up_write(&mm->mmap_sem);
    	mmput(mm);
    }
    return -ENOMEM;
}

#if 0
static int alloc_sharemem(SemutexModHandle *dev,int len)
{
    struct page **page;
    int i,j;
    for(i = 0 ; i < len / PAGE_SIZE; i ++)
    {
        page = &dev->pages[i];
        *page = alloc_page(GFP_HIGHUSER | __GFP_ZERO);
        if(*page == NULL)
        {
            printk("allocated page fail!!! \n");
            goto allocated_fail;
        }
    }

    return 0;

allocated_fail:
    for(j = 0 ; j < i ; j ++)
    {
        page = &dev->pages[i];
        __free_page(*page);
    }
    return -ENOMEM;
}
#endif

static int _Cleanup_Mutex(struct file *filp,SemutexModHandle *dev)
{
    int i;

    for(i = 0 ; i < MUTEX_SUPPORTED ; i ++)
    {
        if(i < 40)
            SEMUTEX_DPRINTK(" i=%d, mutex name = %s ,status = %d,lockedpid=%d,locked_tgid=%d,pid=%d,tgid = %d\n",
            i,dev->mutex_info[i].name,dev->mutex_info[i].locked_status,dev->mutex_info[i].locked_pid,dev->mutex_info[i].locked_tgid,current->pid,current->tgid);

        if(dev->mutex_info[i].locked_tgid == current->tgid)
        {//the process which locked the mutex cleanup(probably terminated un-normally)
            BUG_ON(dev->mutex_info[i].locked_status == false);
            up(dev->mutex_info[i].sem);

            mutex_lock(&semutex_sem);
            dev->mutex_info[i].locked_status = false;
            dev->mutex_info[i].locked_pid = 0;
            dev->mutex_info[i].locked_tgid = 0;
            dev->mutex_info[i].locked_file = NULL;
            mutex_unlock(&semutex_sem);
            SEMUTEX_DPRINTK("@@@@@@@@@@@@@@ i=%d, mutex name = %s \n",i,dev->mutex_info[i].name);
        }
    }

    return 0;
}

static int  _MDrv_SEMUTEX_Flush(struct file *filp, fl_owner_t id)
{
    procinfo *pprocinfo = filp->private_data;
    SemutexModHandle *dev =  pprocinfo->semutex;

    if(!(current->flags & PF_EXITING))      //do not the exit case,
    {
        SEMUTEX_DPRINTK("do not the exit case, flags = %x \n",current->flags);
        return 0;
    }

    SEMUTEX_DPRINTK("current->flags = %x , pid = %d , tgid = %d \n",current->flags, current->pid,current->tgid);
    return _Cleanup_Mutex(filp,dev);
}

static int _MDrv_SEMUTEX_Open (struct inode *inode, struct file *filp)
{
    SemutexModHandle *dev;

//!!! must asume that one process only open the device once!
    procinfo *pprocinfo = (procinfo *)kzalloc(sizeof(procinfo),GFP_KERNEL);

    dev = container_of(inode->i_cdev, SemutexModHandle, cDevice);

    pprocinfo->semutex = dev;
    pprocinfo->tsk = current;
    pprocinfo->createsharememory_filp  = NULL;

    filp->private_data = pprocinfo;

    return 0;
}

static int _MDrv_SEMUTEX_Release(struct inode *inode, struct file *filp)
{
    procinfo *pprocinfo = filp->private_data;
    SemutexModHandle *dev =  pprocinfo->semutex;
    int i = 0;

    BUG_ON(dev->refcnt_sharemem < 0);
    //if open, don't map , then close, dev->refcnt_sharemem will be 0, it is allowed!
    //But in this case , pprocinfo->createsharememory_filp should be NULL.
    //We bug on the case where dev->refcnt_sharemem == 0 and pprocinfo->createsharememory_filp is not NULL
    //(which means that it has been mapped )
    BUG_ON((dev->refcnt_sharemem == 0) && (pprocinfo->createsharememory_filp));

    if(pprocinfo->createsharememory_filp)   //this means it has create the sharememory before.
    {
        mutex_lock(&semutex_sharememory);
        dev->refcnt_sharemem --;
        SEMUTEX_DPRINTK("[SEMUTEX] in release dev->refcnt_sharemem = %d \n",dev->refcnt_sharemem);

        if(0 == dev->refcnt_sharemem)
        {
            //struct page **page1;
            SEMUTEX_DPRINTK("[SEMUTEX] semutex refcnt_sharemem drops to 0.\n");
            free_sharemem(dev);
#if 0
            for(i = 0 ; i < dev->length / PAGE_SIZE ; i ++)
            {
                page1 = &dev->pages[i];
                __free_page(*page1);
            }

            kfree(dev->pages);
            dev->pages = NULL;
#endif
        }
        pprocinfo->createsharememory_filp = NULL;
        mutex_unlock(&semutex_sharememory);
    }
    //mutex clean up??
    for(i = 0 ; i < MUTEX_SUPPORTED ; i ++)
    {
        if(dev->mutex_info[i].locked_file == filp)
        {//the process which locked the mutex cleanup(probably terminated un-normally)
            BUG_ON(dev->mutex_info[i].locked_status == false);
            up(dev->mutex_info[i].sem);

            mutex_lock(&semutex_sem);
            dev->mutex_info[i].locked_status = false;
            dev->mutex_info[i].locked_file = NULL;
            mutex_unlock(&semutex_sem);
            printk(KERN_EMERG" i=%d, mutex name = %s \n",i,dev->mutex_info[i].name);
            //break;
        }
    }


    kfree(pprocinfo);

    return 0;
}



static int semutex_update_page_range(procinfo * pprocinfo, int allocate,
				    unsigned long start, unsigned long end,
				    struct vm_area_struct *vma)
{
    unsigned long user_page_addr;
    SemutexModHandle *dev =  pprocinfo->semutex;
    struct page *page;
    struct mm_struct *mm;
    share_page_node *page_node = NULL;
    struct list_head *p;
    //int index = 0;

    if (end <= start)
    	return 0;

    if (vma)
    	mm = NULL;
    else
    	mm = get_task_mm(pprocinfo->tsk);

    if (mm) {
    	down_write(&mm->mmap_sem);
    	vma = pprocinfo->vma;
    }

    if (allocate == 0)
    	goto free_range;

    if (vma == NULL) {
    	printk(KERN_ERR "no vma\n");
    	goto err_no_vma;
    }

#if 1
    user_page_addr = start;
    list_for_each(p,&(dev->share_page_list)) {
    	int ret;

        if(user_page_addr >= end) 
        {
            break;
        }
        page_node = list_entry(p, share_page_node, list);
        page = page_node->pg;
    	BUG_ON(NULL == page);
        SEMUTEX_DPRINTK("share page:vm_insert_page addr=%lX, %lX, page:%X\n",user_page_addr,end,(unsigned int)page);

    	ret = vm_insert_page(vma, user_page_addr, page);
    	if (ret) {
    		printk(KERN_ERR "vm_insert_page fail. ret1 = %d \n",ret);
    		goto err_no_vma;
    	}
        else
        {
            user_page_addr += PAGE_SIZE;
        }
    }
#else

    for (user_page_addr = start; user_page_addr < end; user_page_addr += PAGE_SIZE) {
    	int ret;
    	page = &dev->pages[index++];

    	BUG_ON(NULL == *page);

    	ret = vm_insert_page(vma, user_page_addr, page[0]);
    	if (ret) {
    		printk(KERN_ERR "vm_insert_page fail. ret1 = %d \n",ret);
    		goto err_no_vma;
    	}
    	/* vm_insert_page does not seem to increment the refcount */
    }
#endif
    if (mm) {
    	up_write(&mm->mmap_sem);
    	mmput(mm);
    }
    return 0;

free_range:
    for (user_page_addr = end - PAGE_SIZE; user_page_addr >= start;user_page_addr -= PAGE_SIZE)
    {
        if (vma)
            zap_page_range(vma, user_page_addr, PAGE_SIZE, NULL);
    }
err_no_vma:
    if (mm) {
    	up_write(&mm->mmap_sem);
    	mmput(mm);
    }
    return -ENOMEM;
}

static int semutex_vma_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    SEMUTEX_DPRINTK("semutex_vma_fault !!! \n");
    BUG();
    return -ENOMEM;
}

static void semutex_vma_close(struct vm_area_struct *vma)
{
    unsigned int len;
    procinfo *pprocinfo = vma->vm_private_data;
    //SemutexModHandle *dev = pprocinfo->semutex;
    len = vma->vm_end - vma->vm_start;

    //unmap it
    semutex_update_page_range(pprocinfo, 0, vma->vm_start, vma->vm_end, vma);
}


static struct vm_operations_struct semutex_vm_ops = {
	//.open = semutex_vma_open,
	.close = semutex_vma_close,
	.fault = semutex_vma_fault,
};

static int  _MDrv_SEMUTEX_MMap(struct file *filp, struct vm_area_struct *vma)
{
    int ret = 0;
    unsigned int len;
    procinfo *pprocinfo = filp->private_data;

    //SemutexModHandle *dev = pprocinfo->semutex;

    if ((vma->vm_end - vma->vm_start) > SHAREDMEMORY_LEN)
        vma->vm_end = vma->vm_start + SHAREDMEMORY_LEN;

    len = vma->vm_end - vma->vm_start;

    vma->vm_ops = &semutex_vm_ops;
    vma->vm_private_data = pprocinfo;

    if (semutex_update_page_range(pprocinfo, 1, vma->vm_start, vma->vm_end,vma))
    {
    	ret = -ENOMEM;
    	goto err_map_failed;
    }

    pprocinfo->vma = vma;

    //
    //strcpy(pprocinfo->sharedmemroy,"hello world from linux kernel!!!!!!");
    //printk("~!~mmaped pprocinfo->sharedmemroy = %p \n",pprocinfo->sharedmemroy);

err_map_failed:
    return ret;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _MDrv_SEMUTEX_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _MDrv_SEMUTEX_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int err= 0,i;
    procinfo *pprocinfo = filp->private_data;
    SemutexModHandle *dev = pprocinfo->semutex;

    /*
    * extract the type and number bitfields, and don't decode
    * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
    */
    if (SEMUTEX_IOC_MAGIC!= _IOC_TYPE(cmd))
        return -ENOTTY;

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
        return -EFAULT;

    // @FIXME: Use a array of function pointer for program readable and code size later
    switch(cmd)
    {
        case MDRV_SEMUTEX_CREATE_MUTEX:
            {
                int first_unused = -1,ret_index;
                char namefromuser[MUTEX_NAME_LEN];
                if(copy_from_user(namefromuser, (void __user *)arg, MUTEX_NAME_LEN))
                {
                    printk(KERN_EMERG"create mutex: copy from user error!\n");
                    ret_index = -ENOENT;
                    goto return_index;
                }

                mutex_lock(&semutex_sem);
                for(i = 0 ; i < MUTEX_SUPPORTED; i ++)
                {
                    //find by name

                    if(strncmp(namefromuser,dev->mutex_info[i].name, MUTEX_NAME_LEN) == 0)
                    {
                        ret_index = MUTEX_INDEX_BEGIN + i;
                        goto return_index;
                    }

                    //if the slot not used (we assue that no mutex named with ZERO length)
                    if(first_unused == -1 && strlen(dev->mutex_info[i].name) == 0)
                        first_unused = i;
                }

                if(first_unused == -1)
                {
                    printk("[SEMUTEX] not enough mutex slot... \n");
                    ret_index = -ENOENT;
                    goto return_index;
                }
                strncpy(dev->mutex_info[first_unused].name,namefromuser,MUTEX_NAME_LEN);
                if(dev->mutex_info[first_unused].sem == NULL)
                {
                    dev->mutex_info[first_unused].sem = kmalloc(sizeof(struct semaphore),GFP_KERNEL);
                    if(dev->mutex_info[first_unused].sem == NULL)
                    {
                        printk("[SEMUTEX] not enough memory for mutex-sem \n");
                        ret_index = -ENOMEM;
                        goto return_index;
                    }
                    sema_init(dev->mutex_info[first_unused].sem,1);
                }
                ret_index = MUTEX_INDEX_BEGIN + first_unused;
return_index:
                mutex_unlock(&semutex_sem);
                if(ret_index > 0)
                    SEMUTEX_DPRINTK("[SEMUTEX] i = %d, create mutex name =%s, ret index = %d \n",i,dev->mutex_info[ret_index - MUTEX_INDEX_BEGIN].name,ret_index);
                return ret_index;
            }
            break;
        case MDRV_SEMUTEX_LOCK_WITHTIMEOUT:
            {
                LOCKARG lockarg;
                int res;
                //printk("@@@@@@@@@@ semutex ioctl cmd MDRV_SEMUTEX_LOCK_WITHTIMEOUT = %d \n",MDRV_SEMUTEX_LOCK_WITHTIMEOUT);
                if (copy_from_user(&lockarg, (void __user *)arg, sizeof(LOCKARG)))
                {
                    printk("[SEMUTEX] get  lock timeout  parameter failed! \n");
                    return -1;
                }

                SEMUTEX_DPRINTK("[SEMUTEX] index %d name %s  lock with timeout \n",lockarg.index,dev->mutex_info[lockarg.index - MUTEX_INDEX_BEGIN].name);

                lockarg.index -= (int)MUTEX_INDEX_BEGIN;
                BUG_ON(lockarg.index < 0);
                BUG_ON(lockarg.index > MUTEX_SUPPORTED);
                BUG_ON(dev->mutex_info[lockarg.index].sem == NULL);

                res = down_timeout(dev->mutex_info[lockarg.index].sem, msecs_to_jiffies(lockarg.time));
                if(res == -ETIME)
                {
                    printk("[SEMUTEX] mutex %s timeout with %d ms \n",dev->mutex_info[lockarg.index].name,lockarg.time);
                }
                else if(res == 0)
                {
                    mutex_lock(&semutex_sem);
                    dev->mutex_info[lockarg.index].locked_status = true;
                    dev->mutex_info[lockarg.index].locked_pid = current->pid;
                    dev->mutex_info[lockarg.index].locked_tgid = current->tgid;
                    dev->mutex_info[lockarg.index].locked_file = filp;
                    mutex_unlock(&semutex_sem);
                }
                else
                    printk("unknown error %d \n",res);
                SEMUTEX_DPRINTK("[SEMUTEX] lock with timeout  return with res %d \n",res);
                return res;
            }
            break;
        case MDRV_SEMUTEX_TRY_LOCK:
            {
                int index,flag;
                err = get_user(index, (int __user *)arg);
                if (err)
                	return -1;

                index -= (int)MUTEX_INDEX_BEGIN;

                BUG_ON(index < 0);
                BUG_ON(index > MUTEX_SUPPORTED);
                BUG_ON(dev->mutex_info[index].sem == NULL);
                flag = down_trylock(dev->mutex_info[index].sem);
                //~!~ may have problem here!!!!

                SEMUTEX_DPRINTK("[SEMUTEX] trylock with index =%d, name = %s \n",index,dev->mutex_info[index].name);
                if(flag == 0)
                {//already locked!!!
                    mutex_lock(&semutex_sem);
                    dev->mutex_info[index].locked_status = true;
                    dev->mutex_info[index].locked_pid = current->pid;
                    dev->mutex_info[index].locked_tgid = current->tgid;
                    dev->mutex_info[index].locked_file = filp;
                    mutex_unlock(&semutex_sem);
                }
                SEMUTEX_DPRINTK("[SEMUTEX] trylock return with res %d \n",flag);
                return flag;
            }
            break;
        case MDRV_SEMUTEX_LOCK:
            {
                int index,ret;
                err = get_user(index, (int __user *)arg);
                if (err)
                	return -1;
                //printk(KERN_EMERG"~!~! lock with index =%d \n",index);
                index -= (int)MUTEX_INDEX_BEGIN;
                BUG_ON(index < 0);
                BUG_ON(index > MUTEX_SUPPORTED);
                BUG_ON(dev->mutex_info[index].sem == NULL);
/*
                int res = down_timeout(dev->mutex_info[index].sem, msecs_to_jiffies(1000 * 30));
                if(res == -ETIME)
                {
                    printk("~!~ mutex %s timeout ,index = %d \n",dev->mutex_info[index].name,index);
                    dump_stack();
                }
*/
#if 0
                down(dev->mutex_info[index].sem);

#else
                ret = down_interruptible(dev->mutex_info[index].sem);
                if(ret == -EINTR)
                    return -EINTR;
#endif
                mutex_lock(&semutex_sem);
                dev->mutex_info[index].locked_status = true;
                dev->mutex_info[index].locked_pid = current->pid;
                dev->mutex_info[index].locked_tgid = current->tgid;
                dev->mutex_info[index].locked_file = filp;
                mutex_unlock(&semutex_sem);

                return 0;
            }
            break;
        case MDRV_SEMUTEX_UNLOCK:
            {
                int index;
                err = get_user(index, (int __user *)arg);
                if (err)
                	return -1;
                //printk(KERN_EMERG"~!~! unlock index %d \n",index);

                index -= (int)MUTEX_INDEX_BEGIN;
                BUG_ON(index < 0);
                BUG_ON(index > MUTEX_SUPPORTED);
                BUG_ON(dev->mutex_info[index].sem == NULL);

                //printk("~!~! index %d ,lock_status=%d \n",index,dev->mutex_info[index].locked_status);


                if(dev->mutex_info[index].locked_status == false)
                {
                    printk(KERN_EMERG"WARNING1!Try to unlock mutex index %d name %s, but it is not in lock status!!! \n",index,dev->mutex_info[index].name);
                    return -1;
                }
                if(dev->mutex_info[index].locked_pid != current->pid)
                {
                    printk(KERN_EMERG"WARNING2!Try to unlock mutex index %d name %s with pid %d , but not in the lock pid %d\n",
                    index,dev->mutex_info[index].name,dev->mutex_info[index].locked_pid,current->pid);
                    return -1;
                }
                if(dev->mutex_info[index].locked_file != filp)
                {
                    printk(KERN_EMERG"WARNING!Try to unlock mutex index %d name %s, but use wrong filp!!! \n",index,dev->mutex_info[index].name);
                    return -1;
                }

                mutex_lock(&semutex_sem);
                dev->mutex_info[index].locked_file = NULL;
                dev->mutex_info[index].locked_status = false;
                dev->mutex_info[index].locked_pid = 0;
                dev->mutex_info[index].locked_tgid = 0;
                mutex_unlock(&semutex_sem);

                up(dev->mutex_info[index].sem);

                return 0;
            }
            break;

        case MDRV_SEMUTEX_DEL_MUTEX:
            {
                int index;
                err = get_user(index, (int __user *)arg);
                if (err)
                	return -1;

                index -= (int)MUTEX_INDEX_BEGIN;
                BUG_ON(index < 0);
                BUG_ON(index > MUTEX_SUPPORTED);
                BUG_ON(dev->mutex_info[index].sem == NULL);
                //BUG_ON(dev->mutex_info[index].locked_status == false);

            }
            break;
        case MDRV_SEMUTEX_QUERY_ISFIRST_SHAREMEMORY:
            {
                return dev->isfirst_sharememory;
            }
            break;
        case MDRV_SEMUTEX_CREATE_SHAREMEMORY:
            {
                int len,err;
                int page_num;
                SEMUTEX_DPRINTK( " Try to create sharememory !!!! \n");
                if(filp == pprocinfo->createsharememory_filp)
                {
                    printk("WARNING! Try to create sharememory again! \n");
                    return -1;
                }
                err = get_user(len, (int __user *)arg);
                if (err)
                	return -1;

                SEMUTEX_DPRINTK("create sharememory with len %d \n",len);
                if(len > SHAREDMEMORY_LEN)
                {
                    printk("the required length %d exceed the avaiable len   %d\n",len,SHAREDMEMORY_LEN);
                    return -1;
                }

                SEMUTEX_DPRINTK( " Try to create sharememory len=%d !!!! \n",len);
                len += PAGE_SIZE - 1;
                page_num = (len>> PAGE_SHIFT);
                len = (len>> PAGE_SHIFT)<< PAGE_SHIFT; // make it 4KBytes alignment
                //
                mutex_lock(&semutex_sharememory);

           //     printk("[SEMUTEX] in create dev->refcnt_sharemem = %d \n",dev->refcnt_sharemem);

                if(dev->refcnt_sharemem == 0)
                {
                    SEMUTEX_DPRINTK("~!~!~ re-allocate memory!!! \n");
#if 0
                    dev->pages = kzalloc(sizeof(dev->pages[0]) * (len / PAGE_SIZE), GFP_KERNEL);
                    if(dev->pages == NULL)
                    {
                        printk("not enough memory for pages[] \n");
                        mutex_unlock(&semutex_sharememory);
                        return -1;
                    }
                    err = alloc_sharemem(dev,len);
#endif
                    SEMUTEX_DPRINTK("DEBUG! Try to create sharememory pn=%d !!!! \n",page_num);
                    err = alloc_sharepage(dev, page_num);
                    if(err)
                    {
                        mutex_unlock(&semutex_sharememory);
                    //    printk("[SEMUTEX] alloc fail!!!!! dev->refcnt_sharemem = %d \n",dev->refcnt_sharemem);
                        return -1;
                    }
                    dev->length = len;
                    dev->pageCount = page_num;
                    dev->isfirst_sharememory = true;
                }
                else
                    dev->isfirst_sharememory = false;
                dev->refcnt_sharemem ++;
               // printk("[SEMUTEX] dev->refcnt_sharemem = %d \n",dev->refcnt_sharemem);

                pprocinfo->createsharememory_filp = filp;

                mutex_unlock(&semutex_sharememory);

                return 0;
            }
            break ;
        case MDRV_SEMUTEX_EXPAND_SHAREMEMORY:
            {
                int len,err;
                int page_num;
                int expand_num;
                err = get_user(len, (int __user *)arg);
                if (err)
                	return -1;

                SEMUTEX_DPRINTK("expand sharememory with len %d \n",len);
                if(len > SHAREDMEMORY_LEN)
                {
                    printk("the required length %d exceed the avaiable len   %d\n",len,SHAREDMEMORY_LEN);
                    return -1;
                }

                if( len <= dev->length)
                {
                    printk("the required length %d shorter the current len   %d\n",len,dev->length);
                    return -1;
                }

                SEMUTEX_DPRINTK( "DEBUG! Try to expand sharememory len=%d !!!! \n",len);
                len += PAGE_SIZE - 1;
                page_num = (len>> PAGE_SHIFT);
                len = (len>> PAGE_SHIFT)<< PAGE_SHIFT; // make it 4KBytes alignment
                //
                mutex_lock(&semutex_sharememory);

                //printk("~!~!in create dev->refcnt_sharemem = %d \n",dev->refcnt_sharemem);

                if(dev->refcnt_sharemem == 0)
                {
                    SEMUTEX_DPRINTK("~!~!~ uninitialized share memory!!! \n");
                    mutex_unlock(&semutex_sharememory);
                    return -1;
                }
                expand_num = page_num - dev->pageCount;
                SEMUTEX_DPRINTK( "DEBUG! Try to expand sharememory pn=%d !!!! \n",expand_num);
                err = expend_sharemem(pprocinfo, dev, expand_num);
                if(err)
                {
                    mutex_unlock(&semutex_sharememory);
                    return -1;
                }
                 dev->length = len;
                 dev->pageCount = page_num;
                //printk("~!~! dev->refcnt_sharemem = %d \n",dev->refcnt_sharemem);

                //pprocinfo->createsharememory_filp = filp;

                mutex_unlock(&semutex_sharememory);

                return 0;
            }
            break;

        default:
        printk("Unknown ioctl command %d\n", cmd);
        return -ENOTTY;
    }
    return 0;
}

//extern unsigned int MDrv_SYS_GetDRAMLength(void);

/*
struct timer_list gtimer;

void enum_timer_fn(unsigned long arg)
{
    struct task_struct * p;
    printk("timer in NTFS3G~\n");

    for_each_process(p) {

        printk("<<<<<<<<<<<<<<<<<<<<<<<<<< \n");
        //if (task_pid_vnr(p) > 1)
        sched_show_task(p);
        printk(">>>>>>>>>>>>>>>>>>>>>>>>>> \n");

    }
    mod_timer(&gtimer,jiffies + HZ*30);
}
*/

MSYSTEM_STATIC int __init mod_semutex_init(void)
{
    int s32Ret,i;
    dev_t dev;

    semutex_class = class_create(THIS_MODULE, "semutex");
    if (IS_ERR(semutex_class))
    {
        return PTR_ERR(semutex_class);
    }
    for(i = 0 ; i < MUTEX_SUPPORTED ; i ++)
    {
        memset(&SemutexDev.mutex_info[i],0,sizeof(SemutexDev.mutex_info[i]));
    }

    if (SemutexDev.s32SemutexMajor)
    {
        dev = MKDEV(SemutexDev.s32SemutexMajor, SemutexDev.s32SemutexMinor);
        s32Ret = register_chrdev_region(dev, MOD_SEMUTEX_DEVICE_COUNT, MOD_SEMUTEX_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, SemutexDev.s32SemutexMinor, MOD_SEMUTEX_DEVICE_COUNT, MOD_SEMUTEX_NAME);
        SemutexDev.s32SemutexMajor = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        class_destroy(semutex_class);
        return s32Ret;
    }

    cdev_init(&SemutexDev.cDevice, &SemutexDev.SemutexFileop);
    if (0!= (s32Ret= cdev_add(&SemutexDev.cDevice, dev, MOD_SEMUTEX_DEVICE_COUNT)))
    {
        unregister_chrdev_region(dev, MOD_SEMUTEX_DEVICE_COUNT);
        class_destroy(semutex_class);
        return s32Ret;
    }
    //init share page list, add by austin
    INIT_LIST_HEAD(&(SemutexDev.share_page_list));
    device_create(semutex_class, NULL, dev, NULL, MOD_SEMUTEX_NAME);


/*
    init_timer(&gtimer);
    gtimer.data = 0;
    gtimer.function = enum_timer_fn;
    gtimer.expires = jiffies + HZ*30;
    mod_timer(&gtimer,jiffies + HZ*30);
*/

    return 0;
}


MSYSTEM_STATIC void __exit mod_semutex_exit(void)
{
    cdev_del(&SemutexDev.cDevice);
    unregister_chrdev_region(MKDEV(SemutexDev.s32SemutexMajor, SemutexDev.s32SemutexMinor), MOD_SEMUTEX_DEVICE_COUNT);

    device_destroy(semutex_class, MKDEV(SemutexDev.s32SemutexMajor, SemutexDev.s32SemutexMinor));
    class_destroy(semutex_class);
}

module_init(mod_semutex_init);
module_exit(mod_semutex_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("Semutex driver");
MODULE_LICENSE("GPL");

#define _MDRV_G3D_C_

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#include "mst_devid.h"
#include "mdrv_types.h"

#include "mdrv_g3d.h"
#include "mdrv_g3d_io.h"
#include "mdrv_g3d_st.h"

#include "mhal_g3d.h"

/* ---------------------------------------------------------------------- */
#undef OPT_G3D_CMD_SEND_COUNT_DEBUG
#undef OPT_G3D_DRV_DEBUG
#undef G3D_DRV_DBG
#ifdef OPT_G3D_DRV_DEBUG
    #define G3D_DRV_DBG(fmt, args...)  printk(KERN_WARNING "[G3D_DRV][%05d]" fmt, __LINE__, ## args)
#else
    #define G3D_DRV_DBG(fmt, args...)
#endif

#undef G3D_DRV_DBGX
#define G3D_DRV_DBGX(fmt, args...)
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
#define MDRV_NAME_G3D                 "g3d"
#define MDRV_MAJOR_G3D                0xff
#define MDRV_MINOR_G3D                0x00
#define G3D_DEVICE_COUNT              1
/* ---------------------------------------------------------------------- */

static U32 _u32G3DMajor = MDRV_MAJOR_G3D;
static U32 _u32G3DMinor = MDRV_MINOR_G3D;

static struct cdev _stG3DDev;

typedef struct _G3DFilpArray
{
    unsigned int u32Filp;
#ifdef OPT_G3D_CMD_SEND_COUNT_DEBUG
    unsigned int u32CmdSendCount;
#endif
} G3DFilpArray, *pG3DFilpArray;

#define MAX_FILP 32
static G3DFilpArray _u32G3DFilpArray[MAX_FILP];
static rwlock_t _g3dFilpArrayLock = RW_LOCK_UNLOCKED;

static int _u32LockForPNG  = 0;

static int _u32G3DInitDone = 0;
static DECLARE_MUTEX(_g3dInitLock);

/* ---------------------------------------------------------------------- */
static int setFilp(int filp)
{
    int i;

    read_lock(&_g3dFilpArrayLock);
    for (i = 0; i < MAX_FILP; ++i)
    {
        if (_u32G3DFilpArray[i].u32Filp == filp)
        {
            read_unlock(&_g3dFilpArrayLock);
            return i;
        }
    }

    read_unlock(&_g3dFilpArrayLock);
    write_lock(&_g3dFilpArrayLock);

    for (i = 0; i < MAX_FILP; ++i)
    {
        if (_u32G3DFilpArray[i].u32Filp == -1)
        {
            _u32G3DFilpArray[i].u32Filp = filp;
            write_unlock(&_g3dFilpArrayLock);
            printk(KERN_INFO "[G3D] ID = %d (0x%x)\n",i, filp);
            return i;
        }
    }

    write_unlock(&_g3dFilpArrayLock);
    return -1;
}

static int getFilpId(int filp)
{
    int i;

    read_lock(&_g3dFilpArrayLock);

    for (i = 0; i < MAX_FILP; ++i)
    {
        if (_u32G3DFilpArray[i].u32Filp == filp)
        {
            read_unlock(&_g3dFilpArrayLock);
            return i;
        }
    }

    read_unlock(&_g3dFilpArrayLock);
    return -1;
}

static int removeFilp(unsigned int filp)
{
    int i;

    write_lock(&_g3dFilpArrayLock);

    for (i = 0; i < MAX_FILP; ++i)
    {
        if (_u32G3DFilpArray[i].u32Filp == filp)
        {
            _u32G3DFilpArray[i].u32Filp = -1;
#ifdef OPT_G3D_CMD_SEND_COUNT_DEBUG
            _u32G3DFilpArray[i].u32CmdSendCount = 0;
#endif
            write_unlock(&_g3dFilpArrayLock);
            return i;
        }
    }

    write_unlock(&_g3dFilpArrayLock);
    return -1;
}

#ifdef OPT_G3D_CMD_SEND_COUNT_DEBUG
static int increaseConut(int filp)
{
    int i;

    write_lock(&_g3dFilpArrayLock);

    for (i = 0; i < MAX_FILP; ++i)
    {
        if (_u32G3DFilpArray[i].u32Filp == filp)
        {
            _u32G3DFilpArray[i].u32CmdSendCount++;
            write_unlock(&_g3dFilpArrayLock);
            return _u32G3DFilpArray[i].u32CmdSendCount;
        }
    }

    for (i = 0; i < MAX_FILP; ++i)
    {
        if (_u32G3DFilpArray[i].u32Filp == -1)
        {
            _u32G3DFilpArray[i].u32Filp = filp;
            _u32G3DFilpArray[i].u32CmdSendCount=1;
            write_unlock(&_g3dFilpArrayLock);
            return _u32G3DFilpArray[i].u32CmdSendCount;
        }
    }

    write_unlock(&_g3dFilpArrayLock);
    return -1;
}
#endif
/* ---------------------------------------------------------------------- */
static int _MDrv_G3D_Open(struct inode *inode, struct file *filp)
{
#if 0
    int i;
    for (i = 0; i < MaxTask; ++i)
        _u32G3DTaskArray[i] = -1;
#endif
    setFilp((unsigned int)filp);
    filp->private_data = MHal_G3D_Data_Alloc();
    //_u32G3DInitDone = 0;
    return 0;
}

static int _MDrv_G3D_Release(struct inode *inode, struct file *filp)
{
    //_u32G3DInitDone = 0;
    removeFilp((unsigned int)filp);
    if (filp->private_data)
        MHal_G3D_Data_Free((struct g3d_data*)filp->private_data);

    return 0;
}

static int _MDrv_G3D_Ioctl(struct inode *inode, struct file *filp,
                           unsigned int cmd, unsigned long arg)
{
    int err = 0;
    /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
    if (_IOC_TYPE(cmd) != G3D_IOC_MAGIC)
        return -ENOTTY;

#if 0
    if (_IOC_NR(cmd) > G3D_IOC_MAXNR)
        return -ENOTTY;
#endif

    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *) arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err = !access_ok(VERIFY_READ, (void __user *) arg, _IOC_SIZE(cmd));
    }
    if (err)
    {
        return -EFAULT;
    }

    G3D_DRV_DBG("_MDrv_G3D_Ioctl cmd 0x%x \n",cmd);
    switch(cmd)
    {
        case G3D_IOC_INIT:
            if (0 != down_interruptible(&_g3dInitLock))
                return -1;

            if (0 == _u32G3DInitDone)
            {
                if (0 == MHal_G3D_Init())
                {
                    up(&_g3dInitLock);
                    return -1;
                }

                _u32G3DInitDone = 1;
            }

            up(&_g3dInitLock);
            break;
        case G3D_IOC_CMDQ_RESET:
            if (0 != MHal_G3D_CmdQ_Reset())
                return -1;

            break;
        case G3D_IOC_PNGCMDQ_SEND:
        case G3D_IOC_CMDQ_SEND:
            {
                G3D_CMDQ_BUFFER buffer;
#ifdef OPT_G3D_CMD_SEND_COUNT_DEBUG
                int count;
#endif

                if (copy_from_user(&buffer, (G3D_CMDQ_BUFFER __user *) arg, sizeof(G3D_CMDQ_BUFFER)))
                    return -EFAULT;

                if (((_u32LockForPNG==0)&&(cmd ==G3D_IOC_CMDQ_SEND)) ||  //g3d case
                    ((_u32LockForPNG==1)&&(cmd ==G3D_IOC_PNGCMDQ_SEND)) ) //png/g3d case
                {
                    buffer.ret_status = MHal_G3D_CmdQ_Send(buffer.size, buffer.pCmdBuffer, &buffer.ret_sw_stamp);
#ifdef OPT_G3D_CMD_SEND_COUNT_DEBUG
                    count = increaseConut((unsigned int)filp);
                    if ((count%800)==0)
                    {
                        printk("G3D_IOC_CMDQ_SEND count = %d, filp = %p\n",count, filp);
                        printk("_u32G3DFilpArray[0].u32Filp = 0x%x\n",_u32G3DFilpArray[0].u32Filp);
                        printk("_u32G3DFilpArray[1].u32Filp = 0x%x\n",_u32G3DFilpArray[1].u32Filp);
                        printk("_u32G3DFilpArray[2].u32Filp = 0x%x\n",_u32G3DFilpArray[2].u32Filp);
                        printk("_u32G3DFilpArray[3].u32Filp = 0x%x\n",_u32G3DFilpArray[3].u32Filp);
                    }
#endif
                }
                else
                {
                    buffer.ret_status = 0;
                }

                if (copy_to_user((G3D_CMDQ_BUFFER __user *)arg, &buffer, sizeof(G3D_CMDQ_BUFFER)))
                    return -EFAULT;
            }
            break;
        case G3D_IOC_GET_CMDQ:
            {
                G3D_MALLOC_INFO Buf;
                MHal_G3D_Get_CmdQ(&Buf);

                if (copy_to_user((G3D_MALLOC_INFO __user *)arg, &Buf, sizeof(G3D_MALLOC_INFO)))
                    return -EFAULT;
            }
            break;
        case G3D_IOC_IS_IDLE:
            {
                int isIdle = MHal_G3D_IsIdle();

                if (copy_to_user((int __user *) arg, &isIdle, sizeof(int)))
                    return -EFAULT;
            }
            break;
        case G3D_IOC_GET_SWSTAMP:
            {
                int stamp = MHal_G3D_GetSwStamp();

                if (copy_to_user((int __user *) arg, &stamp, sizeof(int)))
                    return -EFAULT;
            }
            break;
        case G3D_IOC_GET_HWSTAMP:
            {
                int stamp = MHal_G3D_GetHwStamp();

                if (copy_to_user((int __user *) arg, &stamp, sizeof(int)))
                    return -EFAULT;
            }
            break;
        case G3D_IOC_LOCK:
            {
                _u32LockForPNG = 1;
            }
            break;
        case G3D_IOC_UNLOCK:
            {
                _u32LockForPNG = 0;
            }
            break;

        case G3D_IOC_MALLOC:
            {
                G3D_MALLOC_INFO Buf;

                if (copy_from_user( &Buf, (G3D_MALLOC_INFO __user *)arg, sizeof(G3D_MALLOC_INFO)))
                    return -EFAULT;

                MHal_G3D_MALLOC(&Buf, (struct g3d_data*)filp->private_data);

                if (copy_to_user((G3D_MALLOC_INFO __user *)arg, &Buf, sizeof(G3D_MALLOC_INFO)))
                    return -EFAULT;
            }
            break;
        case G3D_IOC_MFREE:
            {
                G3D_MALLOC_INFO Buf;

                if (copy_from_user( &Buf, (G3D_MALLOC_INFO __user *)arg, sizeof(G3D_MALLOC_INFO)))
                    return -EFAULT;

                MHal_G3D_MFREE(&Buf, (struct g3d_data*)filp->private_data);
            }
            break;
        case G3D_IOC_GETMMAP:
            {
                G3D_MMAP_BUFFER Buf;
                MHal_G3D_GetMMAP(&Buf);

                if (copy_to_user((G3D_MMAP_BUFFER __user *)arg, &Buf, sizeof(G3D_MMAP_BUFFER)))
                    return -EFAULT;
            }
            break;
        case G3D_IOC_GETFILPID:
            {
                int u32G3DFilpID = getFilpId((unsigned int)filp);

                if (copy_to_user((int __user *) arg, &u32G3DFilpID, sizeof(int)))
                    return -EFAULT;
            }
            break;
        default:
            return -ENOTTY;
            break;
    }

    return err;
}

static int _MDrv_G3D_MMap(struct file *filp, struct vm_area_struct *vma)
{
#if 0
    pgprot_val(vma->vm_page_prot) &= ~_CACHE_MASK;
    pgprot_val(vma->vm_page_prot) |= _CACHE_UNCACHED;
#endif
    pgprot_val(vma->vm_page_prot) &= ~_CACHE_MASK;

    G3D_DRV_DBG("st = 0x%x, end = 0x%x, vm_pgoff = 0x%x \n",
            vma->vm_start, vma->vm_end, vma->vm_pgoff);

    if (io_remap_pfn_range(vma,
                          vma->vm_start,
                          vma->vm_pgoff,
                          vma->vm_end-vma->vm_start,
                          vma->vm_page_prot))
    {
      return -EAGAIN;
    }

    return 0;
}


static struct class *g3d_class;

static struct file_operations G3D_fops = {
    .owner   =  THIS_MODULE,
    .open    =  _MDrv_G3D_Open,
    .release =  _MDrv_G3D_Release,
    .ioctl   =  _MDrv_G3D_Ioctl,
    .mmap    =  _MDrv_G3D_MMap,
};

static int __init G3D_Init_Module(void)
{
    int result,i;
    dev_t dev = 0;

    MHal_G3D_Mem_Init();

    if (_u32G3DMajor)
    {
        dev = MKDEV(_u32G3DMajor, _u32G3DMinor);
        result = register_chrdev_region(dev, G3D_DEVICE_COUNT, MDRV_NAME_G3D);
    }
    else
    {
        result = alloc_chrdev_region(&dev, _u32G3DMinor, G3D_DEVICE_COUNT, MDRV_NAME_G3D);
        _u32G3DMajor = MAJOR(dev);
    }

    if (result < 0)
    {
        G3D_DRV_DBG("can't get major %d\n", _u32G3DMajor);
        return result;
    }

    cdev_init(&_stG3DDev, &G3D_fops);
    _stG3DDev.owner = THIS_MODULE;
    _stG3DDev.ops = &G3D_fops;
    result = cdev_add(&_stG3DDev, dev, G3D_DEVICE_COUNT);

    if (result)
    {
        G3D_DRV_DBG("Error %d adding G3D", result);
        unregister_chrdev_region(dev, G3D_DEVICE_COUNT);
        return result;
    }

    g3d_class = class_create(THIS_MODULE, MDRV_NAME_G3D);
    if (IS_ERR(g3d_class))
    {
        cdev_del(&_stG3DDev);
        unregister_chrdev_region(dev, G3D_DEVICE_COUNT);
        return PTR_ERR(g3d_class);
    }


    device_create(g3d_class, NULL, dev, NULL, MDRV_NAME_G3D);
    G3D_DRV_DBG("init %d\n", _u32G3DMajor);

    for (i = 0; i < MAX_FILP; ++i)
    {
        _u32G3DFilpArray[i].u32Filp = -1;
#ifdef OPT_G3D_CMD_SEND_COUNT_DEBUG
        _u32G3DFilpArray[i].u32CmdSendCount = 0;
#endif
    }
    return 0;

}

static void __exit G3D_Cleanup_Module(void)
{
    MHal_G3D_Mem_Deinit();

    device_destroy(g3d_class, MKDEV(_u32G3DMajor, _u32G3DMinor));
    class_destroy(g3d_class);

    cdev_del(&_stG3DDev);
    unregister_chrdev_region(MKDEV(_u32G3DMajor, _u32G3DMinor), G3D_DEVICE_COUNT);

    G3D_DRV_DBG("exit %d\n", _u32G3DMajor);
}

/* ---------------------------------------------------------------------- */

module_init(G3D_Init_Module);
module_exit(G3D_Cleanup_Module);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("G3D driver");
MODULE_LICENSE("GPL");


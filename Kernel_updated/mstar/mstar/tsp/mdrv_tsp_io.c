///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2010 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_tsp_io.c
// @brief  TSP KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>

//drver header files
#include "mdrv_mstypes.h"
#include "chip_int.h"
#include "mhal_tsp.h"
#include "mdrv_tsp_interrupt.h"
#include "mdrv_tsp_io.h"
#include "mst_devid.h"

//=============================================================================
// Local Defines
//=============================================================================
#define     MDRV_TSP_DEVICE_COUNT       1
#define     MDRV_TSP_NAME               "TSP"

#define     MAX_FILE_HANDLE_SUPPRT      64

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define     TSP_DEBUG_ENABLE  //
#ifdef      TSP_DEBUG_ENABLE
#define     TSPIO_KDBG(_fmt, _args...)        printk(KERN_WARNING _fmt, ## _args)
#define     TSPIO_ASSERT(_con)   do {\
                                                            if (!(_con)) {\
                                                                printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                                                                                                    __FILE__, __LINE__, #_con);\
                                                                BUG();\
                                                            }\
                                                          } while (0)
#else
#define     TSPIO_KDBG(fmt, args...)
#define     TSPIO_ASSERT(arg)
#endif

//--------------------------------------------------------------------------------------------------
// IOCtrl functions declaration
//--------------------------------------------------------------------------------------------------
static int _MDrv_TSPIO_Open(struct inode *inode, struct file *filp);
static int _MDrv_TSPIO_Release(struct inode *inode, struct file *filp);
static int _MDrv_TSPIO_IOCtl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
MSYSTEM_STATIC int _MDrv_TSPIO_ModuleInit(void);
MSYSTEM_STATIC void _MDrv_TSPIO_ModuleExit(void);

//=============================================================================
// Local Variables: Device handler
//=============================================================================
typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
} TSP_DEV;

static TSP_DEV _devTSP =
{
    .s32Major   = MDRV_MAJOR_TSP,
    .s32Minor   = MDRV_MINOR_TSP,
    .refCnt     = 0,
    .cdev =
    {
        .kobj   = {.name = MDRV_NAME_TSP, },
        .owner  = THIS_MODULE,
    },
    .fops =
    {
        .open   = _MDrv_TSPIO_Open,
        .release= _MDrv_TSPIO_Release,
        .ioctl  = _MDrv_TSPIO_IOCtl,
        //.poll   = _MDrv_TSPIO_Poll,
    }
};

spinlock_t   tsp_spinlock;
extern MS_TSP_EVENT_INFO   TspEventInfo;

#if 0
struct
{
    struct file* filp;
    MS_U32    u32GOPIdx;
}_filpGopIdxGFLIP[MAX_FILE_HANDLE_SUPPRT];

MS_U16  g_u16GOPRefCnt[MAX_GOP_SUPPORT];
#endif

//=============================================================================
// Local Function Prototypes
//=============================================================================
static int _MDrv_TSPIO_IOC_Init(struct file *filp, unsigned long arg);
//static int _MDrv_TSPIO_IOC_DeInit(struct file *filp, unsigned long arg);
//static int _MDrv_TSPIO_IOC_SetFlipInfo(struct file *filp, unsigned long arg);
//static int _MDrv_TSPIO_IOC_SetInputSigStatus(struct file *filp, unsigned long arg);
static MS_U32 _MDrv_TSP_RegisterInterrupt(void);
static MS_U32 _MDrv_TSP_DeRegisterInterrupt(void);

//-------------------------------------------------------------------------------------------------
// IOCtrl Driver functions
//-------------------------------------------------------------------------------------------------
int _MDrv_TSPIO_IOC_Init(struct file *filp, unsigned long arg)
{
#if 0
    MS_U32 u32GopIdx;
    MS_U16 u16Idx, u16AllocIdx = MAX_FILE_HANDLE_SUPPRT;

    if (__get_user(u32GopIdx, (MS_U32 __user *)arg))
    {
        return -EFAULT;
    }

    if (MAX_GOP_SUPPORT <= u32GopIdx)
    {
        return -EFAULT;
    }

    for(u16Idx=0; u16Idx<MAX_FILE_HANDLE_SUPPRT; u16Idx++)
    {
        if(_filpGopIdxGFLIP[u16Idx].filp==filp && _filpGopIdxGFLIP[u16Idx].u32GOPIdx==u32GopIdx)
        {//already init:
            return 0;
        }

        if(u16AllocIdx==MAX_FILE_HANDLE_SUPPRT && NULL==_filpGopIdxGFLIP[u16Idx].filp)
        {
            u16AllocIdx= u16Idx;
        }
    }

    if(MAX_FILE_HANDLE_SUPPRT<=u16AllocIdx)
    {
        return -EFAULT;
    }

    _filpGopIdxGFLIP[u16AllocIdx].filp = filp;
    _filpGopIdxGFLIP[u16AllocIdx].u32GOPIdx = u32GopIdx;
    //GFLIPIO_ASSERT(g_u16GOPRefCnt[u32GopIdx] >= 0);

    if(++g_u16GOPRefCnt[u32GopIdx] > 1)
    {
         return 0;
    }
#endif

    return 1;//MDrv_TSP_Init(u32GopIdx) ? 0 : (-EFAULT);
}

/********************************************************
*** Do not export this IO DDI now ****************************
int _MDrv_GFLIPIO_IOC_DeInit(struct file *filp, unsigned long arg)
{
    MS_U32 u32GopIdx;

    if (__get_user(u32GopIdx, (MS_U32 __user *)arg))
    {
        return EFAULT;
    }

    return MDrv_GFLIP_DeInit(u32GopIdx);
}
*/

int _MDrv_TSPIO_IOC_GetEventInfo(struct file *filp, unsigned long arg)
{
    MS_TSP_EVENT_INFO stTspEventInfo;
	unsigned long flags;

    spin_lock_irqsave(tsp_spinlock, flags);
    memcpy(&stTspEventInfo, &TspEventInfo, sizeof(MS_TSP_EVENT_INFO));
    memset(&TspEventInfo, 0x00, sizeof(MS_TSP_EVENT_INFO)-4);
    spin_unlock_irqrestore(tsp_spinlock, flags);

    if (copy_to_user((MS_TSP_EVENT_INFO __user *)arg, &stTspEventInfo, sizeof(MS_TSP_EVENT_INFO)))
    {
        return EFAULT;
    }

    return 0;
}

int _MDrv_TSPIO_IOC_ClearEventInfo(struct file *filp, unsigned long arg)
{
	unsigned long flags;

    spin_lock_irqsave(tsp_spinlock, flags);
    TspEventInfo.SecEvent[arg] = 0;
    TspEventInfo.OverflowEvent[arg] = 0;
    spin_unlock_irqrestore(tsp_spinlock, flags);

    return 0;
}

MS_U32 _MDrv_TSP_RegisterInterrupt(void)
{
    if (0 != (request_irq(E_IRQ_TSP2HK, MDrv_TSPINT_IntHandler, SA_INTERRUPT, "tsp", NULL)))
    {
        TSPIO_KDBG("[TSP] Fail to request IRQ:%d\n", E_IRQ_TSP2HK);
        return EFAULT;
    }

    return 0;
}

MS_U32 _MDrv_TSP_DeRegisterInterrupt(void)
{
    TSPIO_KDBG("[TSP] dismiss IRQ:%d\n", E_IRQ_TSP2HK);
    free_irq(E_IRQ_TSP2HK, NULL);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_TSPIO_Open(struct inode *inode, struct file *filp)
{
    TSPIO_KDBG("[TSP] TSP DRIVER OPEN\n");

    TSPIO_ASSERT(_devTSP.refCnt >= 0);
    spin_lock_init(&tsp_spinlock);
    _devTSP.refCnt++;

    if (EFAULT == _MDrv_TSP_RegisterInterrupt())
    {
        TSPIO_KDBG("[TSP] Startup TSP Driver Failed! %d\n", _devTSP.s32Major);
        //cdev_del(&_devTSP.cdev);
        //unregister_chrdev_region(dev, MDRV_TSP_DEVICE_COUNT);
	    return -ENOMEM;
    }

    memset(&TspEventInfo, 0x00, sizeof(MS_TSP_EVENT_INFO));

    return 0;
}

int _MDrv_TSPIO_Release(struct inode *inode, struct file *filp)
{
    //MS_U32 u32Idx;

    TSPIO_KDBG("[TSP] TSP DRIVER CLOSE\n");

    TSPIO_ASSERT(_devTSP.refCnt>0);

    /*de-initial the who TSPDriver */
    _MDrv_TSP_DeRegisterInterrupt();

#if 0
    for(u32Idx=0; u32Idx<MAX_FILE_HANDLE_SUPPRT; u32Idx++)
    {
        if(_filpGopIdxGFLIP[u32Idx].filp != filp)
        {
            continue;
        }

        GFLIPIO_ASSERT(g_u16GOPRefCnt[_filpGopIdxGFLIP[u32Idx].u32GOPIdx] > 0);

        if(0 == --g_u16GOPRefCnt[_filpGopIdxGFLIP[u32Idx].u32GOPIdx])
        {
                MDrv_GFLIP_DeInit(_filpGopIdxGFLIP[u32Idx].u32GOPIdx);
        }

        _filpGopIdxGFLIP[u32Idx].filp = NULL;
    }
#endif

    return 0;
}

int _MDrv_TSPIO_IOCtl(struct inode *inode, struct file *filp, U32 u32Cmd, unsigned long u32Arg)
{
    int err = 0;
    int retval = 0;
    if (_devTSP.refCnt <= 0)
    {
        return -EFAULT;
    }
    /* check u32Cmd valid */
    if (MDRV_TSP_IOC_MAGIC != _IOC_TYPE(u32Cmd))
    {
        return -ENOTTY;
    }

    if (_IOC_NR(u32Cmd) >= MDRV_TSP_IOC_MAX_NR)
    {
        TSPIO_KDBG("[TSP] IOCtl NR Error!!! (Cmd=%x)\n", u32Cmd);
        return -ENOTTY;
    }

    /* verify Access */
    if (_IOC_DIR(u32Cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    else if (_IOC_DIR(u32Cmd) & _IOC_WRITE)
    {
        err = !access_ok(VERIFY_READ, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }

    if (err)
    {
        return -EFAULT;
    }

    /* handle u32Cmd */
    switch (u32Cmd)
    {
    case MDRV_TSP_IOC_INIT:
        retval = _MDrv_TSPIO_IOC_Init(filp, u32Arg);
        break;
    case MDRV_TSP_IOC_GETEVENTINFO:
        retval = _MDrv_TSPIO_IOC_GetEventInfo(filp, u32Arg);
        break;
    case MDRV_TSP_IOC_CLEAREVENTINFO:
        retval = _MDrv_TSPIO_IOC_ClearEventInfo(filp, u32Arg);
        break;
    default:  /* redundant, as cmd was checked against MAXNR */
        TSPIO_KDBG("[TSP] ERROR IOCtl number %x\n", u32Cmd);
        return -ENOTTY;
    }

    return retval;
}

#if 0
static unsigned int _MDrv_TSPIO_Poll(struct file *filp, poll_table *wait)
{
    //TSP_PRINT("%s is invoked\n", __FUNCTION__);
    poll_wait(filp, &_wait_queue,  wait);
    return (!_u32TspEvent) ? 0 : POLLIN;
}
#endif

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
int _MDrv_TSPIO_ModuleInit(void)
{
    int s32Ret;
    dev_t  dev;

    //memset(_filpGopIdxGFLIP, 0, sizeof(_filpGopIdxGFLIP));
    ///memset(g_u16GOPRefCnt, 0, sizeof(g_u16GOPRefCnt));

    if(_devTSP.s32Major)
    {
        dev = MKDEV(_devTSP.s32Major, _devTSP.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_TSP_DEVICE_COUNT, MDRV_TSP_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, _devTSP.s32Minor, MDRV_TSP_DEVICE_COUNT, MDRV_TSP_NAME);
        _devTSP.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        TSPIO_KDBG("[TSP] Unable to get major %d\n", _devTSP.s32Major);
        return s32Ret;
    }

    cdev_init(&_devTSP.cdev, &_devTSP.fops);
    if (0 != (s32Ret= cdev_add(&_devTSP.cdev, dev, MDRV_TSP_DEVICE_COUNT)))
    {
        TSPIO_KDBG("[TSP] Unable add a character device\n");
        unregister_chrdev_region(dev, MDRV_TSP_DEVICE_COUNT);
        return s32Ret;
    }

    /* initial the whole TSP Driver */

#if 0
    if (EFAULT == _MDrv_TSP_RegisterInterrupt())
    {
        TSPIO_KDBG("[TSP] Startup TSP Driver Failed! %d\n", _devTSP.s32Major);
        cdev_del(&_devTSP.cdev);
        unregister_chrdev_region(dev, MDRV_TSP_DEVICE_COUNT);
	    return -ENOMEM;
    }
#endif

    return 0;
}

void _MDrv_TSPIO_ModuleExit(void)
{
    cdev_del(&_devTSP.cdev);
    unregister_chrdev_region(MKDEV(_devTSP.s32Major, _devTSP.s32Minor), MDRV_TSP_DEVICE_COUNT);
}

#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
module_init(_MDrv_TSPIO_ModuleInit);
module_exit(_MDrv_TSPIO_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("TSP ioctrl driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)

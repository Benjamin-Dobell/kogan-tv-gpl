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
/// file    mdrv_mbx_io.c
/// @brief  MS MailBox ioctrol driver
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

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
#include <linux/platform_device.h>
#include <linux/version.h>

//drver header files
#include "mdrv_mstypes.h"
#include "mdrv_mbx.h"
#include "mdrv_mbx_io.h"
#include "mst_devid.h"

//=============================================================================
// Local Defines
//=============================================================================
#define     MDRV_MBX_DEVICE_COUNT           1
#define     MDRV_MBX_NAME                           "MSMAILBOX"

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define     MBXIO_DEBUG_ENABLE  //
#ifdef      MBXIO_DEBUG_ENABLE
#define     MBXIO_KDBG(_fmt, _args...)        printk(KERN_WARNING _fmt, ## _args)
#define     MBXIO_ASSERT(_con)   do {\
                                                            if (!(_con)) {\
                                                                printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                                                                                                    __FILE__, __LINE__, #_con);\
                                                                BUG();\
                                                            }\
                                                          } while (0)
#else
#define     MBXIO_KDBG(fmt, args...)
#define     MBXIO_ASSERT(arg)
#endif

//--------------------------------------------------------------------------------------------------
// IOCtrl functions declaration
//--------------------------------------------------------------------------------------------------
static int _MDrv_MBXIO_Open (struct inode *inode, struct file *filp);
static int _MDrv_MBXIO_Release(struct inode *inode, struct file *filp);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static int _MDrv_MBXIO_IOCtl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int _MDrv_MBXIO_IOCtl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
static int _MDrv_MBXIO_FASYNC(int fd, struct file *filp, int mode);
MSYSTEM_STATIC int _MDrv_MBXIO_ModuleInit(void);
MSYSTEM_STATIC void _MDrv_MBXIO_ModuleExit(void);

ssize_t _MDrv_MBXIO_Read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t _MDrv_MBXIO_Write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);



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
}MBX_DEV;

static MBX_DEV _devMBX =
{
    .s32Major = MDRV_MAJOR_MSMAILBOX,
    .s32Minor = MDRV_MINOR_MSMAILBOX,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name= MDRV_NAME_MSMAILBOX, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open = _MDrv_MBXIO_Open,
        .read = _MDrv_MBXIO_Read,
        .write = _MDrv_MBXIO_Write,
        .release = _MDrv_MBXIO_Release,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
        .unlocked_ioctl = _MDrv_MBXIO_IOCtl,
        #else
        .ioctl = _MDrv_MBXIO_IOCtl,
        #endif
        .fasync = _MDrv_MBXIO_FASYNC,
    }
};

//=============================================================================
// Local Function Prototypes
//=============================================================================
static int _MDrv_MBXIO_IOC_Init(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_DeInit(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_RegisterMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_UnRegisterMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_ClearMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_SendMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_SendMsgLoopback(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_RecvMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_CheckMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_RemoveLatestMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_GetMsgQStatus(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_MbxEnable(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_SetInformation(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_GetInformation(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_GetDrvStatus(struct file *filp, unsigned long arg);
//-------------------------------------------------------------------------------------------------
// IOCtrl Driver functions
//-------------------------------------------------------------------------------------------------
int _MDrv_MBXIO_IOC_Init(struct file *filp, unsigned long arg)
{
    MS_MBX_INIT_INFO stInitInfo;

    if(copy_from_user(&stInitInfo, (MS_MBX_REGISTER_MSG __user *)arg, sizeof(MS_MBX_INIT_INFO)))
        return EFAULT;

    stInitInfo.mbxResult = MDrv_MBX_Init((MS_U32)filp, stInitInfo.eHKCPU, stInitInfo.eHostRole, stInitInfo.u32TimeoutMillSecs);

    return __put_user(stInitInfo.mbxResult, &(((MS_MBX_INIT_INFO __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_DeInit(struct file *filp, unsigned long arg)
{
    MS_MBX_SET_BINFO stSetBInfo;

    if(__get_user(stSetBInfo.bInfo, &((MS_MBX_SET_BINFO __user *)arg)->bInfo))
        return EFAULT;

    stSetBInfo.mbxResult = MDrv_MBX_DeInit((MS_U32)filp, stSetBInfo.bInfo);

    return __put_user(stSetBInfo.mbxResult, &(((MS_MBX_SET_BINFO __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_RegisterMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_REGISTER_MSG stRegisterMsg;

    if(copy_from_user(&stRegisterMsg, (MS_MBX_REGISTER_MSG __user *)arg, sizeof(MS_MBX_REGISTER_MSG)))
        return EFAULT;

    stRegisterMsg.mbxResult = MDrv_MBX_RegisterMSG((MS_U32)filp, stRegisterMsg.eMsgClass, stRegisterMsg.u16MsgQueueSize);
    return __put_user(stRegisterMsg.mbxResult, &(((MS_MBX_REGISTER_MSG __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_UnRegisterMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_UNREGISTER_MSG stUnRegisterMsg;

    if(copy_from_user(&stUnRegisterMsg, (MS_MBX_UNREGISTER_MSG __user *)arg, sizeof(MS_MBX_UNREGISTER_MSG)))
        return EFAULT;

    stUnRegisterMsg.mbxResult = MDrv_MBX_UnRegisterMSG((MS_U32)filp, stUnRegisterMsg.eMsgClass, stUnRegisterMsg.bForceDiscardMsgQueue);

    return __put_user(stUnRegisterMsg.mbxResult, &(((MS_MBX_UNREGISTER_MSG __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_ClearMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_CLEAR_MSG stClearMsg;

    if(copy_from_user(&stClearMsg, (MS_MBX_CLEAR_MSG __user *)arg, sizeof(MS_MBX_CLEAR_MSG)))
        return EFAULT;

    stClearMsg.mbxResult = MDrv_MBX_ClearMSG((MS_U32)filp, stClearMsg.eMsgClass);

    return __put_user(stClearMsg.mbxResult, &(((MS_MBX_CLEAR_MSG __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_SendMsg(struct file *filp, unsigned long arg)
{
    MBX_Result mbxResult;
    MBX_Msg mbxMsg;

    if(copy_from_user(&mbxMsg, ((MS_MBX_SEND_MSG __user *)arg)->pMsg, sizeof(MBX_Msg)))
        return EFAULT;

    mbxResult = MDrv_MBX_SendMsg((MS_U32)filp, &mbxMsg);

    return __put_user(mbxResult, &(((MS_MBX_SEND_MSG __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_SendMsgLoopback(struct file *filp, unsigned long arg)
{
    MS_MBX_SEND_MSG stSendMsg;
    MBX_Msg mbxMsg;

    if(copy_from_user(&stSendMsg, (MS_MBX_SEND_MSG __user *)arg, sizeof(MS_MBX_SEND_MSG)))
        return EFAULT;

    if(copy_from_user(&mbxMsg, ((MS_MBX_SEND_MSG __user *)arg)->pMsg, sizeof(MBX_Msg)))
        return EFAULT;

    stSendMsg.mbxResult = MDrv_MBX_SendMsgLoopback((MS_U32)filp, &mbxMsg, stSendMsg.mbxResult);

    return __put_user(stSendMsg.mbxResult, &(((MS_MBX_SEND_MSG __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_RecvMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_RECV_MSG stRecvMsg;
    MBX_Msg mbxMsg;

    if(copy_from_user(&stRecvMsg, (MS_MBX_RECV_MSG __user *)arg, sizeof(MS_MBX_RECV_MSG)))
        return EFAULT;

    stRecvMsg.mbxResult = MDrv_MBX_RecvMsg((MS_U32)filp, stRecvMsg.eTargetClass, &mbxMsg, stRecvMsg.u32WaitMillSecs, stRecvMsg.u32Flag);
    if(copy_to_user((( MS_MBX_RECV_MSG __user *)arg)->pMsg, &mbxMsg, sizeof(MBX_Msg)))
        return EFAULT;

    return __put_user(stRecvMsg.mbxResult, &(((MS_MBX_RECV_MSG __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_CheckMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_RECV_MSG stRecvMsg;
    MBX_Msg mbxMsg;

    if(copy_from_user(&stRecvMsg, (MS_MBX_RECV_MSG __user *)arg, sizeof(MS_MBX_RECV_MSG)))
        return EFAULT;

    stRecvMsg.mbxResult = MDrv_MBX_CheckMsg((MS_U32)filp, stRecvMsg.eTargetClass, &mbxMsg, stRecvMsg.u32WaitMillSecs, stRecvMsg.u32Flag);
    if(copy_to_user((( MS_MBX_RECV_MSG __user *)arg)->pMsg, &mbxMsg, sizeof(MBX_Msg)))
        return EFAULT;

    return __put_user(stRecvMsg.mbxResult, &(((MS_MBX_RECV_MSG __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_RemoveLatestMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_SET_BINFO stSetBInfo;

    stSetBInfo.mbxResult = MDrv_MBX_RemoveLatestMsg();

    return __put_user(stSetBInfo.mbxResult, &(((MS_MBX_SET_BINFO __user *)arg)->mbxResult));
}


int _MDrv_MBXIO_IOC_GetMsgQStatus(struct file *filp, unsigned long arg)
{
    MS_MBX_GET_MSGQSTATUS stGetMsgQStatus;
    MBX_MSGQ_Status mbxMsgQueueStatus;

    if(__get_user(stGetMsgQStatus.eTargetClass, &((MS_MBX_GET_MSGQSTATUS __user *)arg)->eTargetClass))
        return EFAULT;

    stGetMsgQStatus.mbxResult = MDrv_MBX_GetMsgQueueStatus((MS_U32)filp, stGetMsgQStatus.eTargetClass, &mbxMsgQueueStatus);

    if(copy_to_user((( MS_MBX_GET_MSGQSTATUS __user *)arg)->pMsgQueueStatus, &mbxMsgQueueStatus, sizeof(MBX_MSGQ_Status)))
        return EFAULT;

    return __put_user(stGetMsgQStatus.mbxResult, &(((MS_MBX_GET_MSGQSTATUS __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_MbxEnable(struct file *filp, unsigned long arg)
{
    MS_MBX_SET_BINFO stSetBInfo;

    if(__get_user(stSetBInfo.bInfo, &((MS_MBX_SET_BINFO __user *)arg)->bInfo))
        return EFAULT;

    stSetBInfo.mbxResult = MDrv_MBX_Enable((MS_U32)filp, stSetBInfo.bInfo);

    return __put_user(stSetBInfo.mbxResult, &(((MS_MBX_SET_BINFO __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_SetInformation(struct file *filp, unsigned long arg)
{
    MS_MBX_CPROSYNC_INFORMATION stSetInfo;
    MS_U8 u8Info[MAX_MBX_INFORMATION_SIZE];

    if(copy_from_user(&stSetInfo, (MS_MBX_CPROSYNC_INFORMATION __user *)arg, sizeof(MS_MBX_CPROSYNC_INFORMATION)))
        return EFAULT;

    if(copy_from_user(u8Info, ((MS_MBX_CPROSYNC_INFORMATION __user *)arg)->pU8Info, stSetInfo.u8Size))
        return EFAULT;

    stSetInfo.mbxResult = MDrv_MBX_SetInformation((MS_U32)filp, stSetInfo.eTargetRole, u8Info, stSetInfo.u8Size);

    return __put_user(stSetInfo.mbxResult, &(((MS_MBX_CPROSYNC_INFORMATION __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_GetInformation(struct file *filp, unsigned long arg)
{
    MS_MBX_CPROSYNC_INFORMATION stGetInfo;
    MS_U8 u8Info[MAX_MBX_INFORMATION_SIZE];

    if(copy_from_user(&stGetInfo, (MS_MBX_CPROSYNC_INFORMATION __user *)arg, sizeof(MS_MBX_CPROSYNC_INFORMATION)))
        return EFAULT;

    stGetInfo.mbxResult = MDrv_MBX_GetInformation((MS_U32)filp, stGetInfo.eTargetRole, u8Info, stGetInfo.u8Size);

    if(copy_to_user((( MS_MBX_CPROSYNC_INFORMATION __user *)arg)->pU8Info, u8Info, stGetInfo.u8Size))
        return EFAULT;

    return __put_user(stGetInfo.mbxResult, &(((MS_MBX_CPROSYNC_INFORMATION __user *)arg)->mbxResult));
}

int _MDrv_MBXIO_IOC_GetDrvStatus(struct file *filp, unsigned long arg)
{
    MS_MBX_GET_DRVSTATUS stGetDrvStatus;

    stGetDrvStatus.bEnabled = MDrv_MBX_GetEnableStatus((MS_U32)filp);
    stGetDrvStatus.s32RefCnt = _devMBX.refCnt;

    if(copy_to_user((( MS_MBX_GET_DRVSTATUS __user *)arg), &stGetDrvStatus, sizeof(MS_MBX_GET_DRVSTATUS)))
        return EFAULT;

    return 0;
}

//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_MBXIO_Open(struct inode *inode, struct file *filp)
{
    MBXIO_KDBG("--------->MBX DRIVER OPEN\n");

    if(_devMBX.refCnt == 0)
    {
        _devMBX.refCnt = 1;
        return 0;
    }
    else
    {
        _devMBX.refCnt++;
    }

    return 0;
}

ssize_t _MDrv_MBXIO_Read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t _MDrv_MBXIO_Write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    return 0;
}

int _MDrv_MBXIO_Release(struct inode *inode, struct file *filp)
{
    MBXIO_KDBG("<---------MBX DRIVER CLOSE\n");

    MBXIO_ASSERT(_devMBX.refCnt>0);

    //de-register from signal queue:
    MDrv_MBX_ReleaseFASYNC((MS_U32)filp); //call driver interface to un-register signal queue.

    //force to release since application is exit!
    MDrv_MBX_DeInit((MS_U32)filp, TRUE);

    if(--_devMBX.refCnt == 0)
    {
        //any de-initialize when release?
    }
    return 0;
}

int _MDrv_MBXIO_FASYNC(int fd, struct file *filp, int mode)
{
    MBXIO_KDBG("%s is invoked\n", __FUNCTION__);

    if(E_MBX_SUCCESS != MDrv_MBX_FASYNC((MS_S32)fd, (MS_U32)filp, (MS_S32)mode)) //fasync_helper(fd, filp, mode, &IRDev.async_queue);//call driver interface to register signal queue.
        return -EBUSY;

    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
int _MDrv_MBXIO_IOCtl(struct file *filp, U32 u32Cmd, unsigned long u32Arg)
#else
int _MDrv_MBXIO_IOCtl(struct inode *inode, struct file *filp, U32 u32Cmd, unsigned long u32Arg)
#endif
{
    int err = 0;
    int retval = 0;

    if(_devMBX.refCnt <= 0)
    {
        return -EFAULT;
    }

    /* check u32Cmd valid */
    if (MDRV_MBX_IOC_MAGIC!= _IOC_TYPE(u32Cmd))
    {
        return -ENOTTY;
    }

    if (_IOC_NR(u32Cmd) > MDRV_MBX_IOC_MAX_NR)
    {
        MBXIO_KDBG("IOCtl NR Error!!! (Cmd=%x)\n",u32Cmd);
        return -ENOTTY;
    }

    /* verify Access */
    if (_IOC_DIR(u32Cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    else if (_IOC_DIR(u32Cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    if (err)
    {
        return -EFAULT;
    }

    /* handle u32Cmd */
    switch(u32Cmd)
    {
        case MDRV_MBX_IOC_INIT:
            retval = _MDrv_MBXIO_IOC_Init(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_DEINIT:
            retval = _MDrv_MBXIO_IOC_DeInit(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_REGISTERMSG:
            retval = _MDrv_MBXIO_IOC_RegisterMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_UNREGISTERMSG:
            retval = _MDrv_MBXIO_IOC_UnRegisterMsg(filp, u32Arg);
            break;
        case MDRV_mBX_IOC_CLEARMSG:
            retval = _MDrv_MBXIO_IOC_ClearMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_SENDMSG:
            retval = _MDrv_MBXIO_IOC_SendMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_RECVMSG:
            retval = _MDrv_MBXIO_IOC_RecvMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_CHECKMSG:
            retval = _MDrv_MBXIO_IOC_CheckMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_REMOVEMSG:
            retval = _MDrv_MBXIO_IOC_RemoveLatestMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_GETMSGQSTATUS:
            retval = _MDrv_MBXIO_IOC_GetMsgQStatus(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_MBXENABLE:
            retval = _MDrv_MBXIO_IOC_MbxEnable(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_SETINFORMATION:
            retval = _MDrv_MBXIO_IOC_SetInformation(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_GETINFORMATION:
            retval = _MDrv_MBXIO_IOC_GetInformation(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_SENDMSGLOOPBACK:
            retval = _MDrv_MBXIO_IOC_SendMsgLoopback(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_GETDRVSTATUS:
            retval = _MDrv_MBXIO_IOC_GetDrvStatus(filp, u32Arg);
            break;
        default:  /* redundant, as cmd was checked against MAXNR */
            MBXIO_KDBG(" ERROR IOCtl number %x\n ",u32Cmd);
            return -ENOTTY;
    }

    return retval;
}

static MSTAR_MBX_DEV _st_mbxdev={0};
static int mstar_mbx_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    MSTAR_MBX_DEV *mbxdev=(MSTAR_MBX_DEV*)dev->dev.platform_data;

    return MDrv_MBX_Suspend(mbxdev);
}
static int mstar_mbx_drv_resume(struct platform_device *dev)
{
    MSTAR_MBX_DEV *mbxdev=(MSTAR_MBX_DEV*)dev->dev.platform_data;

    return MDrv_MBX_Resume(mbxdev);
}

static int mstar_mbx_drv_probe(struct platform_device *pdev)
{
    int retval=0;
    pdev->dev.platform_data=&_st_mbxdev;
	return retval;
}

static int mstar_mbx_drv_remove(struct platform_device *pdev)
{
    pdev->dev.platform_data=NULL;
    return 0;
}

static struct platform_driver Mstar_mbx_driver = {
	.probe 		= mstar_mbx_drv_probe,
	.remove 	= mstar_mbx_drv_remove,
    .suspend    = mstar_mbx_drv_suspend,
    .resume     = mstar_mbx_drv_resume,

	.driver = {
		.name	= "Mstar-mbx",
        .owner  = THIS_MODULE,
	}
};
//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
MSYSTEM_STATIC int _MDrv_MBXIO_ModuleInit(void)
{
    int s32Ret;
    dev_t  dev;

    if(_devMBX.s32Major)
    {
        dev = MKDEV(_devMBX.s32Major, _devMBX.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_MBX_DEVICE_COUNT, MDRV_MBX_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, _devMBX.s32Minor, MDRV_MBX_DEVICE_COUNT, MDRV_MBX_NAME);
        _devMBX.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        MBXIO_KDBG("Unable to get major %d\n", _devMBX.s32Major);
        return s32Ret;
    }

    cdev_init(&_devMBX.cdev, &_devMBX.fops);
    if (0 != (s32Ret= cdev_add(&_devMBX.cdev, dev, MDRV_MBX_DEVICE_COUNT)))
    {
        MBXIO_KDBG("Unable add a character device\n");
        unregister_chrdev_region(dev, MDRV_MBX_DEVICE_COUNT);
        return s32Ret;
    }

    /* initial the whole MBX Driver */
    if(E_MBX_SUCCESS != MDrv_MBX_Startup())
    {
        MBXIO_KDBG("Startup MBX Driver Failed! %d\n", _devMBX.s32Major);
        cdev_del(&_devMBX.cdev);
        unregister_chrdev_region(dev, MDRV_MBX_DEVICE_COUNT);
	 return -ENOMEM;
    }
    platform_driver_register(&Mstar_mbx_driver);
    return 0;
}


MSYSTEM_STATIC void _MDrv_MBXIO_ModuleExit(void)
{
    /*de-initial the who MBX Driver */
    MDrv_MBX_Exit();

    cdev_del(&_devMBX.cdev);
    unregister_chrdev_region(MKDEV(_devMBX.s32Major, _devMBX.s32Minor), MDRV_MBX_DEVICE_COUNT);
    platform_driver_unregister(&Mstar_mbx_driver);
}

#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
module_init(_MDrv_MBXIO_ModuleInit);
module_exit(_MDrv_MBXIO_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MSMAILBOX ioctrl driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)

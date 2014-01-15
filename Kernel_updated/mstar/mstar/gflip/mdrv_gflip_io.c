///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
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
// @file   mdrv_gflip_io.c
// @brief  GFlip KMD Driver Interface
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
#include <linux/platform_device.h>

//drver header files
#include "mdrv_mstypes.h"
#include "chip_int.h"
#include "mdrv_gflip_io.h"
#include "mdrv_gflip.h"
#include "mhal_gflip.h"
#include "mdrv_gflip_interrupt.h"
#include "mdrv_gflip_ve_io.h"
#include "mst_devid.h"

//=============================================================================
// Local Defines
//=============================================================================
#define     MDRV_GFLIP_DEVICE_COUNT           1
#define     MDRV_GFLIP_NAME                           "GFLIP"

#define MAX_FILE_HANDLE_SUPPRT  64

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define     GFLIPIO_DEBUG_ENABLE  //
#ifdef      GFLIPIO_DEBUG_ENABLE
#define     GFLIPIO_KDBG(_fmt, _args...)        printk(KERN_WARNING _fmt, ## _args)
#define     GFLIPIO_ASSERT(_con)   do {\
                                                            if (!(_con)) {\
                                                                printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                                                                                                    __FILE__, __LINE__, #_con);\
                                                                BUG();\
                                                            }\
                                                          } while (0)
#else
#define     GFLIPIO_KDBG(fmt, args...)
#define     GFLIPIO_ASSERT(arg)
#endif

//--------------------------------------------------------------------------------------------------
// IOCtrl functions declaration
//--------------------------------------------------------------------------------------------------
static int _MDrv_GFLIPIO_Open (struct inode *inode, struct file *filp);
static int _MDrv_GFLIPIO_Release(struct inode *inode, struct file *filp);
#ifdef HAVE_UNLOCKED_IOCTL
static int _MDrv_GFLIPIO_IOCtl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int _MDrv_GFLIPIO_IOCtl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
MSYSTEM_STATIC int _MDrv_GFLIPIO_ModuleInit(void);
MSYSTEM_STATIC void _MDrv_GFLIPIO_ModuleExit(void);

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
}GFLIP_DEV;

static GFLIP_DEV _devGFLIP =
{
    .s32Major = MDRV_MAJOR_GFLIP,
    .s32Minor = MDRV_MINOR_GFLIP,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name= MDRV_NAME_GFLIP, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open = _MDrv_GFLIPIO_Open,
        .release = _MDrv_GFLIPIO_Release,
#ifdef HAVE_UNLOCKED_IOCTL
        .unlocked_ioctl = _MDrv_GFLIPIO_IOCtl,
#else
        .ioctl = _MDrv_GFLIPIO_IOCtl,
#endif
    }
};

struct
{
    struct file* filp;
    MS_U32    u32GOPIdx;
}_filpGopIdxGFLIP[MAX_FILE_HANDLE_SUPPRT];
MS_U16  g_u16GOPRefCnt[MAX_GOP_SUPPORT];
//=============================================================================
// Local Function Prototypes
//=============================================================================
static int _MDrv_GFLIPIO_IOC_Init(struct file *filp, unsigned long arg);
//static int _MDrv_GFLIPIO_IOC_DeInit(struct file *filp, unsigned long arg);
static int _MDrv_GFLIPIO_IOC_SetFlipInfo(struct file *filp, unsigned long arg);
static int _MDrv_GFLIPIO_IOC_SetInputSigStatus(struct file *filp, unsigned long arg);
static int _MDrv_GFLIPIO_IOC_SetPixelIDAddr(struct file *filp, unsigned long arg);
static int _MDrv_GFLIPIO_IOC_SetGPIO3DPin(struct file *filp, unsigned long arg);
static MS_U32 _MDrv_GFLIP_RegisterInterrupt(void);
static MS_U32 _MDrv_GFLIP_DeRegisterInterrupt(void);

//-------------------------------------------------------------------------------------------------
// IOCtrl Driver functions
//-------------------------------------------------------------------------------------------------
int _MDrv_GFLIPIO_IOC_Init(struct file *filp, unsigned long arg)
{
    MS_U32 u32GopIdx;
    MS_U16 u16Idx, u16AllocIdx=MAX_FILE_HANDLE_SUPPRT;

    if(__get_user(u32GopIdx, (MS_U32 __user *)arg))
    {
        return -EFAULT;
    }

    if(MAX_GOP_SUPPORT<= u32GopIdx)
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

    return MDrv_GFLIP_Init(u32GopIdx) ? 0 : (-EFAULT);
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

//Flip function for GOP 3D function
int _MDrv_GFLIPIO_IOC_Set3DFlipInfo(struct file *filp, unsigned long arg)
{
    MS_GFLIP_3DINFO st3DFlipInfo;

    if(copy_from_user(&st3DFlipInfo, (MS_GFLIP_3DINFO __user *)arg, sizeof(MS_GFLIP_3DINFO)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetFlipInfo(st3DFlipInfo.u32GopIdx, st3DFlipInfo.u32GwinIdx, st3DFlipInfo.u32MainAddr, st3DFlipInfo.u32SubAddr, st3DFlipInfo.u32TagId,&st3DFlipInfo.u32QEntry,&st3DFlipInfo.u32Result))
    {
        __put_user(st3DFlipInfo.u32QEntry, &(((MS_GFLIP_INFO __user *)arg)->u32QEntry));
        __put_user(st3DFlipInfo.u32Result, &(((MS_GFLIP_INFO __user *)arg)->u32Result));
	    return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

//Flip function for GOP normal 2D function
int _MDrv_GFLIPIO_IOC_SetFlipInfo(struct file *filp, unsigned long arg)
{
    MS_GFLIP_INFO stFlipInfo;

    if(copy_from_user(&stFlipInfo, (MS_GFLIP_INFO __user *)arg, sizeof(MS_GFLIP_INFO)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetFlipInfo(stFlipInfo.u32GopIdx, stFlipInfo.u32GwinIdx, stFlipInfo.u32Addr, 0, stFlipInfo.u32TagId,&stFlipInfo.u32QEntry,&stFlipInfo.u32Result))
    {
        __put_user(stFlipInfo.u32QEntry, &(((MS_GFLIP_INFO __user *)arg)->u32QEntry));
        __put_user(stFlipInfo.u32Result, &(((MS_GFLIP_INFO __user *)arg)->u32Result));
	    return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

int _MDrv_GFLIPIO_IOC_SetPixelIDAddr(struct file *filp, unsigned long arg)
{
    MS_GFLIP_INFO stFlipInfo;

    if(copy_from_user(&stFlipInfo, (MS_GFLIP_INFO __user *)arg, sizeof(MS_GFLIP_INFO)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetPixelIDAddr(stFlipInfo.u32GopIdx, stFlipInfo.u32GwinIdx, stFlipInfo.u32Addr,stFlipInfo.u32TagId,&stFlipInfo.u32QEntry,&stFlipInfo.u32Result))
    {
        __put_user(stFlipInfo.u32Result, &(((MS_GFLIP_INFO __user *)arg)->u32Result));
	    return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

int _MDrv_GFLIPIO_IOC_SetGPIO3DPin(struct file *filp, unsigned long arg)
{
    MS_GFLIP_INFO stFlipInfo;

    if(copy_from_user(&stFlipInfo, (MS_GFLIP_INFO __user *)arg, sizeof(MS_GFLIP_INFO)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetGPIO3DPin(stFlipInfo.u32Addr, &stFlipInfo.u32Result))
    {
        __put_user(stFlipInfo.u32Result, &(((MS_GFLIP_INFO __user *)arg)->u32Result));
	    return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

#if	( defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT))
//Old method by GOP driver
int _MDrv_GFLIPIO_IOC_EnableVECapture(struct file *filp, unsigned long arg)
{
    MS_GFLIP_VECAPTURESTATE stVECaptureState;
    MS_GFLIP_VECAPTURESTATE stCurState;

    if(copy_from_user(&stVECaptureState, (MS_GFLIP_VECAPTURESTATE __user *)arg, sizeof(MS_GFLIP_VECAPTURESTATE)))
    {
        return EFAULT;
    }
    MDrv_GFLIP_GetVECapCurState(&stCurState.bEnable, &stCurState.u8FrameCount);
    stVECaptureState.u8FrameCount = stCurState.u8FrameCount; //return current FrameCount
    if(stCurState.bEnable != stVECaptureState.bEnable)
    {
        stVECaptureState.u8Result = TRUE;//State change, return TRUE
        MDrv_GFLIP_SetVECapCurState(&stVECaptureState.bEnable);//Change current state
    }
    else
    {
        stVECaptureState.u8Result = FALSE;//No State change mean duplicate set, return FALSE
    }
    __put_user(stVECaptureState.u8Result, &(((MS_GFLIP_VECAPTURESTATE __user *)arg)->u8Result));
    __put_user(stVECaptureState.u8FrameCount, &(((MS_GFLIP_VECAPTURESTATE __user *)arg)->u8FrameCount));

    return 0;
}

int _MDrv_GFLIPIO_IOC_GetVECaptureState(struct file *filp, unsigned long arg)
{
    MS_GFLIP_VECAPTURESTATE stCurState;

    if(arg == NULL)
    {
        return EFAULT;
    }
    MDrv_GFLIP_GetVECapCurState(&stCurState.bEnable, &stCurState.u8FrameCount);
    __put_user(TRUE, &(((MS_GFLIP_VECAPTURESTATE __user *)arg)->u8Result));
    __put_user(stCurState.u8FrameCount, &(((MS_GFLIP_VECAPTURESTATE __user *)arg)->u8FrameCount));
    __put_user(stCurState.bEnable, &(((MS_GFLIP_VECAPTURESTATE __user *)arg)->bEnable));
    //printk("IO=%u\n", stCurState.u8FrameCount);

    return 0;
}

int _MDrv_GFLIPIO_IOC_VECaptureWaitOnFrame(struct file *filp, unsigned long arg)
{
    MS_GFLIP_VECAPTURESTATE stVECaptureState;

    if(copy_from_user(&stVECaptureState, (MS_GFLIP_VECAPTURESTATE __user *)arg, sizeof(MS_GFLIP_VECAPTURESTATE)))
    {
        return EFAULT;
    }
    stVECaptureState.u8Result = MDrv_GFLIP_VECapWaitOnFrame(&stVECaptureState.bEnable, &stVECaptureState.u8FrameCount);
    __put_user(stVECaptureState.u8Result, &(((MS_GFLIP_VECAPTURESTATE __user *)arg)->u8Result));
    __put_user(stVECaptureState.bEnable, &(((MS_GFLIP_VECAPTURESTATE __user *)arg)->bEnable));

    return 0;
}

//Below is new method by VE driver
MS_U32 _MDrv_GFLIP_VEC_RegisterInterrupt(MS_BOOL bEnable)
{
    if(bEnable)
    {
        if(0 != (request_irq(E_FIQ_VSYNC_VE4VBI, MDrv_GFLIP_VECINT_IntHandler, SA_INTERRUPT, "gop", NULL)))
        {
            GFLIPIO_KDBG("[GFLIP] Fail to request IRQ:%d\n", E_FIQ_VSYNC_VE4VBI);
            return EFAULT;
        }
        /*
        else
        {
            GFLIPIO_KDBG("[GFLIP] Success to request IRQ:%d\n", E_FIQ_VSYNC_VE4VBI);
        }*/
    }
    else
    {
        free_irq(E_FIQ_VSYNC_VE4VBI,NULL);
        GFLIPIO_KDBG("[GFLIP VEC]  dismiss IRQ:%d\n", E_FIQ_VSYNC_VE4VBI);
    }
    return 0;
}

int _MDrv_GFLIP_VECIO_IOC_EnableVECapture(struct file *filp, unsigned long arg)
{
    MS_GFLIP_VEC_STATE stVECState;
    MS_GFLIP_VEC_STATE stCurVECState;
    MS_GFLIP_VEC_CONFIG stGflipVECConfig;

    if(copy_from_user(&stVECState, (MS_GFLIP_VEC_STATE __user *)arg, sizeof(MS_GFLIP_VEC_STATE)))
    {
        return EFAULT;
    }

    MDrv_GFLIP_GetVECapCurState(&stCurVECState.bEnable, &stCurVECState.u8FrameCount);
    if(stCurVECState.bEnable != stVECState.bEnable)
    {
        stVECState.u8Result = TRUE;//State change, return TRUE
        MDrv_GFLIP_SetVECapCurState(&stVECState.bEnable);//Change current state
    }
    else
    {
        stVECState.u8Result = FALSE;//No State change mean duplicate set, return FALSE
    }
    stVECState.u8FrameCount = stCurVECState.u8FrameCount; //return current FrameCount

    //Register/De-register Interrupt
    MDrv_GFLIP_GetVECaptureConfig(&stGflipVECConfig);
    if(stVECState.u8Result && (MS_VEC_ISR_VE == stGflipVECConfig.eIsrType)) //For disable operation
    {
        if(EFAULT == _MDrv_GFLIP_VEC_RegisterInterrupt(stVECState.bEnable))
        {
            stVECState.u8Result = FALSE;
        }
    }
    __put_user(stVECState.u8Result, &(((MS_GFLIP_VEC_STATE __user *)arg)->u8Result));
    __put_user(stVECState.u8FrameCount, &(((MS_GFLIP_VEC_STATE __user *)arg)->u8FrameCount));
    __put_user(stVECState.bEnable, &(((MS_GFLIP_VEC_STATE __user *)arg)->bEnable));
    return 0;
}

int _MDrv_GFLIP_VECIO_IOC_GetVECaptureState(struct file *filp, unsigned long arg)
{
    MS_GFLIP_VEC_STATE stCurVECState;

    if(arg == NULL)
    {
        return EFAULT;
    }
    MDrv_GFLIP_GetVECapCurState(&stCurVECState.bEnable, &stCurVECState.u8FrameCount);
    __put_user(TRUE, &(((MS_GFLIP_VEC_STATE __user *)arg)->u8Result));
    __put_user(stCurVECState.u8FrameCount, &(((MS_GFLIP_VEC_STATE __user *)arg)->u8FrameCount));
    __put_user(stCurVECState.bEnable, &(((MS_GFLIP_VEC_STATE __user *)arg)->bEnable));
    //printk("IO=%u\n", stCurState.u8FrameCount);

    return 0;
}

int _MDrv_GFLIP_VECIO_IOC_VECaptureWaitOnFrame(struct file *filp, unsigned long arg)
{
    MS_GFLIP_VEC_STATE stVECState;

    if(copy_from_user(&stVECState, (MS_GFLIP_VEC_STATE __user *)arg, sizeof(MS_GFLIP_VEC_STATE)))
    {
        return EFAULT;
    }
    stVECState.u8Result = MDrv_GFLIP_VECapWaitOnFrame(&stVECState.bEnable, &stVECState.u8FrameCount);
    __put_user(stVECState.u8Result, &(((MS_GFLIP_VEC_STATE __user *)arg)->u8Result));
    __put_user(stVECState.bEnable, &(((MS_GFLIP_VEC_STATE __user *)arg)->bEnable));

    return 0;
}

int _MDrv_GFLIP_VECIO_IOC_VECaptureConfig(struct file *filp, unsigned long arg)
{
    MS_U8 u8Length;
    MS_GFLIP_VEC_CONFIG stGflipVECConfig;
    memset(&stGflipVECConfig, 0, sizeof(MS_GFLIP_VEC_CONFIG));
    if(copy_from_user(&stGflipVECConfig, (MS_GFLIP_VEC_CONFIG __user *)arg, VE_VEC_CONFIG_LENGTH_1STVERSION))
    {
        return EFAULT;
    }
    u8Length = (stGflipVECConfig.u16Length > sizeof(MS_GFLIP_VEC_CONFIG)) ? sizeof(MS_GFLIP_VEC_CONFIG) : stGflipVECConfig.u16Length;
    if(VE_VEC_CONFIG_LENGTH_1STVERSION < u8Length)
    {
        if(copy_from_user(&stGflipVECConfig, (MS_GFLIP_VEC_CONFIG __user *)arg, u8Length))
        {
            return EFAULT;
        }
    }
    MDrv_GFLIP_SetVECaptureConfig(&stGflipVECConfig);
    __put_user(stGflipVECConfig.u8Result, &(((MS_GFLIP_VEC_CONFIG __user *)arg)->u8Result));
    return 0;
}

#endif //CONFIG_MSATR_VE_CAPTURE_SUPPORT

int _MDrv_GFLIPIO_IOC_SetInputSigStatus(struct file *filp, unsigned long arg)
{
    MS_BOOL bHasSignal;

    if (__get_user(bHasSignal, (MS_BOOL __user *)arg))
    {
        return EFAULT;
    }

    if(bHasSignal)
    {
        MDrv_GFLIP_RestoreFromVsyncLimitation();
    }

    return 0;
}

int _MDrv_GFLIPIO_IOC_GetDWinIntInfo(struct file *filp, unsigned long arg)
{
    MS_GFLIP_DWININT_INFO stDWinIntInfo;

    if(copy_from_user(&stDWinIntInfo, (MS_GFLIP_DWININT_INFO __user *)arg, sizeof(MS_GFLIP_DWININT_INFO)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_GetDWinIntInfo(&stDWinIntInfo.gflipDWinIntInfo, stDWinIntInfo.bResetDWinIntInfo, 0))
    {
        __put_user(stDWinIntInfo.gflipDWinIntInfo.u8DWinIntInfo, &(((MS_GFLIP_DWININT_INFO __user *)arg)->gflipDWinIntInfo.u8DWinIntInfo));
	    return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

int _MDrv_GFLIPIO_IOC_GetDWinIntInfo2(struct file *filp, unsigned long arg)
{
    MS_GFLIP_DWININT_INFO2 stDWinIntInfo;

    if(copy_from_user(&stDWinIntInfo, (MS_GFLIP_DWININT_INFO2 __user *)arg, sizeof(MS_GFLIP_DWININT_INFO2)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_GetDWinIntInfo(&stDWinIntInfo.gflipDWinIntInfo, stDWinIntInfo.bResetDWinIntInfo,stDWinIntInfo.u32Timeout))
    {
        __put_user(stDWinIntInfo.gflipDWinIntInfo.u8DWinIntInfo, &(((MS_GFLIP_DWININT_INFO __user *)arg)->gflipDWinIntInfo.u8DWinIntInfo));
	    return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

int _MDrv_GFLIPIO_IOC_ClearFlipQueue(struct file *filp, unsigned long arg)
{

    MS_GFLIP_GOPGWINIDX stGFlipQueueIdx;

    if(copy_from_user(&stGFlipQueueIdx, (MS_GFLIP_GOPGWINIDX __user *)arg, sizeof(MS_GFLIP_GOPGWINIDX)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_ClearFlipQueue(stGFlipQueueIdx.u32GopIdx,stGFlipQueueIdx.u32GwinIdx))
    {

	    return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}
int _MDrv_GFLIPIO_IOC_SetGwinInfo(struct file *filp, unsigned long arg)
{

    MS_GWIN_INFO stGwinInfo;

    if(copy_from_user(&stGwinInfo, (MS_GWIN_INFO __user *)arg, sizeof(MS_GWIN_INFO)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetGwinInfo(stGwinInfo))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }

}

int _MDrv_GFLIPIO_IOC_DlcChangeCurveInfo(struct file *filp, unsigned long arg)
{
    MS_DLC_INFO stDlcInfo;

    if(copy_from_user(&stDlcInfo, (MS_DLC_INFO __user *)arg, sizeof(MS_DLC_INFO)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetDlcChangeCurveInfo(stDlcInfo))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

int _MDrv_GFLIPIO_IOC_DlcOnOffInfo(struct file *filp, unsigned long arg)
{

    MS_BOOL bSetDlcOn ;

    if (__get_user(bSetDlcOn, (MS_BOOL __user *)arg))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetDlcOnOffInfo(bSetDlcOn))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

int _MDrv_GFLIPIO_IOC_BleChangeSlopPointInfo(struct file *filp, unsigned long arg)
{
    MS_BLE_INFO stBleInfo;

    if(copy_from_user(&stBleInfo, (MS_BLE_INFO __user *)arg, sizeof(MS_BLE_INFO)))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetBlePointChangeInfo(stBleInfo))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

int _MDrv_GFLIPIO_IOC_SendDlcHistogram32Info(struct file *filp, unsigned long arg)
{
    MS_U16 g_wLumaKernelHistogram32H[32];

    if (!MDrv_GFLIP_GetDlcHistogram32Info(g_wLumaKernelHistogram32H))
    {
        // The return value is not so appropriate.
        return EFAULT;
    }

    copy_to_user( (void*)arg,g_wLumaKernelHistogram32H,sizeof(g_wLumaKernelHistogram32H));

    return 0;
}

int _MDrv_GFLIPIO_IOC_BleOnOffInfo(struct file *filp, unsigned long arg)
{

    MS_BOOL bSetBleOn ;

    if (__get_user(bSetBleOn, (MS_BOOL __user *)arg))
    {
        return EFAULT;
    }

    if (MDrv_GFLIP_SetBleOnOffInfo(bSetBleOn))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return EFAULT;
    }
}

MS_U32 _MDrv_GFLIP_RegisterInterrupt(void)
{
    if(0 != (request_irq(E_IRQ_GOP, MDrv_GFLIPINT_IntHandler, SA_INTERRUPT, "gop", NULL)))
    {
        GFLIPIO_KDBG("[GFLIP] Fail to request IRQ:%d\n", E_IRQ_GOP);
        return EFAULT;
    }

    return 0;
}

MS_U32 _MDrv_GFLIP_DeRegisterInterrupt(void)
{
    GFLIPIO_KDBG("[GFLIP]  dismiss IRQ:%d\n", E_IRQ_GOP);
    free_irq(E_IRQ_GOP,NULL);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_GFLIPIO_Open(struct inode *inode, struct file *filp)
{
    GFLIPIO_KDBG("[GFLIP] GFLIP DRIVER OPEN\n");

    GFLIPIO_ASSERT(_devGFLIP.refCnt>=0);
#if ( defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE))
    GFLIPIO_KDBG("[GFLIP] New flip function enable\n");
    if(_devGFLIP.refCnt == 0)//Init timier when first open gflip
    {
        MDrv_GFLIP_InitTimer();
    }
#endif
    _devGFLIP.refCnt++;
    return 0;
}

int _MDrv_GFLIPIO_Release(struct inode *inode, struct file *filp)
{
    MS_U32 u32Idx;

    GFLIPIO_KDBG("[GFLIP] GFLIP DRIVER CLOSE\n");

    GFLIPIO_ASSERT(_devGFLIP.refCnt>0);
#if ( defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE))
    if(_devGFLIP.refCnt == 1)//delete timier when the last close of gflip
    {
        MDrv_GFLIP_Del_Timer();
    }
#endif
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

    _devGFLIP.refCnt--;

    return 0;
}

#ifdef HAVE_UNLOCKED_IOCTL
int _MDrv_GFLIPIO_IOCtl(struct file *filp, U32 u32Cmd, unsigned long u32Arg)
#else
int _MDrv_GFLIPIO_IOCtl(struct inode *inode, struct file *filp, U32 u32Cmd, unsigned long u32Arg)
#endif
{
    int err = 0;
    int retval = 0;
    if(_devGFLIP.refCnt <= 0)
    {
        return -EFAULT;
    }
    /* check u32Cmd valid */
    if(MDRV_GFLIP_IOC_MAGIC == _IOC_TYPE(u32Cmd))
    {
        if(_IOC_NR(u32Cmd) >= MDRV_GFLIP_IOC_MAX_NR)
        {
            GFLIPIO_KDBG("[GFLIP] IOCtl NR Error!!! (Cmd=%x)\n",u32Cmd);
            return -ENOTTY;
        }
    }
    else if(MDRV_GFLIP_VEC_IOC_MAGIC == _IOC_TYPE(u32Cmd))
    {
        if(_IOC_NR(u32Cmd) >= MDRV_GFLIP_VEC_IOC_MAX_NR)
        {
            GFLIPIO_KDBG("[GFLIP VEC] IOCtl NR Error!!! (Cmd=%x)\n",u32Cmd);
            return -ENOTTY;
        }
    }
    else
    {
        GFLIPIO_KDBG("[GFLIP] IOCtl MAGIC Error!!! (Cmd=%x)\n",u32Cmd);
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
        case MDRV_GFLIP_IOC_INIT:
            retval = _MDrv_GFLIPIO_IOC_Init(filp, u32Arg);
            break;
        //case MDRV_GFLIP_IOC_DEINIT:
            //retval = _MDrv_GFLIPIO_IOC_DeInit(filp, u32Arg);
            //break;
        case MDRV_GFLIP_IOC_SETFLIPINFO:
            retval = _MDrv_GFLIPIO_IOC_SetFlipInfo(filp, u32Arg);
            break;
        case MDRV_GFLIP_IOC_SET3DFLIPINFO:
            retval = _MDrv_GFLIPIO_IOC_Set3DFlipInfo(filp, u32Arg);
            break;
        case MDRV_GFLIP_IOC_SETINPUTSIGSTATUS:
            retval = _MDrv_GFLIPIO_IOC_SetInputSigStatus(filp, u32Arg);
            break;
        case MDRV_GFLIP_IOC_GETDWININTINFO:
            retval = _MDrv_GFLIPIO_IOC_GetDWinIntInfo(filp, u32Arg);
            break;
        case MDRV_GFLIP_IOC_GETDWININTINFO2:
            retval = _MDrv_GFLIPIO_IOC_GetDWinIntInfo2(filp, u32Arg);
            break;
        case MDRV_GFLIP_IOC_SETPIXELIDADDR:
            retval = _MDrv_GFLIPIO_IOC_SetPixelIDAddr(filp, u32Arg);
            break;
        case MDRV_GFLIP_IOC_SETGPIO3DPIN:
            retval = _MDrv_GFLIPIO_IOC_SetGPIO3DPin(filp, u32Arg);
            break;
#if	( defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT))
        //Old method by GOP driver
        case MDRV_GFLIP_IOC_GETVECAPTURESTATE:
            retval = _MDrv_GFLIPIO_IOC_GetVECaptureState(filp, u32Arg);
            break;
        case MDRV_GFLIP_IOC_VECAPTUREWAITONFRAME:
            retval = _MDrv_GFLIPIO_IOC_VECaptureWaitOnFrame(filp, u32Arg);
            break;
        case MDRV_GFLIP_IOC_ENABLEVECAPTURE:
            retval = _MDrv_GFLIPIO_IOC_EnableVECapture(filp, u32Arg);
            break;

        //New method by VE driver
        case MDRV_GFLIP_VEC_IOC_CONFIG:
            retval = _MDrv_GFLIP_VECIO_IOC_VECaptureConfig(filp, u32Arg);
            break;
        case MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE:
            retval = _MDrv_GFLIP_VECIO_IOC_EnableVECapture(filp, u32Arg);
            break;
        case MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE:
            retval = _MDrv_GFLIP_VECIO_IOC_GetVECaptureState(filp, u32Arg);
            break;
        case MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME:
            retval = _MDrv_GFLIP_VECIO_IOC_VECaptureWaitOnFrame(filp, u32Arg);
            break;
#endif //CONFIG_MSATR_VE_CAPTURE_SUPPORT
        case MDRV_GFLIP_IOC_CLEARFLIPQUEUE:
            retval = _MDrv_GFLIPIO_IOC_ClearFlipQueue(filp,u32Arg);
            break;
        case MDRV_GFLIP_IOC_SETGWININFO:
            retval = _MDrv_GFLIPIO_IOC_SetGwinInfo(filp,u32Arg);
            break;
        case MDRV_GFLIP_IOC_DLCCHANGECURVE:
            retval = _MDrv_GFLIPIO_IOC_DlcChangeCurveInfo(filp,u32Arg);
            break;
        case MDRV_GFLIP_IOC_DLCONOFFINFO:
            retval = _MDrv_GFLIPIO_IOC_DlcOnOffInfo(filp,u32Arg);
            break;
        case MDRV_GFLIP_IOC_BLECHANGECURVE:
            retval = _MDrv_GFLIPIO_IOC_BleChangeSlopPointInfo(filp,u32Arg);
            break;
        case MDRV_GFLIP_IOC_BLEONOFFINFO:
            retval = _MDrv_GFLIPIO_IOC_BleOnOffInfo(filp,u32Arg);
            break;
        case MDRV_GFLIP_IOC_DLCGETHISTOGRAMINFO:
            retval = _MDrv_GFLIPIO_IOC_SendDlcHistogram32Info(filp,u32Arg);
            break;
        default:  /* redundant, as cmd was checked against MAXNR */
            GFLIPIO_KDBG("[GFLIP] ERROR IOCtl number %x\n ",u32Cmd);
            return -ENOTTY;
    }

    return retval;
}

static GFLIP_REGS_SAVE_AREA _gflip_regs_save={{{0},{0}}};
static int mstar_gflip_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    disable_irq(E_IRQ_GOP);
    return MDrv_GFLIP_Suspend(&_gflip_regs_save);
}
static int mstar_gflip_drv_resume(struct platform_device *dev)
{
    MDrv_GFLIP_Resume(&_gflip_regs_save);
    enable_irq(E_IRQ_GOP);
    return 0;
}

static int mstar_gflip_drv_probe(struct platform_device *pdev)
{
    int retval=0;
    pdev->dev.platform_data=NULL;
	return retval;
}

static int mstar_gflip_drv_remove(struct platform_device *pdev)
{
    pdev->dev.platform_data=NULL;
    return 0;
}

static struct platform_driver Mstar_gflip_driver = {
	.probe 		= mstar_gflip_drv_probe,
	.remove 	= mstar_gflip_drv_remove,
    .suspend    = mstar_gflip_drv_suspend,
    .resume     = mstar_gflip_drv_resume,

	.driver = {
		.name	= "Mstar-gflip",
        .owner  = THIS_MODULE,
	}
};
//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
int _MDrv_GFLIPIO_ModuleInit(void)
{
    int s32Ret;
    dev_t  dev;

    memset(_filpGopIdxGFLIP, 0, sizeof(_filpGopIdxGFLIP));
    memset(g_u16GOPRefCnt, 0, sizeof(g_u16GOPRefCnt));

    if(_devGFLIP.s32Major)
    {
        dev = MKDEV(_devGFLIP.s32Major, _devGFLIP.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_GFLIP_DEVICE_COUNT, MDRV_GFLIP_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, _devGFLIP.s32Minor, MDRV_GFLIP_DEVICE_COUNT, MDRV_GFLIP_NAME);
        _devGFLIP.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        GFLIPIO_KDBG("[GFLIP] Unable to get major %d\n", _devGFLIP.s32Major);
        return s32Ret;
    }

    cdev_init(&_devGFLIP.cdev, &_devGFLIP.fops);
    if (0 != (s32Ret= cdev_add(&_devGFLIP.cdev, dev, MDRV_GFLIP_DEVICE_COUNT)))
    {
        GFLIPIO_KDBG("[GFLIP] Unable add a character device\n");
        unregister_chrdev_region(dev, MDRV_GFLIP_DEVICE_COUNT);
        return s32Ret;
    }

    /* initial the whole GFLIP Driver */

    if(EFAULT == _MDrv_GFLIP_RegisterInterrupt())
    {
        GFLIPIO_KDBG("[GFLIP] Startup GFLIP Driver Failed! %d\n", _devGFLIP.s32Major);
        cdev_del(&_devGFLIP.cdev);
        unregister_chrdev_region(dev, MDRV_GFLIP_DEVICE_COUNT);
	 return -ENOMEM;
    }
    platform_driver_register(&Mstar_gflip_driver);
    return 0;
}


void _MDrv_GFLIPIO_ModuleExit(void)
{
    /*de-initial the who GFLIPDriver */
    _MDrv_GFLIP_DeRegisterInterrupt();

    cdev_del(&_devGFLIP.cdev);
    unregister_chrdev_region(MKDEV(_devGFLIP.s32Major, _devGFLIP.s32Minor), MDRV_GFLIP_DEVICE_COUNT);
    platform_driver_unregister(&Mstar_gflip_driver);
}



#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
module_init(_MDrv_GFLIPIO_ModuleInit);
module_exit(_MDrv_GFLIPIO_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("GFLIP ioctrl driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)

////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   mdrv_gflip.c
/// @brief  MStar gflip Interface
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MDRV_GFLIP_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM)
#include <asm/io.h>
#endif

#include "mdrv_mstypes.h"
#include "mdrv_gflip.h"
#include "mhal_gflip.h"
#include "mhal_gflip_reg.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Local Defines
//=============================================================================
#define MAX_FLIP_ADDR_FIFO 	(0x10)
#define FLIP_INTERRUPT_TIMEOUT   (100)  //In MS

#if	( defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT))
#define VE_CAPTURE_FRAME_INVALID_NUM 0
#endif

#define MAIN_WINDOW         0
#define SUB_WINDOW          1
#define DLC_INI_FILE        "/config/DLC/DLC.ini"
#define DLC_BUF_SIZE         4000  // number of DLC.ini member length
#define DLC_RULE_BUF_SIZE    4 // number of rule_buf length
#define DLC_DATA_BUF_SIZE    6 // number of data_buf length
#define XC_DCL_DBG(x)        //(x)

#ifndef GFLIP_FILM_DRIVER_VER
#define GFLIP_FILM_DRIVER_VER               1
#endif
#define GFLIP_TIMER_CHECK_TIME          	1 //Check every jiffies

//=============================================================================
// Debug Macros
//=============================================================================
#define GFLIP_DEBUG
#ifdef GFLIP_DEBUG
    #define GFLIP_PRINT(fmt, args...)      printk("[GFlip (Driver)][%05d] " fmt, __LINE__, ## args)
    #define GFLIP_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        GFLIP_PRINT(_fmt, ##_args);  \
                                    }
#else
    #define GFLIP_PRINT(_fmt, _args...)
    #define GFLIP_ASSERT(_cnd, _fmt, _args...)
#endif

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Local Variables
//=============================================================================
static volatile GFLIP_INFO _GFlipInfo[MAX_GOP_SUPPORT][MAX_GOP_GWIN][MAX_FLIP_ADDR_FIFO];
static volatile MS_U32 _u32GFlipInfoReadPtr[MAX_GOP_SUPPORT][MAX_GOP_GWIN];
static volatile MS_U32 _u32GFlipInfoWritePtr[MAX_GOP_SUPPORT][MAX_GOP_GWIN];
static volatile MS_BOOL _bGFlipInVsyncLimitation[MAX_GOP_SUPPORT] = { FALSE };
static volatile MS_GWIN_INFO _u32GwinInfo[MAX_GOP_SUPPORT][MAX_GOP_GWIN];
static volatile MS_DLC_INFO  _bDlcInfo ;
static volatile MS_BLE_INFO  _bBleInfo ;

static GFLIP_DWININT_INFO _GFlipDWinIntInfo = { 0x0 };
static MS_U32 _u32GFlipLatestIntTicket; //The jiffies when Interrupt happen
static DECLARE_WAIT_QUEUE_HEAD(_gflip_waitqueue);
static DECLARE_WAIT_QUEUE_HEAD(_gcapture_waitqueue);

#if	( defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT))
static MS_BOOL s_bEnable = FALSE;
static MS_U8   s_u8FrameCount = VE_CAPTURE_FRAME_INVALID_NUM;
static MS_U8   s_u8FrameNumUpdated = VE_CAPTURE_FRAME_INVALID_NUM;
static MS_GFLIP_VEC_CONFIG s_stGflipVECConfig = {0, sizeof(MS_GFLIP_VEC_CONFIG), 0, 0, 3, 4, 0, MS_VEC_CONFIG_INIT}; //the VEC config setting
#endif
static MS_BOOL _bHistogramReady = FALSE , _bDlcInitReady = FALSE;
static MS_U16  _u16DlcInit[50] ={ 0 };
static MS_U8   _u8DlcCurveInit[65] ={ 0 };
static MS_U8   _u8DlcCont=0;
static MS_BOOL _bSetDlcOn = TRUE;   //TURE : Enable DLC handler  ; FALSE : Disnable DLC handler
#if ( defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE))
static struct timer_list _stGflip_timer;
#endif

//=============================================================================
// Global Variables
//=============================================================================
DEFINE_MUTEX(Semutex_Gflip);
//=============================================================================
// Local Function Prototypes
//=============================================================================
static void _MDrv_GFLIP_ResetFlipInfoPtr(MS_U32 u32GopIdx);
void _MHal_GFLIP_WriteGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16 u16Val, MS_U16 u16Mask);
void _MHal_GFLIP_ReadGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16* u16Val);
//=============================================================================
// Local Function
//=============================================================================
//-------------------------------------------------------------------------------------------------
/// Reset Flip Info Reader/Writer ptr.
/// @param  u32GopIdx    \b IN: the idx of ptr's owner(gop)
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_GFLIP_ResetFlipInfoPtr(MS_U32 u32GopIdx)
{
    MS_U16 u16Idx;

    for(u16Idx=0; u16Idx<MAX_GOP_GWIN; u16Idx++)
    {
        _u32GFlipInfoReadPtr[u32GopIdx][u16Idx] = 0;
        _u32GFlipInfoWritePtr[u32GopIdx][u16Idx] = 0;
    }
}

//=============================================================================
// GFLIP Driver Function
//=============================================================================
#if ( defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE))
MS_U32 MDrv_GFLIP_InitTimer(void)
{
    init_timer( &_stGflip_timer);
    return 0;
}

MS_U32 MDrv_GFLIP_Del_Timer(void)
{
    mutex_lock(&Semutex_Gflip);
    del_timer_sync( &_stGflip_timer);
    mutex_unlock(&Semutex_Gflip);
    return 0;
}
#endif

//-------------------------------------------------------------------------------------------------
/// GFlip Init(reset flip info ptr, interrupt enable, etc..)
/// @param  u32GopIdx                  \b IN:the idx of gop which need be initialized.
/// @return TRUE: success
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_Init(MS_U32 u32GopIdx)
{
    //reset flip Info:
    _MDrv_GFLIP_ResetFlipInfoPtr(u32GopIdx);
    //enable interrupt:
    MHal_GFLIP_IntEnable(u32GopIdx,TRUE);

    if(_u8DlcCont ==0 )
    {
        //For new DLC Algorithm to get the init value.
#if (!defined(CONFIG_MSTAR_URANUS4) && !defined(CONFIG_MSTAR_KRONUS) && !defined(CONFIG_MSTAR_KAISERIN))
        _bDlcInitReady = MDrv_DlcIni(DLC_INI_FILE);
#endif
        _u8DlcCont =1 ;
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// GFlip DeInit(interrupt disable)
/// @param  u32GopIdx                  \b IN:the idx of gop which need be de-initialized.
/// @return TRUE: success
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_DeInit(MS_U32 u32GopIdx)
{
    //disable interrupt:
    mutex_lock(&Semutex_Gflip);
    MHal_GFLIP_IntEnable(u32GopIdx,FALSE);
    mutex_unlock(&Semutex_Gflip);

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Clear Irq
/// @param  void
/// @return TRUE: success
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_ClearIRQ(void)
{
    MS_U32 u32GopIdx;

    MHal_GFLIP_ClearDWINIRQ(&_GFlipDWinIntInfo);

    if ( waitqueue_active(&_gcapture_waitqueue))
    {
        wake_up(&_gcapture_waitqueue);
    }

    for(u32GopIdx=0; u32GopIdx<MAX_GOP_SUPPORT; u32GopIdx++)
    {
        if(MHal_GFLIP_IsVSyncInt(u32GopIdx) == FALSE)
        {
            continue;
        }

        MHal_GFLIP_IntEnable(u32GopIdx, FALSE);

        if(MHal_GFLIP_IsVSyncInt(u32GopIdx) == TRUE)
        {   //Wow...The Vsync Issue Happened...
            MHal_GFLIP_HandleVsyncLimitation(u32GopIdx); //different chip maybe has different handle.
            _bGFlipInVsyncLimitation[u32GopIdx] = TRUE;
            continue;
        }

        MHal_GFLIP_IntEnable(u32GopIdx, TRUE);
    }

    //Record current Time Ticket:
    _u32GFlipLatestIntTicket = jiffies;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Process Irq
/// @param  void
/// @return TRUE: success kickoff flip to gop
/// @return FALSE: no flip kickoff to gop
/// @attention
/// <b>[OBAMA] <em>
///         step1: check src of vsync. interrupt
///         step2: check if any flip request in queue with this Gop
///         step3: check if TAG back
///         step4: set flip to GOP
///         step5: set ReadPtr to next, this entry consumed!
/// </em></b>
//-------------------------------------------------------------------------------------------------
static MS_U32 g_remaplastGwinAddr = 0;
static MS_U32 g_GPIO3DPin = 0;

extern void Chip_Flush_Memory(void);
extern void MDrv_GPIO_Set_High(U8 u8IndexGPIO);
extern void MDrv_GPIO_Set_Low(U8 u8IndexGPIO);

MS_BOOL MDrv_GFLIP_ProcessIRQ(void)
{
    MS_U32 u32GopIdx=0;
    MS_U8 u8WakeUp = FALSE;
#if ( defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT))
    static MS_BOOL bVECaptureEna = FALSE, bVEReady = FALSE;
#endif
#if ( defined (CONFIG_MSATR_3D_LR_REPORT))
    static MS_U32 lasttime=0, thistime = 0;
    static MS_U32 uCount = 0;
#endif

#if (!(defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE)))
    MS_U32 u32GwinIdx=0;
#endif

#if ((defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT)) || (defined (CONFIG_MSATR_3D_LR_REPORT)))
    MS_BOOL bOPVsyncInterrupt = FALSE;
#endif

    for(u32GopIdx=0; u32GopIdx<MAX_GOP_SUPPORT; u32GopIdx++)
    {
        //step 1: check src of vsync. interrupt
        if(MHal_GFLIP_IsVSyncInt(u32GopIdx) == FALSE)
        {
            continue;
        }
#if ( defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE))
        {
#if ((defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT)) || (defined (CONFIG_MSATR_3D_LR_REPORT)))
            if((bOPVsyncInterrupt == FALSE) && (GFLIP_GOP_DST_OP0 == MHal_GFLIP_GetGopDst(u32GopIdx)))
            {
                bOPVsyncInterrupt = TRUE;
                break;
            }
#endif
        }
#else //Old flip logic
#if ((defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT)) || (defined (CONFIG_MSATR_3D_LR_REPORT)))
        if((bOPVsyncInterrupt == FALSE) && (GFLIP_GOP_DST_OP0 == MHal_GFLIP_GetGopDst(u32GopIdx)))
        {
            bOPVsyncInterrupt = TRUE;
        }
#endif

        for(u32GwinIdx=0; u32GwinIdx<MAX_GOP_GWIN; u32GwinIdx++)
        {
            //step 2:check if any flip request in queue with this Gop:
            if(_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx] == _u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx]) //no any flip request in queue
            {
                continue;
            }

            //step 3: if get queue, check if TAG back.
            if(MHal_GFLIP_IsTagIDBack(_GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].u32TagId) == FALSE)
            {
                continue;
            }

            //Has Flip Request, and Tag Returned! we need programming flip address:
            //step 4: if Tag Back: set flip to GOP.
            MHal_GFLIP_SetFlipToGop(u32GopIdx, u32GwinIdx, _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].u32MainAddr, _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].u32SubAddr);

            //Step 5: set ReadPtr to next, this entry consumed!
            _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx] = (_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]+1)%MAX_FLIP_ADDR_FIFO;

            u8WakeUp = TRUE;

        }
#endif
    }

#if ( defined (CONFIG_MSATR_3D_LR_REPORT))
    //[3D TV]when AP call the pixelIDAddr IOCTL, below code will draw two pixel on the screen to tell ursa
    //current LR flag of scaler
    if(bOPVsyncInterrupt)
    {
        if(g_remaplastGwinAddr || g_GPIO3DPin)
        {
            MS_U16 result = (MHal_XC_GetCurrentReadBank() & 0x01);
            if(result == 0)
            {
                if(g_remaplastGwinAddr)
                {
                   (*((volatile MS_U8*)(g_remaplastGwinAddr)))     =  0xFF;
                   (*((volatile MS_U8*)(g_remaplastGwinAddr) + 1)) =  0xFF;
                   (*((volatile MS_U8*)(g_remaplastGwinAddr) + 2)) =  0xFF;
                   (*((volatile MS_U8*)(g_remaplastGwinAddr) + 3)) =  0xFF;

                   (*((volatile MS_U8*)(g_remaplastGwinAddr) + 4)) =  0x01;
                   (*((volatile MS_U8*)(g_remaplastGwinAddr) + 5)) =  0x00;
                   (*((volatile MS_U8*)(g_remaplastGwinAddr) + 6)) =  0x00;
                   (*((volatile MS_U8*)(g_remaplastGwinAddr) + 7)) =  0xFF;

                    Chip_Flush_Memory();
                }

                if(g_GPIO3DPin)
                {
                    MDrv_GPIO_Set_High(g_GPIO3DPin);
                }

            }
            else
            {
                if(g_remaplastGwinAddr)
                {
                    (*((volatile MS_U8*)(g_remaplastGwinAddr)))     =  0x01;
                    (*((volatile MS_U8*)(g_remaplastGwinAddr) + 1)) =  0x00;
                    (*((volatile MS_U8*)(g_remaplastGwinAddr) + 2)) =  0x00;
                    (*((volatile MS_U8*)(g_remaplastGwinAddr) + 3)) =  0xFF;

                    (*((volatile MS_U8*)(g_remaplastGwinAddr) + 4)) =  0xFF;
                    (*((volatile MS_U8*)(g_remaplastGwinAddr) + 5)) =  0xFF;
                    (*((volatile MS_U8*)(g_remaplastGwinAddr) + 6)) =  0xFF;
                    (*((volatile MS_U8*)(g_remaplastGwinAddr) + 7)) =  0xFF;
                    Chip_Flush_Memory();
                }

                if(g_GPIO3DPin)
                {
                    MDrv_GPIO_Set_Low(g_GPIO3DPin);
                }
            }
            #if 1
            thistime = jiffies;
            if(thistime - lasttime >22)
            {
               printk("\n----------timeout:----jiffies=%lu, ucount=%lu\n", thistime - lasttime, uCount++);
            }

            lasttime = thistime;
            #endif
        }
    }
#endif
#if ( defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT))
    if((s_stGflipVECConfig.eIsrType == MS_VEC_ISR_GOP_OP) && bOPVsyncInterrupt) //Only GOP OP isr enter here now
    {
        if(s_bEnable != bVECaptureEna)
        {
            //Enable or disable VE capture
            MHal_GFLIP_VECaptureEnable(s_bEnable); //For old version VEC, enable ve in ISR

            bVECaptureEna = s_bEnable;
            bVEReady = FALSE; //Reset HW state
            if(bVECaptureEna)
            {
                s_u8FrameCount = VE_CAPTURE_FRAME_INVALID_NUM; //Valid Frame is 1~3
                s_u8FrameNumUpdated = VE_CAPTURE_FRAME_INVALID_NUM; //Valid Frame is 1~3
            }
        }
        if(bVECaptureEna)
        {
            //Valid Frame is 1~3
            if(!bVEReady)
            {
                bVEReady = MHal_GFLIP_CheckVEReady();
            }
            else
            {
                //printk("%u\n", s_u8FrameCount);
                if(((s_stGflipVECConfig.bInterlace == FALSE) && (s_u8FrameCount >= s_stGflipVECConfig.u8MaxFrameNumber_P)) ||
                   ((s_stGflipVECConfig.bInterlace == TRUE)  && (s_u8FrameCount >= s_stGflipVECConfig.u8MaxFrameNumber_I))
                  )
                {
                    s_u8FrameCount = VE_CAPTURE_FRAME_INVALID_NUM+1;
                }
                else
                {
                    s_u8FrameCount++;
                }
                s_u8FrameNumUpdated = s_u8FrameCount; //Valid Frame is 1~3
            }
        }
    }
#endif

    if(_bDlcInfo.bCurveChange == TRUE)
    {
        MDrv_GFLIP_Dlc_SetCurve();
        _bDlcInfo.bCurveChange = FALSE;
    }

    if(_bBleInfo.bBLEPointChange == TRUE)
    {
        MDrv_GFLIP_Dlc_SetBlePoint();
        _bBleInfo.bBLEPointChange = FALSE;
    }

    //The new DLC algorithm flow, only support main windows.
    if((_bHistogramReady == TRUE) && (_bDlcInitReady == TRUE) && (_bSetDlcOn ==  TRUE))
    {
        msDlcHandler(MAIN_WINDOW);
        _bHistogramReady = FALSE;
    }

    if(_bDlcInitReady == FALSE)
    {
        XC_DCL_DBG(printk("\n Error in  [Kernel DLC][ %s  , %d ]  DLC Init is Fail  !!!\n",__FUNCTION__,__LINE__));
    }

    if((_bHistogramReady == FALSE) && (_bDlcInitReady == TRUE))
    {
        if(msGetHistogramHandler(MAIN_WINDOW) == TRUE)
        {
            _bHistogramReady = TRUE;
        }
    }

#if (GFLIP_FILM_DRIVER_VER == 1)
    MDrv_GFLIP_FilmDriverHWVer1();
#elif (GFLIP_FILM_DRIVER_VER == 2)
    MDrv_GFLIP_FilmDriverHWVer2();
#endif

    if (u8WakeUp && waitqueue_active(&_gflip_waitqueue))
    {
        wake_up(&_gflip_waitqueue);
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Restore HW Limitation -- Vsync Limitation.
/// @return TRUE: DDI call success
/// @attention
/// <b>[OBAMA] <em>The Signal of GOP Dst from IP/VE restored and we re-enable GFlip. </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_RestoreFromVsyncLimitation(void)
{
    MS_U32 u32GopIdx;

    for(u32GopIdx=0; u32GopIdx<MAX_GOP_SUPPORT; u32GopIdx++)
    {
        if(TRUE == _bGFlipInVsyncLimitation[u32GopIdx])
        {
            MHal_GFLIP_RestoreFromVsyncLimitation(u32GopIdx);
            _bGFlipInVsyncLimitation[u32GopIdx] = FALSE;
        }
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Get DWin Interrupt Info
/// @param  pGFlipDWinIntInfo       \b OUT: the dwin interrupt info
/// @param  bResetDWinIntInfo       \b IN: reset dwin interrupt infor or not
/// @return TRUE: DDI call success
/// @attention
/// <b>[OBAMA] <em>Get DWin Interrupt Info and reset it if requested. </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_GetDWinIntInfo(GFLIP_DWININT_INFO *pGFlipDWinIntInfo, MS_BOOL bResetDWinIntInfo, MS_U32 u32Timeout)
{
    pGFlipDWinIntInfo->u8DWinIntInfo = _GFlipDWinIntInfo.u8DWinIntInfo;
    if(bResetDWinIntInfo)
    {
        _GFlipDWinIntInfo.u8DWinIntInfo = 0x0;
    }

    if(!pGFlipDWinIntInfo->sDwinIntInfo.bDWinIntPROG && u32Timeout > 0)
    {
        interruptible_sleep_on_timeout(&_gcapture_waitqueue, msecs_to_jiffies(u32Timeout));
    }


    return TRUE;
}

#if ( defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE))
MS_BOOL MDrv_GFLIP_IsGOPACK(MS_U32 u32GopIdx)
{
    MS_U16 u16GOPWAck = GOP_REG(REG_GOP_WR_ACK, 0);
    MS_BOOL bGOPACK = FALSE;
    switch(u32GopIdx)
    {
        case 0:
            if(u16GOPWAck&0x1000)
                bGOPACK = FALSE;
            else
                bGOPACK = TRUE;
            break;
        case 1:
            if(u16GOPWAck&0x2000)
                bGOPACK = FALSE;
            else
                bGOPACK = TRUE;
            break;
        case 2:
            if(u16GOPWAck&0x4000)
                bGOPACK = FALSE;
            else
                bGOPACK = TRUE;
            break;
        case 3:
            if(u16GOPWAck&0x8000)
                bGOPACK = FALSE;
            else
                bGOPACK = TRUE;
            break;
        case 4:
            if(u16GOPWAck&0x2000)
                bGOPACK = FALSE;
            else
                bGOPACK = TRUE;
            break;
        default:
            break;
    }
    //printk("<0>" "@-AK=%u->%x\n", bGOPACK, u16GOPWAck);
    return bGOPACK;
}

//-------------------------------------------------------------------------------------------------
/// Kick off Flip to Gop HW.
/// @param  u32GopIdx                  \b IN: The gop idx who is the flip owner
/// @param  u32GwinIdx                 \b IN: The gwin idx who is the flip owner
/// @param  u32MainAddr                \b IN: The flip address for normal 2D display
/// @param  u32SubAddr                 \b IN: The 2nd fb flip address for GOP 3D display
/// @param  bForceWriteIn              \b IN: if use force write to update register, TRUE=force write
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_SetFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32MainAddr, MS_U32 u32SubAddr, MS_BOOL bForceWriteIn)
{
    MS_U16 u16GOPWAck = GOP_REG(REG_GOP_WR_ACK, 0);
    MS_U8 u8BankOffset = MHal_GFLIP_GetBankOffset(u32GopIdx, GFLIP_GOP_BANK_IDX_0);

    u32GwinIdx = MHal_GFLIP_GetValidGWinIDPerGOPIdx(u32GopIdx, u32GwinIdx);

    _MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_1, REG_GOP_DRAM_RBLK_STR_L(u32GwinIdx), (MS_U16)(u32MainAddr), 0xFFFF);
    _MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_1, REG_GOP_DRAM_RBLK_STR_H(u32GwinIdx), (MS_U16)(u32MainAddr>>16), 0xFFFF);

    if(u32SubAddr != 0)
    {
		_MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_1, REG_GOP_3DOSD_SUB_RBLK_L(u32GwinIdx), (MS_U16)(u32SubAddr), 0xFFFF);
        _MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_1, REG_GOP_3DOSD_SUB_RBLK_H(u32GwinIdx), (MS_U16)(u32SubAddr>>16), 0xFFFF);
    }

    if(bForceWriteIn)
    {
        if((u16GOPWAck & GOP_FWR) == 0)
        {
            GOP_REG(REG_GOP_WR_ACK, 0) |= GOP_FWR;
            GOP_REG(REG_GOP_WR_ACK, 0) &= ~GOP_FWR;
        }
    }
    else
    {
        if((u16GOPWAck & GOP_BK_WR) == 0)
        {
            GOP_REG(REG_GOP_WR_ACK, 0) = (u16GOPWAck & (~GOP_BANK_SEL)) | u8BankOffset | GOP_BK_WR;
            GOP_REG(REG_GOP_WR_ACK, 0) &= ~GOP_BK_WR;
            GOP_REG(REG_GOP_WR_ACK, 0) = (GOP_REG(REG_GOP_WR_ACK, 0) & (~GOP_BANK_SEL)) | (u16GOPWAck & GOP_BANK_SEL);//Restore bank
        }
    }
    return TRUE;
}

static void MDrv_GFLIP_Timer_Callback(unsigned long value)
{
    MS_U32 u32GopIdx = 0, u32GwinIdx = 0;
    MS_BOOL bWakeUpQue = FALSE, bContinueTimer = FALSE;

    for(u32GopIdx=0; u32GopIdx<MAX_GOP_SUPPORT; u32GopIdx++)
    {
        for(u32GwinIdx=0; u32GwinIdx<MAX_GOP_GWIN; u32GwinIdx++)
        {
            //step 1:check if any flip request in queue with this gwin
            if(_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx] == _u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx]) //no any flip request in queue
            {
                continue;
            }

            bContinueTimer= TRUE;//There is new flip to check
            //Step 2: check if old request if done, if true:set ReadPtr to next, this entry consumed!
            if(_GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].bKickOff == TRUE) //kicked off, check hw ack
            {
                if(MDrv_GFLIP_IsGOPACK(u32GopIdx))
                {
                    //flip is done
                    bWakeUpQue = TRUE;
                    _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].bKickOff = FALSE;
                    _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx] = (_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]+1)%MAX_FLIP_ADDR_FIFO;
                }
            }
            else //no kick off, try kick off
            {
                //step 3: if get queue, check if TAG back.
                if(MHal_GFLIP_IsTagIDBack(_GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].u32TagId) == FALSE)
                {
                    continue;
                }

                //Has Flip Request, and Tag Returned! we need programming flip address:
                //step 4: if Tag Back: set flip to GOP.
                MDrv_GFLIP_SetFlipToGop(u32GopIdx, u32GwinIdx, _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].u32MainAddr,
                                        _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].u32SubAddr, FALSE);
                _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].bKickOff = TRUE;
                _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]].u32KickOffStartTime = jiffies_to_msecs(jiffies);
            }
        }
    }

    if(bContinueTimer)
    {
        _stGflip_timer.data = u32GopIdx;
        _stGflip_timer.function = MDrv_GFLIP_Timer_Callback;
        _stGflip_timer.expires = jiffies + GFLIP_TIMER_CHECK_TIME;
        mod_timer(&_stGflip_timer, _stGflip_timer.expires);
    }

    if (bWakeUpQue && waitqueue_active(&_gflip_waitqueue))
    {
        wake_up(&_gflip_waitqueue);
    }

}
#endif
//-------------------------------------------------------------------------------------------------
/// Set Flip Info(flip request) to GFlip
/// @param  u32GopIdx       \b IN: the gop which flip kickoff to
/// @param  u32GwinIdx       \b IN: the gwin which flip kickoff to
/// @param  u32Addr             \b IN: the flip address which will be kickoff
/// @param  u32TagId            \b IN: the TagId need be waited before flip
/// @param  u32QEntry           \b IN: the queued flips, if current queued flips >= u32QEntry, won't set this flip request to gop
/// @param  u32QEntry           \b out: the current queued flips in GFLIP
/// @param  u32Result   \b out: kickoff flip info success(TRUE) or fail(FALSE)
/// @return TRUE: DDI call success
/// @attention
/// <b>[OBAMA] <em>Flip when vsync interrupt, and the DDI used for set flip info to GFlip. </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_SetFlipInfo(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32MainAddr, MS_U32 u32SubAddr, MS_U32 u32TagId, MS_U32 * u32QEntry, MS_U32 *u32Result)
{
    MS_U32 u32NextWritePtr;
    MS_U32 u32QCnt = 0;
    MS_U32 cur_jiffies;
    MS_BOOL bRet = TRUE;

    if ((u32GopIdx >= MAX_GOP_SUPPORT) || (u32GwinIdx >= MAX_GOP_GWIN)) //overflow
    {
        *u32Result = FALSE;
        return bRet;
    }

    if(TRUE == _bGFlipInVsyncLimitation[u32GopIdx])
    { //in handling vsync limitation status, just print the error and return true to avoid block app:
        printk("\n Error in MDrv_GFLIP_SetFlipInfo. No Signal for GOP Clock Source and GFlip Vsync Interrupt disabled!!!");
        *u32Result = TRUE;
        return bRet;
    }
    mutex_lock(&Semutex_Gflip);
    u32NextWritePtr = (_u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx]+1) % MAX_FLIP_ADDR_FIFO;
    if(u32NextWritePtr != _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx]) //can write
    {
        if(_u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx] >= _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx])
        {
            u32QCnt = _u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx] - _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx];
        }
        else
        {
            u32QCnt = MAX_FLIP_ADDR_FIFO + _u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx] - _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx];
        }

        if( u32QCnt >= *u32QEntry )
        {
            *u32QEntry = u32QCnt;
            *u32Result = FALSE;
            bRet = TRUE;
            goto _Release_Mutex;
        }

        _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx]].u32MainAddr = (u32MainAddr >> GFLIP_ADDR_ALIGN_RSHIFT);
        _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx]].u32SubAddr = (u32SubAddr >> GFLIP_ADDR_ALIGN_RSHIFT);
        _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx]].u32TagId= u32TagId;
        _GFlipInfo[u32GopIdx][u32GwinIdx][_u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx]].bKickOff   = FALSE;
        _u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx] = u32NextWritePtr;
#if ( defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE))
        mb();
        if(FALSE == timer_pending(&_stGflip_timer)) //no call back timer, then add one
        {
            _stGflip_timer.data = u32GopIdx;
            _stGflip_timer.function = MDrv_GFLIP_Timer_Callback;
            _stGflip_timer.expires = jiffies + GFLIP_TIMER_CHECK_TIME;
            mod_timer(&_stGflip_timer, _stGflip_timer.expires);
        }
#endif
        //Hold on when New QCnt can't match requested QCnt:
        //Fix issue if we set Flip Info and back which leads to App write to on-show window.
        cur_jiffies = jiffies;
        while(TRUE)
        {
            if(_u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx] >= _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx])
            {
                u32QCnt = _u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx] - _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx];
            }
            else
            {
                u32QCnt = MAX_FLIP_ADDR_FIFO + _u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx] - _u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx];
            }

            if( u32QCnt < *u32QEntry )
            {
                break;
            }

            if(time_after(jiffies, (msecs_to_jiffies(FLIP_INTERRUPT_TIMEOUT)+cur_jiffies)))
            {
                break;//Time out skip
            }
            interruptible_sleep_on_timeout(&_gflip_waitqueue, msecs_to_jiffies(FLIP_INTERRUPT_TIMEOUT));
        }

        //since do not do the sync. with mutex/spinlock, the return of avaiable queue number maybe not accurate.
        *u32QEntry = u32QCnt + 1;
        *u32Result = TRUE;
        bRet = TRUE;
    }
    else
    {
        *u32QEntry = MAX_FLIP_ADDR_FIFO-1;
        *u32Result = FALSE;
        bRet = TRUE;
    }
_Release_Mutex:
    mutex_unlock(&Semutex_Gflip);
    return bRet;
}

MS_BOOL MDrv_GFLIP_SetPixelIDAddr(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32Addr, MS_U32 u32TagId, MS_U32 * u32QEntry, MS_U32 *u32Result)
{
    if ((u32GopIdx >= MAX_GOP_SUPPORT) || (u32GwinIdx >= MAX_GOP_GWIN)) //overflow
    {
        *u32Result = FALSE;
        return TRUE;
    }

    *u32Result = TRUE;
    if(u32Addr != 0)
    {
#if defined(CONFIG_MIPS)
        g_remaplastGwinAddr = (MS_U32)__ioremap(u32Addr + 0, 100, _CACHE_UNCACHED);
#elif defined(CONFIG_ARM)
        g_remaplastGwinAddr = (MS_U32) ioremap(u32Addr + 0, 100);
#endif
    }
    else
    {
        g_remaplastGwinAddr = 0;
    }

    return TRUE;
}

MS_BOOL MDrv_GFLIP_SetGPIO3DPin(MS_U32 u32Addr, MS_U32 *u32Result)
{
    *u32Result = TRUE;

    g_GPIO3DPin = u32Addr;

    return TRUE;
}

#if	( defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT))
//-------------------------------------------------------------------------------------------------
/// Get VE capture state: Enable state and the FrameNumber that has been full captured
/// @param  pbEna                  \b OUT: TRUE: Enable, FALSE: Disable
/// @param  pu8FramCount           \b OUT: Current captured FrameNumber
/// @return TRUE: success
/// @return FALSE: fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_GetVECapCurState(MS_BOOL *pbEna, MS_U8 *pu8FramCount)
{
    *pbEna = s_bEnable;
    *pu8FramCount = s_u8FrameCount;
    //printk("KL: %u,%u\n", s_bEnable, s_u8FrameCount);
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Wait on the finish of specified frame: Check if the frame is captured,
/// if Yes, return TRUE, otherwise sleep until the next Vsync ISR
/// @param  pbEna                  \b OUT: TRUE: Enable, FALSE: Disable
/// @param  pu8FramCount           \b IN : Specified frame number to waiting
/// @return TRUE: success
/// @return FALSE: fail
//-------------------------------------------------------------------------------------------------
#define TIMEOUT_SLEEP 60
MS_BOOL MDrv_GFLIP_VECapWaitOnFrame(MS_BOOL *pbEna, MS_U8 *pu8FramNum)
{
    MS_BOOL bret = FALSE;
    *pbEna = s_bEnable;
    if(s_bEnable == TRUE)
    {
        if(((s_stGflipVECConfig.bInterlace == FALSE) && (s_u8FrameNumUpdated == *pu8FramNum)) ||
           ((s_stGflipVECConfig.bInterlace == TRUE ) && (s_u8FrameNumUpdated == (*pu8FramNum<<1)))
          )
        {
            bret = TRUE;//State change, return TRUE
            s_u8FrameNumUpdated = VE_CAPTURE_FRAME_INVALID_NUM; //reset to wait next update frame
            //printk("<0>" "WS=%u\n", *pu8FramNum);
        }
        else
        {
            interruptible_sleep_on_timeout(&_gcapture_waitqueue, msecs_to_jiffies(TIMEOUT_SLEEP));
            //printk("<0>" "WF=%u\n", *pu8FramNum);
        }
    }
    else
    {
        interruptible_sleep_on_timeout(&_gcapture_waitqueue, msecs_to_jiffies(TIMEOUT_SLEEP));
    }
    //printk("KL: %u,%u\n", s_bEnable, s_u8FrameCount);
    return bret;
}

//-------------------------------------------------------------------------------------------------
/// Set VE capture state: Enable or Disable VE capture function
/// @param  pbEna                  \b IN: TRUE: Enable, FALSE: Disable
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
///           the enable or disable operation will be taken in the following vsync interrupt process.
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_SetVECapCurState(MS_BOOL *pbEna)
{
    //printk("<0>" "VEC state change=%u->%u\n", s_bEnable, *pbEna);
    s_bEnable = *pbEna;
    s_u8FrameCount = VE_CAPTURE_FRAME_INVALID_NUM;
    s_u8FrameNumUpdated = VE_CAPTURE_FRAME_INVALID_NUM; //Valid Frame is 1~3 for P mode or 1~4 for interlace mode
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Config the VE capture
/// otherwise sleep until the next Vsync ISR
/// @param  pstGflipVECConfig      \b IN : @ref PMS_GFLIP_VEC_CONFIG
/// @return TRUE: success
/// @return FALSE: fail
//-------------------------------------------------------------------------------------------------
void MDrv_GFLIP_GetVECaptureConfig(PMS_GFLIP_VEC_CONFIG pstGflipVECConfig)
{
    memcpy(pstGflipVECConfig, &s_stGflipVECConfig, sizeof(MS_GFLIP_VEC_CONFIG));
}

void MDrv_GFLIP_SetVECaptureConfig(PMS_GFLIP_VEC_CONFIG pstGflipVECConfig)
{
    pstGflipVECConfig->u8Result = TRUE;
    if(pstGflipVECConfig->eConfigType == MS_VEC_CONFIG_ENABLE)
    {
        s_stGflipVECConfig.eConfigType = MS_VEC_CONFIG_ENABLE;
        s_stGflipVECConfig.bInterlace  = pstGflipVECConfig->bInterlace;
    }
    else if(pstGflipVECConfig->eConfigType == MS_VEC_CONFIG_INIT)
    {
        memcpy(&s_stGflipVECConfig, pstGflipVECConfig, sizeof(MS_GFLIP_VEC_CONFIG));
        s_stGflipVECConfig.u8MaxFrameNumber_I <<= 1; //In kernel it is counted by field for interlace in ISR
        if(s_stGflipVECConfig.eIsrType >= MS_VEC_ISR_MAXNUM)
        {
            s_stGflipVECConfig.eIsrType = MS_VEC_ISR_GOP_OP;
            pstGflipVECConfig->u8Result = FALSE;//Unknown config type
        }
    }
    else
    {
        pstGflipVECConfig->u8Result = FALSE;//Unknown config type
    }
}

//-------------------------------------------------------------------------------------------------
/// Clear Irq
/// @param  void
/// @return TRUE: success
/// @attention
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_VEC_ClearIRQ(void)
{
    if(s_stGflipVECConfig.eIsrType == MS_VEC_ISR_VE)
    {
        //No need to clear for FIQ interrupt, just wake up queue
        if ( waitqueue_active(&_gcapture_waitqueue))
        {
            wake_up(&_gcapture_waitqueue);
        }
        return TRUE;
    }
    return FALSE;
}

//-------------------------------------------------------------------------------------------------
/// Process Irq
/// @param  void
/// @return TRUE: success
/// @attention
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_VEC_ProcessIRQ(void)
{
    static MS_BOOL bVEReady = FALSE;
    if(s_stGflipVECConfig.eIsrType == MS_VEC_ISR_VE)
    {
        if(s_bEnable && (s_u8FrameCount == VE_CAPTURE_FRAME_INVALID_NUM))
        {
            //VEC restart, reset state
            bVEReady = MHal_GFLIP_CheckVEReady();
        }
        if(bVEReady)
        {
            //Valid Frame is 1~3 for P mode or 1~4 for interlace mode
            //But HW is count from 0, so need conversion

            s_u8FrameCount = MHal_GFLIP_GetFrameIdx();
            if(s_u8FrameCount == 0)
            {
                if(s_stGflipVECConfig.bInterlace == FALSE)
                {
                    s_u8FrameCount = s_stGflipVECConfig.u8MaxFrameNumber_P;
                }
                else
                {
                    s_u8FrameCount = s_stGflipVECConfig.u8MaxFrameNumber_I;
                }
            }
            s_u8FrameNumUpdated = s_u8FrameCount;
        }
        //printk("<0>" "F=%u\n", s_u8FrameCount);
        return TRUE;
    }
    return FALSE;
}

#endif //CONFIG_MSATR_VE_CAPTURE_SUPPORT

//-------------------------------------------------------------------------------------------
///Clear the FlipQueue by the  gop index and gwin Index
///@param u32GopIndex
///@param u32GwinIndex
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_ClearFlipQueue(MS_U32 u32GopIdx,MS_U32 u32GwinIdx)
{
    mutex_lock(&Semutex_Gflip);
	_u32GFlipInfoWritePtr[u32GopIdx][u32GwinIdx]=_u32GFlipInfoReadPtr[u32GopIdx][u32GwinIdx] = 0;
	//printk("\nClear the gop :%d gwin:%d\n",u32GopIdx,u32GwinIdx);
	mutex_unlock(&Semutex_Gflip);
	return TRUE;
}
//-------------------------------------------------------------------------------------------
///Set GWIN info
///@param stGwinInfo
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_SetGwinInfo(MS_GWIN_INFO stGwinInfo)
{

    _u32GwinInfo[stGwinInfo.u8GopIdx][stGwinInfo.u8GwinIdx].u32Addr =  stGwinInfo.u32Addr;
    _u32GwinInfo[stGwinInfo.u8GopIdx][stGwinInfo.u8GwinIdx].u16X =  stGwinInfo.u16X;
    _u32GwinInfo[stGwinInfo.u8GopIdx][stGwinInfo.u8GwinIdx].u16Y =  stGwinInfo.u16Y;
    _u32GwinInfo[stGwinInfo.u8GopIdx][stGwinInfo.u8GwinIdx].u16W =  stGwinInfo.u16W;
    _u32GwinInfo[stGwinInfo.u8GopIdx][stGwinInfo.u8GwinIdx].u16H =  stGwinInfo.u16H;
    _u32GwinInfo[stGwinInfo.u8GopIdx][stGwinInfo.u8GwinIdx].u8GopIdx =  stGwinInfo.u8GopIdx;
    _u32GwinInfo[stGwinInfo.u8GopIdx][stGwinInfo.u8GwinIdx].u8GwinIdx =  stGwinInfo.u8GwinIdx;
    _u32GwinInfo[stGwinInfo.u8GopIdx][stGwinInfo.u8GwinIdx].clrType =  stGwinInfo.clrType;

     return TRUE;
}
//-------------------------------------------------------------------------------------------
///Get GWIN info
///@param u32GopIndex
///@param u32GwinIndex
///@return FALSE :MS_GWIN_INFO
//---------------------------------------------------------------------------------------------

MS_GWIN_INFO MDrv_GFLIP_GetGwinInfo(MS_U8 u8GopIdx,MS_U8 u8GwinIdx)
{
    return _u32GwinInfo[u8GopIdx][u8GwinIdx];
}

//-------------------------------------------------------------------------------------------
///Set Dlc Change Curve Info
///@param stDlcInfo
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_SetDlcChangeCurveInfo(MS_DLC_INFO stDlcInfo)
{
    MS_U8 i;
    _bDlcInfo.bCurveChange =  stDlcInfo.bCurveChange;

     for (i=0; i<16; i++)
     {
         _bDlcInfo.u8LumaCurve[i]     = stDlcInfo.u8LumaCurve[i];
         _bDlcInfo.u8LumaCurve2_a[i]  = stDlcInfo.u8LumaCurve2_a[i];
         _bDlcInfo.u8LumaCurve2_b[i]  = stDlcInfo.u8LumaCurve2_b[i];
     }

     return TRUE;
}

//-------------------------------------------------------------------------------------------
///Set Dlc On Off Info
///@param bSetDlcOn
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_SetDlcOnOffInfo(MS_BOOL bSetDlcOn )
{
    _bSetDlcOn =  bSetDlcOn;

     return TRUE;
}

//-------------------------------------------------------------------------------------------
///Set BLE point Change Info
///@param stBleInfo
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_SetBlePointChangeInfo(MS_BLE_INFO stBleInfo)
{
    MS_U8 i;

    _bBleInfo.bBLEPointChange =  stBleInfo.bBLEPointChange;

     for (i=0; i<6; i++)
     {
         _bBleInfo.u16BLESlopPoint[i] = stBleInfo.u16BLESlopPoint[i];
     }

     return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Get histogram data
/// @param pu16Histogram                \b OUT: the value of histogram
/// @return FALSE :fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_GetDlcHistogram32Info(MS_U16 *pu16Histogram)
{
    MS_BOOL bret = FALSE;

    bret = msDlcGetHistogram32Info(pu16Histogram);

    return bret;
}

//-------------------------------------------------------------------------------------------
///Set Ble On Off Info
///@param bSetBleOn
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_GFLIP_SetBleOnOffInfo(MS_BOOL bSetBleOn )
{
    msDlcSetBleOnOff(bSetBleOn);
    return TRUE;
}

int MDrv_GFLIP_Suspend(GFLIP_REGS_SAVE_AREA *pSave)
{
    int i,j;
    for(i=0;i<GFLIP_REG_BANKS;i++){
        for(j=0;j<GFLIP_REG16_NUM_PER_BANK;j++){
            _MHal_GFLIP_ReadGopReg((i/GFLIP_BANKS_PER_GOP),
                (i%GFLIP_BANKS_PER_GOP), j, &(pSave->BankReg[i][j]));
        }
    }
    return 0;
}

int MDrv_GFLIP_Resume(GFLIP_REGS_SAVE_AREA *pSave)
{
    int i,j;
    int GopIdx, GopBks;
    unsigned short GWinEnable[GFLIP_REG_BANKS/GFLIP_BANKS_PER_GOP]={0};
    for(i=0;i<GFLIP_REG_BANKS;i++){
        GopIdx=i/GFLIP_BANKS_PER_GOP;
        GopBks=i%GFLIP_BANKS_PER_GOP;
        if(GopBks==0){//reset gop
            GWinEnable[GopIdx]=pSave->BankReg[i+1][0];
            pSave->BankReg[i+1][0] &= 0xFFFE;
        }
        for(j=0;j<GFLIP_REG16_NUM_PER_BANK;j++){
            _MHal_GFLIP_WriteGopReg(GopIdx,
                GopBks, j, (pSave->BankReg[i][j]),0xFFFF);
        }
    }
    _MHal_GFLIP_WriteGopReg(0,GFLIP_GOP_BANK_IDX_0,REG_GOP_WR_ACK,GOP_FWR,GOP_FWR);
    _MHal_GFLIP_WriteGopReg(0,GFLIP_GOP_BANK_IDX_0,REG_GOP_WR_ACK,0,GOP_FWR);

    for(i=0;i<(GFLIP_REG_BANKS/GFLIP_BANKS_PER_GOP);i++){//restore gwin enable
        _MHal_GFLIP_WriteGopReg(i,
        GFLIP_GOP_BANK_IDX_0, REG_GOP4G_CTRL0, GOP_RST_MASK,GOP_RST_MASK);
        mdelay(5);
        _MHal_GFLIP_WriteGopReg(i,
        GFLIP_GOP_BANK_IDX_0, REG_GOP4G_CTRL0, 0,GOP_RST_MASK);
        mdelay(5);
    }

    for(i=0;i<(GFLIP_REG_BANKS/GFLIP_BANKS_PER_GOP);i++){//restore gwin enable
        _MHal_GFLIP_WriteGopReg(i,
        GFLIP_GOP_BANK_IDX_1, REG_GOP4G_GWIN_CTRL0(0), GWinEnable[i],0xFFFF);
    }

    _MHal_GFLIP_WriteGopReg(0,GFLIP_GOP_BANK_IDX_0,REG_GOP_WR_ACK,GOP_WR,GOP_WR);
    _MHal_GFLIP_WriteGopReg(0,GFLIP_GOP_BANK_IDX_0,REG_GOP_WR_ACK,0,GOP_WR);
    return 0;
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init table from DLC.ini
///@param DLC_con_file
///@return TRUE: success
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_DlcIni(char *con_file)
{
    char *buf;
    struct file *fp;
    int ret=0;
    MS_BOOL  bReturn= FALSE;

    //When accessing user memory, we need to make sure the entire area really is in user-level space.
    //KERNEL_DS addr user-level space need less than TASK_SIZE
    mm_segment_t old_fs=get_fs();
    set_fs(KERNEL_DS);

    fp = filp_open(con_file, O_RDONLY , 0x400);

    if (!fp)
    {
        return FALSE;
    }

    buf = kzalloc(DLC_BUF_SIZE+1, GFP_KERNEL);
    if(!buf)
		return FALSE;

    if (fp->f_op && fp->f_op->read)
    {
        ret = fp->f_op->read(fp, buf, DLC_BUF_SIZE, &fp->f_pos);
    }

    if (ret < 0)
    {
        set_fs(old_fs);
        filp_close(fp, NULL);
        return FALSE;
    }

    MDrv_ReadDlcCurveInfo(buf);

    MDrv_ReadDlcInfo(buf);

    msDlcSetCurve(_u8DlcCurveInit);

    bReturn = msDlcInit(_u16DlcInit );

    set_fs(old_fs);
    filp_close(fp, NULL);
    kfree(buf);
    return bReturn;
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init curve table from DLC.ini
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_ReadDlcCurveInfo(char *buf)
{
    MS_U16 u16BufIdx=0;
    MS_U8 u8DataIdx=0, u8CntSwitch=0, u8InitCnt=0;
    char data_buf[3]= { 0 };
    data_buf[2] = '\0';

    for(u16BufIdx=0 ; u16BufIdx<(DLC_BUF_SIZE-4) ; u16BufIdx++)
    {
        if(u8CntSwitch==0)
        {
            //The two symbols of '#&' is start to load curve table for Kernel.
            if((buf[u16BufIdx] == '#')&&(buf[u16BufIdx+1] == '&'))
                u8CntSwitch=1;//The u8CntSwitch = 1 between '#&' and '#%' .

            continue;
        }

        // Get the four curves init value .
        if((buf[u16BufIdx] == '0')&&(buf[u16BufIdx+1] == 'x'))
        {
            //Clear the data_buf
            for(u8DataIdx=0 ; u8DataIdx<2 ; u8DataIdx++)
            {
                data_buf[u8DataIdx]=0;
            }
            //Get the two values after '0x' .
            data_buf[0] = buf[u16BufIdx+2];
            data_buf[1] = buf[u16BufIdx+3];

            //Save the data value to _u8DlcCurveInit .
            _u8DlcCurveInit[u8InitCnt] = simple_strtol(data_buf, NULL, 16) ;
            u8InitCnt++;
        }
        //The two symbols of '#%' is end to load curve table for Kernel .
        if((buf[u16BufIdx]=='#')&&(buf[u16BufIdx+1]=='%'))
            break;
    }
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init table from DLC.ini
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_ReadDlcInfo(char *buf)
{
    MS_U16 u16BufIdx=0,u16BufIdx_2=0;
    MS_U8 u8DataIdx=0, u8RuleIdx=0, u8CntSwitch=0;
    char data_buf[DLC_DATA_BUF_SIZE]= { 0 }, rule_buf[DLC_RULE_BUF_SIZE]= { 0 };

    for(u16BufIdx=0 ; u16BufIdx<(DLC_BUF_SIZE-4) ; u16BufIdx++)
    {
        if(u8CntSwitch==0)
        {
            //The three symbols of '#@@' is start to load init value for Kernel.
            if((buf[u16BufIdx] == '#')&&(buf[u16BufIdx+1] == '@')&&(buf[u16BufIdx+2] == '@'))
                u8CntSwitch=1;

            continue;
        }

        // Get the Data value
        if(buf[u16BufIdx] == '=')
        {
            //Clear the data_buf
            for(u8DataIdx=0;u8DataIdx<(DLC_DATA_BUF_SIZE-1);u8DataIdx++)
            {
                data_buf[u8DataIdx]=0;
            }
            data_buf[DLC_DATA_BUF_SIZE-1]='\0';

            //Get the value between '=' and ';' , if the value is ' ' , can't to save.
            for(u16BufIdx_2=u16BufIdx+1,u8DataIdx=0;(buf[u16BufIdx_2]!=';')&&(u8DataIdx<DLC_DATA_BUF_SIZE);u16BufIdx_2++)
            {
               if(buf[u16BufIdx_2]!=' ')
               {
                   data_buf[u8DataIdx]=buf[u16BufIdx_2];
                   u8DataIdx++;
               }
            }
            u8CntSwitch=2;
        }

        if(u8CntSwitch==2)
        {
            // Get the Rule value after '#@'
            if((buf[u16BufIdx]=='#')&&(buf[u16BufIdx+1]=='@'))
            {
                //Clear the rule_buf
                for(u8RuleIdx=0;u8RuleIdx<(DLC_RULE_BUF_SIZE-1);u8RuleIdx++)
                {
                    rule_buf[u8RuleIdx]=0;
                }
                rule_buf[DLC_RULE_BUF_SIZE-1] = '\0';
                //Get the rule value between '#@' and ' ' .
                for(u16BufIdx_2=u16BufIdx+2,u8RuleIdx=0;(buf[u16BufIdx_2]!=' ')&&(u8RuleIdx<DLC_RULE_BUF_SIZE);u16BufIdx_2++)
                {
                    rule_buf[u8RuleIdx]=buf[u16BufIdx_2];
                    u8RuleIdx++;
                }
                u8CntSwitch=1;

                //Save the Data and rule value to _u16DlcInit .
                _u16DlcInit[simple_strtol(rule_buf, NULL, 10)] = simple_strtol(data_buf, NULL, 10);
            }
        }
        //The three symbols of '#!@' is end to load init value for Kernel .
        if((buf[u16BufIdx]=='#')&&(buf[u16BufIdx+1]=='!')&&(buf[u16BufIdx+2]=='@'))
            break;
    }
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init table from DLC.ini
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_GFLIP_Dlc_SetCurve(void)
{
    BYTE ucTmp;

    for(ucTmp=0; ucTmp<16; ucTmp++)
    {
        _u8DlcCurveInit[ucTmp]=_bDlcInfo.u8LumaCurve[ucTmp] ;
        _u8DlcCurveInit[ucTmp+16]=_bDlcInfo.u8LumaCurve2_a[ucTmp];
        _u8DlcCurveInit[ucTmp+32]=_bDlcInfo.u8LumaCurve2_b[ucTmp] ;
    }
    msDlcSetCurve(_u8DlcCurveInit);
}

//-------------------------------------------------------------------------------------------------
///Get the Ble slop point from DLC.ini
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_GFLIP_Dlc_SetBlePoint(void)
{
    WORD _u16BLESlopPoint[6] = { 0 };
    BYTE ucTmp;

    for (ucTmp=0; ucTmp<6; ucTmp++)
    {
        _u16BLESlopPoint[ucTmp] = _bBleInfo.u16BLESlopPoint[ucTmp];
    }

    msDlcSetBleSlopPoint(_u16BLESlopPoint);
}

EXPORT_SYMBOL(MDrv_GFLIP_GetGwinInfo);

#define FPLL_PHASE_VAR_LIMIT    (0x10)          // If phase error variation exceed this, means un-stable
#define FPLL_PRD_THRESHOLD      (0x100)         // The PRD must be smaller than this value to indicate almost finished PRD lock
#define FPLL_PRD_VAR_LIMIT      (0x10)          // If PRD variation exceed this, means un-stable

MS_BOOL MDrv_GFLIP_FrameLockCheck (void)
{
    MS_U16 u16NewPhase,u16NewPrd;
    static MS_U16 _u16OrigPhase,_u16OrigPrd;

    u16NewPrd = MApi_XC_R2BYTEMSK(REG_LPLL_13_L, 0xFFFF);
    u16NewPhase = MApi_XC_R2BYTEMSK(REG_LPLL_11_L, 0xFFFF);

    if((u16NewPrd < FPLL_PRD_THRESHOLD) && (abs(_u16OrigPrd - u16NewPrd) < FPLL_PRD_VAR_LIMIT)
        && (abs(_u16OrigPhase - u16NewPhase) < FPLL_PHASE_VAR_LIMIT))
    {
        _u16OrigPhase = u16NewPhase;
        _u16OrigPrd = u16NewPrd;
        return TRUE;
    }
    else
    {
        _u16OrigPhase = u16NewPhase;
        _u16OrigPrd = u16NewPrd;
        return FALSE;
    }
}

void MDrv_GFLIP_FilmDriverHWVer1(void)
{
    MS_BOOL bIsFilmXOO = 0;
    MS_U32 u32Film22AdaptiveCounter1Th = 0;
    static MS_U16 _u16RdBank_Prv=0;
    MS_U16 u16RdBank;
    MS_U32 u32CurCounter1=0, u32CurCounter3=0, u32FixThofC3=0;
    static MS_U32 _u32Counter3_2=0, _u32Counter3_3=0;
    static MS_U32 _u32Counter1_2=0, _u32Counter1_3=0, _u32Counter1_4=0;
    static MS_U32 _u32CounterA_2=0, _u32CounterA_3=0;
    static MS_U32 _u32CounterB_2=0, _u32CounterB_3=0;
    static MS_U8 _u8Film22LikeCounter, _u8Film22LikeCounter2;
    static MS_U8 _u8Film22LikeCounter_ForSlowMotionDisableMCDi;
    static MS_U8 _u8SceneChangeFrame;
    static MS_U8 _u8IsFilmXOOCounter=0;
    static MS_BOOL _bIs2ndFilmXOOCounter;
    MS_BOOL bCounter1Have2xDiff, bFilm, bFilmAny;
    MS_U32 u32CurCounterA, u32CurCounterB;
    static MS_U8 _u8SmallCounter3Cnt;
    static MS_U8 _u8Film22LikeCounterA, _u8Film22LikeCounterB;
    static MS_U8 _u8Film22LikeCounter3_Big;
    static MS_U8 _u8Film32LikeCounter3_Big;
    static MS_U8 _u8Film22LikeCounter1_Big;
    static MS_U8 _u8VerticalCnt, _u8Unmatch3Cnt;
    static MS_U8 _u8FilmsmallCounter1 = 0 ;
    MS_BOOL bIsHDSource;
    MS_U16 u16Unmatch_3, u16Unmatch_1;
    static MS_U8 _u8NoCounter3_Cnt, _u8BigCounter3_Cnt, _u8BigCounter1_Cnt, _u8BigCounter_Cnt3;
    static MS_U8 _u8BigCounter1_Cnt2, _u8SmallCounter1_Cnt, _u8BigCounter3_Cnt2;
    MS_BOOL bVerticalMovingU ;
    MS_BOOL bIs60Hz = ( MApi_XC_R2BYTE(REG_SC_BK0A_10_L) & 0x8000 ) != 0; // checkfilm32 enable for 60Hz source
    static MS_U8 _u8StillCounter3;
    MS_U32 u32Counter3;
    static MS_U8 _u8MovingCharacterCounter;
#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    MS_U32 u32Reg7C, u32Reg7D, u32Reg7E;
#endif
    MS_U16 u16InvStillFrameCnt;

    u16RdBank = ( MApi_XC_R2BYTE(REG_SC_BK12_3A_L) & 0xF0 ) >> 4;
    bIsHDSource = ( MApi_XC_R2BYTEMSK(REG_SC_BK12_0F_L, 0xFFFF) > 0x700 );

    u32CurCounterA = 0  ; // MApi_XC_R2BYTE(REG_SC_BK0A_4A_L);
    u32CurCounterB = 0  ; // MApi_XC_R2BYTE(REG_SC_BK0A_4B_L);

    u32CurCounter1 = MApi_XC_R2BYTE(REG_SC_BK0A_0A_L);
    u32CurCounter3 = MApi_XC_R2BYTE(REG_SC_BK0A_0B_L);

    if(u16RdBank == _u16RdBank_Prv)  // Not change read bank
    {
        return ;
    }

    bFilmAny = ( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040);
    bFilm = (( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008) ||  // film22
            ( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010) ||  // film32
            ( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040) );   // film any

    // check XOO
    if( ( (!bFilm) && (u32CurCounter3 > 0x200) && (_u32Counter3_2< 0x80) && (_u32Counter3_3< 0x80) ) || //not film
        ( (!bFilm) && (u32CurCounter3 > 0x80) && (_u32Counter3_2< 0x20) && (_u32Counter3_3< 0x20) ) ) //not film
    {
        if ( _u8IsFilmXOOCounter > 0 )
        {
            _bIs2ndFilmXOOCounter = true;
        }

        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if( bFilm && (!bFilmAny) && (u32CurCounter3 > 0x200) && (u32CurCounter3 > (_u32Counter3_2*2)) && (u32CurCounter3 > (_u32Counter3_3*2)) ) //32 or 22
    {
        if( _u8IsFilmXOOCounter > 0 )
        {
            _bIs2ndFilmXOOCounter = true;
        }
        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if(  bFilm && (u32CurCounter3 > 0x80)  && (u32CurCounter3 > (_u32Counter3_2*2)) && (u32CurCounter3 > (_u32Counter3_3*2)) ) // film-any
    {
        _bIs2ndFilmXOOCounter = true;
        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if( _u8IsFilmXOOCounter != 0 )
    {
        _u8IsFilmXOOCounter --;
    }
    else
    {
        _bIs2ndFilmXOOCounter = false;
    }

    //detect scene change into film...
    if( (u32CurCounter1 > (_u32Counter1_4*4)) && (u32CurCounter1 > (_u32Counter1_2*4)) && (u32CurCounter1 > (_u32Counter1_3*4)) )
        _u8SceneChangeFrame = 60;
    else if( _u8SceneChangeFrame != 0 )
        _u8SceneChangeFrame --;

    if( (u32CurCounter3 > 0x80) && (_u32Counter3_2 < 0x10) && (_u32Counter3_3 > 0x80) )
        _u8Film22LikeCounter += 2;
    else if( (u32CurCounter3 < 0x10) && (_u32Counter3_2 > 0x80) && (_u32Counter3_3 < 0x10) )
        _u8Film22LikeCounter += 2;
    else if( (u32CurCounter1 > (_u32Counter1_2 + 0x200)) && (_u32Counter1_3 > (_u32Counter1_2 + 0x200)) )
        _u8Film22LikeCounter += 1;
    else if( (_u32Counter1_2 > (u32CurCounter1 + 0x200)) && (_u32Counter1_2 > (_u32Counter1_3 + 0x200)) )
        _u8Film22LikeCounter += 1;
    else if ( _u8Film22LikeCounter > 0 )
        _u8Film22LikeCounter --;

    if( u32CurCounter3 < 0x80 ) // for slow motion film22-like
    {
        if( (u32CurCounter1 > (_u32Counter1_2*2)) && (_u32Counter1_3 > (_u32Counter1_2*2)) )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi += 1;
        else if ( (_u32Counter1_2 > (u32CurCounter1*2)) && (_u32Counter1_2 > (_u32Counter1_3*2)) )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi += 1;
        else if ( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0 )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi --;
    }
    else if( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0 )
    {
        _u8Film22LikeCounter_ForSlowMotionDisableMCDi --;
    }

    if( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 10 )
        _u8Film22LikeCounter_ForSlowMotionDisableMCDi = 10;

    if( (u32CurCounter3 > 0x80) && (_u32Counter3_2 < 0x10) && (_u32Counter3_3 > 0x80) )
        _u8Film22LikeCounter2 += 1;
    else if( (u32CurCounter3 < 0x10) && (_u32Counter3_2 > 0x80) && (_u32Counter3_3 < 0x10) )
        _u8Film22LikeCounter2 += 1;
    else
        _u8Film22LikeCounter2 = 0;

    if( _u8Film22LikeCounter2 > 10 )
        _u8Film22LikeCounter2 = 10;

    if( _u8Film22LikeCounter > 10 ) // clamp
        _u8Film22LikeCounter = 10;


    if(u32CurCounter1<0xF)
        _u8FilmsmallCounter1 ++;
    else
        _u8FilmsmallCounter1 = 0;

    if(_u8FilmsmallCounter1>10)
        _u8FilmsmallCounter1 = 10;

    if( _u32Counter3_2 < _u32Counter3_3 )
        u32FixThofC3 = _u32Counter3_2*4;
    else
        u32FixThofC3 = _u32Counter3_3*4;

    bCounter1Have2xDiff = (((u32CurCounter1 < 0xf000) && (_u32Counter1_2 < 0xf000)) &&
                        ((u32CurCounter1 > (_u32Counter1_2*2)) || ((u32CurCounter1*2) < _u32Counter1_2)));

    if( u32FixThofC3 > 0x260)
        u32FixThofC3 = 0x260;
    else if( u32FixThofC3 < 0x80)
        u32FixThofC3 = 0x80;

    if((!bFilm) && (_u8Film22LikeCounter2 > 5) && (!bCounter1Have2xDiff) ) //when moving sub-pixel, counter3 will like film22, check counter1 don't have big difference
        u32FixThofC3 = 0x80;   // moving shibasogu buildingg

    MApi_XC_W2BYTE(REG_SC_BK0A_0D_L, (MS_U16)u32FixThofC3);
    MApi_XC_W2BYTE(REG_SC_BK0A_41_L, 0xFFFF);
    MApi_XC_W2BYTE(REG_SC_BK0A_5C_L, 0);
    MApi_XC_W2BYTE(REG_SC_BK0A_5E_L, 0);
    MApi_XC_W2BYTE(REG_SC_BK0A_20_L, 0x10);

    if( u32CurCounter1 > _u32Counter1_2 ) //max
        u32Film22AdaptiveCounter1Th = u32CurCounter1;
    else
        u32Film22AdaptiveCounter1Th = _u32Counter1_2;

    if(u32CurCounter3<5)
    {
        _u8SmallCounter3Cnt += 1;

        if(_u8SmallCounter3Cnt>10)
            _u8SmallCounter3Cnt = 10;
    }
    else
    {
        _u8SmallCounter3Cnt --  ;
    }

    if( (u32CurCounterA > (_u32CounterA_2 + 0x200/0x10)) && (_u32CounterA_3 > (_u32CounterA_2 + 0x200/0x10)))
        _u8Film22LikeCounterA += 1;
    else if(( _u32CounterA_2 > (u32CurCounterA + 0x200/0x10)) && (_u32CounterA_2 >( _u32CounterA_3 + 0x200/0x10)))
        _u8Film22LikeCounterA += 1;
    else if( _u8Film22LikeCounterA > 0 )
        _u8Film22LikeCounterA --;

    if(_u8Film22LikeCounterA>10)
        _u8Film22LikeCounterA = 10;

    if((u32CurCounterB > (_u32CounterB_2 + 0x200/0x10)) &&( _u32CounterB_3 > (_u32CounterB_2 + 0x200/0x10)))
        _u8Film22LikeCounterB += 1;
    else if(( _u32CounterB_2 > (u32CurCounterB + 0x200/0x10)) && (_u32CounterB_2 > (_u32CounterB_3 + 0x200/0x10)))
        _u8Film22LikeCounterB += 1;
    else if( _u8Film22LikeCounterB > 0 )
        _u8Film22LikeCounterB --;

    if(_u8Film22LikeCounterB>10)
        _u8Film22LikeCounterB = 10;


    if ((u32CurCounter3 > ((_u32Counter3_2+1) *10)) && (_u32Counter3_3 > ((_u32Counter3_2+1)*10)))
        _u8Film22LikeCounter3_Big += 1;
    else if((_u32Counter3_2 > ((u32CurCounter3+1)*10)) && (_u32Counter3_2 > ((_u32Counter3_3+1)*10)))
        _u8Film22LikeCounter3_Big += 1;
    else if(_u8Film22LikeCounter3_Big > 0 )
        _u8Film22LikeCounter3_Big --;

    if(_u8Film22LikeCounter3_Big>10)
        _u8Film22LikeCounter3_Big = 10;

    if( ((u32CurCounter3 > (_u32Counter3_2*4))||(u32CurCounter3 > (_u32Counter3_3*4)) ) && (u32CurCounter3>0x40) )
        _u8Film32LikeCounter3_Big += 1;
    else if( ( (_u32Counter3_2 > (_u32Counter3_3*4))||(_u32Counter3_2 > (u32CurCounter3*4)) ) && (_u32Counter3_2>0x40))
        _u8Film32LikeCounter3_Big += 1;
    else if( ( (_u32Counter3_3 > (u32CurCounter3*4))||(_u32Counter3_3 > (_u32Counter3_2*4)) ) && (_u32Counter3_3>0x40))
        _u8Film32LikeCounter3_Big += 1;
    else
        _u8Film32LikeCounter3_Big --;

    if(_u8Film32LikeCounter3_Big>10)
        _u8Film32LikeCounter3_Big = 10;

    if ( (u32CurCounter1 > ((_u32Counter1_2+1)*20)) && (_u32Counter1_3 > ((_u32Counter1_2+1)*20)))
        _u8Film22LikeCounter1_Big += 1;
    else if (( _u32Counter1_2 > ((u32CurCounter1+1)*20)) && (_u32Counter1_2 > ((_u32Counter1_3+1)*20)))
        _u8Film22LikeCounter1_Big += 1;
    else if ( _u8Film22LikeCounter1_Big > 0 )
        _u8Film22LikeCounter1_Big --;

    if(_u8Film22LikeCounter1_Big>10)
        _u8Film22LikeCounter1_Big = 10;


    //for KURO patch

    MS_U16 u16KuroPatchEn = 0;

    if( u32CurCounter3 >_u32Counter3_2+0x20 && u32CurCounter1 < _u32Counter1_2+0x20 && u32CurCounter1==0 )
        u16KuroPatchEn = 1;
    else if( u32CurCounter3 <_u32Counter3_2+0x20 && u32CurCounter1 > _u32Counter1_2+0x20 && _u32Counter1_2==0)
        u16KuroPatchEn = 1;


    if( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008 )
    {  //in film22
        if(((u32CurCounter3 > (_u32Counter3_2+1)*10)||(_u32Counter3_2 > (u32CurCounter3+1)*10)))
        {
            if(_u8Film22LikeCounter1_Big == 10)
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*3/8;
            }
            else
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*9/8;

                if( bIsHDSource )
                {
                    if( u32Film22AdaptiveCounter1Th < 0x400/0x2 *6)
                        u32Film22AdaptiveCounter1Th = 0x400/0x2 *6;
                }
                else
                {
                    if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                        u32Film22AdaptiveCounter1Th = 0x400/0x2;
                }
            }
        }
        else
        {
            if ( MApi_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)) ) // 2R
            {
                u32Film22AdaptiveCounter1Th = 0xFFFF;
            }
            else
            {
                if(_u8Film22LikeCounter1_Big == 10)
                     u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*3/8;
                else
                    u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;
            }

        }
    }
    else if( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010 )
    {  //in film32
        if((u32CurCounter3 > (_u32Counter3_2*10))||(_u32Counter3_2 > (u32CurCounter3*10))||(_u32Counter3_3 > (_u32Counter3_2*10)))
        {
            if( (u32CurCounter1>= _u32Counter1_2) && (u32CurCounter1 >= _u32Counter1_3)  )
                u32Film22AdaptiveCounter1Th = u32CurCounter1 * 2;
            else if((_u32Counter1_2>= u32CurCounter1) && (_u32Counter1_2 >= _u32Counter1_3))
                u32Film22AdaptiveCounter1Th = _u32Counter1_2 * 2;
            else if((_u32Counter1_3>= u32CurCounter1) &&( _u32Counter1_3 >= _u32Counter1_2))
                u32Film22AdaptiveCounter1Th = _u32Counter1_3 * 2;

            if( bIsHDSource )
            {
                if( u32Film22AdaptiveCounter1Th < (0x400/0x2*6) )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2 *6;
            }
            else
            {
                if( u32Film22AdaptiveCounter1Th < (0x400/0x2) )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;
            }
        }
        else
        {
            if ( MApi_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)) ) // 2R
            {
                u32Film22AdaptiveCounter1Th = 0xFFFF;
            }
            else
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;
           }
        }
    }
    else if( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040 )
    {  //in filmany
        u32Film22AdaptiveCounter1Th = 0x800;
    }
    else
    {
        if(_u8SmallCounter3Cnt==10)
        {
            if((_u8Film22LikeCounterA>5) || (_u8Film22LikeCounterB>5))
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;
                if( bIsHDSource )
                {
                    if( u32Film22AdaptiveCounter1Th < (0x400/0x2*6) )
                        u32Film22AdaptiveCounter1Th = 0x400/0x2*6;
                }
                else
                {
                    if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                        u32Film22AdaptiveCounter1Th = 0x400/0x2;
                }
            }
            else
            {
                if( bIsHDSource )
                {
                    u32Film22AdaptiveCounter1Th = 0x400/0x2*6;
                }
                else
                {
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;
                }
            }
        }
        else
        {
            u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;

            if( bIsHDSource )
            {
                if( u32Film22AdaptiveCounter1Th < 0x400/0x2*6 )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2*6;
            }
            else
            {
                if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;
            }
        }

        if(_u8Film22LikeCounter3_Big>8 || _u8Film32LikeCounter3_Big>8)
            u32Film22AdaptiveCounter1Th  = 0x7500;
    }


    if( bIsHDSource )
    {
        if( u32Film22AdaptiveCounter1Th < 0x400/0x4*6 )
            u32Film22AdaptiveCounter1Th = 0x400/0x4*6;
    }
    else
    {
        if( u32Film22AdaptiveCounter1Th < 0x800)
            u32Film22AdaptiveCounter1Th = 0x800;
    }

    _u32Counter3_3 = _u32Counter3_2;
    _u32Counter3_2  = u32CurCounter3;
    _u32Counter1_4 = _u32Counter1_3;
    _u32Counter1_3 = _u32Counter1_2;
    _u32Counter1_2  = u32CurCounter1;

    _u32CounterA_3 = _u32CounterA_2;
    _u32CounterA_2  = u32CurCounterA;
    _u32CounterB_3 = _u32CounterB_2;
    _u32CounterB_2  = u32CurCounterB;

    bIsFilmXOO = (_u8IsFilmXOOCounter != 0);

#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    u32Reg7C = MApi_XC_R2BYTE(REG_SC_BK2A_7C_L);
    u32Reg7D = MApi_XC_R2BYTE(REG_SC_BK2A_7D_L);
    u32Reg7E = MApi_XC_R2BYTE(REG_SC_BK2A_7E_L);

    // for all chip
    if( bIsHDSource )
    {
        // for MCNR only
        bVerticalMovingU = ((u32Reg7E > 0x0200) && (u32Reg7E > (8*u32Reg7C)) && (u32Reg7E > (8*u32Reg7D)));
    }
    else
    {
        // for MCNR only
        bVerticalMovingU = ((u32Reg7E > 0x0100) && (u32Reg7E > (8*u32Reg7C)) && (u32Reg7E > (8*u32Reg7D)));
    }
#else
    // for no MCNR
    bVerticalMovingU = false;
#endif

    u16Unmatch_3 = MApi_XC_R2BYTE(REG_SC_BK0A_0B_L);
    u16Unmatch_1 = MApi_XC_R2BYTE(REG_SC_BK0A_0A_L);

    // for sec , activate counter 1
    if( u16Unmatch_3 < 0x40 )
    {
        _u8NoCounter3_Cnt ++;
    }
    else
    {
        _u8NoCounter3_Cnt = 0;
    }

    // deactivate counter 1 jump out
    if( u16Unmatch_3 > 0x6000  )
    {
        _u8BigCounter3_Cnt = 10;
    }
    else if(_u8BigCounter3_Cnt > 0)
    {
        _u8BigCounter3_Cnt --;
    }

    if( u16Unmatch_3 > 0x1000  )
    {
        _u8BigCounter3_Cnt2 = 10;
    }
    else if(_u8BigCounter3_Cnt2 > 0)
    {
        _u8BigCounter3_Cnt2 --;
    }

    // deactivate counter 1 jump out
#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    if( u16Unmatch_1 > 0xF000  )
#else
    if( u16Unmatch_1 > 0xA000  )
#endif
    {
        _u8BigCounter1_Cnt = 10;
    }
    else if(_u8BigCounter1_Cnt > 0)
    {
        _u8BigCounter1_Cnt --;
    }

    if( u16Unmatch_1 > 0xc000)
    {
        if( _u8BigCounter_Cnt3 < 80)
            _u8BigCounter_Cnt3 ++;
    }
    else
        _u8BigCounter_Cnt3 = 0;

    if( u16Unmatch_1 > 0x1000 )
        _u8BigCounter1_Cnt2 = 0x8;
    else if( _u8BigCounter1_Cnt2 != 0)
        _u8BigCounter1_Cnt2 --;

    if( u16Unmatch_1 < 0x0800 )
    {
        _u8SmallCounter1_Cnt = 10;
    }
    else if(_u8SmallCounter1_Cnt > 0)
    {
        _u8SmallCounter1_Cnt --;
    }

    if((MApi_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2500) || (MApi_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2700) )//(stXCStatus.bInterlace)
    {
        if(_u8NoCounter3_Cnt > 20)
        {
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_02_L, ((MS_U16)0x01)<<8, 0x1F00);
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_05_L, ((MS_U16)0x10)<<8, 0xFF00);
        }
        else
        {
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_02_L, ((MS_U16)0x04)<<8, 0x1F00);  // for SD cross hatch
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_05_L, ((MS_U16)0x10)<<8, 0xFF00);
        }

        // Film32 On,f2 32 film mode enable
        if( (MApi_XC_R2BYTEMSK( REG_SC_BK0A_10_L, BIT(14) ) ==  BIT(14) )
           || (MApi_XC_R2BYTEMSK( REG_SC_BK0A_10_L, BIT(15) ) ==  BIT(15) ) )
        {
            if(!MDrv_GFLIP_FrameLockCheck()) // Make not frame LOCKED then turn off FilmMode
            {
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, BIT(1), BIT(1)); //hardware film mode DISABLE
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_21_L, BIT(1)|BIT(2), BIT(1) | BIT(2)); //software film mode DISABLE
            }
            else    // Make sure frame LOCKED then turn on FilmMode
            {
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, 0, BIT(1)); //hardware film mode ENABLE
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_21_L, 0, BIT(1) | BIT(2)); //software film mode ENABLE

                ///--> new add function for skip film mode in still pattern
                if((!MApi_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)))&&(!MApi_XC_R2BYTEMSK(REG_SC_BK0A_02_L, BIT(14))))    // 2R mode can't enter this loop
                {
                    u16Unmatch_3 = MApi_XC_R2BYTE(REG_SC_BK0A_0B_L);

                    if( (u16Unmatch_3 > 0x80) )
                        _u8Unmatch3Cnt = 0;
                    else if(_u8Unmatch3Cnt == 0xFF)
                        _u8Unmatch3Cnt = 0xFF;
                    else
                        _u8Unmatch3Cnt = _u8Unmatch3Cnt + 1;

                    ///--> Jump out Film for LG tower
                    if( !bVerticalMovingU )
                        _u8VerticalCnt = 0;
                    else if(_u8VerticalCnt == 0xFF)
                        _u8VerticalCnt = 0xFF;
                    else
                        _u8VerticalCnt = _u8VerticalCnt + 1;
                }
            }
        }
        else
        {
            // DEFAULT value of hardware and software film mode suggested by SY.Shih
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, 0, BIT(1));
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_21_L, 0, BIT(1) | BIT(2));
        }

        // film-any continuous error line patch, not easy to enter film, but easy to exit film
        if ( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040 ) // film-any status
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_57_L, 0x08, 0xff);
        else
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_57_L, 0x20, 0xff);

        if ( bIsHDSource && (!bFilm) )
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_15_L, 0x0008, 0x000F ); // for film32, also favor de-bouncing
        else
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_15_L, 0x0008, 0x000F ); // for De-bouncing

        if(_u8VerticalCnt>0x03)  // only active at Film
        {
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_23_L, 0, BIT(15));
        }
        else
        {
            if(u32CurCounter3<0x9 )
            {
                _u8StillCounter3++;
            }
            else
            {
                _u8StillCounter3 = 0;
            }

            if(_u8StillCounter3>=8)
            {
                _u8StillCounter3 = 8;
            }

            if(u32Film22AdaptiveCounter1Th>0xFFFF)
                u32Film22AdaptiveCounter1Th = 0xFFFF;

            MApi_XC_W2BYTE(REG_SC_BK0A_0C_L, (MS_U16)u32Film22AdaptiveCounter1Th );


            if(_u8StillCounter3==8 || u16KuroPatchEn==1 || _u8FilmsmallCounter1==10) //default
            {
                if ( MApi_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)) )  // 2R
                {

                }
                else //4R
                {
                    MApi_XC_W2BYTE(REG_SC_BK0A_0C_L, 0 );
                    MApi_XC_W2BYTE(REG_SC_BK0A_0D_L, 0 );
                }
            }

            if( bIs60Hz ) //NTSC -> turn off fast film22
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_23_L, 0, BIT(15));
            else // PAL
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_23_L, BIT(15), BIT(15));
        }

        // for SEC pattern gen
        if(_u8NoCounter3_Cnt > 20)
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x00FF, 0xFFFF); // disable film-any
        else if ( !bIsFilmXOO )
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0080, 0xFFFF); // disable film-any
        else
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0810, 0xFFFF);

        // for auto 1
        // film32
        u32Counter3  = MApi_XC_R2BYTE(REG_SC_BK0A_0B_L);


        if( u32Counter3 < 0x100 )
            _u8MovingCharacterCounter = 0; //
        else if(_u8MovingCharacterCounter != 0xff )
            _u8MovingCharacterCounter ++;

        if ( MApi_XC_R2BYTEMSK(REG_SC_BK22_7C_L, BIT(12)) ) // auto 1 , deal with video-over-film
        {
        }
        else // default
        {
            if ( bIsHDSource )
            {
                MApi_XC_W2BYTE(REG_SC_BK0A_06_L, 0xCF40);
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_17_L, BIT(13),BIT(13) );
                //MApi_XC_W2BYTE(REG_SC_BK0A_1F_L, 0x0fff);
            }
            else
            {
                MApi_XC_W2BYTE(REG_SC_BK0A_06_L, 0xA810);
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_17_L, 0x00 ,BIT(13) );
                MApi_XC_W2BYTE(REG_SC_BK0A_1F_L, 0x0250);
            }

            MApi_XC_W2BYTE(REG_SC_BK0A_16_L, 0x00D8);
            MApi_XC_W2BYTE(REG_SC_BK0A_1E_L, 0x0551);
        }

        if ( MApi_XC_R2BYTEMSK(REG_SC_BK22_7C_L, BIT(12)) ) // auto 1 , deal with video-over-film
        {
        }
        else // default
        {
            if ( bIsHDSource )
            {
                MApi_XC_W2BYTE(REG_SC_BK0A_07_L, 0x7F10);

            }
            else
            {
                MApi_XC_W2BYTE(REG_SC_BK0A_07_L, 0x7F08);
            }
        }
    } // end interlace loop
    _u16RdBank_Prv = u16RdBank;

    u16InvStillFrameCnt = 15;

    if((_u8Film22LikeCounter > 0x5) || (_u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0x5))
    {
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_2F_L, 0x8000, 0xFFFF); //disable MCDi
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_29_L, 0xF000, 0xFF00); //Favor SST
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_2A_L, 0x0000, 0xFF00); //Disable KFC-cplx
    }
    else
    {
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_2F_L, 0x0000, 0xFFFF);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_29_L, 0x2100, 0xFF00);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_2A_L, ((MS_U16)0xF000)|((MS_U16)(u16InvStillFrameCnt<<8)), 0xFF00);
        MApi_XC_W2BYTEMSK(REG_SC_BK22_2A_L, (MS_U16)(u16InvStillFrameCnt<<4), 0x00F0);
    }
}

void MDrv_GFLIP_FilmDriverHWVer2(void)
{
    MS_BOOL bIsFilmXOO = 0;
    MS_U32 u32Film22AdaptiveCounter1Th = 0;
    static MS_U16 _u16RdBank_Prv=0;
    MS_U16 u16RdBank;
    MS_U32 u32CurCounter1=0, u32CurCounter3=0, u32FixThofC3=0;
    static MS_U32 _u32Counter3_2=0, _u32Counter3_3=0;
    static MS_U32 _u32Counter1_2=0, _u32Counter1_3=0, _u32Counter1_4=0;
    static MS_U32 _u32CounterA_2=0, _u32CounterA_3=0;
    static MS_U32 _u32CounterB_2=0, _u32CounterB_3=0;
    static MS_U8 _u8Film22LikeCounter, _u8Film22LikeCounter2;
    static MS_U8 _u8Film22LikeCounter_ForSlowMotionDisableMCDi;
    static MS_U8 _u8SceneChangeFrame;
    static MS_U8 _u8IsFilmXOOCounter=0;
    static MS_BOOL _bIs2ndFilmXOOCounter;
    MS_BOOL bCounter1Have2xDiff, bFilm, bFilmAny;
    MS_U32 u32CurCounterA, u32CurCounterB;
    static MS_U8 _u8SmallCounter3Cnt;
    static MS_U8 _u8Film22LikeCounterA, _u8Film22LikeCounterB;
    static MS_U8 _u8Film22LikeCounter3_Big;
    static MS_U8 _u8Film32LikeCounter3_Big;
    static MS_U8 _u8Film22LikeCounter1_Big;
    static MS_U8 _u8VerticalCnt, _u8Unmatch3Cnt;
    MS_BOOL bIsHDSource;
    MS_U16 u16Unmatch_3, u16Unmatch_1;
    static MS_U8 _u8NoCounter3_Cnt, _u8BigCounter3_Cnt, _u8BigCounter1_Cnt, _u8BigCounter_Cnt3;
    static MS_U8 _u8BigCounter1_Cnt2, _u8SmallCounter1_Cnt, _u8BigCounter3_Cnt2;
    MS_BOOL bVerticalMovingU ;
    MS_BOOL bIs60Hz = ( MApi_XC_R2BYTE(REG_SC_BK0A_10_L) & 0x8000 ) != 0; // checkfilm32 enable for 60Hz source
    static MS_U8 _u8StillCounter3;
    MS_U32 u32Counter3;
    static MS_U8 _u8MovingCharacterCounter;
#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    MS_U32 u32Reg7C, u32Reg7D, u32Reg7E;
#endif

    u16RdBank = ( MApi_XC_R2BYTE(REG_SC_BK12_3A_L) & 0xF0 ) >> 4;
    bIsHDSource = ( MApi_XC_R2BYTEMSK(REG_SC_BK12_0F_L, 0xFFFF) > 0x700 );

    u32CurCounterA = MApi_XC_R2BYTE(REG_SC_BK0A_4A_L);
    u32CurCounterB = MApi_XC_R2BYTE(REG_SC_BK0A_4B_L);

    u32CurCounter1 = MApi_XC_R2BYTE(REG_SC_BK0A_0A_L);
    u32CurCounter3 = MApi_XC_R2BYTE(REG_SC_BK0A_0B_L);

    if(u16RdBank == _u16RdBank_Prv)  // Not change read bank
    {
        return ;
    }

    bFilmAny = ( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040);
    bFilm = (( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008) ||  // film22
            ( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010) ||  // film32
            ( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040) );   // film any

    // check XOO
    if( ( (!bFilm) && (u32CurCounter3 > 0x200) && (_u32Counter3_2< 0x80) && (_u32Counter3_3< 0x80) ) || //not film
        ( (!bFilm) && (u32CurCounter3 > 0x80) && (_u32Counter3_2< 0x20) && (_u32Counter3_3< 0x20) ) ) //not film
    {
        if ( _u8IsFilmXOOCounter > 0 )
        {
            _bIs2ndFilmXOOCounter = true;
        }

        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if( bFilm && (!bFilmAny) && (u32CurCounter3 > 0x200) && (u32CurCounter3 > (_u32Counter3_2*2)) && (u32CurCounter3 > (_u32Counter3_3*2)) ) //32 or 22
    {
        if( _u8IsFilmXOOCounter > 0 )
        {
            _bIs2ndFilmXOOCounter = true;
        }
        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if(  bFilm && (u32CurCounter3 > 0x80)  && (u32CurCounter3 > (_u32Counter3_2*2)) && (u32CurCounter3 > (_u32Counter3_3*2)) ) // film-any
    {
        _bIs2ndFilmXOOCounter = true;
        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if( _u8IsFilmXOOCounter != 0 )
    {
        _u8IsFilmXOOCounter --;
    }
    else
    {
        _bIs2ndFilmXOOCounter = false;
    }

    //detect scene change into film...
    if( (u32CurCounter1 > (_u32Counter1_4*4)) && (u32CurCounter1 > (_u32Counter1_2*4)) && (u32CurCounter1 > (_u32Counter1_3*4)) )
        _u8SceneChangeFrame = 60;
    else if( _u8SceneChangeFrame != 0 )
        _u8SceneChangeFrame --;

    if( (u32CurCounter3 > 0x80) && (_u32Counter3_2 < 0x10) && (_u32Counter3_3 > 0x80) )
        _u8Film22LikeCounter += 2;
    else if( (u32CurCounter3 < 0x10) && (_u32Counter3_2 > 0x80) && (_u32Counter3_3 < 0x10) )
        _u8Film22LikeCounter += 2;
    else if( (u32CurCounter1 > (_u32Counter1_2 + 0x20)) && (_u32Counter1_3 > (_u32Counter1_2 + 0x20)) )
        _u8Film22LikeCounter += 1;
    else if( (_u32Counter1_2 > (u32CurCounter1 + 0x20)) && (_u32Counter1_2 > (_u32Counter1_3 + 0x20)) )
        _u8Film22LikeCounter += 1;
    else if ( _u8Film22LikeCounter > 0 )
        _u8Film22LikeCounter --;

    if( u32CurCounter3 < 0x80 ) // for slow motion film22-like
    {
        if( (u32CurCounter1 > (_u32Counter1_2*2)) && (_u32Counter1_3 > (_u32Counter1_2*2)) )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi += 1;
        else if ( (_u32Counter1_2 > (u32CurCounter1*2)) && (_u32Counter1_2 > (_u32Counter1_3*2)) )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi += 1;
        else if ( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0 )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi --;
    }
    else if( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0 )
    {
        _u8Film22LikeCounter_ForSlowMotionDisableMCDi --;
    }

    if( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 10 )
        _u8Film22LikeCounter_ForSlowMotionDisableMCDi = 10;

    if( (u32CurCounter3 > 0x80) && (_u32Counter3_2 < 0x10) && (_u32Counter3_3 > 0x80) )
        _u8Film22LikeCounter2 += 1;
    else if( (u32CurCounter3 < 0x10) && (_u32Counter3_2 > 0x80) && (_u32Counter3_3 < 0x10) )
        _u8Film22LikeCounter2 += 1;
    else
        _u8Film22LikeCounter2 = 0;

    if( _u8Film22LikeCounter2 > 10 )
        _u8Film22LikeCounter2 = 10;

    if( _u8Film22LikeCounter > 10 ) // clamp
        _u8Film22LikeCounter = 10;

    if( _u32Counter3_2 < _u32Counter3_3 )
        u32FixThofC3 = _u32Counter3_2*4;
    else
        u32FixThofC3 = _u32Counter3_3*4;


    bCounter1Have2xDiff = (((u32CurCounter1 < 0xf00) && (_u32Counter1_2 < 0xf00)) &&
                        ((u32CurCounter1 > (_u32Counter1_2*2)) || ((u32CurCounter1*2) < _u32Counter1_2)));

    if( u32FixThofC3 > 0x260)
        u32FixThofC3 = 0x260;
    else if( u32FixThofC3 < 0x80)
        u32FixThofC3 = 0x80;

    if((!bFilm) && (_u8Film22LikeCounter2 > 5) && (!bCounter1Have2xDiff) ) //when moving sub-pixel, counter3 will like film22, check counter1 don't have big difference
        u32FixThofC3 = 0x80;   // moving shibasogu buildingg

    MApi_XC_W2BYTE(REG_SC_BK0A_0D_L, (MS_U16)u32FixThofC3);
    MApi_XC_W2BYTE(REG_SC_BK0A_41_L, 0xFFFF);
    MApi_XC_W2BYTE(REG_SC_BK0A_5C_L, 0);
    MApi_XC_W2BYTE(REG_SC_BK0A_5E_L, 0);
    MApi_XC_W2BYTEMSK(REG_SC_BK0A_02_L, 0, 0x0F );
    MApi_XC_W2BYTE(REG_SC_BK0A_20_L, 0x10);

    if( u32CurCounter1 > _u32Counter1_2 ) //max
        u32Film22AdaptiveCounter1Th = u32CurCounter1;
    else
        u32Film22AdaptiveCounter1Th = _u32Counter1_2;

    if(u32CurCounter3<5)
    {
        _u8SmallCounter3Cnt += 1;

        if(_u8SmallCounter3Cnt>10)
            _u8SmallCounter3Cnt = 10;
    }
    else
    {
        _u8SmallCounter3Cnt --  ;
    }

    if( (u32CurCounterA > (_u32CounterA_2 + 0x200/0x10)) && (_u32CounterA_3 > (_u32CounterA_2 + 0x200/0x10)))
        _u8Film22LikeCounterA += 1;
    else if(( _u32CounterA_2 > (u32CurCounterA + 0x200/0x10)) && (_u32CounterA_2 >( _u32CounterA_3 + 0x200/0x10)))
        _u8Film22LikeCounterA += 1;
    else if( _u8Film22LikeCounterA > 0 )
        _u8Film22LikeCounterA --;

    if(_u8Film22LikeCounterA>10)
        _u8Film22LikeCounterA = 10;

    if((u32CurCounterB > (_u32CounterB_2 + 0x200/0x10)) &&( _u32CounterB_3 > (_u32CounterB_2 + 0x200/0x10)))
        _u8Film22LikeCounterB += 1;
    else if(( _u32CounterB_2 > (u32CurCounterB + 0x200/0x10)) && (_u32CounterB_2 > (_u32CounterB_3 + 0x200/0x10)))
        _u8Film22LikeCounterB += 1;
    else if( _u8Film22LikeCounterB > 0 )
        _u8Film22LikeCounterB --;

    if(_u8Film22LikeCounterB>10)
        _u8Film22LikeCounterB = 10;


    if ((u32CurCounter3 > ((_u32Counter3_2+1) *10)) && (_u32Counter3_3 > ((_u32Counter3_2+1)*10)))
        _u8Film22LikeCounter3_Big += 1;
    else if((_u32Counter3_2 > ((u32CurCounter3+1)*10)) && (_u32Counter3_2 > ((_u32Counter3_3+1)*10)))
        _u8Film22LikeCounter3_Big += 1;
    else if(_u8Film22LikeCounter3_Big > 0 )
        _u8Film22LikeCounter3_Big --;

    if(_u8Film22LikeCounter3_Big>10)
        _u8Film22LikeCounter3_Big = 10;

    if( ((u32CurCounter3 > (_u32Counter3_2*4))||(u32CurCounter3 > (_u32Counter3_3*4)) ) && (u32CurCounter3>0x40) )
        _u8Film32LikeCounter3_Big += 1;
    else if( ( (_u32Counter3_2 > (_u32Counter3_3*4))||(_u32Counter3_2 > (u32CurCounter3*4)) ) && (_u32Counter3_2>0x40))
        _u8Film32LikeCounter3_Big += 1;
    else if( ( (_u32Counter3_3 > (u32CurCounter3*4))||(_u32Counter3_3 > (_u32Counter3_2*4)) ) && (_u32Counter3_3>0x40))
        _u8Film32LikeCounter3_Big += 1;
    else
        _u8Film32LikeCounter3_Big --;

    if(_u8Film32LikeCounter3_Big>10)
        _u8Film32LikeCounter3_Big = 10;

    if ( (u32CurCounter1 > ((_u32Counter1_2+1)*20)) && (_u32Counter1_3 > ((_u32Counter1_2+1)*20)))
        _u8Film22LikeCounter1_Big += 1;
    else if (( _u32Counter1_2 > ((u32CurCounter1+1)*20)) && (_u32Counter1_2 > ((_u32Counter1_3+1)*20)))
        _u8Film22LikeCounter1_Big += 1;
    else if ( _u8Film22LikeCounter1_Big > 0 )
        _u8Film22LikeCounter1_Big --;

    if(_u8Film22LikeCounter1_Big>10)
        _u8Film22LikeCounter1_Big = 10;


    //for KURO patch
    MS_U16 u16KuroPatchEn = 0;
    {
        if( u32CurCounter3 >_u32Counter3_2+0x20 && u32CurCounter1 < _u32Counter1_2+0x20 && u32CurCounter1==0 )
            u16KuroPatchEn = 1;
        else if( u32CurCounter3 <_u32Counter3_2+0x20 && u32CurCounter1 > _u32Counter1_2+0x20 && _u32Counter1_2==0)
            u16KuroPatchEn = 1;
    }


    if( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008 )
    {  //in film22
        if(((u32CurCounter3 > (_u32Counter3_2+1)*10)||(_u32Counter3_2 > (u32CurCounter3+1)*10)))
        {
            if(_u8Film22LikeCounter1_Big == 10)
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*3/8;

            else
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*9/8;

                if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;
            }
        }
        else
        {
            if(_u8Film22LikeCounter1_Big == 10)
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*3/8;
            else
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;

        }
    }
    else if( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010 )
    {  //in film32
        if((u32CurCounter3 > (_u32Counter3_2*10))||(_u32Counter3_2 > (u32CurCounter3*10))||(_u32Counter3_3 > (_u32Counter3_2*10)))
        {
            if( (u32CurCounter1>= _u32Counter1_2) && (u32CurCounter1 >= _u32Counter1_3)  )
                u32Film22AdaptiveCounter1Th = u32CurCounter1 * 2;
            else if((_u32Counter1_2>= u32CurCounter1) && (_u32Counter1_2 >= _u32Counter1_3))
                u32Film22AdaptiveCounter1Th = _u32Counter1_2 * 2;
            else if((_u32Counter1_3>= u32CurCounter1) &&( _u32Counter1_3 >= _u32Counter1_2))
                u32Film22AdaptiveCounter1Th = _u32Counter1_3 * 2;

            if( u32Film22AdaptiveCounter1Th < (0x400/0x2) )
                u32Film22AdaptiveCounter1Th = 0x400/0x2;
        }
        else
        {
            u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;
        }
    }
    else if( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040 )
    {  //in filmany
        u32Film22AdaptiveCounter1Th = 0x5F00;
    }
    else
    {
        if(_u8SmallCounter3Cnt==10)
        {
            if((_u8Film22LikeCounterA>5) || (_u8Film22LikeCounterB>5))
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;

                if( u32Film22AdaptiveCounter1Th > 0x400/0x2 )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;

            }
            else
            {
                u32Film22AdaptiveCounter1Th = 0x400/0x2;
            }
        }
        else
        {
            u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;

            if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                u32Film22AdaptiveCounter1Th = 0x400/0x2;
        }

        if(_u8Film22LikeCounter3_Big>8 || _u8Film32LikeCounter3_Big>8)
            u32Film22AdaptiveCounter1Th  = 0xFFFF;
    }


    if( u32Film22AdaptiveCounter1Th < 0x400/0x4)
        u32Film22AdaptiveCounter1Th = 0x400/0x4;

    _u32Counter3_3 = _u32Counter3_2;
    _u32Counter3_2  = u32CurCounter3;
    _u32Counter1_4 = _u32Counter1_3;
    _u32Counter1_3 = _u32Counter1_2;
    _u32Counter1_2  = u32CurCounter1;

    _u32CounterA_3 = _u32CounterA_2;
    _u32CounterA_2  = u32CurCounterA;
    _u32CounterB_3 = _u32CounterB_2;
    _u32CounterB_2  = u32CurCounterB;

    bIsFilmXOO = (_u8IsFilmXOOCounter != 0);

#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    u32Reg7C = MApi_XC_R2BYTE(REG_SC_BK2A_7C_L);
    u32Reg7D = MApi_XC_R2BYTE(REG_SC_BK2A_7D_L);
    u32Reg7E = MApi_XC_R2BYTE(REG_SC_BK2A_7E_L);

    // for all chip
    if( bIsHDSource )
    {
        // for MCNR only
        bVerticalMovingU = ((u32Reg7E > 0x0200) && (u32Reg7E > (8*u32Reg7C)) && (u32Reg7E > (8*u32Reg7D)));
    }
    else
    {
        // for MCNR only
        bVerticalMovingU = ((u32Reg7E > 0x0100) && (u32Reg7E > (8*u32Reg7C)) && (u32Reg7E > (8*u32Reg7D)));
    }
#else
    // for no MCNR
    bVerticalMovingU = false;
#endif

    u16Unmatch_3 = MApi_XC_R2BYTE(REG_SC_BK0A_0B_L);
    u16Unmatch_1 = MApi_XC_R2BYTE(REG_SC_BK0A_0A_L);

    // for sec , activate counter 1
    if( u16Unmatch_3 < 0x40 )
    {
        _u8NoCounter3_Cnt ++;
    }
    else
    {
        _u8NoCounter3_Cnt = 0;
    }

    // deactivate counter 1 jump out
    if( u16Unmatch_3 > 0x6000  )
    {
        _u8BigCounter3_Cnt = 10;
    }
    else if(_u8BigCounter3_Cnt > 0)
    {
        _u8BigCounter3_Cnt --;
    }

    if( u16Unmatch_3 > 0x1000  )
    {
        _u8BigCounter3_Cnt2 = 10;
    }
    else if(_u8BigCounter3_Cnt2 > 0)
    {
        _u8BigCounter3_Cnt2 --;
    }

    // deactivate counter 1 jump out
#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    if( u16Unmatch_1 > 0xF000  )
#else
    if( u16Unmatch_1 > 0xA000  )
#endif
    {
        _u8BigCounter1_Cnt = 10;
    }
    else if(_u8BigCounter1_Cnt > 0)
    {
        _u8BigCounter1_Cnt --;
    }

    if( u16Unmatch_1 > 0xc000)
    {
        if( _u8BigCounter_Cnt3 < 80)
            _u8BigCounter_Cnt3 ++;
    }
    else
        _u8BigCounter_Cnt3 = 0;

    if( u16Unmatch_1 > 0x1000 )
        _u8BigCounter1_Cnt2 = 0x8;
    else if( _u8BigCounter1_Cnt2 != 0)
        _u8BigCounter1_Cnt2 --;

    if( u16Unmatch_1 < 0x0800 )
    {
        _u8SmallCounter1_Cnt = 10;
    }
    else if(_u8SmallCounter1_Cnt > 0)
    {
        _u8SmallCounter1_Cnt --;
    }

    if((MApi_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2500) || (MApi_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2700) )//(stXCStatus.bInterlace)
    {
        if(_u8NoCounter3_Cnt > 20)
        {
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_02_L, ((MS_U16)0x01)<<8, 0x1F00);
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_05_L, ((MS_U16)0x10)<<8, 0xFF00);
        }
        else
        {
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_02_L, ((MS_U16)0x04)<<8, 0x1F00);  // for SD cross hatch
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_05_L, ((MS_U16)0x10)<<8, 0xFF00);
        }

        // Film32 On,f2 32 film mode enable
        if( (MApi_XC_R2BYTEMSK( REG_SC_BK0A_10_L, BIT(14) ) ==  BIT(14) )
           || (MApi_XC_R2BYTEMSK( REG_SC_BK0A_10_L, BIT(15) ) ==  BIT(15) ) )
        {
            if(!MDrv_GFLIP_FrameLockCheck()) // Make not frame LOCKED then turn off FilmMode
            {
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, BIT(1), BIT(1)); //hardware film mode DISABLE
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_21_L, BIT(1)|BIT(2), BIT(1) | BIT(2)); //software film mode DISABLE
            }
            else                                   // Make sure frame LOCKED then turn on FilmMode
            {
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, 0, BIT(1)); //hardware film mode ENABLE
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_21_L, 0, BIT(1) | BIT(2)); //software film mode ENABLE

                ///--> new add function for skip film mode in still pattern
                if((!MApi_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)))&&(!MApi_XC_R2BYTEMSK(REG_SC_BK0A_02_L, BIT(14))))    // 2R mode can't enter this loop
                {
                    u16Unmatch_3 = MApi_XC_R2BYTE(REG_SC_BK0A_0B_L);

                    if( (u16Unmatch_3 > 0x80) )
                        _u8Unmatch3Cnt = 0;
                    else if(_u8Unmatch3Cnt == 0xFF)
                        _u8Unmatch3Cnt = 0xFF;
                    else
                        _u8Unmatch3Cnt = _u8Unmatch3Cnt + 1;

                    ///--> Jump out Film for LG tower
                    if( !bVerticalMovingU )
                        _u8VerticalCnt = 0;
                    else if(_u8VerticalCnt == 0xFF)
                        _u8VerticalCnt = 0xFF;
                    else
                        _u8VerticalCnt = _u8VerticalCnt + 1;
                }
            }
        }
        else
        {
            // DEFAULT value of hardware and software film mode suggested by SY.Shih
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, 0, BIT(1));
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_21_L, 0, BIT(1) | BIT(2));
        }

        // film-any continuous error line patch, not easy to enter film, but easy to exit film
        if ( (MS_U16)MApi_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040 ) // film-any status
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_57_L, 0x08, 0xff);
        else
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_57_L, 0x20, 0xff);

        if ( bIsHDSource && (!bFilm) )
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_15_L, 0x0008, 0x000F ); // for film32, also favor de-bouncing
        else
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_15_L, 0x0008, 0x000F ); // for De-bouncing

        if(_u8VerticalCnt>0x03)          // only active at Film
        {
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_23_L, 0, BIT(15));
        }
        else
        {
            if((u32CurCounter3<0x9) && (u32CurCounter1<0x9))
            {
                _u8StillCounter3++;
            }
            else
            {
                _u8StillCounter3 = 0;
            }

            if(_u8StillCounter3>=8)
            {
                _u8StillCounter3 = 8;
            }

            if(u32Film22AdaptiveCounter1Th>0xFFFF)
                u32Film22AdaptiveCounter1Th = 0xFFFF;

            MApi_XC_W2BYTE(REG_SC_BK0A_0C_L, (MS_U16)u32Film22AdaptiveCounter1Th );

            if(_u8StillCounter3==8 || u16KuroPatchEn==1) //default
            {
                MApi_XC_W2BYTE(REG_SC_BK0A_0C_L, 0 );
                MApi_XC_W2BYTE(REG_SC_BK0A_0D_L, 0 );
            }

            if( bIs60Hz ) //NTSC -> turn off fast film22
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_23_L, 0, BIT(15));
            else // PAL
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_23_L, BIT(15), BIT(15));
        }

        // for SEC pattern gen
        if(_u8NoCounter3_Cnt > 20)
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x00FF, 0xFFFF); // disable film-any
        else if ( !bIsFilmXOO )
            MApi_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0080, 0xFFFF); // disable film-any
        else
        {
            if (bIsHDSource)
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0804, 0xFFFF);
            else
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0808, 0xFFFF);
        }

        // for auto 1
        // film32
        u32Counter3  = MApi_XC_R2BYTE(REG_SC_BK0A_0B_L);


        if( u32Counter3 < 0x100 )
            _u8MovingCharacterCounter = 0; //
        else if(_u8MovingCharacterCounter != 0xff )
            _u8MovingCharacterCounter ++;

        if ( MApi_XC_R2BYTEMSK(REG_SC_BK22_7C_L, BIT(12)) ) // auto 1 , deal with video-over-film
        {

        }
        else // default
        {
            if ( bIsHDSource )
            {
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_17_L, BIT(13),BIT(13) );
            }
            else
            {
                MApi_XC_W2BYTEMSK(REG_SC_BK0A_17_L, 0x00 ,BIT(13) );
                MApi_XC_W2BYTE(REG_SC_BK0A_1F_L, 0x0250);
            }

            MApi_XC_W2BYTE(REG_SC_BK0A_16_L, 0x00D8);
            MApi_XC_W2BYTE(REG_SC_BK0A_1E_L, 0x0551);
        }

    } // end interlace loop
    _u16RdBank_Prv = u16RdBank;
}


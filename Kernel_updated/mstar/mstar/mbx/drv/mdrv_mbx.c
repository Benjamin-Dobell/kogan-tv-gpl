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
/// file    mdrv_mbx.c
/// @brief  MStar MailBox DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _DRV_MBX_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/module.h>

#include "mdrv_mstypes.h"
#include "mdrv_mbx.h"
#include "mhal_mbx.h"
#include "mhal_mbx_interrupt.h"
#include "mdrv_mbx_msgpool.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Local Defines
//=============================================================================
#define MBXCLASS_IN_SIGNAL_MASK	 0x0000FF00
#define MBXCLASS_IN_SIGNAL_SHIFT	 0x8

#define MBX_ASYNCNOTIFIER_MAX  E_MBX_CLASS_MAX

//=============================================================================
// Debug Macros
//=============================================================================
#define MBX_DEBUG
#ifdef MBX_DEBUG
    #define MBX_PRINT(fmt, args...)      printk("[MailBox (Driver)][%05d] " fmt, __LINE__, ## args)
    #define MBX_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        MBX_PRINT(_fmt, ##_args);  \
                                        while(1);               \
                                    }
#else
    #define MBX_PRINT(_fmt, _args...)
    #define MBX_ASSERT(_cnd, _fmt, _args...)
#endif

//=============================================================================
// Macros
//=============================================================================
//#define DRV_MBX_LockAsync_Init()     spin_lock_init(&_spinLockAsyncMBX)
//#define DRV_MBX_LockAsync()   spin_lock(&_spinLockAsyncMBX)
//#define DRV_MBX_UnLockAsync()   spin_unlock(&_spinLockAsyncMBX)

#define DRV_MBX_LockSend_Init()     spin_lock_init(&_spinLockSendMBX)
#define DRV_MBX_LockSend()   spin_lock(&_spinLockSendMBX)
#define DRV_MBX_UnLockSend()   spin_unlock(&_spinLockSendMBX)

#define DRV_MBX_LockRecv_Init()     spin_lock_init(&_spinLockRecvMBX)
#define DRV_MBX_LockRecv()   spin_lock_bh(&_spinLockRecvMBX)
#define DRV_MBX_UnLockRecv()   spin_unlock_bh(&_spinLockRecvMBX)

//=============================================================================
// Local Variables
//=============================================================================
//static spinlock_t _spinLockAsyncMBX;
static spinlock_t _spinLockSendMBX;
static spinlock_t _spinLockRecvMBX;

static MBX_ASYNC_NOTIFIER _mbxAsyncNotifierMBX[MBX_ASYNCNOTIFIER_MAX];

static MBX_ROLE_ID _eMbxHostRole = E_MBX_ROLE_MAX;
static MBX_ROLE_ID _eMbxCpu2Role[E_MBX_CPU_MAX];
static MBX_CPU_ID _eMbxRole2Cpu[E_MBX_ROLE_MAX];
static MS_U32 _u32TimeoutMillSecs = 0;

static MS_BOOL _bCalledFromDriver = FALSE;
//=============================================================================
// Global Variables
//=============================================================================

//=============================================================================
// Local Function Prototypes
//=============================================================================

//=============================================================================
//Async. Notifier functions:
static MS_S16 _MDrv_MBX_AllocateAsyncNotifier(MS_U32 u32AsyncID);
//static MS_S16 _MDrv_MBX_FreeAsyncNotifier(MS_U32 u32AsyncID);
static void _MDrv_MBX_FreeAsyncNotifierByID(MS_U16 u16NotifierID);
static MS_S16 _MDrv_MBX_GetAsyncNotifierIDByAsyncID(MS_U32 u32AsyncID);

//=============================================================================
//MBX Config functions:
static MBX_Result _MDrv_MBX_InitConfig(MBX_CPU_ID eHKCPU, MBX_ROLE_ID eHostRole, MS_U32 u32TimeoutMillSecs);
static MBX_Result  _MDrv_MBX_SetConfig(MBX_CPU_ID eHKCPU, MBX_ROLE_ID eHostRole, MS_U32 u32TimeoutMillSecs);

//Util Function
static MS_U32 _MDrv_MBX_GetSystemTime(void); //in ms

//Message Handle functions:
static MBX_Result  _MDrv_MBX_SendMsg(MBX_Msg *pMsg, MBX_ROLE_ID eSrcRole);
static MBX_Result  _MDrv_MBX_RecvMsg(MS_S16 mbxAsyncNotifierID, MBX_Class eTargetClass, MBX_Msg *pMsg, MS_U32 u32Flag);
static MBX_Result  _MDrv_MBX_CheckMsg(MS_S16 mbxAsyncNotifierID, MBX_Class eTargetClass, MBX_Msg *pMsg, MS_U32 u32Flag);

//=============================================================================
//Message interrupt callback function:
static void _MDrv_MBX_MsgRecvCb(MS_S32 s32Irq);
#ifdef CONFIG_MSTAR_PM_SWIR
void (*MDrv_MBX_MsgCbFunc)(void);
#endif

//=============================================================================
// Local Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// Allocate one Async Notifier
/// @param  u32AsyncID                  \b IN: The Async ID
/// @return MS_S16:INVALID_PTR: no more async notifier could be allocated
/// @return MS_S16:Async Notifer ID: between 0-MBX_ASYNCNOTIFIER_MAX
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MS_S16 _MDrv_MBX_AllocateAsyncNotifier(MS_U32 u32AsyncID)
{
    MS_S16 s16Idx;

    for(s16Idx = 0; s16Idx<MBX_ASYNCNOTIFIER_MAX; s16Idx++)
    {
        if(_mbxAsyncNotifierMBX[s16Idx].u16Usage == FALSE)
        {
            _mbxAsyncNotifierMBX[s16Idx].u32AsyncID = u32AsyncID;
            _mbxAsyncNotifierMBX[s16Idx].u16Usage = TRUE;
            _mbxAsyncNotifierMBX[s16Idx].s16MsgQFirst = INVALID_PTR;
            _mbxAsyncNotifierMBX[s16Idx].bEnable = FALSE; //default disable.
            break;
        }
    }

    if(s16Idx>=MBX_ASYNCNOTIFIER_MAX)
    {
        return INVALID_PTR;
    }

    return s16Idx;
}

#if 0
//-------------------------------------------------------------------------------------------------
/// Free one Async Notifier
/// @param  u32AsyncID                  \b IN: The Async ID
/// @return MS_S16:INVALID_PTR: no such an async id has been allocated, free fail
/// @return MS_S16:Async Notifer ID: the freeed notifier id by async id
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MS_S16 _MDrv_MBX_FreeAsyncNotifier(MS_U32 u32AsyncID)
{
    MS_S16 s16Idx;

    DRV_MBX_LockAsync();

    for(s16Idx = 0; s16Idx<MBX_ASYNCNOTIFIER_MAX; s16Idx++)
    {
        if(_mbxAsyncNotifierMBX[s16Idx].u32AsyncID == u32AsyncID)
        {
            _mbxAsyncNotifierMBX[s16Idx].u32AsyncID = NULL;
            _mbxAsyncNotifierMBX[s16Idx].u16Usage = FALSE;
            _mbxAsyncNotifierMBX[s16Idx].bEnable = FALSE; //default disable.
            break;
        }
    }

    DRV_MBX_UnLockAsync();

    if(s16Idx>=MBX_ASYNCNOTIFIER_MAX)
    {
        return INVALID_PTR;
    }

    return s16Idx;
}
#endif

//-------------------------------------------------------------------------------------------------
/// Free one Async Notifier by Notifer ID
/// @param  u16NotifierID                  \b IN: The notifier ID
/// @return void
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_MBX_FreeAsyncNotifierByID(MS_U16 u16NotifierID)
{
    MBX_ASSERT((u16NotifierID<MBX_ASYNCNOTIFIER_MAX), "Invalid Async Notifier ID: %x\n", u16NotifierID);

    _mbxAsyncNotifierMBX[u16NotifierID].u32AsyncID = NULL;
    _mbxAsyncNotifierMBX[u16NotifierID].u16Usage = FALSE;
    _mbxAsyncNotifierMBX[u16NotifierID].bEnable = FALSE; //default disable.

    return;
}

//-------------------------------------------------------------------------------------------------
/// Get  Notifier ID by Async ID
/// @param  u32AsyncID                  \b IN: The Async ID
/// @return MS_S16:INVALID_PTR: no such an async id has been allocated, get fail
/// @return MS_S16:Async Notifer ID: the notifier id by async id
/// @attention
/// <b>[OBAMA] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_S16 _MDrv_MBX_GetAsyncNotifierIDByAsyncID(MS_U32 u32AsyncID)
{
    MS_S16 s16Idx;

    for(s16Idx = 0; s16Idx<MBX_ASYNCNOTIFIER_MAX; s16Idx++)
    {
        if(_mbxAsyncNotifierMBX[s16Idx].u32AsyncID == u32AsyncID)
        {
            MBX_ASSERT((_mbxAsyncNotifierMBX[s16Idx].u16Usage), "Invalid Usage of allocated Async ID: %x Notifier ID: %x\n", (unsigned int)u32AsyncID, s16Idx);
            return s16Idx;
        }
    }

    return (INVALID_PTR);
}

//=============================================================================
//MBX Config functions:

//-------------------------------------------------------------------------------------------------
// Init Config parameters to Driver Config Parameters
// @param  eHostCPU    \b IN: The config Host CPU ID\n
// @return E_MBX_SUCCESS
// @return E_MBX_ERR_INVALID_PARAM
// @return E_MBX_UNKNOW_ERROR
// @attention
// <b>[MXLIB] <em>Int Config in MDrv_MBX_Init, or After MDrv_MBX_Init</em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result _MDrv_MBX_InitConfig(MBX_CPU_ID eHKCPU, MBX_ROLE_ID eHostRole, MS_U32 u32TimeoutMillSecs)
{
    //Set eMbxHostRole:
    _eMbxHostRole = eHostRole;
    //Set Timeout:
    _u32TimeoutMillSecs = u32TimeoutMillSecs;

    //Set eMbxRole2Cpu, eMbxCpu2Role
    _eMbxRole2Cpu[E_MBX_ROLE_HK] = eHKCPU;
    _eMbxRole2Cpu[E_MBX_ROLE_PM] = E_MBX_CPU_PM;

    _eMbxCpu2Role[E_MBX_CPU_PM] = E_MBX_ROLE_PM;

    if(E_MBX_CPU_AEON == eHKCPU)
    {
        _eMbxRole2Cpu[E_MBX_ROLE_CP] = E_MBX_CPU_MIPS;

        _eMbxCpu2Role[E_MBX_CPU_AEON] = E_MBX_ROLE_HK;
        _eMbxCpu2Role[E_MBX_CPU_MIPS] = E_MBX_ROLE_CP;
    }
    else if(E_MBX_CPU_MIPS == eHKCPU)
    {
        _eMbxRole2Cpu[E_MBX_ROLE_CP] = E_MBX_CPU_AEON;

        _eMbxCpu2Role[E_MBX_CPU_AEON] = E_MBX_ROLE_CP;
        _eMbxCpu2Role[E_MBX_CPU_MIPS] = E_MBX_ROLE_HK;
    }
    else
    {
        return E_MBX_ERR_INVALID_PARAM;
    }

    return E_MBX_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
// Config Host CPU ID of Mailbox driver
// @param  eHostCPU    \b IN: The config Host CPU ID\n
// @return E_MBX_SUCCESS
// @return E_MBX_ERR_INVALID_PARAM
// @return E_MBX_ERR_NOT_INITIALIZED
// @return E_MBX_UNKNOW_ERROR
// @attention
// <b>[MXLIB] <em>Pls Set Config in MDrv_MBX_Init, or After MDrv_MBX_Init</em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result  _MDrv_MBX_SetConfig(MBX_CPU_ID eHKCPU, MBX_ROLE_ID eHostRole, MS_U32 u32TimeoutMillSecs)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;

    //Config MBX:]
    if(u32TimeoutMillSecs == 0)
    {
        return E_MBX_ERR_INVALID_PARAM;
    }

    if((_eMbxHostRole != eHostRole) || (_eMbxRole2Cpu[E_MBX_ROLE_HK] != eHKCPU))
    {
        //Config MBX Hardware
        MHAL_MBX_SetConfig(eHostRole);

        mbxResult = MHAL_MBXINT_ResetHostCPU(_eMbxRole2Cpu[_eMbxHostRole], _eMbxRole2Cpu[eHostRole]);
        if(E_MBX_SUCCESS != mbxResult)
        {
            return mbxResult;
        }

        return _MDrv_MBX_InitConfig(eHKCPU, eHostRole, u32TimeoutMillSecs);
    }

    _u32TimeoutMillSecs = u32TimeoutMillSecs;

    return mbxResult;
}


//=============================================================================
//Util functions:
// Get System Time in MS:
MS_U32 _MDrv_MBX_GetSystemTime(void)
{
    return jiffies_to_msecs(jiffies);
}

//=============================================================================
//Mail Message handler functions:

//-------------------------------------------------------------------------------------------------
/// Send the message to MailBox
/// @param  pMsg                  \b IN: The msg for sending
/// @param  eSrcCPUID                  \b IN: the src cpu which send the mail message
/// @return E_MBX_ERR_PEER_CPU_NOTREADY:peer cpu is still fetching the mail message
/// @return E_MBX_ERR_PEER_CPU_NOT_ALIVE: fire bit is on while busy bit is off seems like peer cpu not alive
/// @return E_MBX_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em> Use spin lock to protect co-access </em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result  _MDrv_MBX_SendMsg(MBX_Msg *pMsg, MBX_ROLE_ID eSrcRole)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;
    MBXHAL_Fire_Status mbxHalFireStatus;
    MS_U32 u32WaitCnt = 0;
    MS_U32 u32TimeTicket;

    DRV_MBX_LockSend();

    mbxResult = MHAL_MBX_Fire(pMsg, eSrcRole);
    if(E_MBX_SUCCESS == mbxResult)
    {//fire interrupt
        MHAL_MBXINT_Fire(_eMbxRole2Cpu[pMsg->eRoleID], _eMbxRole2Cpu[eSrcRole]);
        u32TimeTicket = _MDrv_MBX_GetSystemTime();
        do
        {
            mbxResult = MHAL_MBX_GetFireStatus(eSrcRole, pMsg->eRoleID, &mbxHalFireStatus);

            if(E_MBX_SUCCESS != mbxResult)
            {
                break;
            }

            if(E_MBXHAL_FIRE_OVERFLOW == mbxHalFireStatus)
            {
                mbxResult = E_MBX_ERR_PEER_CPU_OVERFLOW;
                break;
            }

            if(E_MBXHAL_FIRE_DISABLED == mbxHalFireStatus)
            {
                mbxResult = E_MBX_ERR_PEER_CPU_NOTREADY;
                break;
            }

            if(E_MBXHAL_FIRE_SUCCESS == mbxHalFireStatus)
            {
                mbxResult = E_MBX_SUCCESS;
                break;
            }

            //check If Timeout:
            u32WaitCnt++;
            if(u32WaitCnt >= 0x10000)
            {
                if((_MDrv_MBX_GetSystemTime()-u32TimeTicket) >= _u32TimeoutMillSecs)
                {
                    MHAL_MBX_SetConfig(_eMbxHostRole);
                    mbxResult = E_MBX_ERR_PEER_CPU_NOT_ALIVE;
                    break;
                }
                u32WaitCnt = 0;
            }
        }while(TRUE);
    }

    DRV_MBX_UnLockSend();

    return mbxResult;
}

//-------------------------------------------------------------------------------------------------
/// Recv the message from mail message queue
/// @param  mbxAsyncNotifierID                  \b IN: Async Notifier ID
/// @param  eTargetClass                  \b IN: The target class for mail message
/// @param  pMsg                  \b OUT: fectch message
/// @param  u32Flag                  \b IN: indicate MBX_CHECK_NORMAL_MSG / MBX_CHECK_INSTANT_MSG / MBX_CHECK_NORMAL_MSG
/// @return E_MBX_ERR_INVALID_PARAM:no any flag setting in u32Flag
/// @return E_MBX_ERR_NO_MORE_MSG: no any mail message in message pool
/// @return E_MBX_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result  _MDrv_MBX_RecvMsg(MS_S16 mbxAsyncNotifierID, MBX_Class eTargetClass, MBX_Msg *pMsg, MS_U32 u32Flag)
{
    MBX_Result mbxResult = E_MBX_ERR_INVALID_PARAM;
    MS_S16 s16MsgQIdx;

    DRV_MBX_LockRecv();

    if(u32Flag == MBX_CHECK_ALL_MSG_CLASS)
    {
        //check Instance Msg for All registered class:
        s16MsgQIdx = _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst;
        if (u32Flag & MBX_CHECK_INSTANT_MSG)
        {
            while(IS_VALID_PTR(s16MsgQIdx))
            {
                mbxResult = MDrv_MSGQ_RecvMsg(s16MsgQIdx, pMsg, TRUE);
                if(mbxResult == E_MBX_SUCCESS)
                {
                    DRV_MBX_UnLockRecv();
                    return mbxResult;
                }

                s16MsgQIdx = MDrv_MSGQ_GetNextMsgQ(s16MsgQIdx);
            }
        }

        //check Normal Msg for All registered class:
        s16MsgQIdx = _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst;

        if (u32Flag & MBX_CHECK_NORMAL_MSG)
        {
            while(IS_VALID_PTR(s16MsgQIdx))
            {
                mbxResult = MDrv_MSGQ_RecvMsg(s16MsgQIdx, pMsg, FALSE);
                if(mbxResult == E_MBX_SUCCESS)
                {
                    DRV_MBX_UnLockRecv();
                    return mbxResult;
                }

                s16MsgQIdx = MDrv_MSGQ_GetNextMsgQ(s16MsgQIdx);
            }
        }

        DRV_MBX_UnLockRecv();
        return mbxResult;
    }
    else
    {

        if(!IS_VALID_PTR(MDrv_MSGQ_GetNotiferIDByQID(eTargetClass)))
        {
            DRV_MBX_UnLockRecv();
            return E_MBX_ERR_SLOT_NOT_OPENNED;
        }

        if(mbxAsyncNotifierID != MDrv_MSGQ_GetNotiferIDByQID(eTargetClass))
        {
            DRV_MBX_UnLockRecv();
            return E_MBX_ERR_SLOT_BUSY;
        }

        if(u32Flag & MBX_CHECK_INSTANT_MSG)
        {
            //check Instance Msg for All registered class:
            mbxResult = MDrv_MSGQ_RecvMsg(eTargetClass, pMsg, TRUE);
            if(mbxResult == E_MBX_SUCCESS)
            {
                DRV_MBX_UnLockRecv();
                return mbxResult;
            }
         }

        if(u32Flag & MBX_CHECK_NORMAL_MSG)
        {
            //check Instance Msg for All registered class:
            mbxResult = MDrv_MSGQ_RecvMsg(eTargetClass, pMsg, FALSE);
        }
    }

    DRV_MBX_UnLockRecv();
    return mbxResult;
}

//-------------------------------------------------------------------------------------------------
/// Check the message from mail message queue
/// @param  mbxAsyncNotifierID                  \b IN: Async Notifier ID
/// @param  eTargetClass                  \b IN: The target class for mail message
/// @param  pMsg                  \b OUT: fectch message
/// @param  u32Flag                  \b IN: indicate MBX_CHECK_NORMAL_MSG / MBX_CHECK_INSTANT_MSG / MBX_CHECK_NORMAL_MSG
/// @return E_MBX_ERR_INVALID_PARAM:no any flag setting in u32Flag
/// @return E_MBX_ERR_NO_MORE_MSG: no any mail message in message pool
/// @return E_MBX_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result  _MDrv_MBX_CheckMsg(MS_S16 mbxAsyncNotifierID, MBX_Class eTargetClass, MBX_Msg *pMsg, MS_U32 u32Flag)
{
    MBX_Result mbxResult = E_MBX_ERR_INVALID_PARAM;
    MS_S16 s16MsgQIdx;

    DRV_MBX_LockRecv();

    if(u32Flag & MBX_CHECK_ALL_MSG_CLASS)
    {
        //check Instance Msg for All registered class:
        s16MsgQIdx = _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst;
        if (u32Flag & MBX_CHECK_INSTANT_MSG)
        {
            while(IS_VALID_PTR(s16MsgQIdx))
            {
                mbxResult = MDrv_MSGQ_CheckMsg(s16MsgQIdx, pMsg, TRUE);
                if(mbxResult == E_MBX_SUCCESS)
                {
                    DRV_MBX_UnLockRecv();
                    return mbxResult;
                }

                s16MsgQIdx = MDrv_MSGQ_GetNextMsgQ(s16MsgQIdx);
            }
        }

        //check Normal Msg for All registered class:
        s16MsgQIdx = _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst;

        if (u32Flag & MBX_CHECK_NORMAL_MSG)
        {
            while(IS_VALID_PTR(s16MsgQIdx))
            {
                mbxResult = MDrv_MSGQ_CheckMsg(s16MsgQIdx, pMsg, FALSE);
                if(mbxResult == E_MBX_SUCCESS)
                {
                    DRV_MBX_UnLockRecv();
                    return mbxResult;
                }

                s16MsgQIdx = MDrv_MSGQ_GetNextMsgQ(s16MsgQIdx);
            }
        }

        DRV_MBX_UnLockRecv();
        return mbxResult;
    }

    if(!IS_VALID_PTR(MDrv_MSGQ_GetNotiferIDByQID(eTargetClass)))
    {
        DRV_MBX_UnLockRecv();
        return E_MBX_ERR_SLOT_NOT_OPENNED;
    }

    if(mbxAsyncNotifierID != MDrv_MSGQ_GetNotiferIDByQID(eTargetClass))
    {
        DRV_MBX_UnLockRecv();
        return E_MBX_ERR_SLOT_BUSY;
    }

    if(u32Flag & MBX_CHECK_INSTANT_MSG)
    {
        //check Instance Msg for All registered class:
        mbxResult = MDrv_MSGQ_CheckMsg(eTargetClass, pMsg, TRUE);
        if(mbxResult == E_MBX_SUCCESS)
        {
            DRV_MBX_UnLockRecv();
            return mbxResult;
        }
     }

    if(u32Flag & MBX_CHECK_NORMAL_MSG)
    {
        //check Instance Msg for All registered class:
        mbxResult = MDrv_MSGQ_CheckMsg(eTargetClass, pMsg, FALSE);
    }

    DRV_MBX_UnLockRecv();
    return mbxResult;
}


//=============================================================================
// Mailbox Driver Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// When Mail Arrived, the function will be called in interrupt tasklet. it will notify the UMD Driver the msg arrived.
/// @param  s32Irq                  \b IN: The Irq Number which indicated the interrupt is from which to which.
/// @return void
/// @attention
/// <b>[OBAMA] <em>get message class information from signal information. </em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_MBX_MsgRecvCb(MS_S32 s32Irq)
{
    MBX_Msg mbxMsg;
    MS_S16 s16MbxAsyncNotifierID;
    MBX_Result mbxResult;
    DRV_MBX_LockRecv();
    switch(s32Irq)
    {
        case MBX_INT_AEON2PM:
        case MBX_INT_AEON2MIPS:
            mbxMsg.eRoleID = _eMbxCpu2Role[E_MBX_CPU_AEON];
            break;
        case MBX_INT_MIPS2PM:
        case MBX_INT_MIPS2AEON:
            mbxMsg.eRoleID = _eMbxCpu2Role[E_MBX_CPU_MIPS];
            break;
        case MBX_INT_PM2AEON:
        case MBX_INT_PM2MIPS:
            mbxMsg.eRoleID = _eMbxCpu2Role[E_MBX_CPU_PM];
            break;
        default:
            DRV_MBX_UnLockRecv();
            return;
    }

    mbxResult = MHAL_MBX_Recv(&mbxMsg, _eMbxHostRole);
    if(E_MBX_SUCCESS != mbxResult) //receive message failed, just skip it.
    {
        DRV_MBX_UnLockRecv();
        return;
    }

    s16MbxAsyncNotifierID = MDrv_MSGQ_GetNotiferIDByQID(mbxMsg.u8MsgClass);

    if(!IS_VALID_PTR(s16MbxAsyncNotifierID))
    {//the slot not registered yet, just skip the msg.
        MHAL_MBX_RecvEnd(mbxMsg.eRoleID, _eMbxHostRole, E_MBXHAL_RECV_DISABLED);
        DRV_MBX_UnLockRecv();
        return;
    }

    if(_mbxAsyncNotifierMBX[s16MbxAsyncNotifierID].bEnable == FALSE)
    {//application do not want to recv. the msg:
        MHAL_MBX_RecvEnd(mbxMsg.eRoleID, _eMbxHostRole, E_MBXHAL_RECV_DISABLED);
        DRV_MBX_UnLockRecv();
        return;
    }

    mbxResult = MDrv_MSGQ_AddMSG(&mbxMsg); //Add to msg queue, maybe failed when overflow.

    switch(mbxResult)
    {
        case E_MSGPOOL_ERR_NOT_INITIALIZED:
            MHAL_MBX_RecvEnd(mbxMsg.eRoleID, _eMbxHostRole, E_MBXHAL_RECV_DISABLED);
            break;
//        case E_MSGPOOL_ERR_NO_MORE_MSG:
        case E_MSGPOOL_ERR_NO_MORE_MEMORY:
            MHAL_MBX_RecvEnd(mbxMsg.eRoleID, _eMbxHostRole, E_MBXHAL_RECV_OVERFLOW);
            break;
        default:
            MHAL_MBX_RecvEnd(mbxMsg.eRoleID, _eMbxHostRole, E_MBXHAL_RECV_SUCCESS);
            break;
    }
    DRV_MBX_UnLockRecv();
    //notify the application:
    kill_fasync(&_mbxAsyncNotifierMBX[s16MbxAsyncNotifierID].async_queue, SIGIO, POLL_IN | (mbxMsg.u8MsgClass<<MBXCLASS_IN_SIGNAL_SHIFT));

	#ifdef CONFIG_MSTAR_PM_SWIR
    if (MDrv_MBX_MsgCbFunc) //To-do: function pointer array
        MDrv_MBX_MsgCbFunc();
	#endif
}

/*
  Function: MDrv_MBX_Startup()
  brief: Startup Mailbox driver, including startup Msg Pool, Interrupt...
  Input: None
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NO_MORE_MEMORY-->Not enough memory
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_Startup(void)
{
    //init msg pool:
    if(E_MBX_SUCCESS != (MBX_Result)MDrv_MSGPOOL_Init())
    {
        return E_MBX_ERR_NO_MORE_MEMORY;
    }

    //init msg queue:
    MDrv_MSGQ_Init();

    //init spin lock:
    //DRV_MBX_LockAsync_Init();
    DRV_MBX_LockSend_Init();
    DRV_MBX_LockRecv_Init();
    //init async Notifier:
    memset((void*)_mbxAsyncNotifierMBX, 0, (sizeof(MBX_ASYNC_NOTIFIER)*MBX_ASYNCNOTIFIER_MAX));

    return E_MBX_SUCCESS;
}

/*
  Function: MDrv_MBX_Exit()
  brief: Exit Mailbox driver, including exit Msg Pool, Interrupt...
  Input: None
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NO_MORE_MEMORY-->Not enough memory
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_Exit(void)
{
    //DE-INIT MBX pool
    MDrv_MSGPOOL_DeInit();

    //DE-INIT MBX interrupt:
    MHAL_MBXINT_DeInit(_eMbxRole2Cpu[_eMbxHostRole]);

    return E_MBX_SUCCESS;
}

#ifdef CONFIG_MSTAR_PM_SWIR
/*
  Function: MDrv_MBX_NotifyMsgRecCbFunc()
  brief: notify MBX receive callback function
  Input: None
  Output: None
  Return Value:
        TRUE-->Successed
        FALSE-->notify fail or repeat notify
*/
int MDrv_MBX_NotifyMsgRecCbFunc(void *p(void))
{
    MDrv_MBX_MsgCbFunc = p; //warning messg to avoid repeat
    return TRUE;
}
#endif

//-------------------------------------------------------------------------------------------------
/// Init Mailbox driver
/// @param  eHostCPU    \b IN: The configged CPU ID which mbx driver will run on\n
/// @return E_MBX_SUCCESS
/// @return E_MBX_ERR_NOT_INITIALIZED
/// @return E_MBX_ERR_NO_MORE_MEMORY
/// @return E_MBX_UNKNOW_ERROR
/// @attention
/// <b>[MXLIB] <em></em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result  MDrv_MBX_Init(MS_U32 u32AsyncID, MBX_CPU_ID eHKCPU, MBX_ROLE_ID eHostRole, MS_U32 u32TimeoutMillSecs)
{
    MBX_Result result;
    DRV_MBX_LockRecv();
    if(IS_VALID_PTR(_MDrv_MBX_AllocateAsyncNotifier(u32AsyncID)))
    {
        if(E_MBX_ROLE_MAX ==_eMbxHostRole) //Init
        {
            if(E_MBX_SUCCESS != _MDrv_MBX_InitConfig(eHKCPU, eHostRole, u32TimeoutMillSecs))
            {
                DRV_MBX_UnLockRecv();
                return E_MBX_ERR_INVALID_PARAM;
            }

            //Set mbx hardware:
            MHAL_MBX_Init(_eMbxHostRole);

            //Set mbx int hardware:
            if(E_MBX_SUCCESS != MHAL_MBXINT_Init(_eMbxRole2Cpu[eHostRole], _MDrv_MBX_MsgRecvCb))
            {
                DRV_MBX_UnLockRecv();
                return E_MBX_ERR_INVALID_CPU_ID;
            }
        }
        else
        {
            result=_MDrv_MBX_SetConfig(eHKCPU, eHostRole, u32TimeoutMillSecs);
            DRV_MBX_UnLockRecv();
            return result;
        }
        DRV_MBX_UnLockRecv();
        return E_MBX_SUCCESS;
    }
    DRV_MBX_UnLockRecv();
    return E_MBX_ERR_NO_MORE_MEMORY;
}



/*
  Function: MDrv_MBX_DeInit()
  brief: Deinitialize Mailbox driver
  Input: bForceDiscardPendingMsg--->drop un-processed messages or not
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB driver is not initialized
        MBX_ERR_HAS_MSG_PENDING-->Has unprocessed message.if bForceDiscardPendingMsg is set to TRUE, this  value never be returned
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_DeInit(MS_U32 u32AsyncID, MS_BOOL bForceDiscardPendingMsg)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;
    MS_S16 mbxAsyncNotifierID;
    DRV_MBX_LockRecv();
    mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);

    //handle msg q DeInit:
    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        DRV_MBX_UnLockRecv();
        return E_MBX_ERR_NOT_INITIALIZED;
    }

    if(!bForceDiscardPendingMsg) //false
    {
            if(!IS_VALID_PTR(_mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst)) //no any msg queue.
            {
                DRV_MBX_UnLockRecv();
                return E_MBX_SUCCESS;
            }

            mbxResult = (MBX_Result)MDrv_MSGQ_UnRegisterMSGQ(_mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst, bForceDiscardPendingMsg);

            if(mbxResult == E_MBX_SUCCESS)
            {
                _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst = INVALID_PTR; //there has no mail message, just
            }
            DRV_MBX_UnLockRecv();
            return mbxResult;
    }

    if(!IS_VALID_PTR(_mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst)) //no any msg queue.
    {
        _MDrv_MBX_FreeAsyncNotifierByID(mbxAsyncNotifierID);
    }
    else
    {
        mbxResult = (MBX_Result)MDrv_MSGQ_UnRegisterMSGQ(_mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst, bForceDiscardPendingMsg);

        if(mbxResult == E_MBX_SUCCESS)
        {
            _MDrv_MBX_FreeAsyncNotifierByID(mbxAsyncNotifierID);
        }
        else
        {
             DRV_MBX_UnLockRecv();
             return mbxResult;
        }
    }

    //handle other DeInit:
    DRV_MBX_UnLockRecv();
    return mbxResult;
}

int MDrv_MBX_Suspend(MSTAR_MBX_DEV *pmbx_dev)
{
    MBX_CPU_ID eHostCPU;
    if(!pmbx_dev)
        return -1;
    eHostCPU=_eMbxRole2Cpu[_eMbxHostRole];
    switch(eHostCPU)
    {
        case E_MBX_CPU_PM:
            disable_irq(E_FIQ_INT_MIPS_TO_8051);
            break;
        case E_MBX_CPU_MIPS:
            disable_irq(E_FIQ_INT_8051_TO_MIPS);
            break;
        default:
            break;
    }
    return 0;
}
int MDrv_MBX_Resume(MSTAR_MBX_DEV *pmbx_dev)
{
    MBX_CPU_ID eHostCPU;
    if(!pmbx_dev)
        return -1;
    if(_eMbxHostRole<E_MBX_ROLE_MAX)
        MHAL_MBX_SetConfig(_eMbxHostRole);
    eHostCPU=_eMbxRole2Cpu[_eMbxHostRole];
    switch(eHostCPU)
    {
        case E_MBX_CPU_PM:
            enable_irq(E_FIQ_INT_MIPS_TO_8051);
            break;
        case E_MBX_CPU_MIPS:
            enable_irq(E_FIQ_INT_8051_TO_MIPS);
            break;
        default:
            break;
    }
    return 0;
}
//-------------------------------------------------------------------------------------------------
/// register for real time signal notification.
/// @param  s32Fd      \b IN: s32Fd for fasync_helper
/// @param  u32AsyncID      \b IN: Async ID
/// @param  mode      \b IN: On/Off
/// @return E_MBX_ERR_INVALID_PARAM: not registered AsyncID
/// @return E_MBX_UNKNOW_ERROR: fasync register failed
/// @return E_MBX_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>one AsyncID, one notification queue </em></b>
MBX_Result MDrv_MBX_FASYNC(MS_S32 s32Fd, MS_U32 u32AsyncID, MS_S32 s32Mode)
{
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return E_MBX_ERR_INVALID_PARAM;
    }

    //register the async queue:
    if(fasync_helper(s32Fd, (struct file *)u32AsyncID, s32Mode, &_mbxAsyncNotifierMBX[mbxAsyncNotifierID].async_queue)<0)
    {
        return E_MBX_UNKNOW_ERROR;
    }

    return E_MBX_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// un-register for real time signal notification when application exit
/// @param  u32AsyncID      \b IN: Async ID
/// @return E_MBX_ERR_INVALID_PARAM: not registered AsyncID
/// @return E_MBX_UNKNOW_ERROR: fasync un-register failed
/// @return E_MBX_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>one AsyncID, one notification queue </em></b>
MBX_Result MDrv_MBX_ReleaseFASYNC(MS_U32 u32AsyncID)
{
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return E_MBX_ERR_INVALID_PARAM;
    }

     //un register the async queue:
    if(fasync_helper(-1, (struct file *)u32AsyncID, 0, &_mbxAsyncNotifierMBX[mbxAsyncNotifierID].async_queue)<0)
    {
        return E_MBX_UNKNOW_ERROR;
    }

    return E_MBX_SUCCESS;
}

/*
  Function: MDrv_MBX_RegisterMSG()
  brief: Regisiter Message queue for special class
  Input: u8Class--->target message class
         u16MsgQueueSize-->target message queue size, will be allocated in kernel space
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB Driver is not initialized
        MBX_ERR_NO_MORE_MEMORY-->Not enough memory
        MBX_ERR_SLOT_BUSY-->class has been used by other APP
        MBX_ERR_SLOT_AREADY_OPENNED-->class has been openned by this APP, you do not need to open it again
        MBX_ERR_INVALID_PARAM-->Invalid parameter, please check if u8Class is to valid or u16MsgQueueSize is not larger than MAX_MBX_QUEUE_SIZE.
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_RegisterMSG(MS_U32 u32AsyncID, MBX_Class eMsgClass, MS_U16 u16MsgQueueSize)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return E_MBX_ERR_NOT_INITIALIZED;
    }

    DRV_MBX_LockRecv();

    mbxResult = (MBX_Result)MDrv_MSGQ_RegisterMSG(mbxAsyncNotifierID, _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst, (MS_S16)eMsgClass, u16MsgQueueSize);

    DRV_MBX_UnLockRecv();

    if(mbxResult == E_MBX_SUCCESS)
    {
        _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst = (MS_S16)eMsgClass;
    }

    return mbxResult;
}

/*
  Function: MDrv_MBX_UnRegisterMSG()
  brief: UnRegisiter Message queue for special class
  Input: u8Class--->target message class
         bForceDiscardMsgQueue-->drop un-processed messages or not
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB drv is not initialized
        MBX_ERR_SLOT_BUSY-->class has been used by other APP
        MBX_ERR_SLOT_NOT_OPENNED-->this class is never regisitered
        MBX_ERR_HAS_MSG_PENDING-->Has unprocessed message.if bForceDiscardMsgQueue is set to TRUE, this  value never be returned.
        MBX_ERR_INVALID_PARAM-->Invalid parameter, please check if u8Class is to valid.
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_UnRegisterMSG(MS_U32 u32AsyncID, MBX_Class eMsgClass, MS_BOOL bForceDiscardMsgQueue)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);
    MS_S16 s16MsgQFirst;

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return E_MBX_ERR_NOT_INITIALIZED;
    }

    s16MsgQFirst = _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst;

    DRV_MBX_LockRecv();

    mbxResult = (MBX_Result)MDrv_MSGQ_UnRegisterMSG(mbxAsyncNotifierID, &s16MsgQFirst, (MS_S16)eMsgClass, bForceDiscardMsgQueue);

    DRV_MBX_UnLockRecv();

    if(mbxResult == E_MBX_SUCCESS)
        _mbxAsyncNotifierMBX[mbxAsyncNotifierID].s16MsgQFirst = s16MsgQFirst;

    return mbxResult;
}

//-------------------------------------------------------------------------------------------------
/// Clear Message queue for special class
/// @param  eMsgClass \b IN: Mail Message Class, @ref MBX_Class
/// @return E_MBX_SUCCESS
/// @return E_MBX_ERR_INVALID_PARAM
/// @return E_MBX_ERR_NOT_INITIALIZED
/// @return E_MBX_ERR_SLOT_NOT_OPENNED
/// @return E_MBX_UNKNOW_ERROR
/// @attention
/// <b>[MXLIB] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result  MDrv_MBX_ClearMSG(MS_U32 u32AsyncID, MBX_Class eMsgClass)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return E_MBX_ERR_NOT_INITIALIZED;
    }

    DRV_MBX_LockRecv();

    mbxResult = (MBX_Result)MDrv_MSGQ_ClearMSG(mbxAsyncNotifierID, (MS_S16)eMsgClass);

    DRV_MBX_UnLockRecv();

    return mbxResult;
}

/*
  Function: MDrv_MBX_GetMsgQueueStatus()
  brief: Check the message queue status of a special message class
  Input: targetClass--->target class
  Output: pMsgQueueStatus--> return the queue status
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB drv is not initialized
        MBX_ERR_INVALID_PARAM-->Invalid parameter, please check if targetClass is valid is pMsgQueueStatus is not NULL.
        MBX_ERR_SLOT_BUSY-->class has been used by other APP
        MBX_ERR_SLOT_NOT_OPENNED-->this class is never regisitered
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_GetMsgQueueStatus(MS_U32 u32AsyncID, MBX_Class eTargetClass, MBX_MSGQ_Status *pMsgQueueStatus)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return E_MBX_ERR_NOT_INITIALIZED;
    }

    DRV_MBX_LockRecv();

    mbxResult = MDrv_MSGQ_GetMsgQStatus(mbxAsyncNotifierID, (MS_S16)eTargetClass, pMsgQueueStatus);

    DRV_MBX_UnLockRecv();

    return mbxResult;
}

/*
  Function: MDrv_MBX_SendMsg()
  brief: send message
  Input: pMsg--->message to send
         bWaitPeerIdle-->wait Peer CPU or not
         u32WaitMillSecs--> if bWaitPeerIdle, indentify the timeout millsecons before return
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB drv is not initialized
        MBX_ERR_INVALID_PARAM-->Invalid parameter, please check if pMsg content is valid or not
        MBX_ERR_SLOT_NOT_OPENNED-->this class is never regisitered
        MBX_ERR_INVALID_CPU_ID-->target cpu defined in pMsg is invalid.
        MBX_ERR_PEER_CPU_BUSY-->peer CPU is still busy
        MBX_ERR_PEER_CPU_NOT_ALIVE-->peer CPU seems not alive
        MBX_ERR_TIME_OUT-->timeout if bWaitPeerIdle is set to TRUE
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_SendMsg(MS_U32 u32AsyncID, MBX_Msg *pMsg)
{
    //parameter check:
    if(NULL==pMsg)
    {
        return E_MBX_ERR_INVALID_PARAM;
    }

    return _MDrv_MBX_SendMsg(pMsg, _eMbxHostRole);
}


/*
  Function: MDrv_MBX_SendMsgLoopback()
  brief: send message loop back
  Input: pMsg--->message to send
         bWaitPeerIdle-->wait Peer CPU or not
         u32WaitMillSecs--> if bWaitPeerIdle, indentify the timeout millsecons before return
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB drv is not initialized
        MBX_ERR_INVALID_PARAM-->Invalid parameter, please check if pMsg content is valid or not
        MBX_ERR_SLOT_NOT_OPENNED-->this class is never regisitered
        MBX_ERR_INVALID_CPU_ID-->target cpu defined in pMsg is invalid.
        MBX_ERR_PEER_CPU_BUSY-->peer CPU is still busy
        MBX_ERR_PEER_CPU_NOT_ALIVE-->peer CPU seems not alive
        MBX_ERR_TIME_OUT-->timeout if bWaitPeerIdle is set to TRUE
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_SendMsgLoopback(MS_U32 u32AsyncID, MBX_Msg *pMsg, MBX_ROLE_ID eSrcRoleId)
{
    //parameter check:
    if(NULL==pMsg)
    {
        return E_MBX_ERR_INVALID_PARAM;
    }

    return _MDrv_MBX_SendMsg(pMsg, eSrcRoleId);
}


/*
  Function: MDrv_MBX_RecvMsg()
  brief: recv message
  Input: targetClass--->target class
         bWaitPeerIdle-->wait Peer CPU or not
         u32Flag-->recv flag, has the following possible meaning bits
                       MBX_CHECK_ALL_MSG_CLASS -->recv any message class, this means, the targetClass is useless if this bit is set.
                       MBX_CHECK_INSTANT_MSG-->check INSTANT message
                       MBX_CHECK_NORMAL_MSG->Check Normal message
                       MBX_CHECK_BLOCK_RECV-->block this function call until time out if no message available
  Output: pMsg  message received
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB drv is not initialized
        MBX_ERR_INVALID_PARAM-->Invalid parameter, please check if targetClass, pMsg and u32Flag is valid or not.
        MBX_ERR_SLOT_NOT_OPENNED-->this class is never regisitered
        MBX_ERR_NO_MORE_MSG-->No message is available
        MBX_ERR_TIME_OUT-->timeout if MBX_CHECK_BLOCK_RECV is set to int u32Flag
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_RecvMsg(MS_U32 u32AsyncID, MBX_Class eTargetClass, MBX_Msg *pMsg, MS_U32 u32WaitMillSecs, MS_U32 u32Flag)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);
    MS_U32 u32Idx;

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return E_MBX_ERR_NOT_INITIALIZED;
    }

    if(!_bCalledFromDriver)
    {
        if((u32Flag!=MBX_CHECK_INSTANT_MSG)&&(u32Flag!=MBX_CHECK_NORMAL_MSG))
        {
            return E_MBX_ERR_INVALID_PARAM;
        }
        _bCalledFromDriver=FALSE;
    }

    mbxResult = _MDrv_MBX_RecvMsg(mbxAsyncNotifierID, eTargetClass, pMsg, u32Flag);
    if((mbxResult == E_MBX_ERR_NO_MORE_MSG) && (u32Flag&MBX_CHECK_BLOCK_RECV))
    {
        for(u32Idx=0; u32Idx<u32WaitMillSecs; u32Idx++)
        {
            mbxResult = _MDrv_MBX_RecvMsg(mbxAsyncNotifierID, eTargetClass, pMsg, u32Flag);

            if(mbxResult != E_MBX_ERR_NO_MORE_MSG)
            {
                break;
            }

            msleep(1);
        }

        if(mbxResult == E_MBX_ERR_NO_MORE_MSG)
        {
            mbxResult = E_MBX_ERR_TIME_OUT;
        }
    }

    return mbxResult;
}

/*
  Function: MDrv_MBX_CheckMsg()
  brief: Check message
  Input: targetClass--->target class
         bWaitPeerIdle-->wait Peer CPU or not
         u32Flag-->recv flag, has the following possible meaning bits
                       MBX_CHECK_ALL_MSG_CLASS -->recv any message class, this means, the targetClass is useless if this bit is set.
                       MBX_CHECK_INSTANT_MSG-->check INSTANT message
                       MBX_CHECK_NORMAL_MSG->Check Normal message
                       MBX_CHECK_BLOCK_RECV-->block this function call until time out if no message available
  Output: pMsg  message received (The function only check the message in the Queue, but no remove message from Q after checking.)
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB drv is not initialized
        MBX_ERR_INVALID_PARAM-->Invalid parameter, please check if targetClass, pMsg and u32Flag is valid or not.
        MBX_ERR_SLOT_NOT_OPENNED-->this class is never regisitered
        MBX_ERR_NO_MORE_MSG-->No message is available
        MBX_ERR_TIME_OUT-->timeout if MBX_CHECK_BLOCK_RECV is set to int u32Flag
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_CheckMsg(MS_U32 u32AsyncID, MBX_Class eTargetClass, MBX_Msg *pMsg, MS_U32 u32WaitMillSecs, MS_U32 u32Flag)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);
    MS_U32 u32Idx;

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return E_MBX_ERR_NOT_INITIALIZED;
    }

    if((u32Flag!=MBX_CHECK_INSTANT_MSG)&&(u32Flag!=MBX_CHECK_NORMAL_MSG)&&(u32Flag!=MBX_CHECK_ALL_MSG_CLASS))
    {
        return E_MBX_ERR_INVALID_PARAM;
    }

    mbxResult = _MDrv_MBX_CheckMsg(mbxAsyncNotifierID, eTargetClass, pMsg, u32Flag);
    if((mbxResult == E_MBX_ERR_NO_MORE_MSG) && (u32Flag&MBX_CHECK_BLOCK_RECV))
    {
        for(u32Idx=0; u32Idx<u32WaitMillSecs; u32Idx++)
        {
            mbxResult = _MDrv_MBX_CheckMsg(mbxAsyncNotifierID, eTargetClass, pMsg, u32Flag);

            if(mbxResult != E_MBX_ERR_NO_MORE_MSG)
            {
                break;
            }

            msleep(1);
        }

        if(mbxResult == E_MBX_ERR_NO_MORE_MSG)
        {
            mbxResult = E_MBX_ERR_TIME_OUT;
        }
    }

    return mbxResult;
}
/*
  Function: MDrv_MBX_RemoveLatestMsg()
  brief: Remove the latest received message, this function is used right after check message within message received interrupt handler
  Input: None
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        E_MBX_ERR_NO_MORE_MSG-->no message to be removed
*/
MBX_Result  MDrv_MBX_RemoveLatestMsg(void)
{
    MBX_Result mbxResult = E_MBX_SUCCESS;

    DRV_MBX_LockRecv();

    mbxResult = MDrv_MSGQ_RemoveLatestMsg();

    DRV_MBX_UnLockRecv();

    return mbxResult;
}

//The u32AsyncID is useless since SetInformation could be called before Init.
MBX_Result  MDrv_MBX_SetInformation(MS_U32 u32AsyncID, MBX_ROLE_ID eTargetRole, MS_U8 *pU8Info, MS_U8 u8Size)
{
    return MHAL_MBX_SetInformation(eTargetRole, pU8Info, u8Size);
}

//The u32AsyncID is useless since SetInformation could be called before Init.
MBX_Result  MDrv_MBX_GetInformation(MS_U32 u32AsyncID, MBX_ROLE_ID eTargetRole, MS_U8 *pU8Info, MS_U8 u8Size)
{
    return MHAL_MBX_GetInformation(eTargetRole, pU8Info, u8Size);
}

/*
  Function: MDrv_MBX_Enable()
  brief: Enable Message Queue to recv message from peer CPUs, Enable receiving regisited messages in kernel
  Input: bEnable-->Enable or not
  Output: None
  Return Value:
        MBX_SUCCESS-->Successed
        MBX_ERR_NOT_INITIALIZED-->MB drv is not initialized
        MBX_UNKNOW_ERROR-->Other undefined errors
*/
MBX_Result  MDrv_MBX_Enable(MS_U32 u32AsyncID, MS_BOOL bEnable)
{
    MS_S16 mbxAsyncNotifierID;
    DRV_MBX_LockRecv();
    mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        DRV_MBX_UnLockRecv();
        return E_MBX_ERR_NOT_INITIALIZED;
    }

    _mbxAsyncNotifierMBX[mbxAsyncNotifierID].bEnable = bEnable;
    DRV_MBX_UnLockRecv();
    return E_MBX_SUCCESS;
}

MS_BOOL  MDrv_MBX_GetEnableStatus(MS_U32 u32AsyncID)
{
    MS_S16 mbxAsyncNotifierID = _MDrv_MBX_GetAsyncNotifierIDByAsyncID(u32AsyncID);

    if(!IS_VALID_PTR(mbxAsyncNotifierID))
    {
        return FALSE;
    }

    return _mbxAsyncNotifierMBX[mbxAsyncNotifierID].bEnable;
}
#ifdef CONFIG_MSTAR_PM_WDT
extern int Kernel_Level();
#endif
void  MDrv_MBX_NotifyPMtoSetPowerOff(void)
{
    //write MB RIU to notify 51 to poweroff mips
    MS_U16 grp=MHAL_MBX_RegGroup(E_MBX_ROLE_HK,E_MBX_ROLE_PM);
    if(grp==0xFF)
        return;
#ifdef CONFIG_MSTAR_PM_WDT
    if( Kernel_Level() != 0 )  //Debug mode
    {
        REG8_MBX_GROUP(grp, 0x0) = 0xDD;
        REG8_MBX_GROUP(grp, 0x1) = 0xDD;
    }
    else   //Release mode
#endif
    {
    REG8_MBX_GROUP(grp, 0x0) = 0x99;
    REG8_MBX_GROUP(grp, 0x1) = 0x88;
    }
    REG8_MBX_GROUP(grp, 0x2) = 0x77;
    REG8_MBX_GROUP(grp, 0x3) = 0x66;
    REG8_MBX_GROUP(grp, 0x4) = 0x55;
    REG8_MBX_GROUP(grp, 0x5) = 0x44;
    REG8_MBX_GROUP(grp, 0x6) = 0x33;
    REG8_MBX_GROUP(grp, 0x7) = 0x22;
    REG8_MBX_GROUP(grp, 0x8) = 0x11;
    REG8_MBX_GROUP(grp, 0x9) = 0x00;
    REG8_MBX_GROUP(grp, 0xA) = 0xFF;
    REG8_MBX_GROUP(grp, 0xB) = 0xEE;
    REG8_MBX_GROUP(grp, 0xC) = 0xDD;
    REG8_MBX_GROUP(grp, 0xD) = 0xCC;
    REG8_MBX_GROUP(grp, 0xE) = 0xBB;
    REG8_MBX_GROUP(grp, 0xF) = 0xAA;
}
EXPORT_SYMBOL(MDrv_MBX_NotifyPMtoSetPowerOff);

void  MDrv_MBX_NotifyPMPassword(unsigned char passwd[16])
{
    MS_U16 grp=MHAL_MBX_RegGroup(E_MBX_ROLE_HK,E_MBX_ROLE_PM);
    if(grp==0xFF)
        return;
    REG8_MBX_GROUP(grp, 0x0) = passwd[0];
    REG8_MBX_GROUP(grp, 0x1) = passwd[1];
    REG8_MBX_GROUP(grp, 0x2) = passwd[2];
    REG8_MBX_GROUP(grp, 0x3) = passwd[3];
    REG8_MBX_GROUP(grp, 0x4) = passwd[4];
    REG8_MBX_GROUP(grp, 0x5) = passwd[5];
    REG8_MBX_GROUP(grp, 0x6) = passwd[6];
    REG8_MBX_GROUP(grp, 0x7) = passwd[7];
    REG8_MBX_GROUP(grp, 0x8) = passwd[8];
    REG8_MBX_GROUP(grp, 0x9) = passwd[9];
    REG8_MBX_GROUP(grp, 0xA) = passwd[10];
    REG8_MBX_GROUP(grp, 0xB) = passwd[11];
    REG8_MBX_GROUP(grp, 0xC) = passwd[12];
    REG8_MBX_GROUP(grp, 0xD) = passwd[13];
    REG8_MBX_GROUP(grp, 0xE) = passwd[14];
    REG8_MBX_GROUP(grp, 0xF) = passwd[15];
}
EXPORT_SYMBOL(MDrv_MBX_NotifyPMPassword);

#ifdef CONFIG_MSTAR_PM_SWIR
EXPORT_SYMBOL(MDrv_MBX_Init);
EXPORT_SYMBOL(MDrv_MBX_RegisterMSG);
EXPORT_SYMBOL(MDrv_MBX_SendMsg);
EXPORT_SYMBOL(MDrv_MBX_RecvMsg);
EXPORT_SYMBOL(MDrv_MBX_Enable);
EXPORT_SYMBOL(MDrv_MBX_NotifyMsgRecCbFunc);
#endif


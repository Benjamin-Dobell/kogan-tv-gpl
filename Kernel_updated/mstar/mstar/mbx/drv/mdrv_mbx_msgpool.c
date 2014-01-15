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
/// file    mdrv_mbx_msgpool.c
/// @brief  MStar MailBox DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _DRV_MBX__MSGPOOL_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/spinlock.h>
#include <linux/mm.h>

#include "mdrv_mstypes.h"
#include "mdrv_mbx.h"
#include "mdrv_mbx_msgpool.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Local Defines
//=============================================================================
#define MBX_MSGPOOL_PAGES  (14)

//=============================================================================
// Debug Macros
//=============================================================================
#define MSGPOOL_DEBUG
#ifdef MSGPOOL_DEBUG
    #define MSGPOOL_PRINT(fmt, args...)      printk("[MailBox (Driver)][%05d] " fmt, __LINE__, ## args)
    #define MSGPOOL_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        MSGPOOL_PRINT(_fmt, ##_args);  \
                                        while(1);               \
                                    }
#else
    #define MSGPOOL_PRINT(_fmt, _args...)
    #define MSGPOOL_ASSERT(_cnd, _fmt, _args...)
#endif

//----------------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------------
#define MSGPOOL_MSGQ_MAX  E_MBX_CLASS_MAX

//----------------------------------------------------------------------------
// Local Variables
//----------------------------------------------------------------------------
static MSGPOOL_MsgPoolInfo _infoMSGP;
static MSGPOOL_LastRecvMsgInfo _infoLatestMSGP;
static MSGPOOL_MsgQMgr _msgQMgrMSGQ[MSGPOOL_MSGQ_MAX];

//----------------------------------------------------------------------------
// Global Variables
//----------------------------------------------------------------------------

//=============================================================================
// Local Function Prototypes
//=============================================================================

//=============================================================================
// Mailbox Driver MSG POOL Function
static void _MDrv_MSGPOOL_FreeMSGPoolItemQ(MS_S16 s16MsgFirst);
static void _MDrv_MSGPOOL_SetMsgToPoolItem(MS_S16 s16SlotIdx, MBX_Msg* pMbxMsg);
static void _MDrv_MSGPOOL_GetMsgFromPoolItem(MS_S16 s16SlotIdx, MBX_Msg* pMbxMsg);
static MSGPOOL_Result _MDrv_MSGPOOL_AllocateSlot(MS_S16* s16SlotIdx);
static MSGPOOL_Result _MDrv_MSGPOOL_FreeSlot(MS_S16 s16SlotIdx);
static MS_S16 _MDrv_MSGPOOL_GetNextSlot(MS_S16 s16SlotIdx);
static MSGPOOL_Result _MDrv_MSGPOOL_RegisterSlots(MS_U16 u16RegisteSlotNum);
static MSGPOOL_Result _MDrv_MSGPOOL_UnRegisterSlots(MS_U16 u16RegisteSlotNum);

//=============================================================================
// Mailbox Driver MSG Q Function
static void _MDrv_MSGQ_FreeMSG(MS_S16 s16MsgQIdx);

//=============================================================================
// Mailbox Driver MSG POOL Function

//-------------------------------------------------------------------------------------------------
/// Free mail message items in message queue
/// @param  s16MsgFirst                  \b IN: The first message item slot idx for msg q which need free
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_MSGPOOL_FreeMSGPoolItemQ(MS_S16 s16MsgFirst)
{
    MS_S16 s16MsgNext = s16MsgFirst;
    while(IS_VALID_PTR(s16MsgNext))
    {
        s16MsgFirst = s16MsgNext;
	 s16MsgNext = _MDrv_MSGPOOL_GetNextSlot(s16MsgFirst);

	 _MDrv_MSGPOOL_FreeSlot(s16MsgFirst);
    }
}

//-------------------------------------------------------------------------------------------------
/// set mail message to mail message item
/// @param  s16SlotIdx                  \b IN: The  message item slot idx for setting msg
/// @param  pMbxMsg                  \b IN: The  message to set
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_MSGPOOL_SetMsgToPoolItem(MS_S16 s16SlotIdx, MBX_Msg* pMbxMsg)
{
    _infoMSGP.pMsgPool[s16SlotIdx].mbxMsg = *pMbxMsg;
}

//-------------------------------------------------------------------------------------------------
/// get mail message from mail message item
/// @param  s16SlotIdx                  \b IN: The  message item slot idx for getting msg
/// @param  pMbxMsg                  \b OUT: the output msg
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_MSGPOOL_GetMsgFromPoolItem(MS_S16 s16SlotIdx, MBX_Msg* pMbxMsg)
{
    *pMbxMsg = _infoMSGP.pMsgPool[s16SlotIdx].mbxMsg;
}

//-------------------------------------------------------------------------------------------------
/// allocate a free slot for message
/// @param  s16SlotIdx                  \b OUT: The Allocated slot idx
/// @return E_MSGPOOL_ERR_NO_MORE_MEMORY: no more free slots
/// @return E_MSGPOOL_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result _MDrv_MSGPOOL_AllocateSlot(MS_S16* s16SlotIdx)
{
    MS_S16 s16Slot;
    volatile MSGPOOL_MsgPoolItem * pMsgPoolItem = _infoMSGP.pMsgPool;


    if(_infoMSGP.u16FreeSlots<=0)
    {
        return E_MSGPOOL_ERR_NO_MORE_MEMORY;
    }

    for(s16Slot=0; s16Slot<_infoMSGP.u16Slots; s16Slot++)
    {
        if(pMsgPoolItem[s16Slot].u16Usage == FALSE )
        {
            pMsgPoolItem[s16Slot].u16Usage = TRUE;
            pMsgPoolItem[s16Slot].s16Next = INVALID_PTR;
	     _infoMSGP.u16FreeSlots--;
            *s16SlotIdx = s16Slot;
            break;
        }

    }

    return  E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// free a slot for message
/// @param  s16SlotIdx                  \b IN: The free slot idx
/// @return E_MSGPOOL_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result _MDrv_MSGPOOL_FreeSlot(MS_S16 s16SlotIdx)
{
    volatile MSGPOOL_MsgPoolItem * pMsgPoolItem = _infoMSGP.pMsgPool;

    //MSGPOOL_ASSERT((IS_VALID_PTR(s16SlotIdx)), "The slot idx for free is invliad!\n");
    //MSGPOOL_ASSERT((pMsgPoolItem[s16SlotIdx].u16Usage==TRUE), "The slot for free is not used!\n");
    if(!IS_VALID_PTR(s16SlotIdx))
    {
        printk("[MailBox (Driver)]The slot idx for free is invliad!\n");
        return E_MSGPOOL_UNKNOW_ERROR;
    }
    if(pMsgPoolItem[s16SlotIdx].u16Usage==FALSE)
    {
        printk("[MailBox (Driver)]The slot for free is not used!\n");
        return E_MSGPOOL_UNKNOW_ERROR;
    }

    pMsgPoolItem[s16SlotIdx].u16Usage = FALSE;
    pMsgPoolItem[s16SlotIdx].s16Next = INVALID_PTR;
    _infoMSGP.u16FreeSlots++;

    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// Get Next Slot Idx from input slot idx;
/// @param  s16SlotIdx                  \b IN: The slot idx for get next
/// @return MS_S16:the next slot idx of input idx;
/// @attention
/// <b>[OBAMA] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_S16 _MDrv_MSGPOOL_GetNextSlot(MS_S16 s16SlotIdx)
{
    //MSGPOOL_ASSERT((IS_VALID_PTR(s16SlotIdx)), "The slot idx is invliad! %x\n", s16SlotIdx);
    if(!IS_VALID_PTR(s16SlotIdx))
    {
        printk("[MailBox (Driver)]The slot idx is invliad! %x\n", s16SlotIdx);
        return INVALID_PTR;
    }
    return _infoMSGP.pMsgPool[s16SlotIdx].s16Next;
}

//-------------------------------------------------------------------------------------------------
/// set Next Slot Idx to input slot idx;
/// @param  s16PreSlotIdx                  \b IN: The slot idx for set next
/// @param  s16NextSlotIdx                  \b IN: The next slot idx for set
/// @return void
/// @attention
/// <b>[OBAMA] <em> </em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_MSGPOOL_SetNextSlot(MS_S16 s16PreSlotIdx, MS_S16 s16NextSlotIdx)
{
    _infoMSGP.pMsgPool[s16PreSlotIdx].s16Next = s16NextSlotIdx;
}

//-------------------------------------------------------------------------------------------------
/// register a number of slots from msg pool
/// @param  u16RegisteSlotNum                  \b IN: The register slot number
/// @return E_MSGPOOL_ERR_NO_MORE_MEMORY: no more free slots for register
/// @return E_MSGPOOL_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result _MDrv_MSGPOOL_RegisterSlots(MS_U16 u16RegisteSlotNum)
{
    if((_infoMSGP.u16RegistedSlots+u16RegisteSlotNum) > _infoMSGP.u16Slots) //no enough slots for registe
    {
        return E_MSGPOOL_ERR_NO_MORE_MEMORY;
    }

    //have memory for register:
    _infoMSGP.u16RegistedSlots += u16RegisteSlotNum;

    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// un-register a number of slots from msg pool
/// @param  u16UnRegisteSlotNum                  \b IN: The un register slot number
/// @return E_MSGPOOL_ERR_NO_MORE_MEMORY: no more free slots for register
/// @return E_MSGPOOL_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result _MDrv_MSGPOOL_UnRegisterSlots(MS_U16 u16UnRegisteSlotNum)
{
    if(_infoMSGP.u16RegistedSlots < u16UnRegisteSlotNum)
    {
        return E_MSGPOOL_ERR_INVALID_PARAM;
    }

    _infoMSGP.u16RegistedSlots -= u16UnRegisteSlotNum;

    return E_MSGPOOL_SUCCESS;
}

//=============================================================================
// Mailbox Driver MSG Q Function

//-------------------------------------------------------------------------------------------------
/// Free mail message items in message queue
/// @param  s16MsgFirst                  \b IN: The first message item slot idx for msg q which need free
/// @return MS_S16:INVALID_PTR: no more async notifier could be allocated
/// @return MS_S16:Async Notifer ID: between 0-MBX_ASYNCNOTIFIER_MAX
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_MSGQ_FreeMSG(MS_S16 s16MsgQIdx)
{
    _MDrv_MSGPOOL_FreeMSGPoolItemQ(_msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst);
    _MDrv_MSGPOOL_FreeMSGPoolItemQ(_msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst);
    _MDrv_MSGPOOL_UnRegisterSlots(_msgQMgrMSGQ[s16MsgQIdx].u16MsgQSize);

    _msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst = _msgQMgrMSGQ[s16MsgQIdx].s16MsgEnd
		= _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst = _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgEnd = INVALID_PTR;

    _msgQMgrMSGQ[s16MsgQIdx].u16MsgNum = _msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum = 0;

    _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_INVALID;
    _msgQMgrMSGQ[s16MsgQIdx].u16MsgQSize = 0;

    _msgQMgrMSGQ[s16MsgQIdx].s16MsgQNotifierID = INVALID_PTR;
    _msgQMgrMSGQ[s16MsgQIdx].s16NextMsgQ = INVALID_PTR;
}

//-------------------------------------------------------------------------------------------------
/// Clear mail message items in message queue
/// @param  s16MsgQIdx                  \b IN: The first message item slot idx for msg q which need free
/// @attention
/// <b>[MXLIB] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MDrv_MSGQ_ClearMSG(MS_S16 s16MsgQIdx)
{
    _MDrv_MSGPOOL_FreeMSGPoolItemQ(_msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst);
    _MDrv_MSGPOOL_FreeMSGPoolItemQ(_msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst);

    _msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst = _msgQMgrMSGQ[s16MsgQIdx].s16MsgEnd
		= _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst = _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgEnd = INVALID_PTR;

    _msgQMgrMSGQ[s16MsgQIdx].u16MsgNum = _msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum = 0;

    _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_EMPTY;
}

//----------------------------------------------------------------------------
//=============================================================================
// Mailbox Driver MSG Pool Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// Init the message Pool
/// @return E_MSGPOOL_ERR_NO_MORE_MEMORY:allocate pool failed
/// @return E_MSGPOOL_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGPOOL_Init()
{
    //MS_U32 u32PageIdx;
    //struct page *pPage;
    _infoMSGP.pMsgPool = (MSGPOOL_MsgPoolItem * )__get_free_pages(GFP_KERNEL, get_order(MBX_MSGPOOL_PAGES*(0x01<<PAGE_SHIFT)));

    if(_infoMSGP.pMsgPool == NULL)
    {
        return E_MSGPOOL_ERR_NO_MORE_MEMORY;
    }

    _infoMSGP.u16Slots = (MBX_MSGPOOL_PAGES*(0x01<<PAGE_SHIFT))/sizeof(MSGPOOL_MsgPoolItem);
    _infoMSGP.u16FreeSlots = _infoMSGP.u16Slots;
    _infoMSGP.u16RegistedSlots = 0;

    memset((void*)_infoMSGP.pMsgPool, 0x0, (MBX_MSGPOOL_PAGES*(0x01<<PAGE_SHIFT)));

    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// DeInit the message Pool
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
void MDrv_MSGPOOL_DeInit()
{
    free_pages((U32)_infoMSGP.pMsgPool, get_order(MBX_MSGPOOL_PAGES*(0x01<<PAGE_SHIFT)));
}


//----------------------------------------------------------------------------
//=============================================================================
// Mailbox Driver MSG Q Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// Init the message Q
/// @return E_MSGPOOL_SUCCESS:success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_Init()
{
    MS_U16 u16MsgQIdx;

    for(u16MsgQIdx = 0; u16MsgQIdx < MSGPOOL_MSGQ_MAX; u16MsgQIdx++)
    {
        _msgQMgrMSGQ[u16MsgQIdx].s16MsgFirst = _msgQMgrMSGQ[u16MsgQIdx].s16MsgEnd
		    = _msgQMgrMSGQ[u16MsgQIdx].s16InstantMsgFirst = _msgQMgrMSGQ[u16MsgQIdx].s16InstantMsgEnd = INVALID_PTR;

        _msgQMgrMSGQ[u16MsgQIdx].u16MsgNum = _msgQMgrMSGQ[u16MsgQIdx].u16InstantMsgNum = 0;

        _msgQMgrMSGQ[u16MsgQIdx].u16MsgQStatus = E_MSGQ_INVALID;
        _msgQMgrMSGQ[u16MsgQIdx].u16MsgQSize = 0;

        _msgQMgrMSGQ[u16MsgQIdx].s16MsgQNotifierID = INVALID_PTR;
        _msgQMgrMSGQ[u16MsgQIdx].s16NextMsgQ = INVALID_PTR;
    }

    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// Register message class
/// @param  s16MsgQNotifierID                  \b IN: The Async Notifier ID with the register class
/// @param  s16MsgQFirst                  \b IN: The first msg Q ID of the Async. Notifer
/// @param  s16MsgQID                  \b IN: The Register Msg Class ID
/// @param  u16MsgQSize                  \b IN: The size of Msg Queue with register class
/// @return E_MSGPOOL_ERR_SLOT_AREADY_OPENNED:the class already registered by this Async. Notifier
/// @return E_MSGPOOL_ERR_SLOT_BUSY:the class already registered by another Async. Notifier
/// @return E_MSGPOOL_ERR_NO_MORE_MEMORY:no request msg q size avaiable
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_RegisterMSG(MS_S16 s16MsgQNotifierID, MS_S16 s16MsgQFirst, MS_S16 s16MsgQID, MS_U16 u16MsgQSize)
{
    if(_msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID == s16MsgQNotifierID) //already registed by this app
    {
        return E_MSGPOOL_ERR_SLOT_AREADY_OPENNED;
    }

    if(_msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID != INVALID_PTR) //not the requester, but also not invalid, means others has register the msg class.
    {
        return E_MSGPOOL_ERR_SLOT_BUSY;
    }

    if(E_MSGPOOL_ERR_NO_MORE_MEMORY == _MDrv_MSGPOOL_RegisterSlots(u16MsgQSize))
    {
        return E_MSGPOOL_ERR_NO_MORE_MEMORY;
    }

    //success registed, then allocate the msg q:
    _msgQMgrMSGQ[s16MsgQID].s16MsgFirst = _msgQMgrMSGQ[s16MsgQID].s16MsgEnd
		= _msgQMgrMSGQ[s16MsgQID].s16InstantMsgFirst = _msgQMgrMSGQ[s16MsgQID].s16InstantMsgEnd = INVALID_PTR;

    _msgQMgrMSGQ[s16MsgQID].u16MsgNum = _msgQMgrMSGQ[s16MsgQID].u16InstantMsgNum = 0;

    _msgQMgrMSGQ[s16MsgQID].u16MsgQStatus = E_MSGQ_EMPTY;
    _msgQMgrMSGQ[s16MsgQID].u16MsgQSize = u16MsgQSize;

    _msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID = s16MsgQNotifierID;
    _msgQMgrMSGQ[s16MsgQID].s16NextMsgQ = s16MsgQFirst;

    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// Register message class
/// @param  s16MsgQNotifierID                  \b IN: The Async Notifier ID with the register class
/// @param  s16MsgQFirst                  \b IN: The first msg Q ID of the Async. Notifer
/// @param  s16MsgQID                  \b IN: The Register Msg Class ID
/// @param  bForceDiscardMsgQueue                  \b IN: If TRUE, un-register whatever the status of msg queue
/// @return E_MSGPOOL_ERR_SLOT_NOT_OPENNED:the class have not been registered yet
/// @return E_MSGPOOL_ERR_SLOT_BUSY:the class already registered by another Async. Notifier
/// @return E_MSGPOOL_ERR_HAS_MSG_PENDING: has pendign msg in msg queue for un-register
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_UnRegisterMSG(MS_S16 s16MsgQNotifierID,  MS_S16* ps16MsgQFirst, MS_S16 s16MsgQID, MS_BOOL bForceDiscardMsgQueue)
{
    MS_S16 s16MsgQIdx = *ps16MsgQFirst;

    //if not opened yet?
    if(!IS_VALID_PTR(_msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID) || (_msgQMgrMSGQ[s16MsgQID].u16MsgQStatus== E_MSGQ_INVALID) )
    {
        return E_MSGPOOL_ERR_SLOT_NOT_OPENNED;
    }

    //if used by another application?
    if(_msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID != s16MsgQNotifierID)
    {
        return E_MSGPOOL_ERR_SLOT_BUSY;
    }

    //if has pending msg and app not force discard:
    if((!bForceDiscardMsgQueue) && ((_msgQMgrMSGQ[s16MsgQID].u16InstantMsgNum+_msgQMgrMSGQ[s16MsgQID].u16MsgNum) >  0))
    {
        return E_MSGPOOL_ERR_HAS_MSG_PENDING;
    }

    //free msgq and discard msg if has pending:
    //MSGPOOL_ASSERT(IS_VALID_PTR(*ps16MsgQFirst), "Invlid first MSGQ Ptr for un-register!\n");
    if(!IS_VALID_PTR(*ps16MsgQFirst))
    {
        printk("[MailBox (Driver)]Invlid first MSGQ Ptr for un-register!\n");
        return E_MSGPOOL_UNKNOW_ERROR;
    }

    if(s16MsgQIdx != s16MsgQID)
    {
        while(_msgQMgrMSGQ[s16MsgQIdx].s16NextMsgQ != s16MsgQID)
        {
            s16MsgQIdx = _msgQMgrMSGQ[s16MsgQIdx].s16NextMsgQ;
        }

	 _msgQMgrMSGQ[s16MsgQIdx].s16NextMsgQ = _msgQMgrMSGQ[s16MsgQID].s16NextMsgQ;
    }
    else
    {
        *ps16MsgQFirst = _msgQMgrMSGQ[s16MsgQID].s16NextMsgQ;
    }

    _MDrv_MSGQ_FreeMSG(s16MsgQID);

    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// Clear message class
/// @param  s16MsgQID                  \b IN: The Clear Msg Class ID
/// @return E_MSGPOOL_ERR_SLOT_NOT_OPENNED:the class have not been registered yet
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[MXLIB] <em></em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_ClearMSG(MS_S16 s16MsgQNotifierID, MS_S16 s16MsgQID)
{
    //if not opened yet?
    if(!IS_VALID_PTR(_msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID) || (_msgQMgrMSGQ[s16MsgQID].u16MsgQStatus== E_MSGQ_INVALID) )
    {
        return E_MSGPOOL_ERR_SLOT_NOT_OPENNED;
    }

    //if used by another application?
    if(_msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID != s16MsgQNotifierID)
    {
        return E_MSGPOOL_ERR_SLOT_BUSY;
    }

    //Clear msgq:
    _MDrv_MSGQ_ClearMSG(s16MsgQID);

    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// get status of message class
/// @param  s16MsgQNotifierID                  \b IN: The Async Notifier ID with the msg class to get info.
/// @param  s16MsgQID                  \b IN: The Msg Class ID to get info.
/// @param  pMsgQStatus                  \b OUT: the msg class info to put
/// @return E_MSGPOOL_ERR_SLOT_NOT_OPENNED:the class have not been registered yet
/// @return E_MSGPOOL_ERR_SLOT_BUSY:the class already registered by another Async. Notifier
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_GetMsgQStatus(MS_S16 s16MsgQNotifierID, MS_S16 s16MsgQID, MBX_MSGQ_Status *pMsgQStatus)
{
    if(!IS_VALID_PTR(_msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID) || (_msgQMgrMSGQ[s16MsgQID].u16MsgQStatus== E_MSGQ_INVALID)) //already registed by this app
    {
        return E_MSGPOOL_ERR_SLOT_NOT_OPENNED;
    }

    //if used by another application?
    if(_msgQMgrMSGQ[s16MsgQID].s16MsgQNotifierID != s16MsgQNotifierID)
    {
        return E_MSGPOOL_ERR_SLOT_BUSY;
    }

    pMsgQStatus->status = 0;

    if(_msgQMgrMSGQ[s16MsgQID].u16MsgQStatus == E_MSGQ_OVERFLOW)
    {
        pMsgQStatus->status |= MBX_STATUS_QUEUE_OVER_FLOW;
    }

    if(_msgQMgrMSGQ[s16MsgQID].u16MsgNum > 0)
    {
        pMsgQStatus->status |= MBX_STATUS_QUEUE_HAS_NORMAL_MSG;
    }

    if(_msgQMgrMSGQ[s16MsgQID].u16InstantMsgNum > 0)
    {
        pMsgQStatus->status |= MBX_STATUS_QUEUE_HAS_INSTANT_MSG;
    }

    pMsgQStatus->u32InstantMsgCount = _msgQMgrMSGQ[s16MsgQID].u16InstantMsgNum;
    pMsgQStatus->u32NormalMsgCount = _msgQMgrMSGQ[s16MsgQID].u16MsgNum;
    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// uNRegister message class queue
/// @param  s16MsgQFirst                  \b IN: The first msg Q ID for free
/// @param  bForceDiscardMsgQueue                  \b IN: If TRUE, un-register whatever the status of msg queue
/// @return E_MSGPOOL_ERR_HAS_MSG_PENDING: has pendign msg in msg queue for un-register
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_UnRegisterMSGQ(MS_S16 s16MsgQFirst, MS_BOOL bForceDiscardPendingMsg)
{
    MS_S16 s16MsgQNext = s16MsgQFirst;

    if(!bForceDiscardPendingMsg)
    {
        while(IS_VALID_PTR(s16MsgQNext))
        {
            if((_msgQMgrMSGQ[s16MsgQNext].u16InstantMsgNum+_msgQMgrMSGQ[s16MsgQNext].u16MsgNum) >  0)
            {
                return E_MSGPOOL_ERR_HAS_MSG_PENDING;
            }

            s16MsgQNext = _msgQMgrMSGQ[s16MsgQFirst].s16NextMsgQ;
        }
    }

    s16MsgQNext = s16MsgQFirst;

    while(IS_VALID_PTR(s16MsgQNext))
    {
        s16MsgQFirst = s16MsgQNext;
        s16MsgQNext = _msgQMgrMSGQ[s16MsgQFirst].s16NextMsgQ;

        _MDrv_MSGQ_FreeMSG(s16MsgQFirst);
    }

    return E_MSGPOOL_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// recv message from msg class queue
/// @param  s16MsgQIdx                  \b IN: The  msg Q ID for recv
/// @param  pMsg                  \b OUT: the msg to put
/// @param  bInstantMsg                  \b IN: get the instant msg or normal msg
/// @return E_MSGPOOL_ERR_NO_MORE_MSG: no msg for recv
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_RecvMsg(MS_S16 s16MsgQIdx, MBX_Msg* pMsg, MS_BOOL bInstantMsg)
{
    MS_S16 s16MsgSlotIdx;
    MSGPOOL_Result msgPoolResult;

    if(bInstantMsg )
    {//get instant msg:
        if(_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum>0)
        {
            s16MsgSlotIdx = _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst;
            _MDrv_MSGPOOL_GetMsgFromPoolItem(s16MsgSlotIdx, pMsg);
            _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst = _MDrv_MSGPOOL_GetNextSlot(s16MsgSlotIdx);
            _MDrv_MSGPOOL_FreeSlot(s16MsgSlotIdx);
            _msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum--;
            msgPoolResult = E_MSGPOOL_SUCCESS;
        }
        else
        {
            msgPoolResult = E_MSGPOOL_ERR_NO_MORE_MSG;
        }
    }
    else
    {//get normal msg:
        if(_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum>0)
        {
            s16MsgSlotIdx = _msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst;
            _MDrv_MSGPOOL_GetMsgFromPoolItem(s16MsgSlotIdx, pMsg);
            _msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst = _MDrv_MSGPOOL_GetNextSlot(s16MsgSlotIdx);
            _MDrv_MSGPOOL_FreeSlot(s16MsgSlotIdx);
            _msgQMgrMSGQ[s16MsgQIdx].u16MsgNum--;

            msgPoolResult = E_MSGPOOL_SUCCESS;
        }
        else
        {
            msgPoolResult = E_MSGPOOL_ERR_NO_MORE_MSG;
        }
    }

    if((_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum+_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum)<_msgQMgrMSGQ[s16MsgQIdx].u16MsgQSize)
    {
        _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_NORMAL;
    }

    if((_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum+_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum)<=0)
    {
        _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_EMPTY;
    }

    return msgPoolResult;
}


//-------------------------------------------------------------------------------------------------
/// Check message from msg class queue
/// @param  s16MsgQIdx                  \b IN: The  msg Q ID for check
/// @param  pMsg                  \b OUT: the msg to put
/// @param  bInstantMsg                  \b IN: get the instant msg or normal msg
/// @return E_MSGPOOL_ERR_NO_MORE_MSG: no msg for recv
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
/// <b>[OBAMA] <em>This pMsg contains no message content, only MsgClass available for checking</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_CheckMsg(MS_S16 s16MsgQIdx, MBX_Msg* pMsg, MS_BOOL bInstantMsg)
{
    MS_S16 s16MsgSlotIdx;
    MSGPOOL_Result msgPoolResult;

    if(bInstantMsg )
    {//check instant msg:
        if(_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum>0)
        {
            s16MsgSlotIdx = _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst;
            _MDrv_MSGPOOL_GetMsgFromPoolItem(s16MsgSlotIdx, pMsg);

            _infoLatestMSGP.bInstantMsg=bInstantMsg;
            _infoLatestMSGP.s16MsgQIdx=s16MsgQIdx;
            _infoLatestMSGP.s16MsgSlotIdx=s16MsgSlotIdx;

            msgPoolResult = E_MSGPOOL_SUCCESS;
        }
        else
        {
            msgPoolResult = E_MSGPOOL_ERR_NO_MORE_MSG;
        }
    }
    else
    {//check normal msg:
        if(_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum>0)
        {
            s16MsgSlotIdx = _msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst;
            _MDrv_MSGPOOL_GetMsgFromPoolItem(s16MsgSlotIdx, pMsg);

            _infoLatestMSGP.bInstantMsg=bInstantMsg;
            _infoLatestMSGP.s16MsgQIdx=s16MsgQIdx;
            _infoLatestMSGP.s16MsgSlotIdx=s16MsgSlotIdx;

            msgPoolResult = E_MSGPOOL_SUCCESS;
        }
        else
        {
            msgPoolResult = E_MSGPOOL_ERR_NO_MORE_MSG;
        }
    }

    if((_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum+_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum)<_msgQMgrMSGQ[s16MsgQIdx].u16MsgQSize)
    {
        _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_NORMAL;
    }

    if((_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum+_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum)<=0)
    {
        _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_EMPTY;
    }

    return msgPoolResult;
}

//-------------------------------------------------------------------------------------------------
/// remove latest message from msg class queue
/// @return E_MSGPOOL_ERR_NO_MORE_MSG: no msg for recv
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_RemoveLatestMsg(void)
{
    MS_S16 s16MsgSlotIdx, s16MsgQIdx;
    MS_BOOL bInstantMsg;
    MSGPOOL_Result msgPoolResult;

    s16MsgQIdx=_infoLatestMSGP.s16MsgQIdx;
    bInstantMsg=_infoLatestMSGP.bInstantMsg;
    s16MsgSlotIdx=_infoLatestMSGP.s16MsgSlotIdx;
    if(bInstantMsg )
    {//check instant msg:
        if(_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum>0)
        {
            _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst = _MDrv_MSGPOOL_GetNextSlot(s16MsgSlotIdx);
            _MDrv_MSGPOOL_FreeSlot(s16MsgSlotIdx);
            _msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum--;


            msgPoolResult = E_MSGPOOL_SUCCESS;
        }
        else
        {
            msgPoolResult = E_MSGPOOL_ERR_NO_MORE_MSG;
        }
    }
    else
    {//check normal msg:
        if(_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum>0)
        {
            _msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst = _MDrv_MSGPOOL_GetNextSlot(s16MsgSlotIdx);
            _MDrv_MSGPOOL_FreeSlot(s16MsgSlotIdx);
            _msgQMgrMSGQ[s16MsgQIdx].u16MsgNum--;

            msgPoolResult = E_MSGPOOL_SUCCESS;
        }
        else
        {
            msgPoolResult = E_MSGPOOL_ERR_NO_MORE_MSG;
        }
    }

    if((_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum+_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum)<_msgQMgrMSGQ[s16MsgQIdx].u16MsgQSize)
    {
        _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_NORMAL;
    }

    if((_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum+_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum)<=0)
    {
        _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_EMPTY;
    }

    return msgPoolResult;
}


//-------------------------------------------------------------------------------------------------
/// Get Next msg q Idx from input msg q idx;
/// @param  s16SlotIdx                  \b IN: The q idx for get next
/// @return MS_S16:the next msg q idx of input idx;
/// @attention
/// <b>[OBAMA] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_S16 MDrv_MSGQ_GetNextMsgQ(MS_S16 s16MsgQIdx)
{
    return _msgQMgrMSGQ[s16MsgQIdx].s16NextMsgQ;
}

//-------------------------------------------------------------------------------------------------
/// add message to msg class queue
/// @param  pMsg                  \b IN: the msg to put
/// @return E_MSGPOOL_ERR_NOT_INITIALIZED: no msg not actived yet
/// @return E_MSGPOOL_ERR_NO_MORE_MSG: no msg memory to add
/// @return E_MSGPOOL_SUCCESS: success
/// @attention
/// <b>[OBAMA] <em>Use spin lock to protect co-access</em></b>
//-------------------------------------------------------------------------------------------------
MSGPOOL_Result MDrv_MSGQ_AddMSG(MBX_Msg* pMbxMsg)
{
    MS_S16 s16MsgItemIdx = -1;
    MS_S16 s16MsgQIdx = pMbxMsg->u8MsgClass;

    if((_msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus == E_MSGQ_INVALID))
    {
        return E_MSGPOOL_ERR_NOT_INITIALIZED;
    }

    if(_msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus == E_MSGQ_OVERFLOW)
    {
        return E_MSGPOOL_ERR_NO_MORE_MEMORY;
    }

    if(_MDrv_MSGPOOL_AllocateSlot(&s16MsgItemIdx) == E_MSGPOOL_SUCCESS)
    {
        _MDrv_MSGPOOL_SetMsgToPoolItem(s16MsgItemIdx, pMbxMsg);
        if(pMbxMsg->eMsgType == E_MBX_MSG_TYPE_INSTANT)
        {
            if(_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum > 0)
            {
                _MDrv_MSGPOOL_SetNextSlot(_msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgEnd,s16MsgItemIdx);
                _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgEnd = s16MsgItemIdx;
            }
            else
            {
                _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgFirst = _msgQMgrMSGQ[s16MsgQIdx].s16InstantMsgEnd = s16MsgItemIdx;
            }

            _msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum++;
        }
        else
        {
            if(_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum > 0)
            {
                _MDrv_MSGPOOL_SetNextSlot(_msgQMgrMSGQ[s16MsgQIdx].s16MsgEnd,s16MsgItemIdx);
                _msgQMgrMSGQ[s16MsgQIdx].s16MsgEnd = s16MsgItemIdx;
            }
            else
            {
                _msgQMgrMSGQ[s16MsgQIdx].s16MsgFirst= _msgQMgrMSGQ[s16MsgQIdx].s16MsgEnd = s16MsgItemIdx;
            }

            _msgQMgrMSGQ[s16MsgQIdx].u16MsgNum++;
        }

        if(_msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus == E_MSGQ_EMPTY)
        {
            _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_NORMAL;
        }

        if((_msgQMgrMSGQ[s16MsgQIdx].u16MsgNum+_msgQMgrMSGQ[s16MsgQIdx].u16InstantMsgNum)>=_msgQMgrMSGQ[s16MsgQIdx].u16MsgQSize)
        {
            _msgQMgrMSGQ[s16MsgQIdx].u16MsgQStatus = E_MSGQ_OVERFLOW;
        }


        return E_MSGPOOL_SUCCESS;
    }

    return E_MSGPOOL_ERR_NO_MORE_MEMORY;
}

//-------------------------------------------------------------------------------------------------
/// get NotifierID by msg Q ID
/// @param  s16MsgQIdx                  \b IN: the msg q id
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MS_S16 MDrv_MSGQ_GetNotiferIDByQID(MS_S16 s16MsgQIdx)
{
    return _msgQMgrMSGQ[s16MsgQIdx].s16MsgQNotifierID;
}

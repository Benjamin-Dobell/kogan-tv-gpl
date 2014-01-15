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
/// @file   mdrv_mbx_msgpool.h
/// @brief  MStar Mailbox Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em>legacy interface is only used by MStar proprietary Mail Message communication\n
/// It's API level for backward compatible and will be remove in the next version.\n
/// Please refer @ref drvGE.h for future compatibility.</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_MBX_MSGPOOL_H
#define _DRV_MBX_MSGPOOL_H

#ifdef _DRV_MBX__MSGPOOL_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines & Macros
//=============================================================================
/// MSG Pool return value
typedef MBX_Result MSGPOOL_Result;

#define E_MSGPOOL_SUCCESS                                         E_MBX_SUCCESS
#define E_MSGPOOL_ERR_NOT_INITIALIZED                   E_MBX_ERR_NOT_INITIALIZED
#define E_MSGPOOL_ERR_NO_MORE_MEMORY                  E_MBX_ERR_NO_MORE_MEMORY
#define E_MSGPOOL_ERR_SLOT_BUSY                              E_MBX_ERR_SLOT_BUSY
#define E_MSGPOOL_ERR_SLOT_AREADY_OPENNED         E_MBX_ERR_SLOT_AREADY_OPENNED
#define E_MSGPOOL_ERR_SLOT_NOT_OPENNED               E_MBX_ERR_SLOT_NOT_OPENNED
#define E_MSGPOOL_ERR_INVALID_PARAM                      E_MBX_ERR_INVALID_PARAM
#define E_MSGPOOL_ERR_HAS_MSG_PENDING                 E_MBX_ERR_HAS_MSG_PENDING
#define E_MSGPOOL_ERR_NO_MORE_MSG                         E_MBX_ERR_NO_MORE_MSG
#define E_MSGPOOL_ERR_NOT_IMPLEMENTED                  E_MBX_ERR_NOT_IMPLEMENTED
#define E_MSGPOOL_UNKNOW_ERROR                               E_MBX_UNKNOW_ERROR

//=============================================================================
// Type and Structure Declaration
//=============================================================================
typedef struct
{
    MBX_Msg mbxMsg; //the msg in slot.
    MS_S16 s16Next;  //next item in pool;
    MS_U16 u16Usage; //the slot is used or free;
}MSGPOOL_MsgPoolItem; //sizeof(MSGPOOL_MsgPoolItem) == 28 bytes

typedef struct
{
    volatile MSGPOOL_MsgPoolItem * pMsgPool; //pointer to the reserved msg pool
    MS_U16 u16Slots; //the total number of MsgPoolItem in the MsgPool
    MS_U16 u16FreeSlots; //the free number of MsgPoolItem in the MsgPool
    MS_U16 u16RegistedSlots; //the registed(resevered for registed msg class) number of MsgPoolItem in the MsgPool.
}MSGPOOL_MsgPoolInfo;

typedef struct
{
    MS_BOOL bInstantMsg; //the latest received message's type
    MS_S16 s16MsgQIdx; //the latest received message's QIdx
    MS_S16 s16MsgSlotIdx; //the latest received message's SlotIdx
}MSGPOOL_LastRecvMsgInfo;

typedef enum
{
    E_MSGQ_INVALID    = 0x00,
    E_MSGQ_NORMAL     = 0x01,
    E_MSGQ_EMPTY      = 0x02,
    E_MSGQ_OVERFLOW   = 0x03
} MSGPOOL_MsgQStatus;

typedef struct
{
    MS_S16 s16MsgFirst; //the first MsgPoolItem slot idx in MsgPool
    MS_S16 s16MsgEnd; //the end MsgPoolItem slot idx in MsgPool

    MS_S16 s16InstantMsgFirst; //the first instant MsgPoolItem slot idx in MsgPool
    MS_S16 s16InstantMsgEnd; //the end instant MsgPoolItem slot idx in MsgPool

    MS_U16 u16MsgNum; //the number of MsgPoolItem in MsgQ;
    MS_U16 u16InstantMsgNum; //the number of instant MsgPoolItem in MsgQ;

    MS_U16 u16MsgQStatus; //the MsgQ Status: invalid; normal, empty; voerflow;
    MS_U16 u16MsgQSize; //the MsgQ size when the msg class is registed; should be no larger than MAX_MBX_QUEUE_SIZE

    MS_S16 s16MsgQNotifierID; //the MsgQ Register in MBX Driver
    MS_S16 s16NextMsgQ; //the Next MsgQ which belongs to the same Register;
}MSGPOOL_MsgQMgr; //message queue manager, per class per queue.

////////////////////////////////////////////////////////////////////////////////////////////
// interface to control the Msg Pool:
INTERFACE MSGPOOL_Result MDrv_MSGPOOL_Init(void);
INTERFACE void MDrv_MSGPOOL_DeInit(void);

////////////////////////////////////////////////////////////////////////////////////////////
// interface to control the Msg Queue:
INTERFACE MSGPOOL_Result MDrv_MSGQ_Init(void);

INTERFACE MSGPOOL_Result MDrv_MSGQ_RegisterMSG(MS_S16 s16MsgQNotifierID, MS_S16 s16MsgQFirst, MS_S16 s16MsgQID, MS_U16 u16MsgQSize);
INTERFACE MSGPOOL_Result MDrv_MSGQ_UnRegisterMSG(MS_S16 s16MsgQNotifierID, MS_S16* ps16MsgQFirst, MS_S16 s16MsgQID, MS_BOOL bForceDiscardMsgQueue);
INTERFACE MSGPOOL_Result MDrv_MSGQ_ClearMSG(MS_S16 s16MsgQNotifierID, MS_S16 s16MsgQID);
INTERFACE MSGPOOL_Result MDrv_MSGQ_UnRegisterMSGQ(MS_S16 s16MsgQFirst, MS_BOOL bForceDiscardPendingMsg);
INTERFACE MSGPOOL_Result MDrv_MSGQ_GetMsgQStatus(MS_S16 s16MsgQNotifierID, MS_S16 s16MsgQID, MBX_MSGQ_Status *pMsgQStatus);
INTERFACE MSGPOOL_Result MDrv_MSGQ_RecvMsg(MS_S16 s16MsgQIdx, MBX_Msg* pMsg, MS_BOOL bInstantMsg);
INTERFACE MSGPOOL_Result MDrv_MSGQ_CheckMsg(MS_S16 s16MsgQIdx, MBX_Msg* pMsg, MS_BOOL bInstantMsg);
INTERFACE MSGPOOL_Result MDrv_MSGQ_RemoveLatestMsg(void);
INTERFACE MS_S16 MDrv_MSGQ_GetNextMsgQ(MS_S16 s16MsgQIdx);
INTERFACE MSGPOOL_Result MDrv_MSGQ_AddMSG(MBX_Msg* pMbxMsg);
INTERFACE MS_S16 MDrv_MSGQ_GetNotiferIDByQID(MS_S16 s16MsgQIdx);

#undef INTERFACE
#endif //_DRV_MBX_MSGPOOL_H


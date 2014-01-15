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
// @file   mdrv_mbx_st.h
// @brief  Mialbox KMD Driver Interface
// @author MStar Semiconductor Inc.
//
// Driver to initialize and access mailbox.
//     - Provide functions to initialize/de-initialize mailbox
//     - Provide mailbox functional access.
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MDRV_MBX_ST_H
#define _MDRV_MBX_ST_H

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Type and Structure Declaration
//=============================================================================
//IO Ctrl struct defines:
typedef struct
{
    MBX_CPU_ID eHKCPU;
    MBX_ROLE_ID eHostRole;
    MS_U32 u32TimeoutMillSecs;
    MBX_Result mbxResult;
}MS_MBX_INIT_INFO, *PMS_MBX_INIT_INFO;

typedef struct
{
    MS_BOOL  bInfo;
    MBX_Result mbxResult;
}MS_MBX_SET_BINFO, *PMS_MBX_SET_BINFO;

typedef struct
{
    MBX_Class eMsgClass;
    MS_U16 	u16MsgQueueSize;
    MBX_Result mbxResult;
}MS_MBX_REGISTER_MSG, *PMS_MBX_REGISTER_MSG;

typedef struct
{
    MBX_Class eMsgClass;
    MS_BOOL bForceDiscardMsgQueue;
    MBX_Result mbxResult;
}MS_MBX_UNREGISTER_MSG, *PMS_MBX_UNREGISTER_MSG;

typedef struct
{
    MBX_Class eMsgClass;
    MBX_Result mbxResult;
}MS_MBX_CLEAR_MSG, *PMS_MBX_CLEAR_MSG;

typedef struct
{
    MBX_Msg *pMsg;
    MBX_Result mbxResult;
}MS_MBX_SEND_MSG, *PMS_MBX_SEND_MSG;

typedef struct
{
    MBX_Class eTargetClass;
    MBX_Msg *pMsg;
    MS_U32 u32WaitMillSecs;
    MS_U32 u32Flag;
    MBX_Result mbxResult;
}MS_MBX_RECV_MSG, *PMS_MBX_RECV_MSG;

typedef struct
{
    MBX_Class eTargetClass;
    MBX_MSGQ_Status *pMsgQueueStatus;
    MBX_Result mbxResult;
}MS_MBX_GET_MSGQSTATUS, *PMS_MBX_GET_MSGQSTATUS;

typedef struct
{
    MS_BOOL bEnabled;
    MS_S32  s32RefCnt;
}MS_MBX_GET_DRVSTATUS, *PMS_MBX_GET_DRVSTATUS;

typedef struct
{
    MBX_ROLE_ID eTargetRole;
    MS_U8 *pU8Info;
    MS_U8 u8Size;
    MBX_Result mbxResult;
}MS_MBX_CPROSYNC_INFORMATION, *PMS_MBX_CPROSYNC_INFORMATION;

#endif //_MDRV_MBX_ST_H

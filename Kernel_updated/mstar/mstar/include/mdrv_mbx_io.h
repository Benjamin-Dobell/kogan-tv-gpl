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
// @file   mdrv_mbx_io.h
// @brief  Mialbox KMD Driver Interface
// @author MStar Semiconductor Inc.
//
// Driver to initialize and access mailbox.
//     - Provide functions to initialize/de-initialize mailbox
//     - Provide mailbox functional access.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MBX_IO_H
#define _MDRV_MBX_IO_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_mbx_st.h"

//=============================================================================
// Defines
//=============================================================================
//IO Ctrl defines:
#define MDRV_MBX_IOC_INIT_NR				0
#define MDRV_MBX_IOC_DEINIT_NR			MDRV_MBX_IOC_INIT_NR+1
#define MDRV_MBX_IOC_REGISTERMSG_NR		MDRV_MBX_IOC_DEINIT_NR+1
#define MDRV_MBX_IOC_UNREGISTERMSG_NR	MDRV_MBX_IOC_REGISTERMSG_NR+1
#define MDRV_MBX_IOC_CLEARMSG_NR         MDRV_MBX_IOC_UNREGISTERMSG_NR+1
#define MDRV_MBX_IOC_SENDMSG_NR			MDRV_MBX_IOC_CLEARMSG_NR+1
#define MDRV_MBX_IOC_SENDMSGLOOPBACK_NR MDRV_MBX_IOC_SENDMSG_NR+1
#define MDRV_MBX_IOC_RECVMSG_NR			MDRV_MBX_IOC_SENDMSGLOOPBACK_NR+1
#define MDRV_MBX_IOC_GETMSGQSTATUS_NR	MDRV_MBX_IOC_RECVMSG_NR+1
#define MDRV_MBX_IOC_MBXENABLE_NR		MDRV_MBX_IOC_GETMSGQSTATUS_NR+1
#define MDRV_MBX_IOC_SETINFORMATION_NR   MDRV_MBX_IOC_MBXENABLE_NR+1
#define MDRV_MBX_IOC_GETINFORMATION_NR   MDRV_MBX_IOC_SETINFORMATION_NR+1
#define MDRV_MBX_IOC_GETDRVSTATUS_NR     MDRV_MBX_IOC_GETINFORMATION_NR+1
#define MDRV_MBX_IOC_CHECKMSG_NR		MDRV_MBX_IOC_GETDRVSTATUS_NR+1
#define MDRV_MBX_IOC_REMOVEMSG_NR		MDRV_MBX_IOC_CHECKMSG_NR+1

#define MDRV_MBX_IOC_MAX_NR				MDRV_MBX_IOC_REMOVEMSG_NR+1

// use 'm' as magic number
#define MDRV_MBX_IOC_MAGIC			  'm'
#define MDRV_MBX_IOC_INIT         	  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_INIT_NR, MS_MBX_INIT_INFO)
#define MDRV_MBX_IOC_DEINIT			  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_DEINIT_NR, MS_MBX_SET_BINFO)
#define MDRV_MBX_IOC_REGISTERMSG		  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_REGISTERMSG_NR, MS_MBX_REGISTER_MSG)
#define MDRV_MBX_IOC_UNREGISTERMSG	  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_UNREGISTERMSG_NR, MS_MBX_UNREGISTER_MSG)
#define MDRV_mBX_IOC_CLEARMSG          _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_CLEARMSG_NR, MS_MBX_CLEAR_MSG)
#define MDRV_MBX_IOC_SENDMSG			  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_SENDMSG_NR, MS_MBX_SEND_MSG)
#define MDRV_MBX_IOC_RECVMSG			  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_RECVMSG_NR, MS_MBX_RECV_MSG)
#define MDRV_MBX_IOC_CHECKMSG           _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_CHECKMSG_NR, MS_MBX_RECV_MSG)
#define MDRV_MBX_IOC_REMOVEMSG           _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_REMOVEMSG_NR, MS_MBX_SET_BINFO)
#define MDRV_MBX_IOC_GETMSGQSTATUS	  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_GETMSGQSTATUS_NR, MS_MBX_GET_MSGQSTATUS)
#define MDRV_MBX_IOC_SENDMSGLOOPBACK   _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_SENDMSGLOOPBACK_NR, MS_MBX_SEND_MSG)
#define MDRV_MBX_IOC_MBXENABLE		  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_MBXENABLE_NR, MS_MBX_SET_BINFO)
#define MDRV_MBX_IOC_SETINFORMATION	  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_SETINFORMATION_NR, MS_MBX_CPROSYNC_INFORMATION)
#define MDRV_MBX_IOC_GETINFORMATION	  _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_GETINFORMATION_NR, MS_MBX_CPROSYNC_INFORMATION)
#define MDRV_MBX_IOC_GETDRVSTATUS      _IOWR(MDRV_MBX_IOC_MAGIC, MDRV_MBX_IOC_GETDRVSTATUS_NR, MS_MBX_GET_DRVSTATUS)

#endif //_MDRV_MBX_IO_H

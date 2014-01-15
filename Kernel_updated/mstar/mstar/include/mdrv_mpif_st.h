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
// @file   mdrv_mpif_st.h
// @brief  MPIF Driver Interface
// @author MStar Semiconductor Inc.
//
// Driver to initialize and access mailbox.
//     - Provide functions to initialize/de-initialize mailbox
//     - Provide mailbox functional access.
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MDRV_MPIF_ST_H
#define _MDRV_MPIF_ST_H

//=============================================================================
// Includs
//=============================================================================
#include <asm/types.h>
#include "MsTypes.h"
#include "mdrv_mpif.h"

//=============================================================================
// Type and Structure Declaration
//=============================================================================
typedef struct _MS_MPIF_2XCFG
{
	MS_U8 		   slaveid;	
	DRV_MPIF_2XCTL  *pst2xctl;
}MS_MPIF_2XCFG;

typedef struct _MS_MPIF_3XCFG
{
	MS_U8 		   slaveid;	
	DRV_MPIF_3XCTL  *pst3xctl;
}MS_MPIF_3XCFG;

typedef struct _MS_MPIF_4ACFG
{
	MS_U8 		   slaveid;	
	DRV_MPIF_4ACTL  *pst4Actl;
}MS_MPIF_4ACFG;

typedef struct _MS_MPIF_1ARW
{
	MS_U8 		   			slaveid;	
	MS_BOOL		   			bWrite;	
	MS_U8		   			u8index;
	MS_U8		   			*pu8data;
	DRV_MPIF_DrvStatus		Res;	
}MS_MPIF_1ARW;

typedef struct _MS_MPIF_2XRW
{
	MS_U8 		   				slaveid;	
	MS_BOOL		   				bWrite;	
	MS_U16		   				u16addr;
	MS_U16		   				*pu16data;
	DRV_MPIF_DrvStatus		    Res;	
}MS_MPIF_2XRW;

typedef struct _MS_MPIF_3XRIURW
{
	MS_U8 		   				slaveid;	
	MS_BOOL		   				bWrite;	
	MS_U8		   				u8datalen;
	MS_U8*		   				pu8data;
	DRV_MPIF_DrvStatus		    Res;	
}MS_MPIF_3XRIURW;

typedef struct _MS_MPIF_MIURW
{
	MS_U8 		   				slaveid;	
	MS_BOOL		   				bWrite;	
	MS_U32		   				u32datalen;
	MS_PHYADDR	   				u32miu_addr;
	MS_PHYADDR	   				u32spif_miuaddr;		
}MS_MPIF_MIURW;

typedef struct _MS_MPIF_BUS_PARAM
{
	MS_U8 		   u8slaveid;	
	MS_U8		   u8cmdwidth;	
	MS_U8		   u8datawidth;	
}MS_MPIF_BUS_PARAM;

typedef struct _MS_MPIF_1BYTE_PARAM
{
	MS_U8 		   u8slaveid;	
	MS_U8		   u8data;	
}MS_MPIF_1BYTE_PARAM;






#endif //_MDRV_MPIF_ST_H

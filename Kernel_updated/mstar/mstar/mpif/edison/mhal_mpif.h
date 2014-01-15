////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2009-2012 MStar Semiconductor, Inc.
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
#ifndef _HAL_MPIF_H_
#define _HAL_MPIF_H_

#include "mhal_mpif_reg.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DEBUG_MPIF(debug_level, x)              do { if (_stMPIFDbgLevel >= (debug_level)) (x); } while(0)


//-------------------------------------------------------------
//event status
#define MPIF_EVENT_TX_DONE								0x00
#define MPIF_EVENT_TX_ERROR								0x01
#define MPIF_EVENT_BUSYTIMEOUT                          0x02
#define MPIF_EVENT_WAITTIMEOUT                          0x04
#define MPIF_EVENT_UNKNOWN								0xFF
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
enum _HAL_MPIF_DebugLevel
{
    MPIF_DBGLV_NONE,    ///< disable all the debug message
    MPIF_DBGLV_INFO,    ///< information
    MPIF_DBGLV_NOTICE,  ///< normal but significant condition
    MPIF_DBGLV_WARNING, ///< warning conditions
    MPIF_DBGLV_ERR,     ///< error conditions
    MPIF_DBGLV_CRIT,    ///< critical conditions
    MPIF_DBGLV_ALERT,   ///< action must be taken immediately
    MPIF_DBGLV_EMERG,   ///< system is unusable
    MPIF_DBGLV_DEBUG,   ///< debug-level messages
};
typedef enum _HAL_MPIF_DebugLevel	HAL_MPIF_DebugLevel;

enum _HAL_MPIF_STATUS
{
    HAL_MPIF_SUCCESS = 0,
	HAL_MPIF_FAIL,
    HAL_MPIF_CHANNEL_BUSY,    
    HAL_MPIF_INVALID_PARAMETER,    
};
typedef enum _HAL_MPIF_STATUS	HAL_MPIF_STATUS;


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
HAL_MPIF_STATUS	HAL_MPIF_Init(void);
void	    	HAL_MPIF_Exit(void);

HAL_MPIF_STATUS HAL_MPIF_LC2X_Configure(MS_U8 u8slaveid, HAL_MPIF_2XCTL *p2xctrl);
HAL_MPIF_STATUS HAL_MPIF_LC3X_Configure(MS_U8 u8slaveid, HAL_MPIF_3XCTL *p3xctrl);
HAL_MPIF_STATUS HAL_MPIF_LC4A_Configure(MS_U8 u8slaveid, HAL_MPIF_4ACTL *p4actrl);

HAL_MPIF_STATUS HAL_MPIF_LC1A_RW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U8 u8index, MS_U8 *pu8data);
HAL_MPIF_STATUS HAL_MPIF_LC2A_RW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U16 u16addr, MS_U16 *pu16data);
HAL_MPIF_STATUS HAL_MPIF_LC2B_RW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U16 u16addr, MS_U16 *pu16data);
HAL_MPIF_STATUS HAL_MPIF_LC3A_RIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U8 u8len, MS_U8 *pu8data);
HAL_MPIF_STATUS HAL_MPIF_LC3B_RIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U8 u8len, MS_U8 *pu8data);

HAL_MPIF_STATUS HAL_MPIF_LC3A_MIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U32 u32len, MS_U32 u32miu_addr);
HAL_MPIF_STATUS HAL_MPIF_LC3B_MIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U32 u32len, MS_U32 u32miu_addr);
HAL_MPIF_STATUS HAL_MPIF_LC4A_MIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U16 u16len, MS_U32 u32miu_addr);

HAL_MPIF_STATUS HAL_MPIF_Configure(HAL_MPIF_CONFIG *pmpifconf);
HAL_MPIF_STATUS HAL_MPIF_Set_Clock(MS_U8 u8clk);
HAL_MPIF_STATUS	HAL_MPIF_CLK_Disable(MS_BOOL bDisabled);
HAL_MPIF_STATUS	HAL_MPIF_SET_1A2XTRCycle(MS_U8 u8trc);
HAL_MPIF_STATUS	HAL_MPIF_SET_3X4ATRCycle(MS_U8 u8trc);
HAL_MPIF_STATUS	HAL_MPIF_SET_WaitCycle(MS_U8 u8wc);
HAL_MPIF_STATUS	HAL_MPIF_SET_SlaveDataWidth(MS_U8 u8slaveid, MS_U8 u8dw);
HAL_MPIF_STATUS	HAL_MPIF_CmdDataWidth(MS_U8 u8dw);
HAL_MPIF_STATUS	HAL_MPIF_SWReset(void);
HAL_MPIF_STATUS HAL_MPIF_SetCmdDataMode(MS_U8 u8slaveid, MS_U8 u8cmdwidth, MS_U8 u8datawidth);

static HAL_MPIF_DebugLevel _stMPIFDbgLevel = 0;


#endif // _HAL_MPIF_H_

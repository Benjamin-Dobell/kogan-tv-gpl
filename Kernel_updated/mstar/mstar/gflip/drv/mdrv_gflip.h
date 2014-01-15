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
/// @file   mdrv_gflip.h
/// @brief  MStar gflip Interface header file
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_GFLIP_H
#define _MDRV_GFLIP_H
#include "mdrv_gflip_ve_st.h"
#include "mdrv_gflip_st.h"

#ifdef _MDRV_GFLIP_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//=============================================================================
// Defines & Macros
//=============================================================================
 //Default enable for T12/J2/, For other IC, please use "make menuconfig" to enable it
#if(defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_JANUS2))
#ifndef CONFIG_MSATR_VE_CAPTURE_SUPPORT
#define CONFIG_MSATR_VE_CAPTURE_SUPPORT 1
#endif
#endif

//=============================================================================
// Type and Structure Declaration
//=============================================================================
typedef struct
{
    MS_U32 u32MainAddr; //in
    MS_U32 u32SubAddr; //in
    MS_U32 u32TagId; //in
    MS_BOOL bKickOff;// if base address has been set to HW register(may not take effect because of Vs0 not arising)
    MS_U32 u32KickOffStartTime;
}GFLIP_INFO, *PGFLIP_INFO;

#define GFLIP_REG_BANKS 12
#define GFLIP_REG16_NUM_PER_BANK 128
#define GFLIP_BANKS_PER_GOP 3
typedef struct
{
    unsigned short BankReg[GFLIP_REG_BANKS][GFLIP_REG16_NUM_PER_BANK];
    //unsigned short GWinEnable[GFLIP_REG_BANKS/GFLIP_BANKS_PER_GOP];
}GFLIP_REGS_SAVE_AREA;

//=============================================================================
// Function
//=============================================================================

//Init related:
INTERFACE MS_BOOL MDrv_GFLIP_Init(MS_U32 u32GopIdx);
INTERFACE MS_BOOL MDrv_GFLIP_DeInit(MS_U32 u32GopIdx);

//Irq Relaated(hw process):
INTERFACE MS_BOOL MDrv_GFLIP_ClearIRQ(void);
INTERFACE MS_BOOL MDrv_GFLIP_ProcessIRQ(void);
INTERFACE MS_BOOL MDrv_GFLIP_VEC_ProcessIRQ(void);
INTERFACE MS_BOOL MDrv_GFLIP_VEC_ClearIRQ(void);

//Handle Vsync. Limitation related:
INTERFACE MS_BOOL MDrv_GFLIP_RestoreFromVsyncLimitation(void);

//Drv Interface related(drv interface):
INTERFACE MS_BOOL MDrv_GFLIP_SetFlipInfo(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32MainAddr, MS_U32 u32SubAddr, MS_U32 u32TagId, MS_U32 * u32QEntry, MS_U32 *u32Result);

INTERFACE MS_BOOL MDrv_GFLIP_GetDWinIntInfo(GFLIP_DWININT_INFO *pGFlipDWinIntInfo, MS_BOOL bResetDWinIntInfo,MS_U32 u32Timeout);

INTERFACE MS_BOOL MDrv_GFLIP_SetPixelIDAddr(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32Addr, MS_U32 u32TagId, MS_U32 * u32QEntry, MS_U32 *u32Result);

INTERFACE MS_BOOL MDrv_GFLIP_SetGPIO3DPin(MS_U32 u32Addr, MS_U32 *u32Result);
INTERFACE MS_BOOL MDrv_GFLIP_SetVECapCurState(MS_BOOL *pbEna);
INTERFACE MS_BOOL MDrv_GFLIP_GetVECapCurState(MS_BOOL *pbEna, MS_U8 *pu8FramCount);
INTERFACE MS_BOOL MDrv_GFLIP_VECapWaitOnFrame(MS_BOOL *pbEna, MS_U8 *pu8FramNum);
INTERFACE void MDrv_GFLIP_SetVECaptureConfig(PMS_GFLIP_VEC_CONFIG pstGflipVECConfig);
INTERFACE void MDrv_GFLIP_GetVECaptureConfig(PMS_GFLIP_VEC_CONFIG pstGflipVECConfig);
INTERFACE MS_BOOL MDrv_GFLIP_ClearFlipQueue(MS_U32 u32GopIdx,MS_U32 u32GwinIdx);
INTERFACE MS_BOOL MDrv_GFLIP_SetGwinInfo(MS_GWIN_INFO stGwinInfo);
INTERFACE int MDrv_GFLIP_Suspend(GFLIP_REGS_SAVE_AREA *pSave);
INTERFACE int MDrv_GFLIP_Resume(GFLIP_REGS_SAVE_AREA *pSave);
INTERFACE MS_BOOL MDrv_DlcIni(char *con_file);
INTERFACE void MDrv_ReadDlcCurveInfo(char *buf);
INTERFACE void MDrv_ReadDlcInfo(char *buf);
INTERFACE void MDrv_GFLIP_Dlc_SetCurve(void);
INTERFACE void MDrv_GFLIP_Dlc_SetBlePoint(void);
INTERFACE MS_BOOL MDrv_GFLIP_SetDlcChangeCurveInfo(MS_DLC_INFO stDlcInfo);
INTERFACE MS_BOOL MDrv_GFLIP_SetDlcOnOffInfo(MS_BOOL bSetDlcOn );
INTERFACE MS_BOOL MDrv_GFLIP_SetBlePointChangeInfo(MS_BLE_INFO stBleInfo);
INTERFACE MS_BOOL MDrv_GFLIP_GetDlcHistogram32Info(MS_U16 *pu16Histogram);
INTERFACE MS_BOOL MDrv_GFLIP_SetBleOnOffInfo(MS_BOOL bSetBleOn );
INTERFACE void MDrv_GFLIP_FilmDriverHWVer1(void);
INTERFACE void MDrv_GFLIP_FilmDriverHWVer2(void);
#if ( defined (CONFIG_MSATR_NEW_FLIP_FUNCTION_ENABLE))
INTERFACE MS_U32 MDrv_GFLIP_InitTimer(void);
INTERFACE MS_U32 MDrv_GFLIP_Del_Timer(void);
#endif

#if defined(__cplusplus)
}
#endif

#undef INTERFACE

#endif //_MDRV_GFLIP_H


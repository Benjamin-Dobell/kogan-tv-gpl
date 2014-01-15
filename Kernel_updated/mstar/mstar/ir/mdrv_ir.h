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
/// @file   drvIR.h
/// @brief  IR Driver Interface
/// @author MStar Semiconductor Inc.
///
/// Driver to initialize and access IR.
///     - Provide functions to initialize IR timing, and enable IR interrupt.
///     - Provide IR ISR.
///     - Provide IR callback function registration for AP.
///     - Provide function to get IR key.
///
/// \b [Example]
/// @code
///
/// // Initalize the IR in the boot time.
/// MDrv_IR_Init();
///
/// // *****************************************************************************
///
/// // Set the delay time of IR. First repeat key code is sent after one second.
/// // The following repeat key code is sent after 0.5 seconds.
/// MDrv_IR_SetDelayTime(1000, 500);
///
/// // Please refer to the following diagram. Assume that when users press and hold
/// // IR button, the repeat key is sent every 200ms.
/// // The 1st press is sent, and the return repeat code is 0.
/// // The 5th repeat key is sent because of the 1st delay time is 1000ms.
/// // The 8th repeat key is sent because of the 2nd delay time is 500ms, and
/// // the time between the 5th and the 8th repeat key is 600ms which is greater
/// // than 500ms.
/// // Note: Do not support RELEASE event.
///
/// @endcode
///
/// @image html IR_delay.JPG "IR delay time"
///
/// @code
/// // *****************************************************************************
///
/// // Set the callback function. The function MApi_IR_SetCallback is called if
/// // the IR interrupt is generated and the delay time setting is matched.
/// void MApi_IR_SetCallback(U8 *pu8Key, U8 *pu8Flg);
///
/// MDrv_IR_Set_Callback(MApi_IR_SetCallback);
///
/// // *****************************************************************************
///
/// // Polling & get the IR key directly. Users can call the MDrv_IR_GetKey to get
/// // the IR key if it returns TRUE.
/// U8 u8Key, u8Flg;
///
/// MDrv_IR_GetKey(&u8Key, &u8Flg);
///
/// @endcode
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_IR_H_
#define _MDRV_IR_H_

#include <asm/types.h>
#include "mdrv_ir_st.h"
#include "mdrv_ir_io.h"
#include "mdrv_types.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define IR_TYPE_FULLDECODE_MODE 1
#define IR_TYPE_RAWDATA_MODE    2
#define IR_TYPE_SWDECODE_MODE   3
#define IR_TYPE_SWDECODE_KON_MODE 4
//#define IR_MODE_SEL             IR_TYPE_SWDECODE_MODE

typedef enum
{
    IR_DECMODE_HWFULL = 0,
    IR_DECMODE_HWRAW =1,
    IR_DECMODE_HWRC5,
    IR_DECMODE_HWRC5X,
    IR_DECMODE_HWRC6,
    IR_DECMODE_SW,
    IR_DECMODE_SHOT,

    IR_DECMODE_EXT = 0xE0,
} IR_DECODE_MODE;

///IR data sequence format select for NEC-like (PPM modulation) formats.
//Note:
//S = System Code.
//C = Customer Code Bits, (ex: C8= customer code 8bits).
//D = Data (Key) Code Bits, (ex: D8= data code 8bits).
//P = Format with Parity Check (ex: 3th byte and 4th byte of NEC format).
typedef enum
{
    IR_XFM_NOTDEF = 0,      //Format not define
    IR_XFM_C16D8D8P=1,      //ex: NEC, Toshiba format
    IR_XFM_C8D8=2,          //ex: Mitsubushi, Konka format
    IR_XFM_C4D8C4D8P=3,	    //ex: RCA format
    IR_XFM_C26D8D8P=4,
    IR_XFM_C32D8D8P=5,
    IR_XFM_C5D6C5D6P=6,
    IR_XFM_C6D6C6D6P=7,
    IR_XFM_D7C6=8,          //ex: Sony-D7C6
    IR_XFM_D7C8=9,          //ex: Sony-D7C8
    IR_XFM_D8C6=10,         //ex: Sony-D8C6
    IR_XFM_D5_only=11,      //ex: MV500
    IR_XFM_S1C4D6=12,       //ex: IRT1250
    IR_XFM_C5D6D4=13,       //ex: LR3715M
    IR_XFM_R1T1C3D6=14,     //ex: M3004 LAB1-Carrier
    IR_XFM_RESERVED=15,     //Reserved

    IR_XFM_DUALRC=0xEE,     //Dual header code
} IR_EXT_FORMAT;

#define IRFLAG_IRENABLE         0x00000001
#define IRFLAG_HWINITED         0x00000002

typedef struct
{
    int                         s32IRMajor;
    int                         s32IRMinor;
    struct cdev                 cDevice;
    struct file_operations      IRFop;
    struct fasync_struct        *async_queue; /* asynchronous readers */
    unsigned long               u32IRFlag;
} IRModHandle;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
void MDrv_IR_Init(int bResumeInit);
BOOL MDrv_IR_TimeCfg(MS_IR_TimeCfg* pIRTimeCfg);
BOOL MDrv_IR_InitCfg(MS_IR_InitCfg* pIRInitCfg);
BOOL MDrv_IR_ReadShotBuffer(MS_IR_ShotInfo* pstShotInfo);//--@@@--IR Pulse Shot Mode
unsigned long MDrv_IR_GetLastKeyTime(void);
BOOL MDrv_IR_GetKey(U8 *pu8Key, U8 *pu8System, U8 *pu8Flag);
void MDrv_IR_SetDelayTime(U32 u32_1stDelayTimeMs, U32 u32_2ndDelayTimeMs);
U8   MDrv_IR_ParseKey(U8 u8Key);
void MDrv_IR_EnableIR(U8 bEnable);
BOOL MDrv_IR_IsFantasyProtocolSupported(void);
unsigned int MDrv_IR_Poll(struct file *filp, poll_table *wait);
ssize_t MDrv_IR_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);

pid_t MDrv_IR_GetMasterPid(void);
void MDrv_IR_SetMasterPid(pid_t pid);
void MDrv_IR_SendKey(U8 u8Key, U8 u8RepeatFlag);
BOOL MDrv_IR_SetHeaderCode(MS_MultiIR_HeaderInfo* pMIrHeaderInfo);

#ifdef CONFIG_MSTAR_IR_INPUT_DEVICE
int MDrv_IR_Input_Init(void);
void MDrv_IR_Input_Exit(void);
#endif

#endif // _MDRV_IR_H_


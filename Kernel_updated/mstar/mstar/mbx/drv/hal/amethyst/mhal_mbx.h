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
/// @file   mhal_mbx.h
/// @brief  MStar Mailbox Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em>legacy interface is only used by MStar proprietary Mail Message communication\n
/// It's API level for backward compatible and will be remove in the next version.\n
/// Please refer @ref drvGE.h for future compatibility.</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_MBX_H
#define _HAL_MBX_H

#ifdef _HAL_MBX_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//=============================================================================
// Includs
//=============================================================================
#include "mhal_mbx_reg.h"

//=============================================================================
// Defines & Macros
//=============================================================================
//busy bit Set/Clear/Get
#define   _BUSY_S(arg)  {\
                                        MS_U8 val; \
                                        mb();\
                                        val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                        mb();\
                                        REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val | MBX_STATE1_BUSY;\
                                        mb();\
                                    }

#define   _BUSY_C(arg)  {\
                                        MS_U8 val; \
                                        mb();\
                                        val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                        mb();\
                                        REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~MBX_STATE1_BUSY;\
                                        mb();\
                                    }


#define   _BUSY(arg)    (REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) & MBX_STATE1_BUSY);


//////////////////////////////////////////////////////////////
//error bit Set/Clear/Get
#define   _ERR_S(arg)   {\
                                        MS_U8 val;\
                                        mb();\
                                        val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                        mb();\
                                        REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val | MBX_STATE1_ERROR;\
                                        mb();\
                                    }

#define   _ERR_C(arg)    {\
                                        MS_U8 val;\
                                        mb();\
                                        val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                        mb();\
                                        REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~MBX_STATE1_ERROR;\
                                        mb();\
                                    }


#define   _ERR(arg)    (REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) & MBX_STATE1_ERROR)

//////////////////////////////////////////////////////////////
//disabled bit Set/Clear/Get
#define   _DISABLED_S(arg)   {\
                            MS_U8 val;\
                            mb();\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            mb();\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val | MBX_STATE1_DISABLED;\
                            mb();\
                         }

#define   _DISABLED_C(arg)   {\
                            MS_U8 val;\
                            mb();\
                            val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                            mb();\
                            REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~MBX_STATE1_DISABLED;\
                            mb();\
                         }

#define   _DISABLED(arg)    (REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) & MBX_STATE1_DISABLED)

////////////////////////////////////////////////////////////////////////
//overflow bit Set/Clear/Get

#define   _OVERFLOW_S(arg)  {\
                                                MS_U8 val;\
                                                mb();\
                                                val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                                mb();\
                                                REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val | MBX_STATE1_OVERFLOW;\
                                                mb();\
                                             }


#define   _OVERFLOW_C(arg)   {\
                                                MS_U8 val;\
                                                mb();\
                                                val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                                mb();\
                                                REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~MBX_STATE1_OVERFLOW;\
                                                mb();\
                                              }


#define   _OVERFLOW(arg)   (REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) & MBX_STATE1_OVERFLOW)

////////////////////////////////////////////////////////////////////////
//status bit clear
#define   _S1_C(arg)   {\
                                    MS_U8 val;\
                                    mb();\
                                    val = REG8_MBX_GROUP(arg, REG8_MBX_STATE_1);\
                                    mb();\
                                    REG8_MBX_GROUP(arg, REG8_MBX_STATE_1) = val & ~(MBX_STATE1_DISABLED | MBX_STATE1_OVERFLOW | MBX_STATE1_ERROR | MBX_STATE1_BUSY);\
                                    mb();\
                                 }


////////////////////////////////////////////////////////////////////////
//fire bit Set/Clear/Get

#define   _FIRE_S(arg)   {\
                                        MS_U8 val;\
                                        mb();\
                                        val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                        mb();\
                                        REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val | MBX_CTRL_FIRE;\
                                        mb();\
                                     }


#define   _FIRE_C(arg)   {\
                                        MS_U8 val;\
                                        mb();\
                                        val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                        mb();\
                                        REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val & ~MBX_CTRL_FIRE;\
                                        mb();\
                                     }


#define   _FIRE(arg)   (REG8_MBX_GROUP(arg, REG8_MBX_CTRL) & MBX_CTRL_FIRE)

////////////////////////////////////////////////////////////////////////
//readback bit Set/Clear/Get
#define   _READBACK_S(arg)   {\
                                                MS_U8 val;\
                                                mb();\
                                                val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                                mb();\
                                                REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val | MBX_CTRL_READBACK;\
                                                mb();\
                                             }

#define   _READBACK_C(arg)   {\
                                                MS_U8 val;\
                                                mb();\
                                                val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                                mb();\
                                                REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val & ~MBX_CTRL_READBACK;\
                                                mb();\
                                             }


#define   _READBACK(arg)   (REG8_MBX_GROUP(arg, REG8_MBX_CTRL) & MBX_CTRL_READBACK)

////////////////////////////////////////////////////////////////////////
//instant bit Set/Clear/Get
#define   _INSTANT_S(arg)   {\
                                             MS_U8 val;\
                                             mb();\
                                             val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                             mb();\
                                             REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val | MBX_CTRL_INSTANT;\
                                             mb();\
                                           }


#define   _INSTANT_C(arg)   {\
                                             MS_U8 val;\
                                             mb();\
                                             val = REG8_MBX_GROUP(arg, REG8_MBX_CTRL);\
                                             mb();\
                                             REG8_MBX_GROUP(arg, REG8_MBX_CTRL) = val & ~MBX_CTRL_INSTANT;\
                                             mb();\
                                           }

#define   _INSTANT(arg)   (REG8_MBX_GROUP(arg, REG8_MBX_CTRL) & MBX_CTRL_INSTANT)

//=============================================================================
// Type and Structure Declaration
//=============================================================================

//=============================================================================
// Enums
/// MBX HAL Recv Status Define
typedef enum
{
    /// Recv Success
    E_MBXHAL_RECV_SUCCESS = 0,
    /// Recv Error: OverFlow
    E_MBXHAL_RECV_OVERFLOW = 1,
    /// Recv Error: Not Enabled
    E_MBXHAL_RECV_DISABLED = 2,
} MBXHAL_Recv_Status;

/// MBX HAL Fire Status Define
typedef enum
{
    /// Fire Success
    E_MBXHAL_FIRE_SUCCESS = 0,
    /// Still Firing
    E_MBXHAL_FIRE_ONGOING = 1,
    /// Fire Error: Overflow:
    E_MBXHAL_FIRE_OVERFLOW = 2,
    /// Fire Error: Not Enabled
    E_MBXHAL_FIRE_DISABLED = 3,
} MBXHAL_Fire_Status;

//=============================================================================
// Mailbox HAL Driver Function
//=============================================================================

INTERFACE MBX_Result MHAL_MBX_Init(MBX_ROLE_ID eHostRole);
INTERFACE MBX_Result MHAL_MBX_SetConfig(MBX_ROLE_ID eHostRole);

INTERFACE MBX_Result MHAL_MBX_SetInformation(MBX_ROLE_ID eTargetRole, MS_U8 *pU8Info, MS_U8 u8Size);
INTERFACE MBX_Result MHAL_MBX_GetInformation(MBX_ROLE_ID eTargetRole, MS_U8 *pU8Info, MS_U8 u8Size);

INTERFACE MBX_Result MHAL_MBX_Fire(MBX_Msg* pMbxMsg, MBX_ROLE_ID eSrcRole);
INTERFACE MBX_Result MHAL_MBX_GetFireStatus(MBX_ROLE_ID eSrcRole, MBX_ROLE_ID eDstRole, MBXHAL_Fire_Status *pFireStatus);

INTERFACE MBX_Result MHAL_MBX_Recv(MBX_Msg* pMbxMsg, MBX_ROLE_ID eDstRole);
INTERFACE MBX_Result MHAL_MBX_RecvEnd(MBX_ROLE_ID eSrcRole, MBX_ROLE_ID eDstRole, MBXHAL_Recv_Status eRecvSatus);
INTERFACE MS_U16 MHAL_MBX_RegGroup(MBX_ROLE_ID eSrcRole, MBX_ROLE_ID eDstRole);

#undef INTERFACE
#endif //_HAL_MBX_H


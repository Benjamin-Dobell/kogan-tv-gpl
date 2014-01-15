//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all 
// or part of MStar Software is expressly prohibited, unless prior written 
// permission has been granted by MStar. 
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.  
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software. 
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s 
//    confidential information in strictest confidence and not disclose to any
//    third party.  
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.  
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or 
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.  
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>

#ifndef __CUS_BOOT_H__
#define __CUS_BOOT_H__

#if (ENABLE_MODULE_BOOT_IR == 1)
#if (ENABLE_MSTAR_TITANIA_BD_MSTCUS23_6A801_A3)
#include "IR_SZ_HAIER_TOSHIBA.h"
#define IR_RECOVERY_KEY             IRKEY_EXIT
#define IR_UPGRADEBOOTLOADER_KEY    IRKEY_VOLUME_PLUS
#define IR_FORCEUGRADE_KEY          IRKEY_MENU
#define IR_UART_ON_KEY              IRKEY_INPUT_SOURCE
#elif (ENABLE_MSTAR_TITANIA_BD_CUS28_6A800 \
    || ENABLE_MSTAR_TITANIA_BD_CUS28_6A801 \
    || ENABLE_MSTAR_BD_MST036B_EDISON_CUS28 \
    || ENABLE_MSTAR_BD_MST036B_EDISON_CUS28_AP)
#include "IR_SZ_KON1.h"
#define IR_RECOVERY_KEY             IRKEY_CHANNEL_MINUS
#define IR_UPGRADEBOOTLOADER_KEY    IRKEY_VOLUME_MINUS
#define IR_FORCEUGRADE_KEY          IRKEY_VOLUME_PLUS
#define IR_UART_ON_KEY              IRKEY_CHANNEL_PLUS
#elif (ENABLE_MSTAR_TITANIA_BD_MS801_MAA2XG_A3)
#include "IR_TCL_RCA.h"
#define IR_RECOVERY_KEY             IRKEY_INPUT_SOURCE
#define IR_UPGRADEBOOTLOADER_KEY    IRKEY_VOLUME_MINUS
#define IR_FORCEUGRADE_KEY          IRKEY_SELECT
#define IR_UART_ON_KEY              IRKEY_INPUT_SOURCE
#else
#include "IR_MSTAR_DTV.h"
#define IR_RECOVERY_KEY             IRKEY_EXIT
#define IR_UPGRADEBOOTLOADER_KEY    IRKEY_MENU
#define IR_FORCEUGRADE_KEY          IRKEY_VOLUME_PLUS
#define IR_UART_ON_KEY              IRKEY_INPUT_SOURCE
#endif
#endif

#if (ENABLE_MODULE_BOOT_KEYPAD == 1)

#if (ENABLE_MSTAR_TITANIA_BD_MSTCUS23_6A801_A3)
#include "IR_SZ_HAIER_TOSHIBA.h"
#define KEYPAD_RECOVERY_KEY             IRKEY_INPUT_SOURCE
#define KEYPAD_UPGRADEBOOTLOADER_KEY    IRKEY_VOLUME_PLUS
#define KEYPAD_FORCEUGRADE_KEY          IRKEY_MENU
#define KEYPAD_UART_ON_KEY              IRKEY_INPUT_SOURCE
#elif (ENABLE_MSTAR_TITANIA_BD_CUS28_6A800 \
    || ENABLE_MSTAR_TITANIA_BD_CUS28_6A801 \
    || ENABLE_MSTAR_BD_MST036B_EDISON_CUS28 \
    || ENABLE_MSTAR_BD_MST036B_EDISON_CUS28_AP)
#include "IR_SZ_KON1.h"
#define KEYPAD_RECOVERY_KEY             IRKEY_CHANNEL_MINUS
#define KEYPAD_UPGRADEBOOTLOADER_KEY    IRKEY_VOLUME_MINUS
#define KEYPAD_FORCEUGRADE_KEY          IRKEY_VOLUME_PLUS
#define KEYPAD_UART_ON_KEY              IRKEY_CHANNEL_PLUS
#elif (ENABLE_MSTAR_TITANIA_BD_MS801_MAA2XG_A3)
#include "IR_TCL_RCA.h"
#define KEYPAD_RECOVERY_KEY             IRKEY_INPUT_SOURCE
#define KEYPAD_UPGRADEBOOTLOADER_KEY    IRKEY_VOLUME_MINUS
#define KEYPAD_FORCEUGRADE_KEY          IRKEY_SELECT
#define KEYPAD_UART_ON_KEY              IRKEY_INPUT_SOURCE
#else
#include "IR_MSTAR_DTV.h"
#define KEYPAD_RECOVERY_KEY             IRKEY_INPUT_SOURCE
#define KEYPAD_UPGRADEBOOTLOADER_KEY    IRKEY_MENU
#define KEYPAD_FORCEUGRADE_KEY          IRKEY_VOLUME_PLUS
#define KEYPAD_UART_ON_KEY              IRKEY_INPUT_SOURCE
#endif
#endif


#endif


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
#ifndef _SPI_DATABASE_H__
#define _SPI_DATABASE_H__
#include <ShareType.h>
#include <MsApiKeypad.h>
#include <apiPNL.h>

#define BUFFER_SIZE                 128
#define spi_sector_size             0x10000
#define env_sector_size             0x20000

#define Kpd_ChID_Offset             20

#define PWM_DUTY   "u32PWMDuty"
#define PWM_PERIOD "u32PWMPeriod"
#define PWM_DIVPWM "u16DivPWM"
#define PWM_POLPWM "bPolPWM"
#define PWM_MAXPWM "u16MaxPWMvalue"
#define PWM_MINPWM "u16MinPWMvalue"

#define MIRROR_ON "MIRROR_ON"
#define KEYPAD_SEL "KEYPAD_SEL"

#define LOCALDIMMING "bLocalDIMMINGEnable"

typedef struct
{
    U32 logo_offset;
    U32 logo_size;
    U32 music_offset;
    U32 music_size;
    U32 ursa_6m30_offset;
    U32 ursa_6m30_size;
    U32 panelpara_offset;
    U32 panelpara_size;
    U32 boardpara_offset;
    U32 boardpara_size;
    U32 keypad_offset;
    U32 keypad_size;
}db_struct;

typedef struct
{
    long URSA_6M30_LVDS_CHANNEL_SWAP_MODE;
    long URSA_6M30_IN_LVDS_TIMODE;
    long URSA_6M30_IN_LVDS_TI_BITNUM;
    long URSA_6M30_OUT_LVDS_TIMODE;
    long URSA_6M30_OUT_LVDS_TI_BITNUM;
    long URSA_6M30_OUT_LVDS_SWAP_POLARITY;
    long URSA_6M30_SSC_SEL;
    long URSA_6M30_SSC_ENABLE;
    long URSA_6M30_SSC_FREQ;
    long URSA_6M30_SSC_PERCENT;
}ursa_6m30_cmd_table;

typedef struct
{
    U8   m_u8BOARD_PWM_PORT;
    U16  m_u16BOARD_LVDS_CONNECT_TYPE;
    U8   m_bPANEL_PDP_10BIT;
    U8   m_bPANEL_SWAP_LVDS_POL;
    U8   m_bPANEL_SWAP_LVDS_CH;
    U8   m_bPANEL_CONNECTOR_SWAP_PORT;
    U16  m_u16LVDS_PN_SWAP_L;
    U16  m_u16LVDS_PN_SWAP_H;
}st_board_para;

int write_dbtable(U32 u32LogoOffset);
int read_dbtable(U32 u32LogoOffset);

int Read_LogoToDisplayAddr(U32 u32LogoDramAddr);
int Read_PanelParaFromflash(PanelType * panel_data);
int Read_BoardParaFromflash(st_board_para * board_data);
int Read_KeypadParaFromflash(st_keypad_para *keypad_data);
int Read_MusicToPlayAddr(MS_U32 u32MusicDramAddr);
int Read_Ursa_6m30_Para(ursa_6m30_cmd_table * cmd_table);

#endif //__SPI_DATABASE_H__

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

#ifndef IR_FORMAT_H
#define IR_FORMAT_H

//-------------------------------------------------------------------------------------------
// Customer IR Specification parameter define (Please modify them by IR SPEC)
//-------------------------------------------------------------------------------------------
#define IR_MODE_SEL             IR_TYPE_FULLDECODE_MODE
// IR Header code define
#define IR_HEADER_CODE0         0x87    // Custom 0
#define IR_HEADER_CODE1         0x6A    // Custom 1

// IR Timing define
#define IR_HEADER_CODE_TIME     9000    // us
#define IR_OFF_CODE_TIME        4500    // us
#define IR_OFF_CODE_RP_TIME     2500    // us
#define IR_LOGI_01H_TIME        560     // us
#define IR_LOGI_0_TIME          1120    // us
#define IR_LOGI_1_TIME          2240    // us
#define IR_TIMEOUT_CYC          140000  // us

// IR Format define
#define IRKEY_DUMY              0xFF
#define IRDA_KEY_MAPPING_POWER  IRKEY_POWER

typedef enum _IrCommandType
{
    IRKEY_TV_RADIO          = 0x13,
    IRKEY_CHANNEL_LIST      = 0x10,
    IRKEY_CHANNEL_FAV_LIST  = 0x0F,
    IRKEY_CHANNEL_RETURN    = 0x14,
    IRKEY_CHANNEL_PLUS      = 0x16,
    IRKEY_CHANNEL_MINUS     = 0x1C,

    IRKEY_AUDIO             = IRKEY_DUMY,
    IRKEY_VOLUME_PLUS       = 0x00,
    IRKEY_VOLUME_MINUS      = 0x19,

    IRKEY_UP                = 0x1A,
    IRKEY_POWER             = 0x12,
    IRKEY_EXIT              = 0x0B,
    IRKEY_MENU              = 0x0D,
    IRKEY_DOWN              = 0x1E,
    IRKEY_LEFT              = 0x1F,
    IRKEY_SELECT            = 0x0E,
    IRKEY_RIGHT             = 0x1B,

    IRKEY_NUM_0             = 0x44,
    IRKEY_NUM_1             = 0x01,
    IRKEY_NUM_2             = 0x02,
    IRKEY_NUM_3             = 0x03,
    IRKEY_NUM_4             = 0x04,
    IRKEY_NUM_5             = 0x05,
    IRKEY_NUM_6             = 0x06,
    IRKEY_NUM_7             = 0x07,
    IRKEY_NUM_8             = 0x08,
    IRKEY_NUM_9             = 0x09,

    IRKEY_MUTE              = 0x48,
    IRKEY_PAGE_UP           = IRKEY_DUMY-1,
    IRKEY_PAGE_DOWN         = IRKEY_DUMY-2,
    IRKEY_CLOCK             = IRKEY_DUMY-3,

    IRKEY_INFO              = 0x0C,
    IRKEY_RED               = 0x5C,
    IRKEY_GREEN             = 0x58,
    IRKEY_YELLOW            = 0x54,
    IRKEY_BLUE              = 0x59,
    IRKEY_MTS               = 0x0A,
    IRKEY_NINE_LATTICE      = IRKEY_DUMY-4,
    IRKEY_TTX               = 0x1D,
    IRKEY_CC                = 0x1D,
    IRKEY_INPUT_SOURCE      = 0x15,
    IRKEY_CRADRD            = IRKEY_DUMY-5,
//    IRKEY_PICTURE           = IRKEY_DUMY-6,
    IRKEY_ZOOM              = IRKEY_DUMY-6,
    IRKEY_DASH              = 0x18,
    IRKEY_SLEEP             = IRKEY_DUMY-7,
    IRKEY_EPG               = 0x17,
    IRKEY_PIP               = IRKEY_DUMY-8,

  	IRKEY_MIX               = IRKEY_DUMY-9,
    IRKEY_INDEX             = IRKEY_DUMY-10,
    IRKEY_HOLD              = IRKEY_DUMY-11,
    IRKEY_PREVIOUS          = IRKEY_DUMY-12,
    IRKEY_NEXT              = IRKEY_DUMY-13,
    IRKEY_BACKWARD          = IRKEY_DUMY-14,
    IRKEY_FORWARD           = IRKEY_DUMY-15,
    IRKEY_PLAY              = IRKEY_DUMY-16,
    IRKEY_RECORD            = IRKEY_DUMY-17,
    IRKEY_STOP              = IRKEY_DUMY-18,
    IRKEY_PAUSE             = IRKEY_DUMY-19,

    IRKEY_SIZE              = IRKEY_DUMY-20,
    IRKEY_REVEAL            = IRKEY_DUMY-21,
    IRKEY_SUBCODE           = IRKEY_DUMY-22,
}IrCommandType;
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// IR system parameter define for H/W setting (Please don't modify them)
//-------------------------------------------------------------------------------------------
#define IR_CKDIV_NUM                        ((XTAL_CLOCK_FREQ+500000)/1000000)
#define IR_CKDIV_NUM_BOOT                   XTAL_CLOCK_FREQ
#define IR_CLK_BOOT                         (XTAL_CLOCK_FREQ/1000000)
#define IR_CLK                              IR_CLK_BOOT

#define irGetMinCnt_BOOT(time, tolerance)   ((u32)(((double)time*((double)IR_CLK_BOOT)/(IR_CKDIV_NUM_BOOT+1))*((double)1-tolerance)))
#define irGetMaxCnt_BOOT(time, tolerance)   ((u32)(((double)time*((double)IR_CLK_BOOT)/(IR_CKDIV_NUM_BOOT+1))*((double)1+tolerance)))
#define irGetMinCnt(time, tolerance)        ((u32)(((double)time*((double)IR_CLK)/(IR_CKDIV_NUM+1))*((double)1-tolerance)))
#define irGetMaxCnt(time, tolerance)        ((u32)(((double)time*((double)IR_CLK)/(IR_CKDIV_NUM+1))*((double)1+tolerance)))

#define irGetCnt_BOOT(time)                 ((u32)((double)time*((double)IR_CLK_BOOT)/(IR_CKDIV_NUM_BOOT+1)))
#define irGetCnt(time)                      ((u32)((double)time*((double)IR_CLK)/(IR_CKDIV_NUM+1)))

// 12Mhz
#define IR_RP_TIMEOUT_BOOT      irGetCnt_BOOT(IR_TIMEOUT_CYC)
#define IR_HDC_UPB_BOOT         irGetMaxCnt_BOOT(IR_HEADER_CODE_TIME, 0.2)
#define IR_HDC_LOB_BOOT         irGetMinCnt_BOOT(IR_HEADER_CODE_TIME, 0.2)
#define IR_OFC_UPB_BOOT         irGetMaxCnt_BOOT(IR_OFF_CODE_TIME, 0.2)
#define IR_OFC_LOB_BOOT         irGetMinCnt_BOOT(IR_OFF_CODE_TIME, 0.2)
#define IR_OFC_RP_UPB_BOOT      irGetMaxCnt_BOOT(IR_OFF_CODE_RP_TIME, 0.2)
#define IR_OFC_RP_LOB_BOOT      irGetMinCnt_BOOT(IR_OFF_CODE_RP_TIME, 0.2)
#define IR_LG01H_UPB_BOOT       irGetMaxCnt_BOOT(IR_LOGI_01H_TIME, 0.35)
#define IR_LG01H_LOB_BOOT       irGetMinCnt_BOOT(IR_LOGI_01H_TIME, 0.3)
#define IR_LG0_UPB_BOOT         irGetMaxCnt_BOOT(IR_LOGI_0_TIME, 0.2)
#define IR_LG0_LOB_BOOT         irGetMinCnt_BOOT(IR_LOGI_0_TIME, 0.2)
#define IR_LG1_UPB_BOOT         irGetMaxCnt_BOOT(IR_LOGI_1_TIME, 0.2)
#define IR_LG1_LOB_BOOT         irGetMinCnt_BOOT(IR_LOGI_1_TIME, 0.2)

// 90Mhz
#define IR_RP_TIMEOUT           irGetCnt(IR_TIMEOUT_CYC)
#define IR_HDC_UPB              irGetMaxCnt(IR_HEADER_CODE_TIME, 0.2)
#define IR_HDC_LOB              irGetMinCnt(IR_HEADER_CODE_TIME, 0.2)
#define IR_OFC_UPB              irGetMaxCnt(IR_OFF_CODE_TIME, 0.2)
#define IR_OFC_LOB              irGetMinCnt(IR_OFF_CODE_TIME, 0.2)
#define IR_OFC_RP_UPB           irGetMaxCnt(IR_OFF_CODE_RP_TIME, 0.2)
#define IR_OFC_RP_LOB           irGetMinCnt(IR_OFF_CODE_RP_TIME, 0.2)
#define IR_LG01H_UPB            irGetMaxCnt(IR_LOGI_01H_TIME, 0.35)
#define IR_LG01H_LOB            irGetMinCnt(IR_LOGI_01H_TIME, 0.3)
#define IR_LG0_UPB              irGetMaxCnt(IR_LOGI_0_TIME, 0.2)
#define IR_LG0_LOB              irGetMinCnt(IR_LOGI_0_TIME, 0.2)
#define IR_LG1_UPB              irGetMaxCnt(IR_LOGI_1_TIME, 0.2)
#define IR_LG1_LOB              irGetMinCnt(IR_LOGI_1_TIME, 0.2)

//-------------------------------------------------------------------------------------------

#endif


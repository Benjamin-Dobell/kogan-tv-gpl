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
#define IR_MODE_SEL             IR_TYPE_SWDECODE_MODE
// IR Header code define
#define IR_HEADER_CODE0         0x00    // Custom 0
#define IR_HEADER_CODE1         0x0F    // Custom 1

#define BIKE_HEADER_CODE0     0x05
#define BIKE_HEADER_CODE1     0x0A


// IR Timing define
#define IR_HEADER_CODE_TIME     4000    // us
#define IR_OFF_CODE_TIME        4000    // us
#define IR_OFF_CODE_RP_TIME     10800   // us
#define IR_OFF_CODE_RP_TIME2    25400   // us
#define IR_LOGI_01H_TIME        500     // us
#define IR_LOGI_0_TIME          2500    // us
#define IR_LOGI_1_TIME          1500    // us
#define IR_TIMEOUT_CYC          140000  // us

// IR Format define
#define IRKEY_DUMY              0x00
#define IRDA_KEY_MAPPING_POWER  IRKEY_POWER

typedef enum _IrCommandType
{
    IRKEY_TV_RADIO          = 0x5F,
    IRKEY_CHANNEL_LIST      = 0xD7,
    IRKEY_CHANNEL_FAV_LIST  = 0x2F,
    IRKEY_CHANNEL_RETURN    = 0xD8,
    IRKEY_CHANNEL_PLUS      = 0xD2,
    IRKEY_CHANNEL_MINUS     = 0xD3,

    IRKEY_AUDIO             = 0xA5,
    IRKEY_VOLUME_PLUS       = 0xD0,
    IRKEY_VOLUME_MINUS      = 0xD1,

    IRKEY_UP                = 0xA6,
    IRKEY_POWER             = 0xD5,
    IRKEY_EXIT              = 0xF9,
    IRKEY_MENU              = 0xF7,
    IRKEY_DOWN              = 0xA7,
    IRKEY_LEFT              = 0xA9,
    IRKEY_SELECT            = 0x0B,
    IRKEY_RIGHT             = 0xA8,

    IRKEY_NUM_0             = 0xCF,
    IRKEY_NUM_1             = 0xCE,
    IRKEY_NUM_2             = 0xCD,
    IRKEY_NUM_3             = 0xCC,
    IRKEY_NUM_4             = 0xCB,
    IRKEY_NUM_5             = 0xCA,
    IRKEY_NUM_6             = 0xC9,
    IRKEY_NUM_7             = 0xC8,
    IRKEY_NUM_8             = 0xC7,
    IRKEY_NUM_9             = 0xC6,

    IRKEY_MUTE              = 0xC0,
    IRKEY_PAGE_UP           = IRKEY_DUMY+1,
    IRKEY_PAGE_DOWN         = IRKEY_DUMY+2,
    IRKEY_CLOCK             = IRKEY_DUMY+3,

    IRKEY_INFO              = 0xC3,
    IRKEY_RED               = IRKEY_DUMY+4,
    IRKEY_GREEN             = IRKEY_DUMY+5,
    IRKEY_YELLOW            = IRKEY_DUMY+6,
    IRKEY_BLUE              = IRKEY_DUMY+7,
    IRKEY_MTS               = 0x1B,
    IRKEY_NINE_LATTICE      = IRKEY_DUMY+8,
    IRKEY_TTX               = IRKEY_DUMY+9,
    IRKEY_CC                = IRKEY_DUMY+10,
    IRKEY_INPUT_SOURCE      = 0x5C,
    IRKEY_CRADRD            = IRKEY_DUMY+12,
//    IRKEY_PICTURE           = 0x40,
    IRKEY_ZOOM              = 0x6F,
    IRKEY_DASH              = IRKEY_DUMY+13,
    IRKEY_SLEEP             = 0xF8,
    IRKEY_EPG               = 0xE5,
    IRKEY_PIP               = IRKEY_DUMY+14,

  	IRKEY_MIX               = IRKEY_DUMY+15,
    IRKEY_INDEX             = IRKEY_DUMY+16,
    IRKEY_HOLD              = IRKEY_DUMY+17,
    IRKEY_PREVIOUS          = 0x45,
    IRKEY_NEXT              = 0xAC,
    IRKEY_BACKWARD          = 0xE2,
    IRKEY_FORWARD           = 0xE3,
    IRKEY_PLAY              = 0xEA,
    IRKEY_RECORD            = 0xE8,
    IRKEY_STOP              = 0xE0,
    IRKEY_PAUSE             = IRKEY_DUMY+18,

    IRKEY_SIZE              = 0x8A,
    IRKEY_REVEAL            = IRKEY_DUMY+19,
    IRKEY_SUBCODE           = IRKEY_DUMY+20,
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
#define IR_OFC_RP_UPB           irGetMaxCnt(IR_OFF_CODE_RP_TIME2, 0.2)
#define IR_OFC_RP_LOB           irGetMinCnt(IR_OFF_CODE_RP_TIME, 0.2)
#define IR_LG01H_UPB            irGetMaxCnt(IR_LOGI_01H_TIME, 0.35)
#define IR_LG01H_LOB            irGetMinCnt(IR_LOGI_01H_TIME, 0.3)
#define IR_LG0_UPB              irGetMaxCnt(IR_LOGI_0_TIME, 0.2)
#define IR_LG0_LOB              irGetMinCnt(IR_LOGI_0_TIME, 0.2)
#define IR_LG1_UPB              irGetMaxCnt(IR_LOGI_1_TIME, 0.2)
#define IR_LG1_LOB              irGetMinCnt(IR_LOGI_1_TIME, 0.2)

//-------------------------------------------------------------------------------------------

#endif


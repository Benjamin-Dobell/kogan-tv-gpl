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

//*************************************************************************
// Customer IR Specification parameter define (Please modify them by IR SPEC)
//*************************************************************************
#define IR_MODE_SEL             IR_TYPE_FULLDECODE_MODE
#define IR_LG_MAP               1
// IR Header code define


#define IR_HEADER_CODE0         0x04    // Custom 0     //0x04
#define IR_HEADER_CODE1         0xFB    // Custom 1     //0xFB

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
    IRKEY_TV_RADIO          = IRKEY_DUMY-0,
    IRKEY_CHANNEL_LIST      = IRKEY_DUMY-1,
    IRKEY_CHANNEL_FAV_LIST  = 0x1E,
    IRKEY_CHANNEL_RETURN    = 0x1A,
    IRKEY_CHANNEL_PLUS      = 0x00,
    IRKEY_CHANNEL_MINUS     = 0x01,

    IRKEY_AUDIO             = 0x52,
    IRKEY_VOLUME_PLUS       = 0x02,
    IRKEY_VOLUME_MINUS      = 0x03,

    IRKEY_UP                = 0x40,
    IRKEY_POWER             = 0x08,
    IRKEY_EXIT              = 0x5B,
    IRKEY_MENU              = 0x43,
    IRKEY_DOWN              = 0x41,
    IRKEY_LEFT              = 0x07,
    IRKEY_SELECT            = 0x44,
    IRKEY_RIGHT             = 0x06,

    IRKEY_NUM_0             = 0x10,
    IRKEY_NUM_1             = 0x11,
    IRKEY_NUM_2             = 0x12,
    IRKEY_NUM_3             = 0x13,
    IRKEY_NUM_4             = 0x14,
    IRKEY_NUM_5             = 0x15,
    IRKEY_NUM_6             = 0x16,
    IRKEY_NUM_7             = 0x17,
    IRKEY_NUM_8             = 0x18,
    IRKEY_NUM_9             = 0x19,

    IRKEY_MUTE              = 0x09,
    IRKEY_PAGE_UP           = IRKEY_DUMY-2,
    IRKEY_PAGE_DOWN         = IRKEY_DUMY-3,
    IRKEY_CLOCK             = IRKEY_DUMY-4,

    IRKEY_INFO              = 0xAA,
    IRKEY_RED               = 0x72,
    IRKEY_GREEN             = 0x71,
    IRKEY_YELLOW            = 0x63,
    IRKEY_BLUE              = 0x61,
    IRKEY_MTS               = 0x0A,
    IRKEY_NINE_LATTICE      = IRKEY_DUMY-9,
    IRKEY_TTX               = IRKEY_DUMY-10,
    IRKEY_CC                = 0x39,
    IRKEY_INPUT_SOURCE      = 0x0B,
    IRKEY_CRADRD            = IRKEY_DUMY-11,
//    IRKEY_PICTURE           = 0x40,
    IRKEY_ZOOM              = 0x79,
    IRKEY_DASH              = 0x4C,
    IRKEY_SLEEP             = 0x0E,
    IRKEY_EPG               = IRKEY_DUMY-12,
    IRKEY_PIP               = 0x60,             //0x4D,

  	IRKEY_MIX               = IRKEY_DUMY-13,
    IRKEY_INDEX             = IRKEY_DUMY-14,
    IRKEY_HOLD              = IRKEY_DUMY-15,
    IRKEY_PREVIOUS          = IRKEY_DUMY-16,
    IRKEY_NEXT              = IRKEY_DUMY-17,
    IRKEY_BACKWARD          = IRKEY_DUMY-18,
    IRKEY_FORWARD           = IRKEY_DUMY-19,
    IRKEY_PLAY              = IRKEY_DUMY-20,
    IRKEY_RECORD            = IRKEY_DUMY-21,
    IRKEY_STOP              = IRKEY_DUMY-22,
    IRKEY_PAUSE             = IRKEY_DUMY-23,

    IRKEY_SIZE              = IRKEY_DUMY-24,
    IRKEY_REVEAL            = IRKEY_DUMY-25,
    IRKEY_SUBCODE           = IRKEY_DUMY-26,

    IRKEY_ADJUST            = 0xCB,
    IRKEY_TV_INPUT          = 0x0F,
//    IRKEY_BRIGHTNESS_PLUS = 0xE0,
//    IRKEY_BRIGHTNESS_MINUS = 0xE1,

    IRKEY_IN_STOP           = 0xFA,
    IRKEY_IN_START          = 0xFB,
    IRKEY_P_CHECK           = 0xFC,
    IRKEY_S_CHECK           = 0xFD,
    IRKEY_POWERONLY         = 0xFE,
    IRKEY_EZ_ADJUST         = 0xFF,
    DSC_IRKEY_PWRON         = 0xC4,
    DSC_IRKEY_PWROFF        = 0xC5,
    DSC_IRKEY_ARC4X3        = 0x76,
    DSC_IRKEY_ARC16X9       = 0x77,
    DSC_IRKEY_ARCZOOM       = 0xAF,
    DSC_IRKEY_TV            = 0xD6,
    DSC_IRKEY_VIDEO1        = 0x5A,
    DSC_IRKEY_VIDEO2        = 0xD0,
    DSC_IRKEY_VIDEO3        = 0xD1,
    DSC_IRKEY_COMP1         = 0xBF,
    DSC_IRKEY_COMP2         = 0xD4,
    DSC_IRKEY_RGBPC         = 0xD5,
    DSC_IRKEY_RGBDTV        = 0xD7,
    DSC_IRKEY_RGBDVI        = 0xC6,
    DSC_IRKEY_HDMI1         = 0xCE,
    DSC_IRKEY_HDMI2         = 0xCC,
    DSC_IRKEY_MULTI_PIP     = 0x70,
    DSC_IRKEY_MULTIMEDIA    = 0x98,
}IrCommandType;
//*************************************************************************

//*************************************************************************
// IR system parameter define for H/W setting (Please don't modify them)
//*************************************************************************
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

// Power off mode
#define PM_IR_TH_VAL            (PM_TH0_VAL & 0x0F)
// PM_IR_TH_GRID specify the time (in us) for each threshold bit.
// So PM_IR_TH_GRID = (1/12M) * (2^14) * 1000000 = (2^14) / 12
#define PM_IR_TH_GRID           (16384/IR_CLK_BOOT)
#define PM_IR_HEADER_CODE_TIME  (IR_HEADER_CODE_TIME-(0x0F-PM_IR_TH_VAL)*PM_IR_TH_GRID)
#define PM_IR_HDC_UPB_BOOT      irGetMaxCnt_BOOT(PM_IR_HEADER_CODE_TIME, 0.6)
#define PM_IR_HDC_LOB_BOOT      irGetMinCnt_BOOT(PM_IR_HEADER_CODE_TIME, 0.6)
//*************************************************************************

#endif

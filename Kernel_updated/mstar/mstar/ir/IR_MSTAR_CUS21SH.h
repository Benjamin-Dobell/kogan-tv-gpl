////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
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
#define IR_MODE_SEL             IR_TYPE_SWDECODE_MODE   //IR_TYPE_SWDECODE_SHA_MODE
#define IR_CHANNEL_USE_AS_UPDOWN   0
#define IR_VOLUME_USE_AS_LEFTRIGHT      0
// IR Header code define
#define IR_HEADER_CODE0         0x80    // Custom 0     //0x04
#define IR_HEADER_CODE1         0x7F    // Custom 1     //0xFB

// IR Timing define
#define IR_HEADER_CODE_TIME     9000    // us
#define IR_OFF_CODE_TIME        4500    // us
#define IR_OFF_CODE_RP_TIME     2500    // us
#define IR_LOGI_01H_TIME        560     // us
#define IR_LOGI_0_TIME          1120    // us
#define IR_LOGI_1_TIME          2240    // us
#define IR_TIMEOUT_CYC          140000  // us

#define IR_HEADER_CODE_TIME_UB  20
#define IR_HEADER_CODE_TIME_LB  -20
#define IR_OFF_CODE_TIME_UB  20
#define IR_OFF_CODE_TIME_LB  -20
#define IR_OFF_CODE_RP_TIME_UB  20
#define IR_OFF_CODE_RP_TIME_LB  -20
#define IR_LOGI_01H_TIME_UB  35
#define IR_LOGI_01H_TIME_LB  -30
#define IR_LOGI_0_TIME_UB  20
#define IR_LOGI_0_TIME_LB  -20
#define IR_LOGI_1_TIME_UB  20
#define IR_LOGI_1_TIME_LB  -20
// IR Format define
#define IRKEY_DUMY              0xFF
#define IRDA_KEY_MAPPING_POWER  IRKEY_POWER

#define IR_LEADER_CODE_CHECKING_OPTION 0xBF

#if 1
typedef enum _IR_CUS21SH_RAWDATA_TABLE
{
    SH_IRKEY_NUM_0                 = 0x2141,
    SH_IRKEY_NUM_1                 = 0x2021,
    SH_IRKEY_NUM_2                 = 0x2041,
    SH_IRKEY_NUM_3                 = 0x2061,
    SH_IRKEY_NUM_4                 = 0x2081,
    SH_IRKEY_NUM_5                 = 0x20A1,
    SH_IRKEY_NUM_6                 = 0x20C1,
    SH_IRKEY_NUM_7                 = 0x20E1,
    SH_IRKEY_NUM_8                 = 0x2101,
    SH_IRKEY_NUM_9                 = 0x2121,

    SH_IRKEY_UP                    = 0x2AE1,
    SH_IRKEY_POWER                 = 0x22C1,
    SH_IRKEY_EXIT                  = 0x3EA1,
    SH_IRKEY_MENU                  = 0x38D1,
    SH_IRKEY_DOWN                  = 0x2401,
    SH_IRKEY_LEFT                  = 0x3AE1,
    SH_IRKEY_RIGHT                 = 0x3B01,
    SH_IRKEY_SELECT                = 0x2A41,
    SH_IRKEY_RETURN                = 0x3C81,

    SH_IRKEY_CHANNEL_LIST          = 0x34F1,
    SH_IRKEY_CHANNEL_FAV_LIST      = 0x3451,
    SH_IRKEY_CHANNEL_PLUS          = 0x2221,
    SH_IRKEY_CHANNEL_MINUS         = 0x2241,
    SH_IRKEY_CHANNEL_RETURN        = 0x24E1,

    SH_IRKEY_RED                   = 0x2901,
    SH_IRKEY_GREEN                 = 0x2921,
    SH_IRKEY_YELLOW                = 0x2941,
    SH_IRKEY_BLUE                  = 0x2961,

    SH_IRKEY_MUTE                  = 0x22E1,
    SH_IRKEY_MTS                   = 0x2301,
    SH_IRKEY_VOLUME_PLUS           = 0x2281,
    SH_IRKEY_VOLUME_MINUS          = 0x22A1,

    SH_IRKEY_TTX                   = 0x2681,
    SH_IRKEY_REVEAL                = 0x2701,
    SH_IRKEY_SUBTITLE              = 0x33FE,
    SH_IRKEY_HOLD                  = 0x2661,
    SH_IRKEY_SUBPAGE               = 0x39BE,
    SH_IRKEY_SIZE                  = 0x26E1,

    SH_IRKEY_INPUT_SOURCE          = 0x2261,
    SH_IRKEY_DTV                   = 0x3411,
    SH_IRKEY_TV                    = 0x3E71,
    SH_IRKEY_RADIO                 = 0x3491,

    SH_IRKEY_INFO                  = 0x3511,
    SH_IRKEY_EPG                   = 0x3471,
    SH_IRKEY_SLEEP                 = 0x2341,
    SH_IRKEY_DISPLAY               = 0x2361,
    SH_IRKEY_DEMO                  = 0x3BB1,
    SH_IRKEY_OPC                   = 0x317E,
    SH_IRKEY_PICTURE               = 0x3F01,
    SH_IRKEY_ZOOM                  = 0x3911,
    SH_IRKEY_FREEZE                = 0x3A71,
    SH_IRKEY_PnP                   = 0x3A11,
    SH_IRKEY_SELECT2               = 0x39B1,

//--------------------------------------------------------------------------------------------
// New Sharp codes added
//--------------------------------------------------------------------------------------------
    SH_IRKEY_AUDIO                 = 0x2DC1,
    SH_IRKEY_RED2                  = 0x2901,
    SH_IRKEY_GREEN2                = 0x2921,
    SH_IRKEY_UPDATE                = 0x3E81,
    SH_IRKEY_TTX_MODE              = 0x3E81,
    SH_IRKEY_MIX                   = 0x3E81,
    SH_IRKEY_CLOCK                 = 0x39BE,
    SH_IRKEY_INDEX                 = 0x3E81,
    SH_IRKEY_DASH                  = 0x3E81,
    SH_IRKEY_TV_INPUT              = 0x3E71,
    SH_IRKEY_BACKWARD              = 0x3E81,
    SH_IRKEY_FORWARD               = 0x3E81,
    SH_IRKEY_PAGE_UP               = 0x3E81,
    SH_IRKEY_PAGE_DOWN             = 0x3E81,
    SH_IRKEY_PREVIOUS              = 0x3E81,
    SH_IRKEY_NEXT                  = 0x3E81,
    SH_IRKEY_BACK                  = 0x3C81,
    SH_IRKEY_CC                    = 0x3E81,
    SH_IRKEY_ADJUST                = 0x3E81,
    SH_IRKEY_KEY_DISABLE_KEYPAD    = 0x3E81,
    SH_IRKEY_RECORD                = 0x3E81,
    SH_IRKEY_STOP                  = 0x3E81,
    SH_IRKEY_PLAY                  = 0x3E81,
    SH_IRKEY_PAUSE                 = 0x3E81,
    SH_IRKEY_EXT2                  = 0x3DD1,
    SH_IRKEY_EXT5                  = 0x3EB1,
    SH_IRKEY_PC                    = 0x223E,
    SH_IRKEY_COMPONENT             = 0x3A41,
    SH_IRKEY_EXT1                  = 0x2641,
    SH_IRKEY_SV                    = 0x3E81,

    SH_IRKEY_SERVICE               = 0x3021,
    SH_IRKEY_KMODE_ON              = 0x2EE1,
    SH_IRKEY_KMODE_OFF             = 0x3D9E,
    SH_IRKEY_ADJUST1               = 0x2801,
    SH_IRKEY_ADJUST2               = 0x2621,
    SH_IRKEY_MCL                   = 0x2EA1,
    SH_IRKEY_MCL2                  = 0x35C1,
    SH_IRKEY_ROTATE                = 0x319E,
    SH_IRKEY_ROTATE2               = 0x2A7E,
    SH_IRKEY_OPC_ADJ               = 0x3971,
    SH_IRKEY_OPC_ADJ2              = 0x38B1,
    SH_IRKEY_ONECOLOR_WHITE        = 0x2D01,
    SH_IRKEY_DIGITS                = 0x2781,
    SH_IRKEY_BROWSE                = 0x25E1,
    SH_IRKEY_TAMP                  = 0x397E,
    SH_IRKEY_FLICKER               = 0x21BE,
    SH_IRKEY_CBIAS60               = 0x287E,
    SH_IRKEY_NORVIDEO              = 0x2421,
    SH_IRKEY_B_CUTOFF              = 0x28DE,
    SH_IRKEY_DTV892                = 0x3DBE,
    SH_IRKEY_SHARP_PROTOCOL        = 0x3E1E,
    SH_IRKEY_MSTAR_PROTOCOL        = 0x3E3E,
    SH_IRKEY_MENU30                = 0x3881,
    SH_IRKEY_A_PRESET              = 0x2B29,
    SH_IRKEY_2_PAGE                = 0x3C7E,
    SH_IRKEY_3_PAGE                = 0x3C9E,
    SH_IRKEY_4_PAGE                = 0x3CBE,
    SH_IRKEY_5_PAGE                = 0x3CDE,
    SH_IRKEY_6_PAGE                = 0x3CFE,
    SH_IRKEY_7_PAGE                = 0x3D1E,
    SH_IRKEY_8_PAGE                = 0x3D3E,
    SH_IRKEY_9_PAGE                = 0x3D5E,
    SH_IRKEY_10_PAGE               = 0x3D7E,
    SH_IRKEY_CARD                  = 0x391E,
    SH_IRKEY_SHIP_V                = 0x203E,
    SH_IRKEY_QSTEST                = 0x215E,
    SH_IRKEY_QS                    = 0x229E,
    SH_IRKEY_QSONOFF               = 0x227E,
    SH_IRKEY_COMPDIR               = 0x307E,
    SH_IRKEY_PCDIRECT              = 0x303E,
    SH_IRKEY_CARD_FORMAT           = 0x303E,
    SH_IRKEY_SHIP_E                = 0x3FC1,
    SH_IRKEY_SHIP_K                = 0x3501,
    SH_IRKEY_SHIP_I                = 0x3D01,
    SH_IRKEY_SHIP_F                = 0x3D21,
    SH_IRKEY_SHIP_R                = 0x3D61,
    SH_IRKEY_SHIP_S                = 0x3D81,
    SH_IRKEY_RADIO2                = 0x27E1,
    SH_IRKEY_LEDCHECK              = 0x3C61,
    SH_IRKEY_COMPOSITE             = 0x2C01,
    SH_IRKEY_ANALOGPC              = 0x3A81,
    SH_IRKEY_DOLBY                 = 0x25C1,
    SH_IRKEY_DIGIPC                = 0x3A01,
    SH_IRKEY_CENTERSP              = 0x25A1,
    SH_IRKEY_MMEMORY               = 0x2969,
    SH_IRKEY_AUDIO_OUT_FIX         = 0x333E,
    SH_IRKEY_RF_ACG                = 0x3821,
    SH_IRKEY_DCXO                  = 0x3061,
    SH_IRKEY_OSCCOLOR              = 0x2CA1,
    SH_IRKEY_EXT1_SVIDEO           = 0x3E9E,
    SH_IRKEY_EXT1_RGB              = 0x30BE,
    SH_IRKEY_EXT2_RGB              = 0x3E7E,
    SH_IRKEY_EXT3_SIDE             = 0x32F1,
    SH_IRKEY_EXT3_REAR             = 0x32B1,
    SH_IRKEY_EXT3_S                = 0x2C61,
    SH_IRKEY_ADC_EXT3              = 0x3DE1,
    SH_IRKEY_EXT4                  = 0x3E91,
    SH_IRKEY_EXT6                  = 0x3F11,
    SH_IRKEY_INPUT2                = 0x3E5E,
    SH_IRKEY_INPUT3                = 0x3DF1,
    SH_IRKEY_INPUT4_AVIN           = 0x339E,
    SH_IRKEY_INPUT4_EXT4_SD        = 0x3A61,
    SH_IRKEY_VIDEO1                = 0x3DB1,
    SH_IRKEY_SHIMI1_SHIWA          = 0x2A9E,
    SH_IRKEY_SHIMI2_SHIWA          = 0x26DE,
    SH_IRKEY_PCINVGA               = 0x225E,
    SH_IRKEY_COMP_HD_ADJ           = 0x38A1,
    SH_IRKEY_BALANCE               = 0x29C1,
    SH_IRKEY_BKGD_PRESETTINGS      = 0x3F3E,
    SH_IRKEY_VIDEO_AGC             = 0x3EFE,
    SH_IRKEY_COM1080               = 0x3F1E,
    SH_IRKEY_3D_Y_C                = 0x3EF1,
    SH_IRKEY_SVIDEO                = 0x2C21,
    SH_IRKEY_COMP15K_4LINE         = 0x3DFE,
    SH_IRKEY_COMP_SD_ADJ           = 0x3861,
    SH_IRKEY_RGB_ADJ               = 0x3841,
    SH_IRKEY_AVLINK2               = 0x3B91,
    SH_IRKEY_26INCH                = 0x30E1,
    SH_IRKEY_32INCH                = 0x3101,
    SH_IRKEY_37INCH                = 0x3121,
    SH_IRKEY_42INCH                = 0x3141,
    SH_IRKEY_HDMI_CEC              = 0x2C1E,
    SH_IRKEY_5V_OFF                = 0x3AA1,
    SH_IRKEY_5V_ON                 = 0x3AC1,
    SH_IRKEY_DTV992                = 0x2761,
    SH_IRKEY_DTV95                 = 0x3F31,
    SH_IRKEY_HDD_ON                = 0x2CBE,
    SH_IRKEY_HDD_OFF               = 0x2CDE,
    SH_IRKEY_PAUSE2                = 0x27D1,
    SH_IRKEY_AUDIODIG_ON_OFF       = 0x2D1E,
    SH_IRKEY_DVI_AUDIO             = 0x2A5E,
    SH_IRKEY_HDMI_AUDIO            = 0x2A3E,
    SH_IRKEY_PP_HDMI_WR_ENABLE     = 0x3F9E,
    SH_IRKEY_MAGILINK_OFF          = 0x2C9E,
    SH_IRKEY_MAGILINK_ON           = 0x2C7E,
    SH_IRKEY_USB_INSPECTION        = 0x293E,
    SH_IRKEY_SEES_SRC              = 0x3FE1,
    SH_IRKEY_HOTEL_MODE            = 0x38FE,
    SH_IRKEY_PC_RGB_ADJ            = 0x233E,
    SH_IRKEY_EXT3_SD_ADJ           = 0x221E,
    SH_IRKEY_EXT3_HD_ADJ           = 0x213E,
    SH_IRKEY_TACTSW19              = 0x2ABE,
    SH_IRKEY_TACTSW37              = 0x2AA1,
    SH_IRKEY_MODEL_SETTING         = 0x30A1,
    SH_IRKEY_DTV980                = 0x2E51,
    SH_IRKEY_CH25_E10              = 0x2A11,
    SH_IRKEY_CH26_E12              = 0x2A31,
    SH_IRKEY_RGB_SCART_ADJ         = 0x38C1,
    SH_IRKEY_COMP33K               = 0x3921,
    SH_IRKEY_PC_ANALOG_ADJ         = 0x3941,
    SH_IRKEY_ADJ_SECAM             = 0x3941,
    SH_IRKEY_DTV_MENU              = 0x3431,
    SH_IRKEY_CH23_E5               = 0x29D1,
    SH_IRKEY_CH24_OIRT8            = 0x29F1,
    SH_IRKEY_CH33_I69              = 0x2A51,
    SH_IRKEY_CH34_F4               = 0x2A71,
    SH_IRKEY_CH35_F10              = 0x2A91,
}IR_CUS21SH_RAWDATA_TABLE;

#else
//code U16 IR_CUS21SH_RAWDATA_TABLE[]= //[205] =
U16 IR_CUS21SH_RAWDATA_TABLE[] =
{
    0x2141, //IRKEY_NUM_0                =0x00
    0x2021, //IRKEY_NUM_1                =0x01
    0x2041, //IRKEY_NUM_2                =0x02
    0x2061, //IRKEY_NUM_3                =0x03
    0x2081, //IRKEY_NUM_4                =0x04
    0x20A1, //IRKEY_NUM_5                =0x05
    0x20C1, //IRKEY_NUM_6                =0x06
    0x20E1, //IRKEY_NUM_7                =0x07
    0x2101, //IRKEY_NUM_8                =0x08
    0x2121, //IRKEY_NUM_9                =0x09
    0x2AE1, //IRKEY_UP                   =0x0A
    0x22C1, //IRKEY_POWER                =0x0B
    0x3EA1, //IRKEY_EXIT                 =0x0C
    0x38D1, //IRKEY_MENU                 =0x0D
    0x2401, //IRKEY_DOWN                 =0x0E
    0x3AE1, //IRKEY_LEFT                 =0x0F
    0x3B01, //IRKEY_RIGHT                =0x10
    0x2A41, //IRKEY_SELECT               =0x11
    0x3C81, //IRKEY_RETURN               =0x12
    0x34F1, //IRKEY_CHANNEL_LIST         =0x13
    0x3451, //IRKEY_CHANNEL_FAV_LIST     =0x14
    0x2221, //IRKEY_CHANNEL_PLUS         =0x15
    0x2241, //IRKEY_CHANNEL_MINUS        =0x16
    0x24E1, //IRKEY_CHANNEL_RETURN       =0x17
    0x2901, //IRKEY_RED                  =0x18
    0x2921, //IRKEY_GREEN                =0x19
    0x2941, //IRKEY_YELLOW               =0x1A
    0x2961, //IRKEY_BLUE                 =0x1B
    0x22E1, //IRKEY_MUTE                 =0x1C
    0x2301, //IRKEY_MTS                  =0x1D
    0x2281, //IRKEY_VOLUME_PLUS          =0x1E
    0x22A1, //IRKEY_VOLUME_MINUS         =0x1F
    0x2681, //IRKEY_TTX                  =0x20
    0x2701, //IRKEY_REVEAL               =0x21
    0x33FE, //IRKEY_SUBTITLE             =0x22
    0x2661, //IRKEY_HOLD                 =0x23
    0x39BE, //IRKEY_SUBPAGE              =0x24
    0x26E1, //IRKEY_SIZE                 =0x25
    0x2261, //IRKEY_INPUT_SOURCE         =0x26
    0x3411, //IRKEY_DTV                  =0x27
    0x3E71, //IRKEY_TV                   =0x28
    0x3491, //IRKEY_RADIO                =0x29
    0x3511, //IRKEY_INFO                 =0x2A
    0x3471, //IRKEY_EPG                  =0x2B
    0x2341, //IRKEY_SLEEP                =0x2C
    0x2361, //IRKEY_DISPLAY              =0x2D    // This is the IRKEY_INFO code
    0x3BB1, //IRKEY_DEMO                 =0x2E
    0x317E, //IRKEY_OPC                  =0x2F
    0x3F01, //IRKEY_PICTURE              =0x30    // This is the AV_MODE code
    0x3911, //IRKEY_ZOOM                 =0x31
    0x3A71, //IRKEY_FREEZE               =0x32
    0x3A11, //IRKEY_PnP                  =0x33
    0x39B1, //IRKEY_SELECT2              =0x34

//--------------------------------------------------------------------------------------------
// New Sharp codes added
//--------------------------------------------------------------------------------------------
    0x2DC1, //IRKEY_AUDIO                =0x35, (DOLBY SRS SURROUND)
    0x2901, //IRKEY_RED2                =0x36, (RED)
    0x2921, //IRKEY_GREEN2                =0x37, (GREEN)
    0x3E81, //IRKEY_UPDATE                =0x38, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_TTX_MODE            =0x39, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_MIX                    =0x3A, (Assigned VCT code -not used-)
    0x39BE, //IRKEY_CLOCK                =0x3B, (SUBPAGES)
    0x3E81, //IRKEY_INDEX                =0x3C, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_DASH                =0x3D, (Assigned VCT code -not used-)
    0x3E71, //IRKEY_TV_INPUT            =0x3E, (ATV)
    0x3E81, //IRKEY_BACKWARD            =0x3F, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_FORWARD                =0x40, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_PAGE_UP                =0x41, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_PAGE_DOWN            =0x42, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_PREVIOUS            =0x43, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_NEXT                 =0x44, (Assigned VCT code -not used-)
    0x3C81, //IRKEY_BACK                =0x45, (RETURN)
    0x3E81, //IRKEY_CC                    =0x46, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_ADJUST                =0x47, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_KEY_DISABLE_KEYPAD    =0x48, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_RECORD                =0x49, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_STOP                =0x4A, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_PLAY                =0x4B, (Assigned VCT code -not used-)
    0x3E81, //IRKEY_PAUSE                =0x4C, (Assigned VCT code -not used-)
    0x3DD1, //IRKEY_EXT2                =0x4D, (EXT2)
    0x3EB1, //IRKEY_EXT5                =0x4E (EXT5_HDMI)
    0x223E, //IRKEY_PC                    =0x4F,(PCIN_XGA)
    0x3A41, //IRKEY_COMPONENT            =0x50, (EXT4_HD_COMP)
    0x2641, //IRKEY_EXT1                    =0x51, (EXT1)
    0x3E81, //IRKEY_SV                    =0x52, (Assigned VCT code -not used-)

    0x3021, //IRKEY_SERVICE              =0x53
    0x2EE1, //IRKEY_KMODE_ON              =0x54
    0x3D9E, //IRKEY_KMODE_OFF             =0x55
    0x2801, //IRKEY_ADJUST1             =0x56
    0x2621, //IRKEY_ADJUST2             =0x57
    0x2EA1, //IRKEY_MCL                     =0x58
    0x35C1, //IRKEY_MCL2                 =0x59
    0x319E, //IRKEY_ROTATE                 =0x5A
    0x2A7E, //IRKEY_ROTATE2                 =0x5B
    0x3971, //IRKEY_OPC_ADJ                 =0x5C
    0x38B1, //IRKEY_OPC_ADJ2             =0x5D
    0x2D01, //IRKEY_ONECOLOR_WHITE        =0x5E
    0x2781, //IRKEY_DIGITS                 =0x5F
    0x25E1, //IRKEY_BROWSE                 =0x60
    0x397E, //IRKEY_TAMP                 =0x61
    0x21BE, //IRKEY_FLICKER                 =0x62
    0x287E, //IRKEY_CBIAS60                 =0x63
    0x2421, //IRKEY_NORVIDEO             =0x64
    0x28DE, //IRKEY_B_CUTOFF             =0x65
    0x3DBE, //IRKEY_DTV892                 =0x66
    0x3E1E, //IRKEY_SHARP_PROTOCOL                 =0x67
    0x3E3E, //IRKEY_MSTAR_PROTOCOL                 =0x68
    0x3881, //IRKEY_MENU30                 =0x69
    0x2B29, //IRKEY_A_PRESET             =0x6A
    0x3C7E, //IRKEY_2_PAGE                 =0x6B
    0x3C9E, //IRKEY_3_PAGE                 =0x6C
    0x3CBE, //IRKEY_4_PAGE                 =0x6D
    0x3CDE, //IRKEY_5_PAGE                 =0x6E
    0x3CFE, //IRKEY_6_PAGE                 =0x6F
    0x3D1E, //IRKEY_7_PAGE                 =0x70
    0x3D3E, //IRKEY_8_PAGE                 =0x71
    0x3D5E, //IRKEY_9_PAGE                 =0x72
    0x3D7E, //IRKEY_10_PAGE                 =0x73
    0x391E, //IRKEY_CARD                 =0x74
    0x203E, //IRKEY_SHIP_V                 =0x75
    0x215E, //IRKEY_QSTEST                 =0x76
    0x229E, //IRKEY_QS                     =0x77
    0x227E, //IRKEY_QSONOFF                 =0x78
    0x307E, //IRKEY_COMPDIR             =0x79
    0x303E, //IRKEY_PCDIRECT             =0x7A
    0x303E, //IRKEY_CARD_FORMAT            =0x7B
    0x3FC1, //IRKEY_SHIP_E                =0x7C
    0x3501, //IRKEY_SHIP_K                =0x7D
    0x3D01, //IRKEY_SHIP_I                =0x7E
    0x3D21, //IRKEY_SHIP_F                =0x7F
    0x3D61, //IRKEY_SHIP_R                =0x80
    0x3D81, //IRKEY_SHIP_S                =0x81
    0x27E1, //IRKEY_RADIO2                =0x82
    0x3C61, //IRKEY_LEDCHECK            =0x83
    0x2C01, //IRKEY_COMPOSITE            =0x84
    0x3A81, //IRKEY_ANALOGPC            =0x85
    0x25C1, //IRKEY_DOLBY                =0x86
    0x3A01, //IRKEY_DIGIPC                =0x87
    0x25A1, //IRKEY_CENTERSP            =0x88
    0x2969, //IRKEY_MMEMORY                =0x89
    0x333E, //IRKEY_AUDIO_OUT_FIX          =0x8A
    0x3821, //IRKEY_RF_ACG                  =0x8B
    0x3061, //IRKEY_DCXO                  =0x8C
    0x2CA1, //IRKEY_OSCCOLOR              =0x8D
    0x3E9E, //IRKEY_EXT1_SVIDEO              =0x8E
    0x30BE, //IRKEY_EXT1_RGB              =0x8F
    0x3E7E, //IRKEY_EXT2_RGB              =0x90
    0x32F1, //IRKEY_EXT3_SIDE              =0x91
    0x32B1, //IRKEY_EXT3_REAR              =0x92
    0x2C61, //IRKEY_EXT3_S                  =0x93
    0x3DE1, //IRKEY_ADC_EXT3              =0x94
    0x3E91, //IRKEY_EXT4                  =0x95
    0x3F11, //IRKEY_EXT6              =0x96
    0x3E5E, //IRKEY_INPUT2              =0x97
    0x3DF1, //IRKEY_INPUT3              =0x98
    0x339E, //IRKEY_INPUT4_AVIN            =0x99
    0x3A61, //IRKEY_INPUT4_EXT4_SD         =0x9A
    0x3DB1, //IRKEY_VIDEO1                 =0x9B
    0x2A9E, //IRKEY_SHIMI1_SHIWA        =0x9C
    0x26DE, //IRKEY_SHIMI2_SHIWA        =0x9D
    0x225E, //IRKEY_PCINVGA                =0x9E
    0x38A1, //IRKEY_COMP_HD_ADJ            =0x9F
    0x29C1, //IRKEY_BALANCE                =0xA0
    0x3F3E, //IRKEY_BKGD_PRESETTINGS    =0xA1
    0x3EFE, //IRKEY_VIDEO_AGC            =0xA2
    0x3F1E, //IRKEY_COM1080                =0xA3
    0x3EF1, //IRKEY_3D_Y_C                =0xA4
    0x2C21, //IRKEY_SVIDEO                =0xA5
    0x3DFE, //IRKEY_COMP15K_4LINE        =0xA6
    0x3861, //IRKEY_COMP_SD_ADJ             =0xA7
    0x3841, //IRKEY_RGB_ADJ                =0xA8
    0x3B91, //IRKEY_AVLINK2                =0xA9
    0x30E1, //IRKEY_26INCH                =0xAA
    0x3101, //IRKEY_32INCH                =0xAB
    0x3121, //IRKEY_37INCH                =0xAC
    0x3141, //IRKEY_42INCH                =0xAD
    0x2C1E, //IRKEY_HDMI_CEC            =0xAE
    0x3AA1, //IRKEY_5V_OFF                =0xAF
    0x3AC1, //IRKEY_5V_ON                =0xB0
    0x2761, //IRKEY_DTV992            =0xB1
    0x3F31, //IRKEY_DTV95                    =0xB2
    0x2CBE, //IRKEY_HDD_ON                =0xB3
    0x2CDE, //IRKEY_HDD_OFF                =0xB4
    0x27D1, //IRKEY_PAUSE2                =0xB5
    0x2D1E, //IRKEY_AUDIODIG_ON_OFF        =0xB6
    0x2A5E, //IRKEY_DVI_AUDIO        =0xB7
    0x2A3E, //IRKEY_HDMI_AUDIO        =0xB8
    0x3F9E, //IRKEY_PP_HDMI_WR_ENABLE    =0xB9
    0x2C9E, //IRKEY_MAGILINK_OFF        =0xBA
    0x2C7E, //IRKEY_MAGILINK_ON            =0xBB
    0x293E, //IRKEY_USB_INSPECTION        =0xBC
    0x3FE1, //IRKEY_SEES_SRC            =0xBD
    0x38FE, //IRKEY_HOTEL_MODE            =0xBE
    0x233E, //IRKEY_PC_RGB_ADJ            =0xBF
    0x221E, //IRKEY_EXT3_SD_ADJ            =0xC0
    0x213E, //IRKEY_EXT3_HD_ADJ            =0xC1
    0x2ABE, //IRKEY_TACTSW19            =0xC2
    0x2AA1, //IRKEY_TACTSW37            =0xC3
    0x30A1, //IRKEY_MODEL_SETTING        =0xC4
    0x2E51, //IRKEY_DTV980                =0xC5
    0x2A11, //IRKEY_CH25_E10            =0xC6
    0x2A31, //IRKEY_CH26_E12            =0xC7
    0x38C1, //IRKEY_RGB_SCART_ADJ        =0xC8
    0x3921, //IRKEY_COMP33K                =0xC9
    0x3941, //IRKEY_PC_ANALOG_ADJ        =0xCA
    0x3941, //IRKEY_ADJ_SECAM            =0xCB
    0x3431, //IRKEY_DTV_MENU            =0xCC
    0x29D1, //IRKEY_CH23_E5                = 0xCD,
    0x29F1, //IRKEY_CH24_OIRT8                = 0xCE,
    0x2A51, //IRKEY_CH33_I69                = 0xCF,
    0x2A71, //IRKEY_CH34_F4                = 0xD0,
    0x2A91  //IRKEY_CH35_F10                = 0xD1,
};
#endif

#if 1
typedef enum _IrCommandType
{
    IRKEY_TV_RADIO          = 0x0C,
    IRKEY_CHANNEL_LIST      = 0x10,
    IRKEY_CHANNEL_FAV_LIST  = 0x08,
    IRKEY_CHANNEL_RETURN    = 0x5C,
    IRKEY_CHANNEL_PLUS      = 0x1F,
    IRKEY_CHANNEL_MINUS     = 0x19,

    IRKEY_AUDIO             = 0x44,
    IRKEY_VOLUME_PLUS       = 0x16,
    IRKEY_VOLUME_MINUS      = 0x15,

    IRKEY_UP                = 0x52,
    IRKEY_POWER             = 0x46,
    IRKEY_EXIT              = 0x1B,
    IRKEY_MENU              = 0x07,
    IRKEY_DOWN              = 0x13,
    IRKEY_LEFT              = 0x06,
    IRKEY_SELECT            = 0x0F,
    IRKEY_RIGHT             = 0x1A,

    IRKEY_NUM_0             = 0x50,
    IRKEY_NUM_1             = 0x49,
    IRKEY_NUM_2             = 0x55,
    IRKEY_NUM_3             = 0x59,
    IRKEY_NUM_4             = 0x4D,
    IRKEY_NUM_5             = 0x51,
    IRKEY_NUM_6             = 0x5D,
    IRKEY_NUM_7             = 0x48,
    IRKEY_NUM_8             = 0x54,
    IRKEY_NUM_9             = 0x58,

    IRKEY_MUTE              = 0x5A,
    IRKEY_PAGE_UP           = 0x03,
    IRKEY_PAGE_DOWN         = 0x05,
    IRKEY_CLOCK             = 0x5F,

    IRKEY_INFO              = 0x14,
    IRKEY_RED               = 0x47,
    IRKEY_GREEN             = 0x4B,
    IRKEY_YELLOW            = 0x57,
    IRKEY_BLUE              = 0x5B,
    IRKEY_MTS               = 0x41,
    IRKEY_NINE_LATTICE      = IRKEY_DUMY,
    IRKEY_TTX               = 0x0A,
    IRKEY_CC                = 0x09,
    IRKEY_INPUT_SOURCE      = 0x04,
    IRKEY_CRADRD            = IRKEY_DUMY-1,
//    IRKEY_PICTURE           = 0x40,
    IRKEY_ZOOM              = 0x4C,
    IRKEY_DASH              = 0x4E,
    IRKEY_SLEEP             = 0x45,
    IRKEY_EPG               = 0x4A,
    IRKEY_PIP               = 0x40,

  	IRKEY_MIX               = 0x1C,
    IRKEY_INDEX             = 0x18,
    IRKEY_HOLD              = 0x00,
    IRKEY_PREVIOUS          = 0x0E,
    IRKEY_NEXT              = 0x12,
    IRKEY_BACKWARD          = 0x02,
    IRKEY_FORWARD           = 0x1E,
    IRKEY_PLAY              = 0x01,
    IRKEY_RECORD            = 0x0D,
    IRKEY_STOP              = 0x11,
    IRKEY_PAUSE             = 0x1D,

    IRKEY_SIZE              = 0x43,
    IRKEY_REVEAL            = 0x4F,
    IRKEY_SUBCODE           = 0x53,
}IrCommandType;
#else
typedef enum _IrCommandType
{
    IRKEY_NUM_0                 = 0x50,//  = 0x0A,0x2141
    IRKEY_NUM_1                 = 0x49,//  = 0x01,0x2021
    IRKEY_NUM_2                 = 0x55,//  = 0x02,0x2041
    IRKEY_NUM_3                 = 0x59,//  = 0x03,0x2061
    IRKEY_NUM_4                 = 0x4D,//  = 0x04,0x2081
    IRKEY_NUM_5                 = 0x51,//  = 0x05,0x20A1
    IRKEY_NUM_6                 = 0x5D,//  = 0x06,0x20C1
    IRKEY_NUM_7                 = 0x48,//  = 0x07,0x20E1
    IRKEY_NUM_8                 = 0x54,//  = 0x08,0x2101
    IRKEY_NUM_9                 = 0x58,//  = 0x09,0x2121
    IRKEY_UP                    = 0x52,//  = 0x57,0x2AE1
    IRKEY_POWER                 = 0x46,//  = 0x16,0x22C1
    IRKEY_EXIT                  = 0x1B,//  = 0xF5,0x3EA1
    IRKEY_MENU                  = 0x07,//  = 0xC6,0x38D1
    IRKEY_DOWN                  = 0x13,//  = 0x20,0x2041
    IRKEY_LEFT                  = 0x06,//  = 0xD7,0x3AE1
    IRKEY_RIGHT                 = 0x1A,//  = 0xD8,0x3B01
    IRKEY_SELECT                = 0x0F,//  = 0x52,0x2A41
    IRKEY_RETURN                = IRKEY_DUMY-0,//  = 0xE4,0x3C81

    // channel keys
    IRKEY_CHANNEL_LIST          = 0x10,//  = 0xA7,0x34F1
    IRKEY_CHANNEL_FAV_LIST      = 0x08,//  = 0xA9,0x3531
    IRKEY_CHANNEL_PLUS          = 0x1F,//  = 0x11,0x2221
    IRKEY_CHANNEL_MINUS         = 0x19,//  = 0x12,0x2241
    IRKEY_CHANNEL_RETURN        = 0x5C,//  = 0x27,0x24E1

    // color keys
    IRKEY_RED                   = 0x47,//  = 0x48,0x2901
    IRKEY_GREEN                 = 0x4B,//  = 0x49,0x2921
    IRKEY_YELLOW                = 0x57,//  = 0x4A,0x2941
    IRKEY_BLUE                  = 0x5B,//  = 0x4B,0x2961

    // audio keys
    IRKEY_MUTE                  = 0x5A,//  = 0x17,0x22E1
    IRKEY_MTS                   = 0x41,//  = 0x18,0x2301
    IRKEY_VOLUME_PLUS           = 0x16,//  = 0x14,0x2281
    IRKEY_VOLUME_MINUS          = 0x15,//  = 0x15,0x22A1

    // TTX keys
    IRKEY_TTX                   = 0x0A,//  = 0x34,0x2681
    IRKEY_REVEAL                = 0x4F,//  = 0x38,0x2701
    IRKEY_SUBTITLE              = 0x53,//  = 0x9F,0x33FE
    IRKEY_HOLD                  = 0x00,//  = 0x33,0x2661
    IRKEY_SUBPAGE               = IRKEY_DUMY-1,//  = 0xCD,0x39BE
    IRKEY_SIZE                  = 0x43,//  = 0x37,0x26E1

    // input source keys
    IRKEY_INPUT_SOURCE          = 0x04,//  = 0x13,0x2261
    IRKEY_DTV                   = IRKEY_DUMY-2,//  = 0xA0,0x3411
    IRKEY_TV                    = IRKEY_DUMY-3,//  = 0xF3, 0x3E71
    IRKEY_RADIO                 = 0x0C,//  = 0xA4,0x3491

    // EPG keys
    IRKEY_INFO                  = 0x14,//  = 0xA8,0x3511
    IRKEY_EPG                   = 0x4A,//  = 0xA3,0x3471

    // misc
    IRKEY_SLEEP                  = 0x45,//  = 0x1A,0x2341
    IRKEY_DISPLAY                = IRKEY_DUMY-4,//  = 0x1B,0x2361

    // advanced keys
    IRKEY_DEMO                    = 0x2E,//  = 0xDD,3BB1
    IRKEY_OPC                     = 0x2F,//  = 0x8B,317E
    IRKEY_PICTURE                = 0x40,//  = 0xF8,3F01
    IRKEY_ZOOM                    = 0x4C,//  = 0xC8,3911
    IRKEY_FREEZE                = IRKEY_DUMY-5,//  = 0xD3,3A71
    IRKEY_PnP                    = IRKEY_DUMY-6,//  = 0xD0,3A11
    IRKEY_SELECT2                = IRKEY_DUMY-7,//  = 0xCD,39B1

    IRKEY_AUDIO                    = 0x44,
    IRKEY_RED2                    = IRKEY_DUMY-8,
    IRKEY_GREEN2                = IRKEY_DUMY-9,
    IRKEY_UPDATE                = IRKEY_DUMY-10,
    IRKEY_TTX_MODE                = IRKEY_DUMY-11,
    IRKEY_MIX                    = 0x1C,
    IRKEY_CLOCK                    = 0x5F,
    IRKEY_INDEX                    = 0x18,
    IRKEY_DASH                    = 0x4E,
    IRKEY_TV_INPUT                = IRKEY_DUMY-12,
    IRKEY_BACKWARD                = 0x02,
    IRKEY_FORWARD                = 0x1E,
    IRKEY_PAGE_UP                = 0x03,
    IRKEY_PAGE_DOWN                = 0x05,
    IRKEY_PREVIOUS                = 0x0E,
    IRKEY_NEXT                     = 0x12,
    IRKEY_BACK                    = IRKEY_DUMY-13,
    IRKEY_CC                    = 0x09,
    IRKEY_ADJUST                = IRKEY_DUMY-14,
    IRKEY_KEY_DISABLE_KEYPAD    = IRKEY_DUMY-15,
    IRKEY_RECORD                = 0x0D,
    IRKEY_STOP                    = 0x11,
    IRKEY_PLAY                    = 0x01,
    IRKEY_PAUSE                    = 0x1D,
    IRKEY_EXT2                    = IRKEY_DUMY-16,
    IRKEY_EXT5                    = IRKEY_DUMY-17,
    IRKEY_PC                    = IRKEY_DUMY-18,
    IRKEY_COMPONENT                = IRKEY_DUMY-19,
    IRKEY_EXT1                    = IRKEY_DUMY-20,
    IRKEY_SV                    = IRKEY_DUMY-21,

    IRKEY_SERVICE                  = IRKEY_DUMY-22,
    IRKEY_KMODE_ON              = IRKEY_DUMY-23,
    IRKEY_KMODE_OFF             = IRKEY_DUMY-24,
    IRKEY_ADJUST1                 = IRKEY_DUMY-25,
    IRKEY_ADJUST2                 = IRKEY_DUMY-26,
    IRKEY_MCL                     = IRKEY_DUMY-27,
    IRKEY_MCL2                     = IRKEY_DUMY-28,
    IRKEY_ROTATE                 = IRKEY_DUMY-29,
    IRKEY_ROTATE2                 = IRKEY_DUMY-30,
    IRKEY_OPC_ADJ                 = IRKEY_DUMY-31,
    IRKEY_OPC_ADJ2                 = IRKEY_DUMY-32,/*
    IRKEY_ONECOLOR_WHITE        = 0x5E,
    IRKEY_DIGITS                 = 0x5F,
    IRKEY_BROWSE                 = 0x60,
    IRKEY_TAMP                     = 0x61,
    IRKEY_FLICKER                 = 0x62,
    IRKEY_CBIAS60                 = 0x63,
    IRKEY_NORVIDEO                 = 0x64,
    IRKEY_B_CUTOFF                 = 0x65,
    IRKEY_DTV892                 = 0x66,
    IRKEY_SHARP_PROTOCOL         = 0x67,
    IRKEY_MSTAR_PROTOCOL         = 0x68,
    IRKEY_MENU30                 = 0x69,
    IRKEY_A_PRESET                 = 0x6A,
    IRKEY_2_PAGE                 = 0x6B,
    IRKEY_3_PAGE                 = 0x6C,
    IRKEY_4_PAGE                 = 0x6D,
    IRKEY_5_PAGE                 = 0x6E,
    IRKEY_6_PAGE                 = 0x6F,
    IRKEY_7_PAGE                 = 0x70,
    IRKEY_8_PAGE                 = 0x71,
    IRKEY_9_PAGE                 = 0x72,
    IRKEY_10_PAGE                 = 0x73,
    IRKEY_CARD                     = 0x74,
    IRKEY_SHIP_V                 = 0x75,
    IRKEY_QSTEST                 = 0x76,
    IRKEY_QS                     = 0x77,
    IRKEY_QSONOFF                 = 0x78,
    IRKEY_COMPDIR                 = 0x79,
    IRKEY_PCDIRECT                 = 0x7A,
    IRKEY_CARD_FORMAT            = 0x7B,
    IRKEY_SHIP_E                = 0x7C,
    IRKEY_SHIP_K                = 0x7D,
    IRKEY_SHIP_I                = 0x7E,
    IRKEY_SHIP_F                = 0x7F,
    IRKEY_SHIP_R                = 0x80,
    IRKEY_SHIP_S                = 0x81,
    IRKEY_RADIO2                = 0x82,
    IRKEY_LEDCHECK                = 0x83,
    IRKEY_COMPOSITE                = 0x84,
    IRKEY_ANALOGPC                = 0x85,
    IRKEY_DOLBY                    = 0x86,
    IRKEY_DIGIPC                = 0x87,
    IRKEY_CENTERSP                = 0x88,
    IRKEY_MMEMORY                = 0x89,
    IRKEY_AUDIO_OUT_FIX          = 0x8A,
    IRKEY_RF_ACG                  = 0x8B,
    IRKEY_DCXO                      = 0x8C,
    IRKEY_OSCCOLOR                  = 0x8D,
    IRKEY_EXT1_SVIDEO              = 0x8E,
    IRKEY_EXT1_RGB                  = 0x8F,
    IRKEY_EXT2_RGB                  = 0x90,
    IRKEY_EXT3_SIDE                  = 0x91,
    IRKEY_EXT3_REAR                  = 0x92,
    IRKEY_EXT3_S                  = 0x93,
    IRKEY_ADC_EXT3              = 0x94,
    IRKEY_EXT4                      = 0x95,
    IRKEY_EXT6                  = 0x96,
    IRKEY_INPUT2                  = 0x97,
    IRKEY_INPUT3                  = 0x98,
    IRKEY_INPUT4_AVIN            = 0x99,
    IRKEY_INPUT4_EXT4_SD         = 0x9A,
    IRKEY_VIDEO1                 = 0x9B,
    IRKEY_SHIMI1_SHIWA            = 0x9C,
    IRKEY_SHIMI2_SHIWA            = 0x9D,
    IRKEY_PCINVGA                = 0x9E,
    IRKEY_COMP_HD_ADJ              = 0x9F,
    IRKEY_BALANCE                = 0xA0,
    IRKEY_BKGD_PRESETTINGS        = 0xA1,
    IRKEY_VIDEO_AGC                = 0xA2,
    IRKEY_COM1080                = 0xA3,
    IRKEY_3D_Y_C                = 0xA4,
    IRKEY_SVIDEO                = 0xA5,
    IRKEY_COMP15K_4LINE            = 0xA6,
    IRKEY_COMP_SD_ADJ            = 0xA7,
    IRKEY_RGB_ADJ                = 0xA8,
    IRKEY_AVLINK2                = 0xA9,
    IRKEY_26INCH                = 0xAA,
    IRKEY_32INCH                = 0xAB,
    IRKEY_37INCH                = 0xAC,
    IRKEY_42INCH                = 0xAD,
    IRKEY_HDMI_CEC                = 0xAE,
    IRKEY_5V_OFF                = 0xAF,
    IRKEY_5V_ON                    = 0xB0,
    IRKEY_DTV992                = 0xB1,
    IRKEY_DTV95                    = 0xB2,
    IRKEY_HDD_ON                = 0xB3,
    IRKEY_HDD_OFF                = 0xB4,
    IRKEY_PAUSE2                = 0xB5,
    IRKEY_AUDIODIG_ON_OFF        = 0xB6,
    IRKEY_DVI_AUDIO            = 0xB7,
    IRKEY_HDMI_AUDIO            = 0xB8,
    IRKEY_PP_HDMI_WR_ENABLE        = 0xB9,
    IRKEY_MAGILINK_OFF            = 0xBA,
    IRKEY_MAGILINK_ON            = 0xBB,
    IRKEY_USB_INSPECTION        = 0xBC,
    IRKEY_SEES_SRC                = 0xBD,
    IRKEY_HOTEL_MODE            = 0xBE,
    IRKEY_PC_RGB_ADJ            = 0xBF,
    IRKEY_EXT3_SD_ADJ            = 0xC0,
    IRKEY_EXT3_HD_ADJ            = 0xC1,
    IRKEY_TACTSW19                = 0xC2,
    IRKEY_TACTSW37                = 0xC3,
    IRKEY_MODEL_SETTING            = 0xC4,
    IRKEY_DTV980                = 0xC5,
    IRKEY_CH25_E10                = 0xC6,
    IRKEY_CH26_E12                = 0xC7,
    IRKEY_RGB_SCART_ADJ            = 0xC8,
    IRKEY_COMP33K                = 0xC9,
    IRKEY_PC_ANALOG_ADJ            = 0xCA,
    IRKEY_ADJ_SECAM                = 0xCB,
    IRKEY_DTV_MENU                = 0xCC,
    IRKEY_CH23_E5                = 0xCD,
    IRKEY_CH24_OIRT8                = 0xCE,
    IRKEY_CH33_I69                = 0xCF,
    IRKEY_CH34_F4                = 0xD0,
    IRKEY_CH35_F10              = 0xD1,
*/
    // Define in MStar Key code not define here
    IRKEY_NINE_LATTICE          = IRKEY_DUMY-33,
    IRKEY_CRADRD                = IRKEY_DUMY-34,
    IRKEY_TV_RADIO              = IRKEY_DUMY-35, // 0x0C
    IRKEY_PIP                   = IRKEY_DUMY-36, // 0x40
    IRKEY_SUBCODE               = IRKEY_DUMY-37, // 0x53

}IrCommandType;
#endif
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

#define irGetCnt_BOOT(time)                 ((u32)((double)time*((double)IR_CLK_BOOT)/(IR_CKDIV_NUM_BOOT+1))+0x300000UL)
#define irGetCnt(time)                      ((u32)((double)time*((double)IR_CLK)/(IR_CKDIV_NUM+1))+0x300000UL)

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


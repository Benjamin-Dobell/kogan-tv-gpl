////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Copyright? 2000 Philips Consumer Electronics
// Program : RC6/RCMM Decoding (subset of JADE)
// Filename : IR_Rcmm.h
// Language : Ansi C
// Date : 15 April 2005
///////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __IR_RCMM_H__
#define __IR_RCMM_H__
/////////////////////////////////////////////////////////////////

#include <asm/types.h>//@@@
#include "mdrv_types.h"//@@@


//////////////////////////////////////////////////////////////////
#define RC_MODE 0x01
#define MOUSE_MODE 0x02
#define KEYBOARD_MODE 0x04
#define JOYSTICK_MODE 0x08
#define RC6_MODE_0 0x00
#define RC6_MODE_1A 0x01
#define RC6_MODE_2A 0x02
#define RC6_MODE_6ALOWPOWER 0x03
#define RC6_MODE_6AOEMCMD 0x04
#define RC6_MODE_6AOEMSTR 0x05
#define RC6_MODE_6B 0x06
#define RCMMBASIC_MODE 0x10
#define RCMMOEM_SHORTID_MODE 0x20
#define RCMMOEM_LONGID_MODE 0x40
#define RC6_MODE_FLAG 0x80 //short OEM command mode RC, and RC6+ Keyboard, mouse mode
#define RC6_HEADERLEN 8
#define RC6_MODE_0_DATALEN 16
#define RC6_MODE_0_FRAMELEN (RC6_HEADERLEN + RC6_MODE_0_DATALEN -1)
#define RC6_MODE_1A_DATALEN 32
#define RC6_MODE_1A_FRAMELEN (RC6_HEADERLEN + RC6_MODE_1A_DATALEN -1)
#define RC6_MODE_2A_DATALEN 32
#define RC6_MODE_2A_FRAMELEN (RC6_HEADERLEN + RC6_MODE_2A_DATALEN -1)
#define RC6_MODE_6ALOWPOWER_DATALEN 24
#define RC6_MODE_6ALOWPOWER_FRAMELEN (RC6_HEADERLEN + RC6_MODE_6ALOWPOWER_DATALEN -1)
#define RC6_MODE_6AOEMCMD_DATALEN 32
#define RC6_MODE_6AOEMCMD_FRAMELEN (RC6_HEADERLEN + RC6_MODE_6AOEMCMD_DATALEN -1)
#define RC6_MODE_6AOEMSTR_DATALEN 40
#define RC6_MODE_6AOEMSTR_FRAMELEN (RC6_HEADERLEN + RC6_MODE_6AOEMSTR_DATALEN -1)
#define RC6_MODE_6B_DATALEN 32
#define RC6_MODE_6B_FRAMELEN (RC6_HEADERLEN + RC6_MODE_6B_DATALEN -1)
/****************Timing frame for RCMM (us)*****************************
!* *
!* RCMM Timing Aspects: *
! -------------------------------- *
!* Standard 6 carrier periods ONTIME = 167 us nominal. *
!* *
!* A "0-0" sequence is defined as: *
!* 6 periods ONTIME and 10 periods OFFTIME *
!* *
!* A "0-1" sequence is defined as: *
!* 6 periods ONTIME and 16 periods OFFTIME *
!* *
!* A "1-0" sequence is defined as: *
!* 6 periods ONTIME and 22 periods OFFTIME *
!* *
!* A "1-1" sequence is defined as: *
!* 6 periods ONTIME and 28 periods OFFTIME *
!* *
!* Leader 15 carrier periods ONTIME = 417 us nominal. *
!* *
!* In the program comment we refer these pulse width-sequences as follows: *
!* *
!* Data ONTIME 6p. *
!* * ____
!* * | | |
!* * | | |
!* * _| |___________|
!*
!* < p6 >
!*
!* p6 = 167 us *
!* p6_max = 275 us *
!* p6_min = 80 us *
!* *
!* The 0-0 sequence timing *
!* *
!* * ____
!* * | | |
!* * | | |
!* * _| |_______________|
!*
!* < p6 >< p10 >
!* < p16 >
!* *
!* p16 (P6 + P10) = 444 us *
!* p16_max = 502 us *
!* p16_min = 386 us *
!* *
!* The 0-1 sequence timing *
!* *
!* * ____
!* * | | |
!* * | | |
!* * _| |______________________ |
!*
!* < p6 >< p16 >
!* < p22 >
!* *
!* p22 (P6 + P16) = 611 us *
!* p22_max = 669 us *
!* p22_min = 553 us *
!* *
!* The 1-0 sequence timing *
!* *
!* *
!* * ____
!* * | | |
!* * | | |
!* * _| |________________________ |
!*
!* < p6 >< p22 >
!* < p28 >
!*
!* p28 *
!* *
!* p28 (P6 + P22) = 778 us *
!* p28_max = 836 us *
!* p28_min = 720 us *
!*
************************************************/
#define P15L_MIN 330 //330
#define P15L_MAX 583 //583
//#define P25_MIN 636 //636
//#define P25_MAX 752 //752
#define P6_MIN 80 //80
#define P6_MAX 275 //275
//#define P16_MIN 386 //386
//#define P16_MAX 502 //502
//#define P22_MIN 553 //553
//#define P22_MAX 669 //669
//#define P28_MIN 720 //720
//#define P28_MAX 836 //836
//#define P34_MIN 886 //886
//#define P34_MAX 1002 //1002
/***************Timing frame for RC6 (us)********************************
* RC6 Timing Aspects: *
* *
* Data bit-time = 888 usec. *
* A "0" is defined as a 0 1 transition in the middle of a bit-time. *
* A "1" is defined as a 1 0 transition in the middle of a bit-time. *
* *
* In the program comment we refer these 2 pulse-widths as follows: *
* 1T = 444 usec, *
* 2T = 888 usec. *
* *
* When the pulses are 1T long *
* *
* _______
* |
* |_______
*
* |< >|< >|
* T1 T1
* *
* T1 = 444 us *
* t1_max = 658 us *
* t1_min = 250 us *
*
* When the pulses are 2T long *
* *
* _______________
* |
* |_______________
*
* |< >|< >|
* T2 T2
*
*
* T2 = 888 us *
* t2_max = 1124 us *
* t2_min = 672 us *
* *
* When the pulses are 3T long (when the trailer bit is received) *
* *
*
* ________________
* |
* |_______________
*
* |< >|< >|
* T3 T3
*
* T3 = 1333 us *
* t3_max = 1573 us *
* t3_min = 1126 us *
* *
*
*
*
* RC6 leader bit *
* *
* ________________
* |
* |________
*
* |< >|< >|
* T6 T2
* *
* T6 = 2664 us *
* t6_max = 3360 us *
* t6_min = 2179 us *
* *
* The signal free time of RC6 has a minimum value of t6_max *
* *
***********************************************************************************/
#define T6_MIN 2178
#define T6_MAX 3360
#define T1_MIN 249
#define T1_MAX 658
#define T2_MIN 671
#define T2_MAX 1124
#define T3_MIN 1125
#define T3_MAX 1573
#define TSEP_RC6M6A 400 //391.6ms
#define TREP_RC6M6A 110 //106.7ms
#define TREP_RC6PLUS 40 //40ms for RC6+ for keyboard and mouse
//#define RCMMOEM_KB_AUTOBREAK_TIME 350 //350ms
#define RCMMOEM_KB_AUTOBREAK_TIME 165 //165ms
#define RCMMOEM_RC_AUTOBREAK_TIME 165
#define RCMMBASIC_KB_AUTOBREAK_TIME 165 //165ms
#define RCMMBASIC_RC_AUTOBREAK_TIME 165
///////////////////////
#define P25_MIN 618
#define P25_MAX 658 
#define P16_MIN 300 
#define P16_MAX 507
#define P22_MIN 537 
#define P22_MAX 627 
#define P28_MIN 671 
#define P28_MAX 791 
#define P34_MIN 825
#define P34_MAX 1105
////////////////////////

#define IRDATA_FRAME_LEN    6
typedef struct IRDATA_FRAME
{
    unsigned char Mode;
    unsigned char Data[IRDATA_FRAME_LEN];
}IRDATA_FRAME;

#define RCDATA_BUFLEN 6


//-------------------------------------------------------------------------------------------
// Customer IR Specification parameter define (Please modify them by IR SPEC)
//-------------------------------------------------------------------------------------------
#define IR_INT_NP_EDGE_TRIG
#define IR_MODE_SEL             IR_TYPE_SWDECODE_MODE
// IR Header code define
#define IR_HEADER_CODE0         0x80    // Custom 0
#define IR_HEADER_CODE1         0x7F    // Custom 1

// IR Timing define
#define IR_HEADER_CODE_TIME     9000    // us
#define IR_OFF_CODE_TIME        4500    // us
#define IR_OFF_CODE_RP_TIME     2500    // us
#define IR_LOGI_01H_TIME        560     // us
#define IR_LOGI_0_TIME          1120    // us
#define IR_LOGI_1_TIME          2240    // us
#define IR_TIMEOUT_CYC          160000  // us


// IR Format define
#define IRKEY_DUMY              0xFF
#define IRDA_KEY_MAPPING_POWER  IRKEY_POWER

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

#endif //#ifndef __RECV_H__

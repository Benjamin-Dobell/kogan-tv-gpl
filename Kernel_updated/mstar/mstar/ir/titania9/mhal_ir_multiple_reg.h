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

#ifndef _MHAL_IR_REG_H_
#define _MHAL_IR_REG_H_

#include <asm/types.h>//@@@
#include "mdrv_types.h"//@@@

#ifdef _HAL_IR_C//@@@
#define INTERFACE
#else
#define INTERFACE extern
#endif

// Define IRQ registers
#define REG_MIPS_BASE           0xBF000000
#define REG_IRQ_BASE            (REG_MIPS_BASE+0x3200)
#define REG_IRQ_MASK_IR         (REG_IRQ_BASE + 0x14)
    #define IRQ_UNMASK_IR       0xF7FF

// Define IrDa registers
#define REG_IR_BASE             (0x3D00)
///////////////////////////////////////////////////////////////////////////////
#define REG_IR_CTRL0            ((0x00*0x10000)+ (REG_IR_BASE + 0x40*2))
    #define IR_INV                  (BIT7)
    #define IR_INT_MASK             (BIT6)
    #define IR_RPCODE_EN            (BIT5)
    #define IR_LG01H_CHK_EN         (BIT4)
    #define IR_DCODE_PCHK_EN        (BIT3)
    #define IR_CCODE_CHK_EN         (BIT2)
    #define IR_LDCCHK_EN            (BIT1)
    #define IR_EN                   (BIT0)
#define REG_IR_CTRL1            ((0x00*0x10000)+ (REG_IR_BASE + 0x40*2)+ 1)
    #define IR_SEPR_EN              (BIT1)
    #define IR_TIMEOUT_CHK_EN       (BIT0)
    #define IR_KEY_MSB_FIRST_EN     (BIT2)
    #define IR_BIT_INV_EN           (BIT3)
    #define IR_TSTBUS_SEL_MSK       (BIT5|BIT4)
    #define IR_RAW_RPT_INT_MSK      (BIT6)
    #define IR_INT_CRC_MSK          (BIT7)
#define REG_IR_HDC_UPB_L        ((0x00*0x10000)+ (REG_IR_BASE + 0x41*2))
    #define IR_HDC_UPB_L_MSK        (0xFF)
#define REG_IR_HDC_UPB_H        ((0x00*0x10000)+ (REG_IR_BASE + 0x41*2)+ 1)
    #define IR_HDC_UPB_H_MSK        (0x3F)
#define REG_IR_HDC_LOB_L        ((0x00*0x10000)+ (REG_IR_BASE + 0x42*2))
    #define IR_HDC_LOB_L_MSK        (0xFF)
#define REG_IR_HDC_LOB_H        ((0x00*0x10000)+ (REG_IR_BASE + 0x42*2)+ 1)
    #define IR_HDC_LOB_H_MSK        (0x3F)
#define REG_IR_OFC_UPB_L        ((0x00*0x10000)+ (REG_IR_BASE + 0x43*2))
    #define IR_OFC_UPB_L_MSK        (0xFF)
#define REG_IR_OFC_UPB_H        ((0x00*0x10000)+ (REG_IR_BASE + 0x43*2)+ 1)
    #define IR_OFC_UPB_H_MSK        (0x1F)
#define REG_IR_OFC_LOB_L        ((0x00*0x10000)+ (REG_IR_BASE + 0x44*2))
    #define IR_OFC_LOB_L_MSK        (0xFF)
#define REG_IR_OFC_LOB_H        ((0x00*0x10000)+ (REG_IR_BASE + 0x44*2)+ 1)
    #define IR_OFC_LOB_H_MSK        (0x1F)
#define REG_IR_OFC_RP_UPB_L     ((0x00*0x10000)+ (REG_IR_BASE + 0x45*2))
    #define IR_OFC_RP_UPB_L_MSK     (0xFF)
#define REG_IR_OFC_RP_UPB_H     ((0x00*0x10000)+ (REG_IR_BASE + 0x45*2)+ 1)
    #define IR_OFC_RP_UPB_H_MSK     (0x0F)
#define REG_IR_OFC_RP_LOB_L     ((0x00*0x10000)+ (REG_IR_BASE + 0x46*2))
    #define IR_OFC_RP_LOB_L_MSK     (0xFF)
#define REG_IR_OFC_RP_LOB_H     ((0x00*0x10000)+ (REG_IR_BASE + 0x46*2)+ 1)
    #define IR_OFC_RP_LOB_H_MSK     (0x0F)
#define REG_IR_LG01H_UPB_L      ((0x00*0x10000)+ (REG_IR_BASE + 0x47*2))
    #define IR_LG01H_UPB_L_MSK      (0xFF)
#define REG_IR_LG01H_UPB_H      ((0x00*0x10000)+ (REG_IR_BASE + 0x47*2)+ 1)
    #define IR_LG01H_UPB_H_MSK      (0x03)
#define REG_IR_LG01H_LOB_L      ((0x00*0x10000)+ (REG_IR_BASE + 0x48*2))
    #define IR_LG01H_LOB_L_MSK      (0xFF)
#define REG_IR_LG01H_LOB_H      ((0x00*0x10000)+ (REG_IR_BASE + 0x48*2)+ 1)
    #define IR_LG01H_LOB_H_MSK      (0x03)
#define REG_IR_LG0_UPB_L        ((0x00*0x10000)+ (REG_IR_BASE + 0x49*2))
    #define IR_LG0_UPB_L_MSK        (0xFF)
#define REG_IR_LG0_UPB_H        ((0x00*0x10000)+ (REG_IR_BASE + 0x49*2)+ 1)
    #define IR_LG0_UPB_H_MSK        (0x07)
#define REG_IR_LG0_LOB_L        ((0x00*0x10000)+ (REG_IR_BASE + 0x4A*2))
    #define IR_LG0_LOB_L_MSK        (0xFF)
#define REG_IR_LG0_LOB_H        ((0x00*0x10000)+ (REG_IR_BASE + 0x4A*2)+ 1)
    #define IR_LG0_LOB_H_MSK        (0x07)
#define REG_IR_LG1_UPB_L        ((0x00*0x10000)+ (REG_IR_BASE + 0x4B*2))
    #define IR_LG1_UPB_L_MSK        (0xFF)
#define REG_IR_LG1_UPB_H        ((0x00*0x10000)+ (REG_IR_BASE + 0x4B*2)+ 1)
    #define IR_LG1_UPB_H_MSK        (0x0F)
#define REG_IR_LG1_LOB_L        ((0x00*0x10000)+ (REG_IR_BASE + 0x4C*2))
    #define IR_LG1_LOB_L_MSK        (0xFF)
#define REG_IR_LG1_LOB_H        ((0x00*0x10000)+ (REG_IR_BASE + 0x4C*2)+ 1)
    #define IR_LG1_LOB_H_MSK        (0x0F)
#define REG_IR_SEPR_UPB_L       ((0x00*0x10000)+ (REG_IR_BASE + 0x4D*2))
    #define IR_SEPR_UPB_L_MSK       (0xFF)
#define REG_IR_SEPR_UPB_H       ((0x00*0x10000)+ (REG_IR_BASE + 0x4D*2)+ 1)
    #define IR_SEPR_UPB_H_MSK       (0x0F)
#define REG_IR_SEPR_LOB_L       ((0x00*0x10000)+ (REG_IR_BASE + 0x4E*2))
    #define IR_SEPR_LOB_L_MSK       (0xFF)
#define REG_IR_SEPR_LOB_H       ((0x00*0x10000)+ (REG_IR_BASE + 0x4E*2)+ 1)
    #define IR_SEPR_LOB_H_MSK       (0x0F)
#define REG_IR_TIMEOUT_CYC_L    ((0x00*0x10000)+ (REG_IR_BASE + 0x4F*2))
    #define IR_TIMEOUT_CYC_L_MSK    (0xFF)
#define REG_IR_TIMEOUT_CYC_H    ((0x00*0x10000)+ (REG_IR_BASE + 0x4F*2)+ 1)
    #define IR_TIMEOUT_CYC_H_MSK    (0xFF)
#define REG_IR_TIMEOUT_CYC_O    ((0x00*0x10000)+ (REG_IR_BASE + 0x50*2))
    #define IR_TIMEOUT_CYC_O_MSK    (BIT2|BIT1|BIT0)
#define REG_IR_CBIT_CCBYTE      ((0x00*0x10000)+ (REG_IR_BASE + 0x50*2)+ 1)
    #define IR_CODE_BIT             (0x7F) //ir_code_bit_num
    #define IR_CCODE_BYTE           (BIT7) //ir_ccode_byte
#define REG_IR_SEPR_BIT_CTRL    ((0x00*0x10000)+ (REG_IR_BASE + 0x51*2))
#define REG_IR_FIFO_SHOT_CTRL   ((0x00*0x10000)+ (REG_IR_BASE + 0x51*2)+ 1)
    #define IR_FIFO_DEPTH           (BIT2|BIT1|BIT0)
    #define IR_FIFO_EN              (BIT3)
    #define IR_SHOT_SEL_MSK         (BIT5|BIT4)
      #define IR_SHOT_P               (BIT4)
      #define IR_SHOT_N               (BIT5)
      #define IR_SHOT_PN              (BIT5|BIT4)
    #define IR_SWFIFO_EN            (BIT6) //T8, T9
    #define IR_FIFO_CLR             (BIT7)
#define REG_IR_CCODE_L          ((0x00*0x10000)+ (REG_IR_BASE + 0x52*2))
#define REG_IR_CCODE_H          ((0x00*0x10000)+ (REG_IR_BASE + 0x52*2)+ 1)
#define REG_IR_GLHRM_NUM_L      ((0x00*0x10000)+ (REG_IR_BASE + 0x53*2))
    #define IR_GLHRM_NUM_L_MSK      (0xFF)
#define REG_IR_GLHRM_NUM_H      ((0x00*0x10000)+ (REG_IR_BASE + 0x53*2)+ 1)
    #define IR_GLHRM_NUM_H_MSK      (BIT2|BIT1|BIT0)
    #define IR_GLHRM_EN             (BIT3)
#define REG_IR_DECODE_MODE      ((0x00*0x10000)+ (REG_IR_BASE + 0x53*2)+ 1)
    #define IR_DECODE_MSK           (BIT5|BIT4)
      #define IR_DECODE_FULL          (BIT5|BIT4)
      #define IR_DECODE_RAW           (BIT5)
      #define IR_DECODE_SW            (BIT4)
#define REG_IR_CKDIV_NUM        ((0x00*0x10000)+ (REG_IR_BASE + 0x54*2))
#define REG_IR_KEY_DATA         ((0x00*0x10000)+ (REG_IR_BASE + 0x54*2)+ 1)
#define REG_IR_SHOT_CNT_L       ((0x00*0x10000)+ (REG_IR_BASE + 0x55*2))
    #define IR_SHOT_CNT_L_MSK       (0xFF)
#define REG_IR_SHOT_CNT_H       ((0x00*0x10000)+ (REG_IR_BASE + 0x55*2)+ 1)
    #define IR_SHOT_CNT_H_MSK       (0xFF)
#define REG_IR_SHOT_CNT_O       ((0x00*0x10000)+ (REG_IR_BASE + 0x56*2))
    #define IR_SHOT_CNT_O_MSK       (BIT2|BIT1|BIT0)
#define REG_IR_STATUS_L         ((0x00*0x10000)+ (REG_IR_BASE + 0x56*2))
    #define IR_SHOT_P               (BIT4)
    #define IR_DATA_LOST            (BIT5)//T8,T9
    #define IR_TX1_FLAG             (BIT6)//T8,T9
#define REG_IR_STATUS_H         ((0x00*0x10000)+ (REG_IR_BASE + 0x56*2)+ 1)
    #define IR_RPT_FLAG             (BIT0)
    #define IR_FIFO_EMPTY           (BIT1)
    #define IR_TIMEOUT_FLAG         (BIT2)
    #define IR_FIFO_FULL            (BIT3)
    #define IR_INT_FLAG             (BIT4)
    #define IR_INT_CRC_FLAG         (BIT5)
    #define IR_NEC_WKUP_FLAG        (BIT6)
    #define IR_RC_WKUP_FLAG         (BIT7)
#define REG_IR_STATUS_CTRL      ((0x00*0x10000)+ (REG_IR_BASE + 0x58*2))
    #define IR_FIFO_RD_PULSE        (BIT0)
    #define IR_CRC_FLAG_CLR         (BIT1)
    #define IR_FLAG_CLR             (BIT2)
    #define IR_NEC_WKUP_FLAG_CLR    (BIT3)
    #define IR_WKUP_KEY_SEL         (BIT7|BIT6|BIT5|BIT4)
#define REG_IR_IFORMAT_SEL      ((0x00*0x10000)+ (REG_IR_BASE + 0x5A*2))
    #define IR_IFORMAT_SEL          (0x1F)


#if 1//(IR_MODE_SEL == IR_TYPE_HWDECODE_RC)//@@@
//IR HW_FULL: Time Interval & Timeout
#define HAL_IR_NEC_INTERVAL             108
#define HAL_IR_NEC_TIMEOUT              120
//IR HW_RC5: Time Interval & Timeout
#define HAL_IR_RC5_INTERVAL             29//ms
#define HAL_IR_RC5_TIMEOUT              116//ms
//IR HW_RC6: Time Interval & Timeout
#define HAL_IR_RC6_INTERVAL             26//ms
#define HAL_IR_RC6_TIMEOUT              104//ms
//IR decode mode
#define HAL_IR_DECODE_FULL          0x03//0x00
#define HAL_IR_DECODE_RAW           0x02//0x00
#define HAL_IR_DECODE_SW            0x01//0x00
//IR FIFO
#define HAL_IR_FIFO_DEPTH_MAX       7
#define HAL_IR_RC_FIFO_DEPTH_MAX    7
//IR control
#define HAL_IR_CTRL_0               0
#define HAL_IR_CTRL_1               1
#define HAL_IR_RC_CTRL_0            2
#define HAL_IR_RC_CTRL_1            3
//IR RC Decode Type
#define HAL_IR_RC_RC5               0
#define HAL_IR_RC_RC5X              1
#define HAL_IR_RC_RC6               2
#define HAL_IR_RC_DISABLE           3
//IR RC Wakeup key
#define HAL_IR_RC_WKUP_KEY1         0
#define HAL_IR_RC_WKUP_KEY2         1

typedef struct
{
    U8 u8Valid : 1;
    U8 u8Toggle : 1;
    U8 u8Repeat : 1;
    U8 u8RC6Mode : 3;  //Only for RC6
    U8 u8Reserved : 2;
    U8 u8Address; //system bits: RC5[4:0], RC6[7:0]
    U8 u8Command; //command bits: RC5[5:0], RC5X[6:0], RC6[7:0]
} HAL_IR_RC_KeyData;

//##############################
// For Hardware RC Decode
//##############################
#define REG_IR_RC_CTRL0                         ((0x00*0x10000)+ (REG_IR_BASE + 0x00*2))
#define REG_IR_RC_RC6_LDPS_THR_L                ((0x00*0x10000)+ (REG_IR_BASE + 0x00*2))    //BIT[4:0]
    #define _IR_RC_RC6_LDPS_THR_L_MSK           (0x1F) //BIT[4:0]
    #define _IR_RC_AUTOCONFIG                   (BIT5)
    #define _IR_RC_FIFO_CLR                     (BIT6)
    #define _IR_RC_FIFO_WFST                    (BIT7)
#define REG_IR_RC_CTRL1                         ((0x00*0x10000)+ (REG_IR_BASE + 0x00*2)+ 1)
#define REG_IR_RC_WKUP_DEBUG_SEL                ((0x00*0x10000)+ (REG_IR_BASE + 0x00*2)+ 1) //BIT[6:4]
    #define _IR_RC_WKUP_DEBUG_SEL_MSK           (BIT6|BIT5|BIT4) //BIT[6:4]
    #define _IR_RC_ENABLE                       (BIT0)
    #define _IR_RC_RC6_EN                       (BIT1)
    #define _IR_RC_RC5X_EN                      (BIT2)
    #define _IR_RC_MODE_MSK                     (_IR_RC_RC5X_EN|_IR_RC_RC6_EN|_IR_RC_ENABLE)
    #define _IR_RC_WKUP_EN                      (BIT3)
    #define _IR_RC_DGB_SEL                      (BIT6|BIT5|BIT4)
    #define _IR_RC_IN_INV                       (BIT7)
#define REG_IR_RC_LGPS_THR_L                    ((0x00*0x10000)+ (REG_IR_BASE + 0x01*2))    //BIT[7:0]
#define REG_IR_RC_LGPS_THR_H                    ((0x00*0x10000)+ (REG_IR_BASE + 0x01*2)+ 1) //BIT[4:0]
    #define _IR_RC_LGPS_THR_L_MSK               (0xFF)
    #define _IR_RC_LGPS_THR_H_MSK               (0x1F)
#define REG_IR_RC_RC6_LGPS_MAR_L                ((0x00*0x10000)+ (REG_IR_BASE + 0x02*2))    //BIT[7:0]
#define REG_IR_RC_RC6_LGPS_MAR_H                ((0x00*0x10000)+ (REG_IR_BASE + 0x02*2)+ 1) //BIT[1:0]
    #define _IR_RC_RC6_LGPS_MAR_L_MSK           (0xFF)
    #define _IR_RC_RC6_LGPS_MAR_H_MSK           (0x03)
#define REG_IR_RC_RC6_LDPS_THR_H                ((0x00*0x10000)+ (REG_IR_BASE + 0x02*2)+ 1) //BIT[4:2]
    #define _IR_RC_RC6_LDPS_THR_H_MSK           (BIT4|BIT3|BIT2) //BIT[4:2]
#define REG_IR_RC_RC6_ECO_EN                    ((0x00*0x10000)+ (REG_IR_BASE + 0x03*2))    //BIT[0]
    #define _IR_RC_RC6_ECO_EN                   (BIT0)
#define REG_IR_RC_INTG_THR                      ((0x00*0x10000)+ (REG_IR_BASE + 0x03*2))    //BIT[7:1]
    #define _IR_RC_INTG_THR_MSK                 (0xFE)
#define REG_IR_RC_CLK_DIV                       ((0x00*0x10000)+ (REG_IR_BASE + 0x03*2)+ 1) //BIT[4:0]
    #define _IR_RC_CLK_DIV_MSK                  (0x1F)
#define REG_IR_RC_WDOG_COUNT                    ((0x00*0x10000)+ (REG_IR_BASE + 0x04*2))    //BIT[7:0]
#define REG_IR_RC_TIMEOUT_COUNT                 ((0x00*0x10000)+ (REG_IR_BASE + 0x04*2)+ 1) //BIT[7:0]
#define REG_IR_RC_COMP_KEY1                     ((0x00*0x10000)+ (REG_IR_BASE + 0x05*2))    //BIT[7:0]
#define REG_IR_RC_COMP_KEY2                     ((0x00*0x10000)+ (REG_IR_BASE + 0x05*2)+ 1) //BIT[7:0]

//Registers: Read Only
#define REG_IR_RC_KEY_ADDR                      ((0x00*0x10000)+ (REG_IR_BASE + 0x06*2))    //BIT[7:0]
    #define _IR_RC_RC5_KEY_ADDR_MSK             (0x1F) //{[7:6]:2'b00, [5]:toggle, [4:0]:address}
    #define _IR_RC_RC5_KEY_ADDR_TGL             (BIT5)
    #define _IR_RC_RC6_KEY_ADDR_MSK             (0xFF) //[7:0]:address
#define REG_IR_RC_KEY_CMD                       ((0x00*0x10000)+ (REG_IR_BASE + 0x06*2)+ 1) //BIT[7:0]
    #define _IR_RC_RC5_KEY_CMD_MSK              (0x3F)//{[7]:repeat, [6]:1'b0, [5:0]:command}
    #define _IR_RC_RC5_KEY_CMD_RPT              (BIT7)
    #define _IR_RC_RC5X_KEY_CMD_MSK             (0x7F)//{[7]:repeat, [6:0]:command}
    #define _IR_RC_RC5X_KEY_CMD_RPT             (BIT7)
    #define _IR_RC_RC6_KEY_CMD_MSK              (0xFF)//[7:0]:command
#define REG_IR_RC_RC6_KEY_MISC                  ((0x00*0x10000)+ (REG_IR_BASE + 0x07*2))    //BIT[4:0]
    #define _IR_RC_RC6_KEY_MISC_MOD_MSK         (BIT2|BIT1|BIT0)//{[4]:repeat, [3]:toggle, [2:0]:mode}
    #define _IR_RC_RC6_KEY_MISC_TGL             (BIT3)
    #define _IR_RC_RC6_KEY_MISC_RPT             (BIT4)
#define REG_IR_RC_FIFO_STATUS1                  ((0x00*0x10000)+ (REG_IR_BASE + 0x08*2))
    #define _IR_RC_FIFO_EMPTY                   (BIT0)
    #define _IR_RC_TIMEOUT_FLAG                 (BIT1)
    #define _IR_RC_FIFO_FULL                    (BIT2)
    #define _IR_RC_FIFO_WPTR_MSK                (BIT6|BIT5|BIT4)
#define REG_IR_RC_FIFO_STATUS2                  ((0x00*0x10000)+ (REG_IR_BASE + 0x08*2)+ 1)
    #define _IR_RC_FIFO_RPTR_MSK                (BIT2|BIT1|BIT0)

//Registers: Write Only
#define REG_IR_RC_FIFO_READ_PULSE               ((0x00*0x10000)+ (REG_IR_BASE + 0x09*2))
    #define _IR_RC_FIFO_RD_PULSE                (BIT0)
#define REG_IR_RC_WKUP_CLR                      ((0x00*0x10000)+ (REG_IR_BASE + 0x09*2)+ 1)
    #define _IR_RC_WKUP_CLR                     (BIT0)

    //#######################################
    //#  For Hardware RC Decode
    //#######################################
    INTERFACE void HAL_IR_CfgSetControl(U32 u32Reg,U8 u8CtrlData,U8 u8Mask);
    INTERFACE void HAL_IR_CfgControl(U8 u8Index, U8 u8CtrlData,U8 u8Mask);
    INTERFACE void HAL_IR_CfgHwDecode(U8 u8Enable);
    INTERFACE void HAL_IR_CfgIntrMask(U8 u8Enable);

    INTERFACE U8 HAL_IR_RC_GetDecode(void);
    INTERFACE void HAL_IR_RC_CfgClkDiv(U8 u8ClkMhz);
    INTERFACE void HAL_IR_RC_CfgAutoConfig(U8 u8Enable);
    INTERFACE void HAL_IR_RC_CfgFifoClear(U8 u8Enable);
    INTERFACE void HAL_IR_RC_CfgFifoWFirst(U8 u8Enable);
    INTERFACE void HAL_IR_RC_CfgDecMode(U8 u8DecMode);
    INTERFACE void HAL_IR_RC_CfgWakeup(U8 u8Enable);
    INTERFACE void HAL_IR_RC_CfgOutputSelect(U8 u8OutputSel);
    INTERFACE void HAL_IR_RC_CfgInputInvert(U8 u8Enable);
    INTERFACE void HAL_IR_RC_CfgLgpsThr(U16 u16BitTime);
    INTERFACE void HAL_IR_RC_CfgIntgThr(U16 u16BitTime);
    INTERFACE void HAL_IR_RC_CfgWdogCount(U16 u16BitTime);
    INTERFACE void HAL_IR_RC_CfgTimeoutCount(void);//U16 u16TimeoutUs
    INTERFACE void HAL_IR_RC_CfgLgpsMar(U16 u16BitTime);//Only for RC6
    INTERFACE void HAL_IR_RC_CfgLdpsThr(U16 u16BitTime);//Only for RC6
    INTERFACE void HAL_IR_RC_CfgEcoFunc(BOOL bEnable);//Only for RC6
    INTERFACE BOOL HAL_IR_RC_IsTimeout(void);
    INTERFACE BOOL HAL_IR_RC_IsFIFOEmpty(void);
    INTERFACE BOOL HAL_IR_RC_IsFIFOFull(void);
    INTERFACE void HAL_IR_RC_ClearFIFO(void);
    INTERFACE U8 HAL_IR_RC_GetWakeupKey(U8 u8KeyNum);
    INTERFACE void HAL_IR_RC_GetKeyData(HAL_IR_RC_KeyData *pIRRCKeyData);

#endif

#define MHal_IR_REG(addr)          (*(volatile U8*)(REG_MIPS_BASE + (((addr) & ~1)<<1) + (addr & 1)))//@@@

#endif

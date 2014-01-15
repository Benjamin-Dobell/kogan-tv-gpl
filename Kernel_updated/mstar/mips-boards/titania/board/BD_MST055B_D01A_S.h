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


#ifndef _BD_MST055B_D01A_S_H
#define _BD_MST055B_D01A_S_H


//External crystal clock
#define XTAL_RTC_CLOCK_FREQ         CRYSTAL_32768
#define XTAL_CLOCK_FREQ             12000000    //12 MHz
#define MCU_XTAL_CLK_HZ             (XTAL_CLOCK_FREQ*MCU_PLL_MUL) // Hz
#define MCU_XTAL_CLK_KHZ            (MCU_XTAL_CLK_HZ / 1000)
#define MCU_XTAL_CLK_MHZ            (MCU_XTAL_CLK_KHZ / 1000)


//------Tuner Setting----------------------------------------
#define TS_CLK_INV                      0
#define TS_PARALLEL_OUTPUT              1
#define FRONTEND_DEMOD_TYPE             ZARLINK_ZL10353_DEMODE
#define FRONTEND_TUNER_TYPE             PHILIPS_FQD1216_TUNER//PHILIPS_TDA1316_TUNER //PHILIPS_FQD1216_TUNER

//------Peripheral Device Setting----------------------------
#ifndef IR_TYPE_SEL
#define IR_TYPE_SEL                 IR_TYPE_MSTAR_DTV
#endif

//------Power Setting----------------------------------------


//------Memory Setting---------------------------------------


//------OSD & EPG--------------------------------------------


//------CC & TT----------------------------------------------


//------SVD----------------------------------------------
#define SVD_CLOCK_SEL               SVD_CLOCK_250MHZ
//------Analog Function Setting------------------------------


//------Extra------------------------------------------------


//------ EEPROM ----------------------------------------------------------------


//------ MEMORY MAP 8051 ---------------------------------------------------------
#define MMAP_8051_TYPE_SEL          MMAP_8051_CFG_1


//------ MEMORY MAP 8051 LITE ----------------------------------------------------
#define MMAP_8051_LITE_TYPE_SEL     MMAP_8051_LITE_CFG_1


//------ MEMORY MAP AEON ---------------------------------------------------------
#define MMAP_AEON_TYPE_SEL          MMAP_AEON_CFG_1

//------Function to PIN mapping ------------------------------------------------
#define PIN_UART1                   0
#define PIN_UART2                   0
#define PIN_I2S_OUT_MUTE            PIN_109
#define PIN_I2S_IN                  PADS_PCM
#define PIN_I2S_OUT                 PADS_PCM
#define PIN_SPDIF_IN                PIN_102
#define PIN_SPDIF_OUT               PIN_103
#define PIN_TS0                     PADS_TS0
#define PIN_MIIC                    PADS_DDCR
#define PIN_PF                      0
#define PIN_WF                      0
#define PIN_ET                      0

//------GPIO setting------------------------------------------------------------
#define PIN_1_IS_GPIO               GPIO_OUT_LOW    // POWER_SW     High: turn off
#define PIN_3_IS_GPIO               GPIO_OUT_LOW    // RGB_SW       L: VGA H: SCART
#define PIN_4_IS_GPIO               GPIO_OUT_HIGH   // AMP_STB      High: mute
#define PIN_5_IS_GPIO               GPIO_OUT_HIGH   // MUTE_S       High: mute
#define PIN_7_IS_GPIO               GPIO_OUT_HIGH   // FLASH_WPN    Low - protector
#define PIN_8_IS_GPIO               GPIO_OUT_HIGH   // FLASH_WPN2   Low - protector
#define PIN_112_IS_GPIO             GPIO_OUT_HIGH   // Panel_ON     High: Off
#define PIN_113_IS_GPIO             GPIO_OUT_HIGH   // VBLCTRL      High: Off
#define PIN_110_IS_GPIO             GPIO_IN         // USB_OCD

// Audio Amplifier
#define Audio_Amplifier_ON()             MDrv_MCU8051Lite_RegWrite(0x1005, 0x02) //PM8 pull low
#define Audio_Amplifier_OFF()            MDrv_MCU8051Lite_RegWrite(0x1005, 0x00)
//------Chip Type---------------------------------------------------------------
//#include "chip/msd3102eq.h"
#include "chip/msd3063eq.h"

//------MIU setting ----------------------------------------------------------------
#define MIU_0_02                        0x0545
#define MIU_1_02                        0x0345
#define MIU_0_36                        0x0244
#define MIU_0_38                        0x0070
#define ENABLE_MIU_1                    1
#define DDRII_ODT

//------ SYSTEM MEMORY MAP ---------------------------------------------------------
#if defined(CONFIG_MSTAR_TITANIA_MMAP_32MB)
#include "mmap/mmap_32mb.h"
#elif defined(CONFIG_MSTAR_TITANIA_MMAP_64MB)
#include "mmap/mmap_64mb.h"
#elif defined(CONFIG_MSTAR_TITANIA_MMAP_32MB_32MB)
#include "mmap/mmap_32mb_32mb.h"
#elif defined(CONFIG_MSTAR_TITANIA_MMAP_64MB_32MB)
#include "mmap/mmap_64mb_32mb.h"
#elif defined(CONFIG_MSTAR_TITANIA_MMAP_64MB_64MB)
#include "mmap/mmap_64mb_64mb.h"
#elif defined(CONFIG_MSTAR_TITANIA_MMAP_128MB_128MB)
#include "mmap/mmap_128mb_128mb.h"
#elif defined(CONFIG_MSTAR_TITANIA_MMAP_4MB_FLASH)
#endif

//------ SCALER INPUT SOURCE MUX ---------------------------------------------------
#define SC_VGA_AMUX         0x1
//#define SC_YPBPR_AMUX       0x2
#define SC_YPBPR1_AMUX      0x2
#define SC_YPBPR2_AMUX      0x1F

#define SC_ATV_YMUX         0x0
#define SC_ATV_CMUX         0x1F

//#define SC_CVBS_YMUX        0x3
//#define SC_CVBS_CMUX        0x1F
#define SC_CVBS1_YMUX       0x3
#define SC_CVBS1_CMUX       0x1F
#define SC_CVBS2_YMUX       0x1F
#define SC_CVBS2_CMUX       0x1F
#define SC_CVBS3_YMUX       0x1F
#define SC_CVBS3_CMUX       0x1F

//#define SC_SVIDEO_YMUX      0x4
//#define SC_SVIDEO_CMUX      0x6
#define SC_SVIDEO1_YMUX     0x4
#define SC_SVIDEO1_CMUX     0x6
#define SC_SVIDEO2_YMUX     0x1F
#define SC_SVIDEO2_CMUX     0x1F

#define SC_SCART1_YMUX      0x1
#define SC_SCART1_CMUX      0x1F

#define SC_SCART2_YMUX      0x2
#define SC_SCART2_CMUX      0x1F

#endif // _BD_MST055B_D01A_S_H

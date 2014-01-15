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


#ifndef _BD_T2_CUS03_MBRD_BOARD_DVB_1_H
#define _BD_T2_CUS03_MBRD_BOARD_DVB_1_H

#define BOARD_NAME                  "BD_T2_CUS03_MBRD_BOARD_DVB_1"

//External crystal clock
#define XTAL_RTC_CLOCK_FREQ         CRYSTAL_32768
#define XTAL_CLOCK_FREQ             12000000    //12 MHz
#define MCU_XTAL_CLK_HZ             (XTAL_CLOCK_FREQ*MCU_PLL_MUL) // Hz
#define MCU_XTAL_CLK_KHZ            (MCU_XTAL_CLK_HZ / 1000)
#define MCU_XTAL_CLK_MHZ            (MCU_XTAL_CLK_KHZ / 1000)


//------Tuner Setting----------------------------------------
#define TS_CLK_INV                      0
#define TS_PARALLEL_OUTPUT              0
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
#define PIN_UART2                   0//PADS_UART2_MODE1
#define PIN_I2S_OUT_MUTE            BALL_E7
#define PIN_I2S_IN                  PADS_I2S
#define PIN_I2S_OUT                 PADS_I2S
#define PIN_SPDIF_IN                BALL_F11
#define PIN_SPDIF_OUT               BALL_E9
#define PIN_TS0                     PADS_TS0
#define PIN_TS1                     PADS_TS1
#define PIN_MIIC                    PADS_DDCR2
#define PIN_PCMCIA                  PADS_PCM
#define PIN_PF                      PADS_PF

#if 0 /* SDIO and ET is mutal exclusive, board need to be rework */
#define PIN_WF                      PADS_WF_MODE1
#define PIN_ET                      0
#else
#define PIN_WF                      0
#define PIN_ET                      PADS_ET_MODE0
#endif

// SPI_CSN_2 is wrong, need to rework
#define PIN_FLASH_WP0               BALL_AF11
#define PIN_FLASH_WP1               BALL_AC17

#define PIN_PF_WP0                  BALL_AB17

//------GPIO setting------------------------------------------------------------
//#define BALL_AA24_IS_GPIO           GPIO_OUT_LOW    // POWER_SW     High: turn off
#define BALL_AF5_IS_GPIO            GPIO_OUT_HIGH   // AMP_STB      High: mute
#define BALL_F9_IS_GPIO             GPIO_OUT_HIGH   // MUTE_S       Low:  mute
//#define BALL_AC16_IS_GPIO           GPIO_OUT_HIGH   // MUTE_POP EAR-OUT FIXME
#define BALL_AF11_IS_GPIO           GPIO_OUT_LOW    // FLASH_WPN    Low - protect
#define BALL_AC17_IS_GPIO           GPIO_OUT_LOW    // FLASH_WPN_2
#define BALL_B6_IS_GPIO             GPIO_OUT_HIGH   // Panel_ON     High: Off
#define BALL_A6_IS_GPIO             GPIO_OUT_HIGH   // VBLCTRL      High: Off
#define BALL_AF10_IS_GPIO           GPIO_IN         // USB_OCD_1
#define BALL_AA17_IS_GPIO           GPIO_IN         // USB_OCD_2
//#define BALL_K1_IS_GPIO             GPIO_IN         // M-LINK SW I2C
//#define BALL_K3_IS_GPIO             GPIO_IN         // M-LINK SW I2C
#define BALL_D9_IS_GPIO             GPIO_OUT_LOW    // PCM_PWR_CTL  Low:  Off
#define BALL_D10_IS_GPIO            GPIO_IN         // MS_INS, FIXME
#define BALL_E8_IS_GPIO             GPIO_IN         // ETH-RST, FIXME

// Reply Power On
#define BALL_E5_IS_GPIO             GPIO_OUT_LOW

// Audio Amplifier
#define Audio_Amplifier_ON()        MDrv_GPIO_Set_High(12);MDrv_GPIO_Set_Low(42)
#define Audio_Amplifier_OFF()       MDrv_GPIO_Set_High(12);MDrv_GPIO_Set_High(42)

#define CUS03_DEMO_BOARD
#if 0
#define HeadPhone_ON()                      MDrv_GPIO_Set_High(96)
#define HeadPhone_OFF()                     MDrv_GPIO_Set_Low(96)
#define CUS03_AMP_MUTE_ON()                MDrv_GPIO_Set_High(90)
#define CUS03_AMP_MUTE_OFF()               MDrv_GPIO_Set_Low(90)
#endif
//============================================
// Use ATSC code base audio path setting
//============================================
#define S4_AUDIO_PATH_SETTING
//#define AUDIO_AMP_TYPE              USE_ANALOG_AMP  // USE_I2S_AMP
#define AUDIO_SOURCE_DTV            AUDIO_DSP1_DVB_INPUT
#define AUDIO_SOURCE_DTV2           AUDIO_DSP2_DVB_INPUT
#define AUDIO_SOURCE_ATV            AUDIO_DSP3_SIF_INPUT
#define AUDIO_SOURCE_PC             AUDIO_AUIN1_INPUT
#define AUDIO_SOURCE_YPBPR          AUDIO_AUIN2_INPUT
#define AUDIO_SOURCE_YPBPR2         AUDIO_AUIN2_INPUT
#define AUDIO_SOURCE_AV             AUDIO_AUIN3_INPUT
#define AUDIO_SOURCE_AV2            AUDIO_AUIN3_INPUT
#define AUDIO_SOURCE_AV3            AUDIO_AUIN3_INPUT
#define AUDIO_SOURCE_SV             AUDIO_AUIN3_INPUT
#define AUDIO_SOURCE_SV2            AUDIO_AUIN3_INPUT
#define AUDIO_SOURCE_SCART          AUDIO_AUIN0_INPUT
#define AUDIO_SOURCE_SCART1              AUDIO_AUIN0_INPUT//AUDIO_AUIN0_INPUT
#define AUDIO_SOURCE_SCART2         AUDIO_AUIN1_INPUT
#define AUDIO_SOURCE_HDMI           AUDIO_HDMI_INPUT
#define AUDIO_SOURCE_HDMI2          AUDIO_HDMI_INPUT
#define AUDIO_SOURCE_HDMI3          AUDIO_HDMI_INPUT
#define AUDIO_SOURCE_DVI            AUDIO_AUIN1_INPUT
#define AUDIO_SOURCE_DVI2           AUDIO_AUIN1_INPUT
#define AUDIO_SOURCE_DVI3           AUDIO_AUIN1_INPUT
#define AUDIO_SOURCE_SPDIF               AUDIO_SPDIF_INPUT
#define AUDIO_SOURCE_AM_MONO         AUDIO_AUIN5_INPUT


#define AUDIO_PATH_MAIN_SPEAKER     AUDIO_PATH_MAIN
#define AUDIO_PATH_HP               AUDIO_PATH_0
#define AUDIO_PATH_SCART1           AUDIO_PATH_1
#define AUDIO_PATH_SCART2           AUDIO_PATH_2
#define AUDIO_PATH_LINEOUT          AUDIO_PATH_1
#define AUDIO_PATH_SIFOUT           AUDIO_PATH_1
#define AUDIO_PATH_SRC                  AUDIO_PATH_4
  #define AUDIO_PATH_SPDIF            AUDIO_PATH_3
  #define AUDIO_PATH_I2S              AUDIO_PATH_5


 #define AUDIO_OUTPUT_MAIN_SPEAKER  AUDIO_I2S_OUTPUT
 #define AUDIO_OUTPUT_HP            AUDIO_AUOUT0_OUTPUT
 #define AUDIO_OUTPUT_SCART1        AUDIO_AUOUT1_OUTPUT
 #define AUDIO_OUTPUT_SCART2        AUDIO_AUOUT2_OUTPUT       // always output SIF
 #define AUDIO_OUTPUT_LINEOUT       AUDIO_I2S_OUTPUT       // As SCART1
 #define AUDIO_OUTPUT_SIFOUT        AUDIO_AUOUT2_OUTPUT
//------Chip Type---------------------------------------------------------------
#include "chip/msd3368gv.h"

//------MIU setting ----------------------------------------------------------------
#define MIU_0_02                        0x0C45  // length 128MB
#define MIU_1_02                        0x0C45  // length
#define MIU_0_36                        0x0245
#define MIU_0_38                        0x0040
#define ENABLE_MIU_1                    1       // disable MIU1 for bring up

#define MIU_SSC
//#define DDRII_ODT

#define MIU_0_SPEED                     400
#define MIU_0_DD                        0x0000
#define MIU_0_1A                        0x5757

#define MIU_1_SPEED                     400
#define MIU_1_DD                        0x0000
#define MIU_1_1A                        0x5757

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
#define SC_YPBPR1_AMUX      0x1
#define SC_YPBPR2_AMUX      0x1F

#define SC_ATV_YMUX         0x0
#define SC_ATV_CMUX         0x1F

// T2 CVBS change ip mux
#define SC_CVBS1_YMUX       0x3
#define SC_CVBS1_CMUX       0x1F
#define SC_CVBS2_YMUX       0x1F
#define SC_CVBS2_CMUX       0x1F
#define SC_CVBS3_YMUX       0x1F
#define SC_CVBS3_CMUX       0x1F

#define SC_SVIDEO1_YMUX     0x4
#define SC_SVIDEO1_CMUX     0x6
#define SC_SVIDEO2_YMUX     0x1F
#define SC_SVIDEO2_CMUX     0x1F

#define SC_SCART1_YMUX      0x1
#define SC_SCART1_CMUX      0x1
#define SC_SCART2_YMUX      0x2
#define SC_SCART2_CMUX      0x1F

#endif // _BD_T2_CUS03_MBRD_BOARD_DVB_1_H

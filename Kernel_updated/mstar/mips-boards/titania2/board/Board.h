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

#ifndef _BOARD_H_
#define _BOARD_H_

#include <linux/autoconf.h>
//#include <linux/undefconf.h>

//------------------------------IR_TYPE_SEL--------------
#define IR_TYPE_OLD                 0
#define IR_TYPE_NEW                 1
#define IR_TYPE_MSTAR_DTV           2
#define IR_TYPE_MSTAR_RAW		    3
#define IR_TYPE_RC_V16              4
#define IR_TYPE_CUS03_DTV           5
#define IR_TYPE_MSTAR_FANTASY       6
#define IR_TYPE_MSTAR_SZ1           7
#define IR_TYPE_SKYWORTH_SLIDE      8
#define IR_TYPE_HISENSE_6I78        9
#define IR_TYPE_CUS08_RC5           10
#define IR_TYPE_KONKA               11
#define IR_TYPE_CUS21SH             21
#define IR_TYPE_HAIER_TOSHIBA       22
#define IR_TYPE_TCL_RCA             23
#define IR_TYPE_RCMM                24
#define IR_TYPE_TOSHIBA             25


//------------------------------MEMORY_MAP for 8051------
#define MMAP_8051_CFG_1             0
#define MMAP_8051_CFG_2             1
#define MMAP_8051_CFG_3             2
#define MMAP_8051_CFG_4             3
#define MMAP_8051_CFG_5             4

//------------------------------MEMORY_MAP for 8051 Lite-
#define MMAP_8051_LITE_CFG_1        0
#define MMAP_8051_LITE_CFG_2        1
#define MMAP_8051_LITE_CFG_3        2
#define MMAP_8051_LITE_CFG_4        3
#define MMAP_8051_LITE_CFG_5        4

//------------------------------MEMORY_MAP for Aeon------
#define MMAP_AEON_CFG_1             0
#define MMAP_AEON_CFG_2             1
#define MMAP_AEON_CFG_3             2
#define MMAP_AEON_CFG_4             3
#define MMAP_AEON_CFG_5             4

//------------------------------BOARD_TYPE_SEL-----------
// 0x00 ~ 0x1F LCD Demo board made in Taiwan
#define BD_FPGA                     0x00
#define BD_MST055A_D01A_S           0x01
#define BD_MST055B_D01A_S           0x02
#define BD_MST055C_D01A_S			0x03
#define BD_MST056C_D01A_S			0x04
#define BD_MST055E_D01A_S			0x05
#define BD_MST064C_D01A_S			0x06
#define BD_MST064D_D01A_S			0x07
#define BD_MST064E_D01A_S			0x08
// 0x20 ~ 0x3F LCD Demo board made in China
#define BD_SHENZHEN_01              0x20

// 0x40 ~ 0x6F LCD Customers' board
#define BD_S5_CUS03_DEMO_BOARD_1       0x40
#define BD_S6_CUS03_DEMO_BOARD_ATSC_1  0x41
#define BD_T2_CUS03_VBRD_BOARD_DVB_1  0x42
#define BD_T2_CUS03_MBRD_BOARD_ATSC_1 0x43
#define BD_T2_CUS03_MBRD_BOARD_DVB_1 0x44
#define BD_T2_CUS03_MIBRD_BOARD_ATSC_1 0x45

#define BD_CUS05KS_X1_BN41_090303   0x50
#define BD_CUS05KS_X1_BN41_090420   0x51

// 0x70 ~ 0x8F CRT Demo board made in Taiwan

// 0x90 ~ 0x9F CRT Demo board made in China

// 0xA0 ~ 0xAF CRT Customers' board

// 0xE1 ~ 0xFE Others' board

#define BD_UNKNOWN                  0xFF

#define SVD_CLOCK_250MHZ            0x00
#define SVD_CLOCK_240MHZ            0x01
#define SVD_CLOCK_216MHZ            0x02
#define SVD_CLOCK_SCPLL             0x03
#define SVD_CLOCK_MIU               0x04
#define SVD_CLOCK_144MHZ            0x05
#define SVD_CLOCK_123MHZ            0x06
#define SVD_CLOCK_108MHZ            0x07

#define SVD_CLOCK_ENABLE            TRUE
#define SVD_CLOCK_INVERT            FALSE

#ifndef MS_BOARD_TYPE_SEL
#if defined(CONFIG_MSTAR_TITANIA_BD_MST055A_D01A_S)
    #define MS_BOARD_TYPE_SEL       BD_MST055A_D01A_S
#elif defined (CONFIG_MSTAR_TITANIA_BD_MST055B_D01A_S)
    #define MS_BOARD_TYPE_SEL       BD_MST055B_D01A_S
#elif defined (CONFIG_MSTAR_TITANIA_BD_MST055C_D01A_S)
    #define MS_BOARD_TYPE_SEL       BD_MST055C_D01A_S
#elif defined (CONFIG_MSTAR_TITANIA_BD_MST056C_D01A_S)
    #define MS_BOARD_TYPE_SEL       BD_MST056C_D01A_S
#elif defined (CONFIG_MSTAR_TITANIA_BD_MST055E_D01A_S)
    #define MS_BOARD_TYPE_SEL       BD_MST055E_D01A_S
#elif defined (CONFIG_MSTAR_TITANIA_BD_MST064C_D01A_S)
    #define MS_BOARD_TYPE_SEL       BD_MST064C_D01A_S
#elif defined (CONFIG_MSTAR_TITANIA_BD_MST064D_D01A_S)
    #define MS_BOARD_TYPE_SEL       BD_MST064D_D01A_S
#elif defined (CONFIG_MSTAR_TITANIA_BD_MST064E_D01A_S)
    #define MS_BOARD_TYPE_SEL       BD_MST064E_D01A_S
#elif defined (CONFIG_MSTAR_TITANIA_BD_FPGA)
    #define MS_BOARD_TYPE_SEL       BD_FPGA
#elif defined (CONFIG_MSTAR_TITANIA_BD_S5_CUS03_DEMO_BOARD_1)
    #define MS_BOARD_TYPE_SEL       BD_S5_CUS03_DEMO_BOARD_1
#elif defined (CONFIG_MSTAR_TITANIA_BD_S6_CUS03_DEMO_BOARD_ATSC_1)
    #define MS_BOARD_TYPE_SEL       BD_S6_CUS03_DEMO_BOARD_ATSC_1
#elif defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_VBRD_BOARD_DVB_1)
    #define MS_BOARD_TYPE_SEL       BD_T2_CUS03_VBRD_BOARD_DVB_1
#elif defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)
    #define MS_BOARD_TYPE_SEL       BD_T2_CUS03_MBRD_BOARD_ATSC_1
#elif defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_DVB_1)
    #define MS_BOARD_TYPE_SEL       BD_T2_CUS03_MBRD_BOARD_DVB_1
#elif defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
    #define MS_BOARD_TYPE_SEL       BD_T2_CUS03_MIBRD_BOARD_ATSC_1
#elif defined (CONFIG_MSTAR_TITANIA_BD_CUS05KS_X1_BN41_090303)
    #define MS_BOARD_TYPE_SEL       BD_CUS05KS_X1_BN41_090303
#elif defined (CONFIG_MSTAR_TITANIA_BD_CUS05KS_X1_BN41_090420)
    #define MS_BOARD_TYPE_SEL       BD_CUS05KS_X1_BN41_090420
#else
    #error "BOARD define not found"
#endif
#endif
//-------------------------------------------------------


/////////////////////////////////////////////////////////
#if   (MS_BOARD_TYPE_SEL == BD_FPGA)
    #include "BD_FPGA.h"
#elif (MS_BOARD_TYPE_SEL == BD_MST055A_D01A_S)
    #include "BD_MST055A_D01A_S.h"
#elif (MS_BOARD_TYPE_SEL == BD_MST055B_D01A_S)
    #include "BD_MST055B_D01A_S.h"
#elif (MS_BOARD_TYPE_SEL == BD_MST055C_D01A_S)
    #include "BD_MST055C_D01A_S.h"
#elif (MS_BOARD_TYPE_SEL == BD_MST056C_D01A_S)
    #include "BD_MST056C_D01A_S.h"
#elif (MS_BOARD_TYPE_SEL == BD_MST055E_D01A_S)
    #include "BD_MST055E_D01A_S.h"
#elif (MS_BOARD_TYPE_SEL == BD_MST064C_D01A_S)
    #include "BD_MST064C_D01A_S.h"
#elif (MS_BOARD_TYPE_SEL == BD_MST064D_D01A_S)
    #include "BD_MST064D_D01A_S.h"
#elif (MS_BOARD_TYPE_SEL == BD_MST064E_D01A_S)
    #include "BD_MST064E_D01A_S.h"
#elif (MS_BOARD_TYPE_SEL == BD_S5_CUS03_DEMO_BOARD_1)
    #include "BD_S5_CUS03_DEMO_BOARD_1.h"
#elif (MS_BOARD_TYPE_SEL == BD_S6_CUS03_DEMO_BOARD_ATSC_1)
    #include "BD_S6_CUS03_DEMO_BOARD_ATSC_1.h"
#elif (MS_BOARD_TYPE_SEL == BD_T2_CUS03_VBRD_BOARD_DVB_1)
    #include "BD_T2_CUS03_VBRD_BOARD_DVB_1.h"
#elif (MS_BOARD_TYPE_SEL == BD_T2_CUS03_MBRD_BOARD_ATSC_1)
    #include "BD_T2_CUS03_MBRD_BOARD_ATSC_1.h"
#elif (MS_BOARD_TYPE_SEL == BD_T2_CUS03_MBRD_BOARD_DVB_1)
    #include "BD_T2_CUS03_MBRD_BOARD_DVB_1.h"
#elif (MS_BOARD_TYPE_SEL == BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
    #include "BD_T2_CUS03_MIBRD_BOARD_ATSC_1.h"
#elif (MS_BOARD_TYPE_SEL == BD_CUS05KS_X1_BN41_090303)
    #include "BD_CUS05KS_X1_BN41_090303.h"
#elif (MS_BOARD_TYPE_SEL == BD_CUS05KS_X1_BN41_090420)
    #include "BD_CUS05KS_X1_BN41_090420.h"
#else
    #error "MS_BOARD_TYPE_SEL is not defined"
#endif


/////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// PIN_PWM2 / PIN_PWM3
//----------------------------------------------------------------------------

#ifndef PIN_PWM2
    #if (CHIP_USE_PWM2_IN_GPIO14 && !PAD_GPIO14_IS_GPIO)
        #define PIN_PWM2    PAD_GPIO14
    #else
        #define PIN_PWM2    PAD_PWM2
    #endif
#endif

#ifndef PIN_PWM3
    #if (CHIP_USE_PWM3_IN_GPIO15 && !PAD_GPIO15_IS_GPIO)
        #define PIN_PWM3    PAD_GPIO15
    #else
        #define PIN_PWM3    PAD_PWM3
    #endif
#endif

//------------------------------GPIO_PIN----------------------------------------
#define GPIO_NONE               0       // Not GPIO pin (default)
#define GPIO_IN                 1       // GPI
#define GPIO_OUT_LOW            2       // GPO output low
#define GPIO_OUT_HIGH           3       // GPO output high
#define GPIO_51PORT_LOW         4
#define GPIO_51PORT_HIGH        5

//----------------------------------------------------------------------------
// PIN_CIMAX
//----------------------------------------------------------------------------

#ifndef PIN_CIMAX
    #if (!CHIP_HAS_CIMAX)
        #define PIN_CIMAX   0
    #else
        #error "PIN_CIMAX not defined"
    #endif
#elif (PIN_CIMAX != 0 && !CHIP_HAS_CIMAX)
    #error "PIN_CIMAX error"
#endif

//----------------------------------------------------------------------------
// PIN_PCMCIA
//----------------------------------------------------------------------------

#ifndef PIN_PCMCIA
    #if (!CHIP_HAS_PCMCIA)
        #define PIN_PCMCIA   0
    #else
        #error "PIN_PCMCIA not defined"
    #endif
#elif (PIN_PCMCIA != 0 && !CHIP_HAS_PCMCIA)
    #error "PIN_PCMCIA error"
#endif

//----------------------------------------------------------------------------
// PIN_SPI_CZ[3:1]
//----------------------------------------------------------------------------

#ifndef PIN_SPI_CZ1
    #define PIN_SPI_CZ1 0
#endif

#ifndef PIN_SPI_CZ2
    #define PIN_SPI_CZ2 0
#endif

#ifndef PIN_SPI_CZ3
    #define PIN_SPI_CZ3 0
#endif

//----------------------------------------------------------------------------
// UART_SRC_SEL
//----------------------------------------------------------------------------

#ifndef UART0_SRC_SEL
    #define UART0_SRC_SEL   (UART_SEL_PIU)
#endif

#ifndef UART1_SRC_SEL
    #define UART1_SRC_SEL   (UART_SEL_HK51_UART1)
#endif

#ifndef UART2_SRC_SEL
    #define UART2_SRC_SEL   (UART_SEL_AEON)
#endif

//----------------------------------------------------------------------------
// UART_INV
//----------------------------------------------------------------------------

#ifndef UART0_INV
    #define UART0_INV   0
#endif

#ifndef UART1_INV
    #define UART1_INV   0
#endif

#ifndef UART2_INV
    #define UART2_INV   0
#endif

//----------------------------------------------------------------------------
// PIN_USBDRVBUS
//----------------------------------------------------------------------------

#ifndef PIN_USBDRVBUS
    #define PIN_USBDRVBUS   0
#endif

#endif /* _BOARD_H_ */


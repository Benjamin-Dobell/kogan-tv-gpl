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

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#include "mhal_gpio.h"
#include "mhal_gpio_reg.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#if 1
#define _CONCAT( a, b )     a##b
#define CONCAT( a, b )      _CONCAT( a, b )
/*
#define BIT0    BIT(0)
#define BIT1    BIT(1)
#define BIT2    BIT(2)
#define BIT3    BIT(3)
#define BIT4    BIT(4)
#define BIT5    BIT(5)
#define BIT6    BIT(6)
#define BIT7    BIT(7)
*/
#define EN_PCM_GPIO 1
#define EN_NOR_GPIO 0
#define EN_PF_GPIO  1

// Dummy
#define GPIO999_OEN     0, 0
#define GPIO999_OUT     0, 0
#define GPIO999_IN      0, 0

#define GPIO0_PAD PAD_PM_SPI_CZ
#define GPIO0_OEN 0x0e3c, BIT4
#define GPIO0_OUT 0x0e3a, BIT4
#define GPIO0_IN  0x0e3b, BIT4

#define GPIO1_PAD PAD_PM_SPI_CK
#define GPIO1_OEN 0x0e3c, BIT5
#define GPIO1_OUT 0x0e3a, BIT5
#define GPIO1_IN  0x0e3b, BIT5

#define GPIO2_PAD PAD_PM_SPI_DI
#define GPIO2_OEN 0x0e3c, BIT6
#define GPIO2_OUT 0x0e3a, BIT6
#define GPIO2_IN  0x0e3b, BIT6

#define GPIO3_PAD PAD_PM_SPI_DO
#define GPIO3_OEN 0x0e3c, BIT7
#define GPIO3_OUT 0x0e3a, BIT7
#define GPIO3_IN  0x0e3b, BIT7

#define GPIO4_PAD PAD_IRIN
#define GPIO4_OEN 0x0e3c, BIT0
#define GPIO4_OUT 0x0e3a, BIT0
#define GPIO4_IN  0x0e3b, BIT0

#define GPIO5_PAD PAD_CEC
#define GPIO5_OEN 0x0e3c, BIT2
#define GPIO5_OUT 0x0e3a, BIT2
#define GPIO5_IN  0x0e3b, BIT2

#define GPIO6_PAD PAD_GPIO_PM0
#define GPIO6_OEN 0x0e1e, BIT0
#define GPIO6_OUT 0x0e20, BIT0
#define GPIO6_IN  0x0e22, BIT0

#define GPIO7_PAD PAD_GPIO_PM1
#define GPIO7_OEN 0x0e1e, BIT1
#define GPIO7_OUT 0x0e20, BIT1
#define GPIO7_IN  0x0e22, BIT1

#define GPIO8_PAD PAD_GPIO_PM2
#define GPIO8_OEN 0x0e1e, BIT2
#define GPIO8_OUT 0x0e20, BIT2
#define GPIO8_IN  0x0e22, BIT2

#define GPIO9_PAD PAD_GPIO_PM3
#define GPIO9_OEN 0x0e1e, BIT3
#define GPIO9_OUT 0x0e20, BIT3
#define GPIO9_IN  0x0e22, BIT3

#define GPIO10_PAD PAD_GPIO_PM4
#define GPIO10_OEN 0x0e1e, BIT4
#define GPIO10_OUT 0x0e20, BIT4
#define GPIO10_IN  0x0e22, BIT4

#define GPIO11_PAD PAD_GPIO_PM5
#define GPIO11_OEN 0x0e1e, BIT5
#define GPIO11_OUT 0x0e20, BIT5
#define GPIO11_IN  0x0e22, BIT5

#define GPIO12_PAD PAD_GPIO_PM6
#define GPIO12_OEN 0x0e1e, BIT6
#define GPIO12_OUT 0x0e20, BIT6
#define GPIO12_IN  0x0e22, BIT6

#define GPIO13_PAD PAD_GPIO_PM7
#define GPIO13_OEN 0x0e1e, BIT7
#define GPIO13_OUT 0x0e20, BIT7
#define GPIO13_IN  0x0e22, BIT7

#define GPIO14_PAD PAD_GPIO_PM8
#define GPIO14_OEN 0x0e1f, BIT0
#define GPIO14_OUT 0x0e21, BIT0
#define GPIO14_IN  0x0e23, BIT0

#define GPIO15_PAD PAD_GPIO_PM9
#define GPIO15_OEN 0x0e1f, BIT1
#define GPIO15_OUT 0x0e21, BIT1
#define GPIO15_IN  0x0e23, BIT1

#define GPIO16_PAD PAD_GPIO_PM10
#define GPIO16_OEN 0x0e1f, BIT2
#define GPIO16_OUT 0x0e21, BIT2
#define GPIO16_IN  0x0e23, BIT2

#define GPIO17_PAD PAD_GPIO_PM11
#define GPIO17_OEN 0x0e1f, BIT3
#define GPIO17_OUT 0x0e21, BIT3
#define GPIO17_IN  0x0e23, BIT3

#define GPIO18_PAD PAD_GPIO_PM12
#define GPIO18_OEN 0x0e1f, BIT4
#define GPIO18_OUT 0x0e21, BIT4
#define GPIO18_IN  0x0e23, BIT4

#define GPIO19_PAD PAD_PWM_PM
#define GPIO19_OEN 0x0e3c, BIT1
#define GPIO19_OUT 0x0e3a, BIT1
#define GPIO19_IN  0x0e3b, BIT1

#define GPIO20_PAD PAD_HOTPLUGA
#define GPIO20_OEN 0x0e4e, BIT0
#define GPIO20_OUT 0x0e4e, BIT4
#define GPIO20_IN  0x0e4e, BIT7

#define GPIO21_PAD PAD_HOTPLUGB
#define GPIO21_OEN 0x0e4e, BIT1
#define GPIO21_OUT 0x0e4e, BIT5
#define GPIO21_IN  0x0e4f, BIT0

#define GPIO22_PAD PAD_HOTPLUGC
#define GPIO22_OEN 0x0e4e, BIT2
#define GPIO22_OUT 0x0e4e, BIT6
#define GPIO22_IN  0x0e4f, BIT1

#define GPIO23_PAD PAD_DDCDA_CK
#define GPIO23_OEN 0x0496, BIT1
#define GPIO23_OUT 0x0496, BIT2
#define GPIO23_IN  0x0496, BIT0

#define GPIO24_PAD PAD_DDCDA_DA
#define GPIO24_OEN 0x0496, BIT5
#define GPIO24_OUT 0x0496, BIT6
#define GPIO24_IN  0x0496, BIT4

#define GPIO25_PAD PAD_DDCDB_CK
#define GPIO25_OEN 0x0497, BIT1
#define GPIO25_OUT 0x0497, BIT2
#define GPIO25_IN  0x0497, BIT0

#define GPIO26_PAD PAD_DDCDB_DA
#define GPIO26_OEN 0x0497, BIT5
#define GPIO26_OUT 0x0497, BIT6
#define GPIO26_IN  0x0497, BIT4

#define GPIO27_PAD PAD_DDCDC_CK
#define GPIO27_OEN 0x0498, BIT1
#define GPIO27_OUT 0x0498, BIT2
#define GPIO27_IN  0x0498, BIT0

#define GPIO28_PAD PAD_DDCDC_DA
#define GPIO28_OEN 0x0498, BIT5
#define GPIO28_OUT 0x0498, BIT6
#define GPIO28_IN  0x0498, BIT4

#define GPIO29_PAD PAD_SAR0
#define GPIO29_OEN 0x1423, BIT0
#define GPIO29_OUT 0x1424, BIT0
#define GPIO29_IN  0x1425, BIT0

#define GPIO30_PAD PAD_SAR1
#define GPIO30_OEN 0x1423, BIT1
#define GPIO30_OUT 0x1424, BIT1
#define GPIO30_IN  0x1425, BIT1

#define GPIO31_PAD PAD_SAR2
#define GPIO31_OEN 0x1423, BIT2
#define GPIO31_OUT 0x1424, BIT2
#define GPIO31_IN  0x1425, BIT2

#define GPIO32_PAD PAD_SAR3
#define GPIO32_OEN 0x1423, BIT3
#define GPIO32_OUT 0x1424, BIT3
#define GPIO32_IN  0x1425, BIT3

#define GPIO33_PAD PAD_SAR4
#define GPIO33_OEN 0x1423, BIT4
#define GPIO33_OUT 0x1424, BIT4
#define GPIO33_IN  0x1425, BIT4

#define GPIO34_PAD PAD_GPIO0
#define GPIO34_OEN 0x102b00, BIT1
#define GPIO34_OUT 0x102b00, BIT0
#define GPIO34_IN  0x102b00, BIT2

#define GPIO35_PAD PAD_GPIO1
#define GPIO35_OEN 0x102b01, BIT1
#define GPIO35_OUT 0x102b01, BIT0
#define GPIO35_IN  0x102b01, BIT2

#define GPIO36_PAD PAD_GPIO2
#define GPIO36_OEN 0x102b02, BIT1
#define GPIO36_OUT 0x102b02, BIT0
#define GPIO36_IN  0x102b02, BIT2

#define GPIO37_PAD PAD_GPIO3
#define GPIO37_OEN 0x102b03, BIT1
#define GPIO37_OUT 0x102b03, BIT0
#define GPIO37_IN  0x102b03, BIT2

#define GPIO38_PAD PAD_GPIO4
#define GPIO38_OEN 0x102b04, BIT1
#define GPIO38_OUT 0x102b04, BIT0
#define GPIO38_IN  0x102b04, BIT2

#define GPIO39_PAD PAD_GPIO5
#define GPIO39_OEN 0x102b05, BIT1
#define GPIO39_OUT 0x102b05, BIT0
#define GPIO39_IN  0x102b05, BIT2

#define GPIO40_PAD PAD_GPIO6
#define GPIO40_OEN 0x102b06, BIT1
#define GPIO40_OUT 0x102b06, BIT0
#define GPIO40_IN  0x102b06, BIT2

#define GPIO41_PAD PAD_GPIO7
#define GPIO41_OEN 0x102b07, BIT1
#define GPIO41_OUT 0x102b07, BIT0
#define GPIO41_IN  0x102b07, BIT2

#define GPIO42_PAD PAD_GPIO8
#define GPIO42_OEN 0x102b08, BIT1
#define GPIO42_OUT 0x102b08, BIT0
#define GPIO42_IN  0x102b08, BIT2

#define GPIO43_PAD PAD_GPIO9
#define GPIO43_OEN 0x102b09, BIT1
#define GPIO43_OUT 0x102b09, BIT0
#define GPIO43_IN  0x102b09, BIT2

#define GPIO44_PAD PAD_GPIO10
#define GPIO44_OEN 0x102b0a, BIT1
#define GPIO44_OUT 0x102b0a, BIT0
#define GPIO44_IN  0x102b0a, BIT2

#define GPIO45_PAD PAD_GPIO11
#define GPIO45_OEN 0x102b0b, BIT1
#define GPIO45_OUT 0x102b0b, BIT0
#define GPIO45_IN  0x102b0b, BIT2

#define GPIO46_PAD PAD_GPIO12
#define GPIO46_OEN 0x102b0c, BIT1
#define GPIO46_OUT 0x102b0c, BIT0
#define GPIO46_IN  0x102b0c, BIT2

#define GPIO47_PAD PAD_GPIO13
#define GPIO47_OEN 0x102b0d, BIT1
#define GPIO47_OUT 0x102b0d, BIT0
#define GPIO47_IN  0x102b0d, BIT2

#define GPIO48_PAD PAD_GPIO14
#define GPIO48_OEN 0x102b0e, BIT1
#define GPIO48_OUT 0x102b0e, BIT0
#define GPIO48_IN  0x102b0e, BIT2

#define GPIO49_PAD PAD_GPIO15
#define GPIO49_OEN 0x102b0f, BIT1
#define GPIO49_OUT 0x102b0f, BIT0
#define GPIO49_IN  0x102b0f, BIT2

#define GPIO50_PAD PAD_GPIO16
#define GPIO50_OEN 0x102b10, BIT1
#define GPIO50_OUT 0x102b10, BIT0
#define GPIO50_IN  0x102b10, BIT2

#define GPIO51_PAD PAD_GPIO17
#define GPIO51_OEN 0x102b11, BIT1
#define GPIO51_OUT 0x102b11, BIT0
#define GPIO51_IN  0x102b11, BIT2

#define GPIO52_PAD PAD_GPIO18
#define GPIO52_OEN 0x102b12, BIT1
#define GPIO52_OUT 0x102b12, BIT0
#define GPIO52_IN  0x102b12, BIT2

#define GPIO53_PAD PAD_GPIO19
#define GPIO53_OEN 0x102b13, BIT1
#define GPIO53_OUT 0x102b13, BIT0
#define GPIO53_IN  0x102b13, BIT2

#define GPIO54_PAD PAD_GPIO20
#define GPIO54_OEN 0x102b14, BIT1
#define GPIO54_OUT 0x102b14, BIT0
#define GPIO54_IN  0x102b14, BIT2

#define GPIO55_PAD PAD_GPIO21
#define GPIO55_OEN 0x102b15, BIT1
#define GPIO55_OUT 0x102b15, BIT0
#define GPIO55_IN  0x102b15, BIT2

#define GPIO56_PAD PAD_GPIO22
#define GPIO56_OEN 0x102b16, BIT1
#define GPIO56_OUT 0x102b16, BIT0
#define GPIO56_IN  0x102b16, BIT2

#define GPIO57_PAD PAD_GPIO23
#define GPIO57_OEN 0x102b17, BIT1
#define GPIO57_OUT 0x102b17, BIT0
#define GPIO57_IN  0x102b17, BIT2

#define GPIO58_PAD PAD_GPIO24
#define GPIO58_OEN 0x102b18, BIT1
#define GPIO58_OUT 0x102b18, BIT0
#define GPIO58_IN  0x102b18, BIT2

#define GPIO59_PAD PAD_GPIO25
#define GPIO59_OEN 0x102b19, BIT1
#define GPIO59_OUT 0x102b19, BIT0
#define GPIO59_IN  0x102b19, BIT2

#define GPIO60_PAD PAD_GPIO26
#define GPIO60_OEN 0x102b1a, BIT1
#define GPIO60_OUT 0x102b1a, BIT0
#define GPIO60_IN  0x102b1a, BIT2

#define GPIO61_PAD PAD_GPIO27
#define GPIO61_OEN 0x102b1b, BIT1
#define GPIO61_OUT 0x102b1b, BIT0
#define GPIO61_IN  0x102b1b, BIT2

#define GPIO62_PAD PAD_UART_RX2
#define GPIO62_OEN 0x102b43, BIT1
#define GPIO62_OUT 0x102b43, BIT0
#define GPIO62_IN  0x102b43, BIT2

#define GPIO63_PAD PAD_UART_TX2
#define GPIO63_OEN 0x102b42, BIT1
#define GPIO63_OUT 0x102b42, BIT0
#define GPIO63_IN  0x102b42, BIT2

#define GPIO64_PAD PAD_PWM0
#define GPIO64_OEN 0x102b88, BIT1
#define GPIO64_OUT 0x102b88, BIT0
#define GPIO64_IN  0x102b88, BIT2

#define GPIO65_PAD PAD_PWM1
#define GPIO65_OEN 0x102b89, BIT1
#define GPIO65_OUT 0x102b89, BIT0
#define GPIO65_IN  0x102b89, BIT2

#define GPIO66_PAD PAD_PWM2
#define GPIO66_OEN 0x102b8a, BIT1
#define GPIO66_OUT 0x102b8a, BIT0
#define GPIO66_IN  0x102b8a, BIT2

#define GPIO67_PAD PAD_PWM3
#define GPIO67_OEN 0x102b8b, BIT1
#define GPIO67_OUT 0x102b8b, BIT0
#define GPIO67_IN  0x102b8b, BIT2

#define GPIO68_PAD PAD_DDCR_DA
#define GPIO68_OEN 0x102b86, BIT1
#define GPIO68_OUT 0x102b86, BIT0
#define GPIO68_IN  0x102b86, BIT2

#define GPIO69_PAD PAD_DDCR_CK
#define GPIO69_OEN 0x102b87, BIT1
#define GPIO69_OUT 0x102b87, BIT0
#define GPIO69_IN  0x102b87, BIT2

#define GPIO70_PAD PAD_TGPIO0
#define GPIO70_OEN 0x102b8d, BIT1
#define GPIO70_OUT 0x102b8d, BIT0
#define GPIO70_IN  0x102b8d, BIT2

#define GPIO71_PAD PAD_TGPIO1
#define GPIO71_OEN 0x102b8e, BIT1
#define GPIO71_OUT 0x102b8e, BIT0
#define GPIO71_IN  0x102b8e, BIT2

#define GPIO72_PAD PAD_TGPIO2
#define GPIO72_OEN 0x102b8f, BIT1
#define GPIO72_OUT 0x102b8f, BIT0
#define GPIO72_IN  0x102b8f, BIT2

#define GPIO73_PAD PAD_TGPIO3
#define GPIO73_OEN 0x102b90, BIT1
#define GPIO73_OUT 0x102b90, BIT0
#define GPIO73_IN  0x102b90, BIT2

#define GPIO74_PAD PAD_TS0_D0
#define GPIO74_OEN 0x102b1c, BIT1
#define GPIO74_OUT 0x102b1c, BIT0
#define GPIO74_IN  0x102b1c, BIT2

#define GPIO75_PAD PAD_TS0_D1
#define GPIO75_OEN 0x102b1d, BIT1
#define GPIO75_OUT 0x102b1d, BIT0
#define GPIO75_IN  0x102b1d, BIT2

#define GPIO76_PAD PAD_TS0_D2
#define GPIO76_OEN 0x102b1e, BIT1
#define GPIO76_OUT 0x102b1e, BIT0
#define GPIO76_IN  0x102b1e, BIT2

#define GPIO77_PAD PAD_TS0_D3
#define GPIO77_OEN 0x102b1f, BIT1
#define GPIO77_OUT 0x102b1f, BIT0
#define GPIO77_IN  0x102b1f, BIT2

#define GPIO78_PAD PAD_TS0_D4
#define GPIO78_OEN 0x102b20, BIT1
#define GPIO78_OUT 0x102b20, BIT0
#define GPIO78_IN  0x102b20, BIT2

#define GPIO79_PAD PAD_TS0_D5
#define GPIO79_OEN 0x102b21, BIT1
#define GPIO79_OUT 0x102b21, BIT0
#define GPIO79_IN  0x102b21, BIT2

#define GPIO80_PAD PAD_TS0_D6
#define GPIO80_OEN 0x102b22, BIT1
#define GPIO80_OUT 0x102b22, BIT0
#define GPIO80_IN  0x102b22, BIT2

#define GPIO81_PAD PAD_TS0_D7
#define GPIO81_OEN 0x102b23, BIT1
#define GPIO81_OUT 0x102b23, BIT0
#define GPIO81_IN  0x102b23, BIT2

#define GPIO82_PAD PAD_TS0_VLD
#define GPIO82_OEN 0x102b24, BIT1
#define GPIO82_OUT 0x102b24, BIT0
#define GPIO82_IN  0x102b24, BIT2

#define GPIO83_PAD PAD_TS0_SYNC
#define GPIO83_OEN 0x102b25, BIT1
#define GPIO83_OUT 0x102b25, BIT0
#define GPIO83_IN  0x102b25, BIT2

#define GPIO84_PAD PAD_TS0_CLK
#define GPIO84_OEN 0x102b26, BIT1
#define GPIO84_OUT 0x102b26, BIT0
#define GPIO84_IN  0x102b26, BIT2

#define GPIO85_PAD PAD_TS1_D0
#define GPIO85_OEN 0x102b31, BIT1
#define GPIO85_OUT 0x102b31, BIT0
#define GPIO85_IN  0x102b31, BIT2

#define GPIO86_PAD PAD_TS1_D1
#define GPIO86_OEN 0x102b30, BIT1
#define GPIO86_OUT 0x102b30, BIT0
#define GPIO86_IN  0x102b30, BIT2

#define GPIO87_PAD PAD_TS1_D2
#define GPIO87_OEN 0x102b2f, BIT1
#define GPIO87_OUT 0x102b2f, BIT0
#define GPIO87_IN  0x102b2f, BIT2

#define GPIO88_PAD PAD_TS1_D3
#define GPIO88_OEN 0x102b2e, BIT1
#define GPIO88_OUT 0x102b2e, BIT0
#define GPIO88_IN  0x102b2e, BIT2

#define GPIO89_PAD PAD_TS1_D4
#define GPIO89_OEN 0x102b2d, BIT1
#define GPIO89_OUT 0x102b2d, BIT0
#define GPIO89_IN  0x102b2d, BIT2

#define GPIO90_PAD PAD_TS1_D5
#define GPIO90_OEN 0x102b2c, BIT1
#define GPIO90_OUT 0x102b2c, BIT0
#define GPIO90_IN  0x102b2c, BIT2

#define GPIO91_PAD PAD_TS1_D6
#define GPIO91_OEN 0x102b2b, BIT1
#define GPIO91_OUT 0x102b2b, BIT0
#define GPIO91_IN  0x102b2b, BIT2

#define GPIO92_PAD PAD_TS1_D7
#define GPIO92_OEN 0x102b2a, BIT1
#define GPIO92_OUT 0x102b2a, BIT0
#define GPIO92_IN  0x102b2a, BIT2

#define GPIO93_PAD PAD_TS1_VLD
#define GPIO93_OEN 0x102b29, BIT1
#define GPIO93_OUT 0x102b29, BIT0
#define GPIO93_IN  0x102b29, BIT2

#define GPIO94_PAD PAD_TS1_SYNC
#define GPIO94_OEN 0x102b28, BIT1
#define GPIO94_OUT 0x102b28, BIT0
#define GPIO94_IN  0x102b28, BIT2

#define GPIO95_PAD PAD_TS1_CLK
#define GPIO95_OEN 0x102b27, BIT1
#define GPIO95_OUT 0x102b27, BIT0
#define GPIO95_IN  0x102b27, BIT2

#define GPIO96_PAD PAD_I2S_IN_WS
#define GPIO96_OEN 0x102b36, BIT1
#define GPIO96_OUT 0x102b36, BIT0
#define GPIO96_IN  0x102b36, BIT2

#define GPIO97_PAD PAD_I2S_IN_BCK
#define GPIO97_OEN 0x102b37, BIT1
#define GPIO97_OUT 0x102b37, BIT0
#define GPIO97_IN  0x102b37, BIT2

#define GPIO98_PAD PAD_I2S_IN_SD
#define GPIO98_OEN 0x102b38, BIT1
#define GPIO98_OUT 0x102b38, BIT0
#define GPIO98_IN  0x102b38, BIT2

#define GPIO99_PAD PAD_SPDIF_IN
#define GPIO99_OEN 0x102b39, BIT1
#define GPIO99_OUT 0x102b39, BIT0
#define GPIO99_IN  0x102b39, BIT2

#define GPIO100_PAD PAD_SPDIF_OUT
#define GPIO100_OEN 0x102b3a, BIT1
#define GPIO100_OUT 0x102b3a, BIT0
#define GPIO100_IN  0x102b3a, BIT2

#define GPIO101_PAD PAD_I2S_OUT_MCK
#define GPIO101_OEN 0x102b3c, BIT1
#define GPIO101_OUT 0x102b3c, BIT0
#define GPIO101_IN  0x102b3c, BIT2

#define GPIO102_PAD PAD_I2S_OUT_WS
#define GPIO102_OEN 0x102b3b, BIT1
#define GPIO102_OUT 0x102b3b, BIT0
#define GPIO102_IN  0x102b3b, BIT2

#define GPIO103_PAD PAD_I2S_OUT_BCK
#define GPIO103_OEN 0x102b3d, BIT1
#define GPIO103_OUT 0x102b3d, BIT0
#define GPIO103_IN  0x102b3d, BIT2

#define GPIO104_PAD PAD_I2S_OUT_SD
#define GPIO104_OEN 0x102b3e, BIT1
#define GPIO104_OUT 0x102b3e, BIT0
#define GPIO104_IN  0x102b3e, BIT2

#define GPIO105_PAD PAD_NAND_CEZ
#define GPIO105_OEN 0x102b78, BIT1
#define GPIO105_OUT 0x102b78, BIT0
#define GPIO105_IN  0x102b78, BIT2

#define GPIO106_PAD PAD_NAND_CEZ1
#define GPIO106_OEN 0x102b79, BIT1
#define GPIO106_OUT 0x102b79, BIT0
#define GPIO106_IN  0x102b79, BIT2

#define GPIO107_PAD PAD_NAND_CLE
#define GPIO107_OEN 0x102b7a, BIT1
#define GPIO107_OUT 0x102b7a, BIT0
#define GPIO107_IN  0x102b7a, BIT2

#define GPIO108_PAD PAD_NAND_REZ
#define GPIO108_OEN 0x102b7b, BIT1
#define GPIO108_OUT 0x102b7b, BIT0
#define GPIO108_IN  0x102b7b, BIT2

#define GPIO109_PAD PAD_NAND_WEZ
#define GPIO109_OEN 0x102b7c, BIT1
#define GPIO109_OUT 0x102b7c, BIT0
#define GPIO109_IN  0x102b7c, BIT2

#define GPIO110_PAD PAD_NAND_WPZ
#define GPIO110_OEN 0x102b7d, BIT1
#define GPIO110_OUT 0x102b7d, BIT0
#define GPIO110_IN  0x102b7d, BIT2

#define GPIO111_PAD PAD_NAND_ALE
#define GPIO111_OEN 0x102b7e, BIT1
#define GPIO111_OUT 0x102b7e, BIT0
#define GPIO111_IN  0x102b7e, BIT2

#define GPIO112_PAD PAD_NAND_RBZ
#define GPIO112_OEN 0x102b7f, BIT1
#define GPIO112_OUT 0x102b7f, BIT0
#define GPIO112_IN  0x102b7f, BIT2

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

static const struct gpio_setting
{
    U32 r_oen;
    U8  m_oen;
    U32 r_out;
    U8  m_out;
    U32 r_in;
    U8  m_in;
} gpio_table[] =
{
#define __GPIO__(_x_)   { CONCAT(CONCAT(GPIO, _x_), _OEN),   \
                          CONCAT(CONCAT(GPIO, _x_), _OUT),   \
                          CONCAT(CONCAT(GPIO, _x_), _IN) }
#define __GPIO(_x_)     __GPIO__(_x_)

//
// !! WARNING !! DO NOT MODIFIY !!!!
//
// These defines order must match following
// 1. the PAD name in GPIO excel
// 2. the perl script to generate the package header file
//
    //__GPIO(999), // 0 is not used

    __GPIO(0), __GPIO(1), __GPIO(2), __GPIO(3), __GPIO(4),
    __GPIO(5), __GPIO(6), __GPIO(7), __GPIO(8), __GPIO(9),
    __GPIO(10), __GPIO(11), __GPIO(12), __GPIO(13), __GPIO(14),
    __GPIO(15), __GPIO(16), __GPIO(17), __GPIO(18), __GPIO(19),
    __GPIO(20), __GPIO(21), __GPIO(22), __GPIO(23), __GPIO(24),
    __GPIO(25), __GPIO(26), __GPIO(27), __GPIO(28), __GPIO(29),
    __GPIO(30), __GPIO(31), __GPIO(32), __GPIO(33), __GPIO(34),
    __GPIO(35), __GPIO(36), __GPIO(37), __GPIO(38), __GPIO(39),
    __GPIO(40), __GPIO(41), __GPIO(42), __GPIO(43), __GPIO(44),
    __GPIO(45), __GPIO(46), __GPIO(47), __GPIO(48), __GPIO(49),
    __GPIO(50), __GPIO(51), __GPIO(52), __GPIO(53), __GPIO(54),
    __GPIO(55), __GPIO(56), __GPIO(57), __GPIO(58), __GPIO(59),
    __GPIO(60), __GPIO(61), __GPIO(62), __GPIO(63), __GPIO(64),
    __GPIO(65), __GPIO(66), __GPIO(67), __GPIO(68), __GPIO(69),
    __GPIO(70), __GPIO(71), __GPIO(72), __GPIO(73), __GPIO(74),
    __GPIO(75), __GPIO(76), __GPIO(77), __GPIO(78), __GPIO(79),
    __GPIO(80), __GPIO(81), __GPIO(82), __GPIO(83), __GPIO(84),
    __GPIO(85), __GPIO(86), __GPIO(87), __GPIO(88), __GPIO(89),
    __GPIO(90), __GPIO(91), __GPIO(92), __GPIO(93), __GPIO(94),
    __GPIO(95), __GPIO(96), __GPIO(97), __GPIO(98), __GPIO(99),
    __GPIO(100), __GPIO(101), __GPIO(102), __GPIO(103), __GPIO(104),
    __GPIO(105), __GPIO(106), __GPIO(107), __GPIO(108), __GPIO(109),
    __GPIO(110), __GPIO(111), __GPIO(112), 
};
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//the functions of this section set to initialize
void MHal_GPIO_Init(void)
{
    MHal_GPIO_REG(REG_ALL_PAD_IN) &= ~BIT7;
}

void MHal_GPIO_WriteRegBit(U32 u32Reg, U8 u8Enable, U8 u8BitMsk)
{
    if(u8Enable)
        MHal_GPIO_REG(u32Reg) |= u8BitMsk;
    else
        MHal_GPIO_REG(u32Reg) &= (~u8BitMsk);
}

U8 MHal_GPIO_ReadRegBit(U32 u32Reg, U8 u8BitMsk)
{
    return ((MHal_GPIO_REG(u32Reg)&u8BitMsk)? 1 : 0);
}

void MHal_GPIO_Pad_Set(U8 u8IndexGPIO)
{

}
void MHal_GPIO_Pad_Oen(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
}

void MHal_GPIO_Pad_Odn(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen) |= gpio_table[u8IndexGPIO].m_oen;
}

U8 MHal_GPIO_Pad_Level(U8 u8IndexGPIO)
{
    return ((MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_in)&gpio_table[u8IndexGPIO].m_in)? 1 : 0);
}

U8 MHal_GPIO_Pad_InOut(U8 u8IndexGPIO)
{
    return ((MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen)&gpio_table[u8IndexGPIO].m_oen)? 1 : 0);
}

void MHal_GPIO_Pull_High(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_out) |= gpio_table[u8IndexGPIO].m_out;
}

void MHal_GPIO_Pull_Low(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_out) &= (~gpio_table[u8IndexGPIO].m_out);
}

void MHal_GPIO_Set_High(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_out) |= gpio_table[u8IndexGPIO].m_out;
}

void MHal_GPIO_Set_Low(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_out) &= (~gpio_table[u8IndexGPIO].m_out);
}


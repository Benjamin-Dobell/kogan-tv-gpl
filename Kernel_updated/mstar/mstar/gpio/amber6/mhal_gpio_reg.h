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

#ifndef _REG_GPIO_H_
#define _REG_GPIO_H_

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define GPIO_UNIT_NUM               113

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xBF000000//Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea1)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_PM_SPI_CZ     0	 
#define PAD_PM_SPI_CK     1	 
#define PAD_PM_SPI_DI     2	 
#define PAD_PM_SPI_DO     3	 
#define PAD_IRIN          4	 
#define PAD_CEC           5	 
#define PAD_GPIO_PM0      6	 
#define PAD_GPIO_PM1      7	 
#define PAD_GPIO_PM2      8	 
#define PAD_GPIO_PM3      9	 
#define PAD_GPIO_PM4      10	
#define PAD_GPIO_PM5      11	
#define PAD_GPIO_PM6      12	
#define PAD_GPIO_PM7      13	
#define PAD_GPIO_PM8      14	
#define PAD_GPIO_PM9      15	
#define PAD_GPIO_PM10     16	
#define PAD_GPIO_PM11     17	
#define PAD_GPIO_PM12     18	
#define PAD_PWM_PM        19	
#define PAD_HOTPLUGA      20	
#define PAD_HOTPLUGB      21	
#define PAD_HOTPLUGC      22	
#define PAD_DDCDA_CK      23	
#define PAD_DDCDA_DA      24	
#define PAD_DDCDB_CK      25	
#define PAD_DDCDB_DA      26	
#define PAD_DDCDC_CK      27	
#define PAD_DDCDC_DA      28	
#define PAD_SAR0          29	
#define PAD_SAR1          30	
#define PAD_SAR2          31	
#define PAD_SAR3          32	
#define PAD_SAR4          33	
#define PAD_GPIO0         34	
#define PAD_GPIO1         35	
#define PAD_GPIO2         36	
#define PAD_GPIO3         37	
#define PAD_GPIO4         38	
#define PAD_GPIO5         39	
#define PAD_GPIO6         40	
#define PAD_GPIO7         41	
#define PAD_GPIO8         42	
#define PAD_GPIO9         43	
#define PAD_GPIO10        44	
#define PAD_GPIO11        45	
#define PAD_GPIO12        46	
#define PAD_GPIO13        47	
#define PAD_GPIO14        48	
#define PAD_GPIO15        49	
#define PAD_GPIO16        50	
#define PAD_GPIO17        51	
#define PAD_GPIO18        52	
#define PAD_GPIO19        53	
#define PAD_GPIO20        54	
#define PAD_GPIO21        55	
#define PAD_GPIO22        56	
#define PAD_GPIO23        57	
#define PAD_GPIO24        58	
#define PAD_GPIO25        59	
#define PAD_GPIO26        60	
#define PAD_GPIO27        61	
#define PAD_UART_RX2      62	
#define PAD_UART_TX2      63	
#define PAD_PWM0          64	
#define PAD_PWM1          65	
#define PAD_PWM2          66	
#define PAD_PWM3          67	
#define PAD_DDCR_DA       68	
#define PAD_DDCR_CK       69	
#define PAD_TGPIO0        70	
#define PAD_TGPIO1        71	
#define PAD_TGPIO2        72	
#define PAD_TGPIO3        73	
#define PAD_TS0_D0        74	
#define PAD_TS0_D1        75	
#define PAD_TS0_D2        76	
#define PAD_TS0_D3        77	
#define PAD_TS0_D4        78	
#define PAD_TS0_D5        79	
#define PAD_TS0_D6        80	
#define PAD_TS0_D7        81	
#define PAD_TS0_VLD       82	
#define PAD_TS0_SYNC      83	
#define PAD_TS0_CLK       84	
#define PAD_TS1_D0        85	
#define PAD_TS1_D1        86	
#define PAD_TS1_D2        87	
#define PAD_TS1_D3        88	
#define PAD_TS1_D4        89	
#define PAD_TS1_D5        90	
#define PAD_TS1_D6        91	
#define PAD_TS1_D7        92	
#define PAD_TS1_VLD       93	
#define PAD_TS1_SYNC      94	
#define PAD_TS1_CLK       95	
#define PAD_I2S_IN_WS     96	
#define PAD_I2S_IN_BCK    97	
#define PAD_I2S_IN_SD     98	
#define PAD_SPDIF_IN      99	
#define PAD_SPDIF_OUT     100
#define PAD_I2S_OUT_MCK   101
#define PAD_I2S_OUT_WS    102
#define PAD_I2S_OUT_BCK   103
#define PAD_I2S_OUT_SD    104
#define PAD_NAND_CEZ      105
#define PAD_NAND_CEZ1     106
#define PAD_NAND_CLE      107
#define PAD_NAND_REZ      108
#define PAD_NAND_WEZ      109
#define PAD_NAND_WPZ      110
#define PAD_NAND_ALE      111
#define PAD_NAND_RBZ      112

#define GPIO_OEN    0   //set o to nake output
#define GPIO_ODN    1

#define IN_HIGH    1   //input high
#define IN_LOW     0   //input low

#define OUT_HIGH    1   //output high
#define OUT_LOW     0   //output low

#define MHal_GPIO_REG(addr)             (*(volatile U8*)(REG_MIPS_BASE + (((addr) & ~1)<<1) + (addr & 1)))

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_GPIO_H_


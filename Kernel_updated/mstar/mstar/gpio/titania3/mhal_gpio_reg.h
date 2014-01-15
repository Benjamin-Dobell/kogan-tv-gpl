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
#define GPIO_UNIT_NUM               227

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xBF000000//Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea1)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_PM_SPI_CZ       0  
#define PAD_PM_SPI_CK       1  
#define PAD_PM_SPI_DI       2  
#define PAD_PM_SPI_DO       3  
#define PAD_IRIN            4  
#define PAD_CEC             5  
#define PAD_GPIO_PM0        6  
#define PAD_GPIO_PM1        7  
#define PAD_GPIO_PM2        8  
#define PAD_GPIO_PM3        9  
#define PAD_GPIO_PM4        10 
#define PAD_GPIO_PM5        11 
#define PAD_GPIO_PM6        12 
#define PAD_GPIO_PM7        13 
#define PAD_GPIO_PM8        14 
#define PAD_GPIO_PM9        15 
#define PAD_GPIO_PM10       16 
#define PAD_GPIO_PM11       17 
#define PAD_GPIO_PM12       18 
#define PAD_HOTPLUGA        19 
#define PAD_HOTPLUGB        20 
#define PAD_HOTPLUGC        21 
#define PAD_HOTPLUGD        22 
#define PAD_DDCDA_CK        23 
#define PAD_DDCDA_DA        24 
#define PAD_DDCDB_CK        25 
#define PAD_DDCDB_DA        26 
#define PAD_DDCDC_CK        27 
#define PAD_DDCDC_DA        28 
#define PAD_DDCDD_CK        29 
#define PAD_DDCDD_DA        30 
#define PAD_SAR0            31 
#define PAD_SAR1            32 
#define PAD_SAR2            33 
#define PAD_SAR3            34 
#define PAD_SAR4            35 
#define PAD_GPIO0           36 
#define PAD_GPIO1           37 
#define PAD_GPIO2           38 
#define PAD_GPIO3           39 
#define PAD_GPIO4           40 
#define PAD_GPIO5           41 
#define PAD_GPIO6           42 
#define PAD_GPIO7           43 
#define PAD_GPIO8           44 
#define PAD_GPIO9           45 
#define PAD_GPIO10          46 
#define PAD_GPIO11          47 
#define PAD_GPIO12          48 
#define PAD_GPIO13          49 
#define PAD_GPIO14          50 
#define PAD_GPIO15          51 
#define PAD_GPIO16          52 
#define PAD_GPIO17          53 
#define PAD_GPIO18          54 
#define PAD_GPIO19          55 
#define PAD_GPIO20          56 
#define PAD_GPIO21          57 
#define PAD_GPIO22          58 
#define PAD_GPIO23          59 
#define PAD_GPIO24          60 
#define PAD_GPIO25          61 
#define PAD_GPIO26          62 
#define PAD_GPIO27          63 
#define PAD_UART_RX2        64 
#define PAD_UART_TX2        65 
#define PAD_PWM0            66 
#define PAD_PWM1            67 
#define PAD_PWM2            68 
#define PAD_PWM3            69 
#define PAD_PWM4            70 
#define PAD_DDCR_DA         71 
#define PAD_DDCR_CK         72 
#define PAD_TGPIO0          73 
#define PAD_TGPIO1          74 
#define PAD_TGPIO2          75 
#define PAD_TGPIO3          76 
#define PAD_TS0_D0          77 
#define PAD_TS0_D1          78 
#define PAD_TS0_D2          79 
#define PAD_TS0_D3          80 
#define PAD_TS0_D4          81 
#define PAD_TS0_D5          82 
#define PAD_TS0_D6          83 
#define PAD_TS0_D7          84 
#define PAD_TS0_VLD         85 
#define PAD_TS0_SYNC        86 
#define PAD_TS0_CLK         87 
#define PAD_TS1_D0          88 
#define PAD_TS1_D1          89 
#define PAD_TS1_D2          90 
#define PAD_TS1_D3          91 
#define PAD_TS1_D4          92 
#define PAD_TS1_D5          93 
#define PAD_TS1_D6          94 
#define PAD_TS1_D7          95 
#define PAD_TS1_VLD         96 
#define PAD_TS1_SYNC        97 
#define PAD_TS1_CLK         98 
#define PAD_PCM_A4          99 
#define PAD_PCM_WAIT_N      100
#define PAD_PCM_A5          101
#define PAD_PCM_A6          102
#define PAD_PCM_A7          103
#define PAD_PCM_A12         104
#define PAD_PCM_IRQA_N      105
#define PAD_PCM_A14         106
#define PAD_PCM_A13         107
#define PAD_PCM_A8          108
#define PAD_PCM_IOWR_N      109
#define PAD_PCM_A9          110
#define PAD_PCM_IORD_N      111
#define PAD_PCM_A11         112
#define PAD_PCM_OE_N        113
#define PAD_PCM_A10         114
#define PAD_PCM_CE_N        115
#define PAD_PCM_D7          116
#define PAD_PCM_D6          117
#define PAD_PCM_D5          118
#define PAD_PCM_D4          119
#define PAD_PCM_D3          120
#define PAD_PCM_A3          121
#define PAD_PCM_A2          122
#define PAD_PCM_REG_N       123
#define PAD_PCM_A1          124
#define PAD_PCM_A0          125
#define PAD_PCM_D0          126
#define PAD_PCM_D1          127
#define PAD_PCM_D2          128
#define PAD_PCM_RESET       129
#define PAD_PCM_CD_N        130
#define PAD_PCM2_CE_N       131
#define PAD_PCM2_IRQA_N     132
#define PAD_PCM2_WAIT_N     133
#define PAD_PCM2_RESET      134
#define PAD_PCM2_CD_N       135
#define PAD_PF_AD15         136
#define PAD_PF_CE0Z         137
#define PAD_PF_CE1Z         138
#define PAD_PF_OEZ          139
#define PAD_PF_WEZ          140
#define PAD_PF_ALE          141
#define PAD_F_RBZ           142
#define PAD_TCON0           143
#define PAD_TCON1           144
#define PAD_TCON2           145
#define PAD_TCON3           146
#define PAD_TCON4           147
#define PAD_TCON5           148
#define PAD_TCON6           149
#define PAD_TCON7           150
#define PAD_TCON8           151
#define PAD_TCON9           152
#define PAD_TCON10          153
#define PAD_TCON11          154
#define PAD_TCON12          155
#define PAD_TCON13          156
#define PAD_TCON14          157
#define PAD_TCON15          158
#define PAD_TCON16          159
#define PAD_TCON17          160
#define PAD_TCON18          161
#define PAD_TCON19          162
#define PAD_TCON20          163
#define PAD_TCON21          164
#define PAD_ET_COL          165
#define PAD_ET_TXD1         166
#define PAD_ET_TXD0         167
#define PAD_ET_TX_EN        168
#define PAD_ET_TX_CLK       169
#define PAD_ET_RXD0         170
#define PAD_ET_RXD1         171
#define PAD_ET_MDC          172
#define PAD_ET_MDIO         173
#define PAD_I2S_IN_WS       174
#define PAD_I2S_IN_BCK      175
#define PAD_I2S_IN_SD       176
#define PAD_SPDIF_IN        177
#define PAD_SPDIF_OUT       178
#define PAD_I2S_OUT_MCK     179
#define PAD_I2S_OUT_WS      180
#define PAD_I2S_OUT_BCK     181
#define PAD_I2S_OUT_SD      182
#define PAD_I2S_OUT_SD1     183
#define PAD_I2S_OUT_SD2     184
#define PAD_I2S_OUT_SD3     185
#define PAD_B_ODD0          186
#define PAD_B_ODD1          187
#define PAD_B_ODD2          188
#define PAD_B_ODD3          189
#define PAD_B_ODD4          190
#define PAD_B_ODD5          191
#define PAD_B_ODD6          192
#define PAD_B_ODD7          193
#define PAD_G_ODD0          194
#define PAD_G_ODD1          195
#define PAD_G_ODD2          196
#define PAD_G_ODD3          197
#define PAD_G_ODD4          198
#define PAD_G_ODD5          199
#define PAD_G_ODD6          200
#define PAD_G_ODD7          201
#define PAD_R_ODD0          202
#define PAD_R_ODD1          203
#define PAD_R_ODD2          204
#define PAD_R_ODD3          205
#define PAD_R_ODD4          206
#define PAD_R_ODD5          207
#define PAD_R_ODD6          208
#define PAD_R_ODD7          209
#define PAD_mini_LVDS0      210
#define PAD_mini_LVDS1      211
#define PAD_mini_LVDS2      212
#define PAD_mini_LVDS3      213
#define PAD_mini_LVDS4      214
#define PAD_mini_LVDS5      215
#define PAD_mini_LVDS6      216
#define PAD_mini_LVDS7      217
#define PAD_LCK             218
#define PAD_LDE             219
#define PAD_LHSYNC          220
#define PAD_LVSYNC          221
#define PAD_PCM_WE_N        222


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


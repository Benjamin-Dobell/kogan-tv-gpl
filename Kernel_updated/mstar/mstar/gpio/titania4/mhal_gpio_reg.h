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
#define GPIO_UNIT_NUM               207

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xBF000000//Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea1)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_PM_SPI_CZ		0
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
#define PAD_DDCDA_CK        22
#define PAD_DDCDA_DA        23
#define PAD_DDCDB_CK        24
#define PAD_DDCDB_DA        25
#define PAD_DDCDC_CK        26
#define PAD_DDCDC_DA        27
#define PAD_SAR0            28
#define PAD_SAR1            29
#define PAD_SAR2            30
#define PAD_SAR3            31
#define PAD_GPIO0           32
#define PAD_GPIO1           33
#define PAD_GPIO2           34
#define PAD_GPIO3           35
#define PAD_GPIO4           36
#define PAD_GPIO5           37
#define PAD_GPIO6           38
#define PAD_GPIO7           39
#define PAD_GPIO8           40
#define PAD_GPIO9           41
#define PAD_GPIO10          42
#define PAD_GPIO11          43
#define PAD_GPIO12          44
#define PAD_GPIO13          45
#define PAD_GPIO14          46
#define PAD_GPIO15          47
#define PAD_GPIO16          48
#define PAD_GPIO17          49
#define PAD_GPIO18          50
#define PAD_GPIO19          51
#define PAD_GPIO20          52
#define PAD_GPIO21          53
#define PAD_GPIO22          54
#define PAD_GPIO23          55
#define PAD_GPIO24          56
#define PAD_GPIO25          57
#define PAD_GPIO26          58
#define PAD_GPIO27          59
#define PAD_PWM0            60
#define PAD_PWM1            61
#define PAD_PWM2            62
#define PAD_PWM3            63
#define PAD_DDCR_DA         64
#define PAD_DDCR_CK         65
#define PAD_TGPIO0          66
#define PAD_TGPIO1          67
#define PAD_TGPIO2          68
#define PAD_TGPIO3          69
#define PAD_TS0_D0          70
#define PAD_TS0_D1          71
#define PAD_TS0_D2          72
#define PAD_TS0_D3          73
#define PAD_TS0_D4          74
#define PAD_TS0_D5          75
#define PAD_TS0_D6          76
#define PAD_TS0_D7          77
#define PAD_TS0_VLD         78
#define PAD_TS0_SYNC        79
#define PAD_TS0_CLK         80
#define PAD_TS1_D0          81
#define PAD_TS1_D1          82
#define PAD_TS1_D2          83
#define PAD_TS1_D3          84
#define PAD_TS1_D4          85
#define PAD_TS1_D5          86
#define PAD_TS1_D6          87
#define PAD_TS1_D7          88
#define PAD_TS1_VLD         89
#define PAD_TS1_SYNC        90
#define PAD_TS1_CLK         91
#define PAD_PCM_A4          92
#define PAD_PCM_WAIT_N      93
#define PAD_PCM_A5          94
#define PAD_PCM_A6          95
#define PAD_PCM_A7          96
#define PAD_PCM_A12         97
#define PAD_PCM_IRQA_N      98
#define PAD_PCM_WE_N        99
#define PAD_PCM_A14         100
#define PAD_PCM_A13         101
#define PAD_PCM_A8          102
#define PAD_PCM_IOWR_N      103
#define PAD_PCM_A9          104
#define PAD_PCM_IORD_N      105
#define PAD_PCM_A11         106
#define PAD_PCM_OE_N        107
#define PAD_PCM_A10         108
#define PAD_PCM_CE_N        109
#define PAD_PCM_D7          110
#define PAD_PCM_D6          111
#define PAD_PCM_D5          112
#define PAD_PCM_D4          113
#define PAD_PCM_D3          114
#define PAD_PCM_A3          115
#define PAD_PCM_A2          116
#define PAD_PCM_REG_N       117
#define PAD_PCM_A1          118
#define PAD_PCM_A0          119
#define PAD_PCM_D0          120
#define PAD_PCM_D1          121
#define PAD_PCM_D2          122
#define PAD_PCM_RESET       123
#define PAD_PCM_CD_N        124
#define PAD_PCM2_CE_N       125
#define PAD_PCM2_IRQA_N     126
#define PAD_PCM2_WAIT_N     127
#define PAD_PCM2_RESET      128
#define PAD_PCM2_CD_N       129
#define PAD_PF_AD15         130
#define PAD_PF_CE0Z         131
#define PAD_PF_CE1Z         132
#define PAD_PF_OEZ          133
#define PAD_PF_WEZ          134
#define PAD_PF_ALE          135
#define PAD_F_RBZ           136
#define PAD_TCON0           137
#define PAD_TCON1           138
#define PAD_TCON2           139
#define PAD_TCON3           140
#define PAD_TCON4           141
#define PAD_TCON5           142
#define PAD_TCON6           143
#define PAD_TCON7           144
#define PAD_TCON8           145
#define PAD_TCON9           146
#define PAD_TCON10          147
#define PAD_TCON11          148
#define PAD_TCON12          149
#define PAD_TCON13          150
#define PAD_TCON14          151
#define PAD_TCON15          152
#define PAD_TCON16          153
#define PAD_TCON17          154
#define PAD_TCON18          155
#define PAD_TCON19          156
#define PAD_TCON20          157
#define PAD_TCON21          158
#define PAD_I2S_IN_WS       159
#define PAD_I2S_IN_BCK      160
#define PAD_I2S_IN_SD       161
#define PAD_SPDIF_IN        162
#define PAD_I2S_OUT_MCK     163
#define PAD_I2S_OUT_WS      164
#define PAD_I2S_OUT_BCK     165
#define PAD_I2S_OUT_SD      166
#define PAD_I2S_OUT_SD1     167
#define PAD_I2S_OUT_SD2     168
#define PAD_I2S_OUT_SD3     169
#define PAD_SPDIF_OUT       170
#define PAD_B_ODD0          171
#define PAD_B_ODD1          172
#define PAD_B_ODD2          173
#define PAD_B_ODD3          174
#define PAD_B_ODD4          175
#define PAD_B_ODD5          176
#define PAD_B_ODD6          177
#define PAD_B_ODD7          178
#define PAD_G_ODD0          179
#define PAD_G_ODD1          180
#define PAD_G_ODD2          181
#define PAD_G_ODD3          182
#define PAD_G_ODD4          183
#define PAD_G_ODD5          184
#define PAD_G_ODD6          185
#define PAD_G_ODD7          186
#define PAD_R_ODD0          187
#define PAD_R_ODD1          188
#define PAD_R_ODD2          189
#define PAD_R_ODD3          190
#define PAD_R_ODD4          191
#define PAD_R_ODD5          192
#define PAD_R_ODD6          193
#define PAD_R_ODD7          194
#define PAD_mini_LVDS0      195
#define PAD_mini_LVDS1      196
#define PAD_mini_LVDS2      197
#define PAD_mini_LVDS3      198
#define PAD_mini_LVDS4      199
#define PAD_mini_LVDS5      200
#define PAD_mini_LVDS6      201
#define PAD_mini_LVDS7      202
#define PAD_LCK             203
#define PAD_LDE             204
#define PAD_LHSYNC          205
#define PAD_LVSYNC          206


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


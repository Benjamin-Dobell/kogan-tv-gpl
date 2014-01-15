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
#define GPIO_UNIT_NUM               198

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xFD000000//Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea1)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_PM_SPI_CZ       0  
#define PAD_PM_SPI_CK       1  
#define PAD_PM_SPI_DI       2  
#define PAD_PM_SPI_DO       3  
#define PAD_IRIN            4  
#define PAD_CEC             5  
#define PAD_AV_LNK          6  
#define PAD_PWM_PM          7  
#define PAD_GPIO_PM0        8  
#define PAD_GPIO_PM1        9  
#define PAD_GPIO_PM2        10 
#define PAD_GPIO_PM3        11 
#define PAD_GPIO_PM4        12 
#define PAD_GPIO_PM5        13 
#define PAD_GPIO_PM6        14 
#define PAD_GPIO_PM7        15 
#define PAD_GPIO_PM8        16 
#define PAD_GPIO_PM9        17 
#define PAD_GPIO_PM10       18 
#define PAD_GPIO_PM11       19 
#define PAD_GPIO_PM12       20 
#define PAD_GPIO_PM13       21 
#define PAD_GPIO_PM14       22 
#define PAD_GPIO_PM15       23 
#define PAD_GPIO_PM16       24 
#define PAD_GPIO_PM17       25 
#define PAD_GPIO_PM18       26 
#define PAD_HOTPLUGA        27 
#define PAD_HOTPLUGB        28 
#define PAD_HOTPLUGC        29 
#define PAD_HOTPLUGD        30 
#define PAD_DDCA_CK         31 
#define PAD_DDCA_DA         32 
#define PAD_DDCDA_CK        33 
#define PAD_DDCDA_DA        34 
#define PAD_DDCDB_CK        35 
#define PAD_DDCDB_DA        36 
#define PAD_DDCDC_CK        37 
#define PAD_DDCDC_DA        38 
#define PAD_DDCDD_CK        39 
#define PAD_DDCDD_DA        40 
#define PAD_SAR0            41 
#define PAD_SAR1            42 
#define PAD_SAR2            43 
#define PAD_SAR3            44 
#define PAD_SAR4            45 
#define PAD_GPIO0           46 
#define PAD_GPIO1           47 
#define PAD_GPIO2           48 
#define PAD_GPIO3           49 
#define PAD_GPIO4           50 
#define PAD_GPIO5           51 
#define PAD_GPIO6           52 
#define PAD_GPIO7           53 
#define PAD_GPIO8           54 
#define PAD_GPIO9           55 
#define PAD_GPIO10          56 
#define PAD_GPIO11          57 
#define PAD_GPIO12          58 
#define PAD_GPIO13          59 
#define PAD_GPIO14          60 
#define PAD_GPIO15          61 
#define PAD_GPIO16          62 
#define PAD_GPIO17          63 
#define PAD_GPIO18          64 
#define PAD_GPIO19          65 
#define PAD_GPIO20          66 
#define PAD_GPIO21          67 
#define PAD_GPIO22          68 
#define PAD_GPIO23          69 
#define PAD_GPIO24          70 
#define PAD_GPIO25          71 
#define PAD_GPIO26          72 
#define PAD_GPIO27          73 
#define PAD_PWM0            74 
#define PAD_PWM1            75 
#define PAD_PWM2            76 
#define PAD_PWM3            77 
#define PAD_PWM4            78 
#define PAD_DDCR_DA         79 
#define PAD_DDCR_CK         80 
#define PAD_TGPIO0          81 
#define PAD_TGPIO1          82 
#define PAD_TGPIO2          83 
#define PAD_TGPIO3          84 
#define PAD_TS0_D0          85 
#define PAD_TS0_D1          86 
#define PAD_TS0_D2          87 
#define PAD_TS0_D3          88 
#define PAD_TS0_D4          89 
#define PAD_TS0_D5          90 
#define PAD_TS0_D6          91 
#define PAD_TS0_D7          92 
#define PAD_TS0_VLD         93 
#define PAD_TS0_SYNC        94 
#define PAD_TS0_CLK         95 
#define PAD_TS1_CLK         96 
#define PAD_TS1_SYNC        97 
#define PAD_TS1_VLD         98 
#define PAD_TS1_D7          99 
#define PAD_TS1_D6          100
#define PAD_TS1_D5          101
#define PAD_TS1_D4          102
#define PAD_TS1_D3          103
#define PAD_TS1_D2          104
#define PAD_TS1_D1          105
#define PAD_TS1_D0          106
#define PAD_TS2_D0          107
#define PAD_TS2_VLD         108
#define PAD_TS2_SYNC        109
#define PAD_TS2_CLK         110
#define PAD_PCM_D3          111
#define PAD_PCM_D4          112
#define PAD_PCM_D5          113
#define PAD_PCM_D6          114
#define PAD_PCM_D7          115
#define PAD_PCM_CE_N        116
#define PAD_PCM_A10         117
#define PAD_PCM_OE_N        118
#define PAD_PCM_A11         119
#define PAD_PCM_IORD_N      120
#define PAD_PCM_A9          121
#define PAD_PCM_IOWR_N      122
#define PAD_PCM_A8          123
#define PAD_PCM_A13         124
#define PAD_PCM_A14         125
#define PAD_PCM_WE_N        126
#define PAD_PCM_IRQA_N      127
#define PAD_PCM_A12         128
#define PAD_PCM_A7          129
#define PAD_PCM_A6          130
#define PAD_PCM_A5          131
#define PAD_PCM_WAIT_N      132
#define PAD_PCM_A4          133
#define PAD_PCM_A3          134
#define PAD_PCM_A2          135
#define PAD_PCM_REG_N       136
#define PAD_PCM_A1          137
#define PAD_PCM_A0          138
#define PAD_PCM_D0          139
#define PAD_PCM_D1          140
#define PAD_PCM_D2          141
#define PAD_PCM_RESET       142
#define PAD_PCM_CD_N        143
#define PAD_PCM2_CE_N       144
#define PAD_PCM2_IRQA_N     145
#define PAD_PCM2_WAIT_N     146
#define PAD_PCM2_RESET      147
#define PAD_PCM2_CD_N       148
#define PAD_NAND_CEZ        149
#define PAD_NAND_CEZ1       150
#define PAD_NAND_CLE        151
#define PAD_NAND_REZ        152
#define PAD_NAND_WEZ        153
#define PAD_NAND_WPZ        154
#define PAD_NAND_ALE        155
#define PAD_NAND_RBZ        156
#define PAD_EMMC_RSTN       157
#define PAD_EMMC_CLK        158
#define PAD_EMMC_CMD        159
#define PAD_MPIF_CLK        160
#define PAD_MPIF_BUSY_ERR   161
#define PAD_MPIF_CS_N       162
#define PAD_MPIF_D0         163
#define PAD_MPIF_D1         164
#define PAD_MPIF_D2         165
#define PAD_MPIF_D3         166
#define PAD_I2S_IN_WS       167
#define PAD_I2S_IN_BCK      168
#define PAD_I2S_IN_SD       169
#define PAD_SPDIF_IN        170
#define PAD_SPDIF_OUT       171
#define PAD_I2S_OUT_WS      172
#define PAD_I2S_OUT_MCK     173
#define PAD_I2S_OUT_BCK     174
#define PAD_I2S_OUT_SD      175
#define PAD_I2S_OUT_SD1     176
#define PAD_I2S_OUT_SD2     177
#define PAD_I2S_OUT_SD3     178
#define PAD_VSYNC_Like      179
#define PAD_SPI1_CK         180
#define PAD_SPI1_DI         181
#define PAD_SPI2_CK         182
#define PAD_SPI2_DI         183
#define PAD_TCON0           184
#define PAD_TCON1           185
#define PAD_TCON2           186
#define PAD_TCON3           187
#define PAD_TCON4           188
#define PAD_TCON5           189
#define PAD_TCON6           190
#define PAD_TCON7           191
#define PAD_TCON8           192
#define PAD_TCON9           193
#define PAD_TCON10          194
#define PAD_TCON11          195
#define PAD_TCON12          196
#define PAD_TCON13          197

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


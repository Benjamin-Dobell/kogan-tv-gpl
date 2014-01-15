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
#define GPIO_UNIT_NUM               144

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE              0xBF800000

#define REG_ALL_PAD_IN              ((0x0f50<<2) + 1)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x1900<<2)
#define REG_LVDS_BANK               REG_LVDS_BASE

//the definitions of reg set to initialize
#define REG_UART_RX2_SET            (0x0fd2<<2)
#define REG_UART_TX2_SET1           ((0x0f02<<2) + 1)
#define REG_UART_TX2_SET2           ((0x0f03<<2) + 1)
#define REG_TS0_MODE_SET            (0x0f01<<2)
#define REG_TS0_D0_SET              ((0x0f51<<2) + 1)
#define REG_TS0_D1_SET              ((0x0f51<<2) + 1)
#define REG_TS0_D2_SET              ((0x0f51<<2) + 1)
#define REG_TS0_D3_SET              ((0x0f51<<2) + 1)
#define REG_TS0_D4_SET              ((0x0f51<<2) + 1)
#define REG_TS0_D5_SET              ((0x0f51<<2) + 1)
#define REG_TS0_D6_SET              ((0x0f51<<2) + 1)
#define REG_TS0_D7_SET              ((0x0f51<<2) + 1)
#define REG_TS0_VLD_SET             ((0x0f51<<2) + 1)
#define REG_TS0_SYNC_SET            ((0x0f51<<2) + 1)
#define REG_TS0_CLK_SET             ((0x0f51<<2) + 1)
#define REG_PF_AD15_SET             ((0x0f6f<<2) + 1)
#define REG_PF_CE0Z_SET             ((0x0f6f<<2) + 1)
#define REG_PF_CE1Z_SET             ((0x0f6f<<2) + 1)
#define REG_PF_OEZ_SET              ((0x0f6f<<2) + 1)
#define REG_PF_WEZ_SET              ((0x0f6f<<2) + 1)
#define REG_PF_ALE_SET              ((0x0f6f<<2) + 1)
#define REG_F_RBZ_SET               ((0x0f6f<<2) + 1)
#define REG_PCM_D3_SET              (0x0f70<<2)
#define REG_PCM_D4_SET              (0x0f70<<2)
#define REG_PCM_D5_SET              (0x0f70<<2)
#define REG_PCM_D6_SET              (0x0f70<<2)
#define REG_PCM_D7_SET              (0x0f70<<2)
#define REG_PCM_CE_N_SET            (0x0f70<<2)
#define REG_PCM_A10_SET             (0x0f70<<2)
#define REG_PCM_OE_N_SET            (0x0f70<<2)
#define REG_PCM_A11_SET             (0x0f70<<2)
#define REG_PCM_IORD_N_SET          (0x0f70<<2)
#define REG_PCM_A9_SET              (0x0f70<<2)
#define REG_PCM_IOWR_N_SET          (0x0f70<<2)
#define REG_PCM_A8_SET              (0x0f70<<2)
#define REG_PCM_A13_SET             (0x0f70<<2)
#define REG_PCM_A14_SET             (0x0f70<<2)
#define REG_PCM_IRQA_N_SET          (0x0f70<<2)
#define REG_PCM_A12_SET             (0x0f70<<2)
#define REG_PCM_A7_SET              (0x0f70<<2)
#define REG_PCM_A6_SET              (0x0f70<<2)
#define REG_PCM_A5_SET              (0x0f70<<2)
#define REG_PCM_WAIT_N_SET          (0x0f70<<2)
#define REG_PCM_A4_SET              (0x0f70<<2)
#define REG_PCM2_CE_N_SET           (0x0f6e<<2)
#define REG_PCM2_IRQA_N_SET         (0x0f6e<<2)
#define REG_PCM2_WAIT_N_SET         (0x0f6e<<2)
#define REG_PWM0_SET                (0x0f51<<2)
#define REG_PWM1_SET                (0x0f51<<2)
#define REG_PWM2_SET                (0x0f51<<2)
#define REG_PWM3_SET                (0x0f51<<2)
#define REG_DDCR_DA_SET             (0x0f70<<2)
#define REG_DDCR_CK_SET             (0x0f70<<2)
#define REG_PCM_A3_SET              (0x0f70<<2)
#define REG_PCM_A2_SET              (0x0f70<<2)
#define REG_PCM_REG_N_SET           (0x0f70<<2)
#define REG_PCM_A1_SET              (0x0f70<<2)
#define REG_PCM_A0_SET              (0x0f70<<2)
#define REG_PCM_D0_SET              (0x0f70<<2)
#define REG_PCM_D1_SET              (0x0f70<<2)
#define REG_PCM_D2_SET              (0x0f70<<2)
#define REG_PCM_RESET_SET           (0x0f6e<<2)
#define REG_PCM2_RESET_SET          (0x0f6e<<2)
#define REG_PCM_CD_N_SET            (0x0f6e<<2)
#define REG_PCM2_CD_N_SET           (0x0f6e<<2)
#define REG_TS1_D0_SET              ((0x0f51<<2) + 1)
#define REG_TS1_VLD_SET             ((0x0f51<<2) + 1)
#define REG_TS1_SYNC_SET            ((0x0f51<<2) + 1)
#define REG_TS1_CLK_SET             ((0x0f51<<2) + 1)
#define REG_I2S_IN_WS_SET           (0x0f51<<2)
#define REG_I2S_IN_BCK_SET          (0x0f51<<2)
#define REG_I2S_IN_SD_SET           (0x0f51<<2)
#define REG_SPDIF_IN_SET            ((0x0f70<<2) + 1)
#define REG_I2S_OUT_MCK_SET         (0x0f51<<2)
#define REG_I2S_OUT_WS_SET          (0x0f51<<2)
#define REG_I2S_OUT_BCK_SET         (0x0f51<<2)
#define REG_I2S_OUT_SD_SET          (0x0f51<<2)
#define REG_SPDIF_OUT_SET           ((0x0f70<<2) + 1)
#define REG_DDCR_DA2_SET
#define REG_DDCR_CK2_SET
#define REG_LHSYNC2_SET             ((0x0f02<<2) + 1)
#define REG_LVSYNC2_SET
#define REG_SPI_CK_SET              ((0x0f6f<<2) + 1)
#define REG_SPI_DI_SET              ((0x0f6f<<2) + 1)
#define REG_SPI_CZ_SET              ((0x0f6f<<2) + 1)
#define REG_SPI_DO_SET              ((0x0f6f<<2) + 1)
#define REG_GPIO12_SET
#define REG_GPIO13_SET
#define REG_GPIO14_SET
#define REG_GPIO15_SET1             ((0x0f02<<2) + 1)
#define REG_GPIO15_SET2             (0x0f50<<2)
#define REG_GPIO15_SET3             (0x0f52<<2)
#define REG_ET_CRS_SET              ((0x0f6f<<2) + 1)
#define REG_ET_COL_SET              ((0x0f6f<<2) + 1)
#define REG_ET_TXD3_SET             ((0x0f6f<<2) + 1)
#define REG_ET_TXD2_SET             ((0x0f6f<<2) + 1)
#define REG_ET_TXD1_SET             ((0x0f6f<<2) + 1)
#define REG_ET_TXD0_SET             ((0x0f6f<<2) + 1)
#define REG_ET_TX_EN_SET            ((0x0f6f<<2) + 1)
#define REG_ET_TX_CLK_SET           ((0x0f6f<<2) + 1)
#define REG_ET_TXER_SET             ((0x0f6f<<2) + 1)
#define REG_ET_RXER_SET             ((0x0f6f<<2) + 1)
#define REG_ET_RX_CLK_SET           ((0x0f6f<<2) + 1)
#define REG_ET_RX_DV_SET            ((0x0f6f<<2) + 1)
#define REG_ET_RXD0_SET             ((0x0f6f<<2) + 1)
#define REG_ET_RXD1_SET             ((0x0f6f<<2) + 1)
#define REG_ET_RXD2_SET             ((0x0f6f<<2) + 1)
#define REG_ET_RXD3_SET             ((0x0f6f<<2) + 1)
#define REG_ET_MDC_SET              ((0x0f6f<<2) + 1)
#define REG_ET_MDIO_SET             ((0x0f6f<<2) + 1)
#define REG_LCK_SET1                 (0x194d<<2)
#define REG_LCK_SET2                 (0x1946<<2)
#define REG_LCK_SET3                 (0x196d<<2)
#define REG_LDE_SET1                 (0x194d<<2)
#define REG_LDE_SET2                 (0x1946<<2)
#define REG_LDE_SET3                 (0x196d<<2)
#define REG_LHSYNC_SET1              (0x194d<<2)
#define REG_LHSYNC_SET2              (0x1946<<2)
#define REG_LHSYNC_SET3              (0x196d<<2)
#define REG_LVSYNC_SET1              (0x194d<<2)
#define REG_LVSYNC_SET2              (0x1946<<2)
#define REG_LVSYNC_SET3              (0x196d<<2)
#define REG_GPIO_PM0_SET
#define REG_GPIO_PM1_SET
#define REG_GPIO_PM2_SET
#define REG_GPIO_PM3_SET
#define REG_GPIO_PM4_SET
#define REG_GPIO_PM5_SET
#define REG_GPIO_PM6_SET
#define REG_GPIO_PM7_SET
#define REG_GPIO_PM8_SET
#define REG_GPIO_PM9_SET
#define REG_GPIO_PM10_SET
#define REG_GPIO_SAR0_SET           0x1420  //for 8051lite
#define REG_GPIO_SAR1_SET           0x1420  //for 8051lite
#define REG_GPIO_SAR2_SET           0x1420  //for 8051lite
#define REG_GPIO_SAR3_SET           0x1420  //for 8051lite
#define REG_TCON0_SET               (0x0f50<<2)
#define REG_TCON1_SET               (0x0f50<<2)
#define REG_TCON2_SET               (0x0f50<<2)
#define REG_TCON3_SET               (0x0f50<<2)
#define REG_TCON4_SET               (0x0f50<<2)
#define REG_TCON5_SET               (0x0f50<<2)
#define REG_TCON6_SET               (0x0f50<<2)
#define REG_TCON7_SET               (0x0f50<<2)
#define REG_TCON8_SET               (0x0f50<<2)
#define REG_TCON9_SET               (0x0f50<<2)
#define REG_TCON10_SET              (0x0f50<<2)
#define REG_TCON11_SET              (0x0f50<<2)
#define REG_TCON12_SET              (0x0f50<<2)
#define REG_TCON13_SET              (0x0f50<<2)
#define REG_TCON14_SET              (0x0f50<<2)
#define REG_TCON15_SET              (0x0f50<<2)
#define REG_TCON16_SET              (0x0f50<<2)

//the definitions of reg set to make output
#define REG_UART_RX2_OEN            (0x0f30<<2)
#define REG_UART_TX2_OEN            (0x0f30<<2)
#define REG_TS0_D0_OEN              (0x0f34<<2)
#define REG_TS0_D1_OEN              (0x0f34<<2)
#define REG_TS0_D2_OEN              (0x0f34<<2)
#define REG_TS0_D3_OEN              (0x0f34<<2)
#define REG_TS0_D4_OEN              (0x0f34<<2)
#define REG_TS0_D5_OEN              (0x0f34<<2)
#define REG_TS0_D6_OEN              (0x0f34<<2)
#define REG_TS0_D7_OEN              (0x0f34<<2)
#define REG_TS0_VLD_OEN             ((0x0f34<<2) + 1)
#define REG_TS0_SYNC_OEN            ((0x0f34<<2) + 1)
#define REG_TS0_CLK_OEN             ((0x0f34<<2) + 1)
#define REG_PF_AD15_OEN             (0x0f4a<<2)
#define REG_PF_CE0Z_OEN             (0x0f4a<<2)
#define REG_PF_CE1Z_OEN             (0x0f4a<<2)
#define REG_PF_OEZ_OEN              (0x0f4a<<2)
#define REG_PF_WEZ_OEN              (0x0f4a<<2)
#define REG_PF_ALE_OEN              (0x0f4a<<2)
#define REG_F_RBZ_OEN               (0x0f4a<<2)
#define REG_PCM_D3_OEN              (0x0f3f<<2)
#define REG_PCM_D4_OEN              (0x0f3f<<2)
#define REG_PCM_D5_OEN              (0x0f3f<<2)
#define REG_PCM_D6_OEN              (0x0f3f<<2)
#define REG_PCM_D7_OEN              (0x0f3f<<2)
#define REG_PCM_CE_N_OEN            (0x0f3f<<2)
#define REG_PCM_A10_OEN             ((0x0f3e<<2) + 1)
#define REG_PCM_OE_N_OEN            ((0x0f3e<<2) + 1)
#define REG_PCM_A11_OEN             ((0x0f3e<<2) + 1)
#define REG_PCM_IORD_N_OEN          ((0x0f3e<<2) + 1)
#define REG_PCM_A9_OEN              ((0x0f3e<<2) + 1)
#define REG_PCM_IOWR_N_OEN          ((0x0f3e<<2) + 1)
#define REG_PCM_A8_OEN              ((0x0f3e<<2) + 1)
#define REG_PCM_A13_OEN             ((0x0f3e<<2) + 1)
#define REG_PCM_A14_OEN             (0x0f3e<<2)
#define REG_PCM_IRQA_N_OEN          (0x0f3e<<2)
#define REG_PCM_A12_OEN             (0x0f3e<<2)
#define REG_PCM_A7_OEN              (0x0f3e<<2)
#define REG_PCM_A6_OEN              (0x0f3e<<2)
#define REG_PCM_A5_OEN              (0x0f3e<<2)
#define REG_PCM_WAIT_N_OEN          (0x0f3e<<2)
#define REG_PCM_A4_OEN              (0x0f3e<<2)
#define REG_PCM2_CE_N_OEN           ((0x0f40<<2) + 1)
#define REG_PCM2_IRQA_N_OEN         ((0x0f40<<2) + 1)
#define REG_PCM2_WAIT_N_OEN         ((0x0f40<<2) + 1)
#define REG_PWM0_OEN                 (0x0f30<<2)
#define REG_PWM1_OEN                 (0x0f30<<2)
#define REG_PWM2_OEN                 (0x0f30<<2)
#define REG_PWM3_OEN                 (0x0f30<<2)
#define REG_DDCR_DA_OEN             (0x0f30<<2)
#define REG_DDCR_CK_OEN             (0x0f30<<2)
#define REG_PCM_A3_OEN              (0x0f3f<<2)
#define REG_PCM_A2_OEN              ((0x0f3f<<2) + 1)
#define REG_PCM_REG_N_OEN           ((0x0f3f<<2) + 1)
#define REG_PCM_A1_OEN              ((0x0f3f<<2) + 1)
#define REG_PCM_A0_OEN              ((0x0f3f<<2) + 1)
#define REG_PCM_D0_OEN              ((0x0f3f<<2) + 1)
#define REG_PCM_D1_OEN              ((0x0f3f<<2) + 1)
#define REG_PCM_D2_OEN              ((0x0f3f<<2) + 1)
#define REG_PCM_RESET_OEN           ((0x0f3f<<2) + 1)
#define REG_PCM2_RESET_OEN          ((0x0f40<<2) + 1)
#define REG_PCM_CD_N_OEN            (0x0f40<<2)
#define REG_PCM2_CD_N_OEN           ((0x0f40<<2) + 1)
#define REG_TS1_D0_OEN              ((0x0f34<<2) + 1)
#define REG_TS1_VLD_OEN             ((0x0f34<<2) + 1)
#define REG_TS1_SYNC_OEN            ((0x0f34<<2) + 1)
#define REG_TS1_CLK_OEN             ((0x0f34<<2) + 1)
#define REG_I2S_IN_WS_OEN           (0x0f37<<2)
#define REG_I2S_IN_BCK_OEN          (0x0f37<<2)
#define REG_I2S_IN_SD_OEN           (0x0f37<<2)
#define REG_SPDIF_IN_OEN            (0x0f37<<2)
#define REG_I2S_OUT_MCK_OEN         (0x0f37<<2)
#define REG_I2S_OUT_WS_OEN          (0x0f37<<2)
#define REG_I2S_OUT_BCK_OEN         (0x0f37<<2)
#define REG_I2S_OUT_SD_OEN          ((0x0f37<<2) + 1)
#define REG_SPDIF_OUT_OEN           (0x0f37<<2)
#define REG_DDCR_DA2_OEN            ((0x0f30<<2) + 1)
#define REG_DDCR_CK2_OEN            ((0x0f30<<2) + 1)
#define REG_LHSYNC2_OEN             ((0x0f30<<2) + 1)
#define REG_LVSYNC2_OEN             ((0x0f30<<2) + 1)
#define REG_SPI_CK_OEN              ((0x0f37<<2) + 1)
#define REG_SPI_DI_OEN              ((0x0f37<<2) + 1)
#define REG_SPI_CZ_OEN              ((0x0f37<<2) + 1)
#define REG_SPI_DO_OEN              ((0x0f37<<2) + 1)
#define REG_GPIO12_OEN              ((0x0f30<<2) + 1)
#define REG_GPIO13_OEN              ((0x0f30<<2) + 1)
#define REG_GPIO14_OEN              ((0x0f30<<2) + 1)
#define REG_GPIO15_OEN              ((0x0f30<<2) + 1)
#define REG_ET_CRS_OEN              ((0x0f44<<2) + 1)
#define REG_ET_COL_OEN              (0x0f45<<2)
#define REG_ET_TXD3_OEN             ((0x0f44<<2) + 1)
#define REG_ET_TXD2_OEN             ((0x0f44<<2) + 1)
#define REG_ET_TXD1_OEN             (0x0f45<<2)
#define REG_ET_TXD0_OEN             (0x0f45<<2)
#define REG_ET_TX_EN_OEN            (0x0f45<<2)
#define REG_ET_TX_CLK_OEN           (0x0f45<<2)
#define REG_ET_TXER_OEN             ((0x0f44<<2) + 1)
#define REG_ET_RXER_OEN             ((0x0f44<<2) + 1)
#define REG_ET_RX_CLK_OEN           ((0x0f44<<2) + 1)
#define REG_ET_RX_DV_OEN            ((0x0f44<<2) + 1)
#define REG_ET_RXD0_OEN             (0x0f45<<2)
#define REG_ET_RXD1_OEN             (0x0f45<<2)
#define REG_ET_RXD2_OEN             ((0x0f44<<2) + 1)
#define REG_ET_RXD3_OEN             (0x0f45<<2)
#define REG_ET_MDC_OEN              ((0x0f45<<2) + 1)
#define REG_ET_MDIO_OEN             ((0x0f45<<2) + 1)
#define REG_LCK_OEN                 (0x1951<<2)
#define REG_LDE_OEN                 (0x1951<<2)
#define REG_LHSYNC_OEN              (0x1951<<2)
#define REG_LVSYNC_OEN              (0x1951<<2)
#define REG_GPIO_PM0_OEN            0x1002  //for 8051lite
#define REG_GPIO_PM1_OEN            0x1002  //for 8051lite
#define REG_GPIO_PM2_OEN            0x1002  //for 8051lite
#define REG_GPIO_PM3_OEN            0x1002  //for 8051lite
#define REG_GPIO_PM4_OEN            0x1002  //for 8051lite
#define REG_GPIO_PM5_OEN            0x1002  //for 8051lite
#define REG_GPIO_PM6_OEN            0x1002  //for 8051lite
#define REG_GPIO_PM7_OEN            0x1002  //for 8051lite
#define REG_GPIO_PM8_OEN            0x1003  //for 8051lite
#define REG_GPIO_PM9_OEN            0x1003  //for 8051lite
#define REG_GPIO_PM10_OEN           0x1003  //for 8051lite
#define REG_GPIO_SAR0_OEN           0x1420  //for 8051lite
#define REG_GPIO_SAR1_OEN           0x1420  //for 8051lite
#define REG_GPIO_SAR2_OEN           0x1420  //for 8051lite
#define REG_GPIO_SAR3_OEN           0x1420  //for 8051lite
#define REG_TCON0_OEN               (0x0f8c<<2)
#define REG_TCON1_OEN               (0x0f8c<<2)
#define REG_TCON2_OEN               (0x0f8c<<2)
#define REG_TCON3_OEN               (0x0f8c<<2)
#define REG_TCON4_OEN               (0x0f8c<<2)
#define REG_TCON5_OEN               (0x0f8c<<2)
#define REG_TCON6_OEN               (0x0f8c<<2)
#define REG_TCON7_OEN               (0x0f8c<<2)
#define REG_TCON8_OEN               ((0x0f8c<<2) + 1)
#define REG_TCON9_OEN               ((0x0f8c<<2) + 1)
#define REG_TCON10_OEN              ((0x0f8c<<2) + 1)
#define REG_TCON11_OEN              ((0x0f8c<<2) + 1)
#define REG_TCON12_OEN              ((0x0f8c<<2) + 1)
#define REG_TCON13_OEN              ((0x0f8c<<2) + 1)
#define REG_TCON14_OEN              ((0x0f8c<<2) + 1)
#define REG_TCON15_OEN              ((0x0f8c<<2) + 1)
#define REG_TCON16_OEN              (0x0f8d<<2)
#define REG_HOTPLUGB_OEN            (0x12e8<<2)
#define REG_HOTPLUGA_OEN            (0x12e8<<2)
#define REG_HOTPLUGC_OEN            (0x12eb<<2)

//the definitions of reg set to get input
#define REG_UART_RX2_IN             (0x0f2e<<2)
#define REG_UART_TX2_IN             (0x0f2e<<2)
#define REG_TS0_D0_IN               (0x0f31<<2)
#define REG_TS0_D1_IN               (0x0f31<<2)
#define REG_TS0_D2_IN               (0x0f31<<2)
#define REG_TS0_D3_IN               (0x0f31<<2)
#define REG_TS0_D4_IN               (0x0f31<<2)
#define REG_TS0_D5_IN               (0x0f31<<2)
#define REG_TS0_D6_IN               (0x0f31<<2)
#define REG_TS0_D7_IN               (0x0f31<<2)
#define REG_TS0_VLD_IN              ((0x0f31<<2) + 1)
#define REG_TS0_SYNC_IN             ((0x0f31<<2) + 1)
#define REG_TS0_CLK_IN              ((0x0f31<<2) + 1)
#define REG_PF_AD15_IN              (0x0f46<<2)
#define REG_PF_CE0Z_IN              (0x0f46<<2)
#define REG_PF_CE1Z_IN              (0x0f46<<2)
#define REG_PF_OEZ_IN               (0x0f46<<2)
#define REG_PF_WEZ_IN               (0x0f46<<2)
#define REG_PF_ALE_IN               (0x0f46<<2)
#define REG_F_RBZ_IN                (0x0f46<<2)
#define REG_PCM_D3_IN               (0x0f39<<2)
#define REG_PCM_D4_IN               (0x0f39<<2)
#define REG_PCM_D5_IN               (0x0f39<<2)
#define REG_PCM_D6_IN               (0x0f39<<2)
#define REG_PCM_D7_IN               (0x0f39<<2)
#define REG_PCM_CE_N_IN             (0x0f39<<2)
#define REG_PCM_A10_IN              ((0x0f38<<2) + 1)
#define REG_PCM_OE_N_IN             ((0x0f38<<2) + 1)
#define REG_PCM_A11_IN              ((0x0f38<<2) + 1)
#define REG_PCM_IORD_N_IN           ((0x0f38<<2) + 1)
#define REG_PCM_A9_IN               ((0x0f38<<2) + 1)
#define REG_PCM_IOWR_N_IN           ((0x0f38<<2) + 1)
#define REG_PCM_A8_IN               ((0x0f38<<2) + 1)
#define REG_PCM_A13_IN              ((0x0f38<<2) + 1)
#define REG_PCM_A14_IN              (0x0f38<<2)
#define REG_PCM_IRQA_N_IN           (0x0f38<<2)
#define REG_PCM_A12_IN              (0x0f38<<2)
#define REG_PCM_A7_IN               (0x0f38<<2)
#define REG_PCM_A6_IN               (0x0f38<<2)
#define REG_PCM_A5_IN               (0x0f38<<2)
#define REG_PCM_WAIT_N_IN           (0x0f38<<2)
#define REG_PCM_A4_IN               (0x0f38<<2)
#define REG_PCM2_CE_N_IN            ((0x0f3a<<2) + 1)
#define REG_PCM2_IRQA_N_IN          ((0x0f3a<<2) + 1)
#define REG_PCM2_WAIT_N_IN          ((0x0f3a<<2) + 1)
#define REG_PWM0_IN                 (0x0f2e<<2)
#define REG_PWM1_IN                 (0x0f2e<<2)
#define REG_PWM2_IN                 (0x0f2e<<2)
#define REG_PWM3_IN                 (0x0f2e<<2)
#define REG_DDCR_DA_IN              (0x0f2e<<2)
#define REG_DDCR_CK_IN              (0x0f2e<<2)
#define REG_PCM_A3_IN               (0x0f39<<2)
#define REG_PCM_A2_IN               ((0x0f39<<2) + 1)
#define REG_PCM_REG_N_IN            ((0x0f39<<2) + 1)
#define REG_PCM_A1_IN               ((0x0f39<<2) + 1)
#define REG_PCM_A0_IN               ((0x0f39<<2) + 1)
#define REG_PCM_D0_IN               ((0x0f39<<2) + 1)
#define REG_PCM_D1_IN               ((0x0f39<<2) + 1)
#define REG_PCM_D2_IN               ((0x0f39<<2) + 1)
#define REG_PCM_RESET_IN            ((0x0f39<<2) + 1)
#define REG_PCM2_RESET_IN           ((0x0f3a<<2) + 1)
#define REG_PCM_CD_N_IN             (0x0f3a<<2)
#define REG_PCM2_CD_N_IN            ((0x0f3a<<2) + 1)
#define REG_TS1_D0_IN               ((0x0f31<<2) + 1)
#define REG_TS1_VLD_IN              ((0x0f31<<2) + 1)
#define REG_TS1_SYNC_IN             ((0x0f31<<2) + 1)
#define REG_TS1_CLK_IN              ((0x0f31<<2) + 1)
#define REG_I2S_IN_WS_IN            (0x0f35<<2)
#define REG_I2S_IN_BCK_IN           (0x0f35<<2)
#define REG_I2S_IN_SD_IN            (0x0f35<<2)
#define REG_SPDIF_IN_IN             (0x0f35<<2)
#define REG_I2S_OUT_MCK_IN          (0x0f35<<2)
#define REG_I2S_OUT_WS_IN           (0x0f35<<2)
#define REG_I2S_OUT_BCK_IN          (0x0f35<<2)
#define REG_I2S_OUT_SD_IN           ((0x0f35<<2) + 1)
#define REG_SPDIF_OUT_IN            (0x0f35<<2)
#define REG_DDCR_DA2_IN             ((0x0f2e<<2) + 1)
#define REG_DDCR_CK2_IN             ((0x0f2e<<2) + 1)
#define REG_LHSYNC2_IN              ((0x0f2e<<2) + 1)
#define REG_LVSYNC2_IN              ((0x0f2e<<2) + 1)
#define REG_SPI_CK_IN               ((0x0f35<<2) + 1)
#define REG_SPI_DI_IN               ((0x0f35<<2) + 1)
#define REG_SPI_CZ_IN               ((0x0f35<<2) + 1)
#define REG_SPI_DO_IN               ((0x0f35<<2) + 1)
#define REG_GPIO12_IN               ((0x0f2e<<2) + 1)
#define REG_GPIO13_IN               ((0x0f2e<<2) + 1)
#define REG_GPIO14_IN               ((0x0f2e<<2) + 1)
#define REG_GPIO15_IN               ((0x0f2e<<2) + 1)
#define REG_ET_CRS_IN               (0x0f41<<2)
#define REG_ET_COL_IN               ((0x0f41<<2) + 1)
#define REG_ET_TXD3_IN              (0x0f41<<2)
#define REG_ET_TXD2_IN              (0x0f41<<2)
#define REG_ET_TXD1_IN              ((0x0f41<<2) + 1)
#define REG_ET_TXD0_IN              ((0x0f41<<2) + 1)
#define REG_ET_TX_EN_IN             ((0x0f41<<2) + 1)
#define REG_ET_TX_CLK_IN            ((0x0f41<<2) + 1)
#define REG_ET_TXER_IN              (0x0f41<<2)
#define REG_ET_RXER_IN              (0x0f41<<2)
#define REG_ET_RX_CLK_IN            (0x0f41<<2)
#define REG_ET_RX_DV_IN             (0x0f41<<2)
#define REG_ET_RXD0_IN              ((0x0f41<<2) + 1)
#define REG_ET_RXD1_IN              ((0x0f41<<2) + 1)
#define REG_ET_RXD2_IN              (0x0f41<<2)
#define REG_ET_RXD3_IN              ((0x0f41<<2) + 1)
#define REG_ET_MDC_IN               (0x0f42<<2)
#define REG_ET_MDIO_IN              (0x0f42<<2)
#define REG_LCK_IN                  (0x1955<<2)
#define REG_LDE_IN                  (0x1955<<2)
#define REG_LHSYNC_IN               (0x1955<<2)
#define REG_LVSYNC_IN               (0x1955<<2)
#define REG_GPIO_PM0_IN             0x1006  //for 8051lite
#define REG_GPIO_PM1_IN             0x1006  //for 8051lite
#define REG_GPIO_PM2_IN             0x1006  //for 8051lite
#define REG_GPIO_PM3_IN             0x1006  //for 8051lite
#define REG_GPIO_PM4_IN             0x1006  //for 8051lite
#define REG_GPIO_PM5_IN             0x1006  //for 8051lite
#define REG_GPIO_PM6_IN             0x1006  //for 8051lite
#define REG_GPIO_PM7_IN             0x1006  //for 8051lite
#define REG_GPIO_PM8_IN             0x1007  //for 8051lite
#define REG_GPIO_PM9_IN             0x1007  //for 8051lite
#define REG_GPIO_PM10_IN            0x1007  //for 8051lite
#define REG_GPIO_SAR0_IN            0x1424  //for 8051lite
#define REG_GPIO_SAR1_IN            0x1424  //for 8051lite
#define REG_GPIO_SAR2_IN            0x1424  //for 8051lite
#define REG_GPIO_SAR3_IN            0x1424  //for 8051lite
#define REG_TCON0_IN                (0x0f04<<2)
#define REG_TCON1_IN                (0x0f04<<2)
#define REG_TCON2_IN                (0x0f04<<2)
#define REG_TCON3_IN                (0x0f04<<2)
#define REG_TCON4_IN                (0x0f04<<2)
#define REG_TCON5_IN                (0x0f04<<2)
#define REG_TCON6_IN                (0x0f04<<2)
#define REG_TCON7_IN                (0x0f04<<2)
#define REG_TCON8_IN                ((0x0f04<<2) + 1)
#define REG_TCON9_IN                ((0x0f04<<2) + 1)
#define REG_TCON10_IN               ((0x0f04<<2) + 1)
#define REG_TCON11_IN               ((0x0f04<<2) + 1)
#define REG_TCON12_IN               ((0x0f04<<2) + 1)
#define REG_TCON13_IN               ((0x0f04<<2) + 1)
#define REG_TCON14_IN               ((0x0f04<<2) + 1)
#define REG_TCON15_IN               ((0x0f04<<2) + 1)
#define REG_TCON16_IN               (0x0f05<<2)
#define REG_HOTPLUGB_IN             (0x12e8<<2)
#define REG_HOTPLUGA_IN             (0x12e8<<2)
#define REG_HOTPLUGC_IN             (0x12eb<<2)

//the definitions of reg set to output
#define REG_UART_RX2_OUT            (0x0f2f<<2)
#define REG_UART_TX2_OUT            (0x0f2f<<2)
#define REG_TS0_D0_OUT              (0x0f32<<2)
#define REG_TS0_D1_OUT              (0x0f32<<2)
#define REG_TS0_D2_OUT              (0x0f32<<2)
#define REG_TS0_D3_OUT              (0x0f32<<2)
#define REG_TS0_D4_OUT              (0x0f32<<2)
#define REG_TS0_D5_OUT              (0x0f32<<2)
#define REG_TS0_D6_OUT              (0x0f32<<2)
#define REG_TS0_D7_OUT              (0x0f32<<2)
#define REG_TS0_VLD_OUT             ((0x0f32<<2) + 1)
#define REG_TS0_SYNC_OUT            ((0x0f32<<2) + 1)
#define REG_TS0_CLK_OUT             ((0x0f32<<2) + 1)
#define REG_PF_AD15_OUT             (0x0f48<<2)
#define REG_PF_CE0Z_OUT             (0x0f48<<2)
#define REG_PF_CE1Z_OUT             (0x0f48<<2)
#define REG_PF_OEZ_OUT              (0x0f48<<2)
#define REG_PF_WEZ_OUT              (0x0f48<<2)
#define REG_PF_ALE_OUT              (0x0f48<<2)
#define REG_F_RBZ_OUT               (0x0f48<<2)
#define REG_PCM_D3_OUT              (0x0f3c<<2)
#define REG_PCM_D4_OUT              (0x0f3c<<2)
#define REG_PCM_D5_OUT              (0x0f3c<<2)
#define REG_PCM_D6_OUT              (0x0f3c<<2)
#define REG_PCM_D7_OUT              (0x0f3c<<2)
#define REG_PCM_CE_N_OUT            (0x0f3c<<2)
#define REG_PCM_A10_OUT             ((0x0f3b<<2) + 1)
#define REG_PCM_OE_N_OUT            ((0x0f3b<<2) + 1)
#define REG_PCM_A11_OUT             ((0x0f3b<<2) + 1)
#define REG_PCM_IORD_N_OUT          ((0x0f3b<<2) + 1)
#define REG_PCM_A9_OUT              ((0x0f3b<<2) + 1)
#define REG_PCM_IOWR_N_OUT          ((0x0f3b<<2) + 1)
#define REG_PCM_A8_OUT              ((0x0f3b<<2) + 1)
#define REG_PCM_A13_OUT             ((0x0f3b<<2) + 1)
#define REG_PCM_A14_OUT             (0x0f3b<<2)
#define REG_PCM_IRQA_N_OUT          (0x0f3b<<2)
#define REG_PCM_A12_OUT             (0x0f3b<<2)
#define REG_PCM_A7_OUT              (0x0f3b<<2)
#define REG_PCM_A6_OUT              (0x0f3b<<2)
#define REG_PCM_A5_OUT              (0x0f3b<<2)
#define REG_PCM_WAIT_N_OUT          (0x0f3b<<2)
#define REG_PCM_A4_OUT              (0x0f3b<<2)
#define REG_PCM2_CE_N_OUT           ((0x0f3d<<2) + 1)
#define REG_PCM2_IRQA_N_OUT         ((0x0f3d<<2) + 1)
#define REG_PCM2_WAIT_N_OUT         ((0x0f3d<<2) + 1)
#define REG_PWM0_OUT                (0x0f2f<<2)
#define REG_PWM1_OUT                (0x0f2f<<2)
#define REG_PWM2_OUT                (0x0f2f<<2)
#define REG_PWM3_OUT                (0x0f2f<<2)
#define REG_DDCR_DA_OUT             (0x0f2f<<2)
#define REG_DDCR_CK_OUT             (0x0f2f<<2)
#define REG_PCM_A3_OUT              (0x0f3c<<2)
#define REG_PCM_A2_OUT              ((0x0f3c<<2) + 1)
#define REG_PCM_REG_N_OUT           ((0x0f3c<<2) + 1)
#define REG_PCM_A1_OUT              ((0x0f3c<<2) + 1)
#define REG_PCM_A0_OUT              ((0x0f3c<<2) + 1)
#define REG_PCM_D0_OUT              ((0x0f3c<<2) + 1)
#define REG_PCM_D1_OUT              ((0x0f3c<<2) + 1)
#define REG_PCM_D2_OUT              ((0x0f3c<<2) + 1)
#define REG_PCM_RESET_OUT           ((0x0f3c<<2) + 1)
#define REG_PCM2_RESET_OUT          ((0x0f3d<<2) + 1)
#define REG_PCM_CD_N_OUT            (0x0f3d<<2)
#define REG_PCM2_CD_N_OUT           ((0x0f3d<<2) + 1)
#define REG_TS1_D0_OUT              ((0x0f33<<2) + 1)
#define REG_TS1_VLD_OUT             ((0x0f33<<2) + 1)
#define REG_TS1_SYNC_OUT            ((0x0f33<<2) + 1)
#define REG_TS1_CLK_OUT             ((0x0f33<<2) + 1)
#define REG_I2S_IN_WS_OUT           (0x0f36<<2)
#define REG_I2S_IN_BCK_OUT          (0x0f36<<2)
#define REG_I2S_IN_SD_OUT           (0x0f36<<2)
#define REG_SPDIF_IN_OUT            (0x0f36<<2)
#define REG_I2S_OUT_MCK_OUT         (0x0f36<<2)
#define REG_I2S_OUT_WS_OUT          (0x0f36<<2)
#define REG_I2S_OUT_BCK_OUT         (0x0f36<<2)
#define REG_I2S_OUT_SD_OUT          ((0x0f36<<2) + 1)
#define REG_SPDIF_OUT_OUT           (0x0f36<<2)
#define REG_DDCR_DA2_OUT            ((0x0f2f<<2) + 1)
#define REG_DDCR_CK2_OUT            ((0x0f2f<<2) + 1)
#define REG_LHSYNC2_OUT             ((0x0f2f<<2) + 1)
#define REG_LVSYNC2_OUT             ((0x0f2f<<2) + 1)
#define REG_SPI_CK_OUT              ((0x0f36<<2) + 1)
#define REG_SPI_DI_OUT              ((0x0f36<<2) + 1)
#define REG_SPI_CZ_OUT              ((0x0f36<<2) + 1)
#define REG_SPI_DO_OUT              ((0x0f36<<2) + 1)
#define REG_GPIO12_OUT              ((0x0f2f<<2) + 1)
#define REG_GPIO13_OUT              ((0x0f2f<<2) + 1)
#define REG_GPIO14_OUT              ((0x0f2f<<2) + 1)
#define REG_GPIO15_OUT              ((0x0f2f<<2) + 1)
#define REG_ET_CRS_OUT              (0x0f43<<2)
#define REG_ET_COL_OUT              ((0x0f43<<2) + 1)
#define REG_ET_TXD3_OUT             (0x0f43<<2)
#define REG_ET_TXD2_OUT             (0x0f43<<2)
#define REG_ET_TXD1_OUT             ((0x0f43<<2) + 1)
#define REG_ET_TXD0_OUT             ((0x0f43<<2) + 1)
#define REG_ET_TX_EN_OUT            ((0x0f43<<2) + 1)
#define REG_ET_TX_CLK_OUT           ((0x0f43<<2) + 1)
#define REG_ET_TXER_OUT             (0x0f43<<2)
#define REG_ET_RXER_OUT             (0x0f43<<2)
#define REG_ET_RX_CLK_OUT           (0x0f43<<2)
#define REG_ET_RX_DV_OUT            (0x0f43<<2)
#define REG_ET_RXD0_OUT             ((0x0f43<<2) + 1)
#define REG_ET_RXD1_OUT             ((0x0f43<<2) + 1)
#define REG_ET_RXD2_OUT             (0x0f43<<2)
#define REG_ET_RXD3_OUT             ((0x0f43<<2) + 1)
#define REG_ET_MDC_OUT              (0x0f44<<2)
#define REG_ET_MDIO_OUT             (0x0f44<<2)
#define REG_LCK_OUT                 (0x194f<<2)
#define REG_LDE_OUT                 (0x194f<<2)
#define REG_LHSYNC_OUT              (0x194f<<2)
#define REG_LVSYNC_OUT              (0x194f<<2)
#define REG_GPIO_PM0_OUT            0x1004  //for 8051lite
#define REG_GPIO_PM1_OUT            0x1004  //for 8051lite
#define REG_GPIO_PM2_OUT            0x1004  //for 8051lite
#define REG_GPIO_PM3_OUT            0x1004  //for 8051lite
#define REG_GPIO_PM4_OUT            0x1004  //for 8051lite
#define REG_GPIO_PM5_OUT            0x1004  //for 8051lite
#define REG_GPIO_PM6_OUT            0x1004  //for 8051lite
#define REG_GPIO_PM7_OUT            0x1004  //for 8051lite
#define REG_GPIO_PM8_OUT            0x1005  //for 8051lite
#define REG_GPIO_PM9_OUT            0x1005  //for 8051lite
#define REG_GPIO_PM10_OUT           0x1005  //for 8051lite
#define REG_GPIO_SAR0_OUT           0x1421  //for 8051lite
#define REG_GPIO_SAR1_OUT           0x1421  //for 8051lite
#define REG_GPIO_SAR2_OUT           0x1421  //for 8051lite
#define REG_GPIO_SAR3_OUT           0x1421  //for 8051lite
#define REG_TCON0_OUT               (0x0f08<<2)
#define REG_TCON1_OUT               (0x0f08<<2)
#define REG_TCON2_OUT               (0x0f08<<2)
#define REG_TCON3_OUT               (0x0f08<<2)
#define REG_TCON4_OUT               (0x0f08<<2)
#define REG_TCON5_OUT               (0x0f08<<2)
#define REG_TCON6_OUT               (0x0f08<<2)
#define REG_TCON7_OUT               (0x0f08<<2)
#define REG_TCON8_OUT               ((0x0f08<<2) + 1)
#define REG_TCON9_OUT               ((0x0f08<<2) + 1)
#define REG_TCON10_OUT              ((0x0f08<<2) + 1)
#define REG_TCON11_OUT              ((0x0f08<<2) + 1)
#define REG_TCON12_OUT              ((0x0f08<<2) + 1)
#define REG_TCON13_OUT              ((0x0f08<<2) + 1)
#define REG_TCON14_OUT              ((0x0f08<<2) + 1)
#define REG_TCON15_OUT              ((0x0f08<<2) + 1)
#define REG_TCON16_OUT              (0x0f09<<2)
#define REG_HOTPLUGB_OUT            (0x12e8<<2)
#define REG_HOTPLUGA_OUT            (0x12e8<<2)
#define REG_HOTPLUGC_OUT            (0x12eb<<2)

//define pad number
#define PAD_UART_RX2        0
#define PAD_UART_TX2        1
#define PAD_TS0_D0          2
#define PAD_TS0_D1          3
#define PAD_TS0_D2          4
#define PAD_TS0_D3          5
#define PAD_TS0_D4          6
#define PAD_TS0_D5          7
#define PAD_TS0_D6          8
#define PAD_TS0_D7          9
#define PAD_TS0_VLD         10
#define PAD_TS0_SYNC        11
#define PAD_TS0_CLK         12
#define PAD_PF_AD15         13
#define PAD_PF_CE0Z         14
#define PAD_PF_CE1Z         15
#define PAD_PF_OEZ          16
#define PAD_PF_WEZ          17
#define PAD_PF_ALE          18
#define PAD_F_RBZ           19
#define PAD_PCM_D3          20
#define PAD_PCM_D4          21
#define PAD_PCM_D5          22
#define PAD_PCM_D6          23
#define PAD_PCM_D7          24
#define PAD_PCM_CE_N        25
#define PAD_PCM_A10         26
#define PAD_PCM_OE_N        27
#define PAD_PCM_A11         28
#define PAD_PCM_IORD_N      29
#define PAD_PCM_A9          30
#define PAD_PCM_IOWR_N      31
#define PAD_PCM_A8          32
#define PAD_PCM_A13         33
#define PAD_PCM_A14         34
#define PAD_PCM_IRQA_N      35
#define PAD_PCM_A12         36
#define PAD_PCM_A7          37
#define PAD_PCM_A6          38
#define PAD_PCM_A5          39
#define PAD_PCM_WAIT_N      40
#define PAD_PCM_A4           41
#define PAD_PCM2_CE_N       42
#define PAD_PCM2_IRQA_N     43
#define PAD_PCM2_WAIT_N     44
#define PAD_PWM0             45
#define PAD_PWM1             46
#define PAD_PWM2             47
#define PAD_PWM3             48
#define PAD_DDCR_DA         49
#define PAD_DDCR_CK         50
#define PAD_PCM_A3          51
#define PAD_PCM_A2          52
#define PAD_PCM_REG_N       53
#define PAD_PCM_A1          54
#define PAD_PCM_A0          55
#define PAD_PCM_D0          56
#define PAD_PCM_D1          57
#define PAD_PCM_D2          58
#define PAD_PCM_RESET       59
#define PAD_PCM2_RESET      60
#define PAD_PCM_CD_N        61
#define PAD_PCM2_CD_N       62
#define PAD_TS1_D0          63
#define PAD_TS1_VLD         64
#define PAD_TS1_SYNC        65
#define PAD_TS1_CLK         66
#define PAD_I2S_IN_WS       67
#define PAD_I2S_IN_BCK      68
#define PAD_I2S_IN_SD       69
#define PAD_SPDIF_IN        70
#define PAD_I2S_OUT_MCK     71
#define PAD_I2S_OUT_WS      72
#define PAD_I2S_OUT_BCK     73
#define PAD_I2S_OUT_SD      74
#define PAD_SPDIF_OUT       75
#define PAD_DDCR_DA2        76
#define PAD_DDCR_CK2        77
#define PAD_LHSYNC2         78
#define PAD_LVSYNC2         79
#define PAD_SPI_CK          80
#define PAD_SPI_DI          81
#define PAD_SPI_CZ          82
#define PAD_SPI_DO          83
#define PAD_GPIO12          84
#define PAD_GPIO13          85
#define PAD_GPIO14          86
#define PAD_GPIO15          87
#define PAD_ET_CRS          88
#define PAD_ET_COL          89
#define PAD_ET_TXD3         90
#define PAD_ET_TXD2         91
#define PAD_ET_TXD1         92
#define PAD_ET_TXD0         93
#define PAD_ET_TX_EN        94
#define PAD_ET_TX_CLK       95
#define PAD_ET_TXER         96
#define PAD_ET_RXER         97
#define PAD_ET_RX_CLK       98
#define PAD_ET_RX_DV        99
#define PAD_ET_RXD0         100
#define PAD_ET_RXD1         101
#define PAD_ET_RXD2         102
#define PAD_ET_RXD3         103
#define PAD_ET_MDC          104
#define PAD_ET_MDIO         105
#define PAD_LCK             130
#define PAD_LDE             131
#define PAD_LHSYNC          132
#define PAD_LVSYNC          133
#define PAD_GPIO_PM0        134
#define PAD_GPIO_PM1        135
#define PAD_GPIO_PM2        136
#define PAD_GPIO_PM3        137
#define PAD_GPIO_PM4        138
#define PAD_GPIO_PM5        139
#define PAD_GPIO_PM6        140
#define PAD_GPIO_PM7        141
#define PAD_GPIO_PM8        142
#define PAD_GPIO_PM9        143
#define PAD_GPIO_PM10       144
#define PAD_GPIO_SAR0       146
#define PAD_GPIO_SAR1       147
#define PAD_GPIO_SAR2       148
#define PAD_GPIO_SAR3       149
#define PAD_TCON0           150
#define PAD_TCON1           151
#define PAD_TCON2           152
#define PAD_TCON3           153
#define PAD_TCON4           154
#define PAD_TCON5           155
#define PAD_TCON6           156
#define PAD_TCON7           157
#define PAD_TCON8           158
#define PAD_TCON9           159
#define PAD_TCON10          160
#define PAD_TCON11          161
#define PAD_TCON12          162
#define PAD_TCON13          163
#define PAD_TCON14          164
#define PAD_TCON15          165
#define PAD_TCON16          166
#define PAD_HOTPLUGB        167
#define PAD_HOTPLUGA        168
#define PAD_HOTPLUGC        169

#define GPIO_OEN    0   //set o to nake output
#define GPIO_ODN    1

#define IN_HIGH    1   //input high
#define IN_LOW     0   //input low

#define OUT_HIGH    1   //output high
#define OUT_LOW     0   //output low

#define MHal_GPIO_REG(addr)             (*(volatile U8*)(REG_MIPS_BASE + (addr)))

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_GPIO_H_


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


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


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

void MHal_GPIO_UART_RX2_Set(void)
{
    MHal_GPIO_REG(REG_UART_RX2_SET) &= ~BIT0;
}

void MHal_GPIO_UART_TX2_Set(void)
{
    MHal_GPIO_REG(REG_UART_TX2_SET1) &= ~BIT2;
    MHal_GPIO_REG(REG_UART_TX2_SET1) &= ~BIT3;
    MHal_GPIO_REG(REG_UART_TX2_SET2) &= ~BIT1;
    MHal_GPIO_REG(REG_UART_TX2_SET2) &= ~BIT2;
    MHal_GPIO_REG(REG_UART_TX2_SET2) &= ~BIT3;
}

void MHal_GPIO_TS0_D0_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_D0_SET) |= BIT2;
}

void MHal_GPIO_TS0_D1_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_D1_SET) |= BIT2;
}

void MHal_GPIO_TS0_D2_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_D2_SET) |= BIT2;
}

void MHal_GPIO_TS0_D3_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_D3_SET) |= BIT2;
}

void MHal_GPIO_TS0_D4_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_D4_SET) |= BIT2;
}

void MHal_GPIO_TS0_D5_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_D5_SET) |= BIT2;
}

void MHal_GPIO_TS0_D6_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_D6_SET) |= BIT2;
}

void MHal_GPIO_TS0_D7_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_D7_SET) |= BIT2;
}

void MHal_GPIO_TS0_VLD_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_VLD_SET) |= BIT2;
}

void MHal_GPIO_TS0_SYNC_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_SYNC_SET) |= BIT2;
}

void MHal_GPIO_TS0_CLK_Set(void)
{
    MHal_GPIO_REG(REG_TS0_MODE_SET) &= ~BIT6;
    MHal_GPIO_REG(REG_TS0_CLK_SET) |= BIT2;
}

void MHal_GPIO_PF_AD15_Set(void)
{
    MHal_GPIO_REG(REG_PF_AD15_SET) |= BIT4;
}

void MHal_GPIO_PF_CE0Z_Set(void)
{
    MHal_GPIO_REG(REG_PF_CE0Z_SET) |= BIT4;
}

void MHal_GPIO_PF_CE1Z_Set(void)
{
    MHal_GPIO_REG(REG_PF_CE1Z_SET) |= BIT4;
}

void MHal_GPIO_PF_OEZ_Set(void)
{
    MHal_GPIO_REG(REG_PF_OEZ_SET) |= BIT4;
}

void MHal_GPIO_PF_WEZ_Set(void)
{
    MHal_GPIO_REG(REG_PF_WEZ_SET) |= BIT4;
}

void MHal_GPIO_PF_ALE_Set(void)
{
    MHal_GPIO_REG(REG_PF_ALE_SET) |= BIT4;
}

void MHal_GPIO_F_RBZ_Set(void)
{
    MHal_GPIO_REG(REG_F_RBZ_SET) |= BIT5;
}

void MHal_GPIO_PCM_D3_Set(void)
{
    MHal_GPIO_REG(REG_PCM_D3_SET) |= BIT4;
}

void MHal_GPIO_PCM_D4_Set(void)
{
    MHal_GPIO_REG(REG_PCM_D4_SET) |= BIT4;
}

void MHal_GPIO_PCM_D5_Set(void)
{
    MHal_GPIO_REG(REG_PCM_D5_SET) |= BIT2;
}

void MHal_GPIO_PCM_D6_Set(void)
{
    MHal_GPIO_REG(REG_PCM_D6_SET) |= BIT2;
}

void MHal_GPIO_PCM_D7_Set(void)
{
    MHal_GPIO_REG(REG_PCM_D7_SET) |= BIT5;
}

void MHal_GPIO_PCM_CE_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM_CE_N_SET) |= BIT5;
}

void MHal_GPIO_PCM_A8_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A8_SET) |= BIT5;
}

void MHal_GPIO_PCM_A9_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A9_SET) |= BIT5;
}

void MHal_GPIO_PCM_A10_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A10_SET) |= BIT5;
}

void MHal_GPIO_PCM_A11_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A11_SET) |= BIT5;
}

void MHal_GPIO_PCM_OE_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM_OE_N_SET) |= BIT5;
}

void MHal_GPIO_PCM_IORD_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM_IORD_N_SET) |= BIT5;
}

void MHal_GPIO_PCM_IOWR_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM_IOWR_N_SET) |= BIT5;
}

void MHal_GPIO_PCM_IRQA_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM_IRQA_N_SET) |= BIT1;
}

void MHal_GPIO_PCM_A4_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A4_SET) |= BIT0;
}

void MHal_GPIO_PCM_A5_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A5_SET) |= BIT1;
}

void MHal_GPIO_PCM_A6_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A6_SET) |= BIT1;
}

void MHal_GPIO_PCM_A7_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A7_SET) |= BIT1;
}

void MHal_GPIO_PCM_A12_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A12_SET) |= BIT1;
}

void MHal_GPIO_PCM_A13_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A13_SET) |= BIT5;
}

void MHal_GPIO_PCM_A14_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A14_SET) |= BIT5;
}

void MHal_GPIO_PCM_WAIT_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM_WAIT_N_SET) |= BIT0;
}

void MHal_GPIO_PCM2_CE_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM2_CE_N_SET) |= BIT5;
    MHal_GPIO_REG(REG_PCM2_CE_N_SET) |= BIT4;
}

void MHal_GPIO_PCM2_IRQA_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM2_IRQA_N_SET) |= BIT5;
    MHal_GPIO_REG(REG_PCM2_IRQA_N_SET) |= BIT4;
}

void MHal_GPIO_PCM2_WAIT_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM2_WAIT_N_SET) |= BIT5;
    MHal_GPIO_REG(REG_PCM2_WAIT_N_SET) |= BIT4;
}

void MHal_GPIO_PWM0_Set(void)
{
    MHal_GPIO_REG(REG_PWM0_SET) |= BIT0;
}

void MHal_GPIO_PWM1_Set(void)
{
    MHal_GPIO_REG(REG_PWM1_SET) |= BIT1;
}

void MHal_GPIO_PWM2_Set(void)
{
    MHal_GPIO_REG(REG_PWM2_SET) |= BIT2;
}

void MHal_GPIO_PWM3_Set(void)
{
    MHal_GPIO_REG(REG_PWM3_SET) |= BIT3;
}

void MHal_GPIO_DDCR_DA_Set(void)
{
    MHal_GPIO_REG(REG_DDCR_DA_SET) |= BIT7;
}

void MHal_GPIO_DDCR_CK_Set(void)
{
    MHal_GPIO_REG(REG_DDCR_CK_SET) |= BIT7;
}

void MHal_GPIO_PCM_A0_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A0_SET) |= BIT3;
}

void MHal_GPIO_PCM_A1_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A1_SET) |= BIT3;
}

void MHal_GPIO_PCM_A2_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A2_SET) |= BIT3;
}

void MHal_GPIO_PCM_A3_Set(void)
{
    MHal_GPIO_REG(REG_PCM_A3_SET) |= BIT3;
}

void MHal_GPIO_PCM_REG_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM_REG_N_SET) |= BIT3;
}

void MHal_GPIO_PCM_D0_Set(void)
{
    MHal_GPIO_REG(REG_PCM_D0_SET) |= BIT3;
}

void MHal_GPIO_PCM_D1_Set(void)
{
    MHal_GPIO_REG(REG_PCM_D1_SET) |= BIT3;
}

void MHal_GPIO_PCM_D2_Set(void)
{
    MHal_GPIO_REG(REG_PCM_D2_SET) |= BIT3;
}

void MHal_GPIO_PCM_RESET_Set(void)
{
    MHal_GPIO_REG(REG_PCM_RESET_SET) |= BIT3;
    MHal_GPIO_REG(REG_PCM_RESET_SET) |= BIT2;
}

void MHal_GPIO_PCM2_RESET_Set(void)
{
    MHal_GPIO_REG(REG_PCM2_RESET_SET) |= BIT7;
    MHal_GPIO_REG(REG_PCM2_RESET_SET) &= ~BIT6;
}

void MHal_GPIO_PCM_CD_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM_CD_N_SET) |= BIT3;
    MHal_GPIO_REG(REG_PCM_CD_N_SET) |= BIT2;
}

void MHal_GPIO_PCM2_CD_N_Set(void)
{
    MHal_GPIO_REG(REG_PCM2_CD_N_SET) |= BIT7;
    MHal_GPIO_REG(REG_PCM2_CD_N_SET) &= ~BIT6;
}

void MHal_GPIO_TS1_D0_Set(void)
{
    MHal_GPIO_REG(REG_TS1_D0_SET) |= BIT3;
}

void MHal_GPIO_TS1_VLD_Set(void)
{
    MHal_GPIO_REG(REG_TS1_VLD_SET) |= BIT3;
}

void MHal_GPIO_TS1_SYNC_Set(void)
{
    MHal_GPIO_REG(REG_TS1_SYNC_SET) |= BIT3;
}

void MHal_GPIO_TS1_CLK_Set(void)
{
    MHal_GPIO_REG(REG_TS1_CLK_SET) |= BIT3;
}

void MHal_GPIO_I2S_IN_WS_Set(void)
{
    MHal_GPIO_REG(REG_I2S_IN_WS_SET) |= BIT4;
}

void MHal_GPIO_I2S_IN_BCK_Set(void)
{
    MHal_GPIO_REG(REG_I2S_IN_BCK_SET) |= BIT4;
}

void MHal_GPIO_I2S_IN_SD_Set(void)
{
    MHal_GPIO_REG(REG_I2S_IN_SD_SET) |= BIT4;
}

void MHal_GPIO_SPDIF_IN_Set(void)
{
    MHal_GPIO_REG(REG_SPDIF_IN_SET) |= BIT6;
}

void MHal_GPIO_I2S_OUT_MCK_Set(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_MCK_SET) |= BIT5;
}

void MHal_GPIO_I2S_OUT_WS_Set(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_WS_SET) |= BIT5;
}

void MHal_GPIO_I2S_OUT_BCK_Set(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_WS_SET) |= BIT5;
}

void MHal_GPIO_I2S_OUT_SD_Set(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_SD_SET) |= BIT5;
}

void MHal_GPIO_SPDIF_OUT_Set(void)
{
    MHal_GPIO_REG(REG_SPDIF_OUT_SET) |= BIT7;
}

void MHal_GPIO_LHSYNC2_Set(void)
{
    MHal_GPIO_REG(REG_LHSYNC2_SET) &= ~(BIT7|BIT6);
}

void MHal_GPIO_SPI_CK_Set(void)
{
    MHal_GPIO_REG(REG_SPI_CK_SET) |= BIT6;
}

void MHal_GPIO_SPI_DI_Set(void)
{
    MHal_GPIO_REG(REG_SPI_DI_SET) |= BIT6;
}

void MHal_GPIO_SPI_CZ_Set(void)
{
    MHal_GPIO_REG(REG_SPI_CZ_SET) |= BIT6;
}

void MHal_GPIO_SPI_DO_Set(void)
{
    MHal_GPIO_REG(REG_SPI_DO_SET) |= BIT6;
}

void MHal_GPIO_GPIO15_Set(void)
{
    MHal_GPIO_REG(REG_GPIO15_SET1) &= ~BIT0;
    MHal_GPIO_REG(REG_GPIO15_SET1) &= ~BIT1;
    MHal_GPIO_REG(REG_GPIO15_SET2) &= ~BIT1;
    MHal_GPIO_REG(REG_GPIO15_SET3) &= ~BIT7;
}

void MHal_GPIO_ET_CRS_Set(void)
{
    MHal_GPIO_REG(REG_ET_CRS_SET) |= BIT2;
}

void MHal_GPIO_ET_COL_Set(void)
{
    MHal_GPIO_REG(REG_ET_CRS_SET) |= BIT3;
}

void MHal_GPIO_ET_TXD2_Set(void)
{
    MHal_GPIO_REG(REG_ET_TXD2_SET) |= BIT2;
}

void MHal_GPIO_ET_TXD3_Set(void)
{
    MHal_GPIO_REG(REG_ET_TXD3_SET) |= BIT2;
}

void MHal_GPIO_ET_TXD0_Set(void)
{
    MHal_GPIO_REG(REG_ET_TXD0_SET) |= BIT3;
}

void MHal_GPIO_ET_TXD1_Set(void)
{
    MHal_GPIO_REG(REG_ET_TXD1_SET) |= BIT3;
}

void MHal_GPIO_ET_TX_EN_Set(void)
{
    MHal_GPIO_REG(REG_ET_TX_EN_SET) |= BIT3;
}

void MHal_GPIO_ET_TX_CLK_Set(void)
{
    MHal_GPIO_REG(REG_ET_TX_CLK_SET) |= BIT3;
}

void MHal_GPIO_ET_TXER_Set(void)
{
    MHal_GPIO_REG(REG_ET_TXER_SET) |= BIT2;
}

void MHal_GPIO_ET_RXER_Set(void)
{
    MHal_GPIO_REG(REG_ET_TXER_SET) |= BIT2;
}

void MHal_GPIO_ET_RX_CLK_Set(void)
{
    MHal_GPIO_REG(REG_ET_RX_CLK_SET) |= BIT2;
}

void MHal_GPIO_ET_RX_DV_Set(void)
{
    MHal_GPIO_REG(REG_ET_RX_DV_SET) |= BIT2;
}

void MHal_GPIO_ET_RXD0_Set(void)
{
    MHal_GPIO_REG(REG_ET_RXD0_SET) |= BIT3;
}

void MHal_GPIO_ET_RXD1_Set(void)
{
    MHal_GPIO_REG(REG_ET_RXD1_SET) |= BIT3;
}

void MHal_GPIO_ET_RXD2_Set(void)
{
    MHal_GPIO_REG(REG_ET_RXD2_SET) |= BIT2;
}

void MHal_GPIO_ET_RXD3_Set(void)
{
    MHal_GPIO_REG(REG_ET_RXD3_SET) |= BIT2;
}

void MHal_GPIO_ET_MDC_Set(void)
{
    MHal_GPIO_REG(REG_ET_MDC_SET) |= BIT3;
}

void MHal_GPIO_ET_MDIO_Set(void)
{
    MHal_GPIO_REG(REG_ET_MDIO_SET) |= BIT3;
}

void MHal_GPIO_LCK_Set(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LCK_SET1) |= BIT2;
    MHal_GPIO_REG(REG_LCK_SET1) |= BIT3;
    MHal_GPIO_REG(REG_LCK_SET2) |= BIT2;
    MHal_GPIO_REG(REG_LCK_SET2) |= BIT3;
    MHal_GPIO_REG(REG_LCK_SET3) &= ~BIT2;
    MHal_GPIO_REG(REG_LCK_SET3) &= ~BIT3;
}

void MHal_GPIO_LDE_Set(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LDE_SET1) |= BIT2;
    MHal_GPIO_REG(REG_LDE_SET1) |= BIT3;
    MHal_GPIO_REG(REG_LDE_SET2) |= BIT2;
    MHal_GPIO_REG(REG_LDE_SET2) |= BIT3;
    MHal_GPIO_REG(REG_LDE_SET3) &= ~BIT2;
    MHal_GPIO_REG(REG_LDE_SET3) &= ~BIT3;
}

void MHal_GPIO_LHSYNC_Set(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LHSYNC_SET1) |= BIT0;
    MHal_GPIO_REG(REG_LHSYNC_SET1) |= BIT1;
    MHal_GPIO_REG(REG_LHSYNC_SET2) |= BIT0;
    MHal_GPIO_REG(REG_LHSYNC_SET2) |= BIT1;
    MHal_GPIO_REG(REG_LHSYNC_SET3) &= ~BIT0;
    MHal_GPIO_REG(REG_LHSYNC_SET3) &= ~BIT1;
}

void MHal_GPIO_LVSYNC_Set(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LVSYNC_SET1) |= BIT0;
    MHal_GPIO_REG(REG_LVSYNC_SET1) |= BIT1;
    MHal_GPIO_REG(REG_LVSYNC_SET2) |= BIT0;
    MHal_GPIO_REG(REG_LVSYNC_SET2) |= BIT1;
    MHal_GPIO_REG(REG_LVSYNC_SET3) &= ~BIT0;
    MHal_GPIO_REG(REG_LVSYNC_SET3) &= ~BIT1;
}

void MHal_GPIO_TCON0_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON1_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON2_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON3_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON4_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON5_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON6_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON7_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON8_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON9_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON10_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON11_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON12_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON13_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON14_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON15_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

void MHal_GPIO_TCON16_Set(void)
{
    MHal_GPIO_REG(REG_TCON0_SET) |= BIT6;
}

//the functions of this section set to make output
void MHal_GPIO_UART_RX2_Oen(void)
{
    MHal_GPIO_REG(REG_UART_RX2_OEN) &= ~BIT0;
}

void MHal_GPIO_UART_TX2_Oen(void)
{
    MHal_GPIO_REG(REG_UART_TX2_OEN) &= ~BIT1;
}

void MHal_GPIO_TS0_D0_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_D0_OEN) &= ~BIT0;
}

void MHal_GPIO_TS0_D1_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_D1_OEN) &= ~BIT1;
}

void MHal_GPIO_TS0_D2_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_D2_OEN) &= ~BIT2;
}

void MHal_GPIO_TS0_D3_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_D3_OEN) &= ~BIT3;
}

void MHal_GPIO_TS0_D4_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_D4_OEN) &= ~BIT4;
}

void MHal_GPIO_TS0_D5_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_D5_OEN) &= ~BIT5;
}

void MHal_GPIO_TS0_D6_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_D6_OEN) &= ~BIT6;
}

void MHal_GPIO_TS0_D7_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_D7_OEN) &= ~BIT7;
}

void MHal_GPIO_TS0_VLD_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_VLD_OEN) &= ~BIT0;
}

void MHal_GPIO_TS0_SYNC_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_SYNC_OEN) &= ~BIT1;
}

void MHal_GPIO_TS0_CLK_Oen(void)
{
    MHal_GPIO_REG(REG_TS0_CLK_OEN) &= ~BIT2;
}

void MHal_GPIO_PF_AD15_Oen(void)
{
    MHal_GPIO_REG(REG_PF_AD15_OEN) &= ~BIT0;
}

void MHal_GPIO_PF_CE0Z_Oen(void)
{
    MHal_GPIO_REG(REG_PF_CE0Z_OEN) &= ~BIT1;
}

void MHal_GPIO_PF_CE1Z_Oen(void)
{
    MHal_GPIO_REG(REG_PF_CE1Z_OEN) &= ~BIT2;
}

void MHal_GPIO_PF_OEZ_Oen(void)
{
    MHal_GPIO_REG(REG_PF_OEZ_OEN) &= ~BIT3;
}

void MHal_GPIO_PF_WEZ_Oen(void)
{
    MHal_GPIO_REG(REG_PF_WEZ_OEN) &= ~BIT4;
}

void MHal_GPIO_PF_ALE_Oen(void)
{
    MHal_GPIO_REG(REG_PF_ALE_OEN) &= ~BIT5;
}

void MHal_GPIO_F_RBZ_Oen(void)
{
    MHal_GPIO_REG(REG_F_RBZ_OEN) &= ~BIT6;
}

void MHal_GPIO_PCM_D3_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_D3_OEN) &= ~BIT6;
}

void MHal_GPIO_PCM_D4_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_D4_OEN) &= ~BIT5;
}

void MHal_GPIO_PCM_D5_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_D5_OEN) &= ~BIT4;
}

void MHal_GPIO_PCM_D6_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_D6_OEN) &= ~BIT3;
}

void MHal_GPIO_PCM_D7_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_D7_OEN) &= ~BIT1;
}

void MHal_GPIO_PCM_CE_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_CE_N_OEN) &= ~BIT0;
}

void MHal_GPIO_PCM_A10_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A10_OEN) &= ~BIT7;
}

void MHal_GPIO_PCM_OE_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_OE_N_OEN) &= ~BIT6;
}

void MHal_GPIO_PCM_A11_Oen(void)
{
MHal_GPIO_REG(REG_PCM_A11_OEN) &= ~BIT5;
}

void MHal_GPIO_PCM_IORD_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_IORD_N_OEN) &= ~BIT4;
}

void MHal_GPIO_PCM_A9_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A9_OEN) &= ~BIT3;
}

void MHal_GPIO_PCM_IOWR_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_IOWR_N_OEN) &= ~BIT2;
}

void MHal_GPIO_PCM_A8_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A8_OEN) &= ~BIT1;
}

void MHal_GPIO_PCM_A13_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A13_OEN) &= ~BIT0;
}

void MHal_GPIO_PCM_A14_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A14_OEN) &= ~BIT7;
}

void MHal_GPIO_PCM_IRQA_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_IRQA_N_OEN) &= ~BIT6;
}
void MHal_GPIO_PCM_A12_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A12_OEN) &= ~BIT5;
}

void MHal_GPIO_PCM_A7_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A7_OEN) &= ~BIT4;
}

void MHal_GPIO_PCM_A6_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A6_OEN) &= ~BIT3;
}

void MHal_GPIO_PCM_A5_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A5_OEN) &= ~BIT2;
}

void MHal_GPIO_PCM_WAIT_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_WAIT_N_OEN) &= ~BIT1;
}

void MHal_GPIO_PCM_A4_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A4_OEN) &= ~BIT0;
}

void MHal_GPIO_PCM2_CE_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM2_CE_N_OEN) &= ~BIT0;
}

void MHal_GPIO_PCM2_IRQA_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM2_IRQA_N_OEN) &= ~BIT1;
}

void MHal_GPIO_PCM2_WAIT_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM2_WAIT_N_OEN) &= ~BIT2;
}

void MHal_GPIO_PWM0_Oen(void)
{
    MHal_GPIO_REG(REG_PWM0_OEN) &= ~BIT2;
}

void MHal_GPIO_PWM1_Oen(void)
{
    MHal_GPIO_REG(REG_PWM1_OEN) &= ~BIT3;
}

void MHal_GPIO_PWM2_Oen(void)
{
    MHal_GPIO_REG(REG_PWM2_OEN) &= ~BIT4;
}

void MHal_GPIO_PWM3_Oen(void)
{
    MHal_GPIO_REG(REG_PWM3_OEN) &= ~BIT5;
}

void MHal_GPIO_DDCR_DA_Oen(void)
{
    MHal_GPIO_REG(REG_DDCR_DA_OEN) &= ~BIT6;
}

void MHal_GPIO_DDCR_CK_Oen(void)
{
    MHal_GPIO_REG(REG_DDCR_CK_OEN) &= ~BIT7;
}

void MHal_GPIO_PCM_A3_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A3_OEN) &= ~BIT7;
}

void MHal_GPIO_PCM_A2_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A2_OEN) &= ~BIT0;
}

void MHal_GPIO_PCM_REG_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_REG_N_OEN) &= ~BIT1;
}

void MHal_GPIO_PCM_A1_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A1_OEN) &= ~BIT2;
}

void MHal_GPIO_PCM_A0_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_A0_OEN) &= ~BIT3;
}

void MHal_GPIO_PCM_D0_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_D0_OEN) &= ~BIT4;
}

void MHal_GPIO_PCM_D1_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_D1_OEN) &= ~BIT5;
}

void MHal_GPIO_PCM_D2_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_D2_OEN) &= ~BIT6;
}

void MHal_GPIO_PCM_RESET_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_RESET_OEN) &= ~BIT7;
}

void MHal_GPIO_PCM2_RESET_Oen(void)
{
    MHal_GPIO_REG(REG_PCM2_RESET_OEN) &= ~BIT3;
}

void MHal_GPIO_PCM_CD_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM_CD_N_OEN) &= ~BIT0;
}

void MHal_GPIO_PCM2_CD_N_Oen(void)
{
    MHal_GPIO_REG(REG_PCM2_CD_N_OEN) &= ~BIT4;
}

void MHal_GPIO_TS1_D0_Oen(void)
{
    MHal_GPIO_REG(REG_TS1_D0_OEN) &= ~BIT4;
}

void MHal_GPIO_TS1_VLD_Oen(void)
{
    MHal_GPIO_REG(REG_TS1_VLD_OEN) &= ~BIT5;
}

void MHal_GPIO_TS1_SYNC_Oen(void)
{
    MHal_GPIO_REG(REG_TS1_SYNC_OEN) &= ~BIT6;
}

void MHal_GPIO_TS1_CLK_Oen(void)
{
    MHal_GPIO_REG(REG_TS1_CLK_OEN) &= ~BIT7;
}

void MHal_GPIO_I2S_IN_WS_Oen(void)
{
    MHal_GPIO_REG(REG_I2S_IN_WS_OEN) &= ~BIT0;
}

void MHal_GPIO_I2S_IN_BCK_Oen(void)
{
    MHal_GPIO_REG(REG_I2S_IN_BCK_OEN) &= ~BIT1;
}

void MHal_GPIO_I2S_IN_SD_Oen(void)
{
    MHal_GPIO_REG(REG_I2S_IN_SD_OEN) &= ~BIT2;
}

void MHal_GPIO_SPDIF_IN_Oen(void)
{
    MHal_GPIO_REG(REG_SPDIF_IN_OEN) &= ~BIT3;
}

void MHal_GPIO_I2S_OUT_MCK_Oen(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_MCK_OEN) &= ~BIT5;
}

void MHal_GPIO_I2S_OUT_WS_Oen(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_WS_OEN) &= ~BIT6;
}

void MHal_GPIO_I2S_OUT_BCK_Oen(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_BCK_OEN) &= ~BIT7;
}

void MHal_GPIO_I2S_OUT_SD_Oen(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_SD_OEN) &= ~BIT0;
}

void MHal_GPIO_SPDIF_OUT_Oen(void)
{
    MHal_GPIO_REG(REG_SPDIF_OUT_OEN) &= ~BIT4;
}

void MHal_GPIO_DDCR_DA2_Oen(void)
{
    MHal_GPIO_REG(REG_DDCR_DA2_OEN) &= ~BIT0;
}

void MHal_GPIO_DDCR_CK2_Oen(void)
{
    MHal_GPIO_REG(REG_DDCR_CK2_OEN) &= ~BIT1;
}

void MHal_GPIO_LHSYNC2_Oen(void)
{
    MHal_GPIO_REG(REG_LHSYNC2_OEN) &= ~BIT2;
}

void MHal_GPIO_LVSYNC2_Oen(void)
{
    MHal_GPIO_REG(REG_LVSYNC2_OEN) &= ~BIT3;
}

void MHal_GPIO_SPI_CK_Oen(void)
{
    MHal_GPIO_REG(REG_SPI_CK_OEN) &= ~BIT4;
}

void MHal_GPIO_SPI_DI_Oen(void)
{
    MHal_GPIO_REG(REG_SPI_DI_OEN) &= ~BIT5;
}

void MHal_GPIO_SPI_CZ_Oen(void)
{
    MHal_GPIO_REG(REG_SPI_CZ_OEN) &= ~BIT6;
}

void MHal_GPIO_SPI_DO_Oen(void)
{
    MHal_GPIO_REG(REG_SPI_DO_OEN) &= ~BIT7;
}

void MHal_GPIO_GPIO12_Oen(void)
{
    MHal_GPIO_REG(REG_GPIO12_OEN) &= ~BIT4;
}

void MHal_GPIO_GPIO13_Oen(void)
{
    MHal_GPIO_REG(REG_GPIO13_OEN) &= ~BIT5;
}

void MHal_GPIO_GPIO14_Oen(void)
{
    MHal_GPIO_REG(REG_GPIO14_OEN) &= ~BIT6;
}

void MHal_GPIO_GPIO15_Oen(void)
{
    MHal_GPIO_REG(REG_GPIO15_OEN) &= ~BIT7;
}

void MHal_GPIO_ET_CRS_Oen(void)
{
    MHal_GPIO_REG(REG_ET_CRS_OEN) &= ~BIT0;
}

void MHal_GPIO_ET_COL_Oen(void)
{
    MHal_GPIO_REG(REG_ET_COL_OEN) &= ~BIT1;
}

void MHal_GPIO_ET_TXD3_Oen(void)
{
    MHal_GPIO_REG(REG_ET_TXD3_OEN) &= ~BIT1;
}

void MHal_GPIO_ET_TXD2_Oen(void)
{
    MHal_GPIO_REG(REG_ET_TXD2_OEN) &= ~BIT2;
}

void MHal_GPIO_ET_TXD1_Oen(void)
{
    MHal_GPIO_REG(REG_ET_TXD1_OEN) &= ~BIT2;
}

void MHal_GPIO_ET_TXD0_Oen(void)
{
    MHal_GPIO_REG(REG_ET_TXD0_OEN) &= ~BIT3;
}

void MHal_GPIO_ET_TX_EN_Oen(void)
{
    MHal_GPIO_REG(REG_ET_TX_EN_OEN) &= ~BIT4;
}

void MHal_GPIO_ET_TX_CLK_Oen(void)
{
    MHal_GPIO_REG(REG_ET_TX_CLK_OEN) &= ~BIT5;
}

void MHal_GPIO_ET_TXER_Oen(void)
{
    MHal_GPIO_REG(REG_ET_TXER_OEN) &= ~BIT3;
}

void MHal_GPIO_ET_RXER_Oen(void)
{
    MHal_GPIO_REG(REG_ET_RXER_OEN) &= ~BIT4;
}

void MHal_GPIO_ET_RX_CLK_Oen(void)
{
    MHal_GPIO_REG(REG_ET_RX_CLK_OEN) &= ~BIT5;
}

void MHal_GPIO_ET_RX_DV_Oen(void)
{
    MHal_GPIO_REG(REG_ET_RX_DV_OEN) &= ~BIT6;
}

void MHal_GPIO_ET_RXD0_Oen(void)
{
    MHal_GPIO_REG(REG_ET_RXD0_OEN) &= ~BIT6;
}

void MHal_GPIO_ET_RXD1_Oen(void)
{
    MHal_GPIO_REG(REG_ET_RXD1_OEN) &= ~BIT7;
}

void MHal_GPIO_ET_RXD2_Oen(void)
{
    MHal_GPIO_REG(REG_ET_RXD2_OEN) &= ~BIT7;
}

void MHal_GPIO_ET_RXD3_Oen(void)
{
    MHal_GPIO_REG(REG_ET_RXD3_OEN) &= ~BIT0;
}

void MHal_GPIO_ET_MDC_Oen(void)
{
    MHal_GPIO_REG(REG_ET_MDC_OEN) &= ~BIT0;
}

void MHal_GPIO_ET_MDIO_Oen(void)
{
    MHal_GPIO_REG(REG_ET_MDIO_OEN) &= ~BIT1;
}

void MHal_GPIO_LCK_Oen(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    //MHal_GPIO_REG(REG_LCK_OEN) &= ~BIT3;
    MHal_GPIO_REG(REG_LCK_OEN) &= ~BIT2;
}

void MHal_GPIO_LDE_Oen(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    //MHal_GPIO_REG(REG_LDE_OEN) &= ~BIT2;
    MHal_GPIO_REG(REG_LDE_OEN) &= ~BIT3;
}

void MHal_GPIO_LHSYNC_Oen(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    //MHal_GPIO_REG(REG_LHSYNC_OEN) &= ~BIT1;
    MHal_GPIO_REG(REG_LHSYNC_OEN) &= ~BIT0;
}

void MHal_GPIO_LVSYNC_Oen(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    //MHal_GPIO_REG(REG_LVSYNC_OEN) &= ~BIT0;
    MHal_GPIO_REG(REG_LVSYNC_OEN) &= ~BIT1;
}

void MHal_GPIO_TCON0_Oen(void)
{
    MHal_GPIO_REG(REG_TCON0_OEN) &= ~BIT0;
}

void MHal_GPIO_TCON1_Oen(void)
{
    MHal_GPIO_REG(REG_TCON1_OEN) &= ~BIT1;
}

void MHal_GPIO_TCON2_Oen(void)
{
    MHal_GPIO_REG(REG_TCON2_OEN) &= ~BIT2;
}

void MHal_GPIO_TCON3_Oen(void)
{
    MHal_GPIO_REG(REG_TCON3_OEN) &= ~BIT3;
}

void MHal_GPIO_TCON4_Oen(void)
{
    MHal_GPIO_REG(REG_TCON4_OEN) &= ~BIT4;
}

void MHal_GPIO_TCON5_Oen(void)
{
    MHal_GPIO_REG(REG_TCON5_OEN) &= ~BIT5;
}

void MHal_GPIO_TCON6_Oen(void)
{
    MHal_GPIO_REG(REG_TCON6_OEN) &= ~BIT6;
}

void MHal_GPIO_TCON7_Oen(void)
{
    MHal_GPIO_REG(REG_TCON7_OEN) &= ~BIT7;
}

void MHal_GPIO_TCON8_Oen(void)
{
    MHal_GPIO_REG(REG_TCON8_OEN) &= ~BIT0;
}

void MHal_GPIO_TCON9_Oen(void)
{
    MHal_GPIO_REG(REG_TCON9_OEN) &= ~BIT1;
}

void MHal_GPIO_TCON10_Oen(void)
{
    MHal_GPIO_REG(REG_TCON10_OEN) &= ~BIT2;
}

void MHal_GPIO_TCON11_Oen(void)
{
    MHal_GPIO_REG(REG_TCON11_OEN) &= ~BIT3;
}

void MHal_GPIO_TCON12_Oen(void)
{
    MHal_GPIO_REG(REG_TCON12_OEN) &= ~BIT4;
}

void MHal_GPIO_TCON13_Oen(void)
{
    MHal_GPIO_REG(REG_TCON13_OEN) &= ~BIT5;
}

void MHal_GPIO_TCON14_Oen(void)
{
    MHal_GPIO_REG(REG_TCON14_OEN) &= ~BIT6;
}

void MHal_GPIO_TCON15_Oen(void)
{
    MHal_GPIO_REG(REG_TCON15_OEN) &= ~BIT7;
}

void MHal_GPIO_TCON16_Oen(void)
{
    MHal_GPIO_REG(REG_TCON16_OEN) &= ~BIT0;
}

void MHal_GPIO_HOTPLUGB_Oen(void)
{
    MHal_GPIO_REG(REG_HOTPLUGB_OEN) &= ~BIT7;
}

void MHal_GPIO_HOTPLUGA_Oen(void)
{
    MHal_GPIO_REG(REG_HOTPLUGA_OEN) &= ~BIT6;
}

void MHal_GPIO_HOTPLUGC_Oen(void)
{
    MHal_GPIO_REG(REG_HOTPLUGC_OEN) &= ~BIT5;
}

//the functions of this section set to disable output
void MHal_GPIO_UART_RX2_Odn(void)
{
    MHal_GPIO_REG(REG_UART_RX2_OEN) |= BIT0;
}

void MHal_GPIO_UART_TX2_Odn(void)
{
    MHal_GPIO_REG(REG_UART_TX2_OEN) |= BIT1;
}

void MHal_GPIO_TS0_D0_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_D0_OEN) |= BIT0;
}

void MHal_GPIO_TS0_D1_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_D1_OEN) |= BIT1;
}

void MHal_GPIO_TS0_D2_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_D2_OEN) |= BIT2;
}

void MHal_GPIO_TS0_D3_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_D3_OEN) |= BIT3;
}

void MHal_GPIO_TS0_D4_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_D4_OEN) |= BIT4;
}

void MHal_GPIO_TS0_D5_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_D5_OEN) |= BIT5;
}

void MHal_GPIO_TS0_D6_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_D6_OEN) |= BIT6;
}

void MHal_GPIO_TS0_D7_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_D7_OEN) |= BIT7;
}

void MHal_GPIO_TS0_VLD_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_VLD_OEN) |= BIT0;
}

void MHal_GPIO_TS0_SYNC_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_SYNC_OEN) |= BIT1;
}

void MHal_GPIO_TS0_CLK_Odn(void)
{
    MHal_GPIO_REG(REG_TS0_CLK_OEN) |= BIT2;
}

void MHal_GPIO_PF_AD15_Odn(void)
{
    MHal_GPIO_REG(REG_PF_AD15_OEN) |= BIT0;
}

void MHal_GPIO_PF_CE0Z_Odn(void)
{
    MHal_GPIO_REG(REG_PF_CE0Z_OEN) |= BIT1;
}

void MHal_GPIO_PF_CE1Z_Odn(void)
{
    MHal_GPIO_REG(REG_PF_CE1Z_OEN) |= BIT2;
}

void MHal_GPIO_PF_OEZ_Odn(void)
{
    MHal_GPIO_REG(REG_PF_OEZ_OEN) |= BIT3;
}

void MHal_GPIO_PF_WEZ_Odn(void)
{
    MHal_GPIO_REG(REG_PF_WEZ_OEN) |= BIT4;
}

void MHal_GPIO_PF_ALE_Odn(void)
{
    MHal_GPIO_REG(REG_PF_ALE_OEN) |= BIT5;
}

void MHal_GPIO_F_RBZ_Odn(void)
{
    MHal_GPIO_REG(REG_F_RBZ_OEN) |= BIT6;
}

void MHal_GPIO_PCM_D3_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_D3_OEN) |= BIT6;
}

void MHal_GPIO_PCM_D4_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_D4_OEN) |= BIT5;
}

void MHal_GPIO_PCM_D5_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_D5_OEN) |= BIT4;
}

void MHal_GPIO_PCM_D6_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_D6_OEN) |= BIT3;
}

void MHal_GPIO_PCM_D7_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_D7_OEN) |= BIT1;
}

void MHal_GPIO_PCM_CE_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_CE_N_OEN) |= BIT0;
}

void MHal_GPIO_PCM_A10_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A10_OEN) |= BIT7;
}

void MHal_GPIO_PCM_OE_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_OE_N_OEN) |= BIT6;
}

void MHal_GPIO_PCM_A11_Odn(void)
{
MHal_GPIO_REG(REG_PCM_A11_OEN) |= BIT5;
}

void MHal_GPIO_PCM_IORD_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_IORD_N_OEN) |= BIT4;
}

void MHal_GPIO_PCM_A9_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A9_OEN) |= BIT3;
}

void MHal_GPIO_PCM_IOWR_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_IOWR_N_OEN) |= BIT2;
}

void MHal_GPIO_PCM_A8_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A8_OEN) |= BIT1;
}

void MHal_GPIO_PCM_A13_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A13_OEN) |= BIT0;
}

void MHal_GPIO_PCM_A14_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A14_OEN) |= BIT7;
}

void MHal_GPIO_PCM_IRQA_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_IRQA_N_OEN) |= BIT6;
}
void MHal_GPIO_PCM_A12_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A12_OEN) |= BIT5;
}

void MHal_GPIO_PCM_A7_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A7_OEN) |= BIT4;
}

void MHal_GPIO_PCM_A6_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A6_OEN) |= BIT3;
}

void MHal_GPIO_PCM_A5_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A5_OEN) |= BIT2;
}

void MHal_GPIO_PCM_WAIT_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_WAIT_N_OEN) |= BIT1;
}

void MHal_GPIO_PCM_A4_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A4_OEN) |= BIT0;
}

void MHal_GPIO_PCM2_CE_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM2_CE_N_OEN) |= BIT0;
}

void MHal_GPIO_PCM2_IRQA_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM2_IRQA_N_OEN) |= BIT1;
}

void MHal_GPIO_PCM2_WAIT_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM2_WAIT_N_OEN) |= BIT2;
}

void MHal_GPIO_PWM0_Odn(void)
{
    MHal_GPIO_REG(REG_PWM0_OEN) |= BIT2;
}

void MHal_GPIO_PWM1_Odn(void)
{
    MHal_GPIO_REG(REG_PWM1_OEN) |= BIT3;
}

void MHal_GPIO_PWM2_Odn(void)
{
    MHal_GPIO_REG(REG_PWM2_OEN) |= BIT4;
}

void MHal_GPIO_PWM3_Odn(void)
{
    MHal_GPIO_REG(REG_PWM3_OEN) |= BIT5;
}

void MHal_GPIO_DDCR_DA_Odn(void)
{
    MHal_GPIO_REG(REG_DDCR_DA_OEN) |= BIT6;
}

void MHal_GPIO_DDCR_CK_Odn(void)
{
    MHal_GPIO_REG(REG_DDCR_CK_OEN) |= BIT7;
}

void MHal_GPIO_PCM_A3_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A3_OEN) |= BIT7;
}

void MHal_GPIO_PCM_A2_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A2_OEN) |= BIT0;
}

void MHal_GPIO_PCM_REG_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_REG_N_OEN) |= BIT1;
}

void MHal_GPIO_PCM_A1_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A1_OEN) |= BIT2;
}

void MHal_GPIO_PCM_A0_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_A0_OEN) |= BIT3;
}

void MHal_GPIO_PCM_D0_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_D0_OEN) |= BIT4;
}

void MHal_GPIO_PCM_D1_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_D1_OEN) |= BIT5;
}

void MHal_GPIO_PCM_D2_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_D2_OEN) |= BIT6;
}

void MHal_GPIO_PCM_RESET_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_RESET_OEN) |= BIT7;
}

void MHal_GPIO_PCM2_RESET_Odn(void)
{
    MHal_GPIO_REG(REG_PCM2_RESET_OEN) |= BIT3;
}

void MHal_GPIO_PCM_CD_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM_CD_N_OEN) |= BIT0;
}

void MHal_GPIO_PCM2_CD_N_Odn(void)
{
    MHal_GPIO_REG(REG_PCM2_CD_N_OEN) |= BIT4;
}

void MHal_GPIO_TS1_D0_Odn(void)
{
    MHal_GPIO_REG(REG_TS1_D0_OEN) |= BIT4;
}

void MHal_GPIO_TS1_VLD_Odn(void)
{
    MHal_GPIO_REG(REG_TS1_VLD_OEN) |= BIT5;
}

void MHal_GPIO_TS1_SYNC_Odn(void)
{
    MHal_GPIO_REG(REG_TS1_SYNC_OEN) |= BIT6;
}

void MHal_GPIO_TS1_CLK_Odn(void)
{
    MHal_GPIO_REG(REG_TS1_CLK_OEN) |= BIT7;
}

void MHal_GPIO_I2S_IN_WS_Odn(void)
{
    MHal_GPIO_REG(REG_I2S_IN_WS_OEN) |= BIT0;
}

void MHal_GPIO_I2S_IN_BCK_Odn(void)
{
    MHal_GPIO_REG(REG_I2S_IN_BCK_OEN) |= BIT1;
}

void MHal_GPIO_I2S_IN_SD_Odn(void)
{
    MHal_GPIO_REG(REG_I2S_IN_SD_OEN) |= BIT2;
}

void MHal_GPIO_SPDIF_IN_Odn(void)
{
    MHal_GPIO_REG(REG_SPDIF_IN_OEN) |= BIT3;
}

void MHal_GPIO_I2S_OUT_MCK_Odn(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_MCK_OEN) |= BIT5;
}

void MHal_GPIO_I2S_OUT_WS_Odn(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_WS_OEN) |= BIT6;
}

void MHal_GPIO_I2S_OUT_BCK_Odn(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_BCK_OEN) |= BIT7;
}

void MHal_GPIO_I2S_OUT_SD_Odn(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_SD_OEN) |= BIT0;
}

void MHal_GPIO_SPDIF_OUT_Odn(void)
{
    MHal_GPIO_REG(REG_SPDIF_OUT_OEN) |= BIT4;
}

void MHal_GPIO_DDCR_DA2_Odn(void)
{
    MHal_GPIO_REG(REG_DDCR_DA2_OEN) |= BIT0;
}

void MHal_GPIO_DDCR_CK2_Odn(void)
{
    MHal_GPIO_REG(REG_DDCR_CK2_OEN) |= BIT1;
}

void MHal_GPIO_LHSYNC2_Odn(void)
{
    MHal_GPIO_REG(REG_LHSYNC2_OEN) |= BIT2;
}

void MHal_GPIO_LVSYNC2_Odn(void)
{
    MHal_GPIO_REG(REG_LVSYNC2_OEN) |= BIT3;
}

void MHal_GPIO_SPI_CK_Odn(void)
{
    MHal_GPIO_REG(REG_SPI_CK_OEN) |= BIT4;
}

void MHal_GPIO_SPI_DI_Odn(void)
{
    MHal_GPIO_REG(REG_SPI_DI_OEN) |= BIT5;
}

void MHal_GPIO_SPI_CZ_Odn(void)
{
    MHal_GPIO_REG(REG_SPI_CZ_OEN) |= BIT6;
}

void MHal_GPIO_SPI_DO_Odn(void)
{
    MHal_GPIO_REG(REG_SPI_DO_OEN) |= BIT7;
}

void MHal_GPIO_GPIO12_Odn(void)
{
    MHal_GPIO_REG(REG_GPIO12_OEN) |= BIT4;
}

void MHal_GPIO_GPIO13_Odn(void)
{
    MHal_GPIO_REG(REG_GPIO13_OEN) |= BIT5;
}

void MHal_GPIO_GPIO14_Odn(void)
{
    MHal_GPIO_REG(REG_GPIO14_OEN) |= BIT6;
}

void MHal_GPIO_GPIO15_Odn(void)
{
    MHal_GPIO_REG(REG_GPIO15_OEN) |= BIT7;
}

void MHal_GPIO_ET_CRS_Odn(void)
{
    MHal_GPIO_REG(REG_ET_CRS_OEN) |= BIT0;
}

void MHal_GPIO_ET_COL_Odn(void)
{
    MHal_GPIO_REG(REG_ET_COL_OEN) |= BIT1;
}

void MHal_GPIO_ET_TXD3_Odn(void)
{
    MHal_GPIO_REG(REG_ET_TXD3_OEN) |= BIT1;
}

void MHal_GPIO_ET_TXD2_Odn(void)
{
    MHal_GPIO_REG(REG_ET_TXD2_OEN) |= BIT2;
}

void MHal_GPIO_ET_TXD1_Odn(void)
{
    MHal_GPIO_REG(REG_ET_TXD1_OEN) |= BIT2;
}

void MHal_GPIO_ET_TXD0_Odn(void)
{
    MHal_GPIO_REG(REG_ET_TXD0_OEN) |= BIT3;
}

void MHal_GPIO_ET_TX_EN_Odn(void)
{
    MHal_GPIO_REG(REG_ET_TX_EN_OEN) |= BIT4;
}

void MHal_GPIO_ET_TX_CLK_Odn(void)
{
    MHal_GPIO_REG(REG_ET_TX_CLK_OEN) |= BIT5;
}

void MHal_GPIO_ET_TXER_Odn(void)
{
    MHal_GPIO_REG(REG_ET_TXER_OEN) |= BIT3;
}

void MHal_GPIO_ET_RXER_Odn(void)
{
    MHal_GPIO_REG(REG_ET_RXER_OEN) |= BIT4;
}

void MHal_GPIO_ET_RX_CLK_Odn(void)
{
    MHal_GPIO_REG(REG_ET_RX_CLK_OEN) |= BIT5;
}

void MHal_GPIO_ET_RX_DV_Odn(void)
{
    MHal_GPIO_REG(REG_ET_RX_DV_OEN) |= BIT6;
}

void MHal_GPIO_ET_RXD0_Odn(void)
{
    MHal_GPIO_REG(REG_ET_RXD0_OEN) |= BIT6;
}

void MHal_GPIO_ET_RXD1_Odn(void)
{
    MHal_GPIO_REG(REG_ET_RXD1_OEN) |= BIT7;
}

void MHal_GPIO_ET_RXD2_Odn(void)
{
    MHal_GPIO_REG(REG_ET_RXD2_OEN) |= BIT7;
}

void MHal_GPIO_ET_RXD3_Odn(void)
{
    MHal_GPIO_REG(REG_ET_RXD3_OEN) |= BIT0;
}

void MHal_GPIO_ET_MDC_Odn(void)
{
    MHal_GPIO_REG(REG_ET_MDC_OEN) |= BIT0;
}

void MHal_GPIO_ET_MDIO_Odn(void)
{
    MHal_GPIO_REG(REG_ET_MDIO_OEN) |= BIT1;
}

void MHal_GPIO_LCK_Odn(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    //MHal_GPIO_REG(REG_LCK_OEN) |= BIT3;
    MHal_GPIO_REG(REG_LCK_OEN) |= BIT2;
}

void MHal_GPIO_LDE_Odn(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    //MHal_GPIO_REG(REG_LDE_OEN) |= BIT2;
    MHal_GPIO_REG(REG_LDE_OEN) |= BIT3;
}

void MHal_GPIO_LHSYNC_Odn(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    //MHal_GPIO_REG(REG_LHSYNC_OEN) |= BIT1;
    MHal_GPIO_REG(REG_LHSYNC_OEN) |= BIT0;
}

void MHal_GPIO_LVSYNC_Odn(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    //MHal_GPIO_REG(REG_LVSYNC_OEN) |= BIT0;
    MHal_GPIO_REG(REG_LVSYNC_OEN) |= BIT1;
}

void MHal_GPIO_TCON0_Odn(void)
{
    MHal_GPIO_REG(REG_TCON0_OEN) |= BIT0;
}

void MHal_GPIO_TCON1_Odn(void)
{
    MHal_GPIO_REG(REG_TCON1_OEN) |= BIT1;
}

void MHal_GPIO_TCON2_Odn(void)
{
    MHal_GPIO_REG(REG_TCON2_OEN) |= BIT2;
}

void MHal_GPIO_TCON3_Odn(void)
{
    MHal_GPIO_REG(REG_TCON3_OEN) |= BIT3;
}

void MHal_GPIO_TCON4_Odn(void)
{
    MHal_GPIO_REG(REG_TCON4_OEN) |= BIT4;
}

void MHal_GPIO_TCON5_Odn(void)
{
    MHal_GPIO_REG(REG_TCON5_OEN) |= BIT5;
}

void MHal_GPIO_TCON6_Odn(void)
{
    MHal_GPIO_REG(REG_TCON6_OEN) |= BIT6;
}

void MHal_GPIO_TCON7_Odn(void)
{
    MHal_GPIO_REG(REG_TCON7_OEN) |= BIT7;
}

void MHal_GPIO_TCON8_Odn(void)
{
    MHal_GPIO_REG(REG_TCON8_OEN) |= BIT0;
}

void MHal_GPIO_TCON9_Odn(void)
{
    MHal_GPIO_REG(REG_TCON9_OEN) |= BIT1;
}

void MHal_GPIO_TCON10_Odn(void)
{
    MHal_GPIO_REG(REG_TCON10_OEN) |= BIT2;
}

void MHal_GPIO_TCON11_Odn(void)
{
    MHal_GPIO_REG(REG_TCON11_OEN) |= BIT3;
}

void MHal_GPIO_TCON12_Odn(void)
{
    MHal_GPIO_REG(REG_TCON12_OEN) |= BIT4;
}

void MHal_GPIO_TCON13_Odn(void)
{
    MHal_GPIO_REG(REG_TCON13_OEN) |= BIT5;
}

void MHal_GPIO_TCON14_Odn(void)
{
    MHal_GPIO_REG(REG_TCON14_OEN) |= BIT6;
}

void MHal_GPIO_TCON15_Odn(void)
{
    MHal_GPIO_REG(REG_TCON15_OEN) |= BIT7;
}

void MHal_GPIO_TCON16_Odn(void)
{
    MHal_GPIO_REG(REG_TCON16_OEN) |= BIT0;
}

void MHal_GPIO_HOTPLUGB_Odn(void)
{
    MHal_GPIO_REG(REG_HOTPLUGB_OEN) |= BIT7;
}

void MHal_GPIO_HOTPLUGA_Odn(void)
{
    MHal_GPIO_REG(REG_HOTPLUGA_OEN) |= BIT6;
}

void MHal_GPIO_HOTPLUGC_Odn(void)
{
    MHal_GPIO_REG(REG_HOTPLUGC_OEN) |= BIT5;
}

//the functions of this section set to get input
U8 MHal_GPIO_UART_RX2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_UART_RX2_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_UART_TX2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_UART_TX2_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_D0_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_D0_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_D1_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_D1_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_D2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_D2_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_D3_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_D3_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_D4_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_D4_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_D5_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_D5_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_D6_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_D6_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_D7_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_D7_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_VLD_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_VLD_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_SYNC_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_SYNC_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS0_CLK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS0_CLK_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PF_AD15_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PF_AD15_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PF_CE0Z_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PF_CE0Z_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PF_CE1Z_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PF_CE1Z_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PF_OEZ_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PF_OEZ_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PF_WEZ_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PF_WEZ_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PF_ALE_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PF_ALE_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_F_RBZ_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_F_RBZ_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_D3_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_D3_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_D4_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_D4_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_D5_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_D5_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_D6_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_D6_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_D7_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_D7_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_CE_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_CE_N_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A10_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A10_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_OE_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_OE_N_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A11_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A11_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_IORD_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_IORD_N_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A9_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A9_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_IOWR_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_IOWR_N_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A8_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A8_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A13_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A13_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A14_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A14_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_IRQA_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_IRQA_N_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A12_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A12_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A7_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A7_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A6_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A6_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A5_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A5_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_WAIT_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_WAIT_N_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A4_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A4_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM2_CE_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM2_CE_N_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM2_IRQA_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM2_IRQA_N_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM2_WAIT_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM2_WAIT_N_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PWM0_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PWM0_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PWM1_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PWM1_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PWM2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PWM2_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PWM3_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PWM3_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_DDCR_DA_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_DDCR_DA_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_DDCR_CK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_DDCR_CK_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A3_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A3_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A2_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_REG_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_REG_N_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A1_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A1_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_A0_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_A0_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_D0_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_D0_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_D1_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_D1_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_D2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_D2_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_RESET_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_RESET_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM2_RESET_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM2_RESET_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM_CD_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM_CD_N_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_PCM2_CD_N_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_PCM2_CD_N_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS1_D0_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS1_D0_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS1_VLD_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS1_VLD_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS1_SYNC_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS1_SYNC_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TS1_CLK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TS1_CLK_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_I2S_IN_WS_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_I2S_IN_WS_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_I2S_IN_BCK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_I2S_IN_BCK_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_I2S_IN_SD_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_I2S_IN_SD_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_SPDIF_IN_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_SPDIF_IN_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_I2S_OUT_MCK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_I2S_OUT_MCK_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_I2S_OUT_WS_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_I2S_OUT_WS_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_I2S_OUT_BCK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_I2S_OUT_BCK_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_I2S_OUT_SD_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_I2S_OUT_SD_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_SPDIF_OUT_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_SPDIF_OUT_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_DDCR_DA2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_DDCR_DA2_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_DDCR_CK2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_DDCR_CK2_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_LHSYNC2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_LHSYNC2_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_LVSYNC2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_LVSYNC2_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_SPI_CK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_SPI_CK_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_SPI_DI_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_SPI_DI_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_SPI_CZ_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_SPI_CZ_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_SPI_DO_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_SPI_DO_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_GPIO12_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_GPIO12_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_GPIO13_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_GPIO13_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_GPIO14_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_GPIO14_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_GPIO15_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_GPIO15_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_CRS_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_CRS_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_COL_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_COL_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_TXD3_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_TXD3_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_TXD2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_TXD2_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_TXD1_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_TXD1_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_TXD0_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_TXD0_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_TX_EN_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_TX_EN_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_TX_CLK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_TX_CLK_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_TXER_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_TXER_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_RXER_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_RXER_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_RX_CLK_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_RX_CLK_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_RX_DV_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_RX_DV_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_RXD0_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_RXD0_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_RXD1_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_RXD1_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_RXD2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_RXD2_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_RXD3_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_RXD3_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_MDC_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_MDC_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_ET_MDIO_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_ET_MDIO_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_LCK_In(void)
{
    U8 u8RetGPIO;
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    u8RetGPIO = MHal_GPIO_REG(REG_LCK_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_LDE_In(void)
{
    U8 u8RetGPIO;
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    u8RetGPIO = MHal_GPIO_REG(REG_LDE_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_LHSYNC_In(void)
{
    U8 u8RetGPIO;
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    u8RetGPIO = MHal_GPIO_REG(REG_LHSYNC_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_LVSYNC_In(void)
{
    U8 u8RetGPIO;
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    u8RetGPIO = MHal_GPIO_REG(REG_LVSYNC_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON0_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON0_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON1_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON1_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON2_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON2_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON3_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON3_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON4_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON4_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON5_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON5_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON6_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON6_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON7_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON7_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON8_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON8_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON9_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON9_IN);
    u8RetGPIO = (u8RetGPIO>>1);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON10_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON10_IN);
    u8RetGPIO = (u8RetGPIO>>2);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON11_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON11_IN);
    u8RetGPIO = (u8RetGPIO>>3);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON12_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON12_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON13_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON13_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON14_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON14_IN);
    u8RetGPIO = (u8RetGPIO>>6);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON15_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON15_IN);
    u8RetGPIO = (u8RetGPIO>>7);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_TCON16_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_TCON16_IN);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_HOTPLUGB_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_HOTPLUGB_IN);
    u8RetGPIO = (u8RetGPIO>>5);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_HOTPLUGA_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_HOTPLUGA_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

U8 MHal_GPIO_HOTPLUGC_In(void)
{
    U8 u8RetGPIO;
    u8RetGPIO = MHal_GPIO_REG(REG_HOTPLUGC_IN);
    u8RetGPIO = (u8RetGPIO>>4);
    return (u8RetGPIO &= BIT0);
}

//the functions of this section set to output high
void MHal_GPIO_UART_RX2_High(void)
{
    MHal_GPIO_REG(REG_UART_RX2_OUT) |= BIT0;
}

void MHal_GPIO_UART_TX2_High(void)
{
    MHal_GPIO_REG(REG_UART_TX2_OUT) |= BIT1;
}

void MHal_GPIO_TS0_D0_High(void)
{
    MHal_GPIO_REG(REG_TS0_D0_OUT) |= BIT0;
}

void MHal_GPIO_TS0_D1_High(void)
{
    MHal_GPIO_REG(REG_TS0_D1_OUT) |= BIT1;
}

void MHal_GPIO_TS0_D2_High(void)
{
    MHal_GPIO_REG(REG_TS0_D2_OUT) |= BIT2;
}

void MHal_GPIO_TS0_D3_High(void)
{
    MHal_GPIO_REG(REG_TS0_D3_OUT) |= BIT3;
}

void MHal_GPIO_TS0_D4_High(void)
{
    MHal_GPIO_REG(REG_TS0_D4_OUT) |= BIT4;
}

void MHal_GPIO_TS0_D5_High(void)
{
    MHal_GPIO_REG(REG_TS0_D5_OUT) |= BIT5;
}

void MHal_GPIO_TS0_D6_High(void)
{
    MHal_GPIO_REG(REG_TS0_D6_OUT) |= BIT6;
}

void MHal_GPIO_TS0_D7_High(void)
{
    MHal_GPIO_REG(REG_TS0_D7_OUT) |= BIT7;
}

void MHal_GPIO_TS0_VLD_High(void)
{
    MHal_GPIO_REG(REG_TS0_VLD_OUT) |= BIT0;
}

void MHal_GPIO_TS0_SYNC_High(void)
{
    MHal_GPIO_REG(REG_TS0_SYNC_OUT) |= BIT1;
}

void MHal_GPIO_TS0_CLK_High(void)
{
    MHal_GPIO_REG(REG_TS0_CLK_OUT) |= BIT2;
}

void MHal_GPIO_PF_AD15_High(void)
{
    MHal_GPIO_REG(REG_PF_AD15_OUT) |= BIT0;
}

void MHal_GPIO_PF_CE0Z_High(void)
{
    MHal_GPIO_REG(REG_PF_CE0Z_OUT) |= BIT1;
}

void MHal_GPIO_PF_CE1Z_High(void)
{
    MHal_GPIO_REG(REG_PF_CE1Z_OUT) |= BIT2;
}

void MHal_GPIO_PF_OEZ_High(void)
{
    MHal_GPIO_REG(REG_PF_OEZ_OUT) |= BIT3;
}

void MHal_GPIO_PF_WEZ_High(void)
{
    MHal_GPIO_REG(REG_PF_WEZ_OUT) |= BIT4;
}

void MHal_GPIO_PF_ALE_High(void)
{
    MHal_GPIO_REG(REG_PF_ALE_OUT) |= BIT5;
}

void MHal_GPIO_F_RBZ_High(void)
{
    MHal_GPIO_REG(REG_F_RBZ_OUT) |= BIT6;
}

void MHal_GPIO_PCM_D3_High(void)
{
    MHal_GPIO_REG(REG_PCM_D3_OUT) |= BIT6;
}

void MHal_GPIO_PCM_D4_High(void)
{
    MHal_GPIO_REG(REG_PCM_D4_OUT) |= BIT5;
}

void MHal_GPIO_PCM_D5_High(void)
{
    MHal_GPIO_REG(REG_PCM_D5_OUT) |= BIT4;
}

void MHal_GPIO_PCM_D6_High(void)
{
    MHal_GPIO_REG(REG_PCM_D6_OUT) |= BIT3;
}

void MHal_GPIO_PCM_D7_High(void)
{
    MHal_GPIO_REG(REG_PCM_D7_OUT) |= BIT1;
}

void MHal_GPIO_PCM_CE_N_High(void)
{
    MHal_GPIO_REG(REG_PCM_CE_N_OUT) |= BIT0;
}

void MHal_GPIO_PCM_A10_High(void)
{
    MHal_GPIO_REG(REG_PCM_A10_OUT) |= BIT7;
}

void MHal_GPIO_PCM_OE_N_High(void)
{
    MHal_GPIO_REG(REG_PCM_OE_N_OUT) |= BIT6;
}

void MHal_GPIO_PCM_A11_High(void)
{
    MHal_GPIO_REG(REG_PCM_A11_OUT) |= BIT5;
}

void MHal_GPIO_PCM_IORD_N_High(void)
{
    MHal_GPIO_REG(REG_PCM_IORD_N_OUT) |= BIT4;
}

void MHal_GPIO_PCM_A9_High(void)
{
    MHal_GPIO_REG(REG_PCM_A9_OUT) |= BIT3;
}

void MHal_GPIO_PCM_IOWR_N_High(void)
{
    MHal_GPIO_REG(REG_PCM_IOWR_N_OUT) |= BIT2;
}

void MHal_GPIO_PCM_A8_High(void)
{
    MHal_GPIO_REG(REG_PCM_A8_OUT) |= BIT1;
}

void MHal_GPIO_PCM_A13_High(void)
{
    MHal_GPIO_REG(REG_PCM_A13_OUT) |= BIT0;
}

void MHal_GPIO_PCM_A14_High(void)
{
    MHal_GPIO_REG(REG_PCM_A14_OUT) |= BIT7;
}

void MHal_GPIO_PCM_IRQA_N_High(void)
{
    MHal_GPIO_REG(REG_PCM_IRQA_N_OUT) |= BIT6;
}

void MHal_GPIO_PCM_A12_High(void)
{
    MHal_GPIO_REG(REG_PCM_A12_OUT) |= BIT5;
}

void MHal_GPIO_PCM_A7_High(void)
{
    MHal_GPIO_REG(REG_PCM_A7_OUT) |= BIT4;
}

void MHal_GPIO_PCM_A6_High(void)
{
    MHal_GPIO_REG(REG_PCM_A6_OUT) |= BIT3;
}

void MHal_GPIO_PCM_A5_High(void)
{
    MHal_GPIO_REG(REG_PCM_A5_OUT) |= BIT2;
}

void MHal_GPIO_PCM_WAIT_N_High(void)
{
    MHal_GPIO_REG(REG_PCM_WAIT_N_OUT) |= BIT1;
}

void MHal_GPIO_PCM_A4_High(void)
{
    MHal_GPIO_REG(REG_PCM_A4_OUT) |= BIT0;
}

void MHal_GPIO_PCM2_CE_N_High(void)
{
    MHal_GPIO_REG(REG_PCM2_CE_N_OUT) |= BIT0;
}

void MHal_GPIO_PCM2_IRQA_N_High(void)
{
    MHal_GPIO_REG(REG_PCM2_IRQA_N_OUT) |= BIT1;
}

void MHal_GPIO_PCM2_WAIT_N_High(void)
{
    MHal_GPIO_REG(REG_PCM2_WAIT_N_OUT) |= BIT2;
}

void MHal_GPIO_PWM0_High(void)
{
    MHal_GPIO_REG(REG_PWM0_OUT) |= BIT2;
}

void MHal_GPIO_PWM1_High(void)
{
    MHal_GPIO_REG(REG_PWM1_OUT) |= BIT3;
}

void MHal_GPIO_PWM2_High(void)
{
    MHal_GPIO_REG(REG_PWM2_OUT) |= BIT4;
}

void MHal_GPIO_PWM3_High(void)
{
    MHal_GPIO_REG(REG_PWM3_OUT) |= BIT5;
}

void MHal_GPIO_DDCR_DA_High(void)
{
    MHal_GPIO_REG(REG_DDCR_DA_OUT) |= BIT6;
}

void MHal_GPIO_DDCR_CK_High(void)
{
    MHal_GPIO_REG(REG_DDCR_CK_OUT) |= BIT7;
}

void MHal_GPIO_PCM_A3_High(void)
{
    MHal_GPIO_REG(REG_PCM_A3_OUT) |= BIT7;
}

void MHal_GPIO_PCM_A2_High(void)
{
    MHal_GPIO_REG(REG_PCM_A2_OUT) |= BIT0;
}

void MHal_GPIO_PCM_REG_N_High(void)
{
    MHal_GPIO_REG(REG_PCM_REG_N_OUT) |= BIT1;
}

void MHal_GPIO_PCM_A1_High(void)
{
    MHal_GPIO_REG(REG_PCM_A1_OUT) |= BIT2;
}

void MHal_GPIO_PCM_A0_High(void)
{
    MHal_GPIO_REG(REG_PCM_A0_OUT) |= BIT3;
}

void MHal_GPIO_PCM_D0_High(void)
{
    MHal_GPIO_REG(REG_PCM_D0_OUT) |= BIT4;
}

void MHal_GPIO_PCM_D1_High(void)
{
    MHal_GPIO_REG(REG_PCM_D1_OUT) |= BIT5;
}

void MHal_GPIO_PCM_D2_High(void)
{
    MHal_GPIO_REG(REG_PCM_D2_OUT) |= BIT6;
}

void MHal_GPIO_PCM_RESET_High(void)
{
    MHal_GPIO_REG(REG_PCM_RESET_OUT) |= BIT7;
}

void MHal_GPIO_PCM2_RESET_High(void)
{
    MHal_GPIO_REG(REG_PCM2_RESET_OUT) |= BIT3;
}

void MHal_GPIO_PCM_CD_N_High(void)
{
    MHal_GPIO_REG(REG_PCM_CD_N_OUT) |= BIT0;
}

void MHal_GPIO_PCM2_CD_N_High(void)
{
    MHal_GPIO_REG(REG_PCM2_CD_N_OUT) |= BIT4;
}

void MHal_GPIO_TS1_D0_High(void)
{
    MHal_GPIO_REG(REG_TS1_D0_OUT) |= BIT4;
}

void MHal_GPIO_TS1_VLD_High(void)
{
    MHal_GPIO_REG(REG_TS1_VLD_OUT) |= BIT5;
}

void MHal_GPIO_TS1_SYNC_High(void)
{
    MHal_GPIO_REG(REG_TS1_SYNC_OUT) |= BIT6;
}

void MHal_GPIO_TS1_CLK_High(void)
{
    MHal_GPIO_REG(REG_TS1_CLK_OUT) |= BIT7;
}

void MHal_GPIO_I2S_IN_WS_High(void)
{
    MHal_GPIO_REG(REG_I2S_IN_WS_OUT) |= BIT0;
}

void MHal_GPIO_I2S_IN_BCK_High(void)
{
    MHal_GPIO_REG(REG_I2S_IN_BCK_OUT) |= BIT1;
}

void MHal_GPIO_I2S_IN_SD_High(void)
{
    MHal_GPIO_REG(REG_I2S_IN_SD_OUT) |= BIT2;
}

void MHal_GPIO_SPDIF_IN_High(void)
{
    MHal_GPIO_REG(REG_SPDIF_IN_OUT) |= BIT3;
}

void MHal_GPIO_I2S_OUT_MCK_High(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_MCK_OUT) |= BIT5;
}

void MHal_GPIO_I2S_OUT_WS_High(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_WS_OUT) |= BIT6;
}

void MHal_GPIO_I2S_OUT_BCK_High(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_BCK_OUT) |= BIT7;
}

void MHal_GPIO_I2S_OUT_SD_High(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_SD_OUT) |= BIT0;
}

void MHal_GPIO_SPDIF_OUT_High(void)
{
    MHal_GPIO_REG(REG_SPDIF_OUT_OUT) |= BIT4;
}

void MHal_GPIO_DDCR_DA2_High(void)
{
    MHal_GPIO_REG(REG_DDCR_DA2_OUT) |= BIT0;
}

void MHal_GPIO_DDCR_CK2_High(void)
{
    MHal_GPIO_REG(REG_DDCR_CK2_OUT) |= BIT1;
}

void MHal_GPIO_LHSYNC2_High(void)
{
    MHal_GPIO_REG(REG_LHSYNC2_OUT) |= BIT2;
}

void MHal_GPIO_LVSYNC2_High(void)
{
    MHal_GPIO_REG(REG_LVSYNC2_OUT) |= BIT3;
}

void MHal_GPIO_SPI_CK_High(void)
{
    MHal_GPIO_REG(REG_SPI_CK_OUT) |= BIT4;
}

void MHal_GPIO_SPI_DI_High(void)
{
    MHal_GPIO_REG(REG_SPI_DI_OUT) |= BIT5;
}

void MHal_GPIO_SPI_CZ_High(void)
{
    MHal_GPIO_REG(REG_SPI_CZ_OUT) |= BIT6;
}

void MHal_GPIO_SPI_DO_High(void)
{
    MHal_GPIO_REG(REG_SPI_DO_OUT) |= BIT7;
}

void MHal_GPIO_GPIO12_High(void)
{
    MHal_GPIO_REG(REG_GPIO12_OUT) |= BIT4;
}

void MHal_GPIO_GPIO13_High(void)
{
    MHal_GPIO_REG(REG_GPIO13_OUT) |= BIT5;
}

void MHal_GPIO_GPIO14_High(void)
{
    MHal_GPIO_REG(REG_GPIO14_OUT) |= BIT6;
}

void MHal_GPIO_GPIO15_High(void)
{
    MHal_GPIO_REG(REG_GPIO15_OUT) |= BIT7;
}

void MHal_GPIO_ET_CRS_High(void)
{
    MHal_GPIO_REG(REG_ET_CRS_OUT) |= BIT0;
}

void MHal_GPIO_ET_COL_High(void)
{
    MHal_GPIO_REG(REG_ET_COL_OUT) |= BIT1;
}

void MHal_GPIO_ET_TXD3_High(void)
{
    MHal_GPIO_REG(REG_ET_TXD3_OUT) |= BIT1;
}

void MHal_GPIO_ET_TXD2_High(void)
{
    MHal_GPIO_REG(REG_ET_TXD2_OUT) |= BIT2;
}

void MHal_GPIO_ET_TXD1_High(void)
{
    MHal_GPIO_REG(REG_ET_TXD1_OUT) |= BIT2;
}

void MHal_GPIO_ET_TXD0_High(void)
{
    MHal_GPIO_REG(REG_ET_TXD0_OUT) |= BIT3;
}

void MHal_GPIO_ET_TX_EN_High(void)
{
    MHal_GPIO_REG(REG_ET_TX_EN_OUT) |= BIT4;
}

void MHal_GPIO_ET_TX_CLK_High(void)
{
    MHal_GPIO_REG(REG_ET_TX_CLK_OUT) |= BIT5;
}

void MHal_GPIO_ET_TXER_High(void)
{
    MHal_GPIO_REG(REG_ET_TXER_OUT) |= BIT3;
}

void MHal_GPIO_ET_RXER_High(void)
{
    MHal_GPIO_REG(REG_ET_RXER_OUT) |= BIT4;
}

void MHal_GPIO_ET_RX_CLK_High(void)
{
    MHal_GPIO_REG(REG_ET_RX_CLK_OUT) |= BIT5;
}

void MHal_GPIO_ET_RX_DV_High(void)
{
    MHal_GPIO_REG(REG_ET_RX_DV_OUT) |= BIT6;
}

void MHal_GPIO_ET_RXD0_High(void)
{
    MHal_GPIO_REG(REG_ET_RXD0_OUT) |= BIT6;
}

void MHal_GPIO_ET_RXD1_High(void)
{
    MHal_GPIO_REG(REG_ET_RXD1_OUT) |= BIT7;
}

void MHal_GPIO_ET_RXD2_High(void)
{
    MHal_GPIO_REG(REG_ET_RXD2_OUT) |= BIT7;
}

void MHal_GPIO_ET_RXD3_High(void)
{
    MHal_GPIO_REG(REG_ET_RXD3_OUT) |= BIT0;
}

void MHal_GPIO_ET_MDC_High(void)
{
    MHal_GPIO_REG(REG_ET_MDC_OUT) |= BIT0;
}

void MHal_GPIO_ET_MDIO_High(void)
{
    MHal_GPIO_REG(REG_ET_MDIO_OUT) |= BIT1;
}

void MHal_GPIO_LCK_High(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LCK_OUT) |= BIT3;
}

void MHal_GPIO_LDE_High(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LDE_OUT) |= BIT2;
}

void MHal_GPIO_LHSYNC_High(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LHSYNC_OUT) |= BIT1;
}

void MHal_GPIO_LVSYNC_High(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LVSYNC_OUT) |= BIT0;
}

void MHal_GPIO_HOTPLUGB_High(void)
{
    MHal_GPIO_REG(REG_HOTPLUGB_OUT) |= BIT5;
}

void MHal_GPIO_HOTPLUGA_High(void)
{
    MHal_GPIO_REG(REG_HOTPLUGA_OUT) |= BIT4;
}

void MHal_GPIO_HOTPLUGC_High(void)
{
    MHal_GPIO_REG(REG_HOTPLUGC_OUT) |= BIT4;
}

void MHal_GPIO_TCON0_High(void)
{
    MHal_GPIO_REG(REG_TCON0_OUT) |= BIT0;
}

void MHal_GPIO_TCON1_High(void)
{
    MHal_GPIO_REG(REG_TCON1_OUT) |= BIT1;
}

void MHal_GPIO_TCON2_High(void)
{
    MHal_GPIO_REG(REG_TCON2_OUT) |= BIT2;
}

void MHal_GPIO_TCON3_High(void)
{
    MHal_GPIO_REG(REG_TCON3_OUT) |= BIT3;
}

void MHal_GPIO_TCON4_High(void)
{
    MHal_GPIO_REG(REG_TCON4_OUT) |= BIT4;
}

void MHal_GPIO_TCON5_High(void)
{
    MHal_GPIO_REG(REG_TCON5_OUT) |= BIT5;
}

void MHal_GPIO_TCON6_High(void)
{
    MHal_GPIO_REG(REG_TCON6_OUT) |= BIT6;
}

void MHal_GPIO_TCON7_High(void)
{
    MHal_GPIO_REG(REG_TCON7_OUT) |= BIT7;
}

void MHal_GPIO_TCON8_High(void)
{
    MHal_GPIO_REG(REG_TCON8_OUT) |= BIT0;
}

void MHal_GPIO_TCON9_High(void)
{
    MHal_GPIO_REG(REG_TCON9_OUT) |= BIT1;
}

void MHal_GPIO_TCON10_High(void)
{
    MHal_GPIO_REG(REG_TCON10_OUT) |= BIT2;
}

void MHal_GPIO_TCON11_High(void)
{
    MHal_GPIO_REG(REG_TCON11_OUT) |= BIT3;
}

void MHal_GPIO_TCON12_High(void)
{
    MHal_GPIO_REG(REG_TCON12_OUT) |= BIT4;
}
void MHal_GPIO_TCON13_High(void)
{
    MHal_GPIO_REG(REG_TCON13_OUT) |= BIT5;
}

void MHal_GPIO_TCON14_High(void)
{
    MHal_GPIO_REG(REG_TCON14_OUT) |= BIT6;
}

void MHal_GPIO_TCON15_High(void)
{
    MHal_GPIO_REG(REG_TCON15_OUT) |= BIT7;
}

void MHal_GPIO_TCON16_High(void)
{
    MHal_GPIO_REG(REG_TCON16_OUT) |= BIT0;
}

//the functions of this section set to output low
void MHal_GPIO_UART_RX2_Low(void)
{
    MHal_GPIO_REG(REG_UART_RX2_OUT) &= ~BIT0;
}

void MHal_GPIO_UART_TX2_Low(void)
{
    MHal_GPIO_REG(REG_UART_TX2_OUT) &= ~BIT1;
}

void MHal_GPIO_TS0_D0_Low(void)
{
    MHal_GPIO_REG(REG_TS0_D0_OUT) &= ~BIT0;
}

void MHal_GPIO_TS0_D1_Low(void)
{
    MHal_GPIO_REG(REG_TS0_D1_OUT) &= ~BIT1;
}

void MHal_GPIO_TS0_D2_Low(void)
{
    MHal_GPIO_REG(REG_TS0_D2_OUT) &= ~BIT2;
}

void MHal_GPIO_TS0_D3_Low(void)
{
    MHal_GPIO_REG(REG_TS0_D3_OUT) &= ~BIT3;
}

void MHal_GPIO_TS0_D4_Low(void)
{
    MHal_GPIO_REG(REG_TS0_D4_OUT) &= ~BIT4;
}

void MHal_GPIO_TS0_D5_Low(void)
{
    MHal_GPIO_REG(REG_TS0_D5_OUT) &= ~BIT5;
}

void MHal_GPIO_TS0_D6_Low(void)
{
    MHal_GPIO_REG(REG_TS0_D6_OUT) &= ~BIT6;
}

void MHal_GPIO_TS0_D7_Low(void)
{
    MHal_GPIO_REG(REG_TS0_D7_OUT) &= ~BIT7;
}

void MHal_GPIO_TS0_VLD_Low(void)
{
    MHal_GPIO_REG(REG_TS0_VLD_OUT) &= ~BIT0;
}

void MHal_GPIO_TS0_SYNC_Low(void)
{
    MHal_GPIO_REG(REG_TS0_SYNC_OUT) &= ~BIT1;
}

void MHal_GPIO_TS0_CLK_Low(void)
{
    MHal_GPIO_REG(REG_TS0_CLK_OUT) &= ~BIT2;
}

void MHal_GPIO_PF_AD15_Low(void)
{
    MHal_GPIO_REG(REG_PF_AD15_OUT) &= ~BIT0;
}

void MHal_GPIO_PF_CE0Z_Low(void)
{
    MHal_GPIO_REG(REG_PF_CE0Z_OUT) &= ~BIT1;
}

void MHal_GPIO_PF_CE1Z_Low(void)
{
    MHal_GPIO_REG(REG_PF_CE1Z_OUT) &= ~BIT2;
}

void MHal_GPIO_PF_OEZ_Low(void)
{
    MHal_GPIO_REG(REG_PF_OEZ_OUT) &= ~BIT3;
}

void MHal_GPIO_PF_WEZ_Low(void)
{
    MHal_GPIO_REG(REG_PF_WEZ_OUT) &= ~BIT4;
}

void MHal_GPIO_PF_ALE_Low(void)
{
    MHal_GPIO_REG(REG_PF_ALE_OUT) &= ~BIT5;
}

void MHal_GPIO_F_RBZ_Low(void)
{
    MHal_GPIO_REG(REG_F_RBZ_OUT) &= ~BIT6;
}

void MHal_GPIO_PCM_D3_Low(void)
{
    MHal_GPIO_REG(REG_PCM_D3_OUT) &= ~BIT6;
}

void MHal_GPIO_PCM_D4_Low(void)
{
    MHal_GPIO_REG(REG_PCM_D4_OUT) &= ~BIT5;
}

void MHal_GPIO_PCM_D5_Low(void)
{
    MHal_GPIO_REG(REG_PCM_D5_OUT) &= ~BIT4;
}

void MHal_GPIO_PCM_D6_Low(void)
{
    MHal_GPIO_REG(REG_PCM_D6_OUT) &= ~BIT3;
}

void MHal_GPIO_PCM_D7_Low(void)
{
    MHal_GPIO_REG(REG_PCM_D7_OUT) &= ~BIT1;
}

void MHal_GPIO_PCM_CE_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM_CE_N_OUT) &= ~BIT0;
}

void MHal_GPIO_PCM_A10_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A10_OUT) &= ~BIT7;
}

void MHal_GPIO_PCM_OE_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM_OE_N_OUT) &= ~BIT6;
}

void MHal_GPIO_PCM_A11_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A11_OUT) &= ~BIT5;
}

void MHal_GPIO_PCM_IORD_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM_IORD_N_OUT) &= ~BIT4;
}

void MHal_GPIO_PCM_A9_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A9_OUT) &= ~BIT3;
}

void MHal_GPIO_PCM_IOWR_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM_IOWR_N_OUT) &= ~BIT2;
}

void MHal_GPIO_PCM_A8_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A8_OUT) &= ~BIT1;
}

void MHal_GPIO_PCM_A13_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A13_OUT) &= ~BIT0;
}

void MHal_GPIO_PCM_A14_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A14_OUT) &= ~BIT7;
}

void MHal_GPIO_PCM_IRQA_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM_IRQA_N_OUT) &= ~BIT6;
}

void MHal_GPIO_PCM_A12_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A12_OUT) &= ~BIT5;
}

void MHal_GPIO_PCM_A7_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A7_OUT) &= ~BIT4;
}

void MHal_GPIO_PCM_A6_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A6_OUT) &= ~BIT3;
}

void MHal_GPIO_PCM_A5_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A5_OUT) &= ~BIT2;
}

void MHal_GPIO_PCM_WAIT_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM_WAIT_N_OUT) &= ~BIT1;
}

void MHal_GPIO_PCM_A4_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A4_OUT) &= ~BIT0;
}

void MHal_GPIO_PCM2_CE_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM2_CE_N_OUT) &= ~BIT0;
}

void MHal_GPIO_PCM2_IRQA_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM2_IRQA_N_OUT) &= ~BIT1;
}

void MHal_GPIO_PCM2_WAIT_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM2_WAIT_N_OUT) &= ~BIT2;
}

void MHal_GPIO_PWM0_Low(void)
{
    MHal_GPIO_REG(REG_PWM0_OUT) &= ~BIT2;
}

void MHal_GPIO_PWM1_Low(void)
{
    MHal_GPIO_REG(REG_PWM1_OUT) &= ~BIT3;
}

void MHal_GPIO_PWM2_Low(void)
{
    MHal_GPIO_REG(REG_PWM2_OUT) &= ~BIT4;
}

void MHal_GPIO_PWM3_Low(void)
{
    MHal_GPIO_REG(REG_PWM3_OUT) &= ~BIT5;
}

void MHal_GPIO_DDCR_DA_Low(void)
{
    MHal_GPIO_REG(REG_DDCR_DA_OUT) &= ~BIT6;
}

void MHal_GPIO_DDCR_CK_Low(void)
{
    MHal_GPIO_REG(REG_DDCR_CK_OUT) &= ~BIT7;
}

void MHal_GPIO_PCM_A3_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A3_OUT) &= ~BIT7;
}

void MHal_GPIO_PCM_A2_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A2_OUT) &= ~BIT0;
}

void MHal_GPIO_PCM_REG_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM_REG_N_OUT) &= ~BIT1;
}

void MHal_GPIO_PCM_A1_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A1_OUT) &= ~BIT2;
}

void MHal_GPIO_PCM_A0_Low(void)
{
    MHal_GPIO_REG(REG_PCM_A0_OUT) &= ~BIT3;
}

void MHal_GPIO_PCM_D0_Low(void)
{
    MHal_GPIO_REG(REG_PCM_D0_OUT) &= ~BIT4;
}

void MHal_GPIO_PCM_D1_Low(void)
{
    MHal_GPIO_REG(REG_PCM_D1_OUT) &= ~BIT5;
}

void MHal_GPIO_PCM_D2_Low(void)
{
    MHal_GPIO_REG(REG_PCM_D2_OUT) &= ~BIT6;
}

void MHal_GPIO_PCM_RESET_Low(void)
{
    MHal_GPIO_REG(REG_PCM_RESET_OUT) &= ~BIT7;
}

void MHal_GPIO_PCM2_RESET_Low(void)
{
    MHal_GPIO_REG(REG_PCM2_RESET_OUT) &= ~BIT3;
}

void MHal_GPIO_PCM_CD_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM_CD_N_OUT) &= ~BIT0;
}

void MHal_GPIO_PCM2_CD_N_Low(void)
{
    MHal_GPIO_REG(REG_PCM2_CD_N_OUT) &= ~BIT4;
}

void MHal_GPIO_TS1_D0_Low(void)
{
    MHal_GPIO_REG(REG_TS1_D0_OUT) &= ~BIT4;
}

void MHal_GPIO_TS1_VLD_Low(void)
{
    MHal_GPIO_REG(REG_TS1_VLD_OUT) &= ~BIT5;
}

void MHal_GPIO_TS1_SYNC_Low(void)
{
    MHal_GPIO_REG(REG_TS1_SYNC_OUT) &= ~BIT6;
}

void MHal_GPIO_TS1_CLK_Low(void)
{
    MHal_GPIO_REG(REG_TS1_CLK_OUT) &= ~BIT7;
}

void MHal_GPIO_I2S_IN_WS_Low(void)
{
    MHal_GPIO_REG(REG_I2S_IN_WS_OUT) &= ~BIT0;
}

void MHal_GPIO_I2S_IN_BCK_Low(void)
{
    MHal_GPIO_REG(REG_I2S_IN_BCK_OUT) &= ~BIT1;
}

void MHal_GPIO_I2S_IN_SD_Low(void)
{
    MHal_GPIO_REG(REG_I2S_IN_SD_OUT) &= ~BIT2;
}

void MHal_GPIO_SPDIF_IN_Low(void)
{
    MHal_GPIO_REG(REG_SPDIF_IN_OUT) &= ~BIT3;
}

void MHal_GPIO_I2S_OUT_MCK_Low(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_MCK_OUT) &= ~BIT5;
}

void MHal_GPIO_I2S_OUT_WS_Low(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_WS_OUT) &= ~BIT6;
}

void MHal_GPIO_I2S_OUT_BCK_Low(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_BCK_OUT) &= ~BIT7;
}

void MHal_GPIO_I2S_OUT_SD_Low(void)
{
    MHal_GPIO_REG(REG_I2S_OUT_SD_OUT) &= ~BIT0;
}

void MHal_GPIO_SPDIF_OUT_Low(void)
{
    MHal_GPIO_REG(REG_SPDIF_OUT_OUT) &= ~BIT4;
}

void MHal_GPIO_DDCR_DA2_Low(void)
{
    MHal_GPIO_REG(REG_DDCR_DA2_OUT) &= ~BIT0;
}

void MHal_GPIO_DDCR_CK2_Low(void)
{
    MHal_GPIO_REG(REG_DDCR_CK2_OUT) &= ~BIT1;
}

void MHal_GPIO_LHSYNC2_Low(void)
{
    MHal_GPIO_REG(REG_LHSYNC2_OUT) &= ~BIT2;
}

void MHal_GPIO_LVSYNC2_Low(void)
{
    MHal_GPIO_REG(REG_LVSYNC2_OUT) &= ~BIT3;
}

void MHal_GPIO_SPI_CK_Low(void)
{
    MHal_GPIO_REG(REG_SPI_CK_OUT) &= ~BIT4;
}

void MHal_GPIO_SPI_DI_Low(void)
{
    MHal_GPIO_REG(REG_SPI_DI_OUT) &= ~BIT5;
}

void MHal_GPIO_SPI_CZ_Low(void)
{
    MHal_GPIO_REG(REG_SPI_CZ_OUT) &= ~BIT6;
}

void MHal_GPIO_SPI_DO_Low(void)
{
    MHal_GPIO_REG(REG_SPI_DO_OUT) &= ~BIT7;
}

void MHal_GPIO_GPIO12_Low(void)
{
    MHal_GPIO_REG(REG_GPIO12_OUT) &= ~BIT4;
}

void MHal_GPIO_GPIO13_Low(void)
{
    MHal_GPIO_REG(REG_GPIO13_OUT) &= ~BIT5;
}

void MHal_GPIO_GPIO14_Low(void)
{
    MHal_GPIO_REG(REG_GPIO14_OUT) &= ~BIT6;
}

void MHal_GPIO_GPIO15_Low(void)
{
    MHal_GPIO_REG(REG_GPIO15_OUT) &= ~BIT7;
}

void MHal_GPIO_ET_CRS_Low(void)
{
    MHal_GPIO_REG(REG_ET_CRS_OUT) &= ~BIT0;
}

void MHal_GPIO_ET_COL_Low(void)
{
    MHal_GPIO_REG(REG_ET_COL_OUT) &= ~BIT1;
}

void MHal_GPIO_ET_TXD3_Low(void)
{
    MHal_GPIO_REG(REG_ET_TXD3_OUT) &= ~BIT1;
}

void MHal_GPIO_ET_TXD2_Low(void)
{
    MHal_GPIO_REG(REG_ET_TXD2_OUT) &= ~BIT2;
}

void MHal_GPIO_ET_TXD1_Low(void)
{
    MHal_GPIO_REG(REG_ET_TXD1_OUT) &= ~BIT2;
}

void MHal_GPIO_ET_TXD0_Low(void)
{
    MHal_GPIO_REG(REG_ET_TXD0_OUT) &= ~BIT3;
}

void MHal_GPIO_ET_TX_EN_Low(void)
{
    MHal_GPIO_REG(REG_ET_TX_EN_OUT) &= ~BIT4;
}

void MHal_GPIO_ET_TX_CLK_Low(void)
{
    MHal_GPIO_REG(REG_ET_TX_CLK_OUT) &= ~BIT5;
}

void MHal_GPIO_ET_TXER_Low(void)
{
    MHal_GPIO_REG(REG_ET_TXER_OUT) &= ~BIT3;
}

void MHal_GPIO_ET_RXER_Low(void)
{
    MHal_GPIO_REG(REG_ET_RXER_OUT) &= ~BIT4;
}

void MHal_GPIO_ET_RX_CLK_Low(void)
{
    MHal_GPIO_REG(REG_ET_RX_CLK_OUT) &= ~BIT5;
}

void MHal_GPIO_ET_RX_DV_Low(void)
{
    MHal_GPIO_REG(REG_ET_RX_DV_OUT) &= ~BIT6;
}

void MHal_GPIO_ET_RXD0_Low(void)
{
    MHal_GPIO_REG(REG_ET_RXD0_OUT) &= ~BIT6;
}

void MHal_GPIO_ET_RXD1_Low(void)
{
    MHal_GPIO_REG(REG_ET_RXD1_OUT) &= ~BIT7;
}

void MHal_GPIO_ET_RXD2_Low(void)
{
    MHal_GPIO_REG(REG_ET_RXD2_OUT) &= ~BIT7;
}

void MHal_GPIO_ET_RXD3_Low(void)
{
    MHal_GPIO_REG(REG_ET_RXD3_OUT) &= ~BIT0;
}

void MHal_GPIO_ET_MDC_Low(void)
{
    MHal_GPIO_REG(REG_ET_MDC_OUT) &= ~BIT0;
}

void MHal_GPIO_ET_MDIO_Low(void)
{
    MHal_GPIO_REG(REG_ET_MDIO_OUT) &= ~BIT1;
}

void MHal_GPIO_LCK_Low(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LCK_OUT) &= ~BIT3;
}

void MHal_GPIO_LDE_Low(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LDE_OUT) &= ~BIT2;
}

void MHal_GPIO_LHSYNC_Low(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LHSYNC_OUT) &= ~BIT1;
}

void MHal_GPIO_LVSYNC_Low(void)
{
    MHal_GPIO_REG(REG_LVDS_BANK) = 0;
    MHal_GPIO_REG(REG_LVSYNC_OUT) &= ~BIT0;
}

void MHal_GPIO_TCON0_Low(void)
{
    MHal_GPIO_REG(REG_TCON0_OUT) &= ~BIT0;
}

void MHal_GPIO_TCON1_Low(void)
{
    MHal_GPIO_REG(REG_TCON1_OUT) &= ~BIT1;
}

void MHal_GPIO_TCON2_Low(void)
{
    MHal_GPIO_REG(REG_TCON2_OUT) &= ~BIT2;
}

void MHal_GPIO_TCON3_Low(void)
{
    MHal_GPIO_REG(REG_TCON3_OUT) &= ~BIT3;
}

void MHal_GPIO_TCON4_Low(void)
{
    MHal_GPIO_REG(REG_TCON4_OUT) &= ~BIT4;
}

void MHal_GPIO_TCON5_Low(void)
{
    MHal_GPIO_REG(REG_TCON5_OUT) &= ~BIT5;
}

void MHal_GPIO_TCON6_Low(void)
{
    MHal_GPIO_REG(REG_TCON6_OUT) &= ~BIT6;
}

void MHal_GPIO_TCON7_Low(void)
{
    MHal_GPIO_REG(REG_TCON7_OUT) &= ~BIT7;
}

void MHal_GPIO_TCON8_Low(void)
{
    MHal_GPIO_REG(REG_TCON8_OUT) &= ~BIT0;
}

void MHal_GPIO_TCON9_Low(void)
{
    MHal_GPIO_REG(REG_TCON9_OUT) &= ~BIT1;
}

void MHal_GPIO_TCON10_Low(void)
{
    MHal_GPIO_REG(REG_TCON10_OUT) &= ~BIT2;
}

void MHal_GPIO_TCON11_Low(void)
{
    MHal_GPIO_REG(REG_TCON11_OUT) &= ~BIT3;
}

void MHal_GPIO_TCON12_Low(void)
{
    MHal_GPIO_REG(REG_TCON12_OUT) &= ~BIT4;
}

void MHal_GPIO_TCON13_Low(void)
{
    MHal_GPIO_REG(REG_TCON13_OUT) &= ~BIT5;
}

void MHal_GPIO_TCON14_Low(void)
{
    MHal_GPIO_REG(REG_TCON14_OUT) &= ~BIT6;
}

void MHal_GPIO_TCON15_Low(void)
{
    MHal_GPIO_REG(REG_TCON15_OUT) &= ~BIT7;
}

void MHal_GPIO_TCON16_Low(void)
{
    MHal_GPIO_REG(REG_TCON16_OUT) &= ~BIT0;
}

void MHal_GPIO_HOTPLUGB_Low(void)
{
    MHal_GPIO_REG(REG_HOTPLUGB_OUT) &= ~BIT5;
}

void MHal_GPIO_HOTPLUGA_Low(void)
{
    MHal_GPIO_REG(REG_HOTPLUGA_OUT) &= ~BIT4;
}

void MHal_GPIO_HOTPLUGC_Low(void)
{
    MHal_GPIO_REG(REG_HOTPLUGC_OUT) &= ~BIT4;
}


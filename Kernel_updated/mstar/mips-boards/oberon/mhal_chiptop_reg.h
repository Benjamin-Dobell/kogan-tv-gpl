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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    regSystem.h
/// @brief  System Chip Top Registers Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_SYSTEM_H_
#define _REG_SYSTEM_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define BMASK(bits)                 (BIT(((1)?bits)+1)-BIT(((0)?bits)))
#define BMASK_L(bits)               (BMASK(bits)&0xFFFF)
#define BMASK_H(bits)               (BMASK(bits)>>16)
#define BITS(bits,value)            ((BIT(((1)?bits)+1)-BIT(((0)?bits))) & (value<<((0)?bits)))


#define REG_TOP_BASE                0xBF803C00
#define TOP_REG(addr)               (*((volatile u32*)(REG_TOP_BASE + ((addr)<<2))))


#define REG_TOP_CHIP_CONFIG_OW      0x0000
#define REG_TOP_UTMI_UART_SETHL     0x0001
    #define TOP_UART_RX_EN                      BIT(10)
#define REG_TOP_TEST_IN_OUT         0x0002
    #define TOP_TEST_IN_MASK                    BMASK(5:4)
    #define TOP_TEST_IN_TS0                     BITS(5:4, 1)
    #define TOP_TEST_IN_PCI                     BITS(5:4, 2)
    #define TOP_TEST_IN_I2S                     BITS(5:4, 3)
    #define TOP_TEST_OUT_MASK                   BMASK(7:6)
    #define TOP_GPIO_AU0_EN                     BIT(8)
    #define TOP_GPIO_AU1_EN                     BIT(9)
    #define TOP_GPIO_AU2_EN                     BIT(10)
    #define TOP_GPIO_AU3_EN                     BIT(11)
    #define TOP_GPIO_AU4_EN                     BIT(12)
    #define TOP_GPIO_AU5_EN                     BIT(13)
    #define TOP_GPIO_AU6_EN                     BIT(14)
    #define TOP_GPIO_AU7_EN                     BIT(15)
#define REG_TOP_PWM                 0x0003
    #define TOP_DUMMY_10_MASK                   BMASK(5:0)
    #define TOP_PWM_OEN_TS_ERR                  BIT(13)

#define REG_TOP_PCI                 0x0004
#define REG_TOP_SPI_EMAC            0x0005
    #define TOP_CKG_EMACRX_MASK                 BMASK(12:10)
    #define TOP_CKG_EMACRX_DIS                  BIT(10)
    #define TOP_CKG_EMACRX_INV                  BIT(11)
    #define TOP_CKG_EMACRX_CLK_EMAC_RX_IN_BUF   BITS(12:12, 0)
    #define TOP_CKG_EMACRX_CLK_DFT              BITS(12:12, 1)
    #define TOP_CKG_EMACTX_MASK                 BMASK(15:13)
    #define TOP_CKG_EMACTX_DIS                  BIT(13)
    #define TOP_CKG_EMACTX_INV                  BIT(14)
    #define TOP_CKG_EMACTX_CLK_EMAC_TX_IN_BUF   BITS(15:15, 0)
    #define TOP_CKG_EMACTX_CLK_DFT              BITS(15:15, 1)
#define REG_TOP_CR_CI_GPIO          0x0006
    #define TOP_SM2_GPIO_USE                    BIT(0)
    #define TOP_PAD_CI_ADHI_MASK                BMASK(2:1)
    #define TOP_PAD_CI_ADHI_CCIR                BITS(2:1, 1)            // GPIO_CI5
    #define TOP_PAD_CI_ADHI_CI                  BITS(2:1, 3)
    #define TOP_PACK1_TSO_SWITCH                BIT(3)
    #define TOP_PACK2_CI_SWITCH                 BIT(4)
    #define TOP_PACK9_CI_SWITCH                 BIT(5)
    #define TOP_PACK1_CI_SWITCH                 BIT(7)
    #define TOP_PACKAGE                         BMASK(13:10)
    #define TOP_PACKAGE_0                       BITS(13:10, 0)
    #define TOP_PACKAGE_1                       BITS(13:10, 1)
    #define TOP_PACKAGE_2                       BITS(13:10, 2)
    #define TOP_PACKAGE_3                       BITS(13:10, 3)
    #define TOP_PACKAGE_4                       BITS(13:10, 4)
    #define TOP_PACKAGE_7                       BITS(13:10, 7)
    #define TOP_PACKAGE_8                       BITS(13:10, 8)
    #define TOP_PACKAGE_9                       BITS(13:10, 9)
    #define TOP_SPICZ1_TYPE                     BIT(14)

#define REG_TOP_PCI_PD_TEST         0x0007
    #define TOP_PCI_RPU                         BMASK(9:0)
    #define TOP_PD_ALL                          BIT(10)
    #define TOP_TEST_CLK_MODE                   BIT(11)
    #define TOP_TURN_OFF_PAD                    BIT(12)
#define REG_TOP_CKG_ADCD_DVBC       0x0008
    #define TOP_CKG_ADCD_SHFT                   0
    #define TOP_CKG_ADCD_MASK                   BMASK(3:0)
    #define TOP_CKG_ADCD_DIS                    BIT(0)
    #define TOP_CKG_ADCD_INV                    BIT(1)
    #define TOP_CKG_ADCD_SRC_MASK               BMASK(3:2)
    #define TOP_CKG_ADCD_SRC_ADC                BITS(3:2, 0)
    #define TOP_CKG_ADCD_SRC_XTAL               BITS(3:2, 1)
    #define TOP_CKG_ADCD_SRC_EXTADC             BITS(3:2, 2)
    #define TOP_CKG_ADCD_SRC_DFT                BITS(3:2, 3)
    #define TOP_CKG_DVBC_ADC_SHFT               4
    #define TOP_CKG_DVBC_ADC_MASK               BMASK(7:4)
    #define TOP_CKG_DVBC_INN_SHFT               8
    #define TOP_CKG_DVBC_INN_MASK               BMASK(11:8)
    #define TOP_CKG_DVBC_INN_DIS                BIT(8)
    #define TOP_CKG_DVBC_INN_INV                BIT(9)
    #define TOP_CKG_DVBC_INN_SRC_MASK           BMASK(11:10)              // 3:DFT_Live, other:clk_31
    #define TOP_CKG_DVBC_INN_SRC_31             BITS(11:10, 0)
    #define TOP_CKG_DVBC_INN_SRC_DFT            BITS(11:10, 3)
    #define TOP_CKG_DVBC_INN2X_SHFT             12
    #define TOP_CKG_DVBC_INN2X_MASK             BMASK(15:12)
    #define TOP_CKG_DVBC_INN2X_DIS              BIT(12)
    #define TOP_CKG_DVBC_INN2X_INV              BIT(13)
    #define TOP_CKG_DVBC_INN2X_SRC              BMASK(15:14)              // 3:DFT_Live, other:clk_62
    #define TOP_CKG_DVBC_INN2X_SRC_62           BITS(15:14, 0)
    #define TOP_CKG_DVBC_INN2X_SRC_DFT          BITS(15:14, 3)

#define REG_TOP_DVBC_SC_CCIR        0x0009
    #define TOP_CKG_DVBC_EQ_SHFT                0
    #define TOP_CKG_DVBC_EQ_MASK                BMASK(3:0)
    #define TOP_CKG_DVBC_EQ_DIS                 BIT(0)
    #define TOP_CKG_DVBC_EQ_INV                 BIT(1)
    #define TOP_CKG_DVBC_EQ_SRC                 BMASK(3:2)               // 3:DFT_Live, other:clk_18
    #define TOP_CKG_DVBC_EQ_SRC_18              BITS(3:2, 0)
    #define TOP_CKG_DVBC_EQ_SRC_DFT             BITS(3:2, 3)
    #define TOP_CKG_DVBC_EQ8X_MASK              BMASK(7:4)
    #define TOP_CKG_DVBC_EQ8X_DIS               BIT(4)
    #define TOP_CKG_DVBC_EQ8X_INV               BIT(5)
    #define TOP_CKG_DVBC_EQ8X_SRC               BMASK(7:6)               // 3:DFT_Live, other:clk_144
    #define TOP_CKG_DVBC_EQ8X_SRC_144           BITS(7:6, 0)
    #define TOP_CKG_DVBC_EQ8X_SRC_DFT           BITS(7:6, 3)
    #define TOP_CKG_SC_MASK                     BMASK(13:8)
    #define TOP_CKG_SC_DIS                      BIT(8)
    #define TOP_CKG_SC_INV                      BIT(9)
    #define TOP_CKG_SC_SRC                      BMASK(13:10)
    #define TOP_CKG_SC_SRC_27                   BITS(13:10, 0)
    #define TOP_CKG_SC_SRC_54                   BITS(13:10, 1)
    #define TOP_CKG_SC_SRC_MEMPLL_DIV2          BITS(13:10, 2)
    #define TOP_CKG_SC_SRC_144                  BITS(13:10, 3)
    #define TOP_CKG_SC_SRC_173                  BITS(13:10, 4)
    #define TOP_CKG_SC_SRC_192                  BITS(13:10, 5)
    #define TOP_CKG_SC_SRC_MEMPLL               BITS(13:10, 6)
    #define TOP_CKG_SC_SRC_216                  BITS(13:10, 7)
    #define TOP_CKG_SC_SRC_DFT                  BITS(13:10, 8)
    #define TOP_CLK_CCIR_INV                    BIT(14)

#define REG_TOP_TSOUT_DISP          0x000A
    #define TOP_PCI_RPUPI                       BMASK(9:0)
    #define TOP_CLK_TSOUT_INV                   BIT(10)
    #define TOP_CKG_DISP                        BMASK(15:12)
    #define TOP_CKG_DISP_DIS                    BIT(12)
    #define TOP_CKG_DISP_INV                    BIT(13)
    #define TOP_CKG_DISP_SRC                    BMASK(15:14)
    #define TOP_CKG_DISP_CLK_27_BUF             BITS(15:14, 0)
    #define TOP_CKG_DISP_CLK_54_BUF             BITS(15:14, 1)
    #define TOP_CKG_DISP_HDMIPLL_CLK_BUF_DIV2   BITS(15:14, 2)
    #define TOP_CKG_DISP_HDMIPLL_CLK_BUF        BITS(15:14, 3)

#define REG_TOP_DVBC_HDMI_CI        0x000B
    #define TOP_CKG_DVBC_TS_MASK                BMASK(4:0)
    #define TOP_CKG_DVBC_TS_DIS                 BIT(0)
    #define TOP_CKG_DVBC_TS_INV                 BIT(1)
    #define TOP_CKG_DVBC_TS_SRC_MASK            BMASK(4:0)
    #define TOP_CKG_DVBC_TS_DIS                 BIT(0)
    #define TOP_CKG_DVBC_TS_INV                 BIT(1)
    #define TOP_CKG_DVBC_TS_SRC_62              BITS(4:2, 0)
    #define TOP_CKG_DVBC_TS_SRC_54              BITS(4:2, 1)
    #define TOP_CKG_DVBC_TS_SRC_43P2            BITS(4:2, 2)
    #define TOP_CKG_DVBC_TS_SRC_6               BITS(4:2, 3)
    #define TOP_CKG_DVBC_TS_SRC_7P2             BITS(4:2, 4)
    #define TOP_CKG_DVBC_TS_SRC_7P75            BITS(4:2, 5)
    #define TOP_CKG_DVBC_TS_SRC_9               BITS(4:2, 6)
    #define TOP_CKG_DVBC_TS_SRC_DFT             BITS(4:2, 7)
    #define TOP_PACK9_TSO_SWITCH_BIT0           BIT(5)
    #define TOP_TSI_GPIO_USE                    BIT(7)
    #define TOP_PAD_CI_CTL_MASK                 BIT(9)
    #define TOP_PAD_CI_CTL_X                    0
    #define TOP_PAD_CI_CTL_GPIO                 BIT(9)                  // GPIO_CI8_9
#define REG_TOP_PCI_RPD             0x000D
#define REG_TOP_GPIO_TSI_OUT        0x000E
    #define TOP_GPIO_CI_A23                     BIT(0)
    #define TOP_CI_USE                          BIT(1)
    #define TOP_NOR_8X                          BIT(2)
    #define TOP_SM2_OPEN_DRAIN                  BIT(3)
    #define TOP_SM1_OPEN_DRAIN                  BIT(4)
    #define TOP_GPIO_W67_USE                    BIT(5)
    #define TOP_GPIO_W89_USE                    BIT(6)
    #define TOP_PACK9_TSO_SWITCH_BIT1           BIT(9)
#define REG_TOP_POFF_MCU            0x0010
    #define TOP_POFF_TS0MUX_MASK                BMASK(7:6)
    #define TOP_POFF_TS0MUX_PAD0                BITS(7:6, 0)
    #define TOP_POFF_TS0MUX_PAD1                BITS(7:6, 1)
    #define TOP_POFF_TS0MUX_DVBC                BITS(7:6, 3)
    #define TOP_POFF_TS1MUX_MASK                BMASK(9:8)
    #define TOP_POFF_TS1MUX_PAD0                BITS(9:8, 0)
    #define TOP_POFF_TS1MUX_PAD1                BITS(9:8, 1)
    #define TOP_POFF_TS1MUX_DVBC                BITS(9:8, 3)
    #define TOP_INT_TYPE                        BMASK(12:10)
    #define TOP_INT_TYPE1                       BITS(12:10, 0)
    #define TOP_INT_TYPE2                       BITS(12:10, 1)
    #define TOP_INT_TYPE3                       BITS(12:10, 2)
    #define TOP_INT_TYPE4                       BITS(12:10, 3)
#define REG_TOP_MCU_USB_STC0        0x0011
    #define TOP_MCU_CLK_MASK                    BMASK(0:0)
    #define TOP_MCU_CLK_DFT                     BITS(0:0, 0)
    #define TOP_MCU_CLK_SRC_MCUCLK              BITS(0:0, 1)
    #define TSP_STC0_SEL                        BIT(7)
#define REG_TOP_USB_RIU_DDR_MIU     0x0012
    #define TOP_CKG_USB_MASK                    BMASK(1:0)
    #define TOP_CKG_USB_DIS                     BIT(0)
    #define TOP_CKG_USB_INV                     BIT(1)
    #define TOP_CKG_MIU_DIS                     BIT(12)
    #define TOP_CKG_MIU_INV                     BIT(13)
    #define TOP_CKG_MIU_SRC_MASK                BMASK(15:14)
    #define TOP_CKG_MIU_SRC_MEMPLL              BITS(15:14, 0)
    #define TOP_CKG_MIU_SRC_170                 BITS(15:14, 1)
    #define TOP_CKG_MIU_SRC_MEMPLL_DIV2         BITS(15:14, 2)
    #define TOP_CKG_MIU_SRC_192                 BITS(15:14, 3)
#define REG_TOP_TS0_TSP_STC0        0x0013
    #define TOP_CKG_TS0_MASK                    BMASK(3:0)
    #define TOP_CKG_TS0_DIS                     BIT(0)
    #define TOP_CKG_TS0_INV                     BIT(1)
    #define TOP_CKG_TS0_SRC_MASK                BMASK(3:2)
    #define TOP_CKG_TS0_SRC_DVB                 BITS(3:2, 0)
    #define TOP_CKG_TS0_SRC_TS0                 BITS(3:2, 1)
    #define TOP_CKG_TS0_SRC_TS1                 BITS(3:2, 2)
    #define TOP_CKG_TS0_SRC_DFT                 BITS(3:2, 3)
    #define TOP_CKG_TS0_SRC_DEMOD               TOP_CKG_TS0_SRC_DVB
    #define TOP_CKG_TS0_SRC_CI                  TOP_CKG_TS0_SRC_TS0
    #define TOP_CKG_TSP_MASK                    BMASK(11:6)
    #define TOP_CKG_TSP_DIS                     BIT(6)
    #define TOP_CKG_TSP_INV                     BIT(7)
    #define TOP_CKG_TSP_SRC_144                 BITS(11:8, 0)
    #define TOP_CKG_TSP_SRC_123                 BITS(11:8, 1)
    #define TOP_CKG_TSP_SRC_72                  BITS(11:8, 4)
    #define TOP_CKG_TSP_SRC_DFT                 BITS(11:8, 7)
    #define TOP_CKG_STC0_MASK                   BMASK(15:12)
    #define TOP_CKG_STC0_DIS                    BIT(12)
    #define TOP_CKG_STC0_INV                    BIT(13)
    #define TOP_CKG_STC0_SRC_STC0               BITS(15:14, 0)
    #define TOP_CKG_STC0_SRC_1                  BITS(15:14, 1)
    #define TOP_CKG_STC0_SRC_27                 BITS(15:14, 2)
    #define TOP_CKG_STC0_SRC_DFT                BITS(15:14, 3)
#define REG_TOP_MAD_MVD             0x0014
    #define TOP_CKG_MAD_STC_MASK                BMASK(3:0)
    #define TOP_CKG_MAD_STC_DIS                 BIT(0)
    #define TOP_CKG_MAD_STC_INV                 BIT(1)
    #define TOP_CKG_MAD_STC_SRC_STC0            BITS(3:2, 0)
    #define TOP_CKG_MAD_STC_SRC_27              BITS(3:2, 2)
    #define TOP_CKG_MAD_STC_SRC_DFT             BITS(3:2, 3)
    #define TOP_CKG_MVD_BOOT_MASK               BMASK(9:4)
    #define TOP_CKG_MVD_BOOT_DIS                BIT(4)
    #define TOP_CKG_MVD_BOOT_INV                BIT(5)
    #define TOP_CKG_MVD_BOOT_SRC_144            BITS(9:6, 0)
    #define TOP_CKG_MVD_BOOT_SRC_123            BITS(9:6, 1)
    #define TOP_CKG_MVD_BOOT_SRC_108            BITS(9:6, 2)
    #define TOP_CKG_MVD_BOOT_SRC_86             BITS(9:6, 3)
    #define TOP_CKG_MVD_BOOT_SRC_72             BITS(9:6, 4)
    #define TOP_CKG_MVD_BOOT_SRC_54             BITS(9:6, 5)
    #define TOP_CKG_MVD_BOOT_SRC_MIU            BITS(9:6, 6)
    #define TOP_CKG_MVD_BOOT_SRC_DFT            BITS(9:6, 7)
    #define TOP_CKG_MVD_MASK                    BMASK(15:10)
    #define TOP_CKG_MVD_DIS                     BIT(10)
    #define TOP_CKG_MVD_INV                     BIT(11)
    #define TOP_CKG_MVD_SRC_144                 BITS(15:12, 0)
    #define TOP_CKG_MVD_SRC_123                 BITS(15:12, 1)
    #define TOP_CKG_MVD_SRC_108                 BITS(15:12, 2)
    #define TOP_CKG_MVD_SRC_86                  BITS(15:12, 3)
    #define TOP_CKG_MVD_SRC_72                  BITS(15:12, 4)
    #define TOP_CKG_MVD_SRC_54                  BITS(15:12, 5)
    #define TOP_CKG_MVD_SRC_MIU                 BITS(15:12, 6)
    #define TOP_CKG_MVD_SRC_DFT                 BITS(15:12, 7)
#define REG_TOP_SVD_EMAC_GE         0x0015
    #define TOP_CKG_SVD_MASK                    BMASK(5:0)
    #define TOP_CKG_SVD_DIS                     BIT(0)
    #define TOP_CKG_SVD_INV                     BIT(1)
    #define TOP_CKG_SVD_SRC_240                 BITS(5:2, 0)
    #define TOP_CKG_SVD_SRC_216                 BITS(5:2, 1)
    #define TOP_CKG_SVD_SRC_192                 BITS(5:2, 2)
    #define TOP_CKG_SVD_SRC_123                 BITS(5:2, 3)
    #define TOP_CKG_SVD_SRC_86                  BITS(5:2, 4)
    #define TOP_CKG_SVD_SRC_72                  BITS(5:2, 5)
    #define TOP_CKG_SVD_SRC_54                  BITS(5:2, 6)
    #define TOP_CKG_SVD_SRC_DFT                 BITS(5:2, 7)
    #define TOP_CKG_EMACAHB_MASK                BMASK(11:8)
    #define TOP_CKG_EMACAHB_DIS                 BIT(8)
    #define TOP_CKG_EMACAHB_INV                 BIT(9)
    #define TOP_CKG_EMACAHB_SRC_123             BITS(11:10, 0)
    #define TOP_CKG_EMACAHB_SRC_108             BITS(11:10, 1)
    #define TOP_CKG_EMACAHB_SRC_86              BITS(11:10, 2)
    #define TOP_CKG_EMACAHB_SRC_DFT             BITS(11:10, 3)
    #define TOP_CKG_GE_MASK                     BMASK(15:12)
    #define TOP_CKG_GE_DIS                      BIT(12)
    #define TOP_CKG_GE_INV                      BIT(13)
    #define TOP_CKG_GE_SRC_123                  BITS(15:12, 0)
    #define TOP_CKG_GE_SRC_108                  BITS(15:12, 1)
    #define TOP_CKG_GE_SRC_86                   BITS(15:12, 2)
    #define TOP_CKG_GE_SRC_DFT                  BITS(15:12, 3)

#define REG_TOP_GOP                 0x0016
    #define TOP_CKG_GOP0_MASK                   BMASK(4:0)
    #define TOP_CKG_GOP0_DIS                    BIT(0)
    #define TOP_CKG_GOP0_INV                    BIT(1)
    #define TOP_CKG_GOP0_SRC_MASK               BMASK(4:2)
    #define TOP_CKG_GOP0_SRC_OSDI               BITS(3:2, 0)
    #define TOP_CKG_GOP0_SRC_SC                 BITS(3:2, 1)
    #define TOP_CKG_GOP0_SRC_TCON               BITS(3:2, 2)
    #define TOP_CKG_GOP0_SRC_VE                 BITS(3:2, 3)
    #define TOP_CKG_GOP0_SRC_DFT                BIT(4)
    #define TOP_CKG_GOP1_MASK                   BMASK(9:5)
    #define TOP_CKG_GOP1_DIS                    BIT(5)
    #define TOP_CKG_GOP1_INV                    BIT(6)
    #define TOP_CKG_GOP1_SRC_MASK               BMASK(9:7)
    #define TOP_CKG_GOP1_SRC_OSDI               BITS(8:7, 0)
    #define TOP_CKG_GOP1_SRC_SC                 BITS(8:7, 1)
    #define TOP_CKG_GOP1_SRC_TCON               BITS(8:7, 2)
    #define TOP_CKG_GOP1_SRC_VE                 BITS(8:7, 3)
    #define TOP_CKG_GOP1_SRC_DFT                BIT(9)
    #define TOP_CKG_GOP2_MASK                   BMASK(14:10)
    #define TOP_CKG_GOP2_DIS                    BIT(10)
    #define TOP_CKG_GOP2_INV                    BIT(11)
    #define TOP_CKG_GOP2_SRC_MASK               BMASK(14:12)
    #define TOP_CKG_GOP2_SRC_OSDI               BITS(13:12, 0)
    #define TOP_CKG_GOP2_SRC_SC                 BITS(13:12, 1)
    #define TOP_CKG_GOP2_SRC_TCON               BITS(13:12, 2)
    #define TOP_CKG_GOP2_SRC_VE                 BITS(13:12, 3)
    #define TOP_CKG_GOP2_SRC_DFT                BIT(14)
#define REG_TOP_GOP_UART            0x0017
    #define TOP_CKG_GOP_PALETTE0_MASK           BMASK(1:0)
    #define TOP_CKG_GOP_PALETTE0_DIS            BIT(0)
    #define TOP_CKG_GOP_PALETTE0_INV            BIT(1)
    #define TOP_CKG_GOP_PALETTE1_MASK           BMASK(3:2)
    #define TOP_CKG_GOP_PALETTE1_DIS            BIT(2)
    #define TOP_CKG_GOP_PALETTE1_INV            BIT(3)
    #define TOP_CKG_GOP3_MASK                   BMASK(8:4)
    #define TOP_CKG_GOP3_DIS                    BIT(4)
    #define TOP_CKG_GOP3_INV                    BIT(5)
    #define TOP_CKG_GOP3_SRC_MASK               BMASK(8:6)
    #define TOP_CKG_GOP3_SRC_OSDI               BITS(7:6, 0)
    #define TOP_CKG_GOP3_SRC_SC                 BITS(7:6, 1)
    #define TOP_CKG_GOP3_SRC_TCON               BITS(7:6, 2)
    #define TOP_CKG_GOP3_SRC_VE                 BITS(7:6, 3)
    #define TOP_CKG_GOP3_SRC_DFT                BIT(8)
    #define TOP_CKG_GOPD_MASK                   BMASK(12:9)
    #define TOP_CKG_GOPD_DIS                    BIT(9)
    #define TOP_CKG_GOPD_INV                    BIT(10)
    #define TOP_CKG_GOPD_SRC_MASK               BMASK(8:6)
    #define TOP_CKG_GOPD_SRC_TCON               BITS(12:11, 0)
    #define TOP_CKG_GOPD_SRC_SC                 BITS(12:11, 1)
    #define TOP_CKG_GOPD_SRC_OSDI               BITS(12:11, 2)
    #define TOP_CKG_GOPD_SRC_DFT                BITS(12:11, 3)
    #define TOP_CKG_UART_MASK                   BMASK(15:13)
    #define TOP_CKG_UART_DIS                    BIT(13)
    #define TOP_CKG_UART_INV                    BIT(14)
    #define TOP_CKG_UART_SRC_UART               BITS(15:15, 0)
    #define TOP_CKG_UART_SRC_DFT                BITS(15:15, 1)
#define REG_TOP_SM_PCM              0x0018
    #define TOP_CKG_SM_MASK                     BMASK(11:8)
    #define TOP_CKG_SM_DIS                      BIT(8)
    #define TOP_CKG_SM_INV                      BIT(9)
    #define TOP_CKG_SM_SRC_6                    BITS(11:10, 0)
    #define TOP_CKG_SM_SRC_4P5                  BITS(11:10, 1)
    #define TOP_CKG_SM_SRC_3                    BITS(11:10, 2)
    #define TOP_CKG_SM_SRC_DFT                  BITS(11:10, 3)
    #define TOP_CKG_PCM_MASK                    BMASK(15:12)
    #define TOP_CKG_PCM_DIS                     BIT(12)
    #define TOP_CKG_PCM_INV                     BIT(13)
    #define TOP_CKG_PCM_SRC_27                  BITS(15:14, 0)
    #define TOP_CKG_PCM_SRC_DFT                 BITS(15:14, 2)
#define REG_TOP_VE                  0x0019
    #define TOP_CKG_VE_MASK                     BMASK(11:8)
    #define TOP_CKG_VE_DIS                      BIT(8)
    #define TOP_CKG_VE_INV                      BIT(9)
    #define TOP_CKG_VE_SRC_27                   BITS(11:10, 0)
    #define TOP_CKG_VE_SRC_DFT                  BITS(11:10, 2)
    #define TOP_CKG_VEDAC_MASK                  BMASK(15:12)
    #define TOP_CKG_VEDAC_DIS                   BIT(12)
    #define TOP_CKG_VEDAC_INV                   BIT(13)
    #define TOP_CKG_VEDAC_SRC_27                BITS(15:14, 0)
    #define TOP_CKG_VEDAC_SRC_54                BITS(15:14, 1)
    #define TOP_CKG_VEDAC_SRC_108               BITS(15:14, 2)
    #define TOP_CKG_VEDAC_SRC_DFT               BITS(15:14, 3)
#define REG_TOP_DAC_SDR             0x001A
    #define TOP_CKG_DAC_MASK                    BMASK(4:0)
    #define TOP_CKG_DAC_DIS                     BIT(0)
    #define TOP_CKG_DAC_INV                     BIT(1)
    #define TOP_CKG_DAC_SRC_DISP                BITS(4:2, 0)            //[???]
    #define TOP_CKG_DAC_SRC_VEDAC               BITS(4:2, 1)
    #define TOP_CKG_DAC_SRC_DVBC_INN            BITS(4:2, 2)
    #define TOP_CKG_DAC_SRC_DVBC_EQ             BITS(4:2, 3)
    #define TOP_CKG_DAC_SRC_ADCD_D              BITS(4:2, 4)
    #define TOP_CKG_DAC_SRC_DFT                 BITS(4:2, 6)
#define REG_TOP_SDIO_EMAC_USB       0x001B
    #define TOP_USB_DRVVBUS                     BIT(14)
    #define TOP_USB_DRVVBUS1                    BIT(15)
#define REG_TOP_VE_SDIO             0x001C
    #define TOP_CKG_VEIN_MASK                   BMASK(3:0)
    #define TOP_CKG_VEIN_DIS                    BIT(0)
    #define TOP_CKG_VEIN_INV                    BIT(1)
    #define TOP_CKG_VEIN_SRC_DISP               BITS(3:2, 0)
    #define TOP_CKG_VEIN_SRC_27                 BITS(3:2, 1)
    #define TOP_CKG_VEIN_SC_P                   BITS(3:2, 2)
    #define TOP_CKG_VEIN_SRC_DFT                BITS(3:2, 3)
    #define TOP_CKG_SDIO_MASK                   BMASK(14:8)
    #define TOP_CKG_SDIO_DIS                    BIT(8)
    #define TOP_CKG_SDIO_INV                    BIT(9)
    #define TOP_CKG_SDIO_SRC_86D256             BITS(14:10, 0)
    #define TOP_CKG_SDIO_SRC_86D64              BITS(14:10, 1)
    #define TOP_CKG_SDIO_SRC_86D16              BITS(14:10, 2)
    #define TOP_CKG_SDIO_SRC_54D4               BITS(14:10, 3)
    #define TOP_CKG_SDIO_SRC_72D4               BITS(14:10, 4)
    #define TOP_CKG_SDIO_SRC_86D4               BITS(14:10, 5)
    #define TOP_CKG_SDIO_SRC_54D2               BITS(14:10, 6)
    #define TOP_CKG_SDIO_SRC_72D2               BITS(14:10, 7)
    #define TOP_CKG_SDIO_SRC_86D2               BITS(14:10, 8)
    #define TOP_CKG_SDIO_SRC_54                 BITS(14:10, 9)
    #define TOP_CKG_SDIO_SRC_72                 BITS(14:10,10)
    #define TOP_CKG_SDIO_SRC_SDIO_SSC           BITS(14:10,11)
    #define TOP_CKG_SDIO_SRC_DFT                BITS(14:10,16)
#define REG_TOP_DC0_NUM             0x0020
#define REG_TOP_DC0_DEN             0x0021
#define REG_TOP_MCU                 0x0022
    #define TOP_CKG_MCU_MASK                    BMASK(12:8)
    #define TOP_CKG_MCU_DIS                     BIT(8)
    #define TOP_CKG_MCU_INV                     BIT(9)
    #define TOP_CKG_MCU_SRC_MASK                BMASK(12:10)
    #define TOP_CKG_MCU_SRC_216                 BITS(12:10, 0)
    #define TOP_CKG_MCU_SRC_DRAM                BITS(12:10, 1)
    #define TOP_CKG_MCU_SRC_192                 BITS(12:10, 2)
    #define TOP_CKG_MCU_SRC_173                 BITS(12:10, 3)
    #define TOP_CKG_MCU_SRC_160                 BITS(12:10, 4)
    #define TOP_CKG_MCU_SRC_123                 BITS(12:10, 5)
    #define TOP_CKG_MCU_SRC_108                 BITS(12:10, 6)
    #define TOP_CKG_MCU_SRC_DFT                 BITS(12:10, 7)
#define REG_TOP_USB_CW_L            0x0024
#define REG_TOP_USB_CW_H            0x0025
#define REG_TOP_STC0_CW_L           0x0026
#define REG_TOP_STC0_CW_H           0x0027
#define REG_TOP_DC0_CW_L            0x002A
#define REG_TOP_DC0_CW_H            0x002B
#define REG_TOP_002C                0x002C
    #define TOP_GPIO_TS1_DIS                    BIT(0)
    #define TOP_CI_NOT_PF_USE                   BIT(1)
    // #define TOP_TSO_GPIO                        0x0001 // [ ???]
#define REG_TOP_TSPUART_SM2         0x002D
    #define TOP_CKG_SM2_SHFT                    8
    #define TOP_CKG_SM2_MASK                    BMASK(11:8)
    #define TOP_CKG_SM2_DIS                     BIT(8)
    #define TOP_CKG_SM2_INV                     BIT(9)
    #define TOP_CKG_SM2_SRC_6                   BITS(11:10, 0)
    #define TOP_CKG_SM2_SRC_4P5                 BITS(11:10, 1)
    #define TOP_CKG_SM2_SRC_3                   BITS(11:10, 2)
    #define TOP_CKG_SM2_SRC_DFT                 BITS(11:10, 3)
#define REG_TOP_MIU_DAC2_JPG        0x002E
    #define TOP_CKG_MIU_REC_MASK                BMASK(3:0)
    #define TOP_CKG_MIU_REC_DIS                 BIT(0)
    #define TOP_CKG_MIU_REC_INV                 BIT(1)
    #define TOP_CKG_MIU_REC_SRC_XTAL_DIV2       BITS(2:2, 0)
    #define TOP_CKG_MIU_REC_SRC_DFT             BITS(2:2, 1)
    #define TOP_CKG_DAC2_MASK                   BMASK(8:4)
    #define TOP_CKG_DAC2_DIS                    BIT(4)
    #define TOP_CKG_DAC2_INV                    BIT(5)
    #define TOP_CKG_DAC2_SRC_DISP               BITS(8:6, 0)            //[???]
    #define TOP_CKG_DAC2_SRC_VEDAC              BITS(8:6, 1)
    #define TOP_CKG_DAC2_SRC_DVBC_INN           BITS(8:6, 2)
    #define TOP_CKG_DAC2_SRC_DVBC_EQ            BITS(8:6, 3)
    #define TOP_CKG_DAC2_SRC_ADCD_D             BITS(8:6, 4)
    #define TOP_CKG_DAC2_SRC_DFT                BITS(8:6, 6)
    #define TOP_CKG_JPG_MASK                    BMASK(12:9)
    #define TOP_CKG_JPG_DIS                     BIT(9)
    #define TOP_CKG_JPG_INV                     BIT(10)
    #define TOP_CKG_JPG_SRC_123                 BITS(12:11, 0)            //[???]
    #define TOP_CKG_JPG_SRC_108                 BITS(12:11, 1)
    #define TOP_CKG_JPG_SRC_86                  BITS(12:11, 2)
    #define TOP_CKG_JPG_SRC_DFT                 BITS(12:11, 3)

#define REG_TOP_GPIO_WN_IN          0x0030
#define REG_TOP_GPIO_WN_OUT         0x0031

#define REG_TOP_GPIO_WN_OEN_DVBC    0x0036
/*
    #define TOP_GPIO_W0                         BIT(0)
    #define TOP_GPIO_W1                         BIT(1)
    #define TOP_GPIO_N0                         BIT(4)
    #define TOP_GPIO_N1                         BIT(5)
    #define TOP_GPIO_N2                         BIT(6)
    #define TOP_GPIO_N3                         BIT(7)
*/
    #define TOP_CKG_MVD_IAP_RMEM_DIS            BIT(0)
    #define TOP_CKG_MVD_IAP_RMEM_INV            BIT(1)
    #define TOP_CKG_MVD_IAP_RMEM_MVD_OR_MIU     BITS(2:2, 0)
    #define TOP_CKG_MVD_IAP_RMEM_DFT            BITS(2:2, 1)
    #define TOP_CKG_DVBC_DMA_MASK               BMASK(11:8)
    #define TOP_CKG_DVBC_DMA_EQ                 BITS(11:8, 0)
    #define TOP_CKG_DVBC_DMA_ADCD               BITS(11:8, 1)
    #define TOP_CKG_DVBC_DMA_0                  BITS(11:8, 2)
    #define TOP_CKG_DVBC_DMA_DFT                BITS(11:8, 3)
#define REG_TOP_DUMMY23             0x0037
    #define TOP_CKG_DUMMY23_MASK                BMASK(10:0)
#define REG_TOP_GPIO_PWM_CI_IN      0x0039
#define REG_TOP_GPIO_PWM_CI_OUT     0x003A
    #define TOP_TS_ERR                          BIT(0)
    #define TOP_CKG_TS1_MASK                    BMASK(3:1)
    #define TOP_GPIO_TS1_INV                    BIT(1)
    #define TOP_GPIO_TS1_SRC_MASK               BMASK(3:2)
    #define TOP_GPIO_TS1_SRC_DVB                BITS(3:2, 0)
    #define TOP_GPIO_TS1_SRC_TS0                BITS(3:2, 1)
    #define TOP_GPIO_TS1_SRC_TS1                BITS(3:2, 2)
    #define TOP_GPIO_TS1_SRC_DFT                BITS(3:2, 3)
    #define TOP_GPIO_TS1_SRC_DEMOD              TOP_GPIO_TS1_SRC_DVB
    #define TOP_GPIO_TS1_SRC_CI                 TOP_GPIO_TS1_SRC_TS0
    #define TOP_GPIO_PWM0_OUT                   BIT(4)
    #define TOP_GPIO_PWM1_OUT                   BIT(5)
/*
    #define TOP_GPIO_CI0_OUT                    BIT(6)
    #define TOP_GPIO_CI1_OUT                    BIT(7)
    #define TOP_GPIO_CI2_OUT                    BIT(8)
    #define TOP_GPIO_CI3_OUT                    BIT(9)
    #define TOP_GPIO_CI4_OUT                    BIT(10)
    #define TOP_GPIO_CI5_OUT                    BIT(11)
    #define TOP_GPIO_CI6_OUT                    BIT(12)
    #define TOP_GPIO_CI7_OUT                    BIT(13)
    #define TOP_GPIO_CI8_OUT                    BIT(14)
    #define TOP_GPIO_CI9_OUT                    BIT(15)
*/

#define REG_TOP_GPIO_PWM_CI_OEN     0x003B
    #define TOP_GPIO_PWM0_DISABLE               BIT(4)
    #define TOP_GPIO_PWM1_DISABLE               BIT(5)
/*
    #define TOP_GPIO_CI0_DISABLE                BIT(6)
    #define TOP_GPIO_CI1_DISABLE                BIT(7)
    #define TOP_GPIO_CI2_DISABLE                BIT(8)
    #define TOP_GPIO_CI3_DISABLE                BIT(9)
    #define TOP_GPIO_CI4_DISABLE                BIT(10)
    #define TOP_GPIO_CI5_DISABLE                BIT(11)
    #define TOP_GPIO_CI6_DISABLE                BIT(12)
    #define TOP_GPIO_CI7_DISABLE                BIT(13)
    #define TOP_GPIO_CI8_DISABLE                BIT(14)
    #define TOP_GPIO_CI9_DISABLE                BIT(15)
*/

/*
#define REG_TOP_GPIO_S_IN           0x003C
#define REG_TOP_GPIO_S_OUT          0x003D
    #define TOP_GPIO_S0_OUT                     BIT(0)
    #define TOP_GPIO_S1_OUT                     BIT(1)
    #define TOP_GPIO_S2_OUT                     BIT(2)
    #define TOP_GPIO_S3_OUT                     BIT(3)
    #define TOP_GPIO_S4_OUT                     BIT(4)
    #define TOP_GPIO_S5_OUT                     BIT(5)
    #define TOP_GPIO_S6_OUT                     BIT(6)


#define REG_TOP_GPIO_S_OEN          0x003E
    #define TOP_GPIO_S0_DISABLE                 BIT(0)
    #define TOP_GPIO_S1_DISABLE                 BIT(1)
    #define TOP_GPIO_S2_DISABLE                 BIT(2)
    #define TOP_GPIO_S3_DISABLE                 BIT(3)
    #define TOP_GPIO_S4_DISABLE                 BIT(4)
    #define TOP_GPIO_S5_DISABLE                 BIT(5)
    #define TOP_GPIO_S6_DISABLE                 BIT(6)


#define REG_TOP_GPIO_AU_SPI_IN      0x003F
#define REG_TOP_GPIO_AU_SPI_OUT     0x0040
#define REG_TOP_GPIO_AU_SPI_OEN     0x0041
*/

#define REG_TOP_ET_PAD              0x0040
    #define TOP_ET_PAD1_USE_MASK                BMASK(2:0)
    #define TOP_ET_PAD1_USE_DEFAULT             BITS(2:0, 0)
    #define TOP_ET_PAD1_USE_ET_TX               BITS(2:0, 1)
    #define TOP_ET_PAD1_USE_SDIO                BITS(2:0, 2)
    #define TOP_ET_PAD1_USE_CCIR                BITS(2:0, 3)
    #define TOP_ET_PAD1_USE_TSI                 BITS(2:0, 4)
    #define TOP_ET_PAD2_USE_MASK                BMASK(4:3)
    #define TOP_ET_PAD2_USE_DEFAULT             BITS(4:3, 0)
    #define TOP_ET_PAD2_USE_ET_RX               BITS(4:3, 1)
    #define TOP_ET_PAD2_USE_ET_TXRX             BITS(4:3, 2)
    #define TOP_ET_PAD2_USE_CCIR                BITS(4:3, 3)
    #define TOP_GPIO_S_USE                      BIT(5)

#define REG_TOP_DUMMY_15_16         0x004B
    #define TOP_SM2_VCC_GPIO_USE                BIT(5)
    #define TOP_SM1_VCC_GPIO_USE                BIT(6)
    #define TOP_DUMMY_15_MASK                   BMASK(11:8)

/*
#define REG_TOP_DUMMY_18_21         0x0051
    #define TOP_PAD_TS_ERR_FLAG                 BIT(10)
*/

#define REG_TOP_DUMMY1              0x0053
    #define TOP_DUMMY1                          BIT(0)

/*
#define REG_TOP_GPIO_DI_DO          0x0054

#define REG_TOP_UART                0x0055
*/
#define REG_TOP_JTAG_AUD_CI_GPIO_PWM 0x0056
    #define TOP_PAD_AU1_MASK                    BMASK(5:4)
    #define TOP_PAD_AU1_GPIO                    0
    #define TOP_PAD_AU1_IICM0                   BITS(5:4, 1)
    #define TOP_PAD_AU1_IICM1                   BITS(5:4, 2)
    #define TOP_PAD_AU1_X                       BITS(5:4, 3)
    #define TOP_PAD_AU2_MASK                    BMASK(7:6)
    #define TOP_PAD_AU2_GPIO                    0
    #define TOP_PAD_AU2_PCM                     BMASK(7:6, 1)
    #define TOP_PAD_AU2_X                       BMASK(7:6, 2)
    #define TOP_PAD_GPIOW0_MASK                 BIT(9)
    #define TOP_PAD_GPIOW0_X                    0
    #define TOP_PAD_GPIOW0_CEC                  BIT(9)
    #define TOP_PAD_PWM_MASK                    BIT(10)
    #define TOP_PAD_PWM_X                       0
    #define TOP_PAD_PWM_GPIO                    BIT(10)

#define REG_TOP_STAT_BOND           0x0061
#define REG_TOP_BOND_OV_KEY         0x0063
#define REG_TOP_CHIP_CONFIG_STAT    0x0065
#define REG_TOP_DEVICE_ID           0x0066
#define REG_TOP_CHIP_VERSION        0x0067
    #define CHIP_VERSION_SHFT                   0
    #define CHIP_VERSION_MASK                   BMASK(7:0)
    #define CHIP_REVISION_SHFT                  8
    #define CHIP_REVISION_MASK                  BMASK(15:8)
#define REG_TOP_TESTBUS             0x0075
    #define TOP_TESTBUS_EN                      BIT(14)
#define REG_CLK_EJ_MIPS_TSJ         0x0076
    #define TOP_TESTCLK_OUT_MASK                BMASK(2:0)
    #define TOP_TESTCLK_OUT_NONE                BITS(2:0, 0)
    #define TOP_SM1_GPIO_USE                    BIT(3)
    #define TOP_SPI_GPIO_USE                    BIT(4)
#define REG_TOP_RESET_CPU0          0x0077
    #define TOP_RESET_PASSWD_MASK               BMASK(14:0)
    #define TOP_RESET_CPU0                      BIT(15)


#define REG_ECC_BASE                0xBF8069C0
#define ECC_REG(addr)               (*((volatile u32*)(REG_ECC_BASE + ((addr)<<2))))

#define REG_ECC_REQUEST             0x0001
    #define ECC_MCU_BURST_READ_EN               BIT(5)


#define REG_WDT_BASE                0xBF808400
#define WDT_REG(addr)               (*((volatile u32*)(REG_WDT_BASE + ((addr)<<2))))

#define REG_WDT_DISABLEWORD_L       0x0000
#define REG_WDT_DISABLEWORD_H       0x0001
#define REG_WDT_PROGRAMKEY_L        0x0002
#define REG_WDT_PROGRAMKEY_H        0x0003
#define REG_WDT_ENABLEMCURESET      0x0004
#define REG_WDT_CLEAR_STATUS        0x0010
#define REG_WDT_SETTIME             0x0012
#define REG_WDT_COUNTER_L           0x0014
#define REG_WDT_COUNTER_H           0x0015


#define REG_ANA_BASE                0xBF80A400
#define ANA_REG(addr)               (*((volatile u32*)(REG_ANA_BASE + ((addr)<<2))))

// 0x0006
    #define ANA_REF_TST_IRE                     BIT(0)
    #define ANA_REF_PWDN_ICAL                   BIT(1)
    #define ANA_REF_TST_BGO                     BIT(2)
    #define ANA_REF_TST_BGIE                    BIT(3)
    #define ANA_REF_VCLP                        BMASK(6:4)
    #define ANA_REF_TST_VRPM                    BIT(7)
    #define ANA_REF_TST_VCLPBW                  BMASK(9:8)
    #define ANA_REF_PWDN_CLP                    BIT(10)
    #define ANA_REF_TST_IRO                     BIT(11)
    #define ANA_REF_ICAL                        BMASK(15:12)

// 0x000C
    #define ANA_EXT_DAC                         BIT(0)
    #define ANA_CLK_POL                         BIT(1)
    #define ANA_DBG_SEL                         BMASK(3:2)
    #define ANA_SEL_TUAGC                       BMASK(9:8)
    #define ANA_SEL_IFAGC                       BMASK(11:10)
    #define ANA_AGC_PAD_OEN                     BIT(12)

// 0x000D
    #define ANA_ADC_OFFSET                      BMASK(11:0)
    #define ANA_ADC_RDSEL                       BIT(13)
    #define ANA_ADCIN_SIGN                      BIT(14)
    #define ANA_ADCD_FRZ                        BIT(15)

// 0x0010
    // reg_adcin_i read only register Debug Purpose to check ADC is Input or Not!
    // If ADC is input , reading this register will find it always changing !
    #define ANA_ADCIN_I                         BMASK(10:0)


#define REG_IRQ_BASE               0xBF800000+(0x1580<<2) // 0xBF805600
#define IRQ_REG(addr)               (*((volatile u16*)(REG_IRQ_BASE + ((addr)<<2))))

#define REG_FIQ_MASK_L              0x0000
#define REG_FIQ_MASK_H              0x0001
#define REG_FIQ_CLEAR_L             0x0004
#define REG_FIQ_CLEAR_H             0x0005
#define REG_FIQ_PENDING_L           0x0008
#define REG_FIQ_PENDING_H           0x0009
    //FIQ Low 16 bits
    #define FIQL_MASK                           0xFFFF //[15:0]
    //FIQ High 16 bits
    #define FIQH_MASK                           0xFFFF //[15:0]
    #define FIQH_DSP2UP                         (0x1 << (E_FIQ_DSP2UP       - E_FIQH_START)  )
    #define FIQH_VSYN_GOP1                      (0x1 << (E_FIQ_VSYN_GOP1    - E_FIQH_START)  )
    #define FIQH_VSYN_GOP0                      (0x1 << (E_FIQ_VSYN_GOP0    - E_FIQH_START)  )
    #define FIQH_IR                             (0x1 << (E_FIQ_IR           - E_FIQH_START)  )

#define REG_IRQ_MASK_L              0x0010
#define REG_IRQ_MASK_H              0x0011
#define REG_IRQ_PENDING_L           0x0016
#define REG_IRQ_PENDING_H           0x0017
#define ENABLE_USB_PORT0		//tony
    //IRQ Low 16 bits
    #define IRQL_MASK                           0xFFFF //[15:0]
    #define IRQL_USB                    		(0x1 << (E_IRQ_USB          - E_IRQL_START) )	//tony for OTG USB
#ifdef ENABLE_USB_PORT0	//tony	port0
    #define IRQL_UHC                            (0x1 << (E_IRQ_UHC          - E_IRQL_START) )
#endif
    #define IRQL_UART2                          (0x1 << (E_IRQ_UART2        - E_IRQL_START) )
    #define IRQL_UART1                          (0x1 << (E_IRQ_UART1        - E_IRQL_START) )
    #define IRQL_UART0                          (0x1 << (E_IRQ_UART0        - E_IRQL_START) )
    #define IRQL_EMAC                           (0x1 << (E_IRQ_EMAC         - E_IRQL_START) )
    #define IRQL_DEB                            (0x1 << (E_IRQ_DEB          - E_IRQL_START) )
    //IRQ High 16 bits
#ifndef ENABLE_USB_PORT0	//tony	port1
#define IRQH_UHC                    (0x1 << (E_IRQ_UHC          - E_IRQH_START) )	//tony for port1
#endif
    #define IRQH_MASK                           0xFFFF //[15:0]
    #define IRQH_TSP                            (0x1 << (E_IRQ_TSP          - E_IRQH_START) )
    #define IRQH_VE                             (0x1 << (E_IRQ_VE           - E_IRQH_START) )
    #define IRQH_CIMAX2MCU                      (0x1 << (E_IRQ_CIMAX2MCU    - E_IRQH_START) )
    #define IRQH_DC                             (0x1 << (E_IRQ_DC           - E_IRQH_START) )
    #define IRQH_GOP                            (0x1 << (E_IRQ_GOP          - E_IRQH_START) )
    #define IRQH_PCM2MCU                        (0x1 << (E_IRQ_PCM2MCU      - E_IRQH_START) )
    #define IRQH_IIC0                           (0x1 << (E_IRQ_IIC0         - E_IRQH_START) )
    #define IRQH_OTG                            (0x1 << (E_IRQ_OTG          - E_IRQH_START) )
    #define IRQH_RTC                            (0x1 << (E_IRQ_RTC          - E_IRQH_START) )
    #define IRQH_KEYPAD                         (0x1 << (E_IRQ_KEYPAD       - E_IRQH_START) )
    #define IRQH_PM                             (0x1 << (E_IRQ_PM           - E_IRQH_START) )
    #define IRQH_DMD                            (0x1 << (E_IRQ_DVBC         - E_IRQH_START) )
    #define IRQH_SVD                            (0x1 << (E_IRQ_SVD          - E_IRQH_START) )
    #define IRQH_JPD                            (0x1 << (E_IRQ_JPD          - E_IRQH_START) )
    #define IRQH_SDIO                           (0x1 << (E_IRQ_SDIO         - E_IRQH_START) )
    #define IRQH_HDMITX                         (0x1 << (E_IRQ_HDMITX       - E_IRQH_START) )
    #define IRQL_EMAC                           (0x1 << (E_IRQ_EMAC         - E_IRQL_START) )


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_SYSTEM_H_


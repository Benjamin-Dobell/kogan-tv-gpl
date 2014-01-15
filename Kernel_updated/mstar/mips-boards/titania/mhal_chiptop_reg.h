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
/// file    mhal_chiptop_reg.h
/// @brief  Chip Top Registers Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_CHIPTOP_REG_H_
#define _MHAL_CHIPTOP_REG_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define BIT(x)                      (1<<(x))
#define BMASK(bits)                 (BIT(((1)?bits)+1)-BIT(((0)?bits)))
#define BITS(bits,value)            ((BIT(((1)?bits)+1)-BIT(((0)?bits))) & (value<<((0)?bits)))


#define REG_TOP_BASE                0xBF803C00
#define TOP_REG(addr)               (*((volatile u32*)(REG_TOP_BASE + ((addr)<<2))))


#define REG_TOP_CHIP_CONFIG_OW      0x0000
#define REG_TOP_UTMI_UART_SETHL     0x0001
#define REG_TOP_TEST_IN_OUT         0x0002
    #define TOP_TEST_IN_MASK                    BMASK(5:4)
    #define TOP_TEST_IN_TS0                     BITS(5:4, 1)
    #define TOP_TEST_IN_PCI                     BITS(5:4, 2)
    #define TOP_TEST_IN_I2S                     BITS(5:4, 3)
    #define TOP_TEST_OUT_MASK                   BMASK(7:6)
#define REG_TOP_PWM                 0x0003
#define REG_TOP_PCI                 0x0004
/*#define REG_TOP_SPI_EMAC            0x0005
    #define TOP_CKG_EMACRX_MASK                 BMASK(12:10)
    #define TOP_CKG_EMACRX_DIS                  BIT(10)
    #define TOP_CKG_EMACRX_INV                  BIT(11)
    #define TOP_CKG_EMACTX_MASK                 BMASK(15:13)
    #define TOP_CKG_EMACTX_DIS                  BIT(13)
    #define TOP_CKG_EMACTX_INV                  BIT(14)
#define REG_TOP_CR_CI_GPIO          0x0006
    #define TOP_PAD_CI_ADHI_MASK                BMASK(2:1)
    #define TOP_PAD_CI_ADHI_CCIR                BITS(2:1, 1)            // GPIO_CI5
    #define TOP_PAD_CI_ADHI_CI                  BITS(2:1, 3)
    #define TOP_PAD_CI_DATA_MASK                BMASK(4:3)
    #define TOP_PAD_CI_DATA_TSI                 BITS(4:3, 1)            // GPIO_CI6_7
    #define TOP_PAD_CI_DATA_CI                  BITS(4:3, 2)
    #define TOP_PAD_GPIO_S_MASK                 BIT(5)
    #define TOP_PAD_GPIO_S_X                    0
    #define TOP_PAD_GPIO_S_SM2                  BIT(5)
    #define TOP_PAD_CI_ADLO_MASK                BMASK(11:10)
    #define TOP_PAD_CI_ADLO_SM2                 BITS(11:10, 1)
    #define TOP_PAD_CI_ADLO_GPIO                BITS(11:10, 2)          // GPIO_CI0_4

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
    #define TOP_CKG_DVBC_INN_SRC_MASK           BIT(11:10)              // 3:DFT_Live, other:clk_31
    #define TOP_CKG_DVBC_INN_SRC_31             BITS(11:10, 0)
    #define TOP_CKG_DVBC_INN_SRC_DFT            BITS(11:10, 3)
    #define TOP_CKG_DVBC_INN2X_SHFT             12
    #define TOP_CKG_DVBC_INN2X_MASK             BMASK(15:12)
    #define TOP_CKG_DVBC_INN2X_DIS              BIT(12)
    #define TOP_CKG_DVBC_INN2X_INV              BIT(13)
    #define TOP_CKG_DVBC_INN2X_SRC              BIT(15:14)              // 3:DFT_Live, other:clk_62
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
    #define TOP_CLK_CCIR_INV                    BIT(14)

#define REG_TOP_TSOUT_DISP          0x000A
    #define TOP_PCI_RPUPI                       BMASK(9:0)
    #define TOP_CLK_TSOUT_INV                   BIT(10)
    #define TOP_CKG_DISP                        BIT(15:12)
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
    #define TOP_CKG_DVBC_TS_SRC_36              BITS(4:2, 3)
    #define TOP_CKG_DVBC_TS_SRC_18              BITS(4:2, 4)
    #define TOP_CKG_DVBC_TS_SRC_13P5            BITS(4:2, 5)
    #define TOP_CKG_DVBC_TS_SRC_9               BITS(4:2, 6)
    #define TOP_CKG_DVBC_TS_SRC_DFT             BITS(4:2, 7)
    #define TOP_PAD_HDMI_HP_MASK                BIT(5)
    #define TOP_PAD_HDMI_HP_GPIO                BIT(5)
    #define TOP_PAD_HDMI_DDC_MASK               BIT(7:6)
    #define TOP_PAD_HDMI_DDC_X                  BITS(7:6, 0)
    #define TOP_PAD_HDMI_DDC_GPIO               BITS(7:6, 1)
    #define TOP_PAD_HDMI_DDC_IICM1              BITS(7:6, 2)
    #define TOP_PAD_HDMI_CCIR_MASK              BIT(8)
    #define TOP_PAD_HDMI_CCIR_GPIO              0
    #define TOP_PAD_HDMI_CCIR_X                 BIT(8)
    #define TOP_PAD_CI_CTL_MASK                 BIT(9)
    #define TOP_PAD_CI_CTL_X                    0
    #define TOP_PAD_CI_CTL_GPIO                 BIT(9)                  // GPIO_CI8_9
#define REG_TOP_GPIO_HM_OEN         0x000C
#define REG_TOP_PCI_RPD             0x000D
#define REG_TOP_GPIO_HM_IN          0x000E
#define REG_TOP_GPIO_HM_OUT         0x000F */
#define REG_TOP_POFF_MCU            0x0010
#define REG_TOP_MCU_USB_STC0        0x0011
    #define TOP_MCU_CLK_MASK                    BMASK(0:0)
    #define TOP_MCU_CLK_DFT                     BITS(0:0, 0)
    #define TOP_MCU_CLK_SRC_MCUCLK              BITS(0:0, 1)
    #define TSP_STC0_SEL                        BIT(7)
#define REG_TOP_USB_RIU_DDR_MIU     0x0012
    #define TOP_CKG_USB_MASK                    BMASK(1:0)
    #define TOP_CKG_USB_DIS                     BIT(0)
    #define TOP_CKG_USB_INV                     BIT(1)
#define REG_TOP_TS0_TSP_STC0        0x0013
    #define TOP_CKG_TS0_MASK                    BMASK(3:0)
    #define TOP_CKG_TS0_DIS                     BIT(0)
    #define TOP_CKG_TS0_INV                     BIT(1)
    #define TOP_CKG_TS0_SRC_MASK                BMASK(3:2)
    #define TOP_CKG_TS0_SRC_TS0                 BITS(3:2, 0)
    #define TOP_CKG_TS0_SRC_TS1                 BITS(3:2, 1)
    #define TOP_CKG_TS0_SRC_XTAL                BITS(3:2, 3)
    //#define TOP_CKG_TS0_SRC_DEMOD               TOP_CKG_TS0_SRC_TS0
    //#define TOP_CKG_TS0_SRC_CI                  TOP_CKG_TS0_SRC_TS1
    #define TOP_CKG_TCK_CLOCK                   BIT(4)
    #define TOP_CKG_AEON_CLOCK                  BIT(6)
    #define TOP_CKG_TSP_MASK                    BMASK(11:8)
    #define TOP_CKG_TSP_DIS                     BIT(8)
    #define TOP_CKG_TSP_INV                     BIT(9)
    #define TOP_CKG_TSP_SRC_144                 BITS(11:10, 0)
    #define TOP_CKG_TSP_SRC_123                 BITS(11:10, 1)
    #define TOP_CKG_TSP_SRC_72                  BITS(11:10, 2)
    #define TOP_CKG_TSP_SRC_DFT                 BITS(11:10, 3)
    #define TOP_CKG_STC0_MASK                   BMASK(15:12)
    #define TOP_CKG_STC0_DIS                    BIT(12)
    #define TOP_CKG_STC0_INV                    BIT(13)
    #define TOP_CKG_STC0_SRC_STC0               BITS(15:14, 0)
    #define TOP_CKG_STC0_SRC_1                  BITS(15:14, 1)
    #define TOP_CKG_STC0_SRC_27                 BITS(15:14, 2)
    #define TOP_CKG_STC0_SRC_XTAL               BITS(15:14, 3)
#define REG_TOP_MAD_MVD             0x0014
    #define TOP_CKG_VIF129M_MASK                BMASK(2:0)
    #define TOP_CKG_VIF129M_DIS                 BIT(0)
    #define TOP_CKG_VIF129M_INV                 BIT(1)
    #define TOP_CKG_VIF129M_129M                BIT(2)
    #define TOP_CKG_MAD_STC_MASK                BMASK(7:4)
    #define TOP_CKG_MAD_STC_DIS                 BIT(4)
    #define TOP_CKG_MAD_STC_INV                 BIT(5)
    #define TOP_CKG_MAD_STC_SRC_STC0            BITS(7:6, 0)
    #define TOP_CKG_MAD_STC_SRC_27              BITS(7:6, 2)
    #define TOP_CKG_MAD_STC_SRC_XTAL             BITS(7:6, 3)
    #define TOP_CKG_MVD_BOOT_MASK               BMASK(11:8)
    #define TOP_CKG_MVD_BOOT_DIS                BIT(8)
    #define TOP_CKG_MVD_BOOT_INV                BIT(9)
    #define TOP_CKG_MVD_BOOT_SRC_144            BITS(11:10, 0)
    #define TOP_CKG_MVD_BOOT_SRC_123            BITS(11:10, 1)
    #define TOP_CKG_MVD_BOOT_SRC_MIU            BITS(11:10, 2)
    #define TOP_CKG_MVD_BOOT_SRC_XTAL            BITS(11:10, 3)
    #define TOP_CKG_MVD_MASK                    BMASK(15:12)
    #define TOP_CKG_MVD_DIS                     BIT(12)
    #define TOP_CKG_MVD_INV                     BIT(13)
    #define TOP_CKG_MVD_SRC_144                 BITS(15:14, 0)
    #define TOP_CKG_MVD_SRC_123                 BITS(15:14, 1)
    #define TOP_CKG_MVD_SRC_MIU                 BITS(15:14, 2)
    #define TOP_CKG_MVD_SRC_XTAL                 BITS(15:14, 3)
#define REG_TOP_M4VD_DC0_DHC_SBM_GE        0x0015  //TODO
    #define TOP_CKG_M4VD_MASK                   BMASK(3:0)
    #define TOP_CKG_M4VD_DIS                    BIT(0)
    #define TOP_CKG_M4VD_INV                    BIT(1)
    #define TOP_CKG_M4VD_SRC_144                BITS(3:2, 0)
    #define TOP_CKG_M4VD_SRC_123                BITS(3:2, 1)
    #define TOP_CKG_M4VD_SRC_108                BITS(3:2, 2)
    #define TOP_CKG_M4VD_SRC_XTAL               BITS(3:2, 3)

    #define TOP_CKG_GE_MASK                     BMASK(15:12)
    #define TOP_CKG_GE_DIS                      BIT(12)
    #define TOP_CKG_GE_INV                      BIT(13)
    #define TOP_CKG_GE_SRC_123                  BITS(15:12, 0)
    #define TOP_CKG_GE_SRC_108                  BITS(15:12, 1)
    #define TOP_CKG_GE_SRC_86                   BITS(15:12, 2)
    #define TOP_CKG_GE_SRC_XTAL                  BITS(15:12, 3)
#define REG_TOP_GOP                 0x0016
    #define TOP_CKG_GOP0_MASK                   BMASK(3:0)
    #define TOP_CKG_GOP0_DIS                    BIT(0)
    #define TOP_CKG_GOP0_INV                    BIT(1)
    #define TOP_CKG_GOP0_SRC_DISP               BITS(3:2, 0)
    #define TOP_CKG_GOP0_SRC_DFT                BITS(3:2, 3)
    #define TOP_CKG_GOP1_MASK                   BMASK(7:4)
    #define TOP_CKG_GOP1_DIS                    BIT(4)
    #define TOP_CKG_GOP1_INV                    BIT(5)
    #define TOP_CKG_GOP1_SRC_DISP               BITS(7:6, 0)
    #define TOP_CKG_GOP1_SRC_DFT                BITS(7:6, 3)
    #define TOP_CKG_GOPD_MASK                   BMASK(11:8)
    #define TOP_CKG_GOPD_DIS                    BIT(8)
    #define TOP_CKG_GOPD_INV                    BIT(9)
    #define TOP_CKG_GOPD_SRC_ADC                BITS(11:10, 0)
    #define TOP_CKG_GOPD_SRC_DISP               BITS(11:10, 1)
    #define TOP_CKG_GOPD_SRC_DFT                BITS(11:10, 3)
//#define REG_TOP_GOP_UART            0x0017
//    #define TOP_CKG_GOPG2_MASK                  BMASK(11:8)
//    #define TOP_CKG_GOPG2_DIS                   BIT(8)
//    #define TOP_CKG_GOPG2_INV                   BIT(9)
//    #define TOP_CKG_GOPG2_SRC_DISP              BITS(11:10, 0)
//    #define TOP_CKG_GOPG2_SRC_DFT               BITS(11:10, 3)
//    #define TOP_CKG_UART_MASK                   BMASK(14:12)
//    #define TOP_CKG_UART_DIS                    BIT(12)
//    #define TOP_CKG_UART_INV                    BIT(13)
//    #define TOP_CKG_UART_SRC_UART               BITS(14:14, 0)
//    #define TOP_CKG_UART_SRC_DFT                BITS(14:14, 1)
#define REG_TOP_VDMCU_VD200_DHC       0x0017
	#define TOP_CKG_VDMCU_MASK                  BMASK(3:0)
	#define TOP_CKG_VDMCU_DIS					BIT(0)
	#define TOP_CKG_VDMCU_INV                   BIT(1)
	#define TOP_CKG_VDMCU_108M                  BITS(3:2,0)
	#define TOP_CKG_VDMCU_86M                   BITS(3:2,1)
	#define TOP_CKG_VDMCU_54M                   BITS(3:2,2)
	#define TOP_CKG_VDMCU_XTAL                  BITS(3:2,3)
	#define TOP_CKG_VD200_MASK                  BMASK(7:4)
	#define TOP_CKG_VD200_DIS                   BIT(4)
	#define TOP_CKG_VD200_INV					BIT(5)
	#define TOP_CKG_VD200_216M                  BITS(7:6,0)
	#define TOP_CKG_VD200_XTAL                  BITS(7:6,3)
	#define TOP_CKG_DHC_MASK                    BMASK(13:8)
	#define TOP_CKG_DHC_DIS                     BIT(8)
	#define TOP_CKG_DHC_INV                     BIT(9)
	#define TOP_CKG_DHC_12M						BITS(13:10,0)
	#define TOP_CKG_DHC_54M						BITS(13:10,1)
	#define TOP_CKG_DHC_62M						BITS(13:10,2)
	#define TOP_CKG_DHC_72M						BITS(13:10,3)
	#define TOP_CKG_DHC_86M						BITS(13:10,4)
	#define TOP_CKG_DHC_108M					BITS(13:10,5)
	#define TOP_CKG_DHC_XTAL					BITS(13:10,8)

#define REG_TOP_SM_PCM              0x0018
    #define TOP_CKG_FICLK_F2_MASK               BMASK(7:4)
    #define TOP_CKG_FICLK_F2_DIS                BIT(4)
    #define TOP_CKG_FICLK_F2_INV                BIT(5)
    #define TOP_CKG_FICLK_F2_IDCLK2             BITS(7:6,0)
    #define TOP_CKG_FICLK_F2_FCLK               BITS(7:6,1)
    #define TOP_CKG_FICLK_F2_XTAL               BITS(7:6,3)
    #define TOP_CKG_GOPG2_MASK                  BMASK(11:8)
    #define TOP_CKG_GOPG2_DIS                   BIT(8)
    #define TOP_CKG_GOPG2_INV                   BIT(9)
    #define TOP_CKG_GOPG2_ODCLK                 BITS(11:10, 0)
    #define TOP_CKG_GOPG2_IDCLK2                BITS(11:10, 2)
    #define TOP_CKG_GOPG2_SRC_XTAL              BITS(11:10, 3)
//    #define TOP_CKG_SM_MASK                     BMASK(11:8)
//    #define TOP_CKG_SM_DIS                      BIT(8)
//    #define TOP_CKG_SM_INV                      BIT(9)
//    #define TOP_CKG_SM_SRC_6                    BITS(11:10, 0)
//    #define TOP_CKG_SM_SRC_4P5                  BITS(11:10, 1)
//    #define TOP_CKG_SM_SRC_3                    BITS(11:10, 2)
//    #define TOP_CKG_SM_SRC_DFT                  BITS(11:10, 3)
    #define TOP_CKG_PCM_MASK                    BMASK(15:12)
    #define TOP_CKG_PCM_DIS                     BIT(12)
    #define TOP_CKG_PCM_INV                     BIT(13)
    #define TOP_CKG_PCM_SRC_27                  BITS(15:14, 0)
    #define TOP_CKG_PCM_SRC_DFT                 BITS(15:14, 2)
#define REG_TOP_VE                  0x0019
    #define TOP_CKG_VE_MASK                     BMASK(11:8)
    #define TOP_CKG_VE_DIS                      BIT(8)
    #define TOP_CKG_VE_INV                      BIT(9)
    #define TOP_CKG_VE_27M                      BITS(11:10,0)
    #define TOP_CKG_VE_XTAL                     BITS(11:10,2)
    //[ERROR] bits error
    #define TOP_CKG_VEDAC_MASK                  BMASK(15:12)
    #define TOP_CKG_VEDAC_DIS                   BIT(12)
    #define TOP_CKG_VEDAC_INV                   BIT(13)
    #define TOP_CKG_VEDAC_SRC_27                BITS(15:14, 0)
    #define TOP_CKG_VEDAC_SRC_54                BITS(15:14, 1)
    #define TOP_CKG_VEDAC_SRC_108               BITS(15:14, 2)
    #define TOP_CKG_VEDAC_SRC_DFT               BITS(15:14, 3)
#define REG_TOP_DAC_SDR             0x001A
    #define TOP_CKG_DAC_MASK                    BMASK(3:0)
    #define TOP_CKG_DAC_DIS                     BIT(0)
    #define TOP_CKG_DAC_INV                     BIT(1)
    #define TOP_CKG_DAC_SRC_ODC                 BITS(3:2, 0)            //[???]
    #define TOP_CKG_DAC_SRC_VEDAC               BITS(3:2, 1)
    #define TOP_CKG_DAC_SRC_VD_SIF              BITS(3:2, 2)
    #define TOP_CKG_DAC_SRC_XTAL                BITS(3:2, 3)
    #define TOP_CKG_FCLK_MASK                   BMASK(13:8)
    #define TOP_CKG_FCLK_DIS                    BIT(8)
    #define TOP_CKG_FCLK_INV                    BIT(9)
    #define TOP_CKG_FCLK_MPLL                   BITS(13:10,0)
    #define TOP_CKG_FCLK_MIU                    BITS(13:10,1)
    #define TOP_CKG_FCLK_ODCLK                  BITS(13:10,2)
    #define TOP_CKG_FCLK_216M                   BITS(13:10,3)
    #define TOP_CKG_FCLK_IDCLK2                 BITS(13:10,4)
    #define TOP_CKG_FCLK_PLL_CLK_OUT            BITS(13:10,5)
    #define TOP_CKG_FCLK_XTAL                   BITS(13:10,8)

#define REG_TOP_MIPS_ODCLK          0x001B
	#define TOP_CKG_MIPS_MASK                   BMASK(4:0)
	#define TOP_CKG_MIPS_DIS                    BIT(0)
	#define TOP_CKG_MIPS_INV                    BIT(1)
	#define TOP_CKG_MIPS_216M                   BITS(4:2,0)
	#define TOP_CKG_MIPS_MPLL                   BITS(4:2,1)
	#define TOP_CKG_MIPS_170M                   BITS(4:2,2)
	#define TOP_CKG_MIPS_160M                   BITS(4:2,3)
	#define TOP_CKG_MIPS_144M                   BITS(4:2,4)
	#define TOP_CKG_MIPS_123M                   BITS(4:2,5)
	#define TOP_CKG_MIPS_108M                   BITS(4:2,6)
	#define TOP_CKG_MIPS_200M                   BITS(4:2,7)
	#define TOP_CKG_SW_MIPS_CLK                 BIT(5)
	#define TOP_CKG_ODCLK_MASK                  BMASK(13:8)
	#define TOP_CKG_ODCLK_DIS                   BIT(8)
	#define TOP_CKG_ODCLK_INV                   BIT(9)
	#define TOP_CKG_ODCLK_ADC                   BITS(13:10,0)
	#define TOP_CKG_ODCLK_DVI                   BITS(13:10,1)
	#define TOP_CKG_ODCLK_VD                    BITS(13:10,2)
	#define TOP_CKG_ODCLK_MPEG0                 BITS(13:10,3)
	#define TOP_CKG_ODCLK_DI                    BITS(13:10,5)
	#define TOP_CKG_ODCLK_LPLL                  BITS(13:10,7)
	#define TOP_CKG_MIPS_XTAL                   BITS(13:10,8)


#define REG_TOP_VE_NFIE             0x001C
    #define TOP_CKG_VEIN_MASK                   BMASK(5:0)
    #define TOP_CKG_VEIN_DIS                    BIT(0)
    #define TOP_CKG_VEIN_INV                    BIT(1)
    #define TOP_CKG_VEIN_SRC_ADC                BITS(5:2, 0)
    #define TOP_CKG_VEIN_SRC_DFT                BITS(5:2, 8)
    #define TOP_CKG_NFIE_MASK                   BMASK(12:8)
    #define TOP_CKG_NFIE_DIS                    BIT(8)
    #define TOP_CKG_NFIE_INV                    BIT(9)
    #define TOP_CKG_NFIE_62M                    BITS(12:10,0)
    #define TOP_CKG_NFIE_54M                    BITS(12:10,1)
    #define TOP_CKG_NFIE_43M                    BITS(12:10,2)
    #define TOP_CKG_NFIE_36M                    BITS(12:10,3)
    #define TOP_CKG_NFIE_27M                    BITS(12:10,4)
    #define TOP_CKG_NFIE_18M                    BITS(12:10,5)
    #define TOP_CKG_NFIE_13M                    BITS(12:10,6)
    #define TOP_CKG_NFIE_XTAL                   BITS(12:10,7)

//    #define TOP_CKG_FCIE_MASK                   BMASK(14:8)
//    #define TOP_CKG_FCIE_DIS                    BIT(8)
//    #define TOP_CKG_FCIE_INV                    BIT(9)
//    #define TOP_CKG_FCIE_SRC_86D256             BITS(14:10, 0)
//    #define TOP_CKG_FCIE_SRC_86D64              BITS(14:10, 1)
//    #define TOP_CKG_FCIE_SRC_86D16              BITS(14:10, 2)
//    #define TOP_CKG_FCIE_SRC_54D4               BITS(14:10, 3)
//    #define TOP_CKG_FCIE_SRC_72D4               BITS(14:10, 4)
//    #define TOP_CKG_FCIE_SRC_86D4               BITS(14:10, 5)
//    #define TOP_CKG_FCIE_SRC_54D2               BITS(14:10, 6)
//    #define TOP_CKG_FCIE_SRC_72D2               BITS(14:10, 7)
//    #define TOP_CKG_FCIE_SRC_86D2               BITS(14:10, 8)
//    #define TOP_CKG_FCIE_SRC_54                 BITS(14:10, 9)
//    #define TOP_CKG_FCIE_SRC_72                 BITS(14:10,10)
//    #define TOP_CKG_FCIE_SRC_DFT                BITS(14:10,16)

#define REG_TOP_TS2_TSOUT          0x001D
	#define TOP_CKG_TS2_MASK                    BMASK(3:0)
	#define TOP_CKG_TS2_DIS                     BIT(0)
	#define TOP_CKG_TS2_INV                     BIT(1)
	#define TOP_CKG_TS2_TS2CLK                  BITS(3:2,0)
	#define TOP_CKG_TS2_XTAL                    BITS(3:2,3)
	#define TOP_CKG_TSOUT_MASK                  BMASK(7:4)
	#define TOP_CKG_TSOUT_DIS                   BIT(4)
	#define TOP_CKG_TSOUT_INV                   BIT(5)
	#define TOP_CKG_TSOUT_27M                   BITS(7:6,0)
	#define TOP_CKG_TSOUT_36M                   BITS(7:6,1)
	#define TOP_CKG_TSOUT_43M                   BITS(7:6,2)
	#define TOP_CKG_TSOUT_XTAL                  BITS(7:6,3)

#define REG_TOP_UART_DC1_VIF43M     0x001E   //TODO
	#define TOP_CKG_UART_CLOCK_MASK             BMASK(7:4)
	#define TOP_CKG_UART_CLOCK_DIS              BIT(4)
	#define TOP_CKG_UART_CLOCK_INV              BIT(5)

#define REG_TOP_IDCLK1_IDCLK2       0x001F
	#define TOP_CKG_IDCLK1_MASK                 BMASK(5:0)
	#define TOP_CKG_IDCLK1_DIS                  BIT(0)
	#define TOP_CKG_IDCLK1_INV                  BIT(1)
	#define TOP_CKG_IDCLK1_ADC                  BITS(5:2,0)
	#define TOP_CKG_IDCLK1_DVI                  BITS(5:2,1)
	#define TOP_CKG_IDCLK1_VD                   BITS(5:2,2)
	#define TOP_CKG_IDCLK1_DC0                  BITS(5:2,3)
	#define TOP_CKG_IDCLK1_DI                   BITS(5:2,5)
	#define TOP_CKG_IDCLK1_VDADC                BITS(5:2,6)
	#define TOP_CKG_IDCLK1_XTAL                 BITS(5:2,8)
	#define TOP_CKG_IDCLK2_MASK                 BMASK(13:8)
	#define TOP_CKG_IDCLK2_DIS                  BIT(8)
	#define TOP_CKG_IDCLK2_INV                  BIT(9)
	#define TOP_CKG_IDCLK2_ADC                  BITS(13:10,0)
	#define TOP_CKG_IDCLK2_DVI                  BITS(13:10,1)
	#define TOP_CKG_IDCLK2_VD                   BITS(13:10,2)
	#define TOP_CKG_IDCLK2_DC0                  BITS(13:10,3)
	#define TOP_CKG_IDCLK2_DI                   BITS(13:10,5)
	#define TOP_CKG_IDCLK2_VDADC                BITS(13:10,6)
	#define TOP_CKG_IDCLK2_XTAL                 BITS(13:10,8)


#define REG_TOP_DC0_NUM             0x0020                              //[???]
#define REG_TOP_DC0_DEN             0x0021                              //[???]
#define REG_TOP_MCU                 0x0022

    #define TOP_CKG_STRLD_MASK                  BMASK(3:0)
    #define TOP_CKG_STRLD_DIS                   BIT(0)
    #define TOP_CKG_STRLD_INV                   BIT(1)
    #define TOP_CKG_STRLD_144M                  BITS(3:2,0)
    #define TOP_CKG_STRLD_123M                  BITS(3:2,1)
    #define TOP_CKG_STRLD_108M                  BITS(3:2,2)
    #define TOP_CKG_STRLD_XTAL                  BITS(3:2,3)


    #define TOP_CKG_MCU_MASK                    BMASK(12:8)
    #define TOP_CKG_MCU_DIS                     BIT(8)
    #define TOP_CKG_MCU_INV                     BIT(9)
    #define TOP_CKG_MCU_SRC_MASK                BMASK(12:10)
    #define TOP_CKG_MCU_SRC_170M                 BITS(12:10, 0)
    #define TOP_CKG_MCU_SRC_160M                BITS(12:10, 1)
    #define TOP_CKG_MCU_SRC_144M                 BITS(12:10, 2)
    #define TOP_CKG_MCU_SRC_123M                 BITS(12:10, 3)
    #define TOP_CKG_MCU_SRC_108M                 BITS(12:10, 4)
    #define TOP_CKG_MCU_SRC_MEM                 BITS(12:10, 5)
    #define TOP_CKG_MCU_SRC_MEM_DIV2                 BITS(12:10, 6)
    //#define TOP_CKG_MCU_SRC_DFT                 BITS(12:10, 7)
#define REG_TOP_DC0_MIU_UART        0x0023
	#define TOP_CKG_DC0_4                       BIT(0)
	#define TOP_CKG_MIU_4                       BIT(1)
	#define TOP_CKG_UART_4                      BIT(2)
	#define TOP_CKG_UART_CLK                    BIT(3)
#define REG_TOP_USB_CW_L            0x0024
#define REG_TOP_USB_CW_H            0x0025
#define REG_TOP_STC0_CW_L           0x0026
#define REG_TOP_STC0_CW_H           0x0027
#define REG_TOP_DC0_CW_L            0x002A                              //[???]
#define REG_TOP_DC0_CW_H            0x002B                              //[???]

#define REG_TOP_DHC                 0x002C
	#define TOP_CKG_DHC_DDR_MASK                BMASK(1:0)
	#define TOP_CKG_DHC_DDR_GAT                 BIT(0)
	#define TOP_CKG_DHC_DDR_INV                 BIT(1)
	#define TOP_CKG_DHC_SYNTH_MASK              BMASK(5:2)
	#define TOP_CKG_DHC_SYNTH_GAT               BIT(2)
	#define TOP_CKG_DHC_SYNTH_INV               BIT(3)
	#define TOP_CKG_DHC_SYNTH_VCO_DIV2          BITS(5:4,0)
	#define TOP_CKG_DHC_SYNTH_VCO_DIV2P5        BITS(5:4,1)
	#define TOP_CKG_DHC_SYNTH_VCO_DIV3          BITS(5:4,2)
	#define TOP_CKG_DHC_SYNTH_VCO_DIV4			BITS(5:4,3)
	#define TOP_CKG_DHC_MCU_MASK                BMASK(13:8)
	#define TOP_CKG_DHC_MCU_GAT                 BIT(8)
	#define TOP_CKG_DHC_MCU_INV                 BIT(9)
	#define TOP_CKG_DHC_MCU_XTAL                BITS(13:10,0)
	#define TOP_CKG_DHC_MCU_43M                 BITS(13:10,1)
	#define TOP_CKG_DHC_MCU_54M                 BITS(13:10,2)
	#define TOP_CKG_DHC_MCU_62M                 BITS(13:10,3)
	#define TOP_CKG_DHC_MCU_72M                 BITS(13:10,4)
	#define TOP_CKG_DHC_MCU_86M                 BITS(13:10,5)
	#define TOP_CKG_DHC_MCU_108M                BITS(13:10,6)
	#define TOP_CKG_DHC_LIVE                    BMASK(15:14)
	#define TOP_CKG_DHC_LIVE_GAT                BIT(14)
	#define TOP_CKG_DHC_LIVE_INV                BIT(15)

#define REG_TOP_PSRAM_GOPTGO_MLOAD         0x002D   //TODO
    #define TOP_CKG_PSRAM_MASK                  BMASK(1:0)
    #define TOP_CKG_PSRAM_DIS                   BIT(0)
    #define TOP_CKG_PSRAM_INV                   BIT(1)
    #define TOP_CKG_GOPTGO_MASK                 BMASK(11:8)
    #define TOP_CKG_GOPTGO_DIS                  BIT(8)
    #define TOP_CKG_GOPTGO_INV                  BIT(9)
    #define TOP_CKG_GOPTGO_54M                  BITS(11:10,0)
    #define TOP_CKG_GOPTGO_72M                  BITS(11:10,1)
    #define TOP_CKG_GOPTGO_86M                  BITS(11:10,2)
    #define TOP_CKG_GOPTGO_108M                 BITS(11:10,3)
    #define TOP_CKG_MLOAD_MASK                  BMASK(14:12)
    #define TOP_CKG_MLOAD_DIS                   BIT(12)
    #define TOP_CKG_MLOAD_INV                   BIT(13)
    #define TOP_CKG_MLOAD_SEL_SRAM_CLK_MIU


#define REG_TOP_GPIO_OEN            0x0030
#define REG_TOP_TS0_TS1_GPIO_IN     0x0031
	#define TOP_CKG_TS0_GPIO_IN                  BMASK(10:0)
	#define TOP_CKG_TS1_GPIO_IN                  BMASK(15:12)
#define REG_TOP_TS0_GPIO_OUT        0x0032
#define REG_TOP_TS1_GPIO_OUT        0x0033
#define REG_TOP_TS0_TS1_GPIO_OEN    0x0034
	#define TOP_CKG_TS0_GPIO_OEN                 BMASK(10:0)
	#define TOP_CKG_TS1_GPIO_OEN                 BMASK(15:12)
#define REG_TOP_I2S_SPI_GPIO_IN     0x0035
	#define TOP_CKG_I2S_GPIO_IN                  BMASK(8:0)
	#define TOP_CKG_SPI_GPIO_IN                  BMASK(15:12)
#define REG_TOP_I2S_SPI_GPIO_OUT     0x0036
	#define TOP_CKG_I2S_GPIO_OUT                  BMASK(8:0)
	#define TOP_CKG_SPI_GPIO_OUT                  BMASK(15:12)

#define REG_TOP_I2S_SPI_GPIO_OEN     0x0037
	#define TOP_CKG_I2S_GPIO_OEN                  BMASK(8:0)
	#define TOP_CKG_SPI_GPIO_OEN                  BMASK(15:12)

#define REG_TOP_PCM_GPIO_IN_LO       0x0038
#define REG_TOP_PCM_GPIO_IN_MID      0x0039
#define REG_TOP_PCM_GPIO_IN_HI_PCM2_GPIO_IN         0x003A
    #define TOP_CKG_PCM_GPIO_IN_HI                BIT(0)
	#define TOP_CKG_PCM2_GPIO_IN                  BMASK(12:8)
#define REG_TOP_PCM_GPIO_OUT_LO      0x003B
#define REG_TOP_PCM_GPIO_OUT_MID      0x003C
#define REG_TOP_PCM_GPIO_OUT_HI_PCM2_GPIO_OUT        0x003D
    #define TOP_CKG_PCM_GPIO_OUT                   BIT(0)
	#define TOP_CKG_PCM2_GPIO_OUT                  BMASK(12:8)

#define REG_TOP_PCM_GPIO_OEN_LO      0x003E
#define REG_TOP_PCM_GPIO_OEN_MID      0x003F
#define REG_TOP_PCM_GPIO_OEN_HI_PCM2_GPIO_OEN        0x0040
    #define TOP_CKG_PCM_GPIO_OEN_HI                BIT(0)
	#define TOP_CKG_PCM2_GPIO_OEN                  BMASK(12:8)

#define REG_TOP_ET_GPIO_IN_LO           0x0041
#define REG_TOP_ET_GPIO_IN_HI           0x0042
#define REG_TOP_ET_GPIO_OUT_LO          0x0043
#define REG_TOP_ET_GPIO_OUT_HI_GPIO_OEN_LOW          0x0044
	#define TOP_CKG_GPIO_OUT_HI                    BIT(1:0)
	#define TOP_CKG_GPIO_OEN_LOW                   BMASK(15:8)
#define REG_TOP_ET_GPIO_OEN_HI          0x0045
#define REG_TOP_PF_GPIO_IN_LO           0x0046
#define REG_TOP_PF_GPIO_IN_HI           0x0047
#define REG_TOP_PF_GPIO_OUT_LO          0x0048
#define REG_TOP_PF_GPIO_OUT_HI          0x0049
#define REG_TOP_PF_GPIO_OEN_LO          0x004a
#define REG_TOP_PF_GPIO_OEN_HI_MLINK          0x004b
	#define TOP_CKG_GPIO_OEN_HI                   BMASK(4:0)
	#define TOP_CKG_MLINK_MASK                    BMASK(15:12)
	#define TOP_CKG_MLINK_DIS                     BIT(12)
	#define TOP_CKG_MLINK_INV                     BIT(13)
	#define TOP_CKG_MLINK_108M                    BITS(15:14,0)
	#define TOP_CKG_MLINK_86M                     BITS(15:14,1)
	#define TOP_CKG_MLINK_54M                     BITS(15:14,2)
	#define TOP_CKG_MLINK_DFT                     BITS(15:14,3)

//#define REG_TOP_GPIO_WN_OEN_DVBC    0x0032
//    #define TOP_GPIO_W0                         BIT(0)
//    #define TOP_GPIO_W1                         BIT(1)
//    #define TOP_GPIO_N0                         BIT(4)
//    #define TOP_GPIO_N1                         BIT(5)
//    #define TOP_GPIO_N2                         BIT(6)
//    #define TOP_GPIO_N3                         BIT(7)
//    #define TOP_CKG_DVBC_DMA_MASK               BMASK(11:8)
//    #define TOP_CKG_DVBC_DMA_EQ                 BITS(11:8, 0)
//    #define TOP_CKG_DVBC_DMA_ADCD               BITS(11:8, 1)
//    #define TOP_CKG_DVBC_DMA_0                  BITS(11:8, 2)
//    #define TOP_CKG_DVBC_DMA_DFT                BITS(11:8, 3)
#define REG_TOP_DUMMY23             0x0037
    #define TOP_CKG_DUMMY23_MASK                BMASK(10:0)
#define REG_TOP_GPIO_PWM_CI_IN      0x0039
#define REG_TOP_GPIO_PWM_CI_OUT     0x003A
   #define TOP_GPIO_PWM0_OUT                   BIT(4)
   #define TOP_GPIO_PWM1_OUT                   BIT(5)
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


#define REG_TOP_GPIO_PWM_CI_OEN     0x003B
    #define TOP_GPIO_PWM0_DISABLE               BIT(4)
    #define TOP_GPIO_PWM1_DISABLE               BIT(5)
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
#define REG_TOP_                    0x004f
    #define TOP_CKG_EMACAHB_MASK                BMASK(11:8)
    #define TOP_CKG_EMACAHB_DIS                 BIT(8)
    #define TOP_CKG_EMACAHB_INV                 BIT(9)
    #define TOP_CKG_EMACAHB_SRC_108             BITS(11:10, 0)
    #define TOP_CKG_EMACAHB_SRC_86              BITS(11:10, 1)
    #define TOP_CKG_EMACAHB_SRC_62              BITS(11:10, 2)
    #define TOP_CKG_EMACAHB_SRC_DFT             BITS(11:10, 3)
#define REG_TOP_GPIO_DI_DO          0x0054
#define REG_TOP_UART                0x0055
#define REG_TOP_JTAG_AUD_SPI_GPIO_PWM 0x0056
    #define TOP_PAD_AU1_MASK                    BMASK(5:4)
    #define TOP_PAD_AU1_GPIO                    0
    #define TOP_PAD_AU1_IICM0                   BITS(5:4, 1)
    #define TOP_PAD_AU1_IICM1                   BITS(5:4, 2)
    #define TOP_PAD_AU1_X                       BITS(5:4, 3)
    #define TOP_PAD_AU2_MASK                    BMASK(7:6)
    #define TOP_PAD_AU2_GPIO                    0
    #define TOP_PAD_AU2_PCM                     BMASK(7:6, 1)
    #define TOP_PAD_AU2_X                       BMASK(7:6, 2)
    #define TOP_PAD_SPICZ2_MASK                 BIT(8)
    #define TOP_PAD_SPICZ2_X                    0
    #define TOP_PAD_SPICZ2_GPIO                 BIT(8)
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
#define REG_TOP_RESET_CPU0          0x0077
    #define TOP_RESET_PASSWD_MASK               BMASK(14:0)
    #define TOP_RESET_CPU0                      BIT(15)


#define REG_WDT_BASE                0xBF808400
#define WDT_REG(addr)               (*((volatile MS_U32*)(REG_WDT_BASE + ((addr)<<2))))

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
#define ANA_REG(addr)               (*((volatile MS_U32*)(REG_ANA_BASE + ((addr)<<2))))

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
    #define ANA_AGC_PAD_OEN                     BIT(12);

// 0x000D
    #define ANA_ADC_OFFSET                      BMASK(11:0)
    #define ANA_ADC_RDSEL                       BIT(13)
    #define ANA_ADCIN_SIGN                      BIT(14)
    #define ANA_ADCD_FRZ                        BIT(15)

// 0x0010
    // reg_adcin_i read only register Debug Purpose to check ADC is Input or Not!
    // If ADC is input , reading this register will find it always changing !
    #define ANA_ADCIN_I                         BMASK(10:0)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _MHAL_CHIPTOP_REG_H_


////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
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

#ifndef _HWREG_S5_H
#define _HWREG_S5_H

//------------------------------------------------------------------------------
// Base Address
//
// different with S4
//   NFIE
//   POR_STATUS
//   IRQ_AEON1
//   USB
//   SDIO0 ~ SDIO3 (NEW!)
//   MSTAR_LINK (NEW!)
//   PM_SLEEP (NEW!)
//   MAU_DBG
//   RIU_DBG
//   PWM
//   MAU0 MAU1 (NEW!)
//   WDT
//   ECBRIGHT (NEW!)
//   EMAC0 ~ EMAC3 (NEW!)
//   MLINK (NEW!)
//   RTC
//------------------------------------------------------------------------------
#define MENULOAD_REG_BASE                       0x0200  // 0x0200 - 0x021F
#define NFIE_REG_BASE                           0x0300  // 0x0300 - 0x03FF
#define DDC_REG_BASE                            0x0400  // 0x0400 - 0x04FF
#define POR_STATUS_REG_BASE                     0x0500  // 0x0500 - 0x053F
#define IRQ_AEON1_REG_BASE                      0x0540  // 0x0540 - 0x057F
#define NORPF_REG_BASE                          0x0580  // 0x0580 - 0x05FF
#define MIU2_REG_BASE                           0X0600  // 0x0600 - 0x06FF
#define USB_REG_BASE                            0x0700  // 0x0700 - 0x07FF
#define ISP_REG_BASE                            0x0800  // 0x0800 - 0x08FF
#define BDMA_REG_BASE                           0x0900  // 0x0900 - 0x097F
#define UART_REG_BASE                           0x0980  // 0x0980 - 0x09FF
#define SDIO0_REG_BASE                          0x0A00  // 0x0A00 - 0x0A7F
#define SDIO1_REG_BASE                          0x0A80  // 0x0A80 - 0x0AFF
#define SDIO2_REG_BASE                          0x0B00  // 0x0B00 - 0x0BFF
#define SDIO3_REG_BASE                          0x0C00  // 0x0C00 - 0x0CFF
#define MSTAR_LINK_REG_BASE                     0x0D00  // 0x0D00 - 0x0DFF
#define PM_SLEEP_REG_BASE                       0x0E00  // 0x0E00 - 0x0E7F
#define MAU_DBG_REG_BASE                        0x0E80  // 0x0E80 - 0x0EBF
#define RIU_DBG_REG_BASE                        0x0EC0  // 0x0EC0 - 0x0EFF
#define AEON_REG_BASE                           0x0F00  // 0x0F00 - 0x0FFF
#define MCU_PERI_REG_BASE                       0x1000  // 0x1000 - 0x10FF
#define MVD_REG_BASE                            0x1100  // 0x1100 - 0x11FF
#define MIU_REG_BASE                            0x1200  // 0x1200 - 0x12FF
#define VD_MCU_REG_BASE                         0x1300  // 0x1300 - 0x13FF
#define MVOP_REG_BASE                           0x1400  // 0x1400 - 0x14FF
#define TSP0_REG_BASE                           0x1500  // 0x1500 - 0x15FF
#define TSP1_REG_BASE                           0x1600  // 0x1600 - 0x16FF
#define JPD_REG_BASE                            0x1700  // 0x1700 - 0x17FF
#define SEM_REG_BASE                            0x1800  // 0x1800 - 0x183F
#define MAU0_REG_BASE                           0x1840  // 0x1840 - 0x185F
#define MAU1_REG_BASE                           0x1860  // 0x1860 - 0x187F
#define WDT_REG_BASE                            0x1880  // 0x1880 - 0x18BF
#define ECBRIDGE_REG_BASE                       0x18C0  // 0x18C0 - 0x18FF

//#define VIFDBB1_REG_BASE                        0x1900
//#define VIFDBB2_REG_BASE                        0x1A00
//#define VIFDBB3_REG_BASE                        0x1B00
//#define VIFRF_REG_BASE                          0x1C00

#define CHIP_REG_BASE                           0x1E00  // 0x1E00 - 0x1EFF
#define GOP_REG_BASE                            0x1F00  // 0x1F00 - 0x1FFF
#define EMAC0_REG_BASE                          0x2000  // 0x2000 - 0x20FF
#define EMAC1_REG_BASE                          0x2100  // 0x2100 - 0x21FF
#define EMAC2_REG_BASE                          0x2200  // 0x2200 - 0x22FF
#define EMAC3_REG_BASE                          0x2300  // 0x2300 - 0x23FF
#define USB0_REG_BASE                           0x2400  // 0x2400 - 0x24FF
#define ADC_ATOP_REG_BASE                       0x2500  // 0x2500 - 0x25FF
#define ADC_DTOP_REG_BASE                       0x2600  // 0x2600 - 0x26EF
#define HDCP_REG_BASE                           0x26F0  // 0x26F0 - 0x26FF
#define HDMI_REG_BASE                           0x2700  // 0x2700 - 0x27FF
#define GE_REG_BASE                             0x2800  // 0x2800 - 0x28FF
#define STRLD_REG_BASE                          0x2900  // 0x2900 - 0x29FF
#define CI_REG_BASE                             0x2A00  // 0x2A00 - 0x2AFF
#define IRQ_REG_BASE                            0x2B00  // 0x2B00 - 0x2B7F
#define CACHE_REG_BASE                          0x2B80  // 0x2B80 - 0x2BBF
#define XDMIU_REG_BASE                          0x2BC0  // 0x2BC0 - 0x2BFF
#define VIVALDI0_REG_BASE                       0x2C00  // 0x2C00 - 0x2CFF
#define VIVALDI1_REG_BASE                       0x2D00  // 0x2D00 - 0x2DFF
#define SC0_REG_BASE                            0x2E00  // 0x2E00 - 0x2EFF
#define SC1_REG_BASE                            0x2F00  // 0x2F00 - 0x2FFF
#define SC2_REG_BASE                            0x3000  // 0x3000 - 0x30FF
#define SC3_REG_BASE                            0x3100  // 0x3100 - 0x31FF
#define SC4_REG_BASE                            0x3200  // 0x3200 - 0x32FF
#define MLINK_REG_BASE                          0x3300  // 0x3300 - 0x337F
#define MAILBOX_REG_BASE                        0x3380  // 0x3380 - 0x33FF
#define IIC_REG_BASE                            0x3420  // 0x3420 - 0x343F
#define PCM_REG_BASE                            0x3440  // 0x3440 - 0x345F
#define RTC_REG_BASE                            0x3480  // 0x3480 - 0x349F
#define PM_REG_BASE                             0x34A0  // 0x34A0 - 0x34BF
#define DHC_REG_BASE                            0x34C0  // 0x34C0 - 0x34DF
#define AFEC_REG_BASE                           0x3500  // 0x3500 - 0x35FF
#define COMB_REG_BASE                           0x3600  // 0x3600 - 0x36FF
#define VBI_REG_BASE                            0x3700  // 0x3700 - 0x37FF
#define SCM_REG_BASE                            0x3800  // 0x3800 - 0x38FF
#define CCFL_REG_BASE                           0x3900  // 0x3900 - 0x397F
#define PATGEN_REG_BASE                         0x3980  // 0x3980 - 0x39FF
#define SAR_REG_BASE                            0x3A00  // 0x3A00 - 0x3A7F
#define UTMI_REG_BASE                           0x3A80  // 0x3A80 - 0x3AFF
#define VE0_REG_BASE                            0x3B00  // 0x3B00 - 0x3BFF
#define PIU_MISC_REG_BASE                       0x3C00  // 0x3C00 - 0x3CFF
#define PWM_REG_BASE                            0x3D00  // 0x3D00 - 0x3D3F
#define IR_REG_BASE                             0x3D80  // 0x3D80 - 0x3DFF
#define VE1_REG_BASE                            0x3E00  // 0x3E00 - 0x3EFF
#define VE2_REG_BASE                            0x3F00  // 0x3F00 - 0x3FFF

//------------------------------------------------------------------------------
// Byte DMA controller
//------------------------------------------------------------------------------
#define REG_BDMA_INT_STATE                      (BDMA_REG_BASE+0x00)
#define REG_BDMA_QUEUE_STATUS                   (BDMA_REG_BASE+0x01)
#define REG_BDMA_HOST0_INT_EN                   (BDMA_REG_BASE+0x04)
#define REG_BDMA_HOST1_INT_EN                   (BDMA_REG_BASE+0x08)

#define REG_BDMA_CHCTRL_OFFSET                  (0x20)
#define REG_BDMA_CH0_CTRL                       (BDMA_REG_BASE+0x20)
#define REG_BDMA_CH0_MISC                       (BDMA_REG_BASE+0x21)
#define REG_BDMA_CH0_STATUS                     (BDMA_REG_BASE+0x22)
#define REG_BDMA_CH0_SIZE_0                     (BDMA_REG_BASE+0x24)
#define REG_BDMA_CH0_SIZE_1                     (BDMA_REG_BASE+0x25)
#define REG_BDMA_CH0_SIZE_2                     (BDMA_REG_BASE+0x26)
#define REG_BDMA_CH0_SIZE_3                     (BDMA_REG_BASE+0x27)
#define REG_BDMA_CH0_SRC_ADDR_0                 (BDMA_REG_BASE+0x28)
#define REG_BDMA_CH0_SRC_ADDR_1                 (BDMA_REG_BASE+0x29)
#define REG_BDMA_CH0_SRC_ADDR_2                 (BDMA_REG_BASE+0x2A)
#define REG_BDMA_CH0_SRC_ADDR_3                 (BDMA_REG_BASE+0x2B)
#define REG_BDMA_CH0_DST_ADDR_0                 (BDMA_REG_BASE+0x2C)
#define REG_BDMA_CH0_DST_ADDR_1                 (BDMA_REG_BASE+0x2D)
#define REG_BDMA_CH0_DST_ADDR_2                 (BDMA_REG_BASE+0x2E)
#define REG_BDMA_CH0_DST_ADDR_3                 (BDMA_REG_BASE+0x2F)
#define REG_BDMA_CH0_EXTBITS_0                  (BDMA_REG_BASE+0x30)
#define REG_BDMA_CH0_EXTBITS_1                  (BDMA_REG_BASE+0x31)
#define REG_BDMA_CH0_EXTBITS_2                  (BDMA_REG_BASE+0x32)
#define REG_BDMA_CH0_EXTBITS_3                  (BDMA_REG_BASE+0x33)
#define REG_BDMA_CH0_MATCHADDR_0                (BDMA_REG_BASE+0x34)
#define REG_BDMA_CH0_MATCHADDR_1                (BDMA_REG_BASE+0x35)
#define REG_BDMA_CH0_MATCHADDR_2                (BDMA_REG_BASE+0x36)
#define REG_BDMA_CH0_MATCHADDR_3                (BDMA_REG_BASE+0x37)
#define REG_BDMA_CH1_CTRL                       (BDMA_REG_BASE+0x40)
#define REG_BDMA_CH1_MISC                       (BDMA_REG_BASE+0x41)
#define REG_BDMA_CH1_STATUS                     (BDMA_REG_BASE+0x42)
#define REG_BDMA_CH1_SIZE_0                     (BDMA_REG_BASE+0x44)
#define REG_BDMA_CH1_SIZE_1                     (BDMA_REG_BASE+0x45)
#define REG_BDMA_CH1_SIZE_2                     (BDMA_REG_BASE+0x46)
#define REG_BDMA_CH1_SIZE_3                     (BDMA_REG_BASE+0x47)
#define REG_BDMA_CH1_SRC_ADDR_0                 (BDMA_REG_BASE+0x48)
#define REG_BDMA_CH1_SRC_ADDR_1                 (BDMA_REG_BASE+0x49)
#define REG_BDMA_CH1_SRC_ADDR_2                 (BDMA_REG_BASE+0x4A)
#define REG_BDMA_CH1_SRC_ADDR_3                 (BDMA_REG_BASE+0x4B)
#define REG_BDMA_CH1_DST_ADDR_0                 (BDMA_REG_BASE+0x4C)
#define REG_BDMA_CH1_DST_ADDR_1                 (BDMA_REG_BASE+0x4D)
#define REG_BDMA_CH1_DST_ADDR_2                 (BDMA_REG_BASE+0x4E)
#define REG_BDMA_CH1_DST_ADDR_3                 (BDMA_REG_BASE+0x4F)
#define REG_BDMA_CH1_EXTBITS_0                  (BDMA_REG_BASE+0x50)
#define REG_BDMA_CH1_EXTBITS_1                  (BDMA_REG_BASE+0x51)
#define REG_BDMA_CH1_EXTBITS_2                  (BDMA_REG_BASE+0x52)
#define REG_BDMA_CH1_EXTBITS_3                  (BDMA_REG_BASE+0x53)
#define REG_BDMA_CH1_MATCHADDR_0                (BDMA_REG_BASE+0x54)
#define REG_BDMA_CH1_MATCHADDR_1                (BDMA_REG_BASE+0x55)
#define REG_BDMA_CH1_MATCHADDR_2                (BDMA_REG_BASE+0x56)
#define REG_BDMA_CH1_MATCHADDR_3                (BDMA_REG_BASE+0x57)

    //---------------------------------------------
    // definition for REG_BDMA_CH0_CTRL/REG_BDMA_CH1_CTRL
    #define BDMA_TRIGGER                        BIT0
    #define BDMA_STOP                           BIT1
    #define BDMA_PATTERN_SEARCH_TRIGGER         BIT4
    #define BDMA_RESTART_SEARCH                 BIT6
    #define BDMA_DISABLE_PATTERN_SEARCH         BIT7
    //---------------------------------------------
    // definition for REG_BDMA_CH0_MISC/REG_BDMA_CH1_MISC
    #define BDMA_SRC_MIU                        BIT0
    #define BDMA_DST_MIU                        BIT2
    #define BDMA_ADDR_DIRECTION                 BIT4
    #define BDMA_DMA_DONE_INT_EN                BIT5
    #define BDMA_DMA_ERR_INT_EN                 BIT6
    #define BDMA_PATTERN_SEARCH_DONE            BIT7
    //---------------------------------------------
    // definition for REG_BDMA_CH0_STATUS/REG_BDMA_CH1_STATUS
    #define BDMA_CH_QUEUED                      BIT0
    #define BDMA_CH_BUSY                        BIT1
    #define BDMA_CH_DONE                        BIT2
    #define BDMA_CH_ERR                         BIT3
    #define BDMA_CH_SEARCH_DONE                 BIT4
    #define BDMA_CH_SEARCH_MATCH                BIT5

//------------------------------------------------------------------------------
// MCU and PIU Reg
//------------------------------------------------------------------------------
#define AEON_REG_CTRL                           (AEON_REG_BASE+0x00F0)

    //---------------------------------------------
    // definition for AEON_REG_CTRL   //reg[0x0FF0]
    #define AEON_CTRL_EN                            BIT0
    #define AEON_CTRL_RST                           BIT1
    #define AEON_DWB_SWAP                           BIT3

#define AEON_SPI_ADDR0                          (AEON_REG_BASE+0x00FE)
#define AEON_SPI_ADDR1                          (AEON_REG_BASE+0x00FF)

//------------------------------------------------------------------------------
//PLL control in ATOP
//------------------------------------------------------------------------------
#define PLL_CONTROL_IN_ATOP                     0x2510

//------------------------------------------------------------------------------
// MCU and PIU Reg (0x1000)
//------------------------------------------------------------------------------
#define DRAM_START_ADDR_0                       0x100C
#define DRAM_START_ADDR_1                       0x100D
#define DRAM_START_ADDR_2                       0x1008
#define DRAM_START_ADDR_3                       0x1009
#define DRAM_END_ADDR_0                         0x100E
#define DRAM_END_ADDR_1                         0x100F
#define DRAM_END_ADDR_2                         0x100A
#define DRAM_END_ADDR_3                         0x100B

#define SPI_START_ADDR_0                        0x1014
#define SPI_START_ADDR_1                        0x1015
#define SPI_START_ADDR_2                        0x1010
#define SPI_START_ADDR_3                        0x1011
#define SPI_END_ADDR_0                          0x1016
#define SPI_END_ADDR_1                          0x1017
#define SPI_END_ADDR_2                          0x1012
#define SPI_END_ADDR_3                          0x1013

#define ROM_CODE_CTRL                           0x1018

    #define SRAM_ENABLE                             0x01
    #define SPI_ENABLE                              0x02
    #define DRAM_ENABLE                             0x04
    #define ICACHE_REST                             0x08
    #define MCU_BANK_USE_XFR                        0x80

#define ROM_BANK_ADR                            0x1019

#define REG_RESET_CPU_8051                      0x1080
#define REG_RESET_CPU_AEON                      0x1082
#define REG_SW_RESET_CPU_8051                   0x1084
#define REG_SW_RESET_CPU_AEON                   0x1086

    //---------------------------------------------
    // definition for REG_SW_RESET_CPU_AEON   //reg[0x1086]
    #define AEON_SW_RESET                           BIT0

//---------------------
// Menu load Registers
//---------------------
#define REG_ML_MISC                     (MCU_PERI_REG_BASE+0xC0)
    #define ML_MISC_BUSY                    BIT5 // Busy, Cleared when Done
    #define ML_MISC_DONE                    BIT6 // Done, Cleared when Enabled
    #define ML_MISC_BUFMUX                  BIT7 // Use SRAM as menuload buffer
#define REG_ML_CTRL                     (MCU_PERI_REG_BASE+0xC1)
    #define ML_CTRL_SRAM_CS                 BIT7 // Enable SRAM Chip Select
#define REG_ML_NUMBER                   (MCU_PERI_REG_BASE+0xC2)
#define REG_ML_ABST                     (MCU_PERI_REG_BASE+0xC4)
#define REG_ML_TRI_SEL                  (MCU_PERI_REG_BASE+0xC6)
    #define ML_TRI_SELMASK                  (BIT0+BIT1+BIT2)
#define REG_ML_F_TRI                    (MCU_PERI_REG_BASE+0xC8)
    #define ML_F_TRI_FIRE                   BIT0 // menuload realtime trigger one shot

#define REG_ML_ABORT                    (MCU_PERI_REG_BASE+0xCA)
#define REG_ML_ENABLE                   (MCU_PERI_REG_BASE+0xCC)
    #define ML_ENABLE_ONE_SHOT              BIT0 // menuload enable one shot
#define REG_ML_MODE                     (MCU_PERI_REG_BASE+0xCE)
#define REG_ML_ADDR_CLR                 (MCU_PERI_REG_BASE+0xD0)
    #define ML_CLR_ADDR                     BIT0 // Clear Menu load Address

#define REG_ML_ADDRL                    (MENULOAD_REG_BASE+0x00)
#define REG_ML_ADDRH                    (MENULOAD_REG_BASE+0x01)
#define REG_ML_DATA                     (MENULOAD_REG_BASE+0x02)

//------------------------------------------------------------------------------
// MVD Reg
//------------------------------------------------------------------------------
#define MVD_CTRL                                0x1100
#define MVD_STATUS                              0x1101
#define MVD_COMMAND                             0x1102
#define MVD_ARG0                                0x1104
#define MVD_ARG1                                0x1105
#define MVD_ARG2                                0x1106
#define MVD_ARG3                                0x1107
//------------------------------------------------------------------------------
// MVOP Reg
//------------------------------------------------------------------------------
#define VOP_FRAME_VCOUNT                        0x1400
#define VOP_FRAME_HCOUNT                        0x1402
#define VOP_VB0_STR                             0x1404
#define VOP_VB0_END                             0x1406
#define VOP_VB1_STR                             0x1408
#define VOP_VB1_END                             0x140A
#define VOP_TF_STR                              0x140C
#define VOP_BF_STR                              0x140E
#define VOP_HACT_STR                            0x1410
#define VOP_IMG_HSTR                            0x1412
#define VOP_IMG_VSTR0                           0x1414
#define VOP_IMG_VSTR1                           0x1416
#define VOP_TF_VS                               0x1418
#define VOP_BF_VS                               0x141A
#define VOP_HI_TSH                              0x1420
#define VOP_CTRL0                               0x1422
#define VOP_TST_IMG                             0x1424

#define VOP_U_PAT                               0x1426
#define VOP_INT_MASK                            0x143E
#define VOP_MPG_JPG_SWITCH                      0x1440
#define VOP_JPG_YSTR0_L                         0x1442
#define VOP_JPG_YSTR0_H                         0x1444
#define VOP_JPG_UVSTR0_L                        0x1446
#define VOP_JPG_UVSTR0_H                        0x1448
#define VOP_JPG_HSIZE                           0x144A
#define VOP_JPG_VSIZE                           0x144C
#define VOP_REG_WR                              0x144E
#define VOP_DEBUG_R                             0x145E

#define VOP_JPG_YSTR1_L                         0x1462
#define VOP_JPG_YSTR1_H                         0x1464
#define VOP_JPG_UVSTR1_L                        0x1466
#define VOP_JPG_UVSTR1_H                        0x1468

//------------------------------------------------------------------------------
// VD MCU control register in Saturn4
//------------------------------------------------------------------------------
#define VD_MCU_OFFSET_ADDR_0                    0x1300
#define VD_MCU_OFFSET_ADDR_1                    0x1301
#define VD_MCU_PAGE_ADDR                        0x1302

#define VD_MCU_WDATA                            0x1304
#define VD_MCU_WDATA_CTRL                       0x1305

#define VD_MCU_RDATA                            0x1306
#define VD_MCU_RDATA_CTRL                       0x1307

#define VD_MCU_ADDR_CTRL                        0x1308
#define VD_MCU_READ_WRITE                       0x130A
#define VD_MCU_RESET                            0x130C

//------------------------------------------------------------------------------
// Semaphore controller
//------------------------------------------------------------------------------
#define REG_SEM_ID0                            (SEM_REG_BASE+0x00)
#define REG_SEM_ID1                            (SEM_REG_BASE+0x02)
#define REG_SEM_ID2                            (SEM_REG_BASE+0x04)
#define REG_SEM_ID3                            (SEM_REG_BASE+0x06)
#define REG_SEM_ID4                            (SEM_REG_BASE+0x08)
#define REG_SEM_ID5                            (SEM_REG_BASE+0x0A)
#define REG_SEM_ID6                            (SEM_REG_BASE+0x0C)
#define REG_SEM_ID7                            (SEM_REG_BASE+0x0E)

//------------------------------------------------------------------------------
// IRQ controller
//------------------------------------------------------------------------------
#define EX0_INT_MASK                            (IRQ_REG_BASE+0x0000)
#define EX0_INT_CLEAR_0                         (IRQ_REG_BASE+0x0008)
#define EX0_INT_CLEAR_1                         (IRQ_REG_BASE+0x0009)
#define EX0_INT_CLEAR_2                         (IRQ_REG_BASE+0x000A)
#define EX0_INT_CLEAR_3                         (IRQ_REG_BASE+0x000B)
#define EX0_INT_FINAL_STATUS_0                  (IRQ_REG_BASE+0x0010)
#define EX0_INT_FINAL_STATUS_1                  (IRQ_REG_BASE+0x0011)
#define EX0_INT_FINAL_STATUS_2                  (IRQ_REG_BASE+0x0012)
#define EX0_INT_FINAL_STATUS_3                  (IRQ_REG_BASE+0x0013)

#define EX1_INT_MASK                            (IRQ_REG_BASE+0x0018)
#define EX1_INT_FINAL_STATUS_0                  (IRQ_REG_BASE+0x0028)
#define EX1_INT_FINAL_STATUS_1                  (IRQ_REG_BASE+0x0029)
#define EX1_INT_FINAL_STATUS_2                  (IRQ_REG_BASE+0x002A)
#define EX1_INT_FINAL_STATUS_3                  (IRQ_REG_BASE+0x002B)

#define AEON_C_FIQ_CLR_0                        (IRQ_REG_BASE+0x0048)
#define AEON_C_FIQ_CLR_1                        (IRQ_REG_BASE+0x0049)
#define AEON_C_FIQ_CLR_2                        (IRQ_REG_BASE+0x004A)
#define AEON_C_FIQ_CLR_3                        (IRQ_REG_BASE+0x004B)

#define REG_C_FIQ_FORCE_0                       (IRQ_REG_BASE+0x0004)
#define REG_C_FIQ_FORCE_1                       (IRQ_REG_BASE+0x0005)
#define REG_AEON_C_FIQ_FORCE_0                  (IRQ_REG_BASE+0x0044)
#define REG_AEON_C_FIQ_FORCE_1                  (IRQ_REG_BASE+0x0045)

#define REG_SEND_AEON_IRQ                       (IRQ_REG_BASE+0x007C)

//------------------------------------------------------------------------------
// ISP
//------------------------------------------------------------------------------
#define ISP_BASE                                0x800

#define REG_SFSH_ISP_PASSWORD                   0x0800  //ISP / XIU read / DMA mutual exclusive
#define REG_SFSH_SPI_COMMAND                    0x0802
#define REG_SFSH_ADDRESS21                      0x0804  //A[15:0]
#define REG_SFSH_ADDRESS3                       0x0806  //A[23:16]
#define REG_SFSH_WDATA                          0x0808
#define REG_SFSH_RDATA                          0x080A
#define REG_SFSH_SPI_CLK_DIV                    0x080C
#define REG_SFSH_DEV_SEL                        0x080E
#define REG_SFSH_SPI_CE_CLR                     0x0810
#define REG_SFSH_SPI_RD_REQ                     0x0818
#define REG_SFSH_ENDIAN_SEL_SPI                 0x081E
#define REG_SFSH_SPI_RD_DATA_RDY                0x082A
#define REG_SFSH_SPI_WR_DATA_RDY                0x082C
#define REG_SFSH_SPI_WR_CMD_RDY                 0x082E
#define REG_SFSH_TRIGGER_MODE                   0x0854

//------------------------------------------------------------------------------
// chip top
//------------------------------------------------------------------------------
#define REG_MUX_CONFIG_0                        0x1E02
#define REG_MUX_CONFIG_1                        0x1E03
#define REG_MUX_CONFIG_2                        0x1E04
#define REG_MUX_CONFIG_3                        0x1E05
#define REG_MUX_CONFIG_4                        0x1E06
#define REG_MUX_CONFIG_5                        0x1E07


#define REG_CT_MISC0                            0x1E22
#define REG_CT_MISC1                            0x1E23

#if 1
#define REG_CKG_USB30           0x1E24
    #define CKG_USB30_GATED         BIT0
    #define CKG_USB30_INVERT        BIT1

#define REG_CKG_DDR             0x1E25
    #define CKG_DDR_GATED           BIT0
    #define CKG_DDR_INVERT          BIT1
    #define CKG_DDR_MASK            (BIT3 | BIT2)
    #define CKG_DDR_MEMPLL          (0 << 2)
    #define CKG_DDR_170MHZ          (1 << 2)
    #define CKG_DDR_123MHZ          (2 << 2)
    #define CKG_DDR_108MHZ          (3 << 2)

#define REG_CKG_MIU             0x1E25
    #define CKG_MIU_GATED           BIT4
    #define CKG_MIU_INVERT          BIT5
    #define CKG_MIU_MASK            (BIT7 | BIT6)
#define REG_CKG_MIU_4           0x1E46
    #define CKG_MIU_4_MASK          BIT1
    #define CKG_MIU_MEMPLL          (0 << 6)
    #define CKG_MIU_170MHZ          (1 << 6)
    #define CKG_MIU_123MHZ          (2 << 6)
    #define CKG_MIU_216MHZ          (3 << 6)
    #define CKG_MIU_SCPLL           (4 << 6)

#define REG_CKG_TS0             0x1E26
    #define CKG_TS0_GATED           BIT0
    #define CKG_TS0_INVERT          BIT1
    #define CKG_TS0_MASK            (BIT3 | BIT2)
    #define CKG_TS0_TS0_CLK         (0 << 2)
    #define CKG_TS0_TS2_CLK         (1 << 2)
    #define CKG_TS0_0               (2 << 2)
    #define CKG_TS0_XTAL            (3 << 2)

#define REG_CKG_TCK             0x1E26
    #define CKG_TCK_GATED           BIT4
    #define CKG_TCK_INVERT          BIT5

#define REG_CKG_AEON            0x1E26
    #define CKG_AEON_GATED          BIT6
    #define CKG_AEON_INVERT         BIT7

#define REG_CKG_TSP             0x1E27
    #define CKG_TSP_GATED           BIT0
    #define CKG_TSP_INVERT          BIT1
    #define CKG_TSP_MASK            (BIT3 | BIT2)
    #define CKG_TSP_144MHZ          (0 << 2)
    #define CKG_TSP_123MHZ          (1 << 2)
    #define CKG_TSP_72MHZ           (2 << 2)
    #define CKG_TSP_XTAL            (3 << 2)

#define REG_CKG_STC0            0x1E27
    #define CKG_STC0_GATED          BIT4
    #define CKG_STC0_INVERT         BIT5
    #define CKG_STC0_MASK           (BIT7 | BIT6)
    #define CKG_STC0_STC0_SYNTH     (0 << 6)
    #define CKG_STC0_1              (1 << 6)
    #define CKG_STC0_27MHZ          (2 << 6)
    #define CKG_STC0_XTAL           (3 << 6)

#define REG_CKG_MAD_STC         0x1E28
    #define CKG_MAD_STC_GATED       BIT4
    #define CKG_MAD_STC_INVERT      BIT5
    #define CKG_MAD_STC_MASK        (BIT7 | BIT6)
    #define CKG_MAD_STC_STC0_SYNTH  (0 << 6)
    #define CKG_MAD_STC_1           (1 << 6)
    #define CKG_MAD_STC_27MHZ       (2 << 6)
    #define CKG_MAD_STC_XTAL        (3 << 6)

#define REG_MVD_BOOT            0x1E29
    #define CKG_MVD_BOOT_GATED      BIT0
    #define CKG_MVD_BOOT_INVERT     BIT1
    #define CKG_MVD_BOOT_MASK       (BIT3 | BIT2)
    #define CKG_MVD_BOOT_144MHZ     (0 << 2)
    #define CKG_MVD_BOOT_123MHZ     (1 << 2)
    #define CKG_MVD_BOOT_CLK_MIU    (2 << 2)
    #define CKG_MVD_BOOT_XTAL       (3 << 2)

#define REG_CKG_MVD             0x1E29
    #define CKG_MVD_GATED           BIT4
    #define CKG_MVD_INVERT          BIT5
    #define CKG_MVD_MASK            (BIT7 | BIT6)
    #define CKG_MVD_144MHZ          (0 << 6)
    #define CKG_MVD_123MHZ          (1 << 6)
    #define CKG_MVD_CLK_MIU         (2 << 6)
    #define CKG_MVD_XTAL            (3 << 6)

#define REG_CKG_M4V             0x1E2A
    #define CKG_M4V_GATED           BIT0
    #define CKG_M4V_INVERT          BIT1
    #define CKG_M4V_MASK            (BIT3 | BIT2)
    #define CKG_M4V_144MHZ          (0 << 2)
    #define CKG_M4V_123MHZ          (1 << 2)
    #define CKG_M4V_108MHZ          (2 << 2)
    #define CKG_M4V_XTAL            (3 << 2)

#define REG_CKG_DC0             0x1E2A
    #define CKG_DC0_GATED           BIT4
    #define CKG_DC0_INVERT          BIT5
    #define CKG_DC0_MASK            (BIT7 | BIT6)
#define REG_CKG_DC0_3           0x1E46
    #define CKG_DC0_3_MASK          BIT0
    #define CKG_DC0_SYNCHRONOUS     (0 << 6)
    #define CKG_DC0_FREERUN         (1 << 6)
    #define CKG_DC0_27MHZ           (2 << 6)
    #define CKG_DC0_54MHZ           (3 << 6)
    #define CKG_DC0_72MHZ           (4 << 6)
    #define CKG_DC0_86MHZ           (5 << 6)
    #define CKG_DC0_108MHZ          (6 << 6)
    #define CKG_DC0_144MHZ          (7 << 6)

#define REG_CKG_DHC_SBM         0x1E2B
    #define CKG_DHC_SBM_GATED       BIT0
    #define CKG_DHC_SBM_INVERT      BIT1
    #define CKG_DHC_SBM_MASK        (BIT3 | BIT2)
    #define CKG_DHC_SBM_12MHZ       (0 << 2)
    #define CKG_DHC_SBM_36MHZ       (1 << 2)
    #define CKG_DHC_SBM_62MHZ       (2 << 2)
    #define CKG_DHC_SBM_XTAL        (3 << 2)

#define REG_CKG_GE              0x1E2B
    #define CKG_GE_GATED            BIT4
    #define CKG_GE_INVERT           BIT5
    #define CKG_GE_MASK             (BIT7 | BIT6)
    #define CKG_GE_123MHZ           (0 << 6)
    #define CKG_GE_108MHZ           (1 << 6)
    #define CKG_GE_86MHZ            (2 << 6)
    #define CKG_GE_XTAL             (3 << 6)

#define REG_CKG_GOPG0           0x1E2C
    #define CKG_GOPG0_GATED         BIT0
    #define CKG_GOPG0_INVERT        BIT1
    #define CKG_GOPG0_MASK          (BIT3 | BIT2)
    #define CKG_GOPG0_ODCLK         (0 << 2)
    #define CKG_GOPG0_0             (1 << 2)
    #define CKG_GOPG0_IDCLK2        (2 << 2)
    #define CKG_GOPG0_XTAL          (3 << 2)

#define REG_CKG_GOPG1           0x1E2C
    #define CKG_GOPG1_GATED         BIT4
    #define CKG_GOPG1_INVERT        BIT5
    #define CKG_GOPG1_MASK          (BIT7 | BIT6)
    #define CKG_GOPG1_ODCLK         (0 << 6)
    #define CKG_GOPG1_0             (1 << 6)
    #define CKG_GOPG1_IDCLK2        (2 << 6)
    #define CKG_GOPG1_XTAL          (3 << 6)

#define REG_CKG_GOPD            0x1E2D
    #define CKG_GOPD_GATED          BIT0
    #define CKG_GOPD_INVERT         BIT1
    #define CKG_GOPD_MASK           (BIT3 | BIT2)
    #define CKG_GOPD_CLK_ADC        (0 << 2)
    #define CKG_GOPD_CLK_ODCLK      (1 << 2)
    #define CKG_GOPD_CLK_DC0        (2 << 2)
    #define CKG_GOPD_XTAL           (3 << 2)

#define REG_CKG_VD              0x1E2D
    #define CKG_VD_GATED            BIT4
    #define CKG_VD_INVERT           BIT5
    #define CKG_VD_MASK             (BIT7 | BIT6)
    #define CKG_VD_CLK_VD           (0 << 6)
    #define CKG_VD_CLK_VD_          (1 << 6)
    #define CKG_VD_TESTMODE_CLK     (2 << 6)
    #define CKG_VD_XTAL             (3 << 6)

#define REG_CKG_VDMCU           0x1E2E
    #define CKG_VDMCU_GATED         BIT0
    #define CKG_VDMCU_INVERT        BIT1
    #define CKG_VDMCU_MASK          (BIT3 | BIT2)
    #define CKG_VDMCU_108MHZ        (0 << 2)
    #define CKG_VDMCU_86MHZ         (1 << 2)
    #define CKG_VDMCU_54MHZ         (2 << 2)
    #define CKG_VDMCU_XTAL          (3 << 2)

#define REG_CKG_VD200           0x1E2E
    #define CKG_VD200_GATED         BIT4
    #define CKG_VD200_INVERT        BIT5
    #define CKG_VD200_MASK          (BIT7 | BIT6)
    #define CKG_VD200_216MHZ        (0 << 6)
    #define CKG_VD200_216MHZ_       (1 << 6)
    #define CKG_VD200_216MHZ__      (2 << 6)
    #define CKG_VD200_XTAL          (3 << 6)

#define REG_CKG_DHC             0x1E2F
    #define CKG_DHC_GATED           BIT0
    #define CKG_DHC_INVERT          BIT1
    #define CKG_DHC_MASK            (BIT5 | BIT4 | BIT3 | BIT2)
    #define CKG_DHC_12MHZ           (0 << 2)
    #define CKG_DHC_54MHZ           (1 << 2)
    #define CKG_DHC_62MHZ           (2 << 2)
    #define CKG_DHC_72MHZ           (3 << 2)
    #define CKG_DHC_86MHZ           (4 << 2)
    #define CKG_DHC_108MHZ          (5 << 2)
    #define CKG_DHC_0               (6 << 2)
    #define CKG_DHC_0_              (7 << 2)
    #define CKG_DHC_XTAL            (8 << 2)

#define REG_CKG_FICLK_F2        0x1E30
    #define CKG_FICLK_F2_GATED      BIT4
    #define CKG_FICLK_F2_INVERT     BIT5
    #define CKG_FICLK_F2_MASK       (BIT7 | BIT6)
    #define CKG_FICLK_F2_CLK_IDCLK2 (0 << 6)
    #define CKG_FICLK_F2_CLK_FCLK   (1 << 6)
    #define CKG_FICLK_F2_0          (2 << 6)
    #define CKG_FICLK_F2_XTAL       (3 << 6)

#define REG_CKG_GOPG2           0x1E31
    #define CKG_GOPG2_GATED         BIT0
    #define CKG_GOPG2_INVERT        BIT1
    #define CKG_GOPG2_MASK          (BIT3 | BIT2)
    #define CKG_GOPG2_CLK_ODCLK     (0 << 2)
    #define CKG_GOPG2_0             (1 << 2)
    #define CKG_GOPG2_CLK_IDCLK2    (2 << 2)
    #define CKG_GOPG2_XTAL          (3 << 2)

#define REG_CKG_PCM             0x1E31
    #define CKG_PCM_GATED           BIT4
    #define CKG_PCM_INVERT          BIT5
    #define CKG_PCM_MASK            (BIT7 | BIT6)
    #define CKG_PCM_27MHZ           (0 << 6)
    #define CKG_PCM_27MHZ_          (1 << 6)
    #define CKG_PCM_XTAL            (2 << 6)
    #define CKG_PCM_XTAL_           (3 << 6)

#define REG_CKG_VE              0x1E33
    #define CKG_VE_GATED            BIT0
    #define CKG_VE_INVERT           BIT1
    #define CKG_VE_MASK             (BIT3 | BIT2)
    #define CKG_VE_27MHZ            (0 << 2)
    #define CKG_VE_27MHZ_           (1 << 2)
    #define CKG_VE_XTAL             (2 << 2)
    #define CKG_VE_XTAL_            (3 << 2)

#define REG_CKG_VEDAC           0x1E33
    #define CKG_VEDAC_GATED         BIT4
    #define CKG_VEDAC_INVERT        BIT5
    #define CKG_VEDAC_MASK          (BIT7 | BIT6)
    #define CKG_VEDAC_27MHZ         (0 << 6)
    #define CKG_VEDAC_54MHZ         (1 << 6)
    #define CKG_VEDAC_108MHZ        (2 << 6)
    #define CKG_VEDAC_DFT_LIVE      (3 << 6)

#define REG_CKG_DAC             0x1E34
    #define CKG_DAC_GATED           BIT0
    #define CKG_DAC_INVERT          BIT1
    #define CKG_DAC_MASK            (BIT3 | BIT2)
    #define CKG_DAC_CLK_ODCLK       (0 << 2)
    #define CKG_DAC_CLK_VEDAC       (1 << 2)
    #define CKG_DAC_CLK_VD          (2 << 2)
    #define CKG_DAC_XTAL            (3 << 2)

#define REG_CKG_FCLK            0x1E35
    #define CKG_FCLK_GATED          BIT0
    #define CKG_FCLK_INVERT         BIT1
    #define CKG_FCLK_MASK           (BIT5 | BIT4 | BIT3 | BIT2)
    #define CKG_FCLK_MPLL           (0 << 2)
    #define CKG_FCLK_CLK_MIU        (1 << 2)
    #define CKG_FCLK_CLK_ODCLK      (2 << 2)
    #define CKG_FCLK_216MHZ         (3 << 2)
    #define CKG_FCLK_CLK_IDCLK2     (4 << 2)
    #define CKG_FCLK_SCPLL          (5 << 2)
    #define CKG_FCLK_0              (6 << 2)
    #define CKG_FCLK_XTAL           (7 << 2)
    #define CKG_FCLK_XTAL_          (8 << 2)

#define REG_CKG_ODCLK           0x1E37
    #define CKG_ODCLK_GATED         BIT0
    #define CKG_ODCLK_INVERT        BIT1
    #define CKG_ODCLK_MASK          (BIT5 | BIT4 | BIT3 | BIT2)
    #define CKG_ODCLK_CLK_ADC       (0 << 2)
    #define CKG_ODCLK_CLK_DVI       (1 << 2)
    #define CKG_ODCLK_CLK_VD        (2 << 2)
    #define CKG_ODCLK_CLK_MPEG0     (3 << 2)
    #define CKG_ODCLK_1             (4 << 2)
    #define CKG_ODCLK_CLK_EXT_DI    (5 << 2)
    #define CKG_ODCLK_XTAL          (6 << 2)
    #define CKG_ODCLK_CLK_LPLL      (7 << 2)
    #define CKG_ODCLK_XTAL_         (8 << 2)

#define REG_CKG_VE_IN           0x1E38
    #define CKG_VE_IN_GATED         BIT0
    #define CKG_VE_IN_INVERT        BIT1
    #define CKG_VE_IN_MASK          (BIT5 | BIT4 | BIT3 | BIT2)
    #define CKG_VE_IN_CLK_ADC       (0 << 2)
    #define CKG_VE_IN_CLK_DVI       (1 << 2)
    #define CKG_VE_IN_CLK_VD        (2 << 2)
    #define CKG_VE_IN_CLK_MPEG0     (3 << 2)
    #define CKG_VE_IN_1             (4 << 2)
    #define CKG_VE_IN_CLK_EXT_DI    (5 << 2)
    #define CKG_VE_IN_0             (6 << 2)
    #define CKG_VE_IN_0_            (7 << 2)
    #define CKG_VE_IN_DFT_LIVE      (8 << 2)

#define REG_CKG_NFIE            0x1E39
    #define CKG_NFIE_GATED          BIT0
    #define CKG_NFIE_INVERT         BIT1
    #define CKG_NFIE_MASK           (BIT4 | BIT3 | BIT2)
    #define CKG_NFIE_62MHZ          (0 << 2)
    #define CKG_NFIE_54MHZ          (1 << 2)
    #define CKG_NFIE_43MHZ          (2 << 2)
    #define CKG_NFIE_36MHZ          (3 << 2)
    #define CKG_NFIE_27MHZ          (4 << 2)
    #define CKG_NFIE_18MHZ          (5 << 2)
    #define CKG_NFIE_13MHZ          (6 << 2)
    #define CKG_NFIE_XTAL           (7 << 2)

#define REG_CKG_TS2             0x1E3A
    #define CKG_TS2_GATED           BIT0
    #define CKG_TS2_INVERT          BIT1
    #define CKG_TS2_MASK            (BIT3 | BIT2)
    #define CKG_TS2_TS2_CLK         (0 << 2)
    #define CKG_TS2_0               (1 << 2)
    #define CKG_TS2_0_              (2 << 2)
    #define CKG_TS2_XTAL            (3 << 2)

#define REG_CKG_TSOUT           0x1E3A
    #define CKG_TSOUT_GATED         BIT4
    #define CKG_TSOUT_INVERT        BIT5
    #define CKG_TSOUT_MASK          (BIT7 | BIT6)
    #define CKG_TSOUT_27MHZ         (0 << 6)
    #define CKG_TSOUT_36MHZ         (1 << 6)
    #define CKG_TSOUT_43MHZ         (2 << 6)
    #define CKG_TSOUT_XTAL          (3 << 6)

#define REG_CKG_UART            0x1E3C
    #define CKG_UART_GATED          BIT4        // FIXME
    #define CKG_UART_INVERT         BIT5
    #define CKG_UART_MASK           (BIT7 | BIT6)
#define REG_CKG_UART_4          0x1E46
    #define CKG_UART_4_MASK         BIT2
    #define CKG_UART_MPLL_DIV_BUF   (0 << 6)
    #define CKG_UART_160MHZ         (1 << 6)
    #define CKG_UART_144MHZ         (2 << 6)
    #define CKG_UART_123MHZ         (3 << 6)
    #define CKG_UART_108MHZ         (4 << 6)
    #define CKG_UART_MEMPLL         (5 << 6)
    #define CKG_UART_MEMPLL_DIV2    (6 << 6)

#define REG_CKG_DC1             0x1E3D
    #define CKG_DC1_GATED           BIT0
    #define CKG_DC1_INVERT          BIT1
    #define CKG_DC1_MASK            (BIT3 | BIT2)
    #define CKG_DC1_SYNCHRONOUS     (0 << 2)
    #define CKG_DC1_FREERUN         (1 << 2)
    #define CKG_DC1_27MHZ           (2 << 2)
    #define CKG_DC1_DFT_LIVE        (3 << 2)

#define REG_CKG_IDCLK1          0x1E3E
    #define CKG_IDCLK1_GATED        BIT0
    #define CKG_IDCLK1_INVERT       BIT1
    #define CKG_IDCLK1_MASK         (BIT5 | BIT4 | BIT3 | BIT2)
    #define CKG_IDCLK1_CLK_ADC      (0 << 2)
    #define CKG_IDCLK1_CLK_DVI      (1 << 2)
    #define CKG_IDCLK1_CLK_VD       (2 << 2)
    #define CKG_IDCLK1_CLK_DC0      (3 << 2)
    #define CKG_IDCLK1_1            (4 << 2)
    #define CKG_IDCLK1_CLK_EXT_DI   (5 << 2)
    #define CKG_IDCLK1_CLK_VD_ADC   (6 << 2)
    #define CKG_IDCLK1_0            (7 << 2)
    #define CKG_IDCLK1_XTAL         (8 << 2)

#define REG_CKG_IDCLK2          0x1E3F
    #define CKG_IDCLK2_GATED        BIT0
    #define CKG_IDCLK2_INVERT       BIT1
    #define CKG_IDCLK2_MASK         (BIT5 | BIT4 | BIT3 | BIT2)
    #define CKG_IDCLK2_CLK_ADC      (0 << 2)
    #define CKG_IDCLK2_CLK_DVI      (1 << 2)
    #define CKG_IDCLK2_CLK_VD       (2 << 2)
    #define CKG_IDCLK2_CLK_DC0      (3 << 2)
    #define CKG_IDCLK2_1            (4 << 2)
    #define CKG_IDCLK2_CLK_EXT_DI   (5 << 2)
    #define CKG_IDCLK2_CLK_VD_ADC   (6 << 2)
    #define CKG_IDCLK2_0            (7 << 2)
    #define CKG_IDCLK2_XTAL         (8 << 2)

#define REG_CKG_STRLD           0x1E44
    #define CKG_STRLD_GATED         BIT0
    #define CKG_STRLD_INVERT        BIT1
    #define CKG_STRLD_MASK          (BIT3 | BIT2)
    #define CKG_STRLD_144MHZ        (0 << 2)
    #define CKG_STRLD_123MHZ        (1 << 2)
    #define CKG_STRLD_108MHZ        (2 << 2)
    #define CKG_STRLD_XTAL          (3 << 2)

#define REG_CKG_MCU             0x1E45
    #define CKG_MCU_GATED           BIT0
    #define CKG_MCU_INVERT          BIT1
    #define CKG_MCU_MASK            (BIT4 | BIT3 | BIT2)
    #define CKG_MCU_170MHZ          (0 << 2)
    #define CKG_MCU_160MHZ          (1 << 2)
    #define CKG_MCU_144MHZ          (2 << 2)
    #define CKG_MCU_123MHZ          (3 << 2)
    #define CKG_MCU_108MHZ          (4 << 2)
    #define CKG_MCU_CLK_MIU         (5 << 2)
    #define CKG_MCU_CLK_MIU_DIV2    (6 << 2)
    #define CKG_MCU_0               (7 << 2)

#define REG_CKG_DHC_DDR         0x1E58
    #define CKG_DHC_DDR_GATED       BIT0
    #define CKG_DHC_DDR_INVERT      BIT1

#define REG_CKG_DHC_SYNTH       0x1E58
    #define CKG_DHC_SYNTH_GATED     BIT2
    #define CKG_DHC_SYNTH_INVERT    BIT3
    #define CKG_DHC_SYNTH_MASK      (BIT5 | BIT4)
    #define CKG_DHC_SYNTH_MPLL_DIV2     (0 << 4)
    #define CKG_DHC_SYNTH_MPLL_DIV2P5   (1 << 4)
    #define CKG_DHC_SYNTH_MPLL_DIV3     (2 << 4)
    #define CKG_DHC_SYNTH_MPLL_DIV4     (3 << 4)

#define REG_CKG_DHC_MCU         0x1E59
    #define CKG_DHC_MCU_GATED       BIT0
    #define CKG_DHC_MCU_INVERT      BIT1
    #define CKG_DHC_MCU_MASK        (BIT5 | BIT4 | BIT3 | BIT2)
    #define CKG_DHC_MCU_43MHZ       (0 << 2)
    #define CKG_DHC_MCU_54MHZ       (1 << 2)
    #define CKG_DHC_MCU_62MHZ       (2 << 2)
    #define CKG_DHC_MCU_72MHZ       (3 << 2)
    #define CKG_DHC_MCU_86MHZ       (4 << 2)
    #define CKG_DHC_MCU_108MHZ      (5 << 2)
    #define CKG_DHC_MCU_XTAL        (8 << 2)

#define REG_CKG_DHC_LIVE        0x1E59
    #define CKG_DHC_LIVE_GATED      BIT6
    #define CKG_DHC_LIVE_INVERT     BIT7

#define REG_CKG_PSRAM           0x1E5A
    #define CKG_PSRAM_GATED         BIT0
    #define CKG_PSRAM_INVERT        BIT1

#define REG_CKG_MLOAD           0x1E5B
    #define CKG_MLOAD_GATED         BIT4
    #define CKG_MLOAD_INVERT        BIT5
    #define CKG_MLOAD_MASK          (BIT7 | BIT6)
    #define CKG_MLOAD_REG_SEL_SRAM_CLK_MIU  (0 << 6)    // FIXME
    #define CKG_MLOAD_DFT_LIVE              (3 << 6)

#define REG_CKG_MLINK           0x1E97
    #define CKG_MLINK_GATED         BIT4
    #define CKG_MLINK_INVERT        BIT5
    #define CKG_MLINK_MASK          (BIT7 | BIT6)
    #define CKG_MLINK_108MHZ        (0 << 6)
    #define CKG_MLINK_86MHZ         (1 << 6)
    #define CKG_MLINK_54MHZ         (2 << 6)
    #define CKG_MLINK_DFT_LIVE      (3 << 6)

#define REG_CKG_MLINK_L1        0x1E98
    #define CKG_MLINK_L1_GATED      BIT0
    #define CKG_MLINK_L1_INVERT     BIT1

#define REG_CKG_MLINK_TX_VD     0x1E98
    #define CKG_MLINK_TX_VD_GATED   BIT4
    #define CKG_MLINK_TX_VD_INVERT  BIT5
    #define CKG_MLINK_TX_VD_MASK    (BIT7 | BIT6)
    #define CKG_MLINK_TX_VD_CLK_VD  (0 << 6)
    #define CKG_MLINK_TX_VD_CLK_TS  (1 << 6)
    #define CKG_MLINK_TX_54MHZ      (2 << 6)
    #define CKG_MLINK_TX_DFT_LIVE   (3 << 6)

#define REG_CKG_MLINK_RX_AU     0x1E99
    #define CKG_MLINK_RX_AU_GATED   BIT0
    #define CKG_MLINK_RX_AU_INVERT  BIT1
    #define CKG_MLINK_RX_AU_MASK    BIT2
    #define CKG_MLINK_RX_AU_54MHZ    (0 << 2)
    #define CKG_MLINK_RX_AU_DFT_LIVE (1 << 2)

#define REG_CKG_JPD             0x1E9A
    #define CKG_JPD_GATED           BIT0
    #define CKG_JPD_INVERT          BIT1
    #define CKG_JPD_MASK            (BIT3 | BIT2)
    #define CKG_JPD_72MHZ           (0 << 2)
    #define CKG_JPD_108MHZ          (1 << 2)
    #define CKG_JPD_123MHZ          (2 << 2)

#define REG_CKG_SDIO            0x1E9C
    #define CKG_SDIO_GATED          BIT0
    #define CKG_SDIO_INVERT         BIT1
    #define CKG_SDIO_MASK           (BIT6 | BIT5 | BIT4 | BIT3 | BIT2)
    #define CKG_SDIO_CLK86_DIV256   (0  << 2)
    #define CKG_SDIO_CLK86_DIV64    (1  << 2)
    #define CKG_SDIO_CLK86_DIV16    (2  << 2)
    #define CKG_SDIO_CLK54_DIV4     (3  << 2)
    #define CKG_SDIO_CLK72_DIV4     (4  << 2)
    #define CKG_SDIO_CLK86_DIV4     (5  << 2)
    #define CKG_SDIO_CLK54_DIV2     (6  << 2)
    #define CKG_SDIO_CLK72_DIV2     (7  << 2)
    #define CKG_SDIO_CLK86_DIV2     (8  << 2)
    #define CKG_SDIO_54MHZ          (9  << 2)
    #define CKG_SDIO_72MHZ          (10 << 2)
    #define CKG_SDIO_0              (11 << 2)
    #define CKG_SDIO_XTAL           (16 << 2)

// FIXME, ERROR!!! temporary for compile
#define REG_CKG_AEON1DC0                        0x1E5A
#else

#define REG_CKG_USBRIU                          0x1E24
#define REG_CKG_DDRMIU                          0x1E25

#define REG_CKG_STC0TSP                         0x1E27
#define REG_CKG_MADSTC                          0x1E28
#define REG_CKG_MVD                             0x1E29
#define REG_CKG_DC1M4V                          0x1E2A
#define REG_CKG_GEDHCSBM                        0x1E2B
#define REG_CKG_GOPG1                           0x1E2C
#define REG_CKG_VDGOPD                          0x1E2D
#define REG_CKG_VDS                             0x1E2E
#define REG_CKG_HDC                             0x1E2F
#define REG_CKG_FICLK                           0x1E30
#define REG_CKG_PCM                             0x1E31
#define REG_CKG_PCI                             0x1E32
#define REG_CKG_VE                              0x1E33
#define REG_CKG_SDRDAC                          0x1E34
#define REG_CKG_FCLKCCIR                        0x1E35
#define REG_CKG_FMCLK                           0x1E36
#define REG_CKG_ODCLKPSRAM                      0x1E37
#define REG_CKG_VEIN                            0x1E38
#define REG_CKG_FCIE                            0x1E39
//#define REG_CKG_TS                              0x1E3A
#define REG_CKG_GOPG0                        0x1E3A
#define REG_CKG_MIU1GOPTGO                      0x1E3B
#define REG_CKG_UARTSTRLD                       0x1E3C
#define REG_CKG_VIFUART                         0x1E3D
#define REG_CKG_IDCLK1                          0x1E3E
#define REG_CKG_IDCLK2                          0x1E3F
#endif

#define REG_DC0_NUM0                            0x1E40
#define REG_DC0_NUM1                            0x1E41
#define REG_DC0_DEN0                            0x1E42
#define REG_DC0_DEN1                            0x1E43

//#define REG_CKG_STRLD                           0x1E44
//#define REG_CKG_SVD                             0x1E44
//#define REG_CKG_MCU                             0x1E45
//#define REG_CKG_DMA                             0x1E46
//#define REG_CKG_DHC0                            0x1E58
//#define REG_CKG_DHC1                            0x1E59

//#define REG_CKG_AEON1DC0                        0x1E5A
//#define REG_CKG_MLOADJPD                        0x1E5B
//#define REG_CKG_TSOUT                           0x1E5C
//#define REG_CKG_MVDRST                          0x1E5E

#define REG_GPIO_IN_LOW                         0x1E60
#define REG_GPIO_IN_HIGH                        0x1E61
#define REG_GPIO_OUT_LOW                        0x1E62
#define REG_GPIO_OUT_HIGH                       0x1E63
#define REG_GPIO_OE_0                           0x1E64
#define REG_GPIO_OE_1                           0x1E65

// FIXME: gpio is different
#define REG_FCIE_IS_GPIO_IN_0                   0x1E66
#define REG_FCIE_IS_GPIO_IN_1                   0x1E67
#define REG_FCIE_IS_GPIO_OUT_0                  0x1E68
#define REG_FCIE_IS_GPIO_OUT_1                  0x1E69
#define REG_FCIE_IS_GPIO_OE_0                   0x1E6A
#define REG_FCIE_IS_GPIO_OE_1                   0x1E6B

#define REG_TS0_GPIO_IN_LOW                     0x1E6C
#define REG_TS0_GPIO_IN_HIGH                    0x1E6D
#define REG_TS0_GPIO_OUT_LOW                    0x1E6E
#define REG_TS0_GPIO_OUT_HIGH                   0x1E6F
#define REG_TS0_GPIO_OEN_LOW                    0x1E70
#define REG_TS0_GPIO_OEN_HIGH                   0x1E71

#define REG_TS1_GPIO_IN                         0x1E72
#define REG_TS1_GPIO_OUT                        0x1E74
#define REG_TS1_GPIO_OE                         0x1E76

#define REG_DI_GPIO_IN_LOW                      0x1E78
#define REG_DI_GPIO_IN_HIGH                     0x1E79
#define REG_DI_GPIO_OUT_0                       0x1E7A
#define REG_DI_GPIO_OUT_1                       0x1E7B
#define REG_DI_GPIO_OE_LOW                      0x1E7C
#define REG_DI_GPIO_OE_HIGH                     0x1E7D

// 070419 SK GPIO PORT
#define REG_I2S_GPIO_IN_LOW						0x1E7E
#define REG_I2S_GPIO_IN_HIGH					0x1E7F
#define REG_I2S_GPIO_OUT_LOW					0x1E80
#define REG_I2S_GPIO_OUT_HIGH					0x1E81
#define REG_I2S_GPIO_OE_LOW						0x1E82
#define REG_I2S_GPIO_OE_HIGH					0x1E83

#define REG_PCI_GPIO_IN_0                       0x1E84  //pci 0~7
#define REG_PCI_GPIO_IN_1                       0x1E85  //pci 8~f
#define REG_PCI_GPIO_IN_2                       0x1E86  //pci 10~17
#define REG_PCI_GPIO_IN_3                       0x1E87  //pci 18~1f
#define REG_PCI_GPIO_IN_4                       0x1E88  //pci 20~27
#define REG_PCI_GPIO_IN_5                       0x1E89  //pci 28~2f

#define REG_PCI_GPIO_OUT_0                      0x1E8A  //pci 0~7
#define REG_PCI_GPIO_OUT_1                      0x1E8B  //pci 8~f
#define REG_PCI_GPIO_OUT_2                      0x1E8C  //pci 10~17
#define REG_PCI_GPIO_OUT_3                      0x1E8D  //pci 18~1f
#define REG_PCI_GPIO_OUT_4                      0x1E8E  //pci 20~27
#define REG_PCI_GPIO_OUT_5                      0x1E8F  //pci 28~2f

#define REG_PCI_GPIO_OE_0                       0x1E90  //pci 0~7
#define REG_PCI_GPIO_OE_1                       0x1E91  //pci 8~f
#define REG_PCI_GPIO_OE_2                       0x1E92  //pci 10~17
#define REG_PCI_GPIO_OE_3                       0x1E93  //pci 18~1f
#define REG_PCI_GPIO_OE_4                       0x1E94  //pci 20~27
#define REG_PCI_GPIO_OE_5                       0x1E95  //pci 28~2f

#define REG_TCON_GPIO_INOUT                     0x1E96
#define REG_TCON_GPIO_OEN                       0x1E97

#define REG_DO2_GPIO_IN                         0x1E98
#define REG_UART2_GPIO_IN                       0x1E99
#define REG_DO2_GPIO_OUT                        0x1E9A
#define REG_UART2_GPIO_OUT                      0x1E9B
#define REG_DO2_GPIO_OEN                        0x1E9C
#define REG_UART2_GPIO_OEN                      0x1E9D

#define REG_MUX_FUNC_SEL0                       0x1EA0
#define REG_MUX_FUNC_SEL1                       0x1EA1
#define REG_MUX_FUNC_SEL2                       0x1EA2
#define REG_MUX_FUNC_SEL3                       0x1EA3
#define REG_P1_ENABLE                           0x1EA4

#define REG_MUX_FUNC_SEL5                       0x1EA5
#define REG_MUX_FUNC_SEL6                       0x1EA6
#define REG_MUX_FUNC_SEL7                       0x1EA7
#define REG_DOUT_SRC_SEL                        0x1EA9

#define REG_MUX_UART_JTAG                       0x1EAA

//Eris New [BEG]
#define REG_MUX_UART_INV                        0x1EAB

#define REG_BT656PLL_CTRL2_LOW                  0x1EAC
#define REG_BT656PLL_CTRL2_HIGH                 0x1EAD

#define REG_BT656PLL_CTRL1_LOW                  0x1EAE
#define REG_BT656PLL_CTRL1_HIGH                 0x1EAF

#define REG_PVR_IO_CLK_SEL_INV                  0x1EB0
#define REG_PVRI_CLK_SEL                        0x1EB1
#define REG_DOUT2_CLK_SEL                       0x1EB2
#define REG_DOUT3_CLK_SEL                       0x1EB3
#define REG_SC_PLL_CTRL0_LOW                    0x1EB4
#define REG_SC_PLL_CTRL0_HIGH                   0x1EB5
#define REG_SC_PLL_CTRL1_LOW                    0x1EB6
#define REG_SC_PLL_CTRL1_HIGH                   0x1EB7
#define REG_SC_PLL_CTRL2_LOW                    0x1EB8
#define REG_SC_PLL_CTRL2_HIGH                   0x1EB9
#define REG_SC_PLL_CTRL3_LOW                    0x1EBA
#define REG_SC_PLL_CTRL3_HIGH                   0x1EBB
#define REG_BT656_PLL_CTROl_LOW                 0x1EBC
#define REG_BT656_PLL_CTROl_HIGH                0x1EBD
#define REG_TOP_PLL                             0x1EBE
#define REG_DOUT4_CLK_SEL                       0x1EBF
//Eris New [ENG]

#define REG_CHIP_ID_MAJOR                       0x1ECC
#define REG_CHIP_ID_MINOR                       0x1ECD
#define REG_CHIP_VERSION                        0x1ECE
#define REG_CHIP_REVISION                       0x1ECF

#define REG_BOND_OV_EN2                         0x1ED0
#define REG_BOND_OV2                            0x1ED1
#define REG_VDD2LOW_CTRL_MPLL                   0x1ED2
#define REG_GPIO_A_OUTOEN                       0x1ED4
#define REG_GPIO_A_IN                           0x1ED5
//#define REG_CKG_NFIE                            0x1EE7


#define REG_MUX_PCI                             0x1EE0
#define REG_SEL_CZ                              0x1EE5

//--------------------------------------------------
//REG_MUX_CONFIG_0  //0x1E02
#define DI_MODE_IS_BT656        				(BIT3|BIT2)
#define TS0_IS_DI                               BIT5
#define TS0_IS_TS0                              BIT6
#define UTMI_MODE                               BIT7

//-----------------------
//REG_MUX_CONFIG_1  //0x1E03
#define I2S_IN_PCMPADS                          BIT0
#define UTMI_PHY_MODE                           BIT1

//-----------------------
//REG_MUX_CONFIG_2
#if 0
#define        PCI_CONFIG                       (BIT0|BIT1)  //bit0~1
#define        PAD_IRIN                         0x00  //bit3
#define        PAD_INT2                         BIT2  //bit2
#define        PAD_IRIN2                        BIT3  //bit3
#define        TEST_IN_MODE                     (BIT5|BIT4)
#define        TEST_OUT_MODE                    (BIT7|BIT6)
#endif

//-----------------------------------------------
//definition for REG_MUX_CONFIG_3
#define SECOND_UART_MODE                         (BIT0|BIT1)
#define SECOND_UART_MODE_1                       BIT1
#define PCM2CONFIG                               (BIT2|BIT3)

#define I2S_MUTE_MODE                            (BIT7|BIT6)
#define I2S_MUTE_MODE_0                          BIT6
#define I2S_MUTE_MODE_1                          BIT7



#define        PAD_PCMCIA                       0xFC  //bit0~1 clear

//-----------------------------------------------
//definition for REG_MUX_CONFIG_4
#define FCIE_CONFIG                             (BIT7|BIT6)
#define FCIE_MODE_1                             (BIT6)
#define FCIE_MODE_2                             (BIT7)

//-----------------------------------------------
//definition for REG_MUX_CONFIG_5
#define DSPE_JTAG_MODE                          (BIT3|BIT2)
#define PWM0_ENABLE                             BIT4
#define PWM1_ENABLE                             BIT5
#define PWM2_ENABLE                             BIT6
#define PWM3_ENABLE                             BIT7






//--------------------------------------------------
// REG_MUX_FUNC_SEL0    reg[1ea0]
#define PWM2_SETTING                            BIT0
#define PWM3_SETTING                            BIT1
#define ATCON_SETTING                           (BIT2|BIT3)
#define ATCON_MODE_1                            BIT2
#define ATCON_MODE_2                            BIT3
#define DTCON_SETTING                           (BIT4|BIT5)
#define TCON_IS_GPIO                            (BIT6)

//--------------------------------------------------
//REG_MUX_FUNC_SEL1 //reg[1ea1]
#define PAD_IIC2                                BIT1
#define CEC_MODE                                (BIT3|BIT4)


//--------------------------------------------------
//definition for REG_MUX_FUNC_SEL2  //reg[1ea2]
#define PWM0_IS_GPIO                            0x01
#define PWM1_IS_GPIO                            0x02
#define PWM2_IS_GPIO                            0x04
#define PWM3_IS_GPIO                            0x08
#define CI_PCM_A4TOA14_IS_GPIO_0                0x10
#define CI_PCM_A4TOA14_IS_GPIO_1                0x20
#define IR_IN_IS_GPIO                           0x40
#define INT_IS_GPIO                             0x80

//-------------------------------------------
//definition for REG_MUX_FUNC_SEL3   //reg[1ea3]
#define IR_IN2_IS_GPIO                          BIT0
#define INT2_IS_GPIO                            BIT1
#define TS0_IS_GPIO                             BIT2
#define TS1_IS_GPIO                             BIT3
#define DI_IS_GPIO                              BIT4
#define I2S_IS_GPIO                             BIT5
#define FLHWE_IS_GPIO                           BIT6
#define FCIE_IS_GPIO                            BIT7
//--------------------------------------------------
//definition for REG_MUX_FUNC_SEL5 //reg[1ea5]
#define PCM_DUP_USE                             BIT0
#define I2S_D2PADSETTING                        (BIT1|BIT2)
#define I2S_D2PADSETTING_MODE1                  (BIT1)
#define I2S_D2PADSETTING_MODE2                  (BIT2)

//--------------------------------------------------
//REG_MUX_FUNC_SEL6
#define DHC_DFT_MODE                            BIT1

//--------------------------------------------------
// REG_MUX_UART_JTAG
#define JTAG_SEL_MASK                          (BIT0|BIT1)
#define JTAG_FOR_51                            0x00
#define JTAG_FOR_AEON                          BIT0
#define JTAG_FOR_TSP                           BIT1

#define UART0_SEL_MASK                         (BIT2|BIT3|BIT4)
#define UART0_SEL_51_UART0                     0x00
#define UART0_SEL_51_UART1                     BIT2
#define UART0_SEL_VD_UART0                     BIT3
#define UART0_SEL_AEON                         (BIT2|BIT3)
#define UART0_SEL_TSP                          BIT4

#define UART1_SEL_MASK                         (BIT5|BIT6|BIT7)
#define UART1_SEL_51_UART0                     0x00
#define UART1_SEL_51_UART1                     BIT5
#define UART1_SEL_VD_UART0                     BIT6
#define UART1_SEL_AEON                         (BIT5|BIT6)
#define UART1_SEL_TSP                          (BIT7)

//--------------------------------------------------
//definition for REG_MUX_PCI   //reg[1ee0]
#define PCI_GPIO_0                              BIT0
#define PCI_GPIO_1                              BIT1
#define PCI_GPIO_2                              BIT2
#define PCI_GPIO_3                              BIT3
#define PCI_GPIO_4                              BIT4
#define PCI_GPIO_5                              BIT5

// MISC
//------------------------------------------------------------------------------
// definition for REG_CT_MISC0      //reg[0x1e22]
#define MISC_MCU_CLKSEL_MASK                    0x01    // MASK
#define MISC_MCU_CLKSEL_DFT                     0x00
#define MISC_MCU_CLKSEL_EX                      0x01
#define MISC_VD_CLKSEL_MASK                     0x02    // MASK
#define MISC_VD_CLKSEL_VDADC                    0x00
#define MISC_VD_CLKSEL_ADC                      0x02
#define MISC_DE_ONLY_F1                         0x04
#define MISC_DE_ONLY_F2_SCTOP                   0x08
#define MISC_USBSYN_RST                         0x10
#define MISC_STC0SYN_RST                        0x20
#define MISC_STC0CWSEL_MASK                     0x80    // MASK
#define MISC_STC0CWSEL_HKMCU                    0x00
#define MISC_STC0CWSEL_TSP                      0x80
//--------------------------------------------------

// definition for REG_CT_MISC1      //reg[0x1e23]
#define MISC_UPDATE_STC0CW                      0x02
#define MISC_UPDATE_DC0CW_FRUN                  0x08
#define MISC_UPDATE_DC0CW_SYNC                  0x40

// Clock generate setting
//---------------------------------------------
// definition for REG_CKG_USBRIU    //reg[0x1e24]
#define USBRIU_CLK_ENABLE                       0x00
#define USBRIU_CLK_DISABLE                      0X01
#define USBRIU_CLK_NONINVERT                    0x00
#define USBRIU_CLK_INVERT                       0X02

//---------------------------------------------
// definition for REG_CKG_DDRMIU    //reg[0x1e25]
#define DDR_CLK_ENABLE                          0x00
#define DDR_CLK_DISABLE                         0x01
#define DDR_CLK_NONINVERT                       0x00
#define DDR_CLK_INVERT                          0x02
#define DDR_CLK_MASK                            0x0C
#define DDR_CLK_200_266MHZ                      0x00
#define DDR_CLK_170MHZ                          0x04
#define DDR_CLK_123MHZ                          0x08
#define DDR_CLK_108MHZ                          0x0C
//----------------------------------
#define MIU_CLK_ENABLE                          0x00
#define MIU_CLK_DISABLE                         0x10
#define MIU_CLK_NONINVERT                       0x00
#define MIU_CLK_INVERT                          0x20
#define MIU_CLK_MASK                            0xC0
#define MIU_CLK_MEMPLL_OUT                      0x00
#define MIU_CLK_170MHZ                          0x40
#define MIU_CLK_MEMPLL_OUT_DIV2                 0x80
#define MIU_CLK_216MHZ                          0xC0

//---------------------------------------------
// definition for REG_CKG_AEONTS0 //reg[0x1e26]
#define TS0_CLK_ENABLE                          0x00
#define TS0_CLK_DISABLE                         0x01
#define TS0_CLK_NONINVERT                       0x00
#define TS0_CLK_INVERT                          0x02
#define TS0_CLK_MASK                            0x0C
#define TS0_CLK_TS0                             0x00
#define TS0_CLK_TS2                             0x04
#define TS0_CLK_0                               0x08
#define TS0_CLK_XTAL                            0x0C

//----------------------------------
#define TCK_CLK_ENABLE                          0x00
#define TCK_CLK_DISABLE                         0x10
#define TCK_CLK_NONINVERT                       0x00
#define TCK_CLK_INVERT                          0x20

//----------------------------------
#define AEON_CLK_ENABLE                         0x00
#define AEON_CLK_DISABLE                        0x40
#define AEON_CLK_NONINVERT                      0x00
#define AEON_CLK_INVERT                         0x80

//---------------------------------------------
// definition for REG_CKG_STC0TSP //reg[0x1e27]
#define TSP_CLK_ENABLE                          0x00
#define TSP_CLK_DISABLE                         0x01
#define TSP_CLK_NONINVERT                       0x00
#define TSP_CLK_INVERT                          0x02
#define TSP_CLK_MASK                            0x0C
#define TSP_CLK_144MHZ                          0x00
#define TSP_CLK_123MHZ                          0x04
#define TSP_CLK_72MHZ                           0x08
#define TSP_CLK_XTAL                            0x0C

//----------------------------------
#define STC0_CLK_ENABLE                         0x00
#define STC0_CLK_DISABLE                        0x10
#define STC0_CLK_NONINVERT                      0x00
#define STC0_CLK_INVERT                         0x20
#define STC0_CLK_MASK                           0xC0
#define STC0_CLK_STC0                           0x00
#define STC0_CLK_1                              0x40
#define STC0_CLK_27MHZ                          0x80
#define STC0_CLK_XTAL                           0xC0

//---------------------------------------------
// definition for REG_CKG_MADSTC //reg[0x1e28]
#define MADSTC_CLK_ENABLE                       0x00
#define MADSTC_CLK_DISABLE                      0x10
#define MADSTC_CLK_NONINVERT                    0x00
#define MADSTC_CLK_INVERT                       0x20
#define MADSTC_CLK_MASK                         0xC0
#define MADSTC_CLK_STC0                         0x00
#define MADSTC_CLK_1                            0x40
#define MADSTC_CLK_27MHZ                        0x80
#define MADSTC_CLK_XTAL                         0xC0

//---------------------------------------------
// definition for REG_CKG_MVD   //reg[0x1e29]
#define MVDBOOT_CLK_ENABLE                      0x00
#define MVDBOOT_CLK_DISABLE                     0x01
#define MVDBOOT_CLK_NONINVERT                   0x00
#define MVDBOOT_CLK_INVERT                      0x02
#define MVDBOOT_CLK_MASK                        0x0C
#define MVDBOOT_CLK_MVD                         0x00
#define MVDBOOT_CLK_MIU                         0x04
#define MVDBOOT_CLK_NONE                        0x08
#define MVDBOOT_CLK_XTAL                        0x0C

//----------------------------------
#define MVD_CLK_ENABLE                          0x00
#define MVD_CLK_DISABLE                         0x10
#define MVD_CLK_NONINVERT                       0x00
#define MVD_CLK_INVERT                          0x20
#define MVD_CLK_144MHZ                          0x00
#define MVD_CLK_123MHZ                          0x40
#define MVD_CLK_108MHZ                          0x80
#define MVD_CLK_MIU                             0xC0

//---------------------------------------------
// definition for REG_CKG_DC1M4V //reg[0x1e2a]
#define M4VD_CLK_ENABLE                         0x00
#define M4VD_CLK_DISABLE                        0x01
#define M4VD_CLK_NONINVERT                      0x00
#define M4VD_CLK_INVERT                         0x02
#define M4VD_CLK_MASK                           0x0C
#define M4VD_CLK_144MHZ                         0x00
#define M4VD_CLK_123MHZ                         0x04
#define M4VD_CLK_108MHZ                         0x08
#define M4VD_CLK_XTAL                           0x0C

//----------------------------------
#define DC1_CLK_ENABLE                          0x00
#define DC1_CLK_DISABLE                         0x10
#define DC1_CLK_NONINVERT                       0x00
#define DC1_CLK_INVERT                          0x20
#define DC1_CLK_MASK                            0xC0
#define DC1_CLK_SYNC                            0x00
#define DC1_CLK_FREERUN                         0x40
#define DC1_CLK_27MHZ                           0x80
#define DC1_CLK_XTAL                            0xC0

//---------------------------------------------
// definition for REG_CKG_GEDHCSBM //reg[0x1e2b]
#define DHCSBM_CLK_ENABLE                       0x00
#define DHCSBM_CLK_DISABLE                      0x01
#define DHCSBM_CLK_NONINVERT                    0x00
#define DHCSBM_CLK_INVERT                       0x02
#define DHCSBM_CLK_MASK                         0x0C
#define DHCSBM_CLK_12MHZ                        0x00
#define DHCSBM_CLK_36MHZ                        0x04
#define DHCSBM_CLK_62MHZ                        0x08
#define DHCSBM_CLK_XTAL                         0x0C

//--------------------------------------
#define GE_CLK_ENABLE                           0x00
#define GE_CLK_DISABLE                          0x10
#define GE_CLK_NONINVERT                        0x00
#define GE_CLK_INVERT                           0x20
#define GE_CLK_MASK                             0xC0
#define GE_CLK_123MHZ                           0x00
#define GE_CLK_108MHZ                           0x40
#define GE_CLK_86MHZ                            0x80
#define GE_CLK_XTAL                             0xC0

//---------------------------------------------
// definition for REG_CKG_GOP //reg[0x1e2c]
#define GOP1_CLK_ENABLE                         0x00
#define GOP1_CLK_DISABLE                        0x01
#define GOP1_CLK_NONINVERT                      0x00
#define GOP1_CLK_INVERT                         0x02
#define GOP1_CLK_MASK                           0x1C
#define GOP1_CLK_ODCLK                          0x00
#define GOP1_CLK_IDCLK2                         0x04
#define GOP1_CLK_IDCLK2_GATE                    0x08
#define GOP1_CLK_GOPDG                          0x0C
#define GOP1_CLK_DC0                            0x10
#define GOP1_CLK_0                              0x14

//---------------------------------------------
// definition for REG_CKG_VDGOPD //reg[0x1e2d]
#define GOPD_CLK_ENABLE                         0x00
#define GOPD_CLK_DISABLE                        0x01
#define GOPD_CLK_NONINVERT                      0x00
#define GOPD_CLK_INVERT                         0x02
#define GOPD_CLK_MASK                           0x0C
#define GOPD_CLK_ADC                            0x00
#define GOPD_CLK_ODCLK                          0x04
#define GOPD_CLK_DC0                            0x08
#define GOPD_CLK_XTAL                           0x0C

//----------------------------------
#define VD_CLK_ENABLE                           0x00
#define VD_CLK_DISABLE                          0x10
#define VD_CLK_NONINVERT                        0x00
#define VD_CLK_INVERT                           0x20
#define VD_CLK_MASK                             0xC0
#define VD_CLK_VD0                              0x00
#define VD_CLK_VD1                              0x40
#define VD_CLK_TESTMODE                         0x80
#define VD_CLK_XTAL                             0xC0

//---------------------------------------------
// definition for REG_CKG_VDS //reg[0x1e2e]
#define VDMCU_CLK_ENABLE                        0x00
#define VDMCU_CLK_DISABLE                       0x01
#define VDMCU_CLK_NONINVERT                     0x00
#define VDMCU_CLK_INVERT                        0x02
#define VDMCU_CLK_MASK                          0x0C
#define VDMCU_CLK_108MHZ                        0x00
#define VDMCU_CLK_86MHZ                         BIT2
#define VDMCU_CLK_54MHZ                         BIT3
#define VDMCU_CLK_XTAL                         (BIT3|BIT2)
//----------------------------------
#define VD200_CLK_ENABLE                        0x00
#define VD200_CLK_DISABLE                       0x10
#define VD200_CLK_NONINVERT                     0x00
#define VD200_CLK_INVERT                        0x20
#define VD200_CLK_MASK                          0xC0
#define VD200_CLK_216MHZ1                       0x00
#define VD200_CLK_216MHZ2                       BIT6
#define VD200_CLK_216MHZ3                       BIT7
#define VD200_CLK_XTAL                         (BIT7|BIT6)

//---------------------------------------------
// definition for REG_CKG_HDC //reg[0x1e2f]
#define HDC_CLK_ENABLE                          0x00
#define HDC_CLK_DISABLE                         0x01
#define HDC_CLK_NONINVERT                       0x00
#define HDC_CLK_INVERT                          0x02
#define HDC_CLK_MASK                            0x3C
#define HDC_CLK_12MHZ                           0x00
#define HDC_CLK_54MHZ                           BIT2
#define HDC_CLK_62MHZ                           BIT3
#define HDC_CLK_72MHZ                          (BIT3|BIT2)
#define HDC_CLK_86MHZ                           BIT4
#define HDC_CLK_108MHZ                         (BIT4|BIT2)
#define HDC_CLK_0_0                            (BIT4|BIT3)
#define HDC_CLK_0_1                            (BIT4|BIT3|BIT2)
#define HDC_CLK_XTAL                            BIT5

//---------------------------------------------
// definition for REG_CKG_FICLK //reg[0x1e30]
#define FICLKF2_CLK_ENABLE                      0x00
#define FICLKF2_CLK_DISABLE                     0x10
#define FICLKF2_CLK_NONINVERT                   0x00
#define FICLKF2_CLK_INVERT                      0x20
#define FICLKF2_CLK_MASK                        0xC0
#define FICLKF2_CLK_IDCLK2                      0x00
#define FICLKF2_CLK_FCLK                        BIT6
#define FICLKF2_CLK_0                           BIT7
#define FICLKF2_CLK_XTAL                       (BIT7|BIT6)

//---------------------------------------------
// definition for REG_CKG_PCM   //reg[0x1e31]
#define PCM_CLK_ENABLE                          0x00
#define PCM_CLK_DISABLE                         0x10
#define PCM_CLK_NONINVERT                       0x00
#define PCM_CLK_INVERT                          0x20
#define PCM_CLK_MASK                            0xC0
#define PCM_CLK_27MHZ_0                         0x00
#define PCM_CLK_27MHZ_1                         BIT6
#define PCM_CLK_XTAL_0                          BIT7
#define PCM_CLK_XTAL_1                         (BIT7|BIT6)

//---------------------------------------------
// definition for REG_CKG_VE  //reg[0x1e33]
#define VE_CLK_ENABLE                           0x00
#define VE_CLK_DISABLE                          0x01
#define VE_CLK_NONINVERT                        0x00
#define VE_CLK_INVERT                           0x02
#define VE_CLK_MASK                             0x0C
#define VE_CLK_27MHZ_0                          0x00
#define VE_CLK_27MHZ_1                          BIT2
#define VE_CLK_XTAL_0                           BIT3
#define VE_CLK_XTAL_1                          (BIT3|BIT2)

//----------------------------------
#define VEDAC_CLK_ENABLE                        0x00
#define VEDAC_CLK_DISABLE                       0x10
#define VEDAC_CLK_NONINVERT                     0x00
#define VEDAC_CLK_INVERT                        0x20
#define VEDAC_CLK_MASK                          0xC0
#define VEDAC_CLK_27MHZ                         0x00
#define VEDAC_CLK_54MHZ                         BIT6
#define VEDAC_CLK_108MHZ                        BIT7
#define VEDAC_CLK_DFT                          (BIT7|BIT6)

//---------------------------------------------
// definition for REG_CKG_SDRDAC //reg[0x1e34]
#define DAC_CLK_ENABLE                          0x00
#define DAC_CLK_DISABLE                         0x01
#define DAC_CLK_NONINVERT                       0x00
#define DAC_CLK_INVERT                          0x02
#define DAC_CLK_MASK                            0x0C
#define DAC_CLK_ODCLK                           0x00
#define DAC_CLK_VEDAC                           BIT2
#define DAC_CLK_VD                              BIT3
#define DAC_CLK_XTAL                           (BIT3|BIT2)

//---------------------------------------------
// definition for REG_CKG_FCLK  //reg[0x1e35]
#define FCLK_CLK_ENABLE                         0x00
#define FCLK_CLK_DISABLE                        0x01
#define FCLK_CLK_NONINVERT                      0x00
#define FCLK_CLK_INVERT                         0x02
#define FCLK_CLK_MASK                           0x3C
#define FCLK_CLK_MPLL                           0x00
#define FCLK_CLK_MIU                            BIT2
#define FCLK_CLK_ODCLK                          BIT3
#define FCLK_CLK_0_3                           (BIT3|BIT2)
#define FCLK_CLK_IDCLK2                         BIT4
#define FCLK_CLK_0_1                           (BIT4|BIT2)
#define FCLK_CLK_0_2                           (BIT4|BIT3)
#define FCLK_CLK_XTAL_0                        (BIT4|BIT3|BIT2)
#define FCLK_CLK_XTAL_1                         BIT5

//----------------------------------
#define CCIR_CLK_ENABLE                         0x00
#define CCIR_CLK_DISABLE                        BIT6
#define CCIR_CLK_NONINVERT                      0x00
#define CCIR_CLK_INVERT                         BIT7

//---------------------------------------------
// definition for REG_CKG_FMCLK //reg[0x1e36]
#define FMCLK_CLK_ENABLE                        0x00
#define FMCLK_CLK_DISABLE                       0x01
#define FMCLK_CLK_NONINVERT                     0x00
#define FMCLK_CLK_INVERT                        0x02
#define FMCLK_CLK_MASK                          0x3C
#define FMCLK_CLK_MIU                           0x00
#define FMCLK_CLK_MPLL                          BIT2
#define FMCLK_CLK_ODCLK                         BIT3
#define FMCLK_CLK_0_0                          (BIT3|BIT2)
#define FMCLK_CLK_IDCLK2                        BIT4
#define FMCLK_CLK_0_1                          (BIT4|BIT2)
#define FMCLK_CLK_0_2                          (BIT4|BIT3)
#define FMCLK_CLK_0_3                          (BIT4|BIT3|BIT2)
#define FMCLK_CLK_DFT                           BIT5

//---------------------------------------------
// definition for REG_CKG_ODCLK //reg[0x1e37]
#define ODCLK_CLK_ENABLE                        0x00
#define ODCLK_CLK_DISABLE                       0x01
#define ODCLK_CLK_NONINVERT                     0x00
#define ODCLK_CLK_INVERT                        0x02
#define ODCLK_CLK_MASK                          0x3C
#define ODCLK_CLK_0_0                           0x00
#define ODCLK_CLK_0_1                           BIT2
#define ODCLK_CLK_0_2                           BIT3
#define ODCLK_CLK_0_3                          (BIT3|BIT2)
#define ODCLK_CLK_1                             BIT4
#define ODCLK_CLK_EX_DI                        (BIT4|BIT2)
#define ODCLK_CLK_XTAL_0                       (BIT4|BIT3)
#define ODCLK_CLK_LPLL                         (BIT4|BIT3|BIT2)
#define ODCLK_CLK_XTAL_1                        BIT5

//----------------------------------
#define PSRAM_CLK_ENABLE                         0x00
#define PSRAM_CLK_DISABLE                        BIT6
#define PSRAM_CLK_NONINVERT                      0x00
#define PSRAM_CLK_INVERT                         BIT7

//---------------------------------------------
// definition for REG_CKG_VEIN  //reg[0x1e38]
#define VEIN_CLK_ENABLE                         0x00
#define VEIN_CLK_DISABLE                        0x01
#define VEIN_CLK_NONINVERT                      0x00
#define VEIN_CLK_INVERT                         0x02
#define VEIN_CLK_MASK                           0x3C
#define VEIN_CLK_ADC                            0x00
#define VEIN_CLK_DVI                            BIT2
#define VEIN_CLK_VD                             BIT3
#define VEIN_CLK_MPEG0                         (BIT3|BIT2)
#define VEIN_CLK_1                              BIT4
#define VEIN_CLK_EX_DI                         (BIT4|BIT2)
#define VEIN_CLK_0_0                           (BIT4|BIT3)
#define VEIN_CLK_0_1                           (BIT4|BIT3|BIT2)
#define VEIN_CLK_DFT                            BIT5

//---------------------------------------------
// definition for REG_CKG_FCIE  //reg[0x1e39]
#define FCIE_CLK_ENABLE                         0x00
#define FCIE_CLK_DISABLE                        0x01
#define FCIE_CLK_NONINVERT                      0x00
#define FCIE_CLK_INVERT                         0x02
#define FCIE_CLK_MASK                           0x7C
#define FCIE_CLK_86D256                         0x00
#define FCIE_CLK_86D64                          BIT2
#define FCIE_CLK_86D16                          BIT3
#define FCIE_CLK_54D4                          (BIT3|BIT2)
#define FCIE_CLK_72D4                           BIT4
#define FCIE_CLK_86D4                          (BIT4|BIT2)
#define FCIE_CLK_54D2                          (BIT4|BIT3)
#define FCIE_CLK_72D2                          (BIT4|BIT3|BIT2)
#define FCIE_CLK_86D2                           BIT5
#define FCIE_CLK_54MHZ                         (BIT5|BIT2)
#define FCIE_CLK_72MHZ                         (BIT5|BIT3)
#define FCIE_CLK_0_0                           (BIT5|BIT3|BIT2)
#define FCIE_CLK_0_1                           (BIT5|BIT4)
#define FCIE_CLK_0_2                           (BIT5|BIT4|BIT2)
#define FCIE_CLK_0_3                           (BIT5|BIT4|BIT3)
#define FCIE_CLK_0_4                           (BIT5|BIT4|BIT3|BIT2)
#define FCIE_CLK_XTAL                           BIT6

//---------------------------------------------
// definition for REG_CKG_GOP0    //reg[0x1e3a]
#define GOP0_CLK_ENABLE                         0x00
#define GOP0_CLK_DISABLE                        0x01
#define GOP0_CLK_NONINVERT                      0x00
#define GOP0_CLK_INVERT                         0x02
#define GOP0_CLK_MASK                           0x1C
#define GOP0_CLK_ODCLK                          0x00
#define GOP0_CLK_IDCLK2                         BIT2
#define GOP0_CLK_IDCLK2_GATE                    BIT3
#define GOP0_CLK_GOPDG                         (BIT3|BIT2)
#define GOP0_CLK_DC0                            BIT4
#define GOP0_CLK_0                             (BIT4|BIT2)

//----------------------------------
#define USB_CLK_ENABLE                          0x00
#define USB_CLK_DISABLE                         0x20
#define USB_CLK_NONINVERT                       0x00
#define USB_CLK_INVERT                          0x40
#define USB_CLK_MASK                            0x80
#define USB_CLK_62MHZ                           0x00
#define USB_CLK_XTALI                           0x80

//---------------------------------------------
// definition for REG_CKG_GOPTGO //reg[0x1e3b]
#define GOPTGO_CLK_ENABLE                       0x00
#define GOPTGO_CLK_DISABLE                      0x01
#define GOPTGO_CLK_NONINVERT                    0x00
#define GOPTGO_CLK_INVERT                       0x02
#define GOPTGO_CLK_MASK                         0x0C
#define GOPTGO_CLK_54MHZ                        0x00
#define GOPTGO_CLK_72MHZ                        BIT2
#define GOPTGO_CLK_86MHZ                        BIT3
#define GOPTGO_CLK_108MHZ                      (BIT3|BIT2)

//---------------------------------------------
#define MIU1_CLK_ENABLE                         0x00
#define MIU1_CLK_DISABLE                        BIT4
#define MIU1_CLK_NONINVERT                      0x00
#define MIU1_CLK_INVERT                         BIT5

//---------------------------------------------
// definition for REG_CKG_STRLD //reg[0x1e3c]
#define STRLD_CLK_ENABLE                        0x00
#define STRLD_CLK_DISABLE                       BIT0
#define STRLD_CLK_NONINVERT                     0x00
#define STRLD_CLK_INVERT                        BIT1
#define STRLD_CLK_MASK                          0x0C
#define STRLD_CLK_144MHZ                        0x00
#define STRLD_CLK_123MHZ                        BIT2
#define STRLD_CLK_108MHZ                        BIT3
#define STRLD_CLK_XTAL                         (BIT3|BIT2)

//---------------------------------------------
#define UART_CLK_ENABLE                         0x00
#define UART_CLK_DISABLE                        BIT4
#define UART_CLK_NONINVERT                      0x00
#define UART_CLK_INVERT                         BIT5
#define UART_CLK_MASK_L                         0xC0
#define UART_CLK_MASK_H                         0x01
#define UART_CLK_MPLL_DIVBUF                    0x00
#define UART_CLK_160MHZ                         BIT6
#define UART_CLK_144MHZ                         BIT7
#define UART_CLK_123MHZ                        (BIT7|BIT6)
#define UART_CLK_108MHZ                         0x00
#define UART_CLK_MEMPLL_BUF                     BIT6
#define UART_CLK_MEMPLL_BUF_DIV                 BIT7
#define UART_CLK_0                             (BIT7|BIT6)

//---------------------------------------------
// definition for REG_CKG_VIF //reg[0x1e3d]
#define VIF86M_CLK_ENABLE                       0x00
#define VIF86M_CLK_DISABLE                      BIT2
#define VIF86M_CLK_NONINVERT                    0x00
#define VIF86M_CLK_INVERT                       BIT3
#define VIF86M_CLK_MASK                         BIT4
#define VIF86M_CLK_86MHZ                        0x00
#define VIF86M_CLK_XTAL                         BIT4

//---------------------------------------------
#define VIF43M_CLK_ENABLE                       0x00
#define VIF43M_CLK_DISABLE                      BIT5
#define VIF43M_CLK_NONINVERT                    0x00
#define VIF43M_CLK_INVERT                       BIT6
#define VIF43M_CLK_MASK                         BIT7
#define VIF43M_CLK_43MHZ                        0x00
#define VIF43M_CLK_XTAL                         BIT7

//---------------------------------------------
// definition for REG_CKG_IDCLK1  //reg[0x1e3e]
#define IDCLK1_CLK_ENABLE                       0x00
#define IDCLK1_CLK_DISABLE                      BIT0
#define IDCLK1_CLK_NONINVERT                    0x00
#define IDCLK1_CLK_INVERT                       BIT1
#define IDCLK1_CLK_MASK                        (BIT5|BIT4|BIT3|BIT2)
#define IDCLK1_CLK_ADC                          0x00
#define IDCLK1_CLK_DVI                          BIT2
#define IDCLK1_CLK_VD                           BIT3
#define IDCLK1_CLK_DC0                         (BIT3|BIT2)
#define IDCLK1_CLK_1                            BIT4
#define IDCLK1_CLK_EX_DI                       (BIT4|BIT2)
#define IDCLK1_CLK_VDADC                       (BIT4|BIT3)
#define IDCLK1_CLK_0                           (BIT4|BIT3|BIT2)
#define IDCLK1_CLK_XTAL                         BIT5

//---------------------------------------------
// definition for REG_CKG_IDCLK2  //reg[0x1e3f]
#define IDCLK2_CLK_ENABLE                       0x00
#define IDCLK2_CLK_DISABLE                      BIT0
#define IDCLK2_CLK_NONINVERT                    0x00
#define IDCLK2_CLK_INVERT                       BIT1
#define IDCLK2_CLK_MASK                        (BIT5|BIT4|BIT3|BIT2)
#define IDCLK2_CLK_ADC                          0x00
#define IDCLK2_CLK_DVI                          BIT2
#define IDCLK2_CLK_VD                           BIT3
#define IDCLK2_CLK_DC0                         (BIT3|BIT2)
#define IDCLK2_CLK_1                            BIT4
#define IDCLK2_CLK_EX_DI                       (BIT4|BIT2)
#define IDCLK2_CLK_VDADC                       (BIT4|BIT3)
#define IDCLK2_CLK_0                           (BIT4|BIT3|BIT2)
#define IDCLK2_CLK_XTAL                         BIT5

//---------------------------------------------
// definition for REG_CKG_SVD //reg[0x1E44]
#define SVD_CLK_ENABLE                          0x00
#define SVD_CLK_DISABLE                         BIT3
#define SVD_CLK_NONINVERT                       0x00
#define SVD_CLK_INVERT                          BIT4
#define SVD_CLK_MASK                           (BIT7|BIT6|BIT5)
#define SVD_CLK_250MHZ                          0X00
#define SVD_CLK_240MHZ                          BIT5
#define SVD_CLK_216MHZ                          BIT6
#define SVD_CLK_SCPLL                          (BIT6|BIT5)
#define SVD_CLK_MIU                             BIT7
#define SVD_CLK_144MHZ                         (BIT7|BIT5)
#define SVD_CLK_123MHZ                         (BIT7|BIT6)
#define SVD_CLK_108MHZ                         (BIT7|BIT6|BIT5)

//---------------------------------------------
// definition for REG_CKG_MCU   //reg[0x1e45]
// MPLL   = XTAL * 36
// USBCLK = XTAL * 40
#define MCU_CLK_ENABLE                          0x00
#define MCU_CLK_DISABLE                         0x01
#define MCU_CLK_NONINVERT                       0x00
#define MCU_CLK_INVERT                          0x02
#define MCU_CLK_MASK                            0x1C
#define MCU_CLK_172P8MHZ                        0x00    // MPLL   / 2.5
#define MCU_CLK_160MHZ                          0x04    // USBCLK / 3
#define MCU_CLK_144MHZ                          0x08    // MPLL
#define MCU_CLK_123MHZ                          0x0C    // No function
#define MCU_CLK_108MHZ                          0x10    // No function
#define MCU_CLK_MEM                             0x14    // MIUCLK
#define MCU_CLK_MEMD2                           0x18    // MIUCLK / 2
#define MCU_CLK_0                               0x1C

//---------------------------------------------
// definition for REG_CKG_DMA   //reg[0x1e46]
#define DMA_R_CLK_ENABLE                        0x00
#define DMA_R_CLK_DISABLE                       0x01
#define DMA_R_CLK_NONINVERT                     0x00
#define DMA_R_CLK_INVERT                        0x02
#define DMA_R_CLK_MASK                          0x0C
#define DMA_R_CLK_MIU                           0x00
#define DMA_R_CLK_0                             BIT3
#define DMA_R_CLK_XTALI                        (BIT3|BIT2)

//---------------------------------------------
#define DMA_W_CLK_ENABLE                        0x00
#define DMA_W_CLK_DISABLE                       0x10
#define DMA_W_CLK_NONINVERT                     0x00
#define DMA_W_CLK_INVERT                        0x20
#define DMA_W_CLK_MASK                          0xC0
#define DMA_W_CLK_MIU                           0x00
#define DMA_W_CLK_0                             BIT7
#define DMA_W_CLK_XTALI                        (BIT7|BIT6)

//---------------------------------------------
// definition for REG_CKG_DHC0  //reg[0x1e58]
#define DHCDDR_CLK_NONGATE                      0x00
#define DHCDDR_CLK_GATING                       0x01
#define DHCDDR_CLK_NONINVERT                    0x00
#define DHCDDR_CLK_INVERT                       0x02
#define DHCSYNTH_CLK_NONGATE                    0x00
#define DHCSYNTH_CLK_GATING                     0x04
#define DHCSYNTH_CLK_NONINVERT                  0x00
#define DHCSYNTH_CLK_INVERT                     0x08
#define DHCSYNTH_CLK_MASK                       0x30
#define DHCSYNTH_CLK_D2                         0x00
#define DHCSYNTH_CLK_D2P5                       0x10
#define DHCSYNTH_CLK_D3                         0x20
#define DHCSYNTH_CLK_D4                         0x30

//---------------------------------------------
// definition for REG_CKG_DHC1  //reg[0x1e59]
#define DHCMCU_CLK_NONGATE                      0x00
#define DHCMCU_CLK_GATING                       0x01
#define DHCMCU_CLK_NONINVERT                    0x00
#define DHCMCU_CLK_INVERT                       0x02
#define DHCMCU_CLK_MASK                         0x3C
#define DHCMCU_CLK_43MHZ                        0x00
#define DHCMCU_CLK_54MHZ                        0x04
#define DHCMCU_CLK_62MHZ                        0x08
#define DHCMCU_CLK_72MHZ                        0x0C
#define DHCMCU_CLK_86MHZ                        0x10
#define DHCMCU_CLK_108MHZ                       0x14
#define DHCMCU_CLK_RESERVED                     0x18
#define DHCLIVE_CLK_NONGATE                     0x00
#define DHCLIVE_CLK_GATING                      0x40
#define DHCLIVE_CLK_NONINVERT                   0x00
#define DHCLIVE_CLK_INVERT                      0x80

//---------------------------------------------
// definition for REG_CKG_AEON1DC0  //reg[0x1e5a]
#define DC0_CLK_ENABLE                          0x00
#define DC0_CLK_DISABLE                         BIT0
#define DC0_CLK_NONINVERT                       0x00
#define DC0_CLK_INVERT                          BIT1
#define DC0_CLK_MASK                           (BIT4|BIT3|BIT2)
#define DC0_CLK_SYNCMODE                        0x00
#define DC0_CLK_FREERUN                         BIT2
#define DC0_CLK_27MHZ                           BIT3
#define DC0_CLK_54MHZ                          (BIT3|BIT2)
#define DC0_CLK_72MHZ                           BIT4
#define DC0_CLK_86_4MHZ                        (BIT4|BIT2)
#define DC0_CLK_108MHZ                         (BIT4|BIT3)
#define DC0_CLK_144MHZ                         (BIT4|BIT3|BIT2)

//----------------------------------
#define AEON1_CLK_ENABLE                        0x00
#define AEON1_CLK_DISABLE                       BIT5
#define AEON1_CLK_NONINVERT                     0x00
#define AEON1_CLK_INVERT                        BIT6

//---------------------------------------------
// definition for REG_CKG_MLOADJPD  //reg[0x1e5b]
#define JPD_CLK_ENABLE                          0x00
#define JPD_CLK_DISABLE                         BIT0
#define JPD_CLK_NONINVERT                       0x00
#define JPD_CLK_INVERT                          BIT1
#define JPD_CLK_MASK                           (BIT3|BIT2)
#define JPD_CLK_144MHZ                          0x00
#define JPD_CLK_123MHZ                          BIT2
#define JPD_CLK_CLKMIU                          BIT3
#define JPD_CLK_XTAL                           (BIT3|BIT2)

//---------------------------------------------
// definition for REG_CKG_TSOUT  //reg[0x1e5c]
#define TSOUT_CLK_ENABLE                        0x00
#define TSOUT_CLK_DISABLE                       BIT0
#define TSOUT_CLK_NONINVERT                     0x00
#define TSOUT_CLK_INVERT                        BIT1
#define TSOUT_CLK_MASK                         (BIT4|BIT3|BIT2)
#define TSOUT_CLK_CLK86DIV2                     0x00
#define TSOUT_CLK_54MHZ                         BIT2
#define TSOUT_CLK_62MHZ                         BIT3
#define TSOUT_CLK_72MHZ                        (BIT3|BIT2)
#define TSOUT_CLK_86MHZ                         BIT4
#define TSOUT_CLK_108MHZ                       (BIT4|BIT2)
#define TSOUT_CLK_XTAL                         (BIT4|BIT3)
#define TSOUT_CLK_XTAL1                        (BIT4|BIT3|BIT2)

//---------------------------------------------
// definition for REG_DOUT_SRC_SEL  //reg[0x1EA9]
#define DOUT_SRC_BT656                          0x00
#define DOUT_SRC_CCIR                           BIT5
#define DOUT_SRC_PVR                            BIT6
#define DOUT_SRC_DTV                           (BIT6|BIT5)
#define DOUT_SRC_GOP                            BIT7

//---------------------------------------------
// definition for REG_MUX_UART_INV  //reg[0x1EAB]
#define UART_SEL2_HK51_UART0                    0x00
#define UART_SEL2_HK51_UART1                    BIT0
#define UART_SEL2_VD51_UART0                    BIT1
#define UART_SEL2_AEON_                        (BIT1|BIT0)
#define UART_SEL2_TSP_                          BIT2

#define REG_DIDO_TEST_SEL                       BIT3
#define REG_DIDO_TEST_UNSEL                     0x00

#define REG_UART_UNINV                          0x00
#define REG_UART0_INV                           BIT4
#define REG_UART1_INV                           BIT5
#define REG_UART2_INV                           BIT6
#define REG_UART3_INV                           BIT7

//---------------------------------------------
// definition for REG_MUX_UART_INV  //reg[0x1EE7]
#define NFIE_CLK_ENABLE                         0x00
#define NFIE_CLK_DISABLE                        BIT0
#define NFIE_CLK_NONINVERT                      0x00
#define NFIE_CLK_INVERT                         BIT1
#define NFIE_CLK_62MHZ                          0x00
#define NFIE_CLK_54MHZ                          BIT2
#define NFIE_CLK_43MHZ                          BIT3
#define NFIE_CLK_36MHZ                         (BIT3|BIT2)
#define NFIE_CLK_27MHZ                          BIT4
#define NFIE_CLK_18MHZ                         (BIT4|BIT2)
#define NFIE_CLK_13MHZ                         (BIT4|BIT3)
#define NFIE_CLK_XTALI                         (BIT4|BIT3|BIT2)

//-----------------------------------------------------------------------------
// PM_SLEEP XIU
//-----------------------------------------------------------------------------

#define REG_PM_CLK                              0x0E00

    #define PM_CLK_MCU_EN                   BIT0
    #define PM_CLK_PRAM_EN                  BIT1
    #define PM_CLK_PRAM_XIU                 BIT2

#define REG_PM_CLK_INT_XTALI_FREQ_TUNE          0x0E01  // frequecy tune for internal clock

#define REG_PM_INT_STATUS                       0x0E04
#define REG_PM_INT_EN                           0x0E08

    // [7:0]
    #define PM_INT_MASK_IR_LEVEL            BIT0
    #define PM_INT_MASK_CEC                 BIT1
    #define PM_INT_MASK_EXT_INT_LEVEL       BIT2
    #define PM_INT_MASK_EXT_INT2_LEVEL      BIT3
    #define PM_INT_MASK_KP                  BIT4
    #define PM_INT_MASK_SYNC_DET            BIT5
    #define PM_INT_MASK_DVI_CLK             BIT6
    #define PM_INT_MASK_RTC                 BIT7

    // [15:8]
    #define PM_INT_MASK_WDT                 BIT0
    #define PM_INT_RIU_MBOX0                BIT1
    #define PM_INT_RIU_MBOX1                BIT2
    #define PM_INT_RIU_MBOX2                BIT3
    #define PM_INT_RIU_MBOX3                BIT4

// MBOX to PM_MCU (R/W)
#define REG_PM_XIU_MBOX0                        0x0E0C
#define REG_PM_XIU_MBOX1                        0x0E10
#define REG_PM_XIU_MBOX2                        0x0E14
#define REG_PM_XIU_MBOX3                        0x0E18

#define REG_PM_WDT_STATUS                       0x0E1C

// MBOX to HK_MCU (RDONLY)
#define REG_PM_RIU_MBOX0                        0x0E20
#define REG_PM_RIU_MBOX1                        0x0E24
#define REG_PM_RIU_MBOX2                        0x0E28
#define REG_PM_RIU_MBOX3                        0x0E2C

// XIU download interface
#define REG_PM_XIU2PRAM_ADDR                    0x0E30
#define REG_PM_XIU2PRAM_WD                      0x0E34
#define REG_PM_XIU2PRAM_RD                      0x0E38

#define REG_PM_CTRL                             0x0E3C

    #define PM_CTRL_XTALI_SW_RST            BIT0
    #define PM_CTRL_MCU_RST                 BIT1
    #define PM_CTRL_CLK_INT_XTALI_KREF_EN   BIT2        // calibration enable to clk_int_xtali

//-----------------------------------------------------------------------------
// PWM
//-----------------------------------------------------------------------------
#define REG_PWM_UNIT_DIV                        0x3D02

#define REG_PWM0_PERIOD                         0x3D04
#define REG_PWM0_DUTY                           0x3D06
#define REG_PWM0_DIV                            0x3D08
#define REG_PWM0_CTRL                           0x3D09

    #define PWM_CTRL_PORARITY               BIT0
    #define PWM_CTRL_VBEN                   BIT1
    #define PWM_CTRL_RESET_EN               BIT2
    #define PWM_CTRL_DBEN                   BIT3

#define REG_PWM1_PERIOD                         0x3D0A
#define REG_PWM1_DUTY                           0x3D0C
#define REG_PWM1_DIV                            0x3D0E
#define REG_PWM1_CTRL                           0x3D0F

#define REG_PWM2_PERIOD                         0x3D10
#define REG_PWM2_DUTY                           0x3D12
#define REG_PWM2_DIV                            0x3D14
#define REG_PWM2_CTRL                           0x3D15

#define REG_PWM3_PERIOD                         0x3D16
#define REG_PWM3_DUTY                           0x3D18
#define REG_PWM3_DIV                            0x3D1A
#define REG_PWM3_CTRL                           0x3D1B

#define REG_PWM4_PERIOD                         0x3D1C
#define REG_PWM4_DUTY                           0x3D1E
#define REG_PWM4_DIV                            0x3D20
#define REG_PWM4_CTRL                           0x3D21

#define REG_PWM5_PERIOD                         0x3D22
#define REG_PWM5_DUTY                           0x3D24
#define REG_PWM5_DIV                            0x3D26
#define REG_PWM5_CTRL                           0x3D27

#define REG_PWM0_HS_RST                         0x3D28
#define REG_PWM1_HS_RST                         0x3D29
#define REG_PWM2_HS_RST                         0x3D2A
#define REG_PWM3_HS_RST                         0x3D2B
#define REG_PWM4_HS_RST                         0x3D2C
#define REG_PWM5_HS_RST                         0x3D2D

    #define PWM_HS_RST_MUX                  BIT7
    #define PWM_HS_RST_CNT_MASK             BITMASK(3:0)

//-------------------------------------------
//SC2_REG_BASE  //0x3000
#define REG_SEL_CLOCK                           0x3081
#define REG_SEL_GPO_0                           0x3086
#define REG_SEL_GPO_1                           0x3087

#define REG_SEL_TTL_0                           0x3088
#define REG_SEL_TTL_1                           0x3089

#define ODD_IS_GPIO_0                           0x308C
#define ODD_IS_GPIO_1                           0x308D
#define ODD_IS_GPIO_2                           0x308E
#define ODD_IS_GPIO_3                           0x308F  //VS,HS,DE,CLK,b0~b7,g0~g7,r0~r7

#define ODD_GPO_SEL_0                           0x309A  //selec as GPO pin ;switch to outpur pad
#define ODD_GPO_SEL_1                           0x309B  //selec as GPO pin
#define ODD_GPO_SEL_2                           0x309C  //selec as GPO pin
#define ODD_GPO_SEL_3                           0x309D  //selec as GPO pin

#define ODD_GPO_DATA_0                          0x309E  // GPO data to pinout
#define ODD_GPO_DATA_1                          0x309F  // GPO data to pinout
#define ODD_GPO_DATA_2                          0x30A0  // GPO data to pinout
#define ODD_GPO_DATA_3                          0x30A1  // GPO data to pinout

#define ODD_GPO_OEZ_0                           0x30A2  // 0:output, 1:input
#define ODD_GPO_OEZ_1                           0x30A3
#define ODD_GPO_OEZ_2                           0x30A4
#define ODD_GPO_OEZ_3                           0x30A5

#define ODD_GPI_0                               0x30AA  // ODD is gPi pin
#define ODD_GPI_1                               0x30AB
#define ODD_GPI_2                               0x30AC
#define ODD_GPI_3                               0x30AD

#define MCUCLK_SEL                              0x1003

// MIIC control:
#define IIC0_CTRL                               0x3420
#define IIC0_CLK_SEL                            0x3422
#define IIC0_WDATA                              0x3424
#define IIC0_RDATA                              0x3426
#define IIC0_STATUS                             0x3428

#define ISP_WR_HEAD                             0x1060
#define ISP_WR_AH                               0x1061
#define ISP_WR_AM                               0x1062
#define ISP_WR_AL                               0x1063
#define ISP_WR_DATA                             0x1064
#define ISP_CTRL                                0x1065
#define ISP_RD_DATA                             0x1066
#define ISP_CEB                                 0x1067
#define ISP_OEB                                 0x1068
#define ISP_WEB                                 0x1069
#define ISP_CMDLEN                              0x106A
#define ISP_CTRL1                               0x106B

//------------------------------------------------------------------------------
// IR register define
//------------------------------------------------------------------------------
#define IR_CTRL                                 0x3D80
#define IR_CHECK                                0x3D81
#define IR_HDC_UPB_L                            0x3D82
#define IR_HDC_UPB_H                            0x3D83
#define IR_HDC_LOB_L                            0x3D84
#define IR_HDC_LOB_H                            0x3D85
#define IR_OFC_UPB_L                            0x3D86
#define IR_OFC_UPB_H                            0x3D87
#define IR_OFC_LOB_L                            0x3D88
#define IR_OFC_LOB_H                            0x3D89
#define IR_OFC_RP_UPB_L                         0x3D8A
#define IR_OFC_RP_UPB_H                         0x3D8B
#define IR_OFC_RP_LOB_L                         0x3D8C
#define IR_OFC_RP_LOB_H                         0x3D8D
#define IR_LG01H_UPB_L                          0x3D8E
#define IR_LG01H_UPB_H                          0x3D8F
#define IR_LG01H_LOB_L                          0x3D90
#define IR_LG01H_LOB_H                          0x3D91
#define IR_LG0_UPB_L                            0x3D92
#define IR_LG0_UPB_H                            0x3D93
#define IR_LG0_LOB_L                            0x3D94
#define IR_LG0_LOB_H                            0x3D95
#define IR_LG1_UPB_L                            0x3D96
#define IR_LG1_UPB_H                            0x3D97
#define IR_LG1_LOB_L                            0x3D98
#define IR_LG1_LOB_H                            0x3D99
#define IR_SEPR_UPB_L                           0x3D9A
#define IR_SEPR_UPB_H                           0x3D9B
#define IR_SEPR_LOB_L                           0x3D9C
#define IR_SEPR_LOB_H                           0x3D9D
#define IR_TIMEOUT_CYC_0                        0x3D9E
#define IR_TIMEOUT_CYC_1                        0x3D9F
#define IR_TIMEOUT_CYC_2                        0x3DA0
#define IR_CODEBYTE                             0x3DA1
#define IR_SEPR_BIT                             0x3DA2
#define IR_FIFO_CTRL                            0x3DA3  // different location with Saturn
#define IR_CCODE_L                              0x3DA4
#define IR_CCODE_H                              0x3DA5
#define IR_GLHRM_NUM_L                          0x3DA6
#define IR_GLHRM_NUM_H                          0x3DA7
#define IR_CKDIV_NUM_REG                        0x3DA8  // different location with Saturn
#define IR_KEY                                  0x3DA9
#define IR_SHOT_CNT_0                           0x3DAA
#define IR_SHOT_CNT_1                           0x3DAB
#define IR_SHOT_CNT_2                           0x3DAC
#define IR_RPT_FIFOEMPTY                        0x3DAD
#define IR_FIFO_READ_PULSE                      0x3DB0  // added in Saturn2

#define TAG_A0                                  0x10A8
#define TAG_A1                                  0x10A9
#define TAG_A2                                  0x10AA
#define TAG_A3                                  0x10AB
#define TAG_B0                                  0x10AC
#define TAG_B1                                  0x10AD
#define TAG_B2                                  0x10AE
#define TAG_B3                                  0x10AF

#define VDMCU_MIU_MAP_CMD                       0x10C0
#define VDMCU_SDRAM_CODE_MAP                    0x10C1
#define VDMCU_SDRAM_CODE_MAP_H                  0x10C2
#define VDMCU_CACHE_ACCESS                      0x10C6
#define VDMCU_CACHE_HIT                         0x10C7
#define VDMCU_TAG_A0                            0x10C8
#define VDMCU_TAG_A1                            0x10C9
#define VDMCU_TAG_A2                            0x10CA
#define VDMCU_TAG_A3                            0x10CB
#define VDMCU_TAG_B0                            0x10CC
#define VDMCU_TAG_B1                            0x10CD
#define VDMCU_TAG_B2                            0x10CE
#define VDMCU_TAG_B3                            0x10CF

#define PM_PD                                   0x10D0
#define PM_TH0                                  0x10D1
#define PM_TH1                                  0x10D2
#define PM_XTL_PWD0                             0x10D3
#define PM_XTL_PWD1                             0x10D4
#define PM_SB_PWD0                              0x10D5
#define PM_SB_PWD1                              0x10D6

#define PM_OFF_FLAG                             (0x3C1E)  // U02(0x3C1C)  /U030x3C1E)

#define PM_SYS_PD                               (0x01)
#define PM_GPIO_ACT_LEVEL                       (0x02)
#define PM_IR_ACT_LEVEL                         (0x04)
#define PM_IR_PD                                (0x08)
#define PM_TEST_MODE                            (0x10)
#define PM_CHK_IR_NOISE                         (0x20)

#define PM_XTL_PWD0_VAL                         (0x8E)
#define PM_XTL_PWD1_VAL                         (0x9F)
#define PM_SB_PWD0_VAL                          (0xB4)
#define PM_SB_PWD1_VAL                          (0xA5)
#define PM_TH0_VAL                              (0xBA)
#define PM_TH1_VAL                              (0x0D)

#define PM_FIRST_BOOTUP                         (0x01)
#define PM_MODE_MASK                            (0x06)

#if ( 1 )  //POWERUP_MODE == PUMODE_WORK )
    #define PM_MODE_ON                          (0x00)
    #define PM_MODE_OFF_EXEC                    (0x02)
    #define PM_MODE_OFF                         (0x04)
    #define PM_MODE_ON_EXEC                     (0x06)
#else
    #define PM_MODE_ON                          (0x02)
    #define PM_MODE_OFF_EXEC                    (0x00)
    #define PM_MODE_OFF                         (0x04)
    #define PM_MODE_ON_EXEC                     (0x06)
#endif

#define PM_P3_RESET_FLAG                        (0x08)
#define PM_PDMODE_MASK                          (0x30)
#define PM_PDMODE_S1                            (0x00)
#define PM_PDMODE_S2                            (0x10)
#define PM_PDMODE_S3                            (0x20)
#define PM_PDMODE_S4                            (0x30)

#define     VGA_POWERSAVING         (0x40)

//------------------------------------------------------------------------------
// 2. - MIU Reg
//------------------------------------------------------------------------------
#define DDR_FREQ_SET_0                          0x1220
#define DDR_FREQ_SET_1                          0x1221
#define DDR_FREQ_STEP                           0x1224
#define DDR_FREQ_DIV_1                          0x1225
#define DDR_FREQ_INPUT_DIV_2                    0x1226
#define DDR_FREQ_LOOP_DIV_2                     0x1227
#define DDR_CLK_SELECT                          0x123E
#define DDR_FREQ_STATUS                         0x123F

#define MIU_RQ0L_MASK                           (MIU_REG_BASE+0x46)//Reg[0x1246]
#define MIU_RQ0H_MASK                           (MIU_REG_BASE+0x47)
#define MIU_RQ1L_MASK                           (MIU_REG_BASE+0x66)
#define MIU_RQ1H_MASK                           (MIU_REG_BASE+0x67)
#define MIU_RQ2L_MASK                           (MIU_REG_BASE+0x86)
#define MIU_RQ2H_MASK                           (MIU_REG_BASE+0x87)

#define MIU_PROTECT_EN                          (MIU_REG_BASE+0xC0)
#define MIU_PROTECT_SDR_LIKE                    (MIU_REG_BASE+0xC1)

#define MIU_PROTECT0_ID0                        (MIU_REG_BASE+0xC2)
#define MIU_PROTECT0_ID1                        (MIU_REG_BASE+0xC3)
#define MIU_PROTECT0_ID2                        (MIU_REG_BASE+0xC4)
#define MIU_PROTECT0_ID3                        (MIU_REG_BASE+0xC5)
#define MIU_PROTECT0_START_ADDR_H               (MIU_REG_BASE+0xC6)
#define MIU_PROTECT0_START_ADDR_L               (MIU_REG_BASE+0xC7)
#define MIU_PROTECT0_END_ADDR_H                 (MIU_REG_BASE+0xC8)
#define MIU_PROTECT0_END_ADDR_L                 (MIU_REG_BASE+0xC9)

#define MIU_PROTECT1_ID0                        (MIU_REG_BASE+0xCA)
#define MIU_PROTECT1_ID1                        (MIU_REG_BASE+0xCB)
#define MIU_PROTECT1_START_ADDR_H               (MIU_REG_BASE+0xCC)
#define MIU_PROTECT1_START_ADDR_L               (MIU_REG_BASE+0xCD)
#define MIU_PROTECT1_END_ADDR_H                 (MIU_REG_BASE+0xCE)
#define MIU_PROTECT1_END_ADDR_L                 (MIU_REG_BASE+0xCF)

#define MIU_PROTECT2_ID0                        (MIU_REG_BASE+0xD0)
#define MIU_PROTECT2_ID1                        (MIU_REG_BASE+0xD1)
#define MIU_PROTECT2_START_ADDR_H               (MIU_REG_BASE+0xD2)
#define MIU_PROTECT2_START_ADDR_L               (MIU_REG_BASE+0xD3)
#define MIU_PROTECT2_END_ADDR_H                 (MIU_REG_BASE+0xD4)
#define MIU_PROTECT2_END_ADDR_L                 (MIU_REG_BASE+0xD5)

#define MIU_PROTECT3_ID0                        (MIU_REG_BASE+0xD6)
#define MIU_PROTECT3_ID1                        (MIU_REG_BASE+0xD7)
#define MIU_PROTECT3_START_ADDR_H               (MIU_REG_BASE+0xD8)
#define MIU_PROTECT3_START_ADDR_L               (MIU_REG_BASE+0xD9)
#define MIU_PROTECT3_END_ADDR_H                 (MIU_REG_BASE+0xDA)
#define MIU_PROTECT3_END_ADDR_L                 (MIU_REG_BASE+0xDB)

#define MIU_DDFSET                              0x1220
#define MIU_DDFSPAN                             0x1222
#define MIU_DDFSTEP                             0x1224
#define MIU_PLLCTRL                             0x1225

#define MIU_RQ1_CTRL0                           0x1260
#define MIU_RQ2_CTRL0                           0x1280
#define MIU_RQ2_CTRL1                           0x1281
#define MIU_RQ2_HPRIORITY                       0x1288
#define MIU_GE_FLOW_CTRL_TIME                   0x1294

// MIU0 self test (BIST)
#define REG_MIU_TEST_MODE                       0x12E0
#define REG_MIU_TEST_BASE                       0x12E2
#define REG_MIU_TEST_LENGTH                     0x12E4
#define REG_MIU_TEST_DATA                       0x12E8

// MIU1 self test (BIST)
#define REG_MIU1_TEST_MODE                      0x06E0
#define REG_MIU1_TEST_BASE                      0x06E2
#define REG_MIU1_TEST_LENGTH                    0x06E4
#define REG_MIU1_TEST_DATA                      0x06E8


// MIU selection registers
#define REG_MIU_SEL0                            0x12F0
#define REG_MIU_SEL1                            0x12F1
#define REG_MIU_SEL2                            0x12F2
#define REG_MIU_SEL3                            0x12F3
#define REG_MIU_SEL4                            0x12F4
#define REG_MIU_SEL5                            0x12F5
#define REG_MIU_SEL6                            0x12F6
#define REG_MIU_SEL7                            0x12F7

//----------------------------------------------------------------------------
// 5. DEMUX Reg
//----------------------------------------------------------------------------
#define NIM_HWCONFIG0                           0x1558
#define NIM_HWCONFIG1                           0x155A
#define NIM_HWCONFIG2                           0x1584
#define NIM_HWCONFIG3                           0x1586
#define NIM_HWINPORT                            0x155E

#define PCR_L0                                  0x1560
#define PCR_L1                                  0x1561
#define PCR_L2                                  0x1562
#define PCR_L3                                  0x1563
#define PCR_H                                   0x1564

#define SECISR_INTCODE0                         0x1570
#define SECISR_INTCODE1                         0x1571
#define SECISR_INTCODE2                         0x1572
#define SECISR_INTCODE3                         0x1573
#define SECISR_ACKCODE0                         0x1580
#define SECISR_ACKCODE1                         0x1581
#define SECISR_ACKCODE2                         0x1582
#define SECISR_ACKCODE3                         0x1583

#define TSP_DMA_RADDR0                          0x15F0
#define TSP_DMA_RADDR1                          0x15F1
#define TSP_DMA_RNUM0                           0x15F2
#define TSP_DMA_RNUM1                           0x15F3
#define TSP_CTRL                                0x15F4


//----------------------------------------------------------------------------
// 11. GOP Reg
//----------------------------------------------------------------------------
#define GOP_REG_VAL(x)                          (1<<x)

#define __ERIS_GOP_REG(bk, reg)                                   (((U16)(bk)<<8) + (reg) * 2)
#define __GOP_REG(reg)                                   ((reg) * 2)


#define GOP_4G_CTRL0                        __ERIS_GOP_REG(0, 0x00)
#define GOP_4G_CTRL1                        __ERIS_GOP_REG(0, 0x01)
#define GOP_4G_RATE                         __ERIS_GOP_REG(0, 0x02)
#define GOP_4G_PALDATA_L                    __ERIS_GOP_REG(0, 0x03)
#define GOP_4G_PALDATA_H                    __ERIS_GOP_REG(0, 0x04)
#define GOP_4G_PALCTRL                      __ERIS_GOP_REG(0, 0x05)
#define GOP_4G_REGDMA_END                   __ERIS_GOP_REG(0, 0x06)
#define GOP_4G_REGDMA_STR                   __ERIS_GOP_REG(0, 0x07)
#define GOP_4G_INT                          __ERIS_GOP_REG(0, 0x08)
#define GOP_4G_HWSTATE                      __ERIS_GOP_REG(0, 0x09)
#define GOP_4G_SVM_HSTR                     __ERIS_GOP_REG(0, 0x0a)
#define GOP_4G_SVM_HEND                     __ERIS_GOP_REG(0, 0x0b)
#define GOP_4G_SVM_VSTR                     __ERIS_GOP_REG(0, 0x0c)
#define GOP_4G_SVM_VEND                     __ERIS_GOP_REG(0, 0x0d)
#define GOP_4G_RDMA_HT                      __ERIS_GOP_REG(0, 0x0e)
#define GOP_4G_HS_PIPE                      __ERIS_GOP_REG(0, 0x0f) //new
#define GOP_4G_SLOW                         __ERIS_GOP_REG(0, 0x10) //new
#define GOP_4G_BRI                          __ERIS_GOP_REG(0, 0x11) //new
#define GOP_4G_CON                          __ERIS_GOP_REG(0, 0x12) //new
#define GOP_4G_PRI0                         __ERIS_GOP_REG(0, 0x20)
#define GOP_4G_TRSCLR_L                     __ERIS_GOP_REG(0, 0x24)
#define GOP_4G_TRSCLR_H                     __ERIS_GOP_REG(0, 0x25)
#define GOP_4G_STRCH_HSZ                    __ERIS_GOP_REG(0, 0x30)
#define GOP_4G_STRCH_VSZ                    __ERIS_GOP_REG(0, 0x31)
#define GOP_4G_STRCH_HSTR                   __ERIS_GOP_REG(0, 0x32)
#define GOP_4G_STRCH_VSTR                   __ERIS_GOP_REG(0, 0x34)
#define GOP_4G_HSTRCH                       __ERIS_GOP_REG(0, 0x35)
#define GOP_4G_VSTRCH                       __ERIS_GOP_REG(0, 0x36)
#define GOP_4G_HSTRCH_INI                   __ERIS_GOP_REG(0, 0x38)
#define GOP_4G_VSTRCH_INI                   __ERIS_GOP_REG(0, 0x39)
#define GOP_4G_RBLK0_VOFFL                  __ERIS_GOP_REG(0, 0x60)
#define GOP_4G_RBLK0_VOFFH                  __ERIS_GOP_REG(0, 0x61)
#define GOP_4G_RBLK1_VOFFL                  __ERIS_GOP_REG(0, 0x62)
#define GOP_4G_RBLK1_VOFFH                  __ERIS_GOP_REG(0, 0x63)
#define GOP_4G_RBLK2_VOFFL                  __ERIS_GOP_REG(0, 0x64)
#define GOP_4G_RBLK2_VOFFH                  __ERIS_GOP_REG(0, 0x65)
#define GOP_4G_RBLK3_VOFFL                  __ERIS_GOP_REG(0, 0x66)
#define GOP_4G_RBLK3_VOFFH                  __ERIS_GOP_REG(0, 0x67)
#define GOP_4G_RBLK0_HOFF                   __ERIS_GOP_REG(0, 0x70)
#define GOP_4G_RBLK1_HOFF                   __ERIS_GOP_REG(0, 0x71)
#define GOP_4G_RBLK2_HOFF                   __ERIS_GOP_REG(0, 0x72)
#define GOP_4G_RBLK3_HOFF                   __ERIS_GOP_REG(0, 0x73)
#define GOP_4G_REGDMA_EN                    __ERIS_GOP_REG(0, 0x78)
#define GOP_MUX                             __GOP_REG(0x7e)
#define GOP_BAK_SEL                         __GOP_REG(0x7f)
#define GOP_OFFSET_WR                       8
#define GOP_VAL_WR                          GOP_REG_VAL(GOP_OFFSET_WR)
#define GOP_OFFSET_FWR                      9
#define GOP_VAL_FWR                         GOP_REG_VAL(GOP_OFFSET_FWR)
#define GOP_OFFSET_FCLR                     11
#define GOP_VAL_FCL                         GOP_REG_VAL(GOP_OFFSET_FCLR)
#define GOP4G_OFFSET_WR_ACK                 12
#define GOP4G_VAL_WR_ACK                    GOP_REG_VAL(GOP4G_OFFSET_WR_ACK)
#define GOP2G_OFFSET_WR_ACK                 13
#define GOP2G_VAL_WR_ACK                    GOP_REG_VAL(GOP2G_OFFSET_WR_ACK)
#define GOPD_OFFSET_WR_ACK                  14
#define GOPD_VAL_WR_ACK                     GOP_REG_VAL(GOPD_OFFSET_WR_ACK)
#define GOP1G_OFFSET_WR_ACK                 15
#define GOP1G_VAL_WR_ACK                    GOP_REG_VAL(GOPD_OFFSET_WR_ACK)
#define GOP_VAL_ACK(x)                      GOP_REG_VAL(GOP4G_OFFSET_WR_ACK+x)




#define GOP_4G_GWIN0_CTRL(id)               __ERIS_GOP_REG(1, 0x00+(0x20*((id)%4)))
#define GOP_4G_DRAM_RBLK_L(id)              __ERIS_GOP_REG(1, 0x01+(0x20*((id)%4)))
#define GOP_4G_DRAM_RBLK_H(id)              __ERIS_GOP_REG(1, 0x02+(0x20*((id)%4)))
#define GOP_4G_HSTR(id)                     __ERIS_GOP_REG(1, 0x04+(0x20*((id)%4)))
#define GOP_4G_HEND(id)                     __ERIS_GOP_REG(1, 0x05+(0x20*((id)%4)))
#define GOP_4G_VSTR(id)                     __ERIS_GOP_REG(1, 0x06+(0x20*((id)%4)))
#define GOP_4G_VEND(id)                     __ERIS_GOP_REG(1, 0x08+(0x20*((id)%4)))
#define GOP_4G_DRAM_RBLK_HSIZE(id)          __ERIS_GOP_REG(1, 0x09+(0x20*((id)%4)))
#define GOP_4G_DRAM_VSTR_L(id)              __ERIS_GOP_REG(1, 0x0c+(0x20*((id)%4)))
#define GOP_4G_DRAM_VSTR_H(id)              __ERIS_GOP_REG(1, 0x0d+(0x20*((id)%4)))
#define GOP_4G_DRAM_HSTR(id)                __ERIS_GOP_REG(1, 0x0e+(0x20*((id)%4)))
#define GOP_4G_DRAM_RBLK_SIZE_L(id)         __ERIS_GOP_REG(1, 0x10+(0x20*((id)%4)))
#define GOP_4G_DRAM_RBLK_SIZE_H(id)         __ERIS_GOP_REG(1, 0x11+(0x20*((id)%4)))
#define GOP_4G_DRAM_RLEN(id)                __ERIS_GOP_REG(1, 0x12+(0x20*((id)%4)))
#define GOP_4G_DRAM_HVSTOP_L(id)            __ERIS_GOP_REG(1, 0x14+(0x20*((id)%4)))
#define GOP_4G_DRAM_HVSTOP_H(id)            __ERIS_GOP_REG(1, 0x15+(0x20*((id)%4)))
#define GOP_4G_DRAM_FADE(id)                __ERIS_GOP_REG(1, 0x16+(0x20*((id)%4)))
#define GOP_4G_BG_CLR(id)                   __ERIS_GOP_REG(1, 0x18+(0x20*((id)%4)))
#define GOP_4G_BG_HSTR(id)                  __ERIS_GOP_REG(1, 0x19+(0x20*((id)%4)))
#define GOP_4G_BG_HEND(id)                  __ERIS_GOP_REG(1, 0x1a+(0x20*((id)%4)))
#define GOP_4G_BG_VSTR(id)                  __ERIS_GOP_REG(1, 0x1c+(0x20*((id)%4)))
#define GOP_4G_BG_VEND(id)                  __ERIS_GOP_REG(1, 0x1d+(0x20*((id)%4)))


#define GOP_2G_CTRL0                        __ERIS_GOP_REG(3, 0x00)
#define GOP_2G_CTRL1                        __ERIS_GOP_REG(3, 0x01)
#define GOP_2G_RATE                         __ERIS_GOP_REG(3, 0x02)
//#define GOP_2G_PALDATA_L                    __ERIS_GOP_REG(3, 0x03)
//#define GOP_2G_PALDATA_H                    __ERIS_GOP_REG(3, 0x04)
//#define GOP_2G_PALCTRL                      __ERIS_GOP_REG(3, 0x05)
//#define GOP_2G_REGDMA_END                   __ERIS_GOP_REG(3, 0x06)
//#define GOP_2G_REGDMA_STR                   __ERIS_GOP_REG(3, 0x07)
#define GOP_2G_INT                          __ERIS_GOP_REG(3, 0x08)
#define GOP_2G_HWSTATE                      __ERIS_GOP_REG(3, 0x09)
//#define GOP_2G_SVM_HSTR                     __ERIS_GOP_REG(3, 0x0a)
//#define GOP_2G_SVM_HEND                     __ERIS_GOP_REG(3, 0x0b)
//#define GOP_2G_SVM_VSTR                     __ERIS_GOP_REG(3, 0x0c)
//#define GOP_2G_SVM_VEND                     __ERIS_GOP_REG(3, 0x0d)
#define GOP_2G_RDMA_HT                      __ERIS_GOP_REG(3, 0x0e)
#define GOP_2G_HS_PIPE                      __ERIS_GOP_REG(3, 0x0f) //new
#define GOP_2G_SLOW                         __ERIS_GOP_REG(3, 0x10) //new
#define GOP_2G_BRI                          __ERIS_GOP_REG(3, 0x11) //new
#define GOP_2G_CON                          __ERIS_GOP_REG(3, 0x12) //new
#define GOP_2G_PRI0                         __ERIS_GOP_REG(3, 0x20) //new
#define GOP_2G_TRSCLR_L                     __ERIS_GOP_REG(3, 0x24) //new
#define GOP_2G_TRSCLR_H                     __ERIS_GOP_REG(3, 0x25) //new
#define GOP_2G_STRCH_HSZ                    __ERIS_GOP_REG(3, 0x30)
#define GOP_2G_STRCH_VSZ                    __ERIS_GOP_REG(3, 0x31)
#define GOP_2G_STRCH_HSTR                   __ERIS_GOP_REG(3, 0x32)
#define GOP_2G_STRCH_VSTR                   __ERIS_GOP_REG(3, 0x34)
#define GOP_2G_HSTRCH                       __ERIS_GOP_REG(3, 0x35)
#define GOP_2G_VSTRCH                       __ERIS_GOP_REG(3, 0x36)
#define GOP_2G_HSTRCH_INI                   __ERIS_GOP_REG(3, 0x38)
#define GOP_2G_VSTRCH_INI                   __ERIS_GOP_REG(3, 0x39)
//#define GOP_2G_RBLK0_VOFFL                  __ERIS_GOP_REG(3, 0x60)
//#define GOP_2G_RBLK0_VOFFH                  __ERIS_GOP_REG(3, 0x61)
//#define GOP_2G_RBLK1_VOFFL                  __ERIS_GOP_REG(3, 0x62)
//#define GOP_2G_RBLK1_VOFFH                  __ERIS_GOP_REG(3, 0x63)
//#define GOP_2G_RBLK2_VOFFL                  __ERIS_GOP_REG(3, 0x64)
//#define GOP_2G_RBLK2_VOFFH                  __ERIS_GOP_REG(3, 0x65)
//#define GOP_2G_RBLK3_VOFFL                  __ERIS_GOP_REG(3, 0x66)
//#define GOP_2G_RBLK3_VOFFH                  __ERIS_GOP_REG(3, 0x67)
//#define GOP_2G_RBLK0_HOFF                   __ERIS_GOP_REG(3, 0x70)
//#define GOP_2G_RBLK1_HOFF                   __ERIS_GOP_REG(3, 0x71)
//#define GOP_2G_RBLK2_HOFF                   __ERIS_GOP_REG(3, 0x72)
//#define GOP_2G_RBLK3_HOFF                   __ERIS_GOP_REG(3, 0x73)
//#define GOP_2G_REGDMA_EN                    __ERIS_GOP_REG(3, 0x78)



#define GOP_2G_GWIN0_CTRL(id)               __ERIS_GOP_REG(4, 0x00+(0x20*((id)%2)))
#define GOP_2G_DRAM_RBLK_L(id)              __ERIS_GOP_REG(4, 0x01+(0x20*((id)%2)))
#define GOP_2G_DRAM_RBLK_H(id)              __ERIS_GOP_REG(4, 0x02+(0x20*((id)%2)))
#define GOP_2G_HSTR(id)                     __ERIS_GOP_REG(4, 0x04+(0x20*((id)%2)))
#define GOP_2G_HEND(id)                     __ERIS_GOP_REG(4, 0x05+(0x20*((id)%2)))
#define GOP_2G_VSTR(id)                     __ERIS_GOP_REG(4, 0x06+(0x20*((id)%2)))
#define GOP_2G_VEND(id)                     __ERIS_GOP_REG(4, 0x08+(0x20*((id)%2)))
#define GOP_2G_DRAM_RBLK_HSIZE(id)          __ERIS_GOP_REG(4, 0x09+(0x20*((id)%2)))
//#define GOP_2G_DRAM_VSTR_L(id)              __ERIS_GOP_REG(4, 0x0c+(0x20*((id)%2)))
//#define GOP_2G_DRAM_VSTR_H(id)              __ERIS_GOP_REG(4, 0x0d+(0x20*((id)%2)))
//#define GOP_2G_DRAM_HSTR(id)                __ERIS_GOP_REG(4, 0x0e+(0x20*((id)%2)))
//#define GOP_2G_DRAM_RBLK_SIZE_L(id)         __ERIS_GOP_REG(4, 0x10+(0x20*((id)%2)))
//#define GOP_2G_DRAM_RBLK_SIZE_H(id)         __ERIS_GOP_REG(4, 0x11+(0x20*((id)%2)))
//#define GOP_2G_DRAM_RLEN(id)                __ERIS_GOP_REG(4, 0x12+(0x20*((id)%2)))
//#define GOP_2G_DRAM_HVSTOP_L(id)            __ERIS_GOP_REG(4, 0x14+(0x20*((id)%2)))
//#define GOP_2G_DRAM_HVSTOP_H(id)            __ERIS_GOP_REG(4, 0x15+(0x20*((id)%2)))
#define GOP_2G_DRAM_FADE(id)                __ERIS_GOP_REG(4, 0x16+(0x20*((id)%2)))
//#define GOP_2G_BG_CLR(id)                   __ERIS_GOP_REG(4, 0x18+(0x20*((id)%2)))
//#define GOP_2G_BG_HSTR(id)                  __ERIS_GOP_REG(4, 0x19+(0x20*((id)%2)))
//#define GOP_2G_BG_HEND(id)                  __ERIS_GOP_REG(4, 0x1a+(0x20*((id)%2)))
//#define GOP_2G_BG_VSTR(id)                  __ERIS_GOP_REG(4, 0x1c+(0x20*((id)%2)))
//#define GOP_2G_BG_VEND(id)                  __ERIS_GOP_REG(4, 0x1d+(0x20*((id)%2)))





// DWIN reg
#define GOP_DW_CTL0_EN                          __ERIS_GOP_REG(5, 0x00)
#define GOP_DWIN_EN                             (0x00)
#define GOP_DWIN_EN_VAL                         GOP_REG_VAL(GOP_DWIN_EN)
#define GOP_DWIN_SHOT                           (0x07)
#define GOP_DWIN_SHOT_VAL                       GOP_REG_VAL(GOP_DWIN_SHOT)

#define GOP_DW_LSTR_WBE                         __ERIS_GOP_REG(5, 0x01)
#define GOP_DW_INT_MASK                         __ERIS_GOP_REG(5, 0x02)
#define GOP_DW_DEBUG                            __ERIS_GOP_REG(5, 0x03)
#define GOP_DW_ALPHA                            __ERIS_GOP_REG(5, 0x04)

#define GOP_DW_VSTR                             __ERIS_GOP_REG(5, 0x10)
#define GOP_DW_HSTR                             __ERIS_GOP_REG(5, 0x11)
#define GOP_DW_VEND                             __ERIS_GOP_REG(5, 0x12)
#define GOP_DW_HEND                             __ERIS_GOP_REG(5, 0x13)
#define GOP_DW_HSIZE                            __ERIS_GOP_REG(5, 0x14)
#define GOP_DW_JMPLEN                           __ERIS_GOP_REG(5, 0x15)
#define GOP_DW_DSTR_L                           __ERIS_GOP_REG(5, 0x16)
#define GOP_DW_DSTR_H                           __ERIS_GOP_REG(5, 0x17)
#define GOP_DW_UB_L                             __ERIS_GOP_REG(5, 0x18)
#define GOP_DW_UB_H                             __ERIS_GOP_REG(5, 0x19)

#define GOP_DW_PON_DSTR_L                       __ERIS_GOP_REG(5, 0x1a)
#define GOP_DW_PON_DSTR_H                       __ERIS_GOP_REG(5, 0x1b)
#define GOP_DW_PON_UB_L                         __ERIS_GOP_REG(5, 0x1c)
#define GOP_DW_PON_UB_H                         __ERIS_GOP_REG(5, 0x1d)



#define GOP_1G_CTRL0                        __ERIS_GOP_REG(6, 0x00)
#define GOP_1G_CTRL1                        __ERIS_GOP_REG(6, 0x01)
//#define GOP_1G_RATE                         __ERIS_GOP_REG(6, 0x02)
//#define GOP_1G_PALDATA_L                    __ERIS_GOP_REG(6, 0x03)
//#define GOP_1G_PALDATA_H                    __ERIS_GOP_REG(6, 0x04)
//#define GOP_1G_PALCTRL                      __ERIS_GOP_REG(6, 0x05)
//#define GOP_1G_REGDMA_END                   __ERIS_GOP_REG(6, 0x06)
//#define GOP_1G_REGDMA_STR                   __ERIS_GOP_REG(6, 0x07)
#define GOP_1G_INT                          __ERIS_GOP_REG(6, 0x08)
#define GOP_1G_HWSTATE                      __ERIS_GOP_REG(6, 0x09)
//#define GOP_1G_SVM_HSTR                     __ERIS_GOP_REG(6, 0x0a)
//#define GOP_1G_SVM_HEND                     __ERIS_GOP_REG(6, 0x0b)
//#define GOP_1G_SVM_VSTR                     __ERIS_GOP_REG(6, 0x0c)
//#define GOP_1G_SVM_VEND                     __ERIS_GOP_REG(6, 0x0d)
#define GOP_1G_RDMA_HT                      __ERIS_GOP_REG(6, 0x0e)
#define GOP_1G_HS_PIPE                      __ERIS_GOP_REG(6, 0x0f) //new
//#define GOP_1G_SLOW                         __ERIS_GOP_REG(6, 0x10) //new
#define GOP_1G_BRI                          __ERIS_GOP_REG(6, 0x11) //new
#define GOP_1G_CON                          __ERIS_GOP_REG(6, 0x12) //new
//#define GOP_1G_PRI0                         __ERIS_GOP_REG(6, 0x20)
#define GOP_1G_TRSCLR_L                     __ERIS_GOP_REG(6, 0x24)
#define GOP_1G_TRSCLR_H                     __ERIS_GOP_REG(6, 0x25)
#define GOP_1G_STRCH_HSZ                    __ERIS_GOP_REG(6, 0x30)
#define GOP_1G_STRCH_VSZ                    __ERIS_GOP_REG(6, 0x31)
#define GOP_1G_STRCH_HSTR                   __ERIS_GOP_REG(6, 0x32)
#define GOP_1G_STRCH_VSTR                   __ERIS_GOP_REG(6, 0x34)
#define GOP_1G_HSTRCH                       __ERIS_GOP_REG(6, 0x35)
//#define GOP_1G_VSTRCH                       __ERIS_GOP_REG(6, 0x36)
#define GOP_1G_HSTRCH_INI                   __ERIS_GOP_REG(6, 0x38)
//#define GOP_1G_VSTRCH_INI                   __ERIS_GOP_REG(6, 0x39)
//#define GOP_1G_RBLK0_VOFFL                  __ERIS_GOP_REG(6, 0x60)
//#define GOP_1G_RBLK0_VOFFH                  __ERIS_GOP_REG(6, 0x61)
//#define GOP_1G_RBLK1_VOFFL                  __ERIS_GOP_REG(6, 0x62)
//#define GOP_1G_RBLK1_VOFFH                  __ERIS_GOP_REG(6, 0x63)
//#define GOP_1G_RBLK2_VOFFL                  __ERIS_GOP_REG(6, 0x64)
//#define GOP_1G_RBLK2_VOFFH                  __ERIS_GOP_REG(6, 0x65)
//#define GOP_1G_RBLK3_VOFFL                  __ERIS_GOP_REG(6, 0x66)
//#define GOP_1G_RBLK3_VOFFH                  __ERIS_GOP_REG(6, 0x67)
//#define GOP_1G_RBLK0_HOFF                   __ERIS_GOP_REG(6, 0x70)
//#define GOP_1G_RBLK1_HOFF                   __ERIS_GOP_REG(6, 0x71)
//#define GOP_1G_RBLK2_HOFF                   __ERIS_GOP_REG(6, 0x72)
//#define GOP_1G_RBLK3_HOFF                   __ERIS_GOP_REG(6, 0x73)
//#define GOP_1G_REGDMA_EN                    __ERIS_GOP_REG(6, 0x78)


#define GOP_1G_GWIN0_CTRL(id)               __ERIS_GOP_REG(6, 0x40)// 1+((id)/4), 0x00+(0x20*((id)%4)))
#define GOP_1G_DRAM_RBLK_L(id)              __ERIS_GOP_REG(6, 0x41)// 1+((id)/4), 0x01+(0x20*((id)%4)))
#define GOP_1G_DRAM_RBLK_H(id)              __ERIS_GOP_REG(6, 0x42)// 1+((id)/4), 0x02+(0x20*((id)%4)))
#define GOP_1G_HSTR(id)                     __ERIS_GOP_REG(6, 0x44)// 1+((id)/4), 0x04+(0x20*((id)%4)))
#define GOP_1G_HEND(id)                     __ERIS_GOP_REG(6, 0x45)// 1+((id)/4), 0x05+(0x20*((id)%4)))
#define GOP_1G_VSTR(id)                     __ERIS_GOP_REG(6, 0x46)// 1+((id)/4), 0x06+(0x20*((id)%4)))
#define GOP_1G_VEND(id)                     __ERIS_GOP_REG(6, 0x48// 1+((id)/4), 0x08+(0x20*((id)%4)))
#define GOP_1G_DRAM_RBLK_HSIZE(id)          __ERIS_GOP_REG(6, 0x49)// 1+((id)/4), 0x09+(0x20*((id)%4)))
//#define GOP_1G_DRAM_VSTR_L(id)              __ERIS_GOP_REG(6, 0x4c)// 1+((id)/4), 0x0c+(0x20*((id)%4)))
//#define GOP_1G_DRAM_VSTR_H(id)              __ERIS_GOP_REG(6, 0x4d)// 1+((id)/4), 0x0d+(0x20*((id)%4)))
//#define GOP_1G_DRAM_HSTR(id)                __ERIS_GOP_REG(6, 0x4e)// 1+((id)/4), 0x0e+(0x20*((id)%4)))
//#define GOP_1G_DRAM_RBLK_SIZE_L(id)         __ERIS_GOP_REG(6, 0x50)// 1+((id)/4), 0x10+(0x20*((id)%4)))
//#define GOP_1G_DRAM_RBLK_SIZE_H(id)         __ERIS_GOP_REG(6, 0x51)// 1+((id)/4), 0x11+(0x20*((id)%4)))
//#define GOP_1G_DRAM_RLEN(id)                __ERIS_GOP_REG(6, 0x52)// 1+((id)/4), 0x12+(0x20*((id)%4)))
//#define GOP_1G_DRAM_HVSTOP_L(id)            __ERIS_GOP_REG(6, 0x54)// 1+((id)/4), 0x14+(0x20*((id)%4)))
//#define GOP_1G_DRAM_HVSTOP_H(id)            __ERIS_GOP_REG(6, 0x55)// 1+((id)/4), 0x15+(0x20*((id)%4)))
#define GOP_1G_DRAM_FADE(id)                __ERIS_GOP_REG(6, 0x56// 1+((id)/4), 0x16+(0x20*((id)%4)))
//#define GOP_1G_BG_CLR(id)                   __ERIS_GOP_REG(6, 0x58)// 1+((id)/4), 0x18+(0x20*((id)%4)))
//#define GOP_1G_BG_HSTR(id)                  __ERIS_GOP_REG(6, 0x59)// 1+((id)/4), 0x19+(0x20*((id)%4)))
//#define GOP_1G_BG_HEND(id)                  __ERIS_GOP_REG(6, 0x5a)// 1+((id)/4), 0x1a+(0x20*((id)%4)))
//#define GOP_1G_BG_VSTR(id)                  __ERIS_GOP_REG(6, 0x5c)// 1+((id)/4), 0x1c+(0x20*((id)%4)))
//#define GOP_1G_BG_VEND(id)                  __ERIS_GOP_REG(6, 0x5d)// 1+((id)/4), 0x1d+(0x20*((id)%4)))


//----------------------------------------------------------------------------
// 13. GE Reg (GE core use 16-bit registers)
//----------------------------------------------------------------------------
#define __GE_REG(reg)                           ((reg) * 2)
#define PE_REG_EN                               __GE_REG(0x00)  //==0x0000
#define PE_REG_EN1                              __GE_REG(0x01)
#define PE_REG_STATUS                           __GE_REG(0x07)
#define PE_REG_ABL_COEF                         __GE_REG(0x11)
#define PE_REG_DB_ABL                           __GE_REG(0x12)
#define PE_REG_ABL_CONST                        __GE_REG(0x13)
#define PE_REG_SCK_HTH0                         __GE_REG(0x14)
#define PE_REG_SCK_HTH1                         __GE_REG(0x15)
#define PE_REG_SCK_LTH0                         __GE_REG(0x16)
#define PE_REG_SCK_LTH1                         __GE_REG(0x17)
#define PE_REG_DCK_HTH0                         __GE_REG(0x18)
#define PE_REG_DCK_HTH1                         __GE_REG(0x19)
#define PE_REG_DCK_LTH0                         __GE_REG(0x1A)
#define PE_REG_DCK_LTH1                         __GE_REG(0x1B)
#define PE_REG_KEY_OP                           __GE_REG(0x1C)
#define PE_REG_SB_BASE0                         __GE_REG(0x20)
#define PE_REG_SB_BASE1                         __GE_REG(0x21)
#define PE_REG_DB_BASE0                         __GE_REG(0x26)
#define PE_REG_DB_BASE1                         __GE_REG(0x27)
#define PE_REG_DB_PIT                           __GE_REG(0x33)
#define PE_REG_FM                               __GE_REG(0x34)
#define PE_REG_CMD                              __GE_REG(0x60)

// GE_REG_EN
#define REG_EN_PE                               0x0001
#define REG_EN_PE_DITHER                        0x0002
#define REG_EN_PE_ABL                           0x0004
#define REG_EN_PE_ROP                           0x0020
#define REG_EN_PE_SCK                           0x0040
#define REG_EN_PE_DCK                           0x0080

// GE_REG_EN1
#define REG_EN_PE_CMQ                           0x0001
#define REG_EN_PE_RPRIORITY                     0x0002
#define REG_EN_PE_WPRIORITY                     0x0004
#define REG_EN_PE_STBB                          0x0010
#define REG_EN_PE_ITC                           0x0040

// GE_REG_STATUS
#define REG_CMD_FIFO_STATUS                     0x0080
#define REG_PE_STATUS                           0x0001
#define REG_BLT_STATUS                          0x0002
#define REG_PE_CMD_RDY_MASK                     (REG_CMD_FIFO_STATUS + REG_PE_STATUS)

// PE_REG_DB_ABL
#define REG_PE_MSK_ABL_COEF                     0x0007

// PE_REG_DB_ABL
#define REG_PE_MSK_DB_ABL_H                     0x03
#define REG_PE_MSK_DB_ABL                       0x0300

// GE_REG_FM
#define REG_PE_SB_FM_I1                         0x0000  // 1 bit Intesity 1
#define REG_PE_SB_FM_I2                         0x0001  // 2 bits, Intesity 2
#define REG_PE_SB_FM_I4                         0x0002  // 4 bits, Intesity 4
#define REG_PE_SB_FM_I8                         0x0004  // 8 bits, Pallette 8
#define REG_PE_SB_FM_RESERVED                   0x0007  // RESERVED
#define REG_PE_SB_FM_RGB565                     0x0008  // 16 bits, RGB565[15:0]
#define REG_PE_SB_FM_ARGB1555                   0x0009  // 16 bits, ARGB1555[15:0]
#define REG_PE_SB_FM_ARGB4444                   0x000A  // 16 bits, ARGB4444[15:0]
#define REG_PE_SB_FM_1BAAFgBg123433             0x000B  // 16 bits, 1BAAFgBg123433[15:0]
#define REG_PE_SB_FM_RGB888                     0x000C  // 24 bits, RGB888[23:0]
#define REG_PE_SB_FM_ARGB6666                   0x000D  // 24 bits, ARGB6666[23:0]
#define REG_PE_SB_FM_Y1V1Y0U0_422               0x000E  // 32 bits, Y1V1Y0U0 Packer 422[31:0]
#define REG_PE_DB_FM_I8                         0x0400  // 8 bits Pallette 8
#define REG_PE_DB_FM_RGB565                     0x0800  // 16 bits, RGB565[15:0]
#define REG_PE_DB_FM_0RGB1555                   0x0900  // 16 bits, 0RGB1555[15:0]
#define REG_PE_DB_FM_ARGB4444                   0x0A00  // 16 bits, ARGB4444[15:0]
#define REG_PE_DB_FM_1BA1A2FgBg123433           0x0B00  // 16 bits, 1BA1A2FgBg123433[15:0
#define REG_PE_DB_FM_ARGB8888                   0x0F00  // 32 bits, ARGB8888[31:0]
#define REG_PE_SB_FM_ARGB8888                   0x0F00  // 32 bits, ARGB8888[31:0]

// GE_REG_KEY_OP
#define REG_BLT_SCK_OP_MASK                     0x0001
#define REG_BLT_DCK_OP_MASK                     0x0002

#define GE_REG_EN0                              __GE_REG(0x00)
#define GE_REG_EN1                              __GE_REG(0x01)
#define GE_REG_DBG                              __GE_REG(0x02)
#define GE_REG_STBB                             __GE_REG(0x03)

#define GE_REG_STATUS                           __GE_REG(0x07)
#define GE_REG_ROP2                             __GE_REG(0x10)

#define GE_REG_ABL_COEF                         __GE_REG(0x11)
#define GE_REG_DB_ABL                           __GE_REG(0x12)
#define GE_REG_ABL_CONST                        __GE_REG(0x13)
#define GE_REG_SCK_HTH0                         __GE_REG(0x14)
#define GE_REG_SCK_HTH1                         __GE_REG(0x15)
#define GE_REG_SCK_LTH0                         __GE_REG(0x16)
#define GE_REG_SCK_LTH1                         __GE_REG(0x17)
#define GE_REG_DCK_HTH0                         __GE_REG(0x18)
#define GE_REG_DCK_HTH1                         __GE_REG(0x19)
#define GE_REG_DCK_LTH0                         __GE_REG(0x1a)
#define GE_REG_DCK_LTH1                         __GE_REG(0x1b)

#define GE_REG_BLT_CK_OP_MODE                   __GE_REG(0x1c)
#define GE_REG_DC_CSC_FM                        __GE_REG(0x1f)
#define GE_REG_SB_BASE0                         __GE_REG(0x20)
#define GE_REG_SB_BASE1                         __GE_REG(0x21)
#define GE_REG_DB_BASE0                         __GE_REG(0x26)
#define GE_REG_DB_BASE1                         __GE_REG(0x27)

#define GE_REG_VCMQ_BASE0                       __GE_REG(0x28)
#define GE_REG_VCMQ_BASE1                       __GE_REG(0x29)
#define GE_REG_VCMQ_SZ                          __GE_REG(0x2a)
#define GE_REG_P256_0                           __GE_REG(0x2d)
#define GE_REG_P256_1                           __GE_REG(0x2e)
#define GE_REG_P256_IDX                         __GE_REG(0x2f)

#define GE_REG_SB_PIT                           __GE_REG(0x30)
#define GE_REG_DB_PIT                           __GE_REG(0x33)
#define GE_REG_B_FM                             __GE_REG(0x34)

#define GE_REG_I0_C0                            __GE_REG(0x35)
#define GE_REG_I0_C1                            __GE_REG(0x36)
#define GE_REG_I1_C0                            __GE_REG(0x37)
#define GE_REG_I1_C1                            __GE_REG(0x38)
#define GE_REG_I2_C0                            __GE_REG(0x39)
#define GE_REG_I2_C1                            __GE_REG(0x3a)
#define GE_REG_I3_C0                            __GE_REG(0x3b)
#define GE_REG_I3_C1                            __GE_REG(0x3c)
#define GE_REG_I4_C0                            __GE_REG(0x3d)
#define GE_REG_I4_C1                            __GE_REG(0x3e)
#define GE_REG_I5_C0                            __GE_REG(0x3f)
#define GE_REG_I5_C1                            __GE_REG(0x40)
#define GE_REG_I6_C0                            __GE_REG(0x41)
#define GE_REG_I6_C1                            __GE_REG(0x42)
#define GE_REG_I7_C0                            __GE_REG(0x43)
#define GE_REG_I7_C1                            __GE_REG(0x44)
#define GE_REG_I8_C0                            __GE_REG(0x45)
#define GE_REG_I8_C1                            __GE_REG(0x46)
#define GE_REG_I9_C0                            __GE_REG(0x47)
#define GE_REG_I9_C1                            __GE_REG(0x48)
#define GE_REG_I10_C0                           __GE_REG(0x49)
#define GE_REG_I10_C1                           __GE_REG(0x4a)
#define GE_REG_I11_C0                           __GE_REG(0x4b)
#define GE_REG_I11_C1                           __GE_REG(0x4c)
#define GE_REG_I12_C0                           __GE_REG(0x4d)
#define GE_REG_I12_C1                           __GE_REG(0x4e)
#define GE_REG_I13_C0                           __GE_REG(0x4f)
#define GE_REG_I13_C1                           __GE_REG(0x50)
#define GE_REG_I14_C0                           __GE_REG(0x51)
#define GE_REG_I14_C1                           __GE_REG(0x52)
#define GE_REG_I15_C0                           __GE_REG(0x53)
#define GE_REG_I15_C1                           __GE_REG(0x54)

#define GE_REG_CLIP_LEFT                        __GE_REG(0x55)
#define GE_REG_CLIP_RIGHT                       __GE_REG(0x56)
#define GE_REG_CLIP_TOP                         __GE_REG(0x57)
#define GE_REG_CLIP_BOTTOM                      __GE_REG(0x58)

#define GE_REG_ROT                              __GE_REG(0x59)

#define GE_REG_STBB_SCK_TYPE                    __GE_REG(0x5b)
#define GE_REG_STBB_SCK_BG                      __GE_REG(0x5c)
#define GE_REG_STBB_SCK_AR                      __GE_REG(0x5d)

#define GE_REG_STBB_INI_DX                      __GE_REG(0x5e)
#define GE_REG_STBB_INI_DY                      __GE_REG(0x5f)
#define GE_REG_CMD                              __GE_REG(0x60)
#define GE_REG_LINE                             __GE_REG(0x61)
#define GE_REG_LPT                              __GE_REG(0x62)
#define GE_REG_LINE_LEN                         __GE_REG(0x63)
#define GE_REG_STBB_DX                          __GE_REG(0x64)
#define GE_REG_STBB_DY                          __GE_REG(0x65)
#define GE_REG_ITC0                             __GE_REG(0x66)
#define GE_REG_ITC1                             __GE_REG(0x67)
#define GE_REG_PRI_V0_X                         __GE_REG(0x68)
#define GE_REG_PRI_V0_Y                         __GE_REG(0x69)
#define GE_REG_PRI_V1_X                         __GE_REG(0x6a)
#define GE_REG_PRI_V1_Y                         __GE_REG(0x6b)
#define GE_REG_PRI_V2_X                         __GE_REG(0x6c)
#define GE_REG_PRI_V2_Y                         __GE_REG(0x6d)
#define GE_REG_STBB_S_W                         __GE_REG(0x6e)
#define GE_REG_STBB_S_H                         __GE_REG(0x6f)

#define GE_REG_PRI_GB_ST                        __GE_REG(0x70)
#define GE_REG_PRI_AR_ST                        __GE_REG(0x71)

#define GE_REG_PRI_R_DX0                        __GE_REG(0x72)
#define GE_REG_PRI_R_DX1                        __GE_REG(0x73)
#define GE_REG_PRI_R_DY0                        __GE_REG(0x74)
#define GE_REG_PRI_R_DY1                        __GE_REG(0x75)
#define GE_REG_PRI_G_DX0                        __GE_REG(0x76)
#define GE_REG_PRI_G_DX1                        __GE_REG(0x77)
#define GE_REG_PRI_G_DY0                        __GE_REG(0x78)
#define GE_REG_PRI_G_DY1                        __GE_REG(0x79)
#define GE_REG_PRI_B_DX0                        __GE_REG(0x7a)
#define GE_REG_PRI_B_DX1                        __GE_REG(0x7b)
#define GE_REG_PRI_B_DY0                        __GE_REG(0x7c)
#define GE_REG_PRI_B_DY1                        __GE_REG(0x7d)
#define GE_REG_PRI_A_DX0                        __GE_REG(0x7e)
#define GE_REG_PRI_A_DY0                        __GE_REG(0x7f)

#define PE_VAL_EN_CMDQ                          (1 << 0)
#define PE_VAL_EN_VCMDQ                         (1 << 1)
#define PE_VAL_EN_Read_Prio                    (1 << 2)
#define PE_VAL_EN_Write_Prio                    (1 << 3)
#define PE_VAL_EN_STRETCH_BITBLT                (1 << 4)
#define PE_VAL_EN_ITALIC_FONT                   (1 << 6)

//#define PE_REG_EN                             0x00
#define PE_VAL_EN_PE                            (1 << 0)
#define PE_VAL_EN_DITHER                        (1 << 1)
#define PE_VAL_EN_GY_ABL                        (1 << 2)
#define PE_VAL_EN_ROP                           (1 << 5)
#define PE_VAL_EN_SCK                           (1 << 6)
#define PE_VAL_EN_DCK                           (1 << 7)
#define PE_VAL_EN_LPT                           (1 << 8)

#define PE_MSK_PRIM_TYPE                        (0xF << 4)
#define PE_VAL_PRIM_LINE                        (0x1 << 4)
#define PE_VAL_PRIM_RECTANGLE                   (0x3 << 4)
#define PE_VAL_PRIM_BITBLT                      (0x4 << 4)
#define PE_MSK_CLEAR_FLAG                       (0x1FF << 7)
#define PE_VAL_DRAW_SRC_DIR_X_NEG               (1 << 7)
#define PE_VAL_DRAW_SRC_DIR_Y_NEG               (1 << 8)
#define PE_VAL_DRAW_DST_DIR_X_NEG               (1 << 9)
#define PE_VAL_DRAW_DST_DIR_Y_NEG               (1 << 10)
#define PE_VAL_LINE_GRADIENT                    (1 << 11)
#define PE_VAL_RECT_GRADIENT_H                  (1 << 12)
#define PE_VAL_RECT_GRADIENT_V                  (1 << 13)
#define PE_VAL_STBB_NEAREST                     (1 << 14)
#define PE_VAL_STBB_PATCH                       (1 << 15)

#define PE_MSK_LP                               0x3F
#define PE_MSK_LPT_FACTOR                       (0x3<<6)
#define PE_VAL_LPT_F1                           (0<<6)
#define PE_VAL_LPT_F2                           (1<<6)
#define PE_VAL_LPT_F3                           (2<<6)
#define PE_VAL_LPT_F4                           (3<<6)
#define PE_VAL_LPT_RESET                        (1<<8)
#define PE_VAL_LINE_LAST                        (1<<9)

//------------------------------------------------------------------------------
// CACHE : Base 0x2B80
//------------------------------------------------------------------------------
#define REG_CACHE_BYPASS                        0x2BA0

    #define CACHE_BYPASS_EN                         0x01

//----------------------------------------------------------------------------
// Bank9 - DSP
//----------------------------------------------------------------------------
#define DSP_DSPCTRLPORT                         0x80
#define DSP_DSPBRGDATA                          0x81
#define DSP_IDMAOVALAPH                         0x82
#define DSP_IDMAOVALAPL                         0x83
#define DSP_IDMABASEADDRH                       0x84
#define DSP_IDMABASEADDRL                       0x85
#define DSP_IDMATRSIZEH                         0x86
#define DSP_IDMATRSIZEL                         0x87
#define DSP_CH2_IDMABASEADDRH                   0x88
#define DSP_CH2_IDMABASEADDRL                   0x89
#define DSP_CH2_IDMATRSIZEH                     0x8A
#define DSP_CH2_IDMATRSIZEL                     0x8B
#define DSP_RDDATAH                             0x8C
#define DSP_RDDATAL                             0x8D
#define DSP_RDBASEADDRH                         0x8E
#define DSP_RDBASEADDRL                         0x8F

#define DSP_AUD_CTRL                            0xF0
#define DSP_STR_TYPE                            0xF1
#define DSP_MAD_BASE                            0xF2
#define DSP_ES_MBASE_H                          0xF3
#define DSP_ES_MEND_H                           0xF4
#define DSP_PCM_MBASE_H                         0xF5
#define DSP_PCM_MEND_H                          0xF6
#define DSP_P_AUD_MODE                          0xF7

/*****************************************************************************/
//MCU reg
/*****************************************************************************/
// PIU base address :0x3c00
// Multi-SPI Flash chip select
#define REG_CSZ_SPI_FLASH                       0x3C1C
#define DMA_FLAGS                               0x3C20

#define MCU_WDT_KEY_L                           0x3C60
#define MCU_WDT_KEY_H                           0x3C61
#define WDT_TIMER_0                             0x3C62
#define WDT_TIMER_1                             0x3C63
#define WDT_INT_TIMER_0                         0x3C64
#define WDT_INT_TIMER_1                         0x3C65
#define WDT_RST                                 0x3C66

#define REG_TIMER0_MAX                          0x3C80
#define REG_TIMER0_CNT_CAP                      0x3C84
#define REG_TIMER0_CTRL                         0x3C88
#define REG_TIMER0_EN                           0x3C89

#define REG_TIMER1_MAX                          0x3CA0
#define REG_TIMER1_CNT_CAP                      0x3CA4
#define REG_TIMER1_CTRL                         0x3CA8
#define REG_TIMER1_EN                           0x3CA9

    #define TIMER_CTRL_CAP                          BIT0
    #define TIMER_CTRL_CLR                          BIT1

#define DMA_SRC_ADDR_0                          0x3CE0
#define DMA_SRC_ADDR_1                          0x3CE1
#define DMA_SRC_ADDR_2                          0x3CE2
#define DMA_SRC_ADDR_3                          0x3CE3
#define DMA_DST_ADDR_0                          0x3CE4
#define DMA_DST_ADDR_1                          0x3CE5
#define DMA_DST_ADDR_2                          0x3CE6
#define DMA_DST_ADDR_3                          0x3CE7
#define DMA_SIZE_0                              0x3CE8
#define DMA_SIZE_1                              0x3CE9
#define DMA_SIZE_2                              0x3CEA
#define DMA_SIZE_3                              0x3CEB

#define DMA_CTRL                                0x3CEC

#define SPI_OFFSET                              0x3C1A

//------------------------------------------------------------------------------
// SAR define
#define REG_SAR_CTRL0                           0x3A00
#define REG_SAR_CTRL1                           0x3A01
#define REG_SAR_CTRL2                           0x3A02
#define REG_SAR_CTRL3                           0x3A03
#define REG_SAR_CTRL20                          0x3A20
#define REG_SAR_CTRL21                          0x3A21
#define REG_SAR_CTRL24							0x3A24

#define REG_SAR_ADCOUT1                         0x3A18
#define REG_SAR_ADCOUT2                         0x3A1A
#define REG_SAR_ADCOUT3                         0x3A1C
#define REG_SAR_ADCOUT4                         0x3A1E

    #define MASK_SAR_ADCOUT                         0x3F

#define GPIO_BASE_ADDRESS                       0x1048
#define GPIO_P0_CTRL                            0x1048
#define GPIO_P0_OE                              0x1049
#define GPIO_P0_IN                              0x104A
#define GPIO_P1_CTRL                            0x104B
#define GPIO_P1_OE                              0x104C
#define GPIO_P1_IN                              0x104D
#define GPIO_P2_CTRL                            0x104E
#define GPIO_P2_OE                              0x104F
#define GPIO_P2_IN                              0x1050
#define GPIO_P3_CTRL                            0x1051
#define GPIO_P3_OE                              0x1052
#define GPIO_P3_IN                              0x1053
#define GPIO_P4_CTRL                            0x1054
#define GPIO_P4_OE                              0x1055
#define GPIO_P4_IN                              0x1056

    #define _OFF                                    0
    #define _ON                                     1
    #define _HIGH                                   1
    #define _LOW                                    0
    #define _OUTPUT                                 0
    #define _INPUT                                  1

//----------------------------------------------------------------------------
// XDMIU Reg :: 0x2BC0
//----------------------------------------------------------------------------
#define XDMIU_RESET                             0x2BC0

    #define XDMIU_SW_RESET                          0x01

#define XDMIU_MISC                              0x2BC4

    #define XDMIU_R_PRI                             0x01
    #define XDMIU_W_PRI                             0x02
    #define XDMIU_MEM_MAP_EN                        0x04

#define XDMIU_MCUXD_W0_S1K                      0x2BC6
#define XDMIU_MCUXD_W0_E1K                      0x2BC7
#define XDMIU_MEMMAP_W0_64K                     0x2BC8
#define XDMIU_MEMMAP_W0_64K_1                   0x2BC9
#define XDMIU_MCUXD_W1_S1K                      0x2BCA
#define XDMIU_MCUXD_W1_E2K                      0x2BCB
#define XDMIU_MEMMAP_W1_4K                      0x2BCC
#define XDMIU_MEMMAP_W1_4K_1                    0x2BCD

//------------------------------------------------------------------------------
// Mail Box Reg
//------------------------------------------------------------------------------
#define REG_51_MB_CMD_BASE                      MAILBOX_REG_BASE
#define REG_AEON_MB_CMD_BASE                    (MAILBOX_REG_BASE + 0x10)

#define MB_51_REG_CTRL_REG                      0x3380
#define MB_51_REG_CMD_CLASS                     0x3381
#define MB_51_REG_CMD_IDX                       0x3382
#define MB_51_REG_PARAM_CNT                     0x3383
#define MB_51_REG_PARAM_00                      0x3384
#define MB_51_REG_PARAM_01                      0x3385
#define MB_51_REG_PARAM_02                      0x3386
#define MB_51_REG_PARAM_03                      0x3387
#define MB_51_REG_PARAM_04                      0x3388
#define MB_51_REG_PARAM_05                      0x3389
#define MB_51_REG_PARAM_06                      0x338A
#define MB_51_REG_PARAM_07                      0x338B
#define MB_51_REG_PARAM_08                      0x338C
#define MB_51_REG_PARAM_09                      0x338D
#define MB_51_REG_Status_0                      0x338E
#define MB_51_REG_Status_1                      0x338F

#define MB_AEON_REG_CTRL_REG                    0x3390
#define MB_AEON_REG_CMD_CLASS                   0x3391
#define MB_AEON_REG_CMD_IDX                     0x3392
#define MB_AEON_REG_PARAM_CNT                   0x3393
#define MB_AEON_REG_PARAM_00                    0x3394
#define MB_AEON_REG_PARAM_01                    0x3395
#define MB_AEON_REG_PARAM_02                    0x3396
#define MB_AEON_REG_PARAM_03                    0x3397
#define MB_AEON_REG_PARAM_04                    0x3398
#define MB_AEON_REG_PARAM_05                    0x3399
#define MB_AEON_REG_PARAM_06                    0x339A
#define MB_AEON_REG_PARAM_07                    0x339B
#define MB_AEON_REG_PARAM_08                    0x339C
#define MB_AEON_REG_PARAM_09                    0x339D
#define MB_AEON_REG_Status_0                    0x339E
#define MB_AEON_REG_Status_1                    0x339F

// AEON Index
#define MB_AEON_IDX_HK51_STATUS                 0x80
#define MB_AEON_IDX_PANEL_INFO                  0x81
#define MB_AEON_IDX_QUERY_SYSINFO               0x82
#define MB_AEON_IDX_AEON_HEARTBEAT              0x83

//------------------------------------------------------------------------------
// DDC Reg
//------------------------------------------------------------------------------
//DDC2BI for ADC
#define DDC2BI_ADC_ID                           0x040A
#define DDC2BI_ADC_INT_MASK                     0x0414
#define DDC2BI_ADC_INT_FLAG                     0x0410
#define DDC2BI_ADC_INT_CLR                      0x041C
#define DDC2BI_ADC_WB_RP                        0x0400
#define DDC2BI_ADC_RB_WP                        0x0401

//DDC2BI for DVI0
#define DDC2BI_DVI0_ID                          0x040C
#define DDC2BI_DVI0_INT_MASK                    0x0416
#define DDC2BI_DVI0_INT_FLAG                    0x0412
#define DDC2BI_DVI0_INT_CLR                     0x041E
#define DDC2BI_DVI0_WB_RP                       0x0404
#define DDC2BI_DVI0_RB_WP                       0x0405

//DDC2BI for DVI1
#define DDC2BI_DVI1_ID                          0x040D
#define DDC2BI_DVI1_INT_MASK                    0x0417
#define DDC2BI_DVI1_INT_FLAG                    0x0413
#define DDC2BI_DVI1_INT_CLR                     0x041F
#define DDC2BI_DVI1_WB_RP                       0x0406
#define DDC2BI_DVI1_RB_WP                       0x0407

//DDC2BI for DVI2
#define DDC2BI_DVI2_ID                          0x0452
#define DDC2BI_DVI2_INT_MASK                    0x0455
#define DDC2BI_DVI2_INT_FLAG                    0x0454
#define DDC2BI_DVI2_INT_CLR                     0x0457
#define DDC2BI_DVI2_WB_RP                       0x0450
#define DDC2BI_DVI2_RB_WP                       0x0451

#define DDC2BI_DVI_INT_FLAG                     0x0412
#define DDC2BI_DVI_INT_CLR                      0x041E
#define DDC2BI_DVI_WB_RP                        0x0404
#define DDC2BI_DVI_RB_WP                        0x0405
#define DDC2BI_CTRL                             0x0409

//------------------------------------------------------------------------------
// NOR Paral Flash
//------------------------------------------------------------------------------
//#define REG_NORPF_WEB_END		PFSH_BASE + 0x86
#define REG_NORPF_CLOCK_LENGTH		            0x0580
#define REG_NORPF_FLASH_DATA_WIDTH8	            0x0580
#define REG_NORPF_WRITE_RUN 		            0x0582
#define REG_NORPF_DIRECT_READ_MODE	            0x0582
#define REG_NORPF_CEB_START		                0x0584
#define REG_NORPF_CEB_END		                0x0584
#define REG_NORPF_OEB_START		                0x0584
#define REG_NORPF_OEB_END		                0x0584
#define REG_NORPF_WEB_START		                0x0586
#define REG_NORPF_WEB_END		                0x0586
#define REG_NORPF_DATAOEN_START		            0x0586
#define REG_NORPF_DATAOEN_END		            0x0586
#define REG_NORPF_DATA_LATCH_CNT	            0x0588
#define REG_NORPF_CTRL			                0x058C
#define REG_NORPF_BRIDGE_CTRL		            0x058E
#define REG_NORPF_XIU_CTRL		                0x0590
#define REG_NORPF_XIU_ADDR_L		            0x0592
#define REG_NORPF_XIU_ADDR_H		            0x0594
#define REG_NORPF_XIU_WDATA		                0x0596
#define REG_NORPF_XIU_STATUS		            0x0598
#define REG_NORPF_XIU_RDATA_L		            0x059A
#define REG_NORPF_XIU_RDATA_H		            0x059C
#define REG_NORPF_WRITE_ADDR0_L		            0x05A0
#define REG_NORPF_WRITE_ADDR0_H		            0x05A2
#define REG_NORPF_WRITE_ADDR1_L		            0x05A4
#define REG_NORPF_WRITE_ADDR1_H		            0x05A6
#define REG_NORPF_WRITE_ADDR2_L		            0x05A8
#define REG_NORPF_WRITE_ADDR2_H		            0x05AA
#define REG_NORPF_WRITE_ADDR3_L		            0x05AC
#define REG_NORPF_WRITE_ADDR3_H		            0x05AE
#define REG_NORPF_WRITE_ADDR4_L		            0x05B0
#define REG_NORPF_WRITE_ADDR4_H		            0x05B2
#define REG_NORPF_WRITE_ADDR5_L		            0x05B4
#define REG_NORPF_WRITE_ADDR5_H		            0x05B6
#define REG_NORPF_WRITE_ADDR6_L		            0x05B8
#define REG_NORPF_WRITE_ADDR6_H		            0x05BA
#define REG_NORPF_WRITE_ADDR7_L		            0x05BC
#define REG_NORPF_WRITE_ADDR7_H		            0x05BE
#define REG_NORPF_WRITE_DATA0		            0x05C0
#define REG_NORPF_WRITE_DATA1		            0x05C2
#define REG_NORPF_WRITE_DATA2		            0x05C4
#define REG_NORPF_WRITE_DATA3		            0x05C6
#define REG_NORPF_WRITE_DATA4		            0x05C8
#define REG_NORPF_WRITE_DATA5		            0x05CA
#define REG_NORPF_WRITE_DATA6		            0x05CC
#define REG_NORPF_WRITE_DATA7		            0x05CE

#endif // _HWREG_S4M_H



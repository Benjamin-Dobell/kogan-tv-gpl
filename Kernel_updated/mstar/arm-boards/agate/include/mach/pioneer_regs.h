/*-----------------------------------------------------------------------------
 *                                                                           
 * Copyright (c) 2009 MStar Semiconductor, Inc.  All rights reserved.
 *                                                                          
 *-----------------------------------------------------------------------------
 * FILE
 *      pioneer_regs.h
 *
 * DESCRIPTION
 *       Pioneer physical address of the register.
 *
 * HISTORY
 *      2009.08.18       Jimmy Hsu     Initial Version
 *
 *-----------------------------------------------------------------------------*/
#ifndef PIONEER_REGS_H
#define PIONEER_REGS_H

#define PIONEER_RIU_BASE                       0xA0000000
#define PIONEER_ARM_IRQ_BASE                   0xA0000A00
#define PIONEER_ISP_BASE                       0xA0001000
#define PIONEER_FM_RF_BASE                     0xA0001200
#define PIONEER_FM_DS_BASE                     0xA0001400
#define PIONEER_FM_RDS_BASE                    0xA0001600
#define PIONEER_FM_RX_BASE                     0xA0001800
#define PIONEER_RIU_DBG_BASE                   0xA0001D80
#define PIONEER_ARM_BASE                       0xA0001E00
#define PIONEER_MCU_BASE                       0xA0002000
#define PIONEER_MVD_BASE                       0xA0002200
#define PIONEER_MIU_BASE                       0xA0002400
#define PIONEER_MVOP_BASE                      0xA0002800
#define PIONEER_TSP0_BASE                      0xA0002A00
#define PIONEER_TSP1_BASE                      0xA0002C00
#define PIONEER_SEMAPH_BASE                    0xA0003000
#define PIONEER_POR_STATUS_BASE                0xA0003080
#define PIONEER_OP2_DITH_BASE                  0xA0003800
#define PIONEER_OP2_PIP_BASE                   0xA0003A00
#define PIONEER_CHIPTOP_BASE                   0xA0003C00
#define PIONEER_GOP_BASE                       0xA0003E00
#define PIONEER_FCIE0_BASE                     0xA0004000
#define PIONEER_FCIE1_BASE                     0xA0004200
#define PIONEER_FCIE2_BASE                     0xA0004400
#define PIONEER_FCIE3_BASE                     0xA0004600
#define PIONEER_MPIF_BASE                      0xA0004800
#define PIONEER_MPLL_BASE                      0xA0004A00
#define PIONEER_CPUPLL_BASE                    0xA0004C00
#define PIONEER_GE_BASE                        0xA0005000
#define PIONEER_STRLD_BASE                     0xA0005200
#define PIONEER_JPD_BASE                       0xA0005400
#define PIONEER_MCU_IRQ_BASE                   0xA0005600
#define PIONEER_CACHE_BASE                     0xA0005700
#define PIONEER_XDMIU_BASE                     0xA0005780
#define PIONEER_VIVALDI0_BASE                  0xA0005800
#define PIONEER_VIVALDI1_BASE                  0xA0005A00
#define PIONEER_SC0_BASE                       0xA0005C00
#define PIONEER_SC1_BASE                       0xA0005E00
#define PIONEER_SC2_BASE                       0xA0006000
#define PIONEER_SC3_BASE                       0xA0006200
#define PIONEER_SC4_BASE                       0xA0006400
#define PIONEER_MAILBOX_BASE                   0xA0006700
#define PIONEER_IIC_BASE                       0xA0006840
#define PIONEER_PWM_BASE                       0xA0006880
#define PIONEER_RTC_BASE                       0xA0006900
#define PIONEER_PM_BASE                        0xA0006940
#define PIONEER_TOUCH_BASE                     0xA0006A00
#define PIONEER_NFIE_BASE                      0xA0006C00
#define PIONEER_CPUIF_BASE                     0xA0006E00
#define PIONEER_CPUIF_BASE                     0xA0006E00
#define PIONEER_RTC_STORAGE_BASE               0xA0007000
#define PIONEER_RTC_CCFL_BASE                  0xA0007200
#define PIONEER_PATGEN_BASE                    0xA0007300
#define PIONEER_SAR_ADC_BASE                   0xA0007400
#define PIONEER_SDM_ADC_BASE                   0xA0007500
#define PIONEER_UART0_BASE                     0xA0007600
#define PIONEER_UART1_BASE                     0xA0007680
#define PIONEER_UART2_BASE                     0xA0007700
#define PIONEER_TIMER2_BASE                    0xA0007800
#define PIONEER_IR_BASE                        0xA0007780
#define PIONEER_PIU_MISC_BASE                  0xA0007A00
#define PIONEER_G3D_BASE                       0xA0007C00
#define PIONEER_UTMI_BASE                      0xA0007E00
#define PIONEER_OTG0_BASE                      0xA0008000
#define PIONEER_OTG1_BASE                      0xA0008200
#define PIONEER_OTG2_BASE                      0xA0008400
#define PIONEER_OTG3_BASE                      0xA0008600
#define PIONEER_USBC_BASE                      0xA0008A00

#define PIONEER_MCU_CODE_BASE                  0x08000000

// CHIPTOP
#define PIONEER_GPIO_REG_L(x)                  (PIONEER_CHIPTOP_BASE | ((x) << 2))
#define PIONEER_GPIO_REG_H(x)                  ((PIONEER_CHIPTOP_BASE | ((x) << 2)) + 1)
#define PIONEER_CHIPTOP_GPIO_IP_EN						 PIONEER_GPIO_REG_L(0x08)
#define PIONEER_CHIPTOP_GPIO_INT_EN            PIONEER_GPIO_REG_L(0x0C)
#define PIONEER_CHIPTOP_GPIO_INT_POLARITY      PIONEER_GPIO_REG_L(0x0D)
#define PIONEER_CHIPTOP_GPIO_G_OEN             PIONEER_GPIO_REG_L(0x40)
#define PIONEER_CHIPTOP_GPIO_G_OUT             PIONEER_GPIO_REG_L(0x41)
#define PIONEER_CHIPTOP_GPIO_G_IN              PIONEER_GPIO_REG_L(0x42)
#define PIONEER_CHIPTOP_GPIO_A_OEN             PIONEER_GPIO_REG_L(0x46)
#define PIONEER_CHIPTOP_GPIO_A_OUT             PIONEER_GPIO_REG_L(0x47)
#define PIONEER_CHIPTOP_GPIO_A_IN              PIONEER_GPIO_REG_L(0x48)
#define PIONEER_CHIPTOP_GPIO_G_PU              PIONEER_GPIO_REG_L(0x55)
#define PIONEER_CHIPTOP_GPIO_G_PD              PIONEER_GPIO_REG_L(0x56)
#define PIONEER_CHIPTOP_GPIO_U_OEN             PIONEER_GPIO_REG_L(0x5A)
#define PIONEER_CHIPTOP_GPIO_U_OUT             PIONEER_GPIO_REG_H(0x5A)
#define PIONEER_CHIPTOP_GPIO_U_PU              PIONEER_GPIO_REG_L(0x5B)
#define PIONEER_CHIPTOP_GPIO_U_PD              PIONEER_GPIO_REG_H(0x5B)
#define PIONEER_CHIPTOP_GPIO_U_IN              PIONEER_GPIO_REG_L(0x5C)

#endif // PIONEER_REGS_H

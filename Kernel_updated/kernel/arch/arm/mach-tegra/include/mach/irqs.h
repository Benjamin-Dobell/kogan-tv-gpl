/*
 * arch/arm/mach-tegra/include/mach/irqs.h
 *
 * Copyright (C) 2010 Google, Inc.
 * Copyright (C) 2011 NVIDIA Corporation.
 *
 * Author:
 *	Colin Cross <ccross@google.com>
 *	Erik Gilling <konkers@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MACH_TEGRA_IRQS_H
#define __MACH_TEGRA_IRQS_H

#define INT_GIC_BASE			0

#define IRQ_LOCALTIMER                  29

#ifdef CONFIG_ARCH_TEGRA_2x_SOC
/* Primary Interrupt Controller */
#define INT_PRI_BASE			(INT_GIC_BASE + 32)
#define INT_TMR1			(INT_PRI_BASE + 0)
#define INT_TMR2			(INT_PRI_BASE + 1)
#define INT_RTC				(INT_PRI_BASE + 2)
#define INT_I2S2			(INT_PRI_BASE + 3)
#define INT_SHR_SEM_INBOX_IBF		(INT_PRI_BASE + 4)
#define INT_SHR_SEM_INBOX_IBE		(INT_PRI_BASE + 5)
#define INT_SHR_SEM_OUTBOX_IBF		(INT_PRI_BASE + 6)
#define INT_SHR_SEM_OUTBOX_IBE		(INT_PRI_BASE + 7)
#define INT_VDE_UCQ_ERROR		(INT_PRI_BASE + 8)
#define INT_VDE_SYNC_TOKEN		(INT_PRI_BASE + 9)
#define INT_VDE_BSE_V			(INT_PRI_BASE + 10)
#define INT_VDE_BSE_A			(INT_PRI_BASE + 11)
#define INT_VDE_SXE			(INT_PRI_BASE + 12)
#define INT_I2S1			(INT_PRI_BASE + 13)
#define INT_SDMMC1			(INT_PRI_BASE + 14)
#define INT_SDMMC2			(INT_PRI_BASE + 15)
#define INT_XIO				(INT_PRI_BASE + 16)
#define INT_VDE				(INT_PRI_BASE + 17)
#define INT_AVP_UCQ			(INT_PRI_BASE + 18)
#define INT_SDMMC3			(INT_PRI_BASE + 19)
#define INT_USB				(INT_PRI_BASE + 20)
#define INT_USB2			(INT_PRI_BASE + 21)
#define INT_PRI_RES_22			(INT_PRI_BASE + 22)
#define INT_EIDE			(INT_PRI_BASE + 23)
#define INT_NANDFLASH			(INT_PRI_BASE + 24)
#define INT_VCP				(INT_PRI_BASE + 25)
#define INT_APB_DMA			(INT_PRI_BASE + 26)
#define INT_AHB_DMA			(INT_PRI_BASE + 27)
#define INT_GNT_0			(INT_PRI_BASE + 28)
#define INT_GNT_1			(INT_PRI_BASE + 29)
#define INT_OWR				(INT_PRI_BASE + 30)
#define INT_SDMMC4			(INT_PRI_BASE + 31)

/* Secondary Interrupt Controller */
#define INT_SEC_BASE			(INT_PRI_BASE + 32)
#define INT_GPIO1			(INT_SEC_BASE + 0)
#define INT_GPIO2			(INT_SEC_BASE + 1)
#define INT_GPIO3			(INT_SEC_BASE + 2)
#define INT_GPIO4			(INT_SEC_BASE + 3)
#define INT_UARTA			(INT_SEC_BASE + 4)
#define INT_UARTB			(INT_SEC_BASE + 5)
#define INT_I2C				(INT_SEC_BASE + 6)
#define INT_SPI				(INT_SEC_BASE + 7)
#define INT_TWC				(INT_SEC_BASE + 8)
#define INT_TMR3			(INT_SEC_BASE + 9)
#define INT_TMR4			(INT_SEC_BASE + 10)
#define INT_FLOW_RSM0			(INT_SEC_BASE + 11)
#define INT_FLOW_RSM1			(INT_SEC_BASE + 12)
#define INT_SPDIF			(INT_SEC_BASE + 13)
#define INT_UARTC			(INT_SEC_BASE + 14)
#define INT_MIPI			(INT_SEC_BASE + 15)
#define INT_EVENTA			(INT_SEC_BASE + 16)
#define INT_EVENTB			(INT_SEC_BASE + 17)
#define INT_EVENTC			(INT_SEC_BASE + 18)
#define INT_EVENTD			(INT_SEC_BASE + 19)
#define INT_VFIR			(INT_SEC_BASE + 20)
#define INT_DVC				(INT_SEC_BASE + 21)
#define INT_SYS_STATS_MON		(INT_SEC_BASE + 22)
#define INT_GPIO5			(INT_SEC_BASE + 23)
#define INT_CPU0_PMU_INTR		(INT_SEC_BASE + 24)
#define INT_CPU1_PMU_INTR		(INT_SEC_BASE + 25)
#define INT_SEC_RES_26			(INT_SEC_BASE + 26)
#define INT_SPI_1			(INT_SEC_BASE + 27)
#define INT_APB_DMA_COP			(INT_SEC_BASE + 28)
#define INT_AHB_DMA_COP			(INT_SEC_BASE + 29)
#define INT_DMA_TX			(INT_SEC_BASE + 30)
#define INT_DMA_RX			(INT_SEC_BASE + 31)

/* Tertiary Interrupt Controller */
#define INT_TRI_BASE			(INT_SEC_BASE + 32)
#define INT_HOST1X_COP_SYNCPT		(INT_TRI_BASE + 0)
#define INT_HOST1X_MPCORE_SYNCPT	(INT_TRI_BASE + 1)
#define INT_HOST1X_COP_GENERAL		(INT_TRI_BASE + 2)
#define INT_HOST1X_MPCORE_GENERAL	(INT_TRI_BASE + 3)
#define INT_MPE_GENERAL			(INT_TRI_BASE + 4)
#define INT_VI_GENERAL			(INT_TRI_BASE + 5)
#define INT_EPP_GENERAL			(INT_TRI_BASE + 6)
#define INT_ISP_GENERAL			(INT_TRI_BASE + 7)
#define INT_2D_GENERAL			(INT_TRI_BASE + 8)
#define INT_DISPLAY_GENERAL		(INT_TRI_BASE + 9)
#define INT_DISPLAY_B_GENERAL		(INT_TRI_BASE + 10)
#define INT_HDMI			(INT_TRI_BASE + 11)
#define INT_TVO_GENERAL			(INT_TRI_BASE + 12)
#define INT_MC_GENERAL			(INT_TRI_BASE + 13)
#define INT_EMC_GENERAL			(INT_TRI_BASE + 14)
#define INT_TRI_RES_15			(INT_TRI_BASE + 15)
#define INT_TRI_RES_16			(INT_TRI_BASE + 16)
#define INT_AC97			(INT_TRI_BASE + 17)
#define INT_SPI_2			(INT_TRI_BASE + 18)
#define INT_SPI_3			(INT_TRI_BASE + 19)
#define INT_I2C2			(INT_TRI_BASE + 20)
#define INT_KBC				(INT_TRI_BASE + 21)
#define INT_EXTERNAL_PMU		(INT_TRI_BASE + 22)
#define INT_GPIO6			(INT_TRI_BASE + 23)
#define INT_TVDAC			(INT_TRI_BASE + 24)
#define INT_GPIO7			(INT_TRI_BASE + 25)
#define INT_UARTD			(INT_TRI_BASE + 26)
#define INT_UARTE			(INT_TRI_BASE + 27)
#define INT_I2C3			(INT_TRI_BASE + 28)
#define INT_SPI_4			(INT_TRI_BASE + 29)
#define INT_TRI_RES_30			(INT_TRI_BASE + 30)
#define INT_SW_RESERVED			(INT_TRI_BASE + 31)

/* Quaternary Interrupt Controller */
#define INT_QUAD_BASE			(INT_TRI_BASE + 32)
#define INT_SNOR			(INT_QUAD_BASE + 0)
#define INT_USB3			(INT_QUAD_BASE + 1)
#define INT_PCIE_INTR			(INT_QUAD_BASE + 2)
#define INT_PCIE_MSI			(INT_QUAD_BASE + 3)
#define INT_QUAD_RES_4			(INT_QUAD_BASE + 4)
#define INT_QUAD_RES_5			(INT_QUAD_BASE + 5)
#define INT_QUAD_RES_6			(INT_QUAD_BASE + 6)
#define INT_QUAD_RES_7			(INT_QUAD_BASE + 7)
#define INT_APB_DMA_CH0			(INT_QUAD_BASE + 8)
#define INT_APB_DMA_CH1			(INT_QUAD_BASE + 9)
#define INT_APB_DMA_CH2			(INT_QUAD_BASE + 10)
#define INT_APB_DMA_CH3			(INT_QUAD_BASE + 11)
#define INT_APB_DMA_CH4			(INT_QUAD_BASE + 12)
#define INT_APB_DMA_CH5			(INT_QUAD_BASE + 13)
#define INT_APB_DMA_CH6			(INT_QUAD_BASE + 14)
#define INT_APB_DMA_CH7			(INT_QUAD_BASE + 15)
#define INT_APB_DMA_CH8			(INT_QUAD_BASE + 16)
#define INT_APB_DMA_CH9			(INT_QUAD_BASE + 17)
#define INT_APB_DMA_CH10		(INT_QUAD_BASE + 18)
#define INT_APB_DMA_CH11		(INT_QUAD_BASE + 19)
#define INT_APB_DMA_CH12		(INT_QUAD_BASE + 20)
#define INT_APB_DMA_CH13		(INT_QUAD_BASE + 21)
#define INT_APB_DMA_CH14		(INT_QUAD_BASE + 22)
#define INT_APB_DMA_CH15		(INT_QUAD_BASE + 23)
#define INT_QUAD_RES_24			(INT_QUAD_BASE + 24)
#define INT_QUAD_RES_25			(INT_QUAD_BASE + 25)
#define INT_QUAD_RES_26			(INT_QUAD_BASE + 26)
#define INT_QUAD_RES_27			(INT_QUAD_BASE + 27)
#define INT_QUAD_RES_28			(INT_QUAD_BASE + 28)
#define INT_QUAD_RES_29			(INT_QUAD_BASE + 29)
#define INT_QUAD_RES_30			(INT_QUAD_BASE + 30)
#define INT_QUAD_RES_31			(INT_QUAD_BASE + 31)

#define INT_GIC_NR			(INT_QUAD_BASE + 32)

#define INT_MAIN_NR			(INT_GIC_NR - INT_PRI_BASE)

#define INT_SYNCPT_THRESH_BASE		(INT_QUAD_BASE + 32)
#define INT_SYNCPT_THRESH_NR		32

#define INT_GPIO_BASE			(INT_SYNCPT_THRESH_BASE + \
					 INT_SYNCPT_THRESH_NR)
#define INT_GPIO_NR			(28 * 8)

#define INT_PCI_MSI_BASE		(INT_GPIO_BASE + \
					 INT_GPIO_NR)
#define INT_PCI_MSI_NR			(0)

#elif defined(CONFIG_ARCH_TEGRA_3x_SOC)

/* Primary Interrupt Controller */
#define INT_PRI_BASE			(INT_GIC_BASE + 32)
#define INT_TMR1			(INT_PRI_BASE + 0)
#define INT_TMR2			(INT_PRI_BASE + 1)
#define INT_RTC				(INT_PRI_BASE + 2)
#define INT_CEC				(INT_PRI_BASE + 3)
#define INT_SHR_SEM_INBOX_IBF		(INT_PRI_BASE + 4)
#define INT_SHR_SEM_INBOX_IBE		(INT_PRI_BASE + 5)
#define INT_SHR_SEM_OUTBOX_IBF		(INT_PRI_BASE + 6)
#define INT_SHR_SEM_OUTBOX_IBE		(INT_PRI_BASE + 7)
#define INT_VDE_UCQ_ERROR		(INT_PRI_BASE + 8)
#define INT_VDE_SYNC_TOKEN		(INT_PRI_BASE + 9)
#define INT_VDE_BSE_V			(INT_PRI_BASE + 10)
#define INT_VDE_BSE_A			(INT_PRI_BASE + 11)
#define INT_VDE_SXE			(INT_PRI_BASE + 12)
#define INT_SATA_RX_STAT		(INT_PRI_BASE + 13)
#define INT_SDMMC1			(INT_PRI_BASE + 14)
#define INT_SDMMC2			(INT_PRI_BASE + 15)
#define INT_XIO				(INT_PRI_BASE + 16)
#define INT_VDE				(INT_PRI_BASE + 17)
#define INT_AVP_UCQ			(INT_PRI_BASE + 18)
#define INT_SDMMC3			(INT_PRI_BASE + 19)
#define INT_USB				(INT_PRI_BASE + 20)
#define INT_USB2			(INT_PRI_BASE + 21)
#define INT_HSMMC			(INT_PRI_BASE + 22)
#define INT_SATA_CTL			(INT_PRI_BASE + 23)
#define INT_NANDFLASH			(INT_PRI_BASE + 24)
#define INT_VCP				(INT_PRI_BASE + 25)
#define INT_APB_DMA			(INT_PRI_BASE + 26)
#define INT_AHB_DMA			(INT_PRI_BASE + 27)
#define INT_GNT_0			(INT_PRI_BASE + 28)
#define INT_GNT_1			(INT_PRI_BASE + 29)
#define INT_OWR				(INT_PRI_BASE + 30)
#define INT_SDMMC4			(INT_PRI_BASE + 31)

/* Secondary Interrupt Controller */
#define INT_SEC_BASE			(INT_PRI_BASE + 32)
#define INT_GPIO1			(INT_SEC_BASE + 0)
#define INT_GPIO2			(INT_SEC_BASE + 1)
#define INT_GPIO3			(INT_SEC_BASE + 2)
#define INT_GPIO4			(INT_SEC_BASE + 3)
#define INT_UARTA			(INT_SEC_BASE + 4)
#define INT_UARTB			(INT_SEC_BASE + 5)
#define INT_I2C				(INT_SEC_BASE + 6)
#define INT_SPI				(INT_SEC_BASE + 7)
#define INT_DTV				INT_SPI
#define INT_TWC				(INT_SEC_BASE + 8)
#define INT_TMR3			(INT_SEC_BASE + 9)
#define INT_TMR4			(INT_SEC_BASE + 10)
#define INT_FLOW_RSM0			(INT_SEC_BASE + 11)
#define INT_FLOW_RSM1			(INT_SEC_BASE + 12)
#define INT_ACTMON			(INT_SEC_BASE + 13)
#define INT_UARTC			(INT_SEC_BASE + 14)
#define INT_MIPI			(INT_SEC_BASE + 15)
#define INT_EVENTA			(INT_SEC_BASE + 16)
#define INT_EVENTB			(INT_SEC_BASE + 17)
#define INT_EVENTC			(INT_SEC_BASE + 18)
#define INT_EVENTD			(INT_SEC_BASE + 19)
#define INT_VFIR			(INT_SEC_BASE + 20)
#define INT_I2C5			(INT_SEC_BASE + 21)
#define INT_SYS_STATS_MON		(INT_SEC_BASE + 22)
#define INT_GPIO5			(INT_SEC_BASE + 23)
#define INT_SPEEDO_PMON_0		(INT_SEC_BASE + 24)
#define INT_SPEEDO_PMON_1		(INT_SEC_BASE + 25)
#define INT_SE				(INT_SEC_BASE + 26)
#define INT_SPI_1			(INT_SEC_BASE + 27)
#define INT_APB_DMA_COP			(INT_SEC_BASE + 28)
#define INT_AHB_DMA_COP			(INT_SEC_BASE + 29)
#define INT_DMA_TX			(INT_SEC_BASE + 30)
#define INT_DMA_RX			(INT_SEC_BASE + 31)

/* Tertiary Interrupt Controller */
#define INT_TRI_BASE			(INT_SEC_BASE + 32)
#define INT_HOST1X_COP_SYNCPT		(INT_TRI_BASE + 0)
#define INT_HOST1X_MPCORE_SYNCPT	(INT_TRI_BASE + 1)
#define INT_HOST1X_COP_GENERAL		(INT_TRI_BASE + 2)
#define INT_HOST1X_MPCORE_GENERAL	(INT_TRI_BASE + 3)
#define INT_MPE_GENERAL			(INT_TRI_BASE + 4)
#define INT_VI_GENERAL			(INT_TRI_BASE + 5)
#define INT_EPP_GENERAL			(INT_TRI_BASE + 6)
#define INT_ISP_GENERAL			(INT_TRI_BASE + 7)
#define INT_2D_GENERAL			(INT_TRI_BASE + 8)
#define INT_DISPLAY_GENERAL		(INT_TRI_BASE + 9)
#define INT_DISPLAY_B_GENERAL		(INT_TRI_BASE + 10)
#define INT_HDMI			(INT_TRI_BASE + 11)
#define INT_TVO_GENERAL			(INT_TRI_BASE + 12)
#define INT_MC_GENERAL			(INT_TRI_BASE + 13)
#define INT_EMC_GENERAL			(INT_TRI_BASE + 14)
#define INT_SPI_6			(INT_SEC_BASE + 15)
#define INT_NOR_FLASH			(INT_TRI_BASE + 16)
#define INT_HDA				(INT_TRI_BASE + 17)
#define INT_SPI_2			(INT_TRI_BASE + 18)
#define INT_SPI_3			(INT_TRI_BASE + 19)
#define INT_I2C2			(INT_TRI_BASE + 20)
#define INT_KBC				(INT_TRI_BASE + 21)
#define INT_EXTERNAL_PMU		(INT_TRI_BASE + 22)
#define INT_GPIO6			(INT_TRI_BASE + 23)
#define INT_TVDAC			(INT_TRI_BASE + 24)
#define INT_GPIO7			(INT_TRI_BASE + 25)
#define INT_UARTD			(INT_TRI_BASE + 26)
#define INT_UARTE			(INT_TRI_BASE + 27)
#define INT_I2C3			(INT_TRI_BASE + 28)
#define INT_SPI_4			(INT_TRI_BASE + 29)
#define INT_SPI_5			(INT_TRI_BASE + 30)
#define INT_SW_RESERVED			(INT_TRI_BASE + 31)

/* Quaternary Interrupt Controller */
#define INT_QUAD_BASE			(INT_TRI_BASE + 32)
#define INT_SNOR			(INT_QUAD_BASE + 0)
#define INT_USB3			(INT_QUAD_BASE + 1)
#define INT_PCIE_INTR			(INT_QUAD_BASE + 2)
#define INT_PCIE_MSI			(INT_QUAD_BASE + 3)
#define INT_PCIE			(INT_QUAD_BASE + 4)
#define INT_AVP_CACHE			(INT_QUAD_BASE + 5)
#define INT_TSENSOR			(INT_QUAD_BASE + 6)
#define INT_AUDIO_CLUSTER		(INT_QUAD_BASE + 7)
#define INT_APB_DMA_CH0			(INT_QUAD_BASE + 8)
#define INT_APB_DMA_CH1			(INT_QUAD_BASE + 9)
#define INT_APB_DMA_CH2			(INT_QUAD_BASE + 10)
#define INT_APB_DMA_CH3			(INT_QUAD_BASE + 11)
#define INT_APB_DMA_CH4			(INT_QUAD_BASE + 12)
#define INT_APB_DMA_CH5			(INT_QUAD_BASE + 13)
#define INT_APB_DMA_CH6			(INT_QUAD_BASE + 14)
#define INT_APB_DMA_CH7			(INT_QUAD_BASE + 15)
#define INT_APB_DMA_CH8			(INT_QUAD_BASE + 16)
#define INT_APB_DMA_CH9			(INT_QUAD_BASE + 17)
#define INT_APB_DMA_CH10		(INT_QUAD_BASE + 18)
#define INT_APB_DMA_CH11		(INT_QUAD_BASE + 19)
#define INT_APB_DMA_CH12		(INT_QUAD_BASE + 20)
#define INT_APB_DMA_CH13		(INT_QUAD_BASE + 21)
#define INT_APB_DMA_CH14		(INT_QUAD_BASE + 22)
#define INT_APB_DMA_CH15		(INT_QUAD_BASE + 23)
#define INT_I2C4			(INT_QUAD_BASE + 24)
#define INT_TMR5			(INT_QUAD_BASE + 25)
#define INT_TMR_SHARED			(INT_QUAD_BASE + 26) /* Deprecated */
#define INT_WDT_CPU			(INT_QUAD_BASE + 27)
#define INT_WDT_AVP			(INT_QUAD_BASE + 28)
#define INT_GPIO8			(INT_QUAD_BASE + 29)
#define INT_CAR				(INT_QUAD_BASE + 30)
#define INT_QUAD_RES_31			(INT_QUAD_BASE + 31)

/* Quintary Interrupt Controller */
#define INT_QUINT_BASE			(INT_QUAD_BASE + 32)
#define INT_APB_DMA_CH16		(INT_QUINT_BASE + 0)
#define INT_APB_DMA_CH17		(INT_QUINT_BASE + 1)
#define INT_APB_DMA_CH18		(INT_QUINT_BASE + 2)
#define INT_APB_DMA_CH19		(INT_QUINT_BASE + 3)
#define INT_APB_DMA_CH20		(INT_QUINT_BASE + 4)
#define INT_APB_DMA_CH21		(INT_QUINT_BASE + 5)
#define INT_APB_DMA_CH22		(INT_QUINT_BASE + 6)
#define INT_APB_DMA_CH23		(INT_QUINT_BASE + 7)
#define INT_APB_DMA_CH24		(INT_QUINT_BASE + 8)
#define INT_APB_DMA_CH25		(INT_QUINT_BASE + 9)
#define INT_APB_DMA_CH26		(INT_QUINT_BASE + 10)
#define INT_APB_DMA_CH27		(INT_QUINT_BASE + 11)
#define INT_APB_DMA_CH28		(INT_QUINT_BASE + 12)
#define INT_APB_DMA_CH29		(INT_QUINT_BASE + 13)
#define INT_APB_DMA_CH30		(INT_QUINT_BASE + 14)
#define INT_APB_DMA_CH31		(INT_QUINT_BASE + 15)
#define INT_CPU0_PMU_INTR		(INT_QUINT_BASE + 16)
#define INT_CPU1_PMU_INTR		(INT_QUINT_BASE + 17)
#define INT_CPU2_PMU_INTR		(INT_QUINT_BASE + 18)
#define INT_CPU3_PMU_INTR		(INT_QUINT_BASE + 19)
#define INT_CPU4_PMU_INTR		(INT_QUINT_BASE + 20)
#define INT_CPU5_PMU_INTR		(INT_QUINT_BASE + 21)
#define INT_CPU6_PMU_INTR		(INT_QUINT_BASE + 22)
#define INT_CPU7_PMU_INTR		(INT_QUINT_BASE + 23)
#define INT_TMR6			(INT_QUINT_BASE + 24)
#define INT_TMR7			(INT_QUINT_BASE + 25)
#define INT_TMR8			(INT_QUINT_BASE + 26)
#define INT_TMR9			(INT_QUINT_BASE + 27)
#define INT_TMR10			(INT_QUINT_BASE + 28)
#define INT_QUINT_RES_29		(INT_QUINT_BASE + 29)
#define INT_QUINT_RES_30		(INT_QUINT_BASE + 30)
#define INT_QUINT_RES_31		(INT_QUINT_BASE + 31)

#define INT_GIC_NR			(INT_QUINT_BASE + 32)

#define INT_MAIN_NR			(INT_GIC_NR - INT_PRI_BASE)

#define INT_SYNCPT_THRESH_BASE		(INT_QUINT_BASE + 32)
#define INT_SYNCPT_THRESH_NR		32

#define INT_GPIO_BASE			(INT_SYNCPT_THRESH_BASE + \
					 INT_SYNCPT_THRESH_NR)
#define INT_GPIO_NR			(32 * 8)

#define INT_PCI_MSI_BASE		(INT_GPIO_BASE + \
					 INT_GPIO_NR)
#define INT_PCI_MSI_NR			(32 * 8)

#endif

#define FIQ_START			INT_GIC_BASE

#define TEGRA_NR_IRQS			(INT_PCI_MSI_BASE + \
							INT_PCI_MSI_NR)

#define INT_BOARD_BASE			TEGRA_NR_IRQS

#define NR_BOARD_IRQS			64

#define NR_IRQS				(INT_BOARD_BASE + NR_BOARD_IRQS)

#endif

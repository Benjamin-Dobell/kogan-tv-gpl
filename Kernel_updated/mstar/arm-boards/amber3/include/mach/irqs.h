/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    PROJECT: AMBER3

	FILE NAME: include/asm-arm/arch-AMBER3/irqs.h

    DESCRIPTION:
          Head file of IO table definition

    HISTORY:
         <Date>     <Author>    <Modification Description>
        2008/07/18  Fred Cheng  Modify to define constants for power management
		2008/10/06	Fred Cheng	Modify INT_BASE_ADDR for Pioneer platform

------------------------------------------------------------------------------*/

#ifndef __ARCH_ARM_ASM_IRQS_H
#define __ARCH_ARM_ASM_IRQS_H
/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
//#include "platform.h"

/*------------------------------------------------------------------------------
    Constant
-------------------------------------------------------------------------------*/

#if 0
/******************************************************
       CA9 Private Timer
*******************************************************/

#define MSTAR_CHIP_INT_END                     256 

#define INT_PPI_FIQ                   28 //GIC PPI FIQ number
#define INT_ID_PTIMER	              29
#define INT_PPI_IRQ                   31 //GIC PPI IRQ number 

//Cortex-A9 GIC PPI 
#define IRQ_LOCALTIMER		29
#define IRQ_LOCALWDOG		30


#if defined(CONFIG_MSTAR_AMBER3)
#define IRQ_BASE                      160 

#define    INT_AMBER3_UART_0                 	 (IRQ_BASE  + 0)
#define    INT_AMBER3_PMSLEEP          		 (IRQ_BASE  + 1)  // T12
#define    INT_AMBER3_ONIF                  	 (IRQ_BASE  + 2)  // T8
#define    INT_AMBER3_MVD                   	 (IRQ_BASE  + 3)
#define    INT_AMBER3_PS                    		 (IRQ_BASE  + 4)
#define    INT_AMBER3_NFIE                  	 (IRQ_BASE  + 5)
#define    INT_AMBER3_USB                   		 (IRQ_BASE  + 6)
#define    INT_AMBER3_UHC                   	 (IRQ_BASE  + 7)
#define    INT_AMBER3_EC_BRIDGE         	 (IRQ_BASE  + 8)
#define    INT_AMBER3_EMAC                  	 (IRQ_BASE  + 9)
#define    INT_AMBER3_DISP                  	 (IRQ_BASE  + 10)
#define    INT_AMBER3_MIIC_DMA_INT3   	 (IRQ_BASE  + 12) // A5
#define    INT_AMBER3_MIIC_INT3           	 (IRQ_BASE  + 13) // A5
#define    INT_AMBER3_COMB                  	 (IRQ_BASE  + 14)

#define   INT_AMBER3_TSP2HK                	 (IRQ_BASE  + 16)
#define    INT_AMBER3_VE                    		 (IRQ_BASE  + 17)
#define    INT_AMBER3_CIMAX2MCU        	 (IRQ_BASE  + 18)
#define    INT_AMBER3_DC                   		 (IRQ_BASE  + 19)
#define    INT_AMBER3_GOP                   	 (IRQ_BASE  + 20)
#define    INT_AMBER3_PCM                   	 (IRQ_BASE  + 21)
#define    INT_AMBER3_SMART              		 (IRQ_BASE  + 23)  // A5
#define    INT_AMBER3_VP6                   		 (IRQ_BASE  + 25)  // A3
#define    INT_AMBER3_DDC2BI               	 (IRQ_BASE  + 26)
#define    INT_AMBER3_SCM                   	 (IRQ_BASE  + 27)
#define    INT_AMBER3_VBI                   		 (IRQ_BASE  + 28)
#define    INT_AMBER3_MVD2MIPS           	 (IRQ_BASE  + 29) // A5
#define    INT_AMBER3_GPD                   	 (IRQ_BASE  + 30) // A5
#define    INT_AMBER3_ADCDVI2RIU      	 (IRQ_BASE  + 31)

#define    INT_AMBER3_HVD               		 (IRQ_BASE  + 32)
#define    INT_AMBER3_USB1              		 (IRQ_BASE  + 33)
#define    INT_AMBER3_UHC1              		 (IRQ_BASE  + 34)
#define    INT_AMBER3_MIU               		 (IRQ_BASE  + 35)
#define    INT_AMBER3_USB2              		 (IRQ_BASE  + 36)
#define    INT_AMBER3_UHC2              		 (IRQ_BASE  + 37)
#define    INT_AMBER3_AEON2HI           	 (IRQ_BASE  + 38)
#define    INT_AMBER3_UART1             		 (IRQ_BASE  + 39)
#define    INT_AMBER3_UART2             		 (IRQ_BASE  + 40)
#define    INT_AMBER3_MPIF              		 (IRQ_BASE  + 42)
#define    INT_AMBER3_MIIC_DMA_INT2    	 (IRQ_BASE  + 43) // A5
#define    INT_AMBER3_MIIC_INT2         	 (IRQ_BASE  + 44) // A5
#define    INT_AMBER3_JPD               		 (IRQ_BASE  + 45)
#define    INT_AMBER3_DISPI             		 (IRQ_BASE  + 46)
#define    INT_AMBER3_MFE               		 (IRQ_BASE  + 47) // T8

#define    INT_AMBER3_BDMA0             		 (IRQ_BASE  + 48)
#define    INT_AMBER3_BDMA1             		 (IRQ_BASE  + 49)
#define    INT_AMBER3_UART2MCU          	 (IRQ_BASE  + 50)
#define    INT_AMBER3_URDMA2MCU         	 (IRQ_BASE  + 51)
#define    INT_AMBER3_DVI_HDMI_HDCP   	 (IRQ_BASE  + 52)
#define    INT_AMBER3_G3D2MCU           	 (IRQ_BASE  + 53)
#define    INT_AMBER3_CEC               		 (IRQ_BASE  + 54) // T8
#define    INT_AMBER3_HDCP_IIC          	 (IRQ_BASE  + 55) // T8
#define    INT_AMBER3_HDCP_X74          	 (IRQ_BASE  + 56) // T8
#define    INT_AMBER3_WADR_ERR          	 (IRQ_BASE  + 57) // T8
#define    INT_AMBER3_DCSUB             		 (IRQ_BASE  + 58) // T8
#define    INT_AMBER3_GE                		 (IRQ_BASE  + 59) // T8
#define    INT_AMBER3_MIIC_DMA_INT1    	 (IRQ_BASE  + 60) // A5
#define    INT_AMBER3_MIIC_INT1         	 (IRQ_BASE  + 61) // A5
#define    INT_AMBER3_MIIC_DMA_INT0    	 (IRQ_BASE  + 62) // A5
#define    INT_AMBER3_MIIC_INT0         	 (IRQ_BASE  + 63) // A5

// FIQ
#define FIQ_BASE                      128 

#define    FIQ_AMBER3_EXTIMER0              	 (FIQ_BASE  +  0)
#define    FIQ_AMBER3_EXTIMER1              	 (FIQ_BASE  +  1)
#define    FIQ_AMBER3_WDT                   	 (FIQ_BASE  +  2)
#define    FIQ_AMBER3_STRETCH               	 (FIQ_BASE  +  3)  // T12
#define    FIQ_AMBER3_AEON_MB2_MCU0   	 (FIQ_BASE  +  4)  // T8
#define    FIQ_AMBER3_AEON_MB2_MCU1   	 (FIQ_BASE  +  5)  // T8
#define    FIQ_AMBER3_DSP2_MB2_MCU0   	 (FIQ_BASE  +  6)  // T8
#define    FIQ_AMBER3_DSP2_MB2_MCU1    	 (FIQ_BASE  +  7)  // T8
#define    FIQ_AMBER3_USB                   		 (FIQ_BASE  +  8)  // T8
#define    FIQ_AMBER3_UHC                   	 (FIQ_BASE  +  9)  // T8
#define    FIQ_AMBER3_VP6                   		 (FIQ_BASE  + 10)  // A3
#define    FIQ_AMBER3_HDMI_NON_PCM      	 (FIQ_BASE  + 11)
#define    FIQ_AMBER3_SPDIF_IN_NON_PCM (FIQ_BASE  + 12)
#define    FIQ_AMBER3_EMAC                  	 (FIQ_BASE  + 13)
#define    FIQ_AMBER3_SE_DSP2UP             	 (FIQ_BASE  + 14)
#define    FIQ_AMBER3_TSP2AEON              	 (FIQ_BASE  + 15)

#define    FIQ_AMBER3_VIVALDI_STR           	 (FIQ_BASE  + 16)
#define    FIQ_AMBER3_VIVALDI_PTS           	 (FIQ_BASE  + 17)
#define    FIQ_AMBER3_DSP_MIU_PROT      	 (FIQ_BASE  + 18)
#define    FIQ_AMBER3_XIU_TIMEOUT         	 (FIQ_BASE  + 19)
#define    FIQ_AMBER3_DMDMCU2HK            	 (FIQ_BASE  + 20)  // T8
#define    FIQ_AMBER3_VSYNC_VE4VBI        	 (FIQ_BASE  + 21)
#define    FIQ_AMBER3_FIELD_VE4VBI         	 (FIQ_BASE  + 22)
#define    FIQ_AMBER3_VDMCU2HK             	 (FIQ_BASE  + 23)
#define    FIQ_AMBER3_VE_DONE_TT           	 (FIQ_BASE  + 24)
#define    FIQ_AMBER3_IR                    		 (FIQ_BASE  + 27)
#define    FIQ_AMBER3_AFEC_VSYNC           	 (FIQ_BASE  + 28)
#define    FIQ_AMBER3_DEC_DSP2UP           	 (FIQ_BASE  + 29)
#define    FIQ_AMBER3_DSP2MIPS             	 (FIQ_BASE  + 31)

#define    FIQ_AMBER3_IR_INT_RC             	 (FIQ_BASE  + 32)
#define    FIQ_AMBER3_AU_DMA_BUF_INT 	 (FIQ_BASE  + 33)
#define    FIQ_AMBER3_IR_IN                     	 (FIQ_BASE  + 34)
#define    FIQ_AMBER3_8051_TO_MIPS_VPE0	 (FIQ_BASE  + 38) // MBX (8051->MIPS)
#define    FIQ_AMBER3_GPIO0                    	 (FIQ_BASE  + 39) // T12
#define    FIQ_AMBER3_MIPS_VPE0_TO_8051   	 (FIQ_BASE  +42)// MBX (MIPS-->8051)
#define    FIQ_AMBER3_GPIO1                     	 (FIQ_BASE  + 43)
#define    FIQ_AMBER3_GPIO2                     	 (FIQ_BASE  + 47) // T12

#define    FIQ_AMBER3_USB1                      	 (FIQ_BASE  + 51)
#define    FIQ_AMBER3_UHC1                      	 (FIQ_BASE  + 52)
#define    FIQ_AMBER3_USB2                      	 (FIQ_BASE  + 53)
#define    FIQ_AMBER3_UHC2                      	 (FIQ_BASE  + 54)
#define    FIQ_AMBER3_GPIO3                     	 (FIQ_BASE  + 55) // T12
#define    FIQ_AMBER3_GPIO4                     	 (FIQ_BASE  + 56) // T12
#define    FIQ_AMBER3_GPIO5                     	 (FIQ_BASE  + 57) // T12
#define    FIQ_AMBER3_GPIO6                     	 (FIQ_BASE  + 58) // T12
#define    FIQ_AMBER3_PWM_RP_L              	 (FIQ_BASE  + 59) // T12
#define    FIQ_AMBER3_PWM_FP_L              	 (FIQ_BASE  + 60) // T12
#define    FIQ_AMBER3_PWM_RP_R              	 (FIQ_BASE  + 61) // T12
#define    FIQ_AMBER3_PWM_FP_R              	 (FIQ_BASE  + 62) // T12
#define    FIQ_AMBER3_GPIO7                    	 (FIQ_BASE  + 63) // T12

#else
/* FIQ Definition */
#define FIQ_BASE                      128 
#define INT_AMBER3_TIMER_0            (FIQ_BASE + 0)
#define INT_AMBER3_TIMER_1            (FIQ_BASE + 1)
#define INT_AMBER3_WDT                (FIQ_BASE + 2)
#define INT_AMBER3_IR                 (FIQ_BASE + 3)
#define INT_AMBER3_TOUCHPL_Y_FLAG     (FIQ_BASE + 4)
#define INT_AMBER3_DMA_DONE           (FIQ_BASE + 5)

//* IRQ Definition */
#define IRQ_BASE                      160 
#define INT_AMBER3_UART_0             (IRQ_BASE + 0)
#define INT_AMBER3_PM                 (IRQ_BASE + 1)
#define INT_AMBER3_RTC                (IRQ_BASE + 2)
#define INT_AMBER3_AUDIO_2            (IRQ_BASE + 3)
#define INT_AMBER3_AUDIO_1            (IRQ_BASE + 3)
#define INT_AMBER3_GOP                (IRQ_BASE + 4)
#define INT_AMBER3_IPM                (IRQ_BASE + 5)
#define INT_AMBER3_DISP               (IRQ_BASE + 6)
#define INT_AMBER3_JPD                (IRQ_BASE + 7)
#define INT_AMBER3_UHC                (IRQ_BASE + 8)
#define INT_AMBER3_OTG                (IRQ_BASE + 9)
#define INT_AMBER3_USB                (IRQ_BASE + 10)
#define INT_AMBER3_UTMI               (IRQ_BASE + 11)
#define INT_AMBER3_MPIF               (IRQ_BASE + 12)
#define INT_AMBER3_NFIE               (IRQ_BASE + 13)
#define INT_AMBER3_FCIE               (IRQ_BASE + 14)
#define INT_AMBER3_IIC_0              (IRQ_BASE + 15)
#define INT_AMBER3_SPI_ARB_DUP_REQ    (IRQ_BASE + 16)
#define INT_AMBER3_SPI_ARB_CHG_ADDR   (IRQ_BASE + 17)
#define INT_AMBER3_DR                 (IRQ_BASE + 18)
#define INT_AMBER3_CCFL               (IRQ_BASE + 19)
#define INT_AMBER3_UART_2             (IRQ_BASE + 20)
#define INT_AMBER3_UART_1             (IRQ_BASE + 21)
#define INT_AMBER3_KEYPAD             (IRQ_BASE + 22)
#define INT_AMBER3_EXT_IN             (IRQ_BASE + 23)


#endif

#endif
/* Max number of Interrupts */
#define NR_IRQS		256//


#if 0
#define INT_BASE_ADDR                  AMBER3_BASE_REG_INTR_PA

#define FIQ_MASK_L                  (INT_BASE_ADDR + 0x04<<2)
#define FIQ_MASK_H                  (INT_BASE_ADDR + 0x05<<2)
#define FIQ_FORCE_L                 (INT_BASE_ADDR + 0x00<<2)
#define FIQ_FORCE_H                 (INT_BASE_ADDR + 0x01<<2)
#define FIQ_CLEAR_L                 (INT_BASE_ADDR + 0x0c<<2)
#define FIQ_CLEAR_H                 (INT_BASE_ADDR + 0x0d<<2)
#define FIQ_RAW_STATUS_L            (INT_BASE_ADDR + 0x18)
#define FIQ_RAW_STATUS_H            (INT_BASE_ADDR + 0x1C)
#define FIQ_FINAL_STATUS_L          (INT_BASE_ADDR + 0x0c<<2)
#define FIQ_FINAL_STATUS_H          (INT_BASE_ADDR + 0x0d<<2)
#define FIQ_SEL_HL_TRIGGER_L        (INT_BASE_ADDR + 0x28)
#define FIQ_SEL_HL_TRIGGER_H        (INT_BASE_ADDR + 0x2C)

///* Physcial address of IRQ */
#define IRQ_MASK_L                  (INT_BASE_ADDR + 0x14<<2)
#define IRQ_MASK_H                  (INT_BASE_ADDR + 0x15<<2
#define IRQ_FORCE_L                 (INT_BASE_ADDR + 0x10<<2)
#define IRQ_FORCE_H                 (INT_BASE_ADDR + 0x11<<2
#define IRQ_SEL_HL_TRIGGER_L        (INT_BASE_ADDR + 0x50)
#define IRQ_SEL_HL_TRIGGER_H        (INT_BASE_ADDR + 0x54)
#define IRQ_FIQ2IRQ_RAW_STATUS_L    (INT_BASE_ADDR + 0x60)
#define IRQ_FIQ2IRQ_RAW_STATUS_H    (INT_BASE_ADDR + 0x64)
#define IRQ_RAW_STATUS_L            (INT_BASE_ADDR + 0x68)
#define IRQ_RAW_STATUS_H            (INT_BASE_ADDR + 0x6C)
#define IRQ_FIQ2IRQ_FINAL_STATUS_L  (INT_BASE_ADDR + 0x70)
#define IRQ_FIQ2IRQ_FINAL_STATUS_H  (INT_BASE_ADDR + 0x74)
#define IRQ_FINAL_STATUS_L          (INT_BASE_ADDR + 0x1c<<2)
#define IRQ_FINAL_STATUS_H          (INT_BASE_ADDR + 0x1d<<2)

//* Physcial address of Common part */
#define FIQ2IRQOUT_L                (INT_BASE_ADDR + 0x80)
#define FIQ2IRQOUT_H                (INT_BASE_ADDR + 0x84)
#define FIQ_IDX                     (INT_BASE_ADDR + 0x88)
#define IRQ_IDX                     (INT_BASE_ADDR + 0x8C)
#endif

#endif // __ARCH_ARM_ASM_IRQS_H


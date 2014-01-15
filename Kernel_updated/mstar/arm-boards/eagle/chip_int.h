#ifndef __CHIP_INT_H__
#define __CHIP_INT_H__

#include <mach/platform.h>

/******************************************************
       CA9 Private Timer
*******************************************************/
#define INT_PPI_FIQ                 28 //GIC PPI FIQ number
#define INT_ID_PTIMER	            29
#define INT_PPI_IRQ                 31 //GIC PPI IRQ number

//Cortex-A9 GIC PPI
#define IRQ_LOCALTIMER		        29
#define IRQ_LOCALWDOG		        30

//Cortex-A9 PMU(Performance Monitor Unit)
#define IRQ_PMU0                    59
#define IRQ_PMU1                    60


#define MSTAR_CHIP_INT_END          256


#define REG_INT_BASE_PA		        0x1F203200
#define REG_INT_BASE                0xFD203200

#define NR_IRQS                     256

//IRQ registers
#define REG_IRQ_MASK_L              (REG_INT_BASE + (0x0054 << 2))
#define REG_IRQ_MASK_H              (REG_INT_BASE + (0x0055 << 2))
#define REG_IRQ_PENDING_L           (REG_INT_BASE + (0x005c << 2))
#define REG_IRQ_PENDING_H           (REG_INT_BASE + (0x005d << 2))

//IRQ EXP registers
#define REG_IRQ_EXP_MASK_L          (REG_INT_BASE + (0x0056 << 2))
#define REG_IRQ_EXP_MASK_H          (REG_INT_BASE + (0x0057 << 2))
#define REG_IRQ_EXP_PENDING_L       (REG_INT_BASE + (0x005e << 2))
#define REG_IRQ_EXP_PENDING_H       (REG_INT_BASE + (0x005f << 2))

//FIQ registers
#define REG_FIQ_MASK_L              (REG_INT_BASE + (0x004<< 2))
#define REG_FIQ_MASK_H              (REG_INT_BASE + (0x005<< 2))
#define REG_FIQ_CLEAR_L             (REG_INT_BASE + (0x00c<< 2))
#define REG_FIQ_CLEAR_H             (REG_INT_BASE + (0x00d<< 2))
#define REG_FIQ_PENDING_L           (REG_INT_BASE + (0x00c<< 2))
#define REG_FIQ_PENDING_H           (REG_INT_BASE + (0x00d<< 2))

//FIQ EXP registers
#define REG_FIQ_EXP_MASK_L          (REG_INT_BASE + (0x006<< 2))
#define REG_FIQ_EXP_MASK_H          (REG_INT_BASE + (0x007<< 2))
#define REG_FIQ_EXP_CLEAR_L         (REG_INT_BASE + (0x00e<< 2))
#define REG_FIQ_EXP_CLEAR_H         (REG_INT_BASE + (0x00f<< 2))
#define REG_FIQ_EXP_PENDING_L       (REG_INT_BASE + (0x00e<< 2))
#define REG_FIQ_EXP_PENDING_H       (REG_INT_BASE + (0x00f<< 2))

#define MSTAR_INT_BASE  			128

/*******************************************************/
/*   THE IRQ AND FIQ ARE NOT COMPLETED.                */
/*   FOR EACH IP OWNER, PLEASE REVIEW IT BY YOURSELF   */
/*******************************************************/
typedef enum
{
    // IRQ
    E_IRQL_START                    = 64 + MSTAR_INT_BASE,
    E_IRQ_UART0                     = E_IRQL_START + 0,
    E_IRQ_PM_SLEEP                  = E_IRQL_START + 1,
    E_IRQ_USB2                      = E_IRQL_START + 2,
    E_IRQ_MVD                       = E_IRQL_START + 3,
    E_IRQ_PS                        = E_IRQL_START + 4,
    E_IRQ_NFIE                      = E_IRQL_START + 5,
    E_IRQ_USB                       = E_IRQL_START + 6,
    E_IRQ_UHC                       = E_IRQL_START + 7,
//  E_IRQ_RESERVED                  = E_IRQL_START + 8,
    E_IRQ_EMAC                      = E_IRQL_START + 9,
    E_IRQ_DISP                      = E_IRQL_START + 10,
    E_IRQ_G3D2MCU                   = E_IRQL_START + 11,
    E_IRQ_MIIC2                     = E_IRQL_START + 12,
    E_IRQ_MIIC1                     = E_IRQL_START + 13,
    E_IRQ_VD                        = E_IRQL_START + 14,
    E_IRQ_EXT_GPIO0                 = E_IRQL_START + 15,
    E_IRQL_END                      = 15 + E_IRQL_START ,

    E_IRQH_START                    = 16 + E_IRQL_START ,
    E_IRQ_TSP2HK                    = E_IRQH_START + 0,
    E_IRQ_VE                        = E_IRQH_START + 1,
    E_IRQ_CIMAX2MCU                 = E_IRQH_START + 2,
    E_IRQ_DC                        = E_IRQH_START + 3,
    E_IRQ_GOP                       = E_IRQH_START + 4,
    E_IRQ_PCM                       = E_IRQH_START + 5,
    E_IRQ_MIIC0                     = E_IRQH_START + 6,
//  E_IRQ_RESERVED                  = E_IRQH_START + 7,
    E_IRQ_GPD                       = E_IRQH_START + 8,
    E_IRQ_MFE                       = E_IRQH_START + 9,
    E_IRQ_DDC2BI                    = E_IRQH_START + 10,
    E_IRQ_SMART                     = E_IRQH_START + 11,
//  E_IRQ_RESERVED                  = E_IRQH_START + 12,
    E_IRQ_MVD2ARM                   = E_IRQH_START + 13,
//  E_IRQ_RESERVED                  = E_IRQH_START + 14,
    E_IRQ_ADCDVI2RIU                = E_IRQH_START + 15,
    E_IRQH_END                      = 15 + E_IRQH_START,

    // FIQ
    E_FIQL_START                    = MSTAR_INT_BASE,
    E_FIQ_EXTIMER0                  = E_FIQL_START + 0,
    E_FIQ_EXTIMER1                  = E_FIQL_START + 1,
    E_FIQ_WDT                       = E_FIQL_START + 2,
    E_FIQ_DISP_TGEN0                = E_FIQL_START + 3,
    E_FIQ_MB_AUR2TOMCU0             = E_FIQL_START + 4,
    E_FIQ_MB_AUR2TOMCU1             = E_FIQL_START + 5,
    E_FIQ_MB_DSP2TOMCU0             = E_FIQL_START + 6,
    E_FIQ_MB_DSP2TOMCU1             = E_FIQL_START + 7,
    E_FIQ_USB                       = E_FIQL_START + 8,
    E_FIQ_UHC                       = E_FIQL_START + 9,
    E_FIQ_DISP_TGEN1                = E_FIQL_START + 10,
    E_FIQ_HDMI_NON_PCM              = E_FIQL_START + 11,
    E_FIQ_SPDIF_IN_NON_PCM          = E_FIQL_START + 12,
    E_FIQ_EMAC                      = E_FIQL_START + 13,
    E_FIQ_SE_DSP2UP                 = E_FIQL_START + 14,
    E_FIQ_TSP2AEON                  = E_FIQL_START + 15,
    E_FIQL_END                      = 15 + E_FIQL_START,

    E_FIQH_START                    = 16 + E_FIQL_START,
    E_FIQ_VIVALDI_STR               = E_FIQH_START + 0,
    E_FIQ_VIVALDI_PTS               = E_FIQH_START + 1,
    E_FIQ_DSP_MIU_PROT              = E_FIQH_START + 2,
    E_FIQ_XIU_TIMEOUT               = E_FIQH_START + 3,
    E_FIQ_DMDMCU2HK_INT             = E_FIQH_START + 4,
    E_FIQ_VSYNC_VE4VBI              = E_FIQH_START + 5,
    E_FIQ_FIELD_VE4VBI              = E_FIQH_START + 6,
    E_FIQ_VDMCU2HK                  = E_FIQH_START + 7,
    E_FIQ_VE_DONE_TT                = E_FIQH_START + 8,
    E_FIQ_UHC2                      = E_FIQH_START + 9,
//  E_FIQ_RESERVED                  = E_FIQH_START + 10,
    E_FIQ_IR                        = E_FIQH_START + 11,
    E_FIQ_AFEC_VSYNC                = E_FIQH_START + 12,
    E_FIQ_DEC_DSP2UP                = E_FIQH_START + 13,
    E_FIQ_USB2                      = E_FIQH_START + 14,
    E_FIQ_DSP2ARM                   = E_FIQH_START + 15,
    E_FIQH_END                      = 15 + E_FIQH_START,

    //IRQEXP
    E_IRQEXPL_START                 = 16 + E_IRQH_START,
    E_IRQEXPL_HVD                   = E_IRQEXPL_START + 0,
    E_IRQEXPL_USB1                  = E_IRQEXPL_START + 1,
    E_IRQEXPL_UHC1                  = E_IRQEXPL_START + 2,
    E_IRQEXPL_MIU                   = E_IRQEXPL_START + 3,
    E_IRQEXPL_EXT_GPIO1             = E_IRQEXPL_START + 4,
    E_IRQEXPL_EXT_GPIO2             = E_IRQEXPL_START + 5,
    E_IRQEXPL_AEON2HI               = E_IRQEXPL_START + 6,
    E_IRQEXPL_UART1                 = E_IRQEXPL_START + 7,
//  E_IRQEXPL_RESERVED              = E_IRQEXPL_START + 8,
    E_IRQEXPL_EXT_GPIO3             = E_IRQEXPL_START + 9,
    E_IRQEXPL_MPIF                  = E_IRQEXPL_START + 10,
    E_IRQEXPL_EXT_GPIO4             = E_IRQEXPL_START + 11,
    E_IRQEXPL_EXT_GPIO5             = E_IRQEXPL_START + 12,
    E_IRQEXPL_JPD                   = E_IRQEXPL_START + 13,
    E_IRQEXPL_MSPI1                 = E_IRQEXPL_START + 14,
    E_IRQEXPL_MSPI0                 = E_IRQEXPL_START + 15,
    E_IRQEXPL_END                   = 15 + E_IRQEXPL_START,

    E_IRQEXPH_START                 = 16 + E_IRQEXPL_START ,
    E_IRQEXPH_BDMA0                 = E_IRQEXPH_START + 0,
    E_IRQEXPH_BDMA1                 = E_IRQEXPH_START + 1,
    E_IRQEXPH_UART2MCU              = E_IRQEXPH_START + 2,
    E_IRQEXPH_URDMA2MCU             = E_IRQEXPH_START + 3,
    E_IRQEXPH_DVI_HDMI_HDCP         = E_IRQEXPH_START + 4,
    E_IRQEXPH_EXT_GPIO6             = E_IRQEXPH_START + 5,
    E_IRQEXPH_CEC                   = E_IRQEXPH_START + 6,
    E_IRQEXPH_HDCP_IIC              = E_IRQEXPH_START + 7,
    E_IRQEXPH_HDCP_X74              = E_IRQEXPH_START + 8,
    E_IRQEXPH_WADR_ERR_INT          = E_IRQEXPH_START + 9,
    E_IRQEXPH_UHC2                  = E_IRQEXPH_START + 10,
    E_IRQEXPH_GE                    = E_IRQEXPH_START + 11,
    E_IRQEXPH_MIIC_DMA2             = E_IRQEXPH_START + 12,
    E_IRQEXPH_MIIC_DMA1             = E_IRQEXPH_START + 13,
    E_IRQEXPH_MIIC_DMA0             = E_IRQEXPH_START + 14,
    E_IRQEXPH_EXT_GPIO7             = E_IRQEXPH_START + 15,
    E_IRQEXPH_END                   = 15 + E_IRQEXPH_START ,

    // FIQEXP
    E_FIQEXPL_START                 = 16 + E_FIQH_START,
    E_FIQEXPL_IR_INT_RC             = E_FIQEXPL_START + 0,
    E_FIQEXPL_AU_DMA_BUF_INT        = E_FIQEXPL_START + 1,
    E_FIQEXPL_IR_IN                 = E_FIQEXPL_START + 2,
    E_FIQEXPL_DISP_TGEN2            = E_FIQEXPL_START + 3,
//  E_FIQEXPL_RESERVED              = E_FIQEXPL_START + 4,
    E_FIQEXPL_8051_TO_ARM           = E_FIQEXPL_START + 5,
    E_FIQEXPL_8051_TO_SECURE_R2     = E_FIQEXPL_START + 6,
    E_FIQEXPL_EXT_GPIO0             = E_FIQEXPL_START + 7,
//  E_FIQEXPL_RESERVED              = E_FIQEXPL_START + 8,
    E_FIQEXPL_SECURE_R2_TO_ARM      = E_FIQEXPL_START + 9,
    E_FIQEXPL_SECURE_R2_TO_8051     = E_FIQEXPL_START + 10,
    E_FIQEXPL_EXT_GPIO1             = E_FIQEXPL_START + 11,
//  E_FIQEXPL_RESERVED              = E_FIQEXPL_START + 12,
    E_FIQEXPL_ARM_TO_SECURE_R2      = E_FIQEXPL_START + 13,
    E_FIQEXPL_ARM_TO_8051           = E_FIQEXPL_START + 14,
    E_FIQEXPL_EXT_GPIO2             = E_FIQEXPL_START + 15,
    E_FIQEXPL_END                   = 15 + E_FIQEXPL_START ,

    E_FIQEXPH_START                 = 16 + E_FIQEXPL_START,
    E_FIQEXPH_DISP_TGEN3            = E_FIQEXPH_START + 0,
    E_FIQEXPH_EXT_GPIO8             = E_FIQEXPH_START + 1,
    E_FIQEXPH_EXT_GPIO9             = E_FIQEXPH_START + 2,
    E_FIQEXPH_USB1                  = E_FIQEXPH_START + 3,
    E_FIQEXPH_UHC1                  = E_FIQEXPH_START + 4,
    E_FIQEXPH_LDM_DMA_DONE0         = E_FIQEXPH_START + 5,
    E_FIQEXPH_LDM_DMA_DONE1         = E_FIQEXPH_START + 6,
    E_FIQEXPH_EXT_GPIO3             = E_FIQEXPH_START + 7,
    E_FIQEXPH_EXT_GPIO4             = E_FIQEXPH_START + 8,
    E_FIQEXPH_EXT_GPIO5             = E_FIQEXPH_START + 9,
    E_FIQEXPH_EXT_GPIO6             = E_FIQEXPH_START + 10,
    E_FIQEXPH_PWM_RP_I              = E_FIQEXPH_START + 11,
    E_FIQEXPH_PWM_FP_I              = E_FIQEXPH_START + 12,
    E_FIQEXPH_PWM_RP_R              = E_FIQEXPH_START + 13,
    E_FIQEXPH_PWM_FP_R              = E_FIQEXPH_START + 14,
    E_FIQEXPH_EXT_GPIO7             = E_FIQEXPH_START + 15,
    E_FIQEXPH_END                   = 15 + E_FIQEXPL_START ,

    E_IRQ_FIQ_ALL                   = 0xFF //all IRQs & FIQs
} InterruptNum;

// REG_FIQ_MASK_L
//FIQ Low 16 bits
#define FIQL_MASK                   0xFFFF
#define FIQ_EXTIMER0                (0x01 << (E_FIQ_EXTIMER0            - E_FIQL_START) )
#define FIQ_EXTIMER1                (0x01 << (E_FIQ_EXTIMER1            - E_FIQL_START) )
#define FIQ_WDT                     (0x01 << (E_FIQ_WDT                 - E_FIQL_START) )
#define FIQ_DISP_TGEN0              (0x01 << (E_FIQ_DISP_TGEN0          - E_FIQL_START) )
#define FIQ_MB_AUR2TOMCU0           (0x01 << (E_FIQ_MB_AUR2TOMCU0       - E_FIQL_START) )
#define FIQ_MB_AUR2TOMCU1           (0x01 << (E_FIQ_MB_AUR2TOMCU1       - E_FIQL_START) )
#define FIQ_MB_DSP2TOMCU0           (0x01 << (E_FIQ_MB_DSP2TOMCU0       - E_FIQL_START) )
#define FIQ_MB_DSP2TOMCU1           (0x01 << (E_FIQ_MB_DSP2TOMCU1       - E_FIQL_START) )
#define FIQ_USB                     (0x01 << (E_FIQ_USB                 - E_FIQL_START) )
#define FIQ_UHC                     (0x01 << (E_FIQ_UHC                 - E_FIQL_START) )
#define FIQ_DISP_TGEN1              (0x01 << (E_FIQ_DISP_TGEN1          - E_FIQL_START) )
#define FIQ_HDMI_NON_PCM            (0x01 << (E_FIQ_HDMI_NON_PCM        - E_FIQL_START) )
#define FIQ_SPDIF_IN_NON_PCM        (0x01 << (E_FIQ_SPDIF_IN_NON_PCM    - E_FIQL_START) )
#define FIQ_EMAC                    (0x01 << (E_FIQ_EMAC                - E_FIQL_START) )
#define FIQ_SE_DSP2UP               (0x01 << (E_FIQ_SE_DSP2UP           - E_FIQL_START) )
#define FIQ_TSP2AEON                (0x01 << (E_FIQ_TSP2AEON            - E_FIQL_START) )

// REG_FIQ_MASK_H
//FIQ High 16 bits
#define FIQH_MASK                   0xFFFF
#define FIQ_VIVALDI_STR             (0x01 << (E_FIQ_VIVALDI_STR         - E_FIQH_START) )
#define FIQ_VIVALDI_PTS             (0x01 << (E_FIQ_VIVALDI_PTS         - E_FIQH_START) )
#define FIQ_DSP_MIU_PROT            (0x01 << (E_FIQ_DSP_MIU_PROT        - E_FIQH_START) )
#define FIQ_XIU_TIMEOUT             (0x01 << (E_FIQ_XIU_TIMEOUT         - E_FIQH_START) )
#define FIQ_DMDMCU2HK_INT           (0x01 << (E_FIQ_DMDMCU2HK_INT       - E_FIQH_START) )
#define FIQ_VSYNC_VE4VBI            (0x01 << (E_FIQ_VSYNC_VE4VBI        - E_FIQH_START) )
#define FIQ_FIELD_VE4VBI            (0x01 << (E_FIQ_FIELD_VE4VBI        - E_FIQH_START) )
#define FIQ_VDMCU2HK                (0x01 << (E_FIQ_VDMCU2HK            - E_FIQH_START) )
#define FIQ_VE_DONE_TT              (0x01 << (E_FIQ_VE_DONE_TT          - E_FIQH_START) )
#define FIQ_UHC2                    (0x01 << (E_FIQ_UHC2                - E_FIQH_START) )
#define FIQ_IR                      (0x01 << (E_FIQ_IR                  - E_FIQH_START) )
#define FIQ_AFEC_VSYNC              (0x01 << (E_FIQ_AFEC_VSYNC          - E_FIQH_START) )
#define FIQ_DEC_DSP2UP              (0x01 << (E_FIQ_DEC_DSP2UP          - E_FIQH_START) )
#define FIQ_USB2                    (0x01 << (E_FIQ_USB2                - E_FIQH_START) )
#define FIQ_DSP2ARM                 (0x01 << (E_FIQ_DSP2ARM             - E_FIQH_START) )

// #define REG_IRQ_PENDING_L
#define IRQL_MASK                   0xFFFF
#define IRQ_UART0                   (0x01 << (E_IRQ_UART0               - E_IRQL_START) )
#define IRQ_PM_SLEEP                (0x01 << (E_IRQ_PM_SLEEP            - E_IRQL_START) )
#define IRQ_USB2                    (0x01 << (E_IRQ_USB2                - E_IRQL_START) )
#define IRQ_MVD                     (0x01 << (E_IRQ_MVD                 - E_IRQL_START) )
#define IRQ_PS                      (0x01 << (E_IRQ_PS                  - E_IRQL_START) )
#define IRQ_NFIE                    (0x01 << (E_IRQ_NFIE                - E_IRQL_START) )
#define IRQ_USB                     (0x01 << (E_IRQ_USB                 - E_IRQL_START) )
#define IRQ_UHC                     (0x01 << (E_IRQ_UHC                 - E_IRQL_START) )
#define IRQ_EMAC                    (0x01 << (E_IRQ_EMAC                - E_IRQL_START) )
#define IRQ_DISP                    (0x01 << (E_IRQ_DISP                - E_IRQL_START) )
#define IRQ_G3D2MCU                 (0x01 << (E_IRQ_G3D2MCU             - E_IRQL_START) )
#define IRQ_MIIC2                   (0x01 << (E_IRQ_MIIC2               - E_IRQL_START) )
#define IRQ_MIIC1                   (0x01 << (E_IRQ_MIIC1               - E_IRQL_START) )
#define IRQ_VD                      (0x01 << (E_IRQ_VD                  - E_IRQL_START) )
#define IRQ_EXT_GPIO0               (0x01 << (E_IRQ_EXT_GPIO0           - E_IRQL_START) )

// #define REG_IRQ_PENDING_H
#define IRQH_MASK                   0xFFFF
#define IRQ_TSP2HK                  (0x01 << (E_IRQ_TSP2HK              - E_IRQH_START) )
#define IRQ_VE                      (0x01 << (E_IRQ_VE                  - E_IRQH_START) )
#define IRQ_CIMAX2MCU               (0x01 << (E_IRQ_CIMAX2MCU           - E_IRQH_START) )
#define IRQ_DC                      (0x01 << (E_IRQ_DC                  - E_IRQH_START) )
#define IRQ_GOP                     (0x01 << (E_IRQ_GOP                 - E_IRQH_START) )
#define IRQ_PCM                     (0x01 << (E_IRQ_PCM                 - E_IRQH_START) )
#define IRQ_MIIC0                   (0x01 << (E_IRQ_MIIC0               - E_IRQH_START) )
#define IRQ_GPD                     (0x01 << (E_IRQ_GPD                 - E_IRQH_START) )
#define IRQ_MFE                     (0x01 << (E_IRQ_MFE                 - E_IRQH_START) )
#define IRQ_DDC2BI                  (0x01 << (E_IRQ_DDC2BI              - E_IRQH_START) )
#define IRQ_SMART                   (0x01 << (E_IRQ_SMART               - E_IRQH_START) )
#define IRQ_MVD2ARM                 (0x01 << (E_IRQ_MVD2ARM             - E_IRQH_START) )
#define IRQ_ADCDVI2RIU              (0x01 << (E_IRQ_ADCDVI2RIU          - E_IRQH_START) )

// #define REG_IRQEXP_PENDING_L
#define IRQEXPL_MASK                0xFFFF
#define IRQEXPL_HVD                 (0x01 << (E_IRQEXPL_HVD             - E_IRQEXPL_START) )
#define IRQEXPL_USB1                (0x01 << (E_IRQEXPL_USB1            - E_IRQEXPL_START) )
#define IRQEXPL_UHC1                (0x01 << (E_IRQEXPL_UHC1            - E_IRQEXPL_START) )
#define IRQEXPL_MIU                 (0x01 << (E_IRQEXPL_MIU             - E_IRQEXPL_START) )
#define IRQEXPL_EXT_GPIO1           (0x01 << (E_IRQEXPL_EXT_GPIO1       - E_IRQEXPL_START) )
#define IRQEXPL_EXT_GPIO2           (0x01 << (E_IRQEXPL_EXT_GPIO2       - E_IRQEXPL_START) )
#define IRQEXPL_AEON2HI             (0x01 << (E_IRQEXPL_AEON2HI         - E_IRQEXPL_START) )
#define IRQEXPL_UART1               (0x01 << (E_IRQEXPL_UART1           - E_IRQEXPL_START) )
#define IRQEXPL_EXT_GPIO3           (0x01 << (E_IRQEXPL_EXT_GPIO3       - E_IRQEXPL_START) )
#define IRQEXPL_MPIF                (0x01 << (E_IRQEXPL_MPIF            - E_IRQEXPL_START) )
#define IRQEXPL_EXT_GPIO4           (0x01 << (E_IRQEXPL_EXT_GPIO4       - E_IRQEXPL_START) )
#define IRQEXPL_EXT_GPIO5           (0x01 << (E_IRQEXPL_EXT_GPIO5       - E_IRQEXPL_START) )
#define IRQEXPL_JPD                 (0x01 << (E_IRQEXPL_JPD             - E_IRQEXPL_START) )
#define IRQEXPL_MSPI1               (0x01 << (E_IRQEXPL_MSPI1           - E_IRQEXPL_START) )
#define IRQEXPL_MSPI0               (0x01 << (E_IRQEXPL_MSPI0           - E_IRQEXPL_START) )

// #define REG_IRQEXP_PENDING_H
#define IRQEXPH_MASK                0xFFFF
#define IRQEXPH_BDMA0               (0x01 << (E_IRQEXPH_BDMA0           - E_IRQEXPH_START) )
#define IRQEXPH_BDMA1               (0x01 << (E_IRQEXPH_BDMA1           - E_IRQEXPH_START) )
#define IRQEXPH_UART2MCU            (0x01 << (E_IRQEXPH_UART2MCU        - E_IRQEXPH_START) )
#define IRQEXPH_URDMA2MCU           (0x01 << (E_IRQEXPH_URDMA2MCU       - E_IRQEXPH_START) )
#define IRQEXPH_DVI_HDMI_HDCP       (0x01 << (E_IRQEXPH_DVI_HDMI_HDCP   - E_IRQEXPH_START) )
#define IRQEXPH_EXT_GPIO6           (0x01 << (E_IRQEXPH_EXT_GPIO6       - E_IRQEXPH_START) )
#define IRQEXPH_CEC                 (0x01 << (E_IRQEXPH_CEC             - E_IRQEXPH_START) )
#define IRQEXPH_HDCP_IIC            (0x01 << (E_IRQEXPH_HDCP_IIC        - E_IRQEXPH_START) )
#define IRQEXPH_HDCP_X74            (0x01 << (E_IRQEXPH_HDCP_X74        - E_IRQEXPH_START) )
#define IRQEXPH_WADR_ERR_INT        (0x01 << (E_IRQEXPH_WADR_ERR_INT    - E_IRQEXPH_START) )
#define IRQEXPH_UHC2                (0x01 << (E_IRQEXPH_UHC2            - E_IRQEXPH_START) )
#define IRQEXPH_GE                  (0x01 << (E_IRQEXPH_GE              - E_IRQEXPH_START) )
#define IRQEXPH_MIIC_DMA2           (0x01 << (E_IRQEXPH_MIIC_DMA2       - E_IRQEXPH_START) )
#define IRQEXPH_MIIC_DMA1           (0x01 << (E_IRQEXPH_MIIC_DMA1       - E_IRQEXPH_START) )
#define IRQEXPH_MIIC_DMA0           (0x01 << (E_IRQEXPH_MIIC_DMA0       - E_IRQEXPH_START) )
#define IRQEXPH_EXT_GPIO7           (0x01 << (E_IRQEXPH_EXT_GPIO7       - E_IRQEXPH_START) )

// #define REG_FIQEXP_PENDING_L
#define FIQEXPL_MASK                0xFFFF
#define FIQEXPL_IR_INT_RC           (0x01 << (E_FIQEXPL_IR_INT_RC       - E_FIQEXPL_START) )
#define FIQEXPL_AU_DMA_BUF_INT      (0x01 << (E_FIQEXPL_AU_DMA_BUF_INT  - E_FIQEXPL_START) )
#define FIQEXPL_IR_IN               (0x01 << (E_FIQEXPL_IR_IN           - E_FIQEXPL_START) )
#define FIQEXPL_DISP_TGEN2          (0x01 << (E_FIQEXPL_DISP_TGEN2      - E_FIQEXPL_START) )
#define FIQEXPL_8051_TO_ARM         (0x01 << (E_FIQEXPL_8051_TO_ARM     - E_FIQEXPL_START) )
#define FIQEXPL_8051_TO_SECURE_R2   (0x01 << (E_FIQEXPL_8051_TO_SECURE_R2 - E_FIQEXPL_START) )
#define FIQEXPL_EXT_GPIO0           (0x01 << (E_FIQEXPL_EXT_GPIO0       - E_FIQEXPL_START) )
#define FIQEXPL_SECURE_R2_TO_ARM    (0x01 << (E_FIQEXPL_SECURE_R2_TO_ARM - E_FIQEXPL_START) )
#define FIQEXPL_SECURE_R2_TO_8051   (0x01 << (E_FIQEXPL_SECURE_R2_TO_8051 - E_FIQEXPL_START) )
#define FIQEXPL_EXT_GPIO1           (0x01 << (E_FIQEXPL_EXT_GPIO1       - E_FIQEXPL_START) )
#define FIQEXPL_ARM_TO_SECURE_R2    (0x01 << (E_FIQEXPL_ARM_TO_SECURE_R2 - E_FIQEXPL_START) )
#define FIQEXPL_ARM_TO_8051         (0x01 << (E_FIQEXPL_ARM_TO_8051     - E_FIQEXPL_START) )
#define FIQEXPL_EXT_GPIO2           (0x01 << (E_FIQEXPL_EXT_GPIO2       - E_FIQEXPL_START) )

// #define REG_FIQEXP_PENDING_H
#define FIQEXPH_MASK                0xFFFF
#define FIQEXPH_DISP_TGEN3          (0x01 << (E_FIQEXPH_DISP_TGEN3      - E_FIQEXPH_START) )
#define FIQEXPH_EXT_GPIO8           (0x01 << (E_FIQEXPH_EXT_GPIO8       - E_FIQEXPH_START) )
#define FIQEXPH_EXT_GPIO9           (0x01 << (E_FIQEXPH_EXT_GPIO9       - E_FIQEXPH_START) )
#define FIQEXPH_USB1                (0x01 << (E_FIQEXPH_USB1            - E_FIQEXPH_START) )
#define FIQEXPH_UHC1                (0x01 << (E_FIQEXPH_UHC1            - E_FIQEXPH_START) )
#define FIQEXPH_LDM_DMA_DONE0       (0x01 << (E_FIQEXPH_LDM_DMA_DONE0   - E_FIQEXPH_START) )
#define FIQEXPH_LDM_DMA_DONE1       (0x01 << (E_FIQEXPH_LDM_DMA_DONE1   - E_FIQEXPH_START) )
#define FIQEXPH_EXT_GPIO3           (0x01 << (E_FIQEXPH_EXT_GPIO3       - E_FIQEXPH_START) )
#define FIQEXPH_EXT_GPIO4           (0x01 << (E_FIQEXPH_EXT_GPIO4       - E_FIQEXPH_START) )
#define FIQEXPH_EXT_GPIO5           (0x01 << (E_FIQEXPH_EXT_GPIO5       - E_FIQEXPH_START) )
#define FIQEXPH_EXT_GPIO6           (0x01 << (E_FIQEXPH_EXT_GPIO6       - E_FIQEXPH_START) )
#define FIQEXPH_PWM_RP_I            (0x01 << (E_FIQEXPH_PWM_RP_I        - E_FIQEXPH_START) )
#define FIQEXPH_PWM_FP_I            (0x01 << (E_FIQEXPH_PWM_FP_I        - E_FIQEXPH_START) )
#define FIQEXPH_PWM_RP_R            (0x01 << (E_FIQEXPH_PWM_RP_R        - E_FIQEXPH_START) )
#define FIQEXPH_PWM_FP_R            (0x01 << (E_FIQEXPH_PWM_FP_R        - E_FIQEXPH_START) )
#define FIQEXPH_EXT_GPIO7           (0x01 << (E_FIQEXPH_EXT_GPIO7       - E_FIQEXPH_START) )

#define IRQL_EXP_ALL                0xFFFF
#define IRQH_EXP_ALL                0xFFFF
#define FIQL_EXP_ALL                0xFFFF
#define FIQH_EXP_ALL                0xFFFF

#endif // #ifndef __CHIP_INT_H__

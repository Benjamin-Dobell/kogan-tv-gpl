#ifndef __CHIP_INT_H__
#define __CHIP_INT_H__

#define EUCLIDINT_END 128//64

//IRQ registers
#define REG_IRQ_MASK_L              0xBF200AB0
#define REG_IRQ_MASK_H              0xBF200AB4
#define REG_IRQ_FORCE_L             0xBF200AB8
#define REG_IRQ_FORCE_H             0xBF200ABC
#define REG_IRQ_RAW_L               0xBF200AC8
#define REG_IRQ_RAW_H               0xBF200ACC
#define REG_IRQ_PENDING_L           0xBF200AD0
#define REG_IRQ_PENDING_H           0xBF200AD4

//IRQ EXP registers
#define REG_IRQ_EXP_MASK_L              0xBF203330
#define REG_IRQ_EXP_MASK_H              0xBF203334
#define REG_IRQ_EXP_FORCE_L             0xBF203338
#define REG_IRQ_EXP_FORCE_H             0xBF20333C
#define REG_IRQ_EXP_RAW_L               0xBF203348
#define REG_IRQ_EXP_RAW_H               0xBF20334C
#define REG_IRQ_EXP_PENDING_L           0xBF203350
#define REG_IRQ_EXP_PENDING_H           0xBF203354

//FIQ registers
#define REG_FIQ_MASK_L              0xBF200A80
#define REG_FIQ_MASK_H              0xBF200A84
#define REG_FIQ_FORCE_L             0xBF200A88
#define REG_FIQ_FORCE_H             0xBF200A8C
#define REG_FIQ_CLEAR_L             0xBF200A90
#define REG_FIQ_CLEAR_H             0xBF200A94
#define REG_FIQ_RAW_L               0xBF200A98
#define REG_FIQ_RAW_H               0xBF200A9C
#define REG_FIQ_PENDING_L           0xBF200AA0
#define REG_FIQ_PENDING_H           0xBF200AA4

//FIQ EXP registers
#define REG_FIQ_EXP_MASK_L              0xBF203200
#define REG_FIQ_EXP_MASK_H              0xBF203204
#define REG_FIQ_EXP_FORCE_L             0xBF203208
#define REG_FIQ_EXP_FORCE_H             0xBF20320C
#define REG_FIQ_EXP_CLEAR_L             0xBF203210
#define REG_FIQ_EXP_CLEAR_H             0xBF203214
#define REG_FIQ_EXP_RAW_L               0xBF203218
#define REG_FIQ_EXP_RAW_H               0xBF20321C
#define REG_FIQ_EXP_PENDING_L           0xBF203220
#define REG_FIQ_EXP_PENDING_H           0xBF203224


#define MSTAR_INT_BASE  8
/*******************************************************/
/*   THE IRQ AND FIQ ARE NOT COMPLETED.                */
/*   FOR EACH IP OWNER, PLEASE REVIEW IT BY YOURSELF   */
/*******************************************************/
typedef enum
{
    // IRQ
    E_IRQL_START                 = MSTAR_INT_BASE,
    E_IRQ_UART0                 = E_IRQL_START  + 0,
    E_IRQ_BDMA_CH0              = E_IRQL_START  + 1,
    E_IRQ_BDMA_CH1              = E_IRQL_START  + 2,
    E_IRQ_MVD                   = E_IRQL_START  + 3,
    E_IRQ_PS                    = E_IRQL_START  + 4,
    E_IRQ_NFIE                  = E_IRQL_START  + 5,
    E_IRQ_USB                   = E_IRQL_START  + 6,
    E_IRQ_UHC                   = E_IRQL_START  + 7,
    E_IRQ_EC_BRIDGE             = E_IRQL_START  + 8,
    E_IRQ_EMAC                  = E_IRQL_START  + 9,
    E_IRQ_DISP                  = E_IRQL_START  + 10,
    E_IRQ_DHC                   = E_IRQL_START  + 11,
    E_IRQ_SBM                   = E_IRQL_START  + 13,
    E_IRQ_COMB                  = E_IRQL_START  + 14,
    E_IRQ_ECC_DERR              = E_IRQL_START  + 15,
    E_IRQL_END                  = 15+ MSTAR_INT_BASE,

    E_IRQH_START                = 16+ MSTAR_INT_BASE,
    E_IRQ_TSP2HK                = E_IRQH_START  + 0,
    E_IRQ_VE                    = E_IRQH_START  + 1,
    E_IRQ_CIMAX2MCU             = E_IRQH_START  + 2,
    E_IRQ_DC                    = E_IRQH_START  + 3,
    E_IRQ_GOP                   = E_IRQH_START  + 4,
    E_IRQ_PCM                   = E_IRQH_START  + 5,
    E_IRQ_IIC0                  = E_IRQH_START  + 6,
    E_IRQ_RTC                   = E_IRQH_START  + 7,
    E_IRQ_PM                    = E_IRQH_START  + 9,
    E_IRQ_DDC2BI                = E_IRQH_START  + 10,
    E_IRQ_SCM                   = E_IRQH_START  + 11,
    E_IRQ_VBI                   = E_IRQH_START  + 12,
    E_IRQ_MLINK                 = E_IRQH_START  + 14,
    E_IRQ_ADCDVI2RIU            = E_IRQH_START  + 15,
    E_IRQH_END                  = 31+ MSTAR_INT_BASE,

    // FIQ
    E_FIQL_START                = 32 + MSTAR_INT_BASE,
    E_FIQ_EXTIMER0              = E_FIQL_START +  0,
    E_FIQ_EXTIMER1              = E_FIQL_START +  1,
    E_FIQ_WDT                   = E_FIQL_START +  2,
    E_FIQ_AEON_TO_8051          = E_FIQL_START +  3,
    E_FIQ_8051_TO_AEON          = E_FIQL_START +  4,
    E_FIQ_8051_TO_BEON          = E_FIQL_START +  5,
    E_FIQ_BEON_TO_8051          = E_FIQL_START +  6,
    E_FIQ_BEON_TO_AEON          = E_FIQL_START +  7,
    E_FIQ_AEON_TO_BEON          = E_FIQL_START +  8,

    /* add for compatiable T2 MBx driver. */
    E_FIQ_INT_AEON_TO_8051      = E_FIQL_START +  3,
    E_FIQ_INT_8051_TO_AEON      = E_FIQL_START +  4,
    E_FIQ_INT_8051_TO_MIPS      = E_FIQL_START +  5,
    E_FIQ_INT_MIPS_TO_8051      = E_FIQL_START +  6,
    E_FIQ_INT_MIPS_TO_AEON      = E_FIQL_START +  7,
    E_FIQ_INT_AEON_TO_MIPS      = E_FIQL_START +  8,
    /* ----------------------------------- */
    E_FIQ_DMARD                 = E_FIQL_START +  9,
    E_FIQ_MENULOAD              = E_FIQL_START + 10,
    E_FIQ_HDMI_NON_PCM          = E_FIQL_START + 11,
    E_FIQ_SPDIF_IN_NON_PCM      = E_FIQL_START + 12,
    E_FIQ_EMAC                  = E_FIQL_START + 13,
    E_FIQ_SE_DSP2UP             = E_FIQL_START + 14,
    E_FIQ_TSP2AEON              = E_FIQL_START + 15,
    E_FIQL_END                  = 47+ MSTAR_INT_BASE,

    // E_FIQ_JPD                   = E_FIQL_START +  9,   // ??

    E_FIQH_START                = 48+ MSTAR_INT_BASE,
    E_FIQ_VIVALDI_STR           = E_FIQH_START +  0,
    E_FIQ_VIVALDI_PTS           = E_FIQH_START +  1,
    E_FIQ_DSP_MIU_PROT          = E_FIQH_START +  2,
    E_FIQ_XIU_TIMEOUT           = E_FIQH_START +  3,
    E_FIQ_DMA_DONE              = E_FIQH_START +  4,
    E_FIQ_VSYNC_VE4VBI          = E_FIQH_START +  5,
    E_FIQ_FIELD_VE4VBI          = E_FIQH_START +  6,
    E_FIQ_VDMCU2HK              = E_FIQH_START +  7,
    E_FIQ_VE_DONE_TT            = E_FIQH_START +  8,
    E_FIQ_INT_CCFL              = E_FIQH_START +  9,
    E_FIQ_INT                   = E_FIQH_START + 10,
    E_FIQ_IR                    = E_FIQH_START + 11,
    E_FIQ_AFEC_VSYNC            = E_FIQH_START + 12,
    E_FIQ_DEC_DSP2UP            = E_FIQH_START + 13,
    E_FIQ_MIPS_WDT              = E_FIQH_START + 14,
    E_FIQ_DSP2MIPS              = E_FIQH_START + 15,
    E_FIQH_END                  = 63+ MSTAR_INT_BASE,

    //IRQEXP
    E_IRQEXPL_START             = 64+ MSTAR_INT_BASE,
    E_IRQEXPL_HVD               = E_IRQEXPL_START+0,
    E_IRQEXPL_USB2              = E_IRQEXPL_START+1,
    E_IRQEXPL_UHC2              = E_IRQEXPL_START+2,
    E_IRQEXPL_MIU               = E_IRQEXPL_START+3,
    E_IRQEXPL_GDMA              = E_IRQEXPL_START+4,
    E_IRQEXPL_UART2             = E_IRQEXPL_START+5,
    E_IRQEXPL_UART1             = E_IRQEXPL_START+6,
    E_IRQEXPL_DEMOD             = E_IRQEXPL_START+7,
    E_IRQEXPL_MPIF              = E_IRQEXPL_START+8,
    E_IRQEXPL_JPD               = E_IRQEXPL_START+9,  // ?
    E_IRQEXPL_AEON2HI           = E_IRQEXPL_START+10,
    E_IRQEXPL_BDMA0             = E_IRQEXPL_START+11,
    E_IRQEXPL_BDMA1             = E_IRQEXPL_START+12,
    E_IRQEXPL_OTG               = E_IRQEXPL_START+13,
    E_IRQEXPL_MVD_CHECKSUM_FAIL = E_IRQEXPL_START+14,
    E_IRQEXPL_TSP_CHECKSUM_FAIL = E_IRQEXPL_START+15,
    E_IRQEXPL_END               = 79+ MSTAR_INT_BASE,

    E_IRQEXPH_START             = 80+ MSTAR_INT_BASE,
    E_IRQEXPH_CA_I3             = E_IRQEXPH_START+0,
    E_IRQEXPH_HDMI_LEVEL        = E_IRQEXPH_START+1,
    E_IRQEXPH_MIPS_WADR_ERR     = E_IRQEXPH_START+2,
    E_IRQEXPH_RASP              = E_IRQEXPH_START+3,
    E_IRQEXPH_CA_SVP            = E_IRQEXPH_START+4,
    E_IRQEXPH_UART2MCU          = E_IRQEXPH_START+5,
    E_IRQEXPH_URDMA2MCU         = E_IRQEXPH_START+6,
    E_IRQEXPH_IIC1              = E_IRQEXPH_START+7,
    E_IRQEXPH_HDCP              = E_IRQEXPH_START+8,
    E_IRQEXPH_DMA_WADR_ERR      = E_IRQEXPH_START+9,
    E_IRQEXPH_UP_IRQ_UART_CA    = E_IRQEXPH_START+10,
    E_IRQEXPH_UP_IRQ_EMM_ECM    = E_IRQEXPH_START+11,
    E_IRQEXPH_13_RESERVE        = E_IRQEXPH_START+12,
    E_IRQEXPH_14_RESERVE        = E_IRQEXPH_START+13,
    E_IRQEXPH_15_RESERVE        = E_IRQEXPH_START+14,
    E_IRQEXPH_16_RESERVE        = E_IRQEXPH_START+15,
    E_IRQEXPH_END               = 95+ MSTAR_INT_BASE,

    //FIQEXP
    E_FIQEXPL_START             = 96+ MSTAR_INT_BASE,
    E_FIQEXPL_IR_INT_RC         = E_FIQEXPL_START+0,
    E_FIQEXPL_HDMITX_IRQ_EDGE   = E_FIQEXPL_START+1,
    E_FIQEXPL_UP_IRQ_UART_CA    = E_FIQEXPL_START+2,
    E_FIQEXPL_UP_IRQ_EMM_ECM    = E_FIQEXPL_START+3,
    E_FIQEXPL_PVR2MI_INT0       = E_FIQEXPL_START+4,
    E_FIQEXPL_PVR2MI_INT1       = E_FIQEXPL_START+5,
    E_FIQEXPL_7_RESERVE         = E_FIQEXPL_START+6,
    E_FIQEXPL_8_RESERVE         = E_FIQEXPL_START+7,
    E_FIQEXPL_9_RESERVE         = E_FIQEXPL_START+8,
    E_FIQEXPL_10_RESERVE        = E_FIQEXPL_START+9,
    E_FIQEXPL_11_RESERVE        = E_FIQEXPL_START+10,
    E_FIQEXPL_12_RESERVE        = E_FIQEXPL_START+11,
    E_FIQEXPL_13_RESERVE        = E_FIQEXPL_START+12,
    E_FIQEXPL_14_RESERVE        = E_FIQEXPL_START+13,
    E_FIQEXPL_15_RESERVE        = E_FIQEXPL_START+14,
    E_FIQEXPL_16_RESERVE        = E_FIQEXPL_START+15,
    E_FIQEXPL_END               = 111+ MSTAR_INT_BASE,


    E_FIQEXPH_START             = 112+ MSTAR_INT_BASE,
    E_FIQEXPH_1_RESERVE         = E_FIQEXPH_START+0,
    E_FIQEXPH_2_RESERVE         = E_FIQEXPH_START+1,
    E_FIQEXPH_3_RESERVE         = E_FIQEXPH_START+2,
    E_FIQEXPH_4_RESERVE         = E_FIQEXPH_START+3,
    E_FIQEXPH_5_RESERVE         = E_FIQEXPH_START+4,
    E_FIQEXPH_6_RESERVE         = E_FIQEXPH_START+5,
    E_FIQEXPH_7_RESERVE         = E_FIQEXPH_START+6,
    E_FIQEXPH_8_RESERVE         = E_FIQEXPH_START+7,
    E_FIQEXPH_9_RESERVE         = E_FIQEXPH_START+8,
    E_FIQEXPH_10_RESERVE        = E_FIQEXPH_START+9,
    E_FIQEXPH_11_RESERVE        = E_FIQEXPH_START+10,
    E_FIQEXPH_12_RESERVE        = E_FIQEXPH_START+11,
    E_FIQEXPH_13_RESERVE        = E_FIQEXPH_START+12,
    E_FIQEXPH_14_RESERVE        = E_FIQEXPH_START+13,
    E_FIQEXPH_15_RESERVE        = E_FIQEXPH_START+14,
    E_FIQEXPH_16_RESERVE        = E_FIQEXPH_START+15,
    E_FIQEXPH_END               = 127+ MSTAR_INT_BASE,

    E_IRQ_FIQ_ALL               = 0xFF //all IRQs & FIQs
} InterruptNum;


    #define FIQL_MASK                           0xFFFF
    #define FIQL_EXTIMER0                       (0x1 << (E_FIQ_EXTIMER0     - E_FIQL_START) )
    #define FIQL_EXTIMER1                       (0x1 << (E_FIQ_EXTIMER1     - E_FIQL_START) )
    #define FIQL_WDT                            (0x1 << (E_FIQ_WDT          - E_FIQL_START) )
    #define FIQL_AEON_TO_8051                   (0x1 << (E_FIQ_AEON_TO_8051 - E_FIQL_START) )
    #define FIQL_8051_TO_AEON                   (0x1 << (E_FIQ_8051_TO_AEON - E_FIQL_START) )
    #define FIQL_8051_TO_BEON                   (0x1 << (E_FIQ_8051_TO_BEON - E_FIQL_START) )
    #define FIQL_BEON_TO_8051                   (0x1 << (E_FIQ_BEON_TO_8051 - E_FIQL_START) )
    #define FIQL_BEON_TO_AEON                   (0x1 << (E_FIQ_BEON_TO_AEON - E_FIQL_START) )
    #define FIQL_AEON_TO_BEON                   (0x1 << (E_FIQ_AEON_TO_BEON - E_FIQL_START) )
    // #define FIQL_JPD                            (0x1 << (E_FIQ_JPD          - E_FIQL_START) )
    #define FIQL_FIQ_MENULOAD                   (0x1 << (E_FIQ_MENULOAD     - E_FIQL_START) )
    #define FIQL_HDMI_NON_PCM                   (0x1 << (E_FIQ_HDMI_NON_PCM - E_FIQL_START) )
    #define FIQL_SPDIF_IN_NON_PCM               (0x1 << (E_FIQ_SPDIF_IN_NON_PCM - E_FIQL_START) )
    #define FIQL_EMAC                           (0x1 << (E_FIQ_EMAC         - E_FIQL_START) )
    #define FIQL_SE_DSP2UP                      (0x1 << (E_FIQ_SE_DSP2UP    - E_FIQL_START) )
    #define FIQL_TSP2AEON                       (0x1 << (E_FIQ_TSP2AEON     - E_FIQL_START) )

    // REG_FIQ_MASK_H
    //FIQ High 16 bits
    #define FIQH_MASK                           0xFFFF
    #define FIQH_VIVALDI_STR                    (0x1 << (E_FIQ_VIVALDI_STR  - E_FIQH_START) )
    #define FIQH_DSP_MIU_PROT                   (0x1 << (E_FIQ_DSP_MIU_PROT - E_FIQH_START) )
    #define FIQH_XIU_TIMEOUT                    (0x1 << (E_FIQ_XIU_TIMEOUT  - E_FIQH_START) )
    #define FIQH_DMA_DONE                       (0x1 << (E_FIQ_DMA_DONE     - E_FIQH_START) )
    #define FIQH_VSYNC_VE4VBI                   (0x1 << (E_FIQ_VSYNC_VE4VBI - E_FIQH_START) )
    #define FIQH_FIELD_VE4VBI                   (0x1 << (E_FIQ_FIELD_VE4VBI - E_FIQH_START) )
    #define FIQH_VDMCU2HK                       (0x1 << (E_FIQ_VDMCU2HK     - E_FIQH_START) )
    #define FIQH_VE_DONE_TT                     (0x1 << (E_FIQ_VE_DONE_TT   - E_FIQH_START) )
    #define FIQH_INT_CCFL                       (0x1 << (E_FIQ_INT_CCFL     - E_FIQH_START) )
    #define FIQH_INT                            (0x1 << (E_FIQ_INT          - E_FIQH_START) )
    #define FIQH_IR                             (0x1 << (E_FIQ_IR           - E_FIQH_START) )
    #define FIQH_AFEC_VSYNC                     (0x1 << (E_FIQ_AFEC_VSYNC   - E_FIQH_START) )
    #define FIQH_DSP2UP                         (0x1 << (E_FIQ_DEC_DSP2UP   - E_FIQH_START) )

    // #define REG_IRQ_PENDING_L
    #define IRQL_MASK                           0xFFFF
    #define IRQL_UART0                          (0x1 << (E_IRQ_UART0        - E_IRQL_START) )
    #define IRQL_BDMA_CH0                       (0x1 << (E_IRQ_BDMA_CH0     - E_IRQL_START) )
    #define IRQL_BDMA_CH1                       (0x1 << (E_IRQ_BDMA_CH1     - E_IRQL_START) )
    #define IRQL_MVD                            (0x1 << (E_IRQ_MVD          - E_IRQL_START) )
    #define IRQL_PS                             (0x1 << (E_IRQ_PS           - E_IRQL_START) )
    #define IRQL_NFIE                           (0x1 << (E_IRQ_NFIE         - E_IRQL_START) )
    #define IRQL_USB                            (0x1 << (E_IRQ_USB          - E_IRQL_START) )
    #define IRQL_UHC                            (0x1 << (E_IRQ_UHC          - E_IRQL_START) )
    #define IRQL_EC_BRIDGE                      (0x1 << (E_IRQ_EC_BRIDGE    - E_IRQL_START) )
    #define IRQL_EMAC                           (0x1 << (E_IRQ_EMAC         - E_IRQL_START) )
    #define IRQL_DISP                           (0x1 << (E_IRQ_DISP         - E_IRQL_START) )
    #define IRQL_DHC                            (0x1 << (E_IRQ_DHC          - E_IRQL_START) )
    // #define IRQL_PMSLEEP                        (0x1 << (E_IRQ_PMSLEEP      - E_IRQL_START) )
    #define IRQL_SBM                            (0x1 << (E_IRQ_SBM          - E_IRQL_START) )
    #define IRQL_COMB                           (0x1 << (E_IRQ_COMB         - E_IRQL_START) )
    #define IRQL_ECC_DERR                       (0x1 << (E_IRQ_ECC_DERR     - E_IRQL_START) )

    // #define REG_IRQ_PENDING_H
    #define IRQH_MASK                           0xFFFF
    #define IRQH_TSP2HK                         (0x1 << (E_IRQ_TSP2HK       - E_IRQH_START) )
    #define IRQH_VE                             (0x1 << (E_IRQ_VE           - E_IRQH_START) )
    #define IRQH_CIMAX2MCU                      (0x1 << (E_IRQ_CIMAX2MCU    - E_IRQH_START) )
    #define IRQH_DC                             (0x1 << (E_IRQ_DC           - E_IRQH_START) )
    #define IRQH_GOP                            (0x1 << (E_IRQ_GOP          - E_IRQH_START) )
    #define IRQH_PCM                            (0x1 << (E_IRQ_PCM          - E_IRQH_START) )
    #define IRQH_IIC0                           (0x1 << (E_IRQ_IIC0         - E_IRQH_START) )
    #define IRQH_RTC                            (0x1 << (E_IRQ_RTC          - E_IRQH_START) )
    // #define IRQH_KEYPAD                         (0x1 << (E_IRQ_KEYPAD       - E_IRQH_START) )
    #define IRQH_PM                             (0x1 << (E_IRQ_PM           - E_IRQH_START) )
    #define IRQH_D2B                            (0x1 << (E_IRQ_DDC2BI       - E_IRQH_START) )
    #define IRQH_SCM                            (0x1 << (E_IRQ_SCM          - E_IRQH_START) )
    #define IRQH_VBI                            (0x1 << (E_IRQ_VBI          - E_IRQH_START) )
    // #define IRQH_M4VD                           (0x1 << (E_IRQ_M4VD         - E_IRQH_START) )
    // #define IRQH_FCIE2RIU                       (0x1 << (E_IRQ_FCIE2RIU     - E_IRQH_START) )
    #define IRQH_ADCVI2RIU                      (0x1 << (E_IRQ_ADCDVI2RIU   - E_IRQH_START) )

    #define IRQEXPL_MASK                        0xFFFF
    #define IRQEXPL_HVD                         (0x1<< (E_IRQEXPL_HVD               - E_IRQEXPL_START))
    #define IRQEXPL_USB2                        (0x1<< (E_IRQEXPL_USB2              - E_IRQEXPL_START))
    #define IRQEXPL_UHC2                        (0x1<< (E_IRQEXPL_UHC2              - E_IRQEXPL_START))
    #define IRQEXPL_MIU                         (0x1<< (E_IRQEXPL_MIU               - E_IRQEXPL_START))
    #define IRQEXPL_GDMA                        (0x1<< (E_IRQEXPL_GDMA              - E_IRQEXPL_START))
    #define IRQEXPL_UART2                       (0x1<< (E_IRQEXPL_UART2             - E_IRQEXPL_START))
    #define IRQEXPL_UART1                       (0x1<< (E_IRQEXPL_UART1             - E_IRQEXPL_START))
    #define IRQEXPL_DEMOD                       (0x1<< (E_IRQEXPL_DEMOD             - E_IRQEXPL_START))
    #define IRQEXPL_MPIF                        (0x1<< (E_IRQEXPL_MPIF              - E_IRQEXPL_START))
    #define IRQEXPL_JPD                         (0x1<< (E_IRQEXPL_JPD               - E_IRQEXPL_START))
    #define IRQEXPL_AEON2HI                     (0x1<< (E_IRQEXPL_AEON2HI           - E_IRQEXPL_START))
    #define IRQEXPL_BDMA0                       (0x1<< (E_IRQEXPL_BDMA0             - E_IRQEXPL_START))
    #define IRQEXPL_BDMA1                       (0x1<< (E_IRQEXPL_BDMA1             - E_IRQEXPL_START))
    #define IRQEXPL_OTG                         (0x1<< (E_IRQEXPL_OTG               - E_IRQEXPL_START))
    #define IRQEXPL_MVD_CHECKSUM_FAIL           (0x1<< (E_IRQEXPL_MVD_CHECKSUM_FAIL - E_IRQEXPL_START))
    #define IRQEXPL_TSP_CHECKSUM_FAIL           (0x1<< (E_IRQEXPL_TSP_CHECKSUM_FAIL - E_IRQEXPL_START))

    #define IRQEXPH_MASK                        0xFFFF
    #define IRQEXPH_CA_I3                       (0x1<< (E_IRQEXPH_CA_I3             - E_IRQEXPH_START))
    #define IRQEXPH_HDMI_LEVEL                  (0x1<< (E_IRQEXPH_HDMI_LEVEL        - E_IRQEXPH_START))
    #define IRQEXPH_MIPS_WADR_ERR               (0x1<< (E_IRQEXPH_MIPS_WADR_ERR     - E_IRQEXPH_START))
    #define IRQEXPH_RASP                        (0x1<< (E_IRQEXPH_RASP              - E_IRQEXPH_START))
    #define IRQEXPH_CA_SVP                      (0x1<< (E_IRQEXPH_CA_SVP            - E_IRQEXPH_START))
    #define IRQEXPH_UART2MCU                    (0x1<< (E_IRQEXPH_UART2MCU          - E_IRQEXPH_START))
    #define IRQEXPH_URDMA2MCU                   (0x1<< (E_IRQEXPH_URDMA2MCU         - E_IRQEXPH_START))
    #define IRQEXPH_IIC1                        (0x1<< (E_IRQEXPH_IIC1              - E_IRQEXPH_START))
    #define IRQEXPH_HDCP                        (0x1<< (E_IRQEXPH_HDCP              - E_IRQEXPH_START))
    #define IRQEXPH_DMA_WADR_ERR                (0x1<< (E_IRQEXPH_DMA_WADR_ERR      - E_IRQEXPH_START))
    #define IRQEXPH_UP_IRQ_UART_CA              (0x1<< (E_IRQEXPH_UP_IRQ_UART_CA    - E_IRQEXPH_START))
    #define IRQEXPH_UP_IRQ_EMM_ECM              (0x1<< (E_IRQEXPH_UP_IRQ_EMM_ECM    - E_IRQEXPH_START))
    #define IRQEXPH_13_RESERVE                  (0x1<< (E_IRQEXPH_13_RESERVE        - E_IRQEXPH_START))
    #define IRQEXPH_14_RESERVE                  (0x1<< (E_IRQEXPH_14_RESERVE        - E_IRQEXPH_START))
    #define IRQEXPH_15_RESERVE                  (0x1<< (E_IRQEXPH_15_RESERVE        - E_IRQEXPH_START))
    #define IRQEXPH_16_RESERVE                  (0x1<< (E_IRQEXPH_16_RESERVE        - E_IRQEXPH_START))

    #define FIQEXPL_MASK                        0xFFFF
    #define FIQEXPL_IR_INT_RC                   (0x1<< (E_FIQEXPL_IR_INT_RC         - E_FIQEXPL_START))
    #define FIQEXPL_HDMITX_IRQ_EDGE             (0x1<< (E_FIQEXPL_HDMITX_IRQ_EDGE   - E_FIQEXPL_START))
    #define FIQEXPL_UP_IRQ_UART_CA              (0x1<< (E_FIQEXPL_UP_IRQ_UART_CA    - E_FIQEXPL_START))
    #define FIQEXPL_UP_IRQ_EMM_ECM              (0x1<< (E_FIQEXPL_UP_IRQ_EMM_ECM    - E_FIQEXPL_START))
    #define FIQEXPL_PVR2MI_INT0                 (0x1<< (E_FIQEXPL_PVR2MI_INT0       - E_FIQEXPL_START))
    #define FIQEXPL_PVR2MI_INT1                 (0x1<< (E_FIQEXPL_PVR2MI_INT1       - E_FIQEXPL_START))

    #define FIQEXPH_MASK                        0xFFFF

#define IRQL_EXP_ALL                    0xFFFF
#define IRQH_EXP_ALL                    0xFFFF
#define FIQL_EXP_ALL                    0xFFFF
#define FIQH_EXP_ALL                    0xFFFF

#if 0
//IRQ Low 16 bits
#define IRQL_UART                   (0x1 << (E_IRQ_UART0 - E_IRQL_START))
#define IRQL_MVD                    (0x1 << (E_IRQ_MVD   - E_IRQL_START))
#define IRQL_UHC                    (0x1 << (E_IRQ_UHC   - E_IRQL_START))
#define IRQL_UART2                  (0x1 << (E_IRQ_UART2 - E_IRQL_START))
#define IRQL_UART1                  (0x1 << (E_IRQ_UART1 - E_IRQL_START))
#define IRQL_UART0                  (0x1 << (E_IRQ_UART0 - E_IRQL_START))
#define IRQL_EMAC                   (0x1 << (E_IRQ_EMAC  - E_IRQL_START))
#define IRQL_DEB                    (0x1 << (E_IRQ_DEB   - E_IRQL_START))
#define IRQL_EMAC                   (0x1 << (E_IRQ_EMAC  - E_IRQL_START))
#define IRQL_MICOM                  (0x1 << (E_IRQ_MICOM - E_IRQL_START))
#define IRQL_COMB                    (0x1 << (E_IRQ_COMB - E_IRQL_START))
#define IRQL_ALL                    0xFFFF //[15:0]

//IRQ High 16 bits
#define IRQH_TSP                    (0x1 << (E_IRQ_TSP      - E_IRQH_START)   )
#define IRQH_VE                     (0x1 << (E_IRQ_VE       - E_IRQH_START)   )
#define IRQH_CIMAX2MCU              (0x1 << (E_IRQ_CIMAX2MCU- E_IRQH_START)   )
#define IRQH_DC                     (0x1 << (E_IRQ_DC       - E_IRQH_START)   )
#define IRQH_GOP                    (0x1 << (E_IRQ_GOP      - E_IRQH_START)   )
#define IRQH_PCM2MCU                (0x1 << (E_IRQ_PCM2MCU  - E_IRQH_START)   )
#define IRQH_IIC0                   (0x1 << (E_IRQ_IIC0     - E_IRQH_START)   )
#define IRQH_RTC                    (0x1 << (E_IRQ_RTC      - E_IRQH_START)   )
#define IRQH_KEYPAD                 (0x1 << (E_IRQ_KEYPAD   - E_IRQH_START)   )
#define IRQH_PM                     (0x1 << (E_IRQ_PM       - E_IRQH_START)   )
#define IRQH_D2B                    (0x1 << (E_IRQ_D2B      - E_IRQH_START)   )//2008/10/23 Nick
#define IRQH_VBI                    (0x1 << (E_IRQ_VBI      - E_IRQH_START)   )
#define IRQH_M4V                    (0x1 << (E_IRQ_M4V      - E_IRQH_START)   )
#define IRQH_MLINK                  (0x1 << (E_IRQ_MLINK    - E_IRQH_START)   )
#define IRQH_HDMITX                 (0x1 << (E_IRQ_HDMITX   - E_IRQH_START)   )
#define IRQH_ALL                    0xFFFF //[15:0]


//FIQ Low 16 bits
#define FIQL_AEON_TO_8051           (0x1 << (E_FIQ_INT_AEON_TO_8051 - E_FIQL_START)  )
#define FIQL_8051_TO_AEON           (0x1 << (E_FIQ_INT_8051_TO_AEON - E_FIQL_START)  )
#define FIQL_8051_TO_MIPS           (0x1 << (E_FIQ_INT_8051_TO_MIPS - E_FIQL_START)  )
#define FIQL_MIPS_TO_8051           (0x1 << (E_FIQ_INT_MIPS_TO_8051 - E_FIQL_START)  )
#define FIQL_MIPS_TO_AEON           (0x1 << (E_FIQ_INT_MIPS_TO_AEON - E_FIQL_START)  )
#define FIQL_AEON_TO_MIPS           (0x1 << (E_FIQ_INT_AEON_TO_MIPS - E_FIQL_START)  )
#define FIQL_DSP2_TO_MIPS           (0x1 << (E_FIQ_SE_DSP2UP - E_FIQL_START)  )
#define FIQL_ALL                    0xFFFF //[15:0]


//FIQ High 16 bits
#define FIQH_DSP2UP                 (0x1 << (E_FIQ_DEC_DSP2UP - E_FIQH_START)  )
#define FIQH_VSYN_GOP1              (0x1 << (E_FIQ_INT_CCFL - E_FIQH_START)  )  //?? tmp
#define FIQH_VSYN_GOP0              (0x1 << (E_FIQ_INT - E_FIQH_START)  )  //?? tmp
#define FIQH_IR                     (0x1 << (E_FIQ_IR - E_FIQH_START)   )
#define FIQH_VSYNC_VE4VBI           (0x1 << (E_FIQ_VSYNC_VE4VBI - E_FIQH_START)   )
#define FIQH_FIELD_VE4VBI           (0x1 << (E_FIQ_FIELD_VE4VBI - E_FIQH_START)   )
#define FIQH_DSP2MIPS               (0x1 << (E_FIQ_SE_DSP2UP - E_FIQL_START)  )

//IRQ EXP Low 16 bits
#define IRQ_HVD                   (0x1 << (E_IRQEXP_HVD  - E_IRQL_EXP_START))
#define IRQ_USB1                    (0x1 << (E_IRQEXP_USB2   - E_IRQL_EXP_START))
#define IRQ_UHC1                    (0x1 << (E_IRQEXP_UHC2   - E_IRQL_EXP_START))
#define IRQ_MIU                     (0x1 << (E_IRQEXP_MIU - E_IRQL_EXP_START))
#define IRQL_EXP_ALL                    0xFFFF //[15:0]

//IRQ EXP High 16 bits
#define IRQH_EXP_ALL                    0xFFFF //[15:0]

//FIQ EXP Low 16 bits
#define FIQL_EXP_ALL                    0xFFFF //[15:0]
//FIQ EXP High 16 bits
#define FIQH_EXP_ALL                    0xFFFF //[15:0]

#define FIQH_ALL                    0xFFFF //[15:0]
#endif

#endif // #ifndef __CHIP_INT_H__

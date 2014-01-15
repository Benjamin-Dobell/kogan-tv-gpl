#define TITANIAINT_END 128//64

//IRQ registers
#define REG_IRQ_MASK_L              0xBF800AB0
#define REG_IRQ_MASK_H              0xBF800AB4
#define REG_IRQ_FORCE_L             0xBF800AB8
#define REG_IRQ_FORCE_H             0xBF800ABC
#define REG_IRQ_RAW_L               0xBF800AC8
#define REG_IRQ_RAW_H               0xBF800ACC
#define REG_IRQ_PENDING_L           0xBF800AD0
#define REG_IRQ_PENDING_H           0xBF800AD4

//IRQ EXP registers
#define REG_IRQ_EXP_MASK_L              0xBF803330
#define REG_IRQ_EXP_MASK_H              0xBF803334
#define REG_IRQ_EXP_FORCE_L             0xBF803338
#define REG_IRQ_EXP_FORCE_H             0xBF80333C
#define REG_IRQ_EXP_RAW_L               0xBF803348
#define REG_IRQ_EXP_RAW_H               0xBF80334C
#define REG_IRQ_EXP_PENDING_L           0xBF803350
#define REG_IRQ_EXP_PENDING_H           0xBF803354

//FIQ registers
#define REG_FIQ_MASK_L              0xBF800A80
#define REG_FIQ_MASK_H              0xBF800A84
#define REG_FIQ_FORCE_L             0xBF800A88
#define REG_FIQ_FORCE_H             0xBF800A8C
#define REG_FIQ_CLEAR_L             0xBF800A90
#define REG_FIQ_CLEAR_H             0xBF800A94
#define REG_FIQ_RAW_L               0xBF800A98
#define REG_FIQ_RAW_H               0xBF800A9C
#define REG_FIQ_PENDING_L           0xBF800AA0
#define REG_FIQ_PENDING_H           0xBF800AA4

//FIQ EXP registers
#define REG_FIQ_EXP_MASK_L              0xBF803200
#define REG_FIQ_EXP_MASK_H              0xBF803204
#define REG_FIQ_EXP_FORCE_L             0xBF803208
#define REG_FIQ_EXP_FORCE_H             0xBF80320C
#define REG_FIQ_EXP_CLEAR_L             0xBF803210
#define REG_FIQ_EXP_CLEAR_H             0xBF803214
#define REG_FIQ_EXP_RAW_L               0xBF803218
#define REG_FIQ_EXP_RAW_H               0xBF80321C
#define REG_FIQ_EXP_PENDING_L           0xBF803220
#define REG_FIQ_EXP_PENDING_H           0xBF803224


#define MSTAR_INT_BASE  8
/*******************************************************/
/*   THE IRQ AND FIQ ARE NOT COMPLETED.                */
/*   FOR EACH IP OWNER, PLEASE REVIEW IT BY YOURSELF   */
/*******************************************************/
typedef enum
{
    //IRQ
    E_IRQL_START            = MSTAR_INT_BASE,
    E_IRQ_UART              = E_IRQL_START+0,   /*Titania PIU UART for MIPS*/
    E_IRQ_MVD               = E_IRQL_START+3,
    E_IRQ_EMAC              = E_IRQL_START+9,
    E_IRQ_UART0             = E_IRQL_START+4,
    E_IRQ_UART1             = E_IRQL_START+5,
    E_IRQ_IIC1              = E_IRQL_START+6,
    E_IRQ_UHC               = E_IRQL_START+7,			/* 1st EHCI */
    E_IRQ_SECOND_UHC  = E_IRQL_START+34,		/* tony add for 2st EHCI */
    E_IRQ_SPI_ARB_DUP_REG   = E_IRQL_START+8,
    E_IRQ_SPI_ARB_CHG_ADDR  = E_IRQL_START+3,
    E_IRQ_DEB               = E_IRQL_START+10,
    E_IRQ_UART2             = E_IRQL_START+11,
    E_IRQ_MICOM             = E_IRQL_START+12,
    E_IRQ_SBM               = E_IRQL_START+13,
    E_IRQ_COMB               = E_IRQL_START+14,
    E_IRQ_ECC_DERR          = E_IRQL_START+15,
    E_IRQL_END              = 15+MSTAR_INT_BASE,

    E_IRQH_START            = 16+MSTAR_INT_BASE,
    E_IRQ_TSP               = E_IRQH_START+0,
    E_IRQ_VE                = E_IRQH_START+1,
    E_IRQ_CIMAX2MCU         = E_IRQH_START+2,
    E_IRQ_DC                = E_IRQH_START+3,
    E_IRQ_GOP               = E_IRQH_START+4,
    E_IRQ_PCM2MCU           = E_IRQH_START+5,
    E_IRQ_IIC0              = E_IRQH_START+6,
    E_IRQ_RTC               = E_IRQH_START+7,
    E_IRQ_KEYPAD            = E_IRQH_START+8,
    E_IRQ_PM                = E_IRQH_START+9,
    E_IRQ_D2B               = E_IRQH_START+10,  //2008/10/23 Nick
    E_IRQ_VBI               = E_IRQH_START+12,
    E_IRQ_M4V               = E_IRQH_START+13,
    E_IRQ_MLINK             = E_IRQH_START+14,
    E_IRQ_HDMITX            = E_IRQH_START+15,
    E_IRQH_END              = 31+MSTAR_INT_BASE,

    // FIQ
    E_FIQL_START                = 32+MSTAR_INT_BASE,
    E_FIQ_EXTIMER0              = E_FIQL_START +  0,
    E_FIQ_EXTIMER1              = E_FIQL_START +  1,
    E_FIQ_WDT                   = E_FIQL_START +  2,
    E_FIQ_INT_AEON_TO_8051          = E_FIQL_START +  3,
    E_FIQ_INT_8051_TO_AEON          = E_FIQL_START +  4,
    E_FIQ_INT_8051_TO_MIPS          = E_FIQL_START +  5,
    E_FIQ_INT_MIPS_TO_8051          = E_FIQL_START +  6,
    E_FIQ_INT_MIPS_TO_AEON          = E_FIQL_START +  7,
    E_FIQ_INT_AEON_TO_MIPS          = E_FIQL_START +  8,
    E_FIQ_JPD                   = E_FIQL_START +  9,
    E_FIQ_MENULOAD              = E_FIQL_START + 10,
    E_FIQ_HDMI_NON_PCM          = E_FIQL_START + 11,
    E_FIQ_SPDIF_IN_NON_PCM      = E_FIQL_START + 12,
    E_FIQ_EMAC                  = E_FIQL_START + 13,
    E_FIQ_SE_DSP2UP             = E_FIQL_START + 14,
    E_FIQ_TSP2AEON              = E_FIQL_START + 15,
    E_FIQL_END                  = 47+MSTAR_INT_BASE,

    E_FIQH_START                = 48+MSTAR_INT_BASE,
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
    E_FIQ_DEC_DSP2MIPS          = E_FIQH_START + 15,
    E_FIQH_END                  = 63+MSTAR_INT_BASE,

    //IRQEXP
    E_IRQL_EXP_START             = 64+MSTAR_INT_BASE,
    E_IRQEXP_SVD                = E_IRQL_EXP_START+0,
    E_IRQEXP_USB2               = E_IRQL_EXP_START+1,
    E_IRQEXP_UHC2               = E_IRQL_EXP_START+2,
    E_IRQEXP_MIU                = E_IRQL_EXP_START+3,
    E_IRQEXPL_1_RESERVE         = E_IRQL_EXP_START+4,
    E_IRQEXPL_2_RESERVE         = E_IRQL_EXP_START+5,
    E_IRQEXPL_3_RESERVE         = E_IRQL_EXP_START+6,
    E_IRQEXPL_4_RESERVE         = E_IRQL_EXP_START+7,
    E_IRQEXPL_5_RESERVE         = E_IRQL_EXP_START+8,
    E_IRQEXPL_6_RESERVE         = E_IRQL_EXP_START+9,
    E_IRQEXPL_7_RESERVE         = E_IRQL_EXP_START+10,
    E_IRQEXPL_8_RESERVE         = E_IRQL_EXP_START+11,
    E_IRQEXPL_9_RESERVE         = E_IRQL_EXP_START+12,
    E_IRQEXPL_10_RESERVE        = E_IRQL_EXP_START+13,
    E_IRQEXPL_11_RESERVE        = E_IRQL_EXP_START+14,
    E_IRQEXPL_12_RESERVE        = E_IRQL_EXP_START+15,
    E_IRQL_EXP_END               = 79+MSTAR_INT_BASE,

    E_IRQH_EXP_START             = 80+MSTAR_INT_BASE,
    E_IRQEXPH_1_RESERVE         = E_IRQH_EXP_START+0,
    E_IRQEXPH_2_RESERVE         = E_IRQH_EXP_START+1,
    E_IRQEXPH_3_RESERVE         = E_IRQH_EXP_START+2,
    E_IRQEXPH_4_RESERVE         = E_IRQH_EXP_START+3,
    E_IRQEXPH_5_RESERVE         = E_IRQH_EXP_START+4,
    E_IRQEXPH_6_RESERVE         = E_IRQH_EXP_START+5,
    E_IRQEXPH_7_RESERVE         = E_IRQH_EXP_START+6,
    E_IRQEXPH_8_RESERVE         = E_IRQH_EXP_START+7,
    E_IRQEXPH_9_RESERVE         = E_IRQH_EXP_START+8,
    E_IRQEXPH_10_RESERVE        = E_IRQH_EXP_START+9,
    E_IRQEXPH_11_RESERVE        = E_IRQH_EXP_START+10,
    E_IRQEXPH_12_RESERVE        = E_IRQH_EXP_START+11,
    E_IRQEXPH_13_RESERVE        = E_IRQH_EXP_START+12,
    E_IRQEXPH_14_RESERVE        = E_IRQH_EXP_START+13,
    E_IRQEXPH_15_RESERVE        = E_IRQH_EXP_START+14,
    E_IRQEXPH_16_RESERVE        = E_IRQH_EXP_START+15,
    E_IRQH_EXP_END               = 95+MSTAR_INT_BASE,

    //IRQEXP
    E_FIQL_EXP_START             = 96+MSTAR_INT_BASE,
    E_FIQEXPL_1_RESERVE         = E_FIQL_EXP_START+0,
    E_FIQEXPL_2_RESERVE         = E_FIQL_EXP_START+1,
    E_FIQEXPL_3_RESERVE         = E_FIQL_EXP_START+2,
    E_FIQEXPL_4_RESERVE         = E_FIQL_EXP_START+3,
    E_FIQEXPL_5_RESERVE         = E_FIQL_EXP_START+4,
    E_FIQEXPL_6_RESERVE         = E_FIQL_EXP_START+5,
    E_FIQEXPL_7_RESERVE         = E_FIQL_EXP_START+6,
    E_FIQEXPL_8_RESERVE         = E_FIQL_EXP_START+7,
    E_FIQEXPL_9_RESERVE         = E_FIQL_EXP_START+8,
    E_FIQEXPL_10_RESERVE        = E_FIQL_EXP_START+9,
    E_FIQEXPL_11_RESERVE        = E_FIQL_EXP_START+10,
    E_FIQEXPL_12_RESERVE        = E_FIQL_EXP_START+11,
    E_FIQEXPL_13_RESERVE        = E_FIQL_EXP_START+12,
    E_FIQEXPL_14_RESERVE        = E_FIQL_EXP_START+13,
    E_FIQEXPL_15_RESERVE        = E_FIQL_EXP_START+14,
    E_FIQEXPL_16_RESERVE        = E_FIQL_EXP_START+15,
    E_FIQL_EXP_END               = 111+MSTAR_INT_BASE,


    E_FIQH_EXP_START             = 112+MSTAR_INT_BASE,
    E_FIQEXPH_1_RESERVE         = E_FIQH_EXP_START+0,
    E_FIQEXPH_2_RESERVE         = E_FIQH_EXP_START+1,
    E_FIQEXPH_3_RESERVE         = E_FIQH_EXP_START+2,
    E_FIQEXPH_4_RESERVE         = E_FIQH_EXP_START+3,
    E_FIQEXPH_5_RESERVE         = E_FIQH_EXP_START+4,
    E_FIQEXPH_6_RESERVE         = E_FIQH_EXP_START+5,
    E_FIQEXPH_7_RESERVE         = E_FIQH_EXP_START+6,
    E_FIQEXPH_8_RESERVE         = E_FIQH_EXP_START+7,
    E_FIQEXPH_9_RESERVE         = E_FIQH_EXP_START+8,
    E_FIQEXPH_10_RESERVE        = E_FIQH_EXP_START+9,
    E_FIQEXPH_11_RESERVE        = E_FIQH_EXP_START+10,
    E_FIQEXPH_12_RESERVE        = E_FIQH_EXP_START+11,
    E_FIQEXPH_13_RESERVE        = E_FIQH_EXP_START+12,
    E_FIQEXPH_14_RESERVE        = E_FIQH_EXP_START+13,
    E_FIQEXPH_15_RESERVE        = E_FIQH_EXP_START+14,
    E_FIQEXPH_16_RESERVE        = E_FIQH_EXP_START+15,
    E_FIQH_EXP_END               = 127+MSTAR_INT_BASE,

    E_IRQ_FIQ_ALL               = 0xFF //all IRQs & FIQs

#if 0
    //IRQ EXP
    E_IRQL_EXP_START            = 32+MSTAR_INT_BASE,
    E_IRQ_SVD              = E_IRQL_EXP_START+0,
    E_IRQ_USB1               = E_IRQL_EXP_START+1,
    E_IRQ_UHC1              = E_IRQL_EXP_START+2,
    E_IRQ_MIU             = E_IRQL_EXP_START+3,
    E_IRQL_EXP_END              = 47+MSTAR_INT_BASE,

    E_IRQH_EXP_START            = 48+MSTAR_INT_BASE,
    E_IRQH_EXP_END            = 63+MSTAR_INT_BASE,

    //FIQ
    E_FIQL_START            = 64+MSTAR_INT_BASE,
    E_FIQ_INT_AEON_TO_8051  = E_FIQL_START+3,
    E_FIQ_INT_8051_TO_AEON  = E_FIQL_START+4,
    E_FIQ_INT_8051_TO_MIPS  = E_FIQL_START+5,
    E_FIQ_INT_MIPS_TO_8051  = E_FIQL_START+6,
    E_FIQ_INT_MIPS_TO_AEON  = E_FIQL_START+7,
    E_FIQ_INT_AEON_TO_MIPS  = E_FIQL_START+8,
    E_FIQ_DSP2_TO_MIPS  = E_FIQL_START+14,
    E_FIQL_END              = 79+MSTAR_INT_BASE,

    E_FIQH_START            = 80+MSTAR_INT_BASE,
    E_FIQ_VSYNC_VE4VBI      = E_FIQH_START+5,
    E_FIQ_FIELD_VE4VBI      = E_FIQH_START+6,
    E_FIQ_VSYN_COP          = E_FIQH_START+9,
    E_FIQ_VSYN_GOP0         = E_FIQH_START+11,
    E_FIQ_VSYN_GOP1         = E_FIQH_START+10,
    E_FIQ_IR                = E_FIQH_START+11,
    E_FIQ_DSP2UP            = E_FIQH_START+13,
    E_FIQ_DSP2MIPS          = E_FIQH_START+15,
    E_FIQH_END              = 95+MSTAR_INT_BASE,

    //FIQ EXP
    E_FIQL_EXP_START            = 96+MSTAR_INT_BASE,
    E_FIQL_EXP_END           = 111+MSTAR_INT_BASE,

    E_FIQH_EXP_START            = 112+MSTAR_INT_BASE,
    E_FIQH_EXP_END           = 127+MSTAR_INT_BASE,

    E_IRQ_FIQ_ALL           = 0xFF //all IRQs & FIQs
#endif
} InterruptNum;

//IRQ Low 16 bits
#define IRQL_UART                   (0x1 << (E_IRQ_UART  - E_IRQL_START))
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
#define IRQ_SVD                   (0x1 << (E_IRQEXP_SVD  - E_IRQL_EXP_START))
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



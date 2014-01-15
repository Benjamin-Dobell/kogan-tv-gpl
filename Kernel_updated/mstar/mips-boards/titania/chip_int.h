#define TITANIAINT_END 64

//IRQ registers
#define REG_IRQ_MASK_L              0xBF800AB0
#define REG_IRQ_MASK_H              0xBF800AB4
#define REG_IRQ_FORCE_L             0xBF800AB8
#define REG_IRQ_FORCE_H             0xBF800ABC
#define REG_IRQ_RAW_L               0xBF800AC8
#define REG_IRQ_RAW_H               0xBF800ACC
#define REG_IRQ_PENDING_L           0xBF800AD0
#define REG_IRQ_PENDING_H           0xBF800AD4
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


/*******************************************************/
/*   THE IRQ AND FIQ ARE NOT COMPLETED.                */
/*   FOR EACH IP OWNER, PLEASE REVIEW IT BY YOURSELF   */
/*******************************************************/
typedef enum
{
    //IRQ
    E_IRQL_START            = 0,
    E_IRQ_UART              = E_IRQL_START+0,   /*Titania PIU UART for MIPS*/
    E_IRQ_MVD               = E_IRQL_START+3,
    E_IRQ_EMAC              = E_IRQL_START+9,
    E_IRQ_UART0             = E_IRQL_START+4,
    E_IRQ_UART1             = E_IRQL_START+5,
    E_IRQ_IIC1              = E_IRQL_START+6,
    E_IRQ_UHC               = E_IRQL_START+7,
    E_IRQ_SPI_ARB_DUP_REG   = E_IRQL_START+8,
    E_IRQ_SPI_ARB_CHG_ADDR  = E_IRQL_START+3,
    E_IRQ_DEB               = E_IRQL_START+10,
    E_IRQ_UART2             = E_IRQL_START+11,
    E_IRQ_MICOM             = E_IRQL_START+12,
    E_IRQ_SBM               = E_IRQL_START+13,
    E_IRQ_EC_BRIDGE         = E_IRQL_START+14,
    E_IRQ_ECC_DERR          = E_IRQL_START+15,
    E_IRQL_END              = 15,


    E_IRQH_START            = 16,
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
    E_IRQ_DVBC              = E_IRQH_START+10,
    E_IRQ_VBI               = E_IRQH_START+12,
    E_IRQ_M4V               = E_IRQH_START+13,
    E_IRQ_MLINK             = E_IRQH_START+14,
    E_IRQ_HDMITX            = E_IRQH_START+15,
    E_IRQH_END              = 31,


    //FIQ
    E_FIQL_START            = 32,
    E_FIQ_INT_AEON_TO_8051  = E_FIQL_START+3,
    E_FIQ_INT_8051_TO_AEON  = E_FIQL_START+4,
    E_FIQ_INT_8051_TO_MIPS  = E_FIQL_START+5,
    E_FIQ_INT_MIPS_TO_8051  = E_FIQL_START+6,
    E_FIQ_INT_MIPS_TO_AEON  = E_FIQL_START+7,
    E_FIQ_INT_AEON_TO_MIPS  = E_FIQL_START+8,
    E_FIQL_END              = 47,

    E_FIQH_START            = 48,
    E_FIQ_VSYNC_VE4VBI      = E_FIQH_START+5,
    E_FIQ_FIELD_VE4VBI      = E_FIQH_START+6,
    E_FIQ_VSYN_COP          = E_FIQH_START+9,
    E_FIQ_VSYN_GOP0         = E_FIQH_START+11,
    E_FIQ_VSYN_GOP1         = E_FIQH_START+10,
    E_FIQ_IR                = E_FIQH_START+11,
    E_FIQ_DSP2UP            = E_FIQH_START+13,
    E_FIQ_DSP2MIPS          = E_FIQH_START+15,
    E_FIQH_END              = 63,


    E_IRQ_FIQ_ALL           = 0xFF //all IRQs & FIQs

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
#define IRQH_DMD                    (0x1 << (E_IRQ_DVBC     - E_IRQH_START)   )
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
#define FIQL_ALL                    0xFFFF //[15:0]


//FIQ High 16 bits
#define FIQH_DSP2UP                 (0x1 << (E_FIQ_DSP2UP - E_FIQH_START)  )
#define FIQH_VSYN_GOP1              (0x1 << (E_FIQ_VSYN_GOP1 - E_FIQH_START)  )
#define FIQH_VSYN_GOP0              (0x1 << (E_FIQ_VSYN_GOP0 - E_FIQH_START)  )
#define FIQH_IR                     (0x1 << (E_FIQ_IR - E_FIQH_START)   )
#define FIQH_VSYNC_VE4VBI           (0x1 << (E_FIQ_VSYNC_VE4VBI - E_FIQH_START)   )
#define FIQH_FIELD_VE4VBI           (0x1 << (E_FIQ_FIELD_VE4VBI - E_FIQH_START)   )
#define FIQH_DSP2MIPS               (0x1 << (E_FIQ_DSP2MIPS - E_FIQH_START)  )

#define FIQH_ALL                    0xFFFF //[15:0]



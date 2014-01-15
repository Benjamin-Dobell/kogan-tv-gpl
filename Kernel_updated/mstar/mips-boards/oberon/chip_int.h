/*******************************************************/
/*   THE IRQ AND FIQ ARE NOT COMPLETED.                */
/*   FOR EACH IP OWNER, PLEASE REVIEW IT BY YOURSELF   */
/*******************************************************/
#include "mhal_chiptop_reg.h"

#define MSTAR_INT_BASE  8
#define OBERONINT_END 128//64
//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------
#define MS_IRQ_MAX          (64) //32 IRQs + 32 FIQs
#define ENABLE_USB_PORT0
// Interrupt related
typedef enum
{
    //IRQ
    E_IRQL_START            = MSTAR_INT_BASE,

    E_IRQ_OPHY              = E_IRQL_START,
    E_IRQ_USB		     = E_IRQL_START,
    E_IRQ_UHC               = E_IRQL_START+1,		//tony for port0
    E_IRQ_SECOND_UHC        = E_IRQL_START+34,		//jc_db: not sure/* tony add for 2st EHCI */
    E_IRQ_MVD               = E_IRQL_START+2,
    E_IRQ_EMAC              = E_IRQL_START+3,
    E_IRQ_UART2             = E_IRQL_START+4,
    E_IRQ_UART1             = E_IRQL_START+5,
    E_IRQ_IIC1              = E_IRQL_START+6,
    E_IRQ_UART0             = E_IRQL_START+7,

    E_IRQ_DEB               = E_IRQL_START+10,
    E_IRQ_AESDMA            = E_IRQL_START+11,
    E_IRQ_CYTLIF            = E_IRQL_START+12,
    E_IRQ_SBM               = E_IRQL_START+13,
    E_IRQ_EC_BRIDGE         = E_IRQL_START+14,
    E_IRQ_ECC_DERR          = E_IRQL_START+15,
    E_IRQ_D2B               = E_IRQL_START+16,      //jc_db: not sure

    E_IRQL_END              = E_IRQ_D2B,

    E_IRQH_START            = E_IRQL_END+1,

    E_IRQ_TSP               = E_IRQH_START+0,
    E_IRQ_VE                = E_IRQH_START+1,
    E_IRQ_CIMAX2MCU         = E_IRQH_START+2,
    E_IRQ_MIPS_WR_ERR       = E_IRQH_START+3,
    E_IRQ_GOP               = E_IRQH_START+4,
    E_IRQ_PCM2MCU           = E_IRQH_START+5,
    E_IRQ_IIC0              = E_IRQH_START+6,
    E_IRQ_OTG               = E_IRQH_START+7,
#ifndef ENABLE_USB_PORT0    //tony
    E_IRQ_UHC              = E_IRQH_START+8,
#endif
    E_IRQ_PM                = E_IRQH_START+9,
    E_IRQ_RTC               = E_IRQH_START+9, // used as RTC as well
    E_IRQ_DVBC              = E_IRQH_START+10,
    E_IRQ_DESCMB_WR_ERR     = E_IRQH_START+11,
    E_IRQ_SVD               = E_IRQH_START+12,
    E_IRQ_JPD               = E_IRQH_START+13,
    E_IRQ_SDIO              = E_IRQH_START+14,
    E_IRQ_HDMITX            = E_IRQH_START+15,

    E_IRQH_END              = E_IRQ_HDMITX,


    //FIQ
    E_FIQL_START            = E_IRQH_END+1,

/*
    E_FIQ_INT               = E_FIQL_START+0,
*/

    E_FIQL_END              = E_FIQL_START,


    E_FIQH_START            = E_FIQL_START+1,

/*
    E_FIQ_VIVALDI_STR       = E_FIQH_START+0,
    E_FIQ_VIVALDI_PTS       = E_FIQH_START+1,

    E_FIQ_VE_VBI_F0         = E_FIQH_START+5,
    E_FIQ_VE_VBI_F1         = E_FIQH_START+6,
*/
    E_FIQ_IR                = E_FIQH_START+11,

    E_FIQ_DSP2UP            = E_FIQH_START+13,
    // E_FIQ_HDMI_EDGE         = E_FIQH_START+15,

    E_FIQH_END              = 63,


    E_IRQ_FIQ_ALL           = 0xFF //all IRQs & FIQs

} InterruptNum;


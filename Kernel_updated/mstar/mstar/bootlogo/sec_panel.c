/**
 * @file sec_panel.c
 * @brief Platform Specific API file
 */
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/delay.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/err.h>
#include "SdMisc.h"
#include "SdTypes.h"
#include "sec_panel_b1.h"
#include "sec_panel.h"
#include "mhal_xc_chip_config_x10.h"
#include "hwreg_pnl_x10.h"
#include "gpio_macro_x10.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define ENABLE_SSC 1

#if defined(PRJ_X10)
#define REG_TOP_BASE (0x3C00)
#define REG_PWM_BASE (0x4A00)
#define MMIO_ADDR_BASE 0xBF200000
#define MMIO_ADDR_FRC_BASE       0xBF600000
#else
#define REG_TOP_BASE (0x3C00)
#define REG_PWM_BASE (0x6400)
#define MMIO_ADDR_BASE 0xFD200000
#endif

#define _MOD_FRC_RIU_BASE 0xBF000000

#define _XC_RIU_BASE MMIO_ADDR_BASE
#define REG_SCALER_BASE             0x130000

#define READ_BYTE(_reg)             (*(volatile MS_U8*)(_reg))
#define READ_WORD(_reg)             (*(volatile MS_U16*)(_reg))
#define READ_LONG(_reg)             (*(volatile MS_U32*)(_reg))
#define WRITE_BYTE(_reg, _val)      { (*((volatile MS_U8*)(_reg))) = (MS_U8)(_val); }
#define WRITE_WORD(_reg, _val)      { (*((volatile MS_U16*)(_reg))) = (MS_U16)(_val); }
#define WRITE_LONG(_reg, _val)      { (*((volatile MS_U32*)(_reg))) = (MS_U32)(_val); }

#define WRITE_WORD_MASK(_reg, _val, _mask)  {(*((volatile MS_U16*)(_reg))) = ((*((volatile MS_U16*)(_reg))) & ~(_mask)) | ((MS_U16)(_val) & (_mask)); }
#define HAL_PWM_ReadByte(addr)             READ_BYTE((_gMIO_MapBase + REG_PWM_BASE) + ((addr)<<2))
#define HAL_PWM_Read2Byte(addr)              READ_WORD((_gMIO_MapBase + REG_PWM_BASE) + ((addr)<<2))

#define HAL_PWM_WriteByte(addr, val)          WRITE_BYTE((_gMIO_MapBase + REG_PWM_BASE) + ((addr)<<2), (val))
#define HAL_PWM_Write2Byte(addr, val)        WRITE_WORD((_gMIO_MapBase + REG_PWM_BASE) + ((addr)<<2), (val))
#define HAL_PWM_WriteRegBit(addr, val, mask) WRITE_WORD_MASK((_gMIO_MapBase + REG_PWM_BASE) + ((addr)<<2), (val), (mask))

#define HAL_TOP_ReadByte(addr)             READ_BYTE((_gMIO_MapBase + REG_TOP_BASE) + ((addr)<<2))
#define HAL_TOP_Read2Byte(addr)              READ_WORD((_gMIO_MapBase + REG_TOP_BASE) + ((addr)<<2))
#define HAL_TOP_WriteRegBit(addr, val, mask) WRITE_WORD_MASK((_gMIO_MapBase + REG_TOP_BASE) + ((addr)<<2), (val), (mask))

// Address bus of RIU is 16 bits.
#define RIU_READ_BYTE(addr)         ( READ_BYTE( _XC_RIU_BASE + (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( _XC_RIU_BASE + (addr) ) )
#define RIU_WRITE_BYTE(addr, val)   { WRITE_BYTE( _XC_RIU_BASE + (addr), val) }
#define RIU_WRITE_2BYTE(addr, val)  { WRITE_WORD( _XC_RIU_BASE + (addr), val) }


#if defined(PRJ_X10)
// Address bus of RIU is 16 bits.
#define FRC_RIU_READ_BYTE(addr)         ( READ_BYTE( _MOD_FRC_RIU_BASE + (addr) ) )
#define FRC_RIU_READ_2BYTE(addr)        ( READ_WORD( _MOD_FRC_RIU_BASE + (addr) ) )
#define FRC_RIU_WRITE_BYTE(addr, val)   { WRITE_BYTE( _MOD_FRC_RIU_BASE + (addr), val) }
#define FRC_RIU_WRITE_2BYTE(addr, val)  { WRITE_WORD( _MOD_FRC_RIU_BASE + (addr), val) }
#endif

#define MST_MACRO_START     do {
#define MST_MACRO_END       } while (0)

#define UNUSED( var )       ((void)(var))

#define MDrv_ReadRegBit( u32Reg, u8Mask )                                               \
        (RIU8[(u32Reg) * 2 - ((u32Reg) & 1)] & (u8Mask))

#define MDrv_WriteRegBit( u32Reg, bEnable, u8Mask )                                     \
    MST_MACRO_START                                                                     \
    MS_U32 u32Reg8 = ((u32Reg) * 2) - ((u32Reg) & 1);                                   \
    RIU8[u32Reg8] = (bEnable) ? (RIU8[u32Reg8] |  (u8Mask)) :                           \
                                (RIU8[u32Reg8] & ~(u8Mask));                            \
    MST_MACRO_END

#define MDrv_WriteByte( u32Reg, u8Val )                                                 \
    MST_MACRO_START                                                                     \
    RIU8[((u32Reg) * 2) - ((u32Reg) & 1)] = u8Val;                                      \
    MST_MACRO_END


#define SC_W2BYTE( u32Reg, u16Val)\
            ( { RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 , u16Val  ) ; } )

#define SC_R2BYTE( u32Reg ) \
            ( { RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1 ) ; } )

#define SC_W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) ); } )

#define SC_R4BYTE( u32Reg )\
            ( { RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 ) | (MS_U32)(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1 )) << 16; })

#define SC_R2BYTEMSK( u32Reg, u16mask)\
            ( { RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & (u16mask) ; } )

#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            ( { RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) ) ; })


/*
    R/W register forced to use 8 bit address, everytime need to r/w 2 bytes with mask

    32bit address     16 bit address        8 bit address
        0                   0                       0
        1                   x                       1
        2
        3
        4                   1                       2
        5                   x                       3
        6
        7
        8                   2                       4
        9                   x                       5
        A
        B                                                                                  */

// to read 0x2F03[3], please use R1BYTE(0x2F03, 3:3)
#define R1BYTE(u32Addr, u8mask)            \
    (READ_BYTE (gu32PnlRiuBaseAddr + ( (u32Addr)  << 1) - ( (u32Addr)  & 1)) & BMASK(u8mask))

// to write 0x2F02[4:3] with 2'b10, please use W1BYTE(0x2F02, 0x02, 4:3)
#define W1BYTE(u32Addr, u8Val, u8mask)     \
    (WRITE_BYTE(gu32PnlRiuBaseAddr + ( (u32Addr) << 1 ) - ( (u32Addr) & 1 ), (R1BYTE( (u32Addr), 7:0) & ~BMASK(u8mask)) | (BITS(u8mask, (  (u8Val) ) ) & BMASK(u8mask))))

// u32Addr must be 16bit aligned
#define R2BYTE(u32Addr, u16mask)            \
    (READ_WORD (gu32PnlRiuBaseAddr + ( (u32Addr)  << 1)) & BMASK(u16mask))

// u32Addr must be 16bit aligned
#define W2BYTE(u32Addr, u16Val, u16mask)    \
    (WRITE_WORD(gu32PnlRiuBaseAddr + ( (u32Addr)  << 1), (R2BYTE( (u32Addr) , 15:0) & ~BMASK(u16mask)) | (BITS(u16mask,   (u16Val) ) & BMASK(u16mask))))

#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1, ((u32Val) & 0x0000FFFF) ); \
                RIU_WRITE_2BYTE( ( (u32Reg) + 2) << 1 , (((u32Val) >> 16) & 0x0000FFFF)) ; } )

// u32Addr must be 16bit aligned
#define R2BYTE_TAB(u32Addr, u16mask)            \
    (READ_WORD (gu32PnlRiuBaseAddr + ( (u32Addr)  << 1)) & u16mask)

// u32Addr must be 16bit aligned
#define W2BYTE_TAB(u32Addr, u16Val, u16mask)    \
    (WRITE_WORD(gu32PnlRiuBaseAddr + ( (u32Addr)  << 1), (R2BYTE( (u32Addr) , 15:0) & ~u16mask) | (  (u16Val)  & u16mask)))

#define PM_R1BYTE(u32Addr, u8mask)            \
    (READ_BYTE (gu32PMRiuBaseAddr + ( (u32Addr)  << 1) - ( (u32Addr)  & 1)) & BMASK(u8mask))

#define PM_W1BYTE(u32Addr, u8Val, u8mask)     \
    (WRITE_BYTE(gu32PMRiuBaseAddr + ( (u32Addr) << 1 ) - ( (u32Addr) & 1 ), (PM_R1BYTE( (u32Addr), 7:0) & ~BMASK(u8mask)) | (BITS(u8mask, (  (u8Val) ) ) & BMASK(u8mask))))

// SCALER_BASE
#define XC_SC_BASE                  (0x2F00)
#define XC_SC_BK_SELECT             XC_SC_BASE

// HDGEN BASE
#define HDGEN_BASE                 (0x3000)
#define HDGEN_BK_SELECT             HDGEN_BASE

// MOD_BASE
#define XC_MOD_BASE                 (0x3200)
#define XC_MOD_BK_SELECT            XC_MOD_BASE

// LPLL BASE
#define XC_LPLL_BASE                (0x3100)

#define BK_SC(u32Addr)          (XC_SC_BASE + ((u32Addr) << 1))
#define BK_VOP(u32Addr)         (XC_SC_BASE + ((u32Addr) << 1))             ///< convert 16 bit address to 8 bit address
#define BK_S_VOP(u32Addr)       (XC_SC_BASE + ((u32Addr) << 1))             ///< convert 16 bit address to 8 bit address
#define BK_MOD(u32Addr)         (XC_MOD_BASE + ((u32Addr) << 1))            ///< convert 16 bit address to 8 bit address
#define BK_LPLL(u32Addr)        (XC_LPLL_BASE + ((u32Addr) << 1))           ///< convert 16 bit address to 8 bit address
#define BK_HDGEN(u32Addr)       (HDGEN_BASE + ((u32Addr) << 1))

#define L_BK_PAFRC(x)           ((XC_SC_BASE) | (((x) << 1)))
#define H_BK_PAFRC(x)           (((XC_SC_BASE) | (((x) << 1))) + 1)

#if defined(PRJ_X10P)
#define HAL_SUBBANK0    HAL_PWM_WriteByte(0,0)
#define HAL_SUBBANK1    HAL_PWM_WriteByte(0,1) /* PWM sub-bank */
#else
#define HAL_SUBBANK0
#define HAL_SUBBANK1
#endif

#if defined(PRJ_X10P)
#define PANEL_CONNECTOR_SWAP_PORT       1
#define PANEL_SWAP_LVDS_POL   1
#define PANEL_SWAP_LVDS_CH    0
#else
#define PANEL_CONNECTOR_SWAP_PORT   0
#define PANEL_SWAP_LVDS_POL         0
#define PANEL_SWAP_LVDS_CH          1
#endif

#define PANEL_PDP_10BIT             1
#define LVDS_PN_SWAP_L                  0x00
#define LVDS_PN_SWAP_H                  0x00
#define LVDS_MPLL_CLOCK_MHZ     432 // For crystal 24Mhz
#define LVDS_SPAN_FACTOR        131072

#if ENABLE_SSC
#define MIU_SSC_SPAN_DEFAULT            350
#define MIU_SSC_STEP_DEFAULT            200
#define MIU_SSC_SPAN_MAX                500
#define MIU_SSC_STEP_MAX                300
#define LVDS_SSC_SPAN_DEFAULT           350
#define LVDS_SSC_STEP_DEFAULT           200
#define LVDS_SSC_SPAN_MAX               500
#define LVDS_SSC_STEP_MAX               300
#endif

#define MAX_BACKLIGHT      100
#define PANEL_DE_VSTART    8

#define REG_PWM0_DBEN               (0x04)  //bit11
#define REG_PWM1_DBEN               (0x07)  //bit11
#define REG_PWM2_DBEN               (0x0A)  //bit11
#define REG_PWM3_DBEN               (0x0D)  //bit11
#define REG_PWM4_DBEN               (0x10)  //bit11

#if defined(PRJ_X10P)
#define RIU_READ_BYTE_1(addr)         ( READ_BYTE( 0xFD000000 + (addr) ) )
#define RIU_WRITE_BYTE_1(addr, val)   { WRITE_BYTE( 0xFD000000 + (addr), val) }
#else
#define RIU_READ_BYTE_1(addr)         ( READ_BYTE( 0xBF000000 + (addr) ) )
#define RIU_WRITE_BYTE_1(addr, val)   { WRITE_BYTE( 0xBF000000 + (addr), val) }
#endif

#define RIU_MACRO_START     do {
#define RIU_MACRO_END       } while (0)

#define MDrv_WriteByteMask( u32Reg, u8Val, u8Msk )                                      \
    RIU_MACRO_START                                                                     \
    RIU_WRITE_BYTE_1( (((u32Reg) <<1) - ((u32Reg) & 1)), (RIU_READ_BYTE_1((((u32Reg) <<1) - ((u32Reg) & 1))) & ~(u8Msk)) | ((u8Val) & (u8Msk)));                   \
    RIU_MACRO_END

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
PANEL_RESOLUTION_TYPE g_PNL_TypeSel ;
unsigned int panel_hstart = 0;
XC_INITMISC g_XC_Init_Misc;
//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#if defined(PRJ_X10)
static MS_U32 gu32PnlRiuBaseAddr = 0xBF200000;
static MS_U32 gu32PMRiuBaseAddr  = 0xBF000000;
#else
static MS_U32 gu32PnlRiuBaseAddr = 0xFD200000;
static MS_U32 gu32PMRiuBaseAddr  = 0xFD000000;
#endif

static MS_U32 _gMIO_MapBase = 0;
static MS_BOOL _gPWM_VDBen  = 0;
static PNL_OUTPUT_MODE   _eDrvPnlInitOptions = E_PNL_OUTPUT_CLK_DATA;

static MS_U8 _u8MOD_LVDS_Pair_Shift = TRUE;       ///< 0:default setting, LVDS pair Shift
static MS_U8 _u8MOD_LVDS_Pair_Type = 1;        ///< 0:default setting, LVDS data differential pair

static MS_S8 _usMOD_CALI_OFFSET = 0;        ///< MOD_REG(0x7D),[5:0]+ _usMOD_CALI_OFFSET
static MS_U8 _u8MOD_CALI_TARGET = 0x01;        ///< MOD_REG(0x7D),[10:9]00: 252mV ,01: 302mV ,10:403mV ,11:176mV
static MS_U8 _u8MOD_CALI_VALUE  = 0x15;     /// Final value
static PanelType  tPanelSXGA_AU17_EN05[] =
{
   {
        "SXGA_AU17_EN05",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        1,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB
        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;        //PANEL_EVEN_DATA_CURRENT    // even data current

        10,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        250,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        100,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        32,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        24,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        2,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        38,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        32+24,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH-1)
        2+38,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1280,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        1024,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1800,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1688,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        1664,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        2047,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        1066,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL
        1035,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        140,        //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        108,        //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        100,        //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0xA0,        //m_ucDimmingCtl
        0xFF,        //m_ucMaxPWMVal;
        0x50,         //m_ucMinPWMVal;

        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_4_3,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_8BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x277F44,
       0x1C3655,
       E_PNL_CHG_VTOTAL,
        0,
    },
};//

static PanelType  tPanelWXGA_AU20_T200XW02[] =
{
    {
        "WXGA_AU20_T200XW02",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        0^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30, //45,   //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        400,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        80,         //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        30, //20,   //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        20,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        4,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        34,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        20+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        4+34,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1366,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        768,        //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1722ul,     //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1560ul,//1430,  //WORD m_wPanelHTotal;          //PANEL_HTOTAL
        1414ul,     //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        822,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        806ul,      //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        789,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        88, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        76, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        65, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0xA0,       //m_ucDimmingCtl
        255,        //m_ucMaxPWMVal;
        0x50,//63,        //m_ucMinPWMVal;

        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_8BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x1E61E6,
       0x167109,
       E_PNL_CHG_VTOTAL,
        0,
    },
};//!<Panel Data 저장 구조체.(Multi Panel 대응을 위해 사용)

static PanelType  tPanelWXGA_SEC_LA32C45[] =
{
    {
        "WXGA_SEC",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        0^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30, //45,   //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        400,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        80,         //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        30, //20,   //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        20,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        4,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        34,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        20+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        4+34,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1366,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        768,        //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1722ul,     //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1560ul,//1430,  //WORD m_wPanelHTotal;          //PANEL_HTOTAL
        1414ul,     //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        822,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        806ul,      //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        789,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        88, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        76, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        65, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0xA0,       //m_ucDimmingCtl
        255,        //m_ucMaxPWMVal;
        0x50,//63,        //m_ucMinPWMVal;

        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x1A9040,    // std set = 0x340080, (0x340080 + 0x12000)/2
       0x197040,    // std set = 0x340080, (0x340080 - 0x12000)/2
       E_PNL_CHG_VTOTAL,
        0,
    },
};//!<Panel Data 저장 구조체.(Multi Panel 대응을 위해 사용)

static PanelType  tPanelWXGAP_CMO_M190A1[] =
{
    {

        "WXGAP_CMO_M190A1",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB
        PANEL_SWAP_LVDS_POL,
        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        500,            //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        150,            //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        30,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        104,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        24,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        3,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        6,          //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        104+24,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        3+6,            //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1440,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        900,            //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1920,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1760,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        1520,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        1050,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        950,            //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        910,            //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        112,     //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        100,     //DWORD m_dwPanelDCLK;          //PANEL_DCLK
        80,     //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0000,//0x0010,        //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x0000,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x68,       //m_ucDimmingCtl
        0xDD,                //m_ucMaxPWMVal;
        0x20,//63,        //m_ucMinPWMVal;

        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_8BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x315F15,
       0x2343EB,
       E_PNL_CHG_VTOTAL,
        0,
    },
};

static PanelType  tPanelWSXGA_AU22_M201EW01[] =
{
    {
        "WSXGA_AU22_M201EW01",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        500,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        120,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        30,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        16,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        2,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        8,          //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        16+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        2+8,        //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1680,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        1050,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        2000,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1840,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        1780,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        1195,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        1080,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        1060,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        (150),      //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        (119),      //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        (110),      //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;            //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;            //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0xA0,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x50,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue

       TI_8BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x23E80F,
       0x1A54D8,
       E_PNL_CHG_VTOTAL,
        0,
    },
};

static PanelType  tPanelFullHD_CMO216_H1L01[] =
{
    {
        "FullHD_CMO216_H1L01",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE
        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        32,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        80,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        8,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        16,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        32+80,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        8+16,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1920,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        1080,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        2360,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        2200,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        2020,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        1200,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        1130,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        1090,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        (164),      //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        (150),      //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        (120),      //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
        (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
        TI_8BIT_MODE,               //8bit ti bit mode

        OUTPUT_10BIT_MODE,          //10bit ti bit mode
        0,          //   PANEL_SWAP_ODD_RG
        0,          //   PANEL_SWAP_EVEN_RG
        0,          //   PANEL_SWAP_ODD_GB
        0,          //   PANEL_SWAP_EVEN_GB
        1,          //   double clock
        0x20EA0E,
        0x181568,
        E_PNL_CHG_VTOTAL,
        0,
    },
};

static PanelType  tPanelFULLHD_SEC32_LE32A[] =
{
    {
        "SEC_LE32A",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,
        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        32,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        80,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        8,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        16,          //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        32+80,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        8+16,            //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1920,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        1080,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        2360,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        2200,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        2020,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        1200,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        1130,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        1090,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        (164),      //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        (150),      //DWORD m_dwPanelDCLK;          //PANEL_DCLK
        (136),      //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //0x0010,       //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,   //0x87,     //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,    //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x1ABACE,
       0x1A3ACE,
       E_PNL_CHG_VTOTAL,
       1,
    },
};

static PanelType  tPanelFULLHD_SEC32_CEA[] =
{
    {
        "SEC_CEA",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,
        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        44,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        148,        //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        5,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        15,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        44+148,     //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        5+15,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1920,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        1080,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        2360,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        2200,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        2020,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        1200,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        1125,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        1090,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        (164),      //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        (150),      //DWORD m_dwPanelDCLK;          //PANEL_DCLK
        (136),      //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //0x0010,       //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,   //0x87,     //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,    //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x1D0AC1,
       0x181568,
       E_PNL_CHG_VTOTAL,
        0,
    },
};

static PanelType  tPanelHD_SEC32_VESA[] =
{
    {
        "SEC_VESA",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        0^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30, //45,   //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        400,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        80,         //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        30, //20,   //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        112,        //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        255,//256   //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        6,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        18,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        112+256,    //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        6+18,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1360,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        768,        //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1954ul,     //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1792ul,     //WORD m_wPanelHTotal;          //PANEL_HTOTAL
        1646ul,     //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        811,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        795ul,      //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        778,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        97, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        85, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        74, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0xA0,       //m_ucDimmingCtl
        255,        //m_ucMaxPWMVal;
        0x50,//63,        //m_ucMinPWMVal;

        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x1E61E6,
       0x167109,
       E_PNL_CHG_VTOTAL,
        0,
    },
};


static PanelType  tPanelWSXGA_SEC_LTM200KT03[] =
{
    {
        "LTM200K",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,
        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        32,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        80,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        8,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        16,          //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        32+80,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        8+16,            //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1600,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        900,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        2300,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        2112,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        1900,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        955,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        934,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        906,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        (132),      //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        (118),      //DWORD m_dwPanelDCLK;          //PANEL_DCLK
        (97),      //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //0x0010,       //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,   //0x87,     //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,    //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_8BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x1D0AC1,
       0x181568,
       E_PNL_CHG_VTOTAL,
        0,
    },
};

static PanelType  tPanelFULLHD_SEC32_LTM230H[] =
{
    {
        "SEC_LE230H",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,
        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        32,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        80,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        8,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        16,          //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        32+80,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        8+16,            //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1920,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        1080,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        2360,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        2200,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        2020,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        1200,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        1130,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        1090,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        (164),      //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        (150),      //DWORD m_dwPanelDCLK;          //PANEL_DCLK
        (136),      //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //0x0010,       //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,   //0x87,     //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,    //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_8BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x1D0AC1,
       0x181568,
       E_PNL_CHG_VTOTAL,
        0,
    },
};

static PanelType  tPanelWXGA_SEC_M185B1[] =
{
    {
        "WXGA_SEC",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK
        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT
        0^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE
        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB
        PANEL_SWAP_LVDS_POL,
        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE
        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC
        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current
        30, //45,   //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        400,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        80,         //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        30, //20,   //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power
        20,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH
        4,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        34,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH
        20+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        4+34,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)
        1366,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        768,        //WORD m_wPanelHeight;              //PANEL_HEIGHT
        1936ul,     //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1560ul,//1430,  //WORD m_wPanelHTotal;          //PANEL_HTOTAL
        1446ul,     //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL
        888,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        806ul,      //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        778,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL
        95, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        76, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        50, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK
        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0xA0,       //m_ucDimmingCtl
        255,        //m_ucMaxPWMVal;
        0x50,//63,        //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_8BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x1E61E6,
       0x167109,
       E_PNL_CHG_VTOTAL,
        0,
    },
};//!<Panel Data ?? ???.(Multi Panel ??? ?? ??)

static PanelType  tPanelFULLHD_SEC_FHD_R[] =
{
    {
        "SEC_FHD_R",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,
        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        44,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        106,        //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        4,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        16,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        44+106,     //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        4+16,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1920,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        1080,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        2360,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        2140,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        2020,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        1200,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        1110,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        1090,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        (164),      //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        (142),      //DWORD m_dwPanelDCLK;          //PANEL_DCLK
        (136),      //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //0x0010,       //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,   //0x87,     //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,    //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x1D0AC1,
       0x181568,
       E_PNL_CHG_HTOTAL,
        0,
    },
};

static PanelType  tPanel_SEC_PDP_1080P[] =
{
    {
        "SEC_PDP_1080P",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_PDPLVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        1,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,
        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        20,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        192,        //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        5,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        20,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        20+192,     //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        5+20,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1920,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        1080,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        2360,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        2200,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        2020,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        1200,       //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        1125,       //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        1089,       //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        (152),      //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        (148),      //DWORD m_dwPanelDCLK;          //PANEL_DCLK
        (144),      //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //0x0010,       //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,   //0x87,     //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,    //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       1,          //   double clock
       0x1D0AC1,
       0x181568,
       E_PNL_CHG_HTOTAL,
       0,
    },
};

static PanelType  tPanel_SEC_PDP_WXGA[] =
{
    {
        "SEC_PDP_WXGA",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        0^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30, //45,   //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        400,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        80,         //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        30, //20,   //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        10,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        76,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        6,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        30,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        10+76,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        6+30,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1365,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        768,        //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1722ul,     //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1504ul,     //WORD m_wPanelHTotal;          //PANEL_HTOTAL
        1414ul,     //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        895,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        820ul,      //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        794,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        78,         //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        74,         //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        71,         //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0xA0,       //m_ucDimmingCtl
        255,        //m_ucMaxPWMVal;
        0x50,//63,        //m_ucMinPWMVal;

        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x1A9040,    // std set = 0x340080, (0x340080 + 0x12000)/2
       0x197040,    // std set = 0x340080, (0x340080 - 0x12000)/2
       E_PNL_CHG_HTOTAL,
       0,
    },
};//!<Panel Data 저장 구조체.(Multi Panel 대응을 위해 사용)

static PanelType  tPanel_SEC_PDP_XGA[] =
{
    {
        "SEC_PDP_XGA",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        0^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
        0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30, //45,   //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        400,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        80,         //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        30, //20,   //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        10,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        76,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        6,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        30,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        10+76,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        6+30,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1024,       //WORD m_wPanelWidth;               //PANEL_WIDTH
        768,        //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1722ul,     //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1504ul,     //WORD m_wPanelHTotal;          //PANEL_HTOTAL
        1414ul,     //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        895,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        820ul,      //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        794,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        78,         //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        74,         //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        71,         //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0xA0,       //m_ucDimmingCtl
        255,        //m_ucMaxPWMVal;
        0x50,//63,        //m_ucMinPWMVal;

        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,               //8bit ti bit mode
       OUTPUT_10BIT_MODE,          //10bit ti bit mode
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x1A9040,    // std set = 0x340080, (0x340080 + 0x12000)/2
       0x197040,    // std set = 0x340080, (0x340080 - 0x12000)/2
       E_PNL_CHG_HTOTAL,
       0,
    },
};//!<Panel Data 저장 구조체.(Multi Panel 대응을 위해 사용)

static PanelType * tPanelIndexTbl[] =
{
    tPanelFULLHD_SEC32_LE32A,   // 0 LE32A_FULLHD
    tPanelFULLHD_SEC32_CEA,     // 1 CEA (FHD)
    tPanelHD_SEC32_VESA,        // 2 VESA (HD)
    tPanelSXGA_AU17_EN05,       // 3
    tPanelWXGA_AU20_T200XW02,   // 4
    tPanelWXGAP_CMO_M190A1,     // 5  1440*900  //CHR_060912_1 Resolution 추가
    tPanelWSXGA_AU22_M201EW01,  // 6  1680*1050
    tPanelFullHD_CMO216_H1L01,  // 7  1920x1080
    tPanelFullHD_CMO216_H1L01,  // 8  For auto build 1920x1080
    tPanelWXGA_SEC_LA32C45,     // 9  1366x768
    tPanelWSXGA_SEC_LTM200KT03, // 10 1600x900
    tPanelFULLHD_SEC32_LTM230H, // 11 1920x1080
    tPanelWXGA_SEC_M185B1,      // 12 1366x768 // MFM
    tPanelFULLHD_SEC_FHD_R,     // 13 1920x1080 chnage_Htt only
    tPanel_SEC_PDP_1080P,       // 14 1920x1080 PDP
    tPanel_SEC_PDP_WXGA,        // 15 1920x1080 PDP
    tPanel_SEC_PDP_XGA,         // 16 1920x1080 PDP
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void MHal_BD_LVDS_Output_Type(MS_U16 Type)
{
    switch(Type)
    {
        case LVDS_DUAL_OUTPUT_SPECIAL:  // 1
            _u8MOD_LVDS_Pair_Shift = TRUE;
            _u8MOD_LVDS_Pair_Type = 1; //1;
        break;
        default:
        case LVDS_SINGLE_OUTPUT_A:       // 2
            _u8MOD_LVDS_Pair_Shift = FALSE;
            _u8MOD_LVDS_Pair_Type = 2;// 2;
        break;
        case LVDS_SINGLE_OUTPUT_B:       // 3
            _u8MOD_LVDS_Pair_Shift = FALSE;
            _u8MOD_LVDS_Pair_Type = 3; //3;
        break;
        case LVDS_OUTPUT_User:           // 4
            _u8MOD_LVDS_Pair_Shift = TRUE;
            _u8MOD_LVDS_Pair_Type = 4;
        break;
    }
}

PNL_InitData_ADV_st _stPnlInitData_adv;

void MDrv_Init_PNLAdvInfo_beforePNLinit(p_PNL_InitData_ADV_st pInitInfo)
{
//    p_PNL_InitData_ADV_st pinitinfo = (p_PNL_InitData_ADV_st) pInitInfo;

    _stPnlInitData_adv.bFRC_En              = pInitInfo->bFRC_En;
    _stPnlInitData_adv.u8FRC_LPLL_Mode      = pInitInfo->u8FRC_LPLL_Mode;
    _stPnlInitData_adv.u16MOD_SwingLevel    = pInitInfo->u16MOD_SwingLevel;
    _stPnlInitData_adv.u16MOD_SwingCLK      = pInitInfo->u16MOD_SwingCLK;
    _stPnlInitData_adv.bdither6Bits         = pInitInfo->bdither6Bits;
    _stPnlInitData_adv.blvdsShiftPair       = pInitInfo->blvdsShiftPair;
    _stPnlInitData_adv.blvdsSwapPair        = pInitInfo->blvdsSwapPair;
    _stPnlInitData_adv.u8lvdsSwapCh         = pInitInfo->u8lvdsSwapCh;
    _stPnlInitData_adv.bepiLRSwap           = pInitInfo->bepiLRSwap;
    _stPnlInitData_adv.bepiLMirror          = pInitInfo->bepiLMirror;
    _stPnlInitData_adv.bepiRMirror          = pInitInfo->bepiRMirror;

    //Debug message
#if 0
    printf("\n\n##[%s][%s]%d\n", __FILE__, __FUNCTION__, __LINE__);
    printf("xxxbFRC_En = %s\n",_stPnlInitData_adv.bFRC_En?"TRUE":"FALSE");
    printf("xxxbFRC_LPLL = %d\n",_stPnlInitData_adv.u8FRC_LPLL_Mode);
    printf("xxxblvdsSwapPair = %s\n",_stPnlInitData_adv.blvdsSwapPair?"TRUE":"FALSE");
    printf("xxxu8lvdsSwapCh = %x\n",_stPnlInitData_adv.u8lvdsSwapCh);
    printf("xxxblvdsShiftPair = %s\n",_stPnlInitData_adv.blvdsShiftPair?"TRUE":"FALSE");
#endif

}

void MHal_FRC_MOD_SetPowerdown(MS_U16 u16FuncEnable, MS_U16 u16FuncDisable)
{
    MS_U16 u16value=0;
    MS_U16 u16mask=0;

    //combine value and mask.
    u16value = (u16FuncEnable&(~u16FuncDisable));
    u16mask  = (u16FuncEnable|u16FuncDisable)&(FRC_MOD_PD_MODATOP|FRC_MOD_PD_REG_A|FRC_MOD_PD_REG_B);

    MOD_W2BYTEMSK(REG_MOD_BK00_28_L, u16value, u16mask); // REG_3250
}

void MHal_FRC_MOD_SetClkEn(MS_BOOL bEnable)
{
    MOD_W2BYTEMSK(REG_MOD_BK00_27_L, bEnable?0x3F:0x00, 0x3F); // REG_324E
}

void MHal_FRC_MOD_SetGcrTest_Ch(MS_BOOL bEnable)
{
    MOD_W2BYTEMSK(REG_MOD_BK00_38_L, bEnable?BIT(0):0x00, BIT(0)); // REG_3270
}

void MHal_FRC_MOD_SetOutputCfg(MS_U16 output_cfg_10, MS_U16 output_cfg_11, MS_U16 output_cfg_12, MS_U16 output_cfg_13)
{
    //In A5, Only use 0x10~0x12
    // UNUSED(output_cfg_13);

    MHal_FRC_MOD_SetGcrTest_Ch(ENABLE);

    MOD_W2BYTE(REG_MOD_BK00_10_L, output_cfg_10); // REG_3220
    //(*((volatile MS_U16*)(0xBF606440))) = (MS_U16)(0x5555);
    MOD_W2BYTE(REG_MOD_BK00_11_L, output_cfg_11); // REG_3222
    MOD_W2BYTE(REG_MOD_BK00_12_L, output_cfg_12); // REG_3224
//    MOD_W2BYTEMSK(REG_MOD_BK00_13_L, output_cfg_13, 0xFF); // REG_3226

    MHal_FRC_MOD_SetGcrTest_Ch(DISABLE);
}

void MHal_FRC_MOD_SetExtGpioEn(MS_U16 ext_en_18, MS_U16 ext_en_19)
{
    MOD_W2BYTE(REG_MOD_BK00_18_L, ext_en_18); // REG_3230
    MOD_W2BYTE(REG_MOD_BK00_19_L, ext_en_19); // REG_3232
}

void MHal_FRC_MOD_SetTtlPairEn(MS_U16 pair_en_1a, MS_U16 pair_en_1b)
{
    MOD_W2BYTE(REG_MOD_BK00_1A_L, pair_en_1a); // REG_3234
    MOD_W2BYTE(REG_MOD_BK00_1B_L, pair_en_1b); // REG_3236
}

void MHal_FRC_MOD_SetGpoOez(MS_U16 gpo_oez_14, MS_U16 gpo_oez_15, MS_U16 gpo_oez_16, MS_U16 gpo_oez_17)
{
    MOD_W2BYTE(REG_MOD_BK00_14_L, gpo_oez_14); // REG_3228
    MOD_W2BYTE(REG_MOD_BK00_15_L, gpo_oez_15); // REG_322A
    MOD_W2BYTE(REG_MOD_BK00_16_L, gpo_oez_16); // REG_322C
    MOD_W2BYTE(REG_MOD_BK00_17_L, gpo_oez_17); // REG_322E
}


/**
*   @brief: Power On MOD. but not mutex protected
*
*/
void MHal_Output_LVDS_Pair_Setting(MS_U8 Type);

MS_U8 MHal_MOD_PowerOn(MS_BOOL bEn, MS_U8 u8LPLL_Type)
{
#if defined(PRJ_X10)
    if (bEn)
    {
        MHal_FRC_MOD_SetPowerdown(0, FRC_MOD_PD_MODATOP);
        MHal_FRC_MOD_SetClkEn(TRUE);
        MHal_Output_LVDS_Pair_Setting( LVDS_OUTPUT_User);

        if (u8LPLL_Type == E_PNL_TYPE_LVDS)
        {
            MHal_FRC_MOD_SetExtGpioEn(0xFFFF, 0x3FF);
            MHal_FRC_MOD_SetTtlPairEn(0, 0);
        }

        if (u8LPLL_Type == E_PNL_TYPE_LVDS)
        {
            MHal_FRC_MOD_SetGpoOez(0, 0, 0, 0);
        }
        else
        {
            MHal_FRC_MOD_SetGpoOez(0xffff, 0xffff, 0xffff, 0xffff);
        }
    }
    else
    {
        MHal_FRC_MOD_SetPowerdown(FRC_MOD_PD_MODATOP, 0);
        MHal_FRC_MOD_SetClkEn(FALSE);
        MHal_FRC_MOD_SetOutputCfg(0x0000, 0x0000, 0x0000, 0x00);


        if (u8LPLL_Type == E_PNL_TYPE_LVDS)
        {
            MHal_FRC_MOD_SetExtGpioEn(0, 0);
        }
        MHal_FRC_MOD_SetGpoOez(0, 0, 0, 0);
    }
#else
    if( bEn )
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_37_L, 0x0000, 0x0100);

        //analog MOD power down. 1: power down, 0: power up
        // For Mod2 no output signel
        ///////////////////////////////////////////////////

        //2. Power on MOD (current and regulator)
        MOD_W2BYTEMSK(REG_MOD_BK00_78_L, 0x0000 , 0x0081 );

        // 3. 4. 5.
        MHal_Output_LVDS_Pair_Setting(LVDS_OUTPUT_User);

        //enable ib, enable ck
        MOD_W2BYTEMSK(REG_MOD_BK00_77_L, 0x0003, 0x0003);

        // clock gen of dot-mini
        if(u8LPLL_Type == E_PNL_TYPE_MINILVDS)
        {
            MOD_W2BYTE(REG_MOD_BK00_20_L, 0x4400);
        }
        else
        {
            MOD_W2BYTE(REG_MOD_BK00_20_L, 0x0000);
        }
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_37_L, 0x0100, 0x0100);
        if(u8LPLL_Type !=E_PNL_TYPE_MINILVDS)
        {
            MOD_W2BYTEMSK(REG_MOD_BK00_78_L, 0x0000, 0x0001);                              //analog MOD power down. 1: power down, 0: power up
        }

        MOD_W2BYTEMSK(REG_MOD_BK00_77_L, 0x0000, 0x0003);                           //enable ib, enable ck

        // clock gen of dot-mini
        MOD_W2BYTE(REG_MOD_BK00_20_L, 0x1100);
    }
#endif
    return 1;
}

//-------------------------------------------------------------------------------------------------
//  Export Functions
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/// General Write Byte Mask Function
/// @param  u32Reg                \b IN: which value contain xc sub bank and address
/// @param  u8Val                \b IN: setting value
/// @param  u8Msk                \b IN: decide which bit is valied
//-------------------------------------------------------------------------------------------------
void    MApi_XC_WriteByteMask(MS_U32 u32Reg, MS_U8 u8Val, MS_U8 u8Msk)
{
    MDrv_WriteByteMask( u32Reg, u8Val, u8Msk );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name: HAL_PWM_SetIOMapBase
/// @brief \b Function   \b Description: Set IO Map base
/// @param <IN>          \b None :
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_SetIOMapBase(MS_U32 u32Base)
{
    _gMIO_MapBase = u32Base;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name: HAL_PWM_Init
/// @brief \b Function   \b Description: Initial PWM
/// @param <IN>          \b None :
/// @param <OUT>       \b None :
/// @param <RET>        \b MS_BOOL :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_PWM_Init(void)
{
    MS_BOOL ret = FALSE;

#if defined(PRJ_X10) || defined(PRJ_X10P)
    WRITE_WORD_MASK((_gMIO_MapBase + REG_TOP_BASE) + (0x64<<2),0xFFFF, BIT12); //Pad_PWM3
#endif

    ret = TRUE;
    return ret;
}


//-------------------------------------------------------------------------------------------------
/// Description: Initial PWM driver
/// @param void
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Init(void)
{
#if defined(PRJ_X10)
    HAL_PWM_SetIOMapBase(MMIO_ADDR_FRC_BASE);
#else
    HAL_PWM_SetIOMapBase(MMIO_ADDR_BASE);
#endif

    /* Set All pad output and Is PWM. But it still needs to be enable */
    if( !HAL_PWM_Init() )
    {
        printk("PWM Hal Initial Fail\n");
        return E_PWM_FAIL;
    }
    return E_PWM_OK;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_PWM_Oen
/// @brief \b Function  \b Description: Switch PWM PAD as Output or Input
/// @param <IN>         \b MS_U16 : index
/// @param <IN>         \b MS_BOOL : letch, 1 for Input; 0 for Output
/// @param <OUT>      \b None :
/// @param <RET>       \b MS_BOOL :
/// @param <GLOBAL>  \b None :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_PWM_Oen(PWM_ChNum index, MS_BOOL letch)
{

    MS_BOOL bMode = letch? 0: 1;

    HAL_SUBBANK1;

    switch(index)
    {
        case E_PWM_CH0:
            HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(0:0,letch), BMASK(0:0));
            HAL_TOP_WriteRegBit(REG_PWM_MODE,BITS(2:2,bMode), BMASK(2:2));
            break;
        case E_PWM_CH1:
            HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(1:1,letch), BMASK(1:1));
            HAL_TOP_WriteRegBit(REG_PWM_MODE,BITS(6:6,bMode), BMASK(6:6));
            break;
        case E_PWM_CH2:
            HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(2:2,letch), BMASK(2:2));
            HAL_TOP_WriteRegBit(REG_PWM_MODE,BITS(7:7,bMode), BMASK(7:7));
            break;
        case E_PWM_CH3:
            HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(3:3,letch), BMASK(3:3));
            HAL_TOP_WriteRegBit(REG_PWM_MODE,BITS(12:12,bMode), BMASK(12:12));
            break;
        case E_PWM_CH4:
            HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(4:4,letch), BMASK(4:4));
            HAL_TOP_WriteRegBit(REG_PWM_MODE,BITS(13:13,bMode), BMASK(13:13));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
			printk("[Utopia] The PWM%d is not support\n", (int)index);
                   UNUSED(letch);
            break;
        default:
            break;
    }
    HAL_SUBBANK0;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Description: IOutput enable_bar of PWM pads
/// @param u8IndexPWM    \b which pwm is setting
/// @param bOenPWM    \b True/False for enable/disable
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Oen(PWM_ChNum u8IndexPWM, MS_BOOL bOenPWM)
{
    PWM_Result ret = E_PWM_OK;
    // printk("MDrv_PWM_Oen Start\n");

    if(!(HAL_PWM_Oen(u8IndexPWM, bOenPWM)))
    {
        ret = E_PWM_FAIL;
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_Period
/// @brief \b Function   \b Description : Set the period of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b MS_U32 : the 18-bit Period value
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_Period(PWM_ChNum index, MS_U32 u32PeriodPWM)
{
    MS_U16  Period_L, Period_H;
    Period_L = (MS_U16)u32PeriodPWM;
    Period_H = (MS_U16)(u32PeriodPWM >> 16);

    HAL_SUBBANK1;

    /* the Period capability is restricted to ONLY 18-bit */
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_Write2Byte(REG_PWM0_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM0_PERIOD_EXT,BITS(1:0,Period_H),BMASK(1:0));
            break;
        case E_PWM_CH1:
            HAL_PWM_Write2Byte(REG_PWM1_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM1_PERIOD_EXT,BITS(3:2,Period_H),BMASK(3:2));
            break;
        case E_PWM_CH2:
            HAL_PWM_Write2Byte(REG_PWM2_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM2_PERIOD_EXT,BITS(5:4,Period_H),BMASK(5:4));
            break;
        case E_PWM_CH3:
            HAL_PWM_Write2Byte(REG_PWM3_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM3_PERIOD_EXT,BITS(7:6,Period_H),BMASK(7:6));
            break;
        case E_PWM_CH4:
            HAL_PWM_Write2Byte(REG_PWM4_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM4_PERIOD_EXT,BITS(9:8,Period_H),BMASK(9:8));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(Period_L);
            UNUSED(Period_H);
            break;
        default:
            break;
    }
    HAL_SUBBANK0;

}

//-------------------------------------------------------------------------------------------------
/// Description: Set the period of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16PeriodPWM    \b the 18-bit period value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Period(PWM_ChNum u8IndexPWM, MS_U32 u32PeriodPWM)
{
    PWM_Result ret = E_PWM_FAIL;

    do{
        HAL_PWM_Period(u8IndexPWM, u32PeriodPWM);
        ret = E_PWM_OK;
    }while(0);

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_DutyCycle
/// @brief \b Function   \b Description : Set the Duty of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b MS_U32 : the 18-bit Duty value
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_DutyCycle(PWM_ChNum index, MS_U32 u32DutyPWM)
{
    MS_U16  Duty_L, Duty_H;

    Duty_L = (MS_U16)u32DutyPWM;
    Duty_H = (MS_U8)(u32DutyPWM >> 16);

    HAL_SUBBANK1;
    /* the Duty capability is restricted to ONLY 10-bit */
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_Write2Byte(REG_PWM0_DUTY, Duty_L);
            HAL_PWM_WriteRegBit(REG_PWM0_DUTY_EXT,BITS(1:0,Duty_H),BMASK(1:0));
            break;
        case E_PWM_CH1:
            HAL_PWM_Write2Byte(REG_PWM1_DUTY, Duty_L);
            HAL_PWM_WriteRegBit(REG_PWM1_DUTY_EXT,BITS(3:2,Duty_H),BMASK(3:2));
            break;
        case E_PWM_CH2:
            HAL_PWM_Write2Byte(REG_PWM2_DUTY, Duty_L);
	    HAL_PWM_WriteRegBit(REG_PWM2_DUTY_EXT,BITS(5:4,Duty_H),BMASK(5:4));
            break;
        case E_PWM_CH3:
            HAL_PWM_Write2Byte(REG_PWM3_DUTY, Duty_L);
            HAL_PWM_WriteRegBit(REG_PWM3_DUTY_EXT,BITS(7:6,Duty_H),BMASK(7:6));
            break;
        case E_PWM_CH4:
            HAL_PWM_Write2Byte(REG_PWM4_DUTY, Duty_L);
            HAL_PWM_WriteRegBit(REG_PWM4_DUTY_EXT,BITS(9:8,Duty_H),BMASK(9:8));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(Duty_L);
            UNUSED(Duty_H);
            break;
        default:
            break;
    }
    HAL_SUBBANK0;

}

//-------------------------------------------------------------------------------------------------
/// Description: Set the Duty of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16DutyPWM    \b the 18-bit Duty value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_DutyCycle(PWM_ChNum u8IndexPWM, MS_U32 u32DutyPWM)
{
    PWM_Result ret = E_PWM_FAIL;

    do{
        HAL_PWM_DutyCycle(u8IndexPWM, u32DutyPWM);
        ret = E_PWM_OK;
    }while(0);

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_Div
/// @brief \b Function   \b Description : Set the Div of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b MS_U16 : the 16-bit Div value
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_Div(PWM_ChNum index, MS_U16 u16DivPWM)
{
	MS_U8 u8DivPWM[2] = {0};

	u8DivPWM[0] = (MS_U8)(u16DivPWM&0xFF);
	u8DivPWM[1] = (MS_U8)(u16DivPWM >> 8);

    HAL_SUBBANK1;
    /* the Div capability is restricted to ONLY 16-bit */
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteByte(REG_PWM0_DIV,     u8DivPWM[0]);
            HAL_PWM_WriteByte(REG_PWM0_DIV_EXT, u8DivPWM[1]);
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteByte(REG_PWM1_DIV,     u8DivPWM[0]);
            HAL_PWM_WriteRegBit(REG_PWM1_DIV_EXT,u8DivPWM[1],BMASK(15:8));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteByte(REG_PWM2_DIV,     u8DivPWM[0]);
            HAL_PWM_WriteByte(REG_PWM2_DIV_EXT, u8DivPWM[1]);
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteByte(REG_PWM3_DIV,     u8DivPWM[0]);
            HAL_PWM_WriteRegBit(REG_PWM3_DIV_EXT,u8DivPWM[1],BMASK(15:8));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteByte(REG_PWM4_DIV, 	u8DivPWM[0]);
            HAL_PWM_WriteByte(REG_PWM4_DIV_EXT, u8DivPWM[1]);
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(u16DivPWM);
            break;
        default:
            break;
    }
    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_Polarity
/// @brief \b Function   \b Description : Set the Polarity of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b MS_BOOL : 1 for enable; 0 for disable
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_Polarity(PWM_ChNum index, MS_BOOL bPolPWM)
{
    HAL_SUBBANK1;
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(bPolPWM);
            break;
        default:
            break;
    }
    HAL_SUBBANK0;
}

//-------------------------------------------------------------------------------------------------
/// Description: Set the Div of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16DivPWM    \b the 16-bit div value
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Div(PWM_ChNum u8IndexPWM, MS_U16 u16DivPWM)
{
    HAL_PWM_Div(u8IndexPWM, u16DivPWM);
}

//-------------------------------------------------------------------------------------------------
/// Description:  Set the Polarity of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bPolPWM    \b True/False for Inverse/Non-inverse
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Polarity(PWM_ChNum u8IndexPWM, MS_BOOL bPolPWM)
{
    HAL_PWM_Polarity(u8IndexPWM, bPolPWM);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_VDBen
/// @brief \b Function   \b Description : Set the Vsync Double buffer of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b MS_BOOL : 1 for enable; 0 for disable
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_VDBen(PWM_ChNum index, MS_BOOL bVdbenPWM)
{
    _gPWM_VDBen = bVdbenPWM;
    HAL_SUBBANK1;
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(bVdbenPWM);
            break;
        default:
            break;
    }
    HAL_SUBBANK0;
}

//-------------------------------------------------------------------------------------------------
/// Description: Set the Vsync Double buffer of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bVdbenPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Vdben(PWM_ChNum u8IndexPWM, MS_BOOL bVdbenPWM)
{
    HAL_PWM_VDBen(u8IndexPWM, bVdbenPWM);
}

void MHal_PQ_Clock_Gen_For_Gamma(void)
{
#if defined(PRJ_X10)
    PM_W1BYTE(0x100B00+0xA4,0x00,0:0);
#else
    PM_W1BYTE(0x100B00+0xA4,0x00,2:0);
#endif
}

PNL_Result MDrv_PNL_SetSSC(MS_U16 u16Fmodulation, MS_U16 u16Rdeviation, MS_BOOL bEnable)
{
    MS_U16 u16Span;
    MS_U16 u16Step;
    MS_U32 u32PLL_SET;/// = MDrv_Read3Byte(L_BK_LPLL(0x0F));

    u32PLL_SET = ((MS_U32)R1BYTE( BK_LPLL(0x10), 7:0 ) ) << 16 | (MS_U32)R2BYTE( BK_LPLL(0x0F), 15:0 );

    // Set SPAN
    if(u16Fmodulation < 200 || u16Fmodulation > 400)
        u16Fmodulation = 300;
    u16Span =( ( (((MS_U32)LVDS_MPLL_CLOCK_MHZ*LVDS_SPAN_FACTOR ) / (u16Fmodulation) ) * 10000) / ((MS_U32)u32PLL_SET) ) ;

    // Set STEP
    if(u16Rdeviation > 300)
        u16Rdeviation = 300;
    u16Step = ((MS_U32)u32PLL_SET*u16Rdeviation) / ((MS_U32)u16Span*10000);

    W2BYTE(BK_LPLL(0x17), u16Step & 0x03FF, 15:0);// LPLL_STEP
    W2BYTE(BK_LPLL(0x18), u16Span & 0x3FFF, 15:0);// LPLL_SPAN
    W1BYTE((BK_LPLL(0x0D)+1), bEnable, 3:3); // Enable ssc


    return E_PNL_OK;
}

PNL_Result MDrv_PNL_PreInit(PNL_OUTPUT_MODE eParam)
{
    if ( eParam == E_PNL_OUTPUT_MAX )
        return E_PNL_FAIL;

    _eDrvPnlInitOptions = eParam;
    return E_PNL_OK;
}

//////////////////////////////////////////////////////////////////////
/**
*
* @brief  Give some options for panel init.
*
* @param[in] pSelPanelType  a given panel type choosed by user.
*
*/
MS_BOOL  MApi_PNL_PreInit(E_PNL_PREINIT_OPTIONS eInitParam)
{
    if ( eInitParam == E_PNL_NO_OUTPUT)
    {
        MDrv_PNL_PreInit( E_PNL_OUTPUT_NO_OUTPUT );
    }
    else if ( eInitParam == E_PNL_CLK_ONLY)
    {
        MDrv_PNL_PreInit( E_PNL_OUTPUT_CLK_ONLY);
    }
    else if ( eInitParam == E_PNL_CLK_DATA)
    {
        MDrv_PNL_PreInit(E_PNL_OUTPUT_CLK_DATA);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

PanelType * MApi_XC_GetPanelSpec(PANEL_RESOLUTION_TYPE enResolutionType)
{
    return tPanelIndexTbl[(MS_U8)enResolutionType];
}

void _MDrv_PNL_Init_XC_Clk(void)
{
    // setup output dot clock
    W1BYTE(REG_CKG_ODCLK, CKG_ODCLK_CLK_LPLL, 3:2);      // select source tobe LPLL clock
    W1BYTE(REG_CKG_ODCLK, 0, CKG_ODCLK_INVERT);               // clock not invert
    W1BYTE(REG_CKG_ODCLK, 0, CKG_ODCLK_GATED);                // enable clock
}

void MHal_PNL_Init_XC_Clk(PNL_InitData *pstPanelInitData)
{
//
    // setup output dot clock
    W2BYTE(REG_CLKGEN0_53_L,0x0C00,15:0);
    W2BYTE(REG_CLKGEN0_57_L,0x0000,15:0); //[15:12]ckg_bt656 [3:0]ckg_fifo
    W2BYTE(REG_CLKGEN0_58_L,0x0000,15:0); //[3:0]ckg_tx_mod
    W2BYTE(REG_CLKGEN0_5E_L,0x0000,15:0); //[11:8]ckg_tx_mod [3:0]ckg_osd2mod
    //*/
}

void MHal_Output_LVDS_Pair_Setting(MS_U8 Type)
{
#if defined(PRJ_X10)
    //in A5, the real Channel setting should be 0_7,8_15,16_24
    MS_U16 u16OutputCFG0_7 = 0;
    MS_U16 u16OutputCFG8_15 = 0;
    MS_U16 u16OutputCFG16_24 = 0;
    //==========
    // Bypass
    //  1 : 129A
    //  2 : 129B
    //  3 : 129C
    //User define         4
    //===========================

    //for 129B
    if(Type == LVDS_SINGLE_OUTPUT_A )
    {
        u16OutputCFG0_7 = 0x0000;
        u16OutputCFG8_15 = 0x5500;
        u16OutputCFG16_24 = 0x5555;
    }
    //for 129C
    else if(Type == LVDS_SINGLE_OUTPUT_B)
    {
        u16OutputCFG0_7 = 0x5555;
        u16OutputCFG8_15 = 0x0055;
        u16OutputCFG16_24 = 0x0000;
    }
    //for 129A
    else if( Type == LVDS_DUAL_OUTPUT_SPECIAL)
    {
        u16OutputCFG0_7 = 0x0000;
        u16OutputCFG8_15 = 0x5500;
        u16OutputCFG16_24 = 0x5555;
    }
    else if( Type == LVDS_OUTPUT_User)
    {
        //setting by user
        u16OutputCFG0_7 = 0x5555;
        u16OutputCFG8_15 = 0x5555;
        u16OutputCFG16_24 = 0x5555;
    }
    else
    {
        u16OutputCFG0_7 = 0x0000;
        u16OutputCFG8_15 = 0x5500;
        u16OutputCFG16_24 = 0x5555;
    }
    MHal_FRC_MOD_SetOutputCfg(u16OutputCFG0_7, u16OutputCFG8_15, u16OutputCFG16_24, 0x00);
#else
    if(Type == LVDS_DUAL_OUTPUT_SPECIAL )
    {
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_L), XC_MOD_OUTPUT_CONF_L_LVDS_MD4_VAL, XC_MOD_OUTPUT_CONF_L_LVDS_MD4_MSK);
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_M), XC_MOD_OUTPUT_CONF_M_LVDS_MD3_VAL, XC_MOD_OUTPUT_CONF_M_LVDS_MD4_MSK);
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_H), XC_MOD_OUTPUT_CONF_H_LVDS_MD3_VAL, XC_MOD_OUTPUT_CONF_H_LVDS_MD4_MSK);
    }
    else if(Type == LVDS_SINGLE_OUTPUT_A)
    {
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_L), L_LVDS_SINGLE_A_VAL, XC_MOD_OUTPUT_CONF_L_LVDS_MD3_MSK);
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_M), M_LVDS_SINGLE_A_VAL, XC_MOD_OUTPUT_CONF_M_LVDS_MD3_MSK);
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_H), H_LVDS_SINGLE_A_VAL, XC_MOD_OUTPUT_CONF_H_LVDS_MD3_MSK);
    }
    else if( Type == LVDS_SINGLE_OUTPUT_B)
    {
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_L), L_LVDS_SINGLE_B_VAL, XC_MOD_OUTPUT_CONF_L_LVDS_MD3_MSK);
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_M), M_LVDS_SINGLE_B_VAL, XC_MOD_OUTPUT_CONF_M_LVDS_MD3_MSK);
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_H), H_LVDS_SINGLE_B_VAL, XC_MOD_OUTPUT_CONF_H_LVDS_MD3_MSK);
    }
    else
    {
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_L), XC_MOD_OUTPUT_CONF_L_LVDS_MD3_VAL, XC_MOD_OUTPUT_CONF_L_LVDS_MD3_MSK);
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_M), XC_MOD_OUTPUT_CONF_M_LVDS_MD3_VAL, XC_MOD_OUTPUT_CONF_M_LVDS_MD3_MSK);
        W2BYTE_TAB(BK_MOD(XC_MOD_OUTPUT_CONF_H), XC_MOD_OUTPUT_CONF_H_LVDS_MD3_VAL, XC_MOD_OUTPUT_CONF_H_LVDS_MD3_MSK);
    }
#endif

    msleep(2);
}


void MHal_Shift_LVDS_Pair(MS_U8 Type)
{
    if(Type == 1)
        MOD_W2BYTEMSK(REG_MOD_BK00_42_L, 0x00C0 ,0x00C0); // shift_lvds_pair, set LVDS Mode3
    else
        MOD_W2BYTEMSK(REG_MOD_BK00_42_L, 0x0000 ,0x00C0); // shift_lvds_pair, set LVDS Mode3
}


MS_BOOL MHal_PNL_MOD_Control_Out_Swing(MS_U16 u16Swing_Level)
{
    MS_BOOL bStatus = FALSE;

    MS_U16 u16ValidSwing = 0;

    return TRUE;

    if(u16Swing_Level>600)
        u16Swing_Level=600;
    if(u16Swing_Level<40)
        u16Swing_Level=40;

    u16ValidSwing = _MHal_PNL_MOD_Swing_Refactor_AfterCAL(u16Swing_Level);

    // Separate mode.
    MOD_W2BYTEMSK(REG_MOD_BK00_29_L,
        ( (u16ValidSwing << 4 )|(u16ValidSwing << 10 ) ) , 0xFFF0);

    MOD_W2BYTE(REG_MOD_BK00_2A_L,
        ((u16ValidSwing  )|(u16ValidSwing << 6 ) |(u16ValidSwing << 12 ) ));

    MOD_W2BYTE(REG_MOD_BK00_2B_L,
        ( (u16ValidSwing >> 4  )|(u16ValidSwing << 2 ) |(u16ValidSwing << 8 ) | (u16ValidSwing << 14 )));

    MOD_W2BYTE(REG_MOD_BK00_2C_L,
        ( (u16ValidSwing >> 2  )|(u16ValidSwing << 4 ) |(u16ValidSwing << 10 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_2D_L,
        ( (u16ValidSwing  )|(u16ValidSwing << 6 ) |(u16ValidSwing << 12 ) ));

    MOD_W2BYTE(REG_MOD_BK00_2E_L,
        ( (u16ValidSwing >> 4 )|(u16ValidSwing << 2 ) |(u16ValidSwing << 8 ) |(u16ValidSwing << 14 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_2F_L,
        ( (u16ValidSwing >> 2  )|(u16ValidSwing << 4 ) |(u16ValidSwing << 10 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_30_L,
        ( (u16ValidSwing  )|(u16ValidSwing << 6 ) |(u16ValidSwing << 12 ) ));

    MOD_W2BYTEMSK(REG_MOD_BK00_31_L, (u16ValidSwing >> 4 ) ,0x0003);

    bStatus = TRUE;

    return bStatus;
}


////////////////////////////////////////////////////////////////////////
// Turn Pre-Emphasis Current function
////////////////////////////////////////////////////////////////////////
MS_BOOL MHal_PNL_MOD_Control_Out_PE_Current (MS_U16 u16Current_Level)
{
    MS_BOOL bStatus = FALSE;
    MS_U16 u16ValidCurrent = u16Current_Level & 0x07;

    MOD_W2BYTEMSK(REG_MOD_BK00_23_L,
        ( (u16ValidCurrent  ) |(u16ValidCurrent << 3 )|(u16ValidCurrent << 6 )
        |(u16ValidCurrent << 9 ) |(u16ValidCurrent << 12 )) , 0x7FFF);

    MOD_W2BYTEMSK(REG_MOD_BK00_24_L,
        ( (u16ValidCurrent  ) |(u16ValidCurrent << 3 )|(u16ValidCurrent << 6 )
        |(u16ValidCurrent << 9 ) |(u16ValidCurrent << 12 )) , 0x7FFF);

    MOD_W2BYTEMSK(REG_MOD_BK00_25_L,
        ( (u16ValidCurrent  ) |(u16ValidCurrent << 3 )|(u16ValidCurrent << 6 )
        |(u16ValidCurrent << 9 ) |(u16ValidCurrent << 12 )) , 0x7FFF);

    MOD_W2BYTEMSK(REG_MOD_BK00_26_L,
        ( (u16ValidCurrent  ) |(u16ValidCurrent << 3 )|(u16ValidCurrent << 6 )
        |(u16ValidCurrent << 9 ) |(u16ValidCurrent << 12 )) , 0x7FFF);

    MOD_W2BYTEMSK(REG_MOD_BK00_29_L, u16ValidCurrent  ,0x0007);

    bStatus = TRUE;

    return bStatus;
}


////////////////////////////////////////////////////////////////////////
// 1.Turn TTL low-power mode function
// 2.Turn internal termination function
// 3.Turn DRIVER BIAS OP function
////////////////////////////////////////////////////////////////////////
MS_BOOL MHal_PNL_MOD_Control_Out_TTL_Resistor_OP (MS_BOOL bEnble)
{
    MS_BOOL bStatus = FALSE;
    if(bEnble)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_36_L, 0xFFFF, 0xFFFF); //Enable TTL low-power mode
        MOD_W2BYTEMSK(REG_MOD_BK00_37_L, 0x001E, 0x001E);

        MOD_W2BYTEMSK(REG_MOD_BK00_75_L, 0xFFFF, 0xFFFF); //GCR_EN_RINT (internal termination open)
        MOD_W2BYTEMSK(REG_MOD_BK00_76_L, 0x003F, 0x003F);

        MOD_W2BYTEMSK(REG_MOD_BK00_79_L, 0xFFFF, 0xFFFF); //Disable DRIVER BIAS OP
        MOD_W2BYTEMSK(REG_MOD_BK00_7A_L, 0x003F, 0x003F);
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_36_L, 0x0000, 0xFFFF); //Disable TTL low-power mode
        MOD_W2BYTEMSK(REG_MOD_BK00_37_L, 0x0000, 0x001E);

        MOD_W2BYTEMSK(REG_MOD_BK00_75_L, 0x0000, 0xFFFF); //GCR_EN_RINT (internal termination close)
        MOD_W2BYTEMSK(REG_MOD_BK00_76_L, 0x0000, 0x003F);

        MOD_W2BYTEMSK(REG_MOD_BK00_79_L, 0x0000, 0xFFFF); //Enable DRIVER BIAS OP
        MOD_W2BYTEMSK(REG_MOD_BK00_7A_L, 0x0000, 0x003F);
    }

    bStatus = TRUE;
    return bStatus;
}

void MApi_PNL_SetLVDSFormat_Cus(LvdsFmt_k fmtLVDS)
{
    //printk ("[%s @ %04d] LVDS formt (%04d) \n", __FUNCTION__, __LINE__, fmtLVDS);
    switch(fmtLVDS)
    {
        case LVDS_FMT_VESA_8:
        case LVDS_FMT_PDP_10:
            MOD_W2BYTEMSK(REG_MOD_BK00_40_L, BIT(2), BIT(2));
            MOD_W2BYTEMSK(REG_MOD_BK00_4B_L, BIT(1), BIT(1)|BIT(0));
            break;
        case LVDS_FMT_PDP_8:
            MOD_W2BYTEMSK(REG_MOD_BK00_40_L, 0, BIT(2));
            MOD_W2BYTEMSK(REG_MOD_BK00_4B_L, BIT(1), BIT(1)|BIT(0));
            break;
        case LVDS_FMT_NS_10:
            MOD_W2BYTEMSK(REG_MOD_BK00_40_L, 0, BIT(2));
            MOD_W2BYTEMSK(REG_MOD_BK00_4B_L, 0x00, BIT(1)|BIT(0));
            break;
        case LVDS_FMT_JEIDA_8:
        case LVDS_FMT_JEIDA_10:
        case LVDS_FMT_MAX:
        default:
            MOD_W2BYTEMSK(REG_MOD_BK00_40_L, 0, BIT(2));
            MOD_W2BYTEMSK(REG_MOD_BK00_4B_L, 0, BIT(1)|BIT(0));
            break;
    }
}

void MDrv_PNL_SetOutputType(PNL_OUTPUT_MODE eOutputMode)
{
    MS_U16 u16ValidSwing2 = 0;

    switch(eOutputMode)
    {
            case E_PNL_OUTPUT_NO_OUTPUT:
                // if MOD_45[5:0] = 0x3F && XC_MOD_EXT_DATA_EN_L = 0x0,
                // then if XC_MOD_OUTPUT_CONF_L = 0x0 ---> output TTL as tri-state
                MOD_W2BYTEMSK(REG_MOD_BK00_6D_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x0000);
                MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0x0000, 0x000F);

                //----------------------------------
                // Purpose: Set the output to be the GPO, and let it's level to Low
                // 1. External Enable, Pair 0~5
                // 2. GPIO Enable, pair 0~5
                // 3. GPIO Output data : All low, pair 0~5
                // 4. GPIO OEZ: output piar 0~5
                //----------------------------------

                //1.External Enable, Pair 0~5
                MOD_W2BYTEMSK(REG_MOD_BK00_46_L, 0x0FFF, 0x0FFF);
                //2.GPIO Enable, pair 0~5
                MOD_W2BYTEMSK(REG_MOD_BK00_4D_L, 0x0FFF, 0x0FFF);
                //3.GPIO Output data : All low, pair 0~5
                MOD_W2BYTEMSK(REG_MOD_BK00_4F_L, 0x0000, 0x0FFF);
                //4.GPIO OEZ: output piar 0~5
                MOD_W2BYTEMSK(REG_MOD_BK00_51_L, 0x0000, 0x0FFF);

                //1.External Enable, Pair 6~15
                MOD_W2BYTEMSK(REG_MOD_BK00_46_L, 0xF000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_47_L, 0xFFFF);
                //2.GPIO Enable, pair 6~15
                MOD_W2BYTEMSK(REG_MOD_BK00_4D_L, 0xF000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_4E_L, 0xFFFF);
                //3.GPIO Output data : All low, pair 6~15
                MOD_W2BYTEMSK(REG_MOD_BK00_4F_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_50_L, 0x0000);
                //4.GPIO OEZ: output piar 6~15
                MOD_W2BYTEMSK(REG_MOD_BK00_51_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_52_L, 0x0000);

                //1234.External Enable, Pair 16~17
                MOD_W2BYTE(REG_MOD_BK00_7E_L, 0xFF00);

                //1.External Enable, Pair 18~20, 2.GPIO Enable, pair 18~20
                MOD_W2BYTEMSK(REG_MOD_BK00_7C_L, 0x3F3F, 0x3F3F);
                //3.GPIO Output data : All low, pair 18~20
                MOD_W2BYTEMSK(REG_MOD_BK00_7A_L, 0x0000, 0x3F00);
                //4.GPIO OEZ: output piar 18~20
                MOD_W2BYTEMSK(REG_MOD_BK00_7F_L, 0x0000, 0xFC00);
                break;

            case E_PNL_OUTPUT_CLK_ONLY:
                MOD_W2BYTEMSK(REG_MOD_BK00_6D_L, 0, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x4004);
                MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0, 0x000F);
                break;

            case E_PNL_OUTPUT_DATA_ONLY:
            case E_PNL_OUTPUT_CLK_DATA:
            default:

                MOD_W2BYTEMSK(REG_MOD_BK00_4D_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_4E_L, 0x0000);
                //1. set GCR_PVDD_2P5=1!|b1;           MOD PVDD power:    1: 2.5V
                MOD_W2BYTEMSK(REG_MOD_BK00_37_L, BIT(6), BIT(6));
                //2. set PD_IB_MOD=1!|b0;
                MOD_W2BYTEMSK(REG_MOD_BK00_78_L, 0x00, BIT(0));
                //  save ch6 init value
                u16ValidSwing2 = (MOD_R2BYTEMSK(REG_MOD_BK00_2B_L, 0x3F00)>>8);
                //3. set Desired Pairs: GCR_ICON[5:0]=6!|h3f (current all open);
                MHal_PNL_MOD_Control_Out_Swing(0x3F);
                //4. set Desired Pairs: GCR_PE_ADJ[2:0]=3!|h7 (pre-emphasis current all open )
                MHal_PNL_MOD_Control_Out_PE_Current (0x07);
                //5. Enable low-power modeinternal termination Open, Enable OP
                MHal_PNL_MOD_Control_Out_TTL_Resistor_OP (1);

                //MsOS_DelayTask(1);

                //6. Enable low-power modeinternal termination Open, Enable OP
                MHal_Output_LVDS_Pair_Setting( LVDS_OUTPUT_User);
                MHal_Shift_LVDS_Pair(_u8MOD_LVDS_Pair_Shift);

                //7. set Desired Pairs: GCR_PE_ADJ[2:0]=3!|h0 (pre-emphasis current all Close)
                MHal_PNL_MOD_Control_Out_PE_Current (0x00);
                //8. set Desired Pairs: GCR_ICON[5:0]    (current all init);
                MHal_PNL_MOD_Control_Out_Swing(u16ValidSwing2);
                //9. Disable low-power modeinternal termination Close, Disable OP
                MHal_PNL_MOD_Control_Out_TTL_Resistor_OP (0);

                // other TTL setting
                MOD_W2BYTE(REG_MOD_BK00_45_L, 0x003F);     // LVDS output enable, [5:4] Output enable: PANEL_LVDS/ PANEL_miniLVDS/ PANEL_RSDS

                MOD_W2BYTEMSK(REG_MOD_BK00_46_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_47_L, 0x0000);
                MOD_W2BYTEMSK(REG_MOD_BK00_7E_L, 0x0000, 0x000F);

                MOD_W2BYTEMSK(REG_MOD_BK00_44_L, 0x000, 0x3FF);    // TTL skew

                // GPO gating
                MOD_W2BYTEMSK(REG_MOD_BK00_4A_L, 0x0, BIT(8));     // GPO gating

                break;
        }
}
static void _MDrv_PNL_Init_XC_VOP(PNL_InitData *pstPanelInitData)
{

    MS_U16 u16DE_HEND = (pstPanelInitData->u16HStart + pstPanelInitData->u16Width - 1);
    MS_U16 u16DE_HStart = pstPanelInitData->u16HStart;
    MS_U16 u16DE_VEND = (pstPanelInitData->u16VStart + pstPanelInitData->u16Height - 1);
    MS_U16 u16DE_VStart = pstPanelInitData->u16VStart;

    MS_U8 u8Bank;

    u8Bank = R1BYTE(XC_SC_BK_SELECT, 7:0);
    W1BYTE(XC_SC_BK_SELECT, XC_BK_VOP, 7:0);
    // Htotal/Vtotal
    W2BYTE(BK_VOP(XC_VOP_HDTOT), (pstPanelInitData->u16HTotal - 1), XC_VOP_HDTOT_MASK);         // output htotal
    W2BYTE(BK_VOP(XC_VOP_VDTOT), (pstPanelInitData->u16VTotal - 1), XC_VOP_VDTOT_MASK);         // output vtotal

    // DE H/V start/size
    W2BYTE(BK_VOP(XC_VOP_DE_HSTART), u16DE_HStart, XC_VOP_DE_HSTART_MASK);                                       // DE H start
    W2BYTE(BK_VOP(XC_VOP_DE_HEND)  , u16DE_HEND  , XC_VOP_DE_HEND_MASK);  // DE H end
    W2BYTE(BK_VOP(XC_VOP_DE_VSTART), u16DE_VStart, XC_VOP_DE_VSTART_MASK);                                       // DE V start
    W2BYTE(BK_VOP(XC_VOP_DE_VEND)  , u16DE_VEND  , XC_VOP_DE_VEND_MASK);  // DE V end

    // Display H/V start/size
    W2BYTE(BK_VOP(XC_VOP_SIHST) , u16DE_HStart , XC_VOP_SIHST_MASK);                                               // Display H start
    W2BYTE(BK_VOP(XC_VOP_SIHEND), u16DE_HEND   , XC_VOP_SIHEND_MASK);  // Display H end
    W2BYTE(BK_VOP(XC_VOP_SIVST) , u16DE_VStart , XC_VOP_SIVST_MASK);                                               // Display V start
    W2BYTE(BK_VOP(XC_VOP_SIVEND), u16DE_VEND   , XC_VOP_SIVEND_MASK);  // Display V end

    // H/V sync start/width
    W2BYTE(BK_VOP(XC_VOP_HSEND_WIDTH), (pstPanelInitData->u8HSyncWidth-1), XC_VOP_HSEND_WIDTH_MASK);                    // hsync width
    W2BYTE(BK_VOP(XC_VOP_VSST), (pstPanelInitData->u16VTotal - pstPanelInitData->u16VSyncStart), XC_VOP_VSST_MASK);     // vsync start = Vtt - (VSyncWidth + BackPorch)
    W2BYTE(BK_VOP(XC_VOP_VSEND), (pstPanelInitData->u16VTotal - pstPanelInitData->u8VSyncWidth), XC_VOP_VSEND_MASK);    // vsync end   = Vtt - (VSyncWidth)
    W2BYTE(BK_VOP(XC_VOP_AOVS), pstPanelInitData->bManuelVSyncCtrl, XC_VOP_AOVS_MASK);                                  // manual vsync control
    W2BYTE(BK_VOP(XC_VOP_AOVS), 1, 14:14);       //default set to mode1                                                                       // disable auto_htotal

    // output control
    W2BYTE(BK_VOP(XC_VOP_OCTRL), pstPanelInitData->u16OCTRL, XC_VOP_OCTRL_MASK);

    if (pstPanelInitData->eLPLL_Type == E_PNL_TYPE_LVDS ||
        pstPanelInitData->eLPLL_Type == E_PNL_TYPE_DAC_I ||
        pstPanelInitData->eLPLL_Type == E_PNL_TYPE_DAC_P)
    {
        // LVDS
        W1BYTE(BK_VOP(0x22), 0x11, 7:0);  //   0x2f44 : Enable clock of internal control - LVDS or TTL Output
    }
    else
    {
        // RSDS
        W1BYTE(BK_VOP(0x22), 0x00, 7:0);
    }

    W2BYTE(BK_VOP(XC_VOP_OSTRL), pstPanelInitData->u16OSTRL, XC_VOP_OSTRL_MASK);

    // output driving current
    W2BYTE(BK_VOP(XC_VOP_ODRV), pstPanelInitData->u16ODRV, XC_VOP_ODRV_MASK);

    // dither control
    W2BYTE(BK_VOP(XC_VOP_DITHCTRL), pstPanelInitData->u16DITHCTRL, XC_VOP_DITHCTRL_MASK);

    W1BYTE(XC_SC_BK_SELECT, u8Bank, 7:0);
}

static void _MDrv_PNL_Init_MOD(PNL_InitData *pstPanelInitData)
{
    MST_PANEL_FRC_INFO_t FRCPanelInfo_st;
    MS_U8 u8Bank = 0;
    memset(&FRCPanelInfo_st, 0, sizeof(MST_PANEL_FRC_INFO_t));

#if defined(PRJ_X10)

    switch(_u8MOD_LVDS_Pair_Type)
    {
        default:
        case 1: //BD_129A Bypass mode
        if(pstPanelInitData->stPNL_Init_Adv.bFRC_En)
            pstPanelInitData->u16MOD_CTRL0 &= 0xFFDF;
        else
            pstPanelInitData->u16MOD_CTRL0 &= 0xFFDF;
        break;

        case 2: //BD_129B Bypass mode
        if(pstPanelInitData->stPNL_Init_Adv.bFRC_En)
            pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF;
        else
        {
            switch(pstPanelInitData->eLPLL_Mode)
            {
                default:
                case 1: //single/dual mode
                pstPanelInitData->u16MOD_CTRL0 &= 0xFF9F;
                break;
                case 2: //Quad mode
                pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF; // BIT5=1 => 0x3032_05_L[11]=0 // BIT2=1, PANEL_LVDS_TI_MODE
                break;
            }
        }
        break;

        case 3: //BD_129C Bypass mode
        if(pstPanelInitData->stPNL_Init_Adv.bFRC_En)
            pstPanelInitData->u16MOD_CTRL0 &= 0xFF9F;
        else
        {
            switch(pstPanelInitData->eLPLL_Mode)
            {
                default:
                case 0://single mode
//printf("\n\n##[%s][%d] CTRL0= %x\n\n", __FUNCTION__, __LINE__, pstPanelInitData->u16MOD_CTRL0);
                pstPanelInitData->u16MOD_CTRL0 |= BIT(5);
                pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF;
                break;
                case 1: //dual mode
                pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF;
                break;
                case 2: //Quad mode
                pstPanelInitData->u16MOD_CTRL0 &= 0xFF9F; // BIT5=0 => 0x3032_05_L[11]=0 // BIT2=1, PANEL_LVDS_TI_MODE
                break;
            }
        }
        break;
    }

    MHal_PNL_ParamMap(pstPanelInitData, &FRCPanelInfo_st);
    MHal_FRC_MOD_Init(&FRCPanelInfo_st);
#else

    u8Bank = R1BYTE(XC_MOD_BK_SELECT, 7:0);
    W1BYTE(XC_MOD_BK_SELECT, 0, 7:0);           // switch to MOD bank

    W2BYTE(BK_MOD(XC_MOD_CTRL0), 0x0008, 15:0);

    //    GPIO is controlled in drvPadConf.c
    //MDrv_Write2Byte(L_BK_MOD(0x46), 0x0000);    //EXT GPO disable
    //MDrv_Write2Byte(L_BK_MOD(0x47), 0x0000);    //EXT GPO disable
    W2BYTE(BK_MOD(XC_MOD_CTRL9), pstPanelInitData->u16MOD_CTRL9, XC_MOD_CTRL9_MASK); //{L_BK_MOD(0x49), 0x00}, // [7,6] : output formate selction 10: 8bit, 01: 6bit :other 10bit, bit shift
    W2BYTE(BK_MOD(XC_MOD_CTRLA), pstPanelInitData->u16MOD_CTRLA, XC_MOD_CTRLA_MASK);
    W2BYTE(BK_MOD(XC_MOD_CTRLB),  0x0000, XC_MOD_CTRLB_MASK);  //[1:0]ti_bitmode 10:8bit  11:6bit  0x:10bit

    if ( SUPPORT_SYNC_FOR_DUAL_MODE )
    {
        // Set 1 only when PNL is dual mode
        W2BYTE(BK_MOD(XC_MOD_SYNC_FOR_DUAL_MODE), pstPanelInitData->eLPLL_Mode , XC_MOD_SYNC_FOR_DUAL_MODE_MASK);
    }

    //dual port lvds _start_//
    // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    W2BYTE(BK_MOD(0x6F), 0x0005, 15:0);    // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    W2BYTE(BK_MOD(0x77), 0x1F, 7:0);       // original is MDrv_WriteByteMask(L_BK_MOD(0x77), 0x0F, BITMASK(7:2));
    //dual port lvds _end_//

    //W2BYTE(BK_MOD(0x78), _u8PnlDiffSwingLevel, 7:1);       //differential output swing level
    if(pstPanelInitData->eLPLL_Type != E_PNL_TYPE_MINILVDS)
    {
        W2BYTE(BK_MOD(0x7D), 0x1F, 7:0);       //[6]disable power down bit and [5:0]enable all channel
    }
    W2BYTE(BK_MOD(XC_MOD_LVDS_TX_SWAP_PN), pstPanelInitData->u16LVDSTxSwapValue, XC_MOD_LVDS_TX_SWAP_PN_MASK);


    // TODO: move from MDrv_Scaler_Init(), need to double check!
    W2BYTE(BK_MOD(0x53), ENABLE, 0:0);

    W1BYTE(XC_MOD_BK_SELECT, u8Bank, 7:0);


    //--------------------------------------------------------------
    //Depend On Bitmode to set Dither
    //--------------------------------------------------------------
    u8Bank = R1BYTE(XC_SC_BK_SELECT, 7:0);
    W1BYTE(XC_SC_BK_SELECT, XC_BK_PAFRC, 7:0);

    // always enable noise dither and disable TAILCUT
    W1BYTE(L_BK_PAFRC(XC_PAFRC_DITH), pstPanelInitData->u8PanelNoiseDith ? XC_PAFRC_DITH_NOISEDITH_EN : (1 - XC_PAFRC_DITH_NOISEDITH_EN) , XC_PAFRC_DITH_NOISEDITH_EN_MASK);
    W1BYTE(L_BK_PAFRC(XC_PAFRC_DITH), XC_PAFRC_DITH_TAILCUT_DISABLE, XC_PAFRC_DITH_TAILCUT_MASK);

    switch(pstPanelInitData->u8MOD_CTRLB & BMASK(PNL_HAL_TIMODE_MASK))//[1:0]ti_bitmode b'10:8bit  11:6bit  0x:10bit
    {
        case HAL_TI_6BIT_MODE:
            W1BYTE(L_BK_PAFRC(XC_PAFRC_DITH), XC_PAFRC_DITH_ON_6BITS, XC_PAFRC_DITH_MASK);
            W1BYTE(L_BK_PAFRC(XC_PAFRC_DITH), XC_PAFRC_DITH_BITS_6BIT, XC_PAFRC_DITH_BITS_MASK);
            break;

        case HAL_TI_8BIT_MODE:
            W1BYTE(L_BK_PAFRC(XC_PAFRC_DITH), XC_PAFRC_DITH_ON_8BITS, XC_PAFRC_DITH_MASK);
            W1BYTE(L_BK_PAFRC(XC_PAFRC_DITH), XC_PAFRC_DITH_BITS_8BIT, XC_PAFRC_DITH_BITS_MASK);
            break;

        case HAL_TI_10BIT_MODE:
        default:
            W1BYTE(L_BK_PAFRC(XC_PAFRC_DITH), XC_PAFRC_DITH_ON_10BITS, XC_PAFRC_DITH_MASK);
            W1BYTE(L_BK_PAFRC(XC_PAFRC_DITH), XC_PAFRC_DITH_BITS_10BIT, XC_PAFRC_DITH_BITS_MASK);
            break;
    }

    W1BYTE(XC_SC_BK_SELECT, u8Bank, 7:0);

    //-----depend on bitmode to set Dither------------------------------
    //MDrv_PNL_SetOutputType(E_PNL_OUTPUT_CLK_DATA);     // TTL to Ursa
    MDrv_PNL_SetOutputType(_eDrvPnlInitOptions);     // TTL to Ursa
#endif
}

void MHal_PNL_ParamMap(PNL_InitData *pstPanelInitData, PMST_PANEL_FRC_INFO_t pFRCPanelInfo_st)
{
    pFRCPanelInfo_st->u8LPLL_Type      = (MS_U8)pstPanelInitData->eLPLL_Type;  //0~4, _TTL,_MINI_LVDS,_LVDS,_RSDS,_MINI_LVDS_GIP
    pFRCPanelInfo_st->u8LPLL_Mode      = (MS_U8)pstPanelInitData->eLPLL_Mode;  //_SINGLE,_DUAL,_QUAD,_QUAD_LR,_V_BY1

    // MOD
    pFRCPanelInfo_st->u16MOD_CTRL0     = pstPanelInitData->u16MOD_CTRL0;
    pFRCPanelInfo_st->u16MOD_CTRL9     = pstPanelInitData->u16MOD_CTRL9;
    pFRCPanelInfo_st->u16MOD_CTRLA     = pstPanelInitData->u16MOD_CTRLA;
    pFRCPanelInfo_st->u8MOD_CTRLB     =  pstPanelInitData->u8MOD_CTRLB;

    //-------------------------------------------------------------------------------------------------
    // FRC Control
    //-------------------------------------------------------------------------------------------------
    pFRCPanelInfo_st->u16MOD_SwingLevel= pstPanelInitData->stPNL_Init_Adv.u16MOD_SwingLevel;
    pFRCPanelInfo_st->u16MOD_SwingCLK  = pstPanelInitData->stPNL_Init_Adv.u16MOD_SwingCLK;
    pFRCPanelInfo_st->u8PanelNoiseDith = pstPanelInitData->u8PanelNoiseDith;   ///URSA OP2;
    pFRCPanelInfo_st->bFRC_En          = pstPanelInitData->stPNL_Init_Adv.bFRC_En;
    pFRCPanelInfo_st->u8lvdsSwapCh     = pstPanelInitData->stPNL_Init_Adv.u8lvdsSwapCh;
    pFRCPanelInfo_st->bdither6Bits     = pstPanelInitData->stPNL_Init_Adv.bdither6Bits;             ///URSA OP2
    pFRCPanelInfo_st->blvdsShiftPair   = pstPanelInitData->stPNL_Init_Adv.blvdsShiftPair; // = (MS_BOOL)(_u8MOD_LVDS_Pair_Shift ?(TRUE):(FALSE));
    pFRCPanelInfo_st->blvdsSwapPair    = pstPanelInitData->stPNL_Init_Adv.blvdsSwapPair;
    // EPI
    pFRCPanelInfo_st->bepiLRSwap       = pstPanelInitData->stPNL_Init_Adv.bepiLRSwap;
    pFRCPanelInfo_st->bepiLMirror      = pstPanelInitData->stPNL_Init_Adv.bepiLMirror;
    pFRCPanelInfo_st->bepiRMirror      = pstPanelInitData->stPNL_Init_Adv.bepiRMirror;
}

#if defined(PRJ_X10)
void MHal_FRC_MOD_Init(PMST_PANEL_FRC_INFO_t pFRCPanelInfo_st)
{
    MS_BOOL blvdsSwapPol;
    MS_BOOL bInvert_HS, bInvert_VS, bInvert_DE;
    MS_BOOL bMask_HS, bMask_VS, bMask_DE;

    blvdsSwapPol = ((pFRCPanelInfo_st->u16MOD_CTRL0 & BIT(5)) >> 5);

    // tmp sol : just test for x10 Board 03
    blvdsSwapPol = 1;
    // tmp sol : just test for x10 Board 03

    bInvert_HS = ((pFRCPanelInfo_st->u16MOD_CTRLA & BIT(2)) >> 2);
    bInvert_VS = ((pFRCPanelInfo_st->u16MOD_CTRLA & BIT(3)) >> 3);
    bInvert_DE = ((pFRCPanelInfo_st->u16MOD_CTRLA & BIT(12)) >> 12);
    bMask_HS = 0;
    bMask_VS = 0;
    bMask_DE = 0;
    //*((volatile unsigned short *) 0xbf60649c) = 0x3F;
    MHal_FRC_MOD_SetClkEn(ENABLE);
    MOD_W2BYTEMSK(REG_MOD_BK00_28_L, 0x03<<12, 0xF000); // REG_3251
    MHal_FRC_MOD_SetPowerdown(FRC_MOD_PD_MODATOP, (FRC_MOD_PD_REG_B | FRC_MOD_PD_REG_A));
    MHal_FRC_MOD_SetInvert(bInvert_HS, bInvert_VS, bInvert_DE,
                           bMask_HS, bMask_VS, bMask_DE);
    MHal_FRC_MOD_SetCtrlSwing(pFRCPanelInfo_st->u16MOD_SwingLevel, pFRCPanelInfo_st->u16MOD_SwingCLK);
    MOD_W2BYTEMSK(REG_MOD_BK00_30_L, BIT(15), BIT(15)); // REG_3261 MHal_FRC_MOD_DigiSerializerChk_En
    MHal_FRC_MOD_SetModTypeParam(pFRCPanelInfo_st->u8LPLL_Type,  pFRCPanelInfo_st->u8LPLL_Mode,
                                 pFRCPanelInfo_st->u16MOD_CTRL0, pFRCPanelInfo_st->u8MOD_CTRLB);
    switch(pFRCPanelInfo_st->u8LPLL_Type)
    {
        case E_PNL_TYPE_LVDS:
        default:
            MHal_FRC_MOD_SetLvdsSwap(pFRCPanelInfo_st->u8lvdsSwapCh,
                                     pFRCPanelInfo_st->blvdsShiftPair,
                                     pFRCPanelInfo_st->blvdsSwapPair,
                                     blvdsSwapPol);
            break;
    }

}

// Set LVDS SWAP: 1.Pair Shift 2. swap polarity 3. swap pair
void MHal_FRC_MOD_SetLvdsSwap(MS_U8 u8lvdsSwapCh, MS_BOOL blvdsShiftPair, MS_BOOL blvdsSwapPair, MS_BOOL blvdsSwapPol)
{
    MS_U16 u16EnableMode, u16DisableMode;
    u16EnableMode=0;
    u16DisableMode=0;

    if(blvdsShiftPair)  // low active
        u16DisableMode|=FRC_MOD_LVDS_PAIR_SHIFT;
    else
        u16EnableMode|=FRC_MOD_LVDS_PAIR_SHIFT;

    if(blvdsSwapPair)
        u16EnableMode|=FRC_MOD_LVDS_SWAP_PAIR;
    else
        u16DisableMode|=FRC_MOD_LVDS_SWAP_PAIR;

    if(blvdsSwapPol)
        u16EnableMode|=FRC_MOD_LVDS_SWAP_POLARITY;
    else
        u16DisableMode|=FRC_MOD_LVDS_SWAP_POLARITY;


    MHal_FRC_MOD_LvdsModeSel(u16EnableMode, u16DisableMode );

    MHal_FRC_MOD_LvdsSwapSel(u8lvdsSwapCh);
}

//===================================
//    Channel Swap  [15:8]
//===================================
void MHal_FRC_MOD_LvdsSwapSel(MS_U16 u16SwapCH)
{
    // channel swap
    MOD_W2BYTEMSK(REG_MOD_BK00_0F_L, u16SwapCH<<8, 0xFF00); // REG_321F
}

void MHal_FRC_MOD_SetInvert(MS_BOOL bInvert_HS, MS_BOOL bInvert_VS, MS_BOOL bInvert_DE, MS_BOOL bMask_HS, MS_BOOL bMask_VS, MS_BOOL bMask_DE)
{
    MS_U16 reg=0;
    if(bMask_HS)   reg|=BIT(2);
    if(bMask_VS)   reg|=BIT(3);
    if(bMask_DE)   reg|=BIT(4);
    if(bInvert_HS) reg|=BIT(5);
    if(bInvert_VS) reg|=BIT(6);
    if(bInvert_DE) reg|=BIT(7);

    MOD_W2BYTEMSK(REG_MOD_BK00_00_L, reg, 0xFC); // REG3200
}

MS_BOOL MHal_FRC_MOD_SetCtrlSwing(MS_U16 swing_data, MS_U16 u16Swing_Level)
{
    MS_BOOL bStatus = FALSE;
    if(u16Swing_Level>600)
        u16Swing_Level=600;
    if(u16Swing_Level<40)
        u16Swing_Level=40;

    swing_data = _MHal_PNL_MOD_Swing_Refactor_AfterCAL(u16Swing_Level);
    // Separate mode.
    MOD_W2BYTEMSK(REG_MOD_BK00_29_L,
        ( (swing_data << 4 )|(swing_data << 10 ) ) , 0xFFF0);

    MOD_W2BYTE(REG_MOD_BK00_2A_L,
        ((swing_data  )|(swing_data << 6 ) |(swing_data << 12 ) ));

    MOD_W2BYTE(REG_MOD_BK00_2B_L,
        ( (swing_data >> 4  )|(swing_data << 2 ) |(swing_data << 8 ) | (swing_data << 14 )));

    MOD_W2BYTE(REG_MOD_BK00_2C_L,
        ( (swing_data >> 2  )|(swing_data << 4 ) |(swing_data << 10 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_2D_L,
        ( (swing_data  )|(swing_data << 6 ) |(swing_data << 12 ) ));

    MOD_W2BYTE(REG_MOD_BK00_2E_L,
        ( (swing_data >> 4 )|(swing_data << 2 ) |(swing_data << 8 ) |(swing_data << 14 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_2F_L,
        ( (swing_data >> 2  )|(swing_data << 4 ) |(swing_data << 10 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_30_L,
        ( (swing_data  )|(swing_data << 6 ) |(swing_data << 12 ) ));

    MOD_W2BYTEMSK(REG_MOD_BK00_31_L, (swing_data >> 4 ) ,0x0003);

    bStatus = TRUE;

    return bStatus;
}

void MHal_FRC_MOD_SetModTypeParam(MS_U8 u8LPLL_Type,MS_U8 u8LPLL_Mode, MS_U16 u16MOD_CTRL0, MS_U8 u8MOD_CTRLB)
{
    MS_BOOL btimode;
    MS_U8 u8panelBitNums;

printk("\n\n##[%s][%d] CTRL0= %x, u8MOD_CTRLB=%x\n\n", __FUNCTION__, __LINE__, u16MOD_CTRL0, u8MOD_CTRLB);

    btimode = ((u16MOD_CTRL0 & BIT(2)) >> 2);
    u8panelBitNums = (u8MOD_CTRLB & 0x03);
    if (u8LPLL_Type== E_PNL_TYPE_LVDS)
        MHal_FRC_MOD_SetLvdsTiMode(btimode, u8panelBitNums);

    if((u8LPLL_Type == E_PNL_TYPE_LVDS) &&
      ((u8LPLL_Mode == E_PNL_MODE_QUAD) ||(u8LPLL_Mode  == E_PNL_MODE_QUAD_LR)))
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_00_L, BIT(9), BIT(9)); // REG3201 MHal_FRC_MOD_Quad_Mode(ENABLE);
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_00_L, 0, BIT(9)); // REG3201 MHal_FRC_MOD_Quad_Mode(DISABLE);

    }

    MHal_FRC_MOD_SetModTypeEn(u8LPLL_Type, u8LPLL_Mode);
    MHal_FRC_MOD_SetModDataFormat(u8LPLL_Type);

}

// Set LVDS TI Mode and Bit number
void MHal_FRC_MOD_SetLvdsTiMode(MS_BOOL btimode, MS_U8 u8panelBitNums)
{
    MS_U16 u16EnableMode, u16DisableMode;
    MS_U8 bitsNum = HAL_TI_10BIT_MODE;

    u16EnableMode=0;
    u16DisableMode=0;

    if(btimode)
        bitsNum=u8panelBitNums;
    else
        bitsNum=HAL_TI_10BIT_MODE;
    //Tx
    if(btimode)
        u16EnableMode|=FRC_MOD_LVDS_TI;
    else
        u16DisableMode|= FRC_MOD_LVDS_TI;

    MHal_FRC_MOD_LvdsModeSel(u16EnableMode, u16DisableMode );
    //In A5, Bit number setting at [3:2] not [1:0]
    MHal_FRC_MOD_LvdsBitNumSel(bitsNum);
}

void MHal_FRC_MOD_LvdsModeSel(MS_U16 u16FuncEnable, MS_U16 u16FuncDisable)
{
    MS_U16 u16value, u16mask;

    u16FuncEnable  = u16FuncEnable;
    u16FuncDisable = u16FuncDisable;

    //combine value and mask.
    u16value = (u16FuncEnable&(~u16FuncDisable));
    u16mask  = (u16FuncEnable|u16FuncDisable)&
               (FRC_MOD_LVDS_SWAP_PAIR|FRC_MOD_LVDS_SWAP_POLARITY|FRC_MOD_LVDS_PAIR_SHIFT|FRC_MOD_LVDS_TI);

    MOD_W2BYTEMSK(REG_MOD_BK00_05_L, u16value, u16mask); // REG_320B
}

void MHal_FRC_MOD_LvdsBitNumSel(MS_U16 u16BitNum)
{

    if(u16BitNum==HAL_TI_6BIT_MODE) //11
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_05_L, BIT(3)|BIT(2), BIT(3)|BIT(2)); // REG_320A
    }
    else if(u16BitNum==HAL_TI_8BIT_MODE) //10
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_05_L, BIT(3), BIT(3)|BIT(2)); // REG_320A
    }
    else //if(u16BitNum==HAL_TI_10BIT_MODE)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_05_L, 0x00, BIT(3)|BIT(2)); // REG_320A
    }
}

void MHal_FRC_MOD_SetModDataFormat(MS_U8 panelType)
{
    MS_U8 format=0;
    switch(panelType)
    {
        case E_PNL_TYPE_TTL:
        break;
/*
Reg_data_format: BK32, 0x30[14:12]=0x0
*/
        default:
        case E_PNL_TYPE_LVDS:
            format=0;
        break;
    }
    MHal_FRC_MOD_SetDataFormat(format);
}

void MHal_FRC_MOD_SetDataFormat(MS_U16 format)
{
    MOD_W2BYTEMSK(REG_MOD_BK00_30_L, format<<12, 0x7000); // REG_3261
}

void MHal_FRC_MOD_SetModTypeEn(MS_U8 u8LPLL_Type, MS_U8 u8LPLL_Mode)
{
    if(u8LPLL_Type == E_PNL_TYPE_TTL)
    {
        MHal_FRC_MOD_TtlFuncSel(FRC_MOD_TTL_10BIT|FRC_MOD_TTL_CKPOL, 0);
        MHal_FRC_MOD_SetGpoOez(0, 0, 0, 0);
        MHal_FRC_MOD_SetExtGpioEn(0, 0); //Reg_ext_en: BK32, 0x18~0x19=0x0,
        MHal_FRC_MOD_SetTtlPairEn(0xFFFF, 0x3FF); //Reg_ttl_en: BK32, 0x1A=0xFFFF, 0x1B=0x3FF,
    }
    else
    {
        MHal_FRC_MOD_TtlFuncSel(0, FRC_MOD_TTL_10BIT|FRC_MOD_TTL_CKPOL);
        MHal_FRC_MOD_SetGpoOez(0xffff, 0xffff, 0xffff, 0xffff);
        MHal_FRC_MOD_SetExtGpioEn(0xFFFF, 0x3FF);
        MHal_FRC_MOD_SetTtlPairEn(0,0);
    }
    // Func En.
    switch(u8LPLL_Type)
    {
        case E_PNL_TYPE_TTL:
        default:
        case E_PNL_TYPE_LVDS:
        if(u8LPLL_Mode == E_PNL_MODE_QUAD)
        {
            MHal_FRC_MOD_20FuncEn(FRC_MOD_EN_LVDS_LR, FRC_MOD_HF_LVDS| FRC_MOD_EPI | FRC_MOD_MINI);
        }
        else
        {
            MHal_FRC_MOD_20FuncEn(0, FRC_MOD_HF_LVDS | FRC_MOD_EPI | FRC_MOD_MINI);
        }
        break;

    }
}

void MHal_FRC_MOD_TtlFuncSel(MS_U16 u16FuncEnable, MS_U16 u16FuncDisable)
{
    MS_U16 u16value=0;
    MS_U16 u16mask=0;

    //combine value and mask.
    u16value = (u16FuncEnable&(~u16FuncDisable));
    u16mask  = (u16FuncEnable|u16FuncDisable)&(FRC_MOD_TTL_10BIT|FRC_MOD_TTL_CKPOL);

    MOD_W2BYTEMSK(REG_MOD_BK00_05_L, u16value, u16mask); // REG_320A
}

void MHal_FRC_MOD_20FuncEn(MS_U16 u16FuncEnable, MS_U16 u16FuncDisable)
{
    MS_U16 u16value, u16mask;

    //combine value and mask.
    u16value = (u16FuncEnable&(~u16FuncDisable));
    u16mask  = (u16FuncEnable|u16FuncDisable)&(FRC_MOD_HF_LVDS|FRC_MOD_EPI|FRC_MOD_EN_LVDS_LR|FRC_MOD_MINI);

    MOD_W2BYTEMSK(REG_MOD_BK00_20_L, u16value, u16mask); // REG_3241
}
#endif
MS_U16 _MHal_PNL_MOD_Swing_Refactor_AfterCAL(MS_U16 u16Swing_Level)
{
    MS_U8 u8ibcal = 0x00;
    MS_U16 u16AfterCal_value = 0;
    MS_U16 u16Cus_value = 0;


    // =========
    // GCR_CAL_LEVEL[1:0] : REG_MOD_BK00_7D_L =>
    // 2'b00 250mV ' GCR_ICON_CHx[5:0]=2'h15 (decimal 21)
    // 2'b01 350mV ' GCR_ICON_CHx[5:0]=2'h1F (decimal 31)
    // 2'b10 300mV ' GCR_ICON_CHx[5:0]=2'h1A (decimal 26)
    // 2'b11 200mV ' GCR_ICON_CHx[5:0]=2'h10 (decimal 16)
    // =========
    switch(_u8MOD_CALI_TARGET)
    {
        default:
        case 0x00:
            u8ibcal = 0x15;
        break;
        case 0x01:
            u8ibcal = 0x1F;
        break;
        case 0x10:
            u8ibcal = 0x1A;
        break;
        case 0x11:
            u8ibcal = 0x10;
        break;
    }
    u16Cus_value = (u16Swing_Level) * (_u8MOD_CALI_VALUE + 4)/(u8ibcal + 4);
    u16AfterCal_value = (u16Cus_value-40)/10;

    //printf("\r\n--Swing value after refactor = %d\n", u16AfterCal_value);

    return u16AfterCal_value;
}

void MHal_PNL_Init_LPLL(PNL_TYPE eLPLL_Type,PNL_MODE eLPLL_Mode,MS_U64 ldHz)
{
    W2BYTEMSK(BK_LPLL(0x00), 0x00, 0xFF); //swithc LPLL sub bank to 0

    if(eLPLL_Type == E_PNL_TYPE_LVDS)
    {
        W2BYTEMSK(BK_LPLL(0x03), 0, BIT(5));   //[5] reg_lpll_pd
        W2BYTEMSK(BK_LPLL(0x03), 0x0D, 0x1F);  //[4:2]|[1:0] reg_lpll_icp_ictrl|reg_lpll_ibias_ictrl
        W2BYTEMSK(BK_LPLL(0x01), 0x203, 0xF03);  //[11:8]|[1:0] reg_lpll_loop_div_first | reg_lpll_loop_div_second

        if((eLPLL_Type == E_PNL_TYPE_LVDS)&&(eLPLL_Mode == E_PNL_MODE_QUAD))
        {
            if(ldHz > 2000000000)
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x0700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x01<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
            else if((ldHz <=2000000000)&&(ldHz >=1000000000))
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x1700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x02<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
            else
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x2700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x03<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
        }
        else if( (eLPLL_Type == E_PNL_TYPE_LVDS)&&(eLPLL_Mode == E_PNL_MODE_DUAL) )
        {
            if(ldHz > 2000000000)
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x0700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x00<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
            else if((ldHz <=2000000000)&&(ldHz >=1000000000))
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x1700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x01<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
            else
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x2700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x02<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
        }
        else if( (eLPLL_Type == E_PNL_TYPE_LVDS)&&(eLPLL_Mode == E_PNL_MODE_SINGLE) )
        {
            if(ldHz > 2000000000)
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x0700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x04<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
            else if((ldHz <=2000000000)&&(ldHz >=1000000000))
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x1700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x00<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
            else
            {
                W2BYTEMSK(BK_LPLL(0x02), 0x2700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(BK_LPLL(0x35), 0x01<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
        }
        else
        {
            //Add by different app
        }

        W2BYTEMSK(BK_LPLL(0x33), BIT(5), BIT(5));  //[5] reg_lpll2_pd, power down
        W2BYTEMSK(BK_LPLL(0x35), 0, BIT(5));       //[5] reg_lpll_2ndpll_clk_sel
    }
    else
    {
        W2BYTEMSK(BK_LPLL(0x03), 0, BIT(5));   //[5] reg_lpll_pd
        W2BYTEMSK(BK_LPLL(0x03), 0x0D, 0x1F);  //[4:2]|[1:0] reg_lpll_icp_ictrl|reg_lpll_ibias_ictrl
        W2BYTEMSK(BK_LPLL(0x01), 0x203, 0xF03);  //[11:8]|[1:0] reg_lpll_loop_div_first | reg_lpll_loop_div_second

        W2BYTEMSK(BK_LPLL(0x33), BIT(5), BIT(5));  //[5] reg_lpll2_pd, power down
        W2BYTEMSK(BK_LPLL(0x35), 0, BIT(5));       //[5] reg_lpll_2ndpll_clk_sel

/// need to check
        W2BYTEMSK(BK_LPLL(0x33), 0x2E, 0x2E);  //reg_lpll2
        W2BYTEMSK(BK_LPLL(0x30), 0x00, 0xFF03);  // reg_lpll2_input_div_first | reg_lpll2_input_div_second
        W2BYTEMSK(BK_LPLL(0x31), 0x00, 0xFF03);  // reg_lpll2_loop_div_first | reg_lpll2_loop_div_second
        W2BYTEMSK(BK_LPLL(0x32), 0x00, 0x03);  // reg_lpll2_out_div_first
    }

	if( eLPLL_Type == E_PNL_TYPE_TTL)
		MOD_W2BYTEMSK(REG_MOD_BK00_27_L, 0 , BIT(9));	//MOD PVDD=1: 2.5,PVDD=0: 3.3, MHal_MOD_PVDD_Power_Setting
	else
		MOD_W2BYTEMSK(REG_MOD_BK00_27_L, BIT(9), BIT(9));	//MOD PVDD=1: 2.5,PVDD=0: 3.3, MHal_MOD_PVDD_Power_Setting

}

static void _MDrv_PNL_Init_LPLL(PNL_InitData *pstPanelInitData)
{
    MS_U64 ldHz = 1;
    MS_U16 u16DefaultVFreq = 0;

    if(pstPanelInitData->u16DefaultVFreq > 1100)
    {
        u16DefaultVFreq = 1200;
    }
    else if(pstPanelInitData->u16DefaultVFreq > 900)
    {
        u16DefaultVFreq = 1000;
    }
    else if(pstPanelInitData->u16DefaultVFreq > 550)
    {
        u16DefaultVFreq = 600;
    }
    else
    {
        u16DefaultVFreq = 500;
    }
    ldHz = (MS_U64) (((MS_U64)pstPanelInitData->u16HTotal) * pstPanelInitData->u16VTotal * u16DefaultVFreq);
    MHal_PNL_Init_LPLL(pstPanelInitData->eLPLL_Type,pstPanelInitData->eLPLL_Mode,ldHz);

}

static MS_U64 _MDrv_PNL_DClkFactor(void)
{
    return (((MS_U32)LVDS_MPLL_CLOCK_MHZ * 524288 * LPLL_LOOPGAIN));  //216*524288*8=0x36000000
}

static void _MDrv_PNL_Init_Output_Dclk(PNL_InitData *pstPanelInitData)
{
    MS_U32 u32OutputDclk;
    MS_U16 u16DefaultVFreq = 0;
    MS_U64 ldPllSet = 0xff;
    MS_U64 ldHz = 1;
    MS_U32 u32div = 0;

    if(pstPanelInitData->u16DefaultVFreq > 1100)
    {
        u16DefaultVFreq = 1200;
    }
    else if(pstPanelInitData->u16DefaultVFreq > 900)
    {
        u16DefaultVFreq = 1000;
    }
    else if(pstPanelInitData->u16DefaultVFreq > 550)
    {
        u16DefaultVFreq = 600;
    }
    else
    {
        u16DefaultVFreq = 500;
    }
    ldHz = (MS_U64) (((MS_U64)pstPanelInitData->u16HTotal) * pstPanelInitData->u16VTotal * u16DefaultVFreq);
    switch (pstPanelInitData->eLPLL_Type)
    {
        default:
        case E_PNL_TYPE_LVDS: // LVDS
            if (pstPanelInitData->eLPLL_Mode == E_PNL_MODE_DUAL)
            {
                ldPllSet = (_MDrv_PNL_DClkFactor() * 10000000 *2) + ((ldHz * 7) >> 1);
                // do_div(ldPllSet ,(ldHz * 7)) ;    // dual mode
                do_div(ldPllSet, ldHz) ;
                u32div=(MS_U32)(MHal_PNL_Get_Loop_DIV(pstPanelInitData->eLPLL_Mode, pstPanelInitData->eLPLL_Type, ldHz));
                do_div(ldPllSet, u32div);
            }
            else
            {
                ldPllSet = (_MDrv_PNL_DClkFactor() * 10000000) + ((ldHz * 7) >> 1);
                // do_div(ldPllSet, (ldHz * 7)) ;
                do_div(ldPllSet, ldHz) ;
                u32div=(MS_U32)(MHal_PNL_Get_Loop_DIV(pstPanelInitData->eLPLL_Mode, pstPanelInitData->eLPLL_Type, ldHz));
                do_div(ldPllSet, u32div);
            }
            break;
    }
    u32OutputDclk = (MS_U32)ldPllSet;

    //printk("\n_MDrv_PNL_Init_Output_Dclk(): Dclk = %lx\n", u32OutputDclk);

    W2BYTE(BK_LPLL(XC_LPLL_FPLL_ENABLE), FALSE, XC_LPLL_FPLL_ENABLE_MASK);

    W2BYTE(BK_LPLL(XC_LPLL_SET0), (u32OutputDclk & BMASK(15:0)), XC_LPLL_SET0_MASK);

    W2BYTE(BK_LPLL(XC_LPLL_SET1), ((u32OutputDclk >> 16) & BMASK(15:0)), XC_LPLL_SET1_MASK);

}

MS_U8 MHal_PNL_Get_Loop_DIV( MS_U8 u8LPLL_Mode, MS_U8 eLPLL_Type, MS_U32 ldHz)
{
    MS_U16 u16loop_div=0;
    switch(eLPLL_Type)
    {
        case E_PNL_TYPE_LVDS:
        default:
		{
			if(ldHz > 2000000000)
			{
                u16loop_div = 7;
			}
			else if((ldHz<=2000000000)&&(ldHz>=1000000000))
			{
				u16loop_div = 14;
			}
			else
			{
				u16loop_div = 28;
			}
        }
        break;
    }
    u16loop_div *= 2;

    return u16loop_div;
}


PNL_Result MDrv_PNL_SetPanelType(PanelType *pSelPanelType)
{
    PNL_InitData stPanelInitData;
    MS_U8 u8LPLL_Mode;

    // for GOP settings
    panel_hstart = pSelPanelType->m_wPanelHStart;

    // Output timing
    stPanelInitData.u16HStart = pSelPanelType->m_wPanelHStart;      // DE H start
    stPanelInitData.u16VStart = PANEL_DE_VSTART;
    stPanelInitData.u16Width  = pSelPanelType->m_wPanelWidth;
    stPanelInitData.u16Height = pSelPanelType->m_wPanelHeight;
    stPanelInitData.u16HTotal = pSelPanelType->m_wPanelHTotal;
    stPanelInitData.u16VTotal = pSelPanelType->m_wPanelVTotal;

    stPanelInitData.u16DefaultVFreq = (pSelPanelType->m_dwPanelDCLK*10000000)/(pSelPanelType->m_wPanelHTotal*pSelPanelType->m_wPanelVTotal);
    if(stPanelInitData.u16DefaultVFreq > 1100)
    {
        stPanelInitData.u16DefaultVFreq = 1200;
    }
    else if(stPanelInitData.u16DefaultVFreq > 900)
    {
        stPanelInitData.u16DefaultVFreq = 1000;
    }
    else if(stPanelInitData.u16DefaultVFreq > 550)
    {
        stPanelInitData.u16DefaultVFreq = 600;
    }
    else
    {
        stPanelInitData.u16DefaultVFreq = 500;
    }

    // output type
    switch(pSelPanelType->m_ePanelLinkType)
    {
        case LINK_TTL:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_TTL;
            break;

        case LINK_RSDS:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_RSDS;
            break;

        case LINK_MINILVDS:
        case LINK_ANALOG_MINILVDS:
        case LINK_DIGITAL_MINILVDS:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_MINILVDS;
            break;

        case LINK_MFC:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_MFC;
            break;

        case LINK_DAC_I:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_DAC_I;
            break;
        case LINK_DAC_P:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_DAC_P;
            break;

        case LINK_LVDS:
        case LINK_PDPLVDS:
        default:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_LVDS;
            break;
    }
    u8LPLL_Mode = pSelPanelType->m_bPanelDoubleClk ? 1:0;
    stPanelInitData.eLPLL_Mode = u8LPLL_Mode == 0 ? E_PNL_MODE_SINGLE : E_PNL_MODE_DUAL;

    // sync
    stPanelInitData.u8HSyncWidth = pSelPanelType->m_ucPanelHSyncWidth;
    stPanelInitData.u16VSyncStart = pSelPanelType->m_wPanelVStart;
    stPanelInitData.u8VSyncWidth = pSelPanelType->m_ucPanelVSyncWidth;
    switch(pSelPanelType->m_ePanelLinkType)
    {
        case LINK_DAC_I:
        case LINK_DAC_P:
        case LINK_PDPLVDS:
            stPanelInitData.bManuelVSyncCtrl = TRUE;
            break;
        default:
            stPanelInitData.bManuelVSyncCtrl = FALSE;
            break;
    }

    // output control
    stPanelInitData.u16OCTRL = ((MS_U16)((pSelPanelType->m_bPanelDualPort)<<8) | ( pSelPanelType->m_bPanelSwapEven_RB) );
    stPanelInitData.u16OSTRL = ((MS_U16)(0xF7<<8) | ( pSelPanelType->m_ucPanelDCLKDelay<< 4 ) | ( pSelPanelType->m_bPanelInvHSync));
    stPanelInitData.u16ODRV = ((pSelPanelType->m_ucPanelDCKLCurrent<< 6) |
                              ( pSelPanelType->m_ucPanelDECurrent << 4) |
                              ( pSelPanelType->m_ucPanelODDDataCurrent<< 2) |
                              ( pSelPanelType->m_ucPanelEvenDataCurrent));

    // MOD
    stPanelInitData.u16MOD_CTRL0 = (( /*PANEL_DCLK_DELAY*/pSelPanelType->m_ucPanelDCLKDelay & 0xf) << 8)  |
                                   ((/* PANEL_SWAP_LVDS_CH */   pSelPanelType->m_bPanelSwapLVDS_CH)? BIT(6):0) |
                                   ((/* ~PANEL_SWAP_LVDS_POL */ !pSelPanelType->m_bPanelSwapLVDS_POL)?BIT(5):0)|
                                   ((/* PANEL_LVDS_PDP_10BIT */ pSelPanelType->m_bPanelPDP10BIT)?BIT(3):0) |
                                   ((/* PANEL_LVDS_TI_MODE */   pSelPanelType->m_bPanelLVDS_TI_MODE)? BIT(2):0) ;
    stPanelInitData.u16MOD_CTRL9 = ((/*  PANEL_SWAP_EVEN_ML */ pSelPanelType->m_bPanelSwapEven_ML)? BIT(14):0)|
                                ((/* PANEL_SWAP_EVEN_RB */ pSelPanelType->m_bPanelSwapEven_RB)? BIT(13):0)|
                                ((/* PANEL_SWAP_ODD_ML */  pSelPanelType->m_bPanelSwapOdd_ML)? BIT(12):0) |
                                ((/* PANEL_SWAP_ODD_RB */  pSelPanelType->m_bPanelSwapOdd_RB)? BIT(11):0) |
                                ((/* PANEL_OUTPUT_FORMAT */pSelPanelType->m_ucOutputFormatBitMode) << 6 ) |
                                ((/* PANEL_SWAP_EVEN_RG */  pSelPanelType->m_bPanelSwapEven_RG)? BIT(5):0) |
                                ((/* PANEL_SWAP_EVEN_GB */  pSelPanelType->m_bPanelSwapEven_GB)? BIT(4):0) |
                                ((/* PANEL_SWAP_ODD_RG */  pSelPanelType->m_bPanelSwapOdd_RG)? BIT(3):0) |
                                ((/* PANEL_SWAP_ODD_GB */  pSelPanelType->m_bPanelSwapOdd_GB)? BIT(2):0) ;
    stPanelInitData.u16MOD_CTRLA = ((pSelPanelType->m_bPanelInvHSync)? BIT(12):0)   |
                                ((pSelPanelType->m_bPanelInvDCLK)? BIT(4):0)     |
                                ((pSelPanelType->m_bPanelInvVSync)? BIT(3):0)    |
                                ((pSelPanelType->m_bPanelInvDE)? BIT(2):0)       |
                                ((pSelPanelType->m_bPanelDualPort) ? BIT(1):0)   |
                                ((pSelPanelType->m_bPanelSwapPort) ? BIT(0):0);
    stPanelInitData.u8MOD_CTRLB = pSelPanelType->m_ucTiBitMode;

    // Others
    stPanelInitData.u16LVDSTxSwapValue = pSelPanelType->m_u16LVDSTxSwapValue;
    stPanelInitData.u8PanelNoiseDith= pSelPanelType->m_bPanelNoiseDith;
    stPanelInitData.u16DITHCTRL = (pSelPanelType->m_bPanelDither)?0x2D05 : 0x2D00;

#if defined(PRJ_X10) //FRC only
    {
        stPanelInitData.stPNL_Init_Adv.bFRC_En              = _stPnlInitData_adv.bFRC_En;
        stPanelInitData.eLPLL_Mode                          = (PNL_MODE)_stPnlInitData_adv.u8FRC_LPLL_Mode;
        stPanelInitData.stPNL_Init_Adv.u16MOD_SwingLevel    = _stPnlInitData_adv.u16MOD_SwingLevel;
        stPanelInitData.stPNL_Init_Adv.u16MOD_SwingCLK      = _stPnlInitData_adv.u16MOD_SwingCLK;
        stPanelInitData.stPNL_Init_Adv.bdither6Bits         = _stPnlInitData_adv.bdither6Bits;
        stPanelInitData.stPNL_Init_Adv.blvdsShiftPair       = _stPnlInitData_adv.blvdsShiftPair;
        stPanelInitData.stPNL_Init_Adv.blvdsSwapPair        = _stPnlInitData_adv.blvdsSwapPair;
        stPanelInitData.stPNL_Init_Adv.u8lvdsSwapCh         = _stPnlInitData_adv.u8lvdsSwapCh;
        stPanelInitData.stPNL_Init_Adv.bepiLRSwap           = _stPnlInitData_adv.bepiLRSwap;
        stPanelInitData.stPNL_Init_Adv.bepiLMirror          = _stPnlInitData_adv.bepiLMirror;
        stPanelInitData.stPNL_Init_Adv.bepiRMirror          = _stPnlInitData_adv.bepiRMirror;
    }
#endif

#if defined(PRJ_X10P)
       MHal_PNL_Init_XC_Clk(&stPanelInitData);
#else
#if defined(PRJ_X10)
   _MDrv_PNL_Init_XC_Clk();
#endif
#endif

    _MDrv_PNL_Init_XC_VOP(&stPanelInitData);
    _MDrv_PNL_Init_MOD(&stPanelInitData);
    _MDrv_PNL_Init_LPLL(&stPanelInitData);
    _MDrv_PNL_Init_Output_Dclk(&stPanelInitData);

    return E_PNL_OK;
}

//////////////////////////////////////////////////////////////////////
/**
*
* @brief  Initialize the global panel object with a given PanelType
*
* @param[in] pSelPanelType  a given panel type choosed by user.
*
*/
MS_BOOL MApi_PNL_Init(PanelType *pSelPanelType/* <in > */)
{
    MHal_PQ_Clock_Gen_For_Gamma();
    MDrv_PNL_SetPanelType(pSelPanelType);
    return TRUE;
}

void MHal_PNL_SetOutputPattern(MS_BOOL bEnable, MS_U16 u16Red , MS_U16 u16Green, MS_U16 u16Blue)
{
    if (bEnable)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_01_L, BIT(15) , BIT(15));
        MOD_W2BYTEMSK(REG_MOD_BK00_02_L, u16Red , 0x03FF);
        MOD_W2BYTEMSK(REG_MOD_BK00_03_L, u16Green , 0x03FF);
        MOD_W2BYTEMSK(REG_MOD_BK00_04_L, u16Blue , 0x03FF);
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_01_L, DISABLE , BIT(15));
    }

}


/**
*   @brief: _MApi_XC_Sys_Init_Panel
*
*/
E_APIXC_ReturnValue MApi_XC_GetMISCStatus(XC_INITMISC *pXC_Init_Misc);
static void _MApi_XC_Sys_Init_Panel(void)
{
#ifdef PRJ_X10

    XC_INITMISC sXC_Init_Misc;
    // reset to zero
    memset(&sXC_Init_Misc, 0, sizeof(XC_INITMISC));

    MHal_BD_LVDS_Output_Type(BD_LVDS_CONNECT_TYPE);


//#if((CHIP_FAMILY_TYPE!=CHIP_FAMILY_S7LD)&&(CHIP_FAMILY_TYPE!=CHIP_FAMILY_S7J))
// TODO: =====Start Temp function=====//After Interface/Struct definition are fixed, remove these!!



    MApi_XC_GetMISCStatus(&sXC_Init_Misc);
    if(sXC_Init_Misc.u32MISC_A & E_XC_INIT_MISC_A_FRC_INSIDE)
    {
        PNL_InitData_ADV_st stPNL_INIT_ADV;
        memset(&stPNL_INIT_ADV, 0, sizeof(PNL_InitData_ADV_st));

        stPNL_INIT_ADV.bFRC_En                     = ENABLE_FRC;
        stPNL_INIT_ADV.u8FRC_LPLL_Mode             = MOD_OUTPUT_MODE;//2;
        stPNL_INIT_ADV.u16MOD_SwingLevel           = 0x0A;
        stPNL_INIT_ADV.u16MOD_SwingCLK             = 0x0A;
        stPNL_INIT_ADV.bdither6Bits                = 0x00;
        stPNL_INIT_ADV.bepiLRSwap                  = 0x00;
        stPNL_INIT_ADV.bepiLMirror                 = 0x00;
        stPNL_INIT_ADV.bepiRMirror                 = 0x00;

        switch(BD_LVDS_CONNECT_TYPE)
        {

            case 1: //BD_129A Bypass mode
            if(stPNL_INIT_ADV.bFRC_En)
            {

                stPNL_INIT_ADV.blvdsSwapPair    = FALSE;
                stPNL_INIT_ADV.blvdsShiftPair   = FALSE;
                //pstPanelInitData->u16MOD_CTRL0 &= 0xFFDF;
                //for A5 SEC board XC chanel setting
                stPNL_INIT_ADV.u8lvdsSwapCh     = 0x80;
               // printk("[%s][%d] set 0x80 to LVDS chanel \n",__func__,__LINE__);
            }
            else
            {
                stPNL_INIT_ADV.blvdsSwapPair    = FALSE;
                stPNL_INIT_ADV.blvdsShiftPair   = FALSE;
                //pstPanelInitData->u16MOD_CTRL0 &= 0xFFDF;
                //for A5 SEC board XC chanel setting
                stPNL_INIT_ADV.u8lvdsSwapCh     = 0x80;
                //  printk("[%s][%d] set 0x80 to LVDS chanel \n",__func__,__LINE__);

            }
            break;

            case 2: //BD_129B Bypass mode
            if(stPNL_INIT_ADV.bFRC_En)
            {
                stPNL_INIT_ADV.blvdsSwapPair    = TRUE;
                stPNL_INIT_ADV.blvdsShiftPair   = FALSE;
                //pstPanelInitData->u16MOD_CTRL0 &= 0xFF9F;
                stPNL_INIT_ADV.u8lvdsSwapCh     = 0x8E;
            }
            else
            {
                switch(stPNL_INIT_ADV.u8FRC_LPLL_Mode)
                {
                    default:
                    case E_XC_MOD_OUTPUT_DUAL:
                        stPNL_INIT_ADV.blvdsSwapPair   = TRUE;
                        stPNL_INIT_ADV.blvdsShiftPair   = FALSE;
                        //pstPanelInitData->u16MOD_CTRL0 &= 0xFF9F;
                        stPNL_INIT_ADV.u8lvdsSwapCh     = 0x20;
                        break;
                    case E_XC_MOD_OUTPUT_QUAD:
                        stPNL_INIT_ADV.blvdsSwapPair   = TRUE;   // T => 0x3032_05_L[10]=1
                        stPNL_INIT_ADV.blvdsShiftPair   = FALSE; // T => 0x3032_05_L[13]=0
                        //pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF;
                        stPNL_INIT_ADV.u8lvdsSwapCh     = 0x8E;
                        break;
                }
            }
            break;

            case 3: //BD_129C Bypass mode
            if(stPNL_INIT_ADV.bFRC_En)
            {
                stPNL_INIT_ADV.blvdsSwapPair   = FALSE;
                stPNL_INIT_ADV.blvdsShiftPair   = FALSE;
                //pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF;
                stPNL_INIT_ADV.u8lvdsSwapCh     = 0x70;
            }
            else
            {
                switch(stPNL_INIT_ADV.u8FRC_LPLL_Mode)
                {
                    case E_XC_MOD_OUTPUT_DUAL:
                        stPNL_INIT_ADV.blvdsSwapPair   = FALSE;
                        stPNL_INIT_ADV.blvdsShiftPair   = FALSE;
                        //pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF;
                        stPNL_INIT_ADV.u8lvdsSwapCh     = 0x08;
                        break;
                    case E_XC_MOD_OUTPUT_QUAD:
                        stPNL_INIT_ADV.blvdsSwapPair   = FALSE;   // T => 0x3032_05_L[10]=1
                        stPNL_INIT_ADV.blvdsShiftPair   = FALSE; // T => 0x3032_05_L[13]=0
                        //pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF;
                        stPNL_INIT_ADV.u8lvdsSwapCh     = 0x70;
                        break;
                    default:
                    case E_XC_MOD_OUTPUT_SINGLE:
                        stPNL_INIT_ADV.blvdsSwapPair   = FALSE;
                        stPNL_INIT_ADV.blvdsShiftPair   = FALSE;
                        //pstPanelInitData->u16MOD_CTRL0 &= 0xFFBF;
                        stPNL_INIT_ADV.u8lvdsSwapCh     = 0x70;
                        break;
                }
            }
            break;
            default:
                printk("[%s][%d]\n",__func__,__LINE__);
                break;
        }


        MDrv_Init_PNLAdvInfo_beforePNLinit((p_PNL_InitData_ADV_st)(&stPNL_INIT_ADV));
    }

#endif
    MApi_PNL_PreInit( E_PNL_NO_OUTPUT );

#if 1
    MApi_PNL_Init(MApi_XC_GetPanelSpec(g_PNL_TypeSel));  //GetPanelData() is init in readPanelDataFromProfile()
    // Enable LVDS SSC
 #if ENABLE_SSC
     //SSC disable for fixing LED Ripple Noise.
     MDrv_PNL_SetSSC(LVDS_SSC_SPAN_DEFAULT,LVDS_SSC_STEP_DEFAULT,DISABLE);
 #endif
#endif
}

E_APIXC_ReturnValue MApi_XC_GetMISCStatus(XC_INITMISC *pXC_Init_Misc)
{
    if (SUPPORT_IMMESWITCH)
    {
       g_XC_Init_Misc.u32MISC_A |= (E_XC_INIT_MISC_A_IMMESWITCH | E_XC_INIT_MISC_A_IMMESWITCH_DVI_POWERSAVING);
    }

    if (SUPPORT_DVI_AUTO_EQ)
    {
        g_XC_Init_Misc.u32MISC_A |= E_XC_INIT_MISC_A_DVI_AUTO_EQ;
    }

    if(FRC_INSIDE)
    {
        g_XC_Init_Misc.u32MISC_A |= E_XC_INIT_MISC_A_FRC_INSIDE;
    }

    memcpy(pXC_Init_Misc, &g_XC_Init_Misc, sizeof(XC_INITMISC));
    return E_APIXC_RET_OK;
}

/**
*   @brief: MApi_XC_Sys_Init
*
*/
void MApi_XC_Sys_Init(void)
{
    // XC_INITDATA sXC_InitData;

    //-----------------------------------------------
    // Normal Init
    //-----------------------------------------------
    _MApi_XC_Sys_Init_Panel();
}


static void _MDrv_PNL_PowerDownLPLL(MS_BOOL bEnable)
{
    //W2BYTEMSK(L_BK_LPLL(0x00), 0x00, 0xFF);

    if(bEnable)
    {
        W2BYTE(BK_LPLL(0x03), TRUE, 5:5);
    }
    else
    {
        W2BYTE(BK_LPLL(0x03), FALSE, 5:5);
    }
}
#define HBMASK    0xFF00
#define LBMASK    0x00FF

MS_U8 msModCurrentCalibration(void)
{
    MS_U8 u8cur_ibcal=0;
    MS_U8 u8cnt;
    MS_BOOL bdecflag=0,bincflag=0;
    MS_U8 u8steady=0;
    MS_U16 u16reg_3280,u16reg_3282,u16reg_328a;
    MS_U16 u16reg_32da,u16reg_32dc,u16reg_32de;
    MS_U8 u8preibcal=0;

    u16reg_3280 = MOD_R2BYTEMSK(REG_MOD_BK00_40_L, LBMASK);
    u16reg_3282 = MOD_R2BYTEMSK(REG_MOD_BK00_41_L, LBMASK);
    u16reg_328a = MOD_R2BYTEMSK(REG_MOD_BK00_45_L, LBMASK);

    u16reg_32da = MOD_R2BYTE(REG_MOD_BK00_6D_L);
    u16reg_32dc = MOD_R2BYTE(REG_MOD_BK00_6E_L);
    u16reg_32de = MOD_R2BYTE(REG_MOD_BK00_6F_L);

// =========
// GCR_CAL_LEVEL[1:0] : REG_MOD_BK00_7D_L =>
// 2'b00 250mV ' GCR_ICON_CHx[5:0]=2'h15 (decimal 21)
// 2'b01 350mV ' GCR_ICON_CHx[5:0]=2'h1F (decimal 31)
// 2'b10 300mV ' GCR_ICON_CHx[5:0]=2'h1A (decimal 26)
// 2'b11 200mV ' GCR_ICON_CHx[5:0]=2'h10 (decimal 16)
// =========

    switch(_u8MOD_CALI_TARGET)
    {
        default:
        case 0x00:
            u8cur_ibcal = 0x15;
        break;
        case 0x01:
            u8cur_ibcal = 0x1F;
        break;
        case 0x10:
            u8cur_ibcal = 0x1A;
        break;
        case 0x11:
            u8cur_ibcal = 0x10;
        break;
    }

    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L, ((MS_U16)u8cur_ibcal)<<8, 0x3F00); // calibration initialized value

    //Set output config to be test clock output mode
    MOD_W2BYTE(REG_MOD_BK00_6D_L, 0xFFFF);
    MOD_W2BYTE(REG_MOD_BK00_6E_L, 0xFFFF);
    MOD_W2BYTE(REG_MOD_BK00_6F_L, 0xFFFF);

    // White Pattern for Calibration
    MOD_W2BYTEMSK(REG_MOD_BK00_32_L, BIT(15), BIT(15)); // Enable test enable of digi seri
    MOD_W2BYTEMSK(REG_MOD_BK00_41_L, 0x00, 0xFF);       // Set analog testpix output to low

    // Set Calibration target
    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, BIT(2), BIT(3)|BIT(2));  // Select calibration source pair, 01: channel 6
    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, _u8MOD_CALI_TARGET, BIT(1)|BIT(0));    // Select calibration target voltage, 00: 200mv, 01:350mv, 10: 250mv, 11: 150mv
    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, BIT(7), BIT(7));         // Enable calibration function

    //HAL_MOD_CAL_DBG(printf("\r\n [%s](1)Init value:u8cur_ibcal = %d\n", __FUNCTION__, u8cur_ibcal));

    for (u8cnt=0;u8cnt<40;u8cnt++)
    {
        if ((MOD_R2BYTEMSK(REG_MOD_BK00_7D_L, BIT(8))) >> 8)
        {
            u8cur_ibcal--;
            bdecflag = 1;
        }
        else
        {
            u8cur_ibcal++;
            bincflag = 1;
        }

        //HAL_MOD_CAL_DBG(printf("\r\n (2)u8cur_ibcal = %d", u8cur_ibcal));

        MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))<<8, 0x3F00);

        if (bdecflag&&bincflag)
        {
            if (abs(u8preibcal-u8cur_ibcal)<2)
            {
                u8steady++;
            }
            else
            {
                u8steady =0;
            }

            u8preibcal = u8cur_ibcal;

            bdecflag = 0;
            bincflag = 0;
        }

        if (u8steady==3)
            break;

    }

    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, 0x00, BIT(7));  // Disable calibration function
    MOD_W2BYTEMSK(REG_MOD_BK00_32_L, 0x00, BIT(15)); // Disable test enable of digi seri

    MOD_W2BYTEMSK(REG_MOD_BK00_40_L, u16reg_3280, LBMASK);
    MOD_W2BYTEMSK(REG_MOD_BK00_41_L, u16reg_3282, LBMASK);
    MOD_W2BYTEMSK(REG_MOD_BK00_45_L, u16reg_328a, LBMASK);

    MOD_W2BYTE(REG_MOD_BK00_6D_L, u16reg_32da);
    MOD_W2BYTE(REG_MOD_BK00_6E_L, u16reg_32dc);
    MOD_W2BYTE(REG_MOD_BK00_6F_L, u16reg_32de);

    // If the calibration is failed, used the default value
    if ((u8steady==3)&&(u8cnt<=40))
    {
        //HAL_MOD_CAL_DBG(printf("\r\n----- Calibration ok.=> u8cur_ibcal = %d\n", u8cur_ibcal));
    }
    else
    {

        switch(_u8MOD_CALI_TARGET)
        {
            default:
            case 0x00:
                u8cur_ibcal = 0x15;//msReadByte(REG_32FA); //default 6'b100000
            break;
            case 0x01:
                u8cur_ibcal = 0x1F;
            break;
            case 0x10:
                u8cur_ibcal = 0x1A;
            break;
            case 0x11:
                u8cur_ibcal = 0x10;
            break;
        }
        //HAL_MOD_CAL_DBG(printf("\r\n----- Calibration failed.=> u8cur_ibcal = %d\n", u8cur_ibcal));
    }

    //Limit the u8cur_ibcal range
    //u8cur_ibcal [5:0] => 0~63
    if (_usMOD_CALI_OFFSET > 0)
    {
        u8cur_ibcal += (MS_U8)abs(_usMOD_CALI_OFFSET);

        if(u8cur_ibcal >= 63)
        {
            u8cur_ibcal = 63;
        }
    }
    else if (_usMOD_CALI_OFFSET < 0)
    {
        if(u8cur_ibcal < abs(_usMOD_CALI_OFFSET))
        {
            u8cur_ibcal = 0;
        }
        else
        {
            u8cur_ibcal -= (MS_U8)abs(_usMOD_CALI_OFFSET);
        }

    }

    // Store the final value
    //HAL_MOD_CAL_DBG(printf("\r\n (3)Store value = %d\n", u8cur_ibcal));

    _u8MOD_CALI_VALUE = (u8cur_ibcal & 0x3F);

    // copy the valur of ch6 to all channel( 0~20)
    MOD_W2BYTEMSK(REG_MOD_BK00_29_L,((MS_U16)(u8cur_ibcal&0x3F))<<4, 0x3F0);   // ch0
    MOD_W2BYTEMSK(REG_MOD_BK00_29_L,((MS_U16)(u8cur_ibcal&0x3F))<<10, 0xFC00); // ch1

  /*  MOD_W2BYTEMSK(REG_MOD_BK00_2A_L,((MS_U16)(u8cur_ibcal&0x3F)), 0x3F);       // ch2
    MOD_W2BYTEMSK(REG_MOD_BK00_2A_L,((MS_U16)(u8cur_ibcal&0x3F))<<6, 0xFC0);   // ch3
    MOD_W2BYTEMSK(REG_MOD_BK00_2A_L,((MS_U16)(u8cur_ibcal&0x3F))<<12, 0xF000); // ch4
    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))>>4, 0x03);
    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))<<2, 0xFC);    // ch5
    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))<<8, 0x3F00);  // ch6
    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))<<14, 0xC000);   // ch7
    MOD_W2BYTEMSK(REG_MOD_BK00_2C_L,((MS_U16)(u8cur_ibcal&0x3F))>>2, 0x0F);
    MOD_W2BYTEMSK(REG_MOD_BK00_2C_L,((MS_U16)(u8cur_ibcal&0x3F))<<4, 0x3F0);    // ch8
    MOD_W2BYTEMSK(REG_MOD_BK00_2C_L,((MS_U16)(u8cur_ibcal&0x3F))<<10, 0xFC00);    // ch9

    MOD_W2BYTEMSK(REG_MOD_BK00_2D_L,((MS_U16)(u8cur_ibcal&0x3F)), 0x3F);       // ch10
    MOD_W2BYTEMSK(REG_MOD_BK00_2D_L,((MS_U16)(u8cur_ibcal&0x3F))<<6, 0xFC0);   // ch11
    MOD_W2BYTEMSK(REG_MOD_BK00_2D_L,((MS_U16)(u8cur_ibcal&0x3F))<<12, 0xF000); // ch12
    MOD_W2BYTEMSK(REG_MOD_BK00_2E_L,((MS_U16)(u8cur_ibcal&0x3F))>>4, 0x03);
    MOD_W2BYTEMSK(REG_MOD_BK00_2E_L,((MS_U16)(u8cur_ibcal&0x3F))<<2, 0xFC);    // ch13*/
    MOD_W2BYTEMSK(REG_MOD_BK00_2E_L,((MS_U16)(u8cur_ibcal&0x3F))<<8, 0x3F00);  // ch14
    MOD_W2BYTEMSK(REG_MOD_BK00_2E_L,((MS_U16)(u8cur_ibcal&0x3F))<<14, 0xC000);   // ch15
    MOD_W2BYTEMSK(REG_MOD_BK00_2F_L,((MS_U16)(u8cur_ibcal&0x3F))>>2, 0x0F);
    MOD_W2BYTEMSK(REG_MOD_BK00_2F_L,((MS_U16)(u8cur_ibcal&0x3F))<<4, 0x3F0);    // ch16
    MOD_W2BYTEMSK(REG_MOD_BK00_2F_L,((MS_U16)(u8cur_ibcal&0x3F))<<10, 0xFC00);    // ch17

    MOD_W2BYTEMSK(REG_MOD_BK00_30_L,((MS_U16)(u8cur_ibcal&0x3F)), 0x3F);       // ch18
    MOD_W2BYTEMSK(REG_MOD_BK00_30_L,((MS_U16)(u8cur_ibcal&0x3F))<<6, 0xFC0);   // ch19
    MOD_W2BYTEMSK(REG_MOD_BK00_30_L,((MS_U16)(u8cur_ibcal&0x3F))<<12, 0xF000); // ch20
    MOD_W2BYTEMSK(REG_MOD_BK00_31_L,((MS_U16)(u8cur_ibcal&0x3F))>>4, 0x03);

    return MOD_R2BYTEMSK(REG_MOD_BK00_2B_L, 0x3F00);
}


PNL_Result MDrv_PNL_En(MS_BOOL bPanelOn)
{
    MS_U8 u8Cab;
    MS_U8 u8BackUSBPwrStatus;

    if(bPanelOn)
    {
        // The order is PanelVCC -> delay pnlGetOnTiming1() -> VOP -> MOD
        // VOP
        W1BYTE((BK_VOP(0x46)+1), 0x40, 7:0);
        MHal_MOD_PowerOn(ENABLE, E_PNL_TYPE_LVDS);
        // turn on LPLL
        _MDrv_PNL_PowerDownLPLL(FALSE);

        //remove here from SDAL layer.
        //if(bCalEn)
        {
            MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, ((_u8MOD_CALI_TARGET&0x03) << 9), (BIT(10) | BIT(9)));

            u8BackUSBPwrStatus = R2BYTEMSK(L_BK_UTMI1(0x04), BIT(7));

            W2BYTEMSK(L_BK_UTMI1(0x04), 0x00, BIT(7));

            u8Cab = msModCurrentCalibration();

            W2BYTEMSK(L_BK_UTMI1(0x04), u8BackUSBPwrStatus, BIT(7));

            //if(_stPnlInitData.eLPLL_Type !=E_PNL_TYPE_MINILVDS)
            MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, u8Cab, LBMASK);
        }
    }
    else
    {
        // The order is LPLL -> MOD -> VOP -> delay for MOD power off -> turn off VCC

        // LPLL
        _MDrv_PNL_PowerDownLPLL(TRUE);

        MHal_MOD_PowerOn(DISABLE, E_PNL_TYPE_LVDS);
        // VOP
        W1BYTE(BK_VOP(0x46), 0xFF, 7:0);
    }



    return E_PNL_OK;
}

static MS_BOOL _EnablePanel(MS_BOOL bPanelOn)
{
    MS_BOOL bResult = FALSE;

    if ( E_PNL_OK == MDrv_PNL_En(bPanelOn) )
    {
        bResult = TRUE;
    }
    else
    {
        bResult = FALSE;
    }

    return bResult;
}

void MApi_PNL_En(MS_BOOL bPanelOn )
{
    _EnablePanel(bPanelOn);
    //control@onboot
    //MDrv_PWM_Oen(E_PWM_CH3, TRUE);
}

void MApi_PNL_SetOutput(APIPNL_OUTPUT_MODE eOutputMode)
{
    PNL_OUTPUT_MODE ePnlOutputMode;

    switch(eOutputMode)
    {
        case E_APIPNL_OUTPUT_NO_OUTPUT:
            ePnlOutputMode = E_PNL_OUTPUT_NO_OUTPUT;
            break;

        case E_APIPNL_OUTPUT_CLK_ONLY:
            ePnlOutputMode = E_PNL_OUTPUT_CLK_ONLY;
            break;

        case E_APIPNL_OUTPUT_DATA_ONLY:
            ePnlOutputMode = E_PNL_OUTPUT_DATA_ONLY;
            break;

        case E_APIPNL_OUTPUT_CLK_DATA:
        default:
            ePnlOutputMode = E_PNL_OUTPUT_CLK_DATA;
            break;
    }

    MDrv_PNL_SetOutputType(ePnlOutputMode);
    //MDrv_PNL_MISC_Control(m_PanelInfo.ePNL_MISC);
}

static const struct gpio_setting
{
    MS_U32 r_oen;
    MS_U8  m_oen;
    MS_U32 r_out;
    MS_U8  m_out;
    MS_U32 r_in;
    MS_U8  m_in;
} gpio_table[] =
{
#define __GPIO__(_x_)   { CONCAT(CONCAT(GPIO, _x_), _OEN),   \
                          CONCAT(CONCAT(GPIO, _x_), _OUT),   \
                          CONCAT(CONCAT(GPIO, _x_), _IN) }
#define __GPIO(_x_)     __GPIO__(_x_)

//
// !! WARNING !! DO NOT MODIFIY !!!!
//
// These defines order must match following
// 1. the PAD name in GPIO excel
// 2. the perl script to generate the package header file
//
    __GPIO(999), // 0 is not used

    __GPIO(0), __GPIO(1), __GPIO(2), __GPIO(3), __GPIO(4),
    __GPIO(5), __GPIO(6), __GPIO(7), __GPIO(8), __GPIO(9),
    __GPIO(10), __GPIO(11), __GPIO(12), __GPIO(13), __GPIO(14),
    __GPIO(15), __GPIO(16), __GPIO(17), __GPIO(18), __GPIO(19),
    __GPIO(20), __GPIO(21), __GPIO(22), __GPIO(23), __GPIO(24),
    __GPIO(25), __GPIO(26), __GPIO(27), __GPIO(28), __GPIO(29),
    __GPIO(30), __GPIO(31), __GPIO(32), __GPIO(33), __GPIO(34),
    __GPIO(35), __GPIO(36), __GPIO(37), __GPIO(38), __GPIO(39),
    __GPIO(40), __GPIO(41), __GPIO(42), __GPIO(43), __GPIO(44),
    __GPIO(45), __GPIO(46), __GPIO(47), __GPIO(48), __GPIO(49),
    __GPIO(50), __GPIO(51), __GPIO(52), __GPIO(53), __GPIO(54),
    __GPIO(55), __GPIO(56), __GPIO(57), __GPIO(58), __GPIO(59),
    __GPIO(60), __GPIO(61), __GPIO(62), __GPIO(63), __GPIO(64),
    __GPIO(65), __GPIO(66), __GPIO(67), __GPIO(68), __GPIO(69),
    __GPIO(70), __GPIO(71), __GPIO(72), __GPIO(73), __GPIO(74),
    __GPIO(75), __GPIO(76), __GPIO(77), __GPIO(78), __GPIO(79),
    __GPIO(80), __GPIO(81), __GPIO(82), __GPIO(83), __GPIO(84),
    __GPIO(85), __GPIO(86), __GPIO(87), __GPIO(88), __GPIO(89),
    __GPIO(90),
    __GPIO(91), __GPIO(92), __GPIO(93), __GPIO(94),
    __GPIO(95), __GPIO(96), __GPIO(97), __GPIO(98), __GPIO(99),
    __GPIO(100), __GPIO(101), __GPIO(102), __GPIO(103), __GPIO(104),
    __GPIO(105), __GPIO(106), __GPIO(107), __GPIO(108), __GPIO(109),
    __GPIO(110), __GPIO(111), __GPIO(112), __GPIO(113), __GPIO(114),
    __GPIO(115), __GPIO(116), __GPIO(117), __GPIO(118), __GPIO(119),
    __GPIO(120), __GPIO(121), __GPIO(122), __GPIO(123), __GPIO(124),
    __GPIO(125), __GPIO(126), __GPIO(127), __GPIO(128), __GPIO(129),
    __GPIO(130), __GPIO(131), __GPIO(132), __GPIO(133), __GPIO(134),
    __GPIO(135), __GPIO(136), __GPIO(137), __GPIO(138), __GPIO(139),
    __GPIO(140), __GPIO(141), __GPIO(142), __GPIO(143), __GPIO(144),
    __GPIO(145), __GPIO(146), __GPIO(147), __GPIO(148), __GPIO(149),
    __GPIO(150), __GPIO(151), __GPIO(152), __GPIO(153), __GPIO(154),
    __GPIO(155), __GPIO(156), __GPIO(157), __GPIO(158), __GPIO(159),
    __GPIO(160), __GPIO(161), __GPIO(162), __GPIO(163), __GPIO(164),
    __GPIO(165), __GPIO(166), __GPIO(167), __GPIO(168), __GPIO(169),
    __GPIO(170), __GPIO(171), __GPIO(172), __GPIO(173), __GPIO(174),
    __GPIO(175), __GPIO(176), __GPIO(177), __GPIO(178), __GPIO(179),
    __GPIO(180), __GPIO(181), __GPIO(182), __GPIO(183), __GPIO(184),
    __GPIO(185),
};

    int gpio_table_num = sizeof(gpio_table)/sizeof( struct gpio_setting);

void HAL_GPIO_Set_Pad_Mux(MS_GPIO_NUM gpio)
{
    printk("[FastLogo][GPIO][%s][GPIO%d]\n", __FUNCTION__, (int)(gpio));

    if(gpio == 1)
    {
        MDrv_WriteRegBit(0x0e6a, 1, BIT1);
    }
    else if((gpio == 2)||(gpio == 3)||(gpio == 4))
    {
        MDrv_WriteRegBit(0x0e6a, 1, BIT0);
    }
    else if(gpio == 5)
    {
        MDrv_WriteRegBit(0x0e38, 1, BIT4);
    }
    else if(gpio == 6)
    {
        MDrv_WriteRegBit(0x0e38, 1, BIT6);
    }
    else if(gpio == 7)
    {
        MDrv_WriteRegBit(0x0e38, 1, BIT7);
    }
    else if(gpio == 8)
    {
        MDrv_WriteRegBit(0x0e38, 1, BIT5);
    }
    else if((gpio == 10)||(gpio == 16))
    {
        MDrv_WriteRegBit(0x0e6b, 0, BIT1);
    }
    else if(gpio == 13)
    {
        MDrv_Write2Byte(0x0e24, 0xbabe);
    }
    else if(gpio == 15)
    {
        MDrv_WriteRegBit(0x0e6a, 1, BIT2);
    }
    else if(gpio == 19)
    {
        MDrv_WriteRegBit(0x0e6a, 1, BIT3);
    }
    else if((gpio == 32)||(gpio == 33))
    {
        MDrv_WriteRegBit(0x0494, 1, BIT7);
    }
    else if((gpio == 34)||(gpio == 35))
    {
        MDrv_WriteRegBit(0x0496, 1, BIT7);
    }
    else if((gpio == 36)||(gpio == 37))
    {
        MDrv_WriteRegBit(0x0497, 1, BIT7);
    }
    else if((gpio == 38)||(gpio == 39))
    {
        MDrv_WriteRegBit(0x0498, 1, BIT7);
    }
    else if((gpio == 40)||(gpio==41))
    {
        MDrv_WriteRegBit(0x0499, 1, BIT7);
    }
    else if(gpio == 42)
    {
        MDrv_WriteRegBit(0x1422, 0, BIT0);
    }
    else if(gpio == 43)
    {
        MDrv_WriteRegBit(0x1422, 0, BIT1);
    }
    else if(gpio == 44)
    {
        MDrv_WriteRegBit(0x1422, 0, BIT2);
    }
    else if(gpio == 45)
    {
        MDrv_WriteRegBit(0x1422, 0, BIT3);
    }
    else if(gpio == 46)
    {
        MDrv_WriteRegBit(0x1422, 0, BIT4);
    }
    else if((gpio == 66)||(gpio == 67))
    {
        MDrv_WriteRegBit(0x101eb4, 0, BIT4);
    }
    else if((gpio == 109)||(gpio == 110))
    {
        MDrv_WriteRegBit(0x101eaf, 0, BIT6);
    }
    else if(gpio == 147)
    {

    }
    else if(gpio == 149)
    {

    }
    else if(gpio == 170)
    {

    }
    else
    {

    }
}

int mdrv_gpio_get_level(MS_GPIO_NUM gpio)
{
    int r;

    /* Set Pad Mux */
    HAL_GPIO_Set_Pad_Mux(gpio);
    /* set input first */
    MDrv_WriteRegBit(gpio_table[gpio].r_oen, 1, gpio_table[gpio].m_oen);

    if (gpio >= 1 && gpio <= gpio_table_num)
    {
        r = MDrv_ReadRegBit(gpio_table[gpio].r_in, gpio_table[gpio].m_in);
    }
    else
    {
        r = 0;
    }

    return (r != 0);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_DBen
/// @brief \b Function   \b Description : Set the Double buffer of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b MS_BOOL : 1 for enable; 0 for disable
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_DBen(PWM_ChNum index, MS_BOOL bdbenPWM)
{
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
            break;
    }
}

//-------------------------------------------------------------------------------------------------
/// Description:  Set the Double buffer of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bDbenPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Dben(PWM_ChNum u8IndexPWM, MS_BOOL bDbenPWM)
{
    // printk("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bDbenPWM);
    HAL_PWM_DBen(u8IndexPWM, bDbenPWM);
}


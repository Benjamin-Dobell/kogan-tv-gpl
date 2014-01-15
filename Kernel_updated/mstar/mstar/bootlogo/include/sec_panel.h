/**
 * @file sec_panel.h
 * @brief Platform Specific API header file
 */
#ifndef __SEC_PANEL_H__
#define __SEC_PANEL_H__

//#include "sec_panel_b1.h"
/********************************************************************
	TYPE DEFINITION
********************************************************************/
typedef enum _PWM_Result
{
    E_PWM_OK,
    E_PWM_FAIL
} PWM_Result;

typedef enum _PWM_ChNum
{
    E_PWM_CH0,
    E_PWM_CH1,
    E_PWM_CH2,
    E_PWM_CH3,
    E_PWM_CH4,
    E_PWM_CH5,
    E_PWM_CH6,
    E_PWM_CH7,
    E_PWM_CH8,
    E_PWM_CH9
}PWM_ChNum;

typedef enum{
    PANEL_RES_MIN = 0,

    PNL_FULLHD_SEC_LE32A = PANEL_RES_MIN,
    PNL_FULLHD_SEC_CEA,          /*= 1 */
    PNL_FULLHD_SEC_VESA,         /*= 2 */
    // FOR NORMAL LVDS PANEL
    PNL_SXGA_AU17_EN05,          /*= 3 */    // 1280X1024, PNL_AU17_EN05_SXGA
    PNL_WXGA_AU20_T200XW02,      /*= 4 */    // 1366X768,  PNL_AU20_T200XW02_WXGA,
    PNL_WXGA_PLUS_CMO190_M190A1, /*= 5 */    // 1440X900,  PNL_CMO19_M190A1_WXGA, PNL_AU19PW01_WXGA
    PNL_WSXGA_AU22_M201EW01,     /*= 6 */    // 1680X1050, PNL_AU20_M201EW01_WSXGA,
    PNL_FULLHD_CMO216_H1L01,     /*= 7 */    // 1920X1080, PNL_AU37_T370HW01_HD, PNL_CMO216H1_L01_HD.H
    PANEL_RES_FULL_HD,           /*= 8 */    // for auotbuild compatitable
    PNL_WXGA_SEC_LA32C45,        /*= 9 */    // 1366X768,  General SEC panel
    PNL_WXGA_SEC_LTM200KT03,     /*= 10*/    // 1600x900,  General SEC panel
    PNL_WXGA_SEC_LTM230H,        /*= 11*/    // 1920x1080, General SEC panel
    PNL_WXGA_SEC_M185B1,         /*= 12*/    // 1366X768,  General SEC panel // MFM
    PNL_FULLHD_SEC_R,            /*= 13*/    // 1920x1080, SEC panel, Jeida-10 bit, Change htt-only // MFM
    // PDP
    PNL_SEC_PDP_1080P,           /*= 14*/    // 1920x1080, SEC PDP
    PNL_SEC_PDP_WXGA,            /*= 15*/    // 1365x768,  SEC PDP
    PNL_SEC_PDP_XGA,             /*= 16*/    // 1024x768,  SEC PDP
    PNL_RES_MAX_NUM,
}PANEL_RESOLUTION_TYPE;

//==================
// URSA MOD
//==================
#define URSA_TCON_SUPPORT  0

//TTL Mode setting, REG_MOD_BK00_05_L
#define FRC_MOD_TTL_10BIT  BIT(0)  // Support 10bits TTL
#define FRC_MOD_TTL_CKPOL  BIT(1)


//LVDS Mode setting, REG_MOD_BK00_05_L
#define FRC_MOD_LVDS_SWAP_PAIR      BIT(10)
#define FRC_MOD_LVDS_SWAP_POLARITY  BIT(11)
#define FRC_MOD_LVDS_PAIR_SHIFT     BIT(13)  // 0: Shift
#define FRC_MOD_LVDS_TI             BIT(14)  // 0: JEDIA, 1: VESA

//Set Type, REG_MOD_BK00_20_L
#define FRC_MOD_HF_LVDS       BIT(9)
#define FRC_MOD_EPI           BIT(10)
#define FRC_MOD_EN_LVDS_LR    BIT(11)
#define FRC_MOD_MINI          BIT(15)

//Set MOD Power Down, REG_MOD_BK00_28_L
#define FRC_MOD_PD_MODATOP    BIT(0)
#define FRC_MOD_PD_REG_A       BIT(6)  // Power down Regulator CH12~25
#define FRC_MOD_PD_REG_B       BIT(7)  // Power down Regulator CH0~11

//EPI Mode setting, REG_MOD_BK00_3E_L
#define FRC_MOD_EPI_MIRROR_R   BIT(13)
#define FRC_MOD_EPI_MIRROR_L   BIT(14)
#define FRC_MOD_EPI_SWAP_LR    BIT(15)



//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
//#define BIT(_bit_)                  (1 << (_bit_))
#define ALIGN_4(_x_)                (((_x_) + 3) & ~3)
#define ALIGN_8(_x_)                (((_x_) + 7) & ~7)
#define ALIGN_16(_x_)               (((_x_) + 15) & ~15)           // No data type specified, optimized by complier
#define ALIGN_32(_x_)               (((_x_) + 31) & ~31)           // No data type specified, optimized by complier

#define MASK(x)     (((1<<(x##_BITS))-1) << x##_SHIFT)
// #define BIT(_bit_)                  (1 << (_bit_))
#define BIT_(x)                     BIT(x) //[OBSOLETED] //TODO: remove it later
#define BITS(_bits_, _val_)         ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))

#define REG_ALL_PAD_IN              (0x50) //bit 15;set all pads (except SPI) as input
#define REG_PWM_OEN                 (0x03) //bit 0~4

#define REG_PWM_MODE			(0x64)
#define PAD_PWM0_OUT                (BIT2)
#define PAD_PWM1_OUT                (BIT6)
#define PAD_PWM2_OUT                (BIT7)
#define PAD_PWM3_OUT                (BIT12)
#define PAD_PWM4_OUT                (BIT13)

#define REG_PWM0_VDBEN              (0x04)  //bit9
#define REG_PWM1_VDBEN              (0x07)  //bit9
#define REG_PWM2_VDBEN              (0x0A)  //bit9
#define REG_PWM3_VDBEN              (0x0D)  //bit9
#define REG_PWM4_VDBEN              (0x10)  //bit9

#define REG_PWM0_PERIOD             (0x02)  //bit0~15
#define REG_PWM1_PERIOD             (0x05)  //bit0~15
#define REG_PWM2_PERIOD             (0x08)  //bit0~15
#define REG_PWM3_PERIOD             (0x0B)  //bit0~15
#define REG_PWM4_PERIOD             (0x0E)  //bit0~15

#define REG_PWM0_PERIOD_EXT         (0x20)  //bit0~1
#define REG_PWM1_PERIOD_EXT         (0x20)  //bit2~3
#define REG_PWM2_PERIOD_EXT         (0x20)  //bit4~5
#define REG_PWM3_PERIOD_EXT         (0x20)  //bit6~7
#define REG_PWM4_PERIOD_EXT         (0x20)  //bit8~9

#define REG_PWM0_DUTY               (0x03)  //bit0~15
#define REG_PWM1_DUTY               (0x06)  //bit0~15
#define REG_PWM2_DUTY               (0x09)  //bit0~15
#define REG_PWM3_DUTY               (0x0C)  //bit0~15
#define REG_PWM4_DUTY               (0x0F)  //bit0~15

#define REG_PWM0_DUTY_EXT           (0x21)  //bit0~1
#define REG_PWM1_DUTY_EXT           (0x21)  //bit2~3
#define REG_PWM2_DUTY_EXT           (0x21)  //bit4~5
#define REG_PWM3_DUTY_EXT           (0x21)  //bit6~7
#define REG_PWM4_DUTY_EXT           (0x21)  //bit8~9

#define REG_PWM0_DIV                (0x04)  //bit0~7
#define REG_PWM1_DIV                (0x07)  //bit0~7
#define REG_PWM2_DIV                (0x0A)  //bit0~7
#define REG_PWM3_DIV                (0x0D)  //bit0~7
#define REG_PWM4_DIV                (0x10)  //bit0~7

#define REG_PWM0_DIV_EXT            (0x22)  //bit0~7
#define REG_PWM1_DIV_EXT            (0x22)  //bit8~15
#define REG_PWM2_DIV_EXT            (0x23)  //bit0~7
#define REG_PWM3_DIV_EXT            (0x23)  //bit8~15
#define REG_PWM4_DIV_EXT            (0x24)  //bit0~7

#define REG_PWM0_PORARITY           (0x04)  //bit8
#define REG_PWM1_PORARITY           (0x07)  //bit8
#define REG_PWM2_PORARITY           (0x0A)  //bit8
#define REG_PWM3_PORARITY           (0x0D)  //bit8
#define REG_PWM4_PORARITY           (0x10)  //bit8


/// data type hardware physical address
typedef unsigned long               MS_PHYADDR;                         // 32bit physical address

/// Define which panel output timing change mode is used to change VFreq for same panel
typedef enum
{
    E_XC_PNL_CHG_DCLK   = 0,            ///<change output DClk to change Vfreq.
    E_XC_PNL_CHG_HTOTAL = 1,      ///<change H total to change Vfreq.
    E_XC_PNL_CHG_VTOTAL = 2,            ///<change V total to change Vfreq.
} E_XC_PNL_OUT_TIMING_MODE;

typedef enum
{
    E_XC_PNL_LPLL_TTL,                              ///< TTL  type
    E_XC_PNL_LPLL_LVDS,                             ///< LVDS type
    E_XC_PNL_LPLL_RSDS,                             ///< RSDS type
    E_XC_PNL_LPLL_MINILVDS,                         ///< TCON  //E_XC_PNL_LPLL_MINILVDS_6P_2Link
    E_XC_PNL_LPLL_ANALOG_MINILVDS,                  ///< Analog TCON
    E_XC_PNL_LPLL_DIGITAL_MINILVDS,                 ///< Digital TCON
    E_XC_PNL_LPLL_MFC,                              ///< Ursa (TTL output to Ursa)
    E_XC_PNL_LPLL_DAC_I,                            ///< DAC output
    E_XC_PNL_LPLL_DAC_P,                            ///< DAC output

    E_XC_PNL_LPLL_EPI34_8P,
    E_XC_PNL_LPLL_EPI28_8P,
    E_XC_PNL_LPLL_EPI34_6P,
    E_XC_PNL_LPLL_EPI28_6P,

//    E_XC_PNL_LPLL_MINILVDS_6P,             //replace this with E_XC_PNL_LPLL_MINILVDS
    E_XC_PNL_LPLL_MINILVDS_5P_2Link,
    E_XC_PNL_LPLL_MINILVDS_4P_2Link,
    E_XC_PNL_LPLL_MINILVDS_3P_2Link,

    E_XC_PNL_LPLL_MINILVDS_6P_Link,
    E_XC_PNL_LPLL_MINILVDS_5P_Link,
    E_XC_PNL_LPLL_MINILVDS_4P_Link,
    E_XC_PNL_LPLL_MINILVDS_3P_Link,

}E_XC_PNL_LPLL_TYPE;

/// Define panel information
typedef struct
{
    // XC need below information do to frame lock

    MS_U16 u16HStart;           ///<DE H start

    MS_U16 u16VStart;           ///<DE V start

    MS_U16 u16Width;            ///<DE H width

    MS_U16 u16Height;           ///< DE V height

    MS_U16 u16HTotal;           ///<H total

    MS_U16 u16VTotal;           ///<V total

    MS_U16 u16DefaultVFreq;     ///<Panel output Vfreq., used in free run

    MS_U8  u8LPLL_Mode;         ///<0: single mode, 1: dual mode
    E_XC_PNL_OUT_TIMING_MODE enPnl_Out_Timing_Mode;    ///<Define which panel output timing change mode is used to change VFreq for same panel

    // Later need to refine to use Min/Max SET for PDP panel, but for LCD, it maybe no need to check the Min/Max SET
    MS_U16 u16DefaultHTotal;    ///<default H total
    MS_U16 u16DefaultVTotal;    ///<default V total

    MS_U32 u32MinSET;
    MS_U32 u32MaxSET;
    E_XC_PNL_LPLL_TYPE  eLPLL_Type;         ///
} XC_PANEL_INFO;

/// Define the initial data for XC
typedef struct
{
    MS_U32 u32XC_version;                   ///<Version of current structure.
    // system related
    MS_U32 u32XTAL_Clock;                   ///<Crystal clock in Hz

    // frame buffer related
    MS_PHYADDR u32Main_FB_Start_Addr;       ///<scaler main window frame buffer start address, absolute without any alignment
    MS_U32 u32Main_FB_Size;                 ///<scaler main window frame buffer size, the unit is BYTES
    MS_PHYADDR u32Sub_FB_Start_Addr;        ///<scaler sub window frame buffer start address, absolute without any alignment
    MS_U32 u32Sub_FB_Size;                  ///<scaler sub window frame buffer size, the unit is BYTES

    // HDMI related, will be moved to HDMI module
    MS_BOOL bCEC_Use_Interrupt;             ///<CEC use interrupt or not, if not, will use polling

    // This is related to chip package. ( Share Ground / Non-Share Ground )
    MS_BOOL bIsShareGround;

    // function related
    MS_BOOL bEnableIPAutoCoast;             ///<Enable IP auto coast

    MS_BOOL bMirror;                        ///<mirror mode

    // panel related
    XC_PANEL_INFO stPanelInfo;              ///<panel infomation

    // DLC
    MS_BOOL bDLC_Histogram_From_VBlank;     ///<If set to TRUE, the Y max/min report value is read from V blanking area

    // This is related to layout
    MS_U16 eScartIDPort_Sel;    ///<This is port selection (E_XC_SCART_ID_PORT) of Scart ID pin 8
} XC_INITDATA;

typedef enum
{
    E_PNL_FAIL = 0,
    E_PNL_OK = 1,
    E_PNL_GET_BASEADDR_FAIL,            ///< get base address failed when initialize panel driver
    E_PNL_OBTAIN_MUTEX_FAIL,            ///< obtain mutex timeout when calling this function
} PNL_Result;

typedef enum
{
    E_PNL_OUTPUT_NO_OUTPUT = 0,     ///< even called g_IPanel.Enable(TRUE), still no physical output
    E_PNL_OUTPUT_CLK_ONLY,          ///< after called g_IPanel.Enable(TRUE), will output clock only
    E_PNL_OUTPUT_DATA_ONLY,         ///< after called g_IPanel.Enable(TRUE), will output data only
    E_PNL_OUTPUT_CLK_DATA,          ///< after called g_IPanel.Enable(TRUE), will output clock and data
    E_PNL_OUTPUT_MAX,          ///< after called g_IPanel.Enable(TRUE), will output clock and data
} PNL_OUTPUT_MODE;

typedef enum
{
    E_PNL_NO_OUTPUT,
    E_PNL_CLK_ONLY,
    E_PNL_CLK_DATA,
    E_PNL_MAX,
} E_PNL_PREINIT_OPTIONS;

/// Define PANEL Signaling Type
typedef enum
{
    LINK_TTL,                              ///< TTL  type
    LINK_LVDS,                             ///< LVDS type
    LINK_RSDS,                             ///< RSDS type
    LINK_MINILVDS,                         ///< TCON
    LINK_ANALOG_MINILVDS,                  ///< Analog TCON
    LINK_DIGITAL_MINILVDS,                 ///< Digital TCON
    LINK_MFC,                              ///< Ursa (TTL output to Ursa)
    LINK_DAC_I,                            ///< DAC output
    LINK_DAC_P,                            ///< DAC output
    LINK_PDPLVDS,                          ///< For PDP(Vsync use Manually MODE)
}APIPNL_LINK_TYPE;

/// Define aspect ratio
typedef enum
{
    E_PNL_ASPECT_RATIO_4_3    = 0,         ///< set aspect ratio to 4 : 3
    E_PNL_ASPECT_RATIO_WIDE,               ///< set aspect ratio to 16 : 9
    E_PNL_ASPECT_RATIO_OTHER,              ///< resvered for other aspect ratio other than 4:3/ 16:9
}E_PNL_ASPECT_RATIO;

/// Define TI bit mode
typedef enum
{
    TI_10BIT_MODE = 0,
    TI_8BIT_MODE = 2,
    TI_6BIT_MODE = 3,
} APIPNL_TIBITMODE;

/// Define panel output format bit mode
typedef enum
{
    OUTPUT_10BIT_MODE = 0,//default is 10bit, becasue 8bit panel can use 10bit config and 8bit config.
    OUTPUT_6BIT_MODE = 1, //but 10bit panel(like PDP panel) can only use 10bit config.
    OUTPUT_8BIT_MODE = 2, //and some PDA panel is 6bit.
} APIPNL_OUTPUTFORMAT_BITMODE;

/// Define which panel output timing change mode is used to change VFreq for same panel
typedef enum
{
    E_PNL_CHG_DCLK   = 0,      ///<change output DClk to change Vfreq.
    E_PNL_CHG_HTOTAL = 1,      ///<change H total to change Vfreq.
    E_PNL_CHG_VTOTAL = 2,      ///<change V total to change Vfreq.
} APIPNL_OUT_TIMING_MODE;

typedef enum
{
    LVDS_FMT_VESA_8,
    LVDS_FMT_JEIDA_8,
    LVDS_FMT_JEIDA_10,
    LVDS_FMT_PDP_8,
    LVDS_FMT_PDP_10,
    LVDS_FMT_NS_10,
    LVDS_FMT_MAX
} LvdsFmt_k;

/// A panel struct type used to specify the panel attributes, and settings from Board layout
typedef struct
{
    const char *m_pPanelName;                ///<  PanelName
    //
    //  Panel output
    //
    MS_U8 m_bPanelDither :1;                 ///<  PANEL_DITHER, keep the setting
    APIPNL_LINK_TYPE m_ePanelLinkType   :4;  ///<  PANEL_LINK

    ///////////////////////////////////////////////
    // Board related setting
    ///////////////////////////////////////////////
    MS_U8 m_bPanelDualPort  :1;              ///<  VOP_21[8], MOD_4A[1],    PANEL_DUAL_PORT, refer to m_bPanelDoubleClk
    MS_U8 m_bPanelSwapPort  :1;              ///<  MOD_4A[0],               PANEL_SWAP_PORT, refer to "LVDS output app note" A/B channel swap
    MS_U8 m_bPanelSwapOdd_ML    :1;          ///<  PANEL_SWAP_ODD_ML
    MS_U8 m_bPanelSwapEven_ML   :1;          ///<  PANEL_SWAP_EVEN_ML
    MS_U8 m_bPanelSwapOdd_RB    :1;          ///<  PANEL_SWAP_ODD_RB
    MS_U8 m_bPanelSwapEven_RB   :1;          ///<  PANEL_SWAP_EVEN_RB

    MS_U8 m_bPanelSwapLVDS_POL  :1;          ///<  MOD_40[5], PANEL_SWAP_LVDS_POL, for differential P/N swap
    MS_U8 m_bPanelSwapLVDS_CH   :1;          ///<  MOD_40[6], PANEL_SWAP_LVDS_CH, for pair swap
    MS_U8 m_bPanelPDP10BIT      :1;          ///<  MOD_40[3], PANEL_PDP_10BIT ,for pair swap
    MS_U8 m_bPanelLVDS_TI_MODE  :1;          ///<  MOD_40[2], PANEL_LVDS_TI_MODE, refer to "LVDS output app note"

    ///////////////////////////////////////////////
    // For TTL Only
    ///////////////////////////////////////////////
    MS_U8 m_ucPanelDCLKDelay;                ///<  PANEL_DCLK_DELAY
    MS_U8 m_bPanelInvDCLK   :1;              ///<  MOD_4A[4],                   PANEL_INV_DCLK
    MS_U8 m_bPanelInvDE     :1;              ///<  MOD_4A[2],                   PANEL_INV_DE
    MS_U8 m_bPanelInvHSync  :1;              ///<  MOD_4A[12],                  PANEL_INV_HSYNC
    MS_U8 m_bPanelInvVSync  :1;              ///<  MOD_4A[3],                   PANEL_INV_VSYNC

    ///////////////////////////////////////////////
    // Output driving current setting
    ///////////////////////////////////////////////
    // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
    MS_U8 m_ucPanelDCKLCurrent;              ///<  define PANEL_DCLK_CURRENT
    MS_U8 m_ucPanelDECurrent;                ///<  define PANEL_DE_CURRENT
    MS_U8 m_ucPanelODDDataCurrent;           ///<  define PANEL_ODD_DATA_CURRENT
    MS_U8 m_ucPanelEvenDataCurrent;          ///<  define PANEL_EVEN_DATA_CURRENT

    ///////////////////////////////////////////////
    // panel on/off timing
    ///////////////////////////////////////////////
    MS_U16 m_wPanelOnTiming1;                ///<  time between panel & data while turn on power
    MS_U16 m_wPanelOnTiming2;                ///<  time between data & back light while turn on power
    MS_U16 m_wPanelOffTiming1;               ///<  time between back light & data while turn off power
    MS_U16 m_wPanelOffTiming2;               ///<  time between data & panel while turn off power

    ///////////////////////////////////////////////
    // panel timing spec.
    ///////////////////////////////////////////////
    // sync related
    MS_U8 m_ucPanelHSyncWidth;               ///<  VOP_01[7:0], PANEL_HSYNC_WIDTH
    MS_U8 m_ucPanelHSyncBackPorch;           ///<  PANEL_HSYNC_BACK_PORCH, no register setting, provide value for query only

                                             ///<  not support Manuel VSync Start/End now
                                             ///<  VOP_02[10:0] VSync start = Vtt - VBackPorch - VSyncWidth
                                             ///<  VOP_03[10:0] VSync end = Vtt - VBackPorch
    MS_U8 m_ucPanelVSyncWidth;               ///<  define PANEL_VSYNC_WIDTH
    MS_U8 m_ucPanelVBackPorch;               ///<  define PANEL_VSYNC_BACK_PORCH

    // DE related
    MS_U16 m_wPanelHStart;                   ///<  VOP_04[11:0], PANEL_HSTART, DE H Start (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
    MS_U16 m_wPanelVStart;                   ///<  VOP_06[11:0], PANEL_VSTART, DE V Start
    MS_U16 m_wPanelWidth;                    ///< PANEL_WIDTH, DE width (VOP_05[11:0] = HEnd = HStart + Width - 1)
    MS_U16 m_wPanelHeight;                   ///< PANEL_HEIGHT, DE height (VOP_07[11:0], = Vend = VStart + Height - 1)

    // DClk related
    MS_U16 m_wPanelMaxHTotal;                ///<  PANEL_MAX_HTOTAL. Reserved for future using.
    MS_U16 m_wPanelHTotal;                   ///<  VOP_0C[11:0], PANEL_HTOTAL
    MS_U16 m_wPanelMinHTotal;                ///<  PANEL_MIN_HTOTAL. Reserved for future using.

    MS_U16 m_wPanelMaxVTotal;                ///<  PANEL_MAX_VTOTAL. Reserved for future using.
    MS_U16 m_wPanelVTotal;                   ///<  VOP_0D[11:0], PANEL_VTOTAL
    MS_U16 m_wPanelMinVTotal;                ///<  PANEL_MIN_VTOTAL. Reserved for future using.

    MS_U8 m_dwPanelMaxDCLK;                  ///<  PANEL_MAX_DCLK. Reserved for future using.
    MS_U8 m_dwPanelDCLK;                     ///<  LPLL_0F[23:0], PANEL_DCLK          ,{0x3100_10[7:0], 0x3100_0F[15:0]}
    MS_U8 m_dwPanelMinDCLK;                  ///<  PANEL_MIN_DCLK. Reserved for future using.

                                             ///<  spread spectrum
    MS_U16 m_wSpreadSpectrumStep;            ///<  move to board define, no use now.
    MS_U16 m_wSpreadSpectrumSpan;            ///<  move to board define, no use now.

    MS_U8 m_ucDimmingCtl;                    ///<  Initial Dimming Value
    MS_U8 m_ucMaxPWMVal;                     ///<  Max Dimming Value
    MS_U8 m_ucMinPWMVal;                     ///<  Min Dimming Value

    MS_U8 m_bPanelDeinterMode   :1;          ///<  define PANEL_DEINTER_MODE,  no use now
    E_PNL_ASPECT_RATIO m_ucPanelAspectRatio; ///<  Panel Aspect Ratio, provide information to upper layer application for aspect ratio setting.
  /*
    *
    * Board related params
    *
    *  If a board ( like BD_MST064C_D01A_S ) swap LVDS TX polarity
    *    : This polarity swap value =
    *      (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L from board define,
    *  Otherwise
    *    : The value shall set to 0.
    */
    MS_U16 m_u16LVDSTxSwapValue;
    APIPNL_TIBITMODE m_ucTiBitMode;                         ///< MOD_4B[1:0], refer to "LVDS output app note"
    APIPNL_OUTPUTFORMAT_BITMODE m_ucOutputFormatBitMode;

    MS_U8 m_bPanelSwapOdd_RG    :1;          ///<  define PANEL_SWAP_ODD_RG
    MS_U8 m_bPanelSwapEven_RG   :1;          ///<  define PANEL_SWAP_EVEN_RG
    MS_U8 m_bPanelSwapOdd_GB    :1;          ///<  define PANEL_SWAP_ODD_GB
    MS_U8 m_bPanelSwapEven_GB   :1;          ///<  define PANEL_SWAP_EVEN_GB

    /**
    *  Others
    */
    MS_U8 m_bPanelDoubleClk     :1;             ///<  LPLL_03[7], define Double Clock ,LVDS dual mode
    MS_U32 m_dwPanelMaxSET;                     ///<  define PANEL_MAX_SET
    MS_U32 m_dwPanelMinSET;                     ///<  define PANEL_MIN_SET
    APIPNL_OUT_TIMING_MODE m_ucOutTimingMode;   ///<Define which panel output timing change mode is used to change VFreq for same panel
    MS_U8 m_bPanelNoiseDith     :1;             ///<  PAFRC mixed with noise dither disable
} PanelType;

typedef struct
{
    MS_U8  u8LPLL_Type;
    MS_U8  u8LPLL_Mode;

    // MOD
    MS_U16 u16MOD_CTRL0;  // BIT2: tiMode, BIT5: lvdsSwapPol, BIT6: lvdsSwapCh
    MS_U8  u8MOD_CTRL2;
    MS_U16 u16MOD_CTRL9;
    MS_U16 u16MOD_CTRLA;  // BIT2: invertDE, BIT3: invertVS, BIT12: invertHS
    MS_U8  u8MOD_CTRLB;   // BIT0~1: panelBitNums

//-------------------------------------------------------------------------------------------------
// FRC Control
//-------------------------------------------------------------------------------------------------
    MS_BOOL bFRC_En;
    MS_U16 u16MOD_SwingLevel;
    MS_U16 u16MOD_SwingCLK;
    MS_U8 u8PanelNoiseDith;
    MS_U8 u8lvdsSwapCh;
    MS_BOOL bdither6Bits;
    MS_BOOL blvdsShiftPair;
    MS_BOOL blvdsSwapPair;

// EPI
    MS_BOOL bepiLRSwap;
    MS_BOOL bepiLMirror;
    MS_BOOL bepiRMirror;
} MST_PANEL_FRC_INFO_t, *PMST_PANEL_FRC_INFO_t;


typedef enum
{
    E_PNL_TYPE_TTL,
    E_PNL_TYPE_LVDS,
    E_PNL_TYPE_RSDS,
    E_PNL_TYPE_MINILVDS,                 // <--  E_XC_PNL_LPLL_MINILVDS_6P,
    E_PNL_TYPE_MFC,
    E_PNL_TYPE_DAC_I,
    E_PNL_TYPE_DAC_P,
} PNL_TYPE;

typedef enum
{
    E_PNL_MODE_SINGLE = 0,          ///< single channel
    E_PNL_MODE_DUAL = 1,            ///< dual channel

    E_PNL_MODE_QUAD = 2,            ///< quad channel
    E_PNL_MODE_QUAD_LR = 3,            ///< quad channel
} PNL_MODE;



/// Panel output control, must be called before g_IPanel.Enable(), otherwise will output after called g_IPanel.Enable()
typedef enum
{
    E_APIPNL_OUTPUT_NO_OUTPUT = 0,     ///< even called g_IPanel.Enable(TRUE), still no physical output
    E_APIPNL_OUTPUT_CLK_ONLY,          ///< after called g_IPanel.Enable(TRUE), will output clock only
    E_APIPNL_OUTPUT_DATA_ONLY,         ///< after called g_IPanel.Enable(TRUE), will output data only
    E_APIPNL_OUTPUT_CLK_DATA,          ///< after called g_IPanel.Enable(TRUE), will output clock and data
} APIPNL_OUTPUT_MODE;

// panel related
typedef struct
{
    // Output timing
    MS_U16 u16HStart;               ///< DE H start
    MS_U16 u16VStart;               ///< DE V start
    MS_U16 u16Width;                ///< DE H width
    MS_U16 u16Height;               ///< DE V height
    MS_U16 u16HTotal;               ///< H total
    MS_U16 u16VTotal;               ///< V total

    MS_U16 u16DefaultVFreq;         ///< Panel output Vfreq., used in free run

    // output type
    PNL_TYPE eLPLL_Type;            ///< 0: LVDS type, 1: RSDS type
    PNL_MODE eLPLL_Mode;            ///< 0: single mode, 1: dual mode

    // sync
    MS_U8  u8HSyncWidth;            ///< H sync width
    MS_U16 u16VSyncStart;           ///< V sync start = Vtotal - backporch - VSyncWidth
    MS_U8  u8VSyncWidth;            ///< V sync width
    MS_BOOL bManuelVSyncCtrl;       ///< enable manuel V sync control

    // output control
    MS_U16 u16OCTRL;                ///< Output control such as Swap port, etc.
    MS_U16 u16OSTRL;                ///< Output control sync as Invert sync/DE, etc.
    MS_U16 u16ODRV;                 ///< Driving current
    MS_U16 u16DITHCTRL;             ///< Dither control

    // MOD
    MS_U16 u16MOD_CTRL0;            ///< MOD_REG(0x40), PANEL_DCLK_DELAY:8, PANEL_SWAP_LVDS_CH:6, PANEL_SWAP_LVDS_POL:5, PANEL_LVDS_TI_MODE:2,
    MS_U16 u16MOD_CTRL9;            ///< MOD_REG(0x49), PANEL_SWAP_EVEN_ML:14, PANEL_SWAP_EVEN_RB:13, PANEL_SWAP_ODD_ML:12, PANEL_SWAP_ODD_RB:11, [7,6] : output formate selction 10: 8bit, 01: 6bit :other 10bit
    MS_U16 u16MOD_CTRLA;            ///< MOD_REG(0x4A), PANEL_INV_HSYNC:12, PANEL_INV_DCLK:4, PANEL_INV_VSYNC:3, PANEL_INV_DE:2, PANEL_DUAL_PORT:1, PANEL_SWAP_PORT:0,
    MS_U8  u8MOD_CTRLB;             ///< MOD_REG(0x4B), [1:0]ti_bitmode=00(10bit)

    // Other
    MS_U16 u16LVDSTxSwapValue;      ///< swap PN setting
    MS_U8  u8PanelNoiseDith;        ///<  PAFRC mixed with noise dither disable
    MS_U32 u32PNL_MISC;
    MS_U16 u16OutputCFG0_7;
    MS_U16 u16OutputCFG8_15;
    MS_U16 u16OutputCFG16_21;

    //FRC
    PNL_InitData_ADV_st stPNL_Init_Adv;
} PNL_InitData;

#define NON_INVERSE     0
#define INVERSE         1

#define DISABLE                     0
#define ENABLE                      1

// SCALER PAFRC Bank registers
#define XC_BK_PAFRC                     (0x24)
#define XC_PAFRC_DITH                   (0x3F)
#define XC_PAFRC_DITH_MASK              0:0
    #define XC_PAFRC_DITH_ON_6BITS              (0x01)
    #define XC_PAFRC_DITH_ON_8BITS              (0x01)
    #define XC_PAFRC_DITH_ON_10BITS             (0x00)

#define XC_PAFRC_DITH_BITS_MASK         2:2
    #define XC_PAFRC_DITH_BITS_6BIT             (0x01)
    #define XC_PAFRC_DITH_BITS_8BIT             (0x00)
    #define XC_PAFRC_DITH_BITS_10BIT            (0x00)      // usually 10bit no need dither, don't care the setting here

#define XC_PAFRC_DITH_NOISEDITH_EN_MASK 3:3
    #define XC_PAFRC_DITH_NOISEDITH_EN          (0x00)

#define XC_PAFRC_DITH_TAILCUT_MASK      4:4
    #define XC_PAFRC_DITH_TAILCUT_DISABLE       (0x00)

typedef enum
{
    HAL_TI_10BIT_MODE = 0,
    HAL_TI_8BIT_MODE = 2,
    HAL_TI_6BIT_MODE = 3,
} PNL_HAL_TIMODES;

#define PNL_HAL_TIMODE_MASK 1:0

#define XC_MOD_PATTERN              (0x01)
#define XC_MOD_PATTERN_ENABLE       15:15

#define XC_MOD_PATTERN_RED          (0x02)
#define XC_MOD_PATTERN_RED_MASK     9:0
#define XC_MOD_PATTERN_GREEN        (0x03)
#define XC_MOD_PATTERN_GREEN_MASK   9:0
#define XC_MOD_PATTERN_BLUE         (0x04)
#define XC_MOD_PATTERN_BLUE_MASK    9:0

#define XC_MOD_CTRL0                (0x40)
#define XC_MOD_CTRL0_MASK           15:0

#define XC_MOD_SYNC_FOR_DUAL_MODE        (0x44)
#define XC_MOD_SYNC_FOR_DUAL_MODE_MASK   12:12

#define XC_MOD_CTRL9                (0x49)
#define XC_MOD_CTRL9_MASK           15:0

#define XC_MOD_CTRLA                (0x4A)
#define XC_MOD_CTRLA_MASK           15:0

#define XC_MOD_CTRLB                (0x4B)
#define XC_MOD_CTRLB_MASK           15:0

#define SUPPORT_SYNC_FOR_DUAL_MODE			TRUE  //New feature after T7

#define XC_MOD_LVDS_TX_SWAP_PN      (0x73)
#define XC_MOD_LVDS_TX_SWAP_PN_MASK 15:0

#define XC_MOD_POWER_DOWN           (0x78)
#define XC_MOD_POWER_DOWN_MASK       0:0

#define XC_MOD_EN_DIFFSIG_PORTA      (0x77)
#define XC_MOD_EN_DIFFSIG_PORTA_MASK 1:1
#define XC_MOD_EN_DIFFSIG_CLK_MASK   0:0

#define XC_MOD_OUTPUT_CONF_L                (0x6D)
#define XC_MOD_OUTPUT_CONF_L_MASK           15:0

#define XC_MOD_OUTPUT_CONF_L_LVDS_MD1_MSK  (0xFFC0)
#define XC_MOD_OUTPUT_CONF_L_LVDS_MD2_MSK  (0xFFF0)
#define XC_MOD_OUTPUT_CONF_L_LVDS_MD3_MSK  (0xF000)
#define XC_MOD_OUTPUT_CONF_L_LVDS_MD4_MSK  (0xFFFF)


#define XC_MOD_OUTPUT_CONF_L_LVDS_MD1_VAL  (0x5540)
#define XC_MOD_OUTPUT_CONF_L_LVDS_MD2_VAL  (0x5550)
#define XC_MOD_OUTPUT_CONF_L_LVDS_MD3_VAL  (0x5000)
#define XC_MOD_OUTPUT_CONF_L_LVDS_MD3_CLKONLY_VAL   (0x0000)
#define XC_MOD_OUTPUT_CONF_L_LVDS_MD4_VAL  (0x1555)

#define XC_MOD_OUTPUT_CONF_L_TTL_MSK        (0xFFFF)
#define XC_MOD_OUTPUT_CONF_L_TTL_VAL        (0x0000)


#define XC_MOD_OUTPUT_CONF_M                (0x6E)
#define XC_MOD_OUTPUT_CONF_M_MASK           15:0

#define XC_MOD_OUTPUT_CONF_M_LVDS_MD1_MSK   (0x3FFF)
#define XC_MOD_OUTPUT_CONF_M_LVDS_MD2_MSK   (0x0FFF)
#define XC_MOD_OUTPUT_CONF_M_LVDS_MD3_MSK   (0xFFFF)
#define XC_MOD_OUTPUT_CONF_M_LVDS_MD4_MSK   (0xFFFF)

#define XC_MOD_OUTPUT_CONF_M_LVDS_MD1_VAL   (0x1555)
#define XC_MOD_OUTPUT_CONF_M_LVDS_MD2_VAL   (0x0555)
#define XC_MOD_OUTPUT_CONF_M_LVDS_MD3_VAL   (0x5555)
#define XC_MOD_OUTPUT_CONF_M_LVDS_MD4_VAL   (0x1554)
#define XC_MOD_OUTPUT_CONF_M_LVDS_MD3_CLKONLY_VAL   (0x4004)            // pair 9 & 15 is clock

#define XC_MOD_OUTPUT_CONF_M_TTL_MSK        (0xFFFF)
#define XC_MOD_OUTPUT_CONF_M_TTL_VAL        (0x0000)


#define XC_MOD_OUTPUT_CONF_H                (0x6F)
#define XC_MOD_OUTPUT_CONF_H_MASK           15:0

#define XC_MOD_OUTPUT_CONF_H_LVDS_MD1_MSK   (0x0000)
#define XC_MOD_OUTPUT_CONF_H_LVDS_MD2_MSK   (0x0000)
#define XC_MOD_OUTPUT_CONF_H_LVDS_MD3_MSK   (0x000F)
#define XC_MOD_OUTPUT_CONF_H_LVDS_MD4_MSK   (0x000F)

#define XC_MOD_OUTPUT_CONF_H_LVDS_MD1_VAL   (0x0000)
#define XC_MOD_OUTPUT_CONF_H_LVDS_MD2_VAL   (0x0000)
#define XC_MOD_OUTPUT_CONF_H_LVDS_MD3_VAL   (0x0005)
#define XC_MOD_OUTPUT_CONF_H_LVDS_MD4_VAL   (0x0000)
#define XC_MOD_OUTPUT_CONF_H_LVDS_MD3_CLKONLY_VAL   (0x0000)

#define XC_MOD_OUTPUT_CONF_H_TTL_MSK        (0xEFFF)
#define XC_MOD_OUTPUT_CONF_H_TTL_VAL        (0x0000)

#define XC_MOD_EXT_DATA_EN_L                (0x46)

#define XC_MOD_EXT_DATA_EN_L_LVDS_MD1_MSK   (0xFFC0)
#define XC_MOD_EXT_DATA_EN_L_LVDS_MD2_MSK   (0xFFF0)
#define XC_MOD_EXT_DATA_EN_L_LVDS_MD3_MSK   (0xF000)

#define XC_MOD_EXT_DATA_EN_L_LVDS_MD1_VAL   (0x0000)
#define XC_MOD_EXT_DATA_EN_L_LVDS_MD2_VAL   (0x0000)
#define XC_MOD_EXT_DATA_EN_L_LVDS_MD3_VAL   (0x0000)

#define XC_MOD_EXT_DATA_EN_L_TTL_MSK        (0xFFFF)
#define XC_MOD_EXT_DATA_EN_L_TTL_VAL        (0x0000)

#define XC_MOD_EXT_DATA_EN_M                (0x47)

#define XC_MOD_EXT_DATA_EN_M_LVDS_MD1_MSK   (0x3FFF)
#define XC_MOD_EXT_DATA_EN_M_LVDS_MD2_MSK   (0x0FFF)
#define XC_MOD_EXT_DATA_EN_M_LVDS_MD3_MSK   (0xFFFF)

#define XC_MOD_EXT_DATA_EN_M_LVDS_MD1_VAL   (0x0000)
#define XC_MOD_EXT_DATA_EN_M_LVDS_MD2_VAL   (0x0000)
#define XC_MOD_EXT_DATA_EN_M_LVDS_MD3_VAL   (0x0000)

#define XC_MOD_EXT_DATA_EN_M_TTL_MSK        (0xFFFF)
#define XC_MOD_EXT_DATA_EN_M_TTL_VAL        (0x0000)


#define XC_MOD_EXT_DATA_EN_H                (0x7E)

#define XC_MOD_EXT_DATA_EN_H_LVDS_MD1_MSK   (0x0000)
#define XC_MOD_EXT_DATA_EN_H_LVDS_MD2_MSK   (0x0000)
#define XC_MOD_EXT_DATA_EN_H_LVDS_MD3_MSK   (0x000F)

#define XC_MOD_EXT_DATA_EN_H_LVDS_MD1_VAL   (0x0000)
#define XC_MOD_EXT_DATA_EN_H_LVDS_MD2_VAL   (0x0000)
#define XC_MOD_EXT_DATA_EN_H_LVDS_MD3_VAL   (0x0000)

#define XC_MOD_EXT_DATA_EN_H_TTL_MSK        (0xE000)
#define XC_MOD_EXT_DATA_EN_H_TTL_VAL        (0x0000)

#define LVDS_DUAL_OUTPUT          0
#define LVDS_DUAL_OUTPUT_SPECIAL  1// only for use with T8 board
#define LVDS_SINGLE_OUTPUT_A      2
#define LVDS_SINGLE_OUTPUT_B      3
#define LVDS_OUTPUT_User          4

// SCALER VOP Bank registers
#define XC_BK_S_VOP                 (0x0F)
#define XC_BK_VOP                   (0x10)

#define XC_VOP_HSEND_WIDTH          (0x01)          ///< H sync end -> H sync width
#define XC_VOP_HSEND_WIDTH_MASK     7:0

#define XC_VOP_VSST                 (0x02)          ///< V sync start = V total - back porch - V sync width
#define XC_VOP_VSST_MASK            11:0

#define XC_VOP_VSEND                (0x03)          ///< V sync end = V total - back porch
#define XC_VOP_VSEND_MASK           11:0

#define XC_VOP_DE_HSTART            (0x04)          ///< DE H start
#define XC_VOP_DE_HSTART_MASK       11:0
#define XC_VOP_DE_HEND              (0x05)          ///< DE H end
#define XC_VOP_DE_HEND_MASK         11:0

#define XC_VOP_DE_VSTART            (0x06)          ///< DE V start
#define XC_VOP_DE_VSTART_MASK       11:0
#define XC_VOP_DE_VEND              (0x07)          ///< DE V end
#define XC_VOP_DE_VEND_MASK         11:0

#define XC_VOP_SIHST                (0x08)          ///< Scaling image window H start
#define XC_VOP_SIHST_MASK           11:0
#define XC_VOP_SIHEND               (0x09)          ///< Scaling image window H end
#define XC_VOP_SIHEND_MASK          11:0

#define XC_VOP_SIVST                (0x0A)          ///< Scaling image window V start
#define XC_VOP_SIVST_MASK           11:0
#define XC_VOP_SIVEND               (0x0B)          ///< Scaling image window V end
#define XC_VOP_SIVEND_MASK          11:0

#define XC_VOP_HDTOT                (0x0C)          ///< Htotal
#define XC_VOP_HDTOT_MASK           11:0
#define XC_VOP_VDTOT                (0x0D)          ///< Vtotal
#define XC_VOP_VDTOT_MASK           11:0

#define XC_VOP_AOVS                 (0x10)
#define XC_VOP_AOVS_MASK            15:15

#define XC_VOP_OCTRL                (0x21)
#define XC_VOP_OCTRL_MASK           15:0
    #define XC_VOP_OCTRL_SWAP_EVEN_RB           BIT(0)
    #define XC_VOP_OCTRL_SWAP_ODD_RB            BIT(1)
    #define XC_VOP_OCTRL_SWAP_EVEN_ML           BIT(2)
    #define XC_VOP_OCTRL_SWAP_ODD_ML            BIT(3)
    #define XC_VOP_OCTRL_DUAL_PORT              BIT(8)
    #define XC_VOP_OCTRL_CONNECTOR_SWAP_PORT    BIT(9)

#define XC_VOP_OSTRL                (0x46)
#define XC_VOP_OSTRL_MASK           15:0

#define XC_VOP_ODRV                 (0x47)
#define XC_VOP_ODRV_MASK            7:0

#define XC_VOP_DITHCTRL             (0x1B)
#define XC_VOP_DITHCTRL_MASK        15:0

#define PANEL_LPLL_INPUT_DIV_1st          0x00
#define PANEL_LPLL_INPUT_DIV_2nd          0x00 // 0:/1, 1:/2, 2:/4, 3:/8
#define PANEL_LPLL_LOOP_DIV_1st           0x03 // 0:/1, 1:/2, 2:/4, 3:/8
#define PANEL_LPLL_LOOP_DIV_2nd           0x01 //
#define PANEL_LPLL_OUTPUT_DIV_1st         0x00 // 0:/1, 1:/2, 2:/4, 3:/8
#define PANEL_LPLL_OUTPUT_DIV_2nd         0x00

#define BD_LVDS_CONNECT_TYPE              1

#define DAC_LPLL_ICTRL     0x0002
#define LVDS_LPLL_ICTRL    0x0003

#define XC_LPLL_TYPE                (0x03)
#define XC_LPLL_TYPE_MASK           6:6

#define XC_LPLL_MODE                (0x03)
#define XC_LPLL_MODE_MASK           7:7

// #define do_div(x,y) ((x)/=(y))
#include <asm/div64.h>

#define XC_LPLL_FPLL_ENABLE         (0x0C)
#define XC_LPLL_FPLL_ENABLE_MASK    3:3

#define XC_LPLL_SET0                (0x0F)
#define XC_LPLL_SET0_MASK           15:0

#define XC_LPLL_SET1                (0x10)
#define XC_LPLL_SET1_MASK           15:0

#define ENABLE_FRC                   0

// for 4Ch LVDS
#if ENABLE_FRC
#define MOD_OUTPUT_MODE 2
#else
#define MOD_OUTPUT_MODE 1 // also support single, QUAD mode
#endif

#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif

#define L_LVDS_SINGLE_A_VAL   (0x0000) // for LVDS Single port Channel A
#define M_LVDS_SINGLE_A_VAL   (0x5500)
#define H_LVDS_SINGLE_A_VAL   (0x0005)

#define L_LVDS_SINGLE_B_VAL   (0x5000)// for LVDS Single Port Channel B
#define M_LVDS_SINGLE_B_VAL   (0x0055)
#define H_LVDS_SINGLE_B_VAL   (0x0000)

typedef int MS_GPIO_NUM;        ///< GPIO handle, for default operations,
#define END_GPIO_NUM    186

#define _CONCAT( a, b )     a##b
#define CONCAT( a, b )      _CONCAT( a, b )

// Dummy
#define GPIO999_OEN     0, 0
#define GPIO999_OUT     0, 0
#define GPIO999_IN      0, 0

#if defined(PRJ_X5) || defined(PRJ_X9)
#define GPIO0_OEN 0x0e3c, BIT4
#define GPIO0_OUT 0x0e3a, BIT4
#define GPIO0_IN  0x0e3b, BIT4

#define GPIO1_OEN 0x0e3c, BIT5
#define GPIO1_OUT 0x0e3a, BIT5
#define GPIO1_IN  0x0e3b, BIT5

#define GPIO2_OEN 0x0e3c, BIT6
#define GPIO2_OUT 0x0e3a, BIT6
#define GPIO2_IN  0x0e3b, BIT6

#define GPIO3_OEN 0x0e3c, BIT7
#define GPIO3_OUT 0x0e3a, BIT7
#define GPIO3_IN  0x0e3b, BIT7

#define GPIO4_OEN 0x0e3c, BIT0
#define GPIO4_OUT 0x0e3a, BIT0
#define GPIO4_IN  0x0e3b, BIT0

#define GPIO5_OEN 0x0e3c, BIT2
#define GPIO5_OUT 0x0e3a, BIT2
#define GPIO5_IN  0x0e3b, BIT2

#define GPIO6_OEN 0x0e1e, BIT0
#define GPIO6_OUT 0x0e20, BIT0
#define GPIO6_IN  0x0e22, BIT0

#define GPIO7_OEN 0x0e1e, BIT1
#define GPIO7_OUT 0x0e20, BIT1
#define GPIO7_IN  0x0e22, BIT1

#define GPIO8_OEN 0x0e1e, BIT2
#define GPIO8_OUT 0x0e20, BIT2
#define GPIO8_IN  0x0e22, BIT2

#define GPIO9_OEN 0x0e1e, BIT3
#define GPIO9_OUT 0x0e20, BIT3
#define GPIO9_IN  0x0e22, BIT3

#define GPIO10_OEN 0x0e1e, BIT4
#define GPIO10_OUT 0x0e20, BIT4
#define GPIO10_IN  0x0e22, BIT4

#define GPIO11_OEN 0x0e1e, BIT5
#define GPIO11_OUT 0x0e20, BIT5
#define GPIO11_IN  0x0e22, BIT5

#define GPIO12_OEN 0x0e1e, BIT6
#define GPIO12_OUT 0x0e20, BIT6
#define GPIO12_IN  0x0e22, BIT6

#define GPIO13_OEN 0x0e1e, BIT7
#define GPIO13_OUT 0x0e20, BIT7
#define GPIO13_IN  0x0e22, BIT7

#define GPIO14_OEN 0x0e1f, BIT0
#define GPIO14_OUT 0x0e21, BIT0
#define GPIO14_IN  0x0e23, BIT0

#define GPIO15_OEN 0x0e1f, BIT1
#define GPIO15_OUT 0x0e21, BIT1
#define GPIO15_IN  0x0e23, BIT1

#define GPIO16_OEN 0x0e1f, BIT2
#define GPIO16_OUT 0x0e21, BIT2
#define GPIO16_IN  0x0e23, BIT2

#define GPIO17_OEN 0x0e1f, BIT3
#define GPIO17_OUT 0x0e21, BIT3
#define GPIO17_IN  0x0e23, BIT3

#define GPIO18_OEN 0x0e1f, BIT4
#define GPIO18_OUT 0x0e21, BIT4
#define GPIO18_IN  0x0e23, BIT4

#define GPIO19_OEN 0x0e4e, BIT0
#define GPIO19_OUT 0x0e4e, BIT4
#define GPIO19_IN  0x0e4f, BIT0

#define GPIO20_OEN 0x0e4e, BIT1
#define GPIO20_OUT 0x0e4e, BIT5
#define GPIO20_IN  0x0e4f, BIT1

#define GPIO21_OEN 0x0e4e, BIT2
#define GPIO21_OUT 0x0e4e, BIT6
#define GPIO21_IN  0x0e4f, BIT2

#define GPIO22_OEN 0x0e4e, BIT3
#define GPIO22_OUT 0x0e4e, BIT7
#define GPIO22_IN  0x0e4f, BIT3

#define GPIO23_OEN 0x0496, BIT1
#define GPIO23_OUT 0x0496, BIT2
#define GPIO23_IN  0x0496, BIT0

#define GPIO24_OEN 0x0496, BIT5
#define GPIO24_OUT 0x0496, BIT6
#define GPIO24_IN  0x0496, BIT4

#define GPIO25_OEN 0x0497, BIT1
#define GPIO25_OUT 0x0497, BIT2
#define GPIO25_IN  0x0497, BIT0

#define GPIO26_OEN 0x0497, BIT5
#define GPIO26_OUT 0x0497, BIT6
#define GPIO26_IN  0x0497, BIT4

#define GPIO27_OEN 0x0498, BIT1
#define GPIO27_OUT 0x0498, BIT2
#define GPIO27_IN  0x0498, BIT0

#define GPIO28_OEN 0x0498, BIT5
#define GPIO28_OUT 0x0498, BIT6
#define GPIO28_IN  0x0498, BIT4

#define GPIO29_OEN 0x0499, BIT1
#define GPIO29_OUT 0x0499, BIT2
#define GPIO29_IN  0x0499, BIT0

#define GPIO30_OEN 0x0499, BIT5
#define GPIO30_OUT 0x0499, BIT6
#define GPIO30_IN  0x0499, BIT4

#define GPIO31_OEN 0x1423, BIT0
#define GPIO31_OUT 0x1424, BIT0
#define GPIO31_IN  0x1425, BIT0

#define GPIO32_OEN 0x1423, BIT1
#define GPIO32_OUT 0x1424, BIT1
#define GPIO32_IN  0x1425, BIT1

#define GPIO33_OEN 0x1423, BIT2
#define GPIO33_OUT 0x1424, BIT2
#define GPIO33_IN  0x1425, BIT2

#define GPIO34_OEN 0x1423, BIT3
#define GPIO34_OUT 0x1424, BIT3
#define GPIO34_IN  0x1425, BIT3

#define GPIO35_OEN 0x1423, BIT4
#define GPIO35_OUT 0x1424, BIT4
#define GPIO35_IN  0x1425, BIT4

#define GPIO36_OEN 0x101e5C, BIT0
#define GPIO36_OUT 0x101e56, BIT0
#define GPIO36_IN  0x101e50, BIT0

#define GPIO37_OEN 0x101e5C, BIT1
#define GPIO37_OUT 0x101e56, BIT1
#define GPIO37_IN  0x101e50, BIT1

#define GPIO38_OEN 0x101e5C, BIT2
#define GPIO38_OUT 0x101e56, BIT2
#define GPIO38_IN  0x101e50, BIT2

#define GPIO39_OEN 0x101e5C, BIT3
#define GPIO39_OUT 0x101e56, BIT3
#define GPIO39_IN  0x101e50, BIT3

#define GPIO40_OEN 0x101e5C, BIT4
#define GPIO40_OUT 0x101e56, BIT4
#define GPIO40_IN  0x101e50, BIT4

#define GPIO41_OEN 0x101e5C, BIT5
#define GPIO41_OUT 0x101e56, BIT5
#define GPIO41_IN  0x101e50, BIT5

#define GPIO42_OEN 0x101e5C, BIT6
#define GPIO42_OUT 0x101e56, BIT6
#define GPIO42_IN  0x101e50, BIT6

#define GPIO43_OEN 0x101e5C, BIT7
#define GPIO43_OUT 0x101e56, BIT7
#define GPIO43_IN  0x101e50, BIT7

#define GPIO44_OEN 0x101e5D, BIT0
#define GPIO44_OUT 0x101e57, BIT0
#define GPIO44_IN  0x101e51, BIT0

#define GPIO45_OEN 0x101e5D, BIT1
#define GPIO45_OUT 0x101e57, BIT1
#define GPIO45_IN  0x101e51, BIT1

#define GPIO46_OEN 0x101e5D, BIT2
#define GPIO46_OUT 0x101e57, BIT2
#define GPIO46_IN  0x101e51, BIT2

#define GPIO47_OEN 0x101e5D, BIT3
#define GPIO47_OUT 0x101e57, BIT3
#define GPIO47_IN  0x101e51, BIT3

#define GPIO48_OEN 0x101e5D, BIT4
#define GPIO48_OUT 0x101e57, BIT4
#define GPIO48_IN  0x101e51, BIT4

#define GPIO49_OEN 0x101e5D, BIT5
#define GPIO49_OUT 0x101e57, BIT5
#define GPIO49_IN  0x101e51, BIT5

#define GPIO50_OEN 0x101e5D, BIT6
#define GPIO50_OUT 0x101e57, BIT6
#define GPIO50_IN  0x101e51, BIT6

#define GPIO51_OEN 0x101e5D, BIT7
#define GPIO51_OUT 0x101e57, BIT7
#define GPIO51_IN  0x101e51, BIT7

#define GPIO52_OEN 0x101e5E, BIT0
#define GPIO52_OUT 0x101e58, BIT0
#define GPIO52_IN  0x101e52, BIT0

#define GPIO53_OEN 0x101e5E, BIT1
#define GPIO53_OUT 0x101e58, BIT1
#define GPIO53_IN  0x101e52, BIT1

#define GPIO54_OEN 0x101e5E, BIT2
#define GPIO54_OUT 0x101e58, BIT2
#define GPIO54_IN  0x101e52, BIT2

#define GPIO55_OEN 0x101e5E, BIT3
#define GPIO55_OUT 0x101e58, BIT3
#define GPIO55_IN  0x101e52, BIT3

#define GPIO56_OEN 0x101e5E, BIT4
#define GPIO56_OUT 0x101e58, BIT4
#define GPIO56_IN  0x101e52, BIT4

#define GPIO57_OEN 0x101e5E, BIT5
#define GPIO57_OUT 0x101e58, BIT5
#define GPIO57_IN  0x101e52, BIT5

#define GPIO58_OEN 0x101e5E, BIT6
#define GPIO58_OUT 0x101e58, BIT6
#define GPIO58_IN  0x101e52, BIT6

#define GPIO59_OEN 0x101e5E, BIT7
#define GPIO59_OUT 0x101e58, BIT7
#define GPIO59_IN  0x101e52, BIT7

#define GPIO60_OEN 0x101e5F, BIT0
#define GPIO60_OUT 0x101e59, BIT0
#define GPIO60_IN  0x101e53, BIT0

#define GPIO61_OEN 0x101e5F, BIT1
#define GPIO61_OUT 0x101e59, BIT1
#define GPIO61_IN  0x101e53, BIT1

#define GPIO62_OEN 0x101e5F, BIT2
#define GPIO62_OUT 0x101e59, BIT2
#define GPIO62_IN  0x101e53, BIT2

#define GPIO63_OEN 0x101e5F, BIT3
#define GPIO63_OUT 0x101e59, BIT3
#define GPIO63_IN  0x101e53, BIT3

#define GPIO64_OEN 0x101e5F, BIT4
#define GPIO64_OUT 0x101e59, BIT4
#define GPIO64_IN  0x101e53, BIT4

#define GPIO65_OEN 0x101e5F, BIT5
#define GPIO65_OUT 0x101e59, BIT5
#define GPIO65_IN  0x101e53, BIT5

#define GPIO66_OEN 0x101e5F, BIT6
#define GPIO66_OUT 0x101e59, BIT6
#define GPIO66_IN  0x101e53, BIT6

#define GPIO67_OEN 0x101e5F, BIT7
#define GPIO67_OUT 0x101e59, BIT7
#define GPIO67_IN  0x101e53, BIT7

#define GPIO68_OEN 0x101e60, BIT0
#define GPIO68_OUT 0x101e5A, BIT0
#define GPIO68_IN  0x101e54, BIT0

#define GPIO69_OEN 0x101e60, BIT1
#define GPIO69_OUT 0x101e5A, BIT1
#define GPIO69_IN  0x101e54, BIT1

#define GPIO70_OEN 0x101e60, BIT2
#define GPIO70_OUT 0x101e5A, BIT2
#define GPIO70_IN  0x101e54, BIT2

#define GPIO71_OEN 0x101e60, BIT3
#define GPIO71_OUT 0x101e5A, BIT3
#define GPIO71_IN  0x101e54, BIT3

#define GPIO72_OEN 0x101e60, BIT4
#define GPIO72_OUT 0x101e5A, BIT4
#define GPIO72_IN  0x101e54, BIT4

#define GPIO73_OEN 0x101e60, BIT5
#define GPIO73_OUT 0x101e5A, BIT5
#define GPIO73_IN  0x101e54, BIT5

#define GPIO74_OEN 0x101e60, BIT6
#define GPIO74_OUT 0x101e5A, BIT6
#define GPIO74_IN  0x101e54, BIT6

#define GPIO75_OEN 0x101e60, BIT7
#define GPIO75_OUT 0x101e5A, BIT7
#define GPIO75_IN  0x101e54, BIT7

#define GPIO76_OEN 0x101e61, BIT0
#define GPIO76_OUT 0x101e5B, BIT0
#define GPIO76_IN  0x101e55, BIT0

#define GPIO77_OEN 0x101e68, BIT0
#define GPIO77_OUT 0x101e64, BIT0
#define GPIO77_IN  0x101e62, BIT0

#define GPIO78_OEN 0x101e68, BIT1
#define GPIO78_OUT 0x101e64, BIT1
#define GPIO78_IN  0x101e62, BIT1

#define GPIO79_OEN 0x101e68, BIT2
#define GPIO79_OUT 0x101e64, BIT2
#define GPIO79_IN  0x101e62, BIT2

#define GPIO80_OEN 0x101e68, BIT3
#define GPIO80_OUT 0x101e64, BIT3
#define GPIO80_IN  0x101e62, BIT3

#define GPIO81_OEN 0x101e68, BIT4
#define GPIO81_OUT 0x101e64, BIT4
#define GPIO81_IN  0x101e62, BIT4

#define GPIO82_OEN 0x101e68, BIT5
#define GPIO82_OUT 0x101e64, BIT5
#define GPIO82_IN  0x101e62, BIT5

#define GPIO83_OEN 0x101e68, BIT6
#define GPIO83_OUT 0x101e64, BIT6
#define GPIO83_IN  0x101e62, BIT6

#define GPIO84_OEN 0x101e68, BIT7
#define GPIO84_OUT 0x101e64, BIT7
#define GPIO84_IN  0x101e62, BIT7

#define GPIO85_OEN 0x101e69, BIT0
#define GPIO85_OUT 0x101e65, BIT0
#define GPIO85_IN  0x101e63, BIT0

#define GPIO86_OEN 0x101e69, BIT1
#define GPIO86_OUT 0x101e65, BIT1
#define GPIO86_IN  0x101e63, BIT1

#define GPIO87_OEN 0x101e69, BIT2
#define GPIO87_OUT 0x101e65, BIT2
#define GPIO87_IN  0x101e63, BIT2

#define GPIO88_OEN 0x101e4E, BIT0
#define GPIO88_OUT 0x101e4C, BIT0
#define GPIO88_IN  0x101e4A, BIT0

#define GPIO89_OEN 0x101e4E, BIT1
#define GPIO89_OUT 0x101e4C, BIT1
#define GPIO89_IN  0x101e4A, BIT1

#define GPIO90_OEN 0x101e4E, BIT2
#define GPIO90_OUT 0x101e4C, BIT2
#define GPIO90_IN  0x101e4A, BIT2
#endif

void MApi_PNL_En(MS_BOOL bPanelOn );
void MApi_PNL_SetOutput(APIPNL_OUTPUT_MODE eOutputMode);
int mdrv_gpio_get_level(MS_GPIO_NUM gpio);
PWM_Result MDrv_PWM_Init(void);
PWM_Result MDrv_PWM_Oen(PWM_ChNum u8IndexPWM, MS_BOOL bOenPWM);
PWM_Result MDrv_PWM_Period(PWM_ChNum u8IndexPWM, MS_U32 u32PeriodPWM);
PWM_Result MDrv_PWM_DutyCycle(PWM_ChNum u8IndexPWM, MS_U32 u32DutyPWM);
void MDrv_PWM_Div(PWM_ChNum u8IndexPWM, MS_U16 u16DivPWM);
void MDrv_PWM_Polarity(PWM_ChNum u8IndexPWM, MS_BOOL bPolPWM);
void MDrv_PWM_Vdben(PWM_ChNum u8IndexPWM, MS_BOOL bVdbenPWM);
void MApi_XC_Sys_Init(void);
void MDrv_PWM_Dben(PWM_ChNum u8IndexPWM, MS_BOOL bDbenPWM);
void    MApi_XC_WriteByteMask(MS_U32 u32Reg, MS_U8 u8Val, MS_U8 u8Msk);
void MHal_PNL_ParamMap(PNL_InitData *pstPanelInitData, PMST_PANEL_FRC_INFO_t pFRCPanelInfo_st);
void MHal_FRC_MOD_Init(PMST_PANEL_FRC_INFO_t pFRCPanelInfo_st);
void MHal_FRC_MOD_SetInvert(MS_BOOL bInvert_HS, MS_BOOL bInvert_VS, MS_BOOL bInvert_DE, MS_BOOL bMask_HS, MS_BOOL bMask_VS, MS_BOOL bMask_DE);
MS_BOOL MHal_FRC_MOD_SetCtrlSwing(MS_U16 swing_data, MS_U16 u16Swing_Level);
void MHal_FRC_MOD_SetModTypeParam(MS_U8 u8LPLL_Type,MS_U8 u8LPLL_Mode, MS_U16 u16MOD_CTRL0, MS_U8 u8MOD_CTRLB);
void MHal_FRC_MOD_SetLvdsTiMode(MS_BOOL btimode, MS_U8 u8panelBitNums);
void MHal_FRC_MOD_LvdsModeSel(MS_U16 u16FuncEnable, MS_U16 u16FuncDisable);
void MHal_FRC_MOD_LvdsBitNumSel(MS_U16 u16BitNum);
void MHal_FRC_MOD_SetModTypeEn(MS_U8 u8LPLL_Type, MS_U8 u8LPLL_Mode);
void MHal_FRC_MOD_TtlFuncSel(MS_U16 u16FuncEnable, MS_U16 u16FuncDisable);
void MHal_FRC_MOD_20FuncEn(MS_U16 u16FuncEnable, MS_U16 u16FuncDisable);
void MHal_FRC_MOD_SetModDataFormat(MS_U8 panelType);
void MHal_FRC_MOD_SetDataFormat(MS_U16 format);
void MHal_FRC_MOD_SetLvdsSwap(MS_U8 u8lvdsSwapCh, MS_BOOL blvdsShiftPair, MS_BOOL blvdsSwapPair, MS_BOOL blvdsSwapPol);
void MHal_FRC_MOD_LvdsSwapSel(MS_U16 u16SwapCH);
MS_U16 _MHal_PNL_MOD_Swing_Refactor_AfterCAL(MS_U16 u16Swing_Level);
MS_U8 MHal_PNL_Get_Loop_DIV( MS_U8 u8LPLL_Mode, MS_U8 eLPLL_Type, MS_U32 ldHz);
void MHal_PNL_SetOutputPattern(MS_BOOL bEnable, MS_U16 u16Red , MS_U16 u16Green, MS_U16 u16Blue);
void MApi_PNL_SetLVDSFormat_Cus(LvdsFmt_k fmtLVDS);

#endif // __SEC_PANEL_H__


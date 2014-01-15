#ifndef __SEC_PANEL_B1_H__
#define __SEC_PANEL_B1_H__

typedef enum
{
    E_APIXC_RET_FAIL = 0,   ///<Fail
    E_APIXC_RET_OK,         ///<OK
} E_APIXC_ReturnValue;      /// XC Return Value

/// please enum use BIT0 = 1, BIT1 = 2, BIT3 = 4
typedef enum
{
    E_XC_INIT_MISC_A_NULL = 0,
    E_XC_INIT_MISC_A_IMMESWITCH = 1,
    E_XC_INIT_MISC_A_IMMESWITCH_DVI_POWERSAVING = 2,
    E_XC_INIT_MISC_A_DVI_AUTO_EQ = 4,

    E_XC_INIT_MISC_A_FRC_INSIDE = 8,
} XC_INIT_MISC_A;

/// Define the initial MISC for XC
typedef struct
{
    MS_U32 u32XCMISC_version;                   ///<Version of current structure.
    MS_U32 u32MISC_A;
    MS_U32 u32MISC_B;
    MS_U32 u32MISC_C;
    MS_U32 u32MISC_D;
} XC_INITMISC;

typedef enum
{
    E_XC_MOD_OUTPUT_SINGLE = 0,          ///< single channel
    E_XC_MOD_OUTPUT_DUAL = 1,            ///< dual channel

    E_XC_MOD_OUTPUT_QUAD = 2,            ///< quad channel

} E_XC_OUTPUT_MODE;

// panel related
//-----
typedef struct
{
    MS_BOOL bFRC_En;
    MS_U8   u8FRC_LPLL_Mode;

    MS_U16  u16MOD_SwingLevel;
    MS_U16  u16MOD_SwingCLK;

    MS_BOOL bdither6Bits;
    MS_BOOL blvdsShiftPair;
    MS_BOOL blvdsSwapPair;
    MS_U8   u8lvdsSwapCh;

    // EPI
    MS_BOOL bepiLRSwap;
    MS_BOOL bepiLMirror;
    MS_BOOL bepiRMirror;
} PNL_InitData_ADV_st, *p_PNL_InitData_ADV_st;


E_APIXC_ReturnValue MApi_XC_GetMISCStatus(XC_INITMISC *pXC_Init_Misc);

void MDrv_Init_PNLAdvInfo_beforePNLinit(p_PNL_InitData_ADV_st pInitInfo);

void MHal_BD_LVDS_Output_Type(MS_U16 Type);

void MHal_FRC_MOD_SetPowerdown(MS_U16 u16FuncEnable, MS_U16 u16FuncDisable);

void MHal_FRC_MOD_SetClkEn(MS_BOOL bEnable);

void MHal_FRC_MOD_SetOutputCfg(MS_U16 output_cfg_10, MS_U16 output_cfg_11, MS_U16 output_cfg_12, MS_U16 output_cfg_13);

void MHal_FRC_MOD_SetExtGpioEn(MS_U16 ext_en_18, MS_U16 ext_en_19);

void MHal_FRC_MOD_SetTtlPairEn(MS_U16 pair_en_1a, MS_U16 pair_en_1b);

void MHal_FRC_MOD_SetGpoOez(MS_U16 gpo_oez_14, MS_U16 gpo_oez_15, MS_U16 gpo_oez_16, MS_U16 gpo_oez_17);

MS_U8 MHal_MOD_PowerOn(MS_BOOL bEn, MS_U8 u8LPLL_Type);

#endif // __SEC_PANEL_B1_H__



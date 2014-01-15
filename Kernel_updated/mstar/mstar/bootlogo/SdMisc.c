/**
 * @file SdMisc.c
 * @brief Platform Specific API file
 * This file contains the definitions of miscillaneous functions which are used only for
 * specific platforms or products.
 *
 * VER 1.0 - initial version.
**/
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
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/err.h>
#include "SdMisc.h"
#include "SdTypes.h"
#include "sec_panel_b1.h"
#include "sec_panel.h"
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
extern PANEL_RESOLUTION_TYPE g_PNL_TypeSel ;
//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MS_BOOL bPWM_Dimming = FALSE;
static MS_BOOL bPWM_FristTime = TRUE;
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
SdResult SdDisplay_Init(void)
{
    // Do not change the sequence of Panel enable and backlight enable
    MApi_PNL_En(ENABLE);

    return SD_OK;
}

/**
 * @brief Control LVDS Output
 * @remarks The default state is no LVDS output
 * @param outputLVDS clock and data control parameter
 * @return SD_OK/SD_NOT_OK
 * @see
 */
SdResult SdMisc_LvdsOutput(SdMisc_LvdsOutput_k outputLVDS)
{
    APIPNL_OUTPUT_MODE eOutputMode;

    switch(outputLVDS)
    {
        case SD_MISC_LVDS_OUT_NO_OUTPUT:
            eOutputMode = E_APIPNL_OUTPUT_NO_OUTPUT;
            break;

        case SD_MISC_LVDS_OUT_CLK_ONLY:
            eOutputMode = E_APIPNL_OUTPUT_CLK_ONLY;
            break;

        case SD_MISC_LVDS_OUT_CLK_DATA:
        default:
            eOutputMode = E_APIPNL_OUTPUT_CLK_DATA;
            break;
    }

    MApi_PNL_SetOutput(eOutputMode);
    return SD_OK;
}

SdResult SdCommon_GpioRead(MS_U32 pinIndex, MS_U32 *pOnOff)
{
    *pOnOff  = mdrv_gpio_get_level(pinIndex);
    return SD_OK;
}

/**
 * @brief Configure display resolution
 * @remarks
 * @param resDisp    resolution of display
 * @return SD_OK/SD_NOT_OK
 * @see
 */
SdResult SdMisc_DisplayResolution(SdMisc_DispRes_k resDisp)
{
    static MS_U8 u8XCInitialed = FALSE;
    MDrv_PWM_Init();
    switch(resDisp)
    {
        case SD_MISC_DISP_RES_FHD_LED:      ///< 1080p
        case SD_MISC_DISP_RES_FHD_LCD:      ///< 1080p
        case SD_MISC_DISP_RES_FHD_PDP:      ///< 1080p
        case SD_MISC_DISP_RES_CEA_FHD:      ///< 1080p
        case SD_MISC_DISP_RES_FHD_LED_MFM:  ///< 1080p
        case SD_MISC_DISP_RES_FHD_LCD_MFM:  ///< 1080p
            break;

        default :
            break;
    }
    //=================================

    switch(resDisp)
    {
        case SD_MISC_DISP_RES_FHD_LED:    ///< 1080p
        case SD_MISC_DISP_RES_FHD_LCD:    ///< 1080p
            g_PNL_TypeSel = PNL_FULLHD_SEC_LE32A;
            break;
        case SD_MISC_DISP_RES_FHD_PDP:    ///< 1080p
            g_PNL_TypeSel = PNL_SEC_PDP_1080P;
            break;
        case SD_MISC_DISP_RES_FHD_LED_MFM:     ///< 1080p
        case SD_MISC_DISP_RES_FHD_LCD_MFM:     ///< 1080p
            g_PNL_TypeSel = PNL_WXGA_SEC_LTM230H;
            break;
        case SD_MISC_DISP_RES_CEA_FHD:     ///< 1080p
            g_PNL_TypeSel = PNL_FULLHD_SEC_CEA;
            break;
        case SD_MISC_DISP_RES_WSXGA_LED_MFM: ///< 1600x900
        case SD_MISC_DISP_RES_WSXGA_LCD_MFM: ///< 1600x900
            g_PNL_TypeSel = PNL_WXGA_SEC_LTM200KT03;
            break;
        case SD_MISC_DISP_RES_HD_LED:    ///< 1366x768
        case SD_MISC_DISP_RES_HD_LCD:    ///< 1366x768
            g_PNL_TypeSel = PNL_WXGA_SEC_LA32C45;
            break;
        case SD_MISC_DISP_RES_HD_PDP:    ///< 1365x768
            g_PNL_TypeSel = PNL_SEC_PDP_WXGA;
            break;
        case SD_MISC_DISP_RES_HD_LED_MFM:     ///< 1366x768
        case SD_MISC_DISP_RES_HD_LCD_MFM:     ///< 1366x768
            g_PNL_TypeSel = PNL_WXGA_SEC_LA32C45;
            break;
        case SD_MISC_DISP_RES_VESA_HD:    ///< 1360x768
            g_PNL_TypeSel = PNL_FULLHD_SEC_VESA;
            break;
        case SD_MISC_DISP_RES_SD_LED:    ///< 1024x768
        case SD_MISC_DISP_RES_SD_LCD:    ///< 1024x768
             g_PNL_TypeSel = PNL_WXGA_SEC_LA32C45;
            break;
        case SD_MISC_DISP_RES_SD_PDP:    ///< 1024x768
             g_PNL_TypeSel = PNL_SEC_PDP_XGA;
            break;
        case SD_MISC_DISP_RES_FHD_R:
             g_PNL_TypeSel = PNL_FULLHD_SEC_R;
            break;
        default:
            g_PNL_TypeSel = PNL_FULLHD_SEC_LE32A;
            break;

    }
    if ( u8XCInitialed == FALSE )
    {
        MApi_XC_Sys_Init();
        u8XCInitialed = TRUE;
    }
    else
    {
        printk(KERN_ERR " Panel is already selected. Dynamic change panel setting is illegal.\n");
    }
    return SD_OK;
}

SdResult SdMisc_PWMOnOff(SdMisc_OnOff_k eMode)
{
    if(eMode == SD_ON)
    {
        bPWM_Dimming = TRUE;

		if(bPWM_FristTime)
		{
			MDrv_PWM_Polarity(E_PWM_CH3, 1);
			MDrv_PWM_Div(E_PWM_CH3, 1);
			MDrv_PWM_Period(E_PWM_CH3, 0x9300);
   			MDrv_PWM_DutyCycle(E_PWM_CH3, 0x5833);
			bPWM_FristTime = FALSE;
		}
 	}

    return SD_OK;
}

/**
 * @brief Control PWM Frequency and Duty
 * @param dwFreq:Frequency, dwDuty:Duty(0~512), eMode: Sync with Vsync
 * @return SD_OK/SD_NOT_OK
 */
SdResult SdMisc_SetDimming(MS_U32 dwFreq,MS_U32 dwDuty,SdMisc_OnOff_k eMode)
{
    MS_U32 pu32BaseAddr;

    #define REG_ACCESS ((unsigned short volatile *) pu32BaseAddr)
    pu32BaseAddr = 0xBF200000;

    dwDuty = dwFreq - dwDuty;  //due to input value is invert

    if(bPWM_Dimming == FALSE)
       return SD_OK;

    MDrv_PWM_Polarity(E_PWM_CH3, 1);
    MDrv_PWM_Div(E_PWM_CH3, 1);
    MDrv_PWM_Period(E_PWM_CH3, dwFreq);
    MDrv_PWM_DutyCycle(E_PWM_CH3, dwDuty);

    if(eMode == SD_ON)
        MDrv_PWM_Vdben(E_PWM_CH3,FALSE);
    else
        MDrv_PWM_Vdben(E_PWM_CH3,TRUE);

    REG_ACCESS[0x1e06] &= ~0x0008;
    REG_ACCESS[0x1ec8] |= 0x1000;

    return SD_OK;

}

//Set LVDS Format
/**
 * @brief Set LVDS Format
 * @remarks
 * @param fmtLVDS    The kind of LVDS format
 * @return SD_OK/SD_NOT_OK
 * @see
 */
SdResult SdMisc_LvdsFormat(SdMisc_LvdsFmt_k fmtLVDS)
{
    //printf ("[%s @ %04d] LVDS formt (%04d) \n", __FUNCTION__, __LINE__, fmtLVDS);
    switch(fmtLVDS)
    {
        case SD_MISC_LVDS_FMT_VESA_8:
        case SD_MISC_LVDS_FMT_PDP_10:
            MApi_PNL_SetLVDSFormat_Cus(LVDS_FMT_PDP_10);
            break;
        case SD_MISC_LVDS_FMT_PDP_8:
            MApi_PNL_SetLVDSFormat_Cus(LVDS_FMT_PDP_8);
            break;
        case SD_MISC_LVDS_FMT_NS_10:
            MApi_PNL_SetLVDSFormat_Cus(LVDS_FMT_NS_10);
            break;
        case SD_MISC_LVDS_FMT_JEIDA_8:
        case SD_MISC_LVDS_FMT_JEIDA_10:
        case SD_MISC_LVDS_FMT_MAX:
        default:
            MApi_PNL_SetLVDSFormat_Cus(LVDS_FMT_JEIDA_8);
            break;
    }
    return TRUE;
}
SdResult SdMisc_PanelMute(SdMisc_OnOff_k eMute)
{
    if(eMute == SD_ON)
        MHal_PNL_SetOutputPattern(ENABLE,0x0,0x0,0x0);
    else
        MHal_PNL_SetOutputPattern(DISABLE,0,0,0);

    return SD_OK;
}

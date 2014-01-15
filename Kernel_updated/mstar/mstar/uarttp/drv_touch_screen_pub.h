////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

/**
* @file    drv_touch_screen_pub.h
* @version
* @brief   touch screen UDI header file
*
*/

#ifndef __DRV_TOUCH_SCREEN_PUB_H__
#define __DRV_TOUCH_SCREEN_PUB_H__
#include <linux/types.h>

#ifdef CONFIG_MS_MSG20XX_TOUCHSCREEN
#include "touchscreen_msg_pub.h"
#endif

/*=============================================================*/
// Include files
/*=============================================================*/

/*=============================================================*/
// Extern definition
/*=============================================================*/

/*=============================================================*/
// Macro definition
/*=============================================================*/

/*=============================================================*/
// Data type definition
/*=============================================================*/
typedef enum
{
    DRV_TOUCH_SCREEN_OK = 0,
    DRV_TOUCH_SCREEN_FAIL,
    DRV_TOUCH_SCREEN_INVALID_PARAM
}DrvTsRetCode_e;

typedef enum
{
    DRV_TOUCH_SCREEN_ADC_LOCATION = 0,
    DRV_TOUCH_SCREEN_ADC_RESISTENCE
}DrvTsAdcJob_e;

typedef struct
{
    u16 aAdcData[4];
    u16 nAdcvalidNum;
    u16 nAligh_1;
}DrvTsAdcData_t;

typedef struct
{
    DrvTsAdcJob_e  nAdcJob;
    DrvTsAdcData_t tAdcData;
}DrvTsAdcJob_t;

typedef enum
{
    DRV_TOUCH_SCREEN_SYNC_REPORT = 0,
    DRV_TOUCH_SCREEN_ASYNC_REPORT
}DrvTsAdcReportMode_e;

typedef struct
{
    DrvTsAdcReportMode_e  nAdcReportMode;
    u32                   nMailBox;
}DrvTsAdcReportMode_t;

/*=============================================================*/
// Variable definition
/*=============================================================*/

/*=============================================================*/
// Global function definition
/*=============================================================*/
/** Drv_TsGetAdcData
* @brief        UDI interface to get ADC data from HW
* @param[in]    ptAdcJob:       pointer to describe ADC job type
* @param[in]    ptAdcReport:    pointer to describe ADC result report type
* @retval       DrvTsRetCode_e error code
*
*/
DrvTsRetCode_e DrvTsGetAdcData(DrvTsAdcJob_t *ptAdcJob, DrvTsAdcReportMode_t *ptAdcReport);

/*
 * @brief This function get the 8 bytes touch data from msg20xx
 *
 * @param: touchData:    u32X:X coordinates
 *                       u32Y:Y coordinates
 *                       u32Dis_X: X distance between two fingers
 *                       u32Dis_Y: Y distance between two fingers
 *                       u8Finger:Number of touch fingers
 *
 * @retrun : Touch status     -1 :received packet error
 *                                        0:touch end
 *                                        1:now touching
 */
//#ifdef CONFIG_MS_MSG20XX_TOUCHSCREEN
s32 DrvTouchScreenGetData(MdlTouchScreenInfo_t *touchData);
//#endif

/** DrvTouchScreenDisablePenDetection
* @brief        UDI interface to disable pen detection event
* @param[in]    None
* @retval       DrvTsRetCode_e error code
*
*/
DrvTsRetCode_e DrvTouchScreenDisablePenDetection(void);

/** Drv_TouchScreenEnablePenDetection
* @brief        UDI interface to enable pen detection event
* @param[in]    None
* @retval       DrvTsRetCode_e error code
*
*/
DrvTsRetCode_e DrvTouchScreenEnablePenDetection(void);

/** DrvTouchScreenDisable
* @brief        UDI interface to disable touch screen driver
* @param[in]    None
* @retval       DrvTsRetCode_e error code
*
*/
DrvTsRetCode_e DrvTouchScreenDisable(void);

/** DrvTouchScreenEnable
* @brief        UDI interface to enable touch screen driver
* @param[in]    None
* @retval       DrvTsRetCode_e error code
*
*/
DrvTsRetCode_e DrvTouchScreenEnable(void);

/** DrvIncenseAdcTouchscreenPendetHandler
* @brief        ISR of pen detection INT from Incense
* @param[in]    None
* @retval       None
*
*/
void DrvIncenseAdcTouchscreenPendetHandler(void);

#if defined(__B3__)
/** DrvTouchScreenDisablePenDetectionDirectly
* @brief        UDI interface to disable pen detection event
* @param[in]    None
* @retval       None
*
*/
void DrvTouchScreenDisablePenDetectionDirectly(void);
#endif
#endif //__DRV_TOUCH_SCREEN_PUB_H__


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
* @file    drv_touch_screen.c
* @version
* @brief   UDI of touch screen
*
*/
#define __DRV_TOUCH_SCREEN_C__


//#include "drv_touch_screen.h"
//#include "_drv_touch_screen.h"
//#include "incense_reg.h"
//#include "hal_touch_screen.h"

//#ifdef CONFIG_MS_MSG20XX_TOUCHSCREEN
#include <linux/kernel.h>
#include <asm/string.h>
#include "hal_touch_screen_c_msg20xx.h"
#include "touchscreen_msg_pub.h"
//#include "touchscreen_pub.h"
//#endif

/*=============================================================*/
// Macro definition
/*=============================================================*/

/*=============================================================*/
// Data type definition
/*=============================================================*/

/*=============================================================*/
// Variable definition
/*=============================================================*/
//static u8 _gTouchScreenPendetMailbox = 0xFF;

//#ifdef CONFIG_MS_MSG20XX_TOUCHSCREEN
static u8 _gnPreFingerNum = 0xff;
static u8 pre_KeyMode = 0;

#undef _TRACE
#define MS_TS_DRV_MSG20XX_DEBUG 0
#if (MS_TS_DRV_MSG20XX_DEBUG == 1)
#define DBG(p, fmt, arg...) pr_info(fmt, ##arg)
#else
#define DBG(p, fmt, arg...)
#endif
#define _TRACE(a) DBG a

/*=============================================================*/
// Global Function definition
/*=============================================================*/

/** Drv_TsGetAdcData
* @brief        UDI interface to get ADC data from HW
* @param[in]    ptAdcJob:       pointer to describe ADC job type
* @param[in]    ptAdcReport:    pointer to describe ADC result report type
* @retval       DrvTsRetCode_e error code
*
*/

//#ifdef CONFIG_MS_MSG20XX_TOUCHSCREEN
s32 DrvTouchScreenGetData(MdlTouchScreenInfo_t *touchData)
{
    u8 ret_getdata = 0;
    //Enter critical region and mask interupt
    static MdlTouchScreenInfo_t _gtPreTouchData;

    ret_getdata = HalTscrCDataParser(touchData);
    if(ret_getdata == 0)
    {
//    printk("[FAIL]ret_getdata == 0");
        return 0;
    }

    if(_gnPreFingerNum == 0xff)
    {
        _gnPreFingerNum = touchData->nFingerNum;
    }
    // set KeyMode
    if(_gnPreFingerNum > touchData->nFingerNum || touchData->nFingerNum == 0)
    {
        u8 temp_finger;
        if(touchData->nFingerNum)
        {
             u8 restore_cnt;
             u8 sp;
             u8 i;
             sp = touchData->nFingerNum;
             restore_cnt = _gnPreFingerNum - touchData->nFingerNum;
             // record last pressed point when released
             for(i = 0; i < restore_cnt; i++)
             {
                 _gtPreTouchData.atPoint[(sp + i)].Col = touchData->atPoint[(sp+i)].Col;
                 _gtPreTouchData.atPoint[(sp + i)].Row = touchData->atPoint[(sp+i)].Row;
             }
        }
        temp_finger = _gnPreFingerNum;
        _gnPreFingerNum = touchData->nFingerNum;
        touchData->nKeyMode = TOUCHSCREEN_RELEASED;
        touchData->nFingerNum = temp_finger;
    }
    else
    {
        _gnPreFingerNum = touchData->nFingerNum;
        touchData->nKeyMode = TOUCHSCREEN_PRESSED;
    }
    if(pre_KeyMode == 0)
    {
        pre_KeyMode = touchData->nKeyMode;
    }
	
    memcpy(&_gtPreTouchData, touchData, sizeof(MdlTouchScreenInfo_t));
//    if(touchData->nKeyMode == TOUCHSCREEN_RELEASED &&  pre_KeyMode != TOUCHSCREEN_PRESSED)
//    {
//        _TRACE((_CUS1 | LEVEL_9, "[FAIL] pre_KeyMode != TOUCHSCREEN_PRESSED"));
//        return 0;
//    }
    pre_KeyMode = touchData->nKeyMode;

    return 1;
}


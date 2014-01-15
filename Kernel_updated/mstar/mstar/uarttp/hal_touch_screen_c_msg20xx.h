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
 *
 * @file    hal_touch_screen_c_msg20xx.h
 *
 * @brief   This module defines touch screen c type function
 *
 * @author  Jack Hsieh
 *
 */

 #define __HAL_TOUCH_SCREEN_C_MSG20XX__


/*=============================================================*/
// Include files
/*=============================================================*/
#if 0
#include "vm_types.ht"
#include "sys.ho"
#include "vm_abb.h"
#include "mdl_touchscreen_msg_pub.h"
#else
//#include "hal_touch_screen.h"
#include "touchscreen_msg_pub.h"
#endif

/*=============================================================*/
// Macro definition
/*=============================================================*/

/*=============================================================*/
// Data type definition
/*=============================================================*/

/*=============================================================*/
// Variable definition
/*=============================================================*/

/*=============================================================*/
// Local function definition
/*=============================================================*/
#if 0
void gpio_Delay(u32 t);

/*=============================================================*/
// Local function definition
/*=============================================================*/
u8 HalTscrGpioReadI2C(u8 last_byte);

void HalTscrDevWriteI2CByte(u8 addr, u8 reg, u8 data);

void HalTscrCDevWriteI2CSeq(u8 addr, u8* data, u8 size);

//for Athena
void HalTscrDevReadI2C2Bytes(u8 addr, u8 reg, u8* data);

u8 HalTscrDevReadI2CByte(u8 addr, u8 reg);

void HalTscrCReadI2CSeq(u8 addr, u8* read_data, u8 size);

/**
 * @brief This function is for touch screen interrupt routine
 *
 * @param:   None
 * @retrun     None
 *
 */
void HalTscrCIrq(void);
#endif
#if 0
/**
 * @brief This function is for C type touch panel disable
 * @param : None
 * @retrun : None
 */
HalTsRetCode_e HalTouchScreenDisable(void);
#endif

/**
 * @brief This function is for C type touch panel init
 * @param : None
 * @retrun : None
 */
//HalTsRetCode_e HalTouchScreenMsg20xxEnable(void);

#if 0
/**
 * @brief This function is for C type touch panel disable interrupt
 * @param:      None
 * @retrun :     None
 *
 */
HalTsRetCode_e HalTouchScreenIntDisable(void);

/**
 * @brief This function is for C type touch panel disable interrupt
 * @param:      None
 * @retrun:      None
 *
 */
HalTsRetCode_e HalTouchScreenIntEnable(void);


bool HalTscrCWrite(u8 addr, u8 *pData, u16 size);
#endif

/*
 * @brief This function get the 8 bytes touch data from msg20xx
 *
 * @param: touchData:    u32X:X coordinates
 *                                   u32Y:Y coordinates
 *                                   u32Dis_X: X distance between two fingers
 *                                   u32Dis_Y: Y distance between two fingers
 *                                   u8Finger:Number of touch fingers
 *
 * @retrun: Touch status   -1 :received packet error
 *                                     0 :touch end
 *                                     1 :now touching
 *
 */
u8 HalTscrCDataParser(MdlTouchScreenInfo_t *touchData);

#if 0
/**
  * @brief This function is for set commnad to touch screen driving IC
  * @param1 : command to mapping to register
  * @param2 : parameter to set  register
  * @return : None
  */
void HalTouchScreenSetCommand(u32 command, u32 param);

/**
  * @brief This function is to Set Touch Screen Interrupt Debounce
  * @param : delay time (ms)
  * @return : None
*/
void HalTouchScreenSetIntDebounce(u16 Delay);

/**
  * @brief This function is for Get Touch Screen Intrrupt Debounce
  * @param : None
  * @return : debounce count
*/
u8 HalTouchScreenGetIntDebounce(void);
#endif


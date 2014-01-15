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
 * @file    hal_touch_screen_c_msg20xx.c
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
#include <linux/types.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include "hal_touch_screen_c_msg20xx.h"
#include "touchscreen_msg_pub.h"
#include "drvUART.h"
#include "halUART.h"
//#include "drv_io_chip_pub.h"


/*=============================================================*/
// Macro definition
/*=============================================================*/

//#define TSCR_C_DELAY(X) SYS_UDELAY(X)
#define TSCR_C_DELAY(X) udelay(X)
#define TSCR_LEVEL_C_TYP            //_CUS1|LEVEL_12
#define DATA_PACKAGE_SIZE    8

#undef _TRACE
#define MS_TS_HAL_MSG20XX_DEBUG 0
#if (MS_TS_HAL_MSG20XX_DEBUG == 1)
#define DBG(p, fmt, arg...) pr_info(fmt, ##arg)
#else
#define DBG(p, fmt, arg...)
#endif
#define _TRACE(a) DBG a


/*=============================================================*/
// Data type definition
/*=============================================================*/


/*=============================================================*/
// Variable definition
/*=============================================================*/
static bool isTimeout = false;


/*=============================================================*/
// Local function definition
/*=============================================================*/

/*=============================================================*/
// Local function definition
/*=============================================================*/

/**
 * @brief This function calculate the checksum
 *
 * @param:  *msg:              input data
 *                    s32Length :  data length
 * @retrun:   claculated checksum
 *
 */
static u8 _HalTouchScreen8bitChecksum( u8 *msg, s32 s32Length )
{
    s32 s32Checksum = 0;
    s32 i;

    for ( i = 0 ; i < s32Length; i++ )
    {
        s32Checksum += msg[i];
    }

    return (u8)( ( -s32Checksum ) & 0xFF );
}

/**
 * @brief This function is for touch screen read I2C
 *
 * @param:      read_data :   return data pointer 
 *              size      :   return data pointer size
 * @retrun:  fail
 *              others    :  success
 *
 */
static u16 _HalTscrCRead(u8 *read_data)
{

	u8 i;
	*read_data = (U8) FASTU_ReadByte(UART_RX);
	for (i=1; i<8; i++)
        {
            if(i == 7)
            {
                /* last byte read */
                *(read_data+i) = (U8) FASTU_ReadByte(UART_RX);
            }
            else
            {
                *(read_data+i) = (U8) FASTU_ReadByte(UART_RX);
            }
        }  
    return 1;
}

/*
 * @brief This function get the 8 bytes touch data from msg20xx
 *
 * @param: touchData:    u32X:X coordinates
 *                       u32Y:Y coordinates
 *                       u32Dis_X: X distance between two fingers
 *                       u32Dis_Y: Y distance between two fingers
 *                       u8Finger:Number of touch fingers
 *
 * @return : Touch status   -1 :received packet error
 *                                      0 :touch end
 *                                      1 :now touching
 *
 */
u8 HalTscrCDataParser(MdlTouchScreenInfo_t *touchData)
{
    u8 val[9] = {0};
    u8 Checksum = 0;
    u32 delta_x = 0,delta_y = 0;
    u32 u32X = 0;
    u32 u32Y = 0;
    static u32 uFinalX = 0;
    static u32 uFinalY = 0;
	
    memset(touchData, 0, sizeof(MdlTouchScreenInfo_t));

    //Read 9 u8 data from RX, the last va[8] for checksum			
    _HalTscrCRead(&val[0]);
/*    printk("val[0] = 0x%x\n", val[0]);
    printk("val[1] = 0x%x\n", val[1]);
    printk("val[2] = 0x%x\n", val[2]);
    printk("val[3] = 0x%x\n", val[3]);
    printk("val[4] = 0x%x\n", val[4]);
    printk("val[5] = 0x%x\n", val[5]);
    printk("val[6] = 0x%x\n", val[6]);	
    printk("val[7] = 0x%x\n", val[7]);
    printk("val[8] = 0x%x\n", val[8]);	
*/	
    if(isTimeout)
    {
        pr_err("clock-stretching timeout! \n");
        isTimeout = false;
        return 0;
    }
               
    if((val[0] == 0) && (val[8] ==0))      //Re-get a new packet if previous packet are all zero dute to packet isn't ready
    {
        TSCR_C_DELAY(1000);
        _HalTscrCRead(&val[0]); 		

    }       
    Checksum = _HalTouchScreen8bitChecksum(&val[0],8);  //calculate checksum
//printk("Checksum = 0x%x\n", Checksum);	           
    if((Checksum==val[8]) && (val[0]==0x52))   //check the checksum  of packet
    {
        u32X = (((val[1] & 0xF0) << 4) | val[2]);         //parse the packet to coordinates
        u32Y = (((val[1] & 0x0F) << 8) | val[3]);

        delta_x = (((val[4]&0xF0) << 4) | val[5]);
        delta_y = (((val[4]&0x0F) << 8) | val[6]);
//printk("[HAL] u32X = %x, u32Y = %x\n", u32X, u32Y);	 		
//printk("[HAL] delta_x = %x, delta_y = %x\n", delta_x, delta_y);       
        if((u32X == 0xFFF) && (u32Y == 0xFFF) && (delta_x == 0xFFF) && (delta_y == 0xFFF))
        {
            /* touch end */
            touchData->nFingerNum = 0; //touch end
            touchData->atPoint[0].Col = uFinalX; // final X coordinate
            touchData->atPoint[0].Row = uFinalY; // final Y coordinate               
            return 1;
        }

        if((delta_x == 0) && (delta_y == 0))
        {
            touchData->nFingerNum = 1; //one touch
            touchData->atPoint[0].Col = (u32X * TOUCH_SCREEN_WIDTH) / 2048;
            touchData->atPoint[0].Row = (u32Y * TOUCH_SCREEN_HEIGHT) / 2048;   

            /* Keep final X and Y coordinate*/
            uFinalX = touchData->atPoint[0].Col;
            uFinalY = touchData->atPoint[0].Row;
//printk("[HAL] touchData->atPoint[0].Col = %d, touchData->atPoint[0].Row = %d\n", touchData->atPoint[0].Col, touchData->atPoint[0].Row);			
//printk("[HAL] touchData->atPoint[1].Col = %d, touchData->atPoint[1].Row = %d\n", touchData->atPoint[1].Col, touchData->atPoint[1].Row);			              

            /* Calibrate if the touch panel was reversed in Y */
            #if 0
            touchData->atPoint[0].Row = (TOUCH_SCREEN_HEIGHT - touchData->atPoint[0].Row);    
            #endif
        } 
        else
        {
            u32 x2, y2;

            /* Finger 1 */
            touchData->atPoint[0].Col = (u32X * TOUCH_SCREEN_WIDTH) / 2048;
            touchData->atPoint[0].Row = (u32Y * TOUCH_SCREEN_HEIGHT) / 2048;   

            /* Keep final X and Y coordinate*/
            uFinalX = touchData->atPoint[0].Col;
            uFinalY = touchData->atPoint[0].Row;
            
            /* Finger 2 */
            if(delta_x > 2048)      //transform the unsigh value to sign value
            {
                delta_x -= 4096;
            }
            if(delta_y > 2048)
            {
                delta_y -= 4096;
            }        
            touchData->nFingerNum = 2; //two touch  only draw rectangular  
            x2 = (u32)(u32X + delta_x);
            y2 = (u32)(u32Y + delta_y);
        
            touchData->atPoint[1].Col = (x2 * TOUCH_SCREEN_WIDTH) / 2048;
            touchData->atPoint[1].Row = (y2 * TOUCH_SCREEN_HEIGHT) / 2048;
			
            /* Calibrate if the touch panel was reversed in Y */
            #if 0
            touchData->atPoint[1].Row = (TOUCH_SCREEN_HEIGHT - y2);
            #endif
            
        }

        if((u32X > 0x7FF) || (u32Y > 0x7FF)) //touch edge
        {
   
			return 1;
        }
        else
        {   // return 0
            s32 x2 = (u32X + delta_x);
            s32 y2 = (u32Y + delta_y);
            if((x2 < 0) || (x2 > 0x7FF) || (y2 < 0) || (y2 > 0x7FF)) //touch edge
            {
  
                return 1;
            }
            else   //touching now
            {
  
                return 1;
            }
        }
            
    }
    else
    {
   
        return 0;
    }
}


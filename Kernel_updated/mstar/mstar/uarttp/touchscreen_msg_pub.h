/**
 * \file touchscreen_msg_pub.h
 * Description:  Messages interface for Battery Task
*/
#include <linux/serial_8250.h>

#ifndef __TOUCHSCREEN_MSG_PUB_H__
#define __TOUCHSCREEN_MSG_PUB_H__

#define TOUCHSCREEN_RELEASED  0
#define TOUCHSCREEN_PRESSED    1

//#include "vm_msgof.hc"
//#include "vm_os.hi"
//#include "vm_abb.h"
//#include "device.ho"

/* ------------------------ Messages Types -------------------------- */
/**
 * \brief Union of all messages received or sent by Touchscreen
*/

typedef struct
{
    u16     x;
    u16     y;
    u16     pressure;
    u16     mode;

} vm_Touchscreen_data_t;

#if 1
#define MAX_TOUCH_FINGER 2
typedef struct
{
    u16 Row;
    u16 Col;
} vm_TouchPoint_t;

typedef struct
{
    u8 nFunction; 
    u8 nKeyMode;
    u8 nFingerNum;
    vm_TouchPoint_t atPoint[MAX_TOUCH_FINGER];
}MdlTouchScreenInfo_t;

#else
#define MAX_TOUCH_POINT_SUPPORT 2
typedef struct
{
    u8       function;
    u8       KeyMode;         /**< Key mode define: 1: touch release 2: touch press */
    u8       FingerNum;       /**< Finger number (0,1,2)*/
    vm_TouchPoint_t point[MAX_TOUCH_POINT_SUPPORT];
} vm_Touchscreen_info_t;

typedef struct
{
    u8      KeyMode;
    u16     Row;
    u16     Col;
} vm_Touchscreen_info_t;

typedef union
{
    vm_osTimerMsgBody_t     TimerMsg;
    vm_Touchscreen_data_t   tsData;
    vm_Touchscreen_info_t   tsInfo;
    DrvAbbAdcResults_t      adcResults;
}vm_msgBody_t;

#include "vm_msgt.ht"
#endif

typedef struct{
   u8       KeyMode;
   u16      Row;
   u16      Col;
} Body_t;

#endif  /* __TOUCHSCREEN_MSG_H__ */


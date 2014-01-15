////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvUART.c
/// @brief  UART Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
//#include "MsCommon.h"
//#include "MsVersion.h"
//#include <string.h>
//#include "MsIRQ.h"
//#include "drvMMIO.h"

// Internal Definition
#include "drvUART.h"
#include "halUART.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <asm/irq.h>
#include <asm/gpio.h>

#include <asm/io.h>
#include "hal_touch_screen_c_msg20xx.h"
#include "drv_touch_screen_pub.h"
#include "touchscreen_msg_pub.h"

#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/serial_reg.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/nmi.h>
#include <linux/mutex.h>
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

MS_U8 FASTU_ReadByte(unsigned int regIdx)
{
   return FASTU_REG8(regIdx);
}
void FASTU_WriteByte(unsigned int regIdx, MS_U8 value)
{
   FASTU_REG8(regIdx) = value;
}
 


MS_U16 mdrv_uart_mstp_set_baudrate(MS_U32 baudrate)
{
    return HAL_UART_MSTP_PIU_Set_Baudrate(baudrate);
}

void MDrv_UART_MSTP_Init(MS_U32 baudrate)
{
    // default initialize
    mdrv_uart_mstp_set_baudrate(baudrate) ; // set default baud rate

}

MS_U16 mdrv_uart_mstp_connect()
{
    MS_U16 u16Ret;
    u16Ret = HAL_UART_MSTP_Connect();
    return u16Ret;
}

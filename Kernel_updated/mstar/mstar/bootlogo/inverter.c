////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2011-2012 MStar Semiconductor, Inc.
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
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    sec_logo.c
/// @brief  Show LOGO when boot up.
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
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
#include <linux/delay.h>

#include "SdMisc.h"

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define UART_DIVISOR    (clk/(16 * baud_rate))
#define UART_DIVISOR_H  (UART_DIVISOR >> 8)
#define UART_DIVISOR_L  (UART_DIVISOR & 0xff)
#define REG_UART1_BASE              0xFD220C00 //UART 1

#define UART_TX		                0
#define UART_LCR                3           // Out: Line Control Register
#define UART_LSR                5           // In:  Line Status Register
#define UART_LSR_THRE		        0x20	                            // Transmit-hold-register empty
#define UART_LSR_TEMT		        0x40	                            // Transmitter empty
#define BOTH_EMPTY                  (UART_LSR_TEMT | UART_LSR_THRE)

#define UART1_REG8(addr)            *((volatile unsigned int*)(REG_UART1_BASE + ((addr)<< 3)))
#define UART_REG8(addr)             UART1_REG8(addr) //$ default console output - FUART (not UART2)

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

void micom_putc (char c)
{
    unsigned char u8Reg;
	// unsigned int u32Timer;

	//Wait for Transmit-hold-register empty
	//UART_TIME(u32Timer,10)  //10ms for OS version
	do {
        		printk("micom_putc 111 \n");
		u8Reg = UART1_REG8(UART_LSR);
		if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
		{
			break;
		}
	}while(1);

	UART1_REG8(UART_TX) = c; //put char

	//Wait for both Transmitter empty & Transmit-hold-register empty
	//	UART_TIME(u32Timer,10)  //10ms for OS version
	do {
        		printk("micom_putc 2222\n");

		u8Reg = UART1_REG8(UART_LSR);
		if ((u8Reg & BOTH_EMPTY) == BOTH_EMPTY)
		{
			break;
		}
	}while(1);

}

/*
    Command List

    toggle   : 0x35
    shutdown : 0x12
    reboot   : 0x1d
    rollback : 0x34
    ... : 0xd0 , 1
    ... : 0xd1 , 1
*/
static int micom_cmd(int command, int option)
{
    unsigned char w_databuff[9];
    // unsigned char r_databuff[255];
    // int retry = 5;
    int i = 0;

    // while(retry-- > 0)
    {
        memset(w_databuff, 0, 9);
        // memset(r_databuff, 0, 255);

        w_databuff[0] = 0xff;
        w_databuff[1] = 0xff;
        w_databuff[2] = command;
        w_databuff[3] = option;

		/* Making Check Sum  */
		w_databuff[8] = w_databuff[2];
		w_databuff[8] += w_databuff[3];

        //Send command to MICOM
        for(i=0; i<9; i++)
        {
            micom_putc(w_databuff[i]);
        }
        // mdelay(200);
    }

    return 0;
}

int inverter_init(void)
{
	/* PVCC ON */
	//micom_cmd(0xd0,1);		//apply to X6, X5

	//delay(1000000);

	/* INVERTER ON */

    micom_cmd(0xcd,1);		//change submicom command '0xd1' -> '0xcd'

    return 0;
}


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
////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "drvUART.h"
// Internal Definition
#include "halUART.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------




//conect D14 to UART1
MS_U16 HAL_UART_MSTP_Connect(void)
{
   	GENERAL_WRITE_REG8(0x101E00, 0xA6, (7 << 4), BITMASK(7:4));
//	GENERAL_WRITE_REG8(0x101E00, 0xA7, 0x11, BITMASK(7:0));
	GENERAL_WRITE_REG8(0x101E00, 0x05, 1, BITMASK(1:0));
	return 0;
}

int HAL_UART_MSTP_PIU_Set_Baudrate(MS_U32 baudrate)
{
    MS_U32 divisor;
    //i.	Set "reg_mcr_loopback";
    FASTU_REG8(UART_MCR) |= 0x10;	
    //	Clear FIFO Buffer
    FASTU_REG8(UART_FCR) |= 0x07;	
#define UART_USR	   (7 * 2)
    //ii.	Poll "reg_usr_busy" till 0;
    while(FASTU_REG8(UART_USR) & 0x01);	
//	printk("UART_MCR = %08x\n", FASTU_REG8(UART_MCR));
    /* Set baud rate */
    //divisor = mdrv_clkgen_get(CKG_UART) / (16 * baudrate);
    divisor = ((UART_CLK_FREQ) / (16 * baudrate));
	//printk("divisor = 0x%08x\n", divisor);	
    FASTU_REG8(UART_LCR) |= UART_LCR_DLAB;  
	//printk("FASTU_REG8(UART_LCR) =%08x\n", &FASTU_REG8(UART_LCR));		
    FASTU_REG8(UART_DLL) = divisor & 0xFF;
    FASTU_REG8(UART_DLM) =  (divisor>>8) & 0xFF;
    FASTU_REG8(UART_LCR) &= ~(UART_LCR_DLAB);	
    FASTU_REG8(UART_MCR) &= ~0x10;
    return 0;
}

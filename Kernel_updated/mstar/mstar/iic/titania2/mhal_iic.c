////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
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

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
#include <linux/undefconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#include "mhal_iic.h"
#include "mhal_iic_reg.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void MHal_IIC_Init(void)
{
    MHal_CHIP_REG(REG_IIC_MODE) &= ~BIT15;


    #if 0
    //iic master mode is enabled, bit 9~8 set to 2  
    MHal_CHIP_REG(REG_IIC_MODE) |= BIT9;
    MHal_CHIP_REG(REG_IIC_MODE) &= ~BIT8;
    MHal_CHIP_REG(REG_IIC_MODE) |= BIT10;

    #endif


    //
    // Since we always use sw iic, must disable hw iic here.
    //
    
    #if 1
    //iic master mode is disable, bit 9~8 set to 0  
    MHal_CHIP_REG(REG_IIC_MODE) &= ~BIT9;
    MHal_CHIP_REG(REG_IIC_MODE) &= ~BIT8;
    #endif

	// clock = (Xtal / 4T) / div
	MHal_IIC_REG(REG_IIC_CLK_SEL) = 3; // 28.8M / 4 / 16 = 450KHz
	MHal_IIC_REG(REG_IIC_CTRL) = 0x80; // enable IIC
}

void MHal_IIC_Clock_Select(U8 u8ClockIIC)
{
	//master iic clock select.
	//if 1:clk/4, if 2:clk/8, if 3:clk/16, if 4:clk/32,
	//if 5:clk/64, if 6:clk/128, if 7:clk/256, if 8:clk/512,
	//if 9:clk/1024, otherwise: clk/2
	MHal_IIC_REG(REG_IIC_CLK_SEL) = u8ClockIIC;
}

void MHal_IIC_Start(void)
{
	//reset iic circuit
	MHal_IIC_REG(REG_IIC_STATUS) = 0x08;
	MHal_IIC_REG(REG_IIC_STATUS) = 0x00;

	//reset iic circuit
	MHal_IIC_REG(REG_IIC_CTRL) = 0xC0;
}

void MHal_IIC_Stop(void)
{
	MHal_IIC_REG(REG_IIC_CTRL) = 0xA0;
	MHal_IIC_DELAY();
}

void MHal_IIC_NoAck(void)
{
	MHal_IIC_REG(REG_IIC_CTRL) = 0x90;
}

B16 MHal_IIC_SendData(U8 u8DataIIC)
{
	MHal_IIC_REG(REG_IIC_STATUS) = 0x02; /* clear interrupt */
	MHal_IIC_REG(REG_IIC_WDATA) = u8DataIIC;

	while (1)
	{
		if (MHal_IIC_REG(REG_IIC_STATUS) & 0x01)
		{
			MHal_IIC_REG(REG_IIC_STATUS) = 0x02; /* clear interrupt */
			if (MHal_IIC_REG(REG_IIC_CTRL) & 0x08) /* no ACK */
			{
				return FALSE;
			}
			else
			{
                udelay(1);
				return TRUE;
			}
		}
	}

	return FALSE;
}

B16 MHal_IIC_SendByte(U8 u8DataIIC)
{
	U8 u8I;

	for(u8I=0;u8I<10;u8I++)
	{
		if(MHal_IIC_SendData(u8DataIIC) == TRUE)
			return TRUE;
	}
	return FALSE;
}

B16 MHal_IIC_GetByte(U8* pu8DataIIC) /* auto generate ACK */
{

	/* clear interrupt & start byte reading */
	MHal_IIC_REG(REG_IIC_STATUS) = 0x04;

	while(1)
	{
		if (MHal_IIC_REG(REG_IIC_STATUS))
		{
			MHal_IIC_REG(REG_IIC_STATUS) = 0x02; /* clear interrupt */
			*pu8DataIIC = MHal_IIC_REG(REG_IIC_RDATA);
            udelay(1);
			return TRUE;
		}
	}

	return FALSE;
}

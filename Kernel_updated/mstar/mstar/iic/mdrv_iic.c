#if (defined(CONFIG_MSTAR_TITANIA)||defined(CONFIG_MSTAR_TITANIA2))
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¨MStar Confidential Information〃) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_iic.c
/// @brief  IIC Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

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
#include <asm/delay.h>

#include "mst_devid.h"

#include "mdrv_iic.h"
#include "mhal_iic_reg.h"
#include "mhal_iic.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IIC_DBG_ENABLE			0

#if IIC_DBG_ENABLE
#define IIC_DBG(_f)				(_f)
#else
#define IIC_DBG(_f)
#endif

#define IIC_PRINT(fmt, args...)	//printk("[IIC][%05d] " fmt, __LINE__, ## args)

// for SW I2C
#define I2C_CH_MAX              8//3

#define _INPUT 					1
#define _OUTPUT					0
#define _HIGH  					1
#define _LOW   					0
#define SWIIC_READ 				0
#define SWIIC_WRITE				1
#define I2C_CHECK_PIN_DUMMY		200//100
#define I2C_ACKNOWLEDGE    		_LOW
#define I2C_NON_ACKNOWLEDGE		_HIGH
#define I2C_ACCESS_DUMMY_TIME	5//3

#define SWIIC_SCL_PIN(chNum, x) \
        ( MHal_GPIO_REG(_I2CBus[chNum].SclOenReg) = (x == _INPUT) ? \
         (MHal_GPIO_REG(_I2CBus[chNum].SclOenReg) |  _I2CBus[chNum].SclOenBit) : \
         (MHal_GPIO_REG(_I2CBus[chNum].SclOenReg) & ~_I2CBus[chNum].SclOenBit) )

#define SWIIC_SDA_PIN(chNum, x) \
        ( MHal_GPIO_REG(_I2CBus[chNum].SdaOenReg) = (x == _INPUT) ? \
         (MHal_GPIO_REG(_I2CBus[chNum].SdaOenReg) |  _I2CBus[chNum].SdaOenBit) : \
         (MHal_GPIO_REG(_I2CBus[chNum].SdaOenReg) & ~_I2CBus[chNum].SdaOenBit) )

#define SWIIC_SCL_OUT(chNum, x) \
        ( MHal_GPIO_REG(_I2CBus[chNum].SclOutReg) = (x == _HIGH) ? \
         (MHal_GPIO_REG(_I2CBus[chNum].SclOutReg) |  _I2CBus[chNum].SclOutBit) : \
         (MHal_GPIO_REG(_I2CBus[chNum].SclOutReg) & ~_I2CBus[chNum].SclOutBit) )

#define SWIIC_SDA_OUT(chNum, x) \
        ( MHal_GPIO_REG(_I2CBus[chNum].SdaOutReg) = (x == _HIGH) ? \
         (MHal_GPIO_REG(_I2CBus[chNum].SdaOutReg) |  _I2CBus[chNum].SdaOutBit) : \
         (MHal_GPIO_REG(_I2CBus[chNum].SdaOutReg) & ~_I2CBus[chNum].SdaOutBit) )

#define GET_SWIIC_SCL(chNum) \
        ( (MHal_GPIO_REG(_I2CBus[chNum].SclInReg) & _I2CBus[chNum].SclInBit) ? _HIGH : _LOW )

#define GET_SWIIC_SDA(chNum) \
        ( (MHal_GPIO_REG(_I2CBus[chNum].SdaInReg) & _I2CBus[chNum].SdaInBit) ? _HIGH : _LOW )

#define _SDA_HIGH(chNum)	SWIIC_SDA_PIN(chNum, _INPUT)
#define _SDA_LOW(chNum) 	do { SWIIC_SDA_OUT(chNum, _LOW); SWIIC_SDA_PIN(chNum, _OUTPUT); } while(0)

#define _SCL_HIGH(chNum)	SWIIC_SCL_PIN(chNum, _INPUT)
#define _SCL_LOW(chNum) 	do { SWIIC_SCL_OUT(chNum, _LOW); SWIIC_SCL_PIN(chNum, _OUTPUT); } while(0)

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
IIC_Bus_t _I2CBus[I2C_CH_MAX] =
{
    //channel 0
    {REG_DDCR_CK2_OEN, BIT1, REG_DDCR_CK2_OUT, BIT1, REG_DDCR_CK2_IN, BIT1, \
     REG_DDCR_DA2_OEN, BIT0, REG_DDCR_DA2_OUT, BIT0, REG_DDCR_DA2_IN, BIT0, 5},
    //channel 1
    {REG_LCK_OEN, BIT2, REG_LCK_OUT, BIT3, REG_LCK_IN, BIT3, \
     REG_LDE_OEN, BIT3, REG_LDE_OUT, BIT2, REG_LDE_IN, BIT2, 5},
    //channel 2
    {REG_I2S_IN_SD_OEN, BIT2, REG_I2S_IN_SD_OUT, BIT2, REG_I2S_IN_SD_IN, BIT2, \
     REG_I2S_IN_BCK_OEN, BIT1, REG_I2S_IN_BCK_OUT, BIT1, REG_I2S_IN_BCK_IN, BIT1, 5},
    //channel 3
    {REG_DDCROM_CK_OEN, BIT1, REG_DDCROM_CK_OUT, BIT2, REG_DDCROM_CK_IN, BIT0, \
     REG_DDCROM_DA_OEN, BIT5, REG_DDCROM_DA_OUT, BIT6, REG_DDCROM_DA_IN, BIT4, 5},
    //channel 4
    {REG_DDCA_CK_OEN, BIT1, REG_DDCA_CK_OUT, BIT2, REG_DDCA_CK_IN, BIT0, \
     REG_DDCA_DA_OEN, BIT5, REG_DDCA_DA_OUT, BIT6, REG_DDCA_DA_IN, BIT4, 5},
    //channel 5
    {REG_DDCDA_CK_OEN, BIT1, REG_DDCDA_CK_OUT, BIT2, REG_DDCDA_CK_IN, BIT0, \
     REG_DDCDA_DA_OEN, BIT5, REG_DDCDA_DA_OUT, BIT6, REG_DDCDA_DA_IN, BIT4, 5},
    //channel 6
    {REG_DDCDB_CK_OEN, BIT1, REG_DDCDB_CK_OUT, BIT2, REG_DDCDB_CK_IN, BIT0, \
     REG_DDCDB_DA_OEN, BIT5, REG_DDCDB_DA_OUT, BIT6, REG_DDCDB_DA_IN, BIT4, 5},
    //channel 7
    {REG_DDCDC_CK_OEN, BIT1, REG_DDCDC_CK_OUT, BIT2, REG_DDCDC_CK_IN, BIT0, \
     REG_DDCDC_DA_OEN, BIT5, REG_DDCDC_DA_OUT, BIT6, REG_DDCDC_DA_IN, BIT4, 5},
};

static U16 _u16SwIicDly[I2C_CH_MAX];
#if 0//PIU Timer solution
static U16 u16TimerLastCount;
#endif

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------


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

void MDrv_SW_IIC_SetSpeed(U8 u8ChIIC, U8 u8Speed)
{
    if(u8ChIIC>=I2C_CH_MAX)
        return;

    _u16SwIicDly[u8ChIIC] = ((U16)u8Speed<<1); //clock speed = 2 * u8Speed
}

#if 1
#define MCU_CLOCK_KHZ       216000UL
#define DELAY_CNT(khz)      ((MCU_CLOCK_KHZ / 4 / (khz)) - (120 - ((khz) / 50)))
//#define DELAY_CNT(khz)      ((MCU_CLOCK_KHZ / 4 / (khz)) - (112 - ((khz) / 50)))
void MDrv_SW_IIC_Delay(U8 u8ChIIC)
{
    __asm__ __volatile__ (
        ".set noreorder\n"
        "1:"
        "\tbgtz %0,1b\n"
        "\taddiu %0,%0,-1\n"
        : : "r" (DELAY_CNT(_u16SwIicDly[u8ChIIC])));
}
#endif

#if 0//PIU Timer solution
#define REG_TIMER_MAX1      (0x1E50<<2)
#define REG_TIMER_CNT1      (0x1E52<<2)
#define REG_TIMER_CTL10     (0x1E54<<2)     //[0]: capture, [1]: clear
#define REG_TIMER_CTL11     ((0x1E54<<2)+1) //[1:0]=2'b11: Enable, [1:0]=2'b00: Disable
#define TIMER_EN            (BIT1|BIT0)
#define TIMER_CLR           (BIT1)
#define TIMER_CAP           (BIT0)
#define PIU_XTAL_CLOCK_KHZ  12000UL
#define MCU_CLOCK_KHZ       216000UL
void MDrv_SW_IIC_Delay(U8 u8ChIIC)
{
    static U16 u16TimerExpCount;
    register U16 u16TimerTmpCount;
    register int i;

    //u16TimerExpCount = (PIU_XTAL_CLOCK_KHZ/800) - 8;//100;
    //printk("[IIC]: u16TimerExpCount=%d\n", u16TimerExpCount);
    //MHal_PIU_REG(REG_TIMER_CTL10) &= ~TIMER_CAP;
    while(1)
    {
        MHal_PIU_REG(REG_TIMER_CTL10) = TIMER_CAP;

        __asm__ __volatile__ (
            ".set noreorder\n"
            "1:"
            "\tbgtz %0,1b\n"
            "\taddiu %0,%0,-1\n"
            : : "r" ((MCU_CLOCK_KHZ / PIU_XTAL_CLOCK_KHZ) * 2));

        u16TimerTmpCount = (*(volatile U16*)(REG_MIPS_BASE + (REG_TIMER_CNT1)));//MHal_PIU_REG(REG_TIMER_CNT1);
        //if(u16TimerTmpCount==u16TimerLastCount)
        //    continue;

        if(u16TimerTmpCount>=((PIU_XTAL_CLOCK_KHZ/800) - 8))
        {
            MHal_PIU_REG(REG_TIMER_CTL10) = TIMER_CLR;
            //u16TimerLastCount = u16TimerTmpCount;
            break;
        }
    }
}
#endif

#if 0
void MDrv_SW_IIC_Delay(U8 u8ChIIC)
{
    U8 u8I;
    U8 u8Loop = 5;

    if(u8ChIIC>=I2C_CH_MAX)
        return;

    u8Loop = _u16SwIicDly[u8ChIIC];
    if(u8Loop<2)
        u8Loop = 2;
    //for fast mode
    if (u8Loop == 2)
        for (u8I=0;u8I<17;u8I++)
            ;
    else
    //for normal and slow mode
    {
        u8Loop = (u8Loop - 2) * 2;
        while (u8Loop-- > 0)
        {
         udelay(1);
        }
    }
}
#endif

void MDrv_SW_IIC_SCL(U8 u8ChIIC, U8 u8Data)
{
    if(u8ChIIC>=I2C_CH_MAX)
        return;

    if ( u8Data == _HIGH )
    {
        SWIIC_SCL_PIN(u8ChIIC, _INPUT);  //set to input
    }
    else
    {
        SWIIC_SCL_OUT(u8ChIIC, _LOW);
        SWIIC_SCL_PIN(u8ChIIC, _OUTPUT);
    }
}

void MDrv_SW_IIC_SDA(U8 u8ChIIC, U8 u8Data)
{
    if(u8ChIIC>=I2C_CH_MAX)
        return;

    if ( u8Data == _HIGH )
    {
        SWIIC_SDA_PIN(u8ChIIC, _INPUT);  //set to input
    }
    else
    {
        SWIIC_SDA_OUT(u8ChIIC, _LOW);
        SWIIC_SDA_PIN(u8ChIIC, _OUTPUT); //set to output
    }
}

void MDrv_SW_IIC_SCL_Chk(U8 u8ChIIC, B16 bSet)
{
    U8 u8Dummy;       // loop dummy

    if(u8ChIIC>=I2C_CH_MAX)
        return;

    if (bSet == _HIGH)  // if set pin high
    {
        SWIIC_SCL_PIN(u8ChIIC, _INPUT);
        u8Dummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((GET_SWIIC_SCL(u8ChIIC) == _LOW) && (u8Dummy--)) ; // check SCL pull high
    }
    else
    {
        MDrv_SW_IIC_SCL(u8ChIIC, _LOW);    // set SCL pin
        SWIIC_SCL_PIN(u8ChIIC, _OUTPUT);
    }
}

void MDrv_SW_IIC_SDA_Chk(U8 u8ChIIC, B16 bSet)
{
    U8 u8Dummy;       // loop dummy

    if(u8ChIIC>=I2C_CH_MAX)
        return;

    if (bSet == _HIGH)  // if set pin high
    {
        SWIIC_SDA_PIN(u8ChIIC, _INPUT);
        u8Dummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((GET_SWIIC_SDA(u8ChIIC) == _LOW) && (u8Dummy--)) ; // check SDA pull high
    }
    else
    {
        MDrv_SW_IIC_SDA(u8ChIIC, _LOW);    // set SDA pin
        SWIIC_SDA_PIN(u8ChIIC, _OUTPUT);
    }
}

//-------------------------------------------------------------------------------------------------
// SW I2C: start signal.
// <comment>
//  SCL ________
//              \_________
//  SDA _____
//           \____________
//
// Return value: None
//-------------------------------------------------------------------------------------------------
B16 MDrv_SW_IIC_Start(U8 u8ChIIC)
{
    B16 bStatus = TRUE;    // success status

    if(u8ChIIC>=I2C_CH_MAX)
        return FALSE;

    #if 0//PIU Timer solution
    //MHal_PIU_REG(REG_TIMER_CTL10) = TIMER_CLR;
    #endif

    MDrv_SW_IIC_SDA_Chk(u8ChIIC, _HIGH);
    MDrv_SW_IIC_Delay(u8ChIIC);

    MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);
    MDrv_SW_IIC_Delay(u8ChIIC);

    // check pin error
	SWIIC_SCL_PIN(u8ChIIC, _INPUT);
 	SWIIC_SDA_PIN(u8ChIIC, _INPUT);


    if ((GET_SWIIC_SCL(u8ChIIC) == _LOW) || (GET_SWIIC_SDA(u8ChIIC) == _LOW))
    {
   		SWIIC_SCL_PIN(u8ChIIC, _OUTPUT);
   		SWIIC_SDA_PIN(u8ChIIC, _OUTPUT);
        bStatus = FALSE;
    }
    else // success
    {
        MDrv_SW_IIC_SDA(u8ChIIC, _LOW);
        MDrv_SW_IIC_Delay(u8ChIIC);
        MDrv_SW_IIC_SCL(u8ChIIC, _LOW);
    }

    return bStatus;	 //vain
}

//-------------------------------------------------------------------------------------------------
// SW I2C: stop signal.
// <comment>
//              ____________
//  SCL _______/
//                 _________
//  SDA __________/
//-------------------------------------------------------------------------------------------------
void MDrv_SW_IIC_Stop(U8 u8ChIIC)
{
    if(u8ChIIC>=I2C_CH_MAX)
        return;

	_SCL_LOW(u8ChIIC);

    MDrv_SW_IIC_Delay(u8ChIIC);
    _SDA_LOW(u8ChIIC);

    MDrv_SW_IIC_Delay(u8ChIIC);
    _SCL_HIGH(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);
    _SDA_HIGH(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);
}

//-------------------------------------------------------------------------------------------------
///SW I2C: Send 1 bytes data
///@param u8data \b IN: 1 byte data to send
//-------------------------------------------------------------------------------------------------
B16 MDrv_IIC_SendByte(U8 u8ChIIC, U8 u8data)   // Be used int IIC_SendByte
{
    U8      u8Mask = 0x80;
    B16     bAck; // acknowledge bit

    if(u8ChIIC>=I2C_CH_MAX)
        return I2C_NON_ACKNOWLEDGE;

    while ( u8Mask )
    {
        if (u8data & u8Mask)
        {
            MDrv_SW_IIC_SDA_Chk(u8ChIIC, _HIGH);
        }
        else
        {
            MDrv_SW_IIC_SDA_Chk(u8ChIIC, _LOW);
        }

        MDrv_SW_IIC_Delay(u8ChIIC);
        MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH); // clock
        MDrv_SW_IIC_Delay(u8ChIIC);
        MDrv_SW_IIC_SCL(u8ChIIC, _LOW);

        //MDrv_SW_IIC_Delay(u8ChIIC);

        u8Mask >>= 1; // next
    }

    // recieve acknowledge
    SWIIC_SDA_PIN(u8ChIIC, _INPUT);
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);

    MDrv_SW_IIC_Delay(u8ChIIC);
    bAck = GET_SWIIC_SDA(u8ChIIC); // recieve acknowlege
//	SWIIC_SDA(u8ChIIC, bAck);     //for I2c waveform sharp
//	SWIIC_SDA_PIN(u8ChIIC, _OUTPUT);
    MDrv_SW_IIC_SCL(u8ChIIC, _LOW);

    MDrv_SW_IIC_Delay(u8ChIIC);

    MDrv_SW_IIC_SDA(u8ChIIC, bAck);     //for I2c waveform sharp
    SWIIC_SDA_PIN(u8ChIIC, _OUTPUT);

    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);

    return (bAck);
}

//-------------------------------------------------------------------------------------------------
///SW I2C: Send 1 bytes data, this function will retry 5 times until success.
///@param u8data \b IN: 1 byte data to send
///@return BOOLEAN:
///- TRUE: Success
///- FALSE: Fail
//-------------------------------------------------------------------------------------------------
B16 MDrv_SW_IIC_SendByte(U8 u8ChIIC, U8 u8data, U8 u8Delay4Ack )
{
	U8 u8I;

    if(u8ChIIC>=I2C_CH_MAX)
        return FALSE;

	for(u8I=0;u8I<5;u8I++)
	{
	    if (MDrv_IIC_SendByte(u8ChIIC, u8data) == I2C_ACKNOWLEDGE)
		    return TRUE;
	}

	//printk("IIC write byte 0x%02x fail!!\n", u8data);
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
// SW I2C: access start.
//
// Arguments: u8SlaveAdr - slave address
//            u8Trans - I2C_TRANS_WRITE/I2C_TRANS_READ
//-------------------------------------------------------------------------------------------------
B16 MDrv_SW_IIC_AccessStart(U8 u8ChIIC, U8 u8SlaveAdr, U8 u8Trans)
{
    U8 u8Dummy; // loop dummy

    if(u8ChIIC>=I2C_CH_MAX)
        return FALSE;

    if (u8Trans == SWIIC_READ) // check i2c read or write
    {
        u8SlaveAdr = u8SlaveAdr | 0x01; // read
    }
    else
    {
        u8SlaveAdr = u8SlaveAdr & 0xfe; // write
    }

    u8Dummy = I2C_ACCESS_DUMMY_TIME;

    while (u8Dummy--)
    {
        if ( MDrv_SW_IIC_Start(u8ChIIC) == FALSE)
        {
            printk("MDrv_SW_IIC_Start=>Failed\n");
            continue;
        }

        if ( MDrv_SW_IIC_SendByte(u8ChIIC, u8SlaveAdr, 0) == TRUE )   //I2C_ACKNOWLEDGE) // check acknowledge
        {
            return TRUE;
        }

        MDrv_SW_IIC_Stop(u8ChIIC);
    }

    return FALSE;
}

//-------------------------------------------------------------------------------------------------
///SW I2C: Get 1 bytes data
///@param u16Ack  \b IN: acknowledge
///@return U8:    \b OUT: get data from the device
///- TRUE: Success
///- FALSE: Fail
//-------------------------------------------------------------------------------------------------
U8 MDrv_SW_IIC_GetByte (U8 u8ChIIC, U16 u16Ack)
{
    U8 u8Receive = 0;
    U8 u8Mask = 0x80;

    if(u8ChIIC>=I2C_CH_MAX)
        return 0;

    SWIIC_SDA_PIN(u8ChIIC, _INPUT);		  //SWIIC_SDA_PIN

    while ( u8Mask )
    {
        MDrv_SW_IIC_Delay(u8ChIIC);
        MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);		//SWIIC_SCL_Chk
        MDrv_SW_IIC_Delay(u8ChIIC);

        if (GET_SWIIC_SDA(u8ChIIC) == _HIGH)	  //GET_SWIIC_SDA
        {
            u8Receive |= u8Mask;
        }
        u8Mask >>= 1; // next

        MDrv_SW_IIC_SCL(u8ChIIC, _LOW);	  //SWIIC_SCL
        //SWIIC_Delay();

    }
    if (u16Ack)
    {
        // acknowledge
        MDrv_SW_IIC_SDA_Chk(u8ChIIC, I2C_ACKNOWLEDGE);	  //SWIIC_SDA_Chk
    }
    else
    {
        // non-acknowledge
        MDrv_SW_IIC_SDA_Chk(u8ChIIC, I2C_NON_ACKNOWLEDGE);
    }
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);	//SWIIC_SCL_Chk
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_SCL(u8ChIIC, _LOW);		 //SWIIC_SCL
    MDrv_SW_IIC_Delay(u8ChIIC);


    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);

    return u8Receive;
}

//-------------------------------------------------------------------------------------------------
///SW I2C: Write bytes, be able to write 1 byte or several bytes to several register offsets in same slave address.
///@param u8SlaveID \b IN: Slave ID (Address)
///@param u8AddrCnt \b IN:  register NO to write, this parameter is the NO of register offsets in pu8addr buffer,
///it should be 0 when *pu8Addr = NULL.
///@param *pu8Addr \b IN: pointer to a buffer containing target register offsets to write
///@param u32BufLen \b IN: Data length (in byte) to write
///@param *pu8Buf \b IN: pointer to the data buffer for write
///@return BOOLEAN:
///- TRUE: Success
///- FALSE: Fail
//-------------------------------------------------------------------------------------------------
S32 MDrv_SW_IIC_Write(U8 u8ChIIC, U8 u8SlaveID, U8 u8AddrCnt, U8* pu8Addr, U32 u32BufLen, U8* pu8Buf)
{
    U8  u8Dummy; // loop dummy
    S32 s32RetCountIIC;

    if(u8ChIIC>=I2C_CH_MAX)
        return 0;

    u8Dummy = 1;  //I2C_ACCESS_DUMMY_TIME;
    s32RetCountIIC = u32BufLen;

    while (u8Dummy--)
    {
        if (MDrv_SW_IIC_AccessStart(u8ChIIC, u8SlaveID, SWIIC_WRITE) == FALSE)
        {
            s32RetCountIIC = -2;
            goto SW_IIC_Write_End;
        }

		while( u8AddrCnt )
		{
			  u8AddrCnt--;
			  if ( MDrv_SW_IIC_SendByte(u8ChIIC, *pu8Addr, 0) == FALSE )
              {
			    s32RetCountIIC = -3;
                goto SW_IIC_Write_End;
              }
			  pu8Addr++;
		}
        while (u32BufLen) // loop of writting data
        {
            u32BufLen-- ;
            //IIC_SendByte(*pu8Buf); // send byte
            if ( MDrv_SW_IIC_SendByte(u8ChIIC, *pu8Buf, 0) == FALSE )
            {
			    s32RetCountIIC = -4;
                goto SW_IIC_Write_End;
            }
            pu8Buf++; // next byte pointer
        }

        break;
    }

SW_IIC_Write_End:
    MDrv_SW_IIC_Stop(u8ChIIC);

    return s32RetCountIIC;
}


//-------------------------------------------------------------------------------------------------
///SW I2C: Read bytes, be able to read 1 byte or several bytes from several register offsets in same slave address.
///@param u8SlaveID \b IN: Slave ID (Address)
///@param u8AddrCnt \b IN:  register NO to read, this parameter is the NO of register offsets in pu8addr buffer,
///it should be 0 when *paddr = NULL.
///@param *pu8Addr \b IN: pointer to a buffer containing target register offsets to read
///@param u32BufLen \b IN: Data length (in byte) to read
///@param *pu8Buf \b IN: pointer to retun data buffer.
///@return BOOLEAN:
///- TRUE: Success
///- FALSE: Fail
//-------------------------------------------------------------------------------------------------
S32 MDrv_SW_IIC_Read(U8 u8ChIIC, U8 u8SlaveID, U8 u8AddrCnt, U8* pu8Addr, U32 u32BufLen, U8* pu8Buf)
{
    U8  u8Dummy; // loop dummy
    S32 s32RetCountIIC;

    if(u8ChIIC>=I2C_CH_MAX)
        return 0;

    u8Dummy = I2C_ACCESS_DUMMY_TIME;
    s32RetCountIIC = u32BufLen;

    while (u8Dummy--)
    {
        /*	20080812:
        	fixed:	Don't make a I2C's write start condition when u8AddrCnt == 0
        */
        if( u8AddrCnt > 0 )
        {
            if (MDrv_SW_IIC_AccessStart(u8ChIIC, u8SlaveID, SWIIC_WRITE) == FALSE)
            {
                s32RetCountIIC = -2;
                goto SW_IIC_Read_End;
            }

            while( u8AddrCnt )
            {
                u8AddrCnt--;
                if (MDrv_SW_IIC_SendByte(u8ChIIC, *pu8Addr, 0) == FALSE)
                {
                    s32RetCountIIC = -3;
                    goto SW_IIC_Read_End;
                }
                pu8Addr++;
            }
        }

        if (MDrv_SW_IIC_AccessStart(u8ChIIC, u8SlaveID, SWIIC_READ) == FALSE)
        {
            s32RetCountIIC = -4;
            goto SW_IIC_Read_End;
        }

        while (u32BufLen--) // loop to burst read
        {
            *pu8Buf = MDrv_SW_IIC_GetByte(u8ChIIC, u32BufLen); // receive byte

            pu8Buf++; // next byte pointer
        }

        break;
    }

SW_IIC_Read_End:
    MDrv_SW_IIC_Stop(u8ChIIC);

    return s32RetCountIIC;
}



//-------------------------------------------------------------------------------------------------
/// IIC master initialization
/// @return None
/// @note   Hardware IIC. Called only once at system initialization
//-------------------------------------------------------------------------------------------------
void MDrv_IIC_Init(void)
{
    int chidx;

    MHal_IIC_Init();

    //set all pads(except SPI) as input
    MHal_GPIO_REG(REG_ALL_PAD_IN) &= ~BIT7;

    //for channel 0 set
    //REG_DDCR_CK2_SET
    //REG_DDCR_DA2_SET
    //for channel 1 set
    MHal_GPIO_REG(REG_LCK_SET) |= BIT3;
    MHal_GPIO_REG(REG_LDE_SET) |= BIT2;
    //for channel 2 set
    MHal_GPIO_REG(REG_I2S_IN_SD_SET) |= BIT4;
    MHal_GPIO_REG(REG_I2S_IN_BCK_SET) |= BIT4;
    //for channel 3 set
    MHal_GPIO_REG(REG_DDCROM_CK_SET) |= BIT7;
    MHal_GPIO_REG(REG_DDCROM_DA_SET) |= BIT7;
    //for channel 4 set
    MHal_GPIO_REG(REG_DDCA_CK_SET) |= BIT7;
    MHal_GPIO_REG(REG_DDCA_DA_SET) |= BIT7;
    //for channel 5 set
    MHal_GPIO_REG(REG_DDCDA_CK_SET) |= BIT7;
    MHal_GPIO_REG(REG_DDCDA_DA_SET) |= BIT7;
    //for channel 6 set
    MHal_GPIO_REG(REG_DDCDB_CK_SET) |= BIT7;
    MHal_GPIO_REG(REG_DDCDB_DA_SET) |= BIT7;
    //for channel 7 set
    MHal_GPIO_REG(REG_DDCDC_CK_SET) |= BIT7;
    MHal_GPIO_REG(REG_DDCDC_DA_SET) |= BIT7;

    for(chidx=0;chidx<I2C_CH_MAX;chidx++)
    {
        _u16SwIicDly[chidx] = 100; //100KHz //_I2CBus[chidx].DefDelay;
    }
    #if 0//PIU Timer solution
    MHal_PIU_REG(REG_TIMER_MAX1)=0;
    MHal_PIU_REG(REG_TIMER_MAX1+1)=0;

    //enable PIU timer1 for I2C speed control
    MHal_PIU_REG(REG_TIMER_CTL11) = TIMER_EN;
    u16TimerLastCount=MHal_PIU_REG(REG_TIMER_CNT1);
    #endif
}

//-------------------------------------------------------------------------------------------------
/// IIC clock selection
/// @param  u8ClockIIC            \b IN:  clock selection
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_HW_IIC_Clock_Select(U8 u8ClockIIC)
{
    MHal_IIC_Clock_Select(u8ClockIIC);
}

//-------------------------------------------------------------------------------------------------
/// Write data to an IIC device
/// @param  u8SlaveIdIIC            \b IN:  device slave ID
/// @param  u8AddrSizeIIC           \b IN:  address length in bytes
/// @param  pu8AddrIIC              \b IN:  pointer to the start address of the device
/// @param  u32BufSizeIIC           \b IN:  number of bytes to be written
/// @param  pu8BufIIC               \b IN:  pointer to write data buffer
/// @return TRUE(succeed), FALSE(fail)
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
S32 MDrv_HW_IIC_Write(U8 u8SlaveIdIIC, U8 u8AddrSizeIIC, U8 *pu8AddrIIC, U32 u32BufSizeIIC, U8 *pu8BufIIC)
{
    U32     u32I;
    U8      u8I;
    B16     bReturnIIC = TRUE;
    S32     s32RetCountIIC = -1;

    MHal_IIC_Start();

    if (FALSE == MHal_IIC_SendByte(u8SlaveIdIIC&0xFE))
    {
        bReturnIIC = FALSE;
        s32RetCountIIC = -2;
        goto HW_IIC_Write_End;
    }

    for (u8I = 0; u8I < u8AddrSizeIIC; u8I++)
    {
        if (FALSE == MHal_IIC_SendByte(pu8AddrIIC[u8I]))
        {
            bReturnIIC = FALSE;
            s32RetCountIIC = -3;
            goto HW_IIC_Write_End;
        }
    }

    for (u32I = 0; u32I < u32BufSizeIIC; u32I++)
    {
        if (FALSE == MHal_IIC_SendByte(pu8BufIIC[u32I]))
        {
            bReturnIIC = FALSE;
            s32RetCountIIC = -4;
            goto HW_IIC_Write_End;
        }
    }

    s32RetCountIIC = u32BufSizeIIC;

HW_IIC_Write_End:
    MHal_IIC_Stop();

    IIC_DBG(printk("MDrv_IIC_Write() --> s32RetCountIIC=%d \n", s32RetCountIIC));
    return s32RetCountIIC;
}


//-------------------------------------------------------------------------------------------------
/// Read data from an IIC device
/// @param  u8SlaveIdIIC            \b IN:  device slave ID
/// @param  u8AddrSizeIIC           \b IN:  address length in bytes
/// @param  pu8AddrIIC              \b IN:  ptr to the start address inside the device
/// @param  u32BufSizeIIC           \b IN:  number of bytes to be read
/// @param  pu8BufIIC               \b OUT: pointer to read data buffer
/// @return TRUE : succeed
/// @return FALSE : fail
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
S32 MDrv_HW_IIC_Read(U8 u8SlaveIdIIC, U8 u8AddrSizeIIC, U8 *pu8AddrIIC, U32 u32BufSizeIIC, U8 *pu8BufIIC)
{
    U32     u32I;
    U8      u8I;
    B16     bReturnIIC = TRUE;
    S32     s32RetCountIIC = -1;

    MHal_IIC_Start();

    if (FALSE == MHal_IIC_SendByte(u8SlaveIdIIC&0xFE))
    {
        bReturnIIC = FALSE;
        s32RetCountIIC = -2;
        goto HW_IIC_Read_End;
    }

    for (u8I = 0; u8I < u8AddrSizeIIC; u8I++)
    {
        if (FALSE == MHal_IIC_SendByte(pu8AddrIIC[u8I]))
        {
            bReturnIIC = FALSE;
            s32RetCountIIC = -3;
            goto HW_IIC_Read_End;
        }
    }

    MHal_IIC_Start();

	udelay(100);

    if (FALSE == MHal_IIC_SendByte(u8SlaveIdIIC|0x1))
    {
        bReturnIIC = FALSE;
        s32RetCountIIC = -4;
        goto HW_IIC_Read_End;
    }

    for (u32I = 0; u32I < u32BufSizeIIC; u32I++)
    {
        if (u32I == (u32BufSizeIIC-1))
        {
            MHal_IIC_NoAck();
        }

        if (FALSE == MHal_IIC_GetByte(&pu8BufIIC[u32I]))
        {
            bReturnIIC = FALSE;
            s32RetCountIIC = -5;
            goto HW_IIC_Read_End;
        }
    }

    s32RetCountIIC = u32BufSizeIIC;

HW_IIC_Read_End:
    MHal_IIC_Stop();

    IIC_DBG(printk("MDrv_IIC_Read() --> s32RetCountIIC=%d \n", s32RetCountIIC));
    return s32RetCountIIC;
}

#else
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¨MStar Confidential Information〃) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_iic.c
/// @brief  IIC Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
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

#include "mst_devid.h"

#include "mdrv_iic.h"
#include "mhal_iic_reg.h"
#include "mhal_iic.h"
#include "mdrv_gpio.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#define SWI2C_LOCK      1
#if SWI2C_LOCK
spinlock_t _iic_spinlock;
#define MDrv_SW_IIC_InitLock()      spin_lock_init(&_iic_spinlock)
#define MDrv_SW_IIC_Lock()          spin_lock_irq(&_iic_spinlock)
#define MDrv_SW_IIC_UnLock()        spin_unlock_irq(&_iic_spinlock)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IIC_DBG_ENABLE           0

#if IIC_DBG_ENABLE
#define IIC_DBG(_f)             (_f)
#else
#define IIC_DBG(_f)
#endif

#if 0
#define LINE_DBG()             printf("IIC %d\n", __LINE__)
#else
#define LINE_DBG()
#endif

#define IIC_PRINT(fmt, args...)    //printk("[IIC][%05d] " fmt, __LINE__, ## args)

// for SW I2C
#define _INPUT                  1
#define _OUTPUT                 0
#define _HIGH                   1
#define _LOW                    0
#define SWIIC_READ              0
#define SWIIC_WRITE             1
#define I2C_CHECK_PIN_DUMMY     3200/*6000*//*3200*/
#define I2C_ACKNOWLEDGE         _LOW
#define I2C_NON_ACKNOWLEDGE     _HIGH
#define I2C_ACCESS_DUMMY_TIME   5//3


#define NEW_DELAY_FORMULA   1
#define EXTRA_DELAY_CYCLE   1
#define FACTOR_DLY    69000UL
#define FACTOR_ADJ    11040UL
static U32 u32CpuSpeedMHz;
static U32 u32AdjParam;
//#define DELAY_CYCLES(SpeedKHz)  ((FACTOR_DLY/(SpeedKHz))-((110+u32AdjParam)-((SpeedKHz)/u32AdjParam)))
#define DELAY_CYCLES(SpeedKHz)  ((FACTOR_DLY/(SpeedKHz))-((130+u32AdjParam)-((SpeedKHz)/u32AdjParam))+((1<<((440-SpeedKHz)/40))))
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
I2C_BusCfg_t g_I2CBusCfg[IIC_NUM_OF_MAX];

#define SWIIC_SCL_PIN(chNum, x) \
    ((x == _INPUT) ? MDrv_GPIO_Pad_Odn(g_I2CBusCfg[chNum].u16PadSCL) : MDrv_GPIO_Pad_Oen(g_I2CBusCfg[chNum].u16PadSCL))

#define SWIIC_SDA_PIN(chNum, x) \
    ((x == _INPUT) ? MDrv_GPIO_Pad_Odn(g_I2CBusCfg[chNum].u16PadSDA) : MDrv_GPIO_Pad_Oen(g_I2CBusCfg[chNum].u16PadSDA))

#define SWIIC_SCL_OUT(chNum, x) \
    ((x == _HIGH) ? MDrv_GPIO_Pull_High(g_I2CBusCfg[chNum].u16PadSCL) : MDrv_GPIO_Pull_Low(g_I2CBusCfg[chNum].u16PadSCL))

#define SWIIC_SDA_OUT(chNum, x) \
    ((x == _HIGH) ? MDrv_GPIO_Pull_High(g_I2CBusCfg[chNum].u16PadSDA) : MDrv_GPIO_Pull_Low(g_I2CBusCfg[chNum].u16PadSDA))

#define GET_SWIIC_SCL(chNum) MDrv_GPIO_Pad_Read(g_I2CBusCfg[chNum].u16PadSCL)

#define GET_SWIIC_SDA(chNum) MDrv_GPIO_Pad_Read(g_I2CBusCfg[chNum].u16PadSDA)

//#define SWII_DELAY(chNum)    (_I2CBus[chNum].DefDelay)

#define _SDA_HIGH(chNum)     SWIIC_SDA_PIN(chNum, _INPUT)
#define _SDA_LOW(chNum)     do { SWIIC_SDA_OUT(chNum, _LOW); SWIIC_SDA_PIN(chNum, _OUTPUT); } while(0)

#define _SCL_HIGH(chNum)     SWIIC_SCL_PIN(chNum, _INPUT)
#define _SCL_LOW(chNum)     do { SWIIC_SCL_OUT(chNum, _LOW); SWIIC_SCL_PIN(chNum, _OUTPUT); } while(0)


//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------


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
void MDrv_SW_IIC_DelayEx(U8 u8ChIIC,U8 u8Divisor)
{
    /*
    U32 volatile u32Loop=DELAY_CYCLES(g_I2CBusCfg[u8ChIIC].u16SpeedKHz)/u8Divisor;

    while(u32Loop--)
    {
        #ifdef __mips__
        __asm__ __volatile__ ("nop");
        #endif

        #ifdef __arm__
        __asm__ __volatile__ ("mov r0, r0");
        #endif
    }
    */
    U32 u32DlayUS=1000/(2*g_I2CBusCfg[u8ChIIC].u16SpeedKHz); //half period
    u32DlayUS /= u8Divisor;
    while(u32DlayUS--)
        udelay(1);
}

void MDrv_SW_IIC_Delay(U8 u8ChIIC)
{
    /*
    U32 volatile u32Loop=DELAY_CYCLES(g_I2CBusCfg[u8ChIIC].u16SpeedKHz);

    while(u32Loop--)
    {
        #ifdef __mips__
        __asm__ __volatile__ ("nop");
        #endif

        #ifdef __arm__
        __asm__ __volatile__ ("mov r0, r0");
        #endif
    }
    */
    U32 u32DlayUS=1000/(2*g_I2CBusCfg[u8ChIIC].u16SpeedKHz); //half period
    while(u32DlayUS--)
        udelay(1);
}

void MDrv_SW_IIC_SCL(U8 u8ChIIC, U8 u8Data)
{
    if ( u8Data == _HIGH )
    {
        SWIIC_SCL_PIN(u8ChIIC, _INPUT);  //set to input
    }
    else
    {
        SWIIC_SCL_OUT(u8ChIIC, _LOW);
        SWIIC_SCL_PIN(u8ChIIC, _OUTPUT);
    }
}

void MDrv_SW_IIC_SDA(U8 u8ChIIC, U8 u8Data)
{
    if ( u8Data == _HIGH )
    {
        SWIIC_SDA_PIN(u8ChIIC, _INPUT);  //set to input
    }
    else
    {
        SWIIC_SDA_OUT(u8ChIIC, _LOW);
        SWIIC_SDA_PIN(u8ChIIC, _OUTPUT); //set to output
    }
}

void MDrv_SW_IIC_SCL_Chk(U8 u8ChIIC, B16 bSet)
{
    u16 u16Dummy;       // loop dummy

    if (bSet == _HIGH)  // if set pin high
    {
        SWIIC_SCL_PIN(u8ChIIC, _INPUT);
        u16Dummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        //< 090903 leehc > IIC 통신 오류대응. - hjkoh_090909 udelay제거.
        while ((GET_SWIIC_SCL(u8ChIIC) == _LOW) && (u16Dummy--));
    }
    else
    {
        MDrv_SW_IIC_SCL(u8ChIIC, _LOW);    // set SCL pin
        SWIIC_SCL_PIN(u8ChIIC, _OUTPUT);
    }
}

void MDrv_SW_IIC_SDA_Chk(U8 u8ChIIC, B16 bSet)
{
    U16 u16Dummy;       // loop dummy

    if (bSet == _HIGH)  // if set pin high
    {
        SWIIC_SDA_PIN(u8ChIIC, _INPUT);
        u16Dummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((GET_SWIIC_SDA(u8ChIIC) == _LOW) && (u16Dummy--));// check SDA pull high// - hjkoh_090909 udelay제거.
    }
    else
    {
        MDrv_SW_IIC_SDA(u8ChIIC, _LOW);    // set SDA pin
        SWIIC_SDA_PIN(u8ChIIC, _OUTPUT);
    }
}

//-------------------------------------------------------------------------------------------------
// SW I2C: start signal.
// <comment>
//  SCL ________
//              \_________
//  SDA _____
//           \____________
//
// Return value: None
//-------------------------------------------------------------------------------------------------
B16 MDrv_SW_IIC_Start(U8 u8ChIIC)
{
    B16 bStatus = TRUE;    // success status

    MDrv_SW_IIC_SDA_Chk(u8ChIIC, _HIGH);
    MDrv_SW_IIC_Delay(u8ChIIC);

    MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);
    MDrv_SW_IIC_Delay(u8ChIIC);

    // check pin error
    SWIIC_SCL_PIN(u8ChIIC, _INPUT);
     SWIIC_SDA_PIN(u8ChIIC, _INPUT);


    if ((GET_SWIIC_SCL(u8ChIIC) == _LOW) || (GET_SWIIC_SDA(u8ChIIC) == _LOW))
    {
           SWIIC_SCL_PIN(u8ChIIC, _OUTPUT);
           SWIIC_SDA_PIN(u8ChIIC, _OUTPUT);
        bStatus = FALSE;
    }
    else // success
    {
        MDrv_SW_IIC_SDA(u8ChIIC, _LOW);
        MDrv_SW_IIC_Delay(u8ChIIC);
        MDrv_SW_IIC_SCL(u8ChIIC, _LOW);
    }

    return bStatus;     //vain
}

//-------------------------------------------------------------------------------------------------
// SW I2C: stop signal.
// <comment>
//              ____________
//  SCL _______/
//                 _________
//  SDA __________/
//-------------------------------------------------------------------------------------------------
void MDrv_SW_IIC_Stop(U8 u8ChIIC)
{
    _SCL_LOW(u8ChIIC);

    MDrv_SW_IIC_Delay(u8ChIIC);
    _SDA_LOW(u8ChIIC);

    MDrv_SW_IIC_Delay(u8ChIIC);
//    _SCL_HIGH(u8ChIIC);
	MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);	// <20091212 takerest> fix SCL pin error
    MDrv_SW_IIC_Delay(u8ChIIC);
//    _SDA_HIGH(u8ChIIC);
	MDrv_SW_IIC_SDA_Chk(u8ChIIC, _HIGH);	// <20091212 takerest> fix SDA pin error
    MDrv_SW_IIC_Delay(u8ChIIC);
}

//-------------------------------------------------------------------------------------------------
///SW I2C: Send 1 bytes data
///@param u8data \b IN: 1 byte data to send
//-------------------------------------------------------------------------------------------------
static B16 _IIC_SendByte(U8 u8ChIIC, U8 u8data, U8 u8Delay4Ack )   // Be used int IIC_SendByte
{
    U8      u8Mask = 0x80;
    B16     bAck; // acknowledge bit

    while ( u8Mask )
    {
        if (u8data & u8Mask)
        {
            MDrv_SW_IIC_SDA_Chk(u8ChIIC, _HIGH);
        }
        else
        {
            MDrv_SW_IIC_SDA_Chk(u8ChIIC, _LOW);
        }

        MDrv_SW_IIC_Delay(u8ChIIC);
        #if 0//(EXTRA_DELAY_CYCLE)
    	if(SWII_DELAY(u8ChIIC) == 2)
    	{
    		MDrv_SW_IIC_DelayEx(u8ChIIC, 8);
    	}
        #endif
        MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH); // clock
        MDrv_SW_IIC_Delay(u8ChIIC);
        MDrv_SW_IIC_SCL(u8ChIIC, _LOW);
        u8Mask >>= 1; // next
    }

    // recieve acknowledge
    SWIIC_SDA_PIN(u8ChIIC, _INPUT);
    if( u8Delay4Ack > 0 )
    {
        udelay( u8Delay4Ack );
    }
    else
    {
        MDrv_SW_IIC_Delay(u8ChIIC);
    }
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);

    bAck = GET_SWIIC_SDA(u8ChIIC); // recieve acknowlege
//    SWIIC_SDA(u8ChIIC, bAck);     //for I2c waveform sharp
//    SWIIC_SDA_PIN(u8ChIIC, _OUTPUT);
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_SCL(u8ChIIC, _LOW);

    MDrv_SW_IIC_Delay(u8ChIIC);

    MDrv_SW_IIC_SDA(u8ChIIC, bAck);     //for I2c waveform sharp
    SWIIC_SDA_PIN(u8ChIIC, _OUTPUT);

    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);

    return (bAck);
}

//-------------------------------------------------------------------------------------------------
///SW I2C: Send 1 bytes data, this function will retry 5 times until success.
///@param u8data \b IN: 1 byte data to send
///@return BOOLEAN:
///- TRUE: Success
///- FALSE: Fail
//-------------------------------------------------------------------------------------------------
B16 MDrv_SW_IIC_SendByte(U8 u8ChIIC, U8 u8data, U8 u8Delay4Ack )
{
    U8 u8I;

    for(u8I=0;u8I<I2C_ACCESS_DUMMY_TIME;u8I++)
    {
        if (_IIC_SendByte(u8ChIIC, u8data, u8Delay4Ack) == I2C_ACKNOWLEDGE)
            return TRUE;
    }

    //printk("IIC write byte 0x%02x fail!!\n", u8data);
    return FALSE;
}

//-------------------------------------------------------------------------------------------------
// SW I2C: access start.
//
// Arguments: u8SlaveAdr - slave address
//            u8Trans - I2C_TRANS_WRITE/I2C_TRANS_READ
//-------------------------------------------------------------------------------------------------
B16 MDrv_SW_IIC_AccessStart(U8 u8ChIIC, U8 u8SlaveAdr, U8 u8Trans)
{
    U8 u8Dummy; // loop dummy
    U8 u8Delay4Ack = 0;

    if (u8Trans == SWIIC_READ) // check i2c read or write
    {
        u8SlaveAdr = u8SlaveAdr | 0x01; // read
    }
    else
    {
        u8SlaveAdr = u8SlaveAdr & 0xfe; // write
    }

    u8Dummy = I2C_ACCESS_DUMMY_TIME;

    while (u8Dummy--)
    {
        if ( MDrv_SW_IIC_Start(u8ChIIC) == FALSE)
        {
            //printk("MDrv_SW_IIC_Start=>Failed\n");
            continue;
        }

        if ( MDrv_SW_IIC_SendByte(u8ChIIC, u8SlaveAdr, u8Delay4Ack) == TRUE )   //I2C_ACKNOWLEDGE) // check acknowledge
        {
            return TRUE;
        }

        MDrv_SW_IIC_Stop(u8ChIIC);
    }

    //printk("MDrv_SW_IIC_Start(%c %02x %02x)=>Failed\n", (u8Trans == SWIIC_READ) ? 'r':'w', u8ChIIC, u8SlaveAdr );
    return FALSE;
}

//-------------------------------------------------------------------------------------------------
///SW I2C: Get 1 bytes data
///@param u16Ack  \b IN: acknowledge
///@return U8:    \b OUT: get data from the device
///- TRUE: Success
///- FALSE: Fail
//-------------------------------------------------------------------------------------------------
U8 MDrv_SW_IIC_GetByte (U8 u8ChIIC, U16 u16Ack)
{
    U8 u8Receive = 0;
    U8 u8Mask = 0x80;

    SWIIC_SDA_PIN(u8ChIIC, _INPUT);

    while ( u8Mask )
    {
        MDrv_SW_IIC_Delay(u8ChIIC);
        MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);
        MDrv_SW_IIC_Delay(u8ChIIC);

        if (GET_SWIIC_SDA(u8ChIIC) == _HIGH)
        {
            u8Receive |= u8Mask;
        }
        u8Mask >>= 1; // next

        MDrv_SW_IIC_SCL(u8ChIIC, _LOW);

        #if 0//(EXTRA_DELAY_CYCLE)
	    if(SWII_DELAY(u8ChIIC) == 2)
	    {
            MDrv_SW_IIC_Delay(u8ChIIC);
            MDrv_SW_IIC_Delay(u8ChIIC);
	        MDrv_SW_IIC_DelayEx(u8ChIIC, 8);
	    }
        #endif
    }
    if (u16Ack)
    {
        // acknowledge
        MDrv_SW_IIC_SDA_Chk(u8ChIIC, I2C_ACKNOWLEDGE);
    }
    else
    {
        // non-acknowledge
        MDrv_SW_IIC_SDA_Chk(u8ChIIC, I2C_NON_ACKNOWLEDGE);
    }
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_SCL_Chk(u8ChIIC, _HIGH);
    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_SCL(u8ChIIC, _LOW);
    MDrv_SW_IIC_Delay(u8ChIIC);


    MDrv_SW_IIC_Delay(u8ChIIC);
    MDrv_SW_IIC_Delay(u8ChIIC);

    return u8Receive;
}
//-------------------------------------------------------------------------------------------------
// Get SDA state without check
// return:
//   1: high
//   0: low
//-------------------------------------------------------------------------------------------------
int MDrv_SW_IIC_GetSDA(U8 u8ChIIC)
{
    int ret=0;
    #if SWI2C_LOCK
    MDrv_SW_IIC_Lock();
    #endif
    SWIIC_SDA_PIN(u8ChIIC, _INPUT);
    ret=GET_SWIIC_SDA(u8ChIIC);
    #if SWI2C_LOCK
    MDrv_SW_IIC_UnLock();
    #endif
    return ret;
}
EXPORT_SYMBOL(MDrv_SW_IIC_GetSDA);

//-------------------------------------------------------------------------------------------------
// Get SCL state without check
// return:
//   1: high
//   0: low
//-------------------------------------------------------------------------------------------------
int MDrv_SW_IIC_GetSCL(U8 u8ChIIC)
{
    int ret=0;
    #if SWI2C_LOCK
    MDrv_SW_IIC_Lock();
    #endif
    SWIIC_SCL_PIN(u8ChIIC, _INPUT);
    ret=GET_SWIIC_SCL(u8ChIIC);
    #if SWI2C_LOCK
    MDrv_SW_IIC_UnLock();
    #endif
    return ret;
}
EXPORT_SYMBOL(MDrv_SW_IIC_GetSCL);
//-------------------------------------------------------------------------------------------------
///SW I2C: Write bytes, be able to write 1 byte or several bytes to several register offsets in same slave address.
///@param u8SlaveID \b IN: Slave ID (Address)
///@param u8AddrCnt \b IN:  register NO to write, this parameter is the NO of register offsets in pu8addr buffer,
///it should be 0 when *pu8Addr = NULL.
///@param *pu8Addr \b IN: pointer to a buffer containing target register offsets to write
///@param u32BufLen \b IN: Data length (in byte) to write
///@param *pu8Buf \b IN: pointer to the data buffer for write
///@return BOOLEAN:
///- TRUE: Success
///- FALSE: Fail
//-------------------------------------------------------------------------------------------------
S32 MDrv_SW_IIC_Write(U8 u8ChIIC, U8 u8SlaveID, U8 u8AddrCnt, U8* pu8Addr, U32 u32BufLen, U8* pu8Buf)
{
    U8  u8Dummy; // loop dummy
    S32 s32RetCountIIC;

    //check if sw i2c channel is disabled
    if(g_I2CBusCfg[u8ChIIC].u8Enable == DISABLE)
        return -1;

    u8Dummy = I2C_ACCESS_DUMMY_TIME;
    s32RetCountIIC = u32BufLen;
    #if SWI2C_LOCK
    MDrv_SW_IIC_Lock();
    #endif
    while (u8Dummy--)
    {
        if (MDrv_SW_IIC_AccessStart(u8ChIIC, u8SlaveID, SWIIC_WRITE) == FALSE)
        {
            s32RetCountIIC = -2;
            goto SW_IIC_Write_End;
        }

        while( u8AddrCnt )
        {
              u8AddrCnt--;
              if ( MDrv_SW_IIC_SendByte(u8ChIIC, *pu8Addr, 0) == FALSE )
              {
                s32RetCountIIC = -3;
                goto SW_IIC_Write_End;
              }
              pu8Addr++;
        }
        while (u32BufLen) // loop of writting data
        {
            u32BufLen-- ;
            //IIC_SendByte(*pu8Buf); // send byte
            if ( MDrv_SW_IIC_SendByte(u8ChIIC, *pu8Buf, 0) == FALSE )
            {
                s32RetCountIIC = -4;
                goto SW_IIC_Write_End;
            }
            pu8Buf++; // next byte pointer
        }

        break;
    }

SW_IIC_Write_End:
    MDrv_SW_IIC_Stop(u8ChIIC);

    #if SWI2C_LOCK
    MDrv_SW_IIC_UnLock();
    #endif
    return s32RetCountIIC;
}
EXPORT_SYMBOL(MDrv_SW_IIC_Write);

//-------------------------------------------------------------------------------------------------
///SW I2C: Read bytes, be able to read 1 byte or several bytes from several register offsets in same slave address.
///@param u8SlaveID \b IN: Slave ID (Address)
///@param u8AddrCnt \b IN:  register NO to read, this parameter is the NO of register offsets in pu8addr buffer,
///it should be 0 when *paddr = NULL.
///@param *pu8Addr \b IN: pointer to a buffer containing target register offsets to read
///@param u32BufLen \b IN: Data length (in byte) to read
///@param *pu8Buf \b IN: pointer to retun data buffer.
///@return BOOLEAN:
///- TRUE: Success
///- FALSE: Fail
//-------------------------------------------------------------------------------------------------
S32 MDrv_SW_IIC_Read(U8 u8ChIIC, U8 u8SlaveID, U8 u8AddrCnt, U8* pu8Addr, U32 u32BufLen, U8* pu8Buf)
{
    U8  u8Dummy; // loop dummy
    S32 s32RetCountIIC;

    //check if sw i2c channel is disabled
    if(g_I2CBusCfg[u8ChIIC].u8Enable == DISABLE)
        return 0;

    u8Dummy = I2C_ACCESS_DUMMY_TIME;
    //s32RetCountIIC = u32BufLen;
    s32RetCountIIC=0;
    #if SWI2C_LOCK
    MDrv_SW_IIC_Lock();
    #endif

    while (u8Dummy--)
    {
        if( u8AddrCnt > 0 )
        {
            if (MDrv_SW_IIC_AccessStart(u8ChIIC, u8SlaveID, SWIIC_WRITE) == FALSE)
            {
                //s32RetCountIIC = -2;
                goto SW_IIC_Read_End;
            }

            while( u8AddrCnt )
            {
                u8AddrCnt--;
                if (MDrv_SW_IIC_SendByte(u8ChIIC, *pu8Addr, 0) == FALSE)
                {
                    //s32RetCountIIC = -3;
                    goto SW_IIC_Read_End;
                }
                pu8Addr++;
            }
        }

        if (MDrv_SW_IIC_AccessStart(u8ChIIC, u8SlaveID, SWIIC_READ) == FALSE)
        {
            //s32RetCountIIC = -4;
            goto SW_IIC_Read_End;
        }

        while (u32BufLen--) // loop to burst read
        {
            *pu8Buf = MDrv_SW_IIC_GetByte(u8ChIIC, u32BufLen); // receive byte

            pu8Buf++; // next byte pointer
            s32RetCountIIC++;
        }

        break;
    }

SW_IIC_Read_End:
    MDrv_SW_IIC_Stop(u8ChIIC);
    #if SWI2C_LOCK
    MDrv_SW_IIC_UnLock();
    #endif
    return s32RetCountIIC;
}
EXPORT_SYMBOL(MDrv_SW_IIC_Read);

//-------------------------------------------------------------------------------------------------
/// IIC Set Speed by Index
/// @param  u8ChIIC            \b IN:  channel index
/// @param  u8Speed            \b IN:  u8Speed index
/// @return None
//-------------------------------------------------------------------------------------------------
void MDrv_SW_IIC_SetSpeed(U8 u8ChIIC, U8 u8Speed)
{
    if( (u8ChIIC <= IIC_NUM_OF_HW) || (u8ChIIC >= IIC_NUM_OF_MAX) )
    {
        return;
        //SWII_DELAY(u8ChIIC) = u8Speed;
    }

    //switch(SWII_DELAY(u8ChIIC))
    switch(u8Speed)
    {
        case 1:
            g_I2CBusCfg[u8ChIIC].u16SpeedKHz = 400; //KHz
            break;
        case 2:
            g_I2CBusCfg[u8ChIIC].u16SpeedKHz = 300; //KHz
            break;
        case 3:
            g_I2CBusCfg[u8ChIIC].u16SpeedKHz = 200; //KHz
            break;
        case 4:
            g_I2CBusCfg[u8ChIIC].u16SpeedKHz = 100; //KHz
            break;
        default:
            g_I2CBusCfg[u8ChIIC].u16SpeedKHz = 100; //KHz
            break;
    }

}
EXPORT_SYMBOL(MDrv_SW_IIC_SetSpeed);

//-------------------------------------------------------------------------------------------------
/// IIC Bus Enable or Disable
/// @param  u8ChIIC            \b IN:  channel index
/// @param  bEnable            \b IN:  enable
/// @return None
//-------------------------------------------------------------------------------------------------
void MDrv_SW_IIC_Enable( U8 u8ChIIC, B16 bEnable )
{
    g_I2CBusCfg[u8ChIIC].u8Enable = bEnable;
}
EXPORT_SYMBOL(MDrv_SW_IIC_Enable);

//-------------------------------------------------------------------------------------------------
/// IIC Bus Configuration
/// @param pBusCfg            \b IN:  clock selection
/// @return None
//-------------------------------------------------------------------------------------------------
B16 MDrv_SW_IIC_ConfigBus(I2C_BusCfg_t* pBusCfg)
{
    U8 u8ChIIC;

    if((pBusCfg->u8ChIdx <= IIC_NUM_OF_HW) || (pBusCfg->u8ChIdx >= IIC_NUM_OF_MAX))
    {
        printk("\n[Note]: Bus channel avaliable index is from %d ~ %d\n",(IIC_NUM_OF_HW+1),(IIC_NUM_OF_MAX-1));
        return FALSE;
    }

    printk("[%s] Channel index = %d\n",__FUNCTION__,pBusCfg->u8ChIdx);
    printk("[%s] Channel PadSCL = %d\n",__FUNCTION__,pBusCfg->u16PadSCL);
    printk("[%s] Channel PadSDA = %d\n",__FUNCTION__,pBusCfg->u16PadSDA);
    printk("[%s] Channel SpeedKHz = %d\n",__FUNCTION__,pBusCfg->u16SpeedKHz);
    printk("[%s] Channel Enable = %d\n",__FUNCTION__,pBusCfg->u8Enable);
    u8ChIIC = pBusCfg->u8ChIdx;
    g_I2CBusCfg[u8ChIIC].u8ChIdx = pBusCfg->u8ChIdx;
    g_I2CBusCfg[u8ChIIC].u16PadSCL = pBusCfg->u16PadSCL;
    g_I2CBusCfg[u8ChIIC].u16PadSDA = pBusCfg->u16PadSDA;
    g_I2CBusCfg[u8ChIIC].u16SpeedKHz = pBusCfg->u16SpeedKHz;
    g_I2CBusCfg[u8ChIIC].u8Enable = pBusCfg->u8Enable;
    return TRUE;

}
EXPORT_SYMBOL(MDrv_SW_IIC_ConfigBus);

//-------------------------------------------------------------------------------------------------
/// IIC master initialization
/// @return None
/// @note   Hardware IIC. Called only once at system initialization
//-------------------------------------------------------------------------------------------------
void MDrv_IIC_Init(void)
{
    MHal_IIC_Init();

    #if SWI2C_LOCK
    MDrv_SW_IIC_InitLock();
    #endif

    #if 1
    {
        U8 u8ChIIC;

        #if 0
        u32CpuSpeedMHz = MHal_Query_MIPS_CLK()/1000000UL;
        #else
        u32CpuSpeedMHz = 720;
        #endif
        u32AdjParam = FACTOR_ADJ/u32CpuSpeedMHz;
        printk("[%s] u32CpuSpeedMHz= %d MHz\n",__FUNCTION__,u32CpuSpeedMHz);
        printk("[%s] u32AdjParam= %d MHz\n",__FUNCTION__,u32AdjParam);
        for(u8ChIIC=0;u8ChIIC<IIC_NUM_OF_MAX;u8ChIIC++)
        {
            g_I2CBusCfg[u8ChIIC].u8ChIdx = 0xFF;
            g_I2CBusCfg[u8ChIIC].u16SpeedKHz = 100; //KHz
            g_I2CBusCfg[u8ChIIC].u8Enable = DISABLE;
        }
    }
    #endif
}
EXPORT_SYMBOL(MDrv_IIC_Init);

//-------------------------------------------------------------------------------------------------
/// IIC clock selection
/// @param  u8ClockIIC            \b IN:  clock selection
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_HW_IIC_Clock_Select(U8 u8ClockIIC)
{
    MHal_IIC_Clock_Select(u8ClockIIC);
}
EXPORT_SYMBOL(MDrv_HW_IIC_Clock_Select);

//-------------------------------------------------------------------------------------------------
/// Write data to an IIC device
/// @param  u8SlaveIdIIC            \b IN:  device slave ID
/// @param  u8AddrSizeIIC           \b IN:  address length in bytes
/// @param  pu8AddrIIC              \b IN:  pointer to the start address of the device
/// @param  u32BufSizeIIC           \b IN:  number of bytes to be written
/// @param  pu8BufIIC               \b IN:  pointer to write data buffer
/// @return TRUE(succeed), FALSE(fail)
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
S32 MDrv_HW_IIC_Write(U8 u8SlaveIdIIC, U8 u8AddrSizeIIC, U8 *pu8AddrIIC, U32 u32BufSizeIIC, U8 *pu8BufIIC)
{
    U32     u32I;
    U8      u8I;
    B16     bReturnIIC = TRUE;
    S32     s32RetCountIIC = -1;

    MHal_IIC_Start();
	IIC_DBG(printk(">>  MHal_IIC_Start  %s %d  \n ",__FILE__,__LINE__));

    if (FALSE == MHal_IIC_SendByte(u8SlaveIdIIC&0xFE))
    {
        bReturnIIC = FALSE;
        s32RetCountIIC = -2;
        goto HW_IIC_Write_End;
    }

    for (u8I = 0; u8I < u8AddrSizeIIC; u8I++)
    {
        if (FALSE == MHal_IIC_SendByte(pu8AddrIIC[u8I]))
        {
            bReturnIIC = FALSE;
            s32RetCountIIC = -3;
            goto HW_IIC_Write_End;
        }
    }

    for (u32I = 0; u32I < u32BufSizeIIC; u32I++)
    {
        if (FALSE == MHal_IIC_SendByte(pu8BufIIC[u32I]))
        {
            bReturnIIC = FALSE;
            s32RetCountIIC = -4;
            goto HW_IIC_Write_End;
        }
    }

    s32RetCountIIC = u32BufSizeIIC;

HW_IIC_Write_End:
    MHal_IIC_Stop();
    udelay(1000);
    udelay(1000);
    udelay(1000);

    IIC_DBG(printk("MDrv_IIC_Write() --> s32RetCountIIC=%d \n", s32RetCountIIC));
    return s32RetCountIIC;
}
EXPORT_SYMBOL(MDrv_HW_IIC_Write);

//-------------------------------------------------------------------------------------------------
/// Read data from an IIC device
/// @param  u8SlaveIdIIC            \b IN:  device slave ID
/// @param  u8AddrSizeIIC           \b IN:  address length in bytes
/// @param  pu8AddrIIC              \b IN:  ptr to the start address inside the device
/// @param  u32BufSizeIIC           \b IN:  number of bytes to be read
/// @param  pu8BufIIC               \b OUT: pointer to read data buffer
/// @return TRUE : succeed
/// @return FALSE : fail
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
S32 MDrv_HW_IIC_Read(U8 u8SlaveIdIIC, U8 u8AddrSizeIIC, U8 *pu8AddrIIC, U32 u32BufSizeIIC, U8 *pu8BufIIC)
{
    U32     u32I;
    U8      u8I;
    B16     bReturnIIC = TRUE;
    S32     s32RetCountIIC = -1;

    MHal_IIC_Start();

    if (FALSE == MHal_IIC_SendByte(u8SlaveIdIIC&0xFE))
    {
        bReturnIIC = FALSE;
        s32RetCountIIC = -2;
        goto HW_IIC_Read_End;
    }

    for (u8I = 0; u8I < u8AddrSizeIIC; u8I++)
    {
        if (FALSE == MHal_IIC_SendByte(pu8AddrIIC[u8I]))
        {
            bReturnIIC = FALSE;
            s32RetCountIIC = -3;
            goto HW_IIC_Read_End;
        }
    }

    MHal_IIC_Start();

    udelay(100);

    if (FALSE == MHal_IIC_SendByte(u8SlaveIdIIC|0x1))
    {
        bReturnIIC = FALSE;
        s32RetCountIIC = -4;
        goto HW_IIC_Read_End;
    }

    for (u32I = 0; u32I < u32BufSizeIIC; u32I++)
    {
        if (u32I == (u32BufSizeIIC-1))
        {
            MHal_IIC_NoAck();
        }

        if (FALSE == MHal_IIC_GetByte(&pu8BufIIC[u32I]))
        {
            bReturnIIC = FALSE;
            s32RetCountIIC = -5;
            goto HW_IIC_Read_End;
        }
    }

    s32RetCountIIC = u32BufSizeIIC;

HW_IIC_Read_End:
    MHal_IIC_Stop();
    udelay(1000);
    udelay(1000);
    udelay(1000);

    IIC_DBG(printk("MDrv_IIC_Read() --> s32RetCountIIC=%d \n", s32RetCountIIC));
    return s32RetCountIIC;
}
EXPORT_SYMBOL(MDrv_HW_IIC_Read);

//-------------------------------------------------------------------------------------------------
/// IIC initialization
/// @param  None
/// @return None
//-------------------------------------------------------------------------------------------------
void MDrv_HW_IIC_Init( void )
{
	IIC_DBG(printk(">>  MDrv_HW_IIC_Init  %s %d  \n ",__FILE__,__LINE__));

    MHal_IIC_Init();
}
EXPORT_SYMBOL(MDrv_HW_IIC_Init);
#endif

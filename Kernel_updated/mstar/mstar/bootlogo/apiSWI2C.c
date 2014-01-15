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
#define _API_SWI2C_C_

//#include "MsCommon.h"
//#include "MsVersion.h"
#include "apiSWI2C.h"
//#include "drvGPIO.h"
//#include "MsOS.h"
//#include "drvMMIO.h"
//#include "../tfsr/dtv_bml.h"

#if defined(PRJ_X5) || defined(PRJ_X9)
#include "MSD391STX.h"
#include "gpio_macro_x5.h"
#else
#include "MSD896SCXF.h"
#include "gpio_macro_x10.h"
#endif

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#ifndef BYTE
#define BYTE unsigned char
#endif
#ifndef BOOL
#define BOOL unsigned char
#endif

#define _HIGH                       1
#define _LOW                        0

#define COUNTOF( array )            (sizeof(array) / sizeof((array)[0]))


#define REG_OEN(pad)                CONCAT(GPIO_NUM(pad),_OEN)
#define REG_OUT(pad)                CONCAT(GPIO_NUM(pad),_OUT)
#define REG_IN(pad)                 CONCAT(GPIO_NUM(pad),_IN)
#define _I2C_PAD(pad)               REG_OEN(pad), REG_OUT(pad), REG_IN(pad)
#define _I2C_BUS(scl, sda, dly)     _I2C_PAD(scl), _I2C_PAD(sda), dly
#define I2C_BUS(bus)                _I2C_BUS(bus)

#if defined(PRJ_X5)
#define IIC_BUS_MAX                 4

#define MIIC_BUS_0      PAD_DDCR_CK,    PAD_DDCR_DA,    300    //Bus_0 : DDCR_CK(SCL), DDCR_DA(SDA)		EEPROM
#define MIIC_BUS_1      PAD_TCON1,      PAD_TCON3,      300   //Bus_1 : GPIO144(SCL), GPIO146(SDA)		  TUNER
#define MIIC_BUS_2      PAD_GPIO20,     PAD_GPIO21,     300   //Bus_2 : GPIO56(SCL), GPIO57(SDA)		  TCON,LOGIC
#define MIIC_BUS_3      PAD_GPIO14,     PAD_GPIO15,     300    //Bus_3 : GPIO50(SCL), GPIO51(SDA)		  AMP
#else
#define IIC_BUS_MAX                 10

#define MIIC_BUS_0      PAD_DDCR_CK,    PAD_DDCR_DA,    100	//TCON,LOGIC(*)
#define MIIC_BUS_1      PAD_TGPIO2,      PAD_TGPIO3,      100	//TUNER(*)
#define MIIC_BUS_2      PAD_GPIO_PM14,     PAD_GPIO_PM15,     100	//EEPROM
#define MIIC_BUS_3      PAD_GPIO10,     PAD_GPIO9,     100	//AMP
#define MIIC_BUS_4      34,   35,  100                //DDC PC But Not USE
#define MIIC_BUS_5      34,   35,  100                //DDC HDMI1
#define MIIC_BUS_6      36,   37,  100                //DDC HDMI2
#define MIIC_BUS_7      38,   39,  100                //DDC HDMI3
#define MIIC_BUS_8      40,   41,  100                //DDC HDMI4
#define MIIC_BUS_9      109,   110,  100                //FRC
#endif

typedef struct {
    MS_U32 SclOenReg; MS_U8  SclOenBit;   MS_U32 SclOutReg; MS_U8  SclOutBit;   MS_U32 SclInReg;MS_U8  SclInBit;
    MS_U32 SdaOenReg; MS_U8  SdaOenBit;   MS_U32 SdaOutReg; MS_U8  SdaOutBit;   MS_U32 SdaInReg;MS_U8  SdaInBit;
    MS_U16  defDelay;
} I2C_Bus;

I2C_Bus g_I2CBus[IIC_BUS_MAX] =
{
    { I2C_BUS( MIIC_BUS_0 ) },
    { I2C_BUS( MIIC_BUS_1 ) },
    { I2C_BUS( MIIC_BUS_2 ) },
    { I2C_BUS( MIIC_BUS_3 ) },
#if defined(PRJ_X10)
    { I2C_BUS( MIIC_BUS_4 ) },
    { I2C_BUS( MIIC_BUS_5 ) },
    { I2C_BUS( MIIC_BUS_6 ) },
    { I2C_BUS( MIIC_BUS_7 ) },
    { I2C_BUS( MIIC_BUS_8 ) },
    { I2C_BUS( MIIC_BUS_9 ) },
#endif
};

static MS_U32 _gMIO_MapBase_SWI2C = 0;


#define IIC_MUTEX_CREATE()          //MsOS_CreateMutex(E_MSOS_FIFO, gu8I2CMutex , MSOS_PROCESS_SHARED)//OS_CREATE_MUTEX("I2C_Mutex")
#define IIC_MUTEX_LOCK()            //OS_OBTAIN_MUTEX(g_s32I2CMutex,MSOS_WAIT_FOREVER)
#define IIC_MUTEX_UNLOCK()          //OS_RELEASE_MUTEX(g_s32I2CMutex)
#define IIC_MUTEX_DELETE()          //OS_DELETE_MUTEX(g_s32I2CMutex)

static MS_S32 g_s32I2CMutex = -1;
static MS_U8 u8BusAllocNum = 0;
static MS_U8 u8BusSel = 0;
static SWI2C_ReadMode g_SWI2CReadMode = E_SWI2C_READ_MODE_DIRECTION_CHANGE;
//static char gu8I2CMutex[] = {"OS_I2C_Mutex"};		//not used

#define SWI2C_READ      0
#define SWI2C_WRITE     1

#define I2C_CHECK_PIN_DUMMY     3200//100
#define I2C_ACKNOWLEDGE         _LOW
#define I2C_NON_ACKNOWLEDGE     _HIGH
#define I2C_ACCESS_DUMMY_TIME   3

#define HIBYTE(value)  ((BYTE)((value) / 0x100))
#define LOBYTE(value)  ((BYTE)(value))

#define SLAVE_ADDR_24C512   0xA0

#define  SWI2C_DEBUGINFO(x)  //x

MS_U32 g_SwIicDly = 100;//SWI2C_SPEED_MAPPING_100K;


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
//static MSIF_Version _api_swi2c_version = {
//    .DDI = { SWI2C_API_VERSION },
//};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

#define FACTOR_DLY    69000UL
#define FACTOR_ADJ    11040UL
static MS_U32 u32CpuSpeedMHz;
static MS_U32 u32AdjParam;
#define DELAY_CNT(SpeedKHz)  ((FACTOR_DLY/(SpeedKHz))-((130+u32AdjParam)-((SpeedKHz)/u32AdjParam))+((1<<((440-SpeedKHz)/40))))

#ifdef MSOS_TYPE_LINUX

static void iic_delay(void)
{
#ifdef __mips__
    __asm__ __volatile__ (
        ".set noreorder\n"
        "1:"
        "\tbgtz %0,1b\n"
        "\taddiu %0,%0,-1\n"
        : : "r" (DELAY_CNT(g_SwIicDly)));
#endif
}
#else
static void iic_delay(void)
{
    // volatile is necessary to avoid optimization
    MS_U32 volatile u32Dummy = 0;
    //MS_U32 u32Loop;
    MS_U32 volatile u32Loop;

    u32Loop = g_SwIicDly;
    while (u32Loop-- > 0)
    {
        u32Dummy++;
    }
}

#endif

static void IIC_UseBus( MS_U8 u8BusNum )
{
    if ( u8BusNum < COUNTOF( g_I2CBus ) )
    {
        u8BusSel = u8BusNum;
        g_SwIicDly = g_I2CBus[u8BusSel].defDelay;
        SWI2C_DEBUGINFO(printk("[IIC_UseBus]: u8BusSel = %d\n",u8BusSel));
        SWI2C_DEBUGINFO(printk("[IIC_UseBus]: g_SwIicDly = %ld\n",g_SwIicDly));
    }
}

MS_U32 MApi_SWI2C_Speed_Setting(MS_U8 u8BusNum, MS_U32 u32Speed_K)
{
    MS_U32 u32OriginalValue;

    u32OriginalValue = g_I2CBus[u8BusNum].defDelay;
    #ifdef MSOS_TYPE_LINUX
    g_I2CBus[u8BusNum].defDelay = u32Speed_K;
    #else
    if(u32Speed_K<=200)
        g_I2CBus[u8BusNum].defDelay = 80;
    else if(u32Speed_K<=400)
        g_I2CBus[u8BusNum].defDelay = 40;
    else if(u32Speed_K<=800)
        g_I2CBus[u8BusNum].defDelay = 20;
    else
        g_I2CBus[u8BusNum].defDelay = 10;
    #endif
    SWI2C_DEBUGINFO(printk("[MApi_SWI2C_Speed_Setting]: u8BusNum = %d, u32Speed_K = %ld\n",u8BusNum,u32Speed_K));
    SWI2C_DEBUGINFO(printk("[MApi_SWI2C_Speed_Setting]: g_SwIicDly = %ld\n",g_SwIicDly));

    IIC_UseBus(u8BusNum);
    return u32OriginalValue;
}

MS_BOOL MApi_SWI2C_SetReadMode(SWI2C_ReadMode eReadMode)
{
    if(eReadMode>=E_SWI2C_READ_MODE_MAX)
        return FALSE;
    g_SWI2CReadMode = eReadMode;
    return TRUE;
}

static void IIC_UnuseBus( void )
{
}


#define SclOenReg               g_I2CBus[u8BusSel].SclOenReg
#define SclOenBit               g_I2CBus[u8BusSel].SclOenBit
#define SclOutReg               g_I2CBus[u8BusSel].SclOutReg
#define SclOutBit               g_I2CBus[u8BusSel].SclOutBit
#define SclInReg                g_I2CBus[u8BusSel].SclInReg
#define SclInBit                g_I2CBus[u8BusSel].SclInBit
#define pin_scl_set_input()     MDrv_WriteRegBit(SclOenReg,1,SclOenBit)
#define pin_scl_set_high()      MDrv_WriteRegBit(SclOenReg,1,SclOenBit)
#define pin_scl_set_low()       MDrv_WriteRegBit(SclOenReg,0,SclOenBit); MDrv_WriteRegBit(SclOutReg,0,SclOutBit)
#define pin_scl_get_level()     ((MDrv_ReadRegBit(SclInReg,SclInBit)!=0)? _HIGH : _LOW)


static void pin_scl_check_high(void)
{
    MS_U16 u16Dummy;

    pin_scl_set_high();
    u16Dummy = I2C_CHECK_PIN_DUMMY;
    while ((pin_scl_get_level() == _LOW) && (u16Dummy--));
}


#define SdaOenReg               g_I2CBus[u8BusSel].SdaOenReg
#define SdaOenBit               g_I2CBus[u8BusSel].SdaOenBit
#define SdaOutReg               g_I2CBus[u8BusSel].SdaOutReg
#define SdaOutBit               g_I2CBus[u8BusSel].SdaOutBit
#define SdaInReg                g_I2CBus[u8BusSel].SdaInReg
#define SdaInBit                g_I2CBus[u8BusSel].SdaInBit
#define pin_sda_set_input()     MDrv_WriteRegBit(SdaOenReg,1,SdaOenBit)
#define pin_sda_set_high()      MDrv_WriteRegBit(SdaOenReg,1,SdaOenBit)
#define pin_sda_set_low()       MDrv_WriteRegBit(SdaOenReg,0,SdaOenBit); MDrv_WriteRegBit(SdaOutReg,0,SdaOutBit)
#define pin_sda_get_level()     ((MDrv_ReadRegBit(SdaInReg,SdaInBit)!=0)? _HIGH : _LOW)

static void pin_sda_check_high(void)
{
    MS_U16 u16Dummy;

    pin_sda_set_high();
    u16Dummy = I2C_CHECK_PIN_DUMMY;
    while ((pin_sda_get_level() == _LOW) && (u16Dummy--));
}


/******************************************************************************/
///I2C Initialize: Query CPU clock for delay parameters
/******************************************************************************/
unsigned int Query_MIPS_CLK(void)
{
   // static MS_U32 s_u32BaseAddrCHip, s_u32BaseSize;
    unsigned int u32Count = 0;
    //unsigned int uSpeed_L = 0, uSpeed_H = 0;

    //#define READ_BYTE(_reg)         (*(volatile MS_U8*)(_reg))
    //#define R1BYTE(u32Addr) (READ_BYTE (0xBF200000 + (u32Addr << 1) - (u32Addr & 1)))

    //MDrv_MMIO_GetBASE(&s_u32BaseAddrCHip, &s_u32BaseSize, MS_MODULE_CHIPTOP); //Jway
    //uSpeed_L = (unsigned int)R1BYTE(0x010c32);
    //uSpeed_H = (unsigned int)R1BYTE(0x010c34);
    //printk("[utopia]Query_MIPS_CLK: uSpeed_L = %X\n",uSpeed_L);
    //printk("[utopia]Query_MIPS_CLK: uSpeed_H = %X\n",uSpeed_H);
    #if defined(PRJ_X5) || defined(PRJ_X9)
    u32Count=507000000;
    #else
    u32Count=900000000;
    #endif
    #if 0
    if(uSpeed_H==0x1c)
    {
        //printf("(CPU Speed is 12 MHz)\n");
        u32Count = 12000000;
    }
    else if(uSpeed_L==0x14)
    {
        //printf("(CPU Speed %d MHz)\n", (int)(uSpeed_H*24));
        u32Count = uSpeed_H*24*1000000;
    }
    else if(uSpeed_L==0x18)
    {
        //printf("(CPU Speed %d MHz setting)\n", (int)(uSpeed_H*48));
        u32Count = uSpeed_H*48*1000000;
    }
    else//default 12 MHz
    {
        u32Count = 12000000;
    }
	return u32Count;
	#endif
    return u32Count;
}

/******************************************************************************/
///I2C Initialize: set I2C Clock and enable I2C
/******************************************************************************/

void MApi_SWI2C_Init(SWI2C_BusCfg SWI2CCBusCfg[],MS_U8 u8CfgBusNum)
{

    u8BusAllocNum = IIC_BUS_MAX;

    _gMIO_MapBase_SWI2C = 0xBF000000; //MS_MODULE_PM
    SWI2C_DEBUGINFO(printk("[utopia] Get IOMap success in SWI2C = %lX\n",_gMIO_MapBase_SWI2C));

    //default set read mode: DIRECTION_CHANGE(normal operation case)
    MApi_SWI2C_SetReadMode(E_SWI2C_READ_MODE_DIRECTION_CHANGE);
    // create mutex to protect read/write behavior
    if(g_s32I2CMutex==-1)
    {
        //g_s32I2CMutex = IIC_MUTEX_CREATE();
    }

    //Get CPU clock & delay parameters
    u32CpuSpeedMHz = (MS_U32)(Query_MIPS_CLK()/1000000UL);
    if(u32CpuSpeedMHz>=500)
        u32AdjParam = (MS_U32)(FACTOR_ADJ/u32CpuSpeedMHz); //~22
   else
        u32AdjParam = (MS_U32) 14;
    SWI2C_DEBUGINFO(printk("@@@@@@ u32AdjParam= %d MHz\n",(int)u32AdjParam));
    SWI2C_DEBUGINFO(printk("@@@@@@ u32CpuSpeedMHz= %d MHz\n",(int)u32CpuSpeedMHz));

 }

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// I2C start signal.
// <comment>
//  SCL ________
//              \_________
//  SDA _____
//           \____________
//
// Return value: None
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
static MS_BOOL IIC_Start(void)
{
    MS_BOOL bStatus = TRUE;    // success status

    pin_sda_check_high();
    iic_delay();

    pin_scl_check_high();
    iic_delay();

    // check pin error
    pin_scl_set_input();
    pin_sda_set_input();

    if ((pin_scl_get_level() == _LOW) || (pin_sda_get_level() == _LOW))
    {
        pin_scl_set_high();
        pin_sda_set_high();
        bStatus = FALSE;
    }
    else // success
    {
        pin_sda_set_low();
        iic_delay();
        pin_scl_set_low();
    }

    return bStatus;     //vain
}

////////////////////////////////////////////////////////////////////////////////
// I2C stop signal.
// <comment>
//              ____________
//  SCL _______/
//                 _________
//  SDA __________/
////////////////////////////////////////////////////////////////////////////////
static void IIC_Stop(void)
{
    pin_scl_set_low();
    iic_delay();
    pin_sda_set_low();

    iic_delay();
    pin_scl_set_input();
    iic_delay();
    pin_sda_set_input();
    iic_delay();
}

/******************************************************************************/
///Send 1 bytes data
///@param u8dat \b IN: 1 byte data to send
/******************************************************************************/
static MS_BOOL SendByte(MS_U8 u8dat)   // Be used int IIC_SendByte
{
    MS_U8    u8Mask = 0x80;
    int bAck; // acknowledge bit

    while ( u8Mask )
    {
        if (u8dat & u8Mask)
        {
            pin_sda_check_high();
        }
        else
        {
            pin_sda_set_low();
        }

        iic_delay();
        pin_scl_check_high();
        iic_delay();
        pin_scl_set_low();

        u8Mask >>= 1; // next
    }

    // recieve acknowledge
    pin_sda_set_input();
    iic_delay();
    pin_scl_check_high();

    iic_delay();
    bAck = pin_sda_get_level(); // recieve acknowlege
    pin_scl_set_low();

    iic_delay();

    //for I2c waveform sharp
    if (bAck)
        pin_sda_set_high();
    else
        pin_sda_set_low();

    pin_sda_set_input();

    iic_delay();
    iic_delay();
    iic_delay();
    iic_delay();

    return (bAck)? TRUE: FALSE;
}

/******************************************************************************/
///Send 1 bytes data, this function will retry 5 times until success.
///@param u8dat \b IN: 1 byte data to send
///@return MS_BOOL:
///- TRUE: Success
///- FALSE: Fail
/******************************************************************************/
static MS_BOOL IIC_SendByte(MS_U8 u8dat)
{
    MS_U8 i;

    for(i=0;i<5;i++)
    {
        if (SendByte(u8dat)==I2C_ACKNOWLEDGE)
            return TRUE;
    }

    SWI2C_DEBUGINFO(printk("IIC write byte 0x%x fail!!\n", u8dat));
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// I2C access start.
//
// Arguments: u8SlaveID - Slave ID (Address)
//            trans_t - I2C_TRANS_WRITE/I2C_TRANS_READ
////////////////////////////////////////////////////////////////////////////////
MS_BOOL IIC_AccessStart(MS_U8 u8SlaveID, MS_U8 trans_t)
{
    MS_U8 u8Dummy; // loop dummy

    if (trans_t == SWI2C_READ) // check i2c read or write
    {
        u8SlaveID |= BIT0;
    }
    else
    {
        u8SlaveID &= ~BIT0;
    }

    u8Dummy = I2C_ACCESS_DUMMY_TIME;

    while (u8Dummy--)
    {
        if ( IIC_Start() == FALSE)
        {
            continue;
        }

        if ( IIC_SendByte(u8SlaveID) == TRUE )  // check acknowledge
        {
            return TRUE;
        }

        IIC_Stop();
    }

    return FALSE;
}
/******************************************************************************/
///Get 1 bytes data, this function will retry 5 times until success.
///@param *u8dat \b IN: pointer to 1 byte data buffer for getting data
///@return MS_BOOL:
///- TRUE: Success
///- FALSE: Fail
/******************************************************************************///
//static MS_BOOL IIC_GetByte(MS_U8* pu8data)    // Auto generate ACK
static BYTE IIC_GetByte (MS_U16  bAck)
{
    BYTE ucReceive = 0;
    BYTE ucMask = 0x80;

    pin_sda_set_input();

    while ( ucMask )
    {
        iic_delay();
        pin_scl_check_high();
        iic_delay();

        if (pin_sda_get_level() == _HIGH)
        {
            ucReceive |= ucMask;
        }
        ucMask >>= 1; // next

        pin_scl_set_low();
    }
    if (bAck)
    {
        pin_sda_set_low();     // acknowledge
    }
    else
    {
        pin_sda_check_high();  // non-acknowledge
    }
    iic_delay();
    pin_scl_check_high();
    iic_delay();
    pin_scl_set_low();
    iic_delay();
    iic_delay();
    iic_delay();
    return ucReceive;
}

/******************************************************************************/
///Write bytes, be able to write 1 byte or several bytes to several register offsets in same slave address.
///@param u16BusNumSlaveID \b IN: Bus Number (high byte) and Slave ID (Address) (low byte)
///@param u8addrcount \b IN:  register NO to write, this parameter is the NO of register offsets in pu8addr buffer,
///it should be 0 when *pu8addr = NULL.
///@param *pu8addr \b IN: pointer to a buffer containing target register offsets to write
///@param u16size \b IN: Data length (in byte) to write
///@param *pu8data \b IN: pointer to the data buffer for write
///@return MS_BOOL:
///- TRUE: Success
///- FALSE: Fail
/******************************************************************************/
MS_BOOL MApi_SWI2C_WriteBytes(MS_U16 u16BusNumSlaveID, MS_U8 AddrCnt, MS_U8* pu8addr, MS_U32 u32size, MS_U8* pBuf)
{
    MS_U8 u8Dummy; // loop dummy
    MS_BOOL bRet;

    if(u8BusAllocNum==0)
        return FALSE;

    u8Dummy = 1;  //I2C_ACCESS_DUMMY_TIME;
    bRet = FALSE;

    IIC_MUTEX_LOCK();
    IIC_UseBus(HIBYTE(u16BusNumSlaveID));

    while (u8Dummy--)
    {
        if (IIC_AccessStart(LOBYTE(u16BusNumSlaveID), SWI2C_WRITE) == FALSE)
        {
            if( u8Dummy )
                continue;
            else
                goto fail;
        }

        while (AddrCnt)
        {
            AddrCnt--;
            if (IIC_SendByte(*pu8addr) == FALSE)
            {
                goto fail;
            }
            pu8addr++;
        }
        while (u32size) // loop of writting data
        {
            u32size-- ;
            if (IIC_SendByte(*pBuf) == FALSE)
            {
                goto fail;
            }
            pBuf++; // next byte pointer
        }

        break;
    }
    bRet = TRUE;

fail:

    IIC_Stop();
    IIC_UnuseBus();
    IIC_MUTEX_UNLOCK();

    return bRet;
}


 /******************************************************************************/
///Read bytes, be able to read 1 byte or several bytes from several register offsets in same slave address.
///@param u16BusNumSlaveID \b IN: Bus Number (high byte) and Slave ID (Address) (low byte)
///@param u8AddrNum \b IN:  register NO to read, this parameter is the NO of register offsets in pu8addr buffer,
///it should be 0 when *paddr = NULL.
///@param *paddr \b IN: pointer to a buffer containing target register offsets to read
///@param u16size \b IN: Data length (in byte) to read
///@param *pu8data \b IN: pointer to retun data buffer.
///@return MS_BOOL:
///- TRUE: Success
///- FALSE: Fail
/******************************************************************************/
MS_BOOL MApi_SWI2C_ReadBytes(MS_U16 u16BusNumSlaveID, MS_U8 ucSubAdr, MS_U8* paddr, MS_U32 ucBufLen, MS_U8* pBuf)
{
    MS_U8 u8Dummy; // loop dummy
    MS_BOOL bRet;

    if(u8BusAllocNum==0)
        return FALSE;

    u8Dummy = I2C_ACCESS_DUMMY_TIME;
    bRet = FALSE;

    IIC_MUTEX_LOCK();
    IIC_UseBus(HIBYTE(u16BusNumSlaveID));

    while (u8Dummy--)
    {
        if(g_SWI2CReadMode!=E_SWI2C_READ_MODE_DIRECT)
        {
            if (IIC_AccessStart(LOBYTE(u16BusNumSlaveID), SWI2C_WRITE) == FALSE)
            {
                continue;
            }

            while (ucSubAdr)
            {
                ucSubAdr--;
                if (IIC_SendByte(*paddr) == FALSE)
                {
                    goto fail;
                }
                paddr++;
            }

            if(g_SWI2CReadMode==E_SWI2C_READ_MODE_DIRECTION_CHANGE_STOP_START)
            {
                IIC_Stop();
            }
        }

        if (IIC_AccessStart(LOBYTE(u16BusNumSlaveID), SWI2C_READ) == FALSE)
        {
            continue;
        }

        while (ucBufLen--) // loop to burst read
        {
            *pBuf = IIC_GetByte(ucBufLen); // receive byte
            pBuf++; // next byte pointer
        }

        break;
    }
    bRet = TRUE;

fail:

    IIC_Stop();
    IIC_UnuseBus();
    IIC_MUTEX_UNLOCK();

    return bRet;
}

/******************************************************************************/
///Read 1 byte through IIC
///@param u16BusNumSlaveID \b IN: Bus Number (high byte) and Slave ID (Address) (low byte)
///@param u8RegAddr \b IN: Target register offset to read
///@param *pu8Data \b IN: pointer to 1 byte return data.
///@return MS_BOOL:
///- TRUE: Success
///- FALSE: Fail
/******************************************************************************/
MS_BOOL MApi_SWI2C_ReadByte(MS_U16 u16BusNumSlaveID, MS_U8 u8RegAddr, MS_U8 *pu8Data)
{
    MS_BOOL Result;
    Result=MApi_SWI2C_ReadBytes(u16BusNumSlaveID, 1, &u8RegAddr,1, pu8Data);
    return Result;
}

/******************************************************************************/
///Write 1 byte through IIC
///@param u16BusNumSlaveID \b IN: Bus Number (high byte) and Slave ID (Address) (low byte)
///@param u8RegAddr \b IN: Target register offset to write
///@param u8Data \b IN: 1 byte data to write
///@return MS_BOOL:
///- TRUE: Success
///- FALSE: Fail
/******************************************************************************/
MS_BOOL MApi_SWI2C_WriteByte(MS_U16 u16BusNumSlaveID, MS_U8 u8RegAddr, MS_U8 u8Data)
{
    return( MApi_SWI2C_WriteBytes(u16BusNumSlaveID, 1, &u8RegAddr, 1, &u8Data) );
}
//------------------------------------------------------------------------
MS_BOOL MApi_SWI2C_Write2Bytes(MS_U16 u16BusNumSlaveID, MS_U8 u8addr, MS_U16 u16data)
{
    MS_U8 u8Data[2];
    u8Data[0] = (u16data>>8) & 0xFF;
    u8Data[1] = (u16data) & 0xFF;
    return (MApi_SWI2C_WriteBytes(u16BusNumSlaveID, 1, &u8addr, 2, u8Data));
}

MS_U16 MApi_SWI2C_Read2Bytes(MS_U16 u16BusNumSlaveID, MS_U8 u8addr)
{
    MS_U8 u8Data[2];
    MApi_SWI2C_ReadBytes(u16BusNumSlaveID, 1, &u8addr, 2, u8Data);

    return ( (((MS_U16)u8Data[0])<<8)|u8Data[1] );
}

MS_BOOL MApi_SWI2C_WriteByteDirectly(MS_U16 u16BusNumSlaveID, MS_U8 u8Data)
{
    MS_BOOL bRet;

    bRet = FALSE;

    if(u8BusAllocNum==0)
        return FALSE;

    IIC_MUTEX_LOCK();
    IIC_UseBus(HIBYTE(u16BusNumSlaveID));
    IIC_Start();

    if (IIC_SendByte(LOBYTE(u16BusNumSlaveID) & ~BIT0) == FALSE)
        goto fail;

    if (IIC_SendByte(u8Data)==FALSE)
        goto fail;

    bRet = TRUE;
fail:

    IIC_Stop();
    IIC_UnuseBus();
    IIC_MUTEX_UNLOCK();

    return bRet;
}


MS_BOOL MApi_SWI2C_Write4Bytes(MS_U16 u16BusNumSlaveID, MS_U32 u32Data, MS_U8 u8EndData)
{
    MS_BOOL bRet;

    bRet = FALSE;

    if(u8BusAllocNum==0)
        return FALSE;

    IIC_MUTEX_LOCK();
    IIC_UseBus(HIBYTE(u16BusNumSlaveID));
    IIC_Start();

    if(IIC_SendByte( LOBYTE(u16BusNumSlaveID) & ~BIT0 )==FALSE)
        goto fail;

    if(IIC_SendByte( (MS_U8)(((MS_U32)u32Data)>>24) ) == FALSE )
        goto fail;

    if(IIC_SendByte( (MS_U8)(((MS_U32)u32Data)>>16) ) == FALSE )
        goto fail;

    if(IIC_SendByte( (MS_U8)(((MS_U32)u32Data)>>8) ) == FALSE )
        goto fail;

    if(IIC_SendByte( (MS_U8)(((MS_U32)u32Data)>>0) ) == FALSE )
        goto fail;

    if(IIC_SendByte(u8EndData)==FALSE)
        goto fail;

    bRet = TRUE;
fail:

    IIC_Stop();
    IIC_UnuseBus();
    IIC_MUTEX_UNLOCK();

    return bRet;
}

MS_BOOL MApi_SWI2C_WriteGroupBytes(MS_U16 u16BusNumSlaveID, MS_U8 u8SubGroup, MS_U16 u16Addr, MS_U16 u16Data)
{
    MS_BOOL bRet;

    bRet = FALSE;

    if(u8BusAllocNum==0)
        return FALSE;

    IIC_MUTEX_LOCK();
    IIC_UseBus(HIBYTE(u16BusNumSlaveID));
    IIC_Start();

    if(IIC_SendByte( LOBYTE(u16BusNumSlaveID) & ~BIT0 )==FALSE)
        goto fail;

    if(IIC_SendByte(u8SubGroup)==FALSE)
        goto fail;

    if(IIC_SendByte((u16Addr>>8)&0xFF)==FALSE)
        goto fail;

    if(IIC_SendByte(u16Addr&0xFF)==FALSE)
        goto fail;

    if(IIC_SendByte((u16Data>>8)&0xFF)==FALSE)
        goto fail;

    if(IIC_SendByte(u16Data&0xFF)==FALSE)
        goto fail;

    bRet = TRUE;
fail:

    IIC_Stop();
    IIC_UnuseBus();
    IIC_MUTEX_UNLOCK();

    return bRet;
}

MS_U16 MApi_SWI2C_ReadGroupBytes(MS_U16 u16BusNumSlaveID, MS_U8 u8SubGroup, MS_U16 u16Addr)
{
    MS_U16 u16Data = 0; // Modified it by coverity_507
    MS_U8 u8Address[3];

    u8Address[0] = u8SubGroup;
    u8Address[1] = (u16Addr>>8)&0xFF;
    u8Address[2] = u16Addr&0xFF;

    MApi_SWI2C_ReadBytes(u16BusNumSlaveID, 3, u8Address, 2, (MS_U8 *)&u16Data);

    return u16Data;
}



#undef _API_SWI2C_C_


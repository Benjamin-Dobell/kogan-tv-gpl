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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_iic.h
/// @brief  IIC Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_IIC_H_
#define _DRV_IIC_H_

#include <asm/types.h>
#include "mdrv_types.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

#define	IIC_NUM_OF_MAX				(20)

#if 1
#define	IIC_NUM_OF_HW				(0)
#define IIC_NUM_OF_SYS_EEPROM		(1)
#else
#define	IIC_NUM_OF_HW				(1)
#define IIC_NUM_OF_SYS_EEPROM		(2)
#define IIC_NUM_OF_NEC_MICOM		(3)
#define IIC_NUM_OF_AUDIO_AMP		(4)

#define IIC_NUM_OF_RGB_EDID			(8)
#define IIC_NUM_OF_HDMI_A_EDID		(9)
#define IIC_NUM_OF_HDMI_B_EDID		(10)
#define IIC_NUM_OF_HDMI_C_EDID		(11)
#define IIC_NUM_OF_HDMI_D_EDID		(12)
#endif

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct I2C_BusCfg
{
    U8 u8ChIdx;         ///Channel index
    U16 u16PadSCL;      ///Pad(Gpio) number for SCL
    U16 u16PadSDA;      ///Pad(Gpio) number for SDA
    U16 u16SpeedKHz;    ///Speed in KHz
    U8 u8Enable;        ///Enable
} __attribute__ ((packed));

typedef struct I2C_BusCfg I2C_BusCfg_t;

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
// for software IIC
void MDrv_SW_IIC_SetSpeed(U8 u8ChIIC, U8 u8Speed);
void MDrv_SW_IIC_Delay(U8 u8ChIIC);
void MDrv_SW_IIC_SCL(U8 u8ChIIC, U8 u8Data);
void MDrv_SW_IIC_SDA(U8 u8ChIIC, U8 u8Data);
void MDrv_SW_IIC_SCL_Chk(U8 u8ChIIC, B16 bSet);
void MDrv_SW_IIC_SDA_Chk(U8 u8ChIIC, B16 bSet);
B16 MDrv_SW_IIC_Start(U8 u8ChIIC);
void MDrv_SW_IIC_Stop(U8 u8ChIIC);
B16 MDrv_SW_IIC_SendByte(U8 u8ChIIC, U8 u8data, U8 u8Delay4Ack);
B16 MDrv_SW_IIC_AccessStart(U8 u8ChIIC, U8 u8SlaveAdr, U8 u8Trans);
U8 MDrv_SW_IIC_GetByte (U8 u8ChIIC, U16 u16Ack);
S32 MDrv_SW_IIC_Write(U8 u8ChIIC, U8 u8SlaveID, U8 u8AddrCnt, U8* pu8Addr, U32 u32BufLen, U8* pu8Buf);
S32 MDrv_SW_IIC_Read(U8 u8ChIIC, U8 u8SlaveID, U8 u8AddrCnt, U8* pu8Addr, U32 u32BufLen, U8* pu8Buf);
#if (defined(CONFIG_MSTAR_TITANIA)||defined(CONFIG_MSTAR_TITANIA2))
#else
void MDrv_SW_IIC_Enable( U8 u8ChIIC, B16 bEnable );
#endif
// for hardware IIC
void MDrv_IIC_Init(void);
void MDrv_HW_IIC_Clock_Select(U8 u8ClockIIC);
S32 MDrv_HW_IIC_Write(U8 u8SlaveIdIIC, U8 u8AddrSizeIIC, U8 *pu8AddrIIC, U32 u32BufSizeIIC, U8 *pu8BufIIC);
S32 MDrv_HW_IIC_Read(U8 u8SlaveIdIIC, U8 u8AddrSizeIIC, U8 *pu8AddrIIC, U32 u32BufSizeIIC, U8 *pu8BufIIC);
void MDrv_HW_IIC_Init( void );
#if (defined(CONFIG_MSTAR_TITANIA)||defined(CONFIG_MSTAR_TITANIA2))
#else
B16 MDrv_SW_IIC_ConfigBus(I2C_BusCfg_t* pBusCfg);
int MDrv_SW_IIC_GetSDA(U8 u8ChIIC);
int MDrv_SW_IIC_GetSCL(U8 u8ChIIC);
#endif

#endif // _DRV_IIC_H_


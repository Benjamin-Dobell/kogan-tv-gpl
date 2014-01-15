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

#ifndef _API_SWI2C_H_
#define _API_SWI2C_H_

//#include "MsCommon.h"
#include "SdMisc.h"


#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------
  
  
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
/// define IR library version
#define MSIF_SWI2C_LIB_CODE             {'S','I','2','C'}
#define MSIF_SWI2C_LIBVER               {'0','2'}
#define MSIF_SWI2C_BUILDNUM             {'0','1' }
#define MSIF_SWI2C_CHANGELIST           {'0','0','2','6','5','0','2','0'}
  
#define SWI2C_API_VERSION                 /* Character String for DRV/API version */  \
      MSIF_TAG,                           /* 'MSIF' */  \
      MSIF_CLASS,                         /* '00' */  \
      MSIF_CUS,                           /* 0x0000 */  \
      MSIF_MOD,                           /* 0x0000 */  \
      MSIF_CHIP,                                       \
      MSIF_CPU,                                        \
      MSIF_SWI2C_LIB_CODE,                /* IP__ */  \
      MSIF_SWI2C_LIBVER,                  /* 0.0 ~ Z.Z */  \
      MSIF_SWI2C_BUILDNUM,                /* 00 ~ 99 */  \
      MSIF_SWI2C_CHANGELIST,              /* CL# */  \
      MSIF_OS

  // Speed Mapping: Base on the Driver loading, maybe you must to use the oscilloscope to measure this Value
  // 250K => SwIicDly =1
  // 200K => SwIicDly =10
  // 150K => SwIicDly =20
  // 100K => SwIicDly =40

#define SWI2C_SPEED_MAPPING_250K          1
#define SWI2C_SPEED_MAPPING_200K          10
#define SWI2C_SPEED_MAPPING_150K          20
#define SWI2C_SPEED_MAPPING_100K          40


//typedef unsigned char               MS_U8;                              // 1 byte
/// data type unsigned short, data length 2 byte
//typedef unsigned short              MS_U16;                             // 2 bytes
/// data type unsigned int, data length 4 byte
//typedef unsigned long               MS_U32;                             // 4 bytes
/// data type signed char, data length 1 byte
//typedef signed char                 MS_S8;                              // 1 byte
/// data type signed short, data length 2 byte
//typedef signed short                MS_S16;                             // 2 bytes
/// data type signed int, data length 4 byte
//typedef signed long                 MS_S32;                             // 4 bytes
/// data type float, data length 4 byte
typedef float                       MS_FLOAT;                           // 4 bytes

/// definition for MS_BOOL
typedef unsigned char               MS_BOOL;
/// definition for VOID
typedef void                        VOID;
/// definition for FILEID



#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif






/// emurate SWI2C fucntion return result
typedef enum
{
    /// SWI2C result for failure
    E_SWI2C_FAIL =0,
    /// SWI2C result for OK
    E_SWI2C_OK = 1,

} SWI2C_Result;

///Define SWI2C Bus Configuration
typedef struct
{
    MS_U16 padSCL;
    MS_U16 padSDA;
    MS_U16 defDelay;
} SWI2C_BusCfg;

///Define SWI2C read mode
typedef enum {
    E_SWI2C_READ_MODE_DIRECT,                 ///< first transmit slave address + reg address and then start receive the data */
    E_SWI2C_READ_MODE_DIRECTION_CHANGE,       ///< slave address + reg address in write mode, direction change to read mode, repeat start slave address in read mode, data from device
    E_SWI2C_READ_MODE_DIRECTION_CHANGE_STOP_START,  ///< slave address + reg address in write mode + stop, direction change to read mode, repeat start slave address in read mode, data from device
    E_SWI2C_READ_MODE_MAX
} SWI2C_ReadMode;

///Define SWI2C debug level
typedef enum _SWI2C_DbgLvl
{
    E_SWI2C_DBGLVL_NONE = 0,      /// no debug message
    E_SWI2C_DBGLVL_WARNING,       /// show warning only
    E_SWI2C_DBGLVL_ERROR,         /// show error only
    E_SWI2C_DBGLVL_INFO,          /// show error & informaiton
    E_SWI2C_DBGLVL_ALL,           /// show error, information & funciton name
}SWI2C_DbgLvl;

typedef enum
{
    SD_COMMON_I2C_READ_MODE_DIRECT,                 ///< first transmit slave address + reg address and then start receive the data */
    SD_COMMON_I2C_READ_MODE_DIRECTION_CHANGE,       ///< slave address + reg address in write mode, direction change to read mode, repeat start slave address in read mode, data from device
    SD_COMMON_I2C_READ_MODE_DIRECTION_CHANGE_STOP_START,  ///< slave address + reg address in write mode + stop, direction change to read mode, repeat start slave address in read mode, data from device
    SD_COMMON_I2C_READ_MODE_MAX
} SdCommon_IicReadMode_k;


typedef enum
{
    SD_COMMON_I2C_ADDRRESS_MODE_NO_ADDR,            ///< no address - direct addressing . . . not using any reg address offset
    SD_COMMON_I2C_ADDRRESS_MODE_8_BIT,              ///< 8-bit address
    SD_COMMON_I2C_ADDRRESS_MODE_16_BIT,             ///< 16-bit normal(restart) addressing
    SD_COMMON_I2C_ADDRRESS_MODE_24_BIT,             ///< 24-bit address
    SD_COMMON_I2C_ADDRRESS_MODE_MAX
} SdCommon_IicAddrMode_k;



typedef struct _tagI2CConfigParams_t
{
    unsigned char           ui8Channel;         	///< iic channel
    unsigned int            ui32SlaveAddress;		///< slave address
    SdCommon_IicAddrMode_k  eAddrMode;          	///< address mode : no address, 8-bit, 16-bit, 24-bit
    SdCommon_IicReadMode_k  eReadMode;				///< read mode : Direct, Direction change
    unsigned int            clkFreqInKhz;   		///< interms of khz    
} SdCommon_IicParam_t;


void MApi_SWI2C_Init(SWI2C_BusCfg SWI2CBusCfg[],MS_U8 u8CfgBusNum);
MS_BOOL MApi_SWI2C_WriteBytes(MS_U16 u16BusNumSlaveID, MS_U8 u8addrcount, MS_U8* pu8addr, MS_U32 u32size, MS_U8* pu8data);
MS_BOOL MApi_SWI2C_ReadBytes(MS_U16 u16BusNumSlaveID, MS_U8 u8AddrNum, MS_U8* paddr, MS_U32 u32size, MS_U8* pu8data);
MS_BOOL MApi_SWI2C_ReadByte(MS_U16 u16BusNumSlaveID, MS_U8 u8RegAddr, MS_U8 *pu8Data);
MS_BOOL MApi_SWI2C_WriteByte(MS_U16 u16BusNumSlaveID, MS_U8 u8RegAddr, MS_U8 u8Data);
MS_BOOL MApi_SWI2C_Write2Bytes(MS_U16 u16BusNumSlaveID, MS_U8 u8addr, MS_U16 u16data);
MS_U16 MApi_SWI2C_Read2Bytes(MS_U16 u16BusNumSlaveID, MS_U8 u8addr);
MS_BOOL MApi_SWI2C_Write4Bytes(MS_U16 u16BusNumSlaveID, MS_U32 u32Data, MS_U8 u8EndData);
MS_BOOL MApi_SWI2C_ReadByteDirectly(MS_U16 u16BusNumSlaveID, MS_U8 * pu8Data);
MS_BOOL MApi_SWI2C_WriteGroupBytes(MS_U16 u16BusNumSlaveID, MS_U8 u8SubGroup, MS_U16 u16Addr, MS_U16 u16Data);
MS_U16 MApi_SWI2C_ReadGroupBytes(MS_U16 u16BusNumSlaveID, MS_U8 u8SubGroup, MS_U16 u16Addr);
MS_U32 MApi_SWI2C_Speed_Setting(MS_U8 u8BusNum, MS_U32 u32Speed_K);
MS_BOOL MApi_SWI2C_SetReadMode(SWI2C_ReadMode eReadMode);
//MS_BOOL MApi_SWI2C_GetLibVer(const MSIF_Version **ppVersion);
SWI2C_Result MApi_SWI2C_SetDbgLevel(SWI2C_DbgLvl eLevel);

#ifdef __cplusplus
}
#endif

#endif // _API_SWI2C_H_

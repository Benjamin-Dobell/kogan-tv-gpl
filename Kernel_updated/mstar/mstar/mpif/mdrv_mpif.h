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

///
/// @file   drvMPIF.h
/// @brief  MPIF Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_MPIF_H_
#define _DRV_MPIF_H_

#include "mhal_mpif_reg.h"

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
#define MSIF_MPIF_TAG                          {'M','S','I','F'}    //Version ID header
#define MSIF_MPIF_CLASS                        {'1','0'}            //info Class code
#define MSIF_MPIF_CUS                          0x6666               //Customer ID
#define MSIF_MPIF_MOD                          0x0000               //Module ID
#define MSIF_MPIF_CHIP                         0x000F               //CHIP ID: T3
#define MSIF_MPIF_CPU                          '1'                  //CPU
#define MSIF_MPIF_LIB_CODE                     {'M','P','I','F'}    //Lib code
#define MSIF_MPIF_LIBVER                       {'0','2'}            //LIB version
#define MSIF_MPIF_BUILDNUM                     {'0','0'}            //Build Number
#define MSIF_MPIF_CHANGELIST                   {'0','0','3','2','8','3','4','6'} //P4 ChangeList Number
#define MSIF_MPIF_OS                           '0'                  //OS

#define     MPIF_DRV_VERSION                  /* Character String for DRV/API version             */  \
            MSIF_MPIF_TAG,                         /* 'MSIF'                                           */  \
            MSIF_MPIF_CLASS,                       /* '00'                                             */  \
            MSIF_MPIF_CUS,                         /* 0x0000                                           */  \
            MSIF_MPIF_MOD,                         /* 0x0000                                           */  \
            MSIF_MPIF_CHIP,                                                                              \
            MSIF_MPIF_CPU,                                                                               \
            MSIF_MPIF_LIB_CODE,                    /* IP__                                             */  \
            MSIF_MPIF_LIBVER,                      /* 0.0 ~ Z.Z                                        */  \
            MSIF_MPIF_BUILDNUM,                    /* 00 ~ 99                                          */  \
            MSIF_MPIF_CHANGELIST,                  /* CL#                                              */  \
            MSIF_MPIF_OS


typedef struct _HAL_MPIF_CONFIG	DRV_MPIF_CONFIG;
typedef struct _HAL_MPIF_2XCTL  DRV_MPIF_2XCTL;
typedef struct _HAL_MPIF_3XCTL  DRV_MPIF_3XCTL;
typedef struct _HAL_MPIF_4ACTL  DRV_MPIF_4ACTL;

typedef enum _HAL_MPIF_DebugLevel DRV_MPIF_DebugLevel;

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum 
{
    MPIF_SUCCESS = 0,
    MPIF_CHANNEL_BUSY,    
    MPIF_INVALID_PARAMETER,
    MPIF_TRANSMISSION_FAILED,
    MPIF_FETAL_ERROR,    
    MPIF_UNKNOWN_ERROR,
}DRV_MPIF_DrvStatus;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Description: Show the MPIF driver version
/// @param ppVersion  		\b OUT  Library version string
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_GetLibVer(const MSIF_Version **ppVersion);

//------------------------------------------------------------------------------
/// Set detailed level of MPIF driver debug message
/// @param eDbgLevel  		\b IN  	debug level for MPIF driver
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_SetDbgLevel(DRV_MPIF_DebugLevel eDbgLevel);

//------------------------------------------------------------------------------
/// MPIF driver init
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Init(void);

//-------------------------------------------------------------------------------------------------
/// Exit MPIF driver
/// @return DRV_MPIF_DrvStatus
/// @note   called only once at system initialization
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Uninit(void);

//------------------------------------------------------------------------------
/// Set LC2X configuration
/// @param u8slaveid	  			\b IN 	Slave ID
/// @param p2xctrl 				\b IN 	pointer stored LC2X configure setting
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC2X_Configure(MS_U8 u8slaveid, DRV_MPIF_2XCTL *p2xctrl);

//------------------------------------------------------------------------------
/// Set LC3X configuration
/// @param u8slaveid	  			\b IN 	Slave ID
/// @param p3xctrl 				\b IN 	pointer stored LC3X configure setting
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3X_Configure(MS_U8 u8slaveid, DRV_MPIF_3XCTL *p3xctrl);

//------------------------------------------------------------------------------
/// Set LC3X configuration
/// @param u8slaveid	  			\b IN 	Slave ID
/// @param p4actrl 				\b IN 	pointer stored LC4A configure setting
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC4A_Configure(MS_U8 u8slaveid, DRV_MPIF_4ACTL *p4actrl);

//-------------------------------------------------------------------------------------------------
/// MPIF set 2X path to be SPIF or XIU
/// @param u8slaveid  	\b IN : Slave ID
/// @param u8path      \b IN : 2X path data
/// @return TRUE if successed or return FALSE if failed.
/// @note   Please always call this function before calling LC2X read/write function
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Set_LC2XPath(MS_U8 u8slaveid, MS_U8 u8path);

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 1A read/write
/// @param bWrite  	\b IN : If set to true, do writing; else do reading.
/// @param u8slaveid  	\b IN : Slave ID
/// @param u8index    	\b IN : index of 1A channel
/// @param pu8data    \b OUT : pointer which stored reading data or writing data
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC1A_RW(MS_U8 bWrite, MS_U8 u8slaveid, MS_U8 u8index, MS_U8 *pu8data);

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 2A read/write
/// @param bWrite  		 \b IN : If set to TRUE, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u16addr   		 \b IN : Reading address
/// @param pu16data   	 \b OUT : Pointer to stored reading data. It's a visual address.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC2A_RW(MS_U8 bWrite, MS_U8 u8slaveid, MS_U16 u16addr, MS_U16* pu16data);

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 2B read/write
/// @param bWrite  		 \b IN : If set to TRUE, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u16addr   		 \b IN : Reading address
/// @param pu16data   	 \b OUT : Pointer to stored reading data. It's a visual address.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC2B_RW(MS_U8 bWrite, MS_U8 u8slaveid, MS_U16 u16addr, MS_U16* pu16data);

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3A RIU mode ead/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u8datalen 		 \b IN : Data length. Unit by 16 bytes.
/// @param pu8data  		 \b IN/OUT : pointer to store writing/reading data. It's a visual address.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3A_RIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U8 u8datalen, MS_U8 *pu8data);

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3B RIU mode read/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u8datalen 		 \b IN : Data length. Unit by 16 bytes.
/// @param pu8data  		 \b IN/OUT : pointer to store writing/reading data. It's a visual address.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3B_RIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U8 u8datalen, MS_U8 *pu8data);

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3A MIU mode read/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u32datalen		 \b IN : Data length. Unit by 16 bytes.
/// @param u32miu_addr	 \b IN : MPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @param u32spif_mdr	 \b IN : SPIF MIU Start address. Unit is byte. Must be 8bytes aligmant. 
/// @return DRV_MPIF_DrvStatus
/// @note   If using cache buffer, please do buffer flush first.
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3A_MIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U32 u32datalen, MS_PHYADDR u32miu_addr, MS_PHYADDR u32spif_mdr);

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3B MIU mode read/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u32datalen		 \b IN : Data length. Unit by 16 bytes.
/// @param u32miu_addr	 \b IN : MPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @param u32spif_mdr	 \b IN : SPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @return DRV_MPIF_DrvStatus
/// @note   If using cache buffer, please do buffer flush first.
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3B_MIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U32 u32datalen, MS_PHYADDR u32miu_addr, MS_PHYADDR u32spif_mdr);

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3B MIU mode read/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u16datalen		 \b IN : Data length. Unit by 16 bytes.
/// @param u32miu_addr	 \b IN : MPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @param u32spif_mdr	 \b IN : SPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC4A_MIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U16 u16datalen, MS_PHYADDR u32miu_addr, MS_PHYADDR u32spif_mdr);

//-------------------------------------------------------------------------------------------------
/// MPIF printf SPIF's configure information
/// @param u8slaveid    	 \b IN : Slave ID
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Print_SPIFCfgInfo(MS_U8 u8slaveid);

//-------------------------------------------------------------------------------------------------
/// MPIF Set commend and slave data-width
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u8cmdwidth	 \b IN : Commend data width
/// @param u8datawidth	 \b IN : Slave data width
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_SetCmdDataMode(MS_U8 u8slaveid, MS_U8 u8cmdwidth, MS_U8 u8datawidth);


//-------------------------------------------------------------------------------------------------
/// MPIF Clock Disable or Enable
/// @param bDisabled    			 \b IN : If true, enable MPIF clock; if false, disable MPIF clock 
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Clock_Disable(MS_BOOL bDisabled);

#ifdef __cplusplus
}
#endif

#endif // _DRV_MPIF_H_

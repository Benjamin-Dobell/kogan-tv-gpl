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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvMPIF.c
/// @brief MPIF Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include <linux/spinlock.h>
#include <linux/delay.h>

// Internal Definition
#include "mhal_mpif.h"
#include "mdrv_mpif.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MPIF_MUTEX_WAIT_TIME    						0xFFFFFFFF

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Extern function
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MSIF_Version _drv_mpif_version = {
    .DDI = { MPIF_DRV_VERSION },
};

static MS_BOOL _u8Slave2XDest[4] = {MPIF_2X_DEST_SPIF, MPIF_2X_DEST_SPIF, MPIF_2X_DEST_SPIF, MPIF_2X_DEST_SPIF};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static MS_BOOL _MDrv_MPIF_GetSPIF_ChBusySts(MS_U8 u8slaveid, MS_U16 u16event_bit, long s32timeout)
{
    MS_U16 u16data;
    long s32i;
	MS_BOOL bRes = FALSE;
	
    // Use a limit value to prevent infinite loop
    for (s32i=0; s32i < s32timeout; s32i++)
    {
        if(!HAL_MPIF_LC2A_RW(FALSE, u8slaveid, REG_SPIF_CHANNEL_BUSY_STATUS, &u16data))
            break;		
		
        if((u16data & u16event_bit) == 0x00)        
            return TRUE;        

		udelay(1);
    }

	DEBUG_MPIF(MPIF_DBGLV_ERR, printk("SPIF channel status busy.\n"));

    return bRes;
}

static MS_BOOL _MDrv_MPIF_SetLC2XDest_SPIF(MS_U8 u8slaveid)
{
	MS_U8 u8path = MPIF_2X_DEST_SPIF;
	
	if(_u8Slave2XDest[u8slaveid] == u8path)
		return TRUE;
	
	return (HAL_MPIF_LC1A_RW(TRUE, u8slaveid, MPIF_LC1A_INDEX_1, &u8path));		
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// Description: Show the MPIF driver version
/// @param ppVersion  \b OUT  Library version string
/// @return TRUE : succeed
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
        return FALSE;

    *ppVersion = &_drv_mpif_version;

    return MPIF_SUCCESS;
}

//------------------------------------------------------------------------------
/// Set detailed level of MPIF driver debug message
/// @param eDbgLevel  \b IN  debug level for MPIF driver
/// @return TRUE : succeed
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_SetDbgLevel(DRV_MPIF_DebugLevel eDbgLevel)
{
    _stMPIFDbgLevel = eDbgLevel;

    return MPIF_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// Initialize MPIF driver
/// @return DRV_MPIF_DrvStatus
/// @note   called only once at system initialization
//-------------------------------------------------------------------------------------------------
// struct MPIF_CONFIG must sync to HAL struct HAL_MPIF_CONFIG
DRV_MPIF_DrvStatus MDrv_MPIF_Init(void)
{ 
    _stMPIFDbgLevel = MPIF_DBGLV_INFO; // init debug level first
       
    DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s()\n", __FUNCTION__));
	
    //
    //  1. HAL init
    //        	    
    HAL_MPIF_Init();	

	return MPIF_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// Exit MPIF driver
/// @return DRV_MPIF_DrvStatus
/// @note   called only once at system initialization
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Uninit(void)
{
    return MPIF_SUCCESS;
}

//------------------------------------------------------------------------------
/// Set LC2X configuration
/// @param u8slaveid	  			\b IN 	Slave ID
/// @param p2xctrl 				\b IN 	pointer stored LC2X configure setting
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC2X_Configure(MS_U8 u8slaveid, DRV_MPIF_2XCTL *p2xctrl)
{
	if(p2xctrl == NULL)
		return MPIF_INVALID_PARAMETER;
	
	HAL_MPIF_LC2X_Configure(u8slaveid, p2xctrl);

	return MPIF_SUCCESS;
}

//------------------------------------------------------------------------------
/// Set LC3X configuration
/// @param u8slaveid	  			\b IN 	Slave ID
/// @param p3xctrl 				\b IN 	pointer stored LC3X configure setting
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3X_Configure(MS_U8 u8slaveid, DRV_MPIF_3XCTL *p3xctrl)
{
	if(p3xctrl == NULL)
		return MPIF_INVALID_PARAMETER;
	
	p3xctrl->bfmode = FALSE; //for HW issue
	HAL_MPIF_LC3X_Configure(u8slaveid, p3xctrl);

	return MPIF_SUCCESS;
}

//------------------------------------------------------------------------------
/// Set LC3X configuration
/// @param u8slaveid	  			\b IN 	Slave ID
/// @param p4actrl 				\b IN 	pointer stored LC4A configure setting
/// @return DRV_MPIF_DrvStatus
//------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC4A_Configure(MS_U8 u8slaveid, DRV_MPIF_4ACTL *p4actrl)
{
	if(p4actrl == NULL)
		return MPIF_INVALID_PARAMETER;
	
	MDrv_MPIF_LC4A_Configure(u8slaveid, p4actrl);
	
	return MPIF_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 1A read/write
/// @param bWrite  	\b IN : If set to true, do writing; else do reading.
/// @param u8slaveid  	\b IN : Slave ID
/// @param u8index    	\b IN : index of 1A channel
/// @param pu8data    \b OUT : pointer which stored reading data or writing data
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC1A_RW(MS_U8 bWrite, MS_U8 u8slaveid, MS_U8 u8index, MS_U8 *pu8data)
{		
	MS_BOOL bRes = FALSE;	
	
	if(pu8data == NULL)
		return MPIF_INVALID_PARAMETER;
	
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, %d, %d, 0x%p)\n", __FUNCTION__, (int)bWrite, (int)u8slaveid, (int)u8index, pu8data));
	bRes = HAL_MPIF_LC1A_RW(bWrite, u8slaveid, u8index, pu8data);
			
	return bRes; 
}

//-------------------------------------------------------------------------------------------------
/// MPIF set 2X path to be SPIF or XIU
/// @param u8slaveid  	\b IN : Slave ID
/// @param u8path      \b IN : 2X path data
/// @return DRV_MPIF_DrvStatus
/// @note   Please always call this function before calling LC2X read/write function
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Set_LC2XPath(MS_U8 u8slaveid, MS_U8 u8path)
{
	MS_BOOL bRes = FALSE;
	MS_U8 u8data = u8path;
	
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, %d)\n", __FUNCTION__, (int)u8slaveid, (int)u8path));

	if(_u8Slave2XDest[u8slaveid] == u8path)
		return MPIF_SUCCESS;
	bRes = HAL_MPIF_LC1A_RW(TRUE, u8slaveid, MPIF_LC1A_INDEX_1, &u8data);
	
	return bRes;
}

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 2A read/write
/// @param bWrite  		 \b IN : If set to TRUE, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u16addr   		 \b IN : Reading address
/// @param pu16data   	 \b OUT : Pointer to stored reading data. It's a visual address.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC2A_RW(MS_U8 bWrite, MS_U8 u8slaveid, MS_U16 u16addr, MS_U16* pu16data)
{		
	MS_BOOL bRes = FALSE;
	
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, 0x%x, 0x%p)\n", __FUNCTION__, (int)u8slaveid, u16addr, pu16data));

	if(pu16data == NULL)
		return MPIF_INVALID_PARAMETER;	

	bRes = HAL_MPIF_LC2A_RW(bWrite, u8slaveid, u16addr, pu16data);	
		
	return (bRes ? MPIF_SUCCESS: MPIF_FETAL_ERROR);
}

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 2B read/write
/// @param bWrite  		 \b IN : If set to TRUE, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u16addr   		 \b IN : Reading address
/// @param pu16data   	 \b OUT : Pointer to stored reading data. It's a visual address.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC2B_RW(MS_U8 bWrite, MS_U8 u8slaveid, MS_U16 u16addr, MS_U16* pu16data)
{		
	MS_BOOL bRes = FALSE;
	
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, 0x%x, 0x%p)\n", __FUNCTION__, (int)u8slaveid, u16addr, pu16data));

	if(pu16data == NULL)
		return MPIF_INVALID_PARAMETER;
	
	bRes = HAL_MPIF_LC2B_RW(bWrite, u8slaveid, u16addr, pu16data);
		
	return (bRes ? MPIF_SUCCESS: MPIF_FETAL_ERROR);
}

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3A RIU mode ead/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u8datalen 		 \b IN : Data length. Unit is byte.
/// @param pu8data  		 \b IN/OUT : pointer to store writing/reading data. It's a visual address.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3A_RIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U8 u8datalen, MS_U8 *pu8data)
{	
	MS_U16 u16data;			
	
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, %d, %d, 0x%p)\n", __FUNCTION__, (int)bWrite, (int)u8slaveid, u8datalen, pu8data));

	if(pu8data == NULL)
		return MPIF_INVALID_PARAMETER;		

	// Check the parameter
	if (u8datalen > MPIF_LC3X_PACKET_DATA_MAX_LENGTH)
	{
		DEBUG_MPIF(MPIF_DBGLV_ERR, printk("Data length can't be greater than %d bytes.\n", MPIF_LC3X_PACKET_DATA_MAX_LENGTH));
		return MPIF_INVALID_PARAMETER;
	}
	
	//----------------------- Configure SPIF------------------------------		
	if(!_MDrv_MPIF_SetLC2XDest_SPIF(u8slaveid))
	{		
		return MPIF_TRANSMISSION_FAILED;
	}
	
	if(!_MDrv_MPIF_GetSPIF_ChBusySts(u8slaveid, SPIF_LC3A_BUSY, MAX_TIMEOUT))
	{
		return MPIF_CHANNEL_BUSY;		
	}

	u16data = 1;
	if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC3A_PACKET_LEN, &u16data))  
	{
		return MPIF_TRANSMISSION_FAILED;			
	}
	
	u16data = (MS_U16)(bWrite & 0x01) | (0x0001 << 1); //from RIU
	if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC3A_PACKET_CONTROL, &u16data))  
	{
		return MPIF_TRANSMISSION_FAILED;		
	}	
	//-------------------------------------------------------------------	
	
	if(!HAL_MPIF_LC3A_RIURW(bWrite, u8slaveid, u8datalen, pu8data))	
		return MPIF_TRANSMISSION_FAILED;		

	return MPIF_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3B RIU mode read/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u8datalen 		 \b IN : Data length. Unit is byte.
/// @param pu8data  		 \b IN/OUT : pointer to store writing/reading data. It's a visual address.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3B_RIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U8 u8datalen, MS_U8 *pu8data)
{	
	MS_U16 u16data;
	
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, %d, %d, 0x%p)\n", __FUNCTION__, (int)bWrite, (int)u8slaveid, u8datalen, pu8data));

	if(pu8data == NULL)
		return MPIF_INVALID_PARAMETER;
	
	// Check the parameter
	if(u8datalen > MPIF_LC3X_PACKET_DATA_MAX_LENGTH)
	{
		DEBUG_MPIF(MPIF_DBGLV_ERR, printk("Data length can't be greater than %d bytes.\n", MPIF_LC3X_PACKET_DATA_MAX_LENGTH));
		return MPIF_INVALID_PARAMETER;
	}		

	//----------------------- Configure SPIF------------------------------	
	if(!_MDrv_MPIF_SetLC2XDest_SPIF(u8slaveid))
	{
		return MPIF_TRANSMISSION_FAILED;		
	}

	if(!_MDrv_MPIF_GetSPIF_ChBusySts(u8slaveid, SPIF_LC3B_BUSY, MAX_TIMEOUT))
	{
		return MPIF_CHANNEL_BUSY;    		
	}

	u16data= 1;
	if(!HAL_MPIF_LC2B_RW(TRUE, u8slaveid, REG_SPIF_LC3B_PACKET_LEN, &u16data))  
	{
		return MPIF_TRANSMISSION_FAILED;		
	}
	u16data = (MS_U16)(bWrite & 0x01) | (0x0001 << 1); //from RIU
	if(!HAL_MPIF_LC2B_RW(TRUE,u8slaveid, REG_SPIF_LC3B_PACKET_CONTROL, &u16data))  
	{
		return MPIF_TRANSMISSION_FAILED;		
	}	
	//-------------------------------------------------------------------		

	if(!HAL_MPIF_LC3B_RIURW(bWrite, u8slaveid, u8datalen, pu8data))	
		return MPIF_TRANSMISSION_FAILED;	

	return MPIF_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3A MIU mode read/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u32datalen	 \b IN : Data length. Unit by 16 bytes.
/// @param u32miu_addr	 \b IN : MPIF MIU Start address. Unit is byte. Must be MIU bus aligmant.
/// @param u32spif_mdr	 \b IN : SPIF MIU Start address. Unit is MIU bus aligmant.
/// @return DRV_MPIF_DrvStatus
/// @note   If using cache buffer, please do buffer flush first.
//------------------------------------------------------------------------------------------------- 
DRV_MPIF_DrvStatus MDrv_MPIF_LC3A_MIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U32 u32datalen, MS_PHYADDR u32miu_addr, MS_PHYADDR u32spif_mdr)
{
    MS_U16 u16data;  	
    
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, %d, %d, 0x%x, 0x%x)\n", __FUNCTION__, (int)bWrite, (int)u8slaveid, (int)u32datalen,
				(unsigned int)u32miu_addr, (unsigned int)u32spif_mdr));		
	
    if(u32datalen % MPIF_LC3X_PACKET_DATA_MAX_LENGTH)
    {
        DEBUG_MPIF(MPIF_DBGLV_ERR, printk("LC3X packet data length should be mutipled by 16\n"));
        return MPIF_INVALID_PARAMETER;
    }
    if((u32datalen >> 4) > MPIF_LC3X_MAX_PACKET)
    {
        DEBUG_MPIF(MPIF_DBGLV_ERR, printk("LC3X packet data length overflow\n"));
        return MPIF_INVALID_PARAMETER;
    }    

    //----------------------- Configure SPIF------------------------------
    if(!_MDrv_MPIF_SetLC2XDest_SPIF(u8slaveid))
    {
    	return MPIF_TRANSMISSION_FAILED;		
    }
	
    if(!_MDrv_MPIF_GetSPIF_ChBusySts(u8slaveid, SPIF_LC3A_BUSY, MAX_TIMEOUT))
    {
    	return MPIF_CHANNEL_BUSY;            
    }
	
    u16data = ((MS_U16)bWrite&0x01) | (0x0001 << 1) | 0x04; //MIU 0
				
    if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC3A_PACKET_CONTROL, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }
	
    u16data = (MS_U16)(u32datalen >> 4); //unit is 16 bytes
    if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC3A_PACKET_LEN, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }	
    u16data = (MS_U16)((u32spif_mdr >> 3) & 0xFFFF);
    if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC3A_MADR_L, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }		
    u16data = (MS_U16)((u32spif_mdr >> 19) & 0xFF);
    if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC3A_MADR_H, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }	
    //--------------------------------------------------------------------	

	if(!HAL_MPIF_LC3A_MIURW(bWrite, u8slaveid, u32datalen, u32miu_addr))	
		return MPIF_TRANSMISSION_FAILED;
    
    return MPIF_SUCCESS;
}// Read data from the SPIF to the MIU via LC3A

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3B MIU mode read/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u32datalen	 \b IN : Data length. Unit by 16 bytes.
/// @param u32miu_addr	 \b IN : MPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @param u32spif_mdr	 \b IN : SPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @return DRV_MPIF_DrvStatus
/// @note   If using cache buffer, please do buffer flush first.
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC3B_MIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U32 u32datalen, MS_PHYADDR u32miu_addr, MS_PHYADDR u32spif_mdr)
{	
    MS_U16 u16data;  	

	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, %d, %d, 0x%x, 0x%x)\n", __FUNCTION__, (int)bWrite, (int)u8slaveid, (int)u32datalen,
				(unsigned int)u32miu_addr, (unsigned int)u32spif_mdr));

    if(u32datalen % MPIF_LC3X_PACKET_DATA_MAX_LENGTH)
    {
        DEBUG_MPIF(MPIF_DBGLV_ERR, printk("LC3X packet data length should be mutipled by 16\n"));
        return MPIF_INVALID_PARAMETER;
    }
    if((u32datalen >> 4) > MPIF_LC3X_MAX_PACKET)
    {
        DEBUG_MPIF(MPIF_DBGLV_ERR, printk("LC3X packet data length overflow\n"));
        return MPIF_INVALID_PARAMETER;
    }    
	
    //----------------------- Configure SPIF------------------------------
    if(!_MDrv_MPIF_SetLC2XDest_SPIF(u8slaveid))
    {
    	return MPIF_TRANSMISSION_FAILED;		
    }
	
    if(!_MDrv_MPIF_GetSPIF_ChBusySts(u8slaveid, SPIF_LC3B_BUSY, MAX_TIMEOUT))
    {
    	return MPIF_CHANNEL_BUSY;          
    }
	
    u16data = ((MS_U16)bWrite&0x01) | (0x0001 << 1) | 0x04 ; //MIU 0				
    if(!HAL_MPIF_LC2B_RW(TRUE, u8slaveid, REG_SPIF_LC3B_PACKET_CONTROL, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }	
    u16data = (MS_U16)(u32datalen >> 4); //unit is 16 bytes
    if(!HAL_MPIF_LC2B_RW(TRUE, u8slaveid, REG_SPIF_LC3B_PACKET_LEN, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }	
    u16data = (MS_U16)((u32spif_mdr >> 3) & 0xFFFF);
    if(!HAL_MPIF_LC2B_RW(TRUE, u8slaveid, REG_SPIF_LC3B_MADR_L, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }
	
    u16data = (MS_U16)((u32spif_mdr >> 19) & 0xFF);
    if(!HAL_MPIF_LC2B_RW(TRUE, u8slaveid, REG_SPIF_LC3B_MADR_H, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }	
    //--------------------------------------------------------------------	

	if(!HAL_MPIF_LC3B_MIURW(bWrite, u8slaveid, u32datalen, u32miu_addr))		
		return MPIF_TRANSMISSION_FAILED;	
	
    return MPIF_SUCCESS;
}// Read data from the SPIF to the MIU via LC3A

//-------------------------------------------------------------------------------------------------
/// MPIF logical channel 3B MIU mode read/write
/// @param bWrite	    	 \b IN : If set to true, do writing; else do reading.
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u16datalen	 \b IN : Data length. Unit by 16 bytes.
/// @param u32miu_addr	 \b IN : MPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @param u32spif_mdr	 \b IN : SPIF MIU Start address. Unit is byte. Must be 8bytes aligmant.
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_LC4A_MIURW(MS_BOOL bWrite, MS_U8 u8slaveid, MS_U16 u16datalen, MS_PHYADDR u32miu_addr, MS_PHYADDR u32spif_mdr)
{	
    MS_U16 u16data;      
	        
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, %d, %d, 0x%x, 0x%x)\n", __FUNCTION__, (int)bWrite, (int)u8slaveid, (int)u16datalen,
				(unsigned int)u32miu_addr, (unsigned int)u32spif_mdr));

    //----------------------- Configure SPIF------------------------------
    if(!_MDrv_MPIF_SetLC2XDest_SPIF(u8slaveid))
    {
    	return MPIF_TRANSMISSION_FAILED;		
    }
	 
    if(!_MDrv_MPIF_GetSPIF_ChBusySts(u8slaveid, SPIF_LC4A_BUSY, MAX_TIMEOUT))
    {
    	return MPIF_CHANNEL_BUSY;        
    }
	
    u16data = (((MS_U16)bWrite) & 0x01) | ((MS_U16)(MPIF_LC4A_CHECKPOINT_0 & 0x03) << 1); //MIU0
    if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC4A_CONTROL, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }	
    if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC4A_STREAM_LEN, &u16datalen))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }	
	
    u16data = (MS_U16)((u32spif_mdr >> 3) & 0xFFFF);
    if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC4A_MADR_L, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }
	
    u16data = (MS_U16)((u32spif_mdr >> 19) & 0xFF);
    if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_LC4A_MADR_H, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;        
    }	
    //--------------------------------------------------------------------	   

	if(!HAL_MPIF_LC4A_MIURW(bWrite, u8slaveid, u16datalen, u32miu_addr))		
		return MPIF_TRANSMISSION_FAILED;	

    return MPIF_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// MPIF printf SPIF's configure information
/// @param u8slaveid    	 \b IN : Slave ID
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Print_SPIFCfgInfo(MS_U8 u8slaveid)
{	
	MS_U16 u16data;
	DRV_MPIF_DrvStatus Res = MPIF_FETAL_ERROR;

	printk("\n------------SPIF----------------\n");
	
	//Read SPIF's MISC
    if(!HAL_MPIF_LC2A_RW(FALSE, u8slaveid, REG_SPIF_MISC1, &u16data))
    	goto GETINFO_END;	
	
    printk("Slave ID: %d\n", (int)(u16data & 0x03));
    printk("Commend bit %d\n", (int)((u16data >> 4) & 0x03));
    printk("Data bit %d\n", (int)((u16data >> 6) & 0x03));
    printk("Turn around cycle %d\n", (int)((u16data >> 8) & 0x03));
    printk("Wait cycle %d\n", (int)((u16data >> 10) & 0x03));

	if(!HAL_MPIF_LC2A_RW(FALSE, u8slaveid, REG_SPIF_MISC2, &u16data))
    	goto GETINFO_END;	
	
	printk("SPIF input clock delay %d\n", (int)(u16data & 0x0F));
	printk("SPIF input clock inv %d\n", (int)((u16data >> 4) & 0x01));
	printk("SPIF input clock inv delay %d\n", (int)((u16data >> 8) & 0x0F));
	printk("SPIF input clock inv inv enable %d\n", (int)((u16data >> 12) & 0x01));
	
    printk("---------------------------------\n\n");

	Res = MPIF_SUCCESS;

GETINFO_END:	

	return Res;
}

//-------------------------------------------------------------------------------------------------
/// MPIF Set commend and slave data-width
/// @param u8slaveid    	 \b IN : Slave ID
/// @param u8cmdwidth	 \b IN : Commend data width
/// @param u8datawidth	 \b IN : Slave data width
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_SetCmdDataMode(MS_U8 u8slaveid, MS_U8 u8cmdwidth, MS_U8 u8datawidth)
{
    MS_U16 u16data;	

	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d, %x, %x)\n", __FUNCTION__, (int)u8slaveid, u8cmdwidth, u8datawidth));				

    //---- Configure SPIF first ---------
    if(!_MDrv_MPIF_SetLC2XDest_SPIF(u8slaveid))
    {
    	return MPIF_TRANSMISSION_FAILED;		
    }
	
    if(!HAL_MPIF_LC2A_RW(FALSE, u8slaveid, REG_SPIF_MISC1, &u16data))
    {
    	return MPIF_TRANSMISSION_FAILED;         
    }
	
    u16data &= ~0xF0;
    u16data |= (((MS_U16)(u8cmdwidth & 0x03) << 4) | ((MS_U16)(u8datawidth & 0x03) << 6));
	if(!HAL_MPIF_LC2A_RW(TRUE, u8slaveid, REG_SPIF_MISC1, &u16data))
	{
		return MPIF_TRANSMISSION_FAILED;           
	}	
    //---------------------------------

    //---- Then change MPIF setting -----------
    if(HAL_MPIF_SetCmdDataMode(u8slaveid, u8cmdwidth, u8datawidth))
		return MPIF_SUCCESS;
    //-----------------------------------------

    return MPIF_UNKNOWN_ERROR;
}

//-------------------------------------------------------------------------------------------------
/// MPIF Clock Disable or Enable
/// @param bDisabled    			 \b IN : If true, enable MPIF clock; if false, disable MPIF clock 
/// @return DRV_MPIF_DrvStatus
//-------------------------------------------------------------------------------------------------
DRV_MPIF_DrvStatus MDrv_MPIF_Clock_Disable(MS_BOOL bDisabled)
{	
	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(%d)\n", __FUNCTION__, (int)bDisabled));	
	
	HAL_MPIF_CLK_Disable(bDisabled);

    return MPIF_SUCCESS;
}



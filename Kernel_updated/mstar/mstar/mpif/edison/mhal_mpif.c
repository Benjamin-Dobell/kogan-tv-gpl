////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2009-2012 MStar Semiconductor, Inc.
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
////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/interrupt.h>

// Internal Definition
#include "mhal_mpif_reg.h"
#include "mhal_mpif.h"
#include "chip_int.h"
#include "mst_devid.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#ifdef ENABLE_IRQ
#define WAITQUE_NUM								1 
#endif

#define _BIT(x)                             	(1<<(x))
#define BMASK(bits)                         	(_BIT(((1)?bits)+1)-_BIT(((0)?bits)))
#define BITS(bits,value)                    	((_BIT(((1)?bits)+1)-_BIT(((0)?bits))) & (value<<((0)?bits)))

#define READ_WORD(_reg)                     	(*(volatile MS_U16*)(_reg))
#define WRITE_WORD(_reg, _val)              	{(*((volatile MS_U16*)(_reg))) = (MS_U16)(_val); }
#define WRITE_WORD_MASK(_reg, _val, _mask)  	{(*((volatile MS_U16*)(_reg))) = ((*((volatile MS_U16*)(_reg))) & ~(_mask)) | ((MS_U16)(_val) & (_mask)); }

// MPIF
#define MPIF_RIU8								((unsigned char  volatile *)BK_MPIF)
#define MPIF_REG8(saddr, idx)					MPIF_RIU8[(saddr << 2) + (idx << 1) - ((idx) & 1)] 
#define MPIF_READ(addr)                     	READ_WORD((BK_MPIF + (addr << 2)))
#define MPIF_WRITE(addr, val)               	WRITE_WORD((BK_MPIF + (addr << 2)), (val))
#define MPIF_WRITE_MASK(addr, val, mask)    	WRITE_WORD_MASK(BK_MPIF + ((addr)<<2), (val), (mask))

#define CHIPTOP_READ(addr)                     	READ_WORD((BK_CHIPTOP + (addr << 2))) 
#define CHIPTOP_WRITE(addr, val)               	WRITE_WORD((BK_CHIPTOP + (addr << 2)), (val))
#define CHIPTOP_WRITE_MASK(addr, val, mask)    	WRITE_WORD_MASK(BK_CHIPTOP + ((addr)<<2), (val), (mask))

#define CLKGEN0_READ(addr)                     	READ_WORD((BK_CLKGEN0 + (addr << 2)))
#define CLKGEN0_WRITE(addr, val)               	WRITE_WORD((BK_CLKGEN0 + (addr << 2)), (val))
#define CLKGEN0_WRITE_MASK(addr, val, mask)    	WRITE_WORD_MASK(BK_CLKGEN0 + ((addr)<<2), (val), (mask))

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
static HAL_MPIF_2XCTL _st2XCtrl[4] =
{
	{TRUE, MPIF_MAX_RTX_0},
	{TRUE, MPIF_MAX_RTX_0},
	{TRUE, MPIF_MAX_RTX_0},
	{TRUE, MPIF_MAX_RTX_0},
};

static HAL_MPIF_3XCTL _st3XCtrl[4] = 
{                                    
	{TRUE, MPIF_RTX_INDICATOR_DISABLE, MPIF_MAX_RTX_0, MPIF_SEL_MIU0, TRUE, FALSE, 4}, 
	{TRUE, MPIF_RTX_INDICATOR_DISABLE, MPIF_MAX_RTX_0, MPIF_SEL_MIU0, TRUE, FALSE, 4}, 
	{TRUE, MPIF_RTX_INDICATOR_DISABLE, MPIF_MAX_RTX_0, MPIF_SEL_MIU0, TRUE, FALSE, 4}, 
	{TRUE, MPIF_RTX_INDICATOR_DISABLE, MPIF_MAX_RTX_0, MPIF_SEL_MIU0, TRUE, FALSE, 4}, 
};

static HAL_MPIF_4ACTL _st4ACtrl[4] =
{
	{MPIF_MAX_RTX_0, MPIF_SEL_MIU0, FALSE, MPIF_LC4A_CHECKPOINT_0, 8},
	{MPIF_MAX_RTX_0, MPIF_SEL_MIU0, FALSE, MPIF_LC4A_CHECKPOINT_0, 8},
	{MPIF_MAX_RTX_0, MPIF_SEL_MIU0, FALSE, MPIF_LC4A_CHECKPOINT_0, 8},
	{MPIF_MAX_RTX_0, MPIF_SEL_MIU0, FALSE, MPIF_LC4A_CHECKPOINT_0, 8},
};

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static spinlock_t _spinlock_mpif;

#ifdef ENABLE_IRQ
static MS_U16 	  	_u16IntSts = 0;
wait_queue_head_t 	_gmpif_waitqueue[WAITQUE_NUM];
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
// Wait the channel is free via checking the channel busy status
static MS_BOOL _HAL_MPIF_BusyWait_ChannelFree(MS_U8 u8event_bit, long s32timeout)
{    
    long s32i;

    // Use a limit value to prevent infinite loop
    for (s32i=0; s32i < s32timeout; s32i++)
    {
        if ((MPIF_READ(REG_MPIF_CHANNEL_BUSY_STATUS) & u8event_bit) == 0x00)       	
            return TRUE;        
    }
    
    DEBUG_MPIF(MPIF_DBGLV_INFO, printk("%s Timeout\n", __FUNCTION__));

    return FALSE;
}

// Wait the interrupt or error event
// return MPIF_EVENT_TX_DONE if transmission done, or MPIF_EVENT_TX_ERROR if transmission error
// otherwise, return MPIF_EVENT_NULL
// If u32timeout = 0, wait forever
static MS_U8 _HAL_MPIF_BusyWait_InterruptErrorEvent(MS_U16 u16event_bit, MS_U16 u16error_bit, long s32timeout)
{
    MS_U8 u8Ret = MPIF_EVENT_WAITTIMEOUT;
    long s32i = 0;
    MS_U16 u16temp = 0;
    MS_BOOL bloop = TRUE;

    // Use a limit value to prevent infinite loop
    while(bloop)
    {
        u16temp = MPIF_READ(REG_MPIF_INTERRUPT_EVENT_STATUS);

        if((u16temp & MPIF_EVENTBIT_BUSY_TIMEOUT) == MPIF_EVENTBIT_BUSY_TIMEOUT)
        {
            u8Ret = MPIF_EVENT_BUSYTIMEOUT;
            break;
        }

        // Check if TX done or TX error
        if((u16event_bit > MPIF_EVENTBIT_1A) && ((u16temp & u16error_bit) == u16error_bit))
        {
            u8Ret = MPIF_EVENT_TX_ERROR;
            break;            
        }
     
        if((u16temp & u16event_bit)== u16event_bit)
        {
            u8Ret = MPIF_EVENT_TX_DONE;
            break;
        }
        if(s32timeout > 0)
			bloop = (++s32i < s32timeout);
    }

    if(u8Ret != MPIF_EVENT_TX_DONE)
        DEBUG_MPIF(MPIF_DBGLV_INFO, printk("%s interrupt failed status=0x%x\n", __FUNCTION__, u16temp));
   
    MPIF_WRITE(REG_MPIF_INTERRUPT_EVENT_STATUS, u16temp); //clear status

    return u8Ret;
}

//undefine ENABLE_IRQ

#ifdef ENABLE_IRQ
static irqreturn_t _HAL_MPIF_ISR(int irq, void *dev_id)
{	
	MS_U16 	 u16temp = 0;	
	
	u16temp = MPIF_READ(REG_MPIF_INTERRUPT_EVENT_STATUS);
	_u16IntSts |= u16temp;
    wake_up_interruptible(&(_gmpif_waitqueue[0]));	
	MPIF_WRITE(REG_MPIF_INTERRUPT_EVENT_STATUS, u16temp);	

	return IRQ_HANDLED;
}

MS_U8 _HAL_MPIF_WaitEventDone(MS_U16 u16event, MS_U16 u16err_event, long s32timeout)
{	
	if(interruptible_sleep_on_timeout(&(_gmpif_waitqueue[0]), s32timeout) == 0)
		return MPIF_EVENT_WAITTIMEOUT;

	if(_u16IntSts & MPIF_EVENTBIT_BUSY_TIMEOUT)
	{
		_u16IntSts &= ~MPIF_EVENTBIT_BUSY_TIMEOUT;
		return MPIF_EVENT_BUSYTIMEOUT;
	}
	else if(_u16IntSts & u16err_event)
	{
		_u16IntSts &= ~u16err_event;
		return MPIF_EVENT_TX_ERROR;
	}
    else if(_u16IntSts & u16event)	
	{
		_u16IntSts &= ~u16event;
		return MPIF_EVENT_TX_DONE;
	}	
	
	return MPIF_EVENT_UNKNOWN;
}

#else

MS_U8 _HAL_MPIF_WaitEventDone(MS_U16 u16event, MS_U16 u16err_event, long s32timeout)
{	
	return (_HAL_MPIF_BusyWait_InterruptErrorEvent(u16event, u16err_event, s32timeout));	 
}

#endif //ENABLE_IRQ

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Please set clock, trun around cycle, commend data with,
//slave data width first to meet SPIF status
//--------------------------------------------------------------------------
HAL_MPIF_STATUS HAL_MPIF_Init(void)
{	
	MS_U32 spflag;	
	
    DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s()\n", __FUNCTION__));

#ifdef ENABLE_IRQ
	for(spflag = 0; spflag < WAITQUE_NUM; spflag++)
		init_waitqueue_head(&(_gmpif_waitqueue[spflag]));

	request_irq(E_IRQEXPL_MPIF, _HAL_MPIF_ISR, SA_INTERRUPT, "mpif", NULL);
#endif

	spin_lock_init(&_spinlock_mpif);

	spin_lock_irqsave(&_spinlock_mpif, spflag); 		
    CHIPTOP_WRITE_MASK(REG_ALLPAD_IN, BITS(15:15, 0), BMASK(15:15)); //Disable all pad in    
	CHIPTOP_WRITE_MASK(REG_MPIF_MODE, BITS(11:10, 3), BMASK(11:10)); //set 4 bit mode of pad to support all modes 	

	//CHIPTOP_WRITE_MASK(0x05, BITS(15:11, 0x1D), BMASK(15:11)); //driving 
	//CHIPTOP_WRITE_MASK(0x06, BITS(0:0, 1), BMASK(0:0)); //driving 

	HAL_MPIF_Set_Clock(MPIF_CLOCK_DEFAULT);	
	
    HAL_MPIF_SET_1A2XTRCycle(MPIF_TR_CYCLE_DEFAULT);
	HAL_MPIF_SET_3X4ATRCycle(MPIF_TR_CYCLE_DEFAULT);
	HAL_MPIF_SET_WaitCycle(MPIF_WAIT_CYCLE_DEFAULT);   

	HAL_MPIF_SET_SlaveDataWidth(0, MPIF_SLAVE_DATAWIDTH_1bit);
	HAL_MPIF_SET_SlaveDataWidth(1, MPIF_SLAVE_DATAWIDTH_1bit);
	HAL_MPIF_SET_SlaveDataWidth(2, MPIF_SLAVE_DATAWIDTH_1bit);
	HAL_MPIF_SET_SlaveDataWidth(3, MPIF_SLAVE_DATAWIDTH_1bit);

	HAL_MPIF_CmdDataWidth(MPIF_CMD_DATAWIDTH_DEFAULT);

    MPIF_WRITE(REG_MPIF_BUSY_TIMEOUT_COUNTER, 0xFFFF); //wait forever
    MPIF_WRITE(REG_MPIF_INTERRUPT_EVENT_MASK, MPIF_TNTBIT_ENABLE_MASK);	
	MPIF_WRITE(REG_MPIF_INTERRUPT_EVENT_STATUS, 0xFFFF); //clear interrupt status

	HAL_MPIF_SWReset();

	MPIF_WRITE_MASK(REG_MPIF_MIU_WPROTECT_CONTROL, BITS(3:2, 3), BMASK(3:2)); //enable dumy write
	MPIF_WRITE_MASK(REG_MPIF_MISC2, BITS(10:10, 1), BMASK(10:10)); //enable reg_mpif_miu_w4wlast_done
	//MPIF_WRITE_MASK(REG_MPIF_THROTTLE_CFG, BITS(1:0, MPIF_CLK_THROTTLE_RATE_CYL3), BMASK(1:0));
	//MPIF_WRITE_MASK(REG_MPIF_THROTTLE_CFG, BITS(5:4, MPIF_CLK_THROTTLE_SIZE_8BYTES), BMASK(5:4));	

	DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("MPIF init done!\n"));

	spin_unlock_irqrestore(&_spinlock_mpif, spflag);	

	return HAL_MPIF_SUCCESS;
	
}

void HAL_MPIF_Exit(void)
{
#ifdef ENABLE_IRQ
	free_irq(E_IRQEXPL_MPIF, NULL);
#endif
}

HAL_MPIF_STATUS HAL_MPIF_LC2X_Configure(MS_U8 u8slaveid, HAL_MPIF_2XCTL *p2xctrl)
{
	MS_U32 spflag;	
	
	if(p2xctrl == NULL)
		return HAL_MPIF_INVALID_PARAMETER;

	spin_lock_irqsave(&_spinlock_mpif, spflag);
	_st2XCtrl[u8slaveid].bchksum_en = p2xctrl->bchksum_en;
	_st2XCtrl[u8slaveid].u8retrx = p2xctrl->u8retrx;
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
	
	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_LC3X_Configure(MS_U8 u8slaveid, HAL_MPIF_3XCTL *p3xctrl)
{
	MS_U32 spflag;	
	
	if(p3xctrl == NULL)
		return HAL_MPIF_INVALID_PARAMETER;

	spin_lock_irqsave(&_spinlock_mpif, spflag); 
	_st3XCtrl[u8slaveid].bchksum_en = p3xctrl->bchksum_en;
	_st3XCtrl[u8slaveid].bfmode = p3xctrl->bfmode;
	_st3XCtrl[u8slaveid].bNotWaitMIUDone = p3xctrl->bNotWaitMIUDone;
	_st3XCtrl[u8slaveid].bretrx_idx = p3xctrl->bretrx_idx;
	_st3XCtrl[u8slaveid].u8MIUSel = p3xctrl->u8MIUSel;
	_st3XCtrl[u8slaveid].u8retrx = p3xctrl->u8retrx;
	_st3XCtrl[u8slaveid].u8Wcnt = p3xctrl->u8Wcnt;
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_LC4A_Configure(MS_U8 u8slaveid, HAL_MPIF_4ACTL *p4actrl)
{
	MS_U32 spflag;	
	
	if(p4actrl == NULL)
		return HAL_MPIF_INVALID_PARAMETER;

	spin_lock_irqsave(&_spinlock_mpif, spflag); 
	_st4ACtrl[u8slaveid].bNotWaitMIUDone = p4actrl->bNotWaitMIUDone;
	_st4ACtrl[u8slaveid].u8granular = p4actrl->u8granular;
	_st4ACtrl[u8slaveid].u8MIUSel = p4actrl->u8MIUSel;
	_st4ACtrl[u8slaveid].u8retrx_limit = p4actrl->u8retrx_limit;
	_st4ACtrl[u8slaveid].u8Wcnt = p4actrl->u8Wcnt;
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
	
	return TRUE;
}

// Write data to the specific SPIF register via LC1A
HAL_MPIF_STATUS HAL_MPIF_LC1A_RW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U8 u8index, MS_U8 *pu8data)
{	
	MS_U32 spflag;	
	MS_U16 u16Conf = 0;	
	
	spin_lock_irqsave(&_spinlock_mpif, spflag);	

	// Waiting for the channel being free
    if(!_HAL_MPIF_BusyWait_ChannelFree(MPIF_EVENTBIT_1A, MAX_TIMEOUT))   
    {
    	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
        return HAL_MPIF_CHANNEL_BUSY;    
    }

	u16Conf = ((MS_U16)(bWite & 0x01) << 1) | ((MS_U16)(u8slaveid & 0x03) << 2) | ((MS_U16)(u8index & 0x07) << 4);
	if(bWite == TRUE)
		u16Conf |= ((MS_U16)(*pu8data & 0xFF) << 8);

	MPIF_WRITE(REG_MPIF_LC1A_SETTINGS, u16Conf);
	MPIF_WRITE(REG_MPIF_LC1A_SETTINGS, u16Conf | 0x01); //fired

	if(_HAL_MPIF_BusyWait_InterruptErrorEvent(MPIF_EVENTBIT_1A, 0, MAX_TIMEOUT) != MPIF_EVENT_TX_DONE)
	{
		spin_unlock_irqrestore(&_spinlock_mpif, spflag);
		return HAL_MPIF_FAIL;
	}
	
	if(!bWite)
		*pu8data = (MS_U8)((MPIF_READ(REG_MPIF_LC1A_SETTINGS) & 0xFF00) >> 8);	
			
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
	
	return HAL_MPIF_SUCCESS;
}

// Write data to the specific SPIF register via LC2A
HAL_MPIF_STATUS HAL_MPIF_LC2A_RW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U16 u16addr, MS_U16 *pu16data)
{
	MS_U32 spflag;	
	MS_U16 u16Conf = 0;
	
	spin_lock_irqsave(&_spinlock_mpif, spflag);
	
	// Waiting for the channel being free
    if(!_HAL_MPIF_BusyWait_ChannelFree(MPIF_EVENTBIT_2A, MAX_TIMEOUT))    
    {
    	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
        return HAL_MPIF_CHANNEL_BUSY;    
    }

    // Set the register address
    MPIF_WRITE(REG_MPIF_LC2A_REG_ADDRESS, u16addr); 
 
    // Put the data into the sending buffer
    if(bWite == TRUE)
        MPIF_WRITE(REG_MPIF_LC2A_REG_DATA, *pu16data);

    // Fire the read command, write control register
    u16Conf = ((MS_U16)(bWite & 0x01) << 1) | ((MS_U16)(u8slaveid & 0x03) << 2) | 
    	((MS_U16)(_st2XCtrl[u8slaveid].bchksum_en & 0x01) << 4) | ((MS_U16)(_st2XCtrl[u8slaveid].u8retrx & 0x03) << 6) | 0x01; //fired
    MPIF_WRITE(REG_MPIF_LC2A_REG_CONTROL, u16Conf); 	

	if(_HAL_MPIF_BusyWait_InterruptErrorEvent(MPIF_EVENTBIT_2A, MPIF_EVENTBIT_2A_ERR, MAX_TIMEOUT) != MPIF_EVENT_TX_DONE)
	{
		spin_unlock_irqrestore(&_spinlock_mpif, spflag);
		return HAL_MPIF_FAIL;
	}

	if(!bWite)
		*pu16data = MPIF_READ(REG_MPIF_LC2A_REG_DATA);
	
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
	
	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_LC2B_RW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U16 u16addr, MS_U16 *pu16data)
{
	MS_U32 spflag;	
	MS_U16 u16Conf = 0;
	
	spin_lock_irqsave(&_spinlock_mpif, spflag); 
	
	// Waiting for the channel being free	
    if(!_HAL_MPIF_BusyWait_ChannelFree(MPIF_EVENTBIT_2B, MAX_TIMEOUT))    
    {
    	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
        return HAL_MPIF_CHANNEL_BUSY;    
    }
	
    // Set the register address
    MPIF_WRITE(REG_MPIF_LC2B_REG_ADDRESS, u16addr); 
	
    if(bWite == TRUE)        
		MPIF_WRITE(REG_MPIF_LC2B_REG_DATA, *pu16data);

	// Fire the read command, write control register
    u16Conf = ((MS_U16)(bWite & 0x01) << 1) | ((MS_U16)(u8slaveid & 0x03) << 2) | 
    	((MS_U16)(_st2XCtrl[u8slaveid].bchksum_en & 0x01) << 4) | ((MS_U16)(_st2XCtrl[u8slaveid].u8retrx & 0x03) << 6) | 0x01; //fired
    MPIF_WRITE(REG_MPIF_LC2B_REG_CONTROL, u16Conf); 
	
	if(_HAL_MPIF_BusyWait_InterruptErrorEvent(MPIF_EVENTBIT_2B, MPIF_EVENTBIT_2B_ERR, MAX_TIMEOUT) != MPIF_EVENT_TX_DONE)
	{
		spin_unlock_irqrestore(&_spinlock_mpif, spflag);
		return HAL_MPIF_FAIL;
	}

	if(!bWite)
		*pu16data = MPIF_READ(REG_MPIF_LC2B_REG_DATA);

	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
	
	return HAL_MPIF_SUCCESS;
}

// Read data from the SPIF to the RIU via LC3A
// u8len is WORD data len
HAL_MPIF_STATUS HAL_MPIF_LC3A_RIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U8 u8len, MS_U8 *pu8data)
{  	
	MS_U32 spflag;	
	MS_U8 u8i;
	MS_U16 u16data = 0;    	
	MS_U8 *pu8tr = pu8data; 

	spin_lock_irqsave(&_spinlock_mpif, spflag);

	if(!_HAL_MPIF_BusyWait_ChannelFree(MPIF_EVENTBIT_3A, MAX_TIMEOUT))    
    {
    	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
        return HAL_MPIF_CHANNEL_BUSY;    
    }	
	
    // Set data length    
    MPIF_WRITE(REG_MPIF_LC3A_PACKET_LENGTH, 1); //unit is 16 bytes

	// Put the data into the sending buffer    
	if(bWite == TRUE)
	{
    	for (u8i=0; u8i<u8len; u8i++)    	
        	MPIF_REG8(REG_MPIF_LC3A_PACKET_DATA, u8i) = *pu8tr++;    	
		for (; u8i<16; u8i++)
			MPIF_REG8(REG_MPIF_LC3A_PACKET_DATA, u8i) = 0; 
	}

	u16data = ((MS_U16)(bWite & 0x01) << 1) | ((MS_U16)(u8slaveid & 0x03) << 2) | ((MS_U16)(_st3XCtrl[u8slaveid].bchksum_en & 0x01) << 4) | 
					((MS_U16)(_st3XCtrl[u8slaveid].bretrx_idx & 0x01) << 5) | ((MS_U16)(_st3XCtrl[u8slaveid].u8retrx & 0x03) << 6) |
					((MS_U16)(_st3XCtrl[u8slaveid].bfmode & 0x01) << 10) | ((MS_U16)(_st3XCtrl[u8slaveid].bNotWaitMIUDone & 0x01) << 11) |
					((MS_U16)(_st3XCtrl[u8slaveid].u8Wcnt & 0x0F) << 12);
	
	MPIF_WRITE(REG_MPIF_LC3A_PACKET_CONTROL, u16data);	
	MPIF_WRITE(REG_MPIF_LC3A_PACKET_CONTROL, u16data | 0x01);	//fired

    spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	if(_HAL_MPIF_WaitEventDone(MPIF_EVENTBIT_3A, MPIF_EVENTBIT_3A_ERR, MAX_TIMEOUT) != MPIF_EVENT_TX_DONE)
	{		
		return HAL_MPIF_FAIL;
	}
    
	if(!bWite)
	{
	    spin_lock_irqsave(&_spinlock_mpif, spflag);	
		for (u8i=0; u8i<u8len; u8i++)		
			*pu8tr++ = MPIF_REG8(REG_MPIF_LC3A_PACKET_DATA, u8i);	
        spin_unlock_irqrestore(&_spinlock_mpif, spflag);
	}
	
	return HAL_MPIF_SUCCESS;
}

// Read data from the SPIF to the RIU via LC3A
// u8len is WORD data len
HAL_MPIF_STATUS HAL_MPIF_LC3B_RIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U8 u8len, MS_U8 *pu8data)
{   
	MS_U32 spflag;	
	MS_U8 u8i;
	MS_U16 u16data = 0;    	
	MS_U8 *pu8tr = pu8data;    

	spin_lock_irqsave(&_spinlock_mpif, spflag);

	if(!_HAL_MPIF_BusyWait_ChannelFree(MPIF_EVENTBIT_3B, MAX_TIMEOUT))    
    {
    	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
        return HAL_MPIF_CHANNEL_BUSY;    
    }	
	
    // Set data length    
    MPIF_WRITE(REG_MPIF_LC3B_PACKET_LENGTH, 1); //unit is 16 bytes

    // Put the data into the sending buffer    
	if(bWite == TRUE)
	{
    	for (u8i=0; u8i<u8len; u8i++)    	
        	MPIF_REG8(REG_MPIF_LC3B_PACKET_DATA, u8i) = *pu8tr++;    	
		for (; u8i<16; u8i++)
			MPIF_REG8(REG_MPIF_LC3B_PACKET_DATA, u8i) = 0; 
	}

	u16data = ((MS_U16)(bWite & 0x01) << 1) | ((MS_U16)(u8slaveid & 0x03) << 2) | ((MS_U16)(_st3XCtrl[u8slaveid].bchksum_en & 0x01) << 4) | 
					((MS_U16)(_st3XCtrl[u8slaveid].bretrx_idx & 0x01) << 5) | ((MS_U16)(_st3XCtrl[u8slaveid].u8retrx & 0x03) << 6) |
					((MS_U16)(_st3XCtrl[u8slaveid].bfmode & 0x01) << 10) | ((MS_U16)(_st3XCtrl[u8slaveid].bNotWaitMIUDone & 0x01) << 11) |
					((MS_U16)(_st3XCtrl[u8slaveid].u8Wcnt & 0x0F) << 12); 

	MPIF_WRITE(REG_MPIF_LC3B_PACKET_CONTROL, u16data);	
	MPIF_WRITE(REG_MPIF_LC3B_PACKET_CONTROL, u16data | 0x01);	

    spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	if(_HAL_MPIF_WaitEventDone(MPIF_EVENTBIT_3B, MPIF_EVENTBIT_3B_ERR, MAX_TIMEOUT) != MPIF_EVENT_TX_DONE)
	{		
		return HAL_MPIF_FAIL;
	}

	if(!bWite)
	{
	    spin_lock_irqsave(&_spinlock_mpif, spflag);
		for (u8i=0; u8i<u8len; u8i++)    	
    		*pu8tr++ = MPIF_REG8(REG_MPIF_LC3B_PACKET_DATA, u8i);   
        spin_unlock_irqrestore(&_spinlock_mpif, spflag);
	}	
	
    return HAL_MPIF_SUCCESS;
}

// Read data from the SPIF to the MIU via LC3A
// u32len is number of bytes
HAL_MPIF_STATUS HAL_MPIF_LC3A_MIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U32 u32len, MS_U32 u32miu_addr)
{
	MS_U32 spflag;	
    MS_U16 u16data;   

	spin_lock_irqsave(&_spinlock_mpif, spflag);

	if(!_HAL_MPIF_BusyWait_ChannelFree(MPIF_EVENTBIT_3A, MAX_TIMEOUT))    
    {
    	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
        return HAL_MPIF_CHANNEL_BUSY;    
    }	
	
    // Set the MIU address as the destination address
    u16data = (MS_U16)((u32miu_addr >> MPIF_MIU_BUS) & 0xFFFF);
    MPIF_WRITE(REG_MPIF_LC3A_PACKET_MIU_BASE_ADDRESS_L, u16data);
    u16data = (MS_U16)(((u32miu_addr >> MPIF_MIU_BUS) >> 16) & 0xFFFF);
    MPIF_WRITE(REG_MPIF_LC3A_PACKET_MIU_BASE_ADDRESS_H, u16data);

    // Set data length
    MPIF_WRITE(REG_MPIF_LC3A_PACKET_LENGTH, (MS_U16)(u32len >> 4));

    // Fire the write command
    u16data = ((MS_U16)(bWite & 0x01) << 1) | ((MS_U16)(u8slaveid & 0x03) << 2) | ((MS_U16)(_st3XCtrl[u8slaveid].bchksum_en & 0x01) << 4) |
    				((MS_U16)(_st3XCtrl[u8slaveid].bretrx_idx & 0x01) << 5) | ((MS_U16)(_st3XCtrl[u8slaveid].u8retrx & 0x03) << 6) | 0x100 | 
    				((MS_U16)(_st3XCtrl[u8slaveid].u8MIUSel & 0x01) << 9) | ((MS_U16)(_st3XCtrl[u8slaveid].bfmode & 0x01) << 10) | 
    				((MS_U16)(_st3XCtrl[u8slaveid].bNotWaitMIUDone & 0x01) << 11) | ((MS_U16)(_st3XCtrl[u8slaveid].u8Wcnt & 0x0F) << 12);
    MPIF_WRITE(REG_MPIF_LC3A_PACKET_CONTROL, u16data);
	MPIF_WRITE(REG_MPIF_LC3A_PACKET_CONTROL, u16data | 0x01);

    spin_unlock_irqrestore(&_spinlock_mpif, spflag);
	
	if(_HAL_MPIF_WaitEventDone(MPIF_EVENTBIT_3A, MPIF_EVENTBIT_3A_ERR, MAX_TIMEOUT) != MPIF_EVENT_TX_DONE)
	{		
		return HAL_MPIF_FAIL;
	}	
	
    return HAL_MPIF_SUCCESS;
}// Read data from the SPIF to the MIU via LC3A

// Read data from the SPIF to the MIU via LC3A
// u32len is number of bytes
HAL_MPIF_STATUS HAL_MPIF_LC3B_MIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U32 u32len, MS_U32 u32miu_addr)
{
	MS_U32 spflag;	
    MS_U16 u16data;  	   	

	spin_lock_irqsave(&_spinlock_mpif, spflag); 	

	if(!_HAL_MPIF_BusyWait_ChannelFree(MPIF_EVENTBIT_3B, MAX_TIMEOUT))    
    {
    	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
        return HAL_MPIF_CHANNEL_BUSY;    
    }	
	
    // Set the MIU address as the destination address
    u16data = (MS_U16)((u32miu_addr >> MPIF_MIU_BUS) & 0xFFFF);
    MPIF_WRITE(REG_MPIF_LC3B_PACKET_MIU_BASE_ADDRESS_L, u16data);
    u16data = (MS_U16)(((u32miu_addr >> MPIF_MIU_BUS) >> 16) & 0xFFFF);
    MPIF_WRITE(REG_MPIF_LC3B_PACKET_MIU_BASE_ADDRESS_H, u16data);

    // Set data length
    MPIF_WRITE(REG_MPIF_LC3B_PACKET_LENGTH, (MS_U16)(u32len >> 4));

    // Fire the write command
    u16data = ((MS_U16)(bWite & 0x01) << 1) | ((MS_U16)(u8slaveid & 0x03) << 2) | ((MS_U16)(_st3XCtrl[u8slaveid].bchksum_en) << 4) |
    				((MS_U16)(_st3XCtrl[u8slaveid].bretrx_idx & 0x01) << 5) | ((MS_U16)(_st3XCtrl[u8slaveid].u8retrx & 0x03) << 6) | 0x100 | 
    				((MS_U16)(_st3XCtrl[u8slaveid].u8MIUSel & 0x01) << 9) | ((MS_U16)(_st3XCtrl[u8slaveid].bfmode & 0x01) << 10) | 
    				((MS_U16)(_st3XCtrl[u8slaveid].bNotWaitMIUDone & 0x01) << 11) | ((MS_U16)(_st3XCtrl[u8slaveid].u8Wcnt & 0x0F) << 12);
    MPIF_WRITE(REG_MPIF_LC3B_PACKET_CONTROL, u16data);
	MPIF_WRITE(REG_MPIF_LC3B_PACKET_CONTROL, u16data | 0x01); 

    spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	if(_HAL_MPIF_WaitEventDone(MPIF_EVENTBIT_3B, MPIF_EVENTBIT_3B_ERR, MAX_TIMEOUT) != MPIF_EVENT_TX_DONE)
	{		
		return HAL_MPIF_FAIL;
	}
	
    return HAL_MPIF_SUCCESS;
}// Read data from the SPIF to the MIU via LC3A

// Read data from the SPIF to the MIU via LC4A
HAL_MPIF_STATUS HAL_MPIF_LC4A_MIURW(MS_BOOL bWite, MS_U8 u8slaveid, MS_U16 u16len, MS_U32 u32miu_addr)
{
	MS_U32 spflag;	
    MS_U16 u16data;       

	spin_lock_irqsave(&_spinlock_mpif, spflag);

	if(!_HAL_MPIF_BusyWait_ChannelFree(MPIF_EVENTBIT_4A, MAX_TIMEOUT))    
    {
    	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
        return HAL_MPIF_CHANNEL_BUSY;    
    }
	
    // Set the MIU address as the source address
    u16data = (u32miu_addr >> MPIF_MIU_BUS) & 0xFFFF;
    MPIF_WRITE(REG_MPIF_LC4A_STREAM_MIU_BASE_ADDRESS_L, u16data);
    u16data = ((u32miu_addr >> MPIF_MIU_BUS) >> 16) & 0xFFFF;
    MPIF_WRITE(REG_MPIF_LC4A_STREAM_MIU_BASE_ADDRESS_H, u16data);

    // Set data length
    MPIF_WRITE(REG_MPIF_LC4A_STREAM_LENGTH, u16len);

    // Fire the read command, write control register
    u16data = ((MS_U16)(bWite & 0x01) << 1) | ((MS_U16)(u8slaveid & 0x03) << 2) | ((MS_U16)(_st4ACtrl[u8slaveid].u8retrx_limit & 0x03) << 6) |
    				((MS_U16)(_st4ACtrl[u8slaveid].bNotWaitMIUDone & 0x01) << 8) | ((MS_U16)(_st4ACtrl[u8slaveid].u8MIUSel & 0x01) << 9) | 
    				((MS_U16)(_st4ACtrl[u8slaveid].u8granular & 0x03) << 10) | ((MS_U16)(_st4ACtrl[u8slaveid].u8Wcnt & 0x0F) << 12);

	MPIF_WRITE(REG_MPIF_LC4A_STREAM_CONTROL, u16data);
	MPIF_WRITE(REG_MPIF_LC4A_STREAM_CONTROL, u16data | 0x01);

    spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	if(_HAL_MPIF_WaitEventDone(MPIF_EVENTBIT_4A, MPIF_EVENTBIT_4A_ERR, MAX_TIMEOUT) != MPIF_EVENT_TX_DONE)
	{		
		return HAL_MPIF_FAIL;
	}
	
    return HAL_MPIF_SUCCESS;
}

//-------------------------------------------------------------
HAL_MPIF_STATUS HAL_MPIF_Configure(HAL_MPIF_CONFIG *pmpifconf)
{	
	MS_U32 spflag;	
	
    DEBUG_MPIF(MPIF_DBGLV_DEBUG, printk("%s(0x%x)\n", __FUNCTION__, (unsigned int)pmpifconf));

	spin_lock_irqsave(&_spinlock_mpif, spflag); 		
    CHIPTOP_WRITE_MASK(REG_ALLPAD_IN, BITS(15:15, 0), BMASK(15:15)); //Disable all pad in    
	CHIPTOP_WRITE_MASK(REG_MPIF_MODE, BITS(11:10, 3), BMASK(11:10)); //set 4 bit mode of pad to support all modes 	

	HAL_MPIF_Set_Clock(pmpifconf->u8clk);	
	
    HAL_MPIF_SET_1A2XTRCycle(pmpifconf->u8trc_1A2X);
	HAL_MPIF_SET_3X4ATRCycle(pmpifconf->u8trc_3X4A);
	HAL_MPIF_SET_WaitCycle(pmpifconf->u8wc);   

	HAL_MPIF_SET_SlaveDataWidth(0, pmpifconf->u8Slave0Dw);
	HAL_MPIF_SET_SlaveDataWidth(1, pmpifconf->u8Slave1Dw);
	HAL_MPIF_SET_SlaveDataWidth(2, pmpifconf->u8Slave2Dw);
	HAL_MPIF_SET_SlaveDataWidth(3, pmpifconf->u8Slave3Dw);

	HAL_MPIF_CmdDataWidth(pmpifconf->u8CmdDw);	

	spin_unlock_irqrestore(&_spinlock_mpif, spflag);	

	return HAL_MPIF_SUCCESS;
	
}

HAL_MPIF_STATUS HAL_MPIF_Set_Clock(MS_U8 u8clk)
{
	MS_U32 spflag;	
	
	spin_lock_irqsave(&_spinlock_mpif, spflag); 	
	CLKGEN0_WRITE_MASK(REG_CKG_MPIF, BITS(5:0, ((MS_U16)(u8clk & 0xFF))), BMASK(5:0));		
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_CLK_Disable(MS_BOOL bDisabled)
{
	MS_U32 spflag;	
	
	spin_lock_irqsave(&_spinlock_mpif, spflag); 
	CLKGEN0_WRITE_MASK(REG_CKG_MPIF, BITS(0:0, (MS_U8)bDisabled), BMASK(0:0));
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_SET_1A2XTRCycle(MS_U8 u8trc)
{
	MS_U32 spflag;	
	MS_U16 u16data;

	spin_lock_irqsave(&_spinlock_mpif, spflag); 
	u16data = ((MS_U16)u8trc) & 0x03;	
	MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(3:2, u16data) , BMASK(3:2));	
	
	u16data = (((MS_U16)u8trc) & 0x04) >> 2;	
	MPIF_WRITE_MASK(REG_MPIF_MISC2, BITS(3:3, u16data), BMASK(3:3));	
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);	

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_SET_3X4ATRCycle(MS_U8 u8trc)
{
	MS_U32 spflag;	
	MS_U16 u16data;

	spin_lock_irqsave(&_spinlock_mpif, spflag);
	u16data = (((MS_U16)u8trc) & 0x07);
	MPIF_WRITE_MASK(REG_MPIF_MISC2, BITS(2:0, u16data), BMASK(2:0));
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_SET_WaitCycle(MS_U8 u8wc)
{	
	MS_U32 spflag;	
	MS_U16 u16data = (MS_U16)u8wc;

	spin_lock_irqsave(&_spinlock_mpif, spflag);
	MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(5:4, u16data), BMASK(5:4));	
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_SET_SlaveDataWidth(MS_U8 u8slaveid, MS_U8 u8dw)
{
	MS_U32 spflag;	
	MS_U16 u16data = (MS_U16)u8dw;

	spin_lock_irqsave(&_spinlock_mpif, spflag);
	switch(u8slaveid)
	{
		case MPIF_SLAVE_ID_0:
			MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(7:6, u16data), BMASK(7:6));			
			break;
		case MPIF_SLAVE_ID_1:	
			MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(9:8, u16data), BMASK(9:8));			
			break;
		case MPIF_SLAVE_ID_2:	
			MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(11:10, u16data), BMASK(11:10));			
			break;
		case MPIF_SLAVE_ID_3:	
			MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(13:12, u16data), BMASK(13:12));			
			break;
	}
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_CmdDataWidth(MS_U8 u8dw)
{
	MS_U32 spflag;	
	
	spin_lock_irqsave(&_spinlock_mpif, spflag);
	CHIPTOP_WRITE_MASK(REG_MPIF_MODE, BITS(10:11, (u8dw+1)), BMASK(10:11)); //set 1 bit mode as default 
	MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(15:14, ((MS_U16)(u8dw&0xFF))), BMASK(15:14));	
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_SWReset(void)
{
	MS_U32 spflag;	
	
	spin_lock_irqsave(&_spinlock_mpif, spflag);
	MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(0:0, 0), BMASK(0:0));
	MPIF_WRITE_MASK(REG_MPIF_MISC1, BITS(0:0, 1), BMASK(0:0));
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);

	return HAL_MPIF_SUCCESS;
}

HAL_MPIF_STATUS HAL_MPIF_SetCmdDataMode(MS_U8 u8slaveid, MS_U8 u8cmdwidth, MS_U8 u8datawidth)
{
    MS_U16 u16data;
	MS_U32 spflag;	
	
	spin_lock_irqsave(&_spinlock_mpif, spflag);
    u16data = MPIF_READ(REG_MPIF_MISC1);
    switch(u8slaveid & 0x03)
    {
        case MPIF_SLAVE_ID_0:
            u16data &= ~0xC0;
            u16data |= ((MS_U16)u8datawidth << 6);
            break;
        case MPIF_SLAVE_ID_1:
            u16data &= ~0x300;
            u16data |= ((MS_U16)u8datawidth << 8);
            break;
        case MPIF_SLAVE_ID_2:
            u16data &= ~0xC00;
            u16data |= ((MS_U16)u8datawidth << 10);
            break;
        case MPIF_SLAVE_ID_3:
            u16data &= ~0x3000;
            u16data |= ((MS_U16)u8datawidth << 12);
            break;
    }
    u16data &= ~0xC000;
    u16data |= ((MS_U16)u8cmdwidth << 14);
    MPIF_WRITE(REG_MPIF_MISC1, u16data);
	spin_unlock_irqrestore(&_spinlock_mpif, spflag);
   
    return HAL_MPIF_SUCCESS;
}




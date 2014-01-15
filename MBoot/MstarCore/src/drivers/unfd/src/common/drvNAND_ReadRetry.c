//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>

#include <linux/string.h>
#include "../../inc/common/drvNAND.h"
#include "../../inc/common/drvNAND_utl.h"


//========================================================
// HAL misc. function definitions
//========================================================
#if NC_SEL_FCIE3
U32  NC_WaitComplete(U16 u16_WaitEvent, U32 u32_MicroSec);

  #define OPTYPE_ERASE        1
  #define OPTYPE_WRITE        2
U32 NC_Toshiba_ReadRetrySequence(U8 u8_RetryIndex, U8 u8_SetToDefault);
U32 NC_Samsung_ReadRetrySequence(U8 u8_RetryIndex, U8 u8_SetToDefault);
U32 NC_Micron_ReadRetrySequence(U8 u8_RetryIndex, U8 u8_SetToDefault);
U32 NC_SetReadRetryRegValue(U8 u8_RetryIndex, U8 u8_SetToDefault);


U32 NC_SendCustCmd
(
    U8* pu8_CmdType,
    U8* pu8_CmdValue,
    U32* pu32_AddrRow,
    U32* pu32_AddrCol,
    U8 *pu8_DataBuf, U8 *pu8_SpareBuf,
    U8 u8_CmdCount, U8 u8_CacheOp, U8 u8_plane, U32 u32_PageCnt
);

U32 NC_SendReadRetryCmd(U8 u8_RetryIndex, U8 u8_SetToDefault)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	if (pNandDrv->u8_CellType == 1 && pNandDrv->u8_ReadRetryLoc != 0)
	{
		//choose read retry function
		//FIXME
	//	return NC_SetReadRetryRegValue(u8_RetryIndex, u8_SetToDefault);
		return UNFD_ST_SUCCESS;
	}
	else if(pNandDrv->u8_CellType == 1 && pNandDrv->au8_ID[0] == 0x98)
	{
		return NC_Toshiba_ReadRetrySequence(u8_RetryIndex, u8_SetToDefault);
	}
	else if(pNandDrv->u8_CellType == 1 && pNandDrv->au8_ID[0] == 0xEC)
	{
		return NC_Samsung_ReadRetrySequence(u8_RetryIndex, u8_SetToDefault);
	}
	else if(pNandDrv->u8_CellType == 1 && pNandDrv->au8_ID[0] == 0x2C)
	{
		return NC_Micron_ReadRetrySequence(u8_RetryIndex, u8_SetToDefault);
	}
	else
		return UNFD_ST_SUCCESS;
}

U8 au8_TSBReadRetryValue[TSB_READRETRY_TIME] = {
    0x00, 0x04, 0x7C, 0x78, 0x74, 0x08
};

U8 au8_TSBReadRetryRegister[TSB_REGISTER_NUMBER] = {
    0x4, 0x5, 0x6, 0x7
};

U32 NC_Toshiba_ReadRetrySequence(U8 u8_RetryIndex, U8 u8_SetToDefault)
{
	U8  u8_idx;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	if(u8_SetToDefault)
	{
		return NC_ResetNandFlash();
	}

	REG_WRITE_UINT16(NC_MIE_EVENT, BIT_NC_JOB_END|BIT_MMA_DATA_END);
	NC_ResetFCIE();
	NC_Config();
	nand_clock_setting(NFIE_CLK_300K);
	REG_WRITE_UINT16(NC_WIDTH, 0);
	REG_CLR_BITS_UINT16(NC_LATCH_DATA, BIT0|BIT1|BIT2);

	if(u8_RetryIndex == 0)
	{
	//send pre condition
		REG_WRITE_UINT16(NC_AUXREG_ADR, 0x08);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 0xC5<<8 | 0x5C);

		REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_INSTQUE);
		REG_WRITE_UINT16(NC_AUXREG_DAT, (CMD_REG8H << 8) | CMD_REG8L);
		REG_WRITE_UINT16(NC_AUXREG_DAT, (ACT_BREAK << 8) | ACT_BREAK);

		REG_WRITE_UINT16(NC_CTRL, BIT_NC_JOB_START);
		if (NC_WaitComplete(BIT_NC_JOB_END, WAIT_WRITE_TIME) == WAIT_WRITE_TIME)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Error: Pre Condition Timeout, ErrCode:%Xh \r\n", UNFD_ST_ERR_W_TIMEOUT);
			#if 0==IF_IP_VERIFY
			NC_ResetFCIE();
			NC_Config();
			NC_CLR_DDR_MODE();
			NC_ResetNandFlash();
			#else
			//nand_stop();
			#endif
			NC_CLR_DDR_MODE();
			return UNFD_ST_ERR_W_TIMEOUT; // timeout
		}
	}


	REG_WRITE_UINT16(NC_AUXREG_ADR, 0x08);
	REG_WRITE_UINT16(NC_AUXREG_DAT, 0x55 << 8 | 0x55);

	for(u8_idx = 0; u8_idx < TSB_REGISTER_NUMBER; u8_idx ++)
	{

		REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_ADRSET);
		REG_WRITE_UINT16(NC_AUXREG_DAT, au8_TSBReadRetryRegister[u8_idx]);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 0);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 0);

		REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_INSTQUE);
		REG_WRITE_UINT16(NC_AUXREG_DAT, ((OP_ADR_CYCLE_00|OP_ADR_TYPE_COL|OP_ADR_SET_0) << 8) | (CMD_REG8L));
		REG_WRITE_UINT16(NC_AUXREG_DAT, (ACT_BREAK << 8) | ACT_RAN_DOUT);

		NC_SetCIFD(&au8_TSBReadRetryValue[u8_RetryIndex], 0, 1);

		REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_RAN_CNT);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 1);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 0); // offset
		REG_WRITE_UINT16(NC_CTRL, BIT_NC_JOB_START|BIT_NC_CIFD_ACCESS);
		if (NC_WaitComplete(BIT_NC_JOB_END, WAIT_WRITE_TIME) == WAIT_WRITE_TIME)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Error: Timeout RetryIdx %d, ErrCode:%Xh \r\n", u8_RetryIndex, UNFD_ST_ERR_W_TIMEOUT);
			#if 0==IF_IP_VERIFY
			NC_ResetFCIE();
			NC_Config();
			NC_CLR_DDR_MODE();
			NC_ResetNandFlash();
			#else
			//nand_stop();
			#endif
			NC_CLR_DDR_MODE();
			return UNFD_ST_ERR_W_TIMEOUT; // timeout
		}
	}

	REG_WRITE_UINT16(NC_AUXREG_ADR, 0x08);
	REG_WRITE_UINT16(NC_AUXREG_DAT, 0x5D << 8 | 0x26);

	REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_INSTQUE);
	REG_WRITE_UINT16(NC_AUXREG_DAT, (CMD_REG8H << 8) | (CMD_REG8L));
	REG_WRITE_UINT16(NC_AUXREG_DAT, (ACT_BREAK << 8) | ACT_BREAK);
	REG_WRITE_UINT16(NC_CTRL, BIT_NC_JOB_START);

	if (NC_WaitComplete(BIT_NC_JOB_END, WAIT_WRITE_TIME) == WAIT_WRITE_TIME)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Error: Timeout RetryIdx %d, ErrCode:%Xh \r\n", u8_RetryIndex, UNFD_ST_ERR_W_TIMEOUT);
		#if 0==IF_IP_VERIFY
		NC_ResetFCIE();
		NC_Config();
		NC_CLR_DDR_MODE();
		NC_ResetNandFlash();
		#else
		//nand_stop();
		#endif
		NC_CLR_DDR_MODE();
		return UNFD_ST_ERR_W_TIMEOUT; // timeout
	}
	nand_clock_setting(FCIE3_SW_DEFAULT_CLK);
	REG_WRITE_UINT16(NC_WIDTH, FCIE_REG41_VAL);
	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);

	return UNFD_ST_SUCCESS;
}


U8 au8_SSReadRetryRegister[SAMSUNG_REGISTER_NUMBER] = {
	0xA7, 0xA4, 0xA5, 0xA6
};

U8 au8_SSReadRetryValue[SAMSUNG_READRETRY_TIME][SAMSUNG_REGISTER_NUMBER] = {
		{0x00, 0x00, 0x00, 0x00},
		{0x05, 0x0A, 0x00, 0x00},
		{0x28, 0x00, 0xEC, 0xD8},
		{0xED, 0xF5, 0xED, 0xE6},
		{0x0A, 0x0F, 0x05, 0x00},
		{0x0F, 0x0A, 0xFB, 0xEC},
		{0xE8, 0xEF, 0xE8, 0xDC},
		{0xF1, 0xFB, 0xFE, 0xF0},
		{0x0A, 0x00, 0xFB, 0xEC},
		{0xD0, 0xE2, 0xD0, 0xC2},
		{0x14, 0x0F, 0xFB, 0xEC},
		{0xE8, 0xFB, 0xE8, 0xDC},
		{0x1E, 0x14, 0xFB, 0xEC},
		{0xFB, 0xFF, 0xFB, 0xF8},
		{0x07, 0x0C, 0x02, 0x00}
};

U32 NC_Samsung_ReadRetrySequence(U8 u8_RetryIndex, U8 u8_SetToDefault)
{
	U8  u8_idx, u8_ValueIdx;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	if(u8_SetToDefault)
	{
		u8_ValueIdx = 0;
	}
	else
	{
		u8_ValueIdx = u8_RetryIndex + 1;
	}

	REG_WRITE_UINT16(NC_MIE_EVENT, BIT_NC_JOB_END|BIT_MMA_DATA_END);
	NC_ResetFCIE();
	NC_Config();
	nand_clock_setting(NFIE_CLK_300K);
	REG_WRITE_UINT16(NC_WIDTH, 0);
	REG_CLR_BITS_UINT16(NC_LATCH_DATA, BIT0|BIT1|BIT2);

	REG_WRITE_UINT16(NC_AUXREG_ADR, 0x08);
	REG_WRITE_UINT16(NC_AUXREG_DAT, 0xA1 << 8 | 0xA1);

	for(u8_idx = 0; u8_idx < SAMSUNG_REGISTER_NUMBER; u8_idx ++)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Register %d \r\n",u8_idx);

		REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_ADRSET);
		REG_WRITE_UINT16(NC_AUXREG_DAT, au8_SSReadRetryRegister[u8_idx]);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 0);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 0);

		REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_INSTQUE);
		REG_WRITE_UINT16(NC_AUXREG_DAT, ((OP_ADR_CYCLE_01|OP_ADR_TYPE_COL|OP_ADR_SET_0) << 8) | (CMD_REG8L));
		REG_WRITE_UINT16(NC_AUXREG_DAT, (ACT_BREAK << 8) | ACT_RAN_DOUT);

		NC_SetCIFD(&au8_SSReadRetryValue[u8_ValueIdx][u8_idx], 0, 1);

		REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_RAN_CNT);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 1);
		REG_WRITE_UINT16(NC_AUXREG_DAT, 0); // offset
		REG_WRITE_UINT16(NC_CTRL, BIT_NC_JOB_START|BIT_NC_CIFD_ACCESS);
		if (NC_WaitComplete(BIT_NC_JOB_END, WAIT_WRITE_TIME) == WAIT_WRITE_TIME)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Error: Timeout RetryIdx %d, ErrCode:%Xh \r\n", u8_RetryIndex, UNFD_ST_ERR_W_TIMEOUT);
			#if 0==IF_IP_VERIFY
			NC_ResetFCIE();
			NC_Config();
			NC_CLR_DDR_MODE();
			NC_ResetNandFlash();
			#else
			//nand_stop();
			#endif
			NC_CLR_DDR_MODE();
			return UNFD_ST_ERR_W_TIMEOUT; // timeout
		}
	}
	nand_clock_setting(FCIE3_SW_DEFAULT_CLK);
	REG_WRITE_UINT16(NC_WIDTH, FCIE_REG41_VAL);
	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);

	return UNFD_ST_SUCCESS;
}

U8 au8_MicronReadRetryValue[MCIRON_READRETRY_TIME][MICRON_REIGSTER_NUMBER] ={
	{0,0,0,0},
	{1,0,0,0},
	{2,0,0,0},
	{3,0,0,0}
};

U32 NC_Micron_ReadRetrySequence(U8 u8_RetryIndex, U8 u8_SetToDefault)
{
	U8   u8_ValueIdx;
	volatile U16 u16_Reg;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	pNandDrv = pNandDrv;	//fix warning

	if(u8_SetToDefault)
	{
		u8_ValueIdx = 0;
	}
	else
	{
		u8_ValueIdx = u8_RetryIndex + 1;
	}


	// data go through CIFD
	#if IF_SPARE_AREA_DMA
	REG_WRITE_UINT16(NC_SPARE, pNandDrv->u16_Reg48_Spare &
		~(BIT_NC_SPARE_DEST_MIU|BIT_NC_RANDOM_RW_OP_EN));
	#endif
	
	REG_WRITE_UINT16(NC_MIE_EVENT, BIT_NC_JOB_END|BIT_MMA_DATA_END);
	NC_ResetFCIE();
	NC_Config();

	NC_SET_DDR_MODE();

	REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_CMDREG8);
	REG_WRITE_UINT16(NC_AUXREG_DAT, 0xEF);

	REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_ADRSET);
	REG_WRITE_UINT16(NC_AUXREG_DAT, (0x89<<8)|0x89);

	REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_INSTQUE);
	REG_WRITE_UINT16(NC_AUXREG_DAT, (ADR_C2T1S0 << 8) | CMD_REG8L);
	REG_WRITE_UINT16(NC_AUXREG_DAT, (ACT_WAITRB << 8) | ACT_RAN_DOUT);
	REG_WRITE_UINT16(NC_AUXREG_DAT, ACT_BREAK);

	NC_SetCIFD(&au8_MicronReadRetryValue[u8_ValueIdx][0], 0, MICRON_REIGSTER_NUMBER);

	REG_WRITE_UINT16(NC_AUXREG_ADR, AUXADR_RAN_CNT);
	REG_WRITE_UINT16(NC_AUXREG_DAT, MICRON_REIGSTER_NUMBER);
	REG_WRITE_UINT16(NC_AUXREG_DAT, 0);	/*offset 0*/

	REG_WRITE_UINT16(NC_CTRL, BIT_NC_CIFD_ACCESS|BIT_NC_JOB_START|BIT_NC_IF_DIR_W);

	if (NC_WaitComplete(BIT_NC_JOB_END, DELAY_100ms_in_us) == DELAY_100ms_in_us)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Error! timeout!\r\n");

		#if 0==IF_IP_VERIFY
		REG_READ_UINT16(NC_MIE_EVENT, u16_Reg);
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NC_MIE_EVENT: %Xh \r\n", u16_Reg);
		REG_READ_UINT16(NC_CTRL, u16_Reg);
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NC_CTRL: %Xh \r\n", u16_Reg);

		NC_ResetFCIE();
		NC_Config();
		NC_CLR_DDR_MODE();
		NC_ResetNandFlash();
		#else
		nand_stop();
		#endif
		return UNFD_ST_ERR_SET_FEATURE_TIMEOUT;
	}


	#if IF_SPARE_AREA_DMA
	REG_WRITE_UINT16(NC_SPARE, pNandDrv->u16_Reg48_Spare);
	#endif

	NC_CLR_DDR_MODE();
	return UNFD_ST_SUCCESS;	// ok
}


U32 NC_GetReadRetryRegValue(U8** ppu8_RegisterValue)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U16 u16_i, u16_j = 0, u16_k = 0;
	U32 au32_AddrRow[4];
	U32 au32_AddrCol[4];
	U32 u32_Err = 0;
	
	for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_CustRegNo; u16_i ++)
	{
		u16_k = 0;
		for(u16_j = 0; u16_j < pNandDrv->ReadRetry_t.u8_GetCmdLen; u16_j ++)
			if(pNandDrv->ReadRetry_t.pu8_GetCmdTypeSeq[u16_j] == ADDR_CYCLE_CMD)
			{
				au32_AddrRow[u16_k] = 0;
				au32_AddrCol[u16_k] = pNandDrv->ReadRetry_t.pu8_CustRegTable[u16_i];
				u16_k ++;
			}
		
		u32_Err = NC_SendCustCmd(pNandDrv->ReadRetry_t.pu8_GetCmdTypeSeq, pNandDrv->ReadRetry_t.pu8_GetCmdValueSeq, au32_AddrRow, au32_AddrCol,
			pNandDrv->PlatCtx_t.pu8_PageDataBuf, pNandDrv->PlatCtx_t.pu8_PageSpareBuf, pNandDrv->ReadRetry_t.u8_GetCmdLen, 0, 1, 1);
		
		
		for(u16_j = 0; u16_j < pNandDrv->ReadRetry_t.u8_ByteLenPerCmd; u16_j ++)
			ppu8_RegisterValue[u16_i][u16_j] = pNandDrv->PlatCtx_t.pu8_PageDataBuf[u16_j];
	}
	return u32_Err;
}

U32 NC_SetReadRetryRegValue(U8 u8_RetryIndex, U8 u8_SetToDefault)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U16 u16_i, u16_j, u16_k;
	U32 u32_Err = 0;
	U32 au32_AddrRow[4];
	U32 au32_AddrCol[4];
	U8** ppu8_RegValue;
	
	if(u8_RetryIndex >= pNandDrv->ReadRetry_t.u8_MaxRetryTime)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Retry Index should be smaller than MaxRetryTime\n");
		return UNFD_ST_ERR_INVALID_ADDR;
	}
	ppu8_RegValue = (U8**)malloc(pNandDrv->ReadRetry_t.u8_CustRegNo * sizeof(U8*) +
						pNandDrv->ReadRetry_t.u8_CustRegNo * (pNandDrv->ReadRetry_t.u8_ByteLenPerCmd + 1) *sizeof(U8));
	
	if(!ppu8_RegValue)
		return UNFD_ST_ERR_INVALID_ADDR;
	
	for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_CustRegNo; u16_i ++)
		ppu8_RegValue[u16_i] = ((U8*)(ppu8_RegValue + pNandDrv->ReadRetry_t.u8_CustRegNo)
	    								+ u16_i * (pNandDrv->ReadRetry_t.u8_ByteLenPerCmd + 1) *sizeof(U8));

	if(pNandDrv->ReadRetry_t.u8_DefaultValueOffset)
	{
		NC_GetReadRetryRegValue(ppu8_RegValue);
	}
	for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_CustRegNo; u16_i ++)
	{
		u16_k = 0;
		for(u16_j = 0; u16_j < pNandDrv->ReadRetry_t.u8_GetCmdLen; u16_j ++)
			if(pNandDrv->ReadRetry_t.pu8_GetCmdTypeSeq[u16_j] == ADDR_CYCLE_CMD)
			{
				au32_AddrRow[u16_k] = 0;
				au32_AddrCol[u16_k] = pNandDrv->ReadRetry_t.pu8_CustRegTable[u16_i];
				u16_k ++;
			}
		u16_j = 0;
		if(u16_k == 0)
		{
			pNandDrv->PlatCtx_t.pu8_PageDataBuf[u16_j] = 0;
			pNandDrv->PlatCtx_t.pu8_PageDataBuf[u16_j + 1] = pNandDrv->ReadRetry_t.pu8_CustRegTable[u16_i];
			u16_j +=2;
		}
		
		for(u16_k = 0; u16_k < pNandDrv->ReadRetry_t.u8_ByteLenPerCmd; u16_k ++)
		{
			if(!u8_SetToDefault)
			{
				if(pNandDrv->ReadRetry_t.u8_DefaultValueOffset)
				{
					if(pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u8_RetryIndex][u16_i][0] == 0xFF)
					{
						pNandDrv->PlatCtx_t.pu8_PageDataBuf[u16_j] =
							pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u8_RetryIndex][u16_i][u16_k + 1];
						u16_j ++;
					}
					else if(pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u8_RetryIndex][u16_i][0] == 0)
					{
						pNandDrv->PlatCtx_t.pu8_PageDataBuf[u16_j] = ppu8_RegValue[u16_i][u16_k] +
							pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u8_RetryIndex][u16_i][u16_k + 1];
						u16_j ++;
					}
					else if(pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u8_RetryIndex][u16_i][0] == 1)
					{
						pNandDrv->PlatCtx_t.pu8_PageDataBuf[u16_j] = ppu8_RegValue[u16_i][u16_k] -
						    pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u8_RetryIndex][u16_i][u16_k + 1];
						u16_j ++;
					}
					else
					{
						free(ppu8_RegValue);
						return UNFD_ST_ERR_INVALID_PARAM;
					}
				}
				else
				{
					if(pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u8_RetryIndex][u16_i][0] == 0xFF)
					{
						pNandDrv->PlatCtx_t.pu8_PageDataBuf[u16_j] = pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u8_RetryIndex][u16_i][u16_k + 1];
						u16_j ++;
					}
					else
					{
						free(ppu8_RegValue);
						return UNFD_ST_ERR_INVALID_PARAM;
					}
				}
			}
			else
			{
				pNandDrv->PlatCtx_t.pu8_PageDataBuf[u16_j] = pNandDrv->ppu8_ReadRetryDefault[u16_i][u16_k];
				u16_j ++;
			}
		}
		u32_Err = NC_SendCustCmd(pNandDrv->ReadRetry_t.pu8_SetCmdTypeSeq, pNandDrv->ReadRetry_t.pu8_SetCmdValueSeq, au32_AddrRow, au32_AddrCol,
			pNandDrv->PlatCtx_t.pu8_PageDataBuf, pNandDrv->PlatCtx_t.pu8_PageSpareBuf, pNandDrv->ReadRetry_t.u8_SetCmdLen, 0, 1, 1);
	}
	
	free(ppu8_RegValue);
	return UNFD_ST_SUCCESS;
}

#endif // NC_SEL_FCIE3

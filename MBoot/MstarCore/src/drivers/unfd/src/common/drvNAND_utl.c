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


typedef struct {
	const char *msg;
	U8  u8_OpCode_RW_AdrCycle;
	U8  u8_OpCode_Erase_AdrCycle;
	U16 u16_Reg48_Spare;
} drvNAND_ReadSeq;

static const drvNAND_ReadSeq seq[] = {
	{"R_SEQUENCE_003A", ADR_C3TFS0, ADR_C2TRS0, BIT_NC_ONE_COL_ADDR},
	{"R_SEQUENCE_004A", ADR_C4TFS0,	ADR_C3TRS0,	BIT_NC_ONE_COL_ADDR},
	{"R_SEQUENCE_004A30", ADR_C4TFS0, ADR_C2TRS0, 0},
	{"R_SEQUENCE_005A30", ADR_C5TFS0, ADR_C3TRS0, 0},
	{"R_SEQUENCE_006A30", ADR_C6TFS0, ADR_C3TRS0, 0}, // FIXME
	{NULL, 0, 0, 0},
};

U32 drvNAND_ProbeReadSeq(void)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	U8 u8_seq_idx = 0;
	U32 u32_Err;
	drvNAND_ReadSeq const *pSeq = seq;

	pNandDrv->u16_BlkCnt = 0x400;
	pNandDrv->u16_BlkPageCnt = 0x40;
	pNandDrv->u16_PageByteCnt = 0x200;
	pNandDrv->u16_SpareByteCnt = 0x10;
    pNandDrv->u16_ECCType = ECC_TYPE_4BIT;
    pNandDrv->u8_Flag_004A30 = 0;

	NC_ConfigContext();
	u32_Err = NC_Init();
	if (u32_Err != UNFD_ST_SUCCESS)
		return u32_Err;

	if(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_TOGGLE)
	{
		pSeq +=3;
		u8_seq_idx += 3;
	}
	
	/* Probe read sequence */
	while (pSeq->msg) {

		nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "%s\n", pSeq->msg);
		pNandDrv->u8_OpCode_RW_AdrCycle = pSeq->u8_OpCode_RW_AdrCycle;
		pNandDrv->u8_OpCode_Erase_AdrCycle = pSeq->u8_OpCode_Erase_AdrCycle;
		pNandDrv->u16_Reg48_Spare &= ~BIT_NC_ONE_COL_ADDR;
		pNandDrv->u16_Reg48_Spare |= pSeq->u16_Reg48_Spare;

        NC_Config();

		u32_Err = NC_ProbeReadSeq();
		if (u32_Err == UNFD_ST_SUCCESS)
        {
            // if R_SEQUENCE_004A30
            if(ADR_C4TFS0 == pNandDrv->u8_OpCode_RW_AdrCycle &&
               0 == (pNandDrv->u16_Reg48_Spare & BIT_NC_ONE_COL_ADDR))
            {
                pNandDrv->u8_Flag_004A30 = 1;
            }
			pNandDrv->u8_AddrCycleIdx = u8_seq_idx;
			return UNFD_ST_SUCCESS;
        }
		NC_ResetFCIE(); // <- should be redundant code (wait for test then remove)
		pSeq++;
		u8_seq_idx ++;
	}

	nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "ERROR: Unknown RSEQ\n");

	return UNFD_ST_ERR_UNKNOWN_RSEQ;
}

U32 drvNAND_CheckPartInfo(U32 u32_PageIdx)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
#if NAND_BUF_USE_STACK
	U8 au8_SectorBuf[1024];
	U8 au8_SpareBuf[64];
#else
	U8 *au8_SectorBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
#endif
	U32 u32_Err;
	U32 u32_ChkSum;
	U32 u32_ByteCnt;
	#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
	int s32ECCStatus;
	#endif

	PARTITION_INFO_t *pPartInfo = (PARTITION_INFO_t*)au8_SectorBuf;

	//u32_Err = NC_ReadSector_RIUMode(u32_PageIdx, 0, au8_SectorBuf, au8_SpareBuf);
	u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_SectorBuf, au8_SpareBuf, 1);
	if (u32_Err != UNFD_ST_SUCCESS)
	{
		#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
		if(u32_PageIdx == 0)
			pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_ERROR;
		#endif

		return u32_Err;
	}

	u32_ChkSum = drvNAND_CheckSum(au8_SectorBuf + 0x04, 0x200 - 0x04);
	if (u32_ChkSum != pPartInfo->u32_ChkSum) {
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Skip PartInfo (ChkSum: 0x%08lx != 0x%08lx)\n",
				   u32_ChkSum, pPartInfo->u32_ChkSum);

		return UNFD_ST_ERR_CIS_PART_ERR;
	}

	#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
	if(u32_PageIdx == 0)
	{
		NC_CheckECC(&s32ECCStatus);
		if(s32ECCStatus > 0)
			pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_BITFLIP;
	}
	#endif

	u32_ByteCnt = pPartInfo->u16_PartCnt * pPartInfo->u16_UnitByteCnt;
	memcpy(pNandDrv->pPartInfo, pPartInfo, sizeof(*pPartInfo) + u32_ByteCnt);

	dump_part_info(pPartInfo);
	return UNFD_ST_SUCCESS;
}

void drvNAND_ParseNandInfo(NAND_FLASH_INFO_t *pNandInfo)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();

	pNandDrv->u16_PageByteCnt  = pNandInfo->u16_PageByteCnt;
	pNandDrv->u16_SpareByteCnt = pNandInfo->u16_SpareByteCnt;
	pNandDrv->u16_BlkPageCnt   = pNandInfo->u16_BlkPageCnt;
	pNandDrv->u16_BlkCnt       = pNandInfo->u16_BlkCnt;
	pNandDrv->u16_ECCType      = pNandInfo->u16_ECCType;
	pNandDrv->u8_IDByteCnt     = pNandInfo->u8_IDByteCnt;
	pNandDrv->u32_Config       = pNandInfo->u32_Config;
	pNandDrv->u16_tRC          = pNandInfo->u16_tRC;
	pNandDrv->u8_tRP           = pNandInfo->u8_tRP;
	pNandDrv->u8_tREH          = pNandInfo->u8_tREH;
	pNandDrv->u8_tREA          = pNandInfo->u8_tREA;
	pNandDrv->u8_tRR           = pNandInfo->u8_tRR;
	pNandDrv->u16_tADL         = pNandInfo->u16_tADL;
	pNandDrv->u16_tRHW         = pNandInfo->u16_tRHW;
	pNandDrv->u16_tWHR         = pNandInfo->u16_tWHR;
	pNandDrv->u16_tCCS         = pNandInfo->u16_tCCS;
	pNandDrv->u8_tCS           = pNandInfo->u8_tCS;
	pNandDrv->u16_tWC          = pNandInfo->u16_tWC;
	pNandDrv->u8_tWP           = pNandInfo->u8_tWP;
	pNandDrv->u8_tWH           = pNandInfo->u8_tWH;
	pNandDrv->u16_tCWAW        = pNandInfo->u16_tCWAW;
	pNandDrv->u8_tCLHZ		   = pNandInfo->u8_tCLHZ;
	pNandDrv->u16_tWW		   = pNandInfo->u16_tWW;
	pNandDrv->u8_AddrCycleIdx  = pNandInfo->u8_AddrCycleIdx;

	if(pNandDrv->u8_AddrCycleIdx != 0)
	{
		pNandDrv->u8_OpCode_Erase_AdrCycle = seq[pNandDrv->u8_AddrCycleIdx].u8_OpCode_Erase_AdrCycle;
		pNandDrv->u8_OpCode_RW_AdrCycle = seq[pNandDrv->u8_AddrCycleIdx].u8_OpCode_RW_AdrCycle;
		pNandDrv->u16_One_Col_Addr = seq[pNandDrv->u8_AddrCycleIdx].u16_Reg48_Spare;
		pNandDrv->u16_Reg48_Spare &= ~BIT_NC_ONE_COL_ADDR;
		pNandDrv->u16_Reg48_Spare |= seq[pNandDrv->u8_AddrCycleIdx].u16_Reg48_Spare;
	}
	
	memcpy(pNandDrv->u8_Vendor, pNandInfo->u8_Vendor, 16);
	memcpy(pNandDrv->u8_PartNumber, pNandInfo->u8_PartNumber, 16);

	pNandDrv->u8_CellType      = pNandDrv->u32_Config&0x1;
	pNandDrv->u8_BadBlkMarker      = (pNandDrv->u32_Config>>1)&0x7;
	pNandDrv->u8_PlaneCnt      = ((pNandDrv->u32_Config>>4)&0x7)+1;
	pNandDrv->u8_RequireRandomizer		= (pNandDrv->u32_Config>>8) & 0x1;
	pNandDrv->u8_NANDInterface	= ((pNandDrv->u32_Config>>9) & 3);
	pNandDrv->u8_RequireReadRetry = ((pNandDrv->u32_Config>>15) & 1);

	pNandDrv->u8_PairPageMapLoc = pNandInfo->u8_PairPageMapLoc;
	pNandDrv->u8_ReadRetryLoc =	pNandInfo->u8_ReadRetryLoc;
	pNandDrv->u16_BitflipThreshold = pNandInfo->u8_BitflipThreshold;
	#if defined(FCIE4_DDR) && FCIE4_DDR
	memcpy((void *) &pNandDrv->tDefaultDDR, (const void *) &pNandInfo->tDefaultDDR, sizeof(DDR_TIMING_GROUP_t));
	memcpy((void *) &pNandDrv->tMaxDDR, (const void *) &pNandInfo->tMaxDDR, sizeof(DDR_TIMING_GROUP_t));
	memcpy((void *) &pNandDrv->tMinDDR, (const void *) &pNandInfo->tMinDDR, sizeof(DDR_TIMING_GROUP_t));
	#endif

	pNandDrv->u8_BL0PBA = pNandInfo->u8_BL0PBA;
	pNandDrv->u8_BL1PBA = pNandInfo->u8_BL1PBA;
	pNandDrv->u8_UBOOTPBA = pNandInfo->u8_UBOOTPBA;
	pNandDrv->u8_HashPBA[0][0] = pNandInfo->u8_HashPBA[0][0];
	pNandDrv->u8_HashPBA[0][1] = pNandInfo->u8_HashPBA[0][1];
	pNandDrv->u8_HashPBA[1][0] = pNandInfo->u8_HashPBA[1][0];
	pNandDrv->u8_HashPBA[1][1] = pNandInfo->u8_HashPBA[1][1];
	pNandDrv->u8_HashPBA[2][0] = pNandInfo->u8_HashPBA[2][0];
	pNandDrv->u8_HashPBA[2][1] = pNandInfo->u8_HashPBA[2][1];

}

void drvNAND_DumpReadRetry(void)
{
	U16 u16_i, u16_j, u16_k;
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();

	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"u8_CustRegNo = %X\n", pNandDrv->ReadRetry_t.u8_CustRegNo);

	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"Cust Register: %X\n", pNandDrv->ReadRetry_t.u8_CustRegNo);
	for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_CustRegNo; u16_i++)
		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"%X ", pNandDrv->ReadRetry_t.pu8_CustRegTable[u16_i]);
	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"\n");

	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"u8_DefaultValueOffset = %X\n", pNandDrv->ReadRetry_t.u8_DefaultValueOffset);

	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"u8_MaxRetryTime = %X\n", pNandDrv->ReadRetry_t.u8_MaxRetryTime);

	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"u8_ByteLenPerCmd = %X\n", pNandDrv->ReadRetry_t.u8_ByteLenPerCmd);

	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"Retry Register Value Table\n ");

	for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_CustRegNo; u16_i ++)
		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"Reg%X\t", u16_i);
	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"\n");

	for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_MaxRetryTime; u16_i ++)
	{
		for(u16_j = 0; u16_j < pNandDrv->ReadRetry_t.u8_CustRegNo; u16_j ++)
		{
			for(u16_k = 0; u16_k < pNandDrv->ReadRetry_t.u8_ByteLenPerCmd+1; u16_k ++)
				nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"%02X", pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u16_i][u16_j][u16_k]);
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"\t");
		}
		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"\n");
	}
	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"u8_DefaultValueRestore = %X\n", pNandDrv->ReadRetry_t.u8_DefaultValueRestore);

	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"u8_SetCmdLen = %X\n", pNandDrv->ReadRetry_t.u8_SetCmdLen);
	
	if(pNandDrv->ReadRetry_t.u8_SetCmdLen)
	{
		for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_SetCmdLen; u16_i ++)
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"%X\t", pNandDrv->ReadRetry_t.pu8_SetCmdTypeSeq[u16_i]);
		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"\n");
		
		for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_SetCmdLen; u16_i ++)
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"%X\t", pNandDrv->ReadRetry_t.pu8_SetCmdValueSeq[u16_i]);
		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"\n");
	}
	nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,"u8_GetCmdLen = %X\n", pNandDrv->ReadRetry_t.u8_GetCmdLen);

	if(pNandDrv->ReadRetry_t.u8_GetCmdLen)
	{
		for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_GetCmdLen; u16_i ++)
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"%X\t", pNandDrv->ReadRetry_t.pu8_GetCmdTypeSeq[u16_i]);
		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"\n");
		
		for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_GetCmdLen; u16_i ++)
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"%X\t", pNandDrv->ReadRetry_t.pu8_GetCmdValueSeq[u16_i]);
		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0,"\n");
	}
}

U32 drvNAND_ParseReadRetryCmd(U8 *pu8_DataBuf)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	U16 u16_Idx = 0, u16_i, u16_j;

	pNandDrv->u8_RequireReadRetry = 1;
	pNandDrv->ReadRetry_t.u8_CustRegNo = pu8_DataBuf[u16_Idx];
	u16_Idx ++;
	//Number of Custom Register

	pNandDrv->ReadRetry_t.pu8_CustRegTable = (U8*)malloc(pNandDrv->ReadRetry_t.u8_CustRegNo * sizeof(U8));
	if(!pNandDrv->ReadRetry_t.pu8_CustRegTable)
		return UNFD_ST_ERR_INVALID_ADDR;
	//Reg Table
	memcpy(pNandDrv->ReadRetry_t.pu8_CustRegTable, pu8_DataBuf+u16_Idx, pNandDrv->ReadRetry_t.u8_CustRegNo);
	u16_Idx += pNandDrv->ReadRetry_t.u8_CustRegNo;
	
	pNandDrv->ReadRetry_t.u8_DefaultValueOffset = pu8_DataBuf[u16_Idx];
	u16_Idx ++;
	pNandDrv->ReadRetry_t.u8_MaxRetryTime = pu8_DataBuf[u16_Idx];
	u16_Idx ++;
	pNandDrv->ReadRetry_t.u8_ByteLenPerCmd = pu8_DataBuf[u16_Idx];
	u16_Idx ++;

	pNandDrv->ppu8_ReadRetryDefault = (U8**)malloc(pNandDrv->ReadRetry_t.u8_CustRegNo * sizeof(U8*) +
										pNandDrv->ReadRetry_t.u8_CustRegNo * (pNandDrv->ReadRetry_t.u8_ByteLenPerCmd + 1) *sizeof(U8));
	if(!pNandDrv->ppu8_ReadRetryDefault)
		return UNFD_ST_ERR_INVALID_ADDR;

	for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_CustRegNo; u16_i ++)
		pNandDrv->ppu8_ReadRetryDefault[u16_i] = ((U8*)(pNandDrv->ppu8_ReadRetryDefault + pNandDrv->ReadRetry_t.u8_CustRegNo) 
													+ u16_i * (pNandDrv->ReadRetry_t.u8_ByteLenPerCmd + 1) *sizeof(U8));

	//alloc Retry Register value table and parse Retry Register value table
	pNandDrv->ReadRetry_t.pppu8_RetryRegValue = 
		(U8***) malloc( pNandDrv->ReadRetry_t.u8_MaxRetryTime *sizeof(U8**) 
			+ pNandDrv->ReadRetry_t.u8_MaxRetryTime * pNandDrv->ReadRetry_t.u8_CustRegNo *sizeof(U8*) 
			+ pNandDrv->ReadRetry_t.u8_MaxRetryTime * pNandDrv->ReadRetry_t.u8_CustRegNo * (pNandDrv->ReadRetry_t.u8_ByteLenPerCmd + 1) *sizeof(U8) );
	if(!pNandDrv->ReadRetry_t.pppu8_RetryRegValue)
		return UNFD_ST_ERR_INVALID_ADDR;

	for(u16_i = 0 ; u16_i < pNandDrv->ReadRetry_t.u8_MaxRetryTime; u16_i ++)
		pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u16_i] = ((U8**) pNandDrv->ReadRetry_t.pppu8_RetryRegValue + 
											pNandDrv->ReadRetry_t.u8_MaxRetryTime) + u16_i * pNandDrv->ReadRetry_t.u8_CustRegNo;
	
	for(u16_i = 0; u16_i < pNandDrv->ReadRetry_t.u8_MaxRetryTime; u16_i ++)
		for(u16_j = 0; u16_j < pNandDrv->ReadRetry_t.u8_CustRegNo; u16_j ++)
			pNandDrv->ReadRetry_t.pppu8_RetryRegValue[u16_i][u16_j] = 
				((U8*) (pNandDrv->ReadRetry_t.pppu8_RetryRegValue + pNandDrv->ReadRetry_t.u8_MaxRetryTime + 
					pNandDrv->ReadRetry_t.u8_MaxRetryTime * pNandDrv->ReadRetry_t.u8_CustRegNo)) + 
						(u16_i*pNandDrv->ReadRetry_t.u8_CustRegNo + u16_j)* (pNandDrv->ReadRetry_t.u8_ByteLenPerCmd + 1) *sizeof(U8);
	
	memcpy(&pNandDrv->ReadRetry_t.pppu8_RetryRegValue[0][0][0], pu8_DataBuf + u16_Idx,
            pNandDrv->ReadRetry_t.u8_MaxRetryTime * pNandDrv->ReadRetry_t.u8_CustRegNo * (pNandDrv->ReadRetry_t.u8_ByteLenPerCmd + 1) *sizeof(U8));
	
	u16_Idx+= pNandDrv->ReadRetry_t.u8_MaxRetryTime * pNandDrv->ReadRetry_t.u8_CustRegNo * (pNandDrv->ReadRetry_t.u8_ByteLenPerCmd + 1) *sizeof(U8);
	pNandDrv->ReadRetry_t.u8_DefaultValueRestore = pu8_DataBuf[u16_Idx];
	u16_Idx ++;
	pNandDrv->ReadRetry_t.u8_SetCmdLen = pu8_DataBuf[u16_Idx];
	u16_Idx ++;
	if(pNandDrv->ReadRetry_t.u8_SetCmdLen)
	{
		//parse Set CMD Type Sequence
		pNandDrv->ReadRetry_t.pu8_SetCmdTypeSeq = (U8*)malloc(pNandDrv->ReadRetry_t.u8_SetCmdLen * sizeof(U8));
		if(!pNandDrv->ReadRetry_t.pu8_SetCmdTypeSeq)
			return UNFD_ST_ERR_INVALID_ADDR;
		memcpy(pNandDrv->ReadRetry_t.pu8_SetCmdTypeSeq, pu8_DataBuf + u16_Idx, pNandDrv->ReadRetry_t.u8_SetCmdLen);
		u16_Idx += pNandDrv->ReadRetry_t.u8_SetCmdLen;
		//parse Set CMD Value Sequence
    	pNandDrv->ReadRetry_t.pu8_SetCmdValueSeq = (U8*)malloc(pNandDrv->ReadRetry_t.u8_SetCmdLen * sizeof(U8));
		if(!pNandDrv->ReadRetry_t.pu8_SetCmdValueSeq)
			return UNFD_ST_ERR_INVALID_ADDR;
    	memcpy(pNandDrv->ReadRetry_t.pu8_SetCmdValueSeq, pu8_DataBuf + u16_Idx, pNandDrv->ReadRetry_t.u8_SetCmdLen);
   		u16_Idx += pNandDrv->ReadRetry_t.u8_SetCmdLen;
	}
	pNandDrv->ReadRetry_t.u8_GetCmdLen = pu8_DataBuf[u16_Idx];
	u16_Idx ++;
	if(pNandDrv->ReadRetry_t.u8_GetCmdLen)
	{	
		//parse Get CMD Type Sequence
		pNandDrv->ReadRetry_t.pu8_GetCmdTypeSeq = (U8*)malloc(pNandDrv->ReadRetry_t.u8_GetCmdLen * sizeof(U8));
		if(!pNandDrv->ReadRetry_t.pu8_GetCmdTypeSeq)
			return UNFD_ST_ERR_INVALID_ADDR;
		memcpy(pNandDrv->ReadRetry_t.pu8_GetCmdTypeSeq, pu8_DataBuf + u16_Idx, pNandDrv->ReadRetry_t.u8_GetCmdLen);
		u16_Idx += pNandDrv->ReadRetry_t.u8_GetCmdLen;
		//parse Get CMD Value Sequence
		pNandDrv->ReadRetry_t.pu8_GetCmdValueSeq = (U8*)malloc(pNandDrv->ReadRetry_t.u8_GetCmdLen * sizeof(U8));
		if(!pNandDrv->ReadRetry_t.pu8_GetCmdValueSeq)
			return UNFD_ST_ERR_INVALID_ADDR;
		memcpy(pNandDrv->ReadRetry_t.pu8_GetCmdValueSeq, pu8_DataBuf + u16_Idx, pNandDrv->ReadRetry_t.u8_GetCmdLen);
		u16_Idx += pNandDrv->ReadRetry_t.u8_GetCmdLen;
	}
	return UNFD_ST_SUCCESS;
}
U32 drvNAND_SearchCIS(void)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	NAND_FLASH_INFO_t *pNandInfo = (NAND_FLASH_INFO_t*)au8_PageBuf;
	BLK_INFO_t *pBlkInfo = (BLK_INFO_t*)au8_SpareBuf;
	U16 u16_PBA;
	U32 u32_PageIdx;
	U32 u32_Err;
	U32 u32_ChkSum;
	U32 u16_i;
	#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
	int s32ECCStatus;
	#endif

	#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
	pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_NORMAL;
	#endif

	U8 u8_BlkPageCntBits = 5; /* 32 pages per block */

	pNandDrv->u8_CISBlk = 0xFF;
	for (u16_PBA = 0; u16_PBA < 0x40; u16_PBA++)
	{
		pNandDrv->u16_BlkCnt = 0x400;
		pNandDrv->u16_BlkPageCnt = 0x20;
		pNandDrv->u16_PageByteCnt = 0x800;
		pNandDrv->u16_SpareByteCnt = 0x100;
		pNandDrv->u16_ECCType = NANDINFO_ECC_TYPE;
		pNandDrv->u16_BitflipThreshold = 0;

		NC_ConfigContext();
		NC_ReInit();
		pNandDrv->u16_Reg48_Spare |= BIT_NC_HW_AUTO_RANDOM_CMD_DISABLE;
		//force disable Randomizer
		#if defined(FCIE_LFSR) && FCIE_LFSR	
		NC_DisableLFSR();
		#endif
		NC_Config();
		u32_PageIdx = u16_PBA << u8_BlkPageCntBits;
		nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0, "32Pages: %d\n", u16_PBA);

		// ------------------------------------
		if(pNandDrv->u8_Flag_004A30)
	    {
		    pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
		    pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
	    }
		LABEL_TRY_005A30:
		// ------------------------------------
			
		#if defined(ENABLE_NAND_RIU_MODE)&&ENABLE_NAND_RIU_MODE
		u32_Err = NC_ReadSector_RIUMode(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf);
		#else
		u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
		#endif
		#if 0		//WordMode is unsupported
		if (u32_Err != UNFD_ST_SUCCESS && 0==(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_MASM))
		{
			//nand_debug(0, 0, "NAND, change WordMode, now: %X\n", pNandDrv->u8_WordMode);

			// If read sector fail, change u8_WordMode and try again
			if (pNandDrv->u8_WordMode == 0)
			{
				pNandDrv->u8_WordMode = 1;
				pNandDrv->u16_Reg50_EccCtrl |= BIT_NC_WORD_MODE;
			}
			else
			{
				pNandDrv->u8_WordMode = 0;
				pNandDrv->u16_Reg50_EccCtrl &= (~BIT_NC_WORD_MODE);
			}

			#if defined(ENABLE_NAND_RIU_MODE)&&ENABLE_NAND_RIU_MODE
			u32_Err = NC_ReadSector_RIUMode(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf);
			#else
			u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
			#endif
			//nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0, "NC_ReadSectors2=%x\n", u32_Err);
		}
		#endif

		if (u32_Err != UNFD_ST_SUCCESS || pBlkInfo->u8_BadBlkMark != 0xFF)
		{
			if(1==pNandDrv->u8_Flag_004A30 && ADR_C5TFS0!=pNandDrv->u8_OpCode_RW_AdrCycle)
			{
				nand_debug(0,1,"switch to 005A30, 0 \n");
				pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
				pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
				goto LABEL_TRY_005A30;
			}

			#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
			if(u16_PBA == 0)
				pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_ERROR;
			#endif

			continue;
		}

		/* Valid CIS has block address 0 or 1 */
		//if (pBlkInfo->u16_BlkAddr > 1)
		//	continue;

		if (drvNAND_CompareCISTag(pNandInfo->au8_Tag))
		{
			if(1==pNandDrv->u8_Flag_004A30 && ADR_C5TFS0!=pNandDrv->u8_OpCode_RW_AdrCycle)
			{
				//nand_debug(0,1,"switch to 005A30, 1 \n");
				pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
				pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
				goto LABEL_TRY_005A30;
			}

			#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
			if(u16_PBA == 0)
				pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_ERROR;
			#endif

			continue;
		}

		u32_ChkSum = drvNAND_CheckSum(au8_PageBuf + 0x24, 0x32 - 0x24);
		if (u32_ChkSum != pNandInfo->u32_ChkSum)
		{
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1, "Skip Blk 0x%08x, ChkSum: 0x%08lx != 0x%08lx\n",
				u16_PBA, u32_ChkSum, pNandInfo->u32_ChkSum);

			if(1==pNandDrv->u8_Flag_004A30 && ADR_C5TFS0!=pNandDrv->u8_OpCode_RW_AdrCycle)
			{
				//nand_debug(0,1,"switch to 005A30, 2 \n");
				pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
				pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
				goto LABEL_TRY_005A30;
			}

			#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
			if(u16_PBA == 0)
				pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_ERROR;
			#endif

			continue;
		}

		#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
		if(u16_PBA == 0)
		{
			NC_CheckECC(&s32ECCStatus);
			if(s32ECCStatus > 0)
				pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_BITFLIP;
		}
		#endif

		//u8_CISIdx = pBlkInfo->u16_BlkAddr;

		nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0, "Complete auto-checking 8/16 bits:0x%X\n", pNandDrv->u8_WordMode);
		dump_nand_info(pNandInfo);

		drvNAND_ParseNandInfo(pNandInfo);
		
		NC_ConfigContext();
		NC_ReInit();
		#if defined(FCIE_LFSR) && FCIE_LFSR 
		if(pNandDrv->u8_RequireRandomizer)
			nand_debug(0,1,"Enable Randomizer\n");
		//	NC_EnableLFSR();							//Compatible with ROM Code
		#endif
		NC_Config();
		if(pNandDrv->u8_CellType == 0)
		{
			for(u16_i=0;u16_i<pNandDrv->u16_BlkPageCnt;u16_i++)
			{
				ga_tPairedPageMap[u16_i].u16_LSB = u16_i;
				ga_tPairedPageMap[u16_i].u16_MSB = u16_i;				
			}
		}
		else
		{
			nand_debug(UNFD_DEBUG_LEVEL, 1, "Paired Page Map @0x%08x\n", pNandDrv->u8_PairPageMapLoc);
			u32_Err = NC_ReadPages(u32_PageIdx+pNandDrv->u8_PairPageMapLoc, au8_PageBuf, au8_SpareBuf, 1);
			if (u32_Err != UNFD_ST_SUCCESS)
			{
				nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1, "Skip Blk 0x%08x, Read PPM fail:0x%lX\n",
						   u16_PBA, u32_Err);

				#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
				if(u16_PBA == 0)
					pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_ERROR;
				#endif
		
				continue;
			}
			else
			{
				#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
				if(u16_PBA == 0)
				{
					NC_CheckECC(&s32ECCStatus);
					if(s32ECCStatus > 0)
						pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_BITFLIP;
				}
				#endif

				memcpy(&ga_tPairedPageMap[0], au8_PageBuf, 2048);
			}
		}

		if((u32_Err == UNFD_ST_SUCCESS))
		{
			nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "PartInfo @0x%08x\n", ga_tPairedPageMap[1].u16_LSB);
			pNandDrv->u8_HasPNI = 1;
			u32_Err = drvNAND_CheckPartInfo(u32_PageIdx + ga_tPairedPageMap[1].u16_LSB);
			if (u32_Err != UNFD_ST_SUCCESS) {
				pNandDrv->u8_HasPNI = 0;
				nand_debug(UNFD_DEBUG_LEVEL, 1, "No PartInfo\n");
			}
		}

		//Read Retry
		if(pNandDrv->u8_RequireReadRetry)
		{
			if(pNandDrv->u8_ReadRetryLoc != 0) //Customize Read Retry
			{
				/*					
				//no more customize read retry table (Q_Q)
				
				nand_debug(UNFD_DEBUG_LEVEL, 1, "Customize Read Retry Command Table @0x%08x\n", pNandDrv->u8_ReadRetryLoc);
				u32_Err = NC_ReadPages(u32_PageIdx + pNandDrv->u8_ReadRetryLoc , au8_PageBuf, au8_SpareBuf, 1);
				if(u32_Err != UNFD_ST_SUCCESS)
				{
					nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,  "Skip Blk 0x%08x, Read Customize Read Retry Command Table fail:0x%lX\n",
								u16_PBA, u32_Err);

					#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
					if(u16_PBA == 0)
						pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_ERROR;
					#endif

					continue;
				}
				else
				{
					#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
					if(u16_PBA == 0)
					{
						NC_CheckECC(&s32ECCStatus);
						if(s32ECCStatus > 0)
							pNandDrv->u8_CIS0_EccStatus = ECC_STATUS_BITFLIP;
					}
					#endif

					u32_Err = drvNAND_ParseReadRetryCmd(au8_PageBuf);
					if(u32_Err != UNFD_ST_SUCCESS)
					{
						nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Parse Read Retry Table Fail, disable Read Retry\n");
						pNandDrv->u8_RequireReadRetry = 0;
					}
					else
					{
						drvNAND_DumpReadRetry();
						if(pNandDrv->ReadRetry_t.u8_GetCmdLen)
							NC_GetReadRetryRegValue(pNandDrv->ppu8_ReadRetryDefault);
					}
				}
				*/
				/*
				// FIXME
				u8_ReadRetryLoc is used to choose which function is designed for current MLC nand
				For hynix MLC, a getting read-retry status function is required.
				*/	
				
			}
			else								// Using build-in Read Retry , ex: Toshiba Read-Retry Sequence
			{
				nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1, "Using build-in Read Retry Sequence for ");
				//set up max retry time
				if(pNandDrv->au8_ID[0] == 0x98)			//Toshiba
				{
					nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0, "Toshiba MLC\n");
					pNandDrv->ReadRetry_t.u8_MaxRetryTime = TSB_READRETRY_TIME;
				}
				else if(pNandDrv->au8_ID[0] == 0x2C)	//Micron
				{
					nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0, "Micron MLC\n");
					pNandDrv->ReadRetry_t.u8_MaxRetryTime = MCIRON_READRETRY_TIME;
				}
				else if(pNandDrv->au8_ID[0] == 0xEC)
				{
					nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0, "Samsung MLC\n");
					pNandDrv->ReadRetry_t.u8_MaxRetryTime = SAMSUNG_READRETRY_TIME;
				}
			}
		}

		pNandDrv->u8_CISBlk = (u16_PBA<<5)>>pNandDrv->u8_BlkPageCntBits;
		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0, "CIS is found @Blk%d\n", pNandDrv->u8_CISBlk);
		
		return UNFD_ST_SUCCESS;
	}

	nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "No CIS found\n");
	return UNFD_ST_ERR_NO_CIS;
}

U32 drvNAND_SearchCIS1(U8 *u8_CIS1Blk)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	NAND_FLASH_INFO_t *pNandInfo = (NAND_FLASH_INFO_t*)au8_PageBuf;
	BLK_INFO_t *pBlkInfo = (BLK_INFO_t*)au8_SpareBuf;
	U16 u16_PBA;
	U32 u32_PageIdx;
	U32 u32_Err;
	U32 u32_ChkSum;
	U16 u16_PageByteCnt;
	U16 u16_SpareByteCnt;
	U16 u16_ECCType;

	for (u16_PBA = pNandDrv->u8_CISBlk+1 ; u16_PBA < 10; u16_PBA++)
	{
		u16_PageByteCnt = pNandDrv->u16_PageByteCnt;
		u16_SpareByteCnt = pNandDrv->u16_SpareByteCnt;
		u16_ECCType = pNandDrv->u16_ECCType;
		
		pNandDrv->u16_PageByteCnt = 2048;
		pNandDrv->u16_SpareByteCnt = 256;
		pNandDrv->u16_ECCType = NANDINFO_ECC_TYPE;
		
		NC_ConfigContext();
		NC_ReInit();
		pNandDrv->u16_Reg48_Spare |= BIT_NC_HW_AUTO_RANDOM_CMD_DISABLE;
		//force disable Randomizer
		#if defined(FCIE_LFSR) && FCIE_LFSR	
		NC_DisableLFSR();
		#endif
		NC_Config();
		u32_PageIdx = u16_PBA << pNandDrv->u8_BlkPageCntBits;

		#if defined(ENABLE_NAND_RIU_MODE)&&ENABLE_NAND_RIU_MODE
		u32_Err = NC_ReadSector_RIUMode(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf);
		#else
		u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
		#endif
		#if 0		//WordMode is unsupported
		if (u32_Err != UNFD_ST_SUCCESS && 0==(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_MASM))
		{
			//nand_debug(0, 0, "NAND, change WordMode, now: %X\n", pNandDrv->u8_WordMode);

			// If read sector fail, change u8_WordMode and try again
			if (pNandDrv->u8_WordMode == 0)
			{
				pNandDrv->u8_WordMode = 1;
				pNandDrv->u16_Reg50_EccCtrl |= BIT_NC_WORD_MODE;
			}
			else
			{
				pNandDrv->u8_WordMode = 0;
				pNandDrv->u16_Reg50_EccCtrl &= (~BIT_NC_WORD_MODE);
			}

			#if defined(ENABLE_NAND_RIU_MODE)&&ENABLE_NAND_RIU_MODE
			u32_Err = NC_ReadSector_RIUMode(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf);
			#else
			u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
			#endif
			//nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0, "NC_ReadSectors2=%x\n", u32_Err);
		}
		#endif
	
		if (u32_Err != UNFD_ST_SUCCESS || pBlkInfo->u8_BadBlkMark != 0xFF)
		{
			continue;
		}

		/* Valid CIS has block address 0 or 1 */
		//if (pBlkInfo->u16_BlkAddr > 1)
		//	continue;

		if (drvNAND_CompareCISTag(pNandInfo->au8_Tag))
		{
			continue;
		}

		u32_ChkSum = drvNAND_CheckSum(au8_PageBuf + 0x24, 0x32 - 0x24);
		if (u32_ChkSum != pNandInfo->u32_ChkSum)
		{
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1, "Skip Blk 0x%08x, ChkSum: 0x%08lx != 0x%08lx\n",
				u16_PBA, u32_ChkSum, pNandInfo->u32_ChkSum);

			continue;
		}

		pNandDrv->u16_PageByteCnt = u16_PageByteCnt;
		pNandDrv->u16_SpareByteCnt = u16_SpareByteCnt;
		pNandDrv->u16_ECCType = u16_ECCType;
		
		NC_ConfigContext();
		NC_ReInit();
		pNandDrv->u16_Reg48_Spare &= ~BIT_NC_HW_AUTO_RANDOM_CMD_DISABLE;

		#if defined(FCIE_LFSR) && FCIE_LFSR 
		if(pNandDrv->u8_RequireRandomizer)
			nand_debug(0,1,"Enable Randomizer\n");
		//	NC_EnableLFSR();							//Compatible with ROM Code
		#endif
		NC_Config();
		if(pNandDrv->u8_CellType == 0)
		{
			// do nothing
		}
		else
		{
			nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "Paired Page Map @0x%08x\n", pNandDrv->u8_PairPageMapLoc);
			u32_Err = NC_ReadPages(u32_PageIdx+pNandDrv->u8_PairPageMapLoc, au8_PageBuf, au8_SpareBuf, 1);
			if (u32_Err != UNFD_ST_SUCCESS)
			{
				nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1, "Skip Blk 0x%08x, Read PPM fail:0x%lX\n",
						   u16_PBA, u32_Err);

				continue;
			}
			else
			{
				if(memcmp((const void *) &ga_tPairedPageMap[0], (const void *) au8_PageBuf, 2048) != 0)
					continue;
			}
		}

		if((u32_Err == UNFD_ST_SUCCESS))
		{
			nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "PartInfo @0x%08x\n", ga_tPairedPageMap[1].u16_LSB);
			if(pNandDrv->u8_HasPNI == 1)
			{
				u32_Err = drvNAND_CheckPartInfo(u32_PageIdx + ga_tPairedPageMap[1].u16_LSB);
				if (u32_Err != UNFD_ST_SUCCESS)
					continue;
			}
		}

		//Read Retry
		if(pNandDrv->u8_RequireReadRetry)
		{
			if(pNandDrv->u8_ReadRetryLoc != 0) //Customize Read Retry
			{
				nand_debug(UNFD_DEBUG_LEVEL, 1, "Customize Read Retry Command Table @0x%08x\n", pNandDrv->u8_ReadRetryLoc);
				u32_Err = NC_ReadPages(u32_PageIdx + pNandDrv->u8_ReadRetryLoc , au8_PageBuf, au8_SpareBuf, 1);
				if(u32_Err != UNFD_ST_SUCCESS)
				{
					nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1,  "Skip Blk 0x%08x, Read Customize Read Retry Command Table fail:0x%lX\n",
								u16_PBA, u32_Err);

					continue;
				}
			}
		}

		nand_debug(UNFD_DEBUG_LEVEL_WARNING, 0, "CIS1 is found @Blk%d\n", u16_PBA);
		*u8_CIS1Blk = u16_PBA;

		return UNFD_ST_SUCCESS;
	}

	nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "No CIS1 found\n");
	return UNFD_ST_ERR_NO_CIS;
}

U32 drvNAND_InitBBT(INIT_BBT_t *pInitBBT)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
#if NAND_BUF_USE_STACK
	U8 au8_PageBuf[4096];
	U8 au8_SpareBuf[128];
#else
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
#endif
	U32 u32_PageIdx;
	U32 u32_Err;
	U32 u32_i, u32_k;
	U8 u8_IsBadBlk;
	U8 u8_BadBlkMarker = (pNandDrv->u32_Config>>1)&0x07;
	U8 u8_BusWidth = (pNandDrv->u32_Config>>7)&0x01;

	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "Bad Blk Col Addr=0x%X\n", u8_BadBlkMarker);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "Bus Width=0x%X\n", u8_BusWidth);
    //pInitBBT->u16_Cnt = 0;
	for(u32_i=0; u32_i<pNandDrv->u16_BlkCnt; u32_i++)
	{
		u8_IsBadBlk = 0;
		#if 0
		if(drvNAND_ErasePhyBlk(u32_i))
		{
			u8_IsBadBlk = 1;
			nand_debug(0,1,"bad blk: %X from erase \n", u32_i);
		}
		#endif
		for(u32_k=0; u32_k<3; u32_k++)
		{
			u32_PageIdx = (u32_i << pNandDrv->u8_BlkPageCntBits) + u32_k;
			u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
			if (u32_Err != UNFD_ST_SUCCESS )
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NC_ReadSectors(0x%lX)=0x%lX\n", u32_PageIdx, u32_Err);
			if(u8_BusWidth == 0) // 8-bit mode
			{
				if(au8_SpareBuf[u8_BadBlkMarker] != 0xFF)
				{
					u8_IsBadBlk = 1;
					break;
				}
			}
			else // 16-bit mode
			{
				if(au8_SpareBuf[(u8_BadBlkMarker<<1)] != 0xFF || au8_SpareBuf[(u8_BadBlkMarker<<1)+1] != 0xFF)
				{
					u8_IsBadBlk = 1;
					break;
				}
			}

			u32_PageIdx = ((u32_i + 1) << pNandDrv->u8_BlkPageCntBits) - 1 - u32_k;
			u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
			if (u32_Err != UNFD_ST_SUCCESS )
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NC_ReadSectors(0x%lX)=0x%lX\n", u32_PageIdx, u32_Err);
			if(u8_BusWidth == 0) // 8-bit mode
			{
				if(au8_SpareBuf[u8_BadBlkMarker] != 0xFF)
				{
					u8_IsBadBlk = 1;
					break;
				}
			}
			else // 16-bit mode
			{
				if(au8_SpareBuf[(u8_BadBlkMarker<<1)] != 0xFF || au8_SpareBuf[(u8_BadBlkMarker<<1)+1] != 0xFF)
				{
					u8_IsBadBlk = 1;
					break;
				}
			}
		}

        if (u8_IsBadBlk == 1)
        {
            nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Bad Blk = 0x%lX\n", u32_i);

            if (pInitBBT->u16_Cnt <= 250)
            {
                pInitBBT->au8_BadBlkTbl[(pInitBBT->u16_Cnt<<1)] = (u32_i & 0xFF);
                pInitBBT->au8_BadBlkTbl[(pInitBBT->u16_Cnt<<1)+1] = ((u32_i >> 8) & 0xFF);
            }
			pInitBBT->u16_Cnt++;
            drvNAND_MarkBadBlk(u32_i);
        }
    }

    nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Total Bad Blk = 0x%X\n", pInitBBT->u16_Cnt);

	pInitBBT->u32_ChkSum = drvNAND_CheckSum((U8*)(&(pInitBBT->u16_Cnt)), (512-4));
	return UNFD_ST_SUCCESS;
}


//--------------------------------------------------------------------------
static UNFD_ALIGN0 TEST_ALIGN_PACK_t g_TestAlignPack_t UNFD_ALIGN1;

U32 drvNAND_CheckAlignPack(U8 u8_AlignByteCnt)
{
	// check alignment
	if((U32)&(g_TestAlignPack_t.u8_0) & (u8_AlignByteCnt-1))
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "ERROR, not aliged. expect %X but %lX \n",
			u8_AlignByteCnt, (U32)&(g_TestAlignPack_t.u8_0));
		nand_die();
		return UNFD_ST_ERR_NOT_ALIGN;
	}
	// check packed - 0
	if((U32)&(g_TestAlignPack_t.u16_0)-(U32)&(g_TestAlignPack_t.u8_0) != 1 ||
	   (U32)&(g_TestAlignPack_t.u32_0)-(U32)&(g_TestAlignPack_t.u8_0) != 3 ||
	   (U32)&(g_TestAlignPack_t.u32_1)-(U32)&(g_TestAlignPack_t.u8_0) != 7)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "ERROR, not packed. check err.0 \n");
		nand_die();
		return UNFD_ST_ERR_NOT_PACKED;
	}
	// check packed - 1
	if((U32)&(g_TestAlignPack_t.u16_0)-(U32)&(g_TestAlignPack_t.u8_0) != 1 ||
	   (U32)&(g_TestAlignPack_t.u32_0)-(U32)&(g_TestAlignPack_t.u16_0)!= 2 ||
	   (U32)&(g_TestAlignPack_t.u32_1)-(U32)&(g_TestAlignPack_t.u32_0)!= 4)
	{
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "ERROR, not packed. check err.1 \n");
		nand_die();
		return UNFD_ST_ERR_NOT_PACKED;
	}

	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "ok\n");
	return UNFD_ST_SUCCESS;
}
//--------------------------------------------------------------------------

static __inline void dump_mem_line(unsigned char *buf, int cnt)
{
	int i;

	nand_debug(UNFD_DEBUG_LEVEL_ERROR, 0, " 0x%08lx: ", (U32)buf);
	for (i= 0; i < cnt; i++)
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 0, "%02X ", buf[i]);

	nand_debug(UNFD_DEBUG_LEVEL_ERROR, 0, " | ");

	for (i = 0; i < cnt; i++)
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 0, "%c", (buf[i] >= 32 && buf[i] < 128) ? buf[i] : '.');

	nand_debug(UNFD_DEBUG_LEVEL_ERROR, 0, "\n");
}

void dump_mem(unsigned char *buf, int cnt)
{
	int i;

	for (i= 0; i < cnt; i+= 16)
		dump_mem_line(buf + i, 16);
}

void dump_nand_info(NAND_FLASH_INFO_t *pNandInfo)
{
	int i;
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "###############################################\n");
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "#        NAND INFO                            #\n");
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "###############################################\n");

	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "pNandInfo: 0x%08lx\n", (U32)pNandInfo);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "au8_Tag          : [");
	for (i = 0; i < 16; i++)
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 0, "%c", pNandInfo->au8_Tag[i]);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 0, "]\n");

	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u8_IDByteCnt     : 0x%04x\n", pNandInfo->u8_IDByteCnt);

	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "au8_ID           : 0x[ ");
	for (i = 0; i < pNandInfo->u8_IDByteCnt; i++)
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 0, "%02X ", pNandInfo->au8_ID[i]);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 0, "]\n");

	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u32_ChkSum       : 0x%04lx\n", pNandInfo->u32_ChkSum);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_SpareByteCnt : 0x%04x\n", pNandInfo->u16_SpareByteCnt);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_PageByteCnt  : 0x%04x\n", pNandInfo->u16_PageByteCnt);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_BlkPageCnt   : 0x%04x\n", pNandInfo->u16_BlkPageCnt);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_BlkCnt       : 0x%04x\n", pNandInfo->u16_BlkCnt);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u32_Config       : 0x%08lx\n", pNandInfo->u32_Config);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_ECCType      : 0x%04x\n", pNandInfo->u16_ECCType);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_tRC: 0x%04x\n", pNandInfo->u16_tRC);
}

void dump_part_records(PARTITION_RECORD_t *records, int cnt)
{
	int i;

	if (cnt > 1024) {
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "broken\n");
		return;
	}

	for (i = 0; i < cnt; i++) {
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "record[%d]       : 0x%04x, 0x%04x, 0x%04x, 0x%04x\n",
				i,
				records[i].u16_StartBlk, records[i].u16_BlkCnt,
				records[i].u16_PartType, records[i].u16_BackupBlkCnt);
	}
}

void dump_general_blk_info(BLK_INFO_t *pBlkInfo)
{
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u8_BadBlkIdx    : 0x%04x\n", pBlkInfo->u8_BadBlkMark);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u8_PartType     : 0x%04x\n", pBlkInfo->u8_PartType);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_BlkAddr     : 0x%04x\n", pBlkInfo->u16_BlkAddr);
	//nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "au8_Misc        : 0x%04x\n", *(U16*)pBlkInfo->au8_Misc);
}

void dump_part_info(PARTITION_INFO_t *pPartInfo)
{
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "###############################################\n");
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "#        PART INFO                            #\n");
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "###############################################\n");

	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "pPartInfo: 0x%08lx\n", (U32)pPartInfo);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u32_ChkSum      : 0x%04lx\n", pPartInfo->u32_ChkSum);
//	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u32_BLSectorCnt : 0x%04lx\n", pPartInfo->u32_BLSectorCnt);
//	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u32_OSSectorCnt : 0x%04lx\n", pPartInfo->u32_OSSectorCnt);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_PartCnt     : 0x%04x\n", pPartInfo->u16_PartCnt);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u16_UnitByteCnt : 0x%04x\n", pPartInfo->u16_UnitByteCnt);

	dump_part_records(pPartInfo->records, pPartInfo->u16_PartCnt);
}

void dump_miu_records(MIU_RECORD_t *pRecords, int cnt)
{
	int i;

	if (cnt > (1024 - 8)) {
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "broken\n");
		return;
	}
	for (i = 0; i < (cnt >> 3); i++)
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "miu record[%02d]  : 0x%08lx, 0x%08lx\n",
				i, pRecords[i].u32_RegAddr, pRecords[i].u32_RegValue);
}

void dump_miu_part(MIU_PART_t *pMiuPart)
{
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "###############################################\n");
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "#        MIU PART                             #\n");
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "###############################################\n");

	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "pMiuPart: 0x%08lx\n", (U32)pMiuPart);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u32_ChkSum      : 0x%04lx\n", pMiuPart->u32_ChkSum);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "u32_ByteCnt     : 0x%04lx\n", pMiuPart->u32_ByteCnt);

	dump_miu_records(pMiuPart->records, pMiuPart->u32_ByteCnt);
}

void dump_nand_driver(NAND_DRIVER *pNandDrv)
{
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "NAND_Info:\n");
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  BlkCnt                 : 0x%X\n", pNandDrv->u16_BlkCnt);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  BlkPageCnt             : 0x%X\n", pNandDrv->u16_BlkPageCnt);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  BlkSectorCnt           : 0x%X\n", pNandDrv->u16_BlkSectorCnt);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  PageByteCnt            : 0x%X\n", pNandDrv->u16_PageByteCnt);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  SpareByteCnt           : 0x%X\n\n", pNandDrv->u16_SpareByteCnt);

	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  BlkPageCntBits         : 0x%X\n", pNandDrv->u8_BlkPageCntBits);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  BlkSectorCntBits       : 0x%X\n", pNandDrv->u8_BlkSectorCntBits);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  PageByteCntBits        : 0x%X\n", pNandDrv->u8_PageByteCntBits);

	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  BlkPageCntMask         : 0x%X\n", pNandDrv->u16_BlkPageCntMask);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  BlkSectorCntMask       : 0x%X\n", pNandDrv->u16_BlkSectorCntMask);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  PageByteCntMask        : 0x%X\n", pNandDrv->u16_PageByteCntMask);

	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  PageSectorCnt          : 0x%X\n", pNandDrv->u16_PageSectorCnt);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  SectorByteCnt          : 0x%X\n", pNandDrv->u16_SectorByteCnt);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  SectorSpareByteCnt     : 0x%X\n", pNandDrv->u16_SectorSpareByteCnt);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  ECCCodeByteCnt         : 0x%X\n\n", pNandDrv->u16_ECCCodeByteCnt);

	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  PageSectorCntBits      : 0x%X\n", pNandDrv->u8_PageSectorCntBits);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  SectorByteCntBits      : 0x%X\n", pNandDrv->u8_SectorByteCntBits);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  PageSectorCntMask      : 0x%X\n", pNandDrv->u16_PageSectorCntMask);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  SectorByteCntMask      : 0x%X\n\n", pNandDrv->u16_SectorByteCntMask);

	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_OpCode_Erase_AdrCycle      : 0x%X\n", pNandDrv->u8_OpCode_Erase_AdrCycle);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_OpCode_RW_AdrCycle      : 0x%X\n\n", pNandDrv->u8_OpCode_RW_AdrCycle);

	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u16_tRC;      : 0x%X\n", pNandDrv->u16_tRC);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_tRP      : 0x%X\n", pNandDrv->u8_tRP);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_tREH      : 0x%X\n", pNandDrv->u8_tREH);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_tREA      : 0x%X\n", pNandDrv->u8_tREA);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_tRR      : 0x%X\n", pNandDrv->u8_tRR);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u16_tADL      : 0x%X\n", pNandDrv->u16_tADL);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u16_tRHW      : 0x%X\n", pNandDrv->u16_tRHW);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u16_tWHR      : 0x%X\n", pNandDrv->u16_tWHR);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u16_tCCS      : 0x%X\n", pNandDrv->u16_tCCS);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_tCS      : 0x%X\n", pNandDrv->u8_tCS);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u16_tWC      : 0x%X\n", pNandDrv->u16_tWC);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_tWP      : 0x%X\n", pNandDrv->u8_tWP);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_tWH      : 0x%X\n", pNandDrv->u8_tWH);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u16_tCWAW      : 0x%X\n", pNandDrv->u16_tCWAW);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_tCLHZ      : 0x%X\n", pNandDrv->u8_tCLHZ);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u16_tWW      : 0x%X\n", pNandDrv->u16_tWW);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "  u8_AddrCycleIdx      : 0x%X\n\n", pNandDrv->u8_AddrCycleIdx);


	
}

U8 drvNAND_CountBits(U32 u32_x)
{
	U8 u8_i = 0;

    if(u32_x==0) return u8_i;
    
	while (u32_x) {
		u8_i++;
		u32_x >>= 1;
	}

	return u8_i-1;
}

U32 drvNAND_CheckSum(U8 *pu8_Data, U16 u16_ByteCnt)
{
	U32 u32_Sum = 0;

	while (u16_ByteCnt--)
		u32_Sum += *pu8_Data++;

	return u32_Sum;
}

/* return 0: same, 1: different */
U32 drvNAND_CompareCISTag(U8 *tag)
{
	const char *str = "MSTARSEMIUNFDCIS";
	int i = 0;
	
	for (i = 0; i < 16; i++) {
		if (tag[i] != str[i])
			return 1;
	}

	return 0;
}

/* Search for partition of type @u16_PartType, begin from @pRecord */
PARTITION_RECORD_t *drvNAND_SearchPartition(PARTITION_RECORD_t *pRecord,
											U16 u16_PartType)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	PARTITION_INFO_t *pPartInfo = pNandDrv->pPartInfo;

	while (pRecord - pPartInfo->records < pPartInfo->u16_PartCnt) {

		if (pRecord->u16_PartType == u16_PartType)
			return pRecord;

		pRecord++;
	}

	return (void*)0;
}

/* return 1: Good block, 0: Bad block */
U32 drvNAND_IsGoodBlk(U16 u16_PBA)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
#if NAND_BUF_USE_STACK
	U8 au8_SectorBuf[1024];
	U8 au8_SpareBuf[64];
#else
	U8 *au8_SectorBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
#endif
	U32 u32_PageIdx;
	U32 u32_Err;
	BLK_INFO_t *pBlkInfo = (BLK_INFO_t*)au8_SpareBuf;

	u32_PageIdx = u16_PBA << pNandDrv->u8_BlkPageCntBits;
	//u32_Err = NC_ReadSector_RIUMode(u32_PageIdx, 0, au8_SectorBuf, au8_SpareBuf);
	u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_SectorBuf, au8_SpareBuf, 1);
	if (u32_Err != UNFD_ST_SUCCESS)
        nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NC_ReadSectors(0x%X)=0x%X\n", (unsigned int)u32_PageIdx, (unsigned int)u32_Err);
	
	if (pBlkInfo->u8_BadBlkMark != 0xFF)
		return 0;

	return 1;
}

U8 drvNAND_CheckAll0xFF(U8* pu8_Buf, U32 u32_ByteCnt)
{
	register U32 u32_i;

	for(u32_i=0; u32_i<u32_ByteCnt; u32_i++)
		if(0xFF != pu8_Buf[u32_i])
			return 0;

	return 0xFF;
}

/* return 1: Free block, 0: not Free block */
U32 drvNAND_IsFreeBlk(U16 u16_PBA)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
    #if NAND_BUF_USE_STACK
	U8 au8_SectorBuf[1024];
	U8 au8_SpareBuf[64];
    #else
	U8 *au8_SectorBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
    #endif
	U32 u32_PageIdx;
	U32 u32_Err;

	// check first page
	u32_PageIdx = u16_PBA << pNandDrv->u8_BlkPageCntBits;
	u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_SectorBuf, au8_SpareBuf, 1);
	if (u32_Err != UNFD_ST_SUCCESS)
        nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err Code: %X\n", (unsigned int)u32_Err);

	if(UNFD_ST_SUCCESS != u32_Err ||
	   0xFF != drvNAND_CheckAll0xFF(au8_SpareBuf, pNandDrv->u16_SectorSpareByteCnt))
		return 0; // not free blk

	// check last page
	u32_PageIdx = ((u16_PBA+1) << pNandDrv->u8_BlkPageCntBits) -1;
	u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_SectorBuf, au8_SpareBuf, 1);
	if (u32_Err != UNFD_ST_SUCCESS)
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NC_ReadSectors(0x%lX)=0x%lX\n", u32_PageIdx, u32_Err);

    if(0xFF != drvNAND_CheckAll0xFF(au8_SpareBuf, pNandDrv->u16_SectorSpareByteCnt))
		return 0; // not free blk

	return 1; // free blk
}


U16 drvNAND_FindFreePage(U16 u16_PBA) // binary search
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	U8  *pu8_SpareBuf = pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	U8  *pu8_DataBuf = pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8  u8_i;
	S8  s8_Dir=0;
	U32 u32_Row;
	U32 u32_Ret, u32_FinalRow;

	//----------------------------------
	// binary search
    u32_Row = u16_PBA << pNandDrv->u8_BlkPageCntBits;
	u32_Row += pNandDrv->u16_BlkPageCnt>>1;

	// [CAUTION]: for 2 pages/blk NAND, need to update this routine
	for(u8_i=0; u8_i<pNandDrv->u8_BlkPageCntBits-1; u8_i++)
	{
		u32_Ret = NC_ReadSectors(u32_Row, 0, pu8_DataBuf, pu8_SpareBuf, 1);

	    if(0xFF == drvNAND_CheckAll0xFF(pu8_SpareBuf, pNandDrv->u16_SectorSpareByteCnt) &&
		   UNFD_ST_SUCCESS==u32_Ret)
	    {
			s8_Dir = -1;
			u32_Row -= pNandDrv->u16_BlkPageCnt >> (u8_i+2);
	    }
		else
		{
			s8_Dir = 1;
			u32_Row += pNandDrv->u16_BlkPageCnt >> (u8_i+2);
		}
	}

	//----------------------------------
	// 2nd search
	if(s8_Dir < 0)
	{
		u32_FinalRow = u16_PBA << pNandDrv->u8_BlkPageCntBits;
		do
		{
			u32_Ret = NC_ReadSectors(u32_Row, 0, pu8_DataBuf, pu8_SpareBuf, 1);

			if(0xFF != drvNAND_CheckAll0xFF(pu8_SpareBuf, pNandDrv->u16_SectorSpareByteCnt) ||
			   UNFD_ST_SUCCESS!=u32_Ret)
				break;

			u32_Row--;
		}
		while(u32_Row >= u32_FinalRow);

		return (U8)((u32_Row+1)&pNandDrv->u16_BlkPageCntMask);
	}
	else
	{
		u32_FinalRow = (u16_PBA+1) << pNandDrv->u8_BlkPageCntBits;
		do
		{
			u32_Ret = NC_ReadSectors(u32_Row, 0, pu8_DataBuf, pu8_SpareBuf, 1);

			if(0xFF == drvNAND_CheckAll0xFF(pu8_SpareBuf, pNandDrv->u16_SectorSpareByteCnt) && UNFD_ST_SUCCESS==u32_Ret)
				return (U8)((u32_Row)&pNandDrv->u16_BlkPageCntMask);

			u32_Row++;
		}
		while(u32_Row < u32_FinalRow);
	}

	//----------------------------------
	return 0xFFFF; // no free page
}


U32 drvNAND_MoveBlkData(U16 u16_DestBlk, U16 u16_SrcBlk, U16 u16_PageCnt)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
    #if NAND_BUF_USE_STACK
	U8 au8_PageBuf[4096];
	U8 au8_SpareBuf[128];
    #else
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
    #endif
	U32 u32_Ret;
	U16 u16_i, u32_SrcRow, u32_DestRow;

	u32_SrcRow = u16_SrcBlk << pNandDrv->u8_BlkPageCntBits;
	u32_DestRow = u16_DestBlk << pNandDrv->u8_BlkPageCntBits;

	for(u16_i=0; u16_i<u16_PageCnt; u16_i++)
	{
		u32_Ret = NC_ReadPages(u32_SrcRow + u16_i, au8_PageBuf, au8_SpareBuf, 1);
		if(UNFD_ST_SUCCESS != u32_Ret)
			return u32_Ret;
		u32_Ret = NC_WritePages(u32_DestRow + u16_i, au8_PageBuf, au8_SpareBuf, 1);
		if(UNFD_ST_SUCCESS != u32_Ret)
		{
			drvNAND_MarkBadBlk(u16_DestBlk);
			return u32_Ret;
		}
	}
	return UNFD_ST_SUCCESS;
}


U32 drvNAND_ReadBlk(U8 *pu8_DestAddr, U8 u8_PartType, U16 u16_PBA,
							U16 u16_LBA, U32 u32_StartSector, U32 u32_SectorCnt)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
#if NAND_BUF_USE_STACK
	U8 au8_PageBuf[4096];
	U8 au8_SpareBuf[128];
#else
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	//U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
#endif
	U32 u32_Err;
	//BLK_INFO_t *pBlkInfo = (BLK_INFO_t*)au8_SpareBuf;
	U32 u32_PageIdx;
	U16 u16_PageCnt;
	U16 u16_SectorCntRead;
	U16 u16_SectorIdxInPage;

    nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "BlkIdx:%d, BlkNo:%d, StartSector:%d, SectorCnt:%d\n", (unsigned int)u16_PBA, (unsigned int)u16_LBA, (unsigned int)u32_StartSector, (unsigned int)u32_SectorCnt);
	#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
	pNandDrv->u8_StartCB = 1;
	#endif

	/* Read unaligned sectors first */
	if( u32_StartSector & pNandDrv->u16_PageSectorCntMask)
	{
		u16_SectorIdxInPage = u32_StartSector & pNandDrv->u16_PageSectorCntMask;
		u16_SectorCntRead = u32_SectorCnt > ((U32)pNandDrv->u16_PageSectorCnt-(U32)u16_SectorIdxInPage) ? ((U32)pNandDrv->u16_PageSectorCnt-(U32)u16_SectorIdxInPage) : (u32_SectorCnt);
		u32_PageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>pNandDrv->u8_PageSectorCntBits);
		#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
		pNandDrv->u16_LogiSectorCntCB = u16_SectorCntRead;
		#endif
		u32_Err = NC_ReadPages(u32_PageIdx, au8_PageBuf, (U8*)0, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

        nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04x failed with EC: 0x%04x\n",
                    (unsigned int)u32_PageIdx, (unsigned int)u32_Err);
			#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
			pNandDrv->u8_StartCB = 0;
			#endif
			return u32_Err;
		}

		memcpy(pu8_DestAddr, (au8_PageBuf+(u16_SectorIdxInPage<<9)), (u16_SectorCntRead<<9));
		pu8_DestAddr += (u16_SectorCntRead<<9);
		u32_StartSector += u16_SectorCntRead;
		u32_SectorCnt -= u16_SectorCntRead;
	}

	/* Read aligned sectors then */
	u32_PageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>pNandDrv->u8_PageSectorCntBits);
	u16_PageCnt = (u32_SectorCnt>>pNandDrv->u8_PageSectorCntBits);
	if(pNandDrv->u8_SwPatchWaitRb)
	{
		while (u16_PageCnt)
		{
			#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
			pNandDrv->u16_LogiSectorCntCB = pNandDrv->u16_PageSectorCnt;
			#endif		
			u32_Err = NC_ReadPages(u32_PageIdx, pu8_DestAddr, (U8*)0, 1);
			if (u32_Err != UNFD_ST_SUCCESS) {

                nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04x failed with EC: 0x%04x\n",
                    (unsigned int)u32_PageIdx, (unsigned int)u32_Err);
				#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
				pNandDrv->u8_StartCB = 0;
				#endif
				return u32_Err;
			}

			pu8_DestAddr += pNandDrv->u16_PageByteCnt;
			u32_PageIdx++;
			u16_PageCnt--;
			u32_StartSector += pNandDrv->u16_PageSectorCnt;
			u32_SectorCnt -= pNandDrv->u16_PageSectorCnt;
		}
	}
	else
	{
		if(u16_PageCnt>0)
		{
			#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
			pNandDrv->u16_LogiSectorCntCB = u16_PageCnt*pNandDrv->u16_PageSectorCnt;
			#endif			
			u32_Err = NC_ReadPages(u32_PageIdx, pu8_DestAddr, (U8*)0, u16_PageCnt);
			if (u32_Err != UNFD_ST_SUCCESS) {

				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04x failed with EC: 0x%04x\n",
                        (unsigned int)u32_PageIdx, (unsigned int)u32_Err);
				#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
				pNandDrv->u8_StartCB = 0;
				#endif
				return u32_Err;
			}

			pu8_DestAddr += (pNandDrv->u16_PageByteCnt*u16_PageCnt);
			u32_PageIdx += u16_PageCnt;
			u32_StartSector += (pNandDrv->u16_PageSectorCnt*u16_PageCnt);
			u32_SectorCnt -= (pNandDrv->u16_PageSectorCnt*u16_PageCnt);
		}
	}

	/* Read remaining unaligned sectors finally */
	if( u32_SectorCnt )
	{
		#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
		pNandDrv->u16_LogiSectorCntCB = u32_SectorCnt;
		#endif		
		u32_PageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>pNandDrv->u8_PageSectorCntBits);
		u32_Err = NC_ReadPages(u32_PageIdx, au8_PageBuf, (U8*)0, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04x failed with EC: 0x%04x\n",
                    (unsigned int)u32_PageIdx, (unsigned int)u32_Err);
			#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
			pNandDrv->u8_StartCB = 0;
			#endif
			return u32_Err;
		}
		memcpy(pu8_DestAddr, au8_PageBuf, (u32_SectorCnt<<9));
	}
	#if defined(ENABLE_CB_BEFORE_DMA) && ENABLE_CB_BEFORE_DMA
	pNandDrv->u8_StartCB = 0;
	#endif
	return UNFD_ST_SUCCESS;
}


U32 drvNAND_WriteBlkNonBackup(U8 *pu8_DestAddr, U8 u8_PartType, U16 u16_PBA,
					 U16 u16_LBA,	U32 u32_StartSector,
					 U32 u32_SectorCnt)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
#if NAND_BUF_USE_STACK
	U8 au8_PageBuf[4096];
	U8 au8_SpareBuf[128];
#else
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
#endif
	U32 u32_Err;
	BLK_INFO_t *pBlkInfo = (BLK_INFO_t*)au8_SpareBuf;
	U16 u16_PageSectorCnt;
	U8 u8_PageSectorCntBits;
	//U16 u16_PageSectorCntMask;
	U32 u32_PageIdx;
	U16 u16_PageCnt;

    nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "BlkIdx:%d, BlkNo:%d, StartSector:%d, SectorCnt:%d\n", (int)u16_PBA, (int)u16_LBA, (int)u32_StartSector, (int)u32_SectorCnt);

	/* sector size from AP is 512 Bytes, not NAND sector size */
	u16_PageSectorCnt = pNandDrv->u16_PageByteCnt >> 9;
	u8_PageSectorCntBits = drvNAND_CountBits(u16_PageSectorCnt);
	//u16_PageSectorCntMask = (1<<u8_PageSectorCntBits)-1;

	memset(au8_SpareBuf, '\0', sizeof(*pBlkInfo));
	pBlkInfo->u8_BadBlkMark = 0xFF;
	pBlkInfo->u8_PartType = u8_PartType;
	pBlkInfo->u16_BlkAddr = u16_LBA;

	u32_PageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>u8_PageSectorCntBits);
	u16_PageCnt = (u32_SectorCnt>>u8_PageSectorCntBits);

	while (u16_PageCnt)
	{
		u32_Err = NC_WritePages(u32_PageIdx, pu8_DestAddr, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

			nand_debug(2, 1, "Writting page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_PageIdx, u32_Err);

			return u32_Err;
		}

		pu8_DestAddr += pNandDrv->u16_PageByteCnt;
		u32_PageIdx++;
		u16_PageCnt--;
		u32_SectorCnt -= u16_PageSectorCnt;
	}

	if( u32_SectorCnt )
	{
		memset(au8_PageBuf, 0xFF, pNandDrv->u16_PageByteCnt);
		memcpy(au8_PageBuf, pu8_DestAddr, u32_SectorCnt<<9);

		u32_Err = NC_WritePages(u32_PageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

			nand_debug(2, 1, "Writting page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_PageIdx, u32_Err);

			return u32_Err;
		}
	}

	return UNFD_ST_SUCCESS;
}

U32 drvNAND_WriteBlk(U8 *pu8_DestAddr, U8 u8_PartType, U16 u16_PBA,
					 U16 u16_LBA, U32 u32_StartSector,
					 U32 u32_SectorCnt)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
#if NAND_BUF_USE_STACK
	U8 au8_PageBuf[4096];
	U8 au8_SpareBuf[128];
#else
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
#endif
	U32 u32_Err;
	BLK_INFO_t *pBlkInfo = (BLK_INFO_t*)au8_SpareBuf;
	U16 u16_PageSectorCnt;
	U8 u8_PageSectorCntBits;
	U16 u16_PageSectorCntMask;
	U32 u32_SrcPageIdx, u32_BakPageIdx;
	U16 u16_PageCnt;
	U16 u16_SectorCntRead;
	U16 u16_SectorIdxInPage;
	U16 u16_i;
	U16 u16_LastWrittenPage = 0;
	U16 u16_BakBlkIdx;
	U8 u8_GoodBlkCnt;

	/* sector size from AP is 512 Bytes, not NAND sector size */
	u16_PageSectorCnt = pNandDrv->u16_PageByteCnt >> 9;
	u8_PageSectorCntBits = drvNAND_CountBits(u16_PageSectorCnt);
	u16_PageSectorCntMask = (1<<u8_PageSectorCntBits)-1;

	if( (u32_StartSector & u16_PageSectorCntMask)  == 0 )
	{
		u32_SrcPageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>u8_PageSectorCntBits);
		u32_Err = NC_ReadPages(u32_SrcPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_SrcPageIdx, u32_Err);

			return u32_Err;
		}

		for(u16_i=0 ; u16_i<pNandDrv->u16_SpareByteCnt ; u16_i++)
		{
			if(au8_SpareBuf[u16_i] != 0xFF)
				goto LABEL_WRITE_BLK;
		}

		for(u16_i=0 ; u16_i<pNandDrv->u16_PageByteCnt ; u16_i++)
		{
			if(au8_PageBuf[u16_i] != 0xFF)
				goto LABEL_WRITE_BLK;
		}

		/* empty page, needn't to backup*/
		u32_Err = drvNAND_WriteBlkNonBackup(pu8_DestAddr, u8_PartType, u16_PBA,
				 					u16_LBA, u32_StartSector, u32_SectorCnt);
		return u32_Err;
	}

LABEL_WRITE_BLK:
    nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "BlkIdx:%d, BlkNo:%d, StartSector:%d, SectorCnt:%d\n", (int)u16_PBA, (int)u16_LBA, (int)u32_StartSector, (int)u32_SectorCnt);

	/* Find physical Backup BlkIdx */
	u16_BakBlkIdx = pNandDrv->pPartInfo->records->u16_StartBlk-1;
	u8_GoodBlkCnt = 0;
	while(1)
	{
		if(!drvNAND_IsGoodBlk(u16_BakBlkIdx))
		{
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1, "Skip bad blk: 0x%04x\n", u16_BakBlkIdx);
		}
		else
		{
			// reserve last 4 good blks for E2P0/E2P1/NVRAM0/NVRAM1
			if(++u8_GoodBlkCnt == 5)
				break;
		}

		if((--u16_BakBlkIdx)<(pNandDrv->pPartInfo->records->u16_StartBlk-7))
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "too many bad block\n");
			return UNFD_ST_ERR_LACK_BLK;
		}
	}
	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "u16_BakBlkIdx:%d\n", u16_BakBlkIdx);

	// erase bak block
	u32_Err = drvNAND_ErasePhyBlk(u16_BakBlkIdx);
	if (u32_Err != UNFD_ST_SUCCESS) {
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Erase Blk 0x%04x failed with EC: 0x%08lx\n",
				   u16_BakBlkIdx, u32_Err);
		drvNAND_MarkBadBlk(u16_BakBlkIdx);
		return u32_Err;
	}

	for(u16_i=0 ; u16_i<(u32_StartSector>>u8_PageSectorCntBits) ; u16_i++)
	{
		u32_SrcPageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + u16_i;
		u32_Err = NC_ReadPages(u32_SrcPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_SrcPageIdx, u32_Err);

			return u32_Err;
		}

		u32_BakPageIdx = (u16_BakBlkIdx << pNandDrv->u8_BlkPageCntBits) + u16_i;
		u32_Err = NC_WritePages(u32_BakPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Writing page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_BakPageIdx, u32_Err);
			drvNAND_MarkBadBlk(u16_BakBlkIdx);
			return u32_Err;
		}
		u16_LastWrittenPage = u16_i;
	}

	/* Read unaligned sectors first */
	if( u32_StartSector & u16_PageSectorCntMask )
	{
		u32_SrcPageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>u8_PageSectorCntBits);
		u32_Err = NC_ReadPages(u32_SrcPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_SrcPageIdx, u32_Err);

			return u32_Err;
		}
		u16_SectorIdxInPage = u32_StartSector & u16_PageSectorCntMask;
		u16_SectorCntRead = u32_SectorCnt > ((U32)u16_PageSectorCnt-(U32)u16_SectorIdxInPage) ? ((U32)u16_PageSectorCnt-(U32)u16_SectorIdxInPage) : (u32_SectorCnt);

		memcpy((au8_PageBuf+(u16_SectorIdxInPage<<9)), pu8_DestAddr, (u16_SectorCntRead<<9));
		u32_BakPageIdx = (u16_BakBlkIdx << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>u8_PageSectorCntBits);
		u32_Err = NC_WritePages(u32_BakPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Writing page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_BakPageIdx, u32_Err);
			drvNAND_MarkBadBlk(u16_BakBlkIdx);
			return u32_Err;
		}

		pu8_DestAddr += (u16_SectorCntRead<<9);
		u32_StartSector += u16_SectorCntRead;
		u32_SectorCnt -= u16_SectorCntRead;
		u16_LastWrittenPage = (u32_StartSector>>u8_PageSectorCntBits);
	}

	/* Read aligned sectors then */
	memset(au8_SpareBuf, '\0', sizeof(*pBlkInfo));
	pBlkInfo->u8_BadBlkMark = 0xFF;
	pBlkInfo->u8_PartType = u8_PartType;
	pBlkInfo->u16_BlkAddr = u16_LBA;

	u32_BakPageIdx = (u16_BakBlkIdx << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>u8_PageSectorCntBits);
	u16_PageCnt = (u32_SectorCnt>>u8_PageSectorCntBits);
	while (u16_PageCnt)
	{
		u32_Err = NC_WritePages(u32_BakPageIdx, pu8_DestAddr, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Writing page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_BakPageIdx, u32_Err);
			drvNAND_MarkBadBlk(u16_BakBlkIdx);
			return u32_Err;
		}

		u16_LastWrittenPage = u32_BakPageIdx -(u16_BakBlkIdx << pNandDrv->u8_BlkPageCntBits);
		pu8_DestAddr += pNandDrv->u16_PageByteCnt;
		u32_BakPageIdx++;
		u16_PageCnt--;
		u32_StartSector += u16_PageSectorCnt;
		u32_SectorCnt -= u16_PageSectorCnt;
	}

	/* Read remaining unaligned sectors finally */
	if( u32_SectorCnt )
	{
		u32_SrcPageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>u8_PageSectorCntBits);
		u32_Err = NC_ReadPages(u32_SrcPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_SrcPageIdx, u32_Err);

			return u32_Err;
		}

		memcpy(au8_PageBuf, pu8_DestAddr, (u32_SectorCnt<<9));
		u32_BakPageIdx = (u16_BakBlkIdx << pNandDrv->u8_BlkPageCntBits) + (u32_StartSector>>u8_PageSectorCntBits);
		u32_Err = NC_WritePages(u32_BakPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS) {

			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Writing page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_BakPageIdx, u32_Err);
			drvNAND_MarkBadBlk(u16_BakBlkIdx);
			return u32_Err;
		}
		u16_LastWrittenPage = (u32_StartSector>>u8_PageSectorCntBits);
	}

	for(u16_i=(u16_LastWrittenPage+1) ; u16_i<pNandDrv->u16_BlkPageCnt; u16_i++)
	{
		u32_SrcPageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + u16_i;
		u32_Err = NC_ReadPages(u32_SrcPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_SrcPageIdx, u32_Err);

			return u32_Err;
		}

		u32_BakPageIdx = (u16_BakBlkIdx << pNandDrv->u8_BlkPageCntBits) + u16_i;
		u32_Err = NC_WritePages(u32_BakPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Writing page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_BakPageIdx, u32_Err);
			drvNAND_MarkBadBlk(u16_BakBlkIdx);
			return u32_Err;
		}
	}

	// erase src block
	u32_Err = drvNAND_ErasePhyBlk(u16_PBA);
	if (u32_Err != UNFD_ST_SUCCESS) {
        nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Erase Blk 0x%04x failed with EC: 0x%08x\n",
            (unsigned int)u16_PBA, (unsigned int)u32_Err);
		drvNAND_MarkBadBlk(u16_PBA);
		return u32_Err;
	}

	// copy bak to src
	for(u16_i=0 ; u16_i<pNandDrv->u16_BlkPageCnt; u16_i++)
	{
		u32_BakPageIdx = (u16_BakBlkIdx << pNandDrv->u8_BlkPageCntBits) + u16_i;
		u32_Err = NC_ReadPages(u32_BakPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_BakPageIdx, u32_Err);

			return u32_Err;
		}

		u32_SrcPageIdx = (u16_PBA << pNandDrv->u8_BlkPageCntBits) + u16_i;
		u32_Err = NC_WritePages(u32_SrcPageIdx, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Reading page 0x%04lx failed with EC: 0x%04lx\n",
					   u32_SrcPageIdx, u32_Err);
			drvNAND_MarkBadBlk(u16_PBA);
			return u32_Err;
		}
	}

	// erase bak block
	u32_Err = drvNAND_ErasePhyBlk(u16_BakBlkIdx);
	if (u32_Err != UNFD_ST_SUCCESS) {
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Erase Blk 0x%04x failed with EC: 0x%08lx\n",
				   u16_BakBlkIdx, u32_Err);
		drvNAND_MarkBadBlk(u16_BakBlkIdx);
		return u32_Err;
	}

	return UNFD_ST_SUCCESS;
}

U32 drvNAND_MarkBadBlk(U16 u16_PBA)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
    #if 1 //NAND_BUF_USE_STACK
	//U8 au8_PageBuf[4096];
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 au8_SpareBuf[128];
    #else
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
    #endif
	U32 u32_Err;
	U32 u32_PageIdx;

	u32_Err = drvNAND_ErasePhyBlk(u16_PBA);

	//memset(au8_PageBuf, '\0', pNandDrv->u16_PageByteCnt);
	memset(au8_SpareBuf, '\0', 128);//pNandDrv->u16_SpareByteCnt);

	/* Clear the 1st page (main + spare) of the block */
	u32_PageIdx = u16_PBA << pNandDrv->u8_BlkPageCntBits;
	u32_Err = NC_WritePages(u32_PageIdx, au8_PageBuf, au8_SpareBuf, 1);
	if (u32_Err != UNFD_ST_SUCCESS)
        nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Mark Blk 0x%04x failed with ErrCode: 0x%08x\n",
            (unsigned int)u16_PBA, (unsigned int)u32_Err);
	/* Clear the last page (main + spare) of the block */
	u32_PageIdx += pNandDrv->u16_BlkPageCnt - 1;
	u32_Err = NC_WritePages(u32_PageIdx, au8_PageBuf, au8_SpareBuf, 1);
	if (u32_Err != UNFD_ST_SUCCESS)
        nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Mark Blk 0x%04x failed with ErrCode: 0x%08x\n",
            (unsigned int)u16_PBA, (unsigned int)u32_Err);

	return u32_Err;
}

U32 drvNAND_ErasePhyBlk(U16 u16_PBA)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();

	return NC_EraseBlk (u16_PBA << pNandDrv->u8_BlkPageCntBits);
}


U32 drvNAND_ReadPhyPage(U32 u32_PageIdx, U8 *pu8_Data, U8 *pu8_Spare)
{
	return NC_ReadPages(u32_PageIdx, pu8_Data, pu8_Spare, 1);
}

U32 drvNAND_WritePhyPage(U32 u32_PageIdx, U8 *pu8_Data, U8 *pu8_Spare)
{
	return NC_WritePages(u32_PageIdx, pu8_Data, pu8_Spare, 1);
}


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


#include "eMMC.h"


#if defined(UNIFIED_eMMC_DRIVER) && UNIFIED_eMMC_DRIVER

static U32 eMMC_ReadPartitionInfo_Ex(void);
static U32 eMMC_GetPartitionIndex(U16 u16_PartType, 
							               U32 u32_LogicIdx,
							               volatile U16 *pu16_PartIdx);

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
int fbase[] = {
	10000,
	100000,
	1000000,
	10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
int multipliers[] = {
	0,	/* reserved */
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

//========================================================
U32 eMMC_LoadImages(U32 *pu32_Addr, U32 *pu32_SectorCnt, U32 u32_ItemCnt)
{
	U32 u32_err, u32_i;
	U16 u16_reg, u16_retry=0;

	// --------------------------------	
	eMMC_PlatformInit();
	eMMC_clock_setting(FCIE_SLOW_CLK);
	//*(U16*)0x25020DD4 &= ~BIT6; // FPGA only

	LABEL_BOOT_MODE_START:
	u32_err = eMMC_FCIE_Init();
	if(u32_err)
        goto  LABEL_LOAD_IMAGE_END;

	// --------------------------------
	eMMC_RST_L();
	eMMC_hw_timer_delay(HW_TIMER_DELAY_1ms);
	eMMC_RST_H(); 
	
	u32_i = 0;
	while(u32_i < u32_ItemCnt)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"i:%u  SecCnt:%Xh \n", u32_i, pu32_SectorCnt[u32_i]);
		eMMC_FCIE_ClearEvents();
		REG_FCIE_W(FCIE_SD_MODE, g_eMMCDrv.u16_Reg10_Mode);
		REG_FCIE_W(FCIE_JOB_BL_CNT, pu32_SectorCnt[u32_i]);
		#if FICE_BYTE_MODE_ENABLE
		REG_FCIE_W(FCIE_SDIO_ADDR0, pu32_Addr[u32_i] & 0xFFFF);
		REG_FCIE_W(FCIE_SDIO_ADDR1, pu32_Addr[u32_i] >> 16);
		#else
        REG_FCIE_W(FCIE_MIU_DMA_15_0, (pu32_Addr[u32_i]>>3) & 0xFFFF);
		REG_FCIE_W(FCIE_MIU_DMA_26_16,(pu32_Addr[u32_i]>>3) >> 16);
		#endif
		REG_FCIE_CLRBIT(FCIE_MMA_PRI_REG, BIT_DMA_DIR_W);
		u32_err = eMMC_FCIE_FifoClkRdy(0);
		if(u32_err)
			goto  LABEL_LOAD_IMAGE_END;
		REG_FCIE_SETBIT(FCIE_PATH_CTRL, BIT_MMA_EN);

		if(0 == u32_i) // stg.1
		{
			REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_BOOT_MODE_EN);
		}
		else // stg.2
		{
			// can NOT set BIT_SD_DAT_EN
			//REG_FCIE_SETBIT(FCIE_SD_CTRL, BIT_SD_DAT_EN);
		    REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_BOOT_STG2_EN);
		}

		u32_err = eMMC_FCIE_PollingEvents(FCIE_MIE_EVENT, 
			BIT_MIU_LAST_DONE, HW_TIMER_DELAY_1s);

		// check status
		REG_FCIE_R(FCIE_SD_STATUS, u16_reg);
		if((0==u16_retry) && (eMMC_ST_SUCCESS!=u32_err || (u16_reg & BIT_SD_R_CRC_ERR)))
		{
			u16_retry++;
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC retry, reg.12h: %Xh\n", u16_reg);
			//eMMC_clock_setting(FCIE_SLOWEST_CLK);
			#if 0
			eMMC_DumpDriverStatus();
		    eMMC_DumpPadClk();
		    eMMC_FCIE_DumpRegisters();
		    eMMC_FCIE_DumpDebugBus();		
			#endif
			goto LABEL_BOOT_MODE_START;
		}
		#if 0
		else if(u16_retry && (eMMC_ST_SUCCESS!=u32_err || (u16_reg & BIT_SD_R_CRC_ERR)))
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC retry fail, reg.12h: %Xh\n", u16_reg);
			//eMMC_clock_setting(FCIE_SLOWEST_CLK);
			eMMC_DumpDriverStatus();
		    eMMC_DumpPadClk();
		    eMMC_FCIE_DumpRegisters();
		    eMMC_FCIE_DumpDebugBus();		
		}
		#endif
		u32_i++;		
	}

	// --------------------------------
	// boot end
	REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_BOOT_END_EN);
	u32_err = eMMC_FCIE_PollingEvents(FCIE_MIE_EVENT, 
			BIT_CARD_BOOT_DONE, HW_TIMER_DELAY_1s);
	if(u32_err)
		goto  LABEL_LOAD_IMAGE_END;
	
    return eMMC_ST_SUCCESS;
	
	LABEL_LOAD_IMAGE_END:
	eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: %Xh\n", u32_err);
	return u32_err;	
}



//========================================================
U32 eMMC_GetID(U8 *pu8IDByteCnt, U8 *pu8ID)
{
	eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "\n");

	*pu8IDByteCnt = g_eMMCDrv.u8_IDByteCnt;
	memcpy(pu8ID, g_eMMCDrv.au8_ID, eMMC_ID_BYTE_CNT);

	return eMMC_ST_SUCCESS;
}

U32 eMMC_CheckCIS(eMMC_CIS_t *ptCISData)
{
	eMMC_NNI_t *peMMCInfo = (eMMC_NNI_t*)&ptCISData->au8_eMMC_nni[0];
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)&ptCISData->au8_eMMC_pni[0];
	U32 u32_ChkSum;

	eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "\n");

	if (eMMC_CompareCISTag(peMMCInfo->au8_Tag)) {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: NNI Tag mismatch\n");
		return eMMC_ST_ERR_CIS_NNI;
	}

	u32_ChkSum = eMMC_ChkSum((U8 *)&peMMCInfo->u16_SpareByteCnt, (U16)((U32)(&peMMCInfo->u16_SeqAccessTime)-(U32)(&peMMCInfo->u16_SpareByteCnt)));
	if (u32_ChkSum != peMMCInfo->u32_ChkSum) {

		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: NNI chksum mismatch: 0x%08X, 0x%08X\n",
				   u32_ChkSum, peMMCInfo->u32_ChkSum);

		eMMC_dump_mem((unsigned char *)peMMCInfo, 0x200);
		return eMMC_ST_ERR_CIS_NNI;
	}

	u32_ChkSum = eMMC_ChkSum((U8 *)&pPartInfo->u16_SpareByteCnt, 0x200-0x04);
	if (u32_ChkSum != pPartInfo->u32_ChkSum) {

		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: pni chksum mismatch: 0x%08X, 0x%08X\n",
				   u32_ChkSum, pPartInfo->u32_ChkSum);

		eMMC_dump_mem((unsigned char *)pPartInfo, 0x200);
		return eMMC_ST_ERR_CIS_PNI;
	}

	eMMC_dump_nni(peMMCInfo);
	eMMC_dump_pni(pPartInfo);

	return eMMC_ST_SUCCESS;
}


U32 eMMC_WriteCIS(eMMC_CIS_t *ptCISData)
{
	U32 u32_err;
	U16 u16_i;
	eMMC_NNI_t *peMMCInfo = (eMMC_NNI_t*)&ptCISData->au8_eMMC_nni[0];
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)&ptCISData->au8_eMMC_pni[0];
	
	eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "\n");
	u32_err = eMMC_CheckCIS(ptCISData);
	if(eMMC_ST_SUCCESS != u32_err)
		return u32_err;

	// ----------------------------
	// write NNI in Blk0 and Blk1
	for(u16_i=0; u16_i<eMMC_CIS_NNI_BLK_CNT; u16_i++)
	{
		u32_err = eMMC_CMD24(eMMC_NNI_BLK_0+u16_i, (U8*)peMMCInfo);
	    if(eMMC_ST_SUCCESS != u32_err)
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: write NNI %u fail, %Xh\n", 
			    u16_i, u32_err);
		    return u32_err;
	    }
	}

	// ----------------------------
	// write PNI in Blk2 and Blk3
	memcpy(gau8_eMMC_PartInfoBuf, pPartInfo, eMMC_SECTOR_512BYTE);
	eMMC_FATAutoSize();
	eMMC_debug(0,1,"Total Sec: %Xh, FAT Sec: %Xh \n", g_eMMCDrv.u32_SEC_COUNT, g_eMMCDrv.u32_FATSectorCnt);
	
	for(u16_i=0; u16_i<eMMC_CIS_PNI_BLK_CNT; u16_i++)
	{
		u32_err = eMMC_CMD24(eMMC_PNI_BLK_0+u16_i, (U8*)pPartInfo);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
				"eMMC Err: write PNI %u fail, %Xh\n", 
				u16_i, u32_err);
			return u32_err;
		}
	}
	
	// write linux partition info
	#if defined(eMMC_FCIE_LINUX_DRIVER) && eMMC_FCIE_LINUX_DRIVER
	eMMC_SearchDevNodeStartSector();
	u32_err = eMMC_ErasePartition(eMMC_PART_DEV_NODE);
	if(u32_err){
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: erase PART_DEV_NODE fail: %Xh \n", u32_err);
		return u32_err;
	}		
	u32_err = add_emmc_partitions(NULL, NULL);
    if(u32_err){
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: add_emmc_partitions fail: %Xh \n", u32_err);
		return u32_err;
	}		
	#endif

	memcpy(g_eMMCDrv.au8_Vendor, peMMCInfo->au8_Vendor, 16);
    memcpy(g_eMMCDrv.au8_PartNumber, peMMCInfo->au8_PartNumber, 16);
	
	return u32_err;
}


// read CIS and set config to UNFD
U32 eMMC_ReadCIS(eMMC_CIS_t *ptCISData)
{
	U32 u32_err;
	U16 u16_i;
	eMMC_NNI_t *peMMCInfo = (eMMC_NNI_t*)&ptCISData->au8_eMMC_nni[0];
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)&ptCISData->au8_eMMC_pni[0];
	
	eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "\n");
	
	// read NNI in Blk0 and Blk1
	for(u16_i=0; u16_i<eMMC_CIS_NNI_BLK_CNT; u16_i++)
	{
		u32_err = eMMC_CMD17(eMMC_NNI_BLK_0+u16_i, (U8*)peMMCInfo);
	    if(eMMC_ST_SUCCESS != u32_err)
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: read NNI %u fail, %Xh\n", 
			    u16_i, u32_err);
		    continue;
	    }
		break;
	}
	if(eMMC_CIS_NNI_BLK_CNT == u16_i)
		return eMMC_ST_ERR_CIS_NNI_NONA;

	// read PNI in Blk2 and Blk3
	for(u16_i=0; u16_i<eMMC_CIS_PNI_BLK_CNT; u16_i++)
	{
		u32_err = eMMC_CMD17(eMMC_PNI_BLK_0+u16_i, (U8*)pPartInfo);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
				"eMMC Err: read PNI %u fail, %Xh\n", 
				u16_i, u32_err);
			continue;
		}
		break;
	}
	if(eMMC_CIS_PNI_BLK_CNT == u16_i)
		return eMMC_ST_ERR_CIS_PNI_NONA;
	
	memcpy(gau8_eMMC_PartInfoBuf, pPartInfo, eMMC_SECTOR_512BYTE);	
	eMMC_FATAutoSize();
	return u32_err;
}


U32 eMMC_DumpPartitionInfo(void)
{
	U32 u32_err;
	U16 u16_i;
	U32 u32_PniBlkByteCnt, u32_PniBlkCnt;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;

	if(0==(g_eMMCDrv.u32_DrvFlag & DRV_FLAG_GET_PART_INFO))
	{
		u32_err = eMMC_ReadPartitionInfo_Ex();
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: no Partition Info\n");
			return u32_err;
		}
	}
	
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"ChkSum:       %08Xh \n", pPartInfo->u32_ChkSum);
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"SpareByteCnt: %04Xh \n", pPartInfo->u16_SpareByteCnt);
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"PageByteCnt:  %04Xh \n", pPartInfo->u16_PageByteCnt);
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"BlkPageCnt:   %04Xh \n", pPartInfo->u16_BlkPageCnt);
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"BlkCnt:       %04Xh \n", pPartInfo->u16_BlkCnt);
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"PartitionCnt: %04Xh \n", pPartInfo->u16_PartCnt);
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"RecByteCnt:   %04Xh \n", pPartInfo->u16_UnitByteCnt);

	u32_PniBlkByteCnt = pPartInfo->u16_PageByteCnt * pPartInfo->u16_BlkPageCnt;
	u32_PniBlkCnt = 0;
	
	for(u16_i=0; u16_i<pPartInfo->u16_PartCnt; u16_i++)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"\n");
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"Partition:   %02u \n",  u16_i);
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"StartBlk:    %04Xh \n", pPartInfo->records[u16_i].u16_StartBlk);
	    eMMC_debug(eMMC_DEBUG_LEVEL,1,"BlkCnt:      %04Xh \n", pPartInfo->records[u16_i].u16_BlkCnt);
	    eMMC_debug(eMMC_DEBUG_LEVEL,1,"PartType:    %04Xh \n", pPartInfo->records[u16_i].u16_PartType);
	    eMMC_debug(eMMC_DEBUG_LEVEL,1,"BackupBlkCnt:%04Xh \n", pPartInfo->records[u16_i].u16_BackupBlkCnt);
		
		if(u16_i < pPartInfo->u16_PartCnt-1)
		{
		    eMMC_debug(eMMC_DEBUG_LEVEL,1,"Capacity:    %u KB\n",  
			    (u32_PniBlkByteCnt * pPartInfo->records[u16_i].u16_BlkCnt)>>10);

			u32_PniBlkCnt += pPartInfo->records[u16_i].u16_BlkCnt + 
				             pPartInfo->records[u16_i].u16_BackupBlkCnt;
		}
	}
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"Capacity:    %u MB\n",
		(g_eMMCDrv.u32_SEC_COUNT - (u32_PniBlkCnt*u32_PniBlkByteCnt>>9))>>11);

	return eMMC_ST_SUCCESS;
}


// data length is 512 bytes
U32 eMMC_FATAutoSize(void)
{
	U16 u16_i;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;
	volatile U32 u32_PartSectorCnt=0;

	// calculate FAT sector cnt
	for(u16_i=0; u16_i<pPartInfo->u16_PartCnt; u16_i++)
	{
		if(eMMC_PART_FAT == pPartInfo->records[u16_i].u16_PartType)
			u32_PartSectorCnt = 
			    pPartInfo->records[u16_i].u16_StartBlk * pPartInfo->u16_BlkPageCnt;

		else if(u32_PartSectorCnt)
			u32_PartSectorCnt += 
			    (pPartInfo->records[u16_i].u16_BlkCnt + pPartInfo->records[u16_i].u16_BackupBlkCnt)
			    * pPartInfo->u16_BlkPageCnt;
	}	

	g_eMMCDrv.u32_FATSectorCnt = g_eMMCDrv.u32_SEC_COUNT - u32_PartSectorCnt;

    u32_PartSectorCnt = 0;
	// shift the StartBlk for partitions following FAT
	for(u16_i=0; u16_i<pPartInfo->u16_PartCnt; u16_i++)
	{
		if(eMMC_PART_FAT == pPartInfo->records[u16_i].u16_PartType)
		{
			pPartInfo->records[u16_i].u16_BlkCnt = 
				g_eMMCDrv.u32_FATSectorCnt / pPartInfo->u16_BlkPageCnt;
			
			u32_PartSectorCnt = 
			    pPartInfo->records[u16_i].u16_StartBlk + 
			    pPartInfo->records[u16_i].u16_BlkCnt + 
			    pPartInfo->records[u16_i].u16_BackupBlkCnt;
		}
		else if(u32_PartSectorCnt)
		{
			pPartInfo->records[u16_i].u16_StartBlk = u32_PartSectorCnt;
			
			u32_PartSectorCnt += 
			    pPartInfo->records[u16_i].u16_BlkCnt + 
			    pPartInfo->records[u16_i].u16_BackupBlkCnt;
		}
	}	

	return eMMC_ST_SUCCESS;
}


static U32 eMMC_ReadPartitionInfo_Ex(void)
{
	U32 u32_err;
	U16 u16_i;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}
	
	// read Partition Info from Blk0 and Blk1 (check)
	for(u16_i=0; u16_i<eMMC_CIS_PNI_BLK_CNT; u16_i++)
	{
		u32_err = eMMC_CMD17(eMMC_PNI_BLK_0+u16_i, gau8_eMMC_PartInfoBuf);
	    if(eMMC_ST_SUCCESS != u32_err)
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: %Xh, eMMC, read Part Info %u fail, %Xh\n", 
			    u32_err, u16_i, u32_err);
		    continue;
	    }
		
		if(0==pPartInfo->u32_ChkSum || pPartInfo->u32_ChkSum != eMMC_ChkSum((U8*)&(pPartInfo->u16_SpareByteCnt), 0x200-4))
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Part Info %u ChkSum failed: %Xh %Xh\n",
			    u16_i, pPartInfo->u32_ChkSum, eMMC_ChkSum((U8*)&(pPartInfo->u16_SpareByteCnt), 0x200-4));	        
	    }
		else
		{
			eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC: found Part Info %u\n", u16_i);
			break;
		}
	}

	// print msg
	if(eMMC_CIS_PNI_BLK_CNT== u16_i)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: no valid Partition Info\n");
		return eMMC_ST_ERR_NO_PART_INFO;
	}
	else{
		// calculate FAT capacity
		eMMC_FATAutoSize();
		//eMMC_debug(0,1,"Total Sec: %Xh, FAT Sec: %Xh \n", g_eMMCDrv.u32_SEC_COUNT, g_eMMCDrv.u32_FATSectorCnt);
	}

	g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_GET_PART_INFO;
	return u32_err;
}


// data length is 512 bytes
U32 eMMC_ReadPartitionInfo(U8 *pu8_Data)
{
	U32 u32_err;
	
	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}

	if(0 == (g_eMMCDrv.u32_DrvFlag & DRV_FLAG_GET_PART_INFO))
	{
		u32_err = eMMC_ReadPartitionInfo_Ex();
		if(eMMC_ST_SUCCESS != u32_err){
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_ReadPartitionInfo_Ex fail: %Xh", u32_err);
			return u32_err;
		}
	}

	memcpy(pu8_Data, gau8_eMMC_PartInfoBuf, 0x200);	
	
	return eMMC_ST_SUCCESS;	
}


U32 eMMC_WritePartitionInfo(U8 *pu8_Data, U32 u32_ByteCnt)
{
	U32 u32_err;
	U16 u16_SecCnt, u16_i;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)pu8_Data;
	
	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}
	
	u16_SecCnt = (u32_ByteCnt>>eMMC_SECTOR_512BYTE_BITS) + (u32_ByteCnt&eMMC_SECTOR_512BYTE_MASK);
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC input Part Info keeps %u sectors\n", u16_SecCnt);
	
	// search eMMC Partiton Info from pni (pu8_Data)
	for(u16_i=0; u16_i<u16_SecCnt; u16_i++)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_HIGH,1,"%Xh %Xh\n", pPartInfo->u16_SpareByteCnt, pPartInfo->u16_BlkCnt);
		if(1 == pPartInfo->u16_SpareByteCnt)
			break;
		pPartInfo = (eMMC_PNI_t*)((U32)pPartInfo+0x200);
	}
	if(u16_i == u16_SecCnt)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: no valid Part Info for eMMC\n");
	    return eMMC_ST_ERR_INVALID_PARAM;
	}
	if(pPartInfo->u32_ChkSum != eMMC_ChkSum((U8*)&(pPartInfo->u16_SpareByteCnt), 0x200-4))
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Part Info ChkSum failed for eMMC, %Xh %Xh\n",
			pPartInfo->u32_ChkSum, eMMC_ChkSum((U8*)&(pPartInfo->u16_SpareByteCnt), 0x200-4));
	    return eMMC_ST_ERR_PARTITION_CHKSUM;
	}

	// write Partition Info in Blk0 and Blk1 (as MBR)
	for(u16_i=0; u16_i<eMMC_CIS_PNI_BLK_CNT; u16_i++)
	{
		u32_err = eMMC_CMD24(eMMC_PNI_BLK_0+u16_i, (U8*)pPartInfo);
	    if(eMMC_ST_SUCCESS != u32_err)
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: write Part Info %u fail, %Xh\n", 
			    u16_i, u32_err);
		    return u32_err;
	    }
	}

	// read Partition Info from Blk0 and Blk1 (check)
	for(u16_i=0; u16_i<eMMC_CIS_PNI_BLK_CNT; u16_i++)
	{
		u32_err = eMMC_CMD17(eMMC_PNI_BLK_0+u16_i, gau8_eMMC_PartInfoBuf);
	    if(eMMC_ST_SUCCESS != u32_err)
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: read Part Info %u fail, %Xh\n", 
			    u16_i, u32_err);
		    return u32_err;
	    }

		u32_err = eMMC_ComapreData((U8*)pPartInfo, gau8_eMMC_PartInfoBuf, eMMC_SECTOR_512BYTE);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: check Part Info %u fail, %Xh\n", 
			    u16_i, u32_err);
			eMMC_debug(0,1,"dump W data: %Xh\n", (U32)pPartInfo);
			eMMC_dump_mem((U8*)pPartInfo, 0x200);
			eMMC_debug(0,1,"dump R data: %Xh\n", (U32)gau8_eMMC_PartInfoBuf);
			eMMC_dump_mem(gau8_eMMC_PartInfoBuf, 0x200);
			return u32_err;
		}
	}

	eMMC_FATAutoSize();
	#if defined(eMMC_FCIE_LINUX_DRIVER) && eMMC_FCIE_LINUX_DRIVER
	eMMC_SearchDevNodeStartSector();
	u32_err = eMMC_ErasePartition(eMMC_PART_DEV_NODE);
	if(u32_err){
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: erase PART_DEV_NODE fail: %Xh \n", u32_err);
		return u32_err;
	}		
	u32_err = add_emmc_partitions(NULL, NULL);
    if(u32_err){
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: add_emmc_partitions fail: %Xh \n", u32_err);
		return u32_err;
	}
	#endif
	

	eMMC_debug(eMMC_DEBUG_LEVEL,1,"ok\n");
	return u32_err;
}


static U32 eMMC_GetPartitionIndex(U16 u16_PartType, 
							   U32 u32_LogicIdx,
							   volatile U16 *pu16_PartIdx)
{
	U32 u32_err;
	U16 u16_PartCopy=0;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;
		
	if(0==(g_eMMCDrv.u32_DrvFlag & DRV_FLAG_GET_PART_INFO))
	{
		u32_err = eMMC_ReadPartitionInfo_Ex();
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: no Partition Info\n");
			return u32_err;
		}
	}

	// search partition
	for(*pu16_PartIdx=0; *pu16_PartIdx<pPartInfo->u16_PartCnt; *pu16_PartIdx+=1)
	{	
		if(u16_PartType == pPartInfo->records[*pu16_PartIdx].u16_PartType)
		{
			if(u16_PartCopy == u32_LogicIdx)
				break;
			u16_PartCopy++;
		}
	}
	if(*pu16_PartIdx == pPartInfo->u16_PartCnt)
	{
		if(0 == u16_PartCopy)
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: unknown Partition ID: %Xh\n", u16_PartType);
		else
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC WARN: no %u Partition ID: %Xh \n", 
		        u32_LogicIdx+1, u16_PartType);
		
		return eMMC_ST_ERR_NO_PARTITION;
	}

	return eMMC_ST_SUCCESS;
}


U32 eMMC_GetPartitionCapacity (U16 u16_PartType, U32 *pu32_Cap)
{
	U32 u32_err;
	volatile U16 u16_PartIdx;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}
	
	if(eMMC_PART_FAT == u16_PartType)
		*pu32_Cap = g_eMMCDrv.u32_FATSectorCnt;
	else
	{
		u32_err = eMMC_GetPartitionIndex(u16_PartType, 
			0, &u16_PartIdx);
		if(eMMC_ST_SUCCESS != u32_err)
			return u32_err;

		*pu32_Cap = pPartInfo->records[u16_PartIdx].u16_BlkCnt * pPartInfo->u16_BlkPageCnt;
	}

	return eMMC_ST_SUCCESS;
}


U32 eMMC_ReadPartition(U16 u16_PartType, 
								U8 *pu8_DataBuf, 
								U32 u32_StartSector, 
								U32 u32_SectorCnt,
								U32 u32_LogicIdx)
{
	U32 u32_err, u32_PartSecCnt;
	volatile U16 u16_PartIdx;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;

	//eMMC_debug(eMMC_DEBUG_LEVEL,1,"\n");

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}
	
	u32_err = eMMC_GetPartitionIndex(u16_PartType, 
		u32_LogicIdx, &u16_PartIdx);
	if(eMMC_ST_SUCCESS != u32_err)
		return u32_err;

	// check address
	u32_PartSecCnt = pPartInfo->records[u16_PartIdx].u16_BlkCnt * pPartInfo->u16_BlkPageCnt;
	
	if(u32_StartSector > u32_PartSecCnt || u32_SectorCnt > u32_PartSecCnt ||
		(u32_StartSector+u32_SectorCnt) > u32_PartSecCnt)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: invalid Sector Addr: %Xh, Sector Cnt: %Xh\n",
			u32_StartSector, u32_SectorCnt);
		return eMMC_ST_ERR_INVALID_PARAM;
	}
	
	// read data
	u32_err =  eMMC_ReadData(pu8_DataBuf, 
               u32_SectorCnt<<eMMC_SECTOR_512BYTE_BITS,
               pPartInfo->records[u16_PartIdx].u16_StartBlk*
               pPartInfo->u16_BlkPageCnt+
               u32_StartSector);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_ReadData fail: %Xh\n", u32_err);
		return u32_err;
	}
	
	u32_err = eMMC_BootPartitionHandler_WR(pu8_DataBuf, u16_PartType,
	          u32_StartSector, u32_SectorCnt, eMMC_BOOT_PART_R);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_BootPartitionHandler_R fail: %Xh\n", u32_err);
		return u32_err;
	}
	return u32_err;
}


U32 eMMC_WritePartition(U16 u16_PartType, 
								U8 *pu8_DataBuf, 
								U32 u32_StartSector, 
								U32 u32_SectorCnt,
								U32 u32_LogicIdx)
{
	U32 u32_err, u32_PartSecCnt;
	volatile U16 u16_PartIdx;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}
	
	u32_err = eMMC_GetPartitionIndex(u16_PartType, 
		u32_LogicIdx, &u16_PartIdx);
	if(eMMC_ST_SUCCESS != u32_err)
		return u32_err;

	// check address
	u32_PartSecCnt = pPartInfo->records[u16_PartIdx].u16_BlkCnt * pPartInfo->u16_BlkPageCnt;
	
	if(u32_StartSector > u32_PartSecCnt || u32_SectorCnt > u32_PartSecCnt ||
		(u32_StartSector+u32_SectorCnt) > u32_PartSecCnt)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: invalid Sector Addr: %Xh, Sector Cnt: %Xh\n",
			u32_StartSector, u32_SectorCnt);
		return eMMC_ST_ERR_INVALID_PARAM;
	}
	
	// write data
	u32_err =  eMMC_WriteData(pu8_DataBuf, 
               u32_SectorCnt<<eMMC_SECTOR_512BYTE_BITS,
               pPartInfo->records[u16_PartIdx].u16_StartBlk*
               pPartInfo->u16_BlkPageCnt+
               u32_StartSector);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_WriteData fail: %Xh\n", u32_err);
		return u32_err;
	}
	
	u32_err = eMMC_BootPartitionHandler_WR(pu8_DataBuf, u16_PartType,
	          u32_StartSector, u32_SectorCnt, eMMC_BOOT_PART_W);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_BootPartitionHandler_W fail: %Xh\n", u32_err);
		return u32_err;
	}
	
	return u32_err;	
}


U32 eMMC_GetDevInfo(eMMC_INFO_t *peMMCInfo_t)
{
	const char *str = "MSTARSEMIUNFDCIS";

    memcpy(peMMCInfo_t->au8_Tag, 	str, 16);
    peMMCInfo_t->u8_IDByteCnt = g_eMMCDrv.u8_IDByteCnt;
    memcpy(peMMCInfo_t->au8_ID, g_eMMCDrv.au8_ID, g_eMMCDrv.u8_IDByteCnt);
    peMMCInfo_t->u16_SpareByteCnt = 1;
    peMMCInfo_t->u16_PageByteCnt = eMMC_SECTOR_512BYTE;
    peMMCInfo_t->u16_BlkPageCnt = g_eMMCDrv.u32_EraseUnitSize;
    peMMCInfo_t->u16_BlkCnt = g_eMMCDrv.u32_SEC_COUNT / g_eMMCDrv.u32_EraseUnitSize;
    peMMCInfo_t->u32_ChkSum = eMMC_ChkSum((U8*)&peMMCInfo_t->u16_SpareByteCnt, 0x32 - 0x24);

    memcpy(peMMCInfo_t->au8_Vendor, g_eMMCDrv.au8_Vendor, 16);
    memcpy(peMMCInfo_t->au8_PartNumber, g_eMMCDrv.au8_PartNumber, 16);

    return eMMC_ST_SUCCESS;
}


U32 eMMC_ErasePartition(U16 u16_PartType)
{
	U32 u32_err, u32_PartLogiIdx;
	volatile U16 u16_PartIdx;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;
	U32 u32_eMMCStartBlk, u32_eMMCEndBlk;

	//eMMC_debug(eMMC_DEBUG_LEVEL,1,"\n");

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}

	u32_PartLogiIdx = 0;
	while(1) // find all partition copies
	{		
		u32_err = eMMC_GetPartitionIndex(u16_PartType, 
			u32_PartLogiIdx, &u16_PartIdx);
		if(eMMC_ST_SUCCESS != u32_err)
    		break;
		u32_PartLogiIdx++;

		u32_eMMCStartBlk = pPartInfo->records[u16_PartIdx].u16_StartBlk
			* (pPartInfo->u16_PageByteCnt>>eMMC_SECTOR_512BYTE_BITS)
			* pPartInfo->u16_BlkPageCnt;

		u32_eMMCEndBlk = (pPartInfo->records[u16_PartIdx].u16_StartBlk
			+ pPartInfo->records[u16_PartIdx].u16_BlkCnt)
			* (pPartInfo->u16_PageByteCnt>>eMMC_SECTOR_512BYTE_BITS)
			* pPartInfo->u16_BlkPageCnt;

		u32_err = eMMC_EraseBlock(u32_eMMCStartBlk, u32_eMMCEndBlk-1);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
				"eMMC Err: eMMC_EraseBlock fail: %Xh, PardIdx:%Xh", 
				u32_err, u16_PartIdx);
    		break;
		}
	}

	if(0 == u32_PartLogiIdx)
		return u32_err;

	u32_err = eMMC_BootPartitionHandler_E(u16_PartType);
	if(eMMC_ST_SUCCESS != u32_err)
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_BootPartitionHandler fail: %Xh\n", u32_err);		

    return u32_err;	
}


// erase partitions following eMMC_PART_NVRAMBAK
U32 eMMC_EraseAllPartitions(void)
{
	U32 u32_err, u32_PartLogiIdx;
	volatile U16 u16_PartIdx=0, u16_PartIdx_tmp;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;
	U32 u32_eMMCStartBlk, u32_eMMCEndBlk;

	//eMMC_debug(eMMC_DEBUG_LEVEL,1,"\n");

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}

	u32_PartLogiIdx = 0;
	while(1) // find all partition copies
	{		
		u32_err = eMMC_GetPartitionIndex(eMMC_PART_NVRAMBAK, 
			u32_PartLogiIdx, &u16_PartIdx_tmp);
		if(eMMC_ST_SUCCESS != u32_err)
    		break;

		u16_PartIdx = u16_PartIdx_tmp;
		u32_PartLogiIdx++;
	}

	for(; u16_PartIdx < pPartInfo->u16_PartCnt; u16_PartIdx++)
	{
		// skip some partitions can not be erased
		#if defined(eMMC_FCIE_LINUX_DRIVER) && eMMC_FCIE_LINUX_DRIVER
		if(eMMC_PART_DEV_NODE == pPartInfo->records[u16_PartIdx].u16_PartType ||
		   eMMC_PART_E2PBAK == pPartInfo->records[u16_PartIdx].u16_PartType ||
		   eMMC_PART_NVRAMBAK == pPartInfo->records[u16_PartIdx].u16_PartType)
			continue;
		#endif
		
		u32_eMMCStartBlk = pPartInfo->records[u16_PartIdx].u16_StartBlk
			* (pPartInfo->u16_PageByteCnt>>eMMC_SECTOR_512BYTE_BITS)
			* pPartInfo->u16_BlkPageCnt;

		u32_eMMCEndBlk = (pPartInfo->records[u16_PartIdx].u16_StartBlk
			+ pPartInfo->records[u16_PartIdx].u16_BlkCnt)
			* (pPartInfo->u16_PageByteCnt>>eMMC_SECTOR_512BYTE_BITS)
			* pPartInfo->u16_BlkPageCnt;

		u32_err = eMMC_EraseBlock(u32_eMMCStartBlk, u32_eMMCEndBlk-1);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
				"eMMC Err: eMMC_EraseBlock fail: %Xh, PardIdx:%Xh", 
				u32_err, u16_PartIdx);
    		return u32_err;
		}
	}

	return u32_err;
}


U32 eMMC_EraseAll(void)
{
	U32 u32_err;

	u32_err = eMMC_EraseBlock(0, g_eMMCDrv.u32_SEC_COUNT-1);
	if(eMMC_ST_SUCCESS != u32_err)
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			"eMMC Err: fail: %Xh \n", u32_err);
    
	return u32_err;
}


U32 eMMC_EraseBootPart(U32 u32_eMMCBlkAddr_start, U32 u32_eMMCBlkAddr_end, U8 u8_PartNo)
{
	U32 u32_err, u32_SectorCnt, u32_i, u32_j;

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}

	// set Access Boot Partition 1
	if(u8_PartNo == 1)
	{
		u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3|BIT0);
	}
	else if(u8_PartNo == 2)
	{
		u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3|BIT1); // still boot from BP1
	}
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: %Xh, eMMC, set Ext_CSD[179]: %Xh fail\n", 
			u32_err, BIT6|BIT3|BIT0);
		return u32_err;
	}

	if(g_eMMCDrv.u32_eMMCFlag & eMMC_FLAG_TRIM)
	{
		u32_err =  eMMC_EraseBlock(u32_eMMCBlkAddr_start, u32_eMMCBlkAddr_end);
		if(eMMC_ST_SUCCESS != u32_err)
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_EraseBlock fail: %Xh \n", u32_err);
		    return u32_err;
	    }
	}
	else
	{
		for(u32_i=0; u32_i<eMMC_SECTOR_BUF_BYTECTN; u32_i++)
		    gau8_eMMC_SectorBuf[u32_i] = 0xFF;

		// erase blocks before EraseUnitSize
		u32_SectorCnt = u32_eMMCBlkAddr_start / g_eMMCDrv.u32_EraseUnitSize;
		u32_SectorCnt = (u32_SectorCnt+1) * g_eMMCDrv.u32_EraseUnitSize;
		u32_SectorCnt -= u32_eMMCBlkAddr_start;
		u32_SectorCnt = 
			u32_SectorCnt > (u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start) ?
			(u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start) : u32_SectorCnt;

		for(u32_i=0; u32_i<u32_SectorCnt; u32_i++)
		{
			u32_j = ((u32_SectorCnt-u32_i)<<eMMC_SECTOR_512BYTE_BITS)>eMMC_SECTOR_BUF_BYTECTN?
				eMMC_SECTOR_BUF_BYTECTN:((u32_SectorCnt-u32_i)<<eMMC_SECTOR_512BYTE_BITS);
				
			u32_err = eMMC_WriteData(gau8_eMMC_SectorBuf, u32_j, 
				u32_eMMCBlkAddr_start + u32_i);
			if(eMMC_ST_SUCCESS != u32_err){
		        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: CMD24 fail 0, %Xh\n", u32_err);
		        return u32_err;
	        }

			u32_i += u32_j>>eMMC_SECTOR_512BYTE_BITS;
		}
		if((u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start) == u32_SectorCnt)
			goto LABEL_END_OF_ERASE;

		// erase blocks
        u32_i = 
            (u32_eMMCBlkAddr_end - (u32_eMMCBlkAddr_start+u32_SectorCnt))
            /g_eMMCDrv.u32_EraseUnitSize;
		if(u32_i)
		{
			u32_err = eMMC_EraseBlock(
				(u32_eMMCBlkAddr_start+u32_SectorCnt), 
				(u32_eMMCBlkAddr_start+u32_SectorCnt)+u32_i*g_eMMCDrv.u32_EraseUnitSize);
	        if(eMMC_ST_SUCCESS != u32_err){
		        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: CMD24 fail 0, %Xh\n", u32_err);
		        return u32_err;
	        }
		}
        
        // erase blocks after EraseUnitSize
		u32_eMMCBlkAddr_start = 
			(u32_eMMCBlkAddr_start+u32_SectorCnt) + u32_i*g_eMMCDrv.u32_EraseUnitSize;
		
		while(u32_eMMCBlkAddr_start < u32_eMMCBlkAddr_end)
		{
			u32_j = ((u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start)<<eMMC_SECTOR_512BYTE_BITS)>eMMC_SECTOR_BUF_BYTECTN?
				eMMC_SECTOR_BUF_BYTECTN:((u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start)<<eMMC_SECTOR_512BYTE_BITS);
			
			u32_err = eMMC_WriteData(gau8_eMMC_SectorBuf, u32_j, u32_eMMCBlkAddr_start);
			if(eMMC_ST_SUCCESS != u32_err){
		        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: CMD24 fail 1, %Xh\n", u32_err);
		        return u32_err;
	        }
			u32_eMMCBlkAddr_start += u32_j>>eMMC_SECTOR_512BYTE_BITS;
		}			
	}

	LABEL_END_OF_ERASE:
	// clear Access Boot Partition
	u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: %Xh, eMMC, set Ext_CSD[179]: %Xh fail\n", 
			u32_err, BIT6|BIT3);
		return u32_err;
	}

	return u32_err;		
}

U32 eMMC_WriteBootPart(U8* pu8_DataBuf, U32 u32_DataByteCnt, U32 u32_BlkAddr, U8 u8_PartNo)
{
	U32 u32_err;
	U16 u16_SecCnt, u16_i;

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}

	// set Access Boot Partition 1
	if(u8_PartNo == 1)
	{
		u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3|BIT0);
	}
	else if(u8_PartNo == 2)
	{
		u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3|BIT1); // still boot from BP1
	}
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: %Xh, eMMC, set Ext_CSD[179]: %Xh fail\n", 
			u32_err, BIT6|BIT3|BIT0);
		return u32_err;
	}

	// write Boot Code
	u16_SecCnt = (u32_DataByteCnt>>9) + ((u32_DataByteCnt&0x1FF)?1:0);
	//eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC Boot Data keeps %Xh sectors, ChkSum: %Xh \n", 
	//    u16_SecCnt, eMMC_ChkSum(pu8_DataBuf, u16_SecCnt<<9));
	//    u16_SecCnt, eMMC_ChkSum(pu8_DataBuf, BOOT_PART_TOTAL_CNT<<9));

	u32_err = eMMC_WriteData(pu8_DataBuf, 
                  u16_SecCnt<<eMMC_SECTOR_512BYTE_BITS, 
                  u32_BlkAddr);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
		    "eMMC Err: write Boot Partition fail, %Xh\n", u32_err);
		return u32_err;
	}
	#if 0
	// patch for U01 ROM code (clk not stop)
	for(u16_i=0; u16_i<eMMC_SECTOR_512BYTE; u16_i++)
		gau8_eMMC_SectorBuf[u16_i] = 0xFF;
	u32_err = eMMC_CMD24(BOOT_PART_TOTAL_CNT, gau8_eMMC_SectorBuf);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: U01 patch fail, %Xh\n", u32_err);
		return u32_err;
	}
	#endif
	// verify Boot Code
	for(u16_i=(U16)u32_BlkAddr; u16_i<u16_SecCnt; u16_i++)
	{   
		//eMMC_debug(eMMC_DEBUG_LEVEL,0,"\r checking: %03u%% ", (u16_i+1)*100/u16_SecCnt);
		//u32_err = eMMC_CMD17_CIFD(u16_i<<9, gau8_eMMC_SectorBuf);
		u32_err = eMMC_CMD17(u16_i, gau8_eMMC_SectorBuf);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: read Boot Partition Sector %u fail, %Xh\n", 
			    u16_i, u32_err);
			return u32_err;
		}

		u32_err = eMMC_ComapreData(pu8_DataBuf+(u16_i<<9), gau8_eMMC_SectorBuf, eMMC_SECTOR_512BYTE);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: check Boot Partition Sector %u fail, %Xh\n", 
			    u16_i, u32_err);
			eMMC_dump_mem(gau8_eMMC_SectorBuf, 0x200);
			return u32_err;
		}
	}

	// set Boot Bus
	u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 177, BIT3|BIT1);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: %Xh, eMMC, set Ext_CSD[177]: %Xh fail\n", 
			u32_err, BIT3|BIT1);
		return u32_err;
	}

	// clear Access Boot Partition
	u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: %Xh, eMMC, set Ext_CSD[179]: %Xh fail\n", 
			u32_err, BIT6|BIT3);
		return u32_err;
	}

	// set HW RST 
	u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 162, BIT0);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: %Xh, eMMC, set Ext_CSD[162]: %Xh fail\n", 
			u32_err, BIT0);
		return u32_err;
	}

	//eMMC_debug(eMMC_DEBUG_LEVEL,1,"ok\n");
	return u32_err;
}


U32 eMMC_ReadBootPart(U8* pu8_DataBuf, U32 u32_DataByteCnt, U32 u32_BlkAddr, U8 u8_PartNo)
{
	U32 u32_err;
	U16 u16_SecCnt;

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);
		return u32_err;
	}

	// set Access Boot Partition 1
	if(u8_PartNo == 1)
	{
		u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3|BIT0);
	}
	else if(u8_PartNo == 2)
	{
		u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3|BIT1);
	}
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: %Xh, eMMC, set Ext_CSD[179]: %Xh fail\n", 
			u32_err, BIT6|BIT3|BIT0);
		return u32_err;
	}

	// read Boot Code
	u16_SecCnt = (u32_DataByteCnt>>9) + ((u32_DataByteCnt&0x1FF)?1:0);
	
	u32_err = eMMC_ReadData(pu8_DataBuf, 
                  u16_SecCnt<<eMMC_SECTOR_512BYTE_BITS, 
                  u32_BlkAddr);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
		    "eMMC Err: read Boot Partition fail, %Xh\n", u32_err);
		return u32_err;
	}
	
	// clear Access Boot Partition
	u32_err = eMMC_ModifyExtCSD(eMMC_ExtCSD_WByte, 179, BIT6|BIT3);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: %Xh, eMMC, set Ext_CSD[179]: %Xh fail\n", 
			u32_err, BIT6|BIT3);
		return u32_err;
	}

	return u32_err;
}

// ==============================================================
void eMMC_DumpDriverStatus(void)
{
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC Status:\n");
	
	// ------------------------------------------------------
	// helpful debug info
	// ------------------------------------------------------
	#if defined(ENABLE_eMMC_RIU_MODE)&&ENABLE_eMMC_RIU_MODE
    eMMC_debug(eMMC_DEBUG_LEVEL,1,"  RIU Mode\n");
	#else
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"  MIU Mode\n");
	#endif
	
	#if defined(ENABLE_eMMC_INTERRUPT_MODE)&&ENABLE_eMMC_INTERRUPT_MODE
    eMMC_debug(eMMC_DEBUG_LEVEL,1,"  Interrupt Mode\n");
	#else
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"  Polling Mode\n");
	#endif
	
	#if defined(FICE_BYTE_MODE_ENABLE)&&FICE_BYTE_MODE_ENABLE
    eMMC_debug(eMMC_DEBUG_LEVEL,1,"  FCIE Byte Mode\n");
	#else
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"  FCIE Block Mode\n");
	#endif

	if(g_eMMCDrv.u32_DrvFlag & DRV_FLAG_DDR_MODE)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"  DDR Mode\n");
		eMMC_DumpDDRTTable();
	}
	else
	    eMMC_debug(eMMC_DEBUG_LEVEL,1,"  SDR Mode\n");
	
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"  FCIE Clk: %uKHz\n", g_eMMCDrv.u32_ClkKHz);

	switch(g_eMMCDrv.u8_BUS_WIDTH)
	{
		case BIT_SD_DATA_WIDTH_1:
			eMMC_debug(eMMC_DEBUG_LEVEL,1,"  1-bit ");
			break;
		case BIT_SD_DATA_WIDTH_4:
			eMMC_debug(eMMC_DEBUG_LEVEL,1,"  4-bits ");
			break;
		case BIT_SD_DATA_WIDTH_8:
			eMMC_debug(eMMC_DEBUG_LEVEL,1,"  8-bits ");
			break;
	}
	eMMC_debug(eMMC_DEBUG_LEVEL,0,"width\n");

	switch(g_eMMCDrv.u32_DrvFlag & DRV_FLAG_SPEED_MASK)
	{
		case DRV_FLAG_SPEED_HIGH: 
			eMMC_debug(eMMC_DEBUG_LEVEL,1,"  HIGH");
			break;
		case DRV_FLAG_SPEED_HS200: 
			eMMC_debug(eMMC_DEBUG_LEVEL,1,"  HS200");
			break;
		default:
			eMMC_debug(eMMC_DEBUG_LEVEL,1,"  LOW");
	}
	eMMC_debug(eMMC_DEBUG_LEVEL,0," bus speed\n");	

	eMMC_debug(eMMC_DEBUG_LEVEL,1," PwrOff Notification: ");
	switch(g_eMMCDrv.u32_DrvFlag&DRV_FLAG_PwrOffNotif_MASK)
	{
		case DRV_FLAG_PwrOffNotif_OFF:
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"OFF\n");  break;
		case DRV_FLAG_PwrOffNotif_ON:
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"ON\n");  break;
		case DRV_FLAG_PwrOffNotif_SHORT:
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"SHORT\n");  break;
		case DRV_FLAG_PwrOffNotif_LONG:
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"LONG\n");  break;
		default:
			eMMC_debug(eMMC_DEBUG_LEVEL,0," eMMC Err: unknown: %Xh\n", 
				g_eMMCDrv.u32_DrvFlag&DRV_FLAG_PwrOffNotif_MASK);
	}

	eMMC_DumpBurstLenTable();
	
}


U32 eMMC_Init(void)
{
	U32 u32_err;

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err)
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_Init_Ex fail: %Xh", u32_err);

	return u32_err;
}

#ifdef CONFIG_GENERIC_MMC

extern int fbase[];
extern int multipliers[];


U32 eMMC_GetExtCSD(U8* pu8_Ext_CSD)
{
	return eMMC_CMD8(pu8_Ext_CSD);
}

U32 eMMC_SetExtCSD(U8 u8_AccessMode, U8 u8_ByteIdx, U8 u8_Value)
{
	return eMMC_ModifyExtCSD(u8_AccessMode, u8_ByteIdx,u8_Value);
}
static ulong eMMC_bread(int dev_num, ulong start, lbaint_t blkcnt, void *dst)
{
	U32 u32_Err;
        if( blkcnt == 0)
            return 0;
	u32_Err = eMMC_ReadData(dst, blkcnt << 9, start);
	if (u32_Err == eMMC_ST_SUCCESS)
		return blkcnt;
	else
		return 0;
}

static ulong eMMC_bwrite(int dev_num, ulong start, lbaint_t blkcnt, const void *src)
{
	U32 u32_Err, u32_blkcnt, u32_start = start;
	U8* pu8_src = (U8*) src;
	
	u32_blkcnt = blkcnt;
	
	u32_Err = eMMC_WriteData(pu8_src, u32_blkcnt << 9, u32_start);

	if (u32_Err == eMMC_ST_SUCCESS)
        return blkcnt;
    else
        return 0;
}

static ulong eMMC_berase(int dev_num, ulong start, lbaint_t blkcnt)
{
	
	U32 u32_Err;
	u32_Err = eMMC_EraseBlock (start, start + blkcnt - 1);
	if (u32_Err == eMMC_ST_SUCCESS)
        return blkcnt;
    else
        return 0;
}

int eMMC_mmc_Init(struct mmc* mmc)
{
	
	eMMC_Init();

	sprintf(mmc->name, "Mstar-eMMC");
	mmc->has_init = 1;
	mmc->capacity = (u64)g_eMMCDrv.u32_SEC_COUNT << 9;
	

	eMMC_GetExtCSD((U8*)mmc->ext_csd);

	mmc->high_capacity = g_eMMCDrv.u8_IfSectorMode;
	mmc->bus_width = g_eMMCDrv.u8_BUS_WIDTH << 1;
	
	switch (g_eMMCDrv.u8_SPEC_VERS) {
		case 0:
			mmc->version = MMC_VERSION_1_2;
			break;
		case 1:
			mmc->version = MMC_VERSION_1_4;
			break;
		case 2:
			mmc->version = MMC_VERSION_2_2;
			break;
		case 3:
			mmc->version = MMC_VERSION_3;
			break;
		case 4:
			mmc->version = MMC_VERSION_4;
			break;
		default:
			mmc->version = MMC_VERSION_1_2;
			break;
	}

	/* divide frequency by 10, since the mults are 10x bigger */

	mmc->tran_speed = fbase[( g_eMMCDrv.u8_Tran_Speed & 0x7)] * multipliers[((g_eMMCDrv.u8_Tran_Speed >> 3) & 0xf)];

	    // reliable write is supported
    if ((mmc->ext_csd[EXT_CSD_WR_REL_SET] & 0x01) == 1) // reliable write is configured
        mmc->reliable_write = 2;
    else if((mmc->ext_csd[192] >= 5) && ((mmc->ext_csd[EXT_CSD_WR_REL_PARAM] & 0x1) == 0x1))
	{
        mmc->reliable_write = 1;  // reliable write is supported but not configured
		//printf("[%s]\treliable write is supported\n", __func__);
	}
    else
	{
        mmc->reliable_write = 0;  // reliable write is unsupported
		//printf("[%s]\treliable write is unsupported\n", __func__);
	}

    if ((mmc->ext_csd[192] >= 5) && ((mmc->ext_csd[160] & 0x3) == 3))
    {
        mmc->slc_size = ((u64)(mmc->ext_csd[EXT_CSD_ENH_SIZE_MULT_0]
               | ((u32)(mmc->ext_csd[EXT_CSD_ENH_SIZE_MULT_1]) << 8)
    	       | ((u32)(mmc->ext_csd[EXT_CSD_ENH_SIZE_MULT_2]) << 16)) << 19)
    	       * mmc->ext_csd[221] * mmc->ext_csd[224];
        mmc->max_slc_size = ((u64)(mmc->ext_csd[157]
               | ((u32)(mmc->ext_csd[158]) << 8)
    	       | ((u32)(mmc->ext_csd[159]) << 16)) << 19)
    	       * mmc->ext_csd[221] * mmc->ext_csd[224];
		//printf("[%s]\t slc_size = %lld, max_slc_size = %lld\n",__func__, mmc->slc_size, mmc->max_slc_size);
    }
    else // slc mode is unsupported
    {
        mmc->slc_size = mmc->max_slc_size = 0;
		//printf("[%s]\t slc mode is unsupported\n",__func__);
    }


	{
		U16 u16_i;
		for(u16_i = 0; u16_i < 4; u16_i ++)
		{
			if(u16_i < 3)
				mmc->cid[u16_i] = 
				    g_eMMCDrv.au8_CID[u16_i * 4 + 1] << 24 |  g_eMMCDrv.au8_CID[u16_i * 4 + 2] << 16| 
					g_eMMCDrv.au8_CID[u16_i * 4 + 3] << 8 | g_eMMCDrv.au8_CID[u16_i * 4 + 4];
			else
				mmc->cid[u16_i] = 
				    g_eMMCDrv.au8_CID[u16_i * 4 + 1] << 24 |  g_eMMCDrv.au8_CID[u16_i * 4 + 2] << 16| 
					g_eMMCDrv.au8_CID[u16_i * 4 + 3] << 8;
		}
	}
	
	mmc->read_bl_len = 512;
	mmc->write_bl_len = 512;
	mmc->block_dev.lun = 0;
	mmc->block_dev.type = 0;
	mmc->block_dev.blksz = 512;
	mmc->block_dev.lba = g_eMMCDrv.u32_SEC_COUNT;
	mmc->block_dev.part_type = PART_TYPE_EMMC;
	sprintf(mmc->block_dev.vendor, "Man %06x Snr %08x", mmc->cid[0] >> 8,
			(mmc->cid[2] << 8) | (mmc->cid[3] >> 24));
	sprintf(mmc->block_dev.product, "%c%c%c%c%c", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);
	sprintf(mmc->block_dev.revision, "%d.%d", mmc->cid[2] >> 28,
			(mmc->cid[2] >> 24) & 0xf);	

//	init_part(&mmc->block_dev);
	return 0;
}

int board_mmc_init(bd_t *bis)
{
	struct mmc* mmc = NULL;
	
	mmc = malloc(sizeof(struct mmc)); // uboot origianl design, don't free it.
	if (!mmc)
		return -ENOMEM;
	memset(mmc, 0, sizeof(struct mmc));

	eMMC_mmc_Init(mmc);
	
	mmc_register(mmc);

	mmc->block_dev.block_read = eMMC_bread;
	mmc->block_dev.block_write = eMMC_bwrite;
	mmc->block_dev.block_erase = eMMC_berase;

	return 0; // uboot origianl design, don't free mmc.
}

#endif

#define eMMC_NOT_READY_MARK    ~(('e'<<24)|('M'<<16)|('M'<<8)|'C')
static U32 sgu32_IfReadyGuard = eMMC_NOT_READY_MARK;

static U32 eMMC_Init_Ex(void)
{
	U32 u32_err;
	
	// ---------------------------------
	u32_err = eMMC_CheckAlignPack(eMMC_CACHE_LINE);
	if(u32_err)
        goto  LABEL_INIT_END;

	memset((void*)&g_eMMCDrv, '\0', sizeof(eMMC_DRIVER));
	
	// ---------------------------------
    // init platform & FCIE
    eMMC_PlatformInit();
    g_eMMCDrv.u8_BUS_WIDTH = BIT_SD_DATA_WIDTH_1;
	g_eMMCDrv.u16_Reg10_Mode = BIT_SD_DEFAULT_MODE_REG;
	u32_err = eMMC_FCIE_Init();
	if(u32_err)
        goto  LABEL_INIT_END;

	// ---------------------------------
	u32_err = eMMC_Init_Device();
	if(u32_err)
        goto  LABEL_INIT_END;

	sgu32_IfReadyGuard = ~eMMC_NOT_READY_MARK;

	LABEL_INIT_END:
	g_eMMCDrv.u32_LastErrCode = u32_err;

	// ---------------------------------
	// setup ID
	// use first 10 bytes of CID
	memcpy(g_eMMCDrv.au8_ID, &g_eMMCDrv.au8_CID[1], eMMC_ID_FROM_CID_BYTE_CNT);
	g_eMMCDrv.u8_IDByteCnt = eMMC_ID_DEFAULT_BYTE_CNT;
	// add a 11-th byte for number of GB
	g_eMMCDrv.au8_ID[eMMC_ID_FROM_CID_BYTE_CNT] = 
		(g_eMMCDrv.u32_SEC_COUNT >> (1+10+10)) + 1;
	eMMC_debug(eMMC_DEBUG_LEVEL, 0, "eMMC %uMB\n", g_eMMCDrv.u32_SEC_COUNT>>11);
	//eMMC_dump_mem(g_eMMCDrv.au8_ID, 0x10);
    #if 0	
    u32_err = eMMC_ReadPartitionInfo_Ex();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL, 1, "eMMC Err: eMMC Init, no pni, :%Xh\n", u32_err);
		return eMMC_ST_ERR_NO_CIS;
	}
	//eMMC_DumpDriverStatus();
    #endif  

	return u32_err;
}


U32 eMMC_Init_Device(void)
{
	U32 u32_err;
	#if IF_DETECT_eMMC_DDR_TIMING
	static U8 su8_IfNotBuildDDR=0;
	#endif
	
	#if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
	eMMC_debug(eMMC_DEBUG_LEVEL_LOW,1,"eMMC Info: Rsp from RAM\n");
	g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_RSPFROMRAM_SAVE;
    #endif
	
	g_eMMCDrv.u32_DrvFlag &= ~DRV_FLAG_INIT_DONE;
	
	// init eMMC device
	u32_err = eMMC_Identify();
	if(u32_err)
		goto LABEL_INIT_END;
	
	eMMC_clock_setting(FCIE_SLOW_CLK);
	
	// determine device parameters, from CSD
	u32_err = eMMC_CSD_Config();
	if(eMMC_ST_SUCCESS != u32_err)
		goto  LABEL_INIT_END;

	// setup eMMC device	
	// CMD7
	u32_err = eMMC_CMD3_CMD7(g_eMMCDrv.u16_RCA, 7);
	if(eMMC_ST_SUCCESS != u32_err)
		goto  LABEL_INIT_END;

	#if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
	u32_err = eMMC_CMD16(eMMC_SECTOR_512BYTE);
	if(eMMC_ST_SUCCESS != u32_err)
	 		goto  LABEL_INIT_END;
    #endif
	
	// determine device parameters, from Ext_CSD
	u32_err = eMMC_ExtCSD_Config();
	if(eMMC_ST_SUCCESS != u32_err)
		goto  LABEL_INIT_END;
	
	// ---------------------------------
	u32_err = eMMC_SetBusWidth(8, 0);
	if(eMMC_ST_SUCCESS != u32_err)
        goto  LABEL_INIT_END;

	u32_err = eMMC_SetBusSpeed(eMMC_SPEED_HIGH);
	if(eMMC_ST_SUCCESS != u32_err)
		goto  LABEL_INIT_END;
		
	eMMC_clock_setting(FCIE_DEFAULT_CLK);

	g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_INIT_DONE;

	//eMMC_DumpDriverStatus();
	#if 0
	if(eMMC_ST_SUCCESS != eMMC_FCIE_EnableDDRMode())
	{
	    #if IF_DETECT_eMMC_DDR_TIMING
		if(0==su8_IfNotBuildDDR)
		{
			su8_IfNotBuildDDR = 1;
			eMMC_FCIE_BuildDDRTimingTable();			
		}
		#endif		
	}
	#else
	    #if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
    eMMC_CMD18(0, gau8_eMMC_SectorBuf, 1); // get CMD12 Rsp
		#endif
	#endif

    #if defined(eMMC_BURST_LEN_AUTOCFG) && eMMC_BURST_LEN_AUTOCFG
	u32_err = eMMC_LoadBurstLenTable();
	if(eMMC_ST_SUCCESS != u32_err)
	{
		u32_err = eMMC_SaveBurstLenTable();
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: save Burst Len: %Xh\n", u32_err);
			goto LABEL_INIT_END;
		}
		else
			eMMC_DumpBurstLenTable();
	}
	#endif

	//eMMC_dump_mem(g_eMMCDrv.au8_AllRsp, 0x100);	
	
	LABEL_INIT_END:
	return u32_err;
}


U32 eMMC_CheckIfReady(void)
{
	if(eMMC_NOT_READY_MARK != sgu32_IfReadyGuard)
		return eMMC_ST_SUCCESS;
	else
		return eMMC_Init_Ex();
}

void eMMC_ResetReadyFlag(void)
{
	sgu32_IfReadyGuard = eMMC_NOT_READY_MARK;
}


// =======================================================
// u32_DataByteCnt: has to be 512B-boundary !
// =======================================================
U32 eMMC_EraseBlock(U32 u32_eMMCBlkAddr_start, U32 u32_eMMCBlkAddr_end)
{
	U32 u32_err, u32_SectorCnt, u32_i, u32_j;
    
	if(g_eMMCDrv.u32_eMMCFlag & eMMC_FLAG_TRIM)
	{
		u32_err =  eMMC_EraseCMDSeq(u32_eMMCBlkAddr_start, u32_eMMCBlkAddr_end);
		if(eMMC_ST_SUCCESS != u32_err)
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: EraseCMDSeq fail 0: %Xh \n", u32_err);
		    return u32_err;
	    }
	}
	else
	{
		for(u32_i=0; u32_i<eMMC_SECTOR_BUF_BYTECTN; u32_i++)
		    gau8_eMMC_SectorBuf[u32_i] = g_eMMCDrv.u8_ErasedMemContent;

		// erase blocks before EraseUnitSize
		u32_SectorCnt = u32_eMMCBlkAddr_start / g_eMMCDrv.u32_EraseUnitSize;
		u32_SectorCnt = (u32_SectorCnt+1) * g_eMMCDrv.u32_EraseUnitSize;
		u32_SectorCnt -= u32_eMMCBlkAddr_start;
		u32_SectorCnt = 
			u32_SectorCnt > (u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start) ?
			(u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start) : u32_SectorCnt;

		for(u32_i=0; u32_i<u32_SectorCnt; u32_i++)
		{
			u32_j = ((u32_SectorCnt-u32_i)<<eMMC_SECTOR_512BYTE_BITS)>eMMC_SECTOR_BUF_BYTECTN?
				eMMC_SECTOR_BUF_BYTECTN:((u32_SectorCnt-u32_i)<<eMMC_SECTOR_512BYTE_BITS);
				
			u32_err = eMMC_WriteData(gau8_eMMC_SectorBuf, u32_j, 
				u32_eMMCBlkAddr_start + u32_i);
			if(eMMC_ST_SUCCESS != u32_err){
		        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: WriteData fail 0, %Xh\n", u32_err);
		        return u32_err;
	        }

			u32_i += u32_j>>eMMC_SECTOR_512BYTE_BITS;
		}
		if((u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start) == u32_SectorCnt)
			goto LABEL_END_OF_ERASE;

		// erase blocks
        u32_i = 
            (u32_eMMCBlkAddr_end - (u32_eMMCBlkAddr_start+u32_SectorCnt))
            /g_eMMCDrv.u32_EraseUnitSize;
		if(u32_i)
		{
			u32_err = eMMC_EraseCMDSeq(
				(u32_eMMCBlkAddr_start+u32_SectorCnt), 
				(u32_eMMCBlkAddr_start+u32_SectorCnt)+u32_i*g_eMMCDrv.u32_EraseUnitSize);
	        if(eMMC_ST_SUCCESS != u32_err){
		        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: EraseCMDSeq fail 1, %Xh\n", u32_err);
		        return u32_err;
	        }
		}
        
        // erase blocks after EraseUnitSize
		u32_eMMCBlkAddr_start = 
			(u32_eMMCBlkAddr_start+u32_SectorCnt) + u32_i*g_eMMCDrv.u32_EraseUnitSize;
		
		while(u32_eMMCBlkAddr_start < u32_eMMCBlkAddr_end)
		{
			u32_j = ((u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start)<<eMMC_SECTOR_512BYTE_BITS)>eMMC_SECTOR_BUF_BYTECTN?
				eMMC_SECTOR_BUF_BYTECTN:((u32_eMMCBlkAddr_end-u32_eMMCBlkAddr_start)<<eMMC_SECTOR_512BYTE_BITS);
			
			u32_err = eMMC_WriteData(gau8_eMMC_SectorBuf, u32_j, u32_eMMCBlkAddr_start);
			if(eMMC_ST_SUCCESS != u32_err){
		        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: WriteData fail 1, %Xh\n", u32_err);
		        return u32_err;
	        }
			u32_eMMCBlkAddr_start += u32_j>>eMMC_SECTOR_512BYTE_BITS;
		}			
	}

	LABEL_END_OF_ERASE:
	return u32_err;
}

U32 eMMC_WriteData(U8* pu8_DataBuf, U32 u32_DataByteCnt, U32 u32_BlkAddr)
{
	U32 u32_err=eMMC_ST_SUCCESS;
	volatile U16 u16_BlkCnt;

	// check if eMMC Init 
	if(eMMC_NOT_READY_MARK == sgu32_IfReadyGuard)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC not ready (init) \n");
		return eMMC_ST_ERR_NOT_INIT;
	}

	if(NULL == pu8_DataBuf)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: w data buf is NULL: %Xh \n", (U32)pu8_DataBuf);
		return eMMC_ST_ERR_INVALID_PARAM;
	}
	// check if u32_DataByteCnt is 512B boundary
	if(u32_DataByteCnt & (eMMC_SECTOR_512BYTE-1))
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: data not 512B boundary \n");
		return eMMC_ST_ERR_INVALID_PARAM;
	}
	if(u32_BlkAddr + (u32_DataByteCnt>>9) > g_eMMCDrv.u32_SEC_COUNT) 
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: invalid data range, %Xh > %Xh \n",
			u32_BlkAddr + (u32_DataByteCnt>>9), g_eMMCDrv.u32_SEC_COUNT);
		return eMMC_ST_ERR_INVALID_PARAM;
	}	

	// write data
	while(u32_DataByteCnt)
	{
		if(u32_DataByteCnt > eMMC_SECTOR_512BYTE)
		{
		    if((u32_DataByteCnt>>eMMC_SECTOR_512BYTE_BITS) < BIT_SD_JOB_BLK_CNT_MASK)
		    	u16_BlkCnt = (u32_DataByteCnt>>eMMC_SECTOR_512BYTE_BITS);
    		else
	    		u16_BlkCnt = BIT_SD_JOB_BLK_CNT_MASK;
        
		    u32_err = eMMC_CMD25(u32_BlkAddr, pu8_DataBuf, u16_BlkCnt);			
    	}
	    else
	    {
			u16_BlkCnt = 1;
		    u32_err = eMMC_CMD24(u32_BlkAddr, pu8_DataBuf);
	    }

		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: W fail: %Xh\n", u32_err);
			g_eMMCDrv.u32_LastErrCode = u32_err;
			break;
		}

		u32_BlkAddr += u16_BlkCnt;
		pu8_DataBuf += u16_BlkCnt << eMMC_SECTOR_512BYTE_BITS;
		u32_DataByteCnt -= u16_BlkCnt << eMMC_SECTOR_512BYTE_BITS;
	}

	return u32_err;
}


// =======================================================
// u32_DataByteCnt: has to be 512B-boundary !
// =======================================================
U32 eMMC_ReadData(U8* pu8_DataBuf, U32 u32_DataByteCnt, U32 u32_BlkAddr)
{
	U32 u32_err;
	volatile U16 u16_BlkCnt;
	U8  u8_IfNotCacheLineAligned=0;

	//eMMC_debug(eMMC_DEBUG_LEVEL,1,"\n");

	// check if eMMC Init 
	if(eMMC_NOT_READY_MARK == sgu32_IfReadyGuard)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: not ready (init) \n");
		return eMMC_ST_ERR_NOT_INIT;
	}
	// check if u32_DataByteCnt is 512B boundary
	if(u32_DataByteCnt & (eMMC_SECTOR_512BYTE-1))
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: data not 512B boundary \n");
		return eMMC_ST_ERR_INVALID_PARAM;
	}
	if(u32_BlkAddr + (u32_DataByteCnt>>9) > g_eMMCDrv.u32_SEC_COUNT) 
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: invalid data range, %Xh > %Xh \n",
			u32_BlkAddr + (u32_DataByteCnt>>9), g_eMMCDrv.u32_SEC_COUNT);
		return eMMC_ST_ERR_INVALID_PARAM;
	}

	// read data
	// first 512 bytes, special handle if not cache line aligned
	if((U32)pu8_DataBuf & (eMMC_CACHE_LINE-1))
	{
		#if 0
		eMMC_debug(eMMC_DEBUG_LEVEL_WARNING,1,
			"eMMC Warn: R, buffer not Cache Line aligned: %Xh \n", 
			(U32)pu8_DataBuf);
		#endif
	
		u32_err = eMMC_CMD17(u32_BlkAddr, gau8_eMMC_SectorBuf);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: R fail.0: %Xh\n", u32_err);
			g_eMMCDrv.u32_LastErrCode = u32_err;
			return u32_err;
		}
		memcpy(pu8_DataBuf, gau8_eMMC_SectorBuf, eMMC_SECTOR_512BYTE);
		u32_BlkAddr += 1;
		pu8_DataBuf += eMMC_SECTOR_512BYTE;
		u32_DataByteCnt -= eMMC_SECTOR_512BYTE;

		// last 512B must be not cache line aligned, 
		// reserved for last eMMC_CMD17_CIFD
		if(u32_DataByteCnt){
			u8_IfNotCacheLineAligned = 1;
			u32_DataByteCnt -= eMMC_SECTOR_512BYTE;
		}
	}
		
	while(u32_DataByteCnt)
	{
		if(u32_DataByteCnt > eMMC_SECTOR_512BYTE)
		{
		    if((u32_DataByteCnt>>eMMC_SECTOR_512BYTE_BITS) < BIT_SD_JOB_BLK_CNT_MASK)
		    	u16_BlkCnt = (u32_DataByteCnt>>eMMC_SECTOR_512BYTE_BITS);
    		else
	    		u16_BlkCnt = BIT_SD_JOB_BLK_CNT_MASK;
        
		    u32_err = eMMC_CMD18(u32_BlkAddr, pu8_DataBuf, u16_BlkCnt);
    	}
	    else
	    {
			u16_BlkCnt = 1;
		    u32_err = eMMC_CMD17(u32_BlkAddr, pu8_DataBuf);
	    }

		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: R fail.1: %Xh\n", u32_err);
			g_eMMCDrv.u32_LastErrCode = u32_err;
			break;
		}

		u32_BlkAddr += u16_BlkCnt;
		pu8_DataBuf += u16_BlkCnt << eMMC_SECTOR_512BYTE_BITS;
		u32_DataByteCnt -= u16_BlkCnt << eMMC_SECTOR_512BYTE_BITS;
	}

	// last 512 bytes, special handle if not cache line aligned
	if(u8_IfNotCacheLineAligned)
	{
		u32_err = eMMC_CMD17(u32_BlkAddr, gau8_eMMC_SectorBuf);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: R fail.2: %Xh\n", u32_err);
			g_eMMCDrv.u32_LastErrCode = u32_err;
		}
		memcpy(pu8_DataBuf, gau8_eMMC_SectorBuf, eMMC_SECTOR_512BYTE);
	}

	return u32_err;
}



  #if defined(eMMC_FCIE_LINUX_DRIVER) && eMMC_FCIE_LINUX_DRIVER
U32 eMMC_SearchDevNodeStartSector(void)
{
	U32 u32_err;
	volatile U16 u16_PartIdx;
	eMMC_PNI_t *pPartInfo = (eMMC_PNI_t*)gau8_eMMC_PartInfoBuf;

	u32_err = eMMC_CheckIfReady();
	if(eMMC_ST_SUCCESS != u32_err){
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: init fail: %Xh\n", u32_err);
		return u32_err;
	}

	u32_err = eMMC_GetPartitionIndex(eMMC_PART_DEV_NODE, 0, &u16_PartIdx);
	if(eMMC_ST_SUCCESS != u32_err){
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: GetPartitionIndex fail: %Xh\n", u32_err);
		return u32_err;
	}

	g_eMMCDrv.u32_PartDevNodeStartSector = pPartInfo->records[u16_PartIdx].u16_StartBlk;
	g_eMMCDrv.u32_PartDevNodeStartSector *= pPartInfo->u16_BlkPageCnt;
	g_eMMCDrv.u16_PartDevNodeSectorCnt = pPartInfo->records[u16_PartIdx].u16_BlkCnt;
	g_eMMCDrv.u16_PartDevNodeSectorCnt *= pPartInfo->u16_BlkPageCnt;

	return eMMC_ST_SUCCESS;
	
}

  #endif
#endif


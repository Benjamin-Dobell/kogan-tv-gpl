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
#include <common.h>


#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>

#include "inc/common/drvNAND.h"

#define DMA_MODE					1

U32 u32CurRow = 0;
U32 u32CurCol = 0;
U32 u32CIFDIdx = 0;
U16 u16ByteIdxofPage = 0;
U32 u32WriteLen = 0;
S32 s32_ECCStatus = 0;

int sectoridx=0;

U8 u8MainBuf[8192*4] UNFD_ALIGN1;
U8 u8SpareBuf[640*4] UNFD_ALIGN1;

#define CONFIG_NANDDRV_DEBUG 1 

#if (CONFIG_NANDDRV_DEBUG)
#define NAND_DEBUG(fmt, args...) nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, fmt, ##args)
#else
#define NAND_DEBUG(fmt, args...)
#endif


/* These really don't belong here, as they are specific to the NAND Model */
static uint8_t scan_ff_pattern[] = {0xff};

/* struct nand_bbt_descr - bad block table descriptor */
static struct nand_bbt_descr _titania_nand_bbt_descr = {
	.options = NAND_BBT_2BIT | NAND_BBT_LASTBLOCK | NAND_BBT_VERSION | NAND_BBT_CREATE | NAND_BBT_WRITE,
	.offs = 5,
	.len = 1,
	.pattern = scan_ff_pattern
};

static struct nand_ecclayout unfd_nand_oob_custom;

static uint8_t bbt_pattern[] = {'B', 'b', 't', '0' };
static uint8_t mirror_pattern[] = {'1', 't', 'b', 'B' };

static struct nand_bbt_descr _titania_bbt_main_descr = {
	.options		= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE |
					  NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs			= 1,
	.len			= 3,
	.veroffs		= 4,
	.maxblocks		= NAND_BBT_BLOCK_NUM,
	.pattern		= bbt_pattern
};

static struct nand_bbt_descr _titania_bbt_mirror_descr = {
	.options		= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE |
					  NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs			= 1,
	.len			= 3,
	.veroffs		= 4,
	.maxblocks		= NAND_BBT_BLOCK_NUM,
	.pattern		= mirror_pattern
};

static void _titania_nand_hwcontrol(struct mtd_info *mtdinfo, int cmd, unsigned int ctrl)
{
	NAND_DEBUG("NANDDrv_HWCONTROL()\n");

	switch(cmd)
	{
		case NAND_CTL_SETNCE:
			NAND_DEBUG("NAND_CTL_SETNCE\n");
			// Let FCIE3 control the CE
			break;
		case NAND_CTL_CLRNCE:
			NAND_DEBUG("NAND_CTL_CLRNCE\n");
			// Let FCIE3 control the CE

			break;
		case NAND_CTL_SETCLE:
			NAND_DEBUG("NAND_CTL_SETCLE\n");
			// We have no way to control CLE in NFIE/FCIE2/FCIE3
			break;
		case NAND_CTL_CLRCLE:
			NAND_DEBUG("NAND_CTL_CLRCLE\n");
			// We have no way to control CLE in NFIE/FCIE2/FCIE3
			break;

		case NAND_CTL_SETALE:
			NAND_DEBUG("NAND_CTL_SETALE\n");
			// We have no way to control CLE in NFIE/FCIE2/FCIE3
			break;
		case NAND_CTL_CLRALE:
			NAND_DEBUG("NAND_CTL_CLRALE\n");
			// We have no way to control CLE in NFIE/FCIE2/FCIE3
			break;

		default:
			NAND_DEBUG("UNKNOWN CMD\n");
			break;
  }

  return;
}


static int _titania_nand_device_ready(struct mtd_info *mtdinfo)
{
	NAND_DEBUG("NANDDrv_DEVICE_READY()\n");

	#if 0 // We have wait ready in each HAL functions
	drvNAND_WaitReady(1000);
	#endif

	return 1;
}

static void _titania_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{

	NAND_DEBUG("NANDDrv_WRITE_BUF() 0x%X\r\n",len);

	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U8 *pu8MainBuf;
	U8 *pu8SpareBuf;

	if( len >= pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt ) // whole page or Main area only
	{
		if( len > pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt ) // whole page
		{
			#if !defined(CONFIG_ARM)      	
			if( ((U32)buf) >= 0xC0000000 ) // For MIU1 (MIPS)
			{
				memcpy(u8MainBuf, buf, pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
				memcpy(u8SpareBuf, buf + (pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt), pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
				pu8MainBuf = u8MainBuf;
				pu8SpareBuf = u8SpareBuf;
			}        
			else
			#endif
			{
				pu8MainBuf = (U8*)buf;
				pu8SpareBuf = (U8*)buf + (pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
			}

			#if defined(FCIE_LFSR) && FCIE_LFSR
			if(pNandDrv->u8_RequireRandomizer == 1)
				NC_EnableLFSR();
			#endif

			#if DMA_MODE
			if(pNandDrv->u8_PlaneCnt > 1)
				NC_WritePages2P(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
			else
				NC_WritePages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
			#else
			NC_WritePage_RIUMode(u32CurRow, pu8MainBuf, pu8SpareBuf);
			#endif		
		}
		else // main area only
		{
			memcpy(u8MainBuf, buf, len);
			u32WriteLen += len;
			u16ByteIdxofPage += len;
		}
	}
	else
	{
		if((u32WriteLen==0) && (u16ByteIdxofPage>=pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt)) // mtd skip prepare main area, default all oxff
		{
			memset(u8MainBuf, 0xFF, pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
			memset(u8SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
		}

		memcpy(u8SpareBuf, buf, len);
		u32WriteLen += len;
		u16ByteIdxofPage += len;

		if( (u32WriteLen == (pNandDrv->u16_PageByteCnt + pNandDrv->u16_SpareByteCnt) * pNandDrv->u8_PlaneCnt) ||
		(u32WriteLen ==  pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt))
		{

			#if defined(FCIE_LFSR) && FCIE_LFSR
			if(pNandDrv->u8_RequireRandomizer == 1)
				NC_EnableLFSR();
			#endif

			#if DMA_MODE      	
			if(pNandDrv->u8_PlaneCnt > 1)
				NC_WritePages2P(u32CurRow, u8MainBuf, u8SpareBuf, 1);
			else
				NC_WritePages(u32CurRow, u8MainBuf, u8SpareBuf, 1);
			#else      	
			NC_WritePage_RIUMode(u32CurRow, pu8MainBuf, pu8SpareBuf);
			#endif      	
		}
	}

  return;
}

static void _titania_nand_read_buf(struct mtd_info *mtd, u_char* const buf, int len)
{
//	printf("_titania_nand_read_buf(): %08X, %d\n",buf, len );
	
	NAND_DEBUG("NANDDrv_READ_BUF()0x%X\n",len);

	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U8 *pu8MainBuf;
	U8 *pu8SpareBuf;
	U32 u32_Err = 0;

	u16ByteIdxofPage = len;
	
	if( len >= pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt)
	{
		if( len > pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt)
		{
			#if !defined(CONFIG_ARM)
			if( ((U32)buf) >= 0xC0000000 ) // For MIU1 (MIPS)
			{
				pu8MainBuf = u8MainBuf;
				pu8SpareBuf = u8SpareBuf;
			}						
			else
			#endif	
			{
				pu8MainBuf = buf;
				pu8SpareBuf = (U8*)buf + (pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
			}

			#if DMA_MODE

			#if defined(FCIE_LFSR) && FCIE_LFSR
			if(pNandDrv->u8_RequireRandomizer == 1)
				NC_EnableLFSR();
			#endif
			memset(pu8SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
			if(pNandDrv->u8_PlaneCnt > 1)
			{
				u32_Err = NC_ReadPages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
				if(u32_Err == 0)
					u32_Err = NC_ReadPages(u32CurRow + pNandDrv->u16_BlkPageCnt, 
								pu8MainBuf + pNandDrv->u16_PageByteCnt, pu8SpareBuf + pNandDrv->u16_SpareByteCnt, 1);
			}
			else
			u32_Err = NC_ReadPages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
			#else        
			for(sectoridx = 0; sectoridx < pNandDrv->u16_PageSectorCnt; sectoridx ++)
				u32_Err = NC_ReadSector_RIUMode(u32CurRow, sectoridx, pu8MainBuf + sectoridx*pNandDrv->u16_SectorByteCnt, 
					pu8SpareBuf+ sectoridx*pNandDrv->u16_SectorSpareByteCnt);
			#endif					
			NC_CheckECC((int*)(&s32_ECCStatus));
			if(u32_Err != UNFD_ST_SUCCESS)
				s32_ECCStatus = -1;

			if( buf != pu8MainBuf ) // If MIU1, copy data from temp buffer
			{
				memcpy(buf, u8MainBuf, pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
				memcpy((U8*)buf + (pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt), u8SpareBuf, pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
			}
		}
		else
		{
			#if !defined(CONFIG_ARM)
			if( ((U32)buf) >= 0xC0000000 ) // For MIU1 (MIPS)
			{
				pu8MainBuf = u8MainBuf;
			}
			else
			#endif
			{
				pu8MainBuf = buf;
			}
			pu8SpareBuf = u8SpareBuf; // Preserve spare data in temp buffer for next read_buf()
			
			#if DMA_MODE

			#if defined(FCIE_LFSR) && FCIE_LFSR
			if(pNandDrv->u8_RequireRandomizer == 1)
				NC_EnableLFSR();
			#endif

			memset(pu8SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
			if(pNandDrv->u8_PlaneCnt > 1)
			{
				u32_Err = NC_ReadPages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
				if(u32_Err == 0)
					u32_Err = NC_ReadPages(u32CurRow + pNandDrv->u16_BlkPageCnt, 
								pu8MainBuf + pNandDrv->u16_PageByteCnt, pu8SpareBuf + pNandDrv->u16_SpareByteCnt, 1);

			}
			else
			u32_Err = NC_ReadPages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
			#else
			for(sectoridx = 0; sectoridx < pNandDrv->u16_PageSectorCnt; sectoridx ++)
				u32_Err = NC_ReadSector_RIUMode(u32CurRow, sectoridx, pu8MainBuf + sectoridx*pNandDrv->u16_SectorByteCnt, 
					pu8SpareBuf+ sectoridx*pNandDrv->u16_SectorSpareByteCnt );
			#endif
			NC_CheckECC((int *)(&s32_ECCStatus));
			if(u32_Err != UNFD_ST_SUCCESS)
				s32_ECCStatus = -1;
				
			if( buf != pu8MainBuf ) // If MIU1, copy data from temp buffer
				 memcpy(buf, u8MainBuf, pNandDrv->u16_PageByteCnt);
		}
	}
	else
	{
		memcpy(buf, u8SpareBuf, len);
	}

	return;
}

static u16 _titania_nand_read_word(struct mtd_info *mtd)
{
	NAND_DEBUG("NANDDrv_READ_WORD()\n");
	return 0;
}

static u_char _titania_nand_read_byte(struct mtd_info *mtd)
{
	U8 u8Ret = 0;

	NC_GetCIFD(&u8Ret, u32CIFDIdx, 1);
	u32CIFDIdx++;
	NAND_DEBUG("NANDDrv_READ_BYTE()=0x%X\n",u8Ret);

	return (u8Ret);
}

/**
 * nand_wait - [DEFAULT]  wait until the command is done
 * @mtd:  MTD device structure
 * @this: NAND chip structure
 * @state:  state to select the max. timeout value
 *
 * Wait for command done. This applies to erase and program only
 * Erase can take up to 400ms and program up to 20ms according to
 * general NAND and SmartMedia specs
 *
 */
static int _titania_nand_wait(struct mtd_info *mtd, struct nand_chip *this)
{
	NAND_DEBUG("NANDDrv_WAIT()\n");

	return REG(NC_ST_READ);
}

static void _titania_nand_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	struct nand_chip *chip =(struct nand_chip*)(mtd->priv);
    int LBA, LPA;

	if(pNandDrv->u8_PlaneCnt > 1)
    {
        LBA = page_addr >> (chip->phys_erase_shift - chip->page_shift);
        LPA = page_addr & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
        page_addr = ((LBA<<1)<<pNandDrv->u8_BlkPageCntBits)+LPA;
    }


	switch (command) {
		case NAND_CMD_READ0:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READ0, page_addr: 0x%x, column: 0x%x.\n", page_addr, (column>>1));
			u32CurRow = page_addr;
			u32CurCol = column;
			break;

		case NAND_CMD_READ1:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READ1.\n");

			NAND_DEBUG("\033[31mUNFD not support READ1(CMD 0x01) now!!!\033[m\n");
			break;

		case NAND_CMD_READOOB:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READOOB.\n");

			NAND_DEBUG("\033[34mReadOOB (CMD 0x50)\033[m\n");
			u32CIFDIdx = 0;
			#if defined(FCIE_LFSR) && FCIE_LFSR
			if(pNandDrv->u8_RequireRandomizer)
			{
				NC_EnableLFSR();
				NC_ReadPages(page_addr, u8MainBuf, u8SpareBuf, 1);
			}
			else
			#endif
			{
				u16ByteIdxofPage = 0;
				NC_Read_RandomIn(page_addr, column + pNandDrv->u16_PageByteCnt, u8SpareBuf, 
					(pNandDrv->u16_SpareByteCnt >= 512)? 510 : pNandDrv->u16_SpareByteCnt);
				if(pNandDrv->u8_PlaneCnt > 1)
				{
					NC_Read_RandomIn(page_addr + pNandDrv->u16_BlkPageCnt, column + pNandDrv->u16_PageByteCnt, u8SpareBuf + pNandDrv->u16_SpareByteCnt, 
						(pNandDrv->u16_SpareByteCnt >= 512)? 510 : pNandDrv->u16_SpareByteCnt);
					if(u8SpareBuf[0] != 0xFF || u8SpareBuf[pNandDrv->u16_SpareByteCnt] != 0xFF)
						u8SpareBuf[0] = u8SpareBuf[pNandDrv->u16_SpareByteCnt] = 0;
				}
			}
		break;

		case NAND_CMD_READID:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READID.\n");

			u32CIFDIdx = 0;
			NC_ReadID();
			break;

		case NAND_CMD_PAGEPROG:
			/* sent as a multicommand in NAND_CMD_SEQIN */
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_PAGEPROG.\n");

			// We have done page program in write_buf()
			break;

		case NAND_CMD_ERASE1:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_ERASE1,  page_addr: 0x%x, column: 0x%x.\n", page_addr, column);
			if(pNandDrv->u8_PlaneCnt > 1)
				NC_EraseBlk2P(page_addr);
			else
				NC_EraseBlk(page_addr);

			break;

		case NAND_CMD_ERASE2:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_ERASE2.\n");
			// We do all erase function in Erase1 command.

			break;

		case NAND_CMD_SEQIN:
			/* send PAGE_PROG command(0x1080) */
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_SEQIN/PAGE_PROG,  page_addr: 0x%x, column: 0x%x.\n", page_addr, column);

			u32CurRow = page_addr;
			u32CurCol = column;
			u16ByteIdxofPage = column;
			u32WriteLen = 0;
			break;

		case NAND_CMD_STATUS:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_STATUS.\n");

			u32CIFDIdx = 0;
			NC_ReadStatus();

			break;

		case NAND_CMD_RESET:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_RESET.\n");

			NC_ResetNandFlash();

			break;

		case NAND_CMD_STATUS_MULTI:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_STATUS_MULTI.\n");

			u32CIFDIdx = 0;
			NC_ReadStatus();
			break;

		case NAND_CMD_READSTART:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READSTART.\n");

			// We have done this command in NC_ReadPages()
			break;

		case NAND_CMD_CACHEDPROG:
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_CACHEDPROG.\n");

			NAND_DEBUG("\033[31mUNFD not support CACHEPROG (CMD 0x15) now!!!\033[m\n");
			break;

		default:
			NAND_DEBUG("_titania_nand_cmdfunc: error, unsupported command.\n");
			break;
	}

	return;
}

static void _titania_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	NAND_DEBUG("enable_hwecc\r\n");
	// default enable
}

static int _titania_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	NAND_DEBUG("calculate_ecc\r\n");

	// NFIE/FCIE2/FCIE3 will calculate it.

	return 0;
}

static int _titania_nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
	int s32ECCStatus = 0;
	//NC_CheckECC(&s32ECCStatus);
	s32ECCStatus = s32_ECCStatus;
	return s32ECCStatus;
}
U32 drvNAND_Init(void)
{
	U32 u32_Err = 0;
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();

	memset(pNandDrv, 0, sizeof(NAND_DRIVER));

	NC_PlatformInit();

	pNandDrv->pPartInfo = drvNAND_get_DrvContext_PartInfo();
	memset(pNandDrv->pPartInfo, 0, NAND_PARTITAION_BYTE_CNT);

	u32_Err = drvNAND_ProbeReadSeq();
	if(u32_Err != UNFD_ST_SUCCESS)
	{
		return u32_Err;
	}
	u32_Err = drvNAND_SearchCIS();

	#if defined(ENABLE_READ_DISTURBANCE_SUPPORT) && ENABLE_READ_DISTURBANCE_SUPPORT
	if(u32_Err == UNFD_ST_SUCCESS)
	{
		nand_ReadDisturbance();
	}
	#endif

	return u32_Err; 
		
}

int nand_unfd_init(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U16 u16_i;
	U32 u32_Err;
	u32_Err = drvNAND_Init();

	if(UNFD_ST_ERR_NO_CIS == u32_Err)
	{	
		printf("NAND No CIS found\n");

		printf("NAND ID:");
		for(u16_i = 0; u16_i < pNandDrv->u8_IDByteCnt; u16_i++)
		printf("0x%X ", pNandDrv->au8_ID[u16_i]);
		printf("\n");
		
		#if defined(FCIE4_DDR) && FCIE4_DDR
		if(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_MASM)
		{
			printf("NAND Error: Detect DDR NAND but have no CIS in NAND\n");
			printf("Please use command, \"ncisbl\", to write cis\n");
			return -1;
		}
		#endif
		{
			REG_WRITE_UINT16(NC_PATH_CTL, 0x20);

			NC_PlatformInit();

			NC_ResetFCIE(); // Reset FCIE3

			nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "NC_ResetFCIE done\n");


			// We need to config Reg 0x40 first because we we need to get nand ID first
		//	pNandDrv->u16_Reg40_Signal =
		//	(BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
		//	~(BIT_NC_CHK_RB_EDGEn | BIT_NC_CE_SEL_MASK);
			REG_WRITE_UINT16(NC_SIGNAL, pNandDrv->u16_Reg40_Signal);


#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
			nand_enable_intr_mode();
#endif
		
			NC_ResetNandFlash(); // Reset NAND flash

			nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "NC_ResetNandFlash done\n");

			NC_ReadID();

			nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "NC_ReadID done\n");

			if( drvNAND_CHECK_FLASH_TYPE() != 0)
			{
				printf("[%s]\tUnsupported NAND Type, Need to update CIS or modify code\n",__func__);
				return -1;
			}

			nand_debug(UNFD_DEBUG_LEVEL_HIGH,1,"drvNAND_CHECK_FLASH_TYPE done\n");

			NC_ConfigContext();
			NC_Init();
			
		}
	}
	else if(UNFD_ST_SUCCESS != u32_Err)
	{
		printf("[%s]\tdrvNAND_Init Error : %lX", __func__, u32_Err);
		return -1;
	}
	/*Config clock timing either for DDR NAND or SDR NAND */
	nand_config_clock(pNandDrv->u16_tRC);

	return 0;


}

U32 RestoreRawDatafromCISblk(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 u32_GoodBlkIdx, u32_Err;
	U16 u16_SrcPBA, u16_i;
	U8 *pu8_PageDataBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
    U8 *pu8_PageSpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	MBBS_INFO_t * MBBS_INFO =(MBBS_INFO_t *)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;

	if(pNandDrv->u8_CellType)
	{
		//search last good block of cis
		for(u32_GoodBlkIdx = 9; u32_GoodBlkIdx >= 5; u32_GoodBlkIdx ++)
		{
			u32_Err = NC_ReadPages(u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits, pu8_PageDataBuf, pu8_PageSpareBuf, 1);
			if(pu8_PageSpareBuf[0] == 0xFF)
				break;
		}
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 1,"Last good block is find at %lX\n", u32_GoodBlkIdx);

		//check block content
		// read first and last lsb page for checking ecc and empty
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_LOW, 1,"Read fail, erase cis block %lX\n", u32_GoodBlkIdx);
			//erase block and skip process
			u32_Err = NC_EraseBlk(u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits);
			if(u32_Err != UNFD_ST_SUCCESS)
			{
				//mark bad
				drvNAND_MarkBadBlk(u32_GoodBlkIdx);
			}
			return 0;
		}
		
		if(pu8_PageSpareBuf[3] == 0xFF && pu8_PageSpareBuf[4] == 0xFF)
		{
			nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "First page is empty\n");
			//done for the resetore
			return 0;
		}
		if(!(pu8_PageSpareBuf[3] == 0x36 && pu8_PageSpareBuf[4] ==0x97))
		{
			nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "Dummy data find at backup block @ %lX, spare[3]=%X, spare[4]=%X\n", 
				u32_GoodBlkIdx, pu8_PageSpareBuf[3], pu8_PageSpareBuf[4]);
			return 0;
		}

		nand_debug(UNFD_DEBUG_LEVEL, 1, "Read last page of last good cis block\n");
		u32_Err = NC_ReadPages((u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits) 
			+ ga_tPairedPageMap[pNandDrv->u16_BlkPageCnt/2 - 1].u16_LSB, pu8_PageDataBuf, pu8_PageSpareBuf, 1);
		if(u32_Err != UNFD_ST_SUCCESS ||
				 (pu8_PageSpareBuf[3] == 0xFF && pu8_PageSpareBuf[4] == 0xFF) || (pu8_PageSpareBuf[3] != 0x36 || pu8_PageSpareBuf[4] != 0x97))
		{
			nand_debug(UNFD_DEBUG_LEVEL_LOW, 1, "Read fail or last page is empty, Erase last good cis block\n");
			u32_Err = NC_EraseBlk(u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits);
			if(u32_Err != UNFD_ST_SUCCESS)
			{
				//mark bad
				drvNAND_MarkBadBlk(u32_GoodBlkIdx);
			}
			return 0;
		}

		u16_SrcPBA = MBBS_INFO->u16_BackupPBA;
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 1,"Erase Source block %X\n", u16_SrcPBA);
		//erase source block
		u32_Err = NC_EraseBlk(u16_SrcPBA << pNandDrv->u8_BlkPageCntBits);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			//mark bad and do shifting...
		}
		nand_debug(UNFD_DEBUG_LEVEL_LOW, 1,"Write Back data to Source block %X\n", u16_SrcPBA);
		//restore data to source block
		for(u16_i = 0; u16_i < pNandDrv->u16_BlkPageCnt / 2; u16_i ++)
		{
			u32_Err = NC_ReadPages((u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits) + 
				ga_tPairedPageMap[u16_i].u16_LSB, pu8_PageDataBuf, pu8_PageSpareBuf, 1);
			if(u32_Err != UNFD_ST_SUCCESS)
			{
				//fatal error
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1,"Read backup block ecc fail, when restoring data. Please re-upgrade data to nand offset 0x%X\n", 
												u16_SrcPBA << (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_BlkPageCntBits));
				return UNFD_ST_ERR_DATA_CMP_FAIL;
			}
			else
			{
				if(pu8_PageSpareBuf[3] == 0x36 && pu8_PageSpareBuf[4] == 0x97)
				{
					u32_Err = NC_WritePages((u16_SrcPBA << pNandDrv->u8_BlkPageCntBits) + ga_tPairedPageMap[u16_i].u16_LSB,
								pu8_PageDataBuf, NULL, 1);
					if(u32_Err != UNFD_ST_SUCCESS)
					{
						//mark bad and do shifting...
					}
				}
				else
				{
					nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1,"Read backup block ecc fail, when restoring data. Please re-upgrade data to nand offset 0x%X\n", 
												u16_SrcPBA << (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_BlkPageCntBits));
					return UNFD_ST_ERR_DATA_CMP_FAIL;
				}
			}
		}
		
		//Erase Backup block
		u32_Err = NC_EraseBlk(u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			drvNAND_MarkBadBlk(u32_GoodBlkIdx);
		}

	}
	return 0;
}

/*
 * Board-specific NAND initialization.
 * - hwcontrol: hardwarespecific function for accesing control-lines
 * - dev_ready: hardwarespecific function for  accesing device ready/busy line
 * - eccmode: mode of ecc, see defines
 */

int board_nand_init(struct nand_chip *nand)
{
	U16 u16_tmp, u16_oob_poi, u16_i, u16_SectorSpareByteCnt;
	NAND_DEBUG("NANDDrv_BOARD_NAND_INIT().\n");



#if DMA_MODE
	printf("DMA MODE\n");
#else
	printf("RIU MODE\n");
#endif

	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	
	
	if( nand_unfd_init() != 0 ) return -1;
	
	/* please refer to include/linux/nand.h for more info. */
	nand->dev_ready = _titania_nand_device_ready;
	nand->cmd_ctrl = _titania_nand_hwcontrol;
	nand->ecc.mode  = NAND_ECC_HW;

	nand->ecc.size                      = pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt; 
	nand->ecc.bytes                     = pNandDrv->u16_ECCCodeByteCnt * pNandDrv->u16_PageSectorCnt * pNandDrv->u8_PlaneCnt;

	u16_SectorSpareByteCnt = (pNandDrv->u16_SpareByteCnt/pNandDrv->u16_PageSectorCnt);
	unfd_nand_oob_custom.eccbytes = pNandDrv->u8_PlaneCnt*(pNandDrv->u16_ECCCodeByteCnt * pNandDrv->u16_PageSectorCnt);
	for(u16_tmp=0 ; u16_tmp<pNandDrv->u8_PlaneCnt*pNandDrv->u16_PageSectorCnt ; u16_tmp++)
	{
		u16_oob_poi = ((u16_tmp+1)*u16_SectorSpareByteCnt)-pNandDrv->u16_ECCCodeByteCnt;
		for(u16_i=0 ; u16_i<pNandDrv->u16_ECCCodeByteCnt ; u16_i++)
		{
			unfd_nand_oob_custom.eccpos[(u16_tmp*pNandDrv->u16_ECCCodeByteCnt)+u16_i] = u16_oob_poi++;
		}

		if( u16_tmp == 0 || u16_tmp == pNandDrv->u16_PageSectorCnt)
		{
			unfd_nand_oob_custom.oobfree[u16_tmp].offset = u16_tmp*u16_SectorSpareByteCnt + 2;
			unfd_nand_oob_custom.oobfree[u16_tmp].length= u16_SectorSpareByteCnt - pNandDrv->u16_ECCCodeByteCnt - 2;
		}
		else
		{
			unfd_nand_oob_custom.oobfree[u16_tmp].offset = u16_tmp*u16_SectorSpareByteCnt;
			unfd_nand_oob_custom.oobfree[u16_tmp].length= u16_SectorSpareByteCnt - pNandDrv->u16_ECCCodeByteCnt;
		}
	}
	//dump_mem((unsigned char *) &unfd_nand_oob_custom, sizeof(unfd_nand_oob_custom));
	unfd_nand_oob_custom.oobavail = 0;
    for (u16_i = 0; u16_i < ARRAY_SIZE(unfd_nand_oob_custom.oobfree); u16_i++)
    {
		if(unfd_nand_oob_custom.oobfree[u16_i].length)
    		unfd_nand_oob_custom.oobavail += unfd_nand_oob_custom.oobfree[u16_i].length;
    }

 
	nand->ecc.layout =  &unfd_nand_oob_custom;
	_titania_nand_bbt_descr.offs    = 0;

	nand->ecc.hwctl = _titania_nand_enable_hwecc;
	nand->ecc.correct = _titania_nand_correct_data;
	nand->ecc.calculate = _titania_nand_calculate_ecc;

	if( pNandDrv->u16_PageByteCnt > 512 )
		nand->options = NAND_USE_FLASH_BBT | NAND_NO_AUTOINCR;
	else
		nand->options = NAND_USE_FLASH_BBT;

	nand->options = NAND_USE_FLASH_BBT;
	nand->waitfunc = _titania_nand_wait;
	nand->read_byte = _titania_nand_read_byte;
	nand->read_word = _titania_nand_read_word;
	nand->read_buf = _titania_nand_read_buf;
	nand->write_buf = _titania_nand_write_buf;
	nand->chip_delay = 20; //@FIXME: unite: us, please refer to nand_base.c 20us is default.
	nand->cmdfunc = _titania_nand_cmdfunc;
	nand->badblock_pattern = &_titania_nand_bbt_descr; //using default badblock pattern.
    nand->bbt_td						= &_titania_bbt_main_descr;
    nand->bbt_md						= &_titania_bbt_mirror_descr;

    if(unfd_nand_oob_custom.oobfree[0].length < (nand->bbt_td->len +1))
    {
        nand->bbt_td->offs = 0;
        nand->bbt_td->veroffs = 3;
		nand->bbt_md->offs = 0;
		nand->bbt_md->veroffs = 3;
    }

	if(RestoreRawDatafromCISblk() != UNFD_ST_SUCCESS)
	{
		printf("[%s] NAND fatal Error, restore data fail\n", __func__);
	}

	return 0;

}

U32 drvNAND_WriteCIS_for_ROM(NAND_FLASH_INFO_t * pNandInfo)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	PARTITION_INFO_t *pPartInfo = drvNAND_get_DrvContext_PartInfo();
	U8 *au8_PageBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *au8_SpareBuf = (U8*)pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	BLK_INFO_t *pBlkInfo = (BLK_INFO_t*)au8_SpareBuf;
	U32 u32_Err = UNFD_ST_SUCCESS;
	U16 u16_PBA;
	U32 u32_PageIdx;
	U8 u8_CISIdx;

	NC_ConfigContext();
	NC_ReInit();
	NC_Config();

	nand_config_clock(pNandInfo->u16_tRC);
	
	#if defined(FCIE4_DDR) && FCIE4_DDR
	memcpy((void *) &pNandInfo->tDefaultDDR, (const void *) &pNandDrv->tDefaultDDR, sizeof(DDR_TIMING_GROUP_t));
	memcpy((void *) &pNandInfo->tMaxDDR, (const void *) &pNandDrv->tMaxDDR, sizeof(DDR_TIMING_GROUP_t));
	memcpy((void *) &pNandInfo->tMinDDR, (const void *) &pNandDrv->tMinDDR, sizeof(DDR_TIMING_GROUP_t));
	#endif
	
	u8_CISIdx = 0;

    /* Search for two good blocks within the first 10 physical blocks */
	for (u16_PBA = 0; u16_PBA < 10; u16_PBA++) {

		/* Reeset NAND driver and FCIE to the original settings */
		pNandDrv->u16_SpareByteCnt = pNandInfo->u16_SpareByteCnt;
		pNandDrv->u16_PageByteCnt  = pNandInfo->u16_PageByteCnt;
		pNandDrv->u16_ECCType      = pNandInfo->u16_ECCType;
		NC_ConfigContext();
		NC_ReInit();
		pNandDrv->u16_Reg48_Spare &= ~(1 << 12);
		//Disable Randomizer for nni Read/Write
		#if defined(FCIE_LFSR) && FCIE_LFSR 
		if(pNandDrv->u8_RequireRandomizer)
			NC_DisableLFSR();
		#endif
		NC_Config();

		/* Check first page of block */
		u32_PageIdx = u16_PBA << pNandDrv->u8_BlkPageCntBits;
		u32_Err = NC_ReadSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);
		if (u32_Err != UNFD_ST_SUCCESS)
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "NC_ReadSectors(0x%lX)=0x%lX\n", u32_PageIdx, u32_Err);
		if (au8_SpareBuf[0] != 0xFF)
		{
			nand_debug(UNFD_DEBUG_LEVEL_WARNING, 1, "Skip bad blk 0x%04x\n", u16_PBA);
			continue;
		}


		u32_Err = NC_EraseBlk(u16_PBA << pNandDrv->u8_BlkPageCntBits);
		if (u32_Err != UNFD_ST_SUCCESS) {
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Erase blk 0x%04x failed with EC: 0x%08lx\n",
				u16_PBA, u32_Err);

			drvNAND_MarkBadBlk(u16_PBA);
			continue;
		}

		pNandDrv->u16_PageByteCnt = 2048;
		pNandDrv->u16_SpareByteCnt = 256;
		pNandDrv->u16_ECCType = NANDINFO_ECC_TYPE;

		NC_ConfigContext();
		NC_ReInit();

		pNandDrv->u16_Reg48_Spare |= (1 << 12);
		NC_Config();

		memset(au8_PageBuf, '\0', pNandDrv->u16_PageByteCnt);
		memcpy(au8_PageBuf, pNandInfo, 512);
		memset(au8_SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt);

		pBlkInfo->u8_BadBlkMark = 0xFF;
		pBlkInfo->u8_PartType = 0;

		u32_Err = NC_WriteSectors(u32_PageIdx, 0, au8_PageBuf, au8_SpareBuf, 1);

		if (u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Write Nand Info failed with EC: 0x%08lx\n", u32_Err);
			drvNAND_MarkBadBlk(u16_PBA);
			continue;
		}

		/* Reset NAND driver and FCIE to the original settings */
		pNandDrv->u16_SpareByteCnt = pNandInfo->u16_SpareByteCnt;
		pNandDrv->u16_PageByteCnt  = pNandInfo->u16_PageByteCnt;
		pNandDrv->u16_ECCType      = pNandInfo->u16_ECCType;
		NC_ConfigContext();
		NC_ReInit();

		pNandDrv->u16_Reg48_Spare &= ~BIT_NC_HW_AUTO_RANDOM_CMD_DISABLE;
		NC_Config();
		
		/*
		**	Write Partition Info the 2nd page
		**/
		if(pNandDrv->u8_HasPNI == 1)
		{
			memset(au8_PageBuf, '\0', pNandDrv->u16_PageByteCnt);
			memcpy(au8_PageBuf, pPartInfo, 512);
		
			u32_Err = NC_WriteSectors(u32_PageIdx+ga_tPairedPageMap[1].u16_LSB, 0, au8_PageBuf, au8_SpareBuf, 1);		
			if (u32_Err != UNFD_ST_SUCCESS) 
			{
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Write Part Info failed with EC: 0x%08lx\n", u32_Err);
				drvNAND_MarkBadBlk(u16_PBA);
				continue;
			}
		}

		/*
		**  Write Paired Page Map to the 4th page
		**/
		if(pNandDrv->u8_CellType == 1)  // MLC
		{
			memset(au8_PageBuf, '\0', pNandDrv->u16_PageByteCnt);
			memcpy(au8_PageBuf, &ga_tPairedPageMap, 2048);
			u32_Err = NC_WritePages(u32_PageIdx+pNandInfo->u8_PairPageMapLoc, au8_PageBuf, au8_SpareBuf, 1);
			if (u32_Err != UNFD_ST_SUCCESS) {
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Write Paired-Page Map failed with EC: 0x%08lx\n", u32_Err);
				drvNAND_MarkBadBlk(u16_PBA);
				continue;
			}
		}

		/*
		if(pNandDrv->u8_RequireReadRetry && pNandDrv->u8_ReadRetryLoc != 0)  // MLC
		{
			memset(au8_PageBuf, '\0', pNandDrv->u16_PageByteCnt);
			memcpy(au8_PageBuf, &gau8_ReadRetryTable, 512);
			u32_Err = NC_WritePages(u32_PageIdx+pNandInfo->u8_ReadRetryLoc, au8_PageBuf, au8_SpareBuf, 1);
			if (u32_Err != UNFD_ST_SUCCESS) {
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Write Customize Read-Retry Table failed with EC: 0x%08lx\n", u32_Err);
				drvNAND_MarkBadBlk(u16_PBA);
				continue;
			}
		}
		*/

		printf("CIS%d is written to blk 0x%04x\n", u8_CISIdx, u16_PBA);

		pNandDrv->u16_CISPBA[u8_CISIdx] = u16_PBA;

		if ((++u8_CISIdx) == 2)
			break;
	}

	/* Reset NAND driver and FCIE to the original settings */
	pNandDrv->u16_SpareByteCnt = pNandInfo->u16_SpareByteCnt;
	pNandDrv->u16_PageByteCnt  = pNandInfo->u16_PageByteCnt;
	pNandDrv->u16_ECCType      = pNandInfo->u16_ECCType;
	NC_ConfigContext();
	NC_ReInit();
	pNandDrv->u16_Reg48_Spare &= ~(1 << 12);
	NC_Config();

	switch (u8_CISIdx) {
		case 0:
			u32_Err = UNFD_ST_ERR_NO_BLK_FOR_CIS0;
			break;
		case 1:
			u32_Err = UNFD_ST_ERR_NO_BLK_FOR_CIS1;
			break;
		case 2:
			u32_Err = UNFD_ST_SUCCESS;
			break;
	}

    return u32_Err;
}


int search_cis_in_DRAM(U8* pu8_CISAddr, U8* pu8_PPMAddr, U8* pu8_PNIAddr, NAND_FLASH_INFO_t *pNandInfoOut)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	NAND_FLASH_INFO_t * pNandInfo;
	PARTITION_INFO_t * pPartInfo;
	U32 u32_i, u32_j, u32_Err;

	NC_PlatformInit();

	NC_ReadID();
	
	printf("Begin ProbeReadSeq \n");
	u32_Err = drvNAND_ProbeReadSeq();
	if(u32_Err != UNFD_ST_SUCCESS)
	{
		printf("Unknow Read Sequence, ErrCode %lx\r\n", u32_Err);
		return u32_Err;
	}

	if(1 == pNandDrv->u8_Flag_004A30)
	{
		pNandDrv->u16_BlkPageCnt = 0x20;
		pNandDrv->u16_PageByteCnt = 0x800;
		pNandDrv->u16_SpareByteCnt = 0x100;
		pNandDrv->u16_ECCType = NANDINFO_ECC_TYPE;

		NC_ConfigContext();
		NC_ReInit();
		pNandDrv->u16_Reg48_Spare |= BIT12;
		NC_Config();
		u32_i = 0;
	LABEL_TRY_005A30:

		u32_Err = NC_ReadSectors(u32_i, 0, u8MainBuf, u8SpareBuf, 1);
		if(u32_Err != UNFD_ST_SUCCESS || u8SpareBuf[0] != 0xFF)
		{
			if(1==pNandDrv->u8_Flag_004A30 && ADR_C5TFS0!=pNandDrv->u8_OpCode_RW_AdrCycle)
			{
				//nand_debug(0,1,"switch to 005A30, 0 \n");
				pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
				pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
				goto LABEL_TRY_005A30;
			}
			else
			{
				pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
				pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
				u32_i += 0x20;
				goto LABEL_TRY_005A30;
			}
		}
	} 

	
	printf("Begin Search CIS in DRAM\n");
	
	//Search 20 pieces of possiable cis memory
	for(u32_i = 0; u32_i < 0x200 * 20; u32_i += 0x200)
	{
		pNandInfo = (NAND_FLASH_INFO_t*)(pu8_CISAddr + u32_i);

		for(u32_j=0; u32_j<pNandInfo->u8_IDByteCnt; u32_j++)
		{
			if(pNandInfo->au8_ID[u32_j] != pNandDrv->au8_ID[u32_j])
				break;
		}
		if(u32_j == pNandInfo->u8_IDByteCnt)
			break;
	}
	if(u32_i == 0x200 *20)
	{
		printf("No available CIS match with current nand flash\n");
		return -1;
	}
	printf("Found CIS in given memory\n");
	drvNAND_ParseNandInfo(pNandInfo);

	NC_ConfigContext();
	/*Re-Search DDR timing again*/
	NC_Init();
	NC_Config();

	nand_config_clock(pNandInfo->u16_tRC);
	
	//search pni
	pNandDrv->u8_HasPNI = 0;
	if(pu8_PNIAddr != NULL)
	{	
		for(u32_i = 0; u32_i < NAND_PARTITAION_BYTE_CNT * 20 ; u32_i += NAND_PARTITAION_BYTE_CNT)
		{
			pPartInfo = (PARTITION_INFO_t*) (pu8_PNIAddr + u32_i);

			if(pPartInfo->u16_SpareByteCnt == pNandDrv->u16_SpareByteCnt &&
				pPartInfo->u16_PageByteCnt == pNandDrv->u16_PageByteCnt &&
				pPartInfo->u16_BlkPageCnt == pNandDrv->u16_BlkPageCnt &&
				pPartInfo->u16_BlkCnt == pNandDrv->u16_BlkCnt)				//Match Partition layout
			{
				memcpy(pNandDrv->pPartInfo, pPartInfo, NAND_PARTITAION_BYTE_CNT);
				dump_part_info(pNandDrv->pPartInfo);
				pNandDrv->u8_HasPNI = 1;
				break;
			}
		}
	}
	//memcpy pair page map

	printf("Cell Type %d, Randomizer %d\n", pNandDrv->u8_CellType, pNandDrv->u8_RequireRandomizer);
	if(pNandDrv->u8_CellType)
		memcpy(&ga_tPairedPageMap, pu8_PPMAddr + (pNandInfo->u8_PairPageMapType * 0x800), 0x800);
	else
	{
		for(u32_i = 0; u32_i < 0x200; u32_i++)
		{
			ga_tPairedPageMap[u32_i].u16_LSB = u32_i;
			ga_tPairedPageMap[u32_i].u16_MSB = u32_i;
		}
	}
	printf("Require Read Retry %d\n", pNandDrv->u8_RequireReadRetry);
//	if(pNandDrv->u8_RequireReadRetry && pu8_ReadRetry != NULL)
//		memcpy(&gau8_ReadRetryTable, pu8_ReadRetry, 0x200);

	memcpy(pNandInfoOut, pNandInfo, 0x200);

	return 0;
}

int nand_write_bootloader(U32 u32_Row,U8 * pu8_addr, U32 u32_size)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	U8 * pu8_DataBuf = pu8_addr;
	U16 u16_BlkPageCnt;
	U32 u32_pagecnt, u32_Err;
	u16_BlkPageCnt = (pNandDrv->u8_CellType) ? pNandDrv->u16_BlkPageCnt/2: pNandDrv->u16_BlkPageCnt;

	u32_pagecnt = u32_size >> pNandDrv->u8_PageByteCntBits;

	#if defined(FCIE_LFSR) && FCIE_LFSR 
	if(pNandDrv->u8_RequireRandomizer)
		NC_DisableLFSR();
	#endif
	while(u32_pagecnt >= u16_BlkPageCnt)
	{	
		while (drvNAND_IsGoodBlk(u32_Row >> pNandDrv->u8_BlkPageCntBits) == 0)
		{
			u32_Row += pNandDrv->u16_BlkPageCnt;
			//bad block jump to next block
			if(u32_Row == (pNandDrv->u16_BlkCnt << pNandDrv->u8_BlkPageCntBits))
			{
				printf("Error : There is no available GOOD block in current nand device\n");
				return -1;
			}
		}
		u32_Err = NC_EraseBlk(u32_Row);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			drvNAND_MarkBadBlk(u32_Row >> pNandDrv->u8_BlkPageCntBits);
			//jump to next block
			u32_Row += pNandDrv->u16_BlkPageCnt;
			continue;
		}

		if(pNandDrv->u8_CellType)
		{
			U16 u16_i, u16_flag = 0;
			U32 u32_TmpRow;
			for(u16_i =0; u16_i < u16_BlkPageCnt; u16_i ++)
			{
				u32_TmpRow = u32_Row + ga_tPairedPageMap[u16_i].u16_LSB;
				u32_Err = NC_WritePages(u32_TmpRow, pu8_DataBuf, NULL, 1);
				if(u32_Err != UNFD_ST_SUCCESS)
				{
					drvNAND_MarkBadBlk(u32_Row >> pNandDrv->u8_BlkPageCntBits);
					//jump to next block
					u32_Row += pNandDrv->u16_BlkPageCnt;
					u16_flag = 1;
					break;
				}
				pu8_DataBuf += pNandDrv->u16_PageByteCnt;
			}
			if(u16_flag == 1)
			{
				pu8_DataBuf -= pNandDrv->u16_PageByteCnt * u16_i;
				continue;
			}
		}
		else
		{
			u32_Err = NC_WritePages(u32_Row, pu8_DataBuf, NULL, pNandDrv->u16_BlkPageCnt);
			if(u32_Err != UNFD_ST_SUCCESS)
			{
				drvNAND_MarkBadBlk(u32_Row >> pNandDrv->u8_BlkPageCntBits);
				//jump to next block
				u32_Row += pNandDrv->u16_BlkPageCnt;
				continue;
			}
			pu8_DataBuf += (u16_BlkPageCnt << pNandDrv->u8_PageByteCntBits);
		}

		u32_pagecnt -= u16_BlkPageCnt;
		u32_size -= (u16_BlkPageCnt << pNandDrv->u8_PageByteCntBits);
		u32_Row += pNandDrv->u16_BlkPageCnt;
	}

	while(u32_size)
	{
		while (drvNAND_IsGoodBlk(u32_Row >> pNandDrv->u8_BlkPageCntBits) == 0)
		{
			u32_Row += pNandDrv->u16_BlkPageCnt;
			//bad block jump to next block
			if(u32_Row == (pNandDrv->u16_BlkCnt << pNandDrv->u8_BlkPageCntBits))
			{
				printf("Error : There is no available GOOD block in current nand device\n");
				return -1;
			}
		}

		u32_Err = NC_EraseBlk(u32_Row);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			drvNAND_MarkBadBlk(u32_Row >> pNandDrv->u8_BlkPageCntBits);
			//jump to next block
			u32_Row += pNandDrv->u16_BlkPageCnt;
			continue;
		}

		u32_pagecnt = u32_size >> pNandDrv->u8_PageByteCntBits;
		if((u32_size & (pNandDrv->u16_PageByteCnt -1)))
			 u32_pagecnt += 1;

		if(pNandDrv->u8_CellType)
		{
			U16 u16_i, u16_flag = 0;
			U32 u32_TmpRow;
			for(u16_i =0; u16_i < u32_pagecnt; u16_i ++)
			{
				u32_TmpRow = u32_Row + ga_tPairedPageMap[u16_i].u16_LSB;
				u32_Err = NC_WritePages(u32_TmpRow, pu8_DataBuf, NULL, 1);
				if(u32_Err != UNFD_ST_SUCCESS)
				{
					drvNAND_MarkBadBlk(u32_Row >> pNandDrv->u8_BlkPageCntBits);
					//jump to next block
					u32_Row += pNandDrv->u16_BlkPageCnt;
					u16_flag = 1;
					break;
				}
				pu8_DataBuf += pNandDrv->u16_PageByteCnt;
			}
			if(u16_flag == 1)
			{
				pu8_DataBuf -= pNandDrv->u16_PageByteCnt * u16_i;
				continue;
			}
		}
		else
		{
		    u32_Err = NC_WritePages(u32_Row, pu8_DataBuf, NULL, u32_pagecnt);
		    if(u32_Err != UNFD_ST_SUCCESS)
		    {
			    drvNAND_MarkBadBlk(u32_Row >> pNandDrv->u8_BlkPageCntBits);
			    //jump to next block
			    u32_Row += pNandDrv->u16_BlkPageCnt;
			    continue;
		    }
		}

		u32_size-= u32_size;
	}
	/*
	for (u32_i = 0; u32_i < u32_pagecnt; u32_i ++)
	{
		NC_ReadPages(u32_Row + u32_i, u8MainBuf, u8SpareBuf, 1);
		for( u32_j = 0; u32_j < pNandDrv->u16_PageByteCnt; u32_j ++)
		{
			printf("%X ", u8MainBuf[u32_i]);
			if((u32_j & 0xF) == 0xF)
				printf("\n");
		}
		printf("Spare ECC Code\n");
		for( u32_j = 0; u32_j < pNandDrv->u16_SpareByteCnt; u32_j ++)
		{
			printf("%X ", u8SpareBuf[u32_i]);
			if((u32_j & 0xF) == 0xF)
				printf("\n");
		}
	}
	NC_EraseBlk(u32_Row);
	*/
	return 0;
}

int drvNAND_ReadCISBlk(U32 u32_off, U8* pu8_DataBuf)
{
	NAND_DRIVER * pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 u32_Ret, i;
	U32 u32_Row = u32_off >> pNandDrv->u8_PageByteCntBits;
	U16 u16_BlkCntTmp, u16_BlkPageCntTmp, u16_PageByteCntTmp, u16_SpareByteCntTmp, u16_ECCTypeTmp;
	U32 page_off = pNandDrv->u16_PageByteCnt + pNandDrv->u16_SpareByteCnt;
	
	//set 0xff for nni page
	memset((void*)pu8_DataBuf, 0xff, pNandDrv->u16_PageByteCnt);
	u16_BlkCntTmp = pNandDrv->u16_BlkCnt;
	u16_BlkPageCntTmp = pNandDrv->u16_BlkPageCnt;
	u16_PageByteCntTmp = pNandDrv->u16_PageByteCnt;
	u16_SpareByteCntTmp = pNandDrv->u16_SpareByteCnt;
	u16_ECCTypeTmp = pNandDrv->u16_ECCType;
	//reconfig nni read setting

	pNandDrv->u16_BlkCnt = 0x400;
	pNandDrv->u16_BlkPageCnt = 0x20;
	pNandDrv->u16_PageByteCnt = 0x800;
	pNandDrv->u16_SpareByteCnt = 0x100;
	pNandDrv->u16_ECCType = NANDINFO_ECC_TYPE;

	#if defined(FCIE_LFSR) && FCIE_LFSR 
	if(pNandDrv->u8_RequireRandomizer)
		NC_DisableLFSR();
	#endif

	NC_ConfigContext();
	NC_ReInit();
	pNandDrv->u16_Reg48_Spare |= (1 << 12);
	NC_Config();
	
	u32_Ret = NC_ReadSectors(u32_Row, 0, pu8_DataBuf, (U8*)((U32)pu8_DataBuf + pNandDrv->u16_SectorByteCnt), 1);
	if(u32_Ret != 0)
		return u32_Ret;
	
	//restore original setting
	pNandDrv->u16_BlkCnt = u16_BlkCntTmp;
	pNandDrv->u16_BlkPageCnt = u16_BlkPageCntTmp;
	pNandDrv->u16_PageByteCnt = u16_PageByteCntTmp;
	pNandDrv->u16_SpareByteCnt = u16_SpareByteCntTmp;
	pNandDrv->u16_ECCType = u16_ECCTypeTmp;

	NC_ConfigContext();
	NC_ReInit();
	pNandDrv->u16_Reg48_Spare &= ~(1 << 12);
	NC_Config();
	
	for(i = 1 ; i < pNandDrv->u16_BlkPageCnt; i ++)
	{
		u32_Ret = NC_ReadPages(u32_Row + i, (U8*) ((U32)pu8_DataBuf + page_off * i), 
							(U8*) ((U32)pu8_DataBuf + (page_off * i) + pNandDrv->u16_PageByteCnt), 1);
		if(u32_Ret)
			return u32_Ret;
	}

	return UNFD_ST_SUCCESS;
}

void drvNAND_GetMtdParts(char *buf)
{
	NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
	PARTITION_INFO_t *pPartInfo = pNandDrv->pPartInfo;
	U8 u8_i,u8_PartNo, u8_Unknown = 0;
	int len, Maxlen = 512;
	u32 PartSize = 0;
	u32 virtual_boot;
	char *mtd_buf=buf, tmp[32], PartName[16];
	U16 u16_LastPartType;

	if(pNandDrv->u8_HasPNI == 0)
		goto cleanup;

	sprintf(mtd_buf, "mtdparts=edb64M-nand:");
	//printf("%s\r\n",buf);
	mtd_buf += 21;
	if (Maxlen < 21)
		goto cleanup;
	Maxlen -= 21;
		
	u16_LastPartType = 0;
	for(u8_i = 0; u8_i < pPartInfo->u16_PartCnt; u8_i ++)
	{
		if( (pPartInfo->records[u8_i].u16_PartType & UNFD_LOGI_PART) == UNFD_LOGI_PART)
			break;
		
		PartSize = 0;
		if( u8_i )
		{
			memcpy(mtd_buf, ",", 1);
			mtd_buf ++;
			Maxlen --;
		}
		
		PartSize = pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt *
			(pPartInfo->records[u8_i].u16_BackupBlkCnt + pPartInfo->records[u8_i].u16_BlkCnt);
		
		u8_PartNo = u16_LastPartType != pPartInfo->records[u8_i].u16_PartType ? 0:1;

		u16_LastPartType = pPartInfo->records[u8_i].u16_PartType;

		switch(pPartInfo->records[u8_i].u16_PartType)
		{
			case UNFD_PART_BOOTLOGO:
				sprintf(PartName,"UBILD" );
				break;				
			case UNFD_PART_UBOOT:
				sprintf(PartName,"MBOOT" );
				break;
			case UNFD_PART_OS:
				sprintf(PartName,"KL" );
				break;
			case UNFD_PART_SECINFO:
				sprintf(PartName,"KL_BP" );
				break;
			case UNFD_PART_OTP:
				sprintf(PartName,"NTP" );
				break;
			case UNFD_PART_RECOVERY:
				sprintf(PartName,"RECOVERY" );
				break;
			case UNFD_PART_MISC:
				sprintf(PartName,"MISC" );
				break;	
			case UNFD_PART_TBL:
				sprintf(PartName,"TBL" );
				break;	
			case UNFD_PART_CTRL:
				sprintf(PartName,"CTRL" );
				break;	
			case UNFD_PART_UBIRO:
				sprintf(PartName,"UBIRO" );
				break;	
			default:
				sprintf(PartName,"UNKNOWN%d", u8_Unknown++);
				break;	
		}
		if (u8_i)
			sprintf(tmp, "%dk(%s)", PartSize/1024, PartName); //,pPartInfo->records[u8_i].u16_PartType);
		else 
		{
			if(pNandDrv->u8_BL1PBA >= 10)
			{
				virtual_boot = (pNandDrv->u8_BL1PBA + 1) * (pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt) / 1024;
				PartSize = PartSize - ((pNandDrv->u8_BL1PBA + 1 - 10) * (pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt));
			}
			else if(pNandDrv->u8_HashPBA[2][1] >= 10)
			{
				virtual_boot = (pNandDrv->u8_HashPBA[2][1] + 1) * (pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt) / 1024;
				PartSize = PartSize - (pNandDrv->u8_HashPBA[2][1] + 1 - 10) * (pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt) ;
			}
			else
				virtual_boot = (10 * pNandDrv->u16_BlkPageCnt * pNandDrv->u16_PageByteCnt) / 1024;


			sprintf(tmp, "%dk@%dk(%s)", PartSize/1024, virtual_boot, PartName);
		}
		len = strlen(tmp);
		memcpy(mtd_buf, tmp, len);
		mtd_buf += len;
		if (Maxlen < len)
			goto cleanup;
		Maxlen -= len;
		
	}
	if( (pPartInfo->records[u8_i].u16_PartType & UNFD_LOGI_PART) == UNFD_LOGI_PART)
	{
		sprintf(tmp,",-(UBI)");
		len= strlen(tmp);
		memcpy(mtd_buf, tmp, len);
		mtd_buf += len;
		if (Maxlen < len)
			goto cleanup;
		Maxlen -= len;
	}
	*mtd_buf = '\0';
	return ;
	cleanup:
	buf[0] = '\0';
	return ;
}

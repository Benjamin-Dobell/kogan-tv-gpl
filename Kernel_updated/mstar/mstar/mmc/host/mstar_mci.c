////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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

#include "mstar_mci.h"

/******************************************************************************
 * Defines
 ******************************************************************************/


/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
#define FL_SENT_COMMAND				(1 << 0)
#define FL_SENT_STOP				(1 << 1)

#define MCI_RETRY_CNT_CMD_TO        100
#define MCI_RETRY_CNT_CRC_ERR       200 // avoid stack overflow
#define MCI_RETRY_CNT_OK_CLK_UP     100

static void mstar_mci_send_command(struct mstar_mci_host *pMStarHost_st, struct mmc_command *pCmd_st);
static void mstar_mci_process_next(struct mstar_mci_host *pMStarHost_st);
static void mstar_mci_completed_command(struct mstar_mci_host *pMStarHost_st);
#if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
static void mstar_mci_completed_command_FromRAM(struct mstar_mci_host *pMStarHost_st);
#endif
static void mstar_mci_request(struct mmc_host *pMMCHost_st, struct mmc_request *pMRQ_st);
static void mstar_mci_set_ios(struct mmc_host *pMMCHost_st, struct mmc_ios *pIOS_st);
static  s32 mstar_mci_get_ro(struct mmc_host *pMMCHost_st);

u32 mstar_mci_WaitD0High(u32 u32_us);

/*****************************************************************************
 * Define Static Global Variables
 ******************************************************************************/
/* MSTAR Multimedia Card Interface Operations */
static const struct mmc_host_ops sg_mstar_mci_ops =
{
    .request =	mstar_mci_request,
    .set_ios =	mstar_mci_set_ios,
    .get_ro =	mstar_mci_get_ro,
};

static struct task_struct *sgp_eMMCThread_st = NULL;
static ulong wr_seg_size = 0;

/*****************************************************************************
 * for profiling
 ******************************************************************************/
#if defined(CONFIG_MMC_MSTAR_MMC_EMMC_LIFETEST)
static struct proc_dir_entry * writefile;
const char procfs_name[] = "StorageBytes";

int procfile_read(char* buffer, char ** buffer_location, off_t offset,
					int buffer_length, int *eof, void *data)
{
	int ret;
	if(offset > 0)
		ret = 0;
	else
		ret = sprintf(buffer,"TotalWriteBytes %llu GB %llu MB\nTotalReadBytes %llu GB %llu MB\n", 
				g_eMMCDrv.u64_CNT_TotalWBlk/1024/1024/2, (g_eMMCDrv.u64_CNT_TotalWBlk/1024/2) % 1024,
				g_eMMCDrv.u64_CNT_TotalRBlk/1024/1024/2, (g_eMMCDrv.u64_CNT_TotalRBlk/1024/2) % 1024);
	return ret;
}
#endif
/******************************************************************************
 * Functions
 ******************************************************************************/
static void mstar_mci_pre_dma_read(struct mstar_mci_host *pMStarHost_st)
{
    /* Define Local Variables */
    struct scatterlist *pSG_st = 0;
    struct mmc_command *pCmd_st = 0;
    struct mmc_data *pData_st = 0;
    //int sg_count = 0;
    u32 u32_dmalen = 0, u32_sg_count;
    dma_addr_t dmaaddr = 0;

    pCmd_st = pMStarHost_st->cmd;
    pData_st = pCmd_st->data;

    u32_sg_count = dma_map_sg(mmc_dev(pMStarHost_st->mmc), pData_st->sg, pData_st->sg_len, DMA_FROM_DEVICE);
    pSG_st = &pData_st->sg[0];
    dmaaddr = (u32)sg_dma_address(pSG_st);
    u32_dmalen = sg_dma_len(pSG_st);
	u32_dmalen = ((u32_dmalen&0x1FF)?1:0) + u32_dmalen/512;

	eMMC_FCIE_MIU0_MIU1_SEL(dmaaddr);

	if(g_eMMCDrv.u32_DrvFlag & DRV_FLAG_DDR_MODE)
	{
		REG_FCIE_W(FCIE_TOGGLE_CNT, BITS_8_R_TOGGLE_CNT);
        REG_FCIE_SETBIT(FCIE_MACRO_REDNT, BIT_TOGGLE_CNT_RST);
		REG_FCIE_CLRBIT(FCIE_MACRO_REDNT, BIT_MACRO_DIR);
		eMMC_hw_timer_delay(TIME_WAIT_FCIE_RST_TOGGLE_CNT); // Brian needs 2T
		REG_FCIE_CLRBIT(FCIE_MACRO_REDNT, BIT_TOGGLE_CNT_RST);
	}
	REG_FCIE_W(FCIE_JOB_BL_CNT, u32_dmalen);
    REG_FCIE_W(FCIE_SDIO_ADDR0,(((u32)dmaaddr) & 0xFFFF));
    REG_FCIE_W(FCIE_SDIO_ADDR1,(((u32)dmaaddr) >> 16));
	REG_FCIE_CLRBIT(FCIE_MMA_PRI_REG, BIT_DMA_DIR_W);
    eMMC_FCIE_FifoClkRdy(0);
    REG_FCIE_SETBIT(FCIE_PATH_CTRL, BIT_MMA_EN);

	#if defined(eMMC_PROFILE_WR) && eMMC_PROFILE_WR
    g_eMMCDrv.u64_CNT_TotalRBlk += u32_dmalen;
	g_eMMCDrv.u32_RBlk_tmp = u32_dmalen;
	#endif

}


static void mstar_mci_post_dma_read(struct mstar_mci_host *pMStarHost_st)
{
    /* Define Local Variables */
    struct mmc_command	*pCmd_st = 0;
    struct mmc_data		*pData_st = 0;
    struct scatterlist	*pSG_st = 0;
    int i;
    u32 dmalen = 0;
    dma_addr_t dmaaddr = 0;

    pCmd_st = pMStarHost_st->cmd;
    pData_st = pCmd_st->data;
    pSG_st = &(pData_st->sg[0]);
    pData_st->bytes_xfered += pSG_st->length;

	// [WHY] not dma_map_sg ?
    for(i=1; i<pData_st->sg_len; i++)
    {
		eMMC_FCIE_ClearEvents_Reg0();
		//REG_FCIE_W(FCIE_MIE_EVENT, BIT_ALL_CARD_INT_EVENTS);
        
        pSG_st = &(pData_st->sg[i]);
        dmaaddr = sg_dma_address(pSG_st);
        dmalen = sg_dma_len(pSG_st);

        eMMC_FCIE_MIU0_MIU1_SEL(dmaaddr);

		REG_FCIE_W(FCIE_JOB_BL_CNT,(dmalen/512));
        REG_FCIE_W(FCIE_SDIO_ADDR0,(((u32)dmaaddr) & 0xFFFF));
        REG_FCIE_W(FCIE_SDIO_ADDR1,(((u32)dmaaddr) >> 16));
		REG_FCIE_SETBIT(FCIE_PATH_CTRL, BIT_MMA_EN);
        
        #if defined(ENABLE_eMMC_INTERRUPT_MODE) && ENABLE_eMMC_INTERRUPT_MODE
		REG_FCIE_SETBIT(FCIE_MIE_INT_EN, BIT_MIU_LAST_DONE);
        #endif
		REG_FCIE_W(FCIE_SD_CTRL, BIT_SD_DAT_EN);
		
		if(eMMC_FCIE_WaitEvents(FCIE_MIE_EVENT, BIT_MIU_LAST_DONE,
			eMMC_GENERIC_WAIT_TIME) != eMMC_ST_SUCCESS)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: r timeout \n");
			//eMMC_die("\n");
			g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_ERROR_RETRY;
			break; // goto error retry
        }
		pData_st->bytes_xfered += pSG_st->length;


		// -----------------------------------
		#if defined(eMMC_PROFILE_WR) && eMMC_PROFILE_WR
        g_eMMCDrv.u64_CNT_TotalRBlk += (dmalen / 512);
		g_eMMCDrv.u32_RBlk_tmp += (dmalen / 512);
		#endif		
    }

	#if defined(eMMC_PROFILE_WR) && eMMC_PROFILE_WR
	if(g_eMMCDrv.u32_RBlk_tmp < 0x200)
		g_eMMCDrv.au32_CNT_MinRBlk[g_eMMCDrv.u32_RBlk_tmp]++;
	else if(g_eMMCDrv.u32_RBlk_tmp > g_eMMCDrv.u32_CNT_MaxRBlk)
		g_eMMCDrv.u32_CNT_MaxRBlk = g_eMMCDrv.u32_RBlk_tmp;
	else if(g_eMMCDrv.u32_RBlk_tmp < g_eMMCDrv.u32_CNT_MinRBlk)
		g_eMMCDrv.u32_CNT_MinRBlk = g_eMMCDrv.u32_RBlk_tmp;

	g_eMMCDrv.u32_Addr_RLast += g_eMMCDrv.u32_RBlk_tmp;
	g_eMMCDrv.u32_RBlk_tmp = 0;
	#endif
	
    // -----------------------------------
	dma_unmap_sg(mmc_dev(pMStarHost_st->mmc), pData_st->sg, pData_st->sg_len, DMA_FROM_DEVICE);

	if(g_eMMCDrv.u32_DrvFlag & DRV_FLAG_DDR_MODE)
		REG_FCIE_SETBIT(FCIE_MACRO_REDNT, BIT_MACRO_DIR);
}


static s32 mstar_mci_handle_cmdrdy(struct mstar_mci_host *pMStarHost_st)
{
    struct mmc_command	*pCmd_st = 0;
    struct mmc_data		*pData_st = 0;
    struct scatterlist	*pSG_st = 0;
    int i;
    int sg_count;
    u32 dmalen = 0, split_len = 0;
    dma_addr_t dmaaddr = 0;

	if(g_eMMCDrv.u32_DrvFlag & DRV_FLAG_DDR_MODE)
	{
		REG_FCIE_W(FCIE_TOGGLE_CNT, BITS_8_W_TOGGLE_CNT);
		REG_FCIE_SETBIT(FCIE_MACRO_REDNT, BIT_MACRO_DIR);
	}
    pCmd_st = pMStarHost_st->cmd;
    pData_st = pCmd_st->data;
    
    sg_count = dma_map_sg(mmc_dev(pMStarHost_st->mmc), pData_st->sg, pData_st->sg_len, DMA_TO_DEVICE);

    for(i=0; i<sg_count; i++)
    {
        pSG_st = &(pData_st->sg[i]);
        dmaaddr = sg_dma_address(pSG_st);
		eMMC_FCIE_MIU0_MIU1_SEL(dmaaddr);
        dmalen = sg_dma_len(pSG_st)>>eMMC_SECTOR_BYTECNT_BITS;		        
        
        do{
		    eMMC_FCIE_ClearEvents_Reg0();
		    if(eMMC_ST_SUCCESS != mstar_mci_WaitD0High(TIME_WAIT_DAT0_HIGH))
		    {
			    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: wait D0 H TO\n");
			    eMMC_FCIE_ErrHandler_Stop();
		    }
            split_len = 
				dmalen > g_eMMCDrv.BurstWriteLen_t.u16_BestBrustLen ? 
				g_eMMCDrv.BurstWriteLen_t.u16_BestBrustLen : dmalen;
			split_len = 
				split_len == g_eMMCDrv.BurstWriteLen_t.u16_WorstBrustLen ? 
				split_len>>1 : split_len;
				
            REG_FCIE_W(FCIE_JOB_BL_CNT, split_len);
            REG_FCIE_W(FCIE_SDIO_ADDR0, (dmaaddr & 0xFFFF));
            REG_FCIE_W(FCIE_SDIO_ADDR1, (dmaaddr >> 16));
            REG_FCIE_SETBIT(FCIE_MMA_PRI_REG, BIT_DMA_DIR_W);
    		if(0==i)
    			eMMC_FCIE_FifoClkRdy(BIT_DMA_DIR_W);
            REG_FCIE_SETBIT(FCIE_PATH_CTRL, BIT_MMA_EN);
    		
            #if defined(ENABLE_eMMC_INTERRUPT_MODE) && ENABLE_eMMC_INTERRUPT_MODE
    		REG_FCIE_SETBIT(FCIE_MIE_INT_EN, BIT_CARD_DMA_END);
            #endif		
    		REG_FCIE_W(FCIE_SD_CTRL, BIT_SD_DAT_EN|BIT_SD_DAT_DIR_W);
    		
    		if(eMMC_FCIE_WaitEvents(FCIE_MIE_EVENT, BIT_CARD_DMA_END,
    			eMMC_GENERIC_WAIT_TIME) != eMMC_ST_SUCCESS)
            {
                eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: w timeout \n");
    			//eMMC_die("\n");
    			g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_ERROR_RETRY;
    			break; // goto error retry
            }

			dmalen -= split_len;
			dmaaddr += split_len << eMMC_SECTOR_BYTECNT_BITS;			

		    // -----------------------------------
		    #if defined(eMMC_PROFILE_WR) && eMMC_PROFILE_WR
            g_eMMCDrv.u64_CNT_TotalWBlk += (split_len / 512);
		    g_eMMCDrv.u32_WBlk_tmp += (split_len / 512);
		    #endif			
            
        }while(dmalen > 0);
		
        pData_st->bytes_xfered += pSG_st->length;        
    }

	#if defined(eMMC_PROFILE_WR) && eMMC_PROFILE_WR
	if(g_eMMCDrv.u32_WBlk_tmp < 0x200)
		g_eMMCDrv.au32_CNT_MinWBlk[g_eMMCDrv.u32_WBlk_tmp]++;
	else if(g_eMMCDrv.u32_WBlk_tmp > g_eMMCDrv.u32_CNT_MaxWBlk)
		g_eMMCDrv.u32_CNT_MaxWBlk = g_eMMCDrv.u32_WBlk_tmp;
	else if(g_eMMCDrv.u32_WBlk_tmp < g_eMMCDrv.u32_CNT_MinWBlk)
		g_eMMCDrv.u32_CNT_MinWBlk = g_eMMCDrv.u32_WBlk_tmp;

	g_eMMCDrv.u32_Addr_WLast += g_eMMCDrv.u32_WBlk_tmp;
	g_eMMCDrv.u32_WBlk_tmp = 0;
	#endif

	// -----------------------------------
    dma_unmap_sg(mmc_dev(pMStarHost_st->mmc), pData_st->sg, pData_st->sg_len, DMA_TO_DEVICE);
    return 1;
}


//------------------------------------------------------
// mstar_mci_request ->
// mstar_mci_process_next -> mstar_mci_send_command -> mstar_mci_completed_command ->
// mstar_mci_process_next -> mmc_request_done
//------------------------------------------------------
static void mstar_mci_completed_command(struct mstar_mci_host *pMStarHost_st)
{
    /* Define Local Variables */
    u16 u16_st, u16_i;
	u8 *pTemp;
	struct mmc_command *pCmd_st = pMStarHost_st->cmd;
	static u32 u32_retry_cnt=0, u32_ok_cnt=0, u32_run_cnt=0;

	u32_run_cnt++;
	// ----------------------------------
	// retrun response from FCIE to mmc driver
    pTemp = (u8*)&(pCmd_st->resp[0]);
    for(u16_i=0; u16_i < 15; u16_i++)
    {
        pTemp[(3 - (u16_i % 4)) + (4 * (u16_i / 4))] =
            (u8)(REG_FCIE(FCIE1_BASE+(((u16_i+1)/2)*4)) >> (8*((u16_i+1)%2)));
    }
	#if 0
	eMMC_debug(0,0,"------------------\n");
	eMMC_debug(0,1,"resp[0]: %08Xh\n", pCmd_st->resp[0]);
	eMMC_debug(0,1,"CIFC: %04Xh %04Xh %04Xh \n", REG_FCIE(FCIE_CIFC_BASE_ADDR), 
		REG_FCIE(FCIE_CIFC_BASE_ADDR+4), REG_FCIE(FCIE_CIFC_BASE_ADDR+8));
	//eMMC_dump_mem(pTemp, 0x10);
	eMMC_debug(0,0,"------------------\n");
    #endif

	// ----------------------------------
    u16_st = REG_FCIE(FCIE_SD_STATUS);
	if((u16_st & BIT_SD_FCIE_ERR_FLAGS) || mstar_SD_CardChange() 
		|| (g_eMMCDrv.u32_DrvFlag & DRV_FLAG_ERROR_RETRY) || (pCmd_st->resp[0]&eMMC_ERR_R1_NEED_RETRY)) 
	{
		g_eMMCDrv.u32_DrvFlag &= ~DRV_FLAG_ERROR_RETRY;
			
        if((u16_st & BIT_SD_RSP_CRC_ERR) && !(mmc_resp_type(pCmd_st) & MMC_RSP_CRC))
        {
            pCmd_st->error = 0;
			u32_retry_cnt = 0;  u32_ok_cnt++;
        }
        else
        {
			#if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			    "eMMC Err: ST:%Xh, CMD:%u, retry: %u, flag: %Xh, R1 err: %Xh\n",
				u16_st, pCmd_st->opcode, u32_retry_cnt, g_eMMCDrv.u32_DrvFlag,
				pCmd_st->resp[0]&eMMC_ERR_R1_NEED_RETRY);

			u32_ok_cnt = 0;
			if(u32_retry_cnt++ >= MCI_RETRY_CNT_CRC_ERR)
			    eMMC_FCIE_ErrHandler_Stop(); // fatal error

			if(25==pCmd_st->opcode || 12==pCmd_st->opcode)
				eMMC_CMD12_NoCheck(g_eMMCDrv.u16_RCA);

			eMMC_hw_timer_sleep(1);
			eMMC_FCIE_ErrHandler_ReInit();			
			
			if(0 != pCmd_st->data)
			{
				eMMC_FCIE_ErrHandler_Retry(); // slow dwon clock
				mstar_mci_send_command(pMStarHost_st, pMStarHost_st->request->cmd);
			}
			else // for CMD12
			{
				eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC Info: no data, just reset eMMC. \n");
				REG_FCIE_W(FCIE_SD_STATUS, BIT_SD_FCIE_ERR_FLAGS);
				mstar_mci_completed_command_FromRAM(pMStarHost_st); 
				return;
			}
			
			if(0==u32_retry_cnt)
			{
				eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC Info: retry ok \n");
				return;
			}
			
			//-----------------------------------------
			#else
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Warn: ST:%Xh, CMD:%u, retry:%u \n",
				u16_st, pCmd_st->opcode, pCmd_st->retries);
			#endif

			// should be trivial
			if(u16_st & (BIT_SD_RSP_TIMEOUT | BIT_SD_W_FAIL))
                pCmd_st->error = -ETIMEDOUT;
            else if(u16_st & (BIT_SD_RSP_CRC_ERR | BIT_SD_R_CRC_ERR | BIT_SD_W_CRC_ERR))
            	pCmd_st->error = -EILSEQ;
            else
                pCmd_st->error = -EIO;
        }
    }
    else
    {
        pCmd_st->error = 0;
		
		u32_retry_cnt = 0;  u32_ok_cnt++;
    }

    if((pCmd_st->opcode == 17) || (pCmd_st->opcode == 18) //read
        ||(pCmd_st->opcode == 24) || (pCmd_st->opcode == 25) //write
        ||(pCmd_st->opcode == 12))  //stop transmission
    {
       if(pCmd_st->resp[0] & eMMC_ERR_R1_31_0)
       {
         pCmd_st->error |= -EIO;         
         if(pCmd_st->opcode == 12)
             eMMC_debug(0,0, "eMMC Warn: CMD12 R1 error: %Xh \n", 
                 pCmd_st->resp[0]);
         else
             eMMC_debug(0,0, "eMMC Warn: CMD%u R1 error: %Xh, arg %08x, blocks %08x\n", 
                 pCmd_st->opcode, pCmd_st->resp[0], pCmd_st->arg, pCmd_st->data->blocks);
	   }
    }

	if(MCI_RETRY_CNT_OK_CLK_UP == u32_ok_cnt)
		eMMC_FCIE_ErrHandler_RestoreClk();

    REG_FCIE_W(FCIE_SD_STATUS, BIT_SD_FCIE_ERR_FLAGS);
    mstar_mci_process_next(pMStarHost_st);
}


#define WAIT_D0H_POLLING_TIME    HW_TIMER_DELAY_100us
u32 mstar_mci_WaitD0High(u32 u32_us)
{
	u32 u32_cnt, u32_wait;

	for(u32_cnt=0; u32_cnt<u32_us; u32_cnt+=WAIT_D0H_POLLING_TIME)
	{
		u32_wait = eMMC_FCIE_WaitD0High_Ex(WAIT_D0H_POLLING_TIME);
		if(u32_wait < WAIT_D0H_POLLING_TIME)
		{
			#if 0
			if(u32_cnt + u32_wait)
				printk("eMMC wait d0: %u us\n", u32_cnt+u32_wait);
			#endif
		    return eMMC_ST_SUCCESS;		
		}

		//if(u32_cnt > HW_TIMER_DELAY_1ms)
		{
			//msleep(1);
			  //schedule_hrtimeout is more precise and can reduce idle time of emmc
        {
            ktime_t expires = ktime_add_ns(ktime_get(), 1000 * 1000);
            set_current_state(TASK_UNINTERRUPTIBLE);
            schedule_hrtimeout(&expires, HRTIMER_MODE_ABS);
        }
			u32_cnt += HW_TIMER_DELAY_1ms;
		}		
	}
	return eMMC_ST_ERR_TIMEOUT_WAITD0HIGH;
}

static void mstar_mci_send_command
(
	struct mstar_mci_host *pMStarHost_st, struct mmc_command *pCmd_st
)
{
    struct mmc_data *pData_st;
    u32 u32_mie_int=0, u32_sd_ctl=0, u32_sd_mode;
	u8  u8_retry_cmd=0, u8_retry_data=0;

	LABEL_SEND_CMD:
    u32_sd_mode = g_eMMCDrv.u16_Reg10_Mode;
	pMStarHost_st->cmd = pCmd_st;
	pData_st = pCmd_st->data;

	eMMC_FCIE_ClearEvents();
	if(12!=pCmd_st->opcode)
		if(eMMC_ST_SUCCESS != mstar_mci_WaitD0High(TIME_WAIT_DAT0_HIGH))
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: wait D0 H TO\n");
			eMMC_FCIE_ErrHandler_Stop();
		}

    if(pData_st)
    {
		pData_st->bytes_xfered = 0;
        REG_FCIE_W(FCIE_SDIO_CTRL, BIT_SDIO_BLK_MODE|(u16)pData_st->blksz);
		
        if (pData_st->flags & MMC_DATA_READ)
        {
            u32_sd_ctl |= BIT_SD_DAT_EN;

            // Enable stoping read clock when using scatter list DMA
            if((pData_st->sg_len > 1) && (pCmd_st->opcode == 18))
            	u32_sd_mode |= BIT_SD_DMA_R_CLK_STOP;
            else
            	u32_sd_mode &= ~BIT_SD_DMA_R_CLK_STOP;

			REG_FCIE_W(FCIE_SD_MODE, u32_sd_mode);
			mstar_mci_pre_dma_read(pMStarHost_st);
        }
    }

    u32_sd_ctl |= BIT_SD_CMD_EN;
    u32_mie_int |= BIT_SD_CMD_END;
    REG_FCIE_W(FCIE_CIFC_ADDR(0), (((pCmd_st->arg >> 24)<<8) | (0x40|pCmd_st->opcode)));
    REG_FCIE_W(FCIE_CIFC_ADDR(1), ((pCmd_st->arg & 0xFF00) | ((pCmd_st->arg>>16)&0xFF)));
    REG_FCIE_W(FCIE_CIFC_ADDR(2), (pCmd_st->arg & 0xFF));

    if(mmc_resp_type(pCmd_st) == MMC_RSP_NONE)
    {
        u32_sd_ctl &= ~BIT_SD_RSP_EN;
        REG_FCIE_W(FCIE_RSP_SIZE, 0);
    }
    else
    {
		u32_sd_ctl |= BIT_SD_RSP_EN;
        if(mmc_resp_type(pCmd_st) == MMC_RSP_R2)
        {
            u32_sd_ctl |= BIT_SD_RSPR2_EN;
            REG_FCIE_W(FCIE_RSP_SIZE, 16); /* (136-8)/8 */
        }
        else
        {
            REG_FCIE_W(FCIE_RSP_SIZE, 5); /*(48-8)/8 */
        }
    }

    #if defined(ENABLE_eMMC_INTERRUPT_MODE) && ENABLE_eMMC_INTERRUPT_MODE
	//eMMC_debug(eMMC_DEBUG_LEVEL_LOW,1,"eMMC: INT_EN: %Xh\n", u32_mie_int);
	REG_FCIE_W(FCIE_MIE_INT_EN, u32_mie_int);
	#endif

	#if 0
	//eMMC_debug(0,0,"\n");
	//eMMC_debug(0,0,"cmd:%u, arg:%Xh, databuf:%Xh, ST:%Xh, mode:%Xh, ctrl:%Xh \n",
	//	pCmd_st->opcode, pCmd_st->arg, (u32)pData_st, 
	//	REG_FCIE(FCIE_SD_STATUS), u32_sd_mode, u32_sd_ctl);
	//while(1);
	#endif
	#if defined(eMMC_PROFILE_WR) && eMMC_PROFILE_WR
    switch(pCmd_st->opcode)
    {
		case 17: 
		case 18:
			if(18 == pCmd_st->opcode)
				g_eMMCDrv.u32_CNT_CMD18++; 
			else if(17 == pCmd_st->opcode)
				g_eMMCDrv.u32_CNT_CMD17++; 
			
		    if(g_eMMCDrv.u32_Addr_RLast == pCmd_st->arg)
				g_eMMCDrv.u32_Addr_RHitCnt++;
			else 
				g_eMMCDrv.u32_Addr_RLast = pCmd_st->arg;
			
		case 24: 
		case 25:
			if(25 == pCmd_st->opcode)
				g_eMMCDrv.u32_CNT_CMD25++; 
			else if(24 == pCmd_st->opcode)
				g_eMMCDrv.u32_CNT_CMD24++; 
			
		    if(g_eMMCDrv.u32_Addr_WLast == pCmd_st->arg)
				g_eMMCDrv.u32_Addr_WHitCnt++;
			else
				g_eMMCDrv.u32_Addr_WLast = pCmd_st->arg;
    }
	
	#endif

	// -----------------------------------
	REG_FCIE_W(FCIE_SD_MODE, u32_sd_mode);
    REG_FCIE_W(FCIE_SD_CTRL, u32_sd_ctl);
    // [FIXME]: retry and timing, and omre...
    if(eMMC_FCIE_WaitEvents(FCIE_MIE_EVENT, BIT_SD_CMD_END,
		eMMC_GENERIC_WAIT_TIME) != eMMC_ST_SUCCESS || 
		(REG_FCIE(FCIE_SD_STATUS)&(BIT_SD_RSP_TIMEOUT|BIT_SD_RSP_CRC_ERR)))// || 0==u8_retry_cmd)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			"eMMC Err: cmd.%u arg.%Xh, retry %u \n", 
			pCmd_st->opcode, pCmd_st->arg, u8_retry_cmd);

		if(u8_retry_cmd < MCI_RETRY_CNT_CMD_TO)
		{
			u8_retry_cmd++;
			if(12==pCmd_st->opcode)
		    {
				if(eMMC_ST_SUCCESS != mstar_mci_WaitD0High(TIME_WAIT_DAT0_HIGH))
		        {
			        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: wait D0 H TO\n");
			        eMMC_FCIE_ErrHandler_Stop();
		        }
				eMMC_FCIE_ErrHandler_ReInit();
				eMMC_FCIE_ErrHandler_Retry();
				#if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
				mstar_mci_completed_command_FromRAM(pMStarHost_st);
				#endif
				return;
		    }
			
			if(25==pCmd_st->opcode)
				eMMC_CMD12_NoCheck(g_eMMCDrv.u16_RCA);
			
			eMMC_FCIE_ErrHandler_ReInit();
			eMMC_FCIE_ErrHandler_Retry();
			goto LABEL_SEND_CMD;
		}
		eMMC_FCIE_ErrHandler_Stop();
    }
    else if(pData_st)
    {
        if(pData_st->flags & MMC_DATA_WRITE)
        {
			mstar_mci_handle_cmdrdy(pMStarHost_st);
        }
        else if(pData_st->flags & MMC_DATA_READ)
        {
            #if defined(ENABLE_eMMC_INTERRUPT_MODE) && ENABLE_eMMC_INTERRUPT_MODE
            if((pCmd_st->opcode == 8)||(pCmd_st->opcode == 17)||(pCmd_st->opcode == 18))
                REG_FCIE_SETBIT(FCIE_MIE_INT_EN, BIT_MIU_LAST_DONE);
            #endif

			if(eMMC_FCIE_WaitEvents(FCIE_MIE_EVENT, BIT_MIU_LAST_DONE,
				eMMC_GENERIC_WAIT_TIME) != eMMC_ST_SUCCESS || 
		        (REG_FCIE(FCIE_SD_STATUS)&BIT_SD_R_CRC_ERR))
            {
				eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,	"eMMC Err: r timeout, cmd.%u arg.%Xh, retry %u, ST: %Xh \n", 
			        pCmd_st->opcode, pCmd_st->arg, u8_retry_cmd, REG_FCIE(FCIE_SD_STATUS));

		        if(u8_retry_data < MCI_RETRY_CNT_CMD_TO)
		        {
			        u8_retry_data++;			
			        eMMC_FCIE_ErrHandler_ReInit();
			        eMMC_FCIE_ErrHandler_Retry();
			        goto LABEL_SEND_CMD;
		        }
		        eMMC_FCIE_ErrHandler_Stop();
            }
            else
                mstar_mci_post_dma_read(pMStarHost_st);
        }

    }

	mstar_mci_completed_command(pMStarHost_st);

}


static void mstar_mci_process_next(struct mstar_mci_host *pMStarHost_st)
{
	// [FIXME] ->
    if(!(pMStarHost_st->flags & FL_SENT_COMMAND))
    {
		//eMMC_debug(0,0,"1.CMD%d \n", pMStarHost_st->request->cmd->opcode);
        pMStarHost_st->flags |= FL_SENT_COMMAND;
        mstar_mci_send_command(pMStarHost_st, pMStarHost_st->request->cmd);
    }                                        // [WHY CMD17 CMD24 will request->stop be NULL ?
    else if((!(pMStarHost_st->flags & FL_SENT_STOP)) && pMStarHost_st->request->stop)
    {
		//eMMC_debug(0,0,"2.CMD%d \n", pMStarHost_st->request->stop->opcode);
        pMStarHost_st->flags |= FL_SENT_STOP;
        mstar_mci_send_command(pMStarHost_st, pMStarHost_st->request->stop);
    }
    else
    {   //eMMC_debug(0,0,"3.done \n");
        eMMC_UnlockFCIE((U8*)__FUNCTION__);
        mmc_request_done(pMStarHost_st->mmc, pMStarHost_st->request);
    }
	// <- [FIXME]
}


static void mstar_mci_request(struct mmc_host *pMMCHost_st, struct mmc_request *pMRQ_st)
{
    struct mstar_mci_host *pMStarHost_st;;

	eMMC_LockFCIE((U8*)__FUNCTION__);
    pMStarHost_st = mmc_priv(pMMCHost_st);
    if (!pMMCHost_st)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: pMMCHost_st is NULL \n");
        eMMC_UnlockFCIE((U8*)__FUNCTION__);
		return;
    }
    if (!pMRQ_st)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: pMRQ_st is NULL \n");
        eMMC_UnlockFCIE((U8*)__FUNCTION__);
		return;
    }

	pMStarHost_st->request = pMRQ_st;    
    
	// ---------------------------------------------
	#if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
	if(0 == (g_eMMCDrv.u32_DrvFlag & DRV_FLAG_INIT_DONE))
	{
		eMMC_Init_Device();
	}

	if(NULL == pMStarHost_st->request->cmd->data)
	{
		pMStarHost_st->cmd = pMStarHost_st->request->cmd;
		mstar_mci_completed_command_FromRAM(pMStarHost_st);
		return;
	}
	#endif
	// ---------------------------------------------

	pMStarHost_st->flags = 0;
    mstar_mci_process_next(pMStarHost_st);
}


#if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
static void mstar_mci_completed_command_FromRAM(struct mstar_mci_host *pMStarHost_st)
{
    struct mmc_command *pCmd_st = pMStarHost_st->cmd;
	u16 au16_cifc[32], u16_i;
	u8 *pTemp;

	#if 0
	eMMC_debug(0,1,"\n");
	eMMC_debug(0,1,"cmd:%u, arg:%Xh\n", pCmd_st->opcode, pCmd_st->arg);
	#endif

	if(eMMC_ST_SUCCESS != eMMC_ReturnRsp((u8*)au16_cifc, (u8)pCmd_st->opcode))
	{
		pCmd_st->error = -ETIMEDOUT;
		//eMMC_debug(0,0,"eMMC Info: no rsp\n");
	}
	else
	{
		pCmd_st->error = 0;
		pTemp = (u8*)&(pCmd_st->resp[0]);
        for(u16_i=0; u16_i < 15; u16_i++)
        {
            pTemp[(3-(u16_i%4)) + 4*(u16_i/4)] =
                (u8)(au16_cifc[(u16_i+1)/2] >> 8*((u16_i+1)%2));
        }
		#if 0
	    eMMC_debug(0,1,"------------------\n");
		eMMC_dump_mem((u8*)&(pCmd_st->resp[0]), 0x10);
		eMMC_debug(0,1,"------------------\n");	
		#endif
	}

	eMMC_UnlockFCIE((U8*)__FUNCTION__);
    mmc_request_done(pMStarHost_st->mmc, pMStarHost_st->request);
}
#endif

static void mstar_mci_set_ios(struct mmc_host *pMMCHost_st, struct mmc_ios *pIOS_st)
{
    /* Define Local Variables */
    struct mstar_mci_host *pMStarHost_st;
	static u8 u8_IfLock=0;

	if(0 == (REG_FCIE(FCIE_REG16h) & BIT_EMMC_ACTIVE))
	{
		eMMC_LockFCIE((U8*)__FUNCTION__);
		u8_IfLock = 1;
		//eMMC_debug(0,1,"lock\n");
	}	
	
	pMStarHost_st = mmc_priv(pMMCHost_st);
    if (!pMMCHost_st)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: pMMCHost_st is NULL \n");
        goto LABEL_END;
    }
    if (!pIOS_st)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: pIOS_st is NULL \n");
        goto LABEL_END;
    }

	//eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "eMMC: clock: %u, bus_width %Xh \n",
	//	pIOS_st->clock, pIOS_st->bus_width);

	// ----------------------------------
	if (pIOS_st->clock == 0)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "eMMC Warn: disable clk \n");
        eMMC_clock_gating();
    }
	// ----------------------------------
	#if defined(eMMC_RSP_FROM_RAM) && eMMC_RSP_FROM_RAM
    else
    {   eMMC_clock_setting(g_eMMCDrv.u16_ClkRegVal);		
	}	
	pMStarHost_st->sd_mod = (BIT_SD_DEFAULT_MODE_REG & ~BIT_SD_DATA_WIDTH_MASK) | BIT_SD_DATA_WIDTH_8;
	#else
	// ----------------------------------
    else
    {	
		pMStarHost_st->sd_mod = BIT_SD_DEFAULT_MODE_REG;

        if(pIOS_st->clock > CLK_400KHz)
		{
			eMMC_clock_setting(FCIE_DEFAULT_CLK);
		}
		else
		{	eMMC_clock_setting(FCIE_SLOWEST_CLK);
		}
    }
	
	if (pIOS_st->bus_width == MMC_BUS_WIDTH_8)
    {
        pMStarHost_st->sd_mod = (pMStarHost_st->sd_mod & ~BIT_SD_DATA_WIDTH_MASK) | BIT_SD_DATA_WIDTH_8;
    }
    else if (pIOS_st->bus_width == MMC_BUS_WIDTH_4)
    {
        pMStarHost_st->sd_mod = (pMStarHost_st->sd_mod & ~BIT_SD_DATA_WIDTH_MASK) | BIT_SD_DATA_WIDTH_4;
    }
    else
    {   pMStarHost_st->sd_mod = (pMStarHost_st->sd_mod & ~BIT_SD_DATA_WIDTH_MASK);
    }
	#endif

	LABEL_END:
	if(u8_IfLock)
	{
		u8_IfLock = 0;
		eMMC_UnlockFCIE((U8*)__FUNCTION__);
		//eMMC_debug(0,1,"unlock\n");
	}
}


static s32 mstar_mci_get_ro(struct mmc_host *pMMCHost_st)
{
    s32 read_only;

	read_only = 0;
    if(!pMMCHost_st)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: pMMCHost_st is NULL \n");
        read_only = -EINVAL;
    }

    return read_only;
}


//=======================================================================
static void mstar_mci_enable(struct mstar_mci_host *pMStarHost_st)
{
	u32 u32_err;

	memset((void*)&g_eMMCDrv, '\0', sizeof(eMMC_DRIVER));

	eMMC_PlatformInit();
	
	g_eMMCDrv.u8_BUS_WIDTH = BIT_SD_DATA_WIDTH_1;
	g_eMMCDrv.u16_Reg10_Mode = BIT_SD_DEFAULT_MODE_REG;
	
    u32_err = eMMC_FCIE_Init();
	if(eMMC_ST_SUCCESS != u32_err)
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
		    "eMMC Err: eMMC_FCIE_Init fail: %Xh \n", u32_err);
}

static void mstar_mci_disable(struct mstar_mci_host *pMStarHost_st)
{
	u32 u32_err;

	eMMC_debug(eMMC_DEBUG_LEVEL,1,"\n");

    u32_err = eMMC_FCIE_Reset();
	if(eMMC_ST_SUCCESS != u32_err)
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
		    "eMMC Err: eMMC_FCIE_Reset fail: %Xh\n", u32_err);
	eMMC_clock_gating();
}

static s32 mstar_mci_probe(struct platform_device *pDev_st)
{
    /* Define Local Variables */
    struct mmc_host *pMMCHost_st;
    struct mstar_mci_host *pMStarHost_st;
    s32 s32_ret;

	#if IF_FCIE_SHARE_IP && defined(CONFIG_MSTAR_SDMMC)
    eMMC_debug(0,0,"eMMC: has SD, 2013/02/06 \n");
	#else
	eMMC_debug(0,0,"eMMC: no SD, 2013/02/06 \n");
	#endif
	
	// --------------------------------
    if (!pDev_st)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: pDev_st is NULL \n");
        s32_ret = -EINVAL;
		goto LABEL_END;
    }

	pMMCHost_st = 0;
	pMStarHost_st = 0;
	s32_ret = 0;

	// --------------------------------
	eMMC_LockFCIE((U8*)__FUNCTION__);
	mstar_mci_enable(pMStarHost_st);
	eMMC_UnlockFCIE((U8*)__FUNCTION__);

    pMMCHost_st = mmc_alloc_host(sizeof(struct mstar_mci_host), &pDev_st->dev);
    if (!pMMCHost_st)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: mmc_alloc_host fail \n");
        s32_ret = -ENOMEM;
		goto LABEL_END;
    }

    pMMCHost_st->ops = &sg_mstar_mci_ops;
	// [FIXME]->
    pMMCHost_st->f_min = CLK_400KHz;
	pMMCHost_st->f_max = CLK_200MHz;
    pMMCHost_st->ocr_avail = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 | \
						 MMC_VDD_31_32 | MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;

	pMMCHost_st->max_blk_count  = BIT_SD_JOB_BLK_CNT_MASK;
    pMMCHost_st->max_blk_size   = 512; /* sector */
    pMMCHost_st->max_req_size   = pMMCHost_st->max_blk_count * pMMCHost_st->max_blk_size;

    pMMCHost_st->max_seg_size   = pMMCHost_st->max_req_size;
	#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,20)
    pMMCHost_st->max_phys_segs  = 128;
    pMMCHost_st->max_hw_segs    = 128;
	#else
	pMMCHost_st->max_segs       = 128;
    #endif
	//---------------------------------------
    pMStarHost_st           = mmc_priv(pMMCHost_st);
    pMStarHost_st->mmc      = pMMCHost_st;
	//---------------------------------------

    pMMCHost_st->caps = MMC_CAP_8_BIT_DATA | MMC_CAP_MMC_HIGHSPEED | MMC_CAP_NONREMOVABLE;

    pMStarHost_st->baseaddr = (void __iomem *)FCIE0_BASE;
	// <-[FIXME]

    mmc_add_host(pMMCHost_st);
    platform_set_drvdata(pDev_st, pMMCHost_st);

    #if defined(ENABLE_eMMC_INTERRUPT_MODE) && ENABLE_eMMC_INTERRUPT_MODE
    s32_ret = request_irq(E_IRQ_NFIE, eMMC_FCIE_IRQ, IRQF_SHARED, DRIVER_NAME, pMStarHost_st);
    if (s32_ret)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: request_irq fail \n");
        mmc_free_host(pMMCHost_st);
        goto LABEL_END;
    }
    #endif

	sgp_eMMCThread_st = kthread_create(mstar_mci_Housekeep, NULL, "eMMC_bg_thread");
	if(IS_ERR(sgp_eMMCThread_st))
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: create thread fail \n");
		return PTR_ERR(sgp_eMMCThread_st);
	}
	wake_up_process(sgp_eMMCThread_st);

	#if defined(CONFIG_MMC_MSTAR_MMC_EMMC_LIFETEST)
	g_eMMCDrv.u64_CNT_TotalRBlk = 0;
	g_eMMCDrv.u64_CNT_TotalWBlk = 0;
	writefile = create_proc_entry (procfs_name, 0644, NULL);
	if(writefile == NULL)
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC Err: Can not initialize /proc/%s\n", procfs_name);
	else
	{
        writefile->read_proc = procfile_read;
        writefile->mode      = S_IFREG | S_IRUGO;
        writefile->uid       = 0;
        writefile->gid       = 0;
        writefile->size      = 0x10;
	}
	  #endif    

	LABEL_END:
	return s32_ret;
}

static s32 __exit mstar_mci_remove(struct platform_device *pDev_st)
{
    /* Define Local Variables */
    struct mmc_host *pMMCHost_st;
    struct mstar_mci_host *pMStarHost_st;
	s32 s32_ret;

	eMMC_LockFCIE((U8*)__FUNCTION__);
	pMMCHost_st = platform_get_drvdata(pDev_st);
	pMStarHost_st = mmc_priv(pMMCHost_st);
	s32_ret = 0;

    if (!pDev_st)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: pDev_st is NULL\n");
        s32_ret = -EINVAL;
		goto LABEL_END;
    }
    if (!pMMCHost_st)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: pMMCHost_st is NULL\n");
        s32_ret= -1;
		goto LABEL_END;
    }
	eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC, remove +\n");

	mmc_remove_host(pMMCHost_st);

    mstar_mci_disable(pMStarHost_st);
    #if defined(ENABLE_eMMC_INTERRUPT_MODE) && ENABLE_eMMC_INTERRUPT_MODE
    free_irq(pMStarHost_st->irq, pMStarHost_st);
    #endif

    mmc_free_host(pMMCHost_st);
    platform_set_drvdata(pDev_st, NULL);
    eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC, remove -\n");

	LABEL_END:
	eMMC_UnlockFCIE((U8*)__FUNCTION__);

	if(sgp_eMMCThread_st)
	    kthread_stop(sgp_eMMCThread_st);
		
    return s32_ret;
}


#ifdef CONFIG_PM
static s32 mstar_mci_suspend(struct platform_device *pDev_st, pm_message_t state)
{
    /* Define Local Variables */
    struct mmc_host *pMMCHost_st;
    s32 ret;

	pMMCHost_st = platform_get_drvdata(pDev_st);
	ret = 0;

	// wait for D0 high before losing eMMC Vcc
	eMMC_LockFCIE((U8*)__FUNCTION__);
	if(eMMC_ST_SUCCESS != mstar_mci_WaitD0High(TIME_WAIT_DAT0_HIGH))
	{
	    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: wait D0 H TO\n");
	    eMMC_FCIE_ErrHandler_Stop();
	}
	eMMC_PlatformDeinit();
	eMMC_UnlockFCIE((U8*)__FUNCTION__);

    if (pMMCHost_st)
    {
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC, suspend + \n");
        ret = mmc_suspend_host(pMMCHost_st);
    }

	eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC, suspend -, %Xh\n", ret);
	return ret;
}

static s32 mstar_mci_resume(struct platform_device *pDev_st)
{
    struct mmc_host *pMMCHost_st;
    s32 ret;
    static u8 u8_IfLock=0;

	pMMCHost_st = platform_get_drvdata(pDev_st);
	ret = 0;

	if(0 == (REG_FCIE(FCIE_REG16h) & BIT_EMMC_ACTIVE))
	{
		eMMC_LockFCIE((U8*)__FUNCTION__);
		u8_IfLock = 1;
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"lock\n");
	}
	eMMC_PlatformInit();
	mstar_mci_enable(mmc_priv(pMMCHost_st));
	if(u8_IfLock)
	{
		u8_IfLock = 0;
		eMMC_UnlockFCIE((U8*)__FUNCTION__);
		eMMC_debug(eMMC_DEBUG_LEVEL,1,"unlock\n");
	}

    if (pMMCHost_st)
    {
		//eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC, resume +\n");
        ret = mmc_resume_host(pMMCHost_st);
    }

    //eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC, resume -, %Xh\n", ret);
	return ret;
}
#endif  /* End ifdef CONFIG_PM */

/******************************************************************************
 * Define Static Global Variables
 ******************************************************************************/
static struct platform_driver sg_mstar_mci_driver =
{
	.probe = mstar_mci_probe,
    .remove = __exit_p(mstar_mci_remove),

    #ifdef CONFIG_PM
    .suspend = mstar_mci_suspend,
    .resume = mstar_mci_resume,
    #endif

    .driver  =
    {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    },
};

static struct platform_device sg_mstar_emmc_device_st =
{
    .name =	DRIVER_NAME,
    .id = 0,
    .resource =	NULL,
    .num_resources = 0,
};

/******************************************************************************
 * Init & Exit Modules
 ******************************************************************************/
static s32 __init mstar_mci_init(void)
{
	int err = 0;

    eMMC_debug(eMMC_DEBUG_LEVEL_LOW,1,"\n");

	if((err = platform_device_register(&sg_mstar_emmc_device_st)) < 0)
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: platform_driver_register fail, %Xh\n", err);

    if((err = platform_driver_register(&sg_mstar_mci_driver)) < 0)
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: platform_driver_register fail, %Xh\n", err);

	return err;
}

static void __exit mstar_mci_exit(void)
{
    platform_driver_unregister(&sg_mstar_mci_driver);
}

static int __init write_seg_size_setup(char *str)
{
    wr_seg_size =  simple_strtoul(str, NULL, 16);
    return 1;
}
__setup("mmc_wrsize=", write_seg_size_setup);

subsys_initcall(mstar_mci_init);
//module_init(mstar_mci_init);
module_exit(mstar_mci_exit);

MODULE_LICENSE("non-GPL");
MODULE_DESCRIPTION("Mstar Multimedia Card Interface driver");
MODULE_AUTHOR("Benson.Hsiao");

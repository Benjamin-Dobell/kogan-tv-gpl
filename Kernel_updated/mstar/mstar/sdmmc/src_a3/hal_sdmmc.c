/***************************************************************************************************************
 *
 * FileName hal_sdmmc.c
 *     @author jeremy.wang (2011/07/21)
 * Desc:
 * 	   HAL SD Driver will support basic SD function but not flow process.
 *         The goal is that we don't need to change HAL Level code (But its h file code)
 *
 *	   The limitation were listed as below:
 * 	   (1) This c file belongs to HAL level.
 *	   (2) Its h file is included by driver API level, not driver flow process.
 * 	   (3) FCIE/SDIO IP register and buffer opertion function belong to here.
 * 	   (4) Only IP flow concept, but not SD command concept.
 * 	   (5) This c file could not use project/os/cpu/icver/specific define option here, but its h file could.
 *
 * 	   P.S. EN_XX for ON/OFF Define, , V_XX  for Value Define,
 * 			RT_XX for Retry Times Define, WT_XX for Wait Time Define, M_XX for Mask Value Define
 *
 ***************************************************************************************************************/

#include "hal_sdmmc.h"
#include "hal_card_timer.h"
#include "hal_card_intr.h"  //inlcue but may not use
#if (D_PROJECT == D_PROJECT__A3)
#include <mstar/mstar_chip.h>
#endif

#define A_CHIPTOP_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x0F00)

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************

// Enable Setting
//-----------------------------------------------------------------------------------------------------------
#define EN_TRFUNC         (FALSE)
#define EN_DUMPREG		  (TRUE)
#define EN_BYTEMODE		  (TRUE)
#define EN_DYNTIMEOUT     (TRUE)

// Retry Times
//-----------------------------------------------------------------------------------------------------------
#define RT_CLEAN_SDSTS				100
#define RT_CLEAN_MIEEVENT			100

// Wait Time
//-----------------------------------------------------------------------------------------------------------
#define WT_EVENT_RSP			    1000	//(ms)
#define WT_EVENT_READ			    2000	//(ms)
#define WT_EVENT_WRITE			    3000	//(ms)
#define WT_DAT0HI_END				3000	//(ms)
#define WT_FIFO_CLK_RDY				100		//(ms)
#define WT_RATIO_READ               3       //(ms)
#define WT_RATIO_WRITE              5       //(ms)

//***********************************************************************************************************

// Reg Static Init Setting
//-----------------------------------------------------------------------------------------------------------
#define V_MIE_PATH_INIT     R_SD_EN
#define V_MMA_PRI_INIT      (R_MMA_R_PRIORITY|R_MMA_W_PRIORITY|V_MIUCLK_CTRL_INIT|V_MIUBUS_CTRL_INIT)
#define V_MIE_INT_EN_INIT   R_SD_CMD_END_EN
#define V_RSP_SIZE_INIT     0x0
#define V_CMD_SIZE_INIT     0x05
#define V_SD_CTL_INIT       0x0
#define V_SD_MODE_INIT      (R_SD_CLK_EN|R_SD_CS_EN)

// Mask Range
//-----------------------------------------------------------------------------------------------------------
#define M_SD_ERRSTS			(R_SD_DAT_CERR|R_SD_DAT_STSERR|R_SD_DAT_STSNEG|R_SD_CMD_NORSP|R_SD_CMDRSP_CERR) //0x1F
#define M_SD_MIEEVENT		(R_MMA_DATA_END|R_SD_CMD_END|R_SD_DATA_END|R_SDIO_INT|R_CARD_DMA_END|R_MMA_LSDONE_END)


// Mask Reg Value
//-----------------------------------------------------------------------------------------------------------
#define M_REG_STSERR(IP)            (CARD_REG(A_SD_STS_REG(IP)) & M_SD_ERRSTS)  //0x1F
#define M_REG_SDMIEEvent(IP)        (CARD_REG(A_MIE_EVENT_REG(IP)) & M_SD_MIEEVENT)
#define M_REG_FIFORDYCHK(IP)		(CARD_REG(A_MMA_PRI_REG_REG(IP)) & R_JOB_RW_DIR & R_FIFO_CLK_RDY)

//IP_FCIE or IP_SDIO Register Basic Address
//-----------------------------------------------------------------------------------------------------------

#define A_SD_REG_BANK(IP)           GET_CARD_BANK(IP, EV_REG_BANK)
#define A_SD_CIFC_BANK(IP)          GET_CARD_BANK(IP, EV_CIFC_BANK)
#define A_SD_CIFD_BANK(IP)          GET_CARD_BANK(IP, EV_CIFD_BANK)

#define A_MIE_EVENT_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x00)
#define A_MMA_PRI_REG_REG(IP)       GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x02)
#define A_MIU_DMA1_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x03)
#define A_MIU_DMA0_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x04)
#define A_CARD_DET_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x07)
#define A_CARD_PWR_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x08)
#define A_INTR_TEST_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x09)
#define A_MIE_PATH_CTL_REG(IP)      GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0A)
#define A_JOB_BL_CNT_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0B)
#define A_TR_BK_CNT_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0C)
#define A_RSP_SIZE_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0D)
#define A_CMD_SIZE_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0E)
#define A_CARD_WD_CNT_REG(IP)       GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0F)
#define A_SD_MODE_REG(IP)           GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x10)
#define A_SD_CTL_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x11)
#define A_SD_STS_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x12)
#define A_SDIO_CTL_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x1B)
#define A_SDIO_ADDR0_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x1C)
#define A_SDIO_ADDR1_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x1D)
#define A_SDIO_STS_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x1E)
#define A_NC_REORDER_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2D)
#define A_DAM_OFFSET_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2E)
#define A_TEST_MODE_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x30)

#define A_DBGL_REG(IP)              GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x31)
#define A_DBGH_REG(IP)              GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x32)

// Reg Dynamic Variable
//-----------------------------------------------------------------------------------------------------------
static RspStruct gst_RspStruct[2];
static volatile BOOL_T  gb_StopWProc[2] = {0};
static volatile U16_T   gu16_SD_MODE_DatLine[2] = {0};
static volatile U16_T	gu16_SD_Mode_DatSync[2] = {0};
static volatile U16_T   gu16_WT_NAC[2] = {0};             //Waiting Time for Nac (us)
static volatile U16_T   gu16_WT_READ[2] = {0};            //Waiting Time for Reading (ms)
static volatile U16_T   gu16_WT_WRITE[2] = {0};           //Waiting Time for Writing (ms)

// Trace Funcion
//-----------------------------------------------------------------------------------------------------------
#if (EN_TRFUNC)
	#define TR_SDMMC(p)	    p
#else
	#define TR_SDMMC(p)
#endif

// Register Operation Define ==> For Get Current Port Setting
//-----------------------------------------------------------------------------------------------------------
static U16_T _REG_GetPortSetting(IPEmType eIP)
{
	PortEmType ePort = GET_CARD_PORT(eIP);

	if( ePort == EV_PORT_SDIO1)
		return R_SDIO_SD_BUS_SW;
	else if ( ePort == EV_PORT_SDIO2 )
		return (R_SDIO_SD_BUS_SW|R_SDIO_PORT_SEL);
	else
		return 0;
}

// Register Operation Define ==> For Get Current DAT0 Value
//-----------------------------------------------------------------------------------------------------------
static U16_T _REG_GetDAT0(IPEmType eIP)
{
	PortEmType ePort = GET_CARD_PORT(eIP);

	if( ePort == EV_PORT_SDIO1)
		return (CARD_REG(A_SDIO_STS_REG(eIP)) & R_SDIO_DAT0);
	else if ( ePort == EV_PORT_SDIO2 )
		return (CARD_REG(A_SDIO_STS_REG(eIP)) & R_SDIO2_DAT0);
	else
		return (CARD_REG(A_SD_STS_REG(eIP)) & R_SD_DAT0);
}

// Register Operation Define ==> For Get Current CDET Value
//-----------------------------------------------------------------------------------------------------------
static U16_T _REG_GetCDET(IPEmType eIP)
{
	PortEmType ePort = GET_CARD_PORT(eIP);

	if( ePort == EV_PORT_SDIO1)
		return (CARD_REG(A_CARD_DET_REG(eIP)) & R_SDIO_DET_N);
	else if ( ePort == EV_PORT_SDIO2 )
		return (CARD_REG(A_CARD_DET_REG(eIP)) & R_SDIO2_DET_N);
	else
		return (CARD_REG(A_CARD_DET_REG(eIP)) & R_SD_DET_N);
}

// Register Operation Define ==> For Clean Reg and Special Case
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_ClearSDSTS(IPEmType eIP, U8_T u8Retry)
{
	do
	{
		CARD_REG16_ON(A_SD_STS_REG(eIP), M_SD_ERRSTS);

		if ( !M_REG_STSERR(eIP) )
			return EV_OK;
		else if(!u8Retry)
			return EV_FAIL;

	} while(u8Retry--);
	return EV_FAIL;
}


// Register Operation Define ==> For Clear MIE Event
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_ClearMIEEvent(IPEmType eIP, U8_T u8Retry)
{
	/****** Clean global MIEEvent for Interrupt ******/
#if(EN_BIND_CARD_INT)
	Hal_CARD_INT_ClearMIEEvent(eIP);
#endif

	/****** Clean MIEEvent Reg *******/
	do
	{
		CARD_REG(A_MIE_EVENT_REG(eIP)) = M_SD_MIEEVENT;

		if ( !M_REG_SDMIEEvent(eIP) )
			return EV_OK;
		else if(!u8Retry)
			return EV_FAIL;

	}while(u8Retry--);
	return EV_FAIL;
}

// Register Operation Define ==> For Wait DAT0 High
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_WaitDat0HI(IPEmType eIP, U32_T u32WaitMs)
{
    U32_T u32DiffTime = 0;
	do
	{
		if ( _REG_GetDAT0(eIP) )
			return EV_OK;

		if ( gb_StopWProc[eIP] )
			return EV_FAIL;

		Hal_Timer_uDelay(1);
		u32DiffTime++;
	}while(u32DiffTime <= (u32WaitMs*1000));
	return EV_FAIL;
}

RetEmType Hal_SDMMC_WaitDat0HI(IPEmType eIP)
{
    return (_REG_WaitDat0HI(eIP, WT_DAT0HI_END));
}

// Register Operation Define ==> For Wait FIFO Clk Ready
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_WaitFIFOClkRdy(IPEmType eIP, CmdEmType eCmdType, U32_T u32WaitMs)
{
    U32_T u32DiffTime = 0;

	do
	{
		if( M_REG_FIFORDYCHK(eIP) ==  ((eCmdType>>6) & R_FIFO_CLK_RDY) )
			return EV_OK;

		if ( gb_StopWProc[eIP] )
			return EV_FAIL;

		Hal_Timer_uDelay(1);
		u32DiffTime++;
	}while(u32DiffTime <= (u32WaitMs*1000));
	return EV_FAIL;
}

// Register Operation Define ==> For Wait MIE Event
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_WaitMIEEvent(IPEmType eIP, U16_T u16ReqEvent, U32_T u32WaitMs)
{
	U32_T u32DiffTime = 0;

#if(EN_BIND_CARD_INT)
	if ( Hal_CARD_INT_ModeRunning(eIP, EV_INT_SD) )	// Interrupt Mode
	{
		if ( !Hal_CARD_INT_WaitMIEEvent(eIP, u16ReqEvent, u32WaitMs) )
			return EV_FAIL;
		else
			return EV_OK;
	} else // Polling Mode
#endif
	{
		do
		{
			if ( (CARD_REG(A_MIE_EVENT_REG(eIP))&u16ReqEvent) == u16ReqEvent )
				return EV_OK;

			if ( gb_StopWProc[eIP] )
				return EV_FAIL;

			Hal_Timer_uDelay(1);
			u32DiffTime++;
		}while(u32DiffTime <= (u32WaitMs*1000));
		return EV_FAIL;
	}

}

// Register Operation Define ==> For Software Reset
//-----------------------------------------------------------------------------------------------------------
static void _REG_ResetIP(IPEmType eIP)
{	
	CARD_REG_W16(A_SD_CTL_REG(eIP), 0); 		
	CARD_REG16_OFF(A_MIE_PATH_CTL_REG(eIP), R_MMA_ENABLE);	
	CARD_REG16_ON(A_MMA_PRI_REG_REG(eIP), R_MIU_REQ_RST); 
	Hal_Timer_mDelay(1); 									
    CARD_REG16_OFF(A_TEST_MODE_REG(eIP), R_FCIE_SOFT_RST);	
    Hal_Timer_mDelay(1);	
    CARD_REG16_ON(A_TEST_MODE_REG(eIP), R_FCIE_SOFT_RST);	
    Hal_Timer_mDelay(1); 	
    CARD_REG16_OFF(A_MMA_PRI_REG_REG(eIP), R_MIU_REQ_RST);		   	
    _REG_ClearMIEEvent(eIP, 10); 		   // clear MIE event	
    _REG_ClearSDSTS(eIP, 10);			   // clear SD_STS	
}

// Register Operation Define ==> Fix CIFD (SRAM) First Time R/W Bug
//-----------------------------------------------------------------------------------------------------------
static void _REG_CIFDEmptyRW(IPEmType eIP)
{
	volatile  U32_T *_pu32_CIFDBuf = (volatile U32_T *)(CARD_BANK(A_SD_CIFD_BANK(eIP)));
	U32_T u32FixSRAMBug = 0x00;

	_pu32_CIFDBuf[0] = u32FixSRAMBug;
	_pu32_CIFDBuf[1] = _pu32_CIFDBuf[0];
	u32FixSRAMBug    = _pu32_CIFDBuf[1];
}


// CIFC FIFO Buffer Operation Define
//-----------------------------------------------------------------------------------------------------------
static U8_T _BUF_GetByteFromCIFBuf(volatile U32_T *pu32Buf, U16_T u16Addr)
{
    U32_T u32Tmp = pu32Buf[u16Addr>>1];

    if(u16Addr & 0x1)
        return (U8_T)(u32Tmp>>8);
    else
        return (U8_T)u32Tmp;
}

// SDMMC Internel Logic Function
//-----------------------------------------------------------------------------------------------------------
/*static*/ void _SDMMC_REG_Dump(IPEmType eIP)
{
    int i;

#if (EN_DUMPREG)

	volatile U32_T *_pu32_CIFCBuf = (volatile U32_T *)(CARD_BANK(A_SD_CIFC_BANK(eIP)));
	U8_T u16Pos;

	prtstring("\n------------------------------------------\r\n");
	prtstring("  CMD_");
	prtUInt(gst_RspStruct[eIP].u8Cmd);
	prtstring(" (Arg: ");
	prtU32Hex(gst_RspStruct[eIP].u32Arg);
	prtstring(") [Line: ");
	prtUInt(gst_RspStruct[eIP].u32ErrLine);
	prtstring("]\r\n");

	prtstring("------------------------------------------\r\n");

	for(u16Pos = 0; u16Pos < gst_RspStruct[eIP].u8RspSize; u16Pos++)
	{
		if( (u16Pos == 0) || (u16Pos == 8) )
			prtstring("[");

		prtU8Hex(_BUF_GetByteFromCIFBuf(_pu32_CIFCBuf, u16Pos));
		prtstring(",");

		if( (u16Pos == 7) || (u16Pos == (gst_RspStruct[eIP].u8RspSize-1)) )
			prtstring("]\n");
	}


	prtstring("---------------DumpReg--------------------\r\n");
    prtstring("---------------ChipTop--------------------\r\n");
    prtstring("[0x6F][NAND_CFG_REG]=        ");
    prtU32Hex(CARD_REG(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x6F)));
	prtstring("\r\n");
    prtstring("[0x6E][EMMC_CFG_REG]=        ");
    prtU32Hex(CARD_REG(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x6E)));
	prtstring("\r\n");
    prtstring("[0x5A][SD_CFG_REG]=          ");
    prtU32Hex(CARD_REG(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x5A)));
	prtstring("\r\n");
    prtstring("[0x1C][SD2_CFG_REG]=         ");
    prtU32Hex(CARD_REG(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x1C)));
	prtstring("\r\n");
    prtstring("------------------------------------------\r\n");

	prtstring("[0x12][SD_STS_REG]=          ");
	prtU32Hex(CARD_REG(A_SD_STS_REG(eIP)));
	prtstring("\r\n");

	if(GET_CARD_PORT(eIP) != EV_PORT_SD)
	{
		prtstring("[0x1E][SDIO_STS_REG]=        ");
		prtU32Hex(CARD_REG(A_SDIO_STS_REG(eIP)));
		prtstring("\r\n");
	}

#if(EN_BIND_CARD_INT)
	if ( Hal_CARD_INT_ModeRunning(eIP, EV_INT_SD) )	// Interrupt Mode
	{
		prtstring("[gu16_MIEEvent_ForInt]=      ");
		prtU32Hex(Hal_CARD_INT_GetMIEEvent(eIP));
		prtstring("\r\n");
	}
#else
	prtstring("[0x01][MIE_EVENT_REG]=       ");
	prtU32Hex(CARD_REG(A_MIE_EVENT_REG(eIP)));
	prtstring("\r\n");
#endif

	prtstring("[0x10][SD_MODE_REG]=         ");
	prtU32Hex(CARD_REG(A_SD_MODE_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x11][SD_CTL_REG]=          ");
	prtU32Hex(CARD_REG(A_SD_CTL_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x0A][MIE_PATH_CTL_REG]=    ");
	prtU32Hex(CARD_REG(A_MIE_PATH_CTL_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x02][MMA_PRI_REG_REG]=     ");
	prtU32Hex(CARD_REG(A_MMA_PRI_REG_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x0B][JOB_BL_CNT_REG]=      ");
	prtU32Hex(CARD_REG(A_JOB_BL_CNT_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x0C][TR_BK_CNT_REG]=       ");
	prtU32Hex(CARD_REG(A_TR_BK_CNT_REG(eIP)));
	prtstring("\r\n");

#if (EN_BYTEMODE)

	prtstring("[0x1C][SDIO_ADDR0_REG]=      ");
	prtU32Hex(CARD_REG(A_SDIO_ADDR0_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x1D][SDIO_ADDR1_REG]=      ");
	prtU32Hex(CARD_REG(A_SDIO_ADDR1_REG(eIP)));
	prtstring("\r\n");
#else
	prtstring("[0x04][MIU_DMA0_REG]=        ");
	prtU32Hex(CARD_REG(A_MIU_DMA0_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x03][MIU_DMA1_REG]=        ");
	prtU32Hex(CARD_REG(A_MIU_DMA1_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x2E][DAM_OFFSET_REG]=	    ");
	prtU32Hex(CARD_REG(A_DAM_OFFSET_REG(eIP)));
	prtstring("\r\n");
#endif

	prtstring("[0x1B][SDIO_CTL_REG]=        ");
	prtU32Hex(CARD_REG(A_SDIO_CTL_REG(eIP)));
	prtstring("\r\n");

	prtstring("[0x0F][CARD_WD_CNT_REG]=     ");
	prtU32Hex(CARD_REG(A_CARD_WD_CNT_REG(eIP)));
	prtstring("\r\n");

	prtstring("------------------------------------------\r\n");

    printk("\nFCIE:\n");
	for(i=0; i<0x80; i++)
    {
        if(i%8==0) printk("%02X: ", (i/8)*8);

        printk("0x%04X ", CARD_REG(GET_CARD_REG_ADDR(A_SD_REG_BANK(eIP), i)) );
        if(i%8==7) printk(":%02X\r\n", ((i/8)*8)+7);
    }
    prtstring("------------------------------------------\r\n");

    printk("\nDump FCIE3 debug ports:\n");
	for(i=0; i<=7; i++)
	{
		// Enable debug ports
        CARD_REG16_OFF(A_TEST_MODE_REG(eIP), (BIT10|BIT09|BIT08));
        CARD_REG16_ON(A_TEST_MODE_REG(eIP), i<<8);

		// Dump debug ports
		printk("\nDebug Mode \033[31m%d:\033[m\n", i);
		printk("DBus[15:0]=\033[34m%04X\033[m\n", CARD_REG(A_DBGL_REG(eIP)));
		printk("DBus[23:16]=\033[34m%04X\033[m\n", CARD_REG(A_DBGH_REG(eIP)));
	}

#endif  //End #if(EN_DUMPREG)

}

static RspErrEmType _SDMMC_ErrProcess(IPEmType eIP, CmdEmType eCmdType, RspErrEmType eRspErr, BOOL_T bCloseClk, int Line)
{
	U16_T u16RspErr = (U16_T)eRspErr;
	U16_T u16IPErr = EV_STS_RIU_ERR | EV_STS_MIE_TOUT | EV_STS_FIFO_NRDY | EV_STS_DAT0_BUSY;

	/****** (1) Record Information *******/
	gst_RspStruct[eIP].u32ErrLine = (U32_T)Line;
	gst_RspStruct[eIP].u8RspSize = (U8_T)CARD_REG(A_RSP_SIZE_REG(eIP));
	gst_RspStruct[eIP].eErrCode = eRspErr;

	/****** (2) Dump and the Reg Info + Reset IP *******/

	if ( u16RspErr && gb_StopWProc[eIP] )
	{
		eRspErr = EV_SWPROC_ERR;
		_REG_ResetIP(eIP);
	}
	else if( u16RspErr & u16IPErr ) //SD IP Error
	{
		_SDMMC_REG_Dump(eIP);
        //while(1);
		_REG_ResetIP(eIP);
	}
	else if( u16RspErr & M_SD_ERRSTS ) //SD_STS Reg Error
	{
		//Do Nothing
	}

	/****** (3) Close clock and DMA Stop function ******/
	if(bCloseClk)
		CARD_REG16_OFF(A_SD_MODE_REG(eIP), R_SD_CLK_EN|R_SD_DMA_RD_CLK_STOP);

	/****** (4) Clock MIU Clock (SW) when SW open or not *******/
	CARD_REG16_OFF(A_MMA_PRI_REG_REG(eIP), R_MIU_CLK_EN_SW);

	return eRspErr;
}

//***********************************************************************************************************
// SDMMC HAL Function
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetCmdTokenToCIFC
 *     @author jeremy.wang (2011/11/29)
 * Desc: Set Cmd Token to CIFD
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u8Cmd : SD Command
 * @param u32Arg : SD Argument
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetCmdTokenToCIFC(IPEmType eIP, U8_T u8Cmd, U32_T u32Arg)
{
	volatile U32_T *_pu32_CIFCBuf = (volatile U32_T *)(CARD_BANK(A_SD_CIFC_BANK(eIP)));

	gst_RspStruct[eIP].u8Cmd  = u8Cmd;
	gst_RspStruct[eIP].u32Arg = u32Arg;

	TR_SDMMC(prtstring("@[CMD_"));
	TR_SDMMC(prtUInt(u8Cmd));
	TR_SDMMC(prtstring("]"));

	TR_SDMMC(prtstring(" (Arg: "));
	TR_SDMMC(prtU32Hex(u32Arg));
	TR_SDMMC(prtstring(")...\r\n"));

	_pu32_CIFCBuf[0] =  (((U8_T)(u32Arg>>24))<<8)+(0x40 + u8Cmd);
	_pu32_CIFCBuf[1] =  (((U8_T)(u32Arg>>8))<<8) +((U8_T)(u32Arg>>16));
	_pu32_CIFCBuf[2] =  (U8_T)u32Arg;

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_GetRspTokenFromCIFC
 *     @author jeremy.wang (2011/11/30)
 * Desc: Get Command Response Info.
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eError :  Response Error Code
 *
 * @return RspStruct*  : Response Struct
 ----------------------------------------------------------------------------------------------------------*/
RspStruct* Hal_SDMMC_GetRspTokenFromCIFC(IPEmType eIP, RspErrEmType eError)
{
	volatile U32_T *_pu32_CIFCBuf = (volatile U32_T *)(CARD_BANK(A_SD_CIFC_BANK(eIP)));
	U8_T u8Pos;

	TR_SDMMC(prtstring("==>(Rsp) Error: "));
	TR_SDMMC(prtU32Hex((U32_T)eError));
	TR_SDMMC(prtstring("\r\n"));

	for(u8Pos=0; u8Pos<0x10; u8Pos++ )
		gst_RspStruct[eIP].u8ArrRspToken[u8Pos] = 0;

	TR_SDMMC(prtstring("["));

	for(u8Pos=0; u8Pos < gst_RspStruct[eIP].u8RspSize; u8Pos++)
	{
		gst_RspStruct[eIP].u8ArrRspToken[u8Pos] = _BUF_GetByteFromCIFBuf(_pu32_CIFCBuf, u8Pos);

		TR_SDMMC(prtU8Hex(gst_RspStruct[eIP].u8ArrRspToken[u8Pos]));
		TR_SDMMC(prtstring(", "));

	}
	TR_SDMMC(prtstring("]"));

	TR_SDMMC(prtstring("\r\n\r\n"));

	return &gst_RspStruct[eIP];
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_CIFD_DATA_IO
 *     @author jeremy.wang (2011/12/1)
 * Desc:
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eCmdType : Reading/Writing
 * @param pu16Buf : Memory Buffer
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_CIFD_DATA_IO(IPEmType eIP, CmdEmType eCmdType,  U16_T u16Cnt, volatile U16_T *pu16Buf)
{
	volatile U32_T *_pu32_CIFDBuf = (volatile U32_T *)(CARD_BANK(A_SD_CIFD_BANK(eIP)));
	U16_T u16Pos = 0;

	for(u16Pos=0; u16Pos<(u16Cnt/2); u16Pos++)
	{
		if(eCmdType ==EV_CMDREAD)
			pu16Buf[u16Pos] = (U16_T)_pu32_CIFDBuf[u16Pos];
		else
			_pu32_CIFDBuf[u16Pos] = pu16Buf[u16Pos];
	}

#if 0 //For Debug CIFD Data
	prtstring("\n$[Prt DATA_IO]\n");
	for(u16Pos=0; u16Pos<5; u16Pos++)
	{
		prtstring("[");
		prtU16Hex(pu16Buf[u16Pos]);
		prtstring("]");
	}
	prtstring("\n\n");
#endif
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetDataWidth
 *     @author jeremy.wang (2011/11/28)
 * Desc: According as Data Bus Width to Set IP DataWidth
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eBusWidth : 1BIT/4BITs/8BITs
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetDataWidth(IPEmType eIP, SDMMCBusWidthEmType eBusWidth)
{
	gu16_SD_MODE_DatLine[eIP] = (U16_T)eBusWidth;

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetDataSync
 *     @author jeremy.wang (2011/11/28)
 * Desc: According as High or Default Speed to Set IP DataSync
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : TRUE: Sync, FALSE: NoSync
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetDataSync(IPEmType eIP, BOOL_T bEnable)
{
	if( bEnable )
		gu16_SD_Mode_DatSync[eIP] = R_SD_DATSYNC;
	else
		gu16_SD_Mode_DatSync[eIP] = 0;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetNcrDelay
 *     @author jeremy.wang (2011/11/29)
 * Desc: According as Current Clock to Set Nrc Delay
 *
 * @param eIP :  FCIE1/FCIE2/...
 * @param u32RealClk : Real Clock
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetNcrDelay(IPEmType eIP, U32_T u32RealClk)
{

	if( u32RealClk >= 8000000 )			//>=8MHz
		gu16_WT_NAC[eIP] = 1;
	else if( u32RealClk >= 4000000 )	//>=4MHz
		gu16_WT_NAC[eIP] = 2;
	else if( u32RealClk >= 2000000 )	//>=2MHz
		gu16_WT_NAC[eIP] = 4;
	else if( u32RealClk >= 1000000 )	//>=1MHz
		gu16_WT_NAC[eIP] = 8;
	else if( u32RealClk >= 400000 )     //>=400KHz
		gu16_WT_NAC[eIP] = 20;
	else if( u32RealClk >= 300000 )	    //>=300KHz
		gu16_WT_NAC[eIP] = 27;
	else if( u32RealClk >= 100000 )     //>=100KHz
		gu16_WT_NAC[eIP] = 81;
	else if(u32RealClk==0)
		gu16_WT_NAC[eIP] = 0;            //NoNeed
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_TransCmdSetting
 *     @author jeremy.wang (2011/12/1)
 * Desc: For Data Transfer Setting
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eTransType : CIFD/DMA/NONE
 * @param eCmdType : CMDRSP/READ/WRITIE
 * @param u16BlkCnt : Block Cnt
 * @param u16BlkSize : Block Size
 * @param pu16Buf : Memory Buffer
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_TransCmdSetting(IPEmType eIP, TransEmType eTransType, CmdEmType eCmdType, U16_T u16BlkCnt, U16_T u16BlkSize, volatile U16_T *pu16Buf)
{

#if(!EN_BYTEMODE)
	#if (D_MIU_WIDTH==4)
	U8_T u8BitMask[8]={0x0F,0x0E,0x0C,0x08};
	#else
	U8_T u8BitMask[8]={0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
	#endif
#endif

    U32_T dmaaddr = (U32_T)pu16Buf;

	if(eTransType == EV_DMA)
	{
		CARD_REG(A_MMA_PRI_REG_REG(eIP)) = V_MMA_PRI_INIT | (eCmdType>>6);

		/***** Block Count ******/
		CARD_REG(A_JOB_BL_CNT_REG(eIP)) =  u16BlkCnt;

		/****** Block Size and DMA Addr******/
#if (D_PROJECT == D_PROJECT__A3)
        if( dmaaddr >= MSTAR_MIU1_BUS_BASE)
        {
            dmaaddr -= MSTAR_MIU1_BUS_BASE;
            CARD_REG16_ON(A_MIU_DMA1_REG(eIP), BIT15);
        }
        else
        {
            dmaaddr -= MSTAR_MIU0_BUS_BASE;
            CARD_REG16_OFF(A_MIU_DMA1_REG(eIP), BIT15);
        }
#endif

#if(EN_BYTEMODE)
		CARD_REG(A_SDIO_ADDR1_REG(eIP)) = (U16_T)((dmaaddr)>>16);
		CARD_REG(A_SDIO_ADDR0_REG(eIP)) = (U16_T)(dmaaddr);
		CARD_REG(A_SDIO_CTL_REG(eIP)) = u16BlkSize | R_SDIO_BLK_MOD;
#else
		CARD_REG(A_MIU_DMA1_REG(eIP)) = (U16_T)(((dmaaddr&0xFFFFFFF)/D_MIU_WIDTH)>>16);
		CARD_REG(A_MIU_DMA0_REG(eIP)) = (U16_T)((dmaaddr)/D_MIU_WIDTH);
		CARD_REG(A_DAM_OFFSET_REG9(eIP)) = u8BitMask[dmaaddr&(D_MIU_WIDTH-1)];
		CARD_REG(A_SDIO_CTL_REG(eIP)) = 0x00;
#endif

		CARD_REG(A_CARD_WD_CNT_REG(eIP)) = 0x00;

	}
	else //CIFD or EMPTY
	{
		if( (u16BlkSize>=512) || (u16BlkSize==0))
			CARD_REG(A_CARD_WD_CNT_REG(eIP)) = 0;
		else
		{
			CARD_REG(A_CARD_WD_CNT_REG(eIP)) = (u16BlkSize/2);	//Word Count (Max: 256Words==>512Bytes)
		}

		CARD_REG(A_SDIO_CTL_REG(eIP)) = 0x00;
	}


	/****** Set Waiting Time for Data Transfer *******/

#if (EN_DYNTIMEOUT)

	if(gu16_SD_MODE_DatLine[eIP] == EV_BUS_1BIT)
		u16BlkCnt = u16BlkCnt * 4;

	if(eCmdType == EV_CMDREAD)
	{
		gu16_WT_READ[eIP] = WT_RATIO_READ * u16BlkCnt;
		if( gu16_WT_READ[eIP] < 2000) //2000ms
			gu16_WT_READ[eIP] = 2000;
	}
	else if(eCmdType == EV_CMDWRITE)
	{
		gu16_WT_WRITE[eIP] = WT_RATIO_WRITE * u16BlkCnt;
		if( gu16_WT_WRITE[eIP] < 3000) //3000ms
			gu16_WT_WRITE[eIP] = 3000;
	}
#else

	if(eCmdType == EV_CMDREAD)
		gu16_WT_READ[eIP] = WT_EVENT_READ;
	else if(eCmdType == EV_CMDWRITE)
		gu16_WT_WRITE[eIP] = WT_EVENT_WRITE;
#endif

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SendCmdAndWaitProcess
 *     @author jeremy.wang (2011/11/28)
 * Desc: Send CMD and Wait Process
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eTransType : CIFD/DMA/NONE
 * @param eCmdType : CMDRSP/READ/WRITE
 * @param eRspType : R1/R2/R3/...
 * @param bCloseClk : Close Clock or not
 *
 * @return RspErrEmType  : Response Error Code
 ----------------------------------------------------------------------------------------------------------*/
 extern u8 gCmd;
RspErrEmType Hal_SDMMC_SendCmdAndWaitProcess(IPEmType eIP, TransEmType eTransType, CmdEmType eCmdType, SDMMCRspEmType eRspType, BOOL_T bCloseClk)
{
	U32_T u32WaitMS	= 0;
	U16_T u16WaitMIEEvent = 0;
	U16_T u16MIE_PATH_MMA_EN = (U8_T)eTransType;
	U16_T u16MIE_TRANS_END = (u16MIE_PATH_MMA_EN<<14) | (u16MIE_PATH_MMA_EN<<11) | ((!u16MIE_PATH_MMA_EN)<<2);
	U16_T u16VAR_SD_MODE = _REG_GetPortSetting(eIP) | gu16_SD_MODE_DatLine[eIP] | gu16_SD_Mode_DatSync[eIP];

    if ( _REG_ClearSDSTS(eIP, RT_CLEAN_SDSTS) || _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_RIU_ERR, TRUE, __LINE__);

#if (EN_MIU_WDEN_PATCH_DIS)
	CARD_REG16_ON(A_NC_REORDER_REG(eIP), R_MIU_WDEN_PATCH_DIS);
#endif
#if(EN_BIND_CARD_INT)
	Hal_CARD_INT_SetMIEIntEn(eIP, EV_INT_SD, V_MIE_INT_EN_INIT | u16MIE_TRANS_END);
#endif
	CARD_REG(A_SD_MODE_REG(eIP)) = V_SD_MODE_INIT | u16VAR_SD_MODE | (eTransType>>8) | (u16MIE_PATH_MMA_EN<<11);
	CARD_REG(A_CMD_SIZE_REG(eIP)) = V_CMD_SIZE_INIT;
	CARD_REG(A_RSP_SIZE_REG(eIP)) = V_RSP_SIZE_INIT | ((U8_T)eRspType);
	CARD_REG(A_MIE_PATH_CTL_REG(eIP)) = V_MIE_PATH_INIT;
	CARD_REG(A_SD_CTL_REG(eIP))  = V_SD_CTL_INIT | (eRspType>>12) | (eCmdType>>4);

	Hal_Timer_uDelay(gu16_WT_NAC[eIP]);

	/*if ( _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
		return _SDMMC_ErrProcess(eIP, EV_STS_DAT0_BUSY, TRUE, __LINE__);*/	

	if ( (eTransType== EV_DMA) && _REG_WaitFIFOClkRdy(eIP, eCmdType, WT_FIFO_CLK_RDY) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_FIFO_NRDY, TRUE, __LINE__);

	if(eCmdType==EV_CMDREAD)
	{
		u16WaitMIEEvent = R_SD_CMD_END | u16MIE_TRANS_END;
		u32WaitMS = WT_EVENT_RSP + gu16_WT_READ[eIP];
		CARD_REG16_ON(A_MIE_PATH_CTL_REG(eIP), u16MIE_PATH_MMA_EN);
		CARD_REG16_ON(A_SD_CTL_REG(eIP), (R_SD_CMD_EN | R_SD_DTRX_EN) );
	}
	else
	{
		u16WaitMIEEvent = R_SD_CMD_END;
		u32WaitMS = WT_EVENT_RSP;
		CARD_REG16_ON(A_SD_CTL_REG(eIP), R_SD_CMD_EN);
	}

	if ( _REG_WaitMIEEvent(eIP, u16WaitMIEEvent, u32WaitMS) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_MIE_TOUT, TRUE, __LINE__);

	if(eRspType == EV_R3)  // For IP CRC bug
		CARD_REG(A_SD_STS_REG(eIP)) = R_SD_CMDRSP_CERR; //Clear CMD CRC Error
	else if ( (eRspType == EV_R1B) && _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_DAT0_BUSY, TRUE, __LINE__);

	if( (eCmdType==EV_CMDWRITE) && (!M_REG_STSERR(eIP)) )
	{

		if ( _REG_ClearSDSTS(eIP, RT_CLEAN_SDSTS) || _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
			return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_RIU_ERR, TRUE, __LINE__);

		CARD_REG16_ON(A_MIE_PATH_CTL_REG(eIP), u16MIE_PATH_MMA_EN);
		CARD_REG16_ON(A_SD_CTL_REG(eIP), R_SD_DTRX_EN);

		if ( _REG_WaitMIEEvent(eIP, u16MIE_TRANS_END, gu16_WT_WRITE[eIP]) )
			return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_MIE_TOUT, TRUE, __LINE__);

		if ( (eTransType== EV_CIF) && _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
			return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_DAT0_BUSY, TRUE, __LINE__);

	}

	return _SDMMC_ErrProcess(eIP, eCmdType, (RspErrEmType)M_REG_STSERR(eIP), bCloseClk, __LINE__);
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_RunBrokenDmaAndWaitProcess
 *     @author jeremy.wang (2011/12/1)
 * Desc: For Broken DMA Data Transfer
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eCmdType : READ/WRITE
 *
 * @return RspErrEmType  : Response Error Code
 ----------------------------------------------------------------------------------------------------------*/
RspErrEmType Hal_SDMMC_RunBrokenDmaAndWaitProcess(IPEmType eIP, CmdEmType eCmdType)
{
	U32_T u32WaitMS	= 0;

	if(eCmdType==EV_CMDREAD)
		u32WaitMS = gu16_WT_READ[eIP];
	else if(eCmdType==EV_CMDWRITE)
		u32WaitMS = gu16_WT_WRITE[eIP];

	if ( _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_RIU_ERR, TRUE, __LINE__);

    if ( _REG_WaitFIFOClkRdy(eIP, eCmdType, WT_FIFO_CLK_RDY) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_FIFO_NRDY, TRUE, __LINE__);

    if ( _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_DAT0_BUSY, TRUE, __LINE__);

	CARD_REG16_ON(A_MIE_PATH_CTL_REG(eIP), R_MMA_ENABLE);
	CARD_REG16_ON(A_SD_CTL_REG(eIP), R_SD_DTRX_EN);

	if ( _REG_WaitMIEEvent(eIP, R_CARD_DMA_END|R_MMA_LSDONE_END, u32WaitMS) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_MIE_TOUT, TRUE, __LINE__);

	return EV_STS_OK;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_WriteProtect
 *     @author jeremy.wang (2011/12/1)
 * Desc: Check Card Write Protect
 *
 * @param eIP : FCIE1/FCIE2/...
 *
 * @return BOOL_T  : Write Protect or not
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_SDMMC_WriteProtect(IPEmType eIP)
{
	CARD_REG(A_MIE_PATH_CTL_REG(eIP)) = V_MIE_PATH_INIT;
	CARD_REG(A_SD_MODE_REG(eIP)) = _REG_GetPortSetting(eIP);

	if ( CARD_REG(A_SD_STS_REG(eIP)) & R_SD_WR_PRO_N )
		return (FALSE);  //No Write Protect
	return (TRUE);

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_CardDetect
 *     @author jeremy.wang (2011/10/11)
 * Desc: Check Card Detection
 *
 * @param eIP : FCIE1/FCIE2/...
 *
 * @return BOOL_T  : Card Detection or not
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_SDMMC_CardDetect(IPEmType eIP)
{
	CARD_REG(A_MIE_PATH_CTL_REG(eIP)) = V_MIE_PATH_INIT;
	CARD_REG(A_SD_MODE_REG(eIP)) = _REG_GetPortSetting(eIP);

	if ( _REG_GetCDET(eIP) )
		return (FALSE);  //No Card Detect
	return (TRUE);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_ClkCtrl
 *     @author jeremy.wang (2011/12/14)
 * Desc: OpenCard Clk for Special Request (We always Open/Close Clk by Every Cmd)
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bOpen : Open Clk or not
 * @param u16DelayMs :  Delay ms to Specail Purpose
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_ClkCtrl(IPEmType eIP, BOOL_T bOpen, U16_T u16DelayMs)
{
	CARD_REG(A_MIE_PATH_CTL_REG(eIP)) = V_MIE_PATH_INIT;
	CARD_REG(A_SD_MODE_REG(eIP)) = _REG_GetPortSetting(eIP);

	if( bOpen )
		CARD_REG16_ON(A_SD_MODE_REG(eIP), R_SD_CLK_EN );
	else
		CARD_REG16_OFF(A_SD_MODE_REG(eIP), R_SD_CLK_EN );

    Hal_Timer_uDelay(u16DelayMs);
}


//***********************************************************************************************************
// SD Other Setting
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_Reset
 *     @author jeremy.wang (2011/11/30)
 * Desc: Reset IP to avoid IP Dead + Touch CIFD first time to avoid latch issue
 *
 * @param eIP :  FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_Reset(IPEmType eIP)
{
	_REG_ResetIP(eIP);
	_REG_CIFDEmptyRW(eIP);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_WaitProcessCtrl
 *     @author jeremy.wang (2011/12/1)
 * Desc: Stop Process to Avoid Long Time Waiting
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bStop : Stop Process or not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_WaitProcessCtrl(IPEmType eIP, BOOL_T bStop)
{
	gb_StopWProc[eIP] = bStop;

#if(EN_BIND_CARD_INT)
	if ( gb_StopWProc[eIP] )
		Hal_CARD_INT_WaitMIEEventCtrl(eIP, TRUE);
	else
		Hal_CARD_INT_WaitMIEEventCtrl(eIP, FALSE);
#endif

}
















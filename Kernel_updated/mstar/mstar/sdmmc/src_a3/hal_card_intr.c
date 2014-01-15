/***************************************************************************************************************
 *
 * FileName hal_card_intr.c
 *     @author jeremy.wang (2011/07/21)
 * Desc:
 *     The Interrupt behavior of all cards will run here.
 *     The goal is that we don't need to change HAL Level code (But its h file code)
 *
 *     The limitations were listed as below:
 *     (1) This c file belongs to HAL level.
 *     (2) Its h file is included by driver API or HAL level, not driver flow process.
 *     (3) MIE Event Int and Card Change Event Int function belong to here.
 *     (4) FCIE/SDIO IP interrupt register function belong to here.
 *     (5) Because ISR belongs to OS design, so we must use OS define option to separate them.
 *     (6) This c file could not use project/cpu/icver/specific define option here, but its h file could.
 *
 *     P.S. EN_XX for ON/OFF Define, V_XX  for Value Define,
 *          RT_XX for Retry Times Define, WT_XX for Wait Time Define, M_XX for Mask Value Define
 *
 ***************************************************************************************************************/

#include "hal_card_intr.h"
#include "hal_card_platform.h"

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************
// Wait event timeout value
#define SD_WAIT_MIE_TOMEOUT  (320000)

// Enable Setting
//-----------------------------------------------------------------------------------------------------------
#define EN_TRFUNC         (FALSE)

// Value Setting
//-----------------------------------------------------------------------------------------------------------
#define V_MAXSLOTS          2

// Wait Time
//-----------------------------------------------------------------------------------------------------------
#define WT_INT_WAKEUP       10		//(ms)

//***********************************************************************************************************


// Reg Static Init Setting
//-----------------------------------------------------------------------------------------------------------
#define V_SD_MIE_INT_INIT   (R_SD_CMD_END_EN|R_SD_DATA_END_EN|R_CARD_DMA_END_EN|R_MMA_LSDONE_END_EN)

// Mask Range
//-----------------------------------------------------------------------------------------------------------
#define M_CARD_PATH         (R_SD_EN|R_MS_EN|R_CF_EN|R_SM_EN)
#define M_O_CARD_STS        (R_MS_STS_CHG|R_CF_STS_CHG|R_SM_STS_CHG|R_XD_STS_CHG)

//IP_FCIE or IP_SDIO Register Basic Address
//-----------------------------------------------------------------------------------------------------------
#define A_SD_REG_BANK(IP)           GET_CARD_BANK(IP, EV_REG_BANK)

#define A_MIE_EVENT_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x00)
#define A_MIE_INT_EN_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x01)
#define A_CARD_EVENT_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x05)
#define A_CARD_INT_EN_REG(IP)       GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x06)
#define A_MIE_PATH_CTL_REG(IP)      GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0A)

// Reg Dynamic Variable
//-----------------------------------------------------------------------------------------------------------
static volatile U16_T	gu16_IntMode[2] = {0};
static volatile U16_T	gu16_MIEEvent_ForInt[2] = {0};   //MIEEvent for Interrupt
static volatile BOOL_T  gb_StopWaitMIE[2] = {0};
extern bool gb_card_chg[2];
extern atomic_t sd_in_use;
// Trace Funcion 
//-----------------------------------------------------------------------------------------------------------
#if (EN_TRFUNC)
	#define TR_INT(p)			p
#else
	#define TR_INT(p)
#endif

// Register Operation Define ==> For Return SD Change Status Pos
//-----------------------------------------------------------------------------------------------------------
static U16_T _REG_SDCHG(IPEmType eIP)
{
	PortEmType ePort = GET_CARD_PORT(eIP);

	switch( (U8_T)ePort )
	{
		case (U8_T)EV_PORT_SDIO1:
			return R_SDIO_STS_CHG;
		case (U8_T)EV_PORT_SDIO2:
			return R_SDIO2_STS_CHG;
		case (U8_T) EV_PORT_SD:
			return R_SD_STS_CHG;
		default:
			return 0;
	}
}

// Register Operation Define ==> For Return SD Change Status Int Pos
//-----------------------------------------------------------------------------------------------------------
#if !(defined(CONFIG_MSTAR_SDMMC_GPIO_CDZ) && CONFIG_MSTAR_SDMMC_GPIO_CDZ)
static U16_T _REG_SDCHG_INTEN(IPEmType eIP)
{
	PortEmType ePort = GET_CARD_PORT(eIP);

	switch( (U8_T)ePort )
	{
		case (U8_T)EV_PORT_SDIO1:
			return R_SDIO_STS_EN;
		case (U8_T)EV_PORT_SDIO2:
			return R_SDIO2_STS_EN;
		case (U8_T) EV_PORT_SD:
			return R_SD_STS_EN;
		default:
			return 0;
	}
}

// Detect Card Change Int From IP
//-----------------------------------------------------------------------------------------------------------
static BOOL_T _CARD_INT_DetectCarDChangeFromIP(IPEmType eIP, U16_T *pu16CardCH)
{
	U16_T u16CardEvent = 0, u16CardInt = 0;
	U16_T u16SDMMCStatus = 0, u16OCardStatus = 0;
	U16_T u16CardCH = 0;
	BOOL_T bRet = FALSE;

	u16CardEvent = CARD_REG(A_CARD_EVENT_REG(eIP));
	u16CardInt	 = CARD_REG(A_CARD_INT_EN_REG(eIP));

	if((u16CardEvent==0) || (u16CardInt==0)) // No Card Int Event
		return FALSE;

	u16SDMMCStatus =  ((u16CardEvent & _REG_SDCHG(eIP))>=1) && ((u16CardInt & _REG_SDCHG_INTEN(eIP))>=1);
	u16OCardStatus =  u16CardEvent & u16CardInt & M_O_CARD_STS;

	TR_INT(prtstring("\n>> @@@@@ DetectCH...CINT("));
	TR_INT(prtU32Hex(CARD_REG(A_CARD_INT_EN_REG(eIP))));
	TR_INT(prtstring("),CENT("));
	TR_INT(prtU32Hex(CARD_REG(A_CARD_EVENT_REG(eIP))));
	TR_INT(prtstring(")\n"));


	/******  SDMMMC Card Change Interrupt Event *******/
	if(u16SDMMCStatus)
	{
		CARD_REG16_OFF(A_CARD_INT_EN_REG(eIP), _REG_SDCHG_INTEN(eIP));    //Disable SDMMC Card Change Int
		CARD_REG(A_CARD_EVENT_REG(eIP)) = _REG_SDCHG(eIP);                //Clear SDMMC Card Change Event

		TR_INT(prtstring("\n>> @@@@@ Int DisableCH...CINT("));
		TR_INT(prtU32Hex(CARD_REG(A_CARD_INT_EN_REG(eIP))));
		TR_INT(prtstring("),CENT("));
		TR_INT(prtU32Hex(CARD_REG(A_CARD_EVENT_REG(eIP))));
		TR_INT(prtstring(")\n"));

		u16CardCH |= EV_INT_SD;
		bRet = TRUE;
	}
	/******  Other Card Change Interrupt Event *******/
	if(u16OCardStatus)
	{
		CARD_REG16_OFF(A_CARD_INT_EN_REG(eIP), u16OCardStatus);          //Disable Other Card Change Int
		CARD_REG(A_CARD_EVENT_REG(eIP)) = u16OCardStatus;                //Clear Other Card Change Event
		u16CardCH |= u16OCardStatus;
		bRet = TRUE;
	}

	*pu16CardCH = u16CardCH;
	return bRet;
}
#endif

// Save MIE Int Event to Global variable
//-----------------------------------------------------------------------------------------------------------
static BOOL_T _CARD_INT_SaveMIEEvent(IPEmType eIP)
{
	U16_T u16Reg = CARD_REG(A_MIE_PATH_CTL_REG(eIP));

	if(!(u16Reg & M_CARD_PATH))  //Not Card Event
		return FALSE;

	u16Reg = CARD_REG(A_MIE_EVENT_REG(eIP));

	gu16_MIEEvent_ForInt[eIP] |= u16Reg;  //Summary the MieEvent

	u16Reg &= CARD_REG(A_MIE_INT_EN_REG(eIP));

	/****** Clean MIE Event *******/
	CARD_REG(A_MIE_EVENT_REG(eIP)) = CARD_REG(A_MIE_EVENT_REG(eIP)) & u16Reg;

	return TRUE;
}


//***********************************************************************************************************
// Interrupt Handler Function
//***********************************************************************************************************

//###########################################################################################################
#if(D_OS == D_OS__LINUX)
//###########################################################################################################
static DECLARE_WAIT_QUEUE_HEAD(fcie1_mieint_wait);
static DECLARE_WAIT_QUEUE_HEAD(fcie2_mieint_wait);
struct tasklet_struct*   gpst_SDtaskletSlot[V_MAXSLOTS] = {NULL};
#endif
//###########################################################################################################

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_WaitMIEEvent
 *     @author jeremy.wang (2011/12/20)
 * Desc: Set WaitQueue Handler to Wait MIE Event
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u16ReqEvent : MIE Event for waiting
 * @param u32WaitMs : Waiting Time (ms)
 *
 * @return BOOL_T  : TRUE (Success), FALSE (Timeout)
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_INT_WaitMIEEvent(IPEmType eIP, U16_T u16ReqEvent, U32_T u32WaitMs)
{
//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################
    #if 1
	if(eIP==EV_IP_FCIE1)
		wait_event_timeout(fcie1_mieint_wait, ((gu16_MIEEvent_ForInt[eIP]& u16ReqEvent) == u16ReqEvent) || gb_StopWaitMIE[eIP], msecs_to_jiffies(u32WaitMs+WT_INT_WAKEUP));
	else if((eIP==EV_IP_FCIE2))
		wait_event_timeout(fcie2_mieint_wait, ((gu16_MIEEvent_ForInt[eIP]& u16ReqEvent) == u16ReqEvent) || gb_StopWaitMIE[eIP], msecs_to_jiffies(u32WaitMs+WT_INT_WAKEUP));
    #else
    if(eIP==EV_IP_FCIE1)
		wait_event_timeout(fcie1_mieint_wait, ((gu16_MIEEvent_ForInt[eIP]& u16ReqEvent) == u16ReqEvent) || gb_StopWaitMIE[eIP], SD_WAIT_MIE_TOMEOUT/HZ);
	else if((eIP==EV_IP_FCIE2))
		wait_event_timeout(fcie2_mieint_wait, ((gu16_MIEEvent_ForInt[eIP]& u16ReqEvent) == u16ReqEvent) || gb_StopWaitMIE[eIP], SD_WAIT_MIE_TOMEOUT/HZ);
    #endif
	if((gu16_MIEEvent_ForInt[eIP] & u16ReqEvent) != u16ReqEvent)
		return FALSE;

	return TRUE;
//###########################################################################################################
#endif

	/****** Not Support (Ex: D_OS_ARCH = D_OS_UBOOT) ******/
	return FALSE;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_Card_INT_SetCardChangeEventHandler
 *     @author jeremy.wang (2011/12/20)
 * Desc: Set Card Change Event Handler
 *
 * @param u32Slot :  Slot No. (Current Maximum 1)
 * @param eCardInt : SD/MS/CF/XD...
 * @param pst_event : Event Handler
 ----------------------------------------------------------------------------------------------------------*/
RetEmType Hal_Card_INT_SetCardChangeEventHandler(U32_T u32Slot, IntCardEmType eCardInt, unsigned * pst_event)
{
	if (u32Slot>=V_MAXSLOTS)
		return EV_FAIL;

//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################

	if(eCardInt == EV_INT_SD)
	{
		gpst_SDtaskletSlot[u32Slot] = (struct tasklet_struct*) pst_event;
	}
	else if(eCardInt ==EV_INT_MS)
	{
	}

//###########################################################################################################
#endif

	return EV_OK;

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_WaitMIEEventCtrl
 *     @author jeremy.wang (2011/12/20)
 * Desc: Stop Wait MIE Event to Avoid Long Time Waiting
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bStop : Stop Wait or Not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_WaitMIEEventCtrl(IPEmType eIP, BOOL_T bStop)
{
	gb_StopWaitMIE[eIP] = bStop;

//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################
	if(gb_StopWaitMIE[eIP])
	{
		 /****** Break Current Wait Event *******/
		if(eIP==EV_IP_FCIE1)
			wake_up(&fcie1_mieint_wait);
		else if((eIP==EV_IP_FCIE2))
			wake_up(&fcie2_mieint_wait);
	}
#endif

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_ModeCtrl
 *     @author jeremy.wang (2011/12/20)
 * Desc: Open Card Interrupt Mode
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eCardInt : SD/MS/CF/XD...
 * @param bOpen : Open Card Interrupt Mode or Not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_ModeCtrl(IPEmType eIP, IntCardEmType eCardInt, BOOL_T bOpen)
{
	if(bOpen)
		gu16_IntMode[eIP] |= eCardInt;
	else
		gu16_IntMode[eIP]  &= (~eCardInt);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_ModeRunning
 *     @author jeremy.wang (2011/9/7)
 * Desc: Check Int Mode Running or Not
 *
 * @param eIP :  FCIE1/FCIE2/...
 * @param eCardInt : SD/MS/CF/XD...
 *
 * @return BOOL_T  : Interrupt Running or Not
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_INT_ModeRunning(IPEmType eIP, IntCardEmType eCardInt)
{
	if(gu16_IntMode[eIP] & eCardInt )
		return TRUE;
	else
		return FALSE;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_SetMIEIntEn
 *     @author jeremy.wang (2011/12/20)
 * Desc: Set MIE_Int_En to Trig Interrupt
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eCardInt : SD/MS/CF/XD...
 * @param u16RegMIEIntEN : MIE_Int_En Reg Setting
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_SetMIEIntEn(IPEmType eIP, IntCardEmType eCardInt, U16_T u16RegMIEIntEN)
{
	/****** Set SD MIE Int Event ******/
	if(eCardInt == EV_INT_SD)
	{
		if(gu16_IntMode[eIP] & EV_INT_SD)
			CARD_REG(A_MIE_INT_EN_REG(eIP)) = u16RegMIEIntEN ;  //Enable MIE_INT_EN
		else
			CARD_REG(A_MIE_INT_EN_REG(eIP)) = 0;                //Clear MIE_INT_EN to Avoid Interrupt
	}
	else if(eCardInt ==EV_INT_MS)
	{
	}

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_ClearMIEEvent
 *     @author jeremy.wang (2011/9/7)
 * Desc: Clear MIE Event That We Have Saved in Interrupt Mode
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_ClearMIEEvent(IPEmType eIP)
{
	gu16_MIEEvent_ForInt[eIP] = 0;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_GetMIEEvent
 *     @author jeremy.wang (2011/9/7)
 * Desc: Get Current MIE Event That We Have Saved in Interrupt Mode
 *
 * @param eIP : FCIE1/FCIE2/...

 * @return U16_T  : Current MIE Event
 ----------------------------------------------------------------------------------------------------------*/
U16_T Hal_CARD_INT_GetMIEEvent(IPEmType eIP)
{
	return gu16_MIEEvent_ForInt[eIP];
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_SetCardIntEn
 *     @author jeremy.wang (2011/9/7)
 * Desc: Set Card_Int_En to Trig Interrupt
 *
 * @param eIP :  FCIE1/FCIE2/...
 * @param eCardInt :  SD/MS/CF/XD...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_SetCardIntEn(IPEmType eIP, IntCardEmType eCardInt)
{
    #if !defined(CONFIG_MSTAR_SDMMC_GPIO_CDZ)
	if(eCardInt == EV_INT_SD)
	{
		if(gu16_IntMode[eIP] & EV_INT_SD)
		{
			CARD_REG16_ON(A_CARD_INT_EN_REG(eIP), _REG_SDCHG_INTEN(eIP)); //Enable SDMMC Card Change Int

			TR_INT(prtstring("\n>> [sdmmc] #Int EnableCH...CINT("));
			TR_INT(prtU32Hex(CARD_REG(A_CARD_INT_EN_REG(eIP))));
			TR_INT(prtstring("),CENT("));
			TR_INT(prtU32Hex(CARD_REG(A_CARD_EVENT_REG(eIP))));
			TR_INT(prtstring(")\n"));
		}
		else
			CARD_REG16_OFF(A_CARD_INT_EN_REG(eIP), _REG_SDCHG_INTEN(eIP)); //Disable SDMMC Card Change Int
	}
	else if(eCardInt ==EV_INT_MS)
	{
	}
    #endif

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_ClearCardEvent
 *     @author jeremy.wang (2011/12/21)
 * Desc: Clear Card Event
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eCardInt : SD/MS/CF/XD...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_ClearCardEvent(IPEmType eIP, IntCardEmType eCardInt)
{
	if(eCardInt == EV_INT_SD)
		CARD_REG(A_CARD_EVENT_REG(eIP)) =  _REG_SDCHG(eIP);   //Clear SDMMC Card Change Event
}


//***********************************************************************************************************
// Interrupt Function
//***********************************************************************************************************

//###########################################################################################################
#if(D_OS == D_OS__LINUX)
//###########################################################################################################
#if defined(CONFIG_MSTAR_SDMMC_GPIO_CDZ) && CONFIG_MSTAR_SDMMC_GPIO_CDZ
extern U16_T Show_CARD_GPIO16(void);
irqreturn_t Hal_CARD_CDZ_INT(int irq, void *p_dev_id)
{
    irqreturn_t eIrq = IRQ_NONE;
    IntSourceStruct* pstIntSource = p_dev_id;
    U32_T u32Slot = pstIntSource->slotNo;

    //printk("\033[31mHal_CARD_CDZ_INT(%x)\033[0m\n", Show_CARD_GPIO16());
    /****** Detect Card Change   *******/
	if (Hal_CARD_ProcCdInt() && (gpst_SDtaskletSlot[u32Slot]!=NULL))
	{
		 gb_card_chg[u32Slot] = TRUE;
     tasklet_schedule(gpst_SDtaskletSlot[u32Slot]);
     eIrq = IRQ_HANDLED;
	}

    return eIrq;
}

irqreturn_t Hal_CARD_INT(int irq, void *p_dev_id)
{
	irqreturn_t eIrq = IRQ_NONE;
	IntSourceStruct* pstIntSource = p_dev_id;
	IPEmType eIP  = pstIntSource->eIP;

    if (atomic_read(&sd_in_use) == 0)
        return IRQ_NONE;

	/****** Save MIE Event to Global Variable *******/
	if(_CARD_INT_SaveMIEEvent(eIP))
	{
		if(eIP==EV_IP_FCIE1)
			wake_up(&fcie1_mieint_wait);
		else if((eIP==EV_IP_FCIE2))
			wake_up(&fcie2_mieint_wait);

		eIrq = IRQ_HANDLED;
	}

	return eIrq;
}
#else
irqreturn_t Hal_CARD_INT(int irq, void *p_dev_id)
{
	irqreturn_t eIrq = IRQ_NONE;
	U16_T u16CardCH = 0;
	IntSourceStruct* pstIntSource = p_dev_id;
	IPEmType eIP  = pstIntSource->eIP;
	U32_T u32Slot = pstIntSource->slotNo;

    /****** Detect Card Change   *******/
	if(_CARD_INT_DetectCarDChangeFromIP(eIP, &u16CardCH))
	{
		if((u16CardCH & EV_INT_SD) && (gpst_SDtaskletSlot[u32Slot]!=NULL))
		{
			tasklet_schedule(gpst_SDtaskletSlot[u32Slot]);
		}

		eIrq = IRQ_HANDLED;
	}

    if (atomic_read(&sd_in_use) == 0)
        return IRQ_NONE;

	/****** Save MIE Event to Global Variable *******/
	if(_CARD_INT_SaveMIEEvent(eIP))
	{
		if(eIP==EV_IP_FCIE1)
			wake_up(&fcie1_mieint_wait);
		else if((eIP==EV_IP_FCIE2))
			wake_up(&fcie2_mieint_wait);

		eIrq = IRQ_HANDLED;
	}

	return eIrq;
}
#endif
//###########################################################################################################
#endif









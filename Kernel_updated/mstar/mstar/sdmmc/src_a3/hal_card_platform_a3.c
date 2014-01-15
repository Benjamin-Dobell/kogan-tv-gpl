/***************************************************************************************************************
 *
 * FileName hal_card_platform_cb2.c
 *     @author jeremy.wang (2010/10/14)
 * Desc:
 *     The platform Setting of all cards will run here.
 *     Every Project will have XX project name for different hal_card_platform_XX.c files
 *     The goal is that we don't need to change "other" HAL Level code.
 *
 *     The limitations were listed as below:
 *     (1) This c file belongs to HAL level.
 *     (2) Its h file is included by driver API level, not driver flow process.
 *     (3) IP Init, PADPath, PADInit, Clock and Power function belong to here.
 *     (4) Timer Setting doesn't belong to here, because it will be included by other HAL level.
 *     (5) FCIE/SDIO IP Reg Setting doesn't belong to here.
 *     (6) If we could, we don't need to change any code of hal_card_platform.h
 *
 ***************************************************************************************************************/

#include "hal_card_platform.h"
#include "hal_card_timer.h"

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC	
extern bool mstar_mci_exit_checkdone_ForSD(void);
#endif

// Platform Register Basic Address
//-----------------------------------------------------------------------------------------------------------
#define A_PMGPIO_BANK       GET_CARD_REG_ADDR(A_RIU_PM_BASE, 0x0780)
#define A_CHIPTOP_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x0F00)
#define A_CLKGEN_BANK       GET_CARD_REG_ADDR(A_RIU_BASE, 0x0580)


#define TOP_SD_CONFIG       GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x5A)
#define TOP_SD2_CONFIG      GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x1C)
#define TOP_EMMC_CONFIG     GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x6E)
#define TOP_NAND_CONFIG     GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x6F)
#define TOP_PCM_CONFIG      GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x64)

#define FCIE_MIU_DMA_SEL    GET_CARD_REG_ADDR(A_FCIE1_0_BANK, 0x03)
#define FCIE_MIE_PATH_CTL   GET_CARD_REG_ADDR(A_FCIE1_0_BANK, 0x0A)
#define FCIE_BOOT_CFG_REG   GET_CARD_REG_ADDR(A_FCIE1_0_BANK, 0x2F)
//===========================================================================================================
#define REG_GPIO_PM10       GET_CARD_REG_ADDR(A_PMGPIO_BANK, 0x0A)
#define REG_GPIO_PM16       GET_CARD_REG_ADDR(A_PMGPIO_BANK, 0x10)

#define BIT_GPIO10_IN         BIT02
#define BIT_GPIO10_FIQ_MASK   BIT04
#define BIT_GPIO10_FIQ_CLR    BIT06
#define BIT_GPIO10_FIQ_POL    BIT07
#define BIT_GPIO10_FIQ_FINAL  BIT08

#define BIT_GPIO16_IN         BIT02
#define BIT_GPIO16_FIQ_MASK   BIT04
#define BIT_GPIO16_FIQ_CLR    BIT06
#define BIT_GPIO16_FIQ_POL    BIT07
#define BIT_GPIO16_FIQ_FINAL  BIT08

#if defined(CONFIG_MSTAR_SDMMC_GPIO_CDZ) && CONFIG_MSTAR_SDMMC_GPIO_CDZ
#if defined(CONFIG_MSTAR_SDMMC_GPIO_PM10)
#define REG_GPIO_PM           REG_GPIO_PM10

#define BIT_GPIO_IN           BIT_GPIO10_IN
#define BIT_GPIO_FIQ_MASK     BIT_GPIO10_FIQ_MASK
#define BIT_GPIO_FIQ_CLR      BIT_GPIO10_FIQ_CLR
#define BIT_GPIO_FIQ_POL      BIT_GPIO10_FIQ_POL
#define BIT_GPIO_FIQ_FINAL    BIT_GPIO10_FIQ_FINAL
#elif defined(CONFIG_MSTAR_SDMMC_GPIO_PM16)
#define REG_GPIO_PM           REG_GPIO_PM16

#define BIT_GPIO_IN           BIT_GPIO16_IN
#define BIT_GPIO_FIQ_MASK     BIT_GPIO16_FIQ_MASK
#define BIT_GPIO_FIQ_CLR      BIT_GPIO16_FIQ_CLR
#define BIT_GPIO_FIQ_POL      BIT_GPIO16_FIQ_POL
#define BIT_GPIO_FIQ_FINAL    BIT_GPIO16_FIQ_FINAL
#else
#error  "Please select MSTAR_SDMMC_GPIO_PM10 or 16!"
#endif
#endif

// Clock Level Setting (From High Speed to Low Speed)
//-----------------------------------------------------------------------------------------------------------
#define CLK_F          48000000
#define CLK_E          43000000
#define CLK_D          40000000
#define CLK_C          36000000
#define CLK_B          32000000
#define CLK_A          27000000
#define CLK_9          24000000
#define CLK_8          20000000
#define CLK_7          300000
#define CLK_6          0
#define CLK_5          0
#define CLK_4          0
#define CLK_3          0
#define CLK_2          0
#define CLK_1          0
#define CLK_0          0

// Clock Pass Level Setting
//-----------------------------------------------------------------------------------------------------------
#define CLK_PASS_LEVEL     2      // From Known limitation, We pass number of level (ex. 3 =>CLK_C)

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_Wait_Emmc_D0
 *     @author benson.hsiao (2012/6/21)
 * Desc:  Wait EMMC D0 to High.
 *
 * @param eIP :
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_Wait_Emmc_D0(void)	
{
    #if 0
    int i;

	if (CARD_REG(NAND_CONFIG) & REG_CHK_NAND_PAD)
		return TRUE;
	
    for(i=0;i<1000;i++)
    {
        if (mstar_mci_check_D0_status())
			return TRUE;
		udelay(1);
    }
	for(i=0;i<WAIT_EMMC_D0_TIME;i++)
    {
        if (mstar_mci_check_D0_status())
			return TRUE;
		schedule_timeout(1);
    }
	printk("Wait EMMC D0 time out!\n");
	return FALSE;
	#else	
	
	#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC	
	return (mstar_mci_exit_checkdone_ForSD());
    #else
    return TRUE;
	#endif
		
	#endif
}

//***********************************************************************************************************
// PAD Setting for Card Platform
//***********************************************************************************************************
atomic_t sd_in_use= ATOMIC_INIT(0);
//------------------------------------------------------------------------------------------------
u16 save_nand_reg;
u16 save_pcm_reg;
void enable_top_sdpad(u8 enable)
{
    if (enable)
    {
        #if defined(CONFIG_MSTAR_SDMMC1_PORT1)
            CARD_REG_W16(TOP_SD_CONFIG, (CARD_REG_R16(TOP_SD_CONFIG)|BIT05));
        #else
            CARD_REG_W16(TOP_SD2_CONFIG, (CARD_REG_R16(TOP_SD2_CONFIG)|BIT12));
        #endif
    }
    else
    {
        #if defined(CONFIG_MSTAR_SDMMC1_PORT1)
            CARD_REG_W16(TOP_SD_CONFIG, (CARD_REG_R16(TOP_SD_CONFIG)&~BIT05));
        #else
            CARD_REG_W16(TOP_SD2_CONFIG, (CARD_REG_R16(TOP_SD2_CONFIG)&~BIT12));
        #endif
    }
}
#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
extern void enable_top_emmcpad(u8 enable);
#endif

void sd_bus_obtain(u8 select)
{
    u16 nand_reg, pcm_reg, reg_boot_cfg;

    nand_reg = CARD_REG_R16(TOP_NAND_CONFIG);
    pcm_reg = CARD_REG_R16(TOP_PCM_CONFIG);
    reg_boot_cfg = CARD_REG_R16(FCIE_BOOT_CFG_REG);

    if (select)   // select sd
    {   
        #if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
        enable_top_emmcpad(0);
		#endif
    	save_pcm_reg = pcm_reg;
        save_nand_reg = nand_reg;
        nand_reg &= ~(BIT06|BIT07);
        CARD_REG_W16(TOP_NAND_CONFIG, nand_reg);
        pcm_reg &= ~(BIT04);
        CARD_REG_W16(TOP_PCM_CONFIG, pcm_reg);
        CARD_REG_W16(FCIE_BOOT_CFG_REG, 0x400);  // set bypass mode
        atomic_set(&sd_in_use, 1);
    }
    else // release
    {
        CARD_REG_W16(FCIE_MIU_DMA_SEL, 0);
        CARD_REG_W16(TOP_NAND_CONFIG, save_nand_reg);
        CARD_REG_W16(TOP_PCM_CONFIG, save_pcm_reg);
        CARD_REG_W16(FCIE_BOOT_CFG_REG, reg_boot_cfg);
        atomic_set(&sd_in_use, 0);
    }
}

void Hal_CARD_InitFCIE(IPEmType eIP)
{
    // Set MIU select determined by FCIE, not MIU itself
    CARD_REG_W16(0x1f200de8, CARD_REG_R16(0x1f200de8)|BIT09);
    #if (!defined(CONFIG_MMC_MSTAR_MMC_EMMC))
    CARD_REG_W16(FCIE_BOOT_CFG_REG, 0x400);  // set bypass mode
    #endif
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitSetting
 *     @author jeremy.wang (2011/12/1)
 * Desc: Init IP relative Reg Setting for Different Project
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_InitSetting(IPEmType eIP)
{
    // Todo
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitPADPin
 *     @author jeremy.wang (2011/12/1)
 * Desc: Init PAD Pin Status ( pull enable, pull up/down, driving strength)
 *
 * @param ePAD : PAD
 * @param bTwoCard : two card(1 bit) or not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_InitPADPin(PADEmType ePAD, BOOL_T bTwoCard)
{
   // Todo
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetPADToPortPath
 *     @author jeremy.wang (2011/12/1)
 * Desc: Set PAD to connect IP Port
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePort : IP Port
 * @param ePAD : PAD (Could Set NOPAD for 1-bit two cards)
 * @param bTwoCard : 1-bit two cards or not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetPADToPortPath(IPEmType eIP, PortEmType ePort, PADEmType ePAD, BOOL_T bTwoCard)
{
    //printk("1.Hal_CARD_SetPADToPortPath(%d,%d,%d)\n", eIP, ePort, ePAD);

    SET_CARD_PORT(eIP, ePort);

    //printk("2.Hal_CARD_SetPADToPortPath(%d,%d,%d)\n", eIP, ePort, ePAD);

    if(eIP == EV_IP_FCIE1)
    {
        if(ePort == EV_PORT_SD)
        {
            #if defined(CONFIG_MSTAR_SDMMC1_PORT1)
            //CHIPTOP
            CARD_REG16_ON(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x5A), BIT05);
            #else
            //CHIPTOP
            CARD_REG16_ON(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x1C), BIT12);    //[8:7] ==>3 sdio mode selection
            #endif
        }
    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetPADPower
 *     @author jeremy.wang (2012/1/4)
 * Desc: Set PAD Power to Different Voltage
 *
 * @param ePAD : PAD
 * @param ePADVdd : LOW/MIDDLE/HIGH Voltage Level
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetPADPower(PADEmType ePAD, PADVddEmType ePADVdd)
{
    // Todo
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PullPADPin
 *     @author jeremy.wang (2011/12/1)
 * Desc: Pull PAD Pin for Special Purpose (Avoid Power loss.., Save Power)
 *
 * @param ePAD :  PAD
 * @param ePin :  Group Pin
 * @param ePinPull : Pull up/Pull down
 * @param bTwoCard :  two card(1 bit) or not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PullPADPin(PADEmType ePAD, PinEmType ePin, PinPullEmType ePinPull, BOOL_T bTwoCard)
{
    // Todo
}


//***********************************************************************************************************
// Clock Setting for Card Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_FindClockSetting
 *     @author jeremy.wang (2011/12/14)
 * Desc: Find Real Clock Level Setting by Reference Clock
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u32ReffClk : Reference Clock Value
 * @param u8DownLevel : Down Level to Decrease Clock Speed
 *
 * @return U32_T  :  Real Clock
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_CARD_FindClockSetting(IPEmType eIP, U32_T u32ReffClk, U8_T u8DownLevel)
{
    U8_T  u8LV = CLK_PASS_LEVEL;
    U32_T u32RealClk = 0;
    U32_T u32ClkArr[16] = {CLK_F, CLK_E, CLK_D, CLK_C, CLK_B, CLK_A, \
        CLK_9, CLK_8, CLK_7, CLK_6, CLK_5, CLK_4, CLK_3, CLK_2, CLK_1, CLK_0};

    for(; u8LV<16; u8LV++)
    {
        if( (u32ReffClk >= u32ClkArr[u8LV]) || (u8LV==15) || (u32ClkArr[u8LV+1]==0) )
        {
            u32RealClk = u32ClkArr[u8LV];
            break;
        }
    }

    /****** For decrease clock speed******/
    if( (u8DownLevel) && (u32RealClk) && ((u8LV+u8DownLevel)<=15) )
    {
        if(u32ClkArr[u8LV+u8DownLevel]>0) //Have Level for setting
            u32RealClk = u32ClkArr[u8LV+u8DownLevel];
    }

    return u32RealClk;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetClock
 *     @author jeremy.wang (2011/12/14)
 * Desc: Set Clock Level by Real Clock from IP
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u32KClkFromIP : Clock Value From IP Source Set
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet)
{

    if(eIP == EV_IP_FCIE1)
    {
        CARD_REG16_OFF(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),BIT02|BIT03|BIT04|BIT05);
        switch(u32ClkFromIPSet)
        {
            case CLK_F:      //48000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT05|BIT04|BIT03|BIT02));
                break;
            case CLK_E:      //43000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT04|BIT03));
                break;
            case CLK_D:      //40000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT04|BIT02));
                break;
            case CLK_C:      //36000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT04));
                break;
            case CLK_B:      //32000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT03|BIT02));
                break;
            case CLK_A:      //27000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT03));
                break;
            case CLK_9:      //24000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT05|BIT04|BIT03));
                break;
            case CLK_8:      //20000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT02));
                break;
            case CLK_7:      //300KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT06|BIT05|BIT04|BIT02));
                break;
            /*
            case CLK_6:
                break;
            case CLK_5:
                break;
            case CLK_4:
            case CLK_3:
                break;
            case CLK_2:
                break;
            case CLK_1:
                break;
            case CLK_0:
                break;*/

        }

    }
}

//***********************************************************************************************************
// Power Setting for Card Platform
//***********************************************************************************************************
#define SD_EN_GPIO_NUMBER DrvPadmuxGetGpio(IO_CHIP_INDEX_SD_EN)
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PowerOn
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power on Card Power
 *
 * @param ePAD : PAD
 * @param eVdd : Vdd Voltage
 * @param u16DelayMs : Delay ms for Stable Power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PowerOn(PADEmType ePAD, VddEmType eVdd, U16_T u16DelayMs)
{
   // Todo

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PowerOff
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power off Card Power
 *
 * @param ePAD : PAD
 * @param u16DelayMs :  Delay ms to Confirm No Power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PowerOff(PADEmType ePAD, U16_T u16DelayMs)
{
    // Todo
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PowerOff
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power off Card Power
 *
 * @param ePAD : PAD
 * @param u16DelayMs :  Delay ms to Confirm No Power
 ----------------------------------------------------------------------------------------------------------*/
#if defined(CONFIG_MSTAR_SDMMC_GPIO_CDZ) && CONFIG_MSTAR_SDMMC_GPIO_CDZ
BOOL_T Hal_CARD_CardDetect_Gpio(void)
{
	if ( CARD_REG_R16(REG_GPIO_PM) & BIT_GPIO_IN )
		return (FALSE);  //No Card Detect
	return (TRUE);
}
BOOL_T Hal_CARD_ProcCdInt(void)
{
    if ( CARD_REG_R16(REG_GPIO_PM) & BIT_GPIO_FIQ_MASK )
		return (FALSE);
		if (( CARD_REG_R16(REG_GPIO_PM) & BIT_GPIO_FIQ_FINAL ) == 0)
		return (FALSE);
    do
    {
       CARD_REG16_ON(REG_GPIO_PM, BIT_GPIO_FIQ_CLR);
    }while(CARD_REG_R16(REG_GPIO_PM) & BIT_GPIO_FIQ_FINAL);

	return (TRUE);
}

void Hal_CARD_GPIO_IntEn(void)
{
    CARD_REG16_OFF(REG_GPIO_PM, BIT_GPIO_FIQ_MASK);
}

void Hal_CARD_GPIO_SetPol(U8_T pol)
{
    if (pol == 1)
        CARD_REG16_ON(REG_GPIO_PM, BIT_GPIO_FIQ_POL);
    else
        CARD_REG16_OFF(REG_GPIO_PM, BIT_GPIO_FIQ_POL);
}

U16_T Show_CARD_GPIO16(void)
{
    return (CARD_REG_R16(REG_GPIO_PM));
}
#endif










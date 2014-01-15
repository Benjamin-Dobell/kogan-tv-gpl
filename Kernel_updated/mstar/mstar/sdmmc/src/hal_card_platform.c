/***************************************************************************************************************
 *
 * FileName hal_card_platform.c
 *     @author benson.hsiao (2012/7/14)
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
#include "hal_sdmmc.h"

#include "chip_int.h"
#include <mstar/mstar_chip.h>
#include <linux/sched.h>

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
extern bool mstar_mci_exit_checkdone_ForSD(void);
#endif

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************
#define A_SD_REG_BANK(IP)           GET_CARD_BANK(IP, EV_REG_BANK)
#define A_MIE_EVENT_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x00)
#define A_MIE_INT_EN_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x01)
#define A_MIU_DMA1_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x03)
#define A_SD_MODE_REG(IP)           GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x10)
#define A_SD_STS_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x12)
#define A_NC_REORDER_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2D)
#define A_EMMC_BOOTCFG_REG(IP)      GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2F)

// Reg Dynamic Variable
//-----------------------------------------------------------------------------------------------------------
static volatile BOOL_T	    gb_SD_Mode_HighSpeed[2] = {0};
static U16_T gSdBak, gEmmcBak, gEmmcBak2;

// Timer Definition
//-----------------------------------------------------------------------------------------------------------

#define SD_DELAY_1us					1
#define SD_DELAY_10us					10
#define SD_DELAY_100us				    100
#define SD_DELAY_1ms					(1000 * SD_DELAY_1us)
#define SD_DELAY_10ms					(10   * SD_DELAY_1ms)
#define SD_DELAY_100ms				    (100  * SD_DELAY_1ms)
#define SD_DELAY_1s					    (1000 * SD_DELAY_1ms)

//***********************************************************************************************************
// IP Setting for Card Platform
//***********************************************************************************************************

#if defined(CONFIG_MSTAR_SDMMC1_HOTP)
    #if defined(CONFIG_MSTAR_SDMMC1_GPIO_PM10)
        #define REG_GPIO1_PM           REG_GPIO_PM10
        #define BIT_GPIO1_IN           BIT_GPIO10_IN
        #define BIT_GPIO1_FIQ_MASK     BIT_GPIO10_FIQ_MASK
        #define BIT_GPIO1_FIQ_CLR      BIT_GPIO10_FIQ_CLR
        #define BIT_GPIO1_FIQ_POL      BIT_GPIO10_FIQ_POL
        #define BIT_GPIO1_FIQ_FINAL    BIT_GPIO10_FIQ_FINAL
    #elif defined(CONFIG_MSTAR_SDMMC1_GPIO_PM16)
        #define REG_GPIO1_PM           REG_GPIO_PM16
        #define BIT_GPIO1_IN           BIT_GPIO16_IN
        #define BIT_GPIO1_FIQ_MASK     BIT_GPIO16_FIQ_MASK
        #define BIT_GPIO1_FIQ_CLR      BIT_GPIO16_FIQ_CLR
        #define BIT_GPIO1_FIQ_POL      BIT_GPIO16_FIQ_POL
        #define BIT_GPIO1_FIQ_FINAL    BIT_GPIO16_FIQ_FINAL
    #elif defined(CONFIG_MSTAR_SDMMC1_GPIO_PADSPI2DI)

    #else
        #error  "Please select MSTAR_SDMMC1_GPIO_PM10 or 16!"
    #endif
#endif

#if defined(CONFIG_MSTAR_SDMMC2_HOTP)
#if defined(CONFIG_MSTAR_SDMMC2_GPIO_PM10)
#define REG_GPIO2_PM           REG_GPIO_PM10

#define BIT_GPIO2_IN           BIT_GPIO10_IN
#define BIT_GPIO2_FIQ_MASK     BIT_GPIO10_FIQ_MASK
#define BIT_GPIO2_FIQ_CLR      BIT_GPIO10_FIQ_CLR
#define BIT_GPIO2_FIQ_POL      BIT_GPIO10_FIQ_POL
#define BIT_GPIO2_FIQ_FINAL    BIT_GPIO10_FIQ_FINAL
#elif defined(CONFIG_MSTAR_SDMMC2_GPIO_PM16)
#define REG_GPIO2_PM           REG_GPIO_PM16

#define BIT_GPIO2_IN           BIT_GPIO16_IN
#define BIT_GPIO2_FIQ_MASK     BIT_GPIO16_FIQ_MASK
#define BIT_GPIO2_FIQ_CLR      BIT_GPIO16_FIQ_CLR
#define BIT_GPIO2_FIQ_POL      BIT_GPIO16_FIQ_POL
#define BIT_GPIO2_FIQ_FINAL    BIT_GPIO16_FIQ_FINAL
#elif defined(CONFIG_MSTAR_SDMMC2_GPIO_PADSPI2DI)
#else
#error  "Please select MSTAR_SDMMC2_GPIO_PM10 or 16!"
#endif
#endif

#define WAIT_EMMC_D0_TIME      3000

bool ms_sdmmc_wait_d0_for_emmc(void) // currently wait D0 by sd driver itself
{
#if (D_PROJECT == D_PROJECT__EDISON)

    return TRUE;
#else
    IPEmType eIP      = ge_IPSlot[0];
	PADEmType ePAD    = ge_PADSlot[0];

    if (_GetCardDetect(0)) //if sd exist
    {
        return (Hal_CARD_Wait_D0_ForEmmc(eIP, ePAD));
    }
	else
		return TRUE;
#endif		
}


bool ms_sdmmc_card_chg(unsigned int slotNo)
{
#if (D_PROJECT == D_PROJECT__EDISON)
        return FALSE; // edison don't have share pin issue
 #else
        if (gb_card_chg[slotNo])
        {
            gb_card_chg[slotNo] = FALSE;
            return TRUE;
        }
        return FALSE;
  #endif
}
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitIPOnce
 *     @author jeremy.wang (2012/6/7)
 * Desc:  IP Once Setting , it's about platform setting.
 *
 * @param eIP :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_IPOnceSetting(IPEmType eIP)
{

}

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
	if (CARD_REG(NAND_CONFIG) & REG_CHK_NAND_PAD)
		return TRUE;

	#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
	return (mstar_mci_exit_checkdone_ForSD());
    #else
    return TRUE;
	#endif

	#endif
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitSetting
 *     @author jeremy.wang (2011/12/1)
 * Desc: IP Begin Setting before every operation, it's about platform setting.
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_IPBeginSetting(IPEmType eIP, PADEmType ePAD)
{
    #if (EN_MIU_PATCH_FCIE2MI_LAST)
	CARD_REG16_ON(A_NC_REORDER_REG(eIP), R_MIU_WP_LAST2_SEL);
    #endif
    gSdBak = CARD_REG_R16(FCIE_MACRO_BYPASS);
	gEmmcBak = CARD_REG_R16(TOP_EMMC_CFG);
	gEmmcBak2 = CARD_REG(A_EMMC_BOOTCFG_REG(eIP));
    CARD_REG16_OFF(FCIE_MACRO_BYPASS,SD_USE_BYPASS);
	CARD_REG16_ON(FCIE_MACRO_BYPASS,FCIE2MACRO_SD_BYPASS);
	CARD_REG16_OFF(TOP_EMMC_CFG, REG_EMMC_CFG_CLR);
	CARD_REG_W16(A_EMMC_BOOTCFG_REG(eIP), 0x0400);

	if(eIP == EV_IP_FCIE1)
    {
       switch((U8_T)ePAD)
       {
          case EV_PAD1:
		  	 CARD_REG16_ON(NAND_CONFIG, REG_SD0_CONFIG_SET1);
		  	 break;
          case EV_PAD2:
		  	 CARD_REG16_ON(NAND_CONFIG, REG_SD0_CONFIG_SET2);
		  	 break;
		  case EV_PAD3:
		  	 CARD_REG16_ON(NAND_CONFIG, REG_SD1_CONFIG_SET1);
		  	 break;
          case EV_PAD4:
		  	 CARD_REG16_ON(NAND_CONFIG, REG_SD1_CONFIG_SET2);
		  	 break;
		  case EV_PAD5:
		  	 CARD_REG16_ON(NAND_CONFIG, REG_SD0_CONFIG2_SET1);
		  	 break;
		  case EV_PAD6:
		  	 CARD_REG16_ON(NAND_CONFIG, REG_SD0_CONFIG2_SET2);
		  	 break;
		  case EV_PAD7:
		  	 CARD_REG16_ON(NAND_CONFIG, REG_SD1_CONFIG2_SET1);
		  	 break;
		  case EV_PAD8:
		  	 CARD_REG16_ON(NAND_CONFIG, REG_SD1_CONFIG2_SET2);
		  	 break;
       }
    }
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_IPEndSetting
 *     @author jeremy.wang (2012/5/3)
 * Desc: IP End Setting after every operation, it's about platform setting.
 *
 * @param eIP :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_IPEndSetting(IPEmType eIP, PADEmType ePAD)
{
    #if (EN_MIU_PATCH_FCIE2MI_LAST)
	CARD_REG16_OFF(A_NC_REORDER_REG(eIP), R_MIU_WP_LAST2_SEL);
    #endif
    CARD_REG_W16(FCIE_MACRO_BYPASS, gSdBak);
	CARD_REG_W16(TOP_EMMC_CFG, gEmmcBak);
	CARD_REG_W16(A_EMMC_BOOTCFG_REG(eIP), gEmmcBak2);
	CARD_REG(A_MIE_INT_EN_REG(eIP)) = 0;
	CARD_REG_W16(A_MIE_EVENT_REG(eIP), CARD_REG(A_MIE_EVENT_REG(eIP)));
	if(eIP == EV_IP_FCIE1)
    {
       switch((U8_T)ePAD)
       {
          case EV_PAD1:
		  	 CARD_REG16_OFF(NAND_CONFIG, REG_SD0_CONFIG_SET1);
		  	 break;
          case EV_PAD2:
		  	 CARD_REG16_OFF(NAND_CONFIG, REG_SD0_CONFIG_SET2);
		  	 break;
		  case EV_PAD3:
		  	 CARD_REG16_OFF(NAND_CONFIG, REG_SD1_CONFIG_SET1);
		  	 break;
          case EV_PAD4:
		  	 CARD_REG16_OFF(NAND_CONFIG, REG_SD1_CONFIG_SET2);
		  	 break;
		  case EV_PAD5:
		  	 CARD_REG16_OFF(NAND_CONFIG, REG_SD0_CONFIG2_SET1);
		  	 break;
		  case EV_PAD6:
		  	 CARD_REG16_OFF(NAND_CONFIG, REG_SD0_CONFIG2_SET2);
		  	 break;
		  case EV_PAD7:
		  	 CARD_REG16_OFF(NAND_CONFIG, REG_SD1_CONFIG2_SET1);
		  	 break;
		  case EV_PAD8:
		  	 CARD_REG16_OFF(NAND_CONFIG, REG_SD1_CONFIG2_SET2);
		  	 break;
       }
    }
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_Wait_D0_ForEmmc
 *     @author benson.hsiao (2012/7/31)
 * Desc:  Wait SD D0 to High for EMMC.
 *
 * @param eIP :
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_Wait_D0_ForEmmc(IPEmType eIP, PADEmType ePAD)
{
	U32 u32_Cnt=0;

    Hal_CARD_IPBeginSetting(eIP, ePAD);
  	CARD_REG16_ON(A_SD_MODE_REG(eIP), R_SD_CLK_EN); // output clock
    while((CARD_REG(A_SD_STS_REG(eIP)) & R_SD_DAT0)==0) /* card busy */
    {
	    if ((++u32_Cnt) >= SD_DELAY_1s*3)
	    {
		    CARD_REG16_OFF(A_SD_MODE_REG(eIP), R_SD_CLK_EN); // stop clock
		    Hal_CARD_IPEndSetting(eIP, ePAD);
		    printk(KERN_WARNING "[sdmmc]SD D0 busy!!\n");
		    return FALSE;
	    }

        Hal_Timer_uDelay(1);

		if(0 == (u32_Cnt%SD_DELAY_100us))
		{
		    schedule_timeout(1);
		    u32_Cnt += SD_DELAY_1ms;
		}
    }

    CARD_REG16_OFF(A_SD_MODE_REG(eIP), R_SD_CLK_EN); // stop clock
    Hal_CARD_IPEndSetting(eIP, ePAD);

	return TRUE;
}

// Switch PAD
//------------------------------------------------------------------------------------------------

#define U16  unsigned short
#define FCIE_RIU_R16(addr)       *((volatile U16*)(addr))
#define FCIE_RIU_W16(addr,value) *((volatile U16*)(addr)) = (value)
#define FCIE_RIU_16_ON(addr,value) FCIE_RIU_W16((addr), FCIE_RIU_R16(addr)|(value))
#define FCIE_RIU_16_OF(addr,value) FCIE_RIU_W16((addr), FCIE_RIU_R16(addr)&(~(value)))

#define R_08h_EMMC_DRV              IO_ADDRESS(0x1F203C20)
#define R_09h_PCM_PE_1              IO_ADDRESS(0x1F203C24)
#define R_0Ah_PCM_PE_2              IO_ADDRESS(0x1F203C28)
#define R_0Bh_PCM_PE_3              IO_ADDRESS(0x1F203C2C)
#define R_0Ch_NAND_DRV              IO_ADDRESS(0x1F203C30)
#define R_10h_FCIE2MACRO_SD_BYPASS  IO_ADDRESS(0x1F203C40)
#define R_12h_TEST_MODE             IO_ADDRESS(0x1F203C48)
#define R_1Dh_DUMMY                 IO_ADDRESS(0x1F203C74)
#define R_40h_SD_USE_BYPASS         IO_ADDRESS(0x1F203D00)
#define R_50h_ALL_PAIN              IO_ADDRESS(0x1F203D40)
#define R_5Ah_SD_CONFIG             IO_ADDRESS(0x1F203D68)
#define R_64h_CI_PCM_CFG            IO_ADDRESS(0x1F202D90)
#define R_6Eh_EMMC_CONFIG           IO_ADDRESS(0x1F203DB8)
#define R_6Fh_NAND_CS1_n_MODE       IO_ADDRESS(0x1F203DBC)

#define R_CLKGEN_FCIE               IO_ADDRESS(0x1F201790)

//***********************************************************************************************************
// PAD Setting for Card Platform
//***********************************************************************************************************
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
#if (D_PROJECT == D_PROJECT__EDISON)

    FCIE_RIU_16_OF(R_08h_EMMC_DRV, 0x0007);
    FCIE_RIU_16_ON(R_09h_PCM_PE_1, 0x00FF);
    FCIE_RIU_16_ON(R_0Ah_PCM_PE_2, 0x00FF);
    FCIE_RIU_16_ON(R_0Bh_PCM_PE_3, 0x0002);
    FCIE_RIU_16_OF(R_0Ch_NAND_DRV, 0xFF00);
    FCIE_RIU_16_ON(R_10h_FCIE2MACRO_SD_BYPASS, 0x0100);
    FCIE_RIU_16_OF(R_12h_TEST_MODE, 0x0077);
    //FCIE_RIU_16_OF(R_1Dh_DUMMY)//don't care
    FCIE_RIU_16_OF(R_40h_SD_USE_BYPASS, 0x0001);
    FCIE_RIU_16_OF(R_50h_ALL_PAIN, 0x0001);

    FCIE_RIU_16_OF(R_5Ah_SD_CONFIG, 0x0F00);
    FCIE_RIU_16_ON(R_5Ah_SD_CONFIG, 0x0100); // sd_config = 1

    FCIE_RIU_16_OF(R_64h_CI_PCM_CFG, 0x0011);
    FCIE_RIU_16_OF(R_6Eh_EMMC_CONFIG, 0x00C0);
    FCIE_RIU_16_OF(R_6Fh_NAND_CS1_n_MODE, 0x00E0);
#endif
    //printk("R_CLKGEN_FCIE = 0x%04X\r\n", FCIE_RIU_R16(R_CLKGEN_FCIE));
    //FCIE_RIU_W16(R_CLKGEN_FCIE, 0x0034); // 300K
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
    SET_CARD_PORT(eIP, ePort);
#if 0
    if(eIP == EV_IP_FCIE1)
    {

    }
#endif
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
#if 0
    if(ePAD == EV_PAD1) //Pad RF
    {
    }
    else if(ePAD == EV_PAD2) //Pad MPIF
    {
    }
    else if(ePAD == EV_PAD3) //PAD_SD
    {
        if(ePinPull ==EV_PULLDOWN)
        {
            if(ePin ==EV_SD_CMD_DAT0_3)
            {

            }
        }
        else if(ePinPull == EV_PULLUP)
        {
            if(ePin ==EV_SD_CMD_DAT0_3)
            {

            }
        }
    }
    else if(ePAD == EV_PAD4) //PAD_ROW
    {

    }
    else if(ePAD == EV_PAD5) //PAD_HSL
    {
        if(ePinPull ==EV_PULLDOWN)
        {
            if(ePin ==EV_SD_CMD_DAT0_3)
            {

            }

        }
        else if(ePinPull == EV_PULLUP)
        {
            if(ePin ==EV_SD_CMD_DAT0_3)
            {

            }
        }
    }
#endif
}


//***********************************************************************************************************
// Clock Setting for Card Platform
//***********************************************************************************************************

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
        CARD_REG16_OFF(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64),(BIT02|BIT03|BIT04|BIT05|BIT06));
        switch(u32ClkFromIPSet)
        {
            case CLK_F:      //48000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), SDCLK_48M);
                break;
            case CLK_E:      //43200KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), SDCLK_43_2M);
                break;
            case CLK_D:      //40000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), SDCLK_40M);
                break;
            case CLK_C:      //36000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), SDCLK_36M);
                break;
            case CLK_B:      //32000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), SDCLK_32M);
                break;
            case CLK_A:      //27000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), SDCLK_27M);
                break;
            case CLK_9:      //20000KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), SDCLK_20M);
                break;
            case CLK_8:      //12000KHz
                break;
            case CLK_7:      //300KHz
                CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), SDCLK_300K);
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
		CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x64), BIT06);
    }
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_FindClockSetting
 *     @author jeremy.wang (2012/5/22)
 * Desc: Find Real Clock Level Setting by Reference Clock
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u32ReffClk : Reference Clock Value
 * @param u8PassLevel : Pass Level to Clock Speed
 * @param u8DownLevel : Down Level to Decrease Clock Speed
 *
 * @return U32_T  : Real Clock
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_CARD_FindClockSetting(IPEmType eIP, U32_T u32ReffClk, U8_T u8PassLevel, U8_T u8DownLevel)
{
    U8_T  u8LV = u8PassLevel;
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


//***********************************************************************************************************
// Power Setting for Card Platform
//***********************************************************************************************************

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


}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PowerOn
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power on Card Power
 *
 * @param ePAD : PAD
 * @param u16DelayMs : Delay ms for Stable Power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PowerOn(PADEmType ePAD, U16_T u16DelayMs)
{
    if(ePAD==EV_PAD3)
    {

    }
    else if(ePAD==EV_PAD5)  //PAD_HSL
    {

    }

    //Hal_Timer_uDelay(u16DelayMs);

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

    if(ePAD==EV_PAD3)
    {
        //EVB board
        //CARD_REG16_OFF(GET_CARD_REG_ADDR(A_PMU_BANK,0x1C), BIT00);        //reg_ldoper2_a_en
        //DrvPmDevPowerCtl(PM_DEV_SD, PM_PWR_REQ_OFF);
    }
    else if(ePAD==EV_PAD5)  //PAD_HSL
    {

    }

    Hal_Timer_uDelay(u16DelayMs);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetHighSpeed
 *     @author jeremy.wang (2012/3/29)
 * Desc: Set High Speed registers in other bank for SDR/DDR
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : Enable or Disable
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetHighSpeed(IPEmType eIP, BOOL_T bEnable)
{
    gb_SD_Mode_HighSpeed[eIP] = bEnable;
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_GetGPIONum
 *     @author jeremy.wang (2012/5/22)
 * Desc:
 *
 * @param eGPIO :
 *
 * @return U32_T  :
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_CARD_GetGPIONum(GPIOEmType eGPIO)
{
    S32_T s32GPIO = -1;

    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
    }
    else if( eGPIO==EV_GPIO2)
    {
    }

    if(s32GPIO>0)
        return (U32_T)s32GPIO;
    else
        return 0;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitGPIO
 *     @author jeremy.wang (2012/5/22)
 * Desc:
 *
 * @param eGPIO :
 * @param bEnable :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_InitGPIO(GPIOEmType eGPIO, BOOL_T bEnable)
{
    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
        #if defined(CONFIG_MSTAR_SDMMC1_GPIO_PADSPI2DI)
		    CARD_REG16_ON(REG_CHIPTOP_DUMMY, BIT00);
		    printk("FIQ MASK:0x%x\n", CARD_REG_R16(REG_EXT_GPIO_MASK));
        #else
            if(bEnable)
            {
                CARD_REG16_OFF(REG_GPIO1_PM, BIT_GPIO1_FIQ_MASK);
            }
            else
            {
                CARD_REG16_ON(REG_GPIO1_PM, BIT_GPIO1_FIQ_MASK);
            }
        #endif
    }
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_GetGPIOState
 *     @author jeremy.wang (2012/5/22)
 * Desc:
 *
 * @param eGPIO :
 *
 * @return BOOL_T  :
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_GetGPIOState(GPIOEmType eGPIO)
{
    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
#if defined(CONFIG_MSTAR_SDMMC1_GPIO_PADSPI2DI)
            if ( CARD_REG_R16(REG_EXT_GPIO_CONFIG) & BIT11 )
                return (FALSE);
            else
                return (TRUE);
#else
        if ( CARD_REG_R16(REG_GPIO1_PM) & BIT_GPIO1_IN )        
                return (FALSE);
            else
           return (TRUE);
 #endif
    }

    return (FALSE);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetGPIOIntAttr
 *     @author jeremy.wang (2012/5/22)
 * Desc:
 *
 * @param eGPIO :
 * @param eGPIOOPT :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetGPIOIntAttr(GPIOEmType eGPIO, GPIOOptEmType eGPIOOPT)
{
    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
        if(eGPIOOPT==EV_GPIO_OPT1)
        {
        }
        else if((eGPIOOPT==EV_GPIO_OPT2))
        {
        }
        else if((eGPIOOPT==EV_GPIO_OPT3))
        {
            #if defined(CONFIG_MSTAR_SDMMC1_GPIO_PADSPI2DI)
			    CARD_REG16_OFF(REG_EXT_GPIO_POL, BIT01);
			
		#elif  defined(CONFIG_MSTAR_SDMMC1_GPIO_PM16)||defined(CONFIG_MSTAR_SDMMC1_GPIO_PM10)

                     CARD_REG16_OFF(REG_GPIO1_PM, BIT_GPIO1_FIQ_POL);
	     #endif
        }
        else if((eGPIOOPT==EV_GPIO_OPT4))
        {
            #if defined(CONFIG_MSTAR_SDMMC1_GPIO_PADSPI2DI)
                CARD_REG16_ON(REG_EXT_GPIO_POL, BIT01);
	  #elif  defined(CONFIG_MSTAR_SDMMC1_GPIO_PM16)||defined(CONFIG_MSTAR_SDMMC1_GPIO_PM10)
	     	//{

               CARD_REG16_ON(REG_GPIO1_PM, BIT_GPIO1_FIQ_POL);
	#endif
        }
    }

}




/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_GPIOIntFilter
 *     @author jeremy.wang (2012/5/9)
 * Desc:
 *
 * @param eGPIO :
 *
 * @return BOOL_T  :
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_GPIOIntFilter(GPIOEmType eGPIO)
{
    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
        #if defined(CONFIG_MSTAR_SDMMC1_GPIO_PADSPI2DI)

	#else
         if ( CARD_REG_R16(REG_GPIO1_PM) & BIT_GPIO1_FIQ_MASK )
	         return (FALSE);
        if (( CARD_REG_R16(REG_GPIO1_PM) & BIT_GPIO1_FIQ_FINAL ) == 0)
		         return (FALSE);
          CARD_REG16_ON(REG_GPIO1_PM, BIT_GPIO1_FIQ_CLR);	//clear FIQ_FINAL
        	
        #endif

	    return (TRUE);
    }

    return (FALSE);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_TransMIUAddr
 *     @author jeremy.wang (2012/6/7)
 * Desc:
 *
 * @param eIP
 * @param u32Addr :
 * @return U32_T  :
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_CARD_TransMIUAddr(IPEmType eIP, U32_T u32Addr)
{
    #if (D_PRODUCT == D_TV)
    if( u32Addr >= MSTAR_MIU1_BUS_BASE)
    {
        u32Addr -= MSTAR_MIU1_BUS_BASE;
        CARD_REG16_ON(A_MIU_DMA1_REG(eIP), BIT15);
    }
    else
    {
        u32Addr -= MSTAR_MIU0_BUS_BASE;
        CARD_REG16_OFF(A_MIU_DMA1_REG(eIP), BIT15);
    }
    #endif
    return u32Addr;
}





















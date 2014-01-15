//=======================================================================
//  MStar Semiconductor - Unified Nand Flash Driver
//
//  drvNAND_platform.c - Storage Team, 2009/08/20
//
//  Design Notes: defines common platform-dependent functions.
//
//    1. 2009/08/25 - support C5 eCos platform
//
//=======================================================================
#include "drvNAND.h"
#include "chip_int.h"
#include "chip_setup.h"
#if defined(CONFIG_ARM) // ARM: AMBER3, AGATE
#include <mstar/mstar_chip.h>
#endif
//=============================================================
//=============================================================
#if (defined(NAND_DRV_TV_LINUX)&&NAND_DRV_TV_LINUX)

NAND_DRIVER sg_NandDrv;

#if defined(CONFIG_MSTAR_URANUS4) || defined(CONFIG_MSTAR_AMBER3) || defined(CONFIG_MSTAR_EAGLE) || defined(CONFIG_MSTAR_AGATE)\
			|| defined (CONFIG_MSTAR_EINSTEIN)
static u16 fciepad = 0;
#endif

U32 nand_hw_timer_delay(U32 u32usTick)
{
    #if 0	// Use PIU timer

	U32 u32HWTimer = 0;
	volatile U16 u16TimerLow = 0;
	volatile U16 u16TimerHigh = 0;

	// reset HW timer
	REG_WRITE_UINT16(TIMER0_MAX_LOW, 0xFFFF);
	REG_WRITE_UINT16(TIMER0_MAX_HIGH, 0xFFFF);
	REG_WRITE_UINT16(TIMER0_ENABLE, 0);

	// start HW timer
	REG_SET_BITS_UINT16(TIMER0_ENABLE, 0x0001);

	while( u32HWTimer < 12*u32usTick ) // wait for u32usTick micro seconds
	{
		REG_READ_UINT16(TIMER0_CAP_LOW, u16TimerLow);
		REG_READ_UINT16(TIMER0_CAP_HIGH, u16TimerHigh);

		u32HWTimer = (u16TimerHigh<<16) | u16TimerLow;
	}

	REG_WRITE_UINT16(TIMER0_ENABLE, 0);

    #else	// Use kernel udelay

	udelay(u32usTick);

    #endif

	return u32usTick+1;
}

U32 nand_pads_init(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

#if defined (CONFIG_MSTAR_AGATE)
	U16 u16_NandMode;

	#if defined(reg_fcie_data_strength)
	REG_SET_BITS_UINT16(reg_fcie_data_strength,0xff);
	REG_SET_BITS_UINT16(reg_fcie_control_strength,0xff<<6);
	#endif

	REG_READ_UINT16(reg_nf_en, u16_NandMode);
	if( (BIT6)==(u16_NandMode & REG_NAND_MODE_MASK))
	{
		REG_SET_BITS_UINT16(reg_pcmd_pe, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
	}
	else if( (BIT7)==(u16_NandMode & REG_NAND_MODE_MASK))
	{
		REG_SET_BITS_UINT16(reg_pcma_pe, BIT8|BIT9|BIT10|BIT11|BIT12|BIT13|BIT14|BIT15);
	}
#endif
#if defined(CONFIG_MSTAR_EAGLE)
	U16 u16_NandMode;

	REG_WRITE_UINT16(NC_PATH_CTL, 0x20);

	#if defined(reg_fcie_data_strength)
	REG_SET_BITS_UINT16(reg_fcie_data_strength,(0xff<<8));
	REG_SET_BITS_UINT16(reg_fcie_control_strength1,(0xff<<8));
	REG_SET_BITS_UINT16(reg_fcie_control_strength2, BIT0);
	#endif

	/*Check NAND Mode for PE setting*/
	REG_READ_UINT16(reg_nf_en, u16_NandMode);

	u16_NandMode &= REG_NAND_MODE_MASK;

	if(NAND_MODE3 == u16_NandMode || NAND_MODE1 == u16_NandMode)
	{
		//set nand mode to mode 3
		REG_READ_UINT16(reg_nf_en, u16_NandMode);
		u16_NandMode &= ~REG_NAND_MODE_MASK;
		u16_NandMode |= NAND_MODE3;
		REG_WRITE_UINT16(reg_nf_en, u16_NandMode);

		REG_SET_BITS_UINT16(REG_PCM_D_PE, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
	}
	else if(NAND_MODE2 == u16_NandMode)
	{
		REG_SET_BITS_UINT16(REG_PCM_A_PE, BIT8|BIT9|BIT10|BIT11|BIT12|BIT13|BIT14|BIT15);
	}
	else if(NAND_MODE5 == u16_NandMode)
	{
		REG_SET_BITS_UINT16(REG_PCM2_CD_N, BIT9);
	}
#endif

	//Read CEZ Configure Setting from CEXZ GPIO.
#if defined(CONFIG_MSTAR_EAGLE) || \
	defined(CONFIG_MSTAR_AGATE) || \
	defined(CONFIG_MSTAR_EDISON) || \
	defined(CONFIG_MSTAR_EIFFEL) || \
	defined(CONFIG_MSTAR_EINSTEIN)
	if((REG(REG_NAND_CS1_EN) & BIT_NAND_CS1_EN) == BIT_NAND_CS1_EN)
	{
		pNandDrv->u16_Reg40_Signal =
			(BIT_NC_CE1Z | BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
			~(BIT_NC_CHK_RB_EDGEn);
	}
	else
	{
		pNandDrv->u16_Reg40_Signal =
			(BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
			~(BIT_NC_CHK_RB_EDGEn | BIT_NC_CE_SEL_MASK);
	}
#else
	pNandDrv->u16_Reg40_Signal =
			(BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
			~(BIT_NC_CHK_RB_EDGEn | BIT_NC_CE_SEL_MASK);
#endif

    return UNFD_ST_SUCCESS;
}

#if defined(FCIE4_DDR) && FCIE4_DDR
U32 nand_check_DDR_pad(void)
{
    //check nand mode for wether this mode supports DDR NAND
    U16 u16_NandMode;
    REG_READ_UINT16(reg_nf_en, u16_NandMode);

	u16_NandMode &= REG_NAND_MODE_MASK;

    #if defined(CONFIG_MSTAR_EAGLE)
    if(u16_NandMode != NAND_MODE1 && u16_NandMode != NAND_MODE3)
    {
        nand_debug(0,1,"Nand pad type does not support DDR NAND\n");
        return UNFD_ST_ERR_PAD_UNSUPPORT_DDR_NAND;
    }
	#elif defined (CONFIG_MSTAR_EISTEIN)
	//@FIX ME
    #endif
    return UNFD_ST_SUCCESS;
}
#endif

U32 nand_pads_switch(U32 u32EnableFCIE)
{
	#if defined(CONFIG_MSTAR_TITANIA4) || defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_TITANIA13)

	REG_SET_BITS_UINT16(reg_nf_en, BIT4);

	#elif defined(CONFIG_MSTAR_JANUS)

	REG_SET_BITS_UINT16(reg_nf_en, BIT0);

	#elif defined(CONFIG_MSTAR_KRONUS) || defined(CONFIG_MSTAR_KAISERIN)

	// Let sboot to determine the pad, pcmcia driver will recover it after it finishes job

	REG_CLR_BITS_UINT16(NC_TEST_MODE, BIT5);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "NC_TEST_MODE(%08X)=%04X\n", NC_TEST_MODE, REG(NC_TEST_MODE));

	REG_CLR_BITS_UINT16(reg_boot_from_pf, BIT10);
	nand_debug(UNFD_DEBUG_LEVEL_MEDIUM, 1, "reg_boot_from_pf(%08X)=%04X\n", reg_boot_from_pf, REG(reg_boot_from_pf));

	#elif defined(CONFIG_MSTAR_TITANIA8) || \
          defined(CONFIG_MSTAR_TITANIA12) || \
		  defined(CONFIG_MSTAR_JANUS2) || \
          defined(CONFIG_MSTAR_AMBER1) || \
          defined(CONFIG_MSTAR_AMBER7) || \
          defined(CONFIG_MSTAR_AMETHYST) || \
          defined(CONFIG_MSTAR_AMBER5)

	// Let sboot to determine the pad, pcmcia driver will recover it after it finishes job

    #elif defined(CONFIG_MSTAR_URANUS4) || defined(CONFIG_MSTAR_AMBER3)

    // Use for restoring pad setting when NAND shares pad with SD
    if( fciepad )
    {
        HAL_WRITE_UINT16(NC_PATH_CTL, BIT_NC_EN);
        REG_SET_BITS_UINT16(reg_nf_en, fciepad);
    }
    else
    {
        fciepad = REG(reg_nf_en) & (BIT7|BIT6);
    }
	#elif defined(CONFIG_MSTAR_AGATE)
	{
		NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
		U16 u16_NandMode;

		if( fciepad )
		{
			REG_WRITE_UINT16(NC_PATH_CTL, BIT_NC_EN);
			REG_SET_BITS_UINT16(reg_nf_en, fciepad);
		}
		else
		{
			fciepad = REG(reg_nf_en) & REG_NAND_MODE_MASK;
		}

		pNandDrv->u16_Reg58_DDRCtrl &= ~(BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO);

		if(NAND_PAD_BYPASS_MODE == u32EnableFCIE)
		{
			REG_SET_BITS_UINT16(NC_REG_2Fh, BIT11);
			//for SDR ENABLE BYPASS MODE
			REG_SET_BITS_UINT16(reg_fcie2macro_sd_bypass, BIT8);
			REG_SET_BITS_UINT16(reg_clk4x_div_en, BIT5);

		    NC_DQS_PULL_H();
		}
		else if(NAND_PAD_TOGGLE_MODE == u32EnableFCIE)
		{
		    REG_CLR_BITS_UINT16(NC_REG_2Fh, BIT11);
			//for DDR disable BYPASS MODE
			REG_CLR_BITS_UINT16(reg_fcie2macro_sd_bypass, BIT8);
			REG_CLR_BITS_UINT16(reg_clk4x_div_en, BIT5);

		    NC_DQS_PULL_L();
		    pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_TOGGLE|BIT_SDR_DIN_FROM_MACRO);
		}
		else if(NAND_PAD_ONFI_SYNC_MODE == u32EnableFCIE)
		{
			REG_CLR_BITS_UINT16(NC_REG_2Fh, BIT11);
			//for DDR disable BYPASS MODE
			REG_CLR_BITS_UINT16(reg_fcie2macro_sd_bypass, BIT8);
			REG_CLR_BITS_UINT16(reg_clk4x_div_en, BIT5);

		    NC_DQS_PULL_L();
			pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_ONFI|BIT_SDR_DIN_FROM_MACRO);

		}
		REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);
	}
	#elif defined(CONFIG_MSTAR_EAGLE)
	{
		NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
		U16 u16_NandMode;

        #if defined(CONFIG_MSTAR_SDMMC)
		// Temp patch for someone driver overwrite the register (chiptop 0x1f)
        // The patch for SD card SD_CDZ issue (Eagle).
		REG_SET_BITS_UINT16(REG_CHIPTOP_0x1F, BIT0);
		#endif

		REG_CLR_BITS_UINT16(REG_EMMC_CONFIG, BIT14|BIT15);              //clean EMMC config 0x5D
		REG_CLR_BITS_UINT16(REG_PCM_CONFIG, REG_PCM_CONFIG_MASK);       //clean PCM config  0x4F
		REG_CLR_BITS_UINT16(REG_SD_CONFIG, REG_SD_MODE_MASK);           //clean SD Config   0x0B
		REG_CLR_BITS_UINT16(REG_LD_SPI_CONFIG, BIT2|BIT1|BIT0);         //clean SPI config  0x5A
		REG_CLR_BITS_UINT16(REG_CIAD_CONFIG, BIT0);                     //clean CIAD config 0x64
		REG_CLR_BITS_UINT16(REG_MCUJTAGMODE, BIT6|BIT7);                //clean MCU JTAG    0x03
		//REG_CLR_BITS_UINT16(REG_TS0_CONFIG, BIT10|BIT11);               //clean TSO config  0x51
		REG_CLR_BITS_UINT16(REG_BT656_CTRL, BIT14|BIT15);               //clean BT656 config    0x6F

		if( fciepad )
		{
			REG_WRITE_UINT16(NC_PATH_CTL, BIT_NC_EN);
			REG_SET_BITS_UINT16(reg_nf_en, fciepad);
		}
		else
		{
			fciepad = REG(reg_nf_en) & REG_NAND_MODE_MASK;
		}

		pNandDrv->u16_Reg58_DDRCtrl &= ~(BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO);
    	REG_CLR_BITS_UINT16(REG_SDR_BYPASS_MODE, BIT_SD_USE_BYPASS);
		//new patch for fcie2mi last
		REG_CLR_BITS_UINT16(FCIE_NC_REORDER, BIT14);

		if(NAND_PAD_BYPASS_MODE == u32EnableFCIE)
   	 	{
        	REG_SET_BITS_UINT16(NC_REG_2Fh, BIT11);
        	//for SDR ENABLE BYPASS MODE
        	REG_SET_BITS_UINT16(REG_SDR_BYPASS_MODE, BIT_FCIE2MACRO_SD_BYPASS);
        	NC_DQS_PULL_H();
    	}
		else if(NAND_PAD_TOGGLE_MODE == u32EnableFCIE)
    	{
        	REG_CLR_BITS_UINT16(NC_REG_2Fh, BIT11);
        	//for DDR disable BYPASS MODE
        	REG_CLR_BITS_UINT16(REG_SDR_BYPASS_MODE, BIT_FCIE2MACRO_SD_BYPASS);
        	NC_DQS_PULL_L();
			pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_TOGGLE|BIT_SDR_DIN_FROM_MACRO);

			//set nand mode to mode 3
			REG_READ_UINT16(reg_nf_en, u16_NandMode);
			u16_NandMode &= ~REG_NAND_MODE_MASK;
			u16_NandMode |= NAND_MODE3;
			REG_WRITE_UINT16(reg_nf_en, u16_NandMode);
		}
		else if(NAND_PAD_ONFI_SYNC_MODE == u32EnableFCIE)
		{
			REG_CLR_BITS_UINT16(NC_REG_2Fh, BIT11);
			//for DDR disable BYPASS MODE
			REG_CLR_BITS_UINT16(REG_SDR_BYPASS_MODE, BIT_FCIE2MACRO_SD_BYPASS);
			NC_DQS_PULL_L();
			pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_ONFI|BIT_SDR_DIN_FROM_MACRO);

			//set nand mode to mode 3
			REG_READ_UINT16(reg_nf_en, u16_NandMode);
			u16_NandMode &= ~REG_NAND_MODE_MASK;
			u16_NandMode |= NAND_MODE3;
			REG_WRITE_UINT16(reg_nf_en, u16_NandMode);
		}
		REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);
	}

    #elif defined(CONFIG_MSTAR_EMERALD)

    REG_SET_BITS_UINT16(NC_REG_PAD_SWITCH, BIT11);

    REG_CLR_BITS_UINT16(reg_sd_config, BIT11|BIT10|BIT9|BIT8);

    REG_SET_BITS_UINT16(reg_pcm_d_pe, 0xFF);
	REG_SET_BITS_UINT16(reg_pcm_a_pe, 0xFF);

	// Let sboot determine the reg_nand_mode

    #elif defined(CONFIG_MSTAR_EDISON)

    REG_SET_BITS_UINT16(NC_REG_PAD_SWITCH, BIT11);

    REG_CLR_BITS_UINT16(reg_sd_config, BIT11|BIT10|BIT9|BIT8);

    REG_SET_BITS_UINT16(reg_pcm_d_pe, 0xFF);
	REG_SET_BITS_UINT16(reg_pcm_a_pe, 0xFF);
    REG_SET_BITS_UINT16(reg_pcm2_cd_n_pe, BIT1);

    REG_CLR_BITS_UINT16(reg_sd_use_bypass, BIT0);

    REG_SET_BITS_UINT16(reg_fcie2macro_sd_bypass, BIT8);
    REG_CLR_BITS_UINT16(REG_EMMC_CONFIG, BIT7|BIT6);

    // Let sboot determine the reg_nand_mode

    #elif defined(CONFIG_MSTAR_EIFFEL)

    REG_SET_BITS_UINT16(NC_REG_PAD_SWITCH, BIT11);

    REG_CLR_BITS_UINT16(reg_sd_config, BIT11|BIT10|BIT9|BIT8);

    REG_SET_BITS_UINT16(reg_pcm_d_pe, 0xFF);
	REG_SET_BITS_UINT16(reg_pcm_a_pe, 0xFF);
    REG_SET_BITS_UINT16(reg_pcm2_cd_n_pe, BIT1);

    REG_CLR_BITS_UINT16(reg_sd_use_bypass, BIT0);

    REG_SET_BITS_UINT16(reg_fcie2macro_sd_bypass, BIT8);
    REG_CLR_BITS_UINT16(REG_EMMC_CONFIG, BIT7|BIT6);

    // Let sboot determine the reg_nand_mode
	#elif defined(CONFIG_MSTAR_EINSTEIN)
	#endif

	nand_debug(UNFD_DEBUG_LEVEL_LOW,1,"reg_nf_en(%08X)=%04X\n", reg_nf_en, REG(reg_nf_en));
	nand_debug(UNFD_DEBUG_LEVEL_LOW,1,"reg_allpad_in(%08X)=%04X\n", reg_allpad_in, REG(reg_allpad_in));

	return UNFD_ST_SUCCESS;
}

U32 nand_clock_setting(U32 u32ClkParam)
{
	REG_CLR_BITS_UINT16(reg_ckg_fcie, BIT6-1); // enable FCIE clk, set to lowest clk

	switch(u32ClkParam)
	{
		#if defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_JANUS2) || \
			defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_TITANIA13)

		case NFIE_CLK_5_4M:		break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_43M:		break;
		case NFIE_CLK_86M:		break;

		#if defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_TITANIA13)

		case NFIE_CLK_36M:		break;
		case NFIE_CLK_40M:		break;
		case NFIE_CLK_SSC:		break;

		#endif

		#elif defined(CONFIG_MSTAR_KRONUS)

		case NFIE_CLK_36M:		break;
		case NFIE_CLK_40M:		break;
		case NFIE_CLK_5_4M:		break;
		case NFIE_CLK_13_5M:	break;
		case NFIE_CLK_18M:		break;
		case NFIE_CLK_22_7M:	break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_43M:		break;
		case NFIE_CLK_54M:		break;
		case NFIE_CLK_72M:		break;
		case NFIE_CLK_86M:		break;

        #elif defined(CONFIG_MSTAR_KAISERIN)

         case NFIE_CLK_300K:
         case NFIE_CLK_750K:
         case NFIE_CLK_5_4M:
		 case NFIE_CLK_13M:
		 case NFIE_CLK_18M:
         case NFIE_CLK_22_7M:
		 case NFIE_CLK_27M:
         case NFIE_CLK_32M:
		 case NFIE_CLK_43_2M:
		 case NFIE_CLK_54M:
		 case NFIE_CLK_72M:
		 case NFIE_CLK_86M:
		 case NFIE_CLK_62M:
            break;

		#elif defined(CONFIG_MSTAR_AMBER1) || \
              defined(CONFIG_MSTAR_AMBER7)

		case NFIE_CLK_5_4M:		break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_36M:		break;
		case NFIE_CLK_40M:		break;
		case NFIE_CLK_43M:		break;
		case NFIE_CLK_54M:		break;
		case NFIE_CLK_62M:		break;
		case NFIE_CLK_72M:		break;
		case NFIE_CLK_80M:		break;
		case NFIE_CLK_86M:		break;
		case NFIE_CLK_SSC:		break;

        #elif defined(CONFIG_MSTAR_EMERALD)

		case NFIE_CLK_5_4M:		break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_36M:		break;
		case NFIE_CLK_40M:		break;
		case NFIE_CLK_43M:		break;
		case NFIE_CLK_54M:		break;
		case NFIE_CLK_62M:		break;
		case NFIE_CLK_72M:		break;
		case NFIE_CLK_80M:		break;
		case NFIE_CLK_86M:		break;
        case NFIE_CLK_300K:     break;
		case NFIE_CLK_48M:		break;

        #elif defined(CONFIG_MSTAR_EDISON)

		case NFIE_CLK_XTAL:		break;
        case NFIE_CLK_20M:		break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_36M:		break;
		case NFIE_CLK_40M:		break;
		case NFIE_CLK_43M:		break;
		case NFIE_CLK_54M:		break;
		case NFIE_CLK_62M:		break;
		case NFIE_CLK_72M:		break;
		case NFIE_CLK_80M:		break;
		case NFIE_CLK_86M:		break;
        case NFIE_CLK_300K:     break;
		case NFIE_CLK_48M:		break;

        #elif defined(CONFIG_MSTAR_EIFFEL)

		case NFIE_CLK_XTAL:		break;
        case NFIE_CLK_20M:		break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_36M:		break;
		case NFIE_CLK_40M:		break;
		case NFIE_CLK_43M:		break;
		case NFIE_CLK_54M:		break;
		case NFIE_CLK_62M:		break;
		case NFIE_CLK_72M:		break;
		case NFIE_CLK_80M:		break;
		case NFIE_CLK_86M:		break;
        case NFIE_CLK_300K:     break;
		case NFIE_CLK_48M:		break;

		#elif defined(CONFIG_MSTAR_AMBER5) || defined(CONFIG_MSTAR_AMBER3)

		case NFIE_CLK_300K:		break;
		case NFIE_CLK_20M:		break;
		case NFIE_CLK_24M:		break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_36M:		break;
		case NFIE_CLK_40M:		break;
		case NFIE_CLK_43M:		break;
		case NFIE_CLK_48M:		break;
		case NFIE_CLK_54M:		break;
		case NFIE_CLK_62M:		break;
		case NFIE_CLK_72M:		break;
		case NFIE_CLK_80M:		break;
		case NFIE_CLK_86M:		break;
		case NFIE_CLK_SSC:		break;

		#elif defined(CONFIG_MSTAR_EAGLE)
		case NFIE_CLK_20M:
		case NFIE_CLK_27M:
		case NFIE_CLK_36M:
		case NFIE_CLK_40M:
		case NFIE_CLK_48M:
			{
				/*Check if DDR NAND Enabled*/
				REG_CLR_BITS_UINT16(REG_CLK_EMMC, BIT9|BIT8);
				REG_CLR_BITS_UINT16(REG_CLK_EMMC, EMMC_CLK_MASK);

				if(u32ClkParam == NFIE_CLK_20M)
				{
					REG_SET_BITS_UINT16(REG_CLK_EMMC,  EMMC_CLK_80M);
				}
				else if(u32ClkParam == NFIE_CLK_27M)
				{
					REG_SET_BITS_UINT16(REG_CLK_EMMC,  EMMC_CLK_108M);
				}
				else if(u32ClkParam == NFIE_CLK_36M)
				{
					REG_SET_BITS_UINT16(REG_CLK_EMMC,  EMMC_CLK_144M);
				}
				else if(u32ClkParam == NFIE_CLK_40M)
				{
					REG_SET_BITS_UINT16(REG_CLK_EMMC,  EMMC_CLK_160M);
				}
				else if(u32ClkParam == NFIE_CLK_48M)
				{
					REG_SET_BITS_UINT16(REG_CLK_EMMC,  EMMC_CLK_192M);
				}
				else
					nand_die();
				nand_debug(UNFD_DEBUG_LEVEL_LOW, 0,"reg_clk_emmc(%08X)=%08X\n", REG_CLK_EMMC, REG(REG_CLK_EMMC));
				break;
			}

		case NFIE_CLK_32M:
		case NFIE_CLK_43M:
		case NFIE_CLK_54M:
		case NFIE_CLK_62M:
		case NFIE_CLK_72M:
		case NFIE_CLK_80M:
		case NFIE_CLK_86M:
		case NFIE_CLK_SSC:
		case NFIE_CLK_300K:
		case NFIE_CLK_XTAL:
			break;

		#elif defined(CONFIG_MSTAR_AMETHYST)

		case NFIE_CLK_XTAL:     break;
		case NFIE_CLK_300K:		break;
		case NFIE_CLK_12M:		break;
		case NFIE_CLK_20M:		break;
		case NFIE_CLK_24M:		break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_36M:		break;
		case NFIE_CLK_40M:		break;
		case NFIE_CLK_43_2M:	break;
		case NFIE_CLK_48M:		break;
		case NFIE_CLK_54M:		break;
		case NFIE_CLK_62M:		break;
		case NFIE_CLK_72M:		break;
		case NFIE_CLK_80M:		break;
		case NFIE_CLK_86M:		break;
		case NFIE_CLK_SSC:		break;

        #elif defined(CONFIG_MSTAR_AGATE)

		case NFIE_CLK_XTAL:
		case NFIE_CLK_20M:
		case NFIE_CLK_27M:
		case NFIE_CLK_36M:
		case NFIE_CLK_40M:
		case NFIE_CLK_54M:
		case NFIE_CLK_72M:
		case NFIE_CLK_108M:
		case NFIE_CLK_48M:
			REG_SET_BITS_UINT16(reg_clk4x_div_en, BIT1);
		break;
		case NFIE_CLK_32M:
		case NFIE_CLK_43M:
		case NFIE_CLK_62M:
		case NFIE_CLK_80M:
		case NFIE_CLK_86M:
		case NFIE_CLK_SSC:
		case NFIE_CLK_300K:
			REG_CLR_BITS_UINT16(reg_clk4x_div_en, BIT1);
		break;
		#elif defined(CONFIG_MSTAR_EINSTEIN)

		#else // defined(CONFIG_MSTAR_TITANIA4) || defined(CONFIG_MSTAR_JANUS) || defined(CONFIG_MSTAR_URANUS4)

		case NFIE_CLK_300K:		break;
		case NFIE_CLK_750K:		break;
		case NFIE_CLK_5_4M:		break;
		case NFIE_CLK_13_5M:	break;
		case NFIE_CLK_18M:		break;
		case NFIE_CLK_22_7M:	break;
		case NFIE_CLK_27M:		break;
		case NFIE_CLK_32M:		break;
		case NFIE_CLK_43M:		break;
		case NFIE_CLK_54M:		break;
		case NFIE_CLK_72M:		break;
		case NFIE_CLK_86M:		break;

		#endif

		default:
			nand_die();
	}

	REG_SET_BITS_UINT16(reg_ckg_fcie, u32ClkParam);
	nand_debug(UNFD_DEBUG_LEVEL_LOW, 1,"reg_ckg_fcie(%08X)=%08X\n", reg_ckg_fcie, REG(reg_ckg_fcie));

	return UNFD_ST_SUCCESS;
}


void nand_DumpPadClk(void)
{
    nand_debug(0, 1, "clk setting: \n");
    nand_debug(0, 1, "reg_ckg_fcie(0x%X):0x%x\n", reg_ckg_fcie, REG(reg_ckg_fcie));
#if defined(CONFIG_MSTAR_EAGLE)
    nand_debug(0, 1, "REG_CLK_EMMC(0x%X):0x%x\n", REG_CLK_EMMC, REG(REG_CLK_EMMC));
#endif

#if defined(CONFIG_MSTAR_AGATE)
    nand_debug(0, 1, "reg_clk4x_div_en(0x%X):0x%x\n", reg_clk4x_div_en, REG(reg_clk4x_div_en));
#endif

    nand_debug(0, 1, "pad setting: \n");
    //fcie pad register
    nand_debug(0, 1, "NC_REG_2Fh(0x%X):0x%x\n", NC_REG_2Fh, REG(NC_REG_2Fh));
    nand_debug(0, 1, "NC_DDR_CTRL(0x%X):0x%x\n", NC_DDR_CTRL, REG(NC_DDR_CTRL));
    //chiptop pad register
    nand_debug(0, 1, "reg_all_pad_in(0x%X):0x%x\n", reg_allpad_in, REG(reg_allpad_in));

    //platform depend reg_sd_use_bypass
#if defined(CONFIG_MSTAR_EAGLE)
    nand_debug(0, 1, "REG_SDR_BYPASS_MODE(0x%X):0x%x\n", REG_SDR_BYPASS_MODE, REG(REG_SDR_BYPASS_MODE));
	nand_debug(0, 1, "REG_SD_CONFIG(0x%X):0x%x\n", REG_SD_CONFIG, REG(REG_SD_CONFIG));
	nand_debug(0, 1, "REG_EMMC_CONFIG(0x%X):0x%x\n", REG_EMMC_CONFIG, REG(REG_EMMC_CONFIG));
	nand_debug(0, 1, "REG_PCM_CONFIG(0x%X):0x%x\n", REG_PCM_CONFIG, REG(REG_PCM_CONFIG));
	nand_debug(0, 1, "REG_LD_SPI_CONFIG(0x%X):0x%x\n", REG_LD_SPI_CONFIG, REG(REG_LD_SPI_CONFIG));
	nand_debug(0, 1, "REG_CIAD_CONFIG(0x%X):0x%x\n", REG_CIAD_CONFIG, REG(REG_CIAD_CONFIG));
	nand_debug(0, 1, "REG_MCUJTAGMODE(0x%X):0x%x\n", REG_MCUJTAGMODE, REG(REG_MCUJTAGMODE));
	nand_debug(0, 1, "REG_TS0_CONFIG(0x%X):0x%x\n", REG_TS0_CONFIG, REG(REG_TS0_CONFIG));
	nand_debug(0, 1, "REG_BT656_CTRL(0x%X):0x%x\n", REG_BT656_CTRL, REG(REG_BT656_CTRL));
#endif

#if defined(CONFIG_MSTAR_AGATE)
    nand_debug(0, 1, "reg_fcie2macro_sd_bypass(0x%X):0x%x\n", reg_fcie2macro_sd_bypass, REG(reg_fcie2macro_sd_bypass));
#endif

    #ifdef CONFIG_MSTAR_EDISON
    nand_debug(0, 1, " reg_pcm_d_pe(0x%08X): 0x%04X\n", reg_pcm_d_pe, REG(reg_pcm_d_pe));
    nand_debug(0, 1, " reg_pcm_a_pe(0x%08X): 0x%04X\n", reg_pcm_a_pe, REG(reg_pcm_a_pe));
    nand_debug(0, 1, " reg_pcm2_cd_n_pe(0x%08X): 0x%04X\n", reg_pcm2_cd_n_pe, REG(reg_pcm2_cd_n_pe));
    nand_debug(0, 1, " reg_fcie2macro_sd_bypass(0x%08X): 0x%04X\n", reg_fcie2macro_sd_bypass, REG(reg_fcie2macro_sd_bypass));
    nand_debug(0, 1, " reg_sd_use_bypass(0x%08X): 0x%04X\n", reg_sd_use_bypass, REG(reg_sd_use_bypass));
    nand_debug(0, 1, " reg_sd_config(0x%08X): 0x%04X\n", reg_sd_config, REG(reg_sd_config));
    nand_debug(0, 1, " REG_EMMC_CONFIG(0x%08X): 0x%04X\n", REG_EMMC_CONFIG, REG(REG_EMMC_CONFIG));
    #endif

#ifdef CONFIG_MSTAR_EIFFEL
    nand_debug(0, 1, " reg_pcm_d_pe(0x%08X): 0x%04X\n", reg_pcm_d_pe, REG(reg_pcm_d_pe));
    nand_debug(0, 1, " reg_pcm_a_pe(0x%08X): 0x%04X\n", reg_pcm_a_pe, REG(reg_pcm_a_pe));
    nand_debug(0, 1, " reg_pcm2_cd_n_pe(0x%08X): 0x%04X\n", reg_pcm2_cd_n_pe, REG(reg_pcm2_cd_n_pe));
    nand_debug(0, 1, " reg_fcie2macro_sd_bypass(0x%08X): 0x%04X\n", reg_fcie2macro_sd_bypass, REG(reg_fcie2macro_sd_bypass));
    nand_debug(0, 1, " reg_sd_use_bypass(0x%08X): 0x%04X\n", reg_sd_use_bypass, REG(reg_sd_use_bypass));
    nand_debug(0, 1, " reg_sd_config(0x%08X): 0x%04X\n", reg_sd_config, REG(reg_sd_config));
    nand_debug(0, 1, " REG_EMMC_CONFIG(0x%08X): 0x%04X\n", REG_EMMC_CONFIG, REG(REG_EMMC_CONFIG));
#endif
#ifdef CONFIG_MSTAR_EINSTEIN
    nand_debug(0, 1, " reg_pcm_d_pe(0x%08X): 0x%04X\n", reg_pcm_d_pe, REG(reg_pcm_d_pe));
    nand_debug(0, 1, " reg_pcm_a_pe(0x%08X): 0x%04X\n", reg_pcm_a_pe, REG(reg_pcm_a_pe));
    nand_debug(0, 1, " reg_pcm2_cd_n_pe(0x%08X): 0x%04X\n", reg_pcm2_cd_n_pe, REG(reg_pcm2_cd_n_pe));
    nand_debug(0, 1, " reg_fcie2macro_sd_bypass(0x%08X): 0x%04X\n", reg_fcie2macro_sd_bypass, REG(reg_fcie2macro_sd_bypass));
    nand_debug(0, 1, " reg_sd_use_bypass(0x%08X): 0x%04X\n", reg_sd_use_bypass, REG(reg_sd_use_bypass));
    nand_debug(0, 1, " reg_sd_config(0x%08X): 0x%04X\n", reg_sd_config, REG(reg_sd_config));
    nand_debug(0, 1, " REG_EMMC_CONFIG(0x%08X): 0x%04X\n", REG_EMMC_CONFIG, REG(REG_EMMC_CONFIG));
#endif
    nand_debug(0, 1, "reg_nf_en(0x%X):0x%x\n", reg_nf_en, REG(reg_nf_en));

}

#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
#define MAX(a,b) ((a) > (b) ? (a) : (b))

U32 nand_config_timing(U16 u16_1T)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U16 u16_DefaultTRR;
	U16 u16_DefaultTCS;
	U16 u16_DefaultTWW;
	U16 u16_DefaultRX40Cmd;
	U16 u16_DefaultRX40Adr;
	U16 u16_DefaultRX56;
	U16 u16_DefaultTADL;
	U16 u16_DefaultTCWAW;
	U16 u16_DefaultTCLHZ = 0;
	U16 u16_DefaultTWHR;
	U16 u16_Tmp, u16_Cnt;
	U16 u16_Tmp2, u16_Cnt2;


	#if defined(FCIE4_DDR) && FCIE4_DDR
	if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_ONFI)
	{
		u16_DefaultTRR = NC_ONFI_DEFAULT_TRR;
		u16_DefaultTCS = NC_ONFI_DEFAULT_TCS;
		u16_DefaultTWW = NC_ONFI_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_ONFI_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_ONFI_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_ONFI_DEFAULT_RX56;
		u16_DefaultTADL = NC_ONFI_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_ONFI_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_ONFI_DEFAULT_TWHR;
	}
	else if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_TOGGLE)
	{
		u16_DefaultTRR = NC_TOGGLE_DEFAULT_TRR;
		u16_DefaultTCS = NC_TOGGLE_DEFAULT_TCS;
		u16_DefaultTWW = NC_TOGGLE_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_TOGGLE_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_TOGGLE_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_TOGGLE_DEFAULT_RX56;
		u16_DefaultTADL = NC_TOGGLE_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_TOGGLE_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_TOGGLE_DEFAULT_TWHR;
	}
	else
	#endif
	{
		u16_DefaultTRR = NC_SDR_DEFAULT_TRR;
		u16_DefaultTCS = NC_SDR_DEFAULT_TCS;
		u16_DefaultTWW = NC_SDR_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_SDR_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_SDR_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_SDR_DEFAULT_RX56;
		u16_DefaultTADL = NC_SDR_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_SDR_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_SDR_DEFAULT_TWHR;
	}

	#if defined(NC_INST_DELAY) && NC_INST_DELAY
	// Check CMD_END
	u16_Tmp = MAX(pNandDrv->u16_tWHR, pNandDrv->u16_tCWAW);
	u16_Cnt = (u16_Tmp+u16_1T-1)/u16_1T;

	if(u16_DefaultRX40Cmd >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultRX40Cmd > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultRX40Cmd;

	// Check ADR_END
	u16_Tmp2 = MAX(MAX(pNandDrv->u16_tWHR, pNandDrv->u16_tADL), pNandDrv->u16_tCCS);
	u16_Cnt2 = (u16_Tmp2+u16_1T-1)/u16_1T;

	if(u16_DefaultRX40Adr >= u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultRX40Adr > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultRX40Adr;

	// get the max cnt
	u16_Cnt = MAX(u16_Cnt, u16_Cnt2);

	pNandDrv->u16_Reg40_Signal &= ~(0x00FF<<8);
	pNandDrv->u16_Reg40_Signal |= (u16_Cnt<<8);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg40_Signal =  %X\n",pNandDrv->u16_Reg40_Signal);
	#endif

	#if defined(NC_HWCMD_DELAY) && NC_HWCMD_DELAY
	u16_Cnt = (pNandDrv->u16_tRHW+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultRX56 >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultRX56 > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultRX56;

	pNandDrv->u16_Reg56_Rand_W_Cmd &= ~(0x00FF<<8);
	pNandDrv->u16_Reg56_Rand_W_Cmd |= (u16_Cnt<<8);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg56_Rand_W_Cmd =  %X\n",pNandDrv->u16_Reg56_Rand_W_Cmd);
	#endif

	#if defined(NC_TRR_TCS) && NC_TRR_TCS
	u16_Cnt = (pNandDrv->u8_tRR+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultTRR >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTRR > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTRR;

	u16_Tmp = (pNandDrv->u8_tCS+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultTCS >= u16_Tmp)
		u16_Tmp = 0;
	else if(u16_Tmp-u16_DefaultTCS > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Tmp -= u16_DefaultTCS;

	u16_Tmp2 = (pNandDrv->u16_tWW+u16_1T-1)/u16_1T + 2;

    if(u16_DefaultTWW >= u16_Tmp2)
        u16_Tmp2 = 0;
    else if(u16_Tmp2-u16_DefaultTWW > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
        u16_Tmp2 -= u16_DefaultTWW;

	u16_Cnt2 = MAX(u16_Tmp, u16_Tmp2);

	pNandDrv->u16_Reg59_LFSRCtrl &= ~(0x00FF);
	pNandDrv->u16_Reg59_LFSRCtrl |= (u16_Cnt|(u16_Cnt2<<4));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg59_LFSRCtrl =  %X\n",pNandDrv->u16_Reg59_LFSRCtrl);
	#endif

	#if defined(NC_TCWAW_TADL) && NC_TCWAW_TADL
	u16_Cnt = (pNandDrv->u16_tADL + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTADL > u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTADL > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTADL;

	u16_Cnt2 = (pNandDrv->u16_tCWAW + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTADL > u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultTCWAW > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultTCWAW;

	pNandDrv->u16_Reg5D_tCWAW_tADL &= ~(0xFFFF);
	pNandDrv->u16_Reg5D_tCWAW_tADL |= (u16_Cnt|(u16_Cnt2<<8));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg5D_tCWAW_tADL =  %X\n",pNandDrv->u16_Reg5D_tCWAW_tADL);
	#endif

	#if defined(NC_TWHR_TCLHZ) && NC_TWHR_TCLHZ
	u16_Cnt = (pNandDrv->u8_tCLHZ + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTCLHZ > u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTCLHZ > 0xF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTCLHZ;

	u16_Cnt2 = (pNandDrv->u16_tWHR + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTWHR > u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultTWHR > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultTWHR;

	pNandDrv->u16_Reg5A_tWHR_tCLHZ &= ~(0xFFFF);
	pNandDrv->u16_Reg5A_tWHR_tCLHZ |= ((u16_Cnt&0xF)|(u16_Cnt2<<8));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg5A_tWHR_tCLHZ =  %X\n",pNandDrv->u16_Reg5A_tWHR_tCLHZ);
	#endif

	NC_Config();
	return UNFD_ST_SUCCESS;
}

U32 nand_find_timing(U8 *pu8_ClkIdx, U8 u8_find_DDR_timg)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 au32_1TTable[NFIE_CLK_TABLE_CNT] = NFIE_1T_TABLE;
	U32 au32_ClkValueTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_VALUE_TABLE;

	#if defined(FCIE4_DDR) && FCIE4_DDR
	U32 au32_4Clk1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE;
	U32 au32_4ClkValueTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_VALUE_TABLE;
	#endif
	U32 u32_Clk;
	U16 u16_SeqAccessTime, u16_Tmp, u16_Tmp2, u16_1T, u16_RE_LATCH_DELAY;
	S8 s8_ClkIdx;

	s8_ClkIdx = 0;
	u16_1T = 0;

	if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_ONFI)
	{
		u16_SeqAccessTime = 10;
	}
	else if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_TOGGLE)
	{
		u16_Tmp = MAX(MAX(2*pNandDrv->u8_tRP, 2*pNandDrv->u8_tREH), pNandDrv->u16_tRC);
		u16_Tmp2 = MAX(MAX(pNandDrv->u8_tWP, pNandDrv->u8_tWH), (pNandDrv->u16_tWC+1)/2);
		u16_SeqAccessTime = MAX(u16_Tmp, u16_Tmp2);
	}
	else
	{
		u16_Tmp = MAX(MAX(pNandDrv->u8_tRP, pNandDrv->u8_tREH), (pNandDrv->u16_tRC+1)/2);
		u16_Tmp2 = MAX(MAX(pNandDrv->u8_tWP, pNandDrv->u8_tWH), (pNandDrv->u16_tWC+1)/2);
		u16_SeqAccessTime = MAX(u16_Tmp, u16_Tmp2);

		u16_Tmp = (pNandDrv->u8_tREA + NAND_SEQ_ACC_TIME_TOL)/2;
		u16_Tmp2 = u16_SeqAccessTime;
		u16_SeqAccessTime = MAX(u16_Tmp, u16_Tmp2);

	}

	u32_Clk = 1000000000/((U32)u16_SeqAccessTime);

	if(!u8_find_DDR_timg)
	{
		for(s8_ClkIdx =  0; s8_ClkIdx <= NFIE_CLK_TABLE_CNT - 1; s8_ClkIdx ++)
		{
			if(u32_Clk <= au32_ClkValueTable[s8_ClkIdx])
			{
				break;
			}
		}
	}
	else
	{
		#if defined(FCIE4_DDR) && FCIE4_DDR
		for(s8_ClkIdx =  0; s8_ClkIdx <= NFIE_4CLK_TABLE_CNT - 1; s8_ClkIdx ++)
		{
			if(u32_Clk < au32_4ClkValueTable[s8_ClkIdx])
			{
				break;
			}
		}
		#endif
	}
	s8_ClkIdx --;


RETRY:
	if(s8_ClkIdx<0)
		return UNFD_ST_ERR_INVALID_PARAM;
	if(!u8_find_DDR_timg)
		u16_1T = au32_1TTable[s8_ClkIdx];
	#if defined(FCIE4_DDR) && FCIE4_DDR
	else
		u16_1T = au32_4Clk1TTable[s8_ClkIdx];
	#endif

	if(nand_config_timing(u16_1T) != UNFD_ST_SUCCESS)
	{
		s8_ClkIdx--;
		goto RETRY;
	}

	u16_RE_LATCH_DELAY = BIT_NC_LATCH_DATA_1_0_T;


	pNandDrv->u16_Reg57_RELatch &= ~BIT_NC_LATCH_DATA_MASK;
	pNandDrv->u16_Reg57_RELatch |= u16_RE_LATCH_DELAY;

	*pu8_ClkIdx	= (U8)s8_ClkIdx;

	return UNFD_ST_SUCCESS;

}
#endif


U32 nand_config_clock(U16 u16_SeqAccessTime)
{

#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	NAND_DRIVER * pNandDrv = drvNAND_get_DrvContext_address();
	U32 u32_Err = 0;
	U32 au32_ClkTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE;
	char *ClkStrTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE_STR;

	#if defined(FCIE4_DDR) && FCIE4_DDR
	U32 au32_4ClkTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE;
	char *Clk4StrTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE_STR;
	U32 au32_1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE;
	#endif

	U8 u8_ClkIdx = 0;

	#if defined(FCIE4_DDR) && FCIE4_DDR
	if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_MASM)
	{
		if(pNandDrv->tDefaultDDR.u8_DdrTiming == 0)
		{
			printk("NAND Error: Empty Timing setting for DDR NAND is detected\n");
			nand_die();
		}
		else
		{
			pNandDrv->u32_minClk = au32_4ClkTable[pNandDrv->tMinDDR.u8_ClkIdx];
			pNandDrv->u32_Clk = au32_4ClkTable[pNandDrv->tDefaultDDR.u8_ClkIdx];
			NC_FCIE4SetInterface(1, pNandDrv->tDefaultDDR.u8_DqsMode,
				pNandDrv->tDefaultDDR.u8_DelayCell, pNandDrv->tDefaultDDR.u8_DdrTiming);

			if(nand_config_timing(au32_1TTable[pNandDrv->tDefaultDDR.u8_ClkIdx]) != UNFD_ST_SUCCESS)
			{
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
				nand_die();
				return(u32_Err);
			}
		}
		nand_debug(UNFD_DEBUG_LEVEL,1,"ok, get default DDR timing: 2Ch:%X, 57h:%X\n",
					pNandDrv->u16_Reg2C_SMStatus, pNandDrv->u16_Reg57_RELatch);
		u8_ClkIdx = pNandDrv->tDefaultDDR.u8_ClkIdx;
		printk(KERN_CRIT "[%s]\tFCIE is set to %sHz\n",__func__, Clk4StrTable[u8_ClkIdx]);

	}
	else
	#endif
	{

		u32_Err = nand_find_timing(&u8_ClkIdx, 0);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
			nand_die();
			return(u32_Err);
		}
		pNandDrv->u32_Clk = au32_ClkTable[u8_ClkIdx];
		printk(KERN_CRIT "[%s]\tFCIE is set to %sHz\n",__func__, ClkStrTable[u8_ClkIdx]);
	}

	nand_clock_setting(pNandDrv->u32_Clk);
	printk(KERN_CRIT "[%s]\treg_ckg_fcie(%08X)=%08X\n", __func__, reg_ckg_fcie, REG(reg_ckg_fcie));

	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
	printk(KERN_CRIT "[%s]\tRE LATCH is set to %X\n",__func__, pNandDrv->u16_Reg57_RELatch);

#endif
	return UNFD_ST_SUCCESS;
}

void nand_set_WatchDog(U8 u8_IfEnable)
{
	// do nothing in Linux
}

void nand_reset_WatchDog(void)
{
	// do nothing in Linux
}


//extern struct mtd_info *nand_mtd ;
U32 nand_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode)
{
    #if defined(CONFIG_MIPS)

	_dma_cache_wback_inv(u32_DMAAddr, u32_ByteCnt);

	#if defined(CONFIG_MSTAR_URANUS4)
	Chip_Flush_Memory();
	#endif

	return u32_DMAAddr;

    #elif defined(CONFIG_ARM) // ARM: AMBER3, AGATE

    //mode 0 for write, 1 for read
    if( mode == 0 )	//Write
    {
        Chip_Clean_Cache_Range_VA_PA(u32_DMAAddr,__pa(u32_DMAAddr), u32_ByteCnt);
    }
    else //Read
    {
        Chip_Flush_Cache_Range_VA_PA(u32_DMAAddr,__pa(u32_DMAAddr), u32_ByteCnt);
    }
	if(virt_to_phys((void *)u32_DMAAddr) >= MSTAR_MIU1_BUS_BASE)
	{
    	REG_SET_BITS_UINT16( NC_MIU_DMA_SEL, BIT_MIU1_SELECT);
    }
    else
    	REG_CLR_BITS_UINT16( NC_MIU_DMA_SEL, BIT_MIU1_SELECT);

	return virt_to_phys((void *)u32_DMAAddr);

    #else	// R2: JANUS

    return virt_to_phys((void*)u32_DMAAddr);

    #endif
}

void nand_flush_miu_pipe(void)
{
	#if defined(CONFIG_MSTAR_URANUS4)

	Chip_Read_Memory();

	#endif
}


#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
static U16 u16CurNCMIEEvent = 0;			// Used to store current IRQ state

irqreturn_t NC_FCIE_IRQ(int irq, void *dummy)
{
	if((REG(NC_PATH_CTL) & BIT_NC_EN) != BIT_NC_EN)
		return IRQ_NONE;
	
#if NC_REG_MIU_LAST_DONE == NC_MIE_EVENT		
	//avoid to cleaning LAST_DONE Flag
	u16CurNCMIEEvent |= REG(NC_MIE_EVENT) & (~BIT_MIU_LAST_DONE);
#else
	u16CurNCMIEEvent |= REG(NC_MIE_EVENT);
#endif	
	REG_WRITE_UINT16(NC_MIE_EVENT, u16CurNCMIEEvent);
	wake_up(&fcie_wait);

	return IRQ_HANDLED;
}

U32 nand_WaitCompleteIntr(U16 u16_WaitEvent, U32 u32_MicroSec)
{
	U16 u16_Reg;
	U32 u32_Timeout = (usecs_to_jiffies(u32_MicroSec) > 0) ? usecs_to_jiffies(u32_MicroSec) : 1; // timeout time

	if( wait_event_timeout(fcie_wait, ((u16CurNCMIEEvent & u16_WaitEvent) == u16_WaitEvent), u32_Timeout) == 0 ) // wait at least 2 second for FCIE3 events
	{
		u16CurNCMIEEvent = 0;
		REG_READ_UINT16(NC_MIE_EVENT, u16_Reg);		// Read all events
		REG_WRITE_UINT16(NC_MIE_EVENT, u16_Reg);	// Clear all events
		return UNFD_ST_ERR_E_TIMEOUT;
	}

	u16CurNCMIEEvent = 0;
	REG_WRITE_UINT16(NC_MIE_EVENT, u16_WaitEvent);

	return UNFD_ST_SUCCESS;
}



void nand_enable_intr_mode(void)
{
    int err = 0;
#if defined(CONFIG_ARM)
  	err = request_irq(E_IRQ_NFIE, NC_FCIE_IRQ, IRQF_SHARED, "fcie", &sg_NandDrv);
#else
    err = request_irq(E_IRQ_NFIE, NC_FCIE_IRQ, SA_INTERRUPT, "fcie", NULL);
#endif

	REG_WRITE_UINT16(NC_MIE_INT_EN, BIT_NC_JOB_END|BIT_MMA_DATA_END);
}

#endif

void *drvNAND_get_DrvContext_address(void) // exposed API
{
	return &sg_NandDrv;
}

//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
const U8 u8FSTYPE[256] =
{	0,19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
	0,18, 0, 6, 0, 8,10, 0, 0,12, 0, 0, 0, 0, 0, 0, // 7
	0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, // 8
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C
	0,13, 0,16, 0,17, 3, 0, 0, 0,15, 0,14, 0, 0, 0, // D
	0, 0, 0, 2, 0, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
   20,13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F
};

PAIRED_PAGE_MAP_t PairedPageMap_Samsung[] = {
    {.u16_LSB = 0x00, .u16_MSB = 0x02}, {.u16_LSB = 0x01, .u16_MSB = 0x04}, {.u16_LSB = 0x03, .u16_MSB = 0x06},
    {.u16_LSB = 0x05, .u16_MSB = 0x08}, {.u16_LSB = 0x07, .u16_MSB = 0x0A}, {.u16_LSB = 0x09, .u16_MSB = 0x0C},
    {.u16_LSB = 0x0B, .u16_MSB = 0x0E}, {.u16_LSB = 0x0D, .u16_MSB = 0x10}, {.u16_LSB = 0x0F, .u16_MSB = 0x12},
    {.u16_LSB = 0x11, .u16_MSB = 0x14}, {.u16_LSB = 0x13, .u16_MSB = 0x16}, {.u16_LSB = 0x15, .u16_MSB = 0x18},
    {.u16_LSB = 0x17, .u16_MSB = 0x1A}, {.u16_LSB = 0x19, .u16_MSB = 0x1C}, {.u16_LSB = 0x1B, .u16_MSB = 0x1E},
    {.u16_LSB = 0x1D, .u16_MSB = 0x20}, {.u16_LSB = 0x1F, .u16_MSB = 0x22}, {.u16_LSB = 0x21, .u16_MSB = 0x24},
    {.u16_LSB = 0x23, .u16_MSB = 0x26}, {.u16_LSB = 0x25, .u16_MSB = 0x28}, {.u16_LSB = 0x27, .u16_MSB = 0x2A},
    {.u16_LSB = 0x29, .u16_MSB = 0x2C}, {.u16_LSB = 0x2B, .u16_MSB = 0x2E}, {.u16_LSB = 0x2D, .u16_MSB = 0x30},
    {.u16_LSB = 0x2F, .u16_MSB = 0x32}, {.u16_LSB = 0x31, .u16_MSB = 0x34}, {.u16_LSB = 0x33, .u16_MSB = 0x36},
    {.u16_LSB = 0x35, .u16_MSB = 0x38}, {.u16_LSB = 0x37, .u16_MSB = 0x3A}, {.u16_LSB = 0x39, .u16_MSB = 0x3C},
    {.u16_LSB = 0x3B, .u16_MSB = 0x3E}, {.u16_LSB = 0x3D, .u16_MSB = 0x40}, {.u16_LSB = 0x3F, .u16_MSB = 0x42},
    {.u16_LSB = 0x41, .u16_MSB = 0x44}, {.u16_LSB = 0x43, .u16_MSB = 0x46}, {.u16_LSB = 0x45, .u16_MSB = 0x48},
    {.u16_LSB = 0x47, .u16_MSB = 0x4A}, {.u16_LSB = 0x49, .u16_MSB = 0x4C}, {.u16_LSB = 0x4B, .u16_MSB = 0x4E},
    {.u16_LSB = 0x4D, .u16_MSB = 0x50}, {.u16_LSB = 0x4F, .u16_MSB = 0x52}, {.u16_LSB = 0x51, .u16_MSB = 0x54},
    {.u16_LSB = 0x53, .u16_MSB = 0x56}, {.u16_LSB = 0x55, .u16_MSB = 0x58}, {.u16_LSB = 0x57, .u16_MSB = 0x5A},
    {.u16_LSB = 0x59, .u16_MSB = 0x5C}, {.u16_LSB = 0x5B, .u16_MSB = 0x5E}, {.u16_LSB = 0x5D, .u16_MSB = 0x60},
    {.u16_LSB = 0x5F, .u16_MSB = 0x62}, {.u16_LSB = 0x61, .u16_MSB = 0x64}, {.u16_LSB = 0x63, .u16_MSB = 0x66},
    {.u16_LSB = 0x65, .u16_MSB = 0x68}, {.u16_LSB = 0x67, .u16_MSB = 0x6A}, {.u16_LSB = 0x69, .u16_MSB = 0x6C},
    {.u16_LSB = 0x6B, .u16_MSB = 0x6E}, {.u16_LSB = 0x6D, .u16_MSB = 0x70}, {.u16_LSB = 0x6F, .u16_MSB = 0x72},
    {.u16_LSB = 0x71, .u16_MSB = 0x74}, {.u16_LSB = 0x73, .u16_MSB = 0x76}, {.u16_LSB = 0x75, .u16_MSB = 0x78},
    {.u16_LSB = 0x77, .u16_MSB = 0x7A}, {.u16_LSB = 0x79, .u16_MSB = 0x7C}, {.u16_LSB = 0x7B, .u16_MSB = 0x7E},
    {.u16_LSB = 0x7D, .u16_MSB = 0x7F}
};

PAIRED_PAGE_MAP_t ga_tPairedPageMap[512];

#define VENDOR_FUJITSU						0x04
#define VENDOR_RENESAS						0x07
#define VENDOR_ST							0x20
#define VENDOR_MICRON						0x2C
#define VENDOR_NATIONAL						0x8F
#define VENDOR_TOSHIBA						0x98
#define VENDOR_HYNIX						0xAD
#define VENDOR_SAMSUNG						0xEC
#define VENDOR_MXIC                         0xC2
#define VENDOR_ZENTEL                       0x92
#define VENDOR_ZENTEL1                      0xC8
#define VENDOR_SPANSION                 0x01

void drvNAND_CHECK_FLASH_TYPE(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    U16 u16_i,u16_j=0;
    U8 u8_more_maker=0;

    if(pNandDrv->au8_ID[0] ==0x7F)
    {
		u8_more_maker=1;
    }

    if(u8_more_maker)
    {
		for (u16_i=1; u16_i<NAND_ID_BYTE_CNT;u16_i++)
		{
			if (pNandDrv->au8_ID[u16_i] == 0x7F)
			{
				u16_j = u16_i;
			}
			else
			{
				pNandDrv->au8_ID[u16_i-u16_j-1] = pNandDrv->au8_ID[u16_i];
			}
		}
		pNandDrv->u8_IDByteCnt -= (u16_j+1);
    }

	printk("NAND ID:");
	for(u16_i = 0; u16_i < pNandDrv->u8_IDByteCnt; u16_i++)
	  printk("0x%X ", pNandDrv->au8_ID[u16_i]);
	printk("\n");

	if( (pNandDrv->au8_ID[0] != VENDOR_SAMSUNG) &&
		(pNandDrv->au8_ID[0] != VENDOR_TOSHIBA) &&
		(pNandDrv->au8_ID[0] != VENDOR_RENESAS) &&
		(pNandDrv->au8_ID[0] != VENDOR_HYNIX)  &&
		(pNandDrv->au8_ID[0] != VENDOR_FUJITSU) &&
		(pNandDrv->au8_ID[0] != VENDOR_MICRON) &&
		(pNandDrv->au8_ID[0] != VENDOR_NATIONAL)  &&
		(pNandDrv->au8_ID[0] != VENDOR_ST) &&
        (pNandDrv->au8_ID[0] != VENDOR_MXIC) &&
        (pNandDrv->au8_ID[0] != VENDOR_ZENTEL) &&
        (pNandDrv->au8_ID[0] != VENDOR_ZENTEL1) &&
        (pNandDrv->au8_ID[0] != VENDOR_SPANSION))
	{
		pNandDrv->u16_BlkCnt = 0;
		pNandDrv->u16_BlkPageCnt = 0;
		pNandDrv->u16_PageByteCnt = 0;
		pNandDrv->u16_SectorByteCnt = 0;
		pNandDrv->u16_SpareByteCnt = 0;

		printk("Unsupport Vendor %02X\n", pNandDrv->au8_ID[0]);

		return; // unsupported flash maker
	}

	pNandDrv->u8_IsMLC = 0;
	pNandDrv->u16_ECCType = ECC_TYPE_4BIT;
	pNandDrv->u8_PlaneCnt = 1;
	pNandDrv->u8_CacheProgram = 0;
	pNandDrv->u8_CacheRead = 0;
	/*Default NAND Timing Setting*/
	pNandDrv->u16_tRC = 30;
	pNandDrv->u8_tREA = 20;
	pNandDrv->u8_tRP	= 12;
	pNandDrv->u8_tREH = 10;
	pNandDrv->u16_tWC = 25;
	pNandDrv->u8_tWP = 12;
	pNandDrv->u8_tWH = 10;
	pNandDrv->u16_tWW = 100;
	switch(u8FSTYPE[pNandDrv->au8_ID[1]])
	{
		case 0:
			pNandDrv->u16_BlkCnt = 0;
			pNandDrv->u16_BlkPageCnt = 0;
			pNandDrv->u16_PageByteCnt = 0;
			pNandDrv->u16_SectorByteCnt = 0;
			pNandDrv->u16_SpareByteCnt = 0;
			break;
		case 2:
			pNandDrv->u16_BlkCnt = 512;
			pNandDrv->u16_BlkPageCnt = 16;
			pNandDrv->u16_PageByteCnt = 512;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 16;
			break;
		case 4:
			pNandDrv->u16_BlkCnt = 1024;
			pNandDrv->u16_BlkPageCnt = 16;
			pNandDrv->u16_PageByteCnt = 512;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 16;
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C3TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
			break;
		case 6:
			pNandDrv->u16_BlkCnt = 1024;
			pNandDrv->u16_BlkPageCnt = 32;
			pNandDrv->u16_PageByteCnt = 512;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 16;
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C3TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
			break;
		case 7:
			//_fsinfo.eFlashConfig |= FLASH_WP;
		case 8:
			pNandDrv->u16_BlkCnt = 2048;
			pNandDrv->u16_BlkPageCnt = 32;
			pNandDrv->u16_PageByteCnt = 512;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 16;
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C3TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
			break;
		case 10:
			pNandDrv->u16_BlkCnt = 4096;
			pNandDrv->u16_BlkPageCnt = 32;
			pNandDrv->u16_PageByteCnt = 512;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 16;
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
			break;
		case 12:
			pNandDrv->u16_BlkCnt = 8192;
			pNandDrv->u16_BlkPageCnt = 32;
			pNandDrv->u16_PageByteCnt = 512;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 16;
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
			break;
		case 13:
			if((pNandDrv->au8_ID[0]==VENDOR_SPANSION))      //spansion 1Gbit
			{                                       
				pNandDrv->u16_BlkCnt = 1024;
				pNandDrv->u16_BlkPageCnt = 64;
				pNandDrv->u16_PageByteCnt = 2048;
				pNandDrv->u16_SectorByteCnt = 512;
				pNandDrv->u16_SpareByteCnt = 64;
				pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
				pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
			}
			else
			{
				pNandDrv->u16_BlkCnt = 1024;
				pNandDrv->u16_BlkPageCnt = 64;
				pNandDrv->u16_PageByteCnt = 2048;
				pNandDrv->u16_SectorByteCnt = 512;
				pNandDrv->u16_SpareByteCnt = 64;
				pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
				pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
			}            
			break;
		case 14:
			if((pNandDrv->au8_ID[0]==VENDOR_SPANSION))      //spansion 4Gbit
			{
				pNandDrv->u16_BlkCnt = 4096;
				pNandDrv->u16_BlkPageCnt = 64;
				pNandDrv->u16_PageByteCnt = 2048;
				pNandDrv->u16_SectorByteCnt = 512;
				pNandDrv->u16_SpareByteCnt = 64;
			}
			else
			{
				pNandDrv->u16_PageByteCnt = 1024 << (pNandDrv->au8_ID[3] & 3);
				pNandDrv->u16_BlkPageCnt  = ((64 * 1024)  << ((pNandDrv->au8_ID[3] >> 4) & 3)) /  pNandDrv->u16_PageByteCnt;
				pNandDrv->u16_SectorByteCnt = 512;
				pNandDrv->u16_SpareByteCnt = (8 << (( pNandDrv->au8_ID[3] >> 2)& 0x01)) * ( pNandDrv->u16_PageByteCnt>>9);
				pNandDrv->u16_BlkCnt = 4096 / (pNandDrv->u16_PageByteCnt/2048);
			}            
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
			break;
		case 15:
                    //spansion 2Gbit
			pNandDrv->u16_BlkCnt = 2048;
			pNandDrv->u16_BlkPageCnt = 64;
			pNandDrv->u16_PageByteCnt = 2048;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 64;
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
			break;
		case 16:
			if((pNandDrv->au8_ID[0]==VENDOR_HYNIX))
			{
				pNandDrv->u16_BlkCnt = 4096;
				pNandDrv->u16_BlkPageCnt = 128;
				pNandDrv->u16_PageByteCnt = 2048;
				pNandDrv->u16_SectorByteCnt = 512;
				pNandDrv->u16_SpareByteCnt = 64;
			}
			else if((pNandDrv->au8_ID[0] == VENDOR_TOSHIBA))
			{
				pNandDrv->u16_BlkCnt  = 4096;
				pNandDrv->u16_BlkPageCnt = 64;
				pNandDrv->u16_PageByteCnt = 4096;
				pNandDrv->u16_SectorByteCnt = 512;
				pNandDrv->u16_SpareByteCnt = 128;
			}
			else if(pNandDrv->au8_ID[0] != VENDOR_ST)
			{
				pNandDrv->u16_BlkCnt = 2048;
				pNandDrv->u16_BlkPageCnt = 64;
				pNandDrv->u16_PageByteCnt = 2048;
				pNandDrv->u16_SectorByteCnt = 512;
				pNandDrv->u16_SpareByteCnt = 64;
			}
			else
			{
				pNandDrv->u16_BlkCnt = 8192;
				pNandDrv->u16_BlkPageCnt = 64;
				pNandDrv->u16_PageByteCnt = 2048;
				pNandDrv->u16_SectorByteCnt = 512;
				pNandDrv->u16_SpareByteCnt = 64;
			}
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
			break;
		case 17:
			if ((pNandDrv->au8_ID[2] & 0xC) == 0)  // for SLC
			{
				if(pNandDrv->au8_ID[0] != VENDOR_SAMSUNG)
				{
					pNandDrv->u16_BlkCnt = 8192;
					pNandDrv->u16_BlkPageCnt = 128;
					pNandDrv->u16_PageByteCnt = 2048;
					pNandDrv->u16_SectorByteCnt = 512;
					pNandDrv->u16_SpareByteCnt = 64;
				}
				else
				{
					pNandDrv->u16_BlkCnt = 16384;
					pNandDrv->u16_BlkPageCnt = 64;
					pNandDrv->u16_PageByteCnt = 2048;
					pNandDrv->u16_SectorByteCnt = 512;
					pNandDrv->u16_SpareByteCnt = 64;
				}
			}
			else  // for MLC
			{
				U8 u8PageSize, u8Value;
				U16 u16_i;
				pNandDrv->u8_IsMLC = 1;
				pNandDrv->u16_PageByteCnt = 2048 << (pNandDrv->au8_ID[3] & 0x3);
				u8PageSize = pNandDrv->u16_PageByteCnt >> 10;
				u8Value = ((pNandDrv->au8_ID[3] >> 4) & 0x3) | ((pNandDrv->au8_ID[3] >> 5) & 0x4);
		
				if (pNandDrv->au8_ID[0] == VENDOR_SAMSUNG)
				{
					pNandDrv->u16_BlkCnt = 2076;
			 		pNandDrv->u16_BlkPageCnt = (128 << u8Value) / u8PageSize;
			
			 		for (u16_i = 0; u16_i < 128; u16_i++)
			 		{
						ga_tPairedPageMap[u16_i].u16_LSB = PairedPageMap_Samsung[u16_i].u16_LSB;
						ga_tPairedPageMap[u16_i].u16_MSB = PairedPageMap_Samsung[u16_i].u16_MSB;
		    		}
				}
				else if (pNandDrv->au8_ID[0] == VENDOR_HYNIX)
				{
					pNandDrv->u16_BlkCnt = 1024;
					
					if (u8Value == 0x0)
					{
						pNandDrv->u16_BlkPageCnt = 128 / u8PageSize;
					}
					else if (u8Value == 0x1)
					{
						pNandDrv->u16_BlkPageCnt = 256 / u8PageSize;
					}
					else if (u8Value == 0x2)
					{
						pNandDrv->u16_BlkPageCnt = 512 / u8PageSize;
					}
					else if (u8Value == 0x3) // 768 is not power of 2, should fix according to specific chip
					{
						pNandDrv->u16_BlkPageCnt = 768 / u8PageSize;
					}
					else if (u8Value == 0x4)
					{
						pNandDrv->u16_BlkPageCnt = 1024 / u8PageSize;
					}
					else if (u8Value == 0x5)
					{
						pNandDrv->u16_BlkPageCnt = 2048 / u8PageSize;
					}
				}
				pNandDrv->u16_SectorByteCnt = 1024;
				pNandDrv->u16_SpareByteCnt = 432;
				pNandDrv->u16_ECCType = ECC_TYPE_24BIT1KB;
		    }
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C5TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
			break;
		case 18:
			pNandDrv->u16_BlkCnt = 16384;
			pNandDrv->u16_BlkPageCnt = 32;
			pNandDrv->u16_PageByteCnt = 512;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 16;
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C3TRS0;
			break;
		case 20:
			pNandDrv->u16_BlkCnt = 1024;
			pNandDrv->u16_BlkPageCnt = 64;
			pNandDrv->u16_PageByteCnt = 2048;
			pNandDrv->u16_SectorByteCnt = 512;
			pNandDrv->u16_SpareByteCnt = 64;
			pNandDrv->u8_OpCode_RW_AdrCycle = ADR_C4TFS0;
			pNandDrv->u8_OpCode_Erase_AdrCycle = ADR_C2TRS0;
			break;
		default:
			pNandDrv->u16_BlkCnt = 0;
			pNandDrv->u16_BlkPageCnt = 0;
			pNandDrv->u16_PageByteCnt = 0;
			pNandDrv->u16_SectorByteCnt = 0;
			pNandDrv->u16_SpareByteCnt = 0;
			break;
	}
}

U32 NC_PlatformResetPre(void)
{
	//HalMiuMaskReq(MIU_CLT_FCIE);
	return UNFD_ST_SUCCESS;
}

U32 NC_PlatformResetPost(void)
{
	U16 u16_Reg;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	REG_READ_UINT16(FCIE_NC_CIFD_BASE, u16_Reg); // dummy read for CIFD clock

	//#if defined(DUTY_CYCLE_PATCH)&&DUTY_CYCLE_PATCH
	REG_WRITE_UINT16(NC_WIDTH, FCIE_REG41_VAL);	// duty cycle 3:1 in 86Mhz
	//#endif

	#if !defined(CONFIG_MSTAR_KRONUS)
	// miu eco
	#if !(defined(CONFIG_MSTAR_AMBER5) || \
          defined(CONFIG_MSTAR_AMBER3) || \
          defined(CONFIG_MSTAR_AGATE) || \
          defined(CONFIG_MSTAR_EAGLE) || \
          defined(CONFIG_MSTAR_EDISON) || \
          defined(CONFIG_MSTAR_EIFFEL)) || \
		  defined(CONFIG_MSTAR_EINSTEIN)
	REG_SET_BITS_UINT16(NC_REG_2Fh, BIT0);
	#endif
	#else
	// Why miu eco is different in K1?
	// 1. The register is changed from 0x2F BIT[0] to 0x2D BIT[7].
	// 2. There is a bug in 0x2D:
	//    The original value of 0x2D is 0x1F.
	//    If we set the BIT[1:0] = 0x3, the BIT[9:8] will also be 0x3
	//    If the the BIT[9:8] is set to 0x3, the byte reorder will be enabled. So we will fail in READ_ID.
	//    So we need to force setting 0x2D as 0x90.
	REG_WRITE_UINT16(FCIE_NC_REORDER, 0x90);
	#endif

	#if defined (DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
	#else

	#if defined(REG57_ECO_FIX_INIT_VALUE)
	REG_WRITE_UINT16(NC_LATCH_DATA, REG57_ECO_FIX_INIT_VALUE);
	pNandDrv->u16_Reg57_RELatch = REG57_ECO_FIX_INIT_VALUE;
	#endif

	#endif
	//HalMiuUnMaskReq(MIU_CLT_FCIE);

    return UNFD_ST_SUCCESS;
}

U32 NC_PlatformInit(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	nand_pads_init();
	pNandDrv->u8_WordMode = 0; // TV/Set-Top Box projects did not support x16 NAND flash
	nand_pads_switch(1);

#if defined (DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	pNandDrv->u32_Clk =FCIE3_SW_SLOWEST_CLK;
	nand_clock_setting(FCIE3_SW_SLOWEST_CLK);
#else
	pNandDrv->u32_Clk =FCIE3_SW_DEFAULT_CLK;
	nand_clock_setting(FCIE3_SW_DEFAULT_CLK);
#endif
    // print clock setting
	printk(KERN_CRIT "reg_ckg_fcie(%08X)=%08X\n", reg_ckg_fcie, REG(reg_ckg_fcie));

	// no shared-bus with Disp
	pNandDrv->u8_SwPatchWaitRb= 0;
	pNandDrv->u8_SwPatchJobStart= 0;

	return UNFD_ST_SUCCESS;
}

#else
  #error "Error! no platform functions."
#endif

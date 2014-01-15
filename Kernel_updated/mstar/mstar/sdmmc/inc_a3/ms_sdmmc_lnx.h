/***************************************************************************************************************
 *
 * FileName ms_sdmmc_lnx.h
 *     @author jeremy.wang (2012/01/10)
 * Desc:
 * 	   This file is the header file of ms_sdmmc_lnx.c.
 *
 ***************************************************************************************************************/

#ifndef __MS_SDMMC_LNX_H
#define __MS_SDMMC_LNX_H

#include <linux/cdev.h>
#include "hal_card_base.h"

//***********************************************************************************************************
// Config Setting (Externel)
//***********************************************************************************************************

//###########################################################################################################
#if (D_PROJECT == D_PROJECT__CB2)    //For Columbus2
//###########################################################################################################
	#define D_SDMMC1_IP                EV_IP_FCIE2                    //SDIO
	#define D_SDMMC1_PORT              EV_PORT_SDIO1                  //Port Setting
	#define D_SDMMC1_PAD               EV_PAD2                        //PAD_SD
	#define D_SDMMC1_MUTEX             EV_MUTEX1

	#define D_SDMMC2_IP                EV_IP_FCIE1                    //SDIO
	#define D_SDMMC2_PORT              EV_PORT_SDIO1
	#define D_SDMMC2_PAD               EV_PAD1                        //PAD_NAND
	#define D_SDMMC2_MUTEX             EV_MUTEX2S

	#define EN_SDMMC1_PASS_SWPAD       (TRUE)
	#define EN_SDMMC2_PASS_SWPAD       (TRUE)

	#define EN_MUTEX2SHARE             (FALSE)                        //NO Share (eMMC)

	#define WT_POWERUP                 25 //(ms)
	#define WT_POWERON                 25 //(ms)
	#define WT_POWEROFF                25 //(ms)

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__G2)    //For G2
//###########################################################################################################
	#define D_SDMMC1_IP                EV_IP_FCIE2                    //SDIO
	#define D_SDMMC1_PORT              EV_PORT_SDIO1
	#define D_SDMMC1_PAD               EV_PAD3                        //PAD_SD
	#define D_SDMMC1_MUTEX             EV_MUTEX1

	#define D_SDMMC2_IP                EV_IP_FCIE1
	#define D_SDMMC2_PORT              EV_PORT_SD
	#define D_SDMMC2_PAD               EV_PAD5                        //PAD_HSL
	#define D_SDMMC2_MUTEX             EV_MUTEX2S

	#if defined(CONFIG_MSTAR_NAND)
		#define EN_MUTEX2SHARE             (TRUE)                     //Share NAND Mutex
	#else
		#define EN_MUTEX2SHARE             (FALSE)                     //Share NAND Mutex
	#endif

	#define EN_SDMMC1_PASS_SWPAD       (TRUE)
	#define EN_SDMMC2_PASS_SWPAD       (FALSE)

	#define WT_POWERUP                 5    //(ms)                     //Waiting time for Power up
	#define WT_POWERON                 5    //(ms)                     //Waiting time for Power on
	#define WT_POWEROFF                20   //(ms)                     //Waiting time for Power off

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__A3)
//###########################################################################################################
	#define D_SDMMC1_IP                EV_IP_FCIE1
	#define D_SDMMC1_PORT              EV_PORT_SD
	#define D_SDMMC1_PAD               EV_PAD1                        //PAD_SD
	#define D_SDMMC1_MUTEX             EV_MUTEX2S

    // Not use ======================================
	#define D_SDMMC2_IP                EV_IP_FCIE1
	#define D_SDMMC2_PORT              EV_PORT_SD
	#define D_SDMMC2_PAD               EV_PAD5                        //PAD_HSL
	#define D_SDMMC2_MUTEX             EV_MUTEX2S
    //===============================================

	#define EN_MUTEX2SHARE             (TRUE)                     //Share NAND Mutex

	#define EN_SDMMC1_PASS_SWPAD       (TRUE)
	#define EN_SDMMC2_PASS_SWPAD       (FALSE)

	#define WT_POWERUP                 5    //(ms)                     //Waiting time for Power up
	#define WT_POWERON                 5    //(ms)                     //Waiting time for Power on
	#define WT_POWEROFF                20   //(ms)                     //Waiting time for Power off

//###########################################################################################################
#else    //Templete Description
//###########################################################################################################
/*
	#define D_SDMMC1_IP                //IP Setting
	#define D_SDMMC1_PORT              //Port Setting
 	#define D_SDMMC1_PAD               //PAD from Platform Setting
	#define D_SDMMC1_MUTEX             //Mutex Setting


    //If we have dual cards, the value of D_SDMMC2_IP must differ from the value of D_SDMMC1_IP.
 	//If we have only single card, D_SDMMC2_IP/D_SDMMC2_PORT/D_SDMMC2_PAD have no meaning,
 	//You could set them the same with D_SDMMC1 series.

	#define D_SDMMC2_IP
 	#define D_SDMMC2_PORT
 	#define D_SDMMC2_PAD

    //We could set the value of D_SDMMC2_MUTEX the same with the value of D_SDMMC1_MUTEX if we need.
 	#define D_SDMMC2_MUTEX

	#define EN_SDMMC1_PASS_SWPAD       (TRUE)                          //Pass Switch PAD Before Every Request
	#define EN_SDMMC2_PASS_SWPAD       (FALSE)

	#define EN_SDMMC2_MUTEXSHARE       (TRUE)                         //Share NAND Mutex

	#define WT_POWERUP                 5    //(ms)                     //Waiting time for Power up
	#define WT_POWERON                 5    //(ms)                     //Waiting time for Power on
	#define WT_POWEROFF                20   //(ms)                     //Waiting time for Power off

*/
//###########################################################################################################
#endif
//###########################################################################################################


//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC_REVCDZ) || defined(CONFIG_MS_SDMMC_REVCDZ)
	#define EN_SDMMC_CDZREV        (TRUE)
#else
	#define EN_SDMMC_CDZREV        (FALSE)
#endif
//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC_TCARD) || defined(CONFIG_MS_SDMMC_TCARD)
	#define EN_SDMMC_TCARD         (TRUE)
#else
	#define EN_SDMMC_TCARD         (TRUE)
#endif

#if defined(CONFIG_MSTAR_SDMMC_REVWP) || defined(CONFIG_MS_SDMMC_REVWP)
	#define EN_SDMMC_WPREV         (TRUE)
#else
	#define EN_SDMMC_WPREV         (FALSE)
#endif
//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC_DUALCARDS) || defined(CONFIG_MS_SDMMC_DUALCARDS)
#define EN_SDMMC_DUAL_CARDS    (TRUE)
#else
#define EN_SDMMC_DUAL_CARDS    (FALSE)
#endif
//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC1_MAXCLK)
	#define V_SDMMC1_MAX_CLK       CONFIG_MSTAR_SDMMC1_MAXCLK
#elif defined(CONFIG_MS_SDMMC1_MAXCLK)
	#define V_SDMMC1_MAX_CLK       CONFIG_MS_SDMMC1_MAXCLK
#else
	#define	V_SDMMC1_MAX_CLK       400000
#endif

#if defined(CONFIG_MSTAR_SDMMC1_MAXDLVL)
	#define V_SDMMC1_MAX_DLVL      CONFIG_MSTAR_SDMMC1_MAXDLVL
#elif defined(CONFIG_MS_SDMMC1_MAXDLVL)
	#define V_SDMMC1_MAX_DLVL      CONFIG_MS_SDMMC1_MAXDLVL
#else
	#define V_SDMMC1_MAX_DLVL      0
#endif

#if defined(CONFIG_MSTAR_SDMMC1_HOTP) || defined(CONFIG_MS_SDMMC1_HOTP)
	#define EN_SDMMC1_HOTP         (TRUE)
#else
	#define EN_SDMMC1_HOTP         (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC1_FAKECDZ) || defined(CONFIG_MS_SDMMC1_FAKECDZ)
	#define EN_SDMMC1_FAKECDZ      (TRUE)
#else
	#define EN_SDMMC1_FAKECDZ      (FALSE)
#endif
//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC2_MAXCLK)
	#define V_SDMMC2_MAX_CLK       CONFIG_MSTAR_SDMMC2_MAXCLK
#elif defined(CONFIG_MS_SDMMC2_MAXCLK)
	#define V_SDMMC2_MAX_CLK       CONFIG_MS_SDMMC2_MAXCLK
#else
	#define	V_SDMMC2_MAX_CLK       400000
#endif

#if defined(CONFIG_MSTAR_SDMMC2_MAXDLVL)
	#define V_SDMMC2_MAX_DLVL      CONFIG_MSTAR_SDMMC2_MAXDLVL
#elif defined(CONFIG_MS_SDMMC2_MAXDLVL)
	#define V_SDMMC2_MAX_DLVL      V_SDMMC2_MAX_DLVL
#else
	#define V_SDMMC2_MAX_DLVL      0
#endif

#if defined(CONFIG_MSTAR_SDMMC2_HOTP) || defined(CONFIG_MS_SDMMC2_HOTP)
	#define EN_SDMMC2_HOTP         (TRUE)
#else
	#define EN_SDMMC2_HOTP         (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC2_FAKECDZ) || defined(CONFIG_MS_SDMMC2_FAKECDZ)
	#define EN_SDMMC2_FAKECDZ      (TRUE)
#else
	#define EN_SDMMC2_FAKECDZ      (FALSE)
#endif
//------------------------------------------------------------------------------------------------


//***********************************************************************************************************
typedef enum
{
	EV_SDMMC1 = 0,
	EV_SDMMC2 = 1,

} SlotEmType;

typedef enum
{
	EV_MUTEX1  = 0,
	EV_MUTEX2S = 1,

} MutexEmType;

struct ms_sdmmc_host
{
	struct platform_device	*pdev;
	struct ms_sdmmc_slot *sdmmc_slot[2];

};

struct ms_sdmmc_slot
{
	struct mmc_host		*mmc;

	unsigned int	slotNo;         //Slot No.
	unsigned int	mieIRQNo;       //MIE IRQ No.
	unsigned int	cdzIRQNo;       //CDZ IRQ No.
	unsigned int    pmrsaveClk;     //Power Saving Clock

	unsigned short 	currVdd;        //Current Vdd
	unsigned int	currClk;        //Current Clock
	unsigned int 	currRealClk;    //Current Real Clock
	unsigned char	currWidth;      //Current Bus Width
	unsigned char   currTiming;     //Current Bus Timning
	unsigned char   currPowrMode;   //Current PowerMode
	unsigned char	currBusMode;    //Current Bus Mode
	unsigned char   currDDR;        //Current DDR
	unsigned char   currDownLevel;  //Current Down Level

	int read_only;                  //WP
	int card_det;                   //Card Detect
	int is_initial;

	/****** DMA buffer used for transmitting *******/
	u32 *dma_buffer;
	dma_addr_t dma_phy_addr;

	/***** Tasklet for hotplug ******/
	struct tasklet_struct   hotplug_tasklet;

};  /* struct ms_sdmmc_hot*/


#endif // End of __MS_SDMMC_LNX_H


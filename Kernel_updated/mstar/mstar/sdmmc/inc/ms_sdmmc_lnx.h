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
#include <linux/interrupt.h>
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
	#define D_SDMMC2_MUTEX             EV_MUTEXS

	#define WT_POWERUP                 25 //(ms)
	#define WT_POWERON                 25 //(ms)
	#define WT_POWEROFF                25 //(ms)

	#define EN_SDMMC_DCACHE_FLUSH	   (TRUE)
	#define EN_SDMMC1_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC2_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC1_CDZIRQ_SHARD     (FALSE)
	#define EN_SDMMC2_CDZIRQ_SHARD     (FALSE)
	#define EN_SDMMC1_CDZIRQ_WAKEUP    (FALSE)
	#define EN_SDMMC2_CDZIRQ_WAKEUP    (FALSE)
	#define EV_SDMMC1_DOWN_LVL         (FALSE)
	#define EV_SDMMC2_DOWN_LVL         (FALSE)

	#define V_SDMMC1_MIEIRQ_PARA       IRQF_SHARED|IRQF_DISABLED
	#define V_SDMMC2_MIEIRQ_PARA       IRQF_SHARED|IRQF_DISABLED
	#define V_SDMMC1_CDZIRQ_PARA       IRQ_TYPE_EDGE_BOTH|IRQF_DISABLED
	#define V_SDMMC2_CDZIRQ_PARA       IRQ_TYPE_EDGE_BOTH|IRQF_DISABLED

	#include <mach/irqs.h>
	#define V_SDMMC1_MIEIRQ            INT_MS_SDIO
	#define V_SDMMC2_MIEIRQ            INT_MS_FCIE
	#define V_SDMMC1_CDZIRQ            0
	#define V_SDMMC2_CDZIRQ            0


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
	#define D_SDMMC2_MUTEX             EV_MUTEXS

	#define WT_POWERUP                 5    //(ms)                     //Waiting time for Power up
	#define WT_POWERON                 5    //(ms)                     //Waiting time for Power on
	#define WT_POWEROFF                20   //(ms)                     //Waiting time for Power off

	#define EN_SDMMC_DCACHE_FLUSH	   (TRUE)
	#define EN_SDMMC1_RUN_DDRSDR       (TRUE)
	#define EN_SDMMC2_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC1_CDZIRQ_SHARD     (FALSE)
	#define EN_SDMMC2_CDZIRQ_SHARD     (FALSE)
	#define EN_SDMMC1_CDZIRQ_WAKEUP    (TRUE)
	#define EN_SDMMC2_CDZIRQ_WAKEUP    (FALSE)
	#define EV_SDMMC1_DOWN_LVL         (TRUE)
	#define EV_SDMMC2_DOWN_LVL         (TRUE)

	#define V_SDMMC1_MIEIRQ_PARA       IRQF_SHARED|IRQF_DISABLED
	#define V_SDMMC2_MIEIRQ_PARA       IRQF_SHARED|IRQF_DISABLED
	#define V_SDMMC1_CDZIRQ_PARA       IRQ_TYPE_EDGE_BOTH|IRQF_DISABLED
	#define V_SDMMC2_CDZIRQ_PARA       IRQ_TYPE_EDGE_BOTH|IRQF_DISABLED

	#include <chip_int.h>
	#define V_SDMMC1_MIEIRQ            E_IRQ_SDIO
	#define V_SDMMC2_MIEIRQ            E_IRQ_FCIE
	#define V_SDMMC1_CDZIRQ            0
	#define V_SDMMC2_CDZIRQ            0

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__EAGLE)
//###########################################################################################################
	#define D_SDMMC1_IP                EV_IP_FCIE1                    //SD
	#define D_SDMMC1_PORT              EV_PORT_SD                     //Port Setting
	#define D_SDMMC1_PAD               EV_PAD5                        //REG_SD0_CONFIG2 = 1
	#define D_SDMMC1_MUTEX             EV_MUTEXS

	#define D_SDMMC2_IP                EV_IP_FCIE1                    //SD
	#define D_SDMMC2_PORT              EV_PORT_SD
	#define D_SDMMC2_PAD               EV_PAD1                        //PAD_NAND
	#define D_SDMMC2_MUTEX             EV_MUTEXS

	#define WT_POWERUP                 25 //(ms)
	#define WT_POWERON                 25 //(ms)
	#define WT_POWEROFF                25 //(ms)

	#define EN_SDMMC_DCACHE_FLUSH	   (FALSE)
	#define EN_SDMMC1_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC2_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC1_CDZIRQ_SHARD     (TRUE)
	#define EN_SDMMC2_CDZIRQ_SHARD     (TRUE)
	#define EN_SDMMC1_CDZIRQ_WAKEUP    (FALSE)
	#define EN_SDMMC2_CDZIRQ_WAKEUP    (FALSE)
	#define EV_SDMMC1_DOWN_LVL         (TRUE)
	#define EV_SDMMC2_DOWN_LVL         (TRUE)

	#define V_SDMMC1_MIEIRQ_PARA       IRQF_SHARED
	#define V_SDMMC2_MIEIRQ_PARA       IRQF_SHARED
	#define V_SDMMC1_CDZIRQ_PARA       IRQF_SHARED
	#define V_SDMMC2_CDZIRQ_PARA       IRQF_SHARED

	#include <chip_int.h>
	#define V_SDMMC1_MIEIRQ            E_IRQ_NFIE
	#define V_SDMMC2_MIEIRQ            E_IRQ_NFIE
	#define V_SDMMC1_CDZIRQ            E_FIQEXPH_EXT_GPIO8
	#define V_SDMMC2_CDZIRQ            E_IRQ_PM_SLEEP

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__EIFFEL)
//###########################################################################################################
	#define D_SDMMC1_IP                EV_IP_FCIE1                    //SD
	#define D_SDMMC1_PORT              EV_PORT_SD                     //Port Setting
	#define D_SDMMC1_PAD               EV_PAD5                        //REG_SD0_CONFIG2 = 1
	#define D_SDMMC1_MUTEX             EV_MUTEXS

	#define D_SDMMC2_IP                EV_IP_FCIE1                    //SD
	#define D_SDMMC2_PORT              EV_PORT_SD
	#define D_SDMMC2_PAD               EV_PAD1                        //PAD_NAND
	#define D_SDMMC2_MUTEX             EV_MUTEXS

	#define WT_POWERUP                 25 //(ms)
	#define WT_POWERON                 25 //(ms)
	#define WT_POWEROFF                25 //(ms)

	#define EN_SDMMC_DCACHE_FLUSH	   (FALSE)
	#define EN_SDMMC1_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC2_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC1_CDZIRQ_SHARD     (TRUE)
	#define EN_SDMMC2_CDZIRQ_SHARD     (TRUE)
	#define EN_SDMMC1_CDZIRQ_WAKEUP    (FALSE)
	#define EN_SDMMC2_CDZIRQ_WAKEUP    (FALSE)
	#define EV_SDMMC1_DOWN_LVL         (TRUE)
	#define EV_SDMMC2_DOWN_LVL         (TRUE)

	#define V_SDMMC1_MIEIRQ_PARA       IRQF_SHARED
	#define V_SDMMC2_MIEIRQ_PARA       IRQF_SHARED
	#define V_SDMMC1_CDZIRQ_PARA       IRQF_SHARED
	#define V_SDMMC2_CDZIRQ_PARA       IRQF_SHARED

	#include <chip_int.h>
	#define V_SDMMC1_MIEIRQ            E_IRQ_NFIE
	#define V_SDMMC2_MIEIRQ            E_IRQ_NFIE
	#define V_SDMMC1_CDZIRQ            E_FIQEXPH_EXT_GPIO8
	#define V_SDMMC2_CDZIRQ            E_IRQ_PM_SLEEP

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__EDISON)
//###########################################################################################################
	#define D_SDMMC1_IP                EV_IP_FCIE1                    //SD
	#define D_SDMMC1_PORT              EV_PORT_SD                     //Port Setting
	#define D_SDMMC1_PAD               EV_PAD5                        //REG_SD0_CONFIG2 = 1
	#define D_SDMMC1_MUTEX             EV_MUTEXS

	#define D_SDMMC2_IP                EV_IP_FCIE1                    //SD
	#define D_SDMMC2_PORT              EV_PORT_SD
	#define D_SDMMC2_PAD               EV_PAD1                        //PAD_NAND
	#define D_SDMMC2_MUTEX             EV_MUTEXS

	#define WT_POWERUP                 25 //(ms)
	#define WT_POWERON                 25 //(ms)
	#define WT_POWEROFF                25 //(ms)

	#define EN_SDMMC_DCACHE_FLUSH	   (FALSE)
	#define EN_SDMMC1_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC2_RUN_DDRSDR       (FALSE)
	#define EN_SDMMC1_CDZIRQ_SHARD     (TRUE)
	#define EN_SDMMC2_CDZIRQ_SHARD     (TRUE)
	#define EN_SDMMC1_CDZIRQ_WAKEUP    (FALSE)
	#define EN_SDMMC2_CDZIRQ_WAKEUP    (FALSE)
	#define EV_SDMMC1_DOWN_LVL         (TRUE)
	#define EV_SDMMC2_DOWN_LVL         (TRUE)

	#define V_SDMMC1_MIEIRQ_PARA       IRQF_SHARED
	#define V_SDMMC2_MIEIRQ_PARA       IRQF_SHARED
	#define V_SDMMC1_CDZIRQ_PARA       IRQF_SHARED
	#define V_SDMMC2_CDZIRQ_PARA       IRQF_SHARED

	#include <chip_int.h>
	#define V_SDMMC1_MIEIRQ            E_IRQ_NFIE
	#define V_SDMMC2_MIEIRQ            E_IRQ_NFIE
	#define V_SDMMC1_CDZIRQ            E_IRQ_PM_SLEEP
	#define V_SDMMC2_CDZIRQ            E_IRQ_PM_SLEEP

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
	#define EN_SDMMC_TCARD         (FALSE)
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

#if defined(CONFIG_MSTAR_SDMMC1_PASSLVL)
	#define V_SDMMC1_PASS_LVL      CONFIG_MSTAR_SDMMC1_PASSLVL
#elif defined(CONFIG_MS_SDMMC1_PASSLVL)
	#define V_SDMMC1_PASS_LVL      CONFIG_MS_SDMMC1_PASSLVL
#else
	#define V_SDMMC1_PASS_LVL      0
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

#if defined(CONFIG_MSTAR_SDMMC2_PASSLVL)
	#define V_SDMMC2_PASS_LVL      CONFIG_MSTAR_SDMMC2_PASSLVL
#elif defined(CONFIG_MS_SDMMC2_PASSLVL)
	#define V_SDMMC2_PASS_LVL      CONFIG_MS_SDMMC2_PASSLVL
#else
	#define V_SDMMC2_PASS_LVL      0
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
//***********************************************************************************************************
typedef enum
{
	EV_SDMMC1 = 0,
	EV_SDMMC2 = 1,

} SlotEmType;

typedef enum
{
	EV_MUTEX1  = 0,
	EV_MUTEX2  = 1,
	EV_MUTEXS  = 2,

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
	unsigned int	cdzGPIONo;      //GPIO No.
	unsigned int    pmrsaveClk;     //Power Saving Clock

	unsigned int	currClk;        //Current Clock
	unsigned int 	currRealClk;    //Current Real Clock
	unsigned char	currWidth;      //Current Bus Width
	unsigned char   currTiming;     //Current Bus Timning
	unsigned char   currPowrMode;   //Current PowerMode
	unsigned char	currBusMode;    //Current Bus Mode
	unsigned short 	currVdd;        //Current Vdd
	unsigned char   currDDR;        //Current DDR
	unsigned char   currDownLevel;  //Current Down Level
	unsigned int	currTimeoutCnt; //Current Timeout Count

	int read_only;                  //WP
	int card_det;                   //Card Detect

	/****** DMA buffer used for transmitting *******/
	u32 *dma_buffer;
	dma_addr_t dma_phy_addr;

	/***** Tasklet for hotplug ******/
	struct tasklet_struct   hotplug_tasklet;

};  /* struct ms_sdmmc_hot*/
#ifndef MMC_CARD_REMOVED 
#define MMC_CARD_REMOVED	(1<<7)		/* card has been removed */
#define mmc_card_removed(c)	((c) && ((c)->state & MMC_CARD_REMOVED))
#endif


#endif // End of __MS_SDMMC_LNX_H


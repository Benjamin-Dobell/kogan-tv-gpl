/***************************************************************************************************************
 *
 * FileName hal_card_intr.h
 *     @author jeremy.wang (2012/01/10)
 * Desc:
 * 	   This file is the header file of hal_card_intr.c.
 *
 ***************************************************************************************************************/

#ifndef __HAL_CARD_INTR_H
#define __HAL_CARD_INTR_H

#include "hal_card_regs.h"

//***********************************************************************************************************
// Config Setting (Externel)
//***********************************************************************************************************

//#######################################################################################################
#if (D_PROJECT == D_PROJECT__CB2)
//#######################################################################################################
#include <mach/irqs.h>
#define V_SDMMC1_MIEIRQ    INT_MS_SDIO
#define V_SDMMC2_MIEIRQ    INT_MS_FCIE
#define V_SDMMC1_CDZIRQ    INT_MS_SDIO
#define V_SDMMC2_CDZIRQ    INT_MS_FCIE
//#######################################################################################################
#elif (D_PROJECT == D_PROJECT__G2)
//#######################################################################################################
#include <chip_int.h>
#define V_SDMMC1_MIEIRQ    E_IRQ_SDIO
#define V_SDMMC2_MIEIRQ    E_IRQ_FCIE
#define V_SDMMC1_CDZIRQ    E_IRQ_SDIO
#define V_SDMMC2_CDZIRQ    E_IRQ_FCIE

//#######################################################################################################
#elif (D_PROJECT == D_PROJECT__A3)
//#######################################################################################################
#include <chip_int.h>
#define V_SDMMC1_MIEIRQ    E_IRQ_NFIE
#define V_SDMMC2_MIEIRQ    E_IRQ_NFIE
#if defined(CONFIG_MSTAR_SDMMC_GPIO_CDZ) && CONFIG_MSTAR_SDMMC_GPIO_CDZ
#define V_SDMMC1_CDZIRQ    E_IRQ_PM_SLEEP
#else
#define V_SDMMC1_CDZIRQ    E_IRQ_NFIE
#endif
#define V_SDMMC2_CDZIRQ    E_IRQ_NFIE

//#######################################################################################################
#endif

//***********************************************************************************************************

typedef enum
{
	EV_INT_SD		= R_SD_STS_CHG,
	EV_INT_MS   	= R_MS_STS_CHG,
	EV_INT_CF		= R_CF_STS_CHG,
	EV_INT_SM   	= R_SM_STS_CHG,
	EV_INT_XD		= R_XD_STS_CHG,

} IntCardEmType;

typedef struct
{
	U32_T slotNo;
	IPEmType eIP;

} IntSourceStruct;


BOOL_T Hal_CARD_INT_WaitMIEEvent(IPEmType eIP, U16_T u16ReqEvent, U32_T u32WaitMs);
RetEmType Hal_Card_INT_SetCardChangeEventHandler(U32_T u32Slot, IntCardEmType eCardInt, unsigned * pst_event);
void Hal_CARD_INT_WaitMIEEventCtrl(IPEmType eIP, BOOL_T bStop);

void Hal_CARD_INT_ModeCtrl(IPEmType eIP, IntCardEmType eCardInt, BOOL_T bOpen);
BOOL_T Hal_CARD_INT_ModeRunning(IPEmType eIP, IntCardEmType eCardInt);

void Hal_CARD_INT_SetMIEIntEn(IPEmType eIP, IntCardEmType eCardInt, U16_T u16RegMIEIntEN);
void Hal_CARD_INT_ClearMIEEvent(IPEmType eIP);
U16_T Hal_CARD_INT_GetMIEEvent(IPEmType eIP);

void Hal_CARD_INT_SetCardIntEn(IPEmType eIP, IntCardEmType eCardInt);
void Hal_CARD_INT_ClearCardEvent(IPEmType eIP, IntCardEmType eCardInt);


//###########################################################################################################
#if(D_OS == D_OS__LINUX)
//###########################################################################################################
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>

irqreturn_t Hal_CARD_INT(int irq, void *p_dev_id);
#if defined(CONFIG_MSTAR_SDMMC_GPIO_CDZ) && CONFIG_MSTAR_SDMMC_GPIO_CDZ
irqreturn_t Hal_CARD_CDZ_INT(int irq, void *p_dev_id);
#endif
#endif
//###########################################################################################################




#endif //End of __HAL_CARD_INTR_H

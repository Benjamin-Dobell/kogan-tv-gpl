/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
#ifndef __PMU_IRQS_H
#define __PMU_ORQS_H
/*------------------------------------------------------------------------------
    Constant
-------------------------------------------------------------------------------*/
#define PMU_IRQ_NUM_SWITCH_ON_PULSE 0
#define PMU_IRQ_NUM_SW_TRUN_ON      1
#define PMU_IRQ_NUM_KEY_RELEASED    2
#define PMU_IRQ_NUM_KEY_PRESSED     3
#define PMU_IRQ_NUM_USB_CID         4
#define PMU_IRQ_NUM_RTC_CNTDN       5
#define PMU_IRQ_NUM_RTC_ALARM       6
#define PMU_IRQ_NUM_CHARGDET        7
#define PMU_IRQ_NUM_ATHENA_INT      8
#define PMU_IRQ_NUM_TP_INT          9
#define PMU_IRQ_NUM_SD_INT          10
#define PMU_IRQ_NUM_MS_INT          11
#define PMU_IRQ_NUM_MASK_CRC_INT    12
#define PMU_IRQ_NUM_ALL             13

typedef void (*pm_irq_handler_t)(void *);

extern int request_pmu_irq(u32 pm_irq,pm_irq_handler_t handle, void* pdata);
extern int free_pmu_irq(u32 pm_irq);
#endif


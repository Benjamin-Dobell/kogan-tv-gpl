#ifndef _SLEEP_COMMON_H_
#define _SLEEP_COMMON_H_

#define CA9_SCU_ICD    0x1000       // GIC Distributor offset from SCU_BASE
#define CA9_SCU_ICC    0x100        // GIC CPU Interface offset from SCU_BASE
#define CA9_SCU_TIM64  0x200        // (64-bit) timer block offset from SCU_BASE

			// SCU_BASE offsets:
#define SCU_Ctl		 0x0
#define SCU_Config	 0x4
#define SCU_PwrStatus	 0x8
#define SCU_InvAll	 0xC
#define SCU_FiltStart	 0x40
#define SCU_FiltEnd	 0x44
#define SCU_SAC		 0x50
#define SCU_SSAC	 0x54

// Global timer offsets
#define TIM64_CntLo	 0x0
#define TIM64_CntHi	 0x4
#define TIM64_Ctl	 0x8
#define TIM64_Status	 0xC
#define TIM64_CmpLo	 0x10
#define TIM64_CmpHi	 0x14
#define TIM64_AutoInc	 0x18

#define PM_EVENT_CNT_SHIFT        11
#define PM_EVENT_CNT_BITS         5

#endif
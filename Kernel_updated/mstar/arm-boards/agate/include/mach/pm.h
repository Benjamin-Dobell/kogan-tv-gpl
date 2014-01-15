/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    PROJECT: agate

	FILE NAME: arch/arm/mach-agate/pm.h

    DESCRIPTION:
          Header file of Power Management Driver

    HISTORY:
         <Date>     <Author>    <Modification Description>
        2008/07/18  Fred Cheng  Initial Created
		2008/09/10	Fred Cheng	Add constants: USE_DDR2, USE_MOBILE_DDR,
                                USE_MOBILE_SDR and RAM_TYPE.

------------------------------------------------------------------------------*/

#ifndef __ASM_ARCH_agate_PM_H
#define __ASM_ARCH_agate_PM_H

/*------------------------------------------------------------------------------
    Constant
-------------------------------------------------------------------------------*/

/* Mask for MMU registers */
#define MMU_CTL_MASK                0x3FFF0000
#define MMU_TTB_MASK            	0x00003FFF

/* Definition for ARM CPU */
#define Mode_USR                	0x10
#define Mode_FIQ                	0x11
#define Mode_IRQ                	0x12
#define Mode_SVC                	0x13
#define Mode_ABT                	0x17
#define Mode_UND                	0x1B
#define Mode_SYS                	0x1F
#define I_Bit                   	0x80
#define F_Bit                   	0x40

/* ITCM virtual address */
#define ITCM_BASE_VIRTUAL           0xF0000000

/* IRQ and FIQ Mask */
#define INTR_IRQ_MASK_L             0xE0005630
#define INTR_IRQ_MASK_H             0xE0005634
#define INTR_FIQ_MASK_L             0xE0005600
#define INTR_FIQ_MASK_H             0xE0005604

/* Physcial Addresses of PMU registers */
#define PMU_BASE                    0xA0006940
#define PMU_CPU_CODE                PMU_BASE
#define PMU_STATE_CODE              PMU_BASE+0x00000001*4
#define PMU_RETURN_ADDR2            PMU_BASE+0x00000002*4
#define PMU_RETURN_ADDR1            PMU_BASE+0x00000003*4
#define PMU_STACK_ADDR2             PMU_BASE+0x00000004*4
#define PMU_STACK_ADDR1             PMU_BASE+0x00000005*4
#define PMU_IRQ_FORCE               PMU_BASE+0x00000006*4
#define PMU_IRQ_MASK                PMU_BASE+0x00000007*4
#define PMU_IRQ_CLR                 PMU_BASE+0x00000008*4
#define PMU_IRQ_FINAL_STATUS        PMU_BASE+0x00000009*4
#define PMU_IRQ_RAW_STATUS          PMU_BASE+0x0000000A*4
#define PMU_SW_PK_THRESHOLD         PMU_BASE+0x0000000B*4
#define PMU_TEST_MODE1              PMU_BASE+0x0000000D*4
#define PMU_TEST_MODE2              PMU_BASE+0x0000000E*4
#define PMU_DEBUG_TEMP              PMU_BASE+0x0000000F*4

/* Virtual Addresses of PMU registers */
#define PMU_BASE_VIRTUAL            0xE0006940
#define PMU_1ST_CPU_CODE_VIRTUAL    (PMU_BASE_VIRTUAL + 0x00)
#define PMU_2ND_CPU_CODE_VIRTUAL    (PMU_BASE_VIRTUAL + 0x04)
#define PMU_RETURN_ADDR23_VIRTUAL   (PMU_BASE_VIRTUAL + 0x08)
#define PMU_RETURN_ADDR01_VIRTUAL   (PMU_BASE_VIRTUAL + 0x0C)
#define PMU_STACK_ADDR23_VIRTUAL    (PMU_BASE_VIRTUAL + 0x10)
#define PMU_STACK_ADDR01_VIRTUAL    (PMU_BASE_VIRTUAL + 0x14)
#define PMU_IRQ_MASK_VIRTUAL        (PMU_BASE_VIRTUAL + 0x1C)
#define PMU_IRQ_CLR_VIRTUAL         (PMU_BASE_VIRTUAL + 0x20)
#define PMU_PWR_OK_VIRTUAL          (PMU_BASE_VIRTUAL + 0x2C)

/* PMU_IRQ_STATUS */
#define PMU_IRQ_STATUS_MASK         0x000007FF
#define PMU_IRQ_SWITCH_ON_PULSE     0x00000001
#define PMU_IRQ_SW_TRUN_ON_PUSLSE   0x00000002
#define PMU_IRQ_KEY_RELEASED        0x00000004 // ON->OFF
#define PMU_IRQ_KEY_PRESSED         0x00000008 // OFF->ON
#define PMU_IRQ_USB_CID             0x00000010
#define PMU_IRQ_RTC_CNTDN           0x00000020
#define PMU_IRQ_RTC_ALARM           0x00000040
#define PMU_IRQ_CHARGDET            0x00000080
#define PMU_IRQ_ATHENA_INT          0x00000100
#define PMU_IRQ_TP_INT              0x00000200
#define PMU_IRQ_SD_INT              0x00000400
#define PMU_IRQ_MS_INT              0x00000800
#define PMU_IRQ_MASK_CRC_INT        0x00001000

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

#define PMU_IRQ_MASK_SLEEP_U01      0xA10//MS/TP/USB MASK_ON
//If we need SD card wakeup in agate U02, we shall set MS/SD MASK OFF!
//Turn off USB wakepup : set USB&Chargdet mask ON
#define PMU_IRQ_MASK_SLEEP_U02      0xA90//0x200 for SD detection
#define PMU_IRQ_MASK_SLEEP_U03      ~(PMU_IRQ_SWITCH_ON_PULSE|PMU_IRQ_SW_TRUN_ON_PUSLSE|PMU_IRQ_KEY_RELEASED|PMU_IRQ_KEY_PRESSED|PMU_IRQ_CHARGDET|PMU_IRQ_SD_INT|PMU_IRQ_SD_INT|PMU_IRQ_USB_CID)
#define PMU_IRQ_MASK_SHUTDOWN       0x1FFF

/* Timer0's Max register address */
#define TIMER0_MAX_L_PHY    0xA0007900
#define TIMER0_MAX_H_PHY    0xA0007904
#define TIMER0_CTRL_PHY     0xa0007910
#define TIMER0_EN          (0x00000300)
/* UART0 register address */
#define UART0_IER_PHY       0xA0007605
#define UART0_IIR_FCR_PHY   0xA0007608

/* Define how many bytes in a word */
#define WORD_SIZE	                4

/* Constants used to calculate data size backup in menory */
#define SLEEPSTATE_DATA_START       0
#define SLEEPSTATE_WAKRADDR     (SLEEPSTATE_DATA_START		    )
#define SLEEPSTATE_MMUCTL       (SLEEPSTATE_WAKRADDR    + WORD_SIZE )
#define SLEEPSTATE_MMUTTB       (SLEEPSTATE_MMUCTL  	+ WORD_SIZE )
#define SLEEPSTATE_MMUDOMAIN    (SLEEPSTATE_MMUTTB  	+ WORD_SIZE )
#define SLEEPSTATE_SVC_SP       (SLEEPSTATE_MMUDOMAIN   + WORD_SIZE )
#define SLEEPSTATE_SVC_SPSR     (SLEEPSTATE_SVC_SP  	+ WORD_SIZE )
#define SLEEPSTATE_FIQ_SPSR     (SLEEPSTATE_SVC_SPSR    + WORD_SIZE )
#define SLEEPSTATE_FIQ_R8       (SLEEPSTATE_FIQ_SPSR    + WORD_SIZE )
#define SLEEPSTATE_FIQ_R9       (SLEEPSTATE_FIQ_R8  	+ WORD_SIZE )
#define SLEEPSTATE_FIQ_R10      (SLEEPSTATE_FIQ_R9  	+ WORD_SIZE )
#define SLEEPSTATE_FIQ_R11      (SLEEPSTATE_FIQ_R10 	+ WORD_SIZE )
#define SLEEPSTATE_FIQ_R12      (SLEEPSTATE_FIQ_R11 	+ WORD_SIZE )
#define SLEEPSTATE_FIQ_SP       (SLEEPSTATE_FIQ_R12 	+ WORD_SIZE )
#define SLEEPSTATE_FIQ_LR       (SLEEPSTATE_FIQ_SP  	+ WORD_SIZE )
#define SLEEPSTATE_ABT_SPSR     (SLEEPSTATE_FIQ_LR  	+ WORD_SIZE )
#define SLEEPSTATE_ABT_SP       (SLEEPSTATE_ABT_SPSR    + WORD_SIZE )
#define SLEEPSTATE_ABT_LR       (SLEEPSTATE_ABT_SP  	+ WORD_SIZE )
#define SLEEPSTATE_IRQ_SPSR     (SLEEPSTATE_ABT_LR  	+ WORD_SIZE )
#define SLEEPSTATE_IRQ_SP       (SLEEPSTATE_IRQ_SPSR    + WORD_SIZE )
#define SLEEPSTATE_IRQ_LR       (SLEEPSTATE_IRQ_SP  	+ WORD_SIZE )
#define SLEEPSTATE_UND_SPSR     (SLEEPSTATE_IRQ_LR  	+ WORD_SIZE )
#define SLEEPSTATE_UND_SP       (SLEEPSTATE_UND_SPSR    + WORD_SIZE )
#define SLEEPSTATE_UND_LR       (SLEEPSTATE_UND_SP  	+ WORD_SIZE )
#define SLEEPSTATE_SYS_SP       (SLEEPSTATE_UND_LR  	+ WORD_SIZE )
#define SLEEPSTATE_SYS_LR       (SLEEPSTATE_SYS_SP  	+ WORD_SIZE )
#define SLEEPSTATE_DATA_END     (SLEEPSTATE_SYS_LR	+ WORD_SIZE )
#define SLEEPDATA_SIZE      ((SLEEPSTATE_DATA_END - SLEEPSTATE_DATA_START) / 4)

/* Addresses of RTC registers */
#define REG_RTC_CNTL		0xA0006900
#define REG_RTC_SPARE0      0xA0006904
#define REG_RTC_LOAD_VAL_L  0xA0006908
#define REG_RTC_LOAD_VAL_H  0xA000690C
#define REG_RTC_SEC_CNT_L   0xA0006910
#define REG_RTC_SEC_CNT_H   0xA0006914
#define REG_RTC_SUB_CNT     0xA0006918
#define REG_RTC_STATUS      0xA000691C
#define REG_RTC_FREQ_CW		0xA0006924
#define REG_RTC_MATCH_VAL_L	0xA0006928
#define REG_RTC_MATCH_VAL_H	0xA000692C
#define REG_RTC_SPARE1_L	0xA0006930
#define REG_RTC_SPARE1_H	0xA0006934
#define REG_RTC_SPARE2_L	0xA0006938
#define REG_RTC_SPARE2_H	0xA000693C
#define REG_RTC_PM_MATCH_VALUE_L    0xA0006938
#define REG_RTC_PM_MATCH_VALUE_H    0xA000693C
#define REG_RTC_STORAGE		0xA0007000

/* Bit definition of Control Register */
#define BIT_RTC_RST			(1<<0)  /* software reset */
#define BIT_RTC_STORAGE_RST	(1<<1)  /* No use */
#define BIT_CNT_EN			(1<<2)  /* enable RTC count */
#define BIT_LOAD_EN			(1<<3)  /* enable load for loading value into RTC counter */
#define BIT_WRAP_EN			(1<<4)  /* wrap RTC counter when MATCH_VAL is reached */
#define BIT_READ_EN			(1<<5)	/* No use */
#define BIT_RTC_TST			(1<<6)  /* test signal for testing RTC analog */
#define BIT_RTC_IRQ_MASK	(1<<7)  /* Mask RTC interrupt */
#define BIT_ALARM_IRQ_MASK	(1<<8)  /* Mask alarm interrupt */
#define BIT_RTC_IRQ_FORCE	(1<<9)  /* Force RTC interrupt to be 1 */
#define BIT_ALARM_IRQ_FORCE	(1<<10) /* Force alarm interrupt to be 1 */
#define BIT_RTC_IRQ_CLR		(1<<11) /* Clear RTC interrupt */
#define BIT_ALARM_IRQ_CLR	(1<<12) /* Clear alarm interrupt */
#define BIT_SRAM_MODE		(1<<13) /* PM counter 16-bit mode */
#define BIT_PG_RST			(1<<14) /* reset power good register */

#ifndef BIT0
#define BIT0  (1<<0)
#define BIT1  (1<<1)
#define BIT2  (1<<2)
#define BIT3  (1<<3)
#define BIT4  (1<<4)
#define BIT5  (1<<5)
#define BIT6  (1<<6)
#define BIT7  (1<<7)
#define BIT8  (1<<8)
#define BIT9  (1<<9)
#define BIT10 (1<<10)
#define BIT11 (1<<11)
#define BIT12 (1<<12)
#define BIT13 (1<<13)
#define BIT14 (1<<14)
#define BIT15 (1<<15)
#define BIT16 (1<<16)
#define BIT17 (1<<17)
#define BIT18 (1<<18)
#define BIT19 (1<<19)
#define BIT20 (1<<20)
#define BIT21 (1<<21)
#define BIT22 (1<<22)
#define BIT23 (1<<23)
#define BIT24 (1<<24)
#define BIT25 (1<<25)
#define BIT26 (1<<26)
#define BIT27 (1<<27)
#define BIT28 (1<<28)
#define BIT29 (1<<29)
#define BIT30 (1<<30)
#define BIT31 (1<<31)
#endif

/* RTC Alarm Mode */
#define MODE_RTC			0
#define MODE_ALARM			1

/* Chiptop's UART clock */
#define CHIPTOP_UART_CLOCK  0xA0003CAC
//------------------------------------------------------------------------------
//  PWM Macros
//------------------------------------------------------------------------------
#define BAT_PWM_DUTY_D0 5
#define BAT_PWM_DUTY_D1 5

#define GPIO_BL_CONTROL                     BIT3
#define GPIO_BACKLIGHT_BOOST_EN             BIT1

#define DEFAULT_UNIT_CNT        11 //  12^-7 (1/12 X 1/1000000 sec) X 12 = 0.000001 sec
#define DEFAULT_DIV_CNT         249// 0.000001 X 250 = 0.00025 sec = 0.25 ms
#define DEFAULT_PERIOD          19 // 5 ms

/* 1. Set UNIT_CNT = 10^-7 (0.0000001 sec) X 250 = 0.000025 sec */
#define PWM_CLK_DIVIDER         (249)
/* 2. Set DIV CNT = 0.000025 X 40 => 0.001 sec (1 ms) */
#define PWM0_DIVIDER            (39)

#define SEL_PWM0_OUTPUT         (0x01)

#define PWM_REG_BASE            (0xA0006880)
#define PWM_UNIT_DIV            ( PWM_REG_BASE + 0x00000001 * 4 )

/* ### pwm0 ### */
#define PWM0_PERIOD             ( PWM_REG_BASE + 0x00000002 * 4 )
#define PWM0_DUTY               ( PWM_REG_BASE + 0x00000003 * 4 )
#define PWM0_CTRL               ( PWM_REG_BASE + 0x00000004 * 4 )

/* ### pwm1 ### */
#define PWM1_PERIOD             ( PWM_REG_BASE + 0x00000005 * 4 )
#define PWM1_DUTY               ( PWM_REG_BASE + 0x00000006 * 4 )
#define PWM1_CTRL               ( PWM_REG_BASE + 0x00000007 * 4 )

/* ### pwm2 ### */
#define PWM2_PERIOD             ( PWM_REG_BASE + 0x00000008 * 4 )
#define PWM2_DUTY               ( PWM_REG_BASE + 0x00000009 * 4 )
#define PWM2_CTRL               ( PWM_REG_BASE + 0x0000000A * 4 )

/* ### pwm3 ### */
#define PWM3_PERIOD             ( PWM_REG_BASE + 0x0000000B * 4 )
#define PWM3_DUTY               ( PWM_REG_BASE + 0x0000000C * 4 )
#define PWM3_CTRL               ( PWM_REG_BASE + 0x0000000D * 4 )

/* ### pwm4 ### */
#define PWM4_PERIOD             ( PWM_REG_BASE + 0x0000000E * 4 )
#define PWM4_DUTY               ( PWM_REG_BASE + 0x0000000F * 4 )
#define PWM4_CTRL               ( PWM_REG_BASE + 0x00000010 * 4 )

/* ### pwm5 ### */
#define PWM5_PERIOD             ( PWM_REG_BASE + 0x00000011 * 4 )
#define PWM5_DUTY               ( PWM_REG_BASE + 0x00000012 * 4 )
#define PWM5_CTRL               ( PWM_REG_BASE + 0x00000013 * 4 )

#define PWM01_HSYNC_RESET       ( PWM_REG_BASE + 0x00000014 * 4 )
#define PWM23_HSYNC_RESET       ( PWM_REG_BASE + 0x00000015 * 4 )
#define PWM45_HSYNC_RESET       ( PWM_REG_BASE + 0x00000016 * 4 )

/* PWM_UNIT_DIV */
#define UNIT_DIV_SYNC_PULSE_SEL 0x00000100
#define UNIT_DIV_RST_CNT        0x00008000

/* PWMX_CTRL */
#define PWM_CTRL_DIV_MSAK       0x000000FF
#define PWM_CTRL_POLARITY       0x00000100
#define PWM_CTRL_VBEN           0x00000200
#define PWM_CTRL_RSTEN          0x00000400
#define PWM_CTRL_DBEN           0x00000800
#define PWM_CTRL_EN             0x00001000

/* PWMXY_HSYNC_RESET */
#define PWM0_HSYNC_RESET_SEL    0x00008000
#define PWM0_HS_RST_CNT_MSAK    0x00000F00
#define PWM1_HSYNC_RESET_SEL    0x00000080
#define PWM1_HS_RST_CNT_MASK    0x0000000F
#define PWM2_HSYNC_RESET_SEL    0x00008000
#define PWM2_HS_RST_CNT_MSAK    0x00000F00
#define PWM3_HSYNC_RESET_SEL    0x00000080
#define PWM3_HS_RST_CNT_MASK    0x0000000F
#define PWM4_HSYNC_RESET_SEL    0x00008000
#define PWM4_HS_RST_CNT_MSAK    0x00000F00
#define PWM5_HSYNC_RESET_SEL    0x00000080
#define PWM5_HS_RST_CNT_MASK    0x0000000F

/* Mask */
#define PWM_UINT_DIV_MASK       0x000000FF
#define PWM_UNIT_DIV_PULSE_SEL_MASK 0x00000100
#define PWM_UNIT_DIV_RST_CNT_MASK   0x00008000
#define PWM_PERIOD_MASK         0x000003FF
#define PWM_DUTY_MASK           0x000003FF
/* 0001 0101 1111 1111, BIT9,11,12 will not be tested */
#define PWM_CTRL_MASK           0x000015FF 
#define PWM_14_MASK             0x00008F8F
#define PWM_15_MASK             0x00008F8F
#define PWM_16_MASK             0x00008F8F

/* Default */
#define PWM_UINT_DIV_DEFAULT    0x000000FF
#define PWM_PERIOD_DEFAULT      0x00000000
#define PWM_DUTY_DEFAULT        0x00000000
#define PWM_CTRL_DEFAULT        0x00000000
#define PWM_14_DEFAULT          0x00000000
#define PWM_15_DEFAULT          0x00000000
#define PWM_16_DEFAULT          0x00000000	
/******************************************************************************
 * MIIC base address , refer to CC's code
 ******************************************************************************/
#define MIIC_BASE				GET_REG_ADDR(RIU_BASE, 0x1A10)	/* 0xA0006840 */

#define MIIC_CTRL				GET_REG_ADDR(MIIC_BASE, 0x00)
#define MIIC_CLK_SEL		GET_REG_ADDR(MIIC_BASE, 0x01)
#define MIIC_WBYTE			GET_REG_ADDR(MIIC_BASE, 0x02)
#define MIIC_RBYTE			GET_REG_ADDR(MIIC_BASE, 0x03)
#define MIIC_STATUS			GET_REG_ADDR(MIIC_BASE, 0x04)

//------------------------------------------------------------------------------
//  Athena Macros
//------------------------------------------------------------------------------
#define ATHENA_SLAVE_ID     0x0

#define BIT_WRTIE           0
#define BIT_READ            1
#define BIT_CLR_NEW_DATA    (1<<1)
#define BIT_ACK_IN          (1<<3)
#define BIT_ACK_OUT         (1<<4)
#define BIT_STOP            (1<<5)
#define BIT_START           (1<<6)
#define BIT_ENABLE          (1<<7)

#define BIT_INT_STATUS      (1<<0)
#define BIT_INT_CLR         (1<<1)
#define BIT_READ_START      (1<<2)
#define BIT_I2C_RESET       (1<<3)

#define CLK_400KHZ          2   // CLK / 8
#define CLK_200KHZ          3   // CLK / 16
#define CLK_100KHZ          4   // CLK / 32

#define MAX_WAIT_LOOP   		10000

#ifndef agate_REVISION_U01
// Chip revisions
#define agate_REVISION_U01    (0x0)
#define agate_REVISION_U02    (0x1)
#define agate_REVISION_U03    (0x2)
#endif
/*------------------------------------------------------------------------------
    Macro
-------------------------------------------------------------------------------*/
/* type casting to (u32) */
#define TYPE_CAST_U32(x) ((u32)x)

/* type casting to (u8*) */
#define TYPE_CAST_U8_POINTER(x) ((u8 *)x)

#ifndef INREG32
#define INREG32(a)          agate_readl((u32)a)
#define OUTREG32(a, v)      agate_writel(v, (u32)a)
#define SETREG32(a, v)      agate_writel(agate_readl((u32)a) | (v), (u32)a)
#define CLRREG32(a, v)      agate_writel(agate_readl((u32)a) & ~(v), (u32)a)

#define INREG16(a)          agate_readw((u32)a)
#define OUTREG16(a, v)      agate_writew(v, (u32)a)
#define SETREG16(a, v)	    agate_writew(agate_readw((u32)a) | (v), (u32)a)
#define CLRREG16(a, v)      agate_writew(agate_readw((u32)a) & ~(v), (u32)a)

#define INREG8(a)           agate_readb((u32)a)
#define OUTREG8(a, v)       agate_writeb(v, (u32)a)
#define SETREG8(a, v)       agate_writeb(agate_readb((u32)a) | (v), (u32)a)
#define CLRREG8(a, v)       agate_writeb(agate_readb((u32)a) & ~(v), (u32)a)
#endif

/* data type definitions */
#ifndef U32
#define U32 unsigned long
#define S32 signed long
#define I32 long
#define U16 unsigned short
#define S16 signed short
#define I16 short
#define U8 unsigned char
#define S8 signed char
#define I8 char
#endif

#endif /* __ASM_ARCH_agate_PM_H */

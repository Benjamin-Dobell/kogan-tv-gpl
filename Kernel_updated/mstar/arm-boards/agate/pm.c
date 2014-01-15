/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    PROJECT: Columbus

	FILE NAME: arch/arm/mach-columbus/pm.c

    DESCRIPTION:
          Power Management Driver

    HISTORY:
         <Date>     <Author>    <Modification Description>
        2008/07/18  Fred Cheng  Initial Created
		2008/08/08  Fred Cheng  config ARM clock to 266 MHz after resume
        2008/09/02  Fred Cheng  Remove ITCM/DTCM init code (ITCM/DTCM are
                                initialized by IPL)
								Do not set CPU clock here because it is set by IPL
		2008/10/13	Fred Cheng	add PM stress test code

------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
#include <linux/suspend.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/rtc.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/mach/time.h>
#include <asm/mach/irq.h>
#include <asm/mach-types.h>
#include <mach/pioneer_regs.h>
#include <mach/pm.h>
#include "chip_int.h"
/*------------------------------------------------------------------------------
    Constant
-------------------------------------------------------------------------------*/

/* define debug mode */
#ifdef COLUMBUS_PM_DEBUG
#undef COLUMBUS_PM_DEBUG
#endif
#define COLUMBUS_PM_DEBUG   1
#define RIU_BASE		0x1F000000
#define ARM_266                 0x3400
#define ARM_400                 0x228E

#define GET_REG_ADDR(x, y)  (x+(y)*4)
#if defined(CONFIG_ARCH_PIONEER)
#define RIU_BASE            0x1F000000
#define UTMI_BASE_ADDR     GET_REG_ADDR(RIU_BASE, 0x1F80) /* 0xA0007E00 */
#define USBC_BASE_ADDR     GET_REG_ADDR(RIU_BASE, 0x2280) /* 0xA0008A00 */
#define OTG0_BASE_ADDR     GET_REG_ADDR(RIU_BASE, 0x2000) /* 0xA0008000 */
#elif defined(CONFIG_ARCH_COLUMBUS)
#define RIU_BASE            0x1F000000
#define UTMI_BASE_ADDR     GET_REG_ADDR(RIU_BASE, 0x0F80) /* 0xA0003E00 */
#define USBC_BASE_ADDR     GET_REG_ADDR(RIU_BASE, 0x1280) /* 0xA0004A00 */
#define OTG0_BASE_ADDR     GET_REG_ADDR(RIU_BASE, 0x1300) /* 0xA0004C00 */
#endif

#define BL_MAX_BRIGHTNESS       (100)
#define BL_DEFAULT_BRIGHTNESS   (80)


/*------------------------------------------------------------------------------
    Macro
-------------------------------------------------------------------------------*/

/* 0:not print debug message, 1:print debug message */
#if COLUMBUS_PM_DEBUG
#define DBG(x...) printk(KERN_INFO x)
#else
#define DBG(x...)
#endif

typedef void (*pm_irq_handler_t)(void *);
/*------------------------------------------------------------------------------
    Structure
-------------------------------------------------------------------------------*/
struct pm_irq_table{
    pm_irq_handler_t handler;
    u32 pm_irq_num;
    u32 pm_irq_mask;
    void *pdata;
};

struct pm_platform_data{
    struct pm_irq_table irq_tab[PMU_IRQ_NUM_ALL];
    struct tasklet_struct tasklet;
    u32 cur_irq_idx;
};
struct pm_platform_data pm_data;
/*------------------------------------------------------------------------------
    External Function Prototypes
-------------------------------------------------------------------------------*/
/*
 * Save needed data to SDRAM and PMU, and then power off CPU to
 * enter suspend state.
 */
extern void columbus_cpu_suspend(u32, u32);

/* Set SDRAM in self-refresh mode and then power off CPU */
extern void sdram_self_refresh_and_cpu_off(void);

/* used to calculate the code size of sdram_self_refresh_and_cpu_off */
extern unsigned int sdram_self_refresh_and_cpu_off_sz;

/* use RTC counter to update system time  */
extern int set_rtc_to_system_time(void);

/* begin to enter suspend state */
extern int pm_suspend(suspend_state_t state);

/* configure PMU wakeup source: RTC alarm */
extern void set_pmu_wakeup_from_rtc_alarm(void);

/* enable RTC alarm */
extern void enable_rtc_alarm(int val);

#ifdef CONFIG_MTD_MSTARSPINOR
extern int mstar_spinor_restart (void);
#endif /* CONFIG_MTD_MSTARSPINOR */
#ifdef CONFIG_MTD_NAND_COLUMBUS
extern void columbus_nand_reset(void);
#endif /* CONFIG_MTD_NAND_COLUMBUS */
/*------------------------------------------------------------------------------
    Local Function Prototypes
-------------------------------------------------------------------------------*/
/* Actually enter sleep state */
static int columbus_pm_enter(suspend_state_t state);

/* write function called by user space */
static int write_proc_pm_cmd (struct file *file, const char *buffer,
		    				  size_t count, loff_t * ppos);

/*------------------------------------------------------------------------------
    Global Variable
-------------------------------------------------------------------------------*/
/* data buffer which is used for resuming process */
static u32 sg_Suspend_Buffer[SLEEPDATA_SIZE];

/* virtual address of the data buffer which is used for resuming process */
static u32 sg_sleep_save_virtual;

/* physical address of the data buffer which is used for resuming process */
static u32 sg_sleep_save_physical;

/* global variable used to save Interrupt's registers */
static u16 sg_InterruptSave[24];

/* global variable used to save Timer0's registers */
static u16 sg_TimerSave[2];

/* global variable used to save UART0's registers */
static u16 sg_ConsoleSave[2];

static u16 sg_UTMISave[10];

/* function operation for suspend */
static struct platform_suspend_ops sg_columbus_pm_ops =
{
	.enter		= columbus_pm_enter,
	.valid		= suspend_valid_only_mem,
};

/* function operation for the interface /proc/pm */
static const struct file_operations sg_pm_fops =
{
	.owner	= THIS_MODULE,
	.write 	= write_proc_pm_cmd,
};

/*------------------------------------------------------------------------------
    Global Function
-------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Function: columbus_copy_code_to_itcm

    Description:
        Copy the binary code of sdram_self_refresh_and_cpu_off() to ITCM
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void columbus_copy_code_to_itcm(void)
{
    u8 *pu8Dest = NULL, *pu8Src = NULL;
    u16 u16Count = 0, u16Total = 0;

    /* set destination and source address */
    pu8Dest = TYPE_CAST_U8_POINTER(ITCM_BASE_VIRTUAL);
    pu8Src = TYPE_CAST_U8_POINTER(sdram_self_refresh_and_cpu_off);

    /* set copy size */
    u16Total = TYPE_CAST_U32(sdram_self_refresh_and_cpu_off_sz) + 4;

    /* begin to copy code to ITCM */
    for(u16Count=0; u16Count < u16Total; u16Count++)
    {
        pu8Dest[u16Count] = pu8Src[u16Count];
    }
}
EXPORT_SYMBOL_GPL(columbus_copy_code_to_itcm);

/*------------------------------------------------------------------------------
    Local Function
-------------------------------------------------------------------------------*/

#if 1 //MIU_TEST
u32 mem_test_U8_write(u32 start_addr, u32 count, u32 pattern, u8 flag)
{
    u32 i;
    volatile u8 * p;

    p=(volatile u8 *) (start_addr);
    for(i=0;i<count;i++,p++)
    {
    	if(flag==0)
            *(volatile u8  *)p = i&0x000000ff;
    	else
            *(volatile u8  *)p = pattern&0x000000ff;
    }

    return 1;
}
#endif
//------------------------------------------------------------------------------
//
//  Function:   PMURegDump
//
//    Description
//        Dump PMU registers for debugging
//
//    Parameters
//        None.
//
//    Return Value
//        None
//
void PMURegDump(void)
{
    U16 u32Reg0 = 0, u32Reg1 = 0, u32Reg2 = 0, u32Reg3 = 0;
    printk(KERN_INFO "1.PMU\r\n");
        u32Reg0 = INREG16(0xA0006940);
        u32Reg1 = INREG16(0xA0006944);
        u32Reg2 = INREG16(0xA0006948);
        u32Reg3 = INREG16(0xA000694C);
    //OALMSG(DUMP_ANALOG_REG, (L"    0xA0006940  %8X  %8X  %8X  %8X\r\n", u32Reg0, u32Reg1, u32Reg2, u32Reg3));
    printk(KERN_INFO "    0xA0006940  %8X  %8X  %8X  %8X\r\n", u32Reg0, u32Reg1, u32Reg2, u32Reg3);
        u32Reg0 = INREG16(0xA0006950);
        u32Reg1 = INREG16(0xA0006954);
        u32Reg2 = INREG16(0xA0006958);
        u32Reg3 = INREG16(0xA000695C);
    //OALMSG(DUMP_ANALOG_REG, (L"    0xA0006950  %8X  %8X  %8X  %8X\r\n", u32Reg0, u32Reg1, u32Reg2, u32Reg3));
    printk(KERN_INFO "    0xA0006950  %8X  %8X  %8X  %8X\r\n", u32Reg0, u32Reg1, u32Reg2, u32Reg3);
        u32Reg0 = INREG16(0xA0006960);
        u32Reg1 = INREG16(0xA0006964);
        u32Reg2 = INREG16(0xA0006968);
        u32Reg3 = INREG16(0xA000696C);
    //OALMSG(DUMP_ANALOG_REG, (L"    0xA0006960  %8X  %8X  %8X  %8X\r\n", u32Reg0, u32Reg1, u32Reg2, u32Reg3));
    printk(KERN_INFO "    0xA0006960  %8X  %8X  %8X  %8X\r\n", u32Reg0, u32Reg1, u32Reg2, u32Reg3);
        u32Reg0 = INREG16(0xA0006970);
        u32Reg1 = INREG16(0xA0006974);
        u32Reg2 = INREG16(0xA0006978);
        u32Reg3 = INREG16(0xA000697C);
    //OALMSG(DUMP_ANALOG_REG, (L"    0xA0006970  %8X  %8X  %8X  %8X\r\n", u32Reg0, u32Reg1, u32Reg2, u32Reg3));
		printk(KERN_INFO "    0xA0006970  %8X  %8X  %8X  %8X\r\n", u32Reg0, u32Reg1, u32Reg2, u32Reg3);
}
//------------------------------------------------------------------------------
//
//  Function:  OEMReadChipRevision
//
//  This inline function returns the chip revision (for OAL)
//
U32 OEMReadChipRevision()
{
    U32 u32Tmp = 0;
    u32Tmp = INREG32(0xA0003D9C);
    u32Tmp = (u32Tmp&0xFF00)>>8;
    return u32Tmp;
}

static int board_backlight_init(void)
{
    s32 result = 0;
    printk("board_backlight_init\n");
    // Athena power 20V.
    //while(!WriteI2C(g_pI2CRegs, 0x1E, 0xFA73)); //BOOST_CTRL1_DEFAULT
    //while(!WriteI2C(g_pI2CRegs, 0x20, 0x0F0)); //BOOST_CTRL2_EXT_PWM_ATHENA_U02
    //while(!WriteI2C(g_pI2CRegs, 0x20, 0x0F2)); //BOOST_CTRL2_EXT_PWM_ATHENA_U02

    CLRREG16(PIONEER_CHIPTOP_GPIO_G_OEN, GPIO_BACKLIGHT_BOOST_EN);
    SETREG16(PIONEER_CHIPTOP_GPIO_G_OUT, GPIO_BACKLIGHT_BOOST_EN);

    //Enable PWM0, BIT0 ON
    SETREG16(PIONEER_CHIPTOP_GPIO_IP_EN, 0x0001);
    //*(volatile U32*)0xA0008A00 = 0x0010;

    //SET GPIO G3 for output enable, BIT3 ON
    CLRREG16(PIONEER_CHIPTOP_GPIO_G_OEN, GPIO_BL_CONTROL);
    
    //Set UNIT_CNT = 12^-7 (1/12 X 1/100000 sec) X 240 = 0.00002 sec
    OUTREG16(PWM_UNIT_DIV, DEFAULT_UNIT_CNT);

    //Set DIV CNT = 0.00002 X 50 = 0.001 sec = 1 ms
    OUTREG16(PWM0_CTRL, DEFAULT_DIV_CNT);

    //Set Period = DEFAULT_PERIOD
    OUTREG16(PWM0_PERIOD, DEFAULT_PERIOD);

    //Analog PWM
    //3.5. Set Polarity for s/w workaround
    // 1 => 0 - L - duty - H - period
    SETREG16(PWM0_CTRL, PWM_CTRL_POLARITY);
    //4. Set Duty = BAT_PWM_DUTY_D0
    //Duty = Level is 0~20
    OUTREG16(PWM0_DUTY, BAT_PWM_DUTY_D0);
    //5. Set Polarity, need write twice
    // 1 => 0 - L - duty - H - period
    SETREG16(PWM0_CTRL, PWM_CTRL_POLARITY);
    SETREG16(PWM0_CTRL, PWM_CTRL_POLARITY);
    
	  //6. Set Period Reset mode
    CLRREG16(PWM0_CTRL, PWM_CTRL_RSTEN);
    CLRREG16(PWM0_CTRL, PWM_CTRL_VBEN);
    CLRREG16(PWM0_CTRL, PWM_CTRL_DBEN);

    //7. Enable Boost SW control

    //8. Enable PWM
    SETREG16(PWM0_CTRL, PWM_CTRL_EN);

    //while(!WriteI2C(g_pI2CRegs, 0x1E, 0xFA73));//BOOST_CTRL1_DEFAULT
    //while(!WriteI2C(g_pI2CRegs, 0x20, 0x0f2)); //BOOST_CTRL2_INT_PWM_ATHENA_U02

    return result;
}
/*------------------------------------------------------------------------------
    Function: save_cpu_registers

    Description:
        Save Interrupt's, Timer0's and UART0's registers to SDRAM
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
static void save_cpu_registers(void)
{
	  U32 u32MASK = 0;
    
	  printk(KERN_INFO "PM: save_cpu_registers\n");	
    /* Save Interrupt registers */
    sg_InterruptSave[0] = columbus_readw(FIQ_MASK_L);
    sg_InterruptSave[1] = columbus_readw(FIQ_MASK_H);
    sg_InterruptSave[2] = columbus_readw(FIQ_RAW_STATUS_L);
    sg_InterruptSave[3] = columbus_readw(FIQ_RAW_STATUS_H);
    sg_InterruptSave[4] = columbus_readw(FIQ_FINAL_STATUS_L);
    sg_InterruptSave[5] = columbus_readw(FIQ_FINAL_STATUS_H);
    sg_InterruptSave[6] = columbus_readw(FIQ_SEL_HL_TRIGGER_L);
    sg_InterruptSave[7] = columbus_readw(FIQ_SEL_HL_TRIGGER_H);
    sg_InterruptSave[8] = columbus_readw(IRQ_MASK_L);
    sg_InterruptSave[9] = columbus_readw(IRQ_MASK_H);
    sg_InterruptSave[10] = columbus_readw(IRQ_SEL_HL_TRIGGER_L);
    sg_InterruptSave[11] = columbus_readw(IRQ_SEL_HL_TRIGGER_H);
    sg_InterruptSave[12] = columbus_readw(IRQ_FIQ2IRQ_RAW_STATUS_L);
    sg_InterruptSave[13] = columbus_readw(IRQ_FIQ2IRQ_RAW_STATUS_H);
    sg_InterruptSave[14] = columbus_readw(IRQ_RAW_STATUS_L);
    sg_InterruptSave[15] = columbus_readw(IRQ_RAW_STATUS_H);
    sg_InterruptSave[16] = columbus_readw(IRQ_FIQ2IRQ_FINAL_STATUS_L);
    sg_InterruptSave[17] = columbus_readw(IRQ_FIQ2IRQ_FINAL_STATUS_H);
    sg_InterruptSave[18] = columbus_readw(IRQ_FINAL_STATUS_L);
    sg_InterruptSave[19] = columbus_readw(IRQ_FINAL_STATUS_H);
    sg_InterruptSave[20] = columbus_readw(FIQ2IRQOUT_L);
    sg_InterruptSave[21] = columbus_readw(FIQ2IRQOUT_H);
    sg_InterruptSave[22] = columbus_readw(FIQ_IDX);
    sg_InterruptSave[23] = columbus_readw(IRQ_IDX);

	  /* Save Timer0 registers */
	  sg_TimerSave[0] = columbus_readw(TIMER0_MAX_L_PHY);
	  sg_TimerSave[1] = columbus_readw(TIMER0_MAX_H_PHY);
	  
	  /* Save UART Clock */
	  sg_ConsoleSave[0] = columbus_readw(CHIPTOP_UART_CLOCK);

	  /*
	  * Timer0 must be clear before entering Suspend mode;
	  * otherwise, bootrom will fail to boot
	  */
	  columbus_writew(0, TIMER0_MAX_L_PHY);
	  columbus_writew(0, TIMER0_MAX_H_PHY);
	  
	  //Set PMU Wakeup Source
    if (COLUMBUS_REVISION_U01 == OEMReadChipRevision())
    {
    	  printk(KERN_INFO "PM: save_cpu_registers: Chip_U01\n");
        OUTREG32(PMU_IRQ_MASK, PMU_IRQ_MASK_SLEEP_U01);
    }
    else if (COLUMBUS_REVISION_U02 == OEMReadChipRevision())
    {
    	  printk(KERN_INFO "PM: save_cpu_registers: Chip_U02\n");
        // Enable RTC-alarm int
        CLRREG32(PMU_IRQ_MASK, PMU_IRQ_RTC_ALARM);
        // Mask SD int
        SETREG32(PMU_IRQ_MASK, PMU_IRQ_SD_INT);

        // Polling MS int
        while (0 == (INREG16(PMU_IRQ_FINAL_STATUS)&PMU_IRQ_MS_INT));

        // Clear MS int
        SETREG32(PMU_IRQ_CLR, PMU_IRQ_MS_INT);
        CLRREG32(PMU_IRQ_CLR, PMU_IRQ_MS_INT);

        // Polling MS int
        while ((INREG16(PMU_IRQ_FINAL_STATUS)&PMU_IRQ_MS_INT) == PMU_IRQ_MS_INT);
    }
    else
    {
    	  printk(KERN_INFO "PM: save_cpu_registers: Chip_U03\n");
        OUTREG32(PMU_IRQ_MASK, PMU_IRQ_MASK_SLEEP_U03);
        SETREG32(PMU_IRQ_MASK, PMU_IRQ_SD_INT);
    }
}

/*------------------------------------------------------------------------------
    Function: restore_cpu_registers

    Description:
        Restore Interrupt's, Timer0's and UART0's registers to CPU
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
static void restore_cpu_registers(void)
{
	  printk(KERN_INFO "PM: restore_cpu_registers\n");	
	  /* Clear Stack Addr & Return Addr in PMU */
	  //columbus_writew(0x0,PMU_RETURN_ADDR2);
	  columbus_writew(0x0,PMU_RETURN_ADDR1);
	  columbus_writew(0x0,PMU_STACK_ADDR2);
	  columbus_writew(0x0,PMU_STACK_ADDR1);

    /* Restore Interrupt registers */
    columbus_writew(sg_InterruptSave[0], FIQ_MASK_L);
    columbus_writew(sg_InterruptSave[1], FIQ_MASK_H);
    columbus_writew(sg_InterruptSave[2], FIQ_RAW_STATUS_L);
    columbus_writew(sg_InterruptSave[3], FIQ_RAW_STATUS_H);
    columbus_writew(sg_InterruptSave[4], FIQ_FINAL_STATUS_L);
    columbus_writew(sg_InterruptSave[5], FIQ_FINAL_STATUS_H);
    columbus_writew(sg_InterruptSave[6], FIQ_SEL_HL_TRIGGER_L);
    columbus_writew(sg_InterruptSave[7], FIQ_SEL_HL_TRIGGER_H);
    columbus_writew(sg_InterruptSave[8], IRQ_MASK_L);
    columbus_writew(sg_InterruptSave[9], IRQ_MASK_H);
    columbus_writew(sg_InterruptSave[10],IRQ_SEL_HL_TRIGGER_L);
    columbus_writew(sg_InterruptSave[11],IRQ_SEL_HL_TRIGGER_H);
    columbus_writew(sg_InterruptSave[12],IRQ_FIQ2IRQ_RAW_STATUS_L);
    columbus_writew(sg_InterruptSave[13],IRQ_FIQ2IRQ_RAW_STATUS_H);
    columbus_writew(sg_InterruptSave[14],IRQ_RAW_STATUS_L);
    columbus_writew(sg_InterruptSave[15],IRQ_RAW_STATUS_H);
    columbus_writew(sg_InterruptSave[16],IRQ_FIQ2IRQ_FINAL_STATUS_L);
    columbus_writew(sg_InterruptSave[17],IRQ_FIQ2IRQ_FINAL_STATUS_H);
    columbus_writew(sg_InterruptSave[18],IRQ_FINAL_STATUS_L);
    columbus_writew(sg_InterruptSave[19],IRQ_FINAL_STATUS_H);
    columbus_writew(sg_InterruptSave[20],FIQ2IRQOUT_L);
    columbus_writew(sg_InterruptSave[21],FIQ2IRQOUT_H);
    columbus_writew(sg_InterruptSave[22],FIQ_IDX);
    columbus_writew(sg_InterruptSave[23],IRQ_IDX);

	/* restore Timer0 registers */
	columbus_writew(sg_TimerSave[0],TIMER0_MAX_L_PHY);
	columbus_writew(sg_TimerSave[1],TIMER0_MAX_H_PHY);
	
    /* start timer0 */
	columbus_writew(TIMER0_EN,TIMER0_CTRL_PHY);

	/* restore UART0 Clock */
	columbus_writew(sg_ConsoleSave[0], CHIPTOP_UART_CLOCK);
	
	//8.5 Restore I2C clock (100KHz)
  OUTREG8(MIIC_CLK_SEL, CLK_100KHZ);
    
	//Restore PMU Wakeup Source
  if (COLUMBUS_REVISION_U01 == OEMReadChipRevision())
  {
      ; // Do nothing
  }
  else if (COLUMBUS_REVISION_U02 == OEMReadChipRevision())
  {
      CLRREG32(PMU_IRQ_MASK, PMU_IRQ_SD_INT|PMU_IRQ_MS_INT);
  }
  else
  {
      CLRREG32(PMU_IRQ_MASK, PMU_IRQ_SD_INT);
  }
}

/*------------------------------------------------------------------------------
    Function: columbus_pm_enter

    Description:
        Actually enter sleep state
    Input: (The arguments were used by caller to input data.)
        state - suspend state (not used)
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        0
    Remark:
        None.
-------------------------------------------------------------------------------*/
static int columbus_pm_enter(suspend_state_t state)
{
	/*
	 * turn off interrupts (FIXME: NOTE: already disabled)
	 */
	local_irq_disable();
	local_fiq_disable();

    /* Save Interrupt's, Timer0's and UART0's registers to SDRAM */
	save_cpu_registers();
    PMURegDump();

    DBG("sg_sleep_save_virtual  = 0x%X\n", sg_sleep_save_virtual);
    DBG("sg_sleep_save_physical = 0x%X\n", sg_sleep_save_physical);
//columbus_writew(0x0000,PMU_IRQ_MASK);
    /* begin to enter suspend state */
	columbus_cpu_suspend(sg_sleep_save_virtual, sg_sleep_save_physical);


    /* Restore Interrupt's, Timer0's and UART0's registers to CPU */
	restore_cpu_registers();

	/*
	 * Re-enable interrupts
	 */
	local_irq_enable();
	local_fiq_enable();

    //board_backlight_init();
    #if defined(CONFIG_RTC_DRV_COLUMBUS) && defined(CONFIG_RTC_HCTOSYS)
        /* use RTC counter to update system time  */
    //ycfan	set_rtc_to_system_time();
    #endif
    PMURegDump();
	return 0;
}
/*------------------------------------------------------------------------------
    Function: set_pmu_wakeup_from_rtc_alarm

    Description:
        set wake up source: RTC alarm
    Input:
        None
    Output:
        None
    Return:
        None
    Remark:
        None
-------------------------------------------------------------------------------*/
void set_pmu_wakeup_from_rtc_alarm(void)
{
    u16 u16Val;

    /* enable PMU interrupts: RTC_Alarm bits */
    columbus_writew(0x810, PMU_IRQ_MASK);

    mdelay(50);

    /* clear PMU IRQ register */
    columbus_writew(0x1FFF, PMU_IRQ_CLR);
    DBG("clear PMU IRQ: 0x1FFF\n");

    mdelay(50);

    /* read PMU_IRQ_FINAL_STATUS */
    u16Val = columbus_readw(PMU_IRQ_FINAL_STATUS);
    DBG("PMU_IRQ_FINAL_STATUS = 0x%x\n\n",u16Val);
}
/*------------------------------------------------------------------------------
    Function: IP_gateing

    Description:

    Input:
    Output:
    Return:
    Remark:
-------------------------------------------------------------------------------*/
void IP_gateing(void)
{
    u32 u32Reg1 = 0x0;
    u32 u32Reg2 = 0x0;

    DBG("IP_Off\n");

    //FM1
    *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0493*4)= 0x1000;

    //NFIE
    *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0F25*4)= 0x0040;

    //MPIF
    *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0F27*4)= 0x0001;

    //UART0
    //*(volatile u32 *)(RIU_BASE+0x0F2B*4)= 0x0001;
    //UART1&UART0
    //*(volatile u32 *)(RIU_BASE+0x0F2B*4)= 0x0021;

    //JPD_MIU
    //*(volatile u32 *)(RIU_BASE+0x0F2E*4)= 0x0001;

    //NFIE_MIU
    //*(volatile u32 *)(RIU_BASE+0x0F2E*4)= 0x0010;

    //MPIF_MIU
    //*(volatile u32 *)(RIU_BASE+0x0F2E*4)= 0x0080;
    u32Reg1 = *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0F2E*4);

    //JPD_MIU&NFIE_MIU&MPIF_MIU
	u32Reg1 |= 0x0091;
    *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0F2E*4)= u32Reg1;

    u32Reg2 = *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0F2E*4);
    DBG(KERN_INFO "MIU(0x0F2E): %x, %x\n", u32Reg1, u32Reg2);


    //MVOP_DC1
    //*(volatile u32 *)(RIU_BASE+0x0F30*4)= 0x0010;

    //GOP_DWIN
    //*(volatile u32 *)(RIU_BASE+0x0F30*4)= 0x1000;

    u32Reg1 = *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0F30*4);
	u32Reg1 |= 0x1100;

    //MVOP_DC_1&GOP_DWIN
    *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0F30*4)= u32Reg1;

    u32Reg2 = *(volatile u32 *)IO_ADDRESS(RIU_BASE+0x0F30*4);
    DBG("DWIN(0x0F30): %x, %x\n", u32Reg1, u32Reg2);
}

/*------------------------------------------------------------------------------
    Function: Off_utmi

    Description:

    Input:
    Output:
    Return:
    Remark:
-------------------------------------------------------------------------------*/
void Off_utmi(void)
{

    sg_UTMISave[0]=columbus_readw(UTMI_BASE_ADDR);
    sg_UTMISave[1]=columbus_readw(UTMI_BASE_ADDR + 0x20);
    sg_UTMISave[2]=columbus_readw(UTMI_BASE_ADDR + 0x0C);
    sg_UTMISave[3]=columbus_readw(UTMI_BASE_ADDR + 0x11);
    sg_UTMISave[4]=columbus_readw(UTMI_BASE_ADDR + 0x4D);
    sg_UTMISave[5]=columbus_readw(UTMI_BASE_ADDR + 0x51);
    sg_UTMISave[6]=columbus_readw(UTMI_BASE_ADDR + 0x54);
    sg_UTMISave[7]=columbus_readw(UTMI_BASE_ADDR + 0x58);
    sg_UTMISave[8]=columbus_readw(UTMI_BASE_ADDR + 0x5D);

    columbus_writew(0xFFC1, UTMI_BASE_ADDR);
    columbus_writew(0x00, UTMI_BASE_ADDR + 0x20); /* Disable UTMI interrupt */
    columbus_writew(0x0020, UTMI_BASE_ADDR + 0x0C);
    columbus_writeb(0x00, UTMI_BASE_ADDR + 0x11);
    columbus_writeb(0x00, UTMI_BASE_ADDR + 0x4D);
    columbus_writeb(0x00, UTMI_BASE_ADDR + 0x51);
    columbus_writeb(0x00, UTMI_BASE_ADDR + 0x54);
    columbus_writew(0x0000, UTMI_BASE_ADDR + 0x58);
    columbus_writeb(0x00, UTMI_BASE_ADDR + 0x5D);
    DBG("Set :3UTMI to B mode\n");
}

/*------------------------------------------------------------------------------
    Function: recover_utmi

    Description:

    Input:
    Output:
    Return:
    Remark:
-------------------------------------------------------------------------------*/
void recover_utmi(void)
{
    columbus_writew(sg_UTMISave[0], UTMI_BASE_ADDR);
    columbus_writew(sg_UTMISave[1], UTMI_BASE_ADDR + 0x20); /* Disable UTMI interrupt */
    columbus_writew(sg_UTMISave[2], UTMI_BASE_ADDR + 0x0C);
    columbus_writew(sg_UTMISave[3], UTMI_BASE_ADDR + 0x11);
    columbus_writew(sg_UTMISave[4], UTMI_BASE_ADDR + 0x4D);
    columbus_writew(sg_UTMISave[5], UTMI_BASE_ADDR + 0x51);
    columbus_writew(sg_UTMISave[6], UTMI_BASE_ADDR + 0x54);
    columbus_writew(sg_UTMISave[7], UTMI_BASE_ADDR + 0x58);
    columbus_writew(sg_UTMISave[8], UTMI_BASE_ADDR + 0x5D);
}
/*------------------------------------------------------------------------------
    Function: write_proc_pm_cmd

    Description:
        write function called by user space
    Input: (The arguments were used by caller to input data.)
        *file - The pointer of file descriptor. (not used here)
        *buffer - The command inputted from user space
        count - The buffer size of input command
        *ppos - (not used here)
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        0 if successful
        otherwise if failed
    Remark:
        None.
-------------------------------------------------------------------------------*/
static int write_proc_pm_cmd (struct file *file, const char *buffer,
		    				  size_t count, loff_t * ppos)
{
	char *buf = NULL, *tok = NULL, **tokptrs = 0;
	char *whitespace = " \t\r\n";
	int ret = 0, ntoks = 0;

    /* check file pointer is not NULL */
    if ( !file )
    {
        return (-EFAULT);
    }

    /* check buffer is not NULL */
    if ( !buffer )
    {
        return (-EFAULT);
    }

    /* check ppos is not NULL */
    if ( !ppos )
    {
        return (-EFAULT);
    }

    /* check buffer size is not 0 */
	if (count == 0)
	{
		return 0;
    }

    /* allocate a buffer */
	if (!(buf = kmalloc(count + 1, GFP_KERNEL)))
	{
		return -ENOMEM;
    }

    /* copy command */
	if (copy_from_user(buf, buffer, count))
	{
		ret = -EFAULT;
		goto OUT0;
	}

	buf[count] = '\0';

	if (!(tokptrs = (char **)__get_free_page(GFP_KERNEL)))
	{
		ret = -ENOMEM;
		goto OUT1;
	}

	ret = -EINVAL;
	ntoks = 0;
	do
	{
		buf = buf + strspn(buf, whitespace);
		tok = strsep(&buf, whitespace);
		if (*tok == '\0')
		{
			if (ntoks == 0)
			{
				ret = 0;
				goto OUT1;
			}
			else
			{
				break;
		    }
		}

		if (ntoks == (PAGE_SIZE / sizeof(char **)))
		{
			goto OUT1;
	    }

		tokptrs[ntoks++] = tok;

	} while(buf);

	if (ntoks == 1)
	{
		if (strcmp(tokptrs[0], "suspend") == 0)
		{
			ret = pm_suspend(PM_SUSPEND_MEM);
			//while(1) ;
		}
    else if (strcmp(tokptrs[0], "stress") == 0)
    {
			/*
			 * Note:
			 *     This is a temporarily solution to implement power-saving
			 * mode. In order to implement device's suspend and resume 
			 * function, we reset all system by trigger watch dog when 
			 * after system wakes up.
			 *									2009/9/10, chhuang
			 */
			 printk("pm enter stress mode......\n");
			 //            enable_rtc_alarm(3);
			 //            set_pmu_wakeup_from_rtc_alarm();
			 //            msleep(100);
			 			//IP_gateing();
			 #if 1
			 			/* We only turn on RTC alarm & power key press */
			 			columbus_writew(0x17BB, PMU_IRQ_MASK);
			 			columbus_writew(0x1FFF, PMU_IRQ_CLR);
			 //printk("irq mask=%x, irq status=%x\n", columbus_readw(PMU_IRQ_MASK),
			 //	columbus_readw(PMU_IRQ_FINAL_STATUS));
			 #endif
			             ret = pm_suspend(PM_SUSPEND_MEM);
			 printk("wake up....\n");
			 
			 {
			 	/* Reboot system through watch dog after waking up */
			 	columbus_writew(0xffff, 0xA0007800+0x31*4);
			 	columbus_writew(0x55aa, 0xA0007800+0x30*4);
			 }

    }
    else if (strcmp(tokptrs[0], "12M") == 0)
    {
        IP_gateing();
        //Off_utmi();
        DBG("Down Speed 12M.\n");
        *(volatile u32*)IO_ADDRESS(RIU_BASE+0x3C88) &= ~0x80;   /*ChipTop:0x22,ARM Clock setting*/
        //*(volatile int *)IO_ADDRESS(RIU_BASE+0x1342*4) = 0x01;
        //*(volatile u32*)IO_ADDRESS(RIU_BASE+0x3C44) &= ~0x1;  /*ChipTop:0x11,MCU Clock setting*/
        ret = 0;
    }
    else if (strcmp(tokptrs[0], "266M") == 0)
    {
        printk("Up Speed 266M.\n");
        *(volatile u32*)IO_ADDRESS(RIU_BASE+0x4D10) = ARM_266;
        udelay(10);
        *(volatile u32*)IO_ADDRESS(RIU_BASE+0x3C88) |= 0x80;    /*ChipTop:0x22,ARM Clock setting*/
        //*(volatile u32*)IO_ADDRESS(RIU_BASE+0x3C44) |= 0x1;
        //recover_utmi();
        mdelay(10);
        ret = 0;
#ifdef CONFIG_MTD_MSTARSPINOR
        mstar_spinor_restart();
#endif /* CONFIG_MTD_MSTARSPINOR */
#ifdef CONFIG_MTD_NAND_COLUMBUS
            columbus_nand_reset();
#endif /* CONFIG_MTD_NAND_COLUMBUS */

			  /*
			   * We added the following patch to disable background noise and 
			   * bad click-tone after exiting DTV mode. 
			   * (8051 modifies audio setting and doesn't recover it!)
			   *
			   *											- by chhuang, 10/23/2009
			   */
			  {
			  	void aud_power_on_earphone(unsigned long bOn);
			  	void aud_init(void);
			  	aud_power_on_earphone(1);
			  	aud_power_on_earphone(0);
			  	aud_init();
			  }
    }
    else if (strcmp(tokptrs[0], "poweroff") == 0)
    {
        DBG("power off.\n");
        *(volatile int *)IO_ADDRESS(0xA0006940) = 0xA5;
        *(volatile int *)IO_ADDRESS(0xA0006944) = 0x3C;
        ret = 0;
    }
#if 0
    else if (strcmp(tokptrs[0], "off") == 0)
    {
        DBG("Turn off ARM.\n");
        *(volatile int *)IO_ADDRESS(RIU_BASE+0x1342*4) = 0x01;
        ret = 0;
    }
#endif
    else if (strcmp(tokptrs[0], "ipoff") == 0)
    {
        //DBG("IP clock gating.\n");
        IP_gateing();
        ret = 0;
    }
    else if (strcmp(tokptrs[0], "utmioff") == 0)
    {
        Off_utmi();
        ret = 0;
    }
    else
    {
    	printk(KERN_WARNING "Command %s is not supported!\n", tokptrs[0]);
    }
	}

OUT1:
	free_page(TYPE_CAST_U32(tokptrs));
OUT0:
	kfree(buf);
//while(1) ;
    return ret;
}


/*------------------------------------------------------------------------------
    Function: pm_tasklet

    Description:
        excute per PMU irq handler.
    Input:
        *data - PMU device data
-------------------------------------------------------------------------------*/

static void pm_tasklet(unsigned long data)
{
    struct pm_platform_data *ptable = (struct pm_platform_data *)data;
    DBG("fun: %s\n",__FUNCTION__);
    DBG("Current idx = %d\n",ptable->cur_irq_idx);
    ptable->irq_tab[ptable->cur_irq_idx].handler(ptable->irq_tab[ptable->cur_irq_idx].pdata);
}

/*------------------------------------------------------------------------------
    Function: columbus_pmu_irq

    Description:
        Interrupt dispatch of PMU irqs.
    Input:
        irq - PMU IRQ number
        *data - IRQ private data.
    Output:
        None
    Return:
        IRQ_HANDLED
    Remark:
        None
-------------------------------------------------------------------------------*/
static irqreturn_t columbus_pmu_irq(int irq, void *pdata)
{
    u32 u32Val = 0;
    u8 irq_idx,i=0;
    struct pm_platform_data *ptable = (struct pm_platform_data *)pdata;

    DBG("fun: %s\n",__FUNCTION__);
    u32Val = INREG32(PMU_IRQ_FINAL_STATUS);

    SETREG32(PMU_IRQ_CLR,u32Val);

    while(i<PMU_IRQ_NUM_ALL)
    {
        for(irq_idx=i;irq_idx<PMU_IRQ_NUM_ALL;irq_idx++)
            if(((u32Val>>irq_idx)&0x00000001))
                break;

        i=irq_idx+1;

        DBG("irq_idx = %d\n",irq_idx);
        if(irq_idx < PMU_IRQ_NUM_ALL)
        {
            DBG("ptable->irq_tab[%d].pm_irq_num =%d\n",irq_idx,ptable->irq_tab[irq_idx].pm_irq_num);

            if (ptable->irq_tab[irq_idx].pm_irq_num == irq_idx && ptable->irq_tab[irq_idx].handler != NULL)
            {
                ptable->cur_irq_idx = irq_idx;
                tasklet_schedule(&ptable->tasklet);
            }
        }
    }
    return IRQ_HANDLED;
}
/*------------------------------------------------------------------------------
    Function: request_pmu_irq

    Description:
        Request PMU IRQ
    Input:
-------------------------------------------------------------------------------*/

int request_pmu_irq(u32 pm_irq,pm_irq_handler_t handle, void* pdata)
{
    DBG("pm_irq = %x\n",pm_irq);
    if (pm_irq < PMU_IRQ_NUM_ALL)
    {
        pm_data.irq_tab[pm_irq].pm_irq_num=pm_irq;
        pm_data.irq_tab[pm_irq].handler = handle;
        pm_data.irq_tab[pm_irq].pdata = pdata;
        pm_data.irq_tab[pm_irq].pm_irq_mask = (1<<pm_irq);

        DBG("pm_data.irq_tab[pm_irq].pm_irq_mask = %x\n",pm_data.irq_tab[pm_irq].pm_irq_mask);
        CLRREG32(PMU_IRQ_MASK,pm_data.irq_tab[pm_irq].pm_irq_mask);
        return 0;
    }
    return 1;
}

/*------------------------------------------------------------------------------
    Function: free_pmu_irq

    Description:
        Free PMU IRQ
    Input:
-------------------------------------------------------------------------------*/
int free_pmu_irq(u32 pm_irq)
{
    if (pm_irq < PMU_IRQ_NUM_ALL)
    {
        pm_data.irq_tab[pm_irq].pm_irq_num=0xFF;
        pm_data.irq_tab[pm_irq].handler = NULL;
        pm_data.irq_tab[pm_irq].pdata = NULL;
        pm_data.irq_tab[pm_irq].pm_irq_mask = 0xFF;

        SETREG32(PMU_IRQ_MASK,(1<<pm_irq));
        SETREG32(PMU_IRQ_CLR,(1<<pm_irq));
        return 0;
    }
    return 1;
}
/*------------------------------------------------------------------------------
    Function: columbus_pm_init

    Description:
        init function of power management
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        0
    Remark:
        None.
-------------------------------------------------------------------------------*/
static int __init columbus_pm_init(void)
{
    struct proc_dir_entry *pEntry = NULL;
    u8 i;

    DBG("fun: %s\n",__FUNCTION__);

    /* get address of array "sg_Suspend_Buffer", we do type cast here */
	sg_sleep_save_virtual 	= TYPE_CAST_U32(sg_Suspend_Buffer);

	/* physcial address of sg_sleep_save_virtual */
	sg_sleep_save_physical = __virt_to_phys(sg_sleep_save_virtual);

    /* set operation function of suspend */
	suspend_set_ops(&sg_columbus_pm_ops);

    /* create virtual file "/proc/pm" */
    pEntry = create_proc_entry("pm",S_IWUGO,NULL);
	if (pEntry)
	{
		pEntry->proc_fops = &sg_pm_fops;
	}
	else
	{
		DBG("Attempt to create /proc/pm failed\n");
	}

    for(i=0;i<PMU_IRQ_NUM_ALL;i++)
        (void)free_pmu_irq(i);

    tasklet_init(&pm_data.tasklet, pm_tasklet, (unsigned long)&pm_data);

    if (request_irq(INT_COLUMBUS_PM, columbus_pmu_irq, IRQF_SHARED, "pmu", &pm_data))
	{
	    DBG("register PMU interrupt failed\n");
	}
	return 0;
}

__initcall(columbus_pm_init);


/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    PROJECT: Columbus

	FILE NAME: arch/arm/mach-columbus/sleep.S

    DESCRIPTION:
          Assembly code of Power Management Driver

    HISTORY:
         <Date>     <Author>    <Modification Description>
        2008/07/18  Fred Cheng  Initial Created
		2008/08/08  Fred Cheng  Update MIU config sequence for QC board and
                                config ITCM/DTCM
        2008/09/02  Fred Cheng  Remove ITCM/DTCM init code (ITCM/DTCM are
                                initialized by IPL)
        2008/09/09  Fred Cheng  Add Mobile SDR and DDR refresh mode

------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
#include <linux/linkage.h>
#include <asm/assembler.h>
#include "pm.h"


/*------------------------------------------------------------------------------
    Global Function
-------------------------------------------------------------------------------*/

    .text

/*------------------------------------------------------------------------------
    Function: columbus_cpu_suspend

    Description:
        Save needed data to SDRAM and PMU.
        Then, power off CPU to enter suspend state.
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
ENTRY(columbus_cpu_suspend)

        /* 1. Push SVC state onto our stack */
	    stmdb   sp!, {r4-r12}
	    stmdb   sp!, {lr}

        /* save physcial and virtual addrsses where data is saved in SDRAM */
        mov     r4, r0
        mov     r5, r1

        /* Save MMU & CPU Register to RAM */
        mov     r3, r4

        ldr     r2, =AWAKE_ADDR         /* store Virtual return address */
        str     r2, [r3], #4

        mrc     p15, 0, r2, c1, c0, 0   /* load r2 with MMU Control */
        ldr     r0, =MMU_CTL_MASK       /* mask off the undefined bits */
        bic     r2, r2, r0
        str     r2, [r3], #4            /* store MMU Control data */

        mrc     p15, 0, r2, c2, c0, 0   /* load r2 with TTB address. */
        ldr     r0, =MMU_TTB_MASK       /* mask off the undefined bits */
        bic     r2, r2, r0
        str     r2, [r3], #4            /* store TTB address */

        mrc     p15, 0, r2, c3, c0, 0   /* load r2 with domain access control. */
        str     r2, [r3], #4            /* store domain access control */

        str     sp, [r3], #4            /* store SVC stack pointer */

        mrs     r2, spsr
        str     r2, [r3], #4            /* store SVC status register */

        /* Enter FIQ mode, no interrupts */
        mov     r1, #Mode_FIQ
        orr     r1, r1, #I_Bit
        orr     r1, r1, #F_Bit
        msr     cpsr, r1
        mrs     r2, spsr
        stmia   r3!, {r2, r8-r12, sp, lr}

        /* Enter ABT mode, no interrupts */
	    mov     r1, #Mode_ABT
        orr     r1, r1, #I_Bit
        orr     r1, r1, #F_Bit
	    msr     cpsr, r1
	    mrs		r0, spsr
	    stmia   r3!, {r0, sp, lr}

	    /* Enter IRQ mode, no interrupts */
	    mov     r1, #Mode_IRQ
        orr     r1, r1, #I_Bit
        orr     r1, r1, #F_Bit
	    msr     cpsr, r1
	    mrs     r0, spsr
	    stmia   r3!, {r0, sp, lr}

	    /* Enter UND mode, no interrupts */
	    mov     r1, #Mode_UND
        orr     r1, r1, #I_Bit
        orr     r1, r1, #F_Bit
	    msr     cpsr, r1
	    mrs     r0, spsr
	    stmia   r3!, {r0, sp, lr}

	    /* Enter SYS mode, no interrupts */
	    mov     r1, #Mode_SYS
        orr     r1, r1, #I_Bit
        orr     r1, r1, #F_Bit
	    msr     cpsr, r1
	    stmia   r3!, {sp, lr}

	    /* Back to SVC mode, no interrupts */
	    mov     r1, #Mode_SVC
        orr     r1, r1, #I_Bit
        orr     r1, r1, #F_Bit
	    msr     cpsr, r1

        /* do Checksum on the Sleepdata */
	    mov     r3, r4              /* get pointer to SLEEPDATA */
	    mov     r2, #0
	    ldr     r0, =SLEEPDATA_SIZE /* get size of data structure (in words) */
30:
	    ldr     r1, [r3], #4
	    and     r1, r1, #0x1
	    mov     r1, r1, LSL #31
	    orr     r1, r1, r1, LSR #1
	    add     r2, r2, r1
	    subs    r0, r0, #1
	    bne     30b

	    /* 4. Store checksum in PMU Return Address */
        ldr     r3, =PMU_RETURN_ADDR01_VIRTUAL
        strh    r2, [r3]
        mov     r2, r2, LSR #16
        ldr     r3, =PMU_RETURN_ADDR23_VIRTUAL
        strh    r2, [r3]

        /* 5. Disable Interrupt */
        ldr     r2, =0xFFFF
        ldr     r3, =INTR_IRQ_MASK_L   /* virtual irq mask lo addresss */
        strh    r2, [r3]
        ldr     r3, =INTR_IRQ_MASK_H   /* virtual irq mask high addresss */
        strh    r2, [r3]
        ldr     r3, =INTR_FIQ_MASK_L   /* virtual fiq mask lo addresss */
        strh    r2, [r3]
        ldr     r3, =INTR_FIQ_MASK_H   /* virtual fiq mask high addresss */
        strh    r2, [r3]

        //7. Clear all interrupts triggered before entering sleep mode
		ldr     r2, =0x1FFF
        ldr     r3, =PMU_IRQ_CLR_VIRTUAL                    // PMU_IRQ_CLR = 0x00001000
        strh    r2, [r3]

        /* 8. Save Stack Address */
        mov     r2, r5
        ldr     r3, =PMU_STACK_ADDR01_VIRTUAL
	    strh    r2, [r3]
	    mov     r2, r5, LSR #16
	    ldr     r3, =PMU_STACK_ADDR23_VIRTUAL
	    strh    r2, [r3]

        /* Copy code to item */
        bl      columbus_copy_code_to_itcm

__armv5tej_mmu_cache_flush:
1:		mrc	p15, 0, r15, c7, c14, 3	@ test,clean,invalidate D cache
		bne	1b
		mcr	p15, 0, r0, c7, c5, 0	@ flush I cache
		mcr	p15, 0, r0, c7, c10, 4	@ drain WB

        /* Jump to ITCM */
        ldr     r0, =ITCM_BASE_VIRTUAL
        mov     pc, r0

AWAKE_ADDR:
        /* get SLEEPDATA_BASE_PHYSICAL from PMU stack register */
        ldr     r0, =PMU_STACK_ADDR01_VIRTUAL
        ldrh    r1, [r0]
        ldr     r0, =PMU_STACK_ADDR23_VIRTUAL
        ldrh    r2, [r0]
        mov     r2, r2, LSL #16
        add     r0, r1, r2

        /* translate SLEEPDATA_BASE_PHYSICAL to SLEEPDATA_BASE_VIRTUAL */
        add     r4, r0, #0xB6000000
        /*sub     r4, r0, #0x0a000000
        mov     r0, r4
        add     r4, r0, #0xc0000000
        */
        /* Restore CPU status */
        mov     r3, r4  	/* r3 = SLEEPDATA_BASE_VIRTUAL */

		/* Enter FIQ mode, no interrupts - also FIQ */
	    add     r2, r3, #SLEEPSTATE_FIQ_SPSR
	    mov     r1, #Mode_FIQ
        orr     r1, r1, #I_Bit
        orr     r1, r1, #F_Bit
	    msr     cpsr_c, r1
	    ldr     r0,  [r2], #4
	    msr     spsr_c, r0
	    ldr     r8,  [r2], #4
	    ldr     r9,  [r2], #4
	    ldr     r10, [r2], #4
	    ldr     r11, [r2], #4
	    ldr     r12, [r2], #4
	    ldr     sp,  [r2], #4
	    ldr     lr,  [r2], #4

	    /* Enter ABT mode, no interrupts */
	    mov     r1, #Mode_ABT
        orr     r1, r1, #I_Bit
	    msr     cpsr_c, r1
	    ldr     r0, [r2], #4
	    msr     spsr_c, r0
	    ldr     sp, [r2], #4
	    ldr     lr, [r2], #4

	    /* Enter IRQ mode, no interrupts */
	    mov     r1, #Mode_IRQ
        orr     r1, r1, #I_Bit
	    msr     cpsr_c, r1
	    ldr     r0, [r2], #4
	    msr     spsr_c, r0
	    ldr     sp, [r2], #4
	    ldr     lr, [r2], #4

	    /* Enter UND mode, no interrupts */
	    mov     r1, #Mode_UND
        orr     r1, r1, #I_Bit
	    msr     cpsr_c, r1
	    ldr     r0, [r2], #4
	    msr     spsr_c, r0
	    ldr     sp, [r2], #4
	    ldr     lr, [r2], #4

	    /* Enter SYS mode, no interrupts */
	    mov     r1, #Mode_SYS
        orr     r1, r1, #I_Bit
	    msr     cpsr_c, r1
	    ldr     sp, [r2], #4
	    ldr     lr, [r2]

	    /* Enter SVC mode, no interrupts - FIQ is available */
	    mov     r1, #Mode_SVC
        orr     r1, r1, #I_Bit
	    msr     cpsr_c, r1
	    ldr     r0, [r3, #SLEEPSTATE_SVC_SPSR]
	    msr     spsr_c, r0

        /* Recover Last mode's REG's */
	    mov     r3, r4
	    ldr     sp, [r3, #SLEEPSTATE_SVC_SP]
	    ldr     lr, [sp], #4
	    ldmia   sp!, {r4-r12}

        /* go back to caller of this function */
	    mov     pc, lr

/*------------------------------------------------------------------------------
    Function: columbus_cpu_suspend

    Description:
        Set SDRAM in self-refresh mode and then power off CPU.
        This function is run in ITCM.
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
ENTRY(sdram_self_refresh_and_cpu_off)

        /* Set SDRAM in Self-Refresh mode */
		mov r0, #0xE0000000

#if defined(CONFIG_DDR2_REFRESH)

        //turn off dynamic cke control
        /* DDR2 : [0901] = 0x0541 */
        /* SDR  : [0901] = 0x0311 */
        /* DDR  : [0901] = 0x05C1 */
	    mov     r1, #0x09
	    mov     r2, #0x01
	    add     r1, r2, r1, LSL #8
	    add     r2, r0, r1, LSL #2
	    mov     r1, #0x05
	    mov     r3, #0x41
	    add     r3, r3, r1, LSL #8
	    str     r3, [r2]

	    /* Mask all groups off and only keep client00 on*/
	    mov     r1, #0x09
	    mov     r2, #0x23
	    add     r1, r2, r1, LSL #8
	    add     r2, r0, r1, LSL #2
	    mov     r1, #0xff
	    mov     r3, #0xfe
	    add     r3, r3, r1, LSL #8
	    str     r3, [r2]

	    /* mask other request */
	    mov r1, #0x09
	    mov r2, #0x33
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r3, #0xff
	    mov r1, #0xff
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

	    /* force d2a_fifo_en on */
	    /* Make sure data in fifo are flushed */
	    mov r1, #0x09
	    mov r2, #0x1c
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r3, #0x02
	    str r3, [r2]

        //Wait for 200ns
	    mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

        //Turn on single step[8]
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0100
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    /* Trigger precharge all */
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0300
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
	    mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    /* Trigger auto refresh */
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0500
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
	    mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Turn off command request
	    //We turn off client00 here
	    mov r1, #0x0900
	    add r1, r1, #0x23
	    add r2, r0, r1, LSL #2
	    mov r1, #0xff
	    mov r3, #0xff
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

        //Wait for 200ns
	    mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //turn on self-refresh power down, set bit1 = 1
        //DDR2 : [0901] = 0x0543
        //SDR  : [0901] = 0x0313
        //DDR  : [0901] = 0x05c3
	    mov r1, #0x0900
	    add r1, r1, #0x01
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0500
	    add r3, r3, #0x43
	    str r3, [r2]

        //force d2a_fifo_en off
	    mov r1, #0x0900
	    add r1, r1, #0x1c
	    add r2, r0, r1, LSL #2
	    mov r3, #0x00
	    str r3, [r2]

        //Wait for 10000ns
	    mov r1, #1000
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

#elif defined(CONFIG_DDR_REFRESH)

		//turn off dynamic cke control
        //DDR2 : [0901] = 0x0541
        //SDR  : [0901] = 0x0311
        //DDR  : [0901] = 0x05c1
	    mov r1, #0x09
	    mov r2, #0x01
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r1, #0x05
	    mov r3, #0xc1
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

	    //Mask request
	    //Mask all groups off and only keep client00 on
	    mov r1, #0x09
	    mov r2, #0x23
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r1, #0xff
	    mov r3, #0xfe
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

		//mask other request
	    mov r1, #0x09
	    mov r2, #0x33
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r1, #0xff
	    mov r3, #0xff
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

        //auto ref off
	    mov r1, #0x09
	    mov r2, #0x03
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0600
	    str r3, [r2]

	    //Force d2a_fifo_en on
	    //Make sure data in fifo are flushed
	    mov r1, #0x09
	    mov r2, #0x1c
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r3, #0x32
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Turn on single step[8]
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0100
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Trigger precharge all
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0300
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Trigger auto refresh
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0500
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

        //turn off signle step
	    ldr r1, =0x0900
	    add r2, r0, r1, LSL #2
	    ldr r3, =0x1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Turn off command request
	    //We turn off client00 here
	    mov r1, #0x0900
	    add r1, r1, #0x23
	    add r2, r0, r1, LSL #2
	    mov r1, #0xff
	    mov r3, #0xff
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //turn on self-refresh power down, set bit1 = 1
        //DDR2 : [0901] = 0x0543
        //SDR  : [0901] = 0x0313
        //DDR  : [0901] = 0x05c3
	    mov r1, #0x0900
	    add r1, r1, #0x01
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0500
	    add r3, r3, #0xc3
	    str r3, [r2]

	    //force d2a_fifo_en off
	    mov r1, #0x0900
	    add r1, r1, #0x1c
	    add r2, r0, r1, LSL #2
	    mov r3, #0x30
	    str r3, [r2]

        //Wait for 10000ns
	    mov r1, #1000
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

#elif defined(CONFIG_SDR_REFRESH)

		//turn off dynamic cke control
        //DDR2 : [0901] = 0x0541
        //SDR  : [0901] = 0x0311
        //DDR  : [0901] = 0x05c1
	    mov r1, #0x09
	    mov r2, #0x01
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r1, #0x03
	    mov r3, #0x11
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

	    //Mask request
	    //Mask all groups off and only keep client00 on
	    mov r1, #0x09
	    mov r2, #0x23
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r1, #0xff
	    mov r3, #0xfe
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

		//mask other request
	    mov r1, #0x09
	    mov r2, #0x33
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r1, #0xff
	    mov r3, #0xff
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

        //auto ref off
	    mov r1, #0x09
	    mov r2, #0x03
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0600
	    str r3, [r2]

	    //Force d2a_fifo_en on
	    //Make sure data in fifo are flushed
	    mov r1, #0x09
	    mov r2, #0x1c
	    add r1, r2, r1, LSL #8
	    add r2, r0, r1, LSL #2
	    mov r3, #0x22
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Turn on single step[8]
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0100
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Trigger precharge all
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0300
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Trigger auto refresh
	    mov r1, #0x0900
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0500
	    add r3, r3, #1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

        //turn off signle step
	    ldr r1, =0x0900
	    add r2, r0, r1, LSL #2
	    ldr r3, =0x1
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //Turn off command request
	    //We turn off client00 here
	    mov r1, #0x0900
	    add r1, r1, #0x23
	    add r2, r0, r1, LSL #2
	    mov r1, #0xff
	    mov r3, #0xff
	    add r3, r3, r1, LSL #8
	    str r3, [r2]

        //Wait for 200ns
        mov r1, #20
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

	    //turn on self-refresh power down, set bit1 = 1
        //DDR2 : [0901] = 0x0543
        //SDR  : [0901] = 0x0313
        //DDR  : [0901] = 0x05c3
	    mov r1, #0x0900
	    add r1, r1, #0x01
	    add r2, r0, r1, LSL #2
	    mov r3, #0x0300
	    add r3, r3, #0x13
	    str r3, [r2]

	    //force d2a_fifo_en off
	    mov r1, #0x0900
	    add r1, r1, #0x1c
	    add r2, r0, r1, LSL #2
	    mov r3, #0x20
	    str r3, [r2]

        //Wait for 10000ns
	    mov r1, #1000
1:
	    nop
	    sub r1, r1, #1
	    cmp r1, #0
	    bne 1b

#else

#error "Should not be here"

#endif

CPU_POWER_OFF:

		/* Set PWR_OK low */
	    mov     r2, #0xE000
	    mov     r1, #0x6000
	    add     r1, r1, #0x0900
	    add     r1, r1, #0x0060
	    add     r1, r1, #0x000C
        add 	r3, r1, r2, LSL #16
        ldrb    r4, [r3]
	    and     r4, r4, #0xFFFFFFF7
	    strb    r4, [r3]

        /* Set PMU to sleep mode */
        mov     r4, #0xA5

        /* PMU_1ST_CPU_CODE = 0xE0006940 */
	    mov     r2, #0xE000
	    mov     r1, #0x6000
	    add     r1, r1, #0x0900
	    add     r1, r1, #0x0040
	    add     r1, r1, #0x0000             /* r1 = 0x6940 */
	    add 	r3, r1, r2, LSL #16         /* r3 = 0xE0006940 */
	    strb    r4, [r3]                    /* mem[PMU_1ST_CPU_CODE] = 0xA5 */

        /* PMU_2ND_CPU_CODE = 0xE0006944 */
	    mov     r2, #0xE000
	    mov     r1, #0x6000
	    add     r1, r1, #0x0900
	    add     r1, r1, #0x0040
	    add     r1, r1, #0x0004
	    add 	r3, r1, r2, LSL #16         /* r3 = 0xE0006944 */
	    strb    r4, [r3]                    /* mem[PMU_2ND_CPU_CODE] = 0xA5 */

        /* Set ARM to WFI mode */
        msr cpsr_f, #0xd3
        mov r0, #0x0
        mcr p15, 0, r0, c7, c0, 4

/*------------------------------------------------------------------------------
    Function: sdram_self_refresh_and_cpu_off_sz

    Description:
        This function is only used to calculate the code size of
        sdram_self_refresh_and_cpu_off.
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
ENTRY(sdram_self_refresh_and_cpu_off_sz)
	.word	. - sdram_self_refresh_and_cpu_off

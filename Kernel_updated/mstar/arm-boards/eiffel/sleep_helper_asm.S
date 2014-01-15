#include <linux/linkage.h>
#include <asm/assembler.h>
#include <mach/pm.h>
#include "sleep_common.h"

// Aliases for mode encodings - do not change
MODE_USR        =     0x10
MODE_FIQ        =     0x11
MODE_IRQ        =     0x12
MODE_SVC        =     0x13
MODE_ABT        =     0x17
MODE_UND        =     0x1B
MODE_SYS        =     0x1F

MODE_MON        =     0x16  // A-profile (Security Extensions) only
SCR_NS		=	0x01  // A-profile (Security Extensions) only

TTBCR_EAE	=	(1<<31)	// Are we using LPAE?

CACHE_LINE_SIZE	=	32	// TODO: remove this

    .text
    .align 4

ENTRY(_dmb)
	dmb
	bx	lr
ENDPROC(_dmb)

ENTRY(_dsb)
	dsb
	bx	lr
ENDPROC(_dsb)

ENTRY(_wfi)
	wfi
	bx	lr
ENDPROC(_wfi)

ENTRY(_wfe)
	wfe
	bx	lr
ENDPROC(_wfe)

ENTRY(_sev)
	sev
	bx	lr
ENDPROC(_sev)

	//@ This function takes three arguments
	//@ r0: Destination start address (must be word aligned)
	//@ r1: Source start address (must be word aligned)
	//@ r2: Number of words to copy
	//@ Return value is updated destination pointer (first unwritten word)
ENTRY(copy_words)
	PUSH	{r4, r5, lr}

    cmp r2, #0
    ble 3f

2:	ldr	r3, [r1], #4
    dmb
	str	r3, [r0], #4
	dmb

	subs	r2, r2, #1
	bne	2b

3:	POP	{r4, r5, lr}
	bx	lr
ENDPROC(copy_words)

ENTRY(read_drar)
	mrc	p14, 0, r0, c1, c0, 0	//@ Read Debug ROM Address Register
	bx	lr
ENDPROC(read_drar)

ENTRY(read_dsar)
	mrc	p14, 0, r0, c2, c0, 0	//@ Read Debug Self Address Offset Register
	bx	lr
ENDPROC(read_dsar)

ENTRY(write_osdlr)
	mcr	p14, 0, r0, c1, c3, 4	//@ Write OS Double Lock Register
	bx	lr
ENDPROC(write_osdlr)

ENTRY(write_actlr)
	mcr	p15, 0, r0, c1, c0, 1
	isb
	bx	lr
ENDPROC(write_actlr)

ENTRY(read_actlr)
	mrc	p15, 0, r0, c1, c0, 1
	bx	lr
ENDPROC(read_actlr)

/////////////////////////////////////////////////////////////////////////////////
ENTRY(save_performance_monitors)

	stmfd sp!, {r4, r8, r9, r10}

	//@ Ignore:
	//@        Count Enable Clear Register
	//@        Software Increment Register
	//@        Interrupt Enable Clear Register

	mrc	p15,0,r8,c9,c12,0	//@ PMon: Control Register
	bic	r1,r8,#1
	mcr	p15,0,r1,c9,c12,0	//@ disable counter updates from here
	isb				//@ 0b0 => PMCR<0>
	mrc	p15,0,r9,c9,c12,3	//@ PMon: Overflow Flag Status Reg
	mrc	p15,0,r10,c9,c12,5	//@ PMon: Event Counter Selection Reg
	stm	r0!, {r8-r10}
	ubfx	r9,r8,#11,#5		//@ extract # of event counters, N
	tst	r9, r9
	beq	1f

0:	subs	r9,r9,#1		//@ decrement N
	mcr	p15,0,r9,c9,c12,5	//@ PMon: select CounterN
	isb
	mrc	p15,0,r3,c9,c13,1	//@ PMon: save Event Type register
	mrc	p15,0,r4,c9,c13,2	//@ PMon: save Event Counter register
	stm	r0!, {r3,r4}
	bne	0b

1:	mrc	p15,0,r1,c9,c13,0	//@ PMon: Cycle Count Register
	mrc	p15,0,r2,c9,c14,0	//@ PMon: User Enable Register
	mrc	p15,0,r3,c9,c14,1	//@ PMon: Interrupt Enable Set Reg
	mrc	p15,0,r4,c9,c12,1	//@ PMon: Count Enable Set Register
	stm	r0!, {r1-r4}

	ldmfd sp!, {r4, r8, r9, r10}
	bx	lr
ENDPROC(save_performance_monitors)

ENTRY(restore_performance_monitors)

	stmfd sp!, {r4-r5, r8-r10, lr}
	//@ NOTE: all counters disabled by PMCR<0> == 0 on reset

	//@ Restore performance counters
	ldm	r0!,{r8-r10}	//@ recover first block of PMon context
				//@ (PMCR, PMOVSR, PMSELR)
	mov	r1, #0		//@ generate register of all 0's
	mvn	r2, #0		//@ generate register of all 1's
	mcr	p15,0,r2,c9,c14,2	//@ disable all counter related interrupts
	mcr	p15,0,r2,c9,c12,3	//@ clear all overflow flags
	isb

	ubfx	r12,r8,#11,#5	//@ extract # of event counters, N (0-31)
	tst	r12, r12
	beq	20f
	mov	r3, r12		//@ for N >0, generate a 2nd copy of N
	mov	r4, #1
	lsl	r4, r4, r3
	sub	r4, r4, #1	//@ set bits<N-1:0> to all 1's

0:	subs    r3,r3,#1            //@ decrement N
	mcr	p15,0,r3,c9,c12,5   //@ select Event CounterN
	isb
	mrc	p15,0,r5,c9,c13,1   //@ read Event Type register
	bfc	r5,#0,#8
	mcr	p15,0,r5,c9,c13,1   //@ set Event Type to 0x0
	mcr	p15,0,r2,c9,c13,2   //@ set Event Counter to all 1's
	isb
	bne	0b

	mov	r3, #1
	bic	r5, r9, #1<<31
	mcr	p15,0,r5,c9,c12,1	//@ enable Event Counters
					//@ (PMOVSR bits set)
	mcr	p15,0,r3,c9,c12,0	//@ set the PMCR global enable bit
	isb
	mcr	p15,0,r9,c9,c12,4   //@ set event count overflow bits
	isb
	mcr	p15,0,r4,c9,c12,2   //@ disable Event Counters

	//@ restore the event counters
10:	subs	r12,r12,#1          //@ decrement N
	mcr	p15,0,r12,c9,c12,5  //@ select Event CounterN
	isb
	ldm	r0!,{r3-r4}
	mcr	p15,0,r3,c9,c13,1   //@ restore Event Type
	mcr	p15,0,r4,c9,c13,2   //@ restore Event Counter
	isb
	bne	10b

20:	tst	r9, #0x80000000		//@ check for cycle count overflow flag
	beq	40f
	mcr	p15,0,r2,c9,c13,0	//@ set Cycle Counter to all 1's
	isb
	mov	r3, #0x80000000
	mcr	p15,0,r3,c9,c12,1	//@ enable the Cycle Counter
	isb

30:	mrc	p15,0,r4,c9,c12,3	//@ check cycle count overflow now set
	movs	r4,r4			//@ test bit<31>
	bpl	30b
	mcr	p15,0,r3,c9,c12,2	//@ disable the Cycle Counter

40:	mcr	p15,0,r1,c9,c12,0	//@ clear the PMCR global enable bit
	isb

	//@ restore the remaining PMon registers
	ldm	r0!,{r1-r4}
	mcr	p15,0,r1,c9,c13,0	//@ restore Cycle Count Register
	mcr	p15,0,r2,c9,c14,0	//@ restore User Enable Register
	mcr	p15,0,r3,c9,c14,1	//@ restore Interrupt Enable Set Reg
	mcr	p15,0,r4,c9,c12,1	//@ restore Count Enable Set Register
	mcr	p15,0,r10,c9,c12,5	//@ restore Event Counter Selection
	isb
	mcr	p15,0,r8,c9,c12,0	//@ restore the PM Control Register
	isb

	ldmfd sp!, {r4-r5, r8-r10, pc}
ENDPROC(restore_performance_monitors)

ENTRY(save_banked_registers)
	mrs	r2, CPSR		//@ save current mode
	cps	#MODE_SYS		//@ switch to System mode
	str	sp,[r0], #4		//@ save the User SP
	str	lr,[r0], #4		//@ save the User LR
	cps	#MODE_ABT		//@ switch to Abort mode
	str	sp,[r0], #4		//@ save the current SP
	mrs	r3,SPSR
	stm	r0!,{r3,lr}		//@ save the current SPSR, LR
	cps	#MODE_UND		//@ switch to Undefined mode
	str	sp,[r0], #4		//@ save the current SP
	mrs	r3,SPSR
	stm	r0!,{r3,lr}		//@ save the current SPSR, LR
	cps	#MODE_IRQ		//@ switch to IRQ mode
	str	sp,[r0], #4		//@ save the current SP
	mrs	r3,SPSR
	stm	r0!,{r3,lr}		//@ save the current SPSR, LR
	cps	#MODE_FIQ		//@ switch to FIQ mode
	str	SP,[r0], #4		//@ save the current SP
	mrs	r3,SPSR
	stm	r0!,{r8-r12,lr}		//@ save the current SPSR,r8-r12,LR
	msr	CPSR_cxsf, r2		//@ switch back to original mode

	bx	lr
ENDPROC(save_banked_registers)

ENTRY(restore_banked_registers)
	mrs	r2, CPSR		//@ save current mode
	cps	#MODE_SYS		//@ switch to System mode
	ldr	sp,[r0],#4		//@ restore the User SP
	ldr	lr,[r0],#4		//@ restore the User LR
	cps	#MODE_ABT		//@ switch to Abort mode
	ldr	sp,[r0],#4		//@ restore the current SP
	ldm	r0!,{r3,lr}		//@ restore the current LR
	msr	SPSR_fsxc,r3		//@ restore the current SPSR
	cps	#MODE_UND		//@ switch to Undefined mode
	ldr	sp,[r0],#4		//@ restore the current SP
	ldm	r0!,{r3,lr}		//@ restore the current LR
	msr	SPSR_fsxc,r3		//@ restore the current SPSR
	cps	#MODE_IRQ		//@ switch to IRQ mode
	ldr	sp,[r0],#4		//@ restore the current SP
	ldm	r0!,{r3,lr}		//@ restore the current LR
	msr	SPSR_fsxc,r3		//@ restore the current SPSR
	cps	#MODE_FIQ		//@ switch to FIQ mode
	ldr	sp,[r0],#4		//@ restore the current SP
	ldm	r0!,{r8-r12,lr}		//@ restore the current r8-r12,LR
	msr	SPSR_fsxc,r4		//@ restore the current SPSR
	msr	CPSR_cxsf, r2		//@ switch back to original mode

0:	bx	lr
ENDPROC(restore_banked_registers)


ENTRY(save_cp15)
	//@ CSSELR Cache Size Selection Register
	mrc	p15,2,r3,c0,c0,0
	str	r3,[r0], #4

	//@ IMPLEMENTATION DEFINED - proprietary features:
	//@ (CP15 register 15, TCM support, lockdown support, etc.)

	//@ NOTE: IMP DEF registers might have save and restore order that relate
	//@ to other CP15 registers or logical grouping requirements and can
	//@ therefore occur at any point in this sequence.
	bx	lr
ENDPROC(save_cp15)

ENTRY(restore_cp15)
	//@ CSSELR – Cache Size Selection Register
	ldr	r3,[r0], #4
	mcr	p15,2,r3,c0,c0,0

	bx	lr
ENDPROC(restore_cp15)

	//@ Function called with two arguments:
	//@	r0 contains address to store control registers
	//@	r1 is non-zero if we are Secure
ENTRY(save_control_registers)
	cmp	r1, #0			//@ Are we Secure?
	mrc	p15,0,r2,c1,c0,1	//@ ACTLR - Auxiliary Control Register
	mrc	p15,0,r3,c1,c0,0	//@ SCTLR - System Control Register
	mrc	p15,0,r12,c1,c0,2	//@ CPACR - Coprocessor Access Control Register
	stm	r0!, {r2-r3, r12}
	mrcne	p15,0,r1,c12,c0,1	//@ MVBAR - Monitor Vector Base Address Register
	mrcne	p15,0,r2,c1,c1,0	//@ Secure Configuration Register
	mrcne	p15,0,r3,c1,c1,1	//@ Secure Debug Enable Register
	mrcne	p15,0,r12,c1,c1,2	//@ Non-Secure Access Control Register
	stmne	r0!, {r1-r3,r12}
	mrc	p14,6,r1,c0,c0,0	//@ TEECR
	mrc	p14,6,r2,c1,c0,0	//@ TEEHBR
	mrc	p14,7,r3,c1,c0,0	//@ JOSCR
	mrc	p14,7,r12,c2,c0,0	//@ JMCR
	stm	r0!, {r1-r3,r12}
	bx	lr
ENDPROC(save_control_registers)


	//@ Function called with two arguments:
	//@	r0 contains address to read control registers
	//@	r1 is non-zero if we are Secure
ENTRY(restore_control_registers)
	cmp	r1, #0			//@ Are we Secure?
	ldm	r0!, {r2-r3, r12}
	mcr	p15,0,r2,c1,c0,1	//@ ACTLR - Auxiliary Control Register
	mcr	p15,0,r3,c1,c0,0	//@ SCTLR - System Control Register
	mcr	p15,0,r12,c1,c0,2	//@ CPACR - Coprocessor Access Control Register
	ldmne	r0!, {r1-r3,r12}
	mcrne	p15,0,r1,c12,c0,1	//@ MVBAR - Monitor Vector Base Address Register
	mcrne	p15,0,r2,c1,c1,0	//@ Secure Configuration Register
	mcrne	p15,0,r3,c1,c1,1	//@ Secure Debug Enable Register
	mcrne	p15,0,r12,c1,c1,2	//@ Non-Secure Access Control Register
	ldm	r0!, {r1-r3,r12}
	mcr	p14,6,r1,c0,c0,0	//@ TEECR
	mcr	p14,6,r2,c1,c0,0	//@ TEEHBR
	mcr	p14,7,r3,c1,c0,0	//@ JOSCR
	mcr	p14,7,r12,c2,c0,0	//@ JMCR
	isb
	bx	lr
ENDPROC(restore_control_registers)

ENTRY(save_mmu)
	stmfd sp!, {r4, r5, r6, r7}
	//@ ASSUMPTION: no useful fault address / fault status information

	mrc	p15,0,r4,c12,c0,0	//@ VBAR
	mrc	p15,0,r5,c2,c0,2	//@ TTBCR

	tst	r5, #TTBCR_EAE		//@ Are we using LPAE?

	//@ save 32 or 64 bit TTBRs
	mrceq	p15,0,r6,c2,c0,0	//@ 32 bit TTBR0
	mrceq	p15,0,r7,c2,c0,1	//@ 32 bit TTBR1
	mrrcne	p15,0,r6,r7,c2		//@ 64 bit TTBR0
	stm	r0!, {r4-r7}
	mrrcne	p15,1,r6,r7,c2		//@ 64 bit TTBR1
	stmne	r0!, {r6-r7}

	mrc	p15,0,r4,c3,c0,0	//@ DACR
	mrc	p15,0,r5,c7,c4,0	//@ PAR
	mrc	p15,0,r6,c10,c2,0	//@ PRRR
	mrc	p15,0,r7,c10,c2,1	//@ NMRR
	stm	r0!, {r4-r7}

	//@ TODO: IMPLEMENTATION DEFINED - TCM, lockdown and performance monitor support
	//@     CP15 registers 9 and 11

	mrc	p15,0,r4,c13,c0,1	//@ CONTEXTIDR
	mrc	p15,0,r5,c13,c0,2	//@ TPIDRURW
	mrc	p15,0,r6,c13,c0,3	//@ TPIDRURO
	mrc	p15,0,r7,c13,c0,4	//@ TPIDRPRW
	stm	r0!, {r4-r7}

	ldmfd sp!, {r4, r5, r6, r7}
	bx	lr
ENDPROC(save_mmu)


ENTRY(restore_mmu)

	stmfd sp!, {r4, r5, r6, r7}
	ldm	r0!, {r4-r7}
	mcr	p15,0,r4,c12,c0,0	//@ VBAR
	mcr	p15,0,r5,c2,c0,2	//@ TTBCR

	tst	r5, #TTBCR_EAE		//@ Are we using LPAE?

	//@ restore 32 or 64 bit TTBRs
	mcreq	p15,0,r6,c2,c0,0	//@ 32 bit TTBR0
	mcreq	p15,0,r7,c2,c0,1	//@ 32 bit TTBR1
	mcrrne	p15,0,r6,r7,c2		//@ 64-bit TTBR0
	ldmne	r0!, {r6-r7}
	mcrrne	p15,1,r6,r7,c2		//@ 64-bit TTBR1

	ldm	r0!, {r4-r7}
	mcr	p15,0,r4,c3,c0,0	//@ DACR
	mcr	p15,0,r5,c7,c4,0	//@ PAR
	mcr	p15,0,r6,c10,c2,0	//@ PRRR
	mcr	p15,0,r7,c10,c2,1	//@ NMRR

	//@ TODO: IMPLEMENTATION DEFINED - TCM, lockdown and performance monitor support
	//@     CP15 registers 9 and 11

	ldm	r0!, {r4-r7}
	mcr	p15,0,r4,c13,c0,1	//@ CONTEXTIDR
	mcr	p15,0,r5,c13,c0,2	//@ TPIDRURW
	mcr	p15,0,r6,c13,c0,3	//@ TPIDRURO
	mcr	p15,0,r7,c13,c0,4	//@ TPIDRPRW

	ldmfd sp!, {r4, r5, r6, r7}
	bx	lr
ENDPROC(restore_mmu)

/*
ENTRY(save_vfp)
        //@ FPU state save/restore.
        //@ FPSID,MVFR0 and MVFR1 don't get serialized/saved (Read Only).
	mrc	p15,0,r3,c1,c0,2	//@ CPACR allows CP10 and CP11 access
	ORR	r2,r3,#0xF00000
	mcr	p15,0,r2,c1,c0,2
	isb
	mrc	p15,0,r2,c1,c0,2
	and	r2,r2,#0xF00000
	cmp	r2,#0xF00000
	beq	0f
	movs	r2, #0
	b	2f

0:	//@	Save configuration registers and enable.
	vmrs	r12,FPEXC
	str	r12,[r0],#4		//@ Save the FPEXC
        //@ Enable FPU access to save/restore the other registers.
	ldr	r2,=0x40000000
	vmsr	FPEXC,r2
	vmrs	r2,FPSCR
	str	r2,[r0],#4		//@ Save the FPSCR
        //@ Store the VFP-D16 registers.
	vstm	r0!, {D0-D15}
        //@ Check for Advanced SIMD/VFP-D32 support
	vmrs	r2,MVFR0
	and	r2,r2,#0xF		//@ extract the A_SIMD bitfield
	cmp	r2, #0x2
	blt	1f
        //@ Store the Advanced SIMD/VFP-D32 additional registers.
	vstm	r0!, {D16-D31}

        //@ IMPLEMENTATION DEFINED: save any subarchitecture defined state
        //@ NOTE: Don't change the order of the FPEXC and CPACR restores
1:
	vmsr	FPEXC,r12         //@ Restore the original En bit of FPU.
2:
	mcr	p15,0,r3,c1,c0,2 //@ Restore the original CPACR value.
	bx	lr
ENDPROC(save_vfp)


ENTRY(restore_vfp)
	//@ FPU state save/restore. Obviously FPSID,MVFR0 and MVFR1 don't get
	//@ serialized (RO).
	//@ Modify CPACR to allow CP10 and CP11 access
	mrc	p15,0,r1,c1,c0,2
	ORR	r2,r1,#0x00F00000
	mcr	p15,0,r2,c1,c0,2
	//@ Enable FPU access to save/restore the rest of registers.
	ldr	r2,=0x40000000
	vmsr	FPEXC, r2
	//@ Recover FPEXC and FPSCR. These will be restored later.
	ldm	r0!,{r3,r12}
	//@ Restore the VFP-D16 registers.
	vldm	r0!, {D0-D15}
	//@ Check for Advanced SIMD/VFP-D32 support
	vmrs	r2, MVFR0
	and	r2,r2,#0xF		//@ extract the A_SIMD bitfield
	cmp	r2, #0x2
	blt	0f

	//@ Store the Advanced SIMD/VFP-D32 additional registers.
	vldm	r0!, {D16-D31}

	//@ IMPLEMENTATION DEFINED: restore any subarchitecture defined state

0:	//@ Restore configuration registers and enable.
	//@ Restore FPSCR _before_ FPEXC since FPEXC could disable FPU
	//@ and make setting FPSCR unpredictable.
	vmsr	FPSCR,r12
	vmsr	FPEXC,r3		//@ Restore FPEXC after FPSCR
	//@ Restore CPACR
	mcr	p15,0,r1,c1,c0,2
	bx	lr
ENDPROC(restore_vfp)
*/

	//@ We assume that the OS is not using the Virtualization extensions,
	//@ and that the warm boot code will set up CNTHCTL correctly.
	//@ CNTP_CVAL will be preserved as it is in the always-on domain.

ENTRY(save_generic_timer)
	mrc	p15,0,r2,c14,c2,1	//@ read CNTP_CTL
	mrc	p15,0,r3,c14,c2,0	//@ read CNTP_TVAL
	mrc	p15,0,r12,c14,c1,0	//@ read CNTKCTL
	stm	r0!, {r2, r3, r12}
	bx	lr
ENDPROC(save_generic_timer)

ENTRY(restore_generic_timer)
	ldm	r0!, {r2, r3, r12}
	mcr	p15,0,r3,c14,c2,0	//@ write CNTP_TVAL
	mcr	p15,0,r12,c14,c1,0	//@ write CNTKCTL
	mcr	p15,0,r2,c14,c2,1	//@ write CNTP_CTL
	bx	lr
ENDPROC(restore_generic_timer)


	//@ This function disables L1 data caching, then cleans and invalidates
	//@ the whole L1 data cache.

ENTRY(disable_clean_inv_dcache_v7_l1)
	stmfd sp!, {r4, lr}

	//@ Disable L1 cache
	dsb
	mrc	p15,0,r3,c1,c0,0
        bic	r3, #4			//@ Clear C bit
	mcr	p15,0,r3,c1,c0,0
	dsb

	//@ No more Data cache allocations can happen at L1.
        //@ Until we finish cleaning the Inner cache, any accesses to dirty data
        //@ (e.g. by translation table walks) may get the wrong (Outer) data, so
        //@ we have to be sure everything that might be accessed is clean.
        //@ We already know that the translation tables are clean (see late_init).

	mov	r0, #0			//@ Select L1 Data/Unified cache
     	mcr	p15,2,r0,c0,c0,0
     	mrc	p15,1,r0,c0,c0,0	//@ Read size
        ubfx	r3, r0, #13, #15	//@ sets - 1
        add	r3, r3, #1		//@ sets
        ubfx	r4, r0, #0, #3		//@ log2(words per line) - 2
        add	r4, r4, #4		//@ set shift = log2(bytes per line)
        ubfx	r2, r0, #3, #10		//@ ways - 1
        clz	r12, r2			//@ way shift
        add	r2, r2, #1		//@ ways

        //@ r2,r3 inner, outer loop targets, r1 inner loop counter, r0 zero
5:	cmp	r3, #0
	beq	20f
  	sub	r3, r3, #1
	mov	r1, r2

10:	cmp	r1, #0
        beq	5b
        sub	r1, r1, #1
	mov	r0, r1, lsl r12		//@ Fill in Way field
        orr	r0, r0, r3, lsl r4	//@ Fill in Set field
	mcr	p15,0,r0,c7,c14,2	//@ DCCISW
        b	10b

20:	dsb
	ldmfd sp!, {r4, lr}
	bx	lr
ENDPROC(disable_clean_inv_dcache_v7_l1)


ENTRY(invalidate_icache_v7_pou)
	mov     r0, #0
	mcr     p15, 0, r0, c7, c5, 0      //@ ICIALLU
	bx	lr
ENDPROC(invalidate_icache_v7_pou)

ENTRY(invalidate_dcache_v7_all)
 //@ Must iterate over the caches in order to synthesise a complete invalidation
 //@ of data/unified cache
	stmfd sp!, {r4-r11}
	mrc     p15, 1, r0, c0, c0, 1       //@ read clidr
	ands    r3, r0, #0x7000000          //@ extract loc from clidr
	mov     r3, r3, lsr #23             //@ left align loc bit field
	beq     finished                    //@ if loc is 0, then no need to clean
	mov     r10, #0                     //@ start clean at cache level 0 (in r10)
loop1:
	add     r2, r10, r10, lsr #1        //@ work out 3x current cache level
	mov     r12, r0, lsr r2             //@ extract cache type bits from clidr
	and     r12, r12, #7                //@ mask of bits for current cache only
	cmp     r12, #2                     //@ see what cache we have at this level
	blt     skip                        //@ skip if no cache, or just i-cache
	mcr     p15, 2, r10, c0, c0, 0      //@ select current cache level in cssr
	mov     r12, #0
	mcr     p15, 0, r12, c7, c5, 4      //@ prefetchflush to sync new cssr&csidr
	mrc     p15, 1, r12, c0, c0, 0      //@ read the new csidr
	and     r2, r12, #7                 //@ extract the length of the cache lines
	add     r2, r2, #4                  //@ add 4 (line length offset)
	ldr     r6, =0x3ff
	ands    r6, r6, r12, lsr #3         //@ find maximum number on the way size
	clz     r5, r6                      //@ find bit pos of way size increment
	ldr     r7, =0x7fff
	ands    r7, r7, r12, lsr #13        //@ extract max number of the index size
loop2:
	mov     r8, r6                      //@ create working copy of max way size
loop3:
	orr     r11, r10, r8, lsl r5        //@ factor way and cache number into r11
	orr     r11, r11, r7, lsl r2        //@ factor index number into r11
	mcr     p15, 0, r11, c7, c6, 2      //@ invalidate by set/way
	subs    r8, r8, #1                  //@ decrement the way
	bge     loop3
	subs    r7, r7, #1                  //@ decrement the index
	bge     loop2
skip:
	add     r10, r10, #2                //@ increment cache number
	cmp     r3, r10
	bgt     loop1
finished:
	mov     r10, #0

	mcr     p15, 0, r10, c7, c10, 4     //@ drain write buffer
	mcr     p15, 0, r10, c8, c7, 0      //@ invalidate I + D TLBs
	mcr     p15, 0, r10, c2, c0, 2      //@ TTB control register
	ldmfd sp!, {r4-r11}
	bx      lr
ENDPROC(invalidate_dcache_v7_all)


ENTRY(disable_clean_inv_dcache_v7_all)
 //@ Must iterate over the caches in order to synthesise a complete clean
 //@ of data/unified cache
	stmfd sp!, {r4-r11}

	//@ Disable integrated data/unified cache
	dsb
	mrc	p15, 0, r3, c1, c0, 0
        bic	r3, #4			//@ Clear C bit
	mcr	p15, 0, r3, c1, c0, 0
	isb

	//@ No more Data cache allocations can happen.
        //@ Until we finish cleaning the cache, any accesses to dirty data
        //@ (e.g. by translation table walks) may get the wrong (Outer) data, so
        //@ we have to be sure everything that might be accessed is clean.
        //@ We already know that the translation tables are clean (see late_init).


	mrc     p15, 1, r0, c0, c0, 1       //@ read clidr
	ands    r3, r0, #0x7000000          //@ extract loc from clidr
	mov     r3, r3, lsr #23             //@ left align loc bit field
	beq     50f                        //@ if loc is 0, then no need to clean
	mov     r10, #0                     //@ start clean at cache level 0 (in r10)
10:
	add     r2, r10, r10, lsr #1        //@ work out 3x current cache level
	mov     r12, r0, lsr r2             //@ extract cache type bits from clidr
	and     r12, r12, #7                //@ mask of bits for current cache only
	cmp     r12, #2                     //@ see what cache we have at this level
	blt     40f                        //@ skip if no cache, or just i-cache
	mcr     p15, 2, r10, c0, c0, 0      //@ select current cache level in cssr
	mov     r12, #0
	mcr     p15, 0, r12, c7, c5, 4      //@ prefetchflush to sync new cssr&csidr
	mrc     p15, 1, r12, c0, c0, 0      //@ read the new csidr
	and     r2, r12, #7                 //@ extract the length of the cache lines
	add     r2, r2, #4                  //@ add 4 (line length offset)
	ldr     r6, =0x3ff
	ands    r6, r6, r12, lsr #3         //@ find maximum number on the way size
	clz     r5, r6                      //@ find bit pos of way size increment
	ldr     r7, =0x7fff
	ands    r7, r7, r12, lsr #13        //@ extract max number of the index size
20:
	mov     r8, r6                      //@ create working copy of max way size
30:
	orr     r11, r10, r8, lsl r5        //@ factor way and cache number into r11
	orr     r11, r11, r7, lsl r2        //@ factor index number into r11
	mcr     p15, 0, r11, c7, c14, 2     //@ clean & invalidate by set/way
	subs    r8, r8, #1                  //@ decrement the way
	bge     30b
	subs    r7, r7, #1                  //@ decrement the index
	bge     20b
40:
	add     r10, r10, #2                //@ increment cache number
	cmp     r3, r10
	bgt     10b
50:
	mov     r10, #0
	mcr     p15, 0, r10, c7, c10, 4     //@ drain write buffer
	ldmfd sp!, {r4-r11}
	bx	lr
ENDPROC(disable_clean_inv_dcache_v7_all)



	//@ This function cleans the whole L1 data cache
ENTRY(clean_dcache_v7_l1)
	stmfd sp!, {r4, lr}

	mov	r0, #0			//@ Select L1 Data/Unified cache
     	mcr	p15,2,r0,c0,c0,0
     	mrc	p15,1,r0,c0,c0,0	//@ Read size (CCSIDR)
        ubfx	r3, r0, #13, #15	//@ sets - 1
        add	r3, r3, #1		//@ sets
        ubfx	r4, r0, #0, #3		//@ log2(words per line) - 2
        add	r4, r4, #4		//@ set shift = log2(bytes per line)
        ubfx	r2, r0, #3, #10		//@ ways - 1
        clz	r12, r2			//@ way shift
        add	r2, r2, #1		//@ ways

        //@ r2,r3 inner, outer loop targets, r1 inner loop counter, r0 zero
0:	cmp	r3, #0
	beq	20f
  	sub	r3, r3, #1
	mov	r1, r2

10:	cmp	r1, #0
        beq	0b
        sub	r1, r1, #1
	mov	r0, r1, lsl r12		//@ Fill in Way field
        orr	r0, r0, r3, lsl r4	//@ Fill in Set field
	mcr	p15,0,r0,c7,c10,2	//@ DCCSW
        b	10b

20:	dsb
	ldmfd sp!, {r4, lr}
	bx	lr
ENDPROC(clean_dcache_v7_l1)

	//@ This function cleans a single line from the L1 dcache
ENTRY(clean_mva_dcache_v7_l1)
	mcr	p15,0,r0,c7,c10,1	//@ DCCMVAC
	bx	lr
/*
ENTRY(enter_secure_monitor_mode)
	mov	r0, lr
	mov	r1, sp
	smc	#0
appf_smc_handler:
	//@ We are now in Monitor mode, make sure we're Secure
	mrc	p15, 0, r12, c1, c1, 0
	bic	r12, #SCR_NS
	mcr	p15, 0, r12, c1, c1, 0
	//@ Restore sp and return - stack must be uncached or in NS memory!
	mov	sp, r1
	bx	r0
ENDPROC(enter_secure_monitor_mode)
*/

ENTRY(enter_nonsecure_svc_mode)
	//@ Copy the Monitor mode sp and lr values
	mov	r2, lr
	mov	r3, sp
	mrc	p15, 0, r1, c1, c1, 0
	orr	r1, #SCR_NS
	mcr	p15, 0, r1, c1, c1, 0
	adr	lr, non_secure
	movs	pc, lr
non_secure:
	//@ We are now in non-secure state
	//@ Restore sp and return
	mov	sp, r3
	bx	r2
ENDPROC(enter_nonsecure_svc_mode)

ENTRY(save_a9_other)
	mrc	p15,0,r12,c15,c0,0	//@ Read Power Control Register
	str	r12, [r0], #4
        mrc     p15, 4, r12, c15, c0, 0  //@ Read Configuration Base Address Register
	str	r12, [r0], #4

	mrc	p15,0,r3,c0,c0,0	//@ Read Main ID Register
	ubfx	r3, r3, #20, #4		//@ Extract major version number
	cmp	r3, #2
	blt	1f			//@ PLE only possible in r2p0 onwards
	mrc	p15,0,r3,c11,c0,0	//@ Read PLE IDR
	cmp	r3, #0
	beq	1f			//@ No PLE present

	mrc	p15,0,r3,c11,c1,0	//@ Read PLE UAR
	mrc	p15,0,r12,c11,c1,1	//@ Read PLE PCR
	stm	r0!, {r3, r12}

1:	bx	lr
ENDPROC(save_a9_other)

ENTRY(restore_a9_other)
	cmp	r1, #0			//@ Check we are secure
	ldr	r12, [r0], #4
	andne	r12, r12, #0x01		//@ We only restore the Dynamic Clock gating bit
	mcrne	p15,0,r12,c15,c0,0	//@ Write Power Control Register (if secure)
	ldr	r12, [r0], #4
        mcrne   p15, 4, r12, c15, c0, 0	//@ Write Configuration Base Address Register (if Secure)

	mrc	p15,0,r3,c0,c0,0	//@ Read Main ID Register
	ubfx	r3, r3, #20, #4		//@ Extract major version number
	cmp	r3, #2
	blt	1f			//@ PLE only possible in r2p0 onwards
	mrc	p15,0,r3,c11,c0,0	//@ Read PLE IDR
	cmp	r3, #0
	beq	1f			//@ No PLE present

	ldm	r0!, {r3, r12}
	mcr	p15,0,r3,c11,c1,0	//@ Write PLE UAR
	mcr	p15,0,r12,c11,c1,1	//@ Write PLE PCR

1:	bx	lr
ENDPROC(restore_a9_other)


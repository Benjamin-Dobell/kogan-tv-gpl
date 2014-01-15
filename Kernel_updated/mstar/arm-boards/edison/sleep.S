/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
#include <linux/linkage.h>
#include <asm/assembler.h>
#include <mach/pm.h>
#include <asm/vfpmacros.h>

SAVE_IMP_DEFINE0 = 0
SAVE_IMP_DEFINE1 = 1

.global sleep_save_mmu_ctr

    .text
    .align 4

sleep_save_mmu_ctr:  .long 0
sleep_save_mmu_ttb:  .long 0
sleep_save_mmu_dmn:  .long 0
//@ c1
sleep_save_mmu_ACTLR: .long 0
sleep_save_mmu_CPACR: .long 0
//@ c2
sleep_save_mmu_TTBR1: .long 0
sleep_save_mmu_TTBCR: .long 0
//@ c10
sleep_save_mmu_PRRR:  .long 0
sleep_save_mmu_NMRR:  .long 0
//@ c13
sleep_save_mmu_FCSEIDR:  .long 0
sleep_save_mmu_CTXIDR:   .long 0
sleep_save_mmu_TPIDRURW: .long 0
sleep_save_mmu_TPIDRURO: .long 0
sleep_save_mmu_TPIDRPRW: .long 0

sleep_save_mmu_VBAR:     .long 0

.if (SAVE_IMP_DEFINE0==1)
sleep_save_imp_define0:   .long 0
.endif

.if (SAVE_IMP_DEFINE1==1)
sleep_save_imp_define1:   .long 0
.endif

//@
sleep_save_asimde_FPSCR:  .long 0
sleep_save_asimde_FPEXEC: .long 0

sleep_wakeup_save_magic:    .long 0
sleep_wakeup_save_addr:     .long 0
sleep_wakeup_save_data:     .long 0, 0

    .align 2
SerPrintPhyStk: .fill 256, 4, 0
SerPrintPhyStk_bottom: .long 0,0,0,0


/* input r0: char; r13: sp */
ENTRY(__PUTCHAR)
    stmfd sp!,{r0,r5,lr}
    mov  r5, #0x1F000000
    orr  r5, r5, #0x00200000
    orr  r5, r5, #0x1300
    strb r0,[r5]
    mov  r5,#0x10000
41:
    subs r5, #1
    bgt 41b
    ldmfd sp!,{r0,r5,pc}

/* input r0: reg val; r13: sp */
ENTRY(__PUTREGVAL)
    stmfd sp!,{r0-r4,lr}
    mov   r1, #32
    mov   r2, r0
    mov   r3, #0x0F
40:
    sub   r1, #4
    and   r4, r3, r2, LSR r1
    cmp   r4, #10
    addlo r0, r4, #'0'
    addhs r0, r4, #('A'-10)
    bl    __PUTCHAR
    cmp   r1, #0
    bgt   40b
    mov   r0, #'\r'      /* new line */
    bl    __PUTCHAR
    mov   r0, #'\n'
    bl    __PUTCHAR
    ldmfd sp!,{r0-r4,pc}

ENTRY(PUTCHAR)
    adr  sp, SerPrintPhyStk_bottom
    stmfd sp!,{lr}
    bl    __PUTCHAR
    ldmfd sp!,{pc}

ENTRY(PUTREGVAL)
    adr  sp, SerPrintPhyStk_bottom
    stmfd sp!,{lr}
    bl    __PUTREGVAL
    ldmfd sp!,{pc}


/* input r0: char; r13: sp */
ENTRY(__PUTCHAR_VIRT)
    stmfd sp!,{r0,r5,lr}
    mov  r5, #0xFD000000
    orr  r5, r5, #0x00200000
    orr  r5, r5, #0x1300
    strb r0,[r5]
    ldmfd sp!,{r0,r5,pc}

/* input r0: reg val; r13: sp */
ENTRY(__PUTREGVAL_VIRT)
    stmfd sp!,{r0-r4,lr}
    mov   r1, #32
    mov   r2, r0
    mov   r3, #0x0F
40:
    sub   r1, #4
    and   r4, r3, r2, LSR r1
    cmp   r4, #10
    addlo r0, r4, #'0'
    addhs r0, r4, #('A'-10)
    bl    __PUTCHAR_VIRT
    cmp   r1, #0
    bgt   40b
    mov   r0, #'\r'      /* new line */
    bl    __PUTCHAR_VIRT
    mov   r0, #'\n'
    bl    __PUTCHAR_VIRT
    ldmfd sp!,{r0-r4,pc}

ENTRY(PUTCHAR_VIRT)
    adr  sp, SerPrintPhyStk_bottom
    stmfd sp!,{lr}
    bl    __PUTCHAR_VIRT
    ldmfd sp!,{pc}

ENTRY(PUTREGVAL_VIRT)
    adr  sp, SerPrintPhyStk_bottom
    stmfd sp!,{lr}
    bl    __PUTREGVAL_VIRT
    ldmfd sp!,{pc}
/*------------------------------------------------------------------------------
    Function: read_wkup_pmu

    Description:
        read wakup 16bit addr from pmu reg
    Input: (The arguments were used by caller to input data.)
        noe
    Output: (The arguments were used by caller to receive data.)
        r0: wakup 16bit addr
    Return:
        None.
    Remark:
        r0~r3 was damaged
-------------------------------------------------------------------------------*/
ENTRY(read_wkup_pmu)
    ldr r3, =PMU_WAKEUP_ADDR_REGL
    ldr r0, [r3]

    ldr r3, =PMU_WAKEUP_ADDR_REGH
    ldr r1, [r3]

    and r0, r0, #PMU_WAKEUP_ADDR_LMASK
    and r1, r1, #PMU_WAKEUP_ADDR_HMASK
    orr r0, r0, r1
    mov pc, lr
    .ltorg

ENTRY(read_wkup_pmu_virt)
    ldr r3, =PMU_WAKEUP_ADDR_REGL_VIRT
    ldr r0, [r3]

    ldr r3, =PMU_WAKEUP_ADDR_REGH_VIRT
    ldr r1, [r3]

    and r0, r0, #PMU_WAKEUP_ADDR_LMASK
    and r1, r1, #PMU_WAKEUP_ADDR_HMASK
    orr r0, r0, r1
    mov pc, lr
    .ltorg

/*------------------------------------------------------------------------------
    Function: write_wkup_pmu

    Description:
        save wakup 16bit addr to pmu reg
    Input: (The arguments were used by caller to input data.)
        r0: wakup 16bit addr
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        None.
    Remark:
        r0~r3 was damaged
-------------------------------------------------------------------------------*/
ENTRY(write_wkup_pmu)
    ldr r3, =PMU_WAKEUP_ADDR_REGL
    ldr r1, [r3]
    bic r1, #PMU_WAKEUP_ADDR_LMASK
    and r2, r0, #PMU_WAKEUP_ADDR_LMASK
    orr r1, r1, r2
    str r1, [r3]

    ldr r3, =PMU_WAKEUP_ADDR_REGH
    ldr r1, [r3]
    bic r1, #PMU_WAKEUP_ADDR_HMASK
    and r2, r0, #PMU_WAKEUP_ADDR_HMASK
    orr r1, r1, r2
    str r1, [r3]
    mov pc, lr
    .ltorg

ENTRY(write_wkup_pmu_virt)
    ldr r3, =PMU_WAKEUP_ADDR_REGL_VIRT
    ldr r1, [r3]
    bic r1, #PMU_WAKEUP_ADDR_LMASK
    and r2, r0, #PMU_WAKEUP_ADDR_LMASK
    orr r1, r1, r2
    str r1, [r3]

    ldr r3, =PMU_WAKEUP_ADDR_REGH_VIRT
    ldr r1, [r3]
    bic r1, #PMU_WAKEUP_ADDR_HMASK
    and r2, r0, #PMU_WAKEUP_ADDR_HMASK
    orr r1, r1, r2
    str r1, [r3]
    mov pc, lr
    .ltorg
/*------------------------------------------------------------------------------
    Function: sleep_set_wakeup_save_addr_phy

    Description:
        set the phy address where save the wake up addr
    Input: (The arguments were used by caller to input data.)
        r0: phy addr, r1: virt addr
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        none
-------------------------------------------------------------------------------*/
ENTRY(sleep_set_wakeup_save_addr_phy)
    stmfd sp!, {r0-r6,lr}

    ldr   r2, =MSTAR_SLEEP_MAGIC
    adrl  r3, sleep_wakeup_save_magic
    str   r2, [r3], #4
    str   r1, [r3], #4
    ldmia r1, {r5,r6}
    stmia r3, {r5,r6}

    ldr   r1, =WAKEUP_ADDR_MASK
    and   r0, r1, r0, LSR #16
    //orr   r0, #WAKEUP_FLAG_SLEPT     //set at last
    bl    write_wkup_pmu_virt

    ldmfd sp!, {r0-r6,pc}

ENTRY(sleep_clear_wakeup_save_addr_phy)
    stmfd sp!, {r0-r6,lr}

    ldr   r2, =MSTAR_SLEEP_MAGIC
    adrl  r3, sleep_wakeup_save_magic
    ldmia r3!,{r5,r6}
    cmp   r5, r2
    bne   41f
    cmp   r6, #0
    beq   41f
    cmp   r6, r1
    bne   41f
    ldmia r3, {r5,r6}
    stmia r1, {r5,r6}

    mov   r0, #0
    bl    write_wkup_pmu_virt
41:
    ldmfd sp!, {r0-r6,pc}

/*------------------------------------------------------------------------------
    Function: sleep_save_neon_regs

    Description:
        save neon common regs
    Input: (The arguments were used by caller to input data.)
        r0: buffer virtual address
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        none
-------------------------------------------------------------------------------*/
ENTRY(sleep_save_neon_regs)
    stmfd sp!, {r0-r7,lr}

    mrc   p15,0,r7,c1,c0,2
    orr   r1, r7, #0x00F00000
    bic   r1, r1, #0xC0000000
    mcr   p15,0,r1,c1,c0,2

    VFPFMRX  r6, FPEXC
    orr   r1, r6, #0x40000000
    VFPFMXR  FPEXC, r1

    //vstmia  r0!, {D0-D31}
    VFPFLDMIA r0, r1

    VFPFMXR  FPEXC, r6
    mcr   p15,0,r7,c1,c0,2

    ldmfd sp!, {r0-r7,pc}

/*------------------------------------------------------------------------------
    Function: sleep_restore_neon_regs

    Description:
        restore neon common regs
    Input: (The arguments were used by caller to input data.)
        r0: buffer virtual address
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        none
-------------------------------------------------------------------------------*/
ENTRY(sleep_restore_neon_regs)
    stmfd sp!, {r0-r7,lr}

    mrc   p15,0,r7,c1,c0,2
    orr   r1, r7, #0x00F00000
    bic   r1, r1, #0xC0000000
    mcr   p15,0,r1,c1,c0,2

    VFPFMRX  r6, FPEXC
    orr   r1, r6, #0x40000000
    VFPFMXR  FPEXC, r1

    //vldmia  r0!, {D0-D31}
    VFPFSTMIA r0, r1

    VFPFMXR  FPEXC, r6
    mcr   p15,0,r7,c1,c0,2

    ldmfd sp!, {r0-r7,pc}

/*------------------------------------------------------------------------------
    Function: sleep_save_cpu_registers

    Description:
        save cpu registers
    Input: (The arguments were used by caller to input data.)
        r0: buffer virtual address
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        r0-r3: destroyed
-------------------------------------------------------------------------------*/
ENTRY(sleep_save_cpu_registers)
    mrs     r3, cpsr
    /* switch to svc */
	cps #Mode_SVC

    /* save svc */
    mov     r2, r0
    ldr     r1, =MSTAR_SLEEP_MAGIC
    stmia   r0!,{r1,r4-r12,sp}
    mrs     r1, spsr
    stmia   r0!,{r1,r3}

    stmfd   sp!,{r4-r12,lr}         /* save registers */

    mov     r4, r0                  /* pointer */
    mov     r5, r2                  /* saved pointer */

    /* Enter FIQ mode, no interrupts */
    cps #Mode_FIQ

    mrs     r2, spsr
    stmia   r4!, {r2, r8-r12, sp, lr}

    /* Enter ABT mode, no interrupts */
	cps #Mode_ABT

	mrs		r0, spsr
	stmia   r4!, {r0, sp, lr}

	/* Enter IRQ mode, no interrupts */
	cps #Mode_IRQ

	mrs     r0, spsr
	stmia   r4!, {r0, sp, lr}

	/* Enter UND mode, no interrupts */
	cps #Mode_UND

	mrs     r0, spsr
	stmia   r4!, {r0, sp, lr}

	/* Enter SYS mode, no interrupts */
	cps #Mode_SYS

	stmia   r4!, {sp, lr}

	/* Back to SVC mode, no interrupts */
	cps #Mode_SVC

	/* do Checksum on the Sleepdata */
	mov     r3, r5              /* get pointer to SLEEPDATA */
	mov     r2, #0
30:
	ldr     r1, [r3], #4
	add     r2, r2, r1
	cmp     r3, r4
	blo     30b

	mvn     r2, r2
	str     r2, [r4],#4

    ldmfd   sp!,{r4-r12,pc}         /* restore registers */

/*------------------------------------------------------------------------------
    Function: sleep_restore_cpu_registers

    Description:
        restore cpu registers
    Input: (The arguments were used by caller to input data.)
        r0: buffer virtual address
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        all rx: destroyed
-------------------------------------------------------------------------------*/
ENTRY(sleep_restore_cpu_registers)
    mov     r5, r0                  /* saved pointer */
    add     r4, r0, #SLEEPSTATE_FIQ_SPSR     /* pointer */
    mrs     r3, cpsr

    /* Enter FIQ mode, no interrupts */
    cps #Mode_FIQ

    ldmia   r4!, {r2, r8-r12, sp, lr}
    msr     spsr, r2

    /* Enter ABT mode, no interrupts */
	cps #Mode_ABT

	ldmia   r4!, {r0, sp, lr}
	msr		spsr, r0

	/* Enter IRQ mode, no interrupts */
	cps #Mode_IRQ

	ldmia   r4!, {r0, sp, lr}
	msr		spsr, r0

	/* Enter UND mode, no interrupts */
	cps #Mode_UND

	ldmia   r4!, {r0, sp, lr}
	msr		spsr, r0

	/* Enter SYS mode, no interrupts */
	cps #Mode_SYS

	ldmia   r4!, {sp, lr}

	/* Back to SVC mode, no interrupts */
	cps #Mode_SVC

	add     r0, r5, #SLEEPSTATE_SVC_R4
	ldmia   r0!,{r4-r12,sp}
	ldmia   r0!,{r1,r2}
	msr     spsr, r1
	msr     cpsr, r2
    //can't restore A bit. why?

	mov     pc, lr

/*------------------------------------------------------------------------------
    Function: sleep_prepare_last

    Description:
        last prepare for sleep,should be called after sleep_set_wakeup_save_addr_phy
    Input: (The arguments were used by caller to input data.)
        r0: wake up phy addr
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:

-------------------------------------------------------------------------------*/
ENTRY(sleep_prepare_last)
    stmfd   sp!,{r0-r7,lr}
    /* save mmu */
    adrl    r2, sleep_save_mmu_ctr

    mrc     p15, 0, r3, c1, c0, 0   /* MMU Control */
    str     r3, [r2], #4

    mrc     p15, 0, r3, c2, c0, 0   /* TTB address. */
    str     r3, [r2], #4

    mrc     p15, 0, r3, c3, c0, 0   /* domain access control. */
    str     r3, [r2], #4

    mrc     p15,0,r3,c1,c0,1        /* ACTLR*/
    str     r3, [r2], #4
    mrc     p15,0,r3,c1,c0,2        /* CPACR*/
    str     r3, [r2], #4

    mrc     p15, 0, r3, c2, c0, 1   /* TTBR 1 */
    str     r3, [r2], #4
    mrc     p15, 0, r3, c2, c0, 2   /* TTBCR */
    str     r3, [r2], #4

    mrc     p15, 0, r3, c10, c2, 0   /* PRRR */
    str     r3, [r2], #4
    mrc     p15, 0, r3, c10, c2, 1   /* NMRR */
    str     r3, [r2], #4

    mrc     p15,0,r3,c13,c0,0        /* FCSEIDR */
    str     r3, [r2], #4
    mrc     p15,0,r3,c13,c0,1        /* CTXIDR */
    str     r3, [r2], #4
    mrc     p15,0,r3,c13,c0,2        /* TPIDRURW */
    str     r3, [r2], #4
    mrc     p15,0,r3,c13,c0,3        /* TPIDRURO */
    str     r3, [r2], #4
    mrc     p15,0,r3,c13,c0,4        /* TPIDRPRW */
    str     r3, [r2], #4

    mrc	    p15,0,r3,c12,c0,0	     /* VBAR */
    str     r3, [r2], #4
.if (SAVE_IMP_DEFINE0==1)
    mrc     p15, 1, r3, c9, c0, 2   /* imp defined */
    str     r3, [r2], #4
.endif
.if (SAVE_IMP_DEFINE1==1)
    mrc     p15, 0, r3, c15, c0, 1
    str     r3, [r2], #4
.endif

    /* neon */
    adrl    r2, sleep_save_asimde_FPSCR

    mrc   p15,0,r7,c1,c0,2
    orr   r3, r7, #0x00F00000
    bic   r3, r3, #0xC0000000
    mcr   p15,0,r3,c1,c0,2

    VFPFMRX  r6, FPEXC
    orr   r3, r6, #0x40000000
    VFPFMXR  FPEXC, r3

    VFPFMRX  r3, FPSCR
    str   r3, [r2], #4

    str   r6, [r2], #4

    VFPFMXR  FPEXC, r6
    mcr   p15,0,r7,c1,c0,2

    /* end neon*/

    adrl    r2, sleep_wakeup_save_addr
    ldr     r2, [r2]

    mov     r1, r0
    ldr     r0, =MSTAR_SLEEP_MAGIC
    stmia   r2!, {r0, r1}

    bl      mstar_sleep_cur_cpu_flush

    /* slept flag */
    bl    read_wkup_pmu_virt
    bic   r0, r0, #WAKEUP_FLAG_MASK
    orr   r0, r0, #WAKEUP_FLAG_SLEPT
    bl    write_wkup_pmu_virt

    ldmfd   sp!,{r0-r7,pc}

/*------------------------------------------------------------------------------
    Function: MMU_ctrl

    Description:
        MMU_ctrl
    Input: (The arguments were used by caller to input data.)
        r0: mmu ctrl, r1: exit address
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        r0-r2: damaged
-------------------------------------------------------------------------------*/
    .align 5
ENTRY(MMU_ctrl)
    mov r0, r0
    mcr	p15, 0, r0, c1, c0, 0
    mrc	p15, 0, r2, c0, c0, 0
    mov	r2, r2
    mov	r2, r1
    mov	pc, r2
ENDPROC(MMU_ctrl)
/*------------------------------------------------------------------------------
    Function: sleep_pseudo_down

    Description:
        sleep_pseudo_down
    Input: (The arguments were used by caller to input data.)
        r0: return address PHY
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        r0-r5: damaged
-------------------------------------------------------------------------------*/
ENTRY(sleep_pseudo_down)
    mov r1, r0
    mrc p15, 0, r0, c1, c0, 0
    bic r0,  #0x0F
    bic r0,  #0x1000

    b  MMU_ctrl


enable_l1_cache:
  MRC   p15, 0, r0, c1, c0, 0 //; Read Control Register configuration data
  ORR   r0, r0, #(0x1 << 12)  //; Enable I Cache
  ORR   r0, r0, #(0x1 << 2)   //; Enable D Cache
  MCR   p15, 0, r0, c1, c0, 0 //; Write Control Register configuration data
  BX    lr

disable_l1_cache:
  MRC   p15, 0, r0, c1, c0, 0 //; Read Control Register configuration data
  BIC   r0, r0, #(0x1 << 12)  //; Disable I Cache
  BIC   r0, r0, #(0x1 << 2)   //; Disable D Cache
  MCR   p15, 0, r0, c1, c0, 0 //; Write Control Register configuration data
  BX    lr

//; Invalidate Both Caches. Also flushes the branch target cache
invalidate_l1_cache:

  //; Invalidate Instruction cache
  MOV   r0, #0
  MCR   p15, 0, r0, c7, c5, 0

  //; Invalidate Data cache

  //; Read cache size from the Cache Size Identification Register
  MRC   p15, 1, r3, c0, c0, 0          //; Read current Cache Size Identification Register
  //MOV   r1, #0x1ff
  MOV   r1, #0x0ff
  orr   r1, r1, #0x100
  AND   r3, r1, r3, LSR #13            //; r3 = (number of sets - 1)
  MOV   r0, #0                         //; r0 -> way counter
way_loop:
  MOV   r1, #0                         //; r1 -> set counter
set_loop:
  MOV   r2, r0, LSL #30                //;
  ORR   r2, r1, LSL #5                 //; r2 -> set/way cache-op format
  MCR   p15, 0, r2, c7, c6, 2          //; Invalidate line described by r2
  ADD   r1, r1, #1                     //; Increment set counter
  CMP   r1, r3                         //; Check if the last set is reached...
  BLE   set_loop                       //; ...if not, continue the set_loop...
  ADD   r0, r0, #1                     //; ...else, Increment way counter
  CMP   r0, #4                         //; Check if the last way is reached...
  BLT   way_loop                       //; ...if not, continue the way_loop

  BX    lr

  //; Invalidate Unified TLB
invalidate_tlb:
  MOV   r0, #0
  MCR   p15, 0, r0, c8, c7, 0
  BX    lr
/*------------------------------------------------------------------------------
    Function: sleep_wakeup_first

    Description:
        wake up first. MMU disabled
    Input: (The arguments were used by caller to input data.)
        r0: boot start code, r1: return address virtual
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        r0-r5: damaged
-------------------------------------------------------------------------------*/
ENTRY(sleep_wakeup_first)
    mov     r4, r0          /* backup of r0 r1 */
    mov     r5, r1


	mov     r0, r4
	mov     r1, r5         /* restore of r0,r1 */

    adrl    r2, sleep_wakeup_save_magic
    ldmia   r2!, {r3-r4}

    ldr     r5, =MSTAR_SLEEP_MAGIC
    cmp     r3, r5
    movne   pc, r0             /* reset to boot start address */
    cmp     r4, #0
    moveq   pc, r0             /* reset to boot start address */

    adrl    r2, sleep_save_mmu_ctr
    ldmia   r2!, {r0,r3,r4}    /* r0: mmu ctrl */

    mov     lr, r1
    ldr     r1, =new_world2
	mcr	    p15, 0, r4, c3, c0, 0		@ load domain access register
	mcr	    p15, 0, r3, c2, c0, 0		@ load page table pointer

    ldr     r3, [r2], #4
	mcr     p15,0,r3,c1,c0,1        /* ACTLR*/
    ldr     r3, [r2], #4
    mcr     p15,0,r3,c1,c0,2        /* CPACR*/

    ldr     r3, [r2], #4
    mcr     p15, 0, r3, c2, c0, 1   /* TTBR 1 */
    ldr     r3, [r2], #4
    mcr     p15, 0, r3, c2, c0, 2   /* TTBCR */

    ldr     r3, [r2], #4
    mcr     p15, 0, r3, c10, c2, 0   /* PRRR */
    ldr     r3, [r2], #4
    mcr     p15, 0, r3, c10, c2, 1   /* NMRR */

    ldr     r3, [r2], #4
    mcr     p15,0,r3,c13,c0,0        /* FCSEIDR */
    ldr     r3, [r2], #4
    mcr     p15,0,r3,c13,c0,1        /* CTXIDR */
    ldr     r3, [r2], #4
    mcr     p15,0,r3,c13,c0,2        /* TPIDRURW */
    ldr     r3, [r2], #4
    mcr     p15,0,r3,c13,c0,3        /* TPIDRURO */
    ldr     r3, [r2], #4
    mcr     p15,0,r3,c13,c0,4        /* TPIDRPRW */

    ldr     r3, [r2], #4
    mcr	    p15,0,r3,c12,c0,0	     /* VBAR */
.if (SAVE_IMP_DEFINE0)
    ldr     r3, [r2], #4
    mcr     p15, 1, r3, c9, c0, 2   /* imp defined */
.endif
.if (SAVE_IMP_DEFINE1)
    ldr     r3, [r2], #4
    mcr     p15, 0, r3, c15, c0, 1
.endif

    /* neon */
    adrl    r2, sleep_save_asimde_FPSCR

    mrc   p15,0,r7,c1,c0,2
    orr   r3, r7, #0x00F00000
    bic   r3, r3, #0xC0000000
    mcr   p15,0,r3,c1,c0,2

    VFPFMRX  r6, FPEXC
    orr   r3, r6, #0x40000000
    VFPFMXR  FPEXC, r3

    ldr   r3, [r2], #4
    VFPFMXR  FPSCR, r3

    ldr   r6, [r2], #4

    VFPFMXR  FPEXC, r6
    mcr   p15,0,r7,c1,c0,2

    /* end neon*/

	b	MMU_ctrl

new_world2:
    mov pc, lr

ENTRY(ensure_environment)
    mov  r12, lr

    // Disable interrupt and sure SVC
    cpsid   if, #Mode_SVC

    //; Disable MMU
    MRC   p15, 0, r0, c1, c0, 0 //; Read Control Register configuration data
    BIC   r0, r0, #0x1
    MCR   p15, 0, r0, c1, c0, 0 //; Write Control Register configuration data

    //; Disable L1 Caches
    BL    disable_l1_cache

    //; Invalidate L1 Caches
    BL    invalidate_l1_cache

    //; Invalidate TLB
    BL    invalidate_tlb

    mov  pc, r12

ENTRY(use_tmp_stack)
    adrl sp, SerPrintPhyStk_bottom
    mov  pc, lr

/* r0: addr, r1:mod*/
ENTRY(get_bank_reg)
    stmfd sp!, {r1-r3,lr}
    adrl  r2, Br_Tbl
    cmp   r1, #6
    movge r0, #1
    bge   get_bank_reg_ret
    add   pc, r2, r1, lsl #3
Br_Tbl:
    cps   #Mode_FIQ
    b     get_bank_reg_rd
    cps   #Mode_IRQ
    b     get_bank_reg_rd
    cps   #Mode_SVC
    b     get_bank_reg_rd
    cps   #Mode_ABT
    b     get_bank_reg_rd
    cps   #Mode_UND
    b     get_bank_reg_rd
    cps   #Mode_SYS

get_bank_reg_rd:
    nop
    nop
    nop
    nop
    stmia r0!, {r4-r14}

    mrs   r1, cpsr
    stmia r0!, {r1}
    dsb

    cps   #Mode_SVC
    mov   r0, #0
get_bank_reg_ret:
    ldmfd sp!, {r1-r3,pc}



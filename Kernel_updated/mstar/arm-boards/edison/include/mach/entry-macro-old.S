#include <asm/hardware/gic.h>

        
	.macro WREG reg, value
	LDR     r12, =\reg
	LDR     r11, =\value
	STR     r11, [r12]
	.endm

	.macro	disable_fiq
         /* 
             mrs r12, cpsr
             orr r12,r12, #64
             msr cpsr_c,r12
         */	
	.endm

	.macro	get_irqnr_preamble, base, tmp
#if 0
    /*set INT controller virtual base*/
    ldr	\base, =0xFD203200
#else
	ldr	\base, =gic_cpu_base_addr
	ldr	\base, [\base]
#endif
	.endm

	.macro	arch_ret_to_user, tmp1, tmp2
	.endm

	/*
	 * The interrupt numbering scheme is defined in the
	 * interrupt controller spec.  To wit:
	 *
	 * Interrupts 0-15 are IPI
	 * 16-28 are reserved
	 * 29-31 are local.  We allow 30 to be used for the watchdog.
	 * 32-1020 are global
	 * 1021-1022 are reserved
	 * 1023 is "spurious" (no interrupt)
	 *
	 * For now, we ignore all local interrupts so only return an interrupt if it's
	 * between 30 and 1020.  The test_for_ipi routine below will pick up on IPIs.
	 *
	 * A simple read from the controller will tell us the number of the highest
	 * priority enabled interrupt.  We then just need to check whether it is in the
	 * valid range for an IRQ (30-1020 inclusive).
	 */

	.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
#if 0
    mov \irqnr, #0
    add \tmp, \base, #0x20      /*set tmp = FIQ status register*/

/*find the highest priority register*/
6:
    cmp \irqnr, #32
    addeq \tmp, \base, #0x50    /*set tmp = IRQ status register*/
    cmp \irqnr, #64    
    #beq 8f
    /* out of irq number */
    beq 5f 

    ldr \irqstat, [\tmp], #4
    teq \irqstat, #0
    addeq \irqnr, \irqnr, #16
    beq 6b

/*find irqnr*/    
7:      
    tst \irqstat, #1
    moveq \irqstat, \irqstat, lsr #1
    addeq \irqnr, \irqnr, #1
    beq 7b
    bne 5f

5:

#else    
	ldr     \irqstat, [\base, #GIC_CPU_INTACK] /* bits 12-10 = src CPU, 9-0 = int # */

    /*SMP patch*/
	push {r2-r3}
    mov r2, \irqstat
    mov r3, \base


@	ldr	\tmp, =1021
	bic     \irqnr, \irqstat, #0x1c00
 
        cmp   \irqnr, #29 /*check if private timer interrupt */ 
                          /*currenty most IP use Mstar irq controller*/     
                          /*if need to use other PPI/SPI number ,add code here*/
        beq   5f       
        
        cmp   \irqnr, #16 /*check if IPI interrupt (0-15)*/ 
                          /*currenty most IP use Mstar irq controller*/     
                          /*if need  to use other PPI/SPI number ,add code here*/
        bcc   5f       

        mrc     p15, 0, \irqnr, c0, c0, 5
        and     \irqnr, #0x3
        cmp     \irqnr, #0
        bne     5f


 
        #if 0
	    cmp         \irqnr, #29
	    cmpcc	\irqnr, \irqnr
	    cmpne	\irqnr, \tmp
	    cmpcs	\irqnr, \irqnr
        #else
            @cmp \irqnr, #31  /*check mstar irq controller interrupt*/
@            WREG 0xfd201300, 0x55
            @bne  5f 
        4:
            mov \irqnr, #128    /*always check mstar irq controller interrupt when irqnr is not 29*/
            ldr \base,=0xFD203200
            add \tmp, \base, #0xB0      /*set tmp = FIQ status register*/
        
        /*find the highest priority register*/
        6:
            cmp \irqnr, #192
            ldr \base,=0xFD203200
            addeq \tmp, \base , #0xF0    /*set tmp = IRQ status register*/
            cmp \irqnr, #256   
            /* out of irq number */
            beq 8f 
        
            ldr \irqstat, [\tmp], #4
            teq \irqstat, #0
            addeq \irqnr, \irqnr, #16
            beq 6b
        
        /*find irqnr*/    
        7:      
            tst \irqstat, #1
            moveq \irqstat, \irqstat, lsr #1
            addeq \irqnr, \irqnr, #1
            beq 7b
            #if 0 
              ldr \base,=0xFD2024E8
              str \irqstat,[\base]
              cmp \irqnr, #149
              streq \irqnr,[\base,#4]
              cmp \irqnr, #150
              streq \irqnr,[\base,#8]
            #endif  
            bne 8f
             
        5:
            #if defined(CONFIG_SMP)
            cmp   \irqnr, #29  
            #else
            cmp   \irqnr, #28  /*force patch private timer irq number under single core status*/
	    #endif 
            ldr \tmp, =1021
            cmpcc \irqnr, \irqnr
            cmpne \irqnr, \tmp
            cmpcs \irqnr, \irqnr
        8:
            /*SMP patch*/
    mov  \base, r3
    mov \irqstat, r2
	pop {r2-r3}
        #endif 
                
#endif        
	.endm

	/* We assume that irqstat (the raw value of the IRQ acknowledge
	 * register) is preserved from the macro above.
	 * If there is an IPI, we immediately signal end of interrupt on the
	 * controller, since this requires the original irqstat value which
	 * we won't easily be able to recreate later.
	 */

	.macro test_for_ipi, irqnr, irqstat, base, tmp
	bic	\irqnr, \irqstat, #0x1c00
	cmp	\irqnr, #16
	strcc	\irqstat, [\base, #GIC_CPU_EOI]
	cmpcs	\irqnr, \irqnr
	.endm

	/* As above, this assumes that irqstat and base are preserved.. */

	.macro test_for_ltirq, irqnr, irqstat, base, tmp
	bic	\irqnr, \irqstat, #0x1c00
	mov 	\tmp, #0
	cmp	\irqnr, #29
	moveq	\tmp, #1
	streq	\irqstat, [\base, #GIC_CPU_EOI]
	cmp	\tmp, #0
	.endm


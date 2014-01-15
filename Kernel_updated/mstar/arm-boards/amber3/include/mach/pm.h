#ifndef __ASM_ARCH_AMBER3_PM_H
#define __ASM_ARCH_AMBER3_PM_H

/*------------------------------------------------------------------------------
    Constant
-------------------------------------------------------------------------------*/
#define PMU_WAKEUP_ADDR_REGL     0x1F001D48
#define PMU_WAKEUP_ADDR_REGL_VIRT     0xFD001D48
#define PMU_WAKEUP_ADDR_LMASK    0x000000FF
#define PMU_WAKEUP_ADDR_REGH     0x1F001D48
#define PMU_WAKEUP_ADDR_REGH_VIRT     0xFD001D48
#define PMU_WAKEUP_ADDR_HMASK    0x0000FF00


#define WAKEUP_ADDR_MASK          0x0000FFF0
#define WAKEUP_FLAG_MASK          0x0000000F
#define WAKEUP_FLAG_INVALID       0
#define WAKEUP_FLAG_SLEPT         1
#define WAKEUP_FLAG_WKUP          2

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



/* Define how many bytes in a word */
#define WORD_SIZE	                4
#define MSTAR_SLEEP_MAGIC         0x4D535452 /*MSTR*/

/* Constants used to calculate data size backup in menory */
#define SLEEPSTATE_DATA_START       0
#define SLEEPSTATE_MAGIC        (SLEEPSTATE_DATA_START		    )
#define SLEEPSTATE_SVC_R4       (SLEEPSTATE_MAGIC   +   WORD_SIZE   )
#define SLEEPSTATE_SVC_R5       (SLEEPSTATE_SVC_R4  +   WORD_SIZE   )
#define SLEEPSTATE_SVC_R6       (SLEEPSTATE_SVC_R5  +   WORD_SIZE   )
#define SLEEPSTATE_SVC_R7       (SLEEPSTATE_SVC_R6  +   WORD_SIZE   )
#define SLEEPSTATE_SVC_R8       (SLEEPSTATE_SVC_R7  +   WORD_SIZE   )
#define SLEEPSTATE_SVC_R9       (SLEEPSTATE_SVC_R8  +   WORD_SIZE   )
#define SLEEPSTATE_SVC_R10      (SLEEPSTATE_SVC_R9  +   WORD_SIZE   )
#define SLEEPSTATE_SVC_R11      (SLEEPSTATE_SVC_R10 +   WORD_SIZE   )
#define SLEEPSTATE_SVC_R12      (SLEEPSTATE_SVC_R11 +   WORD_SIZE   )
#define SLEEPSTATE_SVC_SP       (SLEEPSTATE_SVC_R12 +   WORD_SIZE   )
#define SLEEPSTATE_SVC_SPSR     (SLEEPSTATE_SVC_SP  	+ WORD_SIZE )
#define SLEEPSTATE_SVC_CPSR     (SLEEPSTATE_SVC_SPSR+   WORD_SIZE   )
#define SLEEPSTATE_FIQ_SPSR     (SLEEPSTATE_SVC_CPSR    + WORD_SIZE )
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
#define SLEEPSTATE_CHKSUM       (SLEEPSTATE_SYS_LR	+ WORD_SIZE )
//Neon
#define NEON_DREG_NUM 32
#define SLEEPSTATE_NEONREG      (SLEEPSTATE_CHKSUM	+ WORD_SIZE )
#define SLEEPSTATE_DATA_END     (SLEEPSTATE_NEONREG	+ (WORD_SIZE * 2)*NEON_DREG_NUM )
#define SLEEPDATA_SIZE      ((SLEEPSTATE_DATA_END - SLEEPSTATE_DATA_START) / WORD_SIZE)

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

#endif /* __ASM_ARCH_AMBER3_PM_H */

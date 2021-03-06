################################################################################
#
# Copyright (c) 2006-2007 MStar Semiconductor, Inc.
# All rights reserved.
#
# Unless otherwise stipulated in writing, any and all information contained
# herein regardless in any format shall remain the sole proprietary of
# MStar Semiconductor Inc. and be kept in strict confidence
# (��MStar Confidential Information��) by the recipient.
# Any unauthorized act including without limitation unauthorized disclosure,
# copying, use, reproduction, sale, distribution, modification, disassembling,
# reverse engineering and compiling of the contents of MStar Confidential
# Information is unlawful and strictly prohibited. MStar hereby reserves the
# rights to any and all damages, losses, costs and expenses resulting therefrom.
#
################################################################################


    .include "boot.inc"

    .text
    .set    noreorder

#-------------------------------------------------------------------------------------------------
# BOOT_Entry:
# @param  None
# @return None
# @note   Entry point of Boot code
#-------------------------------------------------------------------------------------------------
    .globl  BOOT_Entry
BOOT_Entry:
    j       ROM_start
    nop

    # Revision ID
    .org    0x0014
    .word   BOOT_VERSION

    .org    0x0018
ROM_start:

    WREG 0xbf203188, 0x000A   # ICE Step issue

    LWREG 0xbf20318C, 0x0060  # ec bridge timeout
    LWREG 0xbf203190, 0x0060  # ec bridge threshold

    WREG 0xbf006010, 0x0000 # watch dog
    WREG 0xbf006014, 0x0000 # watch dog

    # Initialize Regs
    bal     BOOT_InitReg
    nop

    DBGREG  0x0001

    # Initialize Cache
    bal     BOOT_InitCache
    nop


    DBGREG  0x0002

    # Intialize DSPRAM
    #bal     BOOT_InitDSPRAM
    #nop

    DBGREG  0x0003

    # Jump to BOOTROM_Entry
    la      $31, BOOTROM_Entry
    j       $31
    nop


#-------------------------------------------------------------------------------------------------
# BOOT_InitReg
# @param  None
# @return None
# @note   Init CPU Registers
#-------------------------------------------------------------------------------------------------
BOOT_InitReg:

    # INIT DEBUG
    mfc0    $8, $23
    and     $8, (~M_DebugCOUNTDM)
    mtc0    $8, $23

    # CLEAR COUNT
    mtc0    $0, $9

    # INIT STATUS
    # @FIXME: the below instructions will make mips hang?
    # why?
    # li      $8, INITIAL_STATUS
    # mtc0    $8, $12

    # @FIXME: MBoot has below setting for mailbox. why?
    # WREG    0xbf206714, 0xF002

    # INIT CAUSE
    mtc0    $0, $13

    # INIT CONFIG
	li	    $8, INITIAL_CONFIG0
	mtc0	$8, $16


    jr      $31
    nop



#-------------------------------------------------------------------------------------------------
# BOOT_InitCache
# @param  None
# @return None
# @note   Init I-cache and D-cache
#-------------------------------------------------------------------------------------------------
BOOT_InitCache:

    # Clear TagLo & TagHi
    mtc0    $0, $28
    mtc0    $0, $29

    # Config1
    mfc0    $8, $16, 1

    ### INIT I-CACHE ###

    # Line Size $10
    li      $9, M_Config1IL
    and     $9, $8, $9
    srl     $9, $9, S_Config1IL
    beqz    $9, Icache_done    # no Icache
    nop
    li      $10, 2
    sll     $10, $10, $9

    # Sets per way $11
    li      $9, M_Config1IS
    and     $9, $8, $9
    srl     $9, $9, S_Config1IS
    li      $11, 64
    sll     $11, $11, $9

    # Associativity $12
    li      $9, M_Config1IA
    and     $9, $8, $9
    srl     $9, $9, S_Config1IA
    add     $12, $9, 1

    li      $14, A_KSEG0BASE
    move    $15, $11

Icache_loop:
    # Invalidate each tag entry
    cache   Index_Store_Tag_I, 0($14)
    add     $14, $10
    sub     $15, 1
    bnez    $15, Icache_loop
    nop
    move    $15, $11
    sub     $12, 1
    bnez    $12, Icache_loop
    nop

Icache_done:


    ### INIT D-CACHE ###

    # Line Size $10
    li      $9, M_Config1DL
    and     $9, $8, $9
    srl     $9, $9, S_Config1DL
    beqz    $9, Dcache_done    # no Dcache
    nop
    li      $10, 2
    sll     $10, $10, $9

    # Sets per way $11
    li      $9, M_Config1DS
    and     $9, $8, $9
    srl     $9, $9, S_Config1DS
    li      $11, 64
    sll     $11, $11, $9

    # Associativity $12
    li      $9, M_Config1DA
    and     $9, $8, $9
    srl     $9, $9, S_Config1DA
    add     $12, $9, 1

    li      $14, A_KSEG0BASE
    move    $15, $11

Dcache_loop:
    # Invalidate each tag entry
    cache   Index_Store_Tag_D, 0($14)
    add     $14, $10
    sub     $15, 1
    bnez    $15, Dcache_loop
    nop
    move    $15, $11
    sub     $12, 1
    bnez    $12, Dcache_loop
    nop

Dcache_done:

    # Enable cache
    mfc0    $8, $16
    and     $8, (~M_ConfigK0)
    li      $8, (K_CacheAttrCN)
    mtc0    $8, $16


    jr      $31
    nop


#-------------------------------------------------------------------------------------------------
# BOOT_InitDSPRAM
# @param  None
# @return None
# @note   Init Data ScratchPad RAM
#-------------------------------------------------------------------------------------------------
# BOOT_InitDSPRAM:
#    li      $17, 0x10000000 #enable write to spram
#    mtc0    $17, $26

#    li      $16, 0xFFFFFFFF
##    ehb
#    mtc0    $16, $28
##    ehb
#    li      $17, 0x80000000
#    cache   Index_Store_Tag_D, 0($17)


#    li      $16, A_DSPRAM                                               # DSPRAM base offset 0x04000000
##    ehb
#    mtc0    $16, $28
##    ehb
#    cache   Index_Store_Tag_D, 0x10($17)

#    mtc0    $0, $28
#    mtc0    $0, $29
#    mtc0    $0, $26

#    li      $15, 0x80000000
#    or      $16, $16, $17                                               # DSPRAM base 0x84000000

#    li      $15, 0x00001000
#Dspram_loop:
#    sw      $0, 0($16)
#    add     $16, 4
#    sub     $15, 4
#    bnez    $15, Dspram_loop
#    nop

#Dspram_done:

#    jr      $31
#    nop



#-------------------------------------------------------------------------------------------------
# BOOT_ExceptionHandler
# @param  None
# @return None
# @note   Exception handler for general exception or decompression failure at boot code
#-------------------------------------------------------------------------------------------------
    .org    0x0380 #0xBFC00380 if StatusBEV=1 StatusEXL=0 CauseIV=0
    .globl BOOT_ExceptionHandler
BOOT_ExceptionHandler:

    .set    noat

.if (SUPPORT_POWER_SAVING)
    # Check if in off mode. The flag is set in drvPower.c.
    li      $26, REG_TOP_POWER_STATUS   # $26 -> K0, $27 -> K1
    lw      $27, 0($26)
    li      $26, 0x03
    and     $27, $26
    bne     $27, $26, BOOT_ExceptionHandler_RESET_CPU
    nop

.if (0)
    # Trun on DRAM analog power
    li      $26, REG_PDN_DPLBG
    lw      $27, 0($26)
    andi    $27, $27, 0xFE
    sw      $27, 0($26)
    li      $26, REG_PDN_DMIBEX
    lw      $27, 0($26)
    andi    $27, $27, (~(1<<4) & 0xFF)
    sw      $27, 0($26)

    # Set DRAM output to be input
    li      $2, REG_MIU_OENZ
    lw      $3, 0($2)
    andi    $3, $3, (~(7<<12) & 0xFFFF)
    sw      $3, 0($2)

    # Turn on PLLs
    # 1. Turn on MPLL
    li      $26, REG_MPLL_LOOP_DIV2
    li      $27, 0x24
    sw      $27, 0($26)
    # Adjust the MPLL output div 2
    li      $26, REG_MPLL_OUTPUT_DIV2
    sw      $0, 0($26)

#    li      $26, REG_MPLL
#    lw      $27, 0($26)
#    andi    $27, $27, (~(1<<4) & 0xFFFF)
#    sw      $27, 0($26)
#    ori     $27, $27, (1<<5)
#    sw      $27, 0($26)
#    andi    $27, $27, (~(1<<5) & 0xFFFF)
#    sw      $27, 0($26)

    # Check MPLL lock status
    li      $26, REG_MPLL_CHECK
    li      $27, (1<<4)
    sw      $27, 0($26)
1:
    li      $26, REG_MPLL_LOCKSTATUS
    lw      $27, 0($26)
    andi    $27, $27, (1<<4)
    li      $26, (1<<4)
    bne     $26, $27, 1b
    nop

    li      $26, REG_MPLL_CHECK
    sw      $0, 0($26)

    # 1. Turn on MEMPLL
    li      $26, REG_MEMPLL
    sw      $0, 0($26)

    # Check MEMPLL lock status
    li      $26, REG_MEMPLL_CHECK
    li      $27, (1<<4)
    sw      $27, 0($26)
1:
    li      $26, REG_MEMPLL_LOCKSTATUS
    lw      $27, 0($26)
    andi    $27, $27, (1<<15)
    li      $26, (1<<15)
    bne     $26, $27, 1b
    nop

    li      $26, REG_MEMPLL_CHECK
    sw      $0, 0($26)

    # 2. Turn on MCUPLL
    li      $26, REG_MCUPLL
    lw      $27, 0($26)
    andi    $27, (~(1<<5) & 0xFFFF)
    sw      $27, 0($26)
    # Check MCUPLL lock status
    li      $26, REG_MCUPLL_CHECK
    li      $27, (1<<12)
    sw      $27, 0($26)
1:
    li      $26, REG_MCUPLL_LOCKSTATUS
    lw      $27, 0($26)
    andi    $27, $27, 1
    li      $26, 1
    bne     $26, $27, 1b
    nop

    li      $26, REG_MCUPLL_CHECK
    sw      $0, 0($26)


    # 3. Turn on LPLL
    li      $26, REG_LPLL
    lw      $27, 0($26)
    andi    $27, (~(1<<5) & 0xFFFF)
    sw      $27, 0($26)

    # Wake up DRAM.
    lui     $26, REG_MIU_ADDR_H16

    # turn off self refresh power down
    lh      $27, REG_MIU_CKE($26)
    andi    $27, ((~MIU_SELF_REFRESH) & 0xFFFF)
    sh      $27, REG_MIU_CKE($26)

    # triggle refresh to issue DQS_CNT_RN
    lh      $27, REG_MIU_INIT($26)
    ori     $27, MIU_TRIG_REFRESH
    sh      $27, REG_MIU_INIT($26)
    andi    $27, ((~MIU_TRIG_REFRESH) & 0xFFFF)
    sh      $27, REG_MIU_INIT($26)

    # turn on command request
    li      $27, ((~MIU_CMD_RQ) & 0xFFFF)
    sh      $27, REG_MIU_RQ0_MASK($26)

    # Delay 200ns
    nop
    nop
    nop

    # assert reset, DLL = 1
    lh      $27, REG_MIU_RD_IN_PHASE($26)
    ori     $27, MIU_DISABLE_DLL
    sh      $27, REG_MIU_RD_IN_PHASE($26)

    # Delay 200ns
    nop
    nop
    nop

    # triggle load mode register extend
    lh      $27, REG_MIU_INIT($26)
    ori     $27, MIU_TRIG_LMR_EXT
    sh      $27, REG_MIU_INIT($26)
    andi    $27, ((~MIU_TRIG_LMR_EXT) & 0xFFFF)
    sh      $27, REG_MIU_INIT($26)

    # Delay 200ns
    nop
    nop
    nop

    # deassert reset, DLL = 0
    lh      $27, REG_MIU_RD_IN_PHASE($26)
    andi    $27, ((~MIU_DISABLE_DLL) & 0xFFFF)
    sh      $27, REG_MIU_RD_IN_PHASE($26)

    # Delay 200ns
    nop
    nop
    nop

    # triggle load mode register extend
    lh      $27, REG_MIU_INIT($26)
    ori     $27, MIU_TRIG_LMR_EXT
    sh      $27, REG_MIU_INIT($26)
    andi    $27, ((~MIU_TRIG_LMR_EXT) & 0xFFFF)
    sh      $27, REG_MIU_INIT($26)

    # Delay 200ns
    nop
    nop
    nop

    # triggle auto refresh
    lh      $27, REG_MIU_INIT($26)
    ori     $27, MIU_TRIG_REFRESH
    sh      $27, REG_MIU_INIT($26)
    andi    $27, ((~MIU_TRIG_REFRESH) & 0xFFFF)
    sh      $27, REG_MIU_INIT($26)

    # Delay 1000ns
    li      $27, 20
BOOT_ExceptionHandler_Delay100NS:
    addiu   $27, $27, -1
    bne     $27, $0, BOOT_ExceptionHandler_Delay100NS
    nop

    # trun off single step[8]
    lh      $27, REG_MIU_INIT($26)
    andi    $27, ((~MIU_SINGLE_STEP_ON) & 0xFFFF)
    sh      $27, REG_MIU_INIT($26)

    sh      $0, REG_MIU_RQ0_MASK($26)   # turn on group 0 all request
#    sh      $0, REG_MIU_RQ1_MASK($26)   # turn on group 1 all request
#    sh      $0, REG_MIU_RQ2_MASK($26)   # turn on group 2 all request
#    sh      $0, REG_MIU_RQ3_MASK($26)   # turn on group 3 all request
    li      $27, 0x0100
    sh      $27, REG_MIU_RQ3_MASK($26)   # turn on group 3 all request except for PCI

.endif #(0)


    # Set StatusBEV = 0. This interrupt will go to normal exception vector.
    mfc0    $26, $12
    li      $27, ~M_StatusBEV
    and     $26, $27
    mtc0    $26, $12

    eret
    nop

BOOT_ExceptionHandler_RESET_CPU:

.endif


.if (BOOT_DEBUG_VERSION)
    # trap here
    j   BOOT_ExceptionHandler
    nop
.else
    # reset CPU
    lui     $8, 0xBF20
    ori     $9, $0, 0x8000
    sw      $9, 0x2000($8)
.endif

    .set    at


.if (SUPPORT_POWER_SAVING)
#-------------------------------------------------------------------------------------------------
# BOOT_CPU_DRAM_Standby
# @param  None
# @return None
# @note   Let DRAM & CPU enter standby mode
#-------------------------------------------------------------------------------------------------
    .org    0xa80      # Cannot change it, _MDrv_Power_EnterPowerSavingMode will call it.
BOOT_CPU_DRAM_Standby:

.if (0)
    lui     $2, REG_MIU_ADDR_H16       # $2 -> v0, $3 -> v1

    # turn off dynamic cke control  : [2]
    lh      $3, REG_MIU_CKE($2)
    andi    $3, ((~MIU_DYN_CKE_CTRL) & 0xFFFF)
    sh      $3, REG_MIU_CKE($2)

    # mask group0 request, except dram command
    li      $3, ((~MIU_CMD_RQ) & 0xFFFF)
    sh      $3, REG_MIU_RQ0_MASK($2)

    li      $3, 0xFFFF
    sh      $3, REG_MIU_RQ1_MASK($2)    # mask group 1 request
    sh      $3, REG_MIU_RQ2_MASK($2)    # mask group 2 request
    sh      $3, REG_MIU_RQ3_MASK($2)    # mask group 3 request

    # force d2a_fifo_en on
    lh      $3, REG_FORCE_CKE_IN($2)
    ori     $3, MIU_FORCE_D2A_FIFO_EN
    sh      $3, REG_FORCE_CKE_IN($2)

    # Delay 200ns
    nop
    nop
    nop

    # trun on single step[8]
    lh      $3, REG_MIU_INIT($2)
    ori     $3, MIU_SINGLE_STEP_ON
    sh      $3, REG_MIU_INIT($2)

    # Delay 200ns
    nop
    nop
    nop

    # triggle precharge all
    lh      $3, REG_MIU_INIT($2)
    ori     $3, MIU_TRIG_PRECHARGE
    sh      $3, REG_MIU_INIT($2)
    andi    $3, ((~MIU_TRIG_PRECHARGE) & 0xFFFF)
    sh      $3, REG_MIU_INIT($2)

    # Delay 200ns
    nop
    nop
    nop

    # triggle auto refresh
    lh      $3, REG_MIU_INIT($2)
    ori     $3, MIU_TRIG_REFRESH
    sh      $3, REG_MIU_INIT($2)
    andi    $3, ((~MIU_TRIG_REFRESH) & 0xFFFF)
    sh      $3, REG_MIU_INIT($2)

    # Delay 200ns
    nop
    nop
    nop

    # turn off command request
    li      $3, 0xFFFF
    sh      $3, REG_MIU_RQ0_MASK($2)

    # Delay 200ns
    nop
    nop
    nop

    # turn on self refresh power down
    lh      $3, REG_MIU_CKE($2)
    ori     $3, MIU_SELF_REFRESH
    sh      $3, REG_MIU_CKE($2)

    # force d2a_fifo_en off
    lh      $3, REG_FORCE_CKE_IN($2)
    andi    $3, ((~MIU_FORCE_D2A_FIFO_EN) & 0xFFFF)
    sh      $3, REG_FORCE_CKE_IN($2)

    # Turn off PLLs
    # 1. Turn off LPLL
    li      $2, REG_LPLL
    lw      $3, 0($2)
    ori     $3, (1<<5)
    sw      $3, 0($2)

    # 2. Turn off MCUPLL
    li      $2, REG_MCUPLL
    lw      $3, 0($2)
    ori     $3, (1<<5)
    sw      $3, 0($2)


    # 3. Turn off MEMPLL
    li      $2, REG_MEMPLL
    lw      $3, 0($2)
    ori     $3, (1<<9)
    sw      $3, 0($2)

    # 3. Turn off MPLL
    #     # Adjust the MPLL output div 2
    li      $2, REG_MPLL_OUTPUT_DIV2
    addi    $3, $0, 255
    sw      $3, 0($2)

    li      $2, REG_MPLL_LOOP_DIV2
    sw      $0, 0($2)

#    li      $2, REG_MPLL
#    lw      $3, 0($2)
#    ori     $3, $3, (1<<4)
#    sw      $3, 0($2)

    # Set DRAM output to be input
    li      $2, REG_MIU_OENZ
    lw      $3, 0($2)
    ori     $3, $3, (7<<12)
    sw      $3, 0($2)

    # Trun off DRAM analog power
    li      $2, REG_PDN_DPLBG
    addi    $3, $0, 0xFF
    sw      $3, 0($2)
    li      $2, REG_PDN_DMIBEX
    addi    $3, $0, 0xFF
    sw      $3, 0($2)

    # Select Xtal crystal clock for RIU & MIU
#    li      $2, REG_CKG_RIU_MCU_MIU_TS0
#    lw      $3, 0($2)
#    andi    $3, $3, 0xFFF0
#    ori     $3, $3, 0x000C
#    sw      $3, 0($2)

    # Select Xtal crystal clock for ISP
#    li      $2, REG_CKG_COP_VD_VD200_ISP
#    lw      $3, 0($2)
#    andi    $3, $3, 0x0FFF
#    ori     $3, $3, 0xC000
#    sw      $3, 0($2)

.endif #(0)

    # Enable Interrupt
    mfc0    $2, $12
    nop
    or      $2, $2, M_StatusIE
    mtc0    $2, $12
    nop
    nop
    nop

    # Enter MIPS power saving mode.
    wait
    nop

    jr      $31
    nop

    .set    at

.endif


#-------------------------------------------------------------------------------------------------
# BOOT_Pad
# @param  None
# @return None
# @note   Pad with zeros (Reserve SIZE_BOOT_RESERVED Bytes for boot)
#-------------------------------------------------------------------------------------------------
    .org    (SIZE_BOOT_RESERVED-4)
    .word   0



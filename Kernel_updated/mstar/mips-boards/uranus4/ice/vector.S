    .set noreorder
    .include "system.inc"
    #include "ice.h"
reset_vector:
    nop
# li $29, 0xA0600000
# li $29, 0x81f00000 #for DRAM test
# li $29, 0xA1f00000 #for DRAM test (32M)
#    la $31, main
#    j $31
#    nop

######################################
ROM_start:

    # Clear CPU reset register
    #WREG 0xbf803ddc, 0x029f

    nop
    nop
    nop
    bal  BOOT_InitReg1
    nop
    nop
    nop

    # Initialize Regs
    bal BOOT_InitReg
    nop

    DBGREG 0x0001
    # Initialize Cache
#   bal BOOT_InitCache
#   nop
    bal sys_init_cache
    nop

    la      $31, KERNEL_ENTRY_ADDR
    j       $31
    nop

    DBGREG 0x0002

    # Intialize DSPRAM
    #bal BOOT_InitDSPRAM
    nop

    # Init UART
    #bal BOOT_InitUART
    nop

    #PUTC '\r'
    #PUTC '\n'
    #PUTC 'B'


    #DBGREG 0x0003

#    la $31, main
#    j $31
    nop

BOOT_InitReg1:
    nop
    nop
    nop
    jr $31
    nop
#-------------------------------------------------------------------------------------------------
# BOOT_InitReg
# @param None
# @return None
# @note Init CPU Registers
#-------------------------------------------------------------------------------------------------
BOOT_InitReg:

    # INIT DEBUG
    mfc0 $8, $23
    and $8, (~M_DebugCOUNTDM)
    mtc0 $8, $23

    # CLEAR COUNT
    mtc0 $0, $9

    # INIT STATUS
    li $8, INITIAL_STATUS
    mtc0 $8, $12

    # INIT CAUSE
    mtc0 $0, $13

    # INIT CONFIG
 li $8, INITIAL_CONFIG0
 mtc0 $8, $16


    jr $31
    nop



#-------------------------------------------------------------------------------------------------
# BOOT_InitCache
# @param None
# @return None
# @note Init I-cache and D-cache
#-------------------------------------------------------------------------------------------------
BOOT_InitCache:

    # Clear TagLo & TagHi
    mtc0 $0, $28
    mtc0 $0, $29

    # Config1
    mfc0 $8, $16, 1

    ### INIT I-CACHE ###

    # Line Size $10
    li $9, M_Config1IL
    and $9, $8, $9
    srl $9, $9, S_Config1IL
    beqz $9, Icache_done # no Icache
    nop
    li $10, 2
    sll $10, $10, $9

    # Sets per way $11
    li $9, M_Config1IS
    and $9, $8, $9
    srl $9, $9, S_Config1IS
    li $11, 64
    sll $11, $11, $9

    # Associativity $12
    li $9, M_Config1IA
    and $9, $8, $9
    srl $9, $9, S_Config1IA
    add $12, $9, 1

    li $14, A_KSEG0BASE
    move $15, $11

Icache_loop:
    # Invalidate each tag entry
    cache Index_Store_Tag_I, 0($14)
    add $14, $10
    sub $15, 1
    bnez $15, Icache_loop
    nop
    move $15, $11
    sub $12, 1
    bnez $12, Icache_loop
    nop

Icache_done:


    ### INIT D-CACHE ###

    # Line Size $10
    li $9, M_Config1DL
    and $9, $8, $9
    srl $9, $9, S_Config1DL
    beqz $9, Dcache_done # no Dcache
    nop
    li $10, 2
    sll $10, $10, $9

    # Sets per way $11
    li $9, M_Config1DS
    and $9, $8, $9
    srl $9, $9, S_Config1DS
    li $11, 64
    sll $11, $11, $9

    # Associativity $12
    li $9, M_Config1DA
    and $9, $8, $9
    srl $9, $9, S_Config1DA
    add $12, $9, 1

    li $14, A_KSEG0BASE
    move $15, $11

Dcache_loop:
    # Invalidate each tag entry
    cache Index_Store_Tag_D, 0($14)
    add $14, $10
    sub $15, 1
    bnez $15, Dcache_loop
    nop
    move $15, $11
    sub $12, 1
    bnez $12, Dcache_loop
    nop

Dcache_done:

    # Enable cache
    mfc0 $8, $16
    and $8, (~M_ConfigK0)
    li $8, (K_CacheAttrCN)
    mtc0 $8, $16


    jr $31
    nop


#-------------------------------------------------------------------------------------------------
# BOOT_InitDSPRAM
# @param None
# @return None
# @note Init Data ScratchPad RAM
#-------------------------------------------------------------------------------------------------
BOOT_InitDSPRAM:
    li $17, 0x10000000 #enable write to spram
    mtc0 $17, $26

    li $16, 0xFFFFFFFF
# ehb
    mtc0 $16, $28
# ehb
    li $17, 0x80000000
    cache Index_Store_Tag_D, 0($17)


    li $16, A_DSPRAM # DSPRAM base offset 0x04000000
# ehb
    mtc0 $16, $28
# ehb
    cache Index_Store_Tag_D, 0x10($17)

    mtc0 $0, $28
    mtc0 $0, $29
    mtc0 $0, $26

    li $15, 0x80000000
    or $16, $16, $17 # DSPRAM base 0x84000000

    li $15, 0x00001000
Dspram_loop:
    sw $0, 0($16)
    add $16, 4
    sub $15, 4
    bnez $15, Dspram_loop
    nop

Dspram_done:


#-------------------------------------------------------------------------------------------------
# BOOT_InitUART
# @param None
# @return None
# @note Init UART
#-------------------------------------------------------------------------------------------------
BOOT_InitUART:

    #Disable all UART interrupts
    WREG UART_IER, 0x00

    #Reset receiver and transmitr
    WREG UART_FCR, 0x07

    #Set 8 bit char, 1 stop bit, no parity
    WREG UART_LCR, 0x03

    #Set baud rate
    WREG UART_LCR, 0x83
    WREG UART_DLM, ((UART_DIVISOR >> 8) & 0x00ff)
    WREG UART_DLL, (UART_DIVISOR & 0x00ff)
    WREG UART_LCR, 0x03

    jr $31
    nop

################################################################################
#
# Copyright (c) 2006-2007 MStar Semiconductor, Inc.
# All rights reserved.
#
# Unless otherwise stipulated in writing, any and all information contained
# herein regardless in any format shall remain the sole proprietary of
# MStar Semiconductor Inc. and be kept in strict confidence
# (¡§MStar Confidential Information¡¨) by the recipient.
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
    .globl  BOOTROM_Entry
BOOTROM_Entry:

    DBGREG  0x0010

    # Power On Sequence
    bal     BOOT_PowerOnSeq
    nop

    DBGREG  0x001A

    # Init UART
    bal     BOOT_InitUART
    nop

	PUTC '\r'
    PUTC '\n'

# do MIU BIST --------------------------------------------------------
	#    MIU0 test
.if MIU0_DO_BIST	
	PUTC 'B'
	PUTC 'I'
	PUTC 'S'
	PUTC 'T'
	PUTC '0'
	PUTC ' '
	ori	$7, $0, MIU0_DO_BIST_PATTERN
	
	# set test length
	LWREG 	0xbf8025c8 MIU0_DO_BIST_LEN_L
	LRREG	$4, 0xbf8025cc
	andi	$4, $4, 0xFF00
	ori		$4, $4, MIU0_DO_BIST_LEN_H
	LSSSW    0xbf8025cc, $4
	
	bal		BOOTROM_MIU_BIST
	nop
.endif	

	#    MIU1 test
.if (MIU1_DO_BIST)
	PUTC 'B'
	PUTC 'I'
	PUTC 'S'
	PUTC 'T'
	PUTC '1'
	PUTC ' '

	LRREG	$4, 0xbf8025e0
	ori		$4, $4, 0x8000
	LSSSW    0xbf8025e0, $4

	LRREG	$4, 0xbf8025c0
	andi	$4, $4, 0xFF00
	LSSSW    0xbf8025c0, $4

	ori	$7, $0, MIU1_DO_BIST_PATTERN
	
	# set test length
	LWREG 	0xbf8025c8 MIU1_DO_BIST_LEN_L
	LRREG	$4, 0xbf8025cc
	andi	$4, $4, 0xFF00
	ori		$4, $4, MIU1_DO_BIST_LEN_H
	LSSSW    0xbf8025cc, $4
	
	bal		BOOTROM_MIU_BIST
	nop

	LRREG	$4, 0xbf8025c0
	andi	$4, $4, 0xFF00
	LSSSW    0xbf8025c0, $4

	LRREG	$4, 0xbf8025e0
	andi	$4, $4, 0x7FFF
	LSSSW    0xbf8025e0, $4
.endif
# EOF MIU BIST ---------------------------------------------------

    PUTC 'B'

    # Increase Flash clock
#    bal     BOOT_Flash_Clock
    nop

    DBGREG	0x001B
    PUTC 'O'

    # Copy Zipped binary from ROM to RAM
    bal     BOOTROM_CopyZipBin
    nop

    DBGREG  0x001C
    PUTC 'O'

    # Copy bootram from ROM to DRAM
    bal     BOOT_CopyBootRAM
    nop

    DBGREG  0x0001D
    PUTC 'T'

    # Jump to BootRam_Entry
    la      $31, BOOTRAM_Entry
    j       $31
    nop


#-------------------------------------------------------------------------------------------------
# BOOTROM_MIU_BIST
# @param  $7 = BIST pattern
# @return None
# @note   do MIU0 bist, maybe used to test MIU1 too. (size is 32MB)
#-------------------------------------------------------------------------------------------------
BOOTROM_MIU_BIST:
	#do MIU BIST(build in self test)------------------------------
	WREG 	0xbf8025c4 0x0000

	#   test pattern
	sw      $7, (0x25d0)($8)
	#   enable test, order (high byte then low byte) is important
	LRREG	$4, 0xbf8025c0
	andi	$4, $4, 0x00FF
	LSSSW    0xbf8025c0, $4
	LRREG	$4, 0xbf8025c0
	andi	$4, $4, 0xFF00
	LSSSW    0xbf8025c0, $4		# clear enable bit
	LRREG	$4, 0xbf8025c0
	andi	$4, $4, 0xFF00
	ori		$4, $4, 0x0003
	LSSSW    0xbf8025c0, $4		# pattern from data
	#   check result
MIU_BIST_CHK:
	LRREG	$4, 0xbf8025c0
	andi	$4, $4, 0x8000
    bnez	$4, MIU_BIST_CHK_RES
    nop
	j MIU_BIST_CHK
	nop
MIU_BIST_CHK_RES:
	LRREG	$4, 0xbf8025c0
	andi	$4, $4, 0x4000
    bnez	$4, MIU_BIST_FAIL
    nop
	#do MIU0 BIST PASS-----------------
	PUTC 'O'
	PUTC 'K'
	PUTC '\n'
	PUTC '\r'
    jr      $31
    nop
MIU_BIST_FAIL:
	PUTC 'N'
	PUTC 'G'
	PUTC '\n'
	PUTC '\r'
MIU_DAMN:
	j MIU_DAMN
	nop

#-------------------------------------------------------------------------------------------------
# BOOTROM_CopyZipBin
# @param  None
# @return None
# @note   Copy Zipped binary from ROM to RAM via DMA or CPU
#-------------------------------------------------------------------------------------------------
BOOTROM_CopyZipBin:

.if (SUPPORT_UNZIP)
    li      $8, ADDR_ZIPBIN_ROM_START
    li      $9, ADDR_ZIPBIN_RAM_START
    li      $10, ADDR_ZIPBIN_RAM_END
.else
    li      $8, ADDR_ZIPBIN_ROM_START
    li      $9, ADDR_SYSTEM_RAM_START
    li      $10, ADDR_SYSTEM_RAM_START+(ADDR_ZIPBIN_RAM_END-ADDR_ZIPBIN_RAM_START)
.endif

.if (BOOT_COPY_VIA_DMA)

    and     $8, ~0xBFC00000
    and     $9, ~0xA0000000
    and     $10, ~0xA0000000

    li      $12, REG_SFSH_DMA_SIZE_L
    sub     $13, $10, $9
    add     $13, $13, SFSH_DMA_ALIGN-1
    and     $13, $13, ~(SFSH_DMA_ALIGN-1)
    and     $14, $13, 0xffff
    sw      $14, 0($12)
    li      $12, REG_SFSH_DMA_SIZE_H
    srl     $14, $13, 0x10
    sw      $14, 0($12)

    li      $12, REG_SFSH_DMA_DRAMSTART_L
    add     $13, $9, 0
    and     $14, $13, 0xffff
    sw      $14, 0($12)
    li      $12, REG_SFSH_DMA_DRAMSTART_H
    srl     $14, $13, 0x10
    sw      $14, 0($12)

    li      $12, REG_SFSH_DMA_SPISTART_L
    add     $13, $8, 0
    and     $14, $13, 0xffff
    sw      $14, 0($12)
    li      $12, REG_SFSH_DMA_SPISTART_H
    srl     $14, $13, 0x10
    sw      $14, 0($12)

    WREG    REG_SFSH_DMA_CMD, (1<<5)        #[0] trigger, [5]0: little-endian 1: big-endian
    WREG    REG_SFSH_DMA_CMD, ((1<<5) | 1)    #trigger

1:
    li      $12, REG_SFSH_DMA_STATE
    lw      $26, 0($12)
    and     $26, 0x2 #finished
    beqz    $26, 1b
    nop

.else

1:
    ld      $12, 0($8)
    ld      $14, 8($8)
    sd      $12, 0($9)
    sd      $14, 8($9)
    addu    $8, $8, 16
    addu    $9, $9, 16
    bltu    $9, $10, 1b
    nop

.endif

    jr      $31
    nop



#-------------------------------------------------------------------------------------------------
# BOOT_CopyBootRAM
# @param  None
# @return None
# @note   Copy bootram from ROM to DRAM
#-------------------------------------------------------------------------------------------------
BOOT_CopyBootRAM:

.if (BOOT_COPY_VIA_DMA)

    la      $8, _ld_bootram_ROM_start
    la      $9, _ld_bootram_RAM_start
    la      $10, _ld_bootram_RAM_end

    and     $8, ~0xBFC00000
    and     $9, ~0xA0000000
    and     $10, ~0xA0000000

    li      $12, REG_SFSH_DMA_SIZE_L
    sub     $13, $10, $9
    add     $13, $13, SFSH_DMA_ALIGN-1
    and     $13, $13, ~(SFSH_DMA_ALIGN-1)
    and     $14, $13, 0xffff
    sw      $14, 0($12)
    li      $12, REG_SFSH_DMA_SIZE_H
    srl     $14, $13, 0x10
    sw      $14, 0($12)

    li      $12, REG_SFSH_DMA_DRAMSTART_L
    add     $13, $9, 0
    and     $14, $13, 0xffff
    sw      $14, 0($12)
    li      $12, REG_SFSH_DMA_DRAMSTART_H
    srl     $14, $13, 0x10
    sw      $14, 0($12)

    li      $12, REG_SFSH_DMA_SPISTART_L
    add     $13, $8, 0
    and     $14, $13, 0xffff
    sw      $14, 0($12)
    li      $12, REG_SFSH_DMA_SPISTART_H
    srl     $14, $13, 0x10
    sw      $14, 0($12)

    WREG    REG_SFSH_DMA_CMD, (1<<5)        #[0] trigger, [5]0: little-endian 1: big-endian
    WREG    REG_SFSH_DMA_CMD, ((1<<5) | 1)    #trigger

1:
    li      $12, REG_SFSH_DMA_STATE
    lw      $26, 0($12)
    and     $26, 0x2 #finished
    beqz    $26, 1b
    nop

.else

    la      $8, _ld_bootram_ROM_start
    la      $9, _ld_bootram_RAM_start
    or      $9, 0xA0000000
    la      $10, _ld_bootram_RAM_end
    or      $10, 0xA0000000

1:
    ld      $11, 0($8)      #ld=lw lw to save time
    ld      $13, 8($8)
    sd      $11, 0($9)
    sd      $13, 8($9)
    addu    $8, $8, 16
    addu    $9, $9, 16
    bltu    $9, $10, 1b
    nop

.endif

    jr      $31
    nop



#-------------------------------------------------------------------------------------------------
# BOOT_InitUART
# @param  None
# @return None
# @note   Init UART
#-------------------------------------------------------------------------------------------------
BOOT_InitUART:

	#Setup UART clk to 123MHZ(for higher bandrate)
	WREG 0xbf803c8c 0x0008
	LWREG 0xbf803c78 0x01C0

.if (SUPPORT_T2_UART)

    #Disable all UART interrupts
    WREG    UART_IER, 0x00

    #Reset receiver and transmitr
    WREG    UART_FCR, 0x07

    #Set 8 bit char, 1 stop bit, no parity
    WREG    UART_LCR, 0x03

    #Set baud rate
    WREG    UART_LCR, 0x83
    WREG    UART_DLM, ((UART_DIVISOR >> 8) & 0x00ff)
    WREG    UART_DLL, UART_DIVISOR & 0x00ff
    WREG    UART_LCR, 0x03

.else

	#Disable all UART interrupts
	LRREG	$4, UART_IER
	andi	$4, $4, 0x00FF
	ori	    $4, $4, 0x0000
	LSSSW    UART_IER, $4
	#WREG    UART_IER, 0x00

	#Reset receiver and transmitr
	LRREG	$4, UART_FCR
	andi	$4, $4, 0xFF00
	ori	    $4, $4, 0x0007
	LSSSW    UART_FCR, $4
	#WREG    UART_FCR, 0x07

    #Set 8 bit char, 1 stop bit, no parity
	LRREG	$4, UART_LCR
	andi	$4, $4, 0x00FF
	ori	    $4, $4, 0x0300
	LSSSW    UART_LCR, $4
	#WREG    UART_LCR, 0x03

    #Set baud rate

	LRREG	$4, UART_LCR
	andi	$4, $4, 0x00FF
	ori	    $4, $4, 0x8300
	LSSSW    UART_LCR, $4
	#WREG    UART_LCR, 0x83

	LRREG	$4, UART_DLM
	andi	$4, $4, 0x0000
	ori	    $4, $4, UART_DIVISOR
	LSSSW    UART_DLM, $4
	#WREG    UART_DLM, ((UART_DIVISOR >> 8) & 0x00ff)

	LRREG	$4, UART_LCR
	andi	$4, $4, 0x00FF
	ori	    $4, $4, 0x0300
	LSSSW    UART_LCR, $4
	#WREG    UART_LCR, 0x03

.endif

    jr      $31
    nop


#-------------------------------------------------------------------------------------------------
# BOOT_PowerOnSeq
# @param  None
# @return None
# @note   Power On Sequence for Chip
#-------------------------------------------------------------------------------------------------
BOOT_PowerOnSeq:

    #Disable WDT, or MIPS will continously reset
    WREG    0xbf8078c0, 0xaa55

    #Initialize MIU-------------------------------

	#clear power down
	LWREG 0xbf804a10 0x0000
	LWREG 0xbf804a10 0x0000

	#clear power down clocks
	LWREG 0xbf804a18 0x0000

	#MPLL
	LWREG 0xbf804a28 0x0024
	LWREG 0xbf804a20 0x0506

	#MPLL div clocks enable
	LWREG 0xbf804a2c 0x0000
	LWREG 0xbf804b80 0x0000

	#DDR MIU
	LRREG	$4, 0xbf803c48
	andi	$4, $4, 0x00FF
	ori	    $4, $4, 0x0000
	LSSSW    0xbf803c48, $4

	#assign all IP to MIU_0
	LWREG 0xbf8025e0 0x0000
	LWREG 0xbf8025e4 0x0000
	LWREG 0xbf8025e8 0x0000
	LWREG 0xbf8025ec 0x0000

	#software reset of MIU0
	LRREG	$4, 0xbf80243c
	ori	    $4, $4, 0x0001
	LSSSW    0xbf80243c, $4
	andi	$4, $4, 0xFFFE
	LSSSW    0xbf80243c, $4

	#software reset of MIU1
.if (MIU1_INIT)
	LRREG	$4, 0xbf800c3c
	ori	    $4, $4, 0x0001
	LSSSW    0xbf800c3c, $4
	andi	$4, $4, 0xFFFE
	LSSSW    0xbf800c3c, $4
.endif

	#MIU_EnableGroupMask
	#MIU0
	LWREG 0xbf80248c 0xFFFF
	LWREG 0xbf8024cc 0xFFFF
	LWREG 0xbf80250c 0xFFFF
	LWREG 0xbf80254c 0xffff
	#MIU1
.if (MIU1_INIT)
	LWREG 0xbf800c8c 0xFFFF
	LWREG 0xbf800ccc 0xFFFF
	LWREG 0xbf800d0c 0xFFFF
	LWREG 0xbf800d4c 0xffff
.endif

	#disable MIU_0 protection
	LRREG	$4, 0xbf802580
	andi	$4, $4, 0xFF00
	ori	    $4, $4, 0x0000
	LSSSW    0xbf802580, $4

	#disable MIU_1 protection
.if (MIU1_INIT)
	LRREG	$4, 0xbf800d80
	andi	$4, $4, 0xFF00
	ori	    $4, $4, 0x0000
	LSSSW    0xbf800d80, $4
.endif

    #init MIU0 table
	LWREG 0xbf802400 0x0000
	LWREG 0xbf802404 MIU0_LEN
.if (ENABLE_ODT)
	LWREG 0xbf802408 0x0E98
.else
	LWREG 0xbf802408 0x0E90
.endif
	LWREG 0xbf80240c 0x4060
	LWREG 0xbf802410 0x6672
	LWREG 0xbf802414 0x3264
	LWREG 0xbf802418 0x3328
	LWREG 0xbf80241c 0x95EA
	LWREG 0xbf802420 0x0000
	LWREG 0xbf802424 0x0000
	LWREG 0xbf802428 0x1040
	LWREG 0xbf80242c 0x2000
	LWREG 0xbf802430 0x3000
	LWREG 0xbf802434 0x5757
	LWREG 0xbf802438 0x1800
	LWREG 0xbf80243c 0x0C00
	LWREG 0xbf802440 MIU_FREQ 
	LWREG 0xbf802444 0x0000
	LWREG 0xbf802448 0x8000
	LWREG 0xbf80244c 0x0000
	LWREG 0xbf802450 0x0000
	LWREG 0xbf802454 0x0000
	LWREG 0xbf802458 0x0000
	LWREG 0xbf80245c 0x0000
	LWREG 0xbf802460 MIU0_PHASE
	LWREG 0xbf802464 0x0055
	LWREG 0xbf802468 0x0000
	LWREG 0xbf80246c MIU0_DRAM_PHASE
	LWREG 0xbf802470 0x0040
	LWREG 0xbf802474 0x00C3
	LWREG 0xbf802478 0x0000
	LWREG 0xbf80247c 0x4090

	#delay for a while
	DELAYUS 100000

	#toggle MIU0 reg_4x_mode
	LRREG	$4, 0xbf802408
	andi	$4, $4, 0xF7FF
	LSSSW    0xbf802408, $4
	ori		$4, $4, 0x0800
	LSSSW    0xbf802408, $4

	#MIU_Adjust_DDRPLL_Speed
	LRREG	$4, 0xbf802448
	andi	$4, $4, 0x00FF
	ori	    $4, $4, 0x8000
	LSSSW    0xbf802448, $4
	LRREG	$4, 0xbf80244c
	andi	$4, $4, 0x00FF
	ori	    $4, $4, 0x0000
	LSSSW    0xbf80244c, $4
	LWREG    0xbf802440 MIU_FREQ

	#XBYTE[0x1E25] = 0x80;
	LRREG	$4, 0xbf803c48
	andi	$4, $4, 0x00FF
	ori	    $4, $4, 0xC000      # MIU clock: set to 0xC000 for 216MHz (MCU access dram speed)
	LSSSW    0xbf803c48, $4

.if (MIU1_INIT)
	#XBYTE[0x121D] |= BIT6; // MIUx2
	LRREG	$4, 0xbf802438
	ori		$4, $4, 0x4000
	LSSSW    0xbf802438, $4
.endif

	#init MIU1 table
.if (MIU1_INIT)
	LWREG 0xbf800c00 0x0000
	LWREG 0xbf800c04 MIU1_LEN
    # Enable MIU1 ODT for MIUx2 packages
.if (ENABLE_ODT)
	LWREG 0xbf800c08 0x0E98
.else
	LWREG 0xbf800c08 0x0E90
.endif
	LWREG 0xbf800c0c 0x0060
	LWREG 0xbf800c10 0x6672
	LWREG 0xbf800c14 0x3264
	LWREG 0xbf800c18 0x3328
	LWREG 0xbf800c1c 0x95EA
	LWREG 0xbf800c20 0x0000
	LWREG 0xbf800c24 0x0000
	LWREG 0xbf800c28 0x1040
	LWREG 0xbf800c2c 0x2000
	LWREG 0xbf800c30 0x3000
	LWREG 0xbf800c34 0x5757
	LWREG 0xbf800c38 0x1800
	LWREG 0xbf800c3c 0x0C00
	LWREG 0xbf800c40 MIU_FREQ
	LWREG 0xbf800c44 0x0000
	LWREG 0xbf800c48 0x8000
	LWREG 0xbf800c4c 0x0000
	LWREG 0xbf800c50 0x0000
	LWREG 0xbf800c54 0x0000
	LWREG 0xbf800c58 0x0000
	LWREG 0xbf800c5c 0x0000
	LWREG 0xbf800c60 MIU1_PHASE
	LWREG 0xbf800c64 0x0055
	LWREG 0xbf800c68 0x0000
	LWREG 0xbf800c6c MIU1_DRAM_PHASE
	LWREG 0xbf800c70 0x0040
	LWREG 0xbf800c74 0x0000
	LWREG 0xbf800c78 0x0000
	LWREG 0xbf800c7c 0x4090
.endif

	#toggle MIU1 reg_4x_mode
.if (MIU1_INIT)
	LRREG	$4, 0xbf800c08
	andi	$4, $4, 0xF7FF
	LSSSW    0xbf800c08, $4
	ori		$4, $4, 0x0800
	LSSSW    0xbf800c08, $4

	#trigger MIU1 init
	LRREG	$4, 0xbf800c00
	andi	$4, $4, 0xFFFE
	LSSSW    0xbf800c00, $4
	ori		$4, $4, 0x0001
	LSSSW    0xbf800c00, $4
.endif

	#trigger MIU0 init
	LRREG	$4, 0xbf802400
	andi	$4, $4, 0xFFFE
	LSSSW    0xbf802400, $4
	ori		$4, $4, 0x0001
	LSSSW    0xbf802400, $4

	#restore mask & protection registers
	#MIU0
	LWREG 0xbf80248c 0x0000
	LWREG 0xbf8024cc 0x0000
	LWREG 0xbf80250c 0x0000
	LWREG 0xbf80254c 0x0000
	#MIU1
.if (MIU1_INIT)
	LWREG 0xbf800c8c 0x0000
	LWREG 0xbf800ccc 0x0000
	LWREG 0xbf800d0c 0x0000
	LWREG 0xbf800d4c 0x0000
.endif

	#while ( (XBYTE[0x1201] & 0x80) == 0 );
MIU_INIT:
	LRREG	$4, 0xbf802400
	andi	$4, $4, 0x8000
    bnez	$4, MIU_INIT_DONE
    nop
	DBGREG  0x002C
	j MIU_INIT
	nop
MIU_INIT_DONE:

	#EOF Initialize MIU-------------------------------

	#Turn on UART clock
	LWREG 0xbf803c5c 0x0800

	#switch UART0 to MIPS (PIU)
	LWREG 0xbf803d54,0x14

    #clk_mcu------------------------------
	#Power up MIPS clock to 216MHZ
	LWREG 0xbf803c6c 0x2020

	#turn on output pad
	LWREG 0xbf803c14 0x0000

	#enable MSTVTool DRAM access ability(copy from workable 8051 products)
	LWREG 0xbf80578c 0x3c12
	LWREG 0xbf805790 0x0058
	LWREG 0xbf805794 0x403c
	LWREG 0xbf805798 0x058f
	LWREG 0xbf805788 0x0004
	
	#set AEON speed
#	LRREG	$4, 0xbf803c88
#	andi	$4, $4, 0xE0FF
#	ori		$4, $4, 0x0800
#	LSSSW    0xbf803c88, $4
    #
#	LRREG	$4, 0xbf803c44
#	ori		$4, $4, 0x0001
#	LSSSW    0xbf803c44, $4

    jr      $31
    nop



#-------------------------------------------------------------------------------------------------
# BOOT_Flash_Clock
# @param  None
# @return None
# @note   Increase Flash Clock
#-------------------------------------------------------------------------------------------------
#BOOT_Flash_Clock:
#
#    li      $12, REG_ISP_CLK_SRC
#    lw      $13, 0($12)
#    nop
#    nop
#    nop
#    and     $13, $13, ~(1<<5)   # Xtal
#    sw      $13, 0($12)
#    nop
#    nop
#    nop
#    and     $13, $13, ~(0x1F)
#    or      $13, $13, 0x08      # 36 MHz
#    or      $13, $13, (1<<5)    # Clk_Sel
#    sw      $13, 0($12)
#    nop
#    nop
#    nop
#
#    jr      $31
#    nop



#-------------------------------------------------------------------------------------------------
# BOOT_Pad
# @param  None
# @return None
# @note   Pad with zeros (Reserve SIZE_BOOT_RESERVED Bytes for boot)
#-------------------------------------------------------------------------------------------------
    .org    (SIZE_BOOTROM_RESERVED-4)
    .word   0


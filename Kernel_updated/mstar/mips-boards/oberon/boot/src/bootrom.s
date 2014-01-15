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

    bal BOOTROM_MIU_BIST
    nop

	PUTC '\r'
    PUTC '\n'

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
    DELAYUS 200000
    PUTC '\r'
    PUTC '\n'
    PUTC 'M'
    PUTC 'I'
    PUTC 'U'
    PUTC '0'
    PUTC '_'
    # MIU0 BIST: Build-in self test
    #WREG     0xbf8025d0, 0x5aa5   #test data = 0xffff
    WREG     0xbf8025c4, 0x0000   #test base = 0x0000
    WREG     0xbf8025c8, 0xffff   #test length low
    WREG     0xbf8025cc, 0x00ff   #test length high
    WREG     0xbf8025E0, 0x0000   #switch BIST to MIU0
    WREG     0xbf8025c0, 0x0001   #test bit0=en, bit1=constant
MIU0_BIST_LOOP:
    DELAYUS  2000
    lui      $8, 0xbf80
    li       $9, 0x8001
    lw       $10, 0x25c0($8)     #RREG 0x0xbf8025c0
    nop
    BEQ      $10, $9, MIU0_BIST_PASS  #if 0xbf8025c0 == 0x8003 is pass
    nop
    B        MIU0_BIST_LOOP
    nop
MIU0_BIST_PASS:
    WREG     0xbf8025c0, 0x0000
    PUTC 'O'
    PUTC 'K'

    # MIU1 BIST: Build-in self test
    .if (RAM_SIZE_128)
    .else
    DELAYUS  200000
    PUTC '\r'
    PUTC '\n'
    PUTC 'M'
    PUTC 'I'
    PUTC 'U'
    PUTC '1'
    PUTC '_'
    WREG     0xbf8025E0, 0x0800   #switch BIST to MIU1
    WREG     0xbf8025c0, 0x0001   #test bit0=en, bit1=constant
MIU1_BIST_LOOP:
    DELAYUS  2000
    lui      $8, 0xbf80
    li       $9, 0x8001
    lw       $10, 0x25c0($8)     #RREG 0x0xbf8025c0
    nop
    BEQ      $10, $9, MIU1_BIST_PASS  #if 0xbf8025c0 == 0x8003 is pass
    nop
    B        MIU1_BIST_LOOP
    nop
MIU1_BIST_PASS:
    WREG     0xbf8025c0, 0x0000
    PUTC 'O'
    PUTC 'K'
    .endif

    DELAYUS  200000
    jr      $31
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

    jr      $31
    nop


#-------------------------------------------------------------------------------------------------
# BOOT_PowerOnSeq
# @param  None
# @return None
# @note   Power On Sequence for Chip
#-------------------------------------------------------------------------------------------------
BOOT_PowerOnSeq:

    .if 1
    #Disable WDT
    WREG    0xbf808400, 0xaa55
    WREG    0xbf808404, 0x55aa
    .endif

    DBGREG  0x0011

    #PLL 12MHz XTAL
    WREG    0xbf80a410, 0x0902
    WREG    0xbf80a414, 0x2006
    WREG    0xbf80a424, 0x4710

    #stablize PLL
    WREG    0xbF80A404, 0x006B  # Set MPLL Power Down 0
    WREG    0xbF804A20, 0x0506  # MPLL enable
    WREG    0xbF804A2C, 0x0000  # MPLL div clocks enable
    WREG    0xbF804B80, 0x0000  # DVI ibias output enable
    DELAYUS 50000               # 5ms to stable

    #power on atop
    WREG    0xbF806400, 0x0010  # DAC's EN
    WREG    0xbF80200C, 0x0000  # Set bit 5 to 0
    WREG    0xbF80210C, 0x0000  # Set bit 5 to 0
    WREG    0xbF804E10, 0x0F21


    DBGREG  0x0012

    #clk_mcu
    WREG    0xbf803c44, 0x0000  # XTAL
    #WREG    0xbf803c88, 0x1400  # 0x0000:216MHz, 0x1400:123MHz # 123MHz
    #WREG    0xbf803c44, 0x0001  # PLL

    #wait clk_miu stable
    DELAYUS 1000000

    DBGREG  0x0013

    #Mask Request of Group 0, 1, 2
    WREG    0xbf80248c, 0x0000
    WREG    0xbf8024cc, 0x0000
    WREG    0xbf80250c, 0x0000

    #Initialize MIU
    # WREG    0xbF802440, 0x1309    # for MIU 720
    # WREG    0xbF802440, 0x14A9    # for MIU 667
    WREG    0xbF802440, 0x2249   # for MIU 800
    WREG    0xbF802448, 0xC000    # reg_DDRPLL_LOOP_DIV_FIRST     : [15:14]       divide by 2
	.if (RAM_SIZE_128)
	WREG    0xbF80A040, 0x2249   # for MIU 800
    WREG    0xbF80A048, 0xC000    # reg_DDRPLL_LOOP_DIV_FIRST     : [15:14]       divide by 2
	.endif
    WREG    0xbF80243C, 0x0000    # dummy register setting
    WREG    0xbF80243C, 0x0000    # dummy register setting
    WREG    0xbF80243C, 0x0000    # dummy register setting
    #WREG    0xbF80243C, 0x0C01    # miu software reset
    #WREG    0xbF80243C, 0x0C00    #
    #WREG    0xbF80A03C, 0x0C01    # miu software reset
    #WREG    0xbF80A03C, 0x0C00    #
    WREG    0xbF80246C, 0x4640    # reg_ddr_clk_switch[1:0]
    WREG    0xbF80A06C, 0x4640    # reg1_ddr_clk_switch[1:0]
	.if (RAM_SIZE_128)
    WREG    0xbF802470, 0x0104    #
    WREG    0xbF80246C, 0x4640    # reg_CLKPH[2:0]          : [2:0]
    WREG    0xbF802428, 0x1040    # ODT = 75
    # WREG    0xbF802404, 0x0545    # support 64Mbyts only
    WREG    0xbF802404, 0x0D40    # support 128Mbytes
    WREG    0xbF802408, 0x0EA8    #  DDR2 800MHz. Could Oberon run that fast?
	.else
    WREG    0xbF802470, 0x0101    #
    WREG    0xbF80246C, 0x4640    # reg_CLKPH[2:0]          : [2:0]
    WREG    0xbF802428, 0x1040    # ODT = 75
    # WREG    0xbF802404, 0x0545    # support 64Mbyts only
    WREG    0xbF802404, 0x0D40    # support 128Mbytes
    WREG    0xbF802408, 0x0E98    #  DDR2 800MHz. Could Oberon run that fast?
	.endif
                                  #  0  	0	reg_rd_in_phase
                                  #  1  	1	reg_rd_mck_sel
                                  #  2  	2	reg_force_ddr_rd_act
                                  #  3  	3	reg_odt
                                  #  7  	4	reg_rd_timing
                                  #  10	8	reg_cas_letatency
                                  #  11	11	reg_4x_mode
    WREG    0xbF80241C, 0x95EA    # tRFC                          : [5:0]         42(105/2.5)
    WREG    0xbF802410, 0x6672    # reg_tRAS                      : [3:0]         16+2=18(45/2.5)
    WREG    0xbF802414, 0x3264    # reg_tRRD                      : [3:0]         4(10/2.5)
    WREG    0xbF802418, 0x4429    #
    WREG    0xbF80240C, 0xc060    #
    WREG    0xbF802460, 0x0033    #
    WREG    0xbF802464, 0x0055    #
    WREG    0xbF802474, 0x00c3    #
    WREG    0xbF80247c, 0x4050    #
    WREG    0xbF802480, 0x00C1    # reg_rq0_round_robin           : [0]
    WREG    0xbF8024C0, 0x00C1    # reg_rq1_round_robin           : [0]
    WREG    0xbF802500, 0x00C1    # reg_rq2_round_robin           : [0]
    WREG    0xbF802540, 0x00C1    # reg_rq3_round_robin           : [0]
    WREG    0xbF80A028, 0x1040    # ODT = 75
	.if (RAM_SIZE_128)
    WREG    0xbF80A070, 0x0401    #
    WREG    0xbF80A06C, 0x4640    #
    # WREG    0xbF80A004, 0x0545    # support 64Mbytes only
    WREG    0xbF80A004, 0x0D40    # support 128Mbytes only
    WREG    0xbF80A008, 0x0EA8    #
    WREG    0xbF80A01C, 0x95EA    # tRFC                          : [5:0]         42(105/2.5)
    WREG    0xbF80A010, 0x6672    # reg_tRAS                      : [3:0]         16+2=18(45/2.5)
    WREG    0xbF80A014, 0x3264    # reg_tRRD                      : [3:0]         4(10/2.5)
    WREG    0xbF80A018, 0x4429    #
    WREG    0xbF80A00C, 0xc060    #
    WREG    0xbF80A038, 0x3814    #
	.else
    WREG    0xbF80A070, 0x0101    #
    WREG    0xbF80A06C, 0x4640    #
    # WREG    0xbF80A004, 0x0545    # support 64Mbytes only
    WREG    0xbF80A004, 0x0D40    # support 128Mbytes only
    WREG    0xbF80A008, 0x0E98    #
    WREG    0xbF80A01C, 0x95EA    # tRFC                          : [5:0]         42(105/2.5)
    WREG    0xbF80A010, 0x6672    # reg_tRAS                      : [3:0]         16+2=18(45/2.5)
    WREG    0xbF80A014, 0x3264    # reg_tRRD                      : [3:0]         4(10/2.5)
    WREG    0xbF80A018, 0x4429    #
    WREG    0xbF80A00C, 0xc060    #
    WREG    0xbF80A038, 0x5800    #
	.endif
    # WREG    0xbF80A060, 0x0044    # for MIU 720
    WREG    0xbF80A060, 0x0022    # for MIU 667
    WREG    0xbF80A064, 0x0055    #
    WREG    0xbF80A074, 0x00c3    #
    WREG    0xbF80A07c, 0x4050    #
    WREG    0xbF80A080, 0x00C1    # reg_rq0_round_robin           : [0]
    WREG    0xbF80A080, 0x00C1    # reg_rq1_round_robin           : [0]
    WREG    0xbF80A100, 0x00C1    # reg_rq2_round_robin           : [0]
    WREG    0xbF80A140, 0x00C1    # reg_rq3_round_robin           : [0]
    WREG    0xbF8025B8, 0x5151    # select the frame of rd dqs
    WREG    0xbF80A1B8, 0x5151    # select the frame of rd dqs
    #WREG    0xbF802400, 0x8005    # initial external sdram
    #WREG    0xbF80A000, 0x8005    # initial external sdram
    #DELAYUS 1000000               #
    WREG    0xbf8025E0, 0x0000    #
    WREG    0xbf8025E4, 0x0000    #
    WREG    0xbf8025E8, 0x0000    #
	.if (RAM_SIZE_128)
    WREG    0xbF802438, 0x3812    # reg_dma0_miu_sel : [11], reg_dma0_miu_sel : [12]
	.else
    WREG    0xbF802438, 0x5800    # reg_dma0_miu_sel : [11], reg_dma0_miu_sel : [12]
	.endif

    # MIU 0 group mask
    WREG    0xbf80248C, 0x0000
    WREG    0xbf8024CC, 0x0000
    WREG    0xbf80250C, 0x0000
    # MIU 1 group mask
    WREG    0xbf80A08C, 0x0000
    WREG    0xbf80A0CC, 0x0000
    WREG    0xbf80A10C, 0x0000
    # MIU 0 soft reset
    WREG    0xbF80243C, 0x0C01 # miu software reset
	WREG    0xbF80243C, 0x0C01 # miu software reset
	WREG    0xbF80243C, 0x0C01 # miu software reset
    WREG    0xbF80243C, 0x0C08
    # MIU 1 soft reset
    WREG    0xbF80A03C, 0x0C01 # miu software reset
	WREG    0xbF80A03C, 0x0C01 # miu software reset
	WREG    0xbF80A03C, 0x0C01 # miu software reset
    WREG    0xbF80A03C, 0x0C08

    WREG    0xbF802404 0x0D45 # MIU0 support 128MBytes CKE enable after soft reset yl modify
    WREG    0xbF80A004 0x0D45 # MIU1 support 128MBytes CKE enanle after soft reset yl modify
	DELAYUS 1000000

    WREG    0xbF802400, 0x0000
    WREG    0xbF802400, 0x0001
    WREG    0xbF80A000, 0x0000
    WREG    0xbF80A000, 0x0001
	DELAYUS 1000000

    # MIU 0 group mask
    WREG    0xbf80248C, 0x0000
    WREG    0xbf8024CC, 0x0000
    WREG    0xbf80250C, 0x0000
    # MIU 1 group mask
    WREG    0xbf80A08C, 0x0000
    WREG    0xbf80A0CC, 0x0000
    WREG    0xbf80A10C, 0x0000

    # MIU0 high priority mask
    WREG    0xbf8024d0, 0xffcf

    #Turn on all clocks
    WREG    0xbf803c48, 0x0000    # miu clock --> mempll
#    WREG    0xbf803c4c, 0x0000
#    WREG    0xbf803c50, 0x0800
#    WREG    0xbf803c54, 0x0000
#    WREG    0xbf803c58, 0x0000
    WREG    0xbf803c5c, 0x0215    # UART clock
#    WREG    0xbf803c60, 0x0000
#    WREG    0xbf803c64, 0x0000
#    WREG    0xbf803c68, 0x0000
#    WREG    0xbf803c6c, 0x0200
#    WREG    0xbf803c70, 0x000c
#    WREG    0xbf803c74, 0x0000
#    WREG    0xbf803c7c, 0x2020

    # RIU mode  #Oberon Only
    WREG    0xbf8010d8, 0x0081

    DBGREG  0x0016

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


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
    ori    $7, $0, MIU0_DO_BIST_PATTERN

    # set test length
    RBASE   0xbf2025c8
    WREG     0xbf2025c8 MIU0_DO_BIST_LEN_L
    LRREG    $4, 0xbf2025cc
    andi    $4, $4, 0xFF00
    ori        $4, $4, MIU0_DO_BIST_LEN_H
    LSSSW    0xbf2025cc, $4

    bal        BOOTROM_MIU_BIST
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

    LRREG    $4, 0xbf2025e0
    ori        $4, $4, 0x8000
    LSSSW    0xbf2025e0, $4

    LRREG    $4, 0xbf2025c0
    andi    $4, $4, 0xFF00
    LSSSW    0xbf2025c0, $4

    ori    $7, $0, MIU1_DO_BIST_PATTERN

    # set test length
    WREG     0xbf2025c8 MIU1_DO_BIST_LEN_L
    LRREG    $4, 0xbf2025cc
    andi    $4, $4, 0xFF00
    ori        $4, $4, MIU1_DO_BIST_LEN_H
    LSSSW    0xbf2025cc, $4

    bal        BOOTROM_MIU_BIST
    nop

    LRREG    $4, 0xbf2025c0
    andi    $4, $4, 0xFF00
    LSSSW    0xbf2025c0, $4

    LRREG    $4, 0xbf2025e0
    andi    $4, $4, 0x7FFF
    LSSSW    0xbf2025e0, $4
.endif
# EOF MIU BIST ---------------------------------------------------
    PUTC 'B'

    # Increase Flash clock
#    bal     BOOT_Flash_Clock
    nop

    DBGREG    0x001B
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
    RBASE   0xbf2025c4
    WREG     0xbf2025c4 0x0000

    #   test pattern
    sw      $7, (0x25d0)($8)
    #   enable test, order (high byte then low byte) is important
    LRREG    $4, 0xbf2025c0
    andi    $4, $4, 0x00FF
    LSSSW    0xbf2025c0, $4
    LRREG    $4, 0xbf2025c0
    andi    $4, $4, 0xFF00
    LSSSW    0xbf2025c0, $4        # clear enable bit
    LRREG    $4, 0xbf2025c0
    andi    $4, $4, 0xFF00
    ori        $4, $4, 0x0003
    LSSSW    0xbf2025c0, $4        # pattern from data
    #   check result
MIU_BIST_CHK:
    LRREG    $4, 0xbf2025c0
    andi    $4, $4, 0x8000
    bnez    $4, MIU_BIST_CHK_RES
    nop
    j MIU_BIST_CHK
    nop
MIU_BIST_CHK_RES:
    LRREG    $4, 0xbf2025c0
    andi    $4, $4, 0x4000
    bnez    $4, MIU_BIST_FAIL
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

    RBASE   REG_SFSH_DMA_CMD
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

    RBASE   REG_SFSH_DMA_CMD
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
.if (SUPPORT_EUCLID_UART)
    #Setup UART clk to 123MHZ(for higher bandrate)
    RBASE 0xbf203C7C
    WREG 0xbf203C7C, 0x00FF
    #WREG 0xbf203C7C, 0x2C00 #123000000
    WREG 0xbf203C7C, 0x2000 #172800000
    WREG 0xbf203C8C, 0x0100

    WREG 0xbf203d54, 0x1234;  # select uart0 source to tsp
    RBASE 0xbf001c24
    WREG 0xbf001c24, 0x00;  # switch uart from hk51 to mips
    WREG 0xbf001c24, 0x800;  # switch uart from hk51 to mips

    #Disable all UART interrupts
    RBASE   UART_IER
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
    #Setup UART clk to 123MHZ(for higher bandrate)
    RBASE 0xbf203c8c
    WREG 0xbf203c8c 0x0008
    WREG 0xbf203c78 0x01C0

    #Disable all UART interrupts
    LRREG    $4, UART_IER
    andi    $4, $4, 0x00FF
    ori        $4, $4, 0x0000
    LSSSW    UART_IER, $4
    #WREG    UART_IER, 0x00

    #Reset receiver and transmitr
    LRREG    $4, UART_FCR
    andi    $4, $4, 0xFF00
    ori        $4, $4, 0x0007
    LSSSW    UART_FCR, $4
    #WREG    UART_FCR, 0x07

    #Set 8 bit char, 1 stop bit, no parity
    LRREG    $4, UART_LCR
    andi    $4, $4, 0x00FF
    ori        $4, $4, 0x0300
    LSSSW    UART_LCR, $4
    #WREG    UART_LCR, 0x03

    #Set baud rate

    LRREG    $4, UART_LCR
    andi    $4, $4, 0x00FF
    ori        $4, $4, 0x8300
    LSSSW    UART_LCR, $4
    #WREG    UART_LCR, 0x83

    LRREG    $4, UART_DLM
    andi    $4, $4, 0x0000
    ori        $4, $4, UART_DIVISOR
    LSSSW    UART_DLM, $4
    #WREG    UART_DLM, ((UART_DIVISOR >> 8) & 0x00ff)

    LRREG    $4, UART_LCR
    andi    $4, $4, 0x00FF
    ori        $4, $4, 0x0300
    LSSSW    UART_LCR, $4
    #WREG    UART_LCR, 0x03

.endif

    jr      $31
    nop

#-------------------------------------------------------------------------------------------------
# BOOT_IP_RESET
# @param  None
# @return None
# @note   IPs Reset
#-------------------------------------------------------------------------------------------------
.macro BOOT_IP_RESET

    #WREG 0xbf204A1C, 0x00FF  # # reset atop,HDMI,HDCP,DVI,PLL_DIG_B,ADC_VD,PLL_DIG_A,ADC_DIG_A
    RBASE 0xbf220A00
    WREG 0xbf220A00, 0x003F # reset ATOP,CRC,VE,HDCP,HMTX,AUD
    RBASE 0xbf2072C0
    WREG 0xbf2072C0, 0x0000 # MPIF reset
    WREG 0xbf204200, 0x0000  # emac reset  # original # WREG, 0xbf204202 0x0000
    WREG 0xbf202BE8, 0x0000 # TSP reset
    WREG 0xbf202BFC, 0x8000 # original # WREG, 0xbf202bfe 0x0080
    WREG 0xbf202BFC, 0x0000 # original # WREG, 0xbf202bfe 0x0080
    WREG 0xbf2006C0, 0x0000  # NAND reset               # original # WREG, 0xbf2006c2 0x0000
    WREG 0xbf205000, 0x0000 # GE reset
    WREG 0xbf205008, 0x0000 # GE_CMQ reset

    WREG 0xbf203FFC, 0x0000 # GOP4G
    WREG 0xbf203E00, 0x0001 # active SW reset
    WREG 0xbf203FFC, 0x0003 # GOP2G
    WREG 0xbf203E00, 0x0001 # active SW reset
    WREG 0xbf203FFC, 0x0007 # GOP1G
    WREG 0xbf203E00, 0x0001 # active SW reset
    WREG 0xbf203FFC, 0x0206 # GOPD       # enable force write
    WREG 0xbf203E00, 0x0001 # active SW reset
    WREG 0xbf203FFC, 0x000A # GOP MIXER SW
    WREG 0xbf203E00, 0x0000 # active SW reset

    WREG 0xbf206C24, 0x4000 # original # WREG, 0xbf206c26 0x0040
    WREG 0xbf206C20, 0x0012
    WREG 0xbf2039E0, 0x0003 # VD_MHEG5 reset
    WREG 0xbf201404, 0x0001 # RVD reset
    WREG 0xbf203604, 0x0001 # HVD reset

    WREG 0xbf207600, 0x0030 # VE engine
    WREG 0xbf200E00, 0x0007 # USBC0
    WREG 0xbf200F00, 0x0007 # USBC1
    WREG 0xbf205800, 0x0001 # VIVALDI

    WREG 0xbf205E00, 0x0000 # select SC bank
    WREG 0xbf205E08, 0x0001 # all engine
.endm

#-------------------------------------------------------------------------------------------------
# BOOT_MIU_Init
# @param  None
# @return None
# @note   MIU Init
#-------------------------------------------------------------------------------------------------
.macro BOOT_MIU_Init

    RBASE 0xbf202444
    WREG 0xbf202444, 0x0000
    WREG 0xbf20244c, 0x0000

    WREG 0xbf20244C, 0x0500 # [15:8] LOOP_DIV_SECOND
    WREG 0xbf202448, 0x0103
    WREG 0xbf202440, 0x15D9 # [7:0]  DDFSTEP [8] SSC_EN # [13:4] DDFSET, 396MHz, SSC 20k +-1%

    WREG 0xbf202400, 0x0000
    WREG 0xbf200C00, 0x0000

    WREG 0xbf20248C, 0x7FFE
    WREG 0xbf2024CC, 0xFFFF
    WREG 0xbf20250C, 0xFFFF

    WREG 0xbf200C8C, 0x7FFE
    WREG 0xbf200CCC, 0xFFFF
    WREG 0xbf200D0C, 0xFFFF

    # IP reset engine
    BOOT_IP_RESET

	RBASE 0xbf202488
#start of disable for bring up
#   WREG 0xbf202488, 0x0400 # Timeout for 3 groups of MIU_0
#   WREG 0xbf2024C8, 0x0400
#   WREG 0xbf202508, 0x0400
#end of disable for bring up

    WREG 0xbf202404, 0x0C44
    WREG 0xbf202408, 0x0EA8

    WREG 0xbf20240C, 0xC060 # DRAM_SIZE_SETTING=0x00(no limit) | RV_TFREPERIOD=0x60)
    WREG 0xbf202410, 0x6672

    WREG 0xbf202414, 0x3264
    WREG 0xbf202418, 0x4428 #0x33
    WREG 0xbf20241C, 0x95EA

    WREG 0xbf202420, 0x0000
    WREG 0xbf202424, 0x0000
    WREG 0xbf202428, 0x1040
    WREG 0xbf20242C, 0x2000
    WREG 0xbf202430, 0x3000
    WREG 0xbf202434, 0x5151
    WREG 0xbf202438, 0x5801
    WREG 0xbf20243C, 0x0C00

    WREG 0xbf202450, 0x0000
    WREG 0xbf202454, 0x0000
    WREG 0xbf202458, 0x0000
    WREG 0xbf20245C, 0x0000


    WREG 0xbf202460, 0x0033
    WREG 0xbf202464, 0x0055

    WREG 0xbf202468, 0x0000
    WREG 0xbf20246C, 0xFE41  #0x44->01 phase
    WREG 0xbf202470, 0x0010 #0x70->0x50 for MIU1
    WREG 0xbf202474, 0x00C3
    WREG 0xbf202478, 0x0000
    WREG 0xbf20247C, 0x4090

    WREG 0xbf202480, 0x0010

    WREG 0xbf202580, 0x0000 # disable MIU_0 write protect

    # MIU_1 initial
    WREG 0xbf200C88, 0x0400
    WREG 0xbf200CC8, 0x0400
    WREG 0xbf200D08, 0x0400
    WREG 0xbf200C04, 0x0C45
    WREG 0xbf200C08, 0x0EA8
    WREG 0xbf200C0C, 0xC060 #RV_TFREPERIOD  # [15] 8 level FIFO
    WREG 0xbf200C10, 0x6672
    WREG 0xbf200C14, 0x3264
    WREG 0xbf200C18, 0x4428 #0x33
    WREG 0xbf200C1C, 0x95EA
    WREG 0xbf200C20, 0x0000
    WREG 0xbf200C24, 0x0000
    WREG 0xbf200C28, 0x1040
    WREG 0xbf200C2C, 0x2000
    WREG 0xbf200C30, 0x3000
    WREG 0xbf200C34, 0x5151
    WREG 0xbf200C3C, 0x0C00
    WREG 0xbf200C54, 0x0000
    WREG 0xbf200C58, 0x0000
    WREG 0xbf200C5C, 0x0000
    WREG 0xbf200C60, 0x0033
    WREG 0xbf200C64, 0x0055
    WREG 0xbf200C68, 0x0000
    WREG 0xbf200C6C, 0xFE41
    WREG 0xbf200C70, 0x0010
    WREG 0xbf200C78, 0x0000
    WREG 0xbf200C7C, 0x4090
    WREG 0xbf200D80, 0x2000 # disable MIU_1 write protect


    WREG 0xbf2025E0, 0x0000
    WREG 0xbf2025E4, 0x0000
    WREG 0xbf2025E8, 0x0000

    WREG 0xbf200C3C, 0x0C01 #reset MIU_1
    WREG 0xbf20243C, 0x0C01 #reset miu_0

    WREG 0xbf200C3C, 0x0C00 #0x01->0x00
    WREG 0xbf20243C, 0x0C00 #0x01->0x00

    WREG 0xbf202404, 0x0C45 #set reg1202 bit0  #enable MIU_0
    WREG 0xbf200C04, 0x0C45 #set reg0602 bit0  #enable_miu_1

    WREG 0xbf202400, 0x0005 #fire miu_0 setting
    WREG 0xbf200C00, 0x0005 #fire miu_1 setting
    DELAYMS 50

    #WREG 0xbf20248C, 0x7F9E
    WREG 0xbf20248C, 0x7B98 #+TSP+ORZ
    #WREG 0xbf2024CC, 0x0000
.if EN_EMAC
    WREG 0xbf20250C, 0x7FED#+ORZ
.else
    WREG 0xbf20250C, 0xFFFD#+ORZ
.endif
.endm
#-------------------------------------------------------------------------------------------------
# BOOT_PowerOnSeq
# @param  None
# @return None
# @note   Power On Sequence for Chip
#-------------------------------------------------------------------------------------------------
BOOT_PowerOnSeq:

    RBASE   0xbf001C40
    WREG    0xbf001C40, 0x0010
    WREG    0xbf001C3C, 0x00EF
    WREG    0xbf001C48, 0xBABE

    #Disable WDT, or MIPS will continously reset
    WREG 0xbf0078C0, 0xAA55 # watch dog
    WREG 0xbf0078C4, 0x55AA # watch dog

    DELAYMS 50

    # sysInitMPLL --> start
    # DAC reference enable (0x1a40[4])
    RBASE 0xbf206900
    WREG 0xbf206900, 0x0010    # DAC reference enable (0x1A40.[4])
    WREG 0xbf204A10, 0x0000
    WREG 0xbf204A14, 0x0000
    WREG 0xbf204A18, 0x0000
    WREG 0xbf204B80, 0x0000
    WREG 0xbf204BA4, 0x0000
    WREG 0xbf204B00, 0x0000

    # 4.MPLL INITIALIZATION
    # MPLL mULTIPLIER = XTAL*0x24h/2=216MHz
    WREG 0xbf201CC4, 0x0021
    WREG 0xbf201CCC, 0x0902
    WREG 0xbf201CD0, 0x0002

    # power ON MPLL PATH IN Vif_adc_top
    # power ON CONTROL OF INPut MPLL
    WREG 0xbf201CD4, 0x0040
    # sysInitMPLL --> end

    # 5.UPLL INITIALIZATION(Turn on 180MHZ and 240MHZ)
    # USBC0 <--> UTMI0
    WREG 0xbf207558, 0x0004 #switch to XIN_LV
    WREG 0xbf207540, 0x0001 #switch to XIN_LV
    WREG 0xbf207504, 0x1084 #switch to XIN_LV

    WREG 0xbf200E00, 0x020A # Disable MAC initial suspend, Reset UHC
    DELAYMS 10
    WREG 0xbf200E00, 0x0268 # Release UHC reset, enable UHC and OTG XIU function
    DELAYMS 30
    WREG 0xbf200E04, 0x0001 # UHC select enable

    WREG 0xbf207510, 0x0000 #Disable force_pll_on,  Enable band-gap current
    WREG 0xbf207500, 0x6BC3 #Turn on reference voltage and regulator, reg_pdn: bit<15>, bit <2> ref_pdn
    DELAYMS 10
    WREG 0xbf207500, 0x69C3 # Turn on UPLL, reg_pdn: bit<9>
    DELAYMS 20
    WREG 0xbf207500, 0x0001 # Turn all (including hs_current) use override mode
    WREG 0xbf207504, 0x1084 #

    # USBC1 <--> UTMI1
    # WREG 0xbf200F00, 0x000A # Disable MAC initial suspend, Reset UHC
    # WREG 0xbf200F00, 0x0068 # Release UHC reset, enable UHC and OTG XIU function

    # WREG 0xbf207510, 0x0000 # Enable band-gap current
    # WREG 0xbf207500, 0x6BC3 # reg_pdn: bit<15>, bit <2> ref_pdn
    # DELAYMS 10
    # WREG 0xbf207500, 0x6900 # Turn on UPLL, reg_pdn: bit<9>
    # DELAYMS 20
    # WREG 0xbf207500, 0x0001 # Turn all (including hs_current) use override mode
    # WREG 0xbf207504, 0x0000
    # WREG 0xbf207558, 0x0004 # Enable DVIBIAS_EN
    # WREG 0xbf207540, 0x0002
    # MDrv_UPLL_init --> USB PLL update --> end

    DELAYMS 50

    # sysInitTbl  --> start
    # 7.Turn ON IP CLOCK CHAIn
    WREG 0xbf203C44, 0x0000

    #WREG 0xbf001c80, 0x0004  # MCU clock -> 144MHz
    #WREG 0xbf001c80, 0x0084  # MCU clock -> 144MHz

    #for Aeon/8051/RIU clock
    RBASE 0xbf001c80
    WREG 0xbf001c80, 0x0000  # MCU clock -> 172MHz
    WREG 0xbf001c80, 0x0080  # MCU clock -> 172MHz

    RBASE 0xbf203C48
    WREG 0xbf203C48, 0x0000
#   WREG 0xbf203190, 0x0020  # flush threshold
#   WREG 0xbf20318c, 0x0020  # flush timeout

    # mips=123MHZ
    #WREG 0xbf203C48, 0x0014
    #WREG 0xbf203C48, 0x2034

    # mips=216MHZ
    #WREG 0xbf203C48, 0x0000
    #WREG 0xbf203C48, 0x2020

    # mips=240MHz
    WREG 0xbf203c48, 0x0018
    WREG 0xbf203c48, 0x2038

    # miu=170MHZ->MEMPLL OUT/2
    WREG 0xbf203C4C, 0x0008 # 0x04->0x08

    WREG 0xbf203C50, 0x0040
    #WREG 0xbf203C52, 0x0008 #jc_db: will stop at this line
    WREG 0xbf203C50, 0x0840
    WREG 0xbf203C54, 0x0400
    WREG 0xbf203C58, 0x0000
    WREG 0xbf203C58, 0x8800
    WREG 0xbf203C6C, 0x0000
    WREG 0xbf203C5C, 0x0C00
    WREG 0xbf203C60, 0x4000      # gopd?
    WREG 0xbf203C64, 0x0000
    WREG 0xbf203C68, 0x0400

    # fclk=170MHZ
    WREG 0xbf203C6C, 0x0000
    WREG 0xbf203C70, 0x0C80 #0x00->0x80
    WREG 0xbf203C74, 0xF81C #0x28->0x1c #<--------------------
    WREG 0xbf203C78, 0x0000
    WREG 0xbf203C7C, 0x0024
    WREG 0xbf203C80, 0x2020
    WREG 0xbf203C84, 0x0000
    WREG 0xbf203C88, 0x1000
    WREG 0xbf203C8C, 0x0000
    WREG 0xbf203CB0, 0x0000
    WREG 0xbf203D28, 0x0000
.if EN_EMAC
    WREG 0xbf203D3C, 0x0440
.else
    WREG 0xbf203D3C, 0x0000
.endif
    # sysInitTbl  --> end

    DELAYMS 50

    # WREG 0xbf201FCC, 0x0006  # t3 fpga modify ??

    # DdrInit --> start
    BOOT_MIU_Init

    #Turn on UART clock
    RBASE 0xbf203c5c
    WREG 0xbf203c5c 0x0800

    #turn on output pad
    WREG 0xbf203c14 0x0000

.if EN_EMAC
    #disable all pad in
    WREG 0xbf203d40 0x0000

    #switch i2c_in bus to emac
    WREG 0xbf203dbc 0x0100
.endif

    #enable MSTVTool DRAM access ability(copy from workable 8051 products)
    RBASE 0xbf00578c
    WREG 0xbf00578c 0x3c12
    WREG 0xbf005790 0x0058
    WREG 0xbf005794 0x403c
    WREG 0xbf005798 0x058f
    WREG 0xbf005788 0x0004

    #set AEON speed
#    RREG    $4, 0xbf203c88
#    andi    $4, $4, 0xE0FF
#    ori        $4, $4, 0x0800
#    LSSSW    0xbf203c88, $4
    #
#    LRREG    $4, 0xbf203c44
#    ori        $4, $4, 0x0001
#    LSSSW    0xbf203c44, $4

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


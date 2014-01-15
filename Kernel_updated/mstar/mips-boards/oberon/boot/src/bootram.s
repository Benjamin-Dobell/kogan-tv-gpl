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
# BOOTRAM_Entry:
# @param  None
# @return None
# @note   Entry point of BootRam code
#-------------------------------------------------------------------------------------------------
    .globl  BOOTRAM_Entry
BOOTRAM_Entry:

    DBGREG  0x0020
    PUTC '_'

    #clk_mcu
    WREG    0xbf803c44, 0x0000  # XTAL
    WREG    0xbf803c88, 0x0000  # 0x0000:216MHz, 0x1400:123MHz #216 MHz
    WREG    0xbf803c44, 0x0001  # PLL

.if (SUPPORT_UNZIP)
    # Initialize system stack pointer for unzip subroutine
    li      $sp, ADDR_UNZIP_STACK_POINTER

    # Copy Unzip subroutine from ROM to RAM
    bal     BOOTRAM_CopyUnzipFun
    nop

    PUTC 'o'
    
    # Unzip binary
    li      $4, ADDR_ZIPBIN_RAM_START_CACHE #a0: src
    li      $5, ADDR_SYSTEM_RAM_START       #a1: dst
    li      $6, ADDR_UNZIP_BUFFER           #a2: buffer
    li      $7, ADDR_UNZIP_RAM_START_CACHE  #unzip subroutine
    
    jal     $7
    nop
    
    blez    $2, BOOTRAM_ExceptionHandler
    nop
.else
    li      $2, 0 #ToDo: DMA cnt later
.endif

    DBGREG  0x0021
    PUTC 'k'

.if (BOOT_DEBUG_VERSION)
    # Record unzip or DMA count
    li      $9, 0xA0000000
    sw      $2, 0($9)

    # Record boot time
    mfc0    $8, $9
    li      $9, (CPU_CLOCK_FREQ/2/1000)
    divu    $10, $8, $9 #ms
    li      $9, 0xA0000004
    sw      $10, 0($9)
.endif

    DBGREG  0x0022

    # Jump to OS + App
    li      $8, ADDR_SYSTEM_RAM_START_CACHE
    j       $8
    nop



#-------------------------------------------------------------------------------------------------
# BOOTRAM_CopyUnzipFun
# @param  None
# @return None
# @note   Copy Unzip subroutine from ROM to RAM
#-------------------------------------------------------------------------------------------------
BOOTRAM_CopyUnzipFun:

    li      $8, ADDR_UNZIP_ROM_START
    li      $9, ADDR_UNZIP_RAM_START
    li      $10, ADDR_UNZIP_RAM_END
    li      $11, ADDR_UNZIP_ROM_END

1:
    ld      $12, 0($8)      #ld=lw lw to save time
    ld      $14, 8($8)
    sd      $12, 0($9)
    sd      $14, 8($9)
    addu    $8, $8, 16
    addu    $9, $9, 16
    bltu    $9, $10, 1b
    nop

    jr      $31
    nop



#-------------------------------------------------------------------------------------------------
# BOOTRAM_ExceptionHandler
# @param  None
# @return None
# @note   Exception handler for general exception or decompression failure at bootram code
#-------------------------------------------------------------------------------------------------
BOOTRAM_ExceptionHandler:

    la      $31,    BOOT_ExceptionHandler
    j       $31
    nop



#-------------------------------------------------------------------------------------------------
# BOOTRAM_Pad
# @param  None
# @return None
# @note   Pad with zeros (Reserve SIZE_BOOTRAM_RESERVED Bytes for bootram)
#-------------------------------------------------------------------------------------------------
    .org    (SIZE_BOOTRAM_RESERVED-4)
    .word   0

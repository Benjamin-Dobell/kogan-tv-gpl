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


#-------------------------------------------------------------------------------------------------
# Initialize Regs & Initialize Cache
#-------------------------------------------------------------------------------------------------
M_DebugCOUNTDM      = (0x1  << 25)

M_StatusCU          = (0xf  << 28)
M_StatusRP          = (0x1  << 27)
M_StatusRE          = (0x1  << 25)
M_StatusBEV         = (0x1  << 22)
M_StatusTS          = (0x1  << 21)
M_StatusSR          = (0x1  << 20)
M_StatusNMI         = (0x1  << 19)
M_StatusIM7         = (0x1  << 15)
M_StatusIM2         = (0x1  << 10)
M_StatusIM          = (0xff << 8)
M_StatusUM          = (0x1  << 4)
M_StatusERL         = (0x1  << 2)
M_StatusEXL         = (0x1  << 1)
M_StatusIE          = (0x1  << 0)

M_CauseIV           = (0x1  << 23)
M_CauseWP           = (0x1  << 22)
M_CauseIP1          = (0x1  << 9)
M_CauseIP0          = (0x1  << 8)

M_ConfigK23         = (0x7  << 28)
M_ConfigKU          = (0x7  << 25)
M_ConfigMT          = (0x7  << 7)
M_ConfigK0          = (0x7  << 0)
K_CacheAttrCN       = (0x3  << 0) #Cacheable, noncoherent, write-back, write allocate.
S_Config1IS         = 22
M_Config1IS         = (0x7  << S_Config1IS)
S_Config1IL         = 19
M_Config1IL         = (0x7  << S_Config1IL)
S_Config1IA         = 16
M_Config1IA         = (0x7  << S_Config1IA)
S_Config1DS         = 13
M_Config1DS         = (0x7  << S_Config1DS)
S_Config1DL         = 10
M_Config1DL         = (0x7  << S_Config1DL)
S_Config1DA         = 7
M_Config1DA         = (0x7  << S_Config1DA)

INITIAL_STATUS      = 0x10400000
INITIAL_CONFIG0     = 0x00000002 #Uncached

Index_Store_Tag_I   = 0x8
Index_Store_Tag_D   = 0x9

A_DSPRAM            = 0x04000000        # 0x84000000
A_KSEG0BASE         = 0x80000000


#-------------------------------------------------------------------------------------------------
# Flash registers
#-------------------------------------------------------------------------------------------------
REG_ISP_CLK_SRC             =   (0xBF800000 + 0x1E26*4)
REG_SFSH_PIU_BASE           =   0xBF807800
REG_SFSH_DMA_STATE          =   (REG_SFSH_PIU_BASE + 0x10*4) #[1]done [2]busy [8:15]state
REG_SFSH_DMA_SPISTART_L     =   (REG_SFSH_PIU_BASE + 0x70*4) #[15:0]
REG_SFSH_DMA_SPISTART_H     =   (REG_SFSH_PIU_BASE + 0x71*4) #[23:16]
REG_SFSH_DMA_DRAMSTART_L    =   (REG_SFSH_PIU_BASE + 0x72*4) #[15:0]  in unit of B; must be 8B aligned
REG_SFSH_DMA_DRAMSTART_H    =   (REG_SFSH_PIU_BASE + 0x73*4) #[23:16]
REG_SFSH_DMA_SIZE_L         =   (REG_SFSH_PIU_BASE + 0x74*4) #[15:0]  in unit of B; must be 8B aligned
REG_SFSH_DMA_SIZE_H         =   (REG_SFSH_PIU_BASE + 0x75*4) #[23:16]
REG_SFSH_DMA_CMD            =   (REG_SFSH_PIU_BASE + 0x76*4)
SFSH_DMA_ALIGN              =   8

#-------------------------------------------------------------------------------------------------
# Other registers
#-------------------------------------------------------------------------------------------------
REG_TOP_POWER_STATUS        = 0xbf803cdc

#-------------------------------------------------------------------------------------------------
# UART registers
#-------------------------------------------------------------------------------------------------
UART_BASE               = 0xBF808C80
UART_RX		            = (0*4 + UART_BASE)
UART_TX		            = (0*4 + UART_BASE)
UART_DLL	            = (0*4 + UART_BASE)
UART_DLM	            = (1*4 + UART_BASE)
UART_IER	            = (1*4 + UART_BASE)
UART_IIR	            = (2*4 + UART_BASE)
UART_FCR	            = (2*4 + UART_BASE)
UART_EFR	            = (2*4 + UART_BASE)
UART_LCR	            = (3*4 + UART_BASE)
UART_MCR	            = (4*4 + UART_BASE)
UART_LSR	            = (5*4 + UART_BASE)
UART_MSR	            = (6*4 + UART_BASE)
UART_SCR	            = (7*4 + UART_BASE)

#Baud rate
UART_BAUD_RATE	        = 38400

#CPU clock
CPU_CLOCK_FREQ          = 216000000

#Clock source
UART_CLK_FREQ           = 14318180  #43200000  #14318180

#Divisor
UART_DIVISOR            = (UART_CLK_FREQ/(16 * UART_BAUD_RATE))

#-------------------------------------------------------------------------------------------------
# Macros
#-------------------------------------------------------------------------------------------------
.macro WREG reg, value
lui     $8, \reg >> 16
li      $9, \value
sw      $9, (\reg & 0xFFFF)($8)
.endm

.macro DBGREG value
#    WREG    0xBF803DDC, \value
.endm

.macro PUTC char
    WREG    UART_TX, \char
.endm

.macro DELAYUS us
        li      $10, \us * (CPU_CLOCK_FREQ/2/1000000) #ticks
        mfc0    $8, $9      #tick1
1:
        mfc0    $9, $9      #tick2
        subu    $11, $9, $8 #tick2-tick1
        blt     $11, $10, 1b
        nop
.endm

.macro RREG reg_d, reg
li      $26, \reg
lw      \reg, 0($26)
.endm


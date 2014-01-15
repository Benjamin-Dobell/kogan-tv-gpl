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
	WREG 	0xbf2025c8 MIU0_DO_BIST_LEN_L
	LRREG	$4, 0xbf2025cc
	andi	$4, $4, 0xFF00
	ori		$4, $4, MIU0_DO_BIST_LEN_H
	LSSSW    0xbf2025cc, $4

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

	LRREG	$4, 0xbf2025e0
	ori		$4, $4, 0x8000
	LSSSW    0xbf2025e0, $4

	LRREG	$4, 0xbf2025c0
	andi	$4, $4, 0xFF00
	LSSSW    0xbf2025c0, $4

	ori	$7, $0, MIU1_DO_BIST_PATTERN

	# set test length
	LWREG 	0xbf2025c8 MIU1_DO_BIST_LEN_L
	LRREG	$4, 0xbf2025cc
	andi	$4, $4, 0xFF00
	ori		$4, $4, MIU1_DO_BIST_LEN_H
	LSSSW    0xbf2025cc, $4

	bal		BOOTROM_MIU_BIST
	nop

	LRREG	$4, 0xbf2025c0
	andi	$4, $4, 0xFF00
	LSSSW    0xbf2025c0, $4

	LRREG	$4, 0xbf2025e0
	andi	$4, $4, 0x7FFF
	LSSSW    0xbf2025e0, $4
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
	WREG 	0xbf2025c4 0x0000

	#   test pattern
	sw      $7, (0x25d0)($8)
	#   enable test, order (high byte then low byte) is important
	LRREG	$4, 0xbf2025c0
	andi	$4, $4, 0x00FF
	LSSSW    0xbf2025c0, $4
	LRREG	$4, 0xbf2025c0
	andi	$4, $4, 0xFF00
	LSSSW    0xbf2025c0, $4		# clear enable bit
	LRREG	$4, 0xbf2025c0
	andi	$4, $4, 0xFF00
	ori		$4, $4, 0x0003
	LSSSW    0xbf2025c0, $4		# pattern from data
	#   check result
MIU_BIST_CHK:
	LRREG	$4, 0xbf2025c0
	andi	$4, $4, 0x8000
    bnez	$4, MIU_BIST_CHK_RES
    nop
	j MIU_BIST_CHK
	nop
MIU_BIST_CHK_RES:
	LRREG	$4, 0xbf2025c0
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
.if (SUPPORT_TITANIA3_UART)
    #Setup UART clk to 123MHZ(for higher bandrate)
    WREG 0xbf203C7C, 0x00FF
    #LWREG 0xbf203C7C, 0x2C00 #123000000
    LWREG 0xbf203C7C, 0x2000 #172800000
    LWREG 0xbf203C8C, 0x0100

    # LWREG 0xbf203d54, 0x1234;  # select uart0 source to tsp
    # LWREG 0xbf001c24, 0x00;  # switch uart from hk51 to mips
    # LWREG 0xbf001c24, 0x800;  # switch uart from hk51 to mips

    LWREG 0xbf203d4c, 0x2104
    WREG 0xbf001c24, 0x0800
    WREG 0xbf203c04, 0x0400
    LWREG 0xbf203db8, 0xF0FF

    #Disable all UART interrupts
    LWREG    UART_IER, 0x00

    #Reset receiver and transmitr
    LWREG    UART_FCR, 0x07

    #Set 8 bit char, 1 stop bit, no parity
    LWREG    UART_LCR, 0x03

    #Set baud rate
    LWREG    UART_LCR, 0x83
    LWREG    UART_DLM, ((UART_DIVISOR >> 8) & 0x00ff)
    LWREG    UART_DLL, UART_DIVISOR & 0x00ff
    LWREG    UART_LCR, 0x03

.else
	#Setup UART clk to 123MHZ(for higher bandrate)
	WREG 0xbf203c8c 0x0008
	LWREG 0xbf203c78 0x01C0

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
    WREG 0xbf006010, 0x0000 # watch dog
    LWREG 0xbf006014, 0x0000 # watch dog


    # i2c PASSWD
    WREG 0xbf203B9C, 0x6677
    LWREG 0xbf203Af8, 0x4455
    LWREG 0xbf203Afc, 0x2233
    LWREG 0xbf203B04, 0x0011

    LWREG 0xbf203Bf0, 0x3210
    LWREG 0xbf203Bf0, 0x7654
    LWREG 0xbf203Bf0, 0xba98
    LWREG 0xbf203Bf0, 0xfedc
    LWREG 0xbf203Bec, 0x1234

    LWREG 0xbf203188, 0x000A # ICE Step issue
    LWREG 0xbf20318C, 0x0060 # ec bridge timeout
    LWREG 0xbf203190, 0x0060 # ec bridge threshold

    # T001_MainPll_MIPSPLL_UPLL_sim.txt
    #turn on mpll and UPLL
    LWREG 0xbf204a1c, 0x001f #soft-reset atop/pllb/adcb/adca/plla
    LWREG 0xbf204a1c, 0x0000 
    LWREG 0xbf204a10, 0x0000 #power on adc_atop
        LWREG 0xbf204A14, 0x0000
        LWREG 0xbf204A18, 0x0000

    WREG 0xbf221218, 0x0000 #power on xtal/plla/adca/pllb/adb.....
    LWREG 0xbf221380, 0x0000 #power on DVI PLL,bias ....
    LWREG 0xbf2213a4, 0x0000 #power on DVI_2 PLL,bias ....

    #clk_dvi
    WREG 0xbf001d28, 0x0000 #PD_ IBGREX
    LWREG 0xbf001d30, 0x0000 #PD_BG

    # LWREG 0xbf001d2e, 0x0000 #PD_CLKIN :074b[8] /PD_CLKIN_OFFL:074b[9]/PD_RCK:074b[13:11]
    WREG 0xbf001d2c, 0x0000 #PD_CLKIN :074b[8] /PD_CLKIN_OFFL:074b[9]/PD_RCK:074b[13:11]
    
    # start turn MPLL
    WREG 0xbf203c70, 0x0004 #turn on bit0=0 : mpll power on, bit1 =0: MIPSPLL and bit 2: USB PLL
    WREG 0xbf221820, 0x0004 #MPLL enable
    
    #   the MPLL output=24x4x9/4=216M
    LWREG 0xbf22182c, 0x0000 #MPLL div clk
     
    # turn 480Mhz in USB
        # USBC0 <--> UTMI0
    WREG 0xbf200e00, 0x000a # Disable MAC initial suspend, Reset UHC
    LWREG 0xbf200e00, 0x0028 # Release UHC reset, enable UHC and OTG XIU function
     
    LWREG 0xbf207544, 0xc000 # Set PLL_TEST[23:22] for enable 480MHz clock
    LWREG 0xbf207540, 0x0002 #UTMI offset 0x20 bit1=1#Set PLL_TEST[1] for PLL multiplier 20X
    
    LWREG 0xbf207510, 0x0000 # #7=0 Enable band-gap current # #3=0 Disable force_pll_on
    
    LWREG 0xbf207500, 0x6bc3 # reg_pdn: bit<15>, bit <2> ref_pdn # Turn on reference voltage and regulator
    DELAYUS 5000 # WAIT 0.001000s # delay 1ms
     
    LWREG 0xbf207500,                                    0x69c3 # Turn on UPLL, reg_pdn: bit<9>
    DELAYUS 5000 # WAIT 0.002000s # delay 2ms
     
        LWREG 0xbf207500, 0x0001 # Turn all (including hs_current) use override mode
    LWREG 0xbf207504, 0x0000 ##7=0
     
    
		# USBC1 <--> UTMI1
        LWREG 0xbf200F00, 0x000A # Disable MAC initial suspend, Reset UHC
        LWREG 0xbf200F00, 0x0068 # Release UHC reset, enable UHC and OTG XIU function

    LWREG 0xbf207410, 0x0000 # Enable band-gap current # Disable force_pll_on
    LWREG 0xbf207400, 0x6bc3 # reg_pdn: bit<15>, bit <2> ref_pdn
    DELAYUS 5000 # WAIT 0.001000s # delay 1ms
     
    LWREG 0xbf207400, 0x69c3 # Turn on UPLL, reg_pdn: bit<9>
    DELAYUS 5000 # WAIT 0.002000s # delay 2ms
     
    LWREG 0xbf207400, 0x0001 # Turn all (including hs_current) use override mode
    
    LWREG 0xbf207404, 0x0000 ##7=0
     
    #mipspll
    WREG 0xbf221860, 0x0004 #MPLL enable

    LWREG 0xbf221868, 0x001c #loop_div2=28
    #   the MIPSMPLL output=(24x28)/(4)=336M

    # mips 144MHz
    LWREG 0xbf221864, 0x0014
    LWREG 0xbf221868, 0x0006

    LWREG 0xbf22186c, 0x0000 #MIPSMPLL div clk #216M main clk out

    # t002_lpll_1366x768.txt
    #turn LPLL for 1366x768 panel
    WREG 0xbf20620c, 0x0002 # set single lvds mode, ictrl = 2
    LWREG 0xbf206204, 0x0203 # set loop_div_1st = 8
    
    LWREG 0xbf20623c, 0x9640 
    LWREG 0xbf206240, 0x002f # set clock to 83Mhz
     
    LWREG 0xbf206400, 0x0000 
    # HELP
    # LWREG 0xbf206482, 0x0000 #[7:4]ckg_dot_mini_pre [3:0]ckg_dot_mini 
    LWREG 0xbf206480, 0x0000 #[7:4]ckg_dot_mini_pre [3:0]ckg_dot_mini 
    
    # HELP
    # LWREG 0xbf2064de, 0x0000 #clr reg_gcr_pd_vdb in initial,
    LWREG 0xbf2064dc, 0x0000 #clr reg_gcr_pd_vdb in initial,
    LWREG 0xbf206500, 0x000c #[6]ch_swap [5]ch_polarity [3]pdp_10bit [2]lvds_ti
    LWREG 0xbf206514, 0x003f #[5]la_oen [4]lb_oen
    LWREG 0xbf206528, 0x0001 #[1]dualmode [0]abswitch
    LWREG 0xbf20652c, 0x0002 #[1:0]ti_bit_mode 0x:10bit, 10:8bit, 01:6bit
    LWREG 0xbf2065b4, 0x0000 #output config
    LWREG 0xbf2065b8, 0x5500 #output config
    LWREG 0xbf2065bc, 0x0005 #output config
    LWREG 0xbf2065dc, 0x0007 #[3]en_portb_ib [1]en_porta_ib [0]en_ck
    LWREG 0xbf2065e0, 0x0046 #[6]swing2x [5:1] swing [0]pd_mod
     
    #------------------------------
    LWREG 0xbf20175c, 0x0000 #CLK_GEN0_0x57 bit0~3 =000b ; ckg_fifo_mini = CLK_LPLL
     
    
    ########################################
    #				SC1 (Base: 0x2F00) SC_BK_10
    ########################################
    LWREG 0xbf205e00, 0x0010 # switch bank to VOP SC_BK_10
     ##################
    ##   H_Total		  #
    ##################
    LWREG 0xbf205e30, 0x0617 # reg_hdtot[7,0]		: Output Horizontal Total (### htotal = 1560 - 1 = 1559 ###) # reg_hdtot[3,0]H
     ##################
    ##   V_Total		  #
    ##################
    LWREG 0xbf205e34, 0x0325 # reg_vdtot[7,0]		  : Output Vertical Total (### vtotal = 806 - 1 = 805 ###) # reg_vdtot[3,0]H
     ##################
    ##   DE_HStart	  #
    ##   DE_HEnd		  #
    ##################
    LWREG 0xbf205e10, 0x003c # reg_dehst[7,0]			: External VD Using Sync (### DE hstart = 60 ###) # 0x0000 # reg_dehst[3,0]H			# 0: Sync is Generated from Data Internally
                                                        	    # 1: Sync from External Source
    LWREG 0xbf205e14, 0x0591 # reg_dehend[7,0]			: Output DE Horizontal END (### DE hend = 60 + 1366 - 1 = 1425 ###)
    # LWREG 0xbf205e16, 0x0500 # reg_dehend[3,0]H
    
    ##################
    ##   DE_VStart	  #
    ##   DE_VEnd	 	  #
    ##################
    LWREG 0xbf205e18, 0x0000 # reg_devst[7,0]			: Output DE Vertical Start (### DE vstart = 38 ###)
    # LWREG 0xbf205e1a,                                    0x0000 # reg_devst[3,0]H			# 00: Default value
    LWREG 0xbf205e1c, 0x02ff # reg_devend[7,0]			: Output DE Vertical END (### DE vend = 38 + 768 - 1= 805 ###)
    # LWREG 0xbf205e1e,                                    0x0200 # reg_devend[3,0]H

     ##################
    ##  Main_HStart  #
    ##  Main_HEnd    #
    ##################
    LWREG 0xbf205e20, 0x003c # reg_sihst[7,0]			: Scaling Image window Horizontal Start
    # LWREG 0xbf205e22,                                    0x0000 # reg_sihst[3,0]H
    LWREG 0xbf205e24, 0x0591 # reg_sihend[7,0]			: Scaling Image window Horizontal End
    # LWREG 0xbf205e26,                                    0x0500 # reg_sihend[3,0]H

     ##################
    ##  Main_VStart  #
    ##  Main_VEnd    #
    ##################
    LWREG 0xbf205e28, 0x0000 # reg_sivst[7,0]			: Scaling Image window Vertical Start (
    # LWREG 0xbf205e2a,                                    0x0000 # reg_sivst[3,0]H
    LWREG 0xbf205e2c, 0x02ff # reg_sivend[7,0]			: Scaling Image window Vertical END
    # LWREG 0xbf205e2e,                                    0x0200 # reg_sivend[3,0]H
    
     ##################
    ## Frame,Window  #
    ##   Control     #
    ##################
    LWREG 0xbf205e64, 0x0001 # reg_adead_en[7]			: Ahead mode enable
    # LWREG 0xbf205e66,                                    0x0000 ##reg_fcl_r[7,0]			:	Frame Color ¡V Red
    LWREG 0xbf205e68, 0xf000 ##reg_fcl_b[7,0]			:	Frame Color ¡V Blue
    # LWREG 0xbf205e6a,                                    0xf000 ##reg_fcl_g[7,0]			: Frame Color ¡V Green
    LWREG 0xbf205e90, 0x1ce0 # reg_mns_col[7,0]			: Main Window No Signal Color
    # LWREG 0xbf205e92,                                    0x1c00 # reg_mbcol[7,0]			: Main Window Border Color
    LWREG 0xbf205e70, 0x0000 # reg_rst_e_4_frame[7]		: reset noise generator by frames enable
     																												# 1: Tempo-dither every 2 frames
    #####################
    ## Output Free-run  #
    #####################
    # LWREG 0xbf205e72,                                    0x0100 # reserved[7]				:	reserved
    LWREG 0xbf205e70, 0x0100 # reserved[7]				:	reserved
     
    
    #NOISY_GEN
    # LWREG 0xbf205e4e,                                    0x1100 #enable noisy_gen form Cloud's suggestion
    LWREG 0xbf205e4c, 0x1100 #enable noisy_gen form Cloud's suggestion
     
    ########################################
    #				CHIPTOP (Base: 0x1E00)
    ########################################
    #
    #	Turn on odclk:  CLK select LPLL output
    #
    LWREG 0xbf203c74, 0x001c # reg_ckg_odclk[5,0]			: enable odclk clock(clock source: select lpll output clock)
     						  # reg_ckg_psram[7,6]			: clk_psram
    #
    #	Set all peds as output
    #
    # LWREG 0xbf203d42,                                    0x0000 # reg_iicmmode[1,0]
    LWREG 0xbf203d40,                                    0x0000 # reg_iicmmode[1,0]
 
    # t003_wait50ms.txt
    DELAYUS 100000 # WAIT 0.001000s # delay 1ms

    # t004_2_MIU_atopInit_ddr3_BGA.txt
    WREG 0xbf221a00, 0x0001 
    LWREG 0xbf221a10, 0xffff 
    LWREG 0xbf221a14, 0x0007 
    LWREG 0xbf221a18, 0x0000 
    LWREG 0xbf221a20, 0x0000 
    LWREG 0xbf221a24, 0x0000 
    LWREG 0xbf221a40, 0x0020 
    LWREG 0xbf221a44, 0x0010 
    LWREG 0xbf221a48, 0x0000 
    LWREG 0xbf221a50, 0x0000 
    LWREG 0xbf221a54, 0x0000 
    LWREG 0xbf221a58, 0x0000 
    LWREG 0xbf221a80, 0x3333 
    LWREG 0xbf221a84, 0x3333 
    LWREG 0xbf221a88, 0x3333 
    LWREG 0xbf221a8c, 0x3333 
    LWREG 0xbf221a90, 0x3333 
    LWREG 0xbf221a94, 0x3333 
    LWREG 0xbf221a98, 0x3333 
    LWREG 0xbf221a9c, 0x3333 
    LWREG 0xbf221aa0, 0x0000 
    LWREG 0xbf221aa4, 0x0000 
    LWREG 0xbf221aa8, 0x0000 
    LWREG 0xbf221ac0, 0x2338 #need refine when chip back
.if BD_MST087B_D01A
    LWREG 0xbf221ac4, 0x307f 
.else #BD_MST087B_D01A
    LWREG 0xbf221ac4, 0x807f
.endif #BD_MST087B_D01A
    LWREG 0xbf221ac8, 0xe200 
    LWREG 0xbf221acc, 0x0000 
    LWREG 0xbf221ad0, 0x0000 
    LWREG 0xbf221ae0, 0x0300 
.if BD_MST087B_D01A
    LWREG 0xbf221ae4, 0x0004 #0x05 suggestion by Yolin
.else #BD_MST087B_D01A
    LWREG 0xbf221ae4, 0x0008 #0x05 suggestion by Yolin
.endif #BD_MST087B_D01A
    LWREG 0xbf221af0, 0x0000 
    LWREG 0xbf221af4, 0x0007 

    LWREG 0xbf221b00, 0x0001 
    LWREG 0xbf221b10, 0xffff 
    LWREG 0xbf221b14, 0x0007 
    LWREG 0xbf221b18, 0x0000 
    LWREG 0xbf221b20, 0x0000 
    LWREG 0xbf221b24, 0x0000 
    LWREG 0xbf221b40, 0x0020 
    LWREG 0xbf221b44, 0x0010 
    LWREG 0xbf221b48, 0x0000 
    LWREG 0xbf221b50, 0x0000 
    LWREG 0xbf221b54, 0x0000 
    LWREG 0xbf221b58, 0x0000 
    LWREG 0xbf221b80, 0x3333 
    LWREG 0xbf221b84, 0x3333 
    LWREG 0xbf221b88, 0x3333 
    LWREG 0xbf221b8c, 0x3333 
    LWREG 0xbf221b90, 0x3333 
    LWREG 0xbf221b94, 0x3333 
    LWREG 0xbf221b98, 0x3333 
    LWREG 0xbf221b9c, 0x3333 
    LWREG 0xbf221ba0, 0x0000 
    LWREG 0xbf221ba4, 0x0000 
    LWREG 0xbf221ba8, 0x0000 
    LWREG 0xbf221bc0, 0x2338 #need refine when chip back
    LWREG 0xbf221bc4, 0x307f 
    LWREG 0xbf221bc8, 0xe200 
    LWREG 0xbf221bcc, 0x0000 
    LWREG 0xbf221bd0, 0x0000 
    LWREG 0xbf221be0, 0x0300 
    LWREG 0xbf221be4, 0x0004 #0x05  sugestion by Yolin
    LWREG 0xbf221bf0, 0x0000 
    LWREG 0xbf221bf4, 0x0007 

    #t005_clk_gen0_ATSC_sim.txt
    WREG 0xbf201600, 0x0000 # turn xtal clk
    LWREG 0xbf201604, 0x0007 # SRAM bist test clk 7:xtal, 6:54M, 5:62M 4:72M 3:86M 2:108M 1:123M 0:144M
     
    #wriu 0x100b0a 0x00        #stc0 synthesizer
    #wriu 0x100b0b 0x00        #keep the value to default
    #wriu 0x100b0c ~ 0x100b0F  # finetune when TSP need
    
    #wriu 0x100b10 ~ 0x100b17  #DC synthersizer , keep the value to defult, finetune when XC need

    WREG 0xbf001c80, 0x0000 #step 1/2:set hk51 to 172.8M
    LWREG 0xbf001c80, 0x0080 #step 2/2:set hk51 to 172.8M
    # HELP
    # LWREG 0xbf201642,                                    0x0000 #turn ckg_mcu for turn riu_bridge,db_bus....
    WREG 0xbf201640, 0x0000 #turn ckg_mcu for turn riu_bridge,db_bus....
     
    # mips OCP
    LWREG 0xbf203a3c, 0x0050
        DELAYUS 10000
    LWREG 0xbf203a3c, 0x0051
    DELAYUS 10000
    LWREG 0xbf203a3c, 0x0030
    DELAYUS 10000
    LWREG 0xbf203a3c, 0x0031
    DELAYUS 10000

    # Richard: remove it since it cause JTAG disconnect
    # LWREG 0xbf201644, 0x0100 #turn OCP clk

    # switch to clk_mipspll_vco
    LWREG 0xbf201644, 0x0000
    LWREG 0xbf201644, 0x8000
     
    LWREG 0xbf201648, 0x0004 # step_1/2 turn aeon clk to 172.8Mhz
    LWREG 0xbf201648, 0x0084 # step_2/2 turn aeon clk to 172.8Mhz
     
    #LWREG 0xbf20164c, 0x0000 # set UART clk to 12Mhz
    LWREG 0xBF20164C, 0x0C01 # UART clk to 123Mhz
    
    #wriu  0x100b27 ~ 0x100b29  #select Uart clk to mpll, (Uart2 has fast uart function)
    
    #wriu 0x100b2b  0x01    #default sbus select :original mode
    #wriu 0x100b2c  0x00    #default , turn spi_clk on,<<check with 0x000e20>>
    
    LWREG 0xbf201668, 0x0000 # set pcm clk to 27Mhz
    
     
    #<<MIU relative >>#
    #wriu 0x0x100b3c   #donot care if MIU_CLK set to 192MHZ
    # HELP
    # LWREG 0xbf20167a,                                    0x0000 #miu_syn clk select for MEMPLL  0: 216M, 1:432M
    LWREG 0xbf201678, 0x0000 #miu_syn clk select for MEMPLL  0: 216M, 1:432M
    LWREG 0xbf20167c, 0x0c14 #ckg_miu set to 192Mhz
    # LWREG 0xbf20167e,                                    0x0c00 #ckg_miu_rec t xtal
     
    LWREG 0xbf201680, 0x0000 #clk vd_sel = 0: vd_adc_clk /1: adc_clk
    # LWREG 0xbf201682,                                    0x0000 #ckg_vd= vd_sel
     
    LWREG 0xbf201684, 0x0410 #ckg_vdmcu = 108Mhz , need fine tune in demod
    # LWREG 0xbf201686,                                    0x0400 #clk vd_200=216Mhz
     
    LWREG 0xbf201688, 0x0c0c #clk mail box0 =xtal  <<hk51 <--mail box 0--> aeon
    # LWREG 0xbf20168a,                                    0x0c00 #clk mail box0 =xtal  <<hk51 <--mail box 1--> aeon
     
    LWREG 0xbf20168c, 0x0000 #tur on vd2x
    # LWREG 0xbf20168e,                                    0x0000 #tur on vd_32fsc
     
    LWREG 0xbf201690, 0x0000 #ckg_Ve=27M
    # LWREG 0xbf201692,                                    0x0000 #ckg_Vedec=27M
    LWREG 0xbf201694, 0x000c #ckg_Ve_in=clk_DC0
     
    LWREG 0xbf201698, 0x0404 #clk_daca2=VD_clk
    # LWREG 0xbf20169a,                                    0x0400 #clk_dscb2=VD_CLK
     
    LWREG 0xbf2016a0, 0x0008 #clk_ts0=demode, CLK_TS1= gating
    # HELP
    # LWREG 0xbf2016a6,                                    0x0000 #CLK_TS0_OUT= 27M  #****
    LWREG 0xbf2016a4,                                    0x0000 #CLK_TS0_OUT= 27M  #****
    LWREG 0xbf2016a8, 0x0000 #CLK_TS0 = 144M
    # LWREG 0xbf2016aa,                                    0x0000 #Ckg_stc0 = stc0_sync_output

    #wriu 0x100b56,0x100b57,0x100b58, 0x100b59 #clk_ts0 ...refine when chip back....
    LWREG 0xbf2016c0, 0x0000 #ckg_vd_mheg5 =160Mhz

    LWREG 0xbf2016c4, 0x0004 #ckg_HVD =144Mhz
    LWREG 0xbf2016d0, 0x0000 #ckg_RVD =123Mhz 0:123M, 4:108M, 8:86M c:72M, 0x10:xtal

    LWREG 0xbf2016d4, 0x0008 #ckg_jpg=123M  0:72m 4:108m c:123m c:reserved
    LWREG 0xbf2016e0, 0x0000 #ckg_mad_stc= stc_syn
    # LWREG 0xbf2016e2,                                    0x0000 #mvd_syn on

    LWREG 0xbf2016e4, 0x0400 #mvd_clk =144m
    # LWREG 0xbf2016e6,                                    0x0400 #mvd2_clk=144m

    LWREG 0xbf2016e8, 0x0000 #mvd_chroma_a,luma_b,c ON,
    # LWREG 0xbf2016ea,                                    0x0000 
    LWREG 0xbf2016ec, 0x0000 
    # LWREG 0xbf2016ee,                                    0x0000 
    LWREG 0xbf2016f0, 0x0000 
    # LWREG 0xbf2016f2,                                    0x0000 
    LWREG 0xbf2016f4, 0x0000 
    # LWREG 0xbf2016f6,                                    0x0000 
    LWREG 0xbf2016f8, 0x0000 
    # LWREG 0xbf2016fa,                                    0x0000 
    LWREG 0xbf2016fc, 0x0000 

    LWREG 0xbf201700, 0x0000 #clk_gop_0 ==odclk_p
    # LWREG 0xbf201702,                                    0x0000 #clk_gop_1 ==odclk_p
    LWREG 0xbf201704, 0x0100 #clk_gop_2 ==odclk_p
    # LWREG 0xbf201706,                                    0x0100 #gating first, ((clk_gopd (DWIN) = CLK_ODCLK))
    LWREG 0xbf201708, 0x0000 #clk_gop_3 ==odclk_p

    LWREG 0xbf20170c, 0x0000 #psram0=on
    # LWREG 0xbf20170e,                                    0x0000 #psram1=on

    LWREG 0xbf201720, 0x000c #ckg_ge=144M
    LWREG 0xbf201724, 0x0000 #ckg_strld=144M
    LWREG 0xbf201730, 0x0000 #ckg_dc0=synchronous mode
    # LWREG 0xbf201732,                                    0x0000 #ckg_dc1=synchronous mode

    #SC
    #wriu 0x100ba2 0x04   #sub_window fclk
    # HELP
    # LWREG 0xbf201746,                                    0x0400 #main_window fclk_f2
    LWREG 0xbf201744, 0x0400 #main_window fclk_f2
    # HELP
    # LWREG 0xbf20174a,                                    0x0000 #flck=170M
    LWREG 0xbf201748,                                    0x0000 #flck=170M
    LWREG 0xbf20174c, 0x1c1c #odclk (output pixel clock) = LPLL

    #wriu 0x100ba8      #idclk_0 depend src off_line signal detect
    #wriu 0x100ba9      #idclk_1 depend src sub_window
    #wriu 0x100baA      #idclk_2 depend src main_window

    #<<MOD>>
    LWREG 0xbf201758, 0x0000 #dot mmini clk= LPLL   #LVDS   fifo_clk=mini_lvds
    # LWREG 0xbf20175a,                                    0x0000 #dot mmini_pre clk= LPLL   #LVDS   fifo_clk=mini_lvds
    LWREG 0xbf20175c, 0x0000 #fifo mmini = LPLL   #LVDS   fifo_clk=mini_lvds

    #<<MAC>>
    LWREG 0xbf201780, 0x0400 #EMAC_ahb =0x00 <0:123M>
    # LWREG 0xbf201782,                                    0x0400 #EMAC_rx 25M>
    LWREG 0xbf201784, 0x0004 #EMAC_rx 25M>
    # LWREG 0xbf201786,                                    0x0000 #EMAC_txin=50M

    #<nand>>
    LWREG 0xbf201790, 0x0024 #NAND Clk=54M  <<T2 in 62M >>
     #wriu 0x100bca      #dominaitor
    #wriu 0x100bcb  0x00   #NAND 0: nand_clk=ckg_nand, 1: dominator

    #<<mpif>>
    LWREG 0xbf2017a0, 0x0020 # mpif  =20M

    #<<MEMPLL>>
    LWREG 0xbf2017f8, 0x0000 #mempll sync 0:216 1:432 2:xtal


    #<<CLK_GEN1_ATSC>>
    LWREG 0xbf201684, 0x0100 # reg_ckg_vdmcu@0x21[4:0]  (bank_CLKGEN0)
    # LWREG 0xbf201686,                                    0x0100 # reg_ckg_vd200@0x21[11:8]

    LWREG 0xbf206628, 0x0100 # reg_ckg_dvbtc_adc@0x0a[3:0] : ADC_CLK
    # LWREG 0xbf20662a,                                    0x0100 # reg_ckg_dvbtc_innc@0x0a[11:8]

    LWREG 0xbf206604, 0x0000 
    # LWREG 0xbf206606,                                    0x0000 

    LWREG 0xbf206608, 0x0000 
    # LWREG 0xbf20660a,                                    0x0000 

    LWREG 0xbf20660c, 0x0000 
    # LWREG 0xbf20660e,                                    0x0000 

    LWREG 0xbf206614, 0x0008 
    # LWREG 0xbf206616,                                    0x0000 

    LWREG 0xbf206630, 0x0000 
    # LWREG 0xbf206632,                                    0x0000 

    LWREG 0xbf206610, 0x0100 
    # LWREG 0xbf206612,                                    0x0100 

    LWREG 0xbf206600, 0x071b #if(TS is parallel mode) 0x071b, serial mode:0x0702
     # LWREG 0xbf206602,                                    0x0700 

    LWREG 0xbf203c44, 0x0002 #internal TS0:0x0002;   internal TS1:0x0200
     # LWREG 0xbf203c46,                                    0x0000 # (bank_CHIP)

    LWREG 0xbf2016a0, 0x0108 #internal TS0:0x0108;   internal TS1:0x0801
     # LWREG 0xbf2016a2,                                    0x0100 # (bank_CLKGEN0)

    LWREG 0xbf203c08, 0x0000 # reg_if_agc_pad_oen@0x02
    LWREG 0xbf203c08, 0x0000 # (bank_CHIP)

    # t006_2_miu_init_DDR3.txt

    LWREG 0xbf20243c, 0x0000 #mask all miu0/miu1 request
    LWREG 0xbf20243c, 0x0000 
    LWREG 0xbf202400, 0x0000 
    # LWREG 0xbf202402,                                    0x0000 #hold miu_0
    LWREG 0xbf200c00, 0x0000 
    # LWREG 0xbf200c02,                                    0x0000 #hold miu_1

.if BD_MST087B_D01A
    LWREG 0xbf202404, 0x03a3 
    # LWREG 0xbf202406,                                    0x0300 
    LWREG 0xbf202408, 0x000b 
    # LWREG 0xbf20240a,                                    0x0000 
    LWREG 0xbf20240c, 0x0650 
    # LWREG 0xbf20240e,                                    0x0600 
    LWREG 0xbf202410, 0x1dcc 
    # LWREG 0xbf202412,                                    0x1d00 
    LWREG 0xbf202414, 0x2886 
    # LWREG 0xbf202416,                                    0x2800 
    LWREG 0xbf202418, 0x96c8 
    # LWREG 0xbf20241a,                                    0x9600 
    LWREG 0xbf20241c, 0x4058 
    # LWREG 0xbf20241e,                                    0x4000 
    LWREG 0xbf202420, 0x0c70 
    # LWREG 0xbf202422,                                    0x0c00 
    LWREG 0xbf202424, 0x4004 
    # LWREG 0xbf202426,                                    0x4000 
    LWREG 0xbf202428, 0x8018 
    # LWREG 0xbf20242a,                                    0x8000 
    LWREG 0xbf20242c, 0xc000 
    # LWREG 0xbf20242e,                                    0xc000 
    LWREG 0xbf202430, 0x0000 
    # LWREG 0xbf202432,                                    0x0000 
    LWREG 0xbf202434, 0x0000 
    # LWREG 0xbf202436,                                    0x0000 
    LWREG 0xbf202438, 0x0000 
    # LWREG 0xbf20243a,                                    0x0000 
    LWREG 0xbf202440, 0x0250 #0x9e    #mempll 0x0193:1600Mhz
    # LWREG 0xbf202442,                                    0x0200 #0x01    #0x250 : 1.1G
    LWREG 0xbf202444, 0x0300 
    # LWREG 0xbf202446,                                    0x0300 
    LWREG 0xbf202448, 0x8000 #mem pll
    # LWREG 0xbf20244a,                                    0x8000 #mem pll
    LWREG 0xbf20244c, 0x0044 
    # LWREG 0xbf20244e,                                    0x0000 
    LWREG 0xbf202450, 0x0000 
    # LWREG 0xbf202452,                                    0x0000 
    LWREG 0xbf202454, 0xaaaa 
    # LWREG 0xbf202456,                                    0xaa00 
    LWREG 0xbf202458, 0x0680 
    # LWREG 0xbf20245a,                                    0x0600 
    LWREG 0xbf20245c, 0x0000 
    # LWREG 0xbf20245e,                                    0x0000 
    LWREG 0xbf202460, 0x0055 
    # LWREG 0xbf202462,                                    0x0000 

    LWREG 0xbf20246c, 0x0100 
    # LWREG 0xbf20246e,                                    0x0100 
    LWREG 0xbf202470, 0x0100 
    # LWREG 0xbf202472,                                    0x0100 
    LWREG 0xbf202478, 0x0000 
    # LWREG 0xbf20247a,                                    0x0000 
    LWREG 0xbf20247c, 0x0605 
    # LWREG 0xbf20247e,                                    0x0600 
.else #BD_MST087B_D01A
    LWREG 0xbf202404, 0x03A2  #bit1~0:ddr2:2  #bit1~0:ddr2->2; ddr3->3; bit9~8: 3->8x,
    LWREG 0xbf202408, 0x000A
    LWREG 0xbf20240C, 0x0438
    LWREG 0xbf202410, 0x1877
    LWREG 0xbf202414, 0x1f46
    LWREG 0xbf202418, 0x5486
    LWREG 0xbf20241C, 0x2044
    LWREG 0xbf202420, 0x0e73
    LWREG 0xbf202424, 0x4004
    LWREG 0xbf202428, 0x8000
    LWREG 0xbf20244C, 0xc000
    LWREG 0xbf202430, 0x0000
    LWREG 0xbf202434, 0x0000
    LWREG 0xbf202438, 0x0000
    #800MHz
    LWREG 0xbf202440, 0x02b4
    LWREG 0xbf202444, 0x0500
    LWREG 0xbf202448, 0x4000
    #1066MHz
    #LWREG 0xbf202440, 0x026e
    #LWREG 0xbf202444, 0x0300
    #LWREG 0xbf202448, 0x8000
    LWREG 0xbf20244C, 0x0044
    LWREG 0xbf202450, 0x0000
    LWREG 0xbf202454, 0xAAAA
    LWREG 0xbf202458, 0x4600
    LWREG 0xbf20245C, 0x0044
    LWREG 0xbf202460, 0x0044   #yolin's suggestion: MIU_0:0x44 /MIU_1:0x55
    LWREG 0xbf20246C, 0x0043
    LWREG 0xbf202470, 0x0043
    LWREG 0xbf202478, 0x0000
    LWREG 0xbf20247C, 0x0603
.endif #BD_MST087B_D01A

    LWREG 0xbf200c04, 0x03a3 
    # LWREG 0xbf200c06,                                    0x0300 
    LWREG 0xbf200c08, 0x000b 
    # LWREG 0xbf200c0a,                                    0x0000 
    LWREG 0xbf200c0c, 0x0650 
    # LWREG 0xbf200c0e,                                    0x0600 
    LWREG 0xbf200c10, 0x1dcc 
    # LWREG 0xbf200c12,                                    0x1d00 
    LWREG 0xbf200c14, 0x2886 
    # LWREG 0xbf200c16,                                    0x2800 
    LWREG 0xbf200c18, 0x96c8 
    # LWREG 0xbf200c1a,                                    0x9600 
    LWREG 0xbf200c1c, 0x4058 
    # LWREG 0xbf200c1e,                                    0x4000 
    LWREG 0xbf200c20, 0x0c70 
    # LWREG 0xbf200c22,                                    0x0c00 
    LWREG 0xbf200c24, 0x4004 
    # LWREG 0xbf200c26,                                    0x4000 
    LWREG 0xbf200c28, 0x8018 
    # LWREG 0xbf200c2a,                                    0x8000 
    LWREG 0xbf200c2c, 0xc000 
    # LWREG 0xbf200c2e,                                    0xc000 
    LWREG 0xbf200c30, 0x0000 
    # LWREG 0xbf200c32,                                    0x0000 
    LWREG 0xbf200c34, 0x0000 
    # LWREG 0xbf200c36,                                    0x0000 
    LWREG 0xbf200c38, 0x0000 
    # LWREG 0xbf200c3a,                                    0x0000 
    LWREG 0xbf200c40, 0x019e #mempll  1600Mhz
    # LWREG 0xbf200c42,                                    0x0100 
    LWREG 0xbf200c44, 0x0300 
    # LWREG 0xbf200c46,                                    0x0300 
    LWREG 0xbf200c48, 0x8000 #mem pll
    # LWREG 0xbf200c4a,                                    0x8000 #mem pll
    LWREG 0xbf200c4c, 0x0044 
    # LWREG 0xbf200c4e,                                    0x0000 
    LWREG 0xbf200c50, 0x0000 
    # LWREG 0xbf200c52,                                    0x0000 
    LWREG 0xbf200c54, 0xaaaa 
    # LWREG 0xbf200c56,                                    0xaa00 
    LWREG 0xbf200c58, 0x0680 
    # LWREG 0xbf200c5a,                                    0x0600 
        LWREG 0xbf200C5C, 0x0000
    # LWREG 0xbf200c5e,                                    0x0000 
    LWREG 0xbf200c60, 0x0055 
    # LWREG 0xbf200c62,                                    0x0000 
    LWREG 0xbf200c6c, 0x0100 
    # LWREG 0xbf200c6e,                                    0x0100 
    LWREG 0xbf200c70, 0x0100 
    # LWREG 0xbf200c72,                                    0x0100 
        LWREG 0xbf200C78, 0x0000
    # LWREG 0xbf200c7a,                                    0x0000 
    LWREG 0xbf200c7c, 0x0605 
    # LWREG 0xbf200c7e,                                    0x0600 
    



        LWREG 0xbf200C3C, 0x0C01 #reset MIU_1
    # LWREG 0xbf200c3e,                                    0x0c00 
    LWREG 0xbf200c3c, 0x0c00 
    # LWREG 0xbf200c3e,                                    0x0c00 
    
        LWREG 0xbf20243C, 0x0C01 #reset miu_0
    # LWREG 0xbf20243e,                                    0x0c00 
    LWREG 0xbf20243c, 0x0c00 
    # LWREG 0xbf20243e,                                    0x0c00 
    
    LWREG 0xbf202400, 0x0000 #auto init miu cycle
    # LWREG 0xbf202402,                                    0x0000 #hold miu_0
    LWREG 0xbf200c00, 0x0000 #hold miu_1
    # LWREG 0xbf200c02,                                    0x0000 
    
    DELAYUS 2000
    

    LWREG 0xbf202400, 0x0008 #DRAM reset
    # LWREG 0xbf202402,                                    0x0000 
    LWREG 0xbf200c00, 0x0008 #DRAM reset
    # LWREG 0xbf200c02,                                    0x0000 

    LWREG 0xbf202400, 0x000c #DRAmreset+DRAM chip select
    # LWREG 0xbf202402,                                    0x0000 
    LWREG 0xbf200c00, 0x000c #DRAmreset+DRAM chip select
    # LWREG 0xbf200c02,                                    0x0000 
    DELAYUS 2000

    LWREG 0xbf202400, 0x000e #DRAmreset+DRAM chip select+enable CKE
    # LWREG 0xbf202402,                                    0x0000 
    LWREG 0xbf200c00, 0x000e #DRAmreset+DRAM chip select+enable CKE
    # LWREG 0xbf200c02,                                    0x0000 
    DELAYUS 2000
    
    LWREG 0xbf202400, 0x000f #DRAmreset+DRAM chip select+enable CKE +auto initial dram cycle
    # LWREG 0xbf202402,                                    0x0000 
    LWREG 0xbf200c00, 0x000f #DRAmreset+DRAM chip select+enable CKE +auto initial dram cycle
    # LWREG 0xbf200c02,                                    0x0000 
    


    #<wait initial done reg_0x101200 #15=1, reg_0c100600 #15=1>

    #bist test here
    LWREG 0xbf20248c, 0x0000 #open all miu_0 mask
    # LWREG 0xbf20248e,                                    0x0000 
    LWREG 0xbf2024cc, 0x0000 
    # LWREG 0xbf2024ce,                                    0x0000 
    LWREG 0xbf20250c, 0x0000 
    # LWREG 0xbf20250e,                                    0x0000 

    LWREG 0xbf200c8c, 0x0000 #open all miu_1 mask
    # LWREG 0xbf200c8e,                                    0x0000 
    LWREG 0xbf200ccc, 0x0000 
    # LWREG 0xbf200cce,                                    0x0000 
    LWREG 0xbf200d0c, 0x0000 
    # LWREG 0xbf200d0e,                                    0x0000 

    LWREG 0xbf20243c, 0x0c08 #open all mask #reg_0x10121e#3 control all miu mask

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


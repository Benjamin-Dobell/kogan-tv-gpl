////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DRVSYS_H_
#define _DRVSYS_H_
#ifdef _DRV_SYS_C_
    #define DRVSYS_EX
#else
    #define DRVSYS_EX extern
#endif
/*
typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;
*/
#define code
#define BYTE u8

#define DISABLE                     0
#define ENABLE                      1
#define ENABLE_UART0                ENABLE
#define ENABLE_UART1                DISABLE
#define WATCH_DOG                   DISABLE
////////////////////////////////////////////////////////////////////////////////
// 8051 Interrupt functions
////////////////////////////////////////////////////////////////////////////////

// R8051XC
#define _GetInt_GLOBAL()            (EAL)
#define _GetInt_SERIAL0()           (ES0)
#define _GetInt_SERIAL1()           (IEN2 & 0x01)
#define _GetInt_TIMER0()            (ET0)
#define _GetInt_TIMER1()            (ET1)
#define _GetInt_TIMER2()            (ET2)
#define _GetInt_TIMER2EX()          (EXEN2)
#define _GetInt_EX0()               (EX0)
#define _GetInt_EX1()               (EX1)
#define _GetInt_EX2()               (EX2)
#define _GetInt_EX3()               (EX3)
#define _GetInt_EX4()               (EX4)
#define _GetInt_EX5()               (EX5)
#define _GetInt_EX6()               (EX6)
#define _GetInt_EX7()               (EX7)
#define _GetInt_EX8()               (IEN2 & 0x02)
#define _GetInt_EX9()               (IEN2 & 0x04)
#define _GetInt_EX10()              (IEN2 & 0x08)
#define _GetInt_EX11()              (IEN2 & 0x10)
#define _GetInt_EX12()              (IEN2 & 0x20)

#define _SetInt_GLOBAL( bEnable )   (EAL = bEnable)
#define _SetInt_SERIAL0( bEnable )  (ES0 = bEnable)
#define _SetInt_SERIAL1( bEnable )  (IEN2 = ((bEnable) ? (IEN2 | 0x01) : (IEN2 & ~0x01)))
#define _SetInt_TIMER0( bEnable )   (ET0 = bEnable)
#define _SetInt_TIMER1( bEnable )   (ET1 = bEnable)
#define _SetInt_TIMER2( bEnable )   (ET2 = bEnable)
#define _SetInt_TIMER2EX( bEnable ) (EXEN2 = bEnable)   // Timer2 external reload
#define _SetInt_EX0( bEnable )      (EX0 = bEnable)
#define _SetInt_EX1( bEnable )      (EX1 = bEnable)
#define _SetInt_EX2( bEnable )      (EX2 = bEnable)
#define _SetInt_EX3( bEnable )      (EX3 = bEnable)
#define _SetInt_EX4( bEnable )      (EX4 = bEnable)
#define _SetInt_EX5( bEnable )      (EX5 = bEnable)
#define _SetInt_EX6( bEnable )      (EX6 = bEnable)
#define _SetInt_EX7( bEnable )      (EX7 = bEnable)
#define _SetInt_EX8( bEnable )      (IEN2 = ((bEnable) ? (IEN2 | 0x02) : (IEN2 & ~0x02)))
#define _SetInt_EX9( bEnable )      (IEN2 = ((bEnable) ? (IEN2 | 0x04) : (IEN2 & ~0x04)))
#define _SetInt_EX10( bEnable )     (IEN2 = ((bEnable) ? (IEN2 | 0x08) : (IEN2 & ~0x08)))
#define _SetInt_EX11( bEnable )     (IEN2 = ((bEnable) ? (IEN2 | 0x10) : (IEN2 & ~0x10)))
#define _SetInt_EX12( bEnable )     (IEN2 = ((bEnable) ? (IEN2 | 0x20) : (IEN2 & ~0x20)))

// EX0
#define _GetInt_EX0_BIT( IntSrc ) \
    MDrv_GetRegBit(EX0_INT_MASK + (IntSrc / 8), 1 << (IntSrc & 0x07))

#define _GetInt_EX0_TIMER0()                _GetInt_EX0_BIT( 0 )
#define _GetInt_EX0_TIMER1()                _GetInt_EX0_BIT( 1 )
#define _GetInt_EX0_WDT()                   _GetInt_EX0_BIT( 2 )
#define _GetInt_EX0_AEON_TO_8051()         _GetInt_EX0_BIT( 3 )
#define _GetInt_EX0_8051_TO_AEON()         _GetInt_EX0_BIT( 4 )
#define _GetInt_EX0_8051_TO_CPU2()          _GetInt_EX0_BIT( 5 )
#define _GetInt_EX0_CPU2_TO_8051()          _GetInt_EX0_BIT( 6 )
#define _GetInt_EX0_CPU2_TO_AEON()         _GetInt_EX0_BIT( 7 )
#define _GetInt_EX0_AEON_TO_CPU2()         _GetInt_EX0_BIT( 8 )
#define _GetInt_EX0_JPD()                   _GetInt_EX0_BIT( 9 )
#define _GetInt_EX0_ML_DONE()               _GetInt_EX0_BIT( 10 )
#define _GetInt_EX0_HDMI_NON_PCM()          _GetInt_EX0_BIT( 11 )
#define _GetInt_EX0_SPDIF_IN_NON_PCM()      _GetInt_EX0_BIT( 12 )
#define _GetInt_EX0_EMAC                    _GetInt_EX0_BIT( 13 )
#define _GetInt_EX0_SE_DSP2UP()             _GetInt_EX0_BIT( 14 )
#define _GetInt_EX0_TSP_TO_AEON()          _GetInt_EX0_BIT( 15 )
#define _GetInt_EX0_VIVALDI_STR()           _GetInt_EX0_BIT( 16 )
#define _GetInt_EX0_VIVALDI_PTS()           _GetInt_EX0_BIT( 17 )
#define _GetInt_EX0_DSP_MIU_PROT()          _GetInt_EX0_BIT( 18 )
#define _GetInt_EX0_XIU_TIMEOUT()           _GetInt_EX0_BIT( 19 )
#define _GetInt_EX0_DMA_DONE()              _GetInt_EX0_BIT( 20 )
#define _GetInt_EX0_VE_VBI_F0()             _GetInt_EX0_BIT( 21 )
#define _GetInt_EX0_VE_VBI_F1()             _GetInt_EX0_BIT( 22 )
#define _GetInt_EX0_VDMCU()                 _GetInt_EX0_BIT( 23 )
#define _GetInt_EX0_VE_DONE_TT_IRQ()        _GetInt_EX0_BIT( 24 )
#define _GetInt_EX0_IR()                    _GetInt_EX0_BIT( 27 )
#define _GetInt_EX0_AFEC_VSYNC()            _GetInt_EX0_BIT( 28 )
#define _GetInt_EX0_DEC_DSP2UP              _GetInt_EX0_BIT( 29 )
//#define _GetInt_EX0_CPU2_WDT                _GetInt_EX0_BIT( 30 )
#define _GetInt_EX0_DSP2CPU2_INT()          _GetInt_EX0_BIT( 31 )

#define _SetInt_EX0_BIT( IntSrc, bEnable ) \
    MDrv_WriteRegBit(EX0_INT_MASK + (IntSrc / 8), !(bEnable), 1 << (IntSrc & 0x07))

#define _SetInt_EX0_TIMER0( bEnable )           _SetInt_EX0_BIT( 0, bEnable )
#define _SetInt_EX0_TIMER1( bEnable )           _SetInt_EX0_BIT( 1, bEnable )
#define _SetInt_EX0_WDT( bEnable )              _SetInt_EX0_BIT( 2, bEnable )
#define _SetInt_EX0_AEON_TO_8051( bEnable )     _SetInt_EX0_BIT( 3, bEnable )
#define _SetInt_EX0_8051_TO_AEON( bEnable )     _SetInt_EX0_BIT( 4, bEnable )
#define _SetInt_EX0_8051_TO_CPU2( bEnable )     _SetInt_EX0_BIT( 5, bEnable )
#define _SetInt_EX0_CPU2_TO_8051( bEnable )     _SetInt_EX0_BIT( 6, bEnable )
#define _SetInt_EX0_CPU2_TO_AEON( bEnable )    _SetInt_EX0_BIT( 7, bEnable )
#define _SetInt_EX0_AEON_TO_CPU2( bEnable )    _SetInt_EX0_BIT( 8, bEnable )
#define _SetInt_EX0_JPD( bEnable )              _SetInt_EX0_BIT( 9, bEnable )
#define _SetInt_EX0_ML_DONE( bEnable )          _SetInt_EX0_BIT( 10, bEnable )
#define _SetInt_EX0_HDMI_NON_PCM( bEnable )     _SetInt_EX0_BIT( 11, bEnable )
#define _SetInt_EX0_SPDIF_IN_NON_PCM( bEnable ) _SetInt_EX0_BIT( 12, bEnable )
#define _SetInt_EX0_EMAC( bEnable )             _SetInt_EX0_BIT( 13, bEnable )
#define _SetInt_EX0_SE_DSP2UP( bEnable )        _SetInt_EX0_BIT( 14, bEnable )
#define _SetInt_EX0_TSP_TO_AEON( bEnable )     _SetInt_EX0_BIT( 15, bEnable )
#define _SetInt_EX0_VIVALDI_STR( bEnable )      _SetInt_EX0_BIT( 16, bEnable )
#define _SetInt_EX0_VIVALDI_PTS( bEnable )      _SetInt_EX0_BIT( 17, bEnable )
#define _SetInt_EX0_DSP_MIU_PROT( bEnable )     _SetInt_EX0_BIT( 18, bEnable )
#define _SetInt_EX0_XIU_TIMEOUT( bEnable )      _SetInt_EX0_BIT( 19, bEnable )
#define _SetInt_EX0_DMA_DONE( bEnable )         _SetInt_EX0_BIT( 20, bEnable )
#define _SetInt_EX0_VE_VBI_F0( bEnable )        _SetInt_EX0_BIT( 21, bEnable )
#define _SetInt_EX0_VE_VBI_F1( bEnable )        _SetInt_EX0_BIT( 22, bEnable )
#define _SetInt_EX0_VDMCU( bEnable )            _SetInt_EX0_BIT( 23, bEnable )
#define _SetInt_EX0_VE_DONE_TT_IRQ( bEnable )   _SetInt_EX0_BIT( 24, bEnable )
#define _SetInt_EX0_IR( bEnable )               _SetInt_EX0_BIT( 27, bEnable )
#define _SetInt_EX0_AFEC_VSYNC( bEnable )       _SetInt_EX0_BIT( 28, bEnable )
#define _SetInt_EX0_DEC_DSP2UP( bEnable )       _SetInt_EX0_BIT( 29, bEnable )
#define _SetInt_EX0_CPU2_WDT( bEnable )         _SetInt_EX0_BIT( 30, bEnable )
#define _SetInt_EX0_DSP2CPU2( bEnable )         _SetInt_EX0_BIT( 31, bEnable )

// EX1
#define _GetInt_EX1_BIT( IntSrc ) \
    MDrv_GetRegBit(EX1_INT_MASK + (IntSrc / 8), 1 << (IntSrc & 0x07))
#define _GetInt_EX1_EXP_BIT( IntSrc ) \
    MDrv_GetRegBit(REG_IRQ_EXP_MASK + ((IntSrc) - 32) / 8, 1 << (IntSrc & 0x07))

#define _GetInt_EX1_UART()                  _GetInt_EX1_BIT( 0 )
#define _GetInt_EX1_HOST_DMA0()             _GetInt_EX1_BIT( 1 )
#define _GetInt_EX1_HOST_DMA1()             _GetInt_EX1_BIT( 2 )
#define _GetInt_EX1_MVD()                   _GetInt_EX1_BIT( 3 )
#define _GetInt_EX1_PS()                    _GetInt_EX1_BIT( 4 )
#define _GetInt_EX1_NFIE()                  _GetInt_EX1_BIT( 5 )
#define _GetInt_EX1_USB()                   _GetInt_EX1_BIT( 6 )
#define _GetInt_EX1_UHC()                   _GetInt_EX1_BIT( 7 )
#define _GetInt_EX1_EMAC()                  _GetInt_EX1_BIT( 9 )
#define _GetInt_EX1_DISP()                  _GetInt_EX1_BIT( 10 )
#define _GetInt_EX1_DHC()                   _GetInt_EX1_BIT( 11 )
#define _GetInt_EX1_PM_SLEEP()              _GetInt_EX1_BIT( 12 )
#define _GetInt_EX1_SBM()                   _GetInt_EX1_BIT( 13 )
#define _GetInt_EX1_COMB()                  _GetInt_EX1_BIT( 14 )
#define _GetInt_EX1_TSP2HK()                _GetInt_EX1_BIT( 16 )
#define _GetInt_EX1_VE()                    _GetInt_EX1_BIT( 17 )
#define _GetInt_EX1_CIMAX2MCU()             _GetInt_EX1_BIT( 18 )
#define _GetInt_EX1_DC()                    _GetInt_EX1_BIT( 19 )
#define _GetInt_EX1_GOP()                   _GetInt_EX1_BIT( 20 )
#define _GetInt_EX1_PCM2MCU()               _GetInt_EX1_BIT( 21 )
#define _GetInt_EX1_IIC0()                  _GetInt_EX1_BIT( 22 )
#define _GetInt_EX1_KEYPAD()                _GetInt_EX1_BIT( 24 )
#define _GetInt_EX1_PM()                    _GetInt_EX1_BIT( 25 )
#define _GetInt_EX1_DDC2BI()                _GetInt_EX1_BIT( 26 )
#define _GetInt_EX1_SCM()                   _GetInt_EX1_BIT( 27 )
#define _GetInt_EX1_VBI()                   _GetInt_EX1_BIT( 28 )
#define _GetInt_EX1_MSTAR_LINK()            _GetInt_EX1_BIT( 29 )
#define _GetInt_EX1_MLINK()                 _GetInt_EX1_BIT( 30 )
#define _GetInt_EX1_ADCDVI2RIU()            _GetInt_EX1_BIT( 31 )
#define _GetInt_EX1_SVD()                   _GetInt_EX1_EXP_BIT( 32 )
#define _GetInt_EX1_USB1()                  _GetInt_EX1_EXP_BIT( 33 )
#define _GetInt_EX1_UHC1()                  _GetInt_EX1_EXP_BIT( 34 )
#define _GetInt_EX1_MIU()                   _GetInt_EX1_EXP_BIT( 35 )

#define _SetInt_EX1_BIT( IntSrc, bEnable ) \
    MDrv_WriteRegBit(EX1_INT_MASK + (IntSrc / 8), !(bEnable), 1 << (IntSrc & 0x07))
#define _SetInt_EX1_EXP_BIT( IntSrc, bEnable ) \
    MDrv_WriteRegBit(REG_IRQ_EXP_MASK + ((IntSrc) - 32) / 8, !(bEnable), 1 << (IntSrc & 0x07))

#define _SetInt_EX1_UART( bEnable )         _SetInt_EX1_BIT(  0, bEnable )
#define _SetInt_EX1_HOST_DMA0( bEnable )    _SetInt_EX1_BIT(  1, bEnable )
#define _SetInt_EX1_HOST_DMA1( bEnable )    _SetInt_EX1_BIT(  2, bEnable )
#define _SetInt_EX1_MVD( bEnable )          _SetInt_EX1_BIT(  3, bEnable )
#define _SetInt_EX1_PS( bEnable )           _SetInt_EX1_BIT(  4, bEnable )
#define _SetInt_EX1_NFIE( bEnable )         _SetInt_EX1_BIT(  5, bEnable )
#define _SetInt_EX1_USB( bEnable )          _SetInt_EX1_BIT(  6, bEnable )
#define _SetInt_EX1_UHC( bEnable )          _SetInt_EX1_BIT(  7, bEnable )
#define _SetInt_EX1_EMAC( bEnable )         _SetInt_EX1_BIT(  9, bEnable )
#define _SetInt_EX1_DISP( bEnable )         _SetInt_EX1_BIT( 10, bEnable )
#define _SetInt_EX1_DHC( bEnable )          _SetInt_EX1_BIT( 11, bEnable )
#define _SetInt_EX1_PM_SLEEP( bEnable )     _SetInt_EX1_BIT( 12, bEnable )
#define _SetInt_EX1_SBM( bEnable )          _SetInt_EX1_BIT( 13, bEnable )
#define _SetInt_EX1_COMB( bEnable )         _SetInt_EX1_BIT( 14, bEnable )
#define _SetInt_EX1_TSP2HK( bEnable )       _SetInt_EX1_BIT( 16, bEnable )
#define _SetInt_EX1_VE( bEnable )           _SetInt_EX1_BIT( 17, bEnable )
#define _SetInt_EX1_CIMAX2MCU( bEnable )    _SetInt_EX1_BIT( 18, bEnable )
#define _SetInt_EX1_DC( bEnable )           _SetInt_EX1_BIT( 19, bEnable )
#define _SetInt_EX1_GOP( bEnable )          _SetInt_EX1_BIT( 20, bEnable )
#define _SetInt_EX1_PCM2MCU( bEnable )      _SetInt_EX1_BIT( 21, bEnable )
#define _SetInt_EX1_IIC0( bEnable )         _SetInt_EX1_BIT( 22, bEnable )
#define _SetInt_EX1_KEYPAD( bEnable )       _SetInt_EX1_BIT( 24, bEnable )
#define _SetInt_EX1_PM( bEnable )           _SetInt_EX1_BIT( 25, bEnable )
#define _SetInt_EX1_DDC2BI( bEnable )       _SetInt_EX1_BIT( 26, bEnable )
#define _SetInt_EX1_SCM( bEnable )          _SetInt_EX1_BIT( 27, bEnable )
#define _SetInt_EX1_VBI( bEnable )          _SetInt_EX1_BIT( 28, bEnable )
#define _SetInt_EX1_MSTAR_LINK( bEnable )   _SetInt_EX1_BIT( 29, bEnable )
#define _SetInt_EX1_MLINK( bEnable )        _SetInt_EX1_BIT( 30, bEnable )
#define _SetInt_EX1_ADCDVI2RIU( bEnable )   _SetInt_EX1_BIT( 31, bEnable )
#define _SetInt_EX1_SVD( bEnable )          _SetInt_EX1_EXP_BIT( 32, bEnable )
#define _SetInt_EX1_USB1( bEnable )         _SetInt_EX1_EXP_BIT( 33, bEnable )
#define _SetInt_EX1_UHC1( bEnable )         _SetInt_EX1_EXP_BIT( 34, bEnable )
#define _SetInt_EX1_MIU( bEnable )          _SetInt_EX1_EXP_BIT( 35, bEnable )

// only disable is allowed
#define _SetInt_ALL( bEnable )                  \
    do {                                        \
        if ( !(bEnable) )                         \
        {                                       \
            IEN0 = 0;                           \
            IEN1 = 0;                           \
            IEN2 = 0;                           \
            XBYTE[EX0_INT_MASK + 0] = 0xFF;     \
            XBYTE[EX0_INT_MASK + 1] = 0xFF;     \
            XBYTE[EX0_INT_MASK + 2] = 0xFF;     \
            XBYTE[EX0_INT_MASK + 3] = 0xFF;     \
            XBYTE[EX1_INT_MASK + 0] = 0xFF;     \
            XBYTE[EX1_INT_MASK + 1] = 0xFF;     \
            XBYTE[EX1_INT_MASK + 2] = 0xFF;     \
            XBYTE[EX1_INT_MASK + 3] = 0xFF;     \
            XBYTE[REG_FIQ_EXP_MASK + 0] = 0xFF; \
            XBYTE[REG_FIQ_EXP_MASK + 1] = 0xFF; \
            XBYTE[REG_FIQ_EXP_MASK + 2] = 0xFF; \
            XBYTE[REG_FIQ_EXP_MASK + 3] = 0xFF; \
            XBYTE[REG_IRQ_EXP_MASK + 0] = 0xFF; \
            XBYTE[REG_IRQ_EXP_MASK + 1] = 0xFF; \
            XBYTE[REG_IRQ_EXP_MASK + 2] = 0xFF; \
            XBYTE[REG_IRQ_EXP_MASK + 3] = 0xFF; \
        }                                       \
    } while (0)

#define MDrv_Sys_SetInterrupt( IntSrc, bEnable )    _SetInt_##IntSrc( bEnable )
#define MDrv_Sys_GetInterrupt( IntSrc )             _GetInt_##IntSrc()

#define SUBTITIE_OFFSET             0x10000

////////////////////////////////////////////////////////////////////////////////
// prototype for putcharUARTn
////////////////////////////////////////////////////////////////////////////////

#if ( ENABLE_UART0 )
DRVSYS_EX char putcharUART0( char c );
#endif

#if ( ENABLE_UART1 )
DRVSYS_EX char putcharUART1( char c );
#endif

DRVSYS_EX void puthex( u16 u16HexVal );


////////////////////////////////////////////////////////////////////////////////
// PIU Watch Dog Timer
////////////////////////////////////////////////////////////////////////////////

#if ( WATCH_DOG == ENABLE )

// Disable watchdog is not suggested at any time, please ask related people first
// 1. During firmware upgrade, disable WDT before erase flash
// 2. In factory menu, WDT on/off option
#define MDrv_Sys_DisableWatchDog()  __ASSERT(0)

#else

#define MDrv_Sys_DisableWatchDog()                                      \
    do {                                                                \
        /* puts("Disable WDT %s %d\n", __FILE__, (int)__LINE__); */     \
        XBYTE[MCU_WDT_KEY_L] = 0x55;                                    \
        XBYTE[MCU_WDT_KEY_H] = 0xAA;                                    \
    } while (0)

#endif

// This is not really necessary, WDT is enabled after MCU reset
#define MDrv_Sys_EnableWatchDog()                                       \
    do {                                                                \
        XBYTE[MCU_WDT_KEY_L] = 0;                                       \
        XBYTE[MCU_WDT_KEY_H] = 0;                                       \
    } while (0)

#define MDrv_Sys_IsWatchDogEnabled()                                    \
    (XBYTE[MCU_WDT_KEY_L] != 0x55 || XBYTE[MCU_WDT_KEY_H] != 0xAA)

#define MDrv_Sys_IsResetByWatchDog()    ( XBYTE[WDT_RST] & BIT0 )

#define MDrv_Sys_ClearWatchDog()        ( XBYTE[WDT_RST] = BIT1 )

#define MDrv_Sys_ClearWatchDogStatus()  ( XBYTE[WDT_RST] = BIT2 )

// Use 0xFFFF instead of 0x10000 to allow sec=0 and approximate ceil()
#define MDrv_Sys_WatchDogCycles( sec ) ( 0xFFFF - (((sec) * MST_XTAL_CLOCK_HZ) >> 16) )

// Internal counter will be updated to new value after set
// To reduce the chance of failure, please don't use variable

#define MDrv_Sys_SetWatchDogTimer( sec )                                \
    do {                                                                \
        /* __ASSERT( (sec) >= 0 ); */                                   \
        /* __ASSERT( (sec) <= (0xFFFF0000 / MST_XTAL_CLOCK_HZ) ); */    \
        XBYTE[WDT_TIMER_0] = LOBYTE( MDrv_Sys_WatchDogCycles( sec ) );  \
        XBYTE[WDT_TIMER_1] = HIBYTE( MDrv_Sys_WatchDogCycles( sec ) );  \
    } while (0)

////////////////////////////////////////////////////////////////////////////////
// Uart mux selection (see UART_SEL_TYPE in hwreg_S4L.h for mux)
////////////////////////////////////////////////////////////////////////////////

#define MDrv_Sys_SetUart0Mux(mux)   MDrv_WriteByteMask( 0x1EAA, ((mux) << 2), BITMASK( 4:2 ) )
#define MDrv_Sys_SetUart1Mux(mux)   MDrv_WriteByteMask( 0x1EAA, ((mux) << 5), BITMASK( 7:5 ) )
#define MDrv_Sys_SetUart2Mux(mux)   MDrv_WriteByteMask( 0x1EAB, ((mux) << 0), BITMASK( 2:0 ) )

////////////////////////////////////////////////////////////////////////////////

DRVSYS_EX u32 u32XDataAddr;
DRVSYS_EX u32 u32XDataSize;

#define TS_FLAG_SERIAL      0x01
#define TS_FLAG_CLKINV      0x02

DRVSYS_EX u32 u32MvdBsAddr;
DRVSYS_EX u32 u32MvdBsSize;
DRVSYS_EX u32 u32MvdFbAddr;
DRVSYS_EX u32 u32MvdFbSize;
DRVSYS_EX u32 u32SecbufAddr;
DRVSYS_EX u32 u32SecbufSize;

DRVSYS_EX u32 u32OadbufAddr;
DRVSYS_EX u32 u32OadbufSize;
#if 0//((COUNTRY == KOREA) && (ENABLE_OTA))
DRVSYS_EX u32 gu32OADImageBufferAddr;
DRVSYS_EX u32 gu32OADImageBufferLen;
DRVSYS_EX u32 u32OADDownloadWin1Addr;
DRVSYS_EX u32 u32OADDownloadWin1Len;
#endif

DRVSYS_EX u32 u32TtxVbiAddr;
DRVSYS_EX u32 u32TtxVbiSize;
DRVSYS_EX u32 u32SubtitleAddr;
DRVSYS_EX u32 u32SubtitleSize;

#ifndef ATSC_SYSTEM // DVB
DRVSYS_EX u32 u32TtxVbiAddr;
DRVSYS_EX u32 u32TtxVbiSize;
DRVSYS_EX u32 u32SubtitleAddr;
DRVSYS_EX u32 u32SubtitleSize;
#endif

DRVSYS_EX u8  u8Sec1KFilterNum;
DRVSYS_EX u8  u8Sec4KFilterNum;
DRVSYS_EX u8  u8RecordFilterNum;
DRVSYS_EX u32 u32Sec1KFilterBufferSize;
DRVSYS_EX u32 u32Sec4KFilterBufferSize;

DRVSYS_EX u8 u8ChipVer;
DRVSYS_EX u8 u8ChipRev;

//DRVSYS_EX void MDrv_Sys_InitUartForXtalClk( char cOutputChar ) small;
DRVSYS_EX void MDrv_Sys_Devices_Init( void );
//DRVSYS_EX bit  MDrv_Sys_IsCodeInSPI( void );
//DRVSYS_EX void MDrv_Sys_RunCodeInMIU( u16 u16MIUAddrInBank, u8 u8NumBanks, BOOLEAN bBypassCache );
DRVSYS_EX void MDrv_Sys_RunCodeInSPI( void );
DRVSYS_EX void MDrv_Sys_WatchDogEnable(u8 mode);
DRVSYS_EX void MDrv_Sys_Set_ECC_DMA(u8 mode);

#if 0//(BOOTLOADER_SYSTEM)
DRVSYS_EX void MDrv_Sys_Reboot( void );
#endif

#if 0//( KEEP_UNUSED_FUNC == 1 )
DRVSYS_EX void MDrv_Sys_ResetAeon( BOOLEAN bReset );
#endif

////////////////////////////////////////////////////////////////////////////////
// XDATA Extension related
////////////////////////////////////////////////////////////////////////////////

// pXDataExtVar
DRVSYS_EX u32 XDEXT2PHY(u32 u32SrcAddr);

DRVSYS_EX u8  MDrv_Sys_SetXdataWindow1Base(u16 u16XDWin1BaseIn4Kytes);

#define XDATA_WINDOW_USE_PUSE_POP 1
#if XDATA_WINDOW_USE_PUSE_POP
DRVSYS_EX u8  MDrv_Sys_PushXdataWindow1Base(void);
DRVSYS_EX u8  MDrv_Sys_PopXdataWindow1Base(void);
#endif

DRVSYS_EX u8  MDrv_Sys_ReleaseXdataWindow1(void);
DRVSYS_EX u16 MDrv_Sys_GetXdataWindow1Base(void);

// use "void *" as generic pointer type
DRVSYS_EX void MDrv_Sys_CopyMIU2VAR(u32 srcaddr, void *dstaddr, u16 len);
DRVSYS_EX void MDrv_Sys_CopyVAR2MIU(void *srcaddr, u32 dstaddr, u16 len);

DRVSYS_EX void MDrv_Sys_UartControl(u8 bEnable);
DRVSYS_EX void AEON_StartAddress_Assignment(u8 Select, u32 CodeAddress, u32 DataAddress);

#if 0//(XMODEM_DWNLD_ENABLE)
    DRVSYS_EX void MDrv_Sys_XmodemUartInit( void );
#endif

#undef DRVSYS_EX
#endif // _DRVSYS_H_

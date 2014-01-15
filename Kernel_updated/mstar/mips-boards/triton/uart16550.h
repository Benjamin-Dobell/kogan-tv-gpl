/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _uart16550_h_
#define _uart16550_h_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define REG_UART0_BASE              0xBF801300
#define REG_UART1_BASE              0xBF801300
#define REG_UART2_BASE              0xBF801300

#define UART0_REG8(addr)            *((volatile unsigned int*)(REG_UART0_BASE + ((addr)<< 2)))
#define UART1_REG8(addr)            *((volatile unsigned int*)(REG_UART1_BASE + ((addr)<< 2)))
#define UART2_REG8(addr)            *((volatile unsigned int*)(REG_UART2_BASE + ((addr)<< 2)))


//
// UART Register List
//
#define UART_DL                     0           // Divisor Latch
#define UART_DL1_LSB                0           // Divisor Latch Low
#define UART_DL2_MSB                0           // Divisor Latch High
#define UART_FIFO                   1           // Divisor Latch Low
#define UART_IER                    1           // Interrupt Enable Register
#define UART_FCR                    2           // FIFO Control Register
#define UART_LCR                    2           // Line Control Register
#define UART_LSR                    3           // Line Status Register
#define UART_RB                     3           // optional: set rf_pop delay for memack ; [3:0] rf_pop_delay; [6] rf_pop_mode_sel ; [7] reg_rb_read_ack

// UART_IER(3)
// Interrupt Enable Register
#define UART_IER_MASK               0xFF00
#define UART_IER_RDA                0x0100      // Enable receiver data available interrupt
#define UART_IER_THRE               0x0200      // Enable Transmitter holding reg empty int

// UART_FCR(4)
// FIFO Control Register (16650 only)
#define UART_FCR_MASK               0x00FF
#define UART_FCR_RXFIFO_CLR         0x0001
#define UART_FCR_TXFIFO_CLR         0x0002
#define UART_FCR_TRIGGER_0		    0x0000
#define UART_FCR_TRIGGER_1		    0x0010
#define UART_FCR_TRIGGER_2		    0x0020
#define UART_FCR_TRIGGER_3		    0x0030
#define UART_FCR_TRIGGER_4		    0x0040
#define UART_FCR_TRIGGER_5		    0x0050
#define UART_FCR_TRIGGER_6		    0x0060
#define UART_FCR_TRIGGER_7		    0x0070


// UART_LCR(5)
// Line Control Register
// Note: if the word length is 5 bits (UART_LCR_WLEN5), then setting
// UART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
#define UART_LCR_MASK               0xFF00
#define UART_LCR_CHAR_BITS_5        0x0000      // Wordlength: 5 bits
#define UART_LCR_CHAR_BITS_6        0x0100      // Wordlength: 6 bits
#define UART_LCR_CHAR_BITS_7        0x0200      // Wordlength: 7 bits
#define UART_LCR_CHAR_BITS_8        0x0300      // Wordlength: 8 bits
#define UART_LCR_STOP_BITS_1        0x0000      // 1 bit
#define UART_LCR_STOP_BITS_2        0x0400      // 1.5, 2 bit
#define UART_LCR_PARITY_EN          0x0800      // Parity Enable
#define UART_LCR_EVEN_PARITY_SEL    0x1000      // Even parity select
#define UART_LCR_DIVISOR_EN         0x8000      // Divisor latch access bit

// UART_LSR(6)
// Line Status Register
#define UART_LSR_DR			        0x0001      // Receiver data ready
#define UART_LSR_TXFIFO_EMPTY       0x0020      // Transmitter FIFO empty
#define UART_LSR_TX_EMPTY           0x0040      //
#define UART_LSR_TXFIFO_FULL        0x0080      //

#define UART_RB_MASK                0xFF00

// Baud rate
#define UART_BAUD_RATE	            115200 //9600 //19200 //38400 //57600 //115200 =>too frequent Rx INT
// Clock source
#define UART_CLK_FREQ               123000000    //14318180    //43200000 //14318180
// Divisor
#define UART_DIVISOR                (UART_CLK_FREQ/(16 * UART_BAUD_RATE))

#define UART_REG8(addr)             UART0_REG8(addr)
#define UART_IRQ                    E_IRQ_UART

typedef unsigned char uint8;
typedef unsigned int  uint32;
typedef unsigned char u8;

void Uart16550Init(uint32 baud, uint8 data, uint8 parity, uint8 stop);

/* blocking call */
uint8 Uart16550GetPoll(void);

void Uart16550Put(uint8 byte);

#endif

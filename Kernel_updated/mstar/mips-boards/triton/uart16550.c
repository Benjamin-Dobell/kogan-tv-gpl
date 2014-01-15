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


#include "uart16550.h"


void Uart16550Init(uint32 baud, uint8 data, uint8 parity, uint8 stop)
{

    // set UART clock again in case you need to load code from ICE
	// Setup UART clk to 123MHZ(for higher bandrate)
	*(volatile uint32*)(0xbf800000+(0x3c8c)) = 0x0010 ;
	*(volatile uint32*)(0xbf800000+(0x3c78)) = 0x01C0 ;

    *(volatile uint32*)(0xbf800000+(0x0F55*4)) &= ~0xFF;
    *(volatile uint32*)(0xbf800000+(0x0F55*4)) |= 0x14;

    //enable reg_uart_rx_enable;
    *(volatile uint32*)(0xbf800000+(0x0F01*4)) |= (1<<10);//BIT10;

	#ifdef CONFIG_Triton
    UART_REG8(8) = 0x10;
	#endif

    //Disable all interrupts
    UART_REG8(UART_IER) &= UART_IER_MASK;

    //Reset receiver and transmiter
    UART_REG8(UART_FCR) &= UART_FCR_MASK;
    UART_REG8(UART_FCR) |= UART_FCR_RXFIFO_CLR | UART_FCR_TXFIFO_CLR | UART_FCR_TRIGGER_0;

    //set character to 8 bits, 1 stop bit, no parity
    UART_REG8(UART_LCR) &= UART_LCR_MASK;
    UART_REG8(UART_LCR) |= UART_LCR_CHAR_BITS_8 | UART_LCR_STOP_BITS_1;

    //set boud rate : disivor = MCU_CLK / ( 16 * boud rate )
    UART_REG8(UART_LCR) |= UART_LCR_DIVISOR_EN;             //Divisor Latch Access enable
    //UART_REG8(UART_DL2_MSB) = (UART_DIVISOR >> 8) & 0x00ff;	    //MSB (write first)
    //UART_REG8(UART_DL1_LSB) = UART_DIVISOR & 0x00ff;		    //LSB (write last)
    //data = UART_DIVISOR;//debug
    UART_REG8(UART_DL) = UART_DIVISOR;
    data = UART_REG8(UART_DL);

    UART_REG8(UART_LCR) &= ~UART_LCR_DIVISOR_EN;            //Dividor Latch Access disable

    //Enable interrupts for RX
    UART_REG8(UART_IER) |= UART_IER_RDA;
}

uint8 Uart16550GetPoll(void)
{
    u8 u8Ch;

    while ( (UART_REG8(UART_LSR)&UART_LSR_DR) != UART_LSR_DR);  //data ready in rx FIFO???

    u8Ch = (u8)(UART_REG8(UART_FIFO));

    UART_REG8(UART_RB)|=0x8000;

    return u8Ch;
}


void Uart16550Put(uint8 byte)
{
    while( ( UART_REG8(UART_LSR)&UART_LSR_TXFIFO_FULL) == UART_LSR_TXFIFO_FULL);  //data full in tx FIFO???

    //CAUTION : Need to sync the Rx and Tx INT config in the MDrv_UART_Init()
    UART_REG8(UART_FIFO) = UART_IER_RDA | byte;	//put char
}

void Uart16550IntrruptEnable(void)
{

#if 1
    Uart16550Init(1,2,3,4);
#else
    //Disable all interrupts
    UART_REG8(UART_IER) &= UART_IER_MASK;

    //Reset receiver and transmiter
    UART_REG8(UART_FCR) &= UART_FCR_MASK;
    UART_REG8(UART_FCR) |= UART_FCR_RXFIFO_CLR | UART_FCR_TXFIFO_CLR | UART_FCR_TRIGGER_0;

    //Enable interrupts for RX
    UART_REG8(UART_IER) |= UART_IER_RDA;
#endif
}

uint8 Uart16550IntrRead(void)
{
    return(Uart16550GetPoll());

}


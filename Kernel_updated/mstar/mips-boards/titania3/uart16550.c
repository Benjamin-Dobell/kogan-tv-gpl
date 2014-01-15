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

static void uart_mode_set(uint8 port, uint8 mode)
{
	//reg_firstuartmode
	if( port == 0 )
	{
		UNUSED(mode);
	}

	//reg_seconduartmode
	if( port == 1 )
	{
		TOP_REG(0x0002) |= (mode << 8);
	}
	//reg_thirduartmode
	if( port == 2 )
	{
		TOP_REG(0x0002) |= (mode << 10);
	}
	//reg_fourthuartmode
	if( port == 3 )
	{
		TOP_REG(0x0002) |= (mode << 6);
	}
	//reg_fastuartmode
	if( port == 4 )
	{
		TOP_REG(0x0002) |= (mode << 4);
	}
}

static uint32 uart_clkgen_set(uint32 uartClkgen)//@@@ update later
{
#define REG_CLK_BASE                0xBF201600
#define CLK_REG(addr)               (*((volatile unsigned int*)(REG_CLK_BASE + ((addr)<<2))))

#if defined (REG_UART0_BASE)
	// Default init in sboot
#endif

#if defined (REG_UART1_BASE)//@@@review latter
	//switch uart clk on
	CLK_REG(0x0013)|=0x0002;
	//set default clk as 123M hz
	CLK_REG(0x0014)|=0x000C;	
#endif

#if defined (REG_UART2_BASE)//@@@review latter
	//switch uart clk on
	CLK_REG(0x0013)|=0x0004;
	//set default clk as 123M hz
	CLK_REG(0x0014)|=0x0C00;	
#endif

	return uartClkgen;
}

static void Uart_Init(uart_dev_t *uart)
{
	uint32 divisor = 0;
	
	// Disable all interrupts
    UART_REG8(UART_IER) = 0x00;

    // card_reset_reg must set to 1 for rx
    UART_REG8(8 * 2) = 0x10;

    // Reset receiver and transmiter
    UART_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_1;

    // Set 8 bit char, 1 stop bit, no parity
    UART_REG8(UART_LCR) = UART_LCR_WLEN8 & ~(UART_LCR_STOP2 | UART_LCR_PARITY);

	// Set baud rate
	
	//while(!(UART_REG8(UART_USR)&UART_USR_BUSY));

	uart_mode_set(uart->port, uart->pinConfig);
    divisor = uart_clkgen_set(uart->uartclk) / (16 * (uart->params_t->baudRate));
    UART_REG8(UART_LCR) |= UART_LCR_DLAB;
    UART_REG8(UART_DLL) = divisor & 0xFF;
    UART_REG8(UART_DLM) = (divisor >> 8) & 0xFF;
    UART_REG8(UART_LCR) &= ~(UART_LCR_DLAB);
}

void Uart16550Init(uint32 baud, uint8 data, uint8 parity, uint8 stop)
{
	static uart_params_t params_t;
	uart_dev_t uart_dev[UART_DEV_MAX];
	
	params_t.baudRate = baud;
	params_t.dataBits = data;
	params_t.partiy   = parity;
	params_t.stopBits = stop;

	// set UART clock again in case you need to load code from ICE
	// Setup UART clk to 123MHZ(for higher bandrate)
	*(volatile uint32*)(0xbf000000+(0x0709*4)) |= 0x0800;//bit 12 UART Enable, bit 11 UART RX Enable

    #if defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10)
    // TITANIA3
    // ------->
	// Switch FUART to UART mode not dma node
	*(volatile uint32*)(0xbf200000+(0x1A60*4)) &= ~0x0002;
	
    // Switch UART0 to PIU UART 0;UART2 to PIU UART 1;UART3 to PIU Fast UART
    *(volatile uint32*)(0xbf200000+(0x0F53*4)) &= ~0xFFFF;          // Clear UART_SEL0
    *(volatile uint32*)(0xbf200000+(0x0F53*4)) |= 0x7514;           // T3 UART_SEL0 --> PIU UART0;UART_SEL3 --> PIU FUART 
	*(volatile uint32*)(0xbf200000+(0x0F54*4)) &= ~0x000F;
	// <-------	

    // switch UART to RS232
    #ifdef DEFAULT_UART_RS232
    *(volatile unsigned int*)(0xbf200000+(0x0F53*4)) |= 0x0520;      //UART_SEL2-->PIU UART0
    *(volatile unsigned int*)(0xbf200000+(0x0F02*4)) &= ~0x0800;    //RX1/TX1 use PAD_GPIO1/GPIO_2
    *(volatile unsigned int*)(0xbf200000+(0x0F02*4)) |= 0x0400;
    #endif
    //enable reg_uart_rx_enable;
    *(volatile uint32*)(0xbf200000+(0x0F01*4)) |= (1<<10);//BIT10;
    #endif

	#if defined (REG_UART0_BASE)
		uart_dev[0].port    = 0;
		uart_dev[0].sel     = 4;
		uart_dev[0].iobase  = REG_UART0_BASE;
		uart_dev[0].uartclk = UART_CLK_FREQ;
		uart_dev[0].params_t= &params_t;
		Uart_Init(&uart_dev[0]);
	#endif

	#if defined (REG_UART1_BASE)
		uart_dev[1].port     = 2;//@@@check point
		uart_dev[1].pinConfig= 2;//@@@check point
		uart_dev[1].sel      = 5;
		uart_dev[1].iobase   = REG_UART1_BASE;
		uart_dev[1].uartclk  = UART_CLK_FREQ;
		uart_dev[1].params_t = &params_t;
		Uart_Init(&uart_dev[1]);
	#endif	
	
	#if defined (REG_UART2_BASE)
		uart_dev[2].port     = 3;//@@@check point
		uart_dev[2].pinConfig= 3;//@@@check point
		uart_dev[2].sel      = 7;
		uart_dev[2].iobase   = REG_UART2_BASE;
		uart_dev[2].uartclk  = UART_CLK_FREQ;
		uart_dev[2].params_t = &params_t;
		Uart_Init(&uart_dev[2]);
	#endif	

    //enable UART Rx
    TOP_REG(REG_TOP_UTMI_UART_SETHL) |= TOP_UART_RX_EN;
}
#if 0
void Uart16550Put(uint8 byte)
{
    unsigned char u8Reg ;
    unsigned char* a1 = 0xA0100000 ;
    unsigned char* a2 = 0xA0100004 ;
    unsigned int i = 0 ;

	                            // Transmitter empty
    do {
        u8Reg = UART_REG8(UART_LSR);
        if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
        {
            break;
}
        *a1 = i++ ;
        //MsOS_YieldTask(); //trapped if called by eCos DSR
    }while(1);//(!UART_EXPIRE(u32Timer));

    UART_REG8(UART_TX) = byte;	//put char

    //Wait for both Transmitter empty & Transmit-hold-register empty
    do {
        u8Reg = UART_REG8(UART_LSR);
        if ((u8Reg & BOTH_EMPTY) == BOTH_EMPTY)
        {
            break;
        }
        *a2 = i++ ;
        //MsOS_YieldTask(); //trapped if called by eCos DSR
    }while(1);//(!UART_EXPIRE(u32Timer));
}
#endif

void Uart16550IntrruptEnable(void)
{
    Uart16550Init(UART_BAUD_RATE,2,3,4);
}

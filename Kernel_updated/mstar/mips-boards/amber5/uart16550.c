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

    #if defined(CONFIG_MSTAR_AMBER5)
     *(volatile uint32*)(0xbf000000+(0x0709*4)) |= 0x0800;//bit 12 UART Enable, bit 11 UART RX Enable
    #endif
/*
    *(volatile uint32*)(0xbf200000+(0x0F53*4)) = 0x2104;    // 0010 0001 0000 0100 => SEL0 : PIU UART0
    *(volatile uint32*)(0xbf200000+(0x04c6*4)) = 0x0083;

    *(volatile uint32*)(0xbf200000+(0x04c2*4)) = 0;//(divisor & 0xFF00)>>8); //0x0000; //MSB of divisor
    *(volatile uint32*)(0xbf200000+(0x04c0*4)) = 0x13;//(divisor & 0x00FF); //0x0004;  //LSB of divisor

    *(volatile uint32*)(0xbf200000+(0x04c6*4)) = 0x0003;
    *(volatile uint32*)(0xbf200000+(0x04c4*4)) = 0x0007;
*/
#if 1

#if defined(CONFIG_MSTAR_TITANIA2)
    // 123Mhz
   	//*(volatile uint32*)(0xbf800000+(0x3c8c)) = 0x0008 ;
    //*(volatile uint32*)(0xbf800000+(0x3c78)) = 0x01C0 ;
    // 144Mhz
   	//*(volatile uint32*)(0xbf800000+(0x3c8c)) = 0x0008 ;
    //*(volatile uint32*)(0xbf800000+(0x3c78)) = 0x0180 ;
    // 160Mhz
   	//*(volatile uint32*)(0xbf800000+(0x3c8c)) = 0x0008 ;
    //*(volatile uint32*)(0xbf800000+(0x3c78)) = 0x0140 ;
    // 170Mhz
   	*(volatile uint32*)(0xbf800000+(0x3c8c)) = 0x0008;
    *(volatile uint32*)(0xbf800000+(0x3c78)) = 0x0100;
    //#endif
#else
    // 12Mhz XTAL
    #if defined(CONFIG_MSTAR_TITANIA2)
	*(volatile uint32*)(0xbf800000+(0x3c8c)) = 0x0000 ; // use XTAL         //F23
	*(volatile uint32*)(0xbf800000+(0x3c78)) = 0x0100 ; // 12MHZ            //F1E [1 0000 0000]
	*(volatile uint32*)(0xbf800000+(0x3c8c)) |= 0x0004 ; // 12MHZ (uart_4)  //F23 [ 0100 ]
	#endif
#endif


    #if defined(CONFIG_MSTAR_AMBER5)
    // ------->
    // Junyou.Lin 20090203 Switch UART0 to PIU UART 0
    *(volatile uint32*)(0xbf200000+(0x0F53*4)) &= ~0xFFFF;          // Clear UART_SEL0
    *(volatile uint32*)(0xbf200000+(0x0F53*4)) |= 0x0004;           // T2 UART_SEL0 --> PIU UART0
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



    #if defined(CONFIG_MSTAR_TITANIA2)
    *(volatile uint32*)(0xbf800000+(0x0F55*4)) &= ~0xFF;
    *(volatile uint32*)(0xbf800000+(0x0F55*4)) |= 0x14;

    //def DEFAULT_UART_RS232
    // switch UART to RS232
    #ifdef DEFAULT_UART_RS232
    *(volatile unsigned int*)(0xbf800000+(0x0F55*4)) = 0x0520;
    *(volatile unsigned int*)(0xbf800000+(0x0F02*4)) &= ~0x0800;
    *(volatile unsigned int*)(0xbf800000+(0x0F02*4)) |= 0x0400;
    #endif

    //enable reg_uart_rx_enable;
    *(volatile uint32*)(0xbf800000+(0x0F01*4)) |= (1<<10);	//BIT10;
    #endif

    UART_REG8(8) = 0x10;

//------------------------------------------
    //Disable all interrupts 							        //Interrupt Enable Register
    UART_REG8(UART_IER) = 0x00;
#define UART_SCCR                   8                                   // Smartcard Control Register
#define UART_SCCR_RST               0x10                                // Smartcard reset 0->1, UART Rx enable 1
    UART_REG8(UART_SCCR) = UART_SCCR_RST;                       // Receive enable & reset

    //UART_REG8(UART_MCR) = 0x20; // New Baud Rate Mode

    //Reset receiver and transmiter 							//FIFO Control Register
    UART_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_1 | UART_FCR_TRIGGER_MASK;

#define UART_LCR_WLEN8		        0x03	                            // Wordlength: 8 bits
#define UART_LCR_STOP1		        0x00	                            //
#define UART_LCR_PARITY		        0x08	                            // Parity Enable
    //Set 8 bit char, 1 stop bit, no parity 					//Line Control Register
    UART_REG8(UART_LCR) = (UART_LCR_WLEN8 | UART_LCR_STOP1) & (~UART_LCR_PARITY);

#define UART_DLL	                0	                                // Out: Divisor Latch Low (DLAB=1)
#define UART_DLM	                1                                // Out: Divisor Latch High (DLAB=1)
#define UART_LCR_DLAB		        0x80	                            // Divisor latch access bit
    //Set baud rate
    UART_REG8(UART_LCR) |= UART_LCR_DLAB;						//Line Control Register
    UART_REG8(UART_DLM) = (UART_DIVISOR >> 8) & 0x00ff;	            //Divisor Latch High
    UART_REG8(UART_DLL) = UART_DIVISOR & 0x00ff;			            //Divisor Latch Low
    UART_REG8(UART_LCR) &= ~(UART_LCR_DLAB);					//Line Control Register

#define UART_IER_RDI		        0x01	                            // Enable receiver data available interrupt
    //Enable receiver data available interrupt
    UART_REG8(UART_IER) = UART_IER_RDI;

#define BIT(bits)                   (1<<bits)
#define TOP_UART_RX_EN                      BIT(10)
#define REG_TOP_UTMI_UART_SETHL     0x0001



#if defined(CONFIG_MSTAR_AMBER5)
#define REG_TOP_BASE                0xBF203C00
#endif

#define TOP_REG(addr)               (*((volatile unsigned int*)(REG_TOP_BASE + ((addr)<<2))))

    //enable UART Rx
    TOP_REG(REG_TOP_UTMI_UART_SETHL) |= TOP_UART_RX_EN;

#endif
}
#if 0
void Uart16550Put(uint8 byte)
{
    unsigned char u8Reg ;
    unsigned char* a1 = 0xA0100000 ;
    unsigned char* a2 = 0xA0100004 ;
    unsigned int i = 0 ;

#define UART_LSR_THRE		        0x20	                            // Transmit-hold-register empty
#define UART_LSR_TEMT		        0x40	                            // Transmitter empty
    do {
        u8Reg = UART_REG8(UART_LSR);
        if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
        {
            break;
}
        *a1 = i++ ;
        //MsOS_YieldTask(); //trapped if called by eCos DSR
    }while(1);//(!UART_EXPIRE(u32Timer));

#define UART_TX     0
    UART_REG8(UART_TX) = byte;	//put char

#define BOTH_EMPTY                  (UART_LSR_TEMT | UART_LSR_THRE)
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
    Uart16550Init(1,2,3,4);
}

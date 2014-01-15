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

/* === CONFIG === */

#define         BASE                    0xBF808C80
#define         MAX_BAUD                1152000
#define		REG_OFFSET		4

/* === END OF CONFIG === */

/* register offset */
#define         OFS_RCV_BUFFER          (0*REG_OFFSET)
#define         OFS_TRANS_HOLD          (0*REG_OFFSET)
#define         OFS_SEND_BUFFER         (0*REG_OFFSET)
#define         OFS_INTR_ENABLE         (1*REG_OFFSET)
#define         OFS_INTR_ID             (2*REG_OFFSET)
#define         OFS_UART_FCR            (2*REG_OFFSET)
#define         OFS_DATA_FORMAT         (3*REG_OFFSET)
#define         OFS_LINE_CONTROL        (3*REG_OFFSET)
#define         OFS_MODEM_CONTROL       (4*REG_OFFSET)
#define         OFS_RS232_OUTPUT        (4*REG_OFFSET)
#define         OFS_LINE_STATUS         (5*REG_OFFSET)
#define         OFS_MODEM_STATUS        (6*REG_OFFSET)
#define         OFS_RS232_INPUT         (6*REG_OFFSET)
#define         OFS_SCRATCH_PAD         (7*REG_OFFSET)
#define         OFS_CTRL                (8*REG_OFFSET)
#define         OFS_DIVISOR_LSB         (0*REG_OFFSET)
#define         OFS_DIVISOR_MSB         (1*REG_OFFSET)


/* memory-mapped read/write of the port */
#define         UART16550_READ(y)    (*((volatile uint8*)(BASE + y)))
#define         UART16550_WRITE(y, z)  ((*((volatile uint8*)(BASE + y))) = z)

#define UART1_REG8(addr)            *((volatile unsigned int*)(BASE + ((addr)<< 2)))

#define UART_REG8(addr)             UART1_REG8(addr)
#define REG(addr) (*(volatile unsigned int *)(addr))



#define DEBUG_LED (*(unsigned short*)0xb7ffffc0)
#define OutputLED(x)  (DEBUG_LED = x)

void Uart16550Init(uint32 baud, uint8 data, uint8 parity, uint8 stop)
{
    /* disable interrupts */
    UART16550_WRITE(OFS_INTR_ENABLE, 0);

    //UART16550_WRITE(OFS_INTR_ID, 0x07);

    /* set up buad rate */
    {
        uint32 divisor;

        /* set DIAB bit */
        UART16550_WRITE(OFS_LINE_CONTROL, 0x80);

        /* set divisor */
        divisor = 43200000 / (16*baud);
        UART16550_WRITE(OFS_DIVISOR_LSB, divisor & 0xff);
        UART16550_WRITE(OFS_DIVISOR_MSB, (divisor & 0xff00)>>8);

        /* clear DIAB bit */
        UART16550_WRITE(OFS_LINE_CONTROL, 0x00);
    }

    /* set data format */
    UART16550_WRITE(OFS_DATA_FORMAT, data | parity | stop);
}

uint8 Uart16550GetPoll(void)
{
    while((UART16550_READ(OFS_LINE_STATUS) & 0x1) == 0);
    return UART16550_READ(OFS_RCV_BUFFER);
}


void Uart16550Put(uint8 byte)
{
    while ((UART16550_READ(OFS_LINE_STATUS) &0x20) == 0);
    UART16550_WRITE(OFS_SEND_BUFFER, byte);
}

#define UART_BAUD_RATE	            38400 //9600 //19200 //38400 //57600 //115200 =>too frequent Rx INT
// Clock source
#define UART_CLK_FREQ               43200000 //14318180
// Divisor
#define UART_DIVISOR                (UART_CLK_FREQ/(16 * UART_BAUD_RATE))

void Uart16550IntrruptEnable(void)
{

	REG(0xBF803C04) |= 0x400;

	UART16550_WRITE(OFS_LINE_CONTROL, 0x03);
	UART16550_WRITE(OFS_UART_FCR,0x01);
	UART16550_WRITE(OFS_INTR_ENABLE, 1);
	UART16550_WRITE(OFS_CTRL, 0x10); //// Receive enable & reset

}

uint8 Uart16550IntrRead(void)
{
    uint8 u8Reg,u8Ch;
    do {
        u8Reg = UART_REG8(5);//UART_LSR
        if ( (u8Reg & 0x01) == 0x01 )
        {
            break;
        }

    } while (1);

    u8Ch = UART_REG8(0);	//get char
    return u8Ch;

}


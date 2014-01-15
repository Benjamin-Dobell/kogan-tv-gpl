/*
 *  arch/arm/mach-vexpress/include/mach/uncompress.h
 *
 *  Copyright (C) 2003 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/version.h>
#if EXTRAVERSION == 13

#define UART_RX                     (0)  // In:  Receive buffer (DLAB=0)
#define UART_TX                     (0)  // Out: Transmit buffer (DLAB=0)
#define UART_DLL                    (0)  // Out: Divisor Latch Low (DLAB=1)
#define UART_DLM                    (1)  // Out: Divisor Latch High (DLAB=1)
#define UART_IER                    (1)  // Out: Interrupt Enable Register
#define UART_IIR                    (2)  // In:  Interrupt ID Register
#define UART_FCR                    (2)  // Out: FIFO Control Register
#define UART_LCR                    (3)  // Out: Line Control Register
#define UART_LSR                    (5)  // In:  Line Status Register
#define UART_MSR                    (6)  // In:  Modem Status Register
#define UART_USR                    (7)

// UART_LSR(5)
// Line Status Register
#define UART_LSR_DR                 0x01          // Receiver data ready
#define UART_LSR_OE                 0x02          // Overrun error indicator
#define UART_LSR_PE                 0x04          // Parity error indicator
#define UART_LSR_FE                 0x08          // Frame error indicator
#define UART_LSR_BI                 0x10          // Break interrupt indicator
#define UART_LSR_THRE               0x20          // Transmit-hold-register empty
#define UART_LSR_TEMT               0x40          // Transmitter empty

#ifndef CONFIG_MSTAR
#define UART_REG(addr) *((volatile unsigned int*)(0x1F201300 + ((addr)<< 3)))
#else
#define UART_REG(addr) *((volatile unsigned int*)(0x1F220D00 + ((addr)<< 3)))
#endif

static u32 UART16550_READ(u8 addr)
{
        u32 data;

        if (addr>80) //ERROR: Not supported
        {
                return(0);
        }

        data = UART_REG(addr);
        return(data);
}

static void UART16550_WRITE(u8 addr, u8 data)
{
        if (addr>80) //ERROR: Not supported
{
                return;
        }
        UART_REG(addr) = data;
}

static inline unsigned int serial_in(int offset)
{
        return UART16550_READ(offset);
}

static inline void serial_out(int offset, int value)
{
        UART16550_WRITE(offset, value);
}

/*
 * This does not append a newline
 */
static inline void putc(int c)
{
        while ((serial_in(UART_LSR) & UART_LSR_THRE) == 0)
                ;

        serial_out(UART_TX, c);
}

#endif /* EXTRAVERSION */

/*
 * nothing to do
 */
#define flush()
#define arch_decomp_setup()
#define arch_decomp_wdog()


/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */
// #include <linux/config.h>
#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/tty.h>

#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,20)
#include <linux/undefconf.h>
#endif

#ifdef CONFIG_MTD
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#endif

#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>
#include <asm/dma.h>
#include <asm/time.h>
#include <asm/traps.h>

#include "chip_int.h"
#include "hwreg_amethyst.h"

#ifdef CONFIG_VT
#include <linux/console.h>
#endif

#include "board/Board.h"
#include "chip_setup.h"

#ifndef ENABLE
#define ENABLE                      1
#endif

#ifndef DISABLE
#define DISABLE                     0
#endif

#ifndef BIT0
#define BIT0	                    0x00000001
#endif

#ifndef BIT1
#define BIT1	                    0x00000002
#endif

#ifndef BIT2
#define BIT2	                    0x00000004
#endif

#ifndef BIT3
#define BIT3	                    0x00000008
#endif

#ifndef BIT4
#define BIT4	                    0x00000010
#endif

#ifndef BIT5
#define BIT5	                    0x00000020
#endif

#ifndef BIT6
#define BIT6	                    0x00000040
#endif

#ifndef BIT7
#define BIT7	                    0x00000080
#endif

#ifndef BIT8
#define BIT8	                    0x00000100
#endif

#ifndef BIT9
#define BIT9	                    0x00000200
#endif

#ifndef BIT10
#define BIT10	                    0x00000400
#endif

#ifndef BIT11
#define BIT11	                    0x00000800
#endif

#ifndef BIT12
#define BIT12	                    0x00001000
#endif

#ifndef BIT13
#define BIT13	                    0x00002000
#endif

#ifndef BIT14
#define BIT14	                    0x00004000
#endif

#ifndef BIT15
#define BIT15  	                    0x00008000
#endif

#ifndef BIT16
#define BIT16                       0x00010000
#endif

#ifndef BIT17
#define BIT17                       0x00020000
#endif

#ifndef BIT18
#define BIT18                       0x00040000
#endif

#ifndef BIT19
#define BIT19                       0x00080000
#endif

#ifndef BIT20
#define BIT20                       0x00100000
#endif

#ifndef BIT21
#define BIT21                       0x00200000
#endif

#ifndef BIT22
#define BIT22                       0x00400000
#endif

#ifndef BIT23
#define BIT23                       0x00800000
#endif

#ifndef BIT24
#define BIT24                       0x01000000
#endif

#ifndef BIT25
#define BIT25                       0x02000000
#endif

#ifndef BIT26
#define BIT26                       0x04000000
#endif

#ifndef BIT27
#define BIT27                       0x08000000
#endif

#ifndef BIT28
#define BIT28                       0x10000000
#endif

#ifndef BIT29
#define BIT29                       0x20000000
#endif

#ifndef BIT30
#define BIT30                       0x40000000
#endif

#ifndef BIT31
#define BIT31                       0x80000000
#endif

/******************************************************************************/
/* Macro for bitwise                                                          */
/******************************************************************************/
#define _BITMASK(loc_msb, loc_lsb)  ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))
#define BITMASK(x)                  _BITMASK(1?x, 0?x)
#define _BIT(x)                     (1<<(x))

// UART_SEL_TYPE
#define UART_SEL_HK51_UART0         0x00
#define UART_SEL_HK51_UART1         0x01
#define UART_SEL_VD51_UART0         0x02
#define UART_SEL_AEON               0x03
#define UART_SEL_TSP                0x04
#define UART_SEL_PIU                0x05
#define UART_SEL_PM51_UART0         0x06

#if defined(CONFIG_MSTAR_AMETHYST_BD_FPGA)
#define MSTAR_CHIP_UART_CLK_FREQ     12000000
#else
#define MSTAR_CHIP_UART_CLK_FREQ     123000000
#endif

extern void mips_reboot_setup(void);
extern void mips_time_init(void);
extern void mips_timer_setup(struct irqaction *irq);
extern unsigned long mips_rtc_get_time(void);

static void __init serial_init(void);
//static void __init pad_config(void);
//static void __init miu_priority_config(void);
//static void __init miu_assignment(void);

//L2 cache
// spinlock for L2/Prefetch buffer
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20)
static DEFINE_RAW_SPINLOCK(l2prefetch_lock);
#else
spinlock_t l2prefetch_lock = SPIN_LOCK_UNLOCKED;
#endif

#define DISABLE_MIPS_SYNC_OP            1
#define MIPS_L2_WB_INV_TH               0   //8192    //0x100000        // 1MB
#define MIPS_L2_WB_TH                   0   //8192    //0x100000        // 1MB
#define MIPS_L2_INV_TH                  0   //8192    //0x10000000      // 256MB

const char *get_system_type(void)
{
	return "MStar Chip";
}

static int __init Mstar_Chip_setup(void)
{
	printk("Begin Mstar_Chip_setup\n");
//  chiptop_init();
//  pad_config();
	serial_init();
	mips_reboot_setup();
	return 0;
}

void __init plat_mem_setup(void)
{
    Mstar_Chip_setup();
}

extern void Uart16550IntrruptEnable(void);
extern void Uart16550Put(unsigned char);
static void __init serial_init(void)
{
#ifdef CONFIG_SERIAL_8250
	struct uart_port s;

	memset(&s, 0, sizeof(s));

    //Uart16550IntrruptEnable();
	//while(1) Uart16550Put('*');
	s.type = PORT_16550;
	s.iobase = (0xBF000000 + (0x100980 << 1));
	// s.irq = E_IRQ_UART;
	s.irq = E_IRQ_UART0;

	s.uartclk   = MSTAR_CHIP_UART_CLK_FREQ;
	s.iotype = 0;
	s.regshift = 0;
	s.fifosize = 16 ; // use the 8 byte depth FIFO well

	if (early_serial_setup(&s) != 0)
	{
		printk(KERN_ERR "Serial setup failed!\n");
	}
#endif

//    Uart2_early_Init();
    struct uart_port sUartInfo;
    memset(&sUartInfo, 0, sizeof(sUartInfo));
    sUartInfo.type = PORT_16550;
    sUartInfo.iobase = 0xBF220C00;
    sUartInfo.irq = E_IRQEXPL_UART1;
    sUartInfo.uartclk = MSTAR_CHIP_UART_CLK_FREQ;
    sUartInfo.iotype = 0;
    sUartInfo.regshift = 0;
    sUartInfo.fifosize = 16;
    sUartInfo.line = 1;

    if(early_serial_setup(&sUartInfo) != 0)
    {
             printk(KERN_ERR "[ERROR] PIU UART1 Init Failed\n");
    }
}


#define UART_REG(addr)                  (*(volatile unsigned int*)((addr)))
#define MSTAR_CHIP_UART_BASE            (0xBF000000 + (0x100980 << 1))

#define MSTAR_CHIP_UART_RX              (0x00*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_TX              (0x00*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_DLL             (0x00*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_DLM             (0x02*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_IER             (0x02*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_IIR             (0x04*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_FCR             (0x04*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_LCR             (0x06*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_MCR             (0x08*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_LSR             (0x0A*4 + MSTAR_CHIP_UART_BASE)
#define MSTAR_CHIP_UART_MSR             (0x0C*4 + MSTAR_CHIP_UART_BASE)

//Baud rate
//#define MSTAR_CHIP_UART_BAUD_RATE  115200
#if defined(CONFIG_MSTAR_AMETHYST_BD_FPGA)
#define MSTAR_CHIP_UART_BAUD_RATE       38400
#else
#define MSTAR_CHIP_UART_BAUD_RATE       115200
#endif

//Divisor
#define MSTAR_CHIP_UART_DIVISOR         (MSTAR_CHIP_UART_CLK_FREQ / (16 * MSTAR_CHIP_UART_BAUD_RATE))

#define UART_LSR_DR			            0x01    // Receiver data ready
#define UART_LSR_OE			            0x02    // Overrun error indicator
#define UART_LSR_PE			            0x04    // Parity error indicator
#define UART_LSR_FE			            0x08    // Frame error indicator
#define UART_LSR_BI			            0x10    // Break interrupt indicator
#define UART_LSR_THRE		            0x20    // Transmit-hold-register empty
#define UART_LSR_TEMT		            0x40    // Transmitter empty

int mstar_chip_putc(int c)
{
    unsigned char u8Reg;

    do
	{
        u8Reg = UART_REG(MSTAR_CHIP_UART_LSR) & 0xff;
        if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
        {
            break;
        }
    }while(1);

    if(c=='\n')
    {
        UART_REG(MSTAR_CHIP_UART_TX)='\n';
        do
		{
            u8Reg = UART_REG(MSTAR_CHIP_UART_LSR) & 0xff;
            if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
            {
                break;
            }
        }while(1);
        UART_REG(MSTAR_CHIP_UART_TX)='\r';
    }
    else
    {
        UART_REG(MSTAR_CHIP_UART_TX) = c & 0xffff;
    }
    do
	{
        u8Reg = UART_REG(MSTAR_CHIP_UART_LSR) & 0xff;
        if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
        {
            break;
        }
    }while(1);
    return 1;
}

#define USE_UART0           1
#define USE_ICE             0

void init_chip_uart(void)
{
    *(volatile volatile unsigned int*)(0xBF000000 + (0x100B26 << 1)) = 0x0C00; //Set UART Clock = 123MHz

///// Switch PAD ISP//////////////////////////////////////////////////////////
#if (USE_UART0 == 1)
#if (USE_ICE == 1)
    *(volatile volatile unsigned int*)(0xBF000000 + (0x000E12 << 1)) |= 0x0800;
    *(volatile volatile unsigned int*)(0xBF000000 + (0x101EA6 << 1)) &= ~0x000F;
    *(volatile volatile unsigned int*)(0xBF000000 + (0x101EA6 << 1)) |= 0x0004; //Select UART 0 to PIU UART 0
//  *(volatile volatile unsigned int*)(0xBF000000 + (0x101E02 << 1)) |= 0x0400;
#endif
#else

//   Select UART2 source
    *(volatile volatile unsigned int*)(0xBF000000 + (0x101EA6 << 1)) = 0x0400;  //Select UART 1 to PIU UART 0
    *(volatile volatile unsigned int*)(0xBF000000 + (0x101EA8 << 1)) = 0x0000;  //Select UART 4 to N.A
    *(volatile volatile unsigned int*)(0xBF000000 + (0x101E04 << 1)) |= 0x0800;
    *(volatile volatile unsigned int*)(0xBF000000 + (0x101E04 << 1)) &= 0xFBFF;

#endif  //#if (USE_UART0==0)

    //Disable all UART interrupts
    UART_REG(MSTAR_CHIP_UART_IER) = 0x00;

    //Reset receiver and transmitr
    UART_REG(MSTAR_CHIP_UART_FCR) = 0x07;

    //Set 8 bit char, 1 stop bit, no parity
    UART_REG(MSTAR_CHIP_UART_LCR) = 0x03;

    //Set baud rate
    UART_REG(MSTAR_CHIP_UART_LCR) |= 0x80;
    UART_REG(MSTAR_CHIP_UART_DLM) = (MSTAR_CHIP_UART_DIVISOR >> 8) & 0x00ff;
    UART_REG(MSTAR_CHIP_UART_DLL) = MSTAR_CHIP_UART_DIVISOR & 0x00ff;
    UART_REG(MSTAR_CHIP_UART_LCR) &= ~(0x80) ;

    mstar_chip_putc('H');
    mstar_chip_putc('e');
    mstar_chip_putc('l');
    mstar_chip_putc('l');
    mstar_chip_putc('o');
    mstar_chip_putc(' ');
    mstar_chip_putc('U');
    mstar_chip_putc('A');
    mstar_chip_putc('R');
    mstar_chip_putc('T');
    mstar_chip_putc('\n');
}

inline void _chip_flush_miu_pipe(void)
{
	unsigned long   dwLockFlag = 0;

#if DISABLE_MIPS_SYNC_OP

    unsigned int    dwReadData = 0;

	spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);

    //Flush Pipe Data
    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1));

	dwReadData &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
    dwReadData |= 0x02;    //Mode 2: Flush Pipe Data

    *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) = dwReadData;
    *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) |= BIT4;

    do
    {
        dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x1018BE << 1));
        dwReadData &= _BIT(9);  //Check Status of Flush Pipe Finish

    } while(dwReadData == 0);

	spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);

#else
    spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);
    asm volatile ("sync;");
    spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);
#endif
}

void Chip_L2_cache_wback_inv( unsigned long addr, unsigned long size)
{
    unsigned int    dwReadData = 0;
    unsigned long   dwLockFlag = 0;

    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101880 << 1));
    if((dwReadData & BIT1) != 0)
    {
		_chip_flush_miu_pipe();
    }
    else
    {
        if((addr > 0x40000000) || (size == 0))
        {
            _chip_flush_miu_pipe();
            printk("[ERROR] L2 Cache: Invalid Parameters\n");
        }
        else
        {
            if(size >= MIPS_L2_INV_TH)
            {
                spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);

                //Flush All and Invalid Cache
                dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1));
                dwReadData &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
                dwReadData |= 0x04;    //Mode 4: Flush All and Invalid Cache

                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) = dwReadData;
                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) |= BIT4;

                do
                {
                    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x1018BE << 1));
                    dwReadData &= _BIT(10); //Check Status of Flush All Finish

                } while(dwReadData == 0);

                spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);
            }
            else
            {
                spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);

                *(volatile unsigned int *)(0xBF000000 + (0x101888 << 1)) = ((addr >> 5) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188A << 1)) = (((addr >> 5) >> 16) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188C << 1)) = (((addr + size - 1) >> 5) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188E << 1)) = ((((addr + size - 1) >> 5) >> 16 ) & 0xFFFF);

                //Flush and Invalid Range
                dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1));
                dwReadData &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
                dwReadData |= 0x06;    //Mode 6: Flush Range and Invalid Cache

                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) = dwReadData;
                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) |= BIT4;

                do
                {
                    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x1018BE << 1));
                    dwReadData &= _BIT(11); //Check Status of Flush Range Finish

                } while(dwReadData == 0);

                spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);
            }
        }

        _chip_flush_miu_pipe();
    }
}

//L2 cache write back data function
void Chip_L2_cache_wback( unsigned long addr, unsigned long size)
{
    unsigned int    dwReadData = 0;
    unsigned long   dwLockFlag = 0;

    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101880 << 1));
    if((dwReadData & BIT1) != 0)
    {
        _chip_flush_miu_pipe();
    }
    else
    {
        if((addr > 0x40000000) || (size == 0))
        {
            printk("[ERROR] L2 Cache: Invalid Parameters\n");
        }
        else
        {
            if(size >= MIPS_L2_INV_TH)
            {
                spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);

                //Flush All and Keep Valid
                dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1));
                dwReadData &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
                dwReadData |= 0x03;    //Mode 3: Flush All and Keep Valid

                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) = dwReadData;
                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) |= BIT4;

                do
                {
                    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x1018BE << 1));
                    dwReadData &= _BIT(10); //Check Status of Flush All Finish

                } while(dwReadData == 0);

                spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);
            }
            else
            {
                spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);

                *(volatile unsigned int *)(0xBF000000 + (0x101888 << 1)) = ((addr >> 5) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188A << 1)) = (((addr >> 5) >> 16) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188C << 1)) = (((addr + size - 1) >> 5) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188E << 1)) = ((((addr + size - 1) >> 5) >> 16 ) & 0xFFFF);

                //Flush Range and Keep Valid
                dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1));
                dwReadData &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
                dwReadData |= 0x05;    //Mode 5: Flush Range and Keep Valid

                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) = dwReadData;
                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) |= BIT4;

                do
                {
                    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x1018BE << 1));
                    dwReadData &= _BIT(11); //Check Status of Flush Range Finish

                } while(dwReadData == 0);

                spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);
            }
        }

        _chip_flush_miu_pipe();
    }
}

//L2 cache invalid function
void Chip_L2_cache_inv( unsigned long addr, unsigned long size)
{
    unsigned int    dwReadData = 0;
    unsigned long   dwLockFlag = 0;

    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101880 << 1));
    if((dwReadData & BIT1) != 0)
    {
        _chip_flush_miu_pipe();
    }
    else
    {
        if((addr > 0x40000000) || (size == 0))
        {
            printk("[ERROR] L2 Cache: Invalid Parameters\n");
        }
        else
        {
            if(size >= MIPS_L2_INV_TH)
            {
                spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);

                //Flush All and Invalid Cache
                dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1));
                dwReadData &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
                dwReadData |= 0x04;    //Mode 4: Flush All and Invalid Cache

                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) = dwReadData;
                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) |= BIT4;

                do
                {
                    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x1018BE << 1));
                    dwReadData &= _BIT(10); //Check Status of Flush All Finish

                } while(dwReadData == 0);

                spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);
            }
            else // range invalid
            {
                spin_lock_irqsave(&l2prefetch_lock, dwLockFlag);

                *(volatile unsigned int *)(0xBF000000 + (0x101888 << 1)) = ((addr >> 5) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188A << 1)) = (((addr >> 5) >> 16) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188C << 1)) = (((addr + size - 1) >> 5) & 0xFFFF);
                *(volatile unsigned int *)(0xBF000000 + (0x10188E << 1)) = ((((addr + size - 1) >> 5) >> 16 ) & 0xFFFF);

                //Flush and Invalid Range
                dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1));
                dwReadData &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
                dwReadData |= 0x06;    //Mode 6: Flush Range and Invalid Cache

                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) = dwReadData;
                *(volatile unsigned int *)(0xBF000000 + (0x101882 << 1)) |= BIT4;

                do
                {
                    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x1018BE << 1));
                    dwReadData &= _BIT(11); //Check Status of Flush Range Finish

                } while(dwReadData == 0);

                spin_unlock_irqrestore(&l2prefetch_lock, dwLockFlag);
            }
        }

        // flush MIU pipe
        _chip_flush_miu_pipe();
    }
}

void Chip_Flush_Memory_Range(unsigned long pAddress , unsigned long  size)
{
    unsigned int    dwReadData = 0;

    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101880 << 1));
    if((dwReadData & BIT2) == 1)
    {
        Chip_L2_cache_wback( pAddress , size );
    }
    else
    {
        _chip_flush_miu_pipe();
    }
}

//flush miu pipe
void Chip_Flush_Memory(void)
{
    Chip_Flush_Memory_Range(0, 0xFFFFFFFF);
}

void Chip_Read_Memory_Range(unsigned long pAddress , unsigned long  size)
{
    unsigned int    dwReadData = 0;
    dwReadData = *(volatile unsigned int *)(0xBF000000 + (0x101880 << 1));
    if((dwReadData & BIT2) == 1)
    {
        Chip_L2_cache_inv(pAddress, size);
    }
    else
    {
	    // flush MIU pipe
	    _chip_flush_miu_pipe();
    }
}

void Chip_Read_Memory(void)
{
    Chip_Read_Memory_Range(0, 0xFFFFFFFF);
}

unsigned int Chip_Query_Rev(void)
{
	unsigned int    dwRevisionId = 0;

	dwRevisionId = *((volatile unsigned int*)(0xBF000000 + (0x1ECE << 1)));
    dwRevisionId >>= 8;

	return dwRevisionId;
}
EXPORT_SYMBOL(Chip_Query_Rev);

unsigned int Chip_Query_MIPS_CLK(void)
{
    unsigned int    dwMipsCpuFreq = 0;

    dwMipsCpuFreq = *(volatile u32*)(0xBF000000 + (0x110C26 << 1));
    dwMipsCpuFreq = ((dwMipsCpuFreq & 0xFF) * 6000000);

    return dwMipsCpuFreq;
}

EXPORT_SYMBOL(Chip_L2_cache_wback_inv);
EXPORT_SYMBOL(Chip_L2_cache_wback);
EXPORT_SYMBOL(Chip_L2_cache_inv);
EXPORT_SYMBOL(Chip_Flush_Memory);
EXPORT_SYMBOL(Chip_Flush_Memory_Range);
EXPORT_SYMBOL(Chip_Read_Memory);
EXPORT_SYMBOL(Chip_Read_Memory_Range);

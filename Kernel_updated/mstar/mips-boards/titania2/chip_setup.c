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
#include <linux/undefconf.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/tty.h>

#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>


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
#include "hwreg_S5.h"
#ifdef CONFIG_VT
#include <linux/console.h>
#endif

#include "Board.h"

const char display_string[] = "        LINUX ON Titania 2       ";

/******************************************************************************/
/* Macro for bitwise                                                          */
/******************************************************************************/
#define _BITMASK(loc_msb, loc_lsb) ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))
#define BITMASK(x) _BITMASK(1?x, 0?x)

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

// UART_SEL_TYPE
#define UART_SEL_HK51_UART0     0x00
#define UART_SEL_HK51_UART1     0x01
#define UART_SEL_VD51_UART0     0x02
#define UART_SEL_AEON           0x03
#define UART_SEL_TSP            0x04
#define UART_SEL_PIU            0x05
#define UART_SEL_PM51_UART0     0x06


//------------------------------------------------------------------------------
// MIU assignment   0: MIU0, 1: MIU1
//------------------------------------------------------------------------------
    #define MIUSEL_FLAG_FSEICH_R        0
    #define MIUSEL_FLAG_SVD_DB_R        1
    #define MIUSEL_FLAG_SVD_DB_W        1
    #define MIUSEL_FLAG_FDECICH_R       0
    #define MIUSEL_FLAG_OD_LSB_R        0
    #define MIUSEL_FLAG_OD_LSB_W        0
    #define MIUSEL_FLAG_GOP2_R          0

    #define MIUSEL_FLAG_OPM_R           0
    #define MIUSEL_FLAG_MVOP_R          1
    #define MIUSEL_FLAG_GOP0_W          1
    #define MIUSEL_FLAG_GOP0_R          1
    #define MIUSEL_FLAG_GOP1_R          0
    #define MIUSEL_FLAG_DNRB_W          0
    #define MIUSEL_FLAG_DNRB_R          0
    #define MIUSEL_FLAG_COMB_R          0
    #define MIUSEL_FLAG_COMB_W          0
    #define MIUSEL_FLAG_VE_R            0
    #define MIUSEL_FLAG_VE_W            0
    #define MIUSEL_FLAG_OD_R            0
    #define MIUSEL_FLAG_OD_W            0
    #define MIUSEL_FLAG_FSEDMA2_RW      0
    #define MIUSEL_FLAG_MAD_RW          0

    #define MIUSEL_FLAG_SVD_EN_R        1
    #define MIUSEL_FLAG_MVD_R           1   // shared by RVD_RW, SVDINTP_R, MVD_R
    #define MIUSEL_FLAG_AESDMA_W        0
    #define MIUSEL_FLAG_AESDMA_R        0
    #define MIUSEL_FLAG_MVD_W           1
    #define MIUSEL_FLAG_JPD_R           0
    #define MIUSEL_FLAG_JPD_W           0
//------------------------------------------------------------------------------

extern void mips_reboot_setup(void);
extern void mips_time_init(void);
extern void mips_timer_setup(struct irqaction *irq);
extern unsigned long mips_rtc_get_time(void);

static void __init serial_init(void);
static void __init chiptop_init(void);
static void __init miu_priority_config(void);
static void __init miu_assignment(void);

#if 0
struct resource standard_io_resources[] = {
	{ "dma1", 0x00, 0x1f, IORESOURCE_BUSY },
	{ "timer", 0x40, 0x5f, IORESOURCE_BUSY },
	{ "keyboard", 0x60, 0x6f, IORESOURCE_BUSY },
	{ "dma page reg", 0x80, 0x8f, IORESOURCE_BUSY },
	{ "dma2", 0xc0, 0xdf, IORESOURCE_BUSY },
};
#endif

//#ifdef CONFIG_MTD
#if 0
static struct mtd_partition malta_mtd_partitions[] = {
	{
		.name =		"YAMON",
		.offset =	0x0,
		.size =		0x100000,
		.mask_flags =	MTD_WRITEABLE
	},
	{
		.name =		"User FS",
		.offset = 	0x100000,
		.size =		0x2e0000
	},
	{
		.name =		"Board Config",
		.offset =	0x3e0000,
		.size =		0x020000,
		.mask_flags =	MTD_WRITEABLE
	}
};

#define number_partitions	(sizeof(malta_mtd_partitions)/sizeof(struct mtd_partition))
#endif

const char *get_system_type(void)
{
	return "MStar Titania";
}

static int __init Titania_setup(void)
{
    int i=0;
    i=i;

    printk("Begin Titania_setup\n");

#if defined(CONFIG_MSTAR_TITANIA2)

#if defined(CONFIG_MSTAR_TITANIA_MMAP_32MB_32MB) || \
    defined(CONFIG_MSTAR_TITANIA_MMAP_64MB_32MB) || \
    defined(CONFIG_MSTAR_TITANIA_MMAP_64MB_64MB) || \
    defined(CONFIG_MSTAR_TITANIA_MMAP_128MB_64MB) || \
    defined(CONFIG_MSTAR_TITANIA_MMAP_128MB_128MB) || \
    defined(CONFIG_MSTAR_TITANIA_MMAP_128MB_64MB_PROJ_OBAMA) || \
    defined(CONFIG_MSTAR_TITANIA_MMAP_128MB_128MB_PROJ_OBAMA)||\
    defined(CONFIG_MSTAR_MMAP_128MB_128MB_DEFAULT)

       // for MIU assignment
       miu_assignment();
#endif

#endif

#if 1
	chiptop_init();
#endif

	miu_priority_config();

	serial_init();

	mips_reboot_setup();

	//board_time_init = mips_time_init;
	//board_timer_setup = mips_timer_setup;
#if 0
	rtc_get_time = mips_rtc_get_time;
#endif
	return 0;
}

// early_initcall(Titania_setup);
void __init plat_mem_setup(void)
{
	Titania_setup();
}

extern void Uart16550IntrruptEnable(void);
extern void Uart16550Put(unsigned char);
static void __init serial_init(void)
{
#ifdef CONFIG_SERIAL_8250
	struct uart_port s;

	memset(&s, 0, sizeof(s));

	Uart16550IntrruptEnable();
	//while(1) Uart16550Put('*');
	s.type = PORT_16550;
	s.iobase = 0xBF801300;
	s.irq = E_IRQ_UART;

#ifdef CONFIG_MSTAR_TITANIA2

	// for FPGA
	//s.uartclk = 14318180;
	//s.uartclk = 123000000;
	//s.uartclk = 144000000;
	//s.uartclk = 160000000;
	s.uartclk = 172800000;
	//s.uartclk = 12000000;
	//s.uartclk = 108000000;

#else

#ifdef CONFIG_MSTAR_TITANIA_BD_FPGA
	s.uartclk = FPGA_XTAL_CLOCK;    //12000000;///14318180;   //43200000;
#else
	s.uartclk = 123000000;
#endif

#endif
	s.iotype = 0;
	s.regshift = 0;
	s.fifosize = 16 ; // use the 8 byte depth FIFO well

	if (early_serial_setup(&s) != 0) {
		printk(KERN_ERR "Serial setup failed!\n");
	}
#endif
}

static void __init _RV1(unsigned int addr, unsigned short value)
{
    if(addr & 1)
        (*(volatile unsigned char *)(0xbf800000+((addr-1)<<1)+1))=(unsigned char)(value & 0xFF);
    else
        (*(volatile unsigned char *)(0xbf800000+(addr<<1)))=(unsigned char)(value & 0xFF);
}

static void __init _RV2(unsigned int addr, unsigned short value)
{
    (*(volatile unsigned short *)(0xbf800000+(addr<<1)))=(unsigned short)(value & 0xFFFF);
}

static void __init chiptop_init(void)
{
    // MIU timeout settings, Samuel, 20090303
    *((volatile unsigned int*)(0xBF800000 + (0x06F4<<1))) = 0x0400; // MIU0 group2 timeout value
    *((volatile unsigned int*)(0xBF800000 + (0x0626<<1))) = 0x0400; // MIU1 group2 timeout value
    *((volatile unsigned int*)(0xBF800000 + (0x06F0<<1))) = 0x8010; // MIU0 group2 timeout enable
    *((volatile unsigned int*)(0xBF800000 + (0x0622<<1))) = 0x8010; // MIU1 group2 timeout enable
    *((volatile unsigned int*)(0xBF800000 + (0x0640<<1))) |= BIT4;  // MIU1 Group0 Timeout enable
    *((volatile unsigned int*)(0xBF800000 + (0x0660<<1))) |= BIT4;  // MIU1 Group1 Timeout enable

    _RV1(0x1E2B, CKG_DHC_SBM_12MHZ | CKG_GE_144MHZ);       // DHC_SBM, GE
    {
        volatile unsigned int a ;
        volatile unsigned int b ;

        //RobertYang, currently, Aeon can't run on 170Mhz.
        //Aeon switch to 170M
        *(volatile unsigned int*)(0xbf800000+(0x3c88)) &= ~(0x1F00) ;
        *(volatile unsigned int*)(0xbf800000+(0x4A20)) |= 0x0080 ; // 170MHZ

        // set mcu (8051) clock to enable TX empty interrupt
        *(volatile unsigned int*)(0xbf800000+(0x3c44)) |= 0x0001 ;

        // wait for a while until mcu clock stable
        for( a=0; a<0xFF; a++ )
            b = a ;
    }
}

static void __init miu_priority_config(void)
{
	//samuel 081022 //*((volatile unsigned int *) (0xbf80240c)) |= 0x4000;        // MIU group 1 priority > group 0
    *((volatile unsigned int *) (0xbf8024c0)) |= 0x000b;        // Group 1 round robin enable; Group 1 fixed priority enable; Group 1 group limit enable
    *((volatile unsigned int *) (0xbf802500)) |= 0x000E;        // Group 2 seeting
    *((volatile unsigned int *) (0xbf802504)) |= 0x0004;        // Group 2 member max.

    // MIPS priority to high, samuel 081022
    *(volatile unsigned int*)(0xbf800000+(0x1248<<1)) = 0xFF9F ;
    *(volatile unsigned int*)(0xbf800000+(0x124A<<1)) = 0x1065 ;
    *(volatile unsigned int*)(0xbf800000+(0x124C<<1)) = 0x7432 ;
    *(volatile unsigned int*)(0xbf800000+(0x1206<<1)) &= ~(1<<14) ;
    *(volatile unsigned int*)(0xbf800000+(0x1240<<1)) |= (1<<1) ; // triger priority setting working
    *(volatile unsigned int*)(0xbf800000+(0x1240<<1)) &= ~(1<<1) ; // triger priority setting working, samuel 081022
}

static int GOP0_MIU=0;
static int __init GOP0_MIU_setup(char *str)
{
    if( str != NULL )
    {
        sscanf(str,"%d",&GOP0_MIU);
    }
    return 0;
}
early_param("MS_GOP0_MIU", GOP0_MIU_setup);

static void __init miu_assignment(void)
{
    _RV2(0x12F0,
                (MIUSEL_FLAG_FSEICH_R   << 0x09)    |\
                (MIUSEL_FLAG_SVD_DB_R   << 0x0C)    |\
                (MIUSEL_FLAG_SVD_DB_W   << 0x0D)    |\
                (MIUSEL_FLAG_FDECICH_R  << 0x0E));

    _RV2(0x12F2,
                (MIUSEL_FLAG_OPM_R      << 0x00)    |\
                (MIUSEL_FLAG_MVOP_R     << 0x01)    |\
                (GOP0_MIU/*GOP0_W*/     << 0x02)    |\
                (GOP0_MIU/*GOP0_R*/     << 0x03)    |\
                (MIUSEL_FLAG_GOP1_R     << 0x04)    |\
                (MIUSEL_FLAG_DNRB_W     << 0x06)    |\
                (MIUSEL_FLAG_DNRB_R     << 0x07)    |\
                (MIUSEL_FLAG_COMB_R     << 0x08)    |\
                (MIUSEL_FLAG_COMB_W     << 0x09)    |\
                (MIUSEL_FLAG_VE_R       << 0x0A)    |\
                (MIUSEL_FLAG_VE_W       << 0x0B)    |\
                (MIUSEL_FLAG_OD_R       << 0x0C)    |\
                (MIUSEL_FLAG_OD_W       << 0x0D)    |\
                (MIUSEL_FLAG_FSEDMA2_RW << 0x0E)    |\
                (MIUSEL_FLAG_MAD_RW     << 0x0F));

    _RV2(0x12F4,
                (MIUSEL_FLAG_SVD_EN_R   << 0x00)    |\
                (MIUSEL_FLAG_MVD_R      << 0x06)    |\
                (MIUSEL_FLAG_AESDMA_W   << 0x07)    |\
                (MIUSEL_FLAG_MVD_W      << 0x0B)    |\
                (MIUSEL_FLAG_JPD_R      << 0x0C)    |\
                (MIUSEL_FLAG_JPD_W      << 0x0D));
}

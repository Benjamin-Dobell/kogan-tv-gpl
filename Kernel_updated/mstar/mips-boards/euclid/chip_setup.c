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

#include "board/Board.h"


const char display_string[] = "        LINUX ON Euclid       ";

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
// Group 0
#define MIUSEL_FLAG_ORZ_W               0
#define MIUSEL_FLAG_TSP_R               0
#define MIUSEL_FLAG_AUDIO_DMA_RW        0
#define MIUSEL_FLAG_PVR2_W              0
#define MIUSEL_FLAG_MAU_0_RW            0
#define MIUSEL_FLAG_MAU_1_RW            0
#define MIUSEL_FLAG_OD_LSB_GOP2_R       0
#define MIUSEL_FLAG_OD_LSB_VE_W         0
#define MIUSEL_FLAG_FDSEICH_R           0
#define MIUSEL_FLAG_TSP_W               0
#define MIUSEL_FLAG_TTXSK_VD_W          0
#define MIUSEL_FLAG_HVD_RW              1
#define MIUSEL_FLAG_VD_MHEG5_I_R        1
#define MIUSEL_FLAG_FDDECICH_R          0

// Group 1
#define MIUSEL_FLAG_OPM_R               0
#define MIUSEL_FLAG_MVOP_R              1
#define MIUSEL_FLAG_GOP0_W              0
#define MIUSEL_FLAG_GOP0_R              0
#define MIUSEL_FLAG_GOP1_R              0
#define MIUSEL_FLAG_GOP2_OD_LSB_R       0
#define MIUSEL_FLAG_DNR_W               0
#define MIUSEL_FLAG_DNR_R               0
#define MIUSEL_FLAG_COMB_W              0
#define MIUSEL_FLAG_COMB_R              0
#define MIUSEL_FLAG_VE_OD_LSB_W         0
#define MIUSEL_FLAG_VE_R                0
#define MIUSEL_FLAG_DNRA_RW             0
#define MIUSEL_FLAG_MPIF_RW             0
#define MIUSEL_FLAG_DMA2_RW             0
#define MIUSEL_FLAG_MAD_RW              0

// Group 2
#define MIUSEL_FLAG_HVD_RVD_BBU_R       1
#define MIUSEL_FLAG_ORZ_R               0
#define MIUSEL_FLAG_GE_RW               0
#define MIUSEL_FLAG_MIU_DMA_RW          1
#define MIUSEL_FLAG_FCIE_RW             0
#define MIUSEL_FLAG_USB20_0_RW          0
#define MIUSEL_FLAG_RVD_MVD_RW          1
#define MIUSEL_FLAG_AESDMA_RW           0
#define MIUSEL_FLAG_TTX_VD_RW           0
#define MIUSEL_FLAG_VD_MHEG5_D_RW       1
#define MIUSEL_FLAG_USB20_1_RW          0
#define MIUSEL_FLAG_MOBF_RW             1
#define MIUSEL_FLAG_STRLE_RW            0
#define MIUSEL_FLAG_VD_MHEG5_QDMA_RW    1
#define MIUSEL_FLAG_JPD_RW              0
#define MIUSEL_FLAG_EMAC_RW             1

#define MIUSEL_FLAG_DNRB_RW         (MIUSEL_FLAG_DNR_R     | \
                                     MIUSEL_FLAG_DNR_W)

#define MIUSEL_FLAG_SC_RW           (MIUSEL_FLAG_DNRB_RW    | \
                                     MIUSEL_FLAG_OPM_R)

//------------------------------------------------------------------------------

extern void mips_reboot_setup(void);
extern void mips_time_init(void);
extern void mips_timer_setup(struct irqaction *irq);
extern unsigned long mips_rtc_get_time(void);

static void __init serial_init(void);
static void __init chiptop_init(void);
static void __init release_ip_reset(void);
//static void __init miu_priority_config(void);
//static void __init miu_assignment(void);

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
	return "MStar Euclid";
}

extern int MDrv_SYS_GetMMAP(int type, unsigned int *addr, unsigned int *len) ;
extern void init_chip_uart(void);

static int __init Euclid_setup(void)
{
       // int i;

#if 0
       *((char*)0xbf204b80) = 0x00;  // Dean
       *((char*)0xbf207558) = *((char*)0xbf207558) | 0x04;  // Dean
       *((char*)0xbf20750c) = *((char*)0xbf20750c) | 0x04;  // Dean
       *((char*)0xbf207500) = 0x00;  // Dean
       *((char*)0xbf207501) = 0x00;  // Dean
       *((char*)0xbf200e00) = *((char*)0xbf200e00) | 0x28;  // Dean
#endif

	printk("Begin Euclid_setup\n");

#if defined(CONFIG_MSTAR_EUCLID)

#if defined(CONFIG_MSTAR_EUCLID_MMAP_32MB_32MB) || \
    defined(CONFIG_MSTAR_EUCLID_MMAP_64MB_32MB) || \
    defined(CONFIG_MSTAR_EUCLID_MMAP_64MB_64MB) || \
    defined(CONFIG_MSTAR_EUCLID_MMAP_128MB_64MB) || \
    defined(CONFIG_MSTAR_MMAP_128MB_128MB_DEFAULT) || \
    defined(CONFIG_MSTAR_EUCLID_MMAP_128MB_128MB)

       // for MIU assignment
       // @FIXME: Richard correct miu assignment later
       //miu_assignment();
       //EMAC access miu1
       *((volatile unsigned int *) (0xbf202438)) |= 0x0040;        // Group 2 member max.
#endif

#endif

#if 0
       // @FIXME: Richard: should Euclid require this?
       // **************************************************
       // disable XIU no-ack timeout issue, for using MSTV tool
       *((char*)0xBF201D80) = *((char*)0xBF201D80) | 0x0001;
       *((unsigned short*)0xBF201DA8) = 0xFFFF;
       // **************************************************
#endif

#if 0
       // setting DRAM size
       #if(CONFIG_MSTAR_EUCLID_MMAP_32MB)
            *((volatile unsigned short *)0xbf202404) = 0x0345; //32M
       #elif(CONFIG_MSTAR_EUCLID_MMAP_64MB)
            *((volatile unsigned short *)0xbf202404) = 0x0545; //64M
       #endif
#endif

#if 1 // @FIXME: Richard correct this later
    chiptop_init();
#endif
    //init_chip_uart();


    release_ip_reset();

#if 0
#ifdef USE_CUS03_DEMO_BOARD
       //for BD_MSTAR_S5_DEMO, UART_TX2 pull low for LG power board
       volatile unsigned int *reg;
       // set UART_TX2
       reg = (volatile unsigned int *)(0xbf203c08);
       *reg = ((*reg) & 0xf3ff);
       reg = (volatile unsigned int *)(0xbf203c0c);
       *reg = ((*reg) & 0xf1ff);
       // UART_TX2 eon
       reg = (volatile unsigned int *)(0xbf203cc0);
       *reg = ((*reg) & 0xfffd);
       // UART_TX2 pull low
       reg = (volatile unsigned int *)(0xbf203cbc);
       *reg = ((*reg) & 0xfffd);
#endif
#endif

       // @FIXME: Richard correct this later
       //miu_priority_config();
       // @FIXME: Richard correct this later

	serial_init();

	mips_reboot_setup();

	//board_time_init = mips_time_init;
	//board_timer_setup = mips_timer_setup;
#if 0
	rtc_get_time = mips_rtc_get_time;
#endif
	return 0;
}

// early_initcall(Euclid_setup);
void __init plat_mem_setup(void)
{
    Euclid_setup();
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
	s.iobase = 0xbf201300;
	// s.irq = E_IRQ_UART;
	s.irq = E_IRQ_UART0;

#ifdef CONFIG_MSTAR_EUCLID

	// for FPGA
	// s.uartclk   = 12000000;
	//s.uartclk = 14318180;
	//s.uartclk = 123000000;
	//s.uartclk = 144000000;
	//s.uartclk = 160000000;
	s.uartclk = 172800000;
	//s.uartclk = 108000000;

#else

#ifdef CONFIG_MSTAR_EUCLID_BD_FPGA
	//s.uartclk = FPGA_XTAL_CLOCK;    //12000000;///14318180;   //43200000;
	s.uartclk = 12000000 ;
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

#define _MapBase_nonPM_U3 0xbf200000
#define _MapBase_PM_U3    0xbf000000

static unsigned int u32MapBase = _MapBase_nonPM_U3;

static void __init _RV1(unsigned int addr, unsigned short value)
{
    if(addr & 1)
        (*(volatile unsigned char *)(u32MapBase+((addr-1)<<1)+1))=(unsigned char)(value & 0xFF);
    else
        (*(volatile unsigned char *)(u32MapBase+(addr<<1)))=(unsigned char)(value & 0xFF);
}
#if 0
static void __init _RV2(unsigned int addr, unsigned short value)
{
    (*(volatile unsigned short *)(u32MapBase+(addr<<1)))=(unsigned short)(value & 0xFFFF);
}


static void __init _RV3(unsigned int addr, unsigned int value)
{
    _RV1(addr, value & 0xFF);
    _RV1(addr+1, (value>>8) & 0xFF);
    _RV1(addr+2, (value>>16) & 0xFF);
}

static void __init _RV4(unsigned int addr, unsigned int value)
{
    (*(volatile unsigned int *)(u32MapBase+(addr<<1)))=(unsigned int)value;
}
#endif

static void __init _RVM1(unsigned int addr, unsigned short value, unsigned char u8Mask)
{
    volatile unsigned char *reg;

    if(addr & 1)
        reg = (volatile unsigned char *)(u32MapBase+((addr-1)<<1)+1);
    else
        reg = (volatile unsigned char *)(u32MapBase+(addr<<1));

    *reg = ((*reg) & (~u8Mask)) | (value & u8Mask);
}

#if 0
static void __init MDrv_WriteByteMask(unsigned int addr, unsigned char value, unsigned char u8Mask)
{
    volatile unsigned char *reg;

    if(addr & 1)
        reg = (volatile unsigned char *)(u32MapBase+((addr-1)<<1)+1);
    else
        reg = (volatile unsigned char *)(u32MapBase+(addr<<1));

    *reg = ((*reg) & (~u8Mask)) | (value & u8Mask);
}

static void __init MDrv_WriteRegBit(unsigned int addr, int bBit, unsigned char u8BitPos)
{
    volatile unsigned char *reg;

    if(addr & 1)
        reg = (volatile unsigned char *)(u32MapBase+((addr-1)<<1)+1);
    else
        reg = (volatile unsigned char *)(u32MapBase+(addr<<1));

    *reg = (bBit) ? ( *reg | u8BitPos) : (*reg | ~u8BitPos);
}
#endif

static void __init chiptop_init(void)
{
//	unsigned int tmp ;
//	int i ;

#if 0
    // The initials are ported from /kingwork/kernel/s4m by Dean Tseng

    // The following 6 setup were done in bootloader

    //_RV4(0x2508, 0x0),      // clear power down
    //_RV2(0x250C, 0x0),      // clear power down clocks

    // MPLL
    //_RV2(0x2514, 0x24),
    //_RV2(0x2510, 0x0506),
    //_RV2(0x2516, 0x0),      // MPLL div clocks enable
    //_RV2(0x25C0, 0x0),

    _RV1(0x1E24, 0x0);                      // USB30

    // DDR MIU was set in bootloader
    //_RV1(0x1E25, CKG_DDR_MEMPLL |           // DDR, MIU
    //             CKG_MIU_MEMPLL),
    _RV1(0x1E26, CKG_TS0_TS0_CLK);          // TS0, TCK, AEON
    _RV1(0x1E27, CKG_TSP_144MHZ |CKG_STC0_STC0_SYNTH);  // TSP, STC0, MAD_STC

    _RV1(0x1E28, CKG_MAD_STC_STC0_SYNTH);   // MAD_STC
    _RV1(0x1E29, CKG_MVD_BOOT_CLK_MIU | CKG_MVD_144MHZ);     // MVD_BOOT, MVD
    _RV1(0x1E2A, CKG_M4V_144MHZ | CKG_DC0_SYNCHRONOUS);      // M4V, DC0
    _RV1(0x1E2B, CKG_DHC_SBM_12MHZ | CKG_GE_144MHZ);       // DHC_SBM, GE
    _RV1(0x1E2C, CKG_GOPG0_ODCLK | CKG_GOPG1_ODCLK);         // GOPG0, GOPG1
    _RV1(0x1E2D, CKG_VD_CLK_VD);            // VD
    _RV1(0x1E2E, CKG_VDMCU_108MHZ | CKG_VD200_216MHZ);        // VDMCU, VD200
    _RV1(0x1E2F, CKG_DHC_86MHZ);            // DHC
    _RV1(0x1E30, CKG_FICLK_F2_CLK_IDCLK2);  // FICLK_F2
    _RV1(0x1E31, CKG_GOPG2_CLK_ODCLK | CKG_PCM_27MHZ);     // GOPG2, PCM
    _RV1(0x1E32, 0);                        // reserved
    _RV1(0x1E33, CKG_VE_27MHZ | CKG_VEDAC_27MHZ);            // VE, VEDEC
    _RV1(0x1E34, CKG_DAC_CLK_ODCLK);        // DAC
    _RV1(0x1E35, CKG_FCLK_MPLL);
//    _RV1(0x1E36, 0x00);                     // FIXME, disable CPU_M later
    _RV1(0x1E37, CKG_ODCLK_XTAL_);          // ODCLK (suggested by janick in bringup)
    _RV1(0x1E38, CKG_VE_IN_CLK_MPEG0);      // VE_IN
    _RV1(0x1E39, CKG_NFIE_62MHZ);           // NFIE
    _RV1(0x1E3A, CKG_TS2_TS2_CLK | CKG_TSOUT_27MHZ);         // TS2, TSOUT
                                                                                                    // UART use XTAL
    _RV1(0x1E3D, CKG_DC1_SYNCHRONOUS);      // DC1    _RV1(0x1E3E, CKG_IDCLK1_XTAL),          // IDCLK1
    _RV1(0x1E3F, CKG_IDCLK2_XTAL);          // IDCLK2
    _RV1(0x1E44, CKG_STRLD_144MHZ);         // STRLD
	//_RV1(0x1E45, CKG_MCU_144MHZ);           // MCU		//RobertYang
    _RV1(0x1E9A, CKG_JPD_123MHZ | CKG_SDIO_XTAL);          // JPD, SDIO
#endif

	_RV1(0x1E2B, CKG_DHC_SBM_12MHZ | CKG_GE_144MHZ);       // DHC_SBM, GE
	{
		volatile unsigned int a ;
		volatile unsigned int b ;

		//RobertYang, currently, Aeon can't run on 170Mhz.
		//Aeon switch to 170M
		*(volatile unsigned int*)(0xbf200000+(0x3c88)) &= ~(0x1F00) ;
		*(volatile unsigned int*)(0xbf200000+(0x4A20)) |= 0x0080 ; // 170MHZ

		// set mcu (8051) clock to enable TX empty interrupt
		*(volatile unsigned int*)(0xbf200000+(0x3c44)) |= 0x0001 ;

		// wait for a while until mcu clock stable
		for( a=0; a<0xFF; a++ )
			b = a ;
	}
}

// The following macros are defined just for easy porting from 8051 code
#define MDrv_WriteByte(addr, value)     _RV1(addr, value)
#define MDrv_Write2Byte(addr, value)    _RV2(addr, value)
#define MDrv_Write3Byte(addr, value)    _RV3(addr, value)
#define MDrv_Write4Byte(addr, value)    _RV4(addr, value)

static void __init release_ip_reset(void)
{
    u32MapBase = _MapBase_nonPM_U3;
    //-----------------------------------------------------------------
    //  Release IP reset
    //-----------------------------------------------------------------
    _RVM1(0x0500,0x00,0xFF);
    _RVM1(0x3960,0x01,0xFF);
    _RVM1(0x2101,0xF0,0xFF);
    _RVM1(0x15F4,0x02,0xFF);
    _RVM1(0x15FF,0x00,0xFF);
    _RVM1(0x0361,0x10,0xFF);
    _RVM1(0x2800,0x03,0xFF);
    _RVM1(0x2804,0x00,0xFF);

    _RVM1(0x1FFE,0x00,0xFF);
    _RVM1(0x1F00,0x00,0xFF);
    #if 0
    _RVM1(0x1FFE,0x03,0xFF);
    _RVM1(0x1F00,0x00,0xFF);
    _RVM1(0x1FFE,0x07,0xFF);
    _RVM1(0x1F00,0x00,0xFF);
    #endif
    _RVM1(0x1FFE,0x06,0xFF);
    _RVM1(0x1FFF,0x00,0xFF);
    _RVM1(0x1F00,0x00,0xFF);
    _RVM1(0x1FFE,0x0A,0xFF);
    _RVM1(0x1F00,0x00,0xFF);

    _RVM1(0x3613,0x00,0xFF);
    _RVM1(0x3610,0x12,0xFF);
    _RVM1(0x1CF0,0x00,0xFF);
    _RVM1(0x0A02,0x00,0xFF);
    _RVM1(0x1B02,0x00,0xFF);

    _RVM1(0x3B00,0x00,0xFF);
    _RVM1(0x0700,0x00,0xFF);
    _RVM1(0x0880,0x00,0xFF);
    _RVM1(0x2C00,0x00,0xFF);

    _RVM1(0x2F00,0x00,0xFF);
    _RVM1(0x2F04,0x00,0xFF);

}
#if 0
static void __init miu_priority_config(void)
{
	//samuel 081022 //*((volatile unsigned int *) (0xbf20240c)) |= 0x4000;        // MIU group 1 priority > group 0
    *((volatile unsigned int *) (0xbf2024c0)) |= 0x000b;        // Group 1 round robin enable; Group 1 fixed priority enable; Group 1 group limit enable
    *((volatile unsigned int *) (0xbf202500)) |= 0x000E;        // Group 2 seeting
    *((volatile unsigned int *) (0xbf202504)) |= 0x0004;        // Group 2 member max.

    // MIPS priority to high, samuel 081022
    *(volatile unsigned int*)(0xbf200000+(0x1248<<1)) = 0xFF9F ;
    *(volatile unsigned int*)(0xbf200000+(0x124A<<1)) = 0x1065 ;
    *(volatile unsigned int*)(0xbf200000+(0x124C<<1)) = 0x7432 ;
    *(volatile unsigned int*)(0xbf200000+(0x1206<<1)) &= ~(1<<14) ;
    *(volatile unsigned int*)(0xbf200000+(0x1240<<1)) |= (1<<1) ; // triger priority setting working
    *(volatile unsigned int*)(0xbf200000+(0x1240<<1)) &= ~(1<<1) ; // triger priority setting working, samuel 081022
}
#endif
static int GOP0_MIU = 0;
static int GOP1_MIU = 0;
static int GOP2_MIU = 0;
int HP=0; // special runtime flag to turn on for certain performance enhancement

static int __init GOP0_MIU_setup(char *str)
{
    if( str != NULL )
    {
        sscanf(str,"%d",&GOP0_MIU);
    }
    return 0;
}

static int __init GOP1_MIU_setup(char *str)
{
    if( str != NULL )
    {
        sscanf(str,"%d",&GOP1_MIU);
    }

    return 0;
}

static int __init GOP2_MIU_setup(char *str)
{
    if( str != NULL )
    {
        sscanf(str,"%d",&GOP2_MIU);
    }

    return 0;
}

static int __init HP_setup(char *str)
{
    if( str != NULL )
    {
        sscanf(str,"%d",&HP);
    }

    return 0;
}

early_param("MS_GOP0_MIU", GOP0_MIU_setup);
early_param("MS_GOP1_MIU", GOP1_MIU_setup);
early_param("MS_GOP2_MIU", GOP2_MIU_setup);
early_param("MS_HP", HP_setup);
#if 0
static void __init miu_assignment(void)
{
     // @FIXME: richard: need the IP assignment
     _RV2(0x12F0,
             (MIUSEL_FLAG_HVD_RW                << 0x0C) |\
             (MIUSEL_FLAG_VD_MHEG5_I_R          << 0x0D)
         );

     _RV2(0x12F2,
                (MIUSEL_FLAG_OPM_R              << 0x00) |\
                (MIUSEL_FLAG_MVOP_R             << 0x01) |\
                (GOP0_MIU/*GOP0_W*/             << 0x02) |\
                (GOP0_MIU/*GOP0_R*/             << 0x03) |\
                (GOP1_MIU                       << 0x04) |\
                (GOP2_MIU                     << 0x05)

         );
    _RV2(0x12F4,
                (MIUSEL_FLAG_HVD_RVD_BBU_R      << 0x00) |\
                (MIUSEL_FLAG_RVD_MVD_RW         << 0x06) |\
                (MIUSEL_FLAG_VD_MHEG5_D_RW      << 0x09) |\
                (MIUSEL_FLAG_MOBF_RW            << 0x0B) |\
                (MIUSEL_FLAG_VD_MHEG5_QDMA_RW   << 0x0D)
         );

    //EMAC access miu1
    *((volatile unsigned int *) (0xbf202438)) |= 0x0040;        // Group 2 member max.
#if 0
    //GOP2 access miu1 patch
    *(((volatile unsigned int *) (0xbf200000+(0x1e5a<<1))) |= (0x01<<2);
    *(((volatile unsigned int *) (0xbf200000+(0x1e5a<<1))) |= (0x01<<6);
    *(((volatile unsigned int *) (0xbf200000+(0x12F0<<1))) |= (0x1<<7);
#endif
}
#endif
#define UART_REG(addr)          (*(volatile unsigned int*)((addr)))
#define EUCLID_UART_BASE        0xbf201300
#define EUCLID_UART_RX          (0*4 + EUCLID_UART_BASE)
#define EUCLID_UART_TX          (0*4 + EUCLID_UART_BASE)
#define EUCLID_UART_DLL         (0*4 + EUCLID_UART_BASE)
#define EUCLID_UART_DLM         (1*4 + EUCLID_UART_BASE)
#define EUCLID_UART_IER         (1*4 + EUCLID_UART_BASE)
#define EUCLID_UART_IIR         (2*4 + EUCLID_UART_BASE)
#define EUCLID_UART_FCR         (2*4 + EUCLID_UART_BASE)
#define EUCLID_UART_EFR         (2*4 + EUCLID_UART_BASE)
#define EUCLID_UART_LCR         (3*4 + EUCLID_UART_BASE)
#define EUCLID_UART_MCR         (4*4 + EUCLID_UART_BASE)
#define EUCLID_UART_LSR         (5*4 + EUCLID_UART_BASE)
#define EUCLID_UART_MSR         (6*4 + EUCLID_UART_BASE)
#define EUCLID_UART_SCR         (7*4 + EUCLID_UART_BASE)
#define EUCLID_UART_SCCR_RST    (8*4 + EUCLID_UART_BASE)

//Baud rate
#define EUCLID_UART_BAUD_RATE  115200
// #define EUCLID_UART_BAUD_RATE  38400

//Clock source
// 12000000, 14318180 , 123000000 , 144000000 , 160000000 , 172800000 , 108000000
#define EUCLID_UART_CLK_FREQ     172800000

//Divisor
#define EUCLID_UART_DIVISOR    (EUCLID_UART_CLK_FREQ/(16 * EUCLID_UART_BAUD_RATE))

#if 1
//UART TEST
#define UART_LSR_DR			        0x01	                            // Receiver data ready
#define UART_LSR_OE			        0x02	                            // Overrun error indicator
#define UART_LSR_PE			        0x04	                            // Parity error indicator
#define UART_LSR_FE			        0x08	                            // Frame error indicator
#define UART_LSR_BI			        0x10	                            // Break interrupt indicator
#define UART_LSR_THRE		        0x20	                            // Transmit-hold-register empty
#define UART_LSR_TEMT		        0x40	                            // Transmitter empty
int euclid_putc(int c)
{//jc_db: n2r
    unsigned char u8Reg;

    do {
        u8Reg = UART_REG(EUCLID_UART_LSR)&0xff;
        if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
        {
            break;
        }
    }while(1);

    if(c=='\n')
    {
        UART_REG(EUCLID_UART_TX)='\n';
        do {
            u8Reg = UART_REG(EUCLID_UART_LSR)&0xff;
            if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
            {
                break;
            }
        }while(1);
        UART_REG(EUCLID_UART_TX)='\r';
    }
    else
    {
        UART_REG(EUCLID_UART_TX)=c&0xffff;
    }
    do {
        u8Reg = UART_REG(EUCLID_UART_LSR)&0xff;
        if ((u8Reg & UART_LSR_THRE) == UART_LSR_THRE)
        {
            break;
        }
    }while(1);
    return 1;
}
void uart_clk_scan(void)
{
    int i;
    unsigned long clk[]={12000000, 43200000, 14318180 , 123000000 , 144000000 , 160000000 , 172800000 , 108000000, 623001600};
    for(i=0;i<sizeof(clk)/sizeof(unsigned long);i++)
    {
        unsigned short divisor= (clk[i]/(16 * EUCLID_UART_BAUD_RATE));
        //UART1 INIT:

        //Disable all UART interrupts
        UART_REG(EUCLID_UART_IER) = 0x00;

        //Reset receiver and transmitr
        UART_REG(EUCLID_UART_FCR) = 0x07;

        //Set 8 bit char, 1 stop bit, no parity
        UART_REG(EUCLID_UART_LCR) = 0x03;

        UART_REG(EUCLID_UART_LCR) = 0x83;
        UART_REG(EUCLID_UART_DLM) = (divisor >> 8) & 0x00ff;
        UART_REG(EUCLID_UART_DLL) = divisor & 0x00ff;
        UART_REG(EUCLID_UART_LCR) = 0x03;

        UART_REG(EUCLID_UART_TX) = '0'+ i;
    }
}

#endif

void pool_getc (void)
{
	  int nflag ;
    char u8Ch;
    nflag = 0 ;
    do {
        if ( ((UART_REG(EUCLID_UART_LSR) & UART_LSR_DR) == UART_LSR_DR) || nflag )
        {
            break;
        }
    } while (1);
    u8Ch = (char)UART_REG(EUCLID_UART_RX);      //get char
    euclid_putc(u8Ch);
    return ;
}

void init_chip_uart(void)
{
	  //int i ;
	  //unsigned int nDivisor0 ;
	  //unsigned int nDivisor1 ;
	  //unsigned int nDivisor2 ;
	  //unsigned int nDivisor3 ;
	  //unsigned int nDivisor4 ;
	  //unsigned int nDivisor5 ;
    //UART1 INIT:

    // select uart0 source to mips's tsp (4)
    // select uart1 source to hk51 uart1 (1)
    // select uart2 source to vd_51 uart0(2)
    // select uart3 source to aeon       (3)

    //*((volatile unsigned int*)0xbf203d54) = 0x3214;
    *((volatile unsigned int*)0xbf203d54) = 0x1234;

    *((volatile unsigned int*)0xbf001c24) = 0x00;  // switch uart from hk51 to mips

    *((volatile unsigned int*)0xbf001c24) |= 0x800 ;

    *(volatile unsigned int*)(0xbf200000+(0x0F01*4)) |= (1<<10);//BIT10; //enable reg_uart_rx_enable;

		//nDivisor1 = ((14318180/2)/(16 *  38400)) ;
		//nDivisor2 = (  (14318180)/(16 *  38400)) ;
		// nDivisor3 = (  (12000000)/(16 *  38400)) ;
		//nDivisor4 = (  (172800000)/(16 * 115200)) ;

		//nDivisor0 = nDivisor4 ;

    //UART_REG(8)=0x10;

    //Disable all UART interrupts
    UART_REG(EUCLID_UART_IER) = 0x00;

    //Reset receiver and transmitr
    UART_REG(EUCLID_UART_FCR) = 0x07;

    //Set 8 bit char, 1 stop bit, no parity
    UART_REG(EUCLID_UART_LCR) = 0x03;

    //Set baud rate

    UART_REG(EUCLID_UART_LCR) |= 0x80;
    UART_REG(EUCLID_UART_DLM) = (EUCLID_UART_DIVISOR >> 8) & 0x00ff;
    UART_REG(EUCLID_UART_DLL) = EUCLID_UART_DIVISOR & 0x00ff;
    UART_REG(EUCLID_UART_LCR) &= ~(0x80) ;

    // Enable receiver data available interrupt
    UART_REG(EUCLID_UART_IER) = 0x01;

    //UART_REG(EUCLID_UART_LCR) &= ~(0x8000);

    //UART_REG(EUCLID_UART_SCCR_RST) = 0x10 ;

#if 1
    euclid_putc('L');
    euclid_putc('o');
    euclid_putc('o');
    euclid_putc('p');
    euclid_putc('b');
    euclid_putc('a');
    euclid_putc('c');
    euclid_putc('k');
    euclid_putc('\n');
    //UART_REG(EUCLID_UART_MCR) |= 0x10 ;

#if 0
    while(1) {
    	pool_getc();
    }
#endif
#endif

#if 0
    euclid_putc('H');
    euclid_putc('E');
    euclid_putc('L');
    euclid_putc('L');
    euclid_putc('O');
    euclid_putc('1');
    euclid_putc('2');
    euclid_putc('3');
    euclid_putc('4');
    euclid_putc('5');
    euclid_putc('6');
#endif
}

void Chip_Flush_Memory(void)
{
    static unsigned char u8_4Lines[64];
    unsigned char *pu8;

    // Transfer the memory to noncache memory
    pu8 = ((unsigned char *)(((unsigned int)u8_4Lines) | 0xa0000000));

    // Flush the data from pipe and buffer in MIU
    pu8[0] = pu8[16] = pu8[32] = pu8[48] = 1;

    // Flush the data in the EC bridge buffer
    mb();
}
EXPORT_SYMBOL(Chip_Flush_Memory);

void Chip_Read_Memory(void)
{
#if 0 // it's not necessary for U3
    volatile unsigned int *pu8;
    volatile unsigned int t ;

    // Transfer the memory to noncache memory
    pu8 = ((volatile unsigned int *)0xA0380000);
    t = pu8[0] ;
    t = pu8[64] ;

    // Fixme: temporary mark this code.
#if 1
    pu8 = ((volatile unsigned int *)0xA8380000);
    t = pu8[0] ;
    t = pu8[64] ;
#endif
#endif
}
EXPORT_SYMBOL(Chip_Read_Memory);

/*
 * misc.c
 * 
 * This is a collection of several routines from gzip-1.0.3 
 * adapted for Linux.
 *
 * malloc by Hannu Savolainen 1993 and Matthias Urlichs 1994
 *
 * Modified for ARM Linux by Russell King
 *
 * Nicolas Pitre <nico@visuaide.com>  1999/04/14 :
 *  For this code to run directly from Flash, all constant variables must
 *  be marked with 'const' and all other variables initialized at run-time 
 *  only.  This way all non constant variables will end up in the bss segment,
 *  which should point to addresses in RAM and cleared to 0 on start.
 *  This allows for a much quicker boot time.
 */

unsigned int __machine_arch_type;

#define _LINUX_STRING_H_

#include <linux/compiler.h>	/* for inline */
#include <linux/types.h>	/* for size_t */
#include <linux/stddef.h>	/* for NULL */
#include <linux/linkage.h>
#include <asm/string.h>

#if defined(CONFIG_MP_PLATFORM_ARM)
#include <asm/unaligned.h>
#endif /* CONFIG_MP_PLATFORM_ARM */

static void putstr(const char *ptr);
extern void error(char *x);

#include <mach/uncompress.h>

#ifdef CONFIG_DEBUG_ICEDCC

#if defined(CONFIG_CPU_V6) || defined(CONFIG_CPU_V6K) || defined(CONFIG_CPU_V7)

static void icedcc_putc(int ch)
{
	int status, i = 0x4000000;

	do {
		if (--i < 0)
			return;

		asm volatile ("mrc p14, 0, %0, c0, c1, 0" : "=r" (status));
	} while (status & (1 << 29));

	asm("mcr p14, 0, %0, c0, c5, 0" : : "r" (ch));
}
#if defined(CONFIG_MP_PLATFORM_ARM)
#elif defined(CONFIG_CPU_V7)

static void icedcc_putc(int ch)
{
	asm(
	"wait:	mrc	p14, 0, pc, c0, c1, 0			\n\
		bcs	wait					\n\
		mcr     p14, 0, %0, c0, c5, 0			"
	: : "r" (ch));
}
#endif /* CONFIG_MP_PLATFORM_ARM */
#elif defined(CONFIG_CPU_XSCALE)

static void icedcc_putc(int ch)
{
	int status, i = 0x4000000;

	do {
		if (--i < 0)
			return;

		asm volatile ("mrc p14, 0, %0, c14, c0, 0" : "=r" (status));
	} while (status & (1 << 28));

	asm("mcr p14, 0, %0, c8, c0, 0" : : "r" (ch));
}

#else

static void icedcc_putc(int ch)
{
	int status, i = 0x4000000;

	do {
		if (--i < 0)
			return;

		asm volatile ("mrc p14, 0, %0, c0, c0, 0" : "=r" (status));
	} while (status & 2);

	asm("mcr p14, 0, %0, c1, c0, 0" : : "r" (ch));
}

#endif

#define putc(ch)	icedcc_putc(ch)
#endif

#if defined(CONFIG_MP_PLATFORM_ARM)
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



#define UART_REG(addr) *((volatile unsigned int*)(0x1F201300 + ((addr)<< 3)))

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
                //printk("W: %d\n",addr);
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

int prom_putchar(char c)
{

//       volatile int i=0;
        while ((serial_in(UART_LSR) & UART_LSR_THRE) == 0)
                ;
       // for ( i=0;i<1000;i++)
       // {
       //     serial_in(UART_LSR);
       // }
        serial_out(UART_TX, c);

        return 1;
}

#define putc(ch)	 prom_putchar(ch);
#endif /* CONFIG_MP_PLATFORM_ARM */
static void putstr(const char *ptr)
{
	char c;

	while ((c = *ptr++) != '\0') {
		if (c == '\n')
			putc('\r');
		putc(c);
	}
#ifndef CONFIG_MP_PLATFORM_ARM
	flush();
#endif /* CONFIG_MP_PLATFORM_ARM */
}


void *memcpy(void *__dest, __const void *__src, size_t __n)
{
	int i = 0;
	unsigned char *d = (unsigned char *)__dest, *s = (unsigned char *)__src;

	for (i = __n >> 3; i > 0; i--) {
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & 1 << 2) {
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & 1 << 1) {
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & 1)
		*d++ = *s++;

	return __dest;
}

/*
 * gzip declarations
 */
extern char input_data[];
extern char input_data_end[];

unsigned char *output_data;

unsigned long free_mem_ptr;
unsigned long free_mem_end_ptr;

#ifndef arch_error
#define arch_error(x)
#endif

void error(char *x)
{
	arch_error(x);

	putstr("\n\n");
	putstr(x);
	putstr("\n\n -- System halted");

	while(1);	/* Halt */
}

asmlinkage void __div0(void)
{
	error("Attempting division by 0!");
}

extern int do_decompress(u8 *input, int len, u8 *output, void (*error)(char *x));


void
decompress_kernel(unsigned long output_start, unsigned long free_mem_ptr_p,
		unsigned long free_mem_ptr_end_p,
		int arch_id)
{
	int ret;

	output_data		= (unsigned char *)output_start;
	free_mem_ptr		= free_mem_ptr_p;
	free_mem_end_ptr	= free_mem_ptr_end_p;
	__machine_arch_type	= arch_id;

	arch_decomp_setup();

	putstr("Uncompressing Linux...");
	ret = do_decompress(input_data, input_data_end - input_data,
			    output_data, error);
	if (ret)
		error("decompressor returned an error");
	else
	{
		putstr(" done, booting the kernel.\n");
#if (CONFIG_MP_DEBUG_TOOL_CHANGELIST==1)
		putstr(KERN_CL);
#endif
	}
}

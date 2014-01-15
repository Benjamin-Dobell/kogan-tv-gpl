/*
 * Copyright (C) 1999, 2000, 2004, 2005  MIPS Technologies, Inc.
 *	All rights reserved.
 *	Authors: Carsten Langgaard <carstenl@mips.com>
 *		 Maciej W. Rozycki <macro@mips.com>
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
 *
 * PROM library initialisation code.
 */
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>

#include <asm/bootinfo.h>
#include <asm/gt64120.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/cacheflush.h>
#include <asm/traps.h>

#include <asm/mips-boards/prom.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/bonito64.h>
#include <asm/mips-boards/msc01_pci.h>

#include <asm/mips-boards/malta.h>

#include <asm/setup.h>

#ifdef CONFIG_KGDB
extern int rs_kgdb_hook(int, int);
extern int rs_putDebugChar(char);
extern char rs_getDebugChar(void);
extern int saa9730_kgdb_hook(int);
extern int saa9730_putDebugChar(char);
extern char saa9730_getDebugChar(void);
#endif

int prom_argc;
int *_prom_argv, *_prom_envp;

/*
 * YAMON (32-bit PROM) pass arguments and environment as 32-bit pointer.
 * This macro take care of sign extension, if running in 64-bit mode.
 */
#define prom_envp(index) ((char *)(long)_prom_envp[(index)])

int init_debug = 0;

int mips_revision_corid;
int mips_revision_sconid;

/* Bonito64 system controller register base. */
unsigned long _pcictrl_bonito;
unsigned long _pcictrl_bonito_pcicfg;

/* GT64120 system controller register base */
unsigned long _pcictrl_gt64120;

/* MIPS System controller register base */
unsigned long _pcictrl_msc;

char *prom_getenv(char *envname)
{
	/*
	 * Return a pointer to the given environment variable.
	 * In 64-bit mode: we're using 64-bit pointers, but all pointers
	 * in the PROM structures are only 32-bit, so we need some
	 * workarounds, if we are running in 64-bit mode.
	 */
	int i, index=0;

	i = strlen(envname);

	while (prom_envp(index)) {
		if(strncmp(envname, prom_envp(index), i) == 0) {
			return(prom_envp(index+1));
		}
		index += 2;
	}

	return NULL;
}

static inline unsigned char str2hexnum(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0; /* foo */
}

static inline void str2eaddr(unsigned char *ea, unsigned char *str)
{
	int i;

	for (i = 0; i < 6; i++) {
		unsigned char num;

		if((*str == '.') || (*str == ':'))
			str++;
		num = str2hexnum(*str++) << 4;
		num |= (str2hexnum(*str++));
		ea[i] = num;
	}
}

int get_ethernet_addr(char *ethernet_addr)
{
        char *ethaddr_str;

        ethaddr_str = prom_getenv("ethaddr");
	if (!ethaddr_str) {
	        printk("ethaddr not set in boot prom\n");
		return -1;
	}
	str2eaddr(ethernet_addr, ethaddr_str);

	if (init_debug > 1) {
	        int i;
		printk("get_ethernet_addr: ");
	        for (i=0; i<5; i++)
		        printk("%02x:", (unsigned char)*(ethernet_addr+i));
		printk("%02x\n", *(ethernet_addr+i));
	}

	return 0;
}

#ifdef CONFIG_SERIAL_8250_CONSOLE
#if 0
static void __init console_config(void)
{
	char console_string[40];
	int baud = 0;
	char parity = '\0', bits = '\0', flow = '\0';
	char *s;

	if ((strstr(prom_getcmdline(), "console=")) == NULL) {
		s = prom_getenv("modetty0");
		if (s) {
			while (*s >= '0' && *s <= '9')
				baud = baud*10 + *s++ - '0';
			if (*s == ',') s++;
			if (*s) parity = *s++;
			if (*s == ',') s++;
			if (*s) bits = *s++;
			if (*s == ',') s++;
			if (*s == 'h') flow = 'r';
		}
		if (baud == 0)
			baud = 38400;
		if (parity != 'n' && parity != 'o' && parity != 'e')
			parity = 'n';
		if (bits != '7' && bits != '8')
			bits = '8';
		if (flow == '\0')
			flow = 'r';
		sprintf(console_string, " console=ttyS0,%d%c%c%c", baud, parity, bits, flow);
		strcat(prom_getcmdline(), console_string);
		pr_info("Config serial console:%s\n", console_string);
	}
}
#endif
#endif

#ifdef CONFIG_KGDB
void __init kgdb_config(void)
{
	extern int (*generic_putDebugChar)(char);
	extern char (*generic_getDebugChar)(void);
	char *argptr;
	int line, speed;

	argptr = prom_getcmdline();
	if ((argptr = strstr(argptr, "kgdb=ttyS")) != NULL) {
		argptr += strlen("kgdb=ttyS");
		if (*argptr != '0' && *argptr != '1')
			printk("KGDB: Unknown serial line /dev/ttyS%c, "
			       "falling back to /dev/ttyS1\n", *argptr);
		line = *argptr == '0' ? 0 : 1;
		printk("KGDB: Using serial line /dev/ttyS%d for session\n", line);

		speed = 0;
		if (*++argptr == ',')
		{
			int c;
			while ((c = *++argptr) && ('0' <= c && c <= '9'))
				speed = speed * 10 + c - '0';
		}
#ifdef CONFIG_MIPS_ATLAS
		if (line == 1) {
			speed = saa9730_kgdb_hook(speed);
			generic_putDebugChar = saa9730_putDebugChar;
			generic_getDebugChar = saa9730_getDebugChar;
		}
		else
#endif
		{
			speed = rs_kgdb_hook(line, speed);
			generic_putDebugChar = rs_putDebugChar;
			generic_getDebugChar = rs_getDebugChar;
		}

		pr_info("KGDB: Using serial line /dev/ttyS%d at %d for "
		        "session, please connect your debugger\n",
		        line ? 1 : 0, speed);

		{
			char *s;
			for (s = "Please connect GDB to this port\r\n"; *s; )
				generic_putDebugChar(*s++);
		}

		/* Breakpoint is invoked after interrupts are initialised */
	}
}
#endif

#if (defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10))
void __init mips_nmi_setup(void)
#else
static void __init mips_nmi_setup(void)
#endif
{
	void *base;
	extern char except_vec_nmi;

	base = cpu_has_veic ?
		(void *)(CAC_BASE + 0xa80) :
		(void *)(CAC_BASE + 0x380);

    if (cpu_has_mips_r2)
        base += (read_c0_ebase() & 0x3ffff000);

	memcpy(base, &except_vec_nmi, 0x80);
	flush_icache_range((unsigned long)base, (unsigned long)base + 0x80);
}

#if (defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10))
void __init mips_ejtag_setup(void)
#else
static void __init mips_ejtag_setup(void)
#endif
{
	void *base;
	extern char except_vec_ejtag_debug;

	base = cpu_has_veic ?
		(void *)(CAC_BASE + 0xa00) :
		(void *)(CAC_BASE + 0x300);

    if (cpu_has_mips_r2)
        base += (read_c0_ebase() & 0x3ffff000);

	memcpy(base, &except_vec_ejtag_debug, 0x80);
	flush_icache_range((unsigned long)base, (unsigned long)base + 0x80);
}

extern struct plat_smp_ops msmtc_smp_ops;

// @FIXME: Richard: what is this oging?
#if defined(CONFIG_MSTAR_OBERON) || \
    defined(CONFIG_MSTAR_EUCLID) || \
    defined(CONFIG_MSTAR_TITANIA3) || \
    defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EMERALD)
    void init_chip_uart(void);
#endif

void __init prom_init(void)
{
#if defined(CONFIG_MSTAR_OBERON) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_EMERALD)
    init_chip_uart();//without this, can't show message when use ICE
#endif

	prom_argc = fw_arg0;
	_prom_argv = (int *) fw_arg1;
	_prom_envp = (int *) fw_arg2;

    board_nmi_handler_setup = mips_nmi_setup;
    board_ejtag_handler_setup = mips_ejtag_setup;

    pr_info("\nLINUX started...\n");
#if defined(CONFIG_INITRAMFS_ROOT_UID) || \
    defined(CONFIG_MSTAR_URANUS4_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA4_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA8_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA9_BD_FPGA) || \
    defined(CONFIG_MSTAR_JANUS2_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA11_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA12_BD_FPGA) || \
    defined(CONFIG_MSTAR_TITANIA13_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER1_BD_FPGA) || \
    defined(CONFIG_MSTAR_KRONUS_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER6_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER7_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMETHYST_BD_FPGA) || \
    defined(CONFIG_MSTAR_KAISERIN_BD_FPGA) || \
    defined(CONFIG_MSTAR_AMBER5_BD_FPGA) || \
    defined(CONFIG_MSTAR_EMERALD_BD_FPGA)
    //this function may fail when use ICE
#else
    prom_init_cmdline();
#endif
    strlcpy(boot_command_line, arcs_cmdline, COMMAND_LINE_SIZE);
    pr_info("boot_command_line: %s\n", boot_command_line);
    parse_early_param();
	prom_meminit();

#ifdef CONFIG_MIPS_CMP
	register_smp_ops(&cmp_smp_ops);
#endif
#ifdef CONFIG_MIPS_MT_SMP
	register_smp_ops(&vsmp_smp_ops);
#endif
#ifdef CONFIG_MIPS_MT_SMTC
	register_smp_ops(&msmtc_smp_ops);
#endif
}

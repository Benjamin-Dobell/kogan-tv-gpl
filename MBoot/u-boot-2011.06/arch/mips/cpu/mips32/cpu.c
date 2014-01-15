/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <netdev.h>
#include <asm/mipsregs.h>
#include <asm/cacheops.h>
#include <asm/reboot.h>
// MSTAR start
#include <MsUboot.h>
// MSTAR end

#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	push					\n"	\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	pop					\n"	\
	:								\
	: "i" (op), "R" (*(unsigned char *)(addr)))

void __attribute__((weak)) _machine_restart(void)
{
}

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	puts ("resetting ...\n");
#if 1 // MSTAR
    {
        //unsigned int u32Tmp=0;
        //MDrv_MMIO_GetBASE((unsigned int *) &RIU_MAP, &u32Tmp, MS_MODULE_PM);
        //printf("%s: Using RIU_MAP: 0x%lx, at %d\n", __func__, RIU_MAP, __LINE__);
        //RIU_MAP = 0x1F000000; //TODO: fix it, when mmu is ready
    }
#endif

	udelay (50000);				/* wait 50 ms */
	disable_interrupts();
#if 1 // MSTAR
     MsSystemReset();
#else
	reset_cpu(0);
#endif

	/*NOTREACHED*/
	return 0;
}

extern void Chip_L2_cache_wback_inv( unsigned long addr, unsigned long size);
void flush_cache(ulong start_addr, ulong size)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (start_addr + size - 1) & ~(lsize - 1);

	while (1) {
		cache_op(Hit_Writeback_Inv_D, addr);
		cache_op(Hit_Invalidate_I, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
    #if defined(CONFIG_TITANIA8) || defined(CONFIG_TITANIA12) || defined(CONFIG_AMBER2) || defined(CONFIG_AMBER5) || defined(CONFIG_TITANIA9) || defined(CONFIG_JANUS2) || defined(CONFIG_TITANIA13) || defined(CONFIG_AMBER1) || defined(CONFIG_AMBER6) || defined(CONFIG_AMBER7) || defined(CONFIG_AMETHYST) || defined(CONFIG_KAISERIN) || defined(CONFIG_EMERALD)
	if(  ( 0x80000000 <= addr )   && ( addr < 0xA0000000) )
    {
        Chip_L2_cache_wback_inv( addr&0x0FFFFFFF , size);
    }
    else //  high mem, user space
    {
        Chip_L2_cache_wback_inv( 0 , 0xFFFFFFFF );
    }
    #endif
}

void flush_dcache_range(ulong start_addr, ulong stop)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (stop - 1) & ~(lsize - 1);

	while (1) {
		cache_op(Hit_Writeback_Inv_D, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void invalidate_dcache_range(ulong start_addr, ulong stop)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (stop - 1) & ~(lsize - 1);

	while (1) {
		cache_op(Hit_Invalidate_D, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void write_one_tlb(int index, u32 pagemask, u32 hi, u32 low0, u32 low1)
{
	write_c0_entrylo0(low0);
	write_c0_pagemask(pagemask);
	write_c0_entrylo1(low1);
	write_c0_entryhi(hi);
	write_c0_index(index);
	tlb_write_indexed();
}

int cpu_eth_init(bd_t *bis)
{
#ifdef CONFIG_SOC_AU1X00
	au1x00_enet_initialize(bis);
#endif
	return 0;
}
/*
 *  arch/arm/mach-vexpress/include/mach/system.h
 *
 *  Copyright (C) 2003 ARM Limited
 *  Copyright (C) 2000 Deep Blue Solutions Ltd
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
#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#define REG_PM_SLEEP_BASE           (0xFD00000 + (0x0E00 << 1))
#define REG_WDT_BASE                (0xFD00000 + (0x3000 << 1))
#define MST_XTAL_CLOCK_HZ           (12000000UL) 

#define BOOT_DELAY_MILLISECOND      100 
#define WDT_MAX_PERIOD              ((MST_XTAL_CLOCK_HZ / 1000) * BOOT_DELAY_MILLISECOND)

extern void  Chip_Flush_Cache_All(void);

static inline void arch_idle(void)
{
	/*
	 * This should do all the clock switching
	 * and wait for interrupt tricks
	 */
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
	// flush L1/L2 cache boefore reboot   
    Chip_Flush_Cache_All();

    *(volatile unsigned int *)(0xFD000000 + (0x0E5C << 1)) = 0x79;    // reg_top_sw_rst 
 
#if 0 
    // set WDT count down timer  
    *(volatile unsigned int *)(0xFD000000 + (0x3008 << 1)) = BOOT_DELAY_MILLISECOND & 0x0000FFFF;               
    *(volatile unsigned int *)(0xFD000000 + (0x300A << 1)) = BOOT_DELAY_MILLISECOND >> 16;               
#endif 
}

#endif

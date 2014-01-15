/*
 * arch/arm/mach-realview/include/mach/entry-macro.S
 *
 * Low-level IRQ helper macros for RealView platforms
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#if defined(CONFIG_MP_PLATFORM_MSTAR_LEGANCY_INTR)
 
#include <mach/hardware.h>
#include <asm/hardware/entry-macro-gic.S>

		.macro	disable_fiq
		.endm

		.macro  arch_ret_to_user, tmp1, tmp2
		.endm

#else

#include <mach/entry-macro-old.S>

#endif /* CONFIG_MP_PLATFORM_MSTAR_LEGANCY_INTR */
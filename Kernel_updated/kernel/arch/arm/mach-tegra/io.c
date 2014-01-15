/*
 * arch/arm/mach-tegra/io.c
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * Author:
 *	Colin Cross <ccross@google.com>
 *	Erik Gilling <konkers@google.com>
 *
 * Copyright (C) 2010-2011 NVIDIA Corporation
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/io.h>

#include <mach/iomap.h>
#include <asm/page.h>
#include <asm/mach/map.h>

#include "board.h"

static struct map_desc tegra_io_desc[] __initdata = {
	{
		.virtual = IO_PPSB_VIRT,
		.pfn = __phys_to_pfn(IO_PPSB_PHYS),
		.length = IO_PPSB_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = IO_APB_VIRT,
		.pfn = __phys_to_pfn(IO_APB_PHYS),
		.length = IO_APB_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = IO_CPU_VIRT,
		.pfn = __phys_to_pfn(IO_CPU_PHYS),
		.length = IO_CPU_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = IO_IRAM_VIRT,
		.pfn = __phys_to_pfn(IO_IRAM_PHYS),
		.length = IO_IRAM_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = IO_HOST1X_VIRT,
		.pfn = __phys_to_pfn(IO_HOST1X_PHYS),
		.length = IO_HOST1X_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = IO_USB_VIRT,
		.pfn = __phys_to_pfn(IO_USB_PHYS),
		.length = IO_USB_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = IO_SDMMC_VIRT,
		.pfn = __phys_to_pfn(IO_SDMMC_PHYS),
		.length = IO_SDMMC_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = IO_PPCS_VIRT,
		.pfn = __phys_to_pfn(IO_PPCS_PHYS),
		.length = IO_PPCS_SIZE,
		.type = MT_DEVICE,
	}
};

void __init tegra_map_common_io(void)
{
	iotable_init(tegra_io_desc, ARRAY_SIZE(tegra_io_desc));
}

/*
 * Intercept ioremap() requests for addresses in our fixed mapping regions.
 */
void __iomem *tegra_ioremap(unsigned long p, size_t size, unsigned int type)
{
	void __iomem *v = IO_ADDRESS(p);

	/*
	 * __arm_ioremap fails to set the domain of ioremapped memory
	 * correctly, only use it on physical memory.
	 */
	if (v == NULL) {
		if ((p >= TEGRA_DRAM_BASE &&
		     (p + size) <= (TEGRA_DRAM_BASE + TEGRA_DRAM_SIZE)) ||
		    (p >= TEGRA_NOR_FLASH_BASE &&
		     (p + size) <= (TEGRA_NOR_FLASH_BASE + TEGRA_NOR_FLASH_SIZE)) ||
		    (p >= TEGRA_PCIE_BASE &&
		     (p + size) <= (TEGRA_PCIE_BASE + TEGRA_PCIE_SIZE)))
			v = __arm_ioremap(p, size, type);
	}

	/*
	 * If the physical address was not physical memory or statically
	 * mapped, there's nothing we can do to map it safely.
	 */
	BUG_ON(v == NULL);

	return v;
}
EXPORT_SYMBOL(tegra_ioremap);

void tegra_iounmap(volatile void __iomem *addr)
{
	unsigned long virt = (unsigned long)addr;

	if (virt >= VMALLOC_START && virt < VMALLOC_END)
		__iounmap(addr);
}
EXPORT_SYMBOL(tegra_iounmap);

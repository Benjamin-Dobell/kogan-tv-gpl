/*
 * arch/arm/mach-tegra/board-p852-panel.c
 *
 * Copyright (c) 2010-2012, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/resource.h>
#include <linux/nvhost.h>
#include <linux/platform_device.h>
#include <asm/mach-types.h>
#include <mach/nvmap.h>
#include <mach/irqs.h>
#include <mach/iomap.h>
#include <mach/dc.h>
#include <mach/fb.h>

#include "board-p852.h"

#define CARVEOUT_IRAM {\
	.name = "iram",\
	.usage_mask = NVMAP_HEAP_CARVEOUT_IRAM,\
	.base = TEGRA_IRAM_BASE,\
	.size = TEGRA_IRAM_SIZE,\
	.buddy_size = 0, /* no buddy allocation for IRAM */\
}

static int p852_panel_enable(void)
{
	pr_info("%s\n", __func__);
	return 0;
}

static int p852_panel_disable(void)
{
	pr_info("%s\n", __func__);
	return 0;
}

static struct resource p852_disp_resources[] = {
	{
	 .name = "irq",
	 .start = INT_DISPLAY_GENERAL,
	 .end = INT_DISPLAY_GENERAL,
	 .flags = IORESOURCE_IRQ,
	},
	{
	 .name = "regs",
	 .start = TEGRA_DISPLAY_BASE,
	 .end = TEGRA_DISPLAY_BASE + TEGRA_DISPLAY_SIZE - 1,
	 .flags = IORESOURCE_MEM,
	},
	{
	 .name = "fbmem",
	 .start = 0,
	 .end =   0,
	 .flags = IORESOURCE_MEM,
	},
};

static struct tegra_dc_mode p852_panel_modes[] = {
/* Timings for the LG LB070WV4 panel */
	{
	 .pclk = 33230769,

	 .h_ref_to_sync = 1,
	 .v_ref_to_sync = 1,

	 .h_sync_width = 128,
	 .v_sync_width = 2,

	 .h_back_porch = 88,
	 .v_back_porch = 30,

	 .h_front_porch = 40,
	 .v_front_porch = 13,

	 .h_active = 800,
	 .v_active = 480,
	 },
};

static struct tegra_fb_data p852_fb_data = {
	.win = 0,
	.xres = 800,
	.yres = 480,
	.bits_per_pixel = 16,
};

static struct tegra_dc_out p852_disp_out = {
	.type = TEGRA_DC_OUT_RGB,

	.align = TEGRA_DC_ALIGN_MSB,
	.order = TEGRA_DC_ORDER_RED_BLUE,

	.modes = p852_panel_modes,
	.n_modes = ARRAY_SIZE(p852_panel_modes),

	.enable = p852_panel_enable,
	.disable = p852_panel_disable,
};

static struct tegra_dc_platform_data p852_disp_pdata = {
	.flags = TEGRA_DC_FLAG_ENABLED,
	.default_out = &p852_disp_out,
	.fb = &p852_fb_data,
};

static struct nvhost_device p852_disp_device = {
	.name = "tegradc",
	.id = 0,
	.resource = p852_disp_resources,
	.num_resources = ARRAY_SIZE(p852_disp_resources),
	.dev = {
		.platform_data = &p852_disp_pdata,
		},
};

static struct nvmap_platform_carveout p852_carveouts[] = {
	[0] = CARVEOUT_IRAM,
	[1] = {
	       .name = "generic-0",
	       .usage_mask = NVMAP_HEAP_CARVEOUT_GENERIC,
	       .base = 0,
	       .size = 0,
	       .buddy_size = SZ_32K,
	       },
};

static struct nvmap_platform_data p852_nvmap_data = {
	.carveouts = p852_carveouts,
	.nr_carveouts = ARRAY_SIZE(p852_carveouts),
};

static struct platform_device p852_nvmap_device = {
	.name = "tegra-nvmap",
	.id = -1,
	.dev = {
		.platform_data = &p852_nvmap_data,
		},
};

static struct platform_device *p852_gfx_devices[] __initdata = {
	&tegra_pwfm2_device,
};

int __init p852_panel_init(void)
{
	int err;
	struct resource *res;

	pr_info("%s\n", __func__);

	p852_carveouts[1].base = tegra_carveout_start;
	p852_carveouts[1].size = tegra_carveout_size;

	err = platform_device_register(&p852_nvmap_device);
	if (err)
		return err;

#ifdef CONFIG_TEGRA_GRHOST
	err = nvhost_device_register(&tegra_grhost_device);
	if (err)
		return err;
#endif

	err = platform_add_devices(p852_gfx_devices,
				   ARRAY_SIZE(p852_gfx_devices));

	res = nvhost_get_resource_byname(&p852_disp_device,
					IORESOURCE_MEM, "fbmem");

	res->start = tegra_fb_start;
	res->end = tegra_fb_start + tegra_fb_size - 1;

	if (!err)
		err = nvhost_device_register(&p852_disp_device);

	return err;
}

/*
 * drivers/video/tegra/dc/dc_priv.h
 *
 * Copyright (C) 2010 Google, Inc.
 * Author: Erik Gilling <konkers@android.com>
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

#ifndef __DRIVERS_VIDEO_TEGRA_DC_DC_PRIV_H
#define __DRIVERS_VIDEO_TEGRA_DC_DC_PRIV_H

#include <linux/io.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/completion.h>
#include <linux/switch.h>

#include <mach/dc.h>

#include "../host/dev.h"
#include "../host/host1x/host1x_syncpt.h"

#include <mach/tegra_dc_ext.h>

#define WIN_IS_TILED(win)	((win)->flags & TEGRA_WIN_FLAG_TILED)
#define WIN_IS_ENABLED(win)	((win)->flags & TEGRA_WIN_FLAG_ENABLED)

#define NEED_UPDATE_EMC_ON_EVERY_FRAME (windows_idle_detection_time == 0)

/* DDR: 8 bytes transfer per clock */
#define DDR_BW_TO_FREQ(bw) ((bw) / 8)

#if defined(CONFIG_TEGRA_EMC_TO_DDR_CLOCK)
#define EMC_BW_TO_FREQ(bw) (DDR_BW_TO_FREQ(bw) * CONFIG_TEGRA_EMC_TO_DDR_CLOCK)
#else
#define EMC_BW_TO_FREQ(bw) (DDR_BW_TO_FREQ(bw) * 2)
#endif

struct tegra_dc;

struct tegra_dc_blend {
	unsigned z[DC_N_WINDOWS];
	unsigned flags[DC_N_WINDOWS];
};

struct tegra_dc_out_ops {
	/* initialize output.  dc clocks are not on at this point */
	int (*init)(struct tegra_dc *dc);
	/* destroy output.  dc clocks are not on at this point */
	void (*destroy)(struct tegra_dc *dc);
	/* detect connected display.  can sleep.*/
	bool (*detect)(struct tegra_dc *dc);
	/* enable output.  dc clocks are on at this point */
	void (*enable)(struct tegra_dc *dc);
	/* disable output.  dc clocks are on at this point */
	void (*disable)(struct tegra_dc *dc);

	/* suspend output.  dc clocks are on at this point */
	void (*suspend)(struct tegra_dc *dc);
	/* resume output.  dc clocks are on at this point */
	void (*resume)(struct tegra_dc *dc);
};

struct tegra_dc {
	struct nvhost_device		*ndev;
	struct tegra_dc_platform_data	*pdata;

	struct resource			*base_res;
	void __iomem			*base;
	int				irq;

	struct clk			*clk;
	struct clk			*emc_clk;
	int				emc_clk_rate;
	int				new_emc_clk_rate;
	u32				shift_clk_div;

	bool				connected;
	bool				enabled;
	bool				suspended;

	struct tegra_dc_out		*out;
	struct tegra_dc_out_ops		*out_ops;
	void				*out_data;

	struct tegra_dc_mode		mode;

	struct tegra_dc_win		windows[DC_N_WINDOWS];
	struct tegra_dc_blend		blend;
	int				n_windows;

	wait_queue_head_t		wq;

	struct mutex			lock;
	struct mutex			one_shot_lock;

	struct resource			*fb_mem;
	struct tegra_fb_info		*fb;

	struct {
		u32			id;
		u32			min;
		u32			max;
	} syncpt[DC_N_WINDOWS];
	u32				vblank_syncpt;

	unsigned long			underflow_mask;
	struct work_struct		reset_work;

#ifdef CONFIG_SWITCH
	struct switch_dev		modeset_switch;
#endif

	struct completion		frame_end_complete;

	struct work_struct		vblank_work;

	struct {
		u64			underflows;
		u64			underflows_a;
		u64			underflows_b;
		u64			underflows_c;
	} stats;

	struct tegra_dc_ext		*ext;

#ifdef CONFIG_DEBUG_FS
	struct dentry			*debugdir;
#endif
	struct tegra_dc_lut		fb_lut;
	struct delayed_work		underflow_work;
	u32				one_shot_delay_ms;
	struct delayed_work		one_shot_work;
};

static inline void tegra_dc_io_start(struct tegra_dc *dc)
{
	nvhost_module_busy(nvhost_get_host(dc->ndev)->dev);
}

static inline void tegra_dc_io_end(struct tegra_dc *dc)
{
	nvhost_module_idle(nvhost_get_host(dc->ndev)->dev);
}

static inline unsigned long tegra_dc_readl(struct tegra_dc *dc,
					   unsigned long reg)
{
	BUG_ON(!nvhost_module_powered(nvhost_get_host(dc->ndev)->dev));
	return readl(dc->base + reg * 4);
}

static inline void tegra_dc_writel(struct tegra_dc *dc, unsigned long val,
				   unsigned long reg)
{
	BUG_ON(!nvhost_module_powered(nvhost_get_host(dc->ndev)->dev));
	writel(val, dc->base + reg * 4);
}

static inline void _tegra_dc_write_table(struct tegra_dc *dc, const u32 *table,
					 unsigned len)
{
	int i;

	for (i = 0; i < len; i++)
		tegra_dc_writel(dc, table[i * 2 + 1], table[i * 2]);
}

#define tegra_dc_write_table(dc, table)		\
	_tegra_dc_write_table(dc, table, ARRAY_SIZE(table) / 2)

static inline void tegra_dc_set_outdata(struct tegra_dc *dc, void *data)
{
	dc->out_data = data;
}

static inline void *tegra_dc_get_outdata(struct tegra_dc *dc)
{
	return dc->out_data;
}

static inline unsigned long tegra_dc_get_default_emc_clk_rate(
							struct tegra_dc *dc)
{
	return dc->pdata->emc_clk_rate ? dc->pdata->emc_clk_rate : ULONG_MAX;
}

void tegra_dc_setup_clk(struct tegra_dc *dc, struct clk *clk);

extern struct tegra_dc_out_ops tegra_dc_rgb_ops;
extern struct tegra_dc_out_ops tegra_dc_hdmi_ops;
extern struct tegra_dc_out_ops tegra_dc_dsi_ops;

/* defined in dc_sysfs.c, used by dc.c */
void __devexit tegra_dc_remove_sysfs(struct device *dev);
void tegra_dc_create_sysfs(struct device *dev);

/* defined in dc.c, used by dc_sysfs.c */
void tegra_dc_stats_enable(struct tegra_dc *dc, bool enable);
bool tegra_dc_stats_get(struct tegra_dc *dc);

/* defined in dc.c, used by dc_sysfs.c */
u32 tegra_dc_read_checksum_latched(struct tegra_dc *dc);
void tegra_dc_enable_crc(struct tegra_dc *dc);
void tegra_dc_disable_crc(struct tegra_dc *dc);

void tegra_dc_set_out_pin_polars(struct tegra_dc *dc,
				const struct tegra_dc_out_pin *pins,
				const unsigned int n_pins);
#endif


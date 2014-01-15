/*
 * arch/arm/mach-tegra/include/mach/tegra_asoc_pdata.h
 *
 * Copyright 2012 NVIDIA, Inc.
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

#define	HIFI_CODEC		0
#define	BASEBAND		1
#define	BT_SCO			2
#define	NUM_I2S_DEVICES		3

#define	TEGRA_DAIFMT_DSP_A		0
#define	TEGRA_DAIFMT_DSP_B		1
#define	TEGRA_DAIFMT_I2S		2
#define	TEGRA_DAIFMT_RIGHT_J		3
#define	TEGRA_DAIFMT_LEFT_J		4

struct baseband_config {
	int rate;
	int channels;
	int bit_format;
	int is_master;
};

struct tegra_asoc_platform_data {
	int gpio_spkr_en;
	int gpio_hp_det;
	int gpio_hp_mute;
	int gpio_int_mic_en;
	int gpio_ext_mic_en;
	unsigned int debounce_time_hp;
	int audio_port_id[NUM_I2S_DEVICES];
	struct baseband_config baseband_param;
};

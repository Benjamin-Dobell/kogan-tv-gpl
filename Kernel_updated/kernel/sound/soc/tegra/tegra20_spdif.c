/*
 * tegra20_spdif.c - Tegra20 SPDIF driver
 *
 * Author: Stephen Warren <swarren@nvidia.com>
 * Copyright (C) 2011 - NVIDIA, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <linux/clk.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <mach/iomap.h>
#include <mach/hdmi-audio.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include "tegra20_spdif.h"

#define DRV_NAME "tegra20-spdif"

static inline void tegra20_spdif_write(struct tegra20_spdif *spdif, u32 reg,
					u32 val)
{
#ifdef CONFIG_PM
	if (reg < TEGRA20_SPDIF_CH_STA_TX_A)
		spdif->reg_ctrl_cache[reg >> 2] = val;
	else
		spdif->reg_tx_cache[((reg - TEGRA20_SPDIF_CH_STA_TX_A) >> 2)]
			= val;
#endif
	__raw_writel(val, spdif->regs + reg);
}

static inline u32 tegra20_spdif_read(struct tegra20_spdif *spdif, u32 reg)
{
	return __raw_readl(spdif->regs + reg);
}

#ifdef CONFIG_DEBUG_FS
static int tegra20_spdif_show(struct seq_file *s, void *unused)
{
#define REG(r) { r, #r }
	static const struct {
		int offset;
		const char *name;
	} regs[] = {
		REG(TEGRA20_SPDIF_CTRL),
		REG(TEGRA20_SPDIF_STATUS),
		REG(TEGRA20_SPDIF_STROBE_CTRL),
		REG(TEGRA20_SPDIF_DATA_FIFO_CSR),
		REG(TEGRA20_SPDIF_CH_STA_RX_A),
		REG(TEGRA20_SPDIF_CH_STA_RX_B),
		REG(TEGRA20_SPDIF_CH_STA_RX_C),
		REG(TEGRA20_SPDIF_CH_STA_RX_D),
		REG(TEGRA20_SPDIF_CH_STA_RX_E),
		REG(TEGRA20_SPDIF_CH_STA_RX_F),
		REG(TEGRA20_SPDIF_CH_STA_TX_A),
		REG(TEGRA20_SPDIF_CH_STA_TX_B),
		REG(TEGRA20_SPDIF_CH_STA_TX_C),
		REG(TEGRA20_SPDIF_CH_STA_TX_D),
		REG(TEGRA20_SPDIF_CH_STA_TX_E),
		REG(TEGRA20_SPDIF_CH_STA_TX_F),
	};
#undef REG

	struct tegra20_spdif *spdif = s->private;
	int i;

	for (i = 0; i < ARRAY_SIZE(regs); i++) {
		u32 val = tegra20_spdif_read(spdif, regs[i].offset);
		seq_printf(s, "%s = %08x\n", regs[i].name, val);
	}

	return 0;
}

static int tegra20_spdif_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, tegra20_spdif_show, inode->i_private);
}

static const struct file_operations tegra20_spdif_debug_fops = {
	.open    = tegra20_spdif_debug_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

static void tegra20_spdif_debug_add(struct tegra20_spdif *spdif)
{
	spdif->debug = debugfs_create_file(DRV_NAME, S_IRUGO,
						snd_soc_debugfs_root, spdif,
						&tegra20_spdif_debug_fops);
}

static void tegra20_spdif_debug_remove(struct tegra20_spdif *spdif)
{
	if (spdif->debug)
		debugfs_remove(spdif->debug);
}
#else
static inline void tegra20_spdif_debug_add(struct tegra20_spdif *spdif)
{
}

static inline void tegra20_spdif_debug_remove(struct tegra20_spdif *spdif)
{
}
#endif

static int tegra20_spdif_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct device *dev = substream->pcm->card->dev;
	struct tegra20_spdif *spdif = snd_soc_dai_get_drvdata(dai);
	int ret, srate, spdifclock;
	u32 ch_sta[2] = {0, 0};

	spdif->reg_ctrl &= ~TEGRA20_SPDIF_CTRL_PACK;
	spdif->reg_ctrl &= ~TEGRA20_SPDIF_CTRL_BIT_MODE_MASK;
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		spdif->reg_ctrl |= TEGRA20_SPDIF_CTRL_PACK;
		spdif->reg_ctrl |= TEGRA20_SPDIF_CTRL_BIT_MODE_16BIT;
		break;
	default:
		return -EINVAL;
	}

	srate = params_rate(params);
	ch_sta[0] = tegra20_spdif_read(spdif, TEGRA20_SPDIF_CH_STA_TX_A);
	ch_sta[0] &= ~TEGRA20_SPDIF_CH_STA_TX_A_SAMP_FREQ_MASK;
	ch_sta[1] = tegra20_spdif_read(spdif, TEGRA20_SPDIF_CH_STA_TX_B);
	ch_sta[1] &= ~TEGRA20_SPDIF_CH_STA_TX_B_ORIG_SAMP_FREQ_MASK;
	switch (params_rate(params)) {
	case 32000:
		spdifclock = 4096000;
		ch_sta[0] |= TEGRA20_SPDIF_CH_STA_TX_A_SAMP_FREQ_32000;
		ch_sta[1] |= TEGRA20_SPDIF_CH_STA_TX_B_ORIG_SAMP_FREQ_32000;
		break;
	case 44100:
		spdifclock = 5644800;
		ch_sta[0] |= TEGRA20_SPDIF_CH_STA_TX_A_SAMP_FREQ_44100;
		ch_sta[1] |= TEGRA20_SPDIF_CH_STA_TX_B_ORIG_SAMP_FREQ_44100;
		break;
	case 48000:
		spdifclock = 6144000;
		ch_sta[0] |= TEGRA20_SPDIF_CH_STA_TX_A_SAMP_FREQ_48000;
		ch_sta[1] |= TEGRA20_SPDIF_CH_STA_TX_B_ORIG_SAMP_FREQ_48000;
		break;
	case 88200:
		spdifclock = 11289600;
		ch_sta[0] |= TEGRA20_SPDIF_CH_STA_TX_A_SAMP_FREQ_88200;
		ch_sta[1] |= TEGRA20_SPDIF_CH_STA_TX_B_ORIG_SAMP_FREQ_88200;
		break;
	case 96000:
		spdifclock = 12288000;
		ch_sta[0] |= TEGRA20_SPDIF_CH_STA_TX_A_SAMP_FREQ_96000;
		ch_sta[1] |= TEGRA20_SPDIF_CH_STA_TX_B_ORIG_SAMP_FREQ_96000;
		break;
	case 176400:
		spdifclock = 22579200;
		ch_sta[0] |= TEGRA20_SPDIF_CH_STA_TX_A_SAMP_FREQ_176400;
		ch_sta[1] |= TEGRA20_SPDIF_CH_STA_TX_B_ORIG_SAMP_FREQ_176400;
		break;
	case 192000:
		spdifclock = 24576000;
		ch_sta[0] |= TEGRA20_SPDIF_CH_STA_TX_A_SAMP_FREQ_192000;
		ch_sta[1] |= TEGRA20_SPDIF_CH_STA_TX_B_ORIG_SAMP_FREQ_192000;
		break;
	default:
		return -EINVAL;
	}

	ret = clk_set_rate(spdif->clk_spdif_out, spdifclock);
	if (ret) {
		dev_err(dev, "Can't set SPDIF clock rate: %d\n", ret);
		return ret;
	}

	clk_enable(spdif->clk_spdif_out);

	tegra20_spdif_write(spdif, TEGRA20_SPDIF_CH_STA_TX_A, ch_sta[0]);
	tegra20_spdif_write(spdif, TEGRA20_SPDIF_CH_STA_TX_B, ch_sta[1]);

	clk_disable(spdif->clk_spdif_out);

	ret = tegra_hdmi_setup_audio_freq_source(srate, SPDIF);
	if (ret) {
		dev_err(dev, "Can't set HDMI audio freq source: %d\n", ret);
		return ret;
	}

	return 0;
}

static void tegra20_spdif_start_playback(struct tegra20_spdif *spdif)
{
	spdif->reg_ctrl |= TEGRA20_SPDIF_CTRL_TC_EN | TEGRA20_SPDIF_CTRL_TX_EN;
	tegra20_spdif_write(spdif, TEGRA20_SPDIF_CTRL, spdif->reg_ctrl);
}

static void tegra20_spdif_stop_playback(struct tegra20_spdif *spdif)
{
	spdif->reg_ctrl &= ~(TEGRA20_SPDIF_CTRL_TX_EN |
				TEGRA20_SPDIF_CTRL_TC_EN);
	tegra20_spdif_write(spdif, TEGRA20_SPDIF_CTRL, spdif->reg_ctrl);
}

static int tegra20_spdif_trigger(struct snd_pcm_substream *substream, int cmd,
				struct snd_soc_dai *dai)
{
	struct tegra20_spdif *spdif = snd_soc_dai_get_drvdata(dai);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	case SNDRV_PCM_TRIGGER_RESUME:
		clk_enable(spdif->clk_spdif_out);
		tegra20_spdif_start_playback(spdif);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		tegra20_spdif_stop_playback(spdif);
		clk_disable(spdif->clk_spdif_out);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int tegra20_spdif_probe(struct snd_soc_dai *dai)
{
	struct tegra20_spdif *spdif = snd_soc_dai_get_drvdata(dai);
#ifdef CONFIG_PM
	int i, reg;
#endif

	dai->capture_dma_data = NULL;
	dai->playback_dma_data = &spdif->playback_dma_data;

#ifdef CONFIG_PM
	clk_enable(spdif->clk_spdif_out);

	/* populate the spdif reg cache with POR values*/
	for (i = 0; i < TEGRA20_SPDIF_CTRL_CACHE_SIZE; i++)
		spdif->reg_ctrl_cache[i] = tegra20_spdif_read(spdif, i << 2);

	for (i = 0; i < TEGRA20_SPDIF_TX_CACHE_SIZE; i++) {
		reg = (TEGRA20_SPDIF_CH_STA_TX_A) + (i << 2);
		spdif->reg_tx_cache[i] = tegra20_spdif_read(spdif, reg);
	}

	clk_disable(spdif->clk_spdif_out);

#endif

	return 0;
}

#ifdef CONFIG_PM
int tegra20_spdif_resume(struct snd_soc_dai *cpu_dai)
{
	struct tegra20_spdif *spdif = snd_soc_dai_get_drvdata(cpu_dai);
	int i, reg;

	clk_enable(spdif->clk_spdif_out);

	/*restore the spdif regs*/
	for (i = 0; i < TEGRA20_SPDIF_CTRL_CACHE_SIZE; i++)
		tegra20_spdif_write(spdif, i << 2, spdif->reg_ctrl_cache[i]);

	for (i = 0; i < TEGRA20_SPDIF_TX_CACHE_SIZE; i++) {
		reg = (TEGRA20_SPDIF_CH_STA_TX_A) + (i << 2);
		tegra20_spdif_write(spdif, reg, spdif->reg_tx_cache[i]);
	}

	clk_disable(spdif->clk_spdif_out);

	return 0;
}
#else
#define tegra20_spdif_resume NULL
#endif

static struct snd_soc_dai_ops tegra20_spdif_dai_ops = {
	.hw_params	= tegra20_spdif_hw_params,
	.trigger	= tegra20_spdif_trigger,
};

struct snd_soc_dai_driver tegra20_spdif_dai = {
	.name = DRV_NAME,
	.probe = tegra20_spdif_probe,
	.resume = tegra20_spdif_resume,
	.playback = {
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
				SNDRV_PCM_RATE_48000,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,
	},
	.ops = &tegra20_spdif_dai_ops,
};

static __devinit int tegra20_spdif_platform_probe(struct platform_device *pdev)
{
	struct tegra20_spdif *spdif;
	struct resource *mem, *memregion, *dmareq;
	int ret;
	u32 reg_val;

	spdif = kzalloc(sizeof(struct tegra20_spdif), GFP_KERNEL);
	if (!spdif) {
		dev_err(&pdev->dev, "Can't allocate tegra20_spdif\n");
		ret = -ENOMEM;
		goto exit;
	}
	dev_set_drvdata(&pdev->dev, spdif);

	spdif->clk_spdif_out = clk_get(&pdev->dev, "spdif_out");
	if (IS_ERR(spdif->clk_spdif_out)) {
		pr_err("Can't retrieve spdif clock\n");
		ret = PTR_ERR(spdif->clk_spdif_out);
		goto err_free;
	}

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(&pdev->dev, "No memory resource\n");
		ret = -ENODEV;
		goto err_clk_put;
	}

	dmareq = platform_get_resource(pdev, IORESOURCE_DMA, 0);
	if (!dmareq) {
		dev_err(&pdev->dev, "No DMA resource\n");
		ret = -ENODEV;
		goto err_clk_put;
	}

	memregion = request_mem_region(mem->start, resource_size(mem),
					DRV_NAME);
	if (!memregion) {
		dev_err(&pdev->dev, "Memory region already claimed\n");
		ret = -EBUSY;
		goto err_clk_put;
	}

	spdif->regs = ioremap(mem->start, resource_size(mem));
	if (!spdif->regs) {
		dev_err(&pdev->dev, "ioremap failed\n");
		ret = -ENOMEM;
		goto err_release;
	}

	spdif->playback_dma_data.addr = mem->start + TEGRA20_SPDIF_DATA_OUT;
	spdif->playback_dma_data.wrap = 4;
	spdif->playback_dma_data.width = 32;
	spdif->playback_dma_data.req_sel = dmareq->start;

	clk_enable(spdif->clk_spdif_out);

	reg_val = tegra20_spdif_read(spdif, TEGRA20_SPDIF_DATA_FIFO_CSR);
	reg_val &= ~TEGRA20_SPDIF_DATA_FIFO_CSR_TX_ATN_LVL_MASK;
	reg_val |= TEGRA20_SPDIF_DATA_FIFO_CSR_TX_ATN_LVL_TU4_WORD_FULL;
	tegra20_spdif_write(spdif, TEGRA20_SPDIF_DATA_FIFO_CSR, reg_val);

	clk_disable(spdif->clk_spdif_out);

	ret = snd_soc_register_dai(&pdev->dev, &tegra20_spdif_dai);
	if (ret) {
		dev_err(&pdev->dev, "Could not register DAI: %d\n", ret);
		ret = -ENOMEM;
		goto err_unmap;
	}

	tegra20_spdif_debug_add(spdif);

	return 0;

err_unmap:
	iounmap(spdif->regs);
err_release:
	release_mem_region(mem->start, resource_size(mem));
err_clk_put:
	clk_put(spdif->clk_spdif_out);
err_free:
	kfree(spdif);
exit:
	return ret;
}

static int __devexit tegra20_spdif_platform_remove(struct platform_device *pdev)
{
	struct tegra20_spdif *spdif = dev_get_drvdata(&pdev->dev);
	struct resource *res;

	snd_soc_unregister_dai(&pdev->dev);

	tegra20_spdif_debug_remove(spdif);

	iounmap(spdif->regs);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(res->start, resource_size(res));

	clk_put(spdif->clk_spdif_out);

	kfree(spdif);

	return 0;
}

static struct platform_driver tegra20_spdif_driver = {
	.driver = {
		.name = DRV_NAME,
		.owner = THIS_MODULE,
	},
	.probe = tegra20_spdif_platform_probe,
	.remove = __devexit_p(tegra20_spdif_platform_remove),
};

static int __init snd_tegra20_spdif_init(void)
{
	return platform_driver_register(&tegra20_spdif_driver);
}
module_init(snd_tegra20_spdif_init);

static void __exit snd_tegra20_spdif_exit(void)
{
	platform_driver_unregister(&tegra20_spdif_driver);
}
module_exit(snd_tegra20_spdif_exit);

MODULE_AUTHOR("Stephen Warren <swarren@nvidia.com>");
MODULE_DESCRIPTION("Tegra SPDIF ASoC driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRV_NAME);

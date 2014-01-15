/*
 * wm8994-core.c  --  Device access for Wolfson WM8994
 *
 * Copyright 2009 Wolfson Microelectronics PLC.
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/mfd/core.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>

#include <linux/mfd/wm8994/core.h>
#include <linux/mfd/wm8994/pdata.h>
#include <linux/mfd/wm8994/registers.h>

static int wm8994_read(struct wm8994 *wm8994, unsigned short reg,
		       int bytes, void *dest)
{
	return regmap_raw_read(wm8994->regmap, reg, dest, bytes);
}

/**
 * wm8994_reg_read: Read a single WM8994 register.
 *
 * @wm8994: Device to read from.
 * @reg: Register to read.
 */
int wm8994_reg_read(struct wm8994 *wm8994, unsigned short reg)
{
	unsigned int val;
	int ret;

	ret = regmap_read(wm8994->regmap, reg, &val);

	if (ret < 0)
		return ret;
	else
		return val;
}
EXPORT_SYMBOL_GPL(wm8994_reg_read);

/**
 * wm8994_bulk_read: Read multiple WM8994 registers
 *
 * @wm8994: Device to read from
 * @reg: First register
 * @count: Number of registers
 * @buf: Buffer to fill.  The data will be returned big endian.
 */
int wm8994_bulk_read(struct wm8994 *wm8994, unsigned short reg,
		     int count, u16 *buf)
{
	return regmap_bulk_read(wm8994->regmap, reg, buf, count);
}

static int wm8994_write(struct wm8994 *wm8994, unsigned short reg,
			int bytes, const void *src)
{
	return regmap_raw_write(wm8994->regmap, reg, src, bytes);
}

/**
 * wm8994_reg_write: Write a single WM8994 register.
 *
 * @wm8994: Device to write to.
 * @reg: Register to write to.
 * @val: Value to write.
 */
int wm8994_reg_write(struct wm8994 *wm8994, unsigned short reg,
		     unsigned short val)
{
	return regmap_write(wm8994->regmap, reg, val);
}
EXPORT_SYMBOL_GPL(wm8994_reg_write);

/**
 * wm8994_bulk_write: Write multiple WM8994 registers
 *
 * @wm8994: Device to write to
 * @reg: First register
 * @count: Number of registers
 * @buf: Buffer to write from.  Data must be big-endian formatted.
 */
int wm8994_bulk_write(struct wm8994 *wm8994, unsigned short reg,
		      int count, const u16 *buf)
{
	return regmap_raw_write(wm8994->regmap, reg, buf, count * sizeof(u16));
}
EXPORT_SYMBOL_GPL(wm8994_bulk_write);

/**
 * wm8994_set_bits: Set the value of a bitfield in a WM8994 register
 *
 * @wm8994: Device to write to.
 * @reg: Register to write to.
 * @mask: Mask of bits to set.
 * @val: Value to set (unshifted)
 */
int wm8994_set_bits(struct wm8994 *wm8994, unsigned short reg,
		    unsigned short mask, unsigned short val)
{
	return regmap_update_bits(wm8994->regmap, reg, mask, val);
}
EXPORT_SYMBOL_GPL(wm8994_set_bits);

static struct mfd_cell wm8994_regulator_devs[] = {
	{
		.name = "wm8994-ldo",
		.id = 1,
		.pm_runtime_no_callbacks = true,
	},
	{
		.name = "wm8994-ldo",
		.id = 2,
		.pm_runtime_no_callbacks = true,
	},
};

static struct resource wm8994_codec_resources[] = {
	{
		.start = WM8994_IRQ_TEMP_SHUT,
		.end   = WM8994_IRQ_TEMP_WARN,
		.flags = IORESOURCE_IRQ,
	},
};

static struct resource wm8994_gpio_resources[] = {
	{
		.start = WM8994_IRQ_GPIO(1),
		.end   = WM8994_IRQ_GPIO(11),
		.flags = IORESOURCE_IRQ,
	},
};

static struct mfd_cell wm8994_devs[] = {
	{
		.name = "wm8994-codec",
		.num_resources = ARRAY_SIZE(wm8994_codec_resources),
		.resources = wm8994_codec_resources,
	},

	{
		.name = "wm8994-gpio",
		.num_resources = ARRAY_SIZE(wm8994_gpio_resources),
		.resources = wm8994_gpio_resources,
		.pm_runtime_no_callbacks = true,
	},
};

/*
 * Supplies for the main bulk of CODEC; the LDO supplies are ignored
 * and should be handled via the standard regulator API supply
 * management.
 */
static const char *wm8994_main_supplies[] = {
	"DBVDD",
	"DCVDD",
	"AVDD1",
	"AVDD2",
	"CPVDD",
	"SPKVDD1",
	"SPKVDD2",
};

static const char *wm8958_main_supplies[] = {
	"DBVDD1",
	"DBVDD2",
	"DBVDD3",
	"DCVDD",
	"AVDD1",
	"AVDD2",
	"CPVDD",
	"SPKVDD1",
	"SPKVDD2",
};

#ifdef CONFIG_PM
static int wm8994_suspend(struct device *dev)
{
	struct wm8994 *wm8994 = dev_get_drvdata(dev);
	int ret;

	/* Don't actually go through with the suspend if the CODEC is
	 * still active (eg, for audio passthrough from CP. */
	ret = wm8994_reg_read(wm8994, WM8994_POWER_MANAGEMENT_1);
	if (ret < 0) {
		dev_err(dev, "Failed to read power status: %d\n", ret);
	} else if (ret & WM8994_VMID_SEL_MASK) {
		dev_dbg(dev, "CODEC still active, ignoring suspend\n");
		return 0;
	}

	/* GPIO configuration state is saved here since we may be configuring
	 * the GPIO alternate functions even if we're not using the gpiolib
	 * driver for them.
	 */
	ret = wm8994_read(wm8994, WM8994_GPIO_1, WM8994_NUM_GPIO_REGS * 2,
			  &wm8994->gpio_regs);
	if (ret < 0)
		dev_err(dev, "Failed to save GPIO registers: %d\n", ret);

	/* For similar reasons we also stash the regulator states */
	ret = wm8994_read(wm8994, WM8994_LDO_1, WM8994_NUM_LDO_REGS * 2,
			  &wm8994->ldo_regs);
	if (ret < 0)
		dev_err(dev, "Failed to save LDO registers: %d\n", ret);

	/* Explicitly put the device into reset in case regulators
	 * don't get disabled in order to ensure consistent restart.
	 */
	wm8994_reg_write(wm8994, WM8994_SOFTWARE_RESET, 0x8994);

	wm8994->suspended = true;

	ret = regulator_bulk_disable(wm8994->num_supplies,
				     wm8994->supplies);
	if (ret != 0) {
		dev_err(dev, "Failed to disable supplies: %d\n", ret);
		return ret;
	}

	return 0;
}

static int wm8994_resume(struct device *dev)
{
	struct wm8994 *wm8994 = dev_get_drvdata(dev);
	int ret, i;

	/* We may have lied to the PM core about suspending */
	if (!wm8994->suspended)
		return 0;

	ret = regulator_bulk_enable(wm8994->num_supplies,
				    wm8994->supplies);
	if (ret != 0) {
		dev_err(dev, "Failed to enable supplies: %d\n", ret);
		return ret;
	}

	/* Write register at a time as we use the cache on the CPU so store
	 * it in native endian.
	 */
	for (i = 0; i < ARRAY_SIZE(wm8994->irq_masks_cur); i++) {
		ret = wm8994_reg_write(wm8994, WM8994_INTERRUPT_STATUS_1_MASK
				       + i, wm8994->irq_masks_cur[i]);
		if (ret < 0)
			dev_err(dev, "Failed to restore interrupt masks: %d\n",
				ret);
	}

	ret = wm8994_write(wm8994, WM8994_LDO_1, WM8994_NUM_LDO_REGS * 2,
			   &wm8994->ldo_regs);
	if (ret < 0)
		dev_err(dev, "Failed to restore LDO registers: %d\n", ret);

	ret = wm8994_write(wm8994, WM8994_GPIO_1, WM8994_NUM_GPIO_REGS * 2,
			   &wm8994->gpio_regs);
	if (ret < 0)
		dev_err(dev, "Failed to restore GPIO registers: %d\n", ret);

	wm8994->suspended = false;

	return 0;
}
#endif

#ifdef CONFIG_REGULATOR
static int wm8994_ldo_in_use(struct wm8994_pdata *pdata, int ldo)
{
	struct wm8994_ldo_pdata *ldo_pdata;

	if (!pdata)
		return 0;

	ldo_pdata = &pdata->ldo[ldo];

	if (!ldo_pdata->init_data)
		return 0;

	return ldo_pdata->init_data->num_consumer_supplies != 0;
}
#else
static int wm8994_ldo_in_use(struct wm8994_pdata *pdata, int ldo)
{
	return 0;
}
#endif

static struct regmap_config wm8994_regmap_config = {
	.reg_bits = 16,
	.val_bits = 16,
};

/*
 * Instantiate the generic non-control parts of the device.
 */
static int wm8994_device_init(struct wm8994 *wm8994, int irq)
{
	struct wm8994_pdata *pdata = wm8994->dev->platform_data;
	const char *devname;
	int ret, i;

	dev_set_drvdata(wm8994->dev, wm8994);

	/* Add the on-chip regulators first for bootstrapping */
	ret = mfd_add_devices(wm8994->dev, -1,
			      wm8994_regulator_devs,
			      ARRAY_SIZE(wm8994_regulator_devs),
			      NULL, 0);
	if (ret != 0) {
		dev_err(wm8994->dev, "Failed to add children: %d\n", ret);
		goto err_regmap;
	}

	switch (wm8994->type) {
	case WM8994:
		wm8994->num_supplies = ARRAY_SIZE(wm8994_main_supplies);
		break;
	case WM8958:
		wm8994->num_supplies = ARRAY_SIZE(wm8958_main_supplies);
		break;
	default:
		BUG();
		goto err_regmap;
	}

	wm8994->supplies = kzalloc(sizeof(struct regulator_bulk_data) *
				   wm8994->num_supplies,
				   GFP_KERNEL);
	if (!wm8994->supplies) {
		ret = -ENOMEM;
		goto err_regmap;
	}

	switch (wm8994->type) {
	case WM8994:
		for (i = 0; i < ARRAY_SIZE(wm8994_main_supplies); i++)
			wm8994->supplies[i].supply = wm8994_main_supplies[i];
		break;
	case WM8958:
		for (i = 0; i < ARRAY_SIZE(wm8958_main_supplies); i++)
			wm8994->supplies[i].supply = wm8958_main_supplies[i];
		break;
	default:
		BUG();
		goto err_regmap;
	}
		
	ret = regulator_bulk_get(wm8994->dev, wm8994->num_supplies,
				 wm8994->supplies);
	if (ret != 0) {
		dev_err(wm8994->dev, "Failed to get supplies: %d\n", ret);
		goto err_supplies;
	}

	ret = regulator_bulk_enable(wm8994->num_supplies,
				    wm8994->supplies);
	if (ret != 0) {
		dev_err(wm8994->dev, "Failed to enable supplies: %d\n", ret);
		goto err_get;
	}

	ret = wm8994_reg_read(wm8994, WM8994_SOFTWARE_RESET);
	if (ret < 0) {
		dev_err(wm8994->dev, "Failed to read ID register\n");
		goto err_enable;
	}
	switch (ret) {
	case 0x8994:
		devname = "WM8994";
		if (wm8994->type != WM8994)
			dev_warn(wm8994->dev, "Device registered as type %d\n",
				 wm8994->type);
		wm8994->type = WM8994;
		break;
	case 0x8958:
		devname = "WM8958";
		if (wm8994->type != WM8958)
			dev_warn(wm8994->dev, "Device registered as type %d\n",
				 wm8994->type);
		wm8994->type = WM8958;
		break;
	default:
		dev_err(wm8994->dev, "Device is not a WM8994, ID is %x\n",
			ret);
		ret = -EINVAL;
		goto err_enable;
	}

	ret = wm8994_reg_read(wm8994, WM8994_CHIP_REVISION);
	if (ret < 0) {
		dev_err(wm8994->dev, "Failed to read revision register: %d\n",
			ret);
		goto err_enable;
	}

	switch (wm8994->type) {
	case WM8994:
		switch (ret) {
		case 0:
		case 1:
			dev_warn(wm8994->dev,
				 "revision %c not fully supported\n",
				 'A' + ret);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	dev_info(wm8994->dev, "%s revision %c\n", devname, 'A' + ret);

	if (pdata) {
		wm8994->irq_base = pdata->irq_base;
		wm8994->gpio_base = pdata->gpio_base;

		/* GPIO configuration is only applied if it's non-zero */
		for (i = 0; i < ARRAY_SIZE(pdata->gpio_defaults); i++) {
			if (pdata->gpio_defaults[i]) {
				wm8994_set_bits(wm8994, WM8994_GPIO_1 + i,
						0xffff,
						pdata->gpio_defaults[i]);
			}
		}
	}

	/* In some system designs where the regulators are not in use,
	 * we can achieve a small reduction in leakage currents by
	 * floating LDO outputs.  This bit makes no difference if the
	 * LDOs are enabled, it only affects cases where the LDOs were
	 * in operation and are then disabled.
	 */
	for (i = 0; i < WM8994_NUM_LDO_REGS; i++) {
		if (wm8994_ldo_in_use(pdata, i))
			wm8994_set_bits(wm8994, WM8994_LDO_1 + i,
					WM8994_LDO1_DISCH, WM8994_LDO1_DISCH);
		else
			wm8994_set_bits(wm8994, WM8994_LDO_1 + i,
					WM8994_LDO1_DISCH, 0);
	}

	wm8994_irq_init(wm8994);

	ret = mfd_add_devices(wm8994->dev, -1,
			      wm8994_devs, ARRAY_SIZE(wm8994_devs),
			      NULL, 0);
	if (ret != 0) {
		dev_err(wm8994->dev, "Failed to add children: %d\n", ret);
		goto err_irq;
	}

	pm_runtime_enable(wm8994->dev);
	pm_runtime_resume(wm8994->dev);

	return 0;

err_irq:
	wm8994_irq_exit(wm8994);
err_enable:
	regulator_bulk_disable(wm8994->num_supplies,
			       wm8994->supplies);
err_get:
	regulator_bulk_free(wm8994->num_supplies, wm8994->supplies);
err_supplies:
	kfree(wm8994->supplies);
err_regmap:
	regmap_exit(wm8994->regmap);
	mfd_remove_devices(wm8994->dev);
	kfree(wm8994);
	return ret;
}

static void wm8994_device_exit(struct wm8994 *wm8994)
{
	pm_runtime_disable(wm8994->dev);
	mfd_remove_devices(wm8994->dev);
	wm8994_irq_exit(wm8994);
	regulator_bulk_disable(wm8994->num_supplies,
			       wm8994->supplies);
	regulator_bulk_free(wm8994->num_supplies, wm8994->supplies);
	kfree(wm8994->supplies);
	regmap_exit(wm8994->regmap);
	kfree(wm8994);
}

static int wm8994_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct wm8994 *wm8994;
	int ret;

	wm8994 = kzalloc(sizeof(struct wm8994), GFP_KERNEL);
	if (wm8994 == NULL)
		return -ENOMEM;

	i2c_set_clientdata(i2c, wm8994);
	wm8994->dev = &i2c->dev;
	wm8994->irq = i2c->irq;
	wm8994->type = id->driver_data;

	wm8994->regmap = regmap_init_i2c(i2c, &wm8994_regmap_config);
	if (IS_ERR(wm8994->regmap)) {
		ret = PTR_ERR(wm8994->regmap);
		dev_err(wm8994->dev, "Failed to allocate register map: %d\n",
			ret);
		kfree(wm8994);
		return ret;
	}

	return wm8994_device_init(wm8994, i2c->irq);
}

static int wm8994_i2c_remove(struct i2c_client *i2c)
{
	struct wm8994 *wm8994 = i2c_get_clientdata(i2c);

	wm8994_device_exit(wm8994);

	return 0;
}

static const struct i2c_device_id wm8994_i2c_id[] = {
	{ "wm8994", WM8994 },
	{ "wm8958", WM8958 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, wm8994_i2c_id);

static UNIVERSAL_DEV_PM_OPS(wm8994_pm_ops, wm8994_suspend, wm8994_resume,
			    NULL);

static struct i2c_driver wm8994_i2c_driver = {
	.driver = {
		.name = "wm8994",
		.owner = THIS_MODULE,
		.pm = &wm8994_pm_ops,
	},
	.probe = wm8994_i2c_probe,
	.remove = wm8994_i2c_remove,
	.id_table = wm8994_i2c_id,
};

static int __init wm8994_i2c_init(void)
{
	int ret;

	ret = i2c_add_driver(&wm8994_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register wm8994 I2C driver: %d\n", ret);

	return ret;
}
module_init(wm8994_i2c_init);

static void __exit wm8994_i2c_exit(void)
{
	i2c_del_driver(&wm8994_i2c_driver);
}
module_exit(wm8994_i2c_exit);

MODULE_DESCRIPTION("Core support for the WM8994 audio CODEC");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");

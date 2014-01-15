/*
 * drivers/regulator/tps65090-regulator.c
 *
 * Regulator driver for tps65090 power management chip.
 *
 * Copyright (C) 2012 NVIDIA Corporation
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
 *
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/mfd/tps65090.h>
#include <linux/regulator/tps65090-regulator.h>

struct tps65090_regulator {
	int		id;
	/* Regulator register address.*/
	u8		reg_en_reg;
	u8		en_bit;

	/* used by regulator core */
	struct regulator_desc	desc;

	/* Device */
	struct device		*dev;
};


static inline struct device *to_tps65090_dev(struct regulator_dev *rdev)
{
	return rdev_get_dev(rdev)->parent->parent;
}
static int tps65090_reg_is_enabled(struct regulator_dev *rdev)
{
	struct tps65090_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_tps65090_dev(rdev);
	uint8_t control;
	int ret;

	ret = tps65090_read(parent, ri->reg_en_reg, &control);
	if (ret < 0) {
		dev_err(&rdev->dev, "Error in reading reg 0x%x\n",
			ri->reg_en_reg);
		return ret;
	}
	return (((control >> ri->en_bit) & 1) == 1);
}

static int tps65090_reg_enable(struct regulator_dev *rdev)
{
	struct tps65090_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_tps65090_dev(rdev);
	int ret;

	ret = tps65090_set_bits(parent, ri->reg_en_reg, ri->en_bit);
	if (ret < 0)
		dev_err(&rdev->dev, "Error in updating reg 0x%x\n",
			ri->reg_en_reg);
	return ret;
}

static int tps65090_reg_disable(struct regulator_dev *rdev)
{
	struct tps65090_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_tps65090_dev(rdev);
	int ret;

	ret = tps65090_clr_bits(parent, ri->reg_en_reg, ri->en_bit);
	if (ret < 0)
		dev_err(&rdev->dev, "Error in updating reg 0x%x\n",
			ri->reg_en_reg);

	return ret;
}

static struct regulator_ops tps65090_ops = {
	.enable		= tps65090_reg_enable,
	.disable	= tps65090_reg_disable,
	.is_enabled	= tps65090_reg_is_enabled,
};

#define tps65090_REG(_id, _en_reg, _en_bit, _ops)	\
{							\
	.reg_en_reg	= _en_reg,			\
	.en_bit		= _en_bit,			\
	.id		= TPS65090_ID_##_id,		\
	.desc = {					\
		.name = tps65090_rails(_id),		\
		.id = TPS65090_ID_##_id,		\
		.ops = &_ops,				\
		.type = REGULATOR_VOLTAGE,		\
		.owner = THIS_MODULE,			\
	},						\
}

static struct tps65090_regulator TPS65090_regulator[] = {
	tps65090_REG(DCDC1, 12, 0, tps65090_ops),
	tps65090_REG(DCDC2, 13, 0, tps65090_ops),
	tps65090_REG(DCDC3, 14, 0, tps65090_ops),
	tps65090_REG(FET1,  15, 0, tps65090_ops),
	tps65090_REG(FET2,  16, 0, tps65090_ops),
	tps65090_REG(FET3,  17, 0, tps65090_ops),
	tps65090_REG(FET4,  18, 0, tps65090_ops),
	tps65090_REG(FET5,  19, 0, tps65090_ops),
	tps65090_REG(FET6,  20, 0, tps65090_ops),
	tps65090_REG(FET7,  21, 0, tps65090_ops),
};
static inline struct tps65090_regulator *find_regulator_info(int id)
{
	struct tps65090_regulator *ri;
	int i;

	for (i = 0; i < ARRAY_SIZE(TPS65090_regulator); i++) {
		ri = &TPS65090_regulator[i];
		if (ri->desc.id == id)
			return ri;
	}
	return NULL;
}
static int __devinit tps65090_regulator_probe(struct platform_device *pdev)
{
	struct tps65090_regulator *ri = NULL;
	struct regulator_dev *rdev;
	struct tps65090_regulator_platform_data *tps_pdata;
	int id = pdev->id;

	dev_dbg(&pdev->dev, "Probing regulator %d\n", id);

	ri = find_regulator_info(id);
	if (ri == NULL) {
		dev_err(&pdev->dev, "invalid regulator ID specified\n");
		return -EINVAL;
	}
	tps_pdata = pdev->dev.platform_data;
	ri->dev = &pdev->dev;

	rdev = regulator_register(&ri->desc, &pdev->dev,
				&tps_pdata->regulator, ri);
	if (IS_ERR_OR_NULL(rdev)) {
		dev_err(&pdev->dev, "failed to register regulator %s\n",
				ri->desc.name);
		return PTR_ERR(rdev);
	}

	platform_set_drvdata(pdev, rdev);
	return 0;
}

static int __devexit tps65090_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	regulator_unregister(rdev);
	return 0;
}

static struct platform_driver tps65090_regulator_driver = {
	.driver	= {
		.name	= "tps65090-regulator",
		.owner	= THIS_MODULE,
	},
	.probe		= tps65090_regulator_probe,
	.remove		= __devexit_p(tps65090_regulator_remove),
};

static int __init tps65090_regulator_init(void)
{
	return platform_driver_register(&tps65090_regulator_driver);
}
subsys_initcall(tps65090_regulator_init);

static void __exit tps65090_regulator_exit(void)
{
	platform_driver_unregister(&tps65090_regulator_driver);
}
module_exit(tps65090_regulator_exit);

MODULE_DESCRIPTION("tps65090 regulator driver");
MODULE_ALIAS("platform:tps65090-regulator");
MODULE_LICENSE("GPL");

/*
 * Copyright (C) 2010 Google, Inc.
 *
 * Author:
 *	Erik Gilling <konkers@google.com>
 *
 * Copyright (C) 2010, NVIDIA Corporation
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
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/io.h>

#include <asm/mach/irq.h>

#include <mach/iomap.h>
#include <mach/irqs.h>

#define HOST1X_SYNC_OFFSET 0x3000
#define HOST1X_SYNC_SIZE 0x800
enum {
	HOST1X_SYNC_SYNCPT_THRESH_CPU0_INT_STATUS = 0x40,
	HOST1X_SYNC_SYNCPT_THRESH_INT_DISABLE = 0x60
};

static void syncpt_thresh_mask(struct irq_data *data)
{
	(void)data;
}

static void syncpt_thresh_unmask(struct irq_data *data)
{
	(void)data;
}

static void syncpt_thresh_cascade(unsigned int irq, struct irq_desc *desc)
{
	void __iomem *sync_regs = irq_desc_get_handler_data(desc);
	unsigned long reg;
	int id;
	struct irq_chip *chip = irq_desc_get_chip(desc);

	chained_irq_enter(chip, desc);

	reg = readl(sync_regs + HOST1X_SYNC_SYNCPT_THRESH_CPU0_INT_STATUS);

	for_each_set_bit(id, &reg, 32)
		generic_handle_irq(id + INT_SYNCPT_THRESH_BASE);

	chained_irq_exit(chip, desc);
}

static struct irq_chip syncpt_thresh_irq = {
	.name		= "syncpt",
	.irq_mask	= syncpt_thresh_mask,
	.irq_unmask	= syncpt_thresh_unmask
};

static int __init syncpt_init_irq(void)
{
	void __iomem *sync_regs;
	unsigned int i;
	int irq;

	sync_regs = ioremap(TEGRA_HOST1X_BASE + HOST1X_SYNC_OFFSET,
			HOST1X_SYNC_SIZE);
	BUG_ON(!sync_regs);

	writel(0xffffffffUL,
		sync_regs + HOST1X_SYNC_SYNCPT_THRESH_INT_DISABLE);
	writel(0xffffffffUL,
		sync_regs + HOST1X_SYNC_SYNCPT_THRESH_CPU0_INT_STATUS);

	for (i = 0; i < INT_SYNCPT_THRESH_NR; i++) {
		irq = INT_SYNCPT_THRESH_BASE + i;
		irq_set_chip_and_handler(irq, &syncpt_thresh_irq,
			handle_simple_irq);
		irq_set_chip_data(irq, sync_regs);
		set_irq_flags(irq, IRQF_VALID);
	}
	irq_set_chained_handler(INT_HOST1X_MPCORE_SYNCPT,
		syncpt_thresh_cascade);
	irq_set_handler_data(INT_HOST1X_MPCORE_SYNCPT, sync_regs);

	return 0;
}

core_initcall(syncpt_init_irq);

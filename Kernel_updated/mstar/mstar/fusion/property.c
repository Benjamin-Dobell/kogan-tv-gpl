/*
 *	Fusion Kernel Module
 *
 *	(c) Copyright 2002-2003  Convergence GmbH
 *
 *      Written by Denis Oliver Kropp <dok@directfb.org>
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#ifdef HAVE_LINUX_CONFIG_H
#include <linux/config.h>
#endif
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
#include <linux/smp_lock.h>
#endif
#include <linux/sched.h>
#include <linux/proc_fs.h>

#ifndef yield
#define yield schedule
#endif

#include <linux/fusion.h>

#include "fusiondev.h"
#include "entries.h"
#include "fusionee.h"
#include "list.h"
#include "property.h"

typedef enum {
	FUSION_PROPERTY_AVAILABLE = 0,
	FUSION_PROPERTY_LEASED,
	FUSION_PROPERTY_PURCHASED
} FusionPropertyState;

typedef struct {
	FusionEntry entry;

	FusionPropertyState state;
	int fusion_id;		/* non-zero if leased/purchased */
	unsigned long purchase_stamp;
	int lock_pid;
	int count;		/* lock counter */
} FusionProperty;

static void
fusion_property_print(FusionEntry * entry, void *ctx, struct seq_file *p)
{
	FusionProperty *property = (FusionProperty *) entry;

	if (property->state != FUSION_PROPERTY_AVAILABLE) {
		seq_printf(p, "%s by 0x%08x (%d) %dx\n",
			   property->state ==
			   FUSION_PROPERTY_LEASED ? "leased" : "purchased",
			   property->fusion_id, property->lock_pid,
			   property->count);
		return;
	}

	seq_printf(p, "\n");
}

FUSION_ENTRY_CLASS(FusionProperty, property, NULL, NULL, fusion_property_print)

/******************************************************************************/
int fusion_property_init(FusionDev * dev)
{
	fusion_entries_init(&dev->properties, &property_class, dev);

	fusion_entries_create_proc_entry(dev, "properties", &dev->properties);

	return 0;
}

void fusion_property_deinit(FusionDev * dev)
{
	remove_proc_entry("properties", dev->proc_dir);

	fusion_entries_deinit(&dev->properties);
}

/******************************************************************************/

int fusion_property_new(FusionDev * dev, int *ret_id)
{
	return fusion_entry_create(&dev->properties, ret_id, NULL);
}

int fusion_property_lease(FusionDev * dev, int id, int fusion_id)
{
	int ret;
	FusionProperty *property;
	long timeout = -1;

	dev->stat.property_lease_purchase++;

	ret = fusion_property_lock(&dev->properties, id, false, &property);
	if (ret)
		return ret;

	while (true) {
		switch (property->state) {
		case FUSION_PROPERTY_AVAILABLE:
			property->state = FUSION_PROPERTY_LEASED;
			property->fusion_id = fusion_id;
			property->lock_pid = current->pid;
			property->count = 1;

			fusion_property_unlock(property);
			return 0;

		case FUSION_PROPERTY_LEASED:
			if (property->lock_pid == current->pid) {
				property->count++;

				fusion_property_unlock(property);
				return 0;
			}

			ret = fusion_property_wait(property, NULL);
			if (ret)
				return ret;

			break;

		case FUSION_PROPERTY_PURCHASED:
			if (property->lock_pid == current->pid) {
				fusion_property_unlock(property);
				return -EIO;
			}

			if (timeout == -1) {
				if (jiffies - property->purchase_stamp >
				    HZ / 10) {
					fusion_property_unlock(property);
					return -EAGAIN;
				}

				timeout = HZ / 10;
			}

			ret = fusion_property_wait(property, &timeout);
			if (ret)
				return ret;

			break;

		default:
			BUG();
		}
	}

	BUG();

	/* won't reach this */
	return -1;
}

int fusion_property_purchase(FusionDev * dev, int id, int fusion_id)
{
	int ret;
	FusionProperty *property;
	signed long timeout = -1;

	dev->stat.property_lease_purchase++;

	ret = fusion_property_lock(&dev->properties, id, false, &property);
	if (ret)
		return ret;

	while (true) {
		switch (property->state) {
		case FUSION_PROPERTY_AVAILABLE:
			property->state = FUSION_PROPERTY_PURCHASED;
			property->fusion_id = fusion_id;
			property->purchase_stamp = jiffies;
			property->lock_pid = current->pid;
			property->count = 1;

			fusion_property_notify(property, true);

			fusion_property_unlock(property);
			return 0;

		case FUSION_PROPERTY_LEASED:
			if (property->lock_pid == current->pid) {
				fusion_property_unlock(property);
				return -EIO;
			}

			ret = fusion_property_wait(property, NULL);
			if (ret)
				return ret;

			break;

		case FUSION_PROPERTY_PURCHASED:
			if (property->lock_pid == current->pid) {
				property->count++;

				fusion_property_unlock(property);
				return 0;
			}

			if (timeout == -1) {
				if (jiffies - property->purchase_stamp > HZ) {
					fusion_property_unlock(property);
					return -EAGAIN;
				}

				timeout = HZ;
			}

			ret = fusion_property_wait(property, &timeout);
			if (ret)
				return ret;

			break;

		default:
			BUG();
		}
	}

	BUG();

	/* won't reach this */
	return -1;
}

int fusion_property_cede(FusionDev * dev, int id, int fusion_id)
{
	int ret;
	FusionProperty *property;
	bool purchased;

	dev->stat.property_cede++;

	ret = fusion_property_lock(&dev->properties, id, false, &property);
	if (ret)
		return ret;

	if (property->lock_pid != current->pid) {
		fusion_property_unlock(property);
		return -EIO;
	}

	if (--property->count) {
		fusion_property_unlock(property);
		return 0;
	}

	purchased = (property->state == FUSION_PROPERTY_PURCHASED);

	property->state = FUSION_PROPERTY_AVAILABLE;
	property->fusion_id = 0;
	property->lock_pid = 0;

	fusion_property_notify(property, true);

	fusion_property_unlock(property);

	if (purchased)
		yield();

	return 0;
}

int fusion_property_holdup(FusionDev * dev, int id, Fusionee * fusionee)
{
	int ret;
	FusionProperty *property;
	FusionID fusion_id = fusionee_id(fusionee);

	if (fusion_id > 1)
		return -EPERM;

	ret = fusion_property_lock(&dev->properties, id, false, &property);
	if (ret)
		return ret;

	if (property->state == FUSION_PROPERTY_PURCHASED) {
		if (property->fusion_id == fusion_id) {
			fusion_property_unlock(property);
			return -EIO;
		}

		fusionee_kill(dev, fusionee, property->fusion_id, SIGKILL, -1);
	}

	fusion_property_unlock(property);

	return 0;
}

int fusion_property_destroy(FusionDev * dev, int id)
{
	return fusion_entry_destroy(&dev->properties, id);
}

void fusion_property_cede_all(FusionDev * dev, int fusion_id)
{
	FusionLink *l;

	down(&dev->properties.lock);

	fusion_list_foreach(l, dev->properties.list) {
		FusionProperty *property = (FusionProperty *) l;

		down(&property->entry.lock);

		if (property->fusion_id == fusion_id) {
			property->state = FUSION_PROPERTY_AVAILABLE;
			property->fusion_id = 0;
			property->lock_pid = 0;

			wake_up_interruptible_all(&property->entry.wait);
		}

		up(&property->entry.lock);
	}

	up(&dev->properties.lock);
}

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

#include <linux/fusion.h>

#include "fusiondev.h"
#include "list.h"
#include "call.h"
#include "ref.h"

typedef struct __Fusion_FusionRef FusionRef;

typedef struct {
	FusionLink link;
	FusionID fusion_id;
	int refs;
} LocalRef;

typedef struct {
	FusionLink link;
	FusionRef *ref;
} Inheritor;

struct __Fusion_FusionRef {
	FusionEntry entry;

	int global;
	int local;

	int locked;		/* non-zero fusion id of lock owner */

	bool watched;		/* true if watch has been installed */
	int call_id;		/* id of call registered with a watch */
	int call_arg;		/* optional call parameter */

	FusionRef *inherited;
	FusionLink *inheritors;

	FusionLink *local_refs;
};

/**********************************************************************************************************************/

static int add_local(FusionRef * ref, FusionID fusion_id, int add);
static void clear_local(FusionDev * dev, FusionRef * ref, FusionID fusion_id);
static int fork_local(FusionDev * dev, FusionRef * ref, FusionID fusion_id,
		      FusionID from_id);
static void free_all_local(FusionRef * ref);

static int propagate_local(FusionDev * dev, FusionRef * ref, int diff);

static void notify_ref(FusionDev * dev, FusionRef * ref);

static int add_inheritor(FusionRef * ref, FusionRef * from);
static void remove_inheritor(FusionRef * ref, FusionRef * from);
static void drop_inheritors(FusionDev * dev, FusionRef * ref);

/**********************************************************************************************************************/

static void fusion_ref_destruct(FusionEntry * entry, void *ctx)
{
	FusionRef *ref = (FusionRef *) entry;
	FusionDev *dev = (FusionDev *) ctx;

	drop_inheritors(dev, ref);

	if (ref->inherited)
		remove_inheritor(ref, ref->inherited);

	free_all_local(ref);
}

static void fusion_ref_print(FusionEntry * entry, void *ctx, struct seq_file *p)
{
	FusionRef *ref = (FusionRef *) entry;

	if (ref->locked) {
		seq_printf(p, "%2d %2d (locked by %d)\n", ref->global,
			   ref->local, ref->locked);
		return;
	}

	seq_printf(p, "%2d %2d\n", ref->global, ref->local);
}

FUSION_ENTRY_CLASS(FusionRef, ref, NULL, fusion_ref_destruct, fusion_ref_print);

/**********************************************************************************************************************/

int fusion_ref_init(FusionDev * dev)
{
	fusion_entries_init(&dev->ref, &ref_class, dev);

	fusion_entries_create_proc_entry(dev, "refs", &dev->ref);

	return 0;
}

void fusion_ref_deinit(FusionDev * dev)
{
	remove_proc_entry("refs", dev->proc_dir);

	fusion_entries_deinit(&dev->ref);
}

/**********************************************************************************************************************/

int fusion_ref_new(FusionDev * dev, int *ret_id)
{
	return fusion_entry_create(&dev->ref, ret_id, NULL);
}

int fusion_ref_up(FusionDev * dev, int id, FusionID fusion_id)
{
	int ret;
	FusionRef *ref;

	ret = fusion_ref_lock(&dev->ref, id, true, &ref);
	if (ret)
		return ret;

	dev->stat.ref_up++;

	if (ref->locked) {
		ret = -EAGAIN;
		goto out;
	}

	if (fusion_id) {
		ret = add_local(ref, fusion_id, 1);
		if (ret)
			goto out;

		ret = propagate_local(dev, ref, 1);
	} else
		ref->global ++;

      out:
	fusion_ref_unlock(ref);
	up(&dev->ref.lock);

	return ret;
}

int fusion_ref_down(FusionDev * dev, int id, FusionID fusion_id)
{
	int ret;
	FusionRef *ref;

	ret = fusion_ref_lock(&dev->ref, id, true, &ref);
	if (ret)
		return ret;

	dev->stat.ref_down++;

	if (ref->locked) {
		ret = -EAGAIN;
		goto out;
	}

	if (fusion_id) {
		ret = -EIO;
		if (!ref->local)
			goto out;

		ret = add_local(ref, fusion_id, -1);
		if (ret)
			goto out;

		ret = propagate_local(dev, ref, -1);
	} else {
		if (!ref->global) {
			ret = -EIO;
			goto out;
		}

		ref->global --;

		if (ref->local + ref->global == 0)
			notify_ref(dev, ref);
	}

      out:
	fusion_ref_unlock(ref);
	up(&dev->ref.lock);

	return ret;
}

int fusion_ref_zero_lock(FusionDev * dev, int id, FusionID fusion_id)
{
	int ret;
	FusionRef *ref;

	ret = fusion_ref_lock(&dev->ref, id, false, &ref);
	if (ret)
		return ret;

	while (true) {
		if (ref->watched) {
			fusion_ref_unlock(ref);
			return -EACCES;
		}

		if (ref->locked) {
			fusion_ref_unlock(ref);
			return ref->locked == fusion_id ? -EIO : -EAGAIN;
		}

		if (ref->global ||ref->local) {
			ret = fusion_ref_wait(ref, NULL);
			if (ret)
				return ret;
		} else
			break;
	}

	ref->locked = fusion_id;

	fusion_ref_unlock(ref);

	return 0;
}

int fusion_ref_zero_trylock(FusionDev * dev, int id, FusionID fusion_id)
{
	int ret;
	FusionRef *ref;

	ret = fusion_ref_lock(&dev->ref, id, false, &ref);
	if (ret)
		return ret;

	if (ref->locked) {
		fusion_ref_unlock(ref);
		return ref->locked == fusion_id ? -EIO : -EAGAIN;
	}

	if (ref->global ||ref->local)
		ret = -ETOOMANYREFS;
	else
		ref->locked = fusion_id;

	fusion_ref_unlock(ref);

	return ret;
}

int fusion_ref_zero_unlock(FusionDev * dev, int id, FusionID fusion_id)
{
	int ret;
	FusionRef *ref;

	ret = fusion_ref_lock(&dev->ref, id, false, &ref);
	if (ret)
		return ret;

	if (ref->locked != fusion_id) {
		fusion_ref_unlock(ref);
		return -EIO;
	}

	ref->locked = 0;

	fusion_ref_unlock(ref);

	return 0;
}

int fusion_ref_stat(FusionDev * dev, int id, int *refs)
{
	int ret;
	FusionRef *ref;

	ret = fusion_ref_lock(&dev->ref, id, false, &ref);
	if (ret)
		return ret;

	*refs = ref->global +ref->local;

	fusion_ref_unlock(ref);

	return 0;
}

int fusion_ref_watch(FusionDev * dev, int id, int call_id, int call_arg)
{
	int ret;
	FusionRef *ref;

	ret = fusion_ref_lock(&dev->ref, id, false, &ref);
	if (ret)
		return ret;

	if (ref->entry.pid != current->pid) {
		fusion_ref_unlock(ref);
		return -EACCES;
	}

	if (ref->global +ref->local == 0) {
		fusion_ref_unlock(ref);
		return -EIO;
	}

	if (ref->watched) {
		fusion_ref_unlock(ref);
		return -EBUSY;
	}

	ref->watched = true;
	ref->call_id = call_id;
	ref->call_arg = call_arg;

	fusion_ref_notify(ref, true);

	fusion_ref_unlock(ref);

	return 0;
}

int fusion_ref_inherit(FusionDev * dev, int id, int from_id)
{
	int ret;
	FusionRef *ref;
	FusionRef *from = NULL;

	ret = fusion_ref_lock(&dev->ref, id, true, &ref);
	if (ret)
		return ret;

	ret = -EBUSY;
	if (ref->inherited)
		goto out;

	ret = -EINVAL;
	fusion_list_foreach(from, dev->ref.list) {
		if (from->entry.id == from_id) {
			if (down_interruptible(&from->entry.lock)) {
				ret = -EINTR;
				from = NULL;
			}

			break;
		}
	}
	if (!from)
		goto out;

	ret = add_inheritor(ref, from);
	if (ret)
		goto out;

	ret = propagate_local(dev, ref, from->local);
	if (ret)
		goto out;

	ref->inherited = from;

      out:
	if (from)
		up(&from->entry.lock);

	fusion_ref_unlock(ref);
	up(&dev->ref.lock);

	return ret;
}

int fusion_ref_destroy(FusionDev * dev, int id)
{
	return fusion_entry_destroy(&dev->ref, id);
}

void fusion_ref_clear_all_local(FusionDev * dev, FusionID fusion_id)
{
	FusionRef *ref;

	down(&dev->ref.lock);

	fusion_list_foreach(ref, dev->ref.list)
	    clear_local(dev, ref, fusion_id);

	up(&dev->ref.lock);
}

int
fusion_ref_fork_all_local(FusionDev * dev, FusionID fusion_id, FusionID from_id)
{
	FusionRef *ref;
	int ret = 0;

	down(&dev->ref.lock);

	fusion_list_foreach(ref, dev->ref.list) {
		ret = fork_local(dev, ref, fusion_id, from_id);
		if (ret)
			break;
	}

	up(&dev->ref.lock);

	return ret;
}

/**********************************************************************************************************************/

static int add_local(FusionRef * ref, FusionID fusion_id, int add)
{
	FusionLink *l;
	LocalRef *local;

	fusion_list_foreach(l, ref->local_refs) {
		local = (LocalRef *) l;

		if (local->fusion_id == fusion_id) {
			fusion_list_move_to_front(&ref->local_refs, l);

			if (local->refs + add < 0)
				return -EIO;

			local->refs += add;
			return 0;
		}
	}

	/* Can only create local node if value is positive. */
	if (add <= 0)
		return -EIO;

	local = kmalloc(sizeof(LocalRef), GFP_KERNEL);
	if (!local)
		return -ENOMEM;

	local->fusion_id = fusion_id;
	local->refs = add;

	fusion_list_prepend(&ref->local_refs, &local->link);

	return 0;
}

static void clear_local(FusionDev * dev, FusionRef * ref, FusionID fusion_id)
{
	FusionLink *l;

	down(&ref->entry.lock);

	if (ref->locked == fusion_id) {
		ref->locked = 0;
		wake_up_interruptible_all(&ref->entry.wait);
	}

	fusion_list_foreach(l, ref->local_refs) {
		LocalRef *local = (LocalRef *) l;

		if (local->fusion_id == fusion_id) {
			if (local->refs)
				propagate_local(dev, ref, -local->refs);

			fusion_list_remove(&ref->local_refs, l);

			kfree(l);
			break;
		}
	}

	up(&ref->entry.lock);
}

static int
fork_local(FusionDev * dev, FusionRef * ref, FusionID fusion_id,
	   FusionID from_id)
{
	FusionLink *l;
	int ret = 0;

	down(&ref->entry.lock);

	fusion_list_foreach(l, ref->local_refs) {
		LocalRef *local = (LocalRef *) l;

		if (local->fusion_id == from_id) {
			if (local->refs) {
				 //local->refs++;

                    //create another local ref for the forked fusionee
                    LocalRef *addlocal;
                    addlocal = kmalloc(sizeof(LocalRef), GFP_KERNEL);
                    if (!addlocal)
                        return -ENOMEM;

                    addlocal->fusion_id = fusion_id;
                    addlocal->refs = local->refs;
                    ref->local ++;


                    fusion_list_prepend(&ref->local_refs, &addlocal->link);
			}
			break;
		}
	}

	up(&ref->entry.lock);

	return ret;
}

static void free_all_local(FusionRef * ref)
{
	FusionLink *l = ref->local_refs;

	while (l) {
		FusionLink *next = l->next;

		kfree(l);

		l = next;
	}

	ref->local_refs = NULL;
}

static void notify_ref(FusionDev * dev, FusionRef * ref)
{
	if (ref->watched) {
		FusionCallExecute execute;

		execute.call_id = ref->call_id;
		execute.call_arg = ref->call_arg;
		execute.call_ptr = NULL;

		fusion_call_execute(dev, 0, &execute);
	} else
		wake_up_interruptible_all(&ref->entry.wait);
}

static int propagate_local(FusionDev * dev, FusionRef * ref, int diff)
{
	FusionLink *l;

	/* Recurse into inheritors. */
	fusion_list_foreach(l, ref->inheritors) {
		FusionRef *inheritor = ((Inheritor *) l)->ref;

		if (down_interruptible(&inheritor->entry.lock)) {
			printk(KERN_ERR
			       "fusion_ref: propagate_local() interrupted!\n");
			/* return -EINTR; */
		}

		propagate_local(dev, inheritor, diff);

		up(&inheritor->entry.lock);
	}

	/* Apply difference. */
	ref->local += diff;

	/* Notify zero count. */
	if (ref->local + ref->global == 0)
		notify_ref(dev, ref);

	return 0;
}

static int add_inheritor(FusionRef * ref, FusionRef * from)
{
	Inheritor *inheritor;

	inheritor = kmalloc(sizeof(Inheritor), GFP_KERNEL);
	if (!inheritor)
		return -ENOMEM;

	inheritor->ref = ref;

	fusion_list_prepend(&from->inheritors, &inheritor->link);

	return 0;
}

static void remove_inheritor(FusionRef * ref, FusionRef * from)
{
	FusionLink *l;

	down(&from->entry.lock);

	fusion_list_foreach(l, from->inheritors) {
		Inheritor *inheritor = (Inheritor *) l;

		if (inheritor->ref == ref) {
			fusion_list_remove(&from->inheritors, &inheritor->link);

			kfree(l);
			break;
		}
	}

	up(&from->entry.lock);
}

static void drop_inheritors(FusionDev * dev, FusionRef * ref)
{
	FusionLink *l = ref->inheritors;

	while (l) {
		FusionLink *next = l->next;
		FusionRef *inheritor = ((Inheritor *) l)->ref;

		if (down_interruptible(&inheritor->entry.lock)) {
			printk(KERN_ERR
			       "fusion_ref: drop_inheritors() interrupted!\n");
			/* return; */
		}

		propagate_local(dev, inheritor, -ref->local);

		inheritor->inherited = NULL;

		up(&inheritor->entry.lock);

		kfree(l);

		l = next;
	}

	ref->inheritors = NULL;
}

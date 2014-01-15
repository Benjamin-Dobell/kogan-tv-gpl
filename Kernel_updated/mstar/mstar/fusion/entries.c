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
#include <linux/time.h>
#include <linux/version.h>

#include <linux/fusion.h>

#include "fusiondev.h"
#include "entries.h"

struct timeval now;

void
fusion_entries_init(FusionEntries * entries,
		    FusionEntryClass * class, void *ctx)
{
	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(class != NULL);
	FUSION_ASSERT(class->object_size >= sizeof(FusionEntry));

	memset(entries, 0, sizeof(FusionEntries));

	entries->class = class;
	entries->ctx = ctx;

	init_MUTEX(&entries->lock);
}

void fusion_entries_deinit(FusionEntries * entries)
{
	FusionLink *tmp;
	FusionEntry *entry;
	FusionEntryClass *class;

	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(entries->class != NULL);

	class = entries->class;

	down(&entries->lock);

	fusion_list_foreach_safe(entry, tmp, entries->list) {
		if (class->Destroy)
			class->Destroy(entry, entries->ctx);

		kfree(entry);
	}

	up(&entries->lock);
}

/* reading PROC entries */

static void *fusion_entries_seq_start(struct seq_file *f, loff_t * pos)
{
	int i = *pos;

	FusionEntry *entry;
	FusionEntries *entries;
	FusionEntryClass *class;

	entries = f->private;
	entry = (void *)(entries->list);

	while (i && entry) {
		entry = (void *)(entry->link.next);
		i--;
	}

	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(entries->class != NULL);

	if (down_interruptible(&entries->lock))
		return NULL;

	class = entries->class;
	if (!class->Print)
		return NULL;

	do_gettimeofday(&now);

	return entry;
}

static void *fusion_entries_seq_next(struct seq_file *f, void *v, loff_t * pos)
{
	FusionEntry *entry = v;

	(*pos)++;
	return entry->link.next;
}

static void fusion_entries_seq_stop(struct seq_file *f, void *v)
{
	FusionEntries *entries;

	entries = f->private;
	(void)v;

	up(&entries->lock);
}

int fusion_entries_show(struct seq_file *p, void *v)
{
	FusionEntry *entry;
	FusionEntryClass *class;

	entry = v;

	class = entry->entries->class;

	if (entry->last_lock.tv_sec) {
		int diff = ((now.tv_sec - entry->last_lock.tv_sec) * 1000 +
			    (now.tv_usec - entry->last_lock.tv_usec) / 1000);

		if (diff < 1000) {
			seq_printf(p, "%3d  ms  ", diff);
		} else if (diff < 1000000) {
			seq_printf(p, "%3d.%d s  ", diff / 1000,
				   (diff % 1000) / 100);
		} else {
			diff = (now.tv_sec - entry->last_lock.tv_sec +
				(now.tv_usec -
				 entry->last_lock.tv_usec) / 1000000);

			seq_printf(p, "%3d.%d h  ", diff / 3600,
				   (diff % 3600) / 360);
		}
	} else
		seq_printf(p, "  -.-    ");

	seq_printf(p, "(%5d) 0x%08x  ", entry->pid, entry->id);
	seq_printf(p, "%-24s  ", entry->name[0] ? entry->name : "");

	class->Print(entry, entry->entries->ctx, p);

	return 0;
}

static const struct seq_operations fusion_entries_seq_ops = {
	.start = fusion_entries_seq_start,
	.next  = fusion_entries_seq_next,
	.stop  = fusion_entries_seq_stop,
	.show  = fusion_entries_show
};

static int fusion_entries_open(struct inode *inode, struct file *file)
{
	struct seq_file *sf;
	int ret;

	ret = seq_open(file, &fusion_entries_seq_ops);
	if (ret < 0)
		return ret;

	sf = file->private_data;
	sf->private = PDE(inode)->data;

	return 0;
}

static const struct file_operations proc_fusion_entries_operations = {
	.open    = fusion_entries_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release,
};

void fusion_entries_create_proc_entry(FusionDev * dev, const char *name,
				      FusionEntries * data)
{
	struct proc_dir_entry *pde;

	pde = create_proc_entry(name, 0, dev->proc_dir);
	if (pde) {
		pde->proc_fops = &proc_fusion_entries_operations;
		pde->data = data;
	}
}

int fusion_entry_create(FusionEntries * entries, int *ret_id, void *create_ctx)
{
	int ret;
	FusionEntry *entry;
	FusionEntryClass *class;

	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(entries->class != NULL);
	FUSION_ASSERT(ret_id != NULL);

	class = entries->class;

	entry = kmalloc(class->object_size, GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	memset(entry, 0, class->object_size);

	if (down_interruptible(&entries->lock)) {
		kfree(entry);
		return -EINTR;
	}

	entry->entries = entries;
	entry->id = ++entries->ids;
	entry->pid = current->pid;

	init_MUTEX(&entry->lock);

	init_waitqueue_head(&entry->wait);

	if (class->Init) {
		ret = class->Init(entry, entries->ctx, create_ctx);
		if (ret) {
			up(&entries->lock);
			kfree(entry);
			return ret;
		}
	}

	fusion_list_prepend(&entries->list, &entry->link);

	up(&entries->lock);

	*ret_id = entry->id;

	return 0;
}

int fusion_entry_destroy(FusionEntries * entries, int id)
{
	FusionEntry *entry;
	FusionEntryClass *class;

	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(entries->class != NULL);

	class = entries->class;

	/* Lock entries. */
	if (down_interruptible(&entries->lock))
		return -EINTR;

	/* Lookup the entry. */
	fusion_list_foreach(entry, entries->list) {
		if (entry->id == id)
			break;
	}

	/* Check if no entry was found. */
	if (!entry) {
		up(&entries->lock);
		return -EINVAL;
	}

	/* Lock the entry. */
	if (down_interruptible(&entry->lock)) {
		up(&entries->lock);
		return -EINTR;
	}

	/* Destroy it now. */
	fusion_entry_destroy_locked(entries, entry);

	/* Unlock entries. */
	up(&entries->lock);

	return 0;
}

void fusion_entry_destroy_locked(FusionEntries * entries, FusionEntry * entry)
{
	FusionEntryClass *class;

	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(entries->class != NULL);

	class = entries->class;

	/* Remove the entry from the list. */
	fusion_list_remove(&entries->list, &entry->link);

	/* Wake up any waiting process. */
	wake_up_interruptible_all(&entry->wait);

	/* Call the destroy function. */
	if (class->Destroy)
		class->Destroy(entry, entries->ctx);

	/* Unlock the entry. */
	up(&entry->lock);

	/* Deallocate the entry. */
	kfree(entry);
}

int fusion_entry_set_info(FusionEntries * entries, const FusionEntryInfo * info)
{
	int ret;
	FusionEntry *entry;

	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(info != NULL);

	ret = fusion_entry_lock(entries, info->id, false, &entry);
	if (ret)
		return ret;

	snprintf(entry->name, FUSION_ENTRY_INFO_NAME_LENGTH, info->name);

	fusion_entry_unlock(entry);

	return 0;
}

int fusion_entry_get_info(FusionEntries * entries, FusionEntryInfo * info)
{
	int ret;
	FusionEntry *entry;

	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(info != NULL);

	ret = fusion_entry_lock(entries, info->id, false, &entry);
	if (ret)
		return ret;

	snprintf(info->name, FUSION_ENTRY_INFO_NAME_LENGTH, entry->name);

	fusion_entry_unlock(entry);

	return 0;
}

int
fusion_entry_lock(FusionEntries * entries,
		  int id, bool keep_entries_lock, FusionEntry ** ret_entry)
{
	FusionEntry *entry;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
	struct timespec xtime;
#endif

	FUSION_ASSERT(entries != NULL);
	FUSION_ASSERT(ret_entry != NULL);

	/* Lock entries. */
	if (down_interruptible(&entries->lock))
		return -EINTR;

	/* Lookup the entry. */
	fusion_list_foreach(entry, entries->list) {
		if (entry->id == id)
			break;
	}

	/* Check if no entry was found. */
	if (!entry) {
		up(&entries->lock);
		return -EINVAL;
	}

	FUSION_ASSUME(entry->lock_pid != current->pid);

	/* Move the entry to the front of all entries. */
	fusion_list_move_to_front(&entries->list, &entry->link);

	/* Lock the entry. */
	if (down_interruptible(&entry->lock)) {
		up(&entries->lock);
		return -EINTR;
	}

	/* Mark as locked. */
	entry->lock_pid = current->pid;

	/* Keep timestamp, but use the slightly
	   inexact version to avoid performance impacts. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) && defined _STRUCT_TIMESPEC
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
	xtime = current_kernel_time();
#endif
	entry->last_lock.tv_sec = xtime.tv_sec;
	entry->last_lock.tv_usec = xtime.tv_nsec / 1000;
#else
	entry->last_lock = xtime;
#endif

	/* Unlock entries. */
	if (!keep_entries_lock)
		up(&entries->lock);

	/* Return the locked entry. */
	*ret_entry = entry;

	return 0;
}

void fusion_entry_unlock(FusionEntry * entry)
{
	FUSION_ASSERT(entry != NULL);
	FUSION_ASSUME(entry->lock_pid == current->pid);

	entry->lock_pid = 0;

	/* Unlock the entry. */
	up(&entry->lock);
}

int fusion_entry_wait(FusionEntry * entry, long *timeout)
{
	int ret;
	int id;
	FusionEntries *entries;
	FusionEntry *entry2;

	FUSION_ASSERT(entry != NULL);
	FUSION_ASSERT(entry->entries != NULL);
	FUSION_ASSUME(entry->lock_pid == current->pid);

	id = entry->id;
	entries = entry->entries;

	entry->waiters++;

	entry->lock_pid = 0;
	fusion_sleep_on(&entry->wait, &entry->lock, timeout);

	entry->waiters--;

	if (signal_pending(current))
		return -EINTR;

	if (timeout && !*timeout)
		return -ETIMEDOUT;

	ret = fusion_entry_lock(entries, id, false, &entry2);
	switch (ret) {
	case -EINVAL:
		return -EIDRM;

	case 0:
		if (entry != entry2)
			BUG();
	}

	return ret;
}

void fusion_entry_notify(FusionEntry * entry, bool all)
{
	FUSION_ASSERT(entry != NULL);
	FUSION_ASSUME(entry->lock_pid == current->pid);

	if (all)
		wake_up_interruptible_all(&entry->wait);
	else
		wake_up_interruptible(&entry->wait);
}

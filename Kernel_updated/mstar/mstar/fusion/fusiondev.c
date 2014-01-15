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

#include <linux/version.h>
#include <linux/module.h>
#ifdef HAVE_LINUX_CONFIG_H
#include <linux/config.h>
#endif
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
#include <linux/devfs_fs_kernel.h>
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 19)
#include <linux/page-flags.h>
#include <linux/mm.h>
#endif

#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 2)
#include <linux/device.h>
#endif

#include <linux/sched.h>
#include <linux/fusion.h>

#include "call.h"
#include "fusiondev.h"
#include "fusionee.h"
#include "property.h"
#include "reactor.h"
#include "ref.h"
#include "skirmish.h"
#include "shmpool.h"

#if 0
#define DEBUG(x...)  printk (KERN_DEBUG "Fusion: " x)
#else
#define DEBUG(x...)  do {} while (0)
#endif

#ifndef FUSION_MAJOR
#define FUSION_MAJOR 237
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Oliver Kropp <dok@directfb.org>");

struct proc_dir_entry *proc_fusion_dir;

#define NUM_MINORS 8

static FusionDev *fusion_devs[NUM_MINORS] = { 0 };

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
static DEFINE_SEMAPHORE(devs_lock);
#else
static DECLARE_MUTEX(devs_lock);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
static devfs_handle_t devfs_handles[NUM_MINORS];
static inline unsigned iminor(struct inode *inode)
{
	return MINOR(inode->i_rdev);
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 2)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
static struct class *fusion_class;
#else
static struct class_simple *fusion_class;
#endif
#endif

/******************************************************************************/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
void
fusion_sleep_on(wait_queue_head_t * q, struct semaphore *lock,
		signed long *timeout)
{
	DEFINE_WAIT(wait);

	prepare_to_wait(q, &wait, TASK_INTERRUPTIBLE);

	up(lock);

	if (timeout)
		*timeout = schedule_timeout(*timeout);
	else
		schedule();

	finish_wait(q, &wait);
}
#else
void
fusion_sleep_on(wait_queue_head_t * q, struct semaphore *lock,
		signed long *timeout)
{
	wait_queue_t wait;

	init_waitqueue_entry(&wait, current);

	current->state = TASK_INTERRUPTIBLE;

	write_lock(&q->lock);
	__add_wait_queue(q, &wait);
	write_unlock(&q->lock);

	up(lock);

	if (timeout)
		*timeout = schedule_timeout(*timeout);
	else
		schedule();

	write_lock(&q->lock);
	__remove_wait_queue(q, &wait);
	write_unlock(&q->lock);
}
#endif

/******************************************************************************/

static int
fusiondev_stat_read_proc(char *buf, char **start, off_t offset,
			 int len, int *eof, void *private)
{
	FusionDev *dev = private;
	int written = 0;

	if ((dev->api.major != 0) || (dev->api.minor != 0))
		written +=
		    sprintf(buf, "Fusion API:%d.%d\n", dev->api.major,
			    dev->api.minor);

	written += snprintf(buf + written, offset + len - written,
			    "lease/purchase   cede      attach     detach   dispatch      "
			    "ref up   ref down  prevail/swoop dismiss\n");
	if (written < offset) {
		offset -= written;
		written = 0;
	}

	if (written < len) {
		written += snprintf(buf + written, offset + len - written,
				    "%10d %10d  %10d %10d %10d  %10d %10d  %10d %10d\n",
				    dev->stat.property_lease_purchase,
				    dev->stat.property_cede,
				    dev->stat.reactor_attach,
				    dev->stat.reactor_detach,
				    dev->stat.reactor_dispatch,
				    dev->stat.ref_up,
				    dev->stat.ref_down,
				    dev->stat.skirmish_prevail_swoop,
				    dev->stat.skirmish_dismiss);
		if (written < offset) {
			offset -= written;
			written = 0;
		}
	}

	*start = buf + offset;
	written -= offset;
	if (written > len) {
		*eof = 0;
		return len;
	}

	*eof = 1;
	return (written < 0) ? 0 : written;
}

/******************************************************************************/

static int fusiondev_init(FusionDev * dev)
{
	int ret;

	init_MUTEX(&dev->enter_lock);
	init_waitqueue_head(&dev->enter_wait);

	ret = fusionee_init(dev);
	if (ret)
		goto error_fusionee;

	ret = fusion_ref_init(dev);
	if (ret)
		goto error_ref;

	ret = fusion_skirmish_init(dev);
	if (ret)
		goto error_skirmish;

	ret = fusion_property_init(dev);
	if (ret)
		goto error_property;

	ret = fusion_reactor_init(dev);
	if (ret)
		goto error_reactor;

	ret = fusion_shmpool_init(dev);
	if (ret)
		goto error_shmpool;

	ret = fusion_call_init(dev);
	if (ret)
		goto error_call;

	create_proc_read_entry("stat", 0, dev->proc_dir,
			       fusiondev_stat_read_proc, dev);

	return 0;

      error_call:
	fusion_shmpool_deinit(dev);

      error_shmpool:
	fusion_reactor_deinit(dev);

      error_reactor:
	fusion_property_deinit(dev);

      error_property:
	fusion_skirmish_deinit(dev);

      error_skirmish:
	fusion_ref_deinit(dev);

      error_ref:
	fusionee_deinit(dev);

      error_fusionee:
	return ret;
}

static void fusiondev_deinit(FusionDev * dev)
{
	remove_proc_entry("stat", dev->proc_dir);

	fusion_call_deinit(dev);
	fusion_shmpool_deinit(dev);
	fusion_reactor_deinit(dev);
	fusion_property_deinit(dev);
	fusion_skirmish_deinit(dev);
	fusion_ref_deinit(dev);
	fusionee_deinit(dev);

	if (dev->shared_area) {
		ClearPageReserved(virt_to_page((void *)dev->shared_area));
		free_page(dev->shared_area);
	}
}

/******************************************************************************/

static int fusion_open(struct inode *inode, struct file *file)
{
	int ret;
	Fusionee *fusionee;
	int minor = iminor(inode);

	DEBUG("fusion_open( %p, %d )\n", file, atomic_read(&file->f_count));

	if (down_interruptible(&devs_lock))
		return -EINTR;

	if (!fusion_devs[minor]) {
		char buf[4];

		fusion_devs[minor] = kmalloc(sizeof(FusionDev), GFP_KERNEL);
		if (!fusion_devs[minor]) {
			up(&devs_lock);
			return -ENOMEM;
		}

		memset(fusion_devs[minor], 0, sizeof(FusionDev));

		snprintf(buf, 4, "%d", minor);

		fusion_devs[minor]->proc_dir = proc_mkdir(buf, proc_fusion_dir);
		fusion_devs[minor]->index = minor;

		ret = fusiondev_init(fusion_devs[minor]);
		if (ret) {
			remove_proc_entry(buf, proc_fusion_dir);

			kfree(fusion_devs[minor]);
			fusion_devs[minor] = NULL;

			up(&devs_lock);

			return ret;
		}
	} else if (file->f_flags & O_EXCL) {
		if (fusion_devs[minor]->fusionee.last_id) {
			up(&devs_lock);
			return -EBUSY;
		}
	}

	ret =
	    fusionee_new(fusion_devs[minor], !!(file->f_flags & O_APPEND),
			 &fusionee);
	if (ret) {
		if (!fusion_devs[minor]->refs) {
			fusiondev_deinit(fusion_devs[minor]);

			remove_proc_entry(fusion_devs[minor]->proc_dir->name,
					  proc_fusion_dir);

			kfree(fusion_devs[minor]);
			fusion_devs[minor] = NULL;
		}

		up(&devs_lock);

		return ret;
	}

	fusion_devs[minor]->refs++;

	up(&devs_lock);

	file->private_data = fusionee;

	return 0;
}

static int fusion_release(struct inode *inode, struct file *file)
{
	int minor = iminor(inode);
	Fusionee *fusionee = file->private_data;

	DEBUG("fusion_release( %p, %d )\n", file, atomic_read(&file->f_count));

	fusionee_destroy(fusion_devs[minor], fusionee);

	down(&devs_lock);

	if (!--fusion_devs[minor]->refs) {
		fusiondev_deinit(fusion_devs[minor]);

		remove_proc_entry(fusion_devs[minor]->proc_dir->name,
				  proc_fusion_dir);

		kfree(fusion_devs[minor]);
		fusion_devs[minor] = NULL;
	}

	up(&devs_lock);

	return 0;
}

static int
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
fusion_flush(struct file *file, fl_owner_t id)
#else
fusion_flush(struct file *file)
#endif
{
	Fusionee *fusionee = file->private_data;
	FusionDev *dev = fusion_devs[iminor(file->f_dentry->d_inode)];

	(void)fusionee;

	DEBUG("fusion_flush( %p, %d, 0x%08x %d )\n", file,
	      atomic_read(&file->f_count), fusionee_id(fusionee), current->pid);

	if (current->flags & PF_EXITING)
		fusion_skirmish_dismiss_all_from_pid(dev, current->pid);

	return 0;
}

static ssize_t
fusion_read(struct file *file, char *buf, size_t count, loff_t * ppos)
{
	Fusionee *fusionee = file->private_data;
	FusionDev *dev = fusion_devs[iminor(file->f_dentry->d_inode)];

	DEBUG("fusion_read( %p, %d, %d )\n", file, atomic_read(&file->f_count),
	      count);

	return fusionee_get_messages(dev, fusionee, buf, count,
				     !(file->f_flags & O_NONBLOCK));
}

static unsigned int fusion_poll(struct file *file, poll_table * wait)
{
	Fusionee *fusionee = file->private_data;
	FusionDev *dev = fusion_devs[iminor(file->f_dentry->d_inode)];

	DEBUG("fusion_poll( %p, %d )\n", file, atomic_read(&file->f_count));

	return fusionee_poll(dev, fusionee, file, wait);
}

static int
lounge_ioctl(struct file *file, FusionDev * dev, Fusionee * fusionee,
	     unsigned int cmd, unsigned long arg)
{
	int ret;
	FusionEnter enter;
	FusionKill kill;
	FusionEntryInfo info;
	FusionFork fork = { 0 };

	switch (_IOC_NR(cmd)) {
	case _IOC_NR(FUSION_ENTER):
		if (copy_from_user(&enter, (FusionEnter *) arg, sizeof(enter)))
			return -EFAULT;

		ret = fusionee_enter(dev, &enter, fusionee);
		if (ret)
			return ret;

		if (copy_to_user((FusionEnter *) arg, &enter, sizeof(enter)))
			return -EFAULT;

		return 0;

	case _IOC_NR(FUSION_UNBLOCK):
		if (fusionee_id(fusionee) != FUSION_ID_MASTER)
			return -EPERM;

		if (down_interruptible(&dev->enter_lock))
			return -EINTR;

		dev->enter_ok = 1;

		wake_up_interruptible_all(&dev->enter_wait);

		up(&dev->enter_lock);

		return 0;

	case _IOC_NR(FUSION_KILL):
		if (copy_from_user(&kill, (FusionKill *) arg, sizeof(kill)))
			return -EFAULT;

		return fusionee_kill(dev, fusionee,
				     kill.fusion_id, kill.signal,
				     kill.timeout_ms);

	case _IOC_NR(FUSION_ENTRY_SET_INFO):
		if (copy_from_user
		    (&info, (FusionEntryInfo *) arg, sizeof(info)))
			return -EFAULT;

		switch (info.type) {
		case FT_SKIRMISH:
			return fusion_entry_set_info(&dev->skirmish, &info);

		case FT_PROPERTY:
			return fusion_entry_set_info(&dev->properties, &info);

		case FT_REACTOR:
			return fusion_entry_set_info(&dev->reactor, &info);

		case FT_REF:
			return fusion_entry_set_info(&dev->ref, &info);

		case FT_SHMPOOL:
			return fusion_entry_set_info(&dev->shmpool, &info);

		default:
			return -ENOSYS;
		}

	case _IOC_NR(FUSION_ENTRY_GET_INFO):
		if (copy_from_user
		    (&info, (FusionEntryInfo *) arg, sizeof(info)))
			return -EFAULT;

		switch (info.type) {
		case FT_SKIRMISH:
			ret = fusion_entry_get_info(&dev->skirmish, &info);
			break;

		case FT_PROPERTY:
			ret = fusion_entry_get_info(&dev->properties, &info);
			break;

		case FT_REACTOR:
			ret = fusion_entry_get_info(&dev->reactor, &info);
			break;

		case FT_REF:
			ret = fusion_entry_get_info(&dev->ref, &info);
			break;

		case FT_SHMPOOL:
			ret = fusion_entry_get_info(&dev->shmpool, &info);
			break;

		default:
			return -ENOSYS;
		}

		if (ret)
			return ret;

		if (copy_to_user((FusionEntryInfo *) arg, &info, sizeof(info)))
			return -EFAULT;

		return 0;

	case _IOC_NR(FUSION_FORK):
		if (copy_from_user(&fork, (FusionFork *) arg, sizeof(fork)))
			return -EFAULT;

		ret = fusionee_fork(dev, &fork, fusionee);
		if (ret)
			return ret;

		if (copy_to_user((FusionFork *) arg, &fork, sizeof(fork)))
			return -EFAULT;

		return 0;
	}

	return -ENOSYS;
}

static int
messaging_ioctl(FusionDev * dev, Fusionee * fusionee,
		unsigned int cmd, unsigned long arg)
{
	FusionSendMessage send;

	switch (_IOC_NR(cmd)) {
	case _IOC_NR(FUSION_SEND_MESSAGE):
		if (copy_from_user
		    (&send, (FusionSendMessage *) arg, sizeof(send)))
			return -EFAULT;

		if (send.msg_size <= 0)
			return -EINVAL;

		/* message data > 64k should be stored in shared memory */
		if (send.msg_size > 0x10000)
			return -EMSGSIZE;

		return fusionee_send_message(dev, fusionee, send.fusion_id,
					     FMT_SEND, send.msg_id,
					     send.msg_channel, send.msg_size,
					     send.msg_data, NULL, NULL, 0);
	}

	return -ENOSYS;
}

static int
call_ioctl(FusionDev * dev, Fusionee * fusionee,
	   unsigned int cmd, unsigned long arg)
{
	int id;
	int ret;
	FusionCallNew call;
	FusionCallExecute execute;
	FusionCallReturn call_ret;
	FusionID fusion_id = fusionee_id(fusionee);

	switch (_IOC_NR(cmd)) {
	case _IOC_NR(FUSION_CALL_NEW):
		if (copy_from_user(&call, (FusionCallNew *) arg, sizeof(call)))
			return -EFAULT;

		ret = fusion_call_new(dev, fusion_id, &call);
		if (ret)
			return ret;

		if (put_user(call.call_id, (int *)arg)) {
			fusion_call_destroy(dev, fusion_id, call.call_id);
			return -EFAULT;
		}
		return 0;

	case _IOC_NR(FUSION_CALL_EXECUTE):
		if (copy_from_user
		    (&execute, (FusionCallExecute *) arg, sizeof(execute)))
			return -EFAULT;

		ret = fusion_call_execute(dev, fusionee, &execute);
		if (ret)
			return ret;

		if (put_user(execute.ret_val, (int *)arg))
			return -EFAULT;
		return 0;

	case _IOC_NR(FUSION_CALL_RETURN):
		if (copy_from_user
		    (&call_ret, (FusionCallReturn *) arg, sizeof(call_ret)))
			return -EFAULT;

		return fusion_call_return(dev, fusion_id, &call_ret);

	case _IOC_NR(FUSION_CALL_DESTROY):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_call_destroy(dev, fusion_id, id);
	}

	return -ENOSYS;
}

static int
ref_ioctl(FusionDev * dev, Fusionee * fusionee,
	  unsigned int cmd, unsigned long arg)
{
	int id;
	int ret;
	int refs;
	FusionRefWatch watch;
	FusionRefInherit inherit;
	FusionID fusion_id = fusionee_id(fusionee);

	switch (_IOC_NR(cmd)) {
	case _IOC_NR(FUSION_REF_NEW):
		ret = fusion_ref_new(dev, &id);
		if (ret)
			return ret;

		if (put_user(id, (int *)arg)) {
			fusion_ref_destroy(dev, id);
			return -EFAULT;
		}
		return 0;

	case _IOC_NR(FUSION_REF_UP):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_ref_up(dev, id, fusion_id);

	case _IOC_NR(FUSION_REF_UP_GLOBAL):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_ref_up(dev, id, 0);

	case _IOC_NR(FUSION_REF_DOWN):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_ref_down(dev, id, fusion_id);

	case _IOC_NR(FUSION_REF_DOWN_GLOBAL):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_ref_down(dev, id, 0);

	case _IOC_NR(FUSION_REF_ZERO_LOCK):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_ref_zero_lock(dev, id, fusion_id);

	case _IOC_NR(FUSION_REF_ZERO_TRYLOCK):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_ref_zero_trylock(dev, id, fusion_id);

	case _IOC_NR(FUSION_REF_UNLOCK):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_ref_zero_unlock(dev, id, fusion_id);

	case _IOC_NR(FUSION_REF_STAT):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		ret = fusion_ref_stat(dev, id, &refs);
		if (ret)
			return ret;

		return refs;

	case _IOC_NR(FUSION_REF_WATCH):
		if (copy_from_user
		    (&watch, (FusionRefWatch *) arg, sizeof(watch)))
			return -EFAULT;

		return fusion_ref_watch(dev, watch.id, watch.call_id,
					watch.call_arg);

	case _IOC_NR(FUSION_REF_INHERIT):
		if (copy_from_user
		    (&inherit, (FusionRefInherit *) arg, sizeof(inherit)))
			return -EFAULT;

		return fusion_ref_inherit(dev, inherit.id, inherit.from);

	case _IOC_NR(FUSION_REF_DESTROY):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_ref_destroy(dev, id);
	}

	return -ENOSYS;
}

static int
skirmish_ioctl(FusionDev * dev, Fusionee * fusionee,
	       unsigned int cmd, unsigned long arg)
{
	int id;
	int ret;
	int lock_count;
	FusionSkirmishWait wait;
	FusionID fusion_id = fusionee_id(fusionee);

	switch (_IOC_NR(cmd)) {
	case _IOC_NR(FUSION_SKIRMISH_NEW):
		ret = fusion_skirmish_new(dev, &id);
		if (ret)
			return ret;

		if (put_user(id, (int *)arg)) {
			fusion_skirmish_destroy(dev, id);
			return -EFAULT;
		}
		return 0;

	case _IOC_NR(FUSION_SKIRMISH_PREVAIL):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_skirmish_prevail(dev, id, fusion_id);

	case _IOC_NR(FUSION_SKIRMISH_SWOOP):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_skirmish_swoop(dev, id, fusion_id);

	case _IOC_NR(FUSION_SKIRMISH_DISMISS):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_skirmish_dismiss(dev, id, fusion_id);

	case _IOC_NR(FUSION_SKIRMISH_DESTROY):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_skirmish_destroy(dev, id);

	case _IOC_NR(FUSION_SKIRMISH_LOCK_COUNT):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		ret =
		    fusion_skirmish_lock_count(dev, id, fusion_id, &lock_count);
		if (put_user(lock_count, ((int *)arg) + 1))
			return -EFAULT;

		return ret;

	case _IOC_NR(FUSION_SKIRMISH_WAIT):
		if (copy_from_user
		    (&wait, (FusionSkirmishWait *) arg, sizeof(wait)))
			return -EFAULT;

		ret = fusion_skirmish_wait_(dev, &wait, fusion_id);
		if (copy_to_user
		    ((FusionSkirmishWait *) arg, &wait, sizeof(wait)))
			return -EFAULT;

		return ret;

	case _IOC_NR(FUSION_SKIRMISH_NOTIFY):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_skirmish_notify_(dev, id, fusion_id);
	}

	return -ENOSYS;
}

static int
property_ioctl(FusionDev * dev, Fusionee * fusionee,
	       unsigned int cmd, unsigned long arg)
{
	int id;
	int ret;
	FusionID fusion_id = fusionee_id(fusionee);

	switch (_IOC_NR(cmd)) {
	case _IOC_NR(FUSION_PROPERTY_NEW):
		ret = fusion_property_new(dev, &id);
		if (ret)
			return ret;

		if (put_user(id, (int *)arg)) {
			fusion_property_destroy(dev, id);
			return -EFAULT;
		}
		return 0;

	case _IOC_NR(FUSION_PROPERTY_LEASE):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_property_lease(dev, id, fusion_id);

	case _IOC_NR(FUSION_PROPERTY_PURCHASE):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_property_purchase(dev, id, fusion_id);

	case _IOC_NR(FUSION_PROPERTY_CEDE):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_property_cede(dev, id, fusion_id);

	case _IOC_NR(FUSION_PROPERTY_HOLDUP):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_property_holdup(dev, id, fusionee);

	case _IOC_NR(FUSION_PROPERTY_DESTROY):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_property_destroy(dev, id);
	}

	return -ENOSYS;
}

static int
reactor_ioctl(FusionDev * dev, Fusionee * fusionee,
	      unsigned int cmd, unsigned long arg)
{
	int id;
	int ret;
	FusionReactorAttach attach;
	FusionReactorDetach detach;
	FusionReactorDispatch dispatch;
	FusionReactorSetCallback callback;
	FusionID fusion_id = fusionee_id(fusionee);

	switch (_IOC_NR(cmd)) {
	case _IOC_NR(FUSION_REACTOR_NEW):
		ret = fusion_reactor_new(dev, &id);
		if (ret)
			return ret;

		if (put_user(id, (int *)arg)) {
			fusion_reactor_destroy(dev, id);
			return -EFAULT;
		}
		return 0;

	case _IOC_NR(FUSION_REACTOR_ATTACH):
		if (dev->api.major <= 4) {
			if (get_user(id, (int *)arg))
				return -EFAULT;

			attach.reactor_id = id;
			attach.channel = 0;
		} else {
			if (copy_from_user(&attach,
					   (FusionReactorAttach *) arg,
					   sizeof(attach)))
				return -EFAULT;
		}

		return fusion_reactor_attach(dev, attach.reactor_id,
					     attach.channel, fusion_id);

	case _IOC_NR(FUSION_REACTOR_DETACH):
		if (dev->api.major <= 4) {
			if (get_user(id, (int *)arg))
				return -EFAULT;

			detach.reactor_id = id;
			detach.channel = 0;
		} else {
			if (copy_from_user(&detach,
					   (FusionReactorDetach *) arg,
					   sizeof(detach)))
				return -EFAULT;
		}

		return fusion_reactor_detach(dev, detach.reactor_id,
					     detach.channel, fusion_id);

	case _IOC_NR(FUSION_REACTOR_DISPATCH):
		if (copy_from_user(&dispatch,
				   (FusionReactorDispatch *) arg,
				   sizeof(dispatch)))
			return -EFAULT;

		if (dispatch.msg_size <= 0)
			return -EINVAL;

		/* message data > 64k should be stored in shared memory */
		if (dispatch.msg_size > 0x10000)
			return -EMSGSIZE;

		if (dev->api.major <= 4)
			dispatch.channel = 0;

		return fusion_reactor_dispatch(dev, dispatch.reactor_id,
					       dispatch.channel,
					       dispatch.self ? NULL : fusionee,
					       dispatch.msg_size,
					       dispatch.msg_data);

	case _IOC_NR(FUSION_REACTOR_DESTROY):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_reactor_destroy(dev, id);

	case _IOC_NR(FUSION_REACTOR_SET_DISPATCH_CALLBACK):
		if (copy_from_user(&callback,
				   (FusionReactorSetCallback *) arg,
				   sizeof(callback)))
			return -EFAULT;

		return fusion_reactor_set_dispatch_callback(dev,
							    callback.reactor_id,
							    callback.call_id,
							    callback.call_ptr);
	}

	return -ENOSYS;
}

static int
shmpool_ioctl(FusionDev * dev, Fusionee * fusionee,
	      unsigned int cmd, unsigned long arg)
{
	int id;
	int ret;
	FusionSHMPoolNew pool;
	FusionSHMPoolAttach attach;
	FusionSHMPoolDispatch dispatch;
	FusionID fusion_id = fusionee_id(fusionee);

	switch (_IOC_NR(cmd)) {
	case _IOC_NR(FUSION_SHMPOOL_NEW):
		if (copy_from_user
		    (&pool, (FusionSHMPoolNew *) arg, sizeof(pool)))
			return -EFAULT;

		ret = fusion_shmpool_new(dev, &pool);
		if (ret)
			return ret;

		if (copy_to_user((FusionSHMPoolNew *) arg, &pool, sizeof(pool))) {
			fusion_shmpool_destroy(dev, pool.pool_id);
			return -EFAULT;
		}

		return 0;

	case _IOC_NR(FUSION_SHMPOOL_ATTACH):
		if (copy_from_user(&attach,
				   (FusionSHMPoolAttach *) arg, sizeof(attach)))
			return -EFAULT;

		ret = fusion_shmpool_attach(dev, &attach, fusion_id);
		if (ret)
			return ret;

		if (copy_to_user
		    ((FusionSHMPoolAttach *) arg, &attach, sizeof(attach))) {
			fusion_shmpool_detach(dev, attach.pool_id, fusion_id);
			return -EFAULT;
		}

		return 0;

	case _IOC_NR(FUSION_SHMPOOL_DETACH):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_shmpool_detach(dev, id, fusion_id);

	case _IOC_NR(FUSION_SHMPOOL_DISPATCH):
		if (copy_from_user(&dispatch,
				   (FusionSHMPoolDispatch *) arg,
				   sizeof(dispatch)))
			return -EFAULT;

		return fusion_shmpool_dispatch(dev, &dispatch, fusionee);

	case _IOC_NR(FUSION_SHMPOOL_DESTROY):
		if (get_user(id, (int *)arg))
			return -EFAULT;

		return fusion_shmpool_destroy(dev, id);
	}

	return -ENOSYS;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)

static int
fusion_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	Fusionee *fusionee = filp->private_data;
	FusionDev *dev = fusionee->fusion_dev;

	DEBUG("fusion_ioctl (0x%08x)\n", cmd);

	switch (_IOC_TYPE(cmd)) {
	case FT_LOUNGE:
		return lounge_ioctl(filp, dev, fusionee, cmd, arg);

	case FT_MESSAGING:
		return messaging_ioctl(dev, fusionee, cmd, arg);

	case FT_CALL:
		return call_ioctl(dev, fusionee, cmd, arg);

	case FT_REF:
		return ref_ioctl(dev, fusionee, cmd, arg);

	case FT_SKIRMISH:
		return skirmish_ioctl(dev, fusionee, cmd, arg);

	case FT_PROPERTY:
		return property_ioctl(dev, fusionee, cmd, arg);

	case FT_REACTOR:
		return reactor_ioctl(dev, fusionee, cmd, arg);

	case FT_SHMPOOL:
		return shmpool_ioctl(dev, fusionee, cmd, arg);
	}

	return -ENOSYS;
}

#else

static int
fusion_ioctl(struct inode *inode, struct file *file,
	     unsigned int cmd, unsigned long arg)
{
	Fusionee *fusionee = file->private_data;
	FusionDev *dev = fusion_devs[iminor(inode)];

	DEBUG("fusion_ioctl (0x%08x)\n", cmd);

	switch (_IOC_TYPE(cmd)) {
	case FT_LOUNGE:
		return lounge_ioctl(file, dev, fusionee, cmd, arg);

	case FT_MESSAGING:
		return messaging_ioctl(dev, fusionee, cmd, arg);

	case FT_CALL:
		return call_ioctl(dev, fusionee, cmd, arg);

	case FT_REF:
		return ref_ioctl(dev, fusionee, cmd, arg);

	case FT_SKIRMISH:
		return skirmish_ioctl(dev, fusionee, cmd, arg);

	case FT_PROPERTY:
		return property_ioctl(dev, fusionee, cmd, arg);

	case FT_REACTOR:
		return reactor_ioctl(dev, fusionee, cmd, arg);

	case FT_SHMPOOL:
		return shmpool_ioctl(dev, fusionee, cmd, arg);
	}

	return -ENOSYS;
}
#endif

static int fusion_mmap(struct file *file, struct vm_area_struct *vma)
{
	Fusionee *fusionee = file->private_data;
	FusionDev *dev = fusion_devs[iminor(file->f_dentry->d_inode)];
	unsigned int size;

	if (vma->vm_pgoff != 0)
		return -EINVAL;

	size = vma->vm_end - vma->vm_start;
	if (!size || size > PAGE_SIZE)
		return -EINVAL;

	if (!dev->shared_area) {
		if (fusionee_id(fusionee) != FUSION_ID_MASTER)
			return -EPERM;

		dev->shared_area = get_zeroed_page(GFP_KERNEL);
		if (!dev->shared_area)
			return -ENOMEM;

		SetPageReserved(virt_to_page((void *)dev->shared_area));
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
	return remap_pfn_range(vma, vma->vm_start,
			       virt_to_phys((void *)dev->
					    shared_area) >> PAGE_SHIFT,
			       PAGE_SIZE, vma->vm_page_prot);
#else
	return io_remap_page_range(vma->vm_start,
				   virt_to_phys((void *)dev->shared_area),
				   PAGE_SIZE, vma->vm_page_prot);
#endif
}

static struct file_operations fusion_fops = {
	.owner = THIS_MODULE,
	.open = fusion_open,
	.flush = fusion_flush,
	.release = fusion_release,
	.read = fusion_read,
	.poll = fusion_poll,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
       .unlocked_ioctl=        fusion_ioctl,
#else
	.ioctl = fusion_ioctl,
#endif
	.mmap = fusion_mmap
};

/******************************************************************************/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
static int __init register_devices(void)
{
	int i;

	if (register_chrdev(FUSION_MAJOR, "fusion", &fusion_fops)) {
		printk(KERN_ERR "fusion: unable to get major %d\n",
		       FUSION_MAJOR);
		return -EIO;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 2)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
	fusion_class = class_create(THIS_MODULE, "fusion");
#else
	fusion_class = class_simple_create(THIS_MODULE, "fusion");
#endif
	if (IS_ERR(fusion_class)) {
		unregister_chrdev(FUSION_MAJOR, "fusion");
		return PTR_ERR(fusion_class);
	}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
	devfs_mk_dir("fusion");
#endif

	for (i = 0; i < NUM_MINORS; i++) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
		device_create(fusion_class,
			      NULL,
			      MKDEV(FUSION_MAJOR, i), NULL, "fusion%d", i);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
		device_create(fusion_class,
			      NULL, MKDEV(FUSION_MAJOR, i), "fusion%d", i);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 15)
		class_device_create(fusion_class,
				    NULL,
				    MKDEV(FUSION_MAJOR, i),
				    NULL, "fusion%d", i);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
		class_device_create(fusion_class,
				    MKDEV(FUSION_MAJOR, i),
				    NULL, "fusion%d", i);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 2)
		class_simple_device_add(fusion_class,
					MKDEV(FUSION_MAJOR, i),
					NULL, "fusion%d", i);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
		devfs_mk_cdev(MKDEV(FUSION_MAJOR, i),
			      S_IFCHR | S_IRUSR | S_IWUSR, "fusion/%d", i);
#endif
	}

	return 0;
}
#else
static int __init register_devices(void)
{
	int i;
	char buf[16];

	if (devfs_register_chrdev(FUSION_MAJOR, "fusion", &fusion_fops)) {
		printk(KERN_ERR "fusion: unable to get major %d\n",
		       FUSION_MAJOR);
		return -EIO;
	}

	for (i = 0; i < NUM_MINORS; i++) {
		snprintf(buf, 16, "fusion/%d", i);

		devfs_handles[i] = devfs_register(NULL, buf, DEVFS_FL_DEFAULT,
						  FUSION_MAJOR, i,
						  S_IFCHR | S_IRUSR | S_IWUSR,
						  &fusion_fops, NULL);
	}

	return 0;
}
#endif

int __init fusion_init(void)
{
	int ret;

	ret = register_devices();
	if (ret)
		return ret;

	proc_fusion_dir = proc_mkdir("fusion", NULL);

	return 0;
}

/******************************************************************************/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
static void __exit deregister_devices(void)
{
	int i;

	unregister_chrdev(FUSION_MAJOR, "fusion");

	for (i = 0; i < NUM_MINORS; i++) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 2)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
		device_destroy(fusion_class, MKDEV(FUSION_MAJOR, i));
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
		class_device_destroy(fusion_class, MKDEV(FUSION_MAJOR, i));
#else
		class_simple_device_remove(MKDEV(FUSION_MAJOR, i));
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
		devfs_remove("fusion/%d", i);
#endif
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 2)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
	class_destroy(fusion_class);
#else
	class_simple_destroy(fusion_class);
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
	devfs_remove("fusion");
#endif
}
#else
static void __exit deregister_devices(void)
{
	int i;

	devfs_unregister_chrdev(FUSION_MAJOR, "fusion");

	for (i = 0; i < NUM_MINORS; i++)
		devfs_unregister(devfs_handles[i]);
}
#endif

void __exit fusion_exit(void)
{
	deregister_devices();

	remove_proc_entry("fusion", NULL);
}

module_init(fusion_init);
module_exit(fusion_exit);

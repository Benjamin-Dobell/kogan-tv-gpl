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
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
#include <linux/smp_lock.h>
#endif
#include <linux/sched.h>
#include <asm/uaccess.h>

#include <linux/fusion.h>

#include "call.h"
#include "list.h"
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


typedef struct {
	FusionLink link;

	FusionMessageType type;
	int id;
	int channel;
	int size;
	void *data;

	MessageCallback callback;
	void *callback_ctx;
	int callback_param;
} Message;

/******************************************************************************/

static int lookup_fusionee(FusionDev * dev, FusionID id,
			   Fusionee ** ret_fusionee);
static int lock_fusionee(FusionDev * dev, FusionID id,
			 Fusionee ** ret_fusionee);
static void unlock_fusionee(Fusionee * fusionee);

static void flush_messages(FusionDev * dev, FusionFifo * fifo);

/******************************************************************************/

static int
fusionees_read_proc(char *buf, char **start, off_t offset,
		    int len, int *eof, void *private)
{
	FusionLink *l;
	FusionDev *dev = private;
	int written = 0;

	if (down_interruptible(&dev->fusionee.lock))
		return -EINTR;

	fusion_list_foreach(l, dev->fusionee.list) {
		Fusionee *fusionee = (Fusionee *) l;

		written +=
		    sprintf(buf + written,
			    "(%5d) 0x%08lx (%4d messages waiting, %7d received, %7d sent)\n",
			    fusionee->pid, fusionee->id,
			    fusionee->messages.count, fusionee->rcv_total,
			    fusionee->snd_total);
		if (written < offset) {
			offset -= written;
			written = 0;
		}

		if (written >= len)
			break;
	}

	up(&dev->fusionee.lock);

	*start = buf + offset;
	written -= offset;
	if (written > len) {
		*eof = 0;
		return len;
	}

	*eof = 1;
	return (written < 0) ? 0 : written;
}

int fusionee_init(FusionDev * dev)
{
	init_waitqueue_head(&dev->fusionee.wait);

	init_MUTEX(&dev->fusionee.lock);

	create_proc_read_entry("fusionees", 0, dev->proc_dir,
			       fusionees_read_proc, dev);

	return 0;
}

void fusionee_deinit(FusionDev * dev)
{
	FusionLink *l;

	down(&dev->fusionee.lock);

	remove_proc_entry("fusionees", dev->proc_dir);

	l = dev->fusionee.list;
	while (l) {
		FusionLink *next = l->next;
		Fusionee *fusionee = (Fusionee *) l;

		while (fusionee->messages.count) {
			Message *message =
			    (Message *) fusion_fifo_get(&fusionee->messages);

			kfree(message);
		}

		kfree(fusionee);

		l = next;
	}

	up(&dev->fusionee.lock);
}

/******************************************************************************/

int fusionee_new(FusionDev * dev, bool force_slave, Fusionee ** ret_fusionee)
{
	Fusionee *fusionee;

	fusionee = kmalloc(sizeof(Fusionee), GFP_KERNEL);
	if (!fusionee)
		return -ENOMEM;

	memset(fusionee, 0, sizeof(Fusionee));

	if (down_interruptible(&dev->fusionee.lock)) {
		kfree(fusionee);
		return -EINTR;
	}

	fusionee->pid = current->pid;
	fusionee->force_slave = force_slave;
	fusionee->mm = current->mm;

	init_MUTEX(&fusionee->lock);

	init_waitqueue_head(&fusionee->wait);

	fusion_list_prepend(&dev->fusionee.list, &fusionee->link);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
      fusionee->fusion_dev = dev;
#endif

	up(&dev->fusionee.lock);

	*ret_fusionee = fusionee;

	return 0;
}

int fusionee_enter(FusionDev * dev, FusionEnter * enter, Fusionee * fusionee)
{
	if (down_interruptible(&dev->enter_lock))
		return -EINTR;

	if (dev->fusionee.last_id || fusionee->force_slave) {
		while (!dev->enter_ok) {
			fusion_sleep_on(&dev->enter_wait, &dev->enter_lock,
					NULL);

			if (signal_pending(current))
				return -EINTR;

			if (down_interruptible(&dev->enter_lock))
				return -EINTR;
		}

		FUSION_ASSERT(dev->fusionee.last_id != 0);
	}

	if (dev->fusionee.last_id == 0) {
		/* master determines Fusion API (if supported) */
		int major = enter->api.major;
		if ((major != 3) && (major != 4) && (major != 8))
			return -ENOPROTOOPT;

		dev->api.major = enter->api.major;
		dev->api.minor = enter->api.minor;
	} else {
		if ((enter->api.major != dev->api.major)
		    || (enter->api.minor > dev->api.minor))
			return -ENOPROTOOPT;
	}

	fusionee->id = ++dev->fusionee.last_id;

	up(&dev->enter_lock);

	enter->fusion_id = fusionee->id;

	return 0;
}

int fusionee_fork(FusionDev * dev, FusionFork * fork, Fusionee * fusionee)
{
	int ret;

	ret = fusion_shmpool_fork_all(dev, fusionee->id, fork->fusion_id);
	if (ret)
		return ret;

	ret = fusion_reactor_fork_all(dev, fusionee->id, fork->fusion_id);
	if (ret)
		return ret;

	ret = fusion_ref_fork_all_local(dev, fusionee->id, fork->fusion_id);
	if (ret)
		return ret;

	fork->fusion_id = fusionee->id;

	return 0;
}

int
fusionee_send_message(FusionDev * dev,
		      Fusionee * sender,
		      FusionID recipient,
		      FusionMessageType msg_type,
		      int msg_id,
		      int msg_channel,
		      int msg_size,
		      const void *msg_data,
		      MessageCallback callback,
		      void *callback_ctx, int callback_param)
{
	int ret;
	Message *message;
	Fusionee *fusionee;

	DEBUG("fusionee_send_message (%d -> %d, type %d, id %d, size %d)\n",
	      fusionee->id, recipient, msg_type, msg_id, msg_size);

	ret = lookup_fusionee(dev, recipient, &fusionee);
	if (ret)
		return ret;

	if (down_interruptible(&fusionee->lock)) {
		up(&dev->fusionee.lock);
		return -EINTR;
	}

	if (sender && sender != fusionee) {
		if (down_interruptible(&sender->lock)) {
			unlock_fusionee(fusionee);
			up(&dev->fusionee.lock);
			return -EINTR;
		}
	}

	up(&dev->fusionee.lock);

	message = kmalloc(sizeof(Message) + msg_size, GFP_KERNEL);
	if (!message) {
		if (sender && sender != fusionee)
			unlock_fusionee(sender);
		unlock_fusionee(fusionee);
		return -ENOMEM;
	}

	message->data = message + 1;

	if (msg_type == FMT_CALL || msg_type == FMT_SHMPOOL)
		memcpy(message->data, msg_data, msg_size);
	else if (copy_from_user(message->data, msg_data, msg_size)) {
		kfree(message);
		if (sender && sender != fusionee)
			unlock_fusionee(sender);
		unlock_fusionee(fusionee);
		return -EFAULT;
	}

	message->type		= msg_type;
	message->id		= msg_id;
	message->channel	= msg_channel;
	message->size		= msg_size;
	message->callback	= callback;
	message->callback_ctx	= callback_ctx;
	message->callback_param	= callback_param;

	fusion_fifo_put(&fusionee->messages, &message->link);

	fusionee->rcv_total++;
	if (sender)
		sender->snd_total++;

	wake_up_interruptible_all(&fusionee->wait);

	if (sender && sender != fusionee)
		unlock_fusionee(sender);

	unlock_fusionee(fusionee);

	return 0;
}

int
fusionee_get_messages(FusionDev * dev,
		      Fusionee * fusionee, void *buf, int buf_size, bool block)
{
	int written = 0;
	FusionFifo prev_msgs;

	if (down_interruptible(&fusionee->lock))
		return -EINTR;

	if (fusionee->dispatcher_pid)
		FUSION_ASSUME(fusionee->dispatcher_pid == current->pid);

	fusionee->dispatcher_pid = current->pid;

	prev_msgs = fusionee->prev_msgs;

	fusion_fifo_reset(&fusionee->prev_msgs);

	wake_up_interruptible_all(&fusionee->wait);

	while (!fusionee->messages.count) {
		if (!block) {
			unlock_fusionee(fusionee);
			flush_messages(dev, &prev_msgs);
			return -EAGAIN;
		}

		if (prev_msgs.count) {
			unlock_fusionee(fusionee);
			flush_messages(dev, &prev_msgs);
		} else {
			fusion_sleep_on(&fusionee->wait, &fusionee->lock, 0);

			if (signal_pending(current))
				return -EINTR;
		}

		if (down_interruptible(&fusionee->lock))
			return -EINTR;
	}

	while (fusionee->messages.count) {
		FusionReadMessage header;
		Message *message = (Message *) fusionee->messages.first;
		int bytes = ((message->size + 3) & ~3) + sizeof(header);

		if (bytes > buf_size) {
			if (!written) {
				unlock_fusionee(fusionee);
				flush_messages(dev, &prev_msgs);
				return -EMSGSIZE;
			}

			break;
		}

		header.msg_type		= message->type;
		header.msg_id		= message->id;
		header.msg_channel	= message->channel;
		header.msg_size		= message->size;

		if (copy_to_user(buf, &header, sizeof(header)) ||
		    copy_to_user(buf + sizeof(header), message->data,
				 message->size)) {
			unlock_fusionee(fusionee);
			flush_messages(dev, &prev_msgs);
			return -EFAULT;
		}

		if (bytes > message->size + sizeof(header)) {
			int pad = bytes - message->size - sizeof(header);
			u8 *dst = buf + sizeof(header) + message->size;

			while (pad--)
				*dst++ = 0;
		}

		written += bytes;
		buf += bytes;
		buf_size -= bytes;

		fusion_fifo_get(&fusionee->messages);

		if (message->callback || message->callback_ctx
		    || message->callback_param)
			fusion_fifo_put(&fusionee->prev_msgs, &message->link);
		else
			kfree(message);
	}

	unlock_fusionee(fusionee);

	flush_messages(dev, &prev_msgs);

	return written;
}

int
fusionee_wait_processing(FusionDev * dev,
			 int fusion_id, FusionMessageType msg_type, int msg_id)
{
	Fusionee *fusionee;

	do {
		int ret;
		Message *message;

		ret = lock_fusionee(dev, fusion_id, &fusionee);
		if (ret)
			return ret;

		/* Search all pending messages. */
		message = (Message *) fusionee->messages.first;
		while (message) {
			if (message->type == msg_type && message->id == msg_id)
				break;

			message = (Message *) message->link.next;
		}

		/* Search messages being processed right now. */
		if (!message) {
			message = (Message *) fusionee->prev_msgs.first;
			while (message) {
				if (message->type == msg_type
				    && message->id == msg_id)
					break;

				message = (Message *) message->link.next;
			}
		}

		/* Really no more message of that type and ID? */
		if (!message)
			break;

		if (fusionee->dispatcher_pid)
			FUSION_ASSUME(fusionee->dispatcher_pid != current->pid);

		/* Otherwise unlock and wait. */
		fusion_sleep_on(&fusionee->wait, &fusionee->lock, 0);

		if (signal_pending(current))
			return -EINTR;
	} while (true);

	unlock_fusionee(fusionee);

	return 0;
}

unsigned int
fusionee_poll(FusionDev * dev,
	      Fusionee * fusionee, struct file *file, poll_table * wait)
{
	int ret;
	FusionID id = fusionee->id;
	FusionFifo prev_msgs;

	ret = lock_fusionee(dev, id, &fusionee);
	if (ret)
		return POLLERR;

	prev_msgs = fusionee->prev_msgs;

	fusion_fifo_reset(&fusionee->prev_msgs);

	unlock_fusionee(fusionee);

	flush_messages(dev, &prev_msgs);

	wake_up_all(&fusionee->wait);

	poll_wait(file, &fusionee->wait, wait);

	ret = lock_fusionee(dev, id, &fusionee);
	if (ret)
		return POLLERR;

	if (fusionee->messages.count) {
		unlock_fusionee(fusionee);

		return POLLIN | POLLRDNORM;
	}

	unlock_fusionee(fusionee);

	return 0;
}

int
fusionee_kill(FusionDev * dev,
	      Fusionee * fusionee, FusionID target, int signal, int timeout_ms)
{
	long timeout = -1;

	while (true) {
		FusionLink *l;
		int killed = 0;

		if (down_interruptible(&dev->fusionee.lock))
			return -EINTR;

		fusion_list_foreach(l, dev->fusionee.list) {
			Fusionee *f = (Fusionee *) l;

			if (f != fusionee && (!target || target == f->id)) {
				struct task_struct *p;

#ifdef rcu_read_lock
				rcu_read_lock();
#else
				read_lock(&tasklist_lock);
#endif
#ifdef for_each_task		/* 2.4 */
				for_each_task(p) {
#else /* for >= 2.6.0 & redhat WS EL3 w/ 2.4 kernel */
				for_each_process(p) {
#endif
					if (p->mm == f->mm) {
						send_sig_info(signal,
							      (void *)1L
							      /* 1 means from kernel */
							      ,
							      p);
						killed++;
					}
				}

#ifdef rcu_read_unlock
				rcu_read_unlock();
#else
				read_unlock(&tasklist_lock);
#endif
			}
		}

		if (!killed || timeout_ms < 0) {
			up(&dev->fusionee.lock);
			break;
		}

		if (timeout_ms) {
			switch (timeout) {
			case 0:	/* timed out */
				up(&dev->fusionee.lock);
				return -ETIMEDOUT;

			case -1:	/* setup timeout */
				timeout = (timeout_ms * HZ + 500) / 1000;
				if (!timeout)
					timeout = 1;

				/* fall through */

			default:
				fusion_sleep_on(&dev->fusionee.wait,
						&dev->fusionee.lock, &timeout);
				break;
			}
		} else
			fusion_sleep_on(&dev->fusionee.wait,
					&dev->fusionee.lock, NULL);

		if (signal_pending(current))
			return -EINTR;
	}

	return 0;
}

void fusionee_destroy(FusionDev * dev, Fusionee * fusionee)
{
	FusionFifo prev_msgs;
	FusionFifo messages;

	/* Lock list. */
	down(&dev->fusionee.lock);

	/* Lock fusionee. */
	down(&fusionee->lock);

	prev_msgs = fusionee->prev_msgs;
	messages = fusionee->messages;

	/* Remove from list. */
	fusion_list_remove(&dev->fusionee.list, &fusionee->link);

	/* Wake up waiting killer. */
	wake_up_interruptible_all(&dev->fusionee.wait);

	/* Unlock list. */
	up(&dev->fusionee.lock);

	/* Release locks, references, ... */
	fusion_call_destroy_all(dev, fusionee->id);
	fusion_skirmish_dismiss_all(dev, fusionee->id);
	fusion_reactor_detach_all(dev, fusionee->id);
	fusion_property_cede_all(dev, fusionee->id);
	fusion_ref_clear_all_local(dev, fusionee->id);
	fusion_shmpool_detach_all(dev, fusionee->id);

	/* Unlock fusionee. */
	up(&fusionee->lock);

	/* Free all pending messages. */
	flush_messages(dev, &prev_msgs);
	flush_messages(dev, &messages);

	/* Free fusionee data. */
	kfree(fusionee);
}

FusionID fusionee_id(const Fusionee * fusionee)
{
	return fusionee->id;
}

pid_t fusionee_dispatcher_pid(FusionDev * dev, FusionID fusion_id)
{
	FusionLink *l;
	int ret = -EINVAL;

	down(&dev->fusionee.lock);

	fusion_list_foreach(l, dev->fusionee.list) {
		Fusionee *fusionee = (Fusionee *) l;

		if (fusionee->id == fusion_id) {
			/* FIXME: wait for it? */
			FUSION_ASSUME(fusionee->dispatcher_pid != 0);

			ret = fusionee->dispatcher_pid;
			break;
		}
	}

	up(&dev->fusionee.lock);

	return ret;
}

/******************************************************************************/

static int
lookup_fusionee(FusionDev * dev, FusionID id, Fusionee ** ret_fusionee)
{
	FusionLink *l;

	if (down_interruptible(&dev->fusionee.lock))
		return -EINTR;

	fusion_list_foreach(l, dev->fusionee.list) {
		Fusionee *fusionee = (Fusionee *) l;

		if (fusionee->id == id) {
			*ret_fusionee = fusionee;
			return 0;
		}
	}

	up(&dev->fusionee.lock);

	return -EINVAL;
}

static int lock_fusionee(FusionDev * dev, FusionID id, Fusionee ** ret_fusionee)
{
	int ret;
	Fusionee *fusionee;

	ret = lookup_fusionee(dev, id, &fusionee);
	if (ret)
		return ret;

	fusion_list_move_to_front(&dev->fusionee.list, &fusionee->link);

	if (down_interruptible(&fusionee->lock)) {
		up(&dev->fusionee.lock);
		return -EINTR;
	}

	up(&dev->fusionee.lock);

	*ret_fusionee = fusionee;

	return 0;
}

static void unlock_fusionee(Fusionee * fusionee)
{
	up(&fusionee->lock);
}

/******************************************************************************/

static void flush_messages(FusionDev * dev, FusionFifo * fifo)
{
	Message *message;

	while ((message = (Message *) fusion_fifo_get(fifo)) != NULL) {
		if (message->callback)
			message->callback(dev, message->id,
					  message->callback_ctx,
					  message->callback_param);

		kfree(message);
	}
}

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

#include "call.h"
#include "fusiondev.h"
#include "fusionee.h"
#include "list.h"
#include "reactor.h"
#include "shmpool.h"

typedef struct {
	FusionLink link;

	int fusion_id;

	int *counts;		/* number of attach calls */
	int num_counts;
} ReactorNode;

typedef struct {
	int count;		/* number of recipients */

	int call_id;		/* id of call to execute when count reaches zero */
	int call_arg;		/* optional parameter of user space */
	void *call_ptr;
} ReactorDispatch;

typedef struct {
	FusionEntry entry;

	FusionLink *nodes;

	int dispatch_count;

	bool destroyed;

	int call_id;
	void *call_ptr;
} FusionReactor;

/******************************************************************************/

static int fork_node(FusionReactor * reactor,
		     FusionID fusion_id, FusionID from_id);

static void free_all_nodes(FusionReactor * reactor);

/******************************************************************************/

static inline ReactorNode *get_node(FusionReactor * reactor, FusionID fusion_id)
{
	ReactorNode *node;

	fusion_list_foreach(node, reactor->nodes) {
		if (node->fusion_id == fusion_id)
			return node;
	}

	return NULL;
}

/******************************************************************************/

static void fusion_reactor_destruct(FusionEntry * entry, void *ctx)
{
	FusionReactor *reactor = (FusionReactor *) entry;

	free_all_nodes(reactor);
}

static void
fusion_reactor_print(FusionEntry * entry, void *ctx, struct seq_file *p)
{
	int num = 0;
	FusionReactor *reactor = (FusionReactor *) entry;
	FusionLink *node = reactor->nodes;

	fusion_list_foreach(node, reactor->nodes) {
		num++;
	}

	seq_printf(p, "%5dx dispatch, %d nodes%s\n", reactor->dispatch_count,
		   num, reactor->destroyed ? "  DESTROYED" : "");
}

FUSION_ENTRY_CLASS(FusionReactor, reactor, NULL,
		   fusion_reactor_destruct, fusion_reactor_print)

/******************************************************************************/
int fusion_reactor_init(FusionDev * dev)
{
	fusion_entries_init(&dev->reactor, &reactor_class, dev);

	fusion_entries_create_proc_entry(dev, "reactors", &dev->reactor);

	return 0;
}

void fusion_reactor_deinit(FusionDev * dev)
{
	remove_proc_entry("reactors", dev->proc_dir);

	fusion_entries_deinit(&dev->reactor);
}

/******************************************************************************/

int fusion_reactor_new(FusionDev * dev, int *ret_id)
{
	return fusion_entry_create(&dev->reactor, ret_id, NULL);
}

int
fusion_reactor_attach(FusionDev * dev, int id, int channel, FusionID fusion_id)
{
	int ret;
	ReactorNode *node;
	FusionReactor *reactor;

	if (channel < 0 || channel > 1023)
		return -EINVAL;

	ret = fusion_reactor_lock(&dev->reactor, id, false, &reactor);
	if (ret)
		return ret;

	if (reactor->destroyed) {
		fusion_reactor_unlock(reactor);
		return -EIDRM;
	}

	dev->stat.reactor_attach++;

	node = get_node(reactor, fusion_id);
	if (!node) {
		int ncount = channel + 4;

		node = kmalloc(sizeof(ReactorNode), GFP_KERNEL);
		if (!node) {
			fusion_reactor_unlock(reactor);
			return -ENOMEM;
		}

		node->counts = kmalloc(sizeof(int) * ncount, GFP_KERNEL);
		if (!node->counts) {
			kfree(node);
			fusion_reactor_unlock(reactor);
			return -ENOMEM;
		}

		memset(node->counts, 0, sizeof(int) * ncount);

		node->num_counts = ncount;
		node->fusion_id = fusion_id;

		node->counts[channel] = 1;

		fusion_list_prepend(&reactor->nodes, &node->link);
	} else {
		if (node->num_counts <= channel) {
			int ncount = channel + 4;
			int *counts = kmalloc(sizeof(int) * ncount, GFP_KERNEL);

			if (!counts) {
				fusion_reactor_unlock(reactor);
				return -ENOMEM;
			}

			memcpy(counts, node->counts,
			       sizeof(int) * node->num_counts);
			memset(counts + node->num_counts, 0,
			       sizeof(int) * (ncount - node->num_counts));

			kfree(node->counts);

			node->counts = counts;
			node->num_counts = ncount;
		}

		node->counts[channel]++;
	}

	fusion_reactor_unlock(reactor);

	return 0;
}

int
fusion_reactor_detach(FusionDev * dev, int id, int channel, FusionID fusion_id)
{
	int ret;
	ReactorNode *node;
	FusionReactor *reactor;

	if (channel < 0 || channel > 1023)
		return -EINVAL;

	ret = fusion_reactor_lock(&dev->reactor, id, true, &reactor);
	if (ret)
		return ret;

	dev->stat.reactor_detach++;

	node = get_node(reactor, fusion_id);
	if (!node || node->num_counts <= channel) {
		fusion_reactor_unlock(reactor);
		up(&dev->reactor.lock);
		return -EIO;
	}

	if (!--node->counts[channel]) {
		int i;

		for (i = 0; i < node->num_counts; i++) {
			if (node->counts[i])
				break;
		}

		if (i == node->num_counts) {
			fusion_list_remove(&reactor->nodes, &node->link);
			kfree(node->counts);
			kfree(node);
		}
	}

	if (reactor->destroyed && !reactor->nodes)
		fusion_entry_destroy_locked(&dev->reactor, &reactor->entry);
	else
		fusion_reactor_unlock(reactor);

	up(&dev->reactor.lock);

	return 0;
}

static void dispatch_callback(FusionDev * dev, int id, void *ctx, int arg)
{
	FusionLink *l;
	FusionReactor *reactor = NULL;
	ReactorDispatch *dispatch = ctx;

	down(&dev->reactor.lock);

	fusion_list_foreach(l, dev->reactor.list) {
		reactor = (FusionReactor *) l;

		if (reactor->entry.id == id) {
			down(&reactor->entry.lock);

			if (!--dispatch->count) {
				FusionCallExecute execute;

				execute.call_id = dispatch->call_id;
				execute.call_arg = dispatch->call_arg;
				execute.call_ptr = dispatch->call_ptr;

				fusion_call_execute(dev, NULL, &execute);

				kfree(dispatch);
			}

			up(&reactor->entry.lock);

			break;
		}
	}

	if (!reactor) {
		if (!--dispatch->count)
			kfree(dispatch);
	}

	up(&dev->reactor.lock);
}

int
fusion_reactor_dispatch(FusionDev * dev, int id, int channel,
			Fusionee * fusionee, int msg_size, const void *msg_data)
{
	int ret;
	FusionLink *l;
	FusionReactor *reactor;
	ReactorDispatch *dispatch = NULL;
	FusionID fusion_id = fusionee ? fusionee_id(fusionee) : 0;

	if (channel < 0 || channel > 1023)
		return -EINVAL;

	ret = fusion_reactor_lock(&dev->reactor, id, false, &reactor);
	if (ret)
		return ret;

	if (reactor->destroyed) {
		fusion_reactor_unlock(reactor);
		return -EIDRM;
	}

	if (reactor->call_id) {
		void *ptr = *(void **)msg_data;

		dispatch = kmalloc(sizeof(ReactorDispatch), GFP_KERNEL);
		if (!dispatch) {
			fusion_reactor_unlock(reactor);
			return -ENOMEM;
		}

		dispatch->count = 0;
		dispatch->call_id = reactor->call_id;
		dispatch->call_arg = channel;

		if (!reactor->call_ptr && msg_size == sizeof(ptr) &&
		    ptr >= FUSION_SHM_BASE &&
		    ptr < (FUSION_SHM_BASE + FUSION_SHM_SIZE))
			dispatch->call_ptr = ptr;
		else
			dispatch->call_ptr = reactor->call_ptr;
	}

	reactor->dispatch_count++;

	dev->stat.reactor_dispatch++;

	fusion_list_foreach(l, reactor->nodes) {
		ReactorNode *node = (ReactorNode *) l;

		if (node->fusion_id == fusion_id || node->num_counts <= channel
		    || !node->counts[channel])
			continue;

		if (dispatch) {
			dispatch->count++;

			ret =
			    fusionee_send_message(dev, fusionee,
						  node->fusion_id, FMT_REACTOR,
						  reactor->entry.id, channel,
						  msg_size, msg_data,
						  dispatch_callback, dispatch,
						  reactor->entry.id);
		} else
			ret =
			    fusionee_send_message(dev, fusionee,
						  node->fusion_id, FMT_REACTOR,
						  reactor->entry.id, channel,
						  msg_size, msg_data, NULL,
						  NULL, 0);
	}

	if (dispatch && !dispatch->count) {
		FusionCallExecute execute;

		execute.call_id = dispatch->call_id;
		execute.call_arg = dispatch->call_arg;
		execute.call_ptr = dispatch->call_ptr;

		fusion_call_execute(dev, NULL, &execute);

		kfree(dispatch);
	}

	fusion_reactor_unlock(reactor);

	return 0;
}

int
fusion_reactor_set_dispatch_callback(FusionDev * dev,
				     int id, int call_id, void *call_ptr)
{
	int ret;
	FusionReactor *reactor;

	ret = fusion_reactor_lock(&dev->reactor, id, false, &reactor);
	if (ret)
		return ret;

	if (reactor->destroyed) {
		fusion_reactor_unlock(reactor);
		return -EIDRM;
	}

	reactor->call_id = call_id;
	reactor->call_ptr = call_ptr;

	fusion_reactor_unlock(reactor);

	return 0;
}

int fusion_reactor_destroy(FusionDev * dev, int id)
{
	int ret;
	FusionReactor *reactor;

	ret = fusion_reactor_lock(&dev->reactor, id, true, &reactor);
	if (ret)
		return ret;

	if (reactor->destroyed) {
		fusion_reactor_unlock(reactor);
		up(&dev->reactor.lock);
		return -EIDRM;
	}

	reactor->destroyed = true;

	if (!reactor->nodes)
		fusion_entry_destroy_locked(&dev->reactor, &reactor->entry);
	else
		fusion_reactor_unlock(reactor);

	up(&dev->reactor.lock);

	return 0;
}

void fusion_reactor_detach_all(FusionDev * dev, FusionID fusion_id)
{
	FusionLink *l, *n;

	down(&dev->reactor.lock);

	fusion_list_foreach_safe(l, n, dev->reactor.list) {
		ReactorNode *node;
		FusionReactor *reactor = (FusionReactor *) l;

		down(&reactor->entry.lock);

		fusion_list_foreach(node, reactor->nodes) {
			if (node->fusion_id == fusion_id) {
				fusion_list_remove(&reactor->nodes,
						   &node->link);
				kfree(node->counts);
				kfree(node);
				break;
			}
		}

		if (reactor->destroyed && !reactor->nodes)
			fusion_entry_destroy_locked(&dev->reactor,
						    &reactor->entry);
		else
			up(&reactor->entry.lock);
	}

	up(&dev->reactor.lock);
}

int
fusion_reactor_fork_all(FusionDev * dev, FusionID fusion_id, FusionID from_id)
{
	FusionLink *l;
	int ret = 0;

	down(&dev->reactor.lock);

	fusion_list_foreach(l, dev->reactor.list) {
		FusionReactor *reactor = (FusionReactor *) l;

		ret = fork_node(reactor, fusion_id, from_id);
		if (ret)
			break;
	}

	up(&dev->reactor.lock);

	return ret;
}

/******************************************************************************/

static int
fork_node(FusionReactor * reactor, FusionID fusion_id, FusionID from_id)
{
	ReactorNode *node;

	down(&reactor->entry.lock);

	fusion_list_foreach(node, reactor->nodes) {
		if (node->fusion_id == from_id) {
			ReactorNode *new_node;

			new_node = kmalloc(sizeof(ReactorNode), GFP_KERNEL);
			if (!new_node) {
				up(&reactor->entry.lock);
				return -ENOMEM;
			}

			new_node->counts =
			    kmalloc(sizeof(int) * node->num_counts, GFP_KERNEL);
			if (!new_node->counts) {
				kfree(new_node);
				up(&reactor->entry.lock);
				return -ENOMEM;
			}

			new_node->fusion_id = fusion_id;
			new_node->num_counts = node->num_counts;

			memcpy(new_node->counts, node->counts,
			       sizeof(int) * node->num_counts);

			fusion_list_prepend(&reactor->nodes, &new_node->link);

			break;
		}
	}

	up(&reactor->entry.lock);

	return 0;
}

static void free_all_nodes(FusionReactor * reactor)
{
	FusionLink *n;
	ReactorNode *node;

	fusion_list_foreach_safe(node, n, reactor->nodes) {
		kfree(node->counts);
		kfree(node);
	}

	reactor->nodes = NULL;
}

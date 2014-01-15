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

#include <linux/fusion.h>

#include "fusiondev.h"
#include "fusionee.h"
#include "list.h"
#include "skirmish.h"
#include "call.h"

typedef struct {
	FusionLink link;

	Fusionee *caller;

	int ret_val;

	bool executed;

	wait_queue_head_t wait;

	int call_id;
	unsigned int serial;
} FusionCallExecution;

typedef struct {
	FusionEntry entry;

	int fusion_id;		/* owner fusion id */

	void *handler;
	void *ctx;

	FusionLink *executions;	/* prepending! */
	FusionLink *last;	/* points to the last item of executions */

	int count;		/* number of calls ever made */

	unsigned int serial;
} FusionCall;

/* collection, required for 1-param-only passing */
struct fusion_construct_ctx {
	int fusion_id;
	FusionCallNew *call_new;
};

/******************************************************************************/

static FusionCallExecution *add_execution(FusionCall * call,
					  Fusionee * caller,
					  FusionCallExecute * execute,
					  unsigned int serial);
static void remove_execution(FusionCall * call,
			     FusionCallExecution * execution);
static void free_all_executions(FusionCall * call);

/******************************************************************************/

static int
fusion_call_construct(FusionEntry * entry, void *ctx, void *create_ctx)
{
	FusionCall *call = (FusionCall *) entry;

	struct fusion_construct_ctx *cc =
	    (struct fusion_construct_ctx *)create_ctx;

	call->fusion_id = cc->fusion_id;
	call->handler = cc->call_new->handler;
	call->ctx = cc->call_new->ctx;

	cc->call_new->call_id = entry->id;

	return 0;
}

static void fusion_call_destruct(FusionEntry * entry, void *ctx)
{
	FusionCall *call = (FusionCall *) entry;

	free_all_executions(call);
}

static void
fusion_call_print(FusionEntry * entry, void *ctx, struct seq_file *p)
{
	FusionLink *e;
	bool idle = true;
	FusionCall *call = (FusionCall *) entry;

	if (call->executions)
		idle = ((FusionCallExecution *) call->executions)->executed;

	seq_printf(p, "(%d calls) %s",
		   call->count, idle ? "idle" : "executing");

	fusion_list_foreach(e, call->executions) {
		FusionCallExecution *exec = (FusionCallExecution *) e;

		seq_printf(p, "  [0x%08lx]",
			   exec->caller ? fusionee_id(exec->caller) : 0);
	}

	seq_printf(p, "\n");
}

FUSION_ENTRY_CLASS(FusionCall, call, fusion_call_construct,
		   fusion_call_destruct, fusion_call_print)

/******************************************************************************/

int fusion_call_init(FusionDev * dev)
{
	fusion_entries_init(&dev->call, &call_class, dev);

	fusion_entries_create_proc_entry(dev, "calls", &dev->call);

	return 0;
}

void fusion_call_deinit(FusionDev * dev)
{
	remove_proc_entry("calls", dev->proc_dir);

	fusion_entries_deinit(&dev->call);
}

/******************************************************************************/

int fusion_call_new(FusionDev * dev, int fusion_id, FusionCallNew * call_new)
{
	int id;
	int ret;

	struct fusion_construct_ctx cc = { fusion_id, call_new };

	ret = fusion_entry_create(&dev->call, &id, &cc);
	if (ret)
		return ret;

	return 0;
}

int
fusion_call_execute(FusionDev * dev, Fusionee * fusionee,
		    FusionCallExecute * execute)
{
	int ret;
	FusionCall *call;
	FusionCallExecution *execution = NULL;
	FusionCallMessage message;
	unsigned int serial;

	/* Lookup and lock call. */
	ret = fusion_call_lock(&dev->call, execute->call_id, false, &call);
	if (ret)
		return ret;

	do {
		serial = ++call->serial;
	} while (!serial);

	/* Add execution to receive the result. */
	if (fusionee && !(execute->flags & FCEF_ONEWAY)) {
		execution = add_execution(call, fusionee, execute, serial);
		if (!execution) {
			fusion_call_unlock(call);
			return -ENOMEM;
		}
	}

	/* Fill call message. */
	message.handler = call->handler;
	message.ctx = call->ctx;

	message.caller = fusionee ? fusionee_id(fusionee) : 0;

	message.call_arg = execute->call_arg;
	message.call_ptr = execute->call_ptr;

	message.serial = execution ? serial : 0;

	/* Put message into queue of callee. */
	ret = fusionee_send_message(dev, fusionee, call->fusion_id, FMT_CALL,
				    call->entry.id, 0, sizeof(message),
				    &message, NULL, NULL, 1);
	if (ret) {
		if (execution) {
			remove_execution(call, execution);
			kfree(execution);
		}
		fusion_call_unlock(call);
		return ret;
	}

	call->count++;

	/* When waiting for a result... */
	if (execution) {
		/* Transfer held skirmishs (locks). */
		fusion_skirmish_transfer_all(dev, call->fusion_id,
					     fusionee_id(fusionee),
					     current->pid);

		/* Unlock call and wait for execution result. TODO: add timeout? */
		fusion_sleep_on(&execution->wait, &call->entry.lock, 0);

		if (signal_pending(current)) {
			/* Indicate that a signal was received and execution won't be freed by caller. */
			execution->caller = NULL;
			return -EINTR;
		}

		/* Return result to calling process. */
		execute->ret_val = execution->ret_val;

		/* Free execution, which has already been removed by callee. */
		kfree(execution);

		/* Reclaim skirmishs. */
		fusion_skirmish_reclaim_all(dev, current->pid);
	} else
		/* Unlock call. */
		fusion_call_unlock(call);

	return 0;
}

int
fusion_call_return(FusionDev * dev, int fusion_id, FusionCallReturn * call_ret)
{
	int ret;
	FusionLink *l;
	FusionCall *call;

	if ( (dev->api.major >= 4) && (call_ret->serial == 0) )
		return -EOPNOTSUPP;

	/* Lookup and lock call. */
	ret = fusion_call_lock(&dev->call, call_ret->call_id, false, &call);
	if (ret)
		return ret;

	/* Search for execution, starting with last (oldest). */
	l = call->last;
	while (l) {
		FusionCallExecution *execution = (FusionCallExecution *) l;

		if ((execution->executed)
		    || (execution->call_id != call_ret->call_id)
		    || ((dev->api.major >= 4)
			&& (execution->serial != call_ret->serial))) {
			l = l->prev;
			continue;
		}

		/*
		 * Check if caller received a signal while waiting for the result.
		 *
		 * TODO: This is not completely solved. Restarting the system call
		 * should be possible without causing another execution.
		 */
		FUSION_ASSUME(execution->caller != NULL);
		if (!execution->caller) {
			/* Remove and free execution. */
			remove_execution(call, execution);
			kfree(execution);
			fusion_call_unlock(call);
			return -EIDRM;
		}

		/* Write result to execution. */
		execution->ret_val = call_ret->val;
		execution->executed = true;

		/* Remove execution, freeing is up to caller. */
		remove_execution(call, execution);

		/* FIXME: Caller might still have received a signal since check above. */
		FUSION_ASSERT(execution->caller != NULL);

		/* Wake up caller. */
		wake_up_interruptible(&execution->wait);

		/* Unlock call. */
		fusion_call_unlock(call);

		return 0;
	}

	/* Unlock call. */
	fusion_call_unlock(call);

	/* DirectFB 1.0.x does not handle one-way-calls properly */
	if (dev->api.major <= 3)
		return 0;

	return -ENOMSG;
}

int fusion_call_destroy(FusionDev * dev, int fusion_id, int call_id)
{
	int ret;
	FusionCall *call;
	FusionCallExecution *execution;

	do {
		/* Wait for all messages being processed. */
		ret =
		    fusionee_wait_processing(dev, fusion_id, FMT_CALL, call_id);
		if (ret)
			return ret;

		ret = fusion_call_lock(&dev->call, call_id, false, &call);
		if (ret)
			return ret;

		/* Check if we own the call. */
		if (call->fusion_id != fusion_id) {
			fusion_call_unlock(call);
			return -EIO;
		}

		/* If an execution is pending... */
		execution = (FusionCallExecution *) call->executions;
		if (execution) {
			/* Unlock call and wait for execution. TODO: add timeout? */
			fusion_sleep_on(&execution->wait, &call->entry.lock, 0);

			if (signal_pending(current))
				return -EINTR;
		}
	} while (execution);

	fusion_entry_destroy_locked(call->entry.entries, &call->entry);

	return 0;
}

void fusion_call_destroy_all(FusionDev * dev, int fusion_id)
{
	FusionLink *l;

	down(&dev->call.lock);

	l = dev->call.list;

	while (l) {
		FusionLink *next = l->next;
		FusionCall *call = (FusionCall *) l;

		down(&call->entry.lock);

		if (call->fusion_id == fusion_id)
			fusion_entry_destroy_locked(call->entry.entries,
						    &call->entry);
		else
			up(&call->entry.lock);

		l = next;
	}

	up(&dev->call.lock);
}

/******************************************************************************/

static FusionCallExecution *add_execution(FusionCall * call,
					  Fusionee * caller,
					  FusionCallExecute * execute,
					  unsigned int serial)
{
	FusionCallExecution *execution;

	/* Allocate execution. */
	execution = kmalloc(sizeof(FusionCallExecution), GFP_KERNEL);
	if (!execution)
		return NULL;

	/* Initialize execution. */
	memset(execution, 0, sizeof(FusionCallExecution));

	execution->caller = caller;
	execution->call_id = call->entry.id;
	execution->serial = serial;

	init_waitqueue_head(&execution->wait);

	/* Add execution. */
	fusion_list_prepend(&call->executions, &execution->link);

	if (!call->last)
		call->last = &execution->link;

	return execution;
}

static void remove_execution(FusionCall * call, FusionCallExecution * execution)
{
	if (call->last == &execution->link)
		call->last = execution->link.prev;

	fusion_list_remove(&call->executions, &execution->link);
}

static void free_all_executions(FusionCall * call)
{
	while (call->last) {
		FusionCallExecution *execution =
		    (FusionCallExecution *) call->last;

		remove_execution(call, execution);

		wake_up_interruptible_all(&execution->wait);

		kfree(execution);
	}
}

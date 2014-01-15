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

#ifndef __FUSION__FUSIONEE_H__
#define __FUSION__FUSIONEE_H__

#include <linux/poll.h>
#include <linux/fusion.h>

#include "fusiondev.h"
#include "types.h"
#include "fifo.h"

struct __Fusion_Fusionee {
	FusionLink link;

	struct semaphore lock;

	FusionID id;
	int pid;

	FusionFifo messages;
	FusionFifo prev_msgs;

	int rcv_total;		/* Total number of messages received. */
	int snd_total;		/* Total number of messages sent. */

	wait_queue_head_t wait;

	bool force_slave;

	struct mm_struct *mm;

	pid_t dispatcher_pid;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
      FusionDev *fusion_dev;
#endif
};
/* module init/cleanup */

int fusionee_init(FusionDev * dev);
void fusionee_deinit(FusionDev * dev);

/* internal functions */

int fusionee_new(FusionDev * dev, bool force_slave, Fusionee ** ret_fusionee);

int fusionee_enter(FusionDev * dev, FusionEnter * enter, Fusionee * fusionee);

int fusionee_fork(FusionDev * dev, FusionFork * fork, Fusionee * fusionee);

int fusionee_send_message(FusionDev * dev,
			  Fusionee * fusionee,
			  FusionID recipient,
			  FusionMessageType msg_type,
			  int msg_id,
			  int msg_channel,
			  int msg_size,
			  const void *msg_data,
			  MessageCallback callback,
			  void *callback_ctx, int callback_param);

int fusionee_get_messages(FusionDev * dev,
			  Fusionee * fusionee,
			  void *buf, int buf_size, bool block);

int fusionee_wait_processing(FusionDev * dev,
			     int fusion_id,
			     FusionMessageType msg_type, int msg_id);

unsigned
int fusionee_poll(FusionDev * dev,
		  Fusionee * fusionee, struct file *file, poll_table * wait);

int fusionee_kill(FusionDev * dev,
		  Fusionee * fusionee,
		  FusionID target, int signal, int timeout_ms);

void fusionee_destroy(FusionDev * dev, Fusionee * fusionee);

FusionID fusionee_id(const Fusionee * fusionee);

pid_t fusionee_dispatcher_pid(FusionDev * dev, FusionID fusion_id);

#endif

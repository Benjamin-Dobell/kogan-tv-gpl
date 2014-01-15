/*
   (c) Copyright 2002-2011  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2002-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version
   2 of the License, or (at your option) any later version.
*/

#ifndef __FUSION__FUSIONEE_H__
#define __FUSION__FUSIONEE_H__

#include <linux/poll.h>
#include <linux/fusion.h>

#include "fusiondev.h"
#include "fifo.h"
#include "types.h"


struct __Fusion_Fusionee {
     FusionLink     link;

     int            magic;

     int            refs;

     FusionID       id;
     int            pid;

     FusionFifo packets;
     FusionFifo prev_packets;

     FusionFifo free_packets;

     atomic_long_t  rcv_total;          /* Total number of messages received. */
     atomic_long_t  snd_total;          /* Total number of messages sent. */

     FusionWaitQueue wait_receive;
     FusionWaitQueue wait_process;

     bool force_slave;

     struct mm_struct *mm;

     pid_t dispatcher_pid;

     FusionDev *fusion_dev;

     char exe_file[PATH_MAX];
};




typedef enum {
     FMC_NONE,
     FMC_DISPATCH
} FusionMessageCallback;


/* module init/cleanup */

int fusionee_init(FusionDev * dev);
void fusionee_deinit(FusionDev * dev);

/* internal functions */

int fusionee_new(FusionDev * dev, bool force_slave, Fusionee ** ret_fusionee);

int fusionee_enter(FusionDev * dev, FusionEnter * enter, Fusionee * fusionee);

void fusionee_ref(Fusionee * fusionee);
void fusionee_unref(Fusionee * fusionee);

int fusionee_fork(FusionDev * dev, FusionFork * fork, Fusionee * fusionee);

int fusionee_get_info(FusionDev * dev, FusionGetFusioneeInfo * get_info);

int fusionee_send_message(FusionDev * dev,
                          Fusionee * fusionee,
                          FusionID recipient,
                          FusionMessageType msg_type,
                          int msg_id,
                          int msg_channel,
                          int msg_size,
                          const void *msg_data,
                          FusionMessageCallback callback,
                          void *callback_ctx, int callback_param,
                          const void *extra_data, unsigned int extra_size);

int fusionee_send_message2(FusionDev * dev,
                           Fusionee * sender,
                           Fusionee * recipient,
                           FusionMessageType msg_type,
                           int msg_id,
                           int msg_channel,
                           int msg_size,
                           const void *msg_data,
                           FusionMessageCallback callback,
                           void *callback_ctx, int callback_param,
                           const void *extra_data, unsigned int extra_size,
                           bool flush);

int fusionee_get_messages(FusionDev * dev,
                          Fusionee * fusionee,
                          void *buf, int buf_size, bool block);

int fusionee_wait_processing(FusionDev * dev,
                             int fusion_id,
                             FusionMessageType msg_type, int msg_id);

unsigned
int fusionee_poll(FusionDev * dev,
                  Fusionee * fusionee, struct file *file, poll_table * wait);

int fusionee_sync(FusionDev *dev,
                  Fusionee  *fusionee);

int fusionee_kill(FusionDev * dev,
                  Fusionee * fusionee,
                  FusionID target, int signal, int timeout_ms);

void fusionee_destroy(FusionDev * dev, Fusionee * fusionee);

FusionID fusionee_id(const Fusionee * fusionee);

pid_t fusionee_dispatcher_pid(FusionDev * dev, FusionID fusion_id);


#endif

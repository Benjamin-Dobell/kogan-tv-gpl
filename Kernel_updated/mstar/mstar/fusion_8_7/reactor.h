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

#ifndef __FUSION__REACTOR_H__
#define __FUSION__REACTOR_H__

#include "fusiondev.h"
#include "types.h"

/* module init/cleanup */

int fusion_reactor_init(FusionDev * dev);
void fusion_reactor_deinit(FusionDev * dev);

/* public API */

int fusion_reactor_new(FusionDev * dev, Fusionee *fusionee, int *id);

int fusion_reactor_attach(FusionDev * dev,
                          int id, int channel, FusionID fusion_id);

int fusion_reactor_detach(FusionDev * dev,
                          int id, int channel, FusionID fusion_id);

int fusion_reactor_dispatch(FusionDev * dev,
                            int id,
                            int channel,
                            Fusionee * fusionee,
                            int msg_size, const void *msg_data);

int fusion_reactor_destroy(FusionDev * dev, int id);

int fusion_reactor_set_dispatch_callback(FusionDev * dev,
                                         int id, int call_id, void *call_ptr);

/* internal functions */

void fusion_reactor_detach_all(FusionDev * dev, FusionID fusion_id);

int fusion_reactor_fork_all(FusionDev * dev,
                            FusionID fusion_id, FusionID from_id);



void fusion_reactor_dispatch_message_callback(FusionDev * dev, int id, void *ctx, int arg);

#endif

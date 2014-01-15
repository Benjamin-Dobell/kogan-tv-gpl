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

#ifndef __FUSION__CALL_H__
#define __FUSION__CALL_H__

#include <linux/fusion.h>

#include "fusiondev.h"

/* module init/cleanup */

int fusion_call_init(FusionDev * dev);
void fusion_call_deinit(FusionDev * dev);

/* public API */

int fusion_call_new(FusionDev * dev, Fusionee *fusionee, FusionCallNew * call);

int fusion_call_execute(FusionDev * dev, Fusionee * fusionee,    /* NULL if call is from kernel */
                        FusionCallExecute * execute);

int fusion_call_execute2(FusionDev * dev, Fusionee * fusionee,   /* NULL if call is from kernel */
                         FusionCallExecute2 * execute);

int fusion_call_return(FusionDev * dev,
                       int fusion_id, FusionCallReturn * call_ret);

int fusion_call_execute3(FusionDev * dev, Fusionee * fusionee,   /* NULL if call is from kernel */
                         FusionCallExecute3 * execute);

int fusion_call_return3(FusionDev * dev,
                        int fusion_id, FusionCallReturn3 * call_ret);

int fusion_call_get_owner(FusionDev * dev, int call_id, FusionID *ret_fusion_id);

int fusion_call_destroy(FusionDev * dev, Fusionee *fusionee, int call_id);

/* internal functions */

void fusion_call_destroy_all(FusionDev * dev, Fusionee *fusionee);

#endif

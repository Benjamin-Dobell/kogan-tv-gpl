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

#ifndef __FUSION__REF_H__
#define __FUSION__REF_H__

#include "fusiondev.h"
#include "types.h"

/* module init/cleanup */

int fusion_ref_init(FusionDev * dev);
void fusion_ref_deinit(FusionDev * dev);

/* public API */

int fusion_ref_new(FusionDev * dev, int *id);

int fusion_ref_up(FusionDev * dev, int id, FusionID fusion_id);

int fusion_ref_down(FusionDev * dev, int id, FusionID fusion_id);

int fusion_ref_zero_lock(FusionDev * dev, int id, FusionID fusion_id);

int fusion_ref_zero_trylock(FusionDev * dev, int id, FusionID fusion_id);

int fusion_ref_zero_unlock(FusionDev * dev, int id, FusionID fusion_id);

int fusion_ref_stat(FusionDev * dev, int id, int *refs);

int fusion_ref_watch(FusionDev * dev, int id, int call_id, int call_arg);

int fusion_ref_inherit(FusionDev * dev, int id, int from);

int fusion_ref_destroy(FusionDev * dev, int id);

/* internal functions */

void fusion_ref_clear_all_local(FusionDev * dev, FusionID fusion_id);

int fusion_ref_fork_all_local(FusionDev * dev,
			      FusionID fusion_id, FusionID from_id);

#endif

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

#ifndef __FUSION__PROPERTY_H__
#define __FUSION__PROPERTY_H__

#include "fusiondev.h"
#include "types.h"

/* module init/cleanup */

int fusion_property_init(FusionDev * dev);
void fusion_property_deinit(FusionDev * dev);

/* public API */

int fusion_property_new(FusionDev * dev, int *ret_id);

int fusion_property_lease(FusionDev * dev, int id, int fusion_id);

int fusion_property_purchase(FusionDev * dev, int id, int fusion_id);

int fusion_property_cede(FusionDev * dev, int id, int fusion_id);

int fusion_property_holdup(FusionDev * dev, int id, Fusionee * fusionee);

int fusion_property_destroy(FusionDev * dev, int id);

/* internal functions */

void fusion_property_cede_all(FusionDev * dev, int fusion_id);

#endif

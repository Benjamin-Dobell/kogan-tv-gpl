/*
 *	Fusion Kernel Module
 *
 *	(c) Copyright 2002  Convergence GmbH
 *
 *      Written by Denis Oliver Kropp <dok@directfb.org>
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#ifndef __FUSION__TYPES_H__
#define __FUSION__TYPES_H__

#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
typedef enum {
	false = 0,
	true = !false
} bool;
#endif

typedef struct __Fusion_FusionDev FusionDev;
typedef struct __Fusion_Fusionee Fusionee;

typedef void (*MessageCallback) (FusionDev * dev,
				 int msg_id, void *ctx, int param);

#endif

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

#ifndef __FUSION__FIFO_H__
#define __FUSION__FIFO_H__

#include "types.h"
#include "list.h"

typedef struct {
	FusionLink *first;
	FusionLink *last;

	int count;
} FusionFifo;

void fusion_fifo_put(FusionFifo * fifo, FusionLink * link);

FusionLink *fusion_fifo_get(FusionFifo * fifo);

static inline void fusion_fifo_reset(FusionFifo * fifo)
{
	fifo->count = 0;
	fifo->first = fifo->last = NULL;
}

#endif /* __FUSION__FIFO_H__ */

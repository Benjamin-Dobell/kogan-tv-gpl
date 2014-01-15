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

#include <linux/types.h>

#include "fifo.h"

void fusion_fifo_put(FusionFifo * fifo, FusionLink * link)
{
	link->prev = fifo->last;
	link->next = NULL;

	if (fifo->last)
		fifo->last->next = link;
	else
		fifo->first = link;

	fifo->last = link;

	fifo->count++;
}

FusionLink *fusion_fifo_get(FusionFifo * fifo)
{
	FusionLink *first = fifo->first;

	if (!first)
		return NULL;

	fifo->first = first->next;

	if (fifo->last == first)
		fifo->last = NULL;
	else
		fifo->first->prev = NULL;

	fifo->count--;

	return first;
}

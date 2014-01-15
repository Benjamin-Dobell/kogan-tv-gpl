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

#ifndef __FUSION__FIFO_H__
#define __FUSION__FIFO_H__

#include "types.h"
#include "list.h"

typedef struct {
     FusionLink *items;
     int         count;
} FusionFifo;

void fusion_fifo_put(FusionFifo * fifo, FusionLink * link);

FusionLink *fusion_fifo_get(FusionFifo * fifo);

static inline void fusion_fifo_reset(FusionFifo * fifo)
{
     fifo->items = NULL;
     fifo->count = 0;
}

#endif /* __FUSION__FIFO_H__ */

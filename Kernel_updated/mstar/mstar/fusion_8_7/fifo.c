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

#include <linux/types.h>

#include "fifo.h"

void fusion_fifo_put(FusionFifo * fifo, FusionLink * link)
{
     direct_list_append( &fifo->items, link );

     fifo->count++;
}

FusionLink *fusion_fifo_get(FusionFifo * fifo)
{
     FusionLink *first = fifo->items;

     if (!first) {
          D_ASSERT( fifo->count == 0 );
          return NULL;
     }

     direct_list_remove( &fifo->items, first );

     fifo->count--;

     return first;
}

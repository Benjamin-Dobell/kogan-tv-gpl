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

#ifndef __FUSION__FUSIONCORE_H__
#define __FUSION__FUSIONCORE_H__

#include <linux/stddef.h>

#include "fusioncore_impl.h"

typedef enum {
     FC_OK,
     FC_FAILURE
} FusionCoreResult;


typedef struct __Fusion_FusionCore FusionCore;


FusionCoreResult  fusion_core_enter    ( int              cpu_index,
                                         FusionCore     **ret_core );

void              fusion_core_exit     ( FusionCore      *core );

pid_t             fusion_core_pid      ( FusionCore      *core );


void             *fusion_core_malloc   ( FusionCore      *core,
                                         size_t           size );

void              fusion_core_free     ( FusionCore      *core,
                                         void            *ptr );

void              fusion_core_set_pointer( FusionCore      *core,
                                           unsigned int     index,
                                           void            *ptr );

void             *fusion_core_get_pointer( FusionCore      *core,
                                           unsigned int     index );


void              fusion_core_lock     ( FusionCore      *core );
void              fusion_core_unlock   ( FusionCore      *core );


FusionCoreResult  fusion_core_wq_init  ( FusionCore      *core,
                                         FusionWaitQueue *queue );

void              fusion_core_wq_deinit( FusionCore      *core,
                                         FusionWaitQueue *queue );

void              fusion_core_wq_wait  ( FusionCore      *core,
                                         FusionWaitQueue *queue,
                                         int             *timeout_ms,
                                         bool             interruptible );

void              fusion_core_wq_wake  ( FusionCore      *core,
                                         FusionWaitQueue *queue );


#endif

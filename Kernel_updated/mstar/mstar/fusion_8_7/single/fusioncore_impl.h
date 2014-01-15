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

#ifndef __FUSION__FUSIONCORE_IMPL_H__
#define __FUSION__FUSIONCORE_IMPL_H__

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif

#include <linux/wait.h>


struct __Fusion_FusionCore {
     int                 magic;

     int                 cpu_index;

     struct semaphore    lock;

     void               *pointers[10];
};


typedef struct {
     int                 magic;

     wait_queue_head_t   queue;
} FusionWaitQueue;


#endif

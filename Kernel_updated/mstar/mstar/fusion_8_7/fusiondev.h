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

#ifndef __FUSION__FUSIONDEV_H__
#define __FUSION__FUSIONDEV_H__

#include <linux/proc_fs.h>

#include "debug.h"
#include "entries.h"
#include "list.h"

#define D_ARRAY_SIZE(array)        ((int)(sizeof(array) / sizeof((array)[0])))

#define NUM_MINORS  8
#define NUM_CLASSES 8

#define CACHE_EXECUTIONS_NUM      10
#define CACHE_EXECUTIONS_DATA_LEN 20

typedef struct __Fusion_FusionShared FusionShared;

struct __Fusion_FusionDev {
     FusionShared *shared;

     int refs;
     int index;
     struct {
          int major;
          int minor;
     } api;

     int enter_ok;
     FusionWaitQueue enter_wait;

     int secure;

#ifdef FUSION_CORE_SHMPOOLS
     void *shared_area;
#else
     unsigned long shared_area;
#endif

     struct {
          int property_lease_purchase;
          int property_cede;

          int reactor_attach;
          int reactor_detach;
          int reactor_dispatch;

          int ref_up;
          int ref_down;

          int ref_catch;
          int ref_throw;

          int skirmish_prevail_swoop;
          int skirmish_dismiss;
          int skirmish_wait;
          int skirmish_notify;

          int shmpool_attach;
          int shmpool_detach;
     } stat;

     struct {
          int last_id;
          FusionLink *list;
          FusionWaitQueue wait;
     } fusionee;

     FusionEntries call;
     FusionEntries properties;
     FusionEntries reactor;
     FusionEntries ref;
     FusionEntries shmpool;
     FusionEntries skirmish;

     FusionLink   *execution_free_list;
     unsigned int  execution_free_list_num;

     unsigned int  next_class_index;
};

struct __Fusion_FusionShared {
     FusionDev   devs[NUM_MINORS];

     FusionLink *addr_entries;
     void       *addr_base;
};

extern FusionCore            *fusion_core;
extern struct proc_dir_entry *fusion_proc_dir[NUM_MINORS];


extern unsigned long fusion_shm_base;
extern unsigned long fusion_shm_size;

#endif

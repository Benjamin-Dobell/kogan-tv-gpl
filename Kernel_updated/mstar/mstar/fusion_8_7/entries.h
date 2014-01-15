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

#ifndef __FUSION__ENTRIES_H__
#define __FUSION__ENTRIES_H__

#include <linux/mutex.h>
#include <linux/seq_file.h>

#include "types.h"
#include "list.h"

#include "fusioncore.h"


typedef struct __FD_FusionEntry FusionEntry;

typedef const struct {
     int object_size;

     int (*Init)     (FusionEntry * entry, void *ctx, void *create_ctx);
     void (*Destroy) (FusionEntry * entry, void *ctx);
     void (*Print)   (FusionEntry * entry, void *ctx, struct seq_file * p);
} FusionEntryClass;

typedef struct {
     unsigned int class_index;
     void *ctx;

     FusionLink *list;
     int ids;

     FusionDev *dev;

     struct timeval now; /* temporary for /proc code (seq start/show) */

     FusionHash *hash;
} FusionEntries;

typedef struct {
     DirectLink     link;

     FusionID       fusion_id;
     unsigned int   permissions;
} FusionEntryPermissionsItem;

struct __FD_FusionEntry {
     FusionLink link;

     FusionEntries *entries;

     int id;
     pid_t pid;

     FusionWaitQueue wait;
     int waiters;
     int *waiters_list;
     int  waiters_list_max;

     struct timeval last_lock;

     char name[FUSION_ENTRY_INFO_NAME_LENGTH];

     DirectLink *permissions;

     FusionID    creator;
};

/* Entries Init & DeInit */

void fusion_entries_init( FusionEntries    *entries,
                          FusionEntryClass *class,
                          void             *ctx,
                          FusionDev        *dev );

void fusion_entries_deinit(FusionEntries * entries);

/* '/proc' support */

void fusion_entries_create_proc_entry(FusionDev * dev, const char *name,
                                      FusionEntries * data);

void fusion_entries_destroy_proc_entry(FusionDev * dev, const char *name);

/* Create & Destroy */

int fusion_entry_create(FusionEntries * entries, int *ret_id, void *create_ctx, FusionID fusion_id);

int fusion_entry_destroy(FusionEntries * entries, int id);

void fusion_entry_destroy_locked(FusionEntries * entries, FusionEntry * entry);

/* Information */

int fusion_entry_set_info(FusionEntries * entries,
                          const FusionEntryInfo * info);

int fusion_entry_get_info(FusionEntries * entries, FusionEntryInfo * info);

/* Permissions */

int fusion_entry_add_permissions  ( FusionEntries                *entries,
                                    const FusionEntryPermissions *permissions,
                                    Fusionee                     *fusionee );

int fusion_entry_check_permissions( FusionEntries                *entries,
                                    int                           entry_id,
                                    FusionID                      fusion_id,
                                    unsigned int                  nr );

/* Lookup */

int fusion_entry_lookup(FusionEntries * entries, int id, FusionEntry ** ret_entry);

/** Wait & Notify **/

/*
 * Wait for the entry to be notified with an optional timeout.
 *
 * The entry
 *   (1) has to be locked prior to calling this function.
 *   (2) is temporarily unlocked while being waited for.
 *
 * If this function returns an error, the entry is not locked again!
 *
 * Possible errors are:
 *   -EIDRM      Entry has been removed while being waited for.
 *   -ETIMEDOUT  Timeout occured.
 *   -EINTR      A signal has been received.
 */
int fusion_entry_wait(FusionEntry * entry, int *timeout);

/*
 * Wake up one or all processes waiting for the entry to be notified.
 *
 * The entry has to be locked prior to calling this function.
 */
void fusion_entry_notify(FusionEntry * entry);

#define FUSION_ENTRY_CLASS( Type, name, init_func, destroy_func, print_func )   \
                                                                                \
     static FusionEntryClass name##_class = {                                   \
          .object_size = sizeof(Type),                                          \
          .Init        = init_func,                                             \
          .Destroy     = destroy_func,                                          \
          .Print       = print_func                                             \
     };                                                                         \
                                                                                \
     static inline int fusion_##name##_lookup( FusionEntries  *entries,         \
                                               int             id,              \
                                               Type          **ret_##name )     \
     {                                                                          \
          int          ret;                                                     \
          FusionEntry *entry;                                                   \
                                                                                \
          ret = fusion_entry_lookup( entries, id, &entry );                     \
                                                                                \
          if (!ret)                                                             \
               *ret_##name = (Type *) entry;                                    \
                                                                                \
          return ret;                                                           \
     }                                                                          \
                                                                                \
     static inline int fusion_##name##_wait( Type *name, int *timeout )         \
     {                                                                          \
          return fusion_entry_wait( (FusionEntry*) name, timeout );             \
     }                                                                          \
                                                                                \
     static inline void fusion_##name##_notify( Type *name )                    \
     {                                                                          \
          fusion_entry_notify( (FusionEntry*) name );                           \
     }

#endif

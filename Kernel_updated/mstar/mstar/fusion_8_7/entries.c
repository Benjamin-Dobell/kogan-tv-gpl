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

#ifdef HAVE_LINUX_CONFIG_H
#include <linux/config.h>
#endif
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
#include <linux/smp_lock.h>
#endif
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/version.h>

#include <linux/fusion.h>

#include "fusiondev.h"
#include "fusionee.h"
#include "entries.h"
#include "hash.h"


static FusionEntryClass *entry_classes[NUM_MINORS][NUM_CLASSES];

void
fusion_entries_init( FusionEntries    *entries,
                     FusionEntryClass *class,
                     void             *ctx,
                     FusionDev        *dev )
{

     FUSION_DEBUG( "%s( entries %p, class %p, ctx %p, dev %p )\n",
                   __FUNCTION__, entries, class, ctx, dev );

     FUSION_DEBUG( "  -> object_size %d, entry size %zu\n", class->object_size, sizeof(FusionEntry) );

     FUSION_ASSERT(entries != NULL);
     FUSION_ASSERT(class != NULL);
     FUSION_ASSERT(class->object_size >= sizeof(FusionEntry));

     if (!dev->refs) {
          memset(entries, 0, sizeof(FusionEntries));

          entries->class_index = dev->next_class_index++;
          entries->ctx = ctx;
          entries->dev = dev;
     }

     entry_classes[dev->index][entries->class_index] = class;

     fusion_hash_create( FHT_INT, FHT_PTR, 17, &entries->hash );
}

void fusion_entries_deinit(FusionEntries * entries)
{
     FUSION_ASSERT(entries != NULL);

     if (!entries->dev->refs) {
          FusionLink *tmp;
          FusionEntry *entry;
          FusionEntryClass *class;

          class = entry_classes[entries->dev->index][entries->class_index];

          fusion_list_foreach_safe(entry, tmp, entries->list) {
               if (class->Destroy)
                    class->Destroy(entry, entries->ctx);

               fusion_core_free( fusion_core, entry);
          }
     }

     fusion_hash_destroy( entries->hash );
}

/* reading PROC entries */

static void *fusion_entries_seq_start(struct seq_file *f, loff_t * pos)
{
     int i = *pos;

     FusionEntry *entry;
     FusionEntries *entries;
     FusionEntryClass *class;

     entries = f->private;

     fusion_core_lock( fusion_core );

     entry = (void *)(entries->list);
     while (i && entry) {
          entry = (void *)(entry->link.next);
          i--;
     }

     FUSION_ASSERT(entries != NULL);

     class = entry_classes[entries->dev->index][entries->class_index];
     if (!class->Print)
          return NULL;

     do_gettimeofday(&entries->now);

     return entry;
}

static void *fusion_entries_seq_next(struct seq_file *f, void *v, loff_t * pos)
{
     FusionEntry *entry = v;

     (*pos)++;
     return entry->link.next;
}

static void fusion_entries_seq_stop(struct seq_file *f, void *v)
{
     FusionEntries *entries;

     entries = f->private;
     (void)v;

     fusion_core_unlock( fusion_core );
}

int fusion_entries_show(struct seq_file *p, void *v)
{
     FusionEntry *entry;
     FusionEntries *entries;
     FusionEntryClass *class;

     entries = p->private;

     entry = v;

     class = entry_classes[entries->dev->index][entries->class_index];

     if (entry->last_lock.tv_sec) {
          int diff = ((entry->entries->now.tv_sec - entry->last_lock.tv_sec) * 1000 +
                      (entry->entries->now.tv_usec - entry->last_lock.tv_usec) / 1000);

          if (diff < 1000) {
               seq_printf(p, "%3d  ms  ", diff);
          } else if (diff < 1000000) {
               seq_printf(p, "%3d.%d s  ", diff / 1000,
                          (diff % 1000) / 100);
          } else {
               diff = (entry->entries->now.tv_sec - entry->last_lock.tv_sec +
                       (entry->entries->now.tv_usec -
                        entry->last_lock.tv_usec) / 1000000);

               seq_printf(p, "%3d.%d h  ", diff / 3600,
                          (diff % 3600) / 360);
          }
     } else
          seq_printf(p, "  -.-    ");

     seq_printf(p, "(%5d) 0x%08x  ", entry->pid, entry->id);
     seq_printf(p, "%-24s  ", entry->name[0] ? entry->name : "");

     class->Print(entry, entry->entries->ctx, p);

     return 0;
}

static const struct seq_operations fusion_entries_seq_ops = {
     .start = fusion_entries_seq_start,
     .next  = fusion_entries_seq_next,
     .stop  = fusion_entries_seq_stop,
     .show  = fusion_entries_show
};

static int fusion_entries_open(struct inode *inode, struct file *file)
{
     struct seq_file *sf;
     int ret;

     ret = seq_open(file, (struct seq_operations*) &fusion_entries_seq_ops);
     if (ret < 0)
          return ret;

     sf = file->private_data;
     sf->private = PDE(inode)->data;

     return 0;
}

static const struct file_operations proc_fusion_entries_operations = {
     .open    = fusion_entries_open,
     .read    = seq_read,
     .llseek  = seq_lseek,
     .release = seq_release,
};

void fusion_entries_create_proc_entry(FusionDev * dev, const char *name,
                                      FusionEntries * data)
{
     struct proc_dir_entry *pde;

     pde = create_proc_entry(name, 0, fusion_proc_dir[dev->index]);
     if (pde) {
          pde->proc_fops = &proc_fusion_entries_operations;
          pde->data = data;
     }
}

void fusion_entries_destroy_proc_entry(FusionDev * dev, const char *name)
{
     remove_proc_entry(name, fusion_proc_dir[dev->index]);
}

int fusion_entry_create(FusionEntries * entries, int *ret_id, void *create_ctx, FusionID fusion_id)
{
     int ret;
     FusionEntry *entry;
     FusionEntryClass *class;

     FUSION_ASSERT(entries != NULL);
     FUSION_ASSERT(ret_id != NULL);

     class = entry_classes[entries->dev->index][entries->class_index];

     entry = fusion_core_malloc( fusion_core, class->object_size );
     if (!entry)
          return -ENOMEM;

     memset(entry, 0, class->object_size);

     entry->entries = entries;
     entry->id = ++entries->ids;
     entry->pid = fusion_core_pid( fusion_core );
     entry->creator = fusion_id;

     fusion_core_wq_init( fusion_core, &entry->wait);

     if (class->Init) {
          ret = class->Init(entry, entries->ctx, create_ctx);
          if (ret) {
               fusion_core_free( fusion_core, entry);
               return ret;
          }
     }

     fusion_list_prepend(&entries->list, &entry->link);

     fusion_hash_insert( entries->hash, (void*)(long) entry->id, entry );

     *ret_id = entry->id;

     return 0;
}

int fusion_entry_destroy(FusionEntries * entries, int id)
{
     FusionEntry *entry;
     FusionEntryClass *class;

     FUSION_ASSERT(entries != NULL);

     class = entry_classes[entries->dev->index][entries->class_index];

     /* Lookup the entry. */
     entry = fusion_hash_lookup( entries->hash, (void*)(long) id );
     if (!entry)
          return -EINVAL;

     /* Destroy it now. */
     fusion_entry_destroy_locked(entries, entry);

     return 0;
}

void fusion_entry_destroy_locked(FusionEntries * entries, FusionEntry * entry)
{
     FusionEntryClass *class;
     FusionEntryPermissionsItem *item, *next;

     FUSION_ASSERT(entries != NULL);

     class = entry_classes[entries->dev->index][entries->class_index];

     direct_list_foreach_safe (item, next, entry->permissions)
          fusion_core_free( fusion_core, item );

     /* Remove the entry from the list. */
     fusion_list_remove(&entries->list, &entry->link);

     fusion_hash_remove( entries->hash, (void*)(long) entry->id, NULL, NULL );

     /* Wake up any waiting process. */
     fusion_core_wq_wake( fusion_core, &entry->wait);

     /* Call the destroy function. */
     if (class->Destroy)
          class->Destroy(entry, entries->ctx);

     /* Deallocate the entry. */
     if (entry->waiters_list)
          fusion_core_free( fusion_core, entry->waiters_list );

     fusion_core_free( fusion_core, entry );
}

int fusion_entry_set_info(FusionEntries * entries, const FusionEntryInfo * info)
{
     int ret;
     FusionEntry *entry;

     FUSION_ASSERT(entries != NULL);
     FUSION_ASSERT(info != NULL);

     ret = fusion_entry_lookup(entries, info->id, &entry);
     if (ret)
          return ret;

     snprintf(entry->name, FUSION_ENTRY_INFO_NAME_LENGTH, "%s", info->name);

     return 0;
}

int fusion_entry_get_info(FusionEntries * entries, FusionEntryInfo * info)
{
     int ret;
     FusionEntry *entry;

     FUSION_ASSERT(entries != NULL);
     FUSION_ASSERT(info != NULL);

     ret = fusion_entry_lookup(entries, info->id, &entry);
     if (ret)
          return ret;

     snprintf(info->name, FUSION_ENTRY_INFO_NAME_LENGTH, "%s", entry->name);

     return 0;
}

int
fusion_entry_add_permissions( FusionEntries                *entries,
                              const FusionEntryPermissions *permissions,
                              Fusionee                     *fusionee )
{
     int                         ret;
     FusionEntry                *entry;
     FusionEntryPermissionsItem *item;

     FUSION_ASSERT( entries != NULL );
     FUSION_ASSERT( permissions != NULL );

     ret = fusion_entry_lookup( entries, permissions->id, &entry );
     if (ret)
          return ret;

     if (fusionee_id( fusionee ) != FUSION_ID_MASTER && fusionee_id( fusionee ) != entry->creator)
          return -EPERM;

     direct_list_foreach (item, entry->permissions) {
          if (item->fusion_id == permissions->fusion_id) {
               item->permissions |= permissions->permissions;
               return 0;
          }
     }

     item = fusion_core_malloc( fusion_core, sizeof(FusionEntryPermissionsItem) );
     if (!item)
          return -ENOMEM;

     item->fusion_id   = permissions->fusion_id;
     item->permissions = permissions->permissions;

     direct_list_append( &entry->permissions, &item->link );

     return 0;
}

int
fusion_entry_check_permissions( FusionEntries *entries,
                                int            entry_id,
                                FusionID       fusion_id,
                                unsigned int   nr )
{
     int                         ret;
     FusionEntry                *entry;
     FusionEntryPermissionsItem *item;

     FUSION_ASSERT( entries != NULL );

     ret = fusion_entry_lookup( entries, entry_id, &entry );
     if (ret)
          return ret;

     if (entry->creator == fusion_id)
          return 0;

     direct_list_foreach (item, entry->permissions) {
          if (!item->fusion_id || item->fusion_id == fusion_id) {
               if (item->permissions & (1 << nr))
                    return 0;
          }
     }

     return -EPERM;
}


int
fusion_entry_lookup(FusionEntries * entries,
                    int id, FusionEntry ** ret_entry)
{
     FusionEntry *entry;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
     struct timespec xtime;
#endif

     FUSION_ASSERT(entries != NULL);
     FUSION_ASSERT(ret_entry != NULL);

     /* Lookup the entry. */
     entry = fusion_hash_lookup( entries->hash, (void*)(long) id );
     if (!entry)
          return -EINVAL;

     /* Move the entry to the front of all entries. */
//     fusion_list_move_to_front(&entries->list, &entry->link);

     /* Keep timestamp, but use the slightly
        inexact version to avoid performance impacts. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) && defined _STRUCT_TIMESPEC
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
     xtime = current_kernel_time();
#endif
     entry->last_lock.tv_sec = xtime.tv_sec;
     entry->last_lock.tv_usec = xtime.tv_nsec / 1000;
#else
     entry->last_lock = xtime;
#endif

     /* Return the locked entry. */
     *ret_entry = entry;

     return 0;
}

int fusion_entry_wait(FusionEntry * entry, int *timeout)
{
     int ret;
     int id;
     int i;
     FusionEntries *entries;
     FusionEntry *entry2;

     FUSION_ASSERT(entry != NULL);
     FUSION_ASSERT(entry->entries != NULL);

     id = entry->id;
     entries = entry->entries;


     /* Reallocate waiters array if needed */
     if (entry->waiters_list_max == entry->waiters) {
          int *new_waiters = fusion_core_malloc( fusion_core, sizeof(int) * (entry->waiters_list_max + 10) );

          if (!new_waiters)
               return -ENOMEM;

          entry->waiters_list_max += 10;

          if (entry->waiters_list) {
               memcpy( new_waiters, entry->waiters_list, sizeof(int) * entry->waiters );

               fusion_core_free( fusion_core, entry->waiters_list );
          }

          entry->waiters_list = new_waiters;
     }

     entry->waiters_list[entry->waiters] = fusion_core_pid( fusion_core );

     entry->waiters++;

     fusion_core_wq_wait( fusion_core, &entry->wait, timeout, true );

     for (i=0; i<entry->waiters-1; i++) {
          if (entry->waiters_list[i] == fusion_core_pid( fusion_core ))
               break;
     }

     for (; i<entry->waiters-1; i++)
          entry->waiters_list[i] = entry->waiters_list[i+1];

     entry->waiters--;


     if (signal_pending(current))
          return -EINTR;

     if (timeout && !*timeout)
          return -ETIMEDOUT;

     ret = fusion_entry_lookup(entries, id, &entry2);
     switch (ret) {
          case -EINVAL:
               return -EIDRM;

          case 0:
               if (entry != entry2)
                    BUG();
     }

     return ret;
}

void fusion_entry_notify(FusionEntry * entry)
{
     FUSION_ASSERT(entry != NULL);

     fusion_core_wq_wake( fusion_core, &entry->wait);
}


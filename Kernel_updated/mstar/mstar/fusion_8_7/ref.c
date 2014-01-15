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
#include <linux/proc_fs.h>

#include <linux/fusion.h>

#include "fusiondev.h"
#include "fusionee.h"
#include "list.h"
#include "call.h"
#include "ref.h"

typedef struct __Fusion_FusionRef FusionRef;

typedef struct {
     FusionLink link;
     FusionID fusion_id;
     int refs;
} LocalRef;

typedef struct {
     FusionLink link;
     FusionRef *ref;
} Inheritor;

typedef struct {
     FusionLink link;
     FusionID   fusion_id;
     FusionID   catcher;
} Throw;

struct __Fusion_FusionRef {
     FusionEntry entry;

     int global;
     int local;

     int locked;         /* non-zero fusion id of lock owner */

     bool watched;       /* true if watch has been installed */
     bool syncwatch;     /* true if watch is executed synchronously */
     int call_id;        /* id of call registered with a watch */
     int call_arg;       /* optional call parameter */

     FusionRef *inherited;
     FusionLink *inheritors;

     FusionLink *local_refs;

     FusionLink *throws;
};

/**********************************************************************************************************************/

static int get_local(FusionRef * ref, FusionID fusion_id);
static int get_throws(FusionRef * ref, FusionID fusion_id);

static int add_throw(FusionRef * ref, FusionID fusion_id, FusionID catcher);

static int add_local(FusionRef * ref, FusionID fusion_id, int add);
static void clear_local(FusionDev * dev, FusionRef * ref, FusionID fusion_id);
static int fork_local(FusionDev * dev, FusionRef * ref, FusionID fusion_id,
                      FusionID from_id);
static void free_all_local(FusionRef * ref);

static int propagate_local(FusionDev * dev, FusionRef * ref, int diff, bool async);

static void notify_ref(FusionDev * dev, FusionRef * ref, bool async);

static int add_inheritor(FusionRef * ref, FusionRef * from);
static void remove_inheritor(FusionRef * ref, FusionRef * from);
static void drop_inheritors(FusionDev * dev, FusionRef * ref);

/**********************************************************************************************************************/

static void fusion_ref_destruct(FusionEntry * entry, void *ctx)
{
     FusionRef *ref = (FusionRef *) entry;
     FusionDev *dev = (FusionDev *) ctx;

     drop_inheritors(dev, ref);

     if (ref->inherited)
          remove_inheritor(ref, ref->inherited);

     free_all_local(ref);
}

static void fusion_ref_print(FusionEntry * entry, void *ctx, struct seq_file *p)
{
     FusionLink *l;
     FusionRef  *ref = (FusionRef *) entry;

     if (ref->locked) {
          seq_printf(p, "%2d %2d (locked by %d)\n", ref->global,
                     ref->local, ref->locked);
          return;
     }

     seq_printf(p, "%2d %2d", ref->global, ref->local);

     fusion_list_foreach(l, ref->local_refs) {
          LocalRef *local = (LocalRef *) l;

          if (local->refs)
               seq_printf(p, "  0x%08lx(%d)", local->fusion_id, local->refs);
     }

     seq_printf(p, "\n");
}

FUSION_ENTRY_CLASS(FusionRef, ref, NULL, fusion_ref_destruct, fusion_ref_print);

/**********************************************************************************************************************/

int fusion_ref_init(FusionDev * dev)
{
     fusion_entries_init(&dev->ref, &ref_class, dev, dev);

     fusion_entries_create_proc_entry(dev, "refs", &dev->ref);

     return 0;
}

void fusion_ref_deinit(FusionDev * dev)
{
     fusion_entries_destroy_proc_entry( dev, "refs" );

     fusion_entries_deinit(&dev->ref);
}

/**********************************************************************************************************************/

int fusion_ref_new(FusionDev * dev, Fusionee *fusionee, int *ret_id)
{
     return fusion_entry_create(&dev->ref, ret_id, NULL, fusionee_id(fusionee));
}

int fusion_ref_up(FusionDev * dev, int id, FusionID fusion_id)
{
     int ret;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     dev->stat.ref_up++;

     if (ref->locked) {
          ret = -EAGAIN;
          return ret;
     }

     if (fusion_id) {
          ret = add_local(ref, fusion_id, 1);
          if (ret)
               return ret;

          ret = propagate_local(dev, ref, 1, false);
     }
     else
          ref->global ++;

     return 0;
}

int fusion_ref_down(FusionDev * dev, int id, FusionID fusion_id)
{
     int ret;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     dev->stat.ref_down++;

     if (ref->locked)
          return -EAGAIN;

     if (fusion_id) {
          ret = -EIO;
          if (!ref->local)
               return ret;

          ret = add_local(ref, fusion_id, -1);
          if (ret)
               return ret;

          ret = propagate_local(dev, ref, -1, false);
     }
     else {
          if (!ref->global)
               return -EIO;

          ref->global --;

          if (ref->local + ref->global == 0)
               notify_ref(dev, ref, false);
     }

     return 0;
}

int fusion_ref_catch(FusionDev * dev, int id, FusionID fusion_id)
{
     int        ret;
     FusionRef *ref;
     Throw     *throw_;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     dev->stat.ref_catch++;

     if (ref->locked)
          return -EAGAIN;

     direct_list_foreach( throw_, ref->throws ) {
          if (throw_->catcher == fusion_id) {
               FusionID thrower = throw_->fusion_id;

               fusion_list_remove( &ref->throws, &throw_->link );

               fusion_core_free( fusion_core, throw_ );


               ret = add_local( ref, thrower, -1 );
               if (ret)
                    return ret;

               propagate_local( dev, ref, -1, false );

               return 0;
          }
     }

     return -EACCES;
}

int fusion_ref_throw(FusionDev * dev, int id, FusionID fusion_id, FusionID catcher)
{
     int        ret;
     int        local;
     int        throws;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     dev->stat.ref_throw++;

     if (ref->locked)
          return -EAGAIN;

     local = get_local( ref, fusion_id );
     if (!local)
          return -EIO;

     throws = get_throws( ref, fusion_id );
     if (throws == local)
          return -EIO;

     // FIXME: cleanup on release() of thrower/catcher, timeout?
     ret = add_throw(ref, fusion_id, catcher);
     if (ret)
          return ret;

     return 0;
}

int fusion_ref_zero_lock(FusionDev * dev, int id, FusionID fusion_id)
{
     int ret;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     while (true) {
          if (ref->locked)
               return ref->locked == fusion_id ? -EIO : -EAGAIN;

          if (ref->global ||ref->local) {
               ret = fusion_ref_wait(ref, NULL);
               if (ret)
                    return ret;
          }
          else
               break;
     }

     ref->locked = fusion_id;

     return 0;
}

int fusion_ref_zero_trylock(FusionDev * dev, int id, FusionID fusion_id)
{
     int ret;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     if (ref->locked)
          return ref->locked == fusion_id ? -EIO : -EAGAIN;

     if (ref->global ||ref->local)
          ret = -ETOOMANYREFS;
     else
          ref->locked = fusion_id;

     return ret;
}

int fusion_ref_zero_unlock(FusionDev * dev, int id, FusionID fusion_id)
{
     int ret;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     if (ref->locked != fusion_id)
          return -EIO;

     ref->locked = 0;

     return 0;
}

int fusion_ref_stat(FusionDev * dev, int id, int *refs)
{
     int ret;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     *refs = ref->global +ref->local;

     return 0;
}

int fusion_ref_watch(FusionDev * dev, int id, int call_id, int call_arg)
{
     int ret;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     if (ref->entry.pid != fusion_core_pid( fusion_core ))
          return -EACCES;

     if (ref->global +ref->local == 0)
          return -EIO;

     if (ref->watched)
          return -EBUSY;

     ref->watched = true;
     ref->call_id = call_id;
     ref->call_arg = call_arg;

     fusion_ref_notify(ref);

     return 0;
}

int fusion_ref_inherit(FusionDev * dev, int id, int from_id)
{
     int ret;
     FusionRef *ref;
     FusionRef *from = NULL;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     if (ref->inherited)
          return -EBUSY;

     fusion_list_foreach(from, dev->ref.list) {
          if (from->entry.id == from_id)
               break;
     }
     if (!from)
          return -EINVAL;

     ret = add_inheritor(ref, from);
     if (ret)
          return ret;

     ret = propagate_local(dev, ref, from->local, false);
     if (ret)
          return ret;

     ref->inherited = from;

     return 0;
}

int fusion_ref_set_sync(FusionDev * dev, int id)
{
     int ret;
     FusionRef *ref;

     ret = fusion_ref_lookup(&dev->ref, id, &ref);
     if (ret)
          return ret;

     ref->syncwatch = true;

     return ret;
}

int fusion_ref_destroy(FusionDev * dev, int id)
{
     return fusion_entry_destroy(&dev->ref, id);
}

void fusion_ref_clear_all_local(FusionDev * dev, FusionID fusion_id)
{
     FusionRef *ref;

     fusion_list_foreach(ref, dev->ref.list)
     clear_local(dev, ref, fusion_id);
}

int
fusion_ref_fork_all_local(FusionDev * dev, FusionID fusion_id, FusionID from_id)
{
     FusionRef *ref;
     int ret = 0;

     fusion_list_foreach(ref, dev->ref.list) {
          ret = fork_local(dev, ref, fusion_id, from_id);
          if (ret)
               break;
     }

     return ret;
}

/**********************************************************************************************************************/

static int get_local( FusionRef * ref, FusionID fusion_id )
{
     LocalRef *local;

     direct_list_foreach (local, ref->local_refs) {
          if (local->fusion_id == fusion_id)
               return local->refs;
     }

     return 0;
}

static int get_throws( FusionRef * ref, FusionID fusion_id )
{
     Throw *throw_;
     int    throws = 0;

     direct_list_foreach (throw_, ref->throws) {
          if (throw_->fusion_id == fusion_id)
               throws++;
     }

     return throws;
}

static int add_throw(FusionRef * ref, FusionID fusion_id, FusionID catcher)
{
     Throw *throw_;

     throw_ = fusion_core_malloc( fusion_core, sizeof(Throw) );
     if (!throw_)
          return -ENOMEM;

     throw_->fusion_id = fusion_id;
     throw_->catcher   = catcher;

     direct_list_append( &ref->throws, &throw_->link );

     return 0;
}

static int add_local(FusionRef * ref, FusionID fusion_id, int add)
{
     FusionLink *l;
     LocalRef *local;

     fusion_list_foreach(l, ref->local_refs) {
          local = (LocalRef *) l;

          if (local->fusion_id == fusion_id) {
               fusion_list_move_to_front(&ref->local_refs, l);

               if (local->refs + add < 0)
                    return -EIO;

               local->refs += add;
               return 0;
          }
     }

     /* Can only create local node if value is positive. */
     if (add <= 0)
          return -EIO;

     local = fusion_core_malloc( fusion_core, sizeof(LocalRef) );
     if (!local)
          return -ENOMEM;

     local->fusion_id = fusion_id;
     local->refs = add;

     fusion_list_prepend(&ref->local_refs, &local->link);

     return 0;
}

static void clear_local(FusionDev * dev, FusionRef * ref, FusionID fusion_id)
{
     FusionLink *l;

     if (ref->locked == fusion_id) {
          ref->locked = 0;
          fusion_core_wq_wake( fusion_core, &ref->entry.wait);
     }

     fusion_list_foreach(l, ref->local_refs) {
          LocalRef *local = (LocalRef *) l;

          if (local->fusion_id == fusion_id) {
               fusion_list_remove(&ref->local_refs, l);

               if (local->refs)
                    propagate_local(dev, ref, -local->refs, true);

               fusion_core_free( fusion_core, l);
               break;
          }
     }
}

static int
fork_local(FusionDev * dev, FusionRef * ref, FusionID fusion_id,
           FusionID from_id)
{
     FusionLink *l;
     int ret = 0;

     fusion_list_foreach(l, ref->local_refs) {
          LocalRef *local = (LocalRef *) l;

          if (local->fusion_id == from_id) {
               if (local->refs) {
                    local->refs++;
               }
               break;
          }
     }

     return ret;
}

static void free_all_local(FusionRef * ref)
{
     FusionLink *l = ref->local_refs;

     while (l) {
          FusionLink *next = l->next;

          fusion_core_free( fusion_core, l);

          l = next;
     }

     ref->local_refs = NULL;
}

static void notify_ref(FusionDev * dev, FusionRef * ref, bool async)
{
     if (ref->watched) {
          FusionCallExecute execute;

          execute.call_id = ref->call_id;
          execute.call_arg = ref->call_arg;
          execute.call_ptr = NULL;
          execute.flags = (!async && ref->syncwatch) ? FCEF_NONE : FCEF_ONEWAY;

          fusion_call_execute(dev, NULL, &execute);
     }
     else
          fusion_core_wq_wake( fusion_core, &ref->entry.wait);
}

static int propagate_local(FusionDev * dev, FusionRef * ref, int diff, bool async)
{
     FusionLink *l;

     /* Recurse into inheritors. */
     fusion_list_foreach(l, ref->inheritors) {
          FusionRef *inheritor = ((Inheritor *) l)->ref;

          propagate_local(dev, inheritor, diff, async);
     }

     /* Apply difference. */
     ref->local += diff;

     FUSION_ASSERT( ref->local >= 0);

     /* Notify zero count. */
     if (ref->local + ref->global == 0)
          notify_ref(dev, ref, async);

     return 0;
}

static int add_inheritor(FusionRef * ref, FusionRef * from)
{
     Inheritor *inheritor;

     inheritor = fusion_core_malloc( fusion_core, sizeof(Inheritor) );
     if (!inheritor)
          return -ENOMEM;

     inheritor->ref = ref;

     fusion_list_prepend(&from->inheritors, &inheritor->link);

     return 0;
}

static void remove_inheritor(FusionRef * ref, FusionRef * from)
{
     FusionLink *l;

     fusion_list_foreach(l, from->inheritors) {
          Inheritor *inheritor = (Inheritor *) l;

          if (inheritor->ref == ref) {
               fusion_list_remove(&from->inheritors, &inheritor->link);

               fusion_core_free( fusion_core, l);
               break;
          }
     }
}

static void drop_inheritors(FusionDev * dev, FusionRef * ref)
{
     FusionLink *l = ref->inheritors;

     while (l) {
          FusionLink *next = l->next;
          FusionRef *inheritor = ((Inheritor *) l)->ref;

          propagate_local(dev, inheritor, -ref->local, true);

          inheritor->inherited = NULL;

          fusion_core_free( fusion_core, l);

          l = next;
     }

     ref->inheritors = NULL;
}

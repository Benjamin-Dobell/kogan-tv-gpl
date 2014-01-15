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
#include "entries.h"
#include "fusionee.h"
#include "list.h"
#include "property.h"

typedef enum {
     FUSION_PROPERTY_AVAILABLE = 0,
     FUSION_PROPERTY_LEASED,
     FUSION_PROPERTY_PURCHASED
} FusionPropertyState;

typedef struct {
     FusionEntry entry;

     FusionPropertyState state;
     int fusion_id;      /* non-zero if leased/purchased */
     unsigned long purchase_stamp;
     int lock_pid;
     int count;          /* lock counter */
} FusionProperty;

static void
fusion_property_print(FusionEntry * entry, void *ctx, struct seq_file *p)
{
     FusionProperty *property = (FusionProperty *) entry;

     if (property->state != FUSION_PROPERTY_AVAILABLE) {
          seq_printf(p, "%s by 0x%08x (%d) %dx\n",
                     property->state ==
                     FUSION_PROPERTY_LEASED ? "leased" : "purchased",
                     property->fusion_id, property->lock_pid,
                     property->count);
          return;
     }

     seq_printf(p, "\n");
}

FUSION_ENTRY_CLASS(FusionProperty, property, NULL, NULL, fusion_property_print)

/******************************************************************************/
int fusion_property_init(FusionDev * dev)
{
     fusion_entries_init(&dev->properties, &property_class, dev, dev);

     fusion_entries_create_proc_entry(dev, "properties", &dev->properties);

     return 0;
}

void fusion_property_deinit(FusionDev * dev)
{
     fusion_entries_destroy_proc_entry( dev, "properties" );

     fusion_entries_deinit(&dev->properties);
}

/******************************************************************************/

int fusion_property_new(FusionDev * dev, Fusionee *fusionee, int *ret_id)
{
     return fusion_entry_create(&dev->properties, ret_id, NULL, fusionee_id(fusionee));
}

int fusion_property_lease(FusionDev * dev, int id, int fusion_id)
{
     int ret;
     FusionProperty *property;
     int timeout = -1;

     dev->stat.property_lease_purchase++;

     ret = fusion_property_lookup(&dev->properties, id, &property);
     if (ret)
          return ret;

     while (true) {
          switch (property->state) {
               case FUSION_PROPERTY_AVAILABLE:
                    property->state = FUSION_PROPERTY_LEASED;
                    property->fusion_id = fusion_id;
                    property->lock_pid = fusion_core_pid( fusion_core );
                    property->count = 1;

                    return 0;

               case FUSION_PROPERTY_LEASED:
                    if (property->lock_pid == fusion_core_pid( fusion_core )) {
                         property->count++;

                         return 0;
                    }

                    ret = fusion_property_wait(property, NULL);
                    if (ret)
                         return ret;

                    break;

               case FUSION_PROPERTY_PURCHASED:
                    if (property->lock_pid == fusion_core_pid( fusion_core ))
                         return -EIO;

                    if (timeout == -1) {
                         // FIXME: add fusion_core_jiffies()
                         if (jiffies - property->purchase_stamp > HZ / 10)
                              return -EAGAIN;

                         timeout = HZ / 10;
                    }

                    ret = fusion_property_wait(property, &timeout);
                    if (ret)
                         return ret;

                    break;

               default:
                    BUG();
          }
     }

     BUG();

     /* won't reach this */
     return -1;
}

int fusion_property_purchase(FusionDev * dev, int id, int fusion_id)
{
     int ret;
     FusionProperty *property;
     int timeout = -1;

     dev->stat.property_lease_purchase++;

     ret = fusion_property_lookup(&dev->properties, id, &property);
     if (ret)
          return ret;

     while (true) {
          switch (property->state) {
               case FUSION_PROPERTY_AVAILABLE:
                    property->state = FUSION_PROPERTY_PURCHASED;
                    property->fusion_id = fusion_id;
                    property->purchase_stamp = jiffies;
                    property->lock_pid = fusion_core_pid( fusion_core );
                    property->count = 1;

                    fusion_property_notify(property);
                    return 0;

               case FUSION_PROPERTY_LEASED:
                    if (property->lock_pid == fusion_core_pid( fusion_core ))
                         return -EIO;

                    ret = fusion_property_wait(property, NULL);
                    if (ret)
                         return ret;

                    break;

               case FUSION_PROPERTY_PURCHASED:
                    if (property->lock_pid == fusion_core_pid( fusion_core )) {
                         property->count++;

                         return 0;
                    }

                    if (timeout == -1) {
                         if (jiffies - property->purchase_stamp > HZ)
                              return -EAGAIN;

                         timeout = HZ;
                    }

                    ret = fusion_property_wait(property, &timeout);
                    if (ret)
                         return ret;

                    break;

               default:
                    BUG();
          }
     }

     BUG();

     /* won't reach this */
     return -1;
}

int fusion_property_cede(FusionDev * dev, int id, int fusion_id)
{
     int ret;
     FusionProperty *property;
     bool purchased;

     dev->stat.property_cede++;

     ret = fusion_property_lookup(&dev->properties, id, &property);
     if (ret)
          return ret;

     if (property->lock_pid != fusion_core_pid( fusion_core ))
          return -EIO;

     if (--property->count)
          return 0;

     purchased = (property->state == FUSION_PROPERTY_PURCHASED);

     property->state = FUSION_PROPERTY_AVAILABLE;
     property->fusion_id = 0;
     property->lock_pid = 0;

     fusion_property_notify(property);

     return 0;
}

int fusion_property_holdup(FusionDev * dev, int id, Fusionee * fusionee)
{
     int ret;
     FusionProperty *property;
     FusionID fusion_id = fusionee_id(fusionee);

     if (fusion_id > 1)
          return -EPERM;

     ret = fusion_property_lookup(&dev->properties, id, &property);
     if (ret)
          return ret;

     if (property->state == FUSION_PROPERTY_PURCHASED) {
          if (property->fusion_id == fusion_id)
               return -EIO;

          fusionee_kill(dev, fusionee, property->fusion_id, SIGKILL, -1);
     }

     return 0;
}

int fusion_property_destroy(FusionDev * dev, int id)
{
     return fusion_entry_destroy(&dev->properties, id);
}

void fusion_property_cede_all(FusionDev * dev, int fusion_id)
{
     FusionLink *l;

     fusion_list_foreach(l, dev->properties.list) {
          FusionProperty *property = (FusionProperty *) l;

          if (property->fusion_id == fusion_id) {
               property->state = FUSION_PROPERTY_AVAILABLE;
               property->fusion_id = 0;
               property->lock_pid = 0;

               fusion_core_wq_wake( fusion_core, &property->entry.wait);
          }
     }
}

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
#include <linux/mm.h>
#include <linux/proc_fs.h>

#include <linux/fusion.h>

#include <asm/io.h>

#include "fusiondev.h"
#include "fusionee.h"
#include "list.h"
#include "shmpool.h"

typedef struct {
     FusionLink link;
     void *next_base;
} AddrEntry;

typedef struct {
     FusionLink link;

     FusionID fusion_id;

     int count;          /* number of attach calls */
} SHMPoolNode;

typedef struct {
     FusionEntry entry;

     int max_size;

     void *addr_base;
     int size;

     AddrEntry *addr_entry;

     FusionLink *nodes;

     int dispatch_count;

#ifdef FUSION_CORE_SHMPOOLS
     void *kernel_base;
#endif
} FusionSHMPool;

/******************************************************************************/

static SHMPoolNode *get_node(FusionSHMPool * shmpool, FusionID fusion_id);

static void remove_node(FusionSHMPool * shmpool, FusionID fusion_id);

static int fork_node(FusionSHMPool * shmpool,
                     FusionID fusion_id, FusionID from_id);

static void free_all_nodes(FusionSHMPool * shmpool);

/******************************************************************************/


/******************************************************************************/

static AddrEntry *
add_addr_entry( FusionDev *dev, void *next_base )
{
     AddrEntry    *entry  = fusion_core_malloc( fusion_core, sizeof(AddrEntry) );
     FusionShared *shared = dev->shared;

     entry->next_base = next_base;

     fusion_list_prepend( &shared->addr_entries, &entry->link );

     return entry;
}

/******************************************************************************/

static int
fusion_shmpool_construct( FusionEntry * entry, void *ctx, void *create_ctx )
{
     FusionSHMPool    *shmpool = (FusionSHMPool *) entry;
     FusionDev        *dev     = (FusionDev *)ctx;
     FusionSHMPoolNew *poolnew = create_ctx;
     FusionShared     *shared  = dev->shared;

     if ((ulong) shared->addr_base + poolnew->max_size >= fusion_shm_base + fusion_shm_size) {
          printk(KERN_WARNING
                 "%s: virtual address space exhausted! (FIXME)\n",
                 __FUNCTION__);
          return -ENOSPC;
     }

#ifdef FUSION_CORE_SHMPOOLS
     shmpool->kernel_base = fusion_core_malloc(fusion_core, poolnew->max_size);
     if(!shmpool->kernel_base)
          return -ENOMEM;
#endif

     shmpool->max_size = poolnew->max_size;
     shmpool->addr_base = poolnew->addr_base = shared->addr_base;

     shared->addr_base += PAGE_ALIGN(poolnew->max_size) + PAGE_SIZE;
     shared->addr_base = (void*)((unsigned long)(shared->addr_base + 0xffff) & ~0xffff);

     shmpool->addr_entry = add_addr_entry( dev, shared->addr_base );

     return 0;
}

static void
fusion_shmpool_destruct( FusionEntry * entry, void *ctx )
{
     AddrEntry     *addr_entry;
     FusionSHMPool *shmpool = (FusionSHMPool *) entry;
     FusionDev     *dev     = (FusionDev *) ctx;
     FusionShared  *shared  = dev->shared;

     free_all_nodes(shmpool);

     fusion_list_remove( &shared->addr_entries, &shmpool->addr_entry->link );

     /*
      * free trailing address space
      */
#ifdef FUSION_CORE_SHMPOOLS
     fusion_core_free(fusion_core, shmpool->kernel_base);
#endif

     shared->addr_base = (void*) fusion_shm_base + 0x80000;

     fusion_list_foreach(addr_entry, shared->addr_entries) {
          if (addr_entry->next_base > shared->addr_base)
               shared->addr_base = addr_entry->next_base;
     }
}

static void
fusion_shmpool_print(FusionEntry * entry, void *ctx, struct seq_file *p)
{
     int num = 0;
     FusionSHMPool *shmpool = (FusionSHMPool *) entry;
     FusionLink *node = shmpool->nodes;

     fusion_list_foreach(node, shmpool->nodes) {
          num++;
     }

     seq_printf(p, "0x%p [0x%x] - 0x%x, %dx dispatch, %d nodes\n",
                shmpool->addr_base, shmpool->max_size, shmpool->size,
                shmpool->dispatch_count, num);
}

FUSION_ENTRY_CLASS(FusionSHMPool, shmpool, fusion_shmpool_construct,
                   fusion_shmpool_destruct, fusion_shmpool_print)

/******************************************************************************/
int fusion_shmpool_init(FusionDev * dev)
{
     fusion_entries_init(&dev->shmpool, &shmpool_class, dev, dev);

     fusion_entries_create_proc_entry(dev, "shmpools", &dev->shmpool);

     return 0;
}

void fusion_shmpool_deinit(FusionDev * dev)
{
     fusion_entries_destroy_proc_entry( dev, "shmpools" );

     fusion_entries_deinit(&dev->shmpool);
}

/******************************************************************************/

int fusion_shmpool_new(FusionDev * dev, Fusionee *fusionee, FusionSHMPoolNew * pool)
{
     if (pool->max_size <= 0)
          return -EINVAL;

     return fusion_entry_create(&dev->shmpool, &pool->pool_id, pool, fusionee_id(fusionee));
}

int
fusion_shmpool_attach(FusionDev * dev,
                      FusionSHMPoolAttach * attach, FusionID fusion_id)
{
     int ret;
     SHMPoolNode *node;
     FusionSHMPool *shmpool;

     ret = fusion_shmpool_lookup( &dev->shmpool, attach->pool_id, &shmpool );
     if (ret)
          return ret;

     dev->stat.shmpool_attach++;

     node = get_node(shmpool, fusion_id);
     if (!node) {
          node = fusion_core_malloc( fusion_core, sizeof(SHMPoolNode) );
          if (!node)
               return -ENOMEM;

          node->fusion_id = fusion_id;
          node->count = 1;

          fusion_list_prepend(&shmpool->nodes, &node->link);
     }
     else
          node->count++;

     attach->addr_base = shmpool->addr_base;
     attach->size = shmpool->size;

     return 0;
}

int fusion_shmpool_detach(FusionDev * dev, int id, FusionID fusion_id)
{
     int ret;
     SHMPoolNode *node;
     FusionSHMPool *shmpool;

     ret = fusion_shmpool_lookup(&dev->shmpool, id, &shmpool);
     if (ret)
          return ret;

     dev->stat.shmpool_detach++;

     node = get_node(shmpool, fusion_id);
     if (!node)
          return -EIO;

     if (!--node->count) {
          fusion_list_remove(&shmpool->nodes, &node->link);
          fusion_core_free( fusion_core, node);
     }

     return 0;
}

int
fusion_shmpool_dispatch(FusionDev * dev,
                        FusionSHMPoolDispatch * dispatch, Fusionee * fusionee)
{
     int ret;
     FusionLink *l;
     FusionSHMPool *shmpool;
     FusionSHMPoolMessage message;
     FusionID fusion_id = fusionee_id(fusionee);

     if (dispatch->size <= 0)
          return -EINVAL;

     ret = fusion_shmpool_lookup( &dev->shmpool, dispatch->pool_id, &shmpool );
     if (ret)
          return ret;

     message.type = FSMT_REMAP;
     message.size = dispatch->size;

     shmpool->dispatch_count++;

     shmpool->size = dispatch->size;

     fusion_list_foreach(l, shmpool->nodes) {
          SHMPoolNode *node = (SHMPoolNode *) l;

          if (node->fusion_id == fusion_id)
               continue;

          fusionee_send_message(dev, fusionee, node->fusion_id,
                                FMT_SHMPOOL, shmpool->entry.id, 0,
                                sizeof(message), &message, FMC_NONE, NULL, 0, NULL, 0);
     }

     return 0;
}

int fusion_shmpool_destroy(FusionDev * dev, int id)
{
     return fusion_entry_destroy(&dev->shmpool, id);
}

void fusion_shmpool_detach_all(FusionDev * dev, FusionID fusion_id)
{
     FusionLink *l;

     fusion_list_foreach(l, dev->shmpool.list) {
          FusionSHMPool *shmpool = (FusionSHMPool *) l;

          remove_node(shmpool, fusion_id);
     }
}

int
fusion_shmpool_fork_all(FusionDev * dev, FusionID fusion_id, FusionID from_id)
{
     FusionLink *l;
     int ret = 0;

     fusion_list_foreach(l, dev->shmpool.list) {
          FusionSHMPool *shmpool = (FusionSHMPool *) l;

          ret = fork_node(shmpool, fusion_id, from_id);
          if (ret)
               break;
     }

     return ret;
}

#ifdef FUSION_CORE_SHMPOOLS
int
fusion_shmpool_map(FusionDev * dev, struct vm_area_struct *vma)
{
     int ret;
     FusionSHMPool *shmpool;

     ret = fusion_shmpool_lookup( &dev->shmpool, vma->vm_pgoff, &shmpool );
     if (ret)
          return ret;

     // FIXME: THIS WON'T WORK WITH NON-PHYSICALLY CONTIGUOUS MEMORY!

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
     ret = remap_pfn_range(vma, vma->vm_start,
                           virt_to_phys((void *)shmpool->kernel_base) >> PAGE_SHIFT,
                           vma->vm_end - vma->vm_start, vma->vm_page_prot);
#else
     ret = io_remap_page_range(vma->vm_start,
                               virt_to_phys((void *)shmpool->kernel_base),
                               vma->vm_end - vma->vm_start, vma->vm_page_prot);
#endif

     return ret;
}
#endif

/******************************************************************************/

static SHMPoolNode *get_node(FusionSHMPool * shmpool, FusionID fusion_id)
{
     SHMPoolNode *node;

     fusion_list_foreach(node, shmpool->nodes) {
          if (node->fusion_id == fusion_id)
               return node;
     }

     return NULL;
}

static void remove_node(FusionSHMPool * shmpool, FusionID fusion_id)
{
     SHMPoolNode *node;

     fusion_list_foreach(node, shmpool->nodes) {
          if (node->fusion_id == fusion_id) {
               fusion_list_remove(&shmpool->nodes, &node->link);
               break;
          }
     }
}

static int
fork_node(FusionSHMPool * shmpool, FusionID fusion_id, FusionID from_id)
{
     int ret = 0;
     SHMPoolNode *node;

     fusion_list_foreach(node, shmpool->nodes) {
          if (node->fusion_id == from_id) {
               SHMPoolNode *new_node;

               new_node = fusion_core_malloc( fusion_core, sizeof(SHMPoolNode) );
               if (!new_node) {
                    ret = -ENOMEM;
                    break;
               }

               new_node->fusion_id = fusion_id;
               new_node->count = node->count;

               fusion_list_prepend(&shmpool->nodes, &new_node->link);

               break;
          }
     }

     return ret;
}

static void free_all_nodes(FusionSHMPool * shmpool)
{
     FusionLink *n;
     SHMPoolNode *node;

     fusion_list_foreach_safe(node, n, shmpool->nodes) {
          fusion_core_free( fusion_core, node);
     }

     shmpool->nodes = NULL;
}

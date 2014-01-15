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

//#define FUSION_ENABLE_DEBUG

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

#include <linux/fusion.h>

#include "fusiondev.h"
#include "fusionee.h"
#include "list.h"
#include "skirmish.h"
#include "call.h"

typedef struct {
     FusionLink link;

     Fusionee *caller;

     int ret_val;

     bool executed;
     bool signalled;

     FusionWaitQueue wait;

     int call_id;
     unsigned int serial;
     int          caller_pid;

     unsigned int ret_size;
     unsigned int ret_length;

     /* return data follows */
} FusionCallExecution;

typedef struct {
     FusionEntry entry;

     Fusionee *fusionee;      /* owner */

     void *handler;
     void *ctx;

     FusionLink *executions;

     int count;          /* number of calls ever made */

     unsigned int serial;
} FusionCall;

/* collection, required for 1-param-only passing */
struct fusion_construct_ctx {
     Fusionee *fusionee;
     FusionCallNew *call_new;
};

/******************************************************************************/

static FusionCallExecution *add_execution(FusionCall * call,
                                          Fusionee * caller,
                                          unsigned int serial,
                                          unsigned int ret_size);
static void remove_execution(FusionCall * call,
                             FusionCallExecution * execution);
static void free_execution(FusionDev * dev,
                           FusionCallExecution * execution);
static void free_all_executions(FusionCall * call);

/******************************************************************************/

static int
fusion_call_construct(FusionEntry * entry, void *ctx, void *create_ctx)
{
     FusionCall *call = (FusionCall *) entry;

     struct fusion_construct_ctx *cc =
     (struct fusion_construct_ctx *)create_ctx;

     call->fusionee = cc->fusionee;
     call->handler = cc->call_new->handler;
     call->ctx = cc->call_new->ctx;

     cc->call_new->call_id = entry->id;

     return 0;
}

static void fusion_call_destruct(FusionEntry * entry, void *ctx)
{
     FusionCall *call = (FusionCall *) entry;

     free_all_executions(call);
}

__attribute__((unused))
static void print_call( FusionCall* call )
{
     FusionEntry *entry;
     FusionLink *e;

     entry = &call->entry;

     printk( KERN_CRIT "%-2d %s, fid:%lu, %d calls)",
             entry->id,
             entry->name[0] ? entry->name : "???",
             call->fusionee->id,
             call->count);

     fusion_list_foreach(e, call->executions) {
          FusionCallExecution *exec = (FusionCallExecution *) e;

          printk( "/%lx:%s",
                  exec->caller   ? fusionee_id(exec->caller) : -1,
                  exec->executed ? "idle" : "busy" );
     }

     printk("\n");
}

static void
fusion_call_print(FusionEntry * entry, void *ctx, struct seq_file *p)
{
     FusionLink *e;
     bool idle = true;
     FusionCall *call = (FusionCall *) entry;

     if (call->executions)
          idle = ((FusionCallExecution *) call->executions)->executed;

     seq_printf(p, "(%d calls) %s",
                call->count, idle ? "idle" : "executing");

     fusion_list_foreach(e, call->executions) {
          FusionCallExecution *exec = (FusionCallExecution *) e;

          seq_printf(p, "  [0x%08lx]",
                     exec->caller ? fusionee_id(exec->caller) : 0);
     }

     seq_printf(p, "\n");
}

FUSION_ENTRY_CLASS(FusionCall, call, fusion_call_construct,
                   fusion_call_destruct, fusion_call_print)

/******************************************************************************/

int fusion_call_init(FusionDev * dev)
{
     fusion_entries_init(&dev->call, &call_class, dev, dev);

     fusion_entries_create_proc_entry(dev, "calls", &dev->call);

     return 0;
}

void fusion_call_deinit(FusionDev * dev)
{
     fusion_entries_destroy_proc_entry(dev, "calls");

     fusion_entries_deinit(&dev->call);
}

/******************************************************************************/

int fusion_call_new(FusionDev * dev, Fusionee *fusionee, FusionCallNew * call_new)
{
     int id;
     int ret;

     struct fusion_construct_ctx cc = { fusionee, call_new};

     ret = fusion_entry_create(&dev->call, &id, &cc, fusionee_id(fusionee));
     if (ret)
          return ret;

     return 0;
}

int
fusion_call_execute(FusionDev * dev, Fusionee * fusionee,
                    FusionCallExecute * execute)
{
     int ret;
     FusionCall *call;
     FusionCallExecution *execution = NULL;
     FusionCallMessage message;
     unsigned int serial;
     bool flush = true;

     FUSION_DEBUG( "%s( dev %p, fusionee %p, execute %p )\n", __FUNCTION__, dev, fusionee, execute );

     /* Lookup and lock call. */
     ret = fusion_call_lookup(&dev->call, execute->call_id, &call);
     if (ret)
          return ret;

     FUSION_DEBUG( "  -> call %u '%s'\n", call->entry.id, call->entry.name );

     if (execute->flags & FCEF_RESUMABLE && execute->serial != 0) {
          /* Search for execution, starting with oldest. */
          direct_list_foreach (execution, call->executions) {
               if (execution->serial == execute->serial)
                    break;
          }

          if (!execution)
               return -EIDRM;
     }
     else {
          do {
               serial = ++call->serial;
          } while (!serial);

          /* Add execution to receive the result. */
          if (!(execute->flags & FCEF_ONEWAY)) {
               execution = add_execution(call, fusionee, serial, 0);
               if (!execution)
                    return -ENOMEM;

               FUSION_DEBUG( "  -> execution %p, serial %u\n", execution, execution->serial );
          }
          else if (execute->flags & FCEF_QUEUE)
               flush = false;

          /* Fill call message. */
          message.handler = call->handler;
          message.ctx = call->ctx;

          message.caller = fusionee ? fusionee_id(fusionee) : 0;

          message.call_arg = execute->call_arg;
          message.call_ptr = execute->call_ptr;

          message.serial = execution ? serial : 0;

          FUSION_DEBUG( "  -> sending call message, caller %u\n", message.caller );

          /* Put message into queue of callee. */
          ret = fusionee_send_message2(dev, fusionee, call->fusionee, FMT_CALL,
                                       call->entry.id, 0, sizeof(message),
                                       &message, FMC_NONE, NULL, 1, NULL, 0,
                                       flush);
          if (ret) {
               FUSION_DEBUG( "  -> MESSAGE SENDING FAILED! (ret %u)\n", ret );
               if (execution) {
                    remove_execution(call, execution);
                    free_execution(dev, execution);
               }
               return ret;
          }

          call->count++;
     }

     /* When waiting for a result... */
     if (execution) {
          FUSION_DEBUG( "  -> message sent, transfering all skirmishs...\n" );

          /* Transfer held skirmishs (locks). */
          if (fusionee && (!(execute->flags & FCEF_RESUMABLE) || execute->serial == 0))
               fusion_skirmish_transfer_all(dev, call->fusionee->id,
                                            fusionee_id(fusionee),
                                            fusion_core_pid( fusion_core ),
                                            execution->serial);

          while (!execution->executed) {
               /* Unlock call and wait for execution result. TODO: add timeout? */

               FUSION_DEBUG( "  -> skirmishs transferred, sleeping on call...\n" );

#ifdef FUSION_CALL_INTERRUPTIBLE
               fusion_core_wq_wait( fusion_core, &execution->wait, 0, true );

               if (signal_pending(current)) {
                    FUSION_DEBUG( "  -> woke up, SIGNAL PENDING!\n" );

                    execute->serial = execution->serial;

                    /* Indicate that a signal was received and execution won't be freed by caller. */
                    if (!(execute->flags & FCEF_RESUMABLE)) {
                         execution->signalled = true;

                         printk( KERN_EMERG "FusionIPC: Warning signal_pending(current) -> exit from fusion_call_execute() without FCEF_RESUMABLE. current->pid=%d, fid=0x%08x, call->fusion_id=%ld\n",
                                 current->pid,
                                 fusionee ? (int)(fusionee_id(fusionee)) : 0,
                                 call->fusionee->id );
                    }

                    return -EINTR;
               }
#else
               fusion_core_wq_wait( fusion_core, &execution->wait, 0, false );
#endif
          }

          /* Return result to calling process. */
          execute->ret_val = execution->ret_val;

          /* Free execution, which has already been removed by callee. */
          free_execution( dev, execution);

          FUSION_DEBUG( "  -> woke up, ret val %u, reclaiming skirmishs...\n", execute->ret_val );

          /* Reclaim skirmishs. */
          if (fusionee)
               fusion_skirmish_reclaim_all(dev, fusion_core_pid( fusion_core ));
     }
     else {
          FUSION_DEBUG( "  -> message sent, not waiting.\n" );
     }

     return 0;
}

int
fusion_call_execute2(FusionDev * dev, Fusionee * fusionee,
                     FusionCallExecute2 * execute)
{
     int ret;
     FusionCall *call;
     FusionCallExecution *execution = NULL;
     FusionCallMessage message;
     unsigned int serial;
     bool flush = true;

     FUSION_DEBUG( "%s( dev %p, fusionee %p, execute %p )\n", __FUNCTION__, dev, fusionee, execute );

     /* Lookup and lock call. */
     ret = fusion_call_lookup(&dev->call, execute->call_id, &call);
     if (ret)
          return ret;

     FUSION_DEBUG( "  -> call %u '%s'\n", call->entry.id, call->entry.name );

     if (execute->flags & FCEF_RESUMABLE && execute->serial != 0) {
          /* Search for execution, starting with oldest. */
          direct_list_foreach (execution, call->executions) {
               if (execution->serial == execute->serial)
                    break;
          }

          if (!execution)
               return -EIDRM;
     }
     else {
          do {
               serial = ++call->serial;
          } while (!serial);

          /* Add execution to receive the result. */
          if (!(execute->flags & FCEF_ONEWAY)) {
               execution = add_execution(call, fusionee, serial, 0);
               if (!execution)
                    return -ENOMEM;

               FUSION_DEBUG( "  -> execution %p, serial %u\n", execution, execution->serial );
          }
          else if (execute->flags & FCEF_QUEUE)
               flush = false;

          /* Fill call message. */
          message.handler = call->handler;
          message.ctx = call->ctx;

          message.caller = fusionee ? fusionee_id(fusionee) : 0;

          message.call_arg = execute->call_arg;
          message.call_ptr = NULL;

          message.serial = execution ? serial : 0;

          FUSION_DEBUG( "  -> sending call message, caller %u\n", message.caller );

          /* Put message into queue of callee. */
          ret = fusionee_send_message2(dev, fusionee, call->fusionee, FMT_CALL,
                                       call->entry.id, 0, sizeof(FusionCallMessage),
                                       &message, FMC_NONE, NULL, 1, execute->ptr, execute->length,
                                       flush);
          if (ret) {
               FUSION_DEBUG( "  -> MESSAGE SENDING FAILED! (ret %u)\n", ret );
               if (execution) {
                    remove_execution(call, execution);
                    free_execution( dev, execution);
               }
               return ret;
          }

          call->count++;
     }

     /* When waiting for a result... */
     if (execution) {
          FUSION_DEBUG( "  -> message sent, transfering all skirmishs...\n" );

          /* Transfer held skirmishs (locks). */
          if (fusionee && (!(execute->flags & FCEF_RESUMABLE) || execute->serial == 0))
               fusion_skirmish_transfer_all(dev, call->fusionee->id,
                                            fusionee_id(fusionee),
                                            fusion_core_pid( fusion_core ),
                                            execution->serial);

          while (!execution->executed) {
               /* Unlock call and wait for execution result. TODO: add timeout? */

               FUSION_DEBUG( "  -> skirmishs transferred, sleeping on call...\n" );

#ifdef FUSION_CALL_INTERRUPTIBLE
               fusion_core_wq_wait( fusion_core, &execution->wait, 0, true );

               if (signal_pending(current)) {
                    FUSION_DEBUG( "  -> woke up, SIGNAL PENDING!\n" );

                    execute->serial = execution->serial;

                    /* Indicate that a signal was received and execution won't be freed by caller. */
                    if (!(execute->flags & FCEF_RESUMABLE)) {
                         execution->signalled = true;

                         printk( KERN_EMERG "FusionIPC: Warning signal_pending(current) -> exit from fusion_call_execute() without FCEF_RESUMABLE. current->pid=%d, fid=0x%08x, call->fusion_id=%ld\n",
                                 current->pid,
                                 fusionee ? (int)(fusionee_id(fusionee)) : 0,
                                 call->fusionee->id );
                    }

                    return -EINTR;
               }
#else
               fusion_core_wq_wait( fusion_core, &execution->wait, 0, false );
#endif
          }

          /* Return result to calling process. */
          execute->ret_val = execution->ret_val;

          /* Free execution, which has already been removed by callee. */
          free_execution( dev, execution);

          FUSION_DEBUG( "  -> woke up, ret val %u, reclaiming skirmishs...\n", execute->ret_val );

          /* Reclaim skirmishs. */
          if (fusionee)
               fusion_skirmish_reclaim_all(dev, fusion_core_pid( fusion_core ));
     }
     else {
          FUSION_DEBUG( "  -> message sent, not waiting.\n" );
     }

     return 0;
}

int
fusion_call_return(FusionDev * dev, int fusion_id, FusionCallReturn * call_ret)
{
     int ret;
     FusionCall *call;
     FusionCallExecution *execution;

     if ( (dev->api.major >= 4) && (call_ret->serial == 0) )
          return -EOPNOTSUPP;

     /* Lookup and lock call. */
     ret = fusion_call_lookup(&dev->call, call_ret->call_id, &call);
     if (ret)
          return ret;

     /* Search for execution, starting with oldest. */
     direct_list_foreach (execution, call->executions) {
          if ((execution->executed)
              || (execution->call_id != call_ret->call_id)
              || ((dev->api.major >= 4)
                  && (execution->serial != call_ret->serial))) {
               continue;
          }

          /*
           * Check if caller received a signal while waiting for the result.
           *
           * TODO: This is not completely solved. Restarting the system call
           * should be possible without causing another execution.
           */
          if (execution->signalled) {
               /* Remove and free execution. */
               remove_execution(call, execution);
               free_execution(dev, execution);
               return -EIDRM;
          }

          /* Write result to execution. */
          execution->ret_val = call_ret->val;
          execution->executed = true;

          /* Remove execution, freeing is up to caller. */
          remove_execution(call, execution);

          /* FIXME: Caller might still have received a signal since check above. */
          FUSION_ASSERT(!execution->signalled);

          /* Return skirmishs. */
          fusion_skirmish_return_all(dev, fusion_id, execution->caller_pid, execution->serial);

          /* Wake up caller. */
          fusion_core_wq_wake( fusion_core, &execution->wait);

          return 0;
     }

     /* DirectFB 1.0.x does not handle one-way-calls properly */
     if (dev->api.major <= 3)
          return 0;

     return -ENOMSG;
}

int
fusion_call_execute3(FusionDev * dev, Fusionee * fusionee,
                     FusionCallExecute3 * execute)
{
     int ret;
     FusionCall *call;
     FusionCallExecution *execution = NULL;
     FusionCallMessage3 message;
     unsigned int serial;
     bool flush = true;

     FUSION_DEBUG( "%s( dev %p, fusionee %p, execute %p )\n", __FUNCTION__, dev, fusionee, execute );

     /* Lookup and lock call. */
     ret = fusion_call_lookup(&dev->call, execute->call_id, &call);
     if (ret)
          return ret;

     FUSION_DEBUG( "  -> call %u '%s'\n", call->entry.id, call->entry.name );

     if (execute->flags & FCEF_RESUMABLE && execute->serial != 0) {
          /* Search for execution, starting with oldest. */
          direct_list_foreach (execution, call->executions) {
               if (execution->serial == execute->serial)
                    break;
          }

          if (!execution)
               return -EIDRM;
     }
     else {
          do {
               serial = ++call->serial;
          } while (!serial);

          /* Add execution to receive the result. */
          if (!(execute->flags & FCEF_ONEWAY)) {
               execution = add_execution(call, fusionee, serial, execute->ret_length);
               if (!execution)
                    return -ENOMEM;

               FUSION_DEBUG( "  -> execution %p, serial %u\n", execution, execution->serial );
          }
          else if (execute->flags & FCEF_QUEUE)
               flush = false;

          /* Fill call message. */
          message.handler = call->handler;
          message.ctx = call->ctx;

          message.caller = fusionee ? fusionee_id(fusionee) : 0;

          message.call_arg    = execute->call_arg;
          message.call_ptr    = NULL;
          message.call_length = execute->length;
          message.ret_length  = execute->ret_length;

          message.serial = execution ? serial : 0;

          FUSION_DEBUG( "  -> sending call message, caller %u, ptr %p, length %u\n", message.caller, execute->ptr, execute->length );

          /* Put message into queue of callee. */
          ret = fusionee_send_message2(dev, fusionee, call->fusionee, FMT_CALL3,
                                       call->entry.id, 0, sizeof(FusionCallMessage3),
                                       &message, FMC_NONE, NULL, 1, execute->ptr, execute->length,
                                       flush);
          if (ret) {
               FUSION_DEBUG( "  -> MESSAGE SENDING FAILED! (ret %u)\n", ret );
               if (execution) {
                    remove_execution(call, execution);
                    free_execution(dev, execution);
               }
               return ret;
          }

          call->count++;
     }

     /* When waiting for a result... */
     if (execution) {
          FUSION_DEBUG( "  -> message sent, transfering all skirmishs...\n" );

          /* Transfer held skirmishs (locks). */
          if (fusionee && (!(execute->flags & FCEF_RESUMABLE) || execute->serial == 0))
               fusion_skirmish_transfer_all(dev, call->fusionee->id,
                                            fusionee_id(fusionee),
                                            fusion_core_pid( fusion_core ),
                                            execution->serial);

          while (!execution->executed) {
               /* Unlock call and wait for execution result. TODO: add timeout? */

               FUSION_DEBUG( "  -> skirmishs transferred, sleeping on call...\n" );

#ifdef FUSION_CALL_INTERRUPTIBLE
               fusion_core_wq_wait( fusion_core, &execution->wait, 0, true );

               if (signal_pending(current)) {
                    FUSION_DEBUG( "  -> woke up, SIGNAL PENDING!\n" );

                    execute->serial = execution->serial;

                    /* Indicate that a signal was received and execution won't be freed by caller. */
                    if (!(execute->flags & FCEF_RESUMABLE)) {
                         execution->signalled = true;

                         printk( KERN_EMERG "FusionIPC: Warning signal_pending(current) -> exit from fusion_call_execute() without FCEF_RESUMABLE. current->pid=%d, fid=0x%08x, call->fusion_id=%ld\n",
                                 current->pid,
                                 fusionee ? (int)(fusionee_id(fusionee)) : 0,
                                 call->fusionee->id );
                    }

                    return -EINTR;
               }
#else
               fusion_core_wq_wait( fusion_core, &execution->wait, 0, false );
#endif
          }

          /* Return result to calling process. */
          if (execution->ret_length) {
               FUSION_DEBUG( "  -> ret_length %u, ret_size %u, ret_ptr %p\n", execution->ret_length, execution->ret_size, execute->ret_ptr );

               FUSION_ASSERT( execution->ret_length <= execution->ret_size );

               if (copy_to_user( execute->ret_ptr, execution + 1, execution->ret_length )) {
                    FUSION_DEBUG( "  -> ERROR COPYING RETURN DATA TO USER!\n" );
                    ret = -EFAULT;
               }
          }
          else
               ret = -ENODATA;

          execute->ret_length = execution->ret_length;

          /* Free execution, which has already been removed by callee. */
          free_execution( dev, execution );

          FUSION_DEBUG( "  -> woke up, ret length %u, reclaiming skirmishs...\n", execute->ret_length );

          /* Reclaim skirmishs. */
          if (fusionee)
               fusion_skirmish_reclaim_all(dev, fusion_core_pid( fusion_core ));
     }
     else {
          FUSION_DEBUG( "  -> message sent, not waiting.\n" );
     }

     return ret;
}

int
fusion_call_return3(FusionDev * dev, int fusion_id, FusionCallReturn3 * call_ret)
{
     int ret;
     FusionCall *call;
     FusionCallExecution *execution;

     if ( (dev->api.major >= 4) && (call_ret->serial == 0) )
          return -EOPNOTSUPP;

     /* Lookup and lock call. */
     ret = fusion_call_lookup(&dev->call, call_ret->call_id, &call);
     if (ret)
          return ret;

     /* Search for execution, starting with oldest. */
     direct_list_foreach (execution, call->executions) {
          if ((execution->executed)
              || (execution->call_id != call_ret->call_id)
              || ((dev->api.major >= 4)
                  && (execution->serial != call_ret->serial))) {
               continue;
          }

          /*
           * Check if caller received a signal while waiting for the result.
           *
           * TODO: This is not completely solved. Restarting the system call
           * should be possible without causing another execution.
           */
          if (execution->signalled) {
               /* Remove and free execution. */
               remove_execution(call, execution);
               free_execution(dev, execution);
               return -EIDRM;
          }

          if (execution->ret_size < call_ret->length) {
               /* Remove and free execution. */
               remove_execution(call, execution);
               free_execution(dev, execution);
               return -E2BIG;
          }

          /* Write result to execution. */
          if (copy_from_user( execution + 1, call_ret->ptr, call_ret->length )) {
               /* Remove and free execution. */
               remove_execution(call, execution);
               free_execution(dev, execution);
               return -EFAULT;
          }

          execution->ret_length = call_ret->length;
          execution->executed = true;

          /* Remove execution, freeing is up to caller. */
          remove_execution(call, execution);

          /* FIXME: Caller might still have received a signal since check above. */
          FUSION_ASSERT(!execution->signalled);

          /* Return skirmishs. */
          fusion_skirmish_return_all(dev, fusion_id, execution->caller_pid, execution->serial);

          /* Wake up caller. */
          fusion_core_wq_wake( fusion_core, &execution->wait);

          return 0;
     }

     /* DirectFB 1.0.x does not handle one-way-calls properly */
     if (dev->api.major <= 3)
          return 0;

     return -ENOMSG;
}

int fusion_call_get_owner(FusionDev * dev, int call_id, FusionID *ret_fusion_id)
{
     int         ret;
     FusionCall *call;

     /* Lookup and lock call. */
     ret = fusion_call_lookup(&dev->call, call_id, &call);
     if (ret)
          return ret;

     *ret_fusion_id = call->entry.creator;

     return 0;
}

int fusion_call_destroy(FusionDev * dev, Fusionee *fusionee, int call_id)
{
     int ret;
     FusionCall *call;
     FusionCallExecution *execution;

     do {
          /* Wait for all messages being processed. */
          ret =
          fusionee_wait_processing(dev, fusionee->id, FMT_CALL, call_id);
          if (ret)
               return ret;

          ret = fusion_call_lookup(&dev->call, call_id, &call);
          if (ret)
               return ret;

          /* in secure mode, check if we own the call. */
          if (dev->secure && call->fusionee != fusionee)
               return -EIO;

          /* If an execution is pending... */
          execution = (FusionCallExecution *) call->executions;
          if (execution) {
               /* Unlock call and wait for execution. TODO: add timeout? */
               fusion_core_wq_wait( fusion_core, &execution->wait, 0, true);

               if (signal_pending(current))
                    return -EINTR;
          }
     } while (execution);

     fusion_entry_destroy_locked(call->entry.entries, &call->entry);

     return 0;
}

void fusion_call_destroy_all(FusionDev * dev, Fusionee *fusionee)
{
     FusionLink *l;

     FUSION_DEBUG( "%s( dev %p, fusion_id %lu )\n", __FUNCTION__, dev, fusionee->id );

     l = dev->call.list;

     while (l) {
          FusionLink *next = l->next;
          FusionCall *call = (FusionCall *) l;

          if (call->fusionee == fusionee) {
               FusionCallExecution *execution;

               /* If an execution is pending... */
               direct_list_foreach (execution, call->executions) {
                    // FIMXE: indicate to caller that fusionee did quit

                    /* Wake up uninterruptibly(!) waiting caller! */
                    fusion_core_wq_wake( fusion_core, &execution->wait );
               }

               fusion_entry_destroy_locked(call->entry.entries,
                                           &call->entry);
          }

          l = next;
     }
}

/******************************************************************************/

static FusionCallExecution *add_execution(FusionCall * call,
                                          Fusionee * caller,
                                          unsigned int serial,
                                          unsigned int ret_size)
{
     FusionCallExecution *execution;

     FUSION_DEBUG( "%s( call %p [%u], caller %p [%lu], serial %i )\n", __FUNCTION__, call, call->entry.id, caller, caller ? caller->id : 0, serial );

     /* Allocate execution. */

     if (ret_size < 20 && call->entry.entries->dev->execution_free_list_num > 0) {
          execution = (FusionCallExecution *)call->entry.entries->dev->execution_free_list;
          direct_list_remove( &call->entry.entries->dev->execution_free_list, &execution->link );
          call->entry.entries->dev->execution_free_list_num--;
     }
     else {
          execution = fusion_core_malloc( fusion_core, sizeof(FusionCallExecution) + (ret_size > CACHE_EXECUTIONS_DATA_LEN ? ret_size : CACHE_EXECUTIONS_DATA_LEN) );
     }
     if (!execution)
          return NULL;

     /* Initialize execution. */
     memset(execution, 0, sizeof(FusionCallExecution));

     execution->caller = caller;
     execution->caller_pid = fusion_core_pid( fusion_core );
     execution->call_id = call->entry.id;
     execution->serial = serial;
     execution->ret_size = ret_size;

     fusion_core_wq_init( fusion_core, &execution->wait);

     /* Add execution. */
     direct_list_append(&call->executions, &execution->link);

     return execution;
}

static void remove_execution( FusionCall * call, FusionCallExecution * execution )
{
     FUSION_DEBUG( "%s( call %p [%u], execution %p )\n", __FUNCTION__, call, call->entry.id, execution );

     fusion_list_remove( &call->executions, &execution->link );
}

static void free_execution( FusionDev * dev, FusionCallExecution * execution )
{
     FUSION_DEBUG( "%s( execution %p )\n", __FUNCTION__, execution );

     if (execution->ret_size <= CACHE_EXECUTIONS_DATA_LEN && dev->execution_free_list_num < CACHE_EXECUTIONS_NUM) {
          direct_list_append( &dev->execution_free_list, &execution->link );

          dev->execution_free_list_num++;
     }
     else
          fusion_core_free( fusion_core, execution );
}

static void free_all_executions(FusionCall * call)
{
     FusionCallExecution *execution, *next;

     FUSION_DEBUG( "%s( call %p [%u] )\n", __FUNCTION__, call, call->entry.id );

     direct_list_foreach_safe (execution, next, call->executions) {
          remove_execution( call, execution );

          fusion_core_wq_wake( fusion_core,  &execution->wait );

          if (!execution->caller)
               free_execution( call->entry.entries->dev,  execution );
     }
}


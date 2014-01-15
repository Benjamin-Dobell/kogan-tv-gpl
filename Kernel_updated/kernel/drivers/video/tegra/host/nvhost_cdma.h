/*
 * drivers/video/tegra/host/nvhost_cdma.h
 *
 * Tegra Graphics Host Command DMA
 *
 * Copyright (c) 2010-2012, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __NVHOST_CDMA_H
#define __NVHOST_CDMA_H

#include <linux/sched.h>
#include <linux/semaphore.h>

#include <linux/nvhost.h>
#include <mach/nvmap.h>
#include <linux/list.h>

#include "nvhost_acm.h"

struct nvhost_syncpt;
struct nvhost_userctx_timeout;
struct nvhost_job;

/*
 * cdma
 *
 * This is in charge of a host command DMA channel.
 * Sends ops to a push buffer, and takes responsibility for unpinning
 * (& possibly freeing) of memory after those ops have completed.
 * Producer:
 *	begin
 *		push - send ops to the push buffer
 *	end - start command DMA and enqueue handles to be unpinned
 * Consumer:
 *	update - call to update sync queue and push buffer, unpin memory
 */

struct nvmap_client_handle {
	struct nvmap_client *client;
	struct nvmap_handle *handle;
};

struct push_buffer {
	struct nvmap_handle_ref *mem;	/* handle to pushbuffer memory */
	u32 *mapped;			/* mapped pushbuffer memory */
	u32 phys;			/* physical address of pushbuffer */
	u32 fence;			/* index we've written */
	u32 cur;			/* index to write to */
	struct nvmap_client_handle *nvmap;
					/* nvmap handle for each opcode pair */
};

struct syncpt_buffer {
	struct nvmap_handle_ref *mem; /* handle to pushbuffer memory */
	u32 *mapped;		/* mapped gather buffer (at channel offset */
	u32 phys;		/* physical address (at channel offset) */
	u32 incr_per_buffer;	/* max # of incrs per GATHER */
	u32 words_per_incr;	/* # of DWORDS in buffer to incr a syncpt */
};

struct buffer_timeout {
	struct delayed_work wq;		/* work queue */
	bool initialized;		/* timer one-time setup flag */
	u32 syncpt_id;			/* buffer completion syncpt id */
	u32 syncpt_val;			/* syncpt value when completed */
	ktime_t start_ktime;		/* starting time */
	/* context timeout information */
	struct nvhost_hwctx *ctx;
	int clientid;
};

enum cdma_event {
	CDMA_EVENT_NONE,		/* not waiting for any event */
	CDMA_EVENT_SYNC_QUEUE_EMPTY,	/* wait for empty sync queue */
	CDMA_EVENT_PUSH_BUFFER_SPACE	/* wait for space in push buffer */
};

struct nvhost_cdma {
	struct mutex lock;		/* controls access to shared state */
	struct semaphore sem;		/* signalled when event occurs */
	enum cdma_event event;		/* event that sem is waiting for */
	unsigned int slots_used;	/* pb slots used in current submit */
	unsigned int slots_free;	/* pb slots free in current submit */
	unsigned int first_get;		/* DMAGET value, where submit begins */
	unsigned int last_put;		/* last value written to DMAPUT */
	struct push_buffer push_buffer;	/* channel's push buffer */
	struct syncpt_buffer syncpt_buffer; /* syncpt incr buffer */
	struct list_head sync_queue;	/* job queue */
	struct buffer_timeout timeout;	/* channel's timeout state/wq */
	bool running;
	bool torndown;
};

#define cdma_to_channel(cdma) container_of(cdma, struct nvhost_channel, cdma)
#define cdma_to_dev(cdma) nvhost_get_host(cdma_to_channel(cdma)->dev)
#define cdma_op(cdma) (cdma_to_dev(cdma)->op.cdma)
#define cdma_to_nvmap(cdma) ((cdma_to_dev(cdma))->nvmap)
#define pb_to_cdma(pb) container_of(pb, struct nvhost_cdma, push_buffer)
#define cdma_pb_op(cdma) (cdma_to_dev(cdma)->op.push_buffer)

int	nvhost_cdma_init(struct nvhost_cdma *cdma);
void	nvhost_cdma_deinit(struct nvhost_cdma *cdma);
void	nvhost_cdma_stop(struct nvhost_cdma *cdma);
int	nvhost_cdma_begin(struct nvhost_cdma *cdma, struct nvhost_job *job);
void	nvhost_cdma_push(struct nvhost_cdma *cdma, u32 op1, u32 op2);
#define NVHOST_CDMA_PUSH_GATHER_CTXSAVE 0xffffffff
void	nvhost_cdma_push_gather(struct nvhost_cdma *cdma,
		struct nvmap_client *client,
		struct nvmap_handle *handle, u32 op1, u32 op2);
void	nvhost_cdma_end(struct nvhost_cdma *cdma,
		struct nvhost_job *job);
void	nvhost_cdma_update(struct nvhost_cdma *cdma);
int	nvhost_cdma_flush(struct nvhost_cdma *cdma, int timeout);
void	nvhost_cdma_peek(struct nvhost_cdma *cdma,
		u32 dmaget, int slot, u32 *out);
unsigned int nvhost_cdma_wait_locked(struct nvhost_cdma *cdma,
		enum cdma_event event);
void nvhost_cdma_update_sync_queue(struct nvhost_cdma *cdma,
		struct nvhost_syncpt *syncpt, struct device *dev);
#endif

/*
 * drivers/video/tegra/host/chip_support.h
 *
 * Tegra Graphics Host Chip Support
 *
 * Copyright (c) 2011-2012, NVIDIA Corporation.
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
#ifndef _NVHOST_CHIP_SUPPORT_H_
#define _NVHOST_CHIP_SUPPORT_H_

#include <linux/types.h>
struct output;
struct nvhost_waitchk;
struct nvhost_userctx_timeout;
struct nvhost_master;
struct nvhost_channel;
struct nvmap_handle;
struct nvmap_client;
struct nvhost_hwctx;
struct nvhost_cdma;
struct nvhost_intr;
struct push_buffer;
struct nvhost_syncpt;
struct nvhost_master;
struct dentry;
struct nvhost_job;

struct nvhost_chip_support {
	struct {
		int (*init)(struct nvhost_channel *,
			    struct nvhost_master *,
			    int chid);
		int (*submit)(struct nvhost_job *job);
		int (*read3dreg)(struct nvhost_channel *channel,
				struct nvhost_hwctx *hwctx,
				u32 offset,
				u32 *value);
	} channel;

	struct {
		void (*start)(struct nvhost_cdma *);
		void (*stop)(struct nvhost_cdma *);
		void (*kick)(struct  nvhost_cdma *);
		int (*timeout_init)(struct nvhost_cdma *,
				    u32 syncpt_id);
		void (*timeout_destroy)(struct nvhost_cdma *);
		void (*timeout_teardown_begin)(struct nvhost_cdma *);
		void (*timeout_teardown_end)(struct nvhost_cdma *,
					     u32 getptr);
		void (*timeout_cpu_incr)(struct nvhost_cdma *,
					 u32 getptr,
					 u32 syncpt_incrs,
					 u32 syncval,
					 u32 nr_slots);
		void (*timeout_pb_incr)(struct nvhost_cdma *,
					u32 getptr,
					u32 syncpt_incrs,
					u32 nr_slots,
					bool exec_ctxsave);
	} cdma;

	struct {
		void (*reset)(struct push_buffer *);
		int (*init)(struct push_buffer *);
		void (*destroy)(struct push_buffer *);
		void (*push_to)(struct push_buffer *,
				struct nvmap_client *,
				struct nvmap_handle *,
				u32 op1, u32 op2);
		void (*pop_from)(struct push_buffer *,
				 unsigned int slots);
		u32 (*space)(struct push_buffer *);
		u32 (*putptr)(struct push_buffer *);
	} push_buffer;

	struct {
		void (*debug_init)(struct dentry *de);
		void (*show_channel_cdma)(struct nvhost_master *,
					  struct nvhost_channel *,
					  struct output *,
					  int chid);
		void (*show_channel_fifo)(struct nvhost_master *,
					  struct nvhost_channel *,
					  struct output *,
					  int chid);
		void (*show_mlocks)(struct nvhost_master *m,
				    struct output *o);

	} debug;

	struct {
		void (*reset)(struct nvhost_syncpt *, u32 id);
		void (*reset_wait_base)(struct nvhost_syncpt *, u32 id);
		void (*read_wait_base)(struct nvhost_syncpt *, u32 id);
		u32 (*update_min)(struct nvhost_syncpt *, u32 id);
		void (*cpu_incr)(struct nvhost_syncpt *, u32 id);
		int (*wait_check)(struct nvhost_syncpt *sp,
				  struct nvmap_client *nvmap,
				  u32 waitchk_mask,
				  struct nvhost_waitchk *wait,
				  int num_waitchk);
		void (*debug)(struct nvhost_syncpt *);
		const char * (*name)(struct nvhost_syncpt *, u32 id);
		int (*mutex_try_lock)(struct nvhost_syncpt *,
				      unsigned int idx);
		void (*mutex_unlock)(struct nvhost_syncpt *,
				     unsigned int idx);
	} syncpt;

	struct {
		void (*init_host_sync)(struct nvhost_intr *);
		void (*set_host_clocks_per_usec)(
			struct nvhost_intr *, u32 clocks);
		void (*set_syncpt_threshold)(
			struct nvhost_intr *, u32 id, u32 thresh);
		void (*enable_syncpt_intr)(struct nvhost_intr *, u32 id);
		void (*disable_all_syncpt_intrs)(struct nvhost_intr *);
		int  (*request_host_general_irq)(struct nvhost_intr *);
		void (*free_host_general_irq)(struct nvhost_intr *);
		int (*request_syncpt_irq)(struct nvhost_intr_syncpt *syncpt);
	} intr;

	struct {
		struct nvhost_device *(*get_nvhost_device)(struct nvhost_master *host,
			char *name);
	} nvhost_dev;
};

#endif /* _NVHOST_CHIP_SUPPORT_H_ */

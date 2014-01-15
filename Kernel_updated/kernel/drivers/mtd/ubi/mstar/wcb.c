////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2012 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#include <linux/slab.h>
#include <linux/crc32.h>
#include <linux/freezer.h>
#include <linux/kthread.h>
#include "../ubi.h"

struct ubi_wcb_work {
	struct list_head list;
	struct ubi_volume_desc *desc;
	int lnum;
	const void *buf;
	int offset;
	int len;
	int dtype;
	struct write_info write;
};

static int do_wcb_work(struct ubi_device *ubi)
{
	int err;
	struct ubi_wcb_work *wrk;
	struct ubi_volume *vol;

	//cond_resched();

	spin_lock(&ubi->wcb_lock);
	if (list_empty(&ubi->wcb_works)) {
		spin_unlock(&ubi->wcb_lock);
		return 0;
	}

	wrk = list_entry(ubi->wcb_works.next, struct ubi_wcb_work, list);
	list_del(&wrk->list);
	ubi->wcb_works_count -= 1;
	ubi_assert(ubi->wcb_works_count >= 0);
	spin_unlock(&ubi->wcb_lock);

	vol = wrk->desc->vol;
	//ubi_msg("do_wcb_work %d bytes to LEB %d:%d:%d, callback:%lX, priv:%lX", wrk->len, vol->vol_id, wrk->lnum, wrk->offset,
	//	(unsigned long)wrk->write.callback, (unsigned long)wrk->write.priv);
	
	err = ubi_eba_write_leb_cb(ubi, vol, wrk->lnum, wrk->buf, wrk->offset, wrk->len, wrk->dtype, &wrk->write);
	if(wrk->write.done == 0)
		wrk->write.callback(wrk->write.priv);

	kfree(wrk);

	return err;
}

static void schedule_ubi_wcb_work(struct ubi_device *ubi, struct ubi_wcb_work *wrk)
{
	spin_lock(&ubi->wcb_lock);
	list_add_tail(&wrk->list, &ubi->wcb_works);
	ubi_assert(ubi->wcb_works_count >= 0);
	ubi->wcb_works_count += 1;
	//ubi_msg("schedule_ubi_wcb_work: callback:%lX, priv:%lX", (unsigned long)wrk->write.callback, (unsigned long)wrk->write.priv);
	if (ubi->wcb_thread_enabled)
		wake_up_process(ubi->wcb_thread);
	spin_unlock(&ubi->wcb_lock);
}

static void wcb_callback(unsigned long priv)
{
	struct mutex *cb_mutex = (struct mutex *)priv;

	mutex_unlock(cb_mutex);
}

int ubi_leb_write_cb(struct ubi_volume_desc *desc, int lnum, const void *buf,
		  int offset, int len, int dtype)
{
	struct ubi_volume *vol = desc->vol;
	struct ubi_device *ubi = vol->ubi;
	int vol_id = vol->vol_id;
	struct ubi_wcb_work *wcb_wrk;
	struct mutex cb_mutex;

	//ubi_msg("ubi_leb_write_cb %d bytes to LEB %d:%d:%d", len, vol_id, lnum, offset);
	if (ubi->ro_mode)
			return -EROFS;

	if (vol_id < 0 || vol_id >= ubi->vtbl_slots)
		return -EINVAL;

	if (desc->mode == UBI_READONLY || vol->vol_type == UBI_STATIC_VOLUME)
		return -EROFS;

	if (lnum < 0 || lnum >= vol->reserved_pebs || offset < 0 || len < 0 ||
	    offset + len > vol->usable_leb_size ||
	    offset & (ubi->min_io_size - 1) || len & (ubi->min_io_size - 1))
		return -EINVAL;

	if (dtype != UBI_LONGTERM && dtype != UBI_SHORTTERM &&
	    dtype != UBI_UNKNOWN)
		return -EINVAL;

	if (vol->upd_marker)
		return -EBADF;

	if (len == 0)
		return 0;
	
	wcb_wrk = kmalloc(sizeof(struct ubi_wcb_work), GFP_NOFS);
	if (!wcb_wrk)
		return -ENOMEM;

	mutex_init(&cb_mutex);
	mutex_lock(&cb_mutex); // initial locked by caller

	wcb_wrk->desc = desc;
	wcb_wrk->lnum = lnum;
	wcb_wrk->buf = buf;
	wcb_wrk->offset = offset;
	wcb_wrk->len = len;
	wcb_wrk->dtype = dtype;
	wcb_wrk->write.callback = &wcb_callback;
	wcb_wrk->write.priv = (unsigned long)&cb_mutex;
	wcb_wrk->write.done = 0;

	schedule_ubi_wcb_work(ubi, wcb_wrk);
	mutex_lock(&cb_mutex); // lock again to wait wcb to unlock 
	mutex_unlock(&cb_mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(ubi_leb_write_cb);

int ubi_wcb_thread(void *u)
{
	struct ubi_device *ubi = u;

	ubi_msg("background thread \"%s\" started, PID %d",
		UBI_WCB_NAME, task_pid_nr(current));

	set_freezable();
	for (;;) {
		int err;

		if (kthread_should_stop())
			break;

		if (try_to_freeze())
			continue;

		spin_lock(&ubi->wcb_lock);
		if (list_empty(&ubi->wcb_works) || ubi->ro_mode ||
		    !ubi->wcb_thread_enabled) {
			set_current_state(TASK_INTERRUPTIBLE);
			spin_unlock(&ubi->wcb_lock);
			schedule();
			continue;
		}
		spin_unlock(&ubi->wcb_lock);

		err = do_wcb_work(ubi);
		if (err) {
			ubi_err("%s: work failed with error code %d",
				UBI_WCB_NAME, err);
				ubi_ro_mode(ubi);
				ubi->wcb_thread_enabled = 0;
				continue;
		}

		cond_resched();
	}

	ubi_msg("background thread \"%s\" is killed", UBI_WCB_NAME);
	return 0;
}

void ubi_wcb_close(struct ubi_device *ubi)
{
	while (!list_empty(&ubi->wcb_works)) {
		struct ubi_wcb_work *wrk;

		wrk = list_entry(ubi->wcb_works.next, struct ubi_wcb_work, list);
		list_del(&wrk->list);
		ubi->wcb_works_count -= 1;
		ubi_assert(ubi->wcb_works_count >= 0);
	}
}


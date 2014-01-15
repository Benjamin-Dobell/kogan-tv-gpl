/*
 * drivers/video/tegra/nvmap/nvmap.c
 *
 * Memory manager for Tegra GPU
 *
 * Copyright (c) 2009-2011, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/err.h>
#include <linux/highmem.h>
#include <linux/io.h>
#include <linux/rbtree.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <linux/slab.h>

#include <asm/pgtable.h>
#include <asm/tlbflush.h>

#include <mach/iovmm.h>
#include <mach/nvmap.h>

#include "nvmap.h"
#include "nvmap_mru.h"

/* private nvmap_handle flag for pinning duplicate detection */
#define NVMAP_HANDLE_VISITED (0x1ul << 31)

/* map the backing pages for a heap_pgalloc handle into its IOVMM area */
static void map_iovmm_area(struct nvmap_handle *h)
{
	tegra_iovmm_addr_t va;
	unsigned long i;

	BUG_ON(!h->heap_pgalloc || !h->pgalloc.area);
	BUG_ON(h->size & ~PAGE_MASK);
	WARN_ON(!h->pgalloc.dirty);

	for (va = h->pgalloc.area->iovm_start, i = 0;
	     va < (h->pgalloc.area->iovm_start + h->size);
	     i++, va += PAGE_SIZE) {
		BUG_ON(!pfn_valid(page_to_pfn(h->pgalloc.pages[i])));
		tegra_iovmm_vm_insert_pfn(h->pgalloc.area, va,
					  page_to_pfn(h->pgalloc.pages[i]));
	}
	h->pgalloc.dirty = false;
}

/* must be called inside nvmap_pin_lock, to ensure that an entire stream
 * of pins will complete without racing with a second stream. handle should
 * have nvmap_handle_get (or nvmap_validate_get) called before calling
 * this function. */
static int pin_locked(struct nvmap_client *client, struct nvmap_handle *h)
{
	struct tegra_iovmm_area *area;
	BUG_ON(!h->alloc);

	nvmap_mru_lock(client->share);
	if (atomic_inc_return(&h->pin) == 1) {
		if (h->heap_pgalloc && !h->pgalloc.contig) {
			area = nvmap_handle_iovmm_locked(client, h);
			if (!area) {
				/* no race here, inside the pin mutex */
				atomic_dec(&h->pin);
				nvmap_mru_unlock(client->share);
				return -ENOMEM;
			}
			if (area != h->pgalloc.area)
				h->pgalloc.dirty = true;
			h->pgalloc.area = area;
		}
	}
	nvmap_mru_unlock(client->share);
	return 0;
}

/* doesn't need to be called inside nvmap_pin_lock, since this will only
 * expand the available VM area */
static int handle_unpin(struct nvmap_client *client,
		struct nvmap_handle *h, int free_vm)
{
	int ret = 0;
	nvmap_mru_lock(client->share);

	if (atomic_read(&h->pin) == 0) {
		nvmap_err(client, "%s unpinning unpinned handle %p\n",
			  current->group_leader->comm, h);
		nvmap_mru_unlock(client->share);
		return 0;
	}

	BUG_ON(!h->alloc);

	if (!atomic_dec_return(&h->pin)) {
		if (h->heap_pgalloc && h->pgalloc.area) {
			/* if a secure handle is clean (i.e., mapped into
			 * IOVMM, it needs to be zapped on unpin. */
			if (h->secure && !h->pgalloc.dirty) {
				tegra_iovmm_zap_vm(h->pgalloc.area);
				h->pgalloc.dirty = true;
			}
			if (free_vm) {
				tegra_iovmm_free_vm(h->pgalloc.area);
				h->pgalloc.area = NULL;
			} else
				nvmap_mru_insert_locked(client->share, h);
			ret = 1;
		}
	}

	nvmap_mru_unlock(client->share);
	nvmap_handle_put(h);
	return ret;
}

static int pin_array_locked(struct nvmap_client *client,
		struct nvmap_handle **h, int count)
{
	int pinned;
	int i;
	int err = 0;

	for (pinned = 0; pinned < count; pinned++) {
		err = pin_locked(client, h[pinned]);
		if (err)
			break;
	}

	if (err) {
		/* unpin pinned handles */
		for (i = 0; i < pinned; i++) {
			/* inc ref counter, because
			 * handle_unpin decrements it */
			nvmap_handle_get(h[i]);
			/* unpin handles and free vm */
			handle_unpin(client, h[i], true);
		}
	}

	if (err && tegra_iovmm_get_max_free(client->share->iovmm) >=
							client->iovm_limit) {
		/* First attempt to pin in empty iovmm
		 * may still fail because of fragmentation caused by
		 * placing handles in MRU areas. After such failure
		 * all MRU gets cleaned and iovm space is freed.
		 *
		 * We have to do pinning again here since there might be is
		 * no more incoming pin_wait wakeup calls from unpin
		 * operations */
		for (pinned = 0; pinned < count; pinned++) {
			err = pin_locked(client, h[pinned]);
			if (err)
				break;
		}
		if (err) {
			pr_err("Pinning in empty iovmm failed!!!\n");
			BUG_ON(1);
		}
	}
	return err;
}

static int wait_pin_array_locked(struct nvmap_client *client,
		struct nvmap_handle **h, int count)
{
	int ret = 0;

	ret = pin_array_locked(client, h, count);

	if (ret) {
		ret = wait_event_interruptible(client->share->pin_wait,
				!pin_array_locked(client, h, count));
	}
	return ret ? -EINTR : 0;
}

static int handle_unpin_noref(struct nvmap_client *client, unsigned long id)
{
	struct nvmap_handle *h;
	int w;

	h = nvmap_validate_get(client, id);
	if (unlikely(!h)) {
		nvmap_err(client, "%s attempting to unpin invalid handle %p\n",
			  current->group_leader->comm, (void *)id);
		return 0;
	}

	nvmap_err(client, "%s unpinning unreferenced handle %p\n",
		  current->group_leader->comm, h);
	WARN_ON(1);

	w = handle_unpin(client, h, false);
	nvmap_handle_put(h);
	return w;
}

void nvmap_unpin_ids(struct nvmap_client *client,
		     unsigned int nr, const unsigned long *ids)
{
	unsigned int i;
	int do_wake = 0;

	for (i = 0; i < nr; i++) {
		struct nvmap_handle_ref *ref;

		if (!ids[i])
			continue;

		nvmap_ref_lock(client);
		ref = _nvmap_validate_id_locked(client, ids[i]);
		if (ref) {
			struct nvmap_handle *h = ref->handle;
			int e = atomic_add_unless(&ref->pin, -1, 0);

			nvmap_ref_unlock(client);

			if (!e) {
				nvmap_err(client, "%s unpinning unpinned "
					  "handle %08lx\n",
					  current->group_leader->comm, ids[i]);
			} else {
				do_wake |= handle_unpin(client, h, false);
			}
		} else {
			nvmap_ref_unlock(client);
			if (client->super)
				do_wake |= handle_unpin_noref(client, ids[i]);
			else
				nvmap_err(client, "%s unpinning invalid "
					  "handle %08lx\n",
					  current->group_leader->comm, ids[i]);
		}
	}

	if (do_wake)
		wake_up(&client->share->pin_wait);
}

/* pins a list of handle_ref objects; same conditions apply as to
 * _nvmap_handle_pin, but also bumps the pin count of each handle_ref. */
int nvmap_pin_ids(struct nvmap_client *client,
		  unsigned int nr, const unsigned long *ids)
{
	int ret = 0;
	unsigned int i;
	struct nvmap_handle **h = (struct nvmap_handle **)ids;
	struct nvmap_handle_ref *ref;

	/* to optimize for the common case (client provided valid handle
	 * references and the pin succeeds), increment the handle_ref pin
	 * count during validation. in error cases, the tree will need to
	 * be re-walked, since the handle_ref is discarded so that an
	 * allocation isn't required. if a handle_ref is not found,
	 * locally validate that the caller has permission to pin the handle;
	 * handle_refs are not created in this case, so it is possible that
	 * if the caller crashes after pinning a global handle, the handle
	 * will be permanently leaked. */
	nvmap_ref_lock(client);
	for (i = 0; i < nr && !ret; i++) {
		ref = _nvmap_validate_id_locked(client, ids[i]);
		if (ref) {
			atomic_inc(&ref->pin);
			nvmap_handle_get(h[i]);
		} else {
			struct nvmap_handle *verify;
			nvmap_ref_unlock(client);
			verify = nvmap_validate_get(client, ids[i]);
			if (verify)
				nvmap_warn(client, "%s pinning unreferenced "
					   "handle %p\n",
					   current->group_leader->comm, h[i]);
			else
				ret = -EPERM;
			nvmap_ref_lock(client);
		}
	}
	nvmap_ref_unlock(client);

	nr = i;

	if (ret)
		goto out;

	ret = mutex_lock_interruptible(&client->share->pin_lock);
	if (WARN_ON(ret))
		goto out;

	ret = wait_pin_array_locked(client, h, nr);

	mutex_unlock(&client->share->pin_lock);

	if (ret) {
		ret = -EINTR;
	} else {
		for (i = 0; i < nr; i++) {
			if (h[i]->heap_pgalloc && h[i]->pgalloc.dirty)
				map_iovmm_area(h[i]);
		}
	}

out:
	if (ret) {
		nvmap_ref_lock(client);
		for (i = 0; i < nr; i++) {
			ref = _nvmap_validate_id_locked(client, ids[i]);
			if (!ref) {
				nvmap_warn(client, "%s freed handle %p "
					   "during pinning\n",
					   current->group_leader->comm,
					   (void *)ids[i]);
				continue;
			}
			atomic_dec(&ref->pin);
		}
		nvmap_ref_unlock(client);

		for (i = 0; i < nr; i++)
			nvmap_handle_put(h[i]);
	}

	return ret;
}

static phys_addr_t handle_phys(struct nvmap_handle *h)
{
	phys_addr_t addr;

	if (h->heap_pgalloc && h->pgalloc.contig) {
		addr = page_to_phys(h->pgalloc.pages[0]);
	} else if (h->heap_pgalloc) {
		BUG_ON(!h->pgalloc.area);
		addr = h->pgalloc.area->iovm_start;
	} else {
		addr = h->carveout->base;
	}

	return addr;
}

/* stores the physical address (+offset) of each handle relocation entry
 * into its output location. see nvmap_pin_array for more details.
 *
 * each entry in arr (i.e., each relocation request) specifies two handles:
 * the handle to pin (pin), and the handle where the address of pin should be
 * written (patch). in pseudocode, this loop basically looks like:
 *
 * for (i = 0; i < nr; i++) {
 *     (pin, pin_offset, patch, patch_offset) = arr[i];
 *     patch[patch_offset] = address_of(pin) + pin_offset;
 * }
 */
static int nvmap_reloc_pin_array(struct nvmap_client *client,
				 const struct nvmap_pinarray_elem *arr,
				 int nr, struct nvmap_handle *gather)
{
	struct nvmap_handle *last_patch = NULL;
	unsigned int last_pfn = 0;
	pte_t **pte;
	void *addr;
	int i;

	pte = nvmap_alloc_pte(client->dev, &addr);
	if (IS_ERR(pte))
		return PTR_ERR(pte);

	for (i = 0; i < nr; i++) {
		struct nvmap_handle *patch;
		struct nvmap_handle *pin;
		phys_addr_t reloc_addr;
		phys_addr_t phys;
		unsigned int pfn;

		/* all of the handles are validated and get'ted prior to
		 * calling this function, so casting is safe here */
		pin = (struct nvmap_handle *)arr[i].pin_mem;

		if (arr[i].patch_mem == (unsigned long)last_patch) {
			patch = last_patch;
		} else if (arr[i].patch_mem == (unsigned long)gather) {
			patch = gather;
		} else {
			if (last_patch)
				nvmap_handle_put(last_patch);

			patch = nvmap_get_handle_id(client, arr[i].patch_mem);
			if (!patch) {
				nvmap_free_pte(client->dev, pte);
				return -EPERM;
			}
			last_patch = patch;
		}

		if (patch->heap_pgalloc) {
			unsigned int page = arr[i].patch_offset >> PAGE_SHIFT;
			phys = page_to_phys(patch->pgalloc.pages[page]);
			phys += (arr[i].patch_offset & ~PAGE_MASK);
		} else {
			phys = patch->carveout->base + arr[i].patch_offset;
		}

		pfn = __phys_to_pfn(phys);
		if (pfn != last_pfn) {
			pgprot_t prot = nvmap_pgprot(patch, pgprot_kernel);
			phys_addr_t kaddr = (phys_addr_t)addr;
			set_pte_at(&init_mm, kaddr, *pte, pfn_pte(pfn, prot));
			flush_tlb_kernel_page(kaddr);
			last_pfn = pfn;
		}

		reloc_addr = handle_phys(pin) + arr[i].pin_offset;
		reloc_addr >>= arr[i].reloc_shift;
		__raw_writel(reloc_addr, addr + (phys & ~PAGE_MASK));
	}

	nvmap_free_pte(client->dev, pte);

	if (last_patch)
		nvmap_handle_put(last_patch);

	wmb();

	return 0;
}

static int nvmap_validate_get_pin_array(struct nvmap_client *client,
					const struct nvmap_pinarray_elem *arr,
					int nr, struct nvmap_handle **h)
{
	int i;
	int ret = 0;
	int count = 0;

	nvmap_ref_lock(client);

	for (i = 0; i < nr; i++) {
		struct nvmap_handle_ref *ref;

		if (need_resched()) {
			nvmap_ref_unlock(client);
			schedule();
			nvmap_ref_lock(client);
		}

		ref = _nvmap_validate_id_locked(client, arr[i].pin_mem);

		if (!ref)
			nvmap_warn(client, "falied to validate id\n");
		else if (!ref->handle)
			nvmap_warn(client, "id had no associated handle\n");
		else if (!ref->handle->alloc)
			nvmap_warn(client, "handle had no allocation\n");

		if (!ref || !ref->handle || !ref->handle->alloc) {
			ret = -EPERM;
			break;
		}

		/* a handle may be referenced multiple times in arr, but
		 * it will only be pinned once; this ensures that the
		 * minimum number of sync-queue slots in the host driver
		 * are dedicated to storing unpin lists, which allows
		 * for greater parallelism between the CPU and graphics
		 * processor */
		if (ref->handle->flags & NVMAP_HANDLE_VISITED)
			continue;

		ref->handle->flags |= NVMAP_HANDLE_VISITED;

		h[count] = nvmap_handle_get(ref->handle);
		BUG_ON(!h[count]);
		count++;
	}

	nvmap_ref_unlock(client);

	if (ret) {
		for (i = 0; i < count; i++) {
			h[i]->flags &= ~NVMAP_HANDLE_VISITED;
			nvmap_handle_put(h[i]);
		}
	}

	return ret ?: count;
}

/* a typical mechanism host1x clients use for using the Tegra graphics
 * processor is to build a command buffer which contains relocatable
 * memory handle commands, and rely on the kernel to convert these in-place
 * to addresses which are understood by the GPU hardware.
 *
 * this is implemented by having clients provide a sideband array
 * of relocatable handles (+ offsets) and the location in the command
 * buffer handle to patch with the GPU address when the client submits
 * its command buffer to the host1x driver.
 *
 * the host driver also uses this relocation mechanism internally to
 * relocate the client's (unpinned) command buffers into host-addressable
 * memory.
 *
 * @client: nvmap_client which should be used for validation; should be
 *          owned by the process which is submitting command buffers
 * @gather: special handle for relocated command buffer outputs used
 *          internally by the host driver. if this handle is encountered
 *          as an output handle in the relocation array, it is assumed
 *          to be a known-good output and is not validated.
 * @arr:    array of ((relocatable handle, offset), (output handle, offset))
 *          tuples.
 * @nr:     number of entries in arr
 * @unique_arr: list of nvmap_handle objects which were pinned by
 *              nvmap_pin_array. must be unpinned by the caller after the
 *              command buffers referenced in gather have completed.
 */
int nvmap_pin_array(struct nvmap_client *client, struct nvmap_handle *gather,
		    const struct nvmap_pinarray_elem *arr, int nr,
		    struct nvmap_handle **unique_arr)
{
	int count = 0;
	int ret = 0;
	int i;

	if (mutex_lock_interruptible(&client->share->pin_lock)) {
		nvmap_warn(client, "%s interrupted when acquiring pin lock\n",
			   current->group_leader->comm);
		return -EINTR;
	}

	count = nvmap_validate_get_pin_array(client, arr, nr, unique_arr);
	if (count < 0) {
		mutex_unlock(&client->share->pin_lock);
		nvmap_warn(client, "failed to validate pin array\n");
		return count;
	}

	for (i = 0; i < count; i++)
		unique_arr[i]->flags &= ~NVMAP_HANDLE_VISITED;

	ret = wait_pin_array_locked(client, unique_arr, count);

	mutex_unlock(&client->share->pin_lock);

	if (!ret)
		ret = nvmap_reloc_pin_array(client, arr, nr, gather);

	if (WARN_ON(ret)) {
		for (i = 0; i < count; i++)
			nvmap_handle_put(unique_arr[i]);
		return ret;
	} else {
		for (i = 0; i < count; i++) {
			if (unique_arr[i]->heap_pgalloc &&
			    unique_arr[i]->pgalloc.dirty)
				map_iovmm_area(unique_arr[i]);
		}
	}

	return count;
}

phys_addr_t nvmap_pin(struct nvmap_client *client,
			struct nvmap_handle_ref *ref)
{
	struct nvmap_handle *h;
	phys_addr_t phys;
	int ret = 0;

	h = nvmap_handle_get(ref->handle);
	if (WARN_ON(!h))
		return -EINVAL;

	atomic_inc(&ref->pin);

	if (WARN_ON(mutex_lock_interruptible(&client->share->pin_lock))) {
		ret = -EINTR;
	} else {
		ret = wait_pin_array_locked(client, &h, 1);
		mutex_unlock(&client->share->pin_lock);
	}

	if (ret) {
		atomic_dec(&ref->pin);
		nvmap_handle_put(h);
	} else {
		if (h->heap_pgalloc && h->pgalloc.dirty)
			map_iovmm_area(h);
		phys = handle_phys(h);
	}

	return ret ?: phys;
}

phys_addr_t nvmap_handle_address(struct nvmap_client *c, unsigned long id)
{
	struct nvmap_handle *h;
	phys_addr_t phys;

	h = nvmap_get_handle_id(c, id);
	if (!h)
		return -EPERM;
	mutex_lock(&h->lock);
	phys = handle_phys(h);
	mutex_unlock(&h->lock);
	nvmap_handle_put(h);

	return phys;
}

void nvmap_unpin(struct nvmap_client *client, struct nvmap_handle_ref *ref)
{
	if (!ref)
		return;

	atomic_dec(&ref->pin);
	if (handle_unpin(client, ref->handle, false))
		wake_up(&client->share->pin_wait);
}

void nvmap_unpin_handles(struct nvmap_client *client,
			 struct nvmap_handle **h, int nr)
{
	int i;
	int do_wake = 0;

	for (i = 0; i < nr; i++) {
		if (WARN_ON(!h[i]))
			continue;
		do_wake |= handle_unpin(client, h[i], false);
	}

	if (do_wake)
		wake_up(&client->share->pin_wait);
}

void *nvmap_mmap(struct nvmap_handle_ref *ref)
{
	struct nvmap_handle *h;
	pgprot_t prot;
	unsigned long adj_size;
	unsigned long offs;
	struct vm_struct *v;
	void *p;

	h = nvmap_handle_get(ref->handle);
	if (!h)
		return NULL;

	prot = nvmap_pgprot(h, pgprot_kernel);

	if (h->heap_pgalloc)
		return vm_map_ram(h->pgalloc.pages, h->size >> PAGE_SHIFT,
				  -1, prot);

	/* carveout - explicitly map the pfns into a vmalloc area */

	nvmap_usecount_inc(h);

	adj_size = h->carveout->base & ~PAGE_MASK;
	adj_size += h->size;
	adj_size = PAGE_ALIGN(adj_size);

	v = alloc_vm_area(adj_size);
	if (!v) {
		nvmap_usecount_dec(h);
		nvmap_handle_put(h);
		return NULL;
	}

	p = v->addr + (h->carveout->base & ~PAGE_MASK);

	for (offs = 0; offs < adj_size; offs += PAGE_SIZE) {
		unsigned long addr = (unsigned long) v->addr + offs;
		unsigned int pfn;
		pgd_t *pgd;
		pud_t *pud;
		pmd_t *pmd;
		pte_t *pte;

		pfn = __phys_to_pfn(h->carveout->base + offs);
		pgd = pgd_offset_k(addr);
		pud = pud_alloc(&init_mm, pgd, addr);
		if (!pud)
			break;
		pmd = pmd_alloc(&init_mm, pud, addr);
		if (!pmd)
			break;
		pte = pte_alloc_kernel(pmd, addr);
		if (!pte)
			break;
		set_pte_at(&init_mm, addr, pte, pfn_pte(pfn, prot));
		flush_tlb_kernel_page(addr);
	}

	if (offs != adj_size) {
		free_vm_area(v);
		nvmap_usecount_dec(h);
		nvmap_handle_put(h);
		return NULL;
	}

	/* leave the handle ref count incremented by 1, so that
	 * the handle will not be freed while the kernel mapping exists.
	 * nvmap_handle_put will be called by unmapping this address */
	return p;
}

void nvmap_munmap(struct nvmap_handle_ref *ref, void *addr)
{
	struct nvmap_handle *h;

	if (!ref)
		return;

	h = ref->handle;

	if (h->heap_pgalloc) {
		vm_unmap_ram(addr, h->size >> PAGE_SHIFT);
	} else {
		struct vm_struct *vm;
		addr -= (h->carveout->base & ~PAGE_MASK);
		vm = remove_vm_area(addr);
		BUG_ON(!vm);
		kfree(vm);
		nvmap_usecount_dec(h);
	}
	nvmap_handle_put(h);
}

struct nvmap_handle_ref *nvmap_alloc(struct nvmap_client *client, size_t size,
				     size_t align, unsigned int flags,
				     unsigned int heap_mask)
{
	const unsigned int default_heap = (NVMAP_HEAP_SYSMEM |
					   NVMAP_HEAP_CARVEOUT_GENERIC);
	struct nvmap_handle_ref *r = NULL;
	int err;

	if (heap_mask == 0)
		heap_mask = default_heap;

	r = nvmap_create_handle(client, size);
	if (IS_ERR(r))
		return r;

	err = nvmap_alloc_handle_id(client, nvmap_ref_to_id(r),
				    heap_mask, align, flags);

	if (err) {
		nvmap_free_handle_id(client, nvmap_ref_to_id(r));
		return ERR_PTR(err);
	}

	return r;
}

/* allocates memory with specifed iovm_start address. */
struct nvmap_handle_ref *nvmap_alloc_iovm(struct nvmap_client *client,
	size_t size, size_t align, unsigned int flags, unsigned int iovm_start)
{
	int err;
	struct nvmap_handle *h;
	struct nvmap_handle_ref *r;
	const unsigned int default_heap = NVMAP_HEAP_IOVMM;

	/* size need to be more than one page.
	 * otherwise heap preference would change to system heap.
	 */
	if (size <= PAGE_SIZE)
		size = PAGE_SIZE << 1;
	r = nvmap_create_handle(client, size);
	if (IS_ERR_OR_NULL(r))
		return r;

	h = r->handle;
	h->pgalloc.iovm_addr = iovm_start;
	err = nvmap_alloc_handle_id(client, nvmap_ref_to_id(r),
			default_heap, align, flags);
	if (err)
		goto fail;

	err = mutex_lock_interruptible(&client->share->pin_lock);
	if (WARN_ON(err))
		goto fail;
	err = pin_locked(client, h);
	mutex_unlock(&client->share->pin_lock);
	if (err)
		goto fail;
	return r;

fail:
	nvmap_free_handle_id(client, nvmap_ref_to_id(r));
	return ERR_PTR(err);
}

void nvmap_free_iovm(struct nvmap_client *client, struct nvmap_handle_ref *r)
{
	unsigned long ref_id = nvmap_ref_to_id(r);

	nvmap_unpin_ids(client, 1, &ref_id);
	nvmap_free_handle_id(client, ref_id);
}

void nvmap_free(struct nvmap_client *client, struct nvmap_handle_ref *r)
{
	if (!r)
		return;

	nvmap_free_handle_id(client, nvmap_ref_to_id(r));
}

/*
 * create a mapping to the user's buffer and write it
 * (uses similar logic from nvmap_reloc_pin_array to map the cmdbuf)
 */
int nvmap_patch_word(struct nvmap_client *client,
				struct nvmap_handle *patch,
				u32 patch_offset, u32 patch_value)
{
	phys_addr_t phys;
	unsigned long kaddr;
	unsigned int pfn;
	void *addr;
	pte_t **pte;
	pgprot_t prot;

	if (patch_offset >= patch->size) {
		nvmap_warn(client, "read/write outside of handle\n");
		return -EFAULT;
	}

	pte = nvmap_alloc_pte(client->dev, &addr);
	if (IS_ERR(pte))
		return PTR_ERR(pte);

	/* derive physaddr of cmdbuf WAIT to patch */
	if (patch->heap_pgalloc) {
		unsigned int page = patch_offset >> PAGE_SHIFT;
		phys = page_to_phys(patch->pgalloc.pages[page]);
		phys += (patch_offset & ~PAGE_MASK);
	} else {
		phys = patch->carveout->base + patch_offset;
	}

	pfn = __phys_to_pfn(phys);
	prot = nvmap_pgprot(patch, pgprot_kernel);
	kaddr = (unsigned long)addr;

	/* write PTE, so addr points to cmdbuf PFN */
	set_pte_at(&init_mm, kaddr, *pte, pfn_pte(pfn, prot));
	flush_tlb_kernel_page(kaddr);

	/* write patch_value to addr + page offset */
	__raw_writel(patch_value, addr + (phys & ~PAGE_MASK));

	nvmap_free_pte(client->dev, pte);
	wmb();
	return 0;
}

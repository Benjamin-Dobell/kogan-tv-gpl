#include <linux/list.h>
#include <linux/hash.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include "mhal_g3d_mem.h"
#include "mhal_g3d_reg.h"
#include "mhal_g3dkmd.h"

/* ---------------------------------------------------------------------- */
#undef OPT_G3D_HAL_DEBUG
#undef G3D_HAL_DBG
#ifdef OPT_G3D_HAL_DEBUG
    #define G3D_HAL_DBG(fmt, args...)  printk(KERN_WARNING "[G3D_HAL][%05d]" fmt, __LINE__, ## args)
#else
    #define G3D_HAL_DBG(fmt, args...)
#endif
/* ---------------------------------------------------------------------- */

#define MEM_ENTRY_POOL_SIZE (PAGE_SIZE<<1)
#define G3D_MEM_CACHE_NAME  "g3d_mem"

struct mem_entry {
    void* mem_addr;
    unsigned int pool_id;
    struct hlist_node entry_list;
};

struct mem_entry_pool {
    struct mem_entry_pool* next;
    struct mem_entry entry[1];
};

static void mem_entry_pool_init(struct mem_entry_pool* pool, const unsigned long size);
static struct mem_entry* get_mem_entry(void);

static struct kmem_cache *g3d_mem_cache;
static struct mem_entry_pool* g3d_mem_entry_pool;
static HLIST_HEAD(g3d_mem_entry_pool_free_list);

static spinlock_t g3d_mem_lock = SPIN_LOCK_UNLOCKED;

/* same memory address cannot be in POOL0 and POOL1 simultaneously */
inline unsigned long hash_addr(void* ptr)
{
    return hash_ptr(ptr, BUCKET_SHIFT-1);
}

inline void mem_hash_init(g3d_mem_hash_t hash)
{
    unsigned long i = 0;
    for (i = 0; i < BUCKET_SIZE; ++i)
    {
        INIT_HLIST_HEAD(&hash[i]);
    }
}

inline void mem_hash_deinit(g3d_mem_hash_t hash)
{
    struct hlist_node *ptr = 0;
    struct hlist_node *next = 0;
    struct mem_entry* entry = 0;
    g3d_bucket_t* bucket = 0;
    unsigned long i = 0;

    spin_lock_irq(&g3d_mem_lock);
    for (i = 0; i < BUCKET_SIZE; ++i)
    {
        bucket = &hash[i];

        hlist_for_each_entry_safe(entry, ptr, next, bucket, entry_list)
        {
            if (unlikely(0 == entry->pool_id))
                g3d0free(entry->mem_addr);
            else
                g3d1free(entry->mem_addr);

            hlist_del_init(ptr);
            hlist_add_head(&entry->entry_list, &g3d_mem_entry_pool_free_list);
            G3D_HAL_DBG("clean mem_addr [%lu] = 0x%x, pool_id = %d\n", i, (unsigned int)entry->mem_addr, entry->pool_id);
        }
    }
    spin_unlock_irq(&g3d_mem_lock);
}

struct g3d_data* MHal_G3D_Data_Alloc(void)
{
    struct g3d_data* data = (struct g3d_data*)kmem_cache_alloc(g3d_mem_cache, SLAB_HWCACHE_ALIGN);
    if (data == 0)
        return 0;
    mem_hash_init(data->mem_hash);
    return data;
}

void MHal_G3D_Data_Free(struct g3d_data* data)
{
    mem_hash_deinit(data->mem_hash);
    kmem_cache_free(g3d_mem_cache, data);
}

static void mem_entry_pool_init(struct mem_entry_pool* pool, const unsigned long size)
{
    const unsigned long header_size = sizeof(struct mem_entry_pool*);
    unsigned long i = 0;
    unsigned long entry_count = 0;

    if (unlikely(size < header_size))
        return;

    pool->next = NULL;

    entry_count = (size - header_size)/sizeof(struct mem_entry);

    spin_lock_irq(&g3d_mem_lock);
    for (i = 0; i < entry_count; ++i)
    {
        INIT_HLIST_NODE(&pool->entry[i].entry_list);
        hlist_add_head(&pool->entry[i].entry_list, &g3d_mem_entry_pool_free_list);
    }
    spin_unlock_irq(&g3d_mem_lock);
}

int __init MHal_G3D_Mem_Init(void)
{
    g3d_mem_cache = kmem_cache_create(G3D_MEM_CACHE_NAME, sizeof(struct g3d_data), 0, SLAB_HWCACHE_ALIGN, NULL);
    if (g3d_mem_cache == 0)
    {
        MHal_G3D_Mem_Deinit();
        return -ENOMEM;
    }

    g3d_mem_entry_pool = kzalloc(MEM_ENTRY_POOL_SIZE, GFP_KERNEL);
    if (g3d_mem_entry_pool == 0)
    {
        MHal_G3D_Mem_Deinit();
        return -ENOMEM;
    }

    mem_entry_pool_init(g3d_mem_entry_pool, MEM_ENTRY_POOL_SIZE);

    return 0;
}

void MHal_G3D_Mem_Deinit(void)
{
    struct mem_entry_pool* pool = g3d_mem_entry_pool;

    if (g3d_mem_cache)
        kmem_cache_destroy(g3d_mem_cache);

    while (pool)
    {
        struct mem_entry_pool* tmp = pool;
        pool = tmp->next;
        kfree(tmp);
    }
}

inline int increate_mem_pool(void)
{
    struct mem_entry_pool* pool = kzalloc(MEM_ENTRY_POOL_SIZE, GFP_KERNEL);
    if (pool == 0)
        return -ENOMEM;

    mem_entry_pool_init(pool, MEM_ENTRY_POOL_SIZE);
    spin_lock_irq(&g3d_mem_lock);
    pool->next = g3d_mem_entry_pool;
    g3d_mem_entry_pool = pool;
    spin_unlock_irq(&g3d_mem_lock);

    return 0;
}

static struct mem_entry* get_mem_entry(void)
{
    struct mem_entry* entry = 0;

    spin_lock_irq(&g3d_mem_lock);
    if (hlist_empty(&g3d_mem_entry_pool_free_list))
    {
        spin_unlock_irq(&g3d_mem_lock);
        G3D_HAL_DBG("g3d_mem_entry_pool_free_list is empty!!\n");
        if (!increate_mem_pool())
            return NULL;
    }

    if (!hlist_empty(&g3d_mem_entry_pool_free_list))
    {
        entry = hlist_entry(g3d_mem_entry_pool_free_list.first, struct mem_entry, entry_list);
        hlist_del_init(&entry->entry_list);
        spin_unlock_irq(&g3d_mem_lock);
        return entry;
    }

    spin_unlock_irq(&g3d_mem_lock);
    return NULL;
}

/* same "mem_addr" should not be inserted twiced without removing first */
int MHal_G3D_Insert_MemAddr(g3d_mem_hash_t hash, void* mem_addr, unsigned pool_id)
{
    g3d_bucket_t* bucket = &hash[hash_addr(mem_addr)];

    struct mem_entry* entry = get_mem_entry();
    if (entry == 0)
        return -ENOMEM;

    entry->mem_addr = mem_addr;
    entry->pool_id  = pool_id;

    G3D_HAL_DBG("insert mem_addr [%lu] = 0x%x, pool_id = %d\n", hash_addr(mem_addr), (unsigned int)mem_addr, pool_id);
    spin_lock_irq(&g3d_mem_lock);
    hlist_add_head(&entry->entry_list, bucket);
    spin_unlock_irq(&g3d_mem_lock);
    return 0;
}

/* same "mem_addr" should not be removed twiced without inserting first */
void MHal_G3D_Remove_MemAddr(g3d_mem_hash_t hash, void* mem_addr, unsigned int pool_id)
{
    struct hlist_node *ptr = 0;
    struct mem_entry* entry = 0;
    g3d_bucket_t* bucket = &hash[hash_addr(mem_addr)];

    hlist_for_each_entry(entry, ptr, bucket, entry_list)
    {
        if (entry->mem_addr == mem_addr)
        {
            G3D_HAL_DBG("remove mem_addr [%lu] = 0x%x, pool_id = %d\n", hash_addr(mem_addr), (unsigned int)mem_addr, entry->pool_id);
            hlist_del_init(ptr);
            spin_lock_irq(&g3d_mem_lock);
            hlist_add_head(ptr, &g3d_mem_entry_pool_free_list);
            spin_unlock_irq(&g3d_mem_lock);
            return;
        }
    }
}


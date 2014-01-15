#ifndef _G3D_MEM_H_
#define _G3D_MEM_H_

#include <linux/list.h>

#define BUCKET_SHIFT 10
#define BUCKET_SIZE  (1<<BUCKET_SHIFT)

typedef struct hlist_head g3d_bucket_t;
typedef g3d_bucket_t g3d_mem_hash_t[BUCKET_SIZE];

struct g3d_data {
    g3d_mem_hash_t mem_hash;
};

int MHal_G3D_Mem_Init(void);
void MHal_G3D_Mem_Deinit(void);

struct g3d_data* MHal_G3D_Data_Alloc(void);
void MHal_G3D_Data_Free(struct g3d_data* data);

void MHal_G3D_Remove_MemAddr(g3d_mem_hash_t hash, void* ptr, unsigned int pool_id);
int MHal_G3D_Insert_MemAddr(g3d_mem_hash_t hash, void* ptr, unsigned int pool_id);

#endif  /* _G3D_MEM_H_ */

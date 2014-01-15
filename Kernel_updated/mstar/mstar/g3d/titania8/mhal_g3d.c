#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <asm/mips-boards/prom.h>

#include "mdrv_types.h"

#include "mhal_g3d.h"
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

#undef G3D_HAL_DBGX
#define G3D_HAL_DBGX(fmt, args...)
/* ---------------------------------------------------------------------- */
#define SIZE_2M 0x200000
#define CMD_QUEUE_SIZE SIZE_2M

#undef G3D_MEMORY_USAGE_INFO

#ifdef G3D_MEMORY_USAGE_INFO
#include <linux/spinlock.h>
static unsigned int pool_allocated[2] = { 0, 0 };
static spinlock_t pool_allocated_lock = SPIN_LOCK_UNLOCKED;
#endif

int MHal_G3D_Init(void)
{
    G3D_HAL_DBG("MHal_G3D_Init start\n");
    return g3dkInitialize(CMD_QUEUE_SIZE);
}

int MHal_G3D_Terminate(void)
{
    G3D_HAL_DBG("MHal_G3D_Terminate start\n");
    g3dkTerminate();
    return 1;
}

int MHal_G3D_CmdQ_Reset(void)
{
    G3D_HAL_DBG("MHal_G3D_CmdQ_Reset start\n");
    g3dkReset(G3DKMD_RESET_MODE_COMMAND_QUEUE);
    return 1;
}

int MHal_G3D_CmdQ_Send(unsigned int size, unsigned int *pCmdBuffer, unsigned int* stamp)
{
    G3D_HAL_DBG("MHal_G3D_CmdQ_Send start\n");
    return g3dkSubmitCommands(pCmdBuffer, size, stamp);
}

void MHal_G3D_Get_CmdQ(pG3D_MALLOC_INFO pInfo)
{
    G3D_HAL_DBG("MHal_G3D_Get_CmdQ start\n");
    g3dkGetCmdQBufferInfo(&pInfo->phsical_address, &pInfo->channel,&pInfo->size);
}

unsigned int MHal_G3D_IsIdle(void)
{
    G3D_HAL_DBG("MHal_G3D_IsIdle start\n");
    return g3dkIsIdle();
}

unsigned int MHal_G3D_GetSwStamp(void)
{
    G3D_HAL_DBG("MHal_G3D_GetSwStamp start\n");
    return g3dkGetSwStamp();
}

unsigned int MHal_G3D_GetHwStamp(void)
{
    G3D_HAL_DBG("MHal_G3D_GetHwStamp start\n");
    return g3dkGetHwStamp();
}

void MHal_G3D_MALLOC(pG3D_MALLOC_INFO pInfo, struct g3d_data* data)
{
    G3D_HAL_DBG("MHal_G3D_MALLOC start\n");

    if (pInfo->poolId == 0)
        pInfo->phsical_address = (U32)g3d0malloc(pInfo->size);
    else
        pInfo->phsical_address = (U32)g3d1malloc(pInfo->size);

    if ((pInfo->phsical_address & MIU1_PHYSADDRESS) == MIU1_PHYSADDRESS)
        pInfo->channel = 1;
    else
        pInfo->channel = 0;

    if (data && pInfo->phsical_address)
        MHal_G3D_Insert_MemAddr(data->mem_hash, (void*)pInfo->phsical_address, pInfo->poolId);

    pInfo->phsical_address &= MASK_MIU_PHYSADDRESS;

#ifdef G3D_MEMORY_USAGE_INFO
    printk("MHal_G3D_MALLOC[%d] get %x physical address at 0x%x(%d)\n",
            pInfo->poolId,pInfo->size,pInfo->phsical_address,pInfo->channel);

    if (pInfo->phsical_address == 0)
    {
        printk("MHal_G3D_MALLOC [%d] failed!\n", pInfo->poolId);
    }
    else
    {
        unsigned int allocated = 0;
        spin_lock(&pool_allocated_lock);
        allocated = (pool_allocated[pInfo->poolId] += pInfo->size);
        spin_unlock(&pool_allocated_lock);
        printk("G3D MALLOC[%d][%d]: (0x%x, 0x%x): TOTAL ALLOCATED[%d]: 0x%x\n",
                pInfo->poolId,
                pInfo->channel,
                pInfo->phsical_address,
                pInfo->size,
                pInfo->poolId,
                allocated);
    }
#endif
}

void MHal_G3D_MFREE(pG3D_MALLOC_INFO pInfo, struct g3d_data* data)
{
    unsigned int mem_addr = pInfo->phsical_address;

    G3D_HAL_DBG("MHal_G3D_MFREE start\n");

    if (mem_addr == 0)
        return;

    mem_addr |= (pInfo->channel == 0) ? MIU0_PHYSADDRESS : MIU1_PHYSADDRESS;

    if (data)
        MHal_G3D_Remove_MemAddr(data->mem_hash, (void*)mem_addr, pInfo->poolId);

    if(0 == pInfo->poolId)
        g3d0free((void*)mem_addr);
    else
        g3d1free((void*)mem_addr);

#ifdef G3D_MEMORY_USAGE_INFO
    {
        unsigned int allocated = 0;
        spin_lock(&pool_allocated_lock);
        allocated = (pool_allocated[pInfo->poolId] -= pInfo->size);
        spin_unlock(&pool_allocated_lock);
        printk("G3D MFREE[%d][%d]: (0x%x, 0x%x): TOTAL ALLOCATED[%d]: 0x%x\n",
                pInfo->poolId,
                pInfo->channel,
                pInfo->phsical_address,
                pInfo->size,
                pInfo->poolId,
                allocated);
    }
#endif

    pInfo->phsical_address = 0;
}

void MHal_G3D_GetMMAP(pG3D_MMAP_BUFFER pBuf)
{
    unsigned int addr, len;

    get_boot_mem_info(G3D_MEM0, &addr, &len);
    pBuf->paMemoryPool0 = addr /* & MASK_MIU_PHYSADDRESS */;
    pBuf->memoryPool0_size = len;

    get_boot_mem_info(G3D_MEM1, &addr, &len);
    pBuf->paMemoryPool1 = addr /* & MASK_MIU_PHYSADDRESS */;
    pBuf->memoryPool1_size = len;
}



#ifndef _MDRV_MPOOL_ST_H_
#define _MDRV_MPOOL_ST_H_


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// MPOOL_IOC_INFO 
typedef struct
{
    unsigned int                u32Addr;
    unsigned int                u32Size;
    unsigned int                u32Interval;
    unsigned char               u8MiuSel;
} DrvMPool_Info_t;

typedef struct
{
    unsigned int                u32AddrVirt;
    unsigned int                u32AddrPhys;
    unsigned int                u32Size;
} DrvMPool_Flush_Info_t;

typedef struct
    {
        unsigned int                LX_MEM_ADDR;
        unsigned int                LX_MEM_LENGTH;
        unsigned int                LX_MEM2_ADDR;
        unsigned int                LX_MEM2_LENGTH;
        unsigned int                EMAC_ADDR;
        unsigned int                EMAC_LENGTH;
        unsigned int                DRAM_ADDR;
        unsigned int                DRAM_LENGTH;
} De_Sys_Info_t;

typedef struct
{
    unsigned int                u32lxAddr;
    unsigned int                u32lxSize;
    unsigned int                u32lx2Addr;
    unsigned int                u32lx2Size;
} DrvMPool_Kernel_Info_t;



#endif // _MDRV_MPOOL_ST_H_


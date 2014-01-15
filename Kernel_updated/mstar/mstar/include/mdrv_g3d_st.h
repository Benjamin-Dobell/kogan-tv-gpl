
#ifndef _MDRV_G3D_ST_H_
#define _MDRV_G3D_ST_H_

//#include "mdrv_types.h"

typedef struct _G3D_CMDQ_BUFFER
{
    unsigned int size;
    unsigned int *pCmdBuffer;
    unsigned int ret_sw_stamp;
    unsigned int ret_status;    
} G3D_CMDQ_BUFFER, *pG3D_CMDQ_BUFFER;


typedef struct _G3D_MALLOC_INFO
{
    unsigned int poolId;
    unsigned int size;
    unsigned int phsical_address;
    unsigned int channel;
}G3D_MALLOC_INFO,*pG3D_MALLOC_INFO;

typedef struct _G3D_MMAP_BUFFER
{
    unsigned int paMemoryPool0;
    unsigned int memoryPool0_size;
    
    unsigned int paMemoryPool1;
    unsigned int memoryPool1_size;
    
}G3D_MMAP_BUFFER,*pG3D_MMAP_BUFFER;    

#endif


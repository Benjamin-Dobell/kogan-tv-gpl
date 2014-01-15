
#ifndef _MHAL_DEMOD_H_
#define _MHAL_DEMOD_H_

#include "mdrv_types.h"
#include "mdrv_g3d_st.h"
#include "mhal_g3d_mem.h"

#ifdef _MHAL_G3D_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

#define USING_PMEM

/* ---------------------------------------------------------------------- */
INTERFACE int  MHal_G3D_Init(void);
INTERFACE int  MHal_G3D_Terminate(void);
INTERFACE int  MHal_G3D_CmdQ_Reset(void);
INTERFACE int  MHal_G3D_CmdQ_Send(unsigned int size, unsigned int *pCmdBuffer, unsigned int *stamp);
INTERFACE void MHal_G3D_Get_CmdQ(pG3D_MALLOC_INFO pInfo);
INTERFACE unsigned int MHal_G3D_IsIdle(void);
INTERFACE unsigned int MHal_G3D_GetSwStamp(void);
INTERFACE unsigned int MHal_G3D_GetHwStamp(void);
INTERFACE void MHal_G3D_MALLOC(pG3D_MALLOC_INFO pInfo, struct g3d_data* data);
INTERFACE void MHal_G3D_MFREE(pG3D_MALLOC_INFO pInfo, struct g3d_data* data);
INTERFACE void MHal_G3D_GetMMAP(pG3D_MMAP_BUFFER pBuf);
/* ---------------------------------------------------------------------- */

#undef INTERFACE

#endif


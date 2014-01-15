/*******************************************************************************
*
* g3dkmd.h
*
*   -Master include file for kernel-mode G3D dirver implementation
*
* Copyright (c) 2009-2010 MStar Semiconductor, Inc.
*
*******************************************************************************/

#ifndef _G3DKMD_H_
#define _G3DKMD_H_


/*------------------------------------------------------------------------------
 *
 * Typedefs
 *
 *----------------------------------------------------------------------------*/
typedef enum G3DKMD_BOOL
{
    G3DKMD_FALSE    = 0,
    G3DKMD_TRUE     = 1,
} G3DKMD_BOOL;

typedef enum G3DKMD_RESET_MODE
{
    G3DKMD_RESET_MODE_ALL           = 0,
    G3DKMD_RESET_MODE_COMMAND_QUEUE = 1,
} G3DKMD_RESET_MODE;


/*------------------------------------------------------------------------------
 *
 * DDI functions
 *
 *----------------------------------------------------------------------------*/
#ifdef _WIN32
#define G3D_API __declspec(dllexport)
#else
#define G3D_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

G3D_API int             g3dkInitialize(unsigned int command_queue_size);
G3D_API void            g3dkTerminate(void);
G3D_API void            g3dkReset(G3DKMD_RESET_MODE mode);
G3D_API void            g3dkGetCmdQBufferInfo(unsigned int *phsical_address,
                                              unsigned int *channel, unsigned int *size);
G3D_API unsigned int    g3dkIsIdle(void);
G3D_API unsigned int    g3dkGetSwStamp(void);
G3D_API unsigned int    g3dkGetHwStamp(void);
G3D_API int             g3dkSubmitCommands(const void* commands, unsigned int length, unsigned int* stamp);

void* g3d0malloc(int bytes);
void  g3d0free(void*);
void* g3d1malloc(int bytes);
void  g3d1free(void*);

#ifdef __cplusplus
}
#endif


#endif  /* _G3DKMD_H_ */

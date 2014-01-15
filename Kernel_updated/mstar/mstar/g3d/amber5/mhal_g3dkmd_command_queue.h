/*******************************************************************************
*
* g3dkmd_command_queue.h
*
*   -G3Dk
*   -Command queue management
*
* Copyright (c) 2009-2010 MStar Semiconductor, Inc.
*
*******************************************************************************/
#ifndef _G3DKMD_COMMAND_QUEUE_H_
#define _G3DKMD_COMMAND_QUEUE_H_

#include "mhal_g3dkmd.h"

#define G3DKMD_COMMAND_QUEUE_GUARDBAND_SIZE     32  /* in bytes */

/*------------------------------------------------------------------------------
 *
 * Command queue info
 *
 *----------------------------------------------------------------------------*/
typedef struct _G3DKMD_COMMAND_QUEUE
{
    unsigned int            Size;
    unsigned int            GuardbandSize;
    unsigned int            Mask;
    unsigned char*          pBuffer;
    unsigned int            PhysicalAddress;
    unsigned char*          pWritePointer;
    const unsigned char*    pPrevWritePointer;
    unsigned int            AvailableSize;
    unsigned int            Channel;
} G3DKMD_COMMAND_QUEUE;

/*------------------------------------------------------------------------------
 *
 * Command queue helper functions
 *
 *----------------------------------------------------------------------------*/
G3DKMD_BOOL     _g3dkmd_CreateCommandQueue(unsigned int size);
void            _g3dkmd_DestroyCommandQueue(void);
void            _g3dkmd_ResetCommandQueue(void);
G3DKMD_BOOL     _g3dkmd_SubmitCommands(const void* commands, unsigned int length, unsigned int* stamp);
void            _g3dkmd_GetCmdQBufferInfo(unsigned int *phsical_address,
                                          unsigned int *channel, unsigned int *size);
#endif  /* _G3DKMD_COMMAND_QUEUE_H_ */

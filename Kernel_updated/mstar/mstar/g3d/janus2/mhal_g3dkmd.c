/*******************************************************************************
*
* g3dkmd.cpp
*
*   -G3Dk
*   -DDI function implementation
*
* Copyright (c) 2009-2010 MStar Semiconductor, Inc.
*
*******************************************************************************/

#include "mhal_g3d_reg.h"
#include "mhal_g3dkmd_command_queue.h"
#include "mhal_g3dkmd_misc.h"
#include "mhal_g3dkmd.h"

#include <linux/kernel.h>
#include <linux/semaphore.h>

/* global varibales */
static unsigned int _g3dkStamp = 0;

static DECLARE_MUTEX(_g3dkLock);

/*------------------------------------------------------------------------------
 *
 * API functions
 *
 *----------------------------------------------------------------------------*/
G3D_API int g3dkInitialize(unsigned int command_queue_size)
{
    /* setup command queue */
    if (0 != down_interruptible(&_g3dkLock))
        return G3DKMD_FALSE;

    if (_g3dkmd_CreateCommandQueue(command_queue_size) == G3DKMD_FALSE)
    {
        up(&_g3dkLock);
        return 0;
    }

    /* reset HW */
    _g3dkmd_Reset(G3DKMD_RESET_MODE_ALL);

    up(&_g3dkLock);
    return 1;
}/* g3dkInitialize */

G3D_API void g3dkTerminate(void)
{
    if (0 != down_interruptible(&_g3dkLock))
        return;
    _g3dkmd_DestroyCommandQueue();
    up(&_g3dkLock);
}/* g3dkTerminate */

G3D_API void g3dkReset(G3DKMD_RESET_MODE mode)
{
    if (0 != down_interruptible(&_g3dkLock))
        return;
    _g3dkmd_Reset(mode);
    up(&_g3dkLock);
}/* g3dkReset */

G3D_API int g3dkSubmitCommands(const void* commands, unsigned int length, unsigned int* stamp)
{
    G3DKMD_BOOL ret = G3DKMD_FALSE;

    if (0 != down_interruptible(&_g3dkLock))
        return 0;
    ret = _g3dkmd_SubmitCommands(commands, length, &_g3dkStamp);
    *stamp = _g3dkStamp;
    up(&_g3dkLock);

    return (G3DKMD_TRUE == ret);
}/* g3dkSubmitCommands */

G3D_API unsigned int g3dkIsIdle(void)
{
    G3DKMD_BOOL idle = G3DKMD_TRUE;

    /* check if idle */
    U32 read_pointer;
    U32 write_pointer;
    U32 cmd_queue_status;

    REG_LATCH_READ(REG_CQ_RD_PTR, read_pointer);
    write_pointer = REG_READ(REG_CQ_WT_PTR);
    cmd_queue_status = REG_READ(REG_CQ_STATUS);

    if (((cmd_queue_status & 0x7fffe) == 0x7fffe) && (read_pointer == write_pointer))
    {
        idle = 1;
    }
    else
    {
        /* printk(" read pointer = 0x%08x, write pointer = 0x%08x\n", read_pointer, write_pointer); */
        idle = 0;
    }

    return idle;
}/* g3dkIsIdle */

G3D_API unsigned int g3dkGetSwStamp(void)
{
    return _g3dkStamp;
}/* g3dkGetStamp */

G3D_API unsigned int g3dkGetHwStamp(void)
{
    unsigned int frame_stamp = 0;

    /* wait frame */
    unsigned int read_pointer;

    REG_LATCH_READ(REG_CQ_RD_PTR, read_pointer);
    REG_LATCH_READ(REG_GP_GBL1, frame_stamp);

    return frame_stamp;
}

G3D_API void g3dkGetCmdQBufferInfo(unsigned int *physical_address,
                                   unsigned int *channel, unsigned int *size)
{
    if (0 != down_interruptible(&_g3dkLock))
        return;
    _g3dkmd_GetCmdQBufferInfo(physical_address, channel, size);
    up(&_g3dkLock);
}

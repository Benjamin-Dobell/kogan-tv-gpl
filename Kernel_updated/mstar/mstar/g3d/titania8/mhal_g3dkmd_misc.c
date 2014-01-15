/*******************************************************************************
*
* g3dkmd_misc.cpp
*
*   -G3Dk
*   -Misc. function implementation
*
* Copyright (c) 2009-2010 MStar Semiconductor, Inc.
*
*******************************************************************************/

#include "mhal_g3dkmd_misc.h"
#include "mhal_g3dkmd_command_queue.h"

/*------------------------------------------------------------------------------
 *
 * Misc. helper functions
 *
 *----------------------------------------------------------------------------*/
void _g3dkmd_Reset(G3DKMD_RESET_MODE mode)
{
    if (mode == G3DKMD_RESET_MODE_COMMAND_QUEUE)
    {
        _g3dkmd_ResetCommandQueue();
    }
    else if (mode == G3DKMD_RESET_MODE_ALL)
    {
        _g3dkmd_ResetCommandQueue();
#if 0
        WaitIdle();

        /* set multi-buffering */
        if (g_G3dGlobals.Config.MultiBufferingEnabled == true)
        {
            REG_MASK_WRITE(REG_MULTI_BUF, MSK_MULTI_BUF_EN, MSK_MULTI_BUF_EN);

            /* double buffers */
            REG_MASK_WRITE(REG_MULTI_BUF, (1 << SHF_MULTI_BUF_NUM), MSK_MULTI_BUF_NUM); 

            REG_MASK_WRITE(REG_MULTI_BUF, 0, MSK_MULTI_BUF_MD);
        }
        else
        {
            REG_MASK_WRITE(REG_MULTI_BUF, 0, MSK_MULTI_BUF_EN);
            REG_MASK_WRITE(REG_MULTI_BUF, 0, MSK_MULTI_BUF_NUM);
            REG_MASK_WRITE(REG_MULTI_BUF, 0, MSK_MULTI_BUF_MD);
        }

        /* initialize fixed HW register settings */
        WaitCommandQueue(sizeof(U32) * 4);
        WriteCommand(SINGLE_COMMAND | REG_PAGE_END);
        WriteCommand(0);
        WriteCommand(SINGLE_COMMAND | REG_VP_TRANSFORM);
        WriteCommand(MSK_CL_VP_TRANSFORM);
#endif
    }
}/* _g3dkmd_Reset */

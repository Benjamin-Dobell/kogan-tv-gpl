////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   mdrv_gflip_interrupt.c
/// @brief  MStar gflip Interrupt Interface
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em>Define the interface which corespond to sysem's interrupt notification</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MDRV_GFLIP_INTERRUPT_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/interrupt.h>

#include "mdrv_mstypes.h"
#include "mdrv_gflip.h"
#include "chip_int.h"
#include "mdrv_gflip_interrupt.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Local Defines
//=============================================================================

//=============================================================================
// Debug Macros
//=============================================================================
#define GFLIPINT_DEBUG
#ifdef GFLIPINT_DEBUG
    #define GFLIPINT_PRINT(fmt, args...)      printk("[GFlip (Interrupter)][%05d] " fmt, __LINE__, ## args)
    #define GFLIPINT_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        GFLIPINT_PRINT(_fmt, ##_args);  \
                                    }
#else
    #define GFLIPINT_PRINT(_fmt, _args...)
    #define GFLIPINT_ASSERT(_cnd, _fmt, _args...)
#endif

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Local Variables
//=============================================================================

//=============================================================================
// Global Variables
//=============================================================================

//=============================================================================
// Local Function Prototypes
//=============================================================================

//=============================================================================
// Local Function
//=============================================================================

//=============================================================================
// GFLIP DRV Interrupt Driver Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// Handle GFLIP Interrupt notification handler
/// @param  irq                  \b IN: interrupt number
/// @param  devid                  \b IN: device id
/// @return IRQ_HANDLED
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_GFLIPINT_IntHandler(int irq,void *devid)
{
    //process gflip interrupt:
    MDrv_GFLIP_ProcessIRQ();

    //clear interrupt
    MDrv_GFLIP_ClearIRQ();

    return IRQ_HANDLED;
}

#if	( defined (CONFIG_MSATR_VE_CAPTURE_SUPPORT))
//-------------------------------------------------------------------------------------------------
/// Handle GFLIP VEC Interrupt notification handler
/// @param  irq                  \b IN: interrupt number
/// @param  devid                  \b IN: device id
/// @return IRQ_HANDLED
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_GFLIP_VECINT_IntHandler(int irq,void *devid)
{
    //process gflip_vec interrupt:
    MDrv_GFLIP_VEC_ProcessIRQ();
    //clear interrupt
    MDrv_GFLIP_VEC_ClearIRQ();
    return IRQ_HANDLED;
}
#endif //CONFIG_MSATR_VE_CAPTURE_SUPPORT

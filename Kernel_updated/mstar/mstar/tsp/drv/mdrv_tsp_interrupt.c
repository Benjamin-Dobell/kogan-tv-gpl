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
/// @file   mdrv_tsp_interrupt.c
/// @brief  MStar tsp Interrupt Interface
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em>Define the interface which corespond to sysem's interrupt notification</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MDRV_TSP_INTERRUPT_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/interrupt.h>

#include "mdrv_mstypes.h"
#include "mhal_tsp.h"
#include "chip_int.h"
#include "mdrv_tsp_interrupt.h"
#include "mdrv_tsp_st.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Local Defines
//=============================================================================

//=============================================================================
// Debug Macros
//=============================================================================
#define TSPINT_DEBUG
#ifdef TSPINT_DEBUG
    #define TSPINT_PRINT(fmt, args...)      printk("[TSP (Interrupter)][%05d] " fmt, __LINE__, ## args)
    #define TSPINT_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        TSPINT_PRINT(_fmt, ##_args);  \
                                    }
#else
    #define TSPINT_PRINT(_fmt, _args...)
    #define TSPINT_ASSERT(_cnd, _fmt, _args...)
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
MS_TSP_EVENT_INFO   TspEventInfo;
extern spinlock_t   tsp_spinlock;

//=============================================================================
// Local Function Prototypes
//=============================================================================

//=============================================================================
// Local Function
//=============================================================================

//=============================================================================
// TSP DRV Interrupt Driver Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// Handle TSP Interrupt notification handler
/// @param  irq                  \b IN: interrupt number
/// @param  devid                  \b IN: device id
/// @return IRQ_HANDLED
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_TSPINT_IntHandler(int irq, void *devid)
{
    //U16 u16HwInt;
    //U16 u16HwInt2;

    U32 u32SwInt;

    //TSP_ISR_SAVE_ALL;

    //u16HwInt = REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1);
    //u16HwInt2 = REG32_R(&_TspCtrl[0].HwInt2_SwInt2_L);
    u32SwInt = REG32_R(&_TspCtrl[0].SwInt_Stat);

    if (u32SwInt & TSP_SWINT_CTRL_FIRE)
    {
        MS_U32          u32EngId;
        //MS_U32          u32PidFltId;
        MS_U32          u32SecFltId;
        MS_U32          u32IntStat;
        //REG_SecFlt*     pSecFilter;

        u32EngId    = (u32SwInt & TSP_SWINT_INFO_ENG_MASK) >> TSP_SWINT_INFO_ENG_SHFT;
        u32IntStat  = (u32SwInt & TSP_SWINT_STATUS_CMD_MASK) >> TSP_SWINT_STATUS_CMD_SHFT;
        u32SecFltId = (u32SwInt & TSP_SWINT_INFO_SEC_MASK) >> TSP_SWINT_INFO_SEC_SHFT;
        //pSecFilter  = &(_TspSec[u32EngId].Flt[u32SecFltId]);

        spin_lock(tsp_spinlock);
        switch (u32IntStat)
        {
        case TSP_SWINT_STATUS_SEC_RDY:
        case TSP_SWINT_STATUS_REQ_RDY:
            TspEventInfo.SecEvent[u32SecFltId]++;
            TspEventInfo.Count++;
            break;

        case TSP_SWINT_STATUS_BUF_OVFLOW:
            TspEventInfo.OverflowEvent[u32SecFltId] = 1;
            break;

        case TSP_SWINT_STATUS_SEC_CRCERR:
            break;

        case TSP_SWINT_STATUS_PKT_OVRUN:
            break;

        case TSP_SWINT_STATUS_SEC_ERROR:
            break;

        case TSP_SWINT_STATUS_DEBUG:
            break;

        default:
            break;
        }
        spin_unlock(tsp_spinlock);
    }

#if 0
    if (HAS_FLAG(u16HwInt, TSP_HWINT_TSP_PVR_TAIL0_STATUS))
    {
        _SetEvent(TSP_TASK_EVENT_PVR0_RDY);
    }
    if (HAS_FLAG(u16HwInt, TSP_HWINT_TSP_PVR_MID0_STATUS))
    {
        _SetEvent(TSP_TASK_EVENT_PVR0_MID);
    }
    if (HAS_FLAG(u16HwInt, TSP_HWINT_TSP_SW_INT_STATUS))
    {
        _SetEvent(TSP_TASK_EVENT_SECTION);
    }

#if defined(CONFIG_Titania8)
    if (HAS_FLAG(u16HwInt2, TSP_HWINT2_CB_TAIL_STATUS))
    {
        _SetEvent(TSP_TASK_EVENT_PVR0_RDY);
    }
    if (HAS_FLAG(u16HwInt2, TSP_HWINT2_CB_MID_STATUS))
    {
        _SetEvent(TSP_TASK_EVENT_PVR0_RDY);
    }
    if (HAS_FLAG(u16HwInt2, TSP_HWINT2_VQ_OV_STATUS))
    {
        _SetEvent(TSP_TASK_EVENT_VQ_OV);
    }
#endif
#endif

    MHal_TSP_Int_ClearSw();
    MHal_TSP_Int_ClearHw(TSP_HWINT_ALL);
    //MHal_TSP_Int2_ClearSw();
    //MHal_TSP_Int2_ClearHw(TSP_HWINT2_ALL);

    //TSP_ISR_RESTORE_ALL;

    // Linux seems not to need to enable int in ISR.

    return IRQ_HANDLED;
}

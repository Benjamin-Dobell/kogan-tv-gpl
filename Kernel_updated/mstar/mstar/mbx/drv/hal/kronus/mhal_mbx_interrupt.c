////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mhal_mbx_interrupt.c
/// @brief  MStar MailBox interrupt DDI
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MHAL_MBX_INTERRUPT_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/interrupt.h>
#include <linux/delay.h>

#include "mdrv_mstypes.h"
#include "mdrv_mbx.h"
#include "mhal_mbx_interrupt.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Local Defines
//=============================================================================

//=============================================================================
// Debug Macros
//=============================================================================
#define MBXINT_DEBUG
#ifdef MBXINT_DEBUG
    #define MBXINT_PRINT(fmt, args...)      printk("[MailBox (Driver)][%05d] " fmt, __LINE__, ## args)
    #define MBXINT_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        MBXINT_PRINT(_fmt, ##_args);  \
                                    }
#else
    #define MBXINT_PRINT(_fmt, _args...)
    #define MBXINT_ASSERT(_cnd, _fmt, _args...)
#endif

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Local Variables
//=============================================================================
static MBX_MSGRECV_CB_FUNC _pMBXMsgRecvCbFunc = NULL;

//=============================================================================
// Global Variables
//=============================================================================

//=============================================================================
// Local Function Prototypes
//=============================================================================
static void _MHAL_MBXINT_AEON2PMHandler(unsigned long unused);
static void _MHAL_MBXINT_AEON2MIPSHandler(unsigned long unused);
static void _MHAL_MBXINT_MIPS2PMHandler(unsigned long unused);
static void _MHAL_MBXINT_MIPS2AEONHandler(unsigned long unused);
static void _MHAL_MBXINT_PM2MIPSHandler(unsigned long unused);
static void _MHAL_MBXINT_PM2AEONHandler(unsigned long unused);

//callback from driver:
static irqreturn_t _MHAL_MBXINT_INTHandler(int irq, void *dev_id);

#if 0
static MS_U16 _MapFiq2FiqMask(MS_S32 s32Fiq);
#endif

static MBX_Result _MHAL_MBXINT_SetHostCPU(MBX_CPU_ID eHostCPU);
//=============================================================================
// Local Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// Notify Interrupt Aeon2PM
/// @param  unused                  \b IN: unused
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MHAL_MBXINT_AEON2PMHandler(unsigned long unused)
{
    if(NULL == _pMBXMsgRecvCbFunc)
    {
        return;
    }

    _pMBXMsgRecvCbFunc(MBX_INT_AEON2PM);
}

DECLARE_TASKLET(_mbxAeon2PMTaskletMBXINT, _MHAL_MBXINT_AEON2PMHandler, 0);

//-------------------------------------------------------------------------------------------------
/// Notify Interrupt Aeon2Mips
/// @param  unused                  \b IN: unused
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MHAL_MBXINT_AEON2MIPSHandler(unsigned long unused)
{
    if(NULL == _pMBXMsgRecvCbFunc)
    {
        return;
    }

    _pMBXMsgRecvCbFunc(MBX_INT_AEON2MIPS);
}

DECLARE_TASKLET(_mbxAeon2MipsTaskletMBXINT, _MHAL_MBXINT_AEON2MIPSHandler, 0);

//-------------------------------------------------------------------------------------------------
/// Notify Interrupt Mips2PM
/// @param  unused                  \b IN: unused
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MHAL_MBXINT_MIPS2PMHandler(unsigned long unused)
{
    if(NULL == _pMBXMsgRecvCbFunc)
    {
        return;
    }

    _pMBXMsgRecvCbFunc(MBX_INT_MIPS2PM);
}

DECLARE_TASKLET(_mbxMips2PMTaskletMBXINT, _MHAL_MBXINT_MIPS2PMHandler, 0);

//-------------------------------------------------------------------------------------------------
/// Notify Interrupt Mips2Aeon
/// @param  unused                  \b IN: unused
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MHAL_MBXINT_MIPS2AEONHandler(unsigned long unused)
{
    if(NULL == _pMBXMsgRecvCbFunc)
    {
        return;
    }

    _pMBXMsgRecvCbFunc(MBX_INT_MIPS2AEON);
}

DECLARE_TASKLET(_mbxMips2AeonTaskletMBXINT, _MHAL_MBXINT_MIPS2AEONHandler, 0);

//-------------------------------------------------------------------------------------------------
/// Notify Interrupt PM2Mips
/// @param  unused                  \b IN: unused
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MHAL_MBXINT_PM2MIPSHandler(unsigned long unused)
{
    if(NULL == _pMBXMsgRecvCbFunc)
    {
        return;
    }

    _pMBXMsgRecvCbFunc(MBX_INT_PM2MIPS);
}

DECLARE_TASKLET(_mbxPM2MipsTaskletMBXINT, _MHAL_MBXINT_PM2MIPSHandler, 0);

//-------------------------------------------------------------------------------------------------
/// Notify Interrupt PM2Aeon
/// @param  unused                  \b IN: unused
/// @return void
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void _MHAL_MBXINT_PM2AEONHandler(unsigned long unused)
{
    if(NULL == _pMBXMsgRecvCbFunc)
    {
        return;
    }

    _pMBXMsgRecvCbFunc(MBX_INT_PM2AEON);
}

DECLARE_TASKLET(_mbxPM2AeonTaskletMBXINT, _MHAL_MBXINT_PM2AEONHandler, 0);

//-------------------------------------------------------------------------------------------------
/// Handle Interrupt, schedule tasklet
/// @param  irq                  \b IN: interrupt number
/// @param  dev_id                  \b IN: dev id
/// @return irqreturn_t: IRQ_HANDLED
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
irqreturn_t _MHAL_MBXINT_INTHandler(int irq, void *dev_id)
{
    switch(irq)
    {
        case E_FIQ_INT_AEON_TO_8051:
            tasklet_schedule(&_mbxAeon2PMTaskletMBXINT);
            MHAL_MBXINT_Clear(MBX_INT_AEON2PM);
            break;
        case E_FIQ_INT_MIPS_TO_8051:
            tasklet_schedule(&_mbxMips2PMTaskletMBXINT);
            MHAL_MBXINT_Clear(MBX_INT_MIPS2PM);
            break;
        case E_FIQ_INT_8051_TO_AEON:
            tasklet_schedule(&_mbxPM2AeonTaskletMBXINT);
            MHAL_MBXINT_Clear(MBX_INT_PM2AEON);
            break;
        case E_FIQ_INT_MIPS_TO_AEON:
            tasklet_schedule(&_mbxMips2AeonTaskletMBXINT);
            MHAL_MBXINT_Clear(MBX_INT_MIPS2AEON);
            break;
        case E_FIQ_INT_8051_TO_MIPS:
            tasklet_schedule(&_mbxPM2MipsTaskletMBXINT);
            MHAL_MBXINT_Clear(MBX_INT_PM2MIPS);
            break;
        case E_FIQ_INT_AEON_TO_MIPS:
            tasklet_schedule(&_mbxAeon2MipsTaskletMBXINT);
            MHAL_MBXINT_Clear(MBX_INT_AEON2MIPS);
            break;
    }

    return IRQ_HANDLED;
}

#if 0
//-------------------------------------------------------------------------------------------------
/// Fiq number 2 Fiq Mask transfer
/// @param  s32Fiq                  \b IN: fiq number
/// @return INT_FIQMASK_xx: mapped fiq mask
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MS_U16 _MapFiq2FiqMask(MS_S32 s32Fiq)
{
    switch(s32Fiq)
    {
        case MBX_INT_AEON2PM:
            return INT_AEON_PM;
        case MBX_INT_MIPS2PM:
            return INT_H3_PM;
        case MBX_INT_PM2AEON:
            return INT_PM_AEON;
        case MBX_INT_MIPS2AEON:
            return INT_H3_AEON;
        case MBX_INT_PM2MIPS:
            return INT_PM_H3;
        case MBX_INT_AEON2MIPS:
            return INT_AEON_H3;
        default:
            return 0;
    }
}
#endif

//-------------------------------------------------------------------------------------------------
/// Set Interrupt to Host CPU ID: Enable related interrupt and attached related callback.
/// @param  eHostCPUID                  \b IN: The Host CPU ID
/// @return E_MBX_SUCCESS
/// @attention
/// <b>[MXLIB] <em></em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result _MHAL_MBXINT_SetHostCPU(MBX_CPU_ID eHostCPU)
{
    MS_U32     s32Err;

    switch(eHostCPU)
    {
        case E_MBX_CPU_PM:
            s32Err = request_irq(E_FIQ_INT_AEON_TO_8051, _MHAL_MBXINT_INTHandler, SA_INTERRUPT, "MBX_FIQ_AEON2PM", NULL);
            if(s32Err<0)
            {
                MBXINT_PRINT("request FIQ: %x Failed!\n", MBX_INT_AEON2PM);
                return E_MBX_UNKNOW_ERROR;
            }

            s32Err = request_irq(E_FIQ_INT_MIPS_TO_8051, _MHAL_MBXINT_INTHandler, SA_INTERRUPT, "MBX_FIQ_MIPS2PM", NULL);
            if(s32Err<0)
            {
                free_irq(E_FIQ_INT_AEON_TO_8051, NULL);
                MBXINT_PRINT("request FIQ: %x Failed!\n", MBX_INT_MIPS2PM);
                return E_MBX_UNKNOW_ERROR;
            }

            MHAL_MBXINT_Enable(MBX_INT_AEON2PM, TRUE);
            MHAL_MBXINT_Enable(MBX_INT_MIPS2PM, TRUE);
            break;
        case E_MBX_CPU_AEON:
            s32Err = request_irq(E_FIQ_INT_8051_TO_AEON, _MHAL_MBXINT_INTHandler, SA_INTERRUPT, "MBX_FIQ_PM2AEON", NULL);
            if(s32Err<0)
            {
                MBXINT_PRINT("request FIQ: %x Failed!\n", MBX_INT_PM2AEON);
                return E_MBX_UNKNOW_ERROR;
            }

            s32Err = request_irq(E_FIQ_INT_MIPS_TO_AEON, _MHAL_MBXINT_INTHandler, SA_INTERRUPT, "MBX_FIQ_MIPS2AEON", NULL);
            if(s32Err<0)
            {
                free_irq(E_FIQ_INT_8051_TO_AEON, NULL);
                MBXINT_PRINT("request FIQ: %x Failed!\n", MBX_INT_MIPS2AEON);
                return E_MBX_UNKNOW_ERROR;
            }
            MHAL_MBXINT_Enable(MBX_INT_PM2AEON, TRUE);
            MHAL_MBXINT_Enable(MBX_INT_MIPS2AEON, TRUE);
            break;
        case E_MBX_CPU_MIPS:
            s32Err = request_irq(E_FIQ_INT_8051_TO_MIPS, _MHAL_MBXINT_INTHandler, SA_INTERRUPT, "MBX_FIQ_PM2MIPS", NULL);
            if(s32Err<0)
            {
                MBXINT_PRINT("request FIQ: %x Failed!\n", MBX_INT_PM2MIPS);
                return E_MBX_UNKNOW_ERROR;
            }

            s32Err = request_irq(E_FIQ_INT_AEON_TO_MIPS, _MHAL_MBXINT_INTHandler, SA_INTERRUPT, "MBX_FIQ_AEON2MIPS", NULL);
            if(s32Err<0)
            {
                free_irq(E_FIQ_INT_8051_TO_MIPS, NULL);
                MBXINT_PRINT("request FIQ: %x Failed!\n", MBX_INT_AEON2MIPS);
                return E_MBX_UNKNOW_ERROR;
            }
            MHAL_MBXINT_Enable(MBX_INT_PM2MIPS, TRUE);
            MHAL_MBXINT_Enable(MBX_INT_AEON2MIPS, TRUE);
            break;
        default:
            return E_MBX_ERR_INVALID_CPU_ID;
    }

    return E_MBX_SUCCESS;
}

//=============================================================================
// Mailbox HAL Interrupt Driver Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// Handle Interrupt INIT
/// @param  eHostCPU                  \b IN: interrupt owner
/// @param  pMBXRecvMsgCBFunc                  \b IN: callback func by driver
/// @return E_MBX_ERR_INVALID_CPU_ID: the cpu id is wrong
/// @return E_MBX_UNKNOW_ERROR: request_irq failed;
/// @return E_MBX_SUCCESS: success;
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result MHAL_MBXINT_Init (MBX_CPU_ID eHostCPU, MBX_MSGRECV_CB_FUNC pMBXRecvMsgCBFunc)
{
    _pMBXMsgRecvCbFunc = pMBXRecvMsgCBFunc;

    return _MHAL_MBXINT_SetHostCPU(eHostCPU);
}

//-------------------------------------------------------------------------------------------------
/// Handle Interrupt DeINIT
/// @param  eHostCPU                  \b IN: interrupt owner
/// @return void;
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
void MHAL_MBXINT_DeInit (MBX_CPU_ID eHostCPU)
{
    switch(eHostCPU)
    {
        case E_MBX_CPU_PM:
            MHAL_MBXINT_Enable(MBX_INT_AEON2PM, FALSE);
            MHAL_MBXINT_Enable(MBX_INT_MIPS2PM, FALSE);
            free_irq(E_FIQ_INT_AEON_TO_8051, NULL);
            free_irq(E_FIQ_INT_MIPS_TO_8051, NULL);
            break;
        case E_MBX_CPU_AEON:
            MHAL_MBXINT_Enable(MBX_INT_PM2AEON, FALSE);
            MHAL_MBXINT_Enable(MBX_INT_MIPS2AEON, FALSE);
            free_irq(E_FIQ_INT_8051_TO_AEON, NULL);
            free_irq(E_FIQ_INT_MIPS_TO_AEON, NULL);
            break;
        case E_MBX_CPU_MIPS:
            MHAL_MBXINT_Enable(MBX_INT_PM2MIPS, FALSE);
            MHAL_MBXINT_Enable(MBX_INT_AEON2MIPS, FALSE);
            free_irq(E_FIQ_INT_8051_TO_MIPS, NULL);
            free_irq(E_FIQ_INT_AEON_TO_MIPS, NULL);
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------------
/// Reset Host CPU for MBX Interrupt
/// @param  ePrevCPU                  \b IN: previous host cpu id
/// @param  eConfigCpu                  \b IN: new configed cpu id
/// @return E_MBX_SUCCESS: success;
/// @return E_MBX_INVALID_CPU_ID
/// @attention
/// <b>[MXLIB] <em></em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result MHAL_MBXINT_ResetHostCPU (MBX_CPU_ID ePrevCPU, MBX_CPU_ID eConfigCpu)
{
    MHAL_MBXINT_DeInit(ePrevCPU);

    return _MHAL_MBXINT_SetHostCPU(eConfigCpu);
}

//-------------------------------------------------------------------------------------------------
/// enable/disable Interrupt
/// @param  s32Fiq                  \b IN: interrupt for enable/disable
/// @param  bEnable                  \b IN: Enable or Disable
/// @return E_MBX_SUCCESS: success;
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result MHAL_MBXINT_Enable (MS_S32 s32Fiq, MS_BOOL bEnable)
{
    MS_U16 u16RegValHost;
    MS_U16 u16FIQMask;
    MS_U16 u16IdxHost;

    switch(s32Fiq)
    {
        case MBX_INT_AEON2PM:
            u16IdxHost = REG_INT_FIQMASK_PM;
            u16FIQMask = _BIT_AEON_PM;
            break;
        case MBX_INT_MIPS2PM:
            u16IdxHost = REG_INT_FIQMASK_PM;
            u16FIQMask = _BIT_H3_PM;
            break;
        case MBX_INT_PM2AEON:
            u16IdxHost = REG_INT_FIQMASK_AEON;
            u16FIQMask = _BIT_PM_AEON;
            break;
        case MBX_INT_MIPS2AEON:
            u16IdxHost = REG_INT_FIQMASK_AEON;
            u16FIQMask = _BIT_H3_AEON;
            break;
        case MBX_INT_PM2MIPS:
            u16IdxHost = REG_INT_FIQMASK_H3;
            u16FIQMask = _BIT_PM_H3;
            break;
        case MBX_INT_AEON2MIPS:
            u16IdxHost = REG_INT_FIQMASK_H3;
            u16FIQMask = _BIT_AEON_H3;
            break;
        default:
            return 0;
    }

    mb();
    u16RegValHost = INT_REG(u16IdxHost);
    if(bEnable)
    {
        INT_REG(u16IdxHost) = u16RegValHost & ~(u16FIQMask);
    }
    else
    {
        INT_REG(u16IdxHost) = u16RegValHost | (u16FIQMask);
    }
    mb();

    return E_MBX_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// Fire Interrupt
/// @param  dstCPUID                  \b IN: dst cpu of interrupt
/// @param  srcCPUID                  \b IN: src cpu of interrupt
/// @return E_MBX_SUCCESS: success;
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result MHAL_MBXINT_Fire (MBX_CPU_ID dstCPUID, MBX_CPU_ID srcCPUID)
{
    MBXINT_ASSERT((dstCPUID!=srcCPUID),"dst cpu is the same as src cpu!\n");

    switch(srcCPUID)
    {
        case E_MBX_CPU_PM:
            if(dstCPUID==E_MBX_CPU_AEON)
            {
                CPU_INT_REG(REG_INT_PMFIRE) = INT_PM_AEON;
            }
            else
            {
                CPU_INT_REG(REG_INT_PMFIRE) = INT_PM_H2;
            }
            mb();
            CPU_INT_REG(REG_INT_PMFIRE) = 0;
            mb();
            break;
        case E_MBX_CPU_AEON:
            if(dstCPUID==E_MBX_CPU_PM)
            {
                CPU_INT_REG(REG_INT_AEONFIRE) = INT_AEON_PM;
            }
            else
            {
                CPU_INT_REG(REG_INT_AEONFIRE) = INT_PM_H2;
            }
            mb();
            CPU_INT_REG(REG_INT_AEONFIRE) = 0;
            mb();
            break;
        case E_MBX_CPU_MIPS:
            if(dstCPUID==E_MBX_CPU_PM)
            {
                CPU_INT_REG(REG_INT_H3FIRE) = INT_H3_PM;
            }
            else
            {
                CPU_INT_REG(REG_INT_H3FIRE) = INT_H3_AEON;
            }
            mb();
            CPU_INT_REG(REG_INT_H3FIRE) = 0;
            mb();
            break;
        default:
            MBXINT_ASSERT(FALSE,"wrong src cpu!\n");
            break;
    }

    return E_MBX_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/// clear Interrupt
/// @param  s32Fiq                  \b IN: interrupt number
/// @return E_MBX_SUCCESS: success;
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
MBX_Result MHAL_MBXINT_Clear (MS_S32 s32Fiq)
{
    MS_U16 u16RegValHost;
    MS_U16 u16FIQMask;
    MS_U16 u16IdxHost;

    switch(s32Fiq)
    {
        case MBX_INT_AEON2PM:
            u16IdxHost = REG_INT_FIQMASK_PM;
            u16FIQMask = _BIT_AEON_PM;
            break;
        case MBX_INT_MIPS2PM:
            u16IdxHost = REG_INT_FIQMASK_PM;
            u16FIQMask = _BIT_H3_PM;
            break;
        case MBX_INT_PM2AEON:
            u16IdxHost = REG_INT_FIQMASK_AEON;
            u16FIQMask = _BIT_PM_AEON;
            break;
        case MBX_INT_MIPS2AEON:
            u16IdxHost = REG_INT_FIQMASK_AEON;
            u16FIQMask = _BIT_H3_AEON;
            break;
        case MBX_INT_PM2MIPS:
            u16IdxHost = REG_INT_FIQMASK_H3;
            u16FIQMask = _BIT_PM_H3;
            break;
        case MBX_INT_AEON2MIPS:
            u16IdxHost = REG_INT_FIQMASK_H3;
            u16FIQMask = _BIT_AEON_H3;
            break;
        default:
            return 0;
    }

    mb();

    u16RegValHost = INT_REG(u16IdxHost);
    INT_REG(u16IdxHost) = u16RegValHost | (u16FIQMask);

    mb();


    return E_MBX_SUCCESS;
}

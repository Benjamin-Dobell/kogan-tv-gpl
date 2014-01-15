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
///
/// file    mhal_gflip.c
/// @brief  MStar GFLIP DDI HAL LEVEL
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_GFLIP_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/kernel.h>	/* printk() */

#include "mdrv_mstypes.h"
#include "mhal_gflip_reg.h"
#include "mdrv_gflip.h"
#include "mhal_gflip.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Debug Macros
//=============================================================================
#define GFLIPHAL_DEBUG
#ifdef GFLIPHAL_DEBUG
    #define GFLIPHAL_PRINT(fmt, args...)      printk("[GFLIP (HAL Driver)][%05d] " fmt, __LINE__, ## args)
    #define GFLIPHAL_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        GFLIPHAL_PRINT(_fmt, ##_args);  \
                                        while(1);               \
                                    }
#else
    #define GFLIPHAL_PRINT(_fmt, _args...)
    #define GFLIPHAL_ASSERT(_cnd, _fmt, _args...)
#endif

//=============================================================================
// Macros
//=============================================================================
#define LO16BIT(x)  (x & 0x0000ffff)
#define HI16BIT(x)  (x >> 16)

//=============================================================================
// Local Variables
//=============================================================================
static volatile MS_U16 _u16GflipGOPDst[MAX_GOP_SUPPORT] = { GFLIP_GOP_DST_OP0 };

//=============================================================================
// Global Variables
//=============================================================================

//=============================================================================
// Local Function Prototypes
//=============================================================================
void _MHal_GFLIP_SetGopBank(MS_U8 u8bank);
MS_U8 _MHal_GFLIP_GetGopBank(void);
void _MHal_GFLIP_WriteGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16 u16Val, MS_U16 u16Mask);
void _MHal_GFLIP_ReadGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16* u16Val);
MS_U32 _MHal_GFLIP_GetValidGWinIDPerGOPIdx(MS_U32 u32GopIdx, MS_U32 u32GwinIdx);
MS_S32 _MHal_GFLIP_DirectSerialDiff(MS_U16 u16TagID1,  MS_U16 u16TagID2);
MS_BOOL _MHal_GFLIP_SetGopDstClk(MS_U32 u32GopIdx, MS_U16 u16GopDst);
//=============================================================================================
// Local Function
//=============================================================================================
//-------------------------------------------------------------------------------------------------
/// Set GOP Bank Regs.
/// @param  u8bank                  \b IN: The gop bank value
/// @return void
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
void _MHal_GFLIP_SetGopBank(MS_U8 u8bank)
{
    MS_U16 u16Bank;
    u16Bank = GOP_REG(REG_GOP_WR_ACK);
    u16Bank &= ~GOP_BANK_SEL;
    u16Bank |= (u8bank&GOP_BANK_SEL);
    GOP_REG(REG_GOP_WR_ACK) = u16Bank;
}

MS_U8 _MHal_GFLIP_GetGopBank(void)
{
    return (MS_U8)(GOP_REG(REG_GOP_WR_ACK)&GOP_BANK_SEL);
}
//-------------------------------------------------------------------------------------------------
/// Set GOP Regs.
/// @param  u32GopIdx                  \b IN: The gop idx value
/// @param  u16BankIdx                  \b IN: The gop bank idx value, should be GFLIP_GOP_BANK_IDX_0/GFLIP_GOP_BANK_IDX_1
/// @param  u16Addr                  \b IN: The gop reg address
/// @param  u16Val                  \b IN: The gop reg value
/// @param  u16Mask                  \b IN: The gop reg mask
/// @return void
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
void _MHal_GFLIP_WriteGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16 u16Val, MS_U16 u16Mask)
{
    MS_U16 u16RegVal;
    MS_U8  u8Bank;

   // GFLIPHAL_ASSERT(((u32GopIdx==GFLIP_GOP_IDX_4G)||(u32GopIdx==GFLIP_GOP_IDX_2G)), "Invalid GopIdx, Currently Only support Gop_4G and Gop_2G!\n");

    u8Bank = _MHal_GFLIP_GetGopBank();

    //[gopIdx-->bank offset]: [0--->0; 1--->3; 2--->6; 3--->9]
     _MHal_GFLIP_SetGopBank((GFLIP_GOP_BANKOFFSET * u32GopIdx) + u16BankIdx);

    u16RegVal = GOP_REG(u16Addr);

    u16RegVal &= ~u16Mask;
    u16RegVal |= (u16Val & u16Mask);

    GOP_REG(u16Addr) = u16RegVal;
    _MHal_GFLIP_SetGopBank(u8Bank);
}

//-------------------------------------------------------------------------------------------------
/// Get GOP Regs.
/// @param  u32GopIdx                  \b IN: The gop idx value
/// @param  u16BankIdx                  \b IN: The gop bank idx value, should be GFLIP_GOP_BANK_IDX_0/GFLIP_GOP_BANK_IDX_1
/// @param  u16Addr                  \b IN: The gop reg address
/// @param  *u16Val                  \b IN: The return gop reg value
/// @return void
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
void _MHal_GFLIP_ReadGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16* u16Val)
{
    MS_U8  u8Bank;

   // GFLIPHAL_ASSERT(((u32GopIdx==GFLIP_GOP_IDX_4G)||(u32GopIdx==GFLIP_GOP_IDX_2G)), "Invalid GopIdx, Currently Only support Gop_4G and Gop_2G!\n");

    u8Bank = _MHal_GFLIP_GetGopBank();

    //[gopIdx-->bank offset]: [0--->0; 1--->3; 2--->6; 3--->9]
     _MHal_GFLIP_SetGopBank((GFLIP_GOP_BANKOFFSET * u32GopIdx) + u16BankIdx);

    *u16Val = GOP_REG(u16Addr);

    _MHal_GFLIP_SetGopBank(u8Bank);
}

//-------------------------------------------------------------------------------------------------
/// Set GOP Regs.
/// @param  u32GopIdx                  \b IN: The gop idx value
/// @param  u32GwinIdx                  \b IN: The gop gwin idx value,
/// @return MS_U32 The valid GWinID for related Gop
/// @attention
/// <b>[GFLIP] <em>GOP0: 0/1/2/3; GOP1: 0/1; GOP2: 1; GOP3: 1</em></b>
//-------------------------------------------------------------------------------------------------
MS_U32 _MHal_GFLIP_GetValidGWinIDPerGOPIdx(MS_U32 u32GopIdx, MS_U32 u32GwinIdx)
{
    switch(u32GopIdx)
    {
        case GFLIP_GOP_IDX_2G:
        case GFLIP_GOP_IDX_2GX:
            u32GwinIdx = u32GwinIdx % 2;
            break;
        case GFLIP_GOP_IDX_1G:
        case GFLIP_GOP_IDX_1GX:
            u32GwinIdx = 0; //only GWin 1 is valid;
            break;
        default:
            break;
    }

    return u32GwinIdx;
}

//-------------------------------------------------------------------------------------------------
/// Differ Two TagIDs.
/// @param  u16TagID1                  \b IN: The First TagID
/// @param  u16TagID2                  \b IN: The Second TagID
/// @return MS_S32: The different of two IDs.
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_S32 _MHal_GFLIP_DirectSerialDiff(MS_U16 u16TagID1,  MS_U16 u16TagID2)
{
    if(u16TagID1 < u16TagID2)
    {
        if((u16TagID2-u16TagID1)>0x7FFF)
         {
             return (MS_S32)(0xFFFFUL-u16TagID2+u16TagID1+1);
         }
        else
            return -(MS_S32)(u16TagID2-u16TagID1);
    }
    else
    {
        if((u16TagID1-u16TagID2)>0x7FFF)
         {
             return -(MS_S32)(0xFFFFUL-u16TagID1+u16TagID2+1);
         }
        else
            return (MS_S32)(u16TagID1-u16TagID2);
    }
}

//-------------------------------------------------------------------------------------------------
/// Get Gop Current Dst Plane.
/// @param  u32GopIdx                  \b IN: The gop idx who is the owner
/// @return TRUE/FALSE
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_U16 MHal_GFLIP_GetGopDst(MS_U32 u32GopIdx)
{
    MS_U16 u16RegVal;

    _MHal_GFLIP_ReadGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_0, REG_GOP_CTRL1, &u16RegVal);

    return (u16RegVal & GOP_DST_MASK);
}

//-------------------------------------------------------------------------------------------------
/// Set Gop Dst Plane Clock Src.
/// @param  u16GopDst                  \b IN: The gop Dst Clock Src
/// @return TRUE/FALSE
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL _MHal_GFLIP_SetGopDstClk(MS_U32 u32GopIdx, MS_U16 u16GopDst)
{
    MS_U16 u16RegAddr, u16RegVal, u16RegMask;

    switch(u16GopDst)
    {
        case GFLIP_GOP_DST_IPMAIN:
            switch(u32GopIdx)
            {
                case GFLIP_GOP_IDX_2G:
                    u16RegAddr = GOP_GOPCLK;
                    u16RegVal = CKG_GOPG0_IDCLK2;
                    u16RegMask = CKG_GOPG0_MASK;
                    break;
                case GFLIP_GOP_IDX_2GX:
                    u16RegAddr = GOP_GOPCLK;
                    u16RegVal = CKG_GOPG1_IDCLK2;
                    u16RegMask = CKG_GOPG1_MASK;
                    break;
                case GFLIP_GOP_IDX_1G:
                    u16RegAddr = GOP_GOP2CLK;
                    u16RegVal = CKG_GOPG2_IDCLK2;
                    u16RegMask = CKG_GOPG2_MASK;
                    break;
                case GFLIP_GOP_IDX_1GX:
                    u16RegAddr = GOP_GOP3CLK;
                    u16RegVal = CKG_GOPG3_IDCLK2;
                    u16RegMask = CKG_GOPG3_MASK;
                    break;
                default:
                    return FALSE;
            }

            break;
        case GFLIP_GOP_DST_IPSUB:
            switch(u32GopIdx)
            {
                case GFLIP_GOP_IDX_2G:
                    u16RegAddr = GOP_GOPCLK;
                    u16RegVal = CKG_GOPG0_IDCLK1;
                    u16RegMask = CKG_GOPG0_MASK;
                    break;
                case GFLIP_GOP_IDX_2GX:
                    u16RegAddr = GOP_GOPCLK;
                    u16RegVal = CKG_GOPG1_IDCLK1;
                    u16RegMask = CKG_GOPG1_MASK;
                    break;
                case GFLIP_GOP_IDX_1G:
                    u16RegAddr = GOP_GOP2CLK;
                    u16RegVal = CKG_GOPG2_IDCLK1;
                    u16RegMask = CKG_GOPG2_MASK;
                    break;
                case GFLIP_GOP_IDX_1GX:
                    u16RegAddr = GOP_GOP3CLK;
                    u16RegVal = CKG_GOPG3_IDCLK1;
                    u16RegMask = CKG_GOPG3_MASK;
                    break;
                default:
                    return FALSE;
            }

            break;
        case GFLIP_GOP_DST_OP0:
            switch(u32GopIdx)
            {
                case GFLIP_GOP_IDX_2G:
                    u16RegAddr = GOP_GOPCLK;
                    u16RegVal = CKG_GOPG0_ODCLK;
                    u16RegMask = CKG_GOPG0_MASK;
                    break;
                case GFLIP_GOP_IDX_2GX:
                    u16RegAddr = GOP_GOPCLK;
                    u16RegVal = CKG_GOPG1_ODCLK;
                    u16RegMask = CKG_GOPG1_MASK;
                    break;
                case GFLIP_GOP_IDX_1G:
                    u16RegAddr = GOP_GOP2CLK;
                    u16RegVal = CKG_GOPG2_ODCLK;
                    u16RegMask = CKG_GOPG2_MASK;
                    break;
                case GFLIP_GOP_IDX_1GX:
                    u16RegAddr = GOP_GOP3CLK;
                    u16RegVal = CKG_GOPG3_ODCLK;
                    u16RegMask = CKG_GOPG3_MASK;
                    break;
                default:
                    return FALSE;
            }

            break;
        case GFLIP_GOP_DST_VOP:
        case GFLIP_GOP_DST_VOPSUB:
        default:
            return FALSE;
    }

    CKG_REG(u16RegAddr) = (CKG_REG(u16RegAddr) & ~u16RegMask) | u16RegVal;

    return TRUE;
}

//=============================================================================
// HAL Driver Function
//=============================================================================
//-------------------------------------------------------------------------------------------------
/// Enable/Disable Vsync. Interrupt of gop.
/// @param  u32GopIdx                  \b IN: The gop idx which need enable/disable vsync. interrupt
/// @param bEnable                          \b IN: enable or disable
/// @return TRUE/FALSE
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_IntEnable(MS_U32 u32GopIdx, MS_BOOL bEnable)
{
    MS_U16 u16RegVal;

    if(bEnable)
    {
        u16RegVal = GOP_INTMASK_VS1;
    }
    else
    {
        u16RegVal = GOP_INTMASK_VS0 | GOP_INTMASK_VS1;
    }

    _MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_0, REG_GOP_INT, u16RegVal, (GOP_INTMASK_VS0 | GOP_INTMASK_VS1));

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// check if the vsync. interrupt
/// @param  void
/// @return MS_BOOL: TRUE/FALSE
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_IsVSyncInt(MS_U32 u32GopIdx)
{
    MS_U8  u8Bank;

    u8Bank = _MHal_GFLIP_GetGopBank();

    {
        //[gopIdx-->bank offset]: [0--->0; 1--->3; 2--->6; 3--->7]
        _MHal_GFLIP_SetGopBank((GFLIP_GOP_BANKOFFSET * (MS_U8)u32GopIdx) + GFLIP_GOP_BANK_IDX_0);
        if(GOP_REG(REG_GOP_INT) & GOP_INT_VS0)
        {
            _MHal_GFLIP_SetGopBank(u8Bank);
            return TRUE;
        }
    }

    _MHal_GFLIP_SetGopBank(u8Bank);
    return FALSE;
}

//-------------------------------------------------------------------------------------------------
/// Get the Gop Idx who fire the vsync. interrupt
/// @param  void
/// @return MS_U32: the gop idx
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_U32 MHal_GFLIP_GetIntGopIdx(void)
{
    MS_U16 u16RegVal;

    u16RegVal = (GOP_REG(REG_GOP_WR_ACK) & GOP_INT_FLAG_MASK) | (GOP_REG(REG_GOP_MUX) & GOPD_INT_FLAG_MASK);

    if(u16RegVal & GOP2G_INT_FLAG)
    {
        return GFLIP_GOP_IDX_2G;
    }
    else if(u16RegVal & GOP2GX_INT_FLAG)
    {
        return GFLIP_GOP_IDX_2GX;
    }
/*
    else if(u16RegVal & GOPD_INT_FLAG)
    {
        return GFLIP_GOP_IDX_DWIN;
    }
*/
    else if(u16RegVal & GOP1G_INT_FLAG)
    {
        return GFLIP_GOP_IDX_1G;
    }
    else if(u16RegVal & GOP1GX_INT_FLAG)
    {
        return GFLIP_GOP_IDX_1GX;
    }
    else
    {
         return GFLIP_GOP_IDX_INVALID;
        //GFLIPHAL_ASSERT(FALSE, "MHal_GFLIP_GetIntGopIdx: unknow GOP Interrupt MASK!\n");
    }
}

//-------------------------------------------------------------------------------------------------
/// Kick off Flip to Gop HW.
/// @param  u32GopIdx                  \b IN: The gop idx who is the flip owner
/// @param  u32GwinIdx                 \b IN: The gwin idx who is the flip owner
/// @param  u32MainAddr                \b IN: The flip address for normal 2D display
/// @param  u32SubAddr                 \b IN: The 2nd fb flip address for GOP 3D display
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_SetFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32MainAddr, MS_U32 u32SubAddr)
{
    MS_U16 u16FWR = GOP_REG(REG_GOP_WR_ACK)&GOP_FWR;

    u32GwinIdx = _MHal_GFLIP_GetValidGWinIDPerGOPIdx(u32GopIdx, u32GwinIdx);

    _MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_1, REG_GOP_DRAM_RBLK_STR_L(u32GwinIdx), LO16BIT(u32MainAddr), 0xFFFF);
    _MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_1, REG_GOP_DRAM_RBLK_STR_H(u32GwinIdx), HI16BIT(u32MainAddr), 0xFFFF);
    if(u32SubAddr != 0)
    {
        _MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_1, REG_GOP_3DOSD_SUB_RBLK_L(u32GwinIdx), LO16BIT(u32SubAddr), 0xFFFF);
        _MHal_GFLIP_WriteGopReg(u32GopIdx, GFLIP_GOP_BANK_IDX_1, REG_GOP_3DOSD_SUB_RBLK_H(u32GwinIdx), HI16BIT(u32SubAddr), 0xFFFF);
    }
    if(u16FWR == 0)
    {
         GOP_REG(REG_GOP_WR_ACK) |= GOP_FWR;
         GOP_REG(REG_GOP_WR_ACK) &= ~GOP_FWR;
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Clear DWinIRQ if It happened.
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_ClearDWINIRQ(GFLIP_DWININT_INFO *pGFlipDWinIntInfo)
{
    MS_U8  u8Bankbk;
    MS_U16 u16ValEn, u16ValDe;

    if(!(GOP_REG(REG_GOP_MUX) & GOPD_INT_FLAG))
    {
        return FALSE;
    }

    u8Bankbk = _MHal_GFLIP_GetGopBank();

    _MHal_GFLIP_SetGopBank(GFLIP_GOP_DWIN_BANKID);

    u16ValEn = u16ValDe = GOP_REG(REG_GOP_DWIN_INT);

    //Check and Clear DWin IRQs:
    if(u16ValEn & GOP_DWIN_INT_WADR)
    {
        u16ValEn &= ~GOP_DWIN_INTMASK_WADR;
        u16ValDe |= GOP_DWIN_INTMASK_WADR;
        pGFlipDWinIntInfo->sDwinIntInfo.bDWinIntWADR = 0x1;
    }

    if(u16ValEn & GOP_DWIN_INT_PROG)
    {
        u16ValEn &= ~GOP_DWIN_INTMASK_PROG;
        u16ValDe |= GOP_DWIN_INTMASK_PROG;
        pGFlipDWinIntInfo->sDwinIntInfo.bDWinIntPROG = 0x1;
    }

    if(u16ValEn & GOP_DWIN_INT_TF)
    {
        u16ValEn &= ~GOP_DWIN_INTMASK_TF;
        u16ValDe |= GOP_DWIN_INTMASK_TF;
        pGFlipDWinIntInfo->sDwinIntInfo.bDWinIntTF = 0x1;
    }

    if(u16ValEn & GOP_DWIN_INT_BF)
    {
        u16ValEn &= ~GOP_DWIN_INTMASK_BF;
        u16ValDe |= GOP_DWIN_INTMASK_BF;
        pGFlipDWinIntInfo->sDwinIntInfo.bDWinIntBF = 0x1;
    }

    if(u16ValEn & GOP_DWIN_INT_VS)
    {
        u16ValEn &= ~GOP_DWIN_INTMASK_VS;
        u16ValDe |= GOP_DWIN_INTMASK_VS;
        pGFlipDWinIntInfo->sDwinIntInfo.bDWinIntVS = 0x1;
    }

    //Disable Interrupt:
    GOP_REG(REG_GOP_DWIN_INT) = u16ValDe;
    //Enable Interrupt:
    GOP_REG(REG_GOP_DWIN_INT) = u16ValEn;

    _MHal_GFLIP_SetGopBank(u8Bankbk);
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Handle HW Limitation -- Vsync Limitation.
/// @param  u32GopIdx                  \b IN: The gop idx who is the owner
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em>The Limitation is found on U3/T2/T3. When GOP Src Clock is no signal and GOP
///     Vsync is on, Then there will be no chance to Reset GOP Vsync Interrupt. this will leads to
///     system looks like hang(busy handling interrupt always). T2/T3/U3 will Switch GOP Dst
///     to OP to fix the issue and later chips could Reset GOP to clear interrupt Bit.</em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_HandleVsyncLimitation(MS_U32 u32GopIdx)
{
    //Save Current Dst Clock Src:
    _u16GflipGOPDst[u32GopIdx] = MHal_GFLIP_GetGopDst(u32GopIdx);

    //Set Dst Clock Src to OP:
    _MHal_GFLIP_SetGopDstClk(u32GopIdx, GFLIP_GOP_DST_OP0);

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Restore HW Limitation -- Vsync Limitation.
/// @param  u32GopIdx                  \b IN: The gop idx who is the owner
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em>The Limitation is found on U3/T2/T3. When GOP Src Clock is no signal and GOP
///     Vsync is on, Then there will be no chance to Reset GOP Vsync Interrupt. this will leads to
///     system looks like hang(busy handling interrupt always). T2/T3/U3 will Switch GOP Dst
///     to OP to fix the issue and later chips could Reset GOP to clear interrupt Bit.</em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_RestoreFromVsyncLimitation(MS_U32 u32GopIdx)
{
    //Restore Dst Clock Src:
    _MHal_GFLIP_SetGopDstClk(u32GopIdx, _u16GflipGOPDst[u32GopIdx]);

    //Restore Interrupt:
    MHal_GFLIP_IntEnable(u32GopIdx, TRUE);

    //Restore Current Dst Clock Src to OP:
    _u16GflipGOPDst[u32GopIdx] = GFLIP_GOP_DST_OP0;

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Check If the gived TagId has back or not.
/// @param  u32TagId                  \b IN: The TagId which need to be check
/// @return TRUE: back
/// @return FALSE: not back
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_IsTagIDBack(MS_U16 u16TagId)
{
    MS_U16 u16TagIDHW;
    MS_U32 u32RegVal;

    u16TagIDHW = GE_REG(REG_GE_TAG);

    if(_MHal_GFLIP_DirectSerialDiff(u16TagIDHW, u16TagId) >= 0)
    {    //TagID is back!
          // printk("tagIDHW = %04x\n", tagID_HW);
        return TRUE;
    }

    u32RegVal = GE_REG(REG_GE_STATUS);
    if((u32RegVal & GE_CMDQ2_STATUS) < (16UL<<11)) //still has commands in queue.
    {
        return FALSE;
    }

    if((u32RegVal & GE_CMDQ1_STATUS) < (16UL<<3)) //still has commands in queue.
    {
        return FALSE;
    }

    if(GE_REG(REG_GE_FMT_BLT) & GE_EN_VCMDQ) //still has commands in VCMDQ.
    {
        u32RegVal = GE_REG(REG_GE_VQ_FIFO_STATUS_L);
        u32RegVal |= (GE_REG(REG_GE_VQ_FIFO_STATUS_H)&1)<<16;
        if(u32RegVal)
        {
            return FALSE;
        }
    }

    if(GE_REG(REG_GE_STATUS) & GE_BUSY) //GE still busy
    {
        return FALSE;
    }

    return TRUE;
}


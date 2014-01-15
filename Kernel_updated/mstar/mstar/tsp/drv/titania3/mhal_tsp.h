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

#ifndef _MHAL_TSP_H_
#define _MHAL_TSP_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// file   mhal_tsp.h
// @brief  Transport Stream Processer (TSP) HAL
// @author MStar Semiconductor,Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mhal_tsp_hardware.h"

#define TSP_MCU_REGMAP                                                  // Type defined
#include "mhal_tsp_reg.h"
#undef TSP_MCU_REGMAP

//--------------------------------------------------------------------------------------------------
//  Driver Compiler Option
//--------------------------------------------------------------------------------------------------
#define TSP_HAL_OPTIMIZE            1
#define TSP_HAL_REG_SAFE_MODE       1                                   // Register protection access between 1 task and 1+ ISR

//[HWBUG]
#define MULTI_ACCESS_SW_PATCH       1                                   // It's still risk becuase some registers like readaddr will
                                                                        // cause overflow before patching to correct value.
//[HWBUG]

// FW COMMAND
#define TSP_CMD_CLEAR           0x00
#define TSP_CMD_RESET_FLT       0xC0   // engine_id, secflt_id, type_id
#define TSP_CMD_SET_NOTVER      0xC1   // secflt_id, version
#define TSP_CMD_CLR_NOTVER      0xC2   // secflt_id, version
#define TSP_CMD_MAP_PIDFLT      0xC3   // engine_id, sectflt_id, pidflt_id
#define TSP_CMD_SET_PCROFS      0xC4

//--------------------------------------------------------------------------------------------------
//  TSP Hardware Abstraction Layer
//--------------------------------------------------------------------------------------------------

// TS Interface
typedef enum
{
    E_HALTSP_TSIF_SERIAL        = 0x0,
    E_HALTSP_TSIF_PARALLEL      = 0x1,
} HalTSP_TSInterface;

typedef enum {
    E_HALTSP_FLT_TYPE_UNKNOWN = 0,
    E_HALTSP_FLT_TYPE_SECTION,
    E_HALTSP_FLT_TYPE_TELETEXT,
    E_HALTSP_FLT_TYPE_PCR,
    E_HALTSP_FLT_TYPE_PES,
    E_HALTSP_FLT_TYPE_PACKET
} HalTSP_FltType;

// Don't change the order unless you are very sure what you are doing
typedef enum
{
    /// Record ENG0 by @ref E_DRVTSP_FLT_TYPE_PVR
    E_HALTSP_REC_MODE_ENG0_FLTTYPE = 0x00000000,                        // TSP_PVR_CTRL_ENG(0)
    /// Record ENG1 by @ref E_DRVTSP_FLT_TYPE_PVR
    E_HALTSP_REC_MODE_ENG1_FLTTYPE = 0x00000001,                        // TSP_PVR_CTRL_ENG(1)
    /// Record ENG0 bypass PID fliter
    E_HALTSP_REC_MODE_ENG0_BYPASS      = 0x00000002,                    // TSP_PVR_CTRL_ENG(0) + TSP_PVR_CTRL_BYPASS
    /// Record ENG1 bypass PID fliter
    E_HALTSP_REC_MODE_ENG1_BYPASS      = 0x00000003,                    // TSP_PVR_CTRL_ENG(1) + TSP_PVR_CTRL_BYPASS
} HalTSP_RecMode;

typedef enum
{
    E_HALTSP_OUT_PATH_VIDEO = 0,
    E_HALTSP_OUT_PATH_AUDIO,
    E_HALTSP_OUT_PATH_SEC
} HalTSP_Output_Path;

// TSP Register
#if TSP_HAL_OPTIMIZE
#define _TspPid                     ((REG_Pid*)(REG_PIDFLT_BASE))
#define _TspCtrl                    ((REG_Ctrl*)(REG_CTRL_BASE))
#define _TspSec                     ((REG_Sec*)(REG_SECFLT_BASE))
#define _TspCsaCtrl                 ((REG_CsaCtrl*)(REG_CSACTRL_BASE))
#else
static REG_Pid                      *_TspPid = (REG_Pid*)REG_PIDFLT_BASE; // TSP engine array
static REG_Ctrl                     *_TspCtrl = (REG_Ctrl*)REG_CTRL_BASE;
static REG_Sec                      *_TspSec = (REG_Sec*)REG_SECFLT_BASE;
static REG_CsaCtrl                  *_TspCsaCtrl= (REG_CsaCtrl*)REG_CSACTRL_BASE;
#endif

#if TSP_HAL_REG_SAFE_MODE
#define TSP_ISR_SAVE_ALL            MHal_TSP_isr_save_all()
#define TSP_ISR_RESTORE_ALL         MHal_TSP_isr_restore_all()
#else
#define TSP_ISR_SAVE_ALL            { }
#define TSP_ISR_RESTORE_ALL         { }
#endif

//[NOTE] Jerry
// Some register has write order, for example, writing PCR_L will disable PCR counter
// writing PCR_M trigger nothing, writing PCR_H will enable PCR counter
#define REG32_W(reg, value)         (reg)->L = ((value) & 0x0000FFFF);                          \
                                    (reg)->H = ((value) >> 16); \
                                    mb();

#define REG16_W(reg, value)         (reg)->data = ((value) & 0x0000FFFF); \
                                    mb();

//--------------------------------------------------------------------------------------------------
//  Macro of bit operations
//--------------------------------------------------------------------------------------------------
#define HAS_FLAG(flag, bit)        ((flag) & (bit))
#define SET_FLAG(flag, bit)        ((flag)|= (bit))
#define RESET_FLAG(flag, bit)      ((flag)&= (~(bit)))
#define SET_FLAG1(flag, bit)       ((flag)|  (bit))
#define RESET_FLAG1(flag, bit)     ((flag)&  (~(bit)))

#ifdef MSOS_TYPE_LINUX
#define ASSERT(b)                  if (!(b)) panic("TSP kernel panic");
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// External functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

U32     REG32_IndR(REG32 *reg);
void    REG32_IndW(REG32 *reg, U32 value);
void    MHal_TSP_SetTSIFExtSync(U32 u32EngId, U32 u32Tsin, B16 bExtSync);
void    MHal_TSP_SetTSIFType(U32 u32EngId, U32 u32Tsin, U32 u32TSIFType);
void    MHal_TSP_isr_save_all(void);
void    MHal_TSP_isr_restore_all(void);
void    MHal_TSP_SecFlt_SetMatchMask(REG_SecFlt *pSecFilter, U8 *pu8Match, U8 *pu8Mask);
void    MHal_TSP_SecFlt_SetType(REG_SecFlt *pSecFilter, U32 u32Type);
U32     MHal_TSP_SecFlt_GetType(REG_SecFlt *pSecFilter);

void    MHal_TSP_PVR_SetBuffer(U32 u32BufStart0, U32 u32BufStart1, U32 u32BufSize);
void    MHal_TSP_PVR_SetMode(HalTSP_RecMode eRecMode);

U32     MHal_TSP_PVR_GetCtrlMode(void);
void    MHal_TSP_PVR_Enable(B16 bEnable);
void    MHal_TSP_filein_enable(B16 b_enable);
void    MHal_TSP_tsif_enable(U32 u32TSIF, B16 bEnable);
void    MHal_TSP_WbDmaEnable(B16 bEnable);
void    MHal_TSP_NewDmaWriteArbiter(B16 bEnable);
void    MHal_TSP_SwReset(void);
void    MHal_TSP_FW_load(U32 u32FwAddrVirt, U32 u32FwSize, B16 bFwDMA, B16 bIQmem, B16 bDQmem);
void    MHal_TSP_Scmb_Detect(B16 bEnable);
void    MHal_TSP_HwPatch(void);
void    MHal_TSP_CPU_SetBase(void);
void    MHal_TSP_SelPad(U32 u32EngId, U32 PadId);
void    MHal_TSP_MUX1_SelPad(U32 u32EngId, U32 PadId);
void    MHal_TSP_SetCtrlMode(U32 u32EngId, U32 u32Mode, U32 u32TsIfId);

U32     MHal_TSP_GetSDRBase(void);

void    MHal_TSP_SendFWCmd(U8 cmd, U8 param1, U8 param2, U8 param3);

void    MHal_TSP_RemoveErrPacket_Enable(HalTSP_Output_Path path);
void    MHal_TSP_RemoveErrPacket_Disable(HalTSP_Output_Path path);

void    MHal_TSP_Flush_AV_FIFO(U32 u32StreamId, B16 bFlush);
U8      MHal_TSP_Fetch_AV_FIFO(U32 u32StreamId);
B16     MHal_TSP_Alive(void);

#ifdef TSP_DUMP	// changed to remove warning(dreamer@lge.com)
void    _TDump_SecFlt(U32 u32EngId, U32 u32SecFltId);
void    _TDump_Flt(U32 u32EngId, U32 u32IdxStart, U32 u32IdxEnd);
#endif // TSP_DUMP

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline function declaration
/////////////////////////////////////////////////////////////////////////////////////////////////////

static inline U32       REG32_R(REG32 *reg);
static inline U16       REG16_R(REG16 *reg);
static inline void      MHal_TSP_PidFlt_SetPid(REG_PidFlt *pPidFilter, U32 u32PID);
static inline U32       MHal_TSP_PidFlt_GetPid(REG_PidFlt* pPidFilter);
static inline void      MHal_TSP_PidFlt_SelSecFlt(REG_PidFlt *pPidFilter, U32 u32SecFltId);
static inline U32       MHal_TSP_PidFlt_GetSecFlt(REG_PidFlt *pPidFilter);
static inline void      MHal_TSP_PidFlt_SelFltOutput(REG_PidFlt *pPidFilter, U32 u32FltOutput);
static inline U32       MHal_TSP_PidFlt_GetFltOutput(REG_PidFlt *pPidFilter);
static inline void      MHal_TSP_PidFlt_SelFltSource(REG_PidFlt *pPidFilter, U32 u32FltSource);
static inline U32       MHal_TSP_PidFlt_GetFltSource(REG_PidFlt *pPidFilter);
static inline U32       MHal_TSP_SecFlt_GetState(REG_SecFlt *pSecFilter);
static inline void      MHal_TSP_SecFlt_SetParam(REG_SecFlt *pSecFilter, U32 u32SecFltParam);
static inline U32       MHal_TSP_SecFlt_GetCtrl(REG_SecFlt *pSecFilter);
static inline void      MHal_TSP_SecFlt_ClrCtrl(REG_SecFlt *pSecFilter);
static inline U32       MHal_TSP_SecFlt_GetMode(REG_SecFlt *pSecFilter);
static inline void      MHal_TSP_SecFlt_SetMode(REG_SecFlt *pSecFilter, U32 u32SecFltMode);
static inline void      MHal_TSP_SecFlt_SetRmnCount(REG_SecFlt *pSecFilter, U32 u32RmnCount);
static inline void      MHal_TSP_SecFlt_SetReqCount(REG_SecFlt *pSecFilter, U32 u32ReqCount);
static inline U32       MHal_TSP_SecFlt_GetCRC32(REG_SecFlt *pSecFilter);
static inline void      MHal_TSP_SecFlt_ResetState(REG_SecFlt* pSecFilter);
static inline void      MHal_TSP_SecFlt_SetBuffer(REG_SecFlt *pSecFlt, U32 u32StartAddr, U32 u32BufSize);
static inline void      MHal_TSP_SecFlt_ResetBuffer(REG_SecFlt *pSecFlt);
static inline void      MHal_TSP_SecFlt_SetBufRead(REG_SecFlt *pSecFlt, U32 u32ReadAddr);
static inline U32       MHal_TSP_SecFlt_GetBufStart(REG_SecFlt *pSecFlt);
static inline U32       MHal_TSP_SecFlt_GetBufEnd(REG_SecFlt *pSecFlt);
static inline U32       MHal_TSP_SecFlt_GetBufRead(REG_SecFlt *pSecFlt);
static inline U32       MHal_TSP_SecFlt_GetBufWrite(REG_SecFlt *pSecFlt);
static inline U32       MHal_TSP_SecFlt_GetBufCur(REG_SecFlt *pSecFlt);
static inline void      MHal_TSP_PVR_WaitFlush(void);
static inline void      MHal_TSP_PVR_Reset(void);
static inline U32       MHal_TSP_PVR_GetBufWrite(void);
static inline void      MHal_TSP_TsDma_SetDelay(U32 u32Delay);
static inline void      MHal_TSP_TsDma_Pause(void);
static inline void      MHal_TSP_TsDma_Resume(void);
static inline void      MHal_TSP_CmdQ_TsDma_SetAddr(U32 u32StreamAddr);
static inline U32       MHal_TSP_CmdQ_TsDma_GetReadPtr(void);
static inline void      MHal_TSP_CmdQ_TsDma_SetSize(U32 u32StreamSize);
static inline void      MHal_TSP_CmdQ_TsDma_Reset(void);
static inline void      MHal_TSP_CmdQ_TsDma_Start(U32 u32TsDmaCtrl);
static inline U32       MHal_TSP_CmdQ_TsDma_GetState(void);
static inline void      MHal_TSP_CmdQ_SetSTC(U32 u32EngId, U32 u32STC);
static inline void      MHal_TSP_CmdQ_SetSTC_32(U32 u32EngId, U32 u32STC_32);
static inline B16       MHal_TSP_CmdQ_IsEmpty(void);
static inline U32       MHal_TSP_CmdQ_EmptyCount(void);
static inline U32       MHal_TSP_CmdQ_CmdCount(void);
static inline U32       MHal_TSP_CmdQ_GetWriteLevel(void);
static inline void      MHal_TSP_CmdQ_ClearQ(void);
static inline U32       MHal_TSP_GetSTC(U32 u32EngId);
static inline U32       MHal_TSP_GetSTC_32(U32 u32EngId);
static inline void      MHal_TSP_SetFwMsg(U32 u32Mode);
static inline void      MHal_TSP_SelAudOut(U32 u32EngId);
static inline void      MHal_TSP_PSAudioEnable(U32 u32EngId);
static inline void      MHal_TSP_PSAudioDisable(U32 u32EngId);
static inline void      MHal_TSP_PSVideoEnable(U32 u32EngId);
static inline void      MHal_TSP_PSVideoDisable(U32 u32EngId);

static inline U32       MHal_TSP_GetFwMsg(void);
static inline U32       MHal_TSP_GetCtrlMode(U32 u32EngId);
static inline void      MHal_TSP_Int_Enable(U32 u32Mask);
static inline void      MHal_TSP_Int_Disable(U32 u32Mask);
static inline void      MHal_TSP_Int_ClearSw(void);
static inline void      MHal_TSP_Int_ClearHw(U32 u32Mask);
static inline void      MHal_TSP_Ind_Enable(void);
static inline B16       MHal_TSP_Scmb_Status(B16 bTsin, U32 u32PidFltId);

static inline void      MHal_TSP_VarClk_Enable(void);
static inline void      MHal_TSP_VarClk_Disable(void);
static inline void      MHal_TSP_OneBitSync_Enable(void);
static inline void      MHal_TSP_OneBitSync_Disable(void);

static inline void      MHal_TSP_CSA_Set_ESA_Mode(U32 u32Mode);
static inline void      MHal_TSP_CSA_KL_Set_ACPU_CMD(U32 u32ACPU_cmd);
static inline void      MHal_TSP_CSA_KL_Start_ACPU_CMD(void);
static inline void      MHal_TSP_CSA_Sel_ScrmEng(U32 u32ScrmEng);
static inline void      MHal_TSP_CSA_Set_ScrmPath(U32 u32ScrmPath, B16 bEnableOutputAV);
static inline void      MHal_TSP_CSA_KTE_Set(U8 Valid,
                                             U32 u32Tsc,
                                             U8 PfIdx, U8 RWPos, U32 Key);

static inline void      MHal_TSP_DoubleBuf_En(B16 bBuf_Sel);
static inline void      MHal_TSP_DoubleBuf_Disable(void);
static inline void      MHal_TSP_LiveAB_En(B16 bPF_EN);
static inline void      MHal_TSP_SetPktSize(U32 u32PKTSize);
static inline void      MHal_TSP_PVR_TimeStampSetRecordStamp(U32 u32Stamp);
static inline U32       MHal_TSP_PVR_TimeStampGetRecordStamp(void);
static inline void      MHal_TSP_PVR_TimeStampSetPlaybackStamp(U32 u32Stamp);
static inline U32       MHal_TSP_PVR_TimeStampGetPlaybackStamp(void);
static inline void      MHal_TSP_PVR_TimeStampEnable(B16 bEnable);
static inline void      MHal_TSP_PVR_TimeStampRecord(B16 bEnable);
static inline U32       MHal_TSP_PVR_TimeStampGetFileinPacket(void);
static inline U32       MHal_TSP_GetPcr(U32 u32EngId);
static inline U32       MHal_TSP_GetPcr_32(U32 u32EngId);

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementation
/////////////////////////////////////////////////////////////////////////////////////////////////////

U32 REG32_R(REG32 *reg)
{
    U32     value;

    mb();

    value = (reg)->H << 16;
    value |= (reg)->L;

    return value;
}

U16 REG16_R(REG16 *reg)
{
    U16     value;

    mb();

    value = (reg)->data;

    return value;
}

void MHal_TSP_PidFlt_SetPid(REG_PidFlt *pPidFilter, U32 u32PID)
{
    REG32_IndW(pPidFilter, (REG32_IndR(pPidFilter) & ~TSP_PIDFLT_PID_MASK) | ((u32PID << TSP_PIDFLT_PID_SHFT) & TSP_PIDFLT_PID_MASK));
}

U32 MHal_TSP_PidFlt_GetPid(REG_PidFlt* pPidFilter)
{
    return (HAS_FLAG(REG32_IndR(pPidFilter), TSP_PIDFLT_PID_MASK)>> TSP_PIDFLT_PID_SHFT);
}

void MHal_TSP_PidFlt_SelSecFlt(REG_PidFlt *pPidFilter, U32 u32SecFltId)
{
    REG32_IndW(pPidFilter, (REG32_IndR(pPidFilter) & ~TSP_PIDFLT_SECFLT_MASK) | ((u32SecFltId << TSP_PIDFLT_SECFLT_SHFT) & TSP_PIDFLT_SECFLT_MASK));
}

U32 MHal_TSP_PidFlt_GetSecFlt(REG_PidFlt *pPidFilter)
{
    return ((REG32_IndR(pPidFilter) & TSP_PIDFLT_SECFLT_MASK) >> TSP_PIDFLT_SECFLT_SHFT);
}

void MHal_TSP_PidFlt_SelFltOutput(REG_PidFlt *pPidFilter, U32 u32FltOutput)
{
    REG32_IndW(pPidFilter, (REG32_IndR(pPidFilter) & ~TSP_PIDFLT_OUT_MASK) | (u32FltOutput & TSP_PIDFLT_OUT_MASK));
}

U32  MHal_TSP_PidFlt_GetFltOutput(REG_PidFlt *pPidFilter)
{
    return (REG32_IndR(pPidFilter) & TSP_PIDFLT_OUT_MASK);
}

void MHal_TSP_PidFlt_SelFltSource(REG_PidFlt *pPidFilter, U32 u32FltSource)
{
    REG32_IndW(pPidFilter, (REG32_IndR(pPidFilter) & ~TSP_PIDFLT_IN_MASK) | (u32FltSource & TSP_PIDFLT_IN_MASK));
}

U32  MHal_TSP_PidFlt_GetFltSource(REG_PidFlt *pPidFilter)
{
    return (REG32_IndR(pPidFilter) & TSP_PIDFLT_IN_MASK);
}

U32 MHal_TSP_SecFlt_GetState(REG_SecFlt *pSecFilter)
{
    return ((REG32_IndR(&pSecFilter->Ctrl) & TSP_SECFLT_STATE_MASK) >> TSP_SECFLT_STATE_SHFT);
}

void MHal_TSP_SecFlt_SetParam(REG_SecFlt *pSecFilter, U32 u32SecFltParam)
{
    REG32_IndW(&pSecFilter->Ctrl, (REG32_IndR(&pSecFilter->Ctrl) & ~TSP_SECFLT_PARAM_MASK) | ((u32SecFltParam << TSP_SECFLT_PARAM_SHFT) & TSP_SECFLT_PARAM_MASK));
}

U32 MHal_TSP_SecFlt_GetCtrl(REG_SecFlt *pSecFilter)
{
    return REG32_IndR(&pSecFilter->Ctrl);
}

void MHal_TSP_SecFlt_ClrCtrl(REG_SecFlt *pSecFilter)
{
    REG32_IndW(&pSecFilter->Ctrl, 0);
}

U32 MHal_TSP_SecFlt_GetMode(REG_SecFlt *pSecFilter)
{
    return ((REG32_IndR(&pSecFilter->Ctrl) & TSP_SECFLT_MODE_MASK) >> TSP_SECFLT_MODE_SHFT);
}

void MHal_TSP_SecFlt_SetMode(REG_SecFlt *pSecFilter, U32 u32SecFltMode)
{
    REG32_IndW(&pSecFilter->Ctrl, (REG32_IndR(&pSecFilter->Ctrl) & ~TSP_SECFLT_MODE_MASK) | ((u32SecFltMode << TSP_SECFLT_MODE_SHFT) & TSP_SECFLT_MODE_MASK));
}

void MHal_TSP_SecFlt_SetRmnCount(REG_SecFlt *pSecFilter, U32 u32RmnCount)
{
    REG32_IndW(&pSecFilter->RmnReqCnt, (REG32_IndR(&pSecFilter->RmnReqCnt) & ~TSP_SECFLT_RMNCNT_MASK) |
                                    ((u32RmnCount << TSP_SECFLT_RMNCNT_SHFT) & TSP_SECFLT_RMNCNT_MASK));
}

void MHal_TSP_SecFlt_SetReqCount(REG_SecFlt *pSecFilter, U32 u32ReqCount)
{
    REG32_IndW(&pSecFilter->RmnReqCnt, (REG32_IndR(&pSecFilter->RmnReqCnt) & ~TSP_SECFLT_REQCNT_MASK) |
                                    ((u32ReqCount << TSP_SECFLT_REQCNT_SHFT) & TSP_SECFLT_REQCNT_MASK));
}

U32  MHal_TSP_SecFlt_GetCRC32(REG_SecFlt *pSecFilter)
{
    return REG32_IndR(&pSecFilter->CRC32);
}

void MHal_TSP_SecFlt_ResetState(REG_SecFlt* pSecFilter)
{
    REG32_IndW(&pSecFilter->Ctrl, REG32_IndR(&pSecFilter->Ctrl) & ~(TSP_SECFLT_STATE_MASK));
}

void MHal_TSP_SecFlt_SetBuffer(REG_SecFlt *pSecFlt, U32 u32StartAddr, U32 u32BufSize)
{
    REG32_IndW(&pSecFlt->BufStart, u32StartAddr);
    REG32_IndW(&pSecFlt->BufEnd, u32StartAddr + u32BufSize);
}

void MHal_TSP_SecFlt_ResetBuffer(REG_SecFlt *pSecFlt)
{
    U32      u32BufStart= _TSP_SEC_FLT_ADDR_START(pSecFlt);
    REG32_IndW(&pSecFlt->BufCur, u32BufStart);
    REG32_IndW(&pSecFlt->BufRead, u32BufStart);
    REG32_IndW(&pSecFlt->BufWrite, u32BufStart);
}

void MHal_TSP_SecFlt_SetBufRead(REG_SecFlt *pSecFlt, U32 u32ReadAddr)
{
    REG32_IndW(&pSecFlt->BufRead, u32ReadAddr);
}

U32  MHal_TSP_SecFlt_GetBufStart(REG_SecFlt *pSecFlt)
{
    return _TSP_SEC_FLT_ADDR_START(pSecFlt);
}

U32  MHal_TSP_SecFlt_GetBufEnd(REG_SecFlt *pSecFlt)
{
    return REG32_IndR(&pSecFlt->BufEnd);
}

U32  MHal_TSP_SecFlt_GetBufRead(REG_SecFlt *pSecFlt)
{
    return REG32_IndR(&pSecFlt->BufRead);
}

U32  MHal_TSP_SecFlt_GetBufWrite(REG_SecFlt *pSecFlt)
{
    return REG32_IndR(&pSecFlt->BufWrite);
}

U32  MHal_TSP_SecFlt_GetBufCur(REG_SecFlt *pSecFlt)
{
    return REG32_IndR(&pSecFlt->BufCur);
}
void MHal_TSP_PVR_WaitFlush(void)
{
    REG32_W(&_TspCtrl[0].Hw_Config4,
            SET_FLAG1(REG32_R(&_TspCtrl[0].Hw_Config4), TSP_HW_CFG4_PVR_FLUSH));
    REG32_W(&_TspCtrl[0].Hw_Config4,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].Hw_Config4), TSP_HW_CFG4_PVR_FLUSH));
}

void MHal_TSP_PVR_Reset(void)
{
    // Richard: @FIXME:
    // Don't know PVR "reset" definition. call flush instead.
    MHal_TSP_PVR_WaitFlush();
}

U32  MHal_TSP_PVR_GetBufWrite(void)
{
    U32 u32WPtr = REG32_R(&_TspCtrl[0].TsRec_WPtr);

    return ((u32WPtr&0xFFFFFF)<<TSP_MIU_BASE_OFFSET);
}

//[HW LIMIT][HW TODO] TsDma pause can not be access by TSP CPU
//[HW LIMIT][HW TODO] TsDma pause it hard to control because read/write in different register
//[HW LIMIT][HW TODO] When setting TsDma it should be disable interrupt
void MHal_TSP_TsDma_SetDelay(U32 u32Delay)
{
    // Richard: the file in timer in Uranus is 24 bits.
    //          to simplify the process, writing 32 bits directly.
    //          HW will truncate the high 8 bits out, and use low 24 bits only (from Albert Lin)
    REG32_W(&_TspCtrl[0].TsFileIn_Timer, u32Delay);
}

void MHal_TSP_TsDma_Pause(void)
{
    REG32_W(&_TspCtrl[0].HwInt_Stat_Ctrl1,
            SET_FLAG1(REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1), TSP_CTRL1_FILEIN_PAUSE));
}

void MHal_TSP_TsDma_Resume(void)
{
    REG32_W(&_TspCtrl[0].HwInt_Stat_Ctrl1,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1), TSP_CTRL1_FILEIN_PAUSE));
}

void MHal_TSP_CmdQ_TsDma_SetAddr(U32 u32StreamAddr)
{
    REG32_W(&_TspCtrl[0].TsDma_Addr, u32StreamAddr);
}

U32 MHal_TSP_CmdQ_TsDma_GetReadPtr(void)
{
    return (REG32_R(&_TspCtrl[0].TsFileIn_RPtr))<<TSP_MIU_BASE_OFFSET;
}

void MHal_TSP_CmdQ_TsDma_SetSize(U32 u32StreamSize)
{
    REG32_W(&_TspCtrl[0].TsDma_Size, u32StreamSize);
}

void MHal_TSP_CmdQ_TsDma_Reset(void)
{
    REG32_W(&_TspCtrl[0].TsDma_Ctrl_CmdQ, 0);
}

void MHal_TSP_CmdQ_TsDma_Start(U32 u32TsDmaCtrl)
{
    // enable filein byte timer
    REG32_W(&_TspCtrl[0].reg15b4,
            SET_FLAG1(REG32_R(&_TspCtrl[0].reg15b4), TSP_FILEIN_TIMER_BYTE_ENABLE));
    REG32_W(&_TspCtrl[0].TsDma_Ctrl_CmdQ, TSP_TSDMA_CTRL_START | u32TsDmaCtrl);
}

U32 MHal_TSP_CmdQ_TsDma_GetState(void)
{
    return (HAS_FLAG(REG32_R(&_TspCtrl[0].TsDma_Ctrl_CmdQ), TSP_TSDMA_CTRL_START) |
            HAS_FLAG(REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1), TSP_CTRL1_FILEIN_PAUSE));
}

void MHal_TSP_CmdQ_SetSTC(U32 u32EngId, U32 u32STC)
{
    //REG32_W(&_TspCtrl[0].Pcr.ML, u32STC);
    // address
    REG16_T(ADDR_INDR_ADDR0)=   ((((U32)0x0244)) & 0xFFFF);
    REG16_T(ADDR_INDR_ADDR1)=   ((((U32)0x0021)) & 0xFFFF);
    // write value
    REG16_T(ADDR_INDR_WRITE0)=  (u32STC & 0xFFFF);
    REG16_T(ADDR_INDR_WRITE1)=  ((u32STC >> 16) & 0xFFFF);
    // set command
    REG16_T(ADDR_INDR_CTRL)=    (TSP_IDR_MCUWAIT | TSP_IDR_WRITE | TSP_IDR_START);
}

void MHal_TSP_CmdQ_SetSTC_32(U32 u32EngId, U32 u32STC_32)
{
    //REG16_W(&_TspCtrl[0].Pcr.H32, u32STC_32);
    // address
    REG16_T(ADDR_INDR_ADDR0)=   ((((U32)0x0248)) & 0xFFFF);
    REG16_T(ADDR_INDR_ADDR1)=   ((((U32)0x0021)) & 0xFFFF);
    // write value
    REG16_T(ADDR_INDR_WRITE0)=  (u32STC_32 & 0xFFFF);
    REG16_T(ADDR_INDR_WRITE1)=  ((u32STC_32 >> 16) & 0xFFFF);
    // set command
    REG16_T(ADDR_INDR_CTRL)=    (TSP_IDR_MCUWAIT | TSP_IDR_WRITE | TSP_IDR_START);
}

B16 MHal_TSP_CmdQ_IsEmpty(void)
{
    if ((REG32_R(&_TspCtrl[0].TsDma_Ctrl_CmdQ) & TSP_CMDQ_EMPTY) ||
            (0 == REG32_R(&_TspCtrl[0].TsDma_Ctrl_CmdQ)))    // Just initied
    {
        return TRUE;
    }
    return FALSE;
}

U32 MHal_TSP_CmdQ_EmptyCount(void)
{
    return (TSP_CMDQ_SIZE- (((REG32_R(&_TspCtrl[0].TsDma_Ctrl_CmdQ) & TSP_CMDQ_CNT_MASK) >> TSP_CMDQ_CNT_SHFT)));
}

U32 MHal_TSP_CmdQ_CmdCount(void)
{
    return ((REG32_R(&_TspCtrl[0].TsDma_Ctrl_CmdQ) & TSP_CMDQ_CNT_MASK) >> TSP_CMDQ_CNT_SHFT);
}

U32 MHal_TSP_CmdQ_GetWriteLevel(void)
{
    return ((REG32_R(&_TspCtrl[0].TsDma_Ctrl_CmdQ) & TSP_CMDQ_WR_LEVEL_MASK) >> TSP_CMDQ_WR_LEVEL_SHFT);
}

void MHal_TSP_CmdQ_ClearQ(void)
{
    REG32_W(&_TspCtrl[0].HwInt_Stat_Ctrl1,
            SET_FLAG1(REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1), TSP_CTRL1_CMDQ_RESET));
    REG32_W(&_TspCtrl[0].HwInt_Stat_Ctrl1,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1), TSP_CTRL1_CMDQ_RESET));
}

U32 MHal_TSP_GetSTC(U32 u32EngId)
{
    //return (REG32_R(&_TspCtrl[0].Pcr.ML));
    // set address
    REG16_T(ADDR_INDR_ADDR0)=   ((((U32)0x0244)) & 0xFFFF);
    REG16_T(ADDR_INDR_ADDR1)=   ((((U32)0x0021)) & 0xFFFF);
    // set command
    REG16_T(ADDR_INDR_CTRL)=    (TSP_IDR_MCUWAIT | TSP_IDR_READ | TSP_IDR_START);
    // get read value
    return ((U32)(REG16_T(ADDR_INDR_READ0))| ((U32)(REG16_T(ADDR_INDR_READ1)<< 16)));
}

U32  MHal_TSP_GetSTC_32(U32 u32EngId)
{
    //return ((REG16_R(&_TspCtrl[0].Pcr.H32)) & 1);
    // set address
    REG16_T(ADDR_INDR_ADDR0)=   ((((U32)0x0248)) & 0xFFFF);
    REG16_T(ADDR_INDR_ADDR1)=   ((((U32)0x0021)) & 0xFFFF);
    // set command
    REG16_T(ADDR_INDR_CTRL)=    (TSP_IDR_MCUWAIT | TSP_IDR_READ | TSP_IDR_START);
    // get read value
    return ((U32)(REG16_T(ADDR_INDR_READ0))| ((U32)(REG16_T(ADDR_INDR_READ1)<< 16))) & 0x1;
}

void MHal_TSP_SetFwMsg(U32 u32Mode)
{
    REG32_W(&_TspCtrl[0].Mcu_Msg, u32Mode);
}

void MHal_TSP_SelAudOut(U32 u32EngId)
{
    // Richard: There is only one TSP engine in Uranus. This function shall do nothing.
}

void MHal_TSP_PSAudioEnable(U32 u32EngId)
{
    REG32_W(&_TspCtrl[0].Hw_Config4,
            SET_FLAG1(REG32_R(&_TspCtrl[0].Hw_Config4), TSP_HW_CFG4_PS_AUD_EN));
}

void MHal_TSP_PSAudioDisable(U32 u32EngId)
{
    REG32_W(&_TspCtrl[0].Hw_Config4,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].Hw_Config4), TSP_HW_CFG4_PS_AUD_EN));
}

void MHal_TSP_PSVideoEnable(U32 u32EngId)
{
    REG32_W(&_TspCtrl[0].Hw_Config4,
            SET_FLAG1(REG32_R(&_TspCtrl[0].Hw_Config4), TSP_HW_CFG4_PS_VID_EN));
}

void MHal_TSP_PSVideoDisable(U32 u32EngId)
{
    REG32_W(&_TspCtrl[0].Hw_Config4,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].Hw_Config4), TSP_HW_CFG4_PS_VID_EN));
}

U32 MHal_TSP_GetFwMsg(void)
{
    return (REG32_R(&_TspCtrl[0].Mcu_Msg));
}

U32 MHal_TSP_GetCtrlMode(U32 u32EngId)
{
    return (REG32_R(&_TspCtrl[u32EngId].TSP_Ctrl));
}

void MHal_TSP_Int_Enable(U32 u32Mask)
{
    REG32_W(&_TspCtrl[0].HwInt_Stat_Ctrl1,
            SET_FLAG1(REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1), u32Mask>>8));
}

void MHal_TSP_Int_Disable(U32 u32Mask)
{
    REG32_W(&_TspCtrl[0].HwInt_Stat_Ctrl1,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1), u32Mask>>8));
}

void MHal_TSP_Int_ClearSw(void)
{
    REG32_W(&_TspCtrl[0].SwInt_Stat, 0);
}

void MHal_TSP_Int_ClearHw(U32 u32Mask)
{
    REG32_W(&_TspCtrl[0].HwInt_Stat_Ctrl1,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].HwInt_Stat_Ctrl1), u32Mask));
}

void MHal_TSP_Ind_Enable(void)
{
    REG32_W(&_TspCtrl[0].TSP_Ctrl,
            SET_FLAG1(REG32_R(&_TspCtrl[0].TSP_Ctrl), TSP_CTRL_SW_RST));
}

B16 MHal_TSP_Scmb_Status(B16 bTsin, U32 u32PidFltId)
{
    U32              u32ScmbMask;

    u32ScmbMask=        (0xFFFFFFFF== u32PidFltId)?
                        0xFFFFFFFF:
                        (1<< (u32PidFltId));
    if (bTsin)
        return (0!= HAS_FLAG(REG32_R(&_TspCtrl[0].TsPidScmbStatTsin), u32ScmbMask));
    else
        return (0!= HAS_FLAG(REG32_R(&_TspCtrl[0].TsPidScmbStatFile), u32ScmbMask));
}

void MHal_TSP_VarClk_Enable(void)
{
    U32 u32Flags = REG32_R(&_TspCtrl[0].Hw_Config4);

    SET_FLAG(u32Flags,   (TSP_HW_CFG4_SYNC_FALLING | TSP_HW_CFG4_SYNC_RISING));
    REG32_W(&_TspCtrl[0].Hw_Config4, u32Flags);
}

void MHal_TSP_VarClk_Disable(void)
{
    U32 u32Flags = REG32_R(&_TspCtrl[0].Hw_Config4);

    RESET_FLAG(u32Flags,   (TSP_HW_CFG4_SYNC_FALLING | TSP_HW_CFG4_SYNC_RISING));
    REG32_W(&_TspCtrl[0].Hw_Config4, u32Flags);
}

void MHal_TSP_OneBitSync_Enable(void)
{
    REG32_W(&_TspCtrl[0].reg15b4,
        SET_FLAG1(REG32_R(&_TspCtrl[0].reg15b4), TSP_SERIAL_SYNC_1T));
}

void MHal_TSP_OneBitSync_Disable(void)
{
    REG32_W(&_TspCtrl[0].reg15b4,
        RESET_FLAG1(REG32_R(&_TspCtrl[0].reg15b4), TSP_SERIAL_SYNC_1T));
}

void MHal_TSP_CSA_Set_ESA_Mode(U32 u32Mode)
{
    REG16_W(&_TspCsaCtrl[0].ESA_Mode, u32Mode);
}

void MHal_TSP_CSA_KL_Set_ACPU_CMD(U32 u32ACPU_cmd)
{
    REG16_W(&_TspCsaCtrl[0].KL_ACPU_Cmd, u32ACPU_cmd);
}

void MHal_TSP_CSA_KL_Start_ACPU_CMD(void)
{
    REG16_W(&_TspCsaCtrl[0].KL_ACPU_Status, TSP_CSA_START_ACPU_CMD);
}

void MHal_TSP_CSA_Sel_ScrmEng(U32 u32ScrmEng)
{
    REG32_W(&_TspCsaCtrl[0].ScmbEngSel, u32ScrmEng);
}

void MHal_TSP_CSA_Set_ScrmPath(U32 u32ScrmPath, B16 bEnableOutputAV)
{
    if (bEnableOutputAV)
    {
        REG16_W(&_TspCtrl[0].CA_CTRL, (u32ScrmPath | TSP_CA_AVPAUSE));
    }
    else
    {
        REG16_W(&_TspCtrl[0].CA_CTRL, u32ScrmPath);
    }
}

void MHal_TSP_CSA_KTE_Set(U8 Valid, U32 u32Tsc, U8 PfIdx, U8 RWPos, U32 Key)
{
    U16 u16WriteValue;
    U32 u32WData = 0;

    u16WriteValue = (Valid<<0xE)|(u32Tsc<<0xC)|(PfIdx<<8)|(RWPos<<4)|0x0001;
    MHal_TSP_CSA_KL_Set_ACPU_CMD((U32)u16WriteValue);
    u32WData |= ((Key >> 24) & 0x000000FF);
    u32WData |= ((Key >>  8) & 0x0000FF00);
    u32WData |= ((Key <<  8) & 0x00FF0000);
    u32WData |= ((Key << 24) & 0xFF000000);
    REG32_W(&_TspCsaCtrl->KL_ACPU_WADDR, u32WData);
    MHal_TSP_CSA_KL_Start_ACPU_CMD();
}

//------------------------------------------------------------------------------
// For MISC part
//------------------------------------------------------------------------------
void MHal_TSP_DoubleBuf_En(B16 bBuf_Sel)
{
    if (bBuf_Sel) //TSIN->PP, Filein->Single
    {
        REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) | TSP_DOUBLE_BUF_EN);
        REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) & ~TSP_DOUBLE_BUF_SWITCH);
    }
    else //TSIN->Single, File->Double
    {
        REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) & ~TSP_DOUBLE_BUF_EN);
        REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) | TSP_DOUBLE_BUF_SWITCH);
    }
    REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) | TSP_DOUBLE_BUF_DESC);
    REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) | TSP_SEC_BLOCK_DISABLE);
}

void MHal_TSP_DoubleBuf_Disable(void)
{
    REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) & ~TSP_DOUBLE_BUF_EN);
    REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) & ~TSP_DOUBLE_BUF_SWITCH);
    //REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) & ~TSP_DOUBLE_BUF_DESC);
    REG32_W(&_TspCtrl[0].reg160C, REG32_R(&_TspCtrl[0].reg160C) | TSP_DOUBLE_BUF_DESC);
}

void MHal_TSP_LiveAB_En(B16 bPF_EN)
{
    if (bPF_EN)
    {
        REG32_W(&_TspCtrl[0].PktChkSizeFilein, REG32_R(&_TspCtrl[0].PktChkSizeFilein) | TSP_LIVEAB_SEL);
    }
    else
    {
        REG32_W(&_TspCtrl[0].PktChkSizeFilein, REG32_R(&_TspCtrl[0].PktChkSizeFilein) & ~TSP_LIVEAB_SEL);
    }
}

void MHal_TSP_SetPktSize(U32 u32PKTSize)
{
    REG32_W(&_TspCtrl[0].Hw_Config4, SET_FLAG1(REG32_R(&_TspCtrl[0].Hw_Config4), TSP_HW_CFG4_ALT_TS_SIZE));
    REG32_W(&_TspCtrl[0].PktChkSizeFilein, (REG32_R(&_TspCtrl[0].PktChkSizeFilein)&~TSP_PKT_SIZE_MASK)|(TSP_PKT_SIZE_MASK&u32PKTSize));
}

/*
U32 MHal_TSP_Get_PktCount(void)
{
    return ((REG32_R(&_TspCtrl[0].PKT_CNT) & TSP_PKT_CNT_MASK));
}
*/

void MHal_TSP_PVR_TimeStampSetRecordStamp(U32 u32Stamp)
{
    REG32_W(&_TspCtrl[0].LPcr1, u32Stamp);

    REG32_W(&_TspCtrl[0].reg160C,
            SET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_LPCR_REC_WRITE));
    REG32_W(&_TspCtrl[0].reg160C,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_LPCR_REC_WRITE));
}

U32 MHal_TSP_PVR_TimeStampGetRecordStamp(void)
{
    return (REG32_R(&_TspCtrl[0].LPcr1));
}

void MHal_TSP_PVR_TimeStampSetPlaybackStamp(U32 u32Stamp)
{
    REG32_W(&_TspCtrl[0].LPcr2, u32Stamp);

    REG32_W(&_TspCtrl[0].reg160C,
            SET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_LPCR_PLAY_WRITE));
    REG32_W(&_TspCtrl[0].reg160C,
            RESET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_LPCR_PLAY_WRITE));
}

U32 MHal_TSP_PVR_TimeStampGetPlaybackStamp(void)
{
    return (REG32_R(&_TspCtrl[0].LPcr2));
}

void MHal_TSP_PVR_TimeStampEnable(B16 bEnable)
{
    if (bEnable)
    {
        REG32_W(&_TspCtrl[0].reg160C,
                SET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_PKT192_EN_FILEIN));
        REG32_W(&_TspCtrl[0].reg160C,
                SET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_TIMESTAMP_RESET));
        REG32_W(&_TspCtrl[0].reg160C,
                RESET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_TIMESTAMP_RESET));
        //REG32_W(&_TspCtrl[0].reg15b4,
        //        RESET_FLAG1(REG32_R(&_TspCtrl[0].reg15b4), TSP_FILEIN_TIMER_BYTE_ENABLE));
    }
    else
    {
        //REG32_W(&_TspCtrl[0].reg15b4,
        //        SET_FLAG1(REG32_R(&_TspCtrl[0].reg15b4), TSP_FILEIN_TIMER_BYTE_ENABLE));
        REG32_W(&_TspCtrl[0].reg160C,
                RESET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_PKT192_EN_FILEIN));
    }
}

void MHal_TSP_PVR_TimeStampRecord(B16 bEnable)
{
    if (bEnable)
    {
        REG32_W(&_TspCtrl[0].reg160C,
                SET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_PKT192_EN_PVR));
    }
    else
    {
        REG32_W(&_TspCtrl[0].reg160C,
                RESET_FLAG1(REG32_R(&_TspCtrl[0].reg160C), TSP_PKT192_EN_PVR));
    }
}

U32 MHal_TSP_PVR_TimeStampGetFileinPacket(void)
{
    U32 u32TimeStamp;
#if 1
    REG32_W(&_TspCtrl[0].Pkt_Cnt,
                SET_FLAG1(REG32_R(&_TspCtrl[0].Pkt_Cnt) & ~TSP_FILEIN_STAMP_MASK, TSP_FILEIN_STAMP_HI));

    u32TimeStamp = (REG32_R(&_TspCtrl[0]._xbf802ba8) & 0xFF) << TSP_DBG_BUS_SHFT;
    REG32_W(&_TspCtrl[0].Pkt_Cnt,
                SET_FLAG1(REG32_R(&_TspCtrl[0].Pkt_Cnt) & ~TSP_FILEIN_STAMP_MASK, TSP_FILEIN_STAMP_LOW));
    u32TimeStamp |= (REG32_R(&_TspCtrl[0]._xbf802ba8) & TSP_DBG_BUS_MASK);
    REG32_W(&_TspCtrl[0].Pkt_Cnt, REG32_R(&_TspCtrl[0].Pkt_Cnt) & ~TSP_FILEIN_STAMP_MASK);
#else
	u32TimeStamp = REG32_R(&_TspCtrl[0].TimeStamp_Filein);
#endif
    return u32TimeStamp;
}

U32 MHal_TSP_GetPcr(U32 u32EngId)
{
    //return (REG32_R(&_TspCtrl[0].Mcu_Msg));
    // set address
    REG16_T(ADDR_INDR_ADDR0)=   ((((U32)0x0330)) & 0xFFFF);
    REG16_T(ADDR_INDR_ADDR1)=   ((((U32)0x0021)) & 0xFFFF);
    // set command
    REG16_T(ADDR_INDR_CTRL)=    (TSP_IDR_MCUWAIT | TSP_IDR_READ | TSP_IDR_START);
    // get read value
    return ((U32)(REG16_T(ADDR_INDR_READ0))| ((U32)(REG16_T(ADDR_INDR_READ1)<< 16)));
}

U32  MHal_TSP_GetPcr_32(U32 u32EngId)
{
    //return ((REG32_R(&_TspCtrl[0].Mcu_Msg2)) & 1);
    // set address
    REG16_T(ADDR_INDR_ADDR0)=   ((((U32)0x033C)) & 0xFFFF);
    REG16_T(ADDR_INDR_ADDR1)=   ((((U32)0x0021)) & 0xFFFF);
    // set command
    REG16_T(ADDR_INDR_CTRL)=    (TSP_IDR_MCUWAIT | TSP_IDR_READ | TSP_IDR_START);
    // get read value
    return ((U32)(REG16_T(ADDR_INDR_READ0)) & 1);
}

#endif

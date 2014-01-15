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
/// @file   mhal_gflip.h
/// @brief  MStar GFLIP Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_GFLIP_H
#define _HAL_GFLIP_H

#ifdef _HAL_GFLIP_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines & Macros
//=============================================================================
#define MAX_GOP_SUPPORT         4
#define MAX_GOP0_GWIN            2
#define MAX_GOP1_GWIN            2
#define MAX_GOP2_GWIN            1
#define MAX_GOP3_GWIN            1
#define MAX_GOP_GWIN              MAX_GOP0_GWIN

#define GFLIP_GOP_BANKOFFSET    (0x3)
#define GFLIP_ADDR_ALIGN_RSHIFT (0x5)

#define GFLIP_GOP_IDX_2G             (0x0)
#define GFLIP_GOP_IDX_2GX            (0x1)
#define GFLIP_GOP_IDX_1G             (0x2)
#define GFLIP_GOP_IDX_1GX            (0x3)
#define GFLIP_GOP_IDX_DWIN           (0x4)
#define GFLIP_GOP_IDX_INVALID       (0xFFFFFFFF)

#define GFLIP_GOP_DWIN_BANKID     0xc

#define GFLIP_GOP_BANK_IDX_0     (0x0)
#define GFLIP_GOP_BANK_IDX_1     (0x1)

#define GFLIP_GOP_DST_IPSUB     0x0
#define GFLIP_GOP_DST_IPMAIN    0x1
#define GFLIP_GOP_DST_OP0      0x2
#define GFLIP_GOP_DST_VOP      0x3
#define GFLIP_GOP_DST_VOPSUB   0x4

#define GFLIP_FILM_DRIVER_VER     1

#define DLC_FUNCTION  1

#if DLC_FUNCTION
#define BYTE MS_U8
#define WORD MS_U16
#define DWORD MS_U32
#define XDATA
#define extern
#define printf printk
#define DLC_DEBUG(x)        //(x)

#define REG_ADDR_DLC_HANDSHAKE                 L_BK_DLC(0x04)
#define REG_ADDR_DLC_HISTOGRAM_MODE            H_BK_DLC(0x04)
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_L         L_BK_DLC(0x06)
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_H         H_BK_DLC(0x06)
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L       L_BK_DLC(0x07)
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H       H_BK_DLC(0x07)
#define REG_ADDR_HISTOGRAM_RANGE_ENABLE        L_BK_DLC(0x08)
#define REG_ADDR_BLE_UPPER_BOND                L_BK_DLC(0x09)
#define REG_ADDR_BLE_LOWER_BOND                H_BK_DLC(0x09)
#define REG_ADDR_WLE_UPPER_BOND                L_BK_DLC(0x0A)
#define REG_ADDR_WLE_LOWER_BOND                H_BK_DLC(0x0A)
#define REG_ADDR_MAIN_MAX_VALUE                L_BK_DLC(0x0B)
#define REG_ADDR_MAIN_MIN_VALUE                H_BK_DLC(0x0B)
#define REG_ADDR_SUB_MAX_VALUE                 L_BK_DLC(0x0C)
#define REG_ADDR_SUB_MIN_VALUE                 H_BK_DLC(0x0C)
#define REG_ADDR_MAIN_BLACK_SLOP_LSB_L         L_BK_DLC(0x0D)
#define REG_ADDR_MAIN_BLACK_SLOP_L             L_BK_DLC(0x10)
#define REG_ADDR_MAIN_BLACK_SLOP_H             H_BK_DLC(0x10)
#define REG_ADDR_HISTOGRAM_DATA_8              L_BK_DLC(0x28)
#define REG_ADDR_DLC_DATA_START_MAIN           L_BK_DLC(0x30)
#define REG_ADDR_DLC_DATA_START_SUB            L_BK_DLC(0x38)
#define REG_ADDR_HISTOGRAM_DATA_32             L_BK_DLC(0x40)
#define REG_ADDR_DLC_DATA_EXTEND_N0_MAIN       L_BK_DLC(0x76)
#define REG_ADDR_DLC_DATA_EXTEND_16_MAIN       L_BK_DLC(0x77)
#define REG_ADDR_DLC_DATA_LSB_START_MAIN       L_BK_DLC(0x78)
#define REG_ADDR_DLC_DATA_LSB_START_SUB        L_BK_DLC(0x7B)
#define REG_ADDR_DLC_DATA_EXTEND_N0_SUB        L_BK_DLC(0x7E)
#define REG_ADDR_DLC_DATA_EXTEND_16_SUB        L_BK_DLC(0x7F)

#define REG_ADDR_VOP_SCREEN_CONTROL           L_BK_VOP(0x19)

#define PRJ_MCNR

#define REG_LPLL_11_L                   (0x103100 | 0x11<< 1)
#define REG_LPLL_13_L                   (0x103100 | 0x13<< 1)

#define REG_SC_BK12_01_L                   (0x131200 | 0x01<< 1)

#define REG_SC_BK0A_02_L                   (0x130A00 | 0x02<< 1)
#define REG_SC_BK0A_05_L                   (0x130A00 | 0x05<< 1)
#define REG_SC_BK0A_06_L                   (0x130A00 | 0x06<< 1)
#define REG_SC_BK0A_07_L                   (0x130A00 | 0x07<< 1)
#define REG_SC_BK0A_0A_L                   (0x130A00 | 0x0A<< 1)
#define REG_SC_BK0A_0B_L                   (0x130A00 | 0x0B<< 1)
#define REG_SC_BK0A_0C_L                   (0x130A00 | 0x0C<< 1)
#define REG_SC_BK0A_0D_L                   (0x130A00 | 0x0D<< 1)
#define REG_SC_BK0A_0F_L                   (0x130A00 | 0x0F<< 1)

#define REG_SC_BK0A_10_L                   (0x130A00 | 0x10<< 1)
#define REG_SC_BK0A_15_L                   (0x130A00 | 0x15<< 1)
#define REG_SC_BK0A_16_L                   (0x130A00 | 0x16<< 1)
#define REG_SC_BK0A_17_L                   (0x130A00 | 0x17<< 1)
#define REG_SC_BK0A_1E_L                   (0x130A00 | 0x1E<< 1)
#define REG_SC_BK0A_1F_L                   (0x130A00 | 0x1F<< 1)

#define REG_SC_BK0A_20_L                   (0x130A00 | 0x20<< 1)
#define REG_SC_BK0A_21_L                   (0x130A00 | 0x21<< 1)
#define REG_SC_BK0A_23_L                   (0x130A00 | 0x23<< 1)
#define REG_SC_BK0A_25_L                   (0x130A00 | 0x25<< 1)
#define REG_SC_BK0A_41_L                   (0x130A00 | 0x41<< 1)
#define REG_SC_BK0A_4A_L                   (0x130A00 | 0x4A<< 1)
#define REG_SC_BK0A_4B_L                   (0x130A00 | 0x4B<< 1)

#define REG_SC_BK0A_57_L                   (0x130A00 | 0x57<< 1)
#define REG_SC_BK0A_5C_L                   (0x130A00 | 0x5C<< 1)
#define REG_SC_BK0A_5E_L                   (0x130A00 | 0x5E<< 1)

#define REG_SC_BK12_03_L                   (0x131200 | 0x03<< 1)
#define REG_SC_BK12_0F_L                   (0x131200 | 0x0F<< 1)
#define REG_SC_BK12_3A_L                   (0x131200 | 0x3A<< 1)
#define REG_SC_BK22_2A_L                   (0x132200 | 0x2A<< 1)
#define REG_SC_BK22_7A_L                   (0x132200 | 0x7A<< 1)
#define REG_SC_BK22_7C_L                   (0x132200 | 0x7C<< 1)

#define REG_SC_BK2A_29_L                   (0x132A00 | 0x29<< 1)
#define REG_SC_BK2A_2A_L                   (0x132A00 | 0x2A<< 1)
#define REG_SC_BK2A_2F_L                   (0x132A00 | 0x2F<< 1)
#define REG_SC_BK2A_7C_L                   (0x132A00 | 0x7C<< 1)
#define REG_SC_BK2A_7D_L                   (0x132A00 | 0x7D<< 1)
#define REG_SC_BK2A_7E_L                   (0x132A00 | 0x7E<< 1)

#define REG_SC_BK30_0C_L                   (0x133000 | 0x0C<< 1)
#define REG_SC_BK30_0D_L                   (0x133000 | 0x0D<< 1)

#define _BIT0			(0x0001)
#define _BIT1			(0x0002)
#define _BIT2			(0x0004)
#define _BIT3			(0x0008)
#define _BIT4			(0x0010)
#define _BIT5			(0x0020)
#define _BIT6			(0x0040)
#define _BIT7			(0x0080)

#define L_BK_VOP(_x_)                 (REG_SCALER_BASE | (REG_SC_BK_VOP << 8) | (_x_ << 1))
#define L_BK_DLC(_x_)                 (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | (_x_ << 1))
#define H_BK_DLC(_x_)                 (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | ((_x_ << 1)+1))

#define REG_SCALER_BASE             0x130000
#define REG_SC_BK_VOP               0x10
#define REG_SC_BK_DLC               0x1A

#define REG_ADDR(addr)              (*((volatile unsigned short int*)(0xFD000000 + (addr << 1))))

// read 2 byte
#define REG_RR(_reg_)                ({ REG_ADDR(_reg_);})

// write low byte
#define REG_WL(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & 0xFF00) | ((_val_) & 0x00FF); }while(0)

// write high byte
#define REG_WH(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_)  & 0x00FF) | ((_val_) << 8); }while(0)

 // write 2 byte
#define REG_W2B(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) =(_val_) ; }while(0)

#define MAIN_WINDOW                                0
#define SUB_WINDOW                                 1
#define msDlc_FunctionExit()
#define msDlc_FunctionEnter()

#define ENABLE_HISTOGRAM_8                          0

#define DLC_DEFLICK_BLEND_FACTOR                    32UL
#define XC_DLC_ALGORITHM_KERNEL                     2

#define XC_DLC_SET_DLC_CURVE_BOTH_SAME    0   // 0:Main and Sub use the same DLC curve
#endif
//=============================================================================
// HAL Driver Function
//=============================================================================

INTERFACE MS_BOOL MHal_GFLIP_IntEnable(MS_U32 u32GopIdx, MS_BOOL bEnable);
INTERFACE MS_BOOL MHal_GFLIP_IsVSyncInt(MS_U32 u32GopIdx);
INTERFACE MS_U32 MHal_GFLIP_GetIntGopIdx(void);
INTERFACE MS_BOOL MHal_GFLIP_SetFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32MainAddr, MS_U32 u32SubAddr);

INTERFACE MS_BOOL MHal_GFLIP_ClearDWINIRQ(GFLIP_DWININT_INFO *pGFlipDWinIntInfo);

INTERFACE MS_BOOL MHal_GFLIP_HandleVsyncLimitation(MS_U32 u32GopIdx);
INTERFACE MS_BOOL MHal_GFLIP_RestoreFromVsyncLimitation(MS_U32 u32GopIdx);

INTERFACE MS_BOOL MHal_GFLIP_IsTagIDBack(MS_U16 u16TagId);
INTERFACE MS_BOOL MHal_GFLIP_VECaptureEnable(MS_BOOL bEnable);
INTERFACE MS_BOOL MHal_GFLIP_CheckVEReady(void);
INTERFACE MS_U16 MHal_GFLIP_GetGopDst(MS_U32 u32GopIdx);
INTERFACE MS_U8 MHal_GFLIP_GetFrameIdx(void);
INTERFACE MS_U32 MHal_GFLIP_GetValidGWinIDPerGOPIdx(MS_U32 u32GopIdx, MS_U32 u32GwinIdx);
INTERFACE MS_U8 MHal_GFLIP_GetBankOffset(MS_U32 u32GopIdx, MS_U16 u16BankIdx);

INTERFACE void MApi_XC_W2BYTE(DWORD u32Reg, WORD u16Val );
INTERFACE WORD MApi_XC_R2BYTE(DWORD u32Reg );
INTERFACE void MApi_XC_W2BYTEMSK(DWORD u32Reg, WORD u16Val, WORD u16Mask );
INTERFACE WORD MApi_XC_R2BYTEMSK(DWORD u32Reg, WORD u16Mask );

#if DLC_FUNCTION

void msDlcHandler(BOOL bWindow);
BOOL msGetHistogramHandler(BOOL bWindow);
void msDlcWriteCurve(BOOL bWindow);
void msDlcWriteCurveLSB(BOOL bWindow, BYTE ucIndex, BYTE ucValue);
void msDlcLumiDiffCtrl(void);
void msDlcSetCurve(MS_U8 *_u8DlcCurveInit);
void msDlcSetBleSlopPoint(MS_U16 *_u16BLESlopPoint);
BOOL msDlcInit(MS_U16 *u16DlcInit );
void msDlcSetBleOnOff(BOOL bSwitch);
BOOL msDlcGetHistogram32Info(MS_U16 *pu16Histogram);

typedef struct
{
    // Default luma curve
    BYTE ucLumaCurve[16];
    BYTE ucLumaCurve2_a[16];
    BYTE ucLumaCurve2_b[16];

    BYTE ucDlcPureImageMode; // Compare difference of max and min bright
    BYTE ucDlcLevelLimit; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    BYTE ucDlcAvgDelta; // n = 0 ~ 50, default value: 12
    BYTE ucDlcAvgDeltaStill; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15 => 0.1 ~ 1.5 enable still curve
    BYTE ucDlcFastAlphaBlending; // min 17 ~ max 32
    BYTE ucDlcSlowEvent; // some event is triggered, DLC must do slowly // for PIP On/Off, msMultiPic.c
    BYTE ucDlcTimeOut; // for IsrApp.c
    BYTE ucDlcFlickAlphaStart; // for force to do fast DLC in a while
    BYTE ucDlcYAvgThresholdH; // default value: 128
    BYTE ucDlcYAvgThresholdL; // default value: 0
    BYTE ucDlcBLEPoint; // n = 24 ~ 64, default value: 48
    BYTE ucDlcWLEPoint; // n = 24 ~ 64, default value: 48
    BYTE bCGCCGainCtrl : 1; // 1: enable; 0: disable
    BYTE bEnableBLE : 1; // 1: enable; 0: disable
    BYTE bEnableWLE : 1; // 1: enable; 0: disable
    BYTE ucDlcYAvgThresholdM;
    BYTE ucDlcCurveMode;
    BYTE ucDlcCurveModeMixAlpha;
    BYTE ucDlcAlgorithmMode;
    BYTE ucDlcHistogramLimitCurve[17];
    BYTE ucDlcSepPointH;
    BYTE ucDlcSepPointL;
    WORD uwDlcBleStartPointTH;
    WORD uwDlcBleEndPointTH;
    BYTE ucDlcCurveDiff_L_TH;
    BYTE ucDlcCurveDiff_H_TH;
    WORD uwDlcBLESlopPoint_1;
    WORD uwDlcBLESlopPoint_2;
    WORD uwDlcBLESlopPoint_3;
    WORD uwDlcBLESlopPoint_4;
    WORD uwDlcBLESlopPoint_5;
    WORD uwDlcDark_BLE_Slop_Min;
    BYTE ucDlcCurveDiffCoringTH;
    BYTE ucDlcAlphaBlendingMin;
    BYTE ucDlcAlphaBlendingMax;
    BYTE ucDlcFlicker_alpha;
    BYTE ucDlcYAVG_L_TH;
    BYTE ucDlcYAVG_H_TH;
    BYTE ucDlcDiffBase_L;
    BYTE ucDlcDiffBase_M;
    BYTE ucDlcDiffBase_H;
}StuDlc_FinetuneParamaters;

#endif

#undef INTERFACE
#endif //_HAL_GFLIP_H


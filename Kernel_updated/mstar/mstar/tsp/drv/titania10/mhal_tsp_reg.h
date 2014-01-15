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

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File name: regTSP.h
//  Description: Transport Stream Processor (TSP) Register Definition
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TSP_REG_MCU_H_
#define _TSP_REG_MCU_H_

#include "mdrv_types.h"

//--------------------------------------------------------------------------------------------------
//  Abbreviation
//--------------------------------------------------------------------------------------------------
// Addr                             Address
// Buf                              Buffer
// Clr                              Clear
// CmdQ                             Command queue
// Cnt                              Count
// Ctrl                             Control
// Flt                              Filter
// Hw                               Hardware
// Int                              Interrupt
// Len                              Length
// Ovfw                             Overflow
// Pkt                              Packet
// Rec                              Record
// Recv                             Receive
// Rmn                              Remain
// Reg                              Register
// Req                              Request
// Rst                              Reset
// Scmb                             Scramble
// Sec                              Section
// Stat                             Status
// Sw                               Software
// Ts                               Transport Stream


//--------------------------------------------------------------------------------------------------
//  Global Definition
//--------------------------------------------------------------------------------------------------
// #define TS_PACKET_SIZE              188


//--------------------------------------------------------------------------------------------------
//  Compliation Option
//--------------------------------------------------------------------------------------------------

//[CMODEL][FWTSP]
// When enable, interrupt will not lost, CModel will block next packet
// and FwTSP will block until interrupt status is clear by MIPS.
// (For firmware and cmodel only)
#define TSP_DBG_SAFE_MODE_ENABLE        0

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

// Preprocessor warning notification
#if !defined(TSP_MCU_REGMAP)
#error "Please chose a TSP register mapping";
#endif

// Software
#ifdef TSP_MCU_REGMAP

    #define REG_PIDFLT_BASE         (0x00210000 << 1)                   // Fit the size of REG32
    #define REG_SECFLT_BASE         (0x00211000 << 1)                   // Fix the size of REG32

    #define REG_CTRL_BASE           (0xBF202A00)                        // 0xBF200000+(0x1500/2)*4
    #define REG_CSACTRL_BASE        (0xBF201800)                        // 0xBF200000+(0xC00/2)*4
    typedef struct _REG32
    {
        volatile U32                L;
        volatile U32                H;
    } REG32;

    typedef struct _REG16
    {
        volatile U32                data;
        volatile U32                _resv;
    } REG16;
#endif


typedef REG32                       REG_PidFlt;

// PID
#define TSP_PIDFLT_PID_MASK         0x00001FFF
#define TSP_PIDFLT_PID_SHFT         0

// Section filter Id
#define TSP_PIDFLT_SECFLT_MASK      0x001F0000                          // [20:16] secflt id
#define TSP_PIDFLT_SECFLT_SHFT      16
#define TSP_PIDFLT_SECFLT_NULL      0x1F                                // software usage

// AF/Sec/Video/Audio/Audio-second
#define TSP_PIDFLT_OUT_MASK         0x01e02000
#define TSP_PIDFLT_OUT_SECFLT_AF    0x00002000
#define TSP_PIDFLT_OUT_NONE         0x00000000
#define TSP_PIDFLT_OUT_SECFLT       0x00200000
#define TSP_PIDFLT_OUT_VFIFO        0x00400000
#define TSP_PIDFLT_OUT_AFIFO        0x00800000
#define TSP_PIDFLT_OUT_AFIFO2       0x01000000

// File/Live
#define TSP_PIDFLT_IN_MASK          0x02000000
#define TSP_PIDFLT_IN_LIVE          0x00000000
#define TSP_PIDFLT_IN_FILE          0x02000000

// note, this bit is only useful for PVR pure pid
// use SEC/VIDEO/AUDIO flag is identical to PVR a certain PID
#define TSP_PIDFLT_PVR_ENABLE       0x04000000

#define REG16_T(addr)               (*((volatile U16*)(addr)))
#define ADDR_INDR_CTRL              0xbf202b20
#define ADDR_INDR_ADDR0             0xbf202b24
#define ADDR_INDR_ADDR1             0xbf202b28
#define ADDR_INDR_WRITE0            0xbf202b2c
#define ADDR_INDR_WRITE1            0xbf202b30
#define ADDR_INDR_READ0             0xbf202b34
#define ADDR_INDR_READ1             0xbf202b38

#define ADDR_PVR_HEAD20             0xbf202a04
#define ADDR_PVR_HEAD21             0xbf202a08
#define ADDR_PVR_MID20              0xbf202a0c
#define ADDR_PVR_MID21              0xbf202a10
#define ADDR_PVR_TAIL20             0xbf202a14
#define ADDR_PVR_TAIL21             0xbf202a18

#define INDR_READ_SAVE_MCU0         0xbf202c00
#define INDR_READ_SAVE_MCU1         0xbf202c04
#define INDR_READ_SAVE_OR           0x0021033c

#define _TSP_SEC_FLT_ADDR_START(pSecFilter) \
    (TSP_SECFLT_BUFSTART_MASK & REG32_IndR(&((pSecFilter)->BufStart)))

#define _TSP_QMEM_I_MASK            0xffffe000
#define _TSP_QMEM_I_ADDR_HIT        0x00000000
#define _TSP_QMEM_I_ADDR_MISS       0xffffffff
#define _TSP_QMEM_D_MASK            0xffffe000
#define _TSP_QMEM_D_ADDR_HIT        0x00002000
#define _TSP_QMEM_D_ADDR_MISS       0xffffffff
#define _TSP_QMEM_SIZE              0x3000

typedef struct _REG_SecFlt
{
    REG32                           Ctrl;
    // SW flag
    #define TSP_SECFLT_BUF_ENABLE                   0x00000008


    #define TSP_SECFLT_TYPE_MASK                    0x00000007
    #define TSP_SECFLT_TYPE_SHFT                    0
    #define TSP_SECFLT_TYPE_UNK                     0x0
    #define TSP_SECFLT_TYPE_SEC                     0x1
    #define TSP_SECFLT_TYPE_PES                     0x2
    #define TSP_SECFLT_TYPE_PKT                     0x3
    #define TSP_SECFLT_TYPE_PCR                     0x4
    #define TSP_SECFLT_TYPE_TTX                     0x5
    #define TSP_SECFLT_TYPE_OAD                     0x6

    #define TSP_SECFLT_MODE_MASK                    0x000000F0          // software implementation
    #define TSP_SECFLT_MODE_SHFT                    4
    #define TSP_SECFLT_MODE_CONTI                   0x0
    #define TSP_SECFLT_MODE_ONESHOT                 0x1
    #define TSP_SECFLT_MODE_CRCCHK                  0x2

#if 1 // [Titania remove] ??? still keep it ???
    //[NOTE] update section filter
    // It's not suggestion user update section filter control register
    // when filter is enable. There may be race condition. Be careful.
    #define TSP_SECFLT_STATE_MASK                   0x0F000000          // software implementation
    #define TSP_SECFLT_STATE_SHFT                   24
    #define TSP_SECFLT_STATE_OVERFLOW               0x1
    #define TSP_SECFLT_STATE_DISABLE                0x2
    #define TSP_SECFLT_PARAM_MASK                   0xF0000000
    #define TSP_SECFLT_PARAM_SHFT                   28
    #define TSP_SECFLT_PARAM_PCRRST                 0x1
#endif

    REG32                           Match[TSP_FILTER_DEPTH/sizeof(U32)];
    REG32                           Mask[TSP_FILTER_DEPTH/sizeof(U32)];
    REG32                           BufStart;
    #define TSP_SECFLT_BUFSTART_MASK                0x07FFFFF8
    REG32                           BufEnd;
    REG32                           BufRead;
    REG32                           BufWrite;
    REG32                           BufCur;

    REG32                           RmnReqCnt;
    #define TSP_SECFLT_REQCNT_MASK                  0xFFFF0000
    #define TSP_SECFLT_REQCNT_SHFT                  16
    #define TSP_SECFLT_RMNCNT_MASK                  0x0000FFFF
    #define TSP_SECFLT_RMNCNT_SHFT                  0


    REG32                           CRC32;
    REG32                           NMatch[TSP_FILTER_DEPTH/sizeof(U32)];
    REG32                           _x50[12]; // (0x210080-0x210050)/4
} REG_SecFlt;


typedef struct _REG_Stc
{
    REG32                           ML;
    REG16                           H32;
} REG_Stc;


typedef struct _REG_Pid
{                                                                       // Index(word)  CPU(byte)       Default
    REG_PidFlt                      Flt[TSP_PIDFLT_NUM_ALL];
} REG_Pid;


typedef struct _REG_Sec
{                                                                       // Index(word)  CPU(byte)       Default
    REG_SecFlt                      Flt[TSP_SECFLT_NUM];
} REG_Sec;


typedef struct _REG_Ctrl
{
    //----------------------------------------------
    // 0xBF202A00 MIPS direct access
    //----------------------------------------------
                                                                        // Index(word)  CPU(byte)     MIPS(0x1500/2+index)*4
    // only 24 bits supported in PVR address. 16 bytes address
    REG32                           TsRec_Head20;                       // 0xbf202a00   0x00
    #define TSP_HW_PVR_BUF_HEAD20_MASK              0xFFFF0000
    #define TSP_HW_PVR_BUF_HEAD20_SHFT              16
    REG32                           TsRec_Head21_Mid20;                 // 0xbf202b08   0x02
    #define TSP_HW_PVR_BUF_HEAD21_MASK              0x000000FF
    #define TSP_HW_PVR_BUF_HEAD21_SHFT              0
    #define TSP_HW_PVR_BUF_MID20_MASK               0xFFFF0000
    #define TSP_HW_PVR_BUF_MID20_SHFT               16
    REG32                           TsRec_Mid21_Tail20;                 // 0xbf202b10   0x04
    #define TSP_HW_PVR_BUF_MID21_MASK               0x000000FF
    #define TSP_HW_PVR_BUF_MID21_SHFT               8
    #define TSP_HW_PVR_BUF_TAIL20_MASK              0xFFFF0000
    #define TSP_HW_PVR_BUF_TAIL20_SHFT              16
    REG32                           TsRec_Tail2;                        // 0xbf202b18   0x06
    #define TSP_HW_PVR_BUF_TAIL21_MASK              0x000000FF
    #define TSP_HW_PVR_BUF_TAIL21_SHFT              0

    REG32                           _xbf802a20_xbf802a78[12];           // 0xbf202a20-- 0xbf202a78
    REG32                           Pkt_CacheW0;                        // 0xbf202a80   0x20
    REG32                           Pkt_CacheW1;                        // 0xbf202a88   0x22
    REG32                           Pkt_CacheW2;                        // 0xbf202a90   0x24
    REG32                           Pkt_CacheW3;                        // 0xbf202a98   0x26
    REG16                           Pkt_CacheIdx;                       // 0xbf202aa0   0x28
    REG32                           _xbf802aa8;                         // 0xbf202aa8   0x2a
    REG32                           Hw_Config0;                         // 0xbf202ab0   0x2c
    #define TSP_HW_CFG0_DATA_PORT_EN                0x00000001
    #define TSP_HW_CFG0_TSIFO_SERL                  0x00000000
    #define TSP_HW_CFG0_TSIF0_PARL                  0x00000002
    #define TSP_HW_CFG0_TSIF0_EXTSYNC               0x00000004
    #define TSP_HW_CFG0_TSIF0_TS_BYPASS             0x00000080
    #define TSP_HW_CFG0_TSIF0_VPID_BYPASS           0x00000010
    #define TSP_HW_CFG0_TSIF0_APID_BYPASS           0x00000020
    #define TSP_HW_CFG0_WB_DMA_RESET                0x00000040
    // [Titania]
    #define TSP_HW_CFG0_PACKET_SIZE_MASK            0xFF000000
    #define TSP_HW_CFG0_PACKET_SIZE_SHFT            24

    REG32                           _xbf802ab8;                         // 0xbf202ab8
    REG_Stc                         Pcr;                                // 0xbf202ac0   0x30 & 0x32
    // [Titania]
    REG32                           Pkt_Info;                           // 0xbf202ad0   0x34
    #define TSP_PKT_INFO_CC_MASK                    0x000F0000
    #define TSP_PKT_INFO_CC_SHFT                    16
    #define TSP_PKT_INFO_SCMB                       0x00C00000
    #define TSP_PKT_INFO_SCMB_SHFT                  22

    REG32                           _xbf802ad8;                         // 0xbf202ad8   0x36
    REG32                           SwInt_Stat;                         // 0xbf202ae0   0x38
    #define TSP_SWINT_INFO_SEC_MASK                 0x000000FF
    #define TSP_SWINT_INFO_SEC_SHFT                 0
    #define TSP_SWINT_INFO_ENG_MASK                 0x0000FF00
    #define TSP_SWINT_INFO_ENG_SHFT                 8
    #define TSP_SWINT_STATUS_CMD_MASK               0x7FFF0000
    #define TSP_SWINT_STATUS_CMD_SHFT               16
    #define TSP_SWINT_STATUS_SEC_RDY                0x0001
    #define TSP_SWINT_STATUS_REQ_RDY                0x0002
    #define TSP_SWINT_STATUS_BUF_OVFLOW             0x0006
    #define TSP_SWINT_STATUS_SEC_CRCERR             0x0007
    #define TSP_SWINT_STATUS_SEC_ERROR              0x0008
    #define TSP_SWINT_STATUS_PES_ERROR              0x0009
    #define TSP_SWINT_STATUS_SYNC_LOST              0x0010
    #define TSP_SWINT_STATUS_PKT_OVRUN              0x0020
    #define TSP_SWINT_STATUS_DEBUG                  0x0030
    #define TSP_SWINT_CMD_DMA_PAUSE                 0x0100
    #define TSP_SWINT_CMD_DMA_RESUME                0x0200

    #define TSP_SWINT_STATUS_SEC_GROUP              0x000F
    #define TSP_SWINT_STATUS_GROUP                  0x00FF
    #define TSP_SWINT_CMD_GROUP                     0x7F00
    #define TSP_SWINT_CMD_STC_UPD                   0x0400

    #define TSP_SWINT_CTRL_FIRE                     0x80000000



    // only 26 bits available for filein address
    REG32                           TsDma_Addr;                         // 0xbf202ae8   0x3a
    // only 24 bits available for filein length
    REG32                           TsDma_Size;                         // 0xbf202af0   0x3c
    REG32                           TsDma_Ctrl_CmdQ;                    // 0xbf202af8   0x3e
    // file in control
    #define TSP_TSDMA_CTRL_START                    0x00000001
    #define TSP_TSDMA_CTRL_VPES0                    0x00000004
    #define TSP_TSDMA_CTRL_APES0                    0x00000008
    #define TSP_TSDMA_CTRL_A2PES0                   0x00000010
    #define TSP_TSDMA_STAT_ABORT                    0x00000080
    // CmdQ
    #define TSP_CMDQ_CNT_MASK                       0x001F0000
    #define TSP_CMDQ_CNT_SHFT                       16
    #define TSP_CMDQ_FULL                           0x00400000
    #define TSP_CMDQ_EMPTY                          0x00800000
    #define TSP_CMDQ_SIZE                           16
    #define TSP_CMDQ_WR_LEVEL_MASK                  0x03000000
    #define TSP_CMDQ_WR_LEVEL_SHFT                  24

    REG32                           Mcu_Msg;                            // 0xbf202b00   0x40
    #define TSP_MSG_FW_STC_NOSYNC                   0x00000001
    #define TSP_MSG_FW_STC1_NOSYNC                  0x00000002          //[reserved]

    REG32                           Hw_Config2;                         // 0xbf202b08   0x42
    #define TSP_HW_CFG2_TSIF1_SERL                  0x00000000
    #define TSP_HW_CFG2_TSIF1_PARL                  0x01000000
    #define TSP_HW_CFG2_TSIF1_EXTSYNC               0x02000000
    #define TSP_HW_CFG2_TSIF1_TS_BYPASS             0x10000000
    #define TSP_HW_CFG2_TSIF1_VPID_BYPASS           0x20000000
    #define TSP_HW_CFG2_TSIF1_APID_BYPASS           0x40000000

    REG32                           Hw_Config4;                         // 0xbf202b10   0x44
    #define TSP_HW_CFG4_PVR_PIDFLT_SEC              0x00000001
    #define TSP_HW_CFG4_PVR_ENABLE                  0x00000002
    #define TSP_HW_CFG4_PVR_ENDIAN_BIG              0x00000004          // 1: record TS to MIU with big endian
                                                                        // 0: record TS to MIU with little endian
    #define TSP_HW_CFG4_TSIF1_ENABLE                0x00000008          // 1: enable ts interface 1 and vice versa
    #define TSP_HW_CFG4_PVR_FLUSH                   0x00000010          // 1: str2mi_wadr <- str2mi_miu_head
    #define TSP_HW_CFG4_PVR_PAUSE                   0x00000040
    #define TSP_HW_CFG4_TSIF0_ENABLE                0x00000100          // 1: enable ts interface 0 and vice versa
    #define TSP_HW_CFG4_SYNC_FALLING                0x00000200          // 1: Reset bit count when data valid signal of TS interface is low
    #define TSP_HW_CFG4_SYNC_RISING                 0x00000400          // 1: Reset bit count on the rising sync signal of TS interface
    #define TSP_HW_CFG4_WB_ECO                      0x00002000          // TSP WB ECO: TSP will lost data while MIU busy, this ECO was fixed for this

    #define TSP_HW_CFG4_BYTE_ADDR_DMA               0x000E0000          // prevent from byte enable bug
    #define TSP_HW_CFG4_ALT_TS_SIZE                 0x00010000          // enable TS packets in 204 mode
    #define TSP_HW_CFG4_PS_VID_EN                   0x00800000          // 1: enable video path in program stream mode
    #define TSP_HW_CFG4_PS_AUD_EN                   0x01000000          // 1: enable audio path in program stream mode
    #define TSP_HW_CFG4_PS_AUD2_EN                  0x02000000          // program stream audioB enable
    #define TSP_HW_CFG4_SEC_ERR_RM_EN               0x10000000          // 1: enable removing section error packets
    #define TSP_HW_CFG4_VPES_ERR_RM_EN              0x08000000          // 1: enable removing vpes error packets
    #define TSP_HW_CFG4_APES_ERR_RM_EN              0x04000000          // 1: enable removing apes error packets

    REG32                           NOEA_PC;                            // 0xbf202b18   0x46
    REG32                           Idr_Ctrl_Addr0;                     // 0xbf202b20   0x48
    #define TSP_IDR_START                           0x00000001
    #define TSP_IDR_READ                            0x00000000
    #define TSP_IDR_WRITE                           0x00000002
    #define TSP_IDR_MCUWAIT                         0x00000020
    #define TSP_IDR_ADDR_MASK0                      0xFFFF0000
    #define TSP_IDR_ADDR_SHFT0                      16
    REG32                           Idr_Addr1_Write0;                   // 0xbf202b28   0x4a
    #define TSP_IDR_ADDR_MASK1                      0x0000FFFF
    #define TSP_IDR_ADDR_SHFT1                      0
    #define TSP_IDR_WRITE_MASK0                     0xFFFF0000
    #define TSP_IDR_WRITE_SHFT0                     16
    REG32                           Idr_Write1_Read0;                   // 0xbf202b30   0x4c
    #define TSP_IDR_WRITE_MASK1                     0x0000FFFF
    #define TSP_IDR_WRITE_SHFT1                     0
    #define TSP_IDR_READ_MASK0                      0xFFFF0000
    #define TSP_IDR_READ_SHFT0                      16
    REG32                           Idr_Read1;                          // 0xbf202b38   0x4e
    #define TSP_IDR_READ_MASK1                      0x0000FFFF
    #define TSP_IDR_READ_SHFT1                      0

    // only 24 bits supported in PVR address. 16 bytes address
    REG32                           TsRec_Head;                         // 0xbf202b40   0x50
    REG32                           TsRec_Mid;                          // 0xbf202b48   0x52
    REG32                           TsRec_Tail;                         // 0xbf202b50   0x54
    REG32                           TsRec_WPtr;                         // 0xbf202b58   0x56
    REG32                           _xbf802b68;                         // 0xbf202b60   0x58
    REG32                           reg15b4;                            // 0xbf202b68   0x5a
    #define TSP_NEW_DMA_WARB                        0x00000001
    #define TSP_DMAW_PROTECT_EN                     0x00000002
    #define TSP_DMAW_ERR_CLEAR                      0x00000004
    #define TSP_PVR_TS_HEADER                       0x00000008
    #define TSP_PVR_FILEIN                          0x00000010
    #define TSP_PVR_ALL                             0x00000040
    #define TSP_NMATCH_DIS                          0x00000800
    #define TSP_PVR_INVERT                          0x00001000
    #define TSP_FILEIN_TIMER_BYTE_ENABLE            0x00008000

    #define TSP_PVR_PINGPONG                        0x00010000
    #define TSP_PVR_PID                             0x00020000
    #define TSP_DUPLICATE_PKT_SKIP                  0x00400000          // Duplicated packet skip -- ECO Done
    #define TSP_SERIAL_SYNC_1T                      0x04000000          // Set 1 to detect serial-in sync without 8-cycle mode
    #define TSP_PVR_BURST_LEN_MASK                  0x18000000
    #define TSP_PVR_BURST_LEN_SHIFT                 27
    #define TSP_PVR_BURST_LEN_EIGHT                 0x0
    #define TSP_PVR_BURST_LEN_FOUR                  0x1
    #define TSP_PVR_BURST_LEN_ONE                   0x3
    #define TSP_SCMB_FILE                           0x40000000
    #define TSP_SCMB_TSIN                           0x80000000

    REG32                           _xbf802b70;                         // 0xbf202b70   0x5c
    REG32                           _xbf802b78;                         // 0xbf202b78   0x5e

    REG32                           Cpu_Base;                           // 0xbf202b80   0x60
    REG32                           Qmem_Ibase;                         // 0xbf202b88   0x62
    REG32                           Qmem_Imask;                         // 0xbf202b90   0x64
    REG32                           Qmem_Dbase;                         // 0xbf202b98   0x66
    REG32                           Qmem_Dmask;                         // 0xbf202ba0   0x68
    REG32                           _xbf802ba8;                         // 0xbf202ba8   0x6a
    #define TSP_DBG_BUS_MASK                        0x00FFFFFF
    #define TSP_DBG_BUS_SHFT                        24

    REG32                           TsFileIn_WPtr;                      // 0xbf202bb0   0x6c
    REG32                           TsFileIn_RPtr;                      // 0xbf202bb8   0x6e
    REG32                           TsFileIn_Timer;                     // 0xbf202bc0   0x70
    REG32                           TsFileIn_Head;                      // 0xbf202bc8   0x72
    REG32                           TsFileIn_Mid;                       // 0xbf202bd0   0x74
    REG32                           TsFileIn_Tail;                      // 0xbf202bd8   0x76

    REG32                           Dnld_Ctrl;                          // 0xbf202be0   0x78, 0x79
    #define TSP_DNLD_ADDR_MASK                      0x0000FFFF
    #define TSP_DNLD_ADDR_SHFT                      0
    #define TSP_DNLD_NUM_MASK                       0xFFFF0000
    #define TSP_DNLD_NUM_SHFT                       16

    REG32                           TSP_Ctrl;                           // 0xbf202be8   0x7a
    #define TSP_CTRL_CPU_EN                         0x00000001
    #define TSP_CTRL_SW_RST                         0x00000002
    #define TSP_CTRL_DNLD_START                     0x00000004
    #define TSP_CTRL_DNLD_DONE                      0x00000008          // see 0x78 for related information
    #define TSP_CTRL_TSFILE_EN                      0x00000010
    #define TSP_CTRL_R_PRIO                         0x00000020
    #define TSP_CTRL_W_PRIO                         0x00000040
    #define TSP_CTRL_PAD_SHIFT                      7
    #define TSP_CTRL_MUX0_PAD0_SEL                  0x00000000
    #define TSP_CTRL_MUX0_PAD1_SEL                  0x00000080
    #define TSP_CTRL_ICACHE_EN                      0x00000100
    #define TSP_CTRL_MUX1_PAD0_SEL                  0x00000000
    #define TSP_CTRL_MUX1_PAD1_SEL                  0x00000200
    #define TSP_CTRL_CPU2MI_R_PRIO                  0x00000400
    #define TSP_CTRL_CPU2MI_W_PRIO                  0x00000800
    #define TSP_CTRL_I_EL                           0x00000000
    #define TSP_CTRL_I_BL                           0x00001000
    #define TSP_CTRL_D_EL                           0x00000000
    #define TSP_CTRL_D_BL                           0x00002000

    REG32                           Pkt_Cnt;                            // 0xbf802bf0   0x7c
    #define TSP_PKT_CNT_MASK                        0x000000FF
    #define TSP_FILEIN_STAMP_MASK                   0x00FF0000
    #define TSP_FILEIN_STAMP_HI                     0x00470000
    #define TSP_FILEIN_STAMP_LOW                    0x00480000

    REG32                           HwInt_Stat_Ctrl1;                   // 0xbf202bf8   0x7e
    #define TSP_HWINT_TSP_PVR_TAIL0_STATUS          0x0100
    #define TSP_HWINT_TSP_PVR_MID0_STATUS           0x0200
    #define TSP_HWINT_TSP_PVR_TAIL1_STATUS          0x0100
    #define TSP_HWINT_TSP_PVR_MID1_STATUS           0x0200
    #define TSP_HWINT_TSP_FILEIN_MID_INT_STATUS     0x0800
    #define TSP_HWINT_TSP_FILEIN_TAIL_INT_STATUS    0x1000
    #define TSP_HWINT_TSP_SW_INT_STATUS             0x2000

#if 0
    #define TSP_HWINT_TSP_FW_INT_STATUS             0x0100
    #define TSP_HWINT_TSP_PVR_TAIL0_STATUS          0x0200
    #define TSP_HWINT_TSP_PVR_MID0_STATUS           0x0400
    #define TSP_HWINT_TSP_PVR_TAIL1_STATUS          0x0800
    #define TSP_HWINT_TSP_PVR_MID1_STATUS           0x1000
    #define TSP_HWINT_TSP_FILEIN_MID_INT_STATUS     0x2000
    #define TSP_HWINT_TSP_FILEIN_TAIL_INT_STATUS    0x4000

    // @FIXME: should backward compatible to venus
    #define TSP_HWINT_PVRBUF_0_READY                TSP_HWINT_TSP_PVR_TAIL0_STATUS
    #define TSP_HWINT_PVRBUF_1_READY                TSP_HWINT_TSP_PVR_TAIL1_STATUS

    #define TSP_HWINT_HW_STATUS                     (TSP_HWINT_TSP_PVR_MID0_STATUS        |     \
                                                     TSP_HWINT_TSP_PVR_TAIL0_STATUS       |     \
                                                     TSP_HWINT_TSP_PVR_MID1_STATUS        |     \
                                                     TSP_HWINT_TSP_PVR_TAIL1_STATUS       |     \
                                                     TSP_HWINT_TSP_FILEIN_MID_INT_STATUS  |     \
                                                     TSP_HWINT_TSP_FILEIN_TAIL_INT_STATUS)

    #define TSP_HWINT_HW_MASK                       (TSP_HWINT_PVRBUF_0_READY| TSP_HWINT_PVRBUF_1_READY)
    #define TSP_HWINT_ALL                           (TSP_HWINT_HW_MASK | TSP_HWINT_TSP_SW_INT_STATUS| TSP_HWINT_TSP_FW_INT_STATUS)
#else
    //#define TSP_HWINT_HW_MASK                       (TSP_HWINT_PVRBUF_0_READY| TSP_HWINT_PVRBUF_1_READY)
    //#define TSP_HWINT_HW_MASK                       (TSP_HWINT_TSP_PVR_TAIL0_STATUS | TSP_HWINT_TSP_PVR_TAIL0_STATUS)
    #define TSP_HWINT_HW_MASK                       (TSP_HWINT_TSP_PVR_TAIL0_STATUS | TSP_HWINT_TSP_PVR_TAIL0_STATUS | TSP_HWINT_TSP_PVR_MID0_STATUS | TSP_HWINT_TSP_PVR_MID1_STATUS)
    #define TSP_HWINT_ALL                           (TSP_HWINT_HW_MASK | TSP_HWINT_TSP_SW_INT_STATUS)
#endif

    // 0x7f: TSP_CTRL1: hidden in HwInt_Stat
    #define TSP_CTRL1_FILEIN_TIMER_ENABLE           0x00010000
    #define TSP_CTRL1_FILEIN_PAUSE                  0x00040000
    #define TSP_CTRL1_FILE_CHECK_WP                 0x00080000
    #define TSP_CTRL1_STANDBY                       0x00800000
    #define TSP_CTRL1_INT2NOEA                      0x01000000
    #define TSP_CTRL1_FILEIN_ENABLE                 0x02000000
    #define TSP_CTRL1_CMDQ_RESET                    0x08000000
    #define TSP_CTRL1_PVR_CMD_QUEUE_ENABLE          0x20000000
    #define TSP_CTRL1_FILE_WP_LOAD                  0x40000000
    #define TSP_CTRL1_DMA_RST                       0x80000000
    //----------------------------------------------
    // 0xbf202C00 MIPS direct access
    //----------------------------------------------
    REG32                           Mcu_Msg2;                           // 0xbf202c00   0x00
    REG32                           LPcr1;                              // 0xbf202c08   0x02
    REG32                           LPcr2;                              // 0xbf202c10   0x04

    REG32                           reg160C;                            // 0xbf202c18   0x06
    #define TSP_LPCR_REC_WRITE                      0x00000001
    #define TSP_LPCR_REC_READ                       0x00000002
    #define TSP_LPCR_PLAY_WRITE                     0x00000004
    #define TSP_LPCR_PLAY_READ                      0x00000008
    #define TSP_DOUBLE_BUF_DISABLE                  0x0000000A          // disable double buffer
    #define TSP_PKT192_EN_PVR                       0x00000010
    #define TSP_PKT192_EN_FILEIN                    0x00000020
    #define TSP_DOUBLE_BUF_EN                       0x00000040          // tsin->pinpon filein->single
    #define TSP_SEC_BLOCK_DISABLE                   0x00000400          //
    #define TSP_DOUBLE_BUF_SWITCH                   0x00000800          // tsin->single filein->pinpon
    #define TSP_DOUBLE_BUF_DESC                     0x00004000          // 160d bit(6) remove buffer limitation
    #define TSP_TIMESTAMP_RESET                     0x00008000          // 160d bit(7)
    #define TSP_RM_DMA_Glitch                       0x00800000          // Fix sec_dma overflow glitch
    #define TSP_RESET_VFIFO                         0x01000000          // Reset VFIFO -- ECO Done
    #define TSP_RESET_AFIFO                         0x02000000          // Reset AFIFO -- ECO Done
    #define TSP_DMA_RADDR_19_22_MASK                0x000F0000          // Firmare DMA address bit[23:20]
    #define TSP_DMA_RADDR_19_22_SHFT                16

    REG32                           PktChkSizeFilein;                   // 0xbf202c20   0x08
    #define TSP_PKT_SIZE_MASK                       0x000000FF
    #define TSP_LIVEAB_SEL                          0x00010000          // switch tsif1 to filein

    REG32                           MCU_Data1;                          // 0xbf202c28   0x0a
    REG32                           TsPidScmbStatTsin;                  // 0xbf202c30   0x0c
    REG32                           TsPidScmbStatFile;                  // 0xbf202c38   0x0e
    REG32                           reg1620_reg1673[21];                // 0xbf202c40-0xbf202ce4   0x10-0x39 -reserved

    REG32                           DMAW1;                              // 0xbf202ce8   0x3a
    REG32                           DMAW2;                              // 0xbf202cf0   0x3c
    REG32                           ORZ_DMAW;                           // 0xbf202cf8   0x3e
    #define TSP_ORZ_DMAW_LBND                       0x0000ffff
    #define TSP_ORZ_DMAW_UBND                       0xffff0000
    #define TSP_ORZ_DMAW_UBND_SHIFT                 16

    REG16                           CA_CTRL;                            // 0xbf202d00   0x40
    #define TSP_CA_CTRL_MASK                        0x000000ff
    #define TSP_CA_INPUT_TSIF0_LIVEIN               0x00000001
    #define TSP_CA_INPUT_TSIF0_FILEIN               0x00000002
    #define TSP_CA_INPUT_TSIF1                      0x00000004
    #define TSP_CA_AVPAUSE                          0x00000008
    #define TSP_CA_OUTPUT_PLAY_LIVE                 0x00000010
    #define TSP_CA_OUTPUT_PLAY_FILE                 0x00000020
    #define TSP_CA_OUTPUT_REC                       0x00000040

    REG32                           reg1684_reg16DF[23];                // 0xbf202d08-0xbf202dbe   0x42-0x6f -reserved
    REG32                           TimeStamp_Filein;                   // 0xbf202dc0   0x70
    REG32                           reg16E4_reg16F7[5];                 // 0xbf202dc8-0xbf202dee   0x72-0x7b -reserved
    REG32                           Mcu_Msg3;                           // 0xbf202df0   0x7c

} REG_Ctrl;

typedef struct _REG_CsaCWACPU{
    REG32                           CWACPU01;
    REG32                           CWACPU23;
    REG32                           CWACPU45;
    REG32                           CWACPU67;
} REG_CsaCWACPU;

typedef struct _REG_CsaACPUKeyIn{
    REG32                           ACPUKey01;
    REG32                           ACPUKey23;
    REG32                           ACPUKey45;
    REG32                           ACPUKey67;
} REG_CsaACPUKeyIn;

typedef struct _REG_CsaCwKey{
    REG32                           CwKeyL;
    REG32                           CwKeyH;
} REG_CsaCwKey;

typedef struct _REG_Cw{
    REG_CsaCwKey                    CsaCw[2];
    #define TSP_CW_EVEN                             0x2
    #define TSP_CW_ODD                              0x3
} REG_Cw_EvenOdd;

typedef struct _REG_CsaCw{
    REG_Cw_EvenOdd                  CsaCwEO[TSP_CSACW_NUM];
} REG_CsaCw;

typedef struct _REG_CsaCtrl{
    REG16                           Csa_Ctrl;                           // 0xbf201800   0x00
    #define TSP_CSA_CONFORM                         0x0020
    #define TSP_CSA_SKIP_TEI_SCMB_STATUS            0x0080

    REG32                           ScmbStatusTS;                       // 0xbf201808   0x02
    REG32                           ScmbStatusPES;                      // 0xbf201810   0x04
    REG32                           FifoStatusCSA;                      // 0xbf201818   0x06
    #define TSP_CSA_FIFO_STATUS_MASK                0x0000FFFF

    REG32                           _0xbf201820;                        // 0xbf201820   0x08
    REG32                           ScmbEngSel;                         // 0xbf201828   0x0a
    #define TSP_CSA_SCRM_ENG_CSA2CIP                0x1045
    #define TSP_CSA_SCRM_ENG_CIP2CSA                0x1210
    #define TSP_CSA_SCRM_ENG_CSA                    0x0105
    #define TSP_CSA_SCRM_ENG_CIP                    0x0000

    REG32                           KL_Ctrl0;                           // 0xbf201830   0x0c
    #define TSP_CSA_KL_START                        0x00000001
    #define TSP_CSA_KL_CTRL0_SWRST                  0x00000080
    #define TSP_CSA_KL_SRC_ACPU                     0x00000000
    #define TSP_CSA_KL_SRC_SCRM                     0x00000100
    #define TSP_CSA_KL_SRC_CRYPTO                   0x00000200
    #define TSP_CSA_KL_SRC_THIRD                    0x00000300
    #define TSP_CSA_KL_SRC_FOURTH                   0x00000400
    #define TSP_CSA_KL_SRC_MASK                     0x00000700
    #define TSP_CSA_KL_DEST_ACPU                    0x01000000
    #define TSP_CSA_KL_DEST_SCRM                    0x02000000
    #define TSP_CSA_KL_DEST_CRYPTO                  0x04000000
    #define TSP_CSA_KL_DEST_THIRD                   0x08000000
    #define TSP_CSA_KL_DEST_FOURTH                  0x10000000
    #define TSP_CSA_KL_DEST_MASK                    0x1F000000
    #define TSP_CSA_KL_LEVEL_MASK                   0x0000F000
    #define TSP_CSA_KL_LEVEL_MASK_SHIFT             0x0000000A

    REG32                           KL_Ctrl1_KL_Status;                 // 0xbf201838   0x0e
    #define TSP_CSA_KL_STATUS_MASK                  0xFFFF0000
    #define TSP_CSA_KL_STATUS_ACPU_RDY              0x00040000
    #define TSP_CSA_KL_STATUS_SCRM_RDY              0x00080000
    #define TSP_CSA_KL_STATUS_CRYPTO_RDY            0x00100000
    #define TSP_CSA_KL_STATUS_THIRD_RDY             0x00100000
    #define TSP_CSA_KL_STATUS_FOURTH_RDY            0x00200000
    #define TSP_CSA_KL_STATUS_RDY_MASK              0x00FC0000
    #define TSP_CSA_KL_CTRL1_MASK                   0x0000FFFF
    #define TSP_CSA_KL_CTRL1_SWRST                  0x00000000
    #define TSP_CSA_KL_CTRL1_KL_KTE_MODE            0x00000002

    REG_CsaCWACPU                   KL_CWACPU;                          // 0xbf201840-0xbf201850   0x10-0x17

    REG_CsaACPUKeyIn                KL_ACPU_Key;                        // 0xbf201860-0xbf201878   0x18-0x1F

    REG16                           KL_ACPU_Status;                     // 0xbf201880   0x20
    #define TSP_CSA_START_ACPU_CMD                  0x00000001

    REG16                           KL_ACPU_Cmd;                        // 0xbf201888   0x22
    REG32                           KL_ACPU_WADDR;                      // 0xbf201890   0x24
    REG32                           KL_ACPU_RADDR;                      // 0xbf201898   0x26
    REG32                           KL_ACPU_Ctrl0;                      // 0xbf2018a0   0x28
    #define TSP_CSA_KL_ACPU_SEL_MASK                0x0000000F
    #define TSP_CSA_KL_ACPU_START                   0x00000001

    REG32                           _0xbf2018a0_0xbf2018b0[2];          // 0xbf2018a0-0xbf2018b0   0x2a-2d
    REG16                           CW_Status;                          // 0xbf2018b8   0x2e
    REG16                           ESA_Mode;                           // 0xbf2018c0   0x30
    #define TSP_CSA_ESA_DES_MODE                    BIT0
    #define TSP_CSA_TDES_MODE                       BIT1
    #define TSP_CSA_TDES_ABC_KEY                    BIT2
    #define TSP_CSA_ESA_AES_MODE                    BIT3
    #define TSP_CSA_ESA_ECB_MODE                    BIT4
    #define TSP_CSA_ESA_CBC_MODE                    BIT5
    #define TSP_CSA_ESA_CBC_CLR_MODE                BIT6
    #define TSP_CSA_ESA_DECRYPT                     BIT7

    REG32                           KTE_Valid_Bank;                     // 0xbf2018c8   0x32

    REG32                           KL_CW_Key[12];                      // 0xbf2018d0-0xbf201930   0x48-0x4b
    REG32                           Desc_Debug;                         // 0xbf201930   0x4c
    REG32                           Reg16_Out;                          // 0xbf201938   0x4e

    // AES/TDES DMA Register
    REG32                           File_DMA_Ctrl;                      // 0xbf201940   0x50
    REG32                           FileIn_DMA_Addr;                    // 0xbf201948   0x52
    REG32                           FileIn_DMA_Size;                    // 0xbf201948   0x54
    REG32                           FileOut_DMA_SAddr;                  // 0xbf201950   0x56
    REG32                           FileOut_DMA_EAddr;                  // 0xbf201958   0x58
    REG32                           File_DMA_Patten;                    // 0xbf201960   0x5a
    REG32                           File_DMA_Patten_Mask;               // 0xbf201968   0x5c
    REG16                           File_DMA2_Ctrl;                     // 0xbf201970   0x5e

    //Mark not finish
    //REG32                           FileOut_DMA_EAddr;                  // 0xbf201958   0x58
    //REG32                           FileOut_DMA_EAddr;                  // 0xbf201958   0x58
} REG_CsaCtrl;

// Firmware status
#define TSP_FW_STATE_MASK           0xFFFF0000
#define TSP_FW_STATE_LOAD           0x00010000
#define TSP_FW_STATE_ENG_OVRUN      0x00020000
#define TSP_FW_STATE_ENG1_OVRUN     0x00040000                          //[reserved]
#define TSP_FW_STATE_IC_ENABLE      0x01000000
#define TSP_FW_STATE_DC_ENABLE      0x02000000
#define TSP_FW_STATE_IS_ENABLE      0x04000000
#define TSP_FW_STATE_DS_ENABLE      0x08000000

#endif // #ifndef _TSP_REG_MCU_H_

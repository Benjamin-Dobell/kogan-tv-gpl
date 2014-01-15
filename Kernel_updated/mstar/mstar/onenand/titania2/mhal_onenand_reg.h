////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
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

#ifndef _MHAL_ONENAND_REG_H_
#define _MHAL_ONENAND_REG_H_

#include "mdrv_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define _BIT(x)                             (1<<(x))
#define BMASK(bits)                         (_BIT(((1)?bits)+1)-_BIT(((0)?bits)))
#define BITS(bits,value)                    ((_BIT(((1)?bits)+1)-_BIT(((0)?bits))) & (value<<((0)?bits)))

#define READ_BYTE(_reg)                     (*(volatile U8*)(_reg))
#define READ_WORD(_reg)                     (*(volatile U16*)(_reg))
#define READ_LONG(_reg)                     (*(volatile U32*)(_reg))
#define WRITE_BYTE(_reg, _val)              { (*((volatile U8*)(_reg))) = (U8)(_val); }
#define WRITE_WORD(_reg, _val)              { (*((volatile U16*)(_reg))) = (U16)(_val); }
#define WRITE_LONG(_reg, _val)              { (*((volatile U32*)(_reg))) = (U32)(_val); }
#define WRITE_WORD_MASK(_reg, _val, _mask)  { (*((volatile U16*)(_reg))) = ((*((volatile U16*)(_reg))) & ~(_mask)) | ((U16)(_val) & (_mask)); }

#define PFSH_READ(_reg)                     READ_WORD(REG_NORPF_BASE + ((_reg)<<2))
#define PFSH_WRITE(_reg, _val)              WRITE_WORD(REG_NORPF_BASE + ((_reg)<<2), (_val))
#define PFSH_WRITE_MASK(_reg, _val, _mask)  WRITE_WORD_MASK(REG_NORPF_BASE + ((_reg)<<2), (_val), (_mask))

#define PIU_WRITE_MASK(addr, val, mask)     WRITE_WORD_MASK(REG_PIU_BASE + ((addr)<<2), (val), (mask))


#define REG_NORPF_BASE                  0xBF800B00
#define REG_PIU_BASE                    0xBF807800

// NORPF
#define REG_NORPF_CLOCK_LENGTH          0x00
#define REG_NORPF_FLASH_DATA_WIDTH8     0x00
#define REG_NORPF_WRITE_RUN             0x01
#define REG_NORPF_DIRECT_READ_MODE      0x01
#define REG_NORPF_CEB_START             0x02
#define REG_NORPF_CEB_END               0x02
#define REG_NORPF_OEB_START             0x02
#define REG_NORPF_OEB_END               0x02
#define REG_NORPF_WEB_START             0x03
#define REG_NORPF_WEB_END               0x03
#define REG_NORPF_DATAOEN_START         0x03
#define REG_NORPF_DATAOEN_END           0x03
#define REG_NORPF_DATA_LATCH_CNT        0x04
#define REG_NORPF_CTRL                  0x06
#define REG_NORPF_BRIDGE_CTRL           0x07
#define REG_NORPF_XIU_CTRL              0x08
#define REG_NORPF_XIU_ADDR_L            0x09
#define REG_NORPF_XIU_ADDR_H            0x0A
#define REG_NORPF_XIU_WDATA             0x0B
#define REG_NORPF_XIU_STATUS            0x0C
    #define NORPF_XIU_STATUS_MASK           BMASK(0:0)
#define REG_NORPF_XIU_RDATA_L           0x0D
#define REG_NORPF_XIU_RDATA_H           0x0E
#define REG_NORPF_WRITE_ADDR0_L         0x10
#define REG_NORPF_WRITE_ADDR0_H         0x11
#define REG_NORPF_WRITE_ADDR1_L         0x12
#define REG_NORPF_WRITE_ADDR1_H         0x13
#define REG_NORPF_WRITE_ADDR2_L         0x14
#define REG_NORPF_WRITE_ADDR2_H         0x15
#define REG_NORPF_WRITE_ADDR3_L         0x16
#define REG_NORPF_WRITE_ADDR3_H         0x17
#define REG_NORPF_WRITE_ADDR4_L         0x18
#define REG_NORPF_WRITE_ADDR4_H         0x19
#define REG_NORPF_WRITE_ADDR5_L         0x1A
#define REG_NORPF_WRITE_ADDR5_H         0x1B
#define REG_NORPF_WRITE_ADDR6_L         0x1C
#define REG_NORPF_WRITE_ADDR6_H         0x1D
#define REG_NORPF_WRITE_ADDR7_L         0x1E
#define REG_NORPF_WRITE_ADDR7_H         0x1F
#define REG_NORPF_WRITE_DATA0           0x20
#define REG_NORPF_WRITE_DATA1           0x21
#define REG_NORPF_WRITE_DATA2           0x22
#define REG_NORPF_WRITE_DATA3           0x23
#define REG_NORPF_WRITE_DATA4           0x24
#define REG_NORPF_WRITE_DATA5           0x25
#define REG_NORPF_WRITE_DATA6           0x26
#define REG_NORPF_WRITE_DATA7           0x27

// PIU_DMA
#define REG_PIU_DMA_STATUS          0x10 // [1]done [2]busy [8:15]state
    #define PIU_DMA_DONE_MASK           BMASK(0:0)
    #define PIU_DMA_DONE                    BITS(0:0, 1)
    #define PIU_DMA_BUSY_MASK           BMASK(1:1)
    #define PIU_DMA_BUSY                    BITS(1:1, 1)
    #define PIU_DMA_STATE_MASK          BMASK(15:8)
#define REG_PIU_SPI_CLK_SRC         0x26 // SPI clock source  [0]:gate  [1]:inv  [4:2]:clk_sel  000:Xtal clock 001:27MHz 010:36MHz 011:43.2MHz 100:54MHz 101:72MHz 110:86MHz 111:108MHz  [5]:0:xtal 1:clk_Sel
    #define PSCS_DISABLE_MASK           BMASK(0:0)
    #define PSCS_INVERT_MASK            BMASK(1:1)
    #define PSCS_CLK_SEL_MASK           BMASK(4:2)
    #define PSCS_CLK_SEL_XTAL               BITS(4:2, 0)
    #define PSCS_CLK_SEL_27MHZ              BITS(4:2, 1)
    #define PSCS_CLK_SEL_36MHZ              BITS(4:2, 2)
    #define PSCS_CLK_SEL_43MHZ              BITS(4:2, 3)
    #define PSCS_CLK_SEL_54MHZ              BITS(4:2, 4)
    #define PSCS_CLK_SEL_72MHZ              BITS(4:2, 5)
    #define PSCS_CLK_SEL_86MHZ              BITS(4:2, 6)
    #define PSCS_CLK_SEL_108MHZ             BITS(4:2, 7)
    #define PSCS_CLK_SRC_SEL_MASK       BMASK(5:5)
    #define PSCS_CLK_SRC_SEL_XTAL           BITS(5:5, 0)
    #define PSCS_CLK_SRC_SEL_CLK            BITS(5:5, 1)
#define REG_PIU_DMA_SPISTART_L      0x70 // [15:0]
#define REG_PIU_DMA_SPISTART_H      0x71 // [23:16]
#define REG_PIU_DMA_DRAMSTART_L     0x72 // [15:0]  in unit of B; must be 8B aligned
#define REG_PIU_DMA_DRAMSTART_H     0x73 // [23:16]
#define REG_PIU_DMA_SIZE_L          0x74 // [15:0]  in unit of B; must be 8B aligned
#define REG_PIU_DMA_SIZE_H          0x75 // [23:16]
#define REG_PIU_DMA_CMD             0x76
    #define PIU_DMA_CMD_FIRE            0x0001
    #define PIU_DMA_CMD_LE              0x0000
    #define PIU_DMA_CMD_BE              0x0020


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif // _MHAL_ONENAND_REG_H_

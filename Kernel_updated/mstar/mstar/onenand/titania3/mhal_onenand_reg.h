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

#define SLP_READ(_reg)                     READ_WORD(REG_PMSLEEP_BASE + ((_reg)<<2))
#define SLP_WRITE(_reg, _val)              WRITE_WORD(REG_PMSLEEP_BASE + ((_reg)<<2), (_val))
#define SLP_WRITE_MASK(addr, val, mask)    WRITE_WORD_MASK(REG_PMSLEEP_BASE + ((addr)<<2), (val), (mask))

#define BK_PFSH 0x0B00
#define RIUBASE_NONPM     0xBF200000
#define REG_NORPF_BASE                  0xBF200B00
#define REG_PIU_BASE                    0xBF207800
#define REG_PMSLEEP_BASE				0xBF001C00

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

// PM_SLEEP CMD.
#define REG_PM_CHIP_OV_CONFIG           0x1F
    #define PM_CHIP_OV_DBUS_MASK            BMASK(9:8)
    #define PM_CHIP_OV_DBUS_ENABLE          BITS(9:8, 3)
#define REG_PM_CKG_SPI                  0x20 // Ref spec. before using these setting.
    #define PM_SPI_CLK_SEL_MASK             BMASK(13:10)
    #define PM_SPI_CLK_XTALI                BITS(13:10, 0)
    #define PM_SPI_CLK_27MHZ                BITS(13:10, 1)
    #define PM_SPI_CLK_36MHZ                BITS(13:10, 2)
    #define PM_SPI_CLK_43MHZ                BITS(13:10, 3)
    #define PM_SPI_CLK_54MHZ                BITS(13:10, 4)
    #define PM_SPI_CLK_72MHZ                BITS(13:10, 5)
    #define PM_SPI_CLK_86MHZ                BITS(13:10, 6)
    #define PM_SPI_CLK_108MHZ               BITS(13:10, 7)
    #define PM_SPI_CLK_24MHZ                BITS(13:10, 15)
    #define PM_SPI_CLK_SWITCH_MASK      BMASK(14:14)
    #define PM_SPI_CLK_SWITCH_OFF           BITS(14:14, 0)
    #define PM_SPI_CLK_SWITCH_ON            BITS(14:14, 1)



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

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

#ifndef _REG_IIC_H_
#define _REG_IIC_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define IIC_UNIT_NUM               2

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define MHal_IIC_DELAY()      { udelay(1000); udelay(1000); udelay(1000); udelay(1000); udelay(1000); }//delay 5ms

#define REG_IIC_BASE              0xBF806800                          // 0xBF800000 + (0x1A10*4)

#define REG_IIC_CTRL               0x00
#define REG_IIC_CLK_SEL            0x01
#define REG_IIC_WDATA              0x02
#define REG_IIC_RDATA              0x03
#define REG_IIC_STATUS             0x04                                // reset, clear and status

#define MHal_IIC_REG(addr)			(*(volatile U32*)(REG_IIC_BASE + ((addr)<<2)))

#define REG_CHIP_BASE              0xBF803C00                          // 0xBF800000 + (0xF00*4)
#define REG_IIC_MODE               0x50
#define REG_DDCR_GPIO_SEL          0x70
#define MHal_CHIP_REG(addr)             (*(volatile U32*)(REG_CHIP_BASE + ((addr)<<2)))


//the definitions of GPIO reg set to initialize
#define REG_MIPS_BASE               0xBF800000
#define REG_ALL_PAD_IN              ((0x0f50<<2) + 1)   //set all pads (except SPI) as input

#define REG_DDCR_CK_SET             (0x0f70<<2)
#define REG_DDCR_DA_SET             (0x0f70<<2)
#define REG_DDCR_CK2_SET
#define REG_DDCR_DA2_SET
#define REG_GPIO12_SET
#define REG_GPIO13_SET
#define REG_GPIO14_SET
#define REG_GPIO15_SET
#define REG_PCM2_IRQA_N_SET         (0x0f6e<<2)
#define REG_PCM2_WAIT_N_SET         (0x0f6e<<2)
#define REG_LHSYNC2_SET             ((0x0f02<<2) + 1)
#define REG_LVSYNC2_SET
/* i2C channel #4 in S6 MSTAR DVB */
#define REG_PCM2_CD_N_SET           (0x0f6e<<2)
#define REG_PCM2_RESET_SET          (0x0f6e<<2)
//PC-RGB EDID Control
#define REG_DDCA_SET                 (0x024a<<2)
#define REG_ET_TX_EN_SET            ((0x0f6f<<2) + 1)

//the definitions of GPIO reg set to make output
#define REG_DDCR_CK_OEN             (0x0f30<<2)
#define REG_DDCR_DA_OEN             (0x0f30<<2)
#define REG_DDCR_CK2_OEN            ((0x0f30<<2) + 1)
#define REG_DDCR_DA2_OEN            ((0x0f30<<2) + 1)
#define REG_GPIO12_OEN              ((0x0f30<<2) + 1)
#define REG_GPIO13_OEN              ((0x0f30<<2) + 1)
#define REG_GPIO14_OEN              ((0x0f30<<2) + 1)
#define REG_GPIO15_OEN              ((0x0f30<<2) + 1)
#define REG_PCM2_IRQA_N_OEN         ((0x0f40<<2) + 1)
#define REG_PCM2_WAIT_N_OEN         ((0x0f40<<2) + 1)
#define REG_LHSYNC2_OEN             ((0x0f30<<2) + 1)
#define REG_LVSYNC2_OEN             ((0x0f30<<2) + 1)
/* i2C channel #4 in S6 MSTAR DVB */
#define REG_PCM2_CD_N_OEN           ((0x0f40<<2) + 1)
#define REG_PCM2_RESET_OEN          ((0x0f40<<2) + 1)
//PC-RGB EDID Control
#define REG_DDCA_CK_OEN             (0x024a<<2)
#define REG_DDCA_DA_OEN             (0x024a<<2)
#define REG_ET_TX_EN_OEN            (0x0f45<<2)

//the definitions of GPIO reg set to get input
#define REG_DDCR_CK_IN              (0x0f2e<<2)
#define REG_DDCR_DA_IN              (0x0f2e<<2)
#define REG_DDCR_CK2_IN             ((0x0f2e<<2) + 1)
#define REG_DDCR_DA2_IN             ((0x0f2e<<2) + 1)
#define REG_GPIO12_IN               ((0x0f2e<<2) + 1)
#define REG_GPIO13_IN               ((0x0f2e<<2) + 1)
#define REG_GPIO14_IN               ((0x0f2e<<2) + 1)
#define REG_GPIO15_IN               ((0x0f2e<<2) + 1)
#define REG_PCM2_IRQA_N_IN          ((0x0f3a<<2) + 1)
#define REG_PCM2_WAIT_N_IN          ((0x0f3a<<2) + 1)
#define REG_LHSYNC2_IN              ((0x0f2e<<2) + 1)
#define REG_LVSYNC2_IN              ((0x0f2e<<2) + 1)
/* i2C channel #4 in S6 MSTAR DVB */
#define REG_PCM2_CD_N_IN            ((0x0f3a<<2) + 1)
#define REG_PCM2_RESET_IN           ((0x0f3a<<2) + 1)
//PC-RGB EDID Control
#define REG_DDCA_CK_IN              (0x024a<<2)
#define REG_DDCA_DA_IN              (0x024a<<2)
#define REG_ET_TX_EN_IN             ((0x0f41<<2) + 1)

//the definitions of GPIO reg set to output
#define REG_DDCR_CK_OUT             (0x0f2f<<2)
#define REG_DDCR_DA_OUT             (0x0f2f<<2)
#define REG_DDCR_CK2_OUT            ((0x0f2f<<2) + 1)
#define REG_DDCR_DA2_OUT            ((0x0f2f<<2) + 1)
#define REG_GPIO12_OUT              ((0x0f2f<<2) + 1)
#define REG_GPIO13_OUT              ((0x0f2f<<2) + 1)
#define REG_GPIO14_OUT              ((0x0f2f<<2) + 1)
#define REG_GPIO15_OUT              ((0x0f2f<<2) + 1)
#define REG_PCM2_IRQA_N_OUT         ((0x0f3d<<2) + 1)
#define REG_PCM2_WAIT_N_OUT         ((0x0f3d<<2) + 1)
#define REG_LHSYNC2_OUT             ((0x0f2f<<2) + 1)
#define REG_LVSYNC2_OUT             ((0x0f2f<<2) + 1)
/* i2C channel #4 in S6 MSTAR DVB */
#define REG_PCM2_CD_N_OUT           ((0x0f3d<<2) + 1)
#define REG_PCM2_RESET_OUT          ((0x0f3d<<2) + 1)
//PC-RGB EDID Control
#define REG_DDCA_CK_OUT             (0x024a<<2)
#define REG_DDCA_DA_OUT             (0x024a<<2)
#define REG_ET_TX_EN_OUT            ((0x0f43<<2) + 1)

#define MHal_GPIO_REG(addr)             (*(volatile U8*)(REG_MIPS_BASE + (addr)))
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    U16 SclOenReg;
    U8  SclOenBit;

    U16 SclOutReg;
    U8  SclOutBit;

    U16 SclInReg;
    U8  SclInBit;

    U16 SdaOenReg;
    U8  SdaOenBit;

    U16 SdaOutReg;
    U8  SdaOutBit;

    U16 SdaInReg;
    U8  SdaInBit;

    U8  DefDelay;
}IIC_Bus_t;

#endif // _REG_IIC_H_


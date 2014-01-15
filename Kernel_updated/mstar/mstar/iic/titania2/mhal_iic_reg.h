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

#define REG_IIC_BASE              0xBF806840                          // 0xBF800000 + (0x1A10*4)

#define REG_IIC_CTRL               0x00
#define REG_IIC_CLK_SEL            0x01
#define REG_IIC_WDATA              0x02
#define REG_IIC_RDATA              0x03
#define REG_IIC_STATUS             0x04                                // reset, clear and status

#define MHal_IIC_REG(addr)			(*(volatile U32*)(REG_IIC_BASE + ((addr)<<2)))

#define REG_CHIP_BASE              0xBF803C00
#define REG_IIC_MODE               0x50
#define REG_DDCR_GPIO_SEL          0x70
#define MHal_CHIP_REG(addr)             (*(volatile U32*)(REG_CHIP_BASE + ((addr)<<2)))


//the definitions of GPIO reg set to initialize
#define REG_MIPS_BASE               0xBF800000
#define REG_ALL_PAD_IN              ((0x0f50<<2) + 1)   //set all pads (except SPI) as input

//channel 0
#define REG_DDCR_DA2_SET
#define REG_DDCR_DA2_IN             ((0x0f2e<<2) + 1)
#define REG_DDCR_DA2_OUT            ((0x0f2f<<2) + 1)
#define REG_DDCR_DA2_OEN            ((0x0f30<<2) + 1)
#define REG_DDCR_CK2_SET
#define REG_DDCR_CK2_IN             ((0x0f2e<<2) + 1)
#define REG_DDCR_CK2_OUT            ((0x0f2f<<2) + 1)
#define REG_DDCR_CK2_OEN            ((0x0f30<<2) + 1)

//channel 1
#define REG_LDE_SET                 (0x194d<<2)
#define REG_LDE_IN                  (0x1955<<2)
#define REG_LDE_OUT                 (0x194f<<2)
#define REG_LDE_OEN                 (0x1951<<2)
#define REG_LCK_SET                 (0x194d<<2)
#define REG_LCK_IN                  (0x1955<<2)
#define REG_LCK_OUT                 (0x194f<<2)
#define REG_LCK_OEN                 (0x1951<<2)

//channel 2
#define REG_I2S_IN_BCK_SET          (0x0f51<<2)
#define REG_I2S_IN_BCK_IN           (0x0f35<<2)
#define REG_I2S_IN_BCK_OUT          (0x0f36<<2)
#define REG_I2S_IN_BCK_OEN          (0x0f37<<2)
#define REG_I2S_IN_SD_SET           (0x0f51<<2)
#define REG_I2S_IN_SD_IN            (0x0f35<<2)
#define REG_I2S_IN_SD_OUT           (0x0f36<<2)
#define REG_I2S_IN_SD_OEN           (0x0f37<<2)

//channel 3
#define REG_DDCROM_DA_SET           (0x024e<<2)
#define REG_DDCROM_DA_IN            (0x024e<<2)
#define REG_DDCROM_DA_OUT           (0x024e<<2)
#define REG_DDCROM_DA_OEN           (0x024e<<2)
#define REG_DDCROM_CK_SET           (0x024e<<2)
#define REG_DDCROM_CK_IN            (0x024e<<2)
#define REG_DDCROM_CK_OUT           (0x024e<<2)
#define REG_DDCROM_CK_OEN           (0x024e<<2)

//channel 4
#define REG_DDCA_DA_SET             (0x024a<<2)
#define REG_DDCA_DA_IN              (0x024a<<2)
#define REG_DDCA_DA_OUT             (0x024a<<2)
#define REG_DDCA_DA_OEN             (0x024a<<2)
#define REG_DDCA_CK_SET             (0x024a<<2)
#define REG_DDCA_CK_IN              (0x024a<<2)
#define REG_DDCA_CK_OUT             (0x024a<<2)
#define REG_DDCA_CK_OEN             (0x024a<<2)

//channel 5
#define REG_DDCDA_DA_SET            (0x024b<<2)
#define REG_DDCDA_DA_IN             (0x024b<<2)
#define REG_DDCDA_DA_OUT            (0x024b<<2)
#define REG_DDCDA_DA_OEN            (0x024b<<2)
#define REG_DDCDA_CK_SET            (0x024b<<2)
#define REG_DDCDA_CK_IN             (0x024b<<2)
#define REG_DDCDA_CK_OUT            (0x024b<<2)
#define REG_DDCDA_CK_OEN            (0x024b<<2)

//channel 6
#define REG_DDCDB_DA_SET            ((0x024b<<2) + 1)
#define REG_DDCDB_DA_IN             ((0x024b<<2) + 1)
#define REG_DDCDB_DA_OUT            ((0x024b<<2) + 1)
#define REG_DDCDB_DA_OEN            ((0x024b<<2) + 1)
#define REG_DDCDB_CK_SET            ((0x024b<<2) + 1)
#define REG_DDCDB_CK_IN             ((0x024b<<2) + 1)
#define REG_DDCDB_CK_OUT            ((0x024b<<2) + 1)
#define REG_DDCDB_CK_OEN            ((0x024b<<2) + 1)

//channel 7
#define REG_DDCDC_DA_SET            (0x024c<<2)
#define REG_DDCDC_DA_IN             (0x024c<<2)
#define REG_DDCDC_DA_OUT            (0x024c<<2)
#define REG_DDCDC_DA_OEN            (0x024c<<2)
#define REG_DDCDC_CK_SET            (0x024c<<2)
#define REG_DDCDC_CK_IN             (0x024c<<2)
#define REG_DDCDC_CK_OUT            (0x024c<<2)
#define REG_DDCDC_CK_OEN            (0x024c<<2)

#define MHal_GPIO_REG(addr)             (*(volatile U8*)(REG_MIPS_BASE + (addr)))
#define MHal_PIU_REG(addr)              (*(volatile U8*)(REG_MIPS_BASE + (addr)))

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


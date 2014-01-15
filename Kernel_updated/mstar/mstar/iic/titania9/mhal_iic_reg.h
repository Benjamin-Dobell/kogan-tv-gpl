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
//#define IIC_UNIT_NUM               2

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define MHal_IIC_DELAY()      { udelay(1000); udelay(1000); udelay(1000); udelay(1000); udelay(1000); }//delay 5ms

#define REG_IIC_BASE              0xBF206840                          // 0xBF200000 + (0x1A10*4)

#define REG_IIC_CTRL               0x00
#define REG_IIC_CLK_SEL            0x01
#define REG_IIC_WDATA              0x02
#define REG_IIC_RDATA              0x03
#define REG_IIC_STATUS             0x04                                // reset, clear and status

#define MHal_IIC_REG(addr)			(*(volatile U32*)(REG_IIC_BASE + ((addr)<<2)))

#define REG_CHIP_BASE              0xBF203C00
#define REG_IIC_ALLPADIN           0x50
#define REG_IIC_MODE               0x50
#define REG_DDCR_GPIO_SEL          0x70
#define MHal_CHIP_REG(addr)             (*(volatile U32*)(REG_CHIP_BASE + ((addr)<<2)))


//the definitions of GPIO reg set to initialize
#define REG_MIPS_BASE              0xBF000000//Use 8 bit addressing
//#define REG_ALL_PAD_IN              ((0x0f50<<1) )   //set all pads (except SPI) as input
#define REG_ALL_PAD_IN              (0x101ea1)   //set all pads (except SPI) as input

//the definitions of GPIO reg set to make output
//REG_DDCR_CK_SET
#define REG_DDCR_DA_OEN                (0x101e60)
#define REG_DDCR_CK_OEN                (0x101e60)
#define REG_DDCR_DA_OUT                (0x101e5a)
#define REG_DDCR_CK_OUT                (0x101e5a)
#define REG_DDCR_DA_IN                (0x101e54)
#define REG_DDCR_CK_IN                (0x101e54)




#define REG_PCM_RESET_OEN              (0x101e7f)
#define REG_PCM_CD_N_OEN             (0x101e80)
#define REG_PCM2_CE_N_OEN              (0x101e81)
#define REG_PCM2_IRQA_N_OEN             (0x101e81)
#define REG_PCM2_WAIT_N_OEN             (0x101e81)
#define REG_PCM2_RESET_OEN             (0x101e81)
#define REG_PCM2_CD_N_OEN             (0x101e81)

#define REG_PCM_RESET_IN              (0x101e73)
#define REG_PCM_CD_N_IN             (0x101e74)
#define REG_PCM2_CE_N_IN              (0x101e75)
#define REG_PCM2_IRQA_N_IN             (0x101e75)
#define REG_PCM2_WAIT_N_IN             (0x101e75)
#define REG_PCM2_RESET_IN             (0x101e75)
#define REG_PCM2_CD_N_IN             (0x101e75)

#define REG_PCM_RESET_OUT              (0x101e79)
#define REG_PCM_CD_N_OUT             (0x101e7a)
#define REG_PCM2_CE_N_OUT              (0x101e7b)
#define REG_PCM2_IRQA_N_OUT             (0x101e78)
#define REG_PCM2_WAIT_N_OUT             (0x101e78)
#define REG_PCM2_RESET_OUT             (0x101e78)
#define REG_PCM2_CD_N_OUT             (0x101e78)

#define REG_PCM2_CD_N_SET             (0x101edf)

#define REG_DDCR_DA_SET1                (0x101e24)
#define REG_DDCR_DA_SET2                (0x101e51)
#define REG_DDCR_DA_SET3                (0x101ee0)
#define REG_DDCR_CK_SET1                (0x101e24)
#define REG_DDCR_CK_SET2                (0x101e51)
#define REG_DDCR_CK_SET3                (0x101ee0)

#define REG_PCM2_IRQA_N_SET1             (0x101e04)
#define REG_PCM2_IRQA_N_SET2             (0x101edc)
#define REG_PCM2_WAIT_N_SET             (0x101edc)

#define REG_LHSYNC2_SET             (0x101e05)
    //PC-RGB EDID Control
#define REG_DDCA_SET                (0x0494)
#define REG_DDCA_CK_OEN             (0x0494)
#define REG_DDCA_DA_OEN             (0x0494)

#define REG_DDCA_CK_OUT             (0x0494)
#define REG_DDCA_DA_OUT             (0x0494)
#define REG_DDCA_CK_IN             (0x0494)
#define REG_DDCA_DA_IN             (0x0494)


    // HDMI port A/B/C EDID Control
#define REG_DDCDA_SET               (0x0496)
#define REG_DDCDB_SET               (0x0497)
#define REG_DDCDC_SET               (0x0498)
#define REG_DDCDD_SET               (0x0499)


    // HDMI port A/B/C EDID Control
#define REG_DDCDA_CK_SET            (0x0496)
#define REG_DDCDA_DA_SET            (0x0496)
#define REG_DDCDB_CK_SET            (0x0497)
#define REG_DDCDB_DA_SET            (0x0497)
#define REG_DDCDC_CK_SET            (0x0498)
#define REG_DDCDC_DA_SET            (0x0498)
#define REG_DDCDD_CK_SET            (0x0499)
#define REG_DDCDD_DA_SET            (0x0499)


#define REG_DDCDA_CK_OEN            (0x0496)
#define REG_DDCDA_DA_OEN            (0x0496)
#define REG_DDCDB_CK_OEN            (0x0497)
#define REG_DDCDB_DA_OEN            (0x0497)
#define REG_DDCDC_CK_OEN            (0x0498)
#define REG_DDCDC_DA_OEN            (0x0498)
#define REG_DDCDD_CK_OEN            (0x0499)
#define REG_DDCDD_DA_OEN            (0x0499)

#define REG_DDCDA_CK_OUT            (0x0496)
#define REG_DDCDA_DA_OUT            (0x0496)
#define REG_DDCDB_CK_OUT            (0x0497)
#define REG_DDCDB_DA_OUT            (0x0497)
#define REG_DDCDC_CK_OUT            (0x0498)
#define REG_DDCDC_DA_OUT            (0x0498)
#define REG_DDCDD_CK_OUT            (0x0499)
#define REG_DDCDD_DA_OUT            (0x0499)

#define REG_DDCDA_CK_IN            (0x0496)
#define REG_DDCDA_DA_IN            (0x0496)
#define REG_DDCDB_CK_IN            (0x0497)
#define REG_DDCDB_DA_IN            (0x0497)
#define REG_DDCDC_CK_IN            (0x0498)
#define REG_DDCDC_DA_IN            (0x0498)
#define REG_DDCDD_CK_IN            (0x0499)
#define REG_DDCDD_DA_IN            (0x0499)


#define REG_I2S_IN_WS_OEN            (0x101e6e)
#define REG_I2S_IN_BCK_OEN            (0x101e6e)

#define REG_I2S_IN_WS_IN            (0x101e6a)
#define REG_I2S_IN_BCK_IN            (0x101e6a)
#define REG_I2S_IN_WS_OUT            (0x101e6c)
#define REG_I2S_IN_BCK_OUT            (0x101e6c)



#define REG_TEST_MODE                (0x101e24)
#define REG_SPDIF_IN_SET            (0x101ee1)

#define REG_I2S_IN_WS_SET1            (0x101e24)
#define REG_I2S_IN_WS_SET2            (0x101e04)
#define REG_I2S_IN_WS_SET3            (0x101ea2)
#define REG_I2S_IN_BCK_SET1            (0x101e24)
#define REG_I2S_IN_BCK_SET2            (0x101ea2)


#define REG_I2S_OUT_SD_SET1            (0x101e24)
#define REG_I2S_OUT_SD_SET2            (0x101ea2)
#define REG_I2S_OUT_SD1_SET            (0x101ea2)
#define REG_I2S_OUT_SD2_SET            (0x101ea2)
#define REG_I2S_OUT_SD3_SET            (0x101ea2)


#define REG_DDCR_DA_SET1                (0x101e24)
#define REG_DDCR_DA_SET2                (0x101e51)
#define REG_DDCR_DA_SET3                (0x101ee0)
#define REG_DDCR_CK_SET1                (0x101e24)
#define REG_DDCR_CK_SET2                (0x101e51)
#define REG_DDCR_CK_SET3                (0x101ee0)
#define REG_TGPIO0_SET1              (0x101e9e)
#define REG_TGPIO0_SET2              (0x101ea3)
#define REG_TGPIO1_SET              (0x101e9e)
#define REG_TGPIO2_SET1              (0x101e24)
#define REG_TGPIO2_SET2              (0x101e9f)
#define REG_TGPIO3_SET1              (0x101e24)
#define REG_TGPIO3_SET2              (0x101e9f)



#define REG_I2S_OUT_SD1_OEN            (0x101e6f)
#define REG_I2S_OUT_SD2_OEN            (0x101e6f)
#define REG_I2S_OUT_SD3_OEN            (0x101e6f)

#define REG_I2S_OUT_SD1_IN            (0x101e6b)
#define REG_I2S_OUT_SD2_IN            (0x101e6b)
#define REG_I2S_OUT_SD3_IN            (0x101e6b)
#define REG_I2S_OUT_SD1_OUT            (0x101e6d)
#define REG_I2S_OUT_SD2_OUT            (0x101e6d)
#define REG_I2S_OUT_SD3_OUT            (0x101e6d)

#define REG_SPDIF_IN_OEN            (0x101e6e)
#define REG_SPDIF_OUT_OEN            (0x101e6e)
#define REG_SPDIF_IN_IN            (0x101e6a)
#define REG_SPDIF_OUT_IN            (0x101e6a)
#define REG_SPDIF_IN_OUT            (0x101e6c)
#define REG_SPDIF_OUT_OUT            (0x101e6c)
#define REG_TGPIO0_OEN              (0x101e60)
#define REG_TGPIO1_OEN              (0x101e60)
#define REG_TGPIO2_OEN              (0x101e60)
#define REG_TGPIO3_OEN              (0x101e61)
#define REG_TGPIO0_IN              (0x101e54)
#define REG_TGPIO1_IN              (0x101e54)
#define REG_TGPIO2_IN              (0x101e54)
#define REG_TGPIO3_IN              (0x101e55)
#define REG_TGPIO0_OUT              (0x101e5a)
#define REG_TGPIO1_OUT              (0x101e5a)
#define REG_TGPIO2_OUT              (0x101e5a)
#define REG_TGPIO3_OUT              (0x101e5b)


#define REG_GPIO24_OEN               (0x101e5f)
#define REG_GPIO25_OEN               (0x101e5f)
#define REG_GPIO24_IN               (0x101e53)
#define REG_GPIO25_IN               (0x101e53)
#define REG_GPIO24_OUT               (0x101e59)
#define REG_GPIO25_OUT               (0x101e59)


#define REG_GPIO26_SET               (0x101ede)
#define REG_GPIO27_SET               (0x101ede)

#define REG_GPIO26_OEN               (0x101e5f)
#define REG_GPIO27_OEN               (0x101e5f)
#define REG_GPIO26_IN               (0x101e53)
#define REG_GPIO27_IN               (0x101e53)
#define REG_GPIO26_OUT               (0x101e59)
#define REG_GPIO27_OUT               (0x101e59)

#define REG_SAR_GPIO                (0x1422)
#define REG_DSPEJTAG_MODE            (0x101e07)
#define REG_SAR3_OEN                (0x1423)
#define REG_SAR3_SET1                (0x101e24)
#define REG_SAR3_SET2                (0x101e07)
#define REG_SAR3_OUT                (0x1424)
#define REG_SAR3_IN                (0x1425)

#if 1//will be remove later
#define PAD_DDCR_CK 173
#define REG_PAD_DDCR_CK_SET (0x101eae)
#define REG_PAD_DDCR_CK_OEN (0x102b87)
#define REG_PAD_DDCR_CK_IN (0x102b87)
#define REG_PAD_DDCR_CK_OUT (0x102b87)
#define PAD_DDCR_DA 172
#define REG_PAD_DDCR_DA_SET (0x101eae)
#define REG_PAD_DDCR_DA_OEN (0x102b86)
#define REG_PAD_DDCR_DA_IN (0x102b86)
#define REG_PAD_DDCR_DA_OUT (0x102b86)

#define PAD_TGPIO2 181
#define REG_PAD_TGPIO2_SET
#define REG_PAD_TGPIO2_OEN (0x102b8f)
#define REG_PAD_TGPIO2_IN (0x102b8f)
#define REG_PAD_TGPIO2_OUT (0x102b8f)
#define PAD_TGPIO3 182
#define REG_PAD_TGPIO3_SET
#define REG_PAD_TGPIO3_OEN (0x102b90)
#define REG_PAD_TGPIO3_IN (0x102b90)
#define REG_PAD_TGPIO3_OUT (0x102b90)

#define PAD_I2S_OUT_SD1 101
#define REG_PAD_I2S_OUT_SD1_SET
#define REG_PAD_I2S_OUT_SD1_OEN (0x102b3f)
#define REG_PAD_I2S_OUT_SD1_IN (0x102b3f)
#define REG_PAD_I2S_OUT_SD1_OUT (0x102b3f)
#define PAD_SPDIF_IN 95
#define REG_PAD_SPDIF_IN_SET
#define REG_PAD_SPDIF_IN_OEN (0x102b39)
#define REG_PAD_SPDIF_IN_IN (0x102b39)
#define REG_PAD_SPDIF_IN_OUT (0x102b39)

#define PAD_I2S_IN_WS 92
#define REG_PAD_I2S_IN_WS_SET
#define REG_PAD_I2S_IN_WS_OEN (0x102b36)
#define REG_PAD_I2S_IN_WS_IN (0x102b36)
#define REG_PAD_I2S_IN_WS_OUT (0x102b36)
#define PAD_I2S_IN_BCK 93
#define REG_PAD_I2S_IN_BCK_SET
#define REG_PAD_I2S_IN_BCK_OEN (0x102b37)
#define REG_PAD_I2S_IN_BCK_IN (0x102b37)
#define REG_PAD_I2S_IN_BCK_OUT (0x102b37)

#define PAD_I2S_OUT_SD3 103
#define REG_PAD_I2S_OUT_SD3_SET
#define REG_PAD_I2S_OUT_SD3_OEN (0x102b41)
#define REG_PAD_I2S_OUT_SD3_IN (0x102b41)
#define REG_PAD_I2S_OUT_SD3_OUT (0x102b41)
#define PAD_I2S_OUT_SD2 102
#define REG_PAD_I2S_OUT_SD2_SET
#define REG_PAD_I2S_OUT_SD2_OEN (0x102b40)
#define REG_PAD_I2S_OUT_SD2_IN (0x102b40)
#define REG_PAD_I2S_OUT_SD2_OUT (0x102b40)

#define PAD_GPIO_PM9 9
#define REG_PAD_GPIO_PM9_SET
#define REG_PAD_GPIO_PM9_OEN (0x0f12)
#define REG_PAD_GPIO_PM9_IN (0x0f12)
#define REG_PAD_GPIO_PM9_OUT (0x0f12)
#define PAD_GPIO_PM8 8
#define REG_PAD_GPIO_PM8_SET
#define REG_PAD_GPIO_PM8_OEN (0x0f10)
#define REG_PAD_GPIO_PM8_IN (0x0f10)
#define REG_PAD_GPIO_PM8_OUT (0x0f10)
#endif

#define MHal_GPIO_REG(addr)             (*(volatile U8*)(REG_MIPS_BASE + (((addr) & ~1)<<1) + (addr & 1)))
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    U32 SclOenReg;
    U8  SclOenBit;

    U32 SclOutReg;
    U8  SclOutBit;

    U32 SclInReg;
    U8  SclInBit;

    U32 SdaOenReg;
    U8  SdaOenBit;

    U32 SdaOutReg;
    U8  SdaOutBit;

    U32 SdaInReg;
    U8  SdaInBit;

    U8  DefDelay;
}IIC_Bus_t;

#endif // _REG_IIC_H_


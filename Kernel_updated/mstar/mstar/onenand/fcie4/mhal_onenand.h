////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2010 MStar Semiconductor, Inc.
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

#ifndef __MHAL_ONIF_H__
#define __MHAL_ONIF_H__

#include "mdrv_types.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
//#define DBG_ONENAND(x)						(x)

/************************************************************************************
 *
 * Register Address Map
 *
 ************************************************************************************/
                                        	
#define REG_MANUFACTURER_ID					0xF000
#define REG_DEVICE_ID						0xF001
#define REG_VERSION_ID						0xF002
#define REG_DATA_BUFFER_SIZE				0xF003
#define REG_BOOT_BUFFER_SIZE				0xF004
#define REG_AMOUNT_OF_BUFFERS				0xF005
#define REG_TECHNOLOGY						0xF006
#define REG_START_ADDRESS_1					0xF100
#define REG_START_ADDRESS_3					0xF102
#define REG_START_ADDRESS_4					0xF103
#define REG_START_ADDRESS_5					0xF104
#define REG_START_ADDRESS_8					0xF107
#define REG_START_BUFFER					0xF200
#define REG_COMMAND							0xF220
#define REG_SYSTEM_CONFIGURATION_1			0xF221
#define REG_SYSTEM_CONFIGURATION_2			0xF222
#define REG_CONTROLLER_STATUS				0xF240
#define REG_INTERRUPT						0xF241
#define REG_START_BLOCK_ADDRESS				0xF24C
#define REG_WRITE_PROTECTION_STATUS			0xF24E
#define REG_ECC_STATUS						0xFF00
#define REG_ECC_RESULT_MAIN_DATA_1ST_SEC	0xFF01
#define REG_ECC_RESULT_SPARE_DATA_1ST_SEC	0xFF02
#define REG_ECC_RESULT_MAIN_DATA_2ND_SEC	0xFF03
#define REG_ECC_RESULT_SPARE_DATA_2ND_SEC	0xFF04
#define REG_ECC_RESULT_MAIN_DATA_3RD_SEC	0xFF05
#define REG_ECC_RESULT_SPARE_DATA_3RD_SEC	0xFF06
#define REG_ECC_RESULT_MAIN_DATA_4TH_SEC	0xFF07
#define REG_ECC_RESULT_SPARE_DATA_4TH_SEC	0xFF08

/************************************************************************************
 *
 * REG_MANUFACTURER_ID						0xF000
 *
 ************************************************************************************/
#define R_MANUFACTURER_ID_MSTAR			0x00EC

/************************************************************************************
 *
 * REG_DEVICE_ID							0xF001
 *
 ************************************************************************************/
#define R_DEVICE_3_3V						0x0001
#define R_DEVICE_DEMUXED					BIT(2)
#define R_DEVICE_DDP						BIT(3)
#define R_DEVICE_256M						(1<<4)
#define R_DEVICE_512M						(2<<4)
#define R_DEVICE_1G							(3<<4)
#define R_DEVICE_2G							(4<<4)
#define R_DEVICE_4G							(5<<4)
#define R_DEVICE_NOT_BOTTOM_BOOT			BIT(8)

/************************************************************************************
 *
 * REG_AMOUNT_OF_BUFFERS					0xF005
 *
 ************************************************************************************/
#define GET_BOOT_BUF_AMOUNT(x)				((x)&0xFF)
#define GET_DATA_BUF_AMOUNT(x)				(((x)>>8)&0xFF)

/************************************************************************************
 *
 * REG_TECHNOLOGY							0xF006
 *
 ************************************************************************************/
#define R_DEVICE_SLC						0x0000
#define R_DEVICE_MLC						0x0001

/************************************************************************************
 *
 * REG_START_ADDRESS_4
 *
 ************************************************************************************/
#define R_COPY_BACK_PAGE_ADDR_MASK			BIT(7)|BIT(6)|BIT(5)|BIT(4)|BIT(3)|BIT(2)
#define R_COPY_BACK_SECTOR_ADDR_MASK		BIT(1)|BIT(0)

/************************************************************************************
 *
 * REG_START_ADDRESS_8
 *
 ******************************************/
#define R_NAND_PAGE_ADDR_MASK				BIT(7)|BIT(6)|BIT(5)|BIT(4)|BIT(3)|BIT(2)
#define R_NAND_SECTOR_ADDR_MASK				BIT(1)|BIT(0)

/************************************************************************************
 *
 * REG_START_BUFFER
 *
 ************************************************************************************/
#define R_BOOTRAM_DATARAM_SEL				BIT(11)
#define R_DATA0_DATA1_SEL					BIT(10)
#define R_SECTOR_0_SEL						(0x00<<8)
#define R_SECTOR_1_SEL						(0x01<<8)
#define R_SECTOR_2_SEL						(0x02<<8)
#define R_SECTOR_3_SEL						(0x03<<8)
#define R_BUFFER_RAM_SECTOR_COUNT_1			0x01
#define R_BUFFER_RAM_SECTOR_COUNT_2			0x02
#define R_BUFFER_RAM_SECTOR_COUNT_3			0x03
#define R_BUFFER_RAM_SECTOR_COUNT_4			0x00	// Be careful

/************************************************************************************
 *
 * REG_COMMAND
 *
 ************************************************************************************/
#define R_CMD_LOAD_DATA						0x0000
#define R_CMD_LOAD_SPARE					0x0013
#define R_CMD_PROGRAM_DATA					0x0080
#define R_CMD_PROGRAM_SPARE					0x001A
#define R_CMD_COPY_BACK_PROGRAM				0x001B
#define R_CMD_UNLOCK_BLOCK					0x0023
#define R_CMD_LOCK_BLOCK					0x002A
#define R_CMD_LOCK_TIGHT_BLOCK				0x002C
#define R_CMD_ALL_BLOCK_UNLOCK				0x0027
#define R_CMD_ERASE_VERIFY_READ				0x0071
#define R_CMD_BLOCK_ERASE					0x0094
#define R_CMD_MULTI_BLOCK_ERASE				0x0095
#define R_CMD_ERASE_SUSPEND					0x00B0
#define R_CMD_ERASE_RESUME					0x0030
#define R_CMD_RESET_NAND_CORE				0x00F0
#define R_CMD_RESET_NAND					0x00F3
#define R_CMD_OTP_ACCESS					0x0065

/************************************************************************************
 *
 * REG_SYSTEM_CONFIGURATION_1
 *
 ************************************************************************************/
#define R_SYNCHRONOUS_READ					BIT(15)
#define R_ECC								BIT(8)
#define R_RDY_POLARITY						BIT(7)
#define R_INT_POLARITY						BIT(6)
#define R_IOBE								BIT(5)
#define R_RDY_CONF							BIT(4)
#define R_BWPS								BIT(0)

/************************************************************************************
 *
 * REG_CONTROLLER_STATUS
 *
 ************************************************************************************/
#define R_STATUS_ONGO						BIT(15)
#define R_STATUS_LOCK						BIT(14)
#define R_STATUS_LOAD						BIT(13)
#define R_STATUS_PROG						BIT(12)
#define R_STATUS_ERASE						BIT(11)
#define R_STATUS_ERROR						BIT(10)
#define R_STATUS_SUSPEND					BIT(9)
#define R_STATUS_RSTB						BIT(7)
#define R_STATUS_OTP_LOCK					BIT(6)
#define R_STATUS_1ST_BLOCK_OTP_LOCK			BIT(5)
#define R_STATUS_TIMEOUT					BIT(0)

/************************************************************************************
 *
 * REG_INTERRUPT
 *
 ************************************************************************************/
#define R_INTERRUPT_INT						BIT(15)
#define R_INTERRUPT_RI						BIT(7)
#define R_INTERRUPT_WI						BIT(6)
#define R_INTERRUPT_EI						BIT(5)
#define R_INTERRUPT_RSTI					BIT(4)						
#define R_CLEAR_INTERRUPT					0x0000

/************************************************************************************
 *
 * REG_WRITE_PROTECTION_STATUS
 *
 ************************************************************************************/
#define R_UNLOCK_STATUS						BIT(2)
#define R_LOCK_STATUS						BIT(1)
#define R_LOCK_TIGHT_STATUS					BIT(0)

#define BOOT_RAM_MAIN_0						0x0000
#define BOOT_RAM_MAIN_1						0x0100

#define DATA_RAM_MAIN_0						0x0200
#define DATA_RAM_MAIN_1						0x0600

#define BOOT_RAM_SPARE_0					0x8000
#define BOOT_RAM_SPARE_1					0x8008

#define DATA_RAM_SPARE_0					0x8010
#define DATA_RAM_SPARE_1					0x8030

#define ONIF_READ_MANUFID					0x00
#define ONIF_READ_DEVICEID					0x01
#define ONIF_READ_BLK_WP_STATUS				0x02

#define BSA_BOOTRAM_0						0x0000
#define BSA_BOOTRAM_1						0x0100
#define BSA_DATARAM_0_0						0x0800
#define BSA_DATARAM_0_1						0x0900
#define BSA_DATARAM_0_2						0x0A00
#define BSA_DATARAM_0_3						0x0B00
#define BSA_DATARAM_1_0						0x0C00
#define BSA_DATARAM_1_1						0x0D00
#define BSA_DATARAM_1_2						0x0E00
#define BSA_DATARAM_1_3						0x0F00

#define BSC_SECTOR_COUNT_1					0x0001
#define BSC_SECTOR_COUNT_2					0x0002
#define BSC_SECTOR_COUNT_3					0x0003
#define BSC_SECTOR_COUNT_4					0x0000

#define BLOCK_UNLOCK						0x00
#define BLOCK_LOCK							0x01
#define BLOCK_LOCK_TIGHT					0x02
#define BLOCK_ALL_UNLOCK					0x80

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
void HAL_ONIF_INIT(void);
void HAL_ONIF_Async_Write(U16 *pu16Buf, U16 u16Addr, U32 len);
void HAL_ONIF_Async_Read(U16 *pu16Buf, U16 u16Addr, U32 len);
void HAL_ONIF_Async_Read2Reg(U16 *pu16Buf, U16 u16Addr, U32 len);
void HAL_ONIF_Sync_Write(U16 *pu16Buf, U16 u16Addr, U32 len);
void HAL_ONIF_Sync_Read(U16 *pu16Buf, U16 u16Addr, U32 len);

#endif // __MHAL_ONIF_H__

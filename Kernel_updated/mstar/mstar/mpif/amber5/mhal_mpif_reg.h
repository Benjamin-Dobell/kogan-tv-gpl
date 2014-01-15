////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2009-2012 MStar Semiconductor, Inc.
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
/// file    mhal_mpif_reg.h
/// @brief  MPIF Register Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_MPIF_H_
#define _REG_MPIF_H_

#include "MsTypes.h"

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define MAX_TIMEOUT								        0x7FFFFFFF//0xA0000

#define MPIF_MIU_BUS									3
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

// BASEADDR & BK
#define BASEADDR_RIU                                    0xBF000000  

#define BK_CHIPTOP                                      (MS_U32)(BASEADDR_RIU + 0x203C00) //0x101E
#define BK_MPIF                                         (MS_U32)(BASEADDR_RIU + 0x220800) //0x1104
#define BK_CLKGEN0                                      (MS_U32)(BASEADDR_RIU + 0x201600) //0x100B

//----- Chip Top -------------------------
#define REG_MPIF_DRV					                0x0A
#define REG_MPIF_RPUPCI_ENZ				                0x0B
#define REG_ALLPAD_IN                                   0x50
#define REG_CKG_MPIF                                    0x68
#define REG_MPIF_MODE					                0x6F

// Register offsets of MPIF
#define REG_MPIF_LC1A_SETTINGS							0x00
#define REG_MPIF_LC2A_REG_CONTROL						0x01
#define REG_MPIF_LC2A_REG_ADDRESS						0x02
#define REG_MPIF_LC2A_REG_DATA							0x03
#define REG_MPIF_LC2B_REG_CONTROL						0x04
#define REG_MPIF_LC2B_REG_ADDRESS						0x05
#define REG_MPIF_LC2B_REG_DATA							0x06
	#define MPIF_LC2X_NUM										2
	#define MPIF_LC2X_NEXT_OFFSET								3	
#define REG_MPIF_LC3A_PACKET_CONTROL					0x07
#define REG_MPIF_LC3A_PACKET_LENGTH						0x08
#define REG_MPIF_LC3A_PACKET_DATA						0x09	// 0x09-0x10
#define REG_MPIF_LC3A_PACKET_MIU_BASE_ADDRESS			0x11	// 0x11-0x12
#define REG_MPIF_LC3A_PACKET_MIU_BASE_ADDRESS_L			0x11	// 0x11[0:15], Base address is at 0x11-0x12
#define REG_MPIF_LC3A_PACKET_MIU_BASE_ADDRESS_H			0x12	// 0x12[16:31]
#define REG_MPIF_LC3B_PACKET_CONTROL					0x13
#define REG_MPIF_LC3B_PACKET_LENGTH						0x14
#define REG_MPIF_LC3B_PACKET_DATA						0x15	// 0x15-0x1C
#define REG_MPIF_LC3B_PACKET_MIU_BASE_ADDRESS			0x1D	// 0x1D-0x1E
#define REG_MPIF_LC3B_PACKET_MIU_BASE_ADDRESS_L			0x1D	// 0x1D[0:15], Base address is at 0x1D
#define REG_MPIF_LC3B_PACKET_MIU_BASE_ADDRESS_H			0x1E	// 0x1E[16:31]
	#define MPIF_LC3X_NEXT_OFFSET								6	
#define REG_MPIF_LC4A_STREAM_CONTROL					0x1F
#define REG_MPIF_LC4A_STREAM_LENGTH						0x20
#define REG_MPIF_LC4A_STREAM_MIU_BASE_ADDRESS			0x21	// 0x21-0x22
#define REG_MPIF_LC4A_STREAM_MIU_BASE_ADDRESS_L			0x21	// 0x21[0:15], Base address is at 0x21
#define REG_MPIF_LC4A_STREAM_MIU_BASE_ADDRESS_H			0x22	// 0x22[16:31]
#define REG_MPIF_4WIRE_SPI_CONTROL						0x23
#define REG_MPIF_3WIRE_SPI_CONTROL						0x24
#define REG_MPIF_SPI_CONTROL							0x25
#define REG_MPIF_SPI_COMMAND_VALUE_L					0x26	// 0x26[0:15], Command value is at 0x26-0x27
#define REG_MPIF_SPI_COMMAND_VALUE_H					0x27	// 0x27[0:15]
#define REG_MPIF_SPI_DATA_LENGTH						0x28
#define REG_MPIF_SPI_MIU_BASE_ADDRESS_L					0x29	// 0x29[0:15], Base address is at 0x29-0x2A
#define REG_MPIF_SPI_MIU_BASE_ADDRESS_H					0x2A	// 0x2A[0:9]
	#define MPIF_SPI_MIU_BASEADDR_H_SHIFT				16
	#define MPIF_SPI_MIU_BASEADDR_H_MASK				0xFFFF
#define REG_MPIF_CHANNEL_BUSY_STATUS					0x2B
#define REG_MPIF_INTERRUPT_EVENT_MASK					0x2C                    
#define REG_MPIF_INTERRUPT_EVENT_STATUS					0x2D
#define REG_MPIF_BUSY_TIMEOUT_COUNTER					0x2E
#define REG_MPIF_BUSY_TIMEOUT_COMMAND_ID				0x2F
#define REG_MPIF_MISC1									0x30
#define REG_MPIF_MISC2									0x31
#define REG_MPIF_MISC3          						0x32
#define REG_MPIF_SYNC_CONTROL   						0x33
#define REG_MPIF_SPI_UCPLT_LENGTH						0x34
#define REG_MPIF_MIU_WPROTECT_CONTROL					0x35
#define REG_MPIF_MIU_ADDR_LOW_BOUND                     0x36
#define REG_MPIF_MIU_ADDR_LOW_BOUND_L                   0x36   // 0x36[0:15], Command value is at 0x36-0x37
#define REG_MPIF_MIU_ADDR_LOW_BOUND_H                   0x37   // 0x37[0:9]
#define REG_MPIF_MIU_ADDR_UPPER_BOUND                   0x38
#define REG_MPIF_MIU_ADDR_UPPER_BOUND_L                 0x38   // 0x38[0:15], Command value is at 0x38-0x39
#define REG_MPIF_MIU_ADDR_UPPER_BOUND_H                 0x39   // 0x39[0:9]
#define REG_MPIF_THROTTLE_CFG							0x3B
#define REG_MPIF_DEBUG_SELECTION						0x40
#define REG_MPIF_DEBUG_CHECKSUM							0x41   // 0x41-0x42
#define REG_MPIF_DEBUG_STS_IF_CTRL0						0x43
#define REG_MPIF_DEBUG_STS_IF_CTRL1						0x44
#define REG_MPIF_DEBUG_STS_PKT3A_CNT					0x45
#define REG_MPIF_DEBUG_STS_PKT3B_CNT					0x46
#define REG_MPIF_DEBUG_STS_STM0_CNT						0x47
#define REG_MPIF_DEBUG_STS_LC2X_CNT						0x48
#define REG_MPIF_DEBUG_STS_LC3X_CNT						0x49
#define REG_MPIF_DEBUG_STS_LC4X_CNT						0x4A
#define REG_MPIF_DEBUG_M2I_I2M_BISTFAIL					0x4B
#define REG_MPIF_DEBUG_MIU_MASK_RDY 					0x4C
#define REG_MPIF_DEBUG_MPIF2_SEL     					0x4D
    #define MPIF_LASTDONEZ_SHIFT                        3
    #define MPIF_LASTDONEZ_MASK                         0x08                              

//----- REG DATA Definition -------------------------------
//slave id
#define MPIF_SLAVE_ID_0									0x00
#define MPIF_SLAVE_ID_1									0x01
#define MPIF_SLAVE_ID_2									0x02
#define MPIF_SLAVE_ID_3									0x03

//mpif clock
#define MPIF_CLOCK_160MHZ                               0x10
#define MPIF_CLOCK_120MHZ                               0x04
#define MPIF_CLOCK_108MHZ                               0x08
#define MPIF_CLOCK_86MHZ                                0x0C
#define MPIF_CLOCK_72MHZ                                0x00
#define MPIF_CLOCK_54MHZ                                0x14
#define MPIF_CLOCK_27MHZ                                0x18
#define MPIF_CLOCK_13_5MHZ                              0x1C
#define MPIF_CLOCK_DEFAULT								MPIF_CLOCK_13_5MHZ

#define MPIF_2X_DEST_SPIF								0
#define MPIF_2X_DEST_XIU								1

#define MPIF_LC1A_INDEX_0								0x00
#define MPIF_LC1A_INDEX_1								0x01
#define MPIF_LC1A_INDEX_2								0x02
#define MPIF_LC1A_INDEX_3								0x03
#define MPIF_LC1A_INDEX_4								0x04
#define MPIF_LC1A_INDEX_5								0x05
#define MPIF_LC1A_INDEX_6								0x06
#define MPIF_LC1A_INDEX_7								0x07

//
#define MPIF_TR_CYCLE_0T								0
#define MPIF_TR_CYCLE_1T								1
#define MPIF_TR_CYCLE_2T								2
#define MPIF_TR_CYCLE_3T								3
#define MPIF_TR_CYCLE_4T								4
#define MPIF_TR_CYCLE_5T								5
#define MPIF_TR_CYCLE_6T								6
#define MPIF_TR_CYCLE_7T								7
#define MPIF_TR_CYCLE_DEFAULT							MPIF_TR_CYCLE_1T

#define MPIF_WAIT_CYCLE_0T								0
#define MPIF_WAIT_CYCLE_1T								1
#define MPIF_WAIT_CYCLE_2T								2
#define MPIF_WAIT_CYCLE_3T								3
#define MPIF_WAIT_CYCLE_DEFAULT							MPIF_WAIT_CYCLE_1T

#define MPIF_SLAVE_DATAWIDTH_1bit						0
#define MPIF_SLAVE_DATAWIDTH_2bit						1
#define MPIF_SLAVE_DATAWIDTH_4bit						2
#define MPIF_SLAVE_DATAWIDTH_8bit						3  //Not support
#define MPIF_SLAVE_DATAWIDTH_DEFAULT					MPIF_SLAVE_DATAWIDTH_1bit

#define MPIF_CMD_DATAWIDTH_1bit							0
#define MPIF_CMD_DATAWIDTH_2bit							1
#define MPIF_CMD_DATAWIDTH_4bit							2
#define MPIF_CMD_DATAWIDTH_8bit							3 //Not support
#define MPIF_CMD_DATAWIDTH_DEFAULT						MPIF_CMD_DATAWIDTH_1bit

#define MPIF_RTX_INDICATOR_DISABLE						0
#define MPIF_RTX_INDICATOR_ENABLE						1

#define MPIF_MAX_RTX_0									0x00    // 0 time
#define MPIF_MAX_RTX_1									0x01    // 1 times
#define MPIF_MAX_RTX_2									0x02    // 2 times
#define MPIF_MAX_RTX_3									0x03    // 3 times

#define MPIF_SEL_MIU0									0
#define MPIF_SEL_MIU1									1

#define MPIF_LC4A_CHECKPOINT_0							0x00	// 1*256 bytes
#define MPIF_LC4A_CHECKPOINT_1							0x01	// 2*256 bytes
#define MPIF_LC4A_CHECKPOINT_2							0x02	// 3*256 bytes
#define MPIF_LC4A_CHECKPOINT_3							0x03	// 4*256 bytes

#define MPIF_CLK_THROTTLE_RATE_CYL0						0						
#define MPIF_CLK_THROTTLE_RATE_CYL1						1
#define MPIF_CLK_THROTTLE_RATE_CYL2						2
#define MPIF_CLK_THROTTLE_RATE_CYL3						3

#define MPIF_CLK_THROTTLE_SIZE_4BYTES					0	
#define MPIF_CLK_THROTTLE_SIZE_8BYTES					1
#define MPIF_CLK_THROTTLE_SIZE_12BYTES					2
#define MPIF_CLK_THROTTLE_SIZE_36BYTES					3

//status event bit    
#define MPIF_EVENTBIT_4WIRESPI                          0x01
#define MPIF_EVENTBIT_3WIRESPI                          0x02
#define MPIF_EVENTBIT_1A                                0x04
#define MPIF_EVENTBIT_2A                                0x08
#define MPIF_EVENTBIT_2B                                0x10
#define MPIF_EVENTBIT_3A                                0x20
#define MPIF_EVENTBIT_3B                                0x40
#define MPIF_EVENTBIT_4A                                0x80
#define MPIF_EVENTBIT_2A_ERR                            0x0100
#define MPIF_EVENTBIT_2B_ERR                            0x0200
#define MPIF_EVENTBIT_3A_ERR                            0x0400
#define MPIF_EVENTBIT_3B_ERR                            0x0800
#define MPIF_EVENTBIT_4A_ERR                            0x1000
#define MPIF_EVENTBIT_BUSY_TIMEOUT                      0x2000
#define MPIF_EVENTBIT_SLAVE_REQUEST                     0x4000

#define MPIF_TNTBIT_ENABLE_MASK                         \
    (MPIF_EVENTBIT_3A|MPIF_EVENTBIT_3B|MPIF_EVENTBIT_4A|MPIF_EVENTBIT_3A_ERR|MPIF_EVENTBIT_3B_ERR|MPIF_EVENTBIT_4A_ERR|MPIF_EVENTBIT_BUSY_TIMEOUT)


#define MPIF_LC3X_PACKET_DATA_MAX_LENGTH				16 // 16 bytes
#define MPIF_LC3X_MAX_PACKET                            0xFFFF

#define MPIF_SPI_RIU_MAXLEN								16

#define MPIF_SPI_SRC_RIU								0
#define MPIF_SPI_SRC_MIU								1

#define MPIF_SPI_READ_MODE								0
#define MPIF_SPI_WRITE_MODE								1


//---------- SPIF REG Definition ---------------------------
#define REG_SPIF_LC3A_PACKET_CONTROL                    0x30
#define REG_SPIF_LC3A_CHKSUM                            0x31
#define REG_SPIF_LC3A_PACKET_LEN                        0x32
#define REG_SPIF_LC3A_MADR_L                            0x33
#define REG_SPIF_LC3A_MADR_H                            0x34

#define REG_SPIF_LC3B_PACKET_CONTROL                    0x40
#define REG_SPIF_LC3B_CHKSUM                            0x41
#define REG_SPIF_LC3B_PACKET_LEN                        0x42
#define REG_SPIF_LC3B_MADR_L                            0x43
#define REG_SPIF_LC3B_MADR_H                            0x44

#define REG_SPIF_LC4A_CONTROL                           0x50
#define REG_SPIF_LC4A_CHKSUM_L                          0x51
#define REG_SPIF_LC4A_CHKSUM_H                          0x52
#define REG_SPIF_LC4A_STREAM_LEN                        0x53
#define REG_SPIF_LC4A_MADR_L                            0x54
#define REG_SPIF_LC4A_MADR_H                            0x55

#define REG_SPIF_INT_MASK                               0x60
#define REG_SPIF_INT_CLR                                0x61
#define REG_SPIF_INT_RAW_STS                            0x63
#define REG_SPIF_INT_FINAL_STS                          0x64
#define REG_SPIF_CHANNEL_BUSY_STATUS                    0x65
#define REG_SPIF_MISC1                                  0x71
#define REG_SPIF_MISC2                                  0x72

//---------- SPIF REG Variable definition ---------------------------
#define SPIF_LC1A_BUSY                                  0x01
#define SPIF_LC2A_BUSY                                  0x02
#define SPIF_LC2B_BUSY                                  0x04
#define SPIF_LC3A_BUSY                                  0x08
#define SPIF_LC3B_BUSY                                  0x10
#define SPIF_LC4A_BUSY                                  0x20

#define SPIF_LC1A_TXDONE                                0x01
#define SPIF_LC2A_TXDONE                                0x02
#define SPIF_LC2B_TXDONE                                0x04
#define SPIF_LC3A_TXDONE                                0x08
#define SPIF_LC3B_TXDONE                                0x10
#define SPIF_LC4A_TXDONE                                0x20
#define SPIF_MIU_READDONE                               0x40
#define SPIF_MIU_WRITEDONE                              0x80
#define SPIF_LC2A_CKSUM_ERR                             0x0200
#define SPIF_LC2B_CKSUM_ERR                             0x0400
#define SPIF_LC3A_CKSUM_ERR                             0x0800
#define SPIF_LC3B_CKSUM_ERR                             0x1000
#define SPIF_LC4A_CKSUM_ERR                             0x2000

#define SPIF_CLK_DLY_BUFDLY_NUM							0 //0~15

#define SPIF_CLK_INV_DISABLE							0
#define SPIF_CLK_INV_ENABLE								1

#define SPIF_CLK_INV_DLY								0 //0~15

#define SPIF_CLK_INV_INV_DISABLE						0
#define SPIF_CLK_INV_INV_ENABLE							1

#define SPIF_EVENT_NULL									0x00
#define SPIF_EVENT_TX_DONE								0x01
#define SPIF_EVENT_TX_ERROR								0x02
//----------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct _HAL_MPIF_CONFIG
{			
	MS_U8 u8clk;      //Clock source setting 
	MS_U8 u8trc_1A2X; //MPIF read/write turn around cycle for 1A/2X channel
	MS_U8 u8trc_3X4A; //MPIF read/write turn around cycle for 3X/4A channel
	MS_U8 u8wc; //MPIF wait ACK/NAK cycle
	MS_U8 u8Slave0Dw; //Slave 0 Data Width
	MS_U8 u8Slave1Dw; //Slave 1 Data Width
	MS_U8 u8Slave2Dw; //Slave 2 Data Width
	MS_U8 u8Slave3Dw; //Slave 3 Data Width
	MS_U8 u8CmdDw; //commend data width			
};

typedef struct _HAL_MPIF_CONFIG	HAL_MPIF_CONFIG;

struct _HAL_MPIF_2XCTL
{	
	MS_BOOL bchksum_en;	
	MS_U8 	u8retrx;			
} __attribute__ ((packed)); 
typedef struct _HAL_MPIF_2XCTL	HAL_MPIF_2XCTL;

struct _HAL_MPIF_3XCTL
{
	MS_BOOL bchksum_en;
	MS_BOOL bretrx_idx;
	MS_U8 u8retrx;	
	MS_U8 u8MIUSel;
	MS_BOOL bfmode;
	MS_BOOL bNotWaitMIUDone;
	MS_U8 u8Wcnt;	
} __attribute__ ((packed)); 
typedef struct _HAL_MPIF_3XCTL	HAL_MPIF_3XCTL;

struct _HAL_MPIF_4ACTL
{		
	MS_U8 u8retrx_limit;	
	MS_U8 u8MIUSel;
	MS_BOOL bNotWaitMIUDone;
	MS_U8 u8granular;
	MS_U8 u8Wcnt;	
} __attribute__ ((packed)); 
typedef struct _HAL_MPIF_4ACTL	HAL_MPIF_4ACTL;

#endif // _REG_MPIF_H_


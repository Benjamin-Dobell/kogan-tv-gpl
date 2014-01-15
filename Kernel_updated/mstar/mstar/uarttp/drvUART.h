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
///
/// @file   drvUART.h
/// @brief  UART Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "mdrv_mstypes.h"

//#define TOUCH_SCREEN_WIDTH              240
//#define TOUCH_SCREEN_HEIGHT             320

#define TOUCH_SCREEN_WIDTH              1440
#define TOUCH_SCREEN_HEIGHT             900


#ifndef __MDRV_UART_H__
#define __MDRV_UART_H__

#define _BITMASK(loc_msb, loc_lsb) ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))
#define BITMASK(x) _BITMASK(1?x, 0?x)


#define MCU_BANK_BASE(bankbase) (0xBF000000+(bankbase)*2)
//#define UART_SEL_BASE ((unsigned char volatile*)0xBF203C00)
#define GENERAL_REG8(bankbase, reg8Idx)  \
	 (((volatile unsigned char *)MCU_BANK_BASE(bankbase))[(((reg8Idx)&~1)<<1)+((reg8Idx)&1)])
#define  GENERAL_WRITE_REG8(bankbase, reg8Idx, value, mask) \
	(GENERAL_REG8(bankbase, reg8Idx) = (GENERAL_REG8(bankbase, reg8Idx) & ~(mask)) | ((value) & (mask)))
	

#define FASTU_BANKBASE 0x110680

//#define FASTU_REG16(reg16Idx)    (((unsigned short volatile*)FASTU_BASE)[(reg16Idx)<<1])
#define FASTU_REG8(reg16Idx)    GENERAL_REG8(FASTU_BANKBASE,(reg16Idx)*2)

MS_U8 FASTU_ReadByte(unsigned int regIdx);

void FASTU_WriteByte(unsigned int regIdx, MS_U8 value);


MS_U16 mdrv_uart_mstp_set_baudrate(MS_U32 baudrate);
void MDrv_UART_MSTP_Init(MS_U32 baudrate);
MS_U16 mdrv_uart_mstp_connect(void);
#endif /* __MDRV_UART_H__ */

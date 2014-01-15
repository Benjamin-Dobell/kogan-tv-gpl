/***************************************************************************************************************
 *
 * FileName hal_card_platform.h
 *     @author jeremy.wang (2012/01/10)
 * Desc:
 * 	   This file is the header file of hal_card_platform_XX.c.
 *	   Every project has the same header file.
 *
 ***************************************************************************************************************/

#ifndef __HAL_CARD_PLATFORM_H
#define __HAL_CARD_PLATFORM_H

#include "hal_card_regs.h"

#define SLOT_NUM  1

typedef enum
{
	EV_PAD1,
	EV_PAD2,
	EV_PAD3,
	EV_PAD4,
	EV_PAD5,
	EV_PAD6,
	EV_NPAD,

} PADEmType;

typedef enum
{
	EV_PULLDOWN,
	EV_PULLUP,

} PinPullEmType;

typedef enum
{
	EV_SD_CMD_DAT0_3,
	EV_SD_CDZ,
	EV_SD_WP,

} PinEmType;

typedef enum
{
	EV_NORVOL        = 0,
	EV_MINVOL        = 1,
	EV_LOWVOL        = 2,
	EV_HIGHVOL       = 3,
    EV_MAXVOL        = 4,

} PADVddEmType;

typedef enum
{
	EV_VDD_DUMMY     = 0,
	EV_VDD_165_195   = BIT07,
	EV_VDD_20_21     = BIT08,
	EV_VDD_21_22     = BIT09,
	EV_VDD_22_23     = BIT10,
	EV_VDD_23_24     = BIT11,
	EV_VDD_24_25     = BIT12,
	EV_VDD_25_26     = BIT13,
	EV_VDD_26_27     = BIT14,
	EV_VDD_27_28     = BIT15,
	EV_VDD_28_29     = BIT16,
	EV_VDD_29_30     = BIT17,
	EV_VDD_30_31     = BIT18,
	EV_VDD_31_32     = BIT19,
	EV_VDD_32_33     = BIT20,
	EV_VDD_33_34     = BIT21,
	EV_VDD_34_35     = BIT22,
	EV_VDD_35_36     = BIT23,
	EV_VDD_50        = BIT24,

} VddEmType;

BOOL_T Hal_CARD_Wait_Emmc_D0(void);
void Hal_CARD_InitFCIE(IPEmType eIP);
void Hal_CARD_InitSetting(IPEmType eIP);
void Hal_CARD_InitPADPin(PADEmType ePAD, BOOL_T bTwoCard);
void Hal_CARD_SetPADToPortPath(IPEmType eIP, PortEmType ePort, PADEmType ePAD, BOOL_T bTwoCard);
void Hal_CARD_SetPADPower(PADEmType ePAD, PADVddEmType ePADVdd);
void Hal_CARD_PullPADPin(PADEmType ePAD, PinEmType ePin, PinPullEmType ePinPull, BOOL_T bTwoCard);

U32_T Hal_CARD_FindClockSetting(IPEmType eIP, U32_T u32ReffClk, U8_T u8DownLevel);
void Hal_CARD_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet);

void Hal_CARD_PowerOn(PADEmType ePAD, VddEmType eVdd, U16_T u16DelayMs);
void Hal_CARD_PowerOff(PADEmType ePAD, U16_T u16DelayMs);
#if defined(CONFIG_MSTAR_SDMMC_GPIO_CDZ) && CONFIG_MSTAR_SDMMC_GPIO_CDZ
BOOL_T Hal_CARD_CardDetect_Gpio(void);
BOOL_T Hal_CARD_ProcCdInt(void);
void Hal_CARD_GPIO_IntEn(void);
void Hal_CARD_GPIO_SetPol(U8_T pol);
#endif
#endif //End of __HAL_CARD_PLATFORM_H







/***************************************************************************************************************
 *
 * FileName hal_card_base.h (for G2p)
 *     @author jeremy.wang (2010/10/14)
 * Desc:
 *	   This header file is independent.
 * 	   We could put different hal_card_base.h in different build code folder but the same name.
 *	   We want to distinguish between this and others settings but the same project.
 * 	   Specific Define is freedom for each project, but we need to put it at inner code of project define.
 *
 *	   For Base Setting:
 * 	   (1) BASE Specific Define for Each Project
 * 	   (2) BASE Project/OS/CPU Define
 * 	   (3) BASE Project/OS/CPU and Specific Option Define
 * 	   (4) BASE TYPE Define
 * 	   (5) BASE Debug System
 *
 * 	   P.S.	D_XX for define and D_XX__ two under line("__") to distinguish define and its define option.
 *
 ***************************************************************************************************************/

#ifndef __HAL_CARD_BASE_H
#define __HAL_CARD_BASE_H


//***********************************************************************************************************
// (1) BASE Specific Define for Each Project
//***********************************************************************************************************
//#define D_ICVER             D_ICVER_00
//#define D_BDVER             0

//***********************************************************************************************************
// (2) BASE Project/OS/CPU Define
//***********************************************************************************************************
#ifdef  CONFIG_MSTAR_EAGLE 
#define D_PROJECT	      D_PROJECT__EAGLE
#endif
#ifdef CONFIG_MSTAR_EDISON
#define D_PROJECT           D_PROJECT__EDISON
#endif
#define D_OS                D_OS__LINUX
#define D_CPU               D_CPU__ARM
#define D_PRODUCT           D_TV

//***********************************************************************************************************
// (3) BASE Project/OS/CPU and Specific Option Define
//***********************************************************************************************************

// Project Option Define
//-----------------------------------------------------------------------------------------------------------
#define D_PROJECT__CB2	  1
#define D_PROJECT__G2	  2
#define D_PROJECT__EAGLE  3
#define D_PROJECT__EIFFEL 4
#define D_PROJECT__EDISON 5

// OS Option
//-----------------------------------------------------------------------------------------------------------
#define D_OS__LINUX       1
#define D_OS__UBOOT       2
#define D_OS__WINCE       3
#define D_OS__EBOOT       4

// CPU Option
//-----------------------------------------------------------------------------------------------------------
#define D_CPU__ARM        1
#define D_CPU__MIPS       2

// Product Option Define
//-----------------------------------------------------------------------------------------------------------
#define D_PHONE           1
#define D_TV              2

// IC Version Option
//-----------------------------------------------------------------------------------------------------------
#define D_ICVER_00      0
#define D_ICVER_01      1
#define D_ICVER_02      2
#define D_ICVER_03      3
#define D_ICVER_04      4
#define D_ICVER_05      5
#define D_ICVER_06      6
#define D_ICVER_07      7
#define D_ICVER_08      8
#define D_ICVER_09      9

//***********************************************************************************************************
// (4) BASE Type Define
//***********************************************************************************************************

typedef unsigned char	BOOL_T;								  // 1 bytes
typedef unsigned char	U8_T;								  // 1 bytes
typedef unsigned short  U16_T;                                // 2 bytes
typedef unsigned int    U32_T;                                // 4 bytes
typedef unsigned long   ULONG_T;                              // 4 or 8 bytes
typedef signed char     S8_T;                                 // 1 byte
typedef signed short    S16_T;                                // 2 bytes
typedef signed int      S32_T;                                // 4 bytes
typedef signed long     SLONG_T;                              // 4 or 8 bytes
typedef unsigned int    U32;

#define FALSE	0
#define TRUE	1

#define BIT00 0x0001
#define BIT01 0x0002
#define BIT02 0x0004
#define BIT03 0x0008
#define BIT04 0x0010
#define BIT05 0x0020
#define BIT06 0x0040
#define BIT07 0x0080
#define BIT08 0x0100
#define BIT09 0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000

typedef enum
{
	EV_IP_FCIE1     = 0,
	EV_IP_FCIE2     = 1,

} IPEmType;

typedef enum
{
	EV_REG_BANK    = 0,
	EV_CIFC_BANK   = 1,
	EV_CIFD_BANK   = 2,

} IPBankEmType;

typedef enum
{
	EV_PORT_SD     = 0,
	EV_PORT_SDIO1  = 1,
	EV_PORT_SDIO2  = 2,
	EV_PORT_MS     = 3,
	EV_PORT_SMXD   = 4,
	EV_PORT_CF     = 5,
	EV_PORT_NANDT  = 6,

} PortEmType;

typedef enum
{
	EV_OK	= 0,
	EV_FAIL	= 1,

} RetEmType;



//***********************************************************************************************************
// (5) BASE Debug System
//***********************************************************************************************************

//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################

#include <linux/kernel.h>
#define prtstring(s)	printk(s)
#define prtUInt(v) 		printk("%u", v)
#define prtU8(v)		printk("0x%02X", v)
#define prtU8Hex(v)		printk("0x%02X", v)
#define prtU16Hex(v)	printk("0x%04X", v)
#define prtU32Hex(v)	printk("0x%08X", v)
//###########################################################################################################
#else
//###########################################################################################################
/*
extern void uart_write_string(U8 *ptr);
extern void uart_write_U8_T_hex(U8 c);
extern void uart_write_U32_T_hex(U32 val););

#define prtstring(s)	uart_write_string(s)
#define prtUInt(v) 		uart_write_U32_hex(v)
#define prtU8(v)		uart_write_U8_hex(v)
#define prtU8Hex(v)		uart_write_U8_hex(v)
#define prtU16Hex(v)	uart_write_U32_hex(v)
#define prtU32Hex(v)	uart_write_U32_hex(v)
*/

//###########################################################################################################
#endif



#endif //End of __HAL_CARD_BASE_H



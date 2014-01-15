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
#ifndef DRVNAND_H
#define DRVNAND_H

/*-------------------------------------------------------------------
                            GENERAL INCLUDES
--------------------------------------------------------------------*/


/*--------------------------------------------------------------------
                            LITERAL CONSTANTS
--------------------------------------------------------------------*/
//cached <-> unchched
#define KSEG02KSEG1(addr)			((void *)((U32)(addr)|0x80000000))
#define KSEG12KSEG0(addr)			((void *)((U32)(addr)&~0x80000000))

//virtual <-> physical
#define VA2PA(addr)					((void *)(((U32)addr) & 0x1fffffff))

#define DBG_NAND(x)					//(x)

#define MULTI_CE					0	//1 original is 1 and set to 0 for test on S5
#define MAX_CE_NO					1

#ifndef bool
#define bool int
#endif

#ifndef BOOL
#define BOOL int
#endif

#ifndef false
#define false 0
#define true (!false)
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

//===================================================================
// fcie register constant
//===================================================================
#define NFIE_OffShift				2
#define REG(addr)					(*(volatile U16 *)(addr))
#define HAL_WRITE_UINT16(addr, val)	(REG(addr) = val)
#define HAL_READ_UINT16(addr, val)	val = REG(addr)

#define SYS_BASE					0xBF200000
#define CHIPTOP_BASE				(SYS_BASE+(0xF00<<NFIE_OffShift))
#define NANDIE_BASE					(SYS_BASE+(0x180<<NFIE_OffShift))
#define reg_clk_nfie				(CHIPTOP_BASE+(0x1F<<NFIE_OffShift))
#define reg_allpad_in				(CHIPTOP_BASE+(0x50<<NFIE_OffShift))

#define NC_MIE_EVENT				(NANDIE_BASE+(0x00<<NFIE_OffShift))
#define NC_MIE_INT_EN				(NANDIE_BASE+(0x01<<NFIE_OffShift))
#define NC_MMA_PRI_REG				(NANDIE_BASE+(0x02<<NFIE_OffShift))
#define NC_MIU_DMA1					(NANDIE_BASE+(0x03<<NFIE_OffShift))
#define NC_MIU_DMA0					(NANDIE_BASE+(0x04<<NFIE_OffShift))
#define NC_NF_RBZ_STS				(NANDIE_BASE+(0x07<<NFIE_OffShift))
#define NC_INTR_TEST0				(NANDIE_BASE+(0x08<<NFIE_OffShift))
#define NC_INTR_TEST1				(NANDIE_BASE+(0x09<<NFIE_OffShift))
#define NC_MIE_PATH_CTL				(NANDIE_BASE+(0x0A<<NFIE_OffShift))
#define NC_JOB_BL_CNT				(NANDIE_BASE+(0x0B<<NFIE_OffShift))
#define NC_TR_BK_CNT				(NANDIE_BASE+(0x0C<<NFIE_OffShift))
#define RIU2NAND_STAT				(NANDIE_BASE+(0x1F<<NFIE_OffShift))
#define RIU2NAND_CTL				(NANDIE_BASE+(0x20<<NFIE_OffShift))
#define RIU2NAND_R					(NANDIE_BASE+(0x21<<NFIE_OffShift))
#define RIU2NAND_W					(NANDIE_BASE+(0x22<<NFIE_OffShift))
#define NC_CIF_FIFO_CTL				(NANDIE_BASE+(0x25<<NFIE_OffShift))
#define NC_MIU_OFFSET				(NANDIE_BASE+(0x2E<<NFIE_OffShift))
#define NC_TEST_MODE				(NANDIE_BASE+(0x30<<NFIE_OffShift))
#define NC_CELL_DELAY				(NANDIE_BASE+(0x37<<NFIE_OffShift))
#define NC_MIU_WPRT_L1				(NANDIE_BASE+(0x38<<NFIE_OffShift))
#define NC_MIU_WPRT_L0				(NANDIE_BASE+(0x39<<NFIE_OffShift))
#define NC_MIU_WPRT_H1				(NANDIE_BASE+(0x3A<<NFIE_OffShift))
#define NC_MIU_WPRT_H0				(NANDIE_BASE+(0x3B<<NFIE_OffShift))
#define NC_MIU_WPRT_ER1				(NANDIE_BASE+(0x3C<<NFIE_OffShift))
#define NC_MIU_WPRT_ER0				(NANDIE_BASE+(0x3D<<NFIE_OffShift))
#define NC_FCIE2_VERSION			(NANDIE_BASE+(0x3F<<NFIE_OffShift))
#define NC_CFG0						(NANDIE_BASE+(0x40<<NFIE_OffShift))
#define NC_CFG1						(NANDIE_BASE+(0x41<<NFIE_OffShift))
#define NC_STAT						(NANDIE_BASE+(0x42<<NFIE_OffShift))
#define NC_AUXR_ADR					(NANDIE_BASE+(0x43<<NFIE_OffShift))
#define NC_AUXR_DAT					(NANDIE_BASE+(0x44<<NFIE_OffShift))
#define NC_CTRL						(NANDIE_BASE+(0x45<<NFIE_OffShift))
#define NC_ST_READ					(NANDIE_BASE+(0x46<<NFIE_OffShift))
#define NC_ADDEND					(NANDIE_BASE+(0x4A<<NFIE_OffShift))

#define CIFIFO_CMD_ADDR				(NANDIE_BASE+(0x60<<NFIE_OffShift))


// fcie register constant
//============================================
//NC_MIE_EVENT:offset 0x0
//============================================
#define R_NC_RIU2NAND_END			0x0400
#define R_NC_JOB_END				0x0200
#define R_MIU_WP_ERR				0x0100
#define R_NC_MMA_DATA_END			0x0001

//============================================
//MIE_INT_EN:offset 0x1
//============================================
#define R_NC_RIU2NAND_ENDE			0x0400
#define R_NC_JOB_ENDE				0x0200
#define R_MIU_WP_ERRE				0x0100
#define R_NC_MMA_DATA_ENDE			0x0001

//============================================
//NC_MMA_PRI_REG:offset 0x2
//============================================
#define R_FIFO_CLKRDY				0x0020
#define R_NC_JOB_RW_DIR				0x0004
#define R_MMA_W_PRIORITY			0x0002
#define R_MMA_R_PRIORITY			0x0001

//============================================
//NC_MIU_DMA1:offset 0x3
//============================================
#define R_MIU1_SELECT				0x0100

//============================================
//NC_MIE_PATH_CTL:offset 0xA
//============================================
#define R_NC_EN						0x0020
#define R_MMA_ENABLE				0x0001

//============================================
//RIU2NAND_STAT:offset 0x1F
//============================================
#define R_DMA_RD4_NFC				0x0000
#define R_RIU2NAND_DIR				0x0001	// 0: RIU2NAND, 1: NAND2RIU

//============================================
//RIU2NAND_CTL:offset 0x20
//============================================
#define R_RIU2NAND_EN				0x0001
#define R_RIU2NAND_R_STR			0x0002
#define R_RIU2NAND_R_EN				0x0004
#define R_RIU2NAND_R_END			0x0008
#define R_RIU2NAND_W_STR			0x0010
#define R_RIU2NAND_W_EN				0x0020
#define R_RIU2NAND_W_END			0x0040

//============================================
//TEST_MODE:offset 0x30
//============================================
#define R_NFIE_SOFT_RESETZ			0x1000

//============================================
//NC_CFG0:offset 0x40
//============================================
#define R_NC_DYNGATED_ON			0x2000
#define R_NC_ECCERR_NSTOP			0x1000
#define R_NC_WP_AUTO				0x0800
#define R_NC_WP_EN					0x0400
#define R_NC_REDU_BYPASS			0x0200
#define R_NC_CHK_RB_HIGH			0x0100
#define R_NC_ONECOLADR				0x0080	// one column address
#define R_NC_CE_AUTO				0x0008
#define R_NC_CE_EN					0x0004
#define R_NC_DSIZE_SEL				0x0002	// 0:512, 1:512+16
#define R_NC_PSIZE_SEL				0x0001	// 0:512+16, 1:2048+64

//============================================
//NC_STAT:offset 0x42
//============================================
#define R_NC_ECC_FLAG1				0x0008
#define R_NC_ECC_FLAG0				0x0004

//============================================
//NC_CTRL:offset 0x45
//============================================
#define R_NC_SER_PART_MODE			0x0080
#define R_NC_ECC_BYPASS				0x0010
#define R_NC_DIR_DSTREAM			0x0008
#define R_NC_CIFC_ACCESS			0x0002
#define R_NC_JOB_START				0x0001

//============================================
//REG0008:offset 0x08
//============================================
#define NC_CLK_SEL3					0x0020
#define NC_CLK_SEL2					0x0010
#define NC_CLK_SEL1					0x0008
#define NC_CLK_SEL0					0x0004
#define R_INVERT_CLOCK				0x0002
#define R_DISABLE_CLOCK				0x0001
#define NC_CLK_MASK					(NC_CLK_SEL3 | NC_CLK_SEL2 | NC_CLK_SEL1 | NC_CLK_SEL0)
#define NC_CLK_300K					(0)
#define NC_CLK_750K					(NC_CLK_SEL0)
#define NC_CLK_5M					(NC_CLK_SEL1)
#define NC_CLK_13M					(NC_CLK_SEL1+NC_CLK_SEL0)
#define NC_CLK_18M					(NC_CLK_SEL2)
#define NC_CLK_22M					(NC_CLK_SEL2+NC_CLK_SEL0)
#define NC_CLK_27M					(NC_CLK_SEL2+NC_CLK_SEL1)
#define NC_CLK_32M					(NC_CLK_SEL2+NC_CLK_SEL1+NC_CLK_SEL0)
#define NC_CLK_43M					(NC_CLK_SEL3)
#define NC_CLK_54M					(NC_CLK_SEL3+NC_CLK_SEL0)
#define NC_CLK_72M					(NC_CLK_SEL3+NC_CLK_SEL1)
#define NC_CLK_SSC					(NC_CLK_SEL3+NC_CLK_SEL1+NC_CLK_SEL0)
#define NC_CLK_86M					(NC_CLK_SEL3+NC_CLK_SEL2)

//============================================
//  AUX Reg Address
//============================================
#define AUXADR_USERCMD				0x08
#define AUXADR_ADRSET				0x0B
#define AUXADR_INSTQUE				0x20
#define AUXADR_RPTCNT				0x18
#define AUXADR_RAN_CNT				0x19
#define AUXADR_RAN_OFFSET			0x1A
#define AUXADR_STATUS_MASK			0x1B
#define AUXADR_WAIT_IDLE_CNT		0x1C

//============================================
//  OP_CODE
//============================================
#define OP_CODE_CMD					0x00
#define OP_CODE_ADDR				0x40
#define OP_CODE_ACT					0x80

//============================================
//  OP_CODE: command
//============================================
#define OP_CMD_WIRE					0x00
#define OP_CMD_REG					0x10

#define OP_CMD00_READ				0x00
#define OP_CMD30_READ				0x01
#define OP_CMD35_READ4COPYBACK		0x02
#define OP_CMD90_READID				0x03
#define OP_CMDFF_RESET				0x04
#define OP_CMD80_SERIAL_DATA_INPUT	0x05
#define OP_CMD10_TRUE_PROGRAM		0x06
#define OP_CMD15_CACHE_PROGRAM		0x07
#define OP_CMD85_COPY_BACK_INPUT	0x08
#define OP_CMD85_RANDOM_DATA_INPUT	0x08
#define OP_CMD60_BLOCK_ERASE		0x09
#define OP_CMDD0_BLOCK_ERASE		0x0A
#define OP_CMD05_RANDOM_DATA_OUTPUT	0x0B
#define OP_CMDE0_RANDOM_DATA_OUTPUT	0x0C
#define OP_CMD70_READ_STATUS		0x0D
#define OP_CMD50_READ_RDD			0x0E
#define OP_CMD01_READHALF			0x0F
//============================================
//  OP_CODE: address
//============================================
// CYCLE
#define OP_ADR_2CYCLE				0x00
#define OP_ADR_3CYCLE				0x10
#define OP_ADR_4CYCLE				0x20
#define OP_ADR_5CYCLE				0x30
//TYPE
#define OP_ADR_COL_ADDR				0x00
#define OP_ADR_ROW_ADDR				0x04
#define OP_ADR_FULL_ADDR			0x08
#define OP_ADR_ADDR_1CYC			0x0C
//SELECT
#define OP_ADR_SET0					0x00
#define OP_ADR_SET1					0x01
#define OP_ADR_SET2					0x02
#define OP_ADR_SET3					0x03

#define OP_ADR_C5TFS0				(OP_CODE_ADDR+OP_ADR_5CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET0)
#define OP_ADR_C5TFS1				(OP_CODE_ADDR+OP_ADR_5CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET1)
#define OP_ADR_C5TFS2				(OP_CODE_ADDR+OP_ADR_5CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET2)
#define OP_ADR_C5TFS3				(OP_CODE_ADDR+OP_ADR_5CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET3)
#define OP_ADR_C4TFS0				(OP_CODE_ADDR+OP_ADR_4CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET0)
#define OP_ADR_C4TFS1				(OP_CODE_ADDR+OP_ADR_4CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET1)
#define OP_ADR_C4TFS2				(OP_CODE_ADDR+OP_ADR_4CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET2)
#define OP_ADR_C4TFS3				(OP_CODE_ADDR+OP_ADR_4CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET3)
#define OP_ADR_C3TFS0				(OP_CODE_ADDR+OP_ADR_3CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET0)
#define OP_ADR_C3TFS1				(OP_CODE_ADDR+OP_ADR_3CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET1)
#define OP_ADR_C3TFS2				(OP_CODE_ADDR+OP_ADR_3CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET2)
#define OP_ADR_C3TFS3				(OP_CODE_ADDR+OP_ADR_3CYCLE+OP_ADR_FULL_ADDR+OP_ADR_SET3)

#define OP_ADR_C5TRS0				(OP_CODE_ADDR+OP_ADR_5CYCLE+OP_ADR_ROW_ADDR+OP_ADR_SET0)
#define OP_ADR_C5TRS1				(OP_CODE_ADDR+OP_ADR_5CYCLE+OP_ADR_ROW_ADDR+OP_ADR_SET1)
#define OP_ADR_C4TRS0				(OP_CODE_ADDR+OP_ADR_4CYCLE+OP_ADR_ROW_ADDR+OP_ADR_SET0)
#define OP_ADR_C4TRS1				(OP_CODE_ADDR+OP_ADR_4CYCLE+OP_ADR_ROW_ADDR+OP_ADR_SET1)
#define OP_ADR_C3TRS0				(OP_CODE_ADDR+OP_ADR_3CYCLE+OP_ADR_ROW_ADDR+OP_ADR_SET0)
#define OP_ADR_C3TRS1				(OP_CODE_ADDR+OP_ADR_3CYCLE+OP_ADR_ROW_ADDR+OP_ADR_SET1)
#define OP_ADR_C2TRS0				(OP_CODE_ADDR+OP_ADR_2CYCLE+OP_ADR_ROW_ADDR+OP_ADR_SET0)
#define OP_ADR_C2TRS1				(OP_CODE_ADDR+OP_ADR_2CYCLE+OP_ADR_ROW_ADDR+OP_ADR_SET1)

#define OP_ADR_C4TCS0				(OP_CODE_ADDR+OP_ADR_4CYCLE+OP_ADR_COL_ADDR+OP_ADR_SET0)
#define OP_ADR_C2TCS0				(OP_CODE_ADDR+OP_ADR_2CYCLE+OP_ADR_COL_ADDR+OP_ADR_SET0)
//Address
#define ADR_C4TCS0					0x60
#define ADR_C2TRS0					0x44
#define ADR_C4TFS0					0x68
#define ADR_C2TIS0					0x4C

//============================================
//  OP_CODE: Action
//============================================
//TYPE
#define OP_ACT_SYSTEM				0x00
#define OP_ACT_DMA_BUF_IO			0x10
#define OP_ACT_QUEUE_CTRL			0x20
#define OP_ACT_REPEAT				0x30

//SELECT
#define OP_ACT_WAIT_READY			0x00
#define OP_ACT_CHK_STATUS			0x01
#define OP_ACT_WAIT_IDLE			0x02
#define OP_ACT_WAIT_MMA				0x03
#define OP_ACT_BREAK				0x08

#define OP_ACT_SERIAL_DATA_OUT		0x00
#define OP_ACT_RANDOM_DATA_OUT		0x01
#define OP_ACT_WRITE_RDD			0x02
#define OP_ACT_SERIAL_DATA_IN		0x08
#define OP_ACT_RANDOM_DATA_IN		0x09
#define OP_ACT_READ_RDD				0x0A

#define OP_ACT_PAGECOPY_US			0x00
#define OP_ACT_PAGECOPY_DS			0x01

#define OP_ACT_INSTRUCT_ADDR		0x00

#define OP_A_WAIT_READY_BUSY		(OP_CODE_ACT+OP_ACT_SYSTEM+OP_ACT_WAIT_READY)
#define OP_A_CHECK_STATUS			(OP_CODE_ACT+OP_ACT_SYSTEM+OP_ACT_CHK_STATUS)
#define OP_A_WAIT_IDLE				(OP_CODE_ACT+OP_ACT_SYSTEM+OP_ACT_WAIT_IDLE)
#define OP_A_WAIT_MMA				(OP_CODE_ACT+OP_ACT_SYSTEM+OP_ACT_WAIT_MMA)
#define OP_A_BREAK					(OP_CODE_ACT+OP_ACT_SYSTEM+OP_ACT_BREAK)

#define OP_A_SERIAL_DATA_OUT		(OP_CODE_ACT+OP_ACT_DMA_BUF_IO+OP_ACT_SERIAL_DATA_OUT)
#define OP_A_RANDOM_DATA_OUT		(OP_CODE_ACT+OP_ACT_DMA_BUF_IO+OP_ACT_RANDOM_DATA_OUT)
#define OP_A_WRITE_RDD				(OP_CODE_ACT+OP_ACT_DMA_BUF_IO+OP_ACT_WRITE_RDD)
#define OP_A_SERIAL_DATA_IN			(OP_CODE_ACT+OP_ACT_DMA_BUF_IO+OP_ACT_SERIAL_DATA_IN)
#define OP_A_RANDOM_DATA_IN			(OP_CODE_ACT+OP_ACT_DMA_BUF_IO+OP_ACT_RANDOM_DATA_IN)
#define OP_A_READ_RDD				(OP_CODE_ACT+OP_ACT_DMA_BUF_IO+OP_ACT_READ_RDD)

#define OP_A_PAGECOPY_US			(OP_CODE_ACT+OP_ACT_QUEUE_CTRL+OP_ACT_PAGECOPY_US)
#define OP_A_PAGECOPY_DS			(OP_CODE_ACT+OP_ACT_QUEUE_CTRL+OP_ACT_PAGECOPY_DS)

#define OP_A_REPEAT					(OP_CODE_ACT+OP_ACT_REPEAT)

//============================================
// NAND Flash capacity
//============================================
#define NAND_DISK_4M				0x1F3F
#define NAND_DISK_8M				0x3E7F
#define NAND_DISK_16M				0x7CFF
#define NAND_DISK_32M				0xF9FF
#define NAND_DISK_64M				0x1F3FF
#define NAND_DISK_128M				0x3E7FF
#define NAND_DISK_256M				0x7CFFF
#define NAND_DISK_512M				0xF9FFF
#define NAND_DISK_1G				0x1F3FFF
#define NAND_DISK_2G				0x3E7FFF

/*--------------------------------------------------------------------
                                 TYPES
--------------------------------------------------------------------*/
#define U8							unsigned char
#define U16							unsigned short int
#define U32							unsigned long int

#define MIUWIDTH					8

typedef enum EFLASHCONFIG
{
    FLASH_UNCONFIG =				0x0,
    FLASH_READY =					0x01,
    FLASH_WP =						0x02,
    FLASH_2KPAGE =					0x04,
    FLASH_16BIT =					0x08,
    FLASH_CACHEPROGRAM =			0x20,
}FLASHCONFIG;

typedef enum EFLASHTYPE
{
    TYPE_SLC =						0,
    TYPE_MLC,
    TYPE_AGAND
}FLASHTYPE;

typedef enum EFLASHPLANE
{
    ONE_PLANE =						0,
    TWO_PLANE,
    FOUR_PLANE,
    EIGHT_PLANE,
}FLASHPLANE;

typedef enum ENCRW_STATUS
{
    NCRW_STS_OK =					0,
    NCRW_STS_PROGRAM_FAIL =			0x2,
    NCRW_STS_ECC_ERROR =			0x4,
    NCRW_STS_BIT_FLIP = 0x5,
    NCRW_STS_WRITE_PROTECT =		0x8,
    NCRW_STS_TIMEOUT =				0x10,
}NCRW_STATUS;

typedef enum EMAKER_CODE
{
    SAMSUMG =						0xEC,
    TOSHIBA =						0x98,
    RENESAS =						0x07,
    HYNIX   =						0xAD,
    ST      =						0x20,
    FUJITSU	=						0x04,
    NATIONAL =						0x8F,
}MAKER_CODE;

typedef enum eHW_CONTROL
{
    HW_CLE = 0x01,
    HW_ALE = 0x02,
}HW_CONTROL;

typedef struct _NAND_FLASH_INFO
{
    U16 u16NC_CFG0;
    FLASHCONFIG eFlashConfig;
    #if MULTI_CE
    MAKER_CODE eMaker[MAX_CE_NO];
    U8 u8ID[MAX_CE_NO];
    #else
    MAKER_CODE eMaker;
    U8 u8ID;
    #endif
    U8 u8PagesPerBlock;
    U8 u8SecNumofBlk;
    U8 u8Pageshift;
    U8 u8AddrWidth;
    U16 u16BlocksPerCS;
    U32 u32Capacity;
    FLASHTYPE eflashtype;
    FLASHPLANE eflashplane;

    U32 u32PreSource[2];

    HW_CONTROL ehw_ctl;
    U8 u8AddressCnt;

}NAND_FLASH_INFO;


/*--------------------------------------------------------------------
                               VARIABLES
--------------------------------------------------------------------*/
extern NAND_FLASH_INFO _fsinfo;
extern volatile U32 *_pu32CIF_C;

/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/


/*--------------------------------------------------------------------
                                PROCEDURES
--------------------------------------------------------------------*/
void drvNAND_ClearNFIE_EVENT(void);
void drvNAND_SoftReset(void);
void drvNAND_FLASHRESET(void);
int drvNAND_FLASH_INIT(void);
void drvNAND_READ_ID(void);

NCRW_STATUS drvNAND_wNCJOBEND(bool wMMA_END);
NCRW_STATUS drvNAND_wNCMMAEND(void);
NCRW_STATUS drvNAND_wNCMMA_JOBEND(void);

U8 drvNAND_GetCMD_RSP_BUF(U8 u8addr);
void drvNAND_SetCMD_RSP_BUF(U8 u8addr, U8 u8value);

void drvNAND_ReadRdd(U32 u32Row, U16 u16Column);
void drvNAND_ReadPage(U32 u32Row,U16 u16Col);
void drvNAND_WritePage(U32 u32Row,U16 u16Col);

U8 MDrv_NAND_ReadByte(void);
NCRW_STATUS MDrv_NAND_ReadPage(U32 u32Row,U16 u16Column);
void MDrv_NAND_ReadBuf(U8 * const u8Buf,U16 u16Len);
U8 MDrv_NAND_Send_Read_Status_CMD(U8 u8Cmd);
void MDrv_NAND_Cmd_PageProgram(U8 u8Cmd);
void MDrv_NAND_WriteBuf(U8 * const u8Buf,U16 u16Len);
NCRW_STATUS MDrv_NAND_WritePage(U32 u32Row,U16 u16Column);
void MDrv_NAND_ReadOOB(U32 u32Row,U16 u16Column);
U8 MDrv_NAND_Set_Command_Latch(U8 u8Level);
U8 MDrv_NAND_Set_Address_Latch(U8 u8Level);
U8 MDrv_NAND_Set_Write_Protect(U8 u8Level);
NCRW_STATUS MDrv_NAND_SendCmd(U8 u8Cmd);
NCRW_STATUS MDrv_NAND_SendAdr(U32 u32Row,U16 u16Column);
NCRW_STATUS MDrv_NAND_SendCmdAdr(U8 u8Cmd,U32 u32Row,U16 u16Column);
U8 MDrv_NAND_WaitReady(void);
void MDrv_NAND_Erase1(U32 u32Row);
bool MDrv_NAND_Erase2(void);
U8 MDrv_NAND_Set_Chip_Select(U8 u8Level);
int MDrv_NAND_CheckECC(void);

#endif

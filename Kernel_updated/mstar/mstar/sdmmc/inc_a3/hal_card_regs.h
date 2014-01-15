/***************************************************************************************************************
 *
 * FileName hal_card_regs.h
 *     @author jeremy.wang (2010/10/14)
 * Desc:
 * 	   This file is the header file of hal_card_regs.c.
 *
 * 	   For Base RegisterSetting:
 * 	   (1) BASE Register Address
 *	   (2) BASE Register Operation
 * 	   (3) BASE FCIE Reg Meaning Position
 *
 *	   P.S. If you want to use only IP for single card or dual cards,
 * 			please modify FCIE1 and FCIE2 setting to the same reg position.
 *
 ***************************************************************************************************************/

#ifndef __HAL_CARD_REGS_H
#define __HAL_CARD_REGS_H

#include "hal_card_base.h"

//***********************************************************************************************************
// (1) BASE Register Address
//***********************************************************************************************************

//###########################################################################################################
#if (D_PROJECT == D_PROJECT__CB2)
//###########################################################################################################

#define A_RIU_BASE			(0xA0000000)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1000) 		// FCIE_0_BANK
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1080)     	// FCIE_1_BANK
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1100)     	// FCIE_2_BANK
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1180) 		// FCIE_3_BANK

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x0A00) 		// SDIO_0_BANK
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x0A80)     	// SDIO_1_BANK
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x0B00)     	// SDIO_2_BANK
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x0B80) 		// SDIO_3_BANK

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__G2)
//###########################################################################################################

#define A_RIU_BASE			(0xE5000000) //(0x25000000)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1400) 		// FCIE_0_BANK
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1480)     	// FCIE_1_BANK
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1500)     	// FCIE_2_BANK
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1580) 		// FCIE_3_BANK

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1600) 		// SDIO_0_BANK
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1680)     	// SDIO_1_BANK
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1700)     	// SDIO_2_BANK
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x1780) 		// SDIO_3_BANK

//###########################################################################################################
//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__A3)
//###########################################################################################################

#define A_RIU_PM_BASE   	(0x1F000000)
#define A_RIU_BASE			(0x1F200000)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x8980) 		// FCIE_0_BANK
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x89E0)     	// FCIE_1_BANK
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x8A00)     	// FCIE_2_BANK
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x8A80) 		// FCIE_3_BANK

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x0A00) 		// SDIO_0_BANK
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x0A80)     	// SDIO_1_BANK
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x0B00)     	// SDIO_2_BANK
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x0B80) 		// SDIO_3_BANK

//###########################################################################################################
#endif

//***********************************************************************************************************
// (2) BASE Register Operation
//***********************************************************************************************************

//###########################################################################################################
#if (D_PROJECT == D_PROJECT__CB2)
//###########################################################################################################
#include <asm/io.h>  //IO_ADDRESS		// IO Mapping Address
#define IO_MAPADDR(Reg_Addr)	IO_ADDRESS(Reg_Addr)
//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__G2)
//###########################################################################################################
#define IO_MAPADDR(Reg_Addr)	Reg_Addr
//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__A3)
//###########################################################################################################
#include <asm/io.h>  //IO_ADDRESS		// IO Mapping Address
#define IO_MAPADDR(Reg_Addr)	IO_ADDRESS(Reg_Addr)
//###########################################################################################################
#endif

#define D_MIU_WIDTH			8			// MIU Info
#define REG_OFFSET_BITS		2			// Register Offset Byte (32bit) = 4Bytes
#define GET_CARD_REG_ADDR(x, y)        ((x)+((y) << REG_OFFSET_BITS))

#define CARD_REG(Reg_Addr)             (*(volatile U16_T*)(IO_MAPADDR(Reg_Addr)))
#define CARD_REG_W16(Reg_Addr,Value)   *((volatile U16_T*)(IO_MAPADDR(Reg_Addr))) = (Value)
#define CARD_REG_R16(Reg_Addr)         *((volatile U16_T*)(IO_MAPADDR(Reg_Addr)))
#define CARD_REG_W32(Reg_Addr,Value)   *((volatile U32_T*)(IO_MAPADDR(Reg_Addr))) = (Value)
#define CARD_REG_R32(Reg_Addr)         *((volatile U32_T*)(IO_MAPADDR(Reg_Addr)))
#define CARD_REG16_ON(Reg_Addr, Value) CARD_REG_W16(Reg_Addr, CARD_REG_R16(Reg_Addr)|(Value))
#define CARD_REG16_OFF(Reg_Addr,Value) CARD_REG_W16(Reg_Addr, CARD_REG_R16(Reg_Addr)&(~(Value)))
#define CARD_BANK(Bank_Addr)           IO_MAPADDR(Bank_Addr)

volatile U32_T Hal_CREG_GET_REG_BANK(IPEmType eIP, IPBankEmType eBANK);
void Hal_CREG_SET_PORT(IPEmType eIP, PortEmType ePort);
volatile PortEmType Hal_CREG_GET_PORT(IPEmType eIP);

#define GET_CARD_BANK           Hal_CREG_GET_REG_BANK
#define GET_CARD_PORT           Hal_CREG_GET_PORT
#define SET_CARD_PORT           Hal_CREG_SET_PORT

//***********************************************************************************************************
// (3) BASE FCIE Reg Meaning Position
//***********************************************************************************************************

//============================================
//MIE_EVENT:offset 0x00
//============================================
#define R_MMA_DATA_END          BIT00
#define R_SD_CMD_END            BIT01
#define R_SD_DATA_END           BIT02
#define R_MS_DATA_END           BIT03
#define R_CF_CMD_END            BIT04
#define R_CF_DATA_END           BIT05
#define R_SM_JOB_END            BIT06
#define R_SDIO_INT              BIT07
#define R_MIU_WP_ERR            BIT08
#define R_NC_JOB_END            BIT09
#define R_NC_RIU2NAND_END       BIT10
#define R_CARD_DMA_END          BIT11
#define R_NC_R2N_ECC_ERR        BIT12
#define R_PWR_SAVE_END          BIT13
#define R_MMA_LSDONE_END		BIT14
#define R_BOOT_DONE_END			BIT15  //NEW

//============================================
//MIE_INT_EN:offset 0x01
//============================================
#define R_MMA_DATA_END_EN       BIT00
#define R_SD_CMD_END_EN         BIT01
#define R_SD_DATA_END_EN        BIT02
#define R_MS_DATA_END_EN        BIT03
#define R_CF_CMD_END_EN         BIT04
#define R_CF_DATA_END_EN        BIT05
#define R_SM_JOB_END_EN         BIT06
#define R_SDIO_INT_EN           BIT07
#define R_MIU_WP_ERR_EN         BIT08
#define R_NC_JOB_END_EN         BIT09
#define R_NC_RIU2NAND_EN    	BIT10
#define R_CARD_DMA_END_EN   	BIT11
#define R_PWR_SAVE_END_EN   	BIT13
#define R_MMA_LSDONE_END_EN     BIT14
#define R_BOOT_DONE_END_EN      BIT15  //NEW

//============================================
//MMA_PRIORITY:offset 0x02
//============================================
#define R_MMA_R_PRIORITY        BIT00
#define R_MMA_W_PRIORITY        BIT01
#define R_JOB_RW_DIR            BIT02
#define R_DATA_SCRAM_EN         BIT03
#define R_MIU_REQ_RST           BIT04
#define R_FIFO_CLK_RDY          BIT05
#define R_MIU_BUS_BURST16       BIT08
#define R_MIU_BUS_BURST32       BIT09
#define R_MIU_BUS_CTRL_EN       BIT10
#define R_MIU_CLK_EN_SW         BIT12  //NEW
#define R_MIU_CLK_EN_HW         BIT13  //NEW
#define R_MIU_CLK_CTL_SW        BIT14  //NEW
#define R_MIU_CLK_FREE          BIT15  //NEW

//============================================
//CARD_EVENT:offset 0x05
//============================================
#define R_SD_STS_CHG            BIT00
#define R_MS_STS_CHG            BIT01
#define R_CF_STS_CHG            BIT02
#define	R_SM_STS_CHG            BIT03
#define	R_XD_STS_CHG            BIT04
#define R_SD_PWR_OC_CHG         BIT05
#define R_CF_PWR_OC_CHG         BIT06
#define R_SDIO_STS_CHG          BIT07
#define R_SDIO2_STS_CHG         BIT08

//============================================
//CARD_INT_EN:offset 0x06
//============================================
#define R_SD_STS_EN             BIT00
#define R_MS_STS_EN             BIT01
#define R_CF_STS_EN             BIT02
#define	R_SM_STS_EN             BIT03
#define	R_XD_STS_EN             BIT04
#define R_SD_PWR_OC_DET_EN      BIT05
#define R_CF_PWR_OC_DET_EN      BIT06
#define R_SD_CD_SRC             BIT07 // 0: SD_CDZ, 1: SD_DAT3
#define R_SDIO_STS_EN           BIT08
#define R_SDIO_CD_SRC           BIT09
#define R_SDIO2_STS_EN          BIT10

//============================================
//CARD_DET:offset 0x07
//============================================
#define R_SD_DET_N              BIT00
#define R_MS_DET_N              BIT01
#define R_CF_DET_N              BIT02
#define	R_SM_DET_N              BIT03
#define	R_XD_DET_N              BIT04
#define R_SD_OCDET_STS          BIT05
#define R_CF_OCDET_STS          BIT06
#define R_SDIO_DET_N            BIT07
#define R_NF_RDBZ_STS           BIT08 // C3
#define R_SDIO2_DET_N           BIT09

//============================================
//CARD_PWR:offset 0x08
//============================================
#define R_SD_PWR_ON             BIT00
#define R_CF_PWR_ON             BIT01
#define R_SD_PWR_PAD_OEN        BIT02
#define	R_CF_PWR_PAD_OEN        BIT03
#define R_RIU2NAND_JOB_END_ACT  BIT06 // C3 for test
#define R_NC_JOB_END_ACT        BIT07 // C3 for test

//============================================
//INTR_TEST:offset 0x09 all for test
//============================================
#define R_FORCE_MMA_DATA_END    BIT00
#define R_FORCE_SD_CMD_END      BIT01
#define R_FORCE_SD_DATA_END     BIT02
#define R_FORCE_MS_DATA_END     BIT03
#define R_FORCE_CF_REQ_END      BIT04
#define R_FORCE_CF_DATA_END     BIT05
#define R_FORCE_SM_JOB_END      BIT06
#define R_FORCE_NC_JOB_END      BIT06 // for backward compatible
#define R_FORCE_SDIO_INT        BIT07
#define R_FORCE_SD_STS_CHG      BIT08
#define R_FORCE_MS_STS_CHG      BIT09
#define R_FORCE_CF_STS_CHG      BIT10
#define R_FORCE_SM_STS_CHG      BIT11
#define R_FORCE_XD_STS_CHG      BIT12
#define R_FORCE_SD_OC_STS_CHG   BIT13
#define R_FORCE_CF_OC_STS_CHG   BIT14
#define R_FORCE_SDIO_STS_CHG    BIT15

//============================================
//MIE_PATH_CTL:offset 0x0A
//============================================
#define R_MMA_ENABLE            BIT00
#define R_SD_EN                 BIT01
#define R_MS_EN                 BIT02
#define R_CF_EN                 BIT03
#define R_SM_EN                 BIT04
#define R_NC_EN                 BIT05

//============================================
//JOB_BL_CNT:offset 0xB
//============================================
#define R_JOB_BL_CNT0   BIT00
#define R_JOB_BL_CNT1   BIT01
#define R_JOB_BL_CNT2   BIT02
#define R_JOB_BL_CNT3   BIT03
#define R_JOB_BL_CNT4   BIT04
#define R_JOB_BL_CNT5   BIT05
#define R_JOB_BL_CNT6   BIT06
#define R_JOB_BL_CNT7   BIT07

//============================================
//TR_BK_CNT:offset 0xC
//============================================
#define R_TR_BK_CNT0   	BIT00
#define R_TR_BK_CNT1   	BIT01
#define R_TR_BK_CNT2    BIT02
#define R_TR_BK_CNT3    BIT03
#define R_TR_BK_CNT4    BIT04
#define R_TR_BK_CNT5    BIT05
#define R_TR_BK_CNT6    BIT06
#define R_TR_BK_CNT7    BIT07

//============================================
//SD_MODE:offset 0x10
//============================================
#define R_SD_CLK_EN             BIT00
#define R_SD_4BITS              BIT01
#define R_SD_8BITS              BIT02
#define R_SDDRL                 BIT03
#define R_SD_CS_EN              BIT04
#define R_SD_DEST               BIT05 // 0: Data FIFO, 1: CIF FIFO
#define R_SD_DATSYNC            BIT06
#define R_MMC_BUS_TEST          BIT07
#define R_SDIO_RDWAIT           BIT08
#define R_SDIO_SD_BUS_SW        BIT09
#define R_SD_DMA_RD_CLK_STOP	BIT11
#define R_SDIO_PORT_SEL			BIT12

//============================================
//SD_CTL:offset 0x11
//============================================
#define R_SD_RSPR2_EN           BIT00 // Response R2 type
#define R_SD_RSP_EN             BIT01
#define R_SD_CMD_EN             BIT02
#define R_SD_DTRX_EN            BIT03
#define R_SD_DTRX_DIR           BIT04 // 0: R, 1:W
#define R_SDIO_INT_MOD0         BIT08
#define R_SDIO_INT_MOD1         BIT09
#define R_SDIO_DET_ON           BIT10

//============================================
//SD_STS:offset 0x12
//============================================
#define R_SD_DAT_CERR           BIT00
#define R_SD_DAT_STSERR         BIT01
#define R_SD_DAT_STSNEG         BIT02
#define R_SD_CMD_NORSP          BIT03
#define R_SD_CMDRSP_CERR        BIT04
#define R_SD_WR_PRO_N           BIT05
#define R_SD_DAT0               BIT08
#define R_SD_DAT1               BIT09
#define R_SD_DAT2               BIT10
#define R_SD_DAT3               BIT11
#define R_SD_DAT4               BIT12
#define R_SD_DAT5               BIT13
#define R_SD_DAT6               BIT14
#define R_SD_DAT7               BIT15

//============================================
//MS_CTL:offset 0x14
//============================================
#define R_MS_REGTRX_EN          BIT00
#define R_MS_DTRX_EN            BIT01
#define R_MS_BUS_DIR            BIT02 // 0: read form card, 1: Write to card
#define R_MS_BURST              BIT03
#define R_MS_DEST               BIT04

//============================================
//CF_CTL:offset 0x1A
//============================================
#define R_CF_REG_EN             BIT00
#define R_CF_DTRX_EN            BIT01
#define R_CF_BUS_DIR            BIT02
#define R_CF_DEST               BIT03 // 0: Data FIFO, 1: CIF FIFO
#define R_CF_BURST              BIT04
#define R_CF_IORDY_DIS          BIT05
#define R_CF_BURST_ERR          BIT06

//============================================
//SDIO_CTL:offset 0x1B
//============================================
#define R_SDIO_BLK_MOD          BIT15

//============================================
//SDIO_STS:offset 0x1E
//============================================
#define R_SDIO_DAT0             BIT00
#define R_SDIO_DAT1             BIT01
#define R_SDIO_DAT2             BIT02
#define R_SDIO_DAT3             BIT03

#define R_SDIO2_DAT0            BIT08
#define R_SDIO2_DAT1            BIT09
#define R_SDIO2_DAT2            BIT10
#define R_SDIO2_DAT3            BIT11

//============================================
//NC_CIF_FIFO_CTL:offset 0x25
//============================================
// only need in montage serious
#define R_CIFC_RD_REQ           BIT00
#define R_CIFD_RD_REQ           BIT01

//============================================
//SM_CTL:offset 0x2B
//============================================
#define R_SM_REG_EN             BIT00
#define R_SM_DTRX_EN            BIT01
#define R_SM_BUS_DIR            BIT02
#define R_SM_DEST               BIT03
#define R_SM_BURST              BIT04

//============================================
//NC_REORDER:offset 0x2D
//============================================
#define R_MIU_WDEN_PATCH_DIS	BIT07

//============================================
//TEST_MODE:offset 0x30
//============================================
#define R_DBFA_BISTFAIL         BIT00
#define R_DBFB_BISTFAIL         BIT01
#define R_CIFC_BISTFAIL         BIT02
#define R_CIFD_BISTFAIL         BIT03
#define R_PAD_SWAP              BIT07 // C3 only
#define R_DEBUG_MODE0           BIT08
#define R_DEBUG_MODE1           BIT09
#define R_DEBUG_MODE2           BIT10
#define R_SD_MS_COBUS           BIT11
#define R_FCIE_SOFT_RST         BIT12
#define R_NFIE_SOFT_RESETZ      BIT12 // For backward compatible
#define R_ENDIAN_SEL            BIT13

//============================================
//NC_MISC offser 0x31
//============================================
//============================================
//PWRSAVE_MASK    0x34
//============================================
//============================================
//RWRSAVE_CTL     0x35
//============================================
#define R_PWRSAVE_EN            BIT00
#define R_PWRSAVE_TEST          BIT01
#define R_PWRSAVE_INT_EN        BIT02
#define R_PWRSAVE_RST_N         BIT03 // low active
#define R_PWRSAVE_RIU_TEST      BIT05
#define R_PWRSAVE_HW_REST       BIT06
#define R_PWRSAVE_LOSS_PWR      BIT07

//============================================
//MIU_WP_MIN_AH_S :offset 0x38
//============================================
#define R_MIU_WP_EN             BIT08
#define R_FORCE_MIU_WP_ERR      BIT09
//============================================
//MIU_WP_MIN_AL   :offset 0x39
//============================================
//============================================
//MIU_WP_MAX_AH   :offset 0x3A
//============================================
//============================================
//MIU_WP_MAX_AL   :offset 0x3B
//============================================
//============================================
//MIU_WP_ERR_AH   :offset 0x3C
//============================================
//============================================
//MIU_WP_ERR_AL   :offset 0x3D
//============================================
//============================================
//FCIE_INFO       :offset 0x3F
//============================================
#define R_SDMMC_HW              BIT00
#define R_MS_HW                 BIT01
#define R_CF_HW                 BIT02
#define R_SM_HW                 BIT03
#define R_SDIO_HW               BIT04
#define R_NAND_HW               BIT05
#define R_MIU_BUS_WIDTH         (BIT06|WBIT07)
#define R_MIU_BUS_ALIGN_BYTE    BIT08
#define R_PROGRAM_PAD           BIT09
#define R_PROGRAM_CLK           BIT10




#endif //End of __HAL_CARD_REGS_H



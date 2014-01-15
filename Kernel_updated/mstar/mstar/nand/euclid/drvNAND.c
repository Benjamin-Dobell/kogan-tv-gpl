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
//--------------------------------------------------------------------
//                             GENERAL INCLUDE
//--------------------------------------------------------------------
#include <linux/delay.h>
#include <linux/string.h>
#include <asm/io.h>

#include "drvNAND.h"

//--------------------------------------------------------------------
//  LITERAL CONSTANTS
//--------------------------------------------------------------------
#define NAND_DMA_RACING_PATCH			1
#define NAND_MIU_CLK_PATCH				1

#ifdef NAND_DMA_RACING_PATCH
#define NAND_READ_MAGIC					0x74240319
#endif

//--------------------------------------------------------------------
//  VARIABLES
//--------------------------------------------------------------------

 //  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
const U8 u8MSTARFSTYPE[256] =
{ 	0,19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
	0,18, 0, 6, 0, 8,10, 0, 0,12, 0, 0, 0, 0, 0, 0, // 7
	0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, // 8
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C
	0, 13, 0,16, 0,17, 3, 0, 0, 0,15, 0,14, 0, 0, 0, // D
	0, 0, 0, 2, 0, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
	0,13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F
};

NAND_FLASH_INFO _fsinfo;

volatile U32 *_pu32CIF_C = (volatile U32 *)CIFIFO_CMD_ADDR;
U8 u8CIF_C_Index=0;
U16 u16ByteIdxofPage=0;
U8 _stPageBuffer[2048+64] __attribute((aligned(64)));
int gint_IS_ECC_Fail=0;

extern unsigned int MDrv_SYS_GetMIU0Length(void);

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_SWITCH_PAD Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
void drvNAND_SWITCH_PAD(U8 u8Type)
{
    U16 u16RegValue = 0;

    u8Type = u8Type;    // fixed compile warning

    HAL_READ_UINT16(CHIPTOP_BASE+(0x6F<<2), u16RegValue);
    u16RegValue |= (0x3<<4);
    HAL_WRITE_UINT16(CHIPTOP_BASE+(0x6F<<2), u16RegValue);

    HAL_READ_UINT16(reg_allpad_in, u16RegValue);
    u16RegValue &= ~(1<<15); /*disable all-pad-in  - hill modified*/
    HAL_WRITE_UINT16(reg_allpad_in, u16RegValue);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_Release_PAD Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
void drvNAND_Release_PAD(void)
{
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_ClearNFIE_EVENT Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
void drvNAND_ClearNFIE_EVENT(void)
{
    U16  u16Cnt=0xFFFF;
    U16 u16Tmp = (R_NC_JOB_END+R_NC_MMA_DATA_END);

    while( u16Tmp & (R_NC_JOB_END+R_NC_MMA_DATA_END))
    {
        HAL_WRITE_UINT16(NC_MIE_EVENT, u16Tmp);
        HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
        if((--u16Cnt)==0)
            break;
    }
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_SetNC_CLOCK Procedure
*
*   DESCRIPTION:
*
*   NOTE:
*
*********************************************************************/
void drvNAND_SetNC_CLOCK(U16 u16Param)
{
    U16 u16Tmp;

    DBG_NAND(printk("drvNAND_SetNC_CLOCK\r\n"));
    DBG_NAND(printk("set u16Param=0x%X\r\n",u16Param));
    HAL_READ_UINT16(reg_clk_nfie, u16Tmp);
    u16Tmp &= ~(NC_CLK_MASK+R_INVERT_CLOCK+R_DISABLE_CLOCK);
    u16Param &= NC_CLK_MASK;
    u16Tmp |= u16Param;
    HAL_WRITE_UINT16(reg_clk_nfie, u16Tmp);
    DBG_NAND(printk("set reg_clk_nfie=0x%X\r\n",u16Tmp));
}

#define NC_CFG0_RESET_VALUE    0x3
/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_SoftReset Procedure
*
*   DESCRIPTION:
*       wait NC_JOB_END event
*   NOTE:
*
*********************************************************************/
void drvNAND_SoftReset(void)
{
	U16 u16Tmp;

	DBG_NAND(printk("drvNAND_SoftReset\r\n"));
	DBG_NAND(printk("_fsinfo.u16NC_CFG0=0x%X\r\n",_fsinfo.u16NC_CFG0));

	HAL_WRITE_UINT16(NC_CFG0,0x00);
	HAL_WRITE_UINT16(NC_CFG1,0x00);
	HAL_WRITE_UINT16(NC_MIE_PATH_CTL,0);
	HAL_WRITE_UINT16(NC_CTRL,0);
	HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END+R_NC_MMA_DATA_END);

    HAL_READ_UINT16(NC_TEST_MODE, u16Tmp);
    HAL_WRITE_UINT16(NC_TEST_MODE,u16Tmp & (~R_NFIE_SOFT_RESETZ));

	udelay(1);

    HAL_WRITE_UINT16(NC_TEST_MODE,(u16Tmp | R_NFIE_SOFT_RESETZ));

	udelay(200);

	HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
	HAL_WRITE_UINT16(NC_CFG1,0x00);
	HAL_WRITE_UINT16(NC_CTRL,0);
	HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_NC_EN);

    #if 1 // only FCIE2 need
    // Hill Sung try to set TEST_MODE register again
    HAL_READ_UINT16(NC_TEST_MODE, u16Tmp);
    DBG_NAND(printk("NC_TEST_MODE: %X \r\n", u16Tmp));
    u16Tmp |= 0xC800;
    HAL_WRITE_UINT16(NC_TEST_MODE, u16Tmp);
    HAL_READ_UINT16(NC_TEST_MODE, u16Tmp);
    DBG_NAND(printk("TestMode: %Xh \r\n", u16Tmp));
    #endif

    DBG_NAND(printk("drvNAND_SoftReset end\r\n"));
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_wNCMMAEND Procedure
*
*   DESCRIPTION:
*       wait NC_MMA_END event
*   NOTE:
*
*********************************************************************/
NCRW_STATUS drvNAND_wNCMMAEND(void)
{
    U32 u32Count=0;
    U16 u16Tmp=0;

    while( !(u16Tmp & R_NC_MMA_DATA_END))
    {
        HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
        if(++u32Count==0x003FFFF)
        {
            DBG_NAND(printk("MMAEND time out\r\n"));
            break;
        }
    }
    HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_MMA_DATA_END); // clear R_NC_JOB_END
    u32Count = 0x00;
    do /*  While Looping_Free: Exit counter added*/
    {   // RIU sync issue, wait R_NC_JOB_END really clear
        HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
    }while((u16Tmp & R_NC_MMA_DATA_END) && (u32Count++<0x80));

    return(NCRW_STS_OK);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_wNCJOBEND Procedure
*
*   DESCRIPTION:
*       wait NC_JOB_END event
*   NOTE:
*
*********************************************************************/
NCRW_STATUS drvNAND_wNCJOBEND(bool wMMA_END)
{
	U16 u16Tmp=0;
	U32 u32Cnt=0;

	while( !(u16Tmp & R_NC_JOB_END))
	{
		HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
		if(++u32Cnt == 0x3FFFFFF)
		{
			DBG_NAND(diag_printf("Job End Time out\r\n"));
			HAL_READ_UINT16(NC_CFG0, u16Tmp);
			DBG_NAND(diag_printf("NC_CFG0=0x%X\r\n",u16Tmp));
			HAL_READ_UINT16(NC_CFG1, u16Tmp);
			DBG_NAND(diag_printf("NC_CFG1=0x%X\r\n",u16Tmp));
			HAL_READ_UINT16(NC_STAT, u16Tmp);
			DBG_NAND(diag_printf("NC_STAT=0x%X\r\n",u16Tmp));
			HAL_READ_UINT16(NC_CTRL, u16Tmp);
			DBG_NAND(diag_printf("NC_CTRL=0x%X\r\n",u16Tmp));
			HAL_READ_UINT16(NC_ST_READ, u16Tmp);
			DBG_NAND(diag_printf("NC_ST_READ=0x%X\r\n",u16Tmp));
			HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
			DBG_NAND(diag_printf("NC_MIE_EVENT=0x%X\r\n",u16Tmp));
			HAL_READ_UINT16(NC_JOB_BL_CNT, u16Tmp);
			DBG_NAND(diag_printf("NC_JOB_BL_CNT=0x%X\r\n",u16Tmp));
			HAL_READ_UINT16(NC_TR_BK_CNT, u16Tmp);
			DBG_NAND(diag_printf("NC_TR_BK_CNT=0x%X\r\n",u16Tmp));

			drvNAND_SoftReset();
			return(NCRW_STS_TIMEOUT);
		}
	}

	//diag_printf("Rcv JobEND\n");
	HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END); // clear R_NC_JOB_END
	u32Cnt = 0;
	HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
	while(u16Tmp & R_NC_JOB_END)
	{   // RIU sync issue, wait R_NC_JOB_END really clear
		HAL_READ_UINT16(NC_MIE_EVENT, u16Tmp);
		if(++u32Cnt == 0x3FFFFF)
		{
			DBG_NAND(diag_printf("Clear EVENT fail\r\n"));
			//while(1);
		}
		HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END);
	}

	if(wMMA_END)
	{
		HAL_READ_UINT16(NC_STAT, u16Tmp);
		if( (u16Tmp & R_NC_ECC_FLAG1) && (!(u16Tmp & R_NC_ECC_FLAG0)) )
		{
			DBG_NAND(diag_printf("NC_STAT=0x%X, SoftReset\r\n",u16Tmp));
			// TODO: remove debug code
			drvNAND_SoftReset();
			return(NCRW_STS_ECC_ERROR);
		}
              else if ((!(u16Tmp & R_NC_ECC_FLAG1)) && (u16Tmp & R_NC_ECC_FLAG0))
              {
                  drvNAND_wNCMMAEND();
                  return(NCRW_STS_BIT_FLIP);
              }
		else if(u16Tmp & R_NC_ECC_FLAG1)
		{
			drvNAND_wNCMMAEND();
			return(NCRW_STS_OK);
		}
		else
		{
			drvNAND_wNCMMAEND();
		}
	}
	return(NCRW_STS_OK);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_FLASHRESET Procedure
*
*   DESCRIPTION:
*       reset NAND flash
*   NOTE:
*
*********************************************************************/
void drvNAND_FLASHRESET(void)
{
	HAL_WRITE_UINT16(NC_MIE_EVENT,R_NC_JOB_END); // clear event first
	HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
	HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_WAIT_READY_BUSY<<8)+OP_CMDFF_RESET);
	HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_BREAK);
	HAL_WRITE_UINT16(NC_CTRL,R_NC_JOB_START);

	if(drvNAND_wNCJOBEND(false)!=NCRW_STS_OK)
		DBG_NAND(printk("Flash RESET FAIL\r\n"));
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_GetCMD_RSP_BUF Procedure
*
*   DESCRIPTION:
*       Get CIF_C byte value
*   NOTE:
*
*********************************************************************/
U8 drvNAND_GetCMD_RSP_BUF(U8 u8addr)
{
	U16 u16Tmp;

	u16Tmp = _pu32CIF_C[u8addr>>1];
	if(u8addr&0x1)
	{
		return(u16Tmp>>8);
	}
	else
	{
		return(u16Tmp&0xFF);
	}
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_SetCMD_RSP_BUF Procedure
*
*   DESCRIPTION:
*       Set CIF_C byte value
*   NOTE:
*
*********************************************************************/
void drvNAND_SetCMD_RSP_BUF(U8 u8addr, U8 u8value)
{
	U16 u16Tmp;

	u16Tmp = _pu32CIF_C[u8addr>>1] & 0xFFFF;

	if(u8addr & 1)
	{
		u16Tmp = ((U16)u8value<<8) + (u16Tmp&0xFF);
	}
	else
	{
		u16Tmp = (u16Tmp&0xFF00)+u8value;
	}
	_pu32CIF_C[u8addr>>1] = u16Tmp;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_CHECK_FLASH_TYPE Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
static void drvNAND_CHECK_FLASH_TYPE(void)
{
    if( (_fsinfo.eMaker != SAMSUMG) && (_fsinfo.eMaker != TOSHIBA) &&
        (_fsinfo.eMaker != RENESAS) && (_fsinfo.eMaker != HYNIX)  &&
        (_fsinfo.eMaker != ST) )
    {
        _fsinfo.u8ID = 0;
        _fsinfo.u16BlocksPerCS = 0;
        _fsinfo.u8PagesPerBlock = 0;
        _fsinfo.u8Pageshift = 0;
        _fsinfo.u8AddrWidth = 0;
        _fsinfo.eFlashConfig = FLASH_UNCONFIG;
        return; // unsupported flash maker
    }

    switch(u8MSTARFSTYPE[_fsinfo.u8ID])
    {
    case 0:
        _fsinfo.u8ID = 0;
        _fsinfo.u16BlocksPerCS = 0;
        _fsinfo.u8PagesPerBlock = 0;
        _fsinfo.u8Pageshift = 0;
        _fsinfo.u8AddrWidth = 0;
        _fsinfo.eFlashConfig = 0;
        return;
    case 1:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 2:
        _fsinfo.u16BlocksPerCS = 512;
        _fsinfo.u8PagesPerBlock = 16;
        _fsinfo.u8Pageshift = 4;
        _fsinfo.u32Capacity = NAND_DISK_4M;
        return;
    case 3:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 4:
        _fsinfo.u16BlocksPerCS = 1024;
        _fsinfo.u8PagesPerBlock = 16;
        _fsinfo.u8Pageshift = 4;
        _fsinfo.u32Capacity = NAND_DISK_8M;
        _fsinfo.u8AddrWidth = 3;
        return;
    case 5:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 6:
        _fsinfo.u16BlocksPerCS = 1024;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u32Capacity = NAND_DISK_16M;
        _fsinfo.u8AddrWidth = 3;
        return;
    case 7:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 8:
        _fsinfo.u16BlocksPerCS = 2048;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u32Capacity = NAND_DISK_32M;
        _fsinfo.u8AddrWidth = 3;
        return;
    case 9:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 10:
        _fsinfo.u16BlocksPerCS = 4096;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u32Capacity = NAND_DISK_64M;
        _fsinfo.u8AddrWidth = 4;
        return;
    case 11:
        _fsinfo.eFlashConfig |= FLASH_WP;
    case 12:
        _fsinfo.u16BlocksPerCS = 8192;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u32Capacity = NAND_DISK_128M;
        _fsinfo.u8AddrWidth = 4;
        return;
    case 13:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        _fsinfo.u16BlocksPerCS = 1024;
        _fsinfo.u8PagesPerBlock = 64;
        _fsinfo.u8Pageshift = 6;
        _fsinfo.u8AddrWidth = 4;
        _fsinfo.u32Capacity = NAND_DISK_128M;
        return;
    case 14:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        if( _fsinfo.eflashtype == TYPE_SLC )
        {
            if( _fsinfo.eflashplane == ONE_PLANE )
            {
                _fsinfo.u16BlocksPerCS = 4096;
                _fsinfo.u8PagesPerBlock = 64;
                _fsinfo.u8Pageshift = 6;
                _fsinfo.u32Capacity = NAND_DISK_512M;
            }
            else if( _fsinfo.eflashplane == TWO_PLANE )
            {
                _fsinfo.u16BlocksPerCS = 8192;
                _fsinfo.u8PagesPerBlock = 64;
                _fsinfo.u8Pageshift = 6;
                _fsinfo.u32Capacity = NAND_DISK_512M;
            }	
        }
        else if( (_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE) )
        {
            _fsinfo.u16BlocksPerCS = 2048;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
            _fsinfo.u32Capacity = NAND_DISK_512M;
        }
        _fsinfo.u8AddrWidth = 5;

        return;
    case 15:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        _fsinfo.u16BlocksPerCS = 2048;
        _fsinfo.u8PagesPerBlock = 64;
        _fsinfo.u8Pageshift = 6;
        _fsinfo.u8AddrWidth = 5;
        _fsinfo.u32Capacity = NAND_DISK_256M;
        return;
    case 16:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        //if((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eflashplane==TWO_PLANE))
        if((_fsinfo.eflashtype==TYPE_MLC) && (_fsinfo.eMaker==SAMSUMG))
        {
            _fsinfo.u16BlocksPerCS = 4096;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
        }
        else if((_fsinfo.eMaker==HYNIX) || (_fsinfo.eMaker==ST) )
        {
            _fsinfo.u16BlocksPerCS = 4096;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
        }
        else
        {
            _fsinfo.u16BlocksPerCS = 8192;
            _fsinfo.u8PagesPerBlock = 64;
            _fsinfo.u8Pageshift = 6;
        }
        _fsinfo.u8AddrWidth = 5;
        _fsinfo.u32Capacity = NAND_DISK_1G;
        return;
    case 17:
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        if(_fsinfo.eMaker==SAMSUMG)
        {
            _fsinfo.u16BlocksPerCS = 8192;
            _fsinfo.u8PagesPerBlock = 128;
            _fsinfo.u8Pageshift = 7;
        }
        else
        {
            _fsinfo.u16BlocksPerCS = 16384;
            _fsinfo.u8PagesPerBlock = 64;
            _fsinfo.u8Pageshift = 6;
        }
        _fsinfo.u8AddrWidth = 5;
        _fsinfo.u32Capacity = NAND_DISK_2G;
        return;
    case 18:
        _fsinfo.u16BlocksPerCS = 16384;
        _fsinfo.u8PagesPerBlock = 32;
        _fsinfo.u8Pageshift = 5;
        _fsinfo.u8AddrWidth = 4;
        _fsinfo.u32Capacity = NAND_DISK_256M;
        return;
    case 19:
        _fsinfo.eflashtype = TYPE_AGAND;
        _fsinfo.eFlashConfig |= FLASH_2KPAGE;
        _fsinfo.u16BlocksPerCS = 32768;
        _fsinfo.u8PagesPerBlock = 2;
        _fsinfo.u8Pageshift = 1;
        _fsinfo.u8AddrWidth = 4;
        _fsinfo.u32Capacity = NAND_DISK_128M;
        return;
    default:
        _fsinfo.u16BlocksPerCS = 0;
        _fsinfo.u8PagesPerBlock = 0;
        _fsinfo.u8Pageshift = 0;
        _fsinfo.u8AddrWidth = 0;
        _fsinfo.u32Capacity = 0;
        return;
    }
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_READ_ID Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
void drvNAND_READ_ID(void)
{
	U8   u8Tmp;
	u8CIF_C_Index = 0;
	drvNAND_FLASHRESET();

	_fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
	_fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
	HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

	HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
	HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00); // Address for Read ID

	HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
	HAL_WRITE_UINT16(NC_AUXR_DAT,(ADR_C2TIS0<<8)+OP_CMD90_READID);
	HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_A_BREAK<<8)+OP_A_RANDOM_DATA_IN);

	HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_RAN_CNT);

	HAL_WRITE_UINT16(NC_AUXR_DAT,0x6);  // read 5 bytes
	HAL_WRITE_UINT16(NC_AUXR_DAT,0x0);
	HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);

	drvNAND_wNCJOBEND(false);
	_fsinfo.u16NC_CFG0 &= ~(R_NC_CE_EN+R_NC_WP_EN);
	HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

	_fsinfo.eMaker = drvNAND_GetCMD_RSP_BUF(0);
	_fsinfo.u8ID = drvNAND_GetCMD_RSP_BUF(1);
	DBG_NAND(diag_printf("\n Flash Make code = 0x%x, ID = 0x%x\n",_fsinfo.eMaker,_fsinfo.u8ID));

	u8Tmp = drvNAND_GetCMD_RSP_BUF(2);
	if(_fsinfo.eMaker == SAMSUMG)
	{
		switch(u8Tmp & 0x0C)
		{
			case 0x00:  _fsinfo.eflashtype = TYPE_SLC;  break;
			case 0x04:
			case 0x08:
			case 0x0C:  _fsinfo.eflashtype = TYPE_MLC;  break;
		}
		switch(u8Tmp & 0x30)
		{
			case 0x00:  _fsinfo.eflashplane = ONE_PLANE;    break;
#ifdef MLC_2PLAN
			case 0x10:  _fsinfo.eflashplane = TWO_PLANE;    break;
			case 0x20:  _fsinfo.eflashplane = FOUR_PLANE;   break;
			case 0x30:  _fsinfo.eflashplane = EIGHT_PLANE;  break;
#endif
		}
	}
	else if((_fsinfo.eMaker == HYNIX) || (_fsinfo.eMaker == ST))
	{
		if( _fsinfo.u8ID == 0x76 )
		{
			_fsinfo.eflashtype = TYPE_SLC;
		}
		else
		{
			switch(u8Tmp & 0x0C)
			{
				case 0x00:  _fsinfo.eflashtype = TYPE_SLC;  break;
				case 0x04:  _fsinfo.eflashtype = TYPE_MLC;  break;
			}
			if(u8Tmp & 0x80)
			{
				DBG_NAND(diag_printf("Support Cache program\r\n"));
				_fsinfo.eFlashConfig |= FLASH_CACHEPROGRAM;
			}
			u8Tmp = drvNAND_GetCMD_RSP_BUF(3);
			if(u8Tmp & 0x40)
			{
				_fsinfo.eFlashConfig |= FLASH_16BIT;
			}
		}
	}

	drvNAND_CHECK_FLASH_TYPE();

	if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
	{
		_fsinfo.u16NC_CFG0 |= R_NC_PSIZE_SEL;
	}
	else
	{
		DBG_NAND(diag_printf("ONE COLUMN mode\r\n"));
		_fsinfo.u16NC_CFG0 |= R_NC_ONECOLADR;
	}

	HAL_WRITE_UINT16(NC_CFG0, _fsinfo.u16NC_CFG0);

	DBG_NAND(diag_printf("_fsinfo.u8SecNumofBlk=0x%X\r\n",_fsinfo.u8SecNumofBlk));
	HAL_WRITE_UINT16(NC_CIF_FIFO_CTL,0);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_ReadRdd Procedure
*
*   DESCRIPTION:
*       Read the first 6 bytes data from spare area
*   NOTE:
*
*********************************************************************/
void drvNAND_ReadRdd(U32 u32Row, U16 u16Column)
{
	HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);

	if( _fsinfo.eFlashConfig & FLASH_2KPAGE )
	{
		if(_fsinfo.eFlashConfig & FLASH_16BIT)
		{
			HAL_WRITE_UINT16(NC_AUXR_DAT, u16Column>>1);
		}
		else
		{
			HAL_WRITE_UINT16(NC_AUXR_DAT, u16Column);
		}
		HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
		HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
		HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
		if( _fsinfo.u8AddrWidth == 5)
		{
			HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C5TFS0<<8) + OP_CMD00_READ);
		}
		else if( _fsinfo.u8AddrWidth == 4)
		{
			HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS0<<8) + OP_CMD00_READ);
		}
		else
		{
			HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS0<<8) + OP_CMD00_READ);
		}
		HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_WAIT_READY_BUSY<<8) + OP_CMD30_READ);
		//HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_BREAK<<8) + OP_A_READ_RDD);
		HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_BREAK<<8) + OP_A_RANDOM_DATA_IN);
	}
	else
	{
		HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
		HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
		HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
		HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
		if( _fsinfo.u8AddrWidth == 4)
		{
			HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C4TFS0<<8)+OP_CMD50_READ_RDD);
		}
		else
		{
			HAL_WRITE_UINT16(NC_AUXR_DAT,(OP_ADR_C3TFS0<<8)+OP_CMD50_READ_RDD);
		}
		//HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_READ_RDD<<8)+OP_A_WAIT_READY_BUSY);
		HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_RANDOM_DATA_IN<<8)+OP_A_WAIT_READY_BUSY);
		HAL_WRITE_UINT16(NC_AUXR_DAT, (OP_A_BREAK<<8) + OP_A_BREAK);
	}

	HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_RPTCNT);
	HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
	HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_RAN_CNT);
	HAL_WRITE_UINT16(NC_AUXR_DAT, 0x10);
	HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
	HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
	drvNAND_wNCJOBEND(false);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_WritePage Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
void drvNAND_WritePage(U32 u32Row,U16 u16Col)
{
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u16Col);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
    HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    // if small block set one column mode first
    if( _fsinfo.u8AddrWidth == 5)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C5TFS0<<8) + OP_CMD80_SERIAL_DATA_INPUT);
    }
    else if( _fsinfo.u8AddrWidth == 4)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C4TFS0<<8) + OP_CMD80_SERIAL_DATA_INPUT);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_ADR_C3TFS0<<8) + OP_CMD80_SERIAL_DATA_INPUT);
    }
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       drvNAND_FLASH_INIT Procedure
*
*   DESCRIPTION:
*       Nand flash interafce initial function
*   NOTE:
*
*********************************************************************/
extern int HP;
int drvNAND_FLASH_INIT(void)
{
	DBG_NAND(printk("drvNAND_FLASH_INIT\r\n"));
	DBG_NAND(printk("FCIE2 Ver:%X\r\n",REG(NC_FCIE2_VERSION)));

	drvNAND_SWITCH_PAD(0);
	HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
	HAL_WRITE_UINT16(NC_AUXR_DAT,0x00); // repeat count 0
	memset(&_fsinfo, 0, sizeof(_fsinfo));

	_fsinfo.u16NC_CFG0 = R_NC_DYNGATED_ON+R_NC_ECCERR_NSTOP+R_NC_CHK_RB_HIGH+R_NC_DSIZE_SEL+R_NC_CE_EN+R_NC_WP_EN;//+R_NC_CE_AUTO; // FPGA version
    
    if(HP==1)
    {
        printk("NAND CLK 36MHZ\n");
        drvNAND_SetNC_CLOCK(NC_CLK_32M);
    }
    else
    {
        drvNAND_SetNC_CLOCK(NC_CLK_27M);
    }

	drvNAND_SoftReset();

	HAL_WRITE_UINT16(NC_MIE_INT_EN,0x00);

	HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_NC_EN);
	
	drvNAND_READ_ID();
	
	if(_fsinfo.u8ID == 0)
	{
		printk("\n unknown FLASH\n");
		drvNAND_Release_PAD();
		return false;
	}

	drvNAND_Release_PAD();

#ifndef CONFIG_MS_USB_MIU1_PACH
    {
        // switch the MIU selection to be controlled by SW
        U16 tmp;
        HAL_READ_UINT16(SYS_BASE+(0x121C<<1),tmp);
        tmp |= (1<<4);
        HAL_WRITE_UINT16(SYS_BASE+(0x121C<<1),tmp);
    }
#endif
    return true;
}

void MDrv_NAND_Erase1(U32 u32Row)
{
    NCRW_STATUS eStatus;

    DBG_NAND(printk("MDrv_NAND_EraseBlock\r\n"));
    drvNAND_SWITCH_PAD(0);
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, 0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
    HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        if( _fsinfo.u8AddrWidth == 5)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C3TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C2TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
    }
    else
    {
        if( _fsinfo.u8AddrWidth == 3)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C2TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_ADR_C3TRS0<<8)+OP_CMD60_BLOCK_ERASE);
        }
    }
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_BREAK);
    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(false);

}

bool MDrv_NAND_Erase2(void)
{
    NCRW_STATUS eStatus;

    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_WAIT_READY_BUSY<<8) + OP_CMDD0_BLOCK_ERASE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_CHECK_STATUS<<8) + OP_CMD70_READ_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(false);

    if(eStatus!= NCRW_STS_OK)
    {
        (printk("Erase fail\r\n"));
        return(false);
    }
    else
    {
        DBG_NAND(printk("Erase PASS\r\n"));
        return(true);
    }
}

U8 MDrv_NAND_Set_Chip_Select(U8 u8Level)
{
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    if(u8Level)
    {
        _fsinfo.u16NC_CFG0 &= ~R_NC_CE_EN;
    }
    else
    {
        _fsinfo.u16NC_CFG0 |= R_NC_CE_EN;
    }

    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    return(1);
}

U8 MDrv_NAND_Set_Command_Latch(U8 u8Level)
{
    if(!u8Level)
        _fsinfo.ehw_ctl = HW_CLE;
    else
        _fsinfo.ehw_ctl= 0;
    return(1);
}

U8 MDrv_NAND_Set_Address_Latch(U8 u8Level)
{
    if(!u8Level)
    {
        _fsinfo.ehw_ctl = HW_ALE;
        _fsinfo.u8AddressCnt = 0;   // reset address bytes count
    }
    else
    {
        _fsinfo.ehw_ctl= 0;
    }
    return(1);
}

U8 MDrv_NAND_Set_Write_Protect(U8 u8Level)
{
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    if(u8Level)
        _fsinfo.u16NC_CFG0 |= (R_NC_WP_EN);
    else
        _fsinfo.u16NC_CFG0 &= ~(R_NC_WP_EN);

    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    return(1);
}

U8 MDrv_NAND_WaitReady(void)
{
    drvNAND_SWITCH_PAD(0);
    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8) + OP_A_WAIT_READY_BUSY);

    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    drvNAND_wNCJOBEND(false);
    drvNAND_Release_PAD();
    return(1);
}

U8 MDrv_NAND_Send_Read_Status_CMD(U8 u8Cmd)
{
    u8CIF_C_Index = 0;
    DBG_NAND(printk("MDrv_NAND_Send_Read_Status_CMD\r\n"));
    drvNAND_ClearNFIE_EVENT();
    drvNAND_SWITCH_PAD(0);
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_USERCMD);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)u8Cmd<<8) + u8Cmd);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_CHECK_STATUS<<8) + OP_CMD_REG);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8)+OP_A_BREAK);

    HAL_WRITE_UINT16(NC_CTRL, R_NC_JOB_START);
    drvNAND_wNCJOBEND(false);
    DBG_NAND(printk("status=0x%X\r\n",(U16)REG(NC_ST_READ)));
    DBG_NAND(printk("_fsinfo.u16NC_CFG0 = 0x%X\r\n",_fsinfo.u16NC_CFG0));
    drvNAND_SetCMD_RSP_BUF(0, (U8)REG(NC_ST_READ));
    drvNAND_Release_PAD();
    return(1);
}

U8 MDrv_NAND_WriteByte(U8 u8Byte)
{
    if(_fsinfo.ehw_ctl == HW_CLE)
    {

    }
    return(true);
}

U8 MDrv_NAND_ReadByte(void)
{
    //DBG_NAND(printk("Read Byte=0x%X\r\n",drvNAND_GetCMD_RSP_BUF(u8CIF_C_Index)));
    return( drvNAND_GetCMD_RSP_BUF(u8CIF_C_Index++));
}

void MDrv_NAND_ReadBuf(U8 * const u8Buf,U16 u16Len)
{
    U32 u32DMAAddr;
    U8 *pu8Buf;
    U16 u16Tmp;

#ifdef NAND_DMA_RACING_PATCH
    U32 *u32pBuf = 0;
#endif

    drvNAND_ClearNFIE_EVENT();
    
    if((u16Len==0x200)||(u16Len==0x210)||(u16Len==0x800)||(u16Len==0x840))
    {
        u16ByteIdxofPage = u16Len;
		
        // DMA must be WIUWIDTH aligned, For titania, MIUWIDTH is 8 bytes.
        if(((U32)u8Buf & (MIUWIDTH-1))==0)
        {
            if( ((unsigned int)u8Buf) >= 0xC0000000 ) // To avoid the DMA using the non-continous memory
            	pu8Buf = _stPageBuffer;
            else
            	pu8Buf = u8Buf;
        }
        else
        {
            pu8Buf = _stPageBuffer;
        }

#ifdef NAND_DMA_RACING_PATCH
    	if( u16Len == 0x200 || u16Len == 0x210 )
    	{
    		u32pBuf = (U32*)(pu8Buf+508);
    		*u32pBuf = NAND_READ_MAGIC;
    	}
    	else if( u16Len == 0x800 || u16Len == 0x840 )
    	{
    		u32pBuf = (U32*)(pu8Buf+2044);
    		*u32pBuf = NAND_READ_MAGIC;
    	}
#endif

        _dma_cache_wback_inv((U32)pu8Buf, u16Len);
        __asm(
                "sync;"
                "nop;"
        );
        
        u32DMAAddr = (U32)VA2PA(pu8Buf)/MIUWIDTH;

        if(u16Len<=0x210)
            HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
        else
            HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);
            
#ifndef CONFIG_MS_NAND_MIU1_PATCH
		if( (u32DMAAddr<<3) > MDrv_SYS_GetMIU0Length() )
		{
			HAL_WRITE_UINT16(NC_MIU_DMA1,((u32DMAAddr>>16)&0x00FF)|R_MIU1_SELECT);
		}
		else
#endif
        	HAL_WRITE_UINT16(NC_MIU_DMA1,(u32DMAAddr>>16)&0x00FF);
        HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
        HAL_WRITE_UINT16(NC_MMA_PRI_REG,0);

#ifdef NAND_MIU_CLK_PATCH
        /* MIU CLK patch */
        do {
        	HAL_READ_UINT16(NC_MMA_PRI_REG, u16Tmp);
        } while( !(u16Tmp & R_FIFO_CLKRDY) );
#endif

        HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);
        HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

        if(u16Len<=0x210)
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_SERIAL_DATA_IN<<8) + OP_A_WAIT_READY_BUSY);
            HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)(OP_A_BREAK)<<8)+OP_A_BREAK);
        }
        else
        {
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_SERIAL_DATA_IN<<8) + OP_A_WAIT_READY_BUSY);
            HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)OP_A_BREAK<<8) + OP_A_BREAK);
        }

        HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);

        u16Tmp = drvNAND_wNCJOBEND(true);

#ifdef NAND_DMA_RACING_PATCH
		if( u16Len >= 0x200 )
		{
			int tmcnt = 0;
			//DBG_NAND(printf("*u32pBuf=%X\n", *u32pBuf));
			while( *u32pBuf == NAND_READ_MAGIC )
			{
				if((tmcnt++)>0x3fffff)
				{
					printk(KERN_WARNING "Timeout!!!!\n");
					break;
				}
			}
		}
#endif

        if(u16Tmp==NCRW_STS_ECC_ERROR)
		{
			// Check ECC code by SW, our HW generate the ECC code by (512+6) bytes
			if((drvNAND_GetCMD_RSP_BUF(6)==0xFF) && (drvNAND_GetCMD_RSP_BUF(7)==0xFF) &&
               (drvNAND_GetCMD_RSP_BUF(8)==0xFF) && (drvNAND_GetCMD_RSP_BUF(9)==0xFF) &&
               (drvNAND_GetCMD_RSP_BUF(10)==0xFF) && (drvNAND_GetCMD_RSP_BUF(11)==0xFF) &&
               (drvNAND_GetCMD_RSP_BUF(12)==0xFF) && (drvNAND_GetCMD_RSP_BUF(13)==0xFF) &&
               (drvNAND_GetCMD_RSP_BUF(14)==0xFF) && (drvNAND_GetCMD_RSP_BUF(15)==0xFF) )
		    {
		        gint_IS_ECC_Fail = 0;
    	    }
            else
    		{
				printk("\r\n\r\n\r\n\r\nECC Error\r\n");
				{
				#if 0
					U16 i;
					printk("============================================\r\n");
					for(i=0;i<0x200;i++)
					{
					    printk("%2X ",pu8Buf[i]);
					    if((i&0xF)==0xF)
					        printk("\r\n");
					}
					printk("============================================\r\n");
					for(i=0;i<0x10;i++)
					    printk("%2X ",drvNAND_GetCMD_RSP_BUF(i));
					printk("\r\n============================================\r\n");
				#endif
				}
				gint_IS_ECC_Fail = -1;
			}
		}
        else if (u16Tmp == NCRW_STS_BIT_FLIP)
        {
            gint_IS_ECC_Fail = 1;
        }
        else
        {
        #if 0
            U16 u16PageSize;
            int i, j;
        
            if ((u16Len = 0x200) || (u16Len = 0x210))
            {
                u16PageSize = 0x200;
                i = 0;
            }
            if ((u16Len = 0x800) || (u16Len = 0x840))
            {
                u16PageSize = 0x800;
                i = 3;
            }
            for (; i >= 0; i--)
            {
                for (j = 15; j >= 6; j--)
                {
                    if (drvNAND_GetCMD_RSP_BUF(i * 16 + j) != 0xFF)
                    {
                        break;
                    }
                }
                if (j >= 6)
                {
                    break;
                }
            }
            if ((i < 0) && (j < 6))  //Maybe an empty page
            {
	         gint_IS_ECC_Fail = 0;

                for (i = u16PageSize - 1; i >= 0; i--)
                {
                    if (pu8Buf[i] != 0xff)
                    {
                        // the page is not really empty
                        printk("0xff error detect, i=%d,data=0x%x\r\n", i, pu8Buf[i]);
                        pu8Buf[i] = 0xff;
                        gint_IS_ECC_Fail = 1;
                    }
                }
	     }
            else
        #endif
            {
                gint_IS_ECC_Fail = 0;
            }
        }

        if( u16Len == 0x840 || u16Len == 0x800 )
        {
            for(u32DMAAddr=0;u32DMAAddr<64;u32DMAAddr++)
            {
                _stPageBuffer[0x800+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
            }
        }
        else //0x210 or 0x200 case
        {
            for(u32DMAAddr=0;u32DMAAddr<16;u32DMAAddr++)
            {
                _stPageBuffer[0x200+u32DMAAddr] = drvNAND_GetCMD_RSP_BUF(u32DMAAddr);
            }
        }
        
        if((U32)u8Buf != (U32)pu8Buf)
        {
            memcpy(u8Buf,_stPageBuffer,u16Len);
        }
    }
    else
    {
        memcpy(u8Buf,&_stPageBuffer[u16ByteIdxofPage],u16Len);
    }

    //printf("end\r\n");
}

void MDrv_NAND_ReadOOB(U32 u32Row,U16 u16Column)
{
    U8 u8Tmp;

    u16ByteIdxofPage = 0;   // for readbuf
    u8CIF_C_Index = u16Column;      // for readbyte

    drvNAND_SWITCH_PAD(0);
    _fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE )
    {
        drvNAND_ReadRdd(u32Row,0x200);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
        drvNAND_ReadRdd(u32Row,0x410);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[0x10+u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[0x10+u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
        drvNAND_ReadRdd(u32Row,0x620);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[0x20+u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[0x20+u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
        drvNAND_ReadRdd(u32Row,0x830);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[0x30+u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[0x30+u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
    }
    else
    {
        drvNAND_ReadRdd(u32Row,0x200);
        for(u8Tmp=0;u8Tmp<0x10;u8Tmp++)
        {
            _stPageBuffer[u8Tmp] = drvNAND_GetCMD_RSP_BUF(u8Tmp);
            //printk("%2X ",_stPageBuffer[u8Tmp]);
            //if((u8Tmp&0xF)==0xF)
            //    printk("\r\n");
        }
    }
}

static U32 _tWriteLen;

NCRW_STATUS MDrv_NAND_WritePage(U32 u32Row,U16 u16Column)
{
    u16ByteIdxofPage = u16Column;
    _tWriteLen = 0;

    _fsinfo.u32PreSource[0] = u32Row;
    _fsinfo.u32PreSource[1] = u16Column;

    return(NCRW_STS_OK);
}

void MDrv_NAND_WriteBuf(U8 * const u8Buf,U16 u16Len)
{
    DBG_NAND(printk("MDrv_NAND_WriteBuf 0x%X,u16ByteIdxofPage=0x%X\r\n",u16Len,u16ByteIdxofPage));
    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
    {
        if((_tWriteLen==0) && (u16ByteIdxofPage>=0x800))
        {
            memset(_stPageBuffer,0xFF,0x840);
        }
    }
    else
    {
    if((_tWriteLen==0) && (u16ByteIdxofPage>=0x200))
    {
        memset(_stPageBuffer,0xFF,0x210);
    }
    }
    memcpy(&(_stPageBuffer[u16ByteIdxofPage]),u8Buf,u16Len);
    u16ByteIdxofPage += u16Len;
    _tWriteLen  += u16Len;
}

void MDrv_NAND_Cmd_PageProgram(U8 u8Cmd)
{
    NCRW_STATUS eStatus;
    U32 u32DMAAddr;
    U16 u16PageSize,u16Cnt;
    U8 u8Tmp,u8Idx;
    bool ECC_ENABLE = true;
#ifdef NAND_MIU_CLK_PATCH
    U16 u16Tmp;
#endif

	drvNAND_SWITCH_PAD(0);
    drvNAND_ClearNFIE_EVENT();
    DBG_NAND(printk("MDrv_NAND_Cmd_PageProgram,Len:0x%lX\r\n",_tWriteLen));

    u32DMAAddr = ((U32)VA2PA(_stPageBuffer))/MIUWIDTH;
    DBG_NAND(printk("_stPageBuffer=0x%lX\r\n",(U32)_stPageBuffer));
    DBG_NAND(printk("u32DMAAddr=0x%lX\r\n",u32DMAAddr));

    if( _fsinfo.eFlashConfig & FLASH_2KPAGE )
    {
        u8Tmp = 64;
        u16PageSize = 0x800;
        if( (_stPageBuffer[u16PageSize+1] == 0xFF) &&
            (_stPageBuffer[u16PageSize+2] == 0xFF) &&
            (_stPageBuffer[u16PageSize+3] == 0xFF) &&
            (_stPageBuffer[u16PageSize+4] == 0xFF) &&
            (_stPageBuffer[u16PageSize+5] == 0xFF) && (_tWriteLen==0x840))
		{
			drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x200);
			for(u8Idx=0;u8Idx<5;u8Idx++)
			{
				_stPageBuffer[u16PageSize+u8Idx] = drvNAND_GetCMD_RSP_BUF(u8Idx);
			}
			drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x410);
			for(u8Idx=0;u8Idx<5;u8Idx++)
			{
				_stPageBuffer[u16PageSize+u8Idx+0x10] = drvNAND_GetCMD_RSP_BUF(u8Idx);
			}
			drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x620);
			for(u8Idx=0;u8Idx<5;u8Idx++)
			{
				_stPageBuffer[u16PageSize+u8Idx+0x20] = drvNAND_GetCMD_RSP_BUF(u8Idx);
			}
			drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x830);
			for(u8Idx=0;u8Idx<5;u8Idx++)
			{
				_stPageBuffer[u16PageSize+u8Idx+0x30] = drvNAND_GetCMD_RSP_BUF(u8Idx);
			}
		}
		HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x4);
    }
    else
    {
        u8Tmp = 16;
        u16PageSize = 0x200;
        if( (_stPageBuffer[u16PageSize+0] == 0xFF) &&
            (_stPageBuffer[u16PageSize+1] == 0xFF) &&
            (_stPageBuffer[u16PageSize+2] == 0xFF) &&
            (_stPageBuffer[u16PageSize+3] == 0xFF) &&
            (_stPageBuffer[u16PageSize+4] == 0xFF) && (_tWriteLen==0x210))
        {
			drvNAND_ReadRdd(_fsinfo.u32PreSource[0], 0x200);
			drvNAND_FLASHRESET();
			for(u8Idx=0;u8Idx<5;u8Idx++)
			{
				_stPageBuffer[u16PageSize+u8Idx] = drvNAND_GetCMD_RSP_BUF(u8Idx);
			}
        }
        HAL_WRITE_UINT16(NC_JOB_BL_CNT,0x1);
    }
    
    // Patch Code for write JFFS2 image
    //==================================================================
    {
       u16Cnt = 0;

       while(_stPageBuffer[u16Cnt++]==0xFF)
       {
           //for Clean Marker case
           if((u16Cnt==u16PageSize) &&
                   (
                     _stPageBuffer[u16Cnt+0]==0x85 &&
                     _stPageBuffer[u16Cnt+1]==0x19 &&
                     _stPageBuffer[u16Cnt+2]==0x03 &&
                     _stPageBuffer[u16Cnt+3]==0x20 &&
                     _stPageBuffer[u16Cnt+4]==0x08
                   )
              )
           {
                ECC_ENABLE = false;
                break;
           }
           //for all 0xFF case
           else if(u16Cnt==(u8Tmp+u16PageSize))
           {
               ECC_ENABLE = false;
               return;
               break;
           }
       }
    }

    u8Idx = 0;
    for(u8Idx=0;u8Idx<u8Tmp;u8Idx++)
    {
        drvNAND_SetCMD_RSP_BUF(u8Idx,_stPageBuffer[u16PageSize+u8Idx]);
    }
    //printk("\r\n");
    _dma_cache_wback_inv((U32)_stPageBuffer, (U32)sizeof(_stPageBuffer));
    __asm(
        "sync;"
        "nop;"
    );
#ifndef CONFIG_MS_NAND_MIU1_PATCH
	if( (u32DMAAddr<<3) > MDrv_SYS_GetMIU0Length() )
	{
		HAL_WRITE_UINT16(NC_MIU_DMA1,((u32DMAAddr>>16)&0x00FF)|R_MIU1_SELECT);
	}
	else
#endif
    	HAL_WRITE_UINT16(NC_MIU_DMA1,(u32DMAAddr>>16)&0x00FF);
    HAL_WRITE_UINT16(NC_MIU_DMA0,u32DMAAddr&0xFFFF);
    HAL_WRITE_UINT16(NC_MMA_PRI_REG,R_NC_JOB_RW_DIR); //write

#ifdef NAND_MIU_CLK_PATCH
    /* MIU CLK patch */
    do {
    	HAL_READ_UINT16(NC_MMA_PRI_REG, u16Tmp);
    } while( !(u16Tmp & R_FIFO_CLKRDY) );
#endif

    HAL_WRITE_UINT16(NC_MIE_PATH_CTL,R_MMA_ENABLE+R_NC_EN);

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_USERCMD);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)u8Cmd<<8) + u8Cmd);

    drvNAND_WritePage(_fsinfo.u32PreSource[0],0x00);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(((U16)OP_CMD_REG)<<8)+OP_A_SERIAL_DATA_OUT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(((U16)OP_A_WAIT_READY_BUSY)<<8)+OP_A_WAIT_IDLE);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(((U16)OP_A_CHECK_STATUS)<<8)+OP_CMD70_READ_STATUS);
    HAL_WRITE_UINT16(NC_AUXR_DAT,(((U16)OP_A_BREAK)<<8)+OP_A_BREAK);
    HAL_WRITE_UINT16(NC_AUXR_ADR,AUXADR_RPTCNT);
    HAL_WRITE_UINT16(NC_AUXR_DAT,0x00);

    if(((_tWriteLen != 0x210) && (_tWriteLen != 0x840)) || (ECC_ENABLE==false))
        HAL_WRITE_UINT16(NC_CTRL,R_NC_ECC_BYPASS+R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    else
    HAL_WRITE_UINT16(NC_CTRL,R_NC_DIR_DSTREAM+R_NC_CIFC_ACCESS+R_NC_JOB_START);
    eStatus = drvNAND_wNCJOBEND(true);

    if(eStatus==NCRW_STS_OK)
    {
        DBG_NAND(printk("MDrv_NAND_Cmd_PageProgram PASS\r\n"));
    }
    else
    {
        DBG_NAND(printk("MDrv_NAND_Cmd_PageProgram false\r\n"));
    }
    drvNAND_Release_PAD();
    //============================================================================

}

NCRW_STATUS MDrv_NAND_SendCmd(U8 u8Cmd)
{
    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_USERCMD);
    HAL_WRITE_UINT16(NC_AUXR_DAT, ((U16)u8Cmd<<8) + u8Cmd);
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_CMD_REG<<8) + OP_A_WAIT_READY_BUSY);
    HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8) + OP_A_BREAK);
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    return(drvNAND_wNCJOBEND(false));
}

NCRW_STATUS MDrv_NAND_SendAdr(U32 u32Row,U16 u16Column)
{
    drvNAND_ClearNFIE_EVENT();
    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_ADRSET);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u16Column);
    HAL_WRITE_UINT16(NC_AUXR_DAT, u32Row);
    HAL_WRITE_UINT16(NC_AUXR_DAT, (u32Row>>16));

    HAL_WRITE_UINT16(NC_AUXR_ADR, AUXADR_INSTQUE);

    if( _fsinfo.u8AddrWidth == 5)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8) + OP_ADR_C5TFS0);
    }
    else if( _fsinfo.u8AddrWidth == 4)
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8) + OP_ADR_C4TFS0);
    }
    else
    {
        HAL_WRITE_UINT16(NC_AUXR_DAT,((U16)OP_A_BREAK<<8) + OP_ADR_C3TFS0);
    }
    HAL_WRITE_UINT16(NC_CTRL,R_NC_CIFC_ACCESS+R_NC_JOB_START);
    return(drvNAND_wNCJOBEND(false));
}

int MDrv_NAND_CheckECC(void)
{
    return(gint_IS_ECC_Fail);
}


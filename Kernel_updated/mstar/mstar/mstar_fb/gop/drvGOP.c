//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!MStar Confidential Information!L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "linux/kernel.h"
#include "regGOP.h"
#include "halGOP.h"
#include "drvGOP.h"

//-------------------------------------------------------------------------------------------------
//  Local Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define ALIGN_CHECK(value,factor) ((value + factor-1) & (~(factor-1)))
//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_GOP0 = 0,
    E_GOP1 = 1,
    E_GOP2 = 2,
    E_GOP3 = 3,
    E_GOP_Dwin = 4,
    E_GOP_MIXER = 5,
}E_GOP_TYPE;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
U8 bForceWriteIn;

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Local Functions
//------------------------------------------------------------------------------
static U16 _GOP_GetBPP(DRV_GOPColorType fbFmt)
{
    U16 bpp=0;

    switch ( fbFmt )
    {
		case E_DRV_GOP_COLOR_RGB555_BLINK :
		case E_DRV_GOP_COLOR_RGB565 :
		case E_DRV_GOP_COLOR_ARGB1555:
		case E_DRV_GOP_COLOR_RGBA5551:
		case E_DRV_GOP_COLOR_ARGB4444 :
		case E_DRV_GOP_COLOR_RGBA4444 :
		case E_DRV_GOP_COLOR_RGB555YUV422:
		case E_DRV_GOP_COLOR_YUV422:
		case E_DRV_GOP_COLOR_2266:
		    bpp = 2;
		    break;
		case E_DRV_GOP_COLOR_ARGB8888 :
		case E_DRV_GOP_COLOR_ABGR8888 :
		    bpp = 4;
		    break;
		case E_DRV_GOP_COLOR_I8 :
		    bpp = 1;
		    break;
		default :
		    //print err
		    bpp = 0xFFFF;
		    break;
    }
    return bpp;
}

void _MDrv_GOP_UpdateReg(U8 u8Gop)
{
	U16 u16GOPOfst = 0;
	U32 bankoffset=0;

	if(bForceWriteIn)
	{
		_HAL_GOP_Write16Reg(GOP_BAK_SEL, GOP_VAL_FWR, GOP_REG_HW_MASK);
		_HAL_GOP_Write16Reg(GOP_BAK_SEL, 0x0000, GOP_REG_HW_MASK);
	}
	else
	{
		_HAL_GOP_GetBnkOfstByGop(u8Gop,&bankoffset);
		u16GOPOfst = (bankoffset>>8);
#ifdef GOP_BANK_SHIFT
	    if((E_GOP_TYPE)u8Gop == E_GOP2)
			u16GOPOfst -= 1;
#endif
		_HAL_GOP_Write16Reg(GOP_BAK_SEL, u16GOPOfst | GOP_BIT10 , GOP_REG_WORD_MASK);
		_HAL_GOP_Write16Reg(GOP_BAK_SEL, u16GOPOfst, GOP_REG_WORD_MASK);
	}

}

void _MDrv_GOP_SetForceWrite(U8 bEnable)
{
    bForceWriteIn = bEnable;
}

void _MDrv_GOP_Init(U8 u8GOP)
{
	U8 u8Idx;
    U32 bankoffset=0;
    

    if(u8GOP >= MAX_GOP_SUPPORT)
    {
        printk("[%s] not support gop%d in this chip version!!\n",__FUNCTION__, u8GOP);
        return;
    }
	_HAL_GOP_GetBnkOfstByGop(u8GOP,&bankoffset);
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, 0x1, 0x1);          // GOP rst
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, 0x0, 0x7);          // clear H/V sync reserve
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL1, 0x4100, 0xff00);     // Set REGDMA interval

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_HSTRCH, 0x1000, 0xFFFF);   // Sanger 070713 For REGDMA Ready issue.
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_VSTRCH, 0x1000, 0xFFFF);
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, GOP_BIT14, GOP_BIT14);    // Set mask Hsync when VFDE is low


    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, 0x000, 0x400);     // Output = RGB

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_RATE, 0x0098, GOP_REG_WORD_MASK);     // enable blink capability - for ttx usage
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, 0x0000, 0x400);       // Enable RGB output
	//_HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, GOP_BIT11, GOP_BIT11);       // Enable Transparent Color
	
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_BW, GOP_FIFO_BURST_ALL, GOP_FIFO_BURST_MASK );  //set GOP DMA Burst length to "32"
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_BW, GOP_FIFO_THRESHOLD, GOP_REG_LW_MASK ); //set DMA FIFO threshold to 3/4 FIFO length

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_OLDADDR, 0x0, (GOP_BIT1 | GOP_BIT0) ); //temporally disable GOP clk dynamical gated - to avoid update palette problem

    /*set clamping value between 2 pixels, it can avoid some cases garbage happen.*/
    _HAL_GOP_Write16Reg((bankoffset+0x200)+GOP_4G_STRCH_HSZ, 0, GOP_BIT0);
    _HAL_GOP_Init(u8GOP);
    

    if (u8GOP==0)
    {
        for (u8Idx=0; u8Idx<MAX_GOP0_GWIN; u8Idx++)
        {
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_VSTR_L(u8Idx),0, GOP_REG_WORD_MASK);
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_VSTR_H(u8Idx),0, GOP_REG_WORD_MASK);
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_HSTR(u8Idx),0, GOP_REG_WORD_MASK);
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_HVSTOP_L(u8Idx),0, GOP_REG_WORD_MASK);
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_HVSTOP_H(u8Idx),0, GOP_REG_WORD_MASK);
        }
    }
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, GOP_BIT2, GOP_BIT2);

}

void _MDrv_GOP_SetStretchWin(U8 u8GOP,U16 x, U16 y, U16 width, U16 height)
{
    U32 bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);

    _HAL_GOP_Write16Reg(bankoffset + GOP_4G_STRCH_HSTR, x, GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(bankoffset + GOP_4G_STRCH_VSTR, y , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(bankoffset + GOP_4G_STRCH_HSZ, (width/GOP_STRETCH_WIDTH_UNIT) , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(bankoffset + GOP_4G_STRCH_VSZ, height , GOP_REG_WORD_MASK);
	_HAL_GOP_Write16Reg(bankoffset + GOP_4G_RDMA_HT, (width+3)/2, 0x07ff);
}

void _MDrv_GOP_SetHScale(U8 u8GOP,U8 bEnable, U16 src, U16 dst )
{
    U32 hratio =0x1000,bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);

    if (bEnable)
    {
        hratio = (U32)(src) * SCALING_MULITPLIER;

        if(hratio %(U32)dst !=0)
            hratio = (hratio/(U32)dst )+1;
        else
            hratio /= (U32)dst;
    }
    else
        hratio = SCALING_MULITPLIER;

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_HSTRCH, hratio , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_HSTRCH_INI, 0 , GOP_REG_WORD_MASK);

}

void _MDrv_GOP_SetVScale(U8 u8GOP,U8 bEnable, U16 src, U16 dst )
{
    U32 vratio =0x1000,bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);


    if (bEnable)
    {
        vratio = (U32)(src) * SCALING_MULITPLIER;

        if(vratio %(U32)dst !=0)
            vratio = (vratio/(U32)dst )+1;
        else
            vratio /= (U32)dst;
    }
    else
        vratio = SCALING_MULITPLIER;

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_VSTRCH, vratio , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_VSTRCH_INI, 0 , GOP_REG_WORD_MASK);

}

void _MDrv_GOP_SetGwinInfo(U8 u8GOP,U8 u8win,DRV_GWIN_INFO WinInfo)
{
    U32 bankoffset=0,u32RingBuffer;
    U16 bpp;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);

    bpp = _GOP_GetBPP(WinInfo.clrType);
    if(bpp == 0xFFFF)
    {
        printk("[%s] invalud color format\n",__FUNCTION__);
        return;
    }
	
	WinInfo.u32Addr = ALIGN_CHECK(WinInfo.u32Addr,GOP_WordUnit);
	WinInfo.u16Pitch = ALIGN_CHECK(WinInfo.u16Pitch,(GOP_WordUnit/bpp));
	WinInfo.u16HStart = ALIGN_CHECK(WinInfo.u16HStart,(GOP_WordUnit/bpp));
	WinInfo.u16HEnd = ALIGN_CHECK(WinInfo.u16HEnd,(GOP_WordUnit/bpp));

	//Color Fmt
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_GWIN0_CTRL(u8win), WinInfo.clrType <<4,0x00f0);
	//Address
	_HAL_GOP_Write32Reg(bankoffset+GOP_4G_DRAM_RBLK_L(u8win), WinInfo.u32Addr/GOP_WordUnit);
	//Pitch
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_DRAM_RBLK_HSIZE(u8win), (WinInfo.u16Pitch*bpp)/GOP_WordUnit , GOP_REG_WORD_MASK);
	//HStart
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_HSTR(u8win), (WinInfo.u16HStart*bpp)/GOP_WordUnit , GOP_REG_WORD_MASK);
	//HEnd
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_HEND(u8win), (WinInfo.u16HEnd*bpp)/GOP_WordUnit , GOP_REG_WORD_MASK);
	//VStart
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_VSTR(u8win), WinInfo.u16VStart , GOP_REG_WORD_MASK);
	//VEnd
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_VEND(u8win), WinInfo.u16VEnd , GOP_REG_WORD_MASK);
	//Ring Buffer
	u32RingBuffer = (WinInfo.u16Pitch*bpp*(WinInfo.u16VEnd - WinInfo.u16VStart))/GOP_WordUnit;
	_HAL_GOP_Write32Reg(bankoffset+GOP_4G_DRAM_RBLK_SIZE_L(u8win), u32RingBuffer);
	//printk("\33[0;36m   %s:%d  \33[m \n",__FUNCTION__,__LINE__);
		
}

void _MDrv_GOP_SetBlending(U8 u8GOP, U8 u8win, U8 bEnable, U8 u8coef)
{
    _HAL_GOP_SetBlending(u8GOP, u8win, bEnable, u8coef);
}
void _MDrv_GOP_SetDstPlane(U8 u8GOP,DRV_GOPDstType eDstType)
{
    U32 bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);

	if (u8GOP==0)
		_HAL_GOP_Write16Reg(GOP_GOPCLK, CKG_GOPG0_ODCLK, CKG_GOPG0_MASK);
	else if (u8GOP==1)
		_HAL_GOP_Write16Reg(GOP_GOPCLK, CKG_GOPG1_ODCLK, CKG_GOPG1_MASK);
	else if (u8GOP==2)
        _HAL_GOP_Write16Reg(GOP_GOP2CLK, CKG_GOPG2_ODCLK, CKG_GOPG2_MASK);
    else if (u8GOP==3)
		_HAL_GOP_Write16Reg(GOP_GOP3CLK, CKG_GOPG3_ODCLK, CKG_GOPG2_MASK);
	
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL1, eDstType, 0x0007); 
	
	_HAL_GOP_SetGOPEnable2SC(u8GOP,TRUE);
}
void _MDrv_GOP_GWIN_Enable(U8 u8GOP,U8 u8win,u8 bEnable)
{
    U32 bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);	
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, 0x0, GOP_BIT11);       // Enable Transparent Color
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_GWIN0_CTRL(u8win), bEnable, 0x1);
}
void _MDrv_GOP_EnableHMirror(U8 u8GOP,u8 bEnable)
{
    U32 bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, bEnable?GOP_BIT12:0, GOP_BIT12);
}
void _MDrv_GOP_EnableVMirror(U8 u8GOP,u8 bEnable)
{
    U32 bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);
	_HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, bEnable?GOP_BIT13:0, GOP_BIT13);
}    
U8 _MDrv_GOP_GetMux(Gop_MuxSel eGopMux)
{
	U16 u16GopMux;
   	_HAL_GOP_Read16Reg(GOP_MUX, &u16GopMux);
    return (U8)(u16GopMux >> ((eGopMux%4)*GOP_MUX_SHIFT))& GOP_REGMUX_MASK;

}
void _MDrv_GOP_SetMux(GOP_MuxConfig GopMuxConfig)
{
    U8 i;
	
    if(GopMuxConfig.u8MuxCounts > MAX_GOP_MUX)
    {
        printk("\nMuxCounts over MAX_GOP_MUX number\n");
        return;
    }

    for(i=0; i<GopMuxConfig.u8MuxCounts; i++)
    {
        _HAL_GOP_Write16Reg(GOP_MUX, GopMuxConfig.GopMux[i].u8GopIndex<<(GOP_MUX_SHIFT*GopMuxConfig.GopMux[i].u8MuxIndex), GOP_REGMUX_MASK<<(GOP_MUX_SHIFT*GopMuxConfig.GopMux[i].u8MuxIndex));
	}
}
void _MDrv_GOP_SetPipeDelay(U8 u8GOP,U16 u16PnlHstart)
{
	int i;
	u8 u8GOPNum = 0xFF;
	u16 u16Offset = 0;
	U32 bankoffset=0;
	
	_HAL_GOP_GetBnkOfstByGop(u8GOP, &bankoffset);
	
	for (i=0; i<MAX_GOP_MUX; i++)
    {
		u8GOPNum = _MDrv_GOP_GetMux((Gop_MuxSel)i);
		if (u8GOP == u8GOPNum)
		{
			switch(i)
			{
				case 0:
					u16Offset = u16PnlHstart + GOP_Mux0_Offset + GOP_PD;
				break;
				case 1:
					u16Offset = u16PnlHstart + GOP_Mux1_Offset + GOP_PD;
				break;
				case 2:
					u16Offset = u16PnlHstart + GOP_Mux2_Offset + GOP_PD;
				break;
				case 3:
					u16Offset = u16PnlHstart + GOP_Mux3_Offset + GOP_PD;
				break;
				default:
				break;
			}
			_HAL_GOP_Write16Reg(bankoffset+GOP_4G_HS_PIPE, u16Offset, 0x3FF);
		}
	}
}
void _MDrv_GOP_MIU_Sel(U8 u8GOP,E_DRV_GOP_SEL_TYPE miusel)
{
    U16 mask_shift = 0;

    switch(u8GOP)
    {
        case E_GOP0:
            mask_shift = GOP_MIU_CLIENT_GOP0;
            break;
        case E_GOP1:
            mask_shift = GOP_MIU_CLIENT_GOP1;
            break;
        case E_GOP2:
            mask_shift = GOP_MIU_CLIENT_GOP2;
            break;
        case E_GOP3:
            mask_shift = GOP_MIU_CLIENT_GOP3;
            break;
        case E_GOP_Dwin:
            mask_shift = GOP_MIU_CLIENT_DWIN;
            break;
        default:
            mask_shift = 0xFF;
    }

    if(mask_shift == 0xFF)
        printk("ERROR gop miu client\n");
    
    _HAL_GOP_Write16Reg(GOP_MIU_GROUP1, miusel<<mask_shift, 1<<mask_shift );  //set GOP DMA Burst length to "32"

}


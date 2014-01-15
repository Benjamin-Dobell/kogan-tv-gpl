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
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_GOP_H_
#define _REG_GOP_H_

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_ARM)
#define RIU_MAP 0xFD200000
#elif defined(CONFIG_MIPS)
#define RIU_MAP 0xBF200000
#endif
#define RIU     ((unsigned short volatile *) RIU_MAP)

#define GOP_WRITE2BYTE(addr, val)    { RIU[addr] = val; }
#define GOP_READ2BYTE(addr)            RIU[addr]

#define GOP_REG_DIRECT_BASE                 (0x20200)
#define GOP_REG(bk, reg)                    (GOP_REG_DIRECT_BASE+((U32)(bk)<<8) + (reg) * 2)
#define GOP_REG_VAL(x)                          (1<<x)
//----------------------------------------------------------------------------
// HW IP Reg Base Adr
//----------------------------------------------------------------------------
#define CKG_REG_BASE                            0xB00
#define SC1_REG_BASE                           0x30000
#define MIU_REG_BASE                            0x1200

//----------------------------------------------------------------------------
// Scaler Reg
//----------------------------------------------------------------------------
#define GOP_SC_GOPEN                            (SC1_REG_BASE+0x0C)

//----------------------------------------------------------------------------
// ChipTop Reg
//----------------------------------------------------------------------------
/* GOP0 and GOP1 CLK */
#define GOP_GOPCLK                 (CKG_REG_BASE+0x80)
#define CKG_GOPG0_ODCLK         (0<<2)
#define CKG_GOPG0_MASK          (GOP_BIT4 | GOP_BIT3 | GOP_BIT2)

#define CKG_GOPG1_ODCLK         (0 << 10)
#define CKG_GOPG1_MASK          (GOP_BIT12 | GOP_BIT11 | GOP_BIT10)

/* GOP2 and GOPDWIN CLK */
#define GOP_GOP2CLK                 (CKG_REG_BASE+0x82)
#define CKG_GOPG2_ODCLK         (0<<2)
#define CKG_GOPG2_MASK          (GOP_BIT4 | GOP_BIT3 | GOP_BIT2)

/* GOP3 CLK*/
#define GOP_GOP3CLK             (CKG_REG_BASE+0x84)
#define CKG_GOPG3_ODCLK         (0<<2)
#define CKG_GOPG3_MASK          (GOP_BIT4 | GOP_BIT3 | GOP_BIT2)
//----------------------------------------------------------------------------
// MIU Reg
//----------------------------------------------------------------------------
#define GOP_MIU_GROUP1          (MIU_REG_BASE+0xF2)

/*Define each gop miu clint bit*/
#define GOP_MIU_CLIENT_DWIN     0x4
#define GOP_MIU_CLIENT_GOP0     0x5
#define GOP_MIU_CLIENT_GOP1     0x6
#define GOP_MIU_CLIENT_GOP2     0x7
#define GOP_MIU_CLIENT_GOP3     0x8

//----------------------------------------------------------------------------
// GOP Reg
//----------------------------------------------------------------------------
#define GOP_OFFSET_WR                       8
#define GOP_VAL_WR                          GOP_REG_VAL(GOP_OFFSET_WR)
#define GOP_OFFSET_FWR                      9
#define GOP_VAL_FWR                         GOP_REG_VAL(GOP_OFFSET_FWR)

#define GOP_4G_OFST                           0x0
#define GOP_2G_OFST                           0x3
#define GOP_1G_OFST                           0x6
#define GOP_1GX_OFST                          0x9
#define GOP_DW_OFST                           0xC

#define GOP_REGMUX_MASK                     0x7
#define GOP_MUX_SHIFT                       0x3

#define GOP_4G_CTRL0                        GOP_REG(GOP_4G_OFST, 0x00)
#define GOP_4G_CTRL1                        GOP_REG(GOP_4G_OFST, 0x01)
#define GOP_4G_RATE                         GOP_REG(GOP_4G_OFST, 0x02)
#define GOP_4G_RDMA_HT                      GOP_REG(GOP_4G_OFST, 0x0e)
#define GOP_4G_HS_PIPE                      GOP_REG(GOP_4G_OFST, 0x0f)
#define GOP_4G_BW                           GOP_REG(GOP_4G_OFST, 0x19)
#define GOP_4G_STRCH_HSZ                    GOP_REG(GOP_4G_OFST, 0x30)
#define GOP_4G_STRCH_VSZ                    GOP_REG(GOP_4G_OFST, 0x31)
#define GOP_4G_STRCH_HSTR                   GOP_REG(GOP_4G_OFST, 0x32)
#define GOP_4G_STRCH_VSTR                   GOP_REG(GOP_4G_OFST, 0x34)
#define GOP_4G_HSTRCH                       GOP_REG(GOP_4G_OFST, 0x35)
#define GOP_4G_VSTRCH                       GOP_REG(GOP_4G_OFST, 0x36)
#define GOP_4G_HSTRCH_INI                   GOP_REG(GOP_4G_OFST, 0x38)
#define GOP_4G_VSTRCH_INI                   GOP_REG(GOP_4G_OFST, 0x39)
#define GOP_4G_OLDADDR                      GOP_REG(GOP_4G_OFST, 0x3b)
#define GOP_MUX                             GOP_REG(GOP_4G_OFST, 0x7e)
#define GOP_BAK_SEL                         GOP_REG(GOP_4G_OFST, 0x7f)

#define GOP_4G_GWIN0_CTRL(id)               GOP_REG(GOP_4G_OFST+1, 0x00 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_L(id)              GOP_REG(GOP_4G_OFST+1, 0x01 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_H(id)              GOP_REG(GOP_4G_OFST+1, 0x02 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_HSTR(id)                     GOP_REG(GOP_4G_OFST+1, 0x04 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_HEND(id)                     GOP_REG(GOP_4G_OFST+1, 0x05 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_VSTR(id)                     GOP_REG(GOP_4G_OFST+1, 0x06 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_VEND(id)                     GOP_REG(GOP_4G_OFST+1, 0x08 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_HSIZE(id)          GOP_REG(GOP_4G_OFST+1, 0x09 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_GWIN_ALPHA01(id)             GOP_REG(GOP_4G_OFST+1, 0x0A + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_VSTR_L(id)              GOP_REG(GOP_4G_OFST+1, 0x0C + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_VSTR_H(id)              GOP_REG(GOP_4G_OFST+1, 0x0D + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_HSTR(id)                GOP_REG(GOP_4G_OFST+1, 0x0E + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_SIZE_L(id)         GOP_REG(GOP_4G_OFST+1, 0x10 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_SIZE_H(id)         GOP_REG(GOP_4G_OFST+1, 0x11 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_HVSTOP_L(id)            GOP_REG(GOP_4G_OFST+1, 0x14 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_HVSTOP_H(id)            GOP_REG(GOP_4G_OFST+1, 0x15 + (0x20*((id)%MAX_GOP0_GWIN)))
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
#endif // _REG_GOP_H_


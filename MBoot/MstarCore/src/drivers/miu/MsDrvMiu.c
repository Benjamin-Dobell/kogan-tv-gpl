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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Inlucde Fils
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <drvMIU.h>
#include <MsTypes.h>

#include <miu/MsDrvMiu.h>
#define MIU_ALIGN(_val_,_shift_) (((_val_) >> _shift_) << _shift_)

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Internal Functions
///
///////////////////////////////////////////////////////////////////////////////////////////////////
U8 _MIU_ReadByte(U32 u32RegOffset)
{
    return ((volatile U8*)(MIUREG_BASE))[(u32RegOffset << 1) - (u32RegOffset & 1)];
}

U16 _MIU_Read2Bytes(U32 u32RegOffset)
{
    return ((volatile U16*)(MIUREG_BASE))[u32RegOffset];
}

void _MIU_WriteByte(U32 u32RegOffset, U8 u8Val)
{
    ((volatile U8*)(MIUREG_BASE))[(u32RegOffset << 1) - (u32RegOffset & 1)] = u8Val;
}

void _MIU_Write2Bytes(U32 u32RegOffset, U16 u16Val)
{
    ((volatile U16*)(MIUREG_BASE))[u32RegOffset] = u16Val;
}

void _MIU_WriteBytesBit(U32 u32RegOffset, U8 bEnable, U16 u16Mask)
{
    U16 val = _MIU_Read2Bytes(u32RegOffset);
    val = (bEnable) ? (val | u16Mask) : (val & ~u16Mask);
    _MIU_Write2Bytes(u32RegOffset, val);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Global Functions
///
///////////////////////////////////////////////////////////////////////////////////////////////////
U8 MDrv_MIU_SetGroupPriority_UBoot(U8 bMIU1, U8 *au8GpPriority, U8 u8GpNum)
{
    u8 u8MIUGpPriority = 0, idx=0;
	U32 u32RegAddr = REG_MIU_GP_PRIORITY;

    /*u8MIUGpPriority=u8MIUGpPriority;*/
    /*idx=idx;*/
	if (u8GpNum > MIU_MAX_GP_NUM)
	{
		printf("%s: exced max group number!\n", __FUNCTION__);
		return FALSE;
	}

    for (idx = 0; idx < MIU_MAX_GP_NUM; idx++)
    {
        if (au8GpPriority[idx] > (MIU_MAX_GP_NUM-1))
        {
            printf("%s: wrong group number!\n", __FUNCTION__);
            return FALSE;
        }
        u8MIUGpPriority |= (au8GpPriority[idx] << (idx*2));
    }
    //printf("Set MIU%u group priority:%x.....\n", bMIU1, u8MIUGpPriority);
    //A3 Revision 1 can not modify miu priority
    if(((volatile U16*)(PM_TOP_BASE))[CHIP_ID]== 0x47 && ((volatile U8*)(PM_TOP_BASE))[((CHIP_VER<<1)-(CHIP_VER&1))] <= 0x01)
        return TRUE;


	u32RegAddr += (bMIU1) ? MIU1_REG_BASE : MIU0_REG_BASE;
	_MIU_WriteBytesBit(u32RegAddr,DISABLE, BIT8);
	_MIU_WriteByte(u32RegAddr, u8MIUGpPriority);
	_MIU_WriteBytesBit(u32RegAddr,ENABLE, BIT8);

	return TRUE;
}

const U16 client_gp[5] = {REG_MIUSEL_GP0, REG_MIUSEL_GP1, REG_MIUSEL_GP2, REG_MIUSEL_GP3, 0};
U8 MDrv_MIU_SelectMIU_UBoot(U8 bMIU1, U16 *au16SelMiu, U8 u8GpNum)
{
	U8 idx;
	U32 u32RegAddr = (bMIU1) ? MIU1_REG_BASE : MIU0_REG_BASE;

	if (u8GpNum > MIU_MAX_GP_NUM)
	{
		printf("%s: exced max group number!\n", __FUNCTION__);
		return FALSE;
	}

    //printf("Change MIU%u select.....\n", bMIU1);
	for (idx = 0; idx < u8GpNum; idx++)
	{
	    //printf("Group%u:%-4x  ", idx, au16SelMiu[idx]);
		_MIU_Write2Bytes( ( u32RegAddr+client_gp[idx]), au16SelMiu[idx]);
    }
    //printf("\n");
    return TRUE;
}
void MsDrv_kernelProtect(MS_U8  u8Blockx,MS_U32 u32Lx1Start, MS_U32 u32Lx1End)

{

    MDrv_MIU_Init();

    MS_U8 u8MIUProtectkernel_ID[16]={0};

    // ==== Linux kernel ID Protect
    printf("Kernel Protect Index[%d] LX[0x%lx][0x%lx] \n",u8Blockx,u32Lx1Start,u32Lx1End);

    u8MIUProtectkernel_ID[0] = MIU_CLIENT_MIPS_RW; //MIPS W

    u8MIUProtectkernel_ID[1] = MIU_CLIENT_NAND_RW;//MIU_CLIENT_BDMA_RW; //BDMA

    u8MIUProtectkernel_ID[2] = MIU_CLIENT_USB_UHC0_RW; //USB0

    u8MIUProtectkernel_ID[3] = MIU_CLIENT_USB_UHC1_RW; //USB1

    u8MIUProtectkernel_ID[4] = MIU_CLIENT_USB_UHC2_RW; //USB2

    u8MIUProtectkernel_ID[5] = MIU_CLIENT_G3D_RW; //G3D

    u8MIUProtectkernel_ID[6] = MIU_CLIENT_USB3_RW; //USB3.0

    MsDrv_MIU_Protect(u8Blockx, &u8MIUProtectkernel_ID[0], u32Lx1Start, u32Lx1End, ENABLE);




}

void MsDrv_PMProtect(MS_U8  u8Blockx,MS_U32 u32Start, MS_U32 u32End)

{

    MDrv_MIU_Init();

    MS_U8 u8MIUProtect_ID[16]={0};

    // ==== Linux kernel ID Protect
    printf("PM Protect Index[%d] LX[0x%lx][0x%lx] \n",u8Blockx,u32Start,u32End);

    u8MIUProtect_ID[0] = MIU_CLIENT_PM51_RW; //PM RW

    MsDrv_MIU_Protect(u8Blockx, &u8MIUProtect_ID[0], u32Start, u32End, DISABLE);

    MsDrv_MIU_Protect(u8Blockx, &u8MIUProtect_ID[0], u32Start, u32End, ENABLE);



}

MS_BOOL MsDrv_MIU_Protect(MS_U8  u8Blockx, MS_U8  *pu8ProtectId, MS_U32 u32Start, MS_U32 u32End, MS_BOOL bSetFlag)

{
    MS_BOOL isProtect=FALSE;

    MS_U32 u32PageShift;

    if(!MDrv_MIU_ProtectAlign(&u32PageShift))
    {
        printf("ERROR MDrv_MIU_ProtectAlign FAIL \n");
    }

    isProtect=MDrv_MIU_Protect(u8Blockx, pu8ProtectId, MIU_ALIGN(u32Start, u32PageShift), MIU_ALIGN(u32End, u32PageShift), bSetFlag);
    if(!isProtect)
    {
        printf("ERROR MDrv_MIU_Protect FAIL \n");
    }


    return isProtect;

}



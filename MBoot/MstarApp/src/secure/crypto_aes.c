//<MStar Software>
//***********************************************************************************
//MStar Software
//Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
//All software, firmware and related documentation herein (¡§MStar Software¡¨) are intellectual property of MStar Semiconductor, Inc. (¡§MStar¡¨) and protected by law, including, but not limited to, copyright law and international treaties.  Any use, modification, reproduction, retransmission, or republication of all or part of MStar Software is expressly prohibited, unless prior written permission has been granted by MStar. 
//By accessing, browsing and/or using MStar Software, you acknowledge that you have read, understood, and agree, to be bound by below terms (¡§Terms¡¨) and to comply with all applicable laws and regulations:
//
//1. MStar shall retain any and all right, ownership and interest to MStar Software and any modification/derivatives thereof.  No right, ownership, or interest to MStar Software and any modification/derivatives thereof is transferred to you under Terms.
//2. You understand that MStar Software might include, incorporate or be supplied together with third party¡¦s software and the use of MStar Software may require additional licenses from third parties.  Therefore, you hereby agree it is your sole responsibility to separately obtain any and all third party right and license necessary for your use of such third party¡¦s software. 
//3. MStar Software and any modification/derivatives thereof shall be deemed as MStar¡¦s confidential information and you agree to keep MStar¡¦s confidential information in strictest confidence and not disclose to any third party.  
//4. MStar Software is provided on an ¡§AS IS¡¨ basis without warranties of any kind. Any warranties are hereby expressly disclaimed by MStar, including without limitation, any warranties of merchantability, non-infringement of intellectual property rights, fitness for a particular purpose, error free and in conformity with any international standard.  You agree to waive any claim against MStar for any loss, damage, cost or expense that you may incur related to your use of MStar Software.  
//   In no event shall MStar be liable for any direct, indirect, incidental or consequential damages, including without limitation, lost of profit or revenues, lost or damage of data, and unauthorized system use.  You agree that this Section 4 shall still apply without being affected even if MStar Software has been modified by MStar in accordance with your request or instruction for your use, except otherwise agreed by both parties in writing.
//5. If requested, MStar may from time to time provide technical supports or services in relation with MStar Software to you for your use of MStar Software in conjunction with your or your customer¡¦s product (¡§Services¡¨).  You understand and agree that, except otherwise agreed by both parties in writing, Services are provided on an ¡§AS IS¡¨ basis and the warranty disclaimer set forth in Section 4 above shall apply.  
//6. Nothing contained herein shall be construed as by implication, estoppels or otherwise: (a) conferring any license or right to use MStar name, trademark, service mark, symbol or any other identification; (b) obligating MStar or any of its affiliates to furnish any person, including without limitation, you and your customers, any assistance of any kind whatsoever, or any information; or (c) conferring any license or right under any intellectual property right.
//7. These terms shall be governed by and construed in accordance with the laws of Taiwan, R.O.C., excluding its conflict of law rules.  Any and all dispute arising out hereof or related hereto shall be finally settled by arbitration referred to the Chinese Arbitration Association, Taipei in accordance with the ROC Arbitration Law and the Arbitration Rules of the Association by three (3) arbitrators appointed in accordance with the said Rules.  The place of arbitration shall be in Taipei, Taiwan and the language shall be English.  
//   The arbitration award shall be final and binding to both parties.
//***********************************************************************************
//<MStar Software>
#define CRYPTO_AES_C

#include <drvAESDMA.h>
#include <secure/crypto_aes.h>
#include <secure/MsSecureCommon.h>
#include <common.h>
#include <MsSystem.h>
#include <MsDebug.h>


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define BIT(bits)                   (1<<bits)



//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Public Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  External Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Local functions
//-------------------------------------------------------------------------------------------------
#if 1//defined(CONFIG_SECURE_USB_UPGRADE))

static int CommonAES128(AES_ParamStruct *AESPara)
{
    DRVAESDMA_RESULT result = DRVAESDMA_OK;
    MS_U32 u32AESInBuf, u32AESOutBuf;
    UBOOT_TRACE("IN\n");
    if((AESPara->pChiperBuf == 0)||(AESPara->pPlainBuf == 0)||(AESPara->pKey == 0))
    {
        UBOOT_ERROR("The input parameters are not correct\n");    
        return DRVAESDMA_INVALID_PARAM;
    }

    flush_cache(AESPara->pChiperBuf, AESPara->u32Len);

    u32AESInBuf=VA2PA(AESPara->pChiperBuf);
    u32AESOutBuf=VA2PA(AESPara->pPlainBuf);
    MDrv_AESDMA_Init(0x00, 0x20000000, 2); // wait the AESDMA.a
    MDrv_AESDMA_Reset();
    MDrv_AESDMA_SelEng(AESPara->eMode, AESPara->bIsDecrypt);
    result=MDrv_AESDMA_SetFileInOut(u32AESInBuf, AESPara->u32Len, u32AESOutBuf, u32AESOutBuf+AESPara->u32Len-1);
    if (result == DRVAESDMA_MIU_ADDR_ERROR)
    {
        UBOOT_ERROR("CommonAES128 execte MDrv_AESDMA_SetFileInOut fail\n");
        return -1;
    }

    if(AESPara->eMode == E_DRVAESDMA_CIPHER_CBC)
    {
        MDrv_AESDMA_SetIV(AESPara->pIV);
    }
    if(AESPara->pKey!=NULL)
        MDrv_AESDMA_SetKey(AESPara->pKey);

    UBOOT_DEBUG("AESDMA Start\n");
    MDrv_AESDMA_Start(TRUE);
    UBOOT_DEBUG("AESDMA polling finish bits\n");    
    
    while ((MDrv_AESDMA_GetStatus() & BIT(16)) == 0);

    flush_cache(AESPara->pPlainBuf, AESPara->u32Len);
    UBOOT_TRACE("OK\n");
    return 0;
}


int Secure_AES_ECB_Decrypt(MS_U32 u32Addr, MS_U32 u32Len, MS_U8 *bKey)
{
    AES_ParamStruct AESParam;
    int ret=0;
    UBOOT_TRACE("IN\n");
    
    AESParam.eMode = E_DRVAESDMA_CIPHER_ECB;
    AESParam.pChiperBuf = u32Addr;
    AESParam.pPlainBuf = u32Addr;
    AESParam.pKey = (MS_U32 *)bKey;
    AESParam.u32Len = u32Len;
    AESParam.pIV= (MS_U32 *)NULL;
    AESParam.bIsDecrypt = TRUE;
    ret=CommonAES128(&AESParam);
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("Secure_AES_Decrypt fail\n");
    }

    return ret;
}

int Secure_AES_ECB_Encrypt(MS_U32 u32Addr, MS_U32 u32Len, MS_U8 *bKey)
{
    AES_ParamStruct AESParam;
    int ret=0;
    UBOOT_TRACE("IN\n");

    AESParam.eMode = E_DRVAESDMA_CIPHER_ECB;
    AESParam.pChiperBuf = u32Addr;
    AESParam.pPlainBuf = u32Addr;
    AESParam.pKey = (MS_U32 *)bKey;
    AESParam.u32Len = u32Len;
    AESParam.pIV= (MS_U32 *)NULL;
    AESParam.bIsDecrypt = FALSE;

    ret=CommonAES128(&AESParam);
    if(ret==0)
    {
        UBOOT_TRACE("OK\n");
    }
    else
    {
        UBOOT_ERROR("Secure_AES_Encrypt fail\n");
    }

    return ret;
}


#endif

#undef CRYPTO_AES_C


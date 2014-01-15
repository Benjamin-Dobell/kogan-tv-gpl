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
#include <common.h>
#include <MsTypes.h>
#include <MsOS.h>
#include <drvBDMA.h>
#include <drvMIU.h>

#include <miu/MsDrvMiu.h>
#include <ShareType.h>

static MIU_DDR_SIZE dram_size_to_enum(MS_U32 u32DramSize)
{
    switch(u32DramSize)
    {
        case 0x02000000:
            return E_MIU_DDR_32MB;
        case 0x04000000:
            return E_MIU_DDR_64MB;
        case 0x08000000:
            return E_MIU_DDR_128MB;
        case 0x10000000:
            return E_MIU_DDR_256MB;
        case 0x20000000:
            return E_MIU_DDR_512MB;
        case 0x40000000:
            return E_MIU_DDR_1024MB;
        case 0x80000000:
            return E_MIU_DDR_2048MB;
        default:
            printf("[PROTECT ERROR], not expected dram size: 0x%X\n", (unsigned int)u32DramSize);
            return -1;
    }
}

MS_U32 u32DramSize1 = 0, u32DramSize2 = 0;
void print_disabled_protect(void)
{
    if (u32DramSize1 == 0 || u32DramSize2 == 0)
    {
            printf("#######################################################################\n");
        if (u32DramSize1 == 0)
            printf("# [PROTECT WARNING], miu kernel protect is not enabled on first dram  #\n");
        if (u32DramSize2 == 0)
            printf("# [PROTECT WARNING], miu kernel protect is not enabled on second dram #\n");
            printf("#######################################################################\n");
    }
}

MS_U32 u32Lx1Start = 0, u32Lx1Size = 0;
MS_U32 u32Lx2Start = 0, u32Lx2Size = 0;
MS_U32 u32Lx3Start = 0, u32Lx3Size = 0;
MS_U32 u32Block = 0;
void MsApi_kernelProtect(void)

{
    MS_U32 u32LxStart=0,u32Size=0;
    char lx1[32],lx2[32],lx3[32], *start=0;
    char dram_size1[32], dram_size2[32];
    MS_U8 u8BlockMiu0Index=0;
    MS_U8 u8BlockMiu1Index=0;
//LX_MEM=0x9C00000 EMAC_MEM=0x100000 DRAM_LEN=0x20000000 LX_MEM2=0x50E00000,0xC200000 LX_MEM3=0xB0000000,0x0000000
    char * tok;
//MIU_INTERVAL
    char *s = getenv("MS_MEM");

    MDrv_MIU_Init();
    if (s != NULL)
    {
        u32Lx1Start = CONFIG_KERNEL_START_ADDRESS;
        u32LxStart = MsOS_VA2PA(CONFIG_KERNEL_START_ADDRESS);

        tok = strtok (s," ");
        while (tok != NULL)
        {
            if((start = strstr (tok, "LX_MEM=")) != NULL)
            {
                memset(lx1, 0, sizeof(lx1));
                strncpy(lx1, tok+7, strlen(lx1)-7);
            }
            else if((start = strstr (tok, "LX_MEM2=")) != NULL)
            {
                memset(lx2, 0, sizeof(lx2));
                strncpy(lx2, tok+8, strlen(lx2)-8);
            }
            else if((start = strstr (tok, "LX_MEM3=")) != NULL)
            {
                memset(lx3, 0, sizeof(lx3));
                strncpy(lx3, tok+8, strlen(lx3)-8);
            }
            else if((start = strstr (tok, "DRAM_SIZE1=")) != NULL)
            {
                memset(dram_size1, 0, sizeof(dram_size1));
                strncpy(dram_size1, tok+11, strlen(dram_size1)-8);
                u32DramSize1=(simple_strtoul(dram_size1, NULL, 16));
                MDrv_MIU_Dram_Size(E_MIU_0, dram_size_to_enum(u32DramSize1));
            }
            else if((start = strstr (tok, "DRAM_SIZE2=")) != NULL)
            {
                memset(dram_size2,0,sizeof(dram_size2));
                strncpy(dram_size2, tok+11, strlen(dram_size2)-8);
                u32DramSize2=(simple_strtoul(dram_size2, NULL, 16));
                MDrv_MIU_Dram_Size(E_MIU_1, dram_size_to_enum(u32DramSize2));
            }
            tok = strtok (NULL, " ");
        }

        // decide whether to do kernel protect or not
        s = getenv("KERNEL_PROTECT");
        if (s != NULL)
        {
            tok = strtok (s," ");
            while (tok != NULL)
            {
                if((start = strstr (tok, "DRAM_SIZE1=")) != NULL)
                {
                    memset(dram_size1, 0, sizeof(dram_size1));
                    strncpy(dram_size1, tok+11, strlen(dram_size1)-8);
                    u32DramSize1=(simple_strtoul(dram_size1, NULL, 16));
                    MDrv_MIU_Dram_Size(E_MIU_0, dram_size_to_enum(u32DramSize1));
                }
                else if((start = strstr (tok, "DRAM_SIZE2=")) != NULL)
                {
                    memset(dram_size2,0,sizeof(dram_size2));
                    strncpy(dram_size2, tok+11, strlen(dram_size2)-8);
                    u32DramSize2=(simple_strtoul(dram_size2, NULL, 16));
                    MDrv_MIU_Dram_Size(E_MIU_1, dram_size_to_enum(u32DramSize2));
                }
                tok = strtok (NULL, " ");
            }
        }


        //if(lx1!= NULL)
        {
            u32Lx1Size = u32Size = (simple_strtoul(lx1, NULL, 16));
            if(u32Size>0)
            {

                if(u32LxStart>=MIU_INTERVAL)
                {
                    MsDrv_kernelProtect(u8BlockMiu1Index,u32LxStart,(u32LxStart + u32Size));
                    u8BlockMiu1Index+=1;
                }
                else
                {
                    MsDrv_kernelProtect(u8BlockMiu0Index,u32LxStart,(u32LxStart + u32Size));
                    u8BlockMiu0Index+=1;
                }
            }
        }
        // if(lx2 != NULL)
        {
            tok=strtok(lx2,",");
            u32Lx2Start = u32LxStart = (simple_strtoul(tok, NULL, 16));
            tok = strtok (NULL, ",");
            u32Lx2Size = u32Size = (simple_strtoul(tok, NULL, 16));
            if(u32LxStart!=0 && u32Size!=0)
            {
                if(u32DramSize1==0x40000000)
                {
                    u32LxStart=(u32LxStart-0x40000000);
                }
                else
                {
                    u32LxStart=MsOS_VA2PA(u32LxStart);
                }
                if(u32LxStart>=MIU_INTERVAL)
                {
                    MsDrv_kernelProtect(u8BlockMiu1Index,u32LxStart,(u32LxStart + u32Size));
                    u8BlockMiu1Index+=1;
                }
                else
                {
                    MsDrv_kernelProtect(u8BlockMiu0Index,u32LxStart,(u32LxStart + u32Size));
                    u8BlockMiu0Index+=1;
                }
            }

        }

        //if(lx3 != NULL)
        {
            tok=strtok(lx3,",");
            u32Lx3Start = u32LxStart = (simple_strtoul(tok, NULL, 16));
            tok = strtok (NULL, ",");
            u32Lx3Size = u32Size = (simple_strtoul(tok, NULL, 16));
            if(u32LxStart!=0 && u32Size!=0)
            {
                if(u32DramSize1==0x40000000)
                {
                    u32LxStart=(u32LxStart-0x40000000);
                }
                else
                {
                    u32LxStart=MsOS_VA2PA(u32LxStart);
                }
                if(u32LxStart>=MIU_INTERVAL)
                {
                    MsDrv_kernelProtect(u8BlockMiu1Index,u32LxStart,(u32LxStart + u32Size));
                    u8BlockMiu1Index+=1;
                }
                else
                {
                    MsDrv_kernelProtect(u8BlockMiu0Index,u32LxStart,(u32LxStart + u32Size));
                    u8BlockMiu0Index+=1;
                }
            }
        }

         if(ENABLE_MSTAR_PM_SWIR==1)
        {
            MsDrv_PMProtect(3,0x0,0x10000); //64K run time PM
        }
    }



    // decide whether to do kernel protect bist or not
    s = getenv("kernelProtectBist");
    tok = strtok (s," ");
    if (tok != NULL && 0 == strcmp(tok, "block"))
        u32Block = 1;
}

int do_MsApi_kernelProtect( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	MsApi_kernelProtect();
	return 0;
}

extern unsigned int u32UbootStart;
extern unsigned int u32UbootEnd;
// return 1 for overlap case, 0 for non-overlap case
static MS_U32 isOverlapUboot(MS_U32 u32Start, MS_U32 u32Size)
{
    if ((u32UbootStart <= u32Start) && (u32Start <= u32UbootEnd))
        return 1;
    if ((u32UbootStart <= (u32Start+u32Size)) && ((u32Start+u32Size) <= u32UbootEnd))
        return 1;
    return 0;
}

static MS_U32 return_or_block(MS_U32 u32RetVal)
{
    if (u32Block)
        while(1);

    return u32RetVal;
}

#define TEST_OFFSET (128*1024*1024) // set 128M offset
#define TEST_OFFSET_MASK (TEST_OFFSET-1)
#define MIU1_OFFSET (0xA0000000)
#define TEST_RANGE (0x80000000) // set 2G test range for each miu
static MS_U32 write_test_to_lxmem(MS_U32 u32Start, MS_U32 u32Size)
{
    MS_U32 i;
    MS_U32 u32DramValue[u32Size/sizeof(MS_U32)];
    MS_U32* pu32Start = (MS_U32*)u32Start;
    MS_U32 failed = 0;
    MS_U32 u32DramSize = 0, u32MiuOffset = 0;
    MS_U32 u32BdmaStart = 0;
    BDMA_CpyType eBdmaCpyType;

    if (!u32Size)
    {
        printf("[PROTECT ERROR], lxmem size = 0\n");
        return return_or_block(-1);
    }

    // 1. decide dram parameters
    u32DramSize = (u32Start < MIU1_OFFSET) ? u32DramSize1 : u32DramSize2;
    u32MiuOffset = (u32Start < MIU1_OFFSET) ? 0 : MIU1_OFFSET;
    eBdmaCpyType = (u32Start < MIU1_OFFSET) ? E_BDMA_FLASH2SDRAM : E_BDMA_FLASH2SDRAM1;
    if (!u32DramSize)
    {
        printf("[PROTECT ERROR], dram size = 0\n");
        return return_or_block(-1);
    }

    // 2. backup parital values in protected region
    for (i = 0; i < sizeof(u32DramValue)/sizeof(MS_U32); i++)
        u32DramValue[i] = *(pu32Start + i);

    // 3. bdma attack
    u32BdmaStart = (u32Start & TEST_OFFSET_MASK) + u32MiuOffset;
    for (i = 0; i < (TEST_RANGE/TEST_OFFSET); i++)
    {
        u32BdmaStart += TEST_OFFSET;
        if (isOverlapUboot(u32BdmaStart, u32Size))
            continue;
        // bdma source is not important, just fill it with reasonable address
        MDrv_BDMA_CopyHnd(0x20000, u32BdmaStart, u32Size, eBdmaCpyType, 0);
    }
    flush_cache(u32Start, u32Size); // necessary, or the value would be kept in cache

    // 4. check whether the region is protected or not
    for (i = 0; i < sizeof(u32DramValue)/sizeof(MS_U32); i++)
    {
        if (u32DramValue[i] != *(pu32Start + i))
        {
            failed = 1;
            break;
        }
    }

    if (failed)
    {
        printf("[PROTECT ERROR], fail at address 0x%p \n", pu32Start + i);
        return return_or_block(-1);
    }
    else
    {
        printf("[PROTECT INFO], pass at address 0x%X, size 0x%X\n", (unsigned int)u32Start, (unsigned int)u32Size);
        return 0;
    }
}

static MS_U32 isDramExist(MS_U32 u32LxStart)
{
    if (u32LxStart < MIU1_OFFSET)
        return u32DramSize1 ? 1 : 0;
    else
        return u32DramSize2 ? 1 : 0;
}

#define TEST_SIZE (256) // it's enough to test 256 bytes on lx mem head & tail
void MsApi_kernelProtectBist(void)
{
    if (isDramExist(u32Lx1Start) && u32Lx1Size >= TEST_SIZE)
    {
        write_test_to_lxmem(u32Lx1Start, TEST_SIZE);
        write_test_to_lxmem(u32Lx1Start + u32Lx1Size - TEST_SIZE, TEST_SIZE);
    }
    if (isDramExist(u32Lx2Start) && u32Lx2Size >= TEST_SIZE)
    {
        write_test_to_lxmem(u32Lx2Start, TEST_SIZE);
        write_test_to_lxmem(u32Lx2Start + u32Lx2Size - TEST_SIZE, TEST_SIZE);
    }
    if (isDramExist(u32Lx3Start) && u32Lx3Size >= TEST_SIZE)
    {
        write_test_to_lxmem(u32Lx3Start, TEST_SIZE);
        write_test_to_lxmem(u32Lx3Start + u32Lx3Size - TEST_SIZE, TEST_SIZE);
    }
}

int do_MsApi_kernelProtectBist(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    MsApi_kernelProtectBist();
    return 0;
}


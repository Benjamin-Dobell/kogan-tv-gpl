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
#include <stdio.h>
#include <MsTypes.h>
#include <ShareType.h>
#include <MsGEGOP.h>
#include <MsApiGEGOP.h>
//#include <SN_MMAP_ANDROID_ICS_512_256.h>
#include <MsUboot.h>

int do_create_osdlayer(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    U32 u32GopBuffer;

    if (argc < 3)
    {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    MS_U16 w = simple_strtoul(argv[1], NULL, 10);
    MS_U16 h = simple_strtoul(argv[2], NULL, 10);
    MS_U32 size = w * h;

    if (0 == size)
    {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

	u32GopBuffer =GOP_BUFFER_ADDR;

    GFX_Block dstBlk = {0, 0, w, h};
    MsApiOsdCreate(&dstBlk,u32GopBuffer);
    return 0;
}
int do_destroy_osdlayer(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    MsApiOsdDestroy();
    return 0;
}
int do_flush2screen_osdlayer(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MsApiFlushCanvas2Screen();
    return 0;
}

int do_draw_rect(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 6)
    {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }
    MS_U16 x = simple_strtoul(argv[1], NULL, 10);
    MS_U16 y = simple_strtoul(argv[2], NULL, 10);
    MS_U16 w = simple_strtoul(argv[3], NULL, 10);
    MS_U16 h = simple_strtoul(argv[4], NULL, 10);
    MS_U32 tColor = simple_strtoul(argv[5], NULL, 16);

    GFX_RgbColor color;
    color.a = tColor >> 24;
    color.r = (tColor >> 16) & 0xFF;
    color.g = (tColor >> 8) & 0xFF;
    color.b = tColor & 0xFF;
    GFX_Block dstBlk = {x, y, w, h};
    MsApiDrawRect(&dstBlk, color);
	return 0;
}

int do_draw_pixel(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 4)
    {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    MS_U16 x = simple_strtoul(argv[1], NULL, 10);
    MS_U16 y = simple_strtoul(argv[2], NULL, 10);
    MS_U32 tColor = simple_strtoul(argv[3], NULL, 16);

    GFX_RgbColor color;
    color.a = tColor >> 24;
    color.r = (tColor >> 16) & 0xFF;
    color.g = (tColor >> 8) & 0xFF;
    color.b = tColor & 0xFF;
    GFX_Point p = {x, y};
    MsApiDrawPixel(p, color);
    return 0;
}
int do_draw_string(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    if (argc < 6)
    {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    MS_U16 x = simple_strtoul(argv[1], NULL, 10);
    MS_U16 y = simple_strtoul(argv[2], NULL, 10);
    MS_U32 tColor = simple_strtoul(argv[3], NULL, 16);

    GFX_RgbColor color;
    color.a = tColor >> 24;
    color.r = (tColor >> 16) & 0xFF;
    color.g = (tColor >> 8) & 0xFF;
    color.b = tColor & 0xFF;
    MS_U16 attrib = simple_strtoul(argv[4], NULL, 10);

    char cStr[202] = {0};
    MS_U16 u16Total = 0;
    MS_S16 i = 5;

    for (; i < argc; ++i)
    {
        MS_U16 u16Len = strlen(argv[i]);
        if (u16Total + u16Len > 200)
        {
            break;
        }

        memset(cStr + u16Total, 0 , (sizeof(cStr)-u16Total));
        snprintf(cStr + u16Total, (sizeof(cStr)-u16Total), "%s", argv[i]);
        u16Total += u16Len;
        cStr[u16Total++] = ' ';
    }

    cStr[u16Total] = 0;

    MsApiDrawStrText(cStr, x, y, color, (TextAttrib)attrib);
    return 0;
}
int do_draw_Progress(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    if (argc < 5)
    {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    MS_U16 x = simple_strtoul(argv[1], NULL, 10);
    MS_U16 y = simple_strtoul(argv[2], NULL, 10);
    MS_U8 per = simple_strtoul(argv[4], NULL, 10);
    MS_U32 tColor = simple_strtoul(argv[3], NULL, 16);

    GFX_RgbColor color;
    color.a = tColor >> 24;
    color.r = (tColor >> 16) & 0xFF;
    color.g = (tColor >> 8) & 0xFF;
    color.b = tColor & 0xFF;
    MsApiDrawProgress(x, y, color, per);
    return 0;
}
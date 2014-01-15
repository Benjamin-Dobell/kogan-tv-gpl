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

//--------------------------------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------------------------------
#include <common.h>
#include <MsCommon.h>
#include <MsOS.h>
#include <apiJPEG.h>
#include <uboot_mmap.h>
#include "MsDrvJpd.h"

unsigned int TARGET_LOGO_ADDR;
unsigned int TARGET_LOGO_SIZE;

int g_JpgPitch;
int g_JpgWidth;
int g_JpgHeight;

//-------------------------------------------------------------------------------------------------
//  Local Compiler Options
//-------------------------------------------------------------------------------------------------

//define jpd library
#define UTOPIA_JPD_OLD  1
#define UTOPIA_JPD_NEW  2
#define MS_JPD_API UTOPIA_JPD_NEW

//define slide show test
//#define SLIDE_SHOW_TEST

//-------------------------------------------------------------------------------------------------
// Local Defines
//-------------------------------------------------------------------------------------------------

#define JPEGBASEWIDTHMAX    1600
#define JPEGBASEHEIGHTMAX   1200
#define JPEGPROWIDTHMAX     1024
#define JPEGPROHEIGHTMAX    768

#define MsDrv_DEBUG(x)     //x

#define printf_black(args...)         do{ printf("\033[1;30m"); printf(args); printf("\033[m"); }while(0)
#define printf_red(args...)           do{ printf("\033[1;31m"); printf(args); printf("\033[m"); }while(0)
#define printf_green(args...)         do{ printf("\033[1;32m"); printf(args); printf("\033[m"); }while(0)
#define printf_yellow(args...)        do{ printf("\033[1;33m"); printf(args); printf("\033[m"); }while(0)
#define printf_blue(args...)          do{ printf("\033[1;34m"); printf(args); printf("\033[m"); }while(0)
#define printf_purple(args...)        do{ printf("\033[1;35m"); printf(args); printf("\033[m"); }while(0)
#define printf_darkgreen(args...)     do{ printf("\033[1;36m"); printf(args); printf("\033[m"); }while(0)
#define printf_white(args...)         do{ printf("\033[1;37m"); printf(args); printf("\033[m"); }while(0)



#if defined(CHIP_T3) || defined(CHIP_T4) || defined(CHIP_T7) || defined(CHIP_JANUS) || defined(CHIP_U4)
#define VERJPD_EVENT_ALL    (E_JPEG_EVENT_DEC_DONE | E_JPEG_EVENT_DEC_ERROR_MASK | E_JPEG_EVENT_DEC_MRB_DONE | E_JPEG_EVENT_DEC_MWB_FULL)
#else
#define VERJPD_EVENT_ALL    (E_JPEG_EVENT_DEC_DONE | E_JPEG_EVENT_DEC_ERROR_MASK | E_JPEG_EVENT_DEC_MRB_DONE)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Local Variables
//--------------------------------------------------------------------------------------------------
static EN_JPEG_DECODE_STATE g_state = E_JPEG_INIT;
static MS_U32 u32ReadBuffVirAddr = 0;
static MS_U32 u32IntBuffVirAddr = 0;
static MS_U32 u32OutBuffVirAddr = 0;

static MS_U16 u16ImageAlignPitch = 0, u16ImageAlignWidth = 0, u16ImageAlignHeight = 0;


static MS_U8 *u8InputDataAddr = NULL;
static MS_U32 u32InputDataLength = 0;
static MS_U32 u32ReadPosition = 0;

static MS_S32 s32InitByteRead = 0;
static MS_BOOL u8EOF_flag = FALSE;

static JPEG_Info stJPEG_Info;
static JPEG_Status stJPEG_Status;
static const MSIF_Version *pu8JPEG_Ver;

static JPEG_EXIF_DateTime _gEXIF_DateTime;
static JPEG_EXIF_Orientation _gEXIF_Orientation;

//-------------------------------------------------------------------------------------------------
// Local Function Prototypes
//-------------------------------------------------------------------------------------------------
static MS_S32 MsDrv_FILE_read(MS_U8 *Pbuf, MS_U32 max_bytes_to_read, MS_BOOL *Peof_flag);
#if 0
static MS_S32 MsDrv_FillHdrFunc(MS_U32 BufAddr, MS_U32 BufLength);
#endif
static MS_BOOL MsDrv_PreLoadBuffer(JPEG_BuffLoadType u8MRBuffType);
static void MsDrv_SetStatus(void);
//static void MsDrv_Init(void);
//static void MsDrv_StartDecode(EN_JPEG_DECODE_TYPE decode_type);

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static MS_S32 MsDrv_FILE_read(MS_U8 *Pbuf, MS_U32 max_bytes_to_read, MS_BOOL *Peof_flag)
{
    MS_U8* u8RequestDataAddr = 0;
    MS_S32 bytes_read = 0;
    MS_S32 i = 0;

    u8RequestDataAddr = Pbuf;

    MsDrv_DEBUG(printf("%s,u8RequestDataAddr = 0x%lX, FilePosition = %lu, FileLength = %lu\n"
        , __FUNCTION__, (MS_U32)u8RequestDataAddr, u32ReadPosition, u32InputDataLength));

    if (u32ReadPosition <= u32InputDataLength)
    {
        bytes_read = max_bytes_to_read;
        if ((u32ReadPosition + max_bytes_to_read) >= u32InputDataLength)
        {
            bytes_read = u32InputDataLength - u32ReadPosition;
            *Peof_flag = TRUE;
        }

        for(i = 0; i < bytes_read; i++)
        {
            u8RequestDataAddr[i] = u8InputDataAddr[u32ReadPosition + i];
        }
        MsOS_FlushMemory();
    }
    else
    {
        *Peof_flag = TRUE;
    }
    u32ReadPosition += bytes_read;
    MsDrv_DEBUG(printf("%s, bytes_read = %lu\n", __FUNCTION__, bytes_read));
    return bytes_read;
}

//--------------------------------------------------------------------------------------------------
#if 0
static MS_S32 MsDrv_FillHdrFunc(MS_PHYADDR BufAddr, MS_U32 BufLength)
{
    MS_S32 bytes_read = 0;
    MS_U32 buf_left = 0;
    MS_U32 bufVirAddr = MS_PA2KSEG1((MS_U32)BufAddr);
    MsDrv_DEBUG(printf("%s, BufAddr = %lu, bufVirAddr = %lu\n", __FUNCTION__, BufAddr, bufVirAddr));
    do
    {
        bytes_read = MsDrv_FILE_read((MS_U8 *)(bufVirAddr + buf_left), BufLength - buf_left, &u8EOF_flag);

        if (bytes_read < 0)
        {
            return bytes_read;
        }

        buf_left += bytes_read;
    } while(( buf_left < BufLength ) && ( !u8EOF_flag ));
    MApi_JPEG_UpdateReadInfo(buf_left, u8EOF_flag);
    return buf_left;
}

#endif
//--------------------------------------------------------------------------------------------------
static MS_BOOL MsDrv_PreLoadBuffer(JPEG_BuffLoadType u8MRBuffType)
{
    MS_S32 bytes_read = 0;
    MS_U32 buf_left = 0;
    MS_U32 u32RequestDataAddr, u32RequestDataSize;

    if(u8EOF_flag)
    {
        MApi_JPEG_ProcessEOF(u8MRBuffType);
        return TRUE;
    }

    u32RequestDataAddr = u32ReadBuffVirAddr;
    u32RequestDataSize = JPD_READBUFF_LEN/2;

    if(u8MRBuffType == E_JPEG_BUFFER_HIGH)
    {
        u32RequestDataAddr += u32RequestDataSize;
    }

    do
    {
        bytes_read = MsDrv_FILE_read((MS_U8 *)(u32RequestDataAddr + buf_left), u32RequestDataSize - buf_left, &u8EOF_flag);

        if (bytes_read == -1)
        {
            MApi_JPEG_SetErrCode( E_JPEG_STREAM_READ );
            return FALSE;
        }

        buf_left += bytes_read;
    } while(( buf_left < u32RequestDataSize ) && ( !u8EOF_flag ));
    MApi_JPEG_UpdateReadInfo(buf_left, u8EOF_flag);
    if(buf_left > 0)
    {
        MApi_JPEG_SetMRBufferValid(u8MRBuffType);
    }
    return TRUE;
}

//--------------------------------------------------------------------------------------------------
static EN_JPEG_DECODE_STATUS MsDrv_WaitDone(void)
{
    EN_JPEG_DECODE_STATUS eDecodeStatus = E_DECODING;
    switch(MApi_JPEG_WaitDone())
    {
        case E_JPEG_DEC_DONE:
            MsDrv_DEBUG(printf_yellow("ApiWaitDone_state: E_JPEG_DEC_DONE\n"));
            eDecodeStatus = E_DECODE_DONE;
            break;
        case E_JPEG_DEC_FAILED:
        case E_JPEG_DEC_RST_ERROR:
        case E_JPEG_DEC_BITSTREAM_ERROR:
            MsDrv_DEBUG(printf_yellow("ApiWaitDone_state: E_JPEG_DEC_FAILED or E_JPEG_DEC_RST_ERROR or E_JPEG_DEC_BITSTREAM_ERROR\n"));
            MsDrv_DEBUG(printf("Baseline decode error\n"));
            eDecodeStatus = E_DECODE_ERR;
            break;
        case E_JPEG_DEC_MRBL_DONE:
            MsDrv_DEBUG(printf_yellow("ApiWaitDone_state: E_JPEG_DEC_MRBL_DONE\n"));
            if(!MsDrv_PreLoadBuffer(E_JPEG_BUFFER_LOW))
            {
                if(MApi_JPEG_GetErrorCode() == E_JPEG_STREAM_READ)
                    eDecodeStatus = E_STREAM_READ_ERR;
                else
                    eDecodeStatus = E_DECODE_ERR;
            }
            else
            {
                eDecodeStatus = E_DECODING;
            }
            break;
        case E_JPEG_DEC_MRBH_DONE:
            MsDrv_DEBUG(printf_yellow("ApiWaitDone_state: E_JPEG_DEC_MRBH_DONE\n"));
            if(!MsDrv_PreLoadBuffer(E_JPEG_BUFFER_HIGH))
            {
                if(MApi_JPEG_GetErrorCode() == E_JPEG_STREAM_READ)
                    eDecodeStatus = E_STREAM_READ_ERR;
                else
                    eDecodeStatus = E_DECODE_ERR;
            }
            else
            {
                eDecodeStatus = E_DECODING;
            }
            break;
        case E_JPEG_DEC_DECODING:
        default:
            MsDrv_DEBUG(printf_yellow("ApiWaitDone_state: E_JPEG_DEC_DECODING\n"));
            eDecodeStatus = E_DECODING;
            break;
    }
    return eDecodeStatus;
}

//--------------------------------------------------------------------------------------------------
static void MsDrv_SetStatus(void)
{
    if( g_state == E_JPEG_DECODE_DONE )
    {
        MsDrv_DEBUG(printf("MsDrv_SetStatus:E_JPEG_DECODE_DONE\n"));
        //u16ImageWidth = MApi_JPEG_GetWidth();
        //u16ImageHeight = MApi_JPEG_GetHeight();
        //bIsProgressive = MApi_JPEG_IsProgressive();
        //u16ImageOriWidth = MApi_JPEG_GetOriginalWidth();
        //u16ImageOriHeight = MApi_JPEG_GetOriginalHeight();
        //u16ImageNonAlignWidth = MApi_JPEG_GetNonAlignmentWidth();
        //u16ImageNonAlignHeight = MApi_JPEG_GetNonAlignmentHeight();
        //u8ScaleDownFactor = MApi_JPEG_GetScaleDownFactor();
        //u8DebugLevel = MApi_JPEG_GetDbgLevel();
        MApi_JPEG_GetInfo(&stJPEG_Info);
        MApi_JPEG_GetStatus(&stJPEG_Status);
        MApi_JPEG_GetLibVer(&pu8JPEG_Ver);
        u16ImageAlignPitch = MApi_JPEG_GetAlignedPitch(),
        u16ImageAlignWidth = MApi_JPEG_GetAlignedWidth(),
        u16ImageAlignHeight = MApi_JPEG_GetAlignedHeight();
//        bThumbnailFound = MApi_JPEG_ThumbnailFound();

        MsDrv_DEBUG(printf("MsDrv_SetStatus >>>>>>>>>>> w:%d,  h:%d,  p:%d\n",u16ImageAlignWidth,u16ImageAlignHeight,u16ImageAlignPitch));
        g_JpgPitch = MApi_JPEG_GetAlignedPitch();
        g_JpgHeight = MApi_JPEG_GetAlignedHeight();
        g_JpgWidth = MApi_JPEG_GetAlignedWidth();

        MApi_JPEG_GetEXIFDateTime(&_gEXIF_DateTime);
        MApi_JPEG_GetEXIFOrientation(&_gEXIF_Orientation);
    }
    else if( g_state == E_JPEG_DECODE_ERR )
    {
        MsDrv_DEBUG(printf("MsDrv_SetStatus:E_JPEG_DECODE_ERR\n"));
        //u16ImageWidth = MApi_JPEG_GetWidth();
        //u16ImageHeight = MApi_JPEG_GetHeight();
        //bIsProgressive = MApi_JPEG_IsProgressive();
        //u16ImageOriWidth = MApi_JPEG_GetOriginalWidth();
        //u16ImageOriHeight = MApi_JPEG_GetOriginalHeight();
        //u16ImageNonAlignWidth = MApi_JPEG_GetNonAlignmentWidth();
        //u16ImageNonAlignHeight = MApi_JPEG_GetNonAlignmentHeight();
        //u8ScaleDownFactor = MApi_JPEG_GetScaleDownFactor();
        //u16ErrorCode = MApi_JPEG_GetErrorCode();
        //u8DebugLevel = MApi_JPEG_GetDbgLevel();
        MApi_JPEG_GetInfo(&stJPEG_Info);
        MApi_JPEG_GetStatus(&stJPEG_Status);
        MApi_JPEG_GetLibVer(&pu8JPEG_Ver);
        u16ImageAlignPitch = MApi_JPEG_GetAlignedPitch(),
        u16ImageAlignWidth = MApi_JPEG_GetAlignedWidth(),
        u16ImageAlignHeight = MApi_JPEG_GetAlignedHeight();
//        bThumbnailFound = MApi_JPEG_ThumbnailFound();
        MApi_JPEG_GetEXIFDateTime(&_gEXIF_DateTime);
        MApi_JPEG_GetEXIFOrientation(&_gEXIF_Orientation);
    }
}

//--------------------------------------------------------------------------------------------------
/// API for JPEG decoder initialization::
void MsDrv_JpdInit(void)
{
    MsDrv_DEBUG(printf_red("%s\n", __FUNCTION__));

    //for test code
    g_state = E_JPEG_INIT;
    //u16ImageWidth = 0;
    //u16ImageHeight = 0;
    //u16ImageOriWidth = 0;
    //u16ImageOriHeight = 0;
    //u16ImageNonAlignWidth = 0;
    //u16ImageNonAlignHeight = 0;
    //u16ErrorCode = 0;
    //u8ScaleDownFactor = 0;
    //bIsProgressive = FALSE;

    //u8DebugLevel = 0x0;

    s32InitByteRead = 0;
    u8EOF_flag = FALSE;

    u32ReadBuffVirAddr = MS_PA2KSEG1((MS_U32)JPD_READBUFF_ADR);
    u32IntBuffVirAddr = MS_PA2KSEG1((MS_U32)JPD_INTERBUFF_ADR);
    u32OutBuffVirAddr = MS_PA2KSEG1((MS_U32)JPD_OUT_ADR);

    MsDrv_DEBUG(printf("u32ReadBuffVirAddr = %lx, u32IntBuffVirAddr = %lx, u32OutBuffVirAddr = %lx\n", u32ReadBuffVirAddr, u32IntBuffVirAddr, u32OutBuffVirAddr));


#if 0
    #if (TEST_EXAMPLE==BASELINE_EXAMPLE)
    u8InputDataAddr = (MS_U8 *)u8BaseLineJPEG;
    u32InputDataLength = sizeof(u8BaseLineJPEG);
    #elif (TEST_EXAMPLE==PROGRESSIVE_EXAMPLE)
    u8InputDataAddr = (MS_U8 *)u8ProgressiveJPEG;
    u32InputDataLength = sizeof(u8ProgressiveJPEG);
    #endif
#else
    //logo jpd decode using BaseLine
    //u8InputDataAddr = (MS_U8 *)TARGET_LOGO_ADDR;
    u8InputDataAddr = (MS_U8 *) MS_PA2KSEG1((MS_U32)TARGET_LOGO_ADDR);
    u32InputDataLength = TARGET_LOGO_SIZE;
#endif


    u32ReadPosition = 0;

    _gEXIF_DateTime.bHasDataTime = FALSE;
    _gEXIF_DateTime.u32Year = 0;
    _gEXIF_DateTime.u32Month = 0;
    _gEXIF_DateTime.u32Day = 0;
    _gEXIF_DateTime.u32Hour = 0;
    _gEXIF_DateTime.u32Minute = 0;
    _gEXIF_DateTime.u32Second = 0;
    _gEXIF_Orientation = E_JPEG_EXIF_ORIENT_NOT_FOUND;

    //MsDrv_InitFileSystem();
    MApi_JPEG_SetDbgLevel(E_JPEG_DEBUG_NONE);

    //MApi_JPEG_SetDbgLevel(E_JPEG_DEBUG_ALL);
}

//--------------------------------------------------------------------------------------------------
/// API for starting JPEG data decoding
//--------------------------------------------------------------------------------------------------
void MsDrv_JpdStartDecode(EN_JPEG_DECODE_TYPE decode_type)
{
    if(g_state != E_JPEG_INIT)
    {
        MsDrv_DEBUG(printf("%s failed, state = %d\n", __FUNCTION__, g_state));
        return;
    }

    // init JPEG DECODE BUFFER
    JPEG_InitParam InitParam;

    MsDrv_DEBUG(printf("%s: INIT\n", __FUNCTION__));

    MApi_JPEG_SetMaxDecodeResolution(JPEGBASEWIDTHMAX, JPEGBASEHEIGHTMAX);
    MApi_JPEG_SetProMaxDecodeResolution(JPEGPROWIDTHMAX, JPEGPROHEIGHTMAX);

    //InitParam.u32MRCBufAddr =  MS_VA2PA((((unsigned int)0x80A01000)|0x20000000) + 0x278000);  // (((unsigned int)0x80A01000)|0x20000000) + 0x278000; //MsDrv_READBUFF_ADR;
    //InitParam.u32MRCBufAddr =  MS_VA2PA((((unsigned int)0x80A01000)|0x20000000) + ResBIN_LogoOffset);
    InitParam.u32MRCBufAddr =  TARGET_LOGO_ADDR;
    InitParam.u32MRCBufSize = JPD_READBUFF_LEN;
    InitParam.u32MWCBufAddr = JPD_OUT_ADR;
    InitParam.u32MWCBufSize = JPD_OUT_LEN;
    InitParam.u32InternalBufAddr = JPD_INTERBUFF_ADR;
    InitParam.u32InternalBufSize = JPD_INTERBUFF_LEN;
    InitParam.u32DecByteRead = TARGET_LOGO_SIZE;// file size
    InitParam.bEOF = TRUE;  //u8EOF_flag; // TRUE, the number of read_act, End of File ?
    if(E_JPD_MAIN_DECODE == decode_type)
    {
        InitParam.u8DecodeType = E_JPEG_TYPE_MAIN;
    }
    else
    {
        InitParam.u8DecodeType = E_JPEG_TYPE_THUMBNAIL;
    }
    InitParam.bInitMem = TRUE;
    InitParam.pFillHdrFunc = NULL; //(JPEG_FillHdrFunc)MsDrv_FillHdrFunc; // NULL

    MApi_JPEG_Init(&InitParam);

    if(MApi_JPEG_GetErrorCode() == E_JPEG_NO_ERROR)
    {
        g_state = E_JPEG_DECODE_HEADER;
    }
    else
    {
        g_state = E_JPEG_DECODE_ERR;
        MsDrv_DEBUG(printf("%s: Init failed\n", __FUNCTION__));
    }

    MsDrv_DEBUG(printf("%s: INIT DONE : EventFlag = 0x%04X\n", __FUNCTION__, MApi_JPEG_GetJPDEventFlag()));

    //parse JPEG Header
    if(g_state == E_JPEG_DECODE_HEADER)
    {
        MsDrv_DEBUG(printf_red("%s: DECODE HEADER=======================================\n", __FUNCTION__));
        if(E_JPEG_FAILED == MApi_JPEG_DecodeHdr())
        {
            if(MApi_JPEG_GetErrorCode() == E_JPEG_STREAM_READ)
            {
                MsDrv_DEBUG(printf("%s: DecodeHdr read failed\n", __FUNCTION__));
                g_state = E_JPEG_IDLE;
            }
            else
            {
                MsDrv_DEBUG(printf("%s: DecodeHdr failed\n", __FUNCTION__));
                g_state = E_JPEG_DECODE_ERR;
            }
        }
        else
        {
            g_state = E_JPEG_DECODE_BEGIN;
        }
    }
    MsDrv_DEBUG(printf("%s: DECODE HEADER DONE : EventFlag = 0x%04X\n", __FUNCTION__, MApi_JPEG_GetJPDEventFlag()));

    if(g_state == E_JPEG_IDLE)
    {
        MsDrv_DEBUG(printf("%s: IDLE\n", __FUNCTION__));
        MApi_JPEG_Rst();
        MApi_JPEG_Exit();
        g_state = E_JPEG_INIT;
        return;
    }

    MsDrv_DEBUG(printf_red("%s: DECODE BEGIN========================================\n", __FUNCTION__));
    //begin decode
    while(g_state == E_JPEG_DECODE_BEGIN)
    {
        switch(MApi_JPEG_Decode())
        {
            case E_JPEG_DONE:
                // This state is only for progressive JPEG file
                MsDrv_DEBUG(printf_blue("Decode_state: E_JPEG_DONE\n"));
                g_state = E_JPEG_DECODE_BEGIN;
                break;
            case E_JPEG_OKAY:
                MsDrv_DEBUG(printf_blue("Decode_state: E_JPEG_OKAY\n"));
                g_state = E_JPEG_WAITDONE;
                break;
            case E_JPEG_FAILED:
            default:
                MsDrv_DEBUG(printf_blue("Decode_state: E_JPEG_FAILED\n"));
                MsDrv_DEBUG(printf("MsDrv_StartDecode: DecodeBegin failed\n"));
                g_state = E_JPEG_DECODE_ERR;
                break;
        }
    }
    MsDrv_DEBUG(printf("%s: DECODE BEGIN DONE : EventFlag = 0x%04X\n", __FUNCTION__, MApi_JPEG_GetJPDEventFlag()));

    //wait done
    MsDrv_DEBUG(printf_red("%s: WAITDONE============================================\n", __FUNCTION__));
    while(g_state == E_JPEG_WAITDONE)
    {
        switch(MsDrv_WaitDone())
        {
            case E_DECODE_DONE:
                MsDrv_DEBUG(printf_blue("APPWaitDone_state: E_DECODE_DONE\n"));
                printf("[AutoTest][MBoot][JPD Decode]\n");
                g_state = E_JPEG_DECODE_DONE;
                break;

            case E_DECODING:
                MsDrv_DEBUG(printf_blue("APPWaitDone_state: E_DECODING\n"));
                g_state = E_JPEG_WAITDONE;
                break;

            case E_STREAM_READ_ERR:
            case E_DECODE_ERR:
            default:
                MsDrv_DEBUG(printf_blue("APPWaitDone_state: E_STREAM_READ_ERR or E_DECODE_ERR\n"));
                MsDrv_DEBUG(printf("%s: WaitDone failed\n", __FUNCTION__));
                g_state = E_JPEG_DECODE_ERR;
                MApi_JPEG_Rst();
                break;
        }
    }

    MsDrv_SetStatus();
    MApi_JPEG_Exit();
    g_state = E_JPEG_INIT;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
int JPG_GetAlignmentWidth(void)
{
    return g_JpgWidth;
}
int JPG_GetAlignmentHeight(void)
{
    return g_JpgHeight;
}
int JPG_GetAlignmentPitch(void)
{
    return g_JpgPitch;
}
MS_U32 JPG_GetOutRawDataAddr(void)
{
    return JPD_OUT_ADR;
}
//--------------------------------------------------------------------------------------------------
///Entry function for JPD testing.
//--------------------------------------------------------------------------------------------------
void MsDrv_JPD_Decode(MS_U32 u32SrcAddr, MS_U32 u32SrcSize)
{
    MsDrv_DEBUG(printf("MsDrv_Main... Start!!\n"));

    TARGET_LOGO_ADDR = u32SrcAddr;
    TARGET_LOGO_SIZE = u32SrcSize;

    MsDrv_JpdInit();

    //Preload data for init JPEG decoder data.
    s32InitByteRead = MsDrv_FILE_read((MS_U8 *)u32ReadBuffVirAddr, JPD_READBUFF_LEN, &u8EOF_flag);

    if(s32InitByteRead < 0)
    {
        MsDrv_DEBUG(printf("Initial read file error!!\n"));
    }

    MsDrv_JpdStartDecode(E_JPD_MAIN_DECODE);

    MsDrv_DEBUG(printf("MsDrv_Main... End!!\n"));

    //MsDrv_DEBUG(printf_red("Dump Memory Addr=0x%x, output: %dx%d.yuyv, size=0x%x\n" ,MsDrv_OUTBUFF_ADR,
    //    u16ImageWidth, u16ImageHeight, u16ImageWidth*u16ImageHeight*2));
}



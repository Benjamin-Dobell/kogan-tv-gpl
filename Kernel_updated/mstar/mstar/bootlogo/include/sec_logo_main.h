////////////////////////////////////////////////////////////////////////////////
////
//// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
//// All rights reserved.
////
//// Unless otherwise stipulated in writing, any and all information contained
//// herein regardless in any format shall remain the sole proprietary of
//// MStar Semiconductor Inc. and be kept in strict confidence
//// (MStar Confidential Information) by the recipient.
//// Any unauthorized act including without limitation unauthorized disclosure,
//// copying, use, reproduction, sale, distribution, modification, disassembling,
//// reverse engineering and compiling of the contents of MStar Confidential
//// Information is unlawful and strictly prohibited. MStar hereby reserves the
//// rights to any and all damages, losses, costs and expenses resulting therefrom.
////
//////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////
///// @file   drvSystem.h
///// @brief  System Control Interface
///// @author MStar Semiconductor Inc.
/////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __SEC_LOGO_H__
#define __SEC_LOGO_H__
typedef unsigned short U16;

typedef enum __colorformat__
{
    RGB1555_BLINK = 0,
    RGB565 = 1,
    ARGB4444 = 2,
    _2266 = 3,
    _8BIT_PALETTE = 4,
    ARGB8888 = 5,
    ARGB1555 = 6,
    ABGR8888 = 7,
    RGB1555_UV7Y8 = 8,
    UV8Y8 = 9,
    RGBA5551 = 10,
    RGBA4444 = 11,
    CLRFMAT_MAX
}COLORFORMAT;

typedef struct __attribute__ ((packed))
{
    unsigned short int type; /* type : Magic identifier,BM(0x42,0x4d) */
    unsigned int size; /* File size in bytes */
    unsigned short int reserved1, reserved2; /* reserved */
    unsigned int offset; /* Offset to image data, bytes */
} FILEHEADER;

typedef struct __attribute__ ((packed))
{
    unsigned int size;/* Info Header size in bytes */
    int width,height;/* Width and height of image */
    unsigned int clrfmt;
    unsigned short int planes;/* Number of colour planes */
    unsigned short int bits; /* Bits per pixel */
    unsigned int compression; /* Compression type */
    unsigned int imagesize; /* Image size in bytes */
    int xresolution,yresolution; /* Pixels per meter */
    unsigned int ncolours; /* Number of colours */
    unsigned int importantcolours; /* Important colours */
    char reserved[32];
} INFOHEADER;

#endif /* __SEC_LOGO_H__ */


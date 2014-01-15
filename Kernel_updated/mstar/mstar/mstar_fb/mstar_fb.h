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

/******************************************************************************
 *-----------------------------------------------------------------------------
 *
 *	Copyright (c) 2011 MStar Semiconductor, Inc.  All rights reserved.
 *
 *-----------------------------------------------------------------------------
 * FILE NAME      mstar_fb.h
 * DESCRIPTION
 *
 * AUTHOR
 *
 * HISTORY
 *
 *        linux/drivers/video/mstar_fb.h    --    mstar_fb frame buffer device
 ******************************************************************************/

#ifndef MSTAR_FB_H     /* in frame buffer driver */
#define MSTAR_FB_H     /* in frame buffer driver */

/* define type cast macro */
#define CAST_2_USER_VOID_P          (void __user *)
#define CAST_2_USER_UINT_P          (u32 __user *)
#define CAST_2_IOMEM_CHAR_P         (u8 __iomem *)
#define CAST_2_ULONG                (unsigned long)

/*********************** GOP data define  ******************************/
/// Define GOP HW Type
typedef enum
{
    /// GOP 0 (4 Gwin Support)
    E_GOP4G_0 = 0,
    /// GOP 1 (2 Gwin Support)
    E_GOP2G_1 = 1,
    /// GOP SIGNAL (1 Gwin Support)
    E_GOP1G = 2,
    /// GOP SIGNAL (1 Gwin Support which can be used for cursor)
    E_GOP1GX = 3,
}GOP_HW_Type;

typedef enum
{
    /// Color format RGB555 and Blink.
    E_GOP_COLOR_RGB555_BLINK    =0,
    /// Color format RGB565.
    E_GOP_COLOR_RGB565          =1,
    /// Color format ARGB4444.
    E_GOP_COLOR_ARGB4444        =2,
    /// Color format 555.
    E_GOP_COLOR_RGB555          =3,
    /// Color format I8 (256-entry palette).
    E_GOP_COLOR_I8              =4,
    /// Color format ARGB8888.
    E_GOP_COLOR_ARGB8888        =5,
    /// Color format ARGB1555.
    E_GOP_COLOR_ARGB1555        =6,
    /// Color format I2.
    E_GOP_COLOR_I2              =7,
    /// Color format YUV422_VU7Y8, Packet format: V7Y8,U7Y8,......
    E_GOP_COLOR_YUV422_VU7Y8    =8,
    /// Color format YUV422_VU8Y8. Packet format: V8Y8,U8Y8,......
    E_GOP_COLOR_YUV422_VU8Y8    =9,
    /// Color format I1
    E_GOP_COLOR_I1              =10,
    /// Invalid color format.
    E_GOP_COLOR_INVALID
} GOPColorType;
typedef union
{
    ///RGB Based
    struct
    {
        /// B8
        U8 u8B;
        /// G8
        U8 u8G;
        /// R8
        U8 u8R;
        /// Reserved
        U8 u8Reserved;
    }RGB;
    U32 u32Data;
}GOPTRSColor;


typedef struct GFX_FBCON_INFO_T
{
    U32 addr; // base addr
    U16 x;
    U16 y;
    U16 w;
    U16 h;
}GFX_FBCON_INFO;

typedef struct
{
    U32 u32Phyaddr;		
    U8 u8Gop;			
} MSTAR_INIT_INFO;

/* ========================================================================= */
/*
 * Ioctl definitions
 */

/* Use 'F' as magic number.
   in Documnentation/ioctl-number.txt says 'F'    all    linux/fb.h */

#define MS_FB_IOC_MAGIC  'F'

#define MS_FB_INIT    _IO(MS_FB_IOC_MAGIC,    0x21)
#define MS_FB_SETPIVOT   _IO(MS_FB_IOC_MAGIC, 0x22)
#define MS_FB_SETMUX   _IO(MS_FB_IOC_MAGIC,  0x23)
#define MS_FB_IOC_MAXNR 0x24  //Edgar

/* ========================================================================= */
/* prototype of function */
//static int mstar_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg);
static int mstar_fb_mmap(struct fb_info *info, struct vm_area_struct *vma);
static int mstar_fb_ioctl(struct fb_info *pinfo, unsigned int cmd, unsigned long arg);
static int mstar_fb_set_par(struct fb_info *info);
static int mstar_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
static int mstar_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info);
static int mstar_fb_blank(int blank, struct fb_info *info);
static int mstar_fb_setcolreg(unsigned regno, unsigned red, unsigned green,
                              unsigned blue, unsigned transp, struct fb_info *info);
static void mstar_fb_destroy(struct fb_info *info);
static void mstar_fb_fillrect(struct fb_info *p, const struct fb_fillrect *rect);
static void mstar_fb_copyarea(struct fb_info *p, const struct fb_copyarea *area);
static void mstar_fb_imageblit(struct fb_info *p, const struct fb_image *image);
#endif


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
 *  Copyright (c) 2011 MStar Semiconductor, Inc.  All rights reserved.
 *
 *-----------------------------------------------------------------------------
 * FILE NAME      mstar_fb.c
 * DESCRIPTION    For Display driver, use frame buffer
 *                architecture
 *
 * AUTHOR
 *
 *
 * HISTORY
 *                linux/drivers/video/mstar_fb.c
 *                -- frame buffer device
 *****************************************************************************/


#include <linux/pfn.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>               /* for dma_alloc_coherent */
#include <linux/slab.h>
#include <linux/vmalloc.h>          /* seems do not need this */
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>      /* for dma_alloc_coherent */
#include <linux/platform_device.h>
#include <linux/io.h>
#include <asm/uaccess.h>            /* access_ok(), and VERIFY_WRITE/READ */
#include <linux/ioctl.h>            /* for _IO() macro */
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/time.h>             /* do_gettimeofday() */
#include "mdrv_types.h"
#include "mdrv_system.h"
#include "halGOP.h"
#include "drvGOP.h"
#include "mstar_fb.h"


/* 0:not print debug message, 1:print debug message */
#define DISPLAY_DEBUG 0
#if (DISPLAY_DEBUG==1)
#define DBG(fmt, arg...) printk(KERN_INFO fmt, ##arg)
#else
#define DBG(fmt, arg...)
#endif
////////////////////////////////////////
//  memory pool define
//
////////////////////////////////////////
/* GOP Definition */
/* NOTE: Use SD_GFX1 plane */
#define WINID        0
#define COLOR_TYPE   5

#define BPX                 32    /* bits per pixel */
#define PANEL_WIDTH         1920
#define PANEL_HEIGHT        1080
#define PANEL_H_START       112
#define XRES                1920 
#define YRES                1080
#define V_XRES              XRES
#define V_YRES              YRES*2 /*  x2 for double buffer */
#define VIDEOMEMSIZE        (V_XRES * V_YRES * (BPX >> 3))  /* frame buffer size */

#if defined(CONFIG_ARM)
#define MIU0_BUS_OFFSET 	ARM_MIU0_BUS_BASE
#define MIU1_INTERVAL 		(ARM_MIU1_BUS_BASE - ARM_MIU0_BUS_BASE)
#elif defined(CONFIG_MIPS)
#define MIU0_BUS_OFFSET 	MIPS_MIU0_BUS_BASE
#define MIU1_INTERVAL 		(MIPS_MIU1_BUS_BASE - MIPS_MIU0_BUS_BASE)
#endif
static int mstar_fb_enable __initdata = 1;	/* disabled by default */
static unsigned long _fb_phy_addr = 0x0;
static U8 bMirror=0;
static U8 GOPID=0xFF;
static U8 bGwinEnable=FALSE;
module_param(_fb_phy_addr, ulong, S_IRUGO|S_IWUSR);

/* Device independent changeable information */
static struct fb_var_screeninfo mstar_fb_var __initdata =
{
    .xres = XRES,
    .yres = YRES,
    .xres_virtual = V_XRES,
    .yres_virtual = V_YRES,
    .bits_per_pixel = 32, //BPX,
    .red =    { 16, 8, 0 },
    .green =  { 8 , 8, 0 },
    .blue =   { 0 , 8, 0 },
    .transp = { 24, 8, 0 },
    .activate = FB_ACTIVATE_NOW,
    .height = -1, /* =maximum, in fb.h is delcalred as _u32* */
    .width = -1,

    /*  timing useless ? use the vfb default */
    .pixclock = 20000, /* pixel clock in ps (pico seconds) */
    .left_margin = 64, /* time from sync to picture */
    .right_margin = 64, /* time from picture to sync */
    .upper_margin = 32, /* time from sync to picture */
    .lower_margin = 32,
    .hsync_len = 64,    /* length of horizontal sync */
    .vsync_len = 2,     /* length of vertical sync */
    .vmode = FB_VMODE_NONINTERLACED,
    /* non interlaced, the most basical framebuffer */
};

/* Device independent unchangeable information */
static struct fb_fix_screeninfo mstar_fb_fix __initdata =
{
    .id = "MStar FB",
    .type = FB_TYPE_PACKED_PIXELS,    /* see FB_TYPE_* */
    .visual = FB_VISUAL_TRUECOLOR,    /* see FB_VISUAL_* */
    .xpanstep = (GOP_WordUnit/(BPX >> 3)),  /* zero if no hardware panning */
    .ypanstep = 1,
    .ywrapstep = 1,
    .line_length = (V_XRES * (BPX  >> 3)), /* length of a line in bytes */
    .mmio_start=0x08000000,	/* Start of Memory Mapped I/O   */
					/* (physical address) */
    .mmio_len=0x10000000,			/* Length of Memory Mapped I/O  */
    .accel = FB_ACCEL_NONE,
    /* Indicate to driver which	specific chip/card we have */
};


static struct fb_ops mstar_fb_ops =
{
    .owner = THIS_MODULE,
    .fb_mmap = mstar_fb_mmap,
    .fb_ioctl = mstar_fb_ioctl,
    .fb_set_par = mstar_fb_set_par,
    .fb_check_var = mstar_fb_check_var,
    .fb_blank = mstar_fb_blank,
    .fb_pan_display = mstar_fb_pan_display,
    .fb_setcolreg = mstar_fb_setcolreg,
    .fb_fillrect = mstar_fb_fillrect,
    .fb_copyarea = mstar_fb_copyarea,
    .fb_imageblit = mstar_fb_imageblit,
    .fb_destroy = mstar_fb_destroy,
};
static U32 get_line_length(int xres_virtual, int bpp)
{
	U32 length;

	length = xres_virtual * bpp;
	length = (length + 31) & ~31;
	length >>= 3;
	return (length);
}
static DRV_GOPColorType get_color_fmt(struct fb_var_screeninfo *var)
{
	DRV_GOPColorType ColorFmt = E_DRV_GOP_COLOR_INVALID;
		
	switch (var->bits_per_pixel) {
	case 1:
	case 8:
		var->red.offset = 0;
		var->red.length = 8;
		var->green.offset = 0;
		var->green.length = 8;
		var->blue.offset = 0;
		var->blue.length = 8;
		var->transp.offset = 0;
		var->transp.length = 0;
		ColorFmt = E_DRV_GOP_COLOR_I8;
		break;
	case 16:		
		if ( (var->transp.length) && (var->transp.offset == 15)) {
			var->blue.offset = 0;
			var->blue.length = 5;
			var->green.offset = 5;
			var->green.length = 5;
			var->red.offset = 10;
			var->red.length = 5;
			var->transp.offset = 15;
			var->transp.length = 1;
			ColorFmt = E_DRV_GOP_COLOR_ARGB1555;
		}else if ( (var->transp.length) && (var->transp.offset == 12)) {
			var->blue.offset = 0;
			var->blue.length = 4;
			var->green.offset = 4;
			var->green.length = 4;
			var->red.offset = 8;
			var->red.length = 4;
			var->transp.offset = 12;
			var->transp.length = 4;
			ColorFmt = E_DRV_GOP_COLOR_ARGB4444;	
		}else if ( (var->transp.length) && (var->transp.offset == 0)) {
			var->transp.offset = 0;
			var->transp.length = 1;
			var->blue.offset = 1;
			var->blue.length = 5;
			var->green.offset = 6;
			var->green.length = 5;
			var->red.offset = 11;
			var->red.length = 5;
			ColorFmt = E_DRV_GOP_COLOR_RGBA5551;
		} else {	/* RGB 565 */
			var->blue.offset = 0;
			var->blue.length = 5;
			var->green.offset = 5;
			var->green.length = 6;
			var->red.offset = 11;
			var->red.length = 5;
			var->transp.offset = 0;
			var->transp.length = 0;
			ColorFmt = E_DRV_GOP_COLOR_RGB565;
		}
		break;
	case 32:		/* ARGB 8888 */
		if ( (var->transp.length) && (var->red.offset == 16)) {
			var->blue.offset = 0;
			var->blue.length = 8;
			var->green.offset = 8;
			var->green.length = 8;
			var->red.offset = 16;
			var->red.length = 8;
			var->transp.offset = 24;
			var->transp.length = 8;
			ColorFmt = E_DRV_GOP_COLOR_ARGB8888;
		}else{
			var->red.offset = 0;
			var->red.length = 8;
			var->green.offset = 8;
			var->green.length = 8;
			var->blue.offset = 16;
			var->blue.length = 8;
			var->transp.offset = 24;
			var->transp.length = 8;
			ColorFmt = E_DRV_GOP_COLOR_ABGR8888;
		}
		break;
	}
	var->red.msb_right = 0;
	var->green.msb_right = 0;
	var->blue.msb_right = 0;
	var->transp.msb_right = 0;
	return ColorFmt;
}

static void _fb_buf_init(struct fb_info *pinfo, unsigned long pa)
{
    pinfo->fix.smem_start = pa;
    pinfo->fix.smem_len = pinfo->var.xres * pinfo->var.yres * (pinfo->var.bits_per_pixel >> 3)*2/*double buffer*/;
    pinfo->screen_base = (char __iomem *)ioremap(pa + MIU0_BUS_OFFSET, pinfo->fix.smem_len);
    memset(pinfo->screen_base, 0x0, pinfo->fix.smem_len);
}
static void _fb_gwin_init(void)
{
	_MDrv_GOP_Init(GOPID);
	_MDrv_GOP_SetDstPlane(GOPID,E_DRV_GOP_DST_OP0);
	_MDrv_GOP_SetPipeDelay(GOPID,(U16)PANEL_H_START);

	if( _fb_phy_addr >= MIU1_INTERVAL)
		_MDrv_GOP_MIU_Sel(GOPID,E_DRV_GOP_SEL_MIU1);
	else
		_MDrv_GOP_MIU_Sel(GOPID,E_DRV_GOP_SEL_MIU0);
	_MDrv_GOP_UpdateReg(GOPID);
}
static void _fb_strewin_update(struct fb_var_screeninfo *var)
{
	_MDrv_GOP_SetStretchWin(GOPID,0,0,var->xres,var->yres);
	_MDrv_GOP_SetHScale(GOPID,TRUE,var->xres,PANEL_WIDTH);
	_MDrv_GOP_SetVScale(GOPID,TRUE,var->yres,PANEL_HEIGHT);
}
static void _fb_gwin_update(struct fb_var_screeninfo *var, struct fb_info *pinfo)
{
	DRV_GWIN_INFO	WinInfo;
	U32 bytes_per_pixel;
	
	if(var->bits_per_pixel == 1)
		bytes_per_pixel = 1;
	else
		bytes_per_pixel = var->bits_per_pixel/8;

	if( _fb_phy_addr >= MIU1_INTERVAL)
		WinInfo.u32Addr = _fb_phy_addr - MIU1_INTERVAL;
	else
		WinInfo.u32Addr = _fb_phy_addr;

	if( var->xoffset || var->yoffset)
	{
		WinInfo.u32Addr += (var->xoffset + var->xres_virtual*var->yoffset)* bytes_per_pixel;
	}
	if(bMirror)
	{
		if(GOPID != 0)
		{
#if (bAutoAdjustMirrorHSize == TRUE)
			WinInfo.u32Addr += ((var->xres - var->xres_virtual) +var->xres_virtual*(var->yres-1))* bytes_per_pixel;
#else
			WinInfo.u32Addr += ((var->xres - pinfo->fix.xpanstep) + var->xres_virtual*(var->yres-1))* bytes_per_pixel;
#endif
		}	
	}

	WinInfo.clrType = get_color_fmt(var);
	WinInfo.u16HStart = 0;
	WinInfo.u16HEnd = var->xres;
	WinInfo.u16VStart = 0;
	WinInfo.u16VEnd = var->yres;
	WinInfo.u16Pitch = var->xres_virtual;
	_MDrv_GOP_SetGwinInfo(GOPID,WINID,WinInfo);
	_MDrv_GOP_EnableHMirror(GOPID,bMirror);
	_MDrv_GOP_EnableVMirror(GOPID,bMirror);
	_MDrv_GOP_SetBlending(GOPID,WINID,TRUE,0xFF);
	_MDrv_GOP_UpdateReg(GOPID);
}
static void _fb_gwin_enable(U8 bEnable)
{
	_MDrv_GOP_GWIN_Enable(GOPID,WINID,bEnable);
	_MDrv_GOP_UpdateReg(GOPID);
}
static int mstar_fb_mmap(struct fb_info *pinfo, struct vm_area_struct *vma)
{
	size_t size;
	size = 0;
	if (NULL == pinfo)
	{
	    DBG("ERROR: mstar_fb_mmap, pinfo is NULL pointer !\n");
	    return -ENOTTY;
	}
	if (NULL == vma)
	{
	    DBG("ERROR: mstar_fb_mmap, vma is NULL pointer !\n");
	    return -ENOTTY;
	}
	if (0 == pinfo->fix.smem_start)
	{
	    DBG("ERROR: mstar_fb_mmap, physical addr is NULL pointer !\n");
	    return -ENOTTY;
	}

	size = vma->vm_end - vma->vm_start;

	vma->vm_pgoff = (pinfo->fix.smem_start + MIU0_BUS_OFFSET) >> PAGE_SHIFT;
	
	DBG(
	"\33[0;36m 2 mstar_fb_mmap vma->vm_start=%x\n vma->vm_end=%x\n vma->vm_pgoff =%x \33[m \n",
	(unsigned int) vma->vm_start, (unsigned int)vma->vm_end ,
	(unsigned int)vma->vm_pgoff);

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	/* Remap-pfn-range will mark the range VM_IO and VM_RESERVED */
	if (remap_pfn_range(vma, vma->vm_start,
	                    vma->vm_pgoff, size, vma->vm_page_prot))
	    return -EAGAIN;

	return 0;	
}
static int mstar_fb_ioctl(struct fb_info *pinfo, unsigned int cmd, unsigned long arg)
{
    int err, ret;
    err = 0;
    ret = 0;
				
    if (NULL == pinfo)
    {
        DBG("ERROR:in mstar_fb_ioctl, pinfo is NULL pointer\r\n");
        return -ENOTTY;
    }
	
    /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
    if (_IOC_TYPE(cmd) != MS_FB_IOC_MAGIC)
        return -ENOTTY;
    if (_IOC_NR(cmd) > MS_FB_IOC_MAXNR)
        return -ENOTTY;
    /*
     * the type is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. Note that the type is user-oriented, while
     * verify_area is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, CAST_2_USER_VOID_P arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  !access_ok(VERIFY_READ, CAST_2_USER_VOID_P arg, _IOC_SIZE(cmd));
    /* check if occurred error */
    if (err)
        return -EFAULT;
    switch (cmd)
    {
        case MS_FB_INIT:
        {
            if(0 == _fb_phy_addr)
            {
                MSTAR_INIT_INFO Init;
                ret = __copy_from_user(&Init, (MSTAR_INIT_INFO __user*) arg, sizeof(Init));
				if((Init.u32Phyaddr != 0) && (Init.u8Gop<MAX_GOP_MUX))
				{
					_fb_phy_addr = Init.u32Phyaddr;
					GOPID = Init.u8Gop;
					_fb_buf_init(pinfo, _fb_phy_addr);
					_fb_gwin_init();				    
					_fb_strewin_update(&mstar_fb_var);
					_fb_gwin_update(&mstar_fb_var,pinfo);
				}
            }
        }
        break;
		
        case MS_FB_SETPIVOT:
        {
            int hvPivot;
            ret = copy_from_user(&hvPivot, (int*) arg, sizeof(hvPivot));
			bMirror = hvPivot;
            break;
        }
		
		case MS_FB_SETMUX:
        {
			GOP_MuxConfig GopMuxConfig;	
            ret = copy_from_user(&GopMuxConfig, (GOP_MuxConfig*) arg, sizeof(GopMuxConfig));
			_MDrv_GOP_SetMux(GopMuxConfig);	
            break;
        }

        /*====================================
         * not implement
         *====================================
         */

        default:  /* redundant, as cmd was checked against MAXNR */
        	DBG("in default ioct\r\n");
            return -ENOTTY;
    }
    return ret;
}


static int mstar_fb_set_par(struct fb_info *pinfo)
{
	struct fb_var_screeninfo *var = &pinfo->var;
	U32 bits_per_pixel;
	
	switch (var->bits_per_pixel) {
	case 32:
	case 16:
		pinfo->fix.visual = FB_VISUAL_TRUECOLOR;
		bits_per_pixel = var->bits_per_pixel;
		break;
	case 1:
		pinfo->fix.visual = FB_VISUAL_MONO01;
		bits_per_pixel = 8;
		break;
	default:
		pinfo->fix.visual = FB_VISUAL_PSEUDOCOLOR;
		bits_per_pixel = 8;
		break;
	}

	pinfo->fix.line_length = (var->xres_virtual * bits_per_pixel) / 8;
	pinfo->fix.xpanstep = GOP_WordUnit/(bits_per_pixel >>3);
	/* activate this new configuration */

    return 0;
}
static int mstar_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{	
	U32 line_length,bits_per_pixel;
	DRV_GOPColorType ColorFmt;
	
	/*
	 *  FB_VMODE_CONUPDATE and FB_VMODE_SMOOTH_XPAN are equal!
	 *  as FB_VMODE_SMOOTH_XPAN is only used internally
	 */
	if (var->vmode & FB_VMODE_CONUPDATE) {
		var->vmode |= FB_VMODE_YWRAP;
		var->xoffset = info->var.xoffset;
		var->yoffset = info->var.yoffset;
	}
	/*
	 *  Some very basic checks
	 */
	if (!var->xres)
		var->xres = info->var.xres;
	if (!var->yres)
		var->yres = info->var.xres;
	if (var->xres > var->xres_virtual)
		var->xres_virtual = var->xres;
	if (var->yres > var->yres_virtual)
		var->yres_virtual = var->yres;
	if (var->bits_per_pixel <= 1){
		var->bits_per_pixel = 1;
		bits_per_pixel = 8;
	}else if (var->bits_per_pixel <= 8){
		var->bits_per_pixel = 8;
		bits_per_pixel = 8;
	}else if (var->bits_per_pixel <= 16){
		var->bits_per_pixel = 16;
		bits_per_pixel = 16;
	}else if (var->bits_per_pixel <= 32){
		var->bits_per_pixel = 32;
		bits_per_pixel = 32;
	}else
		return -EINVAL;
	
	if (var->xres_virtual < var->xoffset + var->xres)
		var->xres_virtual = var->xoffset + var->xres;
	if (var->yres_virtual < var->yoffset + var->yres)
		var->yres_virtual = var->yoffset + var->yres;

	/*
	 *  Memory limit
	 */
	line_length =
	    get_line_length(var->xres_virtual,bits_per_pixel);
	if (line_length * var->yres_virtual > VIDEOMEMSIZE)
		return -ENOMEM;

	/*
	 * Now that we checked it we alter var. The reason being is that the video
	 * mode passed in might not work but slight changes to it might make it 
	 * work. This way we let the user know what is acceptable.
	 */
	ColorFmt = get_color_fmt(var);
	if(ColorFmt == E_DRV_GOP_COLOR_INVALID)
		return -EINVAL;

	return 0;
}
static int mstar_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *pinfo)
{
	if(memcmp(&mstar_fb_var, var, sizeof(struct fb_var_screeninfo)))
	{
		//update stretch win
		if( (mstar_fb_var.xres != var->xres) || (mstar_fb_var.yres != var->yres))
			_fb_strewin_update(var);
		//update gwin
		_fb_gwin_update(var,pinfo);
		//enable gwin
		if(bGwinEnable == FALSE)
		{
			_fb_gwin_enable(TRUE);
			bGwinEnable = TRUE;
		}
		//update  mstar_fb_var
		memcpy(&mstar_fb_var, var, sizeof(struct fb_var_screeninfo));
	}
	return 0;
}
static int mstar_fb_setcolreg(unsigned regno, unsigned red, unsigned green,
                              unsigned blue, unsigned transp, struct fb_info *info)
{
	/* grayscale works only partially under directcolor */
	if(info->var.grayscale)
	{
		/* grayscale = 0.30*R + 0.59*G + 0.11*B */
		red = green = blue = (red * 77 + green * 151 + blue * 28) >> 8;
	}

	if(info->fix.visual == FB_VISUAL_TRUECOLOR || info->fix.visual == FB_VISUAL_DIRECTCOLOR)
	{
		U32 v;

		if(regno >= 16)
		return -EINVAL;

		v = (red << info->var.red.offset) | (green << info->var.green.offset) | (blue << info->var.blue.offset) | (transp << info->var.transp.offset);
		((U32*)(info->pseudo_palette))[regno] = v;
	}

	return 0;
}
static int mstar_fb_blank(int blank, struct fb_info *info)
{
	return 0;
}
static void mstar_fb_destroy(struct fb_info *info)
{
	if (info->screen_base)
		iounmap(info->screen_base);
	framebuffer_release(info);
}
static void mstar_fb_fillrect(struct fb_info *p, const struct fb_fillrect *rect)
{
#ifdef CONFIG_FB_VIRTUAL
    sys_fillrect(p, rect);
#endif
}

static void mstar_fb_copyarea(struct fb_info *p, const struct fb_copyarea *area)
{
#ifdef CONFIG_FB_VIRTUAL
    sys_copyarea(p, area);
#endif
}

static void mstar_fb_imageblit(struct fb_info *p, const struct fb_image *image)
{
#ifdef CONFIG_FB_VIRTUAL    
    sys_imageblit(p, image);
#endif
}

#ifndef MODULE
/*
 * The virtual framebuffer driver is only enabled if explicitly
 * requested by passing 'video=vfb:' (or any actual options).
 */
static int __init mstar_fb_setup(char *options)
{
	char *this_opt;
	char tmp_char[16]="\0";
	U32 u32Gop;
	
	if (!options)
		return 1;

	mstar_fb_enable = 1;

	if (!*options)
		return 1;

	while ((this_opt = strsep(&options, ",")) != NULL) {
		if (!*this_opt)
			continue;

		/* Test disable for backwards compatibility */
		if (!strcmp(this_opt, "disable")){
			mstar_fb_enable = 0;
		}else if(!strncmp(this_opt, "phy_addr",8)){
			sscanf(this_opt,"phy_addr=%lx",&_fb_phy_addr);
		}else if(!strncmp(this_opt, "GOP",3)){
			sscanf(this_opt,"GOP=%d",&u32Gop);
			GOPID = (U8)u32Gop;
		}else if(!strncmp(this_opt, "mirror",6)){
			sscanf(this_opt,"mirror=%s",tmp_char);	
			if (!strcmp(this_opt, "true")){
				bMirror=1;
			}else{
				bMirror=0;
			}
		}
			
	}
	return 1;
}
#endif  /*  MODULE  */
/*
 *	Initialisation
 */

static int __init mstar_fb_probe(struct platform_device *dev)
{
    struct fb_info *pinfo = NULL;
    int retval = -ENOMEM;
	
    if (NULL == dev)
    {
        DBG("ERROR: in mstar_fb_prob: dev is NULL pointer \r\n");
        return -ENOTTY;
    }

    mstar_fb_fix.smem_start = 0;

    pinfo = framebuffer_alloc(sizeof(struct fb_info), &dev->dev);
    if (!pinfo)
        return retval;
  
    pinfo->fbops = &mstar_fb_ops;
    pinfo->var = mstar_fb_var;
    pinfo->fix = mstar_fb_fix;
    pinfo->pseudo_palette = pinfo->par;
    pinfo->par = NULL;
    pinfo->flags = FBINFO_FLAG_DEFAULT;

    if(0 != _fb_phy_addr)
    {
        DBG("\33[0;36m _fb_phy_addr = 0x%lx \33[m \n",_fb_phy_addr);
        _fb_buf_init(pinfo, _fb_phy_addr);
		_fb_gwin_init();
		_fb_strewin_update(&mstar_fb_var);
		_fb_gwin_update(&mstar_fb_var,pinfo);
    }

    /* allocate color map */
    /* 256 is following to the vfb.c */
    retval = fb_alloc_cmap(&pinfo->cmap, 256, 0);
    if (retval < 0)
        goto err1;
    
    /* register mstar_fb to fbmem.c */
    retval = register_framebuffer(pinfo);
    if (retval < 0)
        goto err2;
    
    platform_set_drvdata(dev, pinfo);
	DBG("\33[0;36m  fb%d: Mstar frame buffer device \33[m \n",pinfo->node);
    return 0;

err2:
    fb_dealloc_cmap(&pinfo->cmap);
err1:
    framebuffer_release(pinfo);

    return retval;
}



static int mstar_fb_remove(struct platform_device *dev)
{
    struct fb_info *pinfo;
    pinfo = 0;

    if (NULL == dev)
    {
        DBG("ERROR: mstar_fb_remove: dev is NULL pointer \n");
        return -ENOTTY;
    }
	_fb_gwin_enable(FALSE);
	bGwinEnable = FALSE;
    pinfo = platform_get_drvdata(dev);
    if (pinfo)
    {
        unregister_framebuffer(pinfo);
        framebuffer_release(pinfo);
    }
    return 0;
}

static void mstar_fb_platform_release(struct device *device)
{
    if (NULL == device)
    {
        DBG("ERROR: in mstar_fb_platform_release, \
                device is NULL pointer !\r\n");
    }
    else
    {
        DBG("in mstar_fb_platform_release, module unload!\n");
    }
}

/* device .name and driver .name must be the same, then it will call
   probe function */
static struct platform_driver Mstar_fb_driver =
{
    .probe  = mstar_fb_probe,   /* initiailize */
    .remove = mstar_fb_remove,  /* it free(mem),
                                    release framebuffer, free irq etc. */
    .driver =
    {
        .name = "Mstar-fb",
    },
};

static u64 mstar_fb_device_lcd_dmamask = 0xffffffffUL;
/* for 0x08000000 */
static struct platform_device Mstar_fb_device =
{
    .name = "Mstar-fb",
    .id = 0,
    .dev =
    {
        .release = mstar_fb_platform_release,
        .dma_mask = &mstar_fb_device_lcd_dmamask,
        .coherent_dma_mask = 0xffffffffUL
    }
};

static int __init mstar_fb_init(void)
{
	int ret = 0;
#ifndef MODULE
	char *option = NULL;

	if (!fb_get_options("Mstar-fb", &option))
		mstar_fb_setup(option);
#endif
	if (!mstar_fb_enable)
		return -ENXIO;
	
	ret = platform_driver_register(&Mstar_fb_driver);

	if (!ret)
    {   /* register driver sucess */
        /* register device */
        ret = platform_device_register(&Mstar_fb_device);
        if (ret)    /* if register device fail, then unregister the driver.*/
       	{
            platform_driver_unregister(&Mstar_fb_driver);
        }
    }
    return ret;
}


module_init(mstar_fb_init);

#ifdef MODULE
static void __exit mstar_fb_exit(void)
{
    platform_device_unregister(&Mstar_fb_device);
    platform_driver_unregister(&Mstar_fb_driver);
}

module_exit(vfb_exit);

MODULE_LICENSE("GPL");
#endif				/* MODULE */



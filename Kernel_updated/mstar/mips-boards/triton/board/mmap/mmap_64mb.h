////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mmap_32mb.h
/// @brief  Memory mapping for 32MB RAM
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MS_MMAP_64MB_H_
#define _MS_MMAP_64MB_H_

#if CONFIG_MSTAR_TITANIA_BD_S5_CUS03_DEMO_BOARD_1 || CONFIG_MSTAR_TITANIA_BD_S6_CUS03_DEMO_BOARD_ATSC_1
#define USE_CUS03_DEMO_BOARD
#endif

// Memory alignment
#define MemAlignUnit                64UL
#define MemAlign(n, unit)           ((((n)+(unit)-1)/(unit))*(unit))

#define MIU_DRAM_LEN				(0x4000000)

//----------------------------------------------------------------------
// 32M
//----------------------------------------------------------------------
#if 0
#define EMAC_MEM_LEN                0x100000
#define EMAC_MEM_ADR                0x00200000UL

#define NULL_MEM_LEN                0x100000
// Linux kernel space
#define LINUX_MEM_AVAILABLE			(EMAC_MEM_ADR+EMAC_MEM_LEN+NULL_MEM_LEN)
#define LINUX_MEM_BASE_ADR 			(LINUX_MEM_AVAILABLE)
#define LINUX_MEM_GAP_CHK  			(LINUX_MEM_BASE_ADR-LINUX_MEM_AVAILABLE)
#else

// Linux kernel space
#define LINUX_MEM_AVAILABLE			0x00000000UL
#define LINUX_MEM_BASE_ADR 			(LINUX_MEM_AVAILABLE)
#define LINUX_MEM_GAP_CHK  			(LINUX_MEM_BASE_ADR-LINUX_MEM_AVAILABLE)
#endif

//#define LINUX_MEM_LEN				(0xC00000)	// 12MB
#define LINUX_MEM_LEN				(0x8C0000)	// 12MB - 256K

#define BIN_MEM_AVAILABLE      		(LINUX_MEM_BASE_ADR + LINUX_MEM_LEN)
#define BIN_MEM_ADR                 MemAlign(BIN_MEM_AVAILABLE, 4096)
#define BIN_MEM_GAP_CHK        		(BIN_MEM_ADR-BIN_MEM_AVAILABLE)
#define BIN_MEM_LEN    				0x40000

#define SVD_CPU_AVAILABLE          (BIN_MEM_ADR+BIN_MEM_LEN)
#define SVD_CPU_ADR                MemAlign(SVD_CPU_AVAILABLE, 2048)
#define SVD_CPU_ADR_GAP_CHK        (SVD_CPU_ADR-SVD_CPU_AVAILABLE)
#define SVD_CPU_LEN                0x100000                    // 1024KB

// need 512 byte alignment SD
#define MVD_FRAMEBUFFER_AVAILABLE   (SVD_CPU_ADR + SVD_CPU_LEN)
#define MVD_FRAMEBUFFER_ADR         MemAlign(MVD_FRAMEBUFFER_AVAILABLE, 2048)
#define MVD_FRAMEBUFFER_ADR_GAP_CHK (MVD_FRAMEBUFFER_ADR-MVD_FRAMEBUFFER_AVAILABLE)
#define MVD_FRAMEBUFFER_LEN         0x1B00000 //27MB //0xC00000	//0xE00000 // 14MB //0x1DB000 // 1900KB, 3 Buffers=720*576*1.5*3

// need 8 byte alignment
#define MVD_BITSTREAM_AVAILABLE     (MVD_FRAMEBUFFER_ADR + MVD_FRAMEBUFFER_LEN)
#define MVD_BITSTREAM_ADR           MemAlign(MVD_BITSTREAM_AVAILABLE, 8)
#define MVD_BITSTREAM_ADR_GAP_CHK   (MVD_BITSTREAM_ADR-MVD_BITSTREAM_AVAILABLE)
#define MVD_BITSTREAM_LEN           0x400000 //4MB //0x200000	//2MB //0x80000                     // 512KB


// VD_3DCOMB
#define VD_3DCOMB_AVAILABLE			(BIN_MEM_ADR+BIN_MEM_LEN)
#define VD_3DCOMB_BASE_ADR 			MemAlign(VD_3DCOMB_AVAILABLE, 8)
#define VD_3DCOMB_GAP_CHK  			(VD_3DCOMB_BASE_ADR-VD_3DCOMB_AVAILABLE)
#define VD_3DCOMB_LEN      			0  //(0x400000)	// 4MB

#if 0
// VD_3DCOMB
#define VD_3DCOMB_AVAILABLE			(LINUX_MEM_BASE_ADR+LINUX_MEM_LEN)
#define VD_3DCOMB_BASE_ADR 			MemAlign(VD_3DCOMB_AVAILABLE, 8)
#define VD_3DCOMB_GAP_CHK  			(VD_3DCOMB_BASE_ADR-VD_3DCOMB_AVAILABLE)
#define VD_3DCOMB_LEN      			(0x400000)	// 4MB
#endif


// must start at nnn0000 - 512k alignment   --[Dean] be careful allocate this big alignment area
#define MAD_BASE_AVAILABLE          (VD_3DCOMB_BASE_ADR+VD_3DCOMB_LEN)
#define MAD_BASE_BUFFER_ADR         MemAlign(MAD_BASE_AVAILABLE, 0x80000)
#define MAD_BASE_BUFFER_ADR_GAP_CHK (MAD_BASE_BUFFER_ADR-MAD_BASE_AVAILABLE)
#define MAD_BASE_BUFFER_LEN         0 //0x80000		// 352K


#if 0
#define GE_FRAMEBUFFER_AVAILABLE    (MVD_BITSTREAM_ADR + MVD_BITSTREAM_LEN)
#define GE_FRAMEBUFFER_ADR          MemAlign(GE_FRAMEBUFFER_AVAILABLE, 8)
#define GE_FRAMEBUFFER_ADR_GAP_CHK  (GE_FRAMEBUFFER_ADR-GE_FRAMEBUFFER_AVAILABLE)
#define GE_FRAMEBUFFER_LEN          (0x426800+0x80000+0x100000)         // 5786KB

//------------------------------------------------------------------
// GOP  --- the same with GE_FRAMEBUFFER
#define GOP_GWIN_RB_AVAILABLE		GE_FRAMEBUFFER_ADR
#define GOP_GWIN_RB_BASE_ADR 		(GOP_GWIN_RB_AVAILABLE)
#define GOP_GWIN_RB_GAP_CHK			(GOP_GWIN_RB_BASE_ADR-GOP_GWIN_RB_AVAILABLE)
#define GOP_GWIN_RB_BASE_LEN 		GE_FRAMEBUFFER_LEN

// SUBTITLE ???
#define SUBTITLE_AVAILABLE      ((GE_FRAMEBUFFER_ADR + GE_FRAMEBUFFER_LEN) - 0x189000)  //reserved 1610KB
#define SUBTITLE_START_ADR      MemAlign(SUBTITLE_AVAILABLE, 0x10000)
#define SUBTITLE_GAP_CHK        (SUBTITLE_START_ADR-SUBTITLE_AVAILABLE)
#define SUBTITLE_LEN            0x178000                     // 1510KB
//------------------------------------------------------------------
#endif

// need 4 byte alignment
#if 0
#define EMAC_BUF_AVAILABLE     (MVD_BITSTREAM_ADR + MVD_BITSTREAM_LEN)
#define EMAC_BUF_ADR           MemAlign(EMAC_BUF_AVAILABLE, 4)
#define EMAC_BUF_ADR_GAP_CHK   (EMAC_BUF_ADR-EMAC_BUF_AVAILABLE)
#define EMAC_BUF_LEN           0x180000                     // 1.5MB

#define VE_FRAMEBUFFER_AVAILABLE	(EMAC_BUF_ADR + EMAC_BUF_LEN)
#endif

#define VE_FRAMEBUFFER_AVAILABLE	(MVD_BITSTREAM_ADR + MVD_BITSTREAM_LEN)
#define VE_FRAMEBUFFER_ADR			MemAlign(VE_FRAMEBUFFER_AVAILABLE, 8)
#define VE_FRAMEBUFFER_ADR_GAP_CHK	(VE_FRAMEBUFFER_ADR-VE_FRAMEBUFFER_AVAILABLE)
#define VE_FRAMEBUFFER_LEN			0x900000 //0x195000UL	// 720*576*2*2

#define EMAC_MEM_LEN                0x100000
#define EMAC_MEM_ADR                (VE_FRAMEBUFFER_ADR+VE_FRAMEBUFFER_LEN)//0x00200000UL
#define MIU0_END_ADR                (EMAC_MEM_ADR+EMAC_MEM_LEN)//(MPOOL_ADR+MPOOL_LEN)

//======================================================================
// Can not add any buffer here (in between VE_FRAMEBUFFER_ADR and SCALER_DNR_BUF_ADR)
// Because USB/OAD download use the buffer from VE_FRAMEBUFFER_ADR to the end of SCALER_DNR_BUF_ADR
//======================================================================

// For Maximum is 1920x1088x3x2 about 12MB in HDMI and YPbPr
// Let it overwrite EVENTDB and MHEG5 buffer(These buffers is DTV only)
#ifndef USE_CUS03_DEMO_BOARD
#define SCALER_DNR_AVAILABLE		(VE_FRAMEBUFFER_ADR + VE_FRAMEBUFFER_LEN)
#else
#define SCALER_DNR_AVAILABLE		0x4000000	//64M
#endif
#define SCALER_DNR_BUF_ADR			MemAlign(SCALER_DNR_AVAILABLE, 8)
#define SCALER_DNR_GAP_CHK			(SCALER_DNR_BUF_ADR-SCALER_DNR_AVAILABLE)
//#define SCALER_DNR_BUF_LEN		(((736UL*3+0x0F) & ~0x0F) * 581UL *2)  //(720UL*576*3*2)   // SCALER_MEM_SIZE // 2565696
#define SCALER_DNR_BUF_LEN			0 //12582912	// 12MB

#define SCALER_DNR_W_AVAILABLE		(SCALER_DNR_BUF_ADR + SCALER_DNR_BUF_LEN)
#define SCALER_DNR_W_BARRIER_ADR	(SCALER_DNR_W_AVAILABLE)
#define SCALER_DNR_W_GAP_CHK		(SCALER_DNR_W_BARRIER_ADR-SCALER_DNR_W_AVAILABLE)
#define SCALER_DNR_W_BARRIER_LEN	0 //80//16		// DNR submit 2 64-bit data before compare limit

#define SCALER_DNR_BUF_LEN_EXT		((((896UL-736UL)*3+0x0F) & ~0x0F) * 581UL *2) // the output size of VD will be 848 * 581

#ifndef USE_CUS03_DEMO_BOARD
#define RLD_BUF_AVAILABLE			(SCALER_DNR_W_BARRIER_ADR + SCALER_DNR_W_BARRIER_LEN)
#else
#define RLD_BUF_AVAILABLE			(VE_FRAMEBUFFER_ADR + VE_FRAMEBUFFER_LEN)
#endif
#define RLD_BUF_ADR					MemAlign(RLD_BUF_AVAILABLE, 8)
#define RLD_BUF_GAP_CHK				(RLD_BUF_ADR-RLD_BUF_AVAILABLE)
#define RLD_BUF_LEN					0 //905216

#define TTX_BUF_AVAILABLE			(RLD_BUF_ADR + RLD_BUF_LEN)
#define TTX_BUF_ADR					MemAlign(TTX_BUF_AVAILABLE, 8)
#define TTX_BUF_GAP_CHK				(TTX_BUF_ADR-TTX_BUF_AVAILABLE)
#define TTX_BUF_LEN					0 //65536

#if 1	//tony
#define USB_MEM_AVAILABLE      (TTX_BUF_ADR + TTX_BUF_LEN)
#define USB_MEM_ADR                 MemAlign(USB_MEM_AVAILABLE, 8)
#define USB_MEM_GAP_CHK        (USB_MEM_ADR-USB_MEM_AVAILABLE)
#define USB_MEM_LEN    0x7d050

#define USB_DMA_ADDRESS 	USB_MEM_ADR
#define USB_DMA_LENGTH		USB_MEM_LEN
#endif

#define LINUX_2ND_MEM_AVAILABLE		(USB_MEM_ADR + USB_MEM_LEN)
#define LINUX_2ND_MEM_ADR			MemAlign(LINUX_2ND_MEM_AVAILABLE, 4096)
#define LINUX_2ND_MEM_GAP_CHK		(LINUX_2ND_MEM_ADR-LINUX_2ND_MEM_AVAILABLE)
#define LINUX_2ND_MEM_LEN			0x900000

#define MPOOL_AVAILABLE				(LINUX_2ND_MEM_ADR + LINUX_2ND_MEM_LEN)
#define MPOOL_ADR					MemAlign(MPOOL_AVAILABLE, 4096)
#define MPOOL_GAP_CHK				(MPOOL_ADR-MPOOL_AVAILABLE)
#define MPOOL_LEN					(MIU_DRAM_LEN - MPOOL_ADR + 1)

#define END_OF_MEMORY_MAP			(DUMMY_SPACE+DUMMY_SPACE_LEN)
#define END_MEMORY_GAP				(MIU_DRAM_LEN-END_OF_MEMORY_MAP)

#define MIU0_END_ADR                (MPOOL_ADR+MPOOL_LEN)

#if (END_OF_MEMORY_MAP > MIU_DRAM_LEN)
#error "Memory MAP Overflow!!!"
#endif

#endif


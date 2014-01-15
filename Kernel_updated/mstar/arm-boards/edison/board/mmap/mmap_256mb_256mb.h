////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (??MStar Confidential Information??) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mmap_512mb_512mb.h
/// @brief  Memory mapping for 512MB+512MB RAM
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
//#include <linux/undefconf.h>
//#include "../../../../../include/linux/undefconf.h"
#ifndef _MS_MMAP_256MB_256MB_H_
#define _MS_MMAP_256MB_256MB_H_

// Memory alignment
#define MemAlignUnit                64UL
#define MemAlign(n, unit)           ((((n)+(unit)-1)/(unit))*(unit))

//#define MIU_DRAM_LEN				(0x10000000)

#define MIU0_LEN					(0x10000000)
#define MIU1_LEN					(0x10000000)

#define MIU_DRAM_LEN                0x20000000
#define MIU_DRAM_LEN0               0x10000000
#define MIU_DRAM_LEN1               0x10000000
#define MIU_INTERVAL                0x10000000

//----------------------------------------------------------------------
// MIU 0
//----------------------------------------------------------------------

#define COP_CODE_START                  (0x00000000)
#ifdef CONFIG_MSTAR_RESERVE_MEM_FOR_AEON
#define COP_CODE_LEN CONFIG_MSTAR_RESERVE_MEM_FOR_AEON_SIZE
#else
#define COP_CODE_LEN                    (0x0)
#endif

// Linux kernel space
#define LINUX_MEM_AVAILABLE			(COP_CODE_START+COP_CODE_LEN)
#define LINUX_MEM_BASE_ADR 			(LINUX_MEM_AVAILABLE)
#define LINUX_MEM_GAP_CHK  			(LINUX_MEM_BASE_ADR-LINUX_MEM_AVAILABLE
#define LINUX_MEM_LEN				(0x8C0000) //9MB-256K

#if 1
// for MVD and TSP binary
#define BIN_MEM_AVAILABLE      		(LINUX_MEM_BASE_ADR + LINUX_MEM_LEN)
#define BIN_MEM_ADR                 MemAlign(BIN_MEM_AVAILABLE, 4096)
#define BIN_MEM_GAP_CHK        		(BIN_MEM_ADR-BIN_MEM_AVAILABLE)
#define BIN_MEM_LEN    				0x40000

// must start at nnn0000 - 512k alignment   --[Dean] be careful allocate this big alignment area
#define MAD_BASE_AVAILABLE         	(BIN_MEM_ADR+BIN_MEM_LEN)
#define MAD_BASE_BUFFER_ADR        	MemAlign(MAD_BASE_AVAILABLE, 0x80000)
#define MAD_BASE_BUFFER_ADR_GAP_CHK	(MAD_BASE_BUFFER_ADR-MAD_BASE_AVAILABLE)
#define MAD_BASE_BUFFER_LEN        	0x300000	//3M

#else

#define LINUX_MEM_LEN				(0xC00000)	// 12MB

// must start at nnn0000 - 512k alignment   --[Dean] be careful allocate this big alignment area
#define MAD_BASE_AVAILABLE         	(LINUX_MEM_BASE_ADR+LINUX_MEM_LEN)
#define MAD_BASE_BUFFER_ADR        	MemAlign(MAD_BASE_AVAILABLE, 0x80000)
#define MAD_BASE_BUFFER_ADR_GAP_CHK	(MAD_BASE_BUFFER_ADR-MAD_BASE_AVAILABLE)
#define MAD_BASE_BUFFER_LEN        	0x300000	//3M

#endif

//======================================================================
// Can not add any buffer here (in between VE_FRAMEBUFFER_ADR and SCALER_DNR_BUF_ADR)
// Because USB/OAD download use the buffer from VE_FRAMEBUFFER_ADR to the end of SCALER_DNR_BUF_ADR
//======================================================================

// For Maximum is 1920x1088x3x2 about 12MB in HDMI and YPbPr
// Let it overwrite EVENTDB and MHEG5 buffer(These buffers is DTV only)
#define SCALER_DNR_AVAILABLE  		(MAD_BASE_AVAILABLE + MAD_BASE_BUFFER_LEN)
#define SCALER_DNR_BUF_ADR	  		MemAlign(SCALER_DNR_AVAILABLE, 8)
#define SCALER_DNR_GAP_CHK	  		(SCALER_DNR_BUF_ADR-SCALER_DNR_AVAILABLE)
#define SCALER_DNR_BUF_LEN	  		0x1000000-80 //16MB-80byte  //0xC00000  //12582912	// 12MB

#define SCALER_DNR_W_AVAILABLE		(SCALER_DNR_BUF_ADR + SCALER_DNR_BUF_LEN)
#define SCALER_DNR_W_BARRIER_ADR	(SCALER_DNR_W_AVAILABLE)
#define SCALER_DNR_W_GAP_CHK		(SCALER_DNR_W_BARRIER_ADR-SCALER_DNR_W_AVAILABLE)
#define SCALER_DNR_W_BARRIER_LEN	80//16		// DNR submit 2 64-bit data before compare limit
#define SCALER_DNR_BUF_LEN_EXT		((((896UL-736UL)*3+0x0F) & ~0x0F) * 581UL *2) // the output size of VD will be 848 * 581

// no use in ATSC
#define RLD_BUF_AVAILABLE			(SCALER_DNR_W_BARRIER_ADR+SCALER_DNR_W_BARRIER_LEN)
#define RLD_BUF_ADR					MemAlign(RLD_BUF_AVAILABLE, 8)
#define RLD_BUF_GAP_CHK				(RLD_BUF_ADR-RLD_BUF_AVAILABLE)
#define RLD_BUF_LEN					0xDD000//905216

// VD_3DCOMB
#if 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1) 	\
	|| 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
// RLD is only used in EMP Movie, no need in ATSC
#define VD_3DCOMB_AVAILABLE			(SCALER_DNR_W_BARRIER_ADR+SCALER_DNR_W_BARRIER_LEN)
#else
#define VD_3DCOMB_AVAILABLE			(RLD_BUF_ADR + RLD_BUF_LEN)
#endif
#define VD_3DCOMB_BASE_ADR 			MemAlign(VD_3DCOMB_AVAILABLE, 8)
#define VD_3DCOMB_GAP_CHK  			(VD_3DCOMB_BASE_ADR-VD_3DCOMB_AVAILABLE)
#define VD_3DCOMB_LEN      			(0x400000)	//4MB

//#define VE_FRAMEBUFFER_AVAILABLE	(MVD_BITSTREAM_ADR + MVD_BITSTREAM_LEN)
#define VE_FRAMEBUFFER_AVAILABLE  	(VD_3DCOMB_BASE_ADR+VD_3DCOMB_LEN)
#define VE_FRAMEBUFFER_ADR		  	MemAlign(VE_FRAMEBUFFER_AVAILABLE, 8)
#define VE_FRAMEBUFFER_ADR_GAP_CHK	(VE_FRAMEBUFFER_ADR-VE_FRAMEBUFFER_AVAILABLE)
#define VE_FRAMEBUFFER_LEN			0x195000UL	// 720*576*2*2

#define TTX_BUF_AVAILABLE			(VE_FRAMEBUFFER_ADR + VE_FRAMEBUFFER_LEN)
#define TTX_BUF_ADR					MemAlign(TTX_BUF_AVAILABLE, 4096)
#define TTX_BUF_GAP_CHK				(TTX_BUF_ADR-TTX_BUF_AVAILABLE)
#define TTX_BUF_LEN					0x100000

// no use in ATSC
#define MVD_SW_AVAILABLE     		(RLD_BUF_ADR + RLD_BUF_LEN)
#define MVD_SW_ADR           		MemAlign(MVD_SW_AVAILABLE, 8)
#define MVD_SW_ADR_GAP_CHK   		(MVD_SW_ADR-MVD_SW_AVAILABLE)
#define MVD_SW_LEN           		0xB00000//0x900000//0xB00000	//11MB

#if 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1) 	\
	|| 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
// in ATSC, just skip MVD_SW (no need to play movie)
#define MPOOL_AVAILABLE				(TTX_BUF_ADR + TTX_BUF_LEN)
#else
#define MPOOL_AVAILABLE				(MVD_SW_ADR + MVD_SW_LEN)
#endif
#define MPOOL_ADR					MemAlign(MPOOL_AVAILABLE, 4096)
#define MPOOL_GAP_CHK				(MPOOL_ADR-MPOOL_AVAILABLE)
#if 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1) 	\
	|| 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define MPOOL_LEN					(0x4000000 - MPOOL_ADR - 0xB00000-0x400000 /*released from POSD0*/)
#else
#define MPOOL_LEN					(0x1800000UL)//(0x8000000 - MPOOL_ADR - 0x3A00000-0x400000 /*released from POSD0*/)
#endif

#define EMAC_MEM_LEN                0x100000
#define EMAC_MEM_ADR                (MPOOL_ADR+MPOOL_LEN)//0x00200000UL

#define LINUX_MEM2_AVAILABLE		(EMAC_MEM_ADR+EMAC_MEM_LEN)
#define LINUX_MEM2_BASE_ADR			MemAlign(LINUX_MEM2_AVAILABLE,8) //aligned to 8 byte may not be necessary
#define LINUX_MEM2_GAP_CHK			(LINUX_MEM2_BASE_ADR-LINUX_MEM2_AVAILABLE)
#define LINUX_MEM2_LEN			(MIU0_LEN-LINUX_MEM2_BASE_ADR)

#define MIU0_END_ADR                (LINUX_MEM2_BASE_ADR+LINUX_MEM2_BASE_LEN)//(MPOOL_ADR+MPOOL_LEN)

//#define MIU0_END_ADR                (MPOOL_ADR+MPOOL_LEN)
//----------------------------------------------------------------------
// MIU 1
//----------------------------------------------------------------------
#define SVD_CPU_AVAILABLE  			(0x8000000)
#define SVD_CPU_ADR        			MemAlign(SVD_CPU_AVAILABLE, 2048)
#define SVD_CPU_ADR_GAP_CHK			(SVD_CPU_ADR-SVD_CPU_AVAILABLE)
#define SVD_CPU_LEN        			0x100000    //1024KB

// need 512 byte alignment SD
#define MVD_FRAMEBUFFER_AVAILABLE   (SVD_CPU_ADR + SVD_CPU_LEN)
#define MVD_FRAMEBUFFER_ADR         MemAlign(MVD_FRAMEBUFFER_AVAILABLE, 2048)
#define MVD_FRAMEBUFFER_ADR_GAP_CHK (MVD_FRAMEBUFFER_ADR-MVD_FRAMEBUFFER_AVAILABLE)
#if 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1) 	\
	|| 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define MVD_FRAMEBUFFER_LEN			0xE00000	//14MB
#else
#define MVD_FRAMEBUFFER_LEN			0x1B00000	//27MB: Dean
#endif

// need 8 byte alignment
#define MVD_BITSTREAM_AVAILABLE		(MVD_FRAMEBUFFER_ADR + MVD_FRAMEBUFFER_LEN)
#define MVD_BITSTREAM_ADR			MemAlign(MVD_BITSTREAM_AVAILABLE, 8)
#define MVD_BITSTREAM_ADR_GAP_CHK	(MVD_BITSTREAM_ADR-MVD_BITSTREAM_AVAILABLE)
#if 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1) 	\
	|| 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define MVD_BITSTREAM_LEN        	0x400000	//4MB
#else
#define MVD_BITSTREAM_LEN        	0x400000	//4MB // tmp solution for H264
#endif

#define JPD_OUTPUT_AVAILABLE     	(MVD_BITSTREAM_ADR + MVD_BITSTREAM_LEN)
#define JPD_OUTPUT_ADR           	MemAlign(JPD_OUTPUT_AVAILABLE, 8)
#define JPD_OUTPUT_ADR_GAP_CHK   	(JPD_OUTPUT_ADR-JPD_OUTPUT_AVAILABLE)
#if 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1) 	\
	|| 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define JPD_OUTPUT_LEN           	0x400000	//4MB
#else
#define JPD_OUTPUT_LEN           	0x200000    //2MB, temp solution for H264
#endif

#define MIU1_DUMMY_AVAILABLE             (JPD_OUTPUT_ADR + JPD_OUTPUT_LEN)
#define MIU1_DUMMY_ADR           	    MemAlign(MIU1_DUMMY_AVAILABLE, 8)
#define MIU1_DUMMY_ADR_GAP_CHK   	    (MIU1_DUMMY_ADR-MIU1_DUMMY_AVAILABLE)
#if 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1) 	\
	|| 	(defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define MIU1_DUMMY_LEN           	    0x24F0000 // 36MB+0xF0000(960KB)
#else
#define MIU1_DUMMY_LEN           	    0x8 //25MB+0xF0000(960KB)
#endif

#define POSD0_AVAILABLE             (MIU1_DUMMY_ADR + MIU1_DUMMY_LEN)
#define POSD0_ADR           	    MemAlign(POSD0_AVAILABLE, 8)
#define POSD0_ADR_GAP_CHK   	    (POSD0_ADR-POSD0_AVAILABLE)
#define POSD0_LEN           	    0x410000

#define POSD1_AVAILABLE             (POSD0_ADR + POSD0_LEN)
#define POSD1_ADR           	    MemAlign(POSD1_AVAILABLE, 8)
#define POSD1_ADR_GAP_CHK   	    (POSD1_ADR-POSD1_AVAILABLE)
#define POSD1_LEN           	    0 // not used

#if ((POSD1_ADR+POSD1_LEN)>MIU_DRAM_LEN)
#error your-mmap-over-dram-size
#endif

#endif

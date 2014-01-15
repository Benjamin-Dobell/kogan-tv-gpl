////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mmap_64mb_64mb.h
/// @brief  Memory mapping for 64MB+64MB RAM
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
//#include <linux/undefconf.h>
//#include "../../../../../include/linux/undefconf.h"
#ifndef _MS_MMAP_64MB_64MB_H_
#define _MS_MMAP_64MB_64MB_H_

// Memory alignment
#define MemAlignUnit                    64UL
#define MemAlign(n, unit)               ((((n)+(unit)-1)/(unit))*(unit))

#define MIU_DRAM_LEN                    (0x10000000)

#define MIU0_LEN                        (0x4000000)
#define MIU1_LEN                        (0x4000000)

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
#define LINUX_MEM_AVAILABLE             (COP_CODE_START+COP_CODE_LEN)
#define LINUX_MEM_BASE_ADR              (LINUX_MEM_AVAILABLE)
#define LINUX_MEM_GAP_CHK               (LINUX_MEM_BASE_ADR-LINUX_MEM_AVAILABLE)
#define LINUX_MEM_LEN                   (0x1CA0000)

#define EMAC_MEM_AVAILABLE              (LINUX_MEM_AVAILABLE + LINUX_MEM_LEN)
#define EMAC_MEM_ADR                    (EMAC_MEM_AVAILABLE)
#define EMAC_MEM_GAP_CHK                (EMAC_MEM_ADR-EMAC_MEM_AVAILABLE)
#define EMAC_MEM_LEN                    (0x100000UL)    // FIXME: could be 1mb

#define MPOOL_AVAILABLE                 (EMAC_MEM_AVAILABLE + EMAC_MEM_LEN)
#define MPOOL_ADR                       MemAlign(MPOOL_AVAILABLE, 4096)
#define MPOOL_GAP_CHK                   (MPOOL_ADR-MPOOL_AVAILABLE)
#define MPOOL_LEN                       (MIU_DRAM_LEN-MPOOL_ADR)

//    0x1000 + 0x9A0000 + 0x100000 + 0x200000 + 0x1400000 = 0x19A13E8

#if 0

#define SVD_CPU_AVAILABLE               0x2000000
#define SVD_CPU_ADR                     MemAlign(SVD_CPU_AVAILABLE, 0x100000)
#define SVD_CPU_ADR_GAP_CHK             (SVD_CPU_ADR-SVD_CPU_AVAILABLE)
#define SVD_CPU_LEN                     0x100000    //1024KB

// need 512 byte alignment
#define MVD_FRAMEBUFFER_AVAILABLE       (SVD_CPU_ADR + SVD_CPU_LEN)
#define MVD_FRAMEBUFFER_ADR             MemAlign(MVD_FRAMEBUFFER_AVAILABLE, 2048)
#define MVD_FRAMEBUFFER_ADR_GAP_CHK     (MVD_FRAMEBUFFER_ADR-MVD_FRAMEBUFFER_AVAILABLE)
#define MVD_FRAMEBUFFER_LEN             0xBF4000    // 12 MB for mpeg HD

// need 8 byte alignment
#define MVD_BITSTREAM_AVAILABLE         (MVD_FRAMEBUFFER_ADR + MVD_FRAMEBUFFER_LEN)
#define MVD_BITSTREAM_ADR               MemAlign(MVD_BITSTREAM_AVAILABLE, 8)
#define MVD_BITSTREAM_ADR_GAP_CHK       (MVD_BITSTREAM_ADR-MVD_BITSTREAM_AVAILABLE)
#define MVD_BITSTREAM_LEN               0x149000    // 1.28MB for mpeg2 HD

#define XC_MAIN_FB_AVAILABLE            (MVD_BITSTREAM_ADR + MVD_BITSTREAM_LEN)
#define XC_MAIN_FB_ADR                  MemAlign(XC_MAIN_FB_AVAILABLE, 8)
#define XC_MAIN_FB_ADR_GAP_CHK          (XC_MAIN_FB_ADR - XC_MAIN_FB_AVAILABLE)
#define XC_MAIN_FB_LEN                  0xC1BAE0

#define GOP_FB_AVAILABLE                (XC_MAIN_FB_ADR + XC_MAIN_FB_LEN)
#define GOP_FB_ADR                      MemAlign(GOP_FB_AVAILABLE, 8)
#define    GOP_FB_ADR_GAP_CHK           (GOP_FB_ADR - GOP_FB_AVAILABLE)
#define GOP_FB_LEN                      (0x500000)

#define GOP_REGDMA_AVAILABLE            (GOP_FB_ADR + GOP_FB_LEN)
#define GOP_REGDMA_ADR                  MemAlign(GOP_REGDMA_AVAILABLE, 8)
#define GOP_REGDMA_GAP_CHK              (GOP_REGDMA_ADR - GOP_REGDMA_AVAILABLE)
#define GOP_REGDMA_LEN                  (0x10000)


#endif

#define MIU0_END_ADR                    (GOP_REGDMA_ADR+GOP_REGDMA_LEN)

//#define MIU0_END_ADR                (MPOOL_ADR+MPOOL_LEN)
//----------------------------------------------------------------------
// MIU 1
//----------------------------------------------------------------------
#define MIU1_START_ADR                  (0x8000000)

#define VDEC_CPU_AVAILABLE              MIU1_START_ADR
#define VDEC_CPU_ADR                    MemAlign(VDEC_CPU_AVAILABLE, 0x100000)
#define VDEC_CPU_ADR_GAP_CHK            (VDEC_CPU_ADR - VDEC_CPU_AVAILABLE)
#define VDEC_CPU_LEN                    0x100000    // 1024KB

// need 512 byte alignment
#define VDEC_FRAMEBUFFER_AVAILABLE      (VDEC_CPU_ADR + VDEC_CPU_LEN)
#define VDEC_FRAMEBUFFER_ADR            MemAlign(VDEC_FRAMEBUFFER_AVAILABLE, 2048)
#define VDEC_FRAMEBUFFER_ADR_GAP_CHK    (VDEC_FRAMEBUFFER_ADR - VDEC_FRAMEBUFFER_AVAILABLE)
#define VDEC_FRAMEBUFFER_LEN            0x1B00000   // 27MB for H.264 HD

// need 8 byte alignment
#define VDEC_BITSTREAM_AVAILABLE        (VDEC_FRAMEBUFFER_ADR + VDEC_FRAMEBUFFER_LEN)
#define VDEC_BITSTREAM_ADR              MemAlign(VDEC_BITSTREAM_AVAILABLE, 8)
#define VDEC_BITSTREAM_ADR_GAP_CHK      (VDEC_BITSTREAM_ADR - VDEC_BITSTREAM_AVAILABLE)
#define VDEC_BITSTREAM_LEN              0x400000    // 4MB for H.264 HD

//#define LINUX_MEM2_RESERVED

// Linux kernel space
#define LINUX_MEM2_AVAILABLE            (VDEC_BITSTREAM_ADR + VDEC_BITSTREAM_LEN)
#define LINUX_MEM2_BASE_ADR             (LINUX_MEM2_AVAILABLE)
#define LINUX_MEM2_GAP_CHK              (LINUX_MEM2_BASE_ADR - LINUX_MEM2_AVAILABLE)
#define LINUX_MEM2_LEN                  (MIU1_LEN - (LINUX_MEM2_BASE_ADR - MIU1_START_ADR)) // 32MB

#define MIU1_END_ADR                    (LINUX_MEM2_BASE_ADR + LINUX_MEM2_LEN)

#if MIU1_END_ADR - MIU1_START_ADR > MIU1_LEN
    #error mmap_64mb_64mb MIU1 over dram size
#endif

/*
#define SVD_CPU_AVAILABLE              (0x8000000)
#define SVD_CPU_ADR                    MemAlign(SVD_CPU_AVAILABLE, 2048)
#define SVD_CPU_ADR_GAP_CHK            (SVD_CPU_ADR-SVD_CPU_AVAILABLE)
#define SVD_CPU_LEN                    0x100000    //1024KB

// need 512 byte alignment SD
#define MVD_FRAMEBUFFER_AVAILABLE   (SVD_CPU_ADR + SVD_CPU_LEN)
#define MVD_FRAMEBUFFER_ADR         MemAlign(MVD_FRAMEBUFFER_AVAILABLE, 2048)
#define MVD_FRAMEBUFFER_ADR_GAP_CHK (MVD_FRAMEBUFFER_ADR-MVD_FRAMEBUFFER_AVAILABLE)
#if     (defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)     \
    ||     (defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define MVD_FRAMEBUFFER_LEN            0xE00000    //14MB
#else
#define MVD_FRAMEBUFFER_LEN            0x1B00000    //27MB: Dean
#endif

// need 8 byte alignment
#define MVD_BITSTREAM_AVAILABLE        (MVD_FRAMEBUFFER_ADR + MVD_FRAMEBUFFER_LEN)
#define MVD_BITSTREAM_ADR            MemAlign(MVD_BITSTREAM_AVAILABLE, 8)
#define MVD_BITSTREAM_ADR_GAP_CHK    (MVD_BITSTREAM_ADR-MVD_BITSTREAM_AVAILABLE)
#if     (defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)     \
    ||     (defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define MVD_BITSTREAM_LEN            0x400000    //4MB
#else
#define MVD_BITSTREAM_LEN            0x400000    //4MB // tmp solution for H264
#endif

#define JPD_OUTPUT_AVAILABLE         (MVD_BITSTREAM_ADR + MVD_BITSTREAM_LEN)
#define JPD_OUTPUT_ADR               MemAlign(JPD_OUTPUT_AVAILABLE, 8)
#define JPD_OUTPUT_ADR_GAP_CHK       (JPD_OUTPUT_ADR-JPD_OUTPUT_AVAILABLE)
#if     (defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)     \
    ||     (defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define JPD_OUTPUT_LEN               0x400000    //4MB
#else
#define JPD_OUTPUT_LEN               0x200000    //2MB, temp solution for H264
#endif

#define MIU1_DUMMY_AVAILABLE             (JPD_OUTPUT_ADR + JPD_OUTPUT_LEN)
#define MIU1_DUMMY_ADR                   MemAlign(MIU1_DUMMY_AVAILABLE, 8)
#define MIU1_DUMMY_ADR_GAP_CHK           (MIU1_DUMMY_ADR-MIU1_DUMMY_AVAILABLE)
#if     (defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MBRD_BOARD_ATSC_1)     \
    ||     (defined (CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)&&CONFIG_MSTAR_TITANIA_BD_T2_CUS03_MIBRD_BOARD_ATSC_1)
#define MIU1_DUMMY_LEN                   0x24F0000 // 36MB+0xF0000(960KB)
#else
#define MIU1_DUMMY_LEN                   0x19F0000 //25MB+0xF0000(960KB)
#endif

#define POSD0_AVAILABLE             (MIU1_DUMMY_ADR + MIU1_DUMMY_LEN)
#define POSD0_ADR                   MemAlign(POSD0_AVAILABLE, 8)
#define POSD0_ADR_GAP_CHK           (POSD0_ADR-POSD0_AVAILABLE)
#define POSD0_LEN                   0x410000

#define POSD1_AVAILABLE             (POSD0_ADR + POSD0_LEN)
#define POSD1_ADR                   MemAlign(POSD1_AVAILABLE, 8)
#define POSD1_ADR_GAP_CHK           (POSD1_ADR-POSD1_AVAILABLE)
#define POSD1_LEN                   0 // not used

#if ((POSD1_ADR+POSD1_LEN)>MIU_DRAM_LEN)
#error your-mmap-over-dram-size
#endif
*/
#endif

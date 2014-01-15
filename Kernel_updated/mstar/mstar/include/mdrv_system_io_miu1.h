////////////////////////////////////////////////////////////////////////////////
////
//// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
//// All rights reserved.
////
//// Unless otherwise stipulated in writing, any and all information contained
//// herein regardless in any format shall remain the sole proprietary of
//// MStar Semiconductor Inc. and be kept in strict confidence
//// (¡§MStar Confidential Information¡¨) by the recipient.
//// Any unauthorized act including without limitation unauthorized disclosure,
//// copying, use, reproduction, sale, distribution, modification, disassembling,
//// reverse engineering and compiling of the contents of MStar Confidential
//// Information is unlawful and strictly prohibited. MStar hereby reserves the
//// rights to any and all damages, losses, costs and expenses resulting therefrom.
////
//////////////////////////////////////////////////////////////////////////////////
//
#ifndef __DRV_SYSTEM_IO_MIU1_H__
#define __DRV_SYSTEM_IO_MIU1_H__


#if defined(CONFIG_MSTAR_TITANIA_MMAP_64MB)
#define  MIU1_OFFSET      0
#elif defined(CONFIG_MSTAR_TITANIA_MMAP_64MB_64MB)
#define  MIU1_OFFSET      0
#elif defined(CONFIG_MSTAR_MMAP_128MB_128MB_DEFAULT)
#define  MIU1_OFFSET      0
#elif defined(CONFIG_MSTAR_OBERON_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0
#elif defined(CONFIG_MSTAR_EUCLID_FPGA_MMAP_128MB)
#define  MIU1_OFFSET      0
#elif defined(CONFIG_MSTAR_EUCLID_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0
#elif defined(CONFIG_MSTAR_TITANIA3_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x08000000
#elif defined(CONFIG_MSTAR_TITANIA10_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x08000000
#elif defined(CONFIG_MSTAR_TITANIA4_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x08000000
#elif defined(CONFIG_MSTAR_TITANIA4_MMAP_128MB)
#define  MIU1_OFFSET      0x08000000
#elif defined(CONFIG_MSTAR_TITANIA8_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA8_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA8_MMAP_512MB_512MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA8_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA9_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA9_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA9_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA11_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA11_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA11_MMAP_512MB_512MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA11_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA12_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA12_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA12_MMAP_512MB_512MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA12_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA13_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA13_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_TITANIA13_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER1_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER1_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER1_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER2_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER2_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER2_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER5_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER5_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER5_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_URANUS4_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x08000000
#elif defined(CONFIG_MSTAR_JANUS2_MMAP_64MB_64MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_JANUS2_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_JANUS2_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_JANUS2_MMAP_512MB_512MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_KRONUS_MMAP_128MB)
#define  MIU1_OFFSET      0x8000000
#elif defined(CONFIG_MSTAR_KRONUS_MMAP_64MB)
#define  MIU1_OFFSET      0x8000000
#elif defined(CONFIG_MSTAR_KAISERIN_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER6_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER6_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER6_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER7_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER7_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER7_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMETHYST_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMETHYST_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_AMBER3_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_AMBER3_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_AMBER3_MMAP_512MB_512MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_AMBER3_MMAP_128MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_AGATE_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_AGATE_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_AGATE_MMAP_512MB_512MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_AGATE_MMAP_128MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_EAGLE_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_EAGLE_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_EAGLE_MMAP_512MB_512MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_EAGLE_MMAP_128MB)
#define  MIU1_OFFSET      0x00000000
#elif defined(CONFIG_MSTAR_EMERALD_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_EMERALD_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_EMERALD_MMAP_128MB)
#define  MIU1_OFFSET      0x20000000
#elif defined(CONFIG_MSTAR_ARM_MMAP_128MB_128MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_ARM_MMAP_256MB_256MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_ARM_MMAP_512MB_512MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_ARM_MMAP_128MB)
#define  MIU1_OFFSET      0x60000000
#elif defined(CONFIG_MSTAR_EIFFEL_MMAP_GENERIC)
#define  MIU1_OFFSET      0x60000000
#endif

#endif // __DRV_SYSTEM_IO_MIU1_H__


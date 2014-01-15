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
/// file    mmap_128mb_128mb_default.h
/// @brief  Memory mapping for 128MB+128MB RAM with project Obama
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
//#include <linux/undefconf.h>
//#include "../../../../../include/linux/undefconf.h"
#ifndef _MS_MMAP_128MB_128MB_DEFAULT_H_
#define _MS_MMAP_128MB_128MB_DEFAULT_H_

// Memory alignment
#define MemAlignUnit                    64UL
#define MemAlign(n, unit)               ((((n)+(unit)-1)/(unit))*(unit))

#define MIU_DRAM_LEN				    (0x10000000)

#define MIU0_LEN					    (0x8000000)
#define MIU1_LEN					    (0x8000000)


#define COP_CODE_START				    (0x00000000)
#ifdef CONFIG_MSTAR_RESERVE_MEM_FOR_AEON
#define COP_CODE_LEN CONFIG_MSTAR_RESERVE_MEM_FOR_AEON_SIZE
#else
#define COP_CODE_LEN				    (0x0)
#endif

// Linux kernel space
#define LINUX_MEM_AVAILABLE		        (COP_CODE_START+COP_CODE_LEN)
#define LINUX_MEM_BASE_ADR 		        (LINUX_MEM_AVAILABLE)
#define LINUX_MEM_GAP_CHK  		        (LINUX_MEM_BASE_ADR-LINUX_MEM_AVAILABLE)
#define LINUX_MEM_LEN				    (0x27C0000UL) //8MB-256K


#define EMAC_MEM_AVAILABLE			    (LINUX_MEM_AVAILABLE + LINUX_MEM_LEN)
#define EMAC_MEM_ADR				    (EMAC_MEM_AVAILABLE)
#define EMAC_MEM_GAP_CHK				(EMAC_MEM_BASE_ADR-EMAC_MEM_AVAILABLE)
#define EMAC_MEM_LEN					(0x100000UL)


#define MPOOL_AVAILABLE				    (EMAC_MEM_AVAILABLE + EMAC_MEM_LEN)
#define MPOOL_ADR					    MemAlign(MPOOL_AVAILABLE, 4096)
#define MPOOL_GAP_CHK				    (MPOOL_ADR-MPOOL_AVAILABLE)
#define MPOOL_LEN					    (MIU_DRAM_LEN-MPOOL_ADR)

#endif



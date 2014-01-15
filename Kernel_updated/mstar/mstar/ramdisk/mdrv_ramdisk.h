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
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_mpool.h
/// @brief  MALLOC Driver Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_MSRAMDISK_H_
#define _DRV_MSRAMDISK_H_


//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define MSTAR_BLKDEV_MAXPARTITIONS      1
#define page_size  4096                                       //page size 4K
#define MSTAR_RAMDISK             "msramdisk"
#define KERNEL_SECTOR_SIZE	512

/* Use 'M' as magic number */
#define MSRAMDISK_IOC_MAGIC  'M'
#define MSRAMDISK_IOC_MAXNR   1
#define HDIO_SET_RANGE	_IOW(MSRAMDISK_IOC_MAGIC,  1, struct brd_range)	/* change ramdisk range include: start address, and size*/


//support up to 3 ranges
struct brd_device 
{	
	int		brd_refcnt;
	//loff_t		brd_offset;
	//loff_t		brd_sizelimit;
	unsigned	brd_blocksize;        /* block size in bytes */
	int chunknum;	
	unsigned long bnum[3];                               /* block disk size in sectors */ 
       u8 * data[3];                               /* buffer address */
	unsigned long size;                      /* total size in bytes */

	struct request_queue	*brd_queue;
	struct gendisk		*brd_disk;
	
	spinlock_t		brd_lock;	
};

struct brd_range
{  
    int leng;                           //array length in using 
    unsigned long base[3];
    unsigned long length[3]; 
};


#endif // _DRV_MPOOL_H_


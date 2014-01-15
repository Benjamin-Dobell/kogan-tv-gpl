/////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2010 MStar Semiconductor, Inc.
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
/// file    mdrv_ramdisk.c
/// @brief  expose reserved memory to app
/// @author MStar Semiconductor Inc.
/// @notice: now mstar ramdisk only support up to 3 not consecutive memory chunks
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/autoconf.h>
#include <linux/undefconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <linux/device.h>
#include <linux/bio.h>
#include <asm/io.h>
#include <asm/types.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <asm/mips-boards/prom.h>

#include "mdrv_ramdisk.h"
#include "mst_devid.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"


static struct brd_range mstar_ramdisk_range;
static int msrd_major = 0;
static struct brd_device * brd = NULL;

// Define  Device
static unsigned long msrd_base[3];
static unsigned long MsrdStart[3], MsrdEnd[3], MsrdLeng[3], MsrdTotalLeng;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

static int MsRd_Init(struct  brd_range * brdrange)
{   
    int index;
    for(index = 0; index < brdrange->leng; ++index)
    {
        MsrdStart[index] = (brdrange->base[index] + page_size - 1) & (~(page_size  - 1));
        MsrdEnd[index] = (brdrange->base[index] + brdrange->length[index]) & (~(page_size  - 1));
        MsrdLeng[index] =  MsrdEnd[index] - MsrdStart[index];
	
        /* Get our needed resources. */
         if (! request_mem_region(MsrdStart[index], MsrdLeng[index], "mstarramdisk")) 
         {
	       printk(KERN_ERR "mstar ramdisk mem: %d, can't get I/O mem address 0x%lx\n", index, MsrdStart[index]);
			return -ENODEV;
          }

          /*ioremap it */    
         //msrd_base[index] = (unsigned long) ioremap_cachable(MsrdStart[index], MsrdLeng[index]); 
         msrd_base[index] = (unsigned long) ioremap(MsrdStart[index], MsrdLeng[index]); 
         //printk(KERN_INFO "index %d, start %lu, length %lu\n", index, msrd_base[index], MsrdLeng[index]);
    }
    return 0;
}

static int msbrd_open(struct block_device *bdev, fmode_t mode)
{	
       struct brd_device * dev = (struct brd_device *) bdev->bd_disk->private_data;       
	spin_lock(&dev->brd_lock);
	dev->brd_refcnt ++;
	spin_unlock(&dev->brd_lock);
	
	return 0;
}

static int msbrd_release(struct gendisk *disk, fmode_t mode)
{
       struct brd_device * dev = (struct brd_device *) disk->private_data;   
	spin_lock(&dev->brd_lock);
	dev->brd_refcnt --;	
	spin_unlock(&dev->brd_lock);	

	return 0;
}

/*
 * The ioctl() implementation
 */
static int msbrd_ioctl (struct block_device *bdev, fmode_t mode, unsigned int cmd, unsigned long arg)
{     
	int ret = 0, err = 0, index;
	struct brd_range temp_ramdisk_range;	
	struct brd_device * dev = (struct brd_device *) bdev->bd_disk->private_data; 
	
	memset((void *)&temp_ramdisk_range, 0x00, sizeof(temp_ramdisk_range));
       if (_IOC_TYPE(cmd) != MSRAMDISK_IOC_MAGIC) 
	   	return -ENOTTY;
	if (_IOC_NR(cmd) > MSRAMDISK_IOC_MAXNR) 
		return -ENOTTY;

       if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) 
		return -EFAULT;
	
	switch(cmd) 
	{		
	    case HDIO_SET_RANGE:				
		copy_from_user (&temp_ramdisk_range, (void __user *) arg, sizeof(temp_ramdisk_range));		
		if(temp_ramdisk_range.leng > 3 || temp_ramdisk_range.leng < 1)  //support up to 3 not consecutive memory chunks			
			return -EINVAL; 
		
	   	spin_lock(&dev->brd_lock);		
		//free original resource
		for(index = 0; index<mstar_ramdisk_range.leng; ++index)
		{   
		       //printk(KERN_ERR "index %d, start %lu, length %lu\n", index, msrd_base[index], MsrdLeng[index]);
			iounmap((void __iomem *)msrd_base[index]);
			release_mem_region(MsrdStart[index], MsrdLeng[index]);	
		}
		
	       //get new resource
	       mstar_ramdisk_range = temp_ramdisk_range;
	       if(MsRd_Init(&mstar_ramdisk_range)<0)
              {  
                 ret = -ENODEV;    	          
              }

		dev->chunknum = mstar_ramdisk_range.leng;
              MsrdTotalLeng = 0;
              for(index=0; index<mstar_ramdisk_range.leng; ++index)
		{
                  MsrdTotalLeng += MsrdLeng[index];
                  dev->bnum[index] = MsrdLeng[index]/KERNEL_SECTOR_SIZE;
                  dev->data[index] = (U8 *)msrd_base[index];	
                  memset((void *)msrd_base[index], 0x00, MsrdLeng[index]);     //initilize the range with zero
	       }	

	       set_capacity(dev->brd_disk, MsrdTotalLeng/KERNEL_SECTOR_SIZE);
	       dev->size = MsrdTotalLeng/KERNEL_SECTOR_SIZE;
		
	   	spin_unlock(&dev->brd_lock);	
		break;
		
	    default:
		 ret = -ENOTTY;
	}
	return ret;
}



static struct block_device_operations brd_fops = 
{	
	.owner            =	THIS_MODULE,
	.open 	        =     msbrd_open,
	.release 	        =     msbrd_release,
	.locked_ioctl    =     msbrd_ioctl	
};

/*
 * Handle an I/O request.
 * offset unit is sector
 * nsect unit is sector
 */
static void brd_transfer(struct brd_device *brd, unsigned long offset, unsigned long nsect, char *buffer, int write)
{		
	int index;	
	 
	if ((offset + nsect ) > brd->size)
	{
		printk (KERN_NOTICE "Beyond-end write (%ld %ld) in sector\n", offset, nsect);
		return;
	}     
	
	for(index=0; index<brd->chunknum; ++index)
	{    
              if(offset >= brd->bnum[index])     //dest page not in this sector
			offset -= brd->bnum[index];
	       else
		   	break;                                   //find the location of dest page
	}	

	//printk(KERN_ERR "******offset %d nsect %d\n", offset, nsect);
	if (write)
	 	memcpy(brd->data[index] + offset *brd->brd_blocksize, buffer, nsect*brd->brd_blocksize);
	else
	       memcpy(buffer, brd->data[index] + offset *brd->brd_blocksize, nsect*brd->brd_blocksize);

	 
}

static int brd_make_request(struct request_queue *q, struct bio *bio)
{
	struct brd_device * brd = q->queuedata;		
	int i, status;
	struct bio_vec *bvec;
	sector_t sector = bio->bi_sector;

       //spin_lock(&brd->brd_lock);	
	/* Do each segment independently. */
	bio_for_each_segment(bvec, bio, i) 
	{
		char *buffer = __bio_kmap_atomic(bio, i, KM_USER0);
		brd_transfer(brd, sector, bio_cur_sectors(bio), buffer, bio_data_dir(bio) == WRITE);
		sector += bio_cur_sectors(bio);
		__bio_kunmap_atomic(bio, KM_USER0);
	}
	status = 0; /* Always "succeed" */
	bio_endio(bio, status);
       //spin_unlock(&brd->brd_lock);
	return 0;
}



	
static int __init mod_msramdisk_init(void)
{    
       int ret = 0, index;	
	struct gendisk *disk;

       //mstar_ramdisk_range.leng must <= 3
	#if 1
	mstar_ramdisk_range.leng = 1;
	mstar_ramdisk_range.base[0] = 0x0003A43000;  //Alignment 
        mstar_ramdisk_range.length[0] = 0x400000;      

        /* 
	mstar_ramdisk_range.leng = 1;
	mstar_ramdisk_range.base[0] = 0x0003743000;  //Alignment 
        mstar_ramdisk_range.length[0] = 0x400000;      
       
	mstar_ramdisk_range.base[1] = 0x0003c43000;  //Alignment 
        mstar_ramdisk_range.length[1] = 0x400000;      //8M
      	
	mstar_ramdisk_range.base[2] = 0x0004143000;  //Alignment 
        mstar_ramdisk_range.length[2] = 0x400000;      //8M
        */
	#endif
	
       //get the co buffer from kernel not available range       
       if(MsRd_Init(&mstar_ramdisk_range)<0)
       {  
           ret = -ENODEV;
    	    goto out_cobuffer_init; 
       }		
           
       //Get registered.
       msrd_major = register_blkdev(msrd_major, MSTAR_RAMDISK);
       if (msrd_major <= 0) 
       {
		printk(KERN_WARNING "mstar ramdisk: unable to get major number\n");
		ret =  -EBUSY;
		goto out_unregister; 
       }
	
       //Allocate the device, and initialize it.
	brd = kzalloc(sizeof(*brd), GFP_KERNEL);
	if (!brd)
		goto out;	
	spin_lock_init(&brd->brd_lock);
	
	//initilize the mstar disk 
       brd->brd_blocksize = KERNEL_SECTOR_SIZE;
	
	brd->chunknum = mstar_ramdisk_range.leng;
	MsrdTotalLeng = 0;
	for(index=0; index<mstar_ramdisk_range.leng; ++index)
	{
		MsrdTotalLeng += MsrdLeng[index];
		brd->bnum[index] = MsrdLeng[index]/KERNEL_SECTOR_SIZE;
		brd->data[index] = (U8 *)msrd_base[index];	
		memset((void *)msrd_base[index], 0x00, MsrdLeng[index]);     //initilize the range with zero
	}	

	brd->brd_queue = blk_alloc_queue(GFP_KERNEL);
	if (!brd->brd_queue)
		goto out_free_dev;
	blk_queue_make_request(brd->brd_queue, brd_make_request);
	blk_queue_max_sectors(brd->brd_queue, 1024);
	blk_queue_bounce_limit(brd->brd_queue, BLK_BOUNCE_ANY);

       //blk_queue_hardsect_size(brd->brd_queue, brd->brd_blocksize);
	brd->brd_queue->queuedata = brd;
   
	disk = brd->brd_disk = alloc_disk(MSTAR_BLKDEV_MAXPARTITIONS);
	if (!disk)
		goto out_free_queue;
	disk->major		= msrd_major;
	disk->first_minor	= 0;
	disk->fops		= &brd_fops;
	disk->private_data	= brd;
	disk->queue		= brd->brd_queue;
	//disk->flags |= GENHD_FL_SUPPRESS_PARTITION_INFO;
	sprintf(disk->disk_name, "msram%d", 0);
	set_capacity(disk, MsrdTotalLeng/KERNEL_SECTOR_SIZE);
	brd->size = MsrdTotalLeng/KERNEL_SECTOR_SIZE;
              
       add_disk(brd->brd_disk);	   
	
	printk(KERN_INFO "Mstar ramdisk initilization success, msramdisk major = %d\n", msrd_major);
	return ret;

out_free_queue:
	blk_cleanup_queue(brd->brd_queue);
out_free_dev:
	kfree(brd);
out:	
	unregister_blkdev(msrd_major, MSTAR_RAMDISK);
out_unregister:	
	for(index = 0; index<mstar_ramdisk_range.leng; ++index)
	{
		iounmap((void __iomem *)msrd_base[index]);
		release_mem_region(MsrdStart[index], MsrdLeng[index]);	
	}
out_cobuffer_init:    
	
       return ret;
}


static void __exit mod_msramdisk_exit(void)
{    
       int index;
       spin_lock(&brd->brd_lock);
	if(brd->brd_refcnt != 0)
	{
		spin_unlock(&brd->brd_lock);
		printk(KERN_ERR "msramdisk is in using\n!");
		return;
	}
	spin_unlock(&brd->brd_lock);
	
       if(brd->brd_disk)
       	del_gendisk(brd->brd_disk);
	if(brd->brd_queue)
       	blk_cleanup_queue(brd->brd_queue);
	if(brd)
		kfree(brd);	
	unregister_blkdev(msrd_major, MSTAR_RAMDISK);	
	
	for(index = 0; index<mstar_ramdisk_range.leng; ++index)
	{
		iounmap((void __iomem *)msrd_base[index]);
		release_mem_region(MsrdStart[index], MsrdLeng[index]);	
	}
}


module_init(mod_msramdisk_init);
module_exit(mod_msramdisk_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("mstar ramdisk driver");
MODULE_LICENSE("MSTAR");

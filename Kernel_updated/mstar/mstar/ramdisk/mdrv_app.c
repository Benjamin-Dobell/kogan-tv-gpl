////////////////////////////////////////////////////////////////////////////////
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
/// @brief  demo for changing mstar ramdisk memory space config
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MSRAMDISK_IOC_MAGIC  'M'
#define HDIO_SET_RANGE	_IOW(MSRAMDISK_IOC_MAGIC,  1, struct brd_range)	/* change ramdisk range include: start address, and size*/

//notice: now mstar ramdisk only support up to 3 not consecutive memory chunks
struct brd_range
{  
    int leng;                           //array length in using 
    unsigned long base[3];
    unsigned long length[3]; 
};

//demo for changing mstar ramdisk memory space config
//note: before executing this, you should umount point /dev/msram0 mounted

int main(void)
{
      int fd;
      struct brd_range brdset;
	  
      memset((void *)&brdset, 0x00, sizeof(brdset));
      brdset.leng = 3;
	   
      brdset.base[0] =  0x0003743000;  //Alignment 
      brdset.length[0] = 0x400000;      //30M;
   
      brdset.base[1] = 0x0003c43000;  //Alignment 
      brdset.length[1] = 0x400000;      //10M;
      brdset.base[2] = 0x0004143000;  //Alignment 
      brdset.length[2] = 0x400000;      //10M;
         /**/
      fd=open("/dev/msram0", O_RDWR );
      if(fd < 0)
      	{
	  	printf("open /dev/msram0 failed\n");
		return 0;
      	}

	  
      if(ioctl(fd, HDIO_SET_RANGE, &brdset)<0)
	  	printf("set mstar ramdisk config error!\n");     
   
    
      close(fd);
   
}








////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (��MStar Confidential Information��) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#include <common.h>
#include <command.h>
#include <malloc.h>   /* for free() prototype */
#include <stdio.h>
#include <kk_vfs.h>

#define TMP_BUFSIZE     4096
#define KK_VFS_DBG(x)             x

static int mmc_partition = 0;
int kk_vfs_init(void)
{
	int ret = 0;
	return ret;
}
int kk_vfs_mount(char *partition)
{
	int ret = 0;
	if(strcmp(partition,"tvcustomer")==0)
	{
		mmc_partition = 8;
		KK_VFS_DBG(printf("\n mount tvcustomer"));
	}
	else if(strcmp(partition,"customercfg")==0)
	{
		mmc_partition = 10;
		KK_VFS_DBG(printf("\n mount customercfg"));
	}
	else
		mmc_partition = 8;
	return ret;
}

unsigned int kk_vfsfile_getsize(char *filedir)
{
	unsigned int  filesize = 0;
	char* cmd = (char *)malloc(TMP_BUFSIZE);
	if(strncmp(filedir, "/customercfg/",13)==0)
	{
		kk_vfs_mount("customercfg");
		snprintf(cmd, TMP_BUFSIZE, "ext4filesize mmc 0:0xA %s %x",filedir,(unsigned int)&filesize);
	}
	else
	{
		kk_vfs_mount("tvcustomer");
		snprintf(cmd, TMP_BUFSIZE, "ext4filesize mmc 0:%u %s %x",mmc_partition,filedir,(unsigned int)&filesize);
	}
	KK_VFS_DBG(printf("\ngetsize==%s",cmd));
	run_command(cmd, 0);
	free(cmd);
	return filesize;
}
int kk_vfsfile_load(char *filedir,unsigned int  addr)	
{
	int ret = -1;
	char* cmd = (char *)malloc(TMP_BUFSIZE);

	if(strncmp(filedir, "/customercfg/",13)==0)
	{
		kk_vfs_mount("customercfg");
		snprintf(cmd, TMP_BUFSIZE, "ext4load mmc 0:0xA 0x%x %s",addr,filedir);
	}
	else
	{
		kk_vfs_mount("tvcustomer");
		snprintf(cmd, TMP_BUFSIZE, "ext4load mmc 0:%u 0x%x %s",mmc_partition,addr,filedir);
	}
	KK_VFS_DBG(printf("\nvfsfile_load==%s",cmd));
	ret = run_command(cmd, 0);
	free(cmd);
	return ret;
}
int kk_vfs_deinit(void)
{
	int ret = -1;
	KK_VFS_DBG(printf("not implement yet %s %s %u\n",__FILE__,__FUNCTION__,__LINE__));
	return ret;
}




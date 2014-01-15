////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 Konka Semiconductor, Inc.
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
#include <upgrade_ui.h>
#include <malloc.h>
#include <stdio.h> 
#include <kk_board.h> 

static E_UPGRADE_UI_INIT_STAT sUpgradeUiInitStat = E_UPGRADE_UI_UNINIT;

static int init_upgrade_ui(void)
{
	int ret = -1;
	int upgradetb;
	char * p_str = getenv ("ui_upgrade_table");
	upgradetb = (int)simple_strtol(p_str, NULL, 10);
	printf("upgradetb====== %d\n", upgradetb);
	if(upgradetb == 0)
	{
		ret = kk_write_ui_dbtable();
	}
	else
	{
		ret = kk_read_ui_dbtable();
	}
	return ret;
}

static int clean_display(void)
{
	int ret = -1;
	if(run_command("kk_boot_clean_osdwin",0))
	{
		ret = 0;
	}
	return ret;
}

static int draw_upgrade_init_usb_fail(void)
{
	int ret = -1;
	char *cmd= NULL;
	const int cmd_size = 100;  
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	if(!clean_display())
	{
		snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_USB_INIT_FAIL_ID][1], ui_resource_dbtable[JPG_USB_INIT_FAIL_ID][0], ui_resource_array[JPG_USB_INIT_FAIL_ID][1]);
		if(run_command(cmd,0))
		{
			ret = 0;
		}
	}
	if(cmd != NULL)
		free(cmd);
	return ret;	
}


static int draw_upgrade_search_storage(void)
{
	int ret = -1;
	char *cmd= NULL;
	const int cmd_size = 100;  
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	if(!clean_display())
	{
		snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_FIND_ID][1], ui_resource_dbtable[JPG_FIND_ID][0], ui_resource_array[JPG_FIND_ID][1]);
		if(run_command(cmd,0))
		{
			ret = 0;
		}
	}
	if(cmd != NULL)
		free(cmd);
	return ret;	
}

static int draw_upgrade_search_storage_fail(void)
{
	int ret = -1;
	char *cmd= NULL;
	const int cmd_size = 100;  
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	if(!clean_display())
	{
		snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_NO_USB_STORAGE_ID][1], ui_resource_dbtable[JPG_NO_USB_STORAGE_ID][0], ui_resource_array[JPG_NO_USB_STORAGE_ID][1]);
		if(run_command(cmd,0))
		{
			ret = 0;
		}
	}
	if(cmd != NULL)
		free(cmd);
	return ret;	
}

static int draw_upgrade_search_upgrade_file(void)
{
	return 0;	
}

static int draw_upgrade_search_upgrade_file_fail(void)
{
	return 0;	
}

static int draw_upgrade_load_upgrade_file(void)
{
	return 0;	
}

static int draw_upgrade_load_upgrade_file_fail(void)
{
	return 0;	
}

static int draw_upgrade_progress(unsigned short percent)
{
	static unsigned short s_lastPercent = (unsigned short)-1;
	int ret = -1;
	char *cmd= NULL;
	const int cmd_size = 100;

	if(s_lastPercent/20 == percent/20)
	{
		return 0;
	} else
	{
		s_lastPercent = percent;
	}

	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	if(percent >= 0 && percent < 20)
	{
		if(!clean_display())
		{
			snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_P0_ID][1], ui_resource_dbtable[JPG_P0_ID][0], ui_resource_array[JPG_P0_ID][1]);
			if(run_command(cmd,0))
			{
				ret = 0;
			}
		}
	}
	else if(percent >= 20 && percent < 40)
	{
		if(!clean_display())
		{
			snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_P20_ID][1], ui_resource_dbtable[JPG_P20_ID][0], ui_resource_array[JPG_P20_ID][1]);
			if(run_command(cmd,0))
			{
				ret = 0;
			}
		}
	}
	else if(percent >= 40 && percent < 60)
	{
		if(!clean_display())
		{
			snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_P40_ID][1], ui_resource_dbtable[JPG_P40_ID][0], ui_resource_array[JPG_P40_ID][1]);
			if(run_command(cmd,0))
			{
				ret = 0;
			}
		}
	}
	else if(percent >= 60 && percent < 80)
	{
		if(!clean_display())
		{
			snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_P60_ID][1], ui_resource_dbtable[JPG_P60_ID][0], ui_resource_array[JPG_P60_ID][1]);
			if(run_command(cmd,0))
			{
				ret = 0;
			}
		}
	}
	else 
	{
		if(!clean_display())
		{
			snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_P80_ID][1], ui_resource_dbtable[JPG_P80_ID][0], ui_resource_array[JPG_P80_ID][1]);
			if(run_command(cmd,0))
			{
				ret = 0;
			}
		}
	
	}
	
	return ret;
}

static int draw_upgrade_succeed(void)
{
	int ret = -1;
	char *cmd= NULL;
	const int cmd_size = 100;  
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	if(!clean_display())
	{
		snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_UPGRADE_SUCESS_ID][1], ui_resource_dbtable[JPG_UPGRADE_SUCESS_ID][0], ui_resource_array[JPG_UPGRADE_SUCESS_ID][1]);
		if(run_command(cmd,0))
		{
			ret = 0;
		}
	}
	return ret;	
}

static int draw_upgrade_fail(void)
{
	int ret = -1;
	char *cmd= NULL;
	const int cmd_size = 100;  
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	if(!clean_display())
	{
		snprintf( cmd, cmd_size, "kk_boot_draw_jpg 0x%x 0x%x %s 0 0 1920 1080 0 0",ui_resource_dbtable[JPG_UPGRADE_FAIL_ID][1], ui_resource_dbtable[JPG_UPGRADE_FAIL_ID][0], ui_resource_array[JPG_UPGRADE_FAIL_ID][1]);
		if(run_command(cmd,0))
		{
			ret = 0;
		}
	}
	return ret;	
}

int draw_upgrade_status(ENUM_UPGRADE_STATUS eStatus, unsigned short percent )
{
	int ret = -1;
	clean_display();
	if(sUpgradeUiInitStat == E_UPGRADE_UI_UNINIT) {
		ret = init_upgrade_ui();
		sUpgradeUiInitStat = (ret==0? E_UPGRADE_UI_INIT_OK : E_UPGRADE_UI_INIT_FAILED);
		printf("init upgrade %s!\n", ret==0? "SUCCESS" : "FAILED");
	}

	if( sUpgradeUiInitStat == E_UPGRADE_UI_INIT_FAILED )
	{
		return ret;
	}

	switch(eStatus)
	{
		case UPGRADE_INIT_USB_FAIL:
			ret = draw_upgrade_init_usb_fail();
			break;
		case UPGRADE_SEARCH_STORAGE:
			ret = draw_upgrade_search_storage();
			break;
		case UPGRADE_SEARCH_STORAGE_FAIL:
			ret = draw_upgrade_search_storage_fail();
			break;
		case UPGRADE_SEARCH_UPGARDE_FILE:
			ret = draw_upgrade_search_upgrade_file();
			break;
		case UPGRADE_SEARCH_UPGARDE_FILE_FAIL:
			ret = draw_upgrade_search_upgrade_file_fail();
			break;
		case UPGRADE_PROGRESS:
			ret = draw_upgrade_progress(percent);
			break;
		case UPGRADE_SUCCEED:
			ret = draw_upgrade_succeed();
			break;	
		case UPGRADE_FAIL:
			ret = draw_upgrade_fail();
			break;		
			
	}
	return ret;
}


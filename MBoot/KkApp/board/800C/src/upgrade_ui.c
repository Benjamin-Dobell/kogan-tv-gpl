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
	return 0;
}

static int clean_display(void)
{
	return 0;
}

static int draw_upgrade_search_storage(void)
{
	return 0;	
}

static int draw_upgrade_search_storage_fail(void)
{
	return 0;	
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
	return 0;
}

static int draw_upgrade_succeed(void)
{
	return 0;
}

static int draw_upgrade_fail(void)
{
	return 0;	
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


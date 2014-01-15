////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 Konka Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (锟斤拷MStar Confidential Information锟斤拷) by the recipient.
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
#include <upgrade_ui_database.h>
#include <cmd_kk_ui.h>
#include <CusConfig.h>

static E_UPGRADE_UI_INIT_STAT sUpgradeUiInitStat = E_UPGRADE_UI_UNINIT;


#define ENABLE_BL_UI 1

#define GRAPHIC_WIDTH       600
#define GRAPHIC_HEIGHT      400
#define GRAPHIC_X           60
#define GRAPHIC_Y           88

//#define GRAPHIC_STRING_X           60
//#define GRAPHIC_STRING_Y           88
#define GRAPHIC_STRING_ATTRIBUTE           1

#define LINE_HEIGHT             50
#define RECT_LEFT_INTERVAL      50

static int bShowBac = true;
static int init_upgrade_ui(void)
{
	int ret = -1;
	ret = kk_ui_create_osdWin();
	return ret;
}

static int finalize_upgrade_ui(void)
{
	int ret = -1;
	ret = kk_ui_destory_osdWin();
	return ret;
}

static int draw_upgrade_init_usb_fail(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR
	char* string = "init usb fail";
#else
	char* string = "USB 初始化失败";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw flush error\n");
		//return ret;
	}
	udelay (5000*1000);
	return ret;	
}


static int draw_upgrade_search_storage(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "search usb stroage";
#else
	char* string = "寻找U 盘...";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw flush error\n");
		//return ret;
	}
	udelay (5000*1000);
	return ret;	
}

static int draw_upgrade_search_storage_fail(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "no usb stroage";
#else
	char* string = "没有U 盘";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 1, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw flush error\n");
		//return ret;
	}
	udelay (5000*1000);
	return ret;	
}

static int draw_upgrade_search_upgrade_file(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "search upgrade file";
#else
	char* string = "寻找升级文件...";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	return ret;	
}

static int draw_upgrade_search_upgrade_file_fail(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "no upgrade file";
#else
	char* string = "没有升级文件";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	return ret;	
}

static int draw_upgrade_load_upgrade_file(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "load upgrade file";
#else
	char* string = "加载升级文件...";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	return ret;	
}

static int draw_upgrade_load_upgrade_file_fail(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "load upgrade file fail!!";
#else
	char* string = "加载升级文件失败";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	return ret;	
}

static int draw_upgrade_progress(unsigned short percent)
{
	
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "upgrade software!!";
	char* string1 = "do not turn off!!";
#else	
	char* string = "升级中...";
	char* string1 = "勿断电";
#endif	
	printf("percent ===== %d\n",percent);
	if(bShowBac)
	{
		ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
		if(ret < 0)
		{
			printf("draw rect error\n");
			//return ret;
		}
		ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 2, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
		if(ret < 0)
		{
			printf("draw string error\n");
			//return ret;
		}
		ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string1);
		if(ret < 0)
		{
			printf("draw string error\n");
			//return ret;
		}
		ret = kk_ui_draw_progress(GRAPHIC_X + RECT_LEFT_INTERVAL,  GRAPHIC_Y + LINE_HEIGHT * 5, percent, 0x3fffffff);
		if(ret < 0)
		{
			printf("draw progress error\n");
			//return ret;
		}	
		bShowBac = false;
	}
	else
	{
		ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 5, GRAPHIC_WIDTH, LINE_HEIGHT, 0x800000ff);
		if(ret < 0)
		{
			printf("draw rect error\n");
			//return ret;
		}
		ret = kk_ui_draw_progress(GRAPHIC_X + RECT_LEFT_INTERVAL,  GRAPHIC_Y + LINE_HEIGHT * 5, percent, 0x3fffffff);
		if(ret < 0)
		{
			printf("draw progress error\n");
			//return ret;
		}	
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	return ret;
}

static int draw_upgrade_succeed(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "upgrade success,please wait reset!!";
#else
	char* string = "升级成功，重启中...";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	return ret;	
}

static int draw_upgrade_fail(void)
{
	int ret = -1;
#if 	USB_UPGRADE_USE_ENGLISH_STR	
	char* string = "upgrade fail!!";
#else
	char* string = "升级失败";
#endif
	ret = kk_ui_draw_rect(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0x800000ff);
	if(ret < 0)
	{
		printf("draw rect error\n");
		//return ret;
	}
	ret = kk_ui_draw_string(GRAPHIC_X, GRAPHIC_Y + LINE_HEIGHT * 3, 0x3fffffff, GRAPHIC_STRING_ATTRIBUTE, string);
	if(ret < 0)
	{
		printf("draw string error\n");
		//return ret;
	}
	ret = kk_ui_flush_osd();
	if(ret < 0)
	{
		printf("draw flush error\n");
		//return ret;
	}
	return ret;	
}

int draw_upgrade_status(ENUM_UPGRADE_STATUS eStatus, unsigned short percent )
{
	int ret = -1;
	if(sUpgradeUiInitStat == E_UPGRADE_UI_UNINIT) {
		ret = init_upgrade_ui();
		sUpgradeUiInitStat = (ret==0? E_UPGRADE_UI_INIT_OK : E_UPGRADE_UI_INIT_FAILED);
		printf("init upgrade %s!\n", ret==0? "SUCCESS" : "FAILED");
	}
	if( sUpgradeUiInitStat == E_UPGRADE_UI_INIT_FAILED )
	{
		printf("init error error\n");
		//return ret;
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
		case UPGRADE_LOAD_UPGARDE_FILE:
			ret = draw_upgrade_load_upgrade_file();
			break;
		case UPGRADE_LOAD_UPGARDE_FILE_FAIL:
			ret = draw_upgrade_load_upgrade_file_fail();
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


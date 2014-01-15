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
#include <config.h>
#include <malloc.h>
#include <stdio.h> 

#define KK_UI_DBG(x)             x

int kk_ui_create_osdWin(void)
{
	return 0;
}

int kk_ui_draw_rect(unsigned short dispX, unsigned short dispY, unsigned short dispWidth, unsigned short dispHeiht, unsigned int color)
{
	return 0;
}

int kk_ui_draw_progress(unsigned short dispX, unsigned short dispY, unsigned short per, unsigned int color)
{
	return 0;
}

int kk_ui_draw_string(unsigned short dispX, unsigned short dispY, unsigned int color, unsigned short attrib, char* string)
{
	return 0;
}


int kk_ui_draw_jpg(unsigned int  jpgOffset, unsigned int  jpgSize, char* jpgCrc, unsigned short dispX, unsigned short dispY, unsigned short dispWidth,unsigned short  dispHeiht,unsigned char  bHorStretch, unsigned char  bVerStretch)  //
{
	KK_UI_DBG(printf("u32JpgOffset ======= 0x%x \n", jpgOffset));
	KK_UI_DBG(printf("u32JpgSize ======= 0x%x \n", jpgSize));
	KK_UI_DBG(printf("jpgCrc: %s\n", jpgCrc));
	const int cmd_size = 150;
	char *cmd= NULL;
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	snprintf(cmd, cmd_size, "boot_draw_jpg 0x%x 0x%x 0x%x 0x%x %d %d  0x%x 0x%x %s ui_upgrade_table",dispX, dispY, dispWidth, dispHeiht, bHorStretch, bVerStretch, jpgOffset, jpgSize, jpgCrc);
	if(run_command(cmd, 0))
	{
		if(cmd)
    			free(cmd);
		return 0;
	}
	else
	{
		if(cmd)
    			free(cmd);
		KK_UI_DBG(printf("display jpg error..>>>\n"));
		return -1;
	}
}

int kk_ui_flush_osd(void)
{
	return 0;
}

int kk_ui_clean_osdWin(void)
{
	if(run_command("destroy_logo", 0))	
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int do_kk_boot_create_osdWin (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = -1;
	ret = kk_ui_create_osdWin();
	return ret;
	
}

int do_kk_boot_draw_rect(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = -1;
	unsigned short dispX;
	unsigned short dispY;
	unsigned short dispW;
	unsigned short dispH;
	unsigned int color;
	if (argc == 6)
	{
		dispX = simple_strtoul(argv[1], NULL, 10);
		dispY = simple_strtoul(argv[2], NULL, 10);
		dispW = simple_strtoul(argv[3], NULL, 10);
		dispH = simple_strtoul(argv[4], NULL, 10);
		color = simple_strtoul(argv[5], NULL, 10);
	}
	else
	{
		return -1;
	}
	ret = kk_ui_draw_rect(dispX, dispY, dispW, dispH, color);
	return ret;
}

int do_kk_boot_draw_progress(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = -1;
	unsigned short dispX;
	unsigned short dispY;
	unsigned short per;
	unsigned int color;
	if (argc == 5)
	{
		dispX = simple_strtoul(argv[1], NULL, 10);
		dispY = simple_strtoul(argv[2], NULL, 10);
		per = simple_strtoul(argv[3], NULL, 10);
		color = simple_strtoul(argv[4], NULL, 10);
	}
	else
	{
		return -1;
	}
	ret = kk_ui_draw_progress(dispX, dispY, per, color);
	return ret;
}

int do_kk_boot_draw_string(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = -1;
	unsigned short dispX;
	unsigned short dispY;
	unsigned int color;
	unsigned short attrib;
	if (argc == 6)
	{
		dispX = simple_strtoul(argv[1], NULL, 10);
		dispY = simple_strtoul(argv[2], NULL, 10);
		color = simple_strtoul(argv[3], NULL, 10);
		attrib = simple_strtoul(argv[4], NULL, 10);
	}
	else
	{
		return -1;
	}
	ret = kk_ui_draw_string(dispX, dispY, color, attrib, argv[5]);
	return ret;
}

int do_kk_boot_draw_jpg (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = -1;
	unsigned int jpgOffset;
	unsigned int jpgSize;
	unsigned short dispX;
	unsigned short dispY; 
	unsigned short dispWidth; 
	unsigned short dispHeight; 
	unsigned char bHorStretch;
	unsigned char bVerStretch;

	/* check argc */
	if (argc == 10)
	{
		jpgOffset = simple_strtoul(argv[1], NULL, 10);
		jpgSize = simple_strtoul(argv[2], NULL, 10);
		dispX = simple_strtoul(argv[4], NULL, 10);
		dispY = simple_strtoul(argv[5], NULL, 10);
		dispWidth = simple_strtoul(argv[6], NULL, 10);
		dispHeight = simple_strtoul(argv[7], NULL, 10);	
		bHorStretch = simple_strtoul(argv[8], NULL, 10);
		bVerStretch = simple_strtoul(argv[9], NULL, 10);
	}
	else
	{
         	return -1;
	}

	ret = kk_ui_draw_jpg(jpgOffset,jpgSize,argv[3],dispX,dispY,dispWidth,dispHeight,bHorStretch,bVerStretch);
	return ret;
}

int do_kk_boot_flush_OsdWin (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = -1;
	ret = kk_ui_flush_osd();
	return ret;
}

int do_kk_boot_clean_OsdWin (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = -1;
	ret = kk_ui_clean_osdWin();
	return ret;
}



U_BOOT_CMD(
kk_boot_create_osdwin,    1,   1,  do_kk_boot_create_osdWin,
"kk_boot_create_osdwin - create osd win \n",
"create osd win \n"
);

U_BOOT_CMD(
kk_boot_draw_rect,    6,   1,  do_kk_boot_draw_rect,
"kk_boot_draw_rect - draw rect \n",
"draw rect\n"
"    - dispX: display X-axis\n"
"    - dispY: display Y-axis\n"
"    - dispWidth: display Width\n"
"    - dispHeight: display Height\n"
"    - color: color\n"
);

U_BOOT_CMD(
kk_boot_draw_progress,    5,   1,  do_kk_boot_draw_progress,
"kk_boot_draw_progress - draw progress \n",
"draw progress \n"
"    - dispX: display X-axis\n"
"    - dispY: display Y-axis\n"
"    - per: percent\n"
"    - color: color\n"
);

U_BOOT_CMD(
kk_boot_draw_string,    6,   1,  do_kk_boot_draw_string,
"kk_boot_draw_string - draw string \n",
"draw string \n"
"    - dispX: display X-axis\n"
"    - dispY: display Y-axis\n"
"    - color: percent\n"
"    - attrib: align attrib, 0: left, 1: middle, 2: right\n"
  "    - string: in char\n"
);


U_BOOT_CMD(
kk_boot_draw_jpg,    10,   1,  do_kk_boot_draw_jpg,
"kk_boot_draw_jpg - Logo display \n",
"Boot up logo display\n"
"    - u16JpgId: Display jpg-id \n"
"    - DispX: Display X-axis\n"
"    - DispY: Display Y-axis\n"
"    - DispWidth: Display Width\n"
"    - DispHeight: Display Height\n"
"    - HorStretch: Horizontal Stretch\n"
"    - VerStretch: Vertical Stretch\n"
);

U_BOOT_CMD(
kk_boot_flush_osdwin,    1,   1,  do_kk_boot_flush_OsdWin,
"kk_boot_flush_osdwin - clean display window \n",
"clean display window \n"
);

U_BOOT_CMD(
kk_boot_destory_osdwin,    1,   1,  do_kk_boot_destory_OsdWin,
"kk_boot_destory_osdwin - desotry display window \n",
"destory display window \n"
);


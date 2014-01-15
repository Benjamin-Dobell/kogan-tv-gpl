////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1980-2012 Konka Group.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (��Konka Confidential Information��) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of Konka Confidential
// Information is unlawful and strictly prohibited. Konka hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdio.h> 

#include <upgrade_usb_util.h>
#include <upgrade_ui.h>
#include <kk_board.h>
#include <upgrade_impl.h>
#include <upgrade_usb.h>
#include <term_color.h>
//#include <MsUpgradeUtility.h>

#define UPGRADE_USB_DEBUG
#undef debugcl
#ifdef	UPGRADE_USB_DEBUG
#define debugcl(fg,fmt,args...)	PRINTF_CLF (fg, fmt, ##args)
#else
#define debugcl(fmt,args...)
#endif	

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef MAX_DONTWRITE_COUNT
#define MAX_DONTWRITE_COUNT 20
#endif
static char dont_overwrite_flag=0;
static char dont_overwrite_count=0;
static char* pdont_overwrite_partition[MAX_DONTWRITE_COUNT];

static char get_dont_overwrite_flag(void)
{
    return dont_overwrite_flag;
}

static int will_skip_cmd(char *str)
{
    char count=0;
    char *p=NULL;
    for(count=0;count<dont_overwrite_count;count++){

        {
            p=strstr(str,pdont_overwrite_partition[count]);
            if(p!=NULL){
				size_t dont_overwrite_len = strlen(pdont_overwrite_partition[count]);
                if((*(p+dont_overwrite_len))==' '||
                    (*(p+dont_overwrite_len))==0||
                       (*(p+dont_overwrite_len))=='\r'||
                       (*(p+dont_overwrite_len))=='\n'){

                            if(p==str){ //start from str's first char
                                printf("\033[0;32m skip cmd:%s\033[0m\n",str);
                                return TRUE; //skip this cmd
                            }
                            else{
                                if(*(p-1)==' ')
                                {
                                    printf("\033[0;32m skip cmd:%s\033[0m\n",str);
                                    return TRUE; //skip this cmd
                                }
                            }//if(p==str){ //start from str's first char
               }//if((*(p+strlen(pdont_overwrite_partition[count]))==' ')||
            }//if(p!=NULL){
        }//if(count==0){
    }//for(count=0;count<dont_overwrite_count;count++){

    return FALSE; // don't skip this cmd
}

// if dont_overwrite_skip_cmd return TRUE, we skip this line.
// if dont_overwrite_skip_cmd return FALSE, we execute this line.
static int check_skip_cmd(char *next_line)
{
    char *pLine = NULL;
    if(get_dont_overwrite_flag()==TRUE)
    {
        if(will_skip_cmd(next_line)==TRUE)
        {
            return TRUE;
        }
    }
    //This part is for filter command. Turncat trailing comment.
    pLine=strstr(next_line, "#");
    if(pLine!=NULL){
        *pLine='\0';
    }
    return FALSE;
}

int do_skip_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if(argc < 2) {
		printf( "usage: dont_overwrite <skipped_cmd>\n" );
		return 1;
	}
	if(dont_overwrite_count >= MAX_DONTWRITE_COUNT) {
		printf( "skip command list is already full!\n" );
		return 2;
	}
	pdont_overwrite_partition[dont_overwrite_count]=malloc(strlen(argv[1])+1);
	if(pdont_overwrite_partition[dont_overwrite_count]==NULL){
		printf("\033[0;34m [Error Malloc] %s:%s:%d\033[0m\n",__FILE__,__FUNCTION__,__LINE__);
		return -1;
	}
	memset(pdont_overwrite_partition[dont_overwrite_count],0,strlen(argv[1])+1);
	strncpy(pdont_overwrite_partition[dont_overwrite_count],argv[1], strlen(argv[1]));
	++dont_overwrite_count;

	return 0;
}

int kk_upgrade(char* upgradeFile, LoadUpdateScript loadFunc, int progressStep)
{
	char *next_line = NULL;
	char *script_buffer = 0;
	int usbPort = 0;
	int progress = 0;
	const int cmd_size = 150;
	char *cmd= NULL;
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	

	/* start USB */
	//setUpgradeMode(EN_UPDATE_MODE_USB);
	usb_stop_all();
	draw_upgrade_status(UPGRADE_SEARCH_STORAGE, 0);
	if(0 != usb_stor_scan_all(&usbPort)) {
		debugcl(TERM_FG_RED, "usb_init failed!\n");
		draw_upgrade_status(UPGRADE_INIT_USB_FAIL, 0);
		return -1;
	}
	
	//Load Script
	if(loadFunc == NULL || loadFunc(upgradeFile, &script_buffer) == -1)
	{
		debugcl(TERM_FG_RED, "LoadScript2Dram failed!\n");
		draw_upgrade_status(UPGRADE_SEARCH_STORAGE_FAIL, 0);
		return -1;
	}
	debugcl(TERM_FG_CYAN, "script_buffer=%p\n", script_buffer);

	progress += progressStep;
	//Run Script
	while ((next_line = kk_get_script_next_line(&script_buffer)) != NULL)
	{
		debugcl (TERM_FG_GREEN, "\n>> %s \n", next_line);

		if (strncmp("reset", next_line, 5) == 0)
		{
			draw_upgrade_status(UPGRADE_SUCCEED, 100);
			udelay(2000000);
		}	
		else
		{
		   draw_upgrade_status(UPGRADE_PROGRESS, progress);
		   if(progress < 100)
		   {
			   progress += progressStep;
		   }
		}
		if(check_skip_cmd(next_line)== FALSE)//filterCmd
		{
			debugcl (TERM_FG_YELLOW, "\n>> %s \n", next_line);
			snprintf(cmd, cmd_size, "%s",next_line);
			printf("cmd ====%s \n",cmd );
			if(run_command(cmd, 0) < 0)
			{
				debugcl(TERM_FG_RED, "Error> cmd: %s Fail !!\n", next_line);
				draw_upgrade_status(UPGRADE_FAIL, progress);
				return -1;
			}
			memset( cmd, 0, cmd_size );
		}

	}
	draw_upgrade_status(UPGRADE_SUCCEED, 100);
	udelay(2000000);
	run_command("reset",0);
	return 0;
}


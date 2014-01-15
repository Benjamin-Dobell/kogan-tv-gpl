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
#include <kk_board.h>

#ifndef IS_LINE_END
#define IS_LINE_END(x)		('\r' == (x)|| '\n' == (x))
#endif

#ifndef IS_COMMENT
#define IS_COMMENT(x)       ('#' == (x))
#endif

#ifndef IS_FILE_END
#define IS_FILE_END(x)      (SCRIPT_FILE_END == (x))
#endif

#ifndef MAX_LINE_SIZE
#define MAX_LINE_SIZE		512
#endif

/********
following code is portable, do not change
*********
*/
int kk_check_file_partition(char* tmpBuffer, char *upgradeFile, int *device, int *partition)
{
	int g_device = 0;
	int g_partition = 0;
    char buffer[CONFIG_SYS_CBSIZE] = "\0";
    for(g_device = 0 ; g_device < MAX_DEVICE ;g_device++)
    {
        for(g_partition = 0 ; g_partition < MAX_PARTITION ; g_partition++)
        {
            snprintf(buffer, CONFIG_SYS_CBSIZE, "fatload usb %d:%d  %X %s 1",g_device, g_partition, tmpBuffer, upgradeFile);
            printf("cmd: %s \n",buffer);
            if(run_command(buffer, 0) == 0)
            {
               if((device != NULL) && (partition != NULL))
               {
                   *device=g_device;
                   *partition=g_partition;
               }
               return 0;
            }    
        }
    }    
    return -1;
}

char *kk_get_script_next_line(char **line_buf_ptr)
{
    char *line_buf;
    char *next_line;
    int i;

    line_buf = (*line_buf_ptr);
    //for skip the unnecessary space at the line start.
    line_buf = line_buf + strspn(line_buf," ");
    // strip '\r', '\n' and comment
    while (1)
    {
        // strip '\r' & '\n'
        if (IS_LINE_END(line_buf[0]))
        {
            line_buf++;
        }
        // strip comment
        else if (IS_COMMENT(line_buf[0]))
        {
            for (i = 0; !IS_LINE_END(line_buf[0]) && i <= MAX_LINE_SIZE; i++)
            {
                line_buf++;
            }

            if (i > MAX_LINE_SIZE)
            {
                line_buf[0] = SCRIPT_FILE_END;

                printf ("Error: the max size of one line is %d!!!\n", MAX_LINE_SIZE); // <-@@@

                break;
            }
        }
        else
        {
            break;
        }
    }

    // get next line
    if (IS_FILE_END(line_buf[0]))
    {
        next_line = NULL;
    }
    else
    {
        next_line = line_buf;

        for (i = 0; !IS_LINE_END(line_buf[0]) && i <= MAX_LINE_SIZE; i++)
        {
            line_buf++;
        }

        if (i > MAX_LINE_SIZE)
        {
            next_line = NULL;

            printf ("Error: the max size of one line is %d!!!\n", MAX_LINE_SIZE); // <-@@@
        }
        else
        {
            line_buf[0] = '\0';
            *line_buf_ptr = line_buf + 1;
        }
    }

    return next_line;
}


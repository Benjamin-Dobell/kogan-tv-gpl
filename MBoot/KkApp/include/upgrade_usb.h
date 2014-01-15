#ifndef _UPGRADE_USB_H
#define _UPGRADE_USB_H

#include <command.h>

typedef int (*LoadUpdateScript)(char* /*in, upgradeFile*/, char** /*out, script_buffer*/);

int kk_upgrade(char* upgradeFile, LoadUpdateScript loadFunc, int progressStep);

int do_skip_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

#endif


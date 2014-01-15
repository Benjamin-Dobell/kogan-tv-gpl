#include <common.h>
#include <upgrade_usb.h>
#include <upgrade_impl.h>
#ifndef ARG_NUM_SCRIPT_FILE
#define ARG_NUM_SCRIPT_FILE         1
#endif

#ifndef ARG_SCRIPT_FILE
#define ARG_SCRIPT_FILE(x)              (x)[ARG_NUM_SCRIPT_FILE]
#endif

#ifndef IS_ARG_AVAILABLE_SCRIPT_FILE
#define IS_ARG_AVAILABLE_SCRIPT_FILE(x) ((x) > ARG_NUM_SCRIPT_FILE)
#endif


U_BOOT_CMD(
skip_cmd,    3,   1,  do_skip_cmd,
"skip_cmd <add <word-match-command> | clear>",
"skip_cmd - add or clear commands to skip when executing script \n"
);


int do_usb_upgrade(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *upgradeFile = IS_ARG_AVAILABLE_SCRIPT_FILE(argc) ? ARG_SCRIPT_FILE(argv) : NULL;
	return kk_upgrade(upgradeFile, LoadUpgradeScript2Dram, 1);
}

U_BOOT_CMD(
usb_upgrade,    2,   1,  do_usb_upgrade,
	"usb_upgrade	 - update kernal & root file system automatically by script file\n",
		"[script_file_name] \n"
);


int do_usb_bootloader_upgrade(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *upgradeFile = IS_ARG_AVAILABLE_SCRIPT_FILE(argc) ? ARG_SCRIPT_FILE(argv) : NULL;
	return kk_upgrade(upgradeFile, LoadBootloaderUpgradeScript2Dram, 40);
}

U_BOOT_CMD(
usb_bootloader_upgrade,    2,   1,  do_usb_bootloader_upgrade,
	"usb_bootloader_upgrade	 - update bootloader automatically by script file\n",
		"[script_file_name] \n"
);


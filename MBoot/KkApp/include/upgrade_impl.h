#ifndef _UPGRADE_UI_H
#define _UPGRADE_UI_H

int LoadBootloaderUpgradeScript2Dram(char* upgradeFile, char** script_buffer);

int LoadUpgradeScript2Dram(char *upgradeFile, char ** script_buffer);

int usb_stop_all(void);

int usb_stor_scan_all(int* portId);

#endif

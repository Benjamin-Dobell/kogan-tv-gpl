#ifndef _UPGRADE_USB_UTIL_H
#define _UPGRADE_USB_UTIL_H

int kk_check_file_partition(char* tmpBuffer, char *upgradeFile, int *device, int *partition);

char *kk_get_script_next_line(char **line_buf_ptr);

#endif


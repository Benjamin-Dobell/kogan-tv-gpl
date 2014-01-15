#ifndef _BOARD_6A80X_H
#define _BOARD_6A80X_H

#include <MsTypes.h>
#undef MAX_DEVICE
#define MAX_DEVICE 3   /*      USB prot ����     */

#undef MAX_PARTITION
#define MAX_PARTITION 20   /*     �洢ϵͳ���FLASH ����������     */

#undef SCRIPT_FILE_END
#define SCRIPT_FILE_END '%'

#undef SCRIPT_SIZE
#define SCRIPT_SIZE 0x4000

#undef DEFAULT_MBOOT_BIN_PATH
#define DEFAULT_MBOOT_BIN_PATH			"/mboot.bin"

#undef DEFAULT_USB_UPGRADE_BIN_PATH
#define DEFAULT_USB_UPGRADE_BIN_PATH	"/MstarUpgrade.bin"

#undef KK_SPI_SECTOR_SIZE
#define KK_SPI_SECTOR_SIZE         0x10000   /*      6A80Xʹ�õ�FLASHÿһ����Ĵ�СΪ64K        */
#undef KK_SPI_SIZE
#define KK_SPI_SIZE             16*KK_SPI_SECTOR_SIZE   /*      6A80Xʹ�õ�FLASH�Ĵ�СΪ1M byte       */

#endif


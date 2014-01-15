#ifndef _BOARD_6A80X_H
#define _BOARD_6A80X_H

#include <MsTypes.h>
#undef MAX_DEVICE
#define MAX_DEVICE 3   /*      USB prot 个数     */

#undef MAX_PARTITION
#define MAX_PARTITION 20   /*     存储系统软件FLASH 最大分区个数     */

#undef SCRIPT_FILE_END
#define SCRIPT_FILE_END '%'

#undef SCRIPT_SIZE
#define SCRIPT_SIZE 0x4000

#undef DEFAULT_MBOOT_BIN_PATH
#define DEFAULT_MBOOT_BIN_PATH			"/mboot.bin"

#undef DEFAULT_USB_UPGRADE_BIN_PATH
#define DEFAULT_USB_UPGRADE_BIN_PATH	"/MstarUpgrade.bin"

#undef KK_SPI_SECTOR_SIZE
#define KK_SPI_SECTOR_SIZE         0x10000   /*      6A80X使用的FLASH每一个块的大小为64K        */
#undef KK_SPI_SIZE
#define KK_SPI_SIZE             16*KK_SPI_SECTOR_SIZE   /*      6A80X使用的FLASH的大小为1M byte       */

#endif


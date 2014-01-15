#ifndef _KK_UI_DATABASE_H
#define _KK_UI_DATABASE_H


#define KK_DATABASE_DBG(x)                      x

#undef UI_OFFSET_IN_SPI
#define UI_OFFSET_IN_SPI 		0xC0000   /*      图片资源存放在第0x0c个块       */
#undef MAX_TOTAL_UI_SIZE
#define MAX_TOTAL_UI_SIZE 		0x10000   /*      图片资源最大为64K       */
#undef JPG_ADDR_IN_RAW
#define JPG_ADDR_IN_RAW 		0x45000000   /*      内存中临时存放图片的地方       */
#undef PIC_SOURCE_PARTITION
#define PIC_SOURCE_PARTITION         "customercfg"     /*      存放图片资源的分区       */

typedef enum 
{
	JPG_USB_INIT_FAIL_ID = 0,
	JPG_NO_USB_STORAGE_ID,
	JPG_FIND_ID,	
	JPG_NOT_UPGRADE_FILE_ID,	
	JPG_P0_ID,	
	JPG_P20_ID,	
	JPG_P40_ID,	
	JPG_P60_ID,	
	JPG_P80_ID,	
	JPG_UPGRADE_SUCESS_ID,	
	JPG_UPGRADE_FAIL_ID,	
	JPG_FILE_MAX,
}JPG_FILE_ID;
	
void kk_count_data_crc32_setenv(unsigned int addr,int size,char *envName);
int kk_count_data_crc32_getenv(unsigned int addr,int size,char *envName);
int kk_mount_vfs_partition(void);
int kk_load_vfs_uiToDram(char *fileDir, unsigned int addr, char *envName);
int kk_prepare_ui_dbtable(void);
int kk_write_ui_dbtable(void);
int kk_load_uiToSpiFlash(void);
int kk_read_ui_dbtable(void);
#endif


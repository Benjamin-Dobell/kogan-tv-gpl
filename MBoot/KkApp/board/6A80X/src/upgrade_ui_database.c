#include <common.h>
#include <command.h>
#include <config.h>
#include <malloc.h>
#include <upgrade_ui_database.h>
#include <stdio.h> 
#include <kk_vfs.h>
#include <kk_board.h>


char * ui_resource_array[JPG_FILE_MAX][2] =
{
	{"/customercfg/upgrade/picture/boot_invalid_usb.jpg", "ui_invalid_usb_crc"},
	{"/customercfg/upgrade/picture/boot_invalid_usb.jpg", "ui_invalid_usb_crc"},	
	{"/customercfg/upgrade/picture/boot_find.jpg", "ui_boot_find_crc"},	
	{"/customercfg/upgrade/picture/boot_not_upgrade_file.jpg", "ui_not_upgrade_file_crc"},
	{"/customercfg/upgrade/picture/boot_p0.jpg", "ui_boot_p0_crc"},
	{"/customercfg/upgrade/picture/boot_p20.jpg", "ui_boot_p20_crc"},
	{"/customercfg/upgrade/picture/boot_p40.jpg", "ui_boot_p40_crc"},
	{"/customercfg/upgrade/picture/boot_p60.jpg", "ui_boot_p60_crc"},
	{"/customercfg/upgrade/picture/boot_p80.jpg", "ui_boot_p80_crc"},
	{"/customercfg/upgrade/picture/boot_upgrade_sucess.jpg", "ui_upgrade_sucess_crc"},
	{"/customercfg/upgrade/picture/boot_upgrade_fail.jpg", "ui_upgrade_fail_crc"},
};
unsigned int ui_resource_dbtable[JPG_FILE_MAX][2];

//int Ready_LoadNand_Env(void)
void kk_count_data_crc32_setenv(unsigned int addr,int size,char *envName)
{
    char buf[32];
    ulong checksum;
    checksum = crc32(0,(unsigned char *)addr,size);
    snprintf(buf,sizeof(buf),"%lx",checksum);
    setenv(envName,buf);
    printf("%s checksum =: 0x%lx ... %s \n",envName,checksum,buf); 	
    KK_DATABASE_DBG(printf("%s checksum =: 0x%lx ... %s \n",envName,checksum,buf));
}

int kk_count_data_crc32_getenv(unsigned int addr,int size,char *envName)
{
    int ret = -1;
    ulong checksum,savesum;
    char * p_str = NULL;
    checksum = crc32(0,(unsigned char *)addr,size);
    p_str = getenv(envName);
    if(p_str == NULL)
    {
        savesum = 0;
    }
    else
    {
        savesum = simple_strtoul(p_str,NULL,16);
    }
    if(checksum == savesum)
    {
        KK_DATABASE_DBG(printf("## CRC check done, %s\n", envName));
        ret = 0;
    }
    return ret;
}

int kk_mount_vfs_partition(void)
{
	int ret = -1;
	ret = kk_vfs_init();
	ret= kk_vfs_mount(PIC_SOURCE_PARTITION);
	return ret;
}

int kk_load_vfs_uiToDram(char *fileDir, unsigned int addr, char *envName)
{
	int ret = -1;
	unsigned int fileSize = 0;
	fileSize =  kk_vfsfile_getsize(fileDir);
	if(fileSize > 0)
	{
		if (kk_vfsfile_load(fileDir,addr) == 0)
		{
			kk_count_data_crc32_setenv(addr,fileSize,envName);
			return 0;
		}
		else
		{
			KK_DATABASE_DBG(printf(" load file from emmc to ram error \n"));
		}
	}
	else
	{
		KK_DATABASE_DBG(printf("get filesize get error \n"));
	}
	return ret;
}

int kk_prepare_ui_dbtable(void)
{
	int ret = -1;
	int i = 0;
	unsigned int fileSize = 0;
	unsigned int totalUiSize = 0;
	memset(ui_resource_dbtable,0,sizeof(ui_resource_dbtable));
	totalUiSize = sizeof(ui_resource_dbtable);
	for( i =0; i < JPG_FILE_MAX; i++)
	{
		fileSize = kk_vfsfile_getsize(ui_resource_array[i][0]);
		if(fileSize > 0)
		{
			KK_DATABASE_DBG(printf("%s fileSize ====0x%x! \n", ui_resource_array[i][0],fileSize));
			ui_resource_dbtable[i][0] = fileSize;
			totalUiSize = totalUiSize + fileSize;
		}
		else
		{
			KK_DATABASE_DBG(printf("get vfs file size error!!!! \n"));
			return -1;
		}
		if(i > 0)
		{
			ui_resource_dbtable[i][1] = ui_resource_dbtable[i -1][1] + ui_resource_dbtable[i -1][0];
		}
		else
		{
			ui_resource_dbtable[0][1] = (unsigned int)UI_OFFSET_IN_SPI + sizeof(ui_resource_dbtable);
		}
	}
	KK_DATABASE_DBG(printf("totalUiSize ====0x%x! \n", totalUiSize));
	KK_DATABASE_DBG(printf("resouce db table size ====0x%x! \n", sizeof(ui_resource_dbtable)));
	if(totalUiSize > MAX_TOTAL_UI_SIZE) 
	{
		printf("spiflash space not enough \n");
		return -1;
	} 

	char *cmd= NULL;
	const int cmd_size = 50;  
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	snprintf( cmd, cmd_size, "spi_wblk 0x%x 0x%x 0x%x",(unsigned int)ui_resource_dbtable, (unsigned int)UI_OFFSET_IN_SPI, sizeof(ui_resource_dbtable));
       kk_count_data_crc32_setenv((unsigned int)ui_resource_dbtable,sizeof(ui_resource_dbtable),"ui_db_crc");
	if(run_command( cmd, 0))
	{
		ret = 0;
	}
	else
	{
		ret = -1;
	}
	if(cmd != NULL)
    		free(cmd);	
	KK_DATABASE_DBG(printf("ret ====0x%x! \n", ret));
	return ret;
	
}

int kk_load_uiToSpiFlash(void)
{
	int ret = 0;
	int i = 0;
	char *saved_data = NULL;
	unsigned int protectSize = 0;
	KK_DATABASE_DBG(printf("kk_load_uiToSpiFlash!!!! \n"));
	for(i = 0 ; i < JPG_FILE_MAX; i ++)
	{
		if((ret=kk_load_vfs_uiToDram(ui_resource_array[i][0], (unsigned int)JPG_ADDR_IN_RAW, ui_resource_array[i][1])) == 0)
		{
			protectSize = 	ui_resource_dbtable[i][1] - (unsigned int)UI_OFFSET_IN_SPI;
			if(protectSize <= 0)
			{
				KK_DATABASE_DBG(printf("have not resource!!!! \n"));
				return -1;
			}
			KK_DATABASE_DBG(printf("protectSize ====0x%x \n",protectSize));
			saved_data = (char*)malloc(KK_SPI_SECTOR_SIZE);
			if(saved_data == NULL)
			{
				KK_DATABASE_DBG(printf("malloc memory 0x10000 not success ...>>>\n"));
				return -1;
			}
			else
			{
				char *cmd= NULL;
				const int cmd_size = 50;
				cmd =(char*)malloc(cmd_size);
				memset( cmd, 0, cmd_size );
				snprintf(cmd, cmd_size, "spi_rdc 0x%x 0x%x 0x%x",(unsigned int)saved_data, (unsigned int)UI_OFFSET_IN_SPI, protectSize);
				if(!run_command( cmd, 0))	
				{
					KK_DATABASE_DBG(printf("read protect data fail...>>>\n"));
                			free(saved_data);
					free(cmd);		
					return -1;
				}
				if((protectSize + ui_resource_dbtable[i][0]) < KK_SPI_SECTOR_SIZE)
				{
					memcpy((char *)&saved_data[protectSize],(void*)JPG_ADDR_IN_RAW,ui_resource_dbtable[i][0]);
					memset( cmd, 0, cmd_size );
					snprintf( cmd, cmd_size, "spi_wblk 0x%x 0x%x 0x%x",(unsigned int)saved_data, (unsigned int)UI_OFFSET_IN_SPI, KK_SPI_SECTOR_SIZE);
					if(!run_command( cmd, 0))	
					{
						KK_DATABASE_DBG(printf("write  data fail...>>>\n"));
	                			free(saved_data);
						free(cmd);		
						return -1;
					}
					free(cmd);
					free(saved_data);
				}
				else
				{
					KK_DATABASE_DBG(printf("error ui resource is too big \n"));
					free(saved_data);
					free(cmd);		
					return -1;
				}
			}
		}
		else
		{
			break;
		}
	}
	return ret;
}


int kk_read_ui_dbtable(void)
{
	int ret = -1;
	char *cmd= NULL;
	const int cmd_size = 50;
	cmd =(char*)malloc(cmd_size);
	memset( cmd, 0, cmd_size );
	memset(ui_resource_dbtable,0,sizeof(ui_resource_dbtable));
	snprintf(cmd, cmd_size, "spi_rdc 0x%x 0x%x 0x%x",(unsigned int)ui_resource_dbtable, (unsigned int)UI_OFFSET_IN_SPI, sizeof(ui_resource_dbtable));
	KK_DATABASE_DBG(printf("cmd =====%s>>>\n", cmd));
	if(!run_command( cmd, 0))	
	{
		KK_DATABASE_DBG(printf("read spi error..>>>\n"));
		if(cmd)
    		free(cmd);
		return -1;
	}
	if(cmd)
    		free(cmd);
	ret = kk_count_data_crc32_getenv((unsigned int)ui_resource_dbtable, sizeof(ui_resource_dbtable), "ui_db_crc");
	if(ret)	
	{
		setenv("ui_upgrade_table","0");
       	saveenv();
		KK_DATABASE_DBG(printf("read ui dbtable error..>>>\n"));
	}
	return ret;
 }


int kk_write_ui_dbtable(void)
{
	int ret = -1;
	kk_mount_vfs_partition();
	if(kk_prepare_ui_dbtable()==-1)
	{
		printf("spi database not ready...>>>\n");
		 kk_vfs_deinit();
		ret = -1;
	}
	else
	{
		ret =kk_load_uiToSpiFlash();
		if(!ret)
		{
			setenv("ui_upgrade_table","1");
			saveenv();	
		}
	}
	return ret;
}



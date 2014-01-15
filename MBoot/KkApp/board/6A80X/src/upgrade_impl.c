#include <common.h>
#include <usb.h>
#include <malloc.h>
#include <stdio.h> 

#include <kk_board.h>
#include <upgrade_impl.h>
#include <upgrade_usb_util.h>


static char* sUsbUpgradeBootloaderScript = 0;

int LoadBootloaderUpgradeScript2Dram(char* upgradeFile, char** script_buffer)
{
	if(sUsbUpgradeBootloaderScript == NULL)
	{
		U32 u32Flashsize = KK_SPI_SIZE;
		if(u32Flashsize == 0)
			return -1;
		if(script_buffer == NULL)
			return -1;
		sUsbUpgradeBootloaderScript = (char*)malloc(CONFIG_SYS_CBSIZE);
		if(NULL == sUsbUpgradeBootloaderScript)
			return -1;
		if(upgradeFile == NULL)
			upgradeFile = DEFAULT_MBOOT_BIN_PATH;
		snprintf( sUsbUpgradeBootloaderScript, CONFIG_SYS_CBSIZE,
			"fatload usb 0 0x45000000 %s\n" "spi_wrc 0x45000000 0 0x%x\n" "reset\n%",
			upgradeFile, u32Flashsize);
		*script_buffer = sUsbUpgradeBootloaderScript;
	}
	return 0;
}


int LoadUpgradeScript2Dram(char *upgradeFile, char ** script_buffer)
{
    char buffer[CONFIG_SYS_CBSIZE] = "\0";
	char *tmp_script_buf = (char*)UPGRADE_BUFFER_ADDR;
    int partition = 0;
    int device = 0;

	if(upgradeFile == NULL)
		upgradeFile = DEFAULT_USB_UPGRADE_BIN_PATH;
    if(kk_check_file_partition(tmp_script_buf, upgradeFile,&device,&partition)!=0)
    {
        printf("fail : no file in usb\n");
        return -1;
    }

    //Load Script
    snprintf(buffer, CONFIG_SYS_CBSIZE, "fatload usb %d:%d  %X %s %X",device, partition, UPGRADE_BUFFER_ADDR, upgradeFile, SCRIPT_SIZE);
    if(-1 == run_command(buffer, 0))
    {
        printf("Error: cmd: %s\n",buffer);
        return -1;
    }
    /*Check script file is ciphertext or not(start)*/
     *script_buffer=(char *)UPGRADE_BUFFER_ADDR;
    /*
        //check script crc
        if(check_script_crc(upgradeFile)==-1)
        {
            printf("Error: check_script_crc Fail !!\n");
            return -1;
        }
      */
     return 0;
}

int usb_stop_all(void)
{
	int i = 0;
	for(i=0; i < MAX_DEVICE; ++i) {
		usb_stop(i);
	}
	return 0;
}

int usb_stor_scan_all(int* portId)
{
	int i = 0;
	for(i=0; i < MAX_DEVICE; ++i) {
		if(usb_init(i) == 0)
		{
			if(usb_stor_scan(1) == 0)
			{
				if(portId != NULL)
					*portId = i;
				return 0;
			}
		}
	}
	return -1;
}


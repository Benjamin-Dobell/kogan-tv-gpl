#!/bin/bash

FILE_PART_READ_CMD="filepartload"
CRC_BIN="./crc"
FULL_ERASE=0
NEW_MBOOT=1
FLASH_IS_EMMC=0
SCRIPT_SIZE=0x4000
SPILT_SIZE=16384
CPU_IS_ARM=1

function func_init_env_for_mboot(){
	#==============init env for mboot================================
    INPUT_IMG="../out/mboot.bin"
	SCRIPT_FILE="mboot_script.txt"
	OUTPUT_IMG="MbootUpgrade.bin"
    #==============init env for mboot done===========================
}

function func_init_env_for_sboot(){
    #==============init env for mboot================================
    INPUT_IMG="../out/sboot.bin"
	SCRIPT_FILE="sboot_script.txt"
	OUTPUT_IMG="SbootUpgrade.bin"
    #==============init env for mboot done===========================
}

function func_clean(){
if [ -f "$SCRIPT_FILE" ]; then
    rm -rf $SCRIPT_FILE;
    rm -rf $OUTPUT_IMG;
fi
}

function func_set_dont_overwrite(){
    printf "dont_overwrite_init\n"  >$SCRIPT_FILE
}

function func_init_common_env(){
    #==============init common env================================
	test -f $INPUT_IMG
    if [ $? != 0 ];then
        echo "Please Build Mboot Firstly !"
        exit 0
    fi
    INPUT_IMG_SIZE=$(stat -c%s $INPUT_IMG )
    if [ "$CPU_IS_ARM" == "1" ] ; then
        DRAM_BUF_ADDR=0x45000000
	else
		#Why 0x80210000?? In PM51 booting mode, PM51 is always alive in 0x80200000, and the size of PM.bin is 0x10000
        DRAM_BUF_ADDR=0x80210000
    fi
    #==============init common env end============================	
}

function func_set_crc_to_image(){
    #Set Only-Script CRC1
    $CRC_BIN -a $SCRIPT_FILE
    split -d -a 2 -b $SPILT_SIZE $SCRIPT_FILE $SCRIPT_FILE.tmp.
    cat $SCRIPT_FILE.tmp.01 >> $OUTPUT_IMG
    #Set Full-Bin CRC2
    $CRC_BIN -a $OUTPUT_IMG
    rm $SCRIPT_FILE.*
}

function func_copy_first_16byte(){
    #==============copy the first 16 bytes to last =================================
    dd if=$OUTPUT_IMG of=out.bin bs=16 count=1;
    cat out.bin >>$OUTPUT_IMG
    rm -rf out.bin
    #==============copy the first 16 bytes to last end=================================
}

function func_pad_script(){
    #==============pad script===============================
    SCRIPT_FILE_SIZE=$(stat -c%s $SCRIPT_FILE)
    PADDED_SIZE=$(($SCRIPT_SIZE-$SCRIPT_FILE_SIZE))
    for ((i=0; i<$PADDED_SIZE; i++))
    do
        printf "\xFF" >> $SCRIPT_FILE
    done
    #==============pad script done==========================

    #==============Mboot Align 4byte =======================
    INPUT_IMG_SIZE=$(stat -c%s $INPUT_IMG )
    RESULT=`expr $INPUT_IMG_SIZE  % 4`
    for ((i=0; i<$RESULT; i++))
    do
        printf "\xFF" >> $INPUT_IMG
    done
    #==============Mboot Align 4byte done===================
}

function func_update_mboot_spi(){
    #==================   env init  ==========================
    func_init_env_for_mboot;
	func_clean;
	func_init_common_env;
    #================= script content ========================
	func_set_dont_overwrite;
	printf "$FILE_PART_READ_CMD 0x%x \$(UpgradeImage) 0x%x 0x%x\n" $DRAM_BUF_ADDR $SCRIPT_SIZE $INPUT_IMG_SIZE >> $SCRIPT_FILE
	if [ "$FULL_ERASE" == "1" ] ; then
	    printf "$CMD_SPI_WP 0\n" >> $SCRIPT_FILE
	    printf "$CMD_SPI_EA\n" >> $SCRIPT_FILE
	    printf "$CMD_SPI_WP 1\n" >> $SCRIPT_FILE
	fi
	printf "$CMD_SPI_WRC 0x%x 0 0x%x\n" $DRAM_BUF_ADDR $INPUT_IMG_SIZE >> $SCRIPT_FILE
	printf "setenv upgrade_mode null\n" >> $SCRIPT_FILE
	printf "saveenv\n" >> $SCRIPT_FILE
    #================= script content done ====================
	func_post_process;
}

function func_update_sboot_emmc(){
    #==================   env init  ==========================
    func_init_env_for_sboot;
	func_clean;
	func_init_common_env;
    #================= script content ========================
	func_set_dont_overwrite;
	printf "$FILE_PART_READ_CMD 0x%x \$(UpgradeImage) 0x%x 0x%x\n" $DRAM_BUF_ADDR $SCRIPT_SIZE $INPUT_IMG_SIZE >> $SCRIPT_FILE
	if [ "$FULL_ERASE" == "1" ] ; then
	   printf "mmc erase.boot 1\n" >> $SCRIPT_FILE
	fi  
	printf "mmc write.boot 1 0x%x 0 0x%x\n" $DRAM_BUF_ADDR $INPUT_IMG_SIZE >> $SCRIPT_FILE
    #================= script content done ====================
	func_post_process;
}

function func_update_mboot_emmc(){
    #==================   env init  ==========================
    func_init_env_for_mboot;
	func_clean;
	func_init_common_env;
    #================= script content ========================
	func_set_dont_overwrite;
	printf "$FILE_PART_READ_CMD 0x%x \$(UpgradeImage) 0x%x 0x%x\n" $DRAM_BUF_ADDR $SCRIPT_SIZE $INPUT_IMG_SIZE >> $SCRIPT_FILE
	if [ "$FULL_ERASE" == "1" ] ; then
	   printf "mmc erase.p UBOOT\n" >> $SCRIPT_FILE
	fi  
	printf "mmc write.p 0x%x UBOOT 0x%x\n" $DRAM_BUF_ADDR $INPUT_IMG_SIZE >> $SCRIPT_FILE
    #================= script content done ====================
	func_post_process;
}

function func_post_process(){
	echo "% <- this is end of file symbol" >> $SCRIPT_FILE
    func_pad_script; # padding to $SCRIPT_SIZE(0x4000), and Mboot.bin align 4byte.
    cat $SCRIPT_FILE >> $OUTPUT_IMG
    cat $INPUT_IMG >> $OUTPUT_IMG
    func_set_crc_to_image;
    func_copy_first_16byte;
    cp $OUTPUT_IMG ../out/
}

function func_prompt(){
    #================prompt question==========================
printf "\033[0;31mIs u-boot-2011.06 ? (Y/n)\033[0m"
read temp
if [ "$temp" == "N" ] || [ "$temp" == "n" ]; then
    NEW_MBOOT=0
else
	printf "\033[0;31mIs ARM platform (y/N)\033[0m"
	read temp	
	if [ "$temp" == "N" ] || [ "$temp" == "n" ]; then
		CPU_IS_ARM=0
	fi	
	
	printf "\033[0;31mBurn in Emmc ? (y/N)\033[0m"
	read temp	
	if [ "$temp" == "Y" ] || [ "$temp" == "y" ]; then
		FLASH_IS_EMMC=1
	fi	
fi

printf "\033[0;31mFully Erase ? (y/N)\033[0m"
read temp
if [ "$temp" == "Y" ] || [ "$temp" == "y" ]; then
    FULL_ERASE=1
fi
    #================prompt question end======================
}

function func_cmd_init(){
   #================cmd init==========================
if [ "$NEW_MBOOT" == "1" ] ; then
  CMD_SPI_WP="spi wp"
  CMD_SPI_EA="spi ea" 
  CMD_SPI_WRC="spi wrc"
else
  CMD_SPI_WP="spi_wp"
  CMD_SPI_EA="spi_ea" 
  CMD_SPI_WRC="spi_wrc"
fi 
  #================cmd init end======================= 
}

#=========== main() Start Upgrade ===============
func_prompt;
func_cmd_init;
if [ "$FLASH_IS_EMMC" == "1" ] ; then
	test -f ../out/mboot_emmc.bin
	if [ $? == 0 ];then
		echo "mv ../out/mboot_emmc.bin" $INPUT_IMG
		mv ../out/mboot_emmc.bin $INPUT_IMG
	fi 
	func_update_sboot_emmc;
	echo "Build "$OUTPUT_IMG" Successful !!"
	func_update_mboot_emmc;
else
	func_update_mboot_spi;
fi
echo "Build "$OUTPUT_IMG" Successful !!"
exit 0
#================= End Upgrade ==================

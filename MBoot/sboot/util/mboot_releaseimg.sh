#!/bin/bash


if [ "$1" == "901" ]; then
	PLATFORM=6a901
else
	if [ "$1" == "6369" ]; then
		PLATFORM=msd6369
	else
		echo "./releaseimg.sh 901 (or 6369)"
		exit
	fi
fi

TARGET_DIR=../../../../common/images/$PLATFORM/mstaredison


TMP_IMG_FOLDER=../out
MBOOT_BIN=mboot.bin
MBOOTUPGRADE_BIN=MbootUpgrade.bin


cp -f $TMP_IMG_FOLDER/$MBOOT_BIN $TARGET_DIR/
cp -f $TMP_IMG_FOLDER/$MBOOTUPGRADE_BIN $TARGET_DIR/



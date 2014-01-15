#!/bin/bash

test -d ../u-boot-2011.06/include/kk
if [ $? != 0 ]
then
	mkdir  ../u-boot-2011.06/include/kk
	echo "mkdir ../u-boot-2011.06/include/kk"
fi

test -d ../u-boot-2011.06/include/kk/app
if [ $? != 0 ]
then
	mkdir  ../u-boot-2011.06/include/kk/app
	echo "mkdir ../u-boot-2011.06/include/kk/app"
fi


# link KkApp
ln -fs ../../../../KkApp/include/ ../u-boot-2011.06/include/kk/app/common
ln -fs ../../../../KkApp/board/MST901/include ../u-boot-2011.06/include/kk/app/board

echo "-- kkapp link OK --"
exit 1

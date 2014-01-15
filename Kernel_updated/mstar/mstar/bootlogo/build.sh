#!/bin/csh

if ($#argv < 1) then
echo ""
echo "## Build FastLogo ## "
echo "X5 : $0 X6"
echo "X5_PM51 : $0 X5_PM51"
echo "X5_VOD : $0 X5_VOD"
echo "X6 : $0 X6"
echo "X9 : $0 X9"
echo "X10 : $0 X10"
echo "clean : $0 clean"
echo ""
exit 1
endif

# clean
if ($argv[1] == "clean") then
make -e clean -C $PWD -f Makefile
exit 1
endif

# arg 처리 
if ($#argv == 1) then
set BUILD_DIR=$PWD
else
set BUILD_DIR=$argv[2]
endif

# X5_VOD, X5_PM51가 들어오면 X5 로 path를 설정
set ARG1=$argv[1]
if ($argv[1] == "X5_PM51") then
set LIB_DIR=$BUILD_DIR/../../LIB/X5
else
if ($argv[1] == "X5_VOD") then
set LIB_DIR=$BUILD_DIR/../../LIB/X5
else
set LIB_DIR=$BUILD_DIR/../../LIB/$ARG1
endif
endif

echo "### Build $ARG1 FastLogo modules ###"
echo "# Build Dir : $BUILD_DIR #"
echo "# PWD : $PWD #"

cp -f $BUILD_DIR/Makefile $BUILD_DIR/Makefile_org << End
n
End
rm -f $BUILD_DIR/Makefile;cp -f $BUILD_DIR/Makefile.$ARG1 $BUILD_DIR/Makefile;
make -e clean -C $BUILD_DIR -f Makefile
make -e all -C $BUILD_DIR -f Makefile

if ($argv[1] == "X5_PM51") then
cp -f $BUILD_DIR/FastLogo.ko $LIB_DIR/FastLogo.ko_PM51 << End
y
End

else

if ($argv[1] == "X5_VOD") then
cp -f $BUILD_DIR/FastLogo.ko $LIB_DIR/FastLogo.ko_VOD << End
y
End

else
cp -f $BUILD_DIR/FastLogo.ko $LIB_DIR/FastLogo.ko << End
y
End
endif
endif

rm -f $BUILD_DIR/Makefile;mv $BUILD_DIR/Makefile_org $BUILD_DIR/Makefile

echo ""
echo "### Done. Build $ARG1 FastLogo ###"
echo ""

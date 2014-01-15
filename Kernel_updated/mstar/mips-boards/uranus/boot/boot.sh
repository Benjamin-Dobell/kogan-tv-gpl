
mkdir -p obj
mkdir -p out


CC=mips2_fp_le-gcc
AS=mips2_fp_le-as
LD=mips2_fp_le-ld
OBJCOPY=mips2_fp_le-objcopy
OBJDUMP=mips2_fp_le-objdump
SIZE=mips2_fp_le-size


# ROM layout:
#   BOOT_BIN (boot + bootram)
#   UNZIP_BIN
#   ZIP_BIN

cd unzip
./unzip.sh
cd ..
cp unzip/out/unzip.bin out
cp ../../../../../vmlinux out
cp ../../../../../System.map out
$OBJCOPY -O binary out/vmlinux out/uranus.bin

IN_LINUX_ENV=`uname`
if [ "$IN_LINUX_ENV" == "Linux" ]; then
    ../../boot_utils/linux_lzma e out/uranus.bin out/uranus.zip -lc0 -lp2  # SUPPORT_UNZIP
else
    ../../boot_utils/lzma.exe e out/uranus.bin out/uranus.zip -lc0 -lp2  # SUPPORT_UNZIP
fi

# input
UNZIP_BIN=out/unzip.bin
ZIP_BIN=out/uranus.zip
PAD_BIN=pad00.bin
# output
BOOT1_INC=out/boot1.inc
BOOT_BIN=out/boot.bin
ALL_BIN=out/all.bin
FLASH_BIN=out/flash.bin

rm  -f $BOOT1_INC
rm  -f $BOOT_BIN
rm  -f $ALL_BIN


##### Pad & Calculate Sizes #####
SIZE_UNZIP=`/bin/ls -l $UNZIP_BIN | /bin/awk '{ print $5 }'`
case `expr $SIZE_UNZIP % 4` in
    [1])
	/bin/cat $PAD_BIN >> $UNZIP_BIN
	/bin/cat $PAD_BIN >> $UNZIP_BIN
	/bin/cat $PAD_BIN >> $UNZIP_BIN
	;;
	[2])
	/bin/cat $PAD_BIN >> $UNZIP_BIN
	/bin/cat $PAD_BIN >> $UNZIP_BIN
	;;
	[3])
	/bin/cat $PAD_BIN >> $UNZIP_BIN
	;;
esac


SIZE_ZIPBIN=`/bin/ls -l $ZIP_BIN | /bin/awk '{ print $5 }'`
case `expr $SIZE_ZIPBIN % 4` in
    [1])
	/bin/cat $PAD_BIN >> $ZIP_BIN
	/bin/cat $PAD_BIN >> $ZIP_BIN
	/bin/cat $PAD_BIN >> $ZIP_BIN
	;;
	[2])
	/bin/cat $PAD_BIN >> $ZIP_BIN
	/bin/cat $PAD_BIN >> $ZIP_BIN
	;;
	[3])
	/bin/cat $PAD_BIN >> $ZIP_BIN
	;;
esac

printf "#This file is auto created. Do not modify!\n" >> $BOOT1_INC

SIZE_UNZIP=`/bin/ls -l $UNZIP_BIN | /bin/awk '{ print $5 }'`
gawk '$3 == "kernel_entry" { print "KERNEL_ENTRY_ADDR = 0x"$1 }' out/System.map >> $BOOT1_INC
printf "SIZE_UNZIP = 0X" >> $BOOT1_INC
printf %08X $SIZE_UNZIP  >> $BOOT1_INC
printf '\n' >> $BOOT1_INC

SIZE_ZIPBIN=`/bin/ls -l $ZIP_BIN | /bin/awk '{ print $5 }'`
printf "SIZE_ZIPBIN = 0X" >> $BOOT1_INC
printf %08X $SIZE_ZIPBIN  >> $BOOT1_INC
printf '\n' >> $BOOT1_INC


##### Compile/Link #####
$AS -EL -mips32 -gdwarf2 -O2 -G0 -Isrc -Iout -o obj/boot.o src/boot.s
$AS -EL -mips32 -gdwarf2 -O2 -G0 -Isrc -Iout -o obj/bootrom.o src/bootrom.s
$AS -EL -mips32 -gdwarf2 -O2 -G0 -Isrc -Iout -o obj/bootram.o src/bootram.s

$LD  -EL -o out/boot.elf -Map out/boot.map -T link.ld
$OBJCOPY -O binary -I elf32-little out/boot.elf out/boot.bin
$OBJDUMP -S out/boot.elf > out/boot.dis
$SIZE out/boot.elf > out/boot.siz


##### Concatenate all binaries #####
/bin/cat $BOOT_BIN > $ALL_BIN
/bin/cat $UNZIP_BIN >> $ALL_BIN
/bin/cat $ZIP_BIN >> $ALL_BIN


##### Swap if necessary #####
perl byteswap.pl $ALL_BIN $FLASH_BIN

cp out/flash.bin ../../../../../




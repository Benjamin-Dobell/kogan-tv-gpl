
mkdir -p obj
mkdir -p out



CC=mips2_fp_le-gcc
AS=mips2_fp_le-as
LD=mips2_fp_le-ld
OBJCOPY=mips2_fp_le-objcopy
OBJDUMP=mips2_fp_le-objdump
SIZE=mips2_fp_le-size


##### Compile/Link #####
#$CC  -mips32 -gdwarf-2 -O2 -c -G0 -I. LzmaDec.c -o obj/lzmadec.o
#$CC  -mips32 -gdwarf-2 -O2 -c -G0 -I. LzmaDecode.c -o obj/lzmadecode.o
$CC   -G 0 -mno-abicalls -fno-pic -pipe -mips32   -c  -I.   lzmadec.c -o obj/lzmadec.o
$CC   -G 0 -mno-abicalls -fno-pic -pipe -mips32  -c  -I.   lzmadecode.c -o obj/lzmadecode.o
$LD -EL -o out/unzip.elf obj/lzmadec.o obj/lzmadecode.o -Map out/unzip.map -T link.ld
$OBJCOPY -O binary out/unzip.elf out/unzip.bin
$OBJDUMP -S -D out/unzip.elf > out/unzip.dis
$SIZE out/unzip.elf > out/unzip.siz

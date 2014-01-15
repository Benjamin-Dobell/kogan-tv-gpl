#
# Makefile for MIPS No OS test code
#

#mips-linux-gnu, mipsisa32-elf, mips2_fp_le, mips-sde-elf
CROSS_COMPILEER=mips-sde-elf

CC=$(CROSS_COMPILEER)-gcc
AS=$(CROSS_COMPILEER)-as
LD=$(CROSS_COMPILEER)-ld
OBJCOPY=$(CROSS_COMPILEER)-objcopy
OBJDUMP=$(CROSS_COMPILEER)-objdump
SIZE=$(CROSS_COMPILEER)-size
#CFLAG= -EL -mips32 -G0 -c -g -mno-abicalls -fno-pic
#CFLAG= -EL -mips32 -mno-abicalls -O0 -G0 -c -g -fno-pic
CFLAG= -EL -mips32 -mno-abicalls -O0 -G0 -c -g -fno-pic -pipe -Werror -Wall -W -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Wno-format -fno-exceptions -ffunction-sections
#-pipe -Werror -Wall -W -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -Wno-format -fno-exceptions -ffunction-sections
.PHONY : all clean

all: vector.o mips34k_cache_init.o
	$(LD) -L./lib -static -o mipsnoos.elf vector.o mips34k_cache_init.o -T./main.ld
	$(OBJDUMP) -S -D mipsnoos.elf > mipsnoos.dis
	cp -f mipsnoos.elf Uranus.elf
	$(OBJCOPY) -O binary mipsnoos.elf Uranus.bin

main.o: main.c
	$(CC) $(CFLAG) main.c

vector.o: vector.S system.inc
	./get_kernel_entry.sh
	$(CC) $(CFLAG) vector.S

mips34k_cache_init.o: mips34k_cache_init.S
	$(CC)  $(CFLAG) mips34k_cache_init.S

clean:
	rm *.o
	rm *.elf
	rm *.dis
	rm *.bin
	rm ice.h


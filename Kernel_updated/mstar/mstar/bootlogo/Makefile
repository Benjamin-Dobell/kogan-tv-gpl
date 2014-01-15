#
# Makefile for Fast Logo drivers.
#
BUILD_DIR = $(PWD)
#MODPOST_DIR_X10 ?= $(BUILD_DIR)/../../BSP/X10/source/kernel-src/2.6.35.11
MODPOST_DIR_X10 ?= ../../x10_kernel/linux-2.6.35.11
MODPOST_DIR_X10P ?= ../../x10p_kernel/linux-2.6.35.11
SN_PATH ?= ../../../2.6.35.8

#PRJNAME := PRJ_X10
#PRJNAME := PRJ_X10P
#PRJNAME := PRJ_X10P
PRJNAME := SN

ifeq ($(PRJNAME), PRJ_X10)
MODPOST_DIR = $(MODPOST_DIR_X10)
CROSS_COMPILE ?= mipsel-74kv2r2-linux-gnu-
PLATFORM_CPPFLAGS += -DCONFIG_MIPS -D__MIPS__ -D$(PRJNAME)
else ifeq ($(PRJNAME), PRJ_X10P)
MODPOST_DIR = $(MODPOST_DIR_X10P)
CROSS_COMPILE ?= arm-v7a8v2r2-linux-gnueabi-
PLATFORM_CPPFLAGS += -DCONFIG_ARM -D__ARM__ -D$(PRJNAME)
else
MODPOST_DIR = $(SN_PATH)
CROSS_COMPILE ?= arm-none-linux-gnueabi-
PLATFORM_CPPFLAGS += -DCONFIG_ARM -D__ARM__ -D$(PRJNAME)
endif


#
# Include the make variables (CC, etc...)
#
AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc -g
CPP	= $(CC) -E
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
LDR	= $(CROSS_COMPILE)ldr
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB	= $(CROSS_COMPILE)RANLIB

LOGO_MOD_NAME := FastLogo

EXTRA_CFLAGS      += $(PLATFORM_CPPFLAGS)
	#echo $(PLATFORM_CPPFLAGS) 
EXTRA_CFLAGS      +=  -Iinclude/linux
EXTRA_CFLAGS      +=  -Iarch/arm/include
EXTRA_CFLAGS      +=  -I$(src)/include
#EXTRA_CFLAGS      +=  -I$(src)/../../sdal6_4/Driver/conf
ifeq ($(PRJNAME), PRJ_X10)
EXTRA_CFLAGS      +=  -EL -march=mips32r2 -Wa,-mips32r2 -mdspr2 -fno-pic -O2 -G0 -g -Wall -fno-schedule-insns -fno-schedule-insns2 \
                      -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsigned-char -fshort-wchar -fstrict-overflow -mmemcpy
else
EXTRA_CFLAGS      +=  -Wa, -fno-pic -O2 -g -Wall -fno-schedule-insns -fno-schedule-insns2 \
                      -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsigned-char -fshort-wchar -fstrict-overflow
endif

# specific options
# files
obj-m             := $(LOGO_MOD_NAME).o
$(LOGO_MOD_NAME)-y := SdMisc.o
$(LOGO_MOD_NAME)-y += sec_logo_main.o
$(LOGO_MOD_NAME)-y += inverter.o
$(LOGO_MOD_NAME)-y += sec_panel.o
$(LOGO_MOD_NAME)-y += apiSWI2C.o
all:
	@$(MAKE) -C $(MODPOST_DIR) M=`pwd` modules
	@$(STRIP) --strip-unneeded $(LOGO_MOD_NAME).ko

clean:
	@$(MAKE) -C $(MODPOST_DIR) M=`pwd` clean

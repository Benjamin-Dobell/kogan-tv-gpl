#
# Makefile for MStar SD card device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

ifdef CONFIG_MSTAR_AMBER3
# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/sdmmc/inc_a3
EXTRA_CFLAGS      +=  -Idrivers/mstar/sdmmc/inc_a3/config

# specific options
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-y             += ms_sdmmc_lnx_a3.o 
obj-y	            += src_a3/hal_card_platform_a3.o
obj-y	            += src_a3/hal_card_regs.o
obj-y	            += src_a3/hal_card_timer.o
obj-y	            += src_a3/hal_card_intr.o
obj-y	            += src_a3/hal_sdmmc.o
else
# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/sdmmc/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/sdmmc/inc/config

# specific options
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-y             += ms_sdmmc_lnx.o 
obj-y	            += src/hal_card_platform.o
obj-y	            += src/hal_card_regs.o
obj-y	            += src/hal_card_timer.o
obj-y	            += src/hal_card_intr.o
obj-y	            += src/hal_sdmmc.o
endif


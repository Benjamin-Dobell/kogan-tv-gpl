#
# Makefile for MStar Video Prealloc Memory device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/vid_pre_alloc_mem
EXTRA_CFLAGS      +=  -Idrivers/mstar/vid_pre_alloc_mem/$(CONFIG_MSTAR_CHIP_NAME)

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_VPOOL)	+= mdrv-vpool.o
mdrv-vpool-objs			+= mdrv_vpool.o

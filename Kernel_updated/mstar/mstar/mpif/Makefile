#
# Makefile for MStar MPIIF device drivers.
#

ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/sys/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/mpif
EXTRA_CFLAGS      +=  -Idrivers/mstar/mpif/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
EXTRA_CFLAGS      +=  -DTITANIA
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX
EXTRA_CFLAGS      +=  -DENABLE_IRQ

# files
obj-$(CONFIG_MSTAR_MPIF)	+= mdrv-mpif.o
mdrv-mpif-objs			    += mdrv_mpif.o mdrv_mpif_io.o
mdrv-mpif-objs			    += $(CONFIG_MSTAR_CHIP_NAME)/mhal_mpif.o

# export header files
EXPORT_H_FILES    += mdrv_mpif_io.h

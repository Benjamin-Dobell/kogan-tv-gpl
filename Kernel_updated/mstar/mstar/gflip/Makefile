#
# Makefile for MStar GFLIP device drivers.
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/gflip/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/gflip/drv/$(CONFIG_MSTAR_CHIP_NAME)

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_GFLIP)	+= mdrv-gflip.o
mdrv-gflip-objs		+= mdrv_gflip_io.o drv/mdrv_gflip.o drv/mdrv_gflip_interrupt.o
mdrv-gflip-objs		+= drv/$(CONFIG_MSTAR_CHIP_NAME)/mhal_gflip.o


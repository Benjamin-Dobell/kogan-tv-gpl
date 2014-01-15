#
# Makefile for MStar TSP device drivers.
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/sc
EXTRA_CFLAGS      +=  -Idrivers/mstar/sc/$(CONFIG_MSTAR_CHIP_NAME)

# files
obj-$(CONFIG_MSTAR_SC)	+= mdrv-sc.o
mdrv-sc-objs			+= mdrv_sc.o
mdrv-sc-objs			+= mdrv_sc_io.o
mdrv-sc-objs			+= $(CONFIG_MSTAR_CHIP_NAME)/mhal_sc.o

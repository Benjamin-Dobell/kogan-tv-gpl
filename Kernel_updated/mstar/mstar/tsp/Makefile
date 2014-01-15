#
# Makefile for MStar TSP device drivers.
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/tsp/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/tsp/drv/$(CONFIG_MSTAR_CHIP_NAME)

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_TSP)	+= mdrv-tsp.o
mdrv-tsp-objs		+= mdrv_tsp_io.o drv/mdrv_tsp_interrupt.o
mdrv-tsp-objs		+= drv/$(CONFIG_MSTAR_CHIP_NAME)/mhal_tsp.o


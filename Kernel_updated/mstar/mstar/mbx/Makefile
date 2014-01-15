#
# Makefile for MStar MBX device drivers.
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/mbx/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/mbx/drv/hal/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_MBX)	+= mdrv-mbx.o
mdrv-mbx-objs		+= mdrv_mbx_io.o drv/mdrv_mbx.o drv/mdrv_mbx_msgpool.o
mdrv-mbx-objs		+= drv/hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_mbx.o
mdrv-mbx-objs		+= drv/hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_mbx_interrupt.o


#
# Makefile for MStar Malloc device drivers.
#

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/msystem
EXTRA_CFLAGS      +=  -Idrivers/mstar/iomap
EXTRA_CFLAGS      +=  -Idrivers/mstar/mpool
EXTRA_CFLAGS      +=  -Idrivers/mstar/system
EXTRA_CFLAGS      +=  -Idrivers/mstar/mbx
EXTRA_CFLAGS      +=  -Idrivers/mstar/mbx/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/mbx/drv/hal/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/gflip
EXTRA_CFLAGS      +=  -Idrivers/mstar/gflip/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/gflip/drv/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/iic
EXTRA_CFLAGS      +=  -Idrivers/mstar/gpio

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_MSYSTEM)	+= mdrv-msystem.o

mdrv-msystem-objs			+= mdrv_msystem.o
mdrv-msystem-objs			+= ../iomap/mdrv_iomap.o

ifndef CONFIG_MSTAR_MPOOL
ifndef CONFIG_MSTAR_MPOOL_MODULE
mdrv-msystem-objs			+= ../mpool/mdrv_mpool.o
endif
endif

mdrv-msystem-objs			+= ../mbx/mdrv_mbx_io.o
mdrv-msystem-objs			+= ../mbx/drv/mdrv_mbx.o
mdrv-msystem-objs			+= ../mbx/drv/mdrv_mbx_msgpool.o
mdrv-msystem-objs			+= ../mbx/drv/hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_mbx.o
mdrv-msystem-objs			+= ../mbx/drv/hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_mbx_interrupt.o
mdrv-msystem-objs			+= ../gflip/mdrv_gflip_io.o
mdrv-msystem-objs			+= ../gflip/drv/mdrv_gflip.o
mdrv-msystem-objs			+= ../gflip/drv/mdrv_gflip_interrupt.o
mdrv-msystem-objs			+= ../gflip/drv/$(CONFIG_MSTAR_CHIP_NAME)/mhal_gflip.o
mdrv-msystem-objs		    += ../system/mdrv_system.o
mdrv-msystem-objs		    += ../system/mdrv_system_io.o



#
# Makefile for MStar Malloc device drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/firmware
EXTRA_CFLAGS      +=  -Idrivers/mstar/firmware/$(CONFIG_MSTAR_CHIP_NAME)

# specific options
EXTRA_CFLAGS      +=  

# files
obj-$(CONFIG_MSTAR_FW)		+= mdrv_fw.o
mdrv-mpool-objs			+= mdrv_fw.o

#
# Makefile for MStar GFLIP DRV drivers.
#

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/gflip/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/gflip/drv/$(CONFIG_MSTAR_CHIP_NAME)

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-y             += mdrv_gflip.o
obj-y             += mdrv_gflip_interrupt.o

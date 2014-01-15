#
# Makefile for MStar NAND device drivers.
#

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/sys/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/sys/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/flash
EXTRA_CFLAGS      +=  -Idrivers/mstar/flash/eiffel

# specific options
EXTRA_CFLAGS      +=  -DRED_LION

# files
obj-$(CONFIG_MSTAR_SPI_FLASH)  += halSERFLASH.o

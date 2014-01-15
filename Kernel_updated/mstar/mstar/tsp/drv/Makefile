#
# Makefile for MStar TSP DRV drivers.
#

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/tsp/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/tsp/drv/$(CONFIG_MSTAR_CHIP_NAME)

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
#obj-y             += mdrv_tsp.o
obj-y             += mdrv_tsp_interrupt.o

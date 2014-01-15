#
# Makefile for MStar MBX HAL drivers.
#

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/mbx/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/mbx/drv/hal/$(CONFIG_MSTAR_CHIP_NAME)

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-y             += mhal_mbx.o mhal_mbx_interrupt.o

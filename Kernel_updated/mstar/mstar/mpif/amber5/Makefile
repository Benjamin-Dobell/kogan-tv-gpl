#
# Makefile for MStar MPIF HAL drivers.
#


ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif


CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
# EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/mpif
EXTRA_CFLAGS      +=  -Idrivers/mstar/mpif/$(CONFIG_MSTAR_CHIP_NAME)

# specific options

# files
#obj-$(CONFIG_MSTAR_MPIF)             += mhal_mpif.o

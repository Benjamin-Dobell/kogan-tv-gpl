#
# Makefile for MStar device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
#EXTRA_CFLAGS     +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/mstar_fb/gop/
EXTRA_CFLAGS      +=  -Idrivers/mstar/mstar_fb/gop/hal/$(CONFIG_MSTAR_CHIP_NAME)

# specific options
EXTRA_CFLAGS      +=  -DRED_LION

# files
obj-$(CONFIG_MSTAR_FB)       += mstar_fb.o 
obj-$(CONFIG_MSTAR_FB)       += gop/drvGOP.o 
obj-$(CONFIG_MSTAR_FB)       += gop/hal/$(CONFIG_MSTAR_CHIP_NAME)/halGOP.o 
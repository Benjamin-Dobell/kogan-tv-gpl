#
# Makefile for MStar ONENAND device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/onenand
#EXTRA_CFLAGS      +=  -Idrivers/mstar/onenand/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/onenand/$(CONFIG_MSTAR_ONENAND_IP)

# specific options
#EXTRA_CFLAGS      +=  -O0
EXTRA_CFLAGS      +=  -DTITANIA
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-y             += mdrv_onenand.o
#obj-y             += $(CONFIG_MSTAR_CHIP_NAME)/
obj-y             += $(CONFIG_MSTAR_ONENAND_IP)/

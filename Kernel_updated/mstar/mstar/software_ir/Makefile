#
# Makefile for the kernel IrDA char device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

CHIP    = titania

EXTRA_CFLAGS += -Idrivers/mstar/include
EXTRA_CFLAGS += -Idrivers/mstar/ir
EXTRA_CFLAGS += -Idrivers/mstar/ir/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS += -Iinclude/linux
EXTRA_CFLAGS += -Idrivers/mstar/software_ir


obj-$(CONFIG_MSTAR_IR) += mdrv-software-ir.o
mdrv-software-ir-objs		+= mdrv_software_ir.o
mdrv-software-ir-objs           += mdrv_software_ir_io.o

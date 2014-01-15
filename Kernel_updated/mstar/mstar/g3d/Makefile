#
# Makefile for MStar G3d device drivers.
#


ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif


CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
EXTRA_CFLAGS      +=  -Iinclude/linux
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/g3d
EXTRA_CFLAGS      +=  -Idrivers/mstar/g3d/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Iarch/mips/mips-boards/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Iarch/mips/mips-boards/$(CONFIG_MSTAR_CHIP_NAME)/board

# specific options
# EXTRA_CFLAGS      +=  -DTITANIA
# EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
#obj-$(CONFIG_MSTAR_g3d)             += mdrv_g3d.o
#obj-$(CONFIG_MSTAR_g3d)             += $(CONFIG_MSTAR_CHIP_NAME)/


obj-$(CONFIG_MSTAR_G3D)		+= mdrv-g3d.o
mdrv-g3d-objs 				+= mdrv_g3d.o
mdrv-g3d-objs	            += $(CONFIG_MSTAR_CHIP_NAME)/mhal_g3d.o
mdrv-g3d-objs	            += $(CONFIG_MSTAR_CHIP_NAME)/mhal_g3dkmd.o
mdrv-g3d-objs	            += $(CONFIG_MSTAR_CHIP_NAME)/mhal_g3dkmd_command_queue.o
mdrv-g3d-objs	            += $(CONFIG_MSTAR_CHIP_NAME)/mhal_g3dkmd_misc.o
mdrv-g3d-objs	            += $(CONFIG_MSTAR_CHIP_NAME)/mhal_g3d_mem.o
mdrv-g3d-objs	            += $(CONFIG_MSTAR_CHIP_NAME)/g3d1mem_malloc.o
mdrv-g3d-objs	            += $(CONFIG_MSTAR_CHIP_NAME)/g3dmem_malloc.o
mdrv-g3d-objs	            += $(CONFIG_MSTAR_CHIP_NAME)/g3dmem_sbrk.o

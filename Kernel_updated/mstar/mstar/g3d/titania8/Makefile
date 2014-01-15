#
# Makefile for MStar g3d device driver
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/g3d
EXTRA_CFLAGS      +=  -Idrivers/mstar/g3d/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Iarch/mips/mips-boards/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Iarch/mips/mips-boards/$(CONFIG_MSTAR_CHIP_NAME)/board


# specific options
# EXTRA_CFLAGS      +=  -DTITANIA
# EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-y             += mhal_g3d.o
obj-y             += mhal_g3dkmd.o
obj-y             += mhal_g3dkmd_command_queue.o
obj-y             += mhal_g3dkmd_misc.o
obj-y             += mhal_g3d_mem.o
obj-y             += g3d1mem_malloc.o
obj-y             += g3dmem_malloc.o
obj-y             += g3dmem_sbrk.o


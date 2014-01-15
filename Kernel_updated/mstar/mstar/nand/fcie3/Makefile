#
# Makefile for MStar NAND device drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/nand
EXTRA_CFLAGS      +=  -Idrivers/mstar/nand/fcie3/inc/api
EXTRA_CFLAGS      +=  -Idrivers/mstar/nand/fcie3/inc/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/nand/fcie3/inc/config

# specific options
EXTRA_CFLAGS      +=  -DRED_LION

# files
#obj-y		+= src/drvNand_platform.o
#obj-y		+= src/drvNand_Linux.o
#obj-y		+= src/drvNand_hal.o
#obj-y		+= src/drvNand.o

obj-y	+= src/config/drvNAND_platform.o
obj-y	+= src/common/drvNAND_utl.o
obj-y	+= src/common/drvNAND_hal.o
obj-y	+= src/common/drvNAND_ReadRetry.o

#
# Makefile for MStar eMMC host drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/mmc/host/eMMCDrv/inc/api/
EXTRA_CFLAGS      +=  -Idrivers/mstar/mmc/host/eMMCDrv/inc/common/
EXTRA_CFLAGS      +=  -Idrivers/mstar/mmc/host/eMMCDrv/inc/config/
EXTRA_CFLAGS += -Werror

# specific options
#EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
#ifdef CONFIG_MSTAR_AMBER3
#obj-y               += host/mstar_mci_a3.o 
#else
obj-y               += host/mstar_mci.o 
obj-y	            += host/eMMCDrv/src/api/eMMC_prg.o
obj-y	            += host/eMMCDrv/src/common/eMMC_hal.o
obj-y	            += host/eMMCDrv/src/common/eMMC_ip_verify.o
obj-y	            += host/eMMCDrv/src/common/eMMC_utl.o
obj-y	            += host/eMMCDrv/src/config/eMMC_platform.o
#endif


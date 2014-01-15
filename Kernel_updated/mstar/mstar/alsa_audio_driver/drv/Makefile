#
# Makefile for MStar ALSA audio device drivers - DRV Layer.
#

ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
EXTRA_CFLAGS += -Idrivers/mstar/alsa_audio_driver/drv

# specific options
EXTRA_CFLAGS += -DTITANIA
EXTRA_CFLAGS += -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MSTAR_ALSA_AUDIO_DRIVER_DRV) += mdrv-alsa-audio.o
mdrv-alsa-audio-objs += mdrv_audio.o

#
# Makefile for MStar ALSA audio device drivers - HAL Layer.
#

ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
EXTRA_CFLAGS += -Idrivers/mstar/alsa_audio_driver/drv
EXTRA_CFLAGS += -Idrivers/mstar/alsa_audio_driver/hal

# specific options
EXTRA_CFLAGS += -DTITANIA
EXTRA_CFLAGS += -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MSTAR_ALSA_AUDIO_DRIVER_HAL) += mhal-alsa-audio.o
mhal-alsa-audio-objs += $(CONFIG_MSTAR_CHIP_NAME)/mhal_audio.o

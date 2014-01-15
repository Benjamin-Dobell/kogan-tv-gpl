#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
include $(TOPDIR)/config.mk
include $(TOPDIR)/../sboot/.config

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
#CONFIG_MSTAR_CHIP_NAME := amber5
#CFLAGS += -DET_DEBUG -DDEBUG

LIB	= $(obj)libdrivers.a

COBJS =

#ifeq ($(ENABLE_POWER_MUSIC),y)
COBJS += ./audio/MsDrvAudio.o
COBJS += ./audio/Audio_amplifier.o
#endif

#ifeq ($(ENABLE_GOP),y)
COBJS += ./gop/MsDrvGop.o
#endif

#ifeq ($(ENABLE_JPD),y)
COBJS += ./jpd/MsDrvJpd.o
#endif

ifeq ($(CONFIG_MMC),y)
CFLAGS += -I./mmc/inc/common/
CFLAGS += -I./mmc/inc/api/
CFLAGS += -I./mmc/inc/config/
CPPFLAGS += -I./mmc/inc/common/
CPPFLAGS += -I./mmc/inc/api/
CPPFLAGS += -I./mmc/inc/config/

COBJS += ./mmc/src/common/mmc.o
COBJS += ./mmc/src/config/eMMC_platform.o
COBJS += ./mmc/src/common/eMMC_hal.o
COBJS += ./mmc/src/common/eMMC_ip_verify.o
COBJS += ./mmc/src/common/eMMC_utl.o
COBJS += ./mmc/src/api/eMMC_prg.o
endif

ifeq ($(CONFIG_SD),y)
CFLAGS += -I./sd/inc/common/
CFLAGS += -I./sd/inc/config/
CPPFLAGS += -I./sd/inc/common/
CPPFLAGS += -I./sd/inc/config/

COBJS += ./sd/src/config/drvFCIE_platform.o
COBJS += ./sd/src/common/drvFCIE.o
COBJS += ./sd/src/common/fcie_mmc.o
COBJS += ./sd/src/common/mmc.o
endif

ifeq ($(CONFIG_NAND),y)
COBJS += ./unfd/src/config/drvNAND_platform.o
COBJS += ./unfd/src/common/drvNAND_hal.o
COBJS += ./unfd/src/common/drvNAND_ReadRetry.o
COBJS += ./unfd/nand.o
COBJS += ./unfd/src/common/drvNAND_utl.o
endif

ifeq ($(CONFIG_EMAC),y)
COBJS += ./emac/$(CONFIG_MSTAR_CHIP_NAME)/mhal_emac.o
COBJS += ./emac/mdrv_emac.o
endif

ifeq ($(CONFIG_KEYPAD),y)
COBJS += ./keypad/msKeypad.o
endif

ifeq ($(CONFIG_IR),y)
COBJS += ./ir/$(CONFIG_MSTAR_CHIP_NAME)/msIR.o
endif

ifeq ($(CONFIG_MSTARI2C),y)
COBJS += ./i2c/msI2C.o
endif

ifeq ($(CONFIG_PANEL_INIT),y)
COBJS += ./panel/panel.o
COBJS += ./panel/MsDrvPanel.o
endif

ifeq ($(CONFIG_URSA_6M30),y)
COBJS += ./ursa/ursa_6m30.o
endif

ifeq ($(CONFIG_URSA_6M40),y)
COBJS += ./ursa/ursa_6m40.o
endif

ifeq ($(CONFIG_USB),y)
COBJS += ./usb/usbdcore.o
COBJS += ./usb/usbtty.o
COBJS += ./usb/drvusbmain.o
COBJS += ./usb/drvhostlib.o
COBJS += ./usb/drvusbmain2.o
COBJS += ./usb/drvhostlib2.o
COBJS += ./usb/drvusbmain3.o
COBJS += ./usb/drvhostlib3.o
COBJS += ./usb/ehci_usb.o
COBJS += ./usb/usb_lan.o
COBJS += ./usb/ax88772a.o
COBJS += ./usb/rtl8150.o
COBJS += ./usb/pegasus.o
endif
ifeq ($(CONFIG_HDMI_TX),y)
ifeq ($(CONFIG_HDMITX_NXP_TDA19988),y)
COBJS +=  ./hdmitx/sde2/inc/M_supply.o
COBJS +=  ./hdmitx/sde2/comps/tmbsltda9989/src/tmbslTDA9989_Edid.o
COBJS +=  ./hdmitx/sde2/comps/tmbsltda9989/src/tmbslTDA9989_HDCP.o
COBJS +=  ./hdmitx/sde2/comps/tmbsltda9989/src/tmbslTDA9989_InOut.o
COBJS +=  ./hdmitx/sde2/comps/tmbsltda9989/src/tmbslTDA9989_local.o    
COBJS +=  ./hdmitx/sde2/comps/tmbsltda9989/src/tmbslTDA9989_Misc.o
COBJS +=  ./hdmitx/sde2/comps/tmbsltda9989/src/tmbslTDA9989_State.o    
COBJS +=  ./hdmitx/sde2/comps/tmdlhdmicec/src/tmdlHdmiCEC.o
COBJS +=  ./hdmitx/sde2/comps/tmdlhdmicec/src/tmdlHdmiCEC_local.o
COBJS +=  ./hdmitx/sde2/comps/tmdlhdmitx/cfg/TDA9989/tmdlHdmiTx_cfg.o
COBJS +=  ./hdmitx/sde2/comps/tmdlhdmitx/src/tmdlHdmiTx.o
COBJS +=  ./hdmitx/sde2/comps/tmdlhdmitx/src/tmdlHdmiTx_local.o
COBJS +=  ./hdmitx/sde2/comps/tmdlhdmitx/tst/tmdlHdmiTx_ExampleApplication/src/tmdlHdmiTx_ExampleApplicationArm7_NO_RTOS.o
COBJS +=  ./hdmitx/sde2/comps/tmdlhdmitx/tst/tmdlHdmiTx_ExampleApplication/src/tmdlHdmiTx_IW_RTX.o
endif
endif
COBJS += ./MDrv_Sys.o
COBJS += ./miu/MsDrvMiu.o


CFLAGS += -I./emac/$(CONFIG_MSTAR_CHIP_NAME)
CPPFLAGS += -I./emac/$(CONFIG_MSTAR_CHIP_NAME)

all:	$(LIB) $(AOBJS)

SRCS	:= $(AOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(AOBJS) $(COBJS))

all:	$(LIB)

$(LIB): $(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

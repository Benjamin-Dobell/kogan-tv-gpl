#
# (C) Copyright 2004-2006
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

LIB	=  $(obj)libmscore.a

#AOBJS	=

COBJS	=
#MstarCore:src
COBJS += ./src/api/MsInit.o
COBJS += ./src/api/apiIR.o
COBJS += ./src/api/msAPI_Power.o
COBJS += ./src/api/MsTemplate.o
COBJS += ./src/api/MsSystem.o
COBJS += ./src/api/MsCoreRegister.o
#ifeq ($(ENABLE_POWER_MUSIC),y)
COBJS += ./src/api/MsApiAudio.o
#endif
COBJS += ./src/api/MsApiPanel.o
COBJS += ./src/api/MsApiGEGOP.o
COBJS += ./src/api/MsApiJpd.o
COBJS += ./src/api/MsApiMiu.o
COBJS += ./src/api/MsApiPM.o
COBJS += ./src/api/MsApiSpi.o
COBJS += ./src/api/MsMemPerformance.o
ifdef CONFIG_CMD_MMC
COBJS += ./src/api/MsMmc.o
endif

#MstarCore:cmd
ifeq ($(CONFIG_EMAC),y)
COBJS += ./cmd/cmd_emac.o
endif
COBJS += ./cmd/cmd_spi.o
COBJS += ./cmd/cmd_MsMemPerformance.o
COBJS += ./cmd/cmd_MsTemplate.o
COBJS += ./cmd/cmd_MsSystem.o
COBJS += ./cmd/cmd_MsPanel.o
COBJS += ./cmd/cmd_MsMiu.o
ifdef CONFIG_CMD_MMC
COBJS += ./cmd/cmd_MsMmc.o
endif


SRCS	:= $(AOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(AOBJS) $(COBJS))

#CPPFLAGS += -I..

all:	$(LIB)

$(LIB): $(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

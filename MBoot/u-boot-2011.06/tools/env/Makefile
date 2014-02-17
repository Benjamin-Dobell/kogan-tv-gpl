#
# (C) Copyright 2002-2006
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

HOSTSRCS := $(obj)crc32.c  fw_env.c  fw_env_main.c
HEADERS	:= fw_env.h

# Compile for a hosted environment on the target
HOSTCPPFLAGS  = -idirafter $(SRCTREE)/include \
		-idirafter $(OBJTREE)/include2 \
		-idirafter $(OBJTREE)/include \
		-DUSE_HOSTCC

ifeq ($(MTD_VERSION),old)
HOSTCPPFLAGS += -DMTD_OLD
endif

# macro to check whether a host header is present; outputs 1 if present, 0 if
# not
check_include = $(shell echo | $(HOSTCC) $(HOSTCFLAGS_NOPED) -E -include $(1) -o /dev/null - 2>/dev/null && echo 1 || echo 0)
 
# whether to build tools/env or not 
has_mtd_user := $(call check_include, mtd/mtd-user.h)

ifeq ($(has_mtd_user),1)
all:	$(obj)fw_printenv
else
all: 
endif

# Some files complain if compiled with -pedantic, use HOSTCFLAGS_NOPED
$(obj)fw_printenv:	$(HOSTSRCS) $(HEADERS)
	$(HOSTCC) $(HOSTCFLAGS_NOPED) $(HOSTLDFLAGS) -o $@ $(HOSTSRCS)

clean:
	rm -f $(obj)fw_printenv $(obj)crc32.c

$(obj)crc32.c:
	ln -s $(src)../../lib/crc32.c $(obj)crc32.c

#########################################################################

include $(TOPDIR)/rules.mk

sinclude $(obj).depend

#########################################################################

/*
 * drivers/video/tegra/host/mpe/mpe.h
 *
 * Tegra Graphics Host MPE
 *
 * Copyright (c) 2011-2012, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __NVHOST_MPE_MPE_H
#define __NVHOST_MPE_MPE_H

struct nvhost_hwctx_handler;
struct nvhost_device;

struct nvhost_hwctx_handler *nvhost_mpe_ctxhandler_init(
		u32 syncpt, u32 waitbase,
		struct nvhost_channel *ch);
int nvhost_mpe_prepare_power_off(struct nvhost_device *dev);

#endif

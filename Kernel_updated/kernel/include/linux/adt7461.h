/*
 * include/linux/adt8461.h
 *
 * ADT7461, temperature monitoring device from ON Semiconductors
 *
 * Copyright (c) 2011, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _LINUX_ADT7461_H
#define _LINUX_ADT7461_H

#include <linux/types.h>

struct adt7461_platform_data {
	bool supported_hwrev;
	bool ext_range;
	bool therm2;
	u8 conv_rate;
	u8 offset;
	u8 hysteresis;
	u8 shutdown_ext_limit;
	u8 shutdown_local_limit;
	u8 throttling_ext_limit;
	void (*alarm_fn)(bool raised);
	int irq_gpio;
};

#endif /* _LINUX_ADT7461_H */

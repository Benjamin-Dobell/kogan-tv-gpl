/* changhong-tv.h - Keytable for changhong_tv Remote Controller
 *
 * keymap imported from ir-keymaps.c
 *
 * Copyright (c) 2010 by Mauro Carvalho Chehab <mchehab@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <media/rc-core.h>

/*
 * Jimmy Hsu <jimmy.hsu@mstarsemi.com>
 * this is the remote control that comes with the changhong smart tv
 * which based on STAOS standard.
 */

static struct rc_map_table changhong_tv[] = {
    // 1st IR controller.
    { 0x0012, KEY_POWER },
    { 0x0000, KEY_0 },
    { 0x0001, KEY_1 },
    { 0x0002, KEY_2 },
    { 0x0003, KEY_3 },
    { 0x0004, KEY_4 },
    { 0x0005, KEY_5 },
    { 0x0006, KEY_6 },
    { 0x0007, KEY_7 },
    { 0x0008, KEY_8 },
    { 0x0009, KEY_9 },
    { 0x004A, KEY_RED },
    { 0x004B, KEY_GREEN },
    { 0x004C, KEY_YELLOW },
    { 0x004D, KEY_BLUE },
    { 0x0019, KEY_UP },
    { 0x001D, KEY_DOWN },
    { 0x0046, KEY_LEFT },
    { 0x0047, KEY_RIGHT },
    { 0x000A, KEY_ENTER },
    { 0x001B, KEY_CHANNELUP },
    { 0x001F, KEY_CHANNELDOWN },
    { 0x001A, KEY_VOLUMEUP },
    { 0x001E, KEY_VOLUMEDOWN },
    { 0x0051, KEY_HOME},
    { 0x005B, KEY_MENU },
    { 0x0040, KEY_BACK },
    { 0x0010, KEY_MUTE },
    { 0x0016, KEY_INFO },
    { 0x00C5, KEY_TV },
    { 0x0050, KEY_DELETE },
    { 0x005C, KEY_KP0 },    // TV_INPUT
    { 0x0067, KEY_KP1 },    // 3D_MODE
    { 0x000B, KEY_EPG },
    { 0x0055, KEY_F1 },     // CHANGHONGIR_HELP
    { 0x005E, KEY_F2 },     // CHANGHONGIR_APP
    { 0x0041, KEY_F3 },     // CHANGHONGIR_SPREAD
    { 0x000F, KEY_F4 },     // CHANGHONGIR_PINCH
    { 0x004F, KEY_F5 },     // CHANGHONGIR_FLCK_SL
    { 0x004E, KEY_F6 },     // CHANGHONGIR_FLCK_SR
    { 0x0053, KEY_F7 },     // CHANGHONGIR_INPUT
    { 0x005B, KEY_F8 },     // CHANGHONGIR_BARCODE

    // 2nd IR controller.
};

static struct rc_map_list changhong_tv_map = {
	.map = {
		.scan    = changhong_tv,
		.size    = ARRAY_SIZE(changhong_tv),
		.rc_type = RC_TYPE_UNKNOWN,	/* Legacy IR type */
		.name    = RC_MAP_CHANGHONG_TV,
	}
};

static int __init init_rc_map_changhong_tv(void)
{
	return rc_map_register(&changhong_tv_map);
}

static void __exit exit_rc_map_changhong_tv(void)
{
	rc_map_unregister(&changhong_tv_map);
}

module_init(init_rc_map_changhong_tv)
module_exit(exit_rc_map_changhong_tv)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");

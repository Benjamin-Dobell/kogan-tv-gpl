/* haier-tv.h - Keytable for haier_tv Remote Controller
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
 * this is the remote control that comes with the haier smart tv
 * which based on STAOS standard.
 */

static struct rc_map_table haier_tv[] = {
    // 1st IR controller.
    { 0x000B, KEY_POWER },
    { 0x0009, KEY_0 },
    { 0x0000, KEY_1 },
    { 0x0001, KEY_2 },
    { 0x0002, KEY_3 },
    { 0x0003, KEY_4 },
    { 0x0004, KEY_5 },
    { 0x0005, KEY_6 },
    { 0x0006, KEY_7 },
    { 0x0007, KEY_8 },
    { 0x0008, KEY_9 },
    { 0x005E, KEY_RED },
    { 0x005F, KEY_GREEN },
    { 0x0061, KEY_YELLOW },
    { 0x0062, KEY_BLUE },
    { 0x0064, KEY_UP },
    { 0x0066, KEY_DOWN },
    { 0x0065, KEY_LEFT },
    { 0x0067, KEY_RIGHT },
    { 0x0068, KEY_ENTER },
    { 0x0019, KEY_CHANNELUP },
    { 0x0018, KEY_CHANNELDOWN },
    { 0x001B, KEY_VOLUMEUP },
    { 0x001A, KEY_VOLUMEDOWN },
    { 0x0070, KEY_PAGEUP },
    { 0x0074, KEY_PAGEDOWN },
    { 0x0063, KEY_HOME},
    { 0x001C, KEY_MENU },
    { 0x0069, KEY_BACK },
    { 0x000C, KEY_MUTE },
    { 0x0020, KEY_INFO },
    { 0x0037, KEY_KP0 },        // WINDOW
    { 0x0015, KEY_KP1 },        // TV_INPUT
    { 0x008A, KEY_KP2 },        // 3D_MODE
    { 0x006A, KEY_REWIND },
    { 0x006B, KEY_FORWARD },
    { 0x006C, KEY_PLAYPAUSE },
    { 0x0012, KEY_AUDIO },      // (C)SOUND_MODE
    { 0x000D, KEY_CAMERA },     // (C)PICTURE_MODE
    { 0x0013, KEY_ZOOM },       // (C)ASPECT_RATIO
    { 0x000A, KEY_CHANNEL },    // (C)CHANNEL_RETURN
    { 0x0017, KEY_SLEEP },      // (C)SLEEP
    { 0x005B, KEY_EPG },        // (C)EPG
    { 0x007C, KEY_LIST },       // (C)LIST
    { 0x000F, KEY_FN_F1 },      // (C)SCREENSHOT
    { 0x008C, KEY_FN_F2 },      // (C)CLOUD
    { 0x001F, KEY_FN_F3 },      // (C)USB
    { 0x0090, KEY_F1 },         // HAIER_TASK
    { 0x008B, KEY_F2 },         // HAIER_TOOLS
    { 0x00DA, KEY_F3 },         // HAIER_POWERSLEEP
    { 0x00DB, KEY_F4 },         // HAIER_WAKEUP
    { 0x00DC, KEY_F5 },         // HAIER_UNMUTE
    { 0x00DD, KEY_F6 },         // HAIER_CLEANSEARCH

    // 2nd IR controller.
};

static struct rc_map_list haier_tv_map = {
	.map = {
		.scan    = haier_tv,
		.size    = ARRAY_SIZE(haier_tv),
		.rc_type = RC_TYPE_UNKNOWN,	/* Legacy IR type */
		.name    = RC_MAP_HAIER_TV,
	}
};

static int __init init_rc_map_haier_tv(void)
{
	return rc_map_register(&haier_tv_map);
}

static void __exit exit_rc_map_haier_tv(void)
{
	rc_map_unregister(&haier_tv_map);
}

module_init(init_rc_map_haier_tv)
module_exit(exit_rc_map_haier_tv)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");

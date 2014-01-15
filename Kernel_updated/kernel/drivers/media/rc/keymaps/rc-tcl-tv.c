/* tcl-tv.h - Keytable for tcl_tv Remote Controller
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
 * this is the remote control that comes with the tcl smart tv
 * which based on STAOS standard.
 */

static struct rc_map_table tcl_tv[] = {
    // 1st IR controller.
    { 0xF0D5, KEY_POWER },
    { 0xF0CF, KEY_0 },
    { 0xF0CE, KEY_1 },
    { 0xF0CD, KEY_2 },
    { 0xF0CC, KEY_3 },
    { 0xF0CB, KEY_4 },
    { 0xF0CA, KEY_5 },
    { 0xF0C9, KEY_6 },
    { 0xF0C8, KEY_7 },
    { 0xF0C7, KEY_8 },
    { 0xF0C6, KEY_9 },
    { 0xF0FF, KEY_RED },
    { 0xF017, KEY_GREEN },
    { 0xF01B, KEY_YELLOW },
    { 0xF027, KEY_BLUE },
    { 0xF0A6, KEY_UP },
    { 0xF0A7, KEY_DOWN },
    { 0xF0A9, KEY_LEFT },
    { 0xF0A8, KEY_RIGHT },
    { 0xF00B, KEY_ENTER },
    { 0xF0D2, KEY_CHANNELUP },
    { 0xF0D3, KEY_CHANNELDOWN },
    { 0xF0D0, KEY_VOLUMEUP },
    { 0xF0D1, KEY_VOLUMEDOWN },
    { 0xF0F7, KEY_HOME },
    { 0xF0F9, KEY_BACK },
    { 0xF0C0, KEY_MUTE },
    { 0xF0C3, KEY_INFO },
    { 0xF05C, KEY_KP0 },        // TV_INPUT
    { 0xF067, KEY_KP1 },        // 3D_MODE
    { 0xF0A5, KEY_AUDIO },      // (C)SOUND_MODE
    { 0xF0ED, KEY_CAMERA },     // (C)PICTURE_MODE
    { 0xF06F, KEY_ZOOM },       // (C)ASPECT RATIO
    { 0xF0D8, KEY_CHANNEL },    // (C)CHANNEL_RETURN
    { 0xF0F5, KEY_EPG },        // (C)EPG
    { 0xF0F3, KEY_FN_F1 },      // (C)FREEZE
    { 0xF0FD, KEY_FN_F2 },      // (C)USB
    { 0xF09E, KEY_F1 },         // TCL_MITV
    { 0xF062, KEY_F2 },         // TCL_USB_MENU
    { 0xF055, KEY_F3 },         // TCL_SWING_R1
    { 0xF056, KEY_F4 },         // TCL_SWING_R2
    { 0xF057, KEY_F5 },         // TCL_SWING_R3
    { 0xF058, KEY_F6 },         // TCL_SWING_R4
    { 0xF0FA, KEY_F7 },         // TCL_SWING_L1
    { 0xF034, KEY_F8 },         // TCL_SWING_L2
    { 0xF018, KEY_F9 },         // TCL_SWING_L3
    { 0xF087, KEY_F10 },        // TCL_SWING_L4

    // 2nd IR controller.
};

static struct rc_map_list tcl_tv_map = {
	.map = {
		.scan    = tcl_tv,
		.size    = ARRAY_SIZE(tcl_tv),
		.rc_type = RC_TYPE_UNKNOWN,	/* Legacy IR type */
		.name    = RC_MAP_TCL_TV,
	}
};

static int __init init_rc_map_tcl_tv(void)
{
	return rc_map_register(&tcl_tv_map);
}

static void __exit exit_rc_map_tcl_tv(void)
{
	rc_map_unregister(&tcl_tv_map);
}

module_init(init_rc_map_tcl_tv)
module_exit(exit_rc_map_tcl_tv)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");

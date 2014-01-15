/* hisense-tv.h - Keytable for hisense_tv Remote Controller
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
 * this is the remote control that comes with the hisense smart tv
 * which based on STAOS standard.
 */

static struct rc_map_table hisense_tv[] = {
    // 1st IR controller.
    { 0xBF0D, KEY_POWER },
    { 0xBF00, KEY_0 },
    { 0xBF01, KEY_1 },
    { 0xBF02, KEY_2 },
    { 0xBF03, KEY_3 },
    { 0xBF04, KEY_4 },
    { 0xBF05, KEY_5 },
    { 0xBF06, KEY_6 },
    { 0xBF07, KEY_7 },
    { 0xBF08, KEY_8 },
    { 0xBF09, KEY_9 },
    { 0xBF52, KEY_RED },
    { 0xBF53, KEY_GREEN },
    { 0xBF54, KEY_YELLOW },
    { 0xBF55, KEY_BLUE },
    { 0xBF16, KEY_UP },
    { 0xBF17, KEY_DOWN },
    { 0xBF19, KEY_LEFT },
    { 0xBF18, KEY_RIGHT },
    { 0xBF15, KEY_ENTER },
    { 0xBF44, KEY_VOLUMEUP },
    { 0xBF43, KEY_VOLUMEDOWN },
    { 0xBF5C, KEY_HOME},
    { 0xBF14, KEY_MENU },
    { 0xBF48, KEY_BACK },
    { 0xBF0E, KEY_MUTE },
    { 0xBF0B, KEY_NUMERIC_STAR },
    { 0xBF0C, KEY_NUMERIC_POUND },
    { 0xBF0F, KEY_SETUP },

    // 2nd IR cotroller.
    { 0xFC0D, KEY_POWER },
    { 0xFC00, KEY_0 },
    { 0xFC01, KEY_1 },
    { 0xFC02, KEY_2 },
    { 0xFC03, KEY_3 },
    { 0xFC04, KEY_4 },
    { 0xFC05, KEY_5 },
    { 0xFC06, KEY_6 },
    { 0xFC07, KEY_7 },
    { 0xFC08, KEY_8 },
    { 0xFC09, KEY_9 },
    { 0xFC16, KEY_UP },
    { 0xFC17, KEY_DOWN },
    { 0xFC19, KEY_LEFT },
    { 0xFC18, KEY_RIGHT },
    { 0xFC15, KEY_ENTER },
    { 0xFC4A, KEY_CHANNELUP },
    { 0xFC4B, KEY_CHANNELDOWN },
    { 0xFC44, KEY_VOLUMEUP },
    { 0xFC43, KEY_VOLUMEDOWN },
    { 0xFC48, KEY_HOME},
    { 0xFC14, KEY_MENU },
    { 0xFC53, KEY_BACK },
    { 0xFC0E, KEY_MUTE },
    { 0xFC1B, KEY_TV },
    { 0xFC12, KEY_KP0 },
    { 0xFC5A, KEY_STOP },
    { 0xFC4E, KEY_PLAYPAUSE },
    { 0xFC10, KEY_CAMERA },      // (C)PICTURE_MODE
    { 0xFC13, KEY_ZOOM },        // (C)ASPECT_RATIO
    { 0xFC4D, KEY_FN_1 },        // (C)SONG_SYSTEM
    { 0xFC00, 800 },             // HISENSE_FAC_NEC_M
    { 0xFC4E, 801 },             // HISENSE_FAC_NEC_IP
    { 0xFC09, 802 },             // HISENSE_FAC_NEC_SAVE
    { 0xFC1C, 803 },             // HISENSE_FAC_NEC_3D
    { 0xFC18, 804 },             // HISENSE_FAC_NEC_PC
    { 0xFC0D, 805 },             // HISENSE_FAC_NEC_LOGO
    { 0xFC01, 806 },             // HISENSE_FAC_NEC_YPBPR
    { 0xFC52, 807 },             // HISENSE_FAC_NEC_HDMI
    { 0xFC5D, 808 },             // HISENSE_FAC_NEC_F1
    { 0xFC42, 809 },             // HISENSE_FAC_NEC_F2
    { 0xFC56, 810 },             // HISENSE_FAC_NEC_F3
    { 0xFC48, 811 },             // HISENSE_FAC_NEC_F4
    { 0xFC53, 812 },             // HISENSE_FAC_NEC_F5
    { 0xFC1D, 813 },             // HISENSE_FAC_NEC_F6
    { 0xFC4F, 814 },             // HISENSE_FAC_NEC_F7
    { 0xFC47, 815 },             // HISENSE_FAC_NEC_OK
    { 0xFC0A, 816 },             // HISENSE_FAC_NEC_MAC
    { 0xFC05, 817 },             // HISENSE_FAC_NEC_AV
    { 0xFC58, 818 },             // HISENSE_FAC_NEC_PATTERN
    { 0xFC15, 819 },             // HISENSE_FAC_NEC_AGING
    { 0xFC06, 820 },             // HISENSE_FAC_NEC_BALANCE
    { 0xFC40, 821 },             // HISENSE_FAC_NEC_ADC
    { 0xFC50, 822 },             // HISENSE_FAC_NEC_RDRV_INCREASE
    { 0xFC46, 823 },             // HISENSE_FAC_NEC_RDRV_DECREASE
    { 0xFC4C, 824 },             // HISENSE_FAC_NEC_GDRV_INCREASE
    { 0xFC5A, 825 },             // HISENSE_FAC_NEC_GDRV_DECREASE
    { 0xFC49, 826 },             // HISENSE_FAC_NEC_BDRV_INCREASE
    { 0xFC4A, 827 },             // HISENSE_FAC_NEC_BDRV_DECREASE
    { 0xFC44, 828 },             // HISENSE_FAC_NEC_RCUT_INCREASE
    { 0xFC41, 829 },             // HISENSE_FAC_NEC_RCUT_DECREASE
    { 0xFC4B, 830 },             // HISENSE_FAC_NEC_GCUT_INCREASE
    { 0xFC51, 831 },             // HISENSE_FAC_NEC_GCUT_DECREASE
    { 0xFC08, 832 },             // HISENSE_FAC_NEC_BCUT_INCREASE
    { 0xFC45, 833 },             // HISENSE_FAC_NEC_BCUT_DECREASE

    // 3th IR cotroller.
    { 0x0F20, KEY_NUMERIC_0 },
    { 0x0F21, KEY_NUMERIC_1 },
    { 0x0F22, KEY_NUMERIC_2 },
    { 0x0F23, KEY_NUMERIC_3 },
    { 0x0F24, KEY_NUMERIC_4 },
    { 0x0F25, KEY_NUMERIC_5 },
    { 0x0F26, KEY_NUMERIC_6 },
    { 0x0F27, KEY_NUMERIC_7 },
    { 0x0F28, KEY_NUMERIC_8 },
    { 0x0F29, KEY_NUMERIC_9 },
    { 0x0F18, KEY_A },
    { 0x0F19, KEY_B },
    { 0x0F1A, KEY_C },
    { 0x0F1B, KEY_D },
    { 0x0F1C, KEY_E },
    { 0x0F0A, KEY_F },
    { 0x0F2A, KEY_G },
    { 0x0F2B, KEY_H },
    { 0x0F2C, KEY_I },
    { 0x0F2D, KEY_J },
    { 0x0F2E, KEY_K },
    { 0x0F2F, KEY_L },
    { 0x0F31, KEY_M },
    { 0x0F32, KEY_N },
    { 0x0F33, KEY_O },
    { 0x0F34, KEY_P },
    { 0x0F35, KEY_Q },
    { 0x0F36, KEY_R },
    { 0x0F37, KEY_S },
    { 0x0F38, KEY_T },
    { 0x0F39, KEY_U },
    { 0x0F3A, KEY_V },
    { 0x0F3B, KEY_W },
    { 0x0F3C, KEY_X },
    { 0x0F3D, KEY_Y },
    { 0x0F3E, KEY_Z },
    { 0x0F1F, 700 },             // HISENSE_PRODUCT_SCAN_START
    { 0x0F30, 701 },             // HISENSE_PRODUCT_SCAN_OVER
    { 0x0F00, 702 },             // HISENSE_TEST_BROAD_TV
    { 0x0F01, 703 },             // HISENSE_TEST_BROAD_DTV
    { 0x0F02, 704 },             // HISENSE_TEST_BROAD_AV1
    { 0x0F03, 705 },             // HISENSE_TEST_BROAD_AV2
    { 0x0F04, 706 },             // HISENSE_TEST_BROAD_AV3
    { 0x0F05, 707 },             // HISENSE_TEST_BROAD_SVIDEO1
    { 0x0F06, 708 },             // HISENSE_TEST_BROAD_SVIDEO2
    { 0x0F07, 709 },             // HISENSE_TEST_BROAD_SVIDEO3
    { 0x0F08, 710 },             // HISENSE_TEST_BROAD_SCART1
    { 0x0F09, 711 },             // HISENSE_TEST_BROAD_SCART2
    { 0x0F0A, 712 },             // HISENSE_TEST_BROAD_SCART3
    { 0x0F0B, 713 },             // HISENSE_TEST_BOARD_YPBPR1
    { 0x0F0C, 714 },             // HISENSE_TEST_BOARD_YPBPR2
    { 0x0F0D, 715 },             // HISENSE_TEST_BOARD_YPBPR3
    { 0x0F0E, 716 },             // HISENSE_TEST_BOARD_VGA
    { 0x0F0F, 717 },             // HISENSE_TEST_BOARD_HDMI1
    { 0x0F10, 718 },             // HISENSE_TEST_BOARD_HDMI2
    { 0x0F11, 719 },             // HISENSE_TEST_BOARD_HDMI3
    { 0x0F12, 720 },             // HISENSE_TEST_BOARD_HDMI4
    { 0x0F13, 721 },             // HISENSE_TEST_BOARD_HDMI5
    { 0x0F14, 722 },             // HISENSE_TEST_BOARD_DMP
    { 0x0F15, 723 },             // HISENSE_TEST_BOARD_EMP
    { 0x0F16, 724 },             // HISENSE_TEST_BOARD_AUTOCOLOR
    { 0x0F17, 725 },             // HISENSE_TEST_BOARD_SAVE
    { 0x0F18, 726 },             // HISENSE_TEST_BOARD_TELITEXT
    { 0x0F19, 727 },             // HISENSE_TEST_BOARD_SAPL
    { 0x0F1A, 728 },             // HISENSE_TEST_BOARD_VCHIP
    { 0x0F1B, 729 },             // HISENSE_TEST_BOARD_CCD
    { 0x0F1C, 730 },             // HISENSE_TEST_BOARD_BTSC
    { 0x0F1D, 731 },             // HISENSE_TEST_BOARD_FAC_OK
    { 0x0F1E, 732 },             // HISENSE_TEST_BOARD_SAVE_NIGHT
};

static struct rc_map_list hisense_tv_map = {
	.map = {
		.scan    = hisense_tv,
		.size    = ARRAY_SIZE(hisense_tv),
		.rc_type = RC_TYPE_UNKNOWN,	/* Legacy IR type */
		.name    = RC_MAP_HISENSE_TV,
	}
};

static int __init init_rc_map_hisense_tv(void)
{
	return rc_map_register(&hisense_tv_map);
}

static void __exit exit_rc_map_hisense_tv(void)
{
	rc_map_unregister(&hisense_tv_map);
}

module_init(init_rc_map_hisense_tv)
module_exit(exit_rc_map_hisense_tv)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");

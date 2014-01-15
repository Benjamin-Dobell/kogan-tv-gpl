////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BOARD_H_
#define _BOARD_H_

#include <linux/autoconf.h>
//#include <linux/undefconf.h>

//------------------------------IR_TYPE_SEL--------------
#define IR_TYPE_OLD                 0
#define IR_TYPE_NEW                 1
#define IR_TYPE_MSTAR_DTV           2
#define IR_TYPE_MSTAR_RAW		    3
#define IR_TYPE_RC_V16              4
#define IR_TYPE_CUS03_DTV           5
#define IR_TYPE_MSTAR_FANTASY       6
#define IR_TYPE_MSTAR_SZ1           7
#define IR_TYPE_SKYWORTH_SLIDE      8
#define IR_TYPE_HISENSE_6I78        9
#define IR_TYPE_CUS08_RC5           10
#define IR_TYPE_KONKA               11
#define IR_TYPE_CUS21SH             21
#define IR_TYPE_HAIER_TOSHIBA       22
#define IR_TYPE_TCL_RCA             23
#define IR_TYPE_RCMM                24
#define IR_TYPE_TOSHIBA             25


//------------------------------BOARD_TYPE_SEL-----------
// 0x00 ~ 0x1F LCD Demo board made in Taiwan
#define BD_FPGA			0x01
#define BD_GENERIC	        0x02

#define BD_UNKNOWN                  0xFF

#define SVD_CLOCK_250MHZ            0x00
#define SVD_CLOCK_240MHZ            0x01
#define SVD_CLOCK_216MHZ            0x02
#define SVD_CLOCK_SCPLL             0x03
#define SVD_CLOCK_MIU               0x04
#define SVD_CLOCK_144MHZ            0x05
#define SVD_CLOCK_123MHZ            0x06
#define SVD_CLOCK_108MHZ            0x07

#define SVD_CLOCK_ENABLE            TRUE
#define SVD_CLOCK_INVERT            FALSE

#ifndef MS_BOARD_TYPE_SEL

#if defined(CONFIG_MSTAR_AMBER5_BD_FPGA)
    #define MS_BOARD_TYPE_SEL       BD_FPGA
#elif defined(CONFIG_MSTAR_AMBER5_BD_GENERIC)
    #define MS_BOARD_TYPE_SEL       BD_GENERIC
#else
    #error "BOARD define not found"
#endif

#endif

//-------------------------------------------------------


///////////////////////////////////////////////////////////
#if   (MS_BOARD_TYPE_SEL == BD_FPGA)
    #include "BD_FPGA.h"
#elif (MS_BOARD_TYPE_SEL == BD_GENERIC)
    #include "BD_GENERIC.h"
#endif


/////////////////////////////////////////////////////////

#endif /* _BOARD_H_ */


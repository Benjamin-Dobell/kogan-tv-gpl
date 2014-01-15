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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_msystem.c
/// @brief  Memory msystem Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
#include <linux/undefconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <linux/device.h>
#include <asm/io.h>
#include <asm/types.h>

#include "mst_devid.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_msystem.h"
//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define MSYSTEM_DPRINTK(fmt, args...)

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

static int __init mod_msystem_init(void)
{
    mod_miomap_init();

#if !defined(CONFIG_MSTAR_MPOOL) && !defined(CONFIG_MSTAR_MPOOL_MODULE)
    mod_mpool_init();
#endif//#if !defined(CONFIG_MSTAR_MPOOL) && !defined(CONFIG_MSTAR_MPOOL_MODULE)

    _mod_sys_init();
    _MDrv_MBXIO_ModuleInit();
    _MDrv_GFLIPIO_ModuleInit();
    return 0;
}

static void __exit mod_msystem_exit(void)
{
    mod_miomap_exit();

#if !defined(CONFIG_MSTAR_MPOOL) && !defined(CONFIG_MSTAR_MPOOL_MODULE)
    mod_mpool_exit();
#endif//#if !defined(CONFIG_MSTAR_MPOOL) && !defined(CONFIG_MSTAR_MPOOL_MODULE)

    _mod_sys_exit();
    _MDrv_MBXIO_ModuleExit();
    _MDrv_GFLIPIO_ModuleExit();
}

module_init(mod_msystem_init);
module_exit(mod_msystem_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MSYSTEM driver");
MODULE_LICENSE("GPL");

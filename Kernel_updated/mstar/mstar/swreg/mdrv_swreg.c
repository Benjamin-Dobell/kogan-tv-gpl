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
/// file    mdrv_swreg.c
/// @brief  SWREG Driver Interface
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
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#include "mdrv_types.h"
#include "mst_devid.h"
#include "mdrv_swreg.h"
#include "mhal_swreg.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define SWREG_DBG_ENABLE			1

#if SWREG_DBG_ENABLE
#define SWREG_DBG(_f)				(_f)
#else
#define SWREG_DBG(_f)
#endif

#define SWREG_PRINT(fmt, args...)	printk("[SWREG][%05d] " fmt, __LINE__, ## args)

void MDrv_SWREG_WRITE_BIT(U8 offset, U8 low_bit, U16 mask, U16 value)
{
#if defined(CONFIG_MSTAR_TITANIA2)
#else//CONFIG_MSTAR_TITANIA2
    U16 reg;
    reg=low_bit;//for compile warning
    reg=MDrv_SWREG_REG(offset);
    MDrv_SWREG_REG(offset)=(reg&(~mask))|(value&mask);
    SWREG_PRINT("%s: offset=%x,mask=%x,old_value=%x,new_value=%x\n",__FUNCTION__,offset,mask,reg,MDrv_SWREG_REG(offset));
#endif//CONFIG_MSTAR_TITANIA2
}

U16 MDrv_SWREG_READ_BIT(U8 offset, U8 low_bit, U16 mask)
{
#if defined(CONFIG_MSTAR_TITANIA2)
    return 0;
#else//CONFIG_MSTAR_TITANIA2
    U16 reg;
    reg=MDrv_SWREG_REG(offset);
    return (reg&mask)>>low_bit;
#endif//CONFIG_MSTAR_TITANIA2
}

void SWREG_SYS_HOLD(void)
{
    unsigned long flags ;
    local_irq_save(flags);
    local_irq_disable() ;

    MDrv_SWREG_WRITE_BIT(SWREG_REG_SYS_HOLD,1);
    while( 1 )
    {
        udelay(1000000);
        if(0 == MDrv_SWREG_READ_BIT(SWREG_REG_SYS_HOLD)) //resume kernel if changed to 0
        {
            break ;
        }
    }

    local_irq_restore(flags);
}
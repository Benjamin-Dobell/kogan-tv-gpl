/*******************************************************************************
*
* g3dkmd_command_queue.cpp
*
*   -G3Dk
*   -Command queue management implementation
*
* Copyright (c) 2009-2010 MStar Semiconductor, Inc.
*
*******************************************************************************/
#include <linux/autoconf.h>
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
#include <asm/mips-boards/prom.h>
#include "mdrv_types.h"
#include "Board.h"

#include "mdrv_system.h"
#include "mhal_g3dkmd_command_queue.h"
#include "mhal_g3d_reg.h"
#include "mhal_g3d.h"
#include "mdrv_g3d_st.h"

#ifndef U32_PTR
#define U32_PTR unsigned int
#endif

#undef OPT_G3D_HAL_DEBUG
#undef G3D_HAL_DBG
#ifdef OPT_G3D_HAL_DEBUG
    #define G3D_HAL_DBG(fmt, args...)  printk(KERN_WARNING "[G3D_HAL][%05d]" fmt, __LINE__, ## args)
#else
    #define G3D_HAL_DBG(fmt, args...)
#endif

/*------------------------------------------------------------------------------
 *
 * Command queue
 *
 *-----------------------------------------------------------------------------*/
static G3DKMD_COMMAND_QUEUE _g3dkCommandQueue;

#define ARBIT_REQ_TR        1
#define ARBIT_REQ_CQR       2
#define ARBIT_REQ_USHICR    3

/* priority setting for group C channel 0 */
static const U32 s_ArGpC0RqPri[4] = /* index = ((cq_channel)<<1 | (tr_channel)) */
{
    (ARBIT_REQ_USHICR<<4)|(ARBIT_REQ_CQR<<2)|(ARBIT_REQ_TR<<0),
    (ARBIT_REQ_CQR<<0),
    (ARBIT_REQ_USHICR<<2)|(ARBIT_REQ_TR<<0),
    0
};

/* priority setting for group C channel 1 */
static const U32 s_ArGpC1RqPri[4] = /* index = ((cq_channel)<<1 | (tr_channel)) */
{
    0,
    (ARBIT_REQ_USHICR<<2)|(ARBIT_REQ_TR<<0),
    (ARBIT_REQ_CQR<<0),
    (ARBIT_REQ_USHICR<<4)|(ARBIT_REQ_CQR<<2)|(ARBIT_REQ_TR<<0),
};


/*------------------------------------------------------------------------------
 *
 * Command queue helper functions
 *
 *-----------------------------------------------------------------------------*/
G3DKMD_BOOL _g3dkmd_CreateCommandQueue(unsigned int size)
{
    unsigned int addr, len;

    if (0 != _g3dkCommandQueue.PhysicalAddress)
    {
        return G3DKMD_TRUE;
    }

    get_boot_mem_info(G3D_CMDQ, &addr, &len);
    if(size > len)
    {
        printk("G3D Cmd Queue create fail : required size = 0x%x, actual size = 0x%x\n", size, len);
        return G3DKMD_FALSE;
    }

    /* memset(&_g3dkCommandQueue, 0, sizeof(_g3dkCommandQueue)); */

    /* create command queue... */
    _g3dkCommandQueue.Size             = len;
    _g3dkCommandQueue.GuardbandSize    = G3DKMD_COMMAND_QUEUE_GUARDBAND_SIZE;
    _g3dkCommandQueue.Mask             = len - 1;
    _g3dkCommandQueue.PhysicalAddress  = addr & MASK_MIU_PHYSADDRESS; /* g3d1malloc(_g3dkCommandQueue.Size); */
    _g3dkCommandQueue.pBuffer          = (unsigned char*)addr;

    if(((int)(_g3dkCommandQueue.pBuffer) & MIU1_PHYSADDRESS) == MIU1_PHYSADDRESS)
        _g3dkCommandQueue.Channel = 1;
    else
        _g3dkCommandQueue.Channel = 0;

    G3D_HAL_DBG("create g3d cmdQ buffer at 0x%x, channel %d\n",
            _g3dkCommandQueue.PhysicalAddress,_g3dkCommandQueue.Channel);

    return G3DKMD_TRUE;
}/* _g3dkmd_CreateCommandQueue */

void _g3dkmd_DestroyCommandQueue(void)
{
    /* destroy command queue... */

    /* cleanup... */
    memset(&_g3dkCommandQueue, 0, sizeof(_g3dkCommandQueue));
}/* _g3dkmd_DestroyCommandQueue */

void _g3dkmd_ResetCommandQueue(void)
{
    U32 base;
    U32 bank;
    U32 size;
    U32 dwArGpC01RqPri;
    U32 index;
    U32 tex_channel;
    U32 mpool1_addr, mpool1_len;

    _g3dkCommandQueue.pWritePointer     = _g3dkCommandQueue.pBuffer;
    _g3dkCommandQueue.pPrevWritePointer = _g3dkCommandQueue.pWritePointer;
    _g3dkCommandQueue.AvailableSize     = _g3dkCommandQueue.Size - _g3dkCommandQueue.GuardbandSize;

    switch ((_g3dkCommandQueue.Size >> 10))
    {
        case 16:    size = 0;   break;  /* 0: 16KB  */
        case 32:    size = 1;   break;  /* 1: 32KB  */
        case 64:    size = 2;   break;  /* 2: 64KB  */
        case 128:   size = 3;   break;  /* 3: 128KB */
        case 512:   size = 4;   break;  /* 4: 512KB */
        case 1024:  size = 5;   break;  /* 5: 1M    */
        case 2048:  size = 6;   break;  /* 6: 2M    */
        case 4096:  size = 7;   break;  /* 7: 4M    */
        default:    size = 6;   break;  /* 6: 2M    */
    }

    G3D_HAL_DBG("_g3dkCommandQueue.Size = 0x%x , size = %d\n",_g3dkCommandQueue.Size,size);
    base = _g3dkCommandQueue.PhysicalAddress;
    bank = 0;

    get_boot_mem_info(G3D_MEM1, &mpool1_addr, &mpool1_len);
    tex_channel = ((mpool1_addr&MIU1_PHYSADDRESS) == MIU1_PHYSADDRESS) ? 1 : 0;

    index = (_g3dkCommandQueue.Channel<<1)|tex_channel;
    dwArGpC01RqPri = (s_ArGpC0RqPri[index]<<SHF_AR_GP_C0_RQ_PRI) |
                     (s_ArGpC1RqPri[index]<<SHF_AR_GP_C1_RQ_PRI);

    G3D_HAL_DBG("_g3dkmd_ResetCommandQueue start\n");

    /* MIU setting */
    *(unsigned short volatile *)(REG_MIPS_BASE + ((0x7B)*2+ 0x1200) *2 ) |= 0x20;

    /* clock */
    *(unsigned short volatile *)(REG_MIPS_BASE + ((0x20)*2+ 0x3300) *2 ) = 0;/* 0x1818; */
    *(unsigned short volatile *)(REG_MIPS_BASE + ((0x20)*2+1+ 0x3300)*2) = 0;/* 0x1818; */
    *(unsigned short volatile *)(REG_MIPS_BASE + ((0x21)*2+ 0x3300)*2) = 0;
    *(unsigned short volatile *)(REG_MIPS_BASE + ((0x21)*2+1+ 0x3300)*2) = 0;

    /* MIU1 group 3 interrupt */
    {
        U32 reg = (*(volatile unsigned int *)(REG_MIPS_BASE + ((0x0600 + (0x0050<<1) )<<1)));
        /* (*(volatile unsigned int *)(REG_MIPS_BASE + ((0x0600 + (0x0051<<1) )<<1))) = 2;
         */
        (*(volatile unsigned int *)(REG_MIPS_BASE + ((0x0600 + (0x0051<<1) )<<1))) = 2;
        (*(volatile unsigned int *)(REG_MIPS_BASE + ((0x0600 + (0x0050<<1) )<<1))) = reg|4;
    }

    /* MIU0 group 3 interrupt */
    {
        U32 reg = (*(volatile unsigned int *)(REG_MIPS_BASE + ((0x1200 + (0x0050<<1) )<<1)));
        /* (*(volatile unsigned int *)(REG_MIPS_BASE + ((0x0600 + (0x0051<<1) )<<1))) = 2;
         */
        (*(volatile unsigned int *)(REG_MIPS_BASE + ((0x1200 + (0x0051<<1) )<<1))) = 2;
        (*(volatile unsigned int *)(REG_MIPS_BASE + ((0x1200 + (0x0050<<1) )<<1))) = reg|4;
    }

#if 0
    *(unsigned short volatile *)(REG_MIPS_BASE + ((0x7f)*2+ 0x1f00)*2) = 0;
    *(unsigned short volatile *)(REG_MIPS_BASE + ((0x19)*2+ 0x1f00)*2) = 0xfff8;

    *(unsigned short volatile *)(0xBF200000 + 0x3380 * 2) = 0x55AA;
    *(unsigned short volatile *)(0xBF200000 + 0x3382 * 2) = 0x55AA;
    *(unsigned short volatile *)(0xBF200000 + 0x3384 * 2) = 0x55AA;
    *(unsigned short volatile *)(0xBF200000 + 0x3386 * 2) = 0x55AA;
#endif

    G3D_HAL_DBG("_g3dkmd_ResetCommandQueue start1\n");

    /* G3D internal reset
     */
    REG_MASK_WRITE(REG_SWRST, MSK_SWRST, MSK_SWRST); /* software reset */

#ifdef CONFIG_MSTAR_JANUS2
    REG_WRITE(REG_G3DPLL_CTRL0, 0x00bc0081);
    REG_WRITE(REG_G3DPLL_CTRL1, 0x0000c31e);
#else
    REG_WRITE(REG_G3DPLL_CTRL0, 0x002c0040);
#endif
    REG_WRITE(REG_CQ_STATUS,    0x001f0000);


    REG_MASK_WRITE(REG_SWRST, 0, MSK_SWRST);
    REG_WRITE(REG_CQ_REQ_TH, MSK_CQ_SWRST);          /* command queue reset */
    REG_WRITE(REG_CQ_WT_PTR, 0);
    REG_WRITE(REG_CQ_REQ_TH, 0);

    G3D_HAL_DBG("set REG_CQ_REQ_TH = 0x%x\n",((size << SHF_CQ_SW_CQSIZE) | (2 << SHF_CQ_PRIOR_MODE) | 16));
    REG_WRITE(REG_CQ_REQ_TH, ((size << SHF_CQ_SW_CQSIZE) | (2 << SHF_CQ_PRIOR_MODE) | 16));
    REG_WRITE(REG_CQ_BASE, base);
    REG_MASK_WRITE(REG_SWRST,
                   ((bank << SHF_MEM_BANK_SEL) | MSK_HW_CLR_CACHE_EN | dwArGpC01RqPri),
                   (MSK_MEM_BANK_SEL|MSK_HW_CLR_CACHE_EN|MSK_AR_GP_C0_RQ_PRI|MSK_AR_GP_C1_RQ_PRI));

#ifdef CONFIG_MSTAR_TITANIA8
    {
        U32 regSwRst;
        U32 reg8010,reg8110,reg8210/*, reg8310*/;
        /* set REG_CH2MEM_BANK_SEL for chip U02, RIU have to switch to bank 3 first
        */
        REG_MASK_WRITE(REG_MULTI_BUF, MSK_PA_RIU_ACCESS_EN, MSK_PA_RIU_ACCESS_EN);

        regSwRst = REG_READ(REG_SWRST);
        regSwRst = (regSwRst & ~(MSK_RIU_RD_BK))|(0<<SHF_RIU_RD_BK);
        REG_WRITE(REG_SWRST, regSwRst);
        reg8010 = REG_READ(REG_CH2MEM_BANK_SEL);

        regSwRst = (regSwRst & ~(MSK_RIU_RD_BK))|(1<<SHF_RIU_RD_BK);
        REG_WRITE(REG_SWRST, regSwRst);
        reg8110 = REG_READ(REG_CH2MEM_BANK_SEL);

        regSwRst = (regSwRst & ~(MSK_RIU_RD_BK))|(2<<SHF_RIU_RD_BK);
        REG_WRITE(REG_SWRST, regSwRst);
        reg8210 = REG_READ(REG_CH2MEM_BANK_SEL);

        regSwRst = (regSwRst & ~(MSK_RIU_RD_BK))|(3<<SHF_RIU_RD_BK);
        REG_WRITE(REG_SWRST, regSwRst);
        /* reg8310 = REG_READ(REG_CH2MEM_BANK_SEL);
        */
        REG_WRITE(REG_CH2MEM_BANK_SEL, 0<<SHF_CH2MEM_BANK_SEL);

        regSwRst = (regSwRst & ~(MSK_RIU_RD_BK))|(2<<SHF_RIU_RD_BK);
        REG_WRITE(REG_SWRST, regSwRst);
        REG_WRITE(REG_CH2MEM_BANK_SEL, reg8210);

        regSwRst = (regSwRst & ~(MSK_RIU_RD_BK))|(1<<SHF_RIU_RD_BK);
        REG_WRITE(REG_SWRST, regSwRst);

        REG_WRITE(REG_CH2MEM_BANK_SEL, reg8110);

        regSwRst = (regSwRst & ~(MSK_RIU_RD_BK))|(0<<SHF_RIU_RD_BK);
        REG_WRITE(REG_SWRST, regSwRst);

        REG_WRITE(REG_CH2MEM_BANK_SEL, reg8010);

        /* REG_MASK_WRITE(REG_SWRST, 0<<SHF_RIU_RD_BK, MSK_RIU_RD_BK);
        */
        REG_MASK_WRITE(REG_MULTI_BUF, 0, MSK_PA_RIU_ACCESS_EN);
    }
#endif

    G3D_HAL_DBG("_g3dkmd_ResetCommandQueue end\n");
}/* _g3dkmd_ResetCommandQueue */

U32_PTR GetCommandQueueReadPointer(void)
{
    U32_PTR read_pointer;

#if 0
    if (g_G3dGlobals.Config.ExecutionMode == G3D_EXECUTION_MODE_NORMAL)
#endif
    {
        U32_PTR rp0, rp1, rp2;

        /* safe read. Avoid not credible read pointer. */
        do
        {
            REG_LATCH_READ(REG_CQ_RD_PTR, rp0);
            REG_LATCH_READ(REG_CQ_RD_PTR, rp1);
            REG_LATCH_READ(REG_CQ_RD_PTR, rp2);

        } while((rp0 > rp1) || (rp1 > rp2));

        read_pointer = (U32_PTR)_g3dkCommandQueue.pBuffer + rp2;
    }
#if 0
    else
    {
        read_pointer = (U32_PTR)_g3dkCommandQueue.pWritePointer;
    }
#endif

    return read_pointer;
}/* GetCommandQueueReadPointer */

void _g3dkmd_KillTime(void)
{
    static U32 kill_time_count = 0;
    static U32 reset_count = 1000000;
    static U32 last_read_pointer;
    static U32 last_write_pointer;
    U32 write_pointer;
    U32 read_pointer;

    int i = 0;
    for(i = 0; i < 500; i++)
    {
        ;
    }

    REG_LATCH_READ(REG_CQ_RD_PTR, read_pointer);
    write_pointer = REG_READ(REG_CQ_WT_PTR);

    kill_time_count++;

    if (kill_time_count == 1)
    {
        last_read_pointer  = read_pointer;
        last_write_pointer = write_pointer;
    }
    else if (kill_time_count > reset_count)
    {
        if ((last_read_pointer == read_pointer) && (last_write_pointer == write_pointer))
        {
            printk("G3D is reset!\n");
            _g3dkmd_ResetCommandQueue();
        }

        kill_time_count = 0;
    }
    else if (last_read_pointer != read_pointer)
    {
        kill_time_count = 0;
    }
}/* KillTime */

inline void _g3dkmd_WriteCommand(U32 command)
{
    *(U32*)_g3dkCommandQueue.pWritePointer = command;
    _g3dkCommandQueue.pWritePointer += sizeof(command);
}//WriteCommand

G3DKMD_BOOL _g3dkmd_SubmitCommands(const void* commands, unsigned int length, unsigned int* stamp)
{
    /* submit commands... */
    U32 bSeparate = 0;
    U32 total_length = ( length + 3 ) + 8;  // + 3 for align , +8 for add stamp
    U32_PTR read_pointer = GetCommandQueueReadPointer();

    G3D_HAL_DBG("_g3dkmd_SubmitCommands start (length = %x)!\n",length);
    if(total_length >= _g3dkCommandQueue.Size)
    {
        /* TODO: */
        printk("input cmdQ is too large!!!\n");
        return  G3DKMD_FALSE;
    }

    do
    {
        read_pointer = GetCommandQueueReadPointer();

        G3D_HAL_DBG("read_pointer = %x, pWritePointer = %x, ava_size = %x\n",
                read_pointer,
                _g3dkCommandQueue.pWritePointer,
                _g3dkCommandQueue.AvailableSize);

        if(read_pointer > (U32)_g3dkCommandQueue.pWritePointer)
        {
            _g3dkCommandQueue.AvailableSize = read_pointer - (U32)_g3dkCommandQueue.pWritePointer;
            if( _g3dkCommandQueue.AvailableSize > total_length )
                break;
        }
        else
        {
            _g3dkCommandQueue.AvailableSize =
                _g3dkCommandQueue.Size - (_g3dkCommandQueue.pWritePointer - _g3dkCommandQueue.pBuffer);

            if( _g3dkCommandQueue.AvailableSize > total_length)
            {
                break;
            }
            else
            {
                _g3dkCommandQueue.AvailableSize += (read_pointer - (U32)_g3dkCommandQueue.pBuffer);
                if( _g3dkCommandQueue.AvailableSize > total_length )
                {
                    bSeparate = 1;
                    break;
                }
            }
        }

        _g3dkmd_KillTime();
    } while(1);

    G3D_HAL_DBG("start to copy!!!\n");

    if (bSeparate)
    {
        U8 *ptr = (U8 *)commands;
        U32 size1 =
            _g3dkCommandQueue.Size - (_g3dkCommandQueue.pWritePointer-_g3dkCommandQueue.pBuffer);

        if(size1 > length)
        {
            copy_from_user(_g3dkCommandQueue.pWritePointer, (void __user *) ptr, length);
            _g3dkCommandQueue.pWritePointer += length;
        }
        else
        {
            copy_from_user(_g3dkCommandQueue.pWritePointer, (void __user *) ptr, size1);

            ptr += size1;
            size1 = length - size1;
            copy_from_user(_g3dkCommandQueue.pBuffer, (void __user *) ptr, size1);
            _g3dkCommandQueue.pWritePointer = _g3dkCommandQueue.pBuffer + size1;
        }
    }
    else
    {
         copy_from_user(_g3dkCommandQueue.pWritePointer, (void __user *) commands, length);
         _g3dkCommandQueue.pWritePointer += length;
     }

    G3D_HAL_DBG("flush!!!\n");

    ++(*stamp);  //increase stamp

    _g3dkCommandQueue.pWritePointer = (unsigned char*)(((U32_PTR)_g3dkCommandQueue.pWritePointer + 3)&~3);

    if(_g3dkCommandQueue.pWritePointer == (_g3dkCommandQueue.pBuffer+_g3dkCommandQueue.Size))
        _g3dkCommandQueue.pWritePointer = _g3dkCommandQueue.pBuffer;

    _g3dkmd_WriteCommand((0x20000000 | REG_GP_GBL0));

    if(_g3dkCommandQueue.pWritePointer == (_g3dkCommandQueue.pBuffer+_g3dkCommandQueue.Size))
        _g3dkCommandQueue.pWritePointer = _g3dkCommandQueue.pBuffer;

    _g3dkmd_WriteCommand(*stamp);

    if(_g3dkCommandQueue.pWritePointer == (_g3dkCommandQueue.pBuffer+_g3dkCommandQueue.Size))
        _g3dkCommandQueue.pWritePointer = _g3dkCommandQueue.pBuffer;

     //printk("_g3dkCommandQueue.pWritePointer:0x%x\n", _g3dkCommandQueue.pWritePointer);


    /* Chip_Flush_Memory(); */
    _dma_cache_wback_inv((unsigned long)_g3dkCommandQueue.pBuffer,
            (unsigned long)(_g3dkCommandQueue.Size));

    {
        G3D_MMAP_BUFFER Buf;
        MHal_G3D_GetMMAP(&Buf);
        _dma_cache_wback_inv((unsigned long)Buf.paMemoryPool0 , (unsigned long)(Buf.memoryPool0_size));
        _dma_cache_wback_inv((unsigned long)Buf.paMemoryPool1 , (unsigned long)(Buf.memoryPool1_size));
    }
    G3D_HAL_DBG("update wptr 0x%x!!!\n",
            (_g3dkCommandQueue.pWritePointer - _g3dkCommandQueue.pBuffer));

#ifdef DBGCOMMANDQ
    if( (_g3dkCommandQueue.pWritePointer > (_g3dkCommandQueue.pBuffer + _g3dkCommandQueue.Size)) ||
        (_g3dkCommandQueue.pWritePointer <= _g3dkCommandQueue.pBuffer))
    {
         printk(" wptr 0x%x, prewprt 0x%x, (0x%x,0x%x)!!!\n",
            _g3dkCommandQueue.pWritePointer ,_g3dkCommandQueue.pPrevWritePointer,length, total_length);

    }
#endif

    /* flush */
    REG_WRITE(REG_CQ_STATUS, 0);
    REG_WRITE(REG_CQ_WT_PTR, (_g3dkCommandQueue.pWritePointer - _g3dkCommandQueue.pBuffer));
    REG_WRITE(REG_CQ_STATUS, MSK_CQ_FIRE);

    _g3dkCommandQueue.pPrevWritePointer = _g3dkCommandQueue.pWritePointer;
    return G3DKMD_TRUE;
}/* _g3dkmd_SubmitCommands */


G3D_API void _g3dkmd_GetCmdQBufferInfo(unsigned int *phsical_address,
                                       unsigned int *channel, unsigned int *size)
{
    *phsical_address = _g3dkCommandQueue.PhysicalAddress;
    *channel         = _g3dkCommandQueue.Channel;
    *size            = _g3dkCommandQueue.Size;
}


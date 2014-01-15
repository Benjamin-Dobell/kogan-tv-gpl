/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    PROJECT: Columbus/Pioneer

	FILE NAME: arch/arm/mach-columbus/columbus_ge_dma.c

    DESCRIPTION:
          GE DMA

    HISTORY:
         <Date>     <Author>    <Modification Description>
        2009/09/23  Jesse Su  Do the porting

------------------------------------------------------------------------------*/
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>

#define MIU_ALIGN_UNIT          8
#define MAX_HEIGHT				4		//The max line allowed to transmit. Each line represetns 1024
#define DMA_LINE_WIDTH			1024	//This is a constant
#define MAX_WAIT_FOR_DMA_BUSY	1000		//1000ms

#define GE_REG_EN0              0xA0005000  //1400+00
#define GE_REG_EN1              0xA0005004  //1400+01
#define GE_REG_BUSY             0xA000501C  //1400+07
#define GE_REG_SB_BASE0         0xA0005080  //1400+20
#define GE_REG_SB_BASE1         0xA0005084  //1400+21
#define GE_REG_DB_BASE0         0xA0005098  //1400+26
#define GE_REG_DB_BASE1         0xA000509C  //1400+27
#define GE_REG_SB_PIT           0xA00050C0  //1400+30
#define GE_REG_DB_PIT           0xA00050CC  //1400_33
#define GE_REG_B_FM             0xA00050D0  //1400+34
#define GE_REG_CLIP_LEFT        0xA0005154  //1400+55 
#define GE_REG_CLIP_RIGHT       0xA0005158  //1400+56
#define GE_REG_CLIP_TOP         0xA000515C  //1400+57
#define GE_REG_CLIP_BOTTOM      0xA0005160  //1400+58
#define GE_REG_CMD              0xA0005180  //1400+60
#define GE_REG_STBB_DX          0xA0005190  //1400+64
#define GE_REG_STBB_DY          0xA0005194  //1400+65
#define GE_REG_PRI_V0_X         0xA00051A0  //1400+68
#define GE_REG_PRI_V0_Y         0xA00051A4  //1400+69
#define GE_REG_PRI_V1_X         0xA00051A8  //1400+6A
#define GE_REG_PRI_V1_Y         0xA00051AC  //1400+6B
#define GE_REG_PRI_V2_X         0xA00051B0  //1400+6C
#define GE_REG_PRI_V2_Y         0xA00051B4  //1400+6D
#define GE_REG_STBB_S_W         0xA00051B8  //1400+6E         
#define GE_REG_STBB_S_H         0xA00051BC  //1400+6F

#ifndef TRUE
#define TRUE                    1
#endif

#ifndef FALSE
#define FALSE                   0
#endif

#define COLUMBUS_GE_DMA_DEBUG   0
#if defined (KERN_DBG_PRINT) || (COLUMBUS_GE_DMA_DEBUG==1)
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

#define GE_DMA_ASYN_EN        (1)

static DEFINE_SPINLOCK(ge_dma_lock);

/*------------------------------------------------------------------------------
    Function: is_columbus_ge_busy

    Description:
        To see whether GE is busy
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        TRUE : busy
        FALSE: not busy
    Remark:
        None.
-------------------------------------------------------------------------------*/
u8 is_columbus_ge_busy(void)
{
    if( columbus_readw(GE_REG_BUSY) & 0x0001 )
        return TRUE;
    else
        return FALSE;
}
EXPORT_SYMBOL_GPL(is_columbus_ge_busy);

/*------------------------------------------------------------------------------
    Function: cooumbus_ge_dma

    Description:
        Activate GE DMA for memory-to-memory data movement.
        If the address is not aligned to 8-byte boundary, go do memcpy instead.
        Synchronous DMA is supported. Also for safe, when GE DMA is busy then this function 
        	performs memcpy instead.
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
		None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void columbus_ge_dma(u32 u32srcaddr, u32 u32dstaddr, u32 u32len)
{
    u32 tmpy;
    u32 unalign_src, unalign_dst;
    u32	iCount;
	
    // columbus_writew( ~0x0020 & columbus_readw(0xA0004A2C), 0xA0004A2C); //enable 123MHZ
    columbus_writew( 0x0004, 0xA0003CA0);                                // make GE work @108MHz
    columbus_writew( ~0x0040 & columbus_readw(0xA0003CB8), 0xA0003CB8); //NOT clock gate

    spin_lock(&ge_dma_lock);
    
    DBG("Enter cloumbus_ge_dma u32srcaddr %08x u32dstaddr %08x u32len %08x\n", u32srcaddr, u32dstaddr, u32len);

    if(	is_columbus_ge_busy() )
    {
    	printk("No DMA channel is available. Do MEMCPY\n"); 
		memcpy((void *)dma_to_virt(NULL,u32dstaddr), (void *)dma_to_virt(NULL,u32srcaddr), u32len);
		spin_unlock(&ge_dma_lock);
    	return;
    }	

	//deal with alignment concern
	unalign_src = u32srcaddr%MIU_ALIGN_UNIT;
	unalign_dst = u32dstaddr%MIU_ALIGN_UNIT;
	
	if( 0 != unalign_src )						//src is not aligned
	{
		if( 0 == unalign_dst )					//1. src is not aligned while dst is. Do COPY
		{
			DBG("1. src is not aligned while dst is: MEMCPY\n"); 
			memcpy((void *)dma_to_virt(NULL,u32dstaddr), (void *)dma_to_virt(NULL,u32srcaddr), u32len);
			u32srcaddr += u32len;
        	u32dstaddr += u32len;
			u32len = 0;
			
			spin_unlock(&ge_dma_lock);
    		return;
		}
		else if ( unalign_src == unalign_dst ) 	//2. both are not aligned, but with the same value. Do COPY then DMA
		{
			DBG("2. src/dst are not aligned but the same unalignment: MEMCPY and DMA\n"); 
			memcpy((void *)dma_to_virt(NULL,u32dstaddr), (void *)dma_to_virt(NULL,u32srcaddr), unalign_src);
			u32srcaddr += unalign_src;
        	u32dstaddr += unalign_src;
        	u32len     -= unalign_src;
		}
		else									//3. both are not aligned with different values. Do COPY
		{
			DBG("3. src/dst are not aligned nor with the same alignment: MEMCPY\n"); 
			memcpy((void *)dma_to_virt(NULL,u32dstaddr), (void *)dma_to_virt(NULL,u32srcaddr), u32len);
			u32srcaddr += u32len;
        	u32dstaddr += u32len;
			u32len = 0;
			
			spin_unlock(&ge_dma_lock);
    		return;			
		}
	}
	else										//src is aligned
	{
		if( 0 != unalign_dst )					//4. src is aligned while dst is not. Do COPY
		{
			DBG("4. src is aligned while dst is not: MEMCPY\n"); 
			memcpy((void *)dma_to_virt(NULL,u32dstaddr), (void *)dma_to_virt(NULL,u32srcaddr), u32len);
			u32srcaddr += u32len;
        	u32dstaddr += u32len;
			u32len = 0;
			
			spin_unlock(&ge_dma_lock);
    		return;			
		}
		else									//5. both are aligned. Do DMA
		{
			DBG("5. src/dst are both aligned: DMA\n"); 
		}		
	}
	    
    columbus_writew( 0x0001, GE_REG_EN0);
    columbus_writew( 0x0001, GE_REG_EN1);
    
    columbus_writew( 0x0404, GE_REG_B_FM);

    // Coordinate
    columbus_writew( 0, GE_REG_PRI_V0_X);
    columbus_writew( 0, GE_REG_PRI_V0_Y);
    columbus_writew( 0, GE_REG_PRI_V2_X);
    columbus_writew( 0, GE_REG_PRI_V2_Y);

    // stretch
    columbus_writew( 0x8000, GE_REG_STBB_DX);
    columbus_writew( 0x8000, GE_REG_STBB_DY);
        
    columbus_writew( DMA_LINE_WIDTH, GE_REG_SB_PIT);
    columbus_writew( DMA_LINE_WIDTH, GE_REG_DB_PIT);

    columbus_writew( DMA_LINE_WIDTH, GE_REG_STBB_S_W);
    columbus_writew( MAX_HEIGHT, GE_REG_STBB_S_H);
        
    columbus_writew( 0, GE_REG_CLIP_LEFT);
    columbus_writew( 0, GE_REG_CLIP_TOP);
    columbus_writew( DMA_LINE_WIDTH, GE_REG_CLIP_RIGHT);
    columbus_writew( 0x07FF, GE_REG_CLIP_BOTTOM);

    while( DMA_LINE_WIDTH <= u32len )
    {
    	DBG("One GE DMA round >DMA_LINE_WIDTH\n"); 
        columbus_writew( (u32srcaddr - (u32srcaddr%MIU_ALIGN_UNIT)) & 0x0000FFFF, GE_REG_SB_BASE0);
        columbus_writew( ((u32srcaddr - (u32srcaddr%MIU_ALIGN_UNIT)) & 0x07FF0000) >> 16, GE_REG_SB_BASE1);
           
        columbus_writew( (u32dstaddr - (u32dstaddr%MIU_ALIGN_UNIT))& 0x0000FFFF, GE_REG_DB_BASE0);
        columbus_writew( ((u32dstaddr - (u32dstaddr%MIU_ALIGN_UNIT)) & 0x07FF0000) >> 16, GE_REG_DB_BASE1);

        columbus_writew( u32dstaddr%MIU_ALIGN_UNIT, GE_REG_PRI_V0_X);
        columbus_writew( 0, GE_REG_PRI_V0_Y);

		tmpy = (u32len / DMA_LINE_WIDTH) ;
		if(tmpy > MAX_HEIGHT)
			tmpy = MAX_HEIGHT;
			
        columbus_writew( (u32dstaddr%MIU_ALIGN_UNIT) + (DMA_LINE_WIDTH-1), GE_REG_PRI_V1_X);
        columbus_writew( tmpy-1, GE_REG_PRI_V1_Y);
            
        columbus_writew( (u32srcaddr%MIU_ALIGN_UNIT), GE_REG_PRI_V2_X);
        columbus_writew( 0, GE_REG_PRI_V2_Y);
            
        columbus_writew( 0x40, GE_REG_CMD);     //fire

        u32srcaddr += tmpy*DMA_LINE_WIDTH;
        u32dstaddr += tmpy*DMA_LINE_WIDTH;
        u32len     -= tmpy*DMA_LINE_WIDTH;
    }

    if( 0 < u32len )
    {
    	DBG("One GE DMA round <DMA_LINE_WIDTH\n"); 
        columbus_writew( (u32srcaddr - (u32srcaddr%MIU_ALIGN_UNIT)) & 0x0000FFFF, GE_REG_SB_BASE0);
        columbus_writew( ((u32srcaddr - (u32srcaddr%MIU_ALIGN_UNIT)) & 0x07FF0000) >> 16, GE_REG_SB_BASE1);
        columbus_writew( (u32dstaddr - (u32dstaddr%MIU_ALIGN_UNIT))& 0x0000FFFF, GE_REG_DB_BASE0);
        columbus_writew( ((u32dstaddr - (u32dstaddr%MIU_ALIGN_UNIT)) & 0x07FF0000) >> 16, GE_REG_DB_BASE1);
        
        columbus_writew( u32dstaddr%MIU_ALIGN_UNIT, GE_REG_PRI_V0_X);
        columbus_writew( 0, GE_REG_PRI_V0_Y);

        columbus_writew( (u32dstaddr%MIU_ALIGN_UNIT) + u32len - 1, GE_REG_PRI_V1_X);
        columbus_writew( 0, GE_REG_PRI_V1_Y);
            
        columbus_writew( (u32srcaddr%MIU_ALIGN_UNIT), GE_REG_PRI_V2_X);
        columbus_writew( 0, GE_REG_PRI_V2_Y);
        
        columbus_writew( 0x40, GE_REG_CMD);     //fire

        u32len -= u32len;
    }

#if (GE_DMA_ASYN_EN==0)

	iCount = MAX_WAIT_FOR_DMA_BUSY * 1000;
    while (	is_columbus_ge_busy() && (iCount != 0))
    {
    	udelay(1);		//delay for 1us
		iCount--;
    }
    //DBG("Exit cloumbus_ge_dma sleep %ld ms\n", (MAX_WAIT_FOR_DMA_BUSY * 1000)-iCount);

#endif
    
    spin_unlock(&ge_dma_lock);

}
EXPORT_SYMBOL_GPL(columbus_ge_dma);
 

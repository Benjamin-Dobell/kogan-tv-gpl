
#ifndef _MHAL_G3D_REG_H_
#define _MHAL_G3D_REG_H_

#include "chip_setup.h"


#define MIU0_PHYSADDRESS 0xA0000000
#define MIU1_PHYSADDRESS 0xD0000000

#define MASK_MIU_PHYSADDRESS 0x0fffffff

//----------------------------------------------------------------------
#ifndef U8
#define U8 unsigned char
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef U32
#define U32 unsigned int
#endif

//----------------------------------------------------------------------
#define _BIT0                       0x00000001
#define _BIT1                       0x00000002
#define _BIT2                       0x00000004
#define _BIT3                       0x00000008
#define _BIT4                       0x00000010
#define _BIT5                       0x00000020
#define _BIT6                       0x00000040
#define _BIT7                       0x00000080
#define _BIT8                       0x00000100
#define _BIT9                       0x00000200
#define _BIT10                      0x00000400
#define _BIT11                      0x00000800
#define _BIT12                      0x00001000
#define _BIT13                      0x00002000
#define _BIT14                      0x00004000
#define _BIT15                      0x00008000

#define _BIT31                      0x80000000


/****************************************************************************************/
#define REG_SWRST            0x8014    //debug offset = 0x28
/*--------------------------------------------------------------------------------------*/
#define MSK_SWRST            0x00000001 //Software Rest
#define SHF_SWRST            0

/*--------------------------------------------------------------------------------------*/
#define MSK_G3D2MCU_INT      0x00000002 //Interrupt to MCU when frame ready
#define SHF_G3D2MCU_INT      1

/*--------------------------------------------------------------------------------------*/
#define MSK_MEM_BANK_SEL     0x00000004 //Memory Bank Select
										// 0: 1st 128MByte memory
										// 1: 2rd 128MByte memory
#define SHF_MEM_BANK_SEL     2

/*--------------------------------------------------------------------------------------*/
#define MSK_MULTI_BUF_BASE_BIT27 0x00000008 //MULTI_BUF_BASE[27]
#define SHF_MULTI_BUF_BASE_BIT27 3

/*--------------------------------------------------------------------------------------*/
#define MSK_RIU_RD_BK        0x00000030 //riu read bank 2'b00,2'b01,2'b10,2'b11
#define SHF_RIU_RD_BK        4

/*--------------------------------------------------------------------------------------*/
#define MSK_HW_CLR_CACHE_EN  0x00000040 //hard ware clear cache at list end enable
#define SHF_HW_CLR_CACHE_EN  6

/*--------------------------------------------------------------------------------------*/
#define MSK_PNG_GIF_EN       0x00000080 //PNG/GIF function enable
#define SHF_PNG_GIF_EN       7

/*--------------------------------------------------------------------------------------*/
#define MSK_G3D_U_SEL        0x00000300 //uclk sel: 00: clk_g3d_u_p, 01: clk_g3d_e_p, 11: clk_miu_p
#define SHF_G3D_U_SEL        8

/*--------------------------------------------------------------------------------------*/
#define MSK_G3D_E_SEL        0x00000c00 //eclk_sel: 0: clk_g3d_e_p, 1: clk_miu_p
#define SHF_G3D_E_SEL        10

/*--------------------------------------------------------------------------------------*/
#define MSK_GIF_DONE         0x00001000 //GIF_DONE
#define SHF_GIF_DONE         12

/*--------------------------------------------------------------------------------------*/
#define MSK_OFIFO_ABORT      0x00002000 //OFIFO_ABORT
#define SHF_OFIFO_ABORT      13

/*--------------------------------------------------------------------------------------*/
#define MSK_IFIFO_FULL       0x00004000 //IFIFO_FULL
#define SHF_IFIFO_FULL       14

/*--------------------------------------------------------------------------------------*/
#define MSK_IFIFO_EMPTY      0x00008000 //IFIFO_EMPTY
#define SHF_IFIFO_EMPTY      15

/*--------------------------------------------------------------------------------------*/
#define MSK_BSADR_FULL       0x00010000 //BSADR_FULL
#define SHF_BSADR_FULL       16

/*--------------------------------------------------------------------------------------*/
#define MSK_PNG_BLK_DONE     0x00020000 //PNG_BLK_DONE
#define SHF_PNG_BLK_DONE     17

/*--------------------------------------------------------------------------------------*/
#define MSK_PNG_EOB          0x00040000 //PNG_EOB
#define SHF_PNG_EOB          18

/*--------------------------------------------------------------------------------------*/
#define MSK_GPD_TIME_OUT     0x00080000 //GPD_TIME_OUT
#define SHF_GPD_TIME_OUT     19

/*--------------------------------------------------------------------------------------*/
#define MSK_AR_GP_C0_RQ_PRI  0x03f00000 //GP_C chanel 0 request arbiter
										// GP_C: TR(01), CQR(10), USHICR(11)
										// default value: TR, CQR, USHICR
#define SHF_AR_GP_C0_RQ_PRI  20

/*--------------------------------------------------------------------------------------*/
#define MSK_AR_GP_C1_RQ_PRI  0xfc000000 //GP_C chanel 1 request arbiter
										// GP_C: TR(01), CQR(10), USHICR(11)
										// default value: TR, CQR, USHICR
#define SHF_AR_GP_C1_RQ_PRI  26


/****************************************************************************************/
#define REG_MULTI_BUF        0x8018    //debug offset = 0x30
/*--------------------------------------------------------------------------------------*/
#define MSK_PA_RIU_ACCESS_EN 0x00000001 //Single Global Register is written by
										// 0: command Q
										// 1: RIU
#define SHF_PA_RIU_ACCESS_EN 0

/*--------------------------------------------------------------------------------------*/
#define MSK_LATCH_VALUE_EN   0x00000002 //0: Release latch
										// 1: Latch CQ_ENGINE_RDPTR, GP_GBL0 and GP_GBL1 report value
#define SHF_LATCH_VALUE_EN   1

/*--------------------------------------------------------------------------------------*/
#define MSK_MULTI_BUF_EN     0x00000004 //Multiple frame buffer enable
#define SHF_MULTI_BUF_EN     2

/*--------------------------------------------------------------------------------------*/
#define MSK_MULTI_BUF_NUM    0x00000018 //0: 1 frame buffer
										// 1: 2 frame buffer
										// 2: 3 frame buffer
										// 3: reserved
#define SHF_MULTI_BUF_NUM    3

/*--------------------------------------------------------------------------------------*/
#define MSK_MULTI_BUF_MD     0x00000020 //Multiple frame buffer mode
										// 0: display
										// 1: mcu
#define SHF_MULTI_BUF_MD     5

/*--------------------------------------------------------------------------------------*/
#define MSK_MULTI_BUF_BASE_LATCH 0x00000040 //0: Release latch
										// 1: Latch MULTI_BUF_BASE report value
#define SHF_MULTI_BUF_BASE_LATCH 6

/*--------------------------------------------------------------------------------------*/
#define MSK_MULTI_BUF_BASE   0xffffff80 //multiple frame buffer base address [26:3],  read only ;
#define SHF_MULTI_BUF_BASE   7

/****************************************************************************************/
#define REG_CQ_STATUS        0x801c    //debug offset = 0x38
/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_FIRE          0x00000001 //Command Q fire (through RIU bus only)
#define SHF_CQ_FIRE          0

/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_STATUS        0x000ffffe //[1]: 3D engine idle
										// [2]: cmdQ idle in mclk
										// [3]: cmdQ idle in eclk
										// [4]: parser vtx dec idle
										// [5]: VS idle
										// [6]: clip idle
										// [7]: setup idle
										// [8]: xyshading idle
										// [9]: tile bridge idle
										// [10]: TZ idle
										// [11]: PS idle
										// [12]: AA idle
										// [13]: ZS idle
										// [14]: texture idle
										// [15]: zpost idle
										// [16]: zcache idle
										// [17]: cpost idle
										// [18]: ccache idle
										// [19]: arbiter idle
#define SHF_CQ_STATUS        1

#define MSK_3D_IDLE             (1<<1)
#define MSK_CMDQ_MCLK_IDLE      (1<<2)
#define MSK_CMDQ_ECLK_IDLE      (1<<3)
#define MSK_PARSER_IDLE         (1<<4)
#define MSK_VS_IDLE             (1<<5)
#define MSK_CLIP_IDLE           (1<<6)
#define MSK_SETUP_IDLE          (1<<7)
#define MSK_XYSHADE_IDLE        (1<<8)
#define MSK_TILE_BRIDGE_IDLE    (1<<9)
#define MSK_TZ_IDLE             (1<<10)
#define MSK_PS_IDLE             (1<<11)
#define MSK_AA_IDLE             (1<<12)
#define MSK_ZS_IDLE             (1<<13)
#define MSK_TEX_IDLE            (1<<14)
#define MSK_ZPOST_IDLE          (1<<15)
#define MSK_ZCACHE_IDLE         (1<<16)
#define MSK_CPOST_IDLE          (1<<17)
#define MSK_CCACHE_IDLE         (1<<18)
#define MSK_ARBITER_IDLE        (1<<19)

/*--------------------------------------------------------------------------------------*/
#define MSK_OFIFO_DONE       0x01000000 //PG last image  to arbiter (read only)
#define SHF_OFIFO_DONE       24

/*--------------------------------------------------------------------------------------*/
#define MSK_G3D_U_GATE       0x06000000 //clk_g3d_uclk gating
#define SHF_G3D_U_GATE       25


/****************************************************************************************/
#define REG_CQ_BASE          0x8020    //debug offset = 0x40
/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_BASE          0x0ffffffc //Command Q base address, require  8 byte alignment
										// Command Q size : 4Mbyte (through RIU bus only)
										// Support 64M DRAM (bit 26 is useless)
#define SHF_CQ_BASE          2


/****************************************************************************************/
#define REG_CQ_RD_PTR        0x8024    //debug offset = 0x48
/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_ENGINE_RDPTR  0x003fffff //Command Q read pointer, update by Command Q  engine
										// Command Q size : 4Mbyte (through RIU bus only)
#define SHF_CQ_ENGINE_RDPTR  0


/****************************************************************************************/
#define REG_CQ_WT_PTR        0x8028    //debug offset = 0x50
/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_SW_WRTPTR     0x003fffff //Command Q write pointer, update by SW(ARM CPU),
										// Command Q size : 4Mbyte (through RIU bus only)
#define SHF_CQ_SW_WRTPTR     0


/****************************************************************************************/
#define REG_CQ_REQ_TH        0x802c    //debug offset = 0x58
/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_REQ_THRESH    0x0000001f //HW FIFO request threshold (unit 64 bits)
#define SHF_CQ_REQ_THRESH    0

/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_PRIOR_HI_THRESH 0x00001f00 //when REG_CQ_PRIOR_MODE = 2, CQ2AR_RDRQ_PRIORITY will be assert if command FIFO empty count >= REG_CQ_PRIOR_HI_THRESH
#define SHF_CQ_PRIOR_HI_THRESH 8

/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_PRIOR_MODE    0x00006000 //Command Q read request priority mode.
										// 0: force priority (CQ2AR_RDRQ_PRIORITY) to 0,
										// 1: force priority (CQ2AR_RDRQ_PRIORITY) t0 1,
										// 2: assert priority (CQ2AR_RDRQ_PRIORITY) based on  REG_CQ_HI_PRIOR_THRESH
#define SHF_CQ_PRIOR_MODE    13

/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_SW_CQSIZE     0x00070000 //Dram Command Q Size:
										// 0: 16KB
										// 1: 32KB
										// 2: 64KB
										// 3: 128KB
										// 4: 512KB
										// 5: 1M
										// 6: 2M
										// 7: 4M
#define SHF_CQ_SW_CQSIZE     16

/*--------------------------------------------------------------------------------------*/
#define MSK_CQ_SWRST         0x00080000 //CQ SW reset
#define SHF_CQ_SWRST         19

/****************************************************************************************/
#define REG_G3DPLL_CTRL0     0x8088    //debug offset = 0x110
/*--------------------------------------------------------------------------------------*/
#define MSK_G3DPLL_CTRL0     0x3fffffff //G3DPLL control0
#define SHF_G3DPLL_CTRL0     0

/****************************************************************************************/
#define REG_G3DPLL_CTRL1     0x808c    //debug offset = 0x118
/*--------------------------------------------------------------------------------------*/
#define MSK_G3DPLL_CTRL1     0x007fffff //G3DPLL control1
#define SHF_G3DPLL_CTRL1     0


/****************************************************************************************/
#define REG_GP_GBL0          0x80b4    //debug offset = 0x168
/*--------------------------------------------------------------------------------------*/
#define MSK_GP_GBL0          0xffffffff //General Purpose Global
#define SHF_GP_GBL0          0


/****************************************************************************************/
#define REG_GP_GBL1          0x80b8    //debug offset = 0x170
/*--------------------------------------------------------------------------------------*/
#define MSK_GP_GBL1          0xffffffff //General Purpose Global
#define SHF_GP_GBL1          0


/****************************************************************************************/
#define REG_CH2MEM_BANK_SEL  0x8310    //debug offset = 0x620
/*--------------------------------------------------------------------------------------*/
#define MSK_CH2MEM_BANK_SEL  0x00000001 //2nd channel miu bank sel
#define SHF_CH2MEM_BANK_SEL  0




/* g3d */
#define REG_MIPS_BASE                   0xBF200000
#define REG_G3D_BASE                    0xBF221000

#define REG_LOW(x)      ((x & 0xFF) * 2)
#define REG_HIGH(x)     ((x & 0xFF) * 2 + 4)

#define REG_WRITE(reg, value)                                                       \
    *(volatile U16*)(REG_G3D_BASE + REG_LOW(reg))   = (U16)(value & 0x0000ffff);  \
    *(volatile U16*)(REG_G3D_BASE + REG_HIGH(reg))  = (U16)(value >> 16);

#define REG_MASK_WRITE(reg, value, mask)                                \
{                                                                       \
    U32 data;                                                           \
    data = *(volatile U16*)(REG_G3D_BASE + REG_LOW(reg));             \
    data |= (*(volatile U16*)(REG_G3D_BASE + REG_HIGH(reg))) << 16;   \
    data = (data & ~mask) | (value & mask);                             \
    REG_WRITE(reg, data);                                               \
}

#define REG_READ(reg)                                                   \
    (((*(volatile U32*)(REG_G3D_BASE + REG_LOW(reg))) & 0x0000ffff) | \
     ((*(volatile U32*)(REG_G3D_BASE + REG_HIGH(reg))) << 16))

#define REG_LATCH_READ(reg, value)                                                              \
{                                                                                               \
    *(volatile U16*)(REG_G3D_BASE + REG_LOW(REG_MULTI_BUF)) |= (U16)MSK_LATCH_VALUE_EN;       \
    value = (((*(volatile U32*)(REG_G3D_BASE + REG_LOW(reg))) & 0x0000ffff) |                 \
             ((*(volatile U32*)(REG_G3D_BASE + REG_HIGH(reg))) << 16));                       \
    *(volatile U16*)(REG_G3D_BASE + REG_LOW(REG_MULTI_BUF)) &= (U16)(~MSK_LATCH_VALUE_EN);    \
}


#endif


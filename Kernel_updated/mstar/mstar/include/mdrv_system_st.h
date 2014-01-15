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

#ifndef __DRV_SYSTEM_ST_H__
#define __DRV_SYSTEM_ST_H__

//------------------------------------------------------------------------------
// Data structure
//------------------------------------------------------------------------------
typedef struct IO_SYS_PANEL_INFO_s
{
    U32* pPanelInfo;
    U16  u16Len;
} IO_SYS_PANEL_INFO_t, *PIO_SYS_PANEL_INFO_t;

typedef struct IO_SYS_PANEL_Res_s
{
    U16  u16Width;
    U16  u16Height;
} IO_SYS_PANEL_GET_RES_t, *PIO_SYS_PANEL_GET_RES_t;

typedef struct IO_SYS_BOARD_INFO_s
{
    U32* pu32BoardInfo;
    U16  u16Len;
} IO_SYS_BOARD_INFO_t, *PIO_SYS_BOARD_INFO_t;

typedef struct IO_SYS_GENERAL_REG_s
{
    U16  u16Reg;
    U8   u8Value;
} IO_SYS_GENERAL_REG_t;

typedef struct IO_SYS_AEONBIN_INFO_s
{
	U8* pu8AeonStart;
	U32 u32Len;
	BOOL bRet;
} IO_SYS_AEONBIN_INFO_t;

/* PCMCIA_MAP_IOC_INFO */
typedef struct
{
    U16                       u16Addr;
    U8                         u8Value;
    U8                         u8Type;     // 1: AttribMem, 2: IOMem
    U16                  u16DataLen;
    U8 *                 u8pReadBuffer;
    U8 *                 u8pWriteBuffer;
} PCMCIA_Map_Info_t;

typedef enum
{
    RELOAD_AEON_STOP,
    RELOAD_AEON_RESTART
} AEON_CONTROL ;

typedef enum
{
    // Analog port
    INPUT_SRC_VGA,
    INPUT_SRC_YPBPR_1,
    INPUT_SRC_YPBPR_2,

    // Digital port
    INPUT_SRC_ATV,
    INPUT_SRC_CVBS_1,
    INPUT_SRC_CVBS_2,
    INPUT_SRC_CVBS_3,

    INPUT_SRC_SVIDEO_1,
    INPUT_SRC_SVIDEO_2,

    INPUT_SRC_SCART_1,
    INPUT_SRC_SCART_2,

    // HDMI port
    INPUT_SRC_HDMI_A,
#if 1 
    INPUT_SRC_HDMI_B,
#else
    INPUT_SRC_HDMI_B1,
    INPUT_SRC_HDMI_B2,
    INPUT_SRC_HDMI_B3,
#endif
    INPUT_SRC_HDMI_C,

    // MVD port
    INPUT_SRC_DTV,
    INPUT_SRC_DTV_MLINK,

    INPUT_SRC_STORAGE,              ///< input source is Storage

    INPUT_SRC_NUM,
    INPUT_SRC_NONE = INPUT_SRC_NUM
} SYS_INPUT_SOURCE_e;


typedef struct IO_SYS_SPI_s
{
   U32 u32Start;
   U32 u32Len;
   U8 *u8data;		
} IO_SYS_SPI_t;

typedef struct
{
        unsigned int                LX_MEM_ADDR;
        unsigned int                LX_MEM_LENGTH;
        unsigned int                LX_MEM2_ADDR;
        unsigned int                LX_MEM2_LENGTH;
        unsigned int                EMAC_ADDR;
        unsigned int                EMAC_LENGTH;
        unsigned int                DRAM_ADDR;
        unsigned int                DRAM_LENGTH;
        unsigned int                BB_ADDR;
        unsigned int                BB_LENGTH;
        unsigned int                MPOOL_MEM_ADDR;
        unsigned int                MPOOL_MEM_LENGTH;
        unsigned int                G3D_MEM0_ADDR;
        unsigned int                G3D_MEM0_LENGTH;
        unsigned int                G3D_MEM1_ADDR;
        unsigned int                G3D_MEM1_LENGTH;
        unsigned int                G3D_CMDQ_ADDR;
        unsigned int                G3D_CMDQ_LENGTH;
} IO_Sys_Info_t;

#endif // __DRV_SYSTEM_ST_H__


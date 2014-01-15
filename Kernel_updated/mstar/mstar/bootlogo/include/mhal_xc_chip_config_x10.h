////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2009-2010 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MHAL_XC_CONFIG_H
#define MHAL_XC_CONFIG_H

//-------------------------------------------------------------------------------------------------
//  A5
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Chip Configuration
//-------------------------------------------------------------------------------------------------
#define MAX_WINDOW_NUM          (2)
#define MAX_FRAME_NUM_IN_MEM    (4) // Progressive
#define MAX_FIELD_NUM_IN_MEM    (16) // Interlace
#define NUM_OF_DIGITAL_DDCRAM   (1)

#define SCALER_LINE_BUFFER_MAX  (1920UL)
#define MST_LINE_BFF_MAX        MAX(1680, SCALER_LINE_BUFFER_MAX)

#define SUB_MAIN_LINEOFFSET_GUARD_BAND  16
#define SUB_SCALER_LINE_BUFFER_MAX      960UL - SUB_MAIN_LINEOFFSET_GUARD_BAND
#define SUB_MST_LINE_BFF_MAX            SUB_SCALER_LINE_BUFFER_MAX

#define MS_3D_LINE_BFF_MAX      (1440UL)

// MIU Word (Bytes)
#define BYTE_PER_WORD           (32)  // MIU 128: 16Byte/W, MIU 256: 32Byte/W
#define OFFSET_PIXEL_ALIGNMENT  (64)


#define FRC_BYTE_PER_WORD           32

#define DEFAULT_STEP_P          4 //conservative step value
#define DEFAULT_STEP_I          ((DEFAULT_STEP_P*DEFAULT_STEP_P)/2)
#define STEP_P                  2 //recommended step value -> more faster fpll(T3)
#define STEP_I                  ((STEP_P*STEP_P)/2)

#define F2_WRITE_LIMIT_EN   BIT(31)
#define F2_WRITE_LIMIT_MIN  BIT(30)

#define F1_WRITE_LIMIT_EN   BIT(31)
#define F1_WRITE_LIMIT_MIN  BIT(30)

#define F2_V_WRITE_LIMIT_EN    BIT(15)
#define F1_V_WRITE_LIMIT_EN    BIT(15)

#define ADC_MAX_CLK                     (3500)

#define SUPPORTED_XC_INT        ((1UL << SC_INT_VSINT) |            \
                                 (1UL << SC_INT_F2_VTT_CHG) |       \
                                 (1UL << SC_INT_F1_VTT_CHG) |       \
                                 (1UL << SC_INT_F2_VS_LOSE) |       \
                                 (1UL << SC_INT_F1_VS_LOSE) |       \
                                 (1UL << SC_INT_F2_JITTER) |        \
                                 (1UL << SC_INT_F1_JITTER) |        \
                                 (1UL << SC_INT_F2_IPVS_SB) |       \
                                 (1UL << SC_INT_F1_IPVS_SB) |       \
                                 (1UL << SC_INT_F2_IPHCS_DET) |     \
                                 (1UL << SC_INT_F1_IPHCS_DET) |     \
                                 (1UL << SC_INT_F2_HTT_CHG) |       \
                                 (1UL << SC_INT_F1_HTT_CHG) |       \
                                 (1UL << SC_INT_F2_HS_LOSE) |       \
                                 (1UL << SC_INT_F1_HS_LOSE) |       \
                                 (1UL << SC_INT_F2_CSOG) |          \
                                 (1UL << SC_INT_F1_CSOG) |          \
                                 (1UL << SC_INT_F2_ATP_READY) |     \
                                 (1UL << SC_INT_F1_ATP_READY))


//These table definition is from SC_BK0 spec.
//Because some chip development is different, it need to check and remap when INT function is used
#define IRQ_INT_START         3
#define IRQ_INT_RESERVED1     IRQ_INT_START

#define IRQ_INT_VSINT         4
#define IRQ_INT_F2_VTT_CHG    5
#define IRQ_INT_F1_VTT_CHG    6
#define IRQ_INT_F2_VS_LOSE    7
#define IRQ_INT_F1_VS_LOSE    8
#define IRQ_INT_F2_JITTER     9
#define IRQ_INT_F1_JITTER     10
#define IRQ_INT_F2_IPVS_SB    11
#define IRQ_INT_F1_IPVS_SB    12
#define IRQ_INT_F2_IPHCS_DET  13
#define IRQ_INT_F1_IPHCS_DET  14

#define IRQ_INT_PWM_RP_L_INT  15
#define IRQ_INT_PWM_FP_L_INT  16
#define IRQ_INT_F2_HTT_CHG    17
#define IRQ_INT_F1_HTT_CHG    18
#define IRQ_INT_F2_HS_LOSE    19
#define IRQ_INT_F1_HS_LOSE    20
#define IRQ_INT_PWM_RP_R_INT  21
#define IRQ_INT_PWM_FP_R_INT  22
#define IRQ_INT_F2_CSOG       23
#define IRQ_INT_F1_CSOG       24
#define IRQ_INT_F2_RESERVED2  25
#define IRQ_INT_F1_RESERVED2  26
#define IRQ_INT_F2_ATP_READY  27
#define IRQ_INT_F1_ATP_READY  28
#define IRQ_INT_F2_RESERVED3  29
#define IRQ_INT_F1_RESERVED3  30

//-------------------------------------------------------------------------------------------------
//  Chip Feature
//-------------------------------------------------------------------------------------------------

/* 8 frame mode for progessive */
#define _8FRAME_BUFFER_PMODE_SUPPORTED      1
/* 4 frame mode for progessive */
#define _4FRAME_BUFFER_PMODE_SUPPORTED      1
/* 3 frame mode for progessive */
#define _3FRAME_BUFFER_PMODE_SUPPORTED      1
/* Linear mode */
#define _LINEAR_ADDRESS_MODE_SUPPORTED      0

/*
   Field-packing ( Customized name )
   This is a feature in M10. M10 only needs one IPM buffer address. (Other chips need two or three
   IPM buffer address). We show one of memory format for example at below.

   Block :       Y0      C0      L       M        Y1       C1
   Each block contain 4 fields (F0 ~ F3) and each fields in one block is 64 bits
   Y0 has 64 * 4 bits ( 8 pixel for each field ).
   Y1 has 64 * 4 bits ( 8 pixel for each field ).
   So, in this memory format, pixel alignment is 16 pixels (OFFSET_PIXEL_ALIGNMENT = 16).
   For cropping, OPM address offset have to multiple 4.
*/
#define _FIELD_PACKING_MODE_SUPPORTED       1
/* Because fix loop_div, lpll initial set is different between singal port and dual port */
#define _FIX_LOOP_DIV_SUPPORTED             1

// You can only enable ENABLE_8_FIELD_SUPPORTED or ENABLE_16_FIELD_SUPPORTED. (one of them)
// 16 field mode include 8 field configurion in it. ENABLE_8_FIELD_SUPPORTED is specital case in T7
#define ENABLE_8_FIELD_SUPPORTED            0
#define ENABLE_16_FIELD_SUPPORTED           1
#define ENABLE_OPM_WRITE_SUPPORTED          1
#define ENABLE_YPBPR_PRESCALING_TO_ORIGINAL 0
#define ENABLE_VD_PRESCALING_TO_DOT75       0
#define ENABLE_NONSTD_INPUT_MCNR            0
#define ENABLE_REGISTER_SPREAD              1

#define ENABLE_REQUEST_FBL                  0
#define DELAY_LINE_SC_UP                    7
#define DELAY_LINE_SC_DOWN                  8

#define SUPPORT_IMMESWITCH                  1
#define SUPPORT_DVI_AUTO_EQ                 1

// Special frame lock means that the frame rates of input and output are the same in HW design spec.
#define SUPPORT_SPECIAL_FRAMELOCK           FALSE

#define LD_ENABLE                           0

#define FRC_INSIDE                          TRUE
#define _A5_Bringup_                        TRUE

//-------------------------------------------------------------------------------------------------
//  Register base
//-------------------------------------------------------------------------------------------------
#define BK_REG_L( x, y )            ((x) | (((y) << 1)))
#define BK_REG_H( x, y )            (((x) | (((y) << 1))) + 1)


// PM
#define REG_DDC_BASE                0x000400
#define REG_PM_SLP_BASE             0x000E00
#define REG_PM_SLEEP_BASE           REG_PM_SLP_BASE//0x0E00//alex_tung

// NONPM
#define REG_MIU0_BASE               0x1200
#define REG_MIU1_BASE               0x0600
#define REG_CHIPTOP_BASE            0x0B00  // 0x1E00 - 0x1EFF
#define REG_UHC0_BASE               0x2400
#define REG_UHC1_BASE               0x0D00
#define REG_ADC_ATOP_BASE           0x2500  // 0x2500 - 0x25FF
#define REG_ADC_DTOP_BASE           0x2600  // 0x2600 - 0x26EF
#define REG_HDMI_BASE               0x2700  // 0x2700 - 0x27FF
#define REG_HDMI2_BASE              0x1A00
#define REG_IPMUX_BASE              0x2E00
#if ENABLE_REGISTER_SPREAD
#define REG_SCALER_BASE             0x130000
#else
#define REG_SCALER_BASE             0x102F00
#endif
#define REG_LPLL_BASE               0x303200
#define REG_AFEC_BASE               0x103500
#define REG_COMB_BASE               0x103600

#define REG_DVI_ATOP_BASE           0x110900
#define REG_DVI_DTOP_BASE           0x110A00
#define REG_DVI_EQ_BASE             0x110A80     // EQ started from 0x80
#define REG_HDCP_BASE               0x110AC0     // HDCP started from 0xC0
#define REG_ADC_DTOPB_BASE          0x111200     // ADC DTOPB
#define REG_DVI_ATOP1_BASE          0x113200
#define REG_DVI_DTOP1_BASE          0x113300
#define REG_DVI_EQ1_BASE            0x113380     // EQ started from 0x80
#define REG_HDCP1_BASE              0x1133C0     // HDCP started from 0xC0
#define REG_DVI_ATOP2_BASE          0x113400
#define REG_DVI_DTOP2_BASE          0x113500
#define REG_DVI_EQ2_BASE            0x113580     // EQ started from 0x80
#define REG_HDCP2_BASE              0x1135C0     // HDCP started from 0xC0
#define REG_DVI_PS_BASE             0x113600     // DVI power saving
#define REG_DVI_PS1_BASE            0x113640     // DVI power saving1
#define REG_DVI_PS2_BASE            0x113680     // DVI power saving2
#define REG_DVI_PS3_BASE            0x1136C0     // DVI power saving3
#define REG_DVI_DTOP3_BASE          0x113700
#define REG_DVI_EQ3_BASE            0x113780     // EQ started from 0x80
#define REG_HDCP3_BASE              0x1137C0     // HDCP started from 0xC0

#define REG_CHIP_ID_MAJOR           0x1ECC
#define REG_CHIP_ID_MINOR           0x1ECD
#define REG_CHIP_VERSION            0x1ECE
#define REG_CHIP_REVISION           0x1ECF

#define REG_CLKGEN0_BASE            0x100B00
#define REG_CLKGEN1_BASE            0x103300

///URSA Area
#define REG_FRC_BANK_BASE           (0x300000)

#define L_CLKGEN0(x)                BK_REG_L(REG_CLKGEN0_BASE, x)
#define H_CLKGEN0(x)                BK_REG_H(REG_CLKGEN0_BASE, x)
#define L_CLKGEN1(x)                BK_REG_L(REG_CLKGEN1_BASE, x)
#define H_CLKGEN1(x)                BK_REG_H(REG_CLKGEN1_BASE, x)



// store bank
#define LPLL_BK_STORE     \
        MS_U8 u8LPLL_Bank;      \
        u8LPLL_Bank = MDrv_ReadByte(REG_LPLL_BASE)

// restore bank
#define LPLL_BK_RESTORE     MDrv_WriteByte(REG_LPLL_BASE, u8LPLL_Bank)

// switch bank
#define LPLL_BK_SWITCH(_x_) MDrv_WriteByte(REG_LPLL_BASE, _x_)

//------------------------------------------------------------------------------
// Register configure
//------------------------------------------------------------------------------

#define REG_CKG_FICLK_F1        (REG_CHIPTOP_BASE + 0xA2 ) // scaling line buffer, set to fclk if post scaling, set to idclk is pre-scaling
    #define CKG_FICLK_F1_GATED      BIT(0)
    #define CKG_FICLK_F1_INVERT     BIT(1)
    #define CKG_FICLK_F1_MASK       BMASK(3:2)
    #define CKG_FICLK_F1_IDCLK1     (0 << 2)
    #define CKG_FICLK_F1_FLK        (1 << 2)
    //#define CKG_FICLK_F1_XTAL       (3 << 2)

#define REG_CKG_FICLK_F2        (REG_CHIPTOP_BASE + 0xA3 ) // scaling line buffer, set to fclk if post scaling, set to idclk is pre-scaling
    #define CKG_FICLK_F2_GATED      BIT(0)
    #define CKG_FICLK_F2_INVERT     BIT(1)
    #define CKG_FICLK_F2_MASK      BMASK(3:2)
    #define CKG_FICLK_F2_IDCLK2     (0 << 2)
    #define CKG_FICLK_F2_FLK        (1 << 2)
    //#define CKG_FICLK_F2_XTAL       (3 << 2)

#define REG_CKG_FODCLK          (REG_CHIPTOP_BASE + 0xA4 ) // over drive clock
    #define CKG_FODCLK_GATED        BIT(0)
    #define CKG_FODCLK_INVERT       BIT(1)
    #define CKG_FODCLK_MASK         BMASK(3:2)
    #define CKG_FODCLK_FCLK_P       (0 << 2)
    #define CKG_FODCLK_ODCLK_P      (1 << 2)
    #define CKG_FODCLK_XTAL         (3 << 2)

#define REG_CKG_FCLK            (REG_CHIPTOP_BASE + 0xA5 ) // after memory, before fodclk
    #define CKG_FCLK_GATED          BIT(0)
    #define CKG_FCLK_INVERT         BIT(1)
    #define CKG_FCLK_MASK           BMASK(5:2)
    #define CKG_FCLK_170MHZ         (0 << 2)
    #define CKG_FCLK_CLK_MIU        (1 << 2)
    #define CKG_FCLK_CLK_ODCLK      (2 << 2)
    #define CKG_FCLK_216MHZ         (3 << 2)
    #define CKG_FCLK_192MHZ         (4 << 2)
    #define CKG_FCLK_SCPLL          (5 << 2)
    #define CKG_FCLK_0              (6 << 2)
    #define CKG_FCLK_XTAL           (7 << 2)
    #define CKG_FCLK_XTAL_          (8 << 2)

#define REG_CKG_IDCLK0          (REG_CHIPTOP_BASE + 0xA8 ) // off-line detect idclk
    #define CKG_IDCLK0_GATED        BIT(0)
    #define CKG_IDCLK0_INVERT       BIT(1)
    #define CKG_IDCLK0_MASK         BMASK(5:2)
    #define CKG_IDCLK0_CLK_ADC      (0 << 2)
    #define CKG_IDCLK0_CLK_DVI      (1 << 2)
    #define CKG_IDCLK0_CLK_VD       (2 << 2)
    #define CKG_IDCLK0_CLK_DC0      (3 << 2)
    #define CKG_IDCLK0_ODCLK        (4 << 2)
    #define CKG_IDCLK0_0            (5 << 2)
    #define CKG_IDCLK0_CLK_VD_ADC   (6 << 2)
    #define CKG_IDCLK0_00           (7 << 2)               // same as 5 --> also is 0
    #define CKG_IDCLK0_XTAL         (8 << 2)

#define REG_CKG_IDCLK1          (REG_CHIPTOP_BASE + 0xA9 ) // sub main window idclk
    #define CKG_IDCLK1_GATED        BIT(0)
    #define CKG_IDCLK1_INVERT       BIT(1)
    #define CKG_IDCLK1_MASK         BMASK(5:2)
    #define CKG_IDCLK1_CLK_ADC      (0 << 2)
    #define CKG_IDCLK1_CLK_DVI      (1 << 2)
    #define CKG_IDCLK1_CLK_VD       (2 << 2)
    #define CKG_IDCLK1_CLK_DC0      (3 << 2)
    #define CKG_IDCLK1_ODCLK        (4 << 2)
    #define CKG_IDCLK1_0            (5 << 2)
    #define CKG_IDCLK1_CLK_VD_ADC   (6 << 2)
    #define CKG_IDCLK1_00           (7 << 2)               // same as 5 --> also is 0
    #define CKG_IDCLK1_XTAL         (8 << 2)

#define REG_CKG_IDCLK2          (REG_CHIPTOP_BASE + 0xAA ) // main window idclk
    #define CKG_IDCLK2_GATED        BIT(0)
    #define CKG_IDCLK2_INVERT       BIT(1)
    #define CKG_IDCLK2_MASK         BMASK(5:2)
    #define CKG_IDCLK2_CLK_ADC      (0 << 2)
    #define CKG_IDCLK2_CLK_DVI      (1 << 2)
    #define CKG_IDCLK2_CLK_VD       (2 << 2)
    #define CKG_IDCLK2_CLK_DC0      (3 << 2)
    #define CKG_IDCLK2_ODCLK        (4 << 2)
    #define CKG_IDCLK2_0            (5 << 2)
    #define CKG_IDCLK2_CLK_VD_ADC   (6 << 2)
    #define CKG_IDCLK2_00           (7 << 2)               // same as 5 --> also is 0
    #define CKG_IDCLK2_XTAL         (8 << 2)

#define REG_CKG_IDCLK3          (REG_CHIPTOP_BASE + 0xAB )
    #define CKG_IDCLK3_GATED        BIT(0)
    #define CKG_IDCLK3_INVERT       BIT(1)
    #define CKG_IDCLK3_MASK         BMASK(5:2)
    #define CKG_IDCLK3_CLK_ADC      (0 << 2)
    #define CKG_IDCLK3_CLK_DVI      (1 << 2)
    #define CKG_IDCLK3_CLK_VD       (2 << 2)
    #define CKG_IDCLK3_CLK_DC0      (3 << 2)
    #define CKG_IDCLK3_ODCLK        (4 << 2)
    #define CKG_IDCLK3_0            (5 << 2)
    #define CKG_IDCLK3_CLK_VD_ADC   (6 << 2)
    #define CKG_IDCLK3_00           (7 << 2)               // same as 5 --> also is 0
    #define CKG_IDCLK3_XTAL         (8 << 2)

#define REG_CKG_ODCLK           (REG_CHIPTOP_BASE + 0xA6) // output dot clock, usually select LPLL, select XTAL when debug
    #define CKG_ODCLK_GATED          0:0
    #define CKG_ODCLK_INVERT         1:1
    #define CKG_ODCLK_MASK           3:2
    #define CKG_ODCLK_CLK_SC_PLL        (0 << 2)
    #define CKG_ODCLK_27M           (2 << 2)
    #define CKG_ODCLK_CLK_LPLL     (3)
    #define CKG_ODCLK_XTAL          (1 << 2)

#define REG_CKG_BT656               (REG_CHIPTOP_BASE + 0xA7)
    #define CKG_BT656_GATED        0:0
    #define CKG_BT656_INVERT       1:1
    #define CKG_BT656_MASK          3:2
    #define CKG_BT656_CLK_SC_PLL    (0 << 2)
    #define CKG_BT656_CLK_LPLL_DIV_2 (1 << 2)
    #define CKG_BT656_27M           (2 << 2)
    #define CKG_BT656_CLK_LPLL      (3 << 2)

#define REG_CKG_DACA2           (REG_CHIPTOP_BASE + 0x4C ) //DAC out
    #define CKG_DACA2_GATED         (0:0)
    #define CKG_DACA2_INVERT        (1:1)
    #define CKG_DACA2_MASK          (3:2)
    #define CKG_DACA2_VIF_CLK       (0)
    #define CKG_DACA2_VD_CLK        (1)
    #define CKG_DACA2_EXT_TEST_CLK  (2)
    #define CKG_DACA2_XTAL          (3)

#define REG_CKG_DACB2           (REG_CHIPTOP_BASE + 0x4D ) //DAC out
    #define CKG_DACB2_GATED         BIT(0)
    #define CKG_DACB2_INVERT        BIT(1)
    #define CKG_DACB2_MASK          BMASK(3:2)
    #define CKG_DACB2_VIF_CLK       (0 << 2)
    #define CKG_DACB2_VD_CLK        (1 << 2)
    #define CKG_DACB2_EXT_TEST_CLK  (2 << 2)
    #define CKG_DACB2_XTAL          (3 << 2)


#define REG_DE_ONLY_F2          (REG_CHIPTOP_BASE + 0xA0 )
    #define DE_ONLY_F2_MASK         BIT(2)

#define REG_DE_ONLY_F1          (REG_CHIPTOP_BASE + 0xA0 )
    #define DE_ONLY_F1_MASK         BIT(1)

#define REG_DE_ONLY_F0          (REG_CHIPTOP_BASE + 0xA0 )
    #define DE_ONLY_F0_MASK         BIT(0)

#define REG_PM_DVI_SRC_CLK      (REG_PM_SLP_BASE +  0x96)
#define REG_PM_DDC_CLK          (REG_PM_SLP_BASE +  0x42)

#define REG_CLKGEN0_50_L        (REG_CHIPTOP_BASE + 0xA0)
#define REG_CLKGEN0_51_L        (REG_CHIPTOP_BASE + 0xA2)
#define REG_CLKGEN0_53_L        (REG_CHIPTOP_BASE + 0xA6)
#define REG_CLKGEN0_57_L            (REG_CHIPTOP_BASE + 0xAE)
#define REG_CLKGEN0_58_L            (REG_CHIPTOP_BASE + 0xB0)
#define REG_CLKGEN0_5E_L            (REG_CHIPTOP_BASE + 0xBC)

#define CLK_SRC_IDCLK2  0
#define CLK_SRC_FCLK    1
#define CLK_SRC_XTAL    3

#define MIU0_G0_REQUEST_MASK    (REG_MIU0_BASE + 0x46)
#define MIU0_G1_REQUEST_MASK    (REG_MIU0_BASE + 0x66)
#define MIU0_G2_REQUEST_MASK    (REG_MIU0_BASE + 0x86)
#define MIU0_G3_REQUEST_MASK    (REG_MIU0_BASE + 0xA6)

#define MIU1_G0_REQUEST_MASK    (REG_MIU1_BASE + 0x46)
#define MIU1_G1_REQUEST_MASK    (REG_MIU1_BASE + 0x66)
#define MIU1_G2_REQUEST_MASK    (REG_MIU1_BASE + 0x86)
#define MIU1_G3_REQUEST_MASK    (REG_MIU1_BASE + 0xA6)

#define MIU_SC_G0REQUEST_MASK   (0x0000)
#define MIU_SC_G1REQUEST_MASK   (0x0000)
#define MIU_SC_G2REQUEST_MASK   (0x0000)
#define MIU_SC_G3REQUEST_MASK   (0x01C0)

#define VOP_DE_HSTART_MASK      (0x1FFF) //BK_10_04
#define VOP_DE_HEND_MASK        (0x1FFF) //BK_10_05
#define VOP_VSYNC_END_MASK      (0x0FFF) //BK_10_03
#define VOP_DISPLAY_HSTART_MASK (0x1FFF) //BK_10_08
#define VOP_DISPLAY_HEND_MASK   (0x1FFF) //BK_10_09

#endif /* MHAL_XC_CONFIG_H */


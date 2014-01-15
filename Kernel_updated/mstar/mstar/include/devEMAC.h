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
/// @file   EMAC.h
/// @author MStar Semiconductor Inc.
/// @brief  EMAC Driver Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// Linux EMAC.c start
// -----------------------------------------------------------------------------

#ifndef __DRV_EMAC__
#define __DRV_EMAC__

#define TRUE                1
#define FALSE              0
// Compiler Switches
//#define TESTING
#define REG_BIT_MAP
#define USE_TASK							1			// 1:Yes, 0:No
#define URANUS_ETHER_ADDR_CONFIGURABLE	/* MAC address can be changed? */

#define EMAC_SPEED_100     100
//#ifndef __ASSEMBLY__

#define CONFIG_EMAC_MOA 1   // System Type
#define E_IRQL_START         0
#define E_IRQ_EMAC            (E_IRQL_START+9)

struct net_device Uranus_dev;

static u32 phyaddr = 0xA0667788;

#define EMAC_ALLFF                                   0xFFFFFFFF
#define EMAC_ABSO_MEM_BASE                  0xA0000000//EMAC_ABSO_MEM_BASE                  0xA0000000
#define EMAC_ABSO_PHY_BASE                  0x80000000//EMAC_ABSO_MEM_BASE
#define EMAC_ABSO_MEM_SIZE                   0x16000//(48 * 1024)     // More than: (32 + 16(0x3FFF)) KB
#define EMAC_MEM_SIZE_SQU                     4    // More than: (32 + 16(0x3FFF)) KB
#define EMAC_BUFFER_MEM_SIZE               0x0004000
#define EMAC_MAX_TX_QUEUE                    30
// URANUS here:
#if (!USE_TASK) // MEM_BASE_ADJUSTMENT ......................................
#define RAM_ADDR_BASE 						0xA0000000
#define REG_ADDR_BASE						0xBF804000 		// The register address base. Depends on system define.
#define RX_BUFFER_SEL						0x0003//0x0009			// 0x0=2KB,0x1=4KB,0x2=8KB,0x3=16KB, 0x09=1MB
#define RX_BUFFER_BASE						0x00000000 		// ==0xA0000000 ~~ 0xA0004000 (Max: 16 KB)
#define RBQP_BASE							0x00004000 		// ==0xA0004000 ~~ 0xA0005FFF for MAX 1024 descriptors
#define TX_BUFFER_BASE						0x00006000 	    // ==0xA0006000 ~~ ????????
#define TX_SKB_BASE                                           0x00006100
#define RX_FRAME_ADDR						0x00007000 	    // Software COPY&STORE one RX frame. Size is not defined.
#else // The memory allocation for TASK.
#define REG_ADDR_BASE						0xBF804000 		// The register address base. Depends on system define.
#define RX_BUFFER_SEL						0x0003//0x0009			// 0x0=2KB,0x1=4KB,0x2=8KB,0x3=16KB, 0x09=1MB
u32     RAM_ADDR_BASE                                  = 0x00000000;     // After init, RAM_ADDR_BASE = EMAC_ABSO_MEM_BASE
u32     RX_BUFFER_BASE					  = 0x00000000;     // IMPORTANT: lowest 14 bits as zero.
u32     RBQP_BASE						  = 0x00004000;     // IMPORTANT: lowest 13 bits as zero.
u32     TX_BUFFER_BASE					  = 0x00006000;
u32     TX_SKB_BASE                                        = 0x00006100;
u32     RX_FRAME_ADDR					  = 0x00007000;
#endif //^MEM_BASE_ADJUSTMENT ...............................................
#define RX_BUFFER_SIZE						(0x00000800<<RX_BUFFER_SEL)
#define RBQP_LENG							0x0100                  // ==?descriptors
#define RBQP_SIZE							(0x0008*RBQP_LENG)      // ==8bytes*?==?descriptors

u8 MY_DEV[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
u8 MY_MAC[6] = { 0x00, 0x30, 0x1B, 0xBA, 0x02, 0xDB };
u8 PC_MAC[6] = { 0x00, 0x16, 0xD3, 0x38, 0xB0, 0x77 };

typedef volatile unsigned int EMAC_REG;

typedef struct _URANUS_EMAC {
// Constant: ----------------------------------------------------------------
// Register MAP:
  EMAC_REG   REG_EMAC_CTL_L; 	      //0x00000000 Network Control Register Low  16 bit
  EMAC_REG   REG_EMAC_CTL_H; 	      //0x00000004 Network Control Register High 16 bit
  EMAC_REG   REG_EMAC_CFG_L; 	      //0x00000008 Network Configuration Register Low  16 bit
  EMAC_REG   REG_EMAC_CFG_H; 	      //0x0000000C Network Configuration Register High 16 bit
  EMAC_REG   REG_EMAC_SR_L; 	      //0x00000010 Network Status Register Low  16 bit
  EMAC_REG   REG_EMAC_SR_H; 	      //0x00000014 Network Status Register High 16 bit
  EMAC_REG   REG_EMAC_TAR_L; 	      //0x00000018 Transmit Address Register Low  16 bit
  EMAC_REG   REG_EMAC_TAR_H; 	      //0x0000001C Transmit Address Register High 16 bit
  EMAC_REG   REG_EMAC_TCR_L; 	      //0x00000020 Transmit Control Register Low  16 bit
  EMAC_REG   REG_EMAC_TCR_H; 	      //0x00000024 Transmit Control Register High 16 bit
  EMAC_REG   REG_EMAC_TSR_L; 	      //0x00000028 Transmit Status Register Low  16 bit
  EMAC_REG   REG_EMAC_TSR_H; 	      //0x0000002C Transmit Status Register High 16 bit
  EMAC_REG   REG_EMAC_RBQP_L;         //0x00000030 Receive Buffer Queue Pointer Low  16 bit
  EMAC_REG   REG_EMAC_RBQP_H;         //0x00000034 Receive Buffer Queue Pointer High 16 bit
  EMAC_REG   REG_EMAC_RBCFG_L;        //0x00000038 Receive buffer configuration Low  16 bit
  EMAC_REG   REG_EMAC_RBCFG_H;        //0x0000003C Receive buffer configuration High 16 bit
  EMAC_REG   REG_EMAC_RSR_L; 	      //0x00000040 Receive Status Register Low  16 bit
  EMAC_REG   REG_EMAC_RSR_H; 	      //0x00000044 Receive Status Register High 16 bit
  EMAC_REG   REG_EMAC_ISR_L;          //0x00000048 Interrupt Status Register Low  16 bit
  EMAC_REG   REG_EMAC_ISR_H;          //0x0000004C Interrupt Status Register High 16 bit
  EMAC_REG   REG_EMAC_IER_L;          //0x00000050 Interrupt Enable Register Low  16 bit
  EMAC_REG   REG_EMAC_IER_H;          //0x00000054 Interrupt Enable Register High 16 bit
  EMAC_REG   REG_EMAC_IDR_L; 	      //0x00000058 Interrupt Disable Register Low  16 bit
  EMAC_REG   REG_EMAC_IDR_H; 	      //0x0000005C Interrupt Disable Register High 16 bit
  EMAC_REG   REG_EMAC_IMR_L; 	      //0x00000060 Interrupt Mask Register Low  16 bit
  EMAC_REG   REG_EMAC_IMR_H; 	      //0x00000064 Interrupt Mask Register High 16 bit
  EMAC_REG   REG_EMAC_MAN_L; 	      //0x00000068 PHY Maintenance Register Low  16 bit
  EMAC_REG   REG_EMAC_MAN_H; 	      //0x0000006C PHY Maintenance Register High 16 bit
  EMAC_REG   REG_EMAC_RBRP_L;         //0x00000070 Receive Buffer First full pointer Low  16 bit
  EMAC_REG   REG_EMAC_RBRP_H;         //0x00000074 Receive Buffer First full pointer High 16 bit
  EMAC_REG   REG_EMAC_RBWP_L;         //0x00000078 Receive Buffer Current pointer Low  16 bit
  EMAC_REG   REG_EMAC_RBWP_H;         //0x0000007C Receive Buffer Current pointer High 16 bit
  EMAC_REG   REG_EMAC_FRA_L; 	      //0x00000080 Frames Transmitted OK Register Low  16 bit
  EMAC_REG   REG_EMAC_FRA_H; 	      //0x00000084 Frames Transmitted OK Register High 16 bit
  EMAC_REG   REG_EMAC_SCOL_L;         //0x00000088 Single Collision Frame Register Low  16 bit
  EMAC_REG   REG_EMAC_SCOL_H;         //0x0000008C Single Collision Frame Register High 16 bit
  EMAC_REG   REG_EMAC_MCOL_L;         //0x00000090 Multiple Collision Frame Register Low  16 bit
  EMAC_REG   REG_EMAC_MCOL_H;         //0x00000094 Multiple Collision Frame Register High 16 bit
  EMAC_REG   REG_EMAC_OK_L; 	      //0x00000098 Frames Received OK Register Low  16 bit
  EMAC_REG   REG_EMAC_OK_H; 	      //0x0000009C Frames Received OK Register High 16 bit
  EMAC_REG   REG_EMAC_SEQE_L;         //0x000000A0 Frame Check Sequence Error Register Low  16 bit
  EMAC_REG   REG_EMAC_SEQE_H;         //0x000000A4 Frame Check Sequence Error Register High 16 bit
  EMAC_REG   REG_EMAC_ALE_L; 	      //0x000000A8 Alignment Error Register Low  16 bit
  EMAC_REG   REG_EMAC_ALE_H; 	      //0x000000AC Alignment Error Register High 16 bit
  EMAC_REG   REG_EMAC_DTE_L; 	      //0x000000B0 Deferred Transmission Frame Register Low  16 bit
  EMAC_REG   REG_EMAC_DTE_H; 	      //0x000000B4 Deferred Transmission Frame Register High 16 bit
  EMAC_REG   REG_EMAC_LCOL_L;         //0x000000B8 Late Collision Register Low  16 bit
  EMAC_REG   REG_EMAC_LCOL_H;         //0x000000BC Late Collision Register High 16 bit
  EMAC_REG   REG_EMAC_ECOL_L;         //0x000000C0 Excessive Collision Register Low  16 bit
  EMAC_REG   REG_EMAC_ECOL_H;         //0x000000C4 Excessive Collision Register High 16 bit
  EMAC_REG   REG_EMAC_TUE_L; 	      //0x000000C8 Transmit Underrun Error Register Low  16 bit
  EMAC_REG   REG_EMAC_TUE_H; 	      //0x000000CC Transmit Underrun Error Register High 16 bit
  EMAC_REG   REG_EMAC_CSE_L;          //0x000000D0 Carrier sense errors Register Low  16 bit
  EMAC_REG   REG_EMAC_CSE_H;          //0x000000D4 Carrier sense errors Register High 16 bit
  EMAC_REG   REG_EMAC_RE_L;           //0x000000D8 Receive resource error Low  16 bit
  EMAC_REG   REG_EMAC_RE_H;           //0x000000DC Receive resource error High 16 bit
  EMAC_REG   REG_EMAC_ROVR_L;         //0x000000E0 Received overrun Low  16 bit
  EMAC_REG   REG_EMAC_ROVR_H;         //0x000000E4 Received overrun High 16 bit
  EMAC_REG   REG_EMAC_SE_L;           //0x000000E8 Received symbols error Low  16 bit
  EMAC_REG   REG_EMAC_SE_H;           //0x000000EC Received symbols error High 16 bit
//  EMAC_REG	 REG_EMAC_CDE; 	      //           Code Error Register
  EMAC_REG   REG_EMAC_ELR_L; 	      //0x000000F0 Excessive Length Error Register Low  16 bit
  EMAC_REG   REG_EMAC_ELR_H; 	      //0x000000F4 Excessive Length Error Register High 16 bit

  EMAC_REG   REG_EMAC_RJB_L; 	      //0x000000F8 Receive Jabber Register Low  16 bit
  EMAC_REG   REG_EMAC_RJB_H; 	      //0x000000FC Receive Jabber Register High 16 bit
  EMAC_REG   REG_EMAC_USF_L; 	      //0x00000100 Undersize Frame Register Low  16 bit
  EMAC_REG   REG_EMAC_USF_H; 	      //0x00000104 Undersize Frame Register High 16 bit
  EMAC_REG   REG_EMAC_SQEE_L; 	      //0x00000108 SQE Test Error Register Low  16 bit
  EMAC_REG   REG_EMAC_SQEE_H; 	      //0x0000010C SQE Test Error Register High 16 bit
//  EMAC_REG	 REG_EMAC_DRFC;       //           Discarded RX Frame Register
  EMAC_REG   REG_Reserved1_L; 	      //0x00000110 Low  16 bit
  EMAC_REG   REG_Reserved1_H; 	      //0x00000114 High 16 bit
  EMAC_REG   REG_Reserved2_L; 	      //0x00000118 Low  16 bit
  EMAC_REG   REG_Reserved2_H; 	      //0x0000011C High 16 bit
  EMAC_REG   REG_EMAC_HSH_L; 	      //0x00000120 Hash Address High[63:32] Low  16 bit
  EMAC_REG   REG_EMAC_HSH_H; 	      //0x00000124 Hash Address High[63:32] High 16 bit
  EMAC_REG   REG_EMAC_HSL_L; 	      //0x00000128 Hash Address Low[31:0] Low  16 bit
  EMAC_REG   REG_EMAC_HSL_H; 	      //0x0000012C Hash Address Low[31:0] High 16 bit

  EMAC_REG   REG_EMAC_SA1L_L;         //0x00000130 Specific Address 1 Low, First 4 bytes Low  16 bit
  EMAC_REG   REG_EMAC_SA1L_H;         //0x00000134 Specific Address 1 Low, First 4 bytes High 16 bit
  EMAC_REG   REG_EMAC_SA1H_L;         //0x00000138 Specific Address 1 High, Last 2 bytes Low  16 bit
  EMAC_REG   REG_EMAC_SA1H_H;         //0x0000013C Specific Address 1 High, Last 2 bytes High 16 bit
  EMAC_REG   REG_EMAC_SA2L_L;         //0x00000140 Specific Address 2 Low, First 4 bytes Low  16 bit
  EMAC_REG   REG_EMAC_SA2L_H;         //0x00000144 Specific Address 2 Low, First 4 bytes High 16 bit
  EMAC_REG   REG_EMAC_SA2H_L;         //0x00000148 Specific Address 2 High, Last 2 bytes Low  16 bit
  EMAC_REG   REG_EMAC_SA2H_H;         //0x0000014C Specific Address 2 High, Last 2 bytes High 16 bit
  EMAC_REG   REG_EMAC_SA3L_L;         //0x00000150 Specific Address 3 Low, First 4 bytes Low  16 bit
  EMAC_REG   REG_EMAC_SA3L_H;         //0x00000154 Specific Address 3 Low, First 4 bytes High 16 bit
  EMAC_REG   REG_EMAC_SA3H_L;         //0x00000158 Specific Address 3 High, Last 2 bytes Low  16 bit
  EMAC_REG   REG_EMAC_SA3H_H;         //0x0000015C Specific Address 3 High, Last 2 bytes High 16 bit
  EMAC_REG   REG_EMAC_SA4L_L;         //0x00000160 Specific Address 4 Low, First 4 bytes Low  16 bit
  EMAC_REG   REG_EMAC_SA4L_H;         //0x00000164 Specific Address 4 Low, First 4 bytes High 16 bit
  EMAC_REG   REG_EMAC_SA4H_L;         //0x00000168 Specific Address 4 High, Last 2 bytes Low  16 bit
  EMAC_REG   REG_EMAC_SA4H_H;         //0x0000016C Specific Address 4 High, Last 2 bytes High 16 bit
  EMAC_REG   REG_TAG_TYPE_L; 	      //0x00000170 tag type of the frame Low  16 bit
  EMAC_REG   REG_TAG_TYPE_H; 	      //0x00000174 tag type of the frame High 16 bit
  EMAC_REG   REG_Reserved3[34];       //0x00000178 Low  16 bit
  EMAC_REG   REG_JULIAN_0100_L;       //0x00000200 Low  16 bit
  EMAC_REG   REG_JULIAN_0100_H;       //0x00000204 High 16 bit
  EMAC_REG   REG_JULIAN_0104_L;       //0x00000208 Low  16 bit
  EMAC_REG   REG_JULIAN_0104_H;       //0x0000020C High 16 bit
  EMAC_REG   REG_CAMA0_l_L; 		  //0x00000210 16 LSB of CAM address  0 Low  16 bit
  EMAC_REG   REG_CAMA0_l_H; 		  //0x00000214 16 LSB of CAM address  0 High 16 bit
  EMAC_REG   REG_CAMA0_h_L; 		  //0x00000218 32 MSB of CAM address  0 Low  16 bit
  EMAC_REG   REG_CAMA0_h_H; 		  //0x0000021C 32 MSB of CAM address  0 High 16 bit
  EMAC_REG   REG_Reserved4[368];      //0x00000220
  EMAC_REG   REG_CAMA62_l_L; 	      //0x000007E0 16 LSB of CAM address 62 Low  16 bit
  EMAC_REG   REG_CAMA62_l_H; 	      //0x000007E4 16 LSB of CAM address 62 High 16 bit
  EMAC_REG   REG_CAMA62_h_L; 	      //0x000007E8 32 MSB of CAM address 62 Low  16 bit
  EMAC_REG   REG_CAMA62_h_H; 	      //0x000007EC 32 MSB of CAM address 62 High 16 bit
} URANUS_EMAC_Str, *URANUS_EMAC;      //
//#endif
//
#define EMAC_MIU_RW  (0x3 <<  10)   //EMAC power on clk

// -------- EMAC_CTL : (EMAC Offset: 0x0)  --------
#define EMAC_LB           ( 0x1 <<  0) // (EMAC) Loopback. Optional. When set, loopback signal is at high level.
#define EMAC_LBL          ( 0x1 <<  1) // (EMAC) Loopback local.
#define EMAC_RE           ( 0x1 <<  2) // (EMAC) Receive enable.
#define EMAC_TE           ( 0x1 <<  3) // (EMAC) Transmit enable.
#define EMAC_MPE          ( 0x1 <<  4) // (EMAC) Management port enable.
#define EMAC_CSR          ( 0x1 <<  5) // (EMAC) Clear statistics registers.
#define EMAC_ISR          ( 0x1 <<  6) // (EMAC) Increment statistics registers.
#define EMAC_WES          ( 0x1 <<  7) // (EMAC) Write enable for statistics registers.
#define EMAC_BP           ( 0x1 <<  8) // (EMAC) Back pressure.
// -------- EMAC_CFG : (EMAC Offset: 0x4) Network Configuration Register --------
#define EMAC_SPD          ( 0x1 <<  0) // (EMAC) Speed.
#define EMAC_FD           ( 0x1 <<  1) // (EMAC) Full duplex.
#define EMAC_BR           ( 0x1 <<  2) // (EMAC) Bit rate.
#define EMAC_CAF          ( 0x1 <<  4) // (EMAC) Copy all frames.
#define EMAC_NBC          ( 0x1 <<  5) // (EMAC) No broadcast.
#define EMAC_MTI          ( 0x1 <<  6) // (EMAC) Multicast hash enable
#define EMAC_UNI          ( 0x1 <<  7) // (EMAC) Unicast hash enable.
#define EMAC_RLF          ( 0x1 <<  8) // (EMAC) Receive Long Frame.
#define EMAC_EAE          ( 0x1 <<  9) // (EMAC) External address match enable.
#define EMAC_CLK          ( 0x3 << 10) // (EMAC)
#define EMAC_CLK_HCLK_8   ( 0x0 << 10) // (EMAC) HCLK divided by 8
#define EMAC_CLK_HCLK_16  ( 0x1 << 10) // (EMAC) HCLK divided by 16
#define EMAC_CLK_HCLK_32  ( 0x2 << 10) // (EMAC) HCLK divided by 32
#define EMAC_CLK_HCLK_64  ( 0x3 << 10) // (EMAC) HCLK divided by 64
#define EMAC_RT           ( 0x1 << 12) // (EMAC) Retry test
#define EMAC_CAMMEG       ( 0x1 << 13) // (EMAC)
// -------- EMAC_SR : (EMAC Offset: 0x8) Network Status Register --------
#define EMAC_MDIO         ( 0x1 <<  1) // (EMAC)
#define EMAC_IDLE         ( 0x1 <<  2) // (EMAC)
// -------- EMAC_TCR : (EMAC Offset: 0x10) Transmit Control Register --------
#define EMAC_LEN          ( 0x7FF <<  0) // (EMAC)
#define EMAC_NCRC         ( 0x1 << 15) // (EMAC)
// -------- EMAC_TSR : (EMAC Offset: 0x14) Transmit Control Register --------
#define EMAC_OVR          ( 0x1 <<  0) // (EMAC)
#define EMAC_COL          ( 0x1 <<  1) // (EMAC)
#define EMAC_RLE          ( 0x1 <<  2) // (EMAC)
//#define EMAC_TXIDLE     ( 0x1 <<  3) // (EMAC)
#define EMAC_IDLETSR      ( 0x1 <<  3) // (EMAC)
#define EMAC_BNQ          ( 0x1 <<  4) // (EMAC)
#define EMAC_COMP         ( 0x1 <<  5) // (EMAC)
#define EMAC_UND          ( 0x1 <<  6) // (EMAC)
// -------- EMAC_RSR : (EMAC Offset: 0x20) Receive Status Register --------
#define EMAC_DNA          ( 0x1 <<  0) // (EMAC)
#define EMAC_REC          ( 0x1 <<  1) // (EMAC)
#define EMAC_RSROVR       ( 0x1 <<  2) // (EMAC)
#define EMAC_BNA          ( 0x1 <<  3) // (EMAC)
// -------- EMAC_ISR : (EMAC Offset: 0x24) Interrupt Status Register --------
#define EMAC_INT_DONE     ( 0x1 <<  0) // (EMAC)
#define EMAC_INT_RCOM     ( 0x1 <<  1) // (EMAC)
#define EMAC_INT_RBNA     ( 0x1 <<  2) // (EMAC)
#define EMAC_INT_TOVR     ( 0x1 <<  3) // (EMAC)
#define EMAC_INT_TUND     ( 0x1 <<  4) // (EMAC)
#define EMAC_INT_RTRY     ( 0x1 <<  5) // (EMAC)
#define EMAC_INT_TBRE     ( 0x1 <<  6) // (EMAC)
#define EMAC_INT_TCOM     ( 0x1 <<  7) // (EMAC)
#define EMAC_INT_TIDLE    ( 0x1 <<  8) // (EMAC)
#define EMAC_INT_LINK     ( 0x1 <<  9) // (EMAC)
#define EMAC_INT_ROVR     ( 0x1 << 10) // (EMAC)
#define EMAC_INT_HRESP    ( 0x1 << 11) // (EMAC)

// -------- EMAC_IER : (EMAC Offset: 0x28) Interrupt Enable Register --------
// -------- EMAC_IDR : (EMAC Offset: 0x2c) Interrupt Disable Register --------
// -------- EMAC_IMR : (EMAC Offset: 0x30) Interrupt Mask Register --------
// -------- EMAC_MAN : (EMAC Offset: 0x34) PHY Maintenance Register --------
#define EMAC_DATA         ( 0xFFFF <<  0) // (EMAC)
#define EMAC_CODE         ( 0x3 << 16) // (EMAC)
#define EMAC_CODE_802_3   ( 0x2 << 16) // (EMAC) Write Operation
#define EMAC_REGA         ( 0x1F << 18) // (EMAC)
#define EMAC_PHYA         ( 0x1F << 23) // (EMAC)
#define EMAC_RW           ( 0x3 << 28) // (EMAC)
#define EMAC_RW_R         ( 0x2 << 28) // (EMAC) Read Operation
#define EMAC_RW_W         ( 0x1 << 28) // (EMAC) Write Operation
#define EMAC_HIGH         ( 0x1 << 30) // (EMAC)
#define EMAC_LOW          ( 0x1 << 31) // (EMAC)
// -------- EMAC_RBRP: (EMAC Offset: 0x38) Receive Buffer First full pointer--------
#define EMAC_WRAP_R       ( 0x1 << 14) // Wrap bit
// -------- EMAC_RBWP: (EMAC Offset: 0x3C) Receive Buffer Current pointer--------
#define EMAC_WRAP_W       ( 0x1 << 14) // Wrap bit
// ........................................................................ //

//URANUS PHY //
#define MII_URANUS_ID   0x01111//Test value

//URANUS specific registers //
#define MII_USCR_REG	16
#define MII_USCSR_REG   17
#define MII_USINTR_REG  21

/* ........................................................................ */

//#define MAX_RBUFF_SZ   0x1000     /* 4Kb because of hardware constraint */ //2007/12/07 Nick

#define MAX_RX_DESCR       32     /* max number of receive buffers */
//#define MAX_RX_DESCR       32    /* max number of receive buffers */
#define EMAC_DESC_DONE 0x00000001  /* bit for if DMA is done */
#define EMAC_DESC_WRAP 0x00000002  /* bit for wrap */

#define EMAC_BROADCAST 0x80000000  /* broadcast address */
#define EMAC_MULTICAST 0x40000000  /* multicast address */
#define EMAC_UNICAST   0x20000000  /* unicast address */

struct rbf_t
{
  unsigned int  addr;
  unsigned long size;
};

struct recv_desc_bufs
{
  char recv_buf[RX_BUFFER_SIZE];                /* must be on MAX_RBUFF_SZ boundary */
  struct rbf_t descriptors[MAX_RX_DESCR];     /* must be on sizeof (rbf_t) boundary */
};

struct EMAC_private
{
	struct net_device_stats stats;
	struct mii_if_info mii;			/* ethtool support */

	/* PHY */
	unsigned long phy_type;			/* type of PHY (PHY_ID) */
	spinlock_t lock;			/* lock for MDI interface */
	short phy_media;			/* media interface type */

	/* Transmit */
	struct sk_buff *skb;			/* holds skb until xmit interrupt completes */
	dma_addr_t skb_physaddr;		/* phys addr from pci_map_single */
	int skb_length;				/* saved skb length for pci_unmap_single */

	/* Receive */
	int rxBuffIndex;			/* index into receive descriptor list */
	struct recv_desc_bufs *dlist;		/* descriptor list address */
	struct recv_desc_bufs *dlist_phys;	/* descriptor list physical address */
};

#define ROUND_SUP_4(x) (((x)+3)&~3)

struct eth_drv_sgX {
    u32                 buf;
    u32                 len;
};

struct _BasicConfigEMAC
{
       u8                  connected;          // 0:No, 1:Yes    <== (20070515) Wait for Julian's reply
	u8					speed;				// 10:10Mbps, 100:100Mbps
	// ETH_CTL Register:
	u8                  wes;				// 0:Disable, 1:Enable (WR_ENABLE_STATISTICS_REGS)
	// ETH_CFG Register:
	u8					duplex;				// 1:Half-duplex, 2:Full-duplex
	u8					cam;                // 0:No CAM, 1:Yes
	u8 					rcv_bcast;      	// 0:No, 1:Yes
	u8					rlf;                // 0:No, 1:Yes receive long frame(1522)
	// MAC Address:
	u8					sa1[6];				// Specific Addr 1 (MAC Address)
	u8					sa2[6];				// Specific Addr 2
	u8					sa3[6];				// Specific Addr 3
	u8					sa4[6];				// Specific Addr 4
};
typedef struct _BasicConfigEMAC BasicConfigEMAC;

struct _UtilityVarsEMAC
{
	u32 				readIdxRBQP;		// Reset = 0x00000000
	u32 				rxOneFrameAddr;		// Reset = 0x00000000 (Store the Addr of "ReadONE_RX_Frame")
	// Statistics Counters : (accumulated)
	u32                 cntREG_ETH_FRA;
	u32                 cntREG_ETH_SCOL;
	u32                 cntREG_ETH_MCOL;
	u32                 cntREG_ETH_OK;
	u32                 cntREG_ETH_SEQE;
	u32                 cntREG_ETH_ALE;
	u32                 cntREG_ETH_DTE;
	u32                 cntREG_ETH_LCOL;
	u32                 cntREG_ETH_ECOL;
	u32                 cntREG_ETH_TUE;
	u32                 cntREG_ETH_CSE;
	u32                 cntREG_ETH_RE;
	u32                 cntREG_ETH_ROVR;
	u32                 cntREG_ETH_SE;
	u32                 cntREG_ETH_ELR;
	u32                 cntREG_ETH_RJB;
	u32                 cntREG_ETH_USF;
	u32                 cntREG_ETH_SQEE;
	// Interrupt Counter :
	u32 				cntHRESP;			// Reset = 0x0000
	u32 				cntROVR;			// Reset = 0x0000
	u32 				cntLINK;			// Reset = 0x0000
	u32 				cntTIDLE;			// Reset = 0x0000
	u32 				cntTCOM;			// Reset = 0x0000
	u32 				cntTBRE;			// Reset = 0x0000
	u32 				cntRTRY;			// Reset = 0x0000
	u32 				cntTUND;			// Reset = 0x0000
	u32 				cntTOVR;			// Reset = 0x0000
	u32 				cntRBNA;			// Reset = 0x0000
	u32 				cntRCOM;			// Reset = 0x0000
	u32 				cntDONE;			// Reset = 0x0000
	// Flags:
	u8                  flagMacTxPermit;    // 0:No,1:Permitted.  Initialize as "permitted"
	u8                  flagISR_INT_RCOM;
	u8                  flagISR_INT_RBNA;
	u8                  flagISR_INT_DONE;
	u8                  flagPowerOn;        // 0:Poweroff, 1:Poweron
	u8                  initedEMAC;         // 0:Not initialized, 1:Initialized.
	// Misc Counter:
	u32     			cntRxFrames;		// Reset = 0x00000000 (Counter of RX frames,no matter it's me or not)
	u32                 cntReadONE_RX;      // Counter for ReadONE_RX_Frame
	u32                 cntCase20070806;
	u32                 cntChkToTransmit;
	// Misc Variables:
	u32                 mainThreadTasks;    // (20071029_CHARLES) b0=Poweroff,b1=Poweron
};
typedef struct _UtilityVarsEMAC UtilityVarsEMAC;

BasicConfigEMAC ThisBCE;
UtilityVarsEMAC ThisUVE;

//Define for EMAC_Power_On_Clk and EMAC_Power_Off_Clk
#define MASK(x)                     (((1<<(x##_BITS))-1) << x##_SHIFT)
#define _BIT(x)                      (1<<(x))

#define BMASK(bits)                 (_BIT(((1)?bits)+1)-_BIT(((0)?bits)))
#define BITS(bits,value)            ((_BIT(((1)?bits)+1)-_BIT(((0)?bits))) & (value<<((0)?bits)))

#define REG_TOP_BASE                0xBF803C00
#define TOP_REG(addr)               (*((volatile u32*)(REG_TOP_BASE + ((addr)<<2))))

#define REG_TOP_SPI_EMAC            0x0005
    #define TOP_CKG_EMACRX_MASK                 BMASK(12:10)
    #define TOP_CKG_EMACRX_DIS                  _BIT(10)
    #define TOP_CKG_EMACRX_INV                  _BIT(11)
    #define TOP_CKG_EMACTX_MASK                 BMASK(15:13)
    #define TOP_CKG_EMACTX_DIS                  _BIT(13)
    #define TOP_CKG_EMACTX_INV                  _BIT(14)

#define REG_TOP_PCI_PD_TEST         0x0007
    #define TOP_PCI_RPU                         BMASK(9:0)
    #define TOP_PD_ALL                          _BIT(10)
    #define TOP_TEST_CLK_MODE                   _BIT(11)
    #define TOP_TURN_OFF_PAD                    _BIT(12)

#define REG_TOP_MCU_USB_STC0        0x0011
    #define TOP_MCU_CLK_MASK                    BMASK(0:0)
    #define TOP_MCU_CLK_DFT                     BITS(0:0, 0)
    #define TOP_MCU_CLK_SRC_MCUCLK              BITS(0:0, 1)
    #define TSP_STC0_SEL                        _BIT(7)

#define REG_TOP_M4VD_EMAC_GE        0x0015
    #define TOP_CKG_M4VD_MASK                   BMASK(5:0)
    #define TOP_CKG_M4VD_DIS                    _BIT(0)
    #define TOP_CKG_M4VD_INV                    _BIT(1)
    #define TOP_CKG_M4VD_SRC_144                BITS(5:2, 0)
    #define TOP_CKG_M4VD_SRC_123                BITS(5:2, 1)
    #define TOP_CKG_M4VD_SRC_108                BITS(5:2, 2)
    #define TOP_CKG_M4VD_SRC_DFT                BITS(5:2, 3)
    #define TOP_CKG_EMACAHB_MASK                BMASK(11:8)
    #define TOP_CKG_EMACAHB_DIS                 _BIT(8)
    #define TOP_CKG_EMACAHB_INV                 _BIT(9)
    #define TOP_CKG_EMACAHB_SRC_108             BITS(11:10, 0)
    #define TOP_CKG_EMACAHB_SRC_86              BITS(11:10, 1)
    #define TOP_CKG_EMACAHB_SRC_62              BITS(11:10, 2)
    #define TOP_CKG_EMACAHB_SRC_DFT             BITS(11:10, 3)
    #define TOP_CKG_GE_MASK                     BMASK(15:12)
    #define TOP_CKG_GE_DIS                      _BIT(12)
    #define TOP_CKG_GE_INV                      _BIT(13)
    #define TOP_CKG_GE_SRC_123                  BITS(15:12, 0)
    #define TOP_CKG_GE_SRC_108                  BITS(15:12, 1)
    #define TOP_CKG_GE_SRC_86                   BITS(15:12, 2)
    #define TOP_CKG_GE_SRC_DFT                  BITS(15:12, 3)

#define REG_TOP_MCU                 0x0022
    #define TOP_CKG_MCU_MASK                    BMASK(12:8)
    #define TOP_CKG_MCU_DIS                     _BIT(0)
    #define TOP_CKG_MCU_INV                     _BIT(1)
    #define TOP_CKG_MCU_SRC_MASK                BMASK(12:10)
    #define TOP_CKG_MCU_SRC_216                 BITS(12:10, 0)
    #define TOP_CKG_MCU_SRC_DRAM                BITS(12:10, 1)
    #define TOP_CKG_MCU_SRC_173                 BITS(12:10, 2)
    #define TOP_CKG_MCU_SRC_160                 BITS(12:10, 3)
    #define TOP_CKG_MCU_SRC_144                 BITS(12:10, 4)
    #define TOP_CKG_MCU_SRC_123                 BITS(12:10, 5)
    #define TOP_CKG_MCU_SRC_108                 BITS(12:10, 6)
    #define TOP_CKG_MCU_SRC_DFT                 BITS(12:10, 7)

#define REG(addr) (*(volatile u32 *)(addr))
#define TOP_REG(addr)               (*((volatile u32*)(REG_TOP_BASE + ((addr)<<2))))

#define REG_MIU_BASE                         (0xBF800000 + 0x900*4)
#define REG_MIU_RQ0_MASK                ((0x23<<2)+ (REG_MIU_BASE))
#define REG_MIU_RQ1_MASK                ((0x33<<2)+ (REG_MIU_BASE))
#define REG_MIU_RQ2_MASK                ((0x43<<2)+ (REG_MIU_BASE))


//#ifdef ARP_TEST
// Constant: ----------------------------------------------------------------
// Register MAP:
#define REG_ETH_CTL    		    		0x00000000         // Network control register
#define REG_ETH_CFG  					0x00000004         // Network configuration register
#define REG_ETH_SR 			    		0x00000008         // Network status register
#define REG_ETH_TAR  					0x0000000C         // Transmit address register
#define REG_ETH_TCR  					0x00000010         // Transmit control register
#define REG_ETH_TSR  					0x00000014         // Transmit status register
#define REG_ETH_RBQP  		    		0x00000018         // Receive buffer queue pointer
#define REG_ETH_BUFF  	    			0x0000001C         // Receive Buffer Configuration
#define REG_ETH_RSR 					0x00000020         // Receive status register
#define REG_ETH_ISR 					0x00000024         // Interrupt status register
#define REG_ETH_IER  					0x00000028         // Interrupt enable register
#define REG_ETH_IDR  					0x0000002C         // Interrupt disable register
#define REG_ETH_IMR  					0x00000030         // Interrupt mask register
#define REG_ETH_MAN  					0x00000034         // PHY maintenance register
#define REG_ETH_BUFFRDPTR  	    		0x00000038         // Receive First Full Pointer
#define REG_ETH_BUFFWRPTR  	    		0x0000003C         // Receive Current pointer
#define REG_ETH_FRA  					0x00000040         // Frames transmitted OK
#define REG_ETH_SCOL  		    		0x00000044         // Single collision frames
#define REG_ETH_MCOL  		    		0x00000048         // Multiple collision frames
#define REG_ETH_OK  			    	0x0000004C         // Frames received OK
#define REG_ETH_SEQE  		    		0x00000050         // Frame check sequence errors
#define REG_ETH_ALE  					0x00000054         // Alignment errors
#define REG_ETH_DTE  					0x00000058         // Deferred transmission frames
#define REG_ETH_LCOL  		    		0x0000005C         // Late collisions
#define REG_ETH_ECOL  		    		0x00000060         // Excessive collisions
#define REG_ETH_TUE 					0x00000064         // Transmit under-run errors
#define REG_ETH_CSE  					0x00000068         // Carrier sense errors
#define REG_ETH_RE  			    	0x0000006C         // Receive resource error
#define REG_ETH_ROVR  		    		0x00000070         // Received overrun
#define REG_ETH_SE  			    	0x00000074         // Received symbols error
#define REG_ETH_ELR  					0x00000078         // Excessive length errors
#define REG_ETH_RJB  					0x0000007C         // Receive jabbers
#define REG_ETH_USF  					0x00000080         // Undersize frames
#define REG_ETH_SQEE  		    		0x00000084         // SQE test errors
#define REG_ETH_HSL  					0x00000090         // Hash register [31:0]
#define REG_ETH_HSH  					0x00000094         // Hash register [63:32]
#define REG_ETH_SA1L  		    		0x00000098         // Specific address 1 first 4 bytes
#define REG_ETH_SA1H  		    		0x0000009C         // Specific address 1 last  2 bytes
#define REG_ETH_SA2L  		    		0x000000A0         // Specific address 2 first 4 bytes
#define REG_ETH_SA2H  		    		0x000000A4         // Specific address 2 last  2 bytes
#define REG_ETH_SA3L  		    		0x000000A8         // Specific address 3 first 4 bytes
#define REG_ETH_SA3H  		    		0x000000AC         // Specific address 3 last  2 bytes
#define REG_ETH_SA4L  		    		0x000000B0         // Specific address 4 first 4 bytes
#define REG_ETH_SA4H  		    		0x000000B4         // Specific address 4 last  2 bytes
#define REG_TAG_TYPE  		    		0x000000B8         // tag type of the frame
#define REG_CAMA0_l  					0x00000200         // 16 LSB of CAM address  0
#define REG_CAMA0_h  					0x00000204         // 32 MSB of CAM address  0
#define REG_CAMA62_l  		    		0x000003F0         // 16 LSB of CAM address 62
#define REG_CAMA62_h   		    		0x000003F4         // 32 MSB of CAM address 62

#define EMAC_JULIAN_0100 				0x00000100
#define EMAC_JULIAN_0104 				0x00000104
//#elif

//#endif //#ifdef ARP_TEST

//#define REG_ETH_CFG_OFFSET  						0x00000004         // Network configuration register

#define REG_ETH_SA1L  	            0x00000098    //0x00000130         // Specific address 1 first 4 bytes
#define REG_ETH_SA2L  		    	0x000000A0    //0x00000140         // Specific address 2 first 4 bytes
#define REG_ETH_SA3L  		    	0x000000A8    //0x00000098         // Specific address 3 first 4 bytes
#define REG_ETH_SA4L  		    	0x000000B0    //0x00000160         // Specific address 4 first 4 bytes

struct sk_buff *Tx_SkbAddr;

#define EMAC_DMA_CACHE1                                       0xBF808604
#define EMAC_DMA_CACHE2                                       0xBF808608
//u32 uAllTxLen =0;
#endif

// -----------------------------------------------------------------------------
// Linux EMAC.c End
// -----------------------------------------------------------------------------



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

#ifndef _REG_GPIO_H_
#define _REG_GPIO_H_

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define GPIO_UNIT_NUM               169

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xBF000000//Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea1)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_GPIO_PM0      0	  
#define PAD_GPIO_PM1      1	  
#define PAD_GPIO_PM2      2	  
#define PAD_GPIO_PM3      3	  
#define PAD_GPIO_PM4      4	  
#define PAD_GPIO_PM5      5	  
#define PAD_GPIO_PM6      6	  
#define PAD_GPIO_PM8      7	  
#define PAD_GPIO_PM10     8	  
#define PAD_GPIO_PM12     9	  
#define PAD_IRIN          10	
#define PAD_CEC           11	
#define PAD_PM_SPI_CZ     12	
#define PAD_PM_SPI_CK     13	
#define PAD_PM_SPI_DI     14	
#define PAD_PM_SPI_DO     15	
#define PAD_HOTPLUGA      16	
#define PAD_HOTPLUGB      17	
#define PAD_HOTPLUGC      18	
#define PAD_HOTPLUGD      19	
#define PAD_DDCDA_CK      20	
#define PAD_DDCDA_DA      21	
#define PAD_DDCDB_CK      22	
#define PAD_DDCDB_DA      23	
#define PAD_DDCDC_CK      24	
#define PAD_DDCDC_DA      25	
#define PAD_DDCDD_CK      26	
#define PAD_DDCDD_DA      27	
#define PAD_SAR0          28	
#define PAD_SAR1          29	
#define PAD_SAR2          30	
#define PAD_SAR3          31	
#define PAD_PWM_PM        32	
#define PAD_GPIO0         33	
#define PAD_GPIO1         34	
#define PAD_GPIO2         35	
#define PAD_GPIO3         36	
#define PAD_GPIO4         37	
#define PAD_GPIO5         38	
#define PAD_TS0_D0        39	
#define PAD_TS0_D1        40	
#define PAD_TS0_D2        41	
#define PAD_TS0_D3        42	
#define PAD_TS0_D4        43	
#define PAD_TS0_D5        44	
#define PAD_TS0_D6        45	
#define PAD_TS0_D7        46	
#define PAD_TS0_VLD       47	
#define PAD_TS0_SYNC      48	
#define PAD_TS0_CLK       49	
#define PAD_TS1_CLK       50	
#define PAD_TS1_SYNC      51	
#define PAD_TS1_VLD       52	
#define PAD_TS1_D0        53	
#define PAD_TS1_D1        54	
#define PAD_TS1_D2        55	
#define PAD_TS1_D3        56	
#define PAD_TS1_D4        57	
#define PAD_TS1_D5        58	
#define PAD_TS1_D6        59	
#define PAD_TS1_D7        60	
#define PAD_TS2_D0        61	
#define PAD_TS2_VLD       62	
#define PAD_TS2_SYNC      63	
#define PAD_TS2_CLK       64	
#define PAD_I2S_IN_WS     65	
#define PAD_I2S_IN_BCK    66	
#define PAD_I2S_IN_SD     67	
#define PAD_SPDIF_IN      68	
#define PAD_SPDIF_OUT     69	
#define PAD_I2S_OUT_WS    70	
#define PAD_I2S_OUT_MCK   71	
#define PAD_I2S_OUT_BCK   72	
#define PAD_I2S_OUT_SD    73	
#define PAD_I2S_OUT_MUTE  74	
#define PAD_PCM_D3        75	
#define PAD_PCM_D4        76	
#define PAD_PCM_D5        77	
#define PAD_PCM_D6        78	
#define PAD_PCM_D7        79	
#define PAD_PCM_CE_N      80	
#define PAD_PCM_A10       81	
#define PAD_PCM_OE_N      82	
#define PAD_PCM_A11       83	
#define PAD_PCM_IORD_N    84	
#define PAD_PCM_A9        85	
#define PAD_PCM_IOWR_N    86	
#define PAD_PCM_A8        87	
#define PAD_PCM_A13       88	
#define PAD_PCM_A14       89	
#define PAD_PCM_WE_N      90	
#define PAD_PCM_IRQA_N    91	
#define PAD_PCM_A12       92	
#define PAD_PCM_A7        93	
#define PAD_PCM_A6        94	
#define PAD_PCM_A5        95	
#define PAD_PCM_WAIT_N    96	
#define PAD_PCM_A4        97	
#define PAD_PCM_A3        98	
#define PAD_PCM_A2        99	
#define PAD_PCM_REG_N     100	
#define PAD_PCM_A1        101	
#define PAD_PCM_A0        102	
#define PAD_PCM_D0        103	
#define PAD_PCM_D1        104	
#define PAD_PCM_D2        105	
#define PAD_PCM_RESET     106	
#define PAD_PCM_CD_N      107	
#define PAD_PCM2_CE_N     108	
#define PAD_PCM2_IRQA_N   109	
#define PAD_PCM2_WAIT_N   110	
#define PAD_PCM2_RESET    111	
#define PAD_PCM2_CD_N     112	
#define PAD_MPIF_CLK      113	
#define PAD_MPIF_BUSY_ERR 114	
#define PAD_MPIF_CS_N     115	
#define PAD_MPIF_D0       116	
#define PAD_MPIF_D1       117	
#define PAD_MPIF_D2       118	
#define PAD_MPIF_D3       119	
#define PAD_NAND_CEZ      120	
#define PAD_NAND_CEZ1     121	
#define PAD_NAND_CLE      122	
#define PAD_NAND_REZ      123	
#define PAD_NAND_WEZ      124	
#define PAD_NAND_WPZ      125	
#define PAD_NAND_ALE      126	
#define PAD_NAND_RBZ      127	
#define PAD_PF_CE0Z       128	
#define PAD_PF_CE1Z       129	
#define PAD_PF_OEZ        130	
#define PAD_PF_WEZ        131	
#define PAD_DDCR_DA       132	
#define PAD_DDCR_CK       133	
#define PAD_PWM0          134	
#define PAD_PWM1          135	
#define PAD_PWM2          136	
#define PAD_PWM3          137	
#define PAD_PWM4          138	
#define PAD_TGPIO0        139	
#define PAD_TGPIO1        140	
#define PAD_TGPIO2        141	
#define PAD_TGPIO3        142	
#define PAD_TCON0         143	
#define PAD_TCON01        144	
#define PAD_TCON02        145	
#define PAD_TCON03        146	
#define PAD_TCON04        147	
#define PAD_TCON05        148	
#define PAD_TCON06        149	
#define PAD_TCON07        150	
#define PAD_TCON08        151	
#define PAD_TCON09        152	
#define PAD_TCON10        153	
#define PAD_TCON11        154	
#define PAD_TCON12        155	
#define PAD_TCON13        156	
#define PAD_TCON14        157	
#define PAD_TCON15        158	
#define PAD_TCON16        159	
#define PAD_TCON17        160	
#define PAD_TCON18        161	
#define PAD_TCON19        162	
#define PAD_TCON20        163	
#define PAD_TCON21        164	
#define PAD_LCK           165	
#define PAD_LDE           166	
#define PAD_LHSYNC        167	
#define PAD_LVSYNC        168

#define GPIO_OEN    0   //set o to nake output
#define GPIO_ODN    1

#define IN_HIGH    1   //input high
#define IN_LOW     0   //input low

#define OUT_HIGH    1   //output high
#define OUT_LOW     0   //output low

#define MHal_GPIO_REG(addr)             (*(volatile U8*)(REG_MIPS_BASE + (((addr) & ~1)<<1) + (addr & 1)))

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_GPIO_H_


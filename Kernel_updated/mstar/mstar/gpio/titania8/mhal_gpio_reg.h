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
#define GPIO_UNIT_NUM               214

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xBF000000//Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea1)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_GPIO_PM0       0	
#define PAD_GPIO_PM1       1	
#define PAD_GPIO_PM2       2	
#define PAD_GPIO_PM3       3	
#define PAD_GPIO_PM4       4	
#define PAD_GPIO_PM5       5	
#define PAD_GPIO_PM6       6	
#define PAD_GPIO_PM7       7	
#define PAD_GPIO_PM8       8	
#define PAD_GPIO_PM9       9	
#define PAD_GPIO_PM10     10	
#define PAD_GPIO_PM11     11	
#define PAD_GPIO_PM12     12	
#define PAD_GPIO_PM13     13	
#define PAD_GPIO_PM14     14	
#define PAD_GPIO_PM15     15	
#define PAD_GPIO_PM16     16	
#define PAD_GPIO_PM17     17	
#define PAD_GPIO_PM18     18	
#define PAD_GPIO_PM19     19	
#define PAD_IRIN          20	
#define PAD_CEC           21	
#define PAD_PM_SPI_CZ     22	
#define PAD_PM_SPI_CK     23	
#define PAD_PM_SPI_DI     24	
#define PAD_PM_SPI_DO     25	
#define PAD_HOTPLUGA      26	
#define PAD_HOTPLUGB      27	
#define PAD_HOTPLUGC      209	
#define PAD_HOTPLUGD      28	
#define PAD_DDCDA_CK      205	
#define PAD_DDCDA_DA      206	
#define PAD_DDCDB_CK      207	
#define PAD_DDCDB_DA      208	
#define PAD_DDCDC_CK      29	
#define PAD_DDCDC_DA      30	
#define PAD_DDCDD_CK      31	
#define PAD_DDCDD_DA      32	
#define PAD_SAR0          33	
#define PAD_SAR1          34	
#define PAD_SAR2          35	
#define PAD_SAR3          36	
#define PAD_SAR4          37	
#define PAD_GPIO0         38	
#define PAD_GPIO1         39	
#define PAD_GPIO2         40	
#define PAD_GPIO3         41	
#define PAD_GPIO4         42	
#define PAD_GPIO5         43	
#define PAD_GPIO6         44	
#define PAD_GPIO7         45	
#define PAD_GPIO8         46	
#define PAD_GPIO9         47	
#define PAD_GPIO10        48	
#define PAD_GPIO11        49	
#define PAD_GPIO12        50	
#define PAD_GPIO13        51	
#define PAD_GPIO14        52	
#define PAD_GPIO15        53	
#define PAD_GPIO16        54	
#define PAD_GPIO17        55	
#define PAD_GPIO18        56	
#define PAD_GPIO19        57	
#define PAD_GPIO20        58	
#define PAD_GPIO21        59	
#define PAD_GPIO22        60	
#define PAD_GPIO23        61	
#define PAD_GPIO24        62	
#define PAD_GPIO25        63	
#define PAD_GPIO26        64	
#define PAD_GPIO27        65	
#define PAD_TS0_D0        66	
#define PAD_TS0_D1        67	
#define PAD_TS0_D2        68	
#define PAD_TS0_D3        69	
#define PAD_TS0_D4        70	
#define PAD_TS0_D5        71	
#define PAD_TS0_D6        72	
#define PAD_TS0_D7        73	
#define PAD_TS0_VLD       74	
#define PAD_TS0_SYNC      75	
#define PAD_TS0_CLK       76	
#define PAD_TS1_CLK       77	
#define PAD_TS1_SYNC      78	
#define PAD_TS1_VLD       79	
#define PAD_TS1_D0        80	
#define PAD_TS1_D1        81	
#define PAD_TS1_D2        82	
#define PAD_TS1_D3        83	
#define PAD_TS1_D4        84	
#define PAD_TS1_D5        85	
#define PAD_TS1_D6        86	
#define PAD_TS1_D7        87	
#define PAD_TS2_D0        88	
#define PAD_TS2_VLD       89	
#define PAD_TS2_SYNC      90	
#define PAD_TS2_CLK       91	
#define PAD_I2S_IN_WS     92	
#define PAD_I2S_IN_BCK    93	
#define PAD_I2S_IN_SD     94	
#define PAD_SPDIF_IN      95	
#define PAD_SPDIF_OUT     96	
#define PAD_I2S_OUT_WS    97	
#define PAD_I2S_OUT_MCK   98	
#define PAD_I2S_OUT_BCK   99	
#define PAD_I2S_OUT_SD    100	
#define PAD_I2S_OUT_SD1   101	
#define PAD_I2S_OUT_SD2   102	
#define PAD_I2S_OUT_SD3   103	
#define PAD_PCM_D3        104	
#define PAD_PCM_D4        105	
#define PAD_PCM_D5        106	
#define PAD_PCM_D6        107	
#define PAD_PCM_D7        108	
#define PAD_PCM_CE_N      109	
#define PAD_PCM_A10       110	
#define PAD_PCM_OE_N      111	
#define PAD_PCM_A11       112	
#define PAD_PCM_IORD_N    113	
#define PAD_PCM_A9        114	
#define PAD_PCM_IOWR_N    115	
#define PAD_PCM_A8        116	
#define PAD_PCM_A13       117	
#define PAD_PCM_A14       118	
#define PAD_PCM_WE_N      119	
#define PAD_PCM_IRQA_N    120	
#define PAD_PCM_A12       121	
#define PAD_PCM_A7        122	
#define PAD_PCM_A6        123	
#define PAD_PCM_A5        124	
#define PAD_PCM_WAIT_N    125	
#define PAD_PCM_A4        126	
#define PAD_PCM_A3        127	
#define PAD_PCM_A2        128	
#define PAD_PCM_REG_N     129	
#define PAD_PCM_A1        130	
#define PAD_PCM_A0        131	
#define PAD_PCM_D0        132	
#define PAD_PCM_D1        133	
#define PAD_PCM_D2        134	
#define PAD_PCM_RESET     135	
#define PAD_PCM_CD_N      136	
#define PAD_PCM2_CE_N     137	
#define PAD_PCM2_IRQA_N   138	
#define PAD_PCM2_WAIT_N   139	
#define PAD_PCM2_RESET    140	
#define PAD_PCM2_CD_N     141	
#define PAD_ET_MDIO       142	
#define PAD_ET_MDC        143	
#define PAD_ET_RXD0       144	
#define PAD_ET_RXD1       145	
#define PAD_ET_TX_CLK     146	
#define PAD_ET_TXD0       147	
#define PAD_ET_TXD1       148	
#define PAD_ET_TX_EN      149	
#define PAD_ET_COL        150	
#define PAD_MPIF_CLK      151	
#define PAD_MPIF_BUSY_ERR 152	
#define PAD_MPIF_CS_N     153	
#define PAD_MPIF_D0       154	
#define PAD_MPIF_D1       155	
#define PAD_MPIF_D2       156	
#define PAD_MPIF_D3       157	
#define PAD_NAND_CEZ      158	
#define PAD_NAND_CEZ1     159	
#define PAD_ONE_A15       160	
#define PAD_ONE_D13       161	
#define PAD_ONE_D14       162	
#define PAD_ONE_D15       163	
#define PAD_ONE_CLK       164	
#define PAD_ONE_CEN       165	
#define PAD_ONE_OEN       166	
#define PAD_ONE_WEN       167	
#define PAD_ONE_RPN       168	
#define PAD_ONE_AVDN      169	
#define PAD_ONE_INT       170	
#define PAD_ONE_RDY       171	
#define PAD_DDCR_DA       172	
#define PAD_DDCR_CK       173	
#define PAD_PWM0          174	
#define PAD_PWM1          175	
#define PAD_PWM2          176	
#define PAD_PWM3          177	
#define PAD_PWM4          178	
#define PAD_TGPIO0        179	
#define PAD_TGPIO1        180	
#define PAD_TGPIO2        181	
#define PAD_TGPIO3        182	
#define PAD_TCON0         183	
#define PAD_TCON01        184	
#define PAD_TCON02        185	
#define PAD_TCON03        186	
#define PAD_TCON04        187	
#define PAD_TCON05        188	
#define PAD_TCON06        189	
#define PAD_TCON07        190	
#define PAD_TCON08        191	
#define PAD_TCON09        192	
#define PAD_TCON10        193	
#define PAD_TCON11        194	
#define PAD_TCON12        195	
#define PAD_TCON13        196	
#define PAD_TCON14        197	
#define PAD_TCON15        198	
#define PAD_TCON16        199	
#define PAD_TCON17        200	
#define PAD_TCON18        201	
#define PAD_TCON19        202	
#define PAD_TCON20        203	
#define PAD_TCON21        204	
#define PAD_LCK           210	
#define PAD_LDE           211	
#define PAD_LHSYNC        212	
#define PAD_LVSYNC        213

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


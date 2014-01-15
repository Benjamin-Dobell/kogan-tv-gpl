/*
 * arch/arm/mach-tegra/tegra3_emc.h
 *
 * Copyright (C) 2011 NVIDIA Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef _MACH_TEGRA_TEGRA3_EMC_H
#define _MACH_TEGRA_TEGRA3_EMC_H

#define TEGRA_EMC_NUM_REGS	110

#define TEGRA_EMC_BRIDGE_RATE_MIN	300000000
#define TEGRA_EMC_BRIDGE_MVOLTS_MIN	1200

struct tegra_emc_table {
	u8 rev;
	unsigned long rate;

	/* unconditionally updated in one burst shot */
	u32 burst_regs[TEGRA_EMC_NUM_REGS];

	/* updated separately under some conditions */
	u32 emc_zcal_cnt_long;
	u32 emc_acal_interval;
	u32 emc_periodic_qrst;
	u32 emc_mode_reset;
	u32 emc_mode_1;
	u32 emc_mode_2;
	u32 emc_dsr;
	int emc_min_mv;
};

enum {
	DRAM_OVER_TEMP_NONE = 0,
	DRAM_OVER_TEMP_REFRESH,
};

struct clk;

int tegra_init_emc(const struct tegra_emc_table *table, int table_size);

void tegra_init_dram_bit_map(const u32 *bit_map, int map_size);
void tegra_emc_dram_type_init(struct clk *c);
int tegra_emc_get_dram_type(void);
int tegra_emc_get_dram_temperature(void);
int tegra_emc_set_over_temp_state(unsigned long state);

#ifdef CONFIG_PM_SLEEP
void tegra_mc_timing_restore(void);
#else
static inline void tegra_mc_timing_restore(void)
{ }
#endif

#define EMC_INTSTATUS				0x0
#define EMC_INTSTATUS_CLKCHANGE_COMPLETE	(0x1 << 4)

#define EMC_DBG					0x8
#define EMC_DBG_WRITE_MUX_ACTIVE		(0x1 << 1)

#define EMC_CFG					0xc
#define EMC_CFG_PERIODIC_QRST			(0x1 << 21)
#define EMC_CFG_DYN_SREF_ENABLE			(0x1 << 28)
#define EMC_CFG_PWR_MASK			(0xF << 28)

#define EMC_REFCTRL				0x20
#define EMC_REFCTRL_DEV_SEL_SHIFT		0
#define EMC_REFCTRL_DEV_SEL_MASK		(0x3 << EMC_REFCTRL_DEV_SEL_SHIFT)
#define EMC_REFCTRL_ENABLE			(0x1 << 31)
#define EMC_REFCTRL_ENABLE_ALL(num)		\
	((((num > 1) ? 0 : 2) << EMC_REFCTRL_DEV_SEL_SHIFT) \
	 | EMC_REFCTRL_ENABLE)
#define EMC_REFCTRL_DISABLE_ALL(num)		\
	(((num > 1) ? 0 : 2) << EMC_REFCTRL_DEV_SEL_SHIFT)

#define EMC_TIMING_CONTROL			0x28
#define EMC_RC					0x2c
#define EMC_RFC					0x30
#define EMC_RAS					0x34
#define EMC_RP					0x38
#define EMC_R2W					0x3c
#define EMC_W2R					0x40
#define EMC_R2P					0x44
#define EMC_W2P					0x48
#define EMC_RD_RCD				0x4c
#define EMC_WR_RCD				0x50
#define EMC_RRD					0x54
#define EMC_REXT				0x58
#define EMC_WDV					0x5c
#define EMC_QUSE				0x60
#define EMC_QRST				0x64
#define EMC_QSAFE				0x68
#define EMC_RDV					0x6c
#define EMC_REFRESH				0x70
#define EMC_BURST_REFRESH_NUM			0x74
#define EMC_PDEX2WR				0x78
#define EMC_PDEX2RD				0x7c
#define EMC_PCHG2PDEN				0x80
#define EMC_ACT2PDEN				0x84
#define EMC_AR2PDEN				0x88
#define EMC_RW2PDEN				0x8c
#define EMC_TXSR				0x90
#define EMC_TCKE				0x94
#define EMC_TFAW				0x98
#define EMC_TRPAB				0x9c
#define EMC_TCLKSTABLE				0xa0
#define EMC_TCLKSTOP				0xa4
#define EMC_TREFBW				0xa8
#define EMC_QUSE_EXTRA				0xac
#define EMC_ODT_WRITE				0xb0
#define EMC_ODT_READ				0xb4
#define EMC_WEXT				0xb8
#define EMC_CTT					0xbc

#define EMC_MRS_WAIT_CNT			0xc8
#define EMC_MRS_WAIT_CNT_SHORT_WAIT_SHIFT	0
#define EMC_MRS_WAIT_CNT_SHORT_WAIT_MASK	\
	(0x3FF << EMC_MRS_WAIT_CNT_SHORT_WAIT_SHIFT)
#define EMC_MRS_WAIT_CNT_LONG_WAIT_SHIFT	16
#define EMC_MRS_WAIT_CNT_LONG_WAIT_MASK		\
	(0x3FF << EMC_MRS_WAIT_CNT_LONG_WAIT_SHIFT)

#define EMC_MRS					0xcc
#define EMC_MODE_SET_DLL_RESET			(0x1 << 8)
#define EMC_MODE_SET_LONG_CNT			(0x1 << 26)
#define EMC_EMRS				0xd0
#define EMC_REF					0xd4
#define EMC_REF_FORCE_CMD			1

#define EMC_SELF_REF				0xe0
#define EMC_SELF_REF_CMD_ENABLED		(0x1 << 0)
#define EMC_SELF_REF_DEV_SEL_SHIFT		30
#define EMC_SELF_REF_DEV_SEL_MASK		(0x3 << EMC_SELF_REF_DEV_SEL_SHIFT)
enum {
	DRAM_DEV_SEL_ALL = 0,
	DRAM_DEV_SEL_0	 = (2 << EMC_SELF_REF_DEV_SEL_SHIFT),
	DRAM_DEV_SEL_1   = (1 << EMC_SELF_REF_DEV_SEL_SHIFT),
};
#define DRAM_BROADCAST(num)			\
	(((num) > 1) ? DRAM_DEV_SEL_ALL : DRAM_DEV_SEL_0)

#define EMC_MRW					0xe8
#define EMC_MRR					0xec
#define EMC_MRR_MA_SHIFT			16
#define EMC_MRR_MA_MASK				(0xFF << EMC_MRR_MA_SHIFT)
#define EMC_MRR_DATA_MASK			((0x1 << EMC_MRR_MA_SHIFT) - 1)
#define LPDDR2_MR4_TEMP_SHIFT			0
#define LPDDR2_MR4_TEMP_MASK			(0x7 << LPDDR2_MR4_TEMP_SHIFT)

#define EMC_XM2DQSPADCTRL3			0xf8
#define EMC_XM2DQSPADCTRL3_VREF_ENABLE		(0x1 << 5)
#define EMC_FBIO_SPARE				0x100

#define EMC_FBIO_CFG5				0x104
#define EMC_CFG5_TYPE_SHIFT			0x0
#define EMC_CFG5_TYPE_MASK			(0x3 << EMC_CFG5_TYPE_SHIFT)
enum {
	DRAM_TYPE_DDR3   = 0,
	DRAM_TYPE_LPDDR2 = 2,
};
#define EMC_CFG5_QUSE_MODE_SHIFT		13
#define EMC_CFG5_QUSE_MODE_MASK			(0x7 << EMC_CFG5_QUSE_MODE_SHIFT)
enum {
	EMC_CFG5_QUSE_MODE_NORMAL = 0,
	EMC_CFG5_QUSE_MODE_ALWAYS_ON,
	EMC_CFG5_QUSE_MODE_INTERNAL_LPBK,
	EMC_CFG5_QUSE_MODE_PULSE_INTERN,
	EMC_CFG5_QUSE_MODE_PULSE_EXTERN,
};

#define EMC_FBIO_CFG6				0x114
#define EMC_CFG_RSV				0x120
#define EMC_AUTO_CAL_CONFIG			0x2a4
#define EMC_AUTO_CAL_INTERVAL			0x2a8
#define EMC_AUTO_CAL_STATUS			0x2ac
#define EMC_AUTO_CAL_STATUS_ACTIVE		(0x1 << 31)
#define EMC_STATUS				0x2b4
#define EMC_STATUS_TIMING_UPDATE_STALLED	(0x1 << 23)
#define EMC_STATUS_MRR_DIVLD			(0x1 << 20)

#define EMC_CFG_2				0x2b8
#define EMC_CFG_2_MODE_SHIFT			0
#define EMC_CFG_2_MODE_MASK			(0x7 << EMC_CFG_2_MODE_SHIFT)
#define EMC_CFG_2_SREF_MODE			0x1
#define EMC_CFG_2_PD_MODE			0x3

#define EMC_CFG_DIG_DLL				0x2bc
#define EMC_CFG_DIG_DLL_PERIOD			0x2c0
#define EMC_CTT_DURATION			0x2d8
#define EMC_CTT_TERM_CTRL			0x2dc
#define EMC_ZCAL_INTERVAL			0x2e0
#define EMC_ZCAL_WAIT_CNT			0x2e4

#define EMC_ZQ_CAL				0x2ec
#define EMC_ZQ_CAL_CMD				(0x1 << 0)
#define EMC_ZQ_CAL_LONG				(0x1 << 4)
#define EMC_ZQ_CAL_LONG_CMD_DEV0		\
	(DRAM_DEV_SEL_0 | EMC_ZQ_CAL_LONG | EMC_ZQ_CAL_CMD)
#define EMC_ZQ_CAL_LONG_CMD_DEV1		\
	(DRAM_DEV_SEL_1 | EMC_ZQ_CAL_LONG | EMC_ZQ_CAL_CMD)

#define EMC_XM2CMDPADCTRL			0x2f0
#define EMC_XM2DQSPADCTRL2			0x2fc
#define EMC_XM2DQSPADCTRL2_VREF_ENABLE		(0x1 << 5)
#define EMC_XM2DQPADCTRL2			0x304
#define EMC_XM2CLKPADCTRL			0x308
#define EMC_XM2COMPPADCTRL			0x30c
#define EMC_XM2COMPPADCTRL_VREF_CAL_ENABLE	(0x1 << 10)
#define EMC_XM2VTTGENPADCTRL			0x310
#define EMC_XM2VTTGENPADCTRL2			0x314
#define EMC_XM2QUSEPADCTRL			0x318
#define EMC_XM2QUSEPADCTRL_IVREF_ENABLE		(0x1 << 4)
#define EMC_DLL_XFORM_DQS0			0x328
#define EMC_DLL_XFORM_DQS1			0x32c
#define EMC_DLL_XFORM_DQS2			0x330
#define EMC_DLL_XFORM_DQS3			0x334
#define EMC_DLL_XFORM_DQS4			0x338
#define EMC_DLL_XFORM_DQS5			0x33c
#define EMC_DLL_XFORM_DQS6			0x340
#define EMC_DLL_XFORM_DQS7			0x344
#define EMC_DLL_XFORM_QUSE0			0x348
#define EMC_DLL_XFORM_QUSE1			0x34c
#define EMC_DLL_XFORM_QUSE2			0x350
#define EMC_DLL_XFORM_QUSE3			0x354
#define EMC_DLL_XFORM_QUSE4			0x358
#define EMC_DLL_XFORM_QUSE5			0x35c
#define EMC_DLL_XFORM_QUSE6			0x360
#define EMC_DLL_XFORM_QUSE7			0x364
#define EMC_DLL_XFORM_DQ0			0x368
#define EMC_DLL_XFORM_DQ1			0x36c
#define EMC_DLL_XFORM_DQ2			0x370
#define EMC_DLL_XFORM_DQ3			0x374
#define EMC_DLI_TRIM_TXDQS0			0x3a8
#define EMC_DLI_TRIM_TXDQS1			0x3ac
#define EMC_DLI_TRIM_TXDQS2			0x3b0
#define EMC_DLI_TRIM_TXDQS3			0x3b4
#define EMC_DLI_TRIM_TXDQS4			0x3b8
#define EMC_DLI_TRIM_TXDQS5			0x3bc
#define EMC_DLI_TRIM_TXDQS6			0x3c0
#define EMC_DLI_TRIM_TXDQS7			0x3c4
#define EMC_STALL_BEFORE_CLKCHANGE		0x3c8
#define EMC_STALL_AFTER_CLKCHANGE		0x3cc
#define EMC_UNSTALL_RW_AFTER_CLKCHANGE		0x3d0
#define EMC_SEL_DPD_CTRL			0x3d8
#define EMC_SEL_DPD_CTRL_QUSE_DPD_ENABLE	(0x1 << 9)
#define EMC_PRE_REFRESH_REQ_CNT			0x3dc
#define EMC_DYN_SELF_REF_CONTROL		0x3e0
#define EMC_TXSRDLL				0x3e4

#define MC_EMEM_ADR_CFG				0x54
#define MC_EMEM_ARB_CFG				0x90
#define MC_EMEM_ARB_OUTSTANDING_REQ		0x94
#define MC_EMEM_ARB_OUTSTANDING_REQ_MAX_SHIFT	0
#define MC_EMEM_ARB_OUTSTANDING_REQ_MAX_MASK	\
	(0x1FF << MC_EMEM_ARB_OUTSTANDING_REQ_MAX_SHIFT)
#define MC_EMEM_ARB_OUTSTANDING_REQ_HOLDOFF_OVERRIDE	(0x1 << 30)
#define MC_EMEM_ARB_OUTSTANDING_REQ_LIMIT_ENABLE	(0x1 << 31)
#define MC_EMEM_ARB_TIMING_RCD			0x98
#define MC_EMEM_ARB_TIMING_RP			0x9c
#define MC_EMEM_ARB_TIMING_RC			0xa0
#define MC_EMEM_ARB_TIMING_RAS			0xa4
#define MC_EMEM_ARB_TIMING_FAW			0xa8
#define MC_EMEM_ARB_TIMING_RRD			0xac
#define MC_EMEM_ARB_TIMING_RAP2PRE		0xb0
#define MC_EMEM_ARB_TIMING_WAP2PRE		0xb4
#define MC_EMEM_ARB_TIMING_R2R			0xb8
#define MC_EMEM_ARB_TIMING_W2W			0xbc
#define MC_EMEM_ARB_TIMING_R2W			0xc0
#define MC_EMEM_ARB_TIMING_W2R			0xc4
#define MC_EMEM_ARB_DA_TURNS			0xd0
#define MC_EMEM_ARB_DA_COVERS			0xd4
#define MC_EMEM_ARB_MISC0			0xd8
#define MC_EMEM_ARB_MISC0_EMC_SAME_FREQ		(0x1 << 27)
#define MC_EMEM_ARB_MISC1			0xdc
#define MC_EMEM_ARB_RING1_THROTTLE		0xe0
#define MC_EMEM_ARB_RING3_THROTTLE		0xe4
#define MC_EMEM_ARB_OVERRIDE			0xe8
#define MC_EMEM_ARB_OVERRIDE_EACK_MASK		(0x3 << 0)
#define MC_TIMING_CONTROL			0xfc
#define MC_RESERVED_RSV				0x3fc

#endif

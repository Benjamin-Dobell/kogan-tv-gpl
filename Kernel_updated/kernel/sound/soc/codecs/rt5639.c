/*
 * rt5639.c  --  RT5639 ALSA SoC audio codec driver
 *
 * Copyright 2011 Realtek Semiconductor Corp.
 * Author: Johnny Hsu <johnnyhsu@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "rt5639.h"

#define RT5639_DEMO 1 
#define RT5639_REG_RW 1
#define RT5639_DET_EXT_MIC 0 

#ifdef RT5639_DEMO
struct rt5639_init_reg {
	u8 reg;
	u16 val;
};

static struct rt5639_init_reg init_list[] = {
	{RT5639_DUMMY1		, 0x3701},//fa[12:13] = 1'b;fa[8~10]=1;fa[0]=1
	{RT5639_DEPOP_M1	, 0x0019},//8e[4:3] = 11'b; 8e[0] = 1'b
	{RT5639_DEPOP_M2	, 0x3100},//8f[13] = 1'b
	{RT5639_ADDA_CLK1	, 0x1114},//73[2] = 1'b
	{RT5639_MICBIAS		, 0x3030},//93[5:4] = 11'b
	{RT5639_PRIV_INDEX	, 0x003d},//PR3d[12] = 1'b
	{RT5639_PRIV_DATA	, 0x3600},
	{RT5639_CLS_D_OUT	, 0xa000},//8d[11] = 0'b
	{RT5639_PRIV_INDEX	, 0x001c},//PR1c = 0D21'h
	{RT5639_PRIV_DATA	, 0x0D21},
	
	{RT5639_PRIV_INDEX	, 0x001b},//PR1B = 0D21'h
	{RT5639_PRIV_DATA	, 0x0000},
	{RT5639_PRIV_INDEX	, 0x0012},//PR12 = 0aa8'h
	{RT5639_PRIV_DATA	, 0x0aa8},
	{RT5639_PRIV_INDEX	, 0x0014},//PR14 = 0aaa'h
	{RT5639_PRIV_DATA	, 0x0aaa},
	{RT5639_PRIV_INDEX	, 0x0020},//PR20 = 6110'h
	{RT5639_PRIV_DATA	, 0x6110},
	{RT5639_PRIV_INDEX	, 0x0021},//PR21 = e0e0'h
	{RT5639_PRIV_DATA	, 0xe0e0},
	{RT5639_PRIV_INDEX	, 0x0023},//PR23 = 1804'h
	{RT5639_PRIV_DATA	, 0x1804},
	/*playback*/
	{RT5639_STO_DAC_MIXER	, 0x1414},//Dig inf 1 -> Sto DAC mixer -> DACL
	{RT5639_OUT_L3_MIXER	, 0x01fe},//DACL1 -> OUTMIXL
	{RT5639_OUT_R3_MIXER	, 0x01fe},//DACR1 -> OUTMIXR
	{RT5639_HP_VOL		, 0x8888},//OUTMIX -> HPVOL
	{RT5639_HPO_MIXER	, 0xc000},//HPVOL -> HPOLMIX
//	{RT5639_HPO_MIXER	, 0xa000},//DAC1 -> HPOLMIX
	{RT5639_SPK_L_MIXER	, 0x0036},//DACL1 -> SPKMIXL
	{RT5639_SPK_R_MIXER	, 0x0036},//DACR1 -> SPKMIXR
	{RT5639_SPK_VOL		, 0x8888},//SPKMIX -> SPKVOL
	{RT5639_SPO_L_MIXER	, 0xe800},//SPKVOLL -> SPOLMIX
	{RT5639_SPO_R_MIXER	, 0x2800},//SPKVOLR -> SPORMIX
//	{RT5639_SPO_L_MIXER	, 0xb800},//DAC -> SPOLMIX
//	{RT5639_SPO_R_MIXER	, 0x1800},//DAC -> SPORMIX  
//	{RT5639_I2S1_SDP	, 0xD000},//change IIS1 and IIS2
	/*record*/
	{RT5639_IN1_IN2		, 0x5080},//IN1 boost 40db and differential mode
	{RT5639_IN3_IN4		, 0x0500},//IN2 boost 40db and signal ended mode
	{RT5639_REC_L2_MIXER	, 0x007d},//Mic1 -> RECMIXL
	{RT5639_REC_R2_MIXER	, 0x007d},//Mic1 -> RECMIXR
//	{RT5639_REC_L2_MIXER	, 0x006f},//Mic2 -> RECMIXL
//	{RT5639_REC_R2_MIXER	, 0x006f},//Mic2 -> RECMIXR
	{RT5639_STO_ADC_MIXER	, 0x3020},//ADC -> Sto ADC mixer

#if RT5639_DET_EXT_MIC
	{RT5639_MICBIAS		,0x3800},//enable MICBIAS short current
	{RT5639_GPIO_CTRL1	,0x8400},//set GPIO1 to IRQ
	{RT5639_GPIO_CTRL3	,0x0004},//set GPIO1 output
	{RT5639_IRQ_CTRL2	,0x8000},//set MICBIAS short current to IRQ   ( if sticky set regBE : 8800 )
#endif
};
#define RT5639_INIT_REG_LEN ARRAY_SIZE(init_list)

static int rt5639_reg_init(struct snd_soc_codec *codec)
{
	int i;

	for (i = 0; i < RT5639_INIT_REG_LEN; i++)
		snd_soc_write(codec, init_list[i].reg, init_list[i].val);

	return 0;
}
#endif

static const u16 rt5639_reg[RT5639_VENDOR_ID2 + 1] = {
	[RT5639_RESET] = 0x0008,
	[RT5639_SPK_VOL] = 0xc8c8,
	[RT5639_HP_VOL] = 0xc8c8,
	[RT5639_OUTPUT] = 0xc8c8,
	[RT5639_MONO_OUT] = 0x8000,
	[RT5639_INL_INR_VOL] = 0x0808,
	[RT5639_DAC1_DIG_VOL] = 0xafaf,
	[RT5639_DAC2_DIG_VOL] = 0xafaf,
	[RT5639_ADC_DIG_VOL] = 0x2f2f,
	[RT5639_ADC_DATA] = 0x2f2f,
	[RT5639_STO_ADC_MIXER] = 0x7060,
	[RT5639_MONO_ADC_MIXER] = 0x7070,
	[RT5639_AD_DA_MIXER] = 0x8080,
	[RT5639_STO_DAC_MIXER] = 0x5454,
	[RT5639_MONO_DAC_MIXER] = 0x5454,
	[RT5639_DIG_MIXER] = 0xaa00,
	[RT5639_DSP_PATH2] = 0xa000,
	[RT5639_REC_L2_MIXER] = 0x007f,
	[RT5639_REC_R2_MIXER] = 0x007f,
	[RT5639_HPO_MIXER] = 0xe000,
	[RT5639_SPK_L_MIXER] = 0x003e,
	[RT5639_SPK_R_MIXER] = 0x003e,
	[RT5639_SPO_L_MIXER] = 0xf800,
	[RT5639_SPO_R_MIXER] = 0x3800,
	[RT5639_SPO_CLSD_RATIO] = 0x0004,
	[RT5639_MONO_MIXER] = 0xfc00,
	[RT5639_OUT_L3_MIXER] = 0x01ff,
 	[RT5639_OUT_R3_MIXER] = 0x01ff,
	[RT5639_LOUT_MIXER] = 0xf000,
	[RT5639_PWR_ANLG1] = 0x00c0,
	[RT5639_I2S1_SDP] = 0x8000,
	[RT5639_I2S2_SDP] = 0x8000,
	[RT5639_I2S3_SDP] = 0x8000,
	[RT5639_ADDA_CLK1] = 0x1110,
	[RT5639_ADDA_CLK2] = 0x0c00,
	[RT5639_DMIC] = 0x1d00,
	[RT5639_ASRC_3] = 0x0008,
	[RT5639_HP_OVCD] = 0x0600,
	[RT5639_CLS_D_OVCD] = 0x0228,
	[RT5639_CLS_D_OUT] = 0xa800,
	[RT5639_DEPOP_M1] = 0x0004,
	[RT5639_DEPOP_M2] = 0x1100,
	[RT5639_DEPOP_M3] = 0x0646,
	[RT5639_CHARGE_PUMP] = 0x0c00,
	[RT5639_MICBIAS] = 0x3000,
	[RT5639_EQ_CTRL1] = 0x2080,
	[RT5639_DRC_AGC_1] = 0x2206,
	[RT5639_DRC_AGC_2] = 0x1f00,
	[RT5639_ANC_CTRL1] = 0x034b,
	[RT5639_ANC_CTRL2] = 0x0066,
	[RT5639_ANC_CTRL3] = 0x000b,
	[RT5639_GPIO_CTRL1] = 0x0400,
	[RT5639_DSP_CTRL3] = 0x2000,
	[RT5639_BASE_BACK] = 0x0013,
	[RT5639_MP3_PLUS1] = 0x0680,
	[RT5639_MP3_PLUS2] = 0x1c17,
	[RT5639_3D_HP] = 0x8c00,
	[RT5639_ADJ_HPF] = 0x2a20,
	[RT5639_HP_CALIB_AMP_DET] = 0x0400,
	[RT5639_SV_ZCD1] = 0x0809,
	[RT5639_VENDOR_ID1] = 0x10ec,
	[RT5639_VENDOR_ID2] = 0x6231,
};

static int rt5639_reset(struct snd_soc_codec *codec)
{
	return snd_soc_write(codec, RT5639_RESET, 0);
}

/**
 * rt5639_index_write - Write private register.
 * @codec: SoC audio codec device.
 * @reg: Private register index.
 * @value: Private register Data.
 *
 * Modify private register for advanced setting. It can be written through
 * private index (0x6a) and data (0x6c) register.
 *
 * Returns 0 for success or negative error code.
 */
static int rt5639_index_write(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	int ret;

	ret = snd_soc_write(codec, RT5639_PRIV_INDEX, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private addr: %d\n", ret);
		goto err;
	}
	ret = snd_soc_write(codec, RT5639_PRIV_DATA, value);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private value: %d\n", ret);
		goto err;
	}
	return 0;

err:
	return ret;
}

/**
 * rt5639_index_read - Read private register.
 * @codec: SoC audio codec device.
 * @reg: Private register index.
 *
 * Read advanced setting from private register. It can be read through
 * private index (0x6a) and data (0x6c) register.
 *
 * Returns private register value or negative error code.
 */
static unsigned int rt5639_index_read(
	struct snd_soc_codec *codec, unsigned int reg)
{
	int ret;

	ret = snd_soc_write(codec, RT5639_PRIV_INDEX, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private addr: %d\n", ret);
		return ret;
	}
	return snd_soc_read(codec, RT5639_PRIV_DATA);
}

/**
 * rt5639_index_update_bits - update private register bits
 * @codec: audio codec
 * @reg: Private register index.
 * @mask: register mask
 * @value: new value
 *
 * Writes new register value.
 *
 * Returns 1 for change, 0 for no change, or negative error code.
 */
static int rt5639_index_update_bits(struct snd_soc_codec *codec,
	unsigned int reg, unsigned int mask, unsigned int value)
{
	unsigned int old, new;
	int change, ret;

	ret = rt5639_index_read(codec, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to read private reg: %d\n", ret);
		goto err;
	}

	old = ret;
	new = (old & ~mask) | (value & mask);
	change = old != new;
	if (change) {
		ret = rt5639_index_write(codec, reg, new);
		if (ret < 0) {
			dev_err(codec->dev,
				"Failed to write private reg: %d\n", ret);
			goto err;
		}
	}
	return change;

err:
	return ret;
}

static int rt5639_volatile_register(
	struct snd_soc_codec *codec, unsigned int reg)
{
	switch (reg) {
	case RT5639_RESET:
	case RT5639_PRIV_DATA:
	case RT5639_ASRC_5:
	case RT5639_EQ_CTRL1:
	case RT5639_DRC_AGC_1:
	case RT5639_ANC_CTRL1:
	case RT5639_IRQ_CTRL2:
	case RT5639_INT_IRQ_ST:
	case RT5639_DSP_CTRL2:
	case RT5639_DSP_CTRL3:
	case RT5639_PGM_REG_ARR1:
	case RT5639_PGM_REG_ARR3:
		return 1;
	default:
		return 0;
	}
}

static int rt5639_readable_register(
	struct snd_soc_codec *codec, unsigned int reg)
{
	switch (reg) {
	case RT5639_RESET:
	case RT5639_SPK_VOL:
	case RT5639_HP_VOL:
	case RT5639_OUTPUT:
	case RT5639_MONO_OUT:
	case RT5639_IN1_IN2:
	case RT5639_IN3_IN4:
	case RT5639_INL_INR_VOL:
	case RT5639_DAC1_DIG_VOL:
	case RT5639_DAC2_DIG_VOL:
	case RT5639_DAC2_CTRL:
	case RT5639_ADC_DIG_VOL:
	case RT5639_ADC_DATA:
	case RT5639_ADC_BST_VOL:
	case RT5639_STO_ADC_MIXER:
	case RT5639_MONO_ADC_MIXER:
	case RT5639_AD_DA_MIXER:
	case RT5639_STO_DAC_MIXER:
	case RT5639_MONO_DAC_MIXER:
	case RT5639_DIG_MIXER:
	case RT5639_DSP_PATH1:
	case RT5639_DSP_PATH2:
	case RT5639_DIG_INF_DATA:
	case RT5639_REC_L1_MIXER:
	case RT5639_REC_L2_MIXER:
	case RT5639_REC_R1_MIXER:
	case RT5639_REC_R2_MIXER:
	case RT5639_HPO_MIXER:
	case RT5639_SPK_L_MIXER:
	case RT5639_SPK_R_MIXER:
	case RT5639_SPO_L_MIXER:
	case RT5639_SPO_R_MIXER:
	case RT5639_SPO_CLSD_RATIO:
	case RT5639_MONO_MIXER:
	case RT5639_OUT_L1_MIXER:
	case RT5639_OUT_L2_MIXER:
	case RT5639_OUT_L3_MIXER:
	case RT5639_OUT_R1_MIXER:
	case RT5639_OUT_R2_MIXER:
	case RT5639_OUT_R3_MIXER:
	case RT5639_LOUT_MIXER:
	case RT5639_PWR_DIG1:
	case RT5639_PWR_DIG2:
	case RT5639_PWR_ANLG1:
	case RT5639_PWR_ANLG2:
	case RT5639_PWR_MIXER:
	case RT5639_PWR_VOL:
	case RT5639_PRIV_INDEX:
	case RT5639_PRIV_DATA:
	case RT5639_I2S1_SDP:
	case RT5639_I2S2_SDP:
	case RT5639_I2S3_SDP:
	case RT5639_ADDA_CLK1:
	case RT5639_ADDA_CLK2:
	case RT5639_DMIC:
	case RT5639_GLB_CLK:
	case RT5639_PLL_CTRL1:
	case RT5639_PLL_CTRL2:
	case RT5639_ASRC_1:
	case RT5639_ASRC_2:
	case RT5639_ASRC_3:
	case RT5639_ASRC_4:
	case RT5639_ASRC_5:
	case RT5639_HP_OVCD:
	case RT5639_CLS_D_OVCD:
	case RT5639_CLS_D_OUT:
	case RT5639_DEPOP_M1:
	case RT5639_DEPOP_M2:
	case RT5639_DEPOP_M3:
	case RT5639_CHARGE_PUMP:
	case RT5639_PV_DET_SPK_G:
	case RT5639_MICBIAS:
	case RT5639_EQ_CTRL1:
	case RT5639_EQ_CTRL2:
	case RT5639_WIND_FILTER:
	case RT5639_DRC_AGC_1:
	case RT5639_DRC_AGC_2:
	case RT5639_DRC_AGC_3:
	case RT5639_SVOL_ZC:
	case RT5639_ANC_CTRL1:
	case RT5639_ANC_CTRL2:
	case RT5639_ANC_CTRL3:
	case RT5639_JD_CTRL:
	case RT5639_ANC_JD:
	case RT5639_IRQ_CTRL1:
	case RT5639_IRQ_CTRL2:
	case RT5639_INT_IRQ_ST:
	case RT5639_GPIO_CTRL1:
	case RT5639_GPIO_CTRL2:
	case RT5639_GPIO_CTRL3:
	case RT5639_DSP_CTRL1:
	case RT5639_DSP_CTRL2:
	case RT5639_DSP_CTRL3:
	case RT5639_DSP_CTRL4:
	case RT5639_PGM_REG_ARR1:
	case RT5639_PGM_REG_ARR2:
	case RT5639_PGM_REG_ARR3:
	case RT5639_PGM_REG_ARR4:
	case RT5639_PGM_REG_ARR5:
	case RT5639_SCB_FUNC:
	case RT5639_SCB_CTRL:
	case RT5639_BASE_BACK:
	case RT5639_MP3_PLUS1:
	case RT5639_MP3_PLUS2:
	case RT5639_3D_HP:
	case RT5639_ADJ_HPF:
	case RT5639_HP_CALIB_AMP_DET:
	case RT5639_HP_CALIB2:
	case RT5639_SV_ZCD1:
	case RT5639_SV_ZCD2:
	case RT5639_DUMMY1:
	case RT5639_DUMMY2:
	case RT5639_DUMMY3:
	case RT5639_VENDOR_ID:
	case RT5639_VENDOR_ID1:
	case RT5639_VENDOR_ID2:
		return 1;
	default:
		return 0;
	}
}

int rt5639_headset_detect(struct snd_soc_codec *codec, int jack_insert)
{
	int jack_type;

	if (jack_insert) {
		snd_soc_update_bits(codec, RT5639_PWR_ANLG1,
			RT5639_PWR_LDO2, RT5639_PWR_LDO2);
		snd_soc_update_bits(codec, RT5639_PWR_ANLG2,
			RT5639_PWR_MB1, RT5639_PWR_MB1);
		snd_soc_update_bits(codec, RT5639_MICBIAS,
			RT5639_MIC1_OVCD_MASK | RT5639_MIC1_OVTH_MASK |
			RT5639_PWR_CLK25M_MASK | RT5639_PWR_MB_MASK,
			RT5639_MIC1_OVCD_EN | RT5639_MIC1_OVTH_600UA |
			RT5639_PWR_MB_PU | RT5639_PWR_CLK25M_PU);
		snd_soc_update_bits(codec, RT5639_DUMMY1,
			0x1, 0x1);
		msleep(50);
		if (snd_soc_read(codec, RT5639_IRQ_CTRL2) & 0x8)
			jack_type = RT5639_HEADPHO_DET;
		else
			jack_type = RT5639_HEADSET_DET;
		snd_soc_update_bits(codec, RT5639_IRQ_CTRL2,
			RT5639_MB1_OC_CLR, 0);
	} else {
		snd_soc_update_bits(codec, RT5639_MICBIAS,
			RT5639_MIC1_OVCD_MASK,
			RT5639_MIC1_OVCD_DIS);

		jack_type = RT5639_NO_JACK;
	}

	return jack_type;
}
EXPORT_SYMBOL(rt5639_headset_detect);

static const DECLARE_TLV_DB_SCALE(out_vol_tlv, -4650, 150, 0);
static const DECLARE_TLV_DB_SCALE(dac_vol_tlv, -65625, 375, 0);
static const DECLARE_TLV_DB_SCALE(in_vol_tlv, -3450, 150, 0);
static const DECLARE_TLV_DB_SCALE(adc_vol_tlv, -17625, 375, 0);
static const DECLARE_TLV_DB_SCALE(adc_bst_tlv, 0, 1200, 0);

/* {0, +20, +24, +30, +35, +40, +44, +50, +52} dB */
static unsigned int bst_tlv[] = {
	TLV_DB_RANGE_HEAD(7),
	0, 0, TLV_DB_SCALE_ITEM(0, 0, 0),
	1, 1, TLV_DB_SCALE_ITEM(2000, 0, 0),
	2, 2, TLV_DB_SCALE_ITEM(2400, 0, 0),
	3, 5, TLV_DB_SCALE_ITEM(3000, 500, 0),
	6, 6, TLV_DB_SCALE_ITEM(4400, 0, 0),
	7, 7, TLV_DB_SCALE_ITEM(5000, 0, 0),
	8, 8, TLV_DB_SCALE_ITEM(5200, 0, 0),
};

static int rt5639_dmic_get(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.integer.value[0] = rt5639->dmic_en;

	return 0;
}

static int rt5639_dmic_put(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);

	if (rt5639->dmic_en == ucontrol->value.integer.value[0])
		return 0;

	rt5639->dmic_en = ucontrol->value.integer.value[0];
	switch (rt5639->dmic_en) {
	case RT5639_DMIC_DIS:
		snd_soc_update_bits(codec, RT5639_GPIO_CTRL1,
			RT5639_GP2_PIN_MASK | RT5639_GP3_PIN_MASK |
			RT5639_GP4_PIN_MASK,
			RT5639_GP2_PIN_GPIO2 | RT5639_GP3_PIN_GPIO3 |
			RT5639_GP4_PIN_GPIO4);
		snd_soc_update_bits(codec, RT5639_DMIC,
			RT5639_DMIC_1_DP_MASK | RT5639_DMIC_2_DP_MASK,
			RT5639_DMIC_1_DP_GPIO3 | RT5639_DMIC_2_DP_GPIO4);
		snd_soc_update_bits(codec, RT5639_DMIC,
			RT5639_DMIC_1_EN_MASK | RT5639_DMIC_2_EN_MASK,
			RT5639_DMIC_1_DIS | RT5639_DMIC_2_DIS);
		break;

	case RT5639_DMIC1:
		snd_soc_update_bits(codec, RT5639_GPIO_CTRL1,
			RT5639_GP2_PIN_MASK | RT5639_GP3_PIN_MASK,
			RT5639_GP2_PIN_DMIC1_SCL | RT5639_GP3_PIN_DMIC1_SDA);
		snd_soc_update_bits(codec, RT5639_DMIC,
			RT5639_DMIC_1L_LH_MASK | RT5639_DMIC_1R_LH_MASK |
			RT5639_DMIC_1_DP_MASK,
			RT5639_DMIC_1L_LH_FALLING | RT5639_DMIC_1R_LH_RISING |
			RT5639_DMIC_1_DP_IN1P);
		snd_soc_update_bits(codec, RT5639_DMIC,
			RT5639_DMIC_1_EN_MASK, RT5639_DMIC_1_EN);
		break;

	case RT5639_DMIC2:
		snd_soc_update_bits(codec, RT5639_GPIO_CTRL1,
			RT5639_GP2_PIN_MASK | RT5639_GP4_PIN_MASK,
			RT5639_GP2_PIN_DMIC1_SCL | RT5639_GP4_PIN_DMIC2_SDA);
		snd_soc_update_bits(codec, RT5639_DMIC,
			RT5639_DMIC_2L_LH_MASK | RT5639_DMIC_2R_LH_MASK |
			RT5639_DMIC_2_DP_MASK,
			RT5639_DMIC_2L_LH_FALLING | RT5639_DMIC_2R_LH_RISING |
			RT5639_DMIC_2_DP_IN1N);
		snd_soc_update_bits(codec, RT5639_DMIC,
			RT5639_DMIC_2_EN_MASK, RT5639_DMIC_2_EN);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}


/* IN1/IN2 Input Type */
static const char *rt5639_input_mode[] = {
	"Single ended", "Differential"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_in1_mode_enum, RT5639_IN1_IN2,
	RT5639_IN_SFT1, rt5639_input_mode);

static const SOC_ENUM_SINGLE_DECL(
	rt5639_in2_mode_enum, RT5639_IN3_IN4,
	RT5639_IN_SFT2, rt5639_input_mode);

/* Interface data select */
static const char *rt5639_data_select[] = {
	"Normal", "left copy to right", "right copy to left", "Swap"};

static const SOC_ENUM_SINGLE_DECL(rt5639_if1_dac_enum, RT5639_DIG_INF_DATA,
				RT5639_IF1_DAC_SEL_SFT, rt5639_data_select);

static const SOC_ENUM_SINGLE_DECL(rt5639_if1_adc_enum, RT5639_DIG_INF_DATA,
				RT5639_IF1_ADC_SEL_SFT, rt5639_data_select);

static const SOC_ENUM_SINGLE_DECL(rt5639_if2_dac_enum, RT5639_DIG_INF_DATA,
				RT5639_IF2_DAC_SEL_SFT, rt5639_data_select);

static const SOC_ENUM_SINGLE_DECL(rt5639_if2_adc_enum, RT5639_DIG_INF_DATA,
				RT5639_IF2_ADC_SEL_SFT, rt5639_data_select);

static const SOC_ENUM_SINGLE_DECL(rt5639_if3_dac_enum, RT5639_DIG_INF_DATA,
				RT5639_IF3_DAC_SEL_SFT, rt5639_data_select);

static const SOC_ENUM_SINGLE_DECL(rt5639_if3_adc_enum, RT5639_DIG_INF_DATA,
				RT5639_IF3_ADC_SEL_SFT, rt5639_data_select);

/* Class D speaker gain ratio */
static const char *rt5639_clsd_spk_ratio[] = {"1.66x", "1.83x", "1.94x", "2x",
	"2.11x", "2.22x", "2.33x", "2.44x", "2.55x", "2.66x", "2.77x"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_clsd_spk_ratio_enum, RT5639_CLS_D_OUT,
	RT5639_CLSD_RATIO_SFT, rt5639_clsd_spk_ratio);

/* DMIC */
static const char *rt5639_dmic_mode[] = {"Disable", "DMIC1", "DMIC2"};

static const SOC_ENUM_SINGLE_DECL(rt5639_dmic_enum, 0, 0, rt5639_dmic_mode);



#ifdef RT5639_REG_RW
#define REGVAL_MAX 0xffff
static unsigned int regctl_addr;
static int rt5639_regctl_info(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = REGVAL_MAX;
	return 0;
}

static int rt5639_regctl_get(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	ucontrol->value.integer.value[0] = regctl_addr;
	ucontrol->value.integer.value[1] = snd_soc_read(codec, regctl_addr);
	return 0;
}

static int rt5639_regctl_put(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	regctl_addr = ucontrol->value.integer.value[0];
	if(ucontrol->value.integer.value[1] <= REGVAL_MAX)
		snd_soc_write(codec, regctl_addr, ucontrol->value.integer.value[1]);
	return 0;
}
#endif


static int rt5639_vol_rescale_get(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int val = snd_soc_read(codec, mc->reg);

	ucontrol->value.integer.value[0] = RT5639_VOL_RSCL_MAX -
		((val & RT5639_L_VOL_MASK) >> mc->shift);
	ucontrol->value.integer.value[1] = RT5639_VOL_RSCL_MAX -
		(val & RT5639_R_VOL_MASK);

	return 0;
}

static int rt5639_vol_rescale_put(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int val, val2;

	val = RT5639_VOL_RSCL_MAX - ucontrol->value.integer.value[0];
	val2 = RT5639_VOL_RSCL_MAX - ucontrol->value.integer.value[1];
	return snd_soc_update_bits_locked(codec, mc->reg, RT5639_L_VOL_MASK |
			RT5639_R_VOL_MASK, val << mc->shift | val2);
}


static const struct snd_kcontrol_new rt5639_snd_controls[] = {
	/* Speaker Output Volume */
	SOC_DOUBLE_EXT_TLV("Speaker Playback Volume", RT5639_SPK_VOL,
		RT5639_L_VOL_SFT, RT5639_R_VOL_SFT, RT5639_VOL_RSCL_RANGE, 0,
		rt5639_vol_rescale_get, rt5639_vol_rescale_put, out_vol_tlv),
	/* Headphone Output Volume */
	SOC_DOUBLE("HP Playback Switch", RT5639_HP_VOL,
		RT5639_L_MUTE_SFT, RT5639_R_MUTE_SFT, 1, 1),
	SOC_DOUBLE_EXT_TLV("HP Playback Volume", RT5639_HP_VOL,
		RT5639_L_VOL_SFT, RT5639_R_VOL_SFT, RT5639_VOL_RSCL_RANGE, 0,
		rt5639_vol_rescale_get, rt5639_vol_rescale_put, out_vol_tlv),
	/* OUTPUT Control */
	SOC_DOUBLE("OUT Playback Switch", RT5639_OUTPUT,
		RT5639_L_MUTE_SFT, RT5639_R_MUTE_SFT, 1, 1),
	SOC_DOUBLE("OUT Channel Switch", RT5639_OUTPUT,
		RT5639_VOL_L_SFT, RT5639_VOL_R_SFT, 1, 1),
	SOC_DOUBLE_TLV("OUT Playback Volume", RT5639_OUTPUT,
		RT5639_L_VOL_SFT, RT5639_R_VOL_SFT, 39, 1, out_vol_tlv),
	/* MONO Output Control */
	SOC_SINGLE("Mono Playback Switch", RT5639_MONO_OUT,
				RT5639_L_MUTE_SFT, 1, 1),
	/* DAC Digital Volume */
	SOC_DOUBLE("DAC2 Playback Switch", RT5639_DAC2_CTRL,
		RT5639_M_DAC_L2_VOL_SFT, RT5639_M_DAC_R2_VOL_SFT, 1, 1),
	SOC_DOUBLE_TLV("DAC1 Playback Volume", RT5639_DAC1_DIG_VOL,
			RT5639_L_VOL_SFT, RT5639_R_VOL_SFT,
			175, 0, dac_vol_tlv),
	SOC_DOUBLE_TLV("Mono DAC Playback Volume", RT5639_DAC2_DIG_VOL,
			RT5639_L_VOL_SFT, RT5639_R_VOL_SFT,
			175, 0, dac_vol_tlv),
	/* IN1/IN2 Control */
	SOC_ENUM("IN1 Mode Control",  rt5639_in1_mode_enum),
	SOC_SINGLE_TLV("IN1 Boost", RT5639_IN1_IN2,
		RT5639_BST_SFT1, 8, 0, bst_tlv),
	SOC_ENUM("IN2 Mode Control", rt5639_in2_mode_enum),
	SOC_SINGLE_TLV("IN2 Boost", RT5639_IN3_IN4,
		RT5639_BST_SFT2, 8, 0, bst_tlv),
	/* INL/INR Volume Control */
	SOC_DOUBLE_TLV("IN Capture Volume", RT5639_INL_INR_VOL,
			RT5639_INL_VOL_SFT, RT5639_INR_VOL_SFT,
			31, 1, in_vol_tlv),
	/* ADC Digital Volume Control */
	SOC_DOUBLE("ADC Capture Switch", RT5639_ADC_DIG_VOL,
		RT5639_L_MUTE_SFT, RT5639_R_MUTE_SFT, 1, 1),
	SOC_DOUBLE_TLV("ADC Capture Volume", RT5639_ADC_DIG_VOL,
			RT5639_L_VOL_SFT, RT5639_R_VOL_SFT,
			127, 0, adc_vol_tlv),
	SOC_DOUBLE_TLV("Mono ADC Capture Volume", RT5639_ADC_DATA,
			RT5639_L_VOL_SFT, RT5639_R_VOL_SFT,
			127, 0, adc_vol_tlv),
	/* ADC Boost Volume Control */
	SOC_DOUBLE_TLV("ADC Boost Gain", RT5639_ADC_BST_VOL,
			RT5639_ADC_L_BST_SFT, RT5639_ADC_R_BST_SFT,
			3, 0, adc_bst_tlv),
	/* Class D speaker gain ratio */
	SOC_ENUM("Class D SPK Ratio Control", rt5639_clsd_spk_ratio_enum),
	/* DMIC */
	SOC_ENUM_EXT("DMIC Switch", rt5639_dmic_enum,
		rt5639_dmic_get, rt5639_dmic_put),

#ifdef RT5639_REG_RW
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name = "Register Control",
		.info = rt5639_regctl_info,
		.get = rt5639_regctl_get,
		.put = rt5639_regctl_put,
	},
#endif
};

/**
 * set_dmic_clk - Set parameter of dmic.
 *
 * @w: DAPM widget.
 * @kcontrol: The kcontrol of this widget.
 * @event: Event id.
 *
 * Choose dmic clock between 1MHz and 3MHz.
 * It is better for clock to approximate 3MHz.
 */
static int set_dmic_clk(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);
	int div[] = {2, 3, 4, 6, 12}, idx = -EINVAL, i, rate, red, bound, temp;

	rate = rt5639->lrck[rt5639->aif_pu] << 8;
	red = 3000000 * 12;
	for (i = 0; i < ARRAY_SIZE(div); i++) {
		bound = div[i] * 3000000;
		if (rate > bound)
			continue;
		temp = bound - rate;
		if (temp < red) {
			red = temp;
			idx = i;
		}
	}
	if (idx < 0)
		dev_err(codec->dev, "Failed to set DMIC clock\n");
	else
		snd_soc_update_bits(codec, RT5639_DMIC, RT5639_DMIC_CLK_MASK,
					idx << RT5639_DMIC_CLK_SFT);
	return idx;
}

static int check_sysclk1_source(struct snd_soc_dapm_widget *source,
			 struct snd_soc_dapm_widget *sink)
{
	unsigned int val;

	val = snd_soc_read(source->codec, RT5639_GLB_CLK);
	val &= RT5639_SCLK_SRC_MASK;
	if (val == RT5639_SCLK_SRC_PLL1)
		return 1;
	else
		return 0;
}

/* Digital Mixer */
static const struct snd_kcontrol_new rt5639_sto_adc_l_mix[] = {
	SOC_DAPM_SINGLE("ADC1 Switch", RT5639_STO_ADC_MIXER,
			RT5639_M_ADC_L1_SFT, 1, 1),
	SOC_DAPM_SINGLE("ADC2 Switch", RT5639_STO_ADC_MIXER,
			RT5639_M_ADC_L2_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_sto_adc_r_mix[] = {
	SOC_DAPM_SINGLE("ADC1 Switch", RT5639_STO_ADC_MIXER,
			RT5639_M_ADC_R1_SFT, 1, 1),
	SOC_DAPM_SINGLE("ADC2 Switch", RT5639_STO_ADC_MIXER,
			RT5639_M_ADC_R2_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_mono_adc_l_mix[] = {
	SOC_DAPM_SINGLE("ADC1 Switch", RT5639_MONO_ADC_MIXER,
			RT5639_M_MONO_ADC_L1_SFT, 1, 1),
	SOC_DAPM_SINGLE("ADC2 Switch", RT5639_MONO_ADC_MIXER,
			RT5639_M_MONO_ADC_L2_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_mono_adc_r_mix[] = {
	SOC_DAPM_SINGLE("ADC1 Switch", RT5639_MONO_ADC_MIXER,
			RT5639_M_MONO_ADC_R1_SFT, 1, 1),
	SOC_DAPM_SINGLE("ADC2 Switch", RT5639_MONO_ADC_MIXER,
			RT5639_M_MONO_ADC_R2_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_dac_l_mix[] = {
	SOC_DAPM_SINGLE("Stereo ADC Switch", RT5639_AD_DA_MIXER,
			RT5639_M_ADCMIX_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("INF1 Switch", RT5639_AD_DA_MIXER,
			RT5639_M_IF1_DAC_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_dac_r_mix[] = {
	SOC_DAPM_SINGLE("Stereo ADC Switch", RT5639_AD_DA_MIXER,
			RT5639_M_ADCMIX_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("INF1 Switch", RT5639_AD_DA_MIXER,
			RT5639_M_IF1_DAC_R_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_sto_dac_l_mix[] = {
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5639_STO_DAC_MIXER,
			RT5639_M_DAC_L1_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L2 Switch", RT5639_STO_DAC_MIXER,
			RT5639_M_DAC_L2_SFT, 1, 1),
	SOC_DAPM_SINGLE("ANC Switch", RT5639_STO_DAC_MIXER,
			RT5639_M_ANC_DAC_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_sto_dac_r_mix[] = {
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5639_STO_DAC_MIXER,
			RT5639_M_DAC_R1_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R2 Switch", RT5639_STO_DAC_MIXER,
			RT5639_M_DAC_R2_SFT, 1, 1),
	SOC_DAPM_SINGLE("ANC Switch", RT5639_STO_DAC_MIXER,
			RT5639_M_ANC_DAC_R_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_mono_dac_l_mix[] = {
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5639_MONO_DAC_MIXER,
			RT5639_M_DAC_L1_MONO_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L2 Switch", RT5639_MONO_DAC_MIXER,
			RT5639_M_DAC_L2_MONO_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R2 Switch", RT5639_MONO_DAC_MIXER,
			RT5639_M_DAC_R2_MONO_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_mono_dac_r_mix[] = {
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5639_MONO_DAC_MIXER,
			RT5639_M_DAC_R1_MONO_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R2 Switch", RT5639_MONO_DAC_MIXER,
			RT5639_M_DAC_R2_MONO_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L2 Switch", RT5639_MONO_DAC_MIXER,
			RT5639_M_DAC_L2_MONO_R_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_dig_l_mix[] = {
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5639_DIG_MIXER,
			RT5639_M_STO_L_DAC_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L2 Switch", RT5639_DIG_MIXER,
			RT5639_M_DAC_L2_DAC_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_dig_r_mix[] = {
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5639_DIG_MIXER,
			RT5639_M_STO_R_DAC_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R2 Switch", RT5639_DIG_MIXER,
			RT5639_M_DAC_R2_DAC_R_SFT, 1, 1),
};

/* Analog Input Mixer */
static const struct snd_kcontrol_new rt5639_rec_l_mix[] = {
	SOC_DAPM_SINGLE("HPOL Switch", RT5639_REC_L2_MIXER,
			RT5639_M_HP_L_RM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("INL Switch", RT5639_REC_L2_MIXER,
			RT5639_M_IN_L_RM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST2 Switch", RT5639_REC_L2_MIXER,
			RT5639_M_BST4_RM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST1 Switch", RT5639_REC_L2_MIXER,
			RT5639_M_BST1_RM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUT MIXL Switch", RT5639_REC_L2_MIXER,
			RT5639_M_OM_L_RM_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_rec_r_mix[] = {
	SOC_DAPM_SINGLE("HPOR Switch", RT5639_REC_R2_MIXER,
			RT5639_M_HP_R_RM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("INR Switch", RT5639_REC_R2_MIXER,
			RT5639_M_IN_R_RM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST2 Switch", RT5639_REC_R2_MIXER,
			RT5639_M_BST4_RM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST1 Switch", RT5639_REC_R2_MIXER,
			RT5639_M_BST1_RM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUT MIXR Switch", RT5639_REC_R2_MIXER,
			RT5639_M_OM_R_RM_R_SFT, 1, 1),
};

/* Analog Output Mixer */
static const struct snd_kcontrol_new rt5639_spk_l_mix[] = {
	SOC_DAPM_SINGLE("REC MIXL Switch", RT5639_SPK_L_MIXER,
			RT5639_M_RM_L_SM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("INL Switch", RT5639_SPK_L_MIXER,
			RT5639_M_IN_L_SM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5639_SPK_L_MIXER,
			RT5639_M_DAC_L1_SM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L2 Switch", RT5639_SPK_L_MIXER,
			RT5639_M_DAC_L2_SM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUT MIXL Switch", RT5639_SPK_L_MIXER,
			RT5639_M_OM_L_SM_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_spk_r_mix[] = {
	SOC_DAPM_SINGLE("REC MIXR Switch", RT5639_SPK_R_MIXER,
			RT5639_M_RM_R_SM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("INR Switch", RT5639_SPK_R_MIXER,
			RT5639_M_IN_R_SM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5639_SPK_R_MIXER,
			RT5639_M_DAC_R1_SM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R2 Switch", RT5639_SPK_R_MIXER,
			RT5639_M_DAC_R2_SM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUT MIXR Switch", RT5639_SPK_R_MIXER,
			RT5639_M_OM_R_SM_R_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_out_l_mix[] = {
	SOC_DAPM_SINGLE("SPK MIXL Switch", RT5639_OUT_L3_MIXER,
			RT5639_M_SM_L_OM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST1 Switch", RT5639_OUT_L3_MIXER,
			RT5639_M_BST1_OM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("INL Switch", RT5639_OUT_L3_MIXER,
			RT5639_M_IN_L_OM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("REC MIXL Switch", RT5639_OUT_L3_MIXER,
			RT5639_M_RM_L_OM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R2 Switch", RT5639_OUT_L3_MIXER,
			RT5639_M_DAC_R2_OM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L2 Switch", RT5639_OUT_L3_MIXER,
			RT5639_M_DAC_L2_OM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5639_OUT_L3_MIXER,
			RT5639_M_DAC_L1_OM_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_out_r_mix[] = {
	SOC_DAPM_SINGLE("SPK MIXR Switch", RT5639_OUT_R3_MIXER,
			RT5639_M_SM_L_OM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST2 Switch", RT5639_OUT_R3_MIXER,
			RT5639_M_BST4_OM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST1 Switch", RT5639_OUT_R3_MIXER,
			RT5639_M_BST1_OM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("INR Switch", RT5639_OUT_R3_MIXER,
			RT5639_M_IN_R_OM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("REC MIXR Switch", RT5639_OUT_R3_MIXER,
			RT5639_M_RM_R_OM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L2 Switch", RT5639_OUT_R3_MIXER,
			RT5639_M_DAC_L2_OM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R2 Switch", RT5639_OUT_R3_MIXER,
			RT5639_M_DAC_R2_OM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5639_OUT_R3_MIXER,
			RT5639_M_DAC_R1_OM_R_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_spo_l_mix[] = {
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5639_SPO_L_MIXER,
			RT5639_M_DAC_R1_SPM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5639_SPO_L_MIXER,
			RT5639_M_DAC_L1_SPM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("SPKVOL R Switch", RT5639_SPO_L_MIXER,
			RT5639_M_SV_R_SPM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("SPKVOL L Switch", RT5639_SPO_L_MIXER,
			RT5639_M_SV_L_SPM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST1 Switch", RT5639_SPO_L_MIXER,
			RT5639_M_BST1_SPM_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_spo_r_mix[] = {
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5639_SPO_R_MIXER,
			RT5639_M_DAC_R1_SPM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("SPKVOL R Switch", RT5639_SPO_R_MIXER,
			RT5639_M_SV_R_SPM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST1 Switch", RT5639_SPO_R_MIXER,
			RT5639_M_BST1_SPM_R_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_hpo_mix[] = {
	SOC_DAPM_SINGLE("DAC2 Switch", RT5639_HPO_MIXER,
			RT5639_M_DAC2_HM_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC1 Switch", RT5639_HPO_MIXER,
			RT5639_M_DAC1_HM_SFT, 1, 1),
	SOC_DAPM_SINGLE("HPVOL Switch", RT5639_HPO_MIXER,
			RT5639_M_HPVOL_HM_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_lout_mix[] = {
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5639_LOUT_MIXER,
			RT5639_M_DAC_L1_LM_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5639_LOUT_MIXER,
			RT5639_M_DAC_R1_LM_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUTVOL L Switch", RT5639_LOUT_MIXER,
			RT5639_M_OV_L_LM_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUTVOL R Switch", RT5639_LOUT_MIXER,
			RT5639_M_OV_R_LM_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5639_mono_mix[] = {
	SOC_DAPM_SINGLE("DAC R2 Switch", RT5639_MONO_MIXER,
			RT5639_M_DAC_R2_MM_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L2 Switch", RT5639_MONO_MIXER,
			RT5639_M_DAC_L2_MM_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUTVOL R Switch", RT5639_MONO_MIXER,
			RT5639_M_OV_R_MM_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUTVOL L Switch", RT5639_MONO_MIXER,
			RT5639_M_OV_L_MM_SFT, 1, 1),
	SOC_DAPM_SINGLE("BST1 Switch", RT5639_MONO_MIXER,
			RT5639_M_BST1_MM_SFT, 1, 1),
};

/* INL/R source */
static const char *rt5639_inl_src[] = {"IN2P", "MonoP"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_inl_enum, RT5639_INL_INR_VOL,
	RT5639_INL_SEL_SFT, rt5639_inl_src);

static const struct snd_kcontrol_new rt5639_inl_mux =
	SOC_DAPM_ENUM("INL source", rt5639_inl_enum);

static const char *rt5639_inr_src[] = {"IN2N", "MonoN"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_inr_enum, RT5639_INL_INR_VOL,
	RT5639_INR_SEL_SFT, rt5639_inr_src);

static const struct snd_kcontrol_new rt5639_inr_mux =
	SOC_DAPM_ENUM("INR source", rt5639_inr_enum);

/* Stereo ADC source */
static const char *rt5639_stereo_adc1_src[] = {"DIG MIX", "ADC"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_stereo_adc1_enum, RT5639_STO_ADC_MIXER,
	RT5639_ADC_1_SRC_SFT, rt5639_stereo_adc1_src);

static const struct snd_kcontrol_new rt5639_sto_adc_l1_mux =
	SOC_DAPM_ENUM("Stereo ADC L1 source", rt5639_stereo_adc1_enum);

static const struct snd_kcontrol_new rt5639_sto_adc_r1_mux =
	SOC_DAPM_ENUM("Stereo ADC R1 source", rt5639_stereo_adc1_enum);

static const char *rt5639_stereo_adc2_src[] = {"DMIC1", "DMIC2", "DIG MIX"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_stereo_adc2_enum, RT5639_STO_ADC_MIXER,
	RT5639_ADC_2_SRC_SFT, rt5639_stereo_adc2_src);

static const struct snd_kcontrol_new rt5639_sto_adc_l2_mux =
	SOC_DAPM_ENUM("Stereo ADC L2 source", rt5639_stereo_adc2_enum);

static const struct snd_kcontrol_new rt5639_sto_adc_r2_mux =
	SOC_DAPM_ENUM("Stereo ADC R2 source", rt5639_stereo_adc2_enum);

/* Mono ADC source */
static const char *rt5639_mono_adc_l1_src[] = {"Mono DAC MIXL", "ADCL"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_mono_adc_l1_enum, RT5639_MONO_ADC_MIXER,
	RT5639_MONO_ADC_L1_SRC_SFT, rt5639_mono_adc_l1_src);

static const struct snd_kcontrol_new rt5639_mono_adc_l1_mux =
	SOC_DAPM_ENUM("Mono ADC1 left source", rt5639_mono_adc_l1_enum);

static const char *rt5639_mono_adc_l2_src[] =
	{"DMIC L1", "DMIC L2", "Mono DAC MIXL"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_mono_adc_l2_enum, RT5639_MONO_ADC_MIXER,
	RT5639_MONO_ADC_L2_SRC_SFT, rt5639_mono_adc_l2_src);

static const struct snd_kcontrol_new rt5639_mono_adc_l2_mux =
	SOC_DAPM_ENUM("Mono ADC2 left source", rt5639_mono_adc_l2_enum);

static const char *rt5639_mono_adc_r1_src[] = {"Mono DAC MIXR", "ADCR"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_mono_adc_r1_enum, RT5639_MONO_ADC_MIXER,
	RT5639_MONO_ADC_R1_SRC_SFT, rt5639_mono_adc_r1_src);

static const struct snd_kcontrol_new rt5639_mono_adc_r1_mux =
	SOC_DAPM_ENUM("Mono ADC1 right source", rt5639_mono_adc_r1_enum);

static const char *rt5639_mono_adc_r2_src[] =
	{"DMIC R1", "DMIC R2", "Mono DAC MIXR"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_mono_adc_r2_enum, RT5639_MONO_ADC_MIXER,
	RT5639_MONO_ADC_R2_SRC_SFT, rt5639_mono_adc_r2_src);

static const struct snd_kcontrol_new rt5639_mono_adc_r2_mux =
	SOC_DAPM_ENUM("Mono ADC2 right source", rt5639_mono_adc_r2_enum);

/* DAC2 channel source */
static const char *rt5639_dac_l2_src[] = {"IF2", "IF3", "TxDC", "Base L/R"};

static const SOC_ENUM_SINGLE_DECL(rt5639_dac_l2_enum, RT5639_DSP_PATH2,
				RT5639_DAC_L2_SEL_SFT, rt5639_dac_l2_src);

static const struct snd_kcontrol_new rt5639_dac_l2_mux =
	SOC_DAPM_ENUM("DAC2 left channel source", rt5639_dac_l2_enum);

static const char *rt5639_dac_r2_src[] = {"IF2", "IF3", "TxDC"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_dac_r2_enum, RT5639_DSP_PATH2,
	RT5639_DAC_R2_SEL_SFT, rt5639_dac_r2_src);

static const struct snd_kcontrol_new rt5639_dac_r2_mux =
	SOC_DAPM_ENUM("DAC2 right channel source", rt5639_dac_r2_enum);

/* Interface 2  ADC channel source */
static const char *rt5639_if2_adc_l_src[] = {"TxDP", "Mono ADC MIXL"};

static const SOC_ENUM_SINGLE_DECL(rt5639_if2_adc_l_enum, RT5639_DSP_PATH2,
			RT5639_IF2_ADC_L_SEL_SFT, rt5639_if2_adc_l_src);

static const struct snd_kcontrol_new rt5639_if2_adc_l_mux =
	SOC_DAPM_ENUM("IF2 ADC left channel source", rt5639_if2_adc_l_enum);

static const char *rt5639_if2_adc_r_src[] = {"TxDP", "Mono ADC MIXR"};

static const SOC_ENUM_SINGLE_DECL(rt5639_if2_adc_r_enum, RT5639_DSP_PATH2,
			RT5639_IF2_ADC_R_SEL_SFT, rt5639_if2_adc_r_src);

static const struct snd_kcontrol_new rt5639_if2_adc_r_mux =
	SOC_DAPM_ENUM("IF2 ADC right channel source", rt5639_if2_adc_r_enum);

/* digital interface and iis interface map */
static const char *rt5639_dai_iis_map[] = {"1:1|2:2|3:3", "1:1|2:3|3:2",
	"1:3|2:1|3:2", "1:3|2:2|3:1", "1:2|2:3|3:1",
	"1:2|2:1|3:3", "1:1|2:1|3:3", "1:2|2:2|3:3"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_dai_iis_map_enum, RT5639_I2S1_SDP,
	RT5639_I2S_IF_SFT, rt5639_dai_iis_map);

static const struct snd_kcontrol_new rt5639_dai_mux =
	SOC_DAPM_ENUM("DAI select", rt5639_dai_iis_map_enum);

/* SDI select */
static const char *rt5639_sdi_sel[] = {"IF1", "IF2"};

static const SOC_ENUM_SINGLE_DECL(
	rt5639_sdi_sel_enum, RT5639_I2S2_SDP,
	RT5639_I2S2_SDI_SFT, rt5639_sdi_sel);

static const struct snd_kcontrol_new rt5639_sdi_mux =
	SOC_DAPM_ENUM("SDI select", rt5639_sdi_sel_enum);

static int spk_event(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		printk("spk_event --SND_SOC_DAPM_POST_PMU\n");
		snd_soc_update_bits(codec, RT5639_PWR_DIG1,
			RT5639_PWR_CLS_D, RT5639_PWR_CLS_D);
		rt5639_index_update_bits(codec, 0x1c, 0xf000, 0xf000);
		//rt5639_index_write(codec,0x1c,0xfd21);
		break;

	case SND_SOC_DAPM_PRE_PMD:
		printk("spk_event --SND_SOC_DAPM_POST_PMD\n");
		//rt5639_index_write(codec,0x1c,0xfd00);
		rt5639_index_update_bits(codec, 0x1c, 0xf000, 0x0000);
		snd_soc_update_bits(codec,RT5639_PWR_DIG1,
			RT5639_PWR_CLS_D, 0);
		break;

	default:
		return 0;
	}

	return 0;
}

static int hp_event(struct snd_soc_dapm_widget *w, 
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		printk("hp_event --SND_SOC_DAPM_POST_PMU\n");
		break;

	case SND_SOC_DAPM_PRE_PMD:
		printk("hp_event --SND_SOC_DAPM_POST_PMD\n");
		break;

	default:
		return 0;
	}

	return 0;
}

static const struct snd_soc_dapm_widget rt5639_dapm_widgets[] = {
	SND_SOC_DAPM_SUPPLY("PLL1", RT5639_PWR_ANLG2,
			RT5639_PWR_PLL_BIT, 0, NULL, 0),
	/* Input Side */
	/* micbias */
	SND_SOC_DAPM_SUPPLY("LDO2", RT5639_PWR_ANLG1,
			RT5639_PWR_LDO2_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MICBIAS("micbias1", RT5639_PWR_ANLG2,
			RT5639_PWR_MB1_BIT, 0),
	SND_SOC_DAPM_MICBIAS("micbias2", RT5639_PWR_ANLG2,
			RT5639_PWR_MB2_BIT, 0),
	/* Input Lines */

	SND_SOC_DAPM_INPUT("MIC1"),
	SND_SOC_DAPM_INPUT("MIC2"),
	SND_SOC_DAPM_INPUT("DMIC1"),
	SND_SOC_DAPM_INPUT("DMIC2"),	

	SND_SOC_DAPM_INPUT("IN1P"),
	SND_SOC_DAPM_INPUT("IN1N"),
	SND_SOC_DAPM_INPUT("IN2P"),
	SND_SOC_DAPM_INPUT("IN2N"),
	SND_SOC_DAPM_INPUT("DMIC L1"),
	SND_SOC_DAPM_INPUT("DMIC R1"),
	SND_SOC_DAPM_INPUT("DMIC L2"),
	SND_SOC_DAPM_INPUT("DMIC R2"),
	SND_SOC_DAPM_SUPPLY("DMIC CLK", SND_SOC_NOPM, 0, 0,
		set_dmic_clk, SND_SOC_DAPM_PRE_PMU),
	/* Boost */
	SND_SOC_DAPM_PGA("BST1", RT5639_PWR_ANLG2,
		RT5639_PWR_BST1_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("BST2", RT5639_PWR_ANLG2,
		RT5639_PWR_BST4_BIT, 0, NULL, 0),
	/* Input Volume */
	SND_SOC_DAPM_PGA("INL VOL", RT5639_PWR_VOL,
		RT5639_PWR_IN_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("INR VOL", RT5639_PWR_VOL,
		RT5639_PWR_IN_R_BIT, 0, NULL, 0),
	/* IN Mux */
	SND_SOC_DAPM_MUX("INL Mux", SND_SOC_NOPM, 0, 0, &rt5639_inl_mux),
	SND_SOC_DAPM_MUX("INR Mux", SND_SOC_NOPM, 0, 0, &rt5639_inr_mux),
	/* REC Mixer */
	SND_SOC_DAPM_MIXER("RECMIXL", RT5639_PWR_MIXER, RT5639_PWR_RM_L_BIT, 0,
			rt5639_rec_l_mix, ARRAY_SIZE(rt5639_rec_l_mix)),
	SND_SOC_DAPM_MIXER("RECMIXR", RT5639_PWR_MIXER, RT5639_PWR_RM_R_BIT, 0,
			rt5639_rec_r_mix, ARRAY_SIZE(rt5639_rec_r_mix)),
	/* ADCs */
	SND_SOC_DAPM_ADC("ADC L", NULL, RT5639_PWR_DIG1,
			RT5639_PWR_ADC_L_BIT, 0),
	SND_SOC_DAPM_ADC("ADC R", NULL, RT5639_PWR_DIG1,
			RT5639_PWR_ADC_R_BIT, 0),
	/* ADC Mux */
	SND_SOC_DAPM_MUX("Stereo ADC L2 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_sto_adc_l2_mux),
	SND_SOC_DAPM_MUX("Stereo ADC R2 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_sto_adc_r2_mux),
	SND_SOC_DAPM_MUX("Stereo ADC L1 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_sto_adc_l1_mux),
	SND_SOC_DAPM_MUX("Stereo ADC R1 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_sto_adc_r1_mux),
	SND_SOC_DAPM_MUX("Mono ADC L2 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_mono_adc_l2_mux),
	SND_SOC_DAPM_MUX("Mono ADC L1 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_mono_adc_l1_mux),
	SND_SOC_DAPM_MUX("Mono ADC R1 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_mono_adc_r1_mux),
	SND_SOC_DAPM_MUX("Mono ADC R2 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_mono_adc_r2_mux),
	/* ADC Mixer */
	SND_SOC_DAPM_SUPPLY("stereo filter", RT5639_PWR_DIG2,
		RT5639_PWR_ADC_SF_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER("Stereo ADC MIXL", SND_SOC_NOPM, 0, 0,
		rt5639_sto_adc_l_mix, ARRAY_SIZE(rt5639_sto_adc_l_mix)),
	SND_SOC_DAPM_MIXER("Stereo ADC MIXR", SND_SOC_NOPM, 0, 0,
		rt5639_sto_adc_r_mix, ARRAY_SIZE(rt5639_sto_adc_r_mix)),
	SND_SOC_DAPM_SUPPLY("mono left filter", RT5639_PWR_DIG2,
		RT5639_PWR_ADC_MF_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER("Mono ADC MIXL", SND_SOC_NOPM, 0, 0,
		rt5639_mono_adc_l_mix, ARRAY_SIZE(rt5639_mono_adc_l_mix)),
	SND_SOC_DAPM_SUPPLY("mono right filter", RT5639_PWR_DIG2,
		RT5639_PWR_ADC_MF_R_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER("Mono ADC MIXR", SND_SOC_NOPM, 0, 0,
		rt5639_mono_adc_r_mix, ARRAY_SIZE(rt5639_mono_adc_r_mix)),

	/* IF2 Mux */
	SND_SOC_DAPM_MUX("IF2 ADC L Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_if2_adc_l_mux),
	SND_SOC_DAPM_MUX("IF2 ADC R Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_if2_adc_r_mux),

	/* Digital Interface */
	SND_SOC_DAPM_SUPPLY("I2S1", RT5639_PWR_DIG1,
		RT5639_PWR_I2S1_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 DAC", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 DAC L", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 DAC R", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 ADC", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 ADC L", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 ADC R", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("I2S2", RT5639_PWR_DIG1,
		RT5639_PWR_I2S2_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF2 DAC", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF2 DAC L", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF2 DAC R", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF2 ADC", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF2 ADC L", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF2 ADC R", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("I2S3", RT5639_PWR_DIG1,
		RT5639_PWR_I2S3_BIT, 0, NULL, 0),
 	SND_SOC_DAPM_PGA("IF3 DAC", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF3 DAC L", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF3 DAC R", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF3 ADC", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF3 ADC L", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF3 ADC R", SND_SOC_NOPM, 0, 0, NULL, 0),

	/* Digital Interface Select */
	SND_SOC_DAPM_MUX("DAI1 RX Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("DAI1 TX Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("DAI1 IF1 Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("DAI1 IF2 Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("SDI1 TX Mux", SND_SOC_NOPM, 0, 0, &rt5639_sdi_mux),

	SND_SOC_DAPM_MUX("DAI2 RX Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("DAI2 TX Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("DAI2 IF1 Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("DAI2 IF2 Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("SDI2 TX Mux", SND_SOC_NOPM, 0, 0, &rt5639_sdi_mux),

	SND_SOC_DAPM_MUX("DAI3 RX Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),
	SND_SOC_DAPM_MUX("DAI3 TX Mux", SND_SOC_NOPM, 0, 0, &rt5639_dai_mux),

	/* Audio Interface */
	SND_SOC_DAPM_AIF_IN("AIF1RX", "AIF1 Playback", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_OUT("AIF1TX", "AIF1 Capture", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_IN("AIF2RX", "AIF2 Playback", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_OUT("AIF2TX", "AIF2 Capture", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_IN("AIF3RX", "AIF3 Playback", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_OUT("AIF3TX", "AIF3 Capture", 0, SND_SOC_NOPM, 0, 0),

	/* Audio DSP */
	SND_SOC_DAPM_PGA("Audio DSP", SND_SOC_NOPM, 0, 0, NULL, 0),

	/* ANC */
	SND_SOC_DAPM_PGA("ANC", SND_SOC_NOPM, 0, 0, NULL, 0),

	/* Output Side */
	/* DAC mixer before sound effect  */
	SND_SOC_DAPM_MIXER("DAC MIXL", SND_SOC_NOPM, 0, 0,
		rt5639_dac_l_mix, ARRAY_SIZE(rt5639_dac_l_mix)),
	SND_SOC_DAPM_MIXER("DAC MIXR", SND_SOC_NOPM, 0, 0,
		rt5639_dac_r_mix, ARRAY_SIZE(rt5639_dac_r_mix)),

	/* DAC2 channel Mux */
	SND_SOC_DAPM_MUX("DAC L2 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_dac_l2_mux),
	SND_SOC_DAPM_MUX("DAC R2 Mux", SND_SOC_NOPM, 0, 0,
				&rt5639_dac_r2_mux),

	/* DAC Mixer */
	SND_SOC_DAPM_MIXER("Stereo DAC MIXL", SND_SOC_NOPM, 0, 0,
		rt5639_sto_dac_l_mix, ARRAY_SIZE(rt5639_sto_dac_l_mix)),
	SND_SOC_DAPM_MIXER("Stereo DAC MIXR", SND_SOC_NOPM, 0, 0,
		rt5639_sto_dac_r_mix, ARRAY_SIZE(rt5639_sto_dac_r_mix)),
	SND_SOC_DAPM_MIXER("Mono DAC MIXL", SND_SOC_NOPM, 0, 0,
		rt5639_mono_dac_l_mix, ARRAY_SIZE(rt5639_mono_dac_l_mix)),
	SND_SOC_DAPM_MIXER("Mono DAC MIXR", SND_SOC_NOPM, 0, 0,
		rt5639_mono_dac_r_mix, ARRAY_SIZE(rt5639_mono_dac_r_mix)),
	SND_SOC_DAPM_MIXER("DIG MIXL", SND_SOC_NOPM, 0, 0,
		rt5639_dig_l_mix, ARRAY_SIZE(rt5639_dig_l_mix)),
	SND_SOC_DAPM_MIXER("DIG MIXR", SND_SOC_NOPM, 0, 0,
		rt5639_dig_r_mix, ARRAY_SIZE(rt5639_dig_r_mix)),
	/* DACs */
	SND_SOC_DAPM_DAC("DAC L1", NULL, RT5639_PWR_DIG1,
			RT5639_PWR_DAC_L1_BIT, 0),
	SND_SOC_DAPM_DAC("DAC L2", NULL, RT5639_PWR_DIG1,
			RT5639_PWR_DAC_L2_BIT, 0),
	SND_SOC_DAPM_DAC("DAC R1", NULL, RT5639_PWR_DIG1,
			RT5639_PWR_DAC_R1_BIT, 0),
	SND_SOC_DAPM_DAC("DAC R2", NULL, RT5639_PWR_DIG1,
			RT5639_PWR_DAC_R2_BIT, 0),
	/* SPK/OUT Mixer */
	SND_SOC_DAPM_MIXER("SPK MIXL", RT5639_PWR_MIXER, RT5639_PWR_SM_L_BIT,
		0, rt5639_spk_l_mix, ARRAY_SIZE(rt5639_spk_l_mix)),
	SND_SOC_DAPM_MIXER("SPK MIXR", RT5639_PWR_MIXER, RT5639_PWR_SM_R_BIT,
		0, rt5639_spk_r_mix, ARRAY_SIZE(rt5639_spk_r_mix)),
	SND_SOC_DAPM_MIXER("OUT MIXL", RT5639_PWR_MIXER, RT5639_PWR_OM_L_BIT,
		0, rt5639_out_l_mix, ARRAY_SIZE(rt5639_out_l_mix)),
	SND_SOC_DAPM_MIXER("OUT MIXR", RT5639_PWR_MIXER, RT5639_PWR_OM_R_BIT,
		0, rt5639_out_r_mix, ARRAY_SIZE(rt5639_out_r_mix)),
	/* Ouput Volume */
	SND_SOC_DAPM_PGA("SPKVOL L", RT5639_PWR_VOL,
		RT5639_PWR_SV_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("SPKVOL R", RT5639_PWR_VOL,
		RT5639_PWR_SV_R_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("OUTVOL L", RT5639_PWR_VOL,
		RT5639_PWR_OV_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("OUTVOL R", RT5639_PWR_VOL,
		RT5639_PWR_OV_R_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("HPOVOL L", RT5639_PWR_VOL,
		RT5639_PWR_HV_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("HPOVOL R", RT5639_PWR_VOL,
		RT5639_PWR_HV_R_BIT, 0, NULL, 0),
	/* SPO/HPO/LOUT/Mono Mixer */
	SND_SOC_DAPM_MIXER("SPOL MIX",SND_SOC_NOPM, 0,
		0, rt5639_spo_l_mix, ARRAY_SIZE(rt5639_spo_l_mix)),
	SND_SOC_DAPM_MIXER("SPOR MIX", SND_SOC_NOPM, 0,
		0, rt5639_spo_r_mix, ARRAY_SIZE(rt5639_spo_r_mix)),
	SND_SOC_DAPM_MIXER("HPOL MIX", SND_SOC_NOPM, 0, 0,
		rt5639_hpo_mix, ARRAY_SIZE(rt5639_hpo_mix)),
	SND_SOC_DAPM_MIXER("HPOR MIX", SND_SOC_NOPM, 0, 0,
		rt5639_hpo_mix, ARRAY_SIZE(rt5639_hpo_mix)),
	SND_SOC_DAPM_MIXER("LOUT MIX", RT5639_PWR_ANLG1, RT5639_PWR_LM_BIT, 0,
		rt5639_lout_mix, ARRAY_SIZE(rt5639_lout_mix)),
	SND_SOC_DAPM_MIXER("Mono MIX", RT5639_PWR_ANLG1, RT5639_PWR_MM_BIT, 0,
		rt5639_mono_mix, ARRAY_SIZE(rt5639_mono_mix)),

	SND_SOC_DAPM_SUPPLY("Improve mono amp drv", RT5639_PWR_ANLG1,
		RT5639_PWR_MA_BIT, 0, NULL, 0),

	SND_SOC_DAPM_SUPPLY("Improve HP amp drv", RT5639_PWR_ANLG1,
		SND_SOC_NOPM, 0, hp_event,SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),

	SND_SOC_DAPM_PGA("HP L amp", RT5639_PWR_ANLG1,
		RT5639_PWR_HP_L_BIT, 0, NULL, 0),

	SND_SOC_DAPM_PGA("HP R amp", RT5639_PWR_ANLG1,
		RT5639_PWR_HP_R_BIT, 0, NULL, 0),

	SND_SOC_DAPM_SUPPLY("Improve SPK amp drv", SND_SOC_NOPM, 0, 0,
		spk_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),		

	/* Output Lines */
	SND_SOC_DAPM_OUTPUT("SPOLP"),
	SND_SOC_DAPM_OUTPUT("SPOLN"),
	SND_SOC_DAPM_OUTPUT("SPORP"),
	SND_SOC_DAPM_OUTPUT("SPORN"),
	SND_SOC_DAPM_OUTPUT("HPOL"),
	SND_SOC_DAPM_OUTPUT("HPOR"),
	SND_SOC_DAPM_OUTPUT("LOUTL"),
	SND_SOC_DAPM_OUTPUT("LOUTR"),
};

static const struct snd_soc_dapm_route rt5639_dapm_routes[] = {
	{"IN1P", NULL, "LDO2"},
	{"IN2P", NULL, "LDO2"},

	{"IN1P", NULL, "MIC1"},
	{"IN1N", NULL, "MIC1"},
	{"IN2P", NULL, "MIC2"},
	{"IN2N", NULL, "MIC2"},

	{"DMIC L1", NULL, "DMIC1"},
	{"DMIC R1", NULL, "DMIC1"},
	{"DMIC L2", NULL, "DMIC2"},
	{"DMIC R2", NULL, "DMIC2"},

	{"BST1", NULL, "IN1P"},
	{"BST1", NULL, "IN1N"},
	{"BST2", NULL, "IN2P"},
	{"BST2", NULL, "IN2N"},

	{"INL VOL", NULL, "IN2P"},
	{"INR VOL", NULL, "IN2N"},	
	
	{"RECMIXL", "HPOL Switch", "HPOL"},
	{"RECMIXL", "INL Switch", "INL VOL"},
	{"RECMIXL", "BST2 Switch", "BST2"},
	{"RECMIXL", "BST1 Switch", "BST1"},
	{"RECMIXL", "OUT MIXL Switch", "OUT MIXL"},

	{"RECMIXR", "HPOR Switch", "HPOR"},
	{"RECMIXR", "INR Switch", "INR VOL"},
	{"RECMIXR", "BST2 Switch", "BST2"},
	{"RECMIXR", "BST1 Switch", "BST1"},
	{"RECMIXR", "OUT MIXR Switch", "OUT MIXR"},

	{"ADC L", NULL, "RECMIXL"},
	{"ADC R", NULL, "RECMIXR"},

	{"DMIC L1", NULL, "DMIC CLK"},
	{"DMIC L2", NULL, "DMIC CLK"},

	{"Stereo ADC L2 Mux", "DMIC1", "DMIC L1"},
	{"Stereo ADC L2 Mux", "DMIC2", "DMIC L2"},
	{"Stereo ADC L2 Mux", "DIG MIX", "DIG MIXL"},
	{"Stereo ADC L1 Mux", "ADC", "ADC L"},
	{"Stereo ADC L1 Mux", "DIG MIX", "DIG MIXL"},

	{"Stereo ADC R1 Mux", "ADC", "ADC R"},
	{"Stereo ADC R1 Mux", "DIG MIX", "DIG MIXR"},
	{"Stereo ADC R2 Mux", "DMIC1", "DMIC R1"},
	{"Stereo ADC R2 Mux", "DMIC2", "DMIC R2"},
	{"Stereo ADC R2 Mux", "DIG MIX", "DIG MIXR"},

	{"Mono ADC L2 Mux", "DMIC L1", "DMIC L1"},
	{"Mono ADC L2 Mux", "DMIC L2", "DMIC L2"},
	{"Mono ADC L2 Mux", "Mono DAC MIXL", "Mono DAC MIXL"},
	{"Mono ADC L1 Mux", "Mono DAC MIXL", "Mono DAC MIXL"},
	{"Mono ADC L1 Mux", "ADCL", "ADC L"},

	{"Mono ADC R1 Mux", "Mono DAC MIXR", "Mono DAC MIXR"},
	{"Mono ADC R1 Mux", "ADCR", "ADC R"},
	{"Mono ADC R2 Mux", "DMIC R1", "DMIC R1"},
	{"Mono ADC R2 Mux", "DMIC R2", "DMIC R2"},
	{"Mono ADC R2 Mux", "Mono DAC MIXR", "Mono DAC MIXR"},

	{"Stereo ADC MIXL", "ADC1 Switch", "Stereo ADC L1 Mux"},
	{"Stereo ADC MIXL", "ADC2 Switch", "Stereo ADC L2 Mux"},
	{"Stereo ADC MIXL", NULL, "stereo filter"},
	{"stereo filter", NULL, "PLL1", check_sysclk1_source},

	{"Stereo ADC MIXR", "ADC1 Switch", "Stereo ADC R1 Mux"},
	{"Stereo ADC MIXR", "ADC2 Switch", "Stereo ADC R2 Mux"},
	{"Stereo ADC MIXR", NULL, "stereo filter"},
	{"stereo filter", NULL, "PLL1", check_sysclk1_source},

	{"Mono ADC MIXL", "ADC1 Switch", "Mono ADC L1 Mux"},
	{"Mono ADC MIXL", "ADC2 Switch", "Mono ADC L2 Mux"},
	{"Mono ADC MIXL", NULL, "mono left filter"},
	{"mono left filter", NULL, "PLL1", check_sysclk1_source},

	{"Mono ADC MIXR", "ADC1 Switch", "Mono ADC R1 Mux"},
	{"Mono ADC MIXR", "ADC2 Switch", "Mono ADC R2 Mux"},
	{"Mono ADC MIXR", NULL, "mono right filter"},
	{"mono right filter", NULL, "PLL1", check_sysclk1_source},

	{"IF2 ADC L Mux", "Mono ADC MIXL", "Mono ADC MIXL"},
	{"IF2 ADC R Mux", "Mono ADC MIXR", "Mono ADC MIXR"},

	{"IF2 ADC L", NULL, "IF2 ADC L Mux"},
	{"IF2 ADC R", NULL, "IF2 ADC R Mux"},
	{"IF3 ADC L", NULL, "Mono ADC MIXL"},
	{"IF3 ADC R", NULL, "Mono ADC MIXR"},
	{"IF1 ADC L", NULL, "Stereo ADC MIXL"},
	{"IF1 ADC R", NULL, "Stereo ADC MIXR"},

	{"IF1 ADC", NULL, "I2S1"},
	{"IF1 ADC", NULL, "IF1 ADC L"},
	{"IF1 ADC", NULL, "IF1 ADC R"},
	{"IF2 ADC", NULL, "I2S2"},
	{"IF2 ADC", NULL, "IF2 ADC L"},
	{"IF2 ADC", NULL, "IF2 ADC R"},
	{"IF3 ADC", NULL, "I2S3"},
	{"IF3 ADC", NULL, "IF3 ADC L"},
	{"IF3 ADC", NULL, "IF3 ADC R"},

	{"DAI1 TX Mux", "1:1|2:2|3:3", "IF1 ADC"},
	{"DAI1 TX Mux", "1:1|2:3|3:2", "IF1 ADC"},
	{"DAI1 TX Mux", "1:3|2:1|3:2", "IF2 ADC"},
	{"DAI1 TX Mux", "1:2|2:1|3:3", "IF2 ADC"},
	{"DAI1 TX Mux", "1:3|2:2|3:1", "IF3 ADC"},
	{"DAI1 TX Mux", "1:2|2:3|3:1", "IF3 ADC"},
	{"DAI1 IF1 Mux", "1:1|2:1|3:3", "IF1 ADC"},
	{"DAI1 IF2 Mux", "1:1|2:1|3:3", "IF2 ADC"},
	{"SDI1 TX Mux", "IF1", "DAI1 IF1 Mux"},
	{"SDI1 TX Mux", "IF2", "DAI1 IF2 Mux"},

	{"DAI2 TX Mux", "1:2|2:3|3:1", "IF1 ADC"},
	{"DAI2 TX Mux", "1:2|2:1|3:3", "IF1 ADC"},
	{"DAI2 TX Mux", "1:1|2:2|3:3", "IF2 ADC"},
	{"DAI2 TX Mux", "1:3|2:2|3:1", "IF2 ADC"},
	{"DAI2 TX Mux", "1:1|2:3|3:2", "IF3 ADC"},
	{"DAI2 TX Mux", "1:3|2:1|3:2", "IF3 ADC"},
	{"DAI2 IF1 Mux", "1:2|2:2|3:3", "IF1 ADC"},
	{"DAI2 IF2 Mux", "1:2|2:2|3:3", "IF2 ADC"},
	{"SDI2 TX Mux", "IF1", "DAI2 IF1 Mux"},
	{"SDI2 TX Mux", "IF2", "DAI2 IF2 Mux"},

	{"DAI3 TX Mux", "1:3|2:1|3:2", "IF1 ADC"},
	{"DAI3 TX Mux", "1:3|2:2|3:1", "IF1 ADC"},
	{"DAI3 TX Mux", "1:1|2:3|3:2", "IF2 ADC"},
	{"DAI3 TX Mux", "1:2|2:3|3:1", "IF2 ADC"},
	{"DAI3 TX Mux", "1:1|2:2|3:3", "IF3 ADC"},
	{"DAI3 TX Mux", "1:2|2:1|3:3", "IF3 ADC"},
	{"DAI3 TX Mux", "1:1|2:1|3:3", "IF3 ADC"},
	{"DAI3 TX Mux", "1:2|2:2|3:3", "IF3 ADC"},

	{"AIF1TX", NULL, "DAI1 TX Mux"},
	{"AIF1TX", NULL, "SDI1 TX Mux"},
	{"AIF2TX", NULL, "DAI2 TX Mux"},
	{"AIF2TX", NULL, "SDI2 TX Mux"},
	{"AIF3TX", NULL, "DAI3 TX Mux"},

	{"DAI1 RX Mux", "1:1|2:2|3:3", "AIF1RX"},
	{"DAI1 RX Mux", "1:1|2:3|3:2", "AIF1RX"},
	{"DAI1 RX Mux", "1:1|2:1|3:3", "AIF1RX"},
	{"DAI1 RX Mux", "1:2|2:3|3:1", "AIF2RX"},
	{"DAI1 RX Mux", "1:2|2:1|3:3", "AIF2RX"},
	{"DAI1 RX Mux", "1:2|2:2|3:3", "AIF2RX"},
	{"DAI1 RX Mux", "1:3|2:1|3:2", "AIF3RX"},
	{"DAI1 RX Mux", "1:3|2:2|3:1", "AIF3RX"},

	{"DAI2 RX Mux", "1:3|2:1|3:2", "AIF1RX"},
	{"DAI2 RX Mux", "1:2|2:1|3:3", "AIF1RX"},
	{"DAI2 RX Mux", "1:1|2:1|3:3", "AIF1RX"},
	{"DAI2 RX Mux", "1:1|2:2|3:3", "AIF2RX"},
	{"DAI2 RX Mux", "1:3|2:2|3:1", "AIF2RX"},
	{"DAI2 RX Mux", "1:2|2:2|3:3", "AIF2RX"},
	{"DAI2 RX Mux", "1:1|2:3|3:2", "AIF3RX"},
	{"DAI2 RX Mux", "1:2|2:3|3:1", "AIF3RX"},

	{"DAI3 RX Mux", "1:3|2:2|3:1", "AIF1RX"},
	{"DAI3 RX Mux", "1:2|2:3|3:1", "AIF1RX"},
	{"DAI3 RX Mux", "1:1|2:3|3:2", "AIF2RX"},
	{"DAI3 RX Mux", "1:3|2:1|3:2", "AIF2RX"},
	{"DAI3 RX Mux", "1:1|2:2|3:3", "AIF3RX"},
	{"DAI3 RX Mux", "1:2|2:1|3:3", "AIF3RX"},
	{"DAI3 RX Mux", "1:1|2:1|3:3", "AIF3RX"},
	{"DAI3 RX Mux", "1:2|2:2|3:3", "AIF3RX"},

	{"IF1 DAC", NULL, "I2S1"},
	{"IF1 DAC", NULL, "DAI1 RX Mux"},
	{"IF2 DAC", NULL, "I2S2"},
	{"IF2 DAC", NULL, "DAI2 RX Mux"},
	{"IF3 DAC", NULL, "I2S3"},
	{"IF3 DAC", NULL, "DAI3 RX Mux"},

	{"IF1 DAC L", NULL, "IF1 DAC"},
	{"IF1 DAC R", NULL, "IF1 DAC"},
	{"IF2 DAC L", NULL, "IF2 DAC"},
	{"IF2 DAC R", NULL, "IF2 DAC"},
	{"IF3 DAC L", NULL, "IF3 DAC"},
	{"IF3 DAC R", NULL, "IF3 DAC"},

	{"DAC MIXL", "Stereo ADC Switch", "Stereo ADC MIXL"},
	{"DAC MIXL", "INF1 Switch", "IF1 DAC L"},
	{"DAC MIXR", "Stereo ADC Switch", "Stereo ADC MIXR"},
	{"DAC MIXR", "INF1 Switch", "IF1 DAC R"},

	{"ANC", NULL, "Stereo ADC MIXL"},
	{"ANC", NULL, "Stereo ADC MIXR"},

	{"Audio DSP", NULL, "DAC MIXL"},
	{"Audio DSP", NULL, "DAC MIXR"},

	{"DAC L2 Mux", "IF2", "IF2 DAC L"},
	{"DAC L2 Mux", "IF3", "IF3 DAC L"},
	{"DAC L2 Mux", "Base L/R", "Audio DSP"},

	{"DAC R2 Mux", "IF2", "IF2 DAC R"},
	{"DAC R2 Mux", "IF3", "IF3 DAC R"},

	{"Stereo DAC MIXL", "DAC L1 Switch", "DAC MIXL"},
	{"Stereo DAC MIXL", "DAC L2 Switch", "DAC L2 Mux"},
	{"Stereo DAC MIXL", "ANC Switch", "ANC"},
	{"Stereo DAC MIXR", "DAC R1 Switch", "DAC MIXR"},
	{"Stereo DAC MIXR", "DAC R2 Switch", "DAC R2 Mux"},
	{"Stereo DAC MIXR", "ANC Switch", "ANC"},

	{"Mono DAC MIXL", "DAC L1 Switch", "DAC MIXL"},
	{"Mono DAC MIXL", "DAC L2 Switch", "DAC L2 Mux"},
	{"Mono DAC MIXL", "DAC R2 Switch", "DAC R2 Mux"},
	{"Mono DAC MIXR", "DAC R1 Switch", "DAC MIXR"},
	{"Mono DAC MIXR", "DAC R2 Switch", "DAC R2 Mux"},
	{"Mono DAC MIXR", "DAC L2 Switch", "DAC L2 Mux"},

	{"DIG MIXL", "DAC L1 Switch", "DAC MIXL"},
	{"DIG MIXL", "DAC L2 Switch", "DAC L2 Mux"},
	{"DIG MIXR", "DAC R1 Switch", "DAC MIXR"},
	{"DIG MIXR", "DAC R2 Switch", "DAC R2 Mux"},

	{"DAC L1", NULL, "Stereo DAC MIXL"},
	{"DAC L1", NULL, "PLL1", check_sysclk1_source},
	{"DAC R1", NULL, "Stereo DAC MIXR"},
	{"DAC R1", NULL, "PLL1", check_sysclk1_source},

	{"SPK MIXL", "REC MIXL Switch", "RECMIXL"},
	{"SPK MIXL", "INL Switch", "INL VOL"},
	{"SPK MIXL", "DAC L1 Switch", "DAC L1"},
	{"SPK MIXL", "DAC L2 Switch", "DAC L2"},
	{"SPK MIXL", "OUT MIXL Switch", "OUT MIXL"},
	{"SPK MIXR", "REC MIXR Switch", "RECMIXR"},
	{"SPK MIXR", "INR Switch", "INR VOL"},
	{"SPK MIXR", "DAC R1 Switch", "DAC R1"},
	{"SPK MIXR", "DAC R2 Switch", "DAC R2"},
	{"SPK MIXR", "OUT MIXR Switch", "OUT MIXR"},

	{"OUT MIXL", "SPK MIXL Switch", "SPK MIXL"},
	{"OUT MIXL", "BST1 Switch", "BST1"},
	{"OUT MIXL", "INL Switch", "INL VOL"},
	{"OUT MIXL", "REC MIXL Switch", "RECMIXL"},
	{"OUT MIXL", "DAC R2 Switch", "DAC R2"},
	{"OUT MIXL", "DAC L2 Switch", "DAC L2"},
	{"OUT MIXL", "DAC L1 Switch", "DAC L1"},

	{"OUT MIXR", "SPK MIXR Switch", "SPK MIXR"},
	{"OUT MIXR", "BST2 Switch", "BST2"},
	{"OUT MIXR", "BST1 Switch", "BST1"},
	{"OUT MIXR", "INR Switch", "INR VOL"},
	{"OUT MIXR", "REC MIXR Switch", "RECMIXR"},
	{"OUT MIXR", "DAC L2 Switch", "DAC L2"},
	{"OUT MIXR", "DAC R2 Switch", "DAC R2"},
	{"OUT MIXR", "DAC R1 Switch", "DAC R1"},

	{"SPKVOL L", NULL, "SPK MIXL"},
	{"SPKVOL R", NULL, "SPK MIXR"},
	{"HPOVOL L", NULL, "OUT MIXL"},
	{"HPOVOL R", NULL, "OUT MIXR"},
	{"OUTVOL L", NULL, "OUT MIXL"},
	{"OUTVOL R", NULL, "OUT MIXR"},

	{"SPOL MIX", "DAC R1 Switch", "DAC R1"},
	{"SPOL MIX", "DAC L1 Switch", "DAC L1"},
	{"SPOL MIX", "SPKVOL R Switch", "SPKVOL R"},
	{"SPOL MIX", "SPKVOL L Switch", "SPKVOL L"},
	{"SPOL MIX", "BST1 Switch", "BST1"},
	{"SPOR MIX", "DAC R1 Switch", "DAC R1"},
	{"SPOR MIX", "SPKVOL R Switch", "SPKVOL R"},
	{"SPOR MIX", "BST1 Switch", "BST1"},

	{"HPOL MIX", "DAC2 Switch", "DAC L2"},
	{"HPOL MIX", "DAC1 Switch", "DAC L1"},
	{"HPOL MIX", "HPVOL Switch", "HPOVOL L"},
	{"HPOR MIX", "DAC2 Switch", "DAC R2"},
	{"HPOR MIX", "DAC1 Switch", "DAC R1"},
	{"HPOR MIX", "HPVOL Switch", "HPOVOL R"},

	{"LOUT MIX", "DAC L1 Switch", "DAC L1"},
	{"LOUT MIX", "DAC R1 Switch", "DAC R1"},
	{"LOUT MIX", "OUTVOL L Switch", "OUTVOL L"},
	{"LOUT MIX", "OUTVOL R Switch", "OUTVOL R"},

	{"Mono MIX", "DAC R2 Switch", "DAC R2"},
	{"Mono MIX", "DAC L2 Switch", "DAC L2"},
	{"Mono MIX", "OUTVOL R Switch", "OUTVOL R"},
	{"Mono MIX", "OUTVOL L Switch", "OUTVOL L"},
	{"Mono MIX", "BST1 Switch", "BST1"},

	{"SPOLP", NULL, "Improve SPK amp drv"},
	{"SPOLN", NULL, "Improve SPK amp drv"},
	{"SPORP", NULL, "Improve SPK amp drv"},
	{"SPORN", NULL, "Improve SPK amp drv"},
	{"SPOLP", NULL, "SPOL MIX"},
	{"SPOLN", NULL, "SPOL MIX"},
	{"SPORP", NULL, "SPOR MIX"},
	{"SPORN", NULL, "SPOR MIX"},
	
	{"HP L amp", NULL, "HPOL MIX"},
	{"HP R amp", NULL, "HPOR MIX"},
	{"HPOL", NULL, "Improve HP amp drv"},
	{"HPOR", NULL, "Improve HP amp drv"},
	{"HPOL", NULL, "HP L amp"},
	{"HPOR", NULL, "HP R amp"},

	{"LOUTL", NULL, "LOUT MIX"},
	{"LOUTR", NULL, "LOUT MIX"},
};

static int get_sdp_info(struct snd_soc_codec *codec, int dai_id)
{
	int ret = 0, val = snd_soc_read(codec, RT5639_I2S1_SDP);

	if(codec == NULL)
		return -EINVAL;

	val = (val & RT5639_I2S_IF_MASK) >> RT5639_I2S_IF_SFT;
	switch (dai_id) {
	case RT5639_AIF1:
		if (val == RT5639_IF_123 || val == RT5639_IF_132 ||
			val == RT5639_IF_113)
			ret |= RT5639_U_IF1;
		if (val == RT5639_IF_312 || val == RT5639_IF_213 ||
			val == RT5639_IF_113)
			ret |= RT5639_U_IF2;
		if (val == RT5639_IF_321 || val == RT5639_IF_231)
			ret |= RT5639_U_IF3;
		break;

	case RT5639_AIF2:
		if (val == RT5639_IF_231 || val == RT5639_IF_213 ||
			val == RT5639_IF_223)
			ret |= RT5639_U_IF1;
		if (val == RT5639_IF_123 || val == RT5639_IF_321 ||
			val == RT5639_IF_223)
			ret |= RT5639_U_IF2;
		if (val == RT5639_IF_132 || val == RT5639_IF_312)
			ret |= RT5639_U_IF3;
		break;

	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int get_clk_info(int sclk, int rate)
{
	int i, pd[] = {1, 2, 3, 4, 6, 8, 12, 16};

	if(sclk <= 0 || rate <= 0)
		return -EINVAL;

	rate = rate << 8;
	for (i = 0; i < ARRAY_SIZE(pd); i++)
		if (sclk == rate * pd[i])
			return i;

	return -EINVAL;
}

static int rt5639_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);
	unsigned int val_len = 0, val_clk, mask_clk, dai_sel;
	int pre_div, bclk_ms, frame_size;

	rt5639->lrck[dai->id] = params_rate(params);
	pre_div = get_clk_info(rt5639->sysclk, rt5639->lrck[dai->id]);
	if (pre_div < 0) {
		dev_err(codec->dev, "Unsupported clock setting\n");
		return -EINVAL;
	}
	frame_size = snd_soc_params_to_frame_size(params);
	if (frame_size < 0) {
		dev_err(codec->dev, "Unsupported frame size: %d\n", frame_size);
		return -EINVAL;
	}
	bclk_ms = frame_size > 32 ? 1 : 0;
	rt5639->bclk[dai->id] = rt5639->lrck[dai->id] * (32 << bclk_ms);

	dev_dbg(dai->dev, "bclk is %dHz and lrck is %dHz\n",
		rt5639->bclk[dai->id], rt5639->lrck[dai->id]);
	dev_dbg(dai->dev, "bclk_ms is %d and pre_div is %d for iis %d\n",
				bclk_ms, pre_div, dai->id);

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		val_len |= RT5639_I2S_DL_20;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		val_len |= RT5639_I2S_DL_24;
		break;
	case SNDRV_PCM_FORMAT_S8:
		val_len |= RT5639_I2S_DL_8;
		break;
	default:
		return -EINVAL;
	}

	dai_sel = get_sdp_info(codec, dai->id);
	if (dai_sel < 0) {
		dev_err(codec->dev, "Failed to get sdp info: %d\n", dai_sel);
		return -EINVAL;
	}
	if (dai_sel & RT5639_U_IF1) {
 		mask_clk = RT5639_I2S_BCLK_MS1_MASK | RT5639_I2S_PD1_MASK;
		val_clk = bclk_ms << RT5639_I2S_BCLK_MS1_SFT |
			pre_div << RT5639_I2S_PD1_SFT;
		snd_soc_update_bits(codec, RT5639_I2S1_SDP,
			RT5639_I2S_DL_MASK, val_len);
		snd_soc_update_bits(codec, RT5639_ADDA_CLK1, mask_clk, val_clk);
	}
	if (dai_sel & RT5639_U_IF2) {
		mask_clk = RT5639_I2S_BCLK_MS2_MASK | RT5639_I2S_PD2_MASK;
		val_clk = bclk_ms << RT5639_I2S_BCLK_MS2_SFT |
			pre_div << RT5639_I2S_PD2_SFT;
		snd_soc_update_bits(codec, RT5639_I2S2_SDP,
			RT5639_I2S_DL_MASK, val_len);
		snd_soc_update_bits(codec, RT5639_ADDA_CLK1, mask_clk, val_clk);
	}

	return 0;
}

static int rt5639_prepare(struct snd_pcm_substream *substream,
				struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);

	rt5639->aif_pu = dai->id;
	return 0;
}

static int rt5639_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);
	unsigned int reg_val = 0, dai_sel;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		rt5639->master[dai->id] = 1;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		reg_val |= RT5639_I2S_MS_S;
		rt5639->master[dai->id] = 0;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_NF:
		reg_val |= RT5639_I2S_BP_INV;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		reg_val |= RT5639_I2S_DF_LEFT;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		reg_val |= RT5639_I2S_DF_PCM_A;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		reg_val |= RT5639_I2S_DF_PCM_B;
		break;
	default:
		return -EINVAL;
	}

	dai_sel = get_sdp_info(codec, dai->id);
	if (dai_sel < 0) {
		dev_err(codec->dev, "Failed to get sdp info: %d\n", dai_sel);
		return -EINVAL;
	}
	if (dai_sel & RT5639_U_IF1) {
		snd_soc_update_bits(codec, RT5639_I2S1_SDP,
			RT5639_I2S_MS_MASK | RT5639_I2S_BP_MASK |
			RT5639_I2S_DF_MASK, reg_val);
	}
	if (dai_sel & RT5639_U_IF2) {
		snd_soc_update_bits(codec, RT5639_I2S2_SDP,
			RT5639_I2S_MS_MASK | RT5639_I2S_BP_MASK |
			RT5639_I2S_DF_MASK, reg_val);
	}

	return 0;
}

static int rt5639_set_dai_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);
	unsigned int reg_val = 0;

	if (freq == rt5639->sysclk && clk_id == rt5639->sysclk_src)
		return 0;

	switch (clk_id) {
	case RT5639_SCLK_S_MCLK:
		reg_val |= RT5639_SCLK_SRC_MCLK;
		break;
	case RT5639_SCLK_S_PLL1:
		reg_val |= RT5639_SCLK_SRC_PLL1;
		break;
	case RT5639_SCLK_S_RCCLK:
		reg_val |= RT5639_SCLK_SRC_RCCLK;
		break;
	default:
		dev_err(codec->dev, "Invalid clock id (%d)\n", clk_id);
		return -EINVAL;
	}
	snd_soc_update_bits(codec, RT5639_GLB_CLK,
		RT5639_SCLK_SRC_MASK, reg_val);
	rt5639->sysclk = freq;
	rt5639->sysclk_src = clk_id;

	dev_dbg(dai->dev, "Sysclk is %dHz and clock id is %d\n", freq, clk_id);

	return 0;
}

/**
 * rt5639_pll_calc - Calcualte PLL M/N/K code.
 * @freq_in: external clock provided to codec.
 * @freq_out: target clock which codec works on.
 * @pll_code: Pointer to structure with M, N, K and bypass flag.
 *
 * Calcualte M/N/K code to configure PLL for codec. And K is assigned to 2
 * which make calculation more efficiently.
 *
 * Returns 0 for success or negative error code.
 */
static int rt5639_pll_calc(const unsigned int freq_in,
	const unsigned int freq_out, struct rt5639_pll_code *pll_code)
{
	int max_n = RT5639_PLL_N_MAX, max_m = RT5639_PLL_M_MAX;
	int n, m, red, n_t, m_t, in_t, out_t, red_t = abs(freq_out - freq_in);
	bool bypass = false;

	if (RT5639_PLL_INP_MAX < freq_in || RT5639_PLL_INP_MIN > freq_in)
		return -EINVAL;

	for (n_t = 0; n_t <= max_n; n_t++) {
		in_t = (freq_in >> 1) + (freq_in >> 2) * n_t;
		if (in_t < 0)
			continue;
		if (in_t == freq_out) {
			bypass = true;
			n = n_t;
			goto code_find;
		}
		for (m_t = 0; m_t <= max_m; m_t++) {
			out_t = in_t / (m_t + 2);
			red = abs(out_t - freq_out);
			if (red < red_t) {
				n = n_t;
				m = m_t;
				if(red == 0)
					goto code_find;
				red_t = red;
			}
		}
	}
	pr_debug("Only get approximation about PLL\n");

code_find:

	pll_code->m_bp = bypass;
	pll_code->m_code= m;
	pll_code->n_code = n;
	pll_code->k_code = 2;
	return 0;
}

static int rt5639_set_dai_pll(struct snd_soc_dai *dai, int pll_id, int source,
			unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);
	struct rt5639_pll_code pll_code;
	int ret, dai_sel;

	if (source == rt5639->pll_src && freq_in == rt5639->pll_in &&
	    freq_out == rt5639->pll_out)
		return 0;

	if (!freq_in || !freq_out) {
		dev_dbg(codec->dev, "PLL disabled\n");

		rt5639->pll_in = 0;
		rt5639->pll_out = 0;
		snd_soc_update_bits(codec, RT5639_GLB_CLK,
			RT5639_SCLK_SRC_MASK, RT5639_SCLK_SRC_MCLK);
		return 0;
	}

	switch (source) {
	case RT5639_PLL1_S_MCLK:
		snd_soc_update_bits(codec, RT5639_GLB_CLK,
			RT5639_PLL1_SRC_MASK, RT5639_PLL1_SRC_MCLK);
		break;
	case RT5639_PLL1_S_BCLK1:
	case RT5639_PLL1_S_BCLK2:
		dai_sel = get_sdp_info(codec, dai->id);
		if (dai_sel < 0) {
			dev_err(codec->dev,
				"Failed to get sdp info: %d\n", dai_sel);
			return -EINVAL;
		}
		if (dai_sel & RT5639_U_IF1) {
			snd_soc_update_bits(codec, RT5639_GLB_CLK,
				RT5639_PLL1_SRC_MASK, RT5639_PLL1_SRC_BCLK1);
		}
		if (dai_sel & RT5639_U_IF2) {
			snd_soc_update_bits(codec, RT5639_GLB_CLK,
				RT5639_PLL1_SRC_MASK, RT5639_PLL1_SRC_BCLK2);
		}
		if (dai_sel & RT5639_U_IF3) {
			snd_soc_update_bits(codec, RT5639_GLB_CLK,
				RT5639_PLL1_SRC_MASK, RT5639_PLL1_SRC_BCLK3);
		}
		break;
	default:
		dev_err(codec->dev, "Unknown PLL source %d\n", source);
		return -EINVAL;
	}

	ret = rt5639_pll_calc(freq_in, freq_out, &pll_code);
	if (ret < 0) {
		dev_err(codec->dev, "Unsupport input clock %d\n", freq_in);
		return ret;
	}

	dev_dbg(codec->dev, "bypass=%d m=%d n=%d k=2\n", pll_code.m_bp,
		(pll_code.m_bp ? 0 : pll_code.m_code), pll_code.n_code);

	snd_soc_write(codec, RT5639_PLL_CTRL1,
		pll_code.n_code << RT5639_PLL_N_SFT | pll_code.k_code);
	snd_soc_write(codec, RT5639_PLL_CTRL2,
		(pll_code.m_bp ? 0 : pll_code.m_code) << RT5639_PLL_M_SFT |
		pll_code.m_bp << RT5639_PLL_M_BP_SFT);

	rt5639->pll_in = freq_in;
	rt5639->pll_out = freq_out;
	rt5639->pll_src = source;

	return 0;
}

/**
 * rt5639_index_show - Dump private registers.
 * @dev: codec device.
 * @attr: device attribute.
 * @buf: buffer for display.
 *
 * To show non-zero values of all private registers.
 *
 * Returns buffer length.
 */
static ssize_t rt5639_index_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct rt5639_priv *rt5639 = i2c_get_clientdata(client);
	struct snd_soc_codec *codec = rt5639->codec;
	unsigned int val;
	int cnt = 0, i;

	cnt += sprintf(buf, "RT5639 index register\n");
	for (i = 0; i < 0xb4; i++) {
		if (cnt + RT5639_REG_DISP_LEN >= PAGE_SIZE)
			break;
		val = rt5639_index_read(codec, i);
		if (!val)
			continue;
		cnt += snprintf(buf + cnt, RT5639_REG_DISP_LEN,
				"%02x: %04x\n", i, val);
	}

	if (cnt >= PAGE_SIZE)
		cnt = PAGE_SIZE - 1;

	return cnt;
}
static DEVICE_ATTR(index_reg, 0444, rt5639_index_show, NULL);

static int rt5639_set_bias_level(struct snd_soc_codec *codec,
			enum snd_soc_bias_level level)
{
	switch (level) {
	case SND_SOC_BIAS_ON:
#ifdef RT5639_DEMO
		snd_soc_update_bits(codec, RT5639_SPK_VOL,
			RT5639_L_MUTE | RT5639_R_MUTE, 0);
		snd_soc_update_bits(codec, RT5639_HP_VOL,
			RT5639_L_MUTE | RT5639_R_MUTE, 0);
#endif
		break;

	case SND_SOC_BIAS_PREPARE:
#ifdef RT5639_DEMO
		snd_soc_update_bits(codec, RT5639_PWR_ANLG1,
			RT5639_PWR_VREF1 | RT5639_PWR_MB |
			RT5639_PWR_BG | RT5639_PWR_VREF2,
			RT5639_PWR_VREF1 | RT5639_PWR_MB |
			RT5639_PWR_BG | RT5639_PWR_VREF2);
		msleep(100);

		snd_soc_update_bits(codec, RT5639_PWR_ANLG1,
			RT5639_PWR_FV1 | RT5639_PWR_FV2,	
			RT5639_PWR_FV1 | RT5639_PWR_FV2);

		snd_soc_update_bits(codec, RT5639_PWR_ANLG2,
			RT5639_PWR_MB1 | RT5639_PWR_MB2,
			RT5639_PWR_MB1 | RT5639_PWR_MB2);
#endif
		break;

	case SND_SOC_BIAS_STANDBY:
#ifdef RT5639_DEMO
		snd_soc_update_bits(codec, RT5639_SPK_VOL,
			RT5639_L_MUTE | RT5639_R_MUTE, RT5639_L_MUTE | RT5639_R_MUTE);
		snd_soc_update_bits(codec, RT5639_HP_VOL,
			RT5639_L_MUTE | RT5639_R_MUTE, RT5639_L_MUTE | RT5639_R_MUTE);

		snd_soc_update_bits(codec, RT5639_PWR_ANLG2,
			RT5639_PWR_MB1 | RT5639_PWR_MB2,
			0);
#endif
		if (SND_SOC_BIAS_OFF == codec->dapm.bias_level) {
			snd_soc_update_bits(codec, RT5639_PWR_ANLG1,
				RT5639_PWR_VREF1 | RT5639_PWR_MB |
				RT5639_PWR_BG | RT5639_PWR_VREF2,
				RT5639_PWR_VREF1 | RT5639_PWR_MB |
				RT5639_PWR_BG | RT5639_PWR_VREF2);
			msleep(10);
			snd_soc_update_bits(codec, RT5639_PWR_ANLG1,
				RT5639_PWR_FV1 | RT5639_PWR_FV2,
				RT5639_PWR_FV1 | RT5639_PWR_FV2);
			codec->cache_only = false;
			snd_soc_cache_sync(codec);
		}
		break;

	case SND_SOC_BIAS_OFF:
#ifdef RT5639_DEMO
		snd_soc_update_bits(codec, RT5639_SPK_VOL,
			RT5639_L_MUTE | RT5639_R_MUTE, RT5639_L_MUTE | RT5639_R_MUTE);
		snd_soc_update_bits(codec, RT5639_HP_VOL,
			RT5639_L_MUTE | RT5639_R_MUTE, RT5639_L_MUTE | RT5639_R_MUTE);
		snd_soc_update_bits(codec, RT5639_OUTPUT,
			RT5639_L_MUTE | RT5639_R_MUTE, RT5639_L_MUTE | RT5639_R_MUTE);
		snd_soc_update_bits(codec, RT5639_MONO_OUT,
			RT5639_L_MUTE, RT5639_L_MUTE);
#endif
		snd_soc_write(codec, RT5639_PWR_DIG1, 0x0000);
		snd_soc_write(codec, RT5639_PWR_DIG2, 0x0000);
		snd_soc_write(codec, RT5639_PWR_VOL, 0x0000);
		snd_soc_write(codec, RT5639_PWR_MIXER, 0x0000);
		snd_soc_write(codec, RT5639_PWR_ANLG1, 0x0000);
		snd_soc_write(codec, RT5639_PWR_ANLG2, 0x0000);
		break;

	default:
		break;
	}
	codec->dapm.bias_level = level;

	return 0;
}

static int rt5639_probe(struct snd_soc_codec *codec)
{
	struct rt5639_priv *rt5639 = snd_soc_codec_get_drvdata(codec);
	int ret;

	codec->dapm.idle_bias_off = 1;

	ret = snd_soc_codec_set_cache_io(codec, 8, 16, SND_SOC_I2C);
	if (ret != 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}

	rt5639_reset(codec);
	snd_soc_update_bits(codec, RT5639_PWR_ANLG1,
		RT5639_PWR_VREF1 | RT5639_PWR_MB |
		RT5639_PWR_BG | RT5639_PWR_VREF2,
		RT5639_PWR_VREF1 | RT5639_PWR_MB |
		RT5639_PWR_BG | RT5639_PWR_VREF2);
	msleep(100);
	snd_soc_update_bits(codec, RT5639_PWR_ANLG1,
		RT5639_PWR_FV1 | RT5639_PWR_FV2,
		RT5639_PWR_FV1 | RT5639_PWR_FV2);
	/* DMIC */
	if (rt5639->dmic_en == RT5639_DMIC1) {
		snd_soc_update_bits(codec, RT5639_GPIO_CTRL1,
			RT5639_GP2_PIN_MASK, RT5639_GP2_PIN_DMIC1_SCL);
		snd_soc_update_bits(codec, RT5639_DMIC,
			RT5639_DMIC_1L_LH_MASK | RT5639_DMIC_1R_LH_MASK,
			RT5639_DMIC_1L_LH_FALLING | RT5639_DMIC_1R_LH_RISING);
	} else if (rt5639->dmic_en == RT5639_DMIC2) {
		snd_soc_update_bits(codec, RT5639_GPIO_CTRL1,
			RT5639_GP2_PIN_MASK, RT5639_GP2_PIN_DMIC1_SCL);
		snd_soc_update_bits(codec, RT5639_DMIC,
			RT5639_DMIC_2L_LH_MASK | RT5639_DMIC_2R_LH_MASK,
			RT5639_DMIC_2L_LH_FALLING | RT5639_DMIC_2R_LH_RISING);
	}

#ifdef RT5639_DEMO
	rt5639_reg_init(codec);
#endif

	codec->dapm.bias_level = SND_SOC_BIAS_STANDBY;

	snd_soc_add_controls(codec, rt5639_snd_controls,
		ARRAY_SIZE(rt5639_snd_controls));

	rt5639->codec = codec;
	ret = device_create_file(codec->dev, &dev_attr_index_reg);
	if (ret != 0) {
		dev_err(codec->dev,
			"Failed to create index_reg sysfs files: %d\n", ret);
		return ret;
	}

	return 0;
}

static int rt5639_remove(struct snd_soc_codec *codec)
{
	rt5639_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

#ifdef CONFIG_PM
static int rt5639_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	rt5639_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int rt5639_resume(struct snd_soc_codec *codec)
{
	rt5639_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	return 0;
}
#else
#define rt5639_suspend NULL
#define rt5639_resume NULL
#endif

#define RT5639_STEREO_RATES SNDRV_PCM_RATE_8000_96000
#define RT5639_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | \
			SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S8)

struct snd_soc_dai_ops rt5639_aif_dai_ops = {
	.hw_params = rt5639_hw_params,
	.prepare = rt5639_prepare,
	.set_fmt = rt5639_set_dai_fmt,
	.set_sysclk = rt5639_set_dai_sysclk,
	.set_pll = rt5639_set_dai_pll,
};

struct snd_soc_dai_driver rt5639_dai[] = {
	{
		.name = "rt5639-aif1",
		.id = RT5639_AIF1,
		.playback = {
			.stream_name = "AIF1 Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5639_STEREO_RATES,
			.formats = RT5639_FORMATS,
		},
		.capture = {
			.stream_name = "AIF1 Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5639_STEREO_RATES,
			.formats = RT5639_FORMATS,
		},
		.ops = &rt5639_aif_dai_ops,
	},
	{
		.name = "rt5639-aif2",
		.id = RT5639_AIF2,
		.playback = {
			.stream_name = "AIF2 Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5639_STEREO_RATES,
			.formats = RT5639_FORMATS,
		},
		.capture = {
			.stream_name = "AIF2 Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5639_STEREO_RATES,
			.formats = RT5639_FORMATS,
		},
		.ops = &rt5639_aif_dai_ops,
	},
};

static struct snd_soc_codec_driver soc_codec_dev_rt5639 = {
	.probe = rt5639_probe,
	.remove = rt5639_remove,
	.suspend = rt5639_suspend,
	.resume = rt5639_resume,
	.set_bias_level = rt5639_set_bias_level,
	.reg_cache_size = RT5639_VENDOR_ID2 + 1,
	.reg_word_size = sizeof(u16),
	.reg_cache_default = rt5639_reg,
	.volatile_register = rt5639_volatile_register,
	.readable_register = rt5639_readable_register,
	.reg_cache_step = 1,
	.dapm_widgets = rt5639_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(rt5639_dapm_widgets),
	.dapm_routes = rt5639_dapm_routes,
	.num_dapm_routes = ARRAY_SIZE(rt5639_dapm_routes),
};

static const struct i2c_device_id rt5639_i2c_id[] = {
	{ "rt5639", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, rt5639_i2c_id);

static int rt5639_i2c_probe(struct i2c_client *i2c,
		    const struct i2c_device_id *id)
{
	struct rt5639_priv *rt5639;
	int ret;

	rt5639 = kzalloc(sizeof(struct rt5639_priv), GFP_KERNEL);
	if (NULL == rt5639)
		return -ENOMEM;

	i2c_set_clientdata(i2c, rt5639);

	ret = snd_soc_register_codec(&i2c->dev, &soc_codec_dev_rt5639,
			rt5639_dai, ARRAY_SIZE(rt5639_dai));
	if (ret < 0)
		kfree(rt5639);

	return ret;
}

static __devexit int rt5639_i2c_remove(struct i2c_client *i2c)
{
	snd_soc_unregister_codec(&i2c->dev);
	kfree(i2c_get_clientdata(i2c));
	return 0;
}

struct i2c_driver rt5639_i2c_driver = {
	.driver = {
		.name = "rt5639",
		.owner = THIS_MODULE,
	},
	.probe = rt5639_i2c_probe,
	.remove   = __devexit_p(rt5639_i2c_remove),
	.id_table = rt5639_i2c_id,
};

static int __init rt5639_modinit(void)
{
	return i2c_add_driver(&rt5639_i2c_driver);
}
module_init(rt5639_modinit);

static void __exit rt5639_modexit(void)
{
	i2c_del_driver(&rt5639_i2c_driver);
}
module_exit(rt5639_modexit);

MODULE_DESCRIPTION("ASoC RT5639 driver");
MODULE_AUTHOR("Johnny Hsu <johnnyhsu@realtek.com>");
MODULE_LICENSE("GPL");

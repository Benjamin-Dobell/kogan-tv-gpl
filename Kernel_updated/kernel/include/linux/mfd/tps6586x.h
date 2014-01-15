#ifndef __LINUX_MFD_TPS6586X_H
#define __LINUX_MFD_TPS6586X_H

#define SM0_PWM_BIT 0
#define SM1_PWM_BIT 1
#define SM2_PWM_BIT 2

enum {
	TPS6586X_ID_SM_0,
	TPS6586X_ID_SM_1,
	TPS6586X_ID_SM_2,
	TPS6586X_ID_LDO_0,
	TPS6586X_ID_LDO_1,
	TPS6586X_ID_LDO_2,
	TPS6586X_ID_LDO_3,
	TPS6586X_ID_LDO_4,
	TPS6586X_ID_LDO_5,
	TPS6586X_ID_LDO_6,
	TPS6586X_ID_LDO_7,
	TPS6586X_ID_LDO_8,
	TPS6586X_ID_LDO_9,
	TPS6586X_ID_LDO_RTC,
};

enum {
	TPS6586X_INT_PLDO_0,
	TPS6586X_INT_PLDO_1,
	TPS6586X_INT_PLDO_2,
	TPS6586X_INT_PLDO_3,
	TPS6586X_INT_PLDO_4,
	TPS6586X_INT_PLDO_5,
	TPS6586X_INT_PLDO_6,
	TPS6586X_INT_PLDO_7,
	TPS6586X_INT_COMP_DET,
	TPS6586X_INT_ADC,
	TPS6586X_INT_PLDO_8,
	TPS6586X_INT_PLDO_9,
	TPS6586X_INT_PSM_0,
	TPS6586X_INT_PSM_1,
	TPS6586X_INT_PSM_2,
	TPS6586X_INT_PSM_3,
	TPS6586X_INT_RTC_ALM1,
	TPS6586X_INT_ACUSB_OVP,
	TPS6586X_INT_USB_DET,
	TPS6586X_INT_AC_DET,
	TPS6586X_INT_BAT_DET,
	TPS6586X_INT_CHG_STAT,
	TPS6586X_INT_CHG_TEMP,
	TPS6586X_INT_PP,
	TPS6586X_INT_RESUME,
	TPS6586X_INT_LOW_SYS,
	TPS6586X_INT_RTC_ALM2,
};

enum pwm_pfm_mode {
	PWM_ONLY,
	AUTO_PWM_PFM,
	PWM_DEFAULT_VALUE,

};

enum slew_rate_settings {
	SLEW_RATE_INSTANTLY = 0,
	SLEW_RATE_0110UV_PER_SEC = 0x1,
	SLEW_RATE_0220UV_PER_SEC = 0x2,
	SLEW_RATE_0440UV_PER_SEC = 0x3,
	SLEW_RATE_0880UV_PER_SEC = 0x4,
	SLEW_RATE_1760UV_PER_SEC = 0x5,
	SLEW_RATE_3520UV_PER_SEC = 0x6,
	SLEW_RATE_7040UV_PER_SEC = 0x7,
	SLEW_RATE_DEFAULT_VALUE,
};

struct tps6586x_settings {
	/* SM0, SM1 and SM2 have PWM-only and auto PWM/PFM mode */
	enum pwm_pfm_mode sm_pwm_mode;
	/* SM0 and SM1 have slew rate settings */
	enum slew_rate_settings slew_rate;
};

enum {
	TPS6586X_RTC_CL_SEL_1_5PF  = 0x0,
	TPS6586X_RTC_CL_SEL_6_5PF  = 0x1,
	TPS6586X_RTC_CL_SEL_7_5PF  = 0x2,
	TPS6586X_RTC_CL_SEL_12_5PF = 0x3,
};

struct tps6586x_subdev_info {
	int		id;
	const char	*name;
	void		*platform_data;
};

struct tps6586x_epoch_start {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
};

struct tps6586x_rtc_platform_data {
	int irq;
	struct tps6586x_epoch_start start;
	int cl_sel; /* internal XTAL capacitance, see TPS6586X_RTC_CL_SEL* */
};

struct tps6586x_platform_data {
	int num_subdevs;
	struct tps6586x_subdev_info *subdevs;

	int gpio_base;
	int irq_base;

	bool use_power_off;
};

/*
 * NOTE: the functions below are not intended for use outside
 * of the TPS6586X sub-device drivers
 */
extern int tps6586x_write(struct device *dev, int reg, uint8_t val);
extern int tps6586x_writes(struct device *dev, int reg, int len, uint8_t *val);
extern int tps6586x_read(struct device *dev, int reg, uint8_t *val);
extern int tps6586x_reads(struct device *dev, int reg, int len, uint8_t *val);
extern int tps6586x_set_bits(struct device *dev, int reg, uint8_t bit_mask);
extern int tps6586x_clr_bits(struct device *dev, int reg, uint8_t bit_mask);
extern int tps6586x_update(struct device *dev, int reg, uint8_t val,
			   uint8_t mask);

#endif /*__LINUX_MFD_TPS6586X_H */

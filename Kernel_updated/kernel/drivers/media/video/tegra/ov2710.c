/*
 * ov2710.c - ov2710 sensor driver
 *
 * Copyright (c) 2011, NVIDIA, All Rights Reserved.
 *
 * Contributors:
 *      erik lilliebjerg <elilliebjerg@nvidia.com>
 *
 * Leverage OV5650.c
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <media/ov2710.h>

struct ov2710_reg {
	u16 addr;
	u16 val;
};

struct ov2710_info {
	int mode;
	struct i2c_client *i2c_client;
	struct ov2710_platform_data *pdata;
};

#define OV2710_TABLE_WAIT_MS 0
#define OV2710_TABLE_END 1
#define OV2710_MAX_RETRIES 3

static struct ov2710_reg mode_1920x1080[] = {
	{0x3103, 0x93},
	{0x3008, 0x82},
	{OV2710_TABLE_WAIT_MS, 5},
	{0x3008, 0x42},
	{OV2710_TABLE_WAIT_MS, 5},
	{0x3017, 0x7f},
	{0x3018, 0xfc},
	{0x3706, 0x61},
	{0x3712, 0x0c},
	{0x3630, 0x6d},
	{0x3801, 0xb4},

	{0x3621, 0x04},
	{0x3604, 0x60},
	{0x3603, 0xa7},
	{0x3631, 0x26},
	{0x3600, 0x04},
	{0x3620, 0x37},
	{0x3623, 0x00},
	{0x3702, 0x9e},
	{0x3703, 0x5c},
	{0x3704, 0x40},
	{0x370d, 0x0f},
	{0x3713, 0x9f},
	{0x3714, 0x4c},
	{0x3710, 0x9e},
	{0x3801, 0xc4},
	{0x3605, 0x05},
	{0x3606, 0x3f},
	{0x302d, 0x90},
	{0x370b, 0x40},
	{0x3716, 0x31},
	{0x3707, 0x52},
	{0x380d, 0x74},
	{0x5181, 0x20},
	{0x518f, 0x00},
	{0x4301, 0xff},
	{0x4303, 0x00},
	{0x3a00, 0x78},
	{0x300f, 0x88},
	{0x3011, 0x28},
	{0x3a1a, 0x06},
	{0x3a18, 0x00},
	{0x3a19, 0x7a},
	{0x3a13, 0x54},
	{0x382e, 0x0f},
	{0x381a, 0x1a},
	{0x401d, 0x02},

	{0x381c, 0x00},
	{0x381d, 0x02},
	{0x381e, 0x04},
	{0x381f, 0x38},
	{0x3820, 0x00},
	{0x3821, 0x98},
	{0x3800, 0x01},
	{0x3802, 0x00},
	{0x3803, 0x0a},
	{0x3804, 0x07},
	{0x3805, 0x90},
	{0x3806, 0x04},
	{0x3807, 0x40},
	{0x3808, 0x07},
	{0x3809, 0x90},
	{0x380a, 0x04},
	{0x380b, 0x40},
	{0x380e, 0x04},
	{0x380f, 0x50},
	{0x380c, 0x09},
	{0x380d, 0x74},
	{0x3810, 0x08},
	{0x3811, 0x02},

	{0x5688, 0x03},
	{0x5684, 0x07},
	{0x5685, 0xa0},
	{0x5686, 0x04},
	{0x5687, 0x43},
	{0x3011, 0x0a},
	{0x300f, 0x8a},
	{0x3017, 0x00},
	{0x3018, 0x00},
	{0x4800, 0x24},
	{0x300e, 0x04},
	{0x4801, 0x0f},

	{0x300f, 0xc3},
	{0x3010, 0x00},
	{0x3011, 0x0a},
	{0x3012, 0x01},

	{0x3a0f, 0x40},
	{0x3a10, 0x38},
	{0x3a1b, 0x48},
	{0x3a1e, 0x30},
	{0x3a11, 0x90},
	{0x3a1f, 0x10},

	{0x3a0e, 0x03},
	{0x3a0d, 0x04},
	{0x3a08, 0x14},
	{0x3a09, 0xc0},
	{0x3a0a, 0x11},
	{0x3a0b, 0x40},

	{0x300f, 0xc3},
	{0x3010, 0x00},
	{0x3011, 0x0e},
	{0x3012, 0x02},
	{0x380c, 0x09},
	{0x380d, 0xec},
	{0x3703, 0x61},
	{0x3704, 0x44},
	{0x3801, 0xd2},

	{0x3503, 0x17},
	{0x3500, 0x00},
	{0x3501, 0x00},
	{0x3502, 0x00},
	{0x350a, 0x00},
	{0x350b, 0x00},
	{0x5001, 0x4e},
	{0x5000, 0x5f},
	{0x3008, 0x02},

	{OV2710_TABLE_END, 0x0000}
};

static struct ov2710_reg mode_1280x720[] = {
	{0x3103, 0x93},
	{0x3008, 0x82},
	{OV2710_TABLE_WAIT_MS, 5},
	{0x3008, 0x42},
	{OV2710_TABLE_WAIT_MS, 5},
	{0x3017, 0x7f},
	{0x3018, 0xfc},

	{0x3706, 0x61},
	{0x3712, 0x0c},
	{0x3630, 0x6d},
	{0x3801, 0xb4},
	{0x3621, 0x04},
	{0x3604, 0x60},
	{0x3603, 0xa7},
	{0x3631, 0x26},
	{0x3600, 0x04},
	{0x3620, 0x37},
	{0x3623, 0x00},
	{0x3702, 0x9e},
	{0x3703, 0x5c},
	{0x3704, 0x40},
	{0x370d, 0x0f},
	{0x3713, 0x9f},
	{0x3714, 0x4c},
	{0x3710, 0x9e},
	{0x3801, 0xc4},
	{0x3605, 0x05},
	{0x3606, 0x3f},
	{0x302d, 0x90},
	{0x370b, 0x40},
	{0x3716, 0x31},
	{0x3707, 0x52},
	{0x380d, 0x74},
	{0x5181, 0x20},
	{0x518f, 0x00},
	{0x4301, 0xff},
	{0x4303, 0x00},
	{0x3a00, 0x78},
	{0x300f, 0x88},
	{0x3011, 0x28},
	{0x3a1a, 0x06},
	{0x3a18, 0x00},
	{0x3a19, 0x7a},
	{0x3a13, 0x54},
	{0x382e, 0x0f},
	{0x381a, 0x1a},
	{0x401d, 0x02},

	{0x381c, 0x10},
	{0x381d, 0xb0},
	{0x381e, 0x02},
	{0x381f, 0xec},
	{0x3800, 0x01},
	{0x3820, 0x0a},
	{0x3821, 0x2a},
	{0x3804, 0x05},
	{0x3805, 0x10},
	{0x3802, 0x00},
	{0x3803, 0x04},
	{0x3806, 0x02},
	{0x3807, 0xe0},
	{0x3808, 0x05},
	{0x3809, 0x10},
	{0x380a, 0x02},
	{0x380b, 0xe0},
	{0x380e, 0x02},
	{0x380f, 0xf0},
	{0x380c, 0x07},
	{0x380d, 0x00},
	{0x3810, 0x10},
	{0x3811, 0x06},

	{0x5688, 0x03},
	{0x5684, 0x05},
	{0x5685, 0x00},
	{0x5686, 0x02},
	{0x5687, 0xd0},

	{0x3a08, 0x1b},
	{0x3a09, 0xe6},
	{0x3a0a, 0x17},
	{0x3a0b, 0x40},
	{0x3a0e, 0x01},
	{0x3a0d, 0x02},
	{0x3011, 0x0a},
	{0x300f, 0x8a},
	{0x3017, 0x00},
	{0x3018, 0x00},
	{0x4800, 0x24},
	{0x300e, 0x04},
	{0x4801, 0x0f},
	{0x300f, 0xc3},
	{0x3a0f, 0x40},
	{0x3a10, 0x38},
	{0x3a1b, 0x48},
	{0x3a1e, 0x30},
	{0x3a11, 0x90},
	{0x3a1f, 0x10},

	{0x3010, 0x10},
	{0x3a0e, 0x02},
	{0x3a0d, 0x03},
	{0x3a08, 0x0d},
	{0x3a09, 0xf3},
	{0x3a0a, 0x0b},
	{0x3a0b, 0xa0},

	{0x300f, 0xc3},
	{0x3011, 0x0e},
	{0x3012, 0x02},
	{0x380c, 0x07},
	{0x380d, 0x6a},
	{0x3703, 0x5c},
	{0x3704, 0x40},
	{0x3801, 0xbc},

	{0x3503, 0x17},
	{0x3500, 0x00},
	{0x3501, 0x00},
	{0x3502, 0x00},
	{0x350a, 0x00},
	{0x350b, 0x00},
	{0x5001, 0x4e},
	{0x5000, 0x5f},
	{0x3008, 0x02},

	{OV2710_TABLE_END, 0x0000}
};

enum {
	OV2710_MODE_1920x1080,
	OV2710_MODE_1280x720,
};


static struct ov2710_reg *mode_table[] = {
	[OV2710_MODE_1920x1080] = mode_1920x1080,
	[OV2710_MODE_1280x720] = mode_1280x720,
};

static inline void ov2710_get_frame_length_regs(struct ov2710_reg *regs,
						u32 frame_length)
{
	regs->addr = 0x380e;
	regs->val = (frame_length >> 8) & 0xff;
	(regs + 1)->addr = 0x380f;
	(regs + 1)->val = (frame_length) & 0xff;
}

static inline void ov2710_get_coarse_time_regs(struct ov2710_reg *regs,
					       u32 coarse_time)
{
	regs->addr = 0x3500;
	regs->val = (coarse_time >> 12) & 0xff;
	(regs + 1)->addr = 0x3501;
	(regs + 1)->val = (coarse_time >> 4) & 0xff;
	(regs + 2)->addr = 0x3502;
	(regs + 2)->val = (coarse_time & 0xf) << 4;
}

static inline void ov2710_get_gain_reg(struct ov2710_reg *regs, u16 gain)
{
	regs->addr = 0x350b;
	regs->val = gain;
}

static int ov2710_read_reg(struct i2c_client *client, u16 addr, u8 *val)
{
	int err;
	struct i2c_msg msg[2];
	unsigned char data[3];

	if (!client->adapter)
		return -ENODEV;

	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].len = 2;
	msg[0].buf = data;

	/* high byte goes out first */
	data[0] = (u8) (addr >> 8);;
	data[1] = (u8) (addr & 0xff);

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = 1;
	msg[1].buf = data + 2;

	err = i2c_transfer(client->adapter, msg, 2);

	if (err != 2)

		return -EINVAL;

	*val = data[2];

	return 0;
}

static int ov2710_write_reg(struct i2c_client *client, u16 addr, u8 val)
{
	int err;
	struct i2c_msg msg;
	unsigned char data[3];
	int retry = 0;

	if (!client->adapter)
		return -ENODEV;

	data[0] = (u8) (addr >> 8);;
	data[1] = (u8) (addr & 0xff);
	data[2] = (u8) (val & 0xff);

	msg.addr = client->addr;
	msg.flags = 0;
	msg.len = 3;
	msg.buf = data;

	do {
		err = i2c_transfer(client->adapter, &msg, 1);
		if (err == 1)
			return 0;
		retry++;
		pr_err("ov2710: i2c transfer failed, retrying %x %x\n",
		       addr, val);

		msleep(3);
	} while (retry <= OV2710_MAX_RETRIES);

	return err;
}

static int ov2710_write_table(struct i2c_client *client,
			      const struct ov2710_reg table[],
			      const struct ov2710_reg override_list[],
			      int num_override_regs)
{
	int err;
	const struct ov2710_reg *next;
	int i;
	u16 val;

	for (next = table; next->addr != OV2710_TABLE_END; next++) {
		if (next->addr == OV2710_TABLE_WAIT_MS) {
			msleep(next->val);
			continue;
		}


		val = next->val;

		/* When an override list is passed in, replace the reg */
		/* value to write if the reg is in the list            */
		if (override_list) {
			for (i = 0; i < num_override_regs; i++) {
				if (next->addr == override_list[i].addr) {
					val = override_list[i].val;
					break;
				}
			}
		}

		err = ov2710_write_reg(client, next->addr, val);
		if (err)
			return err;
	}
	return 0;
}

static int ov2710_set_mode(struct ov2710_info *info, struct ov2710_mode *mode)
{
	int sensor_mode;
	int err;
	struct ov2710_reg reg_list[6];

	pr_info("%s: xres %u yres %u framelength %u coarsetime %u gain %u\n",
		__func__, mode->xres, mode->yres, mode->frame_length,
		mode->coarse_time, mode->gain);

	if (mode->xres == 1920 && mode->yres == 1080)
		sensor_mode = OV2710_MODE_1920x1080;
	else if (mode->xres == 1280 && mode->yres == 720)
		sensor_mode = OV2710_MODE_1280x720;
	else {
		pr_err("%s: invalid resolution supplied to set mode %d %d\n",
		       __func__, mode->xres, mode->yres);
		return -EINVAL;
	}

	/* get a list of override regs for the asking frame length, */
	/* coarse integration time, and gain.                       */
	ov2710_get_frame_length_regs(reg_list, mode->frame_length);
	ov2710_get_coarse_time_regs(reg_list + 2, mode->coarse_time);
	ov2710_get_gain_reg(reg_list + 5, mode->gain);

	err = ov2710_write_table(info->i2c_client, mode_table[sensor_mode],
	reg_list, 6);
	if (err)
		return err;

	info->mode = sensor_mode;
	return 0;
}

static int ov2710_set_frame_length(struct ov2710_info *info, u32 frame_length)
{
	struct ov2710_reg reg_list[2];
	int i = 0;
	int ret;

	ov2710_get_frame_length_regs(reg_list, frame_length);

	for (i = 0; i < 2; i++)	{
		ret = ov2710_write_reg(info->i2c_client, reg_list[i].addr,
			reg_list[i].val);
		if (ret)
			return ret;
	}

	return 0;
}

static int ov2710_set_coarse_time(struct ov2710_info *info, u32 coarse_time)
{
	int ret;

	struct ov2710_reg reg_list[3];
	int i = 0;

	ov2710_get_coarse_time_regs(reg_list, coarse_time);

	ret = ov2710_write_reg(info->i2c_client, 0x3212, 0x01);
	if (ret)
		return ret;

	for (i = 0; i < 3; i++)	{
		ret = ov2710_write_reg(info->i2c_client, reg_list[i].addr,
			reg_list[i].val);
		if (ret)
			return ret;
	}

	ret = ov2710_write_reg(info->i2c_client, 0x3212, 0x11);
	if (ret)
		return ret;

	ret = ov2710_write_reg(info->i2c_client, 0x3212, 0xa1);
	if (ret)
		return ret;

	return 0;
}

static int ov2710_set_gain(struct ov2710_info *info, u16 gain)
{
	int ret;
	struct ov2710_reg reg_list;

	ov2710_get_gain_reg(&reg_list, gain);

	ret = ov2710_write_reg(info->i2c_client, reg_list.addr, reg_list.val);

	return ret;
}

static int ov2710_get_status(struct ov2710_info *info, u8 *status)
{
	int err;

	*status = 0;
	err = ov2710_read_reg(info->i2c_client, 0x002, status);
	return err;
}


static long ov2710_ioctl(struct file *file,
			 unsigned int cmd, unsigned long arg)
{
	int err;
	struct ov2710_info *info = file->private_data;

	switch (cmd) {
	case OV2710_IOCTL_SET_MODE:
	{
		struct ov2710_mode mode;
		if (copy_from_user(&mode,
				   (const void __user *)arg,
				   sizeof(struct ov2710_mode))) {
			return -EFAULT;
		}

		return ov2710_set_mode(info, &mode);
	}
	case OV2710_IOCTL_SET_FRAME_LENGTH:
		return ov2710_set_frame_length(info, (u32)arg);
	case OV2710_IOCTL_SET_COARSE_TIME:
		return ov2710_set_coarse_time(info, (u32)arg);
	case OV2710_IOCTL_SET_GAIN:
		return ov2710_set_gain(info, (u16)arg);
	case OV2710_IOCTL_GET_STATUS:
	{
		u8 status;

		err = ov2710_get_status(info, &status);
		if (err)
			return err;
		if (copy_to_user((void __user *)arg, &status,
				 2)) {
			return -EFAULT;
		}
		return 0;
	}
	default:
		return -EINVAL;
	}
	return 0;
}

static struct ov2710_info *info;

static int ov2710_open(struct inode *inode, struct file *file)
{
	u8 status;

	file->private_data = info;
	if (info->pdata && info->pdata->power_on)
		info->pdata->power_on();
	ov2710_get_status(info, &status);
	return 0;
}

int ov2710_release(struct inode *inode, struct file *file)
{
	if (info->pdata && info->pdata->power_off)
		info->pdata->power_off();
	file->private_data = NULL;
	return 0;
}


static const struct file_operations ov2710_fileops = {
	.owner = THIS_MODULE,
	.open = ov2710_open,
	.unlocked_ioctl = ov2710_ioctl,
	.release = ov2710_release,
};

static struct miscdevice ov2710_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ov2710",
	.fops = &ov2710_fileops,
};

static int ov2710_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int err;

	pr_info("ov2710: probing sensor.\n");

	info = kzalloc(sizeof(struct ov2710_info), GFP_KERNEL);
	if (!info) {
		pr_err("ov2710: Unable to allocate memory!\n");
		return -ENOMEM;
	}

	err = misc_register(&ov2710_device);
	if (err) {
		pr_err("ov2710: Unable to register misc device!\n");
		kfree(info);
		return err;
	}

	info->pdata = client->dev.platform_data;
	info->i2c_client = client;

	i2c_set_clientdata(client, info);
	return 0;
}

static int ov2710_remove(struct i2c_client *client)
{
	struct ov2710_info *info;
	info = i2c_get_clientdata(client);
	misc_deregister(&ov2710_device);
	kfree(info);
	return 0;
}

static const struct i2c_device_id ov2710_id[] = {
	{ "ov2710", 0 },
	{ },
};

MODULE_DEVICE_TABLE(i2c, ov2710_id);

static struct i2c_driver ov2710_i2c_driver = {
	.driver = {
		.name = "ov2710",
		.owner = THIS_MODULE,
	},
	.probe = ov2710_probe,
	.remove = ov2710_remove,
	.id_table = ov2710_id,
};

static int __init ov2710_init(void)
{
	pr_info("ov2710 sensor driver loading\n");
	return i2c_add_driver(&ov2710_i2c_driver);
}

static void __exit ov2710_exit(void)
{
	i2c_del_driver(&ov2710_i2c_driver);
}

module_init(ov2710_init);
module_exit(ov2710_exit);


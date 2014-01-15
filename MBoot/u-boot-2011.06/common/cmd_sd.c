/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <mmc.h>

static int curr_device = -1;
#ifndef CONFIG_GENERIC_MMC
int do_mmc (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int dev;

	if (argc < 2)
		return cmd_usage(cmdtp);

	if (strcmp(argv[1], "init") == 0) {
		if (argc == 2) {
			if (curr_device < 0)
				dev = 1;
			else
				dev = curr_device;
		} else if (argc == 3) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);
		} else {
			return cmd_usage(cmdtp);
		}

		if (mmc_legacy_init(dev) != 0) {
			puts("No MMC card found\n");
			return 1;
		}

		curr_device = dev;
		printf("mmc%d is available\n", curr_device);
	} else if (strcmp(argv[1], "device") == 0) {
		if (argc == 2) {
			if (curr_device < 0) {
				puts("No MMC device available\n");
				return 1;
			}
		} else if (argc == 3) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);

#ifdef CONFIG_SYS_MMC_SET_DEV
			if (mmc_set_dev(dev) != 0)
				return 1;
#endif
			curr_device = dev;
		} else {
			return cmd_usage(cmdtp);
		}

		printf("mmc%d is current device\n", curr_device);
	} else {
		return cmd_usage(cmdtp);
	}

	return 0;
}

U_BOOT_CMD(
	mmc, 3, 1, do_mmc,
	"MMC sub-system",
	"init [dev] - init MMC sub system\n"
	"mmc device [dev] - show or set current device"
);
#else /* !CONFIG_GENERIC_MMC */

static void print_mmcinfo(struct mmc *mmc)
{
	printf("Device: %s\n", mmc->name);
	printf("Manufacturer ID: %x\n", mmc->cid[0] >> 24);
	printf("OEM: %x\n", (mmc->cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);

	printf("Tran Speed: %d\n", mmc->tran_speed);
	printf("Rd Block Len: %d\n", mmc->read_bl_len);

	printf("%s version %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
			(mmc->version >> 4) & 0xf, mmc->version & 0xf);

	printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	puts("Capacity: ");
	print_size(mmc->capacity, "\n");

	printf("Bus Width: %d-bit\n", mmc->bus_width);
}

int do_mmcinfo (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;

	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}

	mmc = find_mmc_device(curr_device);

	if (mmc) {
		mmc_init(mmc);

		print_mmcinfo(mmc);
		return 0;
	} else {
		printf("no mmc device at slot %x\n", curr_device);
		return 1;
	}
}

U_BOOT_CMD(
	sdinfo, 1, 0, do_mmcinfo,
	"display SD info",
	"    - device number of the device to dislay info of\n"
	""
);

int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 2)
		return cmd_usage(cmdtp);

	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}

	if (strcmp(argv[1], "rescan") == 0) {
		struct mmc *mmc = find_mmc_device(curr_device);

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		mmc->has_init = 0;
		mmc_init(mmc);

		return 0;
	}else if (strcmp(argv[1], "burning") == 0) {
		struct mmc *mmc = find_mmc_device(curr_device);
        u32 n, i, j;
		u32 blknr = 100;
		static u8 test_pattern[512];
		static u8 read_pattern[512];
		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		mmc->has_init = 0;
		mmc_init(mmc);        

        for(i=0;i<100;i++)
        {
           for(j=0;j<512;j++)
		       test_pattern[j]=j+i; 
		   n = mmc->block_dev.block_write(curr_device, blknr, 1, test_pattern);
		   n = mmc->block_dev.block_read(curr_device, blknr, 1, read_pattern);
		   for(j=0;j<512;j++)
		   {
		       if (read_pattern[j] != test_pattern[j])	
			   {
			       printf("[%d]r:[0x%x]!=w:[0x%x]\n", j,read_pattern[j],test_pattern[j]);
				   while(1);
		       }
		   }
        }
		return 0;	
	} else if (strncmp(argv[1], "part", 4) == 0) {
		block_dev_desc_t *mmc_dev;
		struct mmc *mmc = find_mmc_device(curr_device);

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}
		mmc_init(mmc);
		mmc_dev = mmc_get_dev(curr_device);
		if (mmc_dev != NULL &&
				mmc_dev->type != DEV_TYPE_UNKNOWN) {
			print_part(mmc_dev);
			return 0;
		}

		puts("get mmc type error!\n");
		return 1;
	} else if (strcmp(argv[1], "list") == 0) {
		print_mmc_devices('\n');
		return 0;
	} else if (strcmp(argv[1], "dev") == 0) {
		int dev, part = -1;
		struct mmc *mmc;

		if (argc == 2)
			dev = curr_device;
		else if (argc == 3)
			dev = simple_strtoul(argv[2], NULL, 10);
		else if (argc == 4) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);
			part = (int)simple_strtoul(argv[3], NULL, 10);
			if (part > PART_ACCESS_MASK) {
				printf("#part_num shouldn't be larger"
					" than %d\n", PART_ACCESS_MASK);
				return 1;
			}
		} else
			return cmd_usage(cmdtp);

		mmc = find_mmc_device(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		mmc_init(mmc);
		if (part != -1) {
			int ret;
			if (mmc->part_config == MMCPART_NOAVAILABLE) {
				printf("Card doesn't support part_switch\n");
				return 1;
			}

			if (part != mmc->part_num) {
				ret = mmc_switch_part(dev, part);
				if (!ret)
					mmc->part_num = part;

				printf("switch to partions #%d, %s\n",
						part, (!ret) ? "OK" : "ERROR");
			}
		}
		curr_device = dev;
		if (mmc->part_config == MMCPART_NOAVAILABLE)
			printf("mmc%d is current device\n", curr_device);
		else
			printf("mmc%d(part %d) is current device\n",
				curr_device, mmc->part_num);

		return 0;
	} else if (strcmp(argv[1], "read") == 0) {
		void *addr = (void *)simple_strtoul(argv[2], NULL, 16);
		u32 cnt = simple_strtoul(argv[4], NULL, 16);
		u32 n;
		u32 blk = simple_strtoul(argv[3], NULL, 16);
		struct mmc *mmc = find_mmc_device(curr_device);

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		printf("\nMMC read: dev # %d, block # %d, count %d ... ",
				curr_device, blk, cnt);

		mmc_init(mmc);

		n = mmc->block_dev.block_read(curr_device, blk, cnt, addr);

		/* flush cache after read */
		flush_cache((ulong)addr, cnt * 512); /* FIXME */

		printf("%d blocks read: %s\n",
				n, (n==cnt) ? "OK" : "ERROR");
		return (n == cnt) ? 0 : 1;
	} else if (strcmp(argv[1], "write") == 0) {
		void *addr = (void *)simple_strtoul(argv[2], NULL, 16);
		u32 cnt = simple_strtoul(argv[4], NULL, 16);
		u32 n;
		struct mmc *mmc = find_mmc_device(curr_device);

		int blk = simple_strtoul(argv[3], NULL, 16);

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		printf("\nMMC write: dev # %d, block # %d, count %d ... ",
				curr_device, blk, cnt);

		mmc_init(mmc);

		n = mmc->block_dev.block_write(curr_device, blk, cnt, addr);

		printf("%d blocks written: %s\n",
				n, (n == cnt) ? "OK" : "ERROR");
		return (n == cnt) ? 0 : 1;
	}

	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	sd, 6, 1, do_mmcops,
	"SD Card sub system",
	"read addr blk# cnt\n"
	"sd write addr blk# cnt\n"
	"sd rescan\n"
	"sd burning\n"
	"sd part - lists available partition on current mmc device\n"
	"sd dev [dev] [part] - show or set current mmc device [partition]\n"
	"sd list - lists available devices");
#endif

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

#include <command.h>
#include <MsMmc.h>

#ifndef CONFIG_GENERIC_MMC
U_BOOT_CMD(
	mmc, 3, 1, do_mmc,
	"MMC sub-system",
	"init [dev] - init MMC sub system\n"
	"mmc device [dev] - show or set current device"
);
#else /* !CONFIG_GENERIC_MMC */
U_BOOT_CMD(
	mmcinfo, 1, 0, do_mmcinfo,
	"display MMC info",
	"    - device number of the device to dislay info of\n"
	""
);

U_BOOT_CMD(
	mmcreg, 4, 0, do_mmcreg,
	"mmcreg show ext-csd",
	"<dev num>\n"
	"    - device number of the device to dislay info of\n"
	""
);

#if defined(CONFIG_USB) && defined(CONFIG_FAT)
U_BOOT_CMD(
  bin2emmc, 4, 1, do_emmc_bin_restore,
  "bin2emmc - read bin file and restore it to emmc\n",
  "command: bin2emmc [usbportnum] [binname] [offset/partitionname]\n"
);

U_BOOT_CMD(
  emmcbin, 5, 1, do_emmc_mkbin,
  "emmcbin - dump emmc and restore it to fat usb disk\n",
  "command: emmcbin [usbportnum] [binname] [offset/partitionname] [dumpsize]\n"
);
#endif

U_BOOT_CMD(
        mmc, 7, 1, do_mmcops,
        "MMC sub system",
        "read[.boot] bootpart addr blk# size\n"
        "mmc write[.boot] bootpart addr blk# size [empty_skip:0-disable,1-enable]\n"
        "mmc readall - read all blocks for device internal ecc check\n"
        "mmc crcall - read all blocks and calculate crc32\n"
        "mmc read.p addr partition_name size\n"
        "mmc read.p.continue addr partition_name offset size\n"
        "mmc write.p addr partition_name size [empty_skip:0-disable,1-enable]\n"
        "mmc write.p.continue addr partition_name offset size [empty_skip:0-disable,1-enable]\n"
        "mmc rescan\n"
        "mmc part - lists available partition on current mmc device\n"
        "mmc look [name] - lists specific partition info on mmc\n"
        "mmc dev [dev] [part] - show or set current mmc device [partition]\n"
        "mmc list - lists available devices\n"
        "mmc create [name] [size]- create/change mmc partition [name]\n"
        "mmc remove [name] - remove mmc partition [name]\n"
        "mmc rmgpt - clean all mmc partition table\n"
        "mmc slc size relwr - set slc in the front of user area,  0xffffffff means max slc size\n"
        "mmc ecsd - print ecsd register of emmc\n"
        "mmc setecsd num mask value - set value to num of ecsd according to mask\n"
        "mmc size - print the mmc size info\n"
        "mmc slcchk - check the slc/mlc mode of emmc\n"
        "mmc relwrchk - check the reliable write configuration of emmc\n"
        "mmc slcrelwrchk - check the slc/mlc mode and reliable write configuration of emmc\n"
        "mmc unlzo Src_Address Src_Length Partition_Name [empty_skip:0-disable,1-enable]- decompress lzo file and write to mmc partition \n"
        "mmc erase[.boot] bootpart [blk#] [size]\n"
        "mmc erase.p partition_name\n"
        "mmc erase - erase all blocks in device\n"
        "mmc dd mmc2usb/usb2mmc [pornum] - dump/restore emmc raw data\n"
        "mmc alignsize - check the alignment size of slc partition\n");

U_BOOT_CMD(
	emmc, 5, 1, do_eMMCops,
	"eMMC sub system",
	"emmc info - lists CSD & ExtCSD on eMMC\n"
	"emmc init count - reset & init eMMC for count loops\n"
	"emmc test count - verify eMMC & board signals for count loops\n"
	"emmc speed - show eMMC speed @ driver layer\n"
	"emmc table - build or dump\n"
	"emmc mode - ddr or sdr \n"
	"emmc clk - set ClkRegVal \n"
	"emmc cis - check or erase \n"
	"emmc pwr_cut init [addr][start block] - eMMC Power Cut Init\n"
	"emmc pwr_cut test [addr][start block] - eMMC Power Cut Test\n"
);
#endif

//*********************************************************************
//
//   MODULE NAME:
//       mtd_serflash.c
//
//   DESCRIPTION:
//      This file is an agent between mtd layer and spi flash driver
//
//  PUBLIC PROCEDURES:
//       Name                    Title
//       ----------------------- --------------------------------------
//       int xxx_proc           declare in its corresponding header file
//
//   LOCAL PROCEDURES:
//       Name                    Title
//       ----------------------- --------------------------------------
//       get_prnt_cnvs           the local procedure in the file
//
//  Written by Tao.Zhou@MSTAR Inc.
//---------------------------------------------------------------------
//
//********************************************************************
//--------------------------------------------------------------------
//                             GENERAL INCLUDE
//--------------------------------------------------------------------

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include "drvSERFLASH.h"

/* Flash opcodes. */
#define	OPCODE_BE_4K		0x20	/* Erase 4KiB block */
#define	OPCODE_BE_32K		0x52	/* Erase 32KiB block */
//#define	OPCODE_CHIP_ERASE	0xc7	/* Erase whole flash chip */
#define	OPCODE_SE		0xd8	/* Sector erase (usually 64KiB) */
#define	OPCODE_RDID		0x9f	/* Read JEDEC ID */

/* Define max times to check status register before we give up. */
#define	MAX_READY_WAIT_COUNT	100000
#define	CMD_SIZE		4

#ifdef CONFIG_MTD_PARTITIONS
#define	mtd_has_partitions()	(1)
#else
#define	mtd_has_partitions()	(0)
#endif

/****************************************************************************/

struct serflash {
	struct mutex		lock;
	struct mtd_info		mtd;
	unsigned		partitioned:1;
	u8			erase_opcode;
};

static inline struct serflash *mtd_to_serflash(struct mtd_info *mtd)
{
	return container_of(mtd, struct serflash, mtd);
}

/* Erase flash fully or part of it */
static int serflash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct serflash *flash = mtd_to_serflash(mtd);
	//ulong addr,len,start_sec,end_sec;
        ulong addr,len;
	DEBUG(MTD_DEBUG_LEVEL2, "%s: addr 0x%08x, len %d\n",
			__func__, (u32)instr->addr, instr->len);

	/* range and alignment check */
	if (instr->addr + instr->len > mtd->size)
		return -EINVAL;
	if ((instr->addr % mtd->erasesize) != 0
			|| (instr->len % mtd->erasesize) != 0) {
		return -EINVAL;
	}

	addr = instr->addr;
	len = instr->len;
	mutex_lock(&flash->lock);
    /*write protect false before erase*/
	if (!MDrv_SERFLASH_WriteProtect(0))
	{
	    mutex_unlock(&flash->lock);
	    return -EIO;
	}
	/* erase the whole chip */
	if (len == mtd->size && !MDrv_SERFLASH_EraseChip()) {
		instr->state = MTD_ERASE_FAILED;

        MDrv_SERFLASH_WriteProtect(1);
		mutex_unlock(&flash->lock);
		return -EIO;
	} 
       else {
		if (len) {
			if (!MDrv_SERFLASH_AddressErase(addr, len, 1)) {
				instr->state = MTD_ERASE_FAILED;

                MDrv_SERFLASH_WriteProtect(1);
				mutex_unlock(&flash->lock);
				return -EIO;
			}
		}
	}

    MDrv_SERFLASH_WriteProtect(1);
	mutex_unlock(&flash->lock);

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return 0;
}

/*
 * Read an address range from the flash chip.  The address range
 * may be any size provided it is within the physical boundaries.
 */
static int serflash_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	struct serflash *flash = mtd_to_serflash(mtd);

	DEBUG(MTD_DEBUG_LEVEL2, "%s %s 0x%08x, len %zd\n",
			__func__, "from", (u32)from, len);

	/* sanity checks */
	if (!len)
		return 0;

	if (from + len > flash->mtd.size)
		return -EINVAL;

	mutex_lock(&flash->lock);

    #if 0
	/* Wait till previous write/erase is done. */
	if (wait_till_ready(flash)) {
		/* REVISIT status return?? */
		mutex_unlock(&flash->lock);
		return 1;
	}
    #endif
    
       if (MDrv_SERFLASH_Read(from, len, (unsigned char *)buf))
       {
           *retlen = len;
       }
       else
       {
           *retlen = 0;
           mutex_unlock(&flash->lock);
           return -EIO;
       }

	mutex_unlock(&flash->lock);

	return 0;
}

/*
 * Write an address range to the flash chip.  Data must be written in
 * FLASH_PAGESIZE chunks.  The address range may be any size provided
 * it is within the physical boundaries.
 */
static int serflash_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	struct serflash *flash = mtd_to_serflash(mtd);
       u32 erase_start, erase_end;

	DEBUG(MTD_DEBUG_LEVEL2, "%s %s 0x%08x, len %zd\n",
			__func__, "to", (u32)to, len);

	if (retlen)
		*retlen = 0;

	/* sanity checks */
	if (!len)
		return(0);

	if (to + len > flash->mtd.size)
		return -EINVAL;

       // calculate erase sectors
       erase_start = (u32)to / mtd->erasesize;
       erase_end = (u32)(to+len) / mtd->erasesize - 1;

	mutex_lock(&flash->lock);

    #if 0
	/* Wait until finished previous write command. */
	if (wait_till_ready(flash)) {
		mutex_unlock(&flash->lock);
		return 1;
	}
    #endif
    
	if (!MDrv_SERFLASH_WriteProtect(0))
	{
	    mutex_unlock(&flash->lock);
	    return -EIO;
	}
//modified by daniel.lee 2010/0514
/*    
       if (!MDrv_SERFLASH_BlockErase(erase_start, erase_end, TRUE))
       {
           mutex_unlock(&flash->lock);
	    return -EIO;
       }
*/
       if (MDrv_SERFLASH_Write(to, len, (unsigned char *)buf))
       {
           if (retlen)
               *retlen = len;
       }
       else
       {
           if (retlen)
               *retlen = 0;

           MDrv_SERFLASH_WriteProtect(1);
           mutex_unlock(&flash->lock);
           return -EIO;
       }

       MDrv_SERFLASH_WriteProtect(1);
	mutex_unlock(&flash->lock);

	return 0;
}

/* SPI device information structure */
struct flash_info {
    char		*name;

    u32		jedec_id; //no ID or manufacturer id & two byte device id
    u16            ext_id;

    unsigned	sector_size;
    u16		n_sectors;

    u16		flags;
  #define	SECT_4K		0x01
};

/* chip list */
static struct flash_info __devinitdata serflash_data [] = {

    { "MX25L3205D",     0xc22016,   0,  64 * 1024,  64, },
    { "MX25L6405D",     0xc22017,   0,  64 * 1024,  128,},
    { "MX25L12805D",    0xc22018,   0,  64 * 1024,  256,},
    { "W25X32",         0xef3016,   0,  64 * 1024,  64, },
    { "W25Q32",         0xef4016,   0,  64 * 1024,  64, },
    { "W25X64",         0xef3017,   0,  64 * 1024,  128,},
    { "W25Q64",         0xef4017,   0,  64 * 1024,  128,},
    { "W25Q128",        0xef4018,   0,  64 * 1024,  256,},
    { "AT26DF321",      0x1f4700,   0,  64 * 1024,  64, },
    { "STM25P32",       0x202016,   0,  64 * 1024,  64, },
    { "EN25B32B",       0x1c2016,   0,  64 * 1024,  68, },
    { "EN25B64B",       0x1c2017,   0,  64 * 1024,  132,},
    { "S25FL128P",      0x012018,   0,  64 * 1024,  256,},
};

#define MSTAR_SERFLASH_SIZE					(8 * 1024 * 1024)

#define SERFLASH_PART_PARTITION_TBL_OFFSET		0
#define SERFLASH_PART_PARTITION_TBL_SIZE		512 * 1024

#define SERFLASH_PART_LINUX_BOOT_PARAM_OFFSET	(SERFLASH_PART_PARTITION_TBL_OFFSET + SERFLASH_PART_PARTITION_TBL_SIZE)
#define SERFLASH_PART_LINUX_BOOT_PARAM_SIZE		512 * 1024

#define SERFLASH_PART_KERNEL_OFFSET				(SERFLASH_PART_LINUX_BOOT_PARAM_OFFSET + SERFLASH_PART_LINUX_BOOT_PARAM_SIZE)
#define SERFLASH_PART_KERNEL_SIZE				1536 * 1024

#define SERFLASH_PART_ROOTFS_OFFSET				(SERFLASH_PART_KERNEL_OFFSET + SERFLASH_PART_KERNEL_SIZE)
#define SERFLASH_PART_ROOTFS_SIZE				2560 * 1024

#define SERFLASH_PART_CONF_OFFSET				(SERFLASH_PART_ROOTFS_OFFSET + SERFLASH_PART_ROOTFS_SIZE)
#define SERFLASH_PART_CONF_SIZE					64 * 1024

#if 0
#define SERFLASH_PART_CHAKRA_BOOT_PARAM_OFFSET	(NAND_PART_KERNEL_OFFSET + NAND_PART_KERNEL_SIZE)
#define SERFLASH_PART_CHAKRA_BOOT_PARAM_SIZE	SZ_512KB

#define SERFLASH_PART_CHAKRA_BIN_OFFSET			(NAND_PART_CHAKRA_BOOT_PARAM_OFFSET + NAND_PART_CHAKRA_BOOT_PARAM_SIZE)
#define SERFLASH_PART_CHAKRA_BIN_PARAM_SIZE		SZ_8MB

#define SERFLASH_PART_SUBSYSTEM_OFFSET			(NAND_PART_CONF_OFFSET + NAND_PART_CONF_SIZE)
#define SERFLASH_PART_SUBSYSTEM_SIZE			SZ_2MB

#define SERFLASH_PART_FONT_OFFSET				(NAND_PART_SUBSYSTEM_OFFSET + NAND_PART_SUBSYSTEM_SIZE)
#define SERFLASH_PART_FONT_SIZE					SZ_4MB

#define SERFLASH_PART_OPT_OFFSET				(NAND_PART_FONT_OFFSET + NAND_PART_FONT_SIZE)
#define SERFLASH_PART_OPT_SIZE					SZ_8MB

#define SERFLASH_PART_APPLICATION_OFFSET		(NAND_PART_OPT_OFFSET + NAND_PART_OPT_SIZE)
#define SERFLASH_PART_APPLICATION_SIZE			(MSTAR_NAND_SIZE - NAND_PART_APPLICATION_OFFSET)
#endif

#if ( (NAND_PART_APPLICATION_OFFSET) >= MSTAR_SERFLASH_SIZE)
    #error "Error: NAND partition is not correct!!!"
#endif

static const struct mtd_partition serflash_partition_info[] =
{
    {
    	.name   = "nand_partition_tbl",
    	.offset = SERFLASH_PART_PARTITION_TBL_OFFSET,
        .size   = SERFLASH_PART_PARTITION_TBL_SIZE
    },
    {
        .name   = "linux_boot_param",
        .offset = SERFLASH_PART_LINUX_BOOT_PARAM_OFFSET,
        .size   = SERFLASH_PART_LINUX_BOOT_PARAM_SIZE
    },
    {
        .name   = "kernel",
        .offset = SERFLASH_PART_KERNEL_OFFSET,
        .size   = SERFLASH_PART_KERNEL_SIZE
    },
    {
        .name   = "rootfs",
        .offset = SERFLASH_PART_ROOTFS_OFFSET,
        .size   = SERFLASH_PART_ROOTFS_SIZE
    },
    {
	.name   = "conf",
	.offset = SERFLASH_PART_CONF_OFFSET,
	.size   = SERFLASH_PART_CONF_SIZE
    },
#if 0
    {
        .name   = "chakra_boot_param",
        .offset = NAND_PART_CHAKRA_BOOT_PARAM_OFFSET,
        .size   = NAND_PART_CHAKRA_BOOT_PARAM_SIZE
    },
    {
        .name   = "charkra_bin",
        .offset = NAND_PART_CHAKRA_BIN_OFFSET,
        .size   = NAND_PART_CHAKRA_BIN_PARAM_SIZE
    },
    {
		.name   = "subsystem",
		.offset = NAND_PART_SUBSYSTEM_OFFSET,
		.size   = NAND_PART_SUBSYSTEM_SIZE
    },
    {
		.name   = "font",
		.offset = NAND_PART_FONT_OFFSET,
		.size   = NAND_PART_FONT_SIZE
    },
    {
		.name   = "opt",
		.offset = NAND_PART_OPT_OFFSET,
		.size   = NAND_PART_OPT_SIZE
    },
    {
		.name   = "application",
		.offset = NAND_PART_APPLICATION_OFFSET,
		.size   = NAND_PART_APPLICATION_SIZE
    },
#endif
};

#define SERFLASH_NUM_PARTITIONS ARRAY_SIZE(serflash_partition_info)

static struct flash_info *__devinit jedec_probe(void)
{
	int			tmp;
	u8			id[5];
	u32			jedec;
	u16                     ext_jedec;
	struct flash_info	*info;

	/* JEDEC also defines an optional "extended device information"
	 * string for after vendor-specific data, after the three bytes
	 * we use here.  Supporting some chips might require using it.
	 */
       tmp = MDrv_SERFLASH_ReadID(id, 5);
	if (!tmp) {
		DEBUG(MTD_DEBUG_LEVEL0, "error %d reading JEDEC ID\n", tmp);
		return NULL;
	}
	jedec = id[0];
	jedec = jedec << 8;
	jedec |= id[1];
	jedec = jedec << 8;
	jedec |= id[2];

	ext_jedec = id[3] << 8 | id[4];

	for (tmp = 0, info = serflash_data;
			tmp < ARRAY_SIZE(serflash_data);
			tmp++, info++) {
		if (info->jedec_id == jedec) {
			if (info->ext_id != 0 && info->ext_id != ext_jedec)
				continue;
			return info;
		}
	}
	//dev_err(&spi->dev, "unrecognized JEDEC id %06x\n", jedec);
	return NULL;
}


/*
 * board specific setup should have ensured the SPI clock used here
 * matches what the READ command supports, at least until this driver
 * understands FAST_READ (for clocks over 25 MHz).
 */
static int __init serflash_probe(void)
{
	struct serflash			*flash;
	struct flash_info		*info;
	unsigned			i;

       // jedec_probe() will read id, so initialize hardware first
       MDrv_SERFLASH_Init();

	/* Platform data helps sort out which chip type we have, as
	 * well as how this board partitions it.  If we don't have
	 * a chip ID, try the JEDEC id commands; they'll work for most
	 * newer chips, even if we don't recognize the particular chip.
	 */
	info = jedec_probe();

	if (!info)
		return -ENODEV;

	flash = kzalloc(sizeof *flash, GFP_KERNEL);
	if (!flash)
		return -ENOMEM;

	mutex_init(&flash->lock);

       flash->mtd.priv = flash;

#if 0
	/*
	 * Atmel serial flash tend to power up
	 * with the software protection bits set
	 */

	if (info->jedec_id >> 16 == 0x1f) {
		write_enable(flash);
		write_sr(flash, 0);
	}
#endif

	flash->mtd.type = MTD_NORFLASH;
	flash->mtd.writesize = 1;
	flash->mtd.flags = MTD_CAP_NORFLASH;
	//flash->mtd.size = info->sector_size * info->n_sectors;
        MDrv_SERFLASH_DetectSize((MS_U32*)&flash->mtd.size);
	flash->mtd.erase = serflash_erase;
	flash->mtd.read = serflash_read;
	flash->mtd.write = serflash_write;

	/* prefer "small sector" erase if possible */
	if (info->flags & SECT_4K) {
		flash->erase_opcode = OPCODE_BE_4K;
		flash->mtd.erasesize = 4096;
	} else {
		flash->erase_opcode = OPCODE_SE;
		flash->mtd.erasesize = info->sector_size;
	}

	//dev_info(&spi->dev, "%s (%d Kbytes)\n", info->name,
			//flash->mtd.size / 1024);

	DEBUG(MTD_DEBUG_LEVEL2,
		"mtd .name = %s, .size = 0x%.8x (%uMiB) "
			".erasesize = 0x%.8x (%uKiB) .numeraseregions = %d\n",
		flash->mtd.name,
		flash->mtd.size, flash->mtd.size / (1024*1024),
		flash->mtd.erasesize, flash->mtd.erasesize / 1024,
		flash->mtd.numeraseregions);

	if (flash->mtd.numeraseregions)
		for (i = 0; i < flash->mtd.numeraseregions; i++)
			DEBUG(MTD_DEBUG_LEVEL2,
				"mtd.eraseregions[%d] = { .offset = 0x%.8x, "
				".erasesize = 0x%.8x (%uKiB), "
				".numblocks = %d }\n",
				i, flash->mtd.eraseregions[i].offset,
				flash->mtd.eraseregions[i].erasesize,
				flash->mtd.eraseregions[i].erasesize / 1024,
				flash->mtd.eraseregions[i].numblocks);


	/* partitions should match sector boundaries; and it may be good to
	 * use readonly partitions for writeprotected sectors (BP2..BP0).
	 */
	if (mtd_has_partitions()) {
		struct mtd_partition	*parts = NULL;
		int			nr_parts = 0;

#ifdef CONFIG_MTD_CMDLINE_PARTS
		static const char *part_probes[] = { "cmdlinepart", NULL, };

		nr_parts = parse_mtd_partitions(&flash->mtd,
				part_probes, &parts, 0);
#endif

		if (nr_parts > 0) {
			for (i = 0; i < nr_parts; i++) {
				DEBUG(MTD_DEBUG_LEVEL2, "partitions[%d] = "
					"{.name = %s, .offset = 0x%.8x, "
						".size = 0x%.8x (%uKiB) }\n",
					i, parts[i].name,
					parts[i].offset,
					parts[i].size,
					parts[i].size / 1024);
			}
			flash->partitioned = 1;
			return add_mtd_partitions(&flash->mtd, parts, nr_parts);
		}
              else
		    return add_mtd_partitions(&flash->mtd, serflash_partition_info, SERFLASH_NUM_PARTITIONS);
	} 

	return add_mtd_device(&flash->mtd) == 1 ? -ENODEV : 0;
}

#if 0
static int serflash_remove(struct mtd_info *mtd)
{
	struct serflash	*flash = mtd->priv;
	int		status;

	/* Clean up MTD stuff. */
	if (mtd_has_partitions() && flash->partitioned)
		status = del_mtd_partitions(&flash->mtd);
	else
		status = del_mtd_device(&flash->mtd);
	if (status == 0)
		kfree(flash);
    
	return 0;
}
#endif

static void __exit serflash_cleanup(void)
{
}


module_init(serflash_probe);
module_exit(serflash_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tao.Zhou");
MODULE_DESCRIPTION("MTD Mstar driver for spi flash chips");


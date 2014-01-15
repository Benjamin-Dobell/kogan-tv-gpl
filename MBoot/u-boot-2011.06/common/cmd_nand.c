/*
 * Driver for NAND support, Rick Bronson
 * borrowed heavily from:
 * (c) 1999 Machine Vision Holdings, Inc.
 * (c) 1999, 2000 David Woodhouse <dwmw2@infradead.org>
 *
 * Ported 'dynenv' to 'nand env.oob' command
 * (C) 2010 Nanometrics, Inc.
 * 'dynenv' -- Dynamic environment offset in NAND OOB
 * (C) Copyright 2006-2007 OpenMoko, Inc.
 * Added 16-bit nand support
 * (C) 2004 Texas Instruments
 *
 * Copyright 2010 Freescale Semiconductor
 * The portions of this file whose copyright is held by Freescale and which
 * are not considered a derived work of GPL v2-only code may be distributed
 * and/or modified under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */

#include <common.h>
#include <linux/mtd/mtd.h>
#include <command.h>
#include <watchdog.h>
#include <malloc.h>
#include <asm/byteorder.h>
#include <jffs2/jffs2.h>
#include <nand.h>

//Mstar Start

#include "../drivers/mstar/unfd/inc/common/drvNAND.h"
extern int drvNAND_ReadCISBlk(U32 u32_off, U8* pu8_DataBuf);
#include <fat.h>
#include "uboot_mmap.h"
#define READ_BUFFER_LEN 0x100000
#define NAND_UPGRADE_BUFFER_ADDR 0x44500000
#define NAND_BIN_NAME   "nand.bin"
#define NAND_BUFFER_LEN(pagelen,fatblock) (pagelen * fatblock * 12)

#if defined(CONFIG_JFFS2_CMDLINE)
#define BOOT_IN_MTD    1
#endif

#define SBOOT_SIZE 0x10000
//extern block_dev_desc_t *get_dev (const char* ifname, int dev);

#if defined(CONFIG_CMD_JFFS2) && defined(CONFIG_JFFS2_CMDLINE)
#define KL_IN_MTD    1
#endif
#define NAND_CLK_LEN   2

extern int strcmp(const char * cs,const char * ct);

#if defined(CONFIG_ENV_IS_IN_NAND)
extern unsigned int cfg_env_offset;
#endif

extern void drvNAND_BIST_Test(void);
extern void drvNAND_Stable_Test(void);

EN_NAND_FS_FORMAT nand_fs_format = NAND_FS_FORMAT_NORMAL;

//Mstar End


#if defined(CONFIG_CMD_MTDPARTS)

/* partition handling routines */
int mtdparts_init(void);
int id_parse(const char *id, const char **ret_id, u8 *dev_type, u8 *dev_num);
int find_dev_and_part(const char *id, struct mtd_device **dev,
		      u8 *part_num, struct part_info **part);
struct part_info* mtd_part_info(struct mtd_device *dev, unsigned int part_num);
#endif

static int nand_dump(nand_info_t *nand, ulong off, int only_oob, int repeat)
{
	int i=0;
	u_char *datbuf=NULL, *oobbuf=NULL, *p=NULL;
	static loff_t last;

	if (repeat)
		off = last + nand->writesize;

	last = off;

	datbuf = malloc(nand->writesize + nand->oobsize);
	oobbuf = malloc(nand->oobsize);
	if (!datbuf || !oobbuf) {
		puts("No memory for page buffer\n");
		return 1;
	}
	off &= ~(nand->writesize - 1);
	loff_t addr = (loff_t) off;
	struct mtd_oob_ops ops;
	memset(&ops, 0, sizeof(ops));
	ops.datbuf = datbuf;
	ops.oobbuf = oobbuf; /* must exist, but oob data will be appended to ops.datbuf */
	ops.len = nand->writesize;
	ops.ooblen = nand->oobsize;
	ops.mode = MTD_OOB_RAW;
	i = nand->read_oob(nand, addr, &ops);
	if (i < 0) {
		printf("Error (%d) reading page %08lx\n", i, off);
		free(datbuf);
		free(oobbuf);
		return 1;
	}
	printf("Page %08lx dump:\n", off);
	i = nand->writesize >> 4;
	p = datbuf;

	while (i--) {
		if (!only_oob)
			printf("\t%02x %02x %02x %02x %02x %02x %02x %02x"
			       "  %02x %02x %02x %02x %02x %02x %02x %02x\n",
			       p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
			       p[8], p[9], p[10], p[11], p[12], p[13], p[14],
			       p[15]);
		p += 16;
	}
	puts("OOB:\n");
	i = nand->oobsize >> 3;
	while (i--) {
		printf("\t%02x %02x %02x %02x %02x %02x %02x %02x\n",
		       p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
		p += 8;
	}
	free(datbuf);
	free(oobbuf);

	return 0;
}

/* ------------------------------------------------------------------------- */

static int set_dev(int dev)
{
	if (dev < 0 || dev >= CONFIG_SYS_MAX_NAND_DEVICE ||
	    !nand_info[dev].name) {
		puts("No such device\n");
		return -1;
	}

	if (nand_curr_device == dev)
		return 0;

	printf("Device %d: %s", dev, nand_info[dev].name);
	puts("... is now current device\n");
	nand_curr_device = dev;

#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
	board_nand_select_device(nand_info[dev].priv, dev);
#endif

	return 0;
}

static inline int str2off(const char *p, loff_t *num)
{
	char *endptr=NULL;

	*num = simple_strtoull(p, &endptr, 16);
	return *p != '\0' && *endptr == '\0';
}

static inline int str2long(const char *p, ulong *num)
{
	char *endptr;

	*num = simple_strtoul(p, &endptr, 16);
	return *p != '\0' && *endptr == '\0';
}

static int get_part(const char *partname, int *idx, loff_t *off, loff_t *size)
{
#ifdef CONFIG_CMD_MTDPARTS
	struct mtd_device *dev;
	struct part_info *part;
	u8 pnum;
	int ret;

	ret = mtdparts_init();
	if (ret)
		return ret;

	ret = find_dev_and_part(partname, &dev, &pnum, &part);
	if (ret)
		return ret;

	if (dev->id->type != MTD_DEV_TYPE_NAND) {
		puts("not a NAND device\n");
		return -1;
	}

	*off = part->offset;
	*size = part->size;
	*idx = dev->id->num;

	ret = set_dev(*idx);
	if (ret)
		return ret;

	return 0;
#else
	puts("offset is not a number\n");
	return -1;
#endif
}

static int arg_off(const char *arg, int *idx, loff_t *off, loff_t *maxsize)
{
	if (!str2off(arg, off))
		return get_part(arg, idx, off, maxsize);

	if (*off >= nand_info[*idx].size) {
		puts("Offset exceeds device limit\n");
		return -1;
	}

	*maxsize = nand_info[*idx].size - *off;
	return 0;
}

static int arg_off_size(int argc, char *const argv[], int *idx,
			loff_t *off, loff_t *size)
{
	int ret;
	loff_t maxsize;

	if (argc == 0) {
		*off = 0;
		*size = nand_info[*idx].size;
		goto print;
	}

	ret = arg_off(argv[0], idx, off, &maxsize);
	if (ret)
		return ret;

	if (argc == 1) {
		*size = maxsize;
		goto print;
	}

	if (!str2off(argv[1], size)) {
		printf("'%s' is not a number\n", argv[1]);
		return -1;
	}

	if (*size > maxsize) {
		puts("Size exceeds partition or device limit\n");
		return -1;
	}

print:
#if (!CONFIG_MSTAR_SILENT)
	printf("device %d ", *idx);
	if (*size == nand_info[*idx].size)
		puts("whole chip\n");
	else
		printf("offset 0x%llx, size 0x%llx\n",
		       (unsigned long long)*off, (unsigned long long)*size);
#endif
	return 0;
}

#ifdef CONFIG_CMD_NAND_LOCK_UNLOCK
static void print_status(ulong start, ulong end, ulong erasesize, int status)
{
	printf("%08lx - %08lx: %08lx blocks %s%s%s\n",
		start,
		end - 1,
		(end - start) / erasesize,
		((status & NAND_LOCK_STATUS_TIGHT) ?  "TIGHT " : ""),
		((status & NAND_LOCK_STATUS_LOCK) ?  "LOCK " : ""),
		((status & NAND_LOCK_STATUS_UNLOCK) ?  "UNLOCK " : ""));
}

static void do_nand_status(nand_info_t *nand)
{
	ulong block_start = 0;
	ulong off;
	int last_status = -1;

	struct nand_chip *nand_chip = nand->priv;
	/* check the WP bit */
	nand_chip->cmdfunc(nand, NAND_CMD_STATUS, -1, -1);
	printf("device is %swrite protected\n",
		(nand_chip->read_byte(nand) & 0x80 ?
		"NOT " : ""));

	for (off = 0; off < nand->size; off += nand->erasesize) {
		int s = nand_get_lock_status(nand, off);

		/* print message only if status has changed */
		if (s != last_status && off != 0) {
			print_status(block_start, off, nand->erasesize,
					last_status);
			block_start = off;
		}
		last_status = s;
	}
	/* Print the last block info */
	print_status(block_start, off, nand->erasesize, last_status);
}
#endif

#ifdef CONFIG_ENV_OFFSET_OOB
unsigned long nand_env_oob_offset;

int do_nand_env_oob(cmd_tbl_t *cmdtp, int argc, char *const argv[])
{
	int ret;
	uint32_t oob_buf[ENV_OFFSET_SIZE/sizeof(uint32_t)];
	nand_info_t *nand = &nand_info[0];
	char *cmd = argv[1];

	if (CONFIG_SYS_MAX_NAND_DEVICE == 0 || !nand->name) {
		puts("no devices available\n");
		return 1;
	}

	set_dev(0);

	if (!strcmp(cmd, "get")) {
		ret = get_nand_env_oob(nand, &nand_env_oob_offset);
		if (ret)
			return 1;

		printf("0x%08lx\n", nand_env_oob_offset);
	} else if (!strcmp(cmd, "set")) {
		loff_t addr;
		loff_t maxsize;
		struct mtd_oob_ops ops;
		int idx = 0;

		if (argc < 3)
			goto usage;

		if (arg_off(argv[2], &idx, &addr, &maxsize)) {
			puts("Offset or partition name expected\n");
			return 1;
		}

		if (idx != 0) {
			puts("Partition not on first NAND device\n");
			return 1;
		}

		if (nand->oobavail < ENV_OFFSET_SIZE) {
			printf("Insufficient available OOB bytes:\n"
			       "%d OOB bytes available but %d required for "
			       "env.oob support\n",
			       nand->oobavail, ENV_OFFSET_SIZE);
			return 1;
		}

		if ((addr & (nand->erasesize - 1)) != 0) {
			printf("Environment offset must be block-aligned\n");
			return 1;
		}

		ops.datbuf = NULL;
		ops.mode = MTD_OOB_AUTO;
		ops.ooboffs = 0;
		ops.ooblen = ENV_OFFSET_SIZE;
		ops.oobbuf = (void *) oob_buf;

		oob_buf[0] = ENV_OOB_MARKER;
		oob_buf[1] = addr / nand->erasesize;

		ret = nand->write_oob(nand, ENV_OFFSET_SIZE, &ops);
		if (ret) {
			printf("Error writing OOB block 0\n");
			return ret;
		}

		ret = get_nand_env_oob(nand, &nand_env_oob_offset);
		if (ret) {
			printf("Error reading env offset in OOB\n");
			return ret;
		}

		if (addr != nand_env_oob_offset) {
			printf("Verification of env offset in OOB failed: "
			       "0x%08llx expected but got 0x%08lx\n",
			       (unsigned long long)addr, nand_env_oob_offset);
			return 1;
		}
	} else {
		goto usage;
	}

	return ret;

usage:
	return cmd_usage(cmdtp);
}

#endif

static void nand_print_info(int idx)
{
	nand_info_t *nand = &nand_info[idx];
	struct nand_chip *chip = nand->priv;
	printf("Device %d: ", idx);
	if (chip->numchips > 1)
		printf("%dx ", chip->numchips);
	printf("%s, sector size %u KiB\n",
	       nand->name, (u_int32_t)(nand->erasesize >> 10));
}

#ifdef CONFIG_USB
int dd_cisblock(nand_info_t *meminfo, const nand_read_options_t *opts)
{
    block_dev_desc_t *dev_desc=NULL;
    int dev=0;
    char *ep=NULL, *cmdlinebase="nand read.cis", *cmdlinefull=NULL;
    int ret=0;

    /* Write to usb for binary file making
    Initialize something and open usb file */
    dev = (int)simple_strtoul ("0", &ep, 16);
    dev_desc=get_dev("usb", dev);
    if (dev_desc==NULL) {
      puts ("\n** Invalid boot device **\n");
      return -1;
    }
    
    cmdlinefull = (char *) malloc(100);
    sprintf(cmdlinefull, "%s %p %llx", cmdlinebase, opts->buffer, opts->offset);

    run_command(cmdlinefull, 0);
    free(cmdlinefull);
    
    ret = dev_desc->block_write(dev_desc->dev, opts->offset / 512,
                            opts->length / 512, (ulong*)opts->buffer);
    return 0;
}

int dd_cis_partition(nand_info_t *meminfo, nand_read_options_t *opts)
{
        int ret=0, i=0, cisblocknum=0;
        loff_t offs=0;

        for(i = 0; i < 10; i++){
            ret = meminfo->block_isbad(meminfo, offs);
            if(2 == ret){
                opts->offset = (offs / meminfo->oobblock) *
                                    (meminfo->oobblock + meminfo->oobsize);;
                opts->length = (meminfo->erasesize / meminfo->oobblock) *
                                    (meminfo->oobblock + meminfo->oobsize);
                memset(opts->buffer, 0xff, opts->length);
                ret = dd_cisblock(meminfo, opts);
                flush_cache((ulong)opts->buffer, opts->length);
                if(ret)
                    return ret;
                cisblocknum++;
                if(cisblocknum >= 2)
                    return 0;
             }

             offs += meminfo->erasesize;
        }

        return 0;
}

static int do_ddnand(nand_info_t *nand, char * const argv[], int restore)
{
    int ret=0, percent_complete=-1;
    nand_read_options_t opts;
    int dev=0, nand_read_len=0, read_len=0, 
         bad_len=0, read_len_oob=0, bad_len_oob=0, erasesize_oob=0;
    char *ep;
    block_dev_desc_t *dev_desc=NULL;
    ulong readblkcnt, usbstartblk;    
    u64 imglen=0, nandoffset=0, nandoffsetoob=0;
    struct part_info *part=NULL;    
    struct mtd_device *mtddev=NULL;
    u8 pnum=0;
    u16 partnum=0;
    
    dev = (int)simple_strtoul ("0", &ep, 16);
    dev_desc=get_dev("usb",dev);
    if (dev_desc==NULL) {
        puts ("\n** Invalid boot device **\n");
        return -1;
    }
//    printf("Blksz=%ld, Blknum=%ld\n",dev_desc->blksz, dev_desc->lba);
    if(!restore && ((dev_desc->lba * dev_desc->blksz) < nand->size)){
        printf("\n** Please Check the usb storage capacity! **\n");
        return -1;
    }
    
    memset(&opts, 0, sizeof(opts));
    opts.buffer = (u_char*) NAND_UPGRADE_BUFFER_ADDR;
    opts.offset = 0;
    opts.quiet = 0;
    
    if ((mtdparts_init() == 0) &&
        ((find_dev_and_part("ubi", &mtddev, &pnum, &part) == 0) ||
        (find_dev_and_part("UBI", &mtddev, &pnum, &part) == 0))) {
        if (mtddev->id->type != MTD_DEV_TYPE_NAND) {
            puts("not a NAND device\n");
            return -1;
        }
    }

    if(part == NULL)
    {
        printf("%s: Error: null part, at %d\n", __func__, __LINE__);
        return -1;
    }

    for(partnum = 0; partnum < mtddev->num_parts; partnum ++){
        part = mtd_part_info(mtddev, partnum);
        if(part == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }

        if((0 == strcmp(part->name, "CIS")) || (0 == strcmp(part->name, "cis"))){
            ret = dd_cis_partition(nand, &opts);
            if(ret)
                return -1;
            printf("\n***Dump cis partition success!***\n");
            continue;
        }

        printf("\n***Start to write partition %s at offset: 0x%llx, length: 0x%llx***\n",
                part->name, part->offset, part->size);
                
        /* get image length */
        opts.offset = part->offset;
        opts.length = part->size;    
        imglen = opts.length;
        nandoffset = opts.offset;
        nandoffsetoob = (nandoffset / nand->oobblock) *
                                (nand->oobblock + nand->oobsize);
        usbstartblk = ALIGN(nandoffsetoob, 512) / 512;
        bad_len = 0;
    
        /* get data from input and write to the device */
        while (imglen && (nandoffset < nand->size))
        {

            WATCHDOG_RESET ();

            if (imglen >= READ_BUFFER_LEN * 10)
            {
                nand_read_len = READ_BUFFER_LEN * 10;
            }
            else
            {
                nand_read_len = imglen;
            }

            read_len = nand_read_len;

            if(restore){            
                printf("No implemented\n");
                return 0;            
            }else{
                ret = nand_read_oob_skip_bad(nand, nandoffset, (size_t *)&read_len, opts.buffer);
                if(ret < 0) {
                    printf("\n** Unable to read the nand **\n");
                    return 1;
                }
                read_len_oob = (read_len / nand->oobblock) *
                                    (nand->oobblock + nand->oobsize);
                readblkcnt = ALIGN(read_len_oob, 512) / 512;                
                if(readblkcnt){
                    ret = dev_desc->block_write(dev_desc->dev, usbstartblk, readblkcnt, (ulong*)opts.buffer);
                    if(ret < 0) {
                        printf("\n** Unable to write the usb storage **\n");
                        return 1;
                    }
                    usbstartblk += readblkcnt;
                    flush_cache((ulong)opts.buffer, read_len);
                }
                                
                bad_len  += nand_read_len - read_len;
            }

            imglen -= nand_read_len;

            if (!opts.quiet)
            {
                u64 percent = (u64)
                    ((unsigned long long)
                    (opts.length-imglen) * 100
                    / opts.length);
                /* output progress message only at whole percent
                * steps to reduce the number of messages printed
                * on (slow) serial consoles
                */
                if (percent != percent_complete)
                {
                    printf("\rWriting data at 0x%llx "
                            "-- %3lld%% complete.",
                            nandoffset, percent);
                    percent_complete = percent;
                }
            }

            nandoffset += nand_read_len;
        }
        
        if(bad_len){
            bad_len_oob = (bad_len / nand->oobblock) *
                                    (nand->oobblock + nand->oobsize);
            erasesize_oob = (nand->erasesize / nand->oobblock) *
                                    (nand->oobblock + nand->oobsize);
            memset(opts.buffer, 0xff, erasesize_oob);
            readblkcnt = ALIGN(bad_len_oob, erasesize_oob) / erasesize_oob; 
            while(readblkcnt--){
                ret = dev_desc->block_write(dev_desc->dev, usbstartblk, erasesize_oob/512, (ulong*)opts.buffer);
                if(ret < 0) {
                    printf("\n** Unable to write the usb storage **\n");
                    return 1;
                }
                usbstartblk += erasesize_oob/512;
                flush_cache((ulong)opts.buffer, erasesize_oob);
            }
        }    
        printf("\n***Finished partition %s writing***\n", part->name);
    }
    
    printf("\nDump Block: %lld\n", nand->size / 512);
    return 0;
}
#endif
int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int i=0, ret = 0;
	ulong addr=0;
	loff_t off=0, size=0;
	char *cmd, *s=NULL;
	nand_info_t *nand=NULL;
#ifdef CONFIG_SYS_NAND_QUIET
	int quiet = CONFIG_SYS_NAND_QUIET;
#else
	int quiet = 0;
#endif
	const char *quiet_str = getenv("quiet");
	int dev = nand_curr_device;
	int repeat = flag & CMD_FLAG_REPEAT;
//Mstar Start
    int eraseloop=0;
//Mstar End
	/* at least two arguments please */
	if (argc < 2)
		goto usage;
//Mstar Start
#if defined (CONFIG_BACKUP_PARTITION)
    {
        char *pEnv=NULL;
        pEnv=getenv("updateKLPermit");
        if(pEnv!=NULL){
            if(strcmp(pEnv,"NO")==0){
                if((strncmp(argv[1],"erase",5)==0)||(strncmp(argv[1],"write.e",7)==0)){
                    printf("updateKLPermit=NO");
                    return 0;
                }
            }
        }
    }
#endif
//Mstar End
	if (quiet_str)
		quiet = simple_strtoul(quiet_str, NULL, 0) != 0;

	cmd = argv[1];

	/* Only "dump" is repeatable. */
	if (repeat && strcmp(cmd, "dump"))
		return 0;

	if (strcmp(cmd, "info") == 0) {

		putc('\n');
		for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++) {
			if (nand_info[i].name)
				nand_print_info(i);
		}
		return 0;
	}

	if (strcmp(cmd, "device") == 0) {
#if 0   //Mstar Modify
		if (argc < 3) {
			putc('\n');
			if (dev < 0 || dev >= CONFIG_SYS_MAX_NAND_DEVICE)
				puts("no devices available\n");
			else
				nand_print_info(dev);
			return 0;
		}
#endif

		dev = (int)simple_strtoul(argv[2], NULL, 10);
		set_dev(dev);

		return 0;
	}

#ifdef CONFIG_ENV_OFFSET_OOB
	/* this command operates only on the first nand device */
	if (strcmp(cmd, "env.oob") == 0)
		return do_nand_env_oob(cmdtp, argc - 1, argv + 1);
#endif

	/* The following commands operate on the current device, unless
	 * overridden by a partition specifier.  Note that if somehow the
	 * current device is invalid, it will have to be changed to a valid
	 * one before these commands can run, even if a partition specifier
	 * for another device is to be used.
	 */
#if 0 //Mstar Modify
	if (dev < 0 || dev >= CONFIG_SYS_MAX_NAND_DEVICE ||
	    !nand_info[dev].name) {
		puts("\nno devices available\n");
		return 1;
	}
#endif
	nand = &nand_info[dev];

	if (strcmp(cmd, "bad") == 0) {
		printf("\nDevice %d bad blocks:\n", dev);
		for (off = 0; off < nand->size; off += nand->erasesize)
			if (nand_block_isbad(nand, off))
				printf("  %08llx\n", (unsigned long long)off);
		return 0;
	}
//Mstar Start
#if ENABLE_MODULE_BFN_WRITE_NAND
    /*
        * Syntax is:
        *   0    1     2       3    4
        *   nand bfn [miu/ldr/app] [blk addr][miu address]
        */
    if (strcmp(cmd, "bfn") == 0){
        printf("burn boot from nand data\n");
        if (argc < 4)
        {
            printf("argument wrong!\n");
            return 0;
        }
        U32 addr = simple_strtoul(argv[4], NULL, 16);
        U32 blk = simple_strtoul(argv[3], NULL, 16);
        U32 bfn_type = 0;
        if (strncmp(argv[2],"miu",3)==0)
        {
            printf("burn the MIU param\n");
            bfn_type = 1;
        }
        else if (strncmp(argv[2],"ldr",3)==0)
        {
            printf("burn the LDR param\n");
            bfn_type = 2;
        }
        else if (strncmp(argv[2],"app",3)==0)
        {
            printf("burn the APP param\n");
            bfn_type = 3;
        }
        else
        {
            printf("error boot from nand parameter\n");
            return 0;
        }
        drvNAND_BFN_FLASH_INFO(bfn_type,blk,addr);
        return ret == 0 ? 0 : 1;
    }
#endif

if( strcmp(cmd, "bist") == 0 ) {

    int loop = 0;

    if( !strcmp("forever", argv[2]) )
      loop = 1;

    puts("Warnning:\n"
       "bist option will damage all of your data\n"
       "including the kernel, file system\n"
       "and environments in NAND flash.\n\n"
       "Do you really want to do it? (y/n)\n\n");
    if (getc() == 'y' && getc() == '\r') {
        do {
          drvNAND_BIST_Test();
        } while( loop == 1 );
      } else {
        puts("bist aborted\n");
        return -1;
      }
    return 0;
  }

  if( strcmp(cmd, "stable") == 0 )
  {
    puts("Warnning:\n"
       "stable test option will damage all of your data\n"
       "including the kernel, file system\n"
       "and environments in NAND flash.\n\n"
       "Do you really want to do it? (y/n)\n\n");
    if (getc() == 'y' && getc() == '\r')
    {
      drvNAND_Stable_Test();
    }
    else
    {
      puts("stable test aborted\n");
      return -1;
    }
    return 0;
  }


//Mstar End
	/*
	 * Syntax is:
	 *   0    1     2       3    4
	 *   nand erase [clean] [off size]
	 */
	if (strncmp(cmd, "erase", 5) == 0 || strncmp(cmd, "scrub", 5) == 0
        ||strcmp(cmd, "eloop") == 0 ) {
        int eloop = !strcmp(cmd, "eloop");

		nand_erase_options_t opts;
		/* "clean" at index 2 means request to write cleanmarker */
		int clean = argc > 2 && !strcmp("clean", argv[2]);
		int o = clean ? 3 : 2;
		int scrub = !strncmp(cmd, "scrub", 5);
		int part = 0;
		int chip = 0;
		int spread = 0;
		int args = 2;

		if (cmd[5] != 0) {
			if (!strcmp(&cmd[5], ".spread")) {
				spread = 1;
			} else if (!strcmp(&cmd[5], ".part")) {
				part = 1;
				args = 1;
			} else if (!strcmp(&cmd[5], ".chip")) {
				chip = 1;
				args = 0;
			#if defined(CONFIG_ENV_IS_IN_NANDUBI)
			} else if(!strcmp(&cmd[5], ".chipforce") || !strcmp(&cmd[5], ".partforce") || !strcmp(&cmd[5], ".force")){
				chip = 2;
				args = 0;
			#endif
			} else {
				goto usage;
			}
		}

		/*
		 * Don't allow missing arguments to cause full chip/partition
		 * erases -- easy to do accidentally, e.g. with a misspelled
		 * variable name.
		 */
		if (argc != o + args)
			goto usage;

		printf("\nNAND %s: ", cmd);
		/* skip first two or three arguments, look for offset and size */
        //Mstar Start
         if(strcmp(cmd, "eloop") == 0 )
        {
            clean = 0;
            o     = 2;
          off   = 0;
          //size  = 0x4000000;
          size = nand->size;
        }
        else
        {
        //Mstar End
		if (arg_off_size(argc - o, argv + o, &dev, &off, &size) != 0)
			return 1;
		#if defined(CONFIG_ENV_IS_IN_NANDUBI)
		//get last partition name from cis pni
			{
				int part_idx;
				loff_t part_size, part_off;
				if(get_part("UBILD", &part_idx, &part_off, &part_size))
				{
					printf("Get UBILD partition information Error\n");
				}
				//checking offset and size
				if(chip!= 2 && scrub == 0)
				{
					if(off < part_off + part_size)
					{
						if(size + off > part_off+ part_size)
							size -= part_off + part_size - off;
						else
							size = 0;
						off = part_off + part_size;
						printf("Skip those blocks before UBILD partition\n");
					}
				}
			}
		#endif
        }

		nand = &nand_info[dev];

		memset(&opts, 0, sizeof(opts));
		opts.offset = off;
		opts.length = size;
		opts.jffs2  = clean;
		opts.quiet  = quiet;
		opts.spread = spread;

		if (scrub) {
			puts("Warning: "
			     "scrub option will erase all factory set "
			     "bad blocks!\n"
			     "         "
			     "There is no reliable way to recover them.\n"
			     "         "
			     "Use this command only for testing purposes "
			     "if you\n"
			     "         "
			     "are sure of what you are doing!\n"
			     "\nReally scrub this NAND flash? <y/N>\n");

			if (getc() == 'y') {
				puts("y");
				if (getc() == '\r')
					opts.scrub = 1;
				else {
					puts("scrub aborted\n");
					return -1;
				}
			} else {
				puts("scrub aborted\n");
				return -1;
			}
		}
		//ret = nand_erase_opts(nand, &opts);
		//printf("%s\n", ret ? "ERROR" : "OK");
		//Mstar Start
        if(eloop)
        {
                eraseloop = simple_strtoul(argv[2], NULL, 16);
          printf("eraseloop is %x",eraseloop);
        }
        else
        {
                eraseloop = 1;
        }

        for(i=0; i<eraseloop; i++)
        {
            ret = nand_erase_opts(nand, &opts);
            printf("%4x %s\n",i,ret ? "ERROR" : "OK");
        }
         #if defined (CONFIG_BACKUP_PARTITION)
                    if(ret!=0){
                        printf("setenv updateKLPermit NO\n");
                        run_command("setenv updateKLPermit NO",0);
                        //run_command("saveenv",0);
                    }

         #endif
         //Mstar End
    		return ret == 0 ? 0 : 1;
    	}

	if (strncmp(cmd, "dump", 4) == 0) {
		if (argc < 3)
			goto usage;

		off = (int)simple_strtoul(argv[2], NULL, 16);
		ret = nand_dump(nand, off, !strcmp(&cmd[4], ".oob"), repeat);

		return ret == 0 ? 1 : 0;
	}

	if (strncmp(cmd, "read", 4) == 0 || strncmp(cmd, "write", 5) == 0) {
		size_t rwsize;
		int read;

		if (argc < 4)
			goto usage;

		addr = (0 == strncmp(argv[2], UPGRADE_ADDR_STR, strlen(UPGRADE_ADDR_STR))) ? NAND_UPGRADE_BUFFER_ADDR : simple_strtoul(argv[2], NULL, 16);

		read = strncmp(cmd, "read", 4) == 0; /* 1 = read, 0 = write */
#if (!CONFIG_MSTAR_SILENT)
		printf("\nNAND %s: ", read ? "read" : "write");
#endif
		if (arg_off_size(argc - 3, argv + 3, &dev, &off, &size) != 0)
			return 1;
        nand_fs_format=NAND_FS_FORMAT_NORMAL;
		nand = &nand_info[dev];
		rwsize = size;

		s = strchr(cmd, '.');
		if (!s || !strcmp(s, ".jffs2") ||
		    !strcmp(s, ".e") || !strcmp(s, ".i")|| !strcmp(s, ".raw") || !strcmp(s, ".mkbin")||!strcmp(s,".slc") ||!strcmp(s,".cis")) {
//Mstar Start
            if(!strcmp(s, ".jffs2"))
             {
                 nand_fs_format=NAND_FS_FORMAT_JFFS2;
             }
             else if(!strcmp(s, ".e"))
             {
                 nand_fs_format=NAND_FS_FORMAT_E;
             }
             else if(!strcmp(s, ".i"))
             {
                 nand_fs_format=NAND_FS_FORMAT_I;
             }
                       else if(!strcmp(s, ".raw"))
             {
                 nand_fs_format=NAND_FS_FORMAT_RAW;
             }
                       else if(!strcmp(s, ".mkbin"))
             {
                 nand_fs_format=NAND_FS_FORMAT_MKBIN;
             }

            #if defined(NAND_SPEED_TEST) && NAND_SPEED_TEST
            if( size )
            {
                HWTimer_Start();
            }
            #endif
//Mstar End

			if(!strcmp(s, ".slc"))
			{
				if (read)
					ret = nand_read_slc_skip_bad(nand, off, &rwsize,
								 (u_char *)addr);
				else
					ret = nand_write_slc_skip_bad(nand, off, &rwsize,
								  (u_char *)addr, 0);
				#if 0
				if(ret == -117)
				{
					loff_t off_arg = 0, maxsize_arg;
					size_t  rwsize_arg;
					int ret_arg;
					nand_erase_options_t opts;

					printf("Corretable ECC happens when reading offset %llX, size %X\n", off, rwsize);
					if(!str2off((char * const) (argv[3]), &off_arg))
					{
						printf("Attempt to refresh \"%s\" partition\n", argv[3]);
						get_part((char * const) (argv[3]), &dev, &off_arg, &maxsize_arg);

						rwsize_arg = maxsize_arg / 2;

						ret_arg = nand_read_slc_skip_bad(nand, off_arg, &rwsize_arg, (u_char *) addr);

						if(ret_arg == 0 || ret_arg == -117)
						{
							memset(&opts, 0, sizeof(opts));
							opts.offset = off_arg;
							opts.length = maxsize_arg;
							opts.jffs2  = 0;
							opts.quiet  = quiet;
							opts.spread = 0;
							ret_arg = nand_erase_opts(nand, &opts);
							printf("%s\n",ret_arg ? "ERROR" : "OK");

							if(!ret_arg)
							{
								printf("write back data to \"%s\" partition\n", argv[3]);
								rwsize_arg = maxsize_arg / 2;
								ret_arg = nand_write_slc_skip_bad(nand, off_arg, &rwsize_arg, (u_char *) addr, 0);
								if(!ret_arg)
								{
									if (arg_off_size(argc - 3, argv + 3, &dev, &off, &size) != 0)
										return 1;
									rwsize = size;
									ret = nand_read_slc_skip_bad(nand, off, &rwsize, (u_char *) addr);
									if(ret == -117)
										ret = 0;
								}
							}
							else
							{
								printf("erase fail\n");
								printf("Please re-update whole image\n");
							}
						}
					}
					else
					{
						printf("No partition name in argument value, do nothing\n");
					}
				}
				#else
				if(ret == -117)
				{
					printf("Read correctable ecc bit\n");
					ret= 0;
				}
				#endif
			}
			else if(!strcmp(s, ".cis"))
			{
				if(read)
				{
					ret = drvNAND_ReadCISBlk(off,(U8*)addr);
					if(0 == ret)
						rwsize = nand->erasesize;
					else
						ret = -1;
				}
				else
				{
					printf("invalid command\n");
					ret = -1;
				}
			}
			else
			{
			if (read)
				ret = nand_read_skip_bad(nand, off, &rwsize,
							 (u_char *)addr);
			else
				ret = nand_write_skip_bad(nand, off, &rwsize,
							  (u_char *)addr, 0);
			}
//Mstar Start
            #if defined(NAND_SPEED_TEST) && NAND_SPEED_TEST
            if( size )
            {
                float speed = 0, tmp;
                unsigned int a, b = 0;
                u32 u32TotalNANDRWTime = 0;

                u32TotalNANDRWTime = HWTimer_End();

                speed = (((float)size)/(((float)u32TotalNANDRWTime)/((float)12000000)))/((float)(1024*1024));
                a = (int)speed;

                tmp = (speed - ((float)a))*1000;
                b = (int)tmp;

                printf("total time = %d.%03ds(%d)\n", u32TotalNANDRWTime/12000000, u32TotalNANDRWTime/12000, u32TotalNANDRWTime);
                printf("total size = %llX\n", size);
                printf("nand %s speed = %d.%03d MB/s\n", cmd, a, b);
            }
            #endif
//Mstar End

#ifdef CONFIG_CMD_NAND_YAFFS
		} else if (!strcmp(s, ".yaffs")) {
			if (read) {
				printf("Unknown nand command suffix '%s'.\n", s);
				return 1;
			}
			ret = nand_write_skip_bad(nand, off, &rwsize, (u_char *)addr, 1);
#endif
		} else if (!strcmp(s, ".oob")) {
			/* out-of-band data */
			mtd_oob_ops_t ops = {
				.oobbuf = (u8 *)addr,
				.ooblen = rwsize,
				.mode = MTD_OOB_RAW
			};

			if (read)
				ret = nand->read_oob(nand, off, &ops);
			else
				ret = nand->write_oob(nand, off, &ops);
		} else {
			printf("Unknown nand command suffix '%s'.\n", s);
			return 1;
		}
#if (!CONFIG_MSTAR_SILENT)
		printf(" %zu bytes %s: %s\n", rwsize,
		       read ? "read" : "written", ret ? "ERROR" : "OK");
#endif
		return ret == 0 ? 0 : 1;
	}

	if (strcmp(cmd, "markbad") == 0) {
		argc -= 2;
		argv += 2;

		if (argc <= 0)
			goto usage;

		while (argc > 0) {
			addr = simple_strtoul(*argv, NULL, 16);

			if (nand->block_markbad(nand, addr)) {
				printf("block 0x%08lx NOT marked "
					"as bad! ERROR %d\n",
					addr, ret);
				ret = 1;
			} else {
				printf("block 0x%08lx successfully "
					"marked as bad\n",
					addr);
			}
			--argc;
			++argv;
		}
		return ret;
	}

	if (strcmp(cmd, "biterr") == 0) {
		/* todo */
		return 1;
	}

#ifdef CONFIG_CMD_NAND_LOCK_UNLOCK
	if (strcmp(cmd, "lock") == 0) {
		int tight = 0;
		int status = 0;
		if (argc == 3) {
			if (!strcmp("tight", argv[2]))
				tight = 1;
			if (!strcmp("status", argv[2]))
				status = 1;
		}
		if (status) {
			do_nand_status(nand);
		} else {
			if (!nand_lock(nand, tight)) {
				puts("NAND flash successfully locked\n");
			} else {
				puts("Error locking NAND flash\n");
				return 1;
			}
		}
		return 0;
	}

	if (strcmp(cmd, "unlock") == 0) {
		if (arg_off_size(argc - 2, argv + 2, &dev, &off, &size) < 0)
			return 1;

		if (!nand_unlock(&nand_info[dev], off, size)) {
			puts("NAND flash successfully unlocked\n");
		} else {
			puts("Error unlocking NAND flash, "
			     "write and erase will probably fail\n");
			return 1;
		}
		return 0;
	}
#endif

    #if defined(IF_IP_VERIFY) && IF_IP_VERIFY
  if( strcmp(cmd, "ipverify") == 0 )
  {
    puts("Warnning:\n"
       "ipverify option will damage all of your data\n"
       "including the kernel, file system\n"
       "and environments in NAND flash.\n\n"
       "Do you really want to do it? (y/n)\n");
    if (getc() == 'y' && getc() == '\r')
    {
      drvNAND_IPVerify_Main();
    }
    else
    {
      puts("ip verify aborted\n");
      return -1;
    }
    return 0;
  }
  if(strcmp(cmd, "test") == 0)
  {
	  U32 u32_star_blk_adr = simple_strtoul(argv[4], NULL, 16);

	  if(strcmp(argv[2],"readloop") == 0)
	  {
	      if(strcmp(argv[3],"init") == 0)
		     ret = NCTEST_ReadLoopInitData(u32_star_blk_adr);
          else if (strcmp(argv[3],"test") == 0)
		     ret = NCTEST_ReadLoop(u32_star_blk_adr);
	  }
	if(ret ==0)
	   return 0;
	else
	  return -1;
  } 
  #endif
  
  if( strcmp(cmd, "dd") == 0 )
  {
#ifdef CONFIG_USB
    int port=0;

    if(argc > 3)
        port = (int)simple_strtoul (argv[3], NULL, 16);

    switch(port){
        case 1:
            run_command("usb start 1", 0);
            break;
        case 2:
            run_command("usb start 2", 0);
            break;
        default:
            run_command("usb start", 0);
            break;
    }

    if(strcmp(argv[2], "nand2usb") == 0)
        ret = do_ddnand(nand, argv, 0);
    else if(strcmp(argv[2], "usb2nand") == 0)
        ret = do_ddnand(nand, argv, 1);
    else
        return cmd_usage(cmdtp);

    return ret;
#else
    printf("Please check the USB config\n");
    return 0;
#endif
  }
usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	nand, CONFIG_SYS_MAXARGS, 1, do_nand,
	"NAND sub-system",
	"info - show available NAND devices\n"
	"nand device [dev] - show or set current device\n"
	"nand read - addr off|partition size\n"
	"nand write - addr off|partition size\n"
	"    read/write 'size' bytes starting at offset 'off'\n"
	"    to/from memory address 'addr', skipping bad blocks.\n"
#ifdef CONFIG_CMD_NAND_YAFFS
	"nand write.yaffs - addr off|partition size\n"
	"    write 'size' bytes starting at offset 'off' with yaffs format\n"
	"    from memory address 'addr', skipping bad blocks.\n"
#endif
	"nand erase[.spread] [clean] off size - erase 'size' bytes "
	"from offset 'off'\n"
	"    With '.spread', erase enough for given file size, otherwise,\n"
	"    'size' includes skipped bad blocks.\n"
	"nand erase.part [clean] partition - erase entire mtd partition'\n"
	"nand erase.chip [clean] - erase entire chip'\n"
	"nand bad - show bad blocks\n"
	"nand dump[.oob] off - dump page\n"
	"nand scrub off size | scrub.part partition | scrub.chip\n"
	"    really clean NAND erasing bad blocks (UNSAFE)\n"
	"nand markbad off [...] - mark bad block(s) at offset (UNSAFE)\n"
	"nand biterr off - make a bit error at offset (UNSAFE)"
#ifdef CONFIG_CMD_NAND_LOCK_UNLOCK
	"\n"
	"nand lock [tight] [status]\n"
	"    bring nand to lock state or display locked pages\n"
	"nand unlock [offset] [size] - unlock section"
#endif
#ifdef CONFIG_ENV_OFFSET_OOB
	"\n"
	"nand env.oob - environment offset in OOB of block 0 of"
	"    first device.\n"
	"nand env.oob set off|partition - set enviromnent offset\n"
	"nand env.oob get - get environment offset\n"
#endif

    #if defined(IF_IP_VERIFY) && IF_IP_VERIFY
  "nand ipverify - IP Verify test\n"
  "nand test readloop init blk\n"
  "nand test readloop test blk\n"
  #endif
	"nand dd nand2usb/usb2nand [usb port num] - dump/restore nand raw data between nand and usb\n"  
);

static int nand_load_image(cmd_tbl_t *cmdtp, nand_info_t *nand,
			   ulong offset, ulong addr, char *cmd)
{
	int r=0;
	char *ep=NULL, *s=NULL;
	size_t cnt;
	image_header_t *hdr=NULL;
#if defined(CONFIG_FIT)
	const void *fit_hdr = NULL;
#endif

	s = strchr(cmd, '.');
	if (s != NULL &&
	    (strcmp(s, ".jffs2") && strcmp(s, ".e") && strcmp(s, ".i"))) {
		printf("Unknown nand load suffix '%s'\n", s);
		show_boot_progress(-53);
		return 1;
	}

	printf("\nLoading from %s, offset 0x%lx\n", nand->name, offset);

	cnt = nand->writesize;
	r = nand_read_skip_bad(nand, offset, &cnt, (u_char *) addr);
	if (r) {
		puts("** Read error\n");
		show_boot_progress (-56);
		return 1;
	}
	show_boot_progress (56);

	switch (genimg_get_format ((void *)addr)) {
	case IMAGE_FORMAT_LEGACY:
		hdr = (image_header_t *)addr;

		show_boot_progress (57);
		image_print_contents (hdr);

		cnt = image_get_image_size (hdr);
		break;
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		fit_hdr = (const void *)addr;
		puts ("Fit image detected...\n");

		cnt = fit_get_size (fit_hdr);
		break;
#endif
	default:
		show_boot_progress (-57);
		puts ("** Unknown image type\n");
		return 1;
	}
	show_boot_progress (57);

	r = nand_read_skip_bad(nand, offset, &cnt, (u_char *) addr);
	if (r) {
		puts("** Read error\n");
		show_boot_progress (-58);
		return 1;
	}
	show_boot_progress (58);

#if defined(CONFIG_FIT)
	/* This cannot be done earlier, we need complete FIT image in RAM first */
	if (genimg_get_format ((void *)addr) == IMAGE_FORMAT_FIT) {
		if (!fit_check_format (fit_hdr)) {
			show_boot_progress (-150);
			puts ("** Bad FIT image format\n");
			return 1;
		}
		show_boot_progress (151);
		fit_print_contents (fit_hdr);
	}
#endif

	/* Loading ok, update default load address */

	load_addr = addr;

	/* Check if we should attempt an auto-start */
	if (((ep = getenv("autostart")) != NULL) && (strcmp(ep, "yes") == 0)) {
		char *local_args[2];

		local_args[0] = cmd;
		local_args[1] = NULL;

		printf("Automatic boot of image at addr 0x%08lx ...\n", addr);

		do_bootm(cmdtp, 0, 1, local_args);
		return 1;
	}
	return 0;
}

int do_nandboot(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	char *boot_device = NULL;
	int idx=0;
	ulong addr=0, offset = 0;
#if defined(CONFIG_CMD_MTDPARTS)
	struct mtd_device *dev=NULL;
	struct part_info *part=NULL;
	u8 pnum=0;

	if (argc >= 2) {
		char *p = (argc == 2) ? argv[1] : argv[2];
		if (!(str2long(p, &addr)) && (mtdparts_init() == 0) &&
		    (find_dev_and_part(p, &dev, &pnum, &part) == 0)) {
			if (dev->id->type != MTD_DEV_TYPE_NAND) {
				puts("Not a NAND device\n");
				return 1;
			}
			if (argc > 3)
				goto usage;
			if (argc == 3)
				addr = simple_strtoul(argv[1], NULL, 16);
			else
				addr = CONFIG_SYS_LOAD_ADDR;
			return nand_load_image(cmdtp, &nand_info[dev->id->num],
					       part->offset, addr, argv[0]);
		}
	}
#endif

	show_boot_progress(52);
	switch (argc) {
	case 1:
		addr = CONFIG_SYS_LOAD_ADDR;
		boot_device = getenv("bootdevice");
		break;
	case 2:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = getenv("bootdevice");
		break;
	case 3:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = argv[2];
		break;
	case 4:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = argv[2];
		offset = simple_strtoul(argv[3], NULL, 16);
		break;
	default:
#if defined(CONFIG_CMD_MTDPARTS)
usage:
#endif
		show_boot_progress(-53);
		return cmd_usage(cmdtp);
	}

	show_boot_progress(53);
	if (!boot_device) {
		puts("\n** No boot device **\n");
		show_boot_progress(-54);
		return 1;
	}
	show_boot_progress(54);

	idx = simple_strtoul(boot_device, NULL, 16);

	if (idx < 0 || idx >= CONFIG_SYS_MAX_NAND_DEVICE || !nand_info[idx].name) {
		printf("\n** Device %d not available\n", idx);
		show_boot_progress(-55);
		return 1;
	}
	show_boot_progress(55);

	return nand_load_image(cmdtp, &nand_info[idx], offset, addr, argv[0]);
}
#if 0
U_BOOT_CMD(nboot, 4, 1, do_nandboot,
	"boot from NAND device",
	"[partition] | [[[loadAddr] dev] offset]"
);
#endif
//Mstar Start

int do_nandclk(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    ushort mode=0;
    uint nandclk=0;
    uint shownandclk=0;

    if((argc < NAND_CLK_LEN) || (argc > NAND_CLK_LEN))
    {
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    mode = simple_strtoul(argv[1], NULL, 16);

    switch (mode)
    {
        #if defined(CONFIG_TITANIA8) || defined(CONFIG_TITANIA9) || defined(CONFIG_JANUS2)

        case 0:
      nandclk = NFIE_CLK_5_4M;
      shownandclk = 5;
            break;
        case 1:
      nandclk = NFIE_CLK_27M;
      shownandclk = 27;
            break;
        case 2:
      nandclk=NFIE_CLK_32M;
      shownandclk=32;
            break;
        case 3:
      nandclk = NFIE_CLK_43M;
      shownandclk = 43;
      break;

        #elif defined(CONFIG_TITANIA12) || defined(CONFIG_TITANIA13) || defined(CONFIG_AMBER1) || defined(CONFIG_AMBER6) || defined(CONFIG_AMBER7)


    case 0:
      nandclk = NFIE_CLK_5_4M;
      shownandclk = 5;
      break;
    case 1:
      nandclk=NFIE_CLK_27M;
      shownandclk=27;
            break;
    case 2:
      nandclk = NFIE_CLK_32M;
      shownandclk = 32;
      break;
    case 3:
      nandclk = NFIE_CLK_36M;
      shownandclk = 36;
      break;
        case 4:
      nandclk = NFIE_CLK_40M;
      shownandclk = 40;
            break;
        case 5:
      nandclk = NFIE_CLK_43M;
      shownandclk = 43;
      break;

        #if defined(CONFIG_AMBER1) || defined(CONFIG_AMBER6) || defined(CONFIG_AMBER7)


        case 6:
            nandclk = NFIE_CLK_54M;
            shownandclk = 54;
            break;
        case 7:
            nandclk = NFIE_CLK_62M;
            shownandclk = 62;
            break;
        case 8:
            nandclk = NFIE_CLK_72M;
            shownandclk = 72;
            break;
        case 9:
            nandclk = NFIE_CLK_80M;
            shownandclk = 80;
            break;
        case 10:
            nandclk = NFIE_CLK_86M;
            shownandclk = 86;
            break;
        #endif

        #elif defined(CONFIG_AMBER5)


        case 0:
            nandclk = NFIE_CLK_300K;
            shownandclk = 300;
            break;
        case 1:
            nandclk = NFIE_CLK_20M;
            shownandclk = 20;
            break;
        case 2:
            nandclk = NFIE_CLK_24M;
            shownandclk = 24;
            break;
        case 3:
            nandclk=NFIE_CLK_27M;
            shownandclk=27;
            break;
        case 4:
            nandclk = NFIE_CLK_32M;
            shownandclk = 32;
            break;
        case 5:
            nandclk = NFIE_CLK_36M;
            shownandclk = 36;
            break;
        case 6:
            nandclk = NFIE_CLK_40M;
            shownandclk = 40;
            break;
        case 7:
            nandclk = NFIE_CLK_43M;
            shownandclk = 43;
            break;
        case 8:
            nandclk = NFIE_CLK_48M;
            shownandclk = 48;
            break;
        case 9:
            nandclk = NFIE_CLK_54M;
            shownandclk = 54;
            break;
        case 10:
            nandclk = NFIE_CLK_62M;
            shownandclk = 62;
            break;
        case 11:
            nandclk = NFIE_CLK_72M;
            shownandclk = 72;
            break;
        case 12:
            nandclk = NFIE_CLK_80M;
            shownandclk = 80;
            break;
        case 13:
            nandclk = NFIE_CLK_86M;
            shownandclk = 86;
            break;
        #elif defined(CONFIG_AMBER2)
        case 0:
            nandclk = NFIE_CLK_300K;
            shownandclk = 300;
            break;
        case 1:
            nandclk = NFIE_CLK_20M;
            shownandclk = 20;
            break;
        case 2:
            nandclk = NFIE_CLK_24M;
            shownandclk = 24;
            break;
        case 3:
            nandclk=NFIE_CLK_27M;
            shownandclk=27;
            break;
        case 4:
            nandclk = NFIE_CLK_32M;
            shownandclk = 32;
            break;
        case 5:
            nandclk = NFIE_CLK_36M;
            shownandclk = 36;
            break;
        case 6:
            nandclk = NFIE_CLK_40M;
            shownandclk = 40;
            break;
        case 7:
            nandclk = NFIE_CLK_43M;
            shownandclk = 43;
            break;
        case 8:
            nandclk = NFIE_CLK_48M;
            shownandclk = 48;
            break;
        case 9:
            nandclk = NFIE_CLK_54M;
            shownandclk = 54;
            break;
        case 10:
            nandclk = NFIE_CLK_62M;
            shownandclk = 62;
            break;
        case 11:
            nandclk = NFIE_CLK_72M;
            shownandclk = 72;
            break;
        case 12:
            nandclk = NFIE_CLK_80M;
            shownandclk = 80;
            break;
        case 13:
            nandclk = NFIE_CLK_86M;
            shownandclk = 86;
            break;
        #elif  defined(CONFIG_AMETHYST)
        case 0:
            nandclk = NFIE_CLK_300K;
            shownandclk = 300;
            break;
        case 1:
            nandclk = NFIE_CLK_12M;
            shownandclk = 12;
            break;
        case 2:
            nandclk = NFIE_CLK_20M;
            shownandclk = 20;
            break;
        case 3:
            nandclk = NFIE_CLK_24M;
            shownandclk = 24;
            break;
        case 4:
            nandclk=NFIE_CLK_27M;
            shownandclk=27;
            break;
        case 5:
            nandclk = NFIE_CLK_32M;
            shownandclk = 32;
            break;
        case 6:
            nandclk = NFIE_CLK_36M;
            shownandclk = 36;
            break;
        case 7:
            nandclk = NFIE_CLK_40M;
            shownandclk = 40;
            break;
        case 8:
            nandclk = NFIE_CLK_43_2M;
            shownandclk = 43;
            break;
        case 9:
            nandclk = NFIE_CLK_48M;
            shownandclk = 48;
            break;
        case 10:
            nandclk = NFIE_CLK_54M;
            shownandclk = 54;
            break;
        case 11:
            nandclk = NFIE_CLK_62M;
            shownandclk = 62;
            break;
        case 12:
            nandclk = NFIE_CLK_72M;
            shownandclk = 72;
            break;
        case 13:
            nandclk = NFIE_CLK_80M;
            shownandclk = 80;
            break;
        case 14:
            nandclk = NFIE_CLK_86M;
            shownandclk = 86;
            break;
    #else

      printf("Please modify cmd.nand.c for new nfie clk\n");
    #endif

        default:
      printf("Please select correct clock!");
            return 1;
  }

    #if defined(CONFIG_TITANIA8) || defined(CONFIG_TITANIA9) || defined(CONFIG_JANUS2) || \
        defined(CONFIG_TITANIA12) || defined(CONFIG_TITANIA13) || defined(CONFIG_AMBER1) || \
        defined(CONFIG_AMBER6) || defined(CONFIG_AMBER7) || defined(CONFIG_AMBER5) || \
        defined(CONFIG_AMETHYST)
        printf ("NAND Clock mode :%d and Clock:%d MHz\n", mode, shownandclk);
    #else
    printf ("NAND Clock mode :%d and Clock:%d %cHz\n", mode, shownandclk, (mode<2) ? 'K' : 'M');
    #endif


  nand_clock_setting(nandclk);

    return 0;
}

/*
 * This function tag a number of blocks which located at the end of UBI partition
 * as bad block on purpose (rsv bad). So UBI will not use them temproarily.
 *
 * It can also erase bad block table (rsv good). So the tags setted above will be cleared.
 *
 * The funtion and corresponding commond are only uesd for nand programming
 * in mass production.
 */
int do_nand_write_rsv(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  //#if (CFG_CMD_JFFS2) && defined(CONFIG_JFFS2_CMDLINE)
  #ifdef CONFIG_JFFS2_CMDLINE

  struct mtd_device *dev=NULL;
  struct part_info *part=NULL;
       nand_erase_options_t erase_opts;
       nand_info_t *nand=NULL;
	   unsigned long blockrsv_num=0;
       unsigned long long rsvsize=0, rsvstart=0;
       u8 pnum=0;
       int i=0, ret=0;

       memset(&erase_opts, 0 , sizeof(erase_opts));
       nand = &nand_info[0];
       blockrsv_num = 8;  // if user dose not provide the parameter, use defaule value
        if (argc < 2 || ((2 == argc) && (0 != strcmp(argv[1], "good"))))
        {
            printf("Input error!Command format is: rsv bad [rsv_num] / rsv good\r\n");
            return -1;
        }

        if (0 == strcmp(argv[1], "bad"))
        {
            // find the offset and size of UBI partition
            if ((mtdparts_init() == 0) &&
                (find_dev_and_part("UBI", &dev, &pnum, &part) == 0)) {
                if (dev->id->type != MTD_DEV_TYPE_NAND) {
                  puts("not a NAND device\n");
                  return -1;
                }
            }
            if(part == NULL)
            {
                printf("%s: Error: null part, at %d\n", __func__, __LINE__);
                return -1;
            }
            if(dev == NULL)
            {
                printf("%s: Error: null dev, at %d\n", __func__, __LINE__);
                return -1;
            }

           /*
            * |-----------------------------------------|-----------------|
            * UBI start(offset)                                  reserved start     UBI end(offset + size)
            */
            if (3 == argc)
            {
                if (!str2long(argv[2], &blockrsv_num))
                {
                    return -1;
                }
            }
            rsvsize = blockrsv_num * nand->erasesize;
            rsvstart = part->offset + part->size - rsvsize;

            for (i = 0; i < blockrsv_num; i++)
            {
            #if 0
                memset(&erase_opts, 0, sizeof(erase_opts));
                nand->oobblock;
                erase_opts.length = nand->erasesize;
                erase_opts.offset = rsvstart + i * nand->erasesize;
                erase_opts.quiet = 0;
                erase_opts.jffs2  = 0;

                ret = nand->block_isbad(nand, write_opts.offset);

                if (ret < 0)
                {
                    printf("Bad block check failed\n");
                    return -1;
                }
                if (ret == 1)
                {
                    if (!write_opts.quiet)
                    {
                        printf("\rBad block at 0x%lx "
                               "in erase block from\n",
                               (long) (write_opts.offset));

                        continue;
                    }
                }
            #endif

                ret = nand->block_markbad(nand, (rsvstart + i * nand->erasesize));
                if (ret)
                {
                    printf("Can not mark %llu bad, error %d\r\n",
						     (unsigned long long)(erase_opts.offset), ret);
                    return -1;
                }

                // erase first
                //ret = nand_erase_opts(nand, &erase_opts);
               //printf("%4x %s\n",i,ret ? "ERROR" : "OK");

                //flush_cache((ulong)buf, write_opts.length);
                //ret = nand_write_opts(nand, &write_opts);
            }
        }

        if  (0 == strcmp(argv[1], "good"))
        {
            memset(&erase_opts, 0, sizeof(erase_opts));
            erase_opts.length = (ulong)(nand->bbt_block_num * nand->erasesize);
            erase_opts.offset = (ulong)(nand->size - nand->bbt_block_num * nand->erasesize);
            erase_opts.quiet = 0;
            erase_opts.jffs2  = 0;
            erase_opts.scrub = 1;

            // erase bad block table and the bbt will be recreated by MBoot
            nand_erase_opts(nand, &erase_opts);
        }

        return 0;
    #else
        return 0;
    #endif
}

#if defined(CONFIG_CMD_FAT)
/* read out ubi partition and store it to usb disk */
int do_ubi_partition_mkbin(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    struct mtd_device *dev=NULL;
    struct part_info *part=NULL;
    nand_info_t *nand=NULL;
    nand_read_options_t opts;
    u8 pnum=0;
    int ret=0, port=0;

    port = (int)simple_strtoul (argv[1], NULL, 16);
    switch(port){
        case 1:
            run_command("usb start 1", 0);
            break;
        case 2:
            run_command("usb start 2", 0);
            break;
        default:
            run_command("usb start", 0);
            break;
    }

#if defined(KL_IN_MTD)
    if ((mtdparts_init() == 0) &&
        (find_dev_and_part("KL", &dev, &pnum, &part) == 0)) {
        if (dev->id->type != MTD_DEV_TYPE_NAND) {
          puts("not a NAND device\n");
          return -1;
        }
    }
    if(part == NULL)
    {
        printf("%s: Error: null part, at %d\n", __func__, __LINE__);
        return -1;
    }
    if(dev == NULL)
    {
        printf("%s: Error: null dev, at %d\n", __func__, __LINE__);
        return -1;
    }
    nand = &nand_info[dev->id->num];

    if (part->size & (nand->erasesize - 1))
    {
        printf("KL partition size is not aligned with nand block size\r\n    part size=0x%llx, block size=0x%x\r\n",
                    part->size, nand->erasesize);

        return -1;
    }
    if (part->offset & (nand->erasesize - 1))
    {
        printf("KL partition offset is not aligned with nand block size\r\n    part offset=0x%x, block size=0x%x\r\n",
                    part->offset, nand->erasesize);

        return -1;
    }

    memset(&opts, 0, sizeof(opts));
    opts.buffer = (u_char*)NAND_UPGRADE_BUFFER_ADDR;
    opts.fatfile_name = "part0.bin";
    opts.length = (part->size / nand->oobblock) * (nand->oobblock + nand->oobsize);
    opts.offset = part->offset;
    opts.quiet      = 0;
    opts.readoob = 1;
    nand_fs_format = NAND_FS_FORMAT_MKBIN;

    printf("KL partition: \r\n    offset=0x%llx,\r\n    size=0x%llx(by main:%d), size=0x%x(by main+spare:%d),\r\n    off block=%d, size block=%d\r\n",
                part->offset, part->size, nand->oobblock, ((part->size / nand->oobblock) * (nand->oobblock + nand->oobsize)),
                (nand->oobblock + nand->oobsize), (part->offset / nand->erasesize), (part->size / nand->erasesize));

    ret = nand_read_opts(nand, &opts);
    flush_cache((ulong)opts.buffer, opts.length);
#endif

    if ((mtdparts_init() == 0) &&
        (find_dev_and_part("UBI", &dev, &pnum, &part) == 0)) {
        if (dev->id->type != MTD_DEV_TYPE_NAND) {
          puts("not a NAND device\n");
          return -1;
        }
    }
    nand = &nand_info[dev->id->num];

    if (part->size & (nand->erasesize - 1))
    {
        printf("UBI partition size is not aligned with nand block size\r\n    part size=0x%llx, block size=0x%x\r\n",
                    part->size, nand->erasesize);

        return -1;
    }
    if (part->offset & (nand->erasesize - 1))
    {
        printf("UBI partition offset is not aligned with nand block size\r\n    part offset=0x%llx, block size=0x%x\r\n",
                    part->offset, nand->erasesize);

        return -1;
    }

    memset(&opts, 0, sizeof(opts));
    opts.buffer = (u_char*)NAND_UPGRADE_BUFFER_ADDR;
    opts.fatfile_name = "part1.bin";
    opts.length = (part->size / nand->oobblock) * (nand->oobblock + nand->oobsize);
    opts.offset = part->offset;
    opts.quiet      = 0;
    opts.readoob = 1;
    nand_fs_format = NAND_FS_FORMAT_MKBIN;
    printf("UBI partition: \r\n    offset=0x%llx,\r\n    size=0x%llx(by main:%d), size=0x%llx(by main+spare:%d),\r\n    off block=%lld, size block=%lld\r\n",
                part->offset, part->size, nand->oobblock, ((part->size / nand->oobblock) * (nand->oobblock + nand->oobsize)),
                (nand->oobblock + nand->oobsize), (part->offset / nand->erasesize), (part->size / nand->erasesize));

    ret = nand_read_opts(nand, &opts);
    flush_cache((ulong)opts.buffer, opts.length);

    return ret;
}

/* read out ubi partition and environment parameter at once and store it to usb disk */
int do_nand_mkbin2(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int port = (int)simple_strtoul (argv[1], NULL, 16);
    switch(port){
        case 1:
            run_command("ubibin 1", 0);
            break;
        case 2:
            run_command("ubibin 2", 0);
            break;
        default:
            run_command("ubibin", 0);
            break;
    }

#if defined(CONFIG_ENV_IS_IN_NAND)
    run_command("envbin", 0);
#endif

    return 0;
}

/* print ubi partition and environment parameter information */
int do_print_nandbin_info(cmd_tbl_t *cmdtp, int flag, int argc,char * const argv[])
{
    struct mtd_device *dev=NULL;
    struct part_info *part=NULL;
    nand_info_t *nand=NULL;
    u8 pnum=0;
    u64 page_size=0;
    u64 off_blk=0, size_blk=0;
    u64 end=0, end_blk=0;
    u16 partnum=0;

    if ((mtdparts_init() == 0) &&
        ((find_dev_and_part("UBI", &dev, &pnum, &part) == 0) ||
        (find_dev_and_part("ubi", &dev, &pnum, &part) == 0))) {
        if (dev->id->type != MTD_DEV_TYPE_NAND) {
          puts("not a NAND device\n");
          return -1;
        }

        if(part == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }
    }
    nand = &nand_info[0];
    page_size = nand->oobblock + nand->oobsize;

    for(partnum = 0; partnum < dev->num_parts; partnum ++){
        printf("part#%d\n",partnum);
        part = mtd_part_info(dev, partnum);
        if(part == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }

        if (part->size & (nand->erasesize - 1))
        {
            printf("Warning: %s partition size is not aligned with nand block size\r\n    part size=0x%llx, block size=0x%x\r\n",
                        part->name, part->size, nand->erasesize);
        }

        if (part->offset & (nand->erasesize - 1))
        {
            printf("Warning: %s partition offset is not aligned with nand block size\r\n    part offset=0x%llx, block size=0x%x\r\n",
                        part->name, part->offset, nand->erasesize);
        }

        off_blk = part->offset / nand->erasesize;
        size_blk = part->size / nand->erasesize;
        end = part->offset + part->size - 1;
        end_blk = (part->offset + part->size) / nand->erasesize - 1;

        printf("%s partition: \r\n    offset=0x%llx(by main:%d),  offset=0x%llx(by main+spare:%lld)\r\n",
                    part->name, part->offset, nand->oobblock,
                    ((part->offset / nand->oobblock) * page_size), page_size);
        printf("    offset block=%lld(Hex:0x%llx)\r\n",
                    off_blk, off_blk);
        printf("    size=0x%llx(main),  size=0x%llx(main+spare)\r\n",
                    part->size, ((part->size / nand->oobblock) * page_size));
        printf("    size block=%lld(Hex:0x%llx)\r\n",
                    size_blk, size_blk);
        printf("    end=0x%llx(main),  end=0x%llx(main+spare)\r\n",
                    end, (((end + 1) / nand->oobblock) * page_size - 1));
        printf("    end block=%lld(Hex:0x%llx)\r\n\n",
                    end_blk, end_blk);
    }

    printf("Nand Flash basic information: \r\n    main=%d bytes(Hex:0x%x),  spare=%d bytes(Hex:0x%x)\r\n",
                nand->oobblock, nand->oobblock, nand->oobsize, nand->oobsize);
    printf("    main+spare=%lld bytes(Hex:0x%llx),  page number per block=%d\r\n",
                page_size, page_size, (nand->erasesize / nand->oobblock));
    printf("    block size=0x%x(%dKB, main),  block size=0x%llx(main+spare)\r\n\n",
                nand->erasesize, (nand->erasesize >> 10), ((nand->erasesize / nand->oobblock) * page_size));

    return 0;
}

/* write the length of size 0xff to the file pointed by name, append mode */
int write_empty_fatfile(char *name, ulong size)
{
    block_dev_desc_t *dev_desc=NULL;
    int dev=0;
    int part=1;
    char *ep=NULL;
    int i=0, fd=0;
    u_char *buf_end = (u_char *)NAND_UPGRADE_BUFFER_ADDR;

    /* Write to usb for binary file making
    Initialize something and open usb file */
    dev = (int)simple_strtoul ("0", &ep, 16);
    dev_desc=get_dev("usb", dev);
    if (dev_desc==NULL) {
      puts ("\n** Invalid boot device **\n");
      return -1;
    }
    if (*ep) {
      if (*ep != ':') {
        puts ("\n** Invalid boot device, use `dev[:part]' **\n");
        return -1;
      }
      part = (int)simple_strtoul(++ep, NULL, 16);
    }
    if (fat_register_device(dev_desc, part)!=0) {
        printf ("\n** Unable to use %s %d:%d for fatwrite **\n","usb",dev, part);
        return -1;
    }

    fd = file_fat_open(name, "a");
    if (fd < 0)
    {
        printf("Invalid file descriptor: %d\r\n", fd);
        return -1;
    }

    for (i = 0; i < READ_BUFFER_LEN; i++)
    {
        buf_end[i] = 0xff;
    }
    flush_cache((ulong)buf_end, READ_BUFFER_LEN);

    while (size >=  READ_BUFFER_LEN)
    {
        file_fat_write(fd, (unsigned char *) buf_end, READ_BUFFER_LEN);
        size = size - READ_BUFFER_LEN;
    }

    if (size > 0)
    {
        file_fat_write(fd, (unsigned char *) buf_end, size);
    }

    file_fat_close(fd);
#if 0 // can not enter, dead code
    if(fd == -1) {
        printf("\n** Unable to write \"%s\" from %s %d:%d **\n", name, "usb", dev, part);
       return -1;
    }
#endif

    return 0;
}

int dump_cisblock(nand_info_t *meminfo, const nand_read_options_t *opts)
{
    block_dev_desc_t *dev_desc=NULL;
    int dev=0;
    int part=1;
    char *ep=NULL, *cmdlinebase="nand read.cis", *cmdlinefull=NULL;
    int i=0, fd=0;
    u_char *buf_end = (u_char *)NAND_UPGRADE_BUFFER_ADDR, *buf_src=NULL, *buf_dst=NULL;

    /* Write to usb for binary file making
    Initialize something and open usb file */
    dev = (int)simple_strtoul ("0", &ep, 16);
    dev_desc=get_dev("usb", dev);
    if (dev_desc==NULL) {
      puts ("\n** Invalid boot device **\n");
      return -1;
    }
    if (*ep) {
      if (*ep != ':') {
        puts ("\n** Invalid boot device, use `dev[:part]' **\n");
        return -1;
      }
      part = (int)simple_strtoul(++ep, NULL, 16);
    }
    if (fat_register_device(dev_desc, part)!=0) {
        printf ("\n** Unable to use %s %d:%d for fatwrite **\n","usb",dev, part);
        return -1;
    }

    fd = file_fat_open(opts->fatfile_name, "a");
    if (fd < 0)
    {
        printf("Invalid file descriptor: %d\r\n", fd);
        return -1;
    }
    cmdlinefull = (char *) malloc(100);
    sprintf(cmdlinefull, "%s %p %llx", cmdlinebase, opts->buffer, opts->offset);

    run_command(cmdlinefull, 0);
    free(cmdlinefull);

    if(opts->readoob)
        file_fat_write(fd, (unsigned char *) opts->buffer, opts->length);
    else{
        buf_src = opts->buffer;
        buf_dst = buf_end;
        for(i = 0; i < meminfo->erasesize / meminfo->writesize; i++){
            memcpy(buf_dst, buf_src, meminfo->writesize);
            buf_dst += meminfo->writesize;
            buf_src += (meminfo->writesize + meminfo->oobsize);
        }
        file_fat_write(fd, (unsigned char *) buf_end, meminfo->erasesize);
    }

    file_fat_close(fd);
#if 0 // can not enter, dead code
    if(fd == -1) {
        printf("\n** Unable to write \"%s\" from %s %d:%d **\n", name, "usb", dev, part);
       return -1;
    }
#endif

    return 0;

}

int dump_cis_partition(nand_info_t *meminfo, nand_read_options_t *opts, int pad)
{
        int cissize=0, ret=0, i=0, pagelen=0, cisblocknum=0;
        u64 dumplen=0;
        loff_t offs=0;
        struct part_info *part=NULL;
        struct mtd_device *dev=NULL;
        u8 pnum=0;

        if(opts->readoob)
            pagelen = meminfo->oobblock + meminfo->oobsize;
        else
            pagelen = meminfo->oobblock;

        cissize = 2 * (meminfo->erasesize / meminfo->oobblock) * pagelen;

        for(i = 0; i < 10; i++){
            ret = meminfo->block_isbad(meminfo, offs);
            if(2 == ret){
                opts->offset = offs;
                opts->length = (meminfo->erasesize / meminfo->oobblock) * pagelen;
                memset(opts->buffer, 0xff, opts->length);
                ret = dump_cisblock(meminfo, opts);
                flush_cache((ulong)opts->buffer, opts->length);
                if(ret)
                    return ret;
                cisblocknum++;
                if((cisblocknum >= 2) && (pad == 0))
                    return 0;
             }

             offs += meminfo->erasesize;
        }

        if ((mtdparts_init() == 0) &&
            ((find_dev_and_part("CIS", &dev, &pnum, &part) == 0) ||
            (find_dev_and_part("cis", &dev, &pnum, &part) == 0))) {
            if (dev->id->type != MTD_DEV_TYPE_NAND) {
                puts("not a NAND device\n");
                return -1;
            }
        }

        if(part == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }

        dumplen = (part->size / meminfo->oobblock) * pagelen - cissize;
        ret = write_empty_fatfile(opts->fatfile_name, (ulong)dumplen);
        if(ret)
            return ret;

        return 0;
}

int restore_cisblock(nand_info_t *meminfo, const nand_write_options_t *opts)
{
    block_dev_desc_t *dev_desc=NULL;
    int dev=0;
    int part=1;
    char *ep=NULL, *cmdlinebase="ncisbl", *cmdlinefull=NULL;
    int i=0, fd=0, ret=0;
    u_char *buf_cis = (u_char *)NAND_UPGRADE_BUFFER_ADDR;
    u_char *buf_src=NULL, *buf_dst=NULL, *buf_ppm=NULL;

    /* Write to usb for binary file making
    Initialize something and open usb file */
    dev = (int)simple_strtoul ("0", &ep, 16);
    dev_desc=get_dev("usb", dev);
    if (dev_desc==NULL) {
        puts ("\n** Invalid boot device **\n");
        return -1;
    }
    if (*ep) {
      if (*ep != ':') {
        puts ("\n** Invalid boot device, use `dev[:part]' **\n");
        return -1;
      }
      part = (int)simple_strtoul(++ep, NULL, 16);
    }
    if (fat_register_device(dev_desc, part)!=0) {
        printf ("\n** Unable to use %s %d:%d for fatwrite **\n","usb",dev, part);
        return -1;
    }

    fd = file_fat_open(opts->fatfile_name, "a");
    if (fd < 0)
    {
        printf("Invalid file descriptor: %d\r\n", fd);
        return -1;
    }

    ret = file_fat_part_read (opts->fatfile_name, (unsigned char *)(opts->buffer),
                (unsigned long) opts->offset, (unsigned long) opts->length);

    if(ret == -1) {
        printf("\n** Unable to read \"%s\" from %s %d:%d **\n", opts->fatfile_name, "usb", dev, part);
        return -1;
    }

    if(opts->writeoob){
        buf_src = opts->buffer;
        buf_dst = buf_cis;
        for(i = 0; i < meminfo->erasesize / meminfo->writesize; i++){
            memcpy(buf_dst, buf_src, meminfo->writesize);
            buf_dst += meminfo->writesize;
            buf_src += (meminfo->writesize + meminfo->oobsize);
        }
    }else{
        buf_cis = opts->buffer;
        }

    cmdlinefull = (char *) malloc(100);

    //align to ppm address
    buf_ppm = (u_char *) (buf_cis + buf_cis[96] * meminfo->writesize - buf_cis[97] * 0x800);
    sprintf(cmdlinefull, "%s %p %p", cmdlinebase, buf_cis, buf_ppm);
    ret = run_command(cmdlinefull, 0);

    free(cmdlinefull);
    file_fat_close(fd);

    if(ret < 0){
        printf("Cannot write cis block!\n");
        return -1;
    }

#if 0 // can not enter, dead code
    if(fd == -1) {
        printf("\n** Unable to write \"%s\" from %s %d:%d **\n", name, "usb", dev, part);
       return -1;
    }
#endif

    return 0;

}

/* read out the contents of nand and store it to usb disk */
int do_nand_mkbin(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    struct mtd_device *dev=NULL;
    struct part_info *part=NULL;
    nand_info_t *nand=NULL;
    nand_read_options_t opts;
    u8 pnum=0;
    u64 dumplen=0, dumpoffset=0, badblocksize=0;
    int ret=0, port=0, readoob=0,
		pagelen=0, lpagelen = 0, dumpct = 0;
	u32 phyoobsize;
    char *partname=NULL, *binname = NAND_BIN_NAME;
    u16 partnum=0;

    nand = &nand_info[0];

    if(argc > 1)
        port = (int)simple_strtoul (argv[1], NULL, 16);

    switch(port){
        case 1:
            run_command("usb start 1", 0);
            break;
        case 2:
            run_command("usb start 2", 0);
            break;
        default:
            run_command("usb start", 0);
            break;
    }

    if(argc > 2)
        readoob = (int)simple_strtoul (argv[2], NULL, 16);

    if(argc > 5)
        binname = argv[5];

    phyoobsize = nand->oobsize;
    if(argc > 3)
        phyoobsize = (u32)simple_strtoul(argv[3], NULL, 16);

    if(readoob)
    {
        pagelen = nand->oobblock + phyoobsize;
		lpagelen = nand->oobblock + nand->oobsize;
    }
    else
        pagelen = nand->oobblock;

    if(argc > 6){
        partname = argv[6];

        if ((mtdparts_init() == 0) &&
            (find_dev_and_part(partname, &dev, &pnum, &part) == 0)) {
            if (dev->id->type != MTD_DEV_TYPE_NAND) {
              puts("not a NAND device\n");
              return -1;
            }
        }

        if(part == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }

        dumplen = (part->size/ nand->oobblock) * pagelen;
        dumpoffset = (part->offset/ nand->oobblock) * pagelen;
    }else{
        dumplen = (nand->size / nand->oobblock) * pagelen;
        dumpoffset = 0;
    }

	if(argc > 4){
		dumpct = (int)simple_strtoul (argv[4], NULL, 16);
	}

    memset(&opts, 0, sizeof(opts));
    opts.buffer = (u_char*) NAND_UPGRADE_BUFFER_ADDR;
    opts.fatfile_name = binname;
//    opts.length = NAND_BUFFER_LEN;
    opts.offset = 0;
    opts.quiet = 0;
    opts.readoob = readoob;
    opts.bbtbadblock = 0;
    opts.badblockcnt = 0;
	opts.phyoobsize = phyoobsize;

    if(readoob)
        printf("\n Read 0x%llx bytes from nand to %s with oob data\n", dumplen, opts.fatfile_name);
    else
        printf("\n Read 0x%llx bytes from nand to %s only main data\n", dumplen, opts.fatfile_name);

    //write totall part
    if(!partname){
        if ((mtdparts_init() == 0) &&
            ((find_dev_and_part("ubi", &dev, &pnum, &part) == 0) ||
            (find_dev_and_part("UBI", &dev, &pnum, &part) == 0))) {
            if (dev->id->type != MTD_DEV_TYPE_NAND) {
              puts("not a NAND device\n");
              return -1;
            }
        }

        if(part == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }

        for(partnum = 0; partnum < dev->num_parts; partnum ++){
            part = mtd_part_info(dev, partnum);
            if(part == NULL)
            {
                printf("%s: Error: null part, at %d\n", __func__, __LINE__);
                return -1;
            }

            if((0 == strcmp(part->name, "CIS")) || (0 == strcmp(part->name, "cis"))){
                ret = dump_cis_partition(nand, &opts, 1);
                if(ret)
                    return -1;
                printf("\n***Dump cis partition success!***\n");
                continue;
            }

			if (dumpct == 0)
			{
                if((0 == strcmp(part->name, "CTRL")) || (0 == strcmp(part->name, "ctrl"))){
                    printf("\n***Skip dump ctrl partition!***\n");
                    continue;
                }
		        if((0 == strcmp(part->name, "TBL")) || (0 == strcmp(part->name, "tbl"))){
                    printf("\n***Skip dump tbl partition!***\n");
                    continue;
                }
			}
			if((0 == strcmp(part->name, "reserved"))){
                continue;
            }

            opts.emptyskip = 0;
			if ((0 == strcmp(part->name, "UBI")) ||
				(0 == strcmp(part->name, "ubi")) ||
				(0 == strcmp(part->name, "UBIRO")) ||
				(0 == strcmp(part->name, "ubiro")))
			{
			    opts.emptyskip = 1;
			}

            dumpoffset = (part->offset/ nand->oobblock) * pagelen;
            dumplen = (part->size / nand->oobblock) * pagelen;
            opts.length = (dumplen > NAND_BUFFER_LEN(pagelen, 0x200))
                                    ? NAND_BUFFER_LEN(pagelen, 0x200) : dumplen;
            opts.badblockcnt = 0;
            badblocksize = 0;
            printf("\n***Start to write partition %s at offset: 0x%llx, length: 0x%llx***\n",
                    part->name, part->offset, part->size);

            nand_fs_format = NAND_FS_FORMAT_MKBIN;
            for(opts.offset = dumpoffset; opts.offset < dumpoffset + dumplen;){
                memset(opts.buffer, 0xff, opts.length);
                //printf("opts.offset=0x%llx, opts.length=0x%llx\n",
                //    opts.offset,opts.length);
                // convert to logical length for MLC
                if (readoob)
                {
                    opts.offset = (opts.offset / pagelen) * lpagelen;
				    opts.length = (opts.length / pagelen) * lpagelen;
                }
                ret = nand_read_opts(nand, &opts);
                flush_cache((ulong)opts.buffer, opts.length);
				// convert to physical length for MLC
                if (readoob)
                {
                    opts.offset = (opts.offset / lpagelen) * pagelen;
				    opts.length = (opts.length / lpagelen) * pagelen;
                }
                if(ret)
                    return ret;

                opts.offset += opts.length;
                badblocksize = (opts.badblockcnt * nand->erasesize / nand->oobblock) * pagelen;
                if(opts.offset + opts.length > dumpoffset + dumplen)
                    opts.length = dumpoffset + dumplen - opts.offset;
            }

            //fill the tail 0xff with bad block size
            if(badblocksize &&
			   !(((0 == strcmp(part->name, "UBI")) || (0 == strcmp(part->name, "ubi"))) && (dumpct == 0))){
                printf("Fill the partition tail with 0xff due to bad block!\n");
                ret = write_empty_fatfile(opts.fatfile_name, (ulong)badblocksize);
                if(ret)
                    return ret;
            }
            printf("***Finish partition %s write, skip bad block number 0x%x***\n",
                    part->name, opts.badblockcnt);
        }

        return 0;
    }

    //dump cis partition going to here
    if((0 == strcmp(partname, "CIS")) || (0 == strcmp(partname, "cis"))){
        ret = dump_cis_partition(nand, &opts, 0);
        if(0 == ret)
            return 0;
        else
            return -1;
    }

	if (dumpct == 0)
	{
		if((0 == strcmp(part->name, "CTRL")) || (0 == strcmp(part->name, "ctrl"))){
			printf("\n***Skip dump ctrl partition!***\n");
			return 0;
		}
		if((0 == strcmp(part->name, "TBL")) || (0 == strcmp(part->name, "tbl"))){
			printf("\n***Skip dump tbl partition!***\n");
			return 0;
		}
	}
	if((0 == strcmp(part->name, "reserved"))){
		return 0;
	}

	opts.emptyskip = 0;
	if ((0 == strcmp(part->name, "UBI")) ||
		(0 == strcmp(part->name, "ubi")) ||
		(0 == strcmp(part->name, "UBIRO")) ||
		(0 == strcmp(part->name, "ubiro")))
	{
		opts.emptyskip = 1;
	}

    //dump single partition (except cis) going to here
    opts.length = (dumplen > NAND_BUFFER_LEN(pagelen, 0x200))
                            ? NAND_BUFFER_LEN(pagelen, 0x200) : dumplen;
    printf("\n***Start to write partition %s at offset: 0x%llx, length: 0x%llx***\n",
                part->name, part->offset, part->size);
    nand_fs_format = NAND_FS_FORMAT_MKBIN;
    for(opts.offset = dumpoffset; opts.offset < dumplen + dumpoffset;){
        memset(opts.buffer, 0xff, opts.length);
		// convert to physical length for MLC
        if (readoob)
        {
            opts.offset = (opts.offset / pagelen) * lpagelen;
		    opts.length = (opts.length / pagelen) * lpagelen;
        }
        ret = nand_read_opts(nand, &opts);
        flush_cache((ulong)opts.buffer, opts.length);
		// convert to physical length for MLC
		if (readoob)
		{
			opts.offset = (opts.offset / lpagelen) * pagelen;
			opts.length = (opts.length / lpagelen) * pagelen;
		}
        if(ret)
            return ret;
        opts.offset += opts.length;
        badblocksize = (opts.badblockcnt * nand->erasesize / nand->oobblock) * pagelen;
        if(opts.offset + opts.length > dumplen + dumpoffset)
            opts.length = dumplen + dumpoffset - opts.offset;
    }

    printf("***Finish partition %s write, skip bad block number 0x%x***\n",
                part->name, opts.badblockcnt);

    return ret;
}

/* read out the whole contents of nand and store it to usb disk */
int do_nand_mkbinall(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    struct mtd_device *dev=NULL;
    struct part_info *part=NULL;
    u16 partnum=0;
    int readoob=0, port=0, dumpct = 0;
	u32 phyoobsize = nand_info[0].oobsize;
    u8 pnum=0;
    char *cmdlinebase="nandbin", *cmdlinefull=NULL, *binname=NULL;

    if(argc > 1)
        port = (int)simple_strtoul (argv[1], NULL, 16);

    switch(port){
        case 1:
            run_command("usb start 1", 0);
            break;
        case 2:
            run_command("usb start 2", 0);
            break;
        default:
            run_command("usb start", 0);
            break;
    }

    if(argc > 2)
        readoob = (int)simple_strtoul (argv[2], NULL, 16);
	if(argc > 3)
        phyoobsize = (int)simple_strtoul (argv[3], NULL, 16);
    if(argc > 4)
        dumpct = (int)simple_strtoul (argv[4], NULL, 16);

    if ((mtdparts_init() == 0) &&
        ((find_dev_and_part("UBI", &dev, &pnum, &part) == 0) ||
        (find_dev_and_part("ubi", &dev, &pnum, &part) == 0))) {
        if (dev->id->type != MTD_DEV_TYPE_NAND) {
          puts("not a NAND device\n");
          return -1;
        }

        if(part == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }
    }

    cmdlinefull = (char *) malloc(100);
    binname = (char *) malloc(32);
    for(partnum = 0; partnum < dev->num_parts; partnum ++){
        printf("part#%d\n",partnum);
        part = mtd_part_info(dev, partnum);
        if(part == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            free(cmdlinefull);
            free(binname);
            return -1;
        }
        sprintf(binname, "%.4s%s", part->name, ".bin");
        sprintf(cmdlinefull, "%s %d %d %x %d %s %s",
			    cmdlinebase, port, readoob, phyoobsize, dumpct, binname, part->name);

        run_command(cmdlinefull, 0);
    }

    free(binname);
    free(cmdlinefull);
    return 0;
}

/* read ubi partition file and restore it to nand */
int do_ubi_partition_restore(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    struct mtd_device *dev=NULL;
    struct part_info *part=NULL;
    nand_info_t *nand=NULL;
    nand_write_options_t opts;
    u8 pnum=0;
    int ret=0, port=0;

    if ((mtdparts_init() == 0) &&
        (find_dev_and_part("UBI", &dev, &pnum, &part) == 0)) {
        if (dev->id->type != MTD_DEV_TYPE_NAND) {
          puts("not a NAND device\n");
          return -1;
        }
    }
    if(part == NULL)
    {
        printf("%s: Error: null part, at %d\n", __func__, __LINE__);
        return -1;
    }
    if(dev == NULL)
    {
        printf("%s: Error: null dev, at %d\n", __func__, __LINE__);
        return -1;
    }
    nand = &nand_info[dev->id->num];

    if (part->size & (nand->erasesize - 1))
    {
        printf("UBI partition size is not aligned with nand block size\r\n    part size=0x%llx, block size=0x%x\r\n",
                    part->size, nand->erasesize);

        return -1;
    }
    if (part->offset & (nand->erasesize - 1))
    {
        printf("UBI partition offset is not aligned with nand block size\r\n    part offset=0x%llx, block size=0x%x\r\n",
                    part->offset, nand->erasesize);

        return -1;
    }

    memset(&opts, 0, sizeof(opts));
    opts.buffer = (u_char*)NAND_UPGRADE_BUFFER_ADDR;
    opts.fatfile_name = "part1.bin";
    opts.length = part->size;
    opts.offset = part->offset;
    opts.blockalign = 1;
    opts.quiet      = 0;
    opts.pad  = 1;
    nand_fs_format = NAND_FS_FORMAT_MKBIN;

    printf("UBI partition: \r\n    offset=0x%llx,\r\n    size=0x%llx(by main:%d), size=0x%llx(by main+spare:%d),\r\n    off block=%lld, size block=%lld\r\n",
                part->offset, part->size, nand->oobblock, ((part->size / nand->oobblock) * (nand->oobblock + nand->oobsize)),
                (nand->oobblock + nand->oobsize), (part->offset / nand->erasesize), (part->size / nand->erasesize));

    port = (int)simple_strtoul (argv[1], NULL, 16);
    switch(port){
        case 1:
            run_command("usb start 1", 0);
            break;
        case 2:
            run_command("usb start 2", 0);
            break;
        default:
            run_command("usb start", 0);
            break;
    }
    flush_cache((ulong)opts.buffer, opts.length);
    ret = nand_write_opts(nand, &opts);

    return ret;
}

/* read nand bin file and restore it to nand */
int do_nand_bin_restore(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    struct mtd_device *nanddev=NULL;
    struct part_info *nandpart=NULL;
    nand_info_t *nand=NULL;
    nand_write_options_t opts;
    int ret=0, port=0, pagelen=0, writeoob=0, cisblockcnt;
    u64 nandsize_oob=0, writeoffset=0, _writeoffset=0, badblocksize=0, partsize=0, writesize=0;
    u8 pnum=0;
    u16 partnum=0;
    ulong imagesize=0;
    char *partname = NULL, *binname = NAND_BIN_NAME;

    nand = &nand_info[0];

    nand_fs_format = NAND_FS_FORMAT_MKBIN;

    if(argc > 1)
        port = (int)simple_strtoul (argv[1], NULL, 16);

    switch(port){
        case 1:
            run_command("usb start 1", 0);
            break;
        case 2:
            run_command("usb start 2", 0);
            break;
        default:
            run_command("usb start", 0);
        break;
    }

    if(argc > 2)
        writeoob = (int)simple_strtoul (argv[2], NULL, 16);

    if(writeoob)
        pagelen = nand->oobblock + nand->oobsize;
    else
        pagelen = nand->oobblock;

    if(argc > 3)
        binname = argv[3];

    if(argc > 4){
        partname = argv[4];

        if ((mtdparts_init() == 0) &&
            (find_dev_and_part(partname, &nanddev, &pnum, &nandpart) == 0)) {
            if (nanddev->id->type != MTD_DEV_TYPE_NAND) {
              puts("not a NAND device\n");
              return -1;
            }
        }

        if(nandpart == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }

        writeoffset = (nandpart->offset/ nand->oobblock) * pagelen;
        writesize = (nandpart->size / nand->oobblock) * pagelen;
    }else{
        writeoffset = 0;
    }

    if(!partname)
        run_command("nand erase.chip", 0);
    else{
        char *cmdname = (char *) malloc(100);
        sprintf(cmdname, "nand erase.part %s", partname);
        run_command(cmdname, 0);
        free(cmdname);
    }

#if defined(CONFIG_CMD_FAT)
    block_dev_desc_t *dev_desc=NULL;
    int dev=0;
    int part=1;
    char *ep;
    long lPartno = 0;
    char * cPartno = getenv("partno");

    if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
    {
        if(cPartno == NULL) {
            part = 1;
            lPartno = 1;
        }
        else {
            lPartno = simple_strtol(cPartno, NULL, 10);
            part = lPartno;
            if (part == 0)//env haven't set
                part = 1;
        }

        dev = (int)simple_strtoul ("0", &ep, 16);
        dev_desc=get_dev("usb",dev);
        if (dev_desc==NULL) {
            puts ("\n** Invalid boot device **\n");
            return -1;
        }
        if (*ep) {
            if (*ep != ':') {
                puts ("\n** Invalid boot device, use `dev[:part]' **\n");
                return -1;
            }
            ++ep;
            part = (int)simple_strtoul(ep, NULL, 16);
        }
        if (fat_register_device(dev_desc, part)!=0) {
            printf ("\n** Unable to use %s %d:%d for fatload **\n", "usb", dev, part);
            return -1;
        }
    }
#endif

    memset(&opts, 0, sizeof(opts));
    opts.buffer = (u_char*) NAND_UPGRADE_BUFFER_ADDR;
//    opts.length = NAND_BUFFER_LEN;
    opts.blockalign = 1;
    opts.quiet      = 0;
    opts.bbtbadblock = 0;
    opts.offset = 0;
    opts.fatfile_name = binname;
    opts.writeoob = writeoob;
    opts.badblockcnt = 0;
//    opts.pad  = 1;
//    opts.noecc = 1;
//    opts.autoplace = 1;

    imagesize = (ulong) file_fat_filesize(opts.fatfile_name);
    nandsize_oob = (nand->size / nand->oobblock) * pagelen;

    if(imagesize > (nandsize_oob - writeoffset)){
        printf("\n Error: image too big, check the nand size and the image file name!\n");
        return -1;
    }

    if(opts.writeoob)
        printf("\n Read 0x%lx bytes from %s (contains oob data) to nand\n", imagesize, opts.fatfile_name);
    else
        printf("\n Read 0x%lx bytes from %s (only main data) to nand\n", imagesize, opts.fatfile_name);

    printf("NOTE: Be make sure that it has the same mtd partition info (use mtdparts command to check)!\n");

    //write cis block
    if(0 == writeoffset){
        opts.length = (nand->erasesize / nand->oobblock) * pagelen;
        for(cisblockcnt = 0; cisblockcnt < 10; cisblockcnt ++){
            opts.offset = cisblockcnt * opts.length;
            memset(opts.buffer, 0xff, opts.length);
            ret = restore_cisblock(nand, &opts);
            flush_cache((ulong)opts.buffer, opts.length);
            if(0 == ret)
                break;
        }

        if(ret){
            printf("ERR: Cannot get cis data in the image!\n");
            return -1;
        }

        if((0 == strcmp(partname, "CIS")) || (0 == strcmp(partname, "cis")))
            return 0;
    }

    nand_fs_format = NAND_FS_FORMAT_MKBIN;

    //write other part
    if(!partname){
        if ((mtdparts_init() == 0) &&
            ((find_dev_and_part("ubi", &nanddev, &pnum, &nandpart) == 0) ||
            (find_dev_and_part("UBI", &nanddev, &pnum, &nandpart) == 0))) {
            if (nanddev->id->type != MTD_DEV_TYPE_NAND) {
              puts("not a NAND device\n");
              return -1;
            }
        }

        if(nandpart == NULL)
        {
            printf("%s: Error: null part, at %d\n", __func__, __LINE__);
            return -1;
        }

        for(partnum = 1; partnum < nanddev->num_parts; partnum ++){
            nandpart = mtd_part_info(nanddev, partnum);
            if(nandpart == NULL)
            {
                printf("%s: Error: null part, at %d\n", __func__, __LINE__);
                return -1;
            }
            writeoffset = (nandpart->offset/ nand->oobblock) * pagelen;
            partsize = (nandpart->size / nand->oobblock) * pagelen;
            partsize = (imagesize < partsize) ? imagesize : partsize;
            opts.length = (partsize > NAND_BUFFER_LEN(pagelen, 0x200))
                                    ? NAND_BUFFER_LEN(pagelen, 0x200) : partsize;
            opts.badblockcnt = 0;
            badblocksize = 0;
            printf("\n***Start to write partition %s at offset: 0x%llx, length: 0x%llx***\n",
                    nandpart->name, nandpart->offset, nandpart->size);

            for(opts.offset = writeoffset; opts.offset < writeoffset + partsize - badblocksize;){
                memset(opts.buffer, 0xff, opts.length);
                //printf("opts.offset=0x%llx, opts.length=0x%llx, writeoffset=0x%llx\n",
                //    opts.offset,opts.length,writeoffset);
                ret = _nand_write_opts(nand, &opts, badblocksize);
                flush_cache((ulong)opts.buffer, opts.length);
                if(ret)
                    return ret;
                opts.offset += opts.length;
                badblocksize = (opts.badblockcnt * nand->erasesize / nand->oobblock) * pagelen;
                if(opts.offset + opts.length > writeoffset + partsize - badblocksize)
                    opts.length = writeoffset + partsize - badblocksize - opts.offset;
            }
            printf("***Finish partition %s write, skip bad block number 0x%x***\n",
                    nandpart->name, opts.badblockcnt);
        }

        return 0;
    }

    printf("\n***Start to write partition %s at offset: 0x%llx, length: 0x%llx***\n",
                    partname, nandpart->offset, nandpart->size);
    //restore single partition (except cis) going to here
    writesize = (writesize > imagesize) ? imagesize : writesize;
    opts.length = (writesize > NAND_BUFFER_LEN(pagelen, 0x200))
                        ? NAND_BUFFER_LEN(pagelen, 0x200) : writesize;
    _writeoffset = writeoffset;
    for(opts.offset = 0; opts.offset < writesize - badblocksize;){
        memset(opts.buffer, 0xff, opts.length);
        //printf("opts.offset=0x%llx, opts.length=0x%llx, writeoffset=0x%llx\n",
        //        opts.offset,opts.length,writeoffset);
        ret = _nand_write_opts(nand, &opts, _writeoffset);
        flush_cache((ulong)opts.buffer, opts.length);
        if(ret)
            return ret;
        opts.offset += opts.length;
        badblocksize = (opts.badblockcnt * nand->erasesize / nand->oobblock) * pagelen;
        _writeoffset = writeoffset + badblocksize;
        if(opts.offset + opts.length > writesize - badblocksize)
            opts.length = writesize - badblocksize - opts.offset;
    }
    printf("***Finish partition %s write, skip bad block number 0x%x***\n",
            partname, opts.badblockcnt);
    return 0;
}
#endif

#if 0
U_BOOT_CMD(nand, 5, 1, do_nand,
  "nand    - NAND sub-system\n",
  "info                  - show available NAND devices\n"
  "nand device [dev]     - show or set current device\n"
  "nand read[.jffs2]     - addr off|partition size\n"
  "nand write[.jffs2]    - addr off|partiton size - read/write `size' bytes starting\n"
  "    at offset `off' to/from memory address `addr'\n"
  "nand erase [clean] [off size] - erase `size' bytes from\n"
  "nand eloop [loop]- erase all for loop \n"
  "    offset `off' (entire device if not specified)\n"
  "nand bad - show bad blocks\n"
  "nand dump[.oob] off - dump page\n"
  "nand scrub - really clean NAND erasing bad blocks (UNSAFE)\n"
  "nand markbad off - mark bad block at offset (UNSAFE)\n"
  "nand biterr off - make a bit error at offset (UNSAFE)\n"
  "nand lock [tight] [status] - bring nand to lock state or display locked pages\n"
  "nand unlock [offset] [size] - unlock section\n"
  "nand bist [forever] - BIST test\n"
  "nand stable - Stable test\n"
);
#endif
U_BOOT_CMD(nboot, 5, 1, do_nandboot,
  "nboot   - boot from NAND device\n",
  "[partition] | [[[loadAddr] dev] offset]\n");

U_BOOT_CMD(nandclk, 5, 1, do_nandclk,
  "nandclk - tune nand clock\n",
  "nandclk mode      \n"
#if 0
    #if defined(CONFIG_TITANIA8) || defined(CONFIG_TITANIA9) || defined(CONFIG_JANUS2)
    "mode=0 NFIE_CLK_5_4M \n"
    "mode=1 NFIE_CLK_27M \n"
    "mode=2 NFIE_CLK_32M \n"
    "mode=3 NFIE_CLK_43M \n"
    #elif defined(CONFIG_TITANIA12) || defined(CONFIG_TITANIA13) || defined(CONFIG_AMBER1) || defined(CONFIG_AMBER1) || defined(CONFIG_AMBER6) || defined(CONFIG_AMBER7) || defined(CONFIG_AMBER5) || defined(CONFIG_AMBER2) || defined(CONFIG_AMETHYST)
  "mode=0 NFIE_CLK_5_4M \n"
    "mode=1 NFIE_CLK_27M \n"
    "mode=2 NFIE_CLK_32M \n"
    "mode=3 NFIE_CLK_36M \n"
    "mode=4 NFIE_CLK_40M \n"
    "mode=5 NFIE_CLK_43M \n"
    #else
  "mode=0 NFIE_CLK_300K \n"
    "mode=1 NFIE_CLK_750K \n"
    "mode=2 NFIE_CLK_5_4M \n"
    "mode=3 NFIE_CLK_13M \n"
    "mode=4 NFIE_CLK_18M \n"
    "mode=5 NFIE_CLK_22M \n"
    "mode=6 NFIE_CLK_27M \n"
    "mode=7 NFIE_CLK_32M \n"
    "mode=8 NFIE_CLK_43M \n"
    #endif
#endif
);

U_BOOT_CMD(
    rsv,   3,  1,  do_nand_write_rsv,
    "rsv - Set reserved block as bad or good\n",
    "rsv bad [rsv_num] or rsv good, rsv_num refers to reserved block number\n"
);

#if defined(CONFIG_CMD_FAT)
U_BOOT_CMD(
  ubibin, 2, 1, do_ubi_partition_mkbin,
  "ubibin - read out ubi partition and store it to usb disk\n",
  "command: ubibin\n"
);

U_BOOT_CMD(
  nandbin2, 2, 1, do_nand_mkbin2,
  "nandbin2 - read out ubi partition and environment parameter at once and store it to usb disk\n",
  "command: nandbin2\n"
);

U_BOOT_CMD(
  nandbin, 7, 1,  do_nand_mkbin,
  "nandbin usbport withsparedata?1:0 oobsize dumpC/T?1:0 bin_name partition_name - read out the contents of nand and store it to usb disk\n",
  "command: nandbin usbport withsparedata?1:0 bin_name partition_name\n"
);

U_BOOT_CMD(
  nandbinall, 6, 1,  do_nand_mkbinall,
  "nandbinall usbport withsparedata?1:0 oobsize dumpC/T?1:0- read out the whole contents of nand and store it to usb disk\n",
  "command: nandbinall usbport withsparedata?1:0\n"
);

U_BOOT_CMD(
  ubi2nand, 2, 1, do_ubi_partition_restore,
  "ubi2nand - read ubi partition file and restore it to nand\n",
  "command: ubi2nand\n"
);

U_BOOT_CMD(
  bin2nand, 5, 1, do_nand_bin_restore,
  "bin2nand usbport withsparedata?1:0 binname partition_name - read binary files and restore it to nand\n",
  "command: bin2nand usbport withsparedata?1:0 binname partition_name\n"
);

U_BOOT_CMD(
  bininfo, 1, 1,  do_print_nandbin_info,
  "bininfo - print ubi partition and environment parameter information\n",
  "command: bininfo\n"
);
#endif

#if 0
#Mstar Start
#define READ_NAND_ID_LENGTH    5
int do_nand_readID( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    uchar buffer[READ_NAND_ID_LENGTH];
    uint len=0;

    if ((argc < 1) || (argc > 1))
    {
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }
    memset(buffer, 0, sizeof(buffer));

    len = READ_NAND_ID_LENGTH;
    drvNAND_Read_ID();

    printf("Manufacturer ID is:");
    printf("%4x\n",buffer[0]);
    printf("Device ID is:");
    printf("%4x",buffer[1]);
    printf("%4x\n",buffer[2]);
    return 0;
}

U_BOOT_CMD(
  nandid,    1,        1, do_nand_readID,
  "nand read id  - nand read ID\n",
  "command: nandid\n"
);

//Mstar End
#endif

#if 1
#include"../drivers/mstar/unfd/inc/common/drvNAND.h"

extern int nand_write_bootloader(U32 u32_Row,U8 * pu8_addr, U32 u32_size);
extern U32 drvNAND_WriteCIS_for_ROM(NAND_FLASH_INFO_t * pNandInfo);
extern int search_cis_in_DRAM(U8* pu8_CISAddr, U8* pu8_PPMAddr, U8* pu8_PNIAddr, NAND_FLASH_INFO_t *pNandInfoOut);
/*
 * argv[1] = cis address
 * argv[2] = PPM address
 * argv[3] = PNI address
 * argv[4] = bootloader address
 * argv[5] = bootloader size
 * argv[6] = uboot address
 * argv[7] = uboot size
 *
 * */
int do_nandupdateCIS_BL_UBOOT(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	long unsigned int u32_block_PBA = 0xA, u32_bl_PBA1, u32_bl_PBA2, u32_uboot_PBA0, u32_uboot_PBA1;
	long unsigned int u32_buf, u32_size, u32_Err = 0, u32_PPM_buf, u32_PNI_buf = 0, u32_tmp;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    NAND_DRIVER *pNandDrv_tmp = (NAND_DRIVER*)malloc(sizeof(NAND_DRIVER));

    memcpy(pNandDrv_tmp, pNandDrv, sizeof(NAND_DRIVER));

	NAND_FLASH_INFO_t * pNandInfo = (NAND_FLASH_INFO_t* )malloc(512);

	if(argc < 3)
	{
		printf("Too few arguements\n");
        return cmd_usage(cmdtp);
	}

	u32_buf = simple_strtoul(argv[1], NULL, 16);
    u32_PPM_buf = simple_strtoul(argv[2], NULL, 16);

	if(argc >= 4)
		u32_PNI_buf = simple_strtoul(argv[3], NULL, 16);

	u32_Err = search_cis_in_DRAM((U8*)u32_buf, (U8*)u32_PPM_buf, (U8*) u32_PNI_buf, pNandInfo);
	if(u32_Err != 0)
    {
        printf("search cis in DRAM fail\n");
        goto Error;
    }
	
	if(argc == 6)
	{
		while(drvNAND_IsGoodBlk(u32_block_PBA) == 0)
			u32_block_PBA ++;
		u32_bl_PBA1 = u32_block_PBA;
		
		u32_buf = simple_strtoul(argv[4], NULL, 16);
		u32_size = SBOOT_SIZE;
		
		printf("write bootloader @ block %lX\n", u32_bl_PBA1);
		nand_write_bootloader(u32_bl_PBA1<< pNandDrv->u8_BlkPageCntBits, (U8*) u32_buf, u32_size);
		
		u32_block_PBA = u32_bl_PBA1 + (u32_size >> (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_PageByteCntBits)) + 1;
		
		while(drvNAND_IsGoodBlk(u32_block_PBA) == 0)
			u32_block_PBA ++;
		u32_bl_PBA2 = u32_block_PBA;
		
		u32_buf = simple_strtoul(argv[4], NULL, 16);
		u32_size = SBOOT_SIZE;
		
		printf("write bootloader backup @ block %lX\n", u32_bl_PBA2);
		
		nand_write_bootloader(u32_bl_PBA2<< pNandDrv->u8_BlkPageCntBits, (U8*) u32_buf, u32_size);
		
		u32_block_PBA = u32_bl_PBA2 + 1;
		while(drvNAND_IsGoodBlk(u32_block_PBA) == 0)
			u32_block_PBA ++;
		u32_uboot_PBA0 = u32_block_PBA;
		
		u32_buf = simple_strtoul(argv[4], NULL, 16) + SBOOT_SIZE;
		u32_size = simple_strtoul(argv[5], NULL, 16) - SBOOT_SIZE;
		
		printf("write uboot @ block %lX\n", u32_uboot_PBA0);
		
		nand_write_bootloader(u32_uboot_PBA0 << pNandDrv->u8_BlkPageCntBits, (U8*) u32_buf, u32_size);
		
		u32_block_PBA = u32_uboot_PBA0 + 1;
		
		//calcuate write contain length
		
		u32_tmp = (u32_size >> (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_PageByteCntBits)) + 1;
		
		while(u32_tmp > 0)
		{
			if(drvNAND_IsGoodBlk(u32_block_PBA))
			{
				u32_block_PBA ++;
				u32_tmp --;
			}
		}
		
		u32_uboot_PBA1 = u32_block_PBA;
		
		u32_buf = simple_strtoul(argv[4], NULL, 16) + SBOOT_SIZE;
		u32_size = simple_strtoul(argv[5], NULL, 16) - SBOOT_SIZE;
		
		printf("write uboot backup @ block %lX\n", u32_uboot_PBA1);
		
		nand_write_bootloader(u32_uboot_PBA1 << pNandDrv->u8_BlkPageCntBits, (U8*) u32_buf, u32_size);
		
		pNandInfo->u8_BL0PBA = (U8)(u32_bl_PBA1 & 0xFF);
		pNandInfo->u8_BL1PBA = (U8)(u32_bl_PBA2 & 0xFF);
		pNandInfo->u8_UBOOTPBA = (U8)(u32_uboot_PBA1 & 0xFF);
		printf("%X, %X, %X\n", pNandInfo->u8_BL0PBA, pNandInfo->u8_BL1PBA, pNandInfo->u8_UBOOTPBA);
	}

	if(argc == 8)
	{
		while(drvNAND_IsGoodBlk(u32_block_PBA) == 0)
			u32_block_PBA ++;
		u32_bl_PBA1 = u32_block_PBA;
		
		u32_buf = simple_strtoul(argv[4], NULL, 16);
		u32_size = simple_strtoul(argv[5], NULL, 16);
		
		printf("write bootloader @ block %lX\n", u32_bl_PBA1);
		nand_write_bootloader(u32_bl_PBA1<< pNandDrv->u8_BlkPageCntBits, (U8*) u32_buf, u32_size);
		
		u32_block_PBA = u32_bl_PBA1 + (u32_size >> (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_PageByteCntBits)) + 1;
		
		while(drvNAND_IsGoodBlk(u32_block_PBA) == 0)
			u32_block_PBA ++;
		u32_bl_PBA2 = u32_block_PBA;
		
		u32_buf = simple_strtoul(argv[4], NULL, 16);
		u32_size = simple_strtoul(argv[5], NULL, 16);
		
		printf("write bootloader backup @ block %lX\n", u32_bl_PBA2);
		
		nand_write_bootloader(u32_bl_PBA2<< pNandDrv->u8_BlkPageCntBits, (U8*) u32_buf, u32_size);
		
		u32_block_PBA = u32_bl_PBA2 + 1;
		while(drvNAND_IsGoodBlk(u32_block_PBA) == 0)
			u32_block_PBA ++;
		u32_uboot_PBA0 = u32_block_PBA;
		
		u32_buf = simple_strtoul(argv[6], NULL, 16);
		u32_size = simple_strtoul(argv[7], NULL, 16);
		
		printf("write uboot @ block %lX\n", u32_uboot_PBA0);
		
		nand_write_bootloader(u32_uboot_PBA0 << pNandDrv->u8_BlkPageCntBits, (U8*) u32_buf, u32_size);
		
		u32_block_PBA = u32_uboot_PBA0 + 1;
		
		//calcuate write contain length
		
		u32_tmp = (u32_size >> (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_PageByteCntBits)) + 1;
		
		while(u32_tmp > 0)
		{
			if(drvNAND_IsGoodBlk(u32_block_PBA))
			{
				u32_block_PBA ++;
				u32_tmp --;
			}
		}
		
		u32_uboot_PBA1 = u32_block_PBA;
		
		u32_buf = simple_strtoul(argv[6], NULL, 16);
		u32_size = simple_strtoul(argv[7], NULL, 16);
		
		printf("write uboot backup @ block %lX\n", u32_uboot_PBA1);
		
		nand_write_bootloader(u32_uboot_PBA1 << pNandDrv->u8_BlkPageCntBits, (U8*) u32_buf, u32_size);
		
		pNandInfo->u8_BL0PBA = (U8)(u32_bl_PBA1 & 0xFF);
		pNandInfo->u8_BL1PBA = (U8)(u32_bl_PBA2 & 0xFF);
		pNandInfo->u8_UBOOTPBA = (U8)(u32_uboot_PBA1 & 0xFF);
		printf("%X, %X, %X\n", pNandInfo->u8_BL0PBA, pNandInfo->u8_BL1PBA, pNandInfo->u8_UBOOTPBA);
	}

	u32_Err = drvNAND_WriteCIS_for_ROM(pNandInfo);

    if(u32_Err != 0)
    {
        printf("Write CIS for ROM fail\n");
        goto Error;
    }
	// Reinit NAND Flash
	nand_init();

    return 0;

Error:
    memcpy(pNandDrv, pNandDrv_tmp, sizeof(NAND_DRIVER));
    NC_Init();
    NC_Config();
	return u32_Err;
}

U_BOOT_CMD(ncisbl, 10, 1, do_nandupdateCIS_BL_UBOOT,
	"write cis, bootloader and uboot or only CIS for ROM boot",
	"ncisbl CISAddr PPMAddr [PNIAddr] [bootloaderAddr] [bootloaderSize] [ubootAddr] [ubootSize]\n"
	"  -write CIS, PPM, PNI, bootloader, uboot from those memory address, CISaddr, PPMaddr, bootloaderAddr\n"
	"	and ubootAddr respectively with 'bootloaderSize' and 'ubootSize' bytes \n"
	"	assigned for bootloader and uboot programming."
);

int do_nand_update_CIS_HASH(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int u32_cis_buf = 0;
    unsigned int u32_ppm_buf = 0;
    unsigned int u32_hash_buf[3] = {0};
    unsigned int u32_block_hash[3] = {0};
    unsigned int u32_hash_size[3] = {0x2800, 0x2800, 0};
    unsigned int u32_block_PBA = 10;
	unsigned int u32_pni_buf = 0;
    unsigned int u32_Err = 0;

    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    NAND_DRIVER *pNandDrv_tmp = (NAND_DRIVER*)malloc(sizeof(NAND_DRIVER));
    NAND_FLASH_INFO_t *pNandInfo = (NAND_FLASH_INFO_t* )malloc(512);

    int i;

    if( argc < 7 )
    {
        printf("Too few arguements\n");
        return cmd_usage(cmdtp);
    }

    memcpy(pNandDrv_tmp, pNandDrv, sizeof(NAND_DRIVER));

    u32_cis_buf = simple_strtoul(argv[1], NULL, 16);
    u32_ppm_buf = simple_strtoul(argv[2], NULL, 16);

    for(i=0; i<3; i++)
    {
        u32_hash_buf[i] = simple_strtoul(argv[i+3], NULL, 16);
    }
    u32_hash_size[2] = simple_strtoul(argv[6], NULL, 16);

    u32_Err = search_cis_in_DRAM((U8*)u32_cis_buf, (U8*)u32_ppm_buf, (U8*) u32_pni_buf, pNandInfo);

	if(u32_Err != 0)
    {
        printf("search cis in DRAM fail\n");
        goto HASH_ERR;
    }

    for(i=0; i<3; i++)
    {
        //Search good block for hash[0-2]
        while(drvNAND_IsGoodBlk(u32_block_PBA) == 0)
            u32_block_PBA++;
        u32_block_hash[i] = u32_block_PBA;
        //Write hash[0-2]
        printf("write hash%d @ block %X\n", i, u32_block_hash[i]);
        nand_write_bootloader(u32_block_hash[i]<< pNandDrv->u8_BlkPageCntBits, (U8*)u32_hash_buf[i], u32_hash_size[i]);
        pNandInfo->u8_HashPBA[i][0] = u32_block_hash[i];

        //Search good block for hash[0-2] backup
        u32_block_PBA++;
        while(drvNAND_IsGoodBlk(u32_block_PBA) == 0)
            u32_block_PBA ++;
        u32_block_hash[i] = u32_block_PBA;
        //Write hash[0-2] backup
        printf("write hash%d backup @ block %X\n", i, u32_block_hash[i]);
        nand_write_bootloader(u32_block_hash[i]<< pNandDrv->u8_BlkPageCntBits, (U8*)u32_hash_buf[i], u32_hash_size[i]);
        pNandInfo->u8_HashPBA[i][1] = u32_block_hash[i];
        u32_block_PBA++;
    }

    u32_Err = drvNAND_WriteCIS_for_ROM(pNandInfo);

    if(u32_Err != 0)
    {
        printf("Update CIS for ROM fail\n");
        goto HASH_ERR;
    }

    // Reinit NAND Flash
	nand_init();

    free(pNandDrv_tmp);
    free(pNandInfo);

    return 0;

HASH_ERR:
    memcpy(pNandDrv, pNandDrv_tmp, sizeof(NAND_DRIVER));
    NC_Init();
    NC_Config();
    free(pNandDrv_tmp);
    free(pNandInfo);
    return u32_Err;
}

U_BOOT_CMD(ncishash, 7, 1, do_nand_update_CIS_HASH,
    "write cis and hash[0-2] for Secure ROM boot",
    "<CISAddr> <PPMAddr> <Hash0Addr> <Hash1Addr> <Hash2Addr> <Hash2Size>\n"
    "CISAddr   DRAM address of CIS\n"
    "PPMAddr   DRAM address of PPM\n"
    "Hash0Addr DRAM address of hash0 [Fixed size 10KB]\n"
    "Hash1Addr DRAM address of hash1 [Fixed size 10KB]\n"
    "Hash2Addr DRAM address of hash2\n"
    "Hash2Size Total size of the hash2\n"
);

extern U32 drvNAND_CheckSum(U8 *pu8_Data, U16 u16_ByteCnt);
int do_nandWriteCIS(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	NAND_FLASH_INFO_t * pNandInfo = (NAND_FLASH_INFO_t* )malloc(512);
	U32 u32_Err ;

	if(!pNandInfo)
	{
		printf("Memory Allocate fail for pNandInfo\n");
		return -1;
	}
	memset(pNandInfo, 0, 512);
	memcpy(pNandInfo->au8_Tag, "MSTARSEMIUNFDCIS", 16);
	pNandInfo->u8_IDByteCnt = pNandDrv->u8_IDByteCnt;
	memset(pNandInfo->au8_ID, 0, NAND_ID_BYTE_CNT);
	memcpy(pNandInfo->au8_ID, pNandDrv->au8_ID, pNandDrv->u8_IDByteCnt);
	pNandInfo->u16_SpareByteCnt = pNandDrv->u16_SpareByteCnt;
	pNandInfo->u16_PageByteCnt = pNandDrv->u16_PageByteCnt;
	pNandInfo->u16_BlkPageCnt = pNandDrv->u16_BlkPageCnt;
	pNandInfo->u16_BlkCnt = pNandDrv->u16_BlkCnt;
	pNandInfo->u32_Config =  ((pNandDrv->u8_PlaneCnt - 1) & 7) << 4 |((pNandDrv->u8_BadBlkMarker&7) << 1) |(pNandDrv->u8_CellType & 1);
	pNandInfo->u16_ECCType = pNandDrv->u16_ECCType;
	pNandInfo->u16_tRC          = pNandDrv->u16_tRC;
	pNandInfo->u8_tRP           = pNandDrv->u8_tRP;
	pNandInfo->u8_tREH          = pNandDrv->u8_tREH;
	pNandInfo->u8_tREA          = pNandDrv->u8_tREA;
	pNandInfo->u8_tRR           = pNandDrv->u8_tRR;
	pNandInfo->u16_tADL         = pNandDrv->u16_tADL;
	pNandInfo->u16_tRHW         = pNandDrv->u16_tRHW;
	pNandInfo->u16_tWHR         = pNandDrv->u16_tWHR;
	pNandInfo->u16_tCCS         = pNandDrv->u16_tCCS;
	pNandInfo->u8_tCS           = pNandDrv->u8_tCS;
	pNandInfo->u16_tWC          = pNandDrv->u16_tWC;
	pNandInfo->u8_tWP           = pNandDrv->u8_tWP;
	pNandInfo->u8_tWH           = pNandDrv->u8_tWH;
	pNandInfo->u16_tCWAW        = pNandDrv->u16_tCWAW;
	pNandInfo->u8_tCLHZ			= pNandDrv->u8_tCLHZ;
	pNandInfo->u8_AddrCycleIdx 	= pNandDrv->u8_AddrCycleIdx;
	pNandInfo->u16_tWW			= pNandDrv->u16_tWW;

	pNandInfo->u32_ChkSum 		= drvNAND_CheckSum((U8*) (pNandInfo) + 0x24, 0x32 - 0x24);

	memcpy(pNandInfo->u8_Vendor, pNandDrv->u8_Vendor, 16);
	memcpy(pNandInfo->u8_PartNumber, pNandInfo->u8_PartNumber, 16);

	#if defined(FCIE4_DDR) && FCIE4_DDR
	memcpy((void *) &pNandInfo->tDefaultDDR, (const void *) &pNandDrv->tDefaultDDR, sizeof(DDR_TIMING_GROUP_t));
	memcpy((void *) &pNandInfo->tMaxDDR, (const void *) &pNandDrv->tMaxDDR, sizeof(DDR_TIMING_GROUP_t));
	memcpy((void *) &pNandInfo->tMinDDR, (const void *) &pNandDrv->tMinDDR, sizeof(DDR_TIMING_GROUP_t));
	#endif

	u32_Err = drvNAND_WriteCIS_for_ROM(pNandInfo);
	return u32_Err;
}

U_BOOT_CMD(nwritecis, 7, 1, do_nandWriteCIS,
	"write cis for ROM or DDR NAND",
	""
);
#endif

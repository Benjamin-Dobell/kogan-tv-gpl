/*
 * drivers/mtd/nand/nand_util.c
 *
 * Copyright (C) 2006 by Weiss-Electronic GmbH.
 * All rights reserved.
 *
 * @author:	Guido Classen <clagix@gmail.com>
 * @descr:	NAND Flash support
 * @references: borrowed heavily from Linux mtd-utils code:
 *		flash_eraseall.c by Arcom Control System Ltd
 *		nandwrite.c by Steven J. Hill (sjhill@realitydiluted.com)
 *			       and Thomas Gleixner (tglx@linutronix.de)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Copyright 2010 Freescale Semiconductor
 * The portions of this file whose copyright is held by Freescale and which
 * are not considered a derived work of GPL v2-only code may be distributed
 * and/or modified under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */
#include <common.h>
#include <command.h>
#include <watchdog.h>
#include <malloc.h>
#include <div64.h>

#include <asm/errno.h>
#include <linux/mtd/mtd.h>
#include <nand.h>
#include <jffs2/jffs2.h>
#include <ShareType.h>
#include <fat.h>
#include "../../mstar/unfd/inc/common/drvNAND.h"
extern int Disable_Err_log;

typedef struct erase_info erase_info_t;
typedef struct mtd_info	  mtd_info_t;

/* support only for native endian JFFS2 */
#define cpu_to_je16(x) (x)
#define cpu_to_je32(x) (x)

#define MAX_PAGE_SIZE       16384
#define MAX_OOB_SIZE        1280

 /* write some data to memory at once, this value should be aligned with main+spare size */
#define FAT_BUFFER_LEN(pagelen, fatblock)  (pagelen * fatblock * 4)              

/*****************************************************************************/
static int nand_block_bad_scrub(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	return 0;
}

/**
 * nand_erase_opts: - erase NAND flash with support for various options
 *		      (jffs2 formating)
 *
 * @param meminfo	NAND device to erase
 * @param opts		options,  @see struct nand_erase_options
 * @return		0 in case of success
 *
 * This code is ported from flash_eraseall.c from Linux mtd utils by
 * Arcom Control System Ltd.
 */
int nand_erase_opts(nand_info_t *meminfo, const nand_erase_options_t *opts)
{
	struct jffs2_unknown_node cleanmarker;
	erase_info_t erase;
	unsigned long erase_length, erased_length; /* in blocks */
	int bbtest = 1;
	int result;
	int percent_complete = -1;
	int (*nand_block_bad_old)(struct mtd_info *, loff_t, int) = NULL;
	const char *mtd_device = meminfo->name;
	struct mtd_oob_ops oob_opts;
	struct nand_chip *chip = meminfo->priv;

	if ((opts->offset & (meminfo->writesize - 1)) != 0) {
		printf("Attempt to erase non page aligned data\n");
		return -1;
	}

	memset(&erase, 0, sizeof(erase));
	memset(&oob_opts, 0, sizeof(oob_opts));

	erase.mtd = meminfo;
	erase.len  = meminfo->erasesize;
	erase.addr = opts->offset;
	erase_length = lldiv(opts->length + meminfo->erasesize - 1,
			     meminfo->erasesize);

	cleanmarker.magic = cpu_to_je16 (JFFS2_MAGIC_BITMASK);
	cleanmarker.nodetype = cpu_to_je16 (JFFS2_NODETYPE_CLEANMARKER);
	cleanmarker.totlen = cpu_to_je32(8);

	/* scrub option allows to erase badblock. To prevent internal
	 * check from erase() method, set block check method to dummy
	 * and disable bad block table while erasing.
	 */
	if (opts->scrub) {
		struct nand_chip *priv_nand = meminfo->priv;

		nand_block_bad_old = priv_nand->block_bad;
		priv_nand->block_bad = nand_block_bad_scrub;
		/* we don't need the bad block table anymore...
		 * after scrub, there are no bad blocks left!
		 */
		if (priv_nand->bbt) {
			kfree(priv_nand->bbt);
		}
		priv_nand->bbt = NULL;
	}

	for (erased_length = 0;
	     erased_length < erase_length;
	     erase.addr += meminfo->erasesize) {

		WATCHDOG_RESET ();

		if (!opts->scrub && bbtest) {
			int ret = meminfo->block_isbad(meminfo, erase.addr);
			if (ret > 0) {
				if (!opts->quiet)
					printf("\rSkipping bad block at  "
					       "0x%08llx                 "
					       "                         \n",
					       erase.addr);

				if (!opts->spread)
					erased_length++;

				continue;

			} else if (ret < 0) {
				printf("\n%s: MTD get bad block failed: %d\n",
				       mtd_device,
				       ret);
				return -1;
			}
		}

		erased_length++;

		result = meminfo->erase(meminfo, &erase);
		if (result != 0) {
			printf("\n%s: MTD Erase failure: %d\n",
			       mtd_device, result);
			
		    result = meminfo->block_markbad(meminfo, erase.addr);
            if (result)
            {
                printf("Can not mark %llu bad, error 0x%x\r\n", erase.addr, result);
                //return -1;
            }
			else
			{
			    printf("Add a new bad block 0x%llx to BBT\n", erase.addr);
			}

			continue;
		}

		/* format for JFFS2 ? */
		if (opts->jffs2 && chip->ecc.layout->oobavail >= 8) {
			chip->ops.ooblen = 8;
			chip->ops.datbuf = NULL;
			chip->ops.oobbuf = (uint8_t *)&cleanmarker;
			chip->ops.ooboffs = 0;
			chip->ops.mode = MTD_OOB_AUTO;

			result = meminfo->write_oob(meminfo,
			                            erase.addr,
			                            &chip->ops);
			if (result != 0) {
				printf("\n%s: MTD writeoob failure: %d\n",
				       mtd_device, result);
				continue;
			}
		}

		if (!opts->quiet) {
			unsigned long long n = erased_length * 100ULL;
			int percent;

			do_div(n, erase_length);
			percent = (int)n;

			/* output progress message only at whole percent
			 * steps to reduce the number of messages printed
			 * on (slow) serial consoles
			 */
			if (percent != percent_complete) {
				percent_complete = percent;

				printf("\rErasing at 0x%llx -- %3d%% complete.",
				       erase.addr, percent);

				if (opts->jffs2 && result == 0)
					printf(" Cleanmarker written at 0x%llx.",
					       erase.addr);
			}
		}
	}
	if (!opts->quiet)
		printf("\n");

	if (nand_block_bad_old) {
		struct nand_chip *priv_nand = meminfo->priv;

		priv_nand->block_bad = nand_block_bad_old;
		priv_nand->scan_bbt(meminfo);
	}

	return 0;
}

/*
 * buffer array used for writing data
 */
static unsigned char data_buf[(MAX_PAGE_SIZE + MAX_OOB_SIZE) * 2];
static unsigned char oob_buf[MAX_OOB_SIZE];

/* OOB layouts to pass into the kernel as default */
static struct nand_oobinfo none_oobinfo = {
    .useecc = MTD_NANDECC_OFF,
};

static struct nand_oobinfo jffs2_oobinfo = {
    .useecc = MTD_NANDECC_PLACE,
    .eccbytes = 6,
    .eccpos = { 0, 1, 2, 3, 6, 7 }
};

static struct nand_oobinfo yaffs_oobinfo = {
    .useecc = MTD_NANDECC_PLACE,
    .eccbytes = 6,
    .eccpos = { 8, 9, 10, 13, 14, 15}
};

static struct nand_oobinfo autoplace_oobinfo = {
    .useecc = MTD_NANDECC_AUTOPLACE
};

extern EN_NAND_FS_FORMAT nand_fs_format;
/**
 * _nand_write_opts: - write image to NAND flash with support for various options
 *
 * @param meminfo        AND device to erase
 * @param opts              write options (@see nand_write_options)
 * @param nandoffset     nand offset where image writen to
 * @return                      in case of success
 *
 * This code is ported from nandwrite.c from Linux mtd utils by
 * Strawfly.
 */
int _nand_write_opts(nand_info_t *meminfo, nand_write_options_t *opts, u64 nandoffset)
{
        ulong imglen = 0;
        int baderaseblock;
        int blockstart = -1;
        int readlen;
        int oobinfochanged = 0;
        int percent_complete = -1;
        struct nand_oobinfo old_oobinfo;
        u64 mtdoffset = opts->offset;
        ulong erasesize_blockalign, pagelen;
        u_char *buffer = opts->buffer;
        size_t written;
        int result;
        int ublankpage = 0;
        u64 nandsize_oob=0, mtdoffsetnoOob=0, nandoffsetnoOob=0;
//        struct mtd_oob_ops tempOobOps;

    // For usb read
#if CONFIG_FAT
    block_dev_desc_t *dev_desc=NULL;
    int ret, dev=0;
    int part=1;
    char *ep;
    int fat_read_len = 0, is_fatread = 1, fat_read_len_align;
    long lPartno = 0;
    char * cPartno = getenv("partno");
    u64 mtdoffset_align;
    int datascan;

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

        if (opts->pad && opts->writeoob)
        {
            printf("Can't pad when oob data is present.\n");
            return -1;
        }

        if(opts->writeoob)
            pagelen = meminfo->oobblock + meminfo->oobsize;
        else
            pagelen = meminfo->oobblock;
        
        /* set erasesize to specified number of blocks - to match
        * jffs2 (virtual) block size */
        if (opts->blockalign == 0)
        {
            erasesize_blockalign = meminfo->erasesize ;
        }
        else
        {
            erasesize_blockalign = meminfo->erasesize * opts->blockalign;
        }

        /* get image length */
        imglen = opts->length;

        nandsize_oob = (meminfo->size / meminfo->oobblock) * pagelen;
//        printk("imglen=0x%lx, nandsize_oob - opts->offset=0x%lx, opts->offset=0x%lx\n",
//                    imglen, nandsize_oob - opts->offset, opts->offset);    
        if (imglen > (nandsize_oob - opts->offset - nandoffset))
        {
//            printf("Image %d bytes, NAND page %d bytes, "
//                    "OOB area %ld bytes, device size %ld bytes\n",
//                    imglen, pagelen, meminfo->oobsize, nandsize_oob);
            printf("Input block does not fit into device\n");
            goto restoreoob;
        }

        if (!opts->quiet)
        {
            printf("\n");
        }

        /* get data from input and write to the device */
        while (imglen && (mtdoffset < (nandsize_oob - nandoffset)))
        {
//            printf("imglen=0x%llx, mtdoffset=0x%llx\n",imglen,mtdoffset);
            WATCHDOG_RESET ();

            /*
            * new eraseblock, check for bad block(s). Stay in the
            * loop to be sure if the offset changes because of
            * a bad block, that the next block that will be
            * written to is also checked. Thus avoiding errors if
            * the block(s) after the skipped block(s) is also bad
            * (number of blocks depending on the blockalign
            */            
            mtdoffsetnoOob = mtdoffset / pagelen * meminfo->oobblock;
            blockstart = mtdoffsetnoOob & (~erasesize_blockalign+1);
            baderaseblock = 0;
            do{
                nandoffsetnoOob = nandoffset / pagelen * meminfo->oobblock;
                int ret = meminfo->block_isbad(meminfo, blockstart + nandoffsetnoOob);

                if ((ret < 0) && (blockstart < (meminfo->size - nandoffsetnoOob)))
                {
                    printf("Bad block check failed\n");
                    goto restoreoob;
                }
                
                if ((ret == 1) || ((!opts->bbtbadblock) && (ret == 2)))
                {
                    baderaseblock = 1;
                    opts->badblockcnt++;
                    nandoffsetnoOob += erasesize_blockalign;
                    nandoffset = (nandoffsetnoOob / meminfo->oobblock) * pagelen;
                    if (!opts->quiet)
                    {
                        printf("\rBad block #%d at 0x%llx "
                                "in erase block will be skipped\n",
                                opts->badblockcnt, blockstart + nandoffsetnoOob);
                    }
                 }else{
                    baderaseblock = 0;
                 }
            }while(baderaseblock && (nandoffset + mtdoffset <= nandsize_oob - pagelen));

            if(nandoffset + mtdoffset > nandsize_oob - pagelen)
                goto Finish;
            
          #if CONFIG_FAT
              if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
              {
                  if (1 == is_fatread)
                  {
                      if (imglen >= FAT_BUFFER_LEN(pagelen, 0x200))
                      {
                          fat_read_len = FAT_BUFFER_LEN(pagelen, 0x200);
                      }
                      else
                      {
                          fat_read_len = imglen;
                      }
                      mtdoffset_align = ALIGN(mtdoffset, 0x200);
                      fat_read_len_align = fat_read_len;
                      if(mtdoffset_align > mtdoffset){
                        mtdoffset_align -= 0x200;
                        fat_read_len_align += (int) (mtdoffset - mtdoffset_align);
                      }                      
                      ret = file_fat_part_read (opts->fatfile_name, (unsigned char *)(opts->buffer), mtdoffset_align, fat_read_len_align);
                      flush_cache((ulong)opts->buffer, fat_read_len_align);
                      is_fatread = 0;
                      buffer = (u_char *) (opts->buffer + (mtdoffset - mtdoffset_align));

                      if(ret == -1) {
                          printf("\n** Unable to read \"%s\" from %s %d:%d **\n", opts->fatfile_name, "usb", dev, part);
                          return 1;
                      }
                  }
              }
          #endif

              readlen = meminfo->oobblock;

            if (opts->pad && (imglen < readlen + meminfo->oobsize))
            {
                readlen = imglen - meminfo->oobsize;
                if(readlen < sizeof(data_buf))
                {
                    if(meminfo->oobblock < sizeof(data_buf))
                    {
                        memset(data_buf + readlen, 0xff,
                            meminfo->oobblock - readlen);
                    }
                    else
                    {
                        memset(data_buf + readlen, 0xff,
                            sizeof(data_buf) - readlen);
                    }
                }
                else
                {
                    printf("%s: Error: readlen exceeds the size of data_buf, at %d\n", __func__, __LINE__);
                }
            }

          #if CONFIG_FAT
              if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
              {
                  memcpy(data_buf, buffer, meminfo->oobblock);
                  flush_cache((ulong)buffer, meminfo->oobblock);
                  for(datascan = 0;  datascan < meminfo->oobblock; datascan ++){
                        if(buffer[datascan] != 0xff){
                            ublankpage = 1;
                            break;
                        }
                  }
              }
              else
              {
                    /* read page data from input memory buffer */
                    if(readlen < sizeof(data_buf))
                    {
                        memcpy(data_buf, buffer, readlen);
                    }
                    else
                    {
                        memcpy(data_buf, buffer, sizeof(data_buf));
                    }
              }
              
              if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
              {
                  buffer += pagelen;
                  fat_read_len -= pagelen;
                  if (fat_read_len <= 0)
                  {
                      is_fatread = 1;
                  }
              }
              else
              {
                  buffer += readlen;
              }
              
          #else
              /* read page data from input memory buffer */
              memcpy(data_buf, buffer, readlen);
              buffer += readlen;
          #endif

            if(ublankpage && (mtdoffset < (nandsize_oob - nandoffset))){
                    ublankpage = 0;
                    mtdoffsetnoOob = mtdoffset / pagelen * meminfo->oobblock;
//                    printk("mtdoffset=0x%llx, mtdoffsetnoOob=0x%llx\n",mtdoffset,mtdoffsetnoOob);
                    /* write out the page data */
#if 0
                    if(opts->writeoob){
                        /* write OOB data first, as ecc will be placed
                        * in there*/
                        memset(&tempOobOps, 0, sizeof(tempOobOps));
                        tempOobOps.len = meminfo->oobblock;
                        tempOobOps.ooblen = meminfo->oobsize;
                        tempOobOps.datbuf = (unsigned char *)&data_buf;
                        tempOobOps.oobbuf = (unsigned char *)&oob_buf;
                        result = meminfo->write_oob(meminfo, (mtdoffsetnoOob + nandoffset), &tempOobOps);
                        written=pagelen;

                    }else
#endif

                    {
                        nandoffsetnoOob = nandoffset / pagelen * meminfo->oobblock;
                        result = meminfo->write(meminfo,
                                        (mtdoffsetnoOob + nandoffsetnoOob),
                                        meminfo->oobblock,
                                        &written,
                                        (unsigned char *) &data_buf);
                        }
                        
                    if ((result != 0) && ((result != -EBADMSG) || (!opts->bbtbadblock)))
                    {
                        printf("writing NAND page at offset 0x%llx failed, error %d\n",
                                (mtdoffsetnoOob + nandoffsetnoOob), result);
                        goto restoreoob;
                    }
            }

            if(imglen < pagelen){
                printf("Warning: the image does not align to nand page size!\n"
                            "Left 0x%lx data not write\n", imglen);
                goto restoreoob;
            }
                
            imglen -= pagelen;
            
Finish:
            if (!opts->quiet)
            {
                    int percent = (int)
                            ((unsigned long long)
                            (opts->length-imglen) * 100
                            / opts->length);
                    /* output progress message only at whole percent
                    * steps to reduce the number of messages printed
                    * on (slow) serial consoles
                    */
                    if (percent != percent_complete)
                    {
                        printf("\rWriting data at 0x%llx "
                                "-- %3d%% complete.",
                                mtdoffset + nandoffset, percent);
                        percent_complete = percent;
                    }
            }

            mtdoffset += pagelen;
        }

        if (!opts->quiet)
        {
            printf("\n");
        }

restoreoob:
        if (oobinfochanged)
        {
            memcpy(&meminfo->oobinfo, &old_oobinfo,
                        sizeof(meminfo->oobinfo));
        }

        if (imglen > 0)
        {
            printf("Data did not fit into device, due to bad blocks, imglen= 0x%lu\n",imglen);
            return -1;
        }

        /* return happy */
        return 0;
}

/**
 * nand_write_opts: - write image to NAND flash with support for various options
 *
 * @param meminfo       AND device to erase
 * @param opts              write options (@see nand_write_options)
 * @return                      in case of success
 *
 * This code is ported from nandwrite.c from Linux mtd utils by
 * Steven J. Hill and Thomas Gleixner.
 */
int nand_write_opts(nand_info_t *meminfo, const nand_write_options_t *opts)
{
	int imglen = 0;
	int pagelen;
	int baderaseblock;
	int blockstart = -1;
	loff_t offs;
	int readlen;
	int oobinfochanged = 0;
	int percent_complete = -1;
	struct nand_oobinfo old_oobinfo;
	ulong mtdoffset = opts->offset;
	ulong erasesize_blockalign;
	u_char *buffer = opts->buffer;
	size_t written;
	int result;
        struct mtd_oob_ops tempOobOps;

    // For usb read
#if CONFIG_FAT
    block_dev_desc_t *dev_desc=NULL;
    int i, ret, dev=0;
    int part=1;
    char *ep;
    int fat_read_len = 0, is_fatread = 1;
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
        	part = (int)simple_strtoul(++ep, NULL, 16);
        }
        if (fat_register_device(dev_desc, part)!=0) {
        	printf ("\n** Unable to use %s %d:%d for fatload **\n", "usb", dev, part);
        	return -1;
        }
    }
#endif

	if (opts->pad && opts->writeoob)
    {
		printf("Can't pad when oob data is present.\n");
		return -1;
	}

	/* set erasesize to specified number of blocks - to match
	 * jffs2 (virtual) block size */
	if (opts->blockalign == 0)
    {
		erasesize_blockalign = meminfo->erasesize;
	}
    else
    {
		erasesize_blockalign = meminfo->erasesize * opts->blockalign;
	}

	/* make sure device page sizes are valid */
	if (!(meminfo->oobsize == 16 && meminfo->oobblock == 512)
            && (!((meminfo->oobsize == 8U) && (meminfo->oobblock == 256U)))
            && (!((meminfo->oobsize == 64U) && (meminfo->oobblock == 2048U)))
            && (!((meminfo->oobsize == 128U) && (meminfo->oobblock == 4096U)))
            && (!((meminfo->oobsize == 432U) && (meminfo->oobblock == 8192U))))
	{
		printf("Unknown flash (not normal NAND)\n");
		return -1;
	}

	/* read the current oob info */
	memcpy(&old_oobinfo, &meminfo->oobinfo, sizeof(old_oobinfo));

	/* write without ecc? */
	if (opts->noecc)
    {
		memcpy(&meminfo->oobinfo, &none_oobinfo,
		       sizeof(meminfo->oobinfo));
		oobinfochanged = 1;
	}

	/* autoplace ECC? */
	if (opts->autoplace && (old_oobinfo.useecc != MTD_NANDECC_AUTOPLACE))
    {

		memcpy(&meminfo->oobinfo, &autoplace_oobinfo,
		       sizeof(meminfo->oobinfo));
		oobinfochanged = 1;
	}

	/* force OOB layout for jffs2 or yaffs? */
	if (opts->forcejffs2 || opts->forceyaffs)
    {
		struct nand_oobinfo *oobsel =
			opts->forcejffs2 ? &jffs2_oobinfo : &yaffs_oobinfo;

		if (meminfo->oobsize == 8)
        {
			if (opts->forceyaffs)
            {
				printf("YAFSS cannot operate on "
				       "256 Byte page size\n");
				goto restoreoob;
			}
			/* Adjust number of ecc bytes */
			jffs2_oobinfo.eccbytes = 3;
		}

		memcpy(&meminfo->oobinfo, oobsel, sizeof(meminfo->oobinfo));
	}

	/* get image length */
	imglen = opts->length;
	pagelen = meminfo->oobblock
		+ ((opts->writeoob != 0) ? meminfo->oobsize : 0);

	/* check, if file is pagealigned */
	if ((!opts->pad) && ((imglen % pagelen) != 0))
    {
		printf("Input block length is not page aligned\n");
		goto restoreoob;
	}

	/* check, if length fits into device */
	if (((imglen / pagelen) * meminfo->oobblock)
	     > (meminfo->size - opts->offset))
	{
		printf("Image %d bytes, NAND page %d bytes, "
		       "OOB area %u bytes, device size %llu bytes\n",
		       imglen, pagelen, meminfo->oobblock, meminfo->size);
		printf("Input block does not fit into device\n");
		goto restoreoob;
	}

	if (!opts->quiet)
	{
		printf("\n");
	}

	/* get data from input and write to the device */
	while (imglen && (mtdoffset < meminfo->size))
    {

		WATCHDOG_RESET ();

		/*
		 * new eraseblock, check for bad block(s). Stay in the
		 * loop to be sure if the offset changes because of
		 * a bad block, that the next block that will be
		 * written to is also checked. Thus avoiding errors if
		 * the block(s) after the skipped block(s) is also bad
		 * (number of blocks depending on the blockalign
		 */
		while (blockstart != (mtdoffset & (~erasesize_blockalign+1)))
        {
			blockstart = mtdoffset & (~erasesize_blockalign+1);
			offs = blockstart;
			baderaseblock = 0;

			/* check all the blocks in an erase block for
			 * bad blocks */
			do {
				int ret = meminfo->block_isbad(meminfo, offs);

				if (ret < 0)
                {
					printf("Bad block check failed\n");
					goto restoreoob;
				}
				if (ret == 1)
                {
					baderaseblock = 1;
					if (!opts->quiet)
					{
						printf("\rBad block at 0x%lx "
						       "in erase block from "
						       "0x%x will be skipped\n",
						       (long) offs,
						       blockstart);
					}
				}

				if (baderaseblock)
                {
					mtdoffset = blockstart
						+ erasesize_blockalign;
				}
				offs +=	 erasesize_blockalign
					/ opts->blockalign;
			} while (offs < blockstart + erasesize_blockalign);
		}

          #if CONFIG_FAT
              if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
              {
                  if (1 == is_fatread)
                  {
                      if (imglen >= FAT_BUFFER_LEN(pagelen, 0x200))
                      {
                          fat_read_len = FAT_BUFFER_LEN(pagelen, 0x200);
                      }
                      else
                      {
                          fat_read_len = (imglen / meminfo->oobblock) * (meminfo->oobblock + meminfo->oobsize);
                      }
                	 ret = file_fat_read (opts->fatfile_name, (unsigned char *)(opts->buffer), fat_read_len);
                      flush_cache((ulong)opts->buffer, fat_read_len);
                      is_fatread = 0;
                      buffer = opts->buffer;

                	 if(ret == -1) {
                          printf("\n** Unable to read \"%s\" from %s %d:%d **\n", opts->fatfile_name, "usb", dev, part);
                          return 1;
                      }
                  }
              }
          #endif

              readlen = meminfo->oobblock;

            if (opts->pad && (imglen < readlen))
            {
                readlen = imglen;
                if(readlen < sizeof(data_buf))
                {
                    if(meminfo->oobblock < sizeof(data_buf))
                    {
                        memset(data_buf + readlen, 0xff,
                            meminfo->oobblock - readlen);
                    }
                    else
                    {
                        memset(data_buf + readlen, 0xff,
                            sizeof(data_buf) - readlen);
                    }
                }
                else
                {
                    printf("%s: Error: readlen exceeds the size of data_buf, at %d\n", __func__, __LINE__);
                }
            }

          #if CONFIG_FAT
              if ((NAND_FS_FORMAT_MKBIN == nand_fs_format) && (0x800 == meminfo->oobblock))
              {
                  for (i = 0; i < 4; i++)
                  {
                      memcpy(data_buf + 512 * i, buffer + 528 * i, 512);
                      flush_cache((ulong)buffer, 2048);
                  }
              }
              else
              {
                    /* read page data from input memory buffer */
                    if(readlen < sizeof(data_buf))
                    {
                        memcpy(data_buf, buffer, readlen);
                    }
                    else
                    {
                        memcpy(data_buf, buffer, sizeof(data_buf));
                    }
              }
              if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
              {
		    buffer += (meminfo->oobblock + meminfo->oobsize);
                  fat_read_len = fat_read_len - meminfo->oobblock - meminfo->oobsize;
                  if (fat_read_len <= 0)
                  {
                      is_fatread = 1;
                  }
              }
              else
              {
                  buffer += readlen;
              }
          #else
              /* read page data from input memory buffer */
              memcpy(data_buf, buffer, readlen);
              buffer += readlen;
          #endif

		if (opts->writeoob)
        {
			/* read OOB data from input memory block, exit
			 * on failure */
			memcpy(oob_buf, buffer, meminfo->oobsize);
			buffer += meminfo->oobsize;

			/* write OOB data first, as ecc will be placed
			 * in there*/
                    memset(&tempOobOps, 0, sizeof(tempOobOps));			 
			tempOobOps.ooblen = meminfo->oobsize;

                        tempOobOps.oobbuf = (unsigned char *)&oob_buf;
			result = meminfo->write_oob(meminfo, mtdoffset, &tempOobOps);
                        written=tempOobOps.oobretlen;
			if (result != 0)
            {
				printf("\nMTD writeoob failure: %d\n",
				       result);
				goto restoreoob;
			}
			imglen -= meminfo->oobsize;
		}

		/* write out the page data */
		result = meminfo->write(meminfo,
					mtdoffset,
					meminfo->oobblock,
					&written,
					(unsigned char *) &data_buf);

		if (result != 0)
        {
			printf("writing NAND page at offset 0x%lx failed\n",
			       mtdoffset);
			goto restoreoob;
		}
		imglen -= readlen;

		if (!opts->quiet)
        {
			int percent = (int)
				((unsigned long long)
				 (opts->length-imglen) * 100
				 / opts->length);
			/* output progress message only at whole percent
			 * steps to reduce the number of messages printed
			 * on (slow) serial consoles
			 */
			if (percent != percent_complete)
            {
				printf("\rWriting data at 0x%lux "
				       "-- %3d%% complete.",
				       mtdoffset, percent);
				percent_complete = percent;
			}
		}

		mtdoffset += meminfo->oobblock;
	}

	if (!opts->quiet)
	{
		printf("\n");
	}

restoreoob:
	if (oobinfochanged)
    {
		memcpy(&meminfo->oobinfo, &old_oobinfo,
		       sizeof(meminfo->oobinfo));
	}

	if (imglen > 0)
    {
		printf("Data did not fit into device, due to bad blocks\n");
		return -1;
	}

	/* return happy */
	return 0;
}

/**
 * nand_read_opts: - read image from NAND flash with support for various options
 *
 * @param meminfo	NAND device to erase
 * @param opts		read options (@see struct nand_read_options)
 * @return		0 in case of success
 *
 */
int nand_read_opts(nand_info_t *meminfo, nand_read_options_t *opts)
{
	u64 imglen = opts->length;
	int pagelen;
	int baderaseblock;
	u64 blockstart = -1;
	int percent_complete = -1;
	loff_t offs;
	size_t readlen;
	u64 mtdoffset, write_blocksize=0;
	u_char *buffer = opts->buffer;
	int result;
    struct mtd_oob_ops tempOobOps;

    // For usb write
#if CONFIG_FAT
    block_dev_desc_t *dev_desc=NULL;
    int dev=0;
    int part=1;
    char *ep;
    int fd = 0;
    u_char *buf_end = buffer;
    int fat_write_len = 0, is_fatwrite = 0;
#endif

#if 0
	/* make sure device page sizes are valid */
	if (!(meminfo->oobsize == 16 && meminfo->oobblock == 512)
            && (!((meminfo->oobsize == 8U) && (meminfo->oobblock == 256U)))
            && (!((meminfo->oobsize == 64U) && (meminfo->oobblock == 2048U)))
            && (!((meminfo->oobsize == 128U) && (meminfo->oobblock == 4096U)))
            && (!((meminfo->oobsize == 432U) && (meminfo->oobblock == 8192U))))
	{
		printf("Unknown flash (not normal NAND)\n");
		return -1;
	}
#endif

	pagelen = meminfo->oobblock + ((opts->readoob != 0) ? meminfo->oobsize : 0);
    mtdoffset = (opts->offset / pagelen) * meminfo->oobblock;

	/* check, if length is not larger than device */
	if (((imglen / pagelen) * meminfo->oobblock) > (meminfo->size - mtdoffset))
	{
		printf("Image %lld bytes, NAND page %d bytes, "
		       "OOB area %u bytes, device size %llu bytes\n",
		       imglen, pagelen, meminfo->oobsize, meminfo->size);
		printf("Input block is larger than device\n");
		return -1;
	}

	if (!opts->quiet)
	{
		printf("\n");
	}

       /* Write to usb for binary file making
        Initialize something and open usb file */
   #if CONFIG_FAT
       if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
       {
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
        		part = (int)simple_strtoul(++ep, NULL, 16);
        	}
           	if (fat_register_device(dev_desc,part)!=0) {
		printf ("\n** Unable to use %s %d:%d for fatwrite **\n","usb",dev,part);
		return -1;
	    }

            fd = file_fat_open(opts->fatfile_name, "a");
			if(fd < 0) {
         		printf ("Error: file open %s failed\n",opts->fatfile_name);
				return -1;
			}
       }
   #endif
    
//        printf("Pre while: mtdoffset=0x%llx, opts->offset=0x%llx\n",mtdoffset, opts->offset);
	/* get data from input and write to the device */
	while (imglen && (mtdoffset < meminfo->size))
    {
            WATCHDOG_RESET ();

    		/*
    		 * new eraseblock, check for bad block(s). Stay in the
    		 * loop to be sure if the offset changes because of
    		 * a bad block, that the next block that will be
    		 * written to is also checked. Thus avoiding errors if
    		 * the block(s) after the skipped block(s) is also bad
    		 * (number of blocks depending on the blockalign
    		 */
    		while ((blockstart != (mtdoffset & (~meminfo->erasesize+1))) &&
                        (mtdoffset < meminfo->size))
            {
    			blockstart = mtdoffset & (~meminfo->erasesize+1);
//                    printf("@@@blockstart=0x%llx, mtdoffset=0x%llx@@@\n",blockstart,mtdoffset);
    			offs = blockstart;
    			baderaseblock = 0;

    			/* check all the blocks in an erase block for
    			 * bad blocks */
    			do {
    				int ret = meminfo->block_isbad(meminfo, offs);

    				if (ret < 0)
                    {
    					printf("Bad block check failed\n");
    					return -1;
    				}

    				if ((ret == 1) || ((!opts->bbtbadblock) && (ret == 2)))
                    {
    					baderaseblock = 1;
    					opts->badblockcnt ++;
    					if (!opts->quiet)
    					{
    						printf("\rBad block #%d at 0x%lx "
    						       "in erase block from "
    						       "0x%llx will be skipped\n",
    						       opts->badblockcnt, (long) offs,
    						       mtdoffset);
    					}                   
    				}
				#if CONFIG_FAT
					if ((NAND_FS_FORMAT_MKBIN == nand_fs_format) && (fd >= 0)
						 && (baderaseblock == 0) && (opts->emptyskip == 1))
					{
                        u32 *tmpbuf = (u32 *)data_buf;
					
					    result = meminfo->read(meminfo,
    				        mtdoffset,
    		                meminfo->oobblock,
    				        &readlen,
    				        (unsigned char *) &data_buf);
						if ((result != 0) && ((result != -EBADMSG) || (!opts->bbtbadblock)))
                        {
    			            printf("reading NAND page at offset 0x%llx failed\n",
    			                   mtdoffset);
    			            return -1;
    		            }
					    result = meminfo->read(meminfo,
    				        (mtdoffset + meminfo->oobblock),
    		                meminfo->oobblock,
    				        &readlen,
    				        (unsigned char *)(data_buf + meminfo->oobblock));
						if ((result != 0) && ((result != -EBADMSG) || (!opts->bbtbadblock)))
                        {
    			            printf("reading NAND page at offset 0x%llx failed\n",
    			                   mtdoffset + meminfo->oobblock);
    			            return -1;
    		            }

                        // ubi erase counter page
						if ((be32_to_cpu(tmpbuf[0]) == 0x55424923) ||
							(be32_to_cpu(tmpbuf[0]) == 0xffffffff))
						{
						    // ubi vid page is empty?
						    if (be32_to_cpu(((u32 *)(data_buf + meminfo->oobblock))[0])
								    == 0xffffffff)
						    {
						        baderaseblock = 1;
    					        opts->badblockcnt ++;
						    }
						}
					}
				#endif
    				if (baderaseblock)
    				{
                    #if CONFIG_FAT
                        if ((NAND_FS_FORMAT_MKBIN == nand_fs_format) && (fd >= 0))
                        {
                            /* If meet bad block, we need flush the buffer into disk before. */
                            if(buf_end != opts->buffer)
                            {
                                fat_write_len = buf_end - opts->buffer;
                                buf_end = opts->buffer;                                        
                                flush_cache((ulong)buf_end, fat_write_len);
                                file_fat_write(fd, (unsigned char *) buf_end, fat_write_len);
                                is_fatwrite = 0;
                            }
                         }
                    #endif   
                        /* Fill the bad block area with 0xff data */
                        //write_blocksize = (meminfo->erasesize / meminfo->oobblock) * pagelen;
                        //u_char *blank_buf = (u_char *) malloc(sizeof(pagelen));
                        write_blocksize = blockstart + meminfo->erasesize - mtdoffset;
                        write_blocksize = (write_blocksize / meminfo->oobblock) * pagelen;
                        while ((imglen >=  pagelen) && write_blocksize){
                            //memset(blank_buf, 0xff, pagelen);
                            //file_fat_write(fd, (unsigned char *) blank_buf, pagelen);
                            imglen -= pagelen;
                            write_blocksize -= pagelen;
                        }                                        
                                                                            
                        mtdoffset = blockstart + meminfo->erasesize;

                        if((imglen < pagelen) || (mtdoffset >= meminfo->size))
                            goto Finish;

    				}
    				offs += meminfo->erasesize;

    			} while (offs < blockstart + meminfo->erasesize);
    		}

            if((imglen == 0) || (mtdoffset >= meminfo->size))
                goto Finish;
            
    		/* read page data to memory buffer */
            if (opts->readoob)
            {
                memset(&tempOobOps, 0, sizeof(tempOobOps));
                tempOobOps.datbuf = (unsigned char *) &data_buf;
                tempOobOps.oobbuf = (unsigned char *)&oob_buf;
                tempOobOps.len = meminfo->oobblock;
                tempOobOps.ooblen = meminfo->oobsize;                
                tempOobOps.mode = MTD_OOB_RAW;
                result = meminfo->read_oob(meminfo, mtdoffset, &tempOobOps);
                readlen = meminfo->oobblock + meminfo->oobsize;
                flush_cache((ulong)data_buf, meminfo->oobblock + meminfo->oobsize);
            }
            else 
            {
                result = meminfo->read(meminfo,
    				       mtdoffset,
    				       meminfo->oobblock,
    				       &readlen,
    				       (unsigned char *) &data_buf);
                flush_cache((ulong)data_buf, meminfo->oobblock);
            }

    		if ((result != 0) && ((result != -EBADMSG) || (!opts->bbtbadblock)))
            {
    			printf("reading NAND page at offset 0x%llx failed\n",
    			       mtdoffset);
    			return -1;
    		}

    		if (imglen <= readlen)
            {
    			readlen = imglen;
            #if CONFIG_FAT
                if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
                {
                     is_fatwrite = 1;
                 }
            #endif
    		}

            /* NAND_FS_FORMAT_RAW means read both main and oob area and comply with the
              special oob layout of FCIE2 (512 16, 512 16, 512 16, 512 16 instead of 2048 64) */
            if (NAND_FS_FORMAT_RAW == nand_fs_format)
            {
                //for (i = 0; i < 4; i++)
                //{
                    //memcpy((buffer + 528 * i), (data_buf + 512 * i), 512);
                    //memcpy((buffer + 528 * i + 512), (data_buf + 2048 + 16 * i), 16);
                //}
                memcpy(buffer, data_buf, readlen);
            }
        #if CONFIG_FAT
            else if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
            {
                //if (0x800 == meminfo->oobblock)
                //{
                    //for (i = 0; i < 4; i++)
                    //{
                        //memcpy((buf_end + 528 * i), (data_buf + 512 * i), 512);
                        //memcpy((buf_end + 528 * i + 512), (data_buf + 2048 + 16 * i), 16);
                    //}
                //}
                //else if (0x200 == meminfo->oobblock)
                //{
                    memcpy(buf_end, data_buf, readlen);
                //}
                if (opts->readoob)
                {
                    buf_end += (meminfo->oobblock + opts->phyoobsize);
                }
				else
				{
                    buf_end += readlen;
				}
                if (((buf_end - opts->buffer) >= FAT_BUFFER_LEN(pagelen, 0x200))
                        || (1 == is_fatwrite))
                {
                    fat_write_len = buf_end - opts->buffer;
                    buf_end = opts->buffer;
                    is_fatwrite = 1;
                }
            }
        #endif
            else
            {
                memcpy(buffer, data_buf, readlen);
            }
            
    		buffer += readlen;
    		imglen -= readlen;
            /* Write to usb for binary file making.
                 writing one page to usb each time can save memory consuming, so append writing mode is used */
        #if CONFIG_FAT
            if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
            {
                if((imglen == 0) && (buf_end != opts->buffer))
                {
//                    printf("\nread image complete!\n");
                    fat_write_len = buf_end - opts->buffer;
                    buf_end = opts->buffer;
                    is_fatwrite = 1;
                }
                
                if((is_fatwrite == 1) && (fd >= 0))
                {
                    flush_cache((ulong)buf_end, fat_write_len);
//                    printf("\nstart fat write len=0x%08x\n", fat_write_len);
                    file_fat_write(fd, (unsigned char *) buf_end, fat_write_len);
                    is_fatwrite = 0;
                }
            }
        #endif

Finish:
    		if (!opts->quiet)
            {
    			int percent = (int)
    				((unsigned long long)
    				 (opts->length-imglen) * 100
    				 / opts->length);
    			/* output progress message only at whole percent
    			 * steps to reduce the number of messages printed
    			 * on (slow) serial consoles
    			 */
    			if (percent != percent_complete)
                {
        			if (!opts->quiet)
        			{
#if (!CONFIG_MSTAR_SILENT)
        				printf("\rReading data from 0x%llx "
        				       "-- %3d%% complete.",
        				       mtdoffset, percent);
#endif
        				percent_complete = percent;
        			}
    			}
    		}

    		mtdoffset += meminfo->oobblock;
        
	}

	if (!opts->quiet)
	{
		printf("\n");
	}

	if (imglen > 0)
    {
		printf("Could not read entire image due to bad blocks\n");
		return -1;
	}

   #if CONFIG_FAT
       if (NAND_FS_FORMAT_MKBIN == nand_fs_format)
       {
           file_fat_close(fd);
            /*if(fd==-1) {
                        printf("\n** Unable to write \"%s\" from %s %d:%d **\n",opts->fatfile_name,"usb",dev,part);
                        return 1;
                    }*/
       }
   #endif

	/* return happy */
	return 0;
}

/**
 * nand_erase_env: - erase NAND flash
 *
 * @param meminfo	NAND device to erase
 * @param opts		options,  @see struct nand_erase_options
 * @return		0 in case of success
 *
 * This code is ported from flash_eraseall.c from Linux mtd utils by
 * Arcom Control System Ltd.
 */
#if defined(CONFIG_ENV_IS_IN_NAND) 
extern unsigned int need_blk; 

int nand_erase_env(nand_info_t *meminfo, const nand_erase_options_t *opts)
{
	erase_info_t erase;
	unsigned int erase_length,erased_length;
	int i;
    int result;
	unsigned char *rbuf;
    unsigned char *wbuf;
	
    struct nand_chip *chip = meminfo->priv;
	       	
    wbuf = malloc(meminfo->writesize+meminfo->oobsize);
    if(wbuf ==NULL)
	    return 1;

	rbuf = malloc(meminfo->writesize+meminfo->oobsize);
    if(rbuf ==NULL)
	    return 1;	
    
	memset(&erase, 0, sizeof(erase));
	erase.mtd = meminfo;
	erase.len  = meminfo->erasesize;
	erase.addr = opts->offset;
	erase_length = opts->length;
	erased_length =0;

    for (i = 0;i < (erase_length/meminfo->erasesize); i++) 
	{ 
	    result = meminfo->block_isbad(meminfo, erase.addr);
		if(result ==0)
		{
		    meminfo->block_markbad(meminfo, erase.addr);
		}
		else if(result <0)
		{
		    printf("\n MTD get bad block failed\n");
            return 1;
		}
		
		chip->ops.ooblen  = meminfo->oobsize;
		chip->ops.len     = meminfo->writesize;
		chip->ops.datbuf  = rbuf;
		chip->ops.oobbuf  = &rbuf[meminfo->writesize];
		chip->ops.ooboffs = 0;
		chip->ops.mode = MTD_OOB_RAW;

		meminfo->read_oob(meminfo, erase.addr, &chip->ops);
        
 		if (chip->ops.oobbuf[chip->badblockpos] != 0xff)
		{
		    erase.addr += meminfo->erasesize;
			continue;
		} 
        else
        {
            chip->erase_cmd(meminfo, erase.addr>>chip->page_shift);
			result = chip->waitfunc(meminfo, chip);
            if (result & NAND_STATUS_FAIL)
            {
                memset(wbuf, '\0' , meminfo->writesize+meminfo->oobsize);
				chip->ops.ooblen = meminfo->oobsize;
				chip->ops.len    = meminfo->writesize;
				chip->ops.datbuf = wbuf;
				chip->ops.oobbuf = &wbuf[meminfo->writesize];
				chip->ops.ooboffs = 0;
				chip->ops.mode = MTD_OOB_RAW;

				meminfo->write_oob(meminfo,erase.addr,&chip->ops);
				erase.addr += meminfo->erasesize;			   
            }
			else
			{
			    erase.addr += meminfo->erasesize;
                erased_length++;
			}
        }				
	}
    free(wbuf);
	free(rbuf);
    if(erased_length < (need_blk<<1))
	    return 1;
	else
	   	return 0;

}
#endif

#ifdef CONFIG_CMD_NAND_LOCK_UNLOCK

/******************************************************************************
 * Support for locking / unlocking operations of some NAND devices
 *****************************************************************************/

#define NAND_CMD_LOCK		0x2a
#define NAND_CMD_LOCK_TIGHT	0x2c
#define NAND_CMD_UNLOCK1	0x23
#define NAND_CMD_UNLOCK2	0x24
#define NAND_CMD_LOCK_STATUS	0x7a

/**
 * nand_lock: Set all pages of NAND flash chip to the LOCK or LOCK-TIGHT
 *	      state
 *
 * @param mtd		nand mtd instance
 * @param tight		bring device in lock tight mode
 *
 * @return		0 on success, -1 in case of error
 *
 * The lock / lock-tight command only applies to the whole chip. To get some
 * parts of the chip lock and others unlocked use the following sequence:
 *
 * - Lock all pages of the chip using nand_lock(mtd, 0) (or the lockpre pin)
 * - Call nand_unlock() once for each consecutive area to be unlocked
 * - If desired: Bring the chip to the lock-tight state using nand_lock(mtd, 1)
 *
 *   If the device is in lock-tight state software can't change the
 *   current active lock/unlock state of all pages. nand_lock() / nand_unlock()
 *   calls will fail. It is only posible to leave lock-tight state by
 *   an hardware signal (low pulse on _WP pin) or by power down.
 */
int nand_lock(struct mtd_info *mtd, int tight)
{
	int ret = 0;
	int status;
	struct nand_chip *chip = mtd->priv;

	/* select the NAND device */
	chip->select_chip(mtd, 0);

	chip->cmdfunc(mtd,
		      (tight ? NAND_CMD_LOCK_TIGHT : NAND_CMD_LOCK),
		      -1, -1);

	/* call wait ready function */
	status = chip->waitfunc(mtd, chip);

	/* see if device thinks it succeeded */
	if (status & 0x01) {
		ret = -1;
	}

	/* de-select the NAND device */
	chip->select_chip(mtd, -1);
	return ret;
}

/**
 * nand_get_lock_status: - query current lock state from one page of NAND
 *			   flash
 *
 * @param mtd		nand mtd instance
 * @param offset	page address to query (muss be page aligned!)
 *
 * @return		-1 in case of error
 *			>0 lock status:
 *			  bitfield with the following combinations:
 *			  NAND_LOCK_STATUS_TIGHT: page in tight state
 *			  NAND_LOCK_STATUS_LOCK:  page locked
 *			  NAND_LOCK_STATUS_UNLOCK: page unlocked
 *
 */
int nand_get_lock_status(struct mtd_info *mtd, loff_t offset)
{
	int ret = 0;
	int chipnr;
	int page;
	struct nand_chip *chip = mtd->priv;

	/* select the NAND device */
	chipnr = (int)(offset >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);


	if ((offset & (mtd->writesize - 1)) != 0) {
		printf ("nand_get_lock_status: "
			"Start address must be beginning of "
			"nand page!\n");
		ret = -1;
		goto out;
	}

	/* check the Lock Status */
	page = (int)(offset >> chip->page_shift);
	chip->cmdfunc(mtd, NAND_CMD_LOCK_STATUS, -1, page & chip->pagemask);

	ret = chip->read_byte(mtd) & (NAND_LOCK_STATUS_TIGHT
					  | NAND_LOCK_STATUS_LOCK
					  | NAND_LOCK_STATUS_UNLOCK);

 out:
	/* de-select the NAND device */
	chip->select_chip(mtd, -1);
	return ret;
}

/**
 * nand_unlock: - Unlock area of NAND pages
 *		  only one consecutive area can be unlocked at one time!
 *
 * @param mtd		nand mtd instance
 * @param start		start byte address
 * @param length	number of bytes to unlock (must be a multiple of
 *			page size nand->writesize)
 *
 * @return		0 on success, -1 in case of error
 */
int nand_unlock(struct mtd_info *mtd, ulong start, ulong length)
{
	int ret = 0;
	int chipnr;
	int status;
	int page;
	struct nand_chip *chip = mtd->priv;
	printf ("nand_unlock: start: %08x, length: %d!\n",
		(int)start, (int)length);

	/* select the NAND device */
	chipnr = (int)(start >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	/* check the WP bit */
	chip->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);
	if (!(chip->read_byte(mtd) & NAND_STATUS_WP)) {
		printf ("nand_unlock: Device is write protected!\n");
		ret = -1;
		goto out;
	}

	if ((start & (mtd->erasesize - 1)) != 0) {
		printf ("nand_unlock: Start address must be beginning of "
			"nand block!\n");
		ret = -1;
		goto out;
	}

	if (length == 0 || (length & (mtd->erasesize - 1)) != 0) {
		printf ("nand_unlock: Length must be a multiple of nand block "
			"size %08x!\n", mtd->erasesize);
		ret = -1;
		goto out;
	}

	/*
	 * Set length so that the last address is set to the
	 * starting address of the last block
	 */
	length -= mtd->erasesize;

	/* submit address of first page to unlock */
	page = (int)(start >> chip->page_shift);
	chip->cmdfunc(mtd, NAND_CMD_UNLOCK1, -1, page & chip->pagemask);

	/* submit ADDRESS of LAST page to unlock */
	page += (int)(length >> chip->page_shift);
	chip->cmdfunc(mtd, NAND_CMD_UNLOCK2, -1, page & chip->pagemask);

	/* call wait ready function */
	status = chip->waitfunc(mtd, chip);
	/* see if device thinks it succeeded */
	if (status & 0x01) {
		/* there was an error */
		ret = -1;
		goto out;
	}

 out:
	/* de-select the NAND device */
	chip->select_chip(mtd, -1);
	return ret;
}
#endif

/**
 * check_skip_len
 *
 * Check if there are any bad blocks, and whether length including bad
 * blocks fits into device
 *
 * @param nand NAND device
 * @param offset offset in flash
 * @param length image length
 * @return 0 if the image fits and there are no bad blocks
 *         1 if the image fits, but there are bad blocks
 *        -1 if the image does not fit
 */
static int check_skip_len(nand_info_t *nand, loff_t offset, size_t length)
{
	size_t len_excl_bad = 0;
	int ret = 0;

	while (len_excl_bad < length) {
		size_t block_len, block_off;
		loff_t block_start;

		if (offset >= nand->size)
			return -1;

		block_start = offset & ~(loff_t)(nand->erasesize - 1);
		block_off = offset & (nand->erasesize - 1);
		block_len = nand->erasesize - block_off;

		if (!nand_block_isbad(nand, block_start))
			len_excl_bad += block_len;
		else
			ret = 1;

		offset += block_len;
	}

	return ret;
}

/**
 * nand_write_skip_bad:
 *
 * Write image to NAND flash.
 * Blocks that are marked bad are skipped and the is written to the next
 * block instead as long as the image is short enough to fit even after
 * skipping the bad blocks.
 *
 * @param nand  	NAND device
 * @param offset	offset in flash
 * @param length	buffer length
 * @param buffer        buffer to read from
 * @param withoob	whether write with yaffs format
 * @return		0 in case of success
 */
int nand_write_skip_bad(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int withoob)
{
	int rval = 0, blocksize;
	size_t left_to_write = *length;
	u_char *p_buffer = buffer;
	int need_skip;

#ifdef CONFIG_CMD_NAND_YAFFS
	if (withoob) {
		int pages;
		pages = nand->erasesize / nand->writesize;
		blocksize = (pages * nand->oobsize) + nand->erasesize;
		if (*length % (nand->writesize + nand->oobsize)) {
			printf ("Attempt to write incomplete page"
				" in yaffs mode\n");
			return -EINVAL;
		}
	} else
#endif
	{
		blocksize = nand->erasesize;
	}

	/*
	 * nand_write() handles unaligned, partial page writes.
	 *
	 * We allow length to be unaligned, for convenience in
	 * using the $filesize variable.
	 *
	 * However, starting at an unaligned offset makes the
	 * semantics of bad block skipping ambiguous (really,
	 * you should only start a block skipping access at a
	 * partition boundary).  So don't try to handle that.
	 */
	if ((offset & (nand->writesize - 1)) != 0) {
		printf ("Attempt to write non page aligned data\n");
		*length = 0;
		return -EINVAL;
	}

	need_skip = check_skip_len(nand, offset, *length);
	if (need_skip < 0) {
		printf ("Attempt to write outside the flash area\n");
		*length = 0;
		return -EINVAL;
	}

	if (!need_skip) {
		rval = nand_write (nand, offset, length, buffer);
		if (rval == 0)
			return 0;

		*length = 0;
		printf ("NAND write to offset %llx failed %d\n",
			offset, rval);
		return rval;
	}

	while (left_to_write > 0) {
		size_t block_offset = offset & (nand->erasesize - 1);
		size_t write_size;

		WATCHDOG_RESET ();

		if (nand_block_isbad (nand, offset & ~(nand->erasesize - 1))) {
			printf ("Skip bad block 0x%08llx\n",
				offset & ~(nand->erasesize - 1));
			offset += nand->erasesize - block_offset;
			continue;
		}

		if (left_to_write < (blocksize - block_offset))
			write_size = left_to_write;
		else
			write_size = blocksize - block_offset;

#ifdef CONFIG_CMD_NAND_YAFFS
		if (withoob) {
			int page, pages;
			size_t pagesize = nand->writesize;
			size_t pagesize_oob = pagesize + nand->oobsize;
			struct mtd_oob_ops ops;

			ops.len = pagesize;
			ops.ooblen = nand->oobsize;
			ops.mode = MTD_OOB_AUTO;
			ops.ooboffs = 0;

			pages = write_size / pagesize_oob;
			for (page = 0; page < pages; page++) {
				WATCHDOG_RESET();

				ops.datbuf = p_buffer;
				ops.oobbuf = ops.datbuf + pagesize;

				rval = nand->write_oob(nand, offset, &ops);
				if (!rval)
					break;

				offset += pagesize;
				p_buffer += pagesize_oob;
			}
		}
		else
#endif
		{
			rval = nand_write (nand, offset, &write_size, p_buffer);
			offset += write_size;
			p_buffer += write_size;
		}

		if (rval != 0) {
			printf ("NAND write to offset %llx failed %d\n",
				offset, rval);
			*length -= left_to_write;
			return rval;
		}

		left_to_write -= write_size;
	}

	return 0;
}

/**
 * nand_read_skip_bad:
 *
 * Read image from NAND flash.
 * Blocks that are marked bad are skipped and the next block is readen
 * instead as long as the image is short enough to fit even after skipping the
 * bad blocks.
 *
 * @param nand NAND device
 * @param offset offset in flash
 * @param length buffer length, on return holds remaining bytes to read
 * @param buffer buffer to write to
 * @return 0 in case of success
 */
int nand_read_skip_bad(nand_info_t *nand, loff_t offset, size_t *length,
		       u_char *buffer)
{
	int rval;
	size_t left_to_read = *length;
	u_char *p_buffer = buffer;
	int need_skip;

	if ((offset & (nand->writesize - 1)) != 0) {
		printf ("Attempt to read non page aligned data\n");
		*length = 0;
		return -EINVAL;
	}

	need_skip = check_skip_len(nand, offset, *length);
	if (need_skip < 0) {
		printf ("Attempt to read outside the flash area\n");
		*length = 0;
		return -EINVAL;
	}

	if (!need_skip) {
		rval = nand_read (nand, offset, length, buffer);
		if (!rval || rval == -EUCLEAN)
			return 0;

		*length = 0;
		printf ("NAND read from offset %llx failed %d\n",
			offset, rval);
		return rval;
	}

	while (left_to_read > 0) {
		size_t block_offset = offset & (nand->erasesize - 1);
		size_t read_length;

		WATCHDOG_RESET ();

		if (nand_block_isbad (nand, offset & ~(nand->erasesize - 1))) {
			printf ("Skipping bad block 0x%08llx\n",
				offset & ~(nand->erasesize - 1));
			offset += nand->erasesize - block_offset;
			continue;
		}

		if (left_to_read < (nand->erasesize - block_offset))
			read_length = left_to_read;
		else
			read_length = nand->erasesize - block_offset;

		rval = nand_read (nand, offset, &read_length, p_buffer);
		if (rval && rval != -EUCLEAN) {
			printf ("NAND read from offset %llx failed %d\n",
				offset, rval);
			*length -= left_to_read;
			return rval;
		}

		left_to_read -= read_length;
		offset       += read_length;
		p_buffer     += read_length;
	}

	return 0;
}

/**
 * nand_read_oob_skip_bad:
 *
 * Read image from NAND flash.
 * Blocks that are marked bad are skipped and the next block is readen
 * instead as long as the image is short enough to fit even after skipping the
 * bad blocks.
 *
 * @param nand NAND device
 * @param offset offset in flash
 * @param length buffer length, on return holds remaining bytes to read
 * @param buffer buffer to write to
 * @return 0 in case of success
 */
int nand_read_oob_skip_bad(nand_info_t *nand, loff_t offset, size_t *length,
		       u_char *buffer)
{
	int rval;
	size_t left_to_read = *length;
	int need_skip;
    loff_t offsetend = (loff_t) (offset + left_to_read);
    
    struct mtd_oob_ops ops;
    memset(&ops, 0, sizeof(ops));
    ops.datbuf = buffer;
    ops.oobbuf = buffer; /* must exist, but oob data will be appended to ops.datbuf */
    ops.len = left_to_read;
    ops.ooblen = (left_to_read/nand->oobblock) * nand->oobsize;
    ops.mode = MTD_OOB_RAW;
	if ((offset & (nand->writesize - 1)) != 0) {
		printf ("Attempt to read non page aligned data\n");
		*length = 0;
		return -EINVAL;
	}

	need_skip = check_skip_len(nand, offset, *length);

	if (!need_skip) {
        	rval = nand->read_oob(nand, offset, &ops);
            if (!rval || rval == -EUCLEAN)
                return 0;
             if(rval<0){
        		*length = 0;
        		printf ("NAND read from offset %llx failed %d\n",
        			offset, rval);
        		return rval;
             }
	}

	while ((left_to_read > 0) && (offset < nand->size)) {
		size_t block_offset = offset & (nand->erasesize - 1);
		size_t read_length;

		WATCHDOG_RESET ();

		if (nand_block_isbad (nand, offset & ~(nand->erasesize - 1))) {
			printf ("Skipping bad block 0x%08llx\n",
				offset & ~(nand->erasesize - 1));
			offset += nand->erasesize - block_offset;
                    if(offset <= offsetend){
                        if(*length > nand->erasesize - block_offset)
                            *length -= nand->erasesize - block_offset;
                        else
                            *length = 0;
                    }
			continue;
		}

		if (left_to_read < (nand->erasesize - block_offset))
			read_length = left_to_read;
		else
			read_length = nand->erasesize - block_offset;

            ops.len = read_length;
            ops.ooblen = (read_length/nand->oobblock) * nand->oobsize;
            
		rval = nand->read_oob(nand, offset, &ops);
		if (rval && rval != -EUCLEAN) {
			printf ("NAND read from offset %llx failed %d\n",
				offset, rval);
			*length -= left_to_read;
			return rval;
		}

		left_to_read -= read_length;            
		offset       += read_length;
		ops.datbuf     += (ops.len + ops.ooblen);
	}
        
	return 0;
}

/**
 * nand_write_slc_skip_bad:
 *
 * Write image to NAND flash.
 * Blocks that are marked bad are skipped and the is written to the next
 * block instead as long as the image is short enough to fit even after
 * skipping the bad blocks.
 *
 * @param nand  	NAND device
 * @param offset	offset in flash
 * @param length	buffer length
 * @param buffer        buffer to read from
 * @param withoob	whether write with yaffs format
 * @return		0 in case of success
 */
int nand_write_slc_skip_bad(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, int withoob)
{
	int rval = 0, blocksize;
	size_t left_to_write = *length;
	u_char *p_buffer = buffer;
	int need_skip;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	int BlkPageCnt = (pNandDrv->u8_CellType)? pNandDrv->u16_BlkPageCnt/2 : pNandDrv->u16_BlkPageCnt;
	int PageCnt = *length >> pNandDrv->u8_PageByteCntBits;
	ulong u32_Row;
	
	if(*length % pNandDrv->u16_PageByteCnt)
	{
		PageCnt += 1;
		left_to_write = PageCnt << pNandDrv->u8_PageByteCntBits;
	}
	
	if(pNandDrv->u8_CellType)
		blocksize = nand->erasesize/2;
	else
		blocksize = nand->erasesize;

	if((offset & (nand->erasesize -1))!= 0)
	{
		printf("Attempt to write non block aligned data in SLC mode\n");
		*length = 0;
		return -EINVAL;
	}
	/*
	 * nand_write() handles unaligned, partial page writes.
	 *
	 * We allow length to be unaligned, for convenience in
	 * using the $filesize variable.
	 *
	 * However, starting at an unaligned offset makes the
	 * semantics of bad block skipping ambiguous (really,
	 * you should only start a block skipping access at a
	 * partition boundary).  So don't try to handle that.
	 */
	if ((offset & (nand->writesize - 1)) != 0) {
		printf ("Attempt to write non page aligned data\n");
		*length = 0;
		return -EINVAL;
	}

	need_skip = check_skip_len(nand, offset, *length);
	if (need_skip < 0) {
		printf ("Attempt to write outside the flash area\n");
		*length = 0;
		return -EINVAL;
	}

	if (!need_skip) {
		
		u32_Row = offset >> pNandDrv->u8_PageByteCntBits;
		if(pNandDrv->u8_CellType)
		{
			while(PageCnt >= BlkPageCnt)
			{
				int i;
				ulong u32_TmpRow;
				for(i = 0; i < BlkPageCnt; i ++)
				{
					u32_TmpRow = u32_Row + ga_tPairedPageMap[i].u16_LSB;
					#if defined(FCIE_LFSR) && FCIE_LFSR
					if(pNandDrv->u8_RequireRandomizer == 1)
						NC_EnableLFSR();
					#endif
					rval = NC_WritePages(u32_TmpRow, p_buffer, NULL, 1);
					if(rval)
					{
						*length = 0;
						printf ("NAND write to offset %llx failed %d\n",
							offset, rval);
						return rval;
					}
					p_buffer += pNandDrv->u16_PageByteCnt;
				}
				PageCnt -= BlkPageCnt;
				u32_Row += pNandDrv->u16_BlkPageCnt;
			}
			if(PageCnt)
			{
				int i;
                ulong u32_TmpRow;
                for(i = 0; i < PageCnt; i ++)
                {
                    u32_TmpRow = u32_Row + ga_tPairedPageMap[i].u16_LSB;
					#if defined(FCIE_LFSR) && FCIE_LFSR
					if(pNandDrv->u8_RequireRandomizer == 1)
						NC_EnableLFSR();
					#endif
                    rval = NC_WritePages(u32_TmpRow, p_buffer, NULL, 1);
					if(rval)
					{
						*length = 0;
						printf ("NAND write to offset %llx failed %d\n",
							offset, rval);
						return rval;
					}
                    p_buffer += pNandDrv->u16_PageByteCnt;
                }
                PageCnt -= PageCnt;
			}
		}
		else
			rval = nand_write (nand, offset, length, buffer);
		if (rval == 0)
			return 0;

		*length = 0;
		printf ("NAND write to offset %llx failed %d\n",
			offset, rval);
		return rval;
	}

	while (left_to_write > 0) {
		size_t write_size;

		WATCHDOG_RESET ();

		if (nand_block_isbad (nand, offset & ~(nand->erasesize - 1))) {
			printf ("Skip bad block 0x%08llx\n",
				offset & ~(nand->erasesize - 1));
			offset += nand->erasesize;
			continue;
		}

		if (left_to_write < blocksize)
			write_size = left_to_write;
		else
			write_size = blocksize;

#ifdef CONFIG_CMD_NAND_YAFFS
		if (withoob) {
			int page, pages;
			size_t pagesize = nand->writesize;
			size_t pagesize_oob = pagesize + nand->oobsize;
			struct mtd_oob_ops ops;

			ops.len = pagesize;
			ops.ooblen = nand->oobsize;
			ops.mode = MTD_OOB_AUTO;
			ops.ooboffs = 0;

			pages = write_size / pagesize_oob;
			for (page = 0; page < pages; page++) {
				WATCHDOG_RESET();

				ops.datbuf = p_buffer;
				ops.oobbuf = ops.datbuf + pagesize;

				rval = nand->write_oob(nand, offset, &ops);
				if (!rval)
					break;

				offset += pagesize;
				p_buffer += pagesize_oob;
			}
		}
		else
#endif
		{
			if(pNandDrv->u8_CellType)
			{
				int i;
				ulong u32_TmpRow;
				u32_Row = offset >> pNandDrv->u8_BlkPageCntBits;
				for(i = 0; i < write_size >> pNandDrv->u8_PageByteCntBits; i++)
				{
					#if defined(FCIE_LFSR) && FCIE_LFSR
					if(pNandDrv->u8_RequireRandomizer == 1)
						NC_EnableLFSR();
					#endif

					u32_TmpRow = u32_Row + ga_tPairedPageMap[i].u16_LSB;
					rval = NC_WritePages(u32_TmpRow, p_buffer, NULL, 1);
					if(rval)
					{
					}
					p_buffer += pNandDrv->u16_PageByteCnt;
				}
				offset += write_size *2;
			}
			else
			{
				rval = nand_write (nand, offset, &write_size, p_buffer);
				offset += write_size;
				p_buffer += write_size;
			}
		}

		if (rval != 0) {
			printf ("NAND write to offset %llx failed %d\n",
				offset, rval);
			*length -= left_to_write;
			return rval;
		}

		left_to_write -= write_size;
	}

	return 0;
}

static inline int ErrorBitCountforEmptyPage(uint8_t u8_Byte)
{
	int i;
	int sum = 0;
	if(u8_Byte ^ 0xFF)
	{
		for(i = 0; i < 8; i ++)
			sum += ((u8_Byte >> i)  & 0x1) ^ 1;
	}
	return sum;
}

static int CheckEmptyPageFalseAlarm(unsigned char *main, unsigned char *spare)
{
	int false_alarm = 1;
	int i,j;
	int ECCErrBitCnt;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	//check if false alarm casued by empty page
	for(j = 0; j < pNandDrv->u16_PageSectorCnt; j++)
	{
		ECCErrBitCnt = 0;
		for(i = 0; i < pNandDrv->u16_SectorByteCnt; i++)
		{
			ECCErrBitCnt += ErrorBitCountforEmptyPage(main[ j*pNandDrv->u16_SectorByteCnt + i]);
		}
		if(ECCErrBitCnt > pNandDrv->u16_ECCCorretableBit)
		{
			false_alarm = 0;
			break;
		}
		else
			memset(main + j * pNandDrv->u16_SectorByteCnt, 0xFF, pNandDrv->u16_SectorByteCnt);
		
		for(i = 0; i < pNandDrv->u16_SectorSpareByteCnt; i++)
		{
			if(j*pNandDrv->u16_SectorSpareByteCnt + i < 512)
				ECCErrBitCnt += ErrorBitCountforEmptyPage(spare[ j*pNandDrv->u16_SectorSpareByteCnt + i]);
		}
		if(ECCErrBitCnt > pNandDrv->u16_ECCCorretableBit)
		{
			false_alarm = 0;
			break;
		}
		else
			memset(spare + j * pNandDrv->u16_SectorSpareByteCnt, 0xFF, pNandDrv->u16_SectorSpareByteCnt);
	}
	if(false_alarm == 1)
		printf("[%s]\tecc false alarm caused by empty page\n",__func__);
	else
		printf("[%s]\ttrue ecc fail\n",__func__);
	return false_alarm;
}


/*
 *		Refresh block data to avoid read disturbance happens.
 *
 *
 */

static int RefreshBlk(nand_info_t* nand, uint32_t u32_BlkRow, uint32_t u32_PageCnt, uint8_t u8_torture)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	MBBS_INFO_t* SPAREINFO;
	uint32_t u32_GoodBlkIdx, u32_Err, u32_Row;
	uint16_t u16_i;
	uint8_t *pu8_PageDataBuf = pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	uint8_t *pu8_PageSpareBuf = pNandDrv->PlatCtx_t.pu8_PageSpareBuf;

	SPAREINFO =(MBBS_INFO_t*) pNandDrv->PlatCtx_t.pu8_PageSpareBuf;

RETRY:
	printf("Blk %X need to refresh with page count = %d\n", u32_BlkRow >> pNandDrv->u8_BlkPageCntBits, u32_PageCnt);
	//find last good block of cis partition
	for(u32_GoodBlkIdx = 9; u32_GoodBlkIdx >= 5; u32_GoodBlkIdx ++)
	{
		Disable_Err_log = 1;
		u32_Err = NC_ReadPages(u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits, pu8_PageDataBuf, pu8_PageSpareBuf, 1);
		Disable_Err_log = 0;
	
		//how to avoid reading read cis block?
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			if(CheckEmptyPageFalseAlarm(pu8_PageDataBuf, pu8_PageSpareBuf) != 1)
			{
				u32_Err = NC_EraseBlk(u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits);
				if(u32_Err != UNFD_ST_SUCCESS)
				{
					//mark bad
					nand->block_markbad(nand, (u32_GoodBlkIdx << (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_PageByteCntBits)));
				}
				else
					break;
			}
		}
		if(pu8_PageSpareBuf[0] == 0xFF)
		{
			u32_Err = NC_EraseBlk(u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits);
			if(u32_Err == UNFD_ST_SUCCESS)
				break;
		}
	}
	printf("Blk %X is the last good block\n",	u32_GoodBlkIdx);
	
	// write to empty block, so called backup block

	memset(pu8_PageSpareBuf, 0xff, pNandDrv->u16_SpareByteCnt);
	printf("Write data to Blk %X\n", u32_GoodBlkIdx);
	for(u16_i = 0; u16_i < pNandDrv->u16_BlkPageCnt/2; u16_i ++)
	{
		//read back from target block
		u32_Row = (u32_BlkRow) + ga_tPairedPageMap[u16_i].u16_LSB;
		Disable_Err_log = 1;
		u32_Err = NC_ReadPages(u32_Row, pu8_PageDataBuf, pu8_PageSpareBuf, 1);
		Disable_Err_log = 0;
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			if(CheckEmptyPageFalseAlarm(pu8_PageDataBuf, pu8_PageSpareBuf) != 1)
			{
				printf("[%s] NAND fatal error page @ %X,when reading target block\n", __func__,u32_Row);
				printf("[%s] Please re-program nand\n", __func__);
				return -1;
			}
		}
		SPAREINFO->u8_BadBlkMarker = 0xFF;
		SPAREINFO->u16_BackupPBA = u32_BlkRow >> pNandDrv->u8_BlkPageCntBits;
		pu8_PageSpareBuf[3] = 0x36;
		pu8_PageSpareBuf[4] = 0x97;	
		u32_Row = (u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits) + ga_tPairedPageMap[u16_i].u16_LSB;
		u32_Err = NC_WritePages(u32_Row, pu8_PageDataBuf, pu8_PageSpareBuf, 1);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			//mark bad and goto research a good block
			nand->block_markbad(nand, (u32_GoodBlkIdx << (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_PageByteCntBits)));
			goto RETRY;
		}
	}

	//erase target block
	printf("Erase Target block %X\n", u32_BlkRow >> pNandDrv->u8_BlkPageCntBits);
	u32_Err = NC_EraseBlk(u32_BlkRow);
	if(u32_Err != UNFD_ST_SUCCESS)
	{
		//complicated need to do
		//mark bad and do shifting block
	}
	//if torture needed torture block for recheck read retry

	// write data to target block if torture ok
	// if torture fail -> do shifting block...
	// @FIXME
	if(u8_torture)
		;

	printf("Write data back to target block with page count = %d\n", pNandDrv->u16_BlkPageCnt/2);
	for(u16_i = 0; u16_i < pNandDrv->u16_BlkPageCnt/2; u16_i ++)
	{
		u32_Row = (u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits) + ga_tPairedPageMap[u16_i].u16_LSB;
		Disable_Err_log = 1;
		u32_Err = NC_ReadPages(u32_Row, pu8_PageDataBuf, pu8_PageSpareBuf, 1);
		Disable_Err_log = 0;

		if(u32_Err != UNFD_ST_SUCCESS)
		{
			if(CheckEmptyPageFalseAlarm(pu8_PageDataBuf, pu8_PageSpareBuf) != 1)
			{
				printf("[%s] NAND fatal error page @ %X,when reading backup block\n", __func__,u32_Row);
				printf("[%s] Please re-program nand\n", __func__);
				return -1;
			}
		}

		u32_Row = (u32_BlkRow) + ga_tPairedPageMap[u16_i].u16_LSB;
		u32_Err = NC_WritePages(u32_Row, pu8_PageDataBuf, NULL, 1);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			//mark bad and do shifting block
			//@FIXME
		}
	}
	
	//erase backup block
	u32_Err = NC_EraseBlk(u32_GoodBlkIdx << pNandDrv->u8_BlkPageCntBits);
	if(u32_Err != UNFD_ST_SUCCESS)
	{
		//mark bad
		nand->block_markbad(nand, (u32_GoodBlkIdx << (pNandDrv->u8_BlkPageCntBits + pNandDrv->u8_PageByteCntBits)));
	}

	return 0;
}

/**
 * nand_read_slc_skip_bad:
 *
 * Read image from NAND flash.
 * Blocks that are marked bad are skipped and the next block is readen
 * instead as long as the image is short enough to fit even after skipping the
 * bad blocks.
 *
 * @param nand NAND device
 * @param offset offset in flash
 * @param length buffer length, on return holds remaining bytes to read
 * @param buffer buffer to write to
 * @return 0 in case of success
 */
int nand_read_slc_skip_bad(nand_info_t *nand, loff_t offset, size_t *length,
		       u_char *buffer)
{
	int rval, blocksize;
	size_t left_to_read = *length;
	u_char *p_buffer = buffer;
	u_char *p_Spare;
	int need_skip;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	int BlkPageCnt = (pNandDrv->u8_CellType)? pNandDrv->u16_BlkPageCnt/2 : pNandDrv->u16_BlkPageCnt;
	int PageCnt = *length >> pNandDrv->u8_PageByteCntBits;
	int Corretable_ECC = 0;
	int ReadRetryHappens = 0;
	int OverThreshold = 0;
	int ECCSum = 0;
	ulong u32_Row;
	rval = 0;

	p_Spare = pNandDrv->PlatCtx_t.pu8_PageSpareBuf;

	if(pNandDrv->u8_CellType)
		blocksize = nand->erasesize/2;
	else
		blocksize = nand->erasesize;

	if((offset & (nand->erasesize -1))!= 0)
	{
		printf("Attempt to read non block aligned data in SLC mode\n");
		*length = 0;
		return -EINVAL;
	}

	if ((offset & (nand->writesize - 1)) != 0) {
		printf ("Attempt to read non page aligned data\n");
		*length = 0;
		return -EINVAL;
	}

	need_skip = check_skip_len(nand, offset, *length);
	if (need_skip < 0) {
		printf ("Attempt to read outside the flash area\n");
		*length = 0;
		return -EINVAL;
	}

	if (!need_skip) {
		if(pNandDrv->u8_CellType)
		{
			u32_Row = offset >> pNandDrv->u8_PageByteCntBits;
			while(PageCnt >= BlkPageCnt)
			{
				int i;
				ulong u32_TmpRow;
				ReadRetryHappens = 0;
				OverThreshold = 0;
				for(i = 0; i < BlkPageCnt; i ++)
				{
					#if defined(FCIE_LFSR) && FCIE_LFSR
					if(pNandDrv->u8_RequireRandomizer == 1)
						NC_EnableLFSR();
					#endif

					u32_TmpRow = u32_Row + ga_tPairedPageMap[i].u16_LSB;
					Disable_Err_log = 1;
					rval = NC_ReadPages(u32_TmpRow, p_buffer, p_Spare, 1);
					Disable_Err_log = 0;
					if(rval)
					{
						if(CheckEmptyPageFalseAlarm(p_buffer,p_Spare) == 1)
							ECCSum += 1;
						else
						{
							*length = 0;
							printf ("NAND read to offset %llx failed %d\n",
								offset, rval);
							return rval;
						}
					}
					else
					{
						//check ecc code
						NC_CheckECC(&Corretable_ECC);
						//check if read retry happens
						if(Corretable_ECC == pNandDrv->u16_ECCCorretableBit + 1)
							ReadRetryHappens = 1;
						if(Corretable_ECC >= pNandDrv->u16_ECCCorretableBit * 3 / 4)
							OverThreshold = 1;
						ECCSum += Corretable_ECC;
					}
					p_buffer += pNandDrv->u16_PageByteCnt;
				}
				//Trigger Read Retry backup process
				if(ReadRetryHappens == 1)
					RefreshBlk(nand, u32_Row, BlkPageCnt, 1);				
				//Trigger Too many correctable ecc bits handling
				if(OverThreshold == 1)
					RefreshBlk(nand, u32_Row, BlkPageCnt, 0);
				PageCnt -= BlkPageCnt;
				u32_Row += pNandDrv->u16_BlkPageCnt;
			}
			if(PageCnt)
			{
				int i;
				ulong u32_TmpRow;
				ReadRetryHappens = 0;
				OverThreshold = 0;
				for(i = 0; i < PageCnt; i ++)
				{
					#if defined(FCIE_LFSR) && FCIE_LFSR
					if(pNandDrv->u8_RequireRandomizer == 1)
						NC_EnableLFSR();
					#endif

					u32_TmpRow = u32_Row + ga_tPairedPageMap[i].u16_LSB;
					Disable_Err_log = 1;
					rval = NC_ReadPages(u32_TmpRow, p_buffer, p_Spare, 1);
					Disable_Err_log = 0;
					if(rval)
					{
						if(CheckEmptyPageFalseAlarm(p_buffer,p_Spare) == 1)
							ECCSum += 1;
						else
						{
							*length = 0;
							printf ("NAND read to offset %llx failed %d\n",
								offset, rval);
							return rval;
						}
					}
					else
					{
						//check ecc code
						NC_CheckECC(&Corretable_ECC);
						//check if read retry happens
						if(Corretable_ECC == pNandDrv->u16_ECCCorretableBit + 1)
							ReadRetryHappens = 1;
						if(Corretable_ECC >= pNandDrv->u16_ECCCorretableBit * 3 / 4)
							OverThreshold = 1;
				
						ECCSum += Corretable_ECC;
					}
					p_buffer += pNandDrv->u16_PageByteCnt;
				}
				//Trigger Read Retry backup process
				if(ReadRetryHappens == 1)
					RefreshBlk(nand, u32_Row, BlkPageCnt, 1);
				//Trigger Too many correctable ecc bits handling
				if(OverThreshold == 1)
					RefreshBlk(nand, u32_Row, BlkPageCnt, 0);
				PageCnt -= PageCnt;
			}	
		}
		else
			rval = nand_read (nand, offset, length, buffer);
		if (!rval || rval == -EUCLEAN)
			return (ECCSum || rval == -EUCLEAN) ? -EUCLEAN : 0;

		*length = 0;
		printf ("NAND read from offset %llx failed %d\n",
			offset, rval);
		return rval;
	}

	while (left_to_read > 0) {
		size_t read_length;

		WATCHDOG_RESET ();

		if (nand_block_isbad (nand, offset & ~(nand->erasesize - 1))) {
			printf ("Skipping bad block 0x%08llx\n",
				offset & ~(nand->erasesize - 1));
			offset += nand->erasesize;
			continue;
		}

		if (left_to_read < blocksize)
			read_length = left_to_read;
		else
			read_length = blocksize;
		if(pNandDrv->u8_CellType)
		{
			int i;
			ulong u32_TmpRow;
			ReadRetryHappens = 0;
			OverThreshold = 0;
			u32_Row = offset >> pNandDrv->u8_BlkPageCntBits;
			//Once a block or remaining pages
			for(i = 0; i < read_length >> pNandDrv->u8_PageByteCntBits; i++)
			{
				#if defined(FCIE_LFSR) && FCIE_LFSR
				if(pNandDrv->u8_RequireRandomizer == 1)
					NC_EnableLFSR();
				#endif
				u32_TmpRow = u32_Row + ga_tPairedPageMap[i].u16_LSB;
				Disable_Err_log = 1;
				rval = NC_ReadPages(u32_TmpRow, p_buffer, p_Spare, 1);
				Disable_Err_log = 0;
				if(rval)
				{
					if(CheckEmptyPageFalseAlarm(p_buffer,p_Spare) == 1)
						ECCSum += 1;
					else
					{
						*length = 0;
						printf ("NAND read to offset %llx failed %d\n",
							offset, rval);
						return rval;
					}
				}
				else
				{
					//check ecc code
					NC_CheckECC(&Corretable_ECC);
					//check if read retry happens
					if(Corretable_ECC == pNandDrv->u16_ECCCorretableBit + 1)
						ReadRetryHappens = 1;
					if(Corretable_ECC >= pNandDrv->u16_ECCCorretableBit * 3 / 4)
						OverThreshold = 1;

					ECCSum += Corretable_ECC;
				}

				p_buffer += pNandDrv->u16_PageByteCnt;
			}
			//Trigger Read Retry backup process
			if(ReadRetryHappens == 1)
				RefreshBlk(nand, u32_Row, blocksize >> pNandDrv->u8_PageByteCntBits, 1);
			//Trigger Too many correctable ecc bits handling
			if(OverThreshold == 1)
				RefreshBlk(nand, u32_Row, blocksize >> pNandDrv->u8_PageByteCntBits, 0);
			offset += read_length *2;
		}
		else
		{
			rval = nand_read (nand, offset, &read_length, p_buffer);
			if (rval && rval != -EUCLEAN) {
				printf ("NAND read from offset %llx failed %d\n",
					offset, rval);
				*length -= left_to_read;
				return rval;
			}
			offset       += read_length;
			p_buffer     += read_length;
		}

		left_to_read -= read_length;
	}

	return (ECCSum) ? -EUCLEAN : 0;
}

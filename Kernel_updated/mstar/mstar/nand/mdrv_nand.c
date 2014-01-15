#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <linux/semaphore.h>
#include <linux/version.h>
#include <asm/ptrace.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
extern int add_mtd_partitions(struct mtd_info *, const struct mtd_partition *, int);
#endif


#if defined(CONFIG_ARM)
#include <asm/arm-boards/prom.h>
#include <asm/mach/map.h>

#include <linux/dma-mapping.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <mstar/mstar_chip.h>
#endif


#if	defined(CONFIG_MSTAR_TITANIA4) || defined(CONFIG_MSTAR_JANUS) || defined(CONFIG_MSTAR_URANUS4) || \
	defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA9) || defined(CONFIG_MSTAR_JANUS2) || \
	defined(CONFIG_MSTAR_TITANIA11) || defined(CONFIG_MSTAR_TITANIA12) || defined(CONFIG_MSTAR_TITANIA13) || \
	defined(CONFIG_MSTAR_AMBER1) || defined(CONFIG_MSTAR_AMBER2) || defined(CONFIG_MSTAR_AMBER5) || \
	defined(CONFIG_MSTAR_KRONUS) || defined(CONFIG_MSTAR_AMBER7) || defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AMETHYST) || defined(CONFIG_MSTAR_AMBER3) || defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EAGLE) || defined(CONFIG_MSTAR_EMERALD) || defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL) || defined(CONFIG_MSTAR_EINSTEIN)

#define NAND_USE_UNFD					1

#define DMA_MODE 							1


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
#define MSTAR_W_R_FUNC						1
#define CONFIG_MSTAR_NAND_MAX_CACHE 3 
//#define CONFIG_MSTAR_NAND_PAGE_CACHE        1
#else
#define MSTAR_W_R_FUNC						0
#endif

#endif

#if defined(NAND_USE_UNFD)
#include "inc/common/drvNAND.h"
#include <linux/list.h>

#define MEASURE_PERFORMANCE				1
#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
#include <linux/proc_fs.h>
uint64_t	u64_TotalWriteBytes;
uint64_t	u64_TotalReadBytes;
struct proc_dir_entry * writefile;
const char procfs_name[] = "StorageBytes";
#endif

U32	u32CurRow = 0;
U32 u32CurCol = 0;
U32 u32CIFDIdx = 0;
U16 u16ByteIdxofPage = 0;
U32 u32WriteLen = 0;
S32 s32_ECCStatus = 0;

U8 u8MainBuf[16384] UNFD_ALIGN1;
U8 u8SpareBuf[1280] UNFD_ALIGN1;
#define MSTAR_BLK_FREE_PAGE_CHECK	1

#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
U16 *pu16_FreePagePos;
#endif

#define MSTAR_PARTIAL_READ	1

#if defined(MSTAR_PARTIAL_READ) && MSTAR_PARTIAL_READ
#define MSTAR_READ_FROM_COL	1
#endif


int sectoridx;

#ifdef CONFIG_MSTAR_NAND_MAX_CACHE

struct mstar_read_buf {
	struct list_head list;
	int pagebuf;
#if defined(MSTAR_PARTIAL_READ) && MSTAR_PARTIAL_READ
	int col;
	int bytelen;
#endif
	uint8_t *databuf;
};

struct list_head rcache;

#define CONFIG_MSTAR_NAND_MAX_CACHE_WRITE	5
#ifdef	CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
struct mstar_write_cache_buf{
	struct list_head list;
	int pagebuf;
	uint8_t *databuf;
};
struct list_head wcache;
#endif

#endif

#ifdef  CONFIG_MSTAR_NAND_PAGE_CACHE
typedef struct
{
    uint8_t *databuf;
	int do_page_cache;
	int len;
	int offset;
}nand_page_cache_trace;

#define NAND_READ_CHUNK_LENG 1024
#define MAX_PAGE_CACHE_ENTRY 128
#define NAND_PAGE_SIZE_4K 4096
#define NAND_PAGE_SIZE_8K 8192
#define PAGE_CACHE_DEBUG 0
/* Define trace levels. */
#define pAGE_CACHE_DEBUG_LEVEL_ERROR			(1)    /* Error condition debug messages. */
#define pAGE_CACHE_DEBUG_LEVEL_HIGH				(2)    /* Debug messages (high debugging). */
#define pAGE_CACHE_DEBUG_LEVEL_MEDIUM			(3)    /* Debug messages. */
#define pAGE_CACHE_DEBUG_LEVEL_LOW				(4)    /* Debug messages (low debugging). */

#if defined(PAGE_CACHE_DEBUG) && PAGE_CACHE_DEBUG
#define page_cache_debug(dbg_lv, str, ...)	         \
	do {	                                            \
		if (dbg_lv > pAGE_CACHE_DEBUG_LEVEL_ERROR)     \
			break;									    \
		else {                                          \
			printk(KERN_ERR str, ##__VA_ARGS__);		\
		} \
	} while(0)
#else 
#define page_cache_debug(enable, str, ...)	do{}while(0)
#endif 

static struct list_head page_cache_chain;
static struct radix_tree_root radix_tree_root;
static unsigned long		nr_cache_pages;// number of total pages cached 
static nand_page_cache_trace cache_trace;
static struct page ** cache_page = NULL;
static int nand_page_size = 4096;
static DEFINE_MUTEX(nand_cache_mutex);

#if defined(PAGE_CACHE_DEBUG) && PAGE_CACHE_DEBUG
static unsigned long total_read_num =0;   //read access times
static unsigned long total_write_num =0;  			 //write access times
static unsigned long total_read_hit_num =0;   		 //read page hit cache
static unsigned long total_write_hit_num =0;   //write page hit cache
#endif
#endif

#else
#include "drvNAND.h"
#endif

//#define NAND_DEBUG(fmt, args...)			printk(fmt, ##args)
#define NAND_DEBUG(fmt, args...)

#define CACHE_LINE	0x80

extern struct semaphore					PfModeSem;
#define PF_MODE_SEM(x)					(x)

#ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
struct page *nand_find_get_page(struct radix_tree_root *root, pgoff_t offset)
{
	void **pagep;
	struct page *page;  

	rcu_read_lock();
repeat:
	page = NULL;
	pagep = radix_tree_lookup_slot(root, offset);
	if (pagep) {
		page = radix_tree_deref_slot(pagep);
		if (unlikely(!page))
			goto out;
		if (radix_tree_deref_retry(page))
			goto repeat;
	}
out:
	rcu_read_unlock();

	return page;
}

unsigned nand_find_get_pages(struct radix_tree_root *root, pgoff_t start,unsigned int nr_pages, struct page **pages)
{
	unsigned int i;
	unsigned int nr_found = 0;
    struct page * temp = NULL;
    page_cache_debug(pAGE_CACHE_DEBUG_LEVEL_LOW, "find get pages %lu\n", start);

    for(i=0; i<nr_pages; ++i)
    {
        temp = nand_find_get_page(root, start+i);
        if(temp != NULL)
        {
           pages[nr_found] = temp;
           ++nr_found;
        }
    }

    return nr_found;
}

int nand_add_to_radixtree(struct radix_tree_root *root, struct page *page, pgoff_t offset, gfp_t gfp_mask)
{
	int error;    
    
    mutex_lock(&nand_cache_mutex);
	error = radix_tree_preload(gfp_mask & ~__GFP_HIGHMEM);
	if (error == 0) {
		page->index = offset;
		error = radix_tree_insert(root, offset, page);
        if(error == 0){
            list_add_tail(&(page->lru), &page_cache_chain);	
            ++nr_cache_pages;
        }
		radix_tree_preload_end();
	}
    mutex_unlock(&nand_cache_mutex);

    if(error == 0)
       page_cache_debug(pAGE_CACHE_DEBUG_LEVEL_MEDIUM, "add to tree %lu\n", offset);

    return error;
}

void nand_remove_from_radixtree(struct radix_tree_root *root, struct page *page)
{
	mutex_lock(&nand_cache_mutex);
	if(radix_tree_delete(root, page->index) != NULL)
    {    
       list_del(&(page->lru));
       nr_cache_pages--;
       __free_page(page);  
       page_cache_debug(pAGE_CACHE_DEBUG_LEVEL_MEDIUM, "remove from tree %lu\n", page->index);
    }
	mutex_unlock(&nand_cache_mutex);  
}

static int nand_pagecache_shrink(struct shrinker *s, int nr_to_scan, gfp_t gfp_mask)
{
    unsigned long num_del = 0;
    struct page * entry = NULL, * pos = NULL;  

    page_cache_debug(pAGE_CACHE_DEBUG_LEVEL_ERROR, "emmc pagecache shrink: nr_to_scan %d, nr_cache_pages %lu\n", nr_to_scan, nr_cache_pages);

    page_cache_debug(pAGE_CACHE_DEBUG_LEVEL_ERROR, "total_read_num %lu total_write_num %lu\n", total_read_num, total_write_num);
	page_cache_debug(pAGE_CACHE_DEBUG_LEVEL_ERROR, "total_read_hit_num %lu total_write_hit_num %lu\n", total_read_hit_num, total_write_hit_num);

    if(nr_to_scan <= 0)
    {
        return nand_page_size==4096? nr_cache_pages : 2*nr_cache_pages;
    }
	else
	{
	    if(nand_page_size==8192)
		   nr_to_scan /= 2;			
	}

	mutex_lock(&nand_cache_mutex);
    list_for_each_entry_safe(entry, pos, &page_cache_chain, lru)    
    {
       if(num_del >= nr_to_scan)
           break;
       
       page_cache_debug(pAGE_CACHE_DEBUG_LEVEL_LOW, "to del index  %lu\n", entry->index);       
	   if(radix_tree_delete(&radix_tree_root, entry->index) != NULL)
       {
          list_del(&(entry->lru));      
          __free_page(entry);       
          nr_cache_pages--; 
          ++num_del;
       }
       else
          printk(KERN_ERR "radix tree delete page index %lu error\n", entry->index);
    }
	mutex_unlock(&nand_cache_mutex);  
	
    return  (nand_page_size==4096? nr_cache_pages : 2*nr_cache_pages);
}

static struct shrinker mmc_pagecache_shrinker = {
	.shrink = nand_pagecache_shrink,
	.seeks = DEFAULT_SEEKS
};
#endif


void nand_lock_fcie(void)
{
#if defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL) || \
	defined(CONFIG_MSTAR_EINSTEIN)
		
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

#endif

    //printk("nand_lock_fcie\n");

    PF_MODE_SEM(down(&PfModeSem));

    #if defined(CONFIG_MSTAR_URANUS4) || \
        defined(CONFIG_MSTAR_AMBER3) || \
        defined(CONFIG_MSTAR_EAGLE) || \
        defined(CONFIG_MSTAR_AGATE) || \
        defined(CONFIG_MSTAR_EDISON) || \
        defined(CONFIG_MSTAR_EIFFEL) || \
		defined(CONFIG_MSTAR_EINSTEIN)

    HAL_WRITE_UINT16(NC_PATH_CTL, BIT_NC_EN);	
	nand_clock_setting(pNandDrv->u32_Clk);
	nand_pads_switch(pNandDrv->u8_PadMode);
    NC_ResetFCIE();
	NC_Config();

    #endif
}

void nand_unlock_fcie(void)
{
    PF_MODE_SEM(up(&PfModeSem));

    //printk("nand_unlock_fcie\n");
}

extern unsigned int MDrv_SYS_GetMIU0Length(void);

/* These really don't belong here, as they are specific to the NAND Model */
static uint8_t scan_ff_pattern[] = { 0xff };

/* struct nand_bbt_descr - bad block table descriptor */
static struct nand_bbt_descr _titania_nand_bbt_descr = {
	.options		= NAND_BBT_2BIT | NAND_BBT_LASTBLOCK | NAND_BBT_VERSION | NAND_BBT_CREATE | NAND_BBT_WRITE,
	.offs			= 5,
	.len			= 1,
	.pattern		= scan_ff_pattern
};

static struct nand_ecclayout unfd_nand_oob_custom;

static uint8_t bbt_pattern[] = {'B', 'b', 't', '0' };
static uint8_t mirror_pattern[] = {'1', 't', 'b', 'B' };

static struct nand_bbt_descr _titania_bbt_main_descr = {
	.options		= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE |
					  NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs			= 1,
	.len			= 3,
	.veroffs		= 4,
	.maxblocks		= NAND_BBT_BLOCK_NUM,
	.pattern		= bbt_pattern
};

static struct nand_bbt_descr _titania_bbt_mirror_descr = {
	.options		= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE |
					  NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs			= 1,
	.len			= 3,
	.veroffs		= 4,
	.maxblocks		= NAND_BBT_BLOCK_NUM,
	.pattern		= mirror_pattern
};

static struct nand_ecclayout titania_nand_oob_16 = {
	.eccbytes		= 10,
	.eccpos			= {6,7,8,9,10,11,12,13,14,15},
	.oobfree		=
	{
		{
			.offset = 0,
			.length = 5
		}
	}
};

static struct nand_ecclayout titania_nand_oob_64 = {
    .eccbytes		= 40,
    .eccpos			=
    {
    	0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    	0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    	0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    	0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F
    },
    .oobfree		=
    {
    	{
    		.offset = 1,
    		.length = 5
    	},
        {
        	.offset = 0x10,
        	.length = 6
        },
        {
        	.offset = 0x20,
        	.length = 6
        },
        {
        	.offset = 0x30,
        	.length = 6
        }
    }
};

#if defined(CONFIG_MTD_CMDLINE_PARTS)
extern int parse_cmdline_partitions(struct mtd_info *master, struct mtd_partition **pparts, char *);
#endif

#define MSTAR_NAND_SIZE						0x4000000

#define SZ_256KB							(256 * 1024)
#define SZ_512KB							(512 * 1024)
#define SZ_2MB								(2 * 1024 * 1024)
#define SZ_4MB								(4 * 1024 * 1024)
#define SZ_7MB								(7 * 1024 * 1024)
#define SZ_8MB								(8 * 1024 * 1024)
#define SZ_16MB								(16 * 1024 * 1024)
#define SZ_32MB								(32 * 1024 * 1024)

#define NAND_PART_PARTITION_TBL_OFFSET		0
#define NAND_PART_PARTITION_TBL_SIZE		SZ_256KB

#define NAND_PART_LINUX_BOOT_PARAM_OFFSET	(NAND_PART_PARTITION_TBL_OFFSET + NAND_PART_PARTITION_TBL_SIZE)
#define NAND_PART_LINUX_BOOT_PARAM_SIZE		SZ_256KB

#define NAND_PART_KERNEL_OFFSET				(NAND_PART_LINUX_BOOT_PARAM_OFFSET + NAND_PART_LINUX_BOOT_PARAM_SIZE)
#define NAND_PART_KERNEL_SIZE				SZ_4MB

#define NAND_PART_CHAKRA_BOOT_PARAM_OFFSET	(NAND_PART_KERNEL_OFFSET + NAND_PART_KERNEL_SIZE)
#define NAND_PART_CHAKRA_BOOT_PARAM_SIZE	SZ_512KB

#define NAND_PART_CHAKRA_BIN_OFFSET			(NAND_PART_CHAKRA_BOOT_PARAM_OFFSET + NAND_PART_CHAKRA_BOOT_PARAM_SIZE)
#define NAND_PART_CHAKRA_BIN_PARAM_SIZE		SZ_8MB

#define NAND_PART_ROOTFS_OFFSET				(NAND_PART_CHAKRA_BIN_OFFSET + NAND_PART_CHAKRA_BIN_PARAM_SIZE)
#define NAND_PART_ROOTFS_SIZE				SZ_4MB

#define NAND_PART_CONF_OFFSET				(NAND_PART_ROOTFS_OFFSET + NAND_PART_ROOTFS_SIZE)
#define NAND_PART_CONF_SIZE					SZ_256KB

#define NAND_PART_SUBSYSTEM_OFFSET			(NAND_PART_CONF_OFFSET + NAND_PART_CONF_SIZE)
#define NAND_PART_SUBSYSTEM_SIZE			SZ_2MB

#define NAND_PART_FONT_OFFSET				(NAND_PART_SUBSYSTEM_OFFSET + NAND_PART_SUBSYSTEM_SIZE)
#define NAND_PART_FONT_SIZE					SZ_4MB

#define NAND_PART_OPT_OFFSET				(NAND_PART_FONT_OFFSET + NAND_PART_FONT_SIZE)
#define NAND_PART_OPT_SIZE					SZ_8MB

#define NAND_PART_APPLICATION_OFFSET		(NAND_PART_OPT_OFFSET + NAND_PART_OPT_SIZE)
#define NAND_PART_APPLICATION_SIZE			(MSTAR_NAND_SIZE - NAND_PART_APPLICATION_OFFSET)

#if ( (NAND_PART_APPLICATION_OFFSET) >= MSTAR_NAND_SIZE)
    #error "Error: NAND partition is not correct!!!"
#endif

static const struct mtd_partition partition_info[] =
{
    {
    	.name   = "nand_partition_tbl",
    	.offset = NAND_PART_PARTITION_TBL_OFFSET,
        .size   = NAND_PART_PARTITION_TBL_SIZE
    },
    {
        .name   = "linux_boot_param",
        .offset = NAND_PART_LINUX_BOOT_PARAM_OFFSET,
        .size   = NAND_PART_LINUX_BOOT_PARAM_SIZE
    },
    {
        .name   = "kernel",
        .offset = NAND_PART_KERNEL_OFFSET,
        .size   = NAND_PART_KERNEL_SIZE
    },
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
        .name   = "rootfs",
        .offset = NAND_PART_ROOTFS_OFFSET,
        .size   = NAND_PART_ROOTFS_SIZE
    },
    {
		.name   = "conf",
		.offset = NAND_PART_CONF_OFFSET,
		.size   = NAND_PART_CONF_SIZE
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
};

#define NUM_PARTITIONS ARRAY_SIZE(partition_info)

static struct mtd_info *nand_mtd = NULL;

static void _titania_nand_hwcontrol(struct mtd_info *mtdinfo, int cmd, unsigned int ctrl)
{
    NAND_DEBUG("NANDDrv_HWCONTROL()\n");

    if(ctrl & NAND_CTRL_CHANGE)
    {
		if(ctrl & NAND_NCE)
		{
			NAND_DEBUG("NAND_CTL_SETNCE\n");

			//PF_MODE_SEM(down(&PfModeSem));
            nand_lock_fcie();

			#if defined(NAND_USE_UNFD)
			//drvNAND_SetCEZ(0x00);	// Let FCIE3 control the CE
			#else
			MDrv_NAND_Set_Chip_Select(0x0);
			#endif

			//PF_MODE_SEM(up(&PfModeSem));
			nand_unlock_fcie();
		}
		else
		{
			NAND_DEBUG("NAND_CTL_CLRNCE\n");

			//PF_MODE_SEM(down(&PfModeSem));
			nand_lock_fcie();

			#if defined(NAND_USE_UNFD)
			//drvNAND_SetCEZ(0x1);	// Let FCIE3 control the CE
			#else
			MDrv_NAND_Set_Chip_Select(0x1);
			#endif

			//PF_MODE_SEM(up(&PfModeSem));
			nand_unlock_fcie();
		}

		if(ctrl & NAND_CLE)
        {
			NAND_DEBUG("NAND_CTL_SETCLE\n");
			// We have no way to control CLE in NFIE/FCIE2/FCIE3
		}
		else
		{
			NAND_DEBUG("NAND_CTL_CLRCLE\n");
			// We have no way to control CLE in NFIE/FCIE2/FCIE3
		}

		if(ctrl & NAND_ALE)
		{
			NAND_DEBUG("NAND_CTL_SETALE\n");
			// We have no way to control ALE in NFIE/FCIE2/FCIE3
		}
		else
		{
			NAND_DEBUG("NAND_CTL_CLRALE\n");
			// We have no way to control ALE in NFIE/FCIE2/FCIE3
		}
	}

	#if !defined(NAND_USE_UNFD)
	if(cmd != NAND_CMD_NONE)
    {
    	//PF_MODE_SEM(down(&PfModeSem));
    	nand_lock_fcie();
        MDrv_NAND_SendCmd(cmd);
		//PF_MODE_SEM(up(&PfModeSem));
		nand_unlock_fcie();
    }
    #endif
}

static int _titania_nand_device_ready(struct mtd_info *mtdinfo)
{
	NAND_DEBUG("NANDDrv_DEVICE_READY()\n");

	return 1;
}

static void _titania_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	#ifdef CONFIG_MS_NAND_MIU1_PATCH
	void *tmpBuf;
	int count = 0;
	unsigned int _MIU0_LEN=MDrv_SYS_GetMIU0Length();
	#endif

    NAND_DEBUG("NANDDrv_WRITE_BUF() 0x%X\r\n",len);

	//PF_MODE_SEM(down(&PfModeSem));
	nand_lock_fcie();

	#ifdef CONFIG_MS_NAND_MIU1_PATCH
    if( ((unsigned long)VA2PA(buf)) >= _MIU0_LEN  )
    {
    	NAND_DEBUG("NAND write DMA for MIU1\n");

    	if( (tmpBuf = kmalloc(len, GFP_ATOMIC | GFP_DMA)) == 0 )
    	{
    		printk(KERN_ERR "No memory for NAND write\n");
    		return;
    	}

    	while( (((unsigned long)VA2PA(tmpBuf)) >= _MIU0_LEN) || (count > 0x3FFF) )
    	{
    		printk(KERN_ERR "No memory for MIU0 NAND write\n");
    		kfree(tmpBuf);
    		tmpBuf = 0;
    		count++;
    	}

    	if( tmpBuf != 0 )
    	{
    		NAND_DEBUG("W buf = %p, tmpBuf = %p, len = %X\n", VA2PA(buf), VA2PA(tmpBuf), len);
    		memcpy(tmpBuf, buf, len);
    		MDrv_NAND_WriteBuf((U8 *)tmpBuf, len);
    		kfree(tmpBuf);
    	}
    }
    else
	#endif
    {
    	#if defined(NAND_USE_UNFD)

		NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
		U8 *pu8MainBuf;
		U8 *pu8SpareBuf;

		if( len >= pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt)	// whole page or Main area only
		{
			if( len > pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt )	// whole page
			{
                #if defined(CONFIG_MIPS)
				if( ((U32)buf) >= 0xC0000000 || ((U32)buf) % CACHE_LINE) // For MIU1
                #elif defined(CONFIG_ARM)
				if(!virt_addr_valid((U32)buf) || !virt_addr_valid((U32)buf + (U32)len - 1) ) // For High MEM (ARM)
                #endif
				{
					memcpy(u8MainBuf, buf, pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
					memcpy(u8SpareBuf, buf + (pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt), pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
					pu8MainBuf = u8MainBuf;
					pu8SpareBuf = u8SpareBuf;
				}
				else	// For MIU0
				{
					pu8MainBuf = (U8*)buf;
					pu8SpareBuf = (U8*)buf + (pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
				}

				#if defined(FCIE_LFSR) && FCIE_LFSR
				if(pNandDrv->u8_RequireRandomizer == 1)
					NC_EnableLFSR();
				#endif

                #if DMA_MODE
				if(pNandDrv->u8_PlaneCnt > 1)
					NC_WritePages2P(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
				else
				  NC_WritePages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
                #else
				  NC_WritePage_RIUMode(u32CurRow, pu8MainBuf, pu8SpareBuf);
                #endif
			}
			else	// main area only
			{
				memcpy(u8MainBuf, buf, len);
				u32WriteLen += len;
				u16ByteIdxofPage += len;
			}
		}
		else
		{
			if((u32WriteLen==0) && (u16ByteIdxofPage>=pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt)) // mtd skip prepare main area, default all oxff
			{
				memset(u8MainBuf, 0xFF, pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
				memset(u8SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
			}

			memcpy(u8SpareBuf, buf, len);
			u32WriteLen += len;
			u16ByteIdxofPage += len;

			if( (u32WriteLen == ((pNandDrv->u16_PageByteCnt + pNandDrv->u16_SpareByteCnt) *pNandDrv->u8_PlaneCnt)) ||
                        (u32WriteLen ==  pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt))
			{
				#if defined(FCIE_LFSR) && FCIE_LFSR
				if(pNandDrv->u8_RequireRandomizer == 1)
					NC_EnableLFSR();
				#endif
                #if DMA_MODE
				if(pNandDrv->u8_PlaneCnt > 1)
					NC_WritePages2P(u32CurRow, u8MainBuf, u8SpareBuf, 1);
				else
				  NC_WritePages(u32CurRow, u8MainBuf, u8SpareBuf, 1);
                #else
				  NC_WritePage_RIUMode(u32CurRow, u8MainBuf, u8SpareBuf);
                #endif
				#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
				u64_TotalWriteBytes += pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt;
				#endif
		}
		}

    	#else
    	MDrv_NAND_WriteBuf((U8 *const)buf, len);
    	#endif
    }

	//PF_MODE_SEM(up(&PfModeSem));
	nand_unlock_fcie();

    return;
}

static void _titania_nand_read_buf(struct mtd_info *mtd, u_char* const buf, int len)
{
	#ifdef CONFIG_MS_NAND_MIU1_PATCH
	void *tmpBuf;
	int count = 0;
	unsigned int _MIU0_LEN=MDrv_SYS_GetMIU0Length();
	#endif

	//PF_MODE_SEM(down(&PfModeSem));
	nand_lock_fcie();

    NAND_DEBUG("NANDDrv_READ_BUF()0x%X\n",len);

	#ifdef CONFIG_MS_NAND_MIU1_PATCH
    if( ((unsigned long)VA2PA(buf)) > _MIU0_LEN )
    {
    	NAND_DEBUG("NAND read DMA for MIU1\n");

    	if( (tmpBuf = kmalloc(len, GFP_ATOMIC | GFP_DMA)) == 0 )
    	{
    		printk(KERN_ERR "No memory for NAND write\n");
    		return;
    	}

    	while( (((unsigned long)VA2PA(tmpBuf)) >= _MIU0_LEN) || (count > 0x3FFF) )
    	{
    		printk(KERN_ERR "No memory for MIU0 NAND write\n");
    		kfree(tmpBuf);
    		tmpBuf = 0;
    		count++;
    	}

    	if( tmpBuf != 0 )
    	{
    		NAND_DEBUG("R buf = %p, tmpBuf = %p, len = %X\n", VA2PA(buf), VA2PA(tmpBuf), len);
    		MDrv_NAND_ReadBuf((U8*)tmpBuf, len);
    		memcpy(buf, (U8*)tmpBuf, len);
    		kfree(tmpBuf);
    	}
    }
    else
	#endif
    {
    	#if defined(NAND_USE_UNFD)

		NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
		U8 *pu8MainBuf;
		U8 *pu8SpareBuf;
		U32 u32_Err = 0;

		u16ByteIdxofPage = len;

		if( len >= pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt)
		{
			if( len > pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt)
			{
                #if defined(CONFIG_MIPS)
				if( ((U32)buf) >= 0xC0000000 || ((U32)buf) % CACHE_LINE )	// For
                #elif defined(CONFIG_ARM)
   			    if(!virt_addr_valid((U32)buf) || !virt_addr_valid((U32)buf + (U32)len - 1) ) // For High MEM (ARM)
                #endif
				{
					pu8MainBuf = u8MainBuf;
					pu8SpareBuf = u8SpareBuf;
				}
				else	// For MIU0
				{
					pu8MainBuf = buf;
					pu8SpareBuf = (U8*)buf + (pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
				}

                #if DMA_MODE
				
				#if defined(FCIE_LFSR) && FCIE_LFSR
				if(pNandDrv->u8_RequireRandomizer == 1)
					NC_EnableLFSR();
				#endif

				memset(pu8SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
				if(pNandDrv->u8_PlaneCnt > 1)
				{
					u32_Err = NC_ReadPages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
					u32_Err = NC_ReadPages(u32CurRow + pNandDrv->u16_BlkPageCnt, pu8MainBuf + pNandDrv->u16_PageByteCnt, pu8SpareBuf + pNandDrv->u16_SpareByteCnt, 1);
				}
				else
					u32_Err = NC_ReadPages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
                #else
				if(pNandDrv->u8_PlaneCnt > 1)
				{
					nand_debug(0, 1, "Multi plane Read does not support RIU mode\n");
					nand_die();
				}
				else
				{
				  for(sectoridx = 0; sectoridx < pNandDrv->u16_PageSectorCnt; sectoridx ++)
					  NC_ReadSector_RIUMode(u32CurRow, sectoridx, pu8MainBuf + sectoridx*pNandDrv->u16_SectorByteCnt, 
					  		pu8SpareBuf+ sectoridx*pNandDrv->u16_SectorSpareByteCnt);
				}
                #endif
				NC_CheckECC((int*)(&s32_ECCStatus));
				if(u32_Err != UNFD_ST_SUCCESS)
					s32_ECCStatus = -1;

				#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
				u64_TotalReadBytes += pNandDrv->u8_PlaneCnt * pNandDrv->u16_PageByteCnt;
				#endif
				if( buf != pu8MainBuf ) // If MIU1, copy data from temp buffer
				{
					memcpy(buf, u8MainBuf, pNandDrv->u16_PageByteCnt);
					memcpy((U8*)buf + (pNandDrv->u16_PageByteCnt), u8SpareBuf, pNandDrv->u16_SpareByteCnt);
				}
			}
			else
			{
                #if defined(CONFIG_MIPS)
				if( ((U32)buf) >= 0xC0000000 || ((U32)buf) % CACHE_LINE)	// For MIU1
                #elif defined(CONFIG_ARM)
				//hard code temporily
				if( !virt_addr_valid((U32)buf) || !virt_addr_valid((U32)buf + (U32)len - 1) ) // For High MEM (ARM)
                #endif
				{
					pu8MainBuf = u8MainBuf;
				}
				else	// For MIU0
				{
					pu8MainBuf = buf;
				}
				pu8SpareBuf = u8SpareBuf;	// Preserve spare data in temp buffer for next read_buf()

                #if DMA_MODE
				
				#if defined(FCIE_LFSR) && FCIE_LFSR
				if(pNandDrv->u8_RequireRandomizer == 1)
					NC_EnableLFSR();
				#endif

				memset(pu8SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt * pNandDrv->u8_PlaneCnt);
				if(pNandDrv->u8_PlaneCnt > 1)
				{
					u32_Err = NC_ReadPages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
					u32_Err = NC_ReadPages(u32CurRow + pNandDrv->u16_BlkPageCnt, pu8MainBuf + pNandDrv->u16_PageByteCnt, pu8SpareBuf + pNandDrv->u16_SpareByteCnt, 1);
				}
				else
					u32_Err = NC_ReadPages(u32CurRow, pu8MainBuf, pu8SpareBuf, 1);
                #else
				if(pNandDrv->u8_PlaneCnt > 1)
				{
					nand_debug(0, 1, "Multi plane Read does not support RIU mode\n");
					nand_die();
				}
				else
				{
				  for(sectoridx = 0; sectoridx < pNandDrv->u16_PageSectorCnt; sectoridx ++)
						  NC_ReadSector_RIUMode(u32CurRow, sectoridx, pu8MainBuf + sectoridx*pNandDrv->u16_SectorByteCnt,
						  		pu8SpareBuf+ sectoridx*pNandDrv->u16_SectorSpareByteCnt);
				}
                #endif
				NC_CheckECC((int *)(&s32_ECCStatus));
				if(u32_Err != UNFD_ST_SUCCESS)
					s32_ECCStatus = -1;

				#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
				u64_TotalReadBytes += pNandDrv->u8_PlaneCnt * pNandDrv->u16_PageByteCnt;
				#endif
				if( buf != pu8MainBuf )	// If MIU1, copy data from temp buffer
					memcpy(buf, u8MainBuf, pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);
			}
		}
		else
		{
			memcpy(buf, u8SpareBuf, len);
		}

		if(pNandDrv->u8_PlaneCnt > 1)
		{
			if(u8SpareBuf[0] != 0xFF|| u8SpareBuf[pNandDrv->u16_SpareByteCnt] != 0xFF)
			{
				u8SpareBuf[0] = u8SpareBuf[pNandDrv->u16_PageByteCnt] = 0;
			}
		}

    	#else
    	MDrv_NAND_ReadBuf(buf, len);
    	#endif
    }

	//PF_MODE_SEM(up(&PfModeSem));
	nand_unlock_fcie();

    return;
}

static u16 _titania_nand_read_word(struct mtd_info *mtd)
{
    NAND_DEBUG("NANDDrv_READ_WORD()\n");
    return 0;
}

static u_char _titania_nand_read_byte(struct mtd_info *mtd)
{
    U8 u8Ret = 0;

    NAND_DEBUG("NANDDrv_READ_BYTE()\n");

	#if defined(NAND_USE_UNFD)
	NC_GetCIFD(&u8Ret, u32CIFDIdx, 1);
	u32CIFDIdx++;
	#else
    u8Ret = MDrv_NAND_ReadByte();
	#endif

    return (u8Ret);
}

/**
 * nand_wait - [DEFAULT]  wait until the command is done
 * @mtd:	MTD device structure
 * @this:	NAND chip structure
 * @state:	state to select the max. timeout value
 *
 * Wait for command done. This applies to erase and program only
 * Erase can take up to 400ms and program up to 20ms according to
 * general NAND and SmartMedia specs
 *
*/

static int _titania_nand_wait(struct mtd_info *mtd, struct nand_chip *this)
{
    NAND_DEBUG("NANDDrv_WAIT()\n");

    #if defined(NAND_USE_UNFD)
	return REG(NC_ST_READ);
	#else
	//PF_MODE_SEM(down(&PfModeSem));
	nand_lock_fcie();
    MDrv_NAND_WaitReady();
	MDrv_NAND_Send_Read_Status_CMD(NAND_CMD_STATUS);
	//PF_MODE_SEM(up(&PfModeSem));
	nand_unlock_fcie();
    return (MDrv_NAND_ReadByte());
	#endif
}

static void _titania_nand_cmdfunc(struct mtd_info *mtd, unsigned command,
                        int column, int page_addr)
{
	#if defined(NAND_USE_UNFD)
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	struct nand_chip *chip =(struct nand_chip*)(mtd->priv);
	int LBA, LPA;
	#endif

    //PF_MODE_SEM(down(&PfModeSem));
    nand_lock_fcie();

    #if defined(NAND_USE_UNFD)
	if(pNandDrv->u8_PlaneCnt > 1)
	{
		LBA = page_addr >> (chip->phys_erase_shift - chip->page_shift);
        LPA = page_addr & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
        page_addr = ((LBA<<1)<<pNandDrv->u8_BlkPageCntBits)+LPA;
	}
	#else
	_fsinfo.u16NC_CFG0 &= ~(R_NC_CE_AUTO+R_NC_WP_AUTO);
    _fsinfo.u16NC_CFG0 |= (R_NC_CE_EN+R_NC_WP_EN);
    HAL_WRITE_UINT16(NC_CFG0,_fsinfo.u16NC_CFG0);
	#endif

	switch (command) {
        case NAND_CMD_READ0:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READ0, page_addr: 0x%x, column: 0x%x.\n", page_addr, (column>>1));

            #if defined(NAND_USE_UNFD)
			u32CurRow = page_addr;
			u32CurCol = column;
			#else
            MDrv_NAND_SendCmd(NAND_CMD_READ0);
            MDrv_NAND_SendAdr(page_addr,column);
            if(mtd->oobsize > 16)
            {
                MDrv_NAND_SendCmd(NAND_CMD_READSTART);
            }
            #endif
			break;

        case NAND_CMD_READ1:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READ1.\n");

            #if defined(NAND_USE_UNFD)
			panic("UNFD not support READ1(CMD 0x01) now!!!\n");
			#else
			MDrv_NAND_SendCmd(NAND_CMD_READ1);
			MDrv_NAND_SendAdr(page_addr,column);
			#endif
			break;

        case NAND_CMD_READOOB:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READOOB.\n");

            #if defined(NAND_USE_UNFD)
			printk("\033[34mReadOOB (CMD 0x50)\033[m\n");
			u32CIFDIdx = 0;
			u16ByteIdxofPage = 0;
		#if defined(FCIE_LFSR) && FCIE_LFSR
		if(pNandDrv->u8_RequireRandomizer)
		{
			NC_EnableLFSR();
			NC_ReadPages(page_addr, u8MainBuf, u8SpareBuf, 1);
		}
		else
		#endif
			NC_Read_RandomIn(page_addr, column + pNandDrv->u16_PageByteCnt, u8SpareBuf, 
				(pNandDrv->u16_SpareByteCnt > 512)? 510 : pNandDrv->u16_SpareByteCnt);
			#else
			MDrv_NAND_ReadOOB(page_addr,column);
			#endif
			break;

        case NAND_CMD_READID:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READID.\n");

            #if defined(NAND_USE_UNFD)
			u32CIFDIdx = 0;
			NC_ReadID();
			#else
            drvNAND_READ_ID();
			#endif
			break;

        case NAND_CMD_PAGEPROG:
            /* sent as a multicommand in NAND_CMD_SEQIN */
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_PAGEPROG.\n");

            #if defined(NAND_USE_UNFD)
			// We have done page program in write_buf()
			#else
			NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_PAGEPROG.\n");
            MDrv_NAND_Cmd_PageProgram(NAND_CMD_PAGEPROG);
			#endif
            break;

        case NAND_CMD_ERASE1:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_ERASE1,  page_addr: 0x%x, column: 0x%x.\n", page_addr, column);

            #if defined(NAND_USE_UNFD)
			if(pNandDrv->u8_PlaneCnt > 1)
				NC_EraseBlk2P(page_addr);
			else
			NC_EraseBlk(page_addr);
			#else
			MDrv_NAND_Erase1(page_addr);
			#endif
			break;

        case NAND_CMD_ERASE2:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_ERASE2.\n");

            #if defined(NAND_USE_UNFD)
			// We do all erase function in Erase1 command.
			#else
            MDrv_NAND_Erase2();
			#endif
            break;

        case NAND_CMD_SEQIN:
            /* send PAGE_PROG command(0x1080) */
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_SEQIN/PAGE_PROG,  page_addr: 0x%x, column: 0x%x.\n", page_addr, column);

			#if defined(NAND_USE_UNFD)
			u32CurRow = page_addr;
            u32CurCol = column;
			u16ByteIdxofPage = column;
			u32WriteLen = 0;
			#else
			MDrv_NAND_WritePage(page_addr,column);
			#endif
			break;

        case NAND_CMD_STATUS:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_STATUS.\n");

            #if defined(NAND_USE_UNFD)
			u32CIFDIdx = 0;
            NC_ReadStatus();
			#else
			MDrv_NAND_Send_Read_Status_CMD(NAND_CMD_STATUS);
			#endif
			break;

        case NAND_CMD_RESET:
        	NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_RESET.\n");

            #if defined(NAND_USE_UNFD)
			NC_ResetNandFlash();
			#else
			drvNAND_FLASHRESET();
			#endif
			break;

        case NAND_CMD_STATUS_MULTI:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_STATUS_MULTI.\n");

            #if defined(NAND_USE_UNFD)
			u32CIFDIdx = 0;
			NC_ReadStatus();
			#else
			MDrv_NAND_Send_Read_Status_CMD(NAND_CMD_STATUS_MULTI);
			#endif
			break;

        case NAND_CMD_READSTART:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_READSTART.\n");

            #if defined(NAND_USE_UNFD)
			// We have done this command in NC_ReadPages()
			#else
			MDrv_NAND_SendCmd(NAND_CMD_READSTART);
			#endif
			break;

        case NAND_CMD_CACHEDPROG:
            NAND_DEBUG("_titania_nand_cmdfunc: NAND_CMD_CACHEDPROG.\n");

            #if defined(NAND_USE_UNFD)
			panic("UNFD not support CACHEPROG (CMD 0x15) now!!!\n");
			#else
			MDrv_NAND_Cmd_PageProgram(NAND_CMD_CACHEDPROG);
			#endif
			break;

        default:
            printk("_titania_nand_cmdfunc: error, unsupported command.\n");
            break;
	}

    //PF_MODE_SEM(up(&PfModeSem));
    nand_unlock_fcie();
}

static void _titania_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
    NAND_DEBUG("enable_hwecc\r\n");
    // default enable
}

static int _titania_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
    NAND_DEBUG("calculate_ecc\r\n");

	// NFIE/FCIE2/FCIE3 will calculate it.

	return 0;
}

static int _titania_nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
	#if defined(NAND_USE_UNFD)
	int s32ECCStatus = 0;
	#endif

    NAND_DEBUG("correct_data\r\n");

	#if defined(NAND_USE_UNFD)
	//	NC_CheckECC(&s32ECCStatus);
	s32ECCStatus = s32_ECCStatus;
	return s32ECCStatus;
	#else
	return(MDrv_NAND_CheckECC());
	#endif
}

#if defined(MSTAR_W_R_FUNC) && MSTAR_W_R_FUNC

extern int nand_get_device(struct nand_chip *chip, struct mtd_info *mtd,
               int new_state);
extern void nand_release_device(struct mtd_info *mtd);

extern uint8_t *nand_transfer_oob(struct nand_chip *chip, uint8_t *oob,
                  struct mtd_oob_ops *ops, size_t len);

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
static int mstar_nand_do_read_ops(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 u32_Err = 0;
	int i, j, ECCErrBitCnt, false_alarm = 1;
	int s32ECCStatus;
	int lba, lpa;
	int markbad = 0;
	int page, realpage, col, bytes, aligned;
	struct nand_chip *chip = mtd->priv;
	struct mtd_ecc_stats stats;
	uint32_t readlen = ops->len;
	uint32_t oobreadlen = ops->ooblen;
	uint32_t max_oobsize = ops->mode == MTD_OOB_AUTO ? mtd->oobavail : mtd->oobsize;
	uint16_t u16_SectorCnt;

	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	uint16_t u16_BlkIdx;
	#endif
	uint8_t *bufpoi, *oob, *buf, *oob_poi;
	uint8_t dma = 1;
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
	struct mstar_read_buf *readbuf;
	#endif

	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
	struct mstar_write_cache_buf *writebuf;
	#endif
	
	stats = mtd->ecc_stats;

	page = (int)(from >> chip->page_shift);

	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	u16_BlkIdx = (page / pNandDrv->u16_BlkPageCnt);
	#endif
	col = (int)(from & (mtd->writesize - 1));

	buf = ops->datbuf;
	oob = ops->oobbuf;
	//oob_poi = (oob == NULL) ? chip->oob_poi : oob;
	oob_poi = oob;

#if defined(CONFIG_MIPS)
	if( ((U32)buf) >= 0xC0000000 || ((U32)buf) % CACHE_LINE )	// For
#elif defined(CONFIG_ARM)
	if(!virt_addr_valid((U32)buf) || !virt_addr_valid((U32)buf + (U32)readlen - 1) ) // For High MEM (ARM)  ||virt_to_phys((void*)buf) >= MSTAR_MIU1_BUS_BASE
#endif
		dma = 0;

	while (1) {
		bytes = min(mtd->writesize - col, readlen);
		aligned = (bytes == mtd->writesize);

		#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
		if( (page % pNandDrv->u16_BlkPageCnt) >= pu16_FreePagePos[u16_BlkIdx] && pu16_FreePagePos[u16_BlkIdx] != 0xFFFF)
		{
			memset(buf, 0xFF, bytes);
	        
			buf += bytes;
			readlen -= bytes;

    	    if (!readlen)
        	    break;

        	/* For subsequent reads align to page boundary. */
        	col = 0;
        	/* Increment page address */
        	page++;
			continue;
		}
		#endif

		#if defined(FCIE_LFSR) && FCIE_LFSR
		if(pNandDrv->u8_RequireRandomizer == 1)
			NC_EnableLFSR();
		#endif
		
		#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
		bufpoi = NULL;
		list_for_each_entry(readbuf, &rcache, list) {
			#if defined(MSTAR_PARTIAL_READ) && MSTAR_PARTIAL_READ
			
			#if defined(MSTAR_READ_FROM_COL) && MSTAR_READ_FROM_COL
			if (readbuf->pagebuf == page && readbuf->bytelen >= col + bytes && readbuf->col < col)
			#else
			if (readbuf->pagebuf == page && readbuf->bytelen >= col + bytes)
			#endif

			#else
			if (readbuf->pagebuf == page) 
			#endif
			{
				bufpoi = readbuf->databuf;
				if(list_is_last(&readbuf->list, &rcache))
					list_move(&readbuf->list, &rcache);

				//printk(KERN_INFO"%s: match databuf(%d): %X\n", __func__, page, (unsigned int)readbuf->databuf);
				break;
			}
		}
		#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
		list_for_each_entry(writebuf, &wcache, list) {
			if (writebuf->pagebuf == page) 
			{
				bufpoi = writebuf->databuf;
				if(list_is_last(&writebuf->list, &wcache))
					list_move(&writebuf->list, &wcache);
				
				//printk(KERN_CRIT"%s: match databuf(%d): %X\n", __func__, page, (unsigned int)writebuf->databuf);
				break;
			}
		}
		#endif
		if(bufpoi==NULL || oob) {
			if(bufpoi == NULL) {
				readbuf = list_entry(rcache.prev, struct mstar_read_buf, list);
				if(!readbuf)
					nand_die();
				list_move(&readbuf->list, &rcache);
				bufpoi = readbuf->databuf;
				//printk(KERN_INFO"%s: allocate databuf(%d): %X\n", __func__, page, (unsigned int)readbuf->databuf);
			}
		#else
		/* Is the current page in the buffer ? */
		#if defined(MSTAR_PARTIAL_READ) && MSTAR_PARTIAL_READ
		#if defined(MSTAR_READ_FROM_COL) && MSTAR_READ_FROM_COL
		if (chip->pagebuf != page || chip->bytelen < col + bytes || chip->col >= col || oob)
		#else
		if (chip->pagebuf != page || chip->bytelen < col + bytes || oob)
		#endif
		#else
		if (page != chip->pagebuf || oob) 
		#endif 
		{
			bufpoi = (aligned && dma) ? buf : chip->buffers->databuf;
		#endif

			/*
			 * translate logical page to physical page
			 */
			#if defined(MSTAR_PARTIAL_READ) && MSTAR_PARTIAL_READ
			if(aligned)
			{
			#endif
			if(pNandDrv->u8_PlaneCnt > 1) {
				lba = page >> (chip->phys_erase_shift-chip->page_shift);
				lpa = page & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
				realpage = ((lba<<1) << pNandDrv->u8_BlkPageCntBits) + lpa;
				u32_Err = NC_ReadPages2P(realpage, bufpoi, oob_poi, 1);
			}
			else {
				realpage = page;
				u32_Err = NC_ReadPages(realpage, bufpoi, oob_poi, 1);	
			}
			#if defined(MSTAR_PARTIAL_READ) && MSTAR_PARTIAL_READ
				u16_SectorCnt = pNandDrv->u16_PageSectorCnt;
			}
			else
			{
				if(pNandDrv->u8_PlaneCnt > 1)
				{
					#if defined(MSTAR_READ_FROM_COL) && MSTAR_READ_FROM_COL
					//FIXME
					int colsect = col >> pNandDrv->u8_SectorByteCntBits;
					int remainsect;
					lba = page >> (chip->phys_erase_shift-chip->page_shift);
					lpa = page & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
					realpage = ((lba<<1) << pNandDrv->u8_BlkPageCntBits) + lpa;
					
					u16_SectorCnt = ((col + bytes) >> pNandDrv->u8_SectorByteCntBits);				
					u16_SectorCnt = (u16_SectorCnt == (pNandDrv->u16_PageSectorCnt * 2) || ((col + bytes) % pNandDrv->u16_SectorByteCnt) == 0) 
										? u16_SectorCnt :  u16_SectorCnt + 1;
					u16_SectorCnt -= colsect;
					
					remainsect = u16_SectorCnt;

					memset(bufpoi, 0xff, pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt);

					if(colsect < pNandDrv->u16_PageSectorCnt)
					{
						remainsect = (colsect + u16_SectorCnt > pNandDrv->u16_PageSectorCnt) ? 
										colsect + u16_SectorCnt - pNandDrv->u16_PageSectorCnt :  0;
						u32_Err = NC_ReadSectors_MTD(realpage, colsect, bufpoi + (colsect << pNandDrv->u8_SectorByteCntBits), NULL, 
									u16_SectorCnt - remainsect);
					}
					if(remainsect)
					{
						if(colsect >= pNandDrv->u16_PageSectorCnt)
						{
							u32_Err = NC_ReadSectors_MTD(realpage + pNandDrv->u16_BlkPageCnt, colsect - pNandDrv->u16_PageSectorCnt, 
									bufpoi + (colsect << pNandDrv->u8_SectorByteCntBits), NULL, remainsect);
						}
						else
							u32_Err = NC_ReadSectors_MTD(realpage + pNandDrv->u16_BlkPageCnt, 0, bufpoi + (pNandDrv->u16_PageByteCnt), NULL, remainsect);
						
					}
					#else
					u16_SectorCnt = ((col + bytes) >> pNandDrv->u8_SectorByteCntBits);
					u16_SectorCnt = (u16_SectorCnt == pNandDrv->u16_PageSectorCnt) ? u16_SectorCnt :  u16_SectorCnt + 1;

					lba = page >> (chip->phys_erase_shift-chip->page_shift);
					lpa = page & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
					realpage = ((lba<<1) << pNandDrv->u8_BlkPageCntBits) + lpa;

					if(u16_SectorCnt > pNandDrv->u16_PageSectorCnt)
					{
						u32_Err = NC_ReadSectors_MTD(realpage, 0, bufpoi, NULL, pNandDrv->u16_PageSectorCnt);
						u32_Err = NC_ReadSectors_MTD(realpage + pNandDrv->u16_BlkPageCnt, 0, bufpoi + pNandDrv->u16_PageByteCnt, NULL, u16_SectorCnt - pNandDrv->u16_PageSectorCnt);
					}
					else
						u32_Err = NC_ReadSectors_MTD(realpage, 0, bufpoi, NULL, u16_SectorCnt);
					
					#endif
				}
				else
				{
					#if defined(MSTAR_READ_FROM_COL) && MSTAR_READ_FROM_COL
					int colsect = col >> pNandDrv->u8_SectorByteCntBits;
					u16_SectorCnt = ((col + bytes) >> pNandDrv->u8_SectorByteCntBits);
					u16_SectorCnt = (u16_SectorCnt == pNandDrv->u16_PageSectorCnt || ((col + bytes) % pNandDrv->u16_SectorByteCnt) == 0) 
										? u16_SectorCnt :  u16_SectorCnt + 1;
					u16_SectorCnt -= colsect;
					#else
					u16_SectorCnt = ((col + bytes) >> pNandDrv->u8_SectorByteCntBits);
					u16_SectorCnt = (u16_SectorCnt == pNandDrv->u16_PageSectorCnt) ? u16_SectorCnt :  u16_SectorCnt + 1;
					#endif
					realpage = page;
					memset(bufpoi, 0xff, pNandDrv->u16_PageByteCnt);
					#if defined(MSTAR_READ_FROM_COL) && MSTAR_READ_FROM_COL
					if(bytes >= pNandDrv->u16_SectorByteCnt)
					{
						u16_SectorCnt = pNandDrv->u16_PageSectorCnt - colsect;
					}
					if(oob_poi ==  NULL)
					{
						if(pNandDrv->u8_RequireRandomizer == 1)
							u32_Err = NC_ReadSectors_MTD(realpage, 0, bufpoi, NULL, u16_SectorCnt + colsect);
						else
						u32_Err = NC_ReadSectors_MTD(realpage, colsect, bufpoi + (colsect << pNandDrv->u8_SectorByteCntBits), 
								NULL, u16_SectorCnt);
					}
					else
					{
						if(pNandDrv->u8_RequireRandomizer == 1)
							u32_Err = NC_ReadSectors_MTD(realpage, 0, bufpoi, oob_poi, u16_SectorCnt + colsect);
						else
							u32_Err = NC_ReadSectors_MTD(realpage, colsect, bufpoi + (colsect << pNandDrv->u8_SectorByteCntBits), 
								oob_poi + colsect * pNandDrv->u16_SectorSpareByteCnt, u16_SectorCnt);
					}
					#else
					if(oob_poi == NULL)
					{
						u32_Err = NC_ReadSectors_MTD(realpage, 0, bufpoi, NULL, u16_SectorCnt);
					}
					else
						u32_Err = NC_ReadSectors_MTD(realpage, 0, bufpoi, oob_poi, u16_SectorCnt);

					#endif
				}
			}
			#endif
			NC_CheckECC(&s32ECCStatus);
			if(u32_Err != UNFD_ST_SUCCESS || s32ECCStatus < 0)
			{
				//check if false alarm caused by non-all-0xff empty page
				for(j = 0; j < pNandDrv->u16_PageSectorCnt * pNandDrv->u8_PlaneCnt; j++)
            	{
                	ECCErrBitCnt = 0;
                	for(i = 0; i < pNandDrv->u16_SectorByteCnt; i++)
                	{
                	    ECCErrBitCnt += ErrorBitCountforEmptyPage(bufpoi[ j*pNandDrv->u16_SectorByteCnt + i]);
                	}
                	if(ECCErrBitCnt > pNandDrv->u16_ECCCorretableBit)
                	{
				mtd->ecc_stats.failed++;
                	    false_alarm = 0;
                	    break;
                	}
                	else
                	    memset(bufpoi + j * pNandDrv->u16_SectorByteCnt, 0xFF, pNandDrv->u16_SectorByteCnt);

					if(oob_poi != NULL)
					{
                	for(i = 0; i < pNandDrv->u16_SectorSpareByteCnt; i++)
                	{
                	    if(j*pNandDrv->u16_SectorSpareByteCnt + i < 512)
                	        ECCErrBitCnt += ErrorBitCountforEmptyPage(oob_poi[ j*pNandDrv->u16_SectorSpareByteCnt + i]);
                	}
                	if(ECCErrBitCnt > pNandDrv->u16_ECCCorretableBit)
                	{
                	    mtd->ecc_stats.failed++;
                	    false_alarm = 0;
                	    break;
                	}
                	else
                	    memset(oob_poi + j * pNandDrv->u16_SectorSpareByteCnt, 0xFF, pNandDrv->u16_SectorSpareByteCnt);
            	}
            	}
            	if(false_alarm == 1)
				{
            	    printk("[%s]\tecc false alarm caused by empty page\n",__func__);
					mtd->ecc_stats.corrected += 1;
					#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
					//check page is all 0xff?

					if(pu16_FreePagePos[u16_BlkIdx]	== 0xFFFF || pu16_FreePagePos[u16_BlkIdx] > (realpage % pNandDrv->u16_BlkPageCnt))
					{
						pu16_FreePagePos[u16_BlkIdx] = realpage % pNandDrv->u16_BlkPageCnt;
					}
					#endif
				}
            	else
            	    printk(KERN_CRIT"[%s]\ttrue ecc fail\n",__func__);
			}
			else
				mtd->ecc_stats.corrected += s32ECCStatus;
			
			#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
			//check page is all 0xff?

			if(pu16_FreePagePos[u16_BlkIdx]	== 0xFFFF || pu16_FreePagePos[u16_BlkIdx] > (realpage % pNandDrv->u16_BlkPageCnt))
			{
				#if defined(CONFIG_MSTAR_AGATE) || defined(CONFIG_MSTAR_EAGLE) || defined(CONFIG_MSTAR_EMERALD) || defined(CONFIG_MSTAR_EDISON) || \
					    defined(CONFIG_MSTAR_EIFFEL) || defined(CONFIG_MSTAR_EINSTEIN)
				if(pNandDrv->u8_RequireRandomizer != 1)
				{
				if( ((REG(NC_CTRL) & BIT9) == BIT9) && aligned)
				{
				//	printk("BLK %X, free page @ %X\n", u16_BlkIdx, realpage % pNandDrv->u16_BlkPageCnt);
					pu16_FreePagePos[u16_BlkIdx] = realpage % pNandDrv->u16_BlkPageCnt;
				}
			}
				else
				#endif
				{	
					ECCErrBitCnt = 0;
					for(j = 0; j < pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt; j++)
            		{
						ECCErrBitCnt += ErrorBitCountforEmptyPage(bufpoi[j]);
						if(ECCErrBitCnt != 0)
							break;
					}
					if(ECCErrBitCnt == 0 && aligned)
					{
						pu16_FreePagePos[u16_BlkIdx] = realpage % pNandDrv->u16_BlkPageCnt;
					}
				}
			}
			
			#endif
			//This block should be mark bad
			if(s32ECCStatus >= (pNandDrv->u16_ECCCorretableBit * 4 / 5))
				markbad = 1;
			
			if(s32ECCStatus == (pNandDrv->u16_ECCCorretableBit + 1))
				markbad = 2;

			/* Transfer not aligned data */
			#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
			if (!(mtd->ecc_stats.failed - stats.failed))
			{
				readbuf->pagebuf = page;
			#if defined(MSTAR_PARTIAL_READ) && MSTAR_PARTIAL_READ
				#if defined(MSTAR_READ_FROM_COL) && MSTAR_READ_FROM_COL
				readbuf->col = (col >> pNandDrv->u8_SectorByteCntBits) << pNandDrv->u8_SectorByteCntBits;
				if(pNandDrv->u8_RequireRandomizer == 1)
					readbuf->col = 0;
				#else
				readbuf->col = 0;
				#endif
				readbuf->bytelen =  readbuf->col + (u16_SectorCnt << pNandDrv->u8_SectorByteCntBits);
			#endif
			}
			else
				readbuf->pagebuf = -1;
			memcpy(buf, readbuf->databuf + col, bytes);
			#else
			if (!aligned || !dma) {
				if (!(mtd->ecc_stats.failed - stats.failed))
				{
				#if defined(MSTAR_PARTIAL_READ) && MSTAR_PARTIAL_READ
					#if defined(MSTAR_READ_FROM_COL) && MSTAR_READ_FROM_COL
					chip->col = (col >> pNandDrv->u8_SectorByteCntBits) << pNandDrv->u8_SectorByteCntBits;
					if(pNandDrv->u8_RequireRandomizer == 1)
						chip->col = 0;
					#else
					chip->col = 0;
					#endif
					chip->bytelen = chip->col + (u16_SectorCnt << pNandDrv->u8_SectorByteCntBits);
				#endif
					chip->pagebuf = page;
				}
				memcpy(buf, chip->buffers->databuf + col, bytes);
			}
			#endif

			buf += bytes;

			if (unlikely(oob)) {

				int toread = min(oobreadlen, max_oobsize);

				if (toread) {
					oob = nand_transfer_oob(chip,
						oob, ops, toread);
					oobreadlen -= toread;
				}
			}
		} else {
			#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
			memcpy(buf, bufpoi + col, bytes);
			#else
			memcpy(buf, chip->buffers->databuf + col, bytes);
			#endif
			buf += bytes;
		}

		readlen -= bytes;

		if (!readlen)
			break;

		/* For subsequent reads align to page boundary. */
		col = 0;
		/* Increment page address */
		page++;
	}
	ops->retlen = ops->len - (size_t) readlen;
	if (oob)
		ops->oobretlen = ops->ooblen - oobreadlen;
	#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
	u64_TotalReadBytes += ops->len;
	#endif
	if (mtd->ecc_stats.failed - stats.failed)
		return -EBADMSG;
	if(pNandDrv->u8_CellType == 1)
		return  mtd->ecc_stats.corrected - stats.corrected ? (-EUCLEAN - markbad) : 0;
	else
		return  mtd->ecc_stats.corrected - stats.corrected ? (-EUCLEAN) : 0;
}

static int _unfd_nand_read(struct mtd_info *mtd, loff_t from, size_t len,
             size_t *retlen, uint8_t *buf)
{
    struct nand_chip *chip = mtd->priv;
	int ret;
#ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
		int  page, offset, page_num, page_find, index;	
    int  block_page_num = mtd->erasesize >> chip->page_shift;
		char * cache_srcbuf = NULL; 
	struct page * entry = NULL;
	
		ret = 0;
		*retlen = 0;
#endif

    /* Do not allow reads past end of device */
    if ((from + len) > mtd->size)
        return -EINVAL;
    if (!len)
        return 0;

    nand_get_device(chip, mtd, FL_READING);
    nand_lock_fcie();

#ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
    #if defined(PAGE_CACHE_DEBUG) && PAGE_CACHE_DEBUG
		total_read_num++;
    #endif
	
		offset = (int)(from & (mtd->writesize - 1));
		page = (int)(from >> chip->page_shift);
		cache_trace.do_page_cache = 0;
	
		page_num = (offset+len+mtd->writesize-1) >> chip->page_shift;
    if(cache_page)
		page_find = nand_find_get_pages(&radix_tree_root, page, page_num, cache_page);
	else
		goto UNFD_NAND_READ_START;	
		
	if(page_find>0)
	{	
	    #if defined(PAGE_CACHE_DEBUG) && PAGE_CACHE_DEBUG
        total_read_hit_num += page_find; 
		#endif        

            if(page_find==page_num)
            {
                int copy_len = 0;
				*retlen = len;
				
	            for(index=0; index<page_find; ++index)
	            {
	                if(index==0)
	                {
					   copy_len = min((mtd->writesize - offset), len);
					   len -= copy_len;
	                }
					else
				    {
				       buf = (uint8_t *)((int)buf+copy_len);
				       copy_len = min(mtd->writesize, len);				   
					   len -= copy_len;
					   offset = 0;
				    }

					//printk(KERN_ERR "%08x, copy len %d len %d\n", (unsigned int)buf, copy_len, len);
	                cache_srcbuf = (char *)kmap(cache_page[index]) + offset;	
				    memcpy((void *)buf, cache_srcbuf, copy_len);
				    kunmap(cache_page[index]);
	            }

				goto UNFD_NAND_READ_DONE;
            }
			else
			{
			    int copy_len = 0;
                if(cache_page[page_find-1]->index == (page+page_find-1))
                {                    
					for(index=0; index<page_find; ++index)
					{
					   if(index==0)
					   {
					       copy_len = min((mtd->writesize - offset), len);	
					   }
					   else
					   {						   
						   copy_len = min(mtd->writesize, len);	
						   offset = 0;
					   }					   
					   
					   //printk(KERN_ERR "%08x, copy len %d len %d\n", (unsigned int)buf, copy_len, len);
					   cache_srcbuf = (char *)kmap(cache_page[index]) + offset;    
					   memcpy((void *)buf, cache_srcbuf, copy_len);
					   kunmap(cache_page[index]);
					   
					   *retlen += copy_len;
					   len -= copy_len;					   
				   buf = (uint8_t *)((int)buf+copy_len);
					   page++;
					}

	                from = page << chip->page_shift;
                }
				else
				{
			    //add code for hit at last page
				}
		    }
        }
		else
		{ 
	   index = page % block_page_num;	   
	   if((len <= NAND_READ_CHUNK_LENG) && ((offset+len) <= mtd->writesize) && (index!=0) && (index!=1)) //suppose first read less than 1024, later this page will been read more than one time		   
		   {
			   if(NAND_PAGE_SIZE_8K == mtd->writesize)
			   {
				  entry = alloc_pages(GFP_KERNEL, 1);  // 2 pages
			   }
			   else if(NAND_PAGE_SIZE_4K >= mtd->writesize)
			   {
				  entry = alloc_pages(GFP_KERNEL, 0);  // 1 page
			   } 
	
			   if(entry)
			   {
				  cache_trace.offset = offset;
				  cache_trace.do_page_cache = 1;
				  cache_trace.len = len;
				  cache_trace.databuf = buf;			  
	
				  from = page << chip->page_shift;
				  len = mtd->writesize;
				  buf = (char *)kmap(entry);	  
			   }
			   else
				  printk(KERN_ERR "nand page cache: no memory\n");
		   }
		}	
#endif

UNFD_NAND_READ_START:
    chip->ops.len = len;
    chip->ops.datbuf = buf;
    chip->ops.oobbuf = NULL;
	chip->ops.ooblen = 0;	
	ret = mstar_nand_do_read_ops(mtd, from, &chip->ops);

#ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
		if(cache_trace.do_page_cache)
		{
			nand_add_to_radixtree(&radix_tree_root, entry, page, GFP_KERNEL); 
			cache_srcbuf = chip->ops.datbuf + cache_trace.offset;	
			memcpy((void *)cache_trace.databuf, cache_srcbuf, cache_trace.len);
			kunmap(entry);
			cache_trace.do_page_cache = 0;
			*retlen += cache_trace.len;		
		}
		else
			*retlen += chip->ops.retlen;
#else	
    *retlen = chip->ops.retlen;
#endif	       
	
UNFD_NAND_READ_DONE:	
    nand_unlock_fcie();
    nand_release_device(mtd);

    return ret;
}


static int _unfd_nand_write(struct mtd_info *mtd, loff_t to, size_t len,
              size_t *retlen, const uint8_t *buf)
{
    struct nand_chip *chip = mtd->priv;
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    U8 *pu8_MainBuf = (U8 *)u8MainBuf;
    U32 u32_Err = 0;
    int start_page_addr = 0;
    int LBA, LPA;
    int page_addr;
    int ret = 0;
    int page;
    uint32_t writelen = len;
    int subpage;

    #ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
		int page_find, page_num, index, offset;
    #endif
	
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
	struct mstar_read_buf *readbuf;
	#endif
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
	struct mstar_write_cache_buf* writebuf;
	#endif
	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	U16 u16_BlkIdx;
	#endif


    /* Do not allow reads past end of device */
    if ((to + len) > mtd->size)
        return -EINVAL;
    if (!len)
        return 0;

    /* reject writes, which are not page aligned */
    subpage = (to & (mtd->writesize - 1)) || (writelen & (mtd->writesize - 1));

    if (subpage) {
        pr_info("%s: Attempt to write not "
                "page aligned data\n", __func__);
        return -EINVAL;
    }

    nand_get_device(chip, mtd, FL_WRITING);
    nand_lock_fcie();

	#if defined(FCIE_LFSR) && FCIE_LFSR
	if(pNandDrv->u8_RequireRandomizer == 1)
		NC_EnableLFSR();
	#endif

    page = (int)(to >> chip->page_shift);

    #ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
	#if defined(PAGE_CACHE_DEBUG) && PAGE_CACHE_DEBUG
	total_write_num++;
	#endif
	offset = (int)(to & (mtd->writesize - 1));
	page_num = (offset+len+mtd->writesize-1) >> chip->page_shift;
	if(cache_page)
		page_find = nand_find_get_pages(&radix_tree_root, page, page_num, cache_page);
	else
		page_find = 0;
	if(page_find > 0) 
	{     
		 #if defined(PAGE_CACHE_DEBUG) && PAGE_CACHE_DEBUG
	     total_write_hit_num += page_find;
	     #endif
	     for(index=0; index<page_find; ++index)
		    nand_remove_from_radixtree(&radix_tree_root, cache_page[index]);	     
	}	
    #endif

    /* Invalidate the page cache, when we write to the cached page */
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
	writebuf = list_entry(wcache.prev, struct mstar_write_cache_buf, list);
	if(!writebuf)
		nand_die();
	list_move(&writebuf->list, &wcache);

	//copy first page into write cache buffer
	memcpy(writebuf->databuf, buf, mtd->writesize);
	writebuf->pagebuf = page;

	//printk(KERN_INFO"%s: allocate databuf(%d): %X\n", __func__, page, (unsigned int)readbuf->databuf);

	#endif

	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
	list_for_each_entry(readbuf, &rcache, list) {
		if (readbuf->pagebuf != -1) {
			if (to <= (readbuf->pagebuf << chip->page_shift) &&
			    (readbuf->pagebuf << chip->page_shift) < (to + len)) {
				readbuf->pagebuf = -1;
				list_move_tail(&readbuf->list, &rcache);
				break;
			}
		}
	}
	#else
    if (to <= (chip->pagebuf << chip->page_shift) &&
        (chip->pagebuf << chip->page_shift) < (to + len))
        chip->pagebuf = -1;
	#endif

#if defined(CONFIG_MIPS)
    if((U32)buf >= 0xC0000000 || ((U32)buf) % CACHE_LINE)
#elif defined(CONFIG_ARM)
    if(!virt_addr_valid((U32)buf) || !virt_addr_valid((U32)buf + (U32)len - 1) ) // For High MEM (ARM)
#endif
    {
        while(1) {
            int bytes = mtd->writesize;

            page_addr = page;
            if(pNandDrv->u8_PlaneCnt > 1)
            {
                LBA = page_addr >> (chip->phys_erase_shift-chip->page_shift);
                LPA = page_addr & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
                page_addr = ((LBA<<1)<<pNandDrv->u8_BlkPageCntBits)+LPA;
            }
            page_addr += start_page_addr;

            memcpy(pu8_MainBuf, buf, bytes);

            if(pNandDrv->u8_PlaneCnt > 1) {
                u32_Err = NC_WritePages2P(page_addr, pu8_MainBuf, NULL, 1);
            }
            else {
                u32_Err = NC_WritePages(page_addr, pu8_MainBuf, NULL, 1);
            }

            if (u32_Err != UNFD_ST_SUCCESS) {
                ret = -EIO;
                break;
            }
            writelen -= bytes;
            if (!writelen)
                break;

            buf += bytes;
            page++;
        }
    }
    else {
        page_addr = page;
        if(pNandDrv->u8_PlaneCnt > 1)
        {
            LBA = page_addr >> (chip->phys_erase_shift-chip->page_shift);
            LPA = page_addr & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
            page_addr = ((LBA<<1)<<pNandDrv->u8_BlkPageCntBits)+LPA;
        }
        page_addr += start_page_addr;

        if(pNandDrv->u8_PlaneCnt > 1) {
			if(pNandDrv->u8_CacheProgram == UNFD_RW_MULTIPLANE_CACHE)
			{
	            u32_Err = NC_WritePagesCache2P(page_addr, (U8*)buf, NULL, writelen>>chip->page_shift);
			}
			else if(pNandDrv->u8_CacheProgram == UNFD_RW_MULTIPLANE)
			{
				u32_Err = NC_WritePages2P(page_addr, (U8*)buf, NULL, writelen>>chip->page_shift);
			}
			else
				nand_die();
        }
        else {
			if (pNandDrv->u8_CacheProgram == UNFD_RW_NORMAL)
			{
	            u32_Err = NC_WritePages(page_addr, (U8*)buf, NULL, writelen>>chip->page_shift);
			}
			else if(pNandDrv->u8_CacheProgram == UNFD_RW_CACHE)
			{
	            u32_Err = NC_WritePagesCache(page_addr, (U8*)buf, NULL, writelen>>chip->page_shift);
			}
			else         
				nand_die();
        }

        if (u32_Err != UNFD_ST_SUCCESS) {
            ret = -EIO;
        }

        writelen = 0;
    }

	#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
	u64_TotalWriteBytes += len;
	#endif

	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	u16_BlkIdx = ((int)(to >> chip->page_shift)) / pNandDrv->u16_BlkPageCnt;
//	pu16_FreePagePos[u16_BlkIdx] = (((int)(to >> chip->page_shift)) & pNandDrv->u8_BlkPageCntBits) + (len >> chip->page_shift);
	pu16_FreePagePos[u16_BlkIdx] =  0xFFFF;
	#endif

    nand_unlock_fcie();

    nand_release_device(mtd);

    *retlen = len - writelen;

    return ret;
}

extern int nand_block_checkbad(struct mtd_info *mtd, loff_t ofs, int getchip,
			       int allowbbt);

static int _unfd_erase_nand(struct mtd_info *mtd, struct erase_info *instr, int allowbbt)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 u32_Err;
	int lba;
	int page, realpage, pages_per_block, ret;
	struct nand_chip *chip = mtd->priv;
	loff_t len;

    #ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
	int page_find, index;		 
    #endif
	
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
	struct mstar_read_buf *readbuf;
	#endif
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
	struct mstar_write_cache_buf *writebuf;
	#endif	
	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	U16 u16_BlkIdx;
	#endif

	#ifdef CONFIG_MSTAR_NAND_PARAM_CHECK
	if (check_offs_len(mtd, instr->addr, instr->len))
		return -EINVAL;
	#endif

	instr->fail_addr = MTD_FAIL_ADDR_UNKNOWN;

	/* Grab the lock and see if the device is available */
	nand_get_device(chip, mtd, FL_ERASING);
    nand_lock_fcie();

	/* Shift to get first page */
	page = (int)(instr->addr >> chip->page_shift);

	/* Calculate pages in each block */
	pages_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);

	/* Loop through the pages */
	len = instr->len;
	instr->state = MTD_ERASING;

	while (len) {
		/*
		 * check if we have a bad block, we do not erase bad blocks !
		 */
		if (!(chip->options & NAND_SKIP_BBTSCAN)) {
			if (nand_block_checkbad(mtd, ((loff_t) page) <<
						chip->page_shift, 0, allowbbt)) {
				printk(KERN_WARNING "%s: attempt to erase a bad block "
						"at page 0x%08x\n", __func__, page);
				instr->state = MTD_ERASE_FAILED;
				goto erase_exit;
			}
		}

		#ifdef CONFIG_MSTAR_NAND_PAGE_CACHE		
		if(cache_page)
			page_find = nand_find_get_pages(&radix_tree_root, page, pages_per_block, cache_page);
		else
			page_find = 0;
		
		if(page_find > 0) 
		{	
			 for(index=0; index<page_find; ++index)
				nand_remove_from_radixtree(&radix_tree_root, cache_page[index]);		 
		}	
		#endif		

		/*
		 * Invalidate the page cache, if we erase the block which
		 * contains the current cached page
		 */
		#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
		list_for_each_entry(writebuf, &wcache, list) {
			if (writebuf->pagebuf != -1) {
				if (page <= writebuf->pagebuf && writebuf->pagebuf <
					(page + pages_per_block)) {
					writebuf->pagebuf = -1;
					list_move_tail(&writebuf->list, &wcache);
					break;
				}
			}
		}
		#endif

		#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
		list_for_each_entry(readbuf, &rcache, list) {
			if (readbuf->pagebuf != -1) {
				if (page <= readbuf->pagebuf && readbuf->pagebuf <
					(page + pages_per_block)) {
					readbuf->pagebuf = -1;
					list_move_tail(&readbuf->list, &rcache);
					break;
				}
			}
		}
		#else
		if (page <= chip->pagebuf && chip->pagebuf <
		    (page + pages_per_block))
			chip->pagebuf = -1;
		#endif

		/*
		 * translate logical page to physical page
		 */
		lba = page >> (chip->phys_erase_shift-chip->page_shift);
		if(pNandDrv->u8_PlaneCnt > 1) {
			realpage = ((lba<<1) << pNandDrv->u8_BlkPageCntBits);
			u32_Err = NC_EraseBlk2P(realpage);
		}
		else {
			realpage = lba << pNandDrv->u8_BlkPageCntBits;
			u32_Err = NC_EraseBlk(realpage);
		}

		/* See if block erase succeeded */
		if (u32_Err != UNFD_ST_SUCCESS) {
			instr->state = MTD_ERASE_FAILED;
			instr->fail_addr =
				((loff_t)page << chip->page_shift);
			goto erase_exit;
		}

		/* Increment page address and decrement length */
		len -= (1 << chip->phys_erase_shift);
		page += pages_per_block;
	}
	instr->state = MTD_ERASE_DONE;

	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	u16_BlkIdx = ((int)(instr->addr >> chip->page_shift)) / pNandDrv->u16_BlkPageCnt;
//	pu16_FreePagePos[u16_BlkIdx] = 0;
	pu16_FreePagePos[u16_BlkIdx] = 0xFFFF;
	#endif

erase_exit:

	ret = instr->state == MTD_ERASE_DONE ? 0 : -EIO;

	/* Deselect and wake up anyone waiting on the device */
    nand_unlock_fcie();	
	nand_release_device(mtd);

	/* Do call back function */
	if (!ret)
		mtd_erase_callback(instr);

	return ret;
}


int mstar_nand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	return _unfd_erase_nand(mtd, instr, 0);
}

#ifdef CONFIG_MTD_UBI_WRITE_CALLBACK
int mstar_nand_write_cb(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, 
	const u_char *buf, struct write_info *write)
{
	struct nand_chip *chip = mtd->priv;
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    U8 *pu8_MainBuf = (U8 *)u8MainBuf;
    U32 u32_Err = 0;
    int start_page_addr = 0;
    int LBA, LPA;
    int page_addr;
    int ret = 0;
    int page;
    uint32_t writelen = len;
    int subpage;
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
	struct mstar_read_buf *readbuf;
	#endif
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
	struct mstar_write_cache_buf *writebuf;
	#endif
	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	U16 u16_BlkIdx;
	#endif
    #ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
	int page_find, page_num, offset, index;		 
    #endif
    /* Do not allow reads past end of device */
    if ((to + len) > mtd->size)
        return -EINVAL;
    if (!len)
        return 0;

    /* reject writes, which are not page aligned */
    subpage = (to & (mtd->writesize - 1)) || (writelen & (mtd->writesize - 1));

    if (subpage) {
        pr_info("%s: Attempt to write not "
                "page aligned data\n", __func__);
        return -EINVAL;
    }

    nand_get_device(chip, mtd, FL_WRITING);
    nand_lock_fcie();

    page = (int)(to >> chip->page_shift);

	#if defined(FCIE_LFSR) && FCIE_LFSR
	if(pNandDrv->u8_RequireRandomizer == 1)
		NC_EnableLFSR();
	#endif

    #ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
	offset = (int)(to & (mtd->writesize - 1));
	page_num = (offset+len+mtd->writesize-1) >> chip->page_shift;

	if(cache_page)
		page_find = nand_find_get_pages(&radix_tree_root, page, page_num, cache_page);
	else
		page_find = 0;

	if(page_find > 0) 
	{     
	     for(index=0; index<page_find; ++index)
		    nand_remove_from_radixtree(&radix_tree_root, cache_page[index]);	     
	}	
    #endif
    /* Invalidate the page cache, when we write to the cached page */
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
	writebuf = list_entry(wcache.prev, struct mstar_write_cache_buf, list);
	if(!writebuf)
		nand_die();
	list_move(&writebuf->list, &wcache);

	//copy first page into write cache buffer
	memcpy(writebuf->databuf, buf, mtd->writesize);
	writebuf->pagebuf = page;

	//printk(KERN_INFO"%s: allocate databuf(%d): %X\n", __func__, page, (unsigned int)readbuf->databuf);

	#endif

	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
	list_for_each_entry(readbuf, &rcache, list) {
		if (readbuf->pagebuf != -1) {
			if (to <= (readbuf->pagebuf << chip->page_shift) &&
			    (readbuf->pagebuf << chip->page_shift) < (to + len)) {
				readbuf->pagebuf = -1;
				list_move_tail(&readbuf->list, &rcache);
				break;
			}
		}
	}
	#else
    if (to <= (chip->pagebuf << chip->page_shift) &&
        (chip->pagebuf << chip->page_shift) < (to + len))
        chip->pagebuf = -1;
#endif

#if defined(CONFIG_MIPS)
    if((U32)buf >= 0xC0000000 || ((U32)buf) % CACHE_LINE)
#elif defined(CONFIG_ARM)
    if(!virt_addr_valid((U32)buf) || !virt_addr_valid((U32)buf + (U32)len - 1) ) // For High MEM (ARM)
#endif
    {
        while(1) {
            int bytes = mtd->writesize;

            page_addr = page;
            if(pNandDrv->u8_PlaneCnt > 1)
            {
                LBA = page_addr >> (chip->phys_erase_shift-chip->page_shift);
                LPA = page_addr & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
                page_addr = ((LBA<<1)<<pNandDrv->u8_BlkPageCntBits)+LPA;
            }
            page_addr += start_page_addr;

            memcpy(pu8_MainBuf, buf, bytes);

            if(pNandDrv->u8_PlaneCnt > 1) {
                u32_Err = NC_WritePages2P(page_addr, pu8_MainBuf, NULL, 1);
            }
            else {
                u32_Err = NC_WritePages(page_addr, pu8_MainBuf, NULL, 1);
            }

            if (u32_Err != UNFD_ST_SUCCESS) {
                ret = -EIO;
                break;
            }
            writelen -= bytes;
            if (!writelen)
                break;

            buf += bytes;
            page++;
        }
    }
    else {
        page_addr = page;
        if(pNandDrv->u8_PlaneCnt > 1)
        {
            LBA = page_addr >> (chip->phys_erase_shift-chip->page_shift);
            LPA = page_addr & ((1<<(chip->phys_erase_shift - chip->page_shift))-1);
            page_addr = ((LBA<<1)<<pNandDrv->u8_BlkPageCntBits)+LPA;
        }
        page_addr += start_page_addr;

        if(pNandDrv->u8_PlaneCnt > 1) {
			if(pNandDrv->u8_CacheProgram == UNFD_RW_MULTIPLANE_CACHE)
			{
	            u32_Err = NC_WritePagesCache2P_CB(page_addr, (U8*)buf, NULL, writelen>>chip->page_shift, write);
			}
			else if(pNandDrv->u8_CacheProgram == UNFD_RW_MULTIPLANE)
			{
				u32_Err = NC_WritePages2P_CB(page_addr, (U8*)buf, NULL, writelen>>chip->page_shift, write);
			}
			else
				nand_die();
        }
        else {
			if (pNandDrv->u8_CacheProgram == UNFD_RW_NORMAL)
			{
				u32_Err = NC_WritePages_CB(page_addr, (U8*)buf, NULL, writelen>>chip->page_shift, write);
			}
			else if(pNandDrv->u8_CacheProgram == UNFD_RW_CACHE)
			{
	            u32_Err = NC_WritePagesCache_CB(page_addr, (U8*)buf, NULL, writelen>>chip->page_shift, write);
			}
			else
				nand_die(); 
        }

        if (u32_Err != UNFD_ST_SUCCESS) {
            ret = -EIO;
        }

        writelen = 0;
    }
	#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
	u64_TotalWriteBytes += len;
	#endif

	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	u16_BlkIdx = ((int)(to >> chip->page_shift)) / pNandDrv->u16_BlkPageCnt;
//	pu16_FreePagePos[u16_BlkIdx] = (((int)(to >> chip->page_shift)) & pNandDrv->u8_BlkPageCntBits) + (len >> chip->page_shift);
	pu16_FreePagePos[u16_BlkIdx] = 0xFFFF;
	#endif

    nand_unlock_fcie();

    nand_release_device(mtd);

    *retlen = len - writelen;

    return ret;
}
#endif

#endif

#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
int procfile_read(char* buffer, char ** buffer_location, off_t offset,
					int buffer_length, int *eof, void *data)
{
	int ret;
	if(offset > 0)
		ret = 0;
	else
		ret = sprintf(buffer,"TotalWriteBytes %lld GB %lld MB\nTotalReadBytes %lld GB %lld MB\n", 
				u64_TotalWriteBytes/1024/1024/1024, (u64_TotalWriteBytes/1024/1024) % 1024,
				u64_TotalReadBytes/1024/1024/1024, (u64_TotalReadBytes/1024/1024) % 1024);
	return ret;
}
#endif

#if (defined(NAND_USE_UNFD)&&NAND_USE_UNFD)
U32 drvNAND_Init(void)
{
	U32 u32_Err = 0;
	NC_PlatformInit();

	u32_Err = drvNAND_ProbeReadSeq();
	if(u32_Err != UNFD_ST_SUCCESS)
	{
		return u32_Err;
	}
	//return 1;
	u32_Err = drvNAND_SearchCIS();

	return u32_Err; 
		
}

void nand_unfd_init(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	U32 u32_Err;
	
#if DMA_MODE
	printk("NAND FLASH : DMA MODE\n");
#else
	printk("NAND FLASH : RIU MODE\n");
#endif
	
	u32_Err = drvNAND_Init();
	if(UNFD_ST_ERR_NO_CIS == u32_Err)
	{		
		#if defined(FCIE4_DDR) && FCIE4_DDR
		if(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_MASM)
		{
			printk("NAND Error: Detect DDR NAND but have no CIS in NAND\n");
			nand_die();
		}
		else
		#endif
		{
			REG_WRITE_UINT16(NC_PATH_CTL, 0x20);

			NC_PlatformInit();

			NC_ResetFCIE(); // Reset FCIE3

			// We need to config Reg 0x40 first because we we need to get nand ID first
		//	pNandDrv->u16_Reg40_Signal =
		//	(BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
		//	~(BIT_NC_CHK_RB_EDGEn | BIT_NC_CE_SEL_MASK);
			REG_WRITE_UINT16(NC_SIGNAL, pNandDrv->u16_Reg40_Signal);
		
			NC_ResetNandFlash(); // Reset NAND flash

			NC_ReadID();

			drvNAND_CHECK_FLASH_TYPE();

			NC_ConfigContext();
			NC_Init();
		}

	}
	else if(UNFD_ST_SUCCESS != u32_Err)
	{
		printk("[%s]\tdrvNAND_Init Error : %lX", __func__, u32_Err);
		nand_die();
	}
	if(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_MASM)
	{
		pNandDrv->u8_IsMLC = 1;
	}
	nand_config_clock(pNandDrv->u16_tRC);

#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
	nand_enable_intr_mode();
#endif

}
#endif

/*
 * Board-specific NAND initialization.
 * - hwcontrol: hardwarespecific function for accesing control-lines
 * - dev_ready: hardwarespecific function for  accesing device ready/busy line
 * - eccmode: mode of ecc, see defines
 */
static int __init mstar_nand_init(void)
{
	struct nand_chip *nand;
	int err = 0;
	U16 u16_tmp, u16_oob_poi, u16_i, u16_SectorSpareByteCnt;
	#if defined(NAND_USE_UNFD)
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	#endif
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
	struct mstar_read_buf *readbuf;
	#endif
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
	struct mstar_write_cache_buf *writebuf;
	#endif

#if defined(NAND_USE_UNFD)
  memset(pNandDrv, '\0', sizeof(NAND_DRIVER));

  pNandDrv->u32_Clk=FCIE3_SW_SLOWEST_CLK;
#endif
	//------------------------------------------------------------------------------------

	/* Allocate memory for MTD device structure and private data */
	nand_mtd = kmalloc (sizeof(struct mtd_info) + sizeof(struct nand_chip), GFP_KERNEL);
	if (!nand_mtd) {
		printk ("Unable to allocate NAND MTD device structure.\n");
		return -ENOMEM;
	}

#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
	pNandDrv->PlatCtx_t.pu8_PageDataBuf = u8MainBuf;
	pNandDrv->PlatCtx_t.pu8_PageSpareBuf = u8SpareBuf;
#endif

	/* Get pointer to private data */
	nand = (struct nand_chip *) (&nand_mtd[1]);

	/* Initialize structures */
	memset((char *) nand_mtd, 0, sizeof(struct mtd_info));
	memset((char *) nand, 0, sizeof(struct nand_chip));

	nand_mtd->priv = nand;

	#if defined(MEASURE_PERFORMANCE) && MEASURE_PERFORMANCE
	u64_TotalWriteBytes = u64_TotalReadBytes = 0;
	writefile = create_proc_entry (procfs_name, 0644, NULL);
	if(writefile == NULL)
		printk(KERN_CRIT"Error: Can not initialize /proc/%s\n", procfs_name);
	else
	{
        writefile->read_proc = procfile_read;
        writefile->mode      = S_IFREG | S_IRUGO;
        writefile->uid       = 0;
        writefile->gid      = 0;
        writefile->size      = 0x10;
	}
	#endif

    //PF_MODE_SEM(down(&PfModeSem));
    nand_lock_fcie();

    #if defined(NAND_USE_UNFD)
	nand_unfd_init();
	#else
    drvNAND_FLASH_INIT();
	#endif

	#if defined(MSTAR_BLK_FREE_PAGE_CHECK) && MSTAR_BLK_FREE_PAGE_CHECK
	pu16_FreePagePos = kmalloc(pNandDrv->u16_BlkCnt * sizeof(U16), GFP_KERNEL);
	memset(pu16_FreePagePos, 0xFF, pNandDrv->u16_BlkCnt * sizeof(U16));
	#endif

    //PF_MODE_SEM(up(&PfModeSem));
    nand_unlock_fcie();

    /* please refer to include/linux/nand.h for more info. */

    nand->dev_ready						= _titania_nand_device_ready;
    nand->cmd_ctrl						= _titania_nand_hwcontrol;
    nand->ecc.mode						= NAND_ECC_HW;

	#if defined(NAND_USE_UNFD)

	nand->ecc.size						= pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt;
	nand->ecc.bytes						= pNandDrv->u16_ECCCodeByteCnt * pNandDrv->u16_PageSectorCnt * pNandDrv->u8_PlaneCnt;

	u16_SectorSpareByteCnt = (pNandDrv->u16_SpareByteCnt/pNandDrv->u16_PageSectorCnt);
	unfd_nand_oob_custom.eccbytes = pNandDrv->u8_PlaneCnt*(pNandDrv->u16_ECCCodeByteCnt * pNandDrv->u16_PageSectorCnt);

	for(u16_tmp=0 ; u16_tmp<pNandDrv->u8_PlaneCnt*pNandDrv->u16_PageSectorCnt ; u16_tmp++)
	{
		u16_oob_poi = ((u16_tmp+1)*u16_SectorSpareByteCnt)-pNandDrv->u16_ECCCodeByteCnt;
		for(u16_i=0 ; u16_i<pNandDrv->u16_ECCCodeByteCnt ; u16_i++)
		{
			unfd_nand_oob_custom.eccpos[(u16_tmp*pNandDrv->u16_ECCCodeByteCnt)+u16_i] = u16_oob_poi++;
		}
		
		if( u16_tmp == 0 || u16_tmp == pNandDrv->u16_PageSectorCnt)
		{
			unfd_nand_oob_custom.oobfree[u16_tmp].offset = u16_tmp*u16_SectorSpareByteCnt + 2;
			unfd_nand_oob_custom.oobfree[u16_tmp].length= u16_SectorSpareByteCnt - pNandDrv->u16_ECCCodeByteCnt - 2;
		}
		else
		{
			unfd_nand_oob_custom.oobfree[u16_tmp].offset = u16_tmp*u16_SectorSpareByteCnt;
			unfd_nand_oob_custom.oobfree[u16_tmp].length= u16_SectorSpareByteCnt - pNandDrv->u16_ECCCodeByteCnt;
		}
	}
	//dump_mem((unsigned char *) &unfd_nand_oob_custom, sizeof(unfd_nand_oob_custom));
	unfd_nand_oob_custom.oobavail = 0;

    for (u16_i = 0; u16_i < ARRAY_SIZE(unfd_nand_oob_custom.oobfree); u16_i++)
    {
		if(unfd_nand_oob_custom.oobfree[u16_i].length)
    		unfd_nand_oob_custom.oobavail += unfd_nand_oob_custom.oobfree[u16_i].length;
    }

	
	nand->ecc.layout =  &unfd_nand_oob_custom;
	_titania_nand_bbt_descr.offs    = 0;


	#else

    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
	{
        nand->ecc.size					= 2048;
        nand->ecc.bytes					= 40;
        nand->ecc.layout				= &titania_nand_oob_64;
        _titania_nand_bbt_descr.offs	= 0;
    }
    else
    {
		nand->ecc.size					= 512;
		nand->ecc.bytes					= 10;
		nand->ecc.layout				= &titania_nand_oob_16;
    }

    #endif

    nand->ecc.hwctl						= _titania_nand_enable_hwecc;
    nand->ecc.correct					= _titania_nand_correct_data;
    nand->ecc.calculate					= _titania_nand_calculate_ecc;

    #if defined(NAND_USE_UNFD)
	if( pNandDrv->u16_PageByteCnt > 512 )
	#else
    if(_fsinfo.eFlashConfig & FLASH_2KPAGE)
    #endif
		nand->options					= NAND_USE_FLASH_BBT | NAND_NO_AUTOINCR;
	else
		nand->options					= NAND_USE_FLASH_BBT;

    nand->waitfunc						= _titania_nand_wait;
    nand->read_byte						= _titania_nand_read_byte;
    nand->read_word						= _titania_nand_read_word;
    nand->read_buf						= _titania_nand_read_buf;
    nand->write_buf						= _titania_nand_write_buf;
    nand->chip_delay					= 20;
    nand->cmdfunc						= _titania_nand_cmdfunc;
    nand->badblock_pattern				= &_titania_nand_bbt_descr; //using default badblock pattern.
    nand->bbt_td						= &_titania_bbt_main_descr;
    nand->bbt_md						= &_titania_bbt_mirror_descr;

    if(unfd_nand_oob_custom.oobfree[0].length < (nand->bbt_td->len +1))
    { 
        nand->bbt_td->offs = 0;
        nand->bbt_td->veroffs = 3;
		nand->bbt_md->offs = 0;
		nand->bbt_md->veroffs = 3;
    }


    #if defined(NAND_USE_UNFD)
	// Let FCIE3 determine the WP
	#else
    MDrv_NAND_Set_Write_Protect(1);
	#endif

	if ((err = nand_scan(nand_mtd, 1)) != 0)
	{
		printk("can't register NAND\n");
		return -ENOMEM;
	}

	#if defined(NAND_USE_UNFD)

	#if defined(MSTAR_W_R_FUNC) && MSTAR_W_R_FUNC
	
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE
	INIT_LIST_HEAD(&rcache);
	for(u16_i=0 ; u16_i<CONFIG_MSTAR_NAND_MAX_CACHE ; u16_i++) {
		readbuf = kzalloc(sizeof(struct mstar_read_buf), GFP_KERNEL);
		if(!readbuf) {
			printk(KERN_INFO"%s: Error: failed to allocate cache buffer\n", __func__);
		}
		readbuf->databuf = kmalloc(nand_mtd->writesize, GFP_KERNEL);
		if(!readbuf->databuf) {
			printk(KERN_INFO"%s: Error: failed to allocate cache buffer\n", __func__);
		}
		readbuf->pagebuf = -1;
		list_add(&readbuf->list, &rcache);
		//printk(KERN_INFO"%s: databuf: %X\n", __func__, (unsigned int)readbuf->databuf);
	}
	#endif
	
	#ifdef CONFIG_MSTAR_NAND_PAGE_CACHE
	if(pNandDrv->u16_BlkPageCnt < 128)
		cache_page = kzalloc(128 * sizeof(struct page *), GFP_KERNEL);
	else
		cache_page = kzalloc(pNandDrv->u16_BlkPageCnt * sizeof(struct page *), GFP_KERNEL);
	
	if(cache_page)
	{
		INIT_RADIX_TREE(&radix_tree_root, GFP_KERNEL);
		INIT_LIST_HEAD(&page_cache_chain); 
		register_shrinker(&mmc_pagecache_shrinker);
		nr_cache_pages = 0;
		
		if(NAND_PAGE_SIZE_8K == nand_mtd->writesize)	
			nand_page_size = 8192;	
		else if(NAND_PAGE_SIZE_4K == nand_mtd->writesize)		
			nand_page_size = 4096;
	}
	#endif
	#ifdef CONFIG_MSTAR_NAND_MAX_CACHE_WRITE
	INIT_LIST_HEAD(&wcache);
	for(u16_i=0 ; u16_i<CONFIG_MSTAR_NAND_MAX_CACHE_WRITE ; u16_i++) {
		writebuf = kzalloc(sizeof(struct mstar_write_cache_buf), GFP_KERNEL);
		if(!writebuf) {
			printk(KERN_INFO"%s: Error: failed to allocate cache buffer\n", __func__);
		}
		writebuf->databuf = kmalloc(nand_mtd->writesize, GFP_KERNEL);
		if(!writebuf->databuf) {
			printk(KERN_INFO"%s: Error: failed to allocate cache buffer\n", __func__);
		}
		writebuf->pagebuf = -1;
		list_add(&writebuf->list, &wcache);
		//printk(KERN_INFO"%s: databuf: %X\n", __func__, (unsigned int)readbuf->databuf);
	}
	#endif

	nand_mtd->write = _unfd_nand_write;
	nand_mtd->read 	= _unfd_nand_read;
	nand_mtd->erase = mstar_nand_erase;
	
	#ifdef CONFIG_MTD_UBI_WRITE_CALLBACK
	nand_mtd->write_cb = mstar_nand_write_cb;
	#endif
	
	#endif
	
	#endif
	
	#ifdef CONFIG_MTD_CMDLINE_PARTS
    {
        int mtd_parts_nb = 0;
        struct mtd_partition *mtd_parts = 0;

        mtd_parts_nb = parse_cmdline_partitions(nand_mtd, &mtd_parts, "nand");

        if (mtd_parts_nb > 0)
		{
            add_mtd_partitions(nand_mtd, mtd_parts, mtd_parts_nb);
            printk("parse_cmdline_partitions: add_mtd_partitions\n");
        }
        else
		{
            add_mtd_partitions(nand_mtd, partition_info, NUM_PARTITIONS); // use default partition table if parsing cmdline partition fail
        }
    }
	#else
	add_mtd_partitions(nand_mtd, partition_info, NUM_PARTITIONS);
	#endif

    return 0;
}

static void __exit mstar_nand_cleanup(void)
{
}

module_init(mstar_nand_init);
module_exit(mstar_nand_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MStar");
MODULE_DESCRIPTION("MStar MTD NAND driver");

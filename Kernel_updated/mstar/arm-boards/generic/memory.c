/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * PROM library functions for acquiring/using memory descriptors given to
 * us from the YAMON.
 */
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/pfn.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/binfmts.h>
//#include <asm/bootinfo.h>
#include <asm/page.h>
#include <asm/sections.h>
#include <asm/arm-boards/prom.h>
#include "Board.h" // #include "../titania2/board/Board.h"
/*#define DEBUG*/

enum yamon_memtypes {
    yamon_dontuse,
    yamon_prom,
    yamon_free,
};
//static struct prom_pmemblock mdesc[PROM_MAX_PMEMBLOCKS];

#ifdef DEBUG
static char *mtypes[3] = {
    "Dont use memory",
    "YAMON PROM memory",
    "Free memmory",
};
#endif

/* determined physical memory size, not overridden by command line args  */
unsigned long physical_memsize = 0L;
#if 0
static struct prom_pmemblock * __init prom_getmdesc(void)
{
    char *memsize_str;
    unsigned int memsize;
    char cmdline[CL_SIZE], *ptr;

    /* otherwise look in the environment */
    memsize_str = prom_getenv("memsize");
    if (!memsize_str) {
        printk(KERN_WARNING
               "memsize not set in boot prom, set to default (32Mb)\n");
        physical_memsize = 0x02000000;
    } else {
#ifdef DEBUG
        pr_debug("prom_memsize = %s\n", memsize_str);
#endif
        physical_memsize = simple_strtol(memsize_str, NULL, 0);
    }

#ifdef CONFIG_CPU_BIG_ENDIAN
    /* SOC-it swaps, or perhaps doesn't swap, when DMA'ing the last
       word of physical memory */
    physical_memsize -= PAGE_SIZE;
#endif

    /* Check the command line for a memsize directive that overrides
       the physical/default amount */
    strcpy(cmdline, arcs_cmdline);
    ptr = strstr(cmdline, "memsize=");
    if (ptr && (ptr != cmdline) && (*(ptr - 1) != ' '))
        ptr = strstr(ptr, " memsize=");

    if (ptr)
        memsize = memparse(ptr + 8, &ptr);
    else
        memsize = physical_memsize;

    memset(mdesc, 0, sizeof(mdesc));

    mdesc[0].type = yamon_dontuse;
    mdesc[0].base = 0x00000000;
    mdesc[0].size = 0x00001000;

    mdesc[1].type = yamon_prom;
    mdesc[1].base = 0x00001000;
    mdesc[1].size = 0x000ef000;

#ifdef CONFIG_MIPS_MALTA
    /*
     * The area 0x000f0000-0x000fffff is allocated for BIOS memory by the
     * south bridge and PCI access always forwarded to the ISA Bus and
     * BIOSCS# is always generated.
     * This mean that this area can't be used as DMA memory for PCI
     * devices.
     */
    mdesc[2].type = yamon_dontuse;
    mdesc[2].base = 0x000f0000;
    mdesc[2].size = 0x00010000;
#else
    mdesc[2].type = yamon_prom;
    mdesc[2].base = 0x000f0000;
    mdesc[2].size = 0x00010000;
#endif

    mdesc[3].type = yamon_dontuse;
    mdesc[3].base = 0x00100000;
    mdesc[3].size = CPHYSADDR(PFN_ALIGN((unsigned long)&_end)) - mdesc[3].base;

    mdesc[4].type = yamon_free;
    mdesc[4].base = CPHYSADDR(PFN_ALIGN(&_end));
    mdesc[4].size = memsize - mdesc[4].base;

    return &mdesc[0];
}

static int __init prom_memtype_classify(unsigned int type)
{
    switch (type) {
    case yamon_free:
        return BOOT_MEM_RAM;
    case yamon_prom:
        return BOOT_MEM_ROM_DATA;
    default:
        return BOOT_MEM_RESERVED;
    }
}
#endif

//DRAMlen is the length of DRAM mapping area,not the actual length of DRAM
static unsigned long LXmem=0, LXmem2Addr=0,LXmem2Size=0,EMACmem=0, DRAMlen=0, BBAddr=0;
static unsigned long G3Dmem0Addr=0, G3Dmem0Len=0, G3Dmem1Addr=0, G3Dmem1Len=0, G3DCmdQAddr=0, G3DCmdQLen=0;
static unsigned long LXmem3Addr=0,LXmem3Size=0;
static unsigned int  Kernel_Debug=0;
#if (MP_CORE_PATH==1)
static char coredump_path[CORENAME_MAX_SIZE]={0};
#endif
#ifdef CONFIG_MSTAR_STR_CRC
static unsigned int str_crc=0;
#endif

unsigned long lx_mem_addr = PHYS_OFFSET;
unsigned long lx_mem_size = 0xFFFFFFFF; //default setting
unsigned long lx_mem2_addr = 0xFFFFFFFF; //default setting
unsigned long lx_mem2_size = 0xFFFFFFFF; //default setting
unsigned long lx_mem3_addr = 0xFFFFFFFF; //default setting

EXPORT_SYMBOL(lx_mem_addr);
EXPORT_SYMBOL(lx_mem_size);
EXPORT_SYMBOL(lx_mem2_addr);
EXPORT_SYMBOL(lx_mem2_size);
EXPORT_SYMBOL(lx_mem3_addr);

# define NR_BANKS 8

struct membank {
        unsigned long start;
        unsigned long size;
        unsigned int highmem;
};

struct meminfo {
        int nr_banks;
        struct membank bank[NR_BANKS];
};

extern struct meminfo meminfo;

static int __init LX_MEM_setup(char *str)
{
    //printk("LX_MEM= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx",&LXmem);
        meminfo.nr_banks = 0;
	lx_mem_size = LXmem & ~(0x100000 - 1);
    }
    else
    {
        printk("\nLX_MEM not set\n");
    }
    return 0;
}

static int __init EMAC_MEM_setup(char *str)
{
   //printk("EMAC_MEM= %s\n", str);
    if( str != NULL )
    {
        EMACmem = simple_strtol(str, NULL, 16);
    }
    else
    {
        printk("\nEMAC_MEM not set\n");
    }
    return 0;
}

static int __init DRAM_LEN_setup(char *str)
{
    //printk("DRAM_LEN= %s\n", str);
    if( str != NULL )
    {
        DRAMlen = simple_strtol(str, NULL, 16);
    }
    else
    {
        printk("\nDRAM_LEN not set\n");
    }
    return 0;
}

static int __init LX_MEM2_setup(char *str)
{
    //printk("LX_MEM2= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx,%lx",&LXmem2Addr,&LXmem2Size);
        lx_mem2_addr = LXmem2Addr & ~(0x100000 - 1);
        lx_mem2_size = LXmem2Size;
    }
    else
    {
        printk("\nLX_MEM2 not set\n");
    }
    return 0;
}

static int __init LX_MEM3_setup(char *str)
{
    //printk("LX_MEM3= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx,%lx",&LXmem3Addr,&LXmem3Size);
        lx_mem3_addr = LXmem3Addr & ~(0x100000 - 1);
    }
    else
    {
        printk("\nLX_MEM3 not set\n");
    }
    return 0;
}

static int __init G3D_MEM_setup(char *str)
{
    if( str != NULL )
    {
        sscanf(str,"%lx,%lx,%lx,%lx,%lx,%lx",
        &G3Dmem0Addr,&G3Dmem0Len,&G3Dmem1Addr,&G3Dmem1Len,
        &G3DCmdQAddr,&G3DCmdQLen);
    }
    else
    {
        printk("\nG3D_MEM not set\n");
    }
    return 0;
}


static int __init BBAddr_setup(char *str)
{
    //printk("LX_MEM2= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx",&BBAddr);
    }

    return 0;
}

static int __init KDebug_setup(char *str)
{
   printk("KDebug = %s\n", str);
    if( str != NULL )
    {
        Kernel_Debug = simple_strtol(str, NULL, 16);
    }
    else
    {
        printk("\nKDebug not set\n");
    }
    return 0;
}

#if (MP_CORE_PATH==1)
static int __init Coredump_setup(char *str)
{
    if( str != NULL)
    {
        strncpy(coredump_path, str, CORENAME_MAX_SIZE);
    }
    else
    {
        printk("depend on core pattern\n");
    }
    return 0;
}
#endif
#ifdef CONFIG_MSTAR_STR_CRC
static int __init str_crc_setup(char *str)
{
    if( str != NULL )
    {
        str_crc = simple_strtol(str, NULL, 16);
    }
    else
    {
        printk("\nstr_crc is disable\n");
    }
    printk("\nstr_crc = %d\n", str_crc);
    return 0;
}
#endif
early_param("LX_MEM", LX_MEM_setup);
early_param("EMAC_MEM", EMAC_MEM_setup);
early_param("DRAM_LEN", DRAM_LEN_setup);
early_param("LX_MEM2", LX_MEM2_setup);
early_param("LX_MEM3", LX_MEM3_setup);
early_param("BB_ADDR", BBAddr_setup);
early_param("G3D_MEM", G3D_MEM_setup);
early_param("KDebug", KDebug_setup);
#if (MP_CORE_PATH==1)
early_param("CORE_DUMP_PATH", Coredump_setup);
#endif
#ifdef CONFIG_MSTAR_STR_CRC
early_param("str_crc", str_crc_setup);
#endif

static char bUseDefMMAP=0;
#if 0
static void check_boot_mem_info(void)
{
    if( LXmem==0 || EMACmem==0 || DRAMlen==0 )
    {
        bUseDefMMAP = 1;
    }
}
#endif

#if defined(CONFIG_MSTAR_OFFSET_FOR_SBOOT)
#define SBOOT_LINUX_MEM_START 0x00400000    //4M
#define SBOOT_LINUX_MEM_LEN   0x01400000    //20M
#define SBOOT_EMAC_MEM_LEN    0x100000      //1M
void get_boot_mem_info_sboot(BOOT_MEM_INFO type, unsigned int *addr, unsigned int *len)
{
    switch (type)
    {
    case LINUX_MEM:
        *addr = SBOOT_LINUX_MEM_START;
        *len = SBOOT_LINUX_MEM_LEN;
        break;

    case EMAC_MEM:
        *addr = SBOOT_LINUX_MEM_START+SBOOT_LINUX_MEM_LEN;
        *len = SBOOT_EMAC_MEM_LEN;
        break;

    case MPOOL_MEM:
        *addr = SBOOT_LINUX_MEM_START + SBOOT_LINUX_MEM_LEN + SBOOT_EMAC_MEM_LEN;
        *len = 256*1024*1024;
        break;

    case LINUX_MEM2:
        *addr = 0;
        *len = 0;
        break;

    case LINUX_MEM3:
        *addr = 0;
        *len = 0;
        break;

    default:
        *addr = 0;
        *len = 0;
        break;
    }
}
#endif//CONFIG_MSTAR_OFFSET_FOR_SBOOT


void get_boot_mem_info(BOOT_MEM_INFO type, unsigned int *addr, unsigned int *len)
{
#if defined(CONFIG_MSTAR_OFFSET_FOR_SBOOT)
    get_boot_mem_info_sboot(type, addr, len);
    printk("!!!!!!!!!!SBOOT memory type=%x addr=%x, len=%x!!!!!!!!!!\n",type, *addr, *len);
    return;
#endif//CONFIG_MSTAR_OFFSET_FOR_SBOOT

    if (bUseDefMMAP == 0)
    {
        switch (type)
        {
        case LINUX_MEM:
            *addr = PHYS_OFFSET;
            *len = LXmem;
            break;

        case EMAC_MEM:
            *addr = PHYS_OFFSET + LXmem;
            *len = EMACmem;
            break;

        case MPOOL_MEM:
            *addr = LINUX_MEM_BASE_ADR + LXmem + EMACmem;
            *len = DRAMlen - *addr;
            break;

        case LINUX_MEM2:
            if (LXmem2Addr!=0 && LXmem2Size!=0)
            {
                *addr = LXmem2Addr;
                *len = LXmem2Size;
            }
            else
            {
                *addr = 0;
                *len = 0;
            }
            break;
        case LINUX_MEM3:
            if (LXmem3Addr!=0 && LXmem3Size!=0)
            {
                *addr = LXmem3Addr;
                *len = LXmem3Size;
            }
            else
            {
                *addr = 0;
                *len = 0;
            }
            break;
        case G3D_MEM0:
            *addr = G3Dmem0Addr;
            *len = G3Dmem0Len;
            break;
        case G3D_MEM1:
            *addr = G3Dmem1Addr;
            *len = G3Dmem1Len;
            break;
        case G3D_CMDQ:
            *addr = G3DCmdQAddr;
            *len = G3DCmdQLen;
            break;
        case DRAM:
            *addr = 0;
            *len = DRAMlen;
            break;
        case BB:
            *addr = BBAddr;
            *len = 0;
            break;
        default:
            *addr = 0;
            *len = 0;
            break;
        }
    }
    else
    {
        switch (type)
        {
        case LINUX_MEM:
            *addr =PHYS_OFFSET;
            *len = LINUX_MEM_LEN;
            break;

        case EMAC_MEM:
            *addr = EMAC_MEM_ADR;
            *len = EMAC_MEM_LEN;
            break;

        case MPOOL_MEM:
            *addr = MPOOL_ADR;
            *len = MPOOL_LEN;
            break;

        case LINUX_MEM2:
            if (LXmem2Addr!=0 && LXmem2Size!=0)
            {
                *addr = LXmem2Addr;
                *len  = LXmem2Size;
            }
            else
            {
                #ifdef LINUX_MEM2_BASE_ADR    // reserved miu1 memory for linux
                *addr = LINUX_MEM2_BASE_ADR;
                *len = LINUX_MEM2_LEN;
                printk("LINUX_MEM2 addr %08x, len %08x\n", *addr, *len);
                #else
                *addr = 0;
                *len = 0;
                #endif
            }
            break;
        case LINUX_MEM3:
            if (LXmem3Addr!=0 && LXmem3Size!=0)
            {
                *addr = LXmem3Addr;
                *len  = LXmem3Size;
            }
            else
            {
                *addr = 0;
                *len = 0;
            }
            break;
        case G3D_MEM0:
            *addr = G3Dmem0Addr;
            *len = G3Dmem0Len;
            break;
        case G3D_MEM1:
            *addr = G3Dmem1Addr;
            *len = G3Dmem1Len;
            break;
        case G3D_CMDQ:
            *addr = G3DCmdQAddr;
            *len = G3DCmdQLen;
            break;
        case DRAM:
            *addr = 0;
            *len = DRAMlen;
            break;
        case BB:
            *addr = BBAddr;
            *len = 0;
            break;
        default:
            *addr = 0;
            *len = 0;
            break;
        }
    }
}
EXPORT_SYMBOL(get_boot_mem_info);

extern void early_putstr(const char *fmt, ...);
extern int  arm_add_memory(unsigned long start, unsigned long size);
void __init prom_meminit(void)
{

    unsigned int linux_memory_address = 0, linux_memory_length = 0;
    unsigned int linux_memory2_address = 0, linux_memory2_length = 0;
    unsigned int linux_memory3_address = 0, linux_memory3_length = 0;

    //check_boot_mem_info();
    get_boot_mem_info(LINUX_MEM, &linux_memory_address, &linux_memory_length);
    get_boot_mem_info(LINUX_MEM2, &linux_memory2_address, &linux_memory2_length);
    get_boot_mem_info(LINUX_MEM3, &linux_memory3_address, &linux_memory3_length);

    if ((linux_memory_address | linux_memory_length | linux_memory2_address | linux_memory2_length
                | linux_memory3_address | linux_memory3_length) & (0x100000-1))
    {
        early_putstr("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to 1MB");
        while(1);
    }

    if (linux_memory_length != 0)
        arm_add_memory(linux_memory_address, linux_memory_length);
    if (linux_memory2_length != 0)
        arm_add_memory(linux_memory2_address, linux_memory2_length);
    if (linux_memory3_length != 0)
        arm_add_memory(linux_memory3_address, linux_memory3_length);

    printk("\n");
    printk("LX_MEM  = 0x%X, 0x%X\n", linux_memory_address,linux_memory_length);
    printk("LX_MEM2 = 0x%X, 0x%X\n",linux_memory2_address, linux_memory2_length);
    printk("LX_MEM3 = 0x%X, 0x%X\n",linux_memory3_address, linux_memory3_length);
    //printk("CPHYSADDR(PFN_ALIGN(&_end))= 0x%X\n", CPHYSADDR(PFN_ALIGN(&_end)));
    printk("EMAC_LEN= 0x%lX\n", EMACmem);
    printk("DRAM_LEN= 0x%lX\n", DRAMlen);

    #if defined(CONFIG_MSTAR_AGATE) || \
        defined(CONFIG_MSTAR_AMBER3) || \
        defined(CONFIG_MSTAR_EAGLE) || \
        defined(CONFIG_MSTAR_EDISON) || \
        defined(CONFIG_MSTAR_EIFFEL)
        if ( DRAMlen > 0x20000000 )
            printk("ENV VARIABLE DRAM_LEN SETTING ERROR!!! LARGER THAN MAXIMUM MAPPING SIZE!!!\n");
    #endif
}

inline unsigned long get_BBAddr(void)
{
    return BBAddr;
}

//void __init prom_free_prom_memory(void)
//{
//    unsigned long addr;
//    int i;
//
//    for (i = 0; i < boot_mem_map.nr_map; i++) {
//        if (boot_mem_map.map[i].type != BOOT_MEM_ROM_DATA)
//            continue;
//
//        addr = boot_mem_map.map[i].addr;
//        free_init_pages("prom memory",
//                addr, addr + boot_mem_map.map[i].size);
//    }
//}

int Kernel_Level(void)
{
 printk("\n Kernel_Level test \n");
 return Kernel_Debug;
}

EXPORT_SYMBOL(Kernel_Level);
#if (MP_CORE_PATH==1)
char* get_coredump_path(void)
{
    return coredump_path;
}
EXPORT_SYMBOL(get_coredump_path);
#endif
#ifdef CONFIG_MSTAR_STR_CRC
int get_str_crc(void)
{
    return str_crc;
}
EXPORT_SYMBOL(get_str_crc);
#endif

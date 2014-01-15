////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

//#include <linux/config.h>
#include <linux/autoconf.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/resource.h>
#include <linux/platform_device.h>

#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#endif

#include "chip_int.h"

//#define ENABLE_THIRD_EHC

#define UTMI_BASE_ADDRESS_START             0xbf207500
#define UTMI_BASE_ADDRESS_END               0xbf2075FC
#define USB_HOST20_ADDRESS_START            0xbf204800
#define USB_HOST20_ADDRESS_END              0xbf2049FC
//tony add for 2st EHCI
#define SECOND_UTMI_BASE_ADDRESS_START      0xbf207400
#define SECOND_UTMI_BASE_ADDRESS_END        0xbf2074FC
#define SECOND_USB_HOST20_ADDRESS_START     0xbf201A00
#define SECOND_USB_HOST20_ADDRESS_END       0xbf201BFC

#if defined(ENABLE_THIRD_EHC)
#define THIRD_UTMI_BASE_ADDRESS_START       0xBF207200
#define THIRD_UTMI_BASE_ADDRESS_END         0xBF2072FC
#define THIRD_USB_HOST20_ADDRESS_START      0xbf227200
#define THIRD_USB_HOST20_ADDRESS_END        0xbf2273FC
#endif

static struct resource Mstar_usb_ehci_resources[] = 
{
	[0] = 
	{
		.start		= UTMI_BASE_ADDRESS_START,
		.end		= UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = 
	{
		.start		= USB_HOST20_ADDRESS_START,
		.end		= USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = 
	{
		.start		= E_IRQ_UHC,
		.end		= E_IRQ_UHC,
		.flags		= IORESOURCE_IRQ,
	},
};

//tony add for 2st EHCI
static struct resource Second_Mstar_usb_ehci_resources[] = 
{
	[0] = 
	{
		.start		= SECOND_UTMI_BASE_ADDRESS_START,
		.end		= SECOND_UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = 
	{
		.start		= SECOND_USB_HOST20_ADDRESS_START,
		.end		= SECOND_USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = 
	{
		.start		= E_IRQEXPL_UHC1,
		.end		= E_IRQEXPL_UHC1,
		.flags		= IORESOURCE_IRQ,
	},
};

#ifdef ENABLE_THIRD_EHC
static struct resource Third_Mstar_usb_ehci_resources[] = 
{
	[0] = 
	{
		.start		= THIRD_UTMI_BASE_ADDRESS_START,
		.end		= THIRD_UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = 
	{
		.start		= THIRD_USB_HOST20_ADDRESS_START,
		.end		= THIRD_USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = 
	{
		.start		= E_IRQEXPL_UHC2,
		.end		= E_IRQEXPL_UHC2,
		.flags		= IORESOURCE_IRQ,
	},
};
#endif

/* The dmamask must be set for EHCI to work */
//static u64 ehci_dmamask = ~(u32)0;
static u64 ehci_dmamask = 0x7ffffff;	//tony add for limit DMA range

static struct platform_device Mstar_usb_ehci_device = 
{
	.name = "Mstar-ehci-1",
	.id = 0,
	.dev = 
	{
		.dma_mask = &ehci_dmamask,
		.coherent_dma_mask = 0x7ffffff, //tony add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Mstar_usb_ehci_resources),
	.resource = Mstar_usb_ehci_resources,
};
//tony add for 2st EHCI
static struct platform_device Second_Mstar_usb_ehci_device = 
{
	.name = "Mstar-ehci-2",
	.id = 1,
	.dev = 
	{
		.dma_mask = &ehci_dmamask,
		.coherent_dma_mask = 0x7ffffff, 	//tony add for limit DMA range
	},
	.num_resources = ARRAY_SIZE(Second_Mstar_usb_ehci_resources),
	.resource = Second_Mstar_usb_ehci_resources,
};

#ifdef ENABLE_THIRD_EHC
static struct platform_device Third_Mstar_usb_ehci_device = {
	.name = "Mstar-ehci-3",
	.id	= 2,
	.dev = 
	{
		.dma_mask = &ehci_dmamask,
		.coherent_dma_mask = 0x7ffffff, 	//tony add for limit DMA range
	},
	.num_resources = ARRAY_SIZE(Third_Mstar_usb_ehci_resources),
	.resource = Third_Mstar_usb_ehci_resources,
};
#endif

#ifdef CONFIG_ANDROID_PMEM

#include "board/Board.h"

#define PMEM0_BASE_SIZE                     0x00
#define PMEM0_BASE                          0x00
#define PMEM1_BASE_SIZE                     0x00
#define PMEM1_BASE                          0x00

static unsigned long SN_PMEM0_ADDR = 0;
static unsigned long SN_PMEM0_SIZE = 0;
static unsigned long SN_PMEM1_ADDR = 0;
static unsigned long SN_PMEM1_SIZE = 0;

static int __init SN_PMEM0_setup(char *str)
{
    if(str != NULL)
    {
        sscanf(str,"%lx,%lx",&SN_PMEM0_ADDR,&SN_PMEM0_SIZE);
    }
    else
    {
        printk("\nPMEM0 not set\n");
    }
    return 0;
}
static int __init SN_PMEM1_setup(char *str)
{
    if(str != NULL)
    {
        sscanf(str,"%lx,%lx",&SN_PMEM1_ADDR,&SN_PMEM1_SIZE);
    }
    else
    {
        printk("\nPMEM1  not set\n");
    }
    return 0;
}

early_param("SN_PMEM0", SN_PMEM0_setup);
early_param("SN_PMEM1", SN_PMEM1_setup);

static struct android_pmem_platform_data android_pmem0_pdata = 
{
    .name = "pmem0",
    .start = PMEM0_BASE,
    .size = PMEM0_BASE_SIZE,
    .no_allocator = 0,
    .cached = 0,
};

struct platform_device android_pmem0_device = 
{
    .name = "android_pmem",
    .id = 0,
    .dev = 
    { 
        .platform_data = &android_pmem0_pdata 
    },
};

static struct android_pmem_platform_data android_pmem1_pdata = 
{
    .name = "pmem1",
    .start = PMEM1_BASE,
    .size = PMEM1_BASE_SIZE,
    .no_allocator = 0,
    .cached = 0,
};

struct platform_device android_pmem1_device = 
{
    .name = "android_pmem",
    .id = 1,
    .dev = 
    { 
        .platform_data = &android_pmem1_pdata 
    },
};
#endif

#ifdef CONFIG_RTC_DRV_MSTAR
struct platform_device rtc_mstar_dev = 
{
    .name = "rtc-mstar",
    .id = -1,
};
#endif

static struct platform_device Mstar_emac_device = {
	.name		= "Mstar-emac",
	.id		= 0,
};

static struct platform_device Mstar_ir_device = {
	.name		= "Mstar-ir",
	.id		= 0,
};

static struct platform_device Mstar_nand_device = {
	.name		= "Mstar-nand",
	.id		= 0,
};

static struct platform_device Mstar_mbx_device = {
	.name		= "Mstar-mbx",
	.id		= 0,
};

static struct platform_device Mstar_gflip_device = {
	.name		= "Mstar-gflip",
	.id		= 0,
};

//static struct platform_device *Mstar_platform_devices[] __initdata = {
static struct platform_device *Mstar_platform_devices[] = 
{
	&Mstar_usb_ehci_device,
	&Second_Mstar_usb_ehci_device,	 //tony add for 2st EHCI

#ifdef ENABLE_THIRD_EHC
	&Third_Mstar_usb_ehci_device,
#endif

#ifdef CONFIG_ANDROID_PMEM
    &android_pmem0_device,
    &android_pmem1_device,
#endif

#ifdef CONFIG_RTC_DRV_MSTAR
    &rtc_mstar_dev,
#endif

    &Mstar_emac_device,
    &Mstar_ir_device,
    &Mstar_nand_device,
    &Mstar_mbx_device,
    &Mstar_gflip_device,
};

int Mstar_platform_init(void)
{
#ifdef CONFIG_ANDROID_PMEM
    android_pmem0_pdata.start = SN_PMEM0_ADDR;
    android_pmem0_pdata.size = SN_PMEM0_SIZE;
    android_pmem1_pdata.start = SN_PMEM1_ADDR;
    android_pmem1_pdata.size = SN_PMEM1_SIZE;
    //printk("\nSN_PMEM0_ADDR:%lx SN_PMEM0_SIZE:%lx SN_PMEM1_ADDR:%lx:SN_PMEM1_SIZE:%lx\n",SN_PMEM0_ADDR,SN_PMEM0_SIZE,SN_PMEM1_ADDR,SN_PMEM1_SIZE);
#endif
	return platform_add_devices(Mstar_platform_devices, ARRAY_SIZE(Mstar_platform_devices));
}

arch_initcall(Mstar_platform_init);


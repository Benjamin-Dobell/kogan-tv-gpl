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

#include <mach/irqs.h>
#include "chip_int.h"



#if defined(CONFIG_MSTAR_TITANIA8) || defined(CONFIG_MSTAR_TITANIA12) || \
	defined(CONFIG_MSTAR_AMBER3) || defined(CONFIG_MSTAR_AMBER5) || defined(CONFIG_MSTAR_AGATE)
#define ENABLE_THIRD_EHC
#endif

#if defined(CONFIG_MSTAR_AGATE)
	#if  defined(CONFIG_USB_XHCI_HCD) || defined(CONFIG_USB_XHCI_HCD_MODULE) 
	#define ENABLE_XHC
	#endif
#endif

#define UTMI_BASE_ADDRESS_START		0xFD207500
#define UTMI_BASE_ADDRESS_END		0xFD2075FC
#define USB_HOST20_ADDRESS_START	0xFD204800
#define USB_HOST20_ADDRESS_END		0xFD2049FC
//tony add for 2st EHCI
#define SECOND_UTMI_BASE_ADDRESS_START	0xFD207400
#define SECOND_UTMI_BASE_ADDRESS_END	0xFD2074FC
#define SECOND_USB_HOST20_ADDRESS_START	0xFD201A00
#define SECOND_USB_HOST20_ADDRESS_END	0xFD201BFC

#define THIRD_UTMI_BASE_ADDRESS_START	0xFD207200
#define THIRD_UTMI_BASE_ADDRESS_END	0xFD2072FC
#define THIRD_USB_HOST20_ADDRESS_START	0xFD227200
#define THIRD_USB_HOST20_ADDRESS_END	0xFD2273FC

#define XHCI_ADDRESS_START           0xFD320000
#define XHCI_ADDRESS_END             0xFD327fff
#define U3PHY_ADDRESS_START          0xFD244200
#define U3PHY_ADDRESS_END            0xFD244BFF


static struct resource Mstar_usb_ehci_resources[] = {
	[0] = {
		.start		= UTMI_BASE_ADDRESS_START,
		.end		= UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= USB_HOST20_ADDRESS_START,
		.end		= USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = {
		.start		= E_IRQ_UHC,
		.end		= E_IRQ_UHC,
		.flags		= IORESOURCE_IRQ,
	},
};

//tony add for 2st EHCI
static struct resource Second_Mstar_usb_ehci_resources[] = {
	[0] = {
		.start		= SECOND_UTMI_BASE_ADDRESS_START,
		.end		= SECOND_UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
        },
	[1] = {
		.start		= SECOND_USB_HOST20_ADDRESS_START,
		.end		= SECOND_USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = {
		.start		= E_IRQEXPL_UHC1,
		.end		= E_IRQEXPL_UHC1,
		.flags		= IORESOURCE_IRQ,
	},
};

#ifdef ENABLE_THIRD_EHC
static struct resource Third_Mstar_usb_ehci_resources[] = {
	[0] = {
		.start		= THIRD_UTMI_BASE_ADDRESS_START,
		.end		= THIRD_UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= THIRD_USB_HOST20_ADDRESS_START,
		.end		= THIRD_USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = {
		.start		= E_IRQEXPL_UHC2,
		.end		= E_IRQEXPL_UHC2,
		.flags		= IORESOURCE_IRQ,
	},
};
#endif

/* The dmamask must be set for EHCI to work */
//static u64 ehci_dmamask = ~(u32)0;
static u64 ehci_dmamask = 0x7ffffff;    //tony add for limit DMA range

static struct platform_device Mstar_usb_ehci_device = {
	.name           = "Mstar-ehci-1",
	.id             = 0,
	.dev = {
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff, //tony add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Mstar_usb_ehci_resources),
	.resource	= Mstar_usb_ehci_resources,
};
//tony add for 2st EHCI
static struct platform_device Second_Mstar_usb_ehci_device = {
	.name		= "Mstar-ehci-2",
	.id		= 1,
	.dev = {
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,    //tony add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Second_Mstar_usb_ehci_resources),
	.resource	= Second_Mstar_usb_ehci_resources,
};
#ifdef ENABLE_THIRD_EHC
static struct platform_device Third_Mstar_usb_ehci_device = {
	.name		= "Mstar-ehci-3",
	.id		= 2,
	.dev = {
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,    //tony add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Third_Mstar_usb_ehci_resources),
	.resource	= Third_Mstar_usb_ehci_resources,
};
#endif

#ifdef ENABLE_XHC
//-----------------------------------------
//   xHCI platform device
//-----------------------------------------
static struct resource Mstar_usb_xhci_resources[] = {
	[0] = {
		.start		= U3PHY_ADDRESS_START,
		.end		= U3PHY_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= XHCI_ADDRESS_START,
		.end		= XHCI_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = {
		.start		= E_IRQ_UHC30,
		.end		= E_IRQ_UHC30,
		.flags		= IORESOURCE_IRQ,
	},
};

static u64 xhci_dmamask = 0xffffffff;

static struct platform_device Mstar_usb_xhci_device = {
	.name		= "Mstar-xhci-1",
	.id		= 0,
	.dev = {
		.dma_mask		= &xhci_dmamask,
		.coherent_dma_mask	= 0xffffffff, 
	},
	.num_resources	= ARRAY_SIZE(Mstar_usb_xhci_resources),
	.resource	= Mstar_usb_xhci_resources,

};
//---------------------------------------------------------------
#endif

		
#ifdef CONFIG_ANDROID_PMEM
#include "board/Board.h"
#define PMEM0_BASE_SIZE 0X0
#define PMEM0_BASE 0X0



#define PMEM1_BASE_SIZE 0x0
#define PMEM1_BASE 0x0

static unsigned long SN_PMEM0_ADDR = 0;
static unsigned long SN_PMEM0_SIZE = 0;
static unsigned long SN_PMEM1_ADDR = 0;
static unsigned long SN_PMEM1_SIZE = 0;
static int __init SN_PMEM0_setup(char *str)
{

    if( str != NULL )
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

    if( str != NULL )
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


static struct android_pmem_platform_data android_pmem0_pdata = {
       .name = "pmem0",
       .start = PMEM0_BASE,
       .size = PMEM0_BASE_SIZE,
       .no_allocator = 0,
       .cached = 0,
};

struct platform_device android_pmem0_device = {
       .name = "android_pmem",
       .id = 0,
       .dev = { .platform_data = &android_pmem0_pdata },
};

static struct android_pmem_platform_data android_pmem1_pdata = {
       .name = "pmem1",
       .start = PMEM1_BASE,
       .size = PMEM1_BASE_SIZE,
       .no_allocator = 0,
       .cached = 0,
};

struct platform_device android_pmem1_device = {
       .name = "android_pmem",
       .id = 1,
       .dev = { .platform_data = &android_pmem1_pdata },
};
#endif

#ifdef CONFIG_RTC_DRV_MSTAR
struct platform_device rtc_mstar_dev = {
        .name = "rtc-mstar",
        .id = -1,
};
#endif

//static struct platform_device *Mstar_platform_devices[] __initdata = {
static struct platform_device *Mstar_platform_devices[] = {
	&Second_Mstar_usb_ehci_device,   //tony add for 2st EHCI
	&Mstar_usb_ehci_device,
#ifdef ENABLE_THIRD_EHC
	&Third_Mstar_usb_ehci_device,
#endif
#ifdef ENABLE_XHC
    &Mstar_usb_xhci_device,    
#endif
#ifdef CONFIG_ANDROID_PMEM
    &android_pmem0_device,
    &android_pmem1_device,
#endif
#ifdef CONFIG_RTC_DRV_MSTAR
    &rtc_mstar_dev,
#endif
};

int Mstar_ehc_platform_init(void)
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

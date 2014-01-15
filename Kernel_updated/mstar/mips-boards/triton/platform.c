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

#include <linux/config.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/resource.h>

#include "chip_int.h"

#define UTMI_BASE_ADDRESS_START 0xbf806200
#define UTMI_BASE_ADDRESS_END 0xbf8062FC
#define USB_HOST20_ADDRESS_START 0xbf805c00
#define USB_HOST20_ADDRESS_END 0xbf805cFC

static struct resource Mstar_usb_ehci_resources[] = {
	[0] = {
		.start		= UTMI_BASE_ADDRESS_START,
		.end		= UTMI_BASE_ADDRESS_START,
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

/* The dmamask must be set for EHCI to work */
#ifdef CONFIG_Triton
static u64 ehci_dmamask = ~(u32)0;
#else
static u64 ehci_dmamask = 0xffffff;	//tony limited range for MIU0
#endif

static struct platform_device Mstar_usb_ehci_device = {
	.name		= "Mstar-soc-ehci",
	.id		= 0,
	.dev = {
		.dma_mask		= &ehci_dmamask,
#ifdef CONFIG_Triton
		.coherent_dma_mask	= 0xffffffff,
#else
		.coherent_dma_mask	= 0xffffff,	//tony limited range for MIU0
#endif
	},
	.num_resources	= ARRAY_SIZE(Mstar_usb_ehci_resources),
	.resource	= Mstar_usb_ehci_resources,
};

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

static struct platform_device *Mstar_platform_devices[] __initdata = {
	&Mstar_usb_ehci_device,
    &Mstar_emac_device,
    &Mstar_ir_device,
    &Mstar_nand_device,
    &Mstar_mbx_device,
    &Mstar_gflip_device,
};

int Mstar_platform_init(void)
{
	return platform_add_devices(Mstar_platform_devices, ARRAY_SIZE(Mstar_platform_devices));
}

arch_initcall(Mstar_platform_init);

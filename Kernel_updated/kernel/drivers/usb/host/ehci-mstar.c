////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#include <linux/platform_device.h>
#include "ehci.h"
#include "ehci-mstar.h"
#include "../core/bc-mstar.h"		

static const struct hc_driver ehci_mstar_hc_driver = {
	.description = hcd_name,
	.product_desc = "Mstar EHCI",
	.hcd_priv_size = sizeof(struct ehci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq = ehci_irq,
	.flags = HCD_MEMORY | HCD_USB2,

	/*
	 * basic lifecycle operations
	 *
	 * FIXME -- ehci_init() doesn't do enough here.
	 * See ehci-ppc-soc for a complete implementation.
	 */
#if 1	//tony
	.reset = ehci_init,
#else
	.reset = ehci_mstar_reinit,
#endif
	.start = ehci_run,
	.stop = ehci_stop,
	.shutdown = ehci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue = ehci_urb_enqueue,
	.urb_dequeue = ehci_urb_dequeue,
	.endpoint_disable = ehci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number = ehci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data = ehci_hub_status_data,
	.hub_control = ehci_hub_control,
	.bus_suspend = ehci_bus_suspend,
	.bus_resume = ehci_bus_resume,
	.relinquish_port = ehci_relinquish_port,
	.port_handed_over = ehci_port_handed_over,

	//Colin, 101106 for external hub
	.clear_tt_buffer_complete	= ehci_clear_tt_buffer_complete,
};

void Titania3_series_start_ehc(unsigned int UTMI_base, unsigned int USBC_base, unsigned int UHC_base, unsigned int flag)
{
	printk("Titania3_series_start_ehc start\n");

	if (flag & EHCFLAG_TESTPKG)
	{
		writew(0x2084, (void*)(UTMI_base+0x2*2));
		writew(0x8051, (void*)(UTMI_base+0x20*2));
	}

#if _USB_HS_CUR_DRIVE_DM_ALLWAYS_HIGH_PATCH
	/*
	 * patch for DM always keep high issue
	 * init overwrite register
	 */
	writeb(readb((void*)(UTMI_base+0x0*2)) & ~BIT3, (void*) (UTMI_base+0x0*2)); //DP_PUEN = 0
	writeb(readb((void*)(UTMI_base+0x0*2)) & ~BIT4, (void*) (UTMI_base+0x0*2)); //DM_PUEN = 0

	writeb(readb((void*)(UTMI_base+0x0*2)) & ~BIT5, (void*) (UTMI_base+0x0*2)); //R_PUMODE = 0

	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT6, (void*) (UTMI_base+0x0*2)); //R_DP_PDEN = 1
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT7, (void*) (UTMI_base+0x0*2)); //R_DM_PDEN = 1

	writeb(readb((void*)(UTMI_base+0x10*2)) | BIT6, (void*) (UTMI_base+0x10*2)); //hs_txser_en_cb = 1
	writeb(readb((void*)(UTMI_base+0x10*2)) & ~BIT7, (void*) (UTMI_base+0x10*2)); //hs_se0_cb = 0

	/* turn on overwrite mode */
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT1, (void*) (UTMI_base+0x0*2)); //tern_ov = 1
#endif

	writeb(0x0a, (void*)(USBC_base)); // Disable MAC initial suspend, Reset UHC
	writeb(0x28, (void*)(USBC_base)); // Release UHC reset, enable UHC and OTG XIU function

	if (flag & EHCFLAG_DOUBLE_DATARATE)
	{
		if ((flag & EHCFLAG_DDR_MASK) == EHCFLAG_DDR_x15)
		{
			// Set usb bus = 480MHz x 1.5
			writeb(readb((void*)(UTMI_base+0x20*2)) | 0x76, (void*)(UTMI_base+0x20*2));
		}
		else if ((flag & EHCFLAG_DDR_MASK) == EHCFLAG_DDR_x18)
		{
			// Set usb bus = 480MHz x 1.8
			writeb(readb((void*)(UTMI_base+0x20*2)) | 0x8e, (void*)(UTMI_base+0x20*2));
		}
#if 0
		else if ((flag & EHCFLAG_DDR_MASK) == EHCFLAG_DDR_x20)
		{
			// Set usb bus = 480MHz x2
			writeb(readb((void*)(UTMI_base+0xd*2-1)) | 0x01, (void*)(UTMI_base+0xd*2-1));
		}
#endif
		/* Set slew rate control for overspeed (or 960MHz) */
		writeb(readb((void*)(UTMI_base+0x2c*2)) | BIT0, (void*) (UTMI_base+0x2c*2));
	}

	writeb(readb((void*)(UTMI_base+0x09*2-1)) & ~BIT3, (void*)(UTMI_base+0x09*2-1)); // Disable force_pll_on
	writeb(readb((void*)(UTMI_base+0x08*2)) & ~BIT7, (void*)(UTMI_base+0x08*2)); // Enable band-gap current
	writeb(0xc3, (void*)(UTMI_base)); // reg_pdn: bit<15>, bit <2> ref_pdn
	mdelay(1); // delay 1ms

	writeb(0x69, (void*)(UTMI_base+0x01*2-1)); // Turn on UPLL, reg_pdn: bit<9>
	mdelay(2); // delay 2ms

	writeb(0x01, (void*)(UTMI_base)); // Turn all (including hs_current) use override mode
	writeb(0, (void*)(UTMI_base+0x01*2-1)); // Turn on UPLL, reg_pdn: bit<9>

	writeb(0x0a, (void*)(USBC_base)); // Disable MAC initial suspend, Reset UHC
	writeb(0x28, (void*)(USBC_base)); // Release UHC reset, enable UHC XIU function

	writeb(readb((void*)(UTMI_base+0x3c*2)) | BIT0, (void*)(UTMI_base+0x3c*2)); // set CA_START as 1
	mdelay(10);

	writeb(readb((void*)(UTMI_base+0x3c*2)) & ~BIT0, (void*)(UTMI_base+0x3c*2)); // release CA_START

	while ((readb((void*)(UTMI_base+0x3c*2)) & BIT1) == 0);	// polling bit <1> (CA_END)

	if (flag & EHCFLAG_DPDM_SWAP)
		writeb(readb((void*)(UTMI_base+0x0b*2-1)) | BIT5, (void*)(UTMI_base+0x0b*2-1)); // dp dm swap

	writeb((readb((void*)(USBC_base+0x02*2)) & ~BIT1) | BIT0, (void*)(USBC_base+0x02*2)); // UHC select enable

	writeb(readb((void*)(UHC_base+0x40*2)) & ~BIT4, (void*)(UHC_base+0x40*2)); // 0: VBUS On.
	mdelay(1); // delay 1ms

	writeb(readb((void*)(UHC_base+0x40*2)) | BIT3, (void*)(UHC_base+0x40*2)); // Active HIGH
	mdelay(1); // delay 1ms

	/* improve the efficiency of USB access MIU when system is busy */
	writeb(readb((void*)(UHC_base+0x81*2-1)) | (BIT0 | BIT1 | BIT2 | BIT3 | BIT7), (void*)(UHC_base+0x81*2-1));

	writeb((readb((void*)(UTMI_base+0x06*2)) & ~BIT5) | BIT6, (void*)(UTMI_base+0x06*2)); // reg_tx_force_hs_current_enable

	writeb((readb((void*)(UTMI_base+0x03*2-1)) & ~BIT4) | (BIT3 | BIT5), (void*)(UTMI_base+0x03*2-1)); // Disconnect window select

	writeb(readb((void*)(UTMI_base+0x07*2-1)) & ~BIT1, (void*)(UTMI_base+0x07*2-1)); // Disable improved CDR

#if defined(CONFIG_MSTAR_EDISON)
	/* bit<3> for 240's phase as 120's clock set 1, bit<4> for 240Mhz in mac 0 for faraday 1 for etron */
	writeb(readb((void*)(UTMI_base+0x08*2)) | BIT3, (void*)(UTMI_base+0x08*2));
#elif defined(CONFIG_MSTAR_EIFFEL)
	writeb(readb((void*)(UTMI_base+0x08*2)) & ~BIT3, (void*)(UTMI_base+0x08*2)); //Set sprcial value for Eiffel USB analog LIB issue
#endif

	writeb(readb((void*)(UTMI_base+0x09*2-1)) | (BIT0 | BIT7), (void*)(UTMI_base+0x09*2-1)); // UTMI RX anti-dead-loc, ISI effect improvement

	if ((flag & EHCFLAG_DOUBLE_DATARATE)==0)
	    writeb(readb((void*)(UTMI_base+0x0b*2-1)) | BIT7, (void*)(UTMI_base+0x0b*2-1)); // TX timing select latch path

	writeb(readb((void*)(UTMI_base+0x15*2-1)) | BIT5, (void*)(UTMI_base+0x15*2-1)); // Chirp signal source select

#if !defined(CONFIG_MSTAR_AMBER3) && \
	!defined(CONFIG_MSTAR_AMETHYST)
	writeb(readb((void*)(UTMI_base+0x15*2-1)) | BIT6, (void*)(UTMI_base+0x15*2-1)); // change to 55 interface
#endif

	/* Init UTMI eye diagram parameter setting */
	writeb(readb((void*)(UTMI_base+0x2c*2)) | UTMI_EYE_SETTING_2C, (void*)(UTMI_base+0x2c*2));
	writeb(readb((void*)(UTMI_base+0x2d*2-1)) | UTMI_EYE_SETTING_2D, (void*)(UTMI_base+0x2d*2-1));
	writeb(readb((void*)(UTMI_base+0x2e*2)) | UTMI_EYE_SETTING_2E, (void*)(UTMI_base+0x2e*2));
	writeb(readb((void*)(UTMI_base+0x2f*2-1)) | UTMI_EYE_SETTING_2F, (void*)(UTMI_base+0x2f*2-1));

#if defined(ENABLE_LS_CROSS_POINT_ECO)
	/* Enable deglitch SE0 (low-speed cross point) */
	writeb(readb((void*)(UTMI_base+0x04*2)) | BIT6, (void*)(UTMI_base+0x04*2));
#endif

#if defined(ENABLE_PWR_NOISE_ECO)
	/* Enable use eof2 to reset state machine (power noise) */
	writeb(readb((void*)(USBC_base+0x02*2)) | BIT6, (void*)(USBC_base+0x02*2));
#endif

#if defined(ENABLE_TX_RX_RESET_CLK_GATING_ECO)
	/* Enable hw auto deassert sw reset(tx/rx reset) */
	writeb(readb((void*)(UTMI_base+0x04*2)) | BIT5, (void*)(UTMI_base+0x04*2));
#endif

#if defined(ENABLE_LOSS_SHORT_PACKET_INTR_ECO)
	/* Enable patch for the assertion of interrupt(Lose short packet interrupt) */
	#if defined(CONFIG_MSTAR_AMBER3)
	writeb(readb((void*)(USBC_base+0x0b*2)) | BIT2, (void*)(USBC_base+0x0b*2));
	#elif defined(CONFIG_MSTAR_AGATE)
	writeb(readb((void*)(USBC_base+0x04*2)) | BIT7, (void*)(USBC_base+0x04*2));
	#else
	writeb(readb((void*)(USBC_base+0x04*2)) & ~BIT7, (void*)(USBC_base+0x04*2));
	#endif
#endif

#if defined(ENABLE_BABBLE_ECO)
	/* Enable add patch to Period_EOF1(babble problem) */
	writeb(readb((void*)(USBC_base+0x04*2)) | BIT6, (void*)(USBC_base+0x04*2));
#endif

#if defined(ENABLE_MDATA_ECO)
	/* Enable short packet MDATA in Split transaction clears ACT bit (LS dev under a HS hub) */
	#if defined(CONFIG_MSTAR_EDISON) || \
	    defined(CONFIG_MSTAR_EIFFEL) || \
	    defined(CONFIG_MSTAR_NIKE)
	writeb(readb((void*)USBC_base) | BIT4, (void*)USBC_base);
	#else
	writeb(readb((void*)(USBC_base+0x0f*2-1)) | BIT4, (void*)(USBC_base+0x0f*2-1));
	#endif
#endif

#if defined(ENABLE_HS_DM_KEEP_HIGH_ECO)
	/* Change override to hs_txser_en.  Dm always keep high issue */ 
	writeb(readb((void*)(UTMI_base+0x10*2)) | BIT6, (void*) (UTMI_base+0x10*2));
#endif

#if _USB_HS_CUR_DRIVE_DM_ALLWAYS_HIGH_PATCH
	/*
	 * patch for DM always keep high issue
	 * init overwrite register
	 */
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT6, (void*) (UTMI_base+0x0*2)); //R_DP_PDEN = 1
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT7, (void*) (UTMI_base+0x0*2)); //R_DM_PDEN = 1

	/* turn on overwrite mode */
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT1, (void*) (UTMI_base+0x0*2)); //tern_ov = 1
#endif

	if (flag & EHCFLAG_TESTPKG)
	{
		writew(0x0600, (void*) (UTMI_base+0x14*2));
		writew(0x0038, (void*) (UTMI_base+0x10*2));
		writew(0x0bfe, (void*) (UTMI_base+0x32*2));
	}
}

/* configure so an HC device and id are always provided */
/* always called with process context; sleeping is OK */

/**
 * usb_ehci_au1xxx_probe - initialize Au1xxx-based HCDs
 * Context: !in_interrupt()
 *
 * Allocates basic resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 *
 */

int usb_ehci_mstar_probe(const struct hc_driver *driver,
		struct usb_hcd **hcd_out, struct platform_device *dev)
{
	int retval=0;
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	unsigned int flag = 0;
    
	if( 0==strcmp(dev->name, "Mstar-ehci-1") )
	{
		printk("Mstar-ehci-1 H.W init\n");
#if defined(CONFIG_MSTAR_KAISER)     
		flag |= EHCFLAG_DPDM_SWAP;
#endif
		Titania3_series_start_ehc(_MSTAR_UTMI0_BASE, _MSTAR_USBC0_BASE, _MSTAR_UHC0_BASE, flag);
	}
	else if( 0==strcmp(dev->name, "Mstar-ehci-2") )
	{
		printk("Mstar-ehci-2 H.W init\n");
#if defined(CONFIG_MSTAR_KAISER)        
		flag |= EHCFLAG_DPDM_SWAP;
#endif
		Titania3_series_start_ehc(_MSTAR_UTMI1_BASE, _MSTAR_USBC1_BASE, _MSTAR_UHC1_BASE, flag);
	}
#ifdef ENABLE_THIRD_EHC
	else if( 0==strcmp(dev->name, "Mstar-ehci-3") )
	{
		printk("Mstar-ehci-3 H.W init\n");
		Titania3_series_start_ehc(_MSTAR_UTMI2_BASE, _MSTAR_USBC2_BASE, _MSTAR_UHC2_BASE, 0 );
	}
#endif
#ifdef ENABLE_FOURTH_EHC
	else if( 0==strcmp(dev->name, "Mstar-ehci-4") )
	{
		printk("Mstar-ehci-4 H.W init\n");
		Titania3_series_start_ehc(_MSTAR_UTMI3_BASE, _MSTAR_USBC3_BASE, _MSTAR_UHC3_BASE, 0 );
	}
#endif

	if (dev->resource[2].flags != IORESOURCE_IRQ) {
		pr_debug("resource[1] is not IORESOURCE_IRQ");
		retval = -ENOMEM;
	}
	hcd = usb_create_hcd(driver, &dev->dev, "mstar");
	if (!hcd)
		return -ENOMEM;
	hcd->rsrc_start = dev->resource[1].start;
	hcd->rsrc_len = dev->resource[1].end - dev->resource[1].start + 0;
#if _USB_XIU_TIMEOUT_PATCH
	hcd->usb_reset_lock = __SPIN_LOCK_UNLOCKED(hcd->usb_reset_lock);
#endif

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		pr_debug("request_mem_region failed");
		retval = -EBUSY;
		goto err1;
	}

	//hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len); // tony
	hcd->regs = (void *)(u32)(hcd->rsrc_start); // tony
	if (!hcd->regs) {
		pr_debug("ioremap failed");
		retval = -ENOMEM;
		goto err2;
	}

	ehci = hcd_to_ehci(hcd);
	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs + HC_LENGTH(ehci, ehci_readl(ehci, &ehci->caps->hc_capbase));

	//printk("\nDean: [%s] ehci->regs: 0x%x\n", __FILE__, (unsigned int)ehci->regs);
	/* cache this readonly data; minimize chip reads */
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	/* ehci_hcd_init(hcd_to_ehci(hcd)); */
	if( 0==strcmp(dev->name, "Mstar-ehci-1") )
	{
		hcd->port_index = 1;
		hcd->utmi_base = _MSTAR_UTMI0_BASE;
		hcd->ehc_base = _MSTAR_UHC0_BASE;
		hcd->usbc_base = _MSTAR_USBC0_BASE;
		hcd->bc_base = _MSTAR_BC0_BASE;	
	}
	else if( 0==strcmp(dev->name, "Mstar-ehci-2") )
	{
		hcd->port_index = 2;
		hcd->utmi_base = _MSTAR_UTMI1_BASE;
		hcd->ehc_base = _MSTAR_UHC1_BASE;
		hcd->usbc_base = _MSTAR_USBC1_BASE;
		hcd->bc_base = _MSTAR_BC1_BASE;	
	}
#ifdef ENABLE_THIRD_EHC
	else if( 0==strcmp(dev->name, "Mstar-ehci-3") )
	{
		hcd->port_index = 3;
		hcd->utmi_base = _MSTAR_UTMI2_BASE;
		hcd->ehc_base = _MSTAR_UHC2_BASE;
		hcd->usbc_base = _MSTAR_USBC2_BASE;
		hcd->bc_base = _MSTAR_BC2_BASE;	
	}
#endif
#ifdef ENABLE_FOURTH_EHC
	else if( 0==strcmp(dev->name, "Mstar-ehci-4") )
	{
		hcd->port_index = 4;
		hcd->utmi_base = _MSTAR_UTMI3_BASE;
		hcd->ehc_base = _MSTAR_UHC3_BASE;
		hcd->usbc_base = _MSTAR_USBC3_BASE;
	}
#endif

#ifdef ENABLE_BATTERY_CHARGE
	usb_bc_enable(hcd, true);
#else
	#if !( defined(CONFIG_MSTAR_AGATE)   || \
		   defined(CONFIG_MSTAR_EDISON)  || \
		   defined(CONFIG_MSTAR_EMERALD) || \
		   defined(CONFIG_MSTAR_AMETHYST)|| \
		   defined(CONFIG_MSTAR_EAGLE)      \
		   )  	     //list for ICs before Edison
	//Disable default setting.
	usb_bc_enable(hcd, false);
	#endif
#endif

	retval = usb_add_hcd(hcd, dev->resource[2].start, SA_INTERRUPT);//tony

	hcd->root_port_devnum=0;
	hcd->enum_port_flag=0;
	hcd->enum_dbreset_flag=0;
	hcd->rootflag=0;
	hcd->lock_usbreset=__SPIN_LOCK_UNLOCKED(hcd->lock_usbreset);

	//usb_add_hcd(hcd, dev->resource[2].start, IRQF_DISABLED | IRQF_SHARED);
	if (retval == 0)
		return retval;

	//iounmap(hcd->regs); // tony
err2:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err1:
	usb_put_hcd(hcd);
	return retval;
}

/* may be called without controller electrically present */
/* may be called with controller, bus, and devices active */

/**
 * usb_ehci_hcd_au1xxx_remove - shutdown processing for Au1xxx-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_ehci_hcd_au1xxx_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 *
 */
void usb_ehci_mstar_remove(struct usb_hcd *hcd, struct platform_device *dev)
{
	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

static int ehci_hcd_mstar_drv_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd = NULL;
	int ret;

	pr_debug("In ehci_hcd_mstar_drv_probe\n");

	if (usb_disabled())
		return -ENODEV;

	/* FIXME we only want one one probe() not two */
	ret = usb_ehci_mstar_probe(&ehci_mstar_hc_driver, &hcd, pdev);
	return ret;
}

static int ehci_hcd_mstar_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	/* FIXME we only want one one remove() not two */
	usb_ehci_mstar_remove(hcd, pdev);
	return 0;
}

static int ehci_hcd_mstar_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
	//struct usb_hcd *hcd = platform_get_drvdata(pdev);

	printk("ehci_hcd_mstar_drv_suspend...\n");
	//usb_ehci_mstar_remove(hcd, pdev);
	return 0;
}

static int ehci_hcd_mstar_drv_resume(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	unsigned int flag = 0;
    
#if defined(CONFIG_MSTAR_KAISER)
	flag |= (hcd->port_index == 1 || hcd->port_index == 2) ? EHCFLAG_DPDM_SWAP : 0;
#endif

	printk("ehci_hcd_mstar_drv_resume...\n");
	//usb_ehci_mstar_probe(&ehci_mstar_hc_driver, &hcd, pdev);

	Titania3_series_start_ehc(hcd->utmi_base, hcd->usbc_base, hcd->ehc_base, flag);
#if 0
	if( 0==strcmp(pdev->name, "Mstar-ehci-1") )
		Titania3_series_start_ehc(_MSTAR_UTMI0_BASE, _MSTAR_USBC0_BASE, _MSTAR_UHC0_BASE, 0);
	else if( 0==strcmp(pdev->name, "Mstar-ehci-2") )
		Titania3_series_start_ehc(_MSTAR_UTMI1_BASE, _MSTAR_USBC1_BASE, _MSTAR_UHC1_BASE, 0);
#ifdef ENABLE_THIRD_EHC
	else if( 0==strcmp(pdev->name, "Mstar-ehci-3") )
		Titania3_series_start_ehc(_MSTAR_UTMI2_BASE, _MSTAR_USBC2_BASE, _MSTAR_UHC2_BASE, 0 );
#endif
#ifdef ENABLE_FOURTH_EHC
	else if( 0==strcmp(pdev->name, "Mstar-ehci-4") )
		Titania3_series_start_ehc(_MSTAR_UTMI3_BASE, _MSTAR_USBC3_BASE, _MSTAR_UHC3_BASE, 0 );
#endif
#endif
	enable_irq(hcd->irq);
	return 0;
}

/*-------------------------------------------------------------------------*/

static struct platform_driver ehci_hcd_mstar_driver = {
	.probe 		= ehci_hcd_mstar_drv_probe,
	.remove 	= ehci_hcd_mstar_drv_remove,
	.suspend	= ehci_hcd_mstar_drv_suspend,
	.resume		= ehci_hcd_mstar_drv_resume,
	.driver = {
		.name	= "Mstar-ehci-1",
//		.bus	= &platform_bus_type,
	}
};
static struct platform_driver second_ehci_hcd_mstar_driver = {
	.probe 		= ehci_hcd_mstar_drv_probe,
	.remove 	= ehci_hcd_mstar_drv_remove,
	.suspend	= ehci_hcd_mstar_drv_suspend,
	.resume		= ehci_hcd_mstar_drv_resume,
	.driver = {
		.name 	= "Mstar-ehci-2",
//		.bus	= &platform_bus_type,
	}
};
#ifdef ENABLE_THIRD_EHC
static struct platform_driver third_ehci_hcd_mstar_driver = {
	.probe 		= ehci_hcd_mstar_drv_probe,
	.remove 	= ehci_hcd_mstar_drv_remove,
	.suspend	= ehci_hcd_mstar_drv_suspend,
	.resume		= ehci_hcd_mstar_drv_resume,
	.driver = {
		.name 	= "Mstar-ehci-3",
//		.bus	= &platform_bus_type,
	}
};
#endif
#ifdef ENABLE_FOURTH_EHC
static struct platform_driver fourth_ehci_hcd_mstar_driver = {
	.probe 		= ehci_hcd_mstar_drv_probe,
	.remove 	= ehci_hcd_mstar_drv_remove,
	.suspend	= ehci_hcd_mstar_drv_suspend,
	.resume		= ehci_hcd_mstar_drv_resume,
	.driver = {
		.name 	= "Mstar-ehci-4",
//		.bus	= &platform_bus_type,
	}
};
#endif

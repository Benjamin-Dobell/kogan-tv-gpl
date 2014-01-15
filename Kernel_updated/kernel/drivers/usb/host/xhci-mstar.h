/*
 * xHCI host controller driver
 *
 * Copyright (C) 2012 MStar Inc.
 *
 * Date: May 2012
 */

#ifndef _XHCI_MSTAR_H
#define _XHCI_MSTAR_H

#include "ehci-mstar.h"

// ----- Don't modify it !----------
#if defined(CONFIG_ARM) 
#define XHCI_PA_PATCH   1
#endif
#define XHCI_FLUSHPIPE_PATCH  1
//------------------------------

#define XHCI_CHIRP_PATCH  1
#define ENABLE_XHCI_SSC   1
#define XHCI_TX_SWING_PATCH  1

//------ for test -----------------
//#define XHCI_CURRENT_SHARE_PATCH 0   //Only for USB3; will cause USB2 chirp handshake fail. 
#define XHCI_ENABLE_DEQ  0
#define XHCI_ENABLE_TESTBUS  0
//--------------------------------

//Inter packet delay setting for all chips
#define XHCI_IPACKET_DELAY_PATCH

#if !defined(CONFIG_MSTAR_AGATE)  //exclude Agate. 
#define XHCI_ENABLE_PPC
#endif

#if !defined(CONFIG_MSTAR_AGATE)  //exclude Agate. 
#define XHCI_ENABLE_LOOPBACK_ECO
#endif

#if defined(CONFIG_MSTAR_AGATE)
#define ENABLE_KEEPALIVE_ECO
#endif

#if defined(CONFIG_MSTAR_EDISON)  //Only for Edison. 
#define XHCI_ENABLE_HOTRESET_ECO  //re-enable again to prevent from overwitting by sboot PPC function.
#endif

//--------------------------------

#if defined(CONFIG_ARM)
#define _MSTAR_PM_BASE         0xFD000000
#else
#define _MSTAR_PM_BASE         0xBF000000
#endif

#if defined(CONFIG_MSTAR_EDISON)  || \
    defined(CONFIG_MSTAR_AGATE)   || \
	defined(CONFIG_MSTAR_NIKE)
#define _MSTAR_U3PHY_DTOP_BASE (_MSTAR_USB_BASEADR+(0x22100*2))
#define _MSTAR_U3PHY_ATOP_BASE (_MSTAR_USB_BASEADR+(0x22200*2))
#define _MSTAR_U3UTMI_BASE     (_MSTAR_USB_BASEADR+(0x22300*2))
//#define _MSTAR_U3INDCTL_BASE   (_MSTAR_USB_BASEADR+(0x22400*2))
#define _MSTAR_U3TOP_BASE      (_MSTAR_USB_BASEADR+(0x22500*2))
#define _MSTAR_XHCI_BASE       (_MSTAR_USB_BASEADR+(0x90000*2))
#define _MSTAR_U3BC_BASE       (_MSTAR_USB_BASEADR+(0x23660*2))
#endif

#if defined(CONFIG_MSTAR_EIFFEL) 
#define _MSTAR_U3PHY_DTOP_BASE (_MSTAR_USB_BASEADR+(0x22100*2))
#define _MSTAR_U3PHY_ATOP_BASE (_MSTAR_USB_BASEADR+(0x22200*2))
#define _MSTAR_U3UTMI_BASE     (_MSTAR_USB_BASEADR+(0x22300*2))
#define _MSTAR_U3TOP_BASE      (_MSTAR_USB_BASEADR+(0x22500*2))
#define _MSTAR_XHCI_BASE       (_MSTAR_USB_BASEADR+(0x90000*2))
#define _MSTAR_U3BC_BASE       (_MSTAR_USB_BASEADR+(0x236C0*2))
#endif

#if defined(CONFIG_MSTAR_KAISER) 
#define _MSTAR_U3PHY_DTOP_BASE (_MSTAR_USB_BASEADR+(0x22C00*2))
#define _MSTAR_U3PHY_ATOP_BASE (_MSTAR_USB_BASEADR+(0x22D00*2))
#define _MSTAR_U3UTMI_BASE     (_MSTAR_USB_BASEADR+(0x22B00*2))
#define _MSTAR_U3TOP_BASE      (_MSTAR_USB_BASEADR+(0x22000*2))
#define _MSTAR_XHCI_BASE       (_MSTAR_USB_BASEADR+(0x90000*2))
#define _MSTAR_U3BC_BASE       (_MSTAR_USB_BASEADR+(0x22FC0*2))
#endif


//------ UTMI eye diagram parameters --------
#if defined(CONFIG_MSTAR_AGATE)
	// for 40nm
	#define XHC_UTMI_EYE_2C	(0x98)
	#define XHC_UTMI_EYE_2D	(0x02)
	#define XHC_UTMI_EYE_2E	(0x10)
	#define XHC_UTMI_EYE_2F	(0x01)
#elif defined(CONFIG_MSTAR_EDISON) || defined(CONFIG_MSTAR_KAISER)
	// for 40nm after Agate, use 55nm setting2
	#define XHC_UTMI_EYE_2C	(0x90)
	#define XHC_UTMI_EYE_2D	(0x02)
	#define XHC_UTMI_EYE_2E	(0x00)
	#define XHC_UTMI_EYE_2F	(0x81)
#else
	// for 40nm after Agate, use 55nm setting1, the default
	#define XHC_UTMI_EYE_2C	(0x10)
	#define XHC_UTMI_EYE_2D	(0x02)
	#define XHC_UTMI_EYE_2E	(0x00)
	#define XHC_UTMI_EYE_2F	(0x81)
#endif

#endif	/* _XHCI_MSTAR_H */


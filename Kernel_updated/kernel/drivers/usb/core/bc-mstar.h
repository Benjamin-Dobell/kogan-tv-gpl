#ifndef __BC_MSTAR_H
#define __BC_MSTAR_H


extern void usb_bc_enable(struct usb_hcd *hcd, bool enable); 			
extern int apple_charger_support(struct usb_hcd *hcd, struct usb_device *udev);

#endif


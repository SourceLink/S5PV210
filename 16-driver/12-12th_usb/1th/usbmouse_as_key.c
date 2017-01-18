#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>



static int usbmouse_as_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	printk("found usb_mouse\n");

	return 0;
}


static void usbmouse_as_key__disconnect(struct usb_interface *intf)
{
	printk("disconnect usb_mouse\n");
}

static struct usb_device_id usbmouse_as_key_id_table [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }	/* Terminating entry */
};


static struct usb_driver usbmouse_as_key_driver = {
	.name		= "usbmouse_as_key",
	.probe		= usbmouse_as_key_probe,
	.disconnect	= usbmouse_as_key__disconnect,
	.id_table	= usbmouse_as_key_id_table,
};
		

static int usbmouse_as_key_init(void)
{
	usb_register(&usbmouse_as_key_driver);

	return 0;
}


static void usbmouse_as_key_exit(void)
{
	usb_deregister(&usbmouse_as_key_driver);
}


module_init(usbmouse_as_key_init);
module_exit(usbmouse_as_key_exit);


MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("kernel_3.0.8");
MODULE_DESCRIPTION("TQ210 platform Driver");
MODULE_LICENSE("GPL");



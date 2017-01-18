#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>



static struct input_dev *uk_dev;
static struct urb *uk_urb;
static dma_addr_t usb_buf_phys;
static char *uk_buf;
static int len;
static void usbmouse_as_key_irq(struct urb *urb)
{
	int i;
	static int cnt = 0;
	
	printk("data cnt %d: ", ++cnt);
	for (i = 0; i < len; i++)
	{
		printk("%02x ", uk_buf[i]);
	}
	printk("\n");

	/* 重新提交urb */
	usb_submit_urb(uk_urb, GFP_KERNEL);
}


static int usbmouse_as_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_device *dev = interface_to_usbdev(intf);
	struct usb_host_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	int pipe;

	/* 输入子系统 */

	/* 分配一个input_dev */
	uk_dev = input_allocate_device();

	/* 参数哪类事件 */
	set_bit(EV_KEY, uk_dev->evbit);
	set_bit(EV_REP, uk_dev->evbit);

	/* 这类事件的哪些事 */
	set_bit(KEY_L, uk_dev->keybit);
	set_bit(KEY_S, uk_dev->keybit);
	set_bit(KEY_ENTER, uk_dev->keybit);
	
	/* 注册子系统 */
	input_register_device(uk_dev);

	/* 硬件操作设置usb */
	/* 数据三要素: 源、目的、长度 */
	interface = intf->cur_altsetting;
	endpoint = &interface->endpoint[0].desc;
	
	/* 源 */
	pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);

	/* 长度: */
	len = endpoint->wMaxPacketSize;

	/* 目的 */
	uk_buf = usb_alloc_coherent(dev, len, GFP_ATOMIC, &usb_buf_phys);

	/* 使用 */
	/* 分配usb request block */
	uk_urb = usb_alloc_urb(0, GFP_KERNEL);
	
	/* 使用"3要素设置urb" */
	usb_fill_int_urb(uk_urb, dev, pipe, uk_buf, len,usbmouse_as_key_irq, NULL, endpoint->bInterval);
	
	uk_urb->transfer_dma = usb_buf_phys;
	uk_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	/* 使用urb */
	usb_submit_urb(uk_urb, GFP_KERNEL);

	return 0;
}


static void usbmouse_as_key__disconnect(struct usb_interface *intf)
{
	struct usb_device *dev = interface_to_usbdev(intf);

	usb_kill_urb(uk_urb);
	
	usb_free_urb(uk_urb);
	
	usb_free_coherent(dev, len, uk_buf, usb_buf_phys);

	input_unregister_device(uk_dev);
	input_free_device(uk_dev);
	
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



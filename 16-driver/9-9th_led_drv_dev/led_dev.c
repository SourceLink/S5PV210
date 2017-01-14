#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>


static struct resource led_resource[] = {
	[0] = DEFINE_RES_MEM(0xE0200060, SZ_8),	//IORESOURCE_MEM 可以,IORESOURCE_IO需找不到资源
	[1] = DEFINE_RES_IRQ(3),
};

static void led_release(struct device * dev)
{

}

struct platform_device gpio_led_device = {
	.name		= "myled",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(led_resource),
	.resource	= led_resource,
	.dev	= {
		.release = led_release,
	},
};


static int  gpio_led_init(void)
{
	  platform_device_register(&gpio_led_device);
	  return 0;
}

static void gpio_led_exit(void)
{
	platform_device_unregister(&gpio_led_device);
}


module_init(gpio_led_init);
module_exit(gpio_led_exit);

MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("TQ210 platform Driver");
MODULE_LICENSE("GPL");




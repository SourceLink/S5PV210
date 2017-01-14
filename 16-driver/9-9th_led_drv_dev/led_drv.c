#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <asm/uaccess.h>
#include <asm/io.h>

static struct class	*ledsdrv_class;
static volatile unsigned long *gpc0con;
static volatile unsigned long *gpc0dat;
static unsigned int pin;

static int leds_drv_open(struct inode *inode, struct file *file)
{

	*gpc0con &= ~(0xf << (pin * 4));
	*gpc0con |= (0x1 << (pin * 4)); 

	return 0;
}


static int leds_drv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{

	char val;
	
	copy_from_user(&val, buf, 1);

	
	if (val == 1)
		*gpc0dat |= (1 << pin);
	else
		*gpc0dat &= ~(1 << pin);
		
	
	return 0;
}


struct file_operations leds_drv_fops = {
		.owner = THIS_MODULE,
		.open = leds_drv_open,
		.write = leds_drv_write,
};


int led_major;
static int gpio_led_probe(struct platform_device *pdev)
{
	struct resource *rs ;
	
	led_major = register_chrdev(0, "myled", &leds_drv_fops);
	ledsdrv_class = class_create(THIS_MODULE, "myled");
	device_create(ledsdrv_class, NULL, MKDEV(led_major, 0), NULL, "led");

	/* 根据platform_device的资源进行ioremap */
	rs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	gpc0con = (volatile unsigned long *)ioremap(rs->start, rs->end - rs->start + 1);
	gpc0dat = gpc0con + 1;

	rs = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	pin = rs->start;

	printk("led_probe, found led\n");
	
	return 0;
}


static int gpio_led_remove(struct platform_device *pdev)
{


	/* 卸载字符设备驱动程序 */

	unregister_chrdev(led_major, "led");
	class_destroy(ledsdrv_class);
	device_destroy(ledsdrv_class, MKDEV(led_major, 0));
	
	/* iounmap */
	iounmap(gpc0con);
	
	printk("led_remove, remove led\n");

	return 0;
}


static struct platform_driver gpio_led_driver = {
	.probe		= gpio_led_probe,
	.remove		= gpio_led_remove,
	.driver		= {
		.name	= "myled",
	},
};



static int  led_drv_init(void)
{
	 platform_driver_register(&gpio_led_driver);
	 return 0;
}

static void led_drv_exit(void)
{
	platform_driver_unregister(&gpio_led_driver);
}


module_init(led_drv_init);
module_exit(led_drv_exit);

MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("TQ210 platform Driver");
MODULE_LICENSE("GPL");









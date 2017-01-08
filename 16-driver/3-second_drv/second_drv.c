#include <linux/crypto.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/bitops.h>
#include <asm/unaligned.h>
#include <asm/io.h>


static struct class *seconddrv_class;
static struct device *seconddrv_class_devs;

static volatile unsigned long *gph0con = NULL;
static volatile unsigned long *gph0dat = NULL;



static int second_drv_open(struct inode * inode, struct file * file)
{
	/* 配置 CPH0_0~CPH0_3 为输入引脚 */
	
	*gph0con &= ~((0xf << (0 * 4 )) | (0xf << (1 * 4)) | (0xf << (2 * 4)) | (0xf << (3 * 4)));


	gpio_to_irq(EINT_GPIO_0(1))
	
	return 0;
}


static ssize_t second_drv_read(struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{
	/* 返回四个引脚的电平 */
	unsigned char key_vals[4];
	int i;

	if (bytes != sizeof(key_vals))
		return -EINVAL;
	
	
	/* 读取电平 */
	for (i = 0; i < 4; i++) 
		key_vals[i] = (*gph0dat & (1 << i)) ? 1 : 0;
	
	copy_to_user(userbuf, key_vals, sizeof(key_vals));

	return 4;
}



struct file_operations second_drv_fops = {
		.owner = THIS_MODULE,
		.open = second_drv_open,
		.read = second_drv_read,
};


int major;
static int second_drv_init(void)
{
	major = register_chrdev(0, "sencond_drv", &second_drv_fops);

	seconddrv_class = class_create(THIS_MODULE, "second_drv");

	seconddrv_class_devs = device_create(seconddrv_class, NULL, MKDEV(major,0), NULL, "buttons");

	gph0con = (volatile unsigned long *)ioremap(0xE0200C00, 16);
	gph0dat = gph0con + 1;

	return 0;
}

static void second_drv_exit(void)
{
	unregister_chrdev(major, "sencond_drv");

	device_unregister(seconddrv_class_devs);
	
	class_destroy(seconddrv_class);

	iounmap(gph0con);
}

module_init(second_drv_init);
module_exit(second_drv_exit);

MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("TQ210 Second Driver");
MODULE_LICENSE("GPL");





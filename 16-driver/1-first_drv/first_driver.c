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


static struct class *firstdrv_class;
static struct device *firstdrv_class_devs;


volatile unsigned long *gpc0con = NULL;
volatile unsigned long *gpc0dat = NULL;

static int first_drv_open(struct inode *inode, struct file *file)
{
	*gpc0con &= ~((0xf << (3 * 4)) | (0xf << (4 * 4)));
	*gpc0con |= ((0x1 << (3 * 4)) | (0x1 << (4 * 4)));
	
	return 0;
}


static ssize_t first_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	int val;

	copy_from_user(&val, buf, count);

	if (val == 1)
		*gpc0dat |= ((1 << 3) | (1 << 4));		//on
		else
		*gpc0dat &= ~((1 << 3) | (1 << 4));		//off
	
	return 0;
}


struct file_operations first_drv_fops = {
		.owner = THIS_MODULE,
		.open = first_drv_open,
		.write = first_drv_write,
};


int major;
int first_drv_init(void)
{
	major = register_chrdev(0, "first_drv", &first_drv_fops);	//注册结构 自动分配主设备号

	firstdrv_class = class_create(THIS_MODULE, "first_drv");

	firstdrv_class_devs = device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "LINK");


	gpc0con = (volatile unsigned long *)ioremap(0xE0200060, 24);	//重定义io
	gpc0dat = gpc0con + 1;
	
	return 0;
}

void first_drv_exit(void)
{
	unregister_chrdev(major, "filst_drv");					//卸载结构

	device_unregister(firstdrv_class_devs);
	
	class_destroy(firstdrv_class);
}

module_init(first_drv_init);
module_exit(first_drv_exit);


MODULE_LICENSE("GPL");




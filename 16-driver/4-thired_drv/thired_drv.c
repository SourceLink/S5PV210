#include <linux/crypto.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <asm/unaligned.h>
#include <asm/io.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>

struct pin_desc {
		unsigned int pin;
		unsigned char key_val;
};


static struct class *thireddrv_class;
static struct device *thireddrv_class_devs;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static volatile int ev_press = 0;

/*
	键值
*/
static unsigned char key_val;

struct pin_desc pins_desc[4] = {
		{EINT_GPIO_0(0), 0x01},
		{EINT_GPIO_0(1), 0x02},
		{EINT_GPIO_0(2), 0x04},
		{EINT_GPIO_0(3), 0x08},
};

/*
 * 确定按下的键值	
 */
static irqreturn_t buttons_interrupt (int irq, void *dev_instance)
{
	struct pin_desc *pindesc = (struct pin_desc *)dev_instance;

	if(!gpio_get_value(pindesc->pin))
			key_val = pindesc->key_val;			//按下
	else
			key_val = pindesc->key_val | 0x80;	//松开

	//printk("irqkey_val = 0x%x\n",key_val);
	
	ev_press = 1;
	wake_up_interruptible(&button_waitq);		/* 唤醒休眠的进程 */
	
	return IRQ_RETVAL(IRQ_HANDLED); 
}




static int thired_drv_open (struct inode * inode, struct file * file)
{
	/* 配置 CPH0_0~CPH0_3 为输入引脚 */
	
	request_irq(IRQ_EINT(0), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY1", &pins_desc[0]);
	request_irq(IRQ_EINT(1), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY2", &pins_desc[1]);
	request_irq(IRQ_EINT(2), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY3", &pins_desc[2]);
	request_irq(IRQ_EINT(3), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY4", &pins_desc[3]);
	
	return 0;
}


static ssize_t thired_drv_read (struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{

	if (bytes != 1)
		return -EINVAL;
	
	/* 如果没有按键动作，则休眠 */
	wait_event_interruptible(button_waitq, ev_press);     //ev_press = 0 睡眠 
	
	copy_to_user(userbuf, &key_val, 1);
	//printk("readkey_val = 0x%x\n",key_val);

	ev_press = 0;
		
	return 0;
}



int thired_drv_relaease (struct inode *inode, struct file *file)
{
	free_irq(IRQ_EINT(0), &pins_desc[0]);
	free_irq(IRQ_EINT(1), &pins_desc[1]);
	free_irq(IRQ_EINT(2), &pins_desc[2]);
	free_irq(IRQ_EINT(3), &pins_desc[3]);

	return 0;
}


struct file_operations thired_drv_fops = {
		.owner = THIS_MODULE,
		.open = thired_drv_open,
		.read = thired_drv_read,
		.release = thired_drv_relaease,
};


int major;
static int thired_drv_init(void)
{
	major = register_chrdev(0, "thired_drv", &thired_drv_fops);

	thireddrv_class = class_create(THIS_MODULE, "thired_drv");

	thireddrv_class_devs = device_create(thireddrv_class, NULL, MKDEV(major,0), NULL, "buttons");

	return 0;
}

static void thired_drv_exit(void)
{
	unregister_chrdev(major, "thired_drv");

	device_unregister(thireddrv_class_devs);
	
	class_destroy(thireddrv_class);

}

module_init(thired_drv_init);
module_exit(thired_drv_exit);

MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("TQ210 Second Driver");
MODULE_LICENSE("GPL");





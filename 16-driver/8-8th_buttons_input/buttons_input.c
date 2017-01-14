#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <mach/regs-gpio.h>


static struct input_dev *buttons_dev;
static struct timer_list buttons_timer;
static struct pin_desc  *irq_pd;

struct pin_desc {
		char *name;
		unsigned char key_val;
		unsigned int pin;
		int irq_eint;
};


static struct pin_desc pins_desc[4] = {
		{"KEY5", KEY_L, EINT_GPIO_0(0), IRQ_EINT(0)},
		{"KEY6", KEY_S, EINT_GPIO_0(1), IRQ_EINT(1)},
		{"KEY7", KEY_ENTER, EINT_GPIO_0(2), IRQ_EINT(2)},
		{"KEY8", 0x02, EINT_GPIO_0(3), IRQ_EINT(3)},
};


static void buttons_timer_fun(unsigned long data)
{
	struct pin_desc *pin_desc = irq_pd;

	if (!pin_desc)
	return;

	if(!gpio_get_value(pin_desc->pin))	{
		input_event(buttons_dev, EV_KEY, pin_desc->key_val, 1);				//按下
		input_sync(buttons_dev);
	} else {
		input_event(buttons_dev, EV_KEY, pin_desc->key_val, 0);				//松开
		input_sync(buttons_dev);
	}

}


static irqreturn_t buttons_interrupt (int irq, void *dev_instance)
{	
	irq_pd = (struct pin_desc *)dev_instance;

	/* HZ = 100，10ms中断一次，定时器中断1次就行 也就是10ms清一次 */
	mod_timer(&buttons_timer, jiffies + HZ / 100);

//	printk("HZ = %d, jiffies = %d \n", HZ, jiffies);
	return IRQ_RETVAL(IRQ_HANDLED); 
}


static int buttons_init(void)
{
	int i;

	/* 1. 分配一个input_dev结构体 */
	buttons_dev = input_allocate_device();

	/* 2. 设置 */
	/* 2.1 能产生哪类事件 */
	set_bit(EV_KEY, buttons_dev->evbit);
	set_bit(EV_REP, buttons_dev->evbit);
	
	/* 2.2 能产生这类操作里的哪些事件: */
	set_bit(KEY_L, buttons_dev->keybit);
	set_bit(KEY_S, buttons_dev->keybit);
	set_bit(KEY_ENTER, buttons_dev->keybit);
	set_bit(0x02, buttons_dev->keybit);

	/* 3. 注册 */
	input_register_device(buttons_dev);
	
	/* 4. 硬件相关的操作 */
	init_timer(&buttons_timer);
	buttons_timer.function= buttons_timer_fun;
	add_timer(&buttons_timer);
	

	for (i = 0; i < 4; i++) {
		request_irq(pins_desc[i].irq_eint, buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, pins_desc[i].name, &pins_desc[i]);
	}

	return 0;
}


static void buttons_exit(void)
{
	int i;

	for (i = 0;i < 4; i++) {
		free_irq(pins_desc[i].irq_eint, &pins_desc[i]);
	}

	del_timer(&buttons_timer);
	input_unregister_device(buttons_dev);
	input_free_device(buttons_dev);	
}



module_init(buttons_init);
module_exit(buttons_exit);

MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("TQ210 buttons Driver");
MODULE_LICENSE("GPL");




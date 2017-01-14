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
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <asm/unaligned.h>
#include <asm/io.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>

struct pin_desc {
		unsigned int pin;
		unsigned char key_val;
};


static struct class *sixthdrv_class;
static struct device *sixthdrv_class_devs;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static volatile int ev_press = 0;

struct fasync_struct *button_fasync;				/* 存放信号和进程id信息  */

//static atomic_t canopen = ATOMIC_INIT(1);    		/* 定义原子变量并初始化为1 */
static DEFINE_MUTEX(button_lock);     		/* 定义互斥锁 */
//static DEFINE_SEMAPHORE(button_lock);				/* 定义信号量 */

static struct timer_list buttons_timer;

static struct pin_desc *irq_pd;


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
	irq_pd = (struct pin_desc *)dev_instance;

	/* HZ = 100，10ms中断一次，定时器中断1次就行 也就是10ms清一次 */
	mod_timer(&buttons_timer, jiffies + HZ / 100);

//	printk("HZ = %d, jiffies = %d \n", HZ, jiffies);
	return IRQ_RETVAL(IRQ_HANDLED); 
}




static int sixth_drv_open (struct inode * inode, struct file * file)
{
//原子操作
#if 0
	if (!atomic_dec_and_test(&canopen)) {		/* 减1是否等于0 */
		atomic_inc(&canopen);
		return -EBUSY;
	}
#endif

//信号量
#if 0 
	mutex_lock(&button_lock);
#endif 

//阻塞
#if 1
	if (file->f_flags & O_NONBLOCK) {
		if(!mutex_trylock(&button_lock))	/* 如果沒有成功獲得信號量則返回 */
			return -EBUSY;
	} else { 
		mutex_lock(&button_lock);
	}
#endif	

	/* 配置 CPH0_0~CPH0_3 为输入引脚 */	
	request_irq(IRQ_EINT(0), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY1", &pins_desc[0]);
	request_irq(IRQ_EINT(1), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY2", &pins_desc[1]);
	request_irq(IRQ_EINT(2), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY3", &pins_desc[2]);
	request_irq(IRQ_EINT(3), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY4", &pins_desc[3]);
	
	return 0;
}


static ssize_t sixth_drv_read (struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{

	if (bytes != 1)
		return -EINVAL;

	if (file->f_flags & O_NONBLOCK) {
		if (!ev_press) 
			return -EAGAIN;			
	} else {
		/* 如果没有按键动作，则休眠 */
		wait_event_interruptible(button_waitq, ev_press);     //ev_press = 0 睡眠 ，既把进程加入自己定义的button_waitq等待队列中
	}

	
	copy_to_user(userbuf, &key_val, 1);
	ev_press = 0;
	
	
	//printk("readkey_val = 0x%x\n",key_val);

	
		
	return 0;
}



int sixth_drv_relaease (struct inode *inode, struct file *file)
{
	free_irq(IRQ_EINT(0), &pins_desc[0]);
	free_irq(IRQ_EINT(1), &pins_desc[1]);
	free_irq(IRQ_EINT(2), &pins_desc[2]);
	free_irq(IRQ_EINT(3), &pins_desc[3]);

//	atomic_inc(&canopen);
	if(!mutex_trylock(&button_lock))	/* 如果沒有成功獲得信號量則返回 */
		mutex_unlock(&button_lock);	

	return 0;
}

static unsigned int sixth_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;

	poll_wait(file, &button_waitq, wait);			/* 判断事件发生，把进程加入button_waitq 等待队列中 */


	if (ev_press)									/* 中断中唤醒进程，*/
		mask |= POLLIN |POLLRDNORM;

	return mask;
}

static int sixth_drv_fasync(int fd, struct file *filp, int on)
{
	printk("driver: sixth_drv_fasync\n");

	return fasync_helper(fd, filp, on, &button_fasync);
}


struct file_operations sixth_drv_fops = {
		.owner   = THIS_MODULE,
		.open    = sixth_drv_open,
		.read    = sixth_drv_read,
		.release = sixth_drv_relaease,
		.poll 	 = sixth_drv_poll,
		.fasync  = sixth_drv_fasync,
};


static void buttons_timer_fun(unsigned long data)
{
	struct pin_desc *pin_desc = irq_pd;

	if (!pin_desc)
	return;

	if(!gpio_get_value(pin_desc->pin))
			key_val = pin_desc->key_val;			//按下
	else
			key_val = pin_desc->key_val | 0x80;		//松开

	//printk("irqkey_val = 0x%x\n",key_val);
	
	ev_press = 1;
	wake_up_interruptible(&button_waitq);		/* 唤醒休眠的进程 */

	//kill_fasync(&button_fasync, SIGIO, POLL_IN);	/* 发送io信号给应用层 */

}

int major;
static int sixth_drv_init(void)
{

	init_timer(&buttons_timer);
	buttons_timer.function = buttons_timer_fun;
	//buttons_timer.expires	= 0;
	add_timer(&buttons_timer);
	
	major = register_chrdev(0, "sixth_drv", &sixth_drv_fops);

	sixthdrv_class = class_create(THIS_MODULE, "sixth_drv");

	sixthdrv_class_devs = device_create(sixthdrv_class, NULL, MKDEV(major,0), NULL, "buttons");

	return 0;
}

static void sixth_drv_exit(void)
{
	unregister_chrdev(major, "sixth_drv");

	device_unregister(sixthdrv_class_devs);
	
	class_destroy(sixthdrv_class);

}

module_init(sixth_drv_init);
module_exit(sixth_drv_exit);

MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("TQ210 sixth Driver");
MODULE_LICENSE("GPL");





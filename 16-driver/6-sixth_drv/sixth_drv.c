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


static struct class *fifthdrv_class;
static struct device *fifthdrv_class_devs;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static volatile int ev_press = 0;

struct fasync_struct *button_fasync;				/* ����źźͽ���id��Ϣ  */

//static atomic_t canopen = ATOMIC_INIT(1);    		/* ����ԭ�ӱ�������ʼ��Ϊ1 */
static DEFINE_MUTEX(button_lock);     		/* ���廥���� */
//static DEFINE_SEMAPHORE(button_lock);
/*
	��ֵ
*/
static unsigned char key_val;

struct pin_desc pins_desc[4] = {
		{EINT_GPIO_0(0), 0x01},
		{EINT_GPIO_0(1), 0x02},
		{EINT_GPIO_0(2), 0x04},
		{EINT_GPIO_0(3), 0x08},
};

/*
 * ȷ�����µļ�ֵ	
 */
static irqreturn_t buttons_interrupt (int irq, void *dev_instance)
{
	struct pin_desc *pindesc = (struct pin_desc *)dev_instance;

	if(!gpio_get_value(pindesc->pin))
			key_val = pindesc->key_val;			//����
	else
			key_val = pindesc->key_val | 0x80;	//�ɿ�

	//printk("irqkey_val = 0x%x\n",key_val);
	
	ev_press = 1;
	wake_up_interruptible(&button_waitq);		/* �������ߵĽ��� */

	//kill_fasync(&button_fasync, SIGIO, POLL_IN);	/* ����io�źŸ�Ӧ�ò� */
	
	return IRQ_RETVAL(IRQ_HANDLED); 
}




static int fifth_drv_open (struct inode * inode, struct file * file)
{
//ԭ�Ӳ���
#if 0
	if (!atomic_dec_and_test(&canopen)) {		/* ��1�Ƿ����0 */
		atomic_inc(&canopen);
		return -EBUSY;
	}
#endif

//�ź���
#if 0 
	mutex_lock(&button_lock);
#endif 

//����
#if 1
	if (file->f_flags & O_NONBLOCK) {
		if(!mutex_trylock(&button_lock))	/* ����]�гɹ��@����̖���t���� */
			return -EBUSY;
	} else { 
		mutex_lock(&button_lock);
	}
#endif	

	/* ���� CPH0_0~CPH0_3 Ϊ�������� */	
	request_irq(IRQ_EINT(0), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY1", &pins_desc[0]);
	request_irq(IRQ_EINT(1), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY2", &pins_desc[1]);
	request_irq(IRQ_EINT(2), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY3", &pins_desc[2]);
	request_irq(IRQ_EINT(3), buttons_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "KEY4", &pins_desc[3]);
	
	return 0;
}


static ssize_t fifth_drv_read (struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{

	if (bytes != 1)
		return -EINVAL;

	if (file->f_flags & O_NONBLOCK) {
		if (!ev_press) 
			return -EAGAIN;			
	} else {
		/* ���û�а��������������� */
		wait_event_interruptible(button_waitq, ev_press);     //ev_press = 0 ˯�� ���Ȱѽ��̼����Լ������button_waitq�ȴ�������
	}

	
	copy_to_user(userbuf, &key_val, 1);
	ev_press = 0;
	
	
	//printk("readkey_val = 0x%x\n",key_val);

	
		
	return 0;
}



int fifth_drv_relaease (struct inode *inode, struct file *file)
{
	free_irq(IRQ_EINT(0), &pins_desc[0]);
	free_irq(IRQ_EINT(1), &pins_desc[1]);
	free_irq(IRQ_EINT(2), &pins_desc[2]);
	free_irq(IRQ_EINT(3), &pins_desc[3]);

//	atomic_inc(&canopen);
	mutex_unlock(&button_lock);	

	return 0;
}

static unsigned int fifth_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;

	poll_wait(file, &button_waitq, wait);			/* �ж��¼��������ѽ��̼���button_waitq �ȴ������� */


	if (ev_press)									/* �ж��л��ѽ��̣�*/
		mask |= POLLIN |POLLRDNORM;

	return mask;
}

static int fifth_drv_fasync(int fd, struct file *filp, int on)
{
	printk("driver: fifth_drv_fasync\n");

	return fasync_helper(fd, filp, on, &button_fasync);
}


struct file_operations fifth_drv_fops = {
		.owner   = THIS_MODULE,
		.open    = fifth_drv_open,
		.read    = fifth_drv_read,
		.release = fifth_drv_relaease,
		.poll 	 = fifth_drv_poll,
		.fasync  = fifth_drv_fasync,
};


int major;
static int fifth_drv_init(void)
{
	major = register_chrdev(0, "fifth_drv", &fifth_drv_fops);

	fifthdrv_class = class_create(THIS_MODULE, "fifth_drv");

	fifthdrv_class_devs = device_create(fifthdrv_class, NULL, MKDEV(major,0), NULL, "buttons");

	return 0;
}

static void fifth_drv_exit(void)
{
	unregister_chrdev(major, "fifth_drv");

	device_unregister(fifthdrv_class_devs);
	
	class_destroy(fifthdrv_class);

}

module_init(fifth_drv_init);
module_exit(fifth_drv_exit);

MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("TQ210 Second Driver");
MODULE_LICENSE("GPL");




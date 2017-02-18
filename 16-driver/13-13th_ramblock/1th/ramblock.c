#include <linux/major.h>
#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/bitops.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include <asm/setup.h>
#include <asm/pgtable.h>

struct gendisk *ramblock_gendisk;
int major;

static DEFINE_SPINLOCK(ramblock_lock);
struct request_queue *ramblock_queue;

#define RAMBLOCKSIZE	(1024 * 1024)


static void do_ramblock_request(struct request_queue *q)
{
	static int cnt = 0;
	struct request *req;

	printk("do_ramblock_request cnt = %d\n",++cnt);

	req = blk_fetch_request(q);
	while (req) {
		if (!__blk_end_request_cur(req, 1))
			req = blk_fetch_request(q);
	}
}


static const struct block_device_operations ramblock_fops =
{
	.owner		= THIS_MODULE,
};

static int ramblock_init(void)
{
	/* 注册主设备号 */
	major = register_blkdev(0 , "ramblock");

	/* 分配一个genddisk结构体 */
	ramblock_gendisk = alloc_disk(3);

	/* 初始化队列 */
	ramblock_queue = blk_init_queue(do_ramblock_request, &ramblock_lock);
	
	ramblock_gendisk->major = major;
    ramblock_gendisk->first_minor = 0;
    ramblock_gendisk->fops = &ramblock_fops;
    sprintf(ramblock_gendisk->disk_name, "ramblock");
	set_capacity(ramblock_gendisk, RAMBLOCKSIZE); 					//p->heads * p->cylinders * p->sectors

    ramblock_gendisk->queue = ramblock_queue;

	/* 注册disk */
    add_disk(ramblock_gendisk);
	
	return 0;
}

static void ramblock_exit(void)
{
	unregister_blkdev(major, "ramblock");
	del_gendisk(ramblock_gendisk);
	put_disk(ramblock_gendisk);
	blk_cleanup_queue(ramblock_queue);
}


module_init(ramblock_init);
module_exit(ramblock_exit);

MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("kernel_3.0.8");
MODULE_DESCRIPTION("TQ210 ramblock Driver");
MODULE_LICENSE("GPL");




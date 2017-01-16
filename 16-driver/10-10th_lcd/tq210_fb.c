#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/fb.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/pm_runtime.h>
#include <linux/platform_data/video_s3c.h>
#include <video/samsung_fimd.h>

struct lcd_regs {
	unsigned long vidcon0;
	unsigned long vidcon1;
	unsigned long vidcon2;
	unsigned long vidcon3;
	unsigned long vidtcon0;
	unsigned long vidtcon1;
	unsigned long vidtcon2;
	unsigned long vidtcon3;
	unsigned long wincon0;
	unsigned long reserved1[4];
	unsigned long shad0wcon;
	unsigned long reserved2[2];
	unsigned long vidosd0a;
	unsigned long vidosd0b;
	unsigned long vidosd0c;
	unsigned long reserved4;	
	unsigned long reserved5[20];
	unsigned long vidw00add0b0;
	unsigned long vidw00add1b0;
};


static volatile unsigned long *GPD0CON;
static volatile unsigned long *GPD0DAT;

static volatile unsigned long *GPF0CON;
static volatile unsigned long *GPF1CON;
static volatile unsigned long *GPF2CON;
static volatile unsigned long *GPF3CON;
static volatile unsigned long *DISPLAY_CONTROL;


static struct lcd_regs *lcdregs;
static unsigned long pseudo_pal[16];

static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static int tq_lcdfb_setcolreg(unsigned int regno, unsigned int red,
			     unsigned int green, unsigned int blue,
			     unsigned int transp, struct fb_info *info)
{
	unsigned int val;
	
	if (regno > 16)
		return 1;

	/* 用red,green,blue三原色构造出val */
	val  = chan_to_field(red,	&info->var.red);
	val |= chan_to_field(green, &info->var.green);
	val |= chan_to_field(blue,	&info->var.blue);
	
	pseudo_pal[regno] = val;
	return 0;
}

static struct fb_ops tq_lcdfb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= tq_lcdfb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};



static struct fb_info *tq_lcd;

static int lcd_init(void)
{
	struct clk *tq_clk;
	/* 1.分配一个fb_info结构体 */
	tq_lcd = framebuffer_alloc(0, NULL);   /* mem_size =  sizeof(struct fb_info) + size */

	/* 2.填充fb_info结构体 */
	/* 2.1设置固定参数fix */
	strcpy(tq_lcd->fix.id, "tq_lcd");
	tq_lcd->fix.smem_len		= 800 * 480 *4;					/* 24bit >> 32bit >> 4byt */
	tq_lcd->fix.type 			= FB_TYPE_PACKED_PIXELS;		/* 填充像素 */
	tq_lcd->fix.visual			= FB_VISUAL_TRUECOLOR;
	tq_lcd->fix.line_length		= 800 * 4;						/* length of a line in bytes    */

	/* 2.2设置变化参数var */
	tq_lcd->var.xres			= 800;							/* visible resolution		*/
	tq_lcd->var.yres			= 480;
	tq_lcd->var.xres_virtual	= 800;							/* virtual resolution		*/
	tq_lcd->var.yres_virtual	= 480;
	tq_lcd->var.bits_per_pixel  = 32;							/* 颜色宽度 */

	/* RGB:888 */
	tq_lcd->var.red.offset		= 16;
	tq_lcd->var.red.length		= 8;
	
	tq_lcd->var.green.offset	= 8;
	tq_lcd->var.green.length	= 8;
	
	tq_lcd->var.blue.offset		= 0;
	tq_lcd->var.green.length	= 8;

	tq_lcd->var.activate		= FB_ACTIVATE_NOW;
	
	/* 2.3设置其他参数fbops、flags、pseudo_palette */
	tq_lcd->fbops				= &tq_lcdfb_ops;
	tq_lcd->pseudo_palette	    = pseudo_pal;						/* 假调色板 */
	tq_lcd->screen_size 		= 800 * 480 * 4;
	
	/* 3.硬件设置 */
	/* 3.1配置gpio用于lcd */

	/*
	*	pin map
	*	HSYNC --> GPF0_0
	*	VSYNC --> GPF0_1
	*	VDEN --> GPF0_2
	*	VCLK --> GPF0_3
	*   LXpwmTOUT0 --> GPD0_0
	*	VD0 ~ VD23 --> GPF0_4 ~ GPF0_7, GPF1_0 ~ GPF1_7, GPF2_0 ~ GPF2_7, GPF3_0 ~ GPF3_3
	*/

	GPD0CON = (volatile unsigned long *)ioremap(0xE02000A0, 8);		/* 背光控制引脚 */
	GPD0DAT = GPD0CON + 1;
	
	GPF0CON = (volatile unsigned long *)ioremap(0xE0200120, 4);
	GPF1CON = (volatile unsigned long *)ioremap(0xE0200140, 4);
	GPF2CON = (volatile unsigned long *)ioremap(0xE0200160, 4);
	GPF3CON = (volatile unsigned long *)ioremap(0xE0200180, 4);
	
	*GPD0CON &= ~(0xf << 0);
	*GPD0CON |= (1 << 0);
	*GPD0DAT &= ~(1 << 0);	/* 关闭背光 */

	*GPF0CON = 0x22222222;	/* 配置引脚为lcd功能 */
	*GPF1CON = 0x22222222;
	*GPF2CON = 0x22222222;
	*GPF3CON = 0x00002222;

	/* 使能时钟*/  
	tq_clk = clk_get(NULL, "lcd");	
	if (!tq_clk || IS_ERR(tq_clk)) {  
		printk(KERN_INFO "failed to get lcd clock source\n");  
	}  
	clk_enable(tq_clk); 

	/* 3.2根据lcd手册配置lcd控制器 */

	DISPLAY_CONTROL = (volatile unsigned long *)ioremap(0xE0107008, 4); 
	*DISPLAY_CONTROL = 0x2; 									/* RGB=FIMD I80=FIMD ITU=FIMD */

	/* 2.1 hsync,vsync,vclk,vden的极性和时间参数
	 * 2.2 行数、列数(分辨率),象素颜色的格式
	 * 2.3 分配显存(frame buffer),写入display controller    
	 */
	lcdregs = (struct lcd_regs *)ioremap(0xF8000000, sizeof(struct lcd_regs)); 

	//printk("lcdregs size :%x\n",sizeof(struct lcd_regs));
	
	/* bit[28:26]:使用RGB接口
	 * bit[18]:RGB 并行
	 * bit[13:6]:清除分频系数 VCLK = HCLK / (CLKVAL+1)
	 * bit[4]:选择时钟源 
	 * bit[2]:选择时钟源为HCLK_DSYS=166MHz
	 * bit[1]:使能lcd控制器
	 * bit[0]:当前帧结束后使能lcd控制器
	 */
	lcdregs->vidcon0 &=  ~((7 << 26)| (1 << 18) | (0xf << 6) | (1 << 2) | (1 << 1) | (1 << 0));
	lcdregs->vidcon0 |= ((5 << 6) | (1 << 4));

	/* bit[1]:使能lcd控制器 
	 * bit[0]:当前帧结束后使能lcd控制器 
	 */
 	lcdregs->vidcon0 |= ((1<<1) | (1<<0));


	/* bit[7]:下降沿读取数据 
	 * bit[6]:翻转 查看液晶数据手册
	 * bit[5]:翻转 
	 * bit[4]:标准模式即高电平
	 */
	lcdregs->vidcon1 &= ~((1 <<7) | (1 << 4)); 
	lcdregs->vidcon1 |= ((1 <<6) | (1 <<5));

	/* bit[23:16]:垂直方向的后沿:接收到垂直方向的同步信号后多少个时钟后跳到下一行
	 * bit[15:8]:垂直方向的前沿:接收到垂直方向的同步信号后多少个时钟后开始发送数据
	 * bit[7:0]:垂直方向的信号宽度 既高电平持续时间 
	 */
	lcdregs->vidtcon0 = ((23 << 16) | (22 << 8) | (2 << 0));


	/* bit[23:16]: Horizontal back porch
	 * bit[15:8]:  Horizontal front porch
	 * bit[7:0]:  Horizontal sync pulse width
	 */
	lcdregs->vidtcon1 = ((46 << 16) | (210 << 8) | (2 << 0));

	/* bit[21:11]: Determines the vertical size of display  LINEVAL = (Vertical display size) –1
	 * bit[10:0] : Determines the horizontal size of display  (Horizontal display size) -1
	 */
	lcdregs->vidtcon2 = (((480 - 1) << 11) | ((800 - 1 ) << 0));

	/* bit[31]:使能垂直方向信号输出 */
	lcdregs->vidtcon3 = (1 << 31);

	/* bit[12:11]:Should be 0
	 * bit[5:2] : Unpacked 24 bpp R:8-G:8-B:8
	 * bit[0]: enables the video output and video control signal
	 */
	lcdregs->wincon0 |= (1 << 0);
	lcdregs->wincon0 &= ~(0xf << 2);
	lcdregs->wincon0 |= (11 << 2);
	
	/* bit[21:11]:指定左上像素的水平屏幕坐标.
	 * bit[10:0]:指定左上像素的垂直屏幕坐标
	 */
	lcdregs->vidosd0a = ((0 << 11 ) | (0 << 0));

	/* bit[21:11]:指定右下像素的水平屏幕坐标.
	 * bit[10:0]:指定右下像素的垂直屏幕坐标
	 */
	lcdregs->vidosd0b = ((799 << 11) | (479 << 0));

	/* Specifies the Window Size  */
	lcdregs->vidosd0c = (480 * 800);
	
	/* 3.3分配显存 */
	tq_lcd->screen_base = dma_alloc_writecombine(NULL, tq_lcd->fix.smem_len, (dma_addr_t *)&tq_lcd->fix.smem_start, GFP_KERNEL);

	lcdregs->vidw00add0b0 = tq_lcd->fix.smem_start;
	lcdregs->vidw00add1b0 = tq_lcd->fix.smem_start + tq_lcd->fix.smem_len;

	/* 使能channel 0传输数据 */
	lcdregs->shad0wcon = 0x1;
	
	*GPD0DAT |= (1 << 0);	/* 开背光 */

	/* 4.注册fb_info结构体 */
	
	register_framebuffer(tq_lcd);

	return 0;	
}


static void lcd_exit(void)
{
	unregister_framebuffer(tq_lcd);
	dma_free_writecombine(NULL, tq_lcd->fix.smem_len, tq_lcd->screen_base, tq_lcd->fix.smem_start);
	iounmap(lcdregs);
	iounmap(DISPLAY_CONTROL);
	iounmap(GPF3CON);
	iounmap(GPF2CON);
	iounmap(GPF1CON);
	iounmap(GPF0CON);
	iounmap(GPD0CON);
	framebuffer_release(tq_lcd);
}

module_init(lcd_init);
module_exit(lcd_exit);


MODULE_AUTHOR("Sourcelink");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("TQ210 LCD Driver");
MODULE_LICENSE("GPL");




#define __REG(addr)	(*(volatile unsigned long *)addr)

#define GPD0CON	__REG(0xE02000A0) 
#define GPD0DAT	__REG(0xE02000A4) 

#define GPF0CON	__REG(0xE0200120)
#define GPF1CON	__REG(0xE0200140)
#define GPF2CON	__REG(0xE0200160)
#define GPF3CON	__REG(0xE0200180)

#define DISPLAY_CONTROL	__REG(0xE0107008)	/*  Display path selection  */

#define VIDCON0	__REG(0xF8000000)
#define VIDCON1 __REG(0xF8000004)
#define VIDTCON0 __REG(0xF8000010)
#define VIDTCON1 __REG(0xF8000014)
#define VIDTCON2 __REG(0xF8000018)
#define VIDTCON3 __REG(0xF800001C)


#define WINCON0 __REG(0xF8000020)
#define VIDOSD0A __REG(0xF8000040)
#define VIDOSD0B __REG(0xF8000044)
#define VIDOSD0C __REG(0xF8000048)
#define VIDW00ADD0B0	__REG(0xF80000A0)
#define VIDW00ADD1B0	__REG(0xF80000D0)
#define SHADOWCON	__REG(0xF8000034)

/* modied clock_div */
#define CLKVAL_F 	5		/* HCLK: 166M VCLK:26.4~46.8M VCLK = HCLK / (CLKVAL+1) */

#define VSPW		2		/* Vertical sync pulse width */
#define VFPD		22		/* Vertical front porch */
#define VBPD 		23		/* Vertical back porch */

#define HSPW		2		/* HS pulse width */
#define HFPD		210		/* HS Front Porch */
#define HBPD		46		/* HS Blanking */

#define OSD_LeftTopY_F	0		/* 左边顶部y位置 */
#define OSD_LeftTopX_F	0		/* 左边顶部x位置 */
#define OSD_RightBotY_F	479		/* 左边顶部y位置 */
#define OSD_RightBotX_F	799		/* 左边顶部x位置 */

#define	FRAME_BUFFER	0x30000000
#define ROW		480
#define COL		800
#define HOZVAL		(COL - 1)			/* 水平尺寸即列数 */
#define LINEVAL		(ROW - 1)			/* 垂直尺寸即行数 */

void lcd_init(void)
{
	/*
	*	pin map
	*	HSYNC --> GPF0_0
	*	VSYNC --> GPF0_1
	*	VDEN --> GPF0_2
	*	VCLK --> GPF0_3
	*   LXpwmTOUT0 --> GPD0_0
	*	VD0 ~ VD23 --> GPF0_4 ~ GPF0_7, GPF1_0 ~ GPF1_7, GPF2_0 ~ GPF2_7, GPF3_0 ~ GPF3_3
	*/
	
	GPF0CON = 0x22222222;	/* 配置引脚为lcd功能 */
	GPF1CON = 0x22222222;
	GPF2CON = 0x22222222;
	GPF3CON = 0x00002222;

	GPD0CON &= ~(0xf << 0);	/* 背光控制引脚 */
	GPD0CON |=  (1 << 0);	
	GPD0DAT |=  (1 << 0);	/* 开背光 */

	/* 2. 初始化display controller 
	 * 2.1 hsync,vsync,vclk,vden的极性和时间参数
	 * 2.2 行数、列数(分辨率),象素颜色的格式
	 * 2.3 分配显存(frame buffer),写入display controller
	 */
	
	DISPLAY_CONTROL = 0x02;	/* RGB=FIMD I80=FIMD ITU=FIMD  */

	/* bit[26~28]:使用RGB接口 */
	/* bit[18]:RGB 并行 */
	/* bit[6]:清除分频系数 */
	/* bit[2]:选择时钟源为HCLK_DSYS=166MHz */
	VIDCON0 &= ~((1 << 2) | (0xFF << 6) | (1 << 18) | (7 << 26));

	/* bit[4]:选择需要分频 */
	VIDCON0 |= ((1 << 4) | (CLKVAL_F << 6)); 
	
	/* bit[1]:使能lcd控制器 */
	/* bit[0]:当前帧结束后使能lcd控制器 */
 	VIDCON0 |= ((1<<0)|(1<<1));

	/* bit[7]:下降沿读取数据 */
	/* bit[4]:标准模式即高电平 */
	VIDCON1 &= ~((1 << 4) | (1 <<7)); 

	/* bit[6]:翻转 查看液晶数据手册 */
	/* bit[5]:翻转 */
	VIDCON1 |= ((1 <<5) | (1 <<6));

	/* 查看lcd数据手册 */
	VIDTCON0 = ((VSPW << 0) | (VFPD << 8) | (VBPD << 16));
	VIDTCON1 = ((HSPW << 0) | (HFPD << 8) | (HBPD << 16));
	VIDTCON2 = ((HOZVAL << 0) | (LINEVAL << 11));
	/* Enables VSYNC Signal Output */
	VIDTCON3 |= (1 << 31);
	/* R:8-G:8-B:8 */
	WINCON0 |= 1<<0;
	WINCON0 &= ~(0xF << 2);
	WINCON0 |= (11 << 2);		

	/* Specifies the Window Size */
	VIDOSD0A = ((OSD_LeftTopY_F << 0) | (OSD_LeftTopX_F) << 11);
	VIDOSD0B = ((OSD_RightBotY_F << 0) | (OSD_RightBotX_F << 11));
	VIDOSD0C = ((HOZVAL + 1) * (LINEVAL + 1));

	/* start address */
	VIDW00ADD0B0 = FRAME_BUFFER;
	/* modied */
	VIDW00ADD1B0 = (FRAME_BUFFER + ((HOZVAL + 1) * 4  + 0) * (LINEVAL + 1));	/* 字节为单位 end address VBASEL = VBASEU + (PAGEWIDTH+OFFSIZE) x (LINEVAL+1) */

	/* 使能channel 0传输数据 */
	SHADOWCON = 0x1;
}



void lcd_draw_pixel(unsigned int row, unsigned int col, unsigned int color)					/* 描点 */
{
	unsigned int * pixel = (unsigned int *)FRAME_BUFFER;

	*(pixel + row * COL + col) = color; 		
}


void lcd_clear_screen(unsigned int color)									/* 清屏 */
{
	int i, j;

	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			lcd_draw_pixel(i, j, color);
}

void lcd_draw_line(unsigned int colstart, unsigned int rowstart, unsigned int colend, unsigned int rowend, unsigned int color)
{
	float k;
	int b,i;		/* 斜率k , 截距b, 这个函数有缺陷 */
	unsigned int row;
	k = (float)(rowend - rowstart) / (colend - colstart);
	b = rowstart - k * colstart;
	
	for (i = colstart; i < colend; i++) {
		row = (unsigned int) (k * i + b);
		lcd_draw_pixel(row, i, color);
	}
}

void lcd_draw_hline(unsigned int col_start, unsigned int col_end, unsigned int row, unsigned int color)		/* 水平直线 */
{
	int i;
	
	for (i = col_start; i < col_end; i++) 
		lcd_draw_pixel(row, i, color);
}


void lcd_draw_vline(unsigned int row_start, unsigned int row_end, unsigned int col, unsigned int color)		/* 垂直直线 */
{
	int i;

	for (i = row_start; i < row_end; i++) 
		lcd_draw_pixel(i, col, color);
}




















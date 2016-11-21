
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;


/*
 *channel : sd卡的通道
 *start_block : 从哪开始拷贝
 *block_size : 拷贝的块数
 *mem_ptr : 存放拷贝数据的地址
 *with_init : 是否初始化SD卡
 * */
typedef u32 (*copy_sdmmc_to_mem)(u32 channel, u32 start_block, u8 block_size, u32 *mem_ptr, u32 with_init);


void copy_code_to_iram(void)
{
	u32 V210_SDMMC_BASE;
	u8 ch;
	
	V210_SDMMC_BASE = *(volatile unsigned int *)(0xD0037488);	/* 获取通道信息的地址 */

	copy_sdmmc_to_mem copy_bl2 = (copy_sdmmc_to_mem)(*(u32*)(0xD0037F98));
	if (V210_SDMMC_BASE == 0xEB000000)	/* 通道0 */
		ch = 0;
	else if (V210_SDMMC_BASE == 0xEB200000)	/* 通道1 */
		ch  = 1;
	/* 注意扇区大小，bl1代码16k sd卡一个扇区大小512Byte */
	copy_bl2(ch, 20, 10, (u32*)0x20000000, 0); 
}















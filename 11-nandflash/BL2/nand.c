#define	NFCONF  (*(volatile unsigned long *)0xB0E00000) 
#define	NFCONT  (*(volatile unsigned long *)0xB0E00004) 	
#define	NFCMMD  (*(volatile unsigned long *)0xB0E00008) 
#define	NFADDR  (*(volatile unsigned long *)0xB0E0000C)
#define	NFDATA  (*(volatile unsigned char *)0xB0E00010)
#define	NFSTAT  (*(volatile unsigned long *)0xB0E00028)

#define MP0_1CON  (*(volatile unsigned long *)0xE02002E0)
#define	MP0_3CON  (*(volatile unsigned long *)0xE0200320)
#define	MP0_6CON  (*(volatile unsigned long *)0xE0200380)

/*  Nand Flash Configuration Register  */
#define AddrCycle 	1	/* 5 address cycle */
#define PageSize	0	/* 2KBytes/Page */
#define MLCFlash	0	/* SLC NAND Flash */
#define TWRPH1 		1	/* (0+1) * 7.5 > 5ns (tCLH/tALH) */
#define TWRPH0 		2	/* (1+1) * 7.5ns > 12ns (tWP) */
#define TACLS 		1	/* 7.5ns * 2 > 12ns tALS tCLS */

/* Control Register */
#define MODE		1	/* ENABLE NAND Flash Controller */
#define Reg_nCE0	1	/* Disable chip select */
#define Reg_nCE1	1	/* Disable chip select */
#define RnB_TransMode 	0	/* Detect rising edge */
#define EnbRnBINT  	0	/* Disable RnB interrupt */
#define EnbIllegalAccINT	0 /* Disable interrupt */
#define LOCK		0 	/* Disable lock */
#define LockTight 	0	/* Disable lock-tight  */

/* nand commnd */
#define NAND_CMD_READ_1st             0x00			
#define NAND_CMD_READ_2st             0x30
#define NAND_CMD_RANDOM_WRITE         0x85
#define NAND_CMD_RANDOM_READ_1st      0x05
#define NAND_CMD_RANDOM_READ_2st      0xe0
#define NAND_CMD_READ_CB_1st          0x00
#define NAND_CMD_READ_CB_2st          0x35
#define NAND_CMD_READ_ID              0x90
#define NAND_CMD_RES                  0xff
#define NAND_CMD_WRITE_PAGE_1st       0x80
#define NAND_CMD_WRITE_PAGE_2st       0x10
#define NAND_CMD_BLOCK_ERASE_1st      0x60
#define NAND_CMD_BLOCK_ERASE_2st      0xd0
#define NAND_CMD_READ_STATUS          0x70

#define PAGE_SIZE											2048
#define BLOCK_SIZE										(PAGE_SIZE * 64)

typedef enum { ENABLE = 1, DISABLE = ~ENABLE}function_state;

static void nand_select_chip(function_state  state)	/* 片选控制 */
{
	if(state != DISABLE)
		NFCONT &= ~(1 << 1);	/* 使能片选 */
	else
		NFCONT |= (1 << 1);	/* 失能片选 */
}

static void nand_send_cmd(unsigned char cmd)		/* 发送命令 */
{
	NFCMMD = cmd;
}

static unsigned char nand_read_data(void)		/* 读取一字节数据 */
{
	return NFDATA;
}

static void nand_read_buf(unsigned char *buf, int size)	/* 读size byte数据 */
{
	int i = 0;
	for (; i < size; i++)
		buf[i] =  NFDATA;
}

static void nand_write_data(unsigned char value)	/* 写一字节数据 */
{
	NFDATA = value;
}

static void nand_write_buf(unsigned char *buf, int size)	/* 写size byte数据 */
{
	int i = 0;
	for (; i < size; i++)
		NFDATA = buf[i];
}

static void nand_send_addr(unsigned long addr)		/* 发送nandflash存储地址 */
{
	unsigned int col = addr % PAGE_SIZE;			/* 页内偏移地址 */
	unsigned int row = addr / PAGE_SIZE;			/* 页地址 */
	unsigned char i;	

	NFADDR = col & 0xFF;				/* A0 ~ A7 */

	for (i = 10; i > 0; i--);

	NFADDR = (col >> 8) & 0x0F;			/* A8 ~ A11 */

	for (i = 10; i > 0; i--);
	
	NFADDR = row &	0xFF;				/* A12 ~ A19 */
	 
	for (i = 10; i > 0; i--);

	NFADDR = (row >> 8) & 0xFF;			/* A20 ~ A27 */	
	
	for (i = 10; i > 0; i--);
	
	NFADDR = (row >> 16) & 0x07;			/* A28 ~ A30 */
	
}

static void nand_wait_ready(void)			/* 等待nandflash就绪 */
{
	while(!(NFSTAT & (1 << 0)));
}

static unsigned char nand_read_status(void)		/* 读取nandfalsh状态 */
{
	unsigned char nand_status;
	unsigned char i;

	nand_send_cmd(NAND_CMD_READ_STATUS);
	
	for (i = 10; i> 0; i--);	
	
	nand_status = nand_read_data();	
	
	return nand_status;				/* status 1: success 0:error */
}

static void nand_rest(void)				/* 复位nandflash */
{
	nand_select_chip(ENABLE);			/* 使能片选 */
	nand_send_cmd(NAND_CMD_RES);			/* 发送复位命令 */
	nand_wait_ready();				/* 等待nandflash就绪 */
	nand_select_chip(DISABLE);			/* 失能片选 */
}



void nand_init(void)					/* 初始化nandflash */
{
	NFCONF = ((AddrCycle << 1) | (PageSize << 2) | (MLCFlash << 3) | (TWRPH1 << 4) | (TWRPH0 << 8) |  (TACLS << 12));
	NFCONT = ((MODE) | (Reg_nCE0 << 1));
	
	/*
 	* port map
 	* CE1-> Xm0CSn2 -> MP01_2
 	* CLE-> Xm0FCLE -> MP03_0
 	* ALE-> Xm0FALE -> MP03_1
 	* WE -> Xm0FWEn -> MP03_2
 	* RE -> Xm0FREn -> MP03_3
 	* R/B1->Xm0FRnB0-> MP03_4
 	* IO[7:0]->Xm0DATA[7:0]->MP0_6[7:0]
 	* */
	
	/* 设置片选引脚 CSN[0] */
	MP0_1CON &= ~(0x00000F00);
	MP0_1CON |= (3 << 8);
	
	/* CLE,ALE,WE,RE,R/B1 */
	MP0_3CON &= ~(0x000FFFFF);
	MP0_3CON |= 0x00022222;
	
	/* DATA */
	MP0_6CON = 0x22222222;

	nand_rest();
}


void nand_read_id(unsigned char *buf)			/* 读chip ID */	
{
	unsigned char i;

	nand_select_chip(ENABLE);
	nand_send_cmd(NAND_CMD_READ_ID);
	nand_send_addr(0x00);

	for (i = 0; i < 5; i++)
		buf[i] = nand_read_data();
		
	nand_select_chip(DISABLE);
}

int nand_erase(unsigned long addr)			/* 擦出扇区 */
{	
	if (addr & (BLOCK_SIZE - 1))
	{
		return -1;
	}

	unsigned long row = addr / PAGE_SIZE;
	
	nand_select_chip(ENABLE);
	nand_send_cmd(NAND_CMD_BLOCK_ERASE_1st);
	
	NFADDR = row & 0xFF;				/* 以块擦除 */		
	NFADDR = (row >> 8) & 0xFF;
	NFADDR = (row >> 16) & 0x07;
	
	nand_send_cmd(NAND_CMD_BLOCK_ERASE_2st);
	nand_wait_ready();

	unsigned char status = nand_read_status();

	if (status != 1) {
		nand_select_chip(DISABLE);
		return 0;				/* 擦除成功 */
	}
	else {
		nand_select_chip(DISABLE);
		return -1;				/* 擦除失败 */
	}
}


int nand_read_page(unsigned char *buf, unsigned long addr)	/* 读一页数据 */
{
	if (addr & (PAGE_SIZE - 1))
	{
		return -1;
	}
	
	int i;
	nand_select_chip(ENABLE);
	nand_send_cmd(NAND_CMD_READ_1st);
	nand_send_addr(addr);
	nand_send_cmd(NAND_CMD_READ_2st);
	nand_wait_ready();
	
	nand_read_buf(buf, PAGE_SIZE);
	
	nand_select_chip(DISABLE);
	
	return 0;
}

void nand_read_random(unsigned char *buf, unsigned int addr, unsigned int size)	/* 随机读:从任意地址读任意字节的数据 */
{	
	nand_select_chip(ENABLE);
	nand_send_cmd(NAND_CMD_READ_1st);
	nand_send_addr(addr);
	nand_send_cmd(NAND_CMD_READ_2st);
	nand_wait_ready();
	
	int i;
	unsigned int  col = addr % PAGE_SIZE;			/* 页内偏移 */

	for (i = col; i < size + col; i++)
	{
		nand_send_cmd(NAND_CMD_RANDOM_READ_1st);
		NFADDR = i & 0xFF;
		NFADDR = (i >> 8) & 0xF;
		nand_send_cmd(NAND_CMD_RANDOM_READ_2st);
		*buf++ = nand_read_data();
	}
	
	nand_select_chip(DISABLE);
}

int nand_write_page(unsigned char *buf, unsigned long addr)	/* 写一页数据 */
{
	if (addr & (PAGE_SIZE - 1))
	{
		return -1;
	}
	
	int i;
	
	nand_select_chip(ENABLE);
	nand_send_cmd(NAND_CMD_WRITE_PAGE_1st);
	nand_send_addr(addr);
	nand_wait_ready();
	
	nand_write_buf(buf, PAGE_SIZE);
	
	nand_send_cmd(NAND_CMD_WRITE_PAGE_2st);
	nand_wait_ready();
	nand_select_chip(DISABLE);
	
	return 0;
}





void bzero(unsigned char *s, int size)
{
	int i = 0;
	for (; i < size; i++)
		s[i] = 0;
}

void main()
{	
	
	unsigned char buf[2048];
	int i;
	nand_init();
	bzero(buf, 2048);

	nand_read_id(buf);
	v_printf("\nID:");
	for (i = 0; i < 5; i++)
	{
		v_printf("%X ", buf[i]);
	}
	uart_senddata('\n');
	
	nand_erase(0x00);			/* 擦除以0x80000地址开始的一个块 */
	
	nand_write_page((unsigned char*)0xD0020000, 0x00);	/* 写入1页数据到0x80000地址 */
	
	nand_read_page(buf, 0x00);			/* 从0x80000地址读取一页数据（顺序读） */
	//nand_read_random(buf, 0x80000, 2048);	/* 从0x80000地址读取一页数据（随机读） */
	
	
	/* 打印读取到的数据，与写入的数据一致 */
	for (i = 0; i < 2048; i++)
	{
		if (i % 16 == 0)
			uart_senddata('\n');
		v_printf("%X ", buf[i]);
	}
}

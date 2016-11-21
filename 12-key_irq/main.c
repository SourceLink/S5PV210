


int main(void)
{
		system_initexception();
		v_printf("init exception vecotr\r\n");
		
		irq_init();
		v_printf("init irq\r\n");
		
		v_printf("wait key enter\r\n");
		while(1);
}

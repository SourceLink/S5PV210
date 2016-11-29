

int main(void)
{
		system_initexception();		
		irq_init();
		v_printf("irq init successful\r\n");
		lcd_init();
		
		v_printf("wait key enter\r\n");

		lcd_clear_screen(0xffffff);	
		lcd_draw_hline(266,533,240,0xff);
		lcd_draw_vline(100,380,400,0xff0000);
		while(1);
}

int raise (int signum)
{
	return 0;
}

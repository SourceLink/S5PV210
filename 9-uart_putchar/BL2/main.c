#include "stdio.h"

#define GPC0CON	*(volatile unsigned long*)0xE0200060
#define GPC0DAT *(volatile unsigned long*)0xE0200064 

void delay(int r0)
{
	volatile int count = r0;
	while(count--);
}

int
main(void)
{
	char recive_char = 0;
	/* 初始化io */
	GPC0CON |= (1 << 12);	/* set GPC0_3 Output */
	GPC0CON |= (1 << 16);   /* set GPC0_4 Output */
	
	
	GPC0DAT |= (1 << 3); 	/* set state high */
	GPC0DAT |= (1 << 4);   	/* set state high */
	
	puts("UART TEST IN S5PV210\r\n");
	puts("A:LED1 Toggle\r\n");
	puts("B:LED1 Toggle\r\n");
	puts("Please select A or B to toggle the LED\r\n");
	
	while(1) {	
		recive_char = uart_recived();
		uart_sentdata(recive_char);
		uart_sentdata('\r');
		uart_sentdata('\n');
		
		if(recive_char == 'A')
			GPC0DAT ^= (1 << 4);   	/* set state high */
		if(recive_char == 'B')
			GPC0DAT ^= (1 << 3); 	/* set state high */
		
	}
}

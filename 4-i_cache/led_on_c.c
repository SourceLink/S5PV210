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
	/* 初始化io */
	GPC0CON |= (1 << 12);	/* set GPC0_3 Output */
	GPC0CON |= (1 << 16);   /* set GPC0_4 Output */
	
	
	GPC0DAT |= (1 << 3); 	/* set state high */
	GPC0DAT |= (1 << 4);   	/* set state high */

	while(1) {
		delay(0x100000);
	
		GPC0DAT |= (1 << 4);   	/* set state high */
		GPC0DAT &= ~(1 << 3);    /* set state low */
		delay(0x100000);
	
		GPC0DAT |= (1 << 3); 	/* set state high */
		GPC0DAT &= ~(1 << 4);   	/* set state low */
	}
}

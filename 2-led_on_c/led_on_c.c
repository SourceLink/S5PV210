#include "stdio.h"

#define GPC0CON	*(volatile unsigned long*)0xE0200060
#define GPC0DAT *(volatile unsigned long*)0xE0200064 


int
main(void)
{
	GPC0CON = 0x01000;	/* 设置GPC0_3 Output */
	GPC0DAT = 0x08; 	/* Pin state high */
	
	while(1);
}

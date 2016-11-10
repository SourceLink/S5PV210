#include "stdio.h"

/* botton: GPH0_0, GPH0_1 ; led: GPC0_3,GPC0_4 */
#define GPH0CON	*(volatile unsigned long*)0xE0200C00
#define GPH0DAT *(volatile unsigned long*)0xE0200C04

#define GPC0CON *(volatile unsigned long*)0xE0200060
#define GPC0DAT *(volatile unsigned long*)0xE0200064

/*
 *led1,led2 To GPC0_3,GPC0_4 
 */
#define GPC0_3_msk	(3 << 12)
#define GPC0_4_msk	(3 << 16)

#define GPC0_3_out	(1 << 12)
#define GPC0_4_out	(1 << 16)


/*
 *key1,key2 To GPH0_0,GPH0_1
 */
#define GPH0_0_msk	(3 << 0)
#define GPH0_1_msk	(3 << 4)

#define GPH0_0_in	(0 << 0)
#define GPH0_1_in	(0 << 4)

int
main(void)
{
	unsigned char key_state = 0;
	/* init key ,led */
	
	GPH0CON &= ~(GPH0_0_msk | GPH0_1_msk);	/* first clear want set bit */
	GPH0CON |= (GPH0_0_in | GPH0_1_in); 	/* set in function */

	GPC0CON &= ~(GPC0_3_msk | GPC0_4_msk);
	GPC0CON |= (GPC0_3_out | GPC0_4_out);


	while (1) {
		key_state = GPH0DAT ;	
		
		if(key_state & 0x01)
			GPC0DAT &= ~(1 << 3);	/* led1 off */
		else
			GPC0DAT |= (1 << 3);	/* led1 on */

		if(key_state & 0x02)
			GPC0DAT &= ~(1 << 4);	/* led2 off */
		else
			GPC0DAT |= (1 << 4);	/* led2 on */
	}
	
	return 0;
}











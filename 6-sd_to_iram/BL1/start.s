/******************************************
*
*用来加载sd卡的bin到sram中执行
*
*******************************************/
.global _start

_start : 
	ldr	r0, =0xE2700000
	mov	r1, #0
	str	r1, [r0]		/* 关闭看门狗 */

	ldr	sp, =0xD0037D80	
	
	bl	copy_code_to_iram	/* 代码从sd卡拷贝到iram */
	
	ldr	pc, =0xD0022800


halt :	
 	b	halt

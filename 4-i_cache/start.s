.text
.global _start

_start:
	ldr	r0, =0xE2700000		/* 看门狗控制寄存器 */
	mov	r1, #0
	str	r1, [r0]		/* 关闭看门狗 */
	
	mrc	p15, 0, r0, c1, c0,0	/* cp c1 to r0 */
	orr	r0, r0, #0x00001000	/* bit12 set 1 */
	mcr	p15, 0, r0, c1, c0,0	/* open i-cache */

	ldr	sp, =0xD0037D80	/* 设置栈指针 */

	bl	main			/* pc = main,lr = bl后的指令 */

halt:
	b	halt			/* 死循环 */

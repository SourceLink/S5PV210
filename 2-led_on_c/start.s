
.global _start

_start:
	ldr r0,=0xE2700000	/* 看门狗控制寄存器 */
	mov r1,#0
	str r1,[r0]		/* 关闭看门狗 */


	ldr sp,=0xD0037D80	/* 设置栈指针 */

	bl main			/* pc = main,lr = bl后的指令 */

halt:
	b halt			/* 死循环 */

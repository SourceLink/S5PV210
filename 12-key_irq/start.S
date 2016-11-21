
.global _start
.global IRQ_handle

_start : 
	ldr	r0, =0xE2700000
	mov	r1, #0
	str	r1, [r0]		/* 关闭看门狗 */
	
	bl	mem_init		/* 初始化内存 */
	
	bl	clock_init		/* 初始化时钟 */

	ldr	sp, =0xD0037780		
	
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0x12		
	msr	cpsr, r0		/* irq模式 */
	
	ldr	sp, =0xD0037D80
	bl	irq_init		/* 初始化中断 */
	
	mrs	r0, cpsr
	bic	r0, r0, #0x9f
	orr	r0, r0, #0x10			
	msr	cpsr, r0		/* user模式 */

	ldr	sp, =0x25000000
	
	bl	uart_init	

	bl	main
halt :	
 	b	halt

IRQ_handle :
	
	sub	lr, lr, #4		/* 保存现场 */
	stmfd	sp!, {r0-r12, lr}
	bl	irq_handler		/* 跳转到中断处理函数 */
	
	ldmfd	sp!, {r0-r12,pc}^	/* 恢复现场，且spsr 赋给cpsr */


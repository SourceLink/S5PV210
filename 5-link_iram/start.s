.text
.global _start

_start:
	ldr	r0, =0xE2700000		/* 看门狗控制寄存器 */
	mov	r1, #0
	str	r1, [r0]		/* 关闭看门狗 */
	
	ldr	sp, =0xD0037D80		/* 设置栈指针 */
	
	mrc	p15, 0, r0, c1, c0,0	/* cp c1 to r0 */
	orr	r0, r0, #0x00001000	/* bit12 set 1 */
	mcr	p15, 0, r0, c1, c0,0	/* open i-cache */

	adr	r0, _start		/* 取出当前运行(实际)地址(0xD0020010) */	
	ldr	r1, =_start		/* 取出当前链接(指定)地址(0xD0024000) */
	ldr	r2, =bss_start
	
	cmp	r0,r1
	beq	clean_bss		/* 设置的链接地址和实际运行地址相同就不要代码复制了 */
	
copy_loop:
	ldr	r3, [r0], #4		/* 把r0指向的地址数据赋给r3 */
	str	r3, [r1], #4		/* 把r3中的数据赋给r1所指向的地址*/ 
	cmp	r1, r2			/* 开始的链接地址到bss_start之间的宽带即代码段和数据段的宽度 需要全部转移到新的地址运行 */
	bne	copy_loop		/* 不相等则跳转 */

clean_bss:				/* 清除bss也就相当于把为定义的全剧变量初始化为0	*/
	ldr	r0, =bss_start
	ldr	r1, =bss_end
	
	cmp	r0, r1
	beq	run_on_iram		/* 相等则跳转，说明没有未初始化全剧变量 */

	mov	r2, #0
clean_loop:
	str	r2, [r0], #4
	
	cmp	r0, r1			/* 比较bss_start和bss_end的地址 bss_start的地址在递增 */
	bne	clean_loop		/* 不相等则跳转 */

run_on_iram:
	ldr	pc, =main			/* pc = main,lr = bl后的指令 */

halt:
	b	halt			/* 死循环 */

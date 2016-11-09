.text
.global _start
_start:
	ldr r0,=0xE0200060	/* GPC0 Control Register Address 0-19b */
	mov r1,#0x00010000      /* GPC04 Set Output */
	str r1,[r0]             /* 把r1的值写入r0指向的地址 */

	ldr r0,=0xE0200064      /* GPC0DATRegister Address */
	mov r1,#0x00000010	
	str r1,[r0]		/* 点亮GPC0高电平点亮 */

main_loop:
	b   main_loop

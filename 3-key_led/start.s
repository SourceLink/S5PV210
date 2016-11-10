.text
.global _start
_start:

	ldr	r0 , =0xE2700000	/* WATCHDOG CONFIG */
	mov	r1 , #0x0
	str	r1 , [r0]		/* COLSE WATCHDOG */


	ldr	sp , =0xD0037D80	/* SET STACK */

	bl	main

halt:
	b	halt			/* while(1) */

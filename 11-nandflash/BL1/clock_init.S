/* 时钟控制器基地址 */
#define ELFIN_CLOCK_POWER_BASE		0xE0100000	

/* 时钟相关的寄存器相对时钟控制器基地址的偏移值 */
#define APLL_LOCK_OFFSET		0x00		
#define MPLL_LOCK_OFFSET		0x08
#define EPLL_LOCK_OFFSET		0x10		
#define VPLL_LOCK_OFFSET		0x20

#define APLL_CON0_OFFSET		0x100
#define APLL_CON1_OFFSET		0x104
#define MPLL_CON_OFFSET			0x108
#define EPLL_CON0_OFFSET		0x110
#define EPLL_CON1_OFFSET		0x114
#define EPLL_CON_OFFSET		  	0x120

#define CLK_SRC0_OFFSET			0x200
#define CLK_SRC1_OFFSET			0x204
#define CLK_SRC2_OFFSET			0x208
#define CLK_SRC3_OFFSET			0x20c
#define CLK_SRC4_OFFSET			0x210
#define CLK_SRC5_OFFSET			0x214
#define CLK_SRC6_OFFSET			0x218
#define CLK_SRC_MASK0_OFFSET		0x280
#define CLK_SRC_MASK1_OFFSET		0x284

#define CLK_DIV0_OFFSET			0x300
#define CLK_DIV1_OFFSET			0x304
#define CLK_DIV2_OFFSET			0x308
#define CLK_DIV3_OFFSET			0x30c
#define CLK_DIV4_OFFSET			0x310
#define CLK_DIV5_OFFSET			0x314
#define CLK_DIV6_OFFSET			0x318
#define CLK_DIV7_OFFSET			0x31c


#define APLL_RATIO			0			/* 1G */
#define	A2M_RATIO			4			/* 分完频后200M 查看数据手册374页 */
#define HCLK_MSYS_RATIO			4			/* 200M */
#define PCLK_MSYS_RATIO			1			/* 100M HCLK_MSYS/(PCLK_MSYS_RATIO + 1) */
#define HCLK_DSYS_RATIO			3			/* 166.75M MOUNT_DSYS:667M MOUT_DSYS/(HCLK_MSYS_RATIO + 1) */
#define PCLK_DSYS_RATIO			1			/* 83.375M HCLK_DSYS/(PCLK_MSYS_RATIO + 1) */
#define HCLK_PSYS_RATIO			4			/* 133.4M MOUNT_PSYS:667M MOUT_PSYS/(HCLK_MSYS_RATIO + 1) */
#define PCLK_PSYS_RATIO			1			/* 66.7M HCLK_PSYS/(PCLK_MSYS_RATIO + 1) */

#define	APLL_SDIV			1
#define APLL_PDIV			3
#define	APLL_MDIV			125
#define APLL_ENABLE			1

#define MPLL_SDIV			1
#define MPLL_PDIV			12
#define MPLL_MDIV			667
#define MPLL_ENABLE			1


#define APLL_SEL			1
#define	MPLL_SEL			1
#define EPLL_SEL			0
#define VPLL_SEL			0
#define MUX_MSYS_SEL			0
#define MUX_DSYS_SEL			0
#define	MUX_PSYS_SEL			0
#define ONENAND_SEL			1

.global clock_init

clock_init : 

	ldr	r0, =ELFIN_CLOCK_POWER_BASE
	
	/* step1 */
	ldr	r1, =0x0000FFFF					/* 设置锁定时间 */
	str	r1, [r0, #APLL_LOCK_OFFSET]
	str	r1, [r0, #MPLL_LOCK_OFFSET]

	/* step2 */
	ldr	r1, =((APLL_RATIO) | (A2M_RATIO << 4) | (HCLK_MSYS_RATIO << 8) | (PCLK_MSYS_RATIO << 12) | (HCLK_DSYS_RATIO << 16) |	\
			(PCLK_DSYS_RATIO << 20) | (HCLK_PSYS_RATIO << 24) | (PCLK_PSYS_RATIO << 28))	
	str	r1, [r0, #CLK_DIV0_OFFSET]			/* 设置分频 */

	/* step3 */
	ldr	r1, =((APLL_SDIV) | (APLL_PDIV << 8) | (APLL_MDIV << 16) | (APLL_ENABLE << 31))	
	str	r1, [r0, #APLL_CON0_OFFSET]

	ldr	r1, =((MPLL_SDIV) | (MPLL_PDIV << 8) | (MPLL_MDIV << 16) | (MPLL_ENABLE << 31))
	str	r1, [r0, #MPLL_CON_OFFSET]

	/* step4 */
	ldr	r1, =((APLL_SEL) | (MPLL_SEL << 4) | (EPLL_SEL << 8) | (VPLL_SEL << 12) | (MUX_MSYS_SEL << 16) | (MUX_DSYS_SEL << 20) |	  \
			(MUX_PSYS_SEL << 24) | (ONENAND_SEL << 28))
	str	r1, [r0, #CLK_SRC0_OFFSET]

	mov	pc, lr













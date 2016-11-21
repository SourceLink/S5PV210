
#define	__REG(x)	(*(volatile unsigned long*)x)
#define GPH0CON		__REG(0xE0200C00)
#define GPH0DAT		__REG(0xE0200C04)


#define EXT_INT_0_CON	__REG(0xE0200E00)
#define EXT_INT_0_MASK  __REG(0xE0200F00)
#define EXT_INT_0_PEND	__REG(0xE0200F40)

#define VIC0INTSELECT	__REG(0xF200000C)
#define VIC0INTENABLE	__REG(0xF2000010)
#define VIC0ADDRESS	__REG(0xF2000F00)
#define VIC0VECTADDR0	__REG(0xF2000100)
#define VIC0VECTADDR1	__REG(0xF2000104)

#define _exception_vector	0xD0037400
#define pExceptionRESET	(*(volatile unsigned long*)(_exception_vector + 0x00))
#define pExceptionUNDEF	(*(volatile unsigned long*)(_exception_vector + 0x04))
#define pExceptionSWI	(*(volatile unsigned long*)(_exception_vector + 0x08))
#define pExceptionPABORT (*(volatile unsigned long*)(_exception_vector + 0x0c))
#define pExceptionDABORT (*(volatile unsigned long*)(_exception_vector + 0x10))
#define pExceptionRESERVED (*(volatile unsigned long*)(_exception_vector + 0x14))
#define pExceptionIRQ	(*(volatile unsigned long*)(_exception_vector + 0x18))
#define pExceptionFIRQ	(*(volatile unsigned long*)(_exception_vector + 0x1c))


void exceptionundef(void)
{
    v_printf("undefined instruction exception.\n");
    while(1);
}

void exceptionswi(void)
{
    v_printf("swi exception.\n");
    while(1);
}

void exceptionpabort(void)
{
    v_printf("pabort exception.\n");
    while(1);
}

void exceptiondabort(void)
{

    v_printf("dabort exception.\n");
    while(1);
}


extern void IRQ_handle(void);
void system_initexception(void)
{
   
    pExceptionUNDEF  =	(unsigned long)exceptionundef; /* 设置中断向量表 */
    pExceptionSWI    =	(unsigned long)exceptionswi;
    pExceptionPABORT =	(unsigned long)exceptionpabort;
    pExceptionDABORT =	(unsigned long)exceptiondabort;
    pExceptionIRQ    =	(unsigned long)IRQ_handle;
    pExceptionFIRQ    =	(unsigned long)IRQ_handle;

    VIC0ADDRESS = 0;

}

void key0_irq(void)
{
	VIC0ADDRESS &= ~(1 << 0);
	
	EXT_INT_0_PEND |= (1 << 0);

	v_printf("we get company : EINT0 \r\n");
}

void key1_irq(void)
{
	VIC0ADDRESS &= ~(1 << 1);	/* clear irq function */
	
	EXT_INT_0_PEND |= (1 << 1);	/* clear irq flag */
	
	v_printf("we get company : EINT1 \r\n");
	
}

void irq_init(void)
{
	GPH0CON	|= (0xF << 0) | (0xF << 4);		/* 外部中断 */ 	
	
	EXT_INT_0_CON &= ~((7) | (7 << 4));
	EXT_INT_0_CON |= ((2) | (2 << 4));		/* 下降沿触发 */

	EXT_INT_0_MASK &= ~((1) | (1 << 1));		/* 使能外部中断 */
	
	VIC0INTSELECT &= ~((1) | (1 << 1));		/* IRQ */
	
	VIC0INTENABLE |= ((1) | (1 << 1));		/* 使能中断 */
	
	VIC0VECTADDR0 = (unsigned ) key0_irq;	/* 设置中断向量 */
	
	VIC0VECTADDR1 = (unsigned ) key1_irq; 
}	

void irq_handler(void)					/* 中断处理函数 */
{
	void (*isr)(void) = 0;
	isr = ((void (*)(void))VIC0ADDRESS);
	
	(*isr)();
}



#define GPA0CON 		(*((volatile unsigned long *)0xE0200000))		


// UART相关寄存器
#define ULCON0 			(*((volatile unsigned long *)0xE2900000))		
#define UCON0 			(*((volatile unsigned long *)0xE2900004))
#define UFCON0 			(*((volatile unsigned long *)0xE2900008))
#define UMCON0 			(*((volatile unsigned long *)0xE290000C))
#define UTRSTAT0 		(*((volatile unsigned long *)0xE2900010))
#define UERSTAT0 		(*((volatile unsigned long *)0xE2900014))
#define UFSTAT0 		(*((volatile unsigned long *)0xE2900018))
#define UMSTAT0 		(*((volatile unsigned long *)0xE290001C))
#define UTXH0 			(*((volatile unsigned long *)0xE2900020))
#define URXH0 			(*((volatile unsigned long *)0xE2900024))
#define UBRDIV0 		(*((volatile unsigned long *)0xE2900028))
#define UDIVSLOT0 		(*((volatile unsigned long *)0xE290002C))
#define UINTP 			(*((volatile unsigned long *)0xE2900030))
#define UINTSP 			(*((volatile unsigned long *)0xE2900034))
#define UINTM 			(*((volatile unsigned long *)0xE2900038))

#define Word_Length 		3		/* 8-bit */
#define Stop_Bit 		0		/* One stop bit per frame */
#define Parity_Mode 		0		/* No parity  */
#define Infrared_Mode		0		/* Normal mode operation */

#define Receive_Mode		1		/* Interrupt request or polling mode */
#define Transmit_Mode		1		/* Interrupt request or polling mode */
#define Clock_Selection		0		/*  Selects PCLK or SCLK_UART */

void uart_init(void)
{
	/* TXD0 : GPA0_1 RXD0 : GPA0_0 */	
	GPA0CON &= ~(0x0F << 0);
	GPA0CON |= (2 << 0);	/* UART_RXD */
	
	GPA0CON &= ~(0x0F << 4);
	GPA0CON |= (2 << 4);	/* UART_TXD */
	
	ULCON0 	= ((Word_Length) | (Stop_Bit << 2) | (Parity_Mode << 3) | (Infrared_Mode << 6));
	
	UCON0 	= ((Receive_Mode) | (Transmit_Mode << 2) | (Clock_Selection << 10));
	
	
	/*
	** 波特率计算：115200bps
	** PCLK = 66MHz
	** DIV_VAL = (66000000/(115200 x 16))-1 = 35.8 - 1 = 34.8
	** UBRDIV0 = 34(DIV_VAL的整数部分)
	** (num of 1's in UDIVSLOTn)/16 = 0.8
	** (num of 1's in UDIVSLOTn) = 12
	** UDIVSLOT0 = 0xDDDD (查表)
	*/
	UBRDIV0 = 34; 
	
	UDIVSLOT0 = 0xDDDD;	
}


void uart_sentdata(unsigned char byte)
{
	while(!(UTRSTAT0 & (1 << 2)));		/* 等待发送机空 */
	
	UTXH0 = byte;
}

unsigned uart_recived(void)
{
	while(!(UTRSTAT0 & 0x1));		/* 等待接受机接受到数据 */

	return URXH0;
}


void puts(char *str)
{
	char *p = str;
	
	while(*p)
		uart_sentdata(*p++);
	uart_sentdata('\n');
}



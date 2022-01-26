#include<stdio.h>
#include<stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "boot_loader/bl_config.h"
#include "boot_loader/bl_uart.h"
//串口发送一个字符
int uart_putchar(char c,FILE *stream)
{

	if(c=='\a')
	{
		fputs("*ring*\n",stderr);
		return 0;
	}
	if(c=='\n')
		uart_putchar('\r',stream);
	//等待缓冲区有空闲
	while((HWREG(UART0_BASE+UART_O_FR)&UART_FR_TXFF))
	{}
	HWREG(UART0_BASE+UART_O_DR)=c;

	return 0;
}

//串口接收一个字符
int uart_getchar(FILE *stream)
{
	uint8_t c;
		//等待缓冲区有空闲
	while((HWREG(UART0_BASE+UART_O_FR)&UART_FR_TXFF))
	{}		//等待接收FIFO非空.
	c=HWREG(UART0_BASE+UART_O_DR);
	return c;
}

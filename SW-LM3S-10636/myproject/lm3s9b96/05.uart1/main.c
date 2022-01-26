/*********************************************
U1TX:PC7
U1RX:PC6
***********************************************/
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "uart.h"
#include <stdint.h>
#include <stdio.h>


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

void UART1IntHandler(void)
{
	unsigned long ulStatus;
	ulStatus = UARTIntStatus(UART1_BASE, true);
	UARTIntClear(UART1_BASE, ulStatus);
	UARTprintf("Hello,world\n");
	while(UARTCharsAvail(UART1_BASE))
	{
		UARTCharPutNonBlocking(UART1_BASE, UARTCharGetNonBlocking(UART1_BASE));
	}
}


void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
	while(ulCount--)
	{
		UARTCharPutNonBlocking(UART1_BASE, *pucBuffer++);
	}
}

int main(void)
{
	//100MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//使能串口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	//使能中断
	IntMasterEnable();
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6| GPIO_PIN_7);
	GPIOPinConfigure(GPIO_PC6_U1RX|GPIO_PC7_U1TX);
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
		(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		UART_CONFIG_PAR_NONE));

	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);


	UARTSend((unsigned char *)"Enter text: ", 12);
	UARTStdioInit(1);
	UARTprintf("Hello,world\n");
	unsigned long ulClockMS,ulCount;
	unsigned short usController;
	{
		ulClockMS=SysCtlClockGet();
		UARTprintf("%d\n",ulClockMS);
		SysCtlDelay(2000000);
	}

	while(1)
	{
//		UARTprintf("%d\n",ulClockMS);
	}
}

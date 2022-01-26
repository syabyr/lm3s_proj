/*********************************************
U0TX:PA1
U0RX:PA0
***********************************************/
#include "inc/lm3s9b96.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "uart.h"
#include <stdint.h>
#include <stdio.h>


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

void UART0IntHandler(void)
{
	unsigned long ulStatus;
	ulStatus = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ulStatus);
	while(UARTCharsAvail(UART0_BASE))
	{
		UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
	}
}


void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
	while(ulCount--)
	{
		UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
	}
}

int main(void)
{
	//80MHz,PPL将外部时钟倍频到400MHz,1/2分频为200MHz后再分频,2.5分频后为80MHz
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//使能串口
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//使能中断
	ROM_IntMasterEnable();
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PCTL_PA0_U0RX|GPIO_PCTL_PA1_U0TX);
	ROM_UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
		(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		UART_CONFIG_PAR_NONE));

	ROM_IntEnable(INT_UART0);
	ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);


	UARTSend((unsigned char *)"Enter text: ", 12);
	UARTStdioInit(0);
	UARTprintf("Hello,world\n");
	unsigned long ulClockMS,ulCount;
	unsigned short usController;
	{
		ulClockMS=SysCtlClockGet();
		UARTprintf("系统时钟为:%d MHz\n",ulClockMS/1000000);
		SysCtlDelay(2000000);
	}

	while(1)
	{
//		UARTprintf("%d\n",ulClockMS);
	}
}

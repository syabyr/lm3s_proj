#include"inc/hw_ints.h"
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"inc/hw_ethernet.h"
#include"driverlib/adc.h"
#include"driverlib/ethernet.h"
#include"driverlib/flash.h"
#include"driverlib/debug.h"
#include"driverlib/gpio.h"
#include"driverlib/interrupt.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"driverlib/systick.h"
#include"uart.h"
#include<stdint.h>
#include<stdio.h>
#include"utils/uartstdio.h"
#include"mydelay.h"




void UARTIntHandler(void)
{
	unsigned long ulStatus;
	ulStatus=UARTIntStatus(UART0_BASE,true);
	UARTIntClear(UART0_BASE,ulStatus);
	while(UARTCharsAvail(UART0_BASE))
	{
		UARTCharPutNonBlocking(UART0_BASE,UARTCharGetNonBlocking(UART0_BASE));
	}
}

void UARTSend(const unsigned char *pucBuffer,unsigned long ulCount)
{
	while(ulCount--)
	{
		UARTCharPutNonBlocking(UART0_BASE,*pucBuffer++);
	}
}


	
int main(void)
{
	//时钟配置,100M
	SysCtlClockSet(SYSCTL_SYSDIV_2|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);				//使能串口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);					//使能PORTA,串口
	GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);		//设置PORTA的串口
	UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,		//串口配置
			(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
			UART_CONFIG_PAR_NONE));
	UARTStdioInit(0);										//使能串口输出
//	IntEnable(INT_UART0);
//	UARTIntEnable(UART0_BASE,UART_INT_RX|UART_INT_RT);

	UARTprintf("系统已经启动,时钟频率为%d Hz\n",SysCtlClockGet());

}

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "uartstdio.h"
#include "driverlib/uart.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "drivers/mydelay.h"
int counter=0;
void GPIOFHandler(void);
int main(void)
{
	unsigned long SysClock=0;

	//配置时钟,系统时钟为50Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//使能串口0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	//使能GPIOA
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//把PA0和PA1转换为功能引脚(串口)
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	//配置串口格式,波特率115200,8n1
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE|
			UART_CONFIG_PAR_NONE));
	//使能串口的中断
	IntEnable(INT_UART0);
	//使能串口中断中的收和发
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	//初始化标准输入输出
	UARTStdioInit(0);
	SysClock=SysCtlClockGet();
	UARTprintf("Uart works fine.\nSystem frequency is %d.\n",SysClock);

	//使能GPIOF,PF0为接收管脚
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
	GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);
	IntEnable(INT_GPIOF);
	IntMasterEnable();
	

	
	while(1)
	{

	};
	
}

unsigned char code=0;
void GPIOFHandler(void)
{
	uint8_t i=0,j=0,k=0;
	IntDisable(INT_GPIOF);
	unsigned long ulStatus=GPIOPinIntStatus(GPIO_PORTF_BASE,true);
	GPIOPinIntClear(GPIO_PORTF_BASE,ulStatus);
	if(ulStatus&&0x01)
	{
//		UARTprintf("code: ");
		while(1)
		{
			while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0)
			{
				_delay_us(50);
				i+=1;
			}
			while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==1)
			{
				_delay_us(50);
				j+=1;
				if(j>200)
				{
					j=0;
			//		UARTprintf("\n");
					IntEnable(INT_GPIOF);
					return;
				}
			}
			if((j>16)&&(j<40))
			{
				code=code<<1;
				code=code|0x01;
				k++;
			}
			else if((j>5)&&(j<16))
			{
				code=code<<1;
//				UARTprintf("0");
				k++;
			}
		if((k%8==0)&&(k!=0))
			UARTprintf("%2x ",code);
		if(k==112)
			UARTprintf("\n");
		j=0;
		i=0;
		}
	}
}

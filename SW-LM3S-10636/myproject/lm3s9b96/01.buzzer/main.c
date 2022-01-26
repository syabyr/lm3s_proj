/*********************************************
蜂鸣器接:PD0
*********************************************/

#include"inc/hw_types.h"
#include"inc/hw_memmap.h"
#include"driverlib/gpio.h"
#include"driverlib/sysctl.h"
#include"mydelay.h"
int main(void)
{
	//时钟配置,100MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	//外设使用前要使能,
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	//PD0为蜂鸣器端口,同时需要在板子上将跳线接上
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,0x01);
	while(1)
	{
		GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,0);
		_delay_ms(900);
		GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,1);
		_delay_ms(100);
	}
}

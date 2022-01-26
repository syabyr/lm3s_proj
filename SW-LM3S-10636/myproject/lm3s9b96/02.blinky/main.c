/*********************************************
网卡绿色灯接:PF2
网卡桔红色灯接:PF3
*********************************************/
#include"inc/hw_types.h"
#include"inc/hw_memmap.h"
#include"inc/lm3s9b96.h"
#include"driverlib/gpio.h"
#include"driverlib/sysctl.h"
#include"mydelay.h"

int main(void)
{
	//时钟配置,100MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	//PF2,PF3为LED接口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,0x0C);
	while(1)
	{
		GPIO_PORTF_DATA_R|=0x0C;
		_delay_ms(100);
		GPIO_PORTF_DATA_R&=~(0x0C);
		_delay_ms(100);
	}
}

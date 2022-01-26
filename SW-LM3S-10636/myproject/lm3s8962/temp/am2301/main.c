/*********************************************
LED:PF0
*********************************************/
#include"inc/hw_types.h"
#include"inc/hw_memmap.h"
#include"inc/lm3s8962.h"
#include"driverlib/gpio.h"
#include"driverlib/sysctl.h"
#include"mydelay.h"


int main(void)
{
	//时钟配置,50MHz
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	//PC7为IO接口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	//设置为管脚为上拉
	GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	//设置为输出模式
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_7);
	while(1)
	{
		//设置为输出模式
		GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_7);
		//输出低电平1毫秒,给传感器发信号
		GPIO_PORTC_DATA_R&=~(GPIO_PIN_7);
		_delay_ms(1);
		
		//输出高电平,等待传感器传输信号
		GPIO_PORTC_DATA_R|=GPIO_PIN_7;
		GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_7);
		
		_delay_ms(3000);
	}
}

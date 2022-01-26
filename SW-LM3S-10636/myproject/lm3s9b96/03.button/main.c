/**********************************************
LED0:PG0
LED1:PG1
LED2:PG7
Button Ok:PC4
Buttion Up:PB5
Button Down:PB4
Button Left:PF5
Buttion Right:PC5
**********************************************/
#include"inc/lm3s9b96.h"
#include"inc/hw_ints.h"
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/interrupt.h"
#include"driverlib/sysctl.h"
#include"mydelay.h"


int a=0;
int main(void)
{
	//时钟配置,100MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,0x30);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,0x30);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,0x01);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,0x20);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE,0x83);

	GPIOPadConfigSet(GPIO_PORTB_BASE,0x30,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTC_BASE,0x30,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTF_BASE,0x20,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

	GPIOIntTypeSet(GPIO_PORTB_BASE,0x30,GPIO_FALLING_EDGE);
	GPIOIntTypeSet(GPIO_PORTC_BASE,0x30,GPIO_FALLING_EDGE);
	GPIOIntTypeSet(GPIO_PORTF_BASE,0x20,GPIO_FALLING_EDGE);

	GPIOPinIntEnable(GPIO_PORTB_BASE,0x30);
	GPIOPinIntEnable(GPIO_PORTC_BASE,0x30);
	GPIOPinIntEnable(GPIO_PORTF_BASE,0x30);

	IntEnable(INT_GPIOB);
	IntEnable(INT_GPIOC);
	IntEnable(INT_GPIOF);
	IntMasterEnable();

    //
    // Loop forever.
    //
    while(1)
    {
    }
}

void GPIOBHandler(void)
{
	unsigned long ulStatus=GPIOPinIntStatus(GPIO_PORTB_BASE,true);
	GPIOPinIntClear(GPIO_PORTB_BASE,ulStatus);
	if(ulStatus&0x10)
	{
		GPIO_PORTG_DATA_R|=0x83;
	}
	if(ulStatus&0x20)
	{
		GPIO_PORTG_DATA_R&=~(0x83);
	}
}

void GPIOCHandler(void)
{
	unsigned long ulStatus=GPIOPinIntStatus(GPIO_PORTC_BASE,true);
	GPIOPinIntClear(GPIO_PORTC_BASE,ulStatus);
	if(ulStatus&0x10)
	{
		GPIOPinWrite(GPIO_PORTG_BASE,GPIO_PIN_0,(~GPIOPinRead(GPIO_PORTG_BASE,GPIO_PIN_0)));
	}
	if(ulStatus&0x20)
	{
		GPIOPinWrite(GPIO_PORTG_BASE,GPIO_PIN_0,(~GPIOPinRead(GPIO_PORTG_BASE,GPIO_PIN_0)));
	}
}


void GPIOFHandler(void)
{
	unsigned long ulStatus=GPIOPinIntStatus(GPIO_PORTF_BASE,true);
	GPIOPinIntClear(GPIO_PORTF_BASE,ulStatus);
	if(ulStatus&0x20)
	{
		GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,1);
		_delay_ms(500);
		GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,0);
	}
}

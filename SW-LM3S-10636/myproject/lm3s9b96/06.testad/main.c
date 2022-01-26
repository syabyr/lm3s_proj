/*******************************************
AIN0:PE7

*******************************************/
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

void InitConsole(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTStdioInit(0);
}


int main(void)
{
	unsigned long ulADC0_Value[1];

	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
		SYSCTL_XTAL_16MHZ);
	InitConsole();
	UARTprintf("ADC ->\n");
	UARTprintf("  Type: Single Ended\n");
	UARTprintf("  Samples: One\n");
	UARTprintf("  Update Rate: 250ms\n");
	UARTprintf("  Input Pin: AIN0/PE7\n\n");
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
		ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 3);
	ADCIntClear(ADC0_BASE, 3);

	while(1)
	{
		ADCProcessorTrigger(ADC0_BASE, 3);
		while(!ADCIntStatus(ADC0_BASE, 3, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 3, ulADC0_Value);
		UARTprintf("AIN0 = %4d\r", ulADC0_Value[0]);
		SysCtlDelay(SysCtlClockGet() / 1200);
	}
}

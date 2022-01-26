#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ethernet.h"
#include "driverlib/debug.h"
#include "driverlib/ethernet.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/ustdlib.h"
#include "mydelay.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "oled.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/timer.h"
unsigned char adc_busy=0;	//ADC忙标志位
unsigned long adc0Value;	//保存ADC0转换的值
    unsigned long ulADC0_Value[1];
void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioInit(0);
}

void Init_Timer_A(void)
{
	//使能TIMER0模块
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); 

	//禁止定时器A
	TimerDisable(TIMER0_BASE, TIMER_A);

	//配置TIMER0模块工作在双16位定时器,周期工作模式
	TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PERIODIC);
  
	//配置8位分频器
	TimerPrescaleSet( TIMER0_BASE, TIMER_A,250);
  
	//配置定时器周期
	TimerLoadSet(TIMER0_BASE, TIMER_A, 1000);
  
	//使能时钟溢出中断
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
  
	//在NVIC中使能时钟中断
	IntEnable(INT_TIMER0A);
  
	// 使能TIMER A
	TimerEnable(TIMER0_BASE, TIMER_A);
}

//Timer0中断服务程序
void Timer0IntHandler()
{
	// 清中断标志
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);    
  
	//定时时间到,检查上一次AD转换是否完成,如果完成,进行新的转换
	UARTprintf("hello\n");
        ADCProcessorTrigger(ADC0_BASE, 3);

        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
        }
        ADCSequenceDataGet(ADC0_BASE, 3, ulADC0_Value);
        UARTprintf("AIN0 = %4d\r\n", ulADC0_Value[0]);

}

//*****************************************************************************
//
// Configure ADC0 for a single-ended input and a single sample.  Once the
// sample is ready, an interrupt flag will be set.  Using a polling method,
// the data will be read then displayed on the console via UART0.
//
//*****************************************************************************
int
main(void)
{
    //
    // This array is used for storing the data read from the ADC FIFO. It
    // must be as large as the FIFO for the sequencer in use.  This example
    // uses sequence 3 which has a FIFO depth of 1.  If another sequence
    // was used with a deeper FIFO, then the array size must be changed.
    //


    //
    // Set the clocking to run at 20 MHz (200 MHz / 10) using the PLL.  When
    // using the ADC, you must either use the PLL or supply a 16 MHz clock
    // source.
    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for ADC operation.
    //
    InitConsole();

    //
    // Display the setup on the console.
    //
    UARTprintf("ADC ->\n");
    UARTprintf("  Type: Single Ended\n");
    UARTprintf("  Samples: One\n");
    UARTprintf("  Update Rate: 250ms\n");
    UARTprintf("  Input Pin: AIN0/PE7\n\n");
    Init_Timer_A();
    //
    // The ADC0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //
    // For this example ADC0 is used with AIN0 on port E7.
    // The actual port and pins used may be different on your part, consult
    // the data sheet for more information.  GPIO port E needs to be enabled
    // so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);

    while(1);
    while(1)
    {
        ADCProcessorTrigger(ADC0_BASE, 3);
        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
        }
        ADCSequenceDataGet(ADC0_BASE, 3, ulADC0_Value);
        UARTprintf("AIN0 = %4d\r\n", ulADC0_Value[0]);
        SysCtlDelay(SysCtlClockGet() / 3000);
    }
}

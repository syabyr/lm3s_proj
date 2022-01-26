//ADC+OLED的简易示波器

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "uart.h"
#include <stdint.h>
#include <stdio.h>
#include "oled.h"


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


unsigned char adc_busy=0;
unsigned char a[]={0xff},b[]={0xf0},c[]={0x0f},d[]={0x00};
unsigned long i=0,j=0,temp[2]={0,0};

void Timer0IntHandler()
{
	// 清中断标志
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);    
  
	//定时时间到,检查上一次AD转换是否完成,如果完成,进行新的转换
	if(adc_busy==0)
	{
		ADCProcessorTrigger(ADC0_BASE, 0);
		adc_busy=1;
	}
}


void ADC0IntHandler()
{

    unsigned long adc0Value;   // Holds the ADC result

    // 清ADC0中断标志.
    // ADCIntClear (unsigned long ulBase, unsigned long ulSequenceNum) 
    ADCIntClear(ADC0_BASE, 0);
    
    //从SSO读出转换结果 (FIFO0),本例中只有一个采样.如果要使用多个转换源,需要使用数组做为参数传递给API函数,保存FIFO转换结果.
    // long ADCSequenceDataGet (unsigned long ulBase,unsigned long ulSequenceNum,unsigned long *pulBuffer) 
    ADCSequenceDataGet(ADC0_BASE, 0, &adc0Value);
//    adc0Value= ((59960 - (adc0Value * 100)) / 356);
   IntMasterDisable();

	UARTprintf("%d\n",(int)adc0Value);
//
  
    IntMasterEnable();  

    // ADC模块空闲,可以进行下一次转换
    adc_busy=0;    
 
}


void
UARTIntHandler(void)
{
    unsigned long ulStatus;

    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
    }
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}


void Init_Timer_A(void)
{
  // 使能TIMER0模块
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); 
  
  // 禁止定时器A
  // TimerDisable (unsigned long ulBase, unsigned long ulTimer)
  TimerDisable(TIMER0_BASE, TIMER_A);
  
  // 配置TIMER0模块工作在双16位定时器,周期工作模式
  // TimerConfigure (unsignedlongulBase,unsignedlongulConfig)
  TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PERIODIC);
  
  // 配置8位分频器
  // TimerPrescaleSet (unsigned long ulBase, unsigned long ulTimer, unsigned long ulValue)
  TimerPrescaleSet( TIMER0_BASE, TIMER_A,100);
  
  // 配置定时器周期
  // TimerLoadSet (unsigned long ulBase, unsigned long ulTimer, unsigned long ulValue)
  TimerLoadSet(TIMER0_BASE, TIMER_A, 2500);
  
  // 使能时钟溢出中断
  // TimerIntEnable (unsigned long ulBase, unsigned long ulIntFlags) 
  TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
  
  // 在NVIC中使能时钟中断
  IntEnable(INT_TIMER0A);
  
  // 使能TIMER A
  // TimerEnable (unsigned long ulBase, unsigned long ulTimer)
  TimerEnable(TIMER0_BASE, TIMER_A);  
  
}

void Init_ADC0(void)
{
  
 
  // 使能ADC0模块
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  
  // 配置前先关闭序列0
  // ADCSequenceDisable (unsigned long ulBase, unsigned long ulSequenceNum) 
  ADCSequenceDisable(ADC0_BASE, 0); 
  
  // 配置SS0: 由CPU软件触发,优先级0(最高)
  // ADCSequenceConfigure(unsigned long ulBase,unsigned long ulSequenceNum, unsigned long ulTrigger, unsigned long ulPriority)
  ADCSequenceConfigure(ADC0_BASE, 0,ADC_TRIGGER_PROCESSOR,0); 
  
  // 配置序列里面的每一个成员配置输入通道,是否中断,是否最后采样.本例序列中只有一个采样:ulstep=0,来自内置温度传感器TS
  // ADCSequenceStepConfigure(unsigned long ulBase,unsigned long ulSequenceNum,unsigned long ulStep,unsigned long ulConfig)
 // ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_TS |ADC_CTL_IE| ADC_CTL_END ); //
  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH1 |ADC_CTL_IE| ADC_CTL_END );
  // 使能模块中断
  // ADCIntEnable (unsigned long ulBase, unsigned long ulSequenceNum) 
  ADCIntEnable(ADC0_BASE,0);
  
  // 在NVIC中使能ADC中断
  IntEnable(INT_ADC0);
  
  // 使能序列0
  // ADCSequenceEnable (unsigned long ulBase, unsigned long ulSequenceNum) 
  ADCSequenceEnable(ADC0_BASE, 0);
  
  
  
}



int main()
{
  //系统时钟设置.
  //对于LM3S8962来说,外部晶体只能在1-8MHZ之间.但是ADC需要14-18Mhz才能正常工作,所以必须使用PLL
  //PLL频率400MHZ,预先被2分频输出,所以SYSDIV_4之后得到50MHZ.
  //该函数配置系统使用8M晶体,使能PLL,系统时钟50M. ADC的时钟直接由PLL/25(400/25=18Mhz)获得
  
  
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); 
      SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
     IntMasterEnable();
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //
    // Prompt for text to be entered.
    //
    UARTSend((unsigned char *)"Enter text: ", 12);
    UARTStdioInit(0);
    UARTprintf("Hello,world\n");
    	unsigned long ulClockMS,ulCount;
	unsigned short usController;


	ulClockMS=SysCtlClockGet();
    UARTprintf("%d\n",ulClockMS);
  Init_ADC0();
  Init_Timer_A();
  
 
  while(1)
  {

  };
  

}

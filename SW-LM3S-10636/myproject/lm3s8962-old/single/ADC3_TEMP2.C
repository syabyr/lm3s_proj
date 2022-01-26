
#include<lm3sxxx.h>
#include <stdio.h>
unsigned long Sysclk;
tBoolean ADC_EndFlag = false;

unsigned long TheSysClock = 12000000UL;	  


void put(const char *x);
void URAT_init(void);
void ADC_init(void ); 

unsigned long adcSample(void);
void tmpDisplay(unsigned long ulValue);


void SysTickInit()	  // 系统节拍定时器初始化
{
	SysTickPeriodSet(TheSysClock); 	// 设置SysTick计数器的周期值
	SysTickIntEnable(); 			// 使能SysTick中断
	IntMasterEnable(); 			// 使能处理器中断
	SysTickEnable( ); 				// 使能SysTick计数器
}
	 
/**********************************************************************/
int main()
{
	unsigned long ulValue;
	SysTickInit();
	ADC_init();
	URAT_init();
	while(1)
	{
			put("http://www.cdxy.edu.cn\n");
			put("UTAT0 active\n");
			SysCtlDelay(Sysclk/3);

	        SysCtlSleep();              //  睡眠，减少耗电以降低温度
			ulValue = adcSample( ); 	// 唤醒后ADC温度采样
			tmpDisplay(ulValue); 		// 通过UART显示芯片温度值 			
	}	
}

/************************************************************************/
void System_init(void)
{

    SysCtlLDOSet(SYSCTL_LDO_2_75V);                         //  配置PLL前须将LDO设为2.75V
   	SysCtlClockSet(SYSCTL_XTAL_6MHZ | SYSCTL_SYSDIV_16 | SYSCTL_USE_PLL  | SYSCTL_OSC_MAIN );	// External crystal is 6 MHz
                      
	//  外接6MHz晶振, 分频结果为12.5MHz, 采用PLL ,主振荡器, 系统时钟设置，

    TheSysClock = SysCtlClockGet();                         //  获取当前的系统时钟频率
}

/************************************************************************/
void ADC_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);	//使能ADC模块
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS); // 设置ADC采样速率
	ADCSequenceDisable(ADC_BASE, 3); 	// 配置前先禁止采样序列
		
	ADCSequenceConfigure(ADC_BASE,3,ADC_TRIGGER_PROCESSOR,0);  
	//配置ADC采样序列的触发事件和优先级：ADC基址，采样序列编号，触发事件，采样优先级
	ADCSequenceStepConfigure(ADC_BASE,3,0,ADC_CTL_END | ADC_CTL_TS | ADC_CTL_IE);	
	//配置ADC采样序列发生器的步进 ：ADC基址，采样序列编号，步值，通道设置 

	ADCIntEnable(ADC_BASE,3);	//使能ADC采样序列的中断
	IntEnable(INT_ADC3);	 	// 使能ADC采样序列中断
	IntMasterEnable();			// 使能处理器中断

	ADCSequenceEnable(ADC_BASE,3);	 	//使能一个ADC采样序列
}

unsigned long adcSample(void) 				// ADC采样
{
		unsigned long ulValue;
		ADCProcessorTrigger(ADC_BASE, 3); 	
		// 处理器触发采样序，调用ADCProcessorTrigger( )函数触发ADC采样
		while (!ADC_EndFlag); 				// 等待采样结束
		ADC_EndFlag = 0; 				// 清除ADC采样结束标志
		ADCSequenceDataGet(ADC_BASE, 3, &ulValue); // 读取ADC转换结果
		return(ulValue);
}

// 显示芯片温度值
void tmpDisplay(unsigned long ulValue)
{
		unsigned long ulTmp;
		char cBuf[40];
		ulTmp = 151040UL - 225 * ulValue;
		sprintf(cBuf, "%ld.", ulTmp / 1024);
		put(cBuf);
		sprintf(cBuf, "%ld", (ulTmp % 1024) / 102);
		put(cBuf);
		put("℃\r\n");
}
 
void SysTick_ISR(void)	 // SysTick计数器的中断服务函数
{
	// 仅用于唤醒CPU，而不需要做其他事情
}

void ADC_Sequence_3_ISR(void)  // ADC采样序列3的中断
{
	unsigned long ulStatus;
	ulStatus = ADCIntStatus(ADC_BASE, 3, true); // 读取中断状态
	ADCIntClear(ADC_BASE, 3); // 清除中断状态，重要
	if (ulStatus != 0) // 如果中断状态有效
	{
		ADC_EndFlag = true; // 置位ADC采样结束标志
	}
}
/************************************************************************/

void put(const char *x)
{
	while(*x!='\0')
	{
		UARTCharPut(UART0_BASE,*(x++));	   //通过UART发送字符串
	}
}

void URAT_init(void)
{
	SysCtlLDOSet(SYSCTL_LDO_2_75V);
	SysCtlClockSet(SYSCTL_XTAL_6MHZ | SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL  | SYSCTL_OSC_MAIN );	// External crystal is 16 MHz
	Sysclk=SysCtlClockGet();

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); 	//使能RX/TX所在的GPIO端口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);	//使能UART模块
	GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0 | GPIO_PIN_1);// 配置RX/TX所在管脚为UART收发功能
	UARTConfigSet(UART0_BASE,9600,UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	//配置UART端口、波特率：9600、 数据位：8、停止位：1、 校验位：无
	UARTEnable(UART0_BASE);	 // 使能UART端口
}
/************************************************************************/







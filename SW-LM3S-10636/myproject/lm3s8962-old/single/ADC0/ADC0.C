
#include<lm3sxxx.h>
#include <stdio.h>

unsigned long Sysclk;
tBoolean ADC_EndFlag = false;

void System_init(void);
void put(const char *x);
void URAT_init(void);
void ADC_init(void ); 
unsigned long adcSample(void);
		 
/************************************************************************/
int main()
{
	unsigned long ulVal;
	char cBuf[30];

	System_init();
	ADC_init();
	URAT_init();
	while(1)
	{
			ulVal = adcSample( ); 			// ADC采样
			ulVal = (ulVal * 3000)/1024; 	// 转换成电压值
			sprintf(cBuf, "ADC0 = %ld(mV)\r\n", ulVal); 	// 输出格式化
			put(cBuf); 	// 通过UART显示结果
			SysCtlDelay(Sysclk/3);

			put("http://www.cdxy.edu.cn\n");
			put("UTAT0 active\n");
			SysCtlDelay(Sysclk/3);
	}	
}

/************************************************************************/
void System_init(void)
{
	SysCtlLDOSet(SYSCTL_LDO_2_75V);
	SysCtlClockSet(SYSCTL_XTAL_6MHZ | SYSCTL_SYSDIV_16 | SYSCTL_USE_PLL  | SYSCTL_OSC_MAIN );	// External crystal is 6 MHz
	Sysclk=SysCtlClockGet(); 
}

/************************************************************************/
void ADC_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);	//使能ADC模块
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS); //设置ADC采样速率
	ADCSequenceDisable(ADC_BASE, 0); 	// 配置前先禁止采样序列
		
	ADCSequenceConfigure(ADC_BASE,0,ADC_TRIGGER_PROCESSOR,0);  
	//配置ADC采样序列的触发事件和优先级：ADC基址，采样序列编号，触发事件，采样优先级
	ADCSequenceStepConfigure(ADC_BASE,0,0,ADC_CTL_END | ADC_CTL_CH0 | ADC_CTL_IE);	
	//配置ADC采样序列发生器的步进 ：ADC基址，采样序列编号，步值，通道设置 

	ADCIntEnable(ADC_BASE,0);	//使能ADC采样序列的中断
	IntEnable(INT_ADC0);	 	// 使能ADC采样序列中断
	IntMasterEnable();			// 使能处理器中断

	ADCSequenceEnable(ADC_BASE,0);	 	// 使能一个ADC采样序列
}

unsigned long adcSample(void) 		    // ADC采样
{
	unsigned long ulValue;
	ADCProcessorTrigger(ADC_BASE, 0); 	
	// 处理器触发采样序，调用ADCProcessorTrigger( )函数触发ADC采样
	while (!ADC_EndFlag); 				// 等待采样结束
	ADC_EndFlag = 0; 				// 清除ADC采样结束标志
	ADCSequenceDataGet(ADC_BASE, 0, &ulValue); // 读取ADC转换结果
	return(ulValue);
}

void ADC_Sequence_0_ISR(void) 		// ADC采样序列0的中断函数
{
	unsigned long ulStatus;
	ulStatus = ADCIntStatus(ADC_BASE, 0, true); 	// 读取中断状态
	ADCIntClear(ADC_BASE, 0); 	// 清除中断状态，重要,等待下次AD中断
	if (ulStatus != 0) 			// 如果中断状态有效
	{
		ADC_EndFlag = 1; 	// 置位ADC采样结束标志
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
	GPIOPinTypeUART(GPIO_BASE_PORTA,GPIO_PIN_0 | GPIO_PIN_1);// 配置RX/TX所在管脚为UART收发功能
	UARTConfigSet(UART0_BASE,9600,UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	//配置UART端口、波特率：9600、 数据位：8、停止位：1、 校验位：无
	UARTEnable(UART0_BASE);	 // 使能UART端口
}

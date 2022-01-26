#include"inc/hw_ints.h"
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"inc/hw_ethernet.h"
#include"driverlib/adc.h"
#include"driverlib/ethernet.h"
#include"driverlib/flash.h"
#include"driverlib/debug.h"
#include"driverlib/gpio.h"
#include"driverlib/interrupt.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"driverlib/systick.h"
#include"uip/uip.h"
#include"uip/uip_arp.h"
#include"httpd/httpd.h"
#include"apps/dhcpc/dhcpc.h"
#include"uart.h"
#include<stdint.h>
#include<stdio.h>
#include"grlib/grlib.h"
#include"drivers/320x480_9B95.h"
#include"drivers/mydelay.h"
#include"utils/uartstdio.h"


#ifdef DEBUG
void
__error__(char *pcFilename,unsigned long ulLine)
{
}
#endif


#define SYSTICKHZ	CLOCK_CONF_SECOND
#define SYSTICKMS	(1000/SYSTICKHZ)
#define SYSTICKUS	(1000000/SYSTICKHZ)
#define SYSTICKNS	(1000000000/SYSTICKHZ)


#define BUF	((struct uip_eth_hdr *)&uip_buf[0])


#define FLAG_SYSTICK	0
#define FLAG_RXPKT	1
static volatile unsigned long g_ulFlags;

volatile unsigned long g_ulTickCounter=0;

int ia=0;
#define DEFAULT_IPADDR0	192
#define DEFAULT_IPADDR1	168
#define DEFAULT_IPADDR2	2
#define DEFAULT_IPADDR3	2

#define DEFAULT_NETMASK0	255
#define DEFAULT_NETMASK1	255
#define DEFAULT_NETMASK2	255
#define DEFAULT_NETMASK3	0

#define UIP_PERIODIC_TIMER_MS	500
#define UIP_ARP_TIMER_MS	10000

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

unsigned long fori=0;

unsigned char adc_busy=0;	//ADC忙标志位
unsigned long adc0Value[1];	//保存ADC0转换的值

unsigned char pre=0,this=0;



unsigned char data[2009]="b mybays hello";		//保存最终的数据,预存9个字符,冲data[9]开始为数据.
unsigned long datal[2000];
unsigned long datac=0,dataflag=0;


void SysTickIntHandler(void)
{

	g_ulTickCounter++;
	HWREGBITW(&g_ulFlags,FLAG_SYSTICK)=1;
}

clock_time_t clock_time(void)
{
	return((clock_time_t)g_ulTickCounter);
}

void EthernetIntHandler(void)
{
	unsigned long ulTemp;
	ulTemp=EthernetIntStatus(ETH_BASE,false);
	EthernetIntClear(ETH_BASE,ulTemp);
	if(ulTemp&ETH_INT_RX)
	{
		HWREGBITW(&g_ulFlags,FLAG_RXPKT)=1;
		EthernetIntDisable(ETH_BASE,ETH_INT_RX);
	}
}

static void DisplayIPAddress(void *ipaddr,unsigned long ulCol,unsigned long ulRow)
{
	char pucBuf[16];
	unsigned char *pucTemp=(unsigned char *)ipaddr;
	usprintf(pucBuf,"%d.%d.%d.%d",pucTemp[0],pucTemp[1],pucTemp[2],
		pucTemp[3]);
	UARTprintf("系统IP地址为:%s\n",pucBuf);
}

void dhcpc_configured(const struct dhcpc_state *s)
{
	uip_sethostaddr(&s->ipaddr);
	uip_setnetmask(&s->netmask);
	uip_setdraddr(&s->default_router);
	DisplayIPAddress((void *)&s->ipaddr,18,24);
}



void UARTIntHandler(void)
{
	unsigned long ulStatus;
	ulStatus=UARTIntStatus(UART0_BASE,true);
	UARTIntClear(UART0_BASE,ulStatus);
	while(UARTCharsAvail(UART0_BASE))
	{
		UARTCharPutNonBlocking(UART0_BASE,UARTCharGetNonBlocking(UART0_BASE));
	}
}

void UARTSend(const unsigned char *pucBuffer,unsigned long ulCount)
{
	while(ulCount--)
	{
		UARTCharPutNonBlocking(UART0_BASE,*pucBuffer++);
	}
}

void Timer0IntHandler()
{
	//清中断标志
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	if(adc_busy==0)
	{
		ADCProcessorTrigger(ADC0_BASE,3);
		adc_busy=1;
	}
}

//ADC0转换中断处理程序
void ADC0IntHandler()
{
	//清ADC0中断标志.
	ADCIntClear(ADC0_BASE,3);
	//从SSO读出转换结果 (FIFO0),本例中只有一个采样.如果要使用多个转换源,需要使用数组做为参数传递给API函数,保存FIFO转换结果.
	ADCSequenceDataGet(ADC0_BASE, 3, adc0Value);
	IntMasterDisable();
	UARTprintf("%d %d\n",fori,adc0Value[0]>>2);
		this=(char) (adc0Value[0]>>2);
	if(dataflag)
	{
		datal[datac]=adc0Value[0];
		datac++;
		if(datac==2000)
		{
			dataflag=0;
//			data[2008]='\0';
			connect_server();
			GPIOPinWrite(GPIO_PORTG_BASE,GPIO_PIN_0,0);
			datac=0;
		}
	}
	draw_neb(fori);
	pre=this;
	fori++;
	if(fori==480)
		fori=0;
	clean_neb(fori+1);
	IntMasterEnable();
	adc_busy=0;

}


uip_ipaddr_t ipaddr;
static struct uip_eth_addr sTempAddr;
long lPeriodicTimer,lARPTimer,lPacketLength;
unsigned long ulUser0,ulUser1;
unsigned long ulTemp;

int main(void)
{
	//时钟配置,50M
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);				//使能串口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);					//使能PORTA,串口
	GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);		//设置PORTA的串口
	UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,		//串口配置
			(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
			UART_CONFIG_PAR_NONE));
	UARTStdioInit(0);										//使能串口输出
//	IntEnable(INT_UART0);
//	UARTIntEnable(UART0_BASE,UART_INT_RX|UART_INT_RT);

	UARTprintf("系统已经启动,时钟频率为%d Hz\n",SysCtlClockGet());


	//使能要连接的端口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);		//PE0-PE3,共4个,分别为PE0-CS,PE1-RS,PE2-WR,PE3-RD
		GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,0x0F);	//0B00001111
		GPIOPinWrite(GPIO_PORTE_BASE,0x0F,0x0E);	//RS,WR,RD信号,PE1,PE2,PE3,默认都为高电平
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);		//PF4,液晶RST信号,PF2,3,网卡LED
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_4);
		GPIOPinTypeEthernetLED(GPIO_PORTF_BASE,GPIO_PIN_2|GPIO_PIN_3);
		GPIOPinConfigure(GPIO_PF3_LED0);
		GPIOPinConfigure(GPIO_PF2_LED1);
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_4,true);	//,默认为高电平
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);		//PG0,LED指示灯,输出
		GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE,GPIO_PIN_0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);		//液晶数据低8位,全用
		GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE,0xff);
		GPIOPinWrite(GPIO_PORTH_BASE,0xff,0x00);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);		//液晶数据高8位,全用
		GPIOPinTypeGPIOOutput(GPIO_PORTJ_BASE,0xff);
		GPIOPinWrite(GPIO_PORTJ_BASE,0xff,0x00);

	GPIOPinWrite(GPIO_PORTE_BASE,0x01,0x01);		//使能液晶及其背光

	LCD_320x480_ILI9481Init();
	Draw_Frame();				//绘制液晶大的框架


	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);		//确定按键PC4,按下后进行数据传输
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOIntTypeSet(GPIO_PORTC_BASE,GPIO_PIN_4,GPIO_FALLING_EDGE);
	GPIOPinIntEnable(GPIO_PORTC_BASE,GPIO_PIN_4);
	IntEnable(INT_GPIOC);			//必须使能后才能使用

	UARTprintf("初始化网卡:\n");
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
	SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

	SysTickPeriodSet(SysCtlClockGet()/SYSTICKHZ);
	SysTickEnable();
	SysTickIntEnable();

	EthernetIntDisable(ETH_BASE,(ETH_INT_PHY|ETH_INT_MDIO|ETH_INT_RXER|
		ETH_INT_RXOF|ETH_INT_TX|ETH_INT_TXER|ETH_INT_RX));
	ulTemp=EthernetIntStatus(ETH_BASE,false);
	EthernetIntClear(ETH_BASE,ulTemp);
	EthernetInitExpClk(ETH_BASE,SysCtlClockGet());
	EthernetConfigSet(ETH_BASE,(ETH_CFG_TX_DPLXEN|ETH_CFG_TX_CRCEN|
		ETH_CFG_TX_PADEN));
	UARTprintf("Waiting for Link..........\n");
	ulTemp=EthernetPHYRead(ETH_BASE,PHY_MR1);
	while((ulTemp&0x0004)==0)
	{
		ulTemp=EthernetPHYRead(ETH_BASE,PHY_MR1);
	}
	UARTprintf("Link Established.\n");
	EthernetEnable(ETH_BASE);
	IntEnable(INT_ETH);
	EthernetIntEnable(ETH_BASE,ETH_INT_RX);
	IntMasterEnable();
	uip_init();

	uip_ipaddr(ipaddr,0,0,0,0);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr,0,0,0,0);
	uip_setnetmask(ipaddr);
	ulUser0=0x11111111;
	ulUser1=0x11111111;
	sTempAddr.addr[0]=((ulUser0>>0)&0xff);
	sTempAddr.addr[1]=((ulUser0>>8)&0xff);
	sTempAddr.addr[2]=((ulUser0>>16)&0xff);
	sTempAddr.addr[3]=((ulUser1>>0)&0xff);
	sTempAddr.addr[4]=((ulUser1>>8)&0xff);
	sTempAddr.addr[5]=((ulUser1>>16)&0xff);

	EthernetMACAddrSet(ETH_BASE,(unsigned char *)&sTempAddr);
		uip_setethaddr(sTempAddr);


	UARTprintf("Using DHCP..........\n");
	dhcpc_init(&sTempAddr.addr[0],6);
	dhcpc_request();
	UARTprintf("DHCP Finished..........\n");


//TimerA初始化程序

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerDisable(TIMER0_BASE,TIMER_A);
	//配置TIMER0模块工作在双16位定时器,周期工作模式
	TimerConfigure(TIMER0_BASE,TIMER_CFG_32_BIT_PER);
	//配置定时器周期
	TimerLoadSet(TIMER0_BASE,TIMER_A,SysCtlClockGet()/300);
	//使能时钟溢出中断
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	//在NVIC中使能时钟中断
	IntEnable(INT_TIMER0A);
	//使能TIMERA
	TimerEnable(TIMER0_BASE,TIMER_A);


	//ADC0初始化
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_7);	//PE7,ADC0
//	SysCtlPeripheralEnable(SYSCTL_ADCSPEED_1MSPS);
//	ADCSequenceDisable(ADC0_BASE,3);
	ADCSequenceConfigure(ADC0_BASE,3,ADC_TRIGGER_PROCESSOR,0);
	ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH0|ADC_CTL_IE|
			ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE,3);
	ADCIntClear(ADC0_BASE,3);
	//中断部分
	ADCIntEnable(ADC0_BASE,3);
	IntEnable(INT_ADC0SS3);
	IntMasterEnable();
	ADCSequenceEnable(ADC0_BASE,3);


	httpd_init();
	lPeriodicTimer=0;
	lARPTimer=0;
	IntMasterEnable();
	//main loop
	while(true)
	{
		while(!g_ulFlags)
		{
		}
		if(HWREGBITW(&g_ulFlags,FLAG_SYSTICK)==1)
		{
			HWREGBITW(&g_ulFlags,FLAG_SYSTICK)=0;
			lPeriodicTimer+=SYSTICKMS;
			lARPTimer+=SYSTICKMS;
		}
		lPacketLength=EthernetPacketGetNonBlocking(ETH_BASE,uip_buf,
						sizeof(uip_buf));
		if(lPacketLength>0)
		{

			uip_len=(unsigned short)lPacketLength;
			if(HWREGBITW(&g_ulFlags,FLAG_RXPKT)==1)
			{
				HWREGBITW(&g_ulFlags,FLAG_RXPKT)=0;
				EthernetIntEnable(ETH_BASE,ETH_INT_RX);
			}
			if(BUF->type==htons(UIP_ETHTYPE_IP))
			{
				uip_arp_ipin();
				uip_input();
				if(uip_len>0)
				{
					uip_arp_out();
					EthernetPacketPut(ETH_BASE,uip_buf,uip_len);
					uip_len=0;
				}
			}
			else if(BUF->type==htons(UIP_ETHTYPE_ARP))
			{
				uip_arp_arpin();
				if(uip_len>0)
				{
					EthernetPacketPut(ETH_BASE,uip_buf,uip_len);
					uip_len=0;
				}
			}
		}

		if(lPeriodicTimer>UIP_PERIODIC_TIMER_MS)
		{
			lPeriodicTimer=0;
			for(ulTemp=0;ulTemp<UIP_CONNS;ulTemp++)
			{
				uip_periodic(ulTemp);
				if(uip_len>0)
				{
					uip_arp_out();
					EthernetPacketPut(ETH_BASE,uip_buf,uip_len);
					uip_len=0;
				}
			}

#if UIP_UDP
			for(ulTemp=0;ulTemp<UIP_UDP_CONNS;ulTemp++)
			{
				uip_udp_periodic(ulTemp);
				if(uip_len>0)
				{
					uip_arp_out();
					EthernetPacketPut(ETH_BASE,uip_buf,uip_len);
					uip_len=0;
				}
			}
#endif // UIP_UDP
		}

		if(lARPTimer>UIP_ARP_TIMER_MS)
		{
			lARPTimer=0;
			uip_arp_timer();
		}
	}
	//end of main loop
}



//绘制大的框架
void Draw_Frame(void)
{
	//信息部分
	tContext sContext;
	tRectangle sRect;
	GrContextInit(&sContext,&g_sLCD_320x480_ILI9481);
	sRect.sXMin=0;
	sRect.sYMin=0;
	sRect.sXMax=GrContextDpyWidthGet(&sContext) - 1;
	sRect.sYMax=40;
	GrContextForegroundSet(&sContext, ClrBlue);
	GrRectFill(&sContext, &sRect);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &sRect);

	//心电部分
	tRectangle elecRect;
	elecRect.sXMin=0;
	elecRect.sYMin=320-256;
	elecRect.sXMax=480-1;
	elecRect.sYMax=320-1;
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectFill(&sContext, &elecRect);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &elecRect);
}






void GPIOCHandler(void)
{
	unsigned long ulStatus=GPIOPinIntStatus(GPIO_PORTC_BASE,true);
	GPIOPinIntClear(GPIO_PORTC_BASE,ulStatus);
	if(ulStatus&0x10)
	{
		dataflag=1;
		GPIOPinWrite(GPIO_PORTG_BASE,GPIO_PIN_0,1);
//		GPIOPinWrite(GPIO_PORTG_BASE,GPIO_PIN_0,(~GPIOPinRead(GPIO_PORTG_BASE,GPIO_PIN_0)));	//调试GPIOC使用
	}
}

int connect_server(void)
{
	uip_ipaddr_t ipaddr;
	uip_ipaddr(ipaddr,192,168,1,109);
	struct uip_conn *conn;
	conn=uip_connect(ipaddr,HTONS(9999));
	if(conn==0)
	{
		uip_abort();
		return -1;
	}
}


void draw_neb(uint16_t i)
{
	uint16_t middle;
	if(pre==this)
	{
		LCD_320x480_ILI9481LineDrawH(NULL,i-1,i,319-pre,ClrBlack);
	}
	else if(pre<this)
	{
		middle=(pre+this)/2;
		LCD_320x480_ILI9481LineDrawV(NULL,i-1,319-middle,319-pre,ClrBlack);
		LCD_320x480_ILI9481LineDrawV(NULL,i,319-this,319-middle,ClrBlack);
	}
	else if(pre>this)
	{
		middle=(pre+this)/2;
		LCD_320x480_ILI9481LineDrawV(NULL,i-1,319-pre,319-middle,ClrBlack);
		LCD_320x480_ILI9481LineDrawV(NULL,i,319-middle,319-this,ClrBlack);	
	}
	
}


void clean_neb(uint16_t i)
{
	LCD_320x480_ILI9481LineDrawV(NULL,i,319-256,319,ClrWhite);
}


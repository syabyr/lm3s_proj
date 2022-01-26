//*****************************************************************************
//
// enet_uip.c - Sample WebServer Application for Ethernet Demo
//
// Copyright (c) 2007-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6852 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

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
#include "drivers/rit128x96x4.h"
#include "uip/uip.h"
#include "uip/uip_arp.h"
#include "client.h"
#include "apps/dhcpc/dhcpc.h"
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
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Ethernet with uIP (enet_uip)</h1>
//!
//! This example application demonstrates the operation of the Stellaris
//! Ethernet controller using the uIP TCP/IP Stack.  DHCP is used to obtain
//! an Ethernet address.  A basic web site is served over the Ethernet port.
//! The web site displays a few lines of text, and a counter that increments
//! each time the page is sent.
//!
//! For additional details on uIP, refer to the uIP web page at:
//! http://www.sics.se/~adam/uip/
//
//*****************************************************************************

//*****************************************************************************
//
// Defines for setting up the system clock.
//
//*****************************************************************************
#define SYSTICKHZ               CLOCK_CONF_SECOND
#define SYSTICKMS               (1000 / SYSTICKHZ)
#define SYSTICKUS               (1000000 / SYSTICKHZ)
#define SYSTICKNS               (1000000000 / SYSTICKHZ)

//*****************************************************************************
//
// Macro for accessing the Ethernet header information in the buffer.
//
//*****************************************************************************
#define BUF                     ((struct uip_eth_hdr *)&uip_buf[0])

//*****************************************************************************
//
// A set of flags.  The flag bits are defined as follows:
//
//     0 -> An indicator that a SysTick interrupt has occurred.
//     1 -> An RX Packet has been received.
//
//*****************************************************************************
#define FLAG_SYSTICK            0
#define FLAG_RXPKT              1
static volatile unsigned long g_ulFlags;

//*****************************************************************************
//
// A system tick counter, incremented every SYSTICKMS.
//
//*****************************************************************************
volatile unsigned long g_ulTickCounter = 0;

//*****************************************************************************
//
// Default TCP/IP Settings for this application.
//
// Default to Link Local address ... (169.254.1.0 to 169.254.254.255).  Note:
// This application does not implement the Zeroconf protocol.  No ARP query is
// issued to determine if this static IP address is already in use.
//
// TODO:  Uncomment the following #define statement to enable STATIC IP
// instead of DHCP.
//
//*****************************************************************************
//#define USE_STATIC_IP

#ifndef DEFAULT_IPADDR0
#define DEFAULT_IPADDR0         192
#endif

#ifndef DEFAULT_IPADDR1
#define DEFAULT_IPADDR1         168
#endif

#ifndef DEFAULT_IPADDR2
#define DEFAULT_IPADDR2         1
#endif

#ifndef DEFAULT_IPADDR3
#define DEFAULT_IPADDR3         111
#endif

#ifndef DEFAULT_NETMASK0
#define DEFAULT_NETMASK0        255
#endif

#ifndef DEFAULT_NETMASK1
#define DEFAULT_NETMASK1        255
#endif

#ifndef DEFAULT_NETMASK2
#define DEFAULT_NETMASK2        255
#endif

#ifndef DEFAULT_NETMASK3
#define DEFAULT_NETMASK3        0
#endif

//*****************************************************************************
//
// UIP Timers (in MS)
//
//*****************************************************************************
#define UIP_PERIODIC_TIMER_MS   500
#define UIP_ARP_TIMER_MS        10000

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************



#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif
unsigned char adc_busy=0;	//ADC忙标志位
unsigned long adc0Value;	//保存ADC0转换的值



unsigned char a[]={0xff},b[]={0xf0},c[]={0x0f},d[]={0x00};
unsigned long i=0,j=0,temp[2]={0,0};

unsigned char ads[128];



unsigned char data[2009]="b mybays ";		//保存最终的数据,预存9个字符,冲data[9]开始为数据.
unsigned long datac=0,dataflag=0;

void UART0Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,
		(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
		UART_CONFIG_PAR_NONE));
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE,UART_INT_RX|UART_INT_RT);
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


//Timer0中断服务程序
void Timer0IntHandler()
{
	// 清中断标志
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);    
  
	//定时时间到,检查上一次AD转换是否完成,如果完成,进行新的转换
	if(adc_busy==0)
	{
		ADCProcessorTrigger(ADC0_BASE,0);
		adc_busy=1;
	}
}

//ADC0转换中断处理程序
void ADC0IntHandler()
{
	//清ADC0中断标志.
	ADCIntClear(ADC0_BASE,0);
	//从SSO读出转换结果 (FIFO0),本例中只有一个采样.如果要使用多个转换源,需要使用数组做为参数传递给API函数,保存FIFO转换结果.
	ADCSequenceDataGet(ADC0_BASE, 0, &adc0Value);
	//adc0Value= ((59960 - (adc0Value * 100)) / 356);
	IntMasterDisable();
	ads[i]=(unsigned char)(adc0Value>>2);
	if(dataflag)
	{
		data[datac+9]=(unsigned char)(adc0Value>>2);
		datac++;
		if(datac==1999)
		{
			dataflag=0;
			data[2008]='\0';
			connect_server();
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0);
			datac=0;

		}
		
	}
	if((i&1)!=1)
	{
		temp[0]=(int)(adc0Value/11);
		for (j=0;j<96;j++)
		OLED_ImageDraw(d,i,(int)j,2,1);
	}
	else
	{
		temp[1]=(int)(adc0Value/11);
		if(temp[0]==temp[1])
			OLED_ImageDraw(a,i-1,93-temp[0],2,1);
		else
		{
			OLED_ImageDraw(b,i-1,93-temp[0],2,1);
			OLED_ImageDraw(c,i-1,93-temp[1],2,1);
		}
	}


	i++;
	if(i==128)
		i=0;
	IntMasterEnable();

	//ADC模块空闲,可以进行下一次转换
	adc_busy=0;
}


//系统嘀嗒时钟中断处理程序
void SysTickIntHandler(void)
{
	// Increment the system tick count.
	g_ulTickCounter++;

	// Indicate that a SysTick interrupt has occurred.
	HWREGBITW(&g_ulFlags, FLAG_SYSTICK) = 1;
}


//网卡事件中断处理程序
void EthernetIntHandler(void)
{
	unsigned long ulTemp;
	// Read and Clear the interrupt.
	ulTemp = EthernetIntStatus(ETH_BASE, false);
	EthernetIntClear(ETH_BASE, ulTemp);

	// Check to see if an RX Interrupt has occured.
	if(ulTemp & ETH_INT_RX)
	{
		// Indicate that a packet has been received.
		HWREGBITW(&g_ulFlags, FLAG_RXPKT) = 1;
		// Disable Ethernet RX Interrupt.
		EthernetIntDisable(ETH_BASE, ETH_INT_RX);
	}
}
void GPIOFHandler(void)
{
	unsigned long ulStatus=GPIOPinIntStatus(GPIO_PORTF_BASE,true);
	GPIOPinIntClear(GPIO_PORTF_BASE,ulStatus);
	if(ulStatus&0x02)
	{
		//GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,(~GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)));
		//connect_server();
		dataflag=1;
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,1);
	}
}


//TimerA初始化程序
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

//AD转换初始化程序
void Init_ADC0(void)
{
	// 使能ADC0模块
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  
	// 配置前先关闭序列0
	ADCSequenceDisable(ADC0_BASE, 0);

	//配置SS0: 由CPU软件触发,优先级0(最高)
	ADCSequenceConfigure(ADC0_BASE, 0,ADC_TRIGGER_PROCESSOR,0);
  
	// 配置序列里面的每一个成员配置输入通道,是否中断,是否最后采样.本例序列中只有一个采样:ulstep=0,来自内置温度传感器TS
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH1 |ADC_CTL_IE| ADC_CTL_END);
	//使能模块中断
	ADCIntEnable(ADC0_BASE,0);
	//在NVIC中使能ADC中断
	IntEnable(INT_ADC0);
	//使能序列0
	ADCSequenceEnable(ADC0_BASE, 0);
}

void Init_SYSTICK(void)
{
	// Configure SysTick for a periodic interrupt.
	//产生SYSTICKHZ频率的系统嘀嗒时钟
	SysTickPeriodSet(SysCtlClockGet() / SYSTICKHZ);
	SysTickEnable();
	SysTickIntEnable();
}

void Init_Button(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE);
	GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_1);
	IntEnable(INT_GPIOF);
}

//*****************************************************************************
//
//! When using the timer module in UIP, this function is required to return
//! the number of ticks.  Note that the file "clock-arch.h" must be provided
//! by the application, and define CLOCK_CONF_SECONDS as the number of ticks
//! per second, and must also define the typedef "clock_time_t".
//
//*****************************************************************************
clock_time_t clock_time(void)
{
	return((clock_time_t)g_ulTickCounter);
}



//*****************************************************************************
//
// Display a uIP type IP Address.
//
//*****************************************************************************
static void DisplayIPAddress(void *ipaddr, unsigned long ulCol, unsigned long ulRow)
{
	char pucBuf[16];
	unsigned char *pucTemp = (unsigned char *)ipaddr;

	//
	// Convert the IP Address into a string.
	//
	usprintf(pucBuf, "%d.%d.%d.%d", pucTemp[0], pucTemp[1], pucTemp[2],
		pucTemp[3]);

	//
	// Display the string.
	//
//	RIT128x96x4StringDraw(pucBuf, ulCol, ulRow, 15);
	UARTprintf("%s\n",pucBuf);
}

//*****************************************************************************
//
// Callback for when DHCP client has been configured.
//
//*****************************************************************************
void dhcpc_configured(const struct dhcpc_state *s)
{
	uip_sethostaddr(&s->ipaddr);
	uip_setnetmask(&s->netmask);
	uip_setdraddr(&s->default_router);
	DisplayIPAddress((void *)&s->ipaddr, 18, 24);
}

//*****************************************************************************
//
// This example demonstrates the use of the Ethernet Controller with the uIP
// TCP/IP stack.
//
//*****************************************************************************
int main(void)
{
	uip_ipaddr_t ipaddr;
	static struct uip_eth_addr sTempAddr;
	long lPeriodicTimer, lARPTimer, lPacketLength;
	unsigned long ulUser0, ulUser1;
	unsigned long ulTemp;

	// Set the clocking to run directly from the crystal.
	//系统时钟8MHz,网卡使用自己的晶振
//	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	UART0Init();
	UARTStdioInit(0);
	UARTprintf("Hello,world\n");
	unsigned long ulClockMS;
	ulClockMS=SysCtlClockGet();
	UARTprintf("CPU Frequency is %d Hz\n",ulClockMS);



//	RIT128x96x4Init(1000000);

	UARTprintf("Ethernet with uIP:\n");

	// Enable and Reset the Ethernet Controller.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
	SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

	// Enable Port F for Ethernet LEDs.
	//  LED0        Bit 3   Output
	//  LED1        Bit 2   Output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

	Init_SYSTICK();

	//
	// Intialize the Ethernet Controller and disable all Ethernet Controller
	// interrupt sources.
	//停止网卡控制器的所有中断
	EthernetIntDisable(ETH_BASE, (ETH_INT_PHY | ETH_INT_MDIO | ETH_INT_RXER |
		ETH_INT_RXOF | ETH_INT_TX | ETH_INT_TXER | ETH_INT_RX));
	ulTemp = EthernetIntStatus(ETH_BASE, false);
	EthernetIntClear(ETH_BASE, ulTemp);

	//
	// Initialize the Ethernet Controller for operation.
	//
	EthernetInitExpClk(ETH_BASE, SysCtlClockGet());

	//
	// Configure the Ethernet Controller for normal operation.
	// - Full Duplex
	// - TX CRC Auto Generation
	// - TX Padding Enabled
	//
	EthernetConfigSet(ETH_BASE, (ETH_CFG_TX_DPLXEN | ETH_CFG_TX_CRCEN |
		ETH_CFG_TX_PADEN));

	//
	// Wait for the link to become active.
	// 
	UARTprintf("Waiting for Link..........\n");
	ulTemp = EthernetPHYRead(ETH_BASE, PHY_MR1);
	//等待建立硬件链路
	while((ulTemp & 0x0004) == 0)
	{
		ulTemp = EthernetPHYRead(ETH_BASE, PHY_MR1);
	}
	UARTprintf("Link Established.\n");

	EthernetEnable(ETH_BASE);

	IntEnable(INT_ETH);

	EthernetIntEnable(ETH_BASE, ETH_INT_RX);

	Init_Button();
	IntMasterEnable();

	uip_init();
#ifdef USE_STATIC_IP
	uip_ipaddr(ipaddr, DEFAULT_IPADDR0, DEFAULT_IPADDR1, DEFAULT_IPADDR2,
		DEFAULT_IPADDR3);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, DEFAULT_NETMASK0, DEFAULT_NETMASK1, DEFAULT_NETMASK2,
		DEFAULT_NETMASK3);
	uip_setnetmask(ipaddr);
#else
	uip_ipaddr(ipaddr, 0, 0, 0, 0);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, 0, 0, 0, 0);
	uip_setnetmask(ipaddr);
#endif
	ulUser0=0x11111111;
	ulUser1=0x11111111;
	sTempAddr.addr[0] = ((ulUser0 >>  0) & 0xff);
	sTempAddr.addr[1] = ((ulUser0 >>  8) & 0xff);
	sTempAddr.addr[2] = ((ulUser0 >> 16) & 0xff);
	sTempAddr.addr[3] = ((ulUser1 >>  0) & 0xff);
	sTempAddr.addr[4] = ((ulUser1 >>  8) & 0xff);
 	sTempAddr.addr[5] = ((ulUser1 >> 16) & 0xff);

	EthernetMACAddrSet(ETH_BASE, (unsigned char *)&sTempAddr);
		uip_setethaddr(sTempAddr);
#ifndef USE_STATIC_IP
	dhcpc_init(&sTempAddr.addr[0], 6);
	dhcpc_request();
#endif
	lPeriodicTimer = 0;
	lARPTimer = 0;
	OLED_Init(1000000);
	Init_ADC0();
	Init_Timer_A();
//	connect_server();
	while(true)
	{
		// Wait for an event to occur.  This can be either a System Tick event,
		// or an RX Packet event.
		while(!g_ulFlags)
		{
		}
		// If SysTick, Clear the SysTick interrupt flag and increment the
		// timers.
		if(HWREGBITW(&g_ulFlags, FLAG_SYSTICK) == 1)
		{
			HWREGBITW(&g_ulFlags, FLAG_SYSTICK) = 0;
			lPeriodicTimer += SYSTICKMS;
			lARPTimer += SYSTICKMS;
		}
		// Check for an RX Packet and read it.
		lPacketLength = EthernetPacketGetNonBlocking(ETH_BASE, uip_buf,
						sizeof(uip_buf));
		if(lPacketLength > 0)
		{
			// Set uip_len for uIP stack usage.
			uip_len = (unsigned short)lPacketLength;
			// Clear the RX Packet event and renable RX Packet interrupts.
			if(HWREGBITW(&g_ulFlags, FLAG_RXPKT) == 1)
			{
				HWREGBITW(&g_ulFlags, FLAG_RXPKT) = 0;
				EthernetIntEnable(ETH_BASE, ETH_INT_RX);
			}
			// Process incoming IP packets here.
			if(BUF->type == htons(UIP_ETHTYPE_IP))
			{
				uip_arp_ipin();
				uip_input();

				// If the above function invocation resulted in data that
				// should be sent out on the network, the global variable
				// uip_len is set to a value > 0.
				if(uip_len > 0)
				{
					uip_arp_out();
					EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
					uip_len = 0;
				}
			}
			// Process incoming ARP packets here.
			else if(BUF->type == htons(UIP_ETHTYPE_ARP))
			{
				uip_arp_arpin();
				// If the above function invocation resulted in data that
				// should be sent out on the network, the global variable
				// uip_len is set to a value > 0.
				if(uip_len > 0)
				{
					EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
					uip_len = 0;
				}
			}
		}
		// Process TCP/IP Periodic Timer here.
		if(lPeriodicTimer > UIP_PERIODIC_TIMER_MS)
		{
			lPeriodicTimer = 0;
			for(ulTemp = 0; ulTemp < UIP_CONNS; ulTemp++)
			{
				uip_periodic(ulTemp);

				// If the above function invocation resulted in data that
				// should be sent out on the network, the global variable
				// uip_len is set to a value > 0.
				if(uip_len > 0)
				{
					uip_arp_out();
					EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
					uip_len = 0;
				}
			}
#if UIP_UDP
			for(ulTemp = 0; ulTemp < UIP_UDP_CONNS; ulTemp++)
			{
				uip_udp_periodic(ulTemp);
				// If the above function invocation resulted in data that
				// should be sent out on the network, the global variable
				// uip_len is set to a value > 0.
				if(uip_len > 0)
				{
					uip_arp_out();
					EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
					uip_len = 0;
				}
			}
#endif // UIP_UDP
		}
		// Process ARP Timer here.
		if(lARPTimer > UIP_ARP_TIMER_MS)
		{
			lARPTimer = 0;
			uip_arp_timer();
		}
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

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"

#include "oled.h"

unsigned char bMAC[]={0x12,0x34,0x56,0x78,0x90,0xab};

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

int main()
{
  //系统时钟设置.
  //对于LM3S8962来说,外部晶体只能在1-8MHZ之间.但是ADC需要14-18Mhz才能正常工作,所以必须使用PLL
  //PLL频率400MHZ,预先被2分频输出,所以SYSDIV_4之后得到50MHZ.
  //该函数配置系统使用8M晶体,使能PLL,系统时钟50M. ADC的时钟直接由PLL/25(400/25=18Mhz)获得
  
	//设置系统时钟,8MHz  
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); 
	//使能和复位以太网控制器
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
	SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);
	//使能以太网LED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_DIR_MODE_HW);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	//GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);


	// Initialize the Ethernet controller for operation   初始化以太网控制器
	nEthClk = SysCtlClockGet();
	EthernetInitExpClk(ETH_BASE, nEthClk);
	// Configure the Ethernet controller for normal operation
	// Enable TX Duplex Mode
	// Enable TX Padding
	EthernetConfigSet(ETH_BASE, (ETH_CFG_TX_DPLXEN | ETH_CFG_TX_PADEN | ETH_CFG_TX_CRCEN));
	HWREG(ETH_BASE + MAC_O_THR) = 0x0;
	EthernetMACAddrSet(ETH_BASE,bMAC);

	//使能以太网
	EthernetEnable(ETH_BASE);


	while(1)  //循环发送
	{
     		while(HWREG(ETH_BASE + MAC_O_TR) & MAC_TR_NEWTX)
		{
		}

 

      // Copy data to FIFO
		pS = (unsigned long*)nSendData;
		for ( i = 0; i < sizeof(nSendData) / 4; i++ )
		HWREG(ETH_BASE + MAC_O_DATA) = *pS++;
//		RIT128x96x4DataDisplay(++count,66,36, 15,5) ;   //计数
	}

}

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"


#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/flash.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"

#include "grlib/grlib.h"
#include "utils/locator.h"
#include "utils/swupdate.h"
#include "utils/ustdlib.h"


#include "drivers/240x400_hx8352.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);


	unsigned ulClockMS = SysCtlClockGet() / (3 * 1000);

	LCD_240x400_HX8352Init();
	LCD_240x400_HX8352BacklightOn();		//打开背光
	unsigned int i,j;

	WriteCommand(0x0022);
	for(i=0;i<400;i++)
	{
		for(j=0;j<240;j++)
		{
			WriteData(0x0000);
		}
	}
	WriteCommand(0x0022);
//	TestPixel();
	LCD_240x480_HX8352PixelDraw(10,10,0xffff);

	while(1)
	{
		;
	}

	return 0;
}

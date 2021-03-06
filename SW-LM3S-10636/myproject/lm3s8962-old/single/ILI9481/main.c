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
#include "grlib/grlib.h"
#include "utils/locator.h"
#include "utils/swupdate.h"
#include "utils/ustdlib.h"

#include "ILI9481.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);


//	unsigned ulClockMS = SysCtlClockGet() / (3 * 1000);


	LCDInit();

    HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTB_BASE + GPIO_O_CR) = 0x80;
    HWREG(GPIO_PORTB_BASE + GPIO_O_AFSEL) &= ~0x80;
    HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTB_BASE + GPIO_O_CR) = 0x00;
    HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = 0;


	GPIOPinWrite(GPIO_PORTC_BASE, 0x40, 0x40);		//打开背光
	unsigned int i,j;

	WriteCommand(0x002C);
	for(i=0;i<40;i++)
	{
		for(j=0;j<320;j++)
		{
			WriteData(j/10);
		}
	}

	for(i=0;i<40;i++)
	{
		for(j=0;j<320;j++)
		{
			WriteData((j/10)<<6);
		}
	}

	for(i=0;i<40;i++)
	{
		for(j=0;j<320;j++)
		{
			WriteData((j/10)<<11);
		}
	}
	for(i=0;i<40;i++)
	{
		for(j=0;j<320;j++)
		{
			WriteData(j);
		}
	}
	for(i=0;i<40;i++)
	{
		for(j=0;j<320;j++)
		{
			WriteData(i+j);
		}
	}
	for(i=0;i<40;i++)
	{
		for(j=0;j<320;j++)
		{
			WriteData(i*320+j);
		}
	}
	for(i=0;i<240;i++)
	{
		for(j=0;j<320;j++)
		{
			WriteData(0xffff);
		}
	}
	while(1)
	{
		;
	}

	return 0;
}

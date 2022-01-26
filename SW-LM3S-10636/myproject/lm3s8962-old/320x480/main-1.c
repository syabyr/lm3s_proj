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
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include <stdlib.h>
#include "320x480_ILI9481.h"
#include "mydelay.h"
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//for sdcard
static FATFS g_sFatFs;
static FIL g_sFileObject;
WORD *bw="Hello world";

int main(void)
{
	tContext sContext;
	tRectangle sRect;
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);


	f_mount(0,&g_sFatFs);
	f_open(&g_sFileObject,"test.txt",FA_CREATE_ALWAYS|FA_WRITE);	
	f_write(&g_sFileObject,"Hello world.",1,bw);
	f_close(&g_sFileObject);
	f_mount(0,NULL);
	//绘制一个点
//	for(i=0;i<100;i++)
//	LCD_240x480_HX8352PixelDraw(i,i,0xffff);
	//绘制一条水平的线段
//	LCD_240x480_HX8352LineDrawH(NULL,0,100,0,0x00ff);
	//绘制一条垂直的线段
//	LCD_240x480_HX8352LineDrawV(NULL,0,0,100,0xff00);

	while(1)
	;


	return 0;
}

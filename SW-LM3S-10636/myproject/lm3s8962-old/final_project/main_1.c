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
#include "utils/ustdlib.h"
#include "ds18b20.h"

//温度部分变量定义
uint16_t temperture_raw;
//整数部分和小数部分
uint8_t int_part,frac_part;
//标志位,为1的时候表示总线上没有传感器
uint8_t is_sensor_not_present=1;
//标志位,为1的时候说明温度为零下
uint8_t is_subzero=0;



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
	//使能系统时钟,使用锁相环,50MHz
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	//使能温度传感器所使用的端口
	SysCtlPeripheralEnable(OW_BUS_BASE);


	LCD_320x480_ILI9481Init();
	LCD_320x480_ILI9481BacklightOn();		//打开背光


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


	GrContextFontSet(&sContext, &g_sFontCm30b);
	GrStringDrawCentered(&sContext, "hello,mybays", -5,
		GrContextDpyWidthGet(&sContext) / 2, 20, 0);
	while(1);
	int a=100;
	char d[10]="hello";
	char *pd=d;


//	usprintf(c,"%d",a);
    GrContextFontSet(&sContext, &g_sFontCm48b);
    GrStringDrawCentered(&sContext,pd, -1,
                         GrContextDpyWidthGet(&sContext) / 2,
                         ((GrContextDpyHeightGet(&sContext) - 24) / 2) + 24,
                         ClrDarkRed);

	_delay_ms(3000);
//	usprintf(d,"%d",a);

	while(1)
	{
		if(ow_reset())
		{
			GrContextFontSet(&sContext, &g_sFontCm48b);
			GrStringDrawCentered(&sContext,"Bad Sensor", -1,
				GrContextDpyWidthGet(&sContext) / 2,
				((GrContextDpyHeightGet(&sContext) - 24) / 2) + 24,
				ClrDarkRed);
		}
		else
		{
			GrContextFontSet(&sContext, &g_sFontCm48b);
			GrStringDrawCentered(&sContext,"Sensor OK", -1,
				GrContextDpyWidthGet(&sContext) / 2,
				((GrContextDpyHeightGet(&sContext) - 24) / 2) + 24,
				ClrDarkRed);
		}
		_delay_ms(1000);

	}
/*
	tRectangle myRect;
//	tContext myContext;
//	GrContextInit(&sContext,&g_sLCD_320x480_ILI9481);
	myRect.sXMin=0;
	myRect.sYMin=0;
	myRect.sXMax=GrContextDpyWidthGet(&sContext)-1;
	myRect.sYMax=320 - 1;
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectFill(&sContext, &myRect);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &myRect);
	GrContextForegroundSet(&sContext, ClrRed);
*/
/*
	tContext myContext;
	GrContextInit(&myContext,&g_sLCD_320x480_ILI9481);
	
	GrContextForegroundSet(&myContext, ClrGreen);
	GrCircleFill(&myContext, 160,160,80);
	GrContextForegroundSet(&sContext, ClrRed);
	GrCircleFill(&sContext, 280,160,81);
*/
	/*
	unsigned long i,j;
	GrLineDraw(&sContext,1,20,470,310);
	for(j=0;j<300;j++)
	for(i=0;i<300;i++)
	{
		LCD_320x480_ILI9481PixelDraw(j,i,0xff);
		_delay_us(10);
	}
	*/
	GrLineDraw(&sContext,100,100,100,300);
	GrLineDraw(&sContext,100,100,300,100);
	GrLineDraw(&sContext,100,100,200,300);

	//绘制一个点
//	for(i=0;i<100;i++)
//	LCD_240x480_HX8352PixelDraw(i,i,0xffff);
	//绘制一条水平的线段
//	LCD_240x480_HX8352LineDrawH(NULL,0,100,0,0x00ff);
	//绘制一条垂直的线段
//	LCD_240x480_HX8352LineDrawV(NULL,0,0,100,0xff00);

	while(1)
	;

	while(1)
	{
		;
	}

	return 0;
}

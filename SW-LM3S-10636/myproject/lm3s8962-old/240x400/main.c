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
#include "240x400_hx8352.h"
#include "mydelay.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


int main(void)
{
	tContext sContext;
	tRectangle sRect;
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	LCD_240x400_HX8352Init();
	LCD_240x400_HX8352BacklightOn();		//打开背光
	char pcBuffer[32];

	GrContextInit(&sContext,&g_sLCD_240x480_HX8352);
	sRect.sXMin=0;
	sRect.sYMin=0;
	sRect.sXMax=GrContextDpyWidthGet(&sContext) - 1;
	sRect.sYMax=40;
	GrContextForegroundSet(&sContext, ClrDarkBlue);
	GrRectFill(&sContext, &sRect);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &sRect);
	GrContextFontSet(&sContext, &g_sFontCm30b);
	GrStringDrawCentered(&sContext, "hello,mybays", -5,
		GrContextDpyWidthGet(&sContext) / 2, 20, 0);

    GrContextFontSet(&sContext, &g_sFontCm48b);
    GrStringDrawCentered(&sContext, "Hello World!", -1,
                         GrContextDpyWidthGet(&sContext) / 2,
                         ((GrContextDpyHeightGet(&sContext) - 24) / 2) + 24,
                         0);





	//绘制一个点
//	for(i=0;i<100;i++)
//	LCD_240x480_HX8352PixelDraw(i,i,0xffff);
	//绘制一条水平的线段
//	LCD_240x480_HX8352LineDrawH(NULL,0,100,0,0x00ff);
	//绘制一条垂直的线段
//	LCD_240x480_HX8352LineDrawV(NULL,0,0,100,0xff00);

	while(1)
	;
/*
	WriteRegister(0x0016,1<<5|1<<6|1<<7);
	WriteRegister(0x0003,0);
	WriteRegister(0x0002,0>>8);
	//起始的Y
	WriteRegister(0x0007,0);
	WriteRegister(0x0006,0>>8);
	WriteCommand(0x0022);
	for(i=0;i<240*400-1;i++)
	{
		WriteData(0x00ffff);
	}
	while(1);
*/
/*
	//测试方向
	//左右
	//起始的X
	WriteRegister(0x0002,0>>8);
	WriteRegister(0x0003,0);
	//结束的X
	WriteRegister(0x0004,0x0000);
	WriteRegister(0x0005,0x00EF);

	//起始的Y
	WriteRegister(0x0006,0>>8);
	WriteRegister(0x0007,0);
	//结束的Y
	WriteRegister(0x0008,0x0001);
	WriteRegister(0x0009,0x008F);

	//写数据命令
	WriteCommand(0x0022);
	for(i=0;i<120;i++)
		WriteData(0x00ff);
	for(i=0;i<120;i++)
		WriteData(0x0ff0);

//上下
	WriteRegister(0x0016,1<<5);
	//起始的X
	WriteRegister(0x0002,0>>8);
	WriteRegister(0x0003,0);
	//结束的X
	WriteRegister(0x0004,0x0001); // Column Address end Register, High 8 bit 
	WriteRegister(0x0005,0x008F); // Column Address end Start Register, Low 8 bit 
	//起始的Y
	WriteRegister(0x0006,239>>8);
	WriteRegister(0x0007,239);
	//结束的Y
	WriteRegister(0x0008,0x0000); // Row Address end Register, High 8 bit 
	WriteRegister(0x0009,0x00EF); // Row Address end Register, Low 8bit 

	WriteCommand(0x0022);


	for(i=0;i<120;i++)
		WriteData(0x00ff);
	for(i=0;i<120;i++)
		WriteData(0x0ff0);


//右左

	WriteRegister(0x0016,1<<6);
	//起始的X
	WriteRegister(0x0002,0>>8);
	WriteRegister(0x0003,0);
	WriteRegister(0x0004,0x0000); // Row Address end Register, High 8 bit 
	WriteRegister(0x0005,0x00EF); // Row Address end Register, Low 8bit 
	//起始的Y

	WriteRegister(0x0006,239>>8);
	WriteRegister(0x0007,239);
	WriteRegister(0x0008,0x0001); // Column Address end Register, High 8 bit 
	WriteRegister(0x0009,0x008F); // Column Address end Start Register, Low 8 bit 

	WriteCommand(0x0022);
	for(i=0;i<120;i++)
		WriteData(0x00ff);
	for(i=0;i<120;i++)
		WriteData(0x0ff0);


//下上
	WriteRegister(0x0016,1<<5|1<<7);
	//起始的X
	WriteRegister(0x0002,(399-239)>>8);
	WriteRegister(0x0003,399-239);
	WriteRegister(0x0004,0x0001); // Row Address end Register, High 8 bit 
	WriteRegister(0x0005,0x008F); // Row Address end Register, Low 8bit 
	//起始的Y
	WriteRegister(0x0006,0>>8);
	WriteRegister(0x0007,0);
	WriteRegister(0x0008,0x0000); // Column Address end Register, High 8 bit 
	WriteRegister(0x0009,0x00EF); // Column Address end Start Register, Low 8 bit 

	WriteCommand(0x0022);
	for(i=0;i<120;i++)
		WriteData(0x00ff);
	for(i=0;i<120;i++)
		WriteData(0x0ff0);
*/
/*
	TestDirection();


	//test PixelDraw
	WriteCommand(0x0022);
	for(i=0;i<240;i++)
	LCD_240x480_HX8352PixelDraw(i,i,0x001f);
*/

	while(1)
	{
		;
	}

	return 0;
}

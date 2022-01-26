/***************************************************************************
320x240 TTF crystal driver.
Manufature:Himax
Control:HX8352-A
Author:mybays
Email:mybays@qq.com
Cortex-M3的液晶驱动
Resulation:320x240


***************************************************************************/

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "grlib/grlib.h"
#include "320x480_9B95.h"

#include "mydelay.h"

//定义旋转模式
//#define PORTRAIT
#define LANDSCAPE
//#define PORTRAIT_FLIP
//#define LANDSCAPE_FLIP
#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) && \
	! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define PORTRAIT
#endif

//定义颜色转换
//*****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((c) & 0x00ff0000) >> 19) |               \
                                 ((((c) & 0x0000ff00) >> 5) & 0x000007e0) | \
                                 ((((c) & 0x000000ff) << 8) & 0x0000f800))

//*****************************************************************************

//总线数据写入,将16bit的数据挂载数据总线上,然后使能写信号
void WriteData(unsigned short usData)
{
	//将数据写入数据总线,低8位写在PORTA上,高8位写在PORTB上.
	HWREG(LCD_DATAH_BASE+GPIO_O_DATA+(0xff<<2))=usData>>8;
	HWREG(LCD_DATAL_BASE+GPIO_O_DATA+(0xff<<2))=usData;
	//使能读信号,将信号线拉低后再拉高
	LCD_WRITE_ON;
	LCD_WRITE_OFF;
}

//总线数据读取,将总线设置为输入,使能读信号,然后读入数据,最后将总线设置为输出.
unsigned short ReadData(void)
{
	unsigned short usData=0;
	//将总线设置为输入
	HWREG(LCD_DATAH_BASE+GPIO_O_DIR)=0x00;
	HWREG(LCD_DATAL_BASE+GPIO_O_DIR)=0x00;
	//拉低读信号
	LCD_READ_ON;
	//读取数据
	usData=HWREG(LCD_DATAL_BASE+GPIO_O_DATA+(0xff<<2));
	usData|=HWREG(LCD_DATAH_BASE+GPIO_O_DATA+(0xff<<2))<<8;
	//拉高读信号
	LCD_READ_OFF;
	//将总线设置为输出端口
	HWREG(LCD_DATAL_BASE+GPIO_O_DIR)=0xff;
	HWREG(LCD_DATAH_BASE+GPIO_O_DIR)=0xff;

	return (usData);
}

//写命令,8bit的命令挂载数据总线的低8位,使能寄存器选择信号,再使能写数据信号.
void WriteCommand(unsigned char ucData)
{
	//将数据高8bit清零,低8bit载入数据
	HWREG(LCD_DATAL_BASE+GPIO_O_DATA+(0xff<<2))=ucData;
	//拉低寄存器选择信号,然后使能写信号
	LCD_RS_ON;
	LCD_WRITE_ON;
	LCD_WRITE_OFF;
	LCD_RS_OFF;
}

//读寄存器的值,写命令到系统,然后返回寄存器的值
unsigned short ReadRegister(unsigned char ucIndex)
{
	WriteCommand(ucIndex);
	return (ReadData());
}

//写寄存器的地址,然后写入要写入到这个寄存器的数据,ucIndex为寄存器的地址
void WriteRegister(unsigned char ucIndex,unsigned short usValue)
{
	WriteCommand(ucIndex);
	WriteData(usValue);
}



//初始化液晶
void LCD_320x480_ILI9481Init(void)
{
	//复位部分
	_delay_ms(10);

	GPIOPinWrite(LCD_RST_BASE,LCD_RST_PIN,0x00);	//拉低复位信号
	_delay_ms(10);
	GPIOPinWrite(LCD_RST_BASE,LCD_RST_PIN,LCD_RST_PIN);	//拉高复位信号

	_delay_ms(60);

	WriteCommand(0x11);
	_delay_ms(100);
	WriteCommand(0x13);
	WriteCommand(0xD0);
	WriteData(0x07);
	WriteData(0x40);
	WriteData(0x1C);

	WriteCommand(0xD1);	//VCOM Control
	WriteData(0x00);
	WriteData(0x18);
	WriteData(0x1D);

	WriteCommand(0xD2);	//PowerSetting for Normal Mode
	WriteData(0x01);
	WriteData(0x11);

	WriteCommand(0xC0);	//Panel Driving Setting
	WriteData(0x00);
	WriteData(0x3B);
	WriteData(0x00);
	WriteData(0x02);
	WriteData(0x11);

	WriteCommand(0xC1);	//Display Timing Setting for Normal Mode
	WriteData(0x10);
	WriteData(0x0B);
	WriteData(0x88);

	WriteCommand(0xC5);	//Frame rate and Inversion Control
	WriteData(0x01);

	WriteCommand(0xC8);	//Gamma Setting
	WriteData(0x00);
	WriteData(0x30);
	WriteData(0x36);
	WriteData(0x45);
	WriteData(0x04);
	WriteData(0x16);
	WriteData(0x37);
	WriteData(0x75);
	WriteData(0x77);
	WriteData(0x54);
	WriteData(0x0F);
	WriteData(0x00);

	WriteCommand(0xE4);	//No this command
	WriteData(0xA0);

	WriteCommand(0xF0);	//
	WriteData(0x01);

	WriteCommand(0xF3);
	WriteData(0x40);
	WriteData(0x0A);

	WriteCommand(0xF7);
	WriteData(0x80);

	#ifdef PORTRAIT
	WriteCommand(0x36);
	WriteData(0x02);

	WriteCommand(0x2A);	//set column address
	WriteData(0>>8);
	WriteData(0);
	WriteData(319>>8);
	WriteData(319);

	WriteCommand(0x2B);	//set page address
	WriteData(0>>8);
	WriteData(0);
	WriteData(479>>8);
	WriteData(479);
	#endif
	#ifdef LANDSCAPE
	WriteCommand(0x36);
	WriteData(1<<0|1<<1|1<<5);

	WriteCommand(0x2A);	//set column address
	WriteData(0>>8);
	WriteData(0);
	WriteData(479>>8);
	WriteData(479);

	WriteCommand(0x2B);	//set page address
	WriteData(0>>8);
	WriteData(0);
	WriteData(319>>8);
	WriteData(319);
	#endif
	#ifdef PORTRAIT_FLIP
	WriteCommand(0x36);
	WriteData(0x01);

	WriteCommand(0x2A);	//set column address
	WriteData(0>>8);
	WriteData(0);
	WriteData(319>>8);
	WriteData(319);

	WriteCommand(0x2B);	//set page address
	WriteData(0>>8);
	WriteData(0);
	WriteData(479>>8);
	WriteData(479);
	#endif
	#ifdef LANDSCAPE_FLIP
	WriteCommand(0x36);
	WriteData(1<<5|1<<6);

	WriteCommand(0x2A);	//set column address
	WriteData(0>>8);
	WriteData(0);
	WriteData(479>>8);
	WriteData(479);

	WriteCommand(0x2B);	//set page address
	WriteData(0>>8);
	WriteData(0);
	WriteData(319>>8);
	WriteData(319);
	#endif


	WriteCommand(0x3A);	//set pixel format
	WriteData(0x55);



//	WriteCommand(0x21);
	_delay_ms(100);

	WriteCommand(0x29);	//set display on
	WriteCommand(0x2C);	//write memory start

	unsigned long i;
	for(i=0;i<(320*480);i++)
	{
		WriteData(0x0000);
	}

/*
	for(i=0;i<(320*240);i++)
	{
		WriteData(0xff00);
	}
*/
}
//获取驱动器的ID
unsigned short LCD_320x480_ILI9481ControllerIdGet(void)
{
	return (ReadRegister(0x00));
}
//画一个像素点到液晶屏
void LCD_320x480_ILI9481PixelDraw(unsigned long lX,unsigned long lY,
								unsigned long ulValue)
{


	WriteCommand(0x2A);
	WriteData(lX>>8);
	WriteData(lX);
	WriteData(lX>>8);
	WriteData(lX);

	WriteCommand(0x2B);
	WriteData(lY>>8);
	WriteData(lY);
	WriteData(lY>>8);
	WriteData(lY);

	//画像素点
	WriteCommand(0x002C);
	WriteData(ulValue);
}

//绘制一条水平的线段
void LCD_320x480_ILI9481LineDrawH(void *pvDisplayData,long lX1,long lX2,long lY,unsigned long ulValue)
{

	WriteCommand(0x2A);	//set column address
	WriteData(lX1>>8);
	WriteData(lX1);
	WriteData(lX2>>8);
	WriteData(lX2);

	WriteCommand(0x2B);	//set page address
	WriteData(lY>>8);
	WriteData(lY);
	WriteData(lY>>8);
	WriteData(lY);


	WriteCommand(0x002C);
	while(lX1++<=lX2)
		WriteData(ulValue);
}


//画一条垂直的线段
void LCD_320x480_ILI9481LineDrawV(void *pvDisplayData,long lX,long lY1,long lY2,unsigned long ulValue)
{

	WriteCommand(0x2A);	//set column address
	WriteData(lX>>8);
	WriteData(lX);
	WriteData(lX>>8);
	WriteData(lX);

	WriteCommand(0x2B);	//set page address
	WriteData(lY1>>8);
	WriteData(lY1);
	WriteData(lY2>>8);
	WriteData(lY2);

	WriteCommand(0x002C);
	while(lY1++<=lY2)
		WriteData(ulValue);
}

void LCD_320x480_ILI9481PixelDrawMultiple(void *pvDisplayData,long lX,long lY,long lX0,long lCount,
									long lBPP,
									const unsigned char *pucData,
									const unsigned char *pucPalette)
{
	unsigned long ulByte;
#ifdef PORTRAIT
	WriteCommand(0x2A);	//set column address
	WriteData(lX>>8);
	WriteData(lX);
	WriteData(319>>8);
	WriteData(319);

	WriteCommand(0x2B);	//set page address
	WriteData(lY>>8);
	WriteData(lY);
	WriteData(479>>8);
	WriteData(479);
#endif
#ifdef LANDSCAPE
	WriteCommand(0x36);
	WriteData(1<<3|1<<5|1<<7);
	WriteCommand(0x2A);
	WriteData(lX>>8);
	WriteData(lX);
	WriteData(479>>8);
	WriteData(479);

	WriteCommand(0x2B);
	WriteData(lY>>8);
	WriteData(lY);
	WriteData(319>>8);
	WriteData(319);

#endif
#ifdef PORTRAIT_FLIP
	WriteCommand(0x36);
	WriteData(1<<3|1<<6|1<<7);
	WriteCommand(0x2A);
	WriteData(lX>>8);
	WriteData(lX);
	WriteData(319>>8);
	WriteData(319);

	WriteCommand(0x2B);
	WriteData(lY>>8);
	WriteData(lY);
	WriteData(479>>8);
	WriteData(479);

#endif
#ifdef LANDSCAPE_FLIP
	WriteCommand(0x36);
	WriteData(1<<3|1<<5|1<<6);
	WriteCommand(0x2A);
	WriteData(lX>>8);
	WriteData(lX);
	WriteData(319>>8);
	WriteData(319);

	WriteCommand(0x2B);
	WriteData(lY>>8);
	WriteData(lY);
	WriteData(479>>8);
	WriteData(479);

#endif

	WriteCommand(0x2C);
	switch(lBPP)
	{
		case 1:
		{
			while(lCount)
			{
				ulByte=*pucData++;
				for(;(lX0<8)&&lCount;lX0++,lCount--)
				{
					WriteData(((unsigned long *)pucPalette)[(ulByte>>(7-lX0))&1]);
				}
			lX0=0;
			}
			break;
		}
		case 4:
		{
			switch (lX0 & 1)
			{
				case 0:
					while(lCount)
					{
						ulByte=(*pucData>>4)*3;
						ulByte=(*(unsigned long *)(pucPalette+ulByte) &0x00ffffff);
						WriteData(DPYCOLORTRANSLATE(ulByte));
						lCount--;
						if(lCount)
						{
							case 1:
								ulByte=(*pucData++ &15)*3;
								ulByte=(*(unsigned long *)(pucPalette+ulByte)&0x00ffffff);
								WriteData(DPYCOLORTRANSLATE(ulByte));
								lCount--;
						}
					}
			}
			break;
		}
		case 8:
		{
			while(lCount--)
			{
				ulByte=*pucData++ *3;
				ulByte=*(unsigned long *)(pucPalette+ulByte)&0x00ffffff;
				WriteData(DPYCOLORTRANSLATE(ulByte));
			}
			break;
		}
	}
	WriteCommand(0x36);	//set address mode
	WriteData(0x02);
}



void LCD_320x480_ILI9481RectFill(void *pvDisplayData,const tRectangle *pRect,unsigned long ulValue)
{
	long lCount;

	WriteCommand(0x2A);	//set column address
	WriteData((pRect->sXMin)>>8);
	WriteData(pRect->sXMin);
	WriteData((pRect->sXMax)>>8);
	WriteData(pRect->sXMax);

	WriteCommand(0x2B);	//set page address
	WriteData((pRect->sYMin)>>8);
	WriteData(pRect->sYMin);
	WriteData((pRect->sYMax)>>8);
	WriteData(pRect->sYMax);

	WriteCommand(0x002C);
	for(lCount=((pRect->sXMax-pRect->sXMin+1)*(pRect->sYMax-pRect->sYMin+1));
		lCount>=0;lCount--)
	WriteData(ulValue);
}

static unsigned long LCD_320x480_ILI9481ColorTranslate(void *pvDisplayData,unsigned long ulValue)
{
	return(DPYCOLORTRANSLATE(ulValue));
}


//空函数
char LCD_320x480_ILI9481Flush(void *pvDisplayData)
{
	return 0;
}


const tDisplay g_sLCD_320x480_ILI9481=
{
	sizeof(tDisplay),
	0,
#if defined(PORTRAIT) || defined (PORTRAIT_FLIP)
	320,
	480,
#else
	480,
	320,
#endif
	LCD_320x480_ILI9481PixelDraw,
	LCD_320x480_ILI9481PixelDrawMultiple,
	LCD_320x480_ILI9481LineDrawH,
	LCD_320x480_ILI9481LineDrawV,
	LCD_320x480_ILI9481RectFill,
	LCD_320x480_ILI9481ColorTranslate,
	LCD_320x480_ILI9481Flush
};




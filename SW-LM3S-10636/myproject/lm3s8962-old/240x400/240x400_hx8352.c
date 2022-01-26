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
#include "240x400_hx8352.h"

#include "mydelay.h"

//定义旋转模式
//#define PORTRAIT
//#define PORTRAIT_FLIP
#define LANDSCAPE
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
	GPIOPinWrite(LCD_DATAE_BASE,0x80,(usData>>8)&0x80);
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
	HWREG(LCD_DATAE_BASE+GPIO_O_DIR)&=0x7F;
	//拉低读信号
	LCD_READ_ON;
	//读取数据
	usData=HWREG(LCD_DATAL_BASE+GPIO_O_DATA+(0xff<<2));
	usData|=HWREG(LCD_DATAH_BASE+GPIO_O_DATA+(0xff<<2))<<8;
	usData|=(HWREG(LCD_DATAE_BASE+GPIO_O_DATA+(0xff<<2))<<8)&0x80;
	//拉高读信号
	LCD_READ_OFF;
	//将总线设置为输出端口
	HWREG(LCD_DATAL_BASE+GPIO_O_DIR)=0xff;
	HWREG(LCD_DATAH_BASE+GPIO_O_DIR)=0xff;
	HWREG(LCD_DATAE_BASE+GPIO_O_DIR)|=0x80;

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

//打开背光
void LCD_240x400_HX8352BacklightOn(void)
{
	HWREG(LCD_BL_BASE+GPIO_O_DATA+(LCD_BL_PIN<<2))=LCD_BL_PIN;
}

//关闭背光
void LCD_240x400_HX8352BacklightOff(void)
{
	HWREG(LCD_BL_BASE+GPIO_O_DATA+(LCD_BL_PIN<<2))=0;
}


//初始化液晶
void LCD_240x400_HX8352Init(void)
{
	//使能要连接的端口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);


	//设置这些管脚为输出,以及他们的默认值
	//数据低8bit
	GPIOPinTypeGPIOOutput(LCD_DATAL_BASE,0xff);	//0B1111 1111
	GPIOPinWrite(LCD_DATAL_BASE,0xff,0x00);
	//数据高8bit
	GPIOPinTypeGPIOOutput(LCD_DATAH_BASE,0x7f);	//0B0111 1111
	GPIOPinWrite(GPIO_PORTB_BASE,0x7f,0x00);
	//PC6,背光,默认背光关闭,等会要打开
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,0xE0);	//0B1110 0000
	GPIOPinWrite(GPIO_PORTC_BASE,0xE0,0x20);		//0B0100 0000
	//RS,WR,RD信号,PF0,PF1,PF2,默认都为高电平
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,0x07);	//0B0000 0111
	GPIOPinWrite(GPIO_PORTF_BASE,0x07,0x07);
	//RESET复位信号,PG0,默认为高电平
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE,0x01);
	GPIOPinWrite(GPIO_PORTG_BASE,0x01,0x01);
	SELECT_LCD;


	//复位部分
	_delay_ms(10);

	GPIOPinWrite(GPIO_PORTG_BASE,0x01,0x00);	//拉低复位信号
	_delay_ms(10);
	GPIOPinWrite(GPIO_PORTG_BASE,0x01,0x01);	//拉高复位信号

	_delay_ms(60);

	//初始化命令部分
	WriteRegister(0x0083,0x0002);	//test mode=1,内部寄存器能够接受新的设置
	WriteRegister(0x0085,0x0002); // VDC_SEL=111.VDDD的电压设置
	//Source gamma resistor setting register
	WriteRegister(0x008B,0x0000); // STBA[15:8]=0x00
	WriteRegister(0x008C,0x0033); // STBA[7]=1, STBA[5:4]=01, STBA[1:0]=11
	WriteRegister(0x0091,0x0001); // DCDC_and_Display CLK SYNC=1,

	//结束测试模式
	WriteRegister(0x0083,0x0000); // TESTM=0
	// Gamma Setting
	WriteRegister(0x003E,0x00f0); //γ center adjustment register for positive 
	WriteRegister(0x003F,0x0007); //γ center adjustment register for negative 
	WriteRegister(0x0040,0x0000); //γ macro adjustment register for positive 
	WriteRegister(0x0041,0x0043); //γ macro adjustment register for positive 
	WriteRegister(0x0042,0x0016); //γ macro adjustment register for positive 
	WriteRegister(0x0043,0x0016); //γ macro adjustment register for negative 
	WriteRegister(0x0044,0x0043); //γ macro adjustment register for negative 
	WriteRegister(0x0045,0x0077); //γ macro adjustment register for negative 
	WriteRegister(0x0046,0x0000); //γ offset adjustment register for positive 
	WriteRegister(0x0047,0x001e); //γ offset adjustment register for positive 
	WriteRegister(0x0048,0x000f); //γ offset adjustment register for negative 
	WriteRegister(0x0049,0x0000); //γ offset adjustment register for negative 

	// Power Supply Setting
	WriteRegister(0x0017,0x0011); // RADJ=1001, OSC_EN=1    0071,设置内部振荡器的频率
	WriteRegister(0x0023,0x0001); //TEON=1,Tearing-Effect mode
	WriteRegister(0x002B,0x0053); // N_DCDC=0x7D,Normal mode,普通模式下的分频
	_delay_ms(100);
	WriteRegister(0x001B,0x0014); // BT=0100, AP=100
	WriteRegister(0x001A,0x0030); // VC3=000, VC1=101 
	WriteRegister(0x001C,0x000c); // PON=1, VRH=0110 //0c
	WriteRegister(0x001D,0x000b); //BGP=1111 
	WriteRegister(0x001F,0x0050); // VCM=1010101          0053
	_delay_ms(20);
	WriteRegister(0x0019,0x000A); // GASENB=0, PON=0, DK=1, XDK=0, VLCD_TRI=0, STB=0 
	WriteRegister(0x0019,0x001A); // GASENB=0, PON=0, DK=0, XDK=0, VLCD_TRI=0, STB=0 
	_delay_ms(40);
	WriteRegister(0x0019,0x0012); // GASENB=0, DK=0, XDK=1, VLCD_TRI=0, STB=0, 
	//VLCD=2XVCI by 2 CAPs 
	_delay_ms(80);
	WriteRegister(0x001E,0x002b); // VCOMG=1, VDV=10000     002b
	// DGC Function Enable
	//WriteRegister(0x5A,0x01);
	//GC_PA_REG(0x5C);
	//Window set
	WriteRegister(0x0002,0x0000); // Column Address Start Register, High 8 bit 
	WriteRegister(0x0003,0x0000); // Column Address Start Register, Low 8bit 
	WriteRegister(0x0004,0x0000); // Column Address end Register, High 8 bit 
	WriteRegister(0x0005,0x00EF); // Column Address end Start Register, Low 8 bit 
	WriteRegister(0x0006,0x0000); // Row Address Start Register, High 8 bit 
	WriteRegister(0x0007,0x0000); // Row Address Start Register, Low 8bit 
	WriteRegister(0x0008,0x0001); // Row Address end Register, High 8 bit 
	WriteRegister(0x0009,0x008F); // Row Address end Register, Low 8bit 
	// Display ON Setting
	WriteRegister(0x003C,0x00FF); // N_SAP=1100 000
	WriteRegister(0x003D,0x000e); // I_SAP =1100 0000
	WriteRegister(0x0034,0x0038); // EQS=1000 0111
	WriteRegister(0x0035,0x0038); // EQP=0011 1000
	WriteRegister(0x0024,0x0038); // GON=1, DTE=1, D=10
	_delay_ms(40);
	WriteRegister(0x0024,0x003C); // GON=1, DTE=1, D=11
	WriteRegister(0x0016,0x0000); // BGR=1    1c
	WriteRegister(0x0001,0x0006); // INVON=0, NORNO=1
	WriteRegister(0x0055,0x0002);
/*
	WriteCommand(0x0022);
	unsigned long i;
	for(i=0;i<(240*400);i++)
	{
		WriteData(0xffff);
	}
*/
}
//获取驱动器的ID
unsigned short LCD_240x400_HX8352ControllerIdGet(void)
{
	return (ReadRegister(0x00));
}
//画一个像素点到液晶屏
void LCD_240x480_HX8352PixelDraw(unsigned long lX,unsigned long lY,
								unsigned long ulValue)
{
	//不需要任何模式
	WriteRegister(0x0016,0x00);
	//结束的X
	WriteRegister(0x0004,239>>8);
	WriteRegister(0x0005,239);
	//结束的Y
	WriteRegister(0x0008,399>>8);
	WriteRegister(0x0009,399);
#ifdef PORTRAIT
	//起始的X
	WriteRegister(0x0002,lX>>8);
	WriteRegister(0x0003,lX);

	//起始的Y
	WriteRegister(0x0006,lY>>8);
	WriteRegister(0x0007,lY);

#endif
	
#ifdef LANDSCAPE
	//起始的X
	WriteRegister(0x0002,(lY>>8));
	WriteRegister(0x0003,lY);

	//起始的Y
	WriteRegister(0x0006,(399-lX)>>8);
	WriteRegister(0x0007,399-lX);

#endif

#ifdef PORTRAIT_FLIP

	//起始的X
	WriteRegister(0x0002,(239-lX)>>8);
	WriteRegister(0x0003,239-lX);

	//起始的Y
	WriteRegister(0x0006,(399-lY)>>8);
	WriteRegister(0x0007,399-lY);



#endif

#ifdef LANDSCAPE_FLIP
	//起始的X
	WriteRegister(0x0002,(239-lY)>>8);
	WriteRegister(0x0003,239-lY);

	//起始的Y
	WriteRegister(0x0006,lX>>8);
	WriteRegister(0x0007,lX);

#endif
	//画像素点
	WriteCommand(0x0022);
	WriteData(ulValue);
}

//绘制一条水平的线段
void LCD_240x480_HX8352LineDrawH(void *pvDisplayData,long lX1,long lX2,long lY,unsigned long ulValue)
{
#ifdef PORTRAIT
	//普通模式,从左到右,从上到下
	WriteRegister(0x0016,0x00);
#endif

#ifdef LANDSCAPE
	//从下到上,从左到右
	WriteRegister(0x0016,1<<5|1<<7);
#endif

#ifdef PORTRAIT_FLIP
	//从右到左,从下到上
	WriteRegister(0x0016,1<<7|1<<6);
#endif

#ifdef LANDSCAPE_FLIP
	//从下到上,从右到左
	WriteRegister(0x0016,1<<5|1<<6);
#endif

	//起始的X
	WriteRegister(0x0002,(lX1>>8));
	WriteRegister(0x0003,lX1);
	//结束的X
	WriteRegister(0x0004,(lX2>>8));
	WriteRegister(0x0005,lX2);
	//起始的Y
	WriteRegister(0x0006,lY>>8);
	WriteRegister(0x0007,lY);
	//结束的Y
	WriteRegister(0x0008,lY>>8);
	WriteRegister(0x0009,lY);

	WriteCommand(0x0022);
	while(lX1++<=lX2)
		WriteData(ulValue);
}


//画一条垂直的线段
void LCD_240x480_HX8352LineDrawV(void *pvDisplayData,long lX,long lY1,long lY2,unsigned long ulValue)
{
#ifdef PORTRAIT
	WriteRegister(0x0016,1<<5);
#endif

#ifdef LANDSCAPE
	WriteRegister(0x0016,1<<7);
#endif

#ifdef PORTRAIT_FLIP
	WriteRegister(0x0016,1<<5|1<<6|1<<7);
#endif

#ifdef LANDSCAPE_FLIP
	WriteRegister(0x0016,1<<6);
#endif
	//起始的X
	WriteRegister(0x0002,(lY1>>8));
	WriteRegister(0x0003,lY1);
	//结束的X
	WriteRegister(0x0004,(lY2>>8));
	WriteRegister(0x0005,lY2);
	//起始的Y
	WriteRegister(0x0006,lX>>8);
	WriteRegister(0x0007,lX);
	//结束的Y
	WriteRegister(0x0008,lX>>8);
	WriteRegister(0x0009,lX);

	WriteCommand(0x0022);
	while(lY1++<=lY2)
		WriteData(ulValue);
}
//测试方向
void TestDirection(void)
{
	WriteRegister(0x0016,1<<4|1<<5);
	//起始的X
	WriteRegister(0x0003,120);
	WriteRegister(0x0002,120>>8);
	//起始的Y
	WriteRegister(0x0007,120);
	WriteRegister(0x0006,120>>8);
	//画像素点
	WriteCommand(0x0022);
	unsigned int i;
	for(i=0;i<40;i++)
	WriteData(0x001f);
	for(i=0;i<40;i++)
	WriteData(0x07e0);
	for(i=0;i<40;i++)
	WriteData(0xf800);
}

//测试像素
void TestPixel(void)
{
	//起始列
	WriteRegister(0x0003,10);
	WriteRegister(0x0005,11);
	//起始行
	WriteRegister(0x0007,10);
	WriteRegister(0x0009,11);
	WriteCommand(0x0022);
	WriteData(0xffff);

	WriteRegister(0x0003,20);
	WriteRegister(0x0005,21);
	//起始行
	WriteRegister(0x0007,20);
	WriteRegister(0x0009,21);
	WriteCommand(0x0022);
	WriteData(0xffff);

	WriteRegister(0x0003,30);
	WriteRegister(0x0005,31);
	//起始行
	WriteRegister(0x0007,30);
	WriteRegister(0x0009,31);
	WriteCommand(0x0022);
	WriteData(0xffff);

	WriteRegister(0x0003,10);
	WriteRegister(0x0005,10);
	//起始行
	WriteRegister(0x0007,30);
	WriteRegister(0x0009,30);
	WriteCommand(0x0022);
	WriteData(0xffff);
}



void TestRGB(void)
{
	int i,j;
	for (i=0;i<100;i++)
	{
		for(j=0;j<240;j++)
		//WriteData(0x001f^0xffff);
		WriteData(0x001f);
	};
	WriteCommand(0x0022);
	for (i=0;i<100;i++)
	{
		for(j=0;j<240;j++)
		//WriteData(0x07e0^0xffff);
		WriteData(0x07e0);
	};
	for (i=0;i<100;i++)
	{
		for(j=0;j<240;j++)
		//WriteData(0x0f800^0xffff);
		WriteData(0x0f800);
	};

}

void LCD_240x480_HX8352PixelDrawMultiple(void *pvDisplayData,long lX,long lY,long lX0,long lCount,
									long lBPP,
									const unsigned char *pucData,
									const unsigned char *pucPalette)
{
	unsigned long ulByte;
#ifdef PORTRAIT
	WriteRegister(0x0016,0x00);

	WriteRegister(0x0004,239>>8);
	WriteRegister(0x0005,239);
	WriteRegister(0x0008,399>>8);
	WriteRegister(0x0009,399);
#endif
#ifdef LANDSCAPE
	WriteRegister(0x0016,1<<7|1<<5);

	WriteRegister(0x0008,239>>8);
	WriteRegister(0x0009,239);
	WriteRegister(0x0004,399>>8);
	WriteRegister(0x0005,399);
#endif
#ifdef PORTRAIT_FLIP
	WriteRegister(0x0016,1<<6|1<<7);

	WriteRegister(0x0004,239>>8);
	WriteRegister(0x0005,239);
	WriteRegister(0x0008,399>>8);
	WriteRegister(0x0009,399);
#endif
#ifdef LANDSCAPE_FLIP
	WriteRegister(0x0016,1<<5|1<<6);

	WriteRegister(0x0008,239>>8);
	WriteRegister(0x0009,239);
	WriteRegister(0x0004,399>>8);
	WriteRegister(0x0005,399);
#endif

	WriteRegister(0x0002,lX>>8);
	WriteRegister(0x0003,lX);
	WriteRegister(0x0006,lY>>8);
	WriteRegister(0x0007,lY);

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
}



void LCD_240x480_HX8352RectFill(void *pvDisplayData,const tRectangle *pRect,unsigned long ulValue)
{
	long lCount;
#ifdef PORTRAIT
	WriteRegister(0x0016,0x00);
#endif
#ifdef LANDSCAPE
	WriteRegister(0x0016,1<<7|1<<5);
#endif
#ifdef PORTRAIT_FLIP
	WriteRegister(0x0016,1<<6|1<<7);
#endif
#ifdef LANDSCAPE_FLIP
	WriteRegister(0x0016,1<<5|1<<6);
#endif
	//起始的X
	WriteRegister(0x0002,(pRect->sXMin)>>8);
	WriteRegister(0x0003,pRect->sXMin);
	//结束的X
	WriteRegister(0x0004,(pRect->sXMax)>>8);
	WriteRegister(0x0005,pRect->sXMax);

	//起始的Y
	WriteRegister(0x0006,(pRect->sYMin)>>8);
	WriteRegister(0x0007,pRect->sYMin);
	//结束的Y
	WriteRegister(0x0008,(pRect->sYMax)>>8);
	WriteRegister(0x0009,pRect->sYMax);

	WriteCommand(0x0022);
	for(lCount=((pRect->sXMax-pRect->sXMin+1)*(pRect->sYMax-pRect->sYMin+1));
		lCount>=0;lCount--)
	WriteData(ulValue);
}

static unsigned long LCD_240x480_HX8352ColorTranslate(void *pvDisplayData,unsigned long ulValue)
{
	return(DPYCOLORTRANSLATE(ulValue));
}


//空函数
char LCD_240x480_HX8352Flush(void *pvDisplayData)
{
	return 0;
}


const tDisplay g_sLCD_240x480_HX8352=
{
	sizeof(tDisplay),
	0,
#if defined(PORTRAIT) || defined (PORTRAIT_FLIP)
	240,
	400,
#else
	400,
	240,
#endif
	LCD_240x480_HX8352PixelDraw,
	LCD_240x480_HX8352PixelDrawMultiple,
	LCD_240x480_HX8352LineDrawH,
	LCD_240x480_HX8352LineDrawV,
	LCD_240x480_HX8352RectFill,
	LCD_240x480_HX8352ColorTranslate,
	LCD_240x480_HX8352Flush
};


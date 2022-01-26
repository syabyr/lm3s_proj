#ifndef __320x480_ILI9481_H__
#define __320x480_ILI9481_H__

#include "inc/hw_types.h"
#include "inc/hw_ints.h"

//定义和液晶通信的管脚

//定义数据总线低8位管脚,PORT A
#define LCD_DATAL_BASE GPIO_PORTA_BASE
//定义数据总线高8位管脚,PORT B
#define LCD_DATAH_BASE GPIO_PORTB_BASE
//由于PB7为JTAG调试管脚,用PC7替代PB7
#define LCD_DATAE_BASE GPIO_PORTC_BASE

//定义复位控制线管脚,PG0
#define LCD_RST_BASE	GPIO_PORTG_BASE
#define LCD_RST_PIN	GPIO_PIN_0

//定义寄存器选择管脚,PF2,网口左边LED
#define LCD_RS_BASE	GPIO_PORTF_BASE
#define LCD_RS_PIN		GPIO_PIN_2
//定义读数据管脚,PF0,PWM外部接口LED
#define LCD_RD_BASE	GPIO_PORTF_BASE
#define LCD_RD_PIN		GPIO_PIN_0
//定义写数据管脚,PF1
#define LCD_WR_BASE	GPIO_PORTF_BASE
#define LCD_WR_PIN	GPIO_PIN_1

//定义使能芯片管脚CS,PC5
#define LCD_CS_BASE	GPIO_PORTC_BASE
#define LCD_CS_PIN		GPIO_PIN5

//定义背光控制管脚,PC6
#define LCD_BL_BASE	GPIO_PORTC_BASE
#define LCD_BL_PIN		GPIO_PIN_6

//使能或去能LCD,PC5
#define SELECT_LCD GPIOPinWrite(GPIO_PORTC_BASE,0x20,0)
#define DESELECT_LCD GPIOPinWrite(GPIO_PORTC_BASE,0x20,0x20)

//打开或关闭背光,PC6
#define BACK_LIGHT_ON SELECT_LCD GPIOPinWrite(GPIO_PORTC_BASE,0x40,0x40)
#define BACK_LIGHT_OFF SELECT_LCD GPIOPinWrite(GPIO_PORTC_BASE,0x40,0)

//产生读信号和去除读信号,PF0
#define LCD_READ_ON GPIOPinWrite(GPIO_PORTF_BASE,0x01,0)
#define LCD_READ_OFF GPIOPinWrite(GPIO_PORTF_BASE,0x01,1)

//产生写信号和去除写信号,PF1
#define LCD_WRITE_ON GPIOPinWrite(GPIO_PORTF_BASE,0x02,0)
#define LCD_WRITE_OFF GPIOPinWrite(GPIO_PORTF_BASE,0x02,0x02)

//使用寄存器或不使用寄存器,PF2
#define LCD_RS_ON  GPIOPinWrite(GPIO_PORTF_BASE,0x04,0)
#define LCD_RS_OFF GPIOPinWrite(GPIO_PORTF_BASE,0x04,0x04)

//向数据总线写数据
extern void WriteData(unsigned short usData);
//从数据总线读数据
extern unsigned short ReadData(void);
//向液晶写命令
extern void WriteCommand(unsigned char ucData);
//读寄存器的值
extern unsigned short ReadRegister(unsigned char ucIndex);
//写寄存器的值
extern void WriteRegister(unsigned char ucIndex,unsigned short usValue);

//打开背光
extern void LCD_320x480_ILI9481BacklightOn(void);
//关闭背光
extern void LCD_320x480_ILI9481BacklightOff(void);
//液晶初始化
extern void LCD_320x480_ILI9481Init(void);
//获取驱动器的ID
extern unsigned short LCD_320x480_ILI9481ControllerIdGet(void);
//画一个像素
extern void LCD_320x480_ILI9481PixelDraw(unsigned long lX,unsigned long lY,
								unsigned long ulValue);

//绘制多个像素
extern void LCD_320x480_ILI9481PixelDrawMultiple(void *pvDisplayData,long lX,long lY,long lX0,long lCount,
									long lBPP,
									const unsigned char *pucData,
									const unsigned char *pucPalette);

//绘制一条水平的线段
extern void LCD_320x480_ILI9481LineDrawH(void *pvDisplayData,long lX1,long lX2,long lY,unsigned long ulValue);
//画一条垂直的线段
extern void LCD_320x480_ILI9481LineDrawV(void *pvDisplayData,long lX,long lY1,long lY2,unsigned long ulValue);

void LCD_320x480_ILI9481RectFill(void *pvDisplayData,const tRectangle *pRect,unsigned long ulValue);

extern char LCD_320x480_ILI9481Flush(void *pvDisplayData);

//测试像素
extern void TestPixel(void);
extern void TestDirection(void);


//创建一个驱动类型
extern const tDisplay g_sLCD_320x480_ILI9481;


#endif

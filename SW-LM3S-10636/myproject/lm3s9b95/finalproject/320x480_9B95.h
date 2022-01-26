#ifndef __320x480_ILI9481_H__
#define __320x480_ILI9481_H__

#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include <stdint.h>
#include <stdio.h>


//定义和液晶通信的管脚

//定义数据总线低8位管脚,PORT H
#define LCD_DATAL_BASE GPIO_PORTH_BASE
//定义数据总线高8位管脚,PORT J
#define LCD_DATAH_BASE GPIO_PORTJ_BASE


//定义复位控制线管脚,PF4
#define LCD_RST_BASE	GPIO_PORTF_BASE
#define LCD_RST_PIN	GPIO_PIN_4

//定义寄存器选择管脚RS,PE1
#define LCD_RS_BASE	GPIO_PORTE_BASE
#define LCD_RS_PIN		GPIO_PIN_1
//定义读数据管脚,RD,PE3
#define LCD_RD_BASE	GPIO_PORTE_BASE
#define LCD_RD_PIN		GPIO_PIN_3
//定义写数据管脚WR,PE2
#define LCD_WR_BASE	GPIO_PORTE_BASE
#define LCD_WR_PIN	GPIO_PIN_2

//定义使能芯片管脚CS,PE0
#define LCD_CS_BASE	GPIO_PORTE_BASE
#define LCD_CS_PIN		GPIO_PIN_0



//使能或去能LCD,PE0
#define SELECT_LCD GPIOPinWrite(LCD_CS_BASE,LCD_CS_PIN,LCD_CS_PIN)
#define DESELECT_LCD GPIOPinWrite(LCD_CS_BASE,LCD_CS_PIN,0)



//产生读信号和去除读信号,PE3
#define LCD_READ_ON GPIOPinWrite(LCD_RD_BASE,LCD_RD_PIN,0)
#define LCD_READ_OFF GPIOPinWrite(LCD_RD_BASE,LCD_RD_PIN,LCD_RD_PIN)

//产生写信号和去除写信号,PE2
#define LCD_WRITE_ON GPIOPinWrite(LCD_WR_BASE,LCD_WR_PIN,0)
#define LCD_WRITE_OFF GPIOPinWrite(LCD_WR_BASE,LCD_WR_PIN,LCD_WR_PIN)

//使用寄存器或不使用寄存器,PE1
#define LCD_RS_ON  GPIOPinWrite(LCD_RS_BASE,LCD_RS_PIN,0)
#define LCD_RS_OFF GPIOPinWrite(LCD_RS_BASE,LCD_RS_PIN,LCD_RS_PIN)

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

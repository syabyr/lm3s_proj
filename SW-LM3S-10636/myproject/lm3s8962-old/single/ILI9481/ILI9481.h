#ifndef _ILI9481_
#define _ILI9481_
#include "inc/hw_types.h"

//定义我们要使用的模式
#define LANDSCAPE
//我们要使用左右比较宽,上下比较窄的模式,接口在右边
//如果没有其他方向定义,默认为portrait模式
#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) && \
    ! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define PORTRAIT
#endif


//定义和液晶通信的管脚

//定义数据总线高8位管脚,PA
#define LCD_DATAL_BASE	GPIO_PORTA_BASE
//定义数据总线低8位管脚,PB
#define LCD_DATAH_BASE		GPIO_PORTB_BASE

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
//定义背光控制管脚,PC6
#define LCD_BL_BASE	GPIO_PORTC_BASE
#define LCD_BL_PIN		GPIO_PIN_6



//液晶初始化
extern void ILI9481Init(void);
extern void MyInit(void);
extern void LCDInit(void);
//打开背光
extern void ILI9481BacklightOn(void);
//关闭背光
extern void ILI9481BacklightOff(void);

//获取驱动器的ID
extern unsigned short ILI9481ControllerIdGet(void);

//创建一个驱动类型
extern const tDisplay ILI9481;

//向数据总线写数据
extern void WriteData(unsigned short usData);
//从数据总线读数据
//extern uint16_t ReadData(void);
//向液晶写命令
extern void WriteCommand(unsigned char ucData);

#endif

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "grlib/grlib.h"
#include "ILI9481.h"


//总线数据写入,将16bit的数据挂载数据总线上,然后使能写信号
void WriteData(unsigned short usData)
{
	//将数据写入数据总线,高8位写在PORTA上,低8位写在PORTB上.
	HWREG(LCD_DATAH_BASE+GPIO_O_DATA+(0xff<<2))=usData>>8;
	HWREG(LCD_DATAL_BASE+GPIO_O_DATA+(0xff<<2))=usData;
	GPIOPinWrite(GPIO_PORTC_BASE,0x80,(usData>>8)&0x0080);
	//使能读信号,将信号线拉低后再拉高
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=0;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=0;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=0;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=0;
	
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=LCD_WR_PIN;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=LCD_WR_PIN;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=LCD_WR_PIN;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=LCD_WR_PIN;
	
}

//总线数据读取,将总线设置为输入,使能读信号,然后读入数据,最后将总线设置为输出.
unsigned short ReadData(void)
{
	unsigned short usData;
	//将总线设置为输入
	HWREG(LCD_DATAH_BASE+GPIO_O_DIR)=0x00;
	HWREG(LCD_DATAL_BASE+GPIO_O_DIR)=0x00;
	//拉低读信号
	HWREG(LCD_RD_BASE+GPIO_O_DATA+(LCD_RD_PIN<<2))=0;
	HWREG(LCD_RD_BASE+GPIO_O_DATA+(LCD_RD_PIN<<2))=0;
	HWREG(LCD_RD_BASE+GPIO_O_DATA+(LCD_RD_PIN<<2))=0;
	HWREG(LCD_RD_BASE+GPIO_O_DATA+(LCD_RD_PIN<<2))=0;
	//读取数据
	usData=HWREG(LCD_DATAH_BASE+GPIO_O_DATA+(0xff<<2))<<8;
	usData|=HWREG(LCD_DATAL_BASE+GPIO_O_DATA+(0xff<<2));
	//拉高读信号
	HWREG(LCD_RD_BASE+GPIO_O_DATA+(LCD_RD_PIN<<2))=LCD_RD_PIN;
	//将总线设置为输出端口
	HWREG(LCD_DATAH_BASE+GPIO_O_DIR)=0xff;
	HWREG(LCD_DATAL_BASE+GPIO_O_DIR)=0xff;

	return (usData);
}

//写命令,8bit的命令挂载数据总线的低8位,使能寄存器选择信号,再使能写数据信号.
void WriteCommand(unsigned char ucData)
{
	//将数据高8bit清零,低8bit载入数据
	HWREG(LCD_DATAH_BASE+GPIO_O_DATA+(0xff<<2))=0x00;
	HWREG(LCD_DATAL_BASE+GPIO_O_DATA+(0xff<<2))=ucData;
	GPIOPinWrite(GPIO_PORTC_BASE,0x80,0);
	//拉低寄存器选择信号,然后使能写信号
	HWREG(LCD_RS_BASE+GPIO_O_DATA+(LCD_RS_PIN<<2))=0;
	HWREG(LCD_RS_BASE+GPIO_O_DATA+(LCD_RS_PIN<<2))=0;
	HWREG(LCD_RS_BASE+GPIO_O_DATA+(LCD_RS_PIN<<2))=0;
	HWREG(LCD_RS_BASE+GPIO_O_DATA+(LCD_RS_PIN<<2))=0;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=0;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=0;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=0;
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=0;
	
	//释放写使能信号和寄存器选择信号
	HWREG(LCD_WR_BASE+GPIO_O_DATA+(LCD_WR_PIN<<2))=LCD_WR_PIN;
	HWREG(LCD_RS_BASE+GPIO_O_DATA+(LCD_RS_PIN<<2))=LCD_RS_PIN;
}

//打开背光
void ILI9481BacklightOn(void)
{
	HWREG(LCD_BL_BASE+GPIO_O_DATA+(LCD_BL_PIN<<2))=LCD_BL_PIN;
}

//关闭背光
void ILI9481BacklightOff(void)
{
	HWREG(LCD_BL_BASE+GPIO_O_DATA+(LCD_BL_PIN<<2))=0;
}

void LCDInit(void)
{
	unsigned long ulClockMS;
	ulClockMS = SysCtlClockGet()/(3*1000);
	//使能要连接的端口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

	//把前面JTAG使用的端口转换为GPIO
	HWREG(GPIO_PORTB_BASE+GPIO_O_LOCK)=GPIO_LOCK_KEY;
	HWREG(GPIO_PORTB_BASE+GPIO_O_CR)=0x80;
	HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL)&=~0x80;
	HWREG(GPIO_PORTB_BASE+GPIO_O_LOCK)=GPIO_LOCK_KEY;
	HWREG(GPIO_PORTB_BASE+GPIO_O_CR)=0x00;
	HWREG(GPIO_PORTB_BASE+GPIO_O_LOCK)=0;

	//设置这些管脚为输出,以及他们的默认值
	//数据高8bit
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,0xff);
	GPIOPinWrite(GPIO_PORTA_BASE,0xff,0x00);
	//数据低8bit
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,0xff);
	GPIOPinWrite(GPIO_PORTB_BASE,0xff,0x00);
	//PC6,背光,默认背光关闭,等会要打开
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,0xC0);
	GPIOPinWrite(GPIO_PORTC_BASE,0xC0,0x00);
	//RS,WR,RD信号,PF0,PF1,PF2,默认都为高电平
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,0x07);
	GPIOPinWrite(GPIO_PORTF_BASE,0x07,0x07);
	//RESET复位信号,PG0,默认为高电平
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE,0x01);

	//复位部分
	SysCtlDelay(100*ulClockMS);
	GPIOPinWrite(GPIO_PORTG_BASE,0x01,0x00);	//拉低复位信号
	SysCtlDelay(30*ulClockMS);
	GPIOPinWrite(GPIO_PORTG_BASE,0x01,0x01);	//拉高复位信号
	SysCtlDelay(100*ulClockMS);

	WriteCommand(0x11);
	SysCtlDelay(100*ulClockMS);
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

	WriteCommand(0x36);	//set address mode
	WriteData(0x0A);

	WriteCommand(0x3A);	//set pixel format
	WriteData(0x55);

	WriteCommand(0x2A);	//set column address
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x01);
	WriteData(0x3F);

	WriteCommand(0x2B);	//set page address
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x01);
	WriteData(0xDF);

	SysCtlDelay(100*ulClockMS);

	WriteCommand(0x29);	//set display on
	WriteCommand(0x2C);	//write memory start
}



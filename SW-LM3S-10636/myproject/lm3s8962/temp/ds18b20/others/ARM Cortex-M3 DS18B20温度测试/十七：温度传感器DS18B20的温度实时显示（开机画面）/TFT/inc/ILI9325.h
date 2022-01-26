#ifndef __LCD_H
#define __LCD_H


#include "stm32f10x_lib.h"

/*LCD Control Pins*/
#define  GPIOD_LCD_CS                   GPIO_Pin_7
#define  GPIOD_LCD_RS                   GPIO_Pin_8
#define  GPIOD_LCD_WR                   GPIO_Pin_9
#define  GPIOD_LCD_RD                   GPIO_Pin_10
#define  GPIOD_LCD_RESET                GPIO_Pin_11

#define ILI9325_CS_LOW()     GPIO_ResetBits(GPIOD,GPIOD_LCD_CS)
#define ILI9325_CS_HIGH()    GPIO_SetBits(GPIOD,GPIOD_LCD_CS)
#define ILI9325_RS_LOW()     GPIO_ResetBits(GPIOD,GPIOD_LCD_RS)
#define ILI9325_RS_HIGH()    GPIO_SetBits(GPIOD,GPIOD_LCD_RS)
#define ILI9325_WR_LOW()     GPIO_ResetBits(GPIOD,GPIOD_LCD_WR)
#define ILI9325_WR_HIGH()    GPIO_SetBits(GPIOD,GPIOD_LCD_WR)
#define ILI9325_RD_LOW()     GPIO_ResetBits(GPIOD,GPIOD_LCD_RD)
#define ILI9325_RD_HIGH()    GPIO_SetBits(GPIOD,GPIOD_LCD_RD)
#define ILI9325_RESET_LOW()  GPIO_ResetBits(GPIOD,GPIOD_LCD_RESET)
#define ILI9325_RESET_HIGH() GPIO_SetBits(GPIOD,GPIOD_LCD_RESET)

/* LCD Registers */
#define R0             0x00
#define R1             0x01
#define R2             0x02
#define R3             0x03
#define R4             0x04
#define R7             0x07
#define R8             0x08
#define R9             0x09
#define R0A            0x0A
#define R0C            0x0C
#define R0D            0x0D
#define R0F            0x0F

#define R10            0x10
#define R11            0x11
#define R12            0x12
#define R13            0x13

#define R20            0x20
#define R21            0x21
#define R22            0x22
#define R29            0x29
#define R2B            0x2B

#define R30            0x30
#define R31            0x31
#define R32            0x32
#define R35            0x35
#define R36            0x36
#define R37            0x37
#define R38            0x38
#define R39            0x39
#define R3C            0x3C
#define R3D            0x3D

#define R50            0x50
#define R51            0x51
#define R52            0x52
#define R53            0x53

#define R60            0x60
#define R61            0x61
#define R6A            0x6A

#define R80            0x80
#define R81            0x81
#define R82            0x82
#define R83            0x83
#define R84            0x84
#define R85            0x85

#define R90            0x90
#define R92            0x92
#define R95            0x95
#define R97            0x97

#define RA1            0xA1
#define RA2            0xA2
#define RA5            0xA5


/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define Horizontal     0x00//水平方向
#define Vertical       0x01//垂直方向


//字符颜色默认为黑色，背景默认为白色
static volatile unsigned short TextColor = 0x0000, BackColor = 0xFFFF;

/*----- High layer function -----*/
void ILI9325_Init(void);//LCD的初始化
void ILI9325_SetTextColor(volatile unsigned short Color);//设置字符颜色
void ILI9325_SetBackColor(volatile unsigned short Color);//设置背景颜色
void ILI9325_ClearLine(unsigned char Line);//清除一行（空格）
void ILI9325_Clear(unsigned short COLOR);//清屏(指定颜色）
void ILI9325_SetCursor(unsigned short Xpos, unsigned short Ypos);//设定光标起点
void ILI9325_FullScreen(unsigned short Color);//按指定颜色全屏单色显示
void ILI9325_Row_ColorBar(void);//彩色横条纹显示测试
void ILI9325_Col_ColorBar(void);//彩色竖条纹显示测试
void ILI9325_GrayScale(void);//灰阶显示测试

void ILI9325_DrawChar(unsigned short Xpos, unsigned short Ypos, unsigned char *c);//向指定光标位置显示一个字符(用当前颜色配置）
void ILI9325_DrawCharColor(unsigned short Xpos, unsigned short Ypos, unsigned char *c,unsigned short CharColor,unsigned short BkColor);//向指定光标位置显示一个字符（自指定颜色）
void ILI9325_DisplayChar(unsigned short Line, unsigned short Column, unsigned char Ascii);//向指定行号，列号位置显示一个字符
void ILI9325_DisplayStringLine(unsigned char Line, unsigned char *ptr);//显示一行字符，可能不到一行字符正常显示，余下的空间不改变原来内容。超过一行的不显示
void ILI9325_DisplayString(unsigned char Line, unsigned char *ptr);//最多显示整个屏幕的字符，共600个(还没调试成功）
void ILI9325_DisplayString_At(unsigned short x,unsigned short y,unsigned char *pAscii,unsigned short WordColor,unsigned short BackColor);//在制定光标处显示字符串，可自动换行
void ILI9325_DisplayHanZi_16(unsigned short x,unsigned short y,const unsigned char *pHz,unsigned short WordColor,unsigned short BackColor);//显示一个汉字16*16的（需取字模）
void ILI9325_DisplayHanZi_48(unsigned short x,unsigned short y,const unsigned char *pHz,unsigned short WordColor,unsigned short BackColor);//显示一个汉字48*48的（需取字模）
void ILI9325_Display_FONT16(unsigned short x,unsigned short y,unsigned char *pHz,unsigned short WordColor,unsigned char BackColor);//显示汉字串（需国标字库支持） 

void ILI9325_SetDisplayWindow(unsigned short Xpos, unsigned short Ypos, unsigned short Width, unsigned short Height );//设定显示窗口
void ILI9325_DrawLine(unsigned short Xpos, unsigned short Ypos, unsigned short Length, unsigned short Direction);//画直线
void ILI9325_DrawRect(unsigned short Xpos, unsigned short Ypos, unsigned short Width, unsigned short Height);//在指定光标处画一个矩形

void ILI9325_ScrollText(unsigned char Line, unsigned char *ptr);//在指定行滚动显示字符(还没调试成功）

void ILI9325_DrawCircle(unsigned short Xpos, unsigned short Ypos, unsigned short Radius);//在指定圆心处根据半径长画一个圆

void ILI9325_DrawBMP(unsigned short x,unsigned short y,unsigned short Width,unsigned short Height,const unsigned char *pBMP);//指定位置显示指定大小的图片（图片宽和高最好为偶数）

unsigned short ILI9325_BGR2RGB(unsigned short RGB_Code);//颜色值从RGB转为BGR格式
void ILI9325_SetPixel(unsigned short Xpos,unsigned short Ypos,unsigned short RGB_Code);//写一个指定像素点

unsigned short ILI9325_GetPointRGB(unsigned short Xpos,unsigned short Ypos);//获得指定像素点的颜色数据
unsigned short ILI9325_ReadRegister(unsigned short LCD_Reg);//读取指定寄存器的值
unsigned short CheckController(void);//获得控制器代码
void ILI9325_Reset(void);//复位LCD一次

/*----- Medium layer function -----*/
void ILI9325_WriteReg(unsigned char LCD_Reg, unsigned short LCD_RegValue);//配置指定寄存器
void ILI9325_WriteGRAM(unsigned short RGB_Code);//向GRAM中写入显示数据
void ILI9325_WriteIndex(unsigned char Index);//寻址寄存器
void ILI9325_WriteData(unsigned short Data);//写数据
unsigned short ILI9325_ReadData(void);//读数据
void ILI9325_PowerOn(void);//打开电源（还没调试成功）
void ILI9325_DisplayOn(void);//打开显示（还没调试成功，闪烁情况）
void ILI9325_DisplayOff(void);//关闭显示，但电源没关闭


/*----- Low layer function -----*/
void ILI9325_CtrlLinesConfig(void);//控制线配置
void ILI9325_Delay (unsigned long nCount);//延时（大概1ms的基数）
unsigned long   StrLength(unsigned char *Str);//计算字符串长度
void u32tostr(unsigned long dat,unsigned char *str);
unsigned long strtou32(unsigned char *str);

#endif /* __LCD_H */

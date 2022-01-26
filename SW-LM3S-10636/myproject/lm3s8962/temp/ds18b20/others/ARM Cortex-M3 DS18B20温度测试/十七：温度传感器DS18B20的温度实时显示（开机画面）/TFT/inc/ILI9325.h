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

#define Horizontal     0x00//ˮƽ����
#define Vertical       0x01//��ֱ����


//�ַ���ɫĬ��Ϊ��ɫ������Ĭ��Ϊ��ɫ
static volatile unsigned short TextColor = 0x0000, BackColor = 0xFFFF;

/*----- High layer function -----*/
void ILI9325_Init(void);//LCD�ĳ�ʼ��
void ILI9325_SetTextColor(volatile unsigned short Color);//�����ַ���ɫ
void ILI9325_SetBackColor(volatile unsigned short Color);//���ñ�����ɫ
void ILI9325_ClearLine(unsigned char Line);//���һ�У��ո�
void ILI9325_Clear(unsigned short COLOR);//����(ָ����ɫ��
void ILI9325_SetCursor(unsigned short Xpos, unsigned short Ypos);//�趨������
void ILI9325_FullScreen(unsigned short Color);//��ָ����ɫȫ����ɫ��ʾ
void ILI9325_Row_ColorBar(void);//��ɫ��������ʾ����
void ILI9325_Col_ColorBar(void);//��ɫ��������ʾ����
void ILI9325_GrayScale(void);//�ҽ���ʾ����

void ILI9325_DrawChar(unsigned short Xpos, unsigned short Ypos, unsigned char *c);//��ָ�����λ����ʾһ���ַ�(�õ�ǰ��ɫ���ã�
void ILI9325_DrawCharColor(unsigned short Xpos, unsigned short Ypos, unsigned char *c,unsigned short CharColor,unsigned short BkColor);//��ָ�����λ����ʾһ���ַ�����ָ����ɫ��
void ILI9325_DisplayChar(unsigned short Line, unsigned short Column, unsigned char Ascii);//��ָ���кţ��к�λ����ʾһ���ַ�
void ILI9325_DisplayStringLine(unsigned char Line, unsigned char *ptr);//��ʾһ���ַ������ܲ���һ���ַ�������ʾ�����µĿռ䲻�ı�ԭ�����ݡ�����һ�еĲ���ʾ
void ILI9325_DisplayString(unsigned char Line, unsigned char *ptr);//�����ʾ������Ļ���ַ�����600��(��û���Գɹ���
void ILI9325_DisplayString_At(unsigned short x,unsigned short y,unsigned char *pAscii,unsigned short WordColor,unsigned short BackColor);//���ƶ���괦��ʾ�ַ��������Զ�����
void ILI9325_DisplayHanZi_16(unsigned short x,unsigned short y,const unsigned char *pHz,unsigned short WordColor,unsigned short BackColor);//��ʾһ������16*16�ģ���ȡ��ģ��
void ILI9325_DisplayHanZi_48(unsigned short x,unsigned short y,const unsigned char *pHz,unsigned short WordColor,unsigned short BackColor);//��ʾһ������48*48�ģ���ȡ��ģ��
void ILI9325_Display_FONT16(unsigned short x,unsigned short y,unsigned char *pHz,unsigned short WordColor,unsigned char BackColor);//��ʾ���ִ���������ֿ�֧�֣� 

void ILI9325_SetDisplayWindow(unsigned short Xpos, unsigned short Ypos, unsigned short Width, unsigned short Height );//�趨��ʾ����
void ILI9325_DrawLine(unsigned short Xpos, unsigned short Ypos, unsigned short Length, unsigned short Direction);//��ֱ��
void ILI9325_DrawRect(unsigned short Xpos, unsigned short Ypos, unsigned short Width, unsigned short Height);//��ָ����괦��һ������

void ILI9325_ScrollText(unsigned char Line, unsigned char *ptr);//��ָ���й�����ʾ�ַ�(��û���Գɹ���

void ILI9325_DrawCircle(unsigned short Xpos, unsigned short Ypos, unsigned short Radius);//��ָ��Բ�Ĵ����ݰ뾶����һ��Բ

void ILI9325_DrawBMP(unsigned short x,unsigned short y,unsigned short Width,unsigned short Height,const unsigned char *pBMP);//ָ��λ����ʾָ����С��ͼƬ��ͼƬ��͸����Ϊż����

unsigned short ILI9325_BGR2RGB(unsigned short RGB_Code);//��ɫֵ��RGBתΪBGR��ʽ
void ILI9325_SetPixel(unsigned short Xpos,unsigned short Ypos,unsigned short RGB_Code);//дһ��ָ�����ص�

unsigned short ILI9325_GetPointRGB(unsigned short Xpos,unsigned short Ypos);//���ָ�����ص����ɫ����
unsigned short ILI9325_ReadRegister(unsigned short LCD_Reg);//��ȡָ���Ĵ�����ֵ
unsigned short CheckController(void);//��ÿ���������
void ILI9325_Reset(void);//��λLCDһ��

/*----- Medium layer function -----*/
void ILI9325_WriteReg(unsigned char LCD_Reg, unsigned short LCD_RegValue);//����ָ���Ĵ���
void ILI9325_WriteGRAM(unsigned short RGB_Code);//��GRAM��д����ʾ����
void ILI9325_WriteIndex(unsigned char Index);//Ѱַ�Ĵ���
void ILI9325_WriteData(unsigned short Data);//д����
unsigned short ILI9325_ReadData(void);//������
void ILI9325_PowerOn(void);//�򿪵�Դ����û���Գɹ���
void ILI9325_DisplayOn(void);//����ʾ����û���Գɹ�����˸�����
void ILI9325_DisplayOff(void);//�ر���ʾ������Դû�ر�


/*----- Low layer function -----*/
void ILI9325_CtrlLinesConfig(void);//����������
void ILI9325_Delay (unsigned long nCount);//��ʱ�����1ms�Ļ�����
unsigned long   StrLength(unsigned char *Str);//�����ַ�������
void u32tostr(unsigned long dat,unsigned char *str);
unsigned long strtou32(unsigned char *str);

#endif /* __LCD_H */

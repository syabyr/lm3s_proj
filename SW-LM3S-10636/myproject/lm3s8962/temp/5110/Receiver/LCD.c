#include "include.h"
/*************************************                              
       use SPI send byte
**************************************/
void lcd_sendbyte(unsigned char dat,unsigned char command)
{
	unsigned char i;
	NOKIA5110_CE_L;
	if(command==0)
		NOKIA5110_DC_L;//传送命令
	else
		NOKIA5110_DC_H;	//传送数据
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
			NOKIA5110_DIN_H;
		else
			NOKIA5110_DIN_L;
		NOKIA5110_CLK_L;
		dat=dat<<1;
		NOKIA5110_CLK_H;
	}
	NOKIA5110_CE_H;
}

/********************************************
           lcd_set_xy
*********************************************/
void lcd_set_xy(unsigned char x,unsigned char y)
{
	lcd_sendbyte(0x20,0);//H=0
	lcd_sendbyte(0x80|x,0);//x-0 to 83
	lcd_sendbyte(0x40|y,0);//y-0 to 5
}

/*******************************************
        lcd clear
********************************************/
void lcd_clear()
{
	unsigned char i,j;
	lcd_sendbyte(0x0c,0);//设定显示模式，正常显示	
	lcd_sendbyte(0x80,0);//设置RAM起始地址
	for(j=0;j<6;j++)
	{
		for(i=0;i<84;i++)
		{
			lcd_sendbyte(0,1);
		}
	}
}
/*******************************************
                lcd初始化
********************************************/
void lcd_init()
{
	unsigned char i;
	const unsigned char VOP[]="VOP: 0x";
	NOKIA5110_RST_L;//产生一个让LCD复位的低电平脉冲
	for(i=0;i<50;i++)
	{
		_NOP();
	}
	NOKIA5110_RST_H;

	lcd_sendbyte(0x21,0);//使用扩展命令设置LCD模式chip is active & horizontal addressing (H=1)
	lcd_sendbyte(LCD_Leval,0);//设置VOP值lcd 电压
	lcd_sendbyte(0x20,0);//使用基本命令,水平寻址                                       (H=0)
	lcd_sendbyte(0x0c,0);//设定显示模式，正常显示display in normal mode
	lcd_clear();

	LCD_Disp_BNum(2,1,Receive_Number/10000%10);
	LCD_Disp_BNum(18,1,Receive_Number/1000%10);
	LCD_Disp_BNum(34,1,Receive_Number/100%10);
	LCD_Disp_BNum(50,1,Receive_Number/10%10);
	LCD_Disp_BNum(66,1,Receive_Number%10);

	lcd_write_stringxy(0,0,VOP);
	if(((LCD_Leval&0xf0)>>4)<=9)
		lcd_write_char(((LCD_Leval&0xf0)>>4)+'0');
	else
		lcd_write_char(((LCD_Leval&0xf0)>>4)-0x0a+'a');
	if((LCD_Leval&0x0f)<=9)
		lcd_write_char((LCD_Leval&0x0f)+'0');
	else
		lcd_write_char((LCD_Leval&0x0f)-0x0a+'a');




}

/*********************************************
       display a asciifont6*8
*********************************************/
void lcd_write_char(unsigned char c)
{
	unsigned char line;
	c-=32;
	for(line=0;line<6;line++)
	lcd_sendbyte(ASCII[c][line],1);
}

/*********************************************
        英文字符串显示函数
**********************************************/
void lcd_write_stringxy(unsigned char x,unsigned char y,unsigned char *p)
{
	lcd_set_xy(x,y);
	while(*p)
	{
		lcd_write_char(*p);
		p++;
	}
}

/*********************************************
        英文字符串显示函数
**********************************************/
void lcd_write_string(unsigned char *p)
{
	while(*p)
	{
		lcd_write_char(*p);
		p++;
	}
}

const char number_array[]="0123456789";  //便于显示数字,与下面LCD_Disp_Num()函数有关

/*********************************************************** 
* LCD_Disp_Num
* Function:Write a decimal number on LCD 
* 0 < Num < 9
***********************************************************/
void LCD_Disp_Num(unsigned char x,unsigned char y,unsigned int Num)  
{ 
	lcd_set_xy(x,y);
	lcd_write_char(number_array[Num]);
} 

void LCD_Disp_BNum(unsigned char x,unsigned char y,unsigned char n)
{
  unsigned i;
  lcd_set_xy(x,y);
  for(i=0;i<15;i++)
  lcd_sendbyte(Num[n][i],1);
  lcd_set_xy(x,y+1);
  for(i=0;i<15;i++)
  lcd_sendbyte(Num[n][i+15],1);
  lcd_set_xy(x,y+2);
  for(i=0;i<15;i++)
  lcd_sendbyte(Num[n][i+30],1);
  lcd_set_xy(x,y+3);
  for(i=0;i<15;i++)
  lcd_sendbyte(Num[n][i+45],1);
}

/**************************************************************************************************************
Create Date:	2006-11-10 11:10
Name:			Luquan
Version:		1.0
Note:

**************************************************************************************************************/
#ifndef __SubProgram_define_H__
#define __SubProgram_define_H__

/*-------------------------------------------------------------------------------------------------------------
		子程序声明								子程序声明						2006-11-10 11:10
-------------------------------------------------------------------------------------------------------------*/
void Display_FullColor(unsigned int) small;
void Display_Gray(unsigned int, unsigned int) small;
void Display_BW_Gray(void) small;
void Display_Border(void) small;
void Display_Picture(unsigned int,unsigned int,unsigned int,unsigned int) small;
void LCD_Window_Set(unsigned int, unsigned int, unsigned int, unsigned int) small;
void Display_TP_TestIcon(unsigned int,unsigned int,unsigned int,unsigned int,unsigned char) small;
void TP_Test(void) small;
void TP_Press(void) small;
void TP_Write_Com_TSC2046(unsigned char) small;
unsigned int TP_Read_Data_TSC2046(void) small;
void DelayXms(unsigned int) small;
void Step(void) small;
void LCD_SetReg(unsigned int, unsigned int) small;
void LCD_DataWrite(unsigned char, unsigned char) small;
void LCD_Init_DST1011(unsigned char) small;
void Pen_Input(void) small;


#endif

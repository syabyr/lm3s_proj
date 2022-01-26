/**************************************************************************************************************
CPU Board:			
CPU:				W78E516B at 12MHz
EEPROM:				NC
TP Driver IC:		TC2046
Create Date:		2007-12-16 12:20
Name:				Luquan
Version:			1.0
Note:

**************************************************************************************************************/
#ifndef __IO_Port_define_H__
#define __IO_Port_define_H__

/*-------------------------------------------------------------------------------------------------------------
		硬件接线										硬件接线					2006-11-10 9:48
-------------------------------------------------------------------------------------------------------------*/
#define KEY_Left		P1_0
#define KEY_Right		P1_1

#define TP_DCLK			P1_2
#define TP_CS			P1_3
#define TP_DIN			P1_4
#define TP_BUSY			P1_5
#define TP_DOUT			P1_6
#define TP_PENIRQ		P1_7

#define LCD_CS			P3_0
#define LCD_RS			P3_1
#define LCD_WR			P3_2
#define LCD_RD			P3_3
//#define LCD_NC			P3_4
//#define LCD_NC			P3_5
//#define LCD_NC			P3_6
#define LCD_Reset		P3_7

#define LCD_Port_H		P2		//注意端口顺序
#define LCD_Port_L		P0		//注意端口顺序
#define LCD_Port		P0		//注意端口顺序


#endif
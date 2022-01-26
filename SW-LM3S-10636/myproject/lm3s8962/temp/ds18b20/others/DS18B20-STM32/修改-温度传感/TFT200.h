

#ifndef __TFT200_H
#define __TFT200_H
#include "MyType.h"

#define LCD_WIDTH	220
#define LCD_HIGH	176



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
void delay1ms (int cnt); 
void LCD_Reg_Set(unsigned char Cmd,unsigned char Data);
void LCD_Init( void );
void LCD_SetArea( U8 x0, U8 y0, U8 x1, U8 y1 );
void LCD_Clear_Screen( unsigned int Data );
void LCD_Rectangle( unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned int Data );
void LCD_SetPixel(unsigned short x, unsigned short y, unsigned int color);
void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int f, unsigned int b);
void LCD_PutString(unsigned short x, unsigned short y, char *s, unsigned int f, unsigned int b);
void PutGB1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int f,unsigned int b);
void LCD_BMP( U8 x0, U8 y0, U8 x1, U8 y1 , U8 * Bmp );
#endif


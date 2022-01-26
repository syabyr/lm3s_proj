// 芯片相关的头文件
#include <stm32f10x_lib.h>    // STM32F10x 库文件
#include "MyType.h"
#include "TFT200.h"
 // 实验板上相关定义  
/* PINS: 管脚定义
   - DB0 = PC0
   - DB1 = PC1
   - DB2 = PC2
   - DB3 = PC3
   - DB4 = PC4
   - DB5 = PC5
   - DB6 = PC6
   - DB7 = PC7
   - RD  = PC8
   - RW  = PC9
   - RS  = PC10
   - RST = PC11
   - CS  = PC12                                             */

#define PIN_CS               (   1 << 12)
#define PIN_RST              (   1 << 11)
#define PIN_RS               (   1 << 10)
#define PIN_RW               (   1 <<  9)
#define PIN_RD               (   1 <<  8)
#define PINS_CTRL            (0x1F <<  8)
#define PINS_DATA            (0xFF <<  0)
#define PINS_ALL             (PINS_CTRL | PINS_DATA)

//  时钟使能                            
#define LCD_CLOCK_EN         (RCC->APB2ENR |= (1 << 4)); // enable clock for GPIOC

 //写1，0                                              
#define LCD_CS(x)             GPIOC->ODR = (GPIOC->ODR & ~PIN_CS) | (x ? PIN_CS : 0);                                               
#define LCD_RST(x)            GPIOC->ODR = (GPIOC->ODR & ~PIN_RST) | (x ? PIN_RST : 0);                                             
#define LCD_RS(x)             GPIOC->ODR = (GPIOC->ODR & ~PIN_RS) | (x ? PIN_RS : 0);                                          
#define LCD_RW(x)             GPIOC->ODR = (GPIOC->ODR & ~PIN_RW) | (x ? PIN_RW : 0);                                    
#define LCD_RD(x)             GPIOC->ODR = (GPIOC->ODR & ~PIN_RD) | (x ? PIN_RD : 0);

// 读数据                                                         
#define LCD_DATA_IN           (((GPIOC->IDR & PINS_DATA) >> 0) & 0xFF);
// 写数据                                                
#define LCD_DATA_OUT(x)       GPIOC->ODR = (GPIOC->ODR & ~PINS_DATA) | (x << 0);
// 设置为输出模式                                            
#define LCD_ALL_DIR_OUT       GPIOC->CRL = (GPIOC->CRL & 0x00000000) | 0x33333333;                              GPIOC->CRH = (GPIOC->CRH & 0xFFF00000) | 0x00033333;
// 设置DATA为输入模式                                           
#define LCD_DATA_DIR_IN       GPIOC->CRL = (GPIOC->CRL & 0x00000000) | 0x44444444;
// 设置DATA为输入模式                                            
#define LCD_DATA_DIR_OUT      GPIOC->CRL = (GPIOC->CRL & 0x00000000) | 0x33333333;


//软件相关

static void delay (int cnt)
 { 
  while (cnt--);
  }
void delay1ms (int cnt) 
  {
  int i;
  while (cnt--)
  for(i=0;i<8300;i++);
  }

void LCD_Bus_Initial( void ) 
  {
  LCD_CLOCK_EN                                                              
  LCD_ALL_DIR_OUT
  LCD_CS(1);
  LCD_RW(1);
  LCD_RD(1);
  LCD_RS(1);
  LCD_RST(1);
  }
void LCD_Write_Data( unsigned char Data ) 
  {
  LCD_RD(1);
  LCD_CS(0);
  LCD_RS(1);
  LCD_DATA_OUT( Data );
  LCD_RW(0);
  delay( 10 );
  LCD_RW(1);
  LCD_CS(1);
  }
void LCD_Write_Cmd(unsigned char Cmd) 
  {
  LCD_RD(1);
  LCD_CS(0);
  LCD_RS(0);
  LCD_DATA_OUT( Cmd );
  LCD_RW(0);
  delay( 10 );
  LCD_RW(1);
  LCD_CS(1);
  }
void LCD_Reg_Set(unsigned char Cmd,unsigned char Data)
  {
  LCD_Write_Cmd( Cmd );
  LCD_Write_Data( Data );
  }

 unsigned int LCD_Bus_Read_Byte( void ) 
  {
  unsigned int BusData;
  LCD_DATA_DIR_IN;
  LCD_RD(0);
  BusData = LCD_DATA_IN;
  delay( 10 );
  LCD_RD(1);
  return BusData>>8;
  }

 // 液晶初始化                                
void LCD_Init( void )
    {
	LCD_Bus_Initial();
	//Reset	Lcd
	delay1ms(100 );
  	LCD_RST(0);
    delay1ms(100 );
  	LCD_RST(1);
    delay1ms(100 );
	//Driving ability Setting
	LCD_Reg_Set(0x60,0x00); //PTBA[15:8]
	LCD_Reg_Set(0x61,0x06); //PTBA[7:0]
	LCD_Reg_Set(0x62,0x00); //STBA[15:8]
	LCD_Reg_Set(0x63,0xD9); //STBA[7:0]
	LCD_Reg_Set(0x73,0x70); //OPON[7:0],SET OPON=70h, default 38h
	//Gamma 2.0 Setting
	LCD_Reg_Set(0x40,0x20); //
	LCD_Reg_Set(0x41,0x41); //
	LCD_Reg_Set(0x42,0x41); //
	LCD_Reg_Set(0x43,0x01); //
	LCD_Reg_Set(0x44,0x34); //
	LCD_Reg_Set(0x45,0x02); //
	LCD_Reg_Set(0x46,0x08); //
	LCD_Reg_Set(0x47,0xD1); //
	LCD_Reg_Set(0x48,0x05); //
	LCD_Reg_Set(0x50,0x67); //
	LCD_Reg_Set(0x51,0x00); //
	LCD_Reg_Set(0x52,0x27); //
	LCD_Reg_Set(0x53,0x0B); //
	LCD_Reg_Set(0x54,0x83); //
	LCD_Reg_Set(0x55,0x0E); //
	LCD_Reg_Set(0x56,0x01); //
	LCD_Reg_Set(0x57,0x37); //
	//For SRAM Improve
	
	LCD_Reg_Set(0x66,0x45); //SETVDC
	LCD_Reg_Set(0x80,0x0C); //SETIOOPT
	//Power Voltage Setting
	LCD_Reg_Set(0x1F,0x03); //VRH=4.65V
	LCD_Reg_Set(0x20,0x02); //BT (VGH~15V,VGL~-7V,DDVDH~5V)  0x02
	LCD_Reg_Set(0x24,0x1C); //VMH(VCOM High voltage3.2V)
	LCD_Reg_Set(0x25,0x34); //VML(VCOM Low voltage -1.2V)
	//****VCOM offset**///
	LCD_Reg_Set(0x23,0x3f); //for Flicker adjust //can reload from OTP-48   0x48
	//Power on Setting
	LCD_Reg_Set(0x18,0x44); //I/P_RADJ,N/P_RADJ, Normal mode 60Hz
	LCD_Reg_Set(0x1B,0x44); //FS1 Pumping Clock= 2 x line frequency
	LCD_Reg_Set(0x21,0x01); //OSC_EN='1', start Osc
	LCD_Reg_Set(0x01,0x00); //SLP='0', out sleep
	LCD_Reg_Set(0x1C,0x03); //AP=011
	LCD_Reg_Set(0x19,0x06); // VOMG=1,PON=1, DK=0,
	delay1ms(5);
	//262k/65k color selection
	LCD_Reg_Set(0x17,0x05); //default 0x06 262k color // 0x05 65k color
	//Display ON Setting
	LCD_Reg_Set(0x26,0x84); //PT=10,GON=0, DTE=0, D=0100
	delay1ms(40);
	LCD_Reg_Set(0x26,0xB8); //PT=10,GON=1, DTE=1, D=1000
	delay1ms(40);
	LCD_Reg_Set(0x26,0xBC); //PT=10,GON=1, DTE=1, D=1100
	//Set Display Direction 
	LCD_Reg_Set(0x35,0x02);
	//Set GRAM Area
	LCD_Reg_Set(0x02,0x00);
	LCD_Reg_Set(0x03,0x00); //Column Start
	LCD_Reg_Set(0x04,0x00);
	LCD_Reg_Set(0x05,0xAF); //Column End
	LCD_Reg_Set(0x06,0x00);
	LCD_Reg_Set(0x07,0x00); //Row Start
	LCD_Reg_Set(0x08,0x00);
	LCD_Reg_Set(0x09,0xDB); //Row End
	 LCD_Write_Cmd(0x22);
	 delay1ms(40);		
	}
void LCD_SetArea( U8 x0, U8 y0, U8 x1, U8 y1 )
   {
	//Set GRAM Area
	LCD_Reg_Set(0x02,0x00);
	LCD_Reg_Set(0x03,y0); //Column Start
	LCD_Reg_Set(0x04,0x00);
	LCD_Reg_Set(0x05,y1); //Column End
	LCD_Reg_Set(0x06,0x00);
	LCD_Reg_Set(0x07,x0); //Row Start
	LCD_Reg_Set(0x08,0x00);
	LCD_Reg_Set(0x09,x1); //Row End
	LCD_Write_Cmd(0x22);	//使能数据有效
	}
void LCD_Rectangle( unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned int Data )
   {
	int x,y;
	LCD_SetArea(x0,y0,x1,y1);			
	for(x=0;x<=x1-x0;x++)
		for(y=0;y<=y1-y0;y++) {		    
			LCD_Write_Data( Data>>8 );
			LCD_Write_Data( Data );
			}
	}
void LCD_Clear_Screen( unsigned int Data )
   {
	LCD_Rectangle(0,0,219,175, Data );
	}
void LCD_SetPixel(unsigned short x, unsigned short y, unsigned int color)
    {
	LCD_SetArea(x,y,x,y);	
	LCD_Write_Data( color>>8 );
	LCD_Write_Data( color );
	}
#include "8X16.h"	//英文字符
void LCD_PutChar8x16(unsigned short x, unsigned short y, char c, unsigned int f, unsigned int b)
    {
	register unsigned int i,j;
	LCD_SetArea(x,y,x+16-1,y+16-1);
	for(i=0; i<16;i++) {
		unsigned char m=Font8x16[c*16+i];
		for(j=0;j<8;j++) {
			if((m&0x80)==0x80) {
				LCD_Write_Data(f>>8);
				LCD_Write_Data(f);
				}
			else {
				LCD_Write_Data(b>>8);
				LCD_Write_Data(b);
				}
			m<<=1;
			}
		}
	}

#include "GB1616.h"	//16*16汉字字模
void PutGB1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int f,unsigned int b)
    {
	unsigned int i,j,k;
	LCD_SetArea(x, y, x+16-1, y+16-1);
	for (k=0;k<8;k++) {
	  if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1])){ 
    	for(i=0;i<32;i++) {
		  unsigned short m=codeGB_16[k].Msk[i];
		  for(j=0;j<8;j++) {
			if((m&0x80)==0x80) {
				LCD_Write_Data(f>>8);
				LCD_Write_Data(f);
				}
			else {
				LCD_Write_Data(b>>8);
				LCD_Write_Data(b);
				}
			m<<=1;
			} 
		  }
		}  
	  }	
	}

void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int f, unsigned int b) 
	{
		LCD_PutChar8x16( x, y, c, f, b );
	}

void LCD_PutString(unsigned short x, unsigned short y, char *s, unsigned int f, unsigned int b)
 //f:前景色 b：背景色
{
	register unsigned char l=0;
	while(*s) {
		if( *s < 0x80 ) {
			LCD_PutChar(x+l*8,y,*s,f,b);
			s++;l++;
			}
		else {
			PutGB1616(x+l*8,y,(unsigned char *)s,f,b);
			s+=2;l+=2;
			}
		}
	}
void LCD_BMP( U8 x0, U8 y0, U8 x1, U8 y1 , U8 * Bmp )
    {
	S16 i,j;
	LCD_SetArea(x0,y0,x1,y1);   
	for( i =0 ; i <= x1 - x0; i++ )
		for( j = 0; j <= y1 - y0; j++ ) {
			U16* PixP =  (U16*)(Bmp+0x46+i*(y1-y0+2)*2);
			U16 Pix = *(PixP+j);
			LCD_Write_Data(Pix>>8);
			LCD_Write_Data(Pix);
			}
	}




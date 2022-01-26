#include <320x240_lcd_ssd1963.h>

//****************************************************************************
//              传输颜色数据到BUFFER 24bit to 18 bit
//****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((c) & 0x00fc0000) >> 6) |               \
                                 (((c) & 0x0000fc00) >> 4) |               \
                                 (((c) & 0x000000fc) >> 2))
//****************************************************************************
unsigned short  HDP=319;  //Hsync Display period   320-1
unsigned short  HT=408;	  //Horizontal total period = (HT + 1) pixels
unsigned short  HPS=68;	  //Horizontal Sync Pulse Start Position = (HPS + 1) pixels	 		 
unsigned short  LPS=1;	  //Horizontal Display Period Start Position = LPS pixels		
unsigned char   HPW=38;	 //Horizontal Sync Pulse Width = (HPW + 1) pixels	

unsigned short  VDP=239;
unsigned short  VT=261;	//Vertical Total = (VT + 1) lines
unsigned short  VPS=18; //Vertical Sync Pulse Start Position = VPS lines					  
unsigned short  FPS=1; //Vertical Display Period Start Position = FPS lines				  
unsigned char   VPW=15;//Vertical Sync Pulse Width = (VPW + 1) lines

//************************************************************************

//写命令
void LCD_WRITE_CMD(unsigned char c)
{
    
	 LCD_RS_L
    
  GPIOPinWrite(TFT_DATA_PORT,TFT_DATA,c);
	 
	 LCD_WR_L
	 LCD_WR_H 
    
  LCD_RS_H
 	 
}

//写数据
void LCD_WRITE_DATA(unsigned char d)
{
    
	 LCD_RS_H
	 GPIOPinWrite(TFT_DATA_PORT,TFT_DATA,d);
  LCD_WR_L
  LCD_WR_H
    
  LCD_RS_L
     
}

//写入颜色数据到GRAM
void LCD_WRITE_COLOR(unsigned long color)//RGB666 color
{
  unsigned char r,g,b;
  r=color>>10;
  g=color>>4;
  b=color<<2;
  
	 LCD_RS_H 
	 GPIOPinWrite(TFT_DATA_PORT,TFT_DATA,r);
  LCD_WR_L
  LCD_WR_H
    
 
	 GPIOPinWrite(TFT_DATA_PORT,TFT_DATA,g);
  LCD_WR_L
  LCD_WR_H
    
	 GPIOPinWrite(TFT_DATA_PORT,TFT_DATA,b);
  LCD_WR_L
  LCD_WR_H
    
  LCD_RS_L

}


void LCD_Init(void)
{
//使能lcd控制和数据引脚 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

//设置lcd控制和数据引脚方向
GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,TFT_RST_PIN|TFT_RS_PIN \
                      |TFT_CS_PIN|TFT_WR_PIN);

GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,TFT_RD_PIN);

GPIOPinTypeGPIOOutput(TFT_DATA_PORT,TFT_DATA);

//********************************************************************
//                          开始lcd初始化
//********************************************************************

 LCD_CS_L
	LCD_RST_L
SysCtlDelay(100*(TheSysClock/3000));
	LCD_RST_H
SysCtlDelay(100*(TheSysClock/3000));
 LCD_RD_H

	LCD_WRITE_CMD(0xE2);	//PLL multiplier, set PLL clock to 120M
	LCD_WRITE_DATA(0x23);	    //N=0x36 for 6.5M, 0x23 for 10M crystal
	LCD_WRITE_DATA(0x02);
	LCD_WRITE_DATA(0x04);
 
	LCD_WRITE_CMD(0xE0);  // PLL enable
	LCD_WRITE_DATA(0x01);
SysCtlDelay(100*(TheSysClock/3000));
 
	LCD_WRITE_CMD(0xE0);
	LCD_WRITE_DATA(0x03);  //Use PLL output as system clock 
SysCtlDelay(100*(TheSysClock/3000));

	LCD_WRITE_CMD(0x01);  // software reset
SysCtlDelay(100*(TheSysClock/3000));

	LCD_WRITE_CMD(0xE6);	//PLL setting for PCLK, depends on resolution
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0xd9);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xB0);	//LCD SPECIFICATION
	//LCD_WRITE_DATA(0x0004);	// 18bit RGB   Data latch in faling edge
//	LCD_WRITE_DATA(0x0024); 	//24bit	 Data latch in faling edge
	LCD_WRITE_DATA(0x20); 	//24bit	 Data latch in rising edge
	LCD_WRITE_DATA(0x00);
 
	LCD_WRITE_DATA((HDP>>8)&0XFF);  //Set HDP
	LCD_WRITE_DATA(HDP&0XFF);
 
 LCD_WRITE_DATA((VDP>>8)&0XFF);  //Set VDP
	LCD_WRITE_DATA(VDP&0XFF);
 
 LCD_WRITE_DATA(0x00);
 
SysCtlDelay(100*(TheSysClock/3000));

	LCD_WRITE_CMD(0xB4);	//HSYNC
	LCD_WRITE_DATA((HT>>8)&0XFF);  //Set HT
	LCD_WRITE_DATA(HT&0X00FF);
	LCD_WRITE_DATA((HPS>>8)&0XFF);  //Set HPS
	LCD_WRITE_DATA(HPS&0XFF);
	LCD_WRITE_DATA(HPW);			   //Set HPW
	LCD_WRITE_DATA((LPS>>8)&0XFF);  //SetLPS
	LCD_WRITE_DATA(LPS&0XFF);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xB6);	//VSYNC
	LCD_WRITE_DATA((VT>>8)&0XFF);   //Set VT
	LCD_WRITE_DATA(VT&0XFF);
	LCD_WRITE_DATA((VPS>>8)&0XFF);  //Set VPS
	LCD_WRITE_DATA(VPS&0XFF);
	LCD_WRITE_DATA(VPW);			   //Set VPW
	LCD_WRITE_DATA((FPS>>8)&0XFF);  //Set FPS
	LCD_WRITE_DATA(FPS&0XFF);


	LCD_WRITE_CMD(0x36); //rotation
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xF0); //pixel data interface  
 LCD_WRITE_DATA(0x00); //8bit three timers rgb666
						 // ==	16-bit (565 format)	*/

	 
SysCtlDelay(100*(TheSysClock/3000));


	LCD_WRITE_CMD(0x29); //display on
	//----------------------------------------//
	LCD_WRITE_CMD(0xBE); //set PWM for B/L
	LCD_WRITE_DATA(0x06); //pwm周期
	LCD_WRITE_DATA(0xff); //pwm占空比，调节背光亮度
	LCD_WRITE_DATA(0x01); 
	LCD_WRITE_DATA(0xf0);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

	LCD_WRITE_CMD(0xd0); 	 //Set DBC Configuration
	LCD_WRITE_DATA(0x0d);

    //----------LCD RESET---GPIO0-------------------//
	LCD_WRITE_CMD(0xB8);
	LCD_WRITE_DATA(0x00);    //GPIO3=input, GPIO[2:0]=output
	LCD_WRITE_DATA(0x01);    //GPIO0 normal

	LCD_WRITE_CMD(0xBA);//set gpio value
	LCD_WRITE_DATA(0x00);    
 
}

void SSD1963_SetBacklight(unsigned char Brightness)
{

	LCD_WRITE_CMD(0xBE); //set PWM for B/L
	LCD_WRITE_DATA(0x06); //pwm周期
	LCD_WRITE_DATA(Brightness); //pwm占空比，调节背光亮度
	LCD_WRITE_DATA(0x01); 
	LCD_WRITE_DATA(0xf0);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x00);

}



void LCDClearScreen(unsigned long color)
{
    unsigned short l=240,w;
 LCD_WRITE_CMD(0x2A);	
	LCD_WRITE_DATA(0);	    
	LCD_WRITE_DATA(0);
	LCD_WRITE_DATA(HDP>>8);	    
	LCD_WRITE_DATA(HDP&0xff);
 
 LCD_WRITE_CMD(0x2b);	
	LCD_WRITE_DATA(0);	    
	LCD_WRITE_DATA(0);
	LCD_WRITE_DATA(VDP>>8);	    
	LCD_WRITE_DATA(VDP&0xff);
 
	LCD_WRITE_CMD(0x2c); //准备写数据

 
	 while(l--)
	{
	    for(w=0;w<320;w++)
		{    
       	LCD_WRITE_COLOR(color);
         
		}
	}
}

//****************************************************************************
//在屏幕上画一个点
//****************************************************************************
void LCDPixelDraw(void *pvDisplayData,long lX,long lY, unsigned long ulValue)
{

 //设置x坐标 
 LCD_WRITE_CMD(0x2A);	
	LCD_WRITE_DATA((lX>>8)&0xff);	    
	LCD_WRITE_DATA(lX);
	LCD_WRITE_DATA(((lX)>>8)&0xff);	    
	LCD_WRITE_DATA(lX);

//设置y坐标 
 LCD_WRITE_CMD(0x2b);	
	LCD_WRITE_DATA((lY>>8)&0xff);	    
	LCD_WRITE_DATA(lY);
	LCD_WRITE_DATA((lY>>8)&0xff);	    
	LCD_WRITE_DATA(lY);              
 
 LCD_WRITE_CMD(0x2c); //准备写数据  
 
 LCD_WRITE_COLOR(ulValue);


}


//**************************************************************************
//在屏幕上画水平线
//**************************************************************************

void LCDlineDrawH(void *pvDisplayData,long lX1, long lX2,
                   long lY, unsigned long ulValue)
{
  
unsigned long count;  
unsigned long xsize=lX2-lX1+1;

  
 //设置x坐标 
 LCD_WRITE_CMD(0x2A);	
	LCD_WRITE_DATA((lX1>>8)&0xff);	    
	LCD_WRITE_DATA(lX1);
	LCD_WRITE_DATA((lX2>>8)&0xff);	    
	LCD_WRITE_DATA(lX2);

//设置y坐标 
 LCD_WRITE_CMD(0x2b);	
	LCD_WRITE_DATA((lY>>8)&0xff);	    
	LCD_WRITE_DATA(lY);
	LCD_WRITE_DATA((lY>>8)&0xff);	    
	LCD_WRITE_DATA(lY);  
 
  LCD_WRITE_CMD(0x2c); //准备写数据  
  
  for(count=0;count<xsize;count++)
  {
  
     LCD_WRITE_COLOR(ulValue);
  
  }


}

//**************************************************************************
//在屏幕上画竖直线
//**************************************************************************


void LCDlineDrawV(void *pvDisplayData,long lX, long lY1,long lY2, unsigned long ulValue)
{

unsigned long count;  
unsigned long ysize=lY2-lY1+1;

	LCD_WRITE_CMD(0x36); //rotation
	LCD_WRITE_DATA(0x20);//page increase mode
  
 //设置x坐标 
 LCD_WRITE_CMD(0x2A);	
	LCD_WRITE_DATA((lX>>8)&0xff);	    
	LCD_WRITE_DATA(lX);
	LCD_WRITE_DATA(((lX>>8))&0xff);	    
	LCD_WRITE_DATA(lX);

//设置y坐标 
 LCD_WRITE_CMD(0x2b);	
	LCD_WRITE_DATA((lY1>>8)&0xff);	    
	LCD_WRITE_DATA(lY1);
	LCD_WRITE_DATA((lY2>>8)&0xff);	    
	LCD_WRITE_DATA(lY2);  
 
  LCD_WRITE_CMD(0x2c); //准备写数据  
  
  for(count=0;count<ysize;count++)
  {
  
     LCD_WRITE_COLOR(ulValue);
  
  }
	LCD_WRITE_CMD(0x36); //rotation
	LCD_WRITE_DATA(0x00);//Column increase mode

}

//*****************************************************************************
//画一个填充的矩形
//*****************************************************************************
void LCDRectFill(void *pvDisplayData, const tRectangle *pRect,
                                  unsigned long ulValue)
{  
   
unsigned long count;  
unsigned long pixelsize=((pRect->sXMax)-(pRect->sXMin)+1)\
                        *((pRect->sYMax)-(pRect->sYMin)+1);



 //设置x坐标 
 LCD_WRITE_CMD(0x2A);	
	LCD_WRITE_DATA(((pRect->sXMin)>>8)&0xff);	    
	LCD_WRITE_DATA(pRect->sXMin);
	LCD_WRITE_DATA(((pRect->sXMax)>>8)&0xff);	    
	LCD_WRITE_DATA(pRect->sXMax);

//设置y坐标 
 LCD_WRITE_CMD(0x2b);	
	LCD_WRITE_DATA(((pRect->sYMin)>>8)&0xff);	    
	LCD_WRITE_DATA(pRect->sYMin);
	LCD_WRITE_DATA(((pRect->sYMax)>>8)&0xff);	    
	LCD_WRITE_DATA(pRect->sYMax);  
 
   LCD_WRITE_CMD(0x2c); //准备写数据  
 
   for(count=0;count<pixelsize;count++)
  {
  
     LCD_WRITE_COLOR(ulValue);
  
  }
 
  

}


//**************************************************************************
//在屏幕上画点序列
//**************************************************************************
static void
LCDPixelDrawMultiple(void *pvDisplayData, long lX,
                                           long lY, long lX0, long lCount,
                                           long lBPP,
                                           const unsigned char *pucData,
                                           const unsigned char *pucPalette)
{

unsigned long ulByte;


 //设置x坐标 
 LCD_WRITE_CMD(0x2A);	
	LCD_WRITE_DATA((lX>>8)&0xff);	    
	LCD_WRITE_DATA(lX);
	LCD_WRITE_DATA(((HDP)>>8)&0xff);	    
	LCD_WRITE_DATA(HDP);

//设置y坐标 
 LCD_WRITE_CMD(0x2b);	
	LCD_WRITE_DATA((lY>>8)&0xff);	    
	LCD_WRITE_DATA(lY);
	LCD_WRITE_DATA((lY>>8)&0xff);	    
	LCD_WRITE_DATA(lY>>8);              

 
LCD_WRITE_CMD(0x2c); //准备写数据  

 //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(lBPP)
    {
        //
        // The pixel data is in 1 bit per pixel format.
        //
        case 1:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount)
            {
                //
                // Get the next byte of image data.
                //
                ulByte = *pucData++;

                //
                // Loop through the pixels in this byte of image data.
                //
                for(; (lX0 < 8) && lCount; lX0++, lCount--)
                {
                    //
                    // Draw this pixel in the appropriate color.
                    //
                 LCD_WRITE_COLOR(DPYCOLORTRANSLATE(((unsigned long *)pucPalette)\
                                 [(ulByte >>(7 - lX0)) & 1]));
                }

                //
                // Start at the beginning of the next byte of image data.
                //
                lX0 = 0;
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 4 bit per pixel format.
        //
        case 4:
        {
            //
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            //
            switch(lX0 & 1)
            {
                case 0:
                    while(lCount)
                    {
                        //
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the
                        // palette.
                        //
                        ulByte = (*pucData >> 4) * 3;
                        ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                  0x00ffffff);

                        //
                        // Translate this palette entry and write it to the
                        // screen.
                        //
                       LCD_WRITE_COLOR(DPYCOLORTRANSLATE(ulByte));

                        //
                        // Decrement the count of pixels to draw.
                        //
                        lCount--;

                        //
                        // See if there is another pixel to draw.
                        //
                        if(lCount)
                        {
                case 1:
                            //
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette.
                            //
                            ulByte = (*pucData++ & 15) * 3;
                            ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                      0x00ffffff);

                            //
                            // Translate this palette entry and write it to the
                            // screen.
                            //
                           LCD_WRITE_COLOR(DPYCOLORTRANSLATE(ulByte));
                                       

                            //
                            // Decrement the count of pixels to draw.
                            //
                            lCount--;
                        }
                    }
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 8 bit per pixel format.
        //
        case 8:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                ulByte = *pucData++ * 3;
                ulByte = *(unsigned long *)(pucPalette + ulByte) & 0x00ffffff;

                //
                // Translate this palette entry and write it to the screen.
                //
                  LCD_WRITE_COLOR(DPYCOLORTRANSLATE(ulByte));
                
            }

            //
            // The image data has been drawn.
            //
            break;
        }
    }


}

//****************************************************************************
//更新BUFFER数据到DISPLAYBUFFER
//****************************************************************************
static void
LCDFlush(void *pvDisplayData)
{
    //
    // There is nothing to be done.
    //
}

static unsigned long
LCDColorTranslate(void *pvDisplayData,
                                        unsigned long ulValue)
{
    //
    // Translate from a 24-bit RGB color to a 5-6-5 RGB color.
    //
    return(DPYCOLORTRANSLATE(ulValue));
}


//**************************************************************************
//define the cliping display area of the LCD
//***************************************************************************
 tRectangle DisplayArea={0,0,240,320 };
//***************************************************************************


//***************************************************************************

//the basic Parameter of the LCD display,the basic draw fuction   
// pointers are includeed as well

//***************************************************************************

const tDisplay MyDisplay=
{
 sizeof(tDisplay),
 0,
 320,       // LCD width
 240,       // LCD height
 LCDPixelDraw,//画点函数
LCDPixelDrawMultiple, //像素点子像素调色函数
LCDlineDrawH,
LCDlineDrawV,
LCDRectFill,
LCDColorTranslate,
LCDFlush,
};


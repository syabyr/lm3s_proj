#include <hw_types.h>
#include <hw_memmap.h>
#include  <sysctl.h>
#include <gpio.h>
#include <grlib.h>
extern unsigned long TheSysClock;


//****************************************************************************
//                              LCD接口定义
//****************************************************************************

#define  TFT_RST                GPIO_PORTB_BASE
#define  TFT_RST_PIN            GPIO_PIN_0

#define  TFT_RS                 GPIO_PORTB_BASE
#define  TFT_RS_PIN             GPIO_PIN_1

#define  TFT_CS                 GPIO_PORTB_BASE 
#define  TFT_CS_PIN             GPIO_PIN_4

#define  TFT_WR                 GPIO_PORTB_BASE
#define  TFT_WR_PIN             GPIO_PIN_5

#define  TFT_RD                 GPIO_PORTE_BASE
#define  TFT_RD_PIN             GPIO_PIN_0


#define  LCD_RST_H              GPIOPinWrite(TFT_RST, TFT_RST_PIN, 0xff); 
#define  LCD_RST_L              GPIOPinWrite(TFT_RST, TFT_RST_PIN, 0);

#define  LCD_RD_H               GPIOPinWrite(TFT_RD, TFT_RD_PIN, 0xff); 
#define  LCD_RD_L               GPIOPinWrite(TFT_RD, TFT_RD_PIN, 0);

#define  LCD_WR_H               GPIOPinWrite(TFT_WR, TFT_WR_PIN, 0xff); 
#define  LCD_WR_L               GPIOPinWrite(TFT_WR, TFT_WR_PIN, 0);

#define  LCD_RS_H               GPIOPinWrite(TFT_RS, TFT_RS_PIN, 0xff); 
#define  LCD_RS_L               GPIOPinWrite(TFT_RS, TFT_RS_PIN, 0);

#define  LCD_CS_H               GPIOPinWrite(TFT_CS, TFT_CS_PIN, 0xff); 
#define  LCD_CS_L               GPIOPinWrite(TFT_CS, TFT_CS_PIN, 0);

#define  TFT_DATA_PORT          GPIO_PORTD_BASE
#define  TFT_DATA               0x000000FF

//************************************************************************


//***********************************************************************
//                              LCD函数声明

extern void LCD_Init(void);

extern void goto_start(void);

extern void SSD1963_SetBacklight(unsigned char Brightness);

extern void LCDClearScreen(unsigned long color);

extern void SetPosition(unsigned long ulX,unsigned long ulY);

extern void LCDPixelDraw(void *pvDisplayData,long lX,
                         long lY, unsigned long ulValue);

extern void LCDlineDrawH(void *pvDisplayData,long lX1, long lX2,
                   long lY, unsigned long ulValue);

extern void LCDlineDrawV(void *pvDisplayData,long lX, long lY1,
                  long lY2, unsigned long ulValue);
extern void LCDRectFill(void *pvDisplayData, const tRectangle *pRect,
                                  unsigned long ulValue);

extern const tDisplay MyDisplay;

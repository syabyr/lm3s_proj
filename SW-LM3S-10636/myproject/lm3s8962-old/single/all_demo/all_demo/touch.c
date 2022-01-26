#include <hw_types.h>
#include <hw_memmap.h>
#include  <sysctl.h>
#include <interrupt.h>
#include <hw_ints.h>
#include <gpio.h>
#include <ssi.h>
#include <touch.h>
#include <grlib.h>
#include <widget.h>

extern unsigned long TheSysClock;
unsigned long dump;
unsigned long temp;

#define  CS   GPIO_PIN_4
#define  DCLK GPIO_PIN_5
#define  DOUT GPIO_PIN_6
#define  DIN  GPIO_PIN_7


#define SPI_CS_H GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,0xff)
#define SPI_CS_L GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,0x00)

void start();
void ads7843_wr(unsigned char num);
unsigned int ads7843_rd();
void getpoint(void);

//*****************************************************************************
//
// The current state of the touch screen driver's state machine.  This is used
// to cycle the touch screen interface through the powering sequence required
// to read the two axes of the surface.
//
//*****************************************************************************

#define TS_STATE_INIT           0
#define TS_STATE_READ_X         1
#define TS_STATE_READ_Y         2
#define TS_STATE_SKIP_X         3
#define TS_STATE_SKIP_Y         4

//*****************************************************************************
//
// The most recent raw ADC reading for the X position on the screen.  This
// value is not affected by the selected screen orientation.
//
//*****************************************************************************
 volatile unsigned long g_sTouchX;

//*****************************************************************************
//
// The most recent raw ADC reading for the Y position on the screen.  This
// value is not affected by the selected screen orientation.
//
//*****************************************************************************
volatile unsigned long  g_sTouchY;

//*****************************************************************************
//
// A pointer to the function to receive messages from the touch screen driver
// when events occur on the touch screen (debounced presses, movement while
// pressed, and debounced releases).
//
//*****************************************************************************
 long (*g_pfnTSHandler)(unsigned long ulMessage, long lX, long lY);

//*****************************************************************************

//the last state of the panel.

unsigned long l_state=0;

#define STATE_PEN_UP   0
#define STATE_PEN_DOWN 1
#define STATE_PEN_MOVE 2


void TouchScreenInit(void)
{

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);//enable the gpioa

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);//enable the gpioa

GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,DCLK|DIN|CS);
                      
GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,DOUT);

SPI_CS_H;

GPIOPinWrite(GPIO_PORTC_BASE,DCLK,0x00); 

GPIOPinWrite(GPIO_PORTC_BASE,DIN,0x00); 


//the goio is connected to the touch screen driver tsc2046's
//penirq,when it goes low dedicaded that the penal has been
//prssed and there shoud be a read operation by the host mcu
GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_6);

GPIOIntTypeSet(GPIO_PORTA_BASE,GPIO_PIN_6,GPIO_LOW_LEVEL);

GPIOPinIntEnable(GPIO_PORTA_BASE,GPIO_PIN_6);

IntEnable(INT_GPIOA);

IntMasterEnable();

g_pfnTSHandler = 0;


}

//*****************************************************************************
//
//! This function set the touch screen callback function that
//! can sent the massage to the graph lib to dealing with the event.
//! \return None.
//
//*****************************************************************************

void
TouchScreenCallbackSet(long (*pfnCallback)(unsigned long ulMessage, long lX,
                                           long lY))
{
    //
    // Save the pointer to the callback function.
    //
    g_pfnTSHandler = pfnCallback;
    
    
}

//*****************************************************************************
//
//! Debounces presses of the touch screen.
//!
//! This function is called when a new X/Y sample pair has been captured in
//! order to perform debouncing of the touch screen.
//!
//! \return None.
//
//*****************************************************************************
static void TouchScreenDebouncer(void)
{

  unsigned long ulX,ulY;

 
  
    ulX=319*(g_sTouchX-200)/3600; 
    
    ulY=239*(g_sTouchY-300)/3300; 
      
   //see if the Sample is vaild.
   
    if((ulX>3&&ulY>3)&&(ulX<316&&ulY<236))
    {
      switch(l_state)
      {
      
        case STATE_PEN_DOWN :
          {
            
                 
           if(GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_6)==0)
           {
           
           getpoint();
           
           ulX=319*(g_sTouchX-200)/3600; 
           
           ulY=239*(g_sTouchY-300)/3300;
           
              if(((ulX>3)&&(ulY>3))&&((ulX<316)&&(ulY<236)))
              {
           
                g_pfnTSHandler(WIDGET_MSG_PTR_MOVE,ulX,ulY);
                
                l_state=STATE_PEN_MOVE;
                
                WidgetMessageQueueProcess();
               
              }
           
           
           }
           
           else
           {
           
               g_pfnTSHandler(WIDGET_MSG_PTR_UP,ulX,ulY);
                
               l_state=STATE_PEN_UP;
               
             //  WidgetMessageQueueProcess();
           
           }
           
 //**************************************************************           
  /*           getpoint();
           
             ulX=319*(g_sTouchX-200)/3600; 
    
             ulY=239*(g_sTouchY-300)/3300;
           
                      
              if(((ulX>3)&&(ulY>3))&&((ulX<316)&&(ulY<236)))
              {
           
                g_pfnTSHandler(WIDGET_MSG_PTR_MOVE,ulX,ulY);
                
                l_state=STATE_PEN_MOVE;
                
                WidgetMessageQueueProcess();
               
              }
              
              else
              {
              
              g_pfnTSHandler(WIDGET_MSG_PTR_UP,ulX,ulY);
              
              l_state=STATE_PEN_UP;
              

              
              }  */
//********************************************************************              
            
          }
          
        case STATE_PEN_UP:
          {
          
          g_pfnTSHandler(WIDGET_MSG_PTR_DOWN,ulX,ulY);
          
          l_state=STATE_PEN_DOWN;
//*****************************************************************          
        /*  
           getpoint();
           
           ulX=319*(g_sTouchX-200)/3600; 
    
           ulY=239*(g_sTouchY-300)/3300;
           
                      
              if(((ulX>3)&&(ulY>3))&&((ulX<316)&&(ulY<236)))
              {
           
                g_pfnTSHandler(WIDGET_MSG_PTR_MOVE,ulX,ulY);
                
                l_state=STATE_PEN_MOVE;
                
              }  */
//*******************************************************************          
          }
          
      
        case STATE_PEN_MOVE:
          {
            
           if(GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_6)==0)
           {
           
           getpoint();
           
           ulX=319*(g_sTouchX-200)/3600; 
           
           ulY=239*(g_sTouchY-300)/3300;
           
              if(((ulX>3)&&(ulY>3))&&((ulX<316)&&(ulY<236)))
              {
           
                g_pfnTSHandler(WIDGET_MSG_PTR_MOVE,ulX,ulY);
                
                l_state=STATE_PEN_MOVE;
                
                WidgetMessageQueueProcess();
               
              }
           
           
           }
           
           else
           {
           
               g_pfnTSHandler(WIDGET_MSG_PTR_UP,ulX,ulY);
                
               l_state=STATE_PEN_UP;
               
             //  WidgetMessageQueueProcess();
           
           }
           

          }
          
      }
  
    
   } 
  

}


void TouchScreenIntHandler(void)
{

unsigned long ulX,ulY;

GPIOPinIntClear(GPIO_PORTA_BASE,GPIO_PIN_6);
GPIOPinIntDisable(GPIO_PORTA_BASE,GPIO_PIN_6);

getpoint(); //get the touch point on the panal

ulX=319*(g_sTouchX-200)/3600;          
ulY=239*(g_sTouchY-300)/3300;

if(((ulX>3)&&(ulY>3))&&((ulX<316)&&(ulY<236)))
{
           
g_pfnTSHandler(WIDGET_MSG_PTR_DOWN,ulX,ulY);
WidgetMessageQueueProcess();
g_pfnTSHandler(WIDGET_MSG_PTR_UP,ulX,ulY);
WidgetMessageQueueProcess();
SysCtlDelay(500*(TheSysClock/3000));
           
}

while(GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_6)==0)
{
  
getpoint(); //get the touch point on the panal

ulX=319*(g_sTouchX-200)/3600;          
ulY=239*(g_sTouchY-300)/3300;

if(((ulX>3)&&(ulY>3))&&((ulX<316)&&(ulY<236)))
{
           
g_pfnTSHandler(WIDGET_MSG_PTR_DOWN,ulX,ulY);

g_pfnTSHandler(WIDGET_MSG_PTR_MOVE,ulX,ulY);

WidgetMessageQueueProcess();

           
}
 

}


//TouchScreenDebouncer();
 IntPendClear(INT_GPIOA);
 GPIOPinIntClear(GPIO_PORTA_BASE,GPIO_PIN_6);
 GPIOPinIntEnable(GPIO_PORTA_BASE,GPIO_PIN_6);

}


//*****************************************************************************
//
//! Sets the callback function for touch screen events.
//!
//! \param pfnCallback is a pointer to the function to be called when touch
//! screen events occur.
//!
//! This function sets the address of the function to be called when touch
//! screen events occur.  The events that are recognized are the screen being
//! touched (``pen down''), the touch position moving while the screen is
//! touched (``pen move''), and the screen no longer being touched (``pen
//! up'').
//!
//! \return None.
//
//*****************************************************************************

//*****************************************************************************
void start() //SPI开始
{
  
GPIOPinWrite(GPIO_PORTC_BASE,DCLK,0x00); 
GPIOPinWrite(GPIO_PORTC_BASE,DIN,0X00);

}
//*****************************************************************************
void ads7843_wr(unsigned char num) //SPI写数据
{
unsigned char i=0;
GPIOPinWrite(GPIO_PORTC_BASE,DCLK,0X00); 
for(i=0;i<8;i++)
   {
  
   GPIOPinWrite(GPIO_PORTC_BASE,DIN,((num&0x80)==0x80) ? 0XFF:0X00); 
   GPIOPinWrite(GPIO_PORTC_BASE,DCLK,0X00); 
  // SysCtlDelay(TheSysClock/6000);
    SysCtlDelay(500);
   GPIOPinWrite(GPIO_PORTC_BASE,DCLK,0xff); 
 //  SysCtlDelay(TheSysClock/6000);
    SysCtlDelay(500);
   num<<=1;
   }
}
//*****************************************************************************
unsigned int ads7843_rd() //SPI 读数据
{
unsigned char i=0;
unsigned int Num=0;
for(i=0;i<12;i++)
   {
     
   GPIOPinWrite(GPIO_PORTC_BASE,DCLK,0xff); 
 //  SysCtlDelay(TheSysClock/6000);
   SysCtlDelay(500);
   GPIOPinWrite(GPIO_PORTC_BASE,DCLK,0x00); 
//   SysCtlDelay(TheSysClock/6000);
   SysCtlDelay(500);
   if(GPIOPinRead(GPIO_PORTC_BASE,DOUT)) 
   Num++;
   Num<<=1;
   }
return(Num);
}

void getpoint(void)
{
unsigned int i;
unsigned long TouchX[2];
unsigned long TouchY[2];

SPI_CS_L;
//中断后延时以消除抖动，使得采样数据更准确
//SysCtlDelay(5*(TheSysClock/3000)); 

start();//启动SPI    

//SysCtlDelay(1*(TheSysClock/3000));

xlabel:
for(i=0;i<2;i++)
{
ads7843_wr(0x90); //送控制字 10010000 即用差分方式读X坐标
SysCtlDelay(1*(TheSysClock/3000));
TouchX[i]=ads7843_rd();
}
if(TouchX[0]>TouchX[1]) 
{

i=TouchX[0];
TouchX[0]=TouchX[1];
TouchX[1]=i;

}

if((TouchX[1]-TouchX[0])>10) goto xlabel;

g_sTouchX=TouchX[1];


Ylabel:
for(i=0;i<2;i++)
{
ads7843_wr(0xD0); //送控制字 11010000 即用差分方式读Y坐标 
SysCtlDelay(1*(TheSysClock/3000));
TouchY[i]=ads7843_rd();

}
if(TouchY[0]>TouchY[1]) 
{

i=TouchY[0];
TouchY[0]=TouchY[1];
TouchY[1]=i;

}

if((TouchY[1]-TouchY[0])>10) goto Ylabel;

g_sTouchY=TouchY[1];

SPI_CS_H;

}
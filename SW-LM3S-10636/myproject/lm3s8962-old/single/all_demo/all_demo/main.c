#include "hw_types.h"
#include "hw_memmap.h"
#include "sysctl.h"
#include "320x240_lcd_ssd1963.h"
#include "touch.h"
#include "grlib.h"
#include "widget.h"
#include "canvas.h"
#include "checkbox.h"
#include "container.h"
#include "imgbutton.h"
#include "listbox.h"
#include "pushbutton.h"
#include "radiobutton.h"
#include "slider.h"
#include "images.h"
#include "appfunc.h"
#include "sound.h"
#include "stdlib.h"
#include "stdio.h"
tContext MyContext;
tRectangle MyRect;
unsigned long TheSysClock=12000000UL;
//*****************************************************************************
//
// The panel that is currently being displayed.
//
//*****************************************************************************
unsigned long g_ulPanel=0;


//*****************************************************************************
//
// The canvas widget acting as the background to the display.
//
//*****************************************************************************
Canvas(g_sBackground, WIDGET_ROOT, 0, &g_sHeading,
       &MyDisplay, 1, 24, 318, (239-75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE , ClrBlack, 
       ClrLime, 0, 0, 0, 0, 0);
//*****************************************************************************
//
// The canvas widget acting as the app panel background to the display.
//
//*****************************************************************************
Canvas(g_sAppbackground, WIDGET_ROOT, 0, &LED,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE, ClrBlack, 
       ClrLime, 0, 0, 0, 0, 0);
//*****************************************************************************
//
// The heading containing the application title.
//
//*****************************************************************************
Canvas(g_sHeading, &g_sBackground, 0, &g_sIntroduction,
       &MyDisplay, 1, 1, 319, 23,
       (CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT\
        |CANVAS_STYLE_IMG),
       ClrDarkBlue, ClrWhite, ClrYellow, &g_sFontCm20, "CDUESTC", bianzhi, 0);
//*****************************************************************************
//
// The introduction containing the introduction of this demo
//
//*****************************************************************************
Canvas(g_sIntroduction, &g_sHeading, 0, 0, &MyDisplay,
          1, 24, 319, (239-24-50),  CTR_STYLE_FILL
           |CANVAS_STYLE_TEXT|CANVAS_STYLE_APP_DRAWN,
          ClrBlack, 0, ClrWhite,&g_sFontCm20,0,0,OnIntroPaint);

//*****************************************************************************
//
// The buttons and text across the bottom of the screen.
//
//*****************************************************************************

RectangularButton(g_sNext, &g_sBackground, 0, 0,
                  &MyDisplay, 270, 190, 50, 50,
                  PB_STYLE_IMG | PB_STYLE_TEXT, ClrBlack, ClrBlack, 0,
                  ClrSilver, &g_sFontCm20, "+", g_pucBlue50x50,
                  g_pucBlue50x50Press, 0, 0, OnNext);

Canvas(g_sTitle, &g_sBackground, &g_sNext, 0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |
       CANVAS_STYLE_TEXT_OPAQUE), 0, 0, ClrSilver, &g_sFontCm20, 
       "Intorduction", 0, 0);

RectangularButton(g_sPrevious, &g_sBackground, &g_sTitle, 0,
                  &MyDisplay, 0, 190, 50, 50,
                  PB_STYLE_IMG | PB_STYLE_TEXT|PB_STYLE_IMG
                  |IB_STYLE_AUTO_REPEAT,ClrBlack, ClrBlack, 0,
                  ClrSilver, &g_sFontCm20, "-", g_pucBlue50x50,
                  g_pucBlue50x50Press, 0, 1,OnPrevious);


int main()
{

  SysCtlClockSet(SYSCTL_XTAL_6MHZ
                 |SYSCTL_SYSDIV_8
                 |SYSCTL_OSC_MAIN
                 |SYSCTL_USE_PLL);
  
  TheSysClock=SysCtlClockGet();
  
  GrContextInit(&MyContext,&MyDisplay);
  
  SysCtlDelay(100*(TheSysClock/3000));
  
  LCD_Init();
  
  SSD1963_SetBacklight(255);
  
  TouchScreenInit();
  
  TouchScreenCallbackSet(WidgetPointerMessage);
  
  LCDClearScreen(DpyColorTranslate(&MyDisplay,ClrBlack));
  
  SoundInit();
  
  WidgetPaint(WIDGET_ROOT);
    
  WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBackground);
  
  WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sNext);
  
  g_ulPanel=INTORDUCTION;
  
    //
    // Loop forever, processing widget messages.
    //

    while(1)
    {
                                      
      //
        // Process any messages from or for the widgets.
        //

        WidgetMessageQueueProcess();
        
       

 
    }
  
  
  
}

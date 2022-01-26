#include <hw_types.h>
#include <hw_memmap.h>
#include <sysctl.h>
#include <gpio.h>
#include <pwm.h>
#include <i2c.h>
#include <hw_ints.h>
#include <interrupt.h>
#include <320x240_lcd_ssd1963.h>
#include <touch.h>
#include <grlib.h>
#include <widget.h>
#include <canvas.h>
#include <checkbox.h>
#include <container.h>
#include <imgbutton.h>
#include <listbox.h>
#include <pushbutton.h>
#include <radiobutton.h>
#include <slider.h>
#include <images.h>
#include <appfunc.h>
#include <sound.h>
#include <ustdlib.h>

//the LED define
#define LED1 GPIO_PIN_1
#define LED8 GPIO_PIN_1

extern tContext MyContext;
extern tRectangle MyRect;
extern unsigned long g_ulPanel;
extern void getpoint(void);
extern long (*g_pfnTSHandler)(unsigned long ulMessage, long lX, long lY);
//*****************************************************************************
//
// Handles presses of the previous panel button.
//
//*****************************************************************************
Canvas(g_sApptitle, 0, 0, 0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |
       CANVAS_STYLE_TEXT_OPAQUE), 0, 0, ClrSilver, &g_sFontCm20, 
       "  Show applications ", 0, 0);
//*****************************************************************************
//the applications that will be showed....
//*****************************************************************************

//the first row
RectangularButton(LED, &g_sAppbackground ,0,&BING,
                            &MyDisplay, 2, 25, 70, 50,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20, "LED", app,
                            app, 0, 0, OnLedPress);


RectangularButton(BING,&LED, 0,&GPTM,
                            &MyDisplay, 83, 25, 70, 50,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20, "BING", app,
                            app, 0, 0, OnBingPress);

RectangularButton(GPTM, &BING , 0,&PWM,
                            &MyDisplay, 166, 25, 70, 50,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20, "GPTM", app,
                            app, 0, 0, OnGptmPress);

RectangularButton(PWM, &GPTM , 0,&IIC,
                            &MyDisplay, 249, 25, 70, 50,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20, "PWM", app,
                            app, 0, 0, OnPwmPress);

//the seconed row
RectangularButton(IIC, &PWM , 0,&UART,
                            &MyDisplay, 2, 85, 70, 50,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20, "IIC", app,
                            app, 0, 0, OnIicPress);

RectangularButton(UART, &IIC , 0,&SPI,
                            &MyDisplay, 83, 85, 70, 50,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20, "UART", app,
                            app, 0, 0, OnUartPress);

RectangularButton(SPI, &UART , 0,&CAN,
                            &MyDisplay, 166, 85, 70, 50,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20, "SPI", app,
                            app, 0, 0, 0);


RectangularButton(CAN, &UART , 0,&PAINTING,
                            &MyDisplay, 249, 85, 70, 50,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20, "CAN", app,
                            app, 0, 0, 0);

//the third row
RectangularButton(PAINTING, &CAN , 0,&SETTING,
                            &MyDisplay, 7, 145, 70, 37,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20b, "Pinting", g_pucGreenSlider195x37,
                            g_pucGreenSlider195x37, 0, 0,OnPaintingPress);


RectangularButton(SETTING, &PAINTING , 0,0,
                            &MyDisplay, 245, 145, 70, 37,
                            PB_STYLE_TEXT|PB_STYLE_IMG,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm20b, "Setting", g_pucGreenSlider195x37,
                            g_pucGreenSlider195x37, 0, 0,OnSettingPress);
//******************************************************************************

//******************************************************************************
//      the led demo panel data struct
//******************************************************************************

Canvas(g_sLedbackground, 0, 0, g_psPushButtons,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE, ClrBlack, 
       ClrLime, 0, 0, 0, 0, 0);

Canvas(g_sLedtitle, 0, 0, 0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |
       CANVAS_STYLE_TEXT_OPAQUE), 0, 0, ClrSilver, &g_sFontCm20, 
       "   Led application   ", 0, 0);

tCanvasWidget g_psPushButtonIndicators[] =
{
    CanvasStruct(&g_sLedbackground, g_psPushButtonIndicators + 1, 0,
                 &MyDisplay, 25, 95, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(&g_sLedbackground, g_psPushButtonIndicators + 2, 0,
                 &MyDisplay, 85, 95, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),


    CanvasStruct(&g_sLedbackground, g_psPushButtonIndicators + 3, 0,
                 &MyDisplay, 130, 40, 185, 24,
                 CANVAS_STYLE_TEXT,0, 0, ClrSilver, &g_sFontCm20,
                 "Press the led button",
                 0, 0),
    CanvasStruct(&g_sLedbackground, g_psPushButtonIndicators + 4, 0,
                 &MyDisplay, 130, 60,185, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, 
                 "to toggle the led.",
                 0, 0),
    CanvasStruct(&g_sLedbackground, g_psPushButtonIndicators +5, 0,
                 &MyDisplay, 130, 125, 185, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20,
                 "Press the run button",
                 0, 0),
    CanvasStruct(&g_sLedbackground, 0, 0,
                 &MyDisplay, 130, 145, 185, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, 
                 "to run the led.",
                 0, 0),
};

tPushButtonWidget g_psPushButtons[] =
{
    RectangularButtonStruct(&g_sLedbackground ,g_psPushButtons + 1, 0,
                            &MyDisplay, 10, 35, 50, 50,
                            PB_STYLE_IMG | PB_STYLE_TEXT,
                            0, 0, 0, ClrSilver,
                            &g_sFontCm22, "led1", g_pucBlue50x50,
                            g_pucBlue50x50Press, 0, 0, OnButtonPress),
    
    RectangularButtonStruct(&g_sLedbackground, g_psPushButtons + 2, 0,
                            &MyDisplay, 70, 35, 50, 50,
                            PB_STYLE_IMG | PB_STYLE_TEXT,
                            0, 0, 0, ClrSilver,
                            &g_sFontCm22, "led8", g_pucBlue50x50,
                            g_pucBlue50x50Press, 0, 0, OnButtonPress),
    
    RectangularButtonStruct(&g_sLedbackground, g_psPushButtonIndicators,0,
                            &MyDisplay, 30, 120, 70, 50,
                            PB_STYLE_IMG | PB_STYLE_TEXT,
                            0, 0, 0, ClrSilver,
                            &g_sFontCm22, "run", app,
                            app, 0, 0, OnButtonPress),
    


};

#define NUM_PUSH_BUTTONS        (sizeof(g_psPushButtons) /   \
                                 sizeof(g_psPushButtons[0]))
unsigned long g_ulButtonState;


//********************************************************************

//******************************************************************************
//      the bing panel data struct
//******************************************************************************
tContainerWidget g_psRadioContainers[];
Canvas(g_sBingbackground, 0, 0,g_psRadioContainers,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE, ClrBlack, 
       ClrLime, 0, 0, 0, 0, 0);

Canvas(g_sBingtitle, 0, 0,0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |CANVAS_STYLE_APP_DRAWN
       |CANVAS_STYLE_TEXT_OPAQUE), 0, 0, ClrSilver, &g_sFontCm20, 
       "  Bing application   ", 0, BingDraw);


tCanvasWidget g_psRadioButtonIndicators[] =
{
    CanvasStruct(g_psRadioContainers, g_psRadioButtonIndicators + 1, 0,
                 &MyDisplay, 95, 62, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psRadioContainers,0, 0,
                 &MyDisplay, 95, 122, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),


};
tRadioButtonWidget g_psRadioButtons1[] =
{
    RadioButtonStruct(g_psRadioContainers, g_psRadioButtons1 + 1, 0,
                      &MyDisplay, 10, 50, 100, 45,
                      RB_STYLE_TEXT, 16, 0, ClrSilver, ClrSilver, &g_sFontCm26,
                      "On", 0, OnRadioChange),
    RadioButtonStruct(g_psRadioContainers, g_psRadioButtonIndicators, 0,
                      &MyDisplay, 10, 110, 100, 45,
                      RB_STYLE_TEXT, 16, 0, ClrSilver, ClrSilver, &g_sFontCm26,
                      "Off", 0, OnRadioChange),
  
};
#define NUM_RADIO1_BUTTONS      (sizeof(g_psRadioButtons1) /   \
                                 sizeof(g_psRadioButtons1[0]))

tContainerWidget g_psRadioContainers[] =
{
    ContainerStruct(&g_sBingbackground,0, g_psRadioButtons1,
                    &MyDisplay, 5, 27, 148, 160,
                    CTR_STYLE_OUTLINE | CTR_STYLE_TEXT, 0, ClrGray, ClrSilver,
                    &g_sFontCm20, "Bing"),

};

//******************************************************************************
//      the GPTM panel data struct
//******************************************************************************

Canvas(g_sGptmbackground, 0, 0, 0,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE, ClrBlack, 
       ClrLime, 0, 0, 0, 0, 0);

Canvas(g_sGptmtitle, 0, 0, 0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |
       CANVAS_STYLE_TEXT_OPAQUE), 0, 0, ClrSilver, &g_sFontCm20, 
       "   GPTM application   ", 0, 0);



//******************************************************************************
//      the PWM panel data struct
//******************************************************************************
struct wave 
{
unsigned char height;//波形高度
unsigned long high;//高电平像素
unsigned long low;//低电平像素

};

struct wave NWave;
struct wave BWave;


Canvas(g_sPwmbackground, 0, 0,&g_sDutycycleset,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE, ClrBlack, 
       ClrLime, 0, 0, 0, 0, 0);

Canvas(g_sPwmtitle, 0, 0,0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |CANVAS_STYLE_APP_DRAWN
       |CANVAS_STYLE_TEXT_OPAQUE), 0, 0, ClrSilver, &g_sFontCm20, 
       "  PWM application   ", 0, PwmDraw);

Canvas(g_sDutycyclename, &g_sDutycycleset, 0, 0,
       &MyDisplay, 5, 153, 85, 23,
       CANVAS_STYLE_TEXT,
       ClrDarkBlue, ClrWhite, ClrYellow, &g_sFontCm18, "Duty cycle :", bianzhi, 0);

Slider(g_sDutycycleset,&g_sPwmbackground,0,&g_sDutycyclename,\
       &MyDisplay,95, 150, 220, 30, 0, 100, 50,\
       SL_STYLE_FILL|SL_STYLE_BACKG_FILL|SL_STYLE_TEXT \
       |SL_STYLE_BACKG_TEXT|SL_STYLE_OUTLINE,
       ClrGray, ClrBlack, ClrSilver, ClrWhite, ClrWhite, &g_sFontCmss18i,
         "50%",0,0,
          Dutycycleset);
//******************************************************************************
//      the IIC panel data struct
//******************************************************************************
#define PCA9543_ADDR     0X20
#define CONTROL_BYTE  0x03

extern
void Pca9534Write(unsigned char addr,unsigned char command,unsigned char data);
extern 
unsigned char Pca9534ReadReg(unsigned char addr,unsigned char command);
//*************************************************************
extern 
unsigned char Pca9534ReadPort(unsigned char addr);

//Be careful,this function read the port status of
//the pca9534 efficientlly .(of course the "Pca9534
//ReadReg" can also do the same function.)
//it assumes that the command byte has previously
//been programmed with 00h,so,befor you use this
//function you can call the "Pca9534ReadReg",
//which set the Pca9534 interal register to 0x00,
//that means the Input Port Register.
//*************************************************************
tCheckBoxWidget g_psCheckBoxes[];
unsigned char Iic_val=0x00;

Canvas(g_sIicbackground, 0, 0,g_psPca9534port,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE, ClrBlack, 
       ClrLime, 0, 0, 0, 0, 0);

Canvas(g_sIictitle, 0, 0, 0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |
       CANVAS_STYLE_TEXT_OPAQUE), 0, 0, ClrSilver, &g_sFontCm20, 
       "   Iic application   ", 0, 0);



tCheckBoxWidget  g_psPca9534port[] =
{

CheckBoxStruct(&g_sIicbackground,g_psPca9534port+1,0,
               &MyDisplay,10,40,50,30,CB_STYLE_IMG
                 |CB_STYLE_OUTLINE | CB_STYLE_FILL,
              16,0,ClrGray,0,0,0, g_pucLightOff,Oncheckboxchange
               ),


CheckBoxStruct(&g_sIicbackground,g_psPca9534port+2,0,
               &MyDisplay,70,40,50,30,CB_STYLE_IMG
                 |CB_STYLE_OUTLINE | CB_STYLE_FILL|CB_STYLE_TEXT,
              16,0, ClrGray, ClrSilver, 0, 0,
               g_pucLightOff,Oncheckboxchange
               ),

CheckBoxStruct(&g_sIicbackground,g_psPca9534port+3,0,
               &MyDisplay,130,40,50,30,CB_STYLE_IMG
                 |CB_STYLE_OUTLINE | CB_STYLE_FILL,
              16,0, ClrGray, ClrSilver,0, 0,
              g_pucLightOff,Oncheckboxchange
               ),

CheckBoxStruct(&g_sIicbackground,g_psPca9534port+4,0,
               &MyDisplay,190,40,50,30,CB_STYLE_IMG
                 |CB_STYLE_OUTLINE | CB_STYLE_FILL,
              16,0,ClrGray,0,0,0, g_pucLightOff,Oncheckboxchange
               ),

CheckBoxStruct(&g_sIicbackground,g_psPca9534port+5,0,
               &MyDisplay,10,90,50,30,CB_STYLE_IMG
                 |CB_STYLE_OUTLINE | CB_STYLE_FILL,
              16,0,ClrGray,0,0,0, g_pucLightOff,Oncheckboxchange
               ),

CheckBoxStruct(&g_sIicbackground,g_psPca9534port+6,0,
               &MyDisplay,70,90,50,30,CB_STYLE_IMG
                 |CB_STYLE_OUTLINE | CB_STYLE_FILL,
              16,0,ClrGray,0,0,0, g_pucLightOff,Oncheckboxchange
               ),

CheckBoxStruct(&g_sIicbackground,g_psPca9534port+7,0,
               &MyDisplay,130,90,50,30,CB_STYLE_IMG
                 |CB_STYLE_OUTLINE | CB_STYLE_FILL,
              16,0,ClrGray,0,0,0, g_pucLightOff,Oncheckboxchange
               ),

CheckBoxStruct(&g_sIicbackground,0,0,
               &MyDisplay,190,90,50,30,CB_STYLE_IMG
                 |CB_STYLE_OUTLINE | CB_STYLE_FILL,
              16,0,ClrGray,0,0,0, g_pucLightOff,Oncheckboxchange
               ),


};

#define NUM_CHECK_BOXS sizeof(g_psPca9534port)/ \
                       sizeof(g_psPca9534port[0]) 
//******************************************************************************
//      the UART panel data struct
//******************************************************************************

Canvas(g_sUartbackground, 0, 0, 0,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE, ClrBlack, 
       ClrLime, 0, 0, 0, 0, 0);

Canvas(g_sUarttitle, 0, 0, 0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |
       CANVAS_STYLE_TEXT_OPAQUE), 0, 0, ClrSilver, &g_sFontCm20, 
       "   Uart application   ", 0, 0);


//******************************************************************************
//      the SPI panel data struct
//******************************************************************************


//******************************************************************************
//      the Painting panel data struct
//******************************************************************************

Canvas(g_sPaintingbackground, 0, 0,&g_sPaintingclear,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE
         , ClrBlack, 
       ClrLime, 0, 0, 0, 0,0);

Canvas(g_sPaintingtitle, 0, 0, 0,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |
       CANVAS_STYLE_TEXT_OPAQUE|CANVAS_STYLE_APP_DRAWN),
       0, 0, ClrSilver, &g_sFontCm20, 
       "Painting application", 0, Painting);


RectangularButton(g_sPaintingclear, &g_sPaintingbackground,0, 0,
                  &MyDisplay,270, 190, 50, 50,
                  PB_STYLE_IMG | PB_STYLE_TEXT|PB_STYLE_IMG,
                  ClrBlack, ClrBlack, 0,
                  ClrSilver, &g_sFontCm20, "C", g_pucBlue50x50,
                  g_pucBlue50x50Press, 0, 0,OnPaintingPress);

//******************************************************************************
//      the seting panel data struct
//******************************************************************************


Canvas(g_sSettingbackground, 0, 0,0,
       &MyDisplay, 1, 24, 319, (239 -75),
       CANVAS_STYLE_FILL|CANVAS_STYLE_OUTLINE, ClrBlack, 
       ClrLime, 0, 0, 0, 0,0);

Canvas(g_sSettingtitle, 0, 0,&g_sBacklightset,
       &MyDisplay,  50, 190, 220, 50, (CANVAS_STYLE_TEXT |
       CANVAS_STYLE_TEXT_OPAQUE),
       0, 0, ClrSilver, &g_sFontCm20, 
       "      Setting     ", 0,);

Slider(g_sBacklightset,&g_sSettingtitle,0,0,\
       &MyDisplay,10,40,195,37,1,255,255,\
       SL_STYLE_IMG|SL_STYLE_BACKG_IMG|SL_STYLE_TEXT \
       |SL_STYLE_BACKG_TEXT,
         0,0,ClrSilver,ClrSilver,ClrSilver, &g_sFontCmss22i,
         "Set Backlight",g_pucRedSlider195x37,g_pucGreenSlider195x37,
          OnSliderChange);


void OnPrevious(tWidget *pWidget)
{
  
    SoundPlay();
    
    switch(g_ulPanel)
    {
    case SHOW_APP:
      {
          WidgetAdd(WIDGET_ROOT,(tWidget*)&g_sNext);
    
          WidgetRemove((tWidget *)&g_sAppbackground);

          WidgetAdd(WIDGET_ROOT,(tWidget*)&g_sBackground);
    
          CanvasTextSet(&g_sTitle, "    Intorduction    ");
    
    //
    // Set the button text to read "X" (exit) if this is the first panel,
    // otherwise revert to the usual "-".
    //
          PushButtonTextSet(&g_sPrevious,"x");
          WidgetPaint((tWidget *)&g_sPrevious);
     
     
          WidgetRemove((tWidget *)&g_sPrevious);

    //
    // Set the next button text to read "X" (exit) if this is the last panel,
    // otherwise revert to the usual "+".
    //
          PushButtonTextSet(&g_sNext,"+");

          WidgetPaint(WIDGET_ROOT);
          
          break;
          
      }
    case LED_PANEL:
      {
       WidgetRemove((tWidget *)&g_sLedbackground);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&LED);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&g_sApptitle);
    //   CanvasTextSet(&g_sTitle,"show applications");
       WidgetPaint((tWidget *)&g_sAppbackground);
     
       g_ulPanel=SHOW_APP;
       
       break;
      }  
   
    case BING_PANEL:
      {
      
       WidgetRemove((tWidget *)&g_sBingbackground);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&LED);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&g_sApptitle);
    //   CanvasTextSet(&g_sTitle,"show applications");
       WidgetPaint((tWidget *)&g_sAppbackground);
     
       g_ulPanel=SHOW_APP;
       
       break;
      
      }
      
      case GPTM_PANEL:
      {
       
       WidgetRemove((tWidget *)&g_sGptmbackground);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&LED);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&g_sApptitle);
    //   CanvasTextSet(&g_sTitle,"show applications");
       WidgetPaint((tWidget *)&g_sAppbackground);
     
       g_ulPanel=SHOW_APP;
       break;
       
      }

      case PWM_PANEL:
      {
      
               
       WidgetRemove((tWidget *)&g_sPwmbackground);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&LED);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&g_sApptitle);
    //   CanvasTextSet(&g_sTitle,"show applications");
       WidgetPaint((tWidget *)&g_sAppbackground);
     
       g_ulPanel=SHOW_APP;
       break;
     
      
      }

      case IIC_PANEL:
      {
      
       WidgetRemove((tWidget *)&g_sIicbackground);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&LED);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&g_sApptitle);
    //   CanvasTextSet(&g_sTitle,"show applications");
       WidgetPaint((tWidget *)&g_sAppbackground);
     
       g_ulPanel=SHOW_APP;
         break;
      
      }

      case UART_PANEL:
      {
      
               
       WidgetRemove((tWidget *)&g_sUartbackground);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&LED);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&g_sApptitle);
       WidgetPaint((tWidget *)&g_sAppbackground);
     
       g_ulPanel=SHOW_APP;
       break;
       
      }

      case SPI_PANEL:
      {
      
         break;
      
      }
            case CAN_PANEL:
      {
      
         break;
      
      }
      
            case PAINTING_PANEL:
      {
      
       PushButtonTextSet(&g_sPaintingclear,"x");
       WidgetPaint((tWidget *)&g_sPaintingclear);
       WidgetRemove((tWidget *)&g_sPaintingbackground);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&LED);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&g_sApptitle);

       WidgetPaint((tWidget *)&g_sAppbackground);
       
       g_ulPanel=SHOW_APP;
         break;
      
      }

          case SETTING_PANEL:
        {
        
       WidgetRemove((tWidget *)&g_sSettingbackground);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&LED);
       WidgetAdd((tWidget *)&g_sAppbackground,(tWidget *)&g_sApptitle);
    //   CanvasTextSet(&g_sTitle,"show applications");
       WidgetPaint((tWidget *)&g_sAppbackground);
     
       g_ulPanel=SHOW_APP;
        
        }
    }         
}

//*****************************************************************************
//
// Handles presses of the next panel button.
//
//*****************************************************************************
void
OnNext(tWidget *pWidget)
{
     //
    // Play the key click sound.
    //

     g_ulPanel=SHOW_APP;
    
     SoundPlay();
   
    // Remove the current panel.
    
    WidgetRemove((tWidget *)&g_sBackground);

    WidgetAdd(WIDGET_ROOT,(tWidget*)&g_sAppbackground);
    
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sPrevious);
    
    //
    // Set the title of this panel.
    //
    CanvasTextSet(&g_sTitle, "show applications");
    
    // WidgetPaint((tWidget *)&g_sTitle);
    //
    // Set the "previous" button text to read "X" if this is the first panel,
    // otherwise revert to the usual "-".
    //
    PushButtonTextSet(&g_sPrevious,"-");
 //   WidgetPaint((tWidget *)&g_sPrevious);
    
    PushButtonTextSet(&g_sNext,"x");
    WidgetPaint((tWidget *)&g_sNext);
    
    WidgetPaint(WIDGET_ROOT);
    
    WidgetRemove((tWidget *)&g_sNext);
    
    
}

void OnIntroPaint(tWidget *pWidget,tContext *pContext)
{


     //
    // Display the introduction text in the canvas.
    //
    GrContextFontSet(pContext, &g_sFontCm18);
    GrContextForegroundSet(pContext, ClrSilver);
    GrStringDraw(pContext, "This application demonstrates the Stellaris", -1,
                 3, 25, 0);
    GrStringDraw(pContext, "MCU LM3S2110. shows all the peripheral on",
                 -1, 3, 50, 0);
    GrStringDraw(pContext, "this device. each peripheral on this device",
                 -1, 3,75, 0);
    GrStringDraw(pContext, "can be accessed by samply using your furg",
                 -1, 3,100, 0);
     
    GrStringDraw(pContext, "er touch the screen, please enjoy it . . . ",
                 -1, 3,125, 0);


}

void OnLedPress(tWidget *pWidget)
{

SoundPlay();

g_ulPanel=LED_PANEL;
WidgetRemove((tWidget *)&LED);

WidgetAdd((tWidget*)&g_sAppbackground,(tWidget*)&g_sLedbackground);

WidgetAdd((tWidget*)&g_sLedbackground,(tWidget*)&g_sLedtitle);

WidgetPaint((tWidget*)&g_sLedbackground);

//Configure the gpio peripheral used by this leds 

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,LED8);

GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,LED1);

//set the orignal status of the led
GPIOPinWrite(GPIO_PORTF_BASE,LED1,LED1);

GPIOPinWrite(GPIO_PORTE_BASE,LED8,LED8);


}

void OnButtonPress(tWidget *pWidget)
{
 
unsigned long ulIdx;
  
unsigned long ulX,ulY;

static unsigned char led1=0,led8=0;
  
   SoundPlay();
    //
    // Find the index of this push button.
    //
    for(ulIdx = 0; ulIdx < NUM_PUSH_BUTTONS; ulIdx++)
    {
        if(pWidget == (tWidget *)(g_psPushButtons + ulIdx))
        {
            break;
        }
    }

    //
    // Return if the push button could not be found.
    //
    if(ulIdx == NUM_PUSH_BUTTONS)
    {
        return;
    }

    //
    // Toggle the state of this push button indicator.
    //
    g_ulButtonState ^= 1 << ulIdx;
    
    //
    // Set the matching indicator based on the selected state of the check box.
    //
    CanvasImageSet(g_psPushButtonIndicators + ulIdx,
                   (g_ulButtonState & (1 << ulIdx)) ? g_pucLightOn :
                   g_pucLightOff);
    WidgetPaint((tWidget *)(g_psPushButtonIndicators + ulIdx));

    switch(ulIdx)
    {
    case 0: GPIOPinWrite(GPIO_PORTF_BASE,LED1,
                         ~GPIOPinRead(GPIO_PORTF_BASE,LED1));
            
             
            break;
    
    case 1:GPIOPinWrite(GPIO_PORTE_BASE,LED8,
                         ~GPIOPinRead(GPIO_PORTE_BASE,LED8));
           

           break;
    case 2: 
            GPIOPinWrite(GPIO_PORTF_BASE,LED1,0xff);
            GPIOPinWrite(GPIO_PORTE_BASE,LED8,0xff);
            led1=0;
            led8=0;
            PushButtonTextSet(g_psPushButtons+2,"stop");
             WidgetPaint((tWidget *)(g_psPushButtons+2));
          while(1)
            {
           
            led1=~led1;
            led8=~led8;
            GPIOPinWrite(GPIO_PORTF_BASE,LED1,
                         led1);
            
            CanvasImageSet(g_psPushButtonIndicators,
                   (~led1)&0x1 ? g_pucLightOn :
                   g_pucLightOff);
            
            WidgetPaint((tWidget *)g_psPushButtonIndicators);
            
            WidgetMessageQueueProcess();
            
            SysCtlDelay(TheSysClock/3);
            
            GPIOPinWrite(GPIO_PORTE_BASE,LED8,
                         led8);
            
             CanvasImageSet(g_psPushButtonIndicators +1,
             (~led8)&0x1 ? g_pucLightOn :
             g_pucLightOff);
             
            WidgetPaint((tWidget *)(g_psPushButtonIndicators +1));
             
           WidgetMessageQueueProcess();
             
           getpoint(); //get the touch point on the panal

          ulX=319*(g_sTouchX-200)/3600;          
          ulY=239*(g_sTouchY-300)/3300;

         if(((ulX>30)&&(ulY>120))&&((ulX<100)&&(ulY<170)))
         {
            SoundPlay();      
            g_ulButtonState=0;
            GPIOPinWrite(GPIO_PORTF_BASE,LED1,0xff);
            GPIOPinWrite(GPIO_PORTE_BASE,LED8,0xff);
            PushButtonTextSet(g_psPushButtons+2,"run");
            WidgetPaint((tWidget *)(g_psPushButtons+2));
            break;
               
         }

            }
            break;
           
    }


}


void OnBingPress(tWidget *pWidget)
{

  SoundPlay();
  
  g_ulPanel=BING_PANEL;

WidgetRemove((tWidget *)&LED);

WidgetAdd((tWidget*)&g_sAppbackground,(tWidget*)&g_sBingbackground);

WidgetAdd((tWidget*)&g_sBingbackground,(tWidget*)&g_sBingtitle);

WidgetPaint((tWidget*)&g_sBingbackground);


}

void BingDraw(tWidget *pWidget, tContext *pContext)
{

    GrContextFontSet(&MyContext, &g_sFontCm22i);
    GrContextForegroundSet(&MyContext, ClrSilver);
    GrStringDraw(&MyContext, "Touch the button", -1,
                 158, 50, 0);
    LCDlineDrawH((void*)0,158,318,
                   70,DpyColorTranslate(&MyDisplay,ClrSilver));
    
    GrStringDraw(&MyContext, "to turn on or",
                 -1, 158, 75, 0);
    LCDlineDrawH((void*)0,158,318,
                   95,DpyColorTranslate(&MyDisplay,ClrSilver));
        
    GrStringDraw(&MyContext, "turn off the",
                 -1, 158, 100, 0);
    LCDlineDrawH((void*)0,158,318,
                   120,DpyColorTranslate(&MyDisplay,ClrSilver));
    GrStringDraw(&MyContext, "bing .",
                 -1, 158, 125, 0);
    
    LCDlineDrawH((void*)0,158,318,
                   145,DpyColorTranslate(&MyDisplay,ClrSilver));
}


void OnGptmPress(tWidget *pWidget)
{

SoundPlay();

g_ulPanel=GPTM_PANEL;

WidgetRemove((tWidget *)&LED);

WidgetAdd((tWidget*)&g_sAppbackground,(tWidget*)&g_sGptmbackground);

WidgetAdd((tWidget*)&g_sGptmbackground,(tWidget*)&g_sGptmtitle);


WidgetPaint((tWidget*)&g_sGptmbackground);
}

void OnPwmPress(tWidget *pWidget)
{

SoundPlay();

g_ulPanel=PWM_PANEL;

WidgetRemove((tWidget *)&LED);

WidgetAdd((tWidget*)&g_sAppbackground,(tWidget*)&g_sPwmbackground);

WidgetAdd((tWidget*)&g_sPwmbackground,(tWidget*)&g_sPwmtitle);

WidgetPaint((tWidget*)&g_sPwmbackground);
}

void PwmDraw(tWidget *pWidget, tContext *pContext)
{
  
NWave.height=80; //高80像素
NWave.high=150; //高电平
NWave.low=150;  //低电平

BWave.height=80; //高80像素
BWave.high=150; //高电平
BWave.low=150;  //低电平

GrContextForegroundSet(&MyContext, ClrMediumVioletRed);

GrLineDrawV(&MyContext,10, 50,50+NWave.height);
GrLineDrawH(&MyContext,10,10+NWave.high,50);
GrLineDrawV(&MyContext,10+NWave.high, 50,50+NWave.height);
GrLineDrawH(&MyContext,10+NWave.high,10+NWave.high+NWave.low,50+NWave.height);
GrLineDrawV(&MyContext,10+NWave.high+NWave.low,\
           50+NWave.height,50);

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_1);
SysCtlPWMClockSet(SYSCTL_PWMDIV_1); 
PWMGenConfigure(PWM_BASE,PWM_GEN_0,PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC
                  );

  PWMGenPeriodSet(PWM_BASE,PWM_GEN_0,2500);// 10K
  
  PWMPulseWidthSet(PWM_BASE,PWM_OUT_0,1250);//50%
  
  PWMPulseWidthSet(PWM_BASE, PWM_OUT_1,1250); //50%
 
  PWMOutputInvert(PWM_BASE,PWM_OUT_0_BIT,false); 
  PWMOutputInvert(PWM_BASE,PWM_OUT_1_BIT,true);
  
  PWMOutputState(PWM_BASE,PWM_OUT_0_BIT|PWM_OUT_1_BIT,true); 
  
  PWMGenEnable(PWM_BASE, PWM_GEN_0); 



}

void Dutycycleset(tWidget *pWidget, long lValue)
{

static char pcSliderText[5];
float temp;

temp=lValue*0.01;

PWMPulseWidthSet(PWM_BASE,PWM_OUT_0,(unsigned long)(2500*temp));

PWMPulseWidthSet(PWM_BASE, PWM_OUT_1,(unsigned long)(2500*temp)); 
  
NWave.height=80; //高80像素
NWave.high=(unsigned long)(temp*300); //高电平
NWave.low=300-NWave.high;  //低电平

//clean the waves befor
GrContextForegroundSet(&MyContext, ClrBlack);

GrLineDrawV(&MyContext,10, 50,50+BWave.height);
GrLineDrawH(&MyContext,10,10+BWave.high,50);
GrLineDrawV(&MyContext,10+BWave.high, 50,50+BWave.height);
GrLineDrawH(&MyContext,10+BWave.high,10+BWave.high+BWave.low,50+BWave.height);
GrLineDrawV(&MyContext,10+BWave.high+BWave.low,\
           50+BWave.height,50);

//backup the befor wave

BWave.height= NWave.height; //高80像素
BWave.high=NWave.high; //高电平
BWave.low=NWave.low;  //低电平

//Draw the wave

GrContextForegroundSet(&MyContext, ClrMediumVioletRed);
GrLineDrawV(&MyContext,10, 50,50+NWave.height);
GrLineDrawH(&MyContext,10,10+NWave.high,50);
GrLineDrawV(&MyContext,10+NWave.high, 50,50+NWave.height);
GrLineDrawH(&MyContext,10+NWave.high,10+NWave.high+NWave.low,50+NWave.height);
GrLineDrawV(&MyContext,10+NWave.high+NWave.low,\
           50+NWave.height,50);

usprintf(pcSliderText, "%3d%%", lValue);

SliderTextSet(&g_sDutycycleset, pcSliderText);

WidgetPaint((tWidget*)&g_sDutycycleset);

}

void OnIicPress(tWidget *pWidget)
{
unsigned long ulIdx;

for(ulIdx=0;ulIdx<8;ulIdx++)
{

     CheckBoxImageSet(g_psPca9534port + ulIdx,
                      g_pucLightOff);

}

SoundPlay();

   
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);//使能IIC外设
  
   //配置为IIC引脚
  GPIOPinTypeI2C(GPIO_PORTB_BASE,GPIO_PIN_2|GPIO_PIN_3); 
  
  //初始化IIC master,400kps.
  I2CMasterInitExpClk(I2C0_MASTER_BASE,TheSysClock,true);

  //设置pca9534为输出
  Pca9534Write(PCA9543_ADDR,0x03,0x00);
  
  Pca9534Write(PCA9543_ADDR,0x01,0x00);
  
  
  g_ulPanel=IIC_PANEL;

WidgetRemove((tWidget *)&LED);

WidgetAdd((tWidget*)&g_sAppbackground,(tWidget*)&g_sIicbackground);

WidgetAdd((tWidget*)&g_sIicbackground,(tWidget*)&g_sIictitle);

WidgetPaint((tWidget*)&g_sIicbackground);


}
//*****************************************************************************
//
// Handles change notifications for the radio button widgets.
//
//*****************************************************************************
void
OnRadioChange(tWidget *pWidget, unsigned long bSelected)
{
    unsigned long ulIdx;

    //
    // Find the index of this radio button in the first group.
    //
    for(ulIdx = 0; ulIdx < NUM_RADIO1_BUTTONS; ulIdx++)
    {
        if(pWidget == (tWidget *)(g_psRadioButtons1 + ulIdx))
        {
            break;
        }
    }

    //
    // See if the radio button was found.
    //

        //
        // Return if the radio button could not be found.
        //
        if(ulIdx == NUM_RADIO1_BUTTONS)
        {
            return;
        }

    
        if(ulIdx ==0)
        {
        
        if(bSelected)  GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, 1);
        
        else
          GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, 0);
        
        }
        //
        // Sind the radio button is in the second group, offset the index to
        // the indicators associated with the second group.
        //

    //
    // Set the matching indicator based on the selected state of the radio
    // button.
    //
    CanvasImageSet(g_psRadioButtonIndicators + ulIdx,
                   bSelected ? g_pucLightOn : g_pucLightOff);
    WidgetPaint((tWidget *)(g_psRadioButtonIndicators + ulIdx));

    
}


void Oncheckboxchange(tWidget *pWidget, unsigned long bSelected)
{

    unsigned long ulIdx;
    unsigned char code[8]
      ={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};

    //
    // Find the index of this radio button in the first group.
    //
    for(ulIdx = 0; ulIdx < NUM_CHECK_BOXS; ulIdx++)
    {
        if(pWidget == (tWidget *)(g_psPca9534port + ulIdx))
        {
            break;
        }
    }

    if(bSelected)
    {
    
    Iic_val=(1<<ulIdx)|Iic_val;
    
    }
    
    else
      
    {
    
      Iic_val=code[ulIdx]&Iic_val;
    
    }
    
    Pca9534Write(PCA9543_ADDR,0x01,Iic_val);

     CheckBoxImageSet(g_psPca9534port + ulIdx,
                   bSelected ? g_pucLightOn : g_pucLightOff);
   
     WidgetPaint((tWidget *)(g_psPca9534port + ulIdx));
 
                                    

}

void OnUartPress(tWidget *pWidget)
{

SoundPlay();

g_ulPanel=UART_PANEL;

WidgetRemove((tWidget *)&LED);

WidgetAdd((tWidget*)&g_sAppbackground,(tWidget*)&g_sUartbackground);

WidgetAdd((tWidget*)&g_sUartbackground,(tWidget*)&g_sUarttitle);

WidgetPaint((tWidget*)&g_sUartbackground);

}

void OnPaintingPress(tWidget *pWidget)
{

    SoundPlay();
  
  g_ulPanel=PAINTING_PANEL;
  
PushButtonTextSet(&g_sPaintingclear,"C");

WidgetRemove((tWidget *)&LED);

WidgetAdd((tWidget*)&g_sAppbackground,(tWidget*)&g_sPaintingbackground);

WidgetAdd((tWidget*)&g_sPaintingbackground,(tWidget*)&g_sPaintingtitle);

WidgetPaint((tWidget*)&g_sPaintingbackground);

WidgetMessageQueueProcess();


}

void Painting(tWidget *pWidget, tContext *pContext)

{

unsigned char i=0;
unsigned long ulX,ulY;
  
GPIOPinIntDisable(GPIO_PORTA_BASE,GPIO_PIN_6);
IntDisable(INT_GPIOA);
    while(true)
    {
          getpoint(); //get the touch point on the panal

          ulX=319*(g_sTouchX-200)/3600;          
          ulY=239*(g_sTouchY-300)/3300;
          
          if(((ulX>0)&&(ulY>24))&&((ulX<319)&&(ulY<185)))
                       
          {
            
            MyRect.sXMin=ulX-3;
            MyRect.sXMax=ulX+3;
            MyRect.sYMin=ulY-3;
            MyRect.sYMax=ulY+3;
            LCDRectFill((void*)0,&MyRect,
            DpyColorTranslate(&MyDisplay,ClrWhite));
            if(i==8) i=0;
           
           }
         
          
          else
            
         if(((ulX>0)&&(ulY>190))&&((ulX<50)&&(ulY<240)))
         {
           
           
           IntPendClear(INT_GPIOA);
            
           GPIOPinIntEnable(GPIO_PORTA_BASE,GPIO_PIN_6);
           
           IntEnable(INT_GPIOA);
           
           break;
         }
         
         else
           if(((ulX>270)&&(ulY>190))&&((ulX<320)&&(ulY<240)))
         {
           
           
           IntPendClear(INT_GPIOA);
            
           GPIOPinIntEnable(GPIO_PORTA_BASE,GPIO_PIN_6);
           
           IntEnable(INT_GPIOA);
           
           break;
         }
  

   }
   
   g_pfnTSHandler(WIDGET_MSG_PTR_UP,ulX,ulY);
   g_pfnTSHandler(WIDGET_MSG_PTR_UP,ulX,ulY);
   
}

void OnSettingPress(tWidget *pWidget)
{


SoundPlay();
  
g_ulPanel=SETTING_PANEL;

WidgetRemove((tWidget *)&LED);

WidgetAdd((tWidget*)&g_sAppbackground,(tWidget*)&g_sSettingbackground);

WidgetAdd((tWidget*)&g_sSettingbackground,(tWidget*)&g_sSettingtitle);

WidgetPaint((tWidget*)&g_sSettingbackground);

WidgetMessageQueueProcess();

}

void OnSliderChange(tWidget *pWidget, long lValue)
{

static char pcSliderText[5];

SSD1963_SetBacklight(lValue);

lValue=100*lValue/254;

usprintf(pcSliderText, "%3d%%", lValue);

SliderTextSet(&g_sBacklightset, pcSliderText);

WidgetPaint((tWidget*)&g_sBacklightset);

}
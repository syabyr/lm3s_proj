
//the panel name will be use
#define INTORDUCTION       0
#define SHOW_APP           1
#define LED_PANEL          2
#define BING_PANEL         3
#define GPTM_PANEL         4
#define PWM_PANEL          5
#define IIC_PANEL          6
#define UART_PANEL         7
#define SPI_PANEL          8
#define CAN_PANEL          9

#define PAINTING_PANEL     10
#define SETTING_PANEL     11


//*****************************************************************************
//
// Forward reference to various widget structures.
//
//*****************************************************************************
extern tCanvasWidget g_sHeading;
extern tPushButtonWidget g_sPushBtn;
extern tCanvasWidget g_sIntroduction;

extern tWidget g_sRoot; 
extern tPushButtonWidget g_sPrevious;
extern tPushButtonWidget g_sNext;

extern tCanvasWidget g_sTitle;
extern tCanvasWidget g_sGptmtitle;
extern tCanvasWidget g_sIictitle;
extern tCanvasWidget g_sPaintingtitle;
extern tCanvasWidget g_sSetingtitle;
extern tCanvasWidget g_sPwmtitle;

extern tCanvasWidget g_sBackground;
extern tCanvasWidget g_sAppbackground;
extern tCanvasWidget g_sSettingbackground;
extern tCanvasWidget g_sPwmbackground;
extern tCanvasWidget g_sIicbackground;
//Forward declarations for applications
extern tPushButtonWidget LED;
extern tPushButtonWidget BING;
extern tPushButtonWidget GPTM;
extern tPushButtonWidget PWM;
extern tPushButtonWidget IIC;
extern tPushButtonWidget UART;
extern tPushButtonWidget SPI;
extern tPushButtonWidget CAN;
extern tPushButtonWidget PAINTING;
extern tPushButtonWidget SETTING;
extern tSliderWidget g_sBacklightset;
extern tSliderWidget g_sDutycycleset;
extern tPushButtonWidget g_psPushButtons[];
extern tPushButtonWidget g_sPaintingclear;

//*****************************************************************************
//
// Forward declarations for the globals required to define the widgets at
// compile-time.
//
//*****************************************************************************
void OnPrevious(tWidget *pWidget);
void OnNext(tWidget *pWidget);
void OnIntroPaint(tWidget *pWidget, tContext *pContext);
void OnPrimitivePaint(tWidget *pWidget, tContext *pContext);
void OnCanvasPaint(tWidget *pWidget, tContext *pContext);
void OnCheckChange(tWidget *pWidget, unsigned long bSelected);
void OnButtonPress(tWidget *pWidget);
void OnRadioChange(tWidget *pWidget, unsigned long bSelected);
void OnSliderChange(tWidget *pWidget, long lValue);
void OnIntroPaint(tWidget *pWidget, tContext *pContext);
void OnLedPress(tWidget *pWidget);
void OnBingPress(tWidget *pWidget);
void OnSettingPress(tWidget *pWidget);
void OnGptmPress(tWidget *pWidget);
void OnPwmPress(tWidget *pWidget);
void OnIicPress(tWidget *pWidget);
void OnUartPress(tWidget *pWidget);
void OnPaintingPress(tWidget *pWidget);
void BingDraw(tWidget *pWidget, tContext *pContext);
void PwmDraw(tWidget *pWidget, tContext *pContext);
void Dutycycleset(tWidget *pWidget, long lValue);
void OnRadioChange(tWidget *pWidget, unsigned long bSelected);
void Oncheckboxchange(tWidget *pWidget, unsigned long bSelected);
void Painting(tWidget *pWidget, tContext *pContext);
void OnSliderChange(tWidget *pWidget, long lValue);
extern tCanvasWidget g_psPanels[];
extern tCanvasWidget g_psCheckBoxIndicators[];
extern tRadioButtonWidget g_psRadioButtons[];
extern tCheckBoxWidget  g_psPca9534port[];

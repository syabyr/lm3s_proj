#ifndef TOUCH_H
#define TOUCH_H

//*****************************************************************************
//
// Prototypes for the functions exported by the touch screen driver.
//
//*****************************************************************************
extern volatile unsigned long  g_sTouchX;
extern volatile unsigned long  g_sTouchY;
extern void TouchScreenIntHandler(void);
extern void TouchScreenInit(void);
extern void TouchScreenCallbackSet(long (*pfnCallback)(unsigned long ulMessage,
                                                       long lX, long lY));




#endif
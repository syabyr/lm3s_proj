//*****************************************************************************
//
// touch.h - Prototypes for the touch screen driver.
//
// Copyright (c) 2008-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 5821 of the RDK-IDM Firmware Package.
//
//*****************************************************************************

#ifndef __TOUCH_H__
#define __TOUCH_H__

//*****************************************************************************
//
// Prototypes for the functions exported by the touch screen driver.
//
//*****************************************************************************
extern volatile short g_sTouchX;
extern volatile short g_sTouchY;
extern void TouchScreenIntHandler(void);
extern void TouchScreenInit(void);
extern void TouchScreenCallbackSet(long (*pfnCallback)(unsigned long ulMessage,
                                                       long lX, long lY));

#endif // __TOUCH_H__

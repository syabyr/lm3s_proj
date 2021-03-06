//*****************************************************************************
//
// clock-arch.h - uIP Project Specific Clock-Architecture header file.
//
// Copyright (c) 2009-2011 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 6852 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************

#ifndef __CLOCK_ARCH_H__
#define __CLOCK_ARCH_H__

//
// Define how many clock ticks in one second.
// Note:  This should match the value of SYSTICKHZ in the main program.
//
#define CLOCK_CONF_SECOND       100

//
// Define the clock type used for returning system ticks.
//
typedef unsigned long clock_time_t;

#endif // __CLOCK_ARCH_H__

//*****************************************************************************
//
// watchdog.c - Watchdog timer example.
//
// Copyright (c) 2005-2013 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 10636 of the EK-LM3S811 Firmware Package.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/watchdog.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Watchdog (watchdog)</h1>
//!
//! This example application demonstrates the use of the watchdog as a simple
//! heartbeat for the system.  If the watchdog is not periodically fed, it will
//! reset the system.  Each time the watchdog is fed, the LED connected to port
//! C5 is inverted so that it is easy to see that it is being fed, which occurs
//! once every second.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// The interrupt handler for the watchdog.  This feeds the dog (so that the
// processor does not get reset) and winks the LED connected to GPIO B0.
//
//*****************************************************************************
void
WatchdogIntHandler(void)
{
    //
    // Clear the watchdog interrupt.
    //
    WatchdogIntClear(WATCHDOG0_BASE);

    //
    // Invert the GPIO B0 value.
    //
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5,
                 GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5) ^ GPIO_PIN_5);
}

//*****************************************************************************
//
// This example demonstrates the use of the watchdog timer.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_6MHZ);


    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Set GPIO C5 as an output.  This drives an LED on the board that will
    // toggle when a watchdog interrupt is processed.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5);

    //
    // Enable the watchdog interrupt.
    //
    IntEnable(INT_WATCHDOG);

    //
    // Set the period of the watchdog timer.
    //
    WatchdogReloadSet(WATCHDOG0_BASE, SysCtlClockGet());

    //
    // Enable reset generation from the watchdog timer.
    //
    WatchdogResetEnable(WATCHDOG0_BASE);

    //
    // Enable the watchdog timer.
    //
    WatchdogEnable(WATCHDOG0_BASE);

    //
    // Loop forever while the LED winks as watchdog interrupts are handled.
    //
    while(1)
    {
    }
}

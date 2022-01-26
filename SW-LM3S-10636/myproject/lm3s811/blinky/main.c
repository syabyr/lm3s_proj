//*****************************************************************************
//
// blinky.c - Simple example to blink the on-board LED.
//
// Copyright (c) 2007-2013 Texas Instruments Incorporated.  All rights reserved.
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

#include "inc/lm3s811.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED.
//
//*****************************************************************************

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
int
main(void)
{
    volatile unsigned long ulLoop;

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOC;

    //
    // Do a dummy read to insert a few cycles after enabling the peripheral.
    //
    ulLoop = SYSCTL_RCGC2_R;

    //
    // Enable the GPIO pin for the LED (PC5).  Set the direction as output, and
    // enable the GPIO pin for digital function.  Care is taken to not disrupt
    // the operation of the JTAG pins on PC0-PC3.
    //
    GPIO_PORTC_DIR_R |= 0x20;
    GPIO_PORTC_DEN_R |= 0x20;

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Turn on the LED.
        //
        GPIO_PORTC_DATA_R |= 0x20;

        //
        // Delay for a bit.
        //
        for(ulLoop = 0; ulLoop < 800000; ulLoop++)
        {
        }

        //
        // Turn off the LED.
        //
        GPIO_PORTC_DATA_R &= ~(0x20);

        //
        // Delay for a bit.
        //
        for(ulLoop = 0; ulLoop < 200000; ulLoop++)
        {
        }
    }
}

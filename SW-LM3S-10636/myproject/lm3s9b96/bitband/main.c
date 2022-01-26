//*****************************************************************************
//
// bitband.c - Bit-band manipulation example.
//
// Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 9107 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "grlib/grlib.h"
#include "kitronix320x240x16_ssd2119_8bit.h"
#include "set_pinout.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Bit-Banding (bitband)</h1>
//!
//! This example application demonstrates the use of the bit-banding
//! capabilities of the Cortex-M3 microprocessor.  All of SRAM and all of the
//! peripherals reside within bit-band regions, meaning that bit-banding
//! operations can be applied to any of them.  In this example, a variable in
//! SRAM is set to a particular value one bit at a time using bit-banding
//! operations (it would be more efficient to do a single non-bit-banded write;
//! this simply demonstrates the operation of bit-banding).
//
//*****************************************************************************

//*****************************************************************************
//
// A map of hex nibbles to ASCII characters.
//
//*****************************************************************************
static const char * const pcHex = "0123456789ABCDEF";

//*****************************************************************************
//
// The value that is to be modified via bit-banding.
//
//*****************************************************************************
static volatile unsigned long g_ulValue;

//*****************************************************************************
//
// Graphics context used to show text on the CSTN display.
//
//*****************************************************************************
tContext g_sContext;

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
// Delay for the specified number of seconds.  Depending upon the current
// SysTick value, the delay will be between N-1 and N seconds (i.e. N-1 full
// seconds are guaranteed, along with the remainder of the current second).
//
//*****************************************************************************
void
Delay(unsigned long ulSeconds)
{
    //
    // Loop while there are more seconds to wait.
    //
    while(ulSeconds--)
    {
        //
        // Wait until the SysTick value is less than 1000.
        //
        while(ROM_SysTickValueGet() > 1000)
        {
        }

        //
        // Wait until the SysTick value is greater than 1000.
        //
        while(ROM_SysTickValueGet() < 1000)
        {
        }
    }
}

//*****************************************************************************
//
// Print the given value as a hexadecimal string on the CSTN.
//
//*****************************************************************************
void
PrintValue(unsigned long ulValue)
{
    char pcBuffer[9];

    pcBuffer[0] = pcHex[(ulValue >> 28) & 15];
    pcBuffer[1] = pcHex[(ulValue >> 24) & 15];
    pcBuffer[2] = pcHex[(ulValue >> 20) & 15];
    pcBuffer[3] = pcHex[(ulValue >> 16) & 15];
    pcBuffer[4] = pcHex[(ulValue >> 12) & 15];
    pcBuffer[5] = pcHex[(ulValue >> 8) & 15];
    pcBuffer[6] = pcHex[(ulValue >> 4) & 15];
    pcBuffer[7] = pcHex[(ulValue >> 0) & 15];
    pcBuffer[8] = '\0';

    GrStringDrawCentered(&g_sContext, pcBuffer, -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 120, 1);
}

//*****************************************************************************
//
// This example demonstrates the use of bit-banding to set individual bits
// within a word of SRAM.
//
//*****************************************************************************
int
main(void)
{
    tRectangle sRect;
    unsigned long ulErrors, ulIdx;

    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // Set the device pinout appropriately for this board.
    //
    PinoutSet();

    //
    // Initialize the display driver.
    //
    Kitronix320x240x16_SSD2119Init();

    //
    // Initialize the graphics context and find the middle X coordinate.
    //
    GrContextInit(&g_sContext, &g_sKitronix320x240x16_SSD2119);

    //
    // Fill the top 15 rows of the screen with blue to create the banner.
    //
    sRect.sXMin = 0;
    sRect.sYMin = 0;
    sRect.sXMax = GrContextDpyWidthGet(&g_sContext) - 1;
    sRect.sYMax = 23;
    GrContextForegroundSet(&g_sContext, ClrDarkBlue);
    GrRectFill(&g_sContext, &sRect);

    //
    // Put a white box around the banner.
    //
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrRectDraw(&g_sContext, &sRect);

    //
    // Put the application name in the middle of the banner.
    //
    GrContextFontSet(&g_sContext, g_pFontCm20);
    GrStringDrawCentered(&g_sContext, "bitband", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 11, 0);
    GrContextFontSet(&g_sContext, g_pFontCmss30);

    //
    // Set up and enable the SysTick timer.  It will be used as a reference
    // for delay loops.  The SysTick timer period will be set up for one
    // second.
    //
    ROM_SysTickPeriodSet(SysCtlClockGet());
    ROM_SysTickEnable();

    //
    // Set the value and error count to zero.
    //
    g_ulValue = 0;
    ulErrors = 0;

    //
    // Print the initial value to the CSTN.
    //
    PrintValue(g_ulValue);

    //
    // Delay for 1 second.
    //
    Delay(1);

    //
    // Set the value to 0xdecafbad using bit band accesses to each individual
    // bit.
    //
    for(ulIdx = 0; ulIdx < 32; ulIdx++)
    {
        //
        // Set this bit.
        //
        HWREGBITW(&g_ulValue, 31 - ulIdx) = (0xdecafbad >> (31 - ulIdx)) & 1;

        //
        // Print the current value to the CSTN.
        //
        PrintValue(g_ulValue);

        //
        // Delay for 1 second.
        //
        Delay(1);
    }

    //
    // Make sure that the value is 0xdecafbad.
    //
    if(g_ulValue != 0xdecafbad)
    {
        ulErrors++;
    }

    //
    // Make sure that the individual bits read back correctly.
    //
    for(ulIdx = 0; ulIdx < 32; ulIdx++)
    {
        if(HWREGBITW(&g_ulValue, ulIdx) != ((0xdecafbad >> ulIdx) & 1))
        {
            ulErrors++;
        }
    }

    //
    // Delay for 2 seconds.
    //
    Delay(2);

    //
    // Print out the result.
    //
    if(ulErrors)
    {
        GrStringDrawCentered(&g_sContext, "Errors!", -1,
                             GrContextDpyWidthGet(&g_sContext) / 2, 180, 0);
    }
    else
    {
        GrStringDrawCentered(&g_sContext, "Success!", -1,
                             GrContextDpyWidthGet(&g_sContext) / 2, 180, 0);
    }

    //
    // Flush any cached drawing operations.
    //
    GrFlush(&g_sContext);

    //
    // Loop forever.
    //
    while(1)
    {
    }
}

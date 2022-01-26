//*****************************************************************************
//
// relay.c - Functions for operating the relay output.
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

//*****************************************************************************
//
//! \addtogroup relay_api
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "drivers/relay.h"

//*****************************************************************************
//
// The GPIO pin to which the relay is connected.
//
//*****************************************************************************
#define RELAY_PERIPH            SYSCTL_PERIPH_GPIOG
#define RELAY_BASE              GPIO_PORTG_BASE
#define RELAY_PIN               GPIO_PIN_1

//*****************************************************************************
//
//! Enables the relay output.
//!
//! This function enables the relay output.  This causes the relay to become
//! energized, putting it into the non-default state (in other words, the
//! normally open contact is closed and the normally closed contact is opened).
//!
//! \return None.
//
//*****************************************************************************
void
RelayEnable(void)
{
    //
    // Activate the relay.
    //
    GPIOPinWrite(RELAY_BASE, RELAY_PIN, RELAY_PIN);
}

//*****************************************************************************
//
//! Disable the relay output.
//!
//! This function disables the relay output.  This causes the relay to become
//! de-energized, putting it into its default state (in other words, the
//! normally open connect is opened and the normally closed contact is closed).
//!
//! \return None.
//
//*****************************************************************************
void
RelayDisable(void)
{
    //
    // Deactivate the relay.
    //
    GPIOPinWrite(RELAY_BASE, RELAY_PIN, 0);
}

//*****************************************************************************
//
//! Initializes the relay output.
//!
//! This function initializes the relay output, preparing it to control the
//! relay.  The relay is started in the disabled state (in other words,
//! de-energized).
//!
//! \return None.
//
//*****************************************************************************
void
RelayInit(void)
{
    //
    // Enable the GPIO peripheral used to drive the relay.
    //
    SysCtlPeripheralEnable(RELAY_PERIPH);

    //
    // Configure the relay control signal as a GPIO output.
    //
    GPIOPinTypeGPIOOutput(RELAY_BASE, RELAY_PIN);

    //
    // Deactivate the relay.
    //
    GPIOPinWrite(RELAY_BASE, RELAY_PIN, 0);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

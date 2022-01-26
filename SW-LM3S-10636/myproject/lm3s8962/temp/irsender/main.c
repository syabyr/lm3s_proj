//*****************************************************************************
//
// pwmgen.c - PWM signal generation example.
//
// Copyright (c) 2005-2011 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 7611 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************
#include "inc/lm3s8962.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "drivers/mydelay.h"


#define high() PWMGenEnable(PWM_BASE, PWM_GEN_1)
#define low() PWMGenDisable(PWM_BASE, PWM_GEN_1)
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>PWM (pwmgen)</h1>
//!
//! This example application utilizes the PWM peripheral to output a 25% duty
//! cycle PWM signal and a 75% duty cycle PWM signal, both at 440 Hz.  Once
//! configured, the application enters an infinite loop, doing nothing while
//! the PWM peripheral continues to output its signals.
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




void send_header()
{
	//帧头
	high();
	_delay_us(3000);
	low();
	_delay_us(3000);
	high();
	_delay_us(3000);
	low();
	_delay_us(4500);
}

//发送1bit
void send_bit(int i)
{
	high();
	_delay_us(500);
	low();
	_delay_us(500+i*1000);
}

void send_tail(void)
{
	high();
	_delay_us(500);
	low();
}
//发送1字节,从高位到低位发
void send_byte(unsigned int num)
{
	int i;
	for(i=7;i>=0;i--)
	send_bit((num>>i)&0x01);
}


unsigned int code[14]={0xa6,0x92,0x00,0x00,0x40,0xa0,0x00,0x20,0x00,0x00,0x00,0x00,0x15,0x4d};

void send_body()
{
	int i;
	send_header();
	for(i=0;i<14;i++)
	send_byte(code[i]);
	send_tail();
}






//*****************************************************************************
//
// This example demonstrates how to setup the PWM block to generate signals.
//
//*****************************************************************************
int
main(void)
{
    unsigned long ulPeriod;

    //
    // Set the clocking to run directly from the crystal.
    //
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

    //
    // Initialize the OLED display.
    //
    RIT128x96x4Init(1000000);

    //
    // Clear the screen and tell the user what is happening.
    //
    RIT128x96x4StringDraw("Generating PWM", 18, 24, 15);
    RIT128x96x4StringDraw("on PF0 and PG1", 18, 32, 15);

    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

    //
    // Set GPIO F0 and G1 as PWM pins.  They are used to output the PWM0 and
    // PWM1 signals.
    //
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_0);
//    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);

    //
    // Compute the PWM period based on the system clock.
    //
    ulPeriod = SysCtlClockGet() / 38000;

    //
    // Set the PWM period to 440 (A) Hz.
    //
    PWMGenConfigure(PWM_BASE, PWM_GEN_1,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_BASE, PWM_GEN_1, ulPeriod);

    //
    // Set PWM0 to a duty cycle of 25% and PWM1 to a duty cycle of 75%.
    //
    PWMPulseWidthSet(PWM_BASE, PWM_OUT_2, ulPeriod*3 / 4);
//    PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, ulPeriod * 3 / 4);

    //
    // Enable the PWM0 and PWM1 output signals.
    //
//    PWMOutputState(PWM_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);
    PWMOutputState(PWM_BASE, PWM_OUT_2_BIT, true);
    //
    // Enable the PWM generator.
    //
    PWMGenEnable(PWM_BASE, PWM_GEN_1);

    //
    // Loop forever while the PWM signals are generated.
    //
    while(1)
    {
    	send_body();
    	_delay_ms(3000);
    
    }
    
    
    while(1)
    {
    	_delay_ms(500);
    	GPIO_PORTB_DATA_R |= 0x01;
    	PWMGenEnable(PWM_BASE, PWM_GEN_1);
    	_delay_ms(500);
    	GPIO_PORTB_DATA_R &= ~(0x01);
    	PWMGenDisable(PWM_BASE, PWM_GEN_1);
    }
}


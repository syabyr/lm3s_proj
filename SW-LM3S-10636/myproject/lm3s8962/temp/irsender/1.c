/*
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "uartstdio.h"
#include "driverlib/uart.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "drivers/mydelay.h"
*/
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

/*
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
void send_bit(uint8_t i)
{
	high();
	_delay_us(500);
	low();
	_delay_us(500+i*1000);
}

//发送1字节,从高位到低位发
void send_byte(uint8_t num)
{
	int i;
	for(i=7;i>=0;i--)
	send_bit((num>>i)&0x01);
}
*/
int main(void)
{
	unsigned long ulPeriod;
	//使能PLL,系统时钟为50MHz
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
	ulPeriod = SysCtlClockGet() / 3400;
	PWMGenConfigure(PWM_BASE, PWM_GEN_0,
		PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, ulPeriod);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, ulPeriod / 2);
	PWMOutputState(PWM_BASE, PWM_OUT_0_BIT, true);
	PWMGenEnable(PWM_BASE, PWM_GEN_0);
	while(1)
	{
	
	}
}

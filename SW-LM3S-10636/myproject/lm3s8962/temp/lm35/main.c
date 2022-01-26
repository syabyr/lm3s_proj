#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "I2CINT.H"
#include "rit128x96x4.h"
#ifndef uchar 
#define uchar  unsigned char
#endif
#define SDA      GPIO_PIN_3                                             
#define SCL      GPIO_PIN_2                                             

int ReadData (void)
{
    uchar ucLM75A = 0X90;                                               
	/*  lm75A器件地址               */    
    uchar ucLM75ATEMP = 0x00;                                           
	/*  温度寄存器地址              */
    uchar ucbuf[2] = {0};                                               
	/*  数据缓冲区                  */
    int   iTemp = 0;                                                    
	/*  温度数据                    */
    IRcvStr(ucLM75A, ucLM75ATEMP, 1, ucbuf, 2);                         
	/*  读温度数据                  */ 
    iTemp = ucbuf[0];
    iTemp <<= 8;
    iTemp += ucbuf[1];
	iTemp >>=5;
	iTemp *=12;
    return (iTemp);
}

void delay(unsigned long num)
{
	while(num--);
}
int main()
{
    int   temp = 0;
    uchar a[6]={"  .  C"};                                                               
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);                                                      
	
    RIT128x96x4Init(1000000);//液晶显示初始化
	RIT128x96x4StringDraw("wendu lm75", 30, 24, 15);                                                                
	I2CInit(100000, 1);//I2C初始化
    while (1) 
	{
		temp=ReadData();
		a[0]=temp/1000+0x30;
		a[1]=temp/100%10+0x30;
		a[3]=temp/10%10+0x30;
		a[4]=temp%10+0x30;
	    RIT128x96x4StringDraw(a, 30, 44, 15);
		//显示
		delay(1000000);
    }
}

#include "include.h"
unsigned char delay=0;
void TA1_Init()							//0.5s定时器设置,ACLK实现。
{
	TA1CTL |= TASSEL_1 + ID_3 + MC_1;	//时钟配置中，ACLK=4096Hz。TA1CLK对ACLK再8分频，频率4096/8=512Hz
	TA1CCTL0 = CCIE;
	TA1CCR0 = 255;						//256个计数产生一个中断，中断频率512/256=2Hz 约0.5s产生一次中断
}

void Delay_10us(unsigned char n)		//延时n*10微秒
{
	TAR=0;								//清除TAR，重新配置定时器A0
	TACTL |= TASSEL_2 + ID_0 + MC_1;	//SMCLK=16M
	TACCR0 = 159;						//10us产生一个中断
	TACCTL0=CCIE;						//开启中断
	if(n)n--;
	if(n)n--;else n=0;
	delay=n;							//延时n-2个10us
	if(delay)LPM0;						//进入LPM0，等待定时器中断唤醒
	TACTL=TACLR;						//关闭TA0
	__delay_cycles(80);					//这样的延时时间不够，再延时80个机器周期
}

  
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
	if(delay)delay--;						//如果delay不等于0，自减delay
	else									//其他情况，即delay=0
	{
		__low_power_mode_off_on_exit();		//结束延时，唤醒CPU
		TACCTL0 &= ~CCIE;					//关闭TA0捕获中断
	}

}

#pragma vector=TIMER1_A0_VECTOR				//0.5秒一次中断（由Timer_A1所产生）
__interrupt void Timer_A1 (void)
{
	LPM3_EXIT;    							//唤醒CPU，准备发送下一帧数据
}
  
  
  
  
  
  
  
  
  
  


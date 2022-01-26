#include "include.h"
unsigned char delay=0;
void TA1_Init()							//0.5s��ʱ������,ACLKʵ�֡�
{
	TA1CTL |= TASSEL_1 + ID_3 + MC_1;	//TA1CLK=4096/8=512Hz
	TA1CCTL0 = CCIE;
	TA1CCR0 = 255;						//256����������һ���жϣ��ж�Ƶ��512/256=2Hz
}

void Delay_10us(unsigned char n)					//��ʱ10΢��
{
	TAR=0;
	TACTL |= TASSEL_2 + ID_0 + MC_1;	//SMCLK=16M
	TACCR0 = 159;						//10us����һ���ж�
	TACCTL0=CCIE;						//�����ж�
	if(n)n--;
	if(n)n--;else n=0;
	delay=n;
	if(delay)LPM0;
	TACTL=TACLR;
	__delay_cycles(80);
}

  
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
	if(delay)delay--;
	else
	{
		__low_power_mode_off_on_exit();    //����CPU
		TACCTL0 &= ~CCIE;
	}

}

#pragma vector=TIMER1_A0_VECTOR//0.5��һ���жϣ���Timer_A1��������
__interrupt void Timer_A1 (void)
{
	LPM3_EXIT;    //����CPU
}
  
  
  
  
  
  
  
  
  
  


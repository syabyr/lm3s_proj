#include "include.h"
unsigned char delay=0;
void TA1_Init()							//0.5s��ʱ������,ACLKʵ�֡�
{
	TA1CTL |= TASSEL_1 + ID_3 + MC_1;	//ʱ�������У�ACLK=4096Hz��TA1CLK��ACLK��8��Ƶ��Ƶ��4096/8=512Hz
	TA1CCTL0 = CCIE;
	TA1CCR0 = 255;						//256����������һ���жϣ��ж�Ƶ��512/256=2Hz Լ0.5s����һ���ж�
}

void Delay_10us(unsigned char n)		//��ʱn*10΢��
{
	TAR=0;								//���TAR���������ö�ʱ��A0
	TACTL |= TASSEL_2 + ID_0 + MC_1;	//SMCLK=16M
	TACCR0 = 159;						//10us����һ���ж�
	TACCTL0=CCIE;						//�����ж�
	if(n)n--;
	if(n)n--;else n=0;
	delay=n;							//��ʱn-2��10us
	if(delay)LPM0;						//����LPM0���ȴ���ʱ���жϻ���
	TACTL=TACLR;						//�ر�TA0
	__delay_cycles(80);					//��������ʱʱ�䲻��������ʱ80����������
}

  
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
	if(delay)delay--;						//���delay������0���Լ�delay
	else									//�����������delay=0
	{
		__low_power_mode_off_on_exit();		//������ʱ������CPU
		TACCTL0 &= ~CCIE;					//�ر�TA0�����ж�
	}

}

#pragma vector=TIMER1_A0_VECTOR				//0.5��һ���жϣ���Timer_A1��������
__interrupt void Timer_A1 (void)
{
	LPM3_EXIT;    							//����CPU��׼��������һ֡����
}
  
  
  
  
  
  
  
  
  
  


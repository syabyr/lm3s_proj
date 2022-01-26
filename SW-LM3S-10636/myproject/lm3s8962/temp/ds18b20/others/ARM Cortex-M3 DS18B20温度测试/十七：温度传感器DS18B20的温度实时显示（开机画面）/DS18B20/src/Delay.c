#include "stm32f10x_lib.h"
#include "Delay.h"

//��ʱNms
//ע��Nms�ķ�Χ
//Nms<=0xffffff*8/SYSCLK
//��72M������,Nms<=1864 
void Delay_ms(u16 Nms)
{    
 SysTick->LOAD=(u32)Nms*9000; //����װ�ؼĴ�����ֵ��9000ʱ��������1ms��ʱ�� 
 SysTick->CTRL|=0x01;               //��ʼ����    
 while(!(SysTick->CTRL&(1<<16)));   //�ȴ�ʱ�䵽�� 
 SysTick->CTRL=0X00000000;         //�رռ�����
 SysTick->VAL=0X00000000;           //��ռ�����     
}   
//��ʱus           
void Delay_us(u32 Nus)
{ 
 SysTick->LOAD=Nus*9;       //ʱ�����      
 SysTick->CTRL|=0x01;            //��ʼ����    
 while(!(SysTick->CTRL&(1<<16)));//�ȴ�ʱ�䵽�� 
 SysTick->CTRL=0X00000000;       //�رռ�����
 SysTick->VAL=0X00000000;        //��ռ�����     
}  

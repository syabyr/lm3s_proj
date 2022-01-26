#include<stm32f10x_lib.h>
#include<string.h>
#include<stdio.h>
#include"ds18b20.h"
#include"STM32_Init.h"
#include"TFT200.h"

#define led1 (1<<6)	//led1����ָʾ�Ƿ񳬹�ָ���¶�
#define alarm 30    //�����¶�30
int value=0;
char s[];
int main(void)
{
	stm32_Init();
	LCD_Init();
	LCD_Clear_Screen(Blue);
	LCD_PutString(50,30,"DS18B20����ʵ��",Red,Yellow);

	for(;;)
	{
		value=readtemperature();
		if(value>=30)
		GPIOB->ODR|=led1;  //����¶ȳ���30�ȣ���led1��
		else
		GPIOB->ODR&=~led1;  //����30�ȣ�����led1
		sprintf(s,"�¶�:%d",value); 
		LCD_PutString(70,70,s,Red,Yellow);
	}
		
}







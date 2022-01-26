#include<stm32f10x_lib.h>
#include<string.h>
#include<stdio.h>
#include"ds18b20.h"
#include"STM32_Init.h"
#include"TFT200.h"

#define led1 (1<<6)	//led1用来指示是否超过指定温度
#define alarm 30    //警戒温度30
int value=0;
char s[];
int main(void)
{
	stm32_Init();
	LCD_Init();
	LCD_Clear_Screen(Blue);
	LCD_PutString(50,30,"DS18B20测温实验",Red,Yellow);

	for(;;)
	{
		value=readtemperature();
		if(value>=30)
		GPIOB->ODR|=led1;  //如果温度超过30度，则led1亮
		else
		GPIOB->ODR&=~led1;  //低于30度，则灭led1
		sprintf(s,"温度:%d",value); 
		LCD_PutString(70,70,s,Red,Yellow);
	}
		
}







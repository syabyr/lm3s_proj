/**********************************************************

        +------------------------------------------------+
        |   振南电子  开机画面（DS18B20)实验 演示程序 |
        +------------------------------------------------+

  此源码版权属 振南电子 全权享有，如欲引用，敬请署名并告知
        严禁随意用于商业目的，违者必究，后果自负
         振南电子 产品网站 http://www.znmcu.cn/
         论坛     http://bbs.znmcu.cn/
							  
***********************************************************/
#include "stm32f10x_lib.h"
#include "picture.h"
#include "chineselib.h"
#include "ili9325.h"
#include "18b20.h"
#include "Delay.h"

void RCC_Configuration(void);
void NVIC_Configuration(void);

int main(void)
{
  unsigned char Temperature_str[3]; //整数最高温度为125摄氏度（3位数据,整数部分）

  /* System Clocks Configuration */
  RCC_Configuration();   

   /* NVIC Configuration */
  NVIC_Configuration();
  
  ILI9325_Init();
  ILI9325_Delay(500);
  
  ILI9325_FullScreen(Yellow);//黄色背景

  ILI9325_DrawBMP(0,0,240,80,picture_1);

  
  //横向显示“振南电子欢迎您”蓝色汉字，黄色背景
  ILI9325_DisplayHanZi_48(24,80,ChineseLib_48,Blue,Yellow);
  ILI9325_DisplayHanZi_48(72,80,ChineseLib_48+288,Blue,Yellow);
  ILI9325_DisplayHanZi_48(120,80,ChineseLib_48+576,Blue,Yellow);
  ILI9325_DisplayHanZi_48(168,80,ChineseLib_48+864,Blue,Yellow);
  
  ILI9325_DisplayHanZi_48(48,128,ChineseLib_48+1152,Blue,Yellow);
  ILI9325_DisplayHanZi_48(96,128,ChineseLib_48+1440,Blue,Yellow);
  ILI9325_DisplayHanZi_48(144,128,ChineseLib_48+1728,Blue,Yellow);
  
  //横向显示”WWW.ZNMCU.CN“蓝色字符，黄色背景
  ILI9325_DisplayString_At(28,176,"W W W . Z N M C U . C N",Blue,Yellow);
  
  ILI9325_DrawBMP(0,208,240,112,picture_2);
  //横向显示“环境温度：℃
  ILI9325_DisplayHanZi_16(64,192,ChineseLib_16,Red,Yellow);//显示一个汉字16*16
  ILI9325_DisplayHanZi_16(80,192,ChineseLib_16+32,Red,Yellow);
  ILI9325_DisplayHanZi_16(96,192,ChineseLib_16+64,Red,Yellow);
  ILI9325_DisplayHanZi_16(112,192,ChineseLib_16+96,Red,Yellow);
  ILI9325_DisplayHanZi_16(128,192,ChineseLib_16+128,Red,Yellow);
  ILI9325_DisplayHanZi_16(160,192,ChineseLib_16+160,Red,Yellow);
  
   
   while(1)
   {
      u32tostr(Read_Temperature(), Temperature_str);//将读出的数据转换成字符形式以显示
   
      ILI9325_DisplayString_At(144,192,"  ",Blue,Yellow);//刷新温度显示的区域
      ILI9325_DisplayString_At(144,192,Temperature_str,Blue,Yellow);   
   };
   
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;

  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }

}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the NVIC and Vector Table base address.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  
#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}




#include "stm32f10x_lib.h"
#include "18b20.h"
#include "Delay.h"

void GPIO_DQ_Out_Mode(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
  GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void GPIO_DQ_Input_Mode(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //浮空输入
  GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void Tx_ResetPulse(void)
{
  GPIO_DQ_Out_Mode();
  DQ_Write_0(); //复位脉冲
  Delay_us(500);//至少保持480us
  DQ_Write_1();  //加速上升沿速度
  Delay_us(1);  
}

void Rx_PresencePulse(void)
{
  GPIO_DQ_Input_Mode();//主机释放总线
  while( DQ_ReadBit());  //等待DS18B20应答 
  while(DQ_ReadBit()==0); //DS18B20将总线拉低60~240us，然后总线由上拉电阻拉高
  Delay_us(300);
  GPIO_DQ_Out_Mode();  //接收完成，主机重新控制总线
}

void Write_OneByte_ToDS18B20(unsigned char data)
{
  unsigned char i;
  GPIO_DQ_Out_Mode();
  for(i=0;i<8;i++)
  {
    if(data&0x01)   //低位在前
    {
      //写1
      DQ_Write_0(); //写时间空隙总是从总线的低电平开始的
      Delay_us(8);//延时使得15us内拉高
      DQ_Write_1();
      Delay_us(80); //整个写1时隙不低于60us 
      
    }
    else
    {
      //写0
      DQ_Write_0();
      Delay_us(110);//保持在60us到120us之间
      DQ_Write_1();
      Delay_us(5);
    }
    data=data>>1; //取下一位
  }
}

unsigned char Read_OneByte_FromDS18B20(void)
{
  unsigned char i,data=0;
  
  for(i=0;i<8;i++)
  {
    GPIO_DQ_Out_Mode();
    data>>=1;
    DQ_Write_0();  //读时隙发出
    Delay_us(2);
    GPIO_DQ_Input_Mode();
    Delay_us(1);
    if(DQ_ReadBit())
    {
      data|=0x80;
    }
    Delay_us(70); //等待这一位数据完成传输
  }
  GPIO_DQ_Out_Mode();
  return(data); 
}




unsigned int Read_Temperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  //volatile unsigned char c=0;
  //volatile unsigned char d=0;
  //volatile unsigned char e=0;
  unsigned char t=0;
 
  
  DS18B20_Init();
  Write_OneByte_ToDS18B20(ROM_Skip_Cmd);//跳过读序列号操作
  Write_OneByte_ToDS18B20(Convert_T); //启动温度转换
  Delay_ms(780);//等待DS18B20转换完成
  
  DS18B20_Init();
  Write_OneByte_ToDS18B20(ROM_Skip_Cmd);
  Write_OneByte_ToDS18B20(Read_Scratchpad); //读去寄存器内容（可以从寄存器0一直到寄存器8）
  
  a= Read_OneByte_FromDS18B20();//温度的低8位
  b= Read_OneByte_FromDS18B20();//温度的高8位
  //c= Read_OneByte_FromDS18B20();  //Th
  //d=Read_OneByte_FromDS18B20();   //Tl
  //e=Read_OneByte_FromDS18B20();   //Configuration Register
  Tx_ResetPulse(); //中断数据的读取
  t=(b<<4)|(a>>4); //只取温度值的整数部分
  return(t); 
}

void Write_EEPROM(unsigned char Th,unsigned char Tl,unsigned char Register_Con )
{
  
  DS18B20_Init();
  Write_OneByte_ToDS18B20(ROM_Skip_Cmd);
  Write_OneByte_ToDS18B20(Write_Scratchpad);
  
  Write_OneByte_ToDS18B20(Th);//Th=7F
  Write_OneByte_ToDS18B20(Tl);//Tl=FF（最高位为符号位）
  Write_OneByte_ToDS18B20(Register_Con);//12位模式
  
  Delay_ms(700);
  DS18B20_Init();
  Write_OneByte_ToDS18B20(ROM_Skip_Cmd);
  Write_OneByte_ToDS18B20(Copy_Scratchpad);//将寄存器的配置值写入EEPROM
  Delay_ms(300);
 
}

void DS18B20_Init(void)
{
 Tx_ResetPulse();
 Rx_PresencePulse(); 
}

unsigned char Read_ROM(void)
{
  unsigned char data;
  DS18B20_Init();
  Write_OneByte_ToDS18B20(ROM_Read_Cmd);
  
  data = Read_OneByte_FromDS18B20();

  return data;
}



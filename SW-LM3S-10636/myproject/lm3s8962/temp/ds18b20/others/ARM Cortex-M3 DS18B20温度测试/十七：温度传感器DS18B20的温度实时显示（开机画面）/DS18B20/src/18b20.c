#include "stm32f10x_lib.h"
#include "18b20.h"
#include "Delay.h"

void GPIO_DQ_Out_Mode(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //��©���
  GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void GPIO_DQ_Input_Mode(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //��������
  GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void Tx_ResetPulse(void)
{
  GPIO_DQ_Out_Mode();
  DQ_Write_0(); //��λ����
  Delay_us(500);//���ٱ���480us
  DQ_Write_1();  //�����������ٶ�
  Delay_us(1);  
}

void Rx_PresencePulse(void)
{
  GPIO_DQ_Input_Mode();//�����ͷ�����
  while( DQ_ReadBit());  //�ȴ�DS18B20Ӧ�� 
  while(DQ_ReadBit()==0); //DS18B20����������60~240us��Ȼ��������������������
  Delay_us(300);
  GPIO_DQ_Out_Mode();  //������ɣ��������¿�������
}

void Write_OneByte_ToDS18B20(unsigned char data)
{
  unsigned char i;
  GPIO_DQ_Out_Mode();
  for(i=0;i<8;i++)
  {
    if(data&0x01)   //��λ��ǰ
    {
      //д1
      DQ_Write_0(); //дʱ���϶���Ǵ����ߵĵ͵�ƽ��ʼ��
      Delay_us(8);//��ʱʹ��15us������
      DQ_Write_1();
      Delay_us(80); //����д1ʱ϶������60us 
      
    }
    else
    {
      //д0
      DQ_Write_0();
      Delay_us(110);//������60us��120us֮��
      DQ_Write_1();
      Delay_us(5);
    }
    data=data>>1; //ȡ��һλ
  }
}

unsigned char Read_OneByte_FromDS18B20(void)
{
  unsigned char i,data=0;
  
  for(i=0;i<8;i++)
  {
    GPIO_DQ_Out_Mode();
    data>>=1;
    DQ_Write_0();  //��ʱ϶����
    Delay_us(2);
    GPIO_DQ_Input_Mode();
    Delay_us(1);
    if(DQ_ReadBit())
    {
      data|=0x80;
    }
    Delay_us(70); //�ȴ���һλ������ɴ���
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
  Write_OneByte_ToDS18B20(ROM_Skip_Cmd);//���������кŲ���
  Write_OneByte_ToDS18B20(Convert_T); //�����¶�ת��
  Delay_ms(780);//�ȴ�DS18B20ת�����
  
  DS18B20_Init();
  Write_OneByte_ToDS18B20(ROM_Skip_Cmd);
  Write_OneByte_ToDS18B20(Read_Scratchpad); //��ȥ�Ĵ������ݣ����ԴӼĴ���0һֱ���Ĵ���8��
  
  a= Read_OneByte_FromDS18B20();//�¶ȵĵ�8λ
  b= Read_OneByte_FromDS18B20();//�¶ȵĸ�8λ
  //c= Read_OneByte_FromDS18B20();  //Th
  //d=Read_OneByte_FromDS18B20();   //Tl
  //e=Read_OneByte_FromDS18B20();   //Configuration Register
  Tx_ResetPulse(); //�ж����ݵĶ�ȡ
  t=(b<<4)|(a>>4); //ֻȡ�¶�ֵ����������
  return(t); 
}

void Write_EEPROM(unsigned char Th,unsigned char Tl,unsigned char Register_Con )
{
  
  DS18B20_Init();
  Write_OneByte_ToDS18B20(ROM_Skip_Cmd);
  Write_OneByte_ToDS18B20(Write_Scratchpad);
  
  Write_OneByte_ToDS18B20(Th);//Th=7F
  Write_OneByte_ToDS18B20(Tl);//Tl=FF�����λΪ����λ��
  Write_OneByte_ToDS18B20(Register_Con);//12λģʽ
  
  Delay_ms(700);
  DS18B20_Init();
  Write_OneByte_ToDS18B20(ROM_Skip_Cmd);
  Write_OneByte_ToDS18B20(Copy_Scratchpad);//���Ĵ���������ֵд��EEPROM
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



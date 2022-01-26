#ifndef __18B20_H
#define __18B20_H

#include "stm32f10x_lib.h"

typedef enum {FAILED = 0,PASSED = !FAILED} TestStatus;

// ROM COMMANDS
#define ROM_Search_Cmd 0xF0
#define ROM_Read_Cmd   0x33
#define ROM_Match_Cmd  0x55
#define ROM_Skip_Cmd   0xCC
#define ROM_AlarmSearch_Cmd 0xEC

// DS18B20 FUNCTION COMMANDS
#define Convert_T         0x44
#define Write_Scratchpad  0x4E
#define Read_Scratchpad   0xBE
#define Copy_Scratchpad   0x48
#define Recall_EEPROM     0xB8
#define Read_PowerSupply  0xB4


#define DQ_Write_1()   GPIO_SetBits(GPIOD,GPIO_Pin_4) //д1
#define DQ_Write_0()   GPIO_ResetBits(GPIOD,GPIO_Pin_4) //д0
#define DQ_ReadBit()  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4)//��DQ�ϵ�ֵ

void GPIO_DQ_Out_Mode(void); //���ģʽ
void GPIO_DQ_Input_Mode(void); //����ģʽ

void Tx_ResetPulse(void); //��λ����
void Rx_PresencePulse(void);//Ӧ������

void DS18B20_Init(void);//��ʼ��

void Write_OneByte_ToDS18B20(unsigned char data); //��DS18B20д��һ���ֽ�����
unsigned char Read_OneByte_FromDS18B20(void);//��DS18B20����һ���ֽ�����

unsigned int Read_Temperature(void); //�����¶�ֵ

//д���ò���TH,TL�ͳ�ʼ�����üĴ���
void Write_EEPROM(unsigned char Th,unsigned char Tl,unsigned char Register_Con);

unsigned char Read_ROM(void); //����ROM

#endif

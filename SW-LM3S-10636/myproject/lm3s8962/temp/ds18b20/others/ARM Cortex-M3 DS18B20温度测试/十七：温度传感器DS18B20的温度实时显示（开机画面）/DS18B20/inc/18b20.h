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


#define DQ_Write_1()   GPIO_SetBits(GPIOD,GPIO_Pin_4) //写1
#define DQ_Write_0()   GPIO_ResetBits(GPIOD,GPIO_Pin_4) //写0
#define DQ_ReadBit()  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4)//读DQ上的值

void GPIO_DQ_Out_Mode(void); //输出模式
void GPIO_DQ_Input_Mode(void); //输入模式

void Tx_ResetPulse(void); //复位脉冲
void Rx_PresencePulse(void);//应答脉冲

void DS18B20_Init(void);//初始化

void Write_OneByte_ToDS18B20(unsigned char data); //向DS18B20写入一个字节数据
unsigned char Read_OneByte_FromDS18B20(void);//从DS18B20读出一个字节数据

unsigned int Read_Temperature(void); //读出温度值

//写配置参数TH,TL和初始化配置寄存器
void Write_EEPROM(unsigned char Th,unsigned char Tl,unsigned char Register_Con);

unsigned char Read_ROM(void); //读出ROM

#endif

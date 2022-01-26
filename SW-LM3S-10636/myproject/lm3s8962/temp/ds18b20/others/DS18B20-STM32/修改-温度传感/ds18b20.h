#ifndef _DS18B20_H_
#define _DS18B20_H_
#include"mytype.h"

#define skipRom 0xcc
#define convert 0x44
#define readTemp 0xbe

void Init_ds18b20(void);
U8 readtemperature(void);
#endif





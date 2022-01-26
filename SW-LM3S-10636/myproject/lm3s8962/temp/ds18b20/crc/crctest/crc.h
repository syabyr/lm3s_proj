#ifndef _CRC8_H_
#define _CRC8_H_

//#include <stdint.h>
#include <inttypes.h>

//Dallas器件上使用的CRC8算法
//input 1:要校验数据的首字节指针
//input 2:要校验数据的字节数

//计算的方法求得CRC
extern uint8_t crc8(uint8_t* data_in,uint16_t number_of_bytes_to_read);
//查表的方法求得CRC
extern uint8_t crct(uint8_t* data_in,uint16_t number_of_bytes_to_read);
#endif

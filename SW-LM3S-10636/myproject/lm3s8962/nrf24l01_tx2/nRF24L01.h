/* nRF24L01.h
 * Register definitions for manipulating the Nordic Semiconductor
 * nRF24L01+ RF transceiver chipsets.
 *

    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>
    Some parts copyright (c) 2012 Eric Brundick <spirilis [at] linux dot com>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
 */
#ifndef _NRF24L01_H
#define _NRF24L01_H



#include "nrf_lm3s_hal.h"
#include "hal_nrf_reg.h"



#define REGISTER_MASK   0x1F


    
/* Bit Mnemonics */

#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define ARD         4
#define ARC         0


#define RX_P_NO     1
#define EN_ACK_PAY  1
/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2


#define RX_PLOAD_WIDTH  32      // 20 uints TX payload
#define TX_PLOAD_WIDTH  32      // 20 uints TX payload


#define _BV(x) (1<<(x))

extern const unsigned char TX_ADDRESS[]; // Define a static TX address ;
extern const unsigned char RX_ADDRESS[]; // Define

///////////////////////////////////////////////////////////////////
uint8_t nrf_write_reg(uint8_t reg,uint8_t value);
uint8_t nrf_read_reg(uint8_t reg);

void set_module_tx();


uint8_t nrf_write_buf(uint8_t reg,const uint8_t buf[], const uint8_t len);
uint8_t nrf_read_buf(uint8_t reg,uint8_t buf[], uint8_t len);


//接收
uint8_t nrf_rx_packet(uint8_t *rxbuf);
//发送
uint8_t nrf_tx_packet(uint8_t *txbuf);


//清除TX FIFO寄存器.发射模式下用
uint8_t flush_tx(void);

//清除RX FIFO寄存器.接收模式下用
uint8_t flush_rx(void);

void setPALevel(uint8_t level);
uint8_t getPALevel(void);

bool setDataRate(hal_nrf_datarate_t speed);
hal_nrf_datarate_t getDataRate( void );

void setCRCLength(hal_nrf_crc_mode_t length);
hal_nrf_crc_mode_t getCRCLength(void);

void setChannel(uint8_t channel);
uint8_t getChannel();

void setAutoAckAll(bool enable);
void setAutoAck(uint8_t pipe,bool enable);
void setRetries(uint8_t delay, uint8_t count);

void init_NRF24L01(void);

void openWritingPipe(uint64_t value);
void openReadingPipe(uint8_t child, uint64_t address);

void writeFast( const void* buf, uint8_t len );

bool isPVariant(void);

bool available(uint8_t* pipe_num);

uint8_t get_status(void);
//打印
void printDetails(void);
void print_status(uint8_t status);
void print_address_register(const char* name, uint8_t reg, uint8_t qty);
void print_byte_register(const char* name, uint8_t reg, uint8_t qty);

#endif
#ifndef NRF_LM3S_HAL_H__
#define NRF_LM3S_HAL_H__
#include "inc/hw_types.h"
#include <inttypes.h>
#include <stdbool.h>



void set_nrf24l01_ce_h();
void set_nrf24l01_ce_l();
void set_nrf24l01_csn_h();
void set_nrf24l01_csn_l();

//SPI操作层
uint8_t spi_send_byte(uint8_t data);

//
uint8_t spi_write_reg(const uint8_t reg,const uint8_t value);
uint8_t spi_read_reg(uint8_t reg);


#endif
/*
 * main.h
 *
 *  Created on: 16.05.2009
 *      Author: paul
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <avr/eeprom.h>
extern EEMEM uint16_t max_temp;


void init_main(void);
void init_ports(void);
uint8_t get_key(int key_id);
void read_temp(void);
void ds18x20_search_id(void);
int main(void);
void lcd_put_temp(void);
uint16_t temp_convert(const uint8_t subzero, uint8_t cel, uint8_t cel_frac_bits);
uint8_t search_sensors(void);
void led_off(void);
void led_on(void);
void temp_compare(void);
void init_interrupt(void);
#endif /* MAIN_H_ */

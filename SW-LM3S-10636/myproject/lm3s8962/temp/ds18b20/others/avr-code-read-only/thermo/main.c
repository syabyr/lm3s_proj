/*
 * main.c
 *
 *  Created on: 16.05.2009
 *      Author: paul
 *      eeprom:
 *      0xC6 - включено, 0xFF - выключено
 *      0-5 - кабинет 419 (активен, уставка, оповещение)
 *      10-15 - кабинет 422 (активен, уставка, оповещение)
 */
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "main.h"
#include "delay.h"
#include "lcd.h"
#include "onewire.h"
#include "ds18x20.h"

volatile int timer1=0;
volatile int alert=0;
volatile int beep_off=0;
volatile int key_pressed=0;
char lcd_buffer[33];
unsigned char ds18b20_buffer[16];
uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];
uint16_t kb418=0;
uint16_t kb422=0;
uint16_t kb225=0;
uint8_t kb418_alarm = 0;
uint8_t kb422_alarm = 0;
uint8_t kb225_alarm = 0;
unsigned char alarm_message[33];

EEMEM uint16_t max_temp = 27;
EEMEM unsigned char kb418_rom[16] = "28138A8001000018";
EEMEM unsigned char kb422_rom[16] = "2880DB9301000018";
EEMEM unsigned char kb225_rom[16] = "0000000000000000";
uint8_t kb418_id = 99;
uint8_t kb422_id = 99;
uint8_t kb225_id = 99;

void led_on(void) {
	PORTC |= _BV(PC5);
}
void led_off(void) {
	PORTC &= ~_BV(PC5);
}
uint8_t nSensors=0;

uint8_t search_sensors(void) {
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;


	nSensors = 0;
	for( diff = OW_SEARCH_FIRST; diff != OW_LAST_DEVICE && nSensors < MAXSENSORS ; ) {
		DS18X20_find_sensor( &diff, &id[0] );
		if( diff == OW_PRESENCE_ERR ) {
			lcd_clear();
			_delay_ms(5);
//			lcd_cursor_home();
//			lcd_puts("Sensors not");
			lcd_cursor_line2();
			lcd_puts("Sens. not found!");
			alert=1;
			_delay_ms(1500);
			break;
		}
		if( diff == OW_DATA_ERR ) {
			lcd_clear();
			_delay_ms(5);
			lcd_cursor_line2();
			lcd_puts("Bus Error!");
			alert=1;
			_delay_ms(1500);
			break;
		}
		for (i=0;i<OW_ROMCODE_SIZE;i++) {
			gSensorIDs[nSensors][i]=id[i];
		}
		nSensors++;
		alert=0;
	}
	return nSensors;
}

uint16_t temp_convert(const uint8_t subzero, uint8_t cel, uint8_t cel_frac_bits) {
	uint8_t buffer[sizeof(int)*8+1];
	uint16_t decicelsius;
	uint8_t j;
	itoa((cel_frac_bits*DS18X20_FRACCONV),buffer,10);
	j=4-strlen(buffer);
	// "rounding"
	decicelsius = DS18X20_temp_to_decicel(subzero, cel, cel_frac_bits);
	return decicelsius;
}
void temp_compare(void) {
	if ((((int)(kb422/10) >= eeprom_read_word(&max_temp)) &&  ((int)(kb422/10) < 85)) || (((int)(kb418/10) >= eeprom_read_word(&max_temp))&&  ((int)(kb418/10) < 85)) || (((int)(kb225/10) >= eeprom_read_word(&max_temp))&&  ((int)(kb225/10) < 85))) {
		timer1 = 1;
		if (((int)(kb422/10) >= eeprom_read_word(&max_temp)) &&  ((int)(kb422/10) < 85)) {
			kb422_alarm = 1;
			sprintf(alarm_message, "Каб.422 - %d.%d C", (int)(kb422/10),(int)(kb422%10));
		} else {
			kb422_alarm = 0;
		}
		if (((int)(kb418/10) >= eeprom_read_word(&max_temp)) &&  ((int)(kb418/10) < 85)) {
			kb418_alarm = 1;
			sprintf(alarm_message, "Каб.418 - %d.%d C", (int)(kb418/10),(int)(kb418%10));
		} else {
			kb418_alarm = 0;
		}
		if (((int)(kb225/10) >= eeprom_read_word(&max_temp)) &&  ((int)(kb225/10) < 85)) {
			kb225_alarm = 1;
			sprintf(alarm_message, "Каб.225 - %d.%d C", (int)(kb225/10),(int)(kb225%10));
		} else {
			kb225_alarm = 0;
		}

	} else {
		timer1 = 0;
		beep_off = 0;
		key_pressed = 0;
	}
}
void lcd_put_temp(void) {
	lcd_clear();
	_delay_ms(5);
	lcd_cursor_home();
	lcd_puts("Каб. 418: ");
	sprintf(lcd_buffer,"%d.%d", (int)(kb418/10),(int)(kb418%10));
	lcd_puts(lcd_buffer);
	lcd_putc2(0xdf);
	lcd_puts("C");
	lcd_cursor_line2();
	lcd_puts("Каб. 422: ");
	sprintf(lcd_buffer,"%d.%d", (int)(kb422/10),(int)(kb422%10));
	lcd_puts(lcd_buffer);
	lcd_putc2(0xdf);
	lcd_puts("C");
}

void init_main(void) {
	#ifndef OW_ONE_BUS
	ow_set_bus(&PIND,&PORTD,&DDRD,PD2);
	#endif
	SEI();
	DDRD = 0xF0;                    /* PD4:7 digital outputs */
	/* turn on backlight */
	lcd_backlight(1);
	/* initialize the LCD */
	lcd_initialize(LCD_FUNCTION_8x2, LCD_CMD_ENTRY_INC, LCD_CMD_ON);
}


void init_ports(void) {
	DDRC=0xFF;
	DDRA=0xFF;
	DDRB=0xF8; //PB0-PB4 - input
	//DDRB = ~_BV(0);
	PORTD=0xF8;
}
void init_interrupt(void) {
	TIMSK = _BV(TOIE1); //активация прерывания с T/C1
	TCNT1 = 0xFFE0; //Значение счетчика T/C1 (частота прерывания ~ 0.004 сек)
	TCCR1A = 0x00; //Не выводим сигнал от T/C1
	TCCR1B |= _BV(CS12)|_BV(CS10); //Прескалер clk/1024
}


ISR(SIG_OVERFLOW1) {
	TCNT1 = 0xFFE0;
	temp_compare();
	if (timer1==1) {
		led_on();
	} else {
		led_off();
	}
	if ((alert==1)&(timer1==0)) {
		led_on();
	} else if (timer1==0) {
		led_off();
	}
	if ((timer1==1) & (beep_off==0)) {
		PORTC |= _BV(PC6);
	} else {
		PORTC &= ~_BV(PC6);
	}
}


/** Опрос кнопки.
 * Функция возвращает результат опроса кнопки: 0 - не нажата, 1 - было однократное нажатие,
 * 2 - кнопка нажата и удерживается более 2,5 секунд (примерно).
 * @return - возвращаемое значение
 */
uint8_t get_key(int key_id){
  static uint8_t repeat = 0;            // флаг автоповтора кода кнопки
  int i;
  // нажатой кнопке соответствует 0 на выводе
  if(!(PINB & _BV(key_id))){
    // обнаружен 0 - отфильтруем дребезг
    _delay_ms(15);
    if(!(PINB & _BV(key_id))){
      // через 15 мс (время дребезга) все еще 0
      if(repeat) {
        // идет удержание - делаем автоповтор через 100 мс
        _delay_ms(100);
        return 2;
      }
      // проверим, будет ли кнопка удержана более 2,5 секунд
      for(i=0; i<250; i++){
        if(PINB & _BV(key_id))
          return 1; // если менее - считаем нажатие однократным
        _delay_ms(10);
      }
      // если держали 2,5 секунды - установим признак автоповтора и вернем соотв. результат
      repeat = 1;
      return 2;
    }
  }
  // если не обнаружено нажатие кнопки - вернем ноль, сбросив флаг автоповтора
  repeat = 0;
  return 0;
}

void ds18x20_search_id(void) {
	uint8_t i, j;
	nSensors = search_sensors();
	i=1;
	for (i=0; i < nSensors; i++) {
		for(j = 0; j < 8; j++ ) {
			sprintf(ds18b20_buffer,"%02X%02X%02X%02X%02X%02X%02X%02X",gSensorIDs[i][0],gSensorIDs[i][1],gSensorIDs[i][2],gSensorIDs[i][3],gSensorIDs[i][4],gSensorIDs[i][5],gSensorIDs[i][6],gSensorIDs[i][7]);
		}
		if (strcmp(ds18b20_buffer,eeprom_read_word(&kb418_rom))) {
			kb418_id = i;
		} else if (strcmp(ds18b20_buffer,eeprom_read_word(&kb422_rom))) {
			kb422_id = i;
		} else if (strcmp(ds18b20_buffer,eeprom_read_word(&kb225_rom))) {
			kb225_id = i;
		}
	}
}

void read_temp(void) {
	uint8_t i;
	uint8_t subzero, cel, cel_frac_bits;
	nSensors = search_sensors();
		if ( DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL ) == DS18X20_OK) {
			delay_ms(DS18B20_TCONV_12BIT);
			for (i=0; i < nSensors; i++) {
				if (DS18X20_read_meas(gSensorIDs[i], &subzero, &cel, &cel_frac_bits) == DS18X20_OK) {
					if (i==0) {
						kb418=temp_convert(subzero, cel, cel_frac_bits);
					} else if (i==1) {
						kb422=temp_convert(subzero, cel, cel_frac_bits);
					} else {
						kb225=temp_convert(subzero, cel, cel_frac_bits);
					}
				}
/*				if (kb225_id != 99) {
					if (DS18X20_read_meas(gSensorIDs[kb225_id], &subzero, &cel, &cel_frac_bits) == DS18X20_OK) {
						kb225=temp_convert(subzero, cel, cel_frac_bits);
					}
				}
				if (kb418_id != 99) {
					if (DS18X20_read_meas(gSensorIDs[kb418_id], &subzero, &cel, &cel_frac_bits) == DS18X20_OK) {
							kb418=temp_convert(subzero, cel, cel_frac_bits);
					}
				}
				if (kb422_id != 99) {
					if (DS18X20_read_meas(gSensorIDs[kb422_id], &subzero, &cel, &cel_frac_bits) == DS18X20_OK) {
							kb422=temp_convert(subzero, cel, cel_frac_bits);
					}
				}
*/
			}
		}
}
int main(void) {
	init_ports();
	init_main();
	CLI();
	init_interrupt();
	SEI();
	while (1) {
		nSensors = search_sensors();
		ds18x20_search_id();
		if ((nSensors < 1) && (nSensors > 0)) {
			lcd_clear();
			_delay_ms(5);
//			lcd_cursor_home();
//			lcd_puts("Alarm!");
			lcd_cursor_line2();
			lcd_puts("Sensor Error!");
			_delay_ms(2000);
		} else if (nSensors >= 1) {
			read_temp();
			_delay_ms(800);
			if (timer1==1) {
//				led_on();
				lcd_clear();
				_delay_ms(5);
//				lcd_cursor_home();
//				_delay_ms(10);
//				lcd_puts("Тревога!");
//				_delay_ms(5);
				lcd_cursor_line2();
				lcd_puts(alarm_message);
			} else {
//				lcd_clear();
				lcd_put_temp();
			}
			if (get_key(0) == 2) {
				beep_off = 1;
				key_pressed=1;
			} else if (get_key(1) & (key_pressed==0)) {
				beep_off = 0;
			}
		}
	}
	return 0;
}

/*
    File:       lcd.c
    Version:    0.1.0
    Date:       Feb. 25, 2006

    C header file for HD44780 LCD module using a 74HCT164 serial in, parallel
    out out shift register to operate the LCD in 8-bit mode.  Example schematic
    usuing a Powertip PC-1202A LCD available at http://www.micahcarrick.com

    Uses the avr-libc library for gcc (avr-gcc).

    ****************************************************************************
    Copyright (C) 2006 Micah Carrick   <email@micahcarrick.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ****************************************************************************
*/

#include "lcd.h"
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdlib.h>

//EEMEM const uint8_t TabWin[] = {
const uint8_t TabWin[] = {
0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, // псевдографика
0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, // псевдографика
0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, // псевдографика
0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, // псевдографика
0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, // псевдографика
0xa2, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, // Ё псевдографика
0xfe, 0xff, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, // псевдографика
0xb5, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xfd, 0xff, // ё псевдографика
0x41, 0xa0, 0x42, 0xa1, 0xe0, 0x45, 0xa3, 0xa4, // АБВГДЕЖЗ
0xa5, 0xa6, 0x4b, 0xa7, 0x4d, 0x48, 0x4f, 0xa8, // ИЙКЛМНОП
0x50, 0x43, 0x54, 0xa9, 0xaa, 0x58, 0xe1, 0xab, // РСТУФХЦЧ
0xac, 0xe2, 0xad, 0xae, 0x62, 0xaf, 0xb0, 0xb1, // ШЩЪЫЬЭЮЯ
0x61, 0xb2, 0xb3, 0xb4, 0xe3, 0x65, 0xb6, 0xb7, // абвгдежз
0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0x6f, 0xbe, // ийклмноп
0x70, 0x63, 0xbf, 0x79, 0xe4, 0x78, 0xe5, 0xc0, // рстуфхцч
0xc1, 0xe6, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, // шщъыьэюя
};


/*
 * Turns the backlight on or off.  The LCD_BACKLIGHT_PIN should be defined as
 * the pin connected to the backlight control of the LCD.
 *
 * Parameters:
 *      backlight_on    0=off, 1=on
*/
void
lcd_backlight(int backlight_on)
{
        if (backlight_on)
        {
                /* LCD Backlight on */
                LCD_PORT |= _BV(LCD_BACKLIGHT_PIN);
        } else {
                /*  LCD Backlight off */
                LCD_PORT &= ~_BV(LCD_BACKLIGHT_PIN);
        }
}

/*
 * Initializes the LCD.  Should be called during the initialization of the
 * program.
 *
 * Parameters:
 *      set_function    See LCD_FUNCTION_* definitions in lcd.h
 *      set_entry_mode  See LCD_CMD_ENTRY_* definitions in lcd.h
 *      on              See LCD_CMD_ON_* definitions in lcd.h
*/
void
lcd_initialize(uint8_t set_function, uint8_t set_entry_mode, uint8_t on)
{
        /* 20ms delay while LCD powers on */
        _delay_ms(16);

        /* Write 0x30 to LCD and wait 5 mS for the instruction to complete */
        lcd_load_byte(0x30);
        lcd_send_cmd();
        _delay_ms(5);

        /* Write 0x30 to LCD and wait 160 uS for instruction to complete */
        lcd_load_byte(0x30);
        lcd_send_cmd();
        _delay_us(120);

        /* Write 0x30 AGAIN to LCD and wait 160 uS */
        lcd_load_byte(0x30);
        lcd_send_cmd();
        _delay_us(120);

        /* Set function and wait 40uS */
        lcd_load_byte(set_function);
        lcd_send_cmd();

        /* Turn off the display and wait 40uS */
        lcd_load_byte(LCD_CMD_OFF);
        lcd_send_cmd();

        /* Clear display and wait 1.64mS */
        lcd_load_byte(LCD_CMD_CLEAR);
        lcd_send_cmd();
        _delay_ms(2);

        /* Set entry mode and wait 40uS */
        lcd_load_byte(set_entry_mode);
        lcd_send_cmd();

        /* Turn display back on and wait 40uS */
        lcd_load_byte(on);
        lcd_send_cmd();
};

/*
 * Loads a byte into the shift register (74'164).  Does NOT load into the LCD.
 *
 * Parameters:
 *      out_byte        The byte to load into the '164.
*/
void
lcd_load_byte(uint8_t out_byte)
{
        /* make sure clock is low */
        LCD_PORT &= ~_BV(LCD_CLOCK_PIN);

        int i;
        for(i=0; i<8; i++)
        {
                /* loop through bits */

                if (out_byte & 0x80)
                {
                        /* this bit is high */
                        LCD_PORT |=_BV(LCD_RSDS_PIN);
                }
                else
                {
                        /* this bit is low */
                        LCD_PORT &= ~_BV(LCD_RSDS_PIN);
                }
                out_byte = out_byte << 1;

                /* pulse the the shift register clock */
                LCD_PORT |= _BV(LCD_CLOCK_PIN);
                LCD_PORT &= ~_BV(LCD_CLOCK_PIN);
        }
}

/*
 * Loads the byte in the '164 shift register into the LCD as a command. The
 * '164 should already be loaded with the data using lcd_load_byte().
*/
void
lcd_send_cmd(void)
{
        /* Data in '164 is a command, so RS must be low (0) */
        LCD_PORT &= ~_BV(LCD_RSDS_PIN);
        lcd_strobe_E();
//        _delay_us(40);
}

/*
 * Loads the byte in the '164 shift register into the LCD as a character. The
 * '164 should already be loaded with the data using lcd_load_byte().
*/
void
lcd_send_char(void)
{
        /* Data in '164 is a character, so RS must be high (1) */
        LCD_PORT |= _BV(LCD_RSDS_PIN);
        lcd_strobe_E();
//        _delay_us(40);
}

/*
 * Loads the byte into the shift register and then sends it to the LCD as a char
 * Parameters:
 *      c               The byte (character) to display
*/
void
lcd_putc(const char c)
{
  unsigned char a;
  a = c;
  if (a>=0x80) {
    a -= 0x80;
//    a = eeprom_read_word(&TabWin[a]);
    a = TabWin[a];
  }
    lcd_load_byte(a);
        lcd_send_char();
}

void lcd_putc2(const char c) {
  lcd_load_byte(c);
  lcd_send_char();
}

/*
** print string on lcd  with no auto line feed
*/
void lcd_puts(const char *s)
{
    while (*s) {
        lcd_putc(*s);
        s++;
    }
}

void lcd_puts2(const char *s)
{
    while (*s) {
        lcd_putc2(*s);
        s++;
    }
}

void lcd_puti( const int val )
{
    char buffer[sizeof(int)*8+1];

    lcd_puts( itoa(val, buffer, 10) );

}

void lcd_puthex_nibble(const unsigned char b)
{
    unsigned char  c = b & 0x0f;
    if (c>9) c += 'A'-10;
    else c += '0';
    lcd_putc2(c);
}


void lcd_puthex_byte(const unsigned char  b)
{
    lcd_puthex_nibble(b>>4);
    lcd_puthex_nibble(b);
} /* uart_puthex_byte */


void lcd_puts_table(void) {
  /* for (int i=0; i<=256; i++) {

    lcd_cursor_home();
    lcd_putc(TabWin[i]);
    lcd_puts(" ");
    lcd_puti(i);
    _delay_ms(800);
    //lcd_clear();
  }
  */
  lcd_cursor_home();
  lcd_putc2(0xdf); //градус
  lcd_putc2(0xed); //колокольчик
  //lcd_clear();
}
/*
 * Strobes the E signal on the LCD to "accept" the byte in the '164.  The RS
 * line determines wheter the byte is a character or a command.
*/
void
lcd_strobe_E(void)
{
        /* strobe E signal */
        LCD_PORT |= _BV(LCD_ENABLE_PIN);
        _delay_us(450);
        LCD_PORT &= ~_BV(LCD_ENABLE_PIN);
}
void lcd_clear(void) {
        lcd_load_byte(LCD_CMD_CLEAR);
        lcd_send_cmd();
//        _delay_ms(10);
}

/*
 * Moves the cursor to the home position.
*/
void
lcd_cursor_home()
{
        lcd_load_byte(LCD_CMD_HOME);
        lcd_send_cmd();
//        _delay_ms(10);
}

/*
 * Moves the cursor to the home position.
*/
void
lcd_cursor_line2()
{
        lcd_load_byte(LCD_LINE_2);
        lcd_send_cmd();
//        _delay_ms(10);
}
/*
 * Moves the cursor to the specified position.
 * Parameters:
 *      line            Line (row)
 *      pos             Position on that line (column)
*/
void
lcd_move_cursor(uint8_t line, uint8_t pos)
{
        lcd_load_byte(line+pos);
        lcd_send_cmd();
}

/*
 * Moves the cursor a number of spaces to the right
 * Parameters:
 *      spaces          Number of spaces to move
*/
void
lcd_inc_cursor(uint8_t spaces)
{
        while (spaces--)
        {
                lcd_load_byte(LCD_CMD_CURSOR_RIGHT);
                lcd_send_cmd();
        }
}

/*
 * Moves the cursor a number of spaces to the left
 * Parameters:
 *      spaces          Number of spaces to move
*/
void
lcd_dec_cursor(uint8_t spaces)
{
        while (spaces--)
        {
                lcd_load_byte(LCD_CMD_CURSOR_LEFT);
                lcd_send_cmd();
        }
}

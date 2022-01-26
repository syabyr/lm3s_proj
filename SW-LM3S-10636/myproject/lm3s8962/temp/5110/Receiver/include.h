#include "msp430g2553.h"
#include "CHAR_USER.h"
#include "LCD.h"
#include "NRF24L01.h"
#include "TIMERA.h"

//LCD¶Ë¿Ú¶¨Òå
#define NOKIA5110_CLK_H	P2OUT |= BIT3
#define NOKIA5110_DIN_H	P2OUT |= BIT4
#define NOKIA5110_DC_H	P2OUT |= BIT5
#define NOKIA5110_CE_H	P2OUT |= BIT1
#define NOKIA5110_RST_H	P2OUT |= BIT2
#define NOKIA5110_LED_ON

#define NOKIA5110_CLK_L	P2OUT &= ~BIT3
#define NOKIA5110_DIN_L	P2OUT &= ~BIT4
#define NOKIA5110_DC_L	P2OUT &= ~BIT5
#define NOKIA5110_CE_L	P2OUT &= ~BIT1
#define NOKIA5110_RST_L	P2OUT &= ~BIT2
#define NOKIA5110_LED_OFF

#define NRF24L01_CE BIT0
#define NRF24L01_CSN BIT4
#define NRF24L01_IRQ BIT1

#define SET_NRF24L01_CE_H	P2OUT |= 	BIT0
#define SET_NRF24L01_CSN_H	P1OUT |=	BIT4

#define SET_NRF24L01_CE_L	P2OUT &= 	~BIT0
#define SET_NRF24L01_CSN_L 	P1OUT &=	~BIT4

#define NRF24L01_CE_STATE (P2OUT&BIT0)
#define NRF24L01_CSN_STATE 	(P1OUT&BIT4)


extern unsigned int Receive_Number;
extern unsigned char LCD_Leval;



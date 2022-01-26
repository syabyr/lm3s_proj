#include "msp430g2553.h"
#include "TIMERA.h"
#include "NRF24L01.h"


#define NRF24L01_CE BIT0
#define NRF24L01_CSN BIT4
#define NRF24L01_IRQ BIT1

#define SET_NRF24L01_CE_H	P2OUT |= 	BIT0
#define SET_NRF24L01_CSN_H	P1OUT |=	BIT4

#define SET_NRF24L01_CE_L	P2OUT &= 	~BIT0
#define SET_NRF24L01_CSN_L 	P1OUT &=	~BIT4

#define NRF24L01_CE_STATE (P2OUT&BIT0)
#define NRF24L01_CSN_STATE 	(P1OUT&BIT4)




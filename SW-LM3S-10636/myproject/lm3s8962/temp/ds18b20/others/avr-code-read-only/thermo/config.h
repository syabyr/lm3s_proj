/*
 * config.h
 *
 *  Created on: 16.05.2009
 *      Author: paul
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define F_OSC F_CPU
#define NOP() asm("nop")
#define SEI() asm("sei")
#define CLI() asm("cli")

#define MAXSENSORS 5
/*****************************************************************************
Types definitions
******************************************************************************/
#define UCHAR unsigned char
#define UINT  unsigned int
#define ULONG unsigned long
#define BOOL  UCHAR
#define FALSE 0
#define TRUE  1

#endif /* CONFIG_H_ */

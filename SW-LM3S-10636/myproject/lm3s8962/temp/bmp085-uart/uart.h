#ifndef __MY_UART_DRIVER_
#define __MY_UART_DRIVER_
#include<stdio.h>

extern int uart_putchar(char c,FILE *stream);
extern int uart_getchar(FILE *stream);



#endif


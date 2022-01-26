//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2005-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6459 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

#define GPIO_PB2_I2C0SCL        0x00010801
#define GPIO_PB3_I2C0SDA        0x00010C01
#define BMP085_ADDRESS	0x77
#define BMP085_ADDR   0x77
#define CTLREG        0xf4
#define READ_TEMP     0x2e
#define READ_PRESSURE 0x34
#define MSB           0xf6
#define OSRS          3

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_i2c.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "drivers/rit128x96x4.h"
#include "uart.h"
#include "mydelay.h"
#include "hs_types.h"
#include <stdint.h>
#include <stdio.h>
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>UART Echo (uart_echo)</h1>
//!
//! This example application utilizes the UART to echo text.  The first UART
//! (connected to the FTDI virtual serial port on the evaluation board) will be
//! configured in 115,200 baud, 8-n-1 mode.  All characters received on the
//! UART are transmitted back to the UART.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UART0IntHandler(void)
{
    unsigned long ulStatus;

    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
    }
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************
//FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);


void init_i2c2()
{
		//I2C
		GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_OD);
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_OD);
    
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_DIR_MODE_HW);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_DIR_MODE_HW);
    
    //GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    
    I2CMasterInitExpClk(I2C0_MASTER_BASE, SysCtlClockGet(), true);
    
    
    I2CMasterEnable(I2C0_MASTER_BASE);

    //I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,SLAVE_ADDRESS, false);
}

i2c_error read_i2c(unsigned long base,u08_t address, u08_t * data, u08_t len)
{
  u08_t uiByte;
  unsigned long i2c_error;
  int escape=0;
  //
  // Byte counter to zero..
  //
  uiByte = 0;

  //
  // Read from the XXX.  Start by setting the slave
  // address for a read (true).
  //
  I2CMasterSlaveAddrSet(base, address, true);

  //
  // Wait for bus to become free
  //
  while(I2CMasterBusy(base)){}

  //
  // Start the transfer.
  //
  if (len==1)
  {
    I2CMasterControl(base, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while(I2CMasterBusy(base)){};
    i2c_error = I2CMasterErr(base);
    if (i2c_error != 0)
    {
      return i2c_error;
    }
    data[uiByte] = (char)I2CMasterDataGet(base);
    return 0;
  }
  I2CMasterControl(base, I2C_MASTER_CMD_BURST_RECEIVE_START);
   
  //
  // Wait for transaction to complete
  //
  while(I2CMasterBusy(base)){}

  //
  // Check for errors
  //
  i2c_error = I2CMasterErr(base);
  if (i2c_error != 0)
  {
    return i2c_error;
  }

  //
  // Get data
  //
  data[uiByte++] = (char)I2CMasterDataGet(base);

  for(; uiByte < (len-1); uiByte++)
  {
    
    //
    // Continue the transfer.
    //
    I2CMasterControl(base, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
    
    //
    // Wait for the transaction to complete
    //
    while(I2CMasterBusy(base)){}

    //
    // Check for error
    //
    i2c_error = I2CMasterErr(base);
    if (i2c_error != 0)
    {
      return i2c_error;
    }

    //
    // Get the data
    //
    data[uiByte] = (char)I2CMasterDataGet(base);
  }

  //
  // Wait for the transaction to complete
  //
  while(I2CMasterBusy(base)){}

  I2CMasterControl(base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

  //
  // Check for error
  //
  i2c_error = I2CMasterErr(base);
  if (i2c_error != 0)
  {
    return i2c_error;
  }

  //
  // Wait for the transaction to complete
  //
  while(I2CMasterBusy(base))
  {}

  //
  // Get the (last) data
  //
  data[uiByte] = (char)I2CMasterDataGet(base);

}


i2c_error write_i2c(unsigned long base, u08_t address, u08_t * data, u08_t len)
{
  u08_t uiByte;
  unsigned long i2c_error;
  //
  // Byte counter to zero..
  //
  uiByte = 0;

  //
  // Read from the XXX.  Start by setting the slave
  // address for a read (true).
  //
  I2CMasterSlaveAddrSet(base, address, false);
  
  // wait for i2c master to become ready
  while(I2CMasterBusy(base)){}

  //
  // put the first characer in the i2c out buffer
  //
  I2CMasterDataPut(base,data[uiByte++]);
  
  //
  // Start the transfer.
  //
  if (len > 1)
  {
    I2CMasterControl(base, I2C_MASTER_CMD_BURST_SEND_START);
  }
  else
  {
    I2CMasterControl(base, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(base)){}
    i2c_error = I2CMasterErr(base);
    return i2c_error;
  }
  
  while(I2CMasterBusy(base)){}
  i2c_error = I2CMasterErr(base);
  if (i2c_error !=0)
  {
    return i2c_error;
  }
  

  for(; uiByte < (len-1); uiByte++)
  {
 
    while(I2CMasterBusy(base)){}

    //
    // send the data
    //
    I2CMasterDataPut(base,data[uiByte]);


    //
    // Continue the transfer.
    //
    I2CMasterControl(base, I2C_MASTER_CMD_BURST_SEND_CONT);

    //
    // Check for error
    //
    i2c_error = I2CMasterErr(base);
    if (i2c_error !=0)
    {
      return i2c_error;
    }
  }
  
  //
  // Wait for the transaction to complete
  //
  while(I2CMasterBusy(base)){}
  
  //
  // put the last character characer in the i2c out buffer
  //
  I2CMasterDataPut(base,data[uiByte]);
  
  //
  // send last character with a stop bit.
  //  
  I2CMasterControl(base, I2C_MASTER_CMD_BURST_SEND_FINISH);

  //
  // Wait for the transaction to complete
  //
  while(I2CMasterBusy(base)){}

  //
  // Check for error
  //
  i2c_error=I2CMasterErr(base);
  if (i2c_error !=0)
  {
    return i2c_error;
  }

  return 0;
}




int
main(void)
{
     SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);


    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //
    // Prompt for text to be entered.
    //
//    UARTSend((unsigned char *)"Enter text:\n", 12);
    UARTStdioInit(0);
    UARTprintf("Enter text:\n");
    UARTprintf("Hello,world\n");
    unsigned long ulClockMS,ulCount;
		unsigned short usController;

		{
			ulClockMS=SysCtlClockGet();
    	UARTprintf("%d\n",ulClockMS);

    	SysCtlDelay(2000000);
    }
    
    init_i2c2();
    
    unsigned long ulindex,ulDataRx[11];
    unsigned char cdata[22];
    
    for(ulindex = 0; ulindex < 11; ulindex++)
    {
        ulDataRx[ulindex] = 0;
    }
    for(ulindex = 0; ulindex < 22; ulindex++)
    {
        cdata[ulindex] = 0;
    }
    
		//I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SLAVE_ADDRESS, true);
		//I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
		
		u08_t temp1=0xaa;
		u08_t temp2[22];
		while(1)
    {
    		//
    		write_i2c(I2C0_MASTER_BASE,BMP085_ADDRESS, &temp1, 1);
    		for(ulindex = 0; ulindex < 22; ulindex++)
    		{
      	  temp2[ulindex] = 0;
   			}
    		read_i2c(I2C0_MASTER_BASE,BMP085_ADDRESS,&temp2,22);
    		
    		//
    		u08_t trigtemp[2];
    		u08_t readtemp;
    		u08_t tempresult[2];
    		tempresult[0]=0;
    		tempresult[1]=0;
    		trigtemp[0]=CTLREG;
    		trigtemp[1]=READ_TEMP;
    		write_i2c(I2C0_MASTER_BASE,BMP085_ADDRESS, &trigtemp, 2);
    		_delay_ms(10);
    		readtemp=MSB;
    		write_i2c(I2C0_MASTER_BASE,BMP085_ADDRESS, &readtemp, 1);
    		read_i2c(I2C0_MASTER_BASE,BMP085_ADDRESS,&tempresult,2);
    		
    		
    		//
    		u08_t trigpress[2];
    		u08_t readpress;
    		u08_t pressresult[3];
    		pressresult[0]=0;
    		pressresult[1]=0;
    		pressresult[2]=0;
    		trigpress[0]=CTLREG;
    		trigpress[1]=READ_PRESSURE;
    		write_i2c(I2C0_MASTER_BASE,BMP085_ADDRESS, &trigpress, 2);
    		_delay_ms(10);
    		readpress=MSB;
    		write_i2c(I2C0_MASTER_BASE,BMP085_ADDRESS, &readpress, 1);
    		read_i2c(I2C0_MASTER_BASE,BMP085_ADDRESS,&pressresult,2);
    		

				UARTprintf("temp:0x%02x,0x%02x press:0x%02x,0x%02x,0x%02x calc:",tempresult[0],tempresult[1],pressresult[0],pressresult[1],pressresult[2]);

				for(ulindex=0;ulindex<22;ulindex++)
				{
					UARTprintf("0x%02x,",temp2[ulindex]);
				}

    		UARTprintf("(0) i2c master error: %d\n",I2CMasterErr(I2C0_MASTER_BASE));
    		
    		s16_t UT=(s16_t)tempresult[0]<<8|tempresult[1];
    		s16_t UP=(s16_t)pressresult[0]<<8|pressresult[1];
    		s16_t AC1=(s16_t)temp2[0]<<8|temp2[1];
    		s16_t AC2=(s16_t)temp2[2]<<8|temp2[3];
    		s16_t AC3=(s16_t)temp2[4]<<8|temp2[5];
    		u16_t AC4=(s16_t)temp2[6]<<8|temp2[7];
    		u16_t AC5=(s16_t)temp2[8]<<8|temp2[9];
    		u16_t AC6=(s16_t)temp2[10]<<8|temp2[11];
    		s16_t B1=(s16_t)temp2[12]<<8|temp2[13];
    		s16_t B2=(s16_t)temp2[14]<<8|temp2[15];
    		s16_t MB=(s16_t)temp2[16]<<8|temp2[17];
    		s16_t MC=(s16_t)temp2[18]<<8|temp2[19];
    		s16_t MD=(s16_t)temp2[20]<<8|temp2[21];
    		
    		UARTprintf("hello1\r\n");
    		
    		//
    		s64_t X1=((UT-AC6)*AC5)>>15;
    		s64_t X2=((MC<<11)/(X1+MD));
    		s64_t B5=X1+X2;
    		s64_t TT=(B5+8)>>4;
    		
    		//long temperature=T*0.1;
    		
    		UARTprintf("0x%04x,%d\n",UT,TT);
    		
    		
    		//
    		s64_t P;
    		s64_t B6=B5-4000;
    		X1=(B2*((B6*B6)>>12))>>11;
    		X2=(AC2*B6)>>11;
    		s64_t X3=X1+X2;
    		s64_t B3=(((AC1*4+X3)<<OSRS)+2)/4;
    		X1=(AC3*B6)>>13;
    		X2=(B1*((B6*B6)>>12))>>16;
    		X3=(X1+X2+2)>>2;
    		u64_t B4=(AC4*(unsigned long)(X3+32768))>>15;
    		u64_t B7=((unsigned long)UP-B3)*(50000>>OSRS);
    		if(B7<0x80000000)
    			P=(B7*2)/B4;
    		else
    			P=(B7/B4)*2;
    		X1=(P>>8)*(P>>8);
    		X1=(X1*3038)>>16;
    		X2=(-7357*P)>>16;
    		s64_t PP=P+((X1+X2+3791)>>4);
    		UARTprintf("0x%04x,%d      %d,%d,%d\n",UP,PP,    B6,X1,X2);
    		_delay_ms(2000);
    }



    //
    // Tell the user that the test is done.
    //
    UARTprintf("\nDone.\n\n");
    
    
    //
    // Loop forever echoing data through the UART.
    //
    while(1)
    {
    		
        //UARTprintf("%d\n",ulClockMS);
    }
}

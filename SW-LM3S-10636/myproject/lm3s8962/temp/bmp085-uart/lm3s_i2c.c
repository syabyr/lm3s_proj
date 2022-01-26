/******************************************************************************/
/** \file lm3s_i2c.c
 *  \brief I2C Driver for LM3S. (SolderCore). Uses Stellarisware Rev 5450
 *  
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 *  \Todo Sort out error codes
 *  \author Iain Derrington (http://www.hotsolder.co.uk)
 *  \version: $Rev: 182 $

 */
/*******************************************************************************/
/***************Includes*******************************************************/

#include "inc/hw_types.h"
#include "lm3s_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"

#include "driverlib/i2c.h"
#include "driverlib/gpio.h"

/***************Private Macro**************************************************/


/***************Private Parts**************************************************/
/***************Private Variables**********************************************/
/***************Private Function Proto*****************************************/  
static i2c_error read_i2c(unsigned long ,u08_t , u08_t * , u08_t );
static i2c_error write_i2c(unsigned long ,u08_t , u08_t *, u08_t );

void init_i2c(T_i2cdrv *i2cdrv)
{
  unsigned long base;
  
  switch(i2cdrv->param.channel)
  {
    case 0:
      base = I2C0_MASTER_BASE;
      SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
      I2CMasterIntClear(base);
     
      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);      //for i2c.
      GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);

      i2cdrv->tx = write_i2c_0;
      i2cdrv->rx = read_i2c_0;
    break;
    case 1:
      base = I2C1_MASTER_BASE; 
      //GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
      //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  
      SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

      i2cdrv->tx = write_i2c_1;
      i2cdrv->rx = read_i2c_1;
    break;
    default:
    break;
  }
  
  I2CMasterInitExpClk(base,SysCtlClockGet(),i2cdrv->param.rate);
}

/***********************************************************************/
/** \brief Write write_i2c_0
 *
 *  Send I2C data out port 0
 *
 * \param address I2C address.
 * \param data Pointer to char buffer. Used to hold data
 * \param len Number of byte to write
 * \return i2c_error  0 is ok.           
 * 
 */
/**********************************************************************/
i2c_error write_i2c_0(u08_t address, u08_t * data, u08_t len)
{
  return write_i2c(I2C0_MASTER_BASE,address,data,len);
}

/***********************************************************************/
/** \brief read_i2c_0
 *
 * Read i2c data from port 0.
 * \param address I2C address.
 * \param data Pointer to char buffer.
 * \param len Number of bytes to read.
 * \return i2c_error  0 is ok.            
 */
/**********************************************************************/
i2c_error read_i2c_0(u08_t address, u08_t * data, u08_t len)
{
  return read_i2c(I2C0_MASTER_BASE,address,data,len);
}

/***********************************************************************/
/** \brief read_i2c_1
 *
 * Read i2c data from port 1.
 * \param address I2C address.
 * \param data Pointer to char buffer.
 * \param len Number of bytes to read.
 * \return i2c_error  0 is ok.            
 */
/**********************************************************************/
i2c_error read_i2c_1(u08_t address, u08_t * data , u08_t len)
{
  return read_i2c(I2C1_MASTER_BASE,address,data,len);
}

/***********************************************************************/
/** \brief Write write_i2c_0
 *
 *  Send I2C data out port 0
 *
 * \param address I2C address.
 * \param data Pointer to char buffer. Used to hold data
 * \param len Number of byte to write
 * \return i2c_error  0 is ok.           
 * 
 */
/**********************************************************************/
i2c_error write_i2c_1(u08_t address, u08_t * data , u08_t len)
{
  return write_i2c(I2C1_MASTER_BASE,address,data,len);
}

/***********************************************************************/
/** \brief 
 *
 * Description: 
 * \param
 * \return             
 * 
 *              
 * \author Iain Derrington
 */
/**********************************************************************/
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

/***********************************************************************/
/** \brief 
 *
 * Description: 
 * \param
 * \return             
 * 
 *              
 * \author Iain Derrington
 */
/**********************************************************************/
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

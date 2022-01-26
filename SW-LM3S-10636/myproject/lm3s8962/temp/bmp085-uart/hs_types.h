/******************************************************************************/
/** \file hs_types.h
 *  \brief Structure definintion for hardware peripheral driver.
 *  *
 *  \author Iain Derrington (www.hotsolder.co.uk)
 *  \version: $Rev: 182 $

 */
/*******************************************************************************/
#ifndef _HS_TYPES_H
#define _HS_TYPES_H

typedef unsigned char       u08_t;
typedef char                s08_t;
typedef unsigned short int  u16_t;
typedef short int           s16_t;
typedef unsigned int        u32_t;
typedef int                 s32_t;
typedef unsigned long       u64_t;
typedef long                s64_t;

#define true  1
#define false 0


/***********************************************************************/
/** 
 *  Generic parameter and structures for accesing an I2C port
 * 
 */
/**********************************************************************/
typedef u08_t i2c_error;

typedef enum
{
  SPD_100KHZ = 0,
  SPD_400KHZ = 1
}E_I2CRATE;

/*! Paremeters used to configure the I2C dricer*/
typedef struct
{
  u08_t channel;
  E_I2CRATE rate;
}T_i2cparam;

/* Driver for a i2c port */
typedef struct
{
  i2c_error (*tx)(u08_t,u08_t*,u08_t);  // function pointer to an i2c write driver
  i2c_error (*rx)(u08_t,u08_t*,u08_t);  // function pointer to an i2c read driver
  u08_t address;                        // device address
  T_i2cparam param;                     // parameters used to configure the port
}T_i2cdrv;
/**********************************************************************/


/***********************************************************************/
/** 
 *  Generic parameter and structures for accesing an SPI port
 * 
 */
/**********************************************************************/
typedef struct
{
  u08_t channel;    /* Select SPI channel*/
  u64_t rate;       /* SPI clock frequency (MHZ)*/
  u08_t phase;
  u08_t polarity;
  u08_t datawidth;  /*How many bits*/
}T_spiparam;
  
typedef struct 
{
  void(*fp_spi_com)(unsigned long *, unsigned long *, int , int);   /* SPI exchange function */
  void(*cs)(u08_t);                                                 /* CS funtion */
  T_spiparam param;                                                 /* SPI parameters */
 }T_spidrv;
/**********************************************************************/



/***********************************************************************/
/** 
 *  Generic parameter and structures for accesing a UART port
 *  WIP
 */
/**********************************************************************/
  typedef struct
  {
    void(*fp_uart_write)(void);
  }T_uart_drv;

  typedef struct
  {
    void *buffer;     
    unsigned int len;
    unsigned long head;
    unsigned long tail;
    unsigned int space;
  }T_CommsBuffer;
/**********************************************************************/

#endif

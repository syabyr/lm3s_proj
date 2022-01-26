/******************************************************************************/
/** \file bmp085_drv.c
 *  \brief Driver for the bosch bmp085 pressure sensor.
 *
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 *  \Todo Sort out error codes
 *  \author Iain Derrington (http://www.hotsolder.co.uk)
 *  \version: $Rev: 183 $
 */
/*******************************************************************************/


/***************Includes*******************************************************/
#include"bmp085_drv.h"

/***************Private Macro**************************************************/
#define BMP085_ADDR   0x77
#define CTLREG        0xf4
#define READ_TEMP     0x2e
#define READ_PRESSURE 0x34
#define MSB           0xf6
#define OSRS          3
#define DELAY         8000

/***************Private Parts**************************************************/
typedef struct
{
  s16_t AC1;
  s16_t AC2;
  s16_t AC3;
  u16_t AC4;
  u16_t AC5;
  u16_t AC6;
  s16_t B1;
  s16_t B2;
  s16_t MB;
  s16_t MC;
  s16_t MD;
}T_BMP085_Cal;
  

/***************Private Variables**********************************************/
T_i2cdrv prv_bmp_i2c_drv;            // i2c drvier
T_BMP085_Cal bmp085_cal;             // bmp cal data

/***************Private Function Proto*****************************************/  
void readshort(u08_t,s16_t *);
void read3byte(u08_t,u32_t);
void readBMP085CalData(void);
void read_bmp_temp(s16_t *);
void read_bmp_pressure(s32_t*);
/***********************************************************************/
/** \brief init_bmp085
 *
 * Sets up the i2c drivers. Reads back cal data for calculation later.
 * \param T_i2c_drv. Structure holding the funtions i2c_write and i2c_read * 
 *              
 */
/**********************************************************************/
void init_bmp085(T_i2cdrv bmpi_i2c_drv)
{
  // assign the i2c drivers.
  prv_bmp_i2c_drv.tx = bmpi_i2c_drv.tx;
  prv_bmp_i2c_drv.rx = bmpi_i2c_drv.rx;

  // read Calibration data used to compensate the data
  readBMP085CalData();
}

/***********************************************************************/
/** \brief read_bmp085
 *
 * Reads temperature and presssure values from sensor \n
 * compensates the values and populates structure.
 * \param bmp085. Pointer to a structure that will hold temp and pressure.           
 *              
 */
/**********************************************************************/
void read_bmp085(T_bmp085 * bmp085)
{
  s16_t temperature;
  s32_t pressure;
  s64_t X1, X2, X3, B3, B5, B6, P;
  u64_t B4, B7;
   
 
  // read uncompensated temperature value (working)
  read_bmp_temp(&temperature);
  read_bmp_temp(&temperature);
  // read uncompensated pressure value
  read_bmp_pressure(&pressure);

 
  //calculate true temperature (working)
  X1 = ((temperature - bmp085_cal.AC6) * bmp085_cal.AC5) >> 15;
  X2 = (bmp085_cal.MC << 11) / (X1 +  bmp085_cal.MD);
  B5 = X1 + X2;
  bmp085->temperature = (B5 + 8) >> 4;

    // Calculate true pressure.
  B6 = B5 - 4000;
  
  X1 = (bmp085_cal.B2 * ((B6 * B6) >> 12)) >> 11;
  
  X2 = (bmp085_cal.AC2 * B6) >> 11;
  
  X3 = X1 + X2;
  
  B3 = (( (bmp085_cal.AC1*4 + X3) << OSRS) + 2) / 4;
  
  X1 = (bmp085_cal.AC3 * B6) >> 13;

  X2 = (bmp085_cal.B1 * ((B6 * B6) >> 12)) >> 16;

  X3 = (X1 + X2 + 2)   >> 2;
  
  B4 = (bmp085_cal.AC4 * (unsigned long)(X3 + 32768)) >> 15;

  B7 = ((unsigned long)pressure - B3) * (50000 >> OSRS);


  if (B7 < 0x80000000)
    P = (B7 * 2) / B4;
  else
    P = (B7 / B4) * 2;


  X1 = (P >> 8) * (P >> 8);

  X1 = (X1 * 3038) >> 16;
  
  X2 = (-7357 * P) >> 16;
  bmp085->pressure = P + ((X1 + X2 + 3791) >> 4);
  
  
  
  
  
  
}

/***********************************************************************/
/** \brief readBMP085CalData
 *
 *  Reads the BMP085 calibration data used for temperature and \n 
 *  pressure compensation.        
 */
/**********************************************************************/
void readBMP085CalData(void)
{
  readshort(0xaa,&bmp085_cal.AC1);
  readshort(0xac,&bmp085_cal.AC2);
  readshort(0xae,&bmp085_cal.AC3);
  readshort(0xb0,&bmp085_cal.AC4);
  readshort(0xb2,&bmp085_cal.AC5);
  readshort(0xb4,&bmp085_cal.AC6);
  readshort(0xb6,&bmp085_cal.B1);
  readshort(0xb8,&bmp085_cal.B2);
  readshort(0xba,&bmp085_cal.MB);
  readshort(0xbc,&bmp085_cal.MC);
  readshort(0xbe,&bmp085_cal.MD);
}

/***********************************************************************/
/** \brief read_bmp_temp
 *
 *  Reads raw temperature back from bmp085.
 * \param *temperature Pointer to temperature variable    
 */
/**********************************************************************/
void read_bmp_temp(s16_t *temperature)
{
  u08_t idata[2];
  volatile u64_t d;

  //write command
  idata[0] = CTLREG;      //reg address
  idata[1] = READ_TEMP;   //data to go in reg

  prv_bmp_i2c_drv.tx(BMP085_ADDR, idata, 2);
  
  //wait 4.5ms
  for(d=0; d<DELAY;d++){}
  
  
  // read data back
  idata[0] = MSB;         // reg address
  prv_bmp_i2c_drv.tx(BMP085_ADDR, idata, 1);
  prv_bmp_i2c_drv.rx(BMP085_ADDR, idata, 2);

  // convert two bytes to single short.
  *temperature = ((s16_t)idata[0]) << 8;
  *temperature |= idata[1];
}

/***********************************************************************/
/** \brief read_bmp_pressure
 *
 * Reads the raw pressure value from the BMP085
 * \param *pressure
 */
/**********************************************************************/
void read_bmp_pressure(s32_t *pressure)
{
  u08_t idata[3];
  volatile u64_t d;

  //write command
  idata[0] = CTLREG;
  idata[1] = READ_PRESSURE + (OSRS << 6);
  prv_bmp_i2c_drv.tx(BMP085_ADDR, idata, 2);
  
  for(d=0; d<(DELAY*(OSRS+1));d++){}

  // read data back
  idata[0] = MSB;         // reg address
  prv_bmp_i2c_drv.tx(BMP085_ADDR, idata, 1);
  prv_bmp_i2c_drv.rx(BMP085_ADDR, idata, 3);

  // convert 3 bytes to single int.
  *pressure = (( ((s32_t)idata[0]) << 16) + (((s32_t)idata[1]) << 8) + ((s32_t)idata[2]) ) >> (8-OSRS) ;
  
}

/***********************************************************************/
/** \brief readshort
 *
 * Description: Reads a 16 bit value from the BMP085
 * \param address
 * \param *data
 */
/**********************************************************************/
void readshort(u08_t address,s16_t *data)
{
  u08_t idata[2];

  //read the MSB first
  prv_bmp_i2c_drv.tx(BMP085_ADDR, &address, 1);
  prv_bmp_i2c_drv.rx(BMP085_ADDR, idata, 2);
  
  // store the MSB
  *data|= ((s16_t)idata[0]) << 8;
  *data |= idata[1];
}







#ifndef _BMP085_DRV_H_
#define _BMP085_DRV_H_
/******************************************************************************/
/** \file bmp085_drv.h
 *  \brief Header file for the bmp085 driver code.
 *
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 *  \author Iain Derrington (http://www.hotsolder.co.uk)
 *  \version: $Rev: 173 $
 */
/*******************************************************************************/
/***************Includes*******************************************************/
#include "hs_types.h"
/***************Public Macro**************************************************/
/***************Public Parts**************************************************/
typedef struct
{
  long pressure;
  long temperature;
}T_bmp085;
/***************Public Variables**********************************************/
/***************Public Function Proto*****************************************/  


// initialises the device
void init_bmp085(T_i2cdrv);
//read temperature and pressure
void read_bmp085(T_bmp085 *);

#endif

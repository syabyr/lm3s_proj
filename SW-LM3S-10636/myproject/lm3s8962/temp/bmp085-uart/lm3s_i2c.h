#ifndef _LM3S_I2C_H_
#define _LM3S_I2C_H_
/******************************************************************************/
/** \file lm3s_i2c.h
 *  \brief header file for lm3s i2c code
 *
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 *  \author Iain Derrington (http://www.hotsolder.co.uk)
 *  \version: $Rev: 172 $
 */
/*******************************************************************************/
/***************Includes*******************************************************/
#include "hs_types.h"
/***************Public Macro**************************************************/
/***************Public Parts**************************************************/
/***************Public Variables**********************************************/
/***************Public Function Proto*****************************************/  
void init_i2c(T_i2cdrv *);

i2c_error read_i2c_0(u08_t, u08_t * , u08_t);
i2c_error write_i2c_0(u08_t, u08_t *, u08_t);

i2c_error read_i2c_1(u08_t, u08_t * , u08_t);
i2c_error write_i2c_1(u08_t, u08_t *, u08_t);

#endif
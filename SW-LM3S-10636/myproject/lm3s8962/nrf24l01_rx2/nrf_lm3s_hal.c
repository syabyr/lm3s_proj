#include "nrf_lm3s_hal.h"
#include "stdint.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "drivers/rit128x96x4.h"
#include "drivers/mydelay.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/lm3s8962.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "mydelay.h"
#include <inttypes.h>
#include "uartstdio.h"



void set_nrf24l01_ce_h()
{
	GPIO_PORTC_DATA_R|=(1<<7);
}

void set_nrf24l01_ce_l()
{
	GPIO_PORTC_DATA_R&=~(1<<7);
}

void set_nrf24l01_csn_h()
{
	GPIO_PORTC_DATA_R|=(1<<5);
}

void set_nrf24l01_csn_l()
{
	GPIO_PORTC_DATA_R&=~(1<<5);
}


//spi写/读一字节
uint8_t spi_send_byte(uint8_t data)
{
	uint32_t status;
	//while(SSIBusy(SSI0_BASE)){}
	SSIDataPut(SSI0_BASE,data);
	SSIDataGet(SSI0_BASE,&status);
	//while(SSIBusy(SSI0_BASE)){}
	return (uint8_t)status;
}



//spi写nrf24寄存器
uint8_t spi_write_reg(const uint8_t reg,const uint8_t value)
{
	uint8_t status;
	
	set_nrf24l01_csn_l();
	status=spi_send_byte(reg);
	spi_send_byte(value);
	set_nrf24l01_csn_h();

    return status;
}

//spi读nrf24寄存器
uint8_t spi_read_reg(uint8_t reg)
{
	uint8_t value=0;

	set_nrf24l01_csn_l();
	spi_send_byte(reg);
	value=spi_send_byte(0xff);
	set_nrf24l01_csn_h();

	return value;
}
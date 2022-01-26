#include "nRF24L01.h"


#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload
#define TX_PLOAD_WIDTH  32  	// 20 uints TX payload

#define pgm_read_word(p) (*(p))
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))



static const char rf24_datarate_e_str_0[] = "1MBPS";
static const char rf24_datarate_e_str_1[] = "2MBPS";
static const char rf24_datarate_e_str_2[] = "250KBPS";
static const char * const rf24_datarate_e_str_P[]  = {
  rf24_datarate_e_str_0,
  rf24_datarate_e_str_1,
  rf24_datarate_e_str_2,
};


static const char rf24_crclength_e_str_0[]  = "Disabled";
static const char rf24_crclength_e_str_1[]  = "8 bits";
static const char rf24_crclength_e_str_2[]  = "16 bits" ;
static const char * const rf24_crclength_e_str_P[]  = {
  rf24_crclength_e_str_0,
  rf24_crclength_e_str_1,
  rf24_crclength_e_str_2,
};

static const char rf24_pa_dbm_e_str_0[]  = "PA_MIN";
static const char rf24_pa_dbm_e_str_1[]  = "PA_LOW";
static const char rf24_pa_dbm_e_str_2[]  = "PA_HIGH";
static const char rf24_pa_dbm_e_str_3[]  = "PA_MAX";
static const char * const rf24_pa_dbm_e_str_P[]  = {
  rf24_pa_dbm_e_str_0,
  rf24_pa_dbm_e_str_1,
  rf24_pa_dbm_e_str_2,
  rf24_pa_dbm_e_str_3,
};

static const char rf24_model_e_str_0[]  = "nRF24L01";
static const char rf24_model_e_str_1[]  = "nRF24L01+";
static const char * const rf24_model_e_str_P[]  = {
  rf24_model_e_str_0,
  rf24_model_e_str_1,
};




bool p_variant=false;

const uint8_t child_pipe_enable[] =
{
  ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};

static const uint8_t child_pipe[] =
{
  RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};

const uint8_t child_payload_size[] =
{
  RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};

const unsigned char TX_ADDRESS[5] = { 'T', 'I', 'M', 'C', 'U' }; // 定义一个发送地址（发送数据的地址） ;
const unsigned char RX_ADDRESS[5] = { 'B', 'O', 'O', 'S', 'T' }; // 定义一个接收地址（接收应答信号的地址）;


typedef enum { RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR } rf24_pa_dbm_e ;



///////////////////////////////////////////////////////////////////////////////
uint8_t nrf_write_reg(uint8_t reg,uint8_t value)
{
	return spi_write_reg(WRITE_REG|(REGISTER_MASK & reg),value);
}

uint8_t nrf_read_reg(uint8_t reg)
{
	return spi_read_reg(( REGISTER_MASK & reg ));
}



void set_module_tx()
{
	nrf_write_reg(CONFIG, ( nrf_read_reg(CONFIG) ) & ~_BV(PRIM_RX) );
}




//uint8_t nrf_write_buf(uint8_t reg,const uint8_t * buf,uint8_t len)
uint8_t nrf_write_buf(uint8_t reg,const uint8_t buf[], const uint8_t len)
{
	uint8_t status;
	set_nrf24l01_csn_l();
	status=spi_send_byte(WRITE_REG | ( REGISTER_MASK & reg ));

	for(int i=0;i<len;i++)
	{
		status=spi_send_byte(*buf);
		buf++;
	}
	set_nrf24l01_csn_h();
	return status;
}

uint8_t nrf_read_buf(uint8_t reg,uint8_t buf[], uint8_t len)
{
	uint8_t status,i;
	set_nrf24l01_csn_l();
	status=spi_send_byte( REGISTER_MASK & reg );
	for(i=0;i<len;i++)
	{
		status=spi_send_byte(0xff);
		buf[i]=status;
	}
	set_nrf24l01_csn_h();
	return status;
}



uint8_t nrf_rx_packet(uint8_t *rxbuf)
{
	uint8_t state;
	uint8_t status,i;
	state=nrf_read_reg(STATUS);
	
	if(state&_BV(RX_DR))
	{
		nrf_write_reg(STATUS,state);
		//UARTprintf("status1:0x%02x\r\n",state);
		set_nrf24l01_csn_l();
		status=spi_send_byte(RD_RX_PLOAD);
		for(i=0;i<RX_PLOAD_WIDTH;i++)
		{
			status=spi_send_byte(0xff);
			//buf[i]=status;
		}
		set_nrf24l01_csn_h();
		//

		nrf_write_reg(FLUSH_RX,0xFF);
		return 0;
	}
	return 1;
}

uint8_t nrf_tx_packet(uint8_t *txbuf)
{
	uint8_t state,status;
	set_nrf24l01_ce_l();
	set_nrf24l01_csn_l();
	status=spi_send_byte(WR_TX_PLOAD);
	for(int i=0;i<TX_PLOAD_WIDTH;i++)
	{
		status=spi_send_byte(txbuf);
		txbuf++;
	}
	set_nrf24l01_csn_h();
	set_nrf24l01_ce_h();
	//等待IRQ信号,HOW? IRQ信号有无替代寄存器?
	//while();
	state=nrf_read_reg(STATUS);
	nrf_write_reg(STATUS,state);
	if(state&MAX_RT)
	{
		nrf_write_reg(FLUSH_TX,0xff);
		return MAX_RT;
	}
	if(state&TX_DS)
	{
		return TX_DS;
	}
	return 0xFF;
}


//清除TX FIFO寄存器.发射模式下用
uint8_t flush_tx(void)
{
	set_nrf24l01_csn_l();
	return spi_send_byte(FLUSH_TX);
	set_nrf24l01_csn_h();
}
//清除RX FIFO寄存器.接收模式下用
uint8_t flush_rx(void)
{
	set_nrf24l01_csn_l();
	return spi_send_byte(FLUSH_RX);
	set_nrf24l01_csn_h();
}




void setChannel(uint8_t channel)
{
	nrf_write_reg(RF_CH,channel);
}
uint8_t getChannel()
{
	return spi_read_reg(RF_CH);
}

void setPALevel(uint8_t level)
{
	uint8_t setup=nrf_read_reg(RF_SETUP) & 0b11111000;
	if(level >3)
	{
		level = (RF24_PA_MAX << 1) + 1;
	}
	else
	{
		level = (level << 1) + 1;
	}
	nrf_write_reg(RF_SETUP, setup |= level);
}

bool setDataRate(hal_nrf_datarate_t speed)
{
	bool result=false;
	uint8_t setup=nrf_read_reg(RF_SETUP);

	setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;
	if( speed == HAL_NRF_250KBPS )
	{
		setup |= _BV( RF_DR_LOW ) ;
	}
	else
	{
		if(speed == HAL_NRF_2MBPS)
		{
			setup |= _BV(RF_DR_HIGH);
		}
	}
	nrf_write_reg(RF_SETUP,setup);
	if(nrf_read_reg(RF_SETUP) == setup)
	{
		result=true;
	}
	return result;
}

void setAutoAckAll(bool enable)
{
	if(enable)
	{
		nrf_write_reg(EN_AA,0b111111);
	}
	else
	{
		nrf_write_reg(EN_AA,0);
	}
}
void setAutoAck(uint8_t pipe,bool enable)
{
	if(pipe<=6)
	{
		uint8_t en_aa=nrf_read_reg(EN_AA);
		if(enable)
		{
			en_aa|=_BV(pipe);
		}
		else
		{
			en_aa &=~_BV(pipe);
		}
		nrf_write_reg(EN_AA,en_aa);
	}
}

void setRetries(uint8_t delay, uint8_t count)
{
	nrf_write_reg(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}

void setCRCLength(hal_nrf_crc_mode_t length)
{
	uint8_t config = nrf_read_reg(CONFIG) & ~( _BV(CRCO) | _BV(EN_CRC)) ;

	// switch uses RAM (evil!)
	if ( length == HAL_NRF_CRC_OFF )
	{
 		// Do nothing, we turned it off above.
	}
	else if ( length == HAL_NRF_CRC_8BIT )
	{
		config |= _BV(EN_CRC);
	}
	else
	{
		config |= _BV(EN_CRC);
		config |= _BV( CRCO );
	}
	nrf_write_reg( CONFIG, config ) ;
}


void init_NRF24L01(void)
{
	if( setDataRate( HAL_NRF_250KBPS ) )
    {
        p_variant = true ;
    }

	set_nrf24l01_ce_l();
	set_nrf24l01_csn_h();
	setChannel(74);
	setPALevel(RF24_PA_MAX);
	setDataRate(HAL_NRF_2MBPS);
	setAutoAckAll(1);
	setRetries(2,15);
	setCRCLength(HAL_NRF_CRC_8BIT);
	
	set_nrf24l01_ce_h();
	_delay_us(200);

}


void testprint_address_register(const char* name, uint8_t reg, uint8_t qty)
{
	UARTprintf("%s\t =",name);
	UARTprintf("test1:%d\r\n",qty);

	while(qty--)
	{
		UARTprintf("test:%d\r\n",qty);
		uint8_t buffer[5]={0};
		nrf_read_buf(reg++,&buffer[0],sizeof buffer);
		UARTprintf(" 0x");
		unsigned char* bufptr = buffer + sizeof buffer;
		while( --bufptr >= buffer )
			UARTprintf("%02x",*bufptr);
	}
	UARTprintf("\r\nend......\r\n");
}

void openWritingPipe(uint64_t value)
{
	nrf_write_buf(RX_ADDR_P0, (uint8_t*)(&value), 5);
	nrf_write_buf(TX_ADDR, (uint8_t*)(&value), 5);
	nrf_write_reg(RX_PW_P0,32);
}

void openReadingPipe(uint8_t child, uint64_t address)
{
	if(child == 0)
	{

	}
	if(child <=6)
	{
		if(child <2)
		{
			nrf_write_buf(pgm_read_byte(&child_pipe[child]), (const uint8_t*)(&address), 5);
		}
		else
		{
			nrf_write_buf(pgm_read_byte(&child_pipe[child]), (const uint8_t*)(&address), 1);
		}
		//
		nrf_write_reg(pgm_read_byte(&child_payload_size[child]),5);
		//
		nrf_write_reg(EN_RXADDR,nrf_read_reg(EN_RXADDR) | _BV(pgm_read_byte(&child_pipe_enable[child])));
	}
}

hal_nrf_crc_mode_t getCRCLength(void)
{
  hal_nrf_crc_mode_t result = HAL_NRF_CRC_OFF;
  
  uint8_t config = nrf_read_reg(CONFIG) & ( _BV(CRCO) | _BV(EN_CRC)) ;
  uint8_t AA = nrf_read_reg(EN_AA);
  
  if ( config & _BV(EN_CRC ) || AA)
  {
    if ( config & _BV(CRCO) )
      result = HAL_NRF_CRC_16BIT;
    else
      result = HAL_NRF_CRC_8BIT;
  }

  return result;
}



uint8_t getPALevel(void)
{

  return (nrf_read_reg(RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH))) >> 1 ;
}

bool isPVariant(void)
{
  return p_variant ;
}


hal_nrf_datarate_t getDataRate( void )
{
  hal_nrf_datarate_t result ;
  uint8_t dr = nrf_read_reg(RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));

  // switch uses RAM (evil!)
  // Order matters in our case below
  if ( dr == _BV(RF_DR_LOW) )
  {
    // '10' = 250KBPS
    result = HAL_NRF_250KBPS ;
  }
  else if ( dr == _BV(RF_DR_HIGH) )
  {
    // '01' = 2MBPS
    result = HAL_NRF_2MBPS ;
  }
  else
  {
    // '00' = 1MBPS
    result = HAL_NRF_1MBPS ;
  }
  return result ;
}

bool available(uint8_t* pipe_num)
{
  if (!( nrf_read_reg(FIFO_STATUS) & _BV(RX_EMPTY) )){

    // If the caller wants the pipe number, include that
    if ( pipe_num ){
      uint8_t status = get_status();
      *pipe_num = ( status >> RX_P_NO ) & 0b111;
    }
    return 1;
  }
  return 0;
}



uint8_t get_status(void)
{
	set_nrf24l01_csn_l();
	return spi_send_byte(NOP);
	set_nrf24l01_csn_h();
}

void printDetails(void)
{
	print_status(get_status());
    print_byte_register("SETUP_AW\t",SETUP_AW,1);
    print_address_register("RX_ADDR_P0-1",RX_ADDR_P0,2);
    print_byte_register("RX_ADDR_P2-5",RX_ADDR_P2,4);
    print_address_register("TX_ADDR\t",TX_ADDR,1);
    print_byte_register("RX_PW_P0-6",RX_PW_P0,6);
    print_byte_register("EN_AA\t",EN_AA,1);
    print_byte_register("EN_RXADDR",EN_RXADDR,1);
    print_byte_register("RF_CH\t",RF_CH,1);
    print_byte_register("RF_SETUP",RF_SETUP,1);
    print_byte_register("CONFIG\t",CONFIG,1);
    print_byte_register("DYNPD/FEATURE",DYNPD,2);
    UARTprintf("Data Rate\t = %s\r\n", pgm_read_word(&rf24_datarate_e_str_P[getDataRate()]));
    UARTprintf("Model\t\t = %s\r\n",   pgm_read_word(&rf24_model_e_str_P[isPVariant()]));
    UARTprintf("CRC Length\t = %s\r\n",pgm_read_word(&rf24_crclength_e_str_P[getCRCLength()]));
    UARTprintf("PA Power\t = %s\r\n",  pgm_read_word(&rf24_pa_dbm_e_str_P[getPALevel()]));
}



void print_status(uint8_t status)
{
  UARTprintf("STATUS\t\t = 0x%02x RX_DR=%x TX_DS=%x MAX_RT=%x RX_P_NO=%x TX_FULL=%x\r\n",
           status,
           (status & _BV(RX_DR))?1:0,
           (status & _BV(TX_DS))?1:0,
           (status & _BV(MAX_RT))?1:0,
           ((status >> RX_P_NO) & 0b111),
           (status & _BV(TX_FULL))?1:0
          );
}


void print_address_register(const char* name, uint8_t reg, uint8_t qty)
{
	UARTprintf("%s\t =",name);
	while(qty--)
	{
		uint8_t buffer[5]={0};
		nrf_read_buf(reg++,&buffer[0],5);
		UARTprintf(" 0x");
		unsigned char* bufptr = buffer + sizeof buffer;
		while( --bufptr >= buffer )
			UARTprintf("%02x",*bufptr);
	}
	UARTprintf("\r\n");
}

void print_byte_register(const char* name, uint8_t reg, uint8_t qty)
{
	UARTprintf("%s\t =",name);
	while(qty--)
	{
		UARTprintf(" 0x%02x",nrf_read_reg(reg++));
	}
	UARTprintf("\r\n");
}

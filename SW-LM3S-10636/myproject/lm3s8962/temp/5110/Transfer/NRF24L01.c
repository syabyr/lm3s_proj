#include "include.h"

const unsigned char TX_ADDRESS[5] = { 'T', 'I', 'M', 'C', 'U' }; // 定义一个发送地址（发送数据的地址） ;
const unsigned char RX_ADDRESS[5] = { 'B', 'O', 'O', 'S', 'T' }; // 定义一个接收地址（接收应答信号的地址）;



/*
 * 函数：USCI_B0_Init
 * 功能：初始化SPI总线
 * 参数：无
 * 返回：无
 */

void USCI_B0_Init()
{
    UCB0CTL1 |= UCSWRST;
    UCB0CTL1 = UCSSEL_2 + UCSWRST ;							//复位USCI_B0，时钟源设置为SMCLK
    UCB0CTL0 = UCCKPH +UCMSB + UCMST + UCMODE_0 + UCSYNC; 	//MSB优先 上升沿捕获，下降沿改变数值，设置成SPI主机模式
    UCB0CTL1 &= ~UCSWRST;									//使能SPI
    UCB0BR0 = 2;
    UCB0BR1 = 0;											//16M的时钟，2分频即可实现8M的SPI速率
	IFG2 &= ~(UCB0TXIFG);									//清除中断标志
    IFG2 &= ~(UCB0RXIFG);									//清除中断标志
//	IE2 |= UCB0TXIE;										//开启SPI中断，
//由于SPI速率达到8M，8位的SPI数据发送完毕只需要1-2个us，如果进入低功耗模式，那么中断将极其频繁，因此SPI部分不使用中断，而是采取查询法实现。
//  IE2 |= UCB0RXIE;										//开启SPI中断
}

/*
 * 函数：SPI_Read_Reg
 * 功能：通过SPI总线读取一个字节的数据
 * 参数：reg,无符号字符型,读取的寄存器地址
 * 返回：value,无符号字符型,寄存器的值
 */
unsigned char SPI_Read_Reg(unsigned char reg) 
{
	unsigned char value=0;
	SET_NRF24L01_CSN_L;
	UCB0TXBUF=reg;
	IFG2&=~UCB0RXIFG;
	while(!(IFG2 & UCB0RXIFG));
	UCB0TXBUF=NOP;
	IFG2&=~UCB0RXIFG;
	while(!(IFG2 & UCB0RXIFG));
	SET_NRF24L01_CSN_H;
	value=UCB0RXBUF;
	return value;
}

/*
 * 函数：SPI_Write_Reg
 * 功能：通过SPI总线写入一个字节的数据
 * 参数：reg,无符号字符型,写入的寄存器地址
 * 参数：value,无符号字符型,写入寄存器的值
 * 返回：无
 */
void SPI_Write_Reg(unsigned char reg, unsigned char value)
{
	SET_NRF24L01_CSN_L;
	UCB0TXBUF=reg;
	//	while(!(IFG2 & UCB0TXIFG));
	IFG2&=~UCB0RXIFG;
	while(!(IFG2 & UCB0RXIFG));
	UCB0TXBUF=value;
	IFG2&=~UCB0RXIFG;
	while(!(IFG2 & UCB0RXIFG));
	SET_NRF24L01_CSN_H;
}

/*
 * 函数：SPI_Read_Reg
 * 功能：通过SPI总线读取单一地址的多个字节数据
 * 参数：reg,无符号字符型,需要读取的寄存器地址
 * 参数：pBuf,无符号字符型指针,一般填写数组名。将读取的寄存器数值写入这个指针指向的数组中
 * 参数：bytes,无符号字符型,需要读取的字节数
 * 返回：无
 */
void SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	SET_NRF24L01_CSN_L;
	UCB0TXBUF=reg;
	IFG2&=~UCB0RXIFG;
	while(!(IFG2 & UCB0RXIFG));
	for(;bytes>0;bytes--)
	{
		UCB0TXBUF=NOP;
		IFG2&=~UCB0RXIFG;
		while(!(IFG2 & UCB0RXIFG));
		*pBuf=UCB0RXBUF;
		pBuf++;
	}
	SET_NRF24L01_CSN_H;
}

/*
 * 函数：SPI_Write_Buf
 * 功能：通过SPI总线写入单一地址的多个字节数据
 * 参数：reg,无符号字符型,需要写入的寄存器地址
 * 参数：pBuf,无符号字符型指针,将需要写入的寄存器值填入这个指针之后的存储空间中。
 * 参数：bytes,无符号字符型,需要写入的字节数
 * 返回：无
 */
void SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	SET_NRF24L01_CSN_L;
	UCB0TXBUF=reg;
	IFG2&=~UCB0RXIFG;
	for(;bytes>0;bytes--)
	{
		while(!(IFG2 & UCB0RXIFG));
		UCB0TXBUF=*pBuf;
		IFG2&=~UCB0RXIFG;
		pBuf++;
	}
	while(!(IFG2 & UCB0RXIFG));
	SET_NRF24L01_CSN_H;
}

/*
 * 函数：Set_Module_TX
 * 功能：通过SPI总线写入寄存器数据，将NRF24L01模块配置成为发送模式
 * 返回：无
 */
void Set_Module_TX()
{
SET_NRF24L01_CE_L;
SPI_Write_Reg(WRITE_REG+CONFIG,0x5e);				//配置发送模式
SPI_Write_Reg(WRITE_REG+EN_AA,0x00);				//关闭自动重发
SPI_Write_Reg(WRITE_REG+EN_RXADDR,0x00);			//关闭接收
SPI_Write_Reg(WRITE_REG+RF_CH,88);					//设置频率88
SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,5);
SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,TX_ADDRESS,5);
SPI_Write_Buf(WRITE_REG+RX_ADDR_P1,RX_ADDRESS,5);
}

/*
 * 函数：Set_Module_RX
 * 功能：通过SPI总线写入寄存器数据，将NRF24L01模块配置成为接收模式
 * 返回：无
 */
void Set_Module_RX()
{
SET_NRF24L01_CE_L;
SPI_Write_Reg(WRITE_REG+CONFIG,0);
Delay_10us(10);
SPI_Write_Reg(WRITE_REG+CONFIG,0x3f);				//配置接收模式
SPI_Write_Reg(WRITE_REG+EN_AA,0x00);				//关闭自动重发
SPI_Write_Reg(WRITE_REG+EN_RXADDR,0x02);			//通道1接收
SPI_Write_Reg(WRITE_REG+RF_CH,88);					//设置频率88
SPI_Write_Reg(WRITE_REG+RX_PW_P1,17);				//设置数据宽度17
SPI_Write_Buf(WRITE_REG+TX_ADDR,RX_ADDRESS,5);
SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,5);
SPI_Write_Buf(WRITE_REG+RX_ADDR_P1,TX_ADDRESS,5);
SPI_Write_Reg(WRITE_REG+STATUS,0x70);
SPI_Write_Reg(FLUSH_RX,NOP);
SET_NRF24L01_CE_H;
}

/*
 * 函数：Transfer_String
 * 功能：发送一个无符号字符型的字符串
 * 参数：string，无符号字符型指针，字符串的首字节地址
 * 参数：length，无符号字符型，字符串长度
 * 返回：无
 */
void Transfer_String(unsigned char *string, unsigned char length)
{
	SET_NRF24L01_CE_L;
	SPI_Write_Buf(WR_TX_PLOAD,string,length);
	SPI_Write_Reg(WRITE_REG+STATUS,0x70);
	SET_NRF24L01_CE_H;
	Delay_10us(1);
	Delay_10us(1);
	SET_NRF24L01_CE_L;
}

/*
 * 函数：Get_String
 * 功能：在接收模式下，接收一个无符号字符型的字符串
 * 参数：string，无符号字符型指针，字符串的首字节地址
 * 参数：length，无符号字符型，字符串长度
 * 返回：无
 */
void Get_String(unsigned char *string, unsigned char length)
{
	SET_NRF24L01_CE_L;
	Delay_10us(1);
	SPI_Read_Buf(RD_RX_PLOAD,string,length);
	Delay_10us(1);
	SET_NRF24L01_CE_H;
}




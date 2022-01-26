#include "include.h"

const unsigned char TX_ADDRESS[5] = { 'T', 'I', 'M', 'C', 'U' }; // ����һ�����͵�ַ���������ݵĵ�ַ�� ;
const unsigned char RX_ADDRESS[5] = { 'B', 'O', 'O', 'S', 'T' }; // ����һ�����յ�ַ������Ӧ���źŵĵ�ַ��;



/*
 * ������USCI_B0_Init
 * ���ܣ���ʼ��SPI����
 * ��������
 * ���أ���
 */

void USCI_B0_Init()
{
    UCB0CTL1 |= UCSWRST;
    UCB0CTL1 = UCSSEL_2 + UCSWRST ;							//��λUSCI_B0��ʱ��Դ����ΪSMCLK
    UCB0CTL0 = UCCKPH +UCMSB + UCMST + UCMODE_0 + UCSYNC; 	//MSB���� �����ز����½��ظı���ֵ�����ó�SPI����ģʽ
    UCB0CTL1 &= ~UCSWRST;									//ʹ��SPI
    UCB0BR0 = 2;
    UCB0BR1 = 0;											//16M��ʱ�ӣ�2��Ƶ����ʵ��8M��SPI����
	IFG2 &= ~(UCB0TXIFG);									//����жϱ�־
    IFG2 &= ~(UCB0RXIFG);									//����жϱ�־
//	IE2 |= UCB0TXIE;										//����SPI�жϣ�
//����SPI���ʴﵽ8M��8λ��SPI���ݷ������ֻ��Ҫ1-2��us���������͹���ģʽ����ô�жϽ�����Ƶ�������SPI���ֲ�ʹ���жϣ����ǲ�ȡ��ѯ��ʵ�֡�
//  IE2 |= UCB0RXIE;										//����SPI�ж�
}

/*
 * ������SPI_Read_Reg
 * ���ܣ�ͨ��SPI���߶�ȡһ���ֽڵ�����
 * ������reg,�޷����ַ���,��ȡ�ļĴ�����ַ
 * ���أ�value,�޷����ַ���,�Ĵ�����ֵ
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
 * ������SPI_Write_Reg
 * ���ܣ�ͨ��SPI����д��һ���ֽڵ�����
 * ������reg,�޷����ַ���,д��ļĴ�����ַ
 * ������value,�޷����ַ���,д��Ĵ�����ֵ
 * ���أ���
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
 * ������SPI_Read_Reg
 * ���ܣ�ͨ��SPI���߶�ȡ��һ��ַ�Ķ���ֽ�����
 * ������reg,�޷����ַ���,��Ҫ��ȡ�ļĴ�����ַ
 * ������pBuf,�޷����ַ���ָ��,һ����д������������ȡ�ļĴ�����ֵд�����ָ��ָ���������
 * ������bytes,�޷����ַ���,��Ҫ��ȡ���ֽ���
 * ���أ���
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
 * ������SPI_Write_Buf
 * ���ܣ�ͨ��SPI����д�뵥һ��ַ�Ķ���ֽ�����
 * ������reg,�޷����ַ���,��Ҫд��ļĴ�����ַ
 * ������pBuf,�޷����ַ���ָ��,����Ҫд��ļĴ���ֵ�������ָ��֮��Ĵ洢�ռ��С�
 * ������bytes,�޷����ַ���,��Ҫд����ֽ���
 * ���أ���
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
 * ������Set_Module_TX
 * ���ܣ�ͨ��SPI����д��Ĵ������ݣ���NRF24L01ģ�����ó�Ϊ����ģʽ
 * ���أ���
 */
void Set_Module_TX()
{
SET_NRF24L01_CE_L;
SPI_Write_Reg(WRITE_REG+CONFIG,0x5e);				//���÷���ģʽ
SPI_Write_Reg(WRITE_REG+EN_AA,0x00);				//�ر��Զ��ط�
SPI_Write_Reg(WRITE_REG+EN_RXADDR,0x00);			//�رս���
SPI_Write_Reg(WRITE_REG+RF_CH,88);					//����Ƶ��88
SPI_Write_Buf(WRITE_REG+TX_ADDR,TX_ADDRESS,5);
SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,TX_ADDRESS,5);
SPI_Write_Buf(WRITE_REG+RX_ADDR_P1,RX_ADDRESS,5);
}

/*
 * ������Set_Module_RX
 * ���ܣ�ͨ��SPI����д��Ĵ������ݣ���NRF24L01ģ�����ó�Ϊ����ģʽ
 * ���أ���
 */
void Set_Module_RX()
{
SET_NRF24L01_CE_L;
SPI_Write_Reg(WRITE_REG+CONFIG,0);
Delay_10us(10);
SPI_Write_Reg(WRITE_REG+CONFIG,0x3f);				//���ý���ģʽ
SPI_Write_Reg(WRITE_REG+EN_AA,0x00);				//�ر��Զ��ط�
SPI_Write_Reg(WRITE_REG+EN_RXADDR,0x02);			//ͨ��1����
SPI_Write_Reg(WRITE_REG+RF_CH,88);					//����Ƶ��88
SPI_Write_Reg(WRITE_REG+RX_PW_P1,17);				//�������ݿ��17
SPI_Write_Buf(WRITE_REG+TX_ADDR,RX_ADDRESS,5);
SPI_Write_Buf(WRITE_REG+RX_ADDR_P0,RX_ADDRESS,5);
SPI_Write_Buf(WRITE_REG+RX_ADDR_P1,TX_ADDRESS,5);
SPI_Write_Reg(WRITE_REG+STATUS,0x70);
SPI_Write_Reg(FLUSH_RX,NOP);
SET_NRF24L01_CE_H;
}

/*
 * ������Transfer_String
 * ���ܣ�����һ���޷����ַ��͵��ַ���
 * ������string���޷����ַ���ָ�룬�ַ��������ֽڵ�ַ
 * ������length���޷����ַ��ͣ��ַ�������
 * ���أ���
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
 * ������Get_String
 * ���ܣ��ڽ���ģʽ�£�����һ���޷����ַ��͵��ַ���
 * ������string���޷����ַ���ָ�룬�ַ��������ֽڵ�ַ
 * ������length���޷����ַ��ͣ��ַ�������
 * ���أ���
 */
void Get_String(unsigned char *string, unsigned char length)
{
	SET_NRF24L01_CE_L;
	Delay_10us(1);
	SPI_Read_Buf(RD_RX_PLOAD,string,length);
	Delay_10us(1);
	SET_NRF24L01_CE_H;
}




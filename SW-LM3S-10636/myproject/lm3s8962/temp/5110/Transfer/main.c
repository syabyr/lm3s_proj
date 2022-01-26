#include "include.h"
int value=0;

/*
 * ������io_initial
 * ���ܣ���ʼ��IO
 * ��������
 * ���أ���
 */
void io_initial(void)
{
	/*
	 * P1.0��LED
	 * P1.1��IRQ
	 * P1.4��CSN
	 * P1.5��SCLK
	 * P1.6��MISO
	 * P1.7��MOSI
	 * P2.0��CE
	 */
	P1DIR	=	0xef;
	P2DIR	=	0xff;
	P3DIR	=	0xff;
	P1OUT	=	0xfe;
	P2OUT	=	0xff;
	P3OUT	=	0xff;

	P1DIR	|=	BIT0+BIT4+BIT5+BIT7;
	P1DIR	&=	~(BIT1+BIT3+BIT6);
	P2DIR	|=	BIT0;
	P1REN	|=	BIT1+BIT3;
	P1OUT	|=	BIT1+BIT3;
	P1IE	|=	BIT1+BIT3;
	P1IES	|=	BIT1+BIT3;
	P1IFG	&=	~(BIT1+BIT3);
	P1SEL2 = BIT5 | BIT6 | BIT7;
	P1SEL = BIT5 | BIT6 | BIT7;
}

/*
 * ������board_initial
 * ���ܣ���ʼ��ʱ�ӡ�IO��SPI���ߵ�
 * ��������
 * ���أ���
 */
void board_initial(void)									//
{

	BCSCTL2 = SELM_0 | DIVM_0 | DIVS_0;
	if (CALBC1_16MHZ != 0xFF)
	{
		DCOCTL = 0x00;
		BCSCTL1 = CALBC1_16MHZ;
		DCOCTL = CALDCO_16MHZ;
	}
	BCSCTL1 |= XT2OFF | DIVA_3;								// ACLKʱ�ӣ�32768/8=4096Hz
	BCSCTL3 = XT2S_0 | LFXT1S_0 | XCAP_1;

	io_initial();											//����IO
	USCI_B0_Init();											//��ʼ��Ӳ��SPI��ʹ��UCSIB0
}

int main( void )
{
	unsigned char transfer_value[]="EEWORLD_MSP430_00";//����������������

	WDTCTL = WDTPW + WDTHOLD;
	board_initial();
	_EINT();

	Set_Module_TX();//��ģ������Ϊ����ģʽ
	TA1_Init();//��ʼ����ʱ��A1��ÿ0.5�뻽��MCU��������


	for(;;)
	{
		transfer_value[16]=value&0x00ff;			//��value�ĵ�8λ��������
		transfer_value[15]=((value&0xff00)>>8);		//��value�ĸ�8λ��������
		Transfer_String(transfer_value, 17);		//�������飬���ͳ���Ϊ17�ֽ�
		value++;
		LPM3;										//����LPM3�����ⲿ�жϻ��߶�ʱ���жϻ��ѡ�
	}
}


/*
 * �ⲿ�жϣ��ɰ������ⲿIRQ�жϴ����������ж�����
 * P1.3�Ĺ��ܣ����°�����������������1000
 * P1.1�Ĺ��ܣ�IRQ�жϣ�˵�����ݷ������
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
	if(P1IFG & BIT3)		//�����ж�
	{
		value+=1000;		//���°�����������������1000
		P1IFG &= ~BIT3;		//���жϱ�־λ
		LPM3_EXIT;			//�˳��͹���ģʽ������ѭ���з�����һ֡����
	}
	if(P1IFG & BIT1)		//IRQ�жϣ�˵��һ֡�����Ѿ��������
	{
		unsigned char tmp=SPI_Read_Reg(READ_REG+STATUS);	//��ȡ״̬�Ĵ���

		P1IFG &= ~BIT1; 									//����ⲿ�жϡ�
															//�Ȼ������Ҫ�õ��ж�Ƕ�ף������ж�ʹ��
															//��������жϣ���ô�ж�ʹ��֮��ֱ���ٴν����жϷ���

		tmp&=0x70;											//���IRQ�ж�Դ
		SPI_Write_Reg(WRITE_REG+STATUS,tmp);				//���ģ��IRQ�ж�
		if(tmp&0x20)										//���IRQ�ж��Ƿ����ж�
		{

			_EINT();										//�ж�ʹ��
			P1OUT |= BIT0;									//����LED
			Delay_10us(100);								//���ö�ʱ��A0��ʱ1ms
			P1OUT &= ~BIT0;									//Ϩ��LED
		}

		tmp=SPI_Read_Reg(READ_REG+FIFO_STATUS);             //��ȡFIFO�Ĵ���״̬
	    if(!(tmp&0x10))                                     //�������FIFO�ǿգ�˵����������δ��������Ҫ��CE�ܽŲ���һ�������巢����һ֡����
	    {
	    _EINT();                                            //�ж�ʹ��
		SET_NRF24L01_CE_H;
	    Delay_10us(1);
	    Delay_10us(1);
	    SET_NRF24L01_CE_L;
	    }
		P1IFG &= ~BIT1;										//����ⲿ�жϱ�־λ
	}
}



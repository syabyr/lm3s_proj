#include "include.h"
unsigned int Receive_Number;
unsigned char LCD_Leval=0xc0;

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
	P1DIR	=	BIT0+BIT4+BIT5+BIT7;
//	P1DIR	&=	~(BIT1+BIT3+BIT6);
	P1REN	|=	BIT1+BIT3;
	P1OUT	|=	BIT1+BIT3+BIT4;
	P1IE	|=	BIT1+BIT3;
	P1IES	|=	BIT1+BIT3;
	P1IFG	&=	~(BIT1+BIT3);
	P1SEL	=	BIT5 | BIT6 | BIT7;
	P1SEL2	=	BIT5 | BIT6 | BIT7;

	P2DIR	=	BIT0+BIT1+BIT2+BIT3+BIT4+BIT5;
	P2OUT	=	0xff;

	P3DIR	=	0xff;
	P3OUT	=	0xff;
}

/*
 * ������board_initial
 * ���ܣ���ʼ��ʱ�ӡ�IO��LCD��SPI���ߵ�
 * ��������
 * ���أ���
 */
void board_initial(void)
{
	if (CALBC1_16MHZ != 0xFF)
	{
		DCOCTL = 0x00;
		BCSCTL1 = CALBC1_16MHZ;
		DCOCTL = CALDCO_16MHZ;
	}
	BCSCTL3 = LFXT1S_2;		//VLO��ΪACLKʱ��Դ

	io_initial();           //����IO
	USCI_B0_Init();			//��ʼ��Ӳ��SPI��ʹ��UCSIB0
}

int main( void )
{
	WDTCTL = WDTPW + WDTHOLD;	//�ع�
	board_initial();			//��ʼ������
	_EINT();					//���ж�
	Set_Module_RX();			//��NRF24L01���óɽ���ģʽ
    lcd_init();                 //��ʼ��LCD

	while(1)
	{
		unsigned char transfer_value[]="EEWORLD_MSP430_00";	//����У������
		unsigned char message[20];							//������ռĴ�������
		unsigned char i;									//����һ����������i
		LPM3;                                               //����͹���ģʽ����IRQ�жϻ���

/*ԭ��ֻʹ������������䣬���ַ��ͽ��մ���һ���ӳ٣�Ҳ����MCU����ģ�������ĳЩ����²��ܼ�ʱ������ն�MCU�����Ǵ���һ���ӳ٣����ǽ���ĳ��·��Ĵ��룬ȷ������FIFO�����ݱ����*/
//		Get_String(message, 17);							//����һ֡17�ֽڵ�����

/*��ȡFIFO�Ĵ�����״̬�����䲻Ϊ�յ�ʱ�������ȡFIFO�е�����*/
		i=SPI_Read_Reg(READ_REG+FIFO_STATUS);               //����i����ȡFIFO�Ĵ���״̬
        while(!(i&0x01))
        {
            Get_String(message, 17);
            i=SPI_Read_Reg(READ_REG+FIFO_STATUS);               //����i����ȡFIFO�Ĵ���״̬
        }
/*����������Ϊ�����������ӳٵ�bug�����ģ���Ȼ����ʵ��֤�����ӳ��Ƿ��Ͷ���ɵģ����ڽ���ʹ��ģ��ĽǶȳ��������ɱ�����������*/


        //У����յ�������
		for(i=0;i<15;i++)									//��forѭ����У�飬������յ���������һ���ַ���У�����鲻һ�£���ô����forѭ������ʱi��С��15
		{
			if(message[i]!=transfer_value[i])break;
		}
		if(i<15)continue;									//���i<15��˵��У��û��ͨ�����������ݴ�����ֵ����ʾ��
		Receive_Number=(message[15]<<8)+message[16];		//����ֵ������messageת�Ƶ�Receive_Number

		//��ʾ���յ�������
		LCD_Disp_BNum(2,1,Receive_Number/10000%10);
		LCD_Disp_BNum(18,1,Receive_Number/1000%10);
		LCD_Disp_BNum(34,1,Receive_Number/100%10);
		LCD_Disp_BNum(50,1,Receive_Number/10%10);
		LCD_Disp_BNum(66,1,Receive_Number%10);
	}
}
 
/*
 * �ⲿ�жϣ��ɰ������ⲿIRQ�жϴ����������ж�����
 * P1.3�Ĺ��ܣ����°�����LCD����(VOP��ѹֵ)����1��VOP��ѹֵ��160��230ѭ��
 * P1.1�Ĺ��ܣ�IRQ�жϣ�˵�����յ���һ֡����
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
	if(P1IFG & BIT3)
	{
		P1IFG &= ~BIT3;
		LCD_Leval+=1;
		if(LCD_Leval>250)LCD_Leval=160;
		lcd_init();
		P1IFG &= ~BIT3;
	}
	if(P1IFG & BIT1)
	{
		unsigned char tmp=SPI_Read_Reg(READ_REG+STATUS);	//��ȡ�ж�״̬
		tmp&=0x70;
		SPI_Write_Reg(WRITE_REG+STATUS,tmp);				//���÷���ģʽ
		if(tmp&0x40)
		{
			LPM3_EXIT;
		}
		P1IFG &= ~BIT1;
	}

}

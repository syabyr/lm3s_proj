#include"systemInit.h"


void BUS_Write_Bit(unsigned char h_l)									//	����д0��1
{
	GPIOPinTypeGPIOOutput(DS18B20_BUS_PORT,DS18B20_BUS_PIN);			//	DS18B20���߹ܽ�Ϊ���ģʽ
	if(h_l==0)
	{
		BUS_0;															//	��������
	}
	else
	BUS_1;																//	��������
}


void DS18B20_Init(void)													//	DS18B20��ʼ������
{
//	char s1[20];
//	sprintf(s1,"��ʼ����\r\n");
//	UartSendString(s1);
	BUS_Write_Bit(0);
	time_us=0;
	while(time_us<95)													//	500us�����������ͷ�
	{
	}
	BUS_Write_Bit(1);
	time_us=0;
	GPIODirModeSet(DS18B20_BUS_PORT,DS18B20_BUS_PIN,GPIO_DIR_MODE_IN);	//	DS18B20���߹ܽ�Ϊ����ģʽ
	while(time_us<6)													//	��ʱ38us�ȴ�DS18B20��������ķ���
	{
	}			 
	existence_Flag=GPIOPinRead(DS18B20_BUS_PORT,DS18B20_BUS_PIN);		//	��DS18B20������
	if(existence_Flag==0)												//	�����ź�Ϊ�ͱ�ʾ��λ�ɹ�
	{	
		Init_Flag=1;
	}
	else																//	�����ź�Ϊ�߱�ʾ��λʧ��
	{
		Init_Flag=0;
	}
}


void reset(void)														//	��λ����
{
	while(release_Flag==0)												//	���DS18B20���ں�ֱ���ͷ����ߣ���Ȼ3����һֱ��λ�ȴ����
	{
		while(Init_Flag==0)													
		{
			DS18B20_Init();
		}							  
		time_us=0;
		while(time_us<20)												//	��ʱ100us�ȴ�DS18B20�ͷ�����
		{
		}
		release_Flag=GPIOPinRead(DS18B20_BUS_PORT,DS18B20_BUS_PIN);		//	��DS18B20������
	}
}


void Write_data(unsigned char temperature_alarm)						//	��DS18B20дһ�ֽڵ�ֵ
{
	unsigned char i;
//	char s2[20];
//	sprintf(s2,"д������\r\n");
//	UartSendString(s2);
	write_data=temperature_alarm;
	for(i=8;i>0;i--)
	{
		BUS_Write_Bit(0);												//	��������
		time_us=0;
		while(time_us<2)												//	��ʱ15us
		{
		}				
		if((write_data&0x01))
		{															    //	ȡ���λֵ�����ж�,Ϊ1ʱ������д1
			BUS_Write_Bit(1);
		}
		else															//	����д0
		{
			BUS_Write_Bit(0);
		}
		time_us=0;
 		while(time_us<7)												//	��ʱ45us
		{
		}
		write_data=write_data>>1;										//	����һλ�����ж�
		BUS_Write_Bit(1);									  			//	���������ͷ�ius		
	}
}


long read_data(void)												   //	������
{
	unsigned char i;
	long temp=0,temp1;
//	char s3[20];
//	sprintf(s3,"��������\r\n");
//	UartSendString(s3);
	for(i=8;i>0;i--)
	{
		temp=temp>>1;													//	����һλ
		BUS_Write_Bit(0);												//	��������	
		BUS_Write_Bit(1);												//	�ͷ�����
		GPIODirModeSet(DS18B20_BUS_PORT,DS18B20_BUS_PIN,GPIO_DIR_MODE_IN);	//	DS18B20���߹ܽ�Ϊ����ģʽ
		time_us=0;
 		while(time_us<1)												//	��ʱ8us
		{
		}																   
		temp1=GPIOPinRead(DS18B20_BUS_PORT,DS18B20_BUS_PIN);			//	��DS18B20������	
		if(temp1!=0)													//	����Ϊ1��λ��һ
		{
			temp=temp|0x80;
		}
		else															//	����Ϊ0��λ����
		{
			temp=temp|0x00;
		}
		time_us=0;
 		while(time_us<10)												//	��ʱ60us
		{
		}
		BUS_Write_Bit(1);												//	�ͷ�����
	}
	return temp;														//	���ض���ֵ
}


int Read_Temperature(void)
{
	int Temperature,Temph,Templ;
	unsigned char Minus_Flag;
	reset();														   //	��λ
	existence_Flag=1;
	release_Flag=0;
	Init_Flag=0;
	Write_data(0xCC);												   //	����ROM
	Write_data(0x44);												   //	�¶�ת��
	time_us=0;
	while(time_us<150)												   //	��ʱ750us,�ȴ�DS18B20�¶�ת������
	{
	}				 

	reset();														   //	��λ
	existence_Flag=1;
	release_Flag=0;
	Init_Flag=0;
	Write_data(0xCC);												   //	����ROM
	Write_data(0xBE);												   //	��ȡRAM

    Templ=read_data();												   //	��ȡ�Ͱ�λ��LS Byte,ROM0
	Temph=read_data();												   //	��ȡ�߰�λ��MS Byte,ROM1
	reset();														   //	��λ��ʾ�¶ȶ�ȡ����	
	existence_Flag=1;
	release_Flag=0;
	Init_Flag=0;

	if(Temph&0xFC)
	{																   //	����λΪ����Ϊ����
		Minus_Flag=1;
		Temperature=((Temph<<8)|Templ);								   //	�ߵ�λ����
		Temperature=((~Temperature)+1);								   //	����ȡ����һ
	   	Temperature*=0.0625;
	}
	else
	{
		Minus_Flag=0;
		Temperature=((Temph<<8)|Templ)*0.0625;
	}
	return Temperature;
}

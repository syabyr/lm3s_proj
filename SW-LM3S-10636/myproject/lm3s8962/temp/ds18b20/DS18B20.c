#include"systemInit.h"


void BUS_Write_Bit(unsigned char h_l)									//	总线写0或1
{
	GPIOPinTypeGPIOOutput(DS18B20_BUS_PORT,DS18B20_BUS_PIN);			//	DS18B20总线管脚为输出模式
	if(h_l==0)
	{
		BUS_0;															//	总线拉低
	}
	else
	BUS_1;																//	总线拉高
}


void DS18B20_Init(void)													//	DS18B20初始化函数
{
//	char s1[20];
//	sprintf(s1,"初始化，\r\n");
//	UartSendString(s1);
	BUS_Write_Bit(0);
	time_us=0;
	while(time_us<95)													//	500us后总线拉高释放
	{
	}
	BUS_Write_Bit(1);
	time_us=0;
	GPIODirModeSet(DS18B20_BUS_PORT,DS18B20_BUS_PIN,GPIO_DIR_MODE_IN);	//	DS18B20总线管脚为输入模式
	while(time_us<6)													//	延时38us等待DS18B20存在脉冲的发出
	{
	}			 
	existence_Flag=GPIOPinRead(DS18B20_BUS_PORT,DS18B20_BUS_PIN);		//	读DS18B20的脉冲
	if(existence_Flag==0)												//	存在信号为低表示复位成功
	{	
		Init_Flag=1;
	}
	else																//	存在信号为高表示复位失败
	{
		Init_Flag=0;
	}
}


void reset(void)														//	复位函数
{
	while(release_Flag==0)												//	检测DS18B20存在后直到释放总线，不然3秒内一直复位等待检测
	{
		while(Init_Flag==0)													
		{
			DS18B20_Init();
		}							  
		time_us=0;
		while(time_us<20)												//	延时100us等待DS18B20释放总线
		{
		}
		release_Flag=GPIOPinRead(DS18B20_BUS_PORT,DS18B20_BUS_PIN);		//	读DS18B20的脉冲
	}
}


void Write_data(unsigned char temperature_alarm)						//	向DS18B20写一字节的值
{
	unsigned char i;
//	char s2[20];
//	sprintf(s2,"写操作，\r\n");
//	UartSendString(s2);
	write_data=temperature_alarm;
	for(i=8;i>0;i--)
	{
		BUS_Write_Bit(0);												//	总线拉低
		time_us=0;
		while(time_us<2)												//	延时15us
		{
		}				
		if((write_data&0x01))
		{															    //	取最低位值进行判断,为1时对总线写1
			BUS_Write_Bit(1);
		}
		else															//	否则写0
		{
			BUS_Write_Bit(0);
		}
		time_us=0;
 		while(time_us<7)												//	延时45us
		{
		}
		write_data=write_data>>1;										//	右移一位继续判断
		BUS_Write_Bit(1);									  			//	总线拉高释放ius		
	}
}


long read_data(void)												   //	读函数
{
	unsigned char i;
	long temp=0,temp1;
//	char s3[20];
//	sprintf(s3,"读操作，\r\n");
//	UartSendString(s3);
	for(i=8;i>0;i--)
	{
		temp=temp>>1;													//	右移一位
		BUS_Write_Bit(0);												//	总线拉低	
		BUS_Write_Bit(1);												//	释放总线
		GPIODirModeSet(DS18B20_BUS_PORT,DS18B20_BUS_PIN,GPIO_DIR_MODE_IN);	//	DS18B20总线管脚为输入模式
		time_us=0;
 		while(time_us<1)												//	延时8us
		{
		}																   
		temp1=GPIOPinRead(DS18B20_BUS_PORT,DS18B20_BUS_PIN);			//	读DS18B20的数据	
		if(temp1!=0)													//	读数为1高位置一
		{
			temp=temp|0x80;
		}
		else															//	读数为0高位置零
		{
			temp=temp|0x00;
		}
		time_us=0;
 		while(time_us<10)												//	延时60us
		{
		}
		BUS_Write_Bit(1);												//	释放总线
	}
	return temp;														//	返回读数值
}


int Read_Temperature(void)
{
	int Temperature,Temph,Templ;
	unsigned char Minus_Flag;
	reset();														   //	复位
	existence_Flag=1;
	release_Flag=0;
	Init_Flag=0;
	Write_data(0xCC);												   //	跳过ROM
	Write_data(0x44);												   //	温度转换
	time_us=0;
	while(time_us<150)												   //	延时750us,等待DS18B20温度转换结束
	{
	}				 

	reset();														   //	复位
	existence_Flag=1;
	release_Flag=0;
	Init_Flag=0;
	Write_data(0xCC);												   //	跳过ROM
	Write_data(0xBE);												   //	读取RAM

    Templ=read_data();												   //	读取低八位，LS Byte,ROM0
	Temph=read_data();												   //	读取高八位，MS Byte,ROM1
	reset();														   //	复位表示温度读取结束	
	existence_Flag=1;
	release_Flag=0;
	Init_Flag=0;

	if(Temph&0xFC)
	{																   //	符号位为高则为负数
		Minus_Flag=1;
		Temperature=((Temph<<8)|Templ);								   //	高地位整合
		Temperature=((~Temperature)+1);								   //	负数取反加一
	   	Temperature*=0.0625;
	}
	else
	{
		Minus_Flag=0;
		Temperature=((Temph<<8)|Templ)*0.0625;
	}
	return Temperature;
}

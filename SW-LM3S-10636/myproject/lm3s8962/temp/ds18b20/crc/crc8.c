#include<stdio.h>

#define CRC8INIT 0x00
#define CRC8POLY 0x18


//CRC8部分
unsigned char crc8(unsigned char* data_in,short int number_of_bytes_to_read)
{
	unsigned char crc;
	short int loop_count;
	unsigned char bit_counter;
	unsigned char data;
	unsigned char feedback_bit;

	crc=CRC8INIT;

	for(loop_count=0;loop_count!=number_of_bytes_to_read;loop_count++)
	{
		data=data_in[loop_count];

		bit_counter=8;
		do
		{
			feedback_bit=(crc^data)&0x01;
			if(feedback_bit==0x01)
			{
				crc=crc^CRC8POLY;
			}
			crc=(crc>>1)&0x7F;
			if(feedback_bit==0x01)
			{
				crc=crc|0x80;
			}
			data=data>>1;
			bit_counter--;
		}while(bit_counter>0);
	}
	return crc;
}

unsigned char crc7(unsigned char * chr, int cnt)
{
	int i,a;
	unsigned char crc,Data;

	crc=0;
	for (a=0;a<cnt;a++)
	{
		Data=chr[a];
		for (i=0;i<8;i++)
		{
			crc <<= 1;

			if ((Data & 0x80)^(crc & 0x80))
			crc ^=0x09;
			Data <<= 1;
		}
	}
	crc=(crc<<1)|1;
	return(crc);
}


unsigned char CRC7_Handle(unsigned char *message, unsigned int CRC7_Num)
{ 
	unsigned char i; 
	unsigned int crc=0; 

	while(CRC7_Num--!=0) 
	{ 
		for(i=0x80; i!=0; i/=2) 
		{ 
			if((crc&0x40)!=0) 
			{
				crc*=2; 
				crc^=0x09;
			}    
			else 
			{
				crc*=2; 
			}
			if((*message&i)!=0) 
			{
				crc^=0x09;
			}
		} 
		message++; 
	} 
	crc = crc & 0x7f;
	crc = crc << 1;
	crc = crc | 0x01;
	return(crc); 
} 


unsigned char xCal_crc(unsigned char *ptr,int len)
{unsigned char crc;
 unsigned char i;
    crc = 0;
    while(len--)
    {
       crc ^= *ptr++;
       for(i = 0;i < 8;i++)
       {
           if(crc & 0x01)
           {
               crc = (crc >> 1) ^ 0x1d;
           }else crc >>= 1;
       }
    }
    return crc;
}

int main(void)
{
	unsigned char sp[3];
	int i;
	unsigned char crc;

	for(i=0;i<256;i++)
	{
		sp[0]=i;
		crc=0;
		sp[1]=xCal_crc(&sp[0],1);
		//crc=CRC7_Handle(&sp[0],1);
		crc=xCal_crc(&sp[0],2);
		printf("%3d:%3x   ",i,crc);
		if(i%8==7)
			printf("\n");
	}
}

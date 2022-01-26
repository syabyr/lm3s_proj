#include<stdio.h>
#include"crc.h"

int main(void)
{
	int i=0;
	char ta[1]={0};
	for(i=0;i<256;i++)
	{
		ta[0]=i;
		printf("%3d ",crct(ta,1));
		if(i%16==15)
		printf("\n");
	}
}

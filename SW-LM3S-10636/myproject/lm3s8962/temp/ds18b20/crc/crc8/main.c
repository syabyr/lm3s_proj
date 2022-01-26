#include<stdio.h>
#include"crc8.h"
int main(void)
{
	uint8_t *str="hello world";
//	uint8_t str="hello world";
	uint8_t result;
	result=crc8(str,2);
//	result=crc8(&str,1);
	printf("%d\n",result);
	printf("%d,%d\n",'h','e');
}





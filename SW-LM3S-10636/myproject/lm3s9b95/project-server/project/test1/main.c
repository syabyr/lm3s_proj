#include<stdio.h>

int main(void)
{
	int i;
	unsigned char test[1024];
	for(i=0;i<1024;i++)
	test[i]=i;
	for(i=0;i<1024;i++)
	printf("%d ",test[i]);
	printf("\n");
}

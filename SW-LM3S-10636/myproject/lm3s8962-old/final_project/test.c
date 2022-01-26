#include <stdio.h>
int main(void)
{

	#define DATA_BUFFER_SIZE 20
	char str[DATA_BUFFER_SIZE];
	char *pt=&str;
	strcpy(&pt,"hello world\n");
	printf("%s\n",str);
	sprintf(str, " %d",100);
	printf("%s\n",str);
	int i;
	for (i=0;i<6;i++)
	{
		printf("%c\n",*(str+i));
	}
	printf("%s",pt);
	return 0;
}

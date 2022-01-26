#include<cmath>
#include<iostream>
using namespace std;
#define PI 3.141592653589793
int main(void)
{
	int i=0;


	for(i=0;i<128;i++)
	{
//		cout<<(int)(80*(sin(PI*i/128)+1))<<",";
		cout<<95-(int(48*(sin(PI*i/32)+1)))<<",";

//		cout<<sin(PI*i/128)+1<<",";

		if((i+1)%8==0)
			cout<<endl;
		if((i+1)%64==0)
			cout<<endl;
	}

		cout<<endl;
}

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{

	string fileName;
	ifstream inData;
	ofstream outData;
	cout << "Enter the input file name: ";
	cin >> fileName;
	inData.open(fileName.c_str());
	outData.open("name1.txt");
}

#include<iostream>
#include<fstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 

using namespace std;


int main(int argc, char const *argv[])
{
	/* code */
	ofstream output;
	output.open("input.txt");

	int number;
	cout<<"enter size of input";
	cin>>number;
	output<<number<<endl;
	srand(time(NULL));
	for(int i=1;i<=number ;i++){
		output<<" v"<<rand()%100+1<<" "<<((rand()%7+1))*5<<" "<<rand()%2+1<<endl;
	}

	return 0;
}
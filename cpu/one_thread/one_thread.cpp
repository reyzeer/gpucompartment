//#include "stdafx.h"
//#include <Windows.h>
#include <ctime>
#include <map>
#include <thread>
//#include <conio.h>
#include <iostream>
#include <time.h>
#include <sstream>
#include <math.h>

using namespace std;

typedef unsigned long long int _uint64;

bool primeNumberTesting(_uint64 number) {

	_uint64 maxTestNumber = sqrt((double) number);

	if (number == 2) {
		return true;
	}

	if (number % 2 == 0) {
		return false;
	}

	bool prime = true;
	for (_uint64 n = 3; n < maxTestNumber; n+=2) {
		if (number % n == 0) {
			prime = false;
			break;
		}
	}

	return prime;

}

void rangeTest(_uint64 startRange, _uint64 endRange)
{

	cout << "Start.\n";
	clock_t start = clock();
	for (_uint64 i = startRange; i <= endRange; i++) {
		
		//primeNumberTesting(i);

		cout << i << " ";
		if (primeNumberTesting(i)) {
			cout << "jest pierwsza.\n";
		}
		else {
			cout << "nie jest pierwsza.\n";
		}


	}
	printf( "Czas wykonywania: %lu s\n", (clock() - start)/CLOCKS_PER_SEC );

}

int main(int argc, char ** argv)
{
	
	//18446744073709551614
	_uint64 endRange = 0;
	endRange -= 2;
	cout << endRange << endl;
	_uint64 startRange = endRange - 256;
	for (int i = 0; i < 10; i++) {
		rangeTest(startRange, endRange);
	}

	return 0;

}
//#include "stdafx.h"
//#include <Windows.h>
#include <ctime>
#include <vector>
#include <thread>
//#include <conio.h>
#include <iostream>
#include <time.h>
#include <sstream>
#include <math.h>

using namespace std;

typedef unsigned long long int _uint64;

//Watek -> Koniec
vector<bool> threadEnd;

bool primeResult = true;

void primeNumberTestingTask(int threads, int index, _uint64 number, _uint64 maxTestNumber)
{

	bool prime = true;
	int inc = threads * 2;
	for (_uint64 n = (index+1) * 2 + 1; n < maxTestNumber; n += inc) {
		if (!primeResult) {
			break;
		}

		if (number % n == 0) {
			prime = false;
			break;
		}
	}

	if (primeResult && !prime)
	{
		primeResult = false;
	}

	threadEnd[index] = true;

}

bool primeNumberTesting(_uint64 number)
{

	primeResult = true;
	
	if (number % 2 == 0) {
		return false;
	}

	_uint64 maxTestNumber = sqrt((double) number);

	//Uzyskiwanie liczby watkow
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
	int maxNumberOfThreads = concurentThreadsSupported;

	//Przygotowywanie zadan dla kazdego watku
	thread *threads = new thread[maxNumberOfThreads];
	threadEnd.clear();	
	for (int i = 0; i < maxNumberOfThreads; i++)
	{
		threads[i] = thread(primeNumberTestingTask, maxNumberOfThreads, i, number, maxTestNumber);
		threadEnd.push_back(false);
	}

	//Uruchamianie watkow
	for (int i = 0; i < maxNumberOfThreads; i++)
	{
		threads[i].join();
	}

	//Czeka az wszystkie watki skoncza liczyc 
	while (true) {
		bool stop = true;
		for (int i = 0; i < threadEnd.size(); i++)
	  	{
			//cout << " i: " << i << " : " << threadEnd[i] << endl;
			if (!threadEnd[i]) {
				stop = false;
			}
	   	}
		if (stop) {
			break;
		}
	}

	return primeResult;

}

void oneTest(_uint64 number)
{

	cout << "Start.\n";
	clock_t start = clock();
	cout << number << " ";
	if (primeNumberTesting(number)) {
		cout << "jest pierwsza.\n";
	}
	else {
		cout << "nie jest pierwsza.\n";
	}
	printf( "Czas wykonywania: %lu s\n", (clock() - start)/CLOCKS_PER_SEC/8 );

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
	printf( "Czas wykonywania: %lu s\n", (clock() - start)/CLOCKS_PER_SEC/8 );

}

int main(int argc, char ** argv)
{

	//18446744073709551359
	//oneTest(0 - 2 - 256 + 5);

	/*if (primeNumberTesting(821)) {
		cout << "pierwsza" << endl;
	}
	else {
		cout << "stop" << endl;
	}*/

	//18446744073709551614
	_uint64 endRange = 0;
	endRange -= 2;
	cout << endRange << endl;
	_uint64 startRange = endRange - 256;
	for (int i = 0; i < 10; i++) {
		rangeTest(startRange, endRange);
	}

	/*_uint64 startRange = 3;
	_uint64 endRange = 103;
	rangeTest(startRange, endRange);*/

	return 0;

}

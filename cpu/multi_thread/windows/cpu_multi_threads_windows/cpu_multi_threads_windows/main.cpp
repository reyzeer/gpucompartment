//#include "stdafx.h"
//#include <Windows.h>
#include <ctime>
#include <thread>
//#include <conio.h>
#include <iostream>
#include <time.h>
#include <sstream>
#include <math.h>
#include <mutex>

using namespace std;

typedef unsigned long long int _uint64;

//Watek -> Koniec
std::mutex g_lock;

void primeNumberTestingTask(bool * threadEnd, int threads, int index, _uint64 number, _uint64 maxTestNumber, bool * result)
{

	bool prime = true;
	int inc = threads * 2;
	_uint64 n = (index + 1) * 2 + 1;
	n = (n == 1) ? 2 : n;
	for (; n < maxTestNumber; n += inc) {
		if (!*result) {
			break;
		}
		if (number % n == 0) {
			*result = false;
			break;
		}
	}

	//g_lock.lock();
	threadEnd[index] = true;
	//g_lock.unlock();

}

bool primeNumberTesting(_uint64 number)
{

	if (number % 2 == 0) {
		return false;
	}

	bool * result = new bool;
	*result = true;

	_uint64 maxTestNumber = sqrt((double)number);

	//Uzyskiwanie liczby watkow
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
	int maxNumberOfThreads = concurentThreadsSupported;

	//Przygotowywanie zadan dla kazdego watku
	bool * threadEnd = new bool[maxNumberOfThreads]();
	thread *threads = new thread[maxNumberOfThreads];
	for (int i = 0; i < maxNumberOfThreads; i++)
	{
		threads[i] = thread(primeNumberTestingTask, threadEnd, maxNumberOfThreads, i, number, maxTestNumber, result);
	}

	//Uruchamianie watkow
	for (int i = 0; i < maxNumberOfThreads; i++)
	{
		threads[i].join();
	}

	//Czeka az wszystkie watki skoncza liczyc 
	while (true) {
		bool stop = true;
		for (int i = 0; i < maxNumberOfThreads; i++) {
			if (!threadEnd[i]) {
				stop = false;
			}
		}
		if (stop) {
			break;
		}
	}

	delete[] threadEnd;

	return *result;

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
	printf("Czas wykonywania: %f s\n", ((double)clock() - start) / CLOCKS_PER_SEC);

}

void rangeTest(_uint64 startRange, _uint64 endRange)
{

	cout << "Start.\n";
	clock_t start = clock();
	for (_uint64 i = startRange; i <= endRange; i++) {
		if (primeNumberTesting(i)) {
			cout << i << " jest pierwsza.\n";
		}
	}
	printf("Czas wykonywania: %f s\n", ((double)clock() - start) / CLOCKS_PER_SEC);

}

int main(int argc, char ** argv)
{

	//Uzyskiwanie liczby watkow
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
	int maxNumberOfThreads = concurentThreadsSupported;

	cout << "Liczba watkow: " << maxNumberOfThreads << endl;

	//18446744073709551614
	_uint64 endRange = 0;
	endRange -= 2;
	_uint64 startRange = endRange - 256;


	//startRange = 100;
	//endRange = 100;
	for (int i = 0; i < 10; i++) {
		rangeTest(startRange, endRange);
	}

	system("pause");

	return 0;

}

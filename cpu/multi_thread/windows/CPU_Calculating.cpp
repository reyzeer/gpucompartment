//#include "stdafx.h"
//#include <Windows.h>
#include <ctime>
#include <map>
#include <thread>
//#include <conio.h>
#include <iostream>
#include <time.h>

using namespace std;

clock_t startClock;
clock_t stopClock;
double timer = 0;

typedef unsigned long long int _int64;

map <_int64, int> primesNumbers;
map <_int64, int> ::iterator iter;

#define THREADS 8

void doTask(_int64 startRange, _int64 endRange, _int64 startNumberPerThread, _int64 endNumberPerThread)
{
	startClock = clock();
	for (_int64 j = startRange; j <= endRange; j += 2)
	{
		if(primesNumbers.find(j)->second == 0 )
		{
			continue;
		}
		for (_int64 i = startNumberPerThread; (i < j) && (i < endNumberPerThread); i++)
		{
			if(i == 0 || i == 1)
			{
				continue;
			}
			if(j%i == 0)
			{
				primesNumbers[j] = 0;
				break;			
			}
		}
	}
	stopClock = clock();
	double times = stopClock - startClock;
	timer = timer + times;
}


int main()
{
	//SYSTEM_INFO sysinfo;
	//GetSystemInfo(&sysinfo);

	_int64 maxNumberOfThreads = THREADS;
	cout << "Liczba watkow: " << maxNumberOfThreads;
	_int64 startRange = 171819020;
	_int64 endRange = 171819090;

		if(startRange%2 == 0)
		{
			startRange = startRange + 1;
		}

		if (startRange <= 2)
		{
			startRange = 3;
		}

		for (_int64 i = startRange; i <= endRange; i += 2)
		{
			primesNumbers[i] = 1;
		}

		thread *threads = new thread[(int)maxNumberOfThreads];

		_int64 numbersPerThread = (endRange / maxNumberOfThreads) + 1;

		for (int i = 0; i < maxNumberOfThreads; i++)
		{
			_int64 startNumberPerThread = i * numbersPerThread;
			_int64 endNumberPerThread = (i + 1) * numbersPerThread;
			threads[i] = thread(doTask, startRange, endRange, startNumberPerThread, endNumberPerThread);
		}

		for (int i = 0; i < maxNumberOfThreads; i++)
		{
			threads[i].join();
		}

		iter = primesNumbers.begin();
		while (iter != primesNumbers.end())
		{
			if (iter->second == 1)
			{
				cout << "Liczba pierwsza: " << iter->first << endl;
				iter++;
			}
			else
			{
				iter++;
				continue;
			}
		}

		cout << endl << "Czas wykonywania: " << (timer/(CLOCKS_PER_SEC)/maxNumberOfThreads);

	//_getch();

	return 0;

}

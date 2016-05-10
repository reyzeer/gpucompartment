//CUDA
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <thrust/iterator/counting_iterator.h>

//Others
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <ctime>

using namespace std;

typedef unsigned long long int _uint64;

#define THREADS 512

/* ------------------------------------------------------------
 	 	 	 CUDA func
 ------------------------------------------------------------ */

__device__ bool isPrime;

__global__ void primeNumberTesting(_uint64 number, _uint64 maxTestNumber) {

//__host__ __device__ void primeNumberTesting(_uint64 number, _uint64 maxTestNumber) {

	_uint64 threads	= blockDim.x;	//liczba watkow
	_uint64 thread	= threadIdx.x;	//numer aktualnego watku
	//_uint64 round	= blockIdx.x;	//numer przebiegu petli

	int inc = threads * 2;
	_uint64 n = (thread + 1) * 2 + 1;
	n = (n == 1) ? 2 : n;
	for ( ; n < maxTestNumber; n += inc) {
		if (!isPrime) {
			return;
		}
		if (number % n == 0) {
			isPrime = false;
			return;
		}
	
	}

}

/*__global__ void lackOfdivisorsInRange(_uint64 iNumber, _uint64 iStartRange, _uint64 iEndRange) {

    _uint64 threads	= blockDim.x;    //liczba watkow
    _uint64 thread	= threadIdx.x;   //numer aktualnego watku
    _uint64 round	= blockIdx.x;    //numer przebiegu petli

    //sprawdzany dzielnik
    _uint64 n = ((thread * 2 + 1) + 2 * threads * round) + iStartRange - ((iStartRange%2==1) ? 1 : 0);

    if (n <= iEndRange) {
        if (iNumber % n == 0) {
            isPrime = false;
        }
    }

    __syncthreads();

}*/



/* ------------------------------------------------------------
 	 	 	 	 	 	 CPU func
 ------------------------------------------------------------ */

bool primeNumberTestingStart(_uint64 number) {

	_uint64 iMaxTestNumber = sqrt((double) number); //maksymalna sprawdza wartosc

	// Error code to check return values for CUDA calls
	cudaError_t err = cudaSuccess;

	bool isPrime_Host = true;

	cudaMemcpyToSymbol(isPrime,&isPrime_Host,sizeof(bool),0,cudaMemcpyHostToDevice);

	primeNumberTesting<<<1, THREADS>>>(number, iMaxTestNumber);
	err = cudaGetLastError();

	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to launch primeNumberTesting kernel (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	cudaMemcpyFromSymbol(&isPrime_Host,isPrime,sizeof(bool),0,cudaMemcpyDeviceToHost);

	return isPrime_Host;

}

/*bool lackOfdivisorsInRange(_uint64 iNumber, _uint64 iStartRange, _uint64 iEndRange) {

	// Error code to check return values for CUDA calls
	cudaError_t err = cudaSuccess;

	bool isPrime_Host = true;

	cudaMemcpyToSymbol(isPrime,&isPrime_Host,sizeof(bool),0,cudaMemcpyHostToDevice);

	_uint64 blocksPerGrid = (iEndRange - iStartRange + 1)/THREADS/2+1;
	primeNumberTesting<<<blocksPerGrid, THREADS>>>(_uint64 iNumber, _uint64 iStartRange, _uint64 iEndRange);
	err = cudaGetLastError();

	if (err != cudaSuccess) {
		fprintf(stderr,
				"Failed to launch primeNumberTesting kernel (error code %s)!\n",
				cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	cudaMemcpyFromSymbol(&isPrime_Host,isPrime,sizeof(bool),0,cudaMemcpyDeviceToHost);

	return isPrime_Host;

}*/

/* ------------------------------------------------------------
 main
 ------------------------------------------------------------ */

void mersensNumberTest()
{

	//Tablica liczb Mersena
	_uint64 mersensNumber[32];
	for (int j = 0; j < 32; j++) {
		_uint64 number = 1;
		for (_uint64 i = 0; i < j; number *= 2, i++);
		number--;
		mersensNumber[j] = number;
	}

	cout << "Start.\n";
	clock_t start = clock();
	for (int j = 0; j < 1000; j++) {
		for (int i = 0; i < 32; i++) {
			cout << mersensNumber[i] << " ";
			if (primeNumberTestingStart(mersensNumber[i])) {
				cout << "jest pierwsza.\n";
			}
			else {
				cout << "nie jest pierwsza.\n";
			}
		}
	}
	printf( "Czas wykonywania: %f ms\n", ((double) clock() - start)/CLOCKS_PER_SEC );

}

void all32bitsNumber()
{
	
	cout << "Start.\n";
	clock_t start = clock();
	for (int i = 2; i < 4294967296; i++) {
		
		primeNumberTestingStart(i);

		/*cout << i << " ";
		if (primeNumberTestingStart(i)) {
			cout << "jest pierwsza.\n";
		}
		else {
			cout << "nie jest pierwsza.\n";
		}*/

	}
	printf( "Czas wykonywania: %f ms\n", ((double) clock() - start)/CLOCKS_PER_SEC );

}

void all24bitsNumber()
{
	
	cout << "Start.\n";
	clock_t start = clock();
	for (int i = 2; i < 16777216; i++) {
		
		primeNumberTestingStart(i);

		/*cout << i << " ";
		if (primeNumberTestingStart(i)) {
			cout << "jest pierwsza.\n";
		}
		else {
			cout << "nie jest pierwsza.\n";
		}*/

	}
	printf( "Czas wykonywania: %f s\n", ((double) clock() - start)/CLOCKS_PER_SEC );

}

void all16bitsNumber()
{
	
	cout << "Start.\n";
	clock_t start = clock();
	for (int i = 2; i < 65536; i++) {
		
		primeNumberTestingStart(i);

		/*cout << i << " ";
		if (primeNumberTestingStart(i)) {
			cout << "jest pierwsza.\n";
		}
		else {
			cout << "nie jest pierwsza.\n";
		}*/

	}
	printf( "Czas wykonywania: %f s\n", ((double) clock() - start)/CLOCKS_PER_SEC );

}

void rangeTest(_uint64 startRange, _uint64 endRange)
{

	if (endRange >= 18446744073709551615) {
		endRange = 18446744073709551614;
	}

	cout << "Start.\n";
	clock_t start = clock();
	for (_uint64 i = startRange; i <= endRange; i++) {
		
		/*if (primeNumberTestingStart(i)) {
			cout << i << " jest pierwsza.\n";
		}*/

		clock_t start2 = clock();
		cout << i << " ";
		if (primeNumberTestingStart(i)) {
			cout << "jest pierwsza.\n";
		}
		else {
			cout << "nie jest pierwsza.\n";
		}
		printf( "Czas wykonywania: %f s\n", ((double) clock() - start2)/CLOCKS_PER_SEC );

	}
	printf( "Czas wykonywania: %f s\n", ((double) clock() - start)/CLOCKS_PER_SEC );

}

int main()
{

	/*clock_t start = clock();
	if (primeNumberTestingStart(821)) {
		cout << "jest pierwsza.\n";
	}
	else {
		cout << "nie jest pierwsza.\n";
	}
	printf( "Czas wykonywania: %f s\n", ((double) clock() - start)/CLOCKS_PER_SEC );

	start = clock();
	if (primeNumberTestingStart(820)) {
		cout << "jest pierwsza.\n";
	}
	else {
		cout << "nie jest pierwsza.\n";
	}
	printf( "Czas wykonywania: %f s\n", ((double) clock() - start)/CLOCKS_PER_SEC );

	start = clock();
	if (primeNumberTestingStart(18446744073709551614-3)) {
		cout << "jest pierwsza.\n";
	}
	else {
		cout << "nie jest pierwsza.\n";
	}
	printf( "Czas wykonywania: %f s\n", ((double) clock() - start)/CLOCKS_PER_SEC );*/

	//mersensNumberTest()
	//all16bitsNumber();
	//all24bitsNumber();
	//all32bitsNumber();

	_uint64 endRange = 18446744073709551614;
	_uint64 startRange = endRange - 256;
	//for (int i = 0; i < 10; i++) {
		rangeTest(startRange, endRange);
	//}

	return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#include <time.h>

#define KERNEL_SIZE (0x400)
#define PLATFORM 0
// Platform 0 - GPU, Platform 1 - CPU
typedef unsigned long long int _uint64;

void pause() {
	int x;
	scanf_s("%d", &x);
}

bool primeNumberTestingStart(cl_ulong checkNumber) {

	cl_ulong *Number = NULL;
	cl_int *WorkGroups = NULL;
	cl_ulong *Output = NULL;
	int *Status = NULL;

	const int elements = 64;//checkNumber;

	//Rozmiar wektora wyjœciowego
	size_t dataSize = sizeof(cl_ulong)*elements;

	//Alokowanie pamiêci zmiennych
	Number = (cl_ulong*)malloc(sizeof(cl_ulong));
	WorkGroups = (cl_int*)malloc(sizeof(int));
	Output = (cl_ulong*)malloc(dataSize);
	Status = (int*)malloc(sizeof(int));

	//Inicjalizacja zmiennych
	*Number = checkNumber;
	*WorkGroups = elements;
	Status[0] = 0;
	//printf("%llu %llu\n", Number[0], checkNumber);

	//Do sprawdzania wartoœci odwo³añ do API
	cl_int status;

	//---
	// Inicjalizacja platformy
	//---

	cl_uint platformsNumber = 0;
	cl_platform_id *platforms = NULL;

	status = clGetPlatformIDs(0, NULL, &platformsNumber);
	platforms = (cl_platform_id*)malloc(platformsNumber*sizeof(cl_platform_id));
	status = clGetPlatformIDs(platformsNumber, platforms, NULL);

	//---
	// Inicjalizacja urzadzenia
	//---

	cl_uint devicesNumber = 0;
	cl_device_id *devices = NULL;

	//platforms[0] gdyz interesuje nas tylko karta graficzna
	status = clGetDeviceIDs(platforms[PLATFORM], CL_DEVICE_TYPE_ALL, 0, NULL, &devicesNumber);
	devices = (cl_device_id*)malloc(devicesNumber*sizeof(cl_device_id));
	status = clGetDeviceIDs(platforms[PLATFORM], CL_DEVICE_TYPE_ALL, devicesNumber, devices, NULL);

	//---
	// Kontekst
	//---

	cl_context context = NULL;
	context = clCreateContext(NULL, devicesNumber, devices, NULL, NULL, &status);

	//---
	// Kolejka zadañ
	//---

	cl_command_queue cmdQueue;
	cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);

	//---
	// Bufory urz¹dzenia
	//---

	cl_mem bufferNumber;
	cl_mem bufferWorkGroups;
	cl_mem bufferOutput;
	cl_mem bufferStatus;

	bufferNumber = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_ulong), NULL, &status);
	bufferWorkGroups = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_int), NULL, &status);
	bufferOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, dataSize, NULL, &status);
	bufferStatus = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

	//---
	// Wprowadzenie danych do bufora
	//---

	status = clEnqueueWriteBuffer(cmdQueue, bufferNumber, CL_FALSE, 0, sizeof(cl_ulong), Number, 0, NULL, NULL);
	status += clEnqueueWriteBuffer(cmdQueue, bufferWorkGroups, CL_FALSE, 0, sizeof(cl_int), WorkGroups, 0, NULL, NULL);

	//---
	// Stworzenie programu
	//---

	FILE *fp;
	char *source_str;
	size_t source_size;
	errno_t error;

	if ((error = fopen_s(&fp, "./kernel.cl", "r")) != 0) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	else {
		source_str = (char*)malloc(KERNEL_SIZE);
		source_size = fread(source_str, 1, KERNEL_SIZE, fp);
		fclose(fp);
		//fprintf(stderr, "Kernel loaded\n");
		//Wyœwietlanie kodu kernela
		//printf("%d: %s", source_size, source_str); 
	}

	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &status);
	status = clBuildProgram(program, devicesNumber, devices, NULL, NULL, NULL);

	//---
	// Stworzenie kernela
	//---

	cl_kernel kernel = NULL;
	kernel = clCreateKernel(program, "prime", &status);

	//---
	// Ustawienie argumentow kernela
	//---

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferNumber);
	status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferWorkGroups);
	status |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferOutput);
	status |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufferStatus);

	//---
	// Konfiguracja struktury pracy
	//---

	size_t globalWorkSize[1];
	globalWorkSize[0] = elements;

	//---
	// Uruchomienie kernela
	//---

	cl_event kernelEvent;
	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, 0, globalWorkSize, NULL, 0, NULL, &kernelEvent);

	//---
	// Licznik czasu operacji
	//---

	//cl_ulong ev_start_time = (cl_ulong)0;
	//cl_ulong ev_end_time = (cl_ulong)0;

	//clFinish(cmdQueue);
	//error = clWaitForEvents(1, &kernelEvent);
	//error |= clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	//error |= clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);

	//float run_time_gpu_usec = (float)(ev_end_time - ev_start_time) / 1000; // in usec
	//float run_time_gpu_sec = (float)(run_time_gpu_usec * 0.000001);

	//---
	// Wczytanie bufora do pamiêci
	//---

	clEnqueueReadBuffer(cmdQueue, bufferOutput, CL_TRUE, 0, dataSize, Output, 0, NULL, NULL);
	clEnqueueReadBuffer(cmdQueue, bufferStatus, CL_TRUE, 0, sizeof(int), Status, 0, NULL, NULL);

	//---
	// Sprawdzenie pierwszoœci liczby
	//---

	int x = 0;
	for (long long int i = 0; i < elements; i++) {
		printf("%llu	", Output[i]);
		if (Output[i] > 0) {
			return 0;
		}

		//x += Output[i];
	}
	return 1;
	//printf("Status=%d", x);

	//bool result = true;
	//for (long long int i = 2; i < elements; i++) {
	//	if (Output[i] == false) {
	//		printf("%d: ", i);
	//		result = false;
	//		break;
	//	}
	//}

	//if (result)
	//	printf("Number is prime \n");
	//else
	//	printf("Number isn't prime \n");
	////printf("Time:\n   %f microseconds\n   %f seconds\n", run_time_gpu_usec, run_time_gpu_sec);

	//if (Status) {
	//	printf("1");
	//}
	//else
	//	printf("0");

}

void rangeTest(_uint64 startRange, _uint64 endRange)
{

	if (endRange >= 18446744073709551615) {
		endRange = 18446744073709551614;
	}

	printf( "Start.\n");
	clock_t start = clock();
	for (_uint64 i = startRange; i <= endRange; i++) {
		if (primeNumberTestingStart(i)) {
			printf("%llu jest pierwsza.\n", i);
		}
	}
	printf("Czas wykonywania: %f s\n", ((double)clock() - start) / CLOCKS_PER_SEC);

}

int main() {

	_uint64 i = 2147483647;				//	2^31 -1
	//_uint64 i = 2305843009213693951;	//	2^62 -1
	//_uint64 i = 15481303;
	//_uint64 i = 131071;
	if(primeNumberTestingStart(i))
		printf("%llu jest pierwsza.\n", i);
	else
		printf("%llu nie jest pierwsza.\n", i);

	//_uint64 endRange = 18446744073709551614;
	//_uint64 startRange = endRange - 256;
	//for (int i = 0; i < 10; i++) {
	//	rangeTest(startRange, endRange);
	//}

	

	//---
	// Czyszczenie pamiêci
	//---

	//clReleaseKernel(kernel);
	//clReleaseProgram(program);
	//clReleaseCommandQueue(cmdQueue);
	//clReleaseContext(context);
	//clReleaseMemObject(bufferNumber);
	//clReleaseMemObject(bufferOutput);
	//free(platforms);
	//free(devices);
	//free(Number);
	//free(Output);
	//free(Status);

	pause();

}


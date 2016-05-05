#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define KERNEL_SIZE (0x400)

void pause() {
	int x;
	scanf_s("%d", &x);
}

int main() {

	long long unsigned int checkNumber = 0;
	printf("Check number:\n >");
	scanf_s("%llu", &checkNumber);

	cl_ulong *Number = NULL;
	bool *Output = NULL;

	const int elements = checkNumber;

	//Rozmiar wektora wyjœciowego
	size_t dataSize = sizeof(bool)*elements;

	//Alokowanie pamiêci zmiennych
	Number = (cl_ulong*)malloc(sizeof(cl_ulong));
	Output = (bool*)malloc(dataSize);

	//Inicjalizacja zmiennych
	Number[0] = checkNumber;
	printf("%llu %llu\n", Number[0], checkNumber);

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
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &devicesNumber);
	devices = (cl_device_id*)malloc(devicesNumber*sizeof(cl_device_id));
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, devicesNumber, devices, NULL);

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
	cl_mem bufferOutput;

	bufferNumber = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_ulong), NULL, &status);
	bufferOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, dataSize, NULL, &status);

	//---
	// Wprowadzenie danych do bufora
	//---

	status = clEnqueueWriteBuffer(cmdQueue, bufferNumber, CL_FALSE, 0, sizeof(cl_ulong), Number, 0, NULL, NULL);

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
		fprintf(stderr, "Kernel loaded\n");
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
	status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferOutput);

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

	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;

	clFinish(cmdQueue);
	error = clWaitForEvents(1, &kernelEvent);
	error |= clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	error |= clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);

	float run_time_gpu_usec = (float)(ev_end_time - ev_start_time) / 1000; // in usec
	float run_time_gpu_sec = (float)(run_time_gpu_usec * 0.000001);

	//---
	// Wczytanie bufora do pamiêci
	//---

	clEnqueueReadBuffer(cmdQueue, bufferOutput, CL_TRUE, 0, dataSize, Output, 0, NULL, NULL);

	//---
	// Sprawdzenie pierwszoœci liczby
	//---

	bool result = true;
	for (long long int i = 2; i < elements; i++) {
		if (Output[i] == false) {
			printf("%d: ", i);
			result = false;
			break;
		}
	}

	if (result)
		printf("Number is prime \n");
	else
		printf("Number isn't prime \n");
	printf("Time:\n   %f microseconds\n   %f seconds\n", run_time_gpu_usec, run_time_gpu_sec);


	//---
	// Czyszczenie pamiêci
	//---

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseContext(context);
	clReleaseMemObject(bufferNumber);
	clReleaseMemObject(bufferOutput);
	free(platforms);
	free(devices);
	free(Number);
	free(Output);

	pause();

}


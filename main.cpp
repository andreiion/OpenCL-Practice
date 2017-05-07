//============================================================================
// Name        : opencl-tutorial_ep3.cpp
// Author      : Andrei Ion
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CL/cl.h>
#include <iostream>
#include <string.h>
#include <string>
#include "helper.hpp"
#include "helper.cpp"

using namespace std;

int runCL(float *a, float *b, float *ans, int n)
{
	cl_program program[1];
	cl_kernel kernel[2];
	cl_platform_id *platforms;

	cl_command_queue cmd_queue;
	cl_context context;

	cl_device_id cpu = NULL, device = NULL;

	cl_int err = 0;
	size_t returned_size = 0;
	size_t buffer_size;
	cl_uint platforms_size;

	cl_mem a_mem, b_mem, ans_mem;

	string program_source;

//Platform Information
	err = clGetPlatformIDs(0, NULL, &platforms_size);
	assert(err == CL_SUCCESS);

	cout << "Number of platforms: " << platforms_size << '\n';
//Device Information

	//Find the CPU CL device, as a fallback
	err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_CPU, 1, &cpu, NULL);
	assert(err == CL_SUCCESS);

	//Find the GPU CL device, this is what we really want
	//If there is no GPU device is CL_capable, fallback back to CPU
	err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	if (err != CL_SUCCESS)
		device = cpu;
	//assert(device);

	//Get some info about the returned device
	cl_char vendor_name[1024] = {0};
	cl_char device_name[1024] = {0};
	err = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor_name), vendor_name, &returned_size);
	err |= clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), device_name, &returned_size);
	//assert(err == CL_SUCCESS);
	printf("Connecting to %s %s...\n", vendor_name, device_name);

//Context and Command Queue

	// Now create a context to perform our calculation with the
	// specified device
	context = clCreateContext(0, 1, &device, NULL, NULL, &err);
	//assert(err == CL_SUCCESS);

	//Also a command queue from the context
	cmd_queue = clCreateCommandQueue(context, device, 0, NULL);

//Kernel Creation

	//Load the program source from disk
	//The kernel/program is the project directory and ...
	string filename = "simple.cl";
	read_kernel(filename, program_source);
	program[0] = clCreateProgramWithSource(context, 1, (const char **)&program_source, NULL, &err);
	//assert(err == CL_SUCCESS);

	err = clBuildProgram(program[0], 0, NULL, NULL, NULL, NULL);
	//assert(err == CL_SUCCESS);

	//Now create the kernel "objects" that we want to use in the example file
	kernel[0] = clCreateKernel(program[0], "add", &err);

	buffer_size = sizeof(float) * n;

	// Input array a
	a_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, buffer_size, NULL, NULL);
	err = clEnqueueWriteBuffer(cmd_queue, a_mem, CL_TRUE, 0, buffer_size,
								(void *)a, 0, NULL, NULL);

	b_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, buffer_size, NULL, NULL);
	err |= clEnqueueWriteBuffer(cmd_queue, a_mem, CL_TRUE, 0, buffer_size,
								(void *)b, 0, NULL, NULL);
	//assert(err == CL_SUCCESS);

	//Result array
	ans_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, buffer_size, NULL, NULL);

	//Get all of the stuff written and allocated
	clFinish(cmd_queue);

//Setup Kernel Arguments

	// Setup the arguments to our kernel
	err = clSetKernelArg(kernel[0], 0, sizeof(cl_mem), &a_mem);
	err |= clSetKernelArg(kernel[0], 1, sizeof(cl_mem), &b_mem);
	err |= clSetKernelArg(kernel[0], 2, sizeof(cl_mem), &ans_mem);
	//assert(err == CL_SUCCESS);

//Execution and Read

	// Run the calculation by enquing it and forcing the
	// command queue to complete the task
	size_t global_work_size = n;
	err = clEnqueueNDRangeKernel(cmd_queue, kernel[0], 1, 0, &global_work_size, NULL, 0, NULL, NULL);
	//assert(err == CL_SUCCESS);
	clFinish(cmd_queue);

	//Once finished, read back the results from the ans to array result array
	err = clEnqueueReadBuffer(cmd_queue, ans_mem, CL_TRUE, 0, buffer_size, ans, 0, NULL, NULL);
	//assert(err == CL_SUCCESS);
	clFinish(cmd_queue);

//Teardown
	clReleaseCommandQueue(cmd_queue);
	clReleaseContext(context);

	return CL_SUCCESS;
}

int main (int argc, const char* argv[]) {

	int n = 32;

	float *a = new float[n];
	float *b = new float[n];
	float *res = new float[n];

	for (int i = 0; i < n; ++i) {
		a[i] = i;
		b[i] = n - i;
		res[i] = 0.f;
	}

	runCL(a, b, res, n);

	for (int i = 0; i < n; ++i) {
		cout << res[i] << ' ';
	}
	cout << '\n';

	delete[] a;
	delete[] b;
	delete[] res;

	return EXIT_SUCCESS;
}

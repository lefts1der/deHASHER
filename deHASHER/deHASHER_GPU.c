//#include "C:\Users\vilda\AMD APP SDK\3.0\include\SDKUtil\CLUtil.hpp"
#define _CRT_SECURE_NO_WARNINGS
#define AMD_GPU 1
#define CL_DEVICE_CPU_OR_GPU CL_DEVICE_TYPE_GPU
#define AMD_NVIDIA "AMD"

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define source_file "C:\\Users\\vilda\\source\\repos\\deHASHER\\deHASHER\\kernel_dehasher.cl"

char* readSource(const char* sourceFilename) {

	FILE* fp;
	int err;
	int size;

	char* source;

	fp = fopen(sourceFilename, "rb");
	if (fp == NULL) {
		printf("Could not open kernel file: %s\n", sourceFilename);
		exit(-1);
	}

	err = fseek(fp, 0, SEEK_END);
	if (err != 0) {
		printf("Error seeking to end of file\n");
		exit(-1);
	}

	size = ftell(fp);
	if (size < 0) {
		printf("Error getting file position\n");
		exit(-1);
	}

	err = fseek(fp, 0, SEEK_SET);
	if (err != 0) {
		printf("Error seeking to start of file\n");
		exit(-1);
	}

	source = (char*)malloc(size + 1);
	if (source == NULL) {
		printf("Error allocating %d bytes for the program source\n", size + 1);
		exit(-1);
	}

	err = fread(source, 1, size, fp);
	if (err != size) {
		printf("only read %d bytes\n", err);
		exit(0);
	}

	source[size] = '\0';

	return source;
}

void main() {
	int deviceCount = 0;
	int multiCount = 1;
	cl_int status;
	long long pwd_count = 0;
	const uint64_t BLOCK_SIZE = 1073741824;
	const uint64_t BLOCKS = 1073741824;
	const uint64_t DEMENSION = 100;

#ifdef AMD_GPU
	_putenv("GPU_DUMP_DEVICE_KERNEL=3");
#endif

	///////////////////    OpenCL detection //////////////////////////

	cl_uint numPlatforms = 0;
	cl_platform_id* platforms;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		printf("clGetPlatformIDs failed\n");
		exit(-1);
	}

	if (numPlatforms == 0)
	{
		printf("No platforms detected.\n");
		exit(-1);
	}

	platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
	if (platforms == NULL)
	{
		perror("malloc");
		exit(-1);
	}

	clGetPlatformIDs(numPlatforms, platforms, NULL);
	if (status != CL_SUCCESS)
	{
		printf("clGetPlatformIDs failed\n");
		exit(-1);
	}

	printf("%u platforms detected\n", numPlatforms);

	int selected = 0;
	for (unsigned int i = 0; i < numPlatforms; i++)
	{
		char buf[100];
		printf("Platform %u: \n", i);
		status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,
			sizeof(buf), buf, NULL);
		printf("\tVendor: %s\n", buf);
		status |= clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME,
			sizeof(buf), buf, NULL);
		printf("\tName: %s\n", buf);

		if (strstr(buf, AMD_NVIDIA) != NULL) selected = i;


		if (status != CL_SUCCESS) {
			printf("clGetPlatformInfo failed\n");
			exit(-1);
		}
	}
	printf("\n");

	cl_uint numDevices = 0;
	cl_device_id* devices;

	status = clGetDeviceIDs(platforms[selected], CL_DEVICE_CPU_OR_GPU, 0, NULL,
		&numDevices);
	if (status != CL_SUCCESS)
	{
		printf("clGetDeviceIDs failed\n");
		exit(-1);
	}

	if (numDevices == 0)
	{
		printf("No devices detected.\n");
		exit(-1);
	}


	devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
	if (devices == NULL) {
		perror("malloc");
		exit(-1);
	}

	status = clGetDeviceIDs(platforms[selected], CL_DEVICE_CPU_OR_GPU, numDevices,
		devices, NULL);
	if (status != CL_SUCCESS) {
		printf("clGetDeviceIDs failed\n");
		exit(-1);
	}


	printf("%u devices detected\n", numDevices);
	for (unsigned int i = 0; i < numDevices; i++)
	{
		char buf[100];
		size_t par;
		printf("Device %u: \n", i);
		status = clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR,
			sizeof(buf), buf, NULL);
		printf("\tDevice: %s\n", buf);
		status |= clGetDeviceInfo(devices[i], CL_DEVICE_NAME,
			sizeof(buf), buf, NULL);
		printf("\tName: %s\n", buf);

		status |= clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE,
			sizeof(size_t), &par, NULL);

		printf("\tMax threads: %d\n", par);


		status |= clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS,
			sizeof(size_t), &par, NULL);

		printf("\tMax cores: %d\n", par);


		if (status != CL_SUCCESS) {
			printf("clGetDeviceInfo failed\n");
			exit(-1);
		}
	}
	printf("\n");

	cl_context context;

	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	if (status != CL_SUCCESS || context == NULL)
	{
		printf("clCreateContext failed\n");
		exit(-1);
	}

	cl_command_queue cmdQueue;
	cmdQueue = clCreateCommandQueueWithProperties(context, devices[0], 0, &status);
	if (status != CL_SUCCESS || cmdQueue == NULL)
	{
		printf("clCreateCommandQueue failed\n");
		exit(-1);
	}
	cl_program program;

	char* source;


	source = readSource(source_file);

	program = clCreateProgramWithSource(context, 1, (const char**)&source,
		NULL, &status);
	if (status != CL_SUCCESS)
	{
		printf("clCreateProgramWithSource failed\n");
		exit(-1);
	}

	cl_int buildErr;
	const char* options = "-D " AMD_NVIDIA "_GPU";

	unsigned char input[16] = {
		0x79, 0xaf, 0x87, 0x72,
		0x3d, 0xc2, 0x95, 0xf9,
		0x5b, 0xdb, 0x27, 0x7a,
		0x61, 0x18, 0x9a, 0x2a
	};

	unsigned char* out=( char*)malloc(10*sizeof( char));
	cl_mem DeviceMemInput = clCreateBuffer(context, CL_MEM_READ_ONLY |
		CL_MEM_COPY_HOST_PTR, 16 * sizeof(unsigned char), input, 0);
	cl_mem DeviceMemOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY |
		CL_MEM_COPY_HOST_PTR, 10 * sizeof(unsigned char), out, 0);

	buildErr = clBuildProgram(program, numDevices, devices, 0, NULL, NULL);

	if (buildErr != CL_SUCCESS)
	{
		printf("Program failed to build.\n");
		cl_build_status buildStatus;
		for (unsigned int i = 0; i < numDevices; i++) {
			clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_STATUS,
				sizeof(cl_build_status), &buildStatus, NULL);
			//     if(buildStatus == CL_SUCCESS) {
			//        continue;
			//     }

			char* buildLog;
			size_t buildLogSize;
			clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_LOG,
				0, NULL, &buildLogSize);
			buildLog = (char*)malloc(buildLogSize);
			if (buildLog == NULL) {
				perror("malloc");
				exit(-1);
			}
			clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_LOG,
				buildLogSize, buildLog, NULL);
			buildLog[buildLogSize - 1] = '\0';
			printf("Device %u Build Log:\n%s\n", i, buildLog);
			free(buildLog);
		}
		if (buildErr != CL_SUCCESS)
			exit(0);
	}


	cl_kernel kernel;

	kernel = clCreateKernel(program, "deHASH", &status);
	if (status != CL_SUCCESS)
	{
		printf("clCreateKernel failed\n");
		exit(-1);
	}

	
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&DeviceMemInput);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&DeviceMemOutput);

	cl_event ndrEvent;
	clEnqueueNDRangeKernel(cmdQueue, kernel, 1, 0, &DEMENSION,
		0, 0, 0, &ndrEvent);
	status = clWaitForEvents(1, &ndrEvent);
	status = clEnqueueReadBuffer(cmdQueue, DeviceMemOutput, CL_TRUE, 0,
		10 * sizeof(unsigned char), out, 0, 0, 0);
	printf("%s", out);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(DeviceMemInput);
	clReleaseMemObject(DeviceMemOutput);
	clReleaseCommandQueue(cmdQueue);
	clReleaseContext(context);


}
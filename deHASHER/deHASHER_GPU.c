//#include "C:\Users\vilda\AMD APP SDK\3.0\include\SDKUtil\CLUtil.hpp"
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define _CRT_SECURE_NO_WARNINGS


#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define source_file "C:\\Users\\vilda\\source\\repos\\deHASHER\\deHASHER\\kernel_dehasher.cl"
cl_device_id* devices;
cl_device_id get_device() {

#ifdef AMD_GPU
	putenv("GPU_DUMP_DEVICE_KERNEL=3");
#endif

	///////////////////    OpenCL detection //////////////////////////

	cl_uint numPlatforms = 0;
	cl_platform_id* platforms;
	cl_int status;

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

		if (strstr(buf, "AMD") != NULL) selected = i;


		if (status != CL_SUCCESS) {
			printf("clGetPlatformInfo failed\n");
			exit(-1);
		}
	}
	printf("\n");

	cl_uint numDevices = 0;

	status = clGetDeviceIDs(platforms[selected], CL_DEVICE_TYPE_GPU, 0, NULL,
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

	status = clGetDeviceIDs(platforms[selected], CL_DEVICE_TYPE_GPU, numDevices,
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
	
	unsigned int dev;
	while (1) {
		printf("Choose device:\n");		
		scanf_s("%u", &dev);
		if (dev >= numDevices) printf("!!!Incorrect device!!!\n");
		else return devices[dev];
	}
} 
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
	const unsigned int BLOCK_SIZE = 1073741824;
	const unsigned int BLOCKS = 1073741824;
	const uint64_t DEMENSION = BLOCKS * BLOCK_SIZE;

	cl_device_id device;
	device = get_device();

	cl_int status;

	cl_context context = clCreateContext(0, 1, &device, NULL, NULL, &status);
	cl_command_queue cmdQueue = clCreateCommandQueueWithProperties(context, device, 0, &status);
	char* source = readSource(source_file);
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source,
		NULL, &status);
	cl_int buildErr;
	const char* options = "-D AMD_GPU";
	buildErr = clBuildProgram(program, 0, devices, options, NULL, NULL);
	
	cl_kernel kernel = clCreateKernel(program, "deHASH", &status);

	unsigned char input[16] = {
		0x79, 0xaf, 0x87, 0x72,
		0x3d, 0xc2, 0x95, 0xf9,
		0x5b, 0xdb, 0x27, 0x7a,
		0x61, 0x18, 0x9a, 0x2a
	};

	unsigned char out[10];
	cl_mem DeviceMemInput = clCreateBuffer(context, CL_MEM_READ_ONLY |
		CL_MEM_COPY_HOST_PTR, 16 * sizeof(unsigned char), input, 0);
	cl_mem DeviceMemOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		10 * sizeof(unsigned char), input, 0);
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&DeviceMemInput);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&DeviceMemOutput);

	clEnqueueNDRangeKernel(cmdQueue, kernel, 1, 0, &DEMENSION,
		0, 0, 0, 0);
	clEnqueueReadBuffer(context, DeviceMemOutput, CL_TRUE, 0,
		10 * sizeof(unsigned char), out, 0, 0, 0);
	printf("%s", out);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(DeviceMemInput);
	clReleaseMemObject(DeviceMemOutput);
	clReleaseCommandQueue(cmdQueue);
	clReleaseContext(context);


}
// System includes
#include <stdio.h>
#include <stdlib.h>
#include "adcUtilsOpenCL.h"
#include <math.h>

// OpenCL includes
#include <CL/cl.h>

#define PI 3.14159265

float getCos(float angle) {
    double result, rad;

    rad = PI / 180.0;
    result = cos((double)angle*rad);
    

    printf("El coseno de %.6f es: %.6f\n", angle, result);
    return (float)result;
}

float getSin(float angle) {
    double result, rad;

    rad = PI / 180.0;
    result = sin((double)angle*rad);

    printf("El seno de %.6f es: %.6f\n", angle, result);
    return (float)result;
}


int main() {
    // This code executes on the OpenCL host

    // things
    float angle = 0.0;
    float cos = getCos(angle);
    float sin = getSin(angle);

    int width, height;

    // Read image from file
    float *imgToProcess = readImage("input.bmp", &width, &height);
    int ox = 0, oy = 0;

    int dataSize = height*width*sizeof(float);

    float *outputImg = NULL;
    outputImg = (float*)malloc(dataSize);
    const char* outputFile = "output.bmp";

    // Use this to check the output of each API call
    cl_int status;  
    
    printf("Por crear plataformas\n");

    // Retrieve the number of platforms
    cl_uint numPlatforms = 0;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
 
    // Allocate enough space for each platform
    cl_platform_id *platforms = NULL;
    platforms = (cl_platform_id*)malloc(
        numPlatforms*sizeof(cl_platform_id));
 
    // Fill in the platforms
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);

    printf("Por crear devices\n");

    // Retrieve the number of devices
    cl_uint numDevices = 0;
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, 
        NULL, &numDevices);

    // Allocate enough space for each device
    cl_device_id *devices;
    devices = (cl_device_id*)malloc(
        numDevices*sizeof(cl_device_id));

    // Fill in the devices 
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL,        
        numDevices, devices, NULL);

    printf("Por crear el contexto\n");

    // Create a context and associate it with the devices
    cl_context context;
    context = clCreateContext(NULL, numDevices, devices, NULL, 
        NULL, &status);

    printf("Por crear queue\n");

    // Create a command queue and associate it with the device 
    cl_command_queue cmdQueue;
    cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);

    // Create a buffer object that will contain the image byte array
    cl_mem bufInput;
    bufInput = clCreateBuffer(context, CL_MEM_READ_ONLY, dataSize,
       NULL, &status);

    // Create a buffer object that will hold the output image
    cl_mem bufOutput;
    bufOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, dataSize,
        NULL, &status); 
    
    // Write input image to the device buffer bufInput
    status = clEnqueueWriteBuffer(cmdQueue, bufInput, CL_TRUE, 
        0, dataSize, imgToProcess, 0, NULL, NULL);

    // Create a program with source code
    const char* source = readSource("rotation.cl");
    cl_program program;
    program = clCreateProgramWithSource(context, 1, &source, NULL, &status);

    // Build (compile) the program for the device
    status = clBuildProgram(program, numDevices, devices, 
        NULL, NULL, NULL);

    printf("Por crear el kernel\n");

    // Create the vector addition kernel
    cl_kernel kernel;
    kernel = clCreateKernel(program, "vecadd", &status);

    // Associate the input and output buffers with the kernel 
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufOutput);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufInput);
    status = clSetKernelArg(kernel, 2, sizeof(cl_int), &ox);
    status = clSetKernelArg(kernel, 3, sizeof(cl_int), &oy);
    status = clSetKernelArg(kernel, 4, sizeof(cl_int), &width);
    status = clSetKernelArg(kernel, 5, sizeof(cl_int), &height);
    status = clSetKernelArg(kernel, 6, sizeof(cl_float), &sin);
    status = clSetKernelArg(kernel, 7, sizeof(cl_float), &cos);

    // Define an index space (global work size) of work 
    // items for execution. A workgroup size (local work size) 
    // is not required, but can be used.
    size_t globalWorkSize[1];   
 
    // There are 'elements' work-items 
    globalWorkSize[0] = width*height;

    // Execute the kernel for execution
    status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, 
        globalWorkSize, NULL, 0, NULL, NULL);

    // Read the device output buffer to the host output array
    clEnqueueReadBuffer(cmdQueue, bufOutput, CL_TRUE, 0, 
        dataSize, outputImg, 0, NULL, NULL);

    // 
    storeImage(outputImg, outputFile, height, width, outputFile);

    // Free OpenCL resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufInput);
    clReleaseMemObject(bufOutput);
    clReleaseContext(context);

    // Free host resources
    free(imgToProcess);
    free(outputImg);
    free(platforms);
    free(devices);

    return 0;
}

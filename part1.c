// System includes
#include <stdio.h>
#include <stdlib.h>
#include "adcUtilsOpenCL.h"
#include <math.h>
#include <string.h>

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


int main(int argc, char *argv[]) {
    // This code executes on the OpenCL host

    // check arguments
    if(argc != 4) {
        printf("Error de argumentos\n");
        return -1;
    }

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "Y") != 0 && strcmp(argv[i], "N") != 0) {
            printf("Error de argumentos\n");
            return -1;
        }       
    }

    int actions[3];
    actions[0] = (strcmp(argv[1],"Y") == 0) ? 1 : 0; // rotate image
    actions[1] = (strcmp(argv[2],"Y") == 0) ? 1 : 0; // flip vertically image
    actions[2] = (strcmp(argv[3],"Y") == 0) ? 1 : 0; // flip horizontally image

    float *metadata = NULL;
    int numMetadata;

    // 
    if(actions[0]) {
        int argNum = 0;
        float angle;
        int ox, oy;

        do {
            printf("Ingrese\n");
            argNum = scanf("%f %d %d", &angle, &ox, &oy);
        } while (argNum != 3);
        
        numMetadata = 6;
        metadata = (float*)malloc(sizeof(float) * numMetadata);
        
        printf("angle es: %.6f\n", angle);
        metadata[2] = ox;
        metadata[3] = oy;
        metadata[4] = getCos(angle);
        metadata[5] = getSin(angle);
    } else {
        numMetadata = 2;
        metadata = (float*)malloc(sizeof(float) * numMetadata);
    }


    int width, height;

    const char* inputFile = "input.bmp";
    const char* outputFile = "output.bmp";

    // Read image from file
    float *imgToProcess = readImage(inputFile, &width, &height);

    metadata[0] = width;
    metadata[1] = height;

    for (int i = 0; i < numMetadata; ++i)
    {
        printf("Metadata[%d] es: %f\n", i, metadata[i]);
    }

    int dataSize = height*width*sizeof(float);
    int metadataSize = numMetadata*sizeof(float);

    float *outputImg = NULL;
    outputImg = (float*)malloc(dataSize);
    for(int i = 0; i < height*width; i++)
        outputImg[i] = 0.0;

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

    // Create a buffer object that will contain the image metadata
    cl_mem bufMetadata;
    bufMetadata = clCreateBuffer(context, CL_MEM_READ_ONLY, metadataSize,
       NULL, &status);

    // Create a buffer object that will hold the output image
    cl_mem bufOutput;
    bufOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, dataSize,
        NULL, &status);

    // Write input image to the device buffer bufInput
    status = clEnqueueWriteBuffer(cmdQueue, bufInput, CL_TRUE,
        0, dataSize, imgToProcess, 0, NULL, NULL);

    // Write metadata to the device buffer bufMetadata
    status = clEnqueueWriteBuffer(cmdQueue, bufMetadata, CL_TRUE,
        0, metadataSize, metadata, 0, NULL, NULL);

    // for(i = 3; i >= 0 ; i--) {
        // if(actions[i]) {
        //     if(i == 0) {
        //         size_t globalWorkSize[2];

        //         // There are 'elements' work-items
        //         globalWorkSize[0] = width;
        //         globalWorkSize[1] = height;
        //     }
        // }
    // }

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
    kernel = clCreateKernel(program, "program", &status);
    // check_error(status, "clEnqueueNDRangeKernel");

    // Associate the input and output buffers with the kernel
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufOutput);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufInput);
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufMetadata);

    // Define an index space (global work size) of work
    // items for execution. A workgroup size (local work size)
    // is not required, but can be used.
    size_t globalWorkSize[2];

    // There are 'elements' work-items
    globalWorkSize[0] = width;
    globalWorkSize[1] = height;

    // Execute the kernel for execution
    status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL,
        globalWorkSize, NULL, 0, NULL, NULL);

    // Read the device output buffer to the host output array
    clEnqueueReadBuffer(cmdQueue, bufOutput, CL_TRUE, 0,
        dataSize, outputImg, 0, NULL, NULL);

    //
    storeImage(outputImg, outputFile, height, width, inputFile);

    // Free OpenCL resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufInput);
    clReleaseMemObject(bufMetadata);    
    clReleaseMemObject(bufOutput);
    clReleaseContext(context);

    // Free host resources
    free(imgToProcess);
    free(metadata);
    free(outputImg);
    free(platforms);
    free(devices);

    return 0;
}

// System includes
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <stdio.h>
#include <stdlib.h>

// OpenCL includes
#include <CL/cl.h>
#include "adcUtilsOpenCL.h"
#include "printInfo.h"


char *programFile = "program.cl";
const char* programSource = readSource(programFile);

void showMatrix(double *m, int n) {
    for (int fila = 0; fila < n; ++fila) {
        for (int col = 0; col < n; ++col)
            printf("%.2f ", m[fila*n+col]);
        printf("\n");
    }
}

int main(){
    // This code executes on the OpenCL host

    // Host data
    int N, cant_iter, cant_fuentes;
    double *A = NULL;  // Input matrix
    double *B = NULL;  // Input matrix
    double *C = NULL;  // Output matrix
    double *temp = NULL;

    printf("Ingrese el tamaño de la grilla: ");    
    scanf("%d", &N);

    printf("Ingrese la cantidad de iteraciones: ");    
    scanf("%d", &cant_iter);
    
    printf("Ingrese la cantidad de fuentes: ");    
    scanf("%d", &cant_fuentes);

    // Elements in each array
    const int elements = N*N;
    // Compute the size of the data
    size_t datasize = sizeof(double)*elements;
    size_t datasizeN = sizeof(int);

    // Allocate space for input/output data
    A = (double*)malloc(datasize);
    B = (double*)malloc(datasize);
    C = (double*)malloc(datasize);

    // Initialize the input data
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            A[i*N +j] = 0;
            B[i*N +j] = 0;
        }
    }

    for(int i = 0; i < cant_fuentes; i++) {
        printf("Introduzca fuente %d (fila col temp): ", i);
        int col, fil;
        double t;
        scanf("%d %d %lf", &fil, &col, &t);
        A[N*fil+col] = t;
        B[N*fil+col] = 1;
    }

    printf("El input del usuario fue:\n");
    printf("La placa tiene tamaño %d. Tiene %d iteraciones y %d fuentes\n", N, cant_iter, cant_fuentes);

    printf("La matriz de fuentes es: \n");
    showMatrix(A, N);

    printf("La matriz inicial es: \n");
    showMatrix(B, N);

    // Use this to check the output of each API call
    cl_int status;  

    // Platforms begin
    // Retrieve the number of platforms
    cl_uint numPlatforms = 0;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
 
    // Allocate enough space for each platform
    cl_platform_id *platforms = NULL;
    platforms = (cl_platform_id*)malloc(
        numPlatforms*sizeof(cl_platform_id));
 
    // Fill in the platforms
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);

    printPlatformInfo(platforms, numPlatforms);

    // Retrieve the number of devices
    cl_uint numDevices = 0;
    // selecciona la plataforma 1
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, 
        NULL, &numDevices);

    // Allocate enough space for each device
    cl_device_id *devices;
    devices = (cl_device_id*)malloc(
        numDevices*sizeof(cl_device_id));

    // Fill in the devices 
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL,        
        numDevices, devices, NULL);

    // Choose best device
    cl_device_id device;
    int *performances = (int*)calloc(numDevices, sizeof(int));
    for (int i = 0; i < numDevices; i++)
        performances[i] = printDeviceInfo(devices[i], numDevices);

    printf("\n");

    int maxPerformance = 0;
    for (unsigned int i = 1; i < numDevices - 1; i++)
        if (performances[maxPerformance] > performances[i])
            maxPerformance = i;

    device = devices[maxPerformance];

    // Create a context and associate it with the devices
    cl_context context;
    context = clCreateContext(NULL, numDevices, devices, NULL, 
        NULL, &status);

    // Create a command queue and associate it with the device 
    cl_command_queue cmdQueue;
    cmdQueue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, 
        &status);

    // Create a buffer object that will contain the data 
    // from the host matrix A
    cl_mem bufA;
    bufA = clCreateBuffer(context, CL_MEM_READ_WRITE, datasize,                       
       NULL, &status);

    // Create a buffer object that will contain the data 
    // from the host matrix B
    cl_mem bufB;
    bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize,                        
        NULL, &status);

    // Create a buffer object that will hold the output data
    cl_mem bufC;
    bufC = clCreateBuffer(context, CL_MEM_READ_WRITE, datasize,
        NULL, &status);

     // Write input matrix A to the device buffer bufferA
    status = clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 
        0, datasize, A, 0, NULL, NULL);
    
    // Write input matrix B to the device buffer bufferB
    status = clEnqueueWriteBuffer(cmdQueue, bufB, CL_FALSE, 
        0, datasize, B, 0, NULL, NULL);

    // Create a program with source code
    cl_program program = clCreateProgramWithSource(context, 1,
        (const char**)&programSource, NULL, &status);

    // Build (compile) the program for the device
    status = clBuildProgram(program, numDevices, devices, 
        NULL, NULL, NULL);

    if(status == CL_BUILD_PROGRAM_FAILURE){
        size_t len = 0;
        cl_int ret = CL_SUCCESS;
        ret = clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        char *buffer = (char*)calloc(len, sizeof(char));
        ret = clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
        printf("Error de compilacion: %s",buffer);
    }

    // Create the matrix multiplication kernel
    cl_kernel kernel;
    kernel = clCreateKernel(program, "matrixMult", &status);

    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);
    status = clSetKernelArg(kernel, 3, sizeof(int), &N);
    status = clSetKernelArg(kernel, 4, sizeof(int), &cant_iter);

    // Define an index space (global work size) of work 
    // items for execution. A workgroup size (local work size) 
    // is not required, but can be used.
    size_t globalWorkSize[2] = { N, N };   
    size_t localWorkSize[2] = { N, N };
 
    cl_event event;
    // Execute the kernel for execution
    status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL, 
        globalWorkSize, localWorkSize, 0, NULL, &event);

    //Ensure kernel execution is finished
    clWaitForEvents(1, &event);

    cl_ulong time_start, time_end;
    double total_time;

    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), 
        &time_start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), 
        &time_end, NULL);
    total_time = time_end - time_start;
    printf("\n Execution time in milliseconds = %0.3f ms\n", total_time / 1000000.0);

    // Read the device output buffer to the host output matrix
    if(cant_iter % 2 == 0) {
        clEnqueueReadBuffer(cmdQueue, bufA, CL_TRUE, 0, 
            datasize, C, 0, NULL, NULL);
    } else {
        clEnqueueReadBuffer(cmdQueue, bufC, CL_TRUE, 0, 
            datasize, C, 0, NULL, NULL);
    }
    // Print the ouptut
    printf("Output:\n");
    showMatrix(C, N);

    // Free OpenCL resources
    clReleaseKernel(kernel);
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseContext(context);

    // Free host resources
    free(A);
    free(B);
    free(C);
    free(platforms);
    free(devices);

    return 0;
}


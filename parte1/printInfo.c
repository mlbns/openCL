#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

void printPlatformInfo(cl_platform_id *platforms, cl_uint numPlatforms){
    cl_int status;
    char *param_value = (char*) malloc(1000*sizeof(char));

    printf("\n");
    printf("PLATFORM INFO:\n");

    for (uint i = 0; i < numPlatforms; ++i)
    {
        status = clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(char)*1000, param_value, NULL);
        printf("Plataform Profile: %s\n",param_value);

        status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(char)*1000, param_value, NULL);
        printf("Plataform Version: %s\n",param_value);

        status = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(char)*1000, param_value, NULL);
        printf("Plataform Name: %s\n",param_value);

        status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(char)*1000, param_value, NULL);
        printf("Plataform Vendor: %s\n",param_value);

        status = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS , sizeof(char)*1000, param_value, NULL);
        printf("Plataform Extensions: %s\n",param_value);
    }
    free(param_value);
}

cl_uint printDeviceInfo(cl_device_id device, cl_uint numDevices){
    cl_int status;
    cl_uint *param_value_uint = (cl_uint*)malloc(sizeof(cl_uint)*1000);
    cl_uint ndimensions;
    cl_uint performance;

    printf("\n");
    printf("DEVICES INFO:\n");

    status = clGetDeviceInfo(device, CL_DEVICE_VENDOR_ID, sizeof(cl_uint)*1000, param_value_uint, NULL);
    printf("Device Vendor Id: %d\n", *param_value_uint);

    status = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint)*1000, param_value_uint, NULL);
    printf("Device Max Compute Units: %d\n", *param_value_uint);

    performance = *param_value_uint;

    status = clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint)*1000, param_value_uint, NULL);
    printf("Device Max Clock Frequency: %d\n", *param_value_uint);

    performance *= (*param_value_uint);

    status = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint)*1000, param_value_uint, NULL);
    printf("Device Max Work Item Dimensions: %d\n", *param_value_uint);
    ndimensions = *param_value_uint;
    for(int i = 0; i < ndimensions; i++){
        clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(cl_uint)*1000, param_value_uint, NULL);
        printf("\tDevice Max Work Item Sizes: %d\n", param_value_uint[i]);
    }
    char* param_value_char = (char*)malloc(sizeof(char)*1000);
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(char)*1000, param_value_char, NULL);
    printf("Device Name: %s\n", param_value_char);

    clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(char)*1000, param_value_char, NULL);
    printf("Device Vendor: %s\n", param_value_char);

    clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(char)*1000, param_value_char, NULL);
    printf("Driver Version: %s\n", param_value_char);

    clGetDeviceInfo(device, CL_DEVICE_PROFILE, sizeof(char)*1000, param_value_char, NULL);
    printf("Device Profile: %s\n", param_value_char);

    clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(char)*1000, param_value_char, NULL);
    printf("Device Version: %s\n", param_value_char);

    clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, sizeof(char)*1000, param_value_char, NULL);
    printf("Device OpenCL C Version: %s\n", param_value_char);

    return performance;
}

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#ifndef __ERROR__
#define __ERROR__

// This function is used in check_error
//const char *getErrorString (cl_int error);
//This function return a string associated with the error code. Usage:
//check_error (status, "clGetPlatformIds");
// error: Error code
// const char* name : Name of the function associated with the generated error code. 
void check_error (cl_int error, const char* name);
#endif

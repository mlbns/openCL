__kernel
void matrixMult(__global double *A,
                __global double *B,
                __global double *C,
                int N, int cant_iter) {

   __global double *temp;


   // Get the work-item’s unique ID
   int idx = get_global_id(0);
   int idy = get_global_id(1);

for( int i = 0 ; i < cant_iter; i++){

   // Multiplicar y guardar el resultado en C
   double sum = A[idx*N + idy];
   int cant_elementos = 1;
   if(B[idx*N + idy] != 1) {

       if (idx > 0) {
           cant_elementos+=1;
           sum += A[(idx-1)*N + idy];
       }
       if (idx < N-1) {
           cant_elementos+=1;
           sum += A[(idx+1)*N + idy];
       }
       if (idy > 0) {
           cant_elementos+=1;
           sum += A[idx*N + idy-1];
       }
       if (idy < N-1) {
           cant_elementos+=1;
           sum += A[idx*N + idy+1];
       }
   }
   C[idx*N+idy] = (double)sum/cant_elementos;


     temp = C;
     C = A;
     A = temp;

   barrier(CLK_GLOBAL_MEM_FENCE);
 }
}


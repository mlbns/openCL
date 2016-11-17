__kernel
void rotateImg(__global float* dest_data,
                __global float* src_data,
                int ox, int oy,
                int W, int H,                   //Image Dimensions
                float sinTheta, float cosTheta) //Rotation Parameters
{
    //Work-item gets its index within index space
    const int ix = get_global_id(0);
    const int iy = get_global_id(1);

    //Calculate location of data to move into (ix,iy)
    //Output decomposition as mentioned

    float xpos = ((float)ix - (float)ox)*cosTheta - ((float)iy - (float)oy)*sinTheta + (float)ox;
    float ypos = ((float)ix - (float)ox)*sinTheta + ((float)iy - (float)oy)*cosTheta + (float)oy;

    //Bound Checking
    if(((int)xpos>=0) && ((int)xpos< W) && ((int)ypos>=0) && ((int)ypos< H))
    {
        // Read (ix,iy) src_data and store at (xpos,ypos) in
        // dest_data
        // In this case, because we rotating about the origin
        // and there is no translation, we know that (xpos,ypos)
        // will be unique for each input (ix,iy) and so each
        // work-item can write its results independently
        dest_data[(int)ypos*W+(int)xpos] = src_data[iy*W+ix];
    }
}
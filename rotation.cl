__kernel
void rotateImg(__global float* dest_data,
                __global float* src_data,
                __global float* metaData)
{

    const int W = metaData[0];
    const int H = metaData[1];
    const float ox = metaData[2];
    const float oy = metaData[3];
    const float cosTheta = metaData[4];
    const float sinTheta = metaData[5];

    const int ix = get_global_id(0);
    const int iy = get_global_id(1);

    float xpos = ((float)ix - ox)*cosTheta - ((float)iy - oy)*sinTheta + ox;
    float ypos = ((float)ix - ox)*sinTheta + ((float)iy - oy)*cosTheta + oy;

    if(((int)xpos>=0) && ((int)xpos< W) && ((int)ypos>=0) && ((int)ypos< H))
    {
        dest_data[iy*W+ix] = src_data[(int)ypos*W+(int)xpos];
    } else {
        dest_data[iy*W+ix] = 0;
    }
}
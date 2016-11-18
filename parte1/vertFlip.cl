__kernel
void vertFlip(__global float* dest_data,
                __global float* src_data,
                __global float* metaData)
{

    const int W = metaData[0];
    const int H = metaData[1];

    const int ix = get_global_id(0);
    const int iy = get_global_id(1);

    int ypos = H - iy - 1;

    dest_data[iy*W+ix] = src_data[ypos*W+ix];
    dest_data[ypos*W+ix] = src_data[iy*W+ix];
}